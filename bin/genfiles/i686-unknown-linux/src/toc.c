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
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
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
_tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_vfprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr ap); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int
Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*,
void*), void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern void Cyc_List_iter_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle*, struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_forall(
int(* pred)( void*), struct Cyc_List_List* x); extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k,
void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
k); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null;
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void*
Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_arr* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_arr* v,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp*
init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0*
x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue( struct
Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field(
struct Cyc_List_List*, struct _tagged_arr*); extern struct _tagged_arr* Cyc_Absyn_fieldname(
int); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*); extern
struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*); extern
struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); static const int
Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern
void* Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List* Cyc_Toc_result_decls=
0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; static int Cyc_Toc_temp_var_counter= 0; static int Cyc_Toc_fresh_label_counter=
0; unsigned char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
unsigned char Cyc_Toc_Toc_Impossible[ 19u]="\000\000\000\000Toc_Impossible";
static void* Cyc_Toc_unimp( struct _tagged_arr fmt, struct _tagged_arr ap){ Cyc_Std_vfprintf(
Cyc_Std_stderr, fmt, ap);({ void* _temp0[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp0, sizeof( void*), 0u));});
Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*)
Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct _tagged_arr
fmt, struct _tagged_arr ap){ Cyc_Std_vfprintf( Cyc_Std_stderr, fmt, ap);({ void*
_temp1[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static
unsigned char _temp2[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={
_temp2, _temp2, _temp2 +  5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _temp3[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp3, _temp3, _temp3 +  4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp4[ 14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp4, _temp4, _temp4 +  14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp5[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp5, _temp5, _temp5 +  8u};
static struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp6[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp6, _temp6, _temp6 +  14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp7[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp7, _temp7, _temp7 +  7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)((
void*)& Cyc_Toc__throw_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=&
Cyc_Toc__throw_ev; static unsigned char _temp9[ 7u]="setjmp"; static struct
_tagged_arr Cyc_Toc_setjmp_str={ _temp9, _temp9, _temp9 +  7u}; static struct
_tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,& Cyc_Toc_setjmp_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={ 0,( void*)((
void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp11[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp11, _temp11, _temp11 +  14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_handler_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp13[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp13, _temp13, _temp13 +  13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__pop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=&
Cyc_Toc__pop_handler_ev; static unsigned char _temp15[ 12u]="_exn_thrown";
static struct _tagged_arr Cyc_Toc__exn_thrown_str={ _temp15, _temp15, _temp15 + 
12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp17[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp17, _temp17, _temp17 +  14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp19[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp19, _temp19, _temp19 + 
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1u,& Cyc_Toc__check_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_null_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;
static unsigned char _temp21[ 28u]="_check_known_subscript_null"; static struct
_tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp21, _temp21, _temp21
+  28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str}; static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,& Cyc_Toc__check_known_subscript_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re), 0}; static struct
Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp23[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp23, _temp23,
_temp23 +  31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp25[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp25, _temp25, _temp25 + 
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp27[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp27, _temp27, _temp27 +  12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__tagged_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=&
Cyc_Toc__tagged_arr_ev; static unsigned char _temp29[ 9u]="_tag_arr"; static
struct _tagged_arr Cyc_Toc__tag_arr_str={ _temp29, _temp29, _temp29 +  9u};
static struct _tuple0 Cyc_Toc__tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={
1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,( void*)(( void*)& Cyc_Toc__tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=&
Cyc_Toc__tag_arr_ev; static unsigned char _temp31[ 14u]="_init_tag_arr"; static
struct _tagged_arr Cyc_Toc__init_tag_arr_str={ _temp31, _temp31, _temp31 +  14u};
static struct _tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__init_tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={
1u,& Cyc_Toc__init_tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__init_tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=& Cyc_Toc__init_tag_arr_ev;
static unsigned char _temp33[ 11u]="_untag_arr"; static struct _tagged_arr Cyc_Toc__untag_arr_str={
_temp33, _temp33, _temp33 +  11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__untag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;
static unsigned char _temp35[ 14u]="_get_arr_size"; static struct _tagged_arr
Cyc_Toc__get_arr_size_str={ _temp35, _temp35, _temp35 +  14u}; static struct
_tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={ 1u,& Cyc_Toc__get_arr_size_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={ 0,(
void*)(( void*)& Cyc_Toc__get_arr_size_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev; static unsigned char _temp37[ 17u]="_tagged_arr_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={ _temp37, _temp37,
_temp37 +  17u}; static struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={ 1u,& Cyc_Toc__tagged_arr_plus_pr,( void*)(( void*)
0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_plus_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_plus_e=&
Cyc_Toc__tagged_arr_plus_ev; static unsigned char _temp39[ 25u]="_tagged_arr_inplace_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={ _temp39,
_temp39, _temp39 +  25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={ 1u,&
Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp41[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp41, _temp41,
_temp41 +  30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp43[ 10u]="_cycalloc"; static struct _tagged_arr Cyc_Toc__cycalloc_str={
_temp43, _temp43, _temp43 +  10u}; static struct _tuple0 Cyc_Toc__cycalloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={ 1u,& Cyc_Toc__cycalloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={ 0,( void*)(( void*)& Cyc_Toc__cycalloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static unsigned char _temp45[ 17u]="_cycalloc_atomic"; static struct _tagged_arr
Cyc_Toc__cycalloc_atomic_str={ _temp45, _temp45, _temp45 +  17u}; static struct
_tuple0 Cyc_Toc__cycalloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={ 1u,& Cyc_Toc__cycalloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc__cycalloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev; static unsigned char
_temp47[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp47, _temp47, _temp47 +  15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp49[ 13u]="_check_times"; static struct _tagged_arr Cyc_Toc__check_times_str={
_temp49, _temp49, _temp49 +  13u}; static struct _tuple0 Cyc_Toc__check_times_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={ 1u,& Cyc_Toc__check_times_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_times_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev; static unsigned char _temp51[ 12u]="_new_region";
static struct _tagged_arr Cyc_Toc__new_region_str={ _temp51, _temp51, _temp51 + 
12u}; static struct _tuple0 Cyc_Toc__new_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__new_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp53[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp53, _temp53, _temp53 +  13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__push_region_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev; static unsigned char _temp55[ 12u]="_pop_region";
static struct _tagged_arr Cyc_Toc__pop_region_str={ _temp55, _temp55, _temp55 + 
12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static struct Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={ 9u,( struct
_tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0}; static void* Cyc_Toc_tagged_arr_typ=(
void*)& Cyc_Toc_tagged_arr_typ_v; static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={
0, 0, 0}; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; }
; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp58=( void*) e->r;
void* _temp84; unsigned char _temp86; void* _temp88; short _temp90; void*
_temp92; int _temp94; void* _temp96; long long _temp98; void* _temp100; struct
Cyc_Absyn_Exp* _temp102; struct Cyc_List_List* _temp104; struct Cyc_List_List*
_temp106; struct Cyc_List_List* _temp108; struct Cyc_List_List* _temp110; struct
Cyc_List_List* _temp112; _LL60: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL85: _temp84=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp84 >  1u?*(( int*) _temp84) ==  Cyc_Absyn_Char_c: 0){ _LL87:
_temp86=(( struct Cyc_Absyn_Char_c_struct*) _temp84)->f2; goto _LL61;} else{
goto _LL62;}} else{ goto _LL62;} _LL62: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL89: _temp88=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp88 >  1u?*(( int*) _temp88) ==  Cyc_Absyn_Short_c: 0){ _LL91:
_temp90=(( struct Cyc_Absyn_Short_c_struct*) _temp88)->f2; goto _LL63;} else{
goto _LL64;}} else{ goto _LL64;} _LL64: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL93: _temp92=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp92 >  1u?*(( int*) _temp92) ==  Cyc_Absyn_Int_c: 0){ _LL95:
_temp94=(( struct Cyc_Absyn_Int_c_struct*) _temp92)->f2; goto _LL65;} else{ goto
_LL66;}} else{ goto _LL66;} _LL66: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL97: _temp96=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp96 >  1u?*(( int*) _temp96) ==  Cyc_Absyn_LongLong_c: 0){
_LL99: _temp98=(( struct Cyc_Absyn_LongLong_c_struct*) _temp96)->f2; goto _LL67;}
else{ goto _LL68;}} else{ goto _LL68;} _LL68: if(*(( int*) _temp58) ==  Cyc_Absyn_Const_e){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if(
_temp100 == ( void*) Cyc_Absyn_Null_c){ goto _LL69;} else{ goto _LL70;}} else{
goto _LL70;} _LL70: if(*(( int*) _temp58) ==  Cyc_Absyn_Cast_e){ _LL103:
_temp102=(( struct Cyc_Absyn_Cast_e_struct*) _temp58)->f2; goto _LL71;} else{
goto _LL72;} _LL72: if(*(( int*) _temp58) ==  Cyc_Absyn_Tuple_e){ _LL105:
_temp104=(( struct Cyc_Absyn_Tuple_e_struct*) _temp58)->f1; goto _LL73;} else{
goto _LL74;} _LL74: if(*(( int*) _temp58) ==  Cyc_Absyn_Array_e){ _LL107:
_temp106=(( struct Cyc_Absyn_Array_e_struct*) _temp58)->f1; goto _LL75;} else{
goto _LL76;} _LL76: if(*(( int*) _temp58) ==  Cyc_Absyn_Struct_e){ _LL109:
_temp108=(( struct Cyc_Absyn_Struct_e_struct*) _temp58)->f3; goto _LL77;} else{
goto _LL78;} _LL78: if(*(( int*) _temp58) ==  Cyc_Absyn_CompoundLit_e){ _LL111:
_temp110=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp58)->f2; goto _LL79;}
else{ goto _LL80;} _LL80: if(*(( int*) _temp58) ==  Cyc_Absyn_UnresolvedMem_e){
_LL113: _temp112=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp58)->f2; goto
_LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL61: return _temp86 == '\000';
_LL63: return _temp90 ==  0; _LL65: return _temp94 ==  0; _LL67: return _temp98
==  0; _LL69: return 1; _LL71: return Cyc_Toc_is_zero( _temp102); _LL73: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp104); _LL75: _temp108= _temp106; goto _LL77; _LL77:
_temp110= _temp108; goto _LL79; _LL79: _temp112= _temp110; goto _LL81; _LL81:
for( 0; _temp112 !=  0; _temp112=(( struct Cyc_List_List*) _check_null( _temp112))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp112))->hd)).f2)){ return 0;}} return 1; _LL83: return 0; _LL59:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp114= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp120; struct Cyc_Absyn_Conref* _temp122; _LL116:
if(( unsigned int) _temp114 >  4u?*(( int*) _temp114) ==  Cyc_Absyn_PointerType:
0){ _LL121: _temp120=(( struct Cyc_Absyn_PointerType_struct*) _temp114)->f1;
_LL123: _temp122= _temp120.nullable; goto _LL117;} else{ goto _LL118;} _LL118:
goto _LL119; _LL117: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp122); _LL119:({ void* _temp124[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable", sizeof(
unsigned char), 12u), _tag_arr( _temp124, sizeof( void*), 0u));}); return 0;
_LL115:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x,
struct _tagged_arr tag){ return({ struct _tuple0* _temp125=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp125->f1=(* x).f1; _temp125->f2=({
struct _tagged_arr* _temp126=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp126[ 0]=( struct _tagged_arr) Cyc_Std_strconcat(*(* x).f2,
tag); _temp126;}); _temp125;});} struct _tuple5{ struct _tagged_arr* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct
Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp127= Cyc_Toc_tuple_types; for(
0; _temp127 !=  0; _temp127=(( struct Cyc_List_List*) _check_null( _temp127))->tl){
struct _tuple5 _temp130; struct Cyc_List_List* _temp131; struct _tagged_arr*
_temp133; struct _tuple5* _temp128=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp127))->hd; _temp130=* _temp128; _LL134: _temp133= _temp130.f1;
goto _LL132; _LL132: _temp131= _temp130.f2; goto _LL129; _LL129: { struct Cyc_List_List*
_temp135= tqs0; for( 0; _temp135 !=  0? _temp131 !=  0: 0;( _temp135=(( struct
Cyc_List_List*) _check_null( _temp135))->tl, _temp131=(( struct Cyc_List_List*)
_check_null( _temp131))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp135))->hd)).f2,( void*)(( struct Cyc_List_List*)
_check_null( _temp131))->hd)){ break;}} if( _temp135 ==  0? _temp131 ==  0: 0){
return _temp133;}}}}{ struct _tagged_arr* x=({ struct _tagged_arr* _temp149=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp149[ 0]=(
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp151; _temp151.tag= Cyc_Std_Int_pa;
_temp151.f1=( int)(( unsigned int) Cyc_Toc_tuple_type_counter ++);{ void*
_temp150[ 1u]={& _temp151}; Cyc_Std_aprintf( _tag_arr("_tuple%d", sizeof(
unsigned char), 9u), _tag_arr( _temp150, sizeof( void*), 1u));}}); _temp149;});
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List*
_temp136= 0; struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 !=  0;( ts2=((
struct Cyc_List_List*) _check_null( ts2))->tl, i ++)){ _temp136=({ struct Cyc_List_List*
_temp137=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp137->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp138=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp138->name= Cyc_Absyn_fieldname(
i); _temp138->tq= Cyc_Toc_mt_tq; _temp138->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp138->width= 0; _temp138->attributes= 0; _temp138;});
_temp137->tl= _temp136; _temp137;});}} _temp136=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp136);{ struct Cyc_Absyn_Structdecl*
_temp139=({ struct Cyc_Absyn_Structdecl* _temp145=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp145->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp145->name=({ struct Cyc_Core_Opt* _temp147=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp147->v=( void*)({ struct _tuple0*
_temp148=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp148->f1=
Cyc_Absyn_rel_ns_null; _temp148->f2= x; _temp148;}); _temp147;}); _temp145->tvs=
0; _temp145->fields=({ struct Cyc_Core_Opt* _temp146=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp146->v=( void*) _temp136;
_temp146;}); _temp145->attributes= 0; _temp145;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp140=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp140->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp141=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp141[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp142; _temp142.tag= Cyc_Absyn_Struct_d; _temp142.f1=
_temp139; _temp142;}); _temp141;}), 0); _temp140->tl= Cyc_Toc_result_decls;
_temp140;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp143=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp143->hd=( void*)({ struct
_tuple5* _temp144=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp144->f1= x; _temp144->f2= ts; _temp144;}); _temp143->tl= Cyc_Toc_tuple_types;
_temp143;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp152=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp152->f1=( void*) Cyc_Absyn_Loc_n; _temp152->f2=({ struct _tagged_arr*
_temp153=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp153[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp155;
_temp155.tag= Cyc_Std_Int_pa; _temp155.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp154[ 1u]={& _temp155}; Cyc_Std_aprintf( _tag_arr("_temp%d",
sizeof( unsigned char), 8u), _tag_arr( _temp154, sizeof( void*), 1u));}});
_temp153;}); _temp152;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp156=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp156[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp158; _temp158.tag= Cyc_Std_Int_pa; _temp158.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp157[ 1u]={& _temp158}; Cyc_Std_aprintf( _tag_arr("_LL%d",
sizeof( unsigned char), 6u), _tag_arr( _temp157, sizeof( void*), 1u));}});
_temp156;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp161; struct Cyc_Absyn_Tqual
_temp163; struct Cyc_Core_Opt* _temp165; struct _tuple1 _temp159=* a; _LL166:
_temp165= _temp159.f1; goto _LL164; _LL164: _temp163= _temp159.f2; goto _LL162;
_LL162: _temp161= _temp159.f3; goto _LL160; _LL160: return({ struct _tuple1*
_temp167=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp167->f1=
_temp165; _temp167->f2= _temp163; _temp167->f3= Cyc_Toc_typ_to_c( _temp161);
_temp167;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp170; struct Cyc_Absyn_Tqual _temp172; struct _tuple3 _temp168=* x;
_LL173: _temp172= _temp168.f1; goto _LL171; _LL171: _temp170= _temp168.f2; goto
_LL169; _LL169: return({ struct _tuple3* _temp174=( struct _tuple3*) _cycalloc(
sizeof( struct _tuple3)); _temp174->f1= _temp172; _temp174->f2= Cyc_Toc_typ_to_c(
_temp170); _temp174;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp175= t; struct Cyc_Absyn_Exp* _temp183; struct Cyc_Absyn_Tqual _temp185;
void* _temp187; struct Cyc_Core_Opt* _temp189; struct Cyc_Core_Opt _temp191;
void* _temp192; _LL177: if(( unsigned int) _temp175 >  4u?*(( int*) _temp175) == 
Cyc_Absyn_ArrayType: 0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f1; goto _LL186; _LL186: _temp185=(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f2; goto _LL184; _LL184: _temp183=(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f3; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp175 >  4u?*(( int*) _temp175) ==  Cyc_Absyn_Evar: 0){ _LL190: _temp189=((
struct Cyc_Absyn_Evar_struct*) _temp175)->f2; if( _temp189 ==  0){ goto _LL181;}
else{ _temp191=* _temp189; _LL193: _temp192=( void*) _temp191.v; goto _LL180;}}
else{ goto _LL181;} _LL181: goto _LL182; _LL178: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp194=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp194[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp195; _temp195.tag= Cyc_Absyn_ArrayType;
_temp195.f1=( void*) Cyc_Toc_typ_to_c_array( _temp187); _temp195.f2= _temp185;
_temp195.f3= _temp183; _temp195;}); _temp194;}); _LL180: return Cyc_Toc_typ_to_c_array(
_temp192); _LL182: return Cyc_Toc_typ_to_c( t); _LL176:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp196=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp196->name= f->name; _temp196->tq= f->tq; _temp196->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp196->width= f->width; _temp196->attributes= f->attributes;
_temp196;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp197= t; struct
Cyc_Core_Opt* _temp251; struct Cyc_Core_Opt* _temp253; struct Cyc_Core_Opt
_temp255; void* _temp256; struct Cyc_Absyn_TunionInfo _temp258; void* _temp260;
struct Cyc_Absyn_TunionFieldInfo _temp262; void* _temp264; struct Cyc_Absyn_Tunionfield*
_temp266; struct Cyc_Absyn_Tuniondecl* _temp268; struct Cyc_Absyn_PtrInfo
_temp270; struct Cyc_Absyn_Conref* _temp272; struct Cyc_Absyn_Tqual _temp274;
void* _temp276; struct Cyc_Absyn_Exp* _temp278; struct Cyc_Absyn_Tqual _temp280;
void* _temp282; struct Cyc_Absyn_FnInfo _temp284; struct Cyc_List_List* _temp286;
struct Cyc_Absyn_VarargInfo* _temp288; int _temp290; struct Cyc_List_List*
_temp292; void* _temp294; struct Cyc_List_List* _temp296; struct Cyc_List_List*
_temp298; struct Cyc_List_List* _temp300; struct _tuple0* _temp302; struct
_tuple0* _temp304; struct _tuple0* _temp306; struct Cyc_Core_Opt* _temp308;
struct Cyc_List_List* _temp310; struct _tuple0* _temp312; void* _temp314; _LL199:
if( _temp197 == ( void*) Cyc_Absyn_VoidType){ goto _LL200;} else{ goto _LL201;}
_LL201: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_Evar:
0){ _LL252: _temp251=(( struct Cyc_Absyn_Evar_struct*) _temp197)->f2; if(
_temp251 ==  0){ goto _LL202;} else{ goto _LL203;}} else{ goto _LL203;} _LL203:
if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_Evar: 0){
_LL254: _temp253=(( struct Cyc_Absyn_Evar_struct*) _temp197)->f2; if( _temp253
==  0){ goto _LL205;} else{ _temp255=* _temp253; _LL257: _temp256=( void*)
_temp255.v; goto _LL204;}} else{ goto _LL205;} _LL205: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_VarType: 0){ goto _LL206;}
else{ goto _LL207;} _LL207: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197)
==  Cyc_Absyn_TunionType: 0){ _LL259: _temp258=(( struct Cyc_Absyn_TunionType_struct*)
_temp197)->f1; _LL261: _temp260=( void*) _temp258.tunion_info; if(*(( int*)
_temp260) ==  Cyc_Absyn_KnownTunion){ goto _LL208;} else{ goto _LL209;}} else{
goto _LL209;} _LL209: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) == 
Cyc_Absyn_TunionType: 0){ goto _LL210;} else{ goto _LL211;} _LL211: if((
unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TunionFieldType:
0){ _LL263: _temp262=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp197)->f1;
_LL265: _temp264=( void*) _temp262.field_info; if(*(( int*) _temp264) ==  Cyc_Absyn_KnownTunionfield){
_LL269: _temp268=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp264)->f1;
goto _LL267; _LL267: _temp266=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp264)->f2; goto _LL212;} else{ goto _LL213;}} else{ goto _LL213;} _LL213:
if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_PointerType: 0){ _LL271: _temp270=(( struct Cyc_Absyn_PointerType_struct*)
_temp197)->f1; _LL277: _temp276=( void*) _temp270.elt_typ; goto _LL275; _LL275:
_temp274= _temp270.tq; goto _LL273; _LL273: _temp272= _temp270.bounds; goto
_LL216;} else{ goto _LL217;} _LL217: if(( unsigned int) _temp197 >  4u?*(( int*)
_temp197) ==  Cyc_Absyn_IntType: 0){ goto _LL218;} else{ goto _LL219;} _LL219:
if( _temp197 == ( void*) Cyc_Absyn_FloatType){ goto _LL220;} else{ goto _LL221;}
_LL221: if( _temp197 == ( void*) Cyc_Absyn_DoubleType){ goto _LL222;} else{ goto
_LL223;} _LL223: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_ArrayType:
0){ _LL283: _temp282=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp197)->f1;
goto _LL281; _LL281: _temp280=(( struct Cyc_Absyn_ArrayType_struct*) _temp197)->f2;
goto _LL279; _LL279: _temp278=(( struct Cyc_Absyn_ArrayType_struct*) _temp197)->f3;
goto _LL224;} else{ goto _LL225;} _LL225: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_FnType: 0){ _LL285: _temp284=(( struct Cyc_Absyn_FnType_struct*)
_temp197)->f1; _LL295: _temp294=( void*) _temp284.ret_typ; goto _LL293; _LL293:
_temp292= _temp284.args; goto _LL291; _LL291: _temp290= _temp284.c_varargs; goto
_LL289; _LL289: _temp288= _temp284.cyc_varargs; goto _LL287; _LL287: _temp286=
_temp284.attributes; goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TupleType: 0){ _LL297: _temp296=((
struct Cyc_Absyn_TupleType_struct*) _temp197)->f1; goto _LL228;} else{ goto
_LL229;} _LL229: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_AnonStructType:
0){ _LL299: _temp298=(( struct Cyc_Absyn_AnonStructType_struct*) _temp197)->f1;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_AnonUnionType: 0){ _LL301: _temp300=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp197)->f1; goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_UnionType: 0){ _LL303: _temp302=((
struct Cyc_Absyn_UnionType_struct*) _temp197)->f1; goto _LL234;} else{ goto
_LL235;} _LL235: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_StructType:
0){ _LL305: _temp304=(( struct Cyc_Absyn_StructType_struct*) _temp197)->f1; goto
_LL236;} else{ goto _LL237;} _LL237: if(( unsigned int) _temp197 >  4u?*(( int*)
_temp197) ==  Cyc_Absyn_EnumType: 0){ _LL307: _temp306=(( struct Cyc_Absyn_EnumType_struct*)
_temp197)->f1; goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_TypedefType: 0){ _LL313:
_temp312=(( struct Cyc_Absyn_TypedefType_struct*) _temp197)->f1; goto _LL311;
_LL311: _temp310=(( struct Cyc_Absyn_TypedefType_struct*) _temp197)->f2; goto
_LL309; _LL309: _temp308=(( struct Cyc_Absyn_TypedefType_struct*) _temp197)->f3;
goto _LL240;} else{ goto _LL241;} _LL241: if(( unsigned int) _temp197 >  4u?*((
int*) _temp197) ==  Cyc_Absyn_RgnHandleType: 0){ _LL315: _temp314=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp197)->f1; goto _LL242;} else{ goto
_LL243;} _LL243: if( _temp197 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL244;}
else{ goto _LL245;} _LL245: if(( unsigned int) _temp197 >  4u?*(( int*) _temp197)
==  Cyc_Absyn_AccessEff: 0){ goto _LL246;} else{ goto _LL247;} _LL247: if((
unsigned int) _temp197 >  4u?*(( int*) _temp197) ==  Cyc_Absyn_JoinEff: 0){ goto
_LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp197 >  4u?*(( int*)
_temp197) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL250;} else{ goto _LL198;} _LL200:
return t; _LL202: return Cyc_Absyn_sint_t; _LL204: return Cyc_Toc_typ_to_c(
_temp256); _LL206: return Cyc_Absyn_void_star_typ(); _LL208: return Cyc_Absyn_void_star_typ();
_LL210:( int) _throw(({ void* _temp316[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp316, sizeof( void*), 0u));}));
_LL212: if( _temp266->typs ==  0){ if( _temp268->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp266->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL214:( int) _throw(({ void*
_temp317[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp317, sizeof( void*), 0u));}));
_LL216: _temp276= Cyc_Toc_typ_to_c_array( _temp276);{ void* _temp318=( void*)(
Cyc_Absyn_compress_conref( _temp272))->v; void* _temp324; _LL320: if((
unsigned int) _temp318 >  1u?*(( int*) _temp318) ==  Cyc_Absyn_Eq_constr: 0){
_LL325: _temp324=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp318)->f1;
if( _temp324 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL321;} else{ goto _LL322;}}
else{ goto _LL322;} _LL322: goto _LL323; _LL321: return Cyc_Toc_tagged_arr_typ;
_LL323: return Cyc_Absyn_star_typ( _temp276,( void*) Cyc_Absyn_HeapRgn, _temp274);
_LL319:;} _LL218: return t; _LL220: return t; _LL222: return t; _LL224: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp326=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp326[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp327; _temp327.tag= Cyc_Absyn_ArrayType; _temp327.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp282); _temp327.f2= _temp280; _temp327.f3=
_temp278; _temp327;}); _temp326;}); _LL226: { struct Cyc_List_List* _temp328= 0;
for( 0; _temp286 !=  0; _temp286=(( struct Cyc_List_List*) _check_null( _temp286))->tl){
void* _temp329=( void*)(( struct Cyc_List_List*) _check_null( _temp286))->hd;
_LL331: if( _temp329 == ( void*) Cyc_Absyn_Noreturn_att){ goto _LL332;} else{
goto _LL333;} _LL333: if( _temp329 == ( void*) Cyc_Absyn_Const_att){ goto _LL334;}
else{ goto _LL335;} _LL335: if(( unsigned int) _temp329 >  16u?*(( int*)
_temp329) ==  Cyc_Absyn_Format_att: 0){ goto _LL336;} else{ goto _LL337;} _LL337:
goto _LL338; _LL332: goto _LL334; _LL334: goto _LL336; _LL336: continue; _LL338:
_temp328=({ struct Cyc_List_List* _temp339=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp339->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp286))->hd); _temp339->tl= _temp328; _temp339;}); goto _LL330;
_LL330:;}{ struct Cyc_List_List* _temp340=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp292); if( _temp288 !=  0){ void* _temp341= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp288))->type,( void*)
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp342=({ struct _tuple1*
_temp344=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp344->f1=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp288))->name; _temp344->f2=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp288))->tq; _temp344->f3=
_temp341; _temp344;}); _temp340=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp340,({ struct Cyc_List_List*
_temp343=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp343->hd=( void*) _temp342; _temp343->tl= 0; _temp343;}));} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp345=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp345[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp346; _temp346.tag= Cyc_Absyn_FnType; _temp346.f1=({ struct Cyc_Absyn_FnInfo
_temp347; _temp347.tvars= 0; _temp347.effect= 0; _temp347.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp294); _temp347.args= _temp340; _temp347.c_varargs= _temp290; _temp347.cyc_varargs=
0; _temp347.rgn_po= 0; _temp347.attributes= _temp328; _temp347;}); _temp346;});
_temp345;});}} _LL228: _temp296=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp296);{ struct _tagged_arr* _temp348= Cyc_Toc_add_tuple_type( _temp296);
return Cyc_Absyn_strct( _temp348);} _LL230: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp349=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp349[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp350; _temp350.tag= Cyc_Absyn_AnonStructType; _temp350.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp298); _temp350;}); _temp349;});
_LL232: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp351=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp351[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp352; _temp352.tag=
Cyc_Absyn_AnonUnionType; _temp352.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp300); _temp352;}); _temp351;}); _LL234: if( _temp302 ==  0){({ void*
_temp353[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp353,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp302)); _LL236: if( _temp304 ==  0){({ void* _temp354[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp354, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp304)); _LL238:
return t; _LL240: if( _temp308 ==  0){ return({ void* _temp355[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u),
_tag_arr( _temp355, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp356=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp356[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp357; _temp357.tag= Cyc_Absyn_TypedefType;
_temp357.f1= _temp312; _temp357.f2= 0; _temp357.f3=({ struct Cyc_Core_Opt*
_temp358=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp358->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp308))->v); _temp358;}); _temp357;}); _temp356;}); _LL242:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL244: return({ void* _temp359[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp359, sizeof( void*), 0u));});
_LL246: return({ void* _temp360[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp360, sizeof( void*), 0u));});
_LL248: return({ void* _temp361[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp361, sizeof( void*), 0u));});
_LL250: return({ void* _temp362[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp362, sizeof( void*), 0u));});
_LL198:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp363= t;
struct Cyc_Absyn_Tqual _temp369; void* _temp371; _LL365: if(( unsigned int)
_temp363 >  4u?*(( int*) _temp363) ==  Cyc_Absyn_ArrayType: 0){ _LL372: _temp371=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp363)->f1; goto _LL370; _LL370:
_temp369=(( struct Cyc_Absyn_ArrayType_struct*) _temp363)->f2; goto _LL366;}
else{ goto _LL367;} _LL367: goto _LL368; _LL366: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp371,( void*) Cyc_Absyn_HeapRgn, _temp369), e, l); _LL368: return Cyc_Absyn_cast_exp(
t, e, l); _LL364:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp373= Cyc_Tcutil_compress(
t); void* _temp411; struct Cyc_Absyn_Structdecl** _temp413; struct Cyc_List_List*
_temp415; struct Cyc_List_List* _temp417; struct Cyc_Absyn_TunionFieldInfo
_temp419; void* _temp421; struct Cyc_Absyn_Tunionfield* _temp423; struct Cyc_Absyn_Tuniondecl*
_temp425; struct Cyc_List_List* _temp427; _LL375: if( _temp373 == ( void*) Cyc_Absyn_VoidType){
goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int) _temp373 >  4u?*((
int*) _temp373) ==  Cyc_Absyn_VarType: 0){ goto _LL378;} else{ goto _LL379;}
_LL379: if(( unsigned int) _temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_IntType:
0){ goto _LL380;} else{ goto _LL381;} _LL381: if(( unsigned int) _temp373 >  4u?*((
int*) _temp373) ==  Cyc_Absyn_EnumType: 0){ goto _LL382;} else{ goto _LL383;}
_LL383: if( _temp373 == ( void*) Cyc_Absyn_FloatType){ goto _LL384;} else{ goto
_LL385;} _LL385: if( _temp373 == ( void*) Cyc_Absyn_DoubleType){ goto _LL386;}
else{ goto _LL387;} _LL387: if(( unsigned int) _temp373 >  4u?*(( int*) _temp373)
==  Cyc_Absyn_FnType: 0){ goto _LL388;} else{ goto _LL389;} _LL389: if((
unsigned int) _temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_ArrayType: 0){
_LL412: _temp411=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp373)->f1;
goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int) _temp373 >  4u?*((
int*) _temp373) ==  Cyc_Absyn_StructType: 0){ _LL414: _temp413=(( struct Cyc_Absyn_StructType_struct*)
_temp373)->f3; goto _LL392;} else{ goto _LL393;} _LL393: if(( unsigned int)
_temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_AnonStructType: 0){ _LL416:
_temp415=(( struct Cyc_Absyn_AnonStructType_struct*) _temp373)->f1; goto _LL394;}
else{ goto _LL395;} _LL395: if(( unsigned int) _temp373 >  4u?*(( int*) _temp373)
==  Cyc_Absyn_AnonUnionType: 0){ _LL418: _temp417=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp373)->f1; goto _LL396;} else{ goto _LL397;} _LL397: if(( unsigned int)
_temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_TunionFieldType: 0){ _LL420:
_temp419=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp373)->f1; _LL422:
_temp421=( void*) _temp419.field_info; if(*(( int*) _temp421) ==  Cyc_Absyn_KnownTunionfield){
_LL426: _temp425=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp421)->f1;
goto _LL424; _LL424: _temp423=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp421)->f2; goto _LL398;} else{ goto _LL399;}} else{ goto _LL399;} _LL399:
if(( unsigned int) _temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_TupleType: 0){
_LL428: _temp427=(( struct Cyc_Absyn_TupleType_struct*) _temp373)->f1; goto
_LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp373 >  4u?*(( int*)
_temp373) ==  Cyc_Absyn_TunionType: 0){ goto _LL402;} else{ goto _LL403;} _LL403:
if(( unsigned int) _temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_UnionType: 0){
goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int) _temp373 >  4u?*((
int*) _temp373) ==  Cyc_Absyn_PointerType: 0){ goto _LL406;} else{ goto _LL407;}
_LL407: if(( unsigned int) _temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL408;} else{ goto _LL409;} _LL409: goto _LL410; _LL376: return 1;
_LL378: return 0; _LL380: return 1; _LL382: return 1; _LL384: return 1; _LL386:
return 1; _LL388: return 1; _LL390: return Cyc_Toc_atomic_typ( _temp411); _LL392:
if( _temp413 ==  0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp413)))->fields
==  0){ return 0;}{ struct Cyc_List_List* _temp429=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp413)))->fields))->v; for( 0; _temp429 !=  0; _temp429=((
struct Cyc_List_List*) _check_null( _temp429))->tl){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp429))->hd)->type)){ return 0;}}} return 1; _LL394: _temp417= _temp415; goto
_LL396; _LL396: for( 0; _temp417 !=  0; _temp417=(( struct Cyc_List_List*)
_check_null( _temp417))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp417))->hd)->type)){ return 0;}} return
1; _LL398: _temp427= _temp423->typs; goto _LL400; _LL400: for( 0; _temp427 !=  0;
_temp427=(( struct Cyc_List_List*) _check_null( _temp427))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp427))->hd)).f2)){
return 0;}} return 1; _LL402: return 0; _LL404: return 0; _LL406: return 0;
_LL408: return 0; _LL410:({ struct Cyc_Std_String_pa_struct _temp431; _temp431.tag=
Cyc_Std_String_pa; _temp431.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp430[ 1u]={& _temp431};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp430, sizeof( void*), 1u));}});
return 0; _LL374:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp432=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp438; void* _temp440;
_LL434: if(( unsigned int) _temp432 >  4u?*(( int*) _temp432) ==  Cyc_Absyn_PointerType:
0){ _LL439: _temp438=(( struct Cyc_Absyn_PointerType_struct*) _temp432)->f1;
_LL441: _temp440=( void*) _temp438.elt_typ; goto _LL435;} else{ goto _LL436;}
_LL436: goto _LL437; _LL435: { void* _temp442= Cyc_Tcutil_compress( _temp440);
_LL444: if( _temp442 == ( void*) Cyc_Absyn_VoidType){ goto _LL445;} else{ goto
_LL446;} _LL446: goto _LL447; _LL445: return 1; _LL447: return 0; _LL443:;}
_LL437: return 0; _LL433:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp448= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp460; struct Cyc_Absyn_Structdecl* _temp462; struct Cyc_List_List* _temp463;
_LL450: if(( unsigned int) _temp448 >  4u?*(( int*) _temp448) ==  Cyc_Absyn_StructType:
0){ _LL461: _temp460=(( struct Cyc_Absyn_StructType_struct*) _temp448)->f3; if(
_temp460 ==  0){ goto _LL452;} else{ _temp462=* _temp460; goto _LL451;}} else{
goto _LL452;} _LL452: if(( unsigned int) _temp448 >  4u?*(( int*) _temp448) == 
Cyc_Absyn_AnonStructType: 0){ _LL464: _temp463=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp448)->f1; goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int)
_temp448 >  4u?*(( int*) _temp448) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL455;}
else{ goto _LL456;} _LL456: if(( unsigned int) _temp448 >  4u?*(( int*) _temp448)
==  Cyc_Absyn_UnionType: 0){ goto _LL457;} else{ goto _LL458;} _LL458: goto
_LL459; _LL451: if( _temp462->fields ==  0){( int) _throw(({ void* _temp465[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp465, sizeof( void*), 0u));}));} _temp463=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp462->fields))->v;
goto _LL453; _LL453: { struct Cyc_Absyn_Structfield* _temp466= Cyc_Absyn_lookup_field(
_temp463, f); if( _temp466 ==  0){({ struct Cyc_Std_String_pa_struct _temp468;
_temp468.tag= Cyc_Std_String_pa; _temp468.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp466))->name;{ void* _temp467[ 1u]={& _temp468};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_field: bad field %s",
sizeof( unsigned char), 28u), _tag_arr( _temp467, sizeof( void*), 1u));}});}
return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp466))->type);} _LL455: return 0; _LL457: return 0; _LL459:(
int) _throw(({ struct Cyc_Std_String_pa_struct _temp470; _temp470.tag= Cyc_Std_String_pa;
_temp470.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp469[ 1u]={&
_temp470}; Cyc_Toc_toc_impos( _tag_arr("is_poly_field: bad type %s", sizeof(
unsigned char), 27u), _tag_arr( _temp469, sizeof( void*), 1u));}})); _LL449:;}
static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp471=(
void*) e->r; struct _tagged_arr* _temp479; struct Cyc_Absyn_Exp* _temp481;
struct _tagged_arr* _temp483; struct Cyc_Absyn_Exp* _temp485; _LL473: if(*(( int*)
_temp471) ==  Cyc_Absyn_StructMember_e){ _LL482: _temp481=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp471)->f1; goto _LL480; _LL480: _temp479=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp471)->f2; goto _LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp471)
==  Cyc_Absyn_StructArrow_e){ _LL486: _temp485=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp471)->f1; goto _LL484; _LL484: _temp483=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp471)->f2; goto _LL476;} else{ goto _LL477;} _LL477: goto _LL478; _LL474:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp481->topt))->v, _temp479); _LL476: { void* _temp487= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp485->topt))->v); struct Cyc_Absyn_PtrInfo
_temp493; void* _temp495; _LL489: if(( unsigned int) _temp487 >  4u?*(( int*)
_temp487) ==  Cyc_Absyn_PointerType: 0){ _LL494: _temp493=(( struct Cyc_Absyn_PointerType_struct*)
_temp487)->f1; _LL496: _temp495=( void*) _temp493.elt_typ; goto _LL490;} else{
goto _LL491;} _LL491: goto _LL492; _LL490: return Cyc_Toc_is_poly_field(
_temp495, _temp483); _LL492:({ struct Cyc_Std_String_pa_struct _temp498;
_temp498.tag= Cyc_Std_String_pa; _temp498.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp485->topt))->v);{ void*
_temp497[ 1u]={& _temp498};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s", sizeof(
unsigned char), 30u), _tag_arr( _temp497, sizeof( void*), 1u));}}); return 0;
_LL488:;} _LL478: return 0; _LL472:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_e,({
struct Cyc_List_List* _temp499=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp499->hd=( void*) s; _temp499->tl= 0; _temp499;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_atomic_e,({ struct Cyc_List_List*
_temp500=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp500->hd=( void*) s; _temp500->tl= 0; _temp500;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp501[ 2u]; _temp501[ 1u]= s; _temp501[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp501, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp502=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp502->hd=( void*) e; _temp502->tl= 0; _temp502;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp503=( void*) e->r; void* _temp509; _LL505: if(*((
int*) _temp503) ==  Cyc_Absyn_Const_e){ _LL510: _temp509=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp503)->f1; if(( unsigned int) _temp509 >  1u?*(( int*) _temp509) ==  Cyc_Absyn_String_c:
0){ goto _LL506;} else{ goto _LL507;}} else{ goto _LL507;} _LL507: goto _LL508;
_LL506: is_string= 1; goto _LL504; _LL508: goto _LL504; _LL504:;}{ struct Cyc_Absyn_Exp*
xexp; struct Cyc_Absyn_Exp* xplussz; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp514=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp514[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp515; _temp515.tag= Cyc_Absyn_ArrayType; _temp515.f1=(
void*) Cyc_Absyn_uchar_t; _temp515.f2= Cyc_Toc_mt_tq; _temp515.f3=( struct Cyc_Absyn_Exp*)
sz; _temp515;}); _temp514;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp511=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp511->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp512=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp512[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp513; _temp513.tag= Cyc_Absyn_Var_d;
_temp513.f1= vd; _temp513;}); _temp512;}), 0); _temp511->tl= Cyc_Toc_result_decls;
_temp511;}); xexp= Cyc_Absyn_var_exp( x, 0); xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0);} else{ xexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), e, 0);
xplussz= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_add_exp( e, sz,
0), 0);}{ struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp516[ 3u]; _temp516[ 2u]=({ struct _tuple4* _temp519=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp519->f1= 0; _temp519->f2=
xplussz; _temp519;}); _temp516[ 1u]=({ struct _tuple4* _temp518=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp518->f1= 0; _temp518->f2= xexp;
_temp518;}); _temp516[ 0u]=({ struct _tuple4* _temp517=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp517->f1= 0; _temp517->f2= xexp;
_temp517;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp516, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp520=( struct Cyc_Toc_Env*) _cycalloc( sizeof( struct
Cyc_Toc_Env)); _temp520->break_lab= 0; _temp520->continue_lab= 0; _temp520->fallthru_info=
0; _temp520->varmap=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp520->toplevel= 1;
_temp520;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp521=( struct Cyc_Toc_Env*) _cycalloc( sizeof(
struct Cyc_Toc_Env)); _temp521->break_lab= e->break_lab; _temp521->continue_lab=
e->continue_lab; _temp521->fallthru_info= e->fallthru_info; _temp521->varmap= e->varmap;
_temp521->toplevel= e->toplevel; _temp521;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp522=(* x).f1; _LL524:
if(( unsigned int) _temp522 >  1u?*(( int*) _temp522) ==  Cyc_Absyn_Rel_n: 0){
goto _LL525;} else{ goto _LL526;} _LL526: goto _LL527; _LL525:( int) _throw(({
struct Cyc_Std_String_pa_struct _temp529; _temp529.tag= Cyc_Std_String_pa;
_temp529.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp528[
1u]={& _temp529}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp528, sizeof( void*), 1u));}}));
_LL527: goto _LL523; _LL523:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
k, struct Cyc_Absyn_Exp* v)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict*
f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp530= 0; for( 0; fallthru_binders !=  0; fallthru_binders=(( struct Cyc_List_List*)
_check_null( fallthru_binders))->tl){ _temp530=({ struct Cyc_List_List* _temp531=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp531->hd=(
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
fallthru_binders))->hd)->name; _temp531->tl= _temp530; _temp531;});} _temp530=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp530);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp532=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp532->v=( void*) break_l; _temp532;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp533=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp533->v=( void*)({ struct _tuple6* _temp534=( struct _tuple6*) _cycalloc(
sizeof( struct _tuple6)); _temp534->f1= fallthru_l; _temp534->f2= _temp530;
_temp534->f3= next_case_env->varmap; _temp534;}); _temp533;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp535=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp535->v=( void*) break_l; _temp535;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp536=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp536->v=( void*)({
struct _tuple6* _temp537=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp537->f1= next_l; _temp537->f2= 0; _temp537->f3=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp537;}); _temp536;}); return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt ==  0){({ void* _temp538[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp538, sizeof( void*), 0u));});}
return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp539[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp539, sizeof( void*), 0u));});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp540=( struct _tuple3*) _cycalloc( sizeof(
struct _tuple3)); _temp540->f1= Cyc_Toc_mt_tq; _temp540->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp540;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp541=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp541->f1= 0; _temp541->f2= e; _temp541;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp542= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp542): Cyc_Toc_malloc_ptr( _temp542), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp542), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp543=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp543 !=  0; _temp543=((
struct Cyc_List_List*) _check_null( _temp543))->tl){ struct _tuple4 _temp546;
struct Cyc_Absyn_Exp* _temp547; struct Cyc_List_List* _temp549; struct _tuple4*
_temp544=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp543))->hd;
_temp546=* _temp544; _LL550: _temp549= _temp546.f1; goto _LL548; _LL548:
_temp547= _temp546.f2; goto _LL545; _LL545: { struct Cyc_Absyn_Exp* e_index; if(
_temp549 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp549))->tl !=  0){({ void* _temp551[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp551, sizeof( void*), 0u));});}{
void* _temp552=( void*)(( struct Cyc_List_List*) _check_null( _temp549))->hd;
void* _temp553= _temp552; struct Cyc_Absyn_Exp* _temp559; _LL555: if(*(( int*)
_temp553) ==  Cyc_Absyn_ArrayElement){ _LL560: _temp559=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp553)->f1; goto _LL556;} else{ goto _LL557;} _LL557: if(*(( int*) _temp553)
==  Cyc_Absyn_FieldName){ goto _LL558;} else{ goto _LL554;} _LL556: Cyc_Toc_exp_to_c(
nv, _temp559); e_index= _temp559; goto _LL554; _LL558: e_index=({ void* _temp561[
0u]={};(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_unimp)( _tag_arr("field name designators in array", sizeof(
unsigned char), 32u), _tag_arr( _temp561, sizeof( void*), 0u));}); goto _LL554;
_LL554:;}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0);
void* _temp562=( void*) _temp547->r; struct Cyc_List_List* _temp572; struct Cyc_Absyn_Exp*
_temp574; struct Cyc_Absyn_Exp* _temp576; struct Cyc_Absyn_Vardecl* _temp578;
struct Cyc_List_List* _temp580; void* _temp582; _LL564: if(*(( int*) _temp562)
==  Cyc_Absyn_Array_e){ _LL573: _temp572=(( struct Cyc_Absyn_Array_e_struct*)
_temp562)->f1; goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp562)
==  Cyc_Absyn_Comprehension_e){ _LL579: _temp578=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp562)->f1; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp562)->f2; goto _LL575; _LL575: _temp574=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp562)->f3; goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp562)
==  Cyc_Absyn_AnonStruct_e){ _LL583: _temp582=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp562)->f1; goto _LL581; _LL581: _temp580=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp562)->f2; goto _LL569;} else{ goto _LL570;} _LL570: goto _LL571; _LL565: s=
Cyc_Toc_init_array( nv, lval, _temp572, s); goto _LL563; _LL567: s= Cyc_Toc_init_comprehension(
nv, lval, _temp578, _temp576, _temp574, s, 0); goto _LL563; _LL569: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp582, _temp580, s); goto _LL563; _LL571: Cyc_Toc_exp_to_c( nv,
_temp547); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp547, 0), s, 0); goto _LL563; _LL563:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp584= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp584, Cyc_Absyn_varb_exp( _temp584,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp607=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp607[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp608; _temp608.tag= Cyc_Absyn_Local_b; _temp608.f1= vd; _temp608;});
_temp607;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp584, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp584, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp584, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp584, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp585=( void*) e2->r; struct Cyc_List_List* _temp595; struct Cyc_Absyn_Exp*
_temp597; struct Cyc_Absyn_Exp* _temp599; struct Cyc_Absyn_Vardecl* _temp601;
struct Cyc_List_List* _temp603; void* _temp605; _LL587: if(*(( int*) _temp585)
==  Cyc_Absyn_Array_e){ _LL596: _temp595=(( struct Cyc_Absyn_Array_e_struct*)
_temp585)->f1; goto _LL588;} else{ goto _LL589;} _LL589: if(*(( int*) _temp585)
==  Cyc_Absyn_Comprehension_e){ _LL602: _temp601=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp585)->f1; goto _LL600; _LL600: _temp599=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp585)->f2; goto _LL598; _LL598: _temp597=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp585)->f3; goto _LL590;} else{ goto _LL591;} _LL591: if(*(( int*) _temp585)
==  Cyc_Absyn_AnonStruct_e){ _LL606: _temp605=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp585)->f1; goto _LL604; _LL604: _temp603=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp585)->f2; goto _LL592;} else{ goto _LL593;} _LL593: goto _LL594; _LL588:
body= Cyc_Toc_init_array( nv2, lval, _temp595, Cyc_Absyn_skip_stmt( 0)); goto
_LL586; _LL590: body= Cyc_Toc_init_comprehension( nv2, lval, _temp601, _temp599,
_temp597, Cyc_Absyn_skip_stmt( 0), 0); goto _LL586; _LL592: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp605, _temp603, Cyc_Absyn_skip_stmt( 0)); goto _LL586; _LL594: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL586; _LL586:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp584, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp609=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp609 !=  0; _temp609=(( struct Cyc_List_List*)
_check_null( _temp609))->tl){ struct _tuple4 _temp612; struct Cyc_Absyn_Exp*
_temp613; struct Cyc_List_List* _temp615; struct _tuple4* _temp610=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp609))->hd; _temp612=*
_temp610; _LL616: _temp615= _temp612.f1; goto _LL614; _LL614: _temp613= _temp612.f2;
goto _LL611; _LL611: if( _temp615 ==  0){({ void* _temp617[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp617, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp615))->tl !=  0){({ void* _temp618[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct", sizeof( unsigned char), 41u),
_tag_arr( _temp618, sizeof( void*), 0u));});}{ void* _temp619=( void*)(( struct
Cyc_List_List*) _check_null( _temp615))->hd; struct _tagged_arr* _temp625;
_LL621: if(*(( int*) _temp619) ==  Cyc_Absyn_FieldName){ _LL626: _temp625=((
struct Cyc_Absyn_FieldName_struct*) _temp619)->f1; goto _LL622;} else{ goto
_LL623;} _LL623: goto _LL624; _LL622: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp(
lhs, _temp625, 0);{ void* _temp627=( void*) _temp613->r; struct Cyc_List_List*
_temp637; struct Cyc_Absyn_Exp* _temp639; struct Cyc_Absyn_Exp* _temp641; struct
Cyc_Absyn_Vardecl* _temp643; struct Cyc_List_List* _temp645; void* _temp647;
_LL629: if(*(( int*) _temp627) ==  Cyc_Absyn_Array_e){ _LL638: _temp637=((
struct Cyc_Absyn_Array_e_struct*) _temp627)->f1; goto _LL630;} else{ goto _LL631;}
_LL631: if(*(( int*) _temp627) ==  Cyc_Absyn_Comprehension_e){ _LL644: _temp643=((
struct Cyc_Absyn_Comprehension_e_struct*) _temp627)->f1; goto _LL642; _LL642:
_temp641=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp627)->f2; goto _LL640;
_LL640: _temp639=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp627)->f3;
goto _LL632;} else{ goto _LL633;} _LL633: if(*(( int*) _temp627) ==  Cyc_Absyn_AnonStruct_e){
_LL648: _temp647=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp627)->f1;
goto _LL646; _LL646: _temp645=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp627)->f2;
goto _LL634;} else{ goto _LL635;} _LL635: goto _LL636; _LL630: s= Cyc_Toc_init_array(
nv, lval, _temp637, s); goto _LL628; _LL632: s= Cyc_Toc_init_comprehension( nv,
lval, _temp643, _temp641, _temp639, s, 0); goto _LL628; _LL634: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp647, _temp645, s); goto _LL628; _LL636: Cyc_Toc_exp_to_c( nv,
_temp613); if( Cyc_Toc_is_poly_field( struct_type, _temp625)){ _temp613= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp613, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp613, 0), 0), s, 0); goto _LL628; _LL628:;} goto
_LL620;} _LL624:( int) _throw(({ void* _temp649[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp649, sizeof( void*), 0u));})); _LL620:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp650= _new_region();
struct _RegionHandle* r=& _temp650; _push_region( r);{ struct Cyc_List_List*
_temp651=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp652= Cyc_Toc_add_tuple_type( _temp651); struct
_tuple0* _temp653= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp654= Cyc_Absyn_var_exp(
_temp653, 0); struct Cyc_Absyn_Stmt* _temp655= Cyc_Absyn_exp_stmt( _temp654, 0);
struct Cyc_Absyn_Exp*(* _temp656)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp657=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp657); for( 0; _temp657
!=  0;( _temp657=(( struct Cyc_List_List*) _check_null( _temp657))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp657))->hd; struct Cyc_Absyn_Exp* lval= _temp656( _temp654, Cyc_Absyn_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp658=( void*) e->r; struct Cyc_List_List*
_temp666; struct Cyc_Absyn_Exp* _temp668; struct Cyc_Absyn_Exp* _temp670; struct
Cyc_Absyn_Vardecl* _temp672; _LL660: if(*(( int*) _temp658) ==  Cyc_Absyn_Array_e){
_LL667: _temp666=(( struct Cyc_Absyn_Array_e_struct*) _temp658)->f1; goto _LL661;}
else{ goto _LL662;} _LL662: if(*(( int*) _temp658) ==  Cyc_Absyn_Comprehension_e){
_LL673: _temp672=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp658)->f1;
goto _LL671; _LL671: _temp670=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp658)->f2; goto _LL669; _LL669: _temp668=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp658)->f3; goto _LL663;} else{ goto _LL664;} _LL664: goto _LL665; _LL661:
_temp655= Cyc_Toc_init_array( nv, lval, _temp666, _temp655); goto _LL659; _LL663:
_temp655= Cyc_Toc_init_comprehension( nv, lval, _temp672, _temp670, _temp668,
_temp655, 0); goto _LL659; _LL665: Cyc_Toc_exp_to_c( nv, e); _temp655= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp656( _temp654, Cyc_Absyn_fieldname(
i), 0), e, 0), 0), _temp655, 0); goto _LL659; _LL659:;}}}{ struct Cyc_Absyn_Exp*
_temp674= Cyc_Toc_make_struct( nv, _temp653, Cyc_Absyn_strct( _temp652),
_temp655, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp674;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp675= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp676= Cyc_Absyn_var_exp( _temp675, 0); struct Cyc_Absyn_Stmt*
_temp677= Cyc_Absyn_exp_stmt( _temp676, 0); struct Cyc_Absyn_Exp*(* _temp678)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp679= _new_region(); struct _RegionHandle* r=& _temp679;
_push_region( r);{ struct Cyc_List_List* _temp680=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp680 !=  0; _temp680=(( struct Cyc_List_List*) _check_null( _temp680))->tl){
struct _tuple4 _temp683; struct Cyc_Absyn_Exp* _temp684; struct Cyc_List_List*
_temp686; struct _tuple4* _temp681=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp680))->hd; _temp683=* _temp681; _LL687: _temp686= _temp683.f1;
goto _LL685; _LL685: _temp684= _temp683.f2; goto _LL682; _LL682: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp684->topt))->v): 0; if( _temp686 ==  0){({ void* _temp688[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp688, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp686))->tl !=  0){ struct _tuple0*
_temp689= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp690= Cyc_Absyn_var_exp(
_temp689, 0); for( 0; _temp686 !=  0; _temp686=(( struct Cyc_List_List*)
_check_null( _temp686))->tl){ void* _temp691=( void*)(( struct Cyc_List_List*)
_check_null( _temp686))->hd; struct _tagged_arr* _temp697; _LL693: if(*(( int*)
_temp691) ==  Cyc_Absyn_FieldName){ _LL698: _temp697=(( struct Cyc_Absyn_FieldName_struct*)
_temp691)->f1; goto _LL694;} else{ goto _LL695;} _LL695: goto _LL696; _LL694:
if( Cyc_Toc_is_poly_field( struct_type, _temp697)){ _temp690= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp690, 0);} _temp677= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp678( _temp676, _temp697, 0), _temp690, 0), 0),
_temp677, 0); goto _LL692; _LL696:( int) _throw(({ void* _temp699[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp699, sizeof( void*), 0u));})); _LL692:;} Cyc_Toc_exp_to_c( nv, _temp684);
_temp677= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp690,
_temp684, 0), 0), _temp677, 0);} else{ void* _temp700=( void*)(( struct Cyc_List_List*)
_check_null( _temp686))->hd; struct _tagged_arr* _temp706; _LL702: if(*(( int*)
_temp700) ==  Cyc_Absyn_FieldName){ _LL707: _temp706=(( struct Cyc_Absyn_FieldName_struct*)
_temp700)->f1; goto _LL703;} else{ goto _LL704;} _LL704: goto _LL705; _LL703: {
struct Cyc_Absyn_Exp* lval= _temp678( _temp676, _temp706, 0);{ void* _temp708=(
void*) _temp684->r; struct Cyc_List_List* _temp718; struct Cyc_Absyn_Exp*
_temp720; struct Cyc_Absyn_Exp* _temp722; struct Cyc_Absyn_Vardecl* _temp724;
struct Cyc_List_List* _temp726; void* _temp728; _LL710: if(*(( int*) _temp708)
==  Cyc_Absyn_Array_e){ _LL719: _temp718=(( struct Cyc_Absyn_Array_e_struct*)
_temp708)->f1; goto _LL711;} else{ goto _LL712;} _LL712: if(*(( int*) _temp708)
==  Cyc_Absyn_Comprehension_e){ _LL725: _temp724=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp708)->f1; goto _LL723; _LL723: _temp722=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp708)->f2; goto _LL721; _LL721: _temp720=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp708)->f3; goto _LL713;} else{ goto _LL714;} _LL714: if(*(( int*) _temp708)
==  Cyc_Absyn_AnonStruct_e){ _LL729: _temp728=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp708)->f1; goto _LL727; _LL727: _temp726=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp708)->f2; goto _LL715;} else{ goto _LL716;} _LL716: goto _LL717; _LL711:
_temp677= Cyc_Toc_init_array( nv, lval, _temp718, _temp677); goto _LL709; _LL713:
_temp677= Cyc_Toc_init_comprehension( nv, lval, _temp724, _temp722, _temp720,
_temp677, 0); goto _LL709; _LL715: _temp677= Cyc_Toc_init_anon_struct( nv, lval,
_temp728, _temp726, _temp677); goto _LL709; _LL717: Cyc_Toc_exp_to_c( nv,
_temp684); if( Cyc_Toc_is_poly_field( struct_type, _temp706)){ _temp684= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp684, 0);} _temp677= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp684, 0), 0), _temp677, 0); goto _LL709; _LL709:;}
goto _LL701;} _LL705:( int) _throw(({ void* _temp730[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp730, sizeof( void*), 0u));})); _LL701:;}}}{ struct Cyc_Absyn_Exp* _temp731=
Cyc_Toc_make_struct( nv, _temp675, Cyc_Absyn_strctq( tdn), _temp677, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp731;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp732=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp732[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp733; _temp733.tag= Cyc_Absyn_Increment_e;
_temp733.f1= e1; _temp733.f2=( void*) incr; _temp733;}); _temp732;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp734=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp744; struct Cyc_Absyn_Exp*
_temp746; void* _temp748; struct _tagged_arr* _temp750; struct Cyc_Absyn_Exp*
_temp752; _LL736: if(*(( int*) _temp734) ==  Cyc_Absyn_StmtExp_e){ _LL745:
_temp744=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp734)->f1; goto _LL737;}
else{ goto _LL738;} _LL738: if(*(( int*) _temp734) ==  Cyc_Absyn_Cast_e){ _LL749:
_temp748=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp734)->f1; goto _LL747;
_LL747: _temp746=(( struct Cyc_Absyn_Cast_e_struct*) _temp734)->f2; goto _LL739;}
else{ goto _LL740;} _LL740: if(*(( int*) _temp734) ==  Cyc_Absyn_StructMember_e){
_LL753: _temp752=(( struct Cyc_Absyn_StructMember_e_struct*) _temp734)->f1; goto
_LL751; _LL751: _temp750=(( struct Cyc_Absyn_StructMember_e_struct*) _temp734)->f2;
goto _LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL737: Cyc_Toc_lvalue_assign_stmt(
_temp744, fs, f, f_env); goto _LL735; _LL739: Cyc_Toc_lvalue_assign( _temp746,
fs, f, f_env); goto _LL735; _LL741:( void*)( e1->r=( void*)(( void*) _temp752->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp754=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp754->hd=( void*) _temp750; _temp754->tl= fs; _temp754;}), f, f_env); goto
_LL735; _LL743: if( ! Cyc_Absyn_is_lvalue( e1)){({ struct Cyc_Std_String_pa_struct
_temp756; _temp756.tag= Cyc_Std_String_pa; _temp756.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp755[ 1u]={& _temp756};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u), _tag_arr( _temp755, sizeof( void*), 1u));}});}{
struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs !=  0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd,
e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto
_LL735;} _LL735:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp757=( void*) s->r; struct Cyc_Absyn_Exp*
_temp767; struct Cyc_Absyn_Stmt* _temp769; struct Cyc_Absyn_Decl* _temp771;
struct Cyc_Absyn_Stmt* _temp773; _LL759: if(( unsigned int) _temp757 >  1u?*((
int*) _temp757) ==  Cyc_Absyn_Exp_s: 0){ _LL768: _temp767=(( struct Cyc_Absyn_Exp_s_struct*)
_temp757)->f1; goto _LL760;} else{ goto _LL761;} _LL761: if(( unsigned int)
_temp757 >  1u?*(( int*) _temp757) ==  Cyc_Absyn_Decl_s: 0){ _LL772: _temp771=((
struct Cyc_Absyn_Decl_s_struct*) _temp757)->f1; goto _LL770; _LL770: _temp769=((
struct Cyc_Absyn_Decl_s_struct*) _temp757)->f2; goto _LL762;} else{ goto _LL763;}
_LL763: if(( unsigned int) _temp757 >  1u?*(( int*) _temp757) ==  Cyc_Absyn_Seq_s:
0){ _LL774: _temp773=(( struct Cyc_Absyn_Seq_s_struct*) _temp757)->f2; goto
_LL764;} else{ goto _LL765;} _LL765: goto _LL766; _LL760: Cyc_Toc_lvalue_assign(
_temp767, fs, f, f_env); goto _LL758; _LL762: Cyc_Toc_lvalue_assign_stmt(
_temp769, fs, f, f_env); goto _LL758; _LL764: Cyc_Toc_lvalue_assign_stmt(
_temp773, fs, f, f_env); goto _LL758; _LL766:({ struct Cyc_Std_String_pa_struct
_temp776; _temp776.tag= Cyc_Std_String_pa; _temp776.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp775[ 1u]={& _temp776};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s", sizeof(
unsigned char), 23u), _tag_arr( _temp775, sizeof( void*), 1u));}}); goto _LL758;
_LL758:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2,
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x ==  0){ return 0;} result=({ struct
Cyc_List_List* _temp777=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp777->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp777->tl= 0; _temp777;}); prev= result; for( x=((
struct Cyc_List_List*) _check_null( x))->tl; x !=  0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp778=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp778->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp778->tl= 0; _temp778;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp779=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp779->f1= 0; _temp779->f2= e; _temp779;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp780=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp786; _LL782: if((
unsigned int) _temp780 >  4u?*(( int*) _temp780) ==  Cyc_Absyn_PointerType: 0){
_LL787: _temp786=(( struct Cyc_Absyn_PointerType_struct*) _temp780)->f1; goto
_LL783;} else{ goto _LL784;} _LL784: goto _LL785; _LL783: return _temp786;
_LL785:( int) _throw(({ void* _temp788[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("get_ptr_typ: not a pointer!",
sizeof( unsigned char), 28u), _tag_arr( _temp788, sizeof( void*), 0u));}));
_LL781:;} struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp789=( void*) e->r; if( e->topt ==  0){({
struct Cyc_Std_String_pa_struct _temp791; _temp791.tag= Cyc_Std_String_pa;
_temp791.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp790[ 1u]={&
_temp791};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u), _tag_arr(
_temp790, sizeof( void*), 1u));}}); return;}{ void* old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp792= _temp789; void*
_temp876; void* _temp878; struct _tuple0* _temp880; struct _tuple0* _temp882;
struct Cyc_List_List* _temp884; void* _temp886; void* _temp888; struct Cyc_Absyn_Exp*
_temp890; struct Cyc_Absyn_Exp* _temp892; struct Cyc_Core_Opt* _temp894; struct
Cyc_Absyn_Exp* _temp896; struct Cyc_Absyn_Exp* _temp898; struct Cyc_Absyn_Exp*
_temp900; struct Cyc_Absyn_Exp* _temp902; struct Cyc_Absyn_Exp* _temp904; struct
Cyc_Absyn_Exp* _temp906; struct Cyc_List_List* _temp908; struct Cyc_Absyn_Exp*
_temp910; struct Cyc_Absyn_VarargCallInfo* _temp912; struct Cyc_List_List*
_temp914; struct Cyc_Absyn_Exp* _temp916; struct Cyc_Absyn_VarargCallInfo*
_temp918; struct Cyc_Absyn_VarargCallInfo _temp920; struct Cyc_Absyn_VarargInfo*
_temp921; struct Cyc_List_List* _temp923; int _temp925; struct Cyc_List_List*
_temp927; struct Cyc_Absyn_Exp* _temp929; struct Cyc_Absyn_Exp* _temp931; struct
Cyc_Absyn_Exp* _temp933; struct Cyc_List_List* _temp935; struct Cyc_Absyn_Exp*
_temp937; struct Cyc_Absyn_Exp* _temp939; void* _temp941; void** _temp943;
struct Cyc_Absyn_Exp* _temp944; struct Cyc_Absyn_Exp* _temp946; struct Cyc_Absyn_Exp*
_temp948; struct Cyc_Absyn_Exp* _temp950; void* _temp952; void* _temp954; struct
_tagged_arr* _temp956; void* _temp958; void* _temp960; unsigned int _temp962;
void* _temp964; struct Cyc_Absyn_Exp* _temp966; struct _tagged_arr* _temp968;
struct Cyc_Absyn_Exp* _temp970; struct _tagged_arr* _temp972; struct Cyc_Absyn_Exp*
_temp974; struct Cyc_Absyn_Exp* _temp976; struct Cyc_Absyn_Exp* _temp978; struct
Cyc_List_List* _temp980; struct Cyc_List_List* _temp982; struct Cyc_Absyn_Exp*
_temp984; struct Cyc_Absyn_Exp* _temp986; struct Cyc_Absyn_Vardecl* _temp988;
struct Cyc_Absyn_Structdecl* _temp990; struct Cyc_List_List* _temp992; struct
Cyc_Core_Opt* _temp994; struct _tuple0* _temp996; struct Cyc_List_List* _temp998;
void* _temp1000; struct Cyc_Absyn_Tunionfield* _temp1002; struct Cyc_Absyn_Tuniondecl*
_temp1004; struct Cyc_List_List* _temp1006; struct Cyc_Absyn_Tunionfield*
_temp1008; struct Cyc_Absyn_Tuniondecl* _temp1010; struct Cyc_List_List*
_temp1012; void* _temp1014; struct Cyc_Absyn_Exp* _temp1016; struct Cyc_Absyn_Stmt*
_temp1018; struct Cyc_Absyn_Fndecl* _temp1020; _LL794: if(*(( int*) _temp792) == 
Cyc_Absyn_Const_e){ _LL877: _temp876=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp792)->f1; if( _temp876 == ( void*) Cyc_Absyn_Null_c){ goto _LL795;} else{
goto _LL796;}} else{ goto _LL796;} _LL796: if(*(( int*) _temp792) ==  Cyc_Absyn_Const_e){
goto _LL797;} else{ goto _LL798;} _LL798: if(*(( int*) _temp792) ==  Cyc_Absyn_Var_e){
_LL881: _temp880=(( struct Cyc_Absyn_Var_e_struct*) _temp792)->f1; goto _LL879;
_LL879: _temp878=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp792)->f2; goto
_LL799;} else{ goto _LL800;} _LL800: if(*(( int*) _temp792) ==  Cyc_Absyn_UnknownId_e){
_LL883: _temp882=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp792)->f1; goto
_LL801;} else{ goto _LL802;} _LL802: if(*(( int*) _temp792) ==  Cyc_Absyn_Primop_e){
_LL887: _temp886=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp792)->f1;
goto _LL885; _LL885: _temp884=(( struct Cyc_Absyn_Primop_e_struct*) _temp792)->f2;
goto _LL803;} else{ goto _LL804;} _LL804: if(*(( int*) _temp792) ==  Cyc_Absyn_Increment_e){
_LL891: _temp890=(( struct Cyc_Absyn_Increment_e_struct*) _temp792)->f1; goto
_LL889; _LL889: _temp888=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp792)->f2; goto _LL805;} else{ goto _LL806;} _LL806: if(*(( int*) _temp792)
==  Cyc_Absyn_AssignOp_e){ _LL897: _temp896=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp792)->f1; goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp792)->f2; goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp792)->f3; goto _LL807;} else{ goto _LL808;} _LL808: if(*(( int*) _temp792)
==  Cyc_Absyn_Conditional_e){ _LL903: _temp902=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp792)->f1; goto _LL901; _LL901: _temp900=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp792)->f2; goto _LL899; _LL899: _temp898=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp792)->f3; goto _LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp792)
==  Cyc_Absyn_SeqExp_e){ _LL907: _temp906=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp792)->f1; goto _LL905; _LL905: _temp904=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp792)->f2; goto _LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp792)
==  Cyc_Absyn_UnknownCall_e){ _LL911: _temp910=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp792)->f1; goto _LL909; _LL909: _temp908=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp792)->f2; goto _LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp792)
==  Cyc_Absyn_FnCall_e){ _LL917: _temp916=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp792)->f1; goto _LL915; _LL915: _temp914=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp792)->f2; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp792)->f3; if( _temp912 ==  0){ goto _LL815;} else{ goto _LL816;}} else{
goto _LL816;} _LL816: if(*(( int*) _temp792) ==  Cyc_Absyn_FnCall_e){ _LL930:
_temp929=(( struct Cyc_Absyn_FnCall_e_struct*) _temp792)->f1; goto _LL928;
_LL928: _temp927=(( struct Cyc_Absyn_FnCall_e_struct*) _temp792)->f2; goto
_LL919; _LL919: _temp918=(( struct Cyc_Absyn_FnCall_e_struct*) _temp792)->f3;
if( _temp918 ==  0){ goto _LL818;} else{ _temp920=* _temp918; _LL926: _temp925=
_temp920.num_varargs; goto _LL924; _LL924: _temp923= _temp920.injectors; goto
_LL922; _LL922: _temp921= _temp920.vai; goto _LL817;}} else{ goto _LL818;}
_LL818: if(*(( int*) _temp792) ==  Cyc_Absyn_Throw_e){ _LL932: _temp931=((
struct Cyc_Absyn_Throw_e_struct*) _temp792)->f1; goto _LL819;} else{ goto _LL820;}
_LL820: if(*(( int*) _temp792) ==  Cyc_Absyn_NoInstantiate_e){ _LL934: _temp933=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp792)->f1; goto _LL821;} else{
goto _LL822;} _LL822: if(*(( int*) _temp792) ==  Cyc_Absyn_Instantiate_e){
_LL938: _temp937=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp792)->f1; goto
_LL936; _LL936: _temp935=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp792)->f2;
goto _LL823;} else{ goto _LL824;} _LL824: if(*(( int*) _temp792) ==  Cyc_Absyn_Cast_e){
_LL942: _temp941=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp792)->f1;
_temp943=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp792)->f1);
goto _LL940; _LL940: _temp939=(( struct Cyc_Absyn_Cast_e_struct*) _temp792)->f2;
goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp792) ==  Cyc_Absyn_Address_e){
_LL945: _temp944=(( struct Cyc_Absyn_Address_e_struct*) _temp792)->f1; goto
_LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp792) ==  Cyc_Absyn_New_e){
_LL949: _temp948=(( struct Cyc_Absyn_New_e_struct*) _temp792)->f1; goto _LL947;
_LL947: _temp946=(( struct Cyc_Absyn_New_e_struct*) _temp792)->f2; goto _LL829;}
else{ goto _LL830;} _LL830: if(*(( int*) _temp792) ==  Cyc_Absyn_Sizeofexp_e){
_LL951: _temp950=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp792)->f1; goto
_LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp792) ==  Cyc_Absyn_Sizeoftyp_e){
_LL953: _temp952=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp792)->f1;
goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp792) ==  Cyc_Absyn_Gentyp_e){
goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp792) ==  Cyc_Absyn_Offsetof_e){
_LL959: _temp958=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp792)->f1;
goto _LL955; _LL955: _temp954=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp792)->f2; if(*(( int*) _temp954) ==  Cyc_Absyn_StructField){ _LL957:
_temp956=(( struct Cyc_Absyn_StructField_struct*) _temp954)->f1; goto _LL837;}
else{ goto _LL838;}} else{ goto _LL838;} _LL838: if(*(( int*) _temp792) ==  Cyc_Absyn_Offsetof_e){
_LL965: _temp964=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp792)->f1;
goto _LL961; _LL961: _temp960=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp792)->f2; if(*(( int*) _temp960) ==  Cyc_Absyn_TupleIndex){ _LL963:
_temp962=(( struct Cyc_Absyn_TupleIndex_struct*) _temp960)->f1; goto _LL839;}
else{ goto _LL840;}} else{ goto _LL840;} _LL840: if(*(( int*) _temp792) ==  Cyc_Absyn_Deref_e){
_LL967: _temp966=(( struct Cyc_Absyn_Deref_e_struct*) _temp792)->f1; goto _LL841;}
else{ goto _LL842;} _LL842: if(*(( int*) _temp792) ==  Cyc_Absyn_StructMember_e){
_LL971: _temp970=(( struct Cyc_Absyn_StructMember_e_struct*) _temp792)->f1; goto
_LL969; _LL969: _temp968=(( struct Cyc_Absyn_StructMember_e_struct*) _temp792)->f2;
goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp792) ==  Cyc_Absyn_StructArrow_e){
_LL975: _temp974=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp792)->f1; goto
_LL973; _LL973: _temp972=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp792)->f2;
goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp792) ==  Cyc_Absyn_Subscript_e){
_LL979: _temp978=(( struct Cyc_Absyn_Subscript_e_struct*) _temp792)->f1; goto
_LL977; _LL977: _temp976=(( struct Cyc_Absyn_Subscript_e_struct*) _temp792)->f2;
goto _LL847;} else{ goto _LL848;} _LL848: if(*(( int*) _temp792) ==  Cyc_Absyn_Tuple_e){
_LL981: _temp980=(( struct Cyc_Absyn_Tuple_e_struct*) _temp792)->f1; goto _LL849;}
else{ goto _LL850;} _LL850: if(*(( int*) _temp792) ==  Cyc_Absyn_Array_e){
_LL983: _temp982=(( struct Cyc_Absyn_Array_e_struct*) _temp792)->f1; goto _LL851;}
else{ goto _LL852;} _LL852: if(*(( int*) _temp792) ==  Cyc_Absyn_Comprehension_e){
_LL989: _temp988=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp792)->f1;
goto _LL987; _LL987: _temp986=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp792)->f2; goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp792)->f3; goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp792)
==  Cyc_Absyn_Struct_e){ _LL997: _temp996=(( struct Cyc_Absyn_Struct_e_struct*)
_temp792)->f1; goto _LL995; _LL995: _temp994=(( struct Cyc_Absyn_Struct_e_struct*)
_temp792)->f2; goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_Struct_e_struct*)
_temp792)->f3; goto _LL991; _LL991: _temp990=(( struct Cyc_Absyn_Struct_e_struct*)
_temp792)->f4; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp792)
==  Cyc_Absyn_AnonStruct_e){ _LL1001: _temp1000=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp792)->f1; goto _LL999; _LL999: _temp998=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp792)->f2; goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp792)
==  Cyc_Absyn_Tunion_e){ _LL1007: _temp1006=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp792)->f3; if( _temp1006 ==  0){ goto _LL1005;} else{ goto _LL860;} _LL1005:
_temp1004=(( struct Cyc_Absyn_Tunion_e_struct*) _temp792)->f4; goto _LL1003;
_LL1003: _temp1002=(( struct Cyc_Absyn_Tunion_e_struct*) _temp792)->f5; goto
_LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp792) ==  Cyc_Absyn_Tunion_e){
_LL1013: _temp1012=(( struct Cyc_Absyn_Tunion_e_struct*) _temp792)->f3; goto
_LL1011; _LL1011: _temp1010=(( struct Cyc_Absyn_Tunion_e_struct*) _temp792)->f4;
goto _LL1009; _LL1009: _temp1008=(( struct Cyc_Absyn_Tunion_e_struct*) _temp792)->f5;
goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp792) ==  Cyc_Absyn_Enum_e){
goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp792) ==  Cyc_Absyn_Malloc_e){
_LL1017: _temp1016=(( struct Cyc_Absyn_Malloc_e_struct*) _temp792)->f1; goto
_LL1015; _LL1015: _temp1014=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp792)->f2; goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp792)
==  Cyc_Absyn_StmtExp_e){ _LL1019: _temp1018=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp792)->f1; goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp792)
==  Cyc_Absyn_UnresolvedMem_e){ goto _LL869;} else{ goto _LL870;} _LL870: if(*((
int*) _temp792) ==  Cyc_Absyn_CompoundLit_e){ goto _LL871;} else{ goto _LL872;}
_LL872: if(*(( int*) _temp792) ==  Cyc_Absyn_Codegen_e){ _LL1021: _temp1020=((
struct Cyc_Absyn_Codegen_e_struct*) _temp792)->f1; goto _LL873;} else{ goto
_LL874;} _LL874: if(*(( int*) _temp792) ==  Cyc_Absyn_Fill_e){ goto _LL875;}
else{ goto _LL793;} _LL795: { struct Cyc_Absyn_Exp* _temp1022= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1022,
_temp1022))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1023[ 3u]; _temp1023[ 2u]=
_temp1022; _temp1023[ 1u]= _temp1022; _temp1023[ 0u]= _temp1022;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1023, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL793;} _LL797: goto _LL793; _LL799:{ struct _handler_cons
_temp1024; _push_handler(& _temp1024);{ int _temp1026= 0; if( setjmp( _temp1024.handler)){
_temp1026= 1;} if( ! _temp1026){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp880))->r));; _pop_handler();} else{ void* _temp1025=( void*) _exn_thrown;
void* _temp1028= _temp1025; _LL1030: if( _temp1028 ==  Cyc_Dict_Absent){ goto
_LL1031;} else{ goto _LL1032;} _LL1032: goto _LL1033; _LL1031:({ struct Cyc_Std_String_pa_struct
_temp1035; _temp1035.tag= Cyc_Std_String_pa; _temp1035.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp880);{ void* _temp1034[ 1u]={& _temp1035};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1034, sizeof( void*), 1u));}});
return; _LL1033:( void) _throw( _temp1028); _LL1029:;}}} goto _LL793; _LL801:({
void* _temp1036[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1036, sizeof( void*), 0u));}); goto _LL793; _LL803: { struct Cyc_List_List*
_temp1037=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp884);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp884);{
void* _temp1038= _temp886; _LL1040: if( _temp1038 == ( void*) Cyc_Absyn_Size){
goto _LL1041;} else{ goto _LL1042;} _LL1042: if( _temp1038 == ( void*) Cyc_Absyn_Plus){
goto _LL1043;} else{ goto _LL1044;} _LL1044: if( _temp1038 == ( void*) Cyc_Absyn_Minus){
goto _LL1045;} else{ goto _LL1046;} _LL1046: if( _temp1038 == ( void*) Cyc_Absyn_Eq){
goto _LL1047;} else{ goto _LL1048;} _LL1048: if( _temp1038 == ( void*) Cyc_Absyn_Neq){
goto _LL1049;} else{ goto _LL1050;} _LL1050: if( _temp1038 == ( void*) Cyc_Absyn_Gt){
goto _LL1051;} else{ goto _LL1052;} _LL1052: if( _temp1038 == ( void*) Cyc_Absyn_Gte){
goto _LL1053;} else{ goto _LL1054;} _LL1054: if( _temp1038 == ( void*) Cyc_Absyn_Lt){
goto _LL1055;} else{ goto _LL1056;} _LL1056: if( _temp1038 == ( void*) Cyc_Absyn_Lte){
goto _LL1057;} else{ goto _LL1058;} _LL1058: goto _LL1059; _LL1041: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp884))->hd;{
void* _temp1060= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1068; struct Cyc_Absyn_PtrInfo
_temp1070; struct Cyc_Absyn_Conref* _temp1072; void* _temp1074; _LL1062: if((
unsigned int) _temp1060 >  4u?*(( int*) _temp1060) ==  Cyc_Absyn_ArrayType: 0){
_LL1069: _temp1068=(( struct Cyc_Absyn_ArrayType_struct*) _temp1060)->f3; goto
_LL1063;} else{ goto _LL1064;} _LL1064: if(( unsigned int) _temp1060 >  4u?*((
int*) _temp1060) ==  Cyc_Absyn_PointerType: 0){ _LL1071: _temp1070=(( struct Cyc_Absyn_PointerType_struct*)
_temp1060)->f1; _LL1075: _temp1074=( void*) _temp1070.elt_typ; goto _LL1073;
_LL1073: _temp1072= _temp1070.bounds; goto _LL1065;} else{ goto _LL1066;}
_LL1066: goto _LL1067; _LL1063:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1068))->r)); goto _LL1061; _LL1065:{ void* _temp1076= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1072); struct Cyc_Absyn_Exp* _temp1082; _LL1078: if(
_temp1076 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1079;} else{ goto _LL1080;}
_LL1080: if(( unsigned int) _temp1076 >  1u?*(( int*) _temp1076) ==  Cyc_Absyn_Upper_b:
0){ _LL1083: _temp1082=(( struct Cyc_Absyn_Upper_b_struct*) _temp1076)->f1; goto
_LL1081;} else{ goto _LL1077;} _LL1079:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1084[ 2u]; _temp1084[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1074), 0); _temp1084[ 0u]=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp884))->hd;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1084, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r)); goto _LL1077;
_LL1081:( void*)( e->r=( void*)(( void*) _temp1082->r)); goto _LL1077; _LL1077:;}
goto _LL1061; _LL1067:({ struct Cyc_Std_String_pa_struct _temp1087; _temp1087.tag=
Cyc_Std_String_pa; _temp1087.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp1086; _temp1086.tag= Cyc_Std_String_pa; _temp1086.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
void* _temp1085[ 2u]={& _temp1086,& _temp1087};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u), _tag_arr( _temp1085, sizeof( void*), 2u));}}});
return; _LL1061:;} goto _LL1039;} _LL1043: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1037))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp884))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp884))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1088[ 3u]; _temp1088[ 2u]=
e2; _temp1088[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1088[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1088, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1039;} _LL1045: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1037))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp884))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp884))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1037))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1089[ 3u]; _temp1089[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1089[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1089[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1089, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1039;} _LL1047: goto _LL1049; _LL1049: goto _LL1051; _LL1051: goto
_LL1053; _LL1053: goto _LL1055; _LL1055: goto _LL1057; _LL1057: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp884))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp884))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1037))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1037))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1039;} _LL1059: goto
_LL1039; _LL1039:;} goto _LL793;} _LL805: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp890->topt))->v; Cyc_Toc_exp_to_c( nv, _temp890);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1090=( _temp888 == ( void*) Cyc_Absyn_PostInc?
1: _temp888 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1091=( _temp888 == ( void*) Cyc_Absyn_PreInc?
1: _temp888 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1090,({ struct Cyc_Absyn_Exp* _temp1092[ 3u];
_temp1092[ 2u]= Cyc_Absyn_signed_int_exp( _temp1091, 0); _temp1092[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1092[ 0u]= Cyc_Absyn_address_exp( _temp890,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1092, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp890): 0){ Cyc_Toc_lvalue_assign(
_temp890, 0, Cyc_Toc_incr_lvalue, _temp888);( void*)( e->r=( void*)(( void*)
_temp890->r));} goto _LL793;}} _LL807: { int e1_poly= Cyc_Toc_is_poly_project(
_temp896); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp896->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp892->topt))->v; Cyc_Toc_exp_to_c( nv, _temp896); Cyc_Toc_exp_to_c(
nv, _temp892);{ int done= 0; if( _temp894 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1093=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp894))->v;
_LL1095: if( _temp1093 == ( void*) Cyc_Absyn_Plus){ goto _LL1096;} else{ goto
_LL1097;} _LL1097: if( _temp1093 == ( void*) Cyc_Absyn_Minus){ goto _LL1098;}
else{ goto _LL1099;} _LL1099: goto _LL1100; _LL1096: change= _temp892; goto
_LL1094; _LL1098: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp892,
0); goto _LL1094; _LL1100:( int) _throw(({ void* _temp1101[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u), _tag_arr(
_temp1101, sizeof( void*), 0u));})); goto _LL1094; _LL1094:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp* _temp1102[ 3u]; _temp1102[ 2u]= change; _temp1102[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); _temp1102[ 0u]= Cyc_Absyn_address_exp(
_temp896, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1102, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} if( !
done){ if( e1_poly){( void*)( _temp892->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*) _temp892->r, 0), 0))->r));}
if( ! Cyc_Absyn_is_lvalue( _temp896)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)( _temp896, 0,
Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1103=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7) *  1); _temp1103[ 0]=({ struct _tuple7
_temp1104; _temp1104.f1= _temp894; _temp1104.f2= _temp892; _temp1104;});
_temp1103;}));( void*)( e->r=( void*)(( void*) _temp896->r));}} goto _LL793;}}
_LL809: Cyc_Toc_exp_to_c( nv, _temp902); Cyc_Toc_exp_to_c( nv, _temp900); Cyc_Toc_exp_to_c(
nv, _temp898); goto _LL793; _LL811: Cyc_Toc_exp_to_c( nv, _temp906); Cyc_Toc_exp_to_c(
nv, _temp904); goto _LL793; _LL813: _temp916= _temp910; _temp914= _temp908; goto
_LL815; _LL815: Cyc_Toc_exp_to_c( nv, _temp916);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp914); goto _LL793; _LL817:{
struct _RegionHandle _temp1105= _new_region(); struct _RegionHandle* r=&
_temp1105; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp925, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp921->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1122=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1122[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1123; _temp1123.tag= Cyc_Absyn_ArrayType;
_temp1123.f1=( void*) cva_type; _temp1123.f2= Cyc_Toc_mt_tq; _temp1123.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1123;}); _temp1122;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp927); int num_normargs=
num_args -  _temp925; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp927=(( struct Cyc_List_List*) _check_null( _temp927))->tl)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd); new_args=({ struct Cyc_List_List* _temp1106=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1106->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp927))->hd);
_temp1106->tl= new_args; _temp1106;});}} new_args=({ struct Cyc_List_List*
_temp1107=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1107->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1108[ 3u]; _temp1108[ 2u]= num_varargs_exp; _temp1108[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1108[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1108, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1107->tl= new_args; _temp1107;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp929);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp929, new_args, 0), 0); if( _temp921->inject){ struct _tagged_arr vs=({
unsigned int _temp1118=( unsigned int) _temp925; struct _tuple0** _temp1119=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1118)); struct _tagged_arr _temp1121= _tag_arr( _temp1119, sizeof( struct
_tuple0*),( unsigned int) _temp925);{ unsigned int _temp1120= _temp1118;
unsigned int i; for( i= 0; i <  _temp1120; i ++){ _temp1119[ i]= Cyc_Toc_temp_var();}};
_temp1121;}); struct Cyc_List_List* _temp1109= 0;{ int i= _temp925 -  1; for( 0;
i >=  0; -- i){ _temp1109=({ struct Cyc_List_List* _temp1110=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1110->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1110->tl=
_temp1109; _temp1110;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1109, 0), s, 0);{ int i= 0;
for( 0; _temp927 !=  0;((( _temp927=(( struct Cyc_List_List*) _check_null(
_temp927))->tl, _temp923=(( struct Cyc_List_List*) _check_null( _temp923))->tl)),
++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd; void* arg_type=( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v; struct _tuple0* var=*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)); struct Cyc_Absyn_Exp*
varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield _temp1113;
struct Cyc_List_List* _temp1114; struct _tuple0* _temp1116; struct Cyc_Absyn_Tunionfield*
_temp1111=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp923))->hd; _temp1113=* _temp1111; _LL1117: _temp1116= _temp1113.name; goto
_LL1115; _LL1115: _temp1114= _temp1113.typs; goto _LL1112; _LL1112: { void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1114))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Absyn_fieldname(
1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1116, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1116, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp927
!=  0;( _temp927=(( struct Cyc_List_List*) _check_null( _temp927))->tl, ++ i)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp927))->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL793; _LL819: Cyc_Toc_exp_to_c( nv,
_temp931);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp931), 0))->r)); goto _LL793; _LL821: Cyc_Toc_exp_to_c(
nv, _temp933); goto _LL793; _LL823: Cyc_Toc_exp_to_c( nv, _temp937); for( 0;
_temp935 !=  0; _temp935=(( struct Cyc_List_List*) _check_null( _temp935))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp935))->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1124= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp935))->hd); _LL1126: if(( unsigned int) _temp1124 >  4u?*((
int*) _temp1124) ==  Cyc_Absyn_VarType: 0){ goto _LL1127;} else{ goto _LL1128;}
_LL1128: if(( unsigned int) _temp1124 >  4u?*(( int*) _temp1124) ==  Cyc_Absyn_TunionType:
0){ goto _LL1129;} else{ goto _LL1130;} _LL1130: goto _LL1131; _LL1127:
continue; _LL1129: continue; _LL1131:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp937, 0))->r)); goto _LL1125; _LL1125:;} break;}} goto _LL793; _LL825: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp939->topt))->v;
void* new_typ=* _temp943;* _temp943= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp939);{ struct _tuple8 _temp1133=({ struct _tuple8 _temp1132; _temp1132.f1=
Cyc_Tcutil_compress( old_t2); _temp1132.f2= Cyc_Tcutil_compress( new_typ);
_temp1132;}); void* _temp1141; struct Cyc_Absyn_PtrInfo _temp1143; void*
_temp1145; struct Cyc_Absyn_PtrInfo _temp1147; void* _temp1149; void* _temp1151;
struct Cyc_Absyn_PtrInfo _temp1153; _LL1135: _LL1146: _temp1145= _temp1133.f1;
if(( unsigned int) _temp1145 >  4u?*(( int*) _temp1145) ==  Cyc_Absyn_PointerType:
0){ _LL1148: _temp1147=(( struct Cyc_Absyn_PointerType_struct*) _temp1145)->f1;
goto _LL1142;} else{ goto _LL1137;} _LL1142: _temp1141= _temp1133.f2; if((
unsigned int) _temp1141 >  4u?*(( int*) _temp1141) ==  Cyc_Absyn_PointerType: 0){
_LL1144: _temp1143=(( struct Cyc_Absyn_PointerType_struct*) _temp1141)->f1; goto
_LL1136;} else{ goto _LL1137;} _LL1137: _LL1152: _temp1151= _temp1133.f1; if((
unsigned int) _temp1151 >  4u?*(( int*) _temp1151) ==  Cyc_Absyn_PointerType: 0){
_LL1154: _temp1153=(( struct Cyc_Absyn_PointerType_struct*) _temp1151)->f1; goto
_LL1150;} else{ goto _LL1139;} _LL1150: _temp1149= _temp1133.f2; if((
unsigned int) _temp1149 >  4u?*(( int*) _temp1149) ==  Cyc_Absyn_IntType: 0){
goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140; _LL1136: { int
_temp1155=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1147.nullable); int _temp1156=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1143.nullable); void* _temp1157= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1147.bounds); void* _temp1158= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1143.bounds);{ struct _tuple8 _temp1160=({ struct
_tuple8 _temp1159; _temp1159.f1= _temp1157; _temp1159.f2= _temp1158; _temp1159;});
void* _temp1170; void* _temp1172; void* _temp1174; void* _temp1176; struct Cyc_Absyn_Exp*
_temp1178; void* _temp1180; struct Cyc_Absyn_Exp* _temp1182; void* _temp1184;
_LL1162: _LL1173: _temp1172= _temp1160.f1; if(( unsigned int) _temp1172 >  1u?*((
int*) _temp1172) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1171;} else{ goto _LL1164;}
_LL1171: _temp1170= _temp1160.f2; if(( unsigned int) _temp1170 >  1u?*(( int*)
_temp1170) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1163;} else{ goto _LL1164;}
_LL1164: _LL1177: _temp1176= _temp1160.f1; if(( unsigned int) _temp1176 >  1u?*((
int*) _temp1176) ==  Cyc_Absyn_Upper_b: 0){ _LL1179: _temp1178=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1176)->f1; goto _LL1175;} else{ goto _LL1166;} _LL1175: _temp1174=
_temp1160.f2; if( _temp1174 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1165;}
else{ goto _LL1166;} _LL1166: _LL1185: _temp1184= _temp1160.f1; if( _temp1184 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1181;} else{ goto _LL1168;} _LL1181:
_temp1180= _temp1160.f2; if(( unsigned int) _temp1180 >  1u?*(( int*) _temp1180)
==  Cyc_Absyn_Upper_b: 0){ _LL1183: _temp1182=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1180)->f1; goto _LL1167;} else{ goto _LL1168;} _LL1168: goto _LL1169;
_LL1163: if( _temp1155? ! _temp1156: 0){ if( nv->toplevel){({ void* _temp1186[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1186, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp943, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1187=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1187->hd=( void*) _temp939;
_temp1187->tl= 0; _temp1187;}), 0), 0))->r));} goto _LL1161; _LL1165: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1178, _temp939))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1188[ 3u]; _temp1188[ 2u]=
_temp1178; _temp1188[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1143.elt_typ), 0); _temp1188[ 0u]= _temp939;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1188, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1161; _LL1167: if( nv->toplevel){({ void* _temp1189[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1189, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1190= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1192[ 3u]; _temp1192[ 2u]= _temp1182; _temp1192[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1147.elt_typ), 0); _temp1192[ 0u]= _temp939;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1192, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1156){( void*)(
_temp1190->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1191=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1191->hd=( void*) Cyc_Absyn_copy_exp( _temp1190);
_temp1191->tl= 0; _temp1191;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp943, _temp1190, 0))->r)); goto _LL1161;} _LL1169: goto _LL1161; _LL1161:;}
goto _LL1134;} _LL1138:{ void* _temp1193= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1153.bounds); _LL1195: if( _temp1193 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1196;} else{ goto _LL1197;} _LL1197: goto _LL1198; _LL1196:( void*)( _temp939->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp939->r,
_temp939->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1194; _LL1198: goto _LL1194;
_LL1194:;} goto _LL1134; _LL1140: goto _LL1134; _LL1134:;} goto _LL793;} _LL827:{
void* _temp1199=( void*) _temp944->r; struct Cyc_List_List* _temp1207; struct
_tuple0* _temp1209; struct Cyc_List_List* _temp1211; _LL1201: if(*(( int*)
_temp1199) ==  Cyc_Absyn_Struct_e){ _LL1210: _temp1209=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1199)->f1; goto _LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1199)->f3; goto _LL1202;} else{ goto _LL1203;} _LL1203: if(*(( int*)
_temp1199) ==  Cyc_Absyn_Tuple_e){ _LL1212: _temp1211=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1199)->f1; goto _LL1204;} else{ goto _LL1205;} _LL1205: goto _LL1206;
_LL1202: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1214;
_temp1214.tag= Cyc_Std_String_pa; _temp1214.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp944->loc);{ void* _temp1213[ 1u]={& _temp1214};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1213, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp944->topt))->v, 1, 0, _temp1207, _temp1209))->r)); goto
_LL1200; _LL1204: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1216;
_temp1216.tag= Cyc_Std_String_pa; _temp1216.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp944->loc);{ void* _temp1215[ 1u]={& _temp1216};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1215, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1211))->r));
goto _LL1200; _LL1206: Cyc_Toc_exp_to_c( nv, _temp944); if( ! Cyc_Absyn_is_lvalue(
_temp944)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp944, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp944, 0))->r));} goto _LL1200; _LL1200:;} goto _LL793; _LL829: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1218; _temp1218.tag= Cyc_Std_String_pa;
_temp1218.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp946->loc);{
void* _temp1217[ 1u]={& _temp1218};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1217, sizeof( void*), 1u));}});}{ void*
_temp1219=( void*) _temp946->r; struct Cyc_List_List* _temp1231; struct Cyc_Absyn_Exp*
_temp1233; struct Cyc_Absyn_Exp* _temp1235; struct Cyc_Absyn_Vardecl* _temp1237;
struct Cyc_List_List* _temp1239; struct _tuple0* _temp1241; struct Cyc_List_List*
_temp1243; _LL1221: if(*(( int*) _temp1219) ==  Cyc_Absyn_Array_e){ _LL1232:
_temp1231=(( struct Cyc_Absyn_Array_e_struct*) _temp1219)->f1; goto _LL1222;}
else{ goto _LL1223;} _LL1223: if(*(( int*) _temp1219) ==  Cyc_Absyn_Comprehension_e){
_LL1238: _temp1237=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1219)->f1;
goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1219)->f2; goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1219)->f3; goto _LL1224;} else{ goto _LL1225;} _LL1225: if(*(( int*)
_temp1219) ==  Cyc_Absyn_Struct_e){ _LL1242: _temp1241=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1219)->f1; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1219)->f3; goto _LL1226;} else{ goto _LL1227;} _LL1227: if(*(( int*)
_temp1219) ==  Cyc_Absyn_Tuple_e){ _LL1244: _temp1243=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1219)->f1; goto _LL1228;} else{ goto _LL1229;} _LL1229: goto _LL1230;
_LL1222: { struct _tuple0* _temp1245= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1246= Cyc_Absyn_var_exp( _temp1245, 0); struct Cyc_Absyn_Stmt* _temp1247=
Cyc_Toc_init_array( nv, _temp1246, _temp1231, Cyc_Absyn_exp_stmt( _temp1246, 0));
void* old_elt_typ;{ void* _temp1248= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1254; struct Cyc_Absyn_Tqual _temp1256; void* _temp1258; _LL1250: if((
unsigned int) _temp1248 >  4u?*(( int*) _temp1248) ==  Cyc_Absyn_PointerType: 0){
_LL1255: _temp1254=(( struct Cyc_Absyn_PointerType_struct*) _temp1248)->f1;
_LL1259: _temp1258=( void*) _temp1254.elt_typ; goto _LL1257; _LL1257: _temp1256=
_temp1254.tq; goto _LL1251;} else{ goto _LL1252;} _LL1252: goto _LL1253; _LL1251:
old_elt_typ= _temp1258; goto _LL1249; _LL1253: old_elt_typ=({ void* _temp1260[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1260, sizeof( void*), 0u));}); goto
_LL1249; _LL1249:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1261= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1262= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1231), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp948 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1262);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp948); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1262);}{
struct Cyc_Absyn_Exp* _temp1263= Cyc_Absyn_cast_exp( _temp1261, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1245,
_temp1261,( struct Cyc_Absyn_Exp*) _temp1263, _temp1247, 0), 0))->r)); goto
_LL1220;}}} _LL1224: { int is_tagged_ptr= 0;{ void* _temp1264= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1270; struct Cyc_Absyn_Conref* _temp1272;
struct Cyc_Absyn_Tqual _temp1274; void* _temp1276; _LL1266: if(( unsigned int)
_temp1264 >  4u?*(( int*) _temp1264) ==  Cyc_Absyn_PointerType: 0){ _LL1271:
_temp1270=(( struct Cyc_Absyn_PointerType_struct*) _temp1264)->f1; _LL1277:
_temp1276=( void*) _temp1270.elt_typ; goto _LL1275; _LL1275: _temp1274=
_temp1270.tq; goto _LL1273; _LL1273: _temp1272= _temp1270.bounds; goto _LL1267;}
else{ goto _LL1268;} _LL1268: goto _LL1269; _LL1267: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1272) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1265;
_LL1269:({ void* _temp1278[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1278, sizeof( void*), 0u));}); goto
_LL1265; _LL1265:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1233->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1235);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1237, Cyc_Absyn_var_exp( max, 0), _temp1233, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1279= _new_region(); struct _RegionHandle* r=&
_temp1279; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1299=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1299->hd=( void*)({ struct _tuple9* _temp1300=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1300->f1= max; _temp1300->f2=
Cyc_Absyn_uint_t; _temp1300->f3=( struct Cyc_Absyn_Exp*) _temp1235; _temp1300;});
_temp1299->tl= 0; _temp1299;}); struct Cyc_Absyn_Exp* ai; if( _temp948 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1280[ 2u]; _temp1280[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1280[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1280, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp948); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1281[
2u]; _temp1281[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1281[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1281, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1282=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1282->hd=( void*)({ struct _tuple9* _temp1283=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1283->f1= a; _temp1283->f2=
ptr_typ; _temp1283->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1283;}); _temp1282->tl=
decls; _temp1282;}); if( is_tagged_ptr){ struct _tuple0* _temp1284= Cyc_Toc_temp_var();
void* _temp1285= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1286=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1289[ 3u];
_temp1289[ 2u]= _temp1235; _temp1289[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1289[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1289, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1287=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1287->hd=( void*)({
struct _tuple9* _temp1288=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1288->f1= _temp1284; _temp1288->f2= _temp1285; _temp1288->f3=(
struct Cyc_Absyn_Exp*) _temp1286; _temp1288;}); _temp1287->tl= decls; _temp1287;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1284, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1290= decls; for( 0; _temp1290 !=  0;
_temp1290=(( struct Cyc_List_List*) _check_null( _temp1290))->tl){ struct Cyc_Absyn_Exp*
_temp1293; void* _temp1295; struct _tuple0* _temp1297; struct _tuple9 _temp1291=*((
struct _tuple9*)(( struct Cyc_List_List*) _check_null( _temp1290))->hd); _LL1298:
_temp1297= _temp1291.f1; goto _LL1296; _LL1296: _temp1295= _temp1291.f2; goto
_LL1294; _LL1294: _temp1293= _temp1291.f3; goto _LL1292; _LL1292: s= Cyc_Absyn_declare_stmt(
_temp1297, _temp1295, _temp1293, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1220;}}} _LL1226:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp946->topt))->v, 1, _temp948, _temp1239, _temp1241))->r)); goto _LL1220;
_LL1228:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp948,
_temp1243))->r)); goto _LL1220; _LL1230: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp946->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp948 ==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp948); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1301=( void*)
_temp946->r; struct Cyc_Absyn_Exp* _temp1307; void* _temp1309; _LL1303: if(*((
int*) _temp1301) ==  Cyc_Absyn_Cast_e){ _LL1310: _temp1309=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1301)->f1; goto _LL1308; _LL1308: _temp1307=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1301)->f2; goto _LL1304;} else{ goto _LL1305;} _LL1305: goto _LL1306;
_LL1304:{ struct _tuple8 _temp1312=({ struct _tuple8 _temp1311; _temp1311.f1=
Cyc_Tcutil_compress( _temp1309); _temp1311.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1307->topt))->v); _temp1311;}); void*
_temp1318; struct Cyc_Absyn_PtrInfo _temp1320; struct Cyc_Absyn_Conref*
_temp1322; void* _temp1324; struct Cyc_Absyn_PtrInfo _temp1326; struct Cyc_Absyn_Conref*
_temp1328; void* _temp1330; _LL1314: _LL1325: _temp1324= _temp1312.f1; if((
unsigned int) _temp1324 >  4u?*(( int*) _temp1324) ==  Cyc_Absyn_PointerType: 0){
_LL1327: _temp1326=(( struct Cyc_Absyn_PointerType_struct*) _temp1324)->f1;
_LL1331: _temp1330=( void*) _temp1326.elt_typ; goto _LL1329; _LL1329: _temp1328=
_temp1326.bounds; goto _LL1319;} else{ goto _LL1316;} _LL1319: _temp1318=
_temp1312.f2; if(( unsigned int) _temp1318 >  4u?*(( int*) _temp1318) ==  Cyc_Absyn_PointerType:
0){ _LL1321: _temp1320=(( struct Cyc_Absyn_PointerType_struct*) _temp1318)->f1;
_LL1323: _temp1322= _temp1320.bounds; goto _LL1315;} else{ goto _LL1316;}
_LL1316: goto _LL1317; _LL1315:{ struct _tuple8 _temp1333=({ struct _tuple8
_temp1332; _temp1332.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1328);
_temp1332.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1322); _temp1332;});
void* _temp1339; struct Cyc_Absyn_Exp* _temp1341; void* _temp1343; _LL1335:
_LL1344: _temp1343= _temp1333.f1; if( _temp1343 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1340;} else{ goto _LL1337;} _LL1340: _temp1339= _temp1333.f2; if((
unsigned int) _temp1339 >  1u?*(( int*) _temp1339) ==  Cyc_Absyn_Upper_b: 0){
_LL1342: _temp1341=(( struct Cyc_Absyn_Upper_b_struct*) _temp1339)->f1; goto
_LL1336;} else{ goto _LL1337;} _LL1337: goto _LL1338; _LL1336: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1345[ 4u]; _temp1345[ 3u]= _temp1341; _temp1345[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1330, 0); _temp1345[ 1u]= _temp1307; _temp1345[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1345, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1334; _LL1338: goto _LL1334; _LL1334:;} goto _LL1313;
_LL1317: goto _LL1313; _LL1313:;} goto _LL1302; _LL1306: goto _LL1302; _LL1302:;}
if( ! done){ struct _tuple0* _temp1346= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1347= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1346, 0), 0); struct Cyc_Absyn_Exp*
_temp1348= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp946);
_temp1347= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1346, 0), _temp1348, 0), _temp946, 0), _temp1347, 0);{
void* _temp1349= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1346,
_temp1349,( struct Cyc_Absyn_Exp*) mexp, _temp1347, 0), 0))->r));}} goto _LL1220;}}
_LL1220:;} goto _LL793; _LL831: Cyc_Toc_exp_to_c( nv, _temp950); goto _LL793;
_LL833:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1350=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1350[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1351; _temp1351.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1351.f1=( void*) Cyc_Toc_typ_to_c_array( _temp952);
_temp1351;}); _temp1350;}))); goto _LL793; _LL835:( int) _throw(({ void*
_temp1352[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("__gen() in code generator",
sizeof( unsigned char), 26u), _tag_arr( _temp1352, sizeof( void*), 0u));}));
_LL837:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1353=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1353[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1354; _temp1354.tag=
Cyc_Absyn_Offsetof_e; _temp1354.f1=( void*) Cyc_Toc_typ_to_c_array( _temp958);
_temp1354.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1355=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1355[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1356; _temp1356.tag=
Cyc_Absyn_StructField; _temp1356.f1= _temp956; _temp1356;}); _temp1355;}));
_temp1354;}); _temp1353;}))); goto _LL793; _LL839:{ void* _temp1357= Cyc_Tcutil_compress(
_temp964); struct Cyc_Absyn_Structdecl** _temp1373; struct Cyc_List_List*
_temp1375; struct Cyc_Absyn_Uniondecl** _temp1377; struct Cyc_List_List*
_temp1379; _LL1359: if(( unsigned int) _temp1357 >  4u?*(( int*) _temp1357) == 
Cyc_Absyn_StructType: 0){ _LL1374: _temp1373=(( struct Cyc_Absyn_StructType_struct*)
_temp1357)->f3; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(( unsigned int)
_temp1357 >  4u?*(( int*) _temp1357) ==  Cyc_Absyn_AnonStructType: 0){ _LL1376:
_temp1375=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1357)->f1; goto
_LL1362;} else{ goto _LL1363;} _LL1363: if(( unsigned int) _temp1357 >  4u?*((
int*) _temp1357) ==  Cyc_Absyn_UnionType: 0){ _LL1378: _temp1377=(( struct Cyc_Absyn_UnionType_struct*)
_temp1357)->f3; goto _LL1364;} else{ goto _LL1365;} _LL1365: if(( unsigned int)
_temp1357 >  4u?*(( int*) _temp1357) ==  Cyc_Absyn_AnonUnionType: 0){ _LL1380:
_temp1379=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1357)->f1; goto
_LL1366;} else{ goto _LL1367;} _LL1367: if(( unsigned int) _temp1357 >  4u?*((
int*) _temp1357) ==  Cyc_Absyn_TupleType: 0){ goto _LL1368;} else{ goto _LL1369;}
_LL1369: if(( unsigned int) _temp1357 >  4u?*(( int*) _temp1357) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL1370;} else{ goto _LL1371;} _LL1371: goto _LL1372; _LL1360: if(
_temp1373 ==  0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1373)))->fields
==  0){( int) _throw(({ void* _temp1381[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("struct fields must be known",
sizeof( unsigned char), 28u), _tag_arr( _temp1381, sizeof( void*), 0u));}));}
_temp1375=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp1373)))->fields))->v; goto
_LL1362; _LL1362: { struct Cyc_Absyn_Structfield* _temp1382=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1375,( int) _temp962);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1383=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1383[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1384; _temp1384.tag=
Cyc_Absyn_Offsetof_e; _temp1384.f1=( void*) Cyc_Toc_typ_to_c_array( _temp964);
_temp1384.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1385=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1385[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1386; _temp1386.tag=
Cyc_Absyn_StructField; _temp1386.f1= _temp1382->name; _temp1386;}); _temp1385;}));
_temp1384;}); _temp1383;}))); goto _LL1358;} _LL1364: if( _temp1377 ==  0? 1:(*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp1377)))->fields ==  0){( int)
_throw(({ void* _temp1387[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("union fields must be known",
sizeof( unsigned char), 27u), _tag_arr( _temp1387, sizeof( void*), 0u));}));}
_temp1379=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp1377)))->fields))->v; goto
_LL1366; _LL1366: { struct Cyc_Absyn_Structfield* _temp1388=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1379,( int) _temp962);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1389=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1389[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1390; _temp1390.tag=
Cyc_Absyn_Offsetof_e; _temp1390.f1=( void*) Cyc_Toc_typ_to_c_array( _temp964);
_temp1390.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1391=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1391[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1392; _temp1392.tag=
Cyc_Absyn_StructField; _temp1392.f1= _temp1388->name; _temp1392;}); _temp1391;}));
_temp1390;}); _temp1389;}))); goto _LL1358;} _LL1368:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1393=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1393[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1394; _temp1394.tag= Cyc_Absyn_Offsetof_e;
_temp1394.f1=( void*) Cyc_Toc_typ_to_c_array( _temp964); _temp1394.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1395=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1395[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1396; _temp1396.tag= Cyc_Absyn_StructField;
_temp1396.f1= Cyc_Absyn_fieldname(( int)( _temp962 +  1)); _temp1396;});
_temp1395;})); _temp1394;}); _temp1393;}))); goto _LL1358; _LL1370: if( _temp962
==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1397=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1397[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1398; _temp1398.tag=
Cyc_Absyn_Offsetof_e; _temp1398.f1=( void*) Cyc_Toc_typ_to_c_array( _temp964);
_temp1398.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1399=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1399[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1400; _temp1400.tag=
Cyc_Absyn_StructField; _temp1400.f1= Cyc_Toc_tag_sp; _temp1400;}); _temp1399;}));
_temp1398;}); _temp1397;})));} else{( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1401=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1401[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1402; _temp1402.tag= Cyc_Absyn_Offsetof_e;
_temp1402.f1=( void*) Cyc_Toc_typ_to_c_array( _temp964); _temp1402.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1403=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1403[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1404; _temp1404.tag= Cyc_Absyn_StructField;
_temp1404.f1= Cyc_Absyn_fieldname(( int) _temp962); _temp1404;}); _temp1403;}));
_temp1402;}); _temp1401;})));} goto _LL1358; _LL1372:( int) _throw(({ void*
_temp1405[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("impossible type for offsetof tuple index",
sizeof( unsigned char), 41u), _tag_arr( _temp1405, sizeof( void*), 0u));}));
_LL1358:;} goto _LL793; _LL841: { void* _temp1406= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp966->topt))->v);{ void* _temp1407=
_temp1406; struct Cyc_Absyn_PtrInfo _temp1413; struct Cyc_Absyn_Conref*
_temp1415; struct Cyc_Absyn_Tqual _temp1417; struct Cyc_Absyn_Conref* _temp1419;
void* _temp1421; void* _temp1423; _LL1409: if(( unsigned int) _temp1407 >  4u?*((
int*) _temp1407) ==  Cyc_Absyn_PointerType: 0){ _LL1414: _temp1413=(( struct Cyc_Absyn_PointerType_struct*)
_temp1407)->f1; _LL1424: _temp1423=( void*) _temp1413.elt_typ; goto _LL1422;
_LL1422: _temp1421=( void*) _temp1413.rgn_typ; goto _LL1420; _LL1420: _temp1419=
_temp1413.nullable; goto _LL1418; _LL1418: _temp1417= _temp1413.tq; goto _LL1416;
_LL1416: _temp1415= _temp1413.bounds; goto _LL1410;} else{ goto _LL1411;}
_LL1411: goto _LL1412; _LL1410:{ void* _temp1425= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1415); _LL1427: if(( unsigned int) _temp1425 >  1u?*(( int*) _temp1425) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1428;} else{ goto _LL1429;} _LL1429: if(
_temp1425 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1430;} else{ goto _LL1426;}
_LL1428: Cyc_Toc_exp_to_c( nv, _temp966); if( Cyc_Toc_is_nullable( _temp1406)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp966->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1431=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1431->hd=( void*) _temp966;
_temp1431->tl= 0; _temp1431;}), 0), 0), 0))->r));} goto _LL1426; _LL1430: {
struct Cyc_Absyn_Exp* _temp1432= Cyc_Absyn_uint_exp( 0, 0); _temp1432->topt=({
struct Cyc_Core_Opt* _temp1433=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1433->v=( void*) Cyc_Absyn_uint_t; _temp1433;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp966, _temp1432, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1426;} _LL1426:;} goto _LL1408; _LL1412:({ void* _temp1434[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1434, sizeof( void*), 0u));}); goto _LL1408; _LL1408:;} goto _LL793;}
_LL843: Cyc_Toc_exp_to_c( nv, _temp970); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL793; _LL845: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp974->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp974);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1437; struct Cyc_Absyn_Tqual _temp1439; struct Cyc_Absyn_Conref* _temp1441;
void* _temp1443; void* _temp1445; struct Cyc_Absyn_PtrInfo _temp1435= Cyc_Toc_get_ptr_type(
e1typ); _LL1446: _temp1445=( void*) _temp1435.elt_typ; goto _LL1444; _LL1444:
_temp1443=( void*) _temp1435.rgn_typ; goto _LL1442; _LL1442: _temp1441=
_temp1435.nullable; goto _LL1440; _LL1440: _temp1439= _temp1435.tq; goto _LL1438;
_LL1438: _temp1437= _temp1435.bounds; goto _LL1436; _LL1436:{ void* _temp1447=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1437); struct Cyc_Absyn_Exp*
_temp1453; _LL1449: if(( unsigned int) _temp1447 >  1u?*(( int*) _temp1447) == 
Cyc_Absyn_Upper_b: 0){ _LL1454: _temp1453=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1447)->f1; goto _LL1450;} else{ goto _LL1451;} _LL1451: if( _temp1447 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1452;} else{ goto _LL1448;} _LL1450: if(
Cyc_Evexp_eval_const_uint_exp( _temp1453) <  1){({ void* _temp1455[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1455, sizeof( void*), 0u));});}{ int possibly_null=(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1441); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp974->topt))->v)){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp974->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1456=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1456->hd=( void*) _temp974;
_temp1456->tl= 0; _temp1456;}), 0), 0), _temp972, 0))->r));} goto _LL1448;}
_LL1452: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1445);( void*)( _temp974->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1439), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1457[ 3u];
_temp1457[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1457[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1457[ 0u]= Cyc_Absyn_copy_exp( _temp974);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1457, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1448;} _LL1448:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL793;}} _LL847: { void* _temp1458= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp978->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp978); Cyc_Toc_exp_to_c( nv, _temp976);{ void* _temp1459= _temp1458; struct
Cyc_List_List* _temp1467; struct Cyc_Absyn_PtrInfo _temp1469; struct Cyc_Absyn_Conref*
_temp1471; struct Cyc_Absyn_Tqual _temp1473; struct Cyc_Absyn_Conref* _temp1475;
void* _temp1477; void* _temp1479; _LL1461: if(( unsigned int) _temp1459 >  4u?*((
int*) _temp1459) ==  Cyc_Absyn_TupleType: 0){ _LL1468: _temp1467=(( struct Cyc_Absyn_TupleType_struct*)
_temp1459)->f1; goto _LL1462;} else{ goto _LL1463;} _LL1463: if(( unsigned int)
_temp1459 >  4u?*(( int*) _temp1459) ==  Cyc_Absyn_PointerType: 0){ _LL1470:
_temp1469=(( struct Cyc_Absyn_PointerType_struct*) _temp1459)->f1; _LL1480:
_temp1479=( void*) _temp1469.elt_typ; goto _LL1478; _LL1478: _temp1477=( void*)
_temp1469.rgn_typ; goto _LL1476; _LL1476: _temp1475= _temp1469.nullable; goto
_LL1474; _LL1474: _temp1473= _temp1469.tq; goto _LL1472; _LL1472: _temp1471=
_temp1469.bounds; goto _LL1464;} else{ goto _LL1465;} _LL1465: goto _LL1466;
_LL1462: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp976) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp978, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1460;} _LL1464:{ void* _temp1481= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1471); struct Cyc_Absyn_Exp* _temp1487; _LL1483: if((
unsigned int) _temp1481 >  1u?*(( int*) _temp1481) ==  Cyc_Absyn_Upper_b: 0){
_LL1488: _temp1487=(( struct Cyc_Absyn_Upper_b_struct*) _temp1481)->f1; goto
_LL1484;} else{ goto _LL1485;} _LL1485: if( _temp1481 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1486;} else{ goto _LL1482;} _LL1484: _temp1487= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1487), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1475); void* ta1= Cyc_Toc_typ_to_c( _temp1479);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1473); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1489[ 4u];
_temp1489[ 3u]= _temp976; _temp1489[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1489[ 1u]= _temp1487; _temp1489[ 0u]= _temp978;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1489, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp976->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1490[ 2u];
_temp1490[ 1u]= Cyc_Absyn_copy_exp( _temp976); _temp1490[ 0u]= _temp1487;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1490, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1482;}
_LL1486: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1479);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1473), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1491[ 3u]; _temp1491[ 2u]= _temp976; _temp1491[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1491[ 0u]= _temp978;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1491, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1482;} _LL1482:;} goto _LL1460; _LL1466:({
void* _temp1492[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1492, sizeof( void*), 0u));}); goto
_LL1460; _LL1460:;} goto _LL793;} _LL849: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp980))->r));} else{ struct
Cyc_List_List* _temp1493=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp980); struct _tagged_arr* _temp1494= Cyc_Toc_add_tuple_type( _temp1493);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp980 !=  0;( _temp980=((
struct Cyc_List_List*) _check_null( _temp980))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp980))->hd);
dles=({ struct Cyc_List_List* _temp1495=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1495->hd=( void*)({ struct _tuple4*
_temp1496=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1496->f1=
0; _temp1496->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp980))->hd; _temp1496;}); _temp1495->tl= dles; _temp1495;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL793; _LL851:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp982, 0))->r));{ struct Cyc_List_List* _temp1497= _temp982; for( 0;
_temp1497 !=  0; _temp1497=(( struct Cyc_List_List*) _check_null( _temp1497))->tl){
struct _tuple4 _temp1500; struct Cyc_Absyn_Exp* _temp1501; struct _tuple4*
_temp1498=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1497))->hd;
_temp1500=* _temp1498; _LL1502: _temp1501= _temp1500.f2; goto _LL1499; _LL1499:
Cyc_Toc_exp_to_c( nv, _temp1501);}} goto _LL793; _LL853: { unsigned int
_temp1503= Cyc_Evexp_eval_const_uint_exp( _temp986); Cyc_Toc_exp_to_c( nv,
_temp984);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp984)){
unsigned int i= 0; for( 0; i <  _temp1503; i ++){ es=({ struct Cyc_List_List*
_temp1504=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1504->hd=( void*)({ struct _tuple4* _temp1505=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1505->f1= 0; _temp1505->f2= _temp984; _temp1505;});
_temp1504->tl= es; _temp1504;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL793;}} _LL855: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp992, _temp996))->r));}
else{ if( _temp990 ==  0){({ void* _temp1506[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1506, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp990); struct _RegionHandle _temp1507= _new_region(); struct _RegionHandle*
rgn=& _temp1507; _push_region( rgn);{ struct Cyc_List_List* _temp1508=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp992,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1509= 0;{ struct Cyc_List_List*
_temp1510=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1510 !=  0; _temp1510=(( struct Cyc_List_List*) _check_null(
_temp1510))->tl){ struct Cyc_List_List* _temp1511= _temp1508; for( 0; _temp1511
!=  0; _temp1511=(( struct Cyc_List_List*) _check_null( _temp1511))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1511))->hd)).f1 == (
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1510))->hd){
struct _tuple10 _temp1514; struct Cyc_Absyn_Exp* _temp1515; struct Cyc_Absyn_Structfield*
_temp1517; struct _tuple10* _temp1512=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1511))->hd; _temp1514=* _temp1512; _LL1518: _temp1517=
_temp1514.f1; goto _LL1516; _LL1516: _temp1515= _temp1514.f2; goto _LL1513;
_LL1513: { void* _temp1519=( void*) _temp1517->type; Cyc_Toc_exp_to_c( nv,
_temp1515); if( Cyc_Toc_is_void_star( _temp1519)){( void*)( _temp1515->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1515->r, 0), 0))->r));} _temp1509=({ struct Cyc_List_List* _temp1520=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1520->hd=(
void*)({ struct _tuple4* _temp1521=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1521->f1= 0; _temp1521->f2= _temp1515; _temp1521;}); _temp1520->tl=
_temp1509; _temp1520;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1509), 0))->r));}; _pop_region( rgn);}} goto _LL793; _LL857: { struct Cyc_List_List*
fs;{ void* _temp1522= Cyc_Tcutil_compress( _temp1000); struct Cyc_List_List*
_temp1528; _LL1524: if(( unsigned int) _temp1522 >  4u?*(( int*) _temp1522) == 
Cyc_Absyn_AnonStructType: 0){ _LL1529: _temp1528=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1522)->f1; goto _LL1525;} else{ goto _LL1526;} _LL1526: goto _LL1527;
_LL1525: fs= _temp1528; goto _LL1523; _LL1527: fs=({ struct Cyc_Std_String_pa_struct
_temp1531; _temp1531.tag= Cyc_Std_String_pa; _temp1531.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp1000);{ void* _temp1530[ 1u]={& _temp1531};((
struct Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s", sizeof( unsigned char), 24u), _tag_arr(
_temp1530, sizeof( void*), 1u));}}); goto _LL1523; _LL1523:;}{ struct
_RegionHandle _temp1532= _new_region(); struct _RegionHandle* rgn=& _temp1532;
_push_region( rgn);{ struct Cyc_List_List* _temp1533=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp998, fs); for( 0; _temp1533 !=  0; _temp1533=(( struct Cyc_List_List*)
_check_null( _temp1533))->tl){ struct _tuple10 _temp1536; struct Cyc_Absyn_Exp*
_temp1537; struct Cyc_Absyn_Structfield* _temp1539; struct _tuple10* _temp1534=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1533))->hd;
_temp1536=* _temp1534; _LL1540: _temp1539= _temp1536.f1; goto _LL1538; _LL1538:
_temp1537= _temp1536.f2; goto _LL1535; _LL1535: { void* _temp1541=( void*)
_temp1539->type; Cyc_Toc_exp_to_c( nv, _temp1537); if( Cyc_Toc_is_void_star(
_temp1541)){( void*)( _temp1537->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1537->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp998, 0))->r));}; _pop_region( rgn);}
goto _LL793;} _LL859: { struct _tuple0* qv= _temp1002->name; if( _temp1004->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1542= _temp1004->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1004->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1542))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1542))->hd)->typs ==  0){ tag_count ++;}
_temp1542=(( struct Cyc_List_List*) _check_null( _temp1542))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL793;} _LL861: { struct _tuple0* _temp1543= _temp1008->name; struct Cyc_List_List*
_temp1544= _temp1008->typs; struct _tuple0* _temp1545= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1546= Cyc_Absyn_var_exp( _temp1545, 0); void*
_temp1547= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1543, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1010->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1543, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1548= _temp1010->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1010->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1543,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1548))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1548))->hd)->typs !=  0){ tag_count ++;}
_temp1548=(( struct Cyc_List_List*) _check_null( _temp1548))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1012
!=  0;( _temp1012=(( struct Cyc_List_List*) _check_null( _temp1012))->tl,
_temp1544=(( struct Cyc_List_List*) _check_null( _temp1544))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1012))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1544))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1549=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1549->hd=( void*)({ struct _tuple4* _temp1550=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1550->f1= 0; _temp1550->f2= cur_e; _temp1550;});
_temp1549->tl= dles; _temp1549;});} dles=({ struct Cyc_List_List* _temp1551=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1551->hd=(
void*)({ struct _tuple4* _temp1552=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1552->f1= 0; _temp1552->f2= tag_exp; _temp1552;}); _temp1551->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1551;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1553=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1553->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1543,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1553;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1554=({ struct Cyc_List_List* _temp1560=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1560->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1546, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1543, 0), 0); _temp1560->tl= 0; _temp1560;});{ int i=
1; for( 0; _temp1012 !=  0;((( _temp1012=(( struct Cyc_List_List*) _check_null(
_temp1012))->tl, i ++)), _temp1544=(( struct Cyc_List_List*) _check_null(
_temp1544))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1012))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1544))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp1555= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1546, Cyc_Absyn_fieldname( i), 0), cur_e, 0);
_temp1554=({ struct Cyc_List_List* _temp1556=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1556->hd=( void*) _temp1555; _temp1556->tl=
_temp1554; _temp1556;});}}}{ struct Cyc_Absyn_Stmt* _temp1557= Cyc_Absyn_exp_stmt(
_temp1546, 0); struct Cyc_Absyn_Stmt* _temp1558= Cyc_Absyn_seq_stmts((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List*
_temp1559=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1559->hd=( void*) _temp1557; _temp1559->tl= _temp1554; _temp1559;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1545, _temp1547, 0, _temp1558, 0), 0))->r));}} goto _LL793;} _LL863: goto
_LL793; _LL865: { void* t_c= Cyc_Toc_typ_to_c( _temp1014); if( _temp1016 !=  0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1016); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1014, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL793;} _LL867: Cyc_Toc_stmt_to_c(
nv, _temp1018); goto _LL793; _LL869:({ void* _temp1561[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem",
sizeof( unsigned char), 14u), _tag_arr( _temp1561, sizeof( void*), 0u));}); goto
_LL793; _LL871:({ void* _temp1562[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u), _tag_arr( _temp1562, sizeof( void*), 0u));}); goto _LL793;
_LL873:({ void* _temp1563[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u),
_tag_arr( _temp1563, sizeof( void*), 0u));}); goto _LL793; _LL875:({ void*
_temp1564[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("fill", sizeof( unsigned char), 5u), _tag_arr( _temp1564, sizeof( void*),
0u));}); goto _LL793; _LL793:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1565=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1601; struct Cyc_Absyn_Vardecl _temp1603; struct _tuple0* _temp1604; struct
Cyc_Absyn_Vardecl* _temp1606; int _temp1608; void* _temp1610; unsigned char
_temp1612; struct _tagged_arr _temp1614; struct Cyc_Absyn_Enumfield* _temp1616;
struct Cyc_Absyn_Enumdecl* _temp1618; struct Cyc_List_List* _temp1620; struct
Cyc_List_List* _temp1622; struct Cyc_Absyn_Tunionfield* _temp1624; struct Cyc_Absyn_Tuniondecl*
_temp1626; struct Cyc_Absyn_Pat* _temp1629; struct Cyc_Absyn_Pat _temp1631; void*
_temp1632; struct Cyc_List_List* _temp1634; struct Cyc_List_List* _temp1636;
struct Cyc_Absyn_Tunionfield* _temp1638; struct Cyc_Absyn_Tuniondecl* _temp1640;
struct Cyc_List_List* _temp1642; struct Cyc_List_List* _temp1644; struct Cyc_List_List*
_temp1646; struct Cyc_List_List* _temp1648; struct Cyc_Core_Opt* _temp1650;
struct Cyc_Absyn_Structdecl* _temp1652; struct Cyc_Absyn_Pat* _temp1654; _LL1567:
if(( unsigned int) _temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_Var_p: 0){
_LL1602: _temp1601=(( struct Cyc_Absyn_Var_p_struct*) _temp1565)->f1; _temp1603=*
_temp1601; _LL1605: _temp1604= _temp1603.name; goto _LL1568;} else{ goto _LL1569;}
_LL1569: if( _temp1565 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1570;} else{ goto
_LL1571;} _LL1571: if(( unsigned int) _temp1565 >  2u?*(( int*) _temp1565) == 
Cyc_Absyn_Reference_p: 0){ _LL1607: _temp1606=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1565)->f1; goto _LL1572;} else{ goto _LL1573;} _LL1573: if( _temp1565 == (
void*) Cyc_Absyn_Null_p){ goto _LL1574;} else{ goto _LL1575;} _LL1575: if((
unsigned int) _temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_Int_p: 0){
_LL1611: _temp1610=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1565)->f1;
goto _LL1609; _LL1609: _temp1608=(( struct Cyc_Absyn_Int_p_struct*) _temp1565)->f2;
goto _LL1576;} else{ goto _LL1577;} _LL1577: if(( unsigned int) _temp1565 >  2u?*((
int*) _temp1565) ==  Cyc_Absyn_Char_p: 0){ _LL1613: _temp1612=(( struct Cyc_Absyn_Char_p_struct*)
_temp1565)->f1; goto _LL1578;} else{ goto _LL1579;} _LL1579: if(( unsigned int)
_temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_Float_p: 0){ _LL1615:
_temp1614=(( struct Cyc_Absyn_Float_p_struct*) _temp1565)->f1; goto _LL1580;}
else{ goto _LL1581;} _LL1581: if(( unsigned int) _temp1565 >  2u?*(( int*)
_temp1565) ==  Cyc_Absyn_Enum_p: 0){ _LL1619: _temp1618=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1565)->f1; goto _LL1617; _LL1617: _temp1616=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1565)->f2; goto _LL1582;} else{ goto _LL1583;} _LL1583: if(( unsigned int)
_temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_Tunion_p: 0){ _LL1627:
_temp1626=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1565)->f1; goto _LL1625;
_LL1625: _temp1624=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1565)->f2; goto
_LL1623; _LL1623: _temp1622=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1565)->f3;
goto _LL1621; _LL1621: _temp1620=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1565)->f4;
if( _temp1620 ==  0){ goto _LL1584;} else{ goto _LL1585;}} else{ goto _LL1585;}
_LL1585: if(( unsigned int) _temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_Pointer_p:
0){ _LL1630: _temp1629=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1565)->f1;
_temp1631=* _temp1629; _LL1633: _temp1632=( void*) _temp1631.r; if((
unsigned int) _temp1632 >  2u?*(( int*) _temp1632) ==  Cyc_Absyn_Tunion_p: 0){
_LL1641: _temp1640=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1632)->f1; goto
_LL1639; _LL1639: _temp1638=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1632)->f2;
goto _LL1637; _LL1637: _temp1636=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1632)->f3;
goto _LL1635; _LL1635: _temp1634=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1632)->f4;
goto _LL1628;} else{ goto _LL1587;}} else{ goto _LL1587;} _LL1628: if( _temp1634
!=  0){ goto _LL1586;} else{ goto _LL1587;} _LL1587: if(( unsigned int)
_temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_Tunion_p: 0){ _LL1643:
_temp1642=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1565)->f4; goto _LL1588;}
else{ goto _LL1589;} _LL1589: if(( unsigned int) _temp1565 >  2u?*(( int*)
_temp1565) ==  Cyc_Absyn_Tuple_p: 0){ _LL1645: _temp1644=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1565)->f1; goto _LL1590;} else{ goto _LL1591;} _LL1591: if(( unsigned int)
_temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_Struct_p: 0){ _LL1653:
_temp1652=(( struct Cyc_Absyn_Struct_p_struct*) _temp1565)->f1; goto _LL1651;
_LL1651: _temp1650=(( struct Cyc_Absyn_Struct_p_struct*) _temp1565)->f2; goto
_LL1649; _LL1649: _temp1648=(( struct Cyc_Absyn_Struct_p_struct*) _temp1565)->f3;
goto _LL1647; _LL1647: _temp1646=(( struct Cyc_Absyn_Struct_p_struct*) _temp1565)->f4;
goto _LL1592;} else{ goto _LL1593;} _LL1593: if(( unsigned int) _temp1565 >  2u?*((
int*) _temp1565) ==  Cyc_Absyn_Pointer_p: 0){ _LL1655: _temp1654=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1565)->f1; goto _LL1594;} else{ goto _LL1595;} _LL1595: if(( unsigned int)
_temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1596;}
else{ goto _LL1597;} _LL1597: if(( unsigned int) _temp1565 >  2u?*(( int*)
_temp1565) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1598;} else{ goto _LL1599;}
_LL1599: if(( unsigned int) _temp1565 >  2u?*(( int*) _temp1565) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1600;} else{ goto _LL1566;} _LL1568: nv= Cyc_Toc_add_varmap( nv,
_temp1604, r); goto _LL1570; _LL1570: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1566; _LL1572: { struct _tuple0* _temp1656= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1657=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1657->hd=( void*)({ struct _tuple12*
_temp1658=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1658->f1= _temp1656; _temp1658->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1658;}); _temp1657->tl= decls; _temp1657;}); nv= Cyc_Toc_add_varmap(
nv, _temp1606->name, Cyc_Absyn_var_exp( _temp1656, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1656, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1566;} _LL1574: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1566; _LL1576:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1610, _temp1608, 0),
succ_lab, fail_lab); goto _LL1566; _LL1578: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1612, 0), succ_lab, fail_lab); goto _LL1566; _LL1580: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1614, 0), succ_lab, fail_lab); goto _LL1566;
_LL1582: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1659=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1659[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1660; _temp1660.tag= Cyc_Absyn_Enum_e;
_temp1660.f1= _temp1616->name; _temp1660.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1618; _temp1660.f3=( struct Cyc_Absyn_Enumfield*) _temp1616; _temp1660;});
_temp1659;}), 0), succ_lab, fail_lab); goto _LL1566; _LL1584: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1624->name, 0); if( ! _temp1626->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1566;} _LL1586: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1634);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1638->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1661=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1638->typs); struct Cyc_List_List*
_temp1662=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1634); for( 0; _temp1662 !=  0;((( _temp1662=((
struct Cyc_List_List*) _check_null( _temp1662))->tl, _temp1661=(( struct Cyc_List_List*)
_check_null( _temp1661))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1663=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1662))->hd;
if(( void*) _temp1663->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1664=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1661))->hd)).f2;
struct _tuple0* _temp1665= Cyc_Toc_temp_var(); void* _temp1666=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1663->topt))->v; void* _temp1667= Cyc_Toc_typ_to_c(
_temp1666); struct _tagged_arr* _temp1668= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1669= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1664))){ _temp1669= Cyc_Absyn_cast_exp(
_temp1667, _temp1669, 0);} decls=({ struct Cyc_List_List* _temp1670=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1670->hd=( void*)({
struct _tuple12* _temp1671=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1671->f1= _temp1665; _temp1671->f2= _temp1667; _temp1671;});
_temp1670->tl= decls; _temp1670;});{ struct _tuple11 _temp1672= Cyc_Toc_xlate_pat(
nv, rgn, _temp1666, Cyc_Absyn_var_exp( _temp1665, 0), _temp1669, _temp1663,
succ_lab, fail_lab, decls); nv= _temp1672.f1; decls= _temp1672.f2;{ struct Cyc_Absyn_Stmt*
_temp1673= _temp1672.f3; struct Cyc_Absyn_Stmt* _temp1674= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1665, 0), _temp1669, 0); struct Cyc_Absyn_Stmt*
_temp1675= Cyc_Absyn_seq_stmt( _temp1674, _temp1673, 0); ss=({ struct Cyc_List_List*
_temp1676=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1676->hd=( void*) Cyc_Absyn_label_stmt( _temp1668, _temp1675, 0); _temp1676->tl=
ss; _temp1676;}); succ_lab= _temp1668;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1677=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1677->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1677->tl= 0;
_temp1677;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1640->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1638->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1678=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1640->fields))->v; for( 0; _temp1678 !=  0; _temp1678=((
struct Cyc_List_List*) _check_null( _temp1678))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1679=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1678))->hd; if( _temp1679->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1638->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1566;}} _LL1588: _temp1644= _temp1642; goto _LL1590; _LL1590: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1644);{ struct Cyc_List_List* _temp1680=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1644); for(
0; _temp1680 !=  0;( _temp1680=(( struct Cyc_List_List*) _check_null( _temp1680))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1681=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1680))->hd; if(( void*) _temp1681->r == ( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1682= Cyc_Toc_temp_var(); void* _temp1683=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1681->topt))->v; struct
_tagged_arr* _temp1684= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1685=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1685->hd=( void*)({ struct _tuple12* _temp1686=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1686->f1= _temp1682;
_temp1686->f2= Cyc_Toc_typ_to_c( _temp1683); _temp1686;}); _temp1685->tl= decls;
_temp1685;});{ struct _tuple11 _temp1687= Cyc_Toc_xlate_pat( nv, rgn, _temp1683,
Cyc_Absyn_var_exp( _temp1682, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1681, succ_lab, fail_lab, decls); nv= _temp1687.f1; decls=
_temp1687.f2;{ struct Cyc_Absyn_Stmt* _temp1688= _temp1687.f3; struct Cyc_Absyn_Stmt*
_temp1689= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1682, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp1688, 0); ss=({ struct Cyc_List_List* _temp1690=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1690->hd=( void*) Cyc_Absyn_label_stmt(
_temp1684, _temp1689, 0); _temp1690->tl= ss; _temp1690;}); succ_lab= _temp1684;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1691=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1691->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1691->tl= 0; _temp1691;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1566;} _LL1592: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1692=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1646); for( 0; _temp1692 !=  0; _temp1692=((
struct Cyc_List_List*) _check_null( _temp1692))->tl){ struct _tuple13* _temp1693=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1692))->hd; struct
Cyc_Absyn_Pat* _temp1694=(* _temp1693).f2; if(( void*) _temp1694->r == ( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1695=( void*)((
struct Cyc_List_List*) _check_null((* _temp1693).f1))->hd; struct _tagged_arr*
_temp1701; _LL1697: if(*(( int*) _temp1695) ==  Cyc_Absyn_FieldName){ _LL1702:
_temp1701=(( struct Cyc_Absyn_FieldName_struct*) _temp1695)->f1; goto _LL1698;}
else{ goto _LL1699;} _LL1699: goto _LL1700; _LL1698: f= _temp1701; goto _LL1696;
_LL1700:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1696:;}{ struct _tuple0*
_temp1703= Cyc_Toc_temp_var(); void* _temp1704=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1694->topt))->v; void* _temp1705= Cyc_Toc_typ_to_c( _temp1704);
struct _tagged_arr* _temp1706= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1707=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1707->hd=( void*)({ struct _tuple12* _temp1708=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1708->f1= _temp1703;
_temp1708->f2= _temp1705; _temp1708;}); _temp1707->tl= decls; _temp1707;});{
struct _tuple11 _temp1709= Cyc_Toc_xlate_pat( nv, rgn, _temp1704, Cyc_Absyn_var_exp(
_temp1703, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1694, succ_lab,
fail_lab, decls); nv= _temp1709.f1; decls= _temp1709.f2;{ struct Cyc_Absyn_Exp*
_temp1710= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1652->fields))->v, f)))->type)){
_temp1710= Cyc_Absyn_cast_exp( _temp1705, _temp1710, 0);}{ struct Cyc_Absyn_Stmt*
_temp1711= _temp1709.f3; struct Cyc_Absyn_Stmt* _temp1712= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1703, 0), _temp1710, 0),
_temp1711, 0); ss=({ struct Cyc_List_List* _temp1713=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1713->hd=( void*) Cyc_Absyn_label_stmt(
_temp1706, _temp1712, 0); _temp1713->tl= ss; _temp1713;}); succ_lab= _temp1706;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1714=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1714->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1714->tl= 0; _temp1714;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1566;} _LL1594: { struct _tuple0* _temp1715= Cyc_Toc_temp_var(); void*
_temp1716=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1654->topt))->v;
decls=({ struct Cyc_List_List* _temp1717=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1717->hd=( void*)({ struct _tuple12*
_temp1718=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1718->f1= _temp1715; _temp1718->f2= Cyc_Toc_typ_to_c( _temp1716); _temp1718;});
_temp1717->tl= decls; _temp1717;});{ struct _tuple11 _temp1719= Cyc_Toc_xlate_pat(
nv, rgn, _temp1716, Cyc_Absyn_var_exp( _temp1715, 0), Cyc_Absyn_deref_exp( path,
0), _temp1654, succ_lab, fail_lab, decls); nv= _temp1719.f1; decls= _temp1719.f2;{
struct Cyc_Absyn_Stmt* _temp1720= _temp1719.f3; struct Cyc_Absyn_Stmt* _temp1721=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1715, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1720, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1721, 0);} else{ s= _temp1721;} goto _LL1566;}}} _LL1596: s=({
void* _temp1722[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1722, sizeof( void*), 0u));}); goto _LL1566;
_LL1598: s=({ void* _temp1723[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1723, sizeof( void*), 0u));}); goto
_LL1566; _LL1600: s=({ void* _temp1724[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1724, sizeof( void*), 0u));}); goto
_LL1566; _LL1566:;} return({ struct _tuple11 _temp1725; _temp1725.f1= nv;
_temp1725.f2= decls; _temp1725.f3= s; _temp1725;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1726=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1726->f1= Cyc_Toc_fresh_label(); _temp1726->f2=
Cyc_Toc_fresh_label(); _temp1726->f3= sc; _temp1726;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1727=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1728= Cyc_Tcutil_compress( _temp1727); _LL1730: if(( unsigned int)
_temp1728 >  4u?*(( int*) _temp1728) ==  Cyc_Absyn_IntType: 0){ goto _LL1731;}
else{ goto _LL1732;} _LL1732: if(( unsigned int) _temp1728 >  4u?*(( int*)
_temp1728) ==  Cyc_Absyn_EnumType: 0){ goto _LL1733;} else{ goto _LL1734;}
_LL1734: goto _LL1735; _LL1731: goto _LL1733; _LL1733: leave_as_switch= 1; goto
_LL1729; _LL1735: leave_as_switch= 0; goto _LL1729; _LL1729:;}{ struct Cyc_List_List*
_temp1736= scs; for( 0; _temp1736 !=  0; _temp1736=(( struct Cyc_List_List*)
_check_null( _temp1736))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1736))->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1736))->hd)->where_clause !=  0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1737= scs; for( 0; _temp1737
!=  0; _temp1737=(( struct Cyc_List_List*) _check_null( _temp1737))->tl){ struct
Cyc_Absyn_Stmt* _temp1738=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1737))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1737))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1738, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1738);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1739= _new_region(); struct _RegionHandle* rgn=&
_temp1739; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1740= lscs; for( 0;
_temp1740 !=  0; _temp1740=(( struct Cyc_List_List*) _check_null( _temp1740))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1740))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1740))->tl ==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1740))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1740))->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11
_temp1741= Cyc_Toc_xlate_pat( nv, rgn, _temp1727, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1742=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1742->hd=( void*)
_temp1741.f1; _temp1742->tl= nvs; _temp1742;}); decls= _temp1741.f2; test_stmts=({
struct Cyc_List_List* _temp1743=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1743->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1740))->hd)).f1,
_temp1741.f3, 0); _temp1743->tl= test_stmts; _temp1743;});} else{ struct Cyc_Absyn_Exp*
_temp1744=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1745= Cyc_Toc_fresh_label(); struct _tuple11 _temp1746= Cyc_Toc_xlate_pat(
nv, rgn, _temp1727, r, path, sc->pattern, _temp1745, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1746.f1, _temp1744); nvs=({ struct Cyc_List_List* _temp1747=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1747->hd=( void*)
_temp1746.f1; _temp1747->tl= nvs; _temp1747;}); decls= _temp1746.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1744, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1745, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1748=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1748->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1740))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1746.f3, s3, 0), 0); _temp1748->tl= test_stmts;
_temp1748;});}}}}{ struct Cyc_Absyn_Stmt* _temp1749= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs !=  0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=  0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1750=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1750, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1751=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1751, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1749, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls !=  0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1754; void* _temp1755; struct
_tuple0* _temp1757; struct _tuple12* _temp1752=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1754=* _temp1752; _LL1758: _temp1757= _temp1754.f1;
goto _LL1756; _LL1756: _temp1755= _temp1754.f2; goto _LL1753; _LL1753: res= Cyc_Absyn_declare_stmt(
_temp1757, _temp1755, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1759=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1759[ 0]=({ struct Cyc_List_List
_temp1760; _temp1760.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1760.tl= 0; _temp1760;}); _temp1759;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1761=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1803; struct Cyc_Absyn_Stmt* _temp1805; struct Cyc_Absyn_Stmt* _temp1807;
struct Cyc_Absyn_Exp* _temp1809; struct Cyc_Absyn_Stmt* _temp1811; struct Cyc_Absyn_Stmt*
_temp1813; struct Cyc_Absyn_Exp* _temp1815; struct Cyc_Absyn_Stmt* _temp1817;
struct _tuple2 _temp1819; struct Cyc_Absyn_Exp* _temp1821; struct Cyc_Absyn_Stmt*
_temp1823; struct Cyc_Absyn_Stmt* _temp1825; struct Cyc_Absyn_Stmt* _temp1827;
struct Cyc_Absyn_Stmt* _temp1829; struct _tuple2 _temp1831; struct Cyc_Absyn_Exp*
_temp1833; struct _tuple2 _temp1835; struct Cyc_Absyn_Exp* _temp1837; struct Cyc_Absyn_Exp*
_temp1839; struct Cyc_List_List* _temp1841; struct Cyc_Absyn_Exp* _temp1843;
struct Cyc_Absyn_Switch_clause** _temp1845; struct Cyc_List_List* _temp1847;
struct Cyc_Absyn_Stmt* _temp1849; struct Cyc_Absyn_Decl* _temp1851; struct Cyc_Absyn_Stmt*
_temp1853; struct _tagged_arr* _temp1855; struct _tuple2 _temp1857; struct Cyc_Absyn_Exp*
_temp1859; struct Cyc_Absyn_Stmt* _temp1861; struct Cyc_List_List* _temp1863;
struct Cyc_Absyn_Stmt* _temp1865; struct Cyc_Absyn_Stmt* _temp1867; struct Cyc_Absyn_Vardecl*
_temp1869; struct Cyc_Absyn_Tvar* _temp1871; _LL1763: if( _temp1761 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1764;} else{ goto _LL1765;} _LL1765: if((
unsigned int) _temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Exp_s: 0){
_LL1804: _temp1803=(( struct Cyc_Absyn_Exp_s_struct*) _temp1761)->f1; goto
_LL1766;} else{ goto _LL1767;} _LL1767: if(( unsigned int) _temp1761 >  1u?*((
int*) _temp1761) ==  Cyc_Absyn_Seq_s: 0){ _LL1808: _temp1807=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1761)->f1; goto _LL1806; _LL1806: _temp1805=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1761)->f2; goto _LL1768;} else{ goto _LL1769;} _LL1769: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Return_s: 0){ _LL1810:
_temp1809=(( struct Cyc_Absyn_Return_s_struct*) _temp1761)->f1; goto _LL1770;}
else{ goto _LL1771;} _LL1771: if(( unsigned int) _temp1761 >  1u?*(( int*)
_temp1761) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1816: _temp1815=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1761)->f1; goto _LL1814; _LL1814: _temp1813=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1761)->f2; goto _LL1812; _LL1812: _temp1811=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1761)->f3; goto _LL1772;} else{ goto _LL1773;} _LL1773: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_While_s: 0){ _LL1820:
_temp1819=(( struct Cyc_Absyn_While_s_struct*) _temp1761)->f1; _LL1822:
_temp1821= _temp1819.f1; goto _LL1818; _LL1818: _temp1817=(( struct Cyc_Absyn_While_s_struct*)
_temp1761)->f2; goto _LL1774;} else{ goto _LL1775;} _LL1775: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Break_s: 0){ _LL1824:
_temp1823=(( struct Cyc_Absyn_Break_s_struct*) _temp1761)->f1; goto _LL1776;}
else{ goto _LL1777;} _LL1777: if(( unsigned int) _temp1761 >  1u?*(( int*)
_temp1761) ==  Cyc_Absyn_Continue_s: 0){ _LL1826: _temp1825=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1761)->f1; goto _LL1778;} else{ goto _LL1779;} _LL1779: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Goto_s: 0){ _LL1828:
_temp1827=(( struct Cyc_Absyn_Goto_s_struct*) _temp1761)->f2; goto _LL1780;}
else{ goto _LL1781;} _LL1781: if(( unsigned int) _temp1761 >  1u?*(( int*)
_temp1761) ==  Cyc_Absyn_For_s: 0){ _LL1840: _temp1839=(( struct Cyc_Absyn_For_s_struct*)
_temp1761)->f1; goto _LL1836; _LL1836: _temp1835=(( struct Cyc_Absyn_For_s_struct*)
_temp1761)->f2; _LL1838: _temp1837= _temp1835.f1; goto _LL1832; _LL1832:
_temp1831=(( struct Cyc_Absyn_For_s_struct*) _temp1761)->f3; _LL1834: _temp1833=
_temp1831.f1; goto _LL1830; _LL1830: _temp1829=(( struct Cyc_Absyn_For_s_struct*)
_temp1761)->f4; goto _LL1782;} else{ goto _LL1783;} _LL1783: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Switch_s: 0){ _LL1844:
_temp1843=(( struct Cyc_Absyn_Switch_s_struct*) _temp1761)->f1; goto _LL1842;
_LL1842: _temp1841=(( struct Cyc_Absyn_Switch_s_struct*) _temp1761)->f2; goto
_LL1784;} else{ goto _LL1785;} _LL1785: if(( unsigned int) _temp1761 >  1u?*((
int*) _temp1761) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1848: _temp1847=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1761)->f1; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1761)->f2; goto _LL1786;} else{ goto _LL1787;} _LL1787: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Decl_s: 0){ _LL1852:
_temp1851=(( struct Cyc_Absyn_Decl_s_struct*) _temp1761)->f1; goto _LL1850;
_LL1850: _temp1849=(( struct Cyc_Absyn_Decl_s_struct*) _temp1761)->f2; goto
_LL1788;} else{ goto _LL1789;} _LL1789: if(( unsigned int) _temp1761 >  1u?*((
int*) _temp1761) ==  Cyc_Absyn_Label_s: 0){ _LL1856: _temp1855=(( struct Cyc_Absyn_Label_s_struct*)
_temp1761)->f1; goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_Label_s_struct*)
_temp1761)->f2; goto _LL1790;} else{ goto _LL1791;} _LL1791: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Do_s: 0){ _LL1862: _temp1861=((
struct Cyc_Absyn_Do_s_struct*) _temp1761)->f1; goto _LL1858; _LL1858: _temp1857=((
struct Cyc_Absyn_Do_s_struct*) _temp1761)->f2; _LL1860: _temp1859= _temp1857.f1;
goto _LL1792;} else{ goto _LL1793;} _LL1793: if(( unsigned int) _temp1761 >  1u?*((
int*) _temp1761) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1866: _temp1865=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1761)->f1; goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1761)->f2; goto _LL1794;} else{ goto _LL1795;} _LL1795: if(( unsigned int)
_temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Region_s: 0){ _LL1872:
_temp1871=(( struct Cyc_Absyn_Region_s_struct*) _temp1761)->f1; goto _LL1870;
_LL1870: _temp1869=(( struct Cyc_Absyn_Region_s_struct*) _temp1761)->f2; goto
_LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_Region_s_struct*) _temp1761)->f3;
goto _LL1796;} else{ goto _LL1797;} _LL1797: if(( unsigned int) _temp1761 >  1u?*((
int*) _temp1761) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1798;} else{ goto _LL1799;}
_LL1799: if(( unsigned int) _temp1761 >  1u?*(( int*) _temp1761) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1800;} else{ goto _LL1801;} _LL1801: if(( unsigned int) _temp1761 > 
1u?*(( int*) _temp1761) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1802;} else{ goto
_LL1762;} _LL1764: return; _LL1766: Cyc_Toc_exp_to_c( nv, _temp1803); return;
_LL1768: Cyc_Toc_stmt_to_c( nv, _temp1807); s= _temp1805; continue; _LL1770: {
struct Cyc_Core_Opt* topt= 0; if( _temp1809 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1873=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1873->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1809))->topt))->v);
_temp1873;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1809));} if( s->try_depth >  0){ if( topt !=  0){ struct _tuple0* _temp1874=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1875= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1874, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1874,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1809, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1875, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1772: Cyc_Toc_exp_to_c( nv, _temp1815); Cyc_Toc_stmt_to_c( nv,
_temp1813); s= _temp1811; continue; _LL1774: Cyc_Toc_exp_to_c( nv, _temp1821);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1817); return; _LL1776: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1823 ==  0? 0:(( struct Cyc_Absyn_Stmt*) _check_null(
_temp1823))->try_depth; Cyc_Toc_do_npop_before( s->try_depth -  dest_depth, s);
return;} _LL1778: if( nv->continue_lab !=  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
nv->continue_lab))->v, 0))->r));} _temp1827= _temp1825; goto _LL1780; _LL1780:
Cyc_Toc_do_npop_before( s->try_depth - (( struct Cyc_Absyn_Stmt*) _check_null(
_temp1827))->try_depth, s); return; _LL1782: Cyc_Toc_exp_to_c( nv, _temp1839);
Cyc_Toc_exp_to_c( nv, _temp1837); Cyc_Toc_exp_to_c( nv, _temp1833); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1829); return; _LL1784: Cyc_Toc_xlate_switch( nv, s,
_temp1843, _temp1841); return; _LL1786: if( nv->fallthru_info ==  0){( int)
_throw(({ void* _temp1876[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u), _tag_arr( _temp1876, sizeof( void*), 0u));}));}{
struct _tuple6 _temp1879; struct Cyc_Dict_Dict* _temp1880; struct Cyc_List_List*
_temp1882; struct _tagged_arr* _temp1884; struct _tuple6* _temp1877=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp1879=*
_temp1877; _LL1885: _temp1884= _temp1879.f1; goto _LL1883; _LL1883: _temp1882=
_temp1879.f2; goto _LL1881; _LL1881: _temp1880= _temp1879.f3; goto _LL1878;
_LL1878: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1884, 0); Cyc_Toc_do_npop_before(
s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp1845)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp1886=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1882); struct Cyc_List_List* _temp1887=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1847); for( 0; _temp1886 !=  0;(
_temp1886=(( struct Cyc_List_List*) _check_null( _temp1886))->tl, _temp1887=((
struct Cyc_List_List*) _check_null( _temp1887))->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1887))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp1880,(
struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp1886))->hd),( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1887))->hd, 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1788:{ void* _temp1888=(
void*) _temp1851->r; struct Cyc_Absyn_Vardecl* _temp1898; int _temp1900; struct
Cyc_Absyn_Exp* _temp1902; struct Cyc_Core_Opt* _temp1904; struct Cyc_Absyn_Pat*
_temp1906; struct Cyc_List_List* _temp1908; _LL1890: if(*(( int*) _temp1888) == 
Cyc_Absyn_Var_d){ _LL1899: _temp1898=(( struct Cyc_Absyn_Var_d_struct*)
_temp1888)->f1; goto _LL1891;} else{ goto _LL1892;} _LL1892: if(*(( int*)
_temp1888) ==  Cyc_Absyn_Let_d){ _LL1907: _temp1906=(( struct Cyc_Absyn_Let_d_struct*)
_temp1888)->f1; goto _LL1905; _LL1905: _temp1904=(( struct Cyc_Absyn_Let_d_struct*)
_temp1888)->f3; goto _LL1903; _LL1903: _temp1902=(( struct Cyc_Absyn_Let_d_struct*)
_temp1888)->f4; goto _LL1901; _LL1901: _temp1900=(( struct Cyc_Absyn_Let_d_struct*)
_temp1888)->f5; goto _LL1893;} else{ goto _LL1894;} _LL1894: if(*(( int*)
_temp1888) ==  Cyc_Absyn_Letv_d){ _LL1909: _temp1908=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1888)->f1; goto _LL1895;} else{ goto _LL1896;} _LL1896: goto _LL1897;
_LL1891: { struct Cyc_Toc_Env* _temp1910= Cyc_Toc_add_varmap( nv, _temp1898->name,
Cyc_Absyn_varb_exp( _temp1898->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1911=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1911[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1912; _temp1912.tag= Cyc_Absyn_Local_b;
_temp1912.f1= _temp1898; _temp1912;}); _temp1911;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1910, _temp1910, _temp1898, _temp1849); goto _LL1889;} _LL1893:{ void*
_temp1913=( void*) _temp1906->r; struct Cyc_Absyn_Vardecl* _temp1919; _LL1915:
if(( unsigned int) _temp1913 >  2u?*(( int*) _temp1913) ==  Cyc_Absyn_Var_p: 0){
_LL1920: _temp1919=(( struct Cyc_Absyn_Var_p_struct*) _temp1913)->f1; goto
_LL1916;} else{ goto _LL1917;} _LL1917: goto _LL1918; _LL1916: { struct _tuple0*
old_name= _temp1919->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1919->name= new_name; _temp1919->initializer=( struct Cyc_Absyn_Exp*)
_temp1902;( void*)( _temp1851->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1921=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1921[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1922; _temp1922.tag= Cyc_Absyn_Var_d;
_temp1922.f1= _temp1919; _temp1922;}); _temp1921;})));{ struct Cyc_Toc_Env*
_temp1923= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1924=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1924[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1925; _temp1925.tag= Cyc_Absyn_Local_b; _temp1925.f1=
_temp1919; _temp1925;}); _temp1924;}), 0)); Cyc_Toc_local_decl_to_c( _temp1923,
nv, _temp1919, _temp1849); goto _LL1914;}} _LL1918:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1906, _temp1904, _temp1902, _temp1900,
_temp1849))->r)); goto _LL1914; _LL1914:;} goto _LL1889; _LL1895: { struct Cyc_List_List*
_temp1926=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1908); if( _temp1926 ==  0){({ void* _temp1927[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1927, sizeof( void*), 0u));});}(
void*)( _temp1851->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1928=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1928[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1929; _temp1929.tag= Cyc_Absyn_Var_d;
_temp1929.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1926))->hd; _temp1929;}); _temp1928;}))); _temp1926=(( struct Cyc_List_List*)
_check_null( _temp1926))->tl; for( 0; _temp1926 !=  0; _temp1926=(( struct Cyc_List_List*)
_check_null( _temp1926))->tl){ struct Cyc_Absyn_Decl* _temp1930= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1931=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1931[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1932; _temp1932.tag= Cyc_Absyn_Var_d; _temp1932.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1926))->hd; _temp1932;}); _temp1931;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1930, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1889;} _LL1897:({
void* _temp1933[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1933, sizeof( void*), 0u));}); goto _LL1889;
_LL1889:;} return; _LL1790: s= _temp1853; continue; _LL1792: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1861); Cyc_Toc_exp_to_c( nv, _temp1859); return;
_LL1794: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1934=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1934->v=( void*) e_typ; _temp1934;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1865);{
struct Cyc_Absyn_Stmt* _temp1935= Cyc_Absyn_seq_stmt( _temp1865, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1936= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1937= Cyc_Absyn_var_exp(
_temp1936, 0); struct Cyc_Absyn_Vardecl* _temp1938= Cyc_Absyn_new_vardecl(
_temp1936, Cyc_Absyn_exn_typ, 0); _temp1937->topt=({ struct Cyc_Core_Opt*
_temp1939=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1939->v=( void*) Cyc_Absyn_exn_typ; _temp1939;});{ struct Cyc_Absyn_Pat*
_temp1940=({ struct Cyc_Absyn_Pat* _temp1956=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1956->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1958=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1958[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1959; _temp1959.tag= Cyc_Absyn_Var_p;
_temp1959.f1= _temp1938; _temp1959;}); _temp1958;})); _temp1956->topt=({ struct
Cyc_Core_Opt* _temp1957=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1957->v=( void*) Cyc_Absyn_exn_typ; _temp1957;}); _temp1956->loc= 0;
_temp1956;}); struct Cyc_Absyn_Exp* _temp1941= Cyc_Absyn_throw_exp( _temp1937, 0);
_temp1941->topt=({ struct Cyc_Core_Opt* _temp1942=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1942->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1942;});{ struct Cyc_Absyn_Stmt* _temp1943= Cyc_Absyn_exp_stmt( _temp1941,
0); struct Cyc_Absyn_Switch_clause* _temp1944=({ struct Cyc_Absyn_Switch_clause*
_temp1953=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1953->pattern= _temp1940; _temp1953->pat_vars=({ struct Cyc_Core_Opt*
_temp1954=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1954->v=( void*)({ struct Cyc_List_List* _temp1955=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1955->hd=( void*) _temp1938;
_temp1955->tl= 0; _temp1955;}); _temp1954;}); _temp1953->where_clause= 0;
_temp1953->body= _temp1943; _temp1953->loc= 0; _temp1953;}); struct Cyc_Absyn_Stmt*
_temp1945= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1863,({ struct Cyc_List_List*
_temp1952=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1952->hd=( void*) _temp1944; _temp1952->tl= 0; _temp1952;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1945);{ struct Cyc_Absyn_Exp* _temp1946= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1951=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1951->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1951->tl= 0; _temp1951;}), 0); struct Cyc_Absyn_Stmt*
_temp1947= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1950=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1950->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1950->tl= 0; _temp1950;}), 0), 0); struct Cyc_Absyn_Exp* _temp1948= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1949= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1947, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1948, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1946, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1949, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1935, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1945, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1796: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1869->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1867);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_Absyn_Exp* _temp1960[ 1u]; _temp1960[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1960, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), Cyc_Absyn_seq_stmt(
_temp1867, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp* _temp1961[ 1u]; _temp1961[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1961, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), 0), 0), 0), 0))->r)); return;} _LL1798:({ void* _temp1962[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("switchC_s", sizeof( unsigned char), 10u), _tag_arr( _temp1962, sizeof(
void*), 0u));}); return; _LL1800:({ void* _temp1963[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof(
unsigned char), 4u), _tag_arr( _temp1963, sizeof( void*), 0u));}); return;
_LL1802:({ void* _temp1964[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u),
_tag_arr( _temp1964, sizeof( void*), 0u));}); return; _LL1762:;}} struct
_tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp1965= f->args; for( 0;
_temp1965 !=  0; _temp1965=(( struct Cyc_List_List*) _check_null( _temp1965))->tl){(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1965))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1965))->hd)).f3);{ struct _tuple0* _temp1966=({ struct _tuple0* _temp1967=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp1967->f1=( void*) Cyc_Absyn_Loc_n;
_temp1967->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1965))->hd)).f1; _temp1967;}); nv= Cyc_Toc_add_varmap( nv, _temp1966, Cyc_Absyn_var_exp(
_temp1966, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp1970; void* _temp1972;
struct Cyc_Absyn_Tqual _temp1974; struct Cyc_Core_Opt* _temp1976; struct Cyc_Absyn_VarargInfo
_temp1968=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL1977: _temp1976= _temp1968.name; goto _LL1975; _LL1975: _temp1974= _temp1968.tq;
goto _LL1973; _LL1973: _temp1972=( void*) _temp1968.type; goto _LL1971; _LL1971:
_temp1970= _temp1968.inject; goto _LL1969; _LL1969: { void* _temp1978= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp1972,( void*) Cyc_Absyn_HeapRgn, _temp1974)); struct
_tuple0* _temp1979=({ struct _tuple0* _temp1982=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp1982->f1=( void*) Cyc_Absyn_Loc_n; _temp1982->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp1976))->v;
_temp1982;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp1980=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1980->hd=( void*)({ struct _tuple15* _temp1981=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp1981->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp1976))->v; _temp1981->f2= _temp1974;
_temp1981->f3= _temp1978; _temp1981;}); _temp1980->tl= 0; _temp1980;})); nv= Cyc_Toc_add_varmap(
nv, _temp1979, Cyc_Absyn_var_exp( _temp1979, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp1983=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp1983 !=  0; _temp1983=((
struct Cyc_List_List*) _check_null( _temp1983))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1983))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1983))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1984= s; _LL1986: if(
_temp1984 == ( void*) Cyc_Absyn_Abstract){ goto _LL1987;} else{ goto _LL1988;}
_LL1988: if( _temp1984 == ( void*) Cyc_Absyn_ExternC){ goto _LL1989;} else{ goto
_LL1990;} _LL1990: goto _LL1991; _LL1987: return( void*) Cyc_Absyn_Public;
_LL1989: return( void*) Cyc_Absyn_Extern; _LL1991: return s; _LL1985:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp1992=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1993=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1993->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp1993;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1994=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp1992).f2); if( _temp1994 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp1992).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1994))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Structdecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp1992).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp1995=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1995->v=(
void*) _temp1992; _temp1995;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp1996=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp1996 !=  0; _temp1996=(( struct Cyc_List_List*)
_check_null( _temp1996))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1996))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1996))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp1997=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp1998=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1998->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp1998;});}{ int
seen_defn_before; struct Cyc_Core_Opt* _temp1999=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)(( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp1997).f2); if( _temp1999 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp1997).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1999))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
k, struct Cyc_Absyn_Uniondecl* v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp1997).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2000=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2000->v=(
void*) _temp1997; _temp2000;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2001=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2001 !=  0; _temp2001=(( struct Cyc_List_List*)
_check_null( _temp2001))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2001))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2001))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2002=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2002->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2002;});}{ struct
_tuple0* _temp2003= tud->name; if( tud->fields ==  0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2003)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2003));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2004=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2004 !=  0; _temp2004=(( struct Cyc_List_List*) _check_null(
_temp2004))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2004))->hd; if( f->typs ==  0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2005; _temp2005.q_const= 1;
_temp2005.q_volatile= 0; _temp2005.q_restrict= 0; _temp2005;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2006=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2006->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2007=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2007[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2008; _temp2008.tag= Cyc_Absyn_Var_d; _temp2008.f1= vd; _temp2008;});
_temp2007;}), 0); _temp2006->tl= Cyc_Toc_result_decls; _temp2006;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2009; _temp2009.q_const=
1; _temp2009.q_volatile= 0; _temp2009.q_restrict= 0; _temp2009;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2010=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2010->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2011=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2011[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2012; _temp2012.tag= Cyc_Absyn_Var_d; _temp2012.f1= vd; _temp2012;});
_temp2011;}), 0); _temp2010->tl= Cyc_Toc_result_decls; _temp2010;});{ struct Cyc_List_List*
_temp2013= 0; int i= 1;{ struct Cyc_List_List* _temp2014= f->typs; for( 0;
_temp2014 !=  0;( _temp2014=(( struct Cyc_List_List*) _check_null( _temp2014))->tl,
i ++)){ struct _tagged_arr* _temp2015= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2016=({ struct Cyc_Absyn_Structfield* _temp2018=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2018->name= _temp2015;
_temp2018->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2014))->hd)).f1; _temp2018->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2014))->hd)).f2);
_temp2018->width= 0; _temp2018->attributes= 0; _temp2018;}); _temp2013=({ struct
Cyc_List_List* _temp2017=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2017->hd=( void*) _temp2016; _temp2017->tl= _temp2013; _temp2017;});}}
_temp2013=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2013); _temp2013=({ struct Cyc_List_List* _temp2019=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2019->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2020=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2020->name= Cyc_Toc_tag_sp; _temp2020->tq= Cyc_Toc_mt_tq; _temp2020->type=(
void*) Cyc_Absyn_sint_t; _temp2020->width= 0; _temp2020->attributes= 0;
_temp2020;}); _temp2019->tl= _temp2013; _temp2019;});{ struct Cyc_Absyn_Structdecl*
_temp2021=({ struct Cyc_Absyn_Structdecl* _temp2025=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2025->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2025->name=({ struct Cyc_Core_Opt* _temp2027=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2027->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2027;}); _temp2025->tvs= 0; _temp2025->fields=({ struct Cyc_Core_Opt*
_temp2026=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2026->v=( void*) _temp2013; _temp2026;}); _temp2025->attributes= 0;
_temp2025;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2022=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2022->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2023=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2023[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2024; _temp2024.tag= Cyc_Absyn_Struct_d;
_temp2024.f1= _temp2021; _temp2024;}); _temp2023;}), 0); _temp2022->tl= Cyc_Toc_result_decls;
_temp2022;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp2028=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2028->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2028;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp2029= xd->name; struct Cyc_List_List*
_temp2030=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2030 !=  0; _temp2030=(( struct Cyc_List_List*) _check_null(
_temp2030))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2030))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2031= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2032=({ struct
Cyc_Absyn_ArrayType_struct* _temp2082=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2082[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2083; _temp2083.tag= Cyc_Absyn_ArrayType;
_temp2083.f1=( void*) Cyc_Absyn_uchar_t; _temp2083.f2= Cyc_Toc_mt_tq; _temp2083.f3=(
struct Cyc_Absyn_Exp*) _temp2031; _temp2083;}); _temp2082;}); struct Cyc_Core_Opt*
_temp2033=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2041; int _temp2042; _LL2035: if( _temp2033 ==  0){ goto _LL2036;} else{
goto _LL2037;} _LL2037: if( _temp2033 ==  0){ goto _LL2039;} else{ _temp2041=*
_temp2033; _LL2043: _temp2042=( int) _temp2041.v; if( _temp2042 ==  0){ goto
_LL2038;} else{ goto _LL2039;}} _LL2039: goto _LL2040; _LL2036: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2049; _temp2049.tag= Cyc_Std_String_pa; _temp2049.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp2048; _temp2048.tag= Cyc_Std_Int_pa;
_temp2048.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2047;
_temp2047.tag= Cyc_Std_Int_pa; _temp2047.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp2046; _temp2046.tag= Cyc_Std_Int_pa; _temp2046.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2045; _temp2045.tag=
Cyc_Std_Int_pa; _temp2045.f1=( int)(( unsigned int) 0);{ void* _temp2044[ 5u]={&
_temp2045,& _temp2046,& _temp2047,& _temp2048,& _temp2049}; Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u), _tag_arr( _temp2044,
sizeof( void*), 5u));}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl* _temp2050= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2032, initopt);( void*)( _temp2050->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2051=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2051->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2052=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2052[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2053; _temp2053.tag= Cyc_Absyn_Var_d; _temp2053.f1= _temp2050; _temp2053;});
_temp2052;}), 0); _temp2051->tl= Cyc_Toc_result_decls; _temp2051;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2054= f->typs; for( 0; _temp2054 !=  0;( _temp2054=(( struct Cyc_List_List*)
_check_null( _temp2054))->tl, i ++)){ struct _tagged_arr* _temp2055=({ struct
_tagged_arr* _temp2059=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2059[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp2061; _temp2061.tag= Cyc_Std_Int_pa; _temp2061.f1=( int)(( unsigned int) i);{
void* _temp2060[ 1u]={& _temp2061}; Cyc_Std_aprintf( _tag_arr("f%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2060, sizeof( void*), 1u));}}); _temp2059;});
struct Cyc_Absyn_Structfield* _temp2056=({ struct Cyc_Absyn_Structfield*
_temp2058=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2058->name= _temp2055; _temp2058->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2054))->hd)).f1; _temp2058->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2054))->hd)).f2);
_temp2058->width= 0; _temp2058->attributes= 0; _temp2058;}); fields=({ struct
Cyc_List_List* _temp2057=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2057->hd=( void*) _temp2056; _temp2057->tl= fields; _temp2057;});}} fields=({
struct Cyc_List_List* _temp2062=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2062->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2063=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2063->name= Cyc_Toc_tag_sp; _temp2063->tq= Cyc_Toc_mt_tq; _temp2063->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2063->width=
0; _temp2063->attributes= 0; _temp2063;}); _temp2062->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2062;});{ struct Cyc_Absyn_Structdecl*
_temp2064=({ struct Cyc_Absyn_Structdecl* _temp2068=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2068->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2068->name=({ struct Cyc_Core_Opt* _temp2070=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2070->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2070;}); _temp2068->tvs= 0; _temp2068->fields=({ struct Cyc_Core_Opt*
_temp2069=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2069->v=( void*) fields; _temp2069;}); _temp2068->attributes= 0; _temp2068;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2065=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2065->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2066=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2066[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2067; _temp2067.tag= Cyc_Absyn_Struct_d;
_temp2067.f1= _temp2064; _temp2067;}); _temp2066;}), 0); _temp2065->tl= Cyc_Toc_result_decls;
_temp2065;});}} goto _LL2034;}} _LL2038: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2071= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2081; _temp2081.tag= Cyc_Std_String_pa;
_temp2081.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2080;
_temp2080.tag= Cyc_Std_Int_pa; _temp2080.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp2079; _temp2079.tag= Cyc_Std_Int_pa; _temp2079.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2078; _temp2078.tag=
Cyc_Std_Int_pa; _temp2078.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct
_temp2077; _temp2077.tag= Cyc_Std_Int_pa; _temp2077.f1=( int)(( unsigned int) 0);{
void* _temp2076[ 5u]={& _temp2077,& _temp2078,& _temp2079,& _temp2080,&
_temp2081}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u),
_tag_arr( _temp2076, sizeof( void*), 5u));}}}}}}), 0); struct Cyc_Absyn_Vardecl*
_temp2072= Cyc_Absyn_new_vardecl( f->name,( void*) _temp2032,( struct Cyc_Absyn_Exp*)
_temp2071);( void*)( _temp2072->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2073=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2073->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2074=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2074[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2075; _temp2075.tag= Cyc_Absyn_Var_d; _temp2075.f1= _temp2072; _temp2075;});
_temp2074;}), 0); _temp2073->tl= Cyc_Toc_result_decls; _temp2073;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2034; _LL2040: goto
_LL2034; _LL2034:;}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2084= ed->name;( void*)( ed->sc=(
void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ struct Cyc_List_List*
_temp2085=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; _temp2085 !=  0; _temp2085=(( struct Cyc_List_List*) _check_null(
_temp2085))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2085))->hd)->tag !=  0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2085))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2085))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2086=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2092; struct Cyc_Absyn_Exp* _temp2094; struct Cyc_Absyn_Vardecl* _temp2096;
_LL2088: if(*(( int*) _temp2086) ==  Cyc_Absyn_Comprehension_e){ _LL2097:
_temp2096=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2086)->f1; goto
_LL2095; _LL2095: _temp2094=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2086)->f2; goto _LL2093; _LL2093: _temp2092=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2086)->f3; goto _LL2089;} else{ goto _LL2090;} _LL2090: goto _LL2091;
_LL2089: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2096, _temp2094, _temp2092, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2087; _LL2091: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2098= Cyc_Toc_copy_env( init_nv);
_temp2098->toplevel= 1; Cyc_Toc_exp_to_c( _temp2098, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2087; _LL2087:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2099= _new_region(); struct _RegionHandle* prgn=& _temp2099;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2102; struct Cyc_List_List*
_temp2104; struct Cyc_Toc_Env* _temp2106; struct _tuple11 _temp2100= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2107: _temp2106= _temp2100.f1; goto _LL2105; _LL2105: _temp2104=
_temp2100.f2; goto _LL2103; _LL2103: _temp2102= _temp2100.f3; goto _LL2101;
_LL2101: Cyc_Toc_stmt_to_c( _temp2106, s);{ struct Cyc_Absyn_Stmt* _temp2108=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2102, _temp2108, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2109= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2110= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2109, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2102,
Cyc_Absyn_seq_stmt( _temp2110, _temp2108, 0), 0), 0);} for( 0; _temp2104 !=  0;
_temp2104=(( struct Cyc_List_List*) _check_null( _temp2104))->tl){ struct
_tuple12 _temp2113; void* _temp2114; struct _tuple0* _temp2116; struct _tuple12*
_temp2111=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2104))->hd;
_temp2113=* _temp2111; _LL2117: _temp2116= _temp2113.f1; goto _LL2115; _LL2115:
_temp2114= _temp2113.f2; goto _LL2112; _LL2112: s= Cyc_Absyn_declare_stmt(
_temp2116, _temp2114, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ void* _temp2118[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u), _tag_arr( _temp2118, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2119=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2145; struct Cyc_Absyn_Fndecl* _temp2147; struct Cyc_Absyn_Structdecl*
_temp2149; struct Cyc_Absyn_Uniondecl* _temp2151; struct Cyc_Absyn_Tuniondecl*
_temp2153; struct Cyc_Absyn_Enumdecl* _temp2155; struct Cyc_Absyn_Typedefdecl*
_temp2157; struct Cyc_List_List* _temp2159; struct Cyc_List_List* _temp2161;
struct Cyc_List_List* _temp2163; _LL2121: if(*(( int*) _temp2119) ==  Cyc_Absyn_Var_d){
_LL2146: _temp2145=(( struct Cyc_Absyn_Var_d_struct*) _temp2119)->f1; goto
_LL2122;} else{ goto _LL2123;} _LL2123: if(*(( int*) _temp2119) ==  Cyc_Absyn_Fn_d){
_LL2148: _temp2147=(( struct Cyc_Absyn_Fn_d_struct*) _temp2119)->f1; goto
_LL2124;} else{ goto _LL2125;} _LL2125: if(*(( int*) _temp2119) ==  Cyc_Absyn_Let_d){
goto _LL2126;} else{ goto _LL2127;} _LL2127: if(*(( int*) _temp2119) ==  Cyc_Absyn_Letv_d){
goto _LL2128;} else{ goto _LL2129;} _LL2129: if(*(( int*) _temp2119) ==  Cyc_Absyn_Struct_d){
_LL2150: _temp2149=(( struct Cyc_Absyn_Struct_d_struct*) _temp2119)->f1; goto
_LL2130;} else{ goto _LL2131;} _LL2131: if(*(( int*) _temp2119) ==  Cyc_Absyn_Union_d){
_LL2152: _temp2151=(( struct Cyc_Absyn_Union_d_struct*) _temp2119)->f1; goto
_LL2132;} else{ goto _LL2133;} _LL2133: if(*(( int*) _temp2119) ==  Cyc_Absyn_Tunion_d){
_LL2154: _temp2153=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2119)->f1; goto
_LL2134;} else{ goto _LL2135;} _LL2135: if(*(( int*) _temp2119) ==  Cyc_Absyn_Enum_d){
_LL2156: _temp2155=(( struct Cyc_Absyn_Enum_d_struct*) _temp2119)->f1; goto
_LL2136;} else{ goto _LL2137;} _LL2137: if(*(( int*) _temp2119) ==  Cyc_Absyn_Typedef_d){
_LL2158: _temp2157=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2119)->f1; goto
_LL2138;} else{ goto _LL2139;} _LL2139: if(*(( int*) _temp2119) ==  Cyc_Absyn_Namespace_d){
_LL2160: _temp2159=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2119)->f2; goto
_LL2140;} else{ goto _LL2141;} _LL2141: if(*(( int*) _temp2119) ==  Cyc_Absyn_Using_d){
_LL2162: _temp2161=(( struct Cyc_Absyn_Using_d_struct*) _temp2119)->f2; goto
_LL2142;} else{ goto _LL2143;} _LL2143: if(*(( int*) _temp2119) ==  Cyc_Absyn_ExternC_d){
_LL2164: _temp2163=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2119)->f1; goto
_LL2144;} else{ goto _LL2120;} _LL2122: { struct _tuple0* _temp2165= _temp2145->name;
if(( void*) _temp2145->sc == ( void*) Cyc_Absyn_ExternC){ _temp2165=({ struct
_tuple0* _temp2166=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2166->f1= Cyc_Absyn_rel_ns_null; _temp2166->f2=(* _temp2165).f2; _temp2166;});}
if( _temp2145->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2145->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2145->name,
Cyc_Absyn_varb_exp( _temp2165,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2167=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2167[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2168; _temp2168.tag= Cyc_Absyn_Global_b;
_temp2168.f1= _temp2145; _temp2168;}); _temp2167;}), 0)); _temp2145->name=
_temp2165;( void*)( _temp2145->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2145->sc));(
void*)( _temp2145->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2145->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2169=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2169->hd=( void*) d; _temp2169->tl=
Cyc_Toc_result_decls; _temp2169;}); goto _LL2120;} _LL2124: { struct _tuple0*
_temp2170= _temp2147->name; if(( void*) _temp2147->sc == ( void*) Cyc_Absyn_ExternC){
_temp2170=({ struct _tuple0* _temp2171=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2171->f1= Cyc_Absyn_rel_ns_null; _temp2171->f2=(*
_temp2170).f2; _temp2171;});} nv= Cyc_Toc_add_varmap( nv, _temp2147->name, Cyc_Absyn_var_exp(
_temp2170, 0)); _temp2147->name= _temp2170; Cyc_Toc_fndecl_to_c( nv, _temp2147);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2172=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2172->hd=( void*) d; _temp2172->tl=
Cyc_Toc_result_decls; _temp2172;}); goto _LL2120;} _LL2126: goto _LL2128;
_LL2128:({ void* _temp2173[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2173, sizeof( void*), 0u));}); goto _LL2120;
_LL2130: Cyc_Toc_structdecl_to_c( _temp2149); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2174=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2174->hd=( void*) d; _temp2174->tl= Cyc_Toc_result_decls; _temp2174;});
goto _LL2120; _LL2132: Cyc_Toc_uniondecl_to_c( _temp2151); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2175=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2175->hd=( void*) d; _temp2175->tl= Cyc_Toc_result_decls;
_temp2175;}); goto _LL2120; _LL2134: if( _temp2153->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2153);} else{ Cyc_Toc_tuniondecl_to_c( _temp2153);} goto _LL2120; _LL2136:
Cyc_Toc_enumdecl_to_c( nv, _temp2155); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2176=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2176->hd=( void*) d; _temp2176->tl= Cyc_Toc_result_decls; _temp2176;});
goto _LL2120; _LL2138: _temp2157->name= _temp2157->name; _temp2157->tvs= 0;(
void*)( _temp2157->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2157->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2177=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2177->hd=( void*) d; _temp2177->tl=
Cyc_Toc_result_decls; _temp2177;}); goto _LL2120; _LL2140: _temp2161= _temp2159;
goto _LL2142; _LL2142: _temp2163= _temp2161; goto _LL2144; _LL2144: nv= Cyc_Toc_decls_to_c(
nv, _temp2163, top); goto _LL2120; _LL2120:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
