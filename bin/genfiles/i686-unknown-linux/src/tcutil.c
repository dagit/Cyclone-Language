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
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_exists_c(
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
_temp1273; void* _temp1275; struct Cyc_Absyn_Tuniondecl** _temp1277; struct Cyc_Absyn_Tuniondecl*
_temp1279; void* _temp1280; struct Cyc_Absyn_TunionInfo _temp1282; void*
_temp1284; struct Cyc_List_List* _temp1286; void* _temp1288; struct Cyc_Absyn_Tuniondecl**
_temp1290; struct Cyc_Absyn_Tuniondecl* _temp1292; void* _temp1293; struct Cyc_Absyn_TunionFieldInfo
_temp1295; struct Cyc_List_List* _temp1297; void* _temp1299; struct Cyc_Absyn_Tunionfield*
_temp1301; struct Cyc_Absyn_Tuniondecl* _temp1303; void* _temp1305; struct Cyc_Absyn_TunionFieldInfo
_temp1307; struct Cyc_List_List* _temp1309; void* _temp1311; struct Cyc_Absyn_Tunionfield*
_temp1313; struct Cyc_Absyn_Tuniondecl* _temp1315; void* _temp1317; struct Cyc_Absyn_PtrInfo
_temp1319; struct Cyc_Absyn_Conref* _temp1321; struct Cyc_Absyn_Tqual _temp1323;
struct Cyc_Absyn_Conref* _temp1325; void* _temp1327; void* _temp1329; void*
_temp1331; struct Cyc_Absyn_PtrInfo _temp1333; struct Cyc_Absyn_Conref*
_temp1335; struct Cyc_Absyn_Tqual _temp1337; struct Cyc_Absyn_Conref* _temp1339;
void* _temp1341; void* _temp1343; void* _temp1345; void* _temp1347; void*
_temp1349; void* _temp1351; void* _temp1353; void* _temp1355; void* _temp1357;
void* _temp1359; void* _temp1361; void* _temp1363; void* _temp1365; struct Cyc_Absyn_Exp*
_temp1367; struct Cyc_Absyn_Tqual _temp1369; void* _temp1371; void* _temp1373;
struct Cyc_Absyn_Exp* _temp1375; struct Cyc_Absyn_Tqual _temp1377; void*
_temp1379; void* _temp1381; struct Cyc_Absyn_FnInfo _temp1383; struct Cyc_List_List*
_temp1385; struct Cyc_List_List* _temp1387; struct Cyc_Absyn_VarargInfo*
_temp1389; int _temp1391; struct Cyc_List_List* _temp1393; void* _temp1395;
struct Cyc_Core_Opt* _temp1397; struct Cyc_List_List* _temp1399; void* _temp1401;
struct Cyc_Absyn_FnInfo _temp1403; struct Cyc_List_List* _temp1405; struct Cyc_List_List*
_temp1407; struct Cyc_Absyn_VarargInfo* _temp1409; int _temp1411; struct Cyc_List_List*
_temp1413; void* _temp1415; struct Cyc_Core_Opt* _temp1417; struct Cyc_List_List*
_temp1419; void* _temp1421; struct Cyc_List_List* _temp1423; void* _temp1425;
struct Cyc_List_List* _temp1427; void* _temp1429; struct Cyc_List_List*
_temp1431; void* _temp1433; struct Cyc_List_List* _temp1435; void* _temp1437;
struct Cyc_List_List* _temp1439; void* _temp1441; struct Cyc_List_List*
_temp1443; void* _temp1445; void* _temp1447; void* _temp1449; void* _temp1451;
void* _temp1453; void* _temp1455; void* _temp1457; void* _temp1459; void*
_temp1461; void* _temp1463; void* _temp1465; void* _temp1467; _LL1161: _LL1214:
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
_temp1251)->f3; goto _LL1172;} else{ goto _LL1173;} _LL1173: _LL1281: _temp1280=
_temp1159.f1; if(( unsigned int) _temp1280 >  4u?*(( int*) _temp1280) ==  Cyc_Absyn_TunionType:
0){ _LL1283: _temp1282=(( struct Cyc_Absyn_TunionType_struct*) _temp1280)->f1;
_LL1289: _temp1288=( void*) _temp1282.tunion_info; if(*(( int*) _temp1288) == 
Cyc_Absyn_KnownTunion){ _LL1291: _temp1290=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1288)->f1; _temp1292=* _temp1290; goto _LL1287;} else{ goto _LL1175;}
_LL1287: _temp1286= _temp1282.targs; goto _LL1285; _LL1285: _temp1284=( void*)
_temp1282.rgn; goto _LL1268;} else{ goto _LL1175;} _LL1268: _temp1267= _temp1159.f2;
if(( unsigned int) _temp1267 >  4u?*(( int*) _temp1267) ==  Cyc_Absyn_TunionType:
0){ _LL1270: _temp1269=(( struct Cyc_Absyn_TunionType_struct*) _temp1267)->f1;
_LL1276: _temp1275=( void*) _temp1269.tunion_info; if(*(( int*) _temp1275) == 
Cyc_Absyn_KnownTunion){ _LL1278: _temp1277=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1275)->f1; _temp1279=* _temp1277; goto _LL1274;} else{ goto _LL1175;}
_LL1274: _temp1273= _temp1269.targs; goto _LL1272; _LL1272: _temp1271=( void*)
_temp1269.rgn; goto _LL1174;} else{ goto _LL1175;} _LL1175: _LL1306: _temp1305=
_temp1159.f1; if(( unsigned int) _temp1305 >  4u?*(( int*) _temp1305) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1308: _temp1307=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1305)->f1;
_LL1312: _temp1311=( void*) _temp1307.field_info; if(*(( int*) _temp1311) == 
Cyc_Absyn_KnownTunionfield){ _LL1316: _temp1315=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1311)->f1; goto _LL1314; _LL1314: _temp1313=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1311)->f2; goto _LL1310;} else{ goto _LL1177;} _LL1310: _temp1309=
_temp1307.targs; goto _LL1294;} else{ goto _LL1177;} _LL1294: _temp1293=
_temp1159.f2; if(( unsigned int) _temp1293 >  4u?*(( int*) _temp1293) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1296: _temp1295=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1293)->f1;
_LL1300: _temp1299=( void*) _temp1295.field_info; if(*(( int*) _temp1299) == 
Cyc_Absyn_KnownTunionfield){ _LL1304: _temp1303=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1299)->f1; goto _LL1302; _LL1302: _temp1301=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1299)->f2; goto _LL1298;} else{ goto _LL1177;} _LL1298: _temp1297=
_temp1295.targs; goto _LL1176;} else{ goto _LL1177;} _LL1177: _LL1332: _temp1331=
_temp1159.f1; if(( unsigned int) _temp1331 >  4u?*(( int*) _temp1331) ==  Cyc_Absyn_PointerType:
0){ _LL1334: _temp1333=(( struct Cyc_Absyn_PointerType_struct*) _temp1331)->f1;
_LL1344: _temp1343=( void*) _temp1333.elt_typ; goto _LL1342; _LL1342: _temp1341=(
void*) _temp1333.rgn_typ; goto _LL1340; _LL1340: _temp1339= _temp1333.nullable;
goto _LL1338; _LL1338: _temp1337= _temp1333.tq; goto _LL1336; _LL1336: _temp1335=
_temp1333.bounds; goto _LL1318;} else{ goto _LL1179;} _LL1318: _temp1317=
_temp1159.f2; if(( unsigned int) _temp1317 >  4u?*(( int*) _temp1317) ==  Cyc_Absyn_PointerType:
0){ _LL1320: _temp1319=(( struct Cyc_Absyn_PointerType_struct*) _temp1317)->f1;
_LL1330: _temp1329=( void*) _temp1319.elt_typ; goto _LL1328; _LL1328: _temp1327=(
void*) _temp1319.rgn_typ; goto _LL1326; _LL1326: _temp1325= _temp1319.nullable;
goto _LL1324; _LL1324: _temp1323= _temp1319.tq; goto _LL1322; _LL1322: _temp1321=
_temp1319.bounds; goto _LL1178;} else{ goto _LL1179;} _LL1179: _LL1352:
_temp1351= _temp1159.f1; if(( unsigned int) _temp1351 >  4u?*(( int*) _temp1351)
==  Cyc_Absyn_IntType: 0){ _LL1356: _temp1355=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1351)->f1; goto _LL1354; _LL1354: _temp1353=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1351)->f2; goto _LL1346;} else{ goto _LL1181;} _LL1346: _temp1345=
_temp1159.f2; if(( unsigned int) _temp1345 >  4u?*(( int*) _temp1345) ==  Cyc_Absyn_IntType:
0){ _LL1350: _temp1349=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1345)->f1;
goto _LL1348; _LL1348: _temp1347=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1345)->f2; goto _LL1180;} else{ goto _LL1181;} _LL1181: _LL1360: _temp1359=
_temp1159.f1; if( _temp1359 == ( void*) Cyc_Absyn_FloatType){ goto _LL1358;}
else{ goto _LL1183;} _LL1358: _temp1357= _temp1159.f2; if( _temp1357 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1182;} else{ goto _LL1183;} _LL1183: _LL1364:
_temp1363= _temp1159.f1; if( _temp1363 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1362;} else{ goto _LL1185;} _LL1362: _temp1361= _temp1159.f2; if( _temp1361
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1184;} else{ goto _LL1185;} _LL1185:
_LL1374: _temp1373= _temp1159.f1; if(( unsigned int) _temp1373 >  4u?*(( int*)
_temp1373) ==  Cyc_Absyn_ArrayType: 0){ _LL1380: _temp1379=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1373)->f1; goto _LL1378; _LL1378: _temp1377=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1373)->f2; goto _LL1376; _LL1376: _temp1375=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1373)->f3; goto _LL1366;} else{ goto _LL1187;} _LL1366: _temp1365=
_temp1159.f2; if(( unsigned int) _temp1365 >  4u?*(( int*) _temp1365) ==  Cyc_Absyn_ArrayType:
0){ _LL1372: _temp1371=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1365)->f1;
goto _LL1370; _LL1370: _temp1369=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1365)->f2; goto _LL1368; _LL1368: _temp1367=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1365)->f3; goto _LL1186;} else{ goto _LL1187;} _LL1187: _LL1402: _temp1401=
_temp1159.f1; if(( unsigned int) _temp1401 >  4u?*(( int*) _temp1401) ==  Cyc_Absyn_FnType:
0){ _LL1404: _temp1403=(( struct Cyc_Absyn_FnType_struct*) _temp1401)->f1;
_LL1420: _temp1419= _temp1403.tvars; goto _LL1418; _LL1418: _temp1417= _temp1403.effect;
goto _LL1416; _LL1416: _temp1415=( void*) _temp1403.ret_typ; goto _LL1414;
_LL1414: _temp1413= _temp1403.args; goto _LL1412; _LL1412: _temp1411= _temp1403.c_varargs;
goto _LL1410; _LL1410: _temp1409= _temp1403.cyc_varargs; goto _LL1408; _LL1408:
_temp1407= _temp1403.rgn_po; goto _LL1406; _LL1406: _temp1405= _temp1403.attributes;
goto _LL1382;} else{ goto _LL1189;} _LL1382: _temp1381= _temp1159.f2; if((
unsigned int) _temp1381 >  4u?*(( int*) _temp1381) ==  Cyc_Absyn_FnType: 0){
_LL1384: _temp1383=(( struct Cyc_Absyn_FnType_struct*) _temp1381)->f1; _LL1400:
_temp1399= _temp1383.tvars; goto _LL1398; _LL1398: _temp1397= _temp1383.effect;
goto _LL1396; _LL1396: _temp1395=( void*) _temp1383.ret_typ; goto _LL1394;
_LL1394: _temp1393= _temp1383.args; goto _LL1392; _LL1392: _temp1391= _temp1383.c_varargs;
goto _LL1390; _LL1390: _temp1389= _temp1383.cyc_varargs; goto _LL1388; _LL1388:
_temp1387= _temp1383.rgn_po; goto _LL1386; _LL1386: _temp1385= _temp1383.attributes;
goto _LL1188;} else{ goto _LL1189;} _LL1189: _LL1426: _temp1425= _temp1159.f1;
if(( unsigned int) _temp1425 >  4u?*(( int*) _temp1425) ==  Cyc_Absyn_TupleType:
0){ _LL1428: _temp1427=(( struct Cyc_Absyn_TupleType_struct*) _temp1425)->f1;
goto _LL1422;} else{ goto _LL1191;} _LL1422: _temp1421= _temp1159.f2; if((
unsigned int) _temp1421 >  4u?*(( int*) _temp1421) ==  Cyc_Absyn_TupleType: 0){
_LL1424: _temp1423=(( struct Cyc_Absyn_TupleType_struct*) _temp1421)->f1; goto
_LL1190;} else{ goto _LL1191;} _LL1191: _LL1434: _temp1433= _temp1159.f1; if((
unsigned int) _temp1433 >  4u?*(( int*) _temp1433) ==  Cyc_Absyn_AnonStructType:
0){ _LL1436: _temp1435=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1433)->f1;
goto _LL1430;} else{ goto _LL1193;} _LL1430: _temp1429= _temp1159.f2; if((
unsigned int) _temp1429 >  4u?*(( int*) _temp1429) ==  Cyc_Absyn_AnonStructType:
0){ _LL1432: _temp1431=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1429)->f1;
goto _LL1192;} else{ goto _LL1193;} _LL1193: _LL1442: _temp1441= _temp1159.f1;
if(( unsigned int) _temp1441 >  4u?*(( int*) _temp1441) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1444: _temp1443=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1441)->f1;
goto _LL1438;} else{ goto _LL1195;} _LL1438: _temp1437= _temp1159.f2; if((
unsigned int) _temp1437 >  4u?*(( int*) _temp1437) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1440: _temp1439=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1437)->f1;
goto _LL1194;} else{ goto _LL1195;} _LL1195: _LL1448: _temp1447= _temp1159.f1;
if( _temp1447 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1446;} else{ goto _LL1197;}
_LL1446: _temp1445= _temp1159.f2; if( _temp1445 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1196;} else{ goto _LL1197;} _LL1197: _LL1454: _temp1453= _temp1159.f1;
if(( unsigned int) _temp1453 >  4u?*(( int*) _temp1453) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1456: _temp1455=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1453)->f1; goto _LL1450;} else{ goto _LL1199;} _LL1450: _temp1449=
_temp1159.f2; if(( unsigned int) _temp1449 >  4u?*(( int*) _temp1449) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1452: _temp1451=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1449)->f1; goto _LL1198;} else{ goto _LL1199;} _LL1199: _LL1458: _temp1457=
_temp1159.f1; if(( unsigned int) _temp1457 >  4u?*(( int*) _temp1457) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1200;} else{ goto _LL1201;} _LL1201: _LL1460: _temp1459= _temp1159.f2;
if(( unsigned int) _temp1459 >  4u?*(( int*) _temp1459) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1202;} else{ goto _LL1203;} _LL1203: _LL1462: _temp1461= _temp1159.f1;
if(( unsigned int) _temp1461 >  4u?*(( int*) _temp1461) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1204;} else{ goto _LL1205;} _LL1205: _LL1464: _temp1463= _temp1159.f1;
if(( unsigned int) _temp1463 >  4u?*(( int*) _temp1463) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1206;} else{ goto _LL1207;} _LL1207: _LL1466: _temp1465= _temp1159.f2;
if(( unsigned int) _temp1465 >  4u?*(( int*) _temp1465) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1208;} else{ goto _LL1209;} _LL1209: _LL1468: _temp1467= _temp1159.f2;
if(( unsigned int) _temp1467 >  4u?*(( int*) _temp1467) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1210;} else{ goto _LL1211;} _LL1211: goto _LL1212; _LL1162: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1164: return; _LL1166: { struct _tagged_arr* _temp1469=
_temp1225->name; struct _tagged_arr* _temp1470= _temp1221->name; int _temp1471=*((
int*) _check_null( _temp1225->identity)); int _temp1472=*(( int*) _check_null(
_temp1221->identity)); void* _temp1473= Cyc_Absyn_conref_val( _temp1225->kind);
void* _temp1474= Cyc_Absyn_conref_val( _temp1221->kind); if( _temp1472 == 
_temp1471? Cyc_Std_zstrptrcmp( _temp1469, _temp1470) ==  0: 0){ if( _temp1473 != 
_temp1474){({ struct Cyc_Std_String_pa_struct _temp1478; _temp1478.tag= Cyc_Std_String_pa;
_temp1478.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1474);{ struct
Cyc_Std_String_pa_struct _temp1477; _temp1477.tag= Cyc_Std_String_pa; _temp1477.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1473);{ struct Cyc_Std_String_pa_struct
_temp1476; _temp1476.tag= Cyc_Std_String_pa; _temp1476.f1=( struct _tagged_arr)*
_temp1469;{ void* _temp1475[ 3u]={& _temp1476,& _temp1477,& _temp1478};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1475, sizeof( void*), 3u));}}}});}
return;} goto _LL1160;} _LL1168: if((( _temp1233 !=  0? _temp1241 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1233),( struct _tuple1*) _check_null(
_temp1241)) ==  0: 0)? 1:( _temp1233 ==  0? _temp1241 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1231, _temp1239); return;} goto _LL1160; _LL1170: if( Cyc_Absyn_qvar_cmp(
_temp1249, _temp1245) ==  0){ return;} goto _LL1160; _LL1172: if((( _temp1257 != 
0? _temp1265 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1257),( struct _tuple1*) _check_null( _temp1265)) ==  0: 0)? 1:( _temp1257
==  0? _temp1265 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1255, _temp1263);
return;} goto _LL1160; _LL1174: if( _temp1292 ==  _temp1279? 1: Cyc_Absyn_qvar_cmp(
_temp1292->name, _temp1279->name) ==  0){ Cyc_Tcutil_unify_it( _temp1271,
_temp1284); Cyc_Tcutil_unify_list( _temp1273, _temp1286); return;} goto _LL1160;
_LL1176: if(( _temp1315 ==  _temp1303? 1: Cyc_Absyn_qvar_cmp( _temp1315->name,
_temp1303->name) ==  0)? _temp1313 ==  _temp1301? 1: Cyc_Absyn_qvar_cmp(
_temp1313->name, _temp1301->name) ==  0: 0){ Cyc_Tcutil_unify_list( _temp1297,
_temp1309); return;} goto _LL1160; _LL1178: Cyc_Tcutil_unify_it( _temp1329,
_temp1343); Cyc_Tcutil_unify_it( _temp1341, _temp1327); Cyc_Tcutil_unify_tqual(
_temp1323, _temp1337); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1321, _temp1335);{ void* _temp1479=( void*)( Cyc_Absyn_compress_conref(
_temp1321))->v; void* _temp1485; _LL1481: if(( unsigned int) _temp1479 >  1u?*((
int*) _temp1479) ==  Cyc_Absyn_Eq_constr: 0){ _LL1486: _temp1485=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1479)->f1; if( _temp1485 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1482;} else{ goto _LL1483;}} else{ goto _LL1483;}
_LL1483: goto _LL1484; _LL1482: return; _LL1484: goto _LL1480; _LL1480:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp, _temp1325, _temp1339); return;
_LL1180: if( _temp1349 ==  _temp1355? _temp1347 ==  _temp1353: 0){ return;} goto
_LL1160; _LL1182: return; _LL1184: return; _LL1186: Cyc_Tcutil_unify_it(
_temp1371, _temp1379); Cyc_Tcutil_unify_tqual( _temp1369, _temp1377); if(
_temp1375 ==  _temp1367){ return;} if( _temp1375 ==  0? 1: _temp1367 ==  0){
goto _LL1160;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1375)) ==  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1367))){ return;} goto _LL1160; _LL1188: { int done= 0;{
struct _RegionHandle _temp1487= _new_region(); struct _RegionHandle* rgn=&
_temp1487; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1399
!=  0) { if( _temp1419 ==  0){ break;} inst=({ struct Cyc_List_List* _temp1488=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1488->hd=( void*)({ struct _tuple5* _temp1489=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1489->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1419))->hd; _temp1489->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1490=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1490[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1491; _temp1491.tag= Cyc_Absyn_VarType; _temp1491.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1399))->hd;
_temp1491;}); _temp1490;}); _temp1489;}); _temp1488->tl= inst; _temp1488;});
_temp1399=(( struct Cyc_List_List*) _check_null( _temp1399))->tl; _temp1419=((
struct Cyc_List_List*) _check_null( _temp1419))->tl;} if( _temp1419 !=  0){
_npop_handler( 0u); goto _LL1160;} if( inst !=  0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1492=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1492[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1493; _temp1493.tag= Cyc_Absyn_FnType; _temp1493.f1=({ struct Cyc_Absyn_FnInfo
_temp1494; _temp1494.tvars= 0; _temp1494.effect= _temp1397; _temp1494.ret_typ=(
void*) _temp1395; _temp1494.args= _temp1393; _temp1494.c_varargs= _temp1391;
_temp1494.cyc_varargs= _temp1389; _temp1494.rgn_po= _temp1387; _temp1494.attributes=
_temp1385; _temp1494;}); _temp1493;}); _temp1492;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1495=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1495[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1496; _temp1496.tag= Cyc_Absyn_FnType; _temp1496.f1=({ struct Cyc_Absyn_FnInfo
_temp1497; _temp1497.tvars= 0; _temp1497.effect= _temp1417; _temp1497.ret_typ=(
void*) _temp1415; _temp1497.args= _temp1413; _temp1497.c_varargs= _temp1411;
_temp1497.cyc_varargs= _temp1409; _temp1497.rgn_po= _temp1407; _temp1497.attributes=
_temp1405; _temp1497;}); _temp1496;}); _temp1495;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1395, _temp1415); for( 0;
_temp1393 !=  0? _temp1413 !=  0: 0;( _temp1393=(( struct Cyc_List_List*)
_check_null( _temp1393))->tl, _temp1413=(( struct Cyc_List_List*) _check_null(
_temp1413))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1393))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1413))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1393))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1413))->hd)).f3);} if( _temp1393 !=  0?
1: _temp1413 !=  0){ goto _LL1160;} if( _temp1391 !=  _temp1411){ goto _LL1160;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1499=({ struct _tuple11 _temp1498;
_temp1498.f1= _temp1389; _temp1498.f2= _temp1409; _temp1498;}); struct Cyc_Absyn_VarargInfo*
_temp1509; struct Cyc_Absyn_VarargInfo* _temp1511; struct Cyc_Absyn_VarargInfo*
_temp1513; struct Cyc_Absyn_VarargInfo* _temp1515; struct Cyc_Absyn_VarargInfo*
_temp1517; struct Cyc_Absyn_VarargInfo _temp1519; int _temp1520; void* _temp1522;
struct Cyc_Absyn_Tqual _temp1524; struct Cyc_Core_Opt* _temp1526; struct Cyc_Absyn_VarargInfo*
_temp1528; struct Cyc_Absyn_VarargInfo _temp1530; int _temp1531; void* _temp1533;
struct Cyc_Absyn_Tqual _temp1535; struct Cyc_Core_Opt* _temp1537; _LL1501:
_LL1512: _temp1511= _temp1499.f1; if( _temp1511 ==  0){ goto _LL1510;} else{
goto _LL1503;} _LL1510: _temp1509= _temp1499.f2; if( _temp1509 ==  0){ goto
_LL1502;} else{ goto _LL1503;} _LL1503: _LL1514: _temp1513= _temp1499.f1; if(
_temp1513 ==  0){ goto _LL1504;} else{ goto _LL1505;} _LL1505: _LL1516:
_temp1515= _temp1499.f2; if( _temp1515 ==  0){ goto _LL1506;} else{ goto _LL1507;}
_LL1507: _LL1529: _temp1528= _temp1499.f1; if( _temp1528 ==  0){ goto _LL1500;}
else{ _temp1530=* _temp1528; _LL1538: _temp1537= _temp1530.name; goto _LL1536;
_LL1536: _temp1535= _temp1530.tq; goto _LL1534; _LL1534: _temp1533=( void*)
_temp1530.type; goto _LL1532; _LL1532: _temp1531= _temp1530.inject; goto _LL1518;}
_LL1518: _temp1517= _temp1499.f2; if( _temp1517 ==  0){ goto _LL1500;} else{
_temp1519=* _temp1517; _LL1527: _temp1526= _temp1519.name; goto _LL1525; _LL1525:
_temp1524= _temp1519.tq; goto _LL1523; _LL1523: _temp1522=( void*) _temp1519.type;
goto _LL1521; _LL1521: _temp1520= _temp1519.inject; goto _LL1508;} _LL1502: goto
_LL1500; _LL1504: goto _LL1506; _LL1506: bad_cyc_vararg= 1; goto _LL1500;
_LL1508: Cyc_Tcutil_unify_tqual( _temp1535, _temp1524); Cyc_Tcutil_unify_it(
_temp1533, _temp1522); if( _temp1531 !=  _temp1520){ bad_cyc_vararg= 1;} goto
_LL1500; _LL1500:;} if( bad_cyc_vararg){ goto _LL1160;}{ int bad_effect= 0;{
struct _tuple12 _temp1540=({ struct _tuple12 _temp1539; _temp1539.f1= _temp1397;
_temp1539.f2= _temp1417; _temp1539;}); struct Cyc_Core_Opt* _temp1550; struct
Cyc_Core_Opt* _temp1552; struct Cyc_Core_Opt* _temp1554; struct Cyc_Core_Opt*
_temp1556; _LL1542: _LL1553: _temp1552= _temp1540.f1; if( _temp1552 ==  0){ goto
_LL1551;} else{ goto _LL1544;} _LL1551: _temp1550= _temp1540.f2; if( _temp1550
==  0){ goto _LL1543;} else{ goto _LL1544;} _LL1544: _LL1555: _temp1554=
_temp1540.f1; if( _temp1554 ==  0){ goto _LL1545;} else{ goto _LL1546;} _LL1546:
_LL1557: _temp1556= _temp1540.f2; if( _temp1556 ==  0){ goto _LL1547;} else{
goto _LL1548;} _LL1548: goto _LL1549; _LL1543: goto _LL1541; _LL1545: goto
_LL1547; _LL1547: bad_effect= 1; goto _LL1541; _LL1549: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1397))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1417))->v); goto _LL1541; _LL1541:;} if( bad_effect){ goto
_LL1160;} if( ! Cyc_Tcutil_same_atts( _temp1405, _temp1385)){ goto _LL1160;} if(
! Cyc_Tcutil_same_rgn_po( _temp1407, _temp1387)){ goto _LL1160;} return;}}}
_LL1190: for( 0; _temp1423 !=  0? _temp1427 !=  0: 0;( _temp1423=(( struct Cyc_List_List*)
_check_null( _temp1423))->tl, _temp1427=(( struct Cyc_List_List*) _check_null(
_temp1427))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1423))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1427))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1423))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1427))->hd)).f2);} if( _temp1423 ==  0?
_temp1427 ==  0: 0){ return;} goto _LL1160; _LL1192: _temp1443= _temp1435;
_temp1439= _temp1431; goto _LL1194; _LL1194: for( 0; _temp1439 !=  0? _temp1443
!=  0: 0;( _temp1439=(( struct Cyc_List_List*) _check_null( _temp1439))->tl,
_temp1443=(( struct Cyc_List_List*) _check_null( _temp1443))->tl)){ struct Cyc_Absyn_Structfield*
_temp1558=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1439))->hd; struct Cyc_Absyn_Structfield* _temp1559=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1443))->hd; if( Cyc_Std_zstrptrcmp(
_temp1558->name, _temp1559->name) !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual( _temp1558->tq, _temp1559->tq); Cyc_Tcutil_unify_it((
void*) _temp1558->type,( void*) _temp1559->type); if((( ! Cyc_Tcutil_same_atts(
_temp1558->attributes, _temp1559->attributes)? 1:( _temp1558->width !=  0?
_temp1559->width ==  0: 0))? 1:( _temp1559->width !=  0? _temp1558->width ==  0:
0))? 1:(( _temp1558->width !=  0? _temp1559->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1558->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1559->width)): 0)){( int) _throw(( void*)
Cyc_Tcutil_Unify);}} if( _temp1439 ==  0? _temp1443 ==  0: 0){ return;} goto
_LL1160; _LL1196: return; _LL1198: Cyc_Tcutil_unify_it( _temp1455, _temp1451);
return; _LL1200: goto _LL1202; _LL1202: goto _LL1204; _LL1204: goto _LL1206;
_LL1206: goto _LL1208; _LL1208: goto _LL1210; _LL1210: if( Cyc_Tcutil_unify_effect(
t1, t2)){ return;} else{ goto _LL1160;} _LL1212: goto _LL1160; _LL1160:;}( int)
_throw(( void*) Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp1560= Cyc_Tcutil_compress( t); _LL1562: if(( unsigned int) _temp1560
>  4u?*(( int*) _temp1560) ==  Cyc_Absyn_IntType: 0){ goto _LL1563;} else{ goto
_LL1564;} _LL1564: if( _temp1560 == ( void*) Cyc_Absyn_FloatType){ goto _LL1565;}
else{ goto _LL1566;} _LL1566: if( _temp1560 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1567;} else{ goto _LL1568;} _LL1568: if(( unsigned int) _temp1560 >  4u?*((
int*) _temp1560) ==  Cyc_Absyn_EnumType: 0){ goto _LL1569;} else{ goto _LL1570;}
_LL1570: goto _LL1571; _LL1563: goto _LL1565; _LL1565: goto _LL1567; _LL1567:
goto _LL1569; _LL1569: return 1; _LL1571: return 0; _LL1561:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1573=({ struct _tuple8 _temp1572; _temp1572.f1= t1;
_temp1572.f2= t2; _temp1572;}); void* _temp1595; void* _temp1597; void*
_temp1599; void* _temp1601; void* _temp1603; void* _temp1605; void* _temp1607;
void* _temp1609; void* _temp1611; void* _temp1613; void* _temp1615; void*
_temp1617; void* _temp1619; void* _temp1621; void* _temp1623; void* _temp1625;
void* _temp1627; void* _temp1629; void* _temp1631; void* _temp1633; void*
_temp1635; void* _temp1637; void* _temp1639; void* _temp1641; void* _temp1643;
void* _temp1645; void* _temp1647; _LL1575: _LL1598: _temp1597= _temp1573.f1; if(
_temp1597 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1596;} else{ goto _LL1577;}
_LL1596: _temp1595= _temp1573.f2; if( _temp1595 == ( void*) Cyc_Absyn_FloatType){
goto _LL1576;} else{ goto _LL1577;} _LL1577: _LL1602: _temp1601= _temp1573.f1;
if( _temp1601 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1600;} else{ goto
_LL1579;} _LL1600: _temp1599= _temp1573.f2; if(( unsigned int) _temp1599 >  4u?*((
int*) _temp1599) ==  Cyc_Absyn_IntType: 0){ goto _LL1578;} else{ goto _LL1579;}
_LL1579: _LL1606: _temp1605= _temp1573.f1; if( _temp1605 == ( void*) Cyc_Absyn_FloatType){
goto _LL1604;} else{ goto _LL1581;} _LL1604: _temp1603= _temp1573.f2; if((
unsigned int) _temp1603 >  4u?*(( int*) _temp1603) ==  Cyc_Absyn_IntType: 0){
goto _LL1580;} else{ goto _LL1581;} _LL1581: _LL1612: _temp1611= _temp1573.f1;
if(( unsigned int) _temp1611 >  4u?*(( int*) _temp1611) ==  Cyc_Absyn_IntType: 0){
_LL1614: _temp1613=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1611)->f2;
if( _temp1613 == ( void*) Cyc_Absyn_B8){ goto _LL1608;} else{ goto _LL1583;}}
else{ goto _LL1583;} _LL1608: _temp1607= _temp1573.f2; if(( unsigned int)
_temp1607 >  4u?*(( int*) _temp1607) ==  Cyc_Absyn_IntType: 0){ _LL1610:
_temp1609=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1607)->f2; if(
_temp1609 == ( void*) Cyc_Absyn_B8){ goto _LL1582;} else{ goto _LL1583;}} else{
goto _LL1583;} _LL1583: _LL1616: _temp1615= _temp1573.f1; if(( unsigned int)
_temp1615 >  4u?*(( int*) _temp1615) ==  Cyc_Absyn_IntType: 0){ _LL1618:
_temp1617=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1615)->f2; if(
_temp1617 == ( void*) Cyc_Absyn_B8){ goto _LL1584;} else{ goto _LL1585;}} else{
goto _LL1585;} _LL1585: _LL1622: _temp1621= _temp1573.f1; if(( unsigned int)
_temp1621 >  4u?*(( int*) _temp1621) ==  Cyc_Absyn_IntType: 0){ _LL1624:
_temp1623=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1621)->f2; if(
_temp1623 == ( void*) Cyc_Absyn_B4){ goto _LL1620;} else{ goto _LL1587;}} else{
goto _LL1587;} _LL1620: _temp1619= _temp1573.f2; if( _temp1619 == ( void*) Cyc_Absyn_FloatType){
goto _LL1586;} else{ goto _LL1587;} _LL1587: _LL1630: _temp1629= _temp1573.f1;
if(( unsigned int) _temp1629 >  4u?*(( int*) _temp1629) ==  Cyc_Absyn_IntType: 0){
_LL1632: _temp1631=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1629)->f2;
if( _temp1631 == ( void*) Cyc_Absyn_B4){ goto _LL1626;} else{ goto _LL1589;}}
else{ goto _LL1589;} _LL1626: _temp1625= _temp1573.f2; if(( unsigned int)
_temp1625 >  4u?*(( int*) _temp1625) ==  Cyc_Absyn_IntType: 0){ _LL1628:
_temp1627=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1625)->f2; if(
_temp1627 == ( void*) Cyc_Absyn_B2){ goto _LL1588;} else{ goto _LL1589;}} else{
goto _LL1589;} _LL1589: _LL1638: _temp1637= _temp1573.f1; if(( unsigned int)
_temp1637 >  4u?*(( int*) _temp1637) ==  Cyc_Absyn_IntType: 0){ _LL1640:
_temp1639=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1637)->f2; if(
_temp1639 == ( void*) Cyc_Absyn_B4){ goto _LL1634;} else{ goto _LL1591;}} else{
goto _LL1591;} _LL1634: _temp1633= _temp1573.f2; if(( unsigned int) _temp1633 > 
4u?*(( int*) _temp1633) ==  Cyc_Absyn_IntType: 0){ _LL1636: _temp1635=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1633)->f2; if( _temp1635 == ( void*) Cyc_Absyn_B1){
goto _LL1590;} else{ goto _LL1591;}} else{ goto _LL1591;} _LL1591: _LL1646:
_temp1645= _temp1573.f1; if(( unsigned int) _temp1645 >  4u?*(( int*) _temp1645)
==  Cyc_Absyn_IntType: 0){ _LL1648: _temp1647=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1645)->f2; if( _temp1647 == ( void*) Cyc_Absyn_B2){ goto _LL1642;} else{
goto _LL1593;}} else{ goto _LL1593;} _LL1642: _temp1641= _temp1573.f2; if((
unsigned int) _temp1641 >  4u?*(( int*) _temp1641) ==  Cyc_Absyn_IntType: 0){
_LL1644: _temp1643=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1641)->f2;
if( _temp1643 == ( void*) Cyc_Absyn_B1){ goto _LL1592;} else{ goto _LL1593;}}
else{ goto _LL1593;} _LL1593: goto _LL1594; _LL1576: goto _LL1578; _LL1578: goto
_LL1580; _LL1580: return 1; _LL1582: return 0; _LL1584: goto _LL1586; _LL1586:
goto _LL1588; _LL1588: goto _LL1590; _LL1590: goto _LL1592; _LL1592: return 1;
_LL1594: return 0; _LL1574:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1649=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1649->v=( void*)
t1; _temp1649;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify( t,(
void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp1652; _temp1652.tag= Cyc_Std_String_pa; _temp1652.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1651; _temp1651.tag= Cyc_Std_String_pa;
_temp1651.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1650[
2u]={& _temp1651,& _temp1652}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1650, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1653= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1655: if(( unsigned int) _temp1653 >  4u?*(( int*) _temp1653) ==  Cyc_Absyn_PointerType:
0){ goto _LL1656;} else{ goto _LL1657;} _LL1657: goto _LL1658; _LL1656: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1654; _LL1658: return 0; _LL1654:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1659= Cyc_Tcutil_compress(
t); _LL1661: if(( unsigned int) _temp1659 >  4u?*(( int*) _temp1659) ==  Cyc_Absyn_IntType:
0){ goto _LL1662;} else{ goto _LL1663;} _LL1663: if(( unsigned int) _temp1659 > 
4u?*(( int*) _temp1659) ==  Cyc_Absyn_EnumType: 0){ goto _LL1664;} else{ goto
_LL1665;} _LL1665: goto _LL1666; _LL1662: goto _LL1664; _LL1664: return 1;
_LL1666: return 0; _LL1660:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1667[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1667, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1668[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1668, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1670=({ struct _tuple8 _temp1669; _temp1669.f1= t1; _temp1669.f2= t2;
_temp1669;}); void* _temp1682; struct Cyc_Absyn_PtrInfo _temp1684; void*
_temp1686; struct Cyc_Absyn_PtrInfo _temp1688; void* _temp1690; struct Cyc_Absyn_Exp*
_temp1692; struct Cyc_Absyn_Tqual _temp1694; void* _temp1696; void* _temp1698;
struct Cyc_Absyn_Exp* _temp1700; struct Cyc_Absyn_Tqual _temp1702; void*
_temp1704; void* _temp1706; struct Cyc_Absyn_TunionInfo _temp1708; struct Cyc_List_List*
_temp1710; void* _temp1712; struct Cyc_Absyn_Tuniondecl** _temp1714; struct Cyc_Absyn_Tuniondecl*
_temp1716; void* _temp1717; struct Cyc_Absyn_TunionFieldInfo _temp1719; struct
Cyc_List_List* _temp1721; void* _temp1723; struct Cyc_Absyn_Tunionfield*
_temp1725; struct Cyc_Absyn_Tuniondecl* _temp1727; void* _temp1729; struct Cyc_Absyn_TunionInfo
_temp1731; void* _temp1733; struct Cyc_List_List* _temp1735; void* _temp1737;
struct Cyc_Absyn_Tuniondecl** _temp1739; struct Cyc_Absyn_Tuniondecl* _temp1741;
void* _temp1742; struct Cyc_Absyn_PtrInfo _temp1744; struct Cyc_Absyn_Conref*
_temp1746; struct Cyc_Absyn_Tqual _temp1748; struct Cyc_Absyn_Conref* _temp1750;
void* _temp1752; void* _temp1754; _LL1672: _LL1687: _temp1686= _temp1670.f1; if((
unsigned int) _temp1686 >  4u?*(( int*) _temp1686) ==  Cyc_Absyn_PointerType: 0){
_LL1689: _temp1688=(( struct Cyc_Absyn_PointerType_struct*) _temp1686)->f1; goto
_LL1683;} else{ goto _LL1674;} _LL1683: _temp1682= _temp1670.f2; if((
unsigned int) _temp1682 >  4u?*(( int*) _temp1682) ==  Cyc_Absyn_PointerType: 0){
_LL1685: _temp1684=(( struct Cyc_Absyn_PointerType_struct*) _temp1682)->f1; goto
_LL1673;} else{ goto _LL1674;} _LL1674: _LL1699: _temp1698= _temp1670.f1; if((
unsigned int) _temp1698 >  4u?*(( int*) _temp1698) ==  Cyc_Absyn_ArrayType: 0){
_LL1705: _temp1704=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1698)->f1;
goto _LL1703; _LL1703: _temp1702=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1698)->f2; goto _LL1701; _LL1701: _temp1700=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1698)->f3; goto _LL1691;} else{ goto _LL1676;} _LL1691: _temp1690=
_temp1670.f2; if(( unsigned int) _temp1690 >  4u?*(( int*) _temp1690) ==  Cyc_Absyn_ArrayType:
0){ _LL1697: _temp1696=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1690)->f1;
goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1690)->f2; goto _LL1693; _LL1693: _temp1692=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1690)->f3; goto _LL1675;} else{ goto _LL1676;} _LL1676: _LL1718: _temp1717=
_temp1670.f1; if(( unsigned int) _temp1717 >  4u?*(( int*) _temp1717) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1720: _temp1719=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1717)->f1;
_LL1724: _temp1723=( void*) _temp1719.field_info; if(*(( int*) _temp1723) == 
Cyc_Absyn_KnownTunionfield){ _LL1728: _temp1727=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1723)->f1; goto _LL1726; _LL1726: _temp1725=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1723)->f2; goto _LL1722;} else{ goto _LL1678;} _LL1722: _temp1721=
_temp1719.targs; goto _LL1707;} else{ goto _LL1678;} _LL1707: _temp1706=
_temp1670.f2; if(( unsigned int) _temp1706 >  4u?*(( int*) _temp1706) ==  Cyc_Absyn_TunionType:
0){ _LL1709: _temp1708=(( struct Cyc_Absyn_TunionType_struct*) _temp1706)->f1;
_LL1713: _temp1712=( void*) _temp1708.tunion_info; if(*(( int*) _temp1712) == 
Cyc_Absyn_KnownTunion){ _LL1715: _temp1714=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1712)->f1; _temp1716=* _temp1714; goto _LL1711;} else{ goto _LL1678;}
_LL1711: _temp1710= _temp1708.targs; goto _LL1677;} else{ goto _LL1678;} _LL1678:
_LL1743: _temp1742= _temp1670.f1; if(( unsigned int) _temp1742 >  4u?*(( int*)
_temp1742) ==  Cyc_Absyn_PointerType: 0){ _LL1745: _temp1744=(( struct Cyc_Absyn_PointerType_struct*)
_temp1742)->f1; _LL1755: _temp1754=( void*) _temp1744.elt_typ; goto _LL1753;
_LL1753: _temp1752=( void*) _temp1744.rgn_typ; goto _LL1751; _LL1751: _temp1750=
_temp1744.nullable; goto _LL1749; _LL1749: _temp1748= _temp1744.tq; goto _LL1747;
_LL1747: _temp1746= _temp1744.bounds; goto _LL1730;} else{ goto _LL1680;}
_LL1730: _temp1729= _temp1670.f2; if(( unsigned int) _temp1729 >  4u?*(( int*)
_temp1729) ==  Cyc_Absyn_TunionType: 0){ _LL1732: _temp1731=(( struct Cyc_Absyn_TunionType_struct*)
_temp1729)->f1; _LL1738: _temp1737=( void*) _temp1731.tunion_info; if(*(( int*)
_temp1737) ==  Cyc_Absyn_KnownTunion){ _LL1740: _temp1739=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1737)->f1; _temp1741=* _temp1739; goto _LL1736;} else{ goto _LL1680;}
_LL1736: _temp1735= _temp1731.targs; goto _LL1734; _LL1734: _temp1733=( void*)
_temp1731.rgn; goto _LL1679;} else{ goto _LL1680;} _LL1680: goto _LL1681;
_LL1673: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1688.nullable, _temp1684.nullable)){ void* _temp1756=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1688.nullable))->v;
int _temp1762; _LL1758: if(( unsigned int) _temp1756 >  1u?*(( int*) _temp1756)
==  Cyc_Absyn_Eq_constr: 0){ _LL1763: _temp1762=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1756)->f1; goto _LL1759;} else{ goto _LL1760;} _LL1760: goto _LL1761;
_LL1759: okay= ! _temp1762; goto _LL1757; _LL1761:( int) _throw(({ void*
_temp1764[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp1764, sizeof( void*), 0u));}));
_LL1757:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1688.bounds,
_temp1684.bounds)){ struct _tuple8 _temp1766=({ struct _tuple8 _temp1765;
_temp1765.f1=( void*)( Cyc_Absyn_compress_conref( _temp1688.bounds))->v;
_temp1765.f2=( void*)( Cyc_Absyn_compress_conref( _temp1684.bounds))->v;
_temp1765;}); void* _temp1776; void* _temp1778; void* _temp1780; void* _temp1782;
void* _temp1784; void* _temp1786; struct Cyc_Absyn_Exp* _temp1788; void*
_temp1790; void* _temp1792; struct Cyc_Absyn_Exp* _temp1794; void* _temp1796;
void* _temp1798; void* _temp1800; void* _temp1802; _LL1768: _LL1781: _temp1780=
_temp1766.f1; if(( unsigned int) _temp1780 >  1u?*(( int*) _temp1780) ==  Cyc_Absyn_Eq_constr:
0){ _LL1783: _temp1782=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1780)->f1;
if(( unsigned int) _temp1782 >  1u?*(( int*) _temp1782) ==  Cyc_Absyn_Upper_b: 0){
goto _LL1777;} else{ goto _LL1770;}} else{ goto _LL1770;} _LL1777: _temp1776=
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
okay= okay? Cyc_Tcutil_unify(( void*) _temp1688.elt_typ,( void*) _temp1684.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1688.rgn_typ,( void*) _temp1684.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1688.rgn_typ,( void*) _temp1684.rgn_typ):
0; okay= okay? !( _temp1688.tq).q_const? 1:( _temp1684.tq).q_const: 0; return
okay;} _LL1675: { int okay; okay=( _temp1700 !=  0? _temp1692 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1700)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1692)): 0; return( okay? Cyc_Tcutil_unify(
_temp1704, _temp1696): 0)? ! _temp1702.q_const? 1: _temp1694.q_const: 0;}
_LL1677: if(( _temp1727 ==  _temp1716? 1: Cyc_Absyn_qvar_cmp( _temp1727->name,
_temp1716->name) ==  0)? _temp1725->typs ==  0: 0){ for( 0; _temp1721 !=  0?
_temp1710 !=  0: 0;( _temp1721=(( struct Cyc_List_List*) _check_null( _temp1721))->tl,
_temp1710=(( struct Cyc_List_List*) _check_null( _temp1710))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1721))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1710))->hd)){ break;}} if( _temp1721 ==  0?
_temp1710 ==  0: 0){ return 1;}} return 0; _LL1679:{ void* _temp1805= Cyc_Tcutil_compress(
_temp1754); struct Cyc_Absyn_TunionFieldInfo _temp1811; struct Cyc_List_List*
_temp1813; void* _temp1815; struct Cyc_Absyn_Tunionfield* _temp1817; struct Cyc_Absyn_Tuniondecl*
_temp1819; _LL1807: if(( unsigned int) _temp1805 >  4u?*(( int*) _temp1805) == 
Cyc_Absyn_TunionFieldType: 0){ _LL1812: _temp1811=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1805)->f1; _LL1816: _temp1815=( void*) _temp1811.field_info; if(*(( int*)
_temp1815) ==  Cyc_Absyn_KnownTunionfield){ _LL1820: _temp1819=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1815)->f1; goto _LL1818; _LL1818: _temp1817=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1815)->f2; goto _LL1814;} else{ goto _LL1809;} _LL1814: _temp1813=
_temp1811.targs; goto _LL1808;} else{ goto _LL1809;} _LL1809: goto _LL1810;
_LL1808: if( ! Cyc_Tcutil_unify( _temp1752, _temp1733)? ! Cyc_Tcenv_region_outlives(
te, _temp1752, _temp1733): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1750,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1746,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1741->name, _temp1819->name) ==  0? _temp1817->typs !=  0: 0){ int okay= 1;
for( 0; _temp1813 !=  0? _temp1735 !=  0: 0;( _temp1813=(( struct Cyc_List_List*)
_check_null( _temp1813))->tl, _temp1735=(( struct Cyc_List_List*) _check_null(
_temp1735))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1813))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1735))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1813 !=  0)? 1:
_temp1735 !=  0){ return 0;} return 1;} goto _LL1806; _LL1810: goto _LL1806;
_LL1806:;} return 0; _LL1681: return Cyc_Tcutil_unify( t1, t2); _LL1671:;}} int
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
te, e, t);} int Cyc_Tcutil_coerceable( void* t){ void* _temp1825= Cyc_Tcutil_compress(
t); _LL1827: if(( unsigned int) _temp1825 >  4u?*(( int*) _temp1825) ==  Cyc_Absyn_IntType:
0){ goto _LL1828;} else{ goto _LL1829;} _LL1829: if( _temp1825 == ( void*) Cyc_Absyn_FloatType){
goto _LL1830;} else{ goto _LL1831;} _LL1831: if( _temp1825 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1832;} else{ goto _LL1833;} _LL1833: goto _LL1834; _LL1828: goto _LL1830;
_LL1830: goto _LL1832; _LL1832: return 1; _LL1834: return 0; _LL1826:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1835=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp1835->f1= x->tq; _temp1835->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1835;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1836= t1; struct Cyc_List_List* _temp1848; struct Cyc_Absyn_Structdecl**
_temp1850; struct Cyc_List_List* _temp1852; struct _tuple1* _temp1854; struct
Cyc_List_List* _temp1856; _LL1838: if( _temp1836 == ( void*) Cyc_Absyn_VoidType){
goto _LL1839;} else{ goto _LL1840;} _LL1840: if(( unsigned int) _temp1836 >  4u?*((
int*) _temp1836) ==  Cyc_Absyn_TupleType: 0){ _LL1849: _temp1848=(( struct Cyc_Absyn_TupleType_struct*)
_temp1836)->f1; goto _LL1841;} else{ goto _LL1842;} _LL1842: if(( unsigned int)
_temp1836 >  4u?*(( int*) _temp1836) ==  Cyc_Absyn_StructType: 0){ _LL1855:
_temp1854=(( struct Cyc_Absyn_StructType_struct*) _temp1836)->f1; goto _LL1853;
_LL1853: _temp1852=(( struct Cyc_Absyn_StructType_struct*) _temp1836)->f2; goto
_LL1851; _LL1851: _temp1850=(( struct Cyc_Absyn_StructType_struct*) _temp1836)->f3;
goto _LL1843;} else{ goto _LL1844;} _LL1844: if(( unsigned int) _temp1836 >  4u?*((
int*) _temp1836) ==  Cyc_Absyn_AnonStructType: 0){ _LL1857: _temp1856=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1836)->f1; goto _LL1845;} else{ goto
_LL1846;} _LL1846: goto _LL1847; _LL1839: return 0; _LL1841: return _temp1848;
_LL1843: if( _temp1850 ==  0? 1: _temp1854 ==  0){ return({ struct Cyc_List_List*
_temp1858=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1858->hd=( void*)({ struct _tuple4* _temp1859=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1859->f1= Cyc_Absyn_empty_tqual(); _temp1859->f2=
t1; _temp1859;}); _temp1858->tl= 0; _temp1858;});}{ struct Cyc_Absyn_Structdecl*
_temp1860=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1850)); struct
_tuple1 _temp1861=*(( struct _tuple1*) _check_null( _temp1854)); struct Cyc_List_List*
_temp1862=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1860->tvs, _temp1852); if( _temp1860->fields ==  0){
return({ struct Cyc_List_List* _temp1863=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1863->hd=( void*)({ struct _tuple4*
_temp1864=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1864->f1=
Cyc_Absyn_empty_tqual(); _temp1864->f2= t1; _temp1864;}); _temp1863->tl= 0;
_temp1863;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1862,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1860->fields))->v);}
_LL1845: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1856); _LL1847: return({
struct Cyc_List_List* _temp1865=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1865->hd=( void*)({ struct _tuple4* _temp1866=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1866->f1= Cyc_Absyn_empty_tqual();
_temp1866->f2= t1; _temp1866;}); _temp1865->tl= 0; _temp1865;}); _LL1837:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1868=({ struct _tuple8 _temp1867; _temp1867.f1= t1; _temp1867.f2=
t2; _temp1867;}); void* _temp1874; struct Cyc_Absyn_PtrInfo _temp1876; struct
Cyc_Absyn_Conref* _temp1878; struct Cyc_Absyn_Tqual _temp1880; struct Cyc_Absyn_Conref*
_temp1882; void* _temp1884; void* _temp1886; void* _temp1888; struct Cyc_Absyn_PtrInfo
_temp1890; struct Cyc_Absyn_Conref* _temp1892; struct Cyc_Absyn_Tqual _temp1894;
struct Cyc_Absyn_Conref* _temp1896; void* _temp1898; void* _temp1900; _LL1870:
_LL1889: _temp1888= _temp1868.f1; if(( unsigned int) _temp1888 >  4u?*(( int*)
_temp1888) ==  Cyc_Absyn_PointerType: 0){ _LL1891: _temp1890=(( struct Cyc_Absyn_PointerType_struct*)
_temp1888)->f1; _LL1901: _temp1900=( void*) _temp1890.elt_typ; goto _LL1899;
_LL1899: _temp1898=( void*) _temp1890.rgn_typ; goto _LL1897; _LL1897: _temp1896=
_temp1890.nullable; goto _LL1895; _LL1895: _temp1894= _temp1890.tq; goto _LL1893;
_LL1893: _temp1892= _temp1890.bounds; goto _LL1875;} else{ goto _LL1872;}
_LL1875: _temp1874= _temp1868.f2; if(( unsigned int) _temp1874 >  4u?*(( int*)
_temp1874) ==  Cyc_Absyn_PointerType: 0){ _LL1877: _temp1876=(( struct Cyc_Absyn_PointerType_struct*)
_temp1874)->f1; _LL1887: _temp1886=( void*) _temp1876.elt_typ; goto _LL1885;
_LL1885: _temp1884=( void*) _temp1876.rgn_typ; goto _LL1883; _LL1883: _temp1882=
_temp1876.nullable; goto _LL1881; _LL1881: _temp1880= _temp1876.tq; goto _LL1879;
_LL1879: _temp1878= _temp1876.bounds; goto _LL1871;} else{ goto _LL1872;}
_LL1872: goto _LL1873; _LL1871: if( _temp1894.q_const? ! _temp1880.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1896, _temp1882)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1896): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1882): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1898, _temp1884)? ! Cyc_Tcenv_region_outlives(
te, _temp1898, _temp1884): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1892, _temp1878)){ struct _tuple8 _temp1903=({ struct _tuple8 _temp1902;
_temp1902.f1= Cyc_Absyn_conref_val( _temp1892); _temp1902.f2= Cyc_Absyn_conref_val(
_temp1878); _temp1902;}); void* _temp1911; void* _temp1913; void* _temp1915;
struct Cyc_Absyn_Exp* _temp1917; void* _temp1919; struct Cyc_Absyn_Exp*
_temp1921; _LL1905: _LL1914: _temp1913= _temp1903.f1; if(( unsigned int)
_temp1913 >  1u?*(( int*) _temp1913) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1912;}
else{ goto _LL1907;} _LL1912: _temp1911= _temp1903.f2; if( _temp1911 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1906;} else{ goto _LL1907;} _LL1907: _LL1920:
_temp1919= _temp1903.f1; if(( unsigned int) _temp1919 >  1u?*(( int*) _temp1919)
==  Cyc_Absyn_Upper_b: 0){ _LL1922: _temp1921=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1919)->f1; goto _LL1916;} else{ goto _LL1909;} _LL1916: _temp1915=
_temp1903.f2; if(( unsigned int) _temp1915 >  1u?*(( int*) _temp1915) ==  Cyc_Absyn_Upper_b:
0){ _LL1918: _temp1917=(( struct Cyc_Absyn_Upper_b_struct*) _temp1915)->f1; goto
_LL1908;} else{ goto _LL1909;} _LL1909: goto _LL1910; _LL1906: goto _LL1904;
_LL1908: if( Cyc_Evexp_eval_const_uint_exp( _temp1921) <  Cyc_Evexp_eval_const_uint_exp(
_temp1917)){ return 0;} goto _LL1904; _LL1910: return 0; _LL1904:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1923=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1923->hd=( void*)({ struct _tuple8*
_temp1924=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1924->f1=
t1; _temp1924->f2= t2; _temp1924;}); _temp1923->tl= assume; _temp1923;}),
_temp1900, _temp1886); _LL1873: return 0; _LL1869:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1926=({ struct _tuple8 _temp1925;
_temp1925.f1= Cyc_Tcutil_compress( t1); _temp1925.f2= Cyc_Tcutil_compress( t2);
_temp1925;}); void* _temp1932; void* _temp1934; void* _temp1936; void* _temp1938;
_LL1928: _LL1937: _temp1936= _temp1926.f1; if(( unsigned int) _temp1936 >  4u?*((
int*) _temp1936) ==  Cyc_Absyn_IntType: 0){ _LL1939: _temp1938=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1936)->f2; goto _LL1933;} else{ goto _LL1930;}
_LL1933: _temp1932= _temp1926.f2; if(( unsigned int) _temp1932 >  4u?*(( int*)
_temp1932) ==  Cyc_Absyn_IntType: 0){ _LL1935: _temp1934=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1932)->f2; goto _LL1929;} else{ goto _LL1930;} _LL1930: goto _LL1931;
_LL1929: return _temp1938 ==  _temp1934; _LL1931: return 0; _LL1927:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=((
struct Cyc_List_List*) _check_null( tqs1))->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1942; void* _temp1943; struct Cyc_Absyn_Tqual _temp1945;
struct _tuple4* _temp1940=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs1))->hd; _temp1942=* _temp1940; _LL1946: _temp1945= _temp1942.f1;
goto _LL1944; _LL1944: _temp1943= _temp1942.f2; goto _LL1941; _LL1941: { struct
_tuple4 _temp1949; void* _temp1950; struct Cyc_Absyn_Tqual _temp1952; struct
_tuple4* _temp1947=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs2))->hd;
_temp1949=* _temp1947; _LL1953: _temp1952= _temp1949.f1; goto _LL1951; _LL1951:
_temp1950= _temp1949.f2; goto _LL1948; _LL1948: if( _temp1952.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1943, _temp1950): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1943, _temp1950)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1943,
_temp1950)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1954= t2; struct Cyc_Absyn_Uniondecl** _temp1962; struct Cyc_List_List*
_temp1964; void* _temp1966; _LL1956: if(( unsigned int) _temp1954 >  4u?*(( int*)
_temp1954) ==  Cyc_Absyn_UnionType: 0){ _LL1965: _temp1964=(( struct Cyc_Absyn_UnionType_struct*)
_temp1954)->f2; goto _LL1963; _LL1963: _temp1962=(( struct Cyc_Absyn_UnionType_struct*)
_temp1954)->f3; goto _LL1957;} else{ goto _LL1958;} _LL1958: if(( unsigned int)
_temp1954 >  4u?*(( int*) _temp1954) ==  Cyc_Absyn_IntType: 0){ _LL1967:
_temp1966=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1954)->f2; if(
_temp1966 == ( void*) Cyc_Absyn_B4){ goto _LL1959;} else{ goto _LL1960;}} else{
goto _LL1960;} _LL1960: goto _LL1961; _LL1957: if( _temp1962 ==  0){ goto
_LL1955;}{ struct Cyc_Absyn_Uniondecl* _temp1968=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1962)); if( _temp1968->fields ==  0){ goto _LL1955;}{ struct
Cyc_List_List* _temp1969=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1968->tvs, _temp1964);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1968->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp1969,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1955;}} _LL1959: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1955; _LL1961: goto _LL1955; _LL1955:;}{ void* _temp1970= t1;
struct Cyc_Absyn_PtrInfo _temp1988; struct Cyc_Absyn_Conref* _temp1990; struct
Cyc_Absyn_Tqual _temp1992; struct Cyc_Absyn_Conref* _temp1994; void* _temp1996;
void* _temp1998; struct Cyc_Absyn_Exp* _temp2000; struct Cyc_Absyn_Tqual
_temp2002; void* _temp2004; struct Cyc_Absyn_Enumdecl* _temp2006; _LL1972: if((
unsigned int) _temp1970 >  4u?*(( int*) _temp1970) ==  Cyc_Absyn_PointerType: 0){
_LL1989: _temp1988=(( struct Cyc_Absyn_PointerType_struct*) _temp1970)->f1;
_LL1999: _temp1998=( void*) _temp1988.elt_typ; goto _LL1997; _LL1997: _temp1996=(
void*) _temp1988.rgn_typ; goto _LL1995; _LL1995: _temp1994= _temp1988.nullable;
goto _LL1993; _LL1993: _temp1992= _temp1988.tq; goto _LL1991; _LL1991: _temp1990=
_temp1988.bounds; goto _LL1973;} else{ goto _LL1974;} _LL1974: if(( unsigned int)
_temp1970 >  4u?*(( int*) _temp1970) ==  Cyc_Absyn_ArrayType: 0){ _LL2005:
_temp2004=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1970)->f1; goto
_LL2003; _LL2003: _temp2002=(( struct Cyc_Absyn_ArrayType_struct*) _temp1970)->f2;
goto _LL2001; _LL2001: _temp2000=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1970)->f3; goto _LL1975;} else{ goto _LL1976;} _LL1976: if(( unsigned int)
_temp1970 >  4u?*(( int*) _temp1970) ==  Cyc_Absyn_EnumType: 0){ _LL2007:
_temp2006=(( struct Cyc_Absyn_EnumType_struct*) _temp1970)->f2; goto _LL1977;}
else{ goto _LL1978;} _LL1978: if(( unsigned int) _temp1970 >  4u?*(( int*)
_temp1970) ==  Cyc_Absyn_IntType: 0){ goto _LL1979;} else{ goto _LL1980;}
_LL1980: if( _temp1970 == ( void*) Cyc_Absyn_FloatType){ goto _LL1981;} else{
goto _LL1982;} _LL1982: if( _temp1970 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1983;} else{ goto _LL1984;} _LL1984: if( _temp1970 == ( void*) Cyc_Absyn_VoidType){
goto _LL1985;} else{ goto _LL1986;} _LL1986: goto _LL1987; _LL1973:{ void*
_temp2008= t2; struct Cyc_Absyn_PtrInfo _temp2016; struct Cyc_Absyn_Conref*
_temp2018; struct Cyc_Absyn_Tqual _temp2020; struct Cyc_Absyn_Conref* _temp2022;
void* _temp2024; void* _temp2026; _LL2010: if(( unsigned int) _temp2008 >  4u?*((
int*) _temp2008) ==  Cyc_Absyn_PointerType: 0){ _LL2017: _temp2016=(( struct Cyc_Absyn_PointerType_struct*)
_temp2008)->f1; _LL2027: _temp2026=( void*) _temp2016.elt_typ; goto _LL2025;
_LL2025: _temp2024=( void*) _temp2016.rgn_typ; goto _LL2023; _LL2023: _temp2022=
_temp2016.nullable; goto _LL2021; _LL2021: _temp2020= _temp2016.tq; goto _LL2019;
_LL2019: _temp2018= _temp2016.bounds; goto _LL2011;} else{ goto _LL2012;}
_LL2012: if( _temp2008 == ( void*) Cyc_Absyn_VoidType){ goto _LL2013;} else{
goto _LL2014;} _LL2014: goto _LL2015; _LL2011: { struct Cyc_List_List* _temp2028=({
struct Cyc_List_List* _temp2045=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2045->hd=( void*)({ struct _tuple8* _temp2046=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2046->f1= t1;
_temp2046->f2= t2; _temp2046;}); _temp2045->tl= 0; _temp2045;}); int _temp2029=
Cyc_Tcutil_ptrsubtype( te, _temp2028, _temp1998, _temp2026)? ! _temp1992.q_const?
1: _temp2020.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1990, _temp2018); if( ! bounds_ok){ struct _tuple8 _temp2031=({ struct
_tuple8 _temp2030; _temp2030.f1= Cyc_Absyn_conref_val( _temp1990); _temp2030.f2=
Cyc_Absyn_conref_val( _temp2018); _temp2030;}); void* _temp2037; struct Cyc_Absyn_Exp*
_temp2039; void* _temp2041; struct Cyc_Absyn_Exp* _temp2043; _LL2033: _LL2042:
_temp2041= _temp2031.f1; if(( unsigned int) _temp2041 >  1u?*(( int*) _temp2041)
==  Cyc_Absyn_Upper_b: 0){ _LL2044: _temp2043=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2041)->f1; goto _LL2038;} else{ goto _LL2035;} _LL2038: _temp2037=
_temp2031.f2; if(( unsigned int) _temp2037 >  1u?*(( int*) _temp2037) ==  Cyc_Absyn_Upper_b:
0){ _LL2040: _temp2039=(( struct Cyc_Absyn_Upper_b_struct*) _temp2037)->f1; goto
_LL2034;} else{ goto _LL2035;} _LL2035: goto _LL2036; _LL2034: if( Cyc_Evexp_eval_const_uint_exp(
_temp2043) >=  Cyc_Evexp_eval_const_uint_exp( _temp2039)){ bounds_ok= 1;} goto
_LL2032; _LL2036: bounds_ok= 1; goto _LL2032; _LL2032:;} return( bounds_ok?
_temp2029: 0)? Cyc_Tcutil_unify( _temp1996, _temp2024)? 1: Cyc_Tcenv_region_outlives(
te, _temp1996, _temp2024): 0;} _LL2013: return 1; _LL2015: goto _LL2009; _LL2009:;}
return 0; _LL1975:{ void* _temp2047= t2; struct Cyc_Absyn_Exp* _temp2053; struct
Cyc_Absyn_Tqual _temp2055; void* _temp2057; _LL2049: if(( unsigned int)
_temp2047 >  4u?*(( int*) _temp2047) ==  Cyc_Absyn_ArrayType: 0){ _LL2058:
_temp2057=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2047)->f1; goto
_LL2056; _LL2056: _temp2055=(( struct Cyc_Absyn_ArrayType_struct*) _temp2047)->f2;
goto _LL2054; _LL2054: _temp2053=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2047)->f3; goto _LL2050;} else{ goto _LL2051;} _LL2051: goto _LL2052;
_LL2050: { int okay; okay=( _temp2000 !=  0? _temp2053 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2000)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2053)): 0; return( okay? Cyc_Tcutil_unify(
_temp2004, _temp2057): 0)? ! _temp2002.q_const? 1: _temp2055.q_const: 0;}
_LL2052: return 0; _LL2048:;} return 0; _LL1977:{ void* _temp2059= t2; struct
Cyc_Absyn_Enumdecl* _temp2065; _LL2061: if(( unsigned int) _temp2059 >  4u?*((
int*) _temp2059) ==  Cyc_Absyn_EnumType: 0){ _LL2066: _temp2065=(( struct Cyc_Absyn_EnumType_struct*)
_temp2059)->f2; goto _LL2062;} else{ goto _LL2063;} _LL2063: goto _LL2064;
_LL2062: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2006))->fields != 
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2065))->fields !=  0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp2006))->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2065))->fields))->v): 0){ return 1;} goto _LL2060; _LL2064:
goto _LL2060; _LL2060:;} goto _LL1979; _LL1979: goto _LL1981; _LL1981: goto
_LL1983; _LL1983: return Cyc_Tcutil_coerceable( t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType;
_LL1985: return t2 == ( void*) Cyc_Absyn_VoidType; _LL1987: return 0; _LL1971:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2067= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2068=(
struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2068[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2069; _temp2069.tag= Cyc_Absyn_Cast_e;
_temp2069.f1=( void*) t; _temp2069.f2= _temp2067; _temp2069;}); _temp2068;})));
e->topt=({ struct Cyc_Core_Opt* _temp2070=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2070->v=( void*) t; _temp2070;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2071= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2073: if(( unsigned int) _temp2071
>  4u?*(( int*) _temp2071) ==  Cyc_Absyn_EnumType: 0){ goto _LL2074;} else{ goto
_LL2075;} _LL2075: if(( unsigned int) _temp2071 >  4u?*(( int*) _temp2071) == 
Cyc_Absyn_IntType: 0){ goto _LL2076;} else{ goto _LL2077;} _LL2077: if((
unsigned int) _temp2071 >  4u?*(( int*) _temp2071) ==  Cyc_Absyn_Evar: 0){ goto
_LL2078;} else{ goto _LL2079;} _LL2079: goto _LL2080; _LL2074: goto _LL2076;
_LL2076: return 1; _LL2078: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2080: return 0; _LL2072:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2081= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2083: if( _temp2081 == ( void*) Cyc_Absyn_FloatType){
goto _LL2084;} else{ goto _LL2085;} _LL2085: if( _temp2081 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2086;} else{ goto _LL2087;} _LL2087: goto _LL2088; _LL2084: goto _LL2086;
_LL2086: return 1; _LL2088: return 0; _LL2082:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp2089= Cyc_Tcutil_compress( t); _LL2091: if(( unsigned int)
_temp2089 >  4u?*(( int*) _temp2089) ==  Cyc_Absyn_FnType: 0){ goto _LL2092;}
else{ goto _LL2093;} _LL2093: goto _LL2094; _LL2092: return 1; _LL2094: return 0;
_LL2090:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp2096=({ struct _tuple8 _temp2095; _temp2095.f1= t1; _temp2095.f2=
t2; _temp2095;}); void* _temp2120; void* _temp2122; void* _temp2124; void*
_temp2126; void* _temp2128; void* _temp2130; void* _temp2132; void* _temp2134;
void* _temp2136; void* _temp2138; void* _temp2140; void* _temp2142; void*
_temp2144; void* _temp2146; void* _temp2148; void* _temp2150; void* _temp2152;
void* _temp2154; void* _temp2156; void* _temp2158; void* _temp2160; void*
_temp2162; _LL2098: _LL2121: _temp2120= _temp2096.f1; if( _temp2120 == ( void*)
Cyc_Absyn_DoubleType){ goto _LL2099;} else{ goto _LL2100;} _LL2100: _LL2123:
_temp2122= _temp2096.f2; if( _temp2122 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2101;} else{ goto _LL2102;} _LL2102: _LL2125: _temp2124= _temp2096.f1; if(
_temp2124 == ( void*) Cyc_Absyn_FloatType){ goto _LL2103;} else{ goto _LL2104;}
_LL2104: _LL2127: _temp2126= _temp2096.f2; if( _temp2126 == ( void*) Cyc_Absyn_FloatType){
goto _LL2105;} else{ goto _LL2106;} _LL2106: _LL2129: _temp2128= _temp2096.f1;
if(( unsigned int) _temp2128 >  4u?*(( int*) _temp2128) ==  Cyc_Absyn_IntType: 0){
_LL2133: _temp2132=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2128)->f1;
if( _temp2132 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2131;} else{ goto _LL2108;}
_LL2131: _temp2130=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2128)->f2;
if( _temp2130 == ( void*) Cyc_Absyn_B8){ goto _LL2107;} else{ goto _LL2108;}}
else{ goto _LL2108;} _LL2108: _LL2135: _temp2134= _temp2096.f2; if((
unsigned int) _temp2134 >  4u?*(( int*) _temp2134) ==  Cyc_Absyn_IntType: 0){
_LL2139: _temp2138=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2134)->f1;
if( _temp2138 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2137;} else{ goto _LL2110;}
_LL2137: _temp2136=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2134)->f2;
if( _temp2136 == ( void*) Cyc_Absyn_B8){ goto _LL2109;} else{ goto _LL2110;}}
else{ goto _LL2110;} _LL2110: _LL2141: _temp2140= _temp2096.f1; if((
unsigned int) _temp2140 >  4u?*(( int*) _temp2140) ==  Cyc_Absyn_IntType: 0){
_LL2145: _temp2144=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2140)->f1;
if( _temp2144 == ( void*) Cyc_Absyn_Signed){ goto _LL2143;} else{ goto _LL2112;}
_LL2143: _temp2142=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2140)->f2;
if( _temp2142 == ( void*) Cyc_Absyn_B8){ goto _LL2111;} else{ goto _LL2112;}}
else{ goto _LL2112;} _LL2112: _LL2147: _temp2146= _temp2096.f2; if((
unsigned int) _temp2146 >  4u?*(( int*) _temp2146) ==  Cyc_Absyn_IntType: 0){
_LL2151: _temp2150=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2146)->f1;
if( _temp2150 == ( void*) Cyc_Absyn_Signed){ goto _LL2149;} else{ goto _LL2114;}
_LL2149: _temp2148=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2146)->f2;
if( _temp2148 == ( void*) Cyc_Absyn_B8){ goto _LL2113;} else{ goto _LL2114;}}
else{ goto _LL2114;} _LL2114: _LL2153: _temp2152= _temp2096.f1; if((
unsigned int) _temp2152 >  4u?*(( int*) _temp2152) ==  Cyc_Absyn_IntType: 0){
_LL2157: _temp2156=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2152)->f1;
if( _temp2156 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2155;} else{ goto _LL2116;}
_LL2155: _temp2154=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2152)->f2;
if( _temp2154 == ( void*) Cyc_Absyn_B4){ goto _LL2115;} else{ goto _LL2116;}}
else{ goto _LL2116;} _LL2116: _LL2159: _temp2158= _temp2096.f2; if((
unsigned int) _temp2158 >  4u?*(( int*) _temp2158) ==  Cyc_Absyn_IntType: 0){
_LL2163: _temp2162=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2158)->f1;
if( _temp2162 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2161;} else{ goto _LL2118;}
_LL2161: _temp2160=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2158)->f2;
if( _temp2160 == ( void*) Cyc_Absyn_B4){ goto _LL2117;} else{ goto _LL2118;}}
else{ goto _LL2118;} _LL2118: goto _LL2119; _LL2099: goto _LL2101; _LL2101:
return( void*) Cyc_Absyn_DoubleType; _LL2103: goto _LL2105; _LL2105: return(
void*) Cyc_Absyn_FloatType; _LL2107: goto _LL2109; _LL2109: return Cyc_Absyn_ulonglong_t;
_LL2111: goto _LL2113; _LL2113: return Cyc_Absyn_slonglong_t; _LL2115: goto
_LL2117; _LL2117: return Cyc_Absyn_uint_t; _LL2119: return Cyc_Absyn_sint_t;
_LL2097:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2164=( void*) e->r; struct Cyc_Core_Opt* _temp2170; _LL2166: if(*(( int*)
_temp2164) ==  Cyc_Absyn_AssignOp_e){ _LL2171: _temp2170=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2164)->f2; if( _temp2170 ==  0){ goto _LL2167;} else{ goto _LL2168;}} else{
goto _LL2168;} _LL2168: goto _LL2169; _LL2167:({ void* _temp2172[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2172, sizeof( void*), 0u));}); goto _LL2165; _LL2169: goto _LL2165; _LL2165:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2173=(
struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2173[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2174; _temp2174.tag=
Cyc_Absyn_Forward_constr; _temp2174.f1= c2; _temp2174;}); _temp2173;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2175=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2175[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2176; _temp2176.tag= Cyc_Absyn_Forward_constr;
_temp2176.f1= c1; _temp2176;}); _temp2175;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2177=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2177[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2178; _temp2178.tag= Cyc_Absyn_Forward_constr; _temp2178.f1= c1; _temp2178;});
_temp2177;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2179=( struct Cyc_Absyn_Forward_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2179[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2180; _temp2180.tag= Cyc_Absyn_Forward_constr;
_temp2180.f1= c2; _temp2180;}); _temp2179;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2181=( int*) _cycalloc_atomic( sizeof( int)); _temp2181[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2181;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2185;
_temp2185.tag= Cyc_Std_Int_pa; _temp2185.f1=( int)(( unsigned int) i);{ void*
_temp2184[ 1u]={& _temp2185}; Cyc_Std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2184, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2182=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2182->name=({ struct _tagged_arr* _temp2183=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr) *  1); _temp2183[ 0]=
s; _temp2183;}); _temp2182->identity= 0; _temp2182->kind= Cyc_Absyn_new_conref(
k); _temp2182;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2186=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2186, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct Cyc_Std_String_pa_struct
_temp2188; _temp2188.tag= Cyc_Std_String_pa; _temp2188.f1=( struct _tagged_arr)*
t->name;{ void* _temp2187[ 1u]={& _temp2188}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp2187, sizeof( void*), 1u));}}); if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2189= Cyc_Std_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2189, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2190=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2190[ 0]=( struct _tagged_arr) _temp2189; _temp2190;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2191=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2191->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2192=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2192->v=( void*)(*
x).f1; _temp2192;}); _temp2191->f2=(* x).f2; _temp2191->f3=(* x).f3; _temp2191;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2193= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2193=({ struct Cyc_List_List* _temp2194=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2194->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2194->tl= _temp2193; _temp2194;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2195=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2195[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2196; _temp2196.tag= Cyc_Absyn_FnType; _temp2196.f1=({ struct Cyc_Absyn_FnInfo
_temp2197; _temp2197.tvars= fd->tvs; _temp2197.effect= fd->effect; _temp2197.ret_typ=(
void*)(( void*) fd->ret_type); _temp2197.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2197.c_varargs= fd->c_varargs;
_temp2197.cyc_varargs= fd->cyc_varargs; _temp2197.rgn_po= fd->rgn_po; _temp2197.attributes=
_temp2193; _temp2197;}); _temp2196;}); _temp2195;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2198=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp2198->f1=(* pr).f1; _temp2198->f2= t; _temp2198;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2199=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2199->f1=({ struct _tuple15* _temp2200=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2200->f1=(* y).f1; _temp2200->f2=(* y).f2;
_temp2200;}); _temp2199->f2=(* y).f3; _temp2199;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2203; struct _tuple15* _temp2205; struct
_tuple16 _temp2201=* w; _LL2206: _temp2205= _temp2201.f1; goto _LL2204; _LL2204:
_temp2203= _temp2201.f2; goto _LL2202; _LL2202: { struct Cyc_Absyn_Tqual
_temp2209; struct Cyc_Core_Opt* _temp2211; struct _tuple15 _temp2207=* _temp2205;
_LL2212: _temp2211= _temp2207.f1; goto _LL2210; _LL2210: _temp2209= _temp2207.f2;
goto _LL2208; _LL2208: return({ struct _tuple2* _temp2213=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2213->f1= _temp2211; _temp2213->f2=
_temp2209; _temp2213->f3= _temp2203; _temp2213;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2214=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2214->name= f->name;
_temp2214->tq= f->tq; _temp2214->type=( void*) t; _temp2214->width= f->width;
_temp2214->attributes= f->attributes; _temp2214;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2215= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2263; struct Cyc_Absyn_Structdecl** _temp2265;
struct Cyc_List_List* _temp2267; struct _tuple1* _temp2269; struct Cyc_Absyn_Uniondecl**
_temp2271; struct Cyc_List_List* _temp2273; struct _tuple1* _temp2275; struct
Cyc_Absyn_TunionInfo _temp2277; void* _temp2279; struct Cyc_List_List* _temp2281;
void* _temp2283; struct Cyc_Absyn_TunionFieldInfo _temp2285; struct Cyc_List_List*
_temp2287; void* _temp2289; struct Cyc_Core_Opt* _temp2291; struct Cyc_List_List*
_temp2293; struct _tuple1* _temp2295; struct Cyc_Absyn_Exp* _temp2297; struct
Cyc_Absyn_Tqual _temp2299; void* _temp2301; struct Cyc_Absyn_PtrInfo _temp2303;
struct Cyc_Absyn_Conref* _temp2305; struct Cyc_Absyn_Tqual _temp2307; struct Cyc_Absyn_Conref*
_temp2309; void* _temp2311; void* _temp2313; struct Cyc_Absyn_FnInfo _temp2315;
struct Cyc_List_List* _temp2317; struct Cyc_List_List* _temp2319; struct Cyc_Absyn_VarargInfo*
_temp2321; int _temp2323; struct Cyc_List_List* _temp2325; void* _temp2327;
struct Cyc_Core_Opt* _temp2329; struct Cyc_List_List* _temp2331; struct Cyc_List_List*
_temp2333; struct Cyc_List_List* _temp2335; struct Cyc_List_List* _temp2337;
struct Cyc_Core_Opt* _temp2339; void* _temp2341; void* _temp2343; void*
_temp2345; struct Cyc_List_List* _temp2347; _LL2217: if(( unsigned int)
_temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_VarType: 0){ _LL2264:
_temp2263=(( struct Cyc_Absyn_VarType_struct*) _temp2215)->f1; goto _LL2218;}
else{ goto _LL2219;} _LL2219: if(( unsigned int) _temp2215 >  4u?*(( int*)
_temp2215) ==  Cyc_Absyn_StructType: 0){ _LL2270: _temp2269=(( struct Cyc_Absyn_StructType_struct*)
_temp2215)->f1; goto _LL2268; _LL2268: _temp2267=(( struct Cyc_Absyn_StructType_struct*)
_temp2215)->f2; goto _LL2266; _LL2266: _temp2265=(( struct Cyc_Absyn_StructType_struct*)
_temp2215)->f3; goto _LL2220;} else{ goto _LL2221;} _LL2221: if(( unsigned int)
_temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_UnionType: 0){ _LL2276:
_temp2275=(( struct Cyc_Absyn_UnionType_struct*) _temp2215)->f1; goto _LL2274;
_LL2274: _temp2273=(( struct Cyc_Absyn_UnionType_struct*) _temp2215)->f2; goto
_LL2272; _LL2272: _temp2271=(( struct Cyc_Absyn_UnionType_struct*) _temp2215)->f3;
goto _LL2222;} else{ goto _LL2223;} _LL2223: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_TunionType: 0){ _LL2278: _temp2277=(( struct Cyc_Absyn_TunionType_struct*)
_temp2215)->f1; _LL2284: _temp2283=( void*) _temp2277.tunion_info; goto _LL2282;
_LL2282: _temp2281= _temp2277.targs; goto _LL2280; _LL2280: _temp2279=( void*)
_temp2277.rgn; goto _LL2224;} else{ goto _LL2225;} _LL2225: if(( unsigned int)
_temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2286:
_temp2285=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2215)->f1; _LL2290:
_temp2289=( void*) _temp2285.field_info; goto _LL2288; _LL2288: _temp2287=
_temp2285.targs; goto _LL2226;} else{ goto _LL2227;} _LL2227: if(( unsigned int)
_temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_TypedefType: 0){ _LL2296:
_temp2295=(( struct Cyc_Absyn_TypedefType_struct*) _temp2215)->f1; goto _LL2294;
_LL2294: _temp2293=(( struct Cyc_Absyn_TypedefType_struct*) _temp2215)->f2; goto
_LL2292; _LL2292: _temp2291=(( struct Cyc_Absyn_TypedefType_struct*) _temp2215)->f3;
goto _LL2228;} else{ goto _LL2229;} _LL2229: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_ArrayType: 0){ _LL2302: _temp2301=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2215)->f1; goto _LL2300; _LL2300:
_temp2299=(( struct Cyc_Absyn_ArrayType_struct*) _temp2215)->f2; goto _LL2298;
_LL2298: _temp2297=(( struct Cyc_Absyn_ArrayType_struct*) _temp2215)->f3; goto
_LL2230;} else{ goto _LL2231;} _LL2231: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_PointerType: 0){ _LL2304: _temp2303=(( struct Cyc_Absyn_PointerType_struct*)
_temp2215)->f1; _LL2314: _temp2313=( void*) _temp2303.elt_typ; goto _LL2312;
_LL2312: _temp2311=( void*) _temp2303.rgn_typ; goto _LL2310; _LL2310: _temp2309=
_temp2303.nullable; goto _LL2308; _LL2308: _temp2307= _temp2303.tq; goto _LL2306;
_LL2306: _temp2305= _temp2303.bounds; goto _LL2232;} else{ goto _LL2233;}
_LL2233: if(( unsigned int) _temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_FnType:
0){ _LL2316: _temp2315=(( struct Cyc_Absyn_FnType_struct*) _temp2215)->f1;
_LL2332: _temp2331= _temp2315.tvars; goto _LL2330; _LL2330: _temp2329= _temp2315.effect;
goto _LL2328; _LL2328: _temp2327=( void*) _temp2315.ret_typ; goto _LL2326;
_LL2326: _temp2325= _temp2315.args; goto _LL2324; _LL2324: _temp2323= _temp2315.c_varargs;
goto _LL2322; _LL2322: _temp2321= _temp2315.cyc_varargs; goto _LL2320; _LL2320:
_temp2319= _temp2315.rgn_po; goto _LL2318; _LL2318: _temp2317= _temp2315.attributes;
goto _LL2234;} else{ goto _LL2235;} _LL2235: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_TupleType: 0){ _LL2334: _temp2333=(( struct Cyc_Absyn_TupleType_struct*)
_temp2215)->f1; goto _LL2236;} else{ goto _LL2237;} _LL2237: if(( unsigned int)
_temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_AnonStructType: 0){ _LL2336:
_temp2335=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2215)->f1; goto
_LL2238;} else{ goto _LL2239;} _LL2239: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2338: _temp2337=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2215)->f1; goto _LL2240;} else{ goto
_LL2241;} _LL2241: if(( unsigned int) _temp2215 >  4u?*(( int*) _temp2215) == 
Cyc_Absyn_Evar: 0){ _LL2340: _temp2339=(( struct Cyc_Absyn_Evar_struct*)
_temp2215)->f2; goto _LL2242;} else{ goto _LL2243;} _LL2243: if(( unsigned int)
_temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2342:
_temp2341=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2215)->f1;
goto _LL2244;} else{ goto _LL2245;} _LL2245: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_EnumType: 0){ goto _LL2246;} else{ goto _LL2247;}
_LL2247: if( _temp2215 == ( void*) Cyc_Absyn_VoidType){ goto _LL2248;} else{
goto _LL2249;} _LL2249: if(( unsigned int) _temp2215 >  4u?*(( int*) _temp2215)
==  Cyc_Absyn_IntType: 0){ goto _LL2250;} else{ goto _LL2251;} _LL2251: if(
_temp2215 == ( void*) Cyc_Absyn_FloatType){ goto _LL2252;} else{ goto _LL2253;}
_LL2253: if( _temp2215 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2254;} else{
goto _LL2255;} _LL2255: if( _temp2215 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL2256;} else{ goto _LL2257;} _LL2257: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_RgnsEff: 0){ _LL2344: _temp2343=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2215)->f1; goto _LL2258;} else{ goto _LL2259;}
_LL2259: if(( unsigned int) _temp2215 >  4u?*(( int*) _temp2215) ==  Cyc_Absyn_AccessEff:
0){ _LL2346: _temp2345=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2215)->f1;
goto _LL2260;} else{ goto _LL2261;} _LL2261: if(( unsigned int) _temp2215 >  4u?*((
int*) _temp2215) ==  Cyc_Absyn_JoinEff: 0){ _LL2348: _temp2347=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2215)->f1; goto _LL2262;} else{ goto _LL2216;} _LL2218: { struct
_handler_cons _temp2349; _push_handler(& _temp2349);{ int _temp2351= 0; if(
setjmp( _temp2349.handler)){ _temp2351= 1;} if( ! _temp2351){{ void* _temp2352=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* k)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2263); _npop_handler( 0u); return _temp2352;}; _pop_handler();} else{
void* _temp2350=( void*) _exn_thrown; void* _temp2354= _temp2350; _LL2356: if(
_temp2354 ==  Cyc_Core_Not_found){ goto _LL2357;} else{ goto _LL2358;} _LL2358:
goto _LL2359; _LL2357: return t; _LL2359:( void) _throw( _temp2354); _LL2355:;}}}
_LL2220: { struct Cyc_List_List* _temp2360= Cyc_Tcutil_substs( rgn, inst,
_temp2267); return _temp2360 ==  _temp2267? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2361=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2361[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2362; _temp2362.tag=
Cyc_Absyn_StructType; _temp2362.f1= _temp2269; _temp2362.f2= _temp2360;
_temp2362.f3= _temp2265; _temp2362;}); _temp2361;});} _LL2222: { struct Cyc_List_List*
_temp2363= Cyc_Tcutil_substs( rgn, inst, _temp2273); return _temp2363 == 
_temp2273? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2364=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2364[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2365; _temp2365.tag= Cyc_Absyn_UnionType;
_temp2365.f1= _temp2275; _temp2365.f2= _temp2363; _temp2365.f3= _temp2271;
_temp2365;}); _temp2364;});} _LL2224: { struct Cyc_List_List* _temp2366= Cyc_Tcutil_substs(
rgn, inst, _temp2281); void* _temp2367= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2279); return( _temp2366 ==  _temp2281? _temp2367 ==  _temp2279: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2368=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2368[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2369; _temp2369.tag= Cyc_Absyn_TunionType;
_temp2369.f1=({ struct Cyc_Absyn_TunionInfo _temp2370; _temp2370.tunion_info=(
void*) _temp2283; _temp2370.targs= _temp2366; _temp2370.rgn=( void*) _temp2367;
_temp2370;}); _temp2369;}); _temp2368;});} _LL2226: { struct Cyc_List_List*
_temp2371= Cyc_Tcutil_substs( rgn, inst, _temp2287); return _temp2371 == 
_temp2287? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2372=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2372[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2373; _temp2373.tag=
Cyc_Absyn_TunionFieldType; _temp2373.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2374; _temp2374.field_info=( void*) _temp2289; _temp2374.targs= _temp2371;
_temp2374;}); _temp2373;}); _temp2372;});} _LL2228: { struct Cyc_List_List*
_temp2375= Cyc_Tcutil_substs( rgn, inst, _temp2293); return _temp2375 == 
_temp2293? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2376=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2376[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2377; _temp2377.tag=
Cyc_Absyn_TypedefType; _temp2377.f1= _temp2295; _temp2377.f2= _temp2375;
_temp2377.f3= _temp2291; _temp2377;}); _temp2376;});} _LL2230: { void* _temp2378=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2301); return _temp2378 ==  _temp2301? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2379=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2379[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2380; _temp2380.tag= Cyc_Absyn_ArrayType;
_temp2380.f1=( void*) _temp2378; _temp2380.f2= _temp2299; _temp2380.f3=
_temp2297; _temp2380;}); _temp2379;});} _LL2232: { void* _temp2381= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2313); void* _temp2382= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2311); if( _temp2381 ==  _temp2313? _temp2382 ==  _temp2311: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2383=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2383[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2384; _temp2384.tag= Cyc_Absyn_PointerType;
_temp2384.f1=({ struct Cyc_Absyn_PtrInfo _temp2385; _temp2385.elt_typ=( void*)
_temp2381; _temp2385.rgn_typ=( void*) _temp2382; _temp2385.nullable= _temp2309;
_temp2385.tq= _temp2307; _temp2385.bounds= _temp2305; _temp2385;}); _temp2384;});
_temp2383;});} _LL2234:{ struct Cyc_List_List* _temp2386= _temp2331; for( 0;
_temp2386 !=  0; _temp2386=(( struct Cyc_List_List*) _check_null( _temp2386))->tl){
inst=({ struct Cyc_List_List* _temp2387=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2387->hd=( void*)({ struct _tuple5*
_temp2388=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2388->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2386))->hd; _temp2388->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2389=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2389[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2390; _temp2390.tag= Cyc_Absyn_VarType;
_temp2390.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2386))->hd; _temp2390;}); _temp2389;}); _temp2388;}); _temp2387->tl= inst;
_temp2387;});}}{ struct Cyc_List_List* _temp2393; struct Cyc_List_List*
_temp2395; struct _tuple0 _temp2391=(( struct _tuple0(*)( struct _RegionHandle*
r1, struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2325)); _LL2396:
_temp2395= _temp2391.f1; goto _LL2394; _LL2394: _temp2393= _temp2391.f2; goto
_LL2392; _LL2392: { struct Cyc_List_List* _temp2397= Cyc_Tcutil_substs( rgn,
inst, _temp2393); struct Cyc_List_List* _temp2398=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r1,
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2395, _temp2397)); struct Cyc_Core_Opt* eff2; if( _temp2329 ==  0){
eff2= 0;} else{ void* _temp2399= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2329))->v); if( _temp2399 == ( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2329))->v){ eff2= _temp2329;} else{ eff2=({
struct Cyc_Core_Opt* _temp2400=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2400->v=( void*) _temp2399; _temp2400;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2321 ==  0){ cyc_varargs2= 0;} else{ int _temp2403; void*
_temp2405; struct Cyc_Absyn_Tqual _temp2407; struct Cyc_Core_Opt* _temp2409;
struct Cyc_Absyn_VarargInfo _temp2401=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2321)); _LL2410: _temp2409= _temp2401.name; goto _LL2408;
_LL2408: _temp2407= _temp2401.tq; goto _LL2406; _LL2406: _temp2405=( void*)
_temp2401.type; goto _LL2404; _LL2404: _temp2403= _temp2401.inject; goto _LL2402;
_LL2402: { void* _temp2411= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2405); if(
_temp2411 ==  _temp2405){ cyc_varargs2= _temp2321;} else{ cyc_varargs2=({ struct
Cyc_Absyn_VarargInfo* _temp2412=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2412->name= _temp2409; _temp2412->tq=
_temp2407; _temp2412->type=( void*) _temp2411; _temp2412->inject= _temp2403;
_temp2412;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2415;
struct Cyc_List_List* _temp2417; struct _tuple0 _temp2413= Cyc_List_rsplit( rgn,
rgn, _temp2319); _LL2418: _temp2417= _temp2413.f1; goto _LL2416; _LL2416:
_temp2415= _temp2413.f2; goto _LL2414; _LL2414: { struct Cyc_List_List*
_temp2419= Cyc_Tcutil_substs( rgn, inst, _temp2417); struct Cyc_List_List*
_temp2420= Cyc_Tcutil_substs( rgn, inst, _temp2415); if( _temp2419 ==  _temp2417?
_temp2420 ==  _temp2415: 0){ rgn_po2= _temp2319;} else{ rgn_po2= Cyc_List_zip(
_temp2419, _temp2420);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2421=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2421[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2422; _temp2422.tag= Cyc_Absyn_FnType;
_temp2422.f1=({ struct Cyc_Absyn_FnInfo _temp2423; _temp2423.tvars= _temp2331;
_temp2423.effect= eff2; _temp2423.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2327); _temp2423.args= _temp2398; _temp2423.c_varargs= _temp2323;
_temp2423.cyc_varargs= cyc_varargs2; _temp2423.rgn_po= rgn_po2; _temp2423.attributes=
_temp2317; _temp2423;}); _temp2422;}); _temp2421;});}}}}} _LL2236: { struct Cyc_List_List*
_temp2424=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2333); struct Cyc_List_List* _temp2425= Cyc_Tcutil_substs( rgn, inst,
_temp2424); if( _temp2425 ==  _temp2424){ return t;}{ struct Cyc_List_List*
_temp2426=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2333, _temp2425); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2427=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2427[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2428; _temp2428.tag= Cyc_Absyn_TupleType;
_temp2428.f1= _temp2426; _temp2428;}); _temp2427;});}} _LL2238: { struct Cyc_List_List*
_temp2429=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2335); struct Cyc_List_List* _temp2430= Cyc_Tcutil_substs( rgn, inst,
_temp2429); if( _temp2430 ==  _temp2429){ return t;}{ struct Cyc_List_List*
_temp2431=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2335, _temp2430); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2432=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2432[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2433; _temp2433.tag= Cyc_Absyn_AnonStructType;
_temp2433.f1= _temp2431; _temp2433;}); _temp2432;});}} _LL2240: { struct Cyc_List_List*
_temp2434=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2337); struct Cyc_List_List* _temp2435= Cyc_Tcutil_substs( rgn, inst,
_temp2434); if( _temp2435 ==  _temp2434){ return t;}{ struct Cyc_List_List*
_temp2436=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2337, _temp2435); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2437=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2437[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2438; _temp2438.tag= Cyc_Absyn_AnonStructType;
_temp2438.f1= _temp2436; _temp2438;}); _temp2437;});}} _LL2242: if( _temp2339 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2339))->v);} else{ return t;} _LL2244: { void* _temp2439= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2341); return _temp2439 ==  _temp2341? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2440=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2440[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2441; _temp2441.tag= Cyc_Absyn_RgnHandleType; _temp2441.f1=( void*)
_temp2439; _temp2441;}); _temp2440;});} _LL2246: return t; _LL2248: return t;
_LL2250: return t; _LL2252: return t; _LL2254: return t; _LL2256: return t;
_LL2258: { void* _temp2442= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2343);
return _temp2442 ==  _temp2343? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2443=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2443[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2444; _temp2444.tag= Cyc_Absyn_RgnsEff;
_temp2444.f1=( void*) _temp2442; _temp2444;}); _temp2443;});} _LL2260: { void*
_temp2445= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2345); return _temp2445 == 
_temp2345? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2446=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2446[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2447; _temp2447.tag= Cyc_Absyn_AccessEff;
_temp2447.f1=( void*) _temp2445; _temp2447;}); _temp2446;});} _LL2262: { struct
Cyc_List_List* _temp2448= Cyc_Tcutil_substs( rgn, inst, _temp2347); return
_temp2448 ==  _temp2347? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2449=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2449[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2450; _temp2450.tag= Cyc_Absyn_JoinEff;
_temp2450.f1= _temp2448; _temp2450;}); _temp2449;});} _LL2216:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2451=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2452=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2453= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2451); struct Cyc_List_List* _temp2454= Cyc_Tcutil_substs( rgn,
inst, _temp2452); if( _temp2451 ==  _temp2453? _temp2452 ==  _temp2454: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2455=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2455->hd=(
void*) _temp2453; _temp2455->tl= _temp2454; _temp2455;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2500=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2500[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2501; _temp2501.tag= Cyc_Absyn_Const_e; _temp2501.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2501;}); _temp2500;}), loc);{ void*
_temp2456= Cyc_Tcutil_compress( t); void* _temp2470; void* _temp2472; void*
_temp2474; void* _temp2476; _LL2458: if(( unsigned int) _temp2456 >  4u?*(( int*)
_temp2456) ==  Cyc_Absyn_PointerType: 0){ goto _LL2459;} else{ goto _LL2460;}
_LL2460: if(( unsigned int) _temp2456 >  4u?*(( int*) _temp2456) ==  Cyc_Absyn_IntType:
0){ _LL2473: _temp2472=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2456)->f1;
goto _LL2471; _LL2471: _temp2470=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2456)->f2; if( _temp2470 == ( void*) Cyc_Absyn_B1){ goto _LL2461;} else{
goto _LL2462;}} else{ goto _LL2462;} _LL2462: if(( unsigned int) _temp2456 >  4u?*((
int*) _temp2456) ==  Cyc_Absyn_IntType: 0){ _LL2477: _temp2476=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2456)->f1; goto _LL2475; _LL2475: _temp2474=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2456)->f2; goto _LL2463;} else{
goto _LL2464;} _LL2464: if( _temp2456 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2465;} else{ goto _LL2466;} _LL2466: if( _temp2456 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2467;} else{ goto _LL2468;} _LL2468: goto _LL2469; _LL2459: goto _LL2457;
_LL2461:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2478=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2478[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2479; _temp2479.tag= Cyc_Absyn_Const_e;
_temp2479.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2480=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2480[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2481; _temp2481.tag= Cyc_Absyn_Char_c;
_temp2481.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2481.f2='\000'; _temp2481;});
_temp2480;})); _temp2479;}); _temp2478;}))); goto _LL2457; _LL2463:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2482=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2482[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2483; _temp2483.tag= Cyc_Absyn_Const_e; _temp2483.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2484=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2484[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2485; _temp2485.tag= Cyc_Absyn_Int_c; _temp2485.f1=( void*) _temp2476;
_temp2485.f2= 0; _temp2485;}); _temp2484;})); _temp2483;}); _temp2482;}))); if(
_temp2474 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2486=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2486[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2487; _temp2487.tag= Cyc_Absyn_Cast_e;
_temp2487.f1=( void*) t; _temp2487.f2= e; _temp2487;}); _temp2486;}), loc);}
goto _LL2457; _LL2465:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2488=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2488[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2489; _temp2489.tag= Cyc_Absyn_Const_e;
_temp2489.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2490=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2490[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2491; _temp2491.tag= Cyc_Absyn_Float_c;
_temp2491.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2491;});
_temp2490;})); _temp2489;}); _temp2488;}))); goto _LL2457; _LL2467:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2492=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2492[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2493; _temp2493.tag= Cyc_Absyn_Cast_e; _temp2493.f1=( void*) t; _temp2493.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2494=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2494[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2495; _temp2495.tag= Cyc_Absyn_Const_e;
_temp2495.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2496=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2496[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2497; _temp2497.tag= Cyc_Absyn_Float_c;
_temp2497.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2497;});
_temp2496;})); _temp2495;}); _temp2494;}), loc); _temp2493;}); _temp2492;})));
goto _LL2457; _LL2469:({ struct Cyc_Std_String_pa_struct _temp2499; _temp2499.tag=
Cyc_Std_String_pa; _temp2499.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2498[ 1u]={& _temp2499}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2498, sizeof( void*), 1u));}});
goto _LL2457; _LL2457:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2502=( struct _tuple5*) _cycalloc(
sizeof( struct _tuple5)); _temp2502->f1= tv; _temp2502->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2503=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2503->v=( void*) k; _temp2503;}),({ struct Cyc_Core_Opt*
_temp2504=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2504->v=( void*) s; _temp2504;})); _temp2502;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2507;
struct _RegionHandle* _temp2508; struct Cyc_List_List* _temp2510; struct _tuple6*
_temp2505= env; _temp2507=* _temp2505; _LL2511: _temp2510= _temp2507.f1; goto
_LL2509; _LL2509: _temp2508= _temp2507.f2; goto _LL2506; _LL2506: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2512=( struct _tuple5*) _region_malloc(
_temp2508, sizeof( struct _tuple5)); _temp2512->f1= tv; _temp2512->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2513=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2513->v=( void*) k; _temp2513;}),({ struct Cyc_Core_Opt*
_temp2514=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2514->v=( void*) _temp2510; _temp2514;})); _temp2512;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_Std_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_Std_String_pa_struct _temp2518; _temp2518.tag= Cyc_Std_String_pa;
_temp2518.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Std_String_pa_struct
_temp2517; _temp2517.tag= Cyc_Std_String_pa; _temp2517.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_Std_String_pa_struct _temp2516;
_temp2516.tag= Cyc_Std_String_pa; _temp2516.f1=( struct _tagged_arr)* tv->name;{
void* _temp2515[ 3u]={& _temp2516,& _temp2517,& _temp2518}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2515, sizeof( void*), 3u));}}}});} if( tv->identity
==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!= *(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2519[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2519, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2520=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2520->hd=( void*) tv; _temp2520->tl= tvs; _temp2520;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2521[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2521, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity ==  0){({ void* _temp2522[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2522, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2523=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2523->hd=( void*) tv; _temp2523->tl=
tvs; _temp2523;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp2525; _temp2525.tag= Cyc_Std_String_pa;
_temp2525.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2524[
1u]={& _temp2525};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2524, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2526=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2526->hd=(
void*) tv; _temp2526->tl= tvs; _temp2526;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2527= Cyc_Tcutil_compress( e);
int _temp2533; _LL2529: if(( unsigned int) _temp2527 >  4u?*(( int*) _temp2527)
==  Cyc_Absyn_Evar: 0){ _LL2534: _temp2533=(( struct Cyc_Absyn_Evar_struct*)
_temp2527)->f3; goto _LL2530;} else{ goto _LL2531;} _LL2531: goto _LL2532;
_LL2530:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2535= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2541; _LL2537: if((
unsigned int) _temp2535 >  4u?*(( int*) _temp2535) ==  Cyc_Absyn_Evar: 0){
_LL2542: _temp2541=(( struct Cyc_Absyn_Evar_struct*) _temp2535)->f3; goto
_LL2538;} else{ goto _LL2539;} _LL2539: goto _LL2540; _LL2538: if( _temp2533 == 
_temp2541){ return es;} goto _LL2536; _LL2540: goto _LL2536; _LL2536:;}} return({
struct Cyc_List_List* _temp2543=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2543->hd=( void*) e; _temp2543->tl= es; _temp2543;});
_LL2532: return es; _LL2528:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b !=  0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) == *(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2544=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2544->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2544->tl= r; _temp2544;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp2546; _temp2546.tag= Cyc_Std_String_pa; _temp2546.f1=( struct _tagged_arr)*
fn;{ void* _temp2545[ 1u]={& _temp2546}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2545, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2547= Cyc_Tcutil_compress( field_typ); void* _temp2553;
_LL2549: if(( unsigned int) _temp2547 >  4u?*(( int*) _temp2547) ==  Cyc_Absyn_IntType:
0){ _LL2554: _temp2553=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2547)->f2;
goto _LL2550;} else{ goto _LL2551;} _LL2551: goto _LL2552; _LL2550:{ void*
_temp2555= _temp2553; _LL2557: if( _temp2555 == ( void*) Cyc_Absyn_B1){ goto
_LL2558;} else{ goto _LL2559;} _LL2559: if( _temp2555 == ( void*) Cyc_Absyn_B2){
goto _LL2560;} else{ goto _LL2561;} _LL2561: if( _temp2555 == ( void*) Cyc_Absyn_B4){
goto _LL2562;} else{ goto _LL2563;} _LL2563: if( _temp2555 == ( void*) Cyc_Absyn_B8){
goto _LL2564;} else{ goto _LL2556;} _LL2558: if( w >  8){({ void* _temp2565[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2565, sizeof( void*), 0u));});} goto
_LL2556; _LL2560: if( w >  16){({ void* _temp2566[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2566, sizeof( void*), 0u));});} goto _LL2556; _LL2562: if( w >  32){({ void*
_temp2567[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2567, sizeof( void*), 0u));});}
goto _LL2556; _LL2564: if( w >  64){({ void* _temp2568[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2568, sizeof( void*), 0u));});} goto _LL2556; _LL2556:;} goto
_LL2548; _LL2552:({ struct Cyc_Std_String_pa_struct _temp2571; _temp2571.tag=
Cyc_Std_String_pa; _temp2571.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp2570; _temp2570.tag= Cyc_Std_String_pa;
_temp2570.f1=( struct _tagged_arr)* fn;{ void* _temp2569[ 2u]={& _temp2570,&
_temp2571}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2569, sizeof( void*), 2u));}}});
goto _LL2548; _LL2548:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp2572=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL2574: if( _temp2572 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL2575;} else{ goto _LL2576;} _LL2576: if((
unsigned int) _temp2572 >  16u?*(( int*) _temp2572) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2577;} else{ goto _LL2578;} _LL2578: goto _LL2579; _LL2575: continue;
_LL2577: continue; _LL2579:({ struct Cyc_Std_String_pa_struct _temp2582;
_temp2582.tag= Cyc_Std_String_pa; _temp2582.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp2581; _temp2581.tag= Cyc_Std_String_pa;
_temp2581.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp2580[ 2u]={& _temp2581,&
_temp2582}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2580, sizeof( void*), 2u));}}});
goto _LL2573; _LL2573:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv
cvtenv, void* expected_kind, void* t){{ void* _temp2583= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp2631; struct Cyc_Core_Opt** _temp2633; struct Cyc_Core_Opt*
_temp2634; struct Cyc_Core_Opt** _temp2636; struct Cyc_Absyn_Tvar* _temp2637;
struct Cyc_Absyn_Enumdecl* _temp2639; struct Cyc_Absyn_Enumdecl** _temp2641;
struct _tuple1* _temp2642; struct Cyc_Absyn_TunionInfo _temp2644; void*
_temp2646; struct Cyc_List_List* _temp2648; void* _temp2650; void** _temp2652;
struct Cyc_Absyn_TunionFieldInfo _temp2653; struct Cyc_List_List* _temp2655;
void* _temp2657; void** _temp2659; struct Cyc_Absyn_PtrInfo _temp2660; struct
Cyc_Absyn_Conref* _temp2662; struct Cyc_Absyn_Tqual _temp2664; struct Cyc_Absyn_Conref*
_temp2666; void* _temp2668; void* _temp2670; struct Cyc_Absyn_Exp* _temp2672;
struct Cyc_Absyn_Tqual _temp2674; void* _temp2676; struct Cyc_Absyn_FnInfo
_temp2678; struct Cyc_List_List* _temp2680; struct Cyc_List_List* _temp2682;
struct Cyc_Absyn_VarargInfo* _temp2684; int _temp2686; struct Cyc_List_List*
_temp2688; void* _temp2690; struct Cyc_Core_Opt* _temp2692; struct Cyc_Core_Opt**
_temp2694; struct Cyc_List_List* _temp2695; struct Cyc_List_List** _temp2697;
struct Cyc_List_List* _temp2698; struct Cyc_List_List* _temp2700; struct Cyc_List_List*
_temp2702; struct Cyc_Absyn_Structdecl** _temp2704; struct Cyc_Absyn_Structdecl***
_temp2706; struct Cyc_List_List* _temp2707; struct _tuple1* _temp2709; struct
Cyc_Absyn_Uniondecl** _temp2711; struct Cyc_Absyn_Uniondecl*** _temp2713; struct
Cyc_List_List* _temp2714; struct _tuple1* _temp2716; struct Cyc_Core_Opt*
_temp2718; struct Cyc_Core_Opt** _temp2720; struct Cyc_List_List* _temp2721;
struct Cyc_List_List** _temp2723; struct _tuple1* _temp2724; void* _temp2726;
void* _temp2728; void* _temp2730; struct Cyc_List_List* _temp2732; _LL2585: if(
_temp2583 == ( void*) Cyc_Absyn_VoidType){ goto _LL2586;} else{ goto _LL2587;}
_LL2587: if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_Evar:
0){ _LL2635: _temp2634=(( struct Cyc_Absyn_Evar_struct*) _temp2583)->f1;
_temp2636=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2583)->f1;
goto _LL2632; _LL2632: _temp2631=(( struct Cyc_Absyn_Evar_struct*) _temp2583)->f2;
_temp2633=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp2583)->f2;
goto _LL2588;} else{ goto _LL2589;} _LL2589: if(( unsigned int) _temp2583 >  4u?*((
int*) _temp2583) ==  Cyc_Absyn_VarType: 0){ _LL2638: _temp2637=(( struct Cyc_Absyn_VarType_struct*)
_temp2583)->f1; goto _LL2590;} else{ goto _LL2591;} _LL2591: if(( unsigned int)
_temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_EnumType: 0){ _LL2643:
_temp2642=(( struct Cyc_Absyn_EnumType_struct*) _temp2583)->f1; goto _LL2640;
_LL2640: _temp2639=(( struct Cyc_Absyn_EnumType_struct*) _temp2583)->f2;
_temp2641=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2583)->f2; goto _LL2592;} else{ goto _LL2593;} _LL2593: if(( unsigned int)
_temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_TunionType: 0){ _LL2645:
_temp2644=(( struct Cyc_Absyn_TunionType_struct*) _temp2583)->f1; _LL2651:
_temp2650=( void*) _temp2644.tunion_info; _temp2652=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2583)->f1).tunion_info; goto _LL2649; _LL2649: _temp2648= _temp2644.targs;
goto _LL2647; _LL2647: _temp2646=( void*) _temp2644.rgn; goto _LL2594;} else{
goto _LL2595;} _LL2595: if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2654: _temp2653=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2583)->f1; _LL2658: _temp2657=( void*) _temp2653.field_info; _temp2659=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2583)->f1).field_info;
goto _LL2656; _LL2656: _temp2655= _temp2653.targs; goto _LL2596;} else{ goto
_LL2597;} _LL2597: if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583) == 
Cyc_Absyn_PointerType: 0){ _LL2661: _temp2660=(( struct Cyc_Absyn_PointerType_struct*)
_temp2583)->f1; _LL2671: _temp2670=( void*) _temp2660.elt_typ; goto _LL2669;
_LL2669: _temp2668=( void*) _temp2660.rgn_typ; goto _LL2667; _LL2667: _temp2666=
_temp2660.nullable; goto _LL2665; _LL2665: _temp2664= _temp2660.tq; goto _LL2663;
_LL2663: _temp2662= _temp2660.bounds; goto _LL2598;} else{ goto _LL2599;}
_LL2599: if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_IntType:
0){ goto _LL2600;} else{ goto _LL2601;} _LL2601: if( _temp2583 == ( void*) Cyc_Absyn_FloatType){
goto _LL2602;} else{ goto _LL2603;} _LL2603: if( _temp2583 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2604;} else{ goto _LL2605;} _LL2605: if(( unsigned int) _temp2583 >  4u?*((
int*) _temp2583) ==  Cyc_Absyn_ArrayType: 0){ _LL2677: _temp2676=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2583)->f1; goto _LL2675; _LL2675:
_temp2674=(( struct Cyc_Absyn_ArrayType_struct*) _temp2583)->f2; goto _LL2673;
_LL2673: _temp2672=(( struct Cyc_Absyn_ArrayType_struct*) _temp2583)->f3; goto
_LL2606;} else{ goto _LL2607;} _LL2607: if(( unsigned int) _temp2583 >  4u?*((
int*) _temp2583) ==  Cyc_Absyn_FnType: 0){ _LL2679: _temp2678=(( struct Cyc_Absyn_FnType_struct*)
_temp2583)->f1; _LL2696: _temp2695= _temp2678.tvars; _temp2697=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2583)->f1).tvars; goto _LL2693; _LL2693:
_temp2692= _temp2678.effect; _temp2694=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2583)->f1).effect; goto _LL2691; _LL2691: _temp2690=( void*) _temp2678.ret_typ;
goto _LL2689; _LL2689: _temp2688= _temp2678.args; goto _LL2687; _LL2687:
_temp2686= _temp2678.c_varargs; goto _LL2685; _LL2685: _temp2684= _temp2678.cyc_varargs;
goto _LL2683; _LL2683: _temp2682= _temp2678.rgn_po; goto _LL2681; _LL2681:
_temp2680= _temp2678.attributes; goto _LL2608;} else{ goto _LL2609;} _LL2609:
if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_TupleType:
0){ _LL2699: _temp2698=(( struct Cyc_Absyn_TupleType_struct*) _temp2583)->f1;
goto _LL2610;} else{ goto _LL2611;} _LL2611: if(( unsigned int) _temp2583 >  4u?*((
int*) _temp2583) ==  Cyc_Absyn_AnonStructType: 0){ _LL2701: _temp2700=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2583)->f1; goto _LL2612;} else{ goto
_LL2613;} _LL2613: if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2703: _temp2702=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2583)->f1; goto _LL2614;} else{ goto _LL2615;} _LL2615: if(( unsigned int)
_temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_StructType: 0){ _LL2710:
_temp2709=(( struct Cyc_Absyn_StructType_struct*) _temp2583)->f1; goto _LL2708;
_LL2708: _temp2707=(( struct Cyc_Absyn_StructType_struct*) _temp2583)->f2; goto
_LL2705; _LL2705: _temp2704=(( struct Cyc_Absyn_StructType_struct*) _temp2583)->f3;
_temp2706=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2583)->f3; goto _LL2616;} else{ goto _LL2617;} _LL2617: if(( unsigned int)
_temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_UnionType: 0){ _LL2717:
_temp2716=(( struct Cyc_Absyn_UnionType_struct*) _temp2583)->f1; goto _LL2715;
_LL2715: _temp2714=(( struct Cyc_Absyn_UnionType_struct*) _temp2583)->f2; goto
_LL2712; _LL2712: _temp2711=(( struct Cyc_Absyn_UnionType_struct*) _temp2583)->f3;
_temp2713=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2583)->f3; goto _LL2618;} else{ goto _LL2619;} _LL2619: if(( unsigned int)
_temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_TypedefType: 0){ _LL2725:
_temp2724=(( struct Cyc_Absyn_TypedefType_struct*) _temp2583)->f1; goto _LL2722;
_LL2722: _temp2721=(( struct Cyc_Absyn_TypedefType_struct*) _temp2583)->f2;
_temp2723=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2583)->f2; goto _LL2719; _LL2719: _temp2718=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2583)->f3; _temp2720=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2583)->f3; goto _LL2620;} else{ goto _LL2621;} _LL2621: if( _temp2583 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2622;} else{ goto _LL2623;} _LL2623: if((
unsigned int) _temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2727: _temp2726=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2583)->f1; goto _LL2624;} else{ goto _LL2625;} _LL2625: if(( unsigned int)
_temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_AccessEff: 0){ _LL2729:
_temp2728=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2583)->f1; goto
_LL2626;} else{ goto _LL2627;} _LL2627: if(( unsigned int) _temp2583 >  4u?*((
int*) _temp2583) ==  Cyc_Absyn_RgnsEff: 0){ _LL2731: _temp2730=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2583)->f1; goto _LL2628;} else{ goto _LL2629;}
_LL2629: if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_JoinEff:
0){ _LL2733: _temp2732=(( struct Cyc_Absyn_JoinEff_struct*) _temp2583)->f1; goto
_LL2630;} else{ goto _LL2584;} _LL2586: goto _LL2584; _LL2588: if(* _temp2636 == 
0){* _temp2636=({ struct Cyc_Core_Opt* _temp2734=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2734->v=( void*) expected_kind;
_temp2734;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2633=({ struct Cyc_Core_Opt* _temp2735=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2735->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2735;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2736= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2633=({ struct Cyc_Core_Opt* _temp2737=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2737->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2738=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2738[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2739; _temp2739.tag= Cyc_Absyn_VarType;
_temp2739.f1= _temp2736; _temp2739;}); _temp2738;})); _temp2737;}); _temp2637=
_temp2736; goto _LL2590;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2584; _LL2590: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2637->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2740=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2740[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2741; _temp2741.tag= Cyc_Absyn_Eq_constr;
_temp2741.f1=( void*) expected_kind; _temp2741;}); _temp2740;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2637); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2637); goto _LL2584;}
_LL2592: if(* _temp2641 ==  0){ struct _handler_cons _temp2742; _push_handler(&
_temp2742);{ int _temp2744= 0; if( setjmp( _temp2742.handler)){ _temp2744= 1;}
if( ! _temp2744){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2642);* _temp2641=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2743=( void*) _exn_thrown; void* _temp2746= _temp2743; _LL2748:
if( _temp2746 ==  Cyc_Dict_Absent){ goto _LL2749;} else{ goto _LL2750;} _LL2750:
goto _LL2751; _LL2749:({ struct Cyc_Std_String_pa_struct _temp2753; _temp2753.tag=
Cyc_Std_String_pa; _temp2753.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2642);{ void* _temp2752[ 1u]={& _temp2753}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u), _tag_arr( _temp2752, sizeof( void*), 1u));}});
return cvtenv; _LL2751:( void) _throw( _temp2746); _LL2747:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2641);* _temp2642=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2584;} _LL2594:{ void*
_temp2754=* _temp2652; struct Cyc_Absyn_UnknownTunionInfo _temp2760; int
_temp2762; struct _tuple1* _temp2764; struct Cyc_Absyn_Tuniondecl** _temp2766;
struct Cyc_Absyn_Tuniondecl* _temp2768; _LL2756: if(*(( int*) _temp2754) ==  Cyc_Absyn_UnknownTunion){
_LL2761: _temp2760=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2754)->f1;
_LL2765: _temp2764= _temp2760.name; goto _LL2763; _LL2763: _temp2762= _temp2760.is_xtunion;
goto _LL2757;} else{ goto _LL2758;} _LL2758: if(*(( int*) _temp2754) ==  Cyc_Absyn_KnownTunion){
_LL2767: _temp2766=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2754)->f1;
_temp2768=* _temp2766; goto _LL2759;} else{ goto _LL2755;} _LL2757: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp2769; _push_handler(& _temp2769);{ int
_temp2771= 0; if( setjmp( _temp2769.handler)){ _temp2771= 1;} if( ! _temp2771){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp2764);; _pop_handler();} else{
void* _temp2770=( void*) _exn_thrown; void* _temp2773= _temp2770; _LL2775: if(
_temp2773 ==  Cyc_Dict_Absent){ goto _LL2776;} else{ goto _LL2777;} _LL2777:
goto _LL2778; _LL2776:({ struct Cyc_Std_String_pa_struct _temp2780; _temp2780.tag=
Cyc_Std_String_pa; _temp2780.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2764);{ void* _temp2779[ 1u]={& _temp2780}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type [x]tunion %s",
sizeof( unsigned char), 26u), _tag_arr( _temp2779, sizeof( void*), 1u));}});
return cvtenv; _LL2778:( void) _throw( _temp2773); _LL2774:;}}} if((* tudp)->is_xtunion
!=  _temp2762){({ struct Cyc_Std_String_pa_struct _temp2782; _temp2782.tag= Cyc_Std_String_pa;
_temp2782.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2764);{ void*
_temp2781[ 1u]={& _temp2782}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2781, sizeof( void*), 1u));}});}*
_temp2652=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2783=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2783[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2784; _temp2784.tag= Cyc_Absyn_KnownTunion;
_temp2784.f1= tudp; _temp2784;}); _temp2783;}); _temp2768=* tudp; goto _LL2759;}
_LL2759: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2646);{ struct Cyc_List_List* tvs= _temp2768->tvs; for( 0; _temp2648 !=  0?
tvs !=  0: 0;( _temp2648=(( struct Cyc_List_List*) _check_null( _temp2648))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2648))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2648
!=  0){({ struct Cyc_Std_String_pa_struct _temp2786; _temp2786.tag= Cyc_Std_String_pa;
_temp2786.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2768->name);{
void* _temp2785[ 1u]={& _temp2786}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2785, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2788; _temp2788.tag= Cyc_Std_String_pa;
_temp2788.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2768->name);{
void* _temp2787[ 1u]={& _temp2788}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2787, sizeof( void*), 1u));}});}
goto _LL2755;} _LL2755:;} goto _LL2584; _LL2596:{ void* _temp2789=* _temp2659;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2795; int _temp2797; struct _tuple1*
_temp2799; struct _tuple1* _temp2801; struct Cyc_Absyn_Tunionfield* _temp2803;
struct Cyc_Absyn_Tuniondecl* _temp2805; _LL2791: if(*(( int*) _temp2789) ==  Cyc_Absyn_UnknownTunionfield){
_LL2796: _temp2795=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2789)->f1;
_LL2802: _temp2801= _temp2795.tunion_name; goto _LL2800; _LL2800: _temp2799=
_temp2795.field_name; goto _LL2798; _LL2798: _temp2797= _temp2795.is_xtunion;
goto _LL2792;} else{ goto _LL2793;} _LL2793: if(*(( int*) _temp2789) ==  Cyc_Absyn_KnownTunionfield){
_LL2806: _temp2805=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2789)->f1;
goto _LL2804; _LL2804: _temp2803=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2789)->f2; goto _LL2794;} else{ goto _LL2790;} _LL2792: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2807;
_push_handler(& _temp2807);{ int _temp2809= 0; if( setjmp( _temp2807.handler)){
_temp2809= 1;} if( ! _temp2809){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2801);; _pop_handler();} else{ void* _temp2808=( void*) _exn_thrown; void*
_temp2811= _temp2808; _LL2813: if( _temp2811 ==  Cyc_Dict_Absent){ goto _LL2814;}
else{ goto _LL2815;} _LL2815: goto _LL2816; _LL2814:({ struct Cyc_Std_String_pa_struct
_temp2818; _temp2818.tag= Cyc_Std_String_pa; _temp2818.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2801);{ void* _temp2817[ 1u]={& _temp2818}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2817, sizeof( void*), 1u));}}); return cvtenv; _LL2816:( void) _throw(
_temp2811); _LL2812:;}}}{ struct _handler_cons _temp2819; _push_handler(&
_temp2819);{ int _temp2821= 0; if( setjmp( _temp2819.handler)){ _temp2821= 1;}
if( ! _temp2821){{ void* _temp2822= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2799); struct Cyc_Absyn_Tunionfield* _temp2828; struct Cyc_Absyn_Tuniondecl*
_temp2830; _LL2824: if(*(( int*) _temp2822) ==  Cyc_Tcenv_TunionRes){ _LL2831:
_temp2830=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2822)->f1; goto _LL2829;
_LL2829: _temp2828=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2822)->f2; goto
_LL2825;} else{ goto _LL2826;} _LL2826: goto _LL2827; _LL2825: tuf= _temp2828;
tud= _temp2830; if( tud->is_xtunion !=  _temp2797){({ struct Cyc_Std_String_pa_struct
_temp2833; _temp2833.tag= Cyc_Std_String_pa; _temp2833.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2801);{ void* _temp2832[ 1u]={& _temp2833}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2832, sizeof( void*), 1u));}});} goto
_LL2823; _LL2827:({ struct Cyc_Std_String_pa_struct _temp2836; _temp2836.tag=
Cyc_Std_String_pa; _temp2836.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2801);{ struct Cyc_Std_String_pa_struct _temp2835; _temp2835.tag= Cyc_Std_String_pa;
_temp2835.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2799);{ void*
_temp2834[ 2u]={& _temp2835,& _temp2836}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2834, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2837= cvtenv; _npop_handler( 0u); return _temp2837;}
_LL2823:;}; _pop_handler();} else{ void* _temp2820=( void*) _exn_thrown; void*
_temp2839= _temp2820; _LL2841: if( _temp2839 ==  Cyc_Dict_Absent){ goto _LL2842;}
else{ goto _LL2843;} _LL2843: goto _LL2844; _LL2842:({ struct Cyc_Std_String_pa_struct
_temp2847; _temp2847.tag= Cyc_Std_String_pa; _temp2847.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2801);{ struct Cyc_Std_String_pa_struct _temp2846;
_temp2846.tag= Cyc_Std_String_pa; _temp2846.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2799);{ void* _temp2845[ 2u]={& _temp2846,& _temp2847}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2845, sizeof( void*), 2u));}}}); return cvtenv; _LL2844:( void)
_throw( _temp2839); _LL2840:;}}}* _temp2659=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2848=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2848[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2849; _temp2849.tag= Cyc_Absyn_KnownTunionfield; _temp2849.f1= tud;
_temp2849.f2= tuf; _temp2849;}); _temp2848;}); _temp2805= tud; _temp2803= tuf;
goto _LL2794;} _LL2794: { struct Cyc_List_List* tvs= _temp2805->tvs; for( 0;
_temp2655 !=  0? tvs !=  0: 0;( _temp2655=(( struct Cyc_List_List*) _check_null(
_temp2655))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2655))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2655
!=  0){({ struct Cyc_Std_String_pa_struct _temp2852; _temp2852.tag= Cyc_Std_String_pa;
_temp2852.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2803->name);{
struct Cyc_Std_String_pa_struct _temp2851; _temp2851.tag= Cyc_Std_String_pa;
_temp2851.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2805->name);{
void* _temp2850[ 2u]={& _temp2851,& _temp2852}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2850, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp2855; _temp2855.tag= Cyc_Std_String_pa;
_temp2855.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2803->name);{
struct Cyc_Std_String_pa_struct _temp2854; _temp2854.tag= Cyc_Std_String_pa;
_temp2854.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2805->name);{
void* _temp2853[ 2u]={& _temp2854,& _temp2855}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2853, sizeof( void*), 2u));}}});}
goto _LL2790;} _LL2790:;} goto _LL2584; _LL2598: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2670); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2668);{ void* _temp2856=( void*)(
Cyc_Absyn_compress_conref( _temp2662))->v; void* _temp2862; struct Cyc_Absyn_Exp*
_temp2864; _LL2858: if(( unsigned int) _temp2856 >  1u?*(( int*) _temp2856) == 
Cyc_Absyn_Eq_constr: 0){ _LL2863: _temp2862=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2856)->f1; if(( unsigned int) _temp2862 >  1u?*(( int*) _temp2862) ==  Cyc_Absyn_Upper_b:
0){ _LL2865: _temp2864=(( struct Cyc_Absyn_Upper_b_struct*) _temp2862)->f1; goto
_LL2859;} else{ goto _LL2860;}} else{ goto _LL2860;} _LL2860: goto _LL2861;
_LL2859: if( ! Cyc_Tcutil_is_const_exp( te, _temp2864)){({ void* _temp2866[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2866, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2864); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2864)){({ void*
_temp2867[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2867, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2864); goto _LL2857; _LL2861: goto _LL2857; _LL2857:;} goto _LL2584;
_LL2600: goto _LL2584; _LL2602: goto _LL2584; _LL2604: goto _LL2584; _LL2606:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2676); if( _temp2672 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2672))){({ void* _temp2868[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2868, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2672)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2672))){({ void* _temp2869[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2869, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2672)); goto _LL2584; _LL2608: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2680 !=  0; _temp2680=(( struct Cyc_List_List*)
_check_null( _temp2680))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2680))->hd)){({ struct Cyc_Std_String_pa_struct _temp2871;
_temp2871.tag= Cyc_Std_String_pa; _temp2871.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2680))->hd);{ void* _temp2870[
1u]={& _temp2871}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2870, sizeof( void*), 1u));}});}{
void* _temp2872=( void*)(( struct Cyc_List_List*) _check_null( _temp2680))->hd;
int _temp2884; int _temp2886; void* _temp2888; _LL2874: if( _temp2872 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2875;} else{ goto _LL2876;} _LL2876: if(
_temp2872 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL2877;} else{ goto _LL2878;}
_LL2878: if( _temp2872 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL2879;} else{
goto _LL2880;} _LL2880: if(( unsigned int) _temp2872 >  16u?*(( int*) _temp2872)
==  Cyc_Absyn_Format_att: 0){ _LL2889: _temp2888=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2872)->f1; goto _LL2887; _LL2887: _temp2886=(( struct Cyc_Absyn_Format_att_struct*)
_temp2872)->f2; goto _LL2885; _LL2885: _temp2884=(( struct Cyc_Absyn_Format_att_struct*)
_temp2872)->f3; goto _LL2881;} else{ goto _LL2882;} _LL2882: goto _LL2883;
_LL2875: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2873;
_LL2877: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2873; _LL2879:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2873; _LL2881:
if( ! seen_format){ seen_format= 1; ft= _temp2888; fmt_desc_arg= _temp2886;
fmt_arg_start= _temp2884;} else{({ void* _temp2890[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2890, sizeof( void*), 0u));});} goto _LL2873; _LL2883: goto
_LL2873; _LL2873:;}} if( num_convs >  1){({ void* _temp2891[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2891, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2697);{ struct Cyc_List_List* b=* _temp2697; for( 0; b !=  0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2892=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2898; _LL2894: if(( unsigned int) _temp2892 >  1u?*(( int*) _temp2892)
==  Cyc_Absyn_Eq_constr: 0){ _LL2899: _temp2898=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2892)->f1; if( _temp2898 == ( void*) Cyc_Absyn_MemKind){ goto _LL2895;}
else{ goto _LL2896;}} else{ goto _LL2896;} _LL2896: goto _LL2897; _LL2895:({
struct Cyc_Std_String_pa_struct _temp2901; _temp2901.tag= Cyc_Std_String_pa;
_temp2901.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2900[ 1u]={& _temp2901}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2900, sizeof( void*), 1u));}}); goto
_LL2893; _LL2897: goto _LL2893; _LL2893:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2902=({
struct Cyc_Tcutil_CVTEnv _temp3061; _temp3061.kind_env= cvtenv.kind_env;
_temp3061.free_vars= 0; _temp3061.free_evars= 0; _temp3061.generalize_evars=
cvtenv.generalize_evars; _temp3061.fn_result= 1; _temp3061;}); _temp2902= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2902,( void*) Cyc_Absyn_MemKind, _temp2690); _temp2902.fn_result=
0;{ struct Cyc_List_List* a= _temp2688; for( 0; a !=  0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ _temp2902= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2902,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( a))->hd)).f3);}} if( _temp2684 !=  0){ if( _temp2686){({ void*
_temp2903[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2903, sizeof( void*), 0u));});}{ int _temp2906; void* _temp2908; struct Cyc_Absyn_Tqual
_temp2910; struct Cyc_Core_Opt* _temp2912; struct Cyc_Absyn_VarargInfo _temp2904=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp2684)); _LL2913: _temp2912=
_temp2904.name; goto _LL2911; _LL2911: _temp2910= _temp2904.tq; goto _LL2909;
_LL2909: _temp2908=( void*) _temp2904.type; goto _LL2907; _LL2907: _temp2906=
_temp2904.inject; goto _LL2905; _LL2905: _temp2902= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2902,( void*) Cyc_Absyn_MemKind, _temp2908); if( _temp2906){ void*
_temp2914= Cyc_Tcutil_compress( _temp2908); struct Cyc_Absyn_TunionInfo
_temp2920; void* _temp2922; void* _temp2924; _LL2916: if(( unsigned int)
_temp2914 >  4u?*(( int*) _temp2914) ==  Cyc_Absyn_TunionType: 0){ _LL2921:
_temp2920=(( struct Cyc_Absyn_TunionType_struct*) _temp2914)->f1; _LL2925:
_temp2924=( void*) _temp2920.tunion_info; if(*(( int*) _temp2924) ==  Cyc_Absyn_KnownTunion){
goto _LL2923;} else{ goto _LL2918;} _LL2923: _temp2922=( void*) _temp2920.rgn;
goto _LL2917;} else{ goto _LL2918;} _LL2918: goto _LL2919; _LL2917: goto _LL2915;
_LL2919:({ void* _temp2926[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u), _tag_arr( _temp2926, sizeof( void*), 0u));}); goto
_LL2915; _LL2915:;}}} if( seen_format){ int _temp2927=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2688); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg > 
_temp2927)? 1: fmt_arg_start <  0)? 1:( _temp2684 ==  0? fmt_arg_start !=  0: 0))?
1:( _temp2684 !=  0? fmt_arg_start !=  _temp2927 +  1: 0)){({ void* _temp2928[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2928, sizeof( void*), 0u));});} else{ void* _temp2931;
struct _tuple2 _temp2929=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp2688, fmt_desc_arg -  1); _LL2932: _temp2931= _temp2929.f3;
goto _LL2930; _LL2930:{ void* _temp2933= Cyc_Tcutil_compress( _temp2931); struct
Cyc_Absyn_PtrInfo _temp2939; struct Cyc_Absyn_Conref* _temp2941; void* _temp2943;
_LL2935: if(( unsigned int) _temp2933 >  4u?*(( int*) _temp2933) ==  Cyc_Absyn_PointerType:
0){ _LL2940: _temp2939=(( struct Cyc_Absyn_PointerType_struct*) _temp2933)->f1;
_LL2944: _temp2943=( void*) _temp2939.elt_typ; goto _LL2942; _LL2942: _temp2941=
_temp2939.bounds; goto _LL2936;} else{ goto _LL2937;} _LL2937: goto _LL2938;
_LL2936:{ struct _tuple8 _temp2946=({ struct _tuple8 _temp2945; _temp2945.f1=
Cyc_Tcutil_compress( _temp2943); _temp2945.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2941); _temp2945;}); void* _temp2952; void* _temp2954; void* _temp2956;
void* _temp2958; _LL2948: _LL2955: _temp2954= _temp2946.f1; if(( unsigned int)
_temp2954 >  4u?*(( int*) _temp2954) ==  Cyc_Absyn_IntType: 0){ _LL2959:
_temp2958=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2954)->f1; if(
_temp2958 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2957;} else{ goto _LL2950;}
_LL2957: _temp2956=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2954)->f2;
if( _temp2956 == ( void*) Cyc_Absyn_B1){ goto _LL2953;} else{ goto _LL2950;}}
else{ goto _LL2950;} _LL2953: _temp2952= _temp2946.f2; if( _temp2952 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2949;} else{ goto _LL2950;} _LL2950: goto _LL2951;
_LL2949: goto _LL2947; _LL2951:({ void* _temp2960[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp2960, sizeof( void*), 0u));}); goto _LL2947; _LL2947:;} goto
_LL2934; _LL2938:({ void* _temp2961[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2961, sizeof( void*), 0u));}); goto
_LL2934; _LL2934:;} if( fmt_arg_start !=  0){ void* _temp2962= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2684))->type); int
problem;{ void* _temp2963= ft; _LL2965: if( _temp2963 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL2966;} else{ goto _LL2967;} _LL2967: if( _temp2963 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL2968;} else{ goto _LL2964;} _LL2966:{ void* _temp2969= _temp2962; struct
Cyc_Absyn_TunionInfo _temp2975; void* _temp2977; struct Cyc_Absyn_Tuniondecl**
_temp2979; struct Cyc_Absyn_Tuniondecl* _temp2981; _LL2971: if(( unsigned int)
_temp2969 >  4u?*(( int*) _temp2969) ==  Cyc_Absyn_TunionType: 0){ _LL2976:
_temp2975=(( struct Cyc_Absyn_TunionType_struct*) _temp2969)->f1; _LL2978:
_temp2977=( void*) _temp2975.tunion_info; if(*(( int*) _temp2977) ==  Cyc_Absyn_KnownTunion){
_LL2980: _temp2979=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2977)->f1;
_temp2981=* _temp2979; goto _LL2972;} else{ goto _LL2973;}} else{ goto _LL2973;}
_LL2973: goto _LL2974; _LL2972: problem= Cyc_Absyn_qvar_cmp( _temp2981->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL2970; _LL2974: problem= 1; goto
_LL2970; _LL2970:;} goto _LL2964; _LL2968:{ void* _temp2982= _temp2962; struct
Cyc_Absyn_TunionInfo _temp2988; void* _temp2990; struct Cyc_Absyn_Tuniondecl**
_temp2992; struct Cyc_Absyn_Tuniondecl* _temp2994; _LL2984: if(( unsigned int)
_temp2982 >  4u?*(( int*) _temp2982) ==  Cyc_Absyn_TunionType: 0){ _LL2989:
_temp2988=(( struct Cyc_Absyn_TunionType_struct*) _temp2982)->f1; _LL2991:
_temp2990=( void*) _temp2988.tunion_info; if(*(( int*) _temp2990) ==  Cyc_Absyn_KnownTunion){
_LL2993: _temp2992=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2990)->f1;
_temp2994=* _temp2992; goto _LL2985;} else{ goto _LL2986;}} else{ goto _LL2986;}
_LL2986: goto _LL2987; _LL2985: problem= Cyc_Absyn_qvar_cmp( _temp2994->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL2983; _LL2987: problem= 1; goto
_LL2983; _LL2983:;} goto _LL2964; _LL2964:;} if( problem){({ void* _temp2995[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp2995, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2682; for( 0; rpo !=  0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp2998; void* _temp2999; void*
_temp3001; struct _tuple8* _temp2996=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp2998=* _temp2996; _LL3002: _temp3001= _temp2998.f1;
goto _LL3000; _LL3000: _temp2999= _temp2998.f2; goto _LL2997; _LL2997: _temp2902=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2902,( void*) Cyc_Absyn_RgnKind,
_temp3001); _temp2902= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2902,( void*)
Cyc_Absyn_RgnKind, _temp2999);}} if(* _temp2694 !=  0){ _temp2902= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2902,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2694))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2902.free_vars; for(
0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void*
_temp3003=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3011; void*
_temp3013; _LL3005: if(( unsigned int) _temp3003 >  1u?*(( int*) _temp3003) == 
Cyc_Absyn_Eq_constr: 0){ _LL3012: _temp3011=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3003)->f1; if( _temp3011 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3006;}
else{ goto _LL3007;}} else{ goto _LL3007;} _LL3007: if(( unsigned int) _temp3003
>  1u?*(( int*) _temp3003) ==  Cyc_Absyn_Eq_constr: 0){ _LL3014: _temp3013=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3003)->f1; if( _temp3013 == (
void*) Cyc_Absyn_EffKind){ goto _LL3008;} else{ goto _LL3009;}} else{ goto
_LL3009;} _LL3009: goto _LL3010; _LL3006: effect=({ struct Cyc_List_List*
_temp3015=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3015->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3016=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3016[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3017; _temp3017.tag= Cyc_Absyn_AccessEff;
_temp3017.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3018=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3018[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3019; _temp3019.tag= Cyc_Absyn_VarType;
_temp3019.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3019;}); _temp3018;})); _temp3017;}); _temp3016;})); _temp3015->tl= effect;
_temp3015;}); goto _LL3004; _LL3008: effect=({ struct Cyc_List_List* _temp3020=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3020->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3021=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3021[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3022; _temp3022.tag= Cyc_Absyn_VarType; _temp3022.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3022;}); _temp3021;})); _temp3020->tl= effect; _temp3020;}); goto _LL3004;
_LL3010: effect=({ struct Cyc_List_List* _temp3023=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3023->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3024=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3024[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3025; _temp3025.tag= Cyc_Absyn_RgnsEff; _temp3025.f1=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3026=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3026[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3027; _temp3027.tag= Cyc_Absyn_VarType; _temp3027.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3027;}); _temp3026;})); _temp3025;}); _temp3024;})); _temp3023->tl= effect;
_temp3023;}); goto _LL3004; _LL3004:;}} effect= Cyc_List_imp_rev( effect);{
struct Cyc_List_List* ts= _temp2902.free_evars; for( 0; ts !=  0; ts=(( struct
Cyc_List_List*) _check_null( ts))->tl){ void* _temp3028= Cyc_Tcutil_typ_kind((
void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _LL3030: if( _temp3028
== ( void*) Cyc_Absyn_RgnKind){ goto _LL3031;} else{ goto _LL3032;} _LL3032: if(
_temp3028 == ( void*) Cyc_Absyn_EffKind){ goto _LL3033;} else{ goto _LL3034;}
_LL3034: goto _LL3035; _LL3031: effect=({ struct Cyc_List_List* _temp3036=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3036->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3037=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3037[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3038; _temp3038.tag= Cyc_Absyn_AccessEff;
_temp3038.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3038;}); _temp3037;})); _temp3036->tl= effect; _temp3036;}); goto _LL3029;
_LL3033: effect=({ struct Cyc_List_List* _temp3039=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3039->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3039->tl= effect; _temp3039;});
goto _LL3029; _LL3035: effect=({ struct Cyc_List_List* _temp3040=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3040->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3041=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3041[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3042; _temp3042.tag= Cyc_Absyn_RgnsEff; _temp3042.f1=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _temp3042;});
_temp3041;})); _temp3040->tl= effect; _temp3040;}); goto _LL3029; _LL3029:;}}*
_temp2694=({ struct Cyc_Core_Opt* _temp3043=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3043->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3044=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3044[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3045; _temp3045.tag= Cyc_Absyn_JoinEff;
_temp3045.f1= Cyc_List_imp_rev( effect); _temp3045;}); _temp3044;})); _temp3043;});}}
if(* _temp2697 !=  0){ struct Cyc_List_List* bs=* _temp2697; for( 0; bs !=  0;
bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3046= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3047=( void*) _temp3046->v; void*
_temp3055; _LL3049: if( _temp3047 == ( void*) Cyc_Absyn_No_constr){ goto _LL3050;}
else{ goto _LL3051;} _LL3051: if(( unsigned int) _temp3047 >  1u?*(( int*)
_temp3047) ==  Cyc_Absyn_Eq_constr: 0){ _LL3056: _temp3055=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3047)->f1; if( _temp3055 == ( void*) Cyc_Absyn_MemKind){ goto _LL3052;}
else{ goto _LL3053;}} else{ goto _LL3053;} _LL3053: goto _LL3054; _LL3050:({
struct Cyc_Std_String_pa_struct _temp3058; _temp3058.tag= Cyc_Std_String_pa;
_temp3058.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp3057[ 1u]={& _temp3058}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3057, sizeof( void*), 1u));}}); goto
_LL3052; _LL3052:( void*)( _temp3046->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3059=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3059[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3060; _temp3060.tag= Cyc_Absyn_Eq_constr;
_temp3060.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3060;}); _temp3059;})));
goto _LL3048; _LL3054: goto _LL3048; _LL3048:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2902.kind_env,* _temp2697); _temp2902.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2902.free_vars,* _temp2697);{ struct Cyc_List_List* tvs= _temp2902.free_vars;
for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2902.free_evars; for( 0; evs !=  0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2584;}}
_LL2610: for( 0; _temp2698 !=  0; _temp2698=(( struct Cyc_List_List*)
_check_null( _temp2698))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp2698))->hd)).f2);} goto _LL2584; _LL2612:{ struct
_RegionHandle _temp3062= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp3062; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2700 !=  0; _temp2700=(( struct Cyc_List_List*) _check_null( _temp2700))->tl){
struct Cyc_Absyn_Structfield _temp3065; struct Cyc_List_List* _temp3066; struct
Cyc_Absyn_Exp* _temp3068; void* _temp3070; struct Cyc_Absyn_Tqual _temp3072;
struct _tagged_arr* _temp3074; struct Cyc_Absyn_Structfield* _temp3063=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2700))->hd;
_temp3065=* _temp3063; _LL3075: _temp3074= _temp3065.name; goto _LL3073; _LL3073:
_temp3072= _temp3065.tq; goto _LL3071; _LL3071: _temp3070=( void*) _temp3065.type;
goto _LL3069; _LL3069: _temp3068= _temp3065.width; goto _LL3067; _LL3067:
_temp3066= _temp3065.attributes; goto _LL3064; _LL3064: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3074)){({ struct Cyc_Std_String_pa_struct _temp3077; _temp3077.tag= Cyc_Std_String_pa;
_temp3077.f1=( struct _tagged_arr)* _temp3074;{ void* _temp3076[ 1u]={&
_temp3077}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3076, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3074, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3078=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3078->hd=( void*)
_temp3074; _temp3078->tl= prev_fields; _temp3078;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3070); Cyc_Tcutil_check_bitfield(
loc, te, _temp3070, _temp3068, _temp3074); Cyc_Tcutil_check_field_atts( loc,
_temp3074, _temp3066);}}; _pop_region( sprev_rgn);} goto _LL2584; _LL2614:{
struct _RegionHandle _temp3079= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3079; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2702 !=  0; _temp2702=(( struct Cyc_List_List*) _check_null( _temp2702))->tl){
struct Cyc_Absyn_Structfield _temp3082; struct Cyc_List_List* _temp3083; struct
Cyc_Absyn_Exp* _temp3085; void* _temp3087; struct Cyc_Absyn_Tqual _temp3089;
struct _tagged_arr* _temp3091; struct Cyc_Absyn_Structfield* _temp3080=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2702))->hd;
_temp3082=* _temp3080; _LL3092: _temp3091= _temp3082.name; goto _LL3090; _LL3090:
_temp3089= _temp3082.tq; goto _LL3088; _LL3088: _temp3087=( void*) _temp3082.type;
goto _LL3086; _LL3086: _temp3085= _temp3082.width; goto _LL3084; _LL3084:
_temp3083= _temp3082.attributes; goto _LL3081; _LL3081: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3091)){({ struct Cyc_Std_String_pa_struct _temp3094; _temp3094.tag= Cyc_Std_String_pa;
_temp3094.f1=( struct _tagged_arr)* _temp3091;{ void* _temp3093[ 1u]={&
_temp3094}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3093, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3091, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3095=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3095->hd=( void*)
_temp3091; _temp3095->tl= prev_fields; _temp3095;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3087); if( ! Cyc_Tcutil_bits_only(
_temp3087)){({ struct Cyc_Std_String_pa_struct _temp3097; _temp3097.tag= Cyc_Std_String_pa;
_temp3097.f1=( struct _tagged_arr)* _temp3091;{ void* _temp3096[ 1u]={&
_temp3097}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3096, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3087, _temp3085, _temp3091); Cyc_Tcutil_check_field_atts(
loc, _temp3091, _temp3083);}}; _pop_region( uprev_rgn);} goto _LL2584; _LL2616:
if( _temp2709 ==  0){ if(* _temp2706 ==  0){({ void* _temp3098[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3098, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2706));;}} if(*
_temp2706 ==  0){ struct _handler_cons _temp3099; _push_handler(& _temp3099);{
int _temp3101= 0; if( setjmp( _temp3099.handler)){ _temp3101= 1;} if( !
_temp3101){* _temp2706=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2709));; _pop_handler();} else{
void* _temp3100=( void*) _exn_thrown; void* _temp3103= _temp3100; _LL3105: if(
_temp3103 ==  Cyc_Dict_Absent){ goto _LL3106;} else{ goto _LL3107;} _LL3107:
goto _LL3108; _LL3106:({ struct Cyc_Std_String_pa_struct _temp3110; _temp3110.tag=
Cyc_Std_String_pa; _temp3110.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2709));{ void* _temp3109[ 1u]={& _temp3110};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type struct %s", sizeof( unsigned char),
23u), _tag_arr( _temp3109, sizeof( void*), 1u));}}); return cvtenv; _LL3108:(
void) _throw( _temp3103); _LL3104:;}}}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2706)); if( sd->name !=  0){*((
struct _tuple1*) _check_null( _temp2709))=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2707); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3114; _temp3114.tag= Cyc_Std_Int_pa; _temp3114.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3113; _temp3113.tag= Cyc_Std_Int_pa;
_temp3113.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3112; _temp3112.tag= Cyc_Std_String_pa; _temp3112.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2709));{ void*
_temp3111[ 3u]={& _temp3112,& _temp3113,& _temp3114}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3111, sizeof( void*), 3u));}}}});} for( 0;
_temp2707 !=  0;( _temp2707=(( struct Cyc_List_List*) _check_null( _temp2707))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2707))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2584;}}
_LL2618: if( _temp2716 ==  0){({ void* _temp3115[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp3115,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2713 ==  0){ struct
_handler_cons _temp3116; _push_handler(& _temp3116);{ int _temp3118= 0; if(
setjmp( _temp3116.handler)){ _temp3118= 1;} if( ! _temp3118){* _temp2713=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2716));; _pop_handler();} else{ void* _temp3117=(
void*) _exn_thrown; void* _temp3120= _temp3117; _LL3122: if( _temp3120 ==  Cyc_Dict_Absent){
goto _LL3123;} else{ goto _LL3124;} _LL3124: goto _LL3125; _LL3123:({ struct Cyc_Std_String_pa_struct
_temp3127; _temp3127.tag= Cyc_Std_String_pa; _temp3127.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2716));{ void*
_temp3126[ 1u]={& _temp3127}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type union %s",
sizeof( unsigned char), 22u), _tag_arr( _temp3126, sizeof( void*), 1u));}});
return cvtenv; _LL3125:( void) _throw( _temp3120); _LL3121:;}}}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(* _temp2713)); if( ud->name != 
0){*(( struct _tuple1*) _check_null( _temp2716))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs= Cyc_List_length( _temp2714); if( lvs != 
largs){({ struct Cyc_Std_Int_pa_struct _temp3131; _temp3131.tag= Cyc_Std_Int_pa;
_temp3131.f1=( int)(( unsigned int) largs);{ struct Cyc_Std_Int_pa_struct
_temp3130; _temp3130.tag= Cyc_Std_Int_pa; _temp3130.f1=( int)(( unsigned int)
lvs);{ struct Cyc_Std_String_pa_struct _temp3129; _temp3129.tag= Cyc_Std_String_pa;
_temp3129.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2716));{ void* _temp3128[ 3u]={& _temp3129,& _temp3130,&
_temp3131}; Cyc_Tcutil_terr( loc, _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u), _tag_arr( _temp3128, sizeof( void*), 3u));}}}});}
for( 0; _temp2714 !=  0;( _temp2714=(( struct Cyc_List_List*) _check_null(
_temp2714))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2714))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2584;}}
_LL2620: { struct Cyc_List_List* targs=* _temp2723; struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3132; _push_handler(& _temp3132);{ int _temp3134=
0; if( setjmp( _temp3132.handler)){ _temp3134= 1;} if( ! _temp3134){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2724);; _pop_handler();} else{ void* _temp3133=( void*)
_exn_thrown; void* _temp3136= _temp3133; _LL3138: if( _temp3136 ==  Cyc_Dict_Absent){
goto _LL3139;} else{ goto _LL3140;} _LL3140: goto _LL3141; _LL3139:({ struct Cyc_Std_String_pa_struct
_temp3143; _temp3143.tag= Cyc_Std_String_pa; _temp3143.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2724);{ void* _temp3142[ 1u]={& _temp3143}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3142, sizeof( void*), 1u));}}); return cvtenv; _LL3141:( void) _throw(
_temp3136); _LL3137:;}}} _temp2724[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3144=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3144->hd=( void*)({ struct
_tuple5* _temp3145=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3145->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3145->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3145;}); _temp3144->tl= inst; _temp3144;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp3147; _temp3147.tag= Cyc_Std_String_pa; _temp3147.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2724);{ void* _temp3146[ 1u]={& _temp3147}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3146, sizeof( void*), 1u));}});} if( tvs !=  0){ struct Cyc_List_List*
hidden_ts= 0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({
struct Cyc_List_List* _temp3148=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3148->hd=( void*) e; _temp3148->tl= hidden_ts;
_temp3148;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k, e);
inst=({ struct Cyc_List_List* _temp3149=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3149->hd=( void*)({ struct _tuple5*
_temp3150=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp3150->f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3150->f2= e; _temp3150;}); _temp3149->tl= inst; _temp3149;});}* _temp2723=
Cyc_List_append( targs, hidden_ts);}{ void* new_typ= Cyc_Tcutil_substitute( inst,(
void*) td->defn);* _temp2720=({ struct Cyc_Core_Opt* _temp3151=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3151->v=( void*) new_typ;
_temp3151;}); goto _LL2584;}}} _LL2622: goto _LL2584; _LL2624: _temp2728=
_temp2726; goto _LL2626; _LL2626: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2728); goto _LL2584; _LL2628: cvtenv=
Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2730); goto _LL2584; _LL2630: for( 0; _temp2732 !=  0; _temp2732=(( struct
Cyc_List_List*) _check_null( _temp2732))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2732))->hd);} goto _LL2584; _LL2584:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){({ struct Cyc_Std_String_pa_struct
_temp3155; _temp3155.tag= Cyc_Std_String_pa; _temp3155.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( expected_kind);{ struct Cyc_Std_String_pa_struct
_temp3154; _temp3154.tag= Cyc_Std_String_pa; _temp3154.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ struct Cyc_Std_String_pa_struct
_temp3153; _temp3153.tag= Cyc_Std_String_pa; _temp3153.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp3152[ 3u]={& _temp3153,& _temp3154,&
_temp3155}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3152, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3156= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3171; _temp3171.kind_env= kind_env; _temp3171.free_vars= 0; _temp3171.free_evars=
0; _temp3171.generalize_evars= generalize_evars; _temp3171.fn_result= 0;
_temp3171;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3156.free_vars;
for( 0; vs !=  0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ _temp3156.kind_env=
Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( vs))->hd);}}{ struct Cyc_List_List* evs= _temp3156.free_evars; for(
0; evs !=  0; evs=(( struct Cyc_List_List*) _check_null( evs))->tl){ void*
_temp3157= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null(
evs))->hd); struct Cyc_Core_Opt* _temp3163; struct Cyc_Core_Opt** _temp3165;
_LL3159: if(( unsigned int) _temp3157 >  4u?*(( int*) _temp3157) ==  Cyc_Absyn_Evar:
0){ _LL3164: _temp3163=(( struct Cyc_Absyn_Evar_struct*) _temp3157)->f4;
_temp3165=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3157)->f4;
goto _LL3160;} else{ goto _LL3161;} _LL3161: goto _LL3162; _LL3160: if(*
_temp3165 ==  0){* _temp3165=({ struct Cyc_Core_Opt* _temp3166=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3166->v=( void*) kind_env;
_temp3166;});} else{ struct Cyc_List_List* _temp3167=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3165))->v; struct Cyc_List_List*
_temp3168= 0; for( 0; _temp3167 !=  0; _temp3167=(( struct Cyc_List_List*)
_check_null( _temp3167))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3167))->hd)){ _temp3168=({ struct Cyc_List_List* _temp3169=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3169->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3167))->hd);
_temp3169->tl= _temp3168; _temp3169;});}}* _temp3165=({ struct Cyc_Core_Opt*
_temp3170=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3170->v=( void*) _temp3168; _temp3170;});} goto _LL3158; _LL3162: goto
_LL3158; _LL3158:;}} return _temp3156;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ int
generalize_evars= Cyc_Tcutil_is_function_type( t); struct Cyc_Tcutil_CVTEnv
_temp3172= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
generalize_evars, t); struct Cyc_List_List* _temp3173= _temp3172.free_vars;
struct Cyc_List_List* _temp3174= _temp3172.free_evars;{ struct Cyc_List_List* x=
_temp3173; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3175=( void*) c->v;
void* _temp3183; _LL3177: if( _temp3175 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3178;} else{ goto _LL3179;} _LL3179: if(( unsigned int) _temp3175 >  1u?*((
int*) _temp3175) ==  Cyc_Absyn_Eq_constr: 0){ _LL3184: _temp3183=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3175)->f1; if( _temp3183 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3180;} else{ goto _LL3181;}} else{ goto _LL3181;}
_LL3181: goto _LL3182; _LL3178: goto _LL3180; _LL3180:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3185=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3185[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3186; _temp3186.tag= Cyc_Absyn_Eq_constr;
_temp3186.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3186;}); _temp3185;})));
goto _LL3176; _LL3182: goto _LL3176; _LL3176:;}} if( _temp3173 !=  0? 1:
_temp3174 !=  0){{ void* _temp3187= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3193; struct Cyc_List_List* _temp3195; struct Cyc_List_List* _temp3197;
struct Cyc_Absyn_VarargInfo* _temp3199; int _temp3201; struct Cyc_List_List*
_temp3203; void* _temp3205; struct Cyc_Core_Opt* _temp3207; struct Cyc_List_List*
_temp3209; struct Cyc_List_List** _temp3211; _LL3189: if(( unsigned int)
_temp3187 >  4u?*(( int*) _temp3187) ==  Cyc_Absyn_FnType: 0){ _LL3194:
_temp3193=(( struct Cyc_Absyn_FnType_struct*) _temp3187)->f1; _LL3210: _temp3209=
_temp3193.tvars; _temp3211=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3187)->f1).tvars; goto _LL3208; _LL3208: _temp3207= _temp3193.effect; goto
_LL3206; _LL3206: _temp3205=( void*) _temp3193.ret_typ; goto _LL3204; _LL3204:
_temp3203= _temp3193.args; goto _LL3202; _LL3202: _temp3201= _temp3193.c_varargs;
goto _LL3200; _LL3200: _temp3199= _temp3193.cyc_varargs; goto _LL3198; _LL3198:
_temp3197= _temp3193.rgn_po; goto _LL3196; _LL3196: _temp3195= _temp3193.attributes;
goto _LL3190;} else{ goto _LL3191;} _LL3191: goto _LL3192; _LL3190: if(*
_temp3211 ==  0){* _temp3211= _temp3173; _temp3173= 0;} goto _LL3188; _LL3192:
goto _LL3188; _LL3188:;} if( _temp3173 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3213; _temp3213.tag= Cyc_Std_String_pa; _temp3213.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3173))->hd)->name;{
void* _temp3212[ 1u]={& _temp3213}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp3212, sizeof( void*), 1u));}});}
if( _temp3174 !=  0){ for( 0; _temp3174 !=  0; _temp3174=(( struct Cyc_List_List*)
_check_null( _temp3174))->tl){ void* e=( void*)(( struct Cyc_List_List*)
_check_null( _temp3174))->hd; void* _temp3214= Cyc_Tcutil_typ_kind( e); _LL3216:
if( _temp3214 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3217;} else{ goto _LL3218;}
_LL3218: if( _temp3214 == ( void*) Cyc_Absyn_EffKind){ goto _LL3219;} else{ goto
_LL3220;} _LL3220: goto _LL3221; _LL3217: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp3222[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp3222, sizeof( void*), 0u));});} goto _LL3215; _LL3219: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp3223[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp3223, sizeof( void*), 0u));});} goto _LL3215; _LL3221:({ struct Cyc_Std_String_pa_struct
_temp3226; _temp3226.tag= Cyc_Std_String_pa; _temp3226.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp3225;
_temp3225.tag= Cyc_Std_String_pa; _temp3225.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp3224[ 2u]={& _temp3225,& _temp3226}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3224, sizeof( void*), 2u));}}}); goto
_LL3215; _LL3215:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3227= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3233; struct Cyc_List_List* _temp3235; struct
Cyc_List_List* _temp3237; struct Cyc_Absyn_VarargInfo* _temp3239; int _temp3241;
struct Cyc_List_List* _temp3243; void* _temp3245; struct Cyc_Core_Opt* _temp3247;
struct Cyc_List_List* _temp3249; _LL3229: if(( unsigned int) _temp3227 >  4u?*((
int*) _temp3227) ==  Cyc_Absyn_FnType: 0){ _LL3234: _temp3233=(( struct Cyc_Absyn_FnType_struct*)
_temp3227)->f1; _LL3250: _temp3249= _temp3233.tvars; goto _LL3248; _LL3248:
_temp3247= _temp3233.effect; goto _LL3246; _LL3246: _temp3245=( void*) _temp3233.ret_typ;
goto _LL3244; _LL3244: _temp3243= _temp3233.args; goto _LL3242; _LL3242:
_temp3241= _temp3233.c_varargs; goto _LL3240; _LL3240: _temp3239= _temp3233.cyc_varargs;
goto _LL3238; _LL3238: _temp3237= _temp3233.rgn_po; goto _LL3236; _LL3236:
_temp3235= _temp3233.attributes; goto _LL3230;} else{ goto _LL3231;} _LL3231:
goto _LL3232; _LL3230: fd->tvs= _temp3249; fd->effect= _temp3247; goto _LL3228;
_LL3232:({ void* _temp3251[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3251, sizeof( void*), 0u));});
return; _LL3228:;}{ struct _RegionHandle _temp3252= _new_region(); struct
_RegionHandle* r=& _temp3252; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3253=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3253->v=(
void*) t; _temp3253;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3254=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3255= Cyc_Tcutil_remove_bound_tvars( _temp3254.free_vars,
bound_tvars); struct Cyc_List_List* _temp3256= _temp3254.free_evars;{ struct Cyc_List_List*
fs= _temp3255; for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3257=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name;({ struct Cyc_Std_String_pa_struct _temp3260;
_temp3260.tag= Cyc_Std_String_pa; _temp3260.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp3259; _temp3259.tag= Cyc_Std_String_pa;
_temp3259.f1=( struct _tagged_arr)* _temp3257;{ void* _temp3258[ 2u]={&
_temp3259,& _temp3260}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3258, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3256 !=  0: 0){ for( 0; _temp3256 !=  0; _temp3256=((
struct Cyc_List_List*) _check_null( _temp3256))->tl){ void* e=( void*)(( struct
Cyc_List_List*) _check_null( _temp3256))->hd; void* _temp3261= Cyc_Tcutil_typ_kind(
e); _LL3263: if( _temp3261 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3264;} else{
goto _LL3265;} _LL3265: if( _temp3261 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3266;} else{ goto _LL3267;} _LL3267: goto _LL3268; _LL3264: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3269[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3269, sizeof( void*), 0u));});}
goto _LL3262; _LL3266: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3270=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3270[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3271; _temp3271.tag= Cyc_Absyn_JoinEff;
_temp3271.f1= 0; _temp3271;}); _temp3270;}))){({ void* _temp3272[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3272, sizeof( void*), 0u));});}
goto _LL3262; _LL3268:({ struct Cyc_Std_String_pa_struct _temp3275; _temp3275.tag=
Cyc_Std_String_pa; _temp3275.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp3274; _temp3274.tag= Cyc_Std_String_pa;
_temp3274.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3273[
2u]={& _temp3274,& _temp3275}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3273, sizeof( void*), 2u));}}});
goto _LL3262; _LL3262:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
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
struct Cyc_Std_String_pa_struct _temp3278; _temp3278.tag= Cyc_Std_String_pa;
_temp3278.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_Std_String_pa_struct _temp3277; _temp3277.tag=
Cyc_Std_String_pa; _temp3277.f1=( struct _tagged_arr) msg;{ void* _temp3276[ 2u]={&
_temp3277,& _temp3278}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3276, sizeof( void*), 2u));}}});}}}} static
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
unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List* _temp3279=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3279->hd=( void*)({
struct _tuple17* _temp3280=( struct _tuple17*) _cycalloc( sizeof( struct
_tuple17)); _temp3280->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3280->f2= 0; _temp3280;}); _temp3279->tl=
fields; _temp3279;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3283;
void* _temp3284; struct Cyc_List_List* _temp3286; struct _tuple18* _temp3281=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3283=*
_temp3281; _LL3287: _temp3286= _temp3283.f1; goto _LL3285; _LL3285: _temp3284=
_temp3283.f2; goto _LL3282; _LL3282: if( _temp3286 ==  0){ struct Cyc_List_List*
_temp3288= fields; for( 0; _temp3288 !=  0; _temp3288=(( struct Cyc_List_List*)
_check_null( _temp3288))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3288))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3288))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3289=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3289->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3290=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3290[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3291; _temp3291.tag= Cyc_Absyn_FieldName;
_temp3291.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3288))->hd)).f1)->name; _temp3291;}); _temp3290;})); _temp3289->tl= 0;
_temp3289;}); ans=({ struct Cyc_List_List* _temp3292=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3292->hd=( void*)({
struct _tuple19* _temp3293=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3293->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3288))->hd)).f1; _temp3293->f2= _temp3284; _temp3293;});
_temp3292->tl= ans; _temp3292;}); break;}} if( _temp3288 ==  0){({ void*
_temp3294[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3294, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3286))->tl !=  0){({ void*
_temp3295[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3295, sizeof( void*), 0u));});}
else{ void* _temp3296=( void*)(( struct Cyc_List_List*) _check_null( _temp3286))->hd;
struct _tagged_arr* _temp3302; _LL3298: if(*(( int*) _temp3296) ==  Cyc_Absyn_ArrayElement){
goto _LL3299;} else{ goto _LL3300;} _LL3300: if(*(( int*) _temp3296) ==  Cyc_Absyn_FieldName){
_LL3303: _temp3302=(( struct Cyc_Absyn_FieldName_struct*) _temp3296)->f1; goto
_LL3301;} else{ goto _LL3297;} _LL3299:({ void* _temp3304[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3304, sizeof( void*), 0u));}); goto _LL3297;
_LL3301: { struct Cyc_List_List* _temp3305= fields; for( 0; _temp3305 !=  0;
_temp3305=(( struct Cyc_List_List*) _check_null( _temp3305))->tl){ if( Cyc_Std_zstrptrcmp(
_temp3302,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3305))->hd)).f1)->name) ==  0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3305))->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp3307;
_temp3307.tag= Cyc_Std_String_pa; _temp3307.f1=( struct _tagged_arr)* _temp3302;{
void* _temp3306[ 1u]={& _temp3307}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3306, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3305))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3308=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3308->hd=( void*)({ struct _tuple19*
_temp3309=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3309->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3305))->hd)).f1; _temp3309->f2= _temp3284; _temp3309;}); _temp3308->tl= ans;
_temp3308;}); break;}} if( _temp3305 ==  0){({ struct Cyc_Std_String_pa_struct
_temp3311; _temp3311.tag= Cyc_Std_String_pa; _temp3311.f1=( struct _tagged_arr)*
_temp3302;{ void* _temp3310[ 1u]={& _temp3311}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3310, sizeof( void*), 1u));}});}
goto _LL3297;} _LL3297:;}}} for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3312[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3312, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3313= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3319; struct Cyc_Absyn_Conref* _temp3321; void* _temp3323;
_LL3315: if(( unsigned int) _temp3313 >  4u?*(( int*) _temp3313) ==  Cyc_Absyn_PointerType:
0){ _LL3320: _temp3319=(( struct Cyc_Absyn_PointerType_struct*) _temp3313)->f1;
_LL3324: _temp3323=( void*) _temp3319.elt_typ; goto _LL3322; _LL3322: _temp3321=
_temp3319.bounds; goto _LL3316;} else{ goto _LL3317;} _LL3317: goto _LL3318;
_LL3316: { struct Cyc_Absyn_Conref* _temp3325= Cyc_Absyn_compress_conref(
_temp3321); void* _temp3326=( void*)( Cyc_Absyn_compress_conref( _temp3325))->v;
void* _temp3334; _LL3328: if(( unsigned int) _temp3326 >  1u?*(( int*) _temp3326)
==  Cyc_Absyn_Eq_constr: 0){ _LL3335: _temp3334=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3326)->f1; if( _temp3334 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3329;}
else{ goto _LL3330;}} else{ goto _LL3330;} _LL3330: if( _temp3326 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3331;} else{ goto _LL3332;} _LL3332: goto _LL3333;
_LL3329:* elt_typ_dest= _temp3323; return 1; _LL3331:( void*)( _temp3325->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3336=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3336[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3337; _temp3337.tag= Cyc_Absyn_Eq_constr;
_temp3337.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3337;}); _temp3336;})));*
elt_typ_dest= _temp3323; return 1; _LL3333: return 0; _LL3327:;} _LL3318: return
0; _LL3314:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3520;
_temp3520.f1= 0; _temp3520.f2=( void*) Cyc_Absyn_HeapRgn; _temp3520;}); void*
_temp3338=( void*) e->r; void* _temp3352; struct _tuple1* _temp3354; struct
_tagged_arr* _temp3356; struct Cyc_Absyn_Exp* _temp3358; struct _tagged_arr*
_temp3360; struct Cyc_Absyn_Exp* _temp3362; struct Cyc_Absyn_Exp* _temp3364;
struct Cyc_Absyn_Exp* _temp3366; struct Cyc_Absyn_Exp* _temp3368; _LL3340: if(*((
int*) _temp3338) ==  Cyc_Absyn_Var_e){ _LL3355: _temp3354=(( struct Cyc_Absyn_Var_e_struct*)
_temp3338)->f1; goto _LL3353; _LL3353: _temp3352=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3338)->f2; goto _LL3341;} else{ goto _LL3342;} _LL3342: if(*(( int*)
_temp3338) ==  Cyc_Absyn_StructMember_e){ _LL3359: _temp3358=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3338)->f1; goto _LL3357; _LL3357: _temp3356=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3338)->f2; goto _LL3343;} else{ goto _LL3344;} _LL3344: if(*(( int*)
_temp3338) ==  Cyc_Absyn_StructArrow_e){ _LL3363: _temp3362=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3338)->f1; goto _LL3361; _LL3361: _temp3360=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3338)->f2; goto _LL3345;} else{ goto _LL3346;} _LL3346: if(*(( int*)
_temp3338) ==  Cyc_Absyn_Deref_e){ _LL3365: _temp3364=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3338)->f1; goto _LL3347;} else{ goto _LL3348;} _LL3348: if(*(( int*)
_temp3338) ==  Cyc_Absyn_Subscript_e){ _LL3369: _temp3368=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3338)->f1; goto _LL3367; _LL3367: _temp3366=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3338)->f2; goto _LL3349;} else{ goto _LL3350;} _LL3350: goto _LL3351;
_LL3341: { void* _temp3370= _temp3352; struct Cyc_Absyn_Vardecl* _temp3384;
struct Cyc_Absyn_Vardecl* _temp3386; struct Cyc_Absyn_Vardecl* _temp3388; struct
Cyc_Absyn_Vardecl* _temp3390; _LL3372: if( _temp3370 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3373;} else{ goto _LL3374;} _LL3374: if(( unsigned int) _temp3370 >  1u?*((
int*) _temp3370) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3375;} else{ goto _LL3376;}
_LL3376: if(( unsigned int) _temp3370 >  1u?*(( int*) _temp3370) ==  Cyc_Absyn_Global_b:
0){ _LL3385: _temp3384=(( struct Cyc_Absyn_Global_b_struct*) _temp3370)->f1;
goto _LL3377;} else{ goto _LL3378;} _LL3378: if(( unsigned int) _temp3370 >  1u?*((
int*) _temp3370) ==  Cyc_Absyn_Local_b: 0){ _LL3387: _temp3386=(( struct Cyc_Absyn_Local_b_struct*)
_temp3370)->f1; goto _LL3379;} else{ goto _LL3380;} _LL3380: if(( unsigned int)
_temp3370 >  1u?*(( int*) _temp3370) ==  Cyc_Absyn_Pat_b: 0){ _LL3389: _temp3388=((
struct Cyc_Absyn_Pat_b_struct*) _temp3370)->f1; goto _LL3381;} else{ goto
_LL3382;} _LL3382: if(( unsigned int) _temp3370 >  1u?*(( int*) _temp3370) == 
Cyc_Absyn_Param_b: 0){ _LL3391: _temp3390=(( struct Cyc_Absyn_Param_b_struct*)
_temp3370)->f1; goto _LL3383;} else{ goto _LL3371;} _LL3373: return bogus_ans;
_LL3375: return({ struct _tuple7 _temp3392; _temp3392.f1= 0; _temp3392.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3392;}); _LL3377: { void* _temp3393= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3395: if((
unsigned int) _temp3393 >  4u?*(( int*) _temp3393) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3396;} else{ goto _LL3397;} _LL3397: goto _LL3398; _LL3396: return({
struct _tuple7 _temp3399; _temp3399.f1= 1; _temp3399.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3399;}); _LL3398: return({ struct _tuple7 _temp3400; _temp3400.f1=(
_temp3384->tq).q_const; _temp3400.f2=( void*) Cyc_Absyn_HeapRgn; _temp3400;});
_LL3394:;} _LL3379: { void* _temp3401= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3403: if(( unsigned int) _temp3401 >  4u?*(( int*)
_temp3401) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3404;} else{ goto _LL3405;}
_LL3405: goto _LL3406; _LL3404: return({ struct _tuple7 _temp3407; _temp3407.f1=
1; _temp3407.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3386->rgn))->v;
_temp3407;}); _LL3406: return({ struct _tuple7 _temp3408; _temp3408.f1=(
_temp3386->tq).q_const; _temp3408.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3386->rgn))->v; _temp3408;}); _LL3402:;} _LL3381: _temp3390=
_temp3388; goto _LL3383; _LL3383: return({ struct _tuple7 _temp3409; _temp3409.f1=(
_temp3390->tq).q_const; _temp3409.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3390->rgn))->v; _temp3409;}); _LL3371:;} _LL3343: { void*
_temp3410= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3358->topt))->v); struct Cyc_List_List* _temp3422; struct Cyc_List_List*
_temp3424; struct Cyc_Absyn_Structdecl** _temp3426; struct Cyc_Absyn_Structdecl*
_temp3428; struct Cyc_Absyn_Uniondecl** _temp3429; struct Cyc_Absyn_Uniondecl*
_temp3431; _LL3412: if(( unsigned int) _temp3410 >  4u?*(( int*) _temp3410) == 
Cyc_Absyn_AnonStructType: 0){ _LL3423: _temp3422=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3410)->f1; goto _LL3413;} else{ goto _LL3414;} _LL3414: if(( unsigned int)
_temp3410 >  4u?*(( int*) _temp3410) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3425:
_temp3424=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3410)->f1; goto
_LL3415;} else{ goto _LL3416;} _LL3416: if(( unsigned int) _temp3410 >  4u?*((
int*) _temp3410) ==  Cyc_Absyn_StructType: 0){ _LL3427: _temp3426=(( struct Cyc_Absyn_StructType_struct*)
_temp3410)->f3; if( _temp3426 ==  0){ goto _LL3418;} else{ _temp3428=* _temp3426;
goto _LL3417;}} else{ goto _LL3418;} _LL3418: if(( unsigned int) _temp3410 >  4u?*((
int*) _temp3410) ==  Cyc_Absyn_UnionType: 0){ _LL3430: _temp3429=(( struct Cyc_Absyn_UnionType_struct*)
_temp3410)->f3; if( _temp3429 ==  0){ goto _LL3420;} else{ _temp3431=* _temp3429;
goto _LL3419;}} else{ goto _LL3420;} _LL3420: goto _LL3421; _LL3413: _temp3424=
_temp3422; goto _LL3415; _LL3415: { struct Cyc_Absyn_Structfield* _temp3432= Cyc_Absyn_lookup_field(
_temp3424, _temp3356); if( _temp3432 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3432))->width !=  0: 0){ return({ struct _tuple7 _temp3433;
_temp3433.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3432))->tq).q_const;
_temp3433.f2=( Cyc_Tcutil_addressof_props( te, _temp3358)).f2; _temp3433;});}
return bogus_ans;} _LL3417: { struct Cyc_Absyn_Structfield* _temp3434= Cyc_Absyn_lookup_struct_field(
_temp3428, _temp3356); if( _temp3434 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3434))->width !=  0: 0){ return({ struct _tuple7 _temp3435;
_temp3435.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3434))->tq).q_const;
_temp3435.f2=( Cyc_Tcutil_addressof_props( te, _temp3358)).f2; _temp3435;});}
return bogus_ans;} _LL3419: { struct Cyc_Absyn_Structfield* _temp3436= Cyc_Absyn_lookup_union_field(
_temp3431, _temp3356); if( _temp3436 !=  0){ return({ struct _tuple7 _temp3437;
_temp3437.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3436))->tq).q_const;
_temp3437.f2=( Cyc_Tcutil_addressof_props( te, _temp3358)).f2; _temp3437;});}
goto _LL3421;} _LL3421: return bogus_ans; _LL3411:;} _LL3345: { void* _temp3438=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3362->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3444; void* _temp3446; void* _temp3448; _LL3440:
if(( unsigned int) _temp3438 >  4u?*(( int*) _temp3438) ==  Cyc_Absyn_PointerType:
0){ _LL3445: _temp3444=(( struct Cyc_Absyn_PointerType_struct*) _temp3438)->f1;
_LL3449: _temp3448=( void*) _temp3444.elt_typ; goto _LL3447; _LL3447: _temp3446=(
void*) _temp3444.rgn_typ; goto _LL3441;} else{ goto _LL3442;} _LL3442: goto
_LL3443; _LL3441: { void* _temp3450= Cyc_Tcutil_compress( _temp3448); struct Cyc_List_List*
_temp3462; struct Cyc_List_List* _temp3464; struct Cyc_Absyn_Structdecl**
_temp3466; struct Cyc_Absyn_Structdecl* _temp3468; struct Cyc_Absyn_Uniondecl**
_temp3469; struct Cyc_Absyn_Uniondecl* _temp3471; _LL3452: if(( unsigned int)
_temp3450 >  4u?*(( int*) _temp3450) ==  Cyc_Absyn_AnonStructType: 0){ _LL3463:
_temp3462=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3450)->f1; goto
_LL3453;} else{ goto _LL3454;} _LL3454: if(( unsigned int) _temp3450 >  4u?*((
int*) _temp3450) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3465: _temp3464=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3450)->f1; goto _LL3455;} else{ goto
_LL3456;} _LL3456: if(( unsigned int) _temp3450 >  4u?*(( int*) _temp3450) == 
Cyc_Absyn_StructType: 0){ _LL3467: _temp3466=(( struct Cyc_Absyn_StructType_struct*)
_temp3450)->f3; if( _temp3466 ==  0){ goto _LL3458;} else{ _temp3468=* _temp3466;
goto _LL3457;}} else{ goto _LL3458;} _LL3458: if(( unsigned int) _temp3450 >  4u?*((
int*) _temp3450) ==  Cyc_Absyn_UnionType: 0){ _LL3470: _temp3469=(( struct Cyc_Absyn_UnionType_struct*)
_temp3450)->f3; if( _temp3469 ==  0){ goto _LL3460;} else{ _temp3471=* _temp3469;
goto _LL3459;}} else{ goto _LL3460;} _LL3460: goto _LL3461; _LL3453: _temp3464=
_temp3462; goto _LL3455; _LL3455: { struct Cyc_Absyn_Structfield* _temp3472= Cyc_Absyn_lookup_field(
_temp3464, _temp3360); if( _temp3472 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3472))->width !=  0: 0){ return({ struct _tuple7 _temp3473;
_temp3473.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3472))->tq).q_const;
_temp3473.f2= _temp3446; _temp3473;});} return bogus_ans;} _LL3457: { struct Cyc_Absyn_Structfield*
_temp3474= Cyc_Absyn_lookup_struct_field( _temp3468, _temp3360); if( _temp3474
!=  0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3474))->width !=  0: 0){
return({ struct _tuple7 _temp3475; _temp3475.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3474))->tq).q_const; _temp3475.f2= _temp3446; _temp3475;});}
return bogus_ans;} _LL3459: { struct Cyc_Absyn_Structfield* _temp3476= Cyc_Absyn_lookup_union_field(
_temp3471, _temp3360); if( _temp3476 !=  0){ return({ struct _tuple7 _temp3477;
_temp3477.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3476))->tq).q_const;
_temp3477.f2= _temp3446; _temp3477;});} return bogus_ans;} _LL3461: return
bogus_ans; _LL3451:;} _LL3443: return bogus_ans; _LL3439:;} _LL3347: { void*
_temp3478= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3364->topt))->v); struct Cyc_Absyn_PtrInfo _temp3484; struct Cyc_Absyn_Tqual
_temp3486; void* _temp3488; _LL3480: if(( unsigned int) _temp3478 >  4u?*(( int*)
_temp3478) ==  Cyc_Absyn_PointerType: 0){ _LL3485: _temp3484=(( struct Cyc_Absyn_PointerType_struct*)
_temp3478)->f1; _LL3489: _temp3488=( void*) _temp3484.rgn_typ; goto _LL3487;
_LL3487: _temp3486= _temp3484.tq; goto _LL3481;} else{ goto _LL3482;} _LL3482:
goto _LL3483; _LL3481: return({ struct _tuple7 _temp3490; _temp3490.f1=
_temp3486.q_const; _temp3490.f2= _temp3488; _temp3490;}); _LL3483: return
bogus_ans; _LL3479:;} _LL3349: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3368->topt))->v); void* _temp3491= t; struct
Cyc_Absyn_Tqual _temp3501; struct Cyc_List_List* _temp3503; struct Cyc_Absyn_PtrInfo
_temp3505; struct Cyc_Absyn_Conref* _temp3507; struct Cyc_Absyn_Tqual _temp3509;
void* _temp3511; void* _temp3513; _LL3493: if(( unsigned int) _temp3491 >  4u?*((
int*) _temp3491) ==  Cyc_Absyn_ArrayType: 0){ _LL3502: _temp3501=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3491)->f2; goto _LL3494;} else{ goto _LL3495;} _LL3495: if(( unsigned int)
_temp3491 >  4u?*(( int*) _temp3491) ==  Cyc_Absyn_TupleType: 0){ _LL3504:
_temp3503=(( struct Cyc_Absyn_TupleType_struct*) _temp3491)->f1; goto _LL3496;}
else{ goto _LL3497;} _LL3497: if(( unsigned int) _temp3491 >  4u?*(( int*)
_temp3491) ==  Cyc_Absyn_PointerType: 0){ _LL3506: _temp3505=(( struct Cyc_Absyn_PointerType_struct*)
_temp3491)->f1; _LL3514: _temp3513=( void*) _temp3505.elt_typ; goto _LL3512;
_LL3512: _temp3511=( void*) _temp3505.rgn_typ; goto _LL3510; _LL3510: _temp3509=
_temp3505.tq; goto _LL3508; _LL3508: _temp3507= _temp3505.bounds; goto _LL3498;}
else{ goto _LL3499;} _LL3499: goto _LL3500; _LL3494: return({ struct _tuple7
_temp3515; _temp3515.f1= _temp3501.q_const; _temp3515.f2=( Cyc_Tcutil_addressof_props(
te, _temp3368)).f2; _temp3515;}); _LL3496: { struct _tuple4* _temp3516= Cyc_Absyn_lookup_tuple_field(
_temp3503,( int) Cyc_Evexp_eval_const_uint_exp( _temp3366)); if( _temp3516 !=  0){
return({ struct _tuple7 _temp3517; _temp3517.f1=((*(( struct _tuple4*)
_check_null( _temp3516))).f1).q_const; _temp3517.f2=( Cyc_Tcutil_addressof_props(
te, _temp3368)).f2; _temp3517;});} return bogus_ans;} _LL3498: return({ struct
_tuple7 _temp3518; _temp3518.f1= _temp3509.q_const; _temp3518.f2= _temp3511;
_temp3518;}); _LL3500: return bogus_ans; _LL3492:;} _LL3351:({ void* _temp3519[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3519, sizeof( void*), 0u));});
return bogus_ans; _LL3339:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3521= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3527; void* _temp3529; _LL3523: if((
unsigned int) _temp3521 >  4u?*(( int*) _temp3521) ==  Cyc_Absyn_ArrayType: 0){
_LL3530: _temp3529=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3521)->f1;
goto _LL3528; _LL3528: _temp3527=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3521)->f2; goto _LL3524;} else{ goto _LL3525;} _LL3525: goto _LL3526;
_LL3524: { void* _temp3533; struct _tuple7 _temp3531= Cyc_Tcutil_addressof_props(
te, e); _LL3534: _temp3533= _temp3531.f2; goto _LL3532; _LL3532: return Cyc_Absyn_atb_typ(
_temp3529, _temp3533, _temp3527,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3535=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3535[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3536; _temp3536.tag= Cyc_Absyn_Upper_b;
_temp3536.f1= e; _temp3536;}); _temp3535;}));} _LL3526: return e_typ; _LL3522:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3537=(
void*) b->v; void* _temp3545; void* _temp3547; struct Cyc_Absyn_Exp* _temp3549;
_LL3539: if(( unsigned int) _temp3537 >  1u?*(( int*) _temp3537) ==  Cyc_Absyn_Eq_constr:
0){ _LL3546: _temp3545=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3537)->f1;
if( _temp3545 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3540;} else{ goto
_LL3541;}} else{ goto _LL3541;} _LL3541: if(( unsigned int) _temp3537 >  1u?*((
int*) _temp3537) ==  Cyc_Absyn_Eq_constr: 0){ _LL3548: _temp3547=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3537)->f1; if(( unsigned int) _temp3547
>  1u?*(( int*) _temp3547) ==  Cyc_Absyn_Upper_b: 0){ _LL3550: _temp3549=((
struct Cyc_Absyn_Upper_b_struct*) _temp3547)->f1; goto _LL3542;} else{ goto
_LL3543;}} else{ goto _LL3543;} _LL3543: goto _LL3544; _LL3540: return; _LL3542:
if( Cyc_Evexp_eval_const_uint_exp( _temp3549) <=  i){({ void* _temp3551[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3551, sizeof( void*), 0u));});} return;
_LL3544:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3552=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3552[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3553; _temp3553.tag= Cyc_Absyn_Eq_constr;
_temp3553.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3554=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3554[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3555; _temp3555.tag= Cyc_Absyn_Upper_b;
_temp3555.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3555;}); _temp3554;}));
_temp3553;}); _temp3552;}))); return; _LL3538:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp3556=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp3562; struct
Cyc_Absyn_Exp* _temp3564; _LL3558: if(( unsigned int) _temp3556 >  1u?*(( int*)
_temp3556) ==  Cyc_Absyn_Eq_constr: 0){ _LL3563: _temp3562=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3556)->f1; if(( unsigned int) _temp3562 >  1u?*(( int*) _temp3562) ==  Cyc_Absyn_Upper_b:
0){ _LL3565: _temp3564=(( struct Cyc_Absyn_Upper_b_struct*) _temp3562)->f1; goto
_LL3559;} else{ goto _LL3560;}} else{ goto _LL3560;} _LL3560: goto _LL3561;
_LL3559: return Cyc_Evexp_eval_const_uint_exp( _temp3564) ==  1; _LL3561: return
0; _LL3557:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp3566= Cyc_Tcutil_compress(
t); void* _temp3592; struct Cyc_List_List* _temp3594; struct Cyc_Absyn_Structdecl**
_temp3596; struct Cyc_List_List* _temp3598; struct Cyc_Absyn_Uniondecl**
_temp3600; struct Cyc_List_List* _temp3602; struct Cyc_List_List* _temp3604;
struct Cyc_List_List* _temp3606; _LL3568: if( _temp3566 == ( void*) Cyc_Absyn_VoidType){
goto _LL3569;} else{ goto _LL3570;} _LL3570: if(( unsigned int) _temp3566 >  4u?*((
int*) _temp3566) ==  Cyc_Absyn_IntType: 0){ goto _LL3571;} else{ goto _LL3572;}
_LL3572: if( _temp3566 == ( void*) Cyc_Absyn_FloatType){ goto _LL3573;} else{
goto _LL3574;} _LL3574: if( _temp3566 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3575;} else{ goto _LL3576;} _LL3576: if(( unsigned int) _temp3566 >  4u?*((
int*) _temp3566) ==  Cyc_Absyn_EnumType: 0){ goto _LL3577;} else{ goto _LL3578;}
_LL3578: if(( unsigned int) _temp3566 >  4u?*(( int*) _temp3566) ==  Cyc_Absyn_ArrayType:
0){ _LL3593: _temp3592=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3566)->f1;
goto _LL3579;} else{ goto _LL3580;} _LL3580: if(( unsigned int) _temp3566 >  4u?*((
int*) _temp3566) ==  Cyc_Absyn_TupleType: 0){ _LL3595: _temp3594=(( struct Cyc_Absyn_TupleType_struct*)
_temp3566)->f1; goto _LL3581;} else{ goto _LL3582;} _LL3582: if(( unsigned int)
_temp3566 >  4u?*(( int*) _temp3566) ==  Cyc_Absyn_StructType: 0){ _LL3599:
_temp3598=(( struct Cyc_Absyn_StructType_struct*) _temp3566)->f2; goto _LL3597;
_LL3597: _temp3596=(( struct Cyc_Absyn_StructType_struct*) _temp3566)->f3; goto
_LL3583;} else{ goto _LL3584;} _LL3584: if(( unsigned int) _temp3566 >  4u?*((
int*) _temp3566) ==  Cyc_Absyn_UnionType: 0){ _LL3603: _temp3602=(( struct Cyc_Absyn_UnionType_struct*)
_temp3566)->f2; goto _LL3601; _LL3601: _temp3600=(( struct Cyc_Absyn_UnionType_struct*)
_temp3566)->f3; goto _LL3585;} else{ goto _LL3586;} _LL3586: if(( unsigned int)
_temp3566 >  4u?*(( int*) _temp3566) ==  Cyc_Absyn_AnonStructType: 0){ _LL3605:
_temp3604=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3566)->f1; goto
_LL3587;} else{ goto _LL3588;} _LL3588: if(( unsigned int) _temp3566 >  4u?*((
int*) _temp3566) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3607: _temp3606=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3566)->f1; goto _LL3589;} else{ goto
_LL3590;} _LL3590: goto _LL3591; _LL3569: goto _LL3571; _LL3571: goto _LL3573;
_LL3573: goto _LL3575; _LL3575: return 1; _LL3577: return 0; _LL3579: return Cyc_Tcutil_bits_only(
_temp3592); _LL3581: for( 0; _temp3594 !=  0; _temp3594=(( struct Cyc_List_List*)
_check_null( _temp3594))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3594))->hd)).f2)){ return 0;}} return 1;
_LL3583: if( _temp3596 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp3608=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp3596)); if(
_temp3608->fields ==  0){ return 0;}{ struct _RegionHandle _temp3609=
_new_region(); struct _RegionHandle* rgn=& _temp3609; _push_region( rgn);{
struct Cyc_List_List* _temp3610=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3608->tvs, _temp3598);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp3608->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp3610,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ int _temp3611= 0;
_npop_handler( 0u); return _temp3611;}}}{ int _temp3612= 1; _npop_handler( 0u);
return _temp3612;}}; _pop_region( rgn);}} _LL3585: if( _temp3600 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3613=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3600)); if( _temp3613->fields ==  0){ return 0;}{ struct
_RegionHandle _temp3614= _new_region(); struct _RegionHandle* rgn=& _temp3614;
_push_region( rgn);{ struct Cyc_List_List* _temp3615=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3613->tvs, _temp3602);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3613->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3615,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3616= 0; _npop_handler( 0u); return
_temp3616;}}}{ int _temp3617= 1; _npop_handler( 0u); return _temp3617;}};
_pop_region( rgn);}} _LL3587: _temp3606= _temp3604; goto _LL3589; _LL3589: for(
0; _temp3606 !=  0; _temp3606=(( struct Cyc_List_List*) _check_null( _temp3606))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3606))->hd)->type)){ return 0;}} return 1;
_LL3591: return 0; _LL3567:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3618=( void*) e->r; struct
_tuple1* _temp3662; struct Cyc_Absyn_Exp* _temp3664; struct Cyc_Absyn_Exp*
_temp3666; struct Cyc_Absyn_Exp* _temp3668; struct Cyc_Absyn_Exp* _temp3670;
struct Cyc_Absyn_Exp* _temp3672; struct Cyc_Absyn_Exp* _temp3674; struct Cyc_Absyn_Exp*
_temp3676; struct Cyc_Absyn_Exp* _temp3678; void* _temp3680; struct Cyc_Absyn_Exp*
_temp3682; struct Cyc_Absyn_Exp* _temp3684; struct Cyc_Absyn_Exp* _temp3686;
struct Cyc_List_List* _temp3688; struct Cyc_List_List* _temp3690; struct Cyc_List_List*
_temp3692; struct Cyc_List_List* _temp3694; void* _temp3696; struct Cyc_List_List*
_temp3698; struct Cyc_List_List* _temp3700; _LL3620: if(*(( int*) _temp3618) == 
Cyc_Absyn_Const_e){ goto _LL3621;} else{ goto _LL3622;} _LL3622: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3623;} else{ goto _LL3624;}
_LL3624: if(*(( int*) _temp3618) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3625;}
else{ goto _LL3626;} _LL3626: if(*(( int*) _temp3618) ==  Cyc_Absyn_Offsetof_e){
goto _LL3627;} else{ goto _LL3628;} _LL3628: if(*(( int*) _temp3618) ==  Cyc_Absyn_Gentyp_e){
goto _LL3629;} else{ goto _LL3630;} _LL3630: if(*(( int*) _temp3618) ==  Cyc_Absyn_Enum_e){
goto _LL3631;} else{ goto _LL3632;} _LL3632: if(*(( int*) _temp3618) ==  Cyc_Absyn_Var_e){
_LL3663: _temp3662=(( struct Cyc_Absyn_Var_e_struct*) _temp3618)->f1; goto
_LL3633;} else{ goto _LL3634;} _LL3634: if(*(( int*) _temp3618) ==  Cyc_Absyn_Conditional_e){
_LL3669: _temp3668=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3618)->f1;
goto _LL3667; _LL3667: _temp3666=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3618)->f2; goto _LL3665; _LL3665: _temp3664=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3618)->f3; goto _LL3635;} else{ goto _LL3636;} _LL3636: if(*(( int*)
_temp3618) ==  Cyc_Absyn_SeqExp_e){ _LL3673: _temp3672=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3618)->f1; goto _LL3671; _LL3671: _temp3670=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3618)->f2; goto _LL3637;} else{ goto _LL3638;} _LL3638: if(*(( int*)
_temp3618) ==  Cyc_Absyn_NoInstantiate_e){ _LL3675: _temp3674=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3618)->f1; goto _LL3639;} else{ goto _LL3640;} _LL3640: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Instantiate_e){ _LL3677: _temp3676=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3618)->f1; goto _LL3641;} else{ goto _LL3642;} _LL3642: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Cast_e){ _LL3681: _temp3680=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3618)->f1; goto _LL3679; _LL3679: _temp3678=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3618)->f2; goto _LL3643;} else{ goto _LL3644;} _LL3644: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Address_e){ _LL3683: _temp3682=(( struct Cyc_Absyn_Address_e_struct*)
_temp3618)->f1; goto _LL3645;} else{ goto _LL3646;} _LL3646: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Comprehension_e){ _LL3687: _temp3686=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3618)->f2; goto _LL3685; _LL3685: _temp3684=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3618)->f3; goto _LL3647;} else{ goto _LL3648;} _LL3648: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Array_e){ _LL3689: _temp3688=(( struct Cyc_Absyn_Array_e_struct*)
_temp3618)->f1; goto _LL3649;} else{ goto _LL3650;} _LL3650: if(*(( int*)
_temp3618) ==  Cyc_Absyn_AnonStruct_e){ _LL3691: _temp3690=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3618)->f2; goto _LL3651;} else{ goto _LL3652;} _LL3652: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Struct_e){ _LL3693: _temp3692=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3618)->f3; goto _LL3653;} else{ goto _LL3654;} _LL3654: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Primop_e){ _LL3697: _temp3696=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3618)->f1; goto _LL3695; _LL3695: _temp3694=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3618)->f2; goto _LL3655;} else{ goto _LL3656;} _LL3656: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Tuple_e){ _LL3699: _temp3698=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3618)->f1; goto _LL3657;} else{ goto _LL3658;} _LL3658: if(*(( int*)
_temp3618) ==  Cyc_Absyn_Tunion_e){ _LL3701: _temp3700=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3618)->f3; goto _LL3659;} else{ goto _LL3660;} _LL3660: goto _LL3661;
_LL3621: return 1; _LL3623: return 1; _LL3625: return 1; _LL3627: return 1;
_LL3629: return 1; _LL3631: return 1; _LL3633: { struct _handler_cons _temp3702;
_push_handler(& _temp3702);{ int _temp3704= 0; if( setjmp( _temp3702.handler)){
_temp3704= 1;} if( ! _temp3704){{ void* _temp3705= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3662); void* _temp3713; void* _temp3715; struct Cyc_Absyn_Vardecl*
_temp3717; _LL3707: if(*(( int*) _temp3705) ==  Cyc_Tcenv_VarRes){ _LL3714:
_temp3713=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3705)->f1; if((
unsigned int) _temp3713 >  1u?*(( int*) _temp3713) ==  Cyc_Absyn_Funname_b: 0){
goto _LL3708;} else{ goto _LL3709;}} else{ goto _LL3709;} _LL3709: if(*(( int*)
_temp3705) ==  Cyc_Tcenv_VarRes){ _LL3716: _temp3715=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3705)->f1; if(( unsigned int) _temp3715 >  1u?*(( int*) _temp3715) ==  Cyc_Absyn_Global_b:
0){ _LL3718: _temp3717=(( struct Cyc_Absyn_Global_b_struct*) _temp3715)->f1;
goto _LL3710;} else{ goto _LL3711;}} else{ goto _LL3711;} _LL3711: goto _LL3712;
_LL3708: { int _temp3719= 1; _npop_handler( 0u); return _temp3719;} _LL3710: {
void* _temp3720= Cyc_Tcutil_compress(( void*) _temp3717->type); _LL3722: if((
unsigned int) _temp3720 >  4u?*(( int*) _temp3720) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3723;} else{ goto _LL3724;} _LL3724: goto _LL3725; _LL3723: { int
_temp3726= 1; _npop_handler( 0u); return _temp3726;} _LL3725: { int _temp3727=
var_okay; _npop_handler( 0u); return _temp3727;} _LL3721:;} _LL3712: { int
_temp3728= var_okay; _npop_handler( 0u); return _temp3728;} _LL3706:;};
_pop_handler();} else{ void* _temp3703=( void*) _exn_thrown; void* _temp3730=
_temp3703; _LL3732: if( _temp3730 ==  Cyc_Dict_Absent){ goto _LL3733;} else{
goto _LL3734;} _LL3734: goto _LL3735; _LL3733: return 0; _LL3735:( void) _throw(
_temp3730); _LL3731:;}}} _LL3635: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3668)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3666): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3664): 0; _LL3637: return Cyc_Tcutil_cnst_exp( te, 0, _temp3672)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3670): 0; _LL3639: _temp3676= _temp3674; goto _LL3641; _LL3641:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3676); _LL3643: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3678); _LL3645: return Cyc_Tcutil_cnst_exp( te, 1, _temp3682);
_LL3647: return Cyc_Tcutil_cnst_exp( te, 0, _temp3686)? Cyc_Tcutil_cnst_exp( te,
0, _temp3684): 0; _LL3649: _temp3690= _temp3688; goto _LL3651; _LL3651:
_temp3692= _temp3690; goto _LL3653; _LL3653: for( 0; _temp3692 !=  0; _temp3692=((
struct Cyc_List_List*) _check_null( _temp3692))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3692))->hd)).f2)){
return 0;}} return 1; _LL3655: _temp3698= _temp3694; goto _LL3657; _LL3657:
_temp3700= _temp3698; goto _LL3659; _LL3659: for( 0; _temp3700 !=  0; _temp3700=((
struct Cyc_List_List*) _check_null( _temp3700))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3700))->hd)){
return 0;}} return 1; _LL3661: return 0; _LL3619:;} int Cyc_Tcutil_is_const_exp(
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
