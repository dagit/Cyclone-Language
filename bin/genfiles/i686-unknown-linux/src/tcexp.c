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
struct _tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct
_tuple0*); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern struct Cyc_List_List*
Cyc_List_nth_tail( struct Cyc_List_List* x, int i); extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void*
Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_Std_zstrcmp( struct _tagged_arr, struct _tagged_arr); struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern struct _tagged_arr Cyc_Std_vrprintf( struct _RegionHandle* r1, struct
_tagged_arr fmt, struct _tagged_arr ap); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); struct
_tuple4{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; struct _tuple5{
struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple4*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, int
allow_evars, void*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref* b); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple6{ int f1; void* f2; } ; extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern int Cyc_Evexp_okay_szofarg( void* t); extern
void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int
new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*); extern
struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part);
struct Cyc_CfFlowInfo_Place; enum  Cyc_CfFlowInfo_Escaped; struct Cyc_CfFlowInfo_InitState;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ; enum 
Cyc_CfFlowInfo_Escaped{ Cyc_CfFlowInfo_Esc  =  0u, Cyc_CfFlowInfo_Unesc  =  1u};
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_MustPointTo=
0; struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; struct Cyc_CfFlowInfo_InitState{ enum  Cyc_CfFlowInfo_Escaped esc; void*
level; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; struct Cyc_CfFlowInfo_InitState f1; } ; static const int Cyc_CfFlowInfo_DictPI=
1; struct Cyc_CfFlowInfo_DictPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; struct Cyc_Set_Set** all_changed; } ; static
void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap){({ void* _temp0[ 0u]={}; Cyc_Tcutil_terr(
loc,( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, msg, ap),
_tag_arr( _temp0, sizeof( void*), 0u));}); return Cyc_Absyn_wildtyp(({ struct
Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1;}));} static struct
Cyc_Core_Opt Cyc_Tcexp_rk={( void*)(( void*) 3u)}; static struct Cyc_Core_Opt
Cyc_Tcexp_ak={( void*)(( void*) 0u)}; static struct Cyc_Core_Opt Cyc_Tcexp_bk={(
void*)(( void*) 2u)}; static struct Cyc_Core_Opt Cyc_Tcexp_mk={( void*)(( void*)
1u)}; static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct _handler_cons _temp2;
_push_handler(& _temp2);{ int _temp4= 0; if( setjmp( _temp2.handler)){ _temp4= 1;}
if( ! _temp4){{ void* _temp5= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp15; struct Cyc_Absyn_Enumfield* _temp17; struct Cyc_Absyn_Enumdecl* _temp19;
struct Cyc_Absyn_Tunionfield* _temp21; struct Cyc_Absyn_Tuniondecl* _temp23;
_LL7: if(*(( int*) _temp5) ==  Cyc_Tcenv_VarRes){ _LL16: _temp15=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9:
if(*(( int*) _temp5) ==  Cyc_Tcenv_EnumRes){ _LL20: _temp19=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f1; goto _LL18; _LL18: _temp17=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f2; goto _LL10;} else{ goto _LL11;} _LL11: if(*(( int*) _temp5) ==  Cyc_Tcenv_TunionRes){
_LL24: _temp23=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f1; goto _LL22;
_LL22: _temp21=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f2; goto _LL12;}
else{ goto _LL13;} _LL13: if(*(( int*) _temp5) ==  Cyc_Tcenv_StructRes){ goto
_LL14;} else{ goto _LL6;} _LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp25=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp25[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp26; _temp26.tag= Cyc_Absyn_Var_e;
_temp26.f1= q; _temp26.f2=( void*) _temp15; _temp26;}); _temp25;}))); goto _LL6;
_LL10:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp27=(
struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp27[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp28; _temp28.tag= Cyc_Absyn_Enum_e;
_temp28.f1= q; _temp28.f2=( struct Cyc_Absyn_Enumdecl*) _temp19; _temp28.f3=(
struct Cyc_Absyn_Enumfield*) _temp17; _temp28;}); _temp27;}))); goto _LL6; _LL12:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp29=(
struct Cyc_Absyn_Tunion_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp30; _temp30.tag= Cyc_Absyn_Tunion_e;
_temp30.f1= 0; _temp30.f2= 0; _temp30.f3= 0; _temp30.f4= _temp23; _temp30.f5=
_temp21; _temp30;}); _temp29;}))); goto _LL6; _LL14:({ struct Cyc_Std_String_pa_struct
_temp32; _temp32.tag= Cyc_Std_String_pa; _temp32.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp31[ 1u]={& _temp32}; Cyc_Tcutil_terr( e->loc, _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u), _tag_arr( _temp31, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp33=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp33[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp34; _temp34.tag= Cyc_Absyn_Var_e; _temp34.f1= q; _temp34.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp34;}); _temp33;}))); goto _LL6; _LL6:;};
_pop_handler();} else{ void* _temp3=( void*) _exn_thrown; void* _temp36= _temp3;
_LL38: if( _temp36 ==  Cyc_Dict_Absent){ goto _LL39;} else{ goto _LL40;} _LL40:
goto _LL41; _LL39:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp42=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp43; _temp43.tag= Cyc_Absyn_Var_e;
_temp43.f1= q; _temp43.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp43;});
_temp42;}))); goto _LL37; _LL41:( void) _throw( _temp36); _LL37:;}}} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple7* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple7* _temp44=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp44->f1=
0; _temp44->f2= e; _temp44;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp45=( void*) e1->r; struct _tuple1* _temp51; _LL47:
if(*(( int*) _temp45) ==  Cyc_Absyn_UnknownId_e){ _LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp45)->f1; goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct
_handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp(
_temp53.handler)){ _temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68;
struct Cyc_Absyn_Tuniondecl* _temp70; struct Cyc_Absyn_Structdecl* _temp72;
_LL58: if(*(( int*) _temp56) ==  Cyc_Tcenv_VarRes){ _LL67: _temp66=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto _LL59;} else{ goto _LL60;}
_LL60: if(*(( int*) _temp56) ==  Cyc_Tcenv_TunionRes){ _LL71: _temp70=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69; _LL69: _temp68=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;} else{ goto _LL62;} _LL62:
if(*(( int*) _temp56) ==  Cyc_Tcenv_StructRes){ _LL73: _temp72=(( struct Cyc_Tcenv_StructRes_struct*)
_temp56)->f1; goto _LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp56) == 
Cyc_Tcenv_EnumRes){ goto _LL65;} else{ goto _LL57;} _LL59:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp74=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp74[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp75; _temp75.tag= Cyc_Absyn_FnCall_e; _temp75.f1= e1; _temp75.f2= es;
_temp75.f3= 0; _temp75;}); _temp74;}))); _npop_handler( 0u); return; _LL61: if(
_temp68->typs ==  0){({ struct Cyc_Std_String_pa_struct _temp77; _temp77.tag=
Cyc_Std_String_pa; _temp77.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp68->name);{ void* _temp76[ 1u]={& _temp77}; Cyc_Tcutil_terr( e->loc,
_tag_arr("%s is a constant, not a function", sizeof( unsigned char), 33u),
_tag_arr( _temp76, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp78=( struct Cyc_Absyn_Tunion_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp78[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp79; _temp79.tag= Cyc_Absyn_Tunion_e; _temp79.f1= 0; _temp79.f2= 0; _temp79.f3=
es; _temp79.f4= _temp70; _temp79.f5= _temp68; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL63: { struct Cyc_List_List*
_temp80=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp72->name !=  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp81=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp82; _temp82.tag= Cyc_Absyn_Struct_e;
_temp82.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp72->name))->v;
_temp82.f2= 0; _temp82.f3= _temp80; _temp82.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp82;}); _temp81;})));} else{({ void* _temp83[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("missing struct name", sizeof( unsigned char), 20u), _tag_arr(
_temp83, sizeof( void*), 0u));});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp84=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp85; _temp85.tag= Cyc_Absyn_Struct_e;
_temp85.f1= _temp51; _temp85.f2= 0; _temp85.f3= _temp80; _temp85.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp85;}); _temp84;})));} _npop_handler( 0u); return;} _LL65:({ struct
Cyc_Std_String_pa_struct _temp87; _temp87.tag= Cyc_Std_String_pa; _temp87.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp51);{ void* _temp86[ 1u]={&
_temp87}; Cyc_Tcutil_terr( e->loc, _tag_arr("%s is an enum constructor, not a function",
sizeof( unsigned char), 42u), _tag_arr( _temp86, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL57:;}; _pop_handler();} else{ void* _temp54=(
void*) _exn_thrown; void* _temp89= _temp54; _LL91: if( _temp89 ==  Cyc_Dict_Absent){
goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL92:({ struct Cyc_Std_String_pa_struct
_temp96; _temp96.tag= Cyc_Std_String_pa; _temp96.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp51);{ void* _temp95[ 1u]={& _temp96}; Cyc_Tcutil_terr( e1->loc, _tag_arr("unbound identifier %s",
sizeof( unsigned char), 22u), _tag_arr( _temp95, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp97=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp97[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp98; _temp98.tag= Cyc_Absyn_FnCall_e; _temp98.f1= e1; _temp98.f2= es;
_temp98.f3= 0; _temp98;}); _temp97;}))); return; _LL94:( void) _throw( _temp89);
_LL90:;}}} _LL50:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp99=( struct Cyc_Absyn_FnCall_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp99[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp100; _temp100.tag= Cyc_Absyn_FnCall_e;
_temp100.f1= e1; _temp100.f2= es; _temp100.f3= 0; _temp100;}); _temp99;})));
return; _LL46:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* des){ if( topt ==  0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp101=( struct Cyc_Absyn_Array_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp101[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp102; _temp102.tag= Cyc_Absyn_Array_e; _temp102.f1= des; _temp102;});
_temp101;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp103=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual _temp113; void* _temp115; struct
Cyc_Absyn_Structdecl** _temp117; struct Cyc_List_List* _temp119; struct _tuple1*
_temp121; _LL105: if(( unsigned int) _temp103 >  4u?*(( int*) _temp103) ==  Cyc_Absyn_ArrayType:
0){ _LL116: _temp115=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f1;
goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f2;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 >  4u?*((
int*) _temp103) ==  Cyc_Absyn_StructType: 0){ _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f1; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f2; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp103 >  4u?*(( int*) _temp103) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL110;}
else{ goto _LL111;} _LL111: goto _LL112; _LL106:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp123=( struct Cyc_Absyn_Array_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp123[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp124; _temp124.tag= Cyc_Absyn_Array_e; _temp124.f1= des; _temp124;});
_temp123;}))); goto _LL104; _LL108: if( _temp117 ==  0? 1: _temp121 ==  0){({
void* _temp125[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u), _tag_arr( _temp125, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp126=(
struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp126[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp127; _temp127.tag= Cyc_Absyn_Struct_e;
_temp127.f1=( struct _tuple1*) _check_null( _temp121); _temp127.f2= 0; _temp127.f3=
des; _temp127.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp117)); _temp127;}); _temp126;}))); goto _LL104; _LL110:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp128=( struct
Cyc_Absyn_AnonStruct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp128[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp129; _temp129.tag= Cyc_Absyn_AnonStruct_e;
_temp129.f1=( void*) t; _temp129.f2= des; _temp129;}); _temp128;}))); goto
_LL104; _LL112:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp130=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp130[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp131; _temp131.tag= Cyc_Absyn_Array_e;
_temp131.f1= des; _temp131;}); _temp130;}))); goto _LL104; _LL104:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es !=  0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){({ struct Cyc_Std_String_pa_struct _temp134; _temp134.tag= Cyc_Std_String_pa;
_temp134.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp133; _temp133.tag=
Cyc_Std_String_pa; _temp133.f1=( struct _tagged_arr) msg_part;{ void* _temp132[
2u]={& _temp133,& _temp134}; Cyc_Tcutil_terr( e->loc, _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u), _tag_arr( _temp132, sizeof( void*), 2u));}}});}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp135= c;
void* _temp151; void* _temp153; void* _temp155; int _temp157; void* _temp159;
struct _tagged_arr _temp161; _LL137: if(( unsigned int) _temp135 >  1u?*(( int*)
_temp135) ==  Cyc_Absyn_Char_c: 0){ _LL152: _temp151=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp135)->f1; goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_Short_c: 0){ _LL154: _temp153=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp135)->f1; goto _LL140;} else{
goto _LL141;} _LL141: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135) == 
Cyc_Absyn_LongLong_c: 0){ _LL156: _temp155=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp135)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_Float_c: 0){ goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135)
==  Cyc_Absyn_Int_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f1; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_String_c: 0){ _LL162: _temp161=((
struct Cyc_Absyn_String_c_struct*) _temp135)->f1; goto _LL148;} else{ goto
_LL149;} _LL149: if( _temp135 == ( void*) Cyc_Absyn_Null_c){ goto _LL150;} else{
goto _LL136;} _LL138: t= _temp151 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL136; _LL140: t= _temp153 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL136; _LL142: t= _temp155 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto
_LL136; _LL144: t= Cyc_Absyn_float_t; goto _LL136; _LL146: if( topt ==  0){ t=
_temp159 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp163= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); void*
_temp173; void* _temp175; void* _temp177; void* _temp179; void* _temp181; _LL165:
if(( unsigned int) _temp163 >  4u?*(( int*) _temp163) ==  Cyc_Absyn_IntType: 0){
_LL176: _temp175=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp163)->f1;
goto _LL174; _LL174: _temp173=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp173 == ( void*) Cyc_Absyn_B1){ goto _LL166;} else{ goto
_LL167;}} else{ goto _LL167;} _LL167: if(( unsigned int) _temp163 >  4u?*(( int*)
_temp163) ==  Cyc_Absyn_IntType: 0){ _LL180: _temp179=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL178; _LL178: _temp177=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp177 == ( void*) Cyc_Absyn_B2){ goto _LL168;} else{ goto
_LL169;}} else{ goto _LL169;} _LL169: if(( unsigned int) _temp163 >  4u?*(( int*)
_temp163) ==  Cyc_Absyn_IntType: 0){ _LL182: _temp181=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL170;} else{ goto _LL171;} _LL171: goto _LL172; _LL166: t=
_temp175 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp183=(
struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp184; _temp184.tag= Cyc_Absyn_Const_e;
_temp184.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp185=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp185[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp186; _temp186.tag= Cyc_Absyn_Char_c;
_temp186.f1=( void*) _temp175; _temp186.f2=( unsigned char) _temp157; _temp186;});
_temp185;})); _temp184;}); _temp183;}))); goto _LL164; _LL168: t= _temp179 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp187=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp187[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp188; _temp188.tag= Cyc_Absyn_Const_e; _temp188.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp189=( struct Cyc_Absyn_Short_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp189[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp190; _temp190.tag= Cyc_Absyn_Short_c; _temp190.f1=( void*) _temp179;
_temp190.f2=( short) _temp157; _temp190;}); _temp189;})); _temp188;}); _temp187;})));
goto _LL164; _LL170: t= _temp181 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL164; _LL172: t= _temp159 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL164; _LL164:;} goto _LL136; _LL148: {
int len=( int) _get_arr_size( _temp161, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp191=({ struct Cyc_Absyn_Const_e_struct* _temp228=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp228[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp229; _temp229.tag= Cyc_Absyn_Const_e; _temp229.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp230=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp230[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp231; _temp231.tag= Cyc_Absyn_Int_c; _temp231.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp231.f2= len; _temp231;}); _temp230;})); _temp229;}); _temp228;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp191, loc); elen->topt=({
struct Cyc_Core_Opt* _temp192=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp192->v=( void*) Cyc_Absyn_uint_t; _temp192;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp193=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp193[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp194; _temp194.tag= Cyc_Absyn_Upper_b; _temp194.f1= elen; _temp194;});
_temp193;})); if( topt !=  0){ void* _temp195= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp203; struct Cyc_Absyn_Tqual
_temp205; void* _temp207; struct Cyc_Absyn_PtrInfo _temp209; struct Cyc_Absyn_Conref*
_temp211; struct Cyc_Absyn_Tqual _temp213; struct Cyc_Absyn_Conref* _temp215;
void* _temp217; void* _temp219; _LL197: if(( unsigned int) _temp195 >  4u?*((
int*) _temp195) ==  Cyc_Absyn_ArrayType: 0){ _LL208: _temp207=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp195)->f1; goto _LL206; _LL206: _temp205=((
struct Cyc_Absyn_ArrayType_struct*) _temp195)->f2; goto _LL204; _LL204: _temp203=((
struct Cyc_Absyn_ArrayType_struct*) _temp195)->f3; goto _LL198;} else{ goto
_LL199;} _LL199: if(( unsigned int) _temp195 >  4u?*(( int*) _temp195) ==  Cyc_Absyn_PointerType:
0){ _LL210: _temp209=(( struct Cyc_Absyn_PointerType_struct*) _temp195)->f1;
_LL220: _temp219=( void*) _temp209.elt_typ; goto _LL218; _LL218: _temp217=( void*)
_temp209.rgn_typ; goto _LL216; _LL216: _temp215= _temp209.nullable; goto _LL214;
_LL214: _temp213= _temp209.tq; goto _LL212; _LL212: _temp211= _temp209.bounds;
goto _LL200;} else{ goto _LL201;} _LL201: goto _LL202; _LL198: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp221=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp221[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp222; _temp222.tag= Cyc_Absyn_ArrayType; _temp222.f1=(
void*) Cyc_Absyn_uchar_t; _temp222.f2= _temp205; _temp222.f3=( struct Cyc_Absyn_Exp*)
elen; _temp222;}); _temp221;}); _LL200: if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp223=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp223->v=( void*) t; _temp223;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp224=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp224->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp224;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp225=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp225[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp226; _temp226.tag= Cyc_Absyn_Upper_b; _temp226.f1= elen; _temp226;});
_temp225;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp227=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp227->v=( void*) t; _temp227;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL196; _LL202:
goto _LL196; _LL196:;} return t;} _LL150: { struct Cyc_List_List* _temp232= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp233=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp233[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp234; _temp234.tag= Cyc_Absyn_PointerType;
_temp234.f1=({ struct Cyc_Absyn_PtrInfo _temp235; _temp235.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp237=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp237->v=( void*)
_temp232; _temp237;})); _temp235.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp236=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=( void*) _temp232;
_temp236;})); _temp235.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp235.tq= Cyc_Absyn_empty_tqual(); _temp235.bounds= Cyc_Absyn_empty_conref();
_temp235;}); _temp234;}); _temp233;}); goto _LL136;} _LL136:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp238= b; struct Cyc_Absyn_Vardecl*
_temp252; struct Cyc_Absyn_Fndecl* _temp254; struct Cyc_Absyn_Vardecl* _temp256;
struct Cyc_Absyn_Vardecl* _temp258; struct Cyc_Absyn_Vardecl* _temp260; _LL240:
if( _temp238 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL241;} else{ goto
_LL242;} _LL242: if(( unsigned int) _temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Global_b:
0){ _LL253: _temp252=(( struct Cyc_Absyn_Global_b_struct*) _temp238)->f1; goto
_LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp238 >  1u?*(( int*)
_temp238) ==  Cyc_Absyn_Funname_b: 0){ _LL255: _temp254=(( struct Cyc_Absyn_Funname_b_struct*)
_temp238)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int)
_temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Pat_b: 0){ _LL257: _temp256=((
struct Cyc_Absyn_Pat_b_struct*) _temp238)->f1; goto _LL247;} else{ goto _LL248;}
_LL248: if(( unsigned int) _temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Local_b:
0){ _LL259: _temp258=(( struct Cyc_Absyn_Local_b_struct*) _temp238)->f1; goto
_LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp238 >  1u?*(( int*)
_temp238) ==  Cyc_Absyn_Param_b: 0){ _LL261: _temp260=(( struct Cyc_Absyn_Param_b_struct*)
_temp238)->f1; goto _LL251;} else{ goto _LL239;} _LL241: return({ struct Cyc_Std_String_pa_struct
_temp263; _temp263.tag= Cyc_Std_String_pa; _temp263.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp262[ 1u]={& _temp263}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("unresolved identifier: %s",
sizeof( unsigned char), 26u), _tag_arr( _temp262, sizeof( void*), 1u));}});
_LL243: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp252->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp252->type; _LL245:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp254->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp254);
_LL247: _temp258= _temp256; goto _LL249; _LL249: _temp260= _temp258; goto _LL251;
_LL251:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp260->type; _LL239:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp264=( void*) fmt->r; void* _temp272; struct _tagged_arr
_temp274; struct Cyc_Absyn_Exp* _temp276; struct Cyc_Absyn_Exp _temp278; void*
_temp279; void* _temp281; struct _tagged_arr _temp283; _LL266: if(*(( int*)
_temp264) ==  Cyc_Absyn_Const_e){ _LL273: _temp272=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp264)->f1; if(( unsigned int) _temp272 >  1u?*(( int*) _temp272) ==  Cyc_Absyn_String_c:
0){ _LL275: _temp274=(( struct Cyc_Absyn_String_c_struct*) _temp272)->f1; goto
_LL267;} else{ goto _LL268;}} else{ goto _LL268;} _LL268: if(*(( int*) _temp264)
==  Cyc_Absyn_Cast_e){ _LL277: _temp276=(( struct Cyc_Absyn_Cast_e_struct*)
_temp264)->f2; _temp278=* _temp276; _LL280: _temp279=( void*) _temp278.r; if(*((
int*) _temp279) ==  Cyc_Absyn_Const_e){ _LL282: _temp281=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp279)->f1; if(( unsigned int) _temp281 >  1u?*(( int*) _temp281) ==  Cyc_Absyn_String_c:
0){ _LL284: _temp283=(( struct Cyc_Absyn_String_c_struct*) _temp281)->f1; goto
_LL269;} else{ goto _LL270;}} else{ goto _LL270;}} else{ goto _LL270;} _LL270:
goto _LL271; _LL267: _temp283= _temp274; goto _LL269; _LL269: desc_types=
type_getter( te,( struct _tagged_arr) _temp283, fmt->loc); goto _LL265; _LL271:
return; _LL265:;} if( opt_args !=  0){ struct Cyc_List_List* _temp285=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( opt_args))->v; for( 0;
desc_types !=  0? _temp285 !=  0: 0;( desc_types=(( struct Cyc_List_List*)
_check_null( desc_types))->tl, _temp285=(( struct Cyc_List_List*) _check_null(
_temp285))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp285))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct
Cyc_Std_String_pa_struct _temp288; _temp288.tag= Cyc_Std_String_pa; _temp288.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp287; _temp287.tag=
Cyc_Std_String_pa; _temp287.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp286[ 2u]={& _temp287,& _temp288}; Cyc_Tcutil_terr( e->loc, _tag_arr("descriptor has type %s but argument has type %s",
sizeof( unsigned char), 48u), _tag_arr( _temp286, sizeof( void*), 2u));}}});}}
if( desc_types !=  0){({ void* _temp289[ 0u]={}; Cyc_Tcutil_terr( fmt->loc,
_tag_arr("too few arguments", sizeof( unsigned char), 18u), _tag_arr( _temp289,
sizeof( void*), 0u));});} if( _temp285 !=  0){({ void* _temp290[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp285))->hd)->loc,
_tag_arr("too many arguments", sizeof( unsigned char), 19u), _tag_arr( _temp290,
sizeof( void*), 0u));});}}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp291= p; _LL293: if( _temp291 == ( void*) Cyc_Absyn_Plus){ goto _LL294;}
else{ goto _LL295;} _LL295: if( _temp291 == ( void*) Cyc_Absyn_Minus){ goto
_LL296;} else{ goto _LL297;} _LL297: if( _temp291 == ( void*) Cyc_Absyn_Not){
goto _LL298;} else{ goto _LL299;} _LL299: if( _temp291 == ( void*) Cyc_Absyn_Bitnot){
goto _LL300;} else{ goto _LL301;} _LL301: if( _temp291 == ( void*) Cyc_Absyn_Size){
goto _LL302;} else{ goto _LL303;} _LL303: goto _LL304; _LL294: goto _LL296;
_LL296: if( ! Cyc_Tcutil_is_numeric( e)){({ struct Cyc_Std_String_pa_struct
_temp306; _temp306.tag= Cyc_Std_String_pa; _temp306.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp305[ 1u]={& _temp306}; Cyc_Tcutil_terr( loc, _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u), _tag_arr( _temp305, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL298: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){({ struct Cyc_Std_String_pa_struct
_temp308; _temp308.tag= Cyc_Std_String_pa; _temp308.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp307[ 1u]={& _temp308}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u), _tag_arr( _temp307, sizeof( void*), 1u));}});}
return Cyc_Absyn_sint_t; _LL300: if( ! Cyc_Tcutil_is_integral( e)){({ struct Cyc_Std_String_pa_struct
_temp310; _temp310.tag= Cyc_Std_String_pa; _temp310.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp309[ 1u]={& _temp310}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp309, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL302:{ void*
_temp311= t; struct Cyc_Absyn_PtrInfo _temp319; _LL313: if(( unsigned int)
_temp311 >  4u?*(( int*) _temp311) ==  Cyc_Absyn_ArrayType: 0){ goto _LL314;}
else{ goto _LL315;} _LL315: if(( unsigned int) _temp311 >  4u?*(( int*) _temp311)
==  Cyc_Absyn_PointerType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_PointerType_struct*)
_temp311)->f1; goto _LL316;} else{ goto _LL317;} _LL317: goto _LL318; _LL314:
goto _LL312; _LL316: goto _LL312; _LL318:({ struct Cyc_Std_String_pa_struct
_temp322; _temp322.tag= Cyc_Std_String_pa; _temp322.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp321[ 1u]={& _temp322}; Cyc_Tcutil_terr( loc, _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u), _tag_arr( _temp321, sizeof( void*), 1u));}}); goto
_LL312; _LL312:;} return Cyc_Absyn_uint_t; _LL304: return({ void* _temp323[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof( unsigned char), 17u),
_tag_arr( _temp323, sizeof( void*), 0u));}); _LL292:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ struct Cyc_Std_String_pa_struct
_temp325; _temp325.tag= Cyc_Std_String_pa; _temp325.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp324[ 1u]={&
_temp325}; Cyc_Tcutil_terr( e1->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp324, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp326=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp326->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp326;}));} if( ! checker( e2)){({ struct Cyc_Std_String_pa_struct
_temp328; _temp328.tag= Cyc_Std_String_pa; _temp328.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp327[ 1u]={&
_temp328}; Cyc_Tcutil_terr( e2->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp327, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp329=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp329->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp329;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp330= t1; struct
Cyc_Absyn_PtrInfo _temp336; struct Cyc_Absyn_Conref* _temp338; struct Cyc_Absyn_Tqual
_temp340; struct Cyc_Absyn_Conref* _temp342; void* _temp344; void* _temp346;
_LL332: if(( unsigned int) _temp330 >  4u?*(( int*) _temp330) ==  Cyc_Absyn_PointerType:
0){ _LL337: _temp336=(( struct Cyc_Absyn_PointerType_struct*) _temp330)->f1;
_LL347: _temp346=( void*) _temp336.elt_typ; goto _LL345; _LL345: _temp344=( void*)
_temp336.rgn_typ; goto _LL343; _LL343: _temp342= _temp336.nullable; goto _LL341;
_LL341: _temp340= _temp336.tq; goto _LL339; _LL339: _temp338= _temp336.bounds;
goto _LL333;} else{ goto _LL334;} _LL334: goto _LL335; _LL333: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){({ struct Cyc_Std_String_pa_struct _temp349; _temp349.tag= Cyc_Std_String_pa;
_temp349.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp348[
1u]={& _temp349}; Cyc_Tcutil_terr( e2->loc, _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u), _tag_arr( _temp348, sizeof( void*), 1u));}});}
_temp338= Cyc_Absyn_compress_conref( _temp338);{ void* _temp350=( void*)
_temp338->v; void* _temp358; void* _temp360; struct Cyc_Absyn_Exp* _temp362;
_LL352: if(( unsigned int) _temp350 >  1u?*(( int*) _temp350) ==  Cyc_Absyn_Eq_constr:
0){ _LL359: _temp358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp350)->f1;
if( _temp358 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL353;} else{ goto _LL354;}}
else{ goto _LL354;} _LL354: if(( unsigned int) _temp350 >  1u?*(( int*) _temp350)
==  Cyc_Absyn_Eq_constr: 0){ _LL361: _temp360=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp350)->f1; if(( unsigned int) _temp360 >  1u?*(( int*) _temp360) ==  Cyc_Absyn_Upper_b:
0){ _LL363: _temp362=(( struct Cyc_Absyn_Upper_b_struct*) _temp360)->f1; goto
_LL355;} else{ goto _LL356;}} else{ goto _LL356;} _LL356: goto _LL357; _LL353:
return t1; _LL355: { struct Cyc_Absyn_PointerType_struct* _temp364=({ struct Cyc_Absyn_PointerType_struct*
_temp365=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp365[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp366; _temp366.tag= Cyc_Absyn_PointerType;
_temp366.f1=({ struct Cyc_Absyn_PtrInfo _temp367; _temp367.elt_typ=( void*)
_temp346; _temp367.rgn_typ=( void*) _temp344; _temp367.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp367.tq= _temp340; _temp367.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp367;}); _temp366;}); _temp365;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp364); return( void*) _temp364;} _LL357:( void*)( _temp338->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp368=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp368[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp369; _temp369.tag= Cyc_Absyn_Eq_constr; _temp369.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp369;}); _temp368;}))); return t1;
_LL351:;} _LL335: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL331:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){({ struct Cyc_Std_String_pa_struct
_temp372; _temp372.tag= Cyc_Std_String_pa; _temp372.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp371; _temp371.tag= Cyc_Std_String_pa; _temp371.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp370[ 2u]={&
_temp371,& _temp372}; Cyc_Tcutil_terr( e1->loc, _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u), _tag_arr( _temp370, sizeof( void*), 2u));}}});}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({
struct Cyc_Std_String_pa_struct _temp375; _temp375.tag= Cyc_Std_String_pa;
_temp375.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp374; _temp374.tag= Cyc_Std_String_pa; _temp374.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp373[ 2u]={& _temp374,& _temp375}; Cyc_Tcutil_terr( e2->loc,
_tag_arr("expecting either %s or int but found %s", sizeof( unsigned char), 40u),
_tag_arr( _temp373, sizeof( void*), 2u));}}});} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp376=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp376->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp376;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp378=({ struct _tuple0 _temp377;
_temp377.f1= Cyc_Tcutil_compress( t1); _temp377.f2= Cyc_Tcutil_compress( t2);
_temp377;}); void* _temp384; struct Cyc_Absyn_PtrInfo _temp386; void* _temp388;
void* _temp390; struct Cyc_Absyn_PtrInfo _temp392; void* _temp394; _LL380:
_LL391: _temp390= _temp378.f1; if(( unsigned int) _temp390 >  4u?*(( int*)
_temp390) ==  Cyc_Absyn_PointerType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_PointerType_struct*)
_temp390)->f1; _LL395: _temp394=( void*) _temp392.elt_typ; goto _LL385;} else{
goto _LL382;} _LL385: _temp384= _temp378.f2; if(( unsigned int) _temp384 >  4u?*((
int*) _temp384) ==  Cyc_Absyn_PointerType: 0){ _LL387: _temp386=(( struct Cyc_Absyn_PointerType_struct*)
_temp384)->f1; _LL389: _temp388=( void*) _temp386.elt_typ; goto _LL381;} else{
goto _LL382;} _LL382: goto _LL383; _LL381: if( Cyc_Tcutil_unify( _temp394,
_temp388)){ return Cyc_Absyn_sint_t;} goto _LL379; _LL383: goto _LL379; _LL379:;}({
struct Cyc_Std_String_pa_struct _temp398; _temp398.tag= Cyc_Std_String_pa;
_temp398.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp397; _temp397.tag= Cyc_Std_String_pa; _temp397.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp396[ 2u]={& _temp397,& _temp398}; Cyc_Tcutil_terr( loc,
_tag_arr("comparison not allowed between %s and %s", sizeof( unsigned char), 41u),
_tag_arr( _temp396, sizeof( void*), 2u));}}}); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp399->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp399;}));}}}}}
static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp400= p; _LL402: if( _temp400 == ( void*) Cyc_Absyn_Plus){ goto _LL403;}
else{ goto _LL404;} _LL404: if( _temp400 == ( void*) Cyc_Absyn_Minus){ goto
_LL405;} else{ goto _LL406;} _LL406: if( _temp400 == ( void*) Cyc_Absyn_Times){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp400 == ( void*) Cyc_Absyn_Div){
goto _LL409;} else{ goto _LL410;} _LL410: if( _temp400 == ( void*) Cyc_Absyn_Mod){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp400 == ( void*) Cyc_Absyn_Bitand){
goto _LL413;} else{ goto _LL414;} _LL414: if( _temp400 == ( void*) Cyc_Absyn_Bitor){
goto _LL415;} else{ goto _LL416;} _LL416: if( _temp400 == ( void*) Cyc_Absyn_Bitxor){
goto _LL417;} else{ goto _LL418;} _LL418: if( _temp400 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL419;} else{ goto _LL420;} _LL420: if( _temp400 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL421;} else{ goto _LL422;} _LL422: if( _temp400 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL423;} else{ goto _LL424;} _LL424: if( _temp400 == ( void*) Cyc_Absyn_Eq){
goto _LL425;} else{ goto _LL426;} _LL426: if( _temp400 == ( void*) Cyc_Absyn_Neq){
goto _LL427;} else{ goto _LL428;} _LL428: if( _temp400 == ( void*) Cyc_Absyn_Gt){
goto _LL429;} else{ goto _LL430;} _LL430: if( _temp400 == ( void*) Cyc_Absyn_Lt){
goto _LL431;} else{ goto _LL432;} _LL432: if( _temp400 == ( void*) Cyc_Absyn_Gte){
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp400 == ( void*) Cyc_Absyn_Lte){
goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL403: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL405: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL407: goto _LL409;
_LL409: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL411: goto _LL413; _LL413: goto _LL415; _LL415: goto _LL417; _LL417: goto
_LL419; _LL419: goto _LL421; _LL421: goto _LL423; _LL423: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL425: goto _LL427; _LL427: goto _LL429;
_LL429: goto _LL431; _LL431: goto _LL433; _LL433: goto _LL435; _LL435: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL437:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp438=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp438[ 0]=({ struct Cyc_Core_Failure_struct
_temp439; _temp439.tag= Cyc_Core_Failure; _temp439.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp439;}); _temp438;})); _LL401:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p == ( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) ==  1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL440: return({ void* _temp442[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("primitive operator has 0 arguments", sizeof( unsigned char),
35u), _tag_arr( _temp442, sizeof( void*), 0u));}); case 1: _LL441: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL443: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL444: return({ void* _temp446[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u), _tag_arr( _temp446, sizeof( void*), 0u));});}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp447=( void*) e->r; void* _temp471; struct Cyc_Absyn_Vardecl* _temp473;
void* _temp475; struct Cyc_Absyn_Vardecl* _temp477; void* _temp479; struct Cyc_Absyn_Vardecl*
_temp481; void* _temp483; struct Cyc_Absyn_Vardecl* _temp485; struct Cyc_Absyn_Exp*
_temp487; struct Cyc_Absyn_Exp* _temp489; struct _tagged_arr* _temp491; struct
Cyc_Absyn_Exp* _temp493; struct _tagged_arr* _temp495; struct Cyc_Absyn_Exp*
_temp497; struct Cyc_Absyn_Exp* _temp499; struct Cyc_Absyn_Exp* _temp501; struct
Cyc_Absyn_Exp* _temp503; _LL449: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL472: _temp471=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp471 >  1u?*(( int*) _temp471) ==  Cyc_Absyn_Param_b: 0){
_LL474: _temp473=(( struct Cyc_Absyn_Param_b_struct*) _temp471)->f1; goto _LL450;}
else{ goto _LL451;}} else{ goto _LL451;} _LL451: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL476: _temp475=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp475 >  1u?*(( int*) _temp475) ==  Cyc_Absyn_Local_b: 0){
_LL478: _temp477=(( struct Cyc_Absyn_Local_b_struct*) _temp475)->f1; goto _LL452;}
else{ goto _LL453;}} else{ goto _LL453;} _LL453: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL480: _temp479=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp479 >  1u?*(( int*) _temp479) ==  Cyc_Absyn_Pat_b: 0){ _LL482:
_temp481=(( struct Cyc_Absyn_Pat_b_struct*) _temp479)->f1; goto _LL454;} else{
goto _LL455;}} else{ goto _LL455;} _LL455: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL484: _temp483=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp483 >  1u?*(( int*) _temp483) ==  Cyc_Absyn_Global_b: 0){
_LL486: _temp485=(( struct Cyc_Absyn_Global_b_struct*) _temp483)->f1; goto
_LL456;} else{ goto _LL457;}} else{ goto _LL457;} _LL457: if(*(( int*) _temp447)
==  Cyc_Absyn_Subscript_e){ _LL490: _temp489=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f1; goto _LL488; _LL488: _temp487=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp447)
==  Cyc_Absyn_StructMember_e){ _LL494: _temp493=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f1; goto _LL492; _LL492: _temp491=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp447)
==  Cyc_Absyn_StructArrow_e){ _LL498: _temp497=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f1; goto _LL496; _LL496: _temp495=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f2; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp447)
==  Cyc_Absyn_Deref_e){ _LL500: _temp499=(( struct Cyc_Absyn_Deref_e_struct*)
_temp447)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp447)
==  Cyc_Absyn_NoInstantiate_e){ _LL502: _temp501=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp447)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp447)
==  Cyc_Absyn_Instantiate_e){ _LL504: _temp503=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp447)->f1; goto _LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL450:
_temp477= _temp473; goto _LL452; _LL452: _temp481= _temp477; goto _LL454; _LL454:
_temp485= _temp481; goto _LL456; _LL456: if( !( _temp485->tq).q_const){ return;}
goto _LL448; _LL458:{ void* _temp505= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp489->topt))->v); struct Cyc_Absyn_PtrInfo _temp515; struct Cyc_Absyn_Tqual
_temp517; struct Cyc_Absyn_Tqual _temp519; struct Cyc_List_List* _temp521;
_LL507: if(( unsigned int) _temp505 >  4u?*(( int*) _temp505) ==  Cyc_Absyn_PointerType:
0){ _LL516: _temp515=(( struct Cyc_Absyn_PointerType_struct*) _temp505)->f1;
_LL518: _temp517= _temp515.tq; goto _LL508;} else{ goto _LL509;} _LL509: if((
unsigned int) _temp505 >  4u?*(( int*) _temp505) ==  Cyc_Absyn_ArrayType: 0){
_LL520: _temp519=(( struct Cyc_Absyn_ArrayType_struct*) _temp505)->f2; goto
_LL510;} else{ goto _LL511;} _LL511: if(( unsigned int) _temp505 >  4u?*(( int*)
_temp505) ==  Cyc_Absyn_TupleType: 0){ _LL522: _temp521=(( struct Cyc_Absyn_TupleType_struct*)
_temp505)->f1; goto _LL512;} else{ goto _LL513;} _LL513: goto _LL514; _LL508:
_temp519= _temp517; goto _LL510; _LL510: if( ! _temp519.q_const){ return;} goto
_LL506; _LL512: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp487);{
struct _handler_cons _temp523; _push_handler(& _temp523);{ int _temp525= 0; if(
setjmp( _temp523.handler)){ _temp525= 1;} if( ! _temp525){{ struct _tuple8
_temp528; struct Cyc_Absyn_Tqual _temp529; struct _tuple8* _temp526=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp521,( int) i);
_temp528=* _temp526; _LL530: _temp529= _temp528.f1; goto _LL527; _LL527: if( !
_temp529.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp524=( void*) _exn_thrown; void* _temp532= _temp524; _LL534: if( _temp532 == 
Cyc_List_Nth){ goto _LL535;} else{ goto _LL536;} _LL536: goto _LL537; _LL535:
return; _LL537:( void) _throw( _temp532); _LL533:;}}} goto _LL506;} _LL514: goto
_LL506; _LL506:;} goto _LL448; _LL460:{ void* _temp538= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp493->topt))->v); struct Cyc_Absyn_Structdecl**
_temp550; struct Cyc_List_List* _temp552; struct Cyc_List_List* _temp554; struct
Cyc_Absyn_Uniondecl** _temp556; _LL540: if(( unsigned int) _temp538 >  4u?*((
int*) _temp538) ==  Cyc_Absyn_StructType: 0){ _LL551: _temp550=(( struct Cyc_Absyn_StructType_struct*)
_temp538)->f3; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp538 >  4u?*(( int*) _temp538) ==  Cyc_Absyn_AnonUnionType: 0){ _LL553:
_temp552=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp538)->f1; goto _LL543;}
else{ goto _LL544;} _LL544: if(( unsigned int) _temp538 >  4u?*(( int*) _temp538)
==  Cyc_Absyn_AnonStructType: 0){ _LL555: _temp554=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp538)->f1; goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp538 >  4u?*(( int*) _temp538) ==  Cyc_Absyn_UnionType: 0){ _LL557: _temp556=((
struct Cyc_Absyn_UnionType_struct*) _temp538)->f3; goto _LL547;} else{ goto
_LL548;} _LL548: goto _LL549; _LL541: { struct Cyc_Absyn_Structfield* sf=
_temp550 ==  0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp550)), _temp491); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL539;} _LL543: _temp554= _temp552; goto _LL545; _LL545: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp554, _temp491); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL539;}
_LL547: { struct Cyc_Absyn_Structfield* sf= _temp556 ==  0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)*(( struct Cyc_Absyn_Uniondecl**) _check_null(
_temp556)), _temp491); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL539;} _LL549: goto _LL539;
_LL539:;} goto _LL448; _LL462:{ void* _temp558= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp497->topt))->v); struct Cyc_Absyn_PtrInfo
_temp564; void* _temp566; _LL560: if(( unsigned int) _temp558 >  4u?*(( int*)
_temp558) ==  Cyc_Absyn_PointerType: 0){ _LL565: _temp564=(( struct Cyc_Absyn_PointerType_struct*)
_temp558)->f1; _LL567: _temp566=( void*) _temp564.elt_typ; goto _LL561;} else{
goto _LL562;} _LL562: goto _LL563; _LL561:{ void* _temp568= Cyc_Tcutil_compress(
_temp566); struct Cyc_Absyn_Structdecl** _temp580; struct Cyc_Absyn_Uniondecl**
_temp582; struct Cyc_List_List* _temp584; struct Cyc_List_List* _temp586; _LL570:
if(( unsigned int) _temp568 >  4u?*(( int*) _temp568) ==  Cyc_Absyn_StructType:
0){ _LL581: _temp580=(( struct Cyc_Absyn_StructType_struct*) _temp568)->f3; goto
_LL571;} else{ goto _LL572;} _LL572: if(( unsigned int) _temp568 >  4u?*(( int*)
_temp568) ==  Cyc_Absyn_UnionType: 0){ _LL583: _temp582=(( struct Cyc_Absyn_UnionType_struct*)
_temp568)->f3; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp568 >  4u?*(( int*) _temp568) ==  Cyc_Absyn_AnonStructType: 0){ _LL585:
_temp584=(( struct Cyc_Absyn_AnonStructType_struct*) _temp568)->f1; goto _LL575;}
else{ goto _LL576;} _LL576: if(( unsigned int) _temp568 >  4u?*(( int*) _temp568)
==  Cyc_Absyn_AnonUnionType: 0){ _LL587: _temp586=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp568)->f1; goto _LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL571: {
struct Cyc_Absyn_Structfield* sf= _temp580 ==  0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp580)), _temp495); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL569;} _LL573: { struct Cyc_Absyn_Structfield*
sf= _temp582 ==  0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp582)), _temp495); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL569;} _LL575: _temp586= _temp584; goto _LL577; _LL577: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp586, _temp495); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL569;}
_LL579: goto _LL569; _LL569:;} goto _LL559; _LL563: goto _LL559; _LL559:;} goto
_LL448; _LL464:{ void* _temp588= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp499->topt))->v); struct Cyc_Absyn_PtrInfo _temp596; struct Cyc_Absyn_Tqual
_temp598; struct Cyc_Absyn_Tqual _temp600; _LL590: if(( unsigned int) _temp588 > 
4u?*(( int*) _temp588) ==  Cyc_Absyn_PointerType: 0){ _LL597: _temp596=(( struct
Cyc_Absyn_PointerType_struct*) _temp588)->f1; _LL599: _temp598= _temp596.tq;
goto _LL591;} else{ goto _LL592;} _LL592: if(( unsigned int) _temp588 >  4u?*((
int*) _temp588) ==  Cyc_Absyn_ArrayType: 0){ _LL601: _temp600=(( struct Cyc_Absyn_ArrayType_struct*)
_temp588)->f2; goto _LL593;} else{ goto _LL594;} _LL594: goto _LL595; _LL591:
_temp600= _temp598; goto _LL593; _LL593: if( ! _temp600.q_const){ return;} goto
_LL589; _LL595: goto _LL589; _LL589:;} goto _LL448; _LL466: _temp503= _temp501;
goto _LL468; _LL468: Cyc_Tcexp_check_writable( te, _temp503); return; _LL470:
goto _LL448; _LL448:;}({ void* _temp602[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("attempt to write a read-only location", sizeof( unsigned char), 38u),
_tag_arr( _temp602, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return({ void* _temp603[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u), _tag_arr( _temp603, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if(
! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){({
struct Cyc_Std_String_pa_struct _temp605; _temp605.tag= Cyc_Std_String_pa;
_temp605.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp604[ 1u]={&
_temp605}; Cyc_Tcutil_terr( e->loc, _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u), _tag_arr( _temp604, sizeof( void*), 1u));}});}
return t;}} static void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest( te, e1, _tag_arr("conditional expression",
sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp( te, topt, e2); Cyc_Tcexp_tcExp(
te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({
struct Cyc_Core_Opt* _temp613=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp613->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp613;}));
struct Cyc_List_List* _temp606=({ struct Cyc_List_List* _temp612=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp612->hd=( void*) e3; _temp612->tl=
0; _temp612;}); struct Cyc_List_List* _temp607=({ struct Cyc_List_List* _temp611=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp611->hd=(
void*) e2; _temp611->tl= _temp606; _temp611;}); if( ! Cyc_Tcutil_coerce_list( te,
t, _temp607)){({ struct Cyc_Std_String_pa_struct _temp610; _temp610.tag= Cyc_Std_String_pa;
_temp610.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ struct Cyc_Std_String_pa_struct _temp609; _temp609.tag=
Cyc_Std_String_pa; _temp609.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp608[ 2u]={&
_temp609,& _temp610}; Cyc_Tcutil_terr( loc, _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u), _tag_arr( _temp608, sizeof( void*), 2u));}}});}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp614=
Cyc_Tcutil_compress( t1); _LL616: if(( unsigned int) _temp614 >  4u?*(( int*)
_temp614) ==  Cyc_Absyn_ArrayType: 0){ goto _LL617;} else{ goto _LL618;} _LL618:
goto _LL619; _LL617:({ void* _temp620[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp620, sizeof( void*), 0u));}); goto
_LL615; _LL619: goto _LL615; _LL615:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({
void* _temp621[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp621, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po ==  0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return({
struct Cyc_Std_String_pa_struct _temp624; _temp624.tag= Cyc_Std_String_pa;
_temp624.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp623; _temp623.tag= Cyc_Std_String_pa; _temp623.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp622[ 2u]={& _temp623,& _temp624}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u), _tag_arr(
_temp622, sizeof( void*), 2u));}}});}} else{ void* _temp625=( void*)(( struct
Cyc_Core_Opt*) _check_null( po))->v; void* _temp626= Cyc_Tcexp_tcBinPrimop( te,
loc, 0, _temp625, e1, e2); if( !( Cyc_Tcutil_unify( _temp626, t1)? 1:( Cyc_Tcutil_coerceable(
_temp626)? Cyc_Tcutil_coerceable( t1): 0))){ return({ struct Cyc_Std_String_pa_struct
_temp629; _temp629.tag= Cyc_Std_String_pa; _temp629.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp628; _temp628.tag= Cyc_Std_String_pa;
_temp628.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp627[
2u]={& _temp628,& _temp629}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof( unsigned char), 82u), _tag_arr( _temp627, sizeof( void*), 2u));}}});}
return _temp626;} return t1;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;{ void* _temp630= Cyc_Tcutil_compress(
t1); void* _temp640; void* _temp642; _LL632: if( _temp630 == ( void*) Cyc_Absyn_FloatType){
goto _LL633;} else{ goto _LL634;} _LL634: if(( unsigned int) _temp630 >  4u?*((
int*) _temp630) ==  Cyc_Absyn_IntType: 0){ _LL641: _temp640=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp630)->f2; if( _temp640 == ( void*) Cyc_Absyn_B1){
goto _LL635;} else{ goto _LL636;}} else{ goto _LL636;} _LL636: if(( unsigned int)
_temp630 >  4u?*(( int*) _temp630) ==  Cyc_Absyn_IntType: 0){ _LL643: _temp642=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp630)->f2; if( _temp642 == ( void*)
Cyc_Absyn_B2){ goto _LL637;} else{ goto _LL638;}} else{ goto _LL638;} _LL638:
goto _LL639; _LL633: Cyc_Tcutil_unchecked_cast( te, e,( void*) Cyc_Absyn_DoubleType);
t1=( void*) Cyc_Absyn_DoubleType; goto _LL631; _LL635: goto _LL637; _LL637: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); t1= Cyc_Absyn_sint_t; goto _LL631; _LL639: goto _LL631;
_LL631:;} for( fields= fs; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ void* _temp646; struct Cyc_Position_Segment*
_temp648; struct Cyc_List_List* _temp650; struct Cyc_List_List* _temp652; struct
_tuple1* _temp654; struct Cyc_Absyn_Tunionfield _temp644=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd); _LL655: _temp654= _temp644.name;
goto _LL653; _LL653: _temp652= _temp644.tvs; goto _LL651; _LL651: _temp650=
_temp644.typs; goto _LL649; _LL649: _temp648= _temp644.loc; goto _LL647; _LL647:
_temp646=( void*) _temp644.sc; goto _LL645; _LL645: if(( _temp650 ==  0? 1:((
struct Cyc_List_List*) _check_null( _temp650))->tl !=  0)? 1: _temp652 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( _temp650))->hd)).f2); if( Cyc_Tcutil_unify(
t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} for( fields= fs; fields != 
0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp658;
struct Cyc_Position_Segment* _temp660; struct Cyc_List_List* _temp662; struct
Cyc_List_List* _temp664; struct _tuple1* _temp666; struct Cyc_Absyn_Tunionfield
_temp656=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL667: _temp666= _temp656.name; goto _LL665; _LL665: _temp664=
_temp656.tvs; goto _LL663; _LL663: _temp662= _temp656.typs; goto _LL661; _LL661:
_temp660= _temp656.loc; goto _LL659; _LL659: _temp658=( void*) _temp656.sc; goto
_LL657; _LL657: if(( _temp662 ==  0? 1:(( struct Cyc_List_List*) _check_null(
_temp662))->tl !=  0)? 1: _temp664 !=  0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp662))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}({
struct Cyc_Std_String_pa_struct _temp670; _temp670.tag= Cyc_Std_String_pa;
_temp670.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ struct Cyc_Std_String_pa_struct
_temp669; _temp669.tag= Cyc_Std_String_pa; _temp669.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
tu);{ void* _temp668[ 2u]={& _temp669,& _temp670}; Cyc_Tcutil_terr( e->loc,
_tag_arr("can't find a field in %s to inject a value of type %s", sizeof(
unsigned char), 54u), _tag_arr( _temp668, sizeof( void*), 2u));}}}); return 0;}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* args, struct
Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List* _temp671=
args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp672= t; struct Cyc_Absyn_PtrInfo _temp678; struct Cyc_Absyn_Conref*
_temp680; struct Cyc_Absyn_Tqual _temp682; struct Cyc_Absyn_Conref* _temp684;
void* _temp686; void* _temp688; _LL674: if(( unsigned int) _temp672 >  4u?*((
int*) _temp672) ==  Cyc_Absyn_PointerType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_PointerType_struct*)
_temp672)->f1; _LL689: _temp688=( void*) _temp678.elt_typ; goto _LL687; _LL687:
_temp686=( void*) _temp678.rgn_typ; goto _LL685; _LL685: _temp684= _temp678.nullable;
goto _LL683; _LL683: _temp682= _temp678.tq; goto _LL681; _LL681: _temp680=
_temp678.bounds; goto _LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL675:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp686); Cyc_Tcutil_check_nonzero_bound(
loc, _temp680);{ void* _temp690= Cyc_Tcutil_compress( _temp688); struct Cyc_Absyn_FnInfo
_temp696; struct Cyc_List_List* _temp698; struct Cyc_List_List* _temp700; struct
Cyc_Absyn_VarargInfo* _temp702; int _temp704; struct Cyc_List_List* _temp706;
void* _temp708; struct Cyc_Core_Opt* _temp710; struct Cyc_List_List* _temp712;
_LL692: if(( unsigned int) _temp690 >  4u?*(( int*) _temp690) ==  Cyc_Absyn_FnType:
0){ _LL697: _temp696=(( struct Cyc_Absyn_FnType_struct*) _temp690)->f1; _LL713:
_temp712= _temp696.tvars; goto _LL711; _LL711: _temp710= _temp696.effect; goto
_LL709; _LL709: _temp708=( void*) _temp696.ret_typ; goto _LL707; _LL707:
_temp706= _temp696.args; goto _LL705; _LL705: _temp704= _temp696.c_varargs; goto
_LL703; _LL703: _temp702= _temp696.cyc_varargs; goto _LL701; _LL701: _temp700=
_temp696.rgn_po; goto _LL699; _LL699: _temp698= _temp696.attributes; goto _LL693;}
else{ goto _LL694;} _LL694: goto _LL695; _LL693: if( topt !=  0){ Cyc_Tcutil_unify(
_temp708,*(( void**) _check_null( topt)));} while( _temp671 !=  0? _temp706 != 
0: 0) { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp671))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp706))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){({ struct Cyc_Std_String_pa_struct _temp716;
_temp716.tag= Cyc_Std_String_pa; _temp716.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp715; _temp715.tag= Cyc_Std_String_pa;
_temp715.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v);{ void* _temp714[ 2u]={& _temp715,& _temp716}; Cyc_Tcutil_terr(
loc, _tag_arr("actual argument has type %s but formal has type %s", sizeof(
unsigned char), 51u), _tag_arr( _temp714, sizeof( void*), 2u));}}});} _temp671=((
struct Cyc_List_List*) _check_null( _temp671))->tl; _temp706=(( struct Cyc_List_List*)
_check_null( _temp706))->tl;} if( _temp706 !=  0){({ void* _temp717[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments for function", sizeof( unsigned char), 31u),
_tag_arr( _temp717, sizeof( void*), 0u));});} else{ if(( _temp671 !=  0? 1:
_temp704)? 1: _temp702 !=  0){ if( _temp704){ for( 0; _temp671 !=  0; _temp671=((
struct Cyc_List_List*) _check_null( _temp671))->tl){ Cyc_Tcexp_tcExp( te, 0,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp671))->hd);}}
else{ if( _temp702 ==  0){({ void* _temp718[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too many arguments for function", sizeof( unsigned char), 32u),
_tag_arr( _temp718, sizeof( void*), 0u));});} else{ int _temp721; void* _temp723;
struct Cyc_Absyn_VarargInfo _temp719=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp702)); _LL724: _temp723=( void*) _temp719.type; goto _LL722;
_LL722: _temp721= _temp719.inject; goto _LL720; _LL720: { struct Cyc_Absyn_VarargCallInfo*
_temp725=({ struct Cyc_Absyn_VarargCallInfo* _temp755=( struct Cyc_Absyn_VarargCallInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargCallInfo)); _temp755->num_varargs= 0;
_temp755->injectors= 0; _temp755->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp702); _temp755;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp725; if( ! _temp721){ for( 0; _temp671 !=  0; _temp671=(( struct Cyc_List_List*)
_check_null( _temp671))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp671))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp725))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp723,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp723)){({ struct Cyc_Std_String_pa_struct
_temp728; _temp728.tag= Cyc_Std_String_pa; _temp728.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp727; _temp727.tag= Cyc_Std_String_pa; _temp727.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp723);{ void* _temp726[ 2u]={& _temp727,& _temp728}; Cyc_Tcutil_terr( loc,
_tag_arr("vararg requires type %s but argument has type %s", sizeof(
unsigned char), 49u), _tag_arr( _temp726, sizeof( void*), 2u));}}});}}} else{
void* _temp729= Cyc_Tcutil_compress( _temp723); struct Cyc_Absyn_TunionInfo
_temp735; void* _temp737; struct Cyc_List_List* _temp739; void* _temp741; struct
Cyc_Absyn_Tuniondecl** _temp743; struct Cyc_Absyn_Tuniondecl* _temp745; _LL731:
if(( unsigned int) _temp729 >  4u?*(( int*) _temp729) ==  Cyc_Absyn_TunionType:
0){ _LL736: _temp735=(( struct Cyc_Absyn_TunionType_struct*) _temp729)->f1;
_LL742: _temp741=( void*) _temp735.tunion_info; if(*(( int*) _temp741) ==  Cyc_Absyn_KnownTunion){
_LL744: _temp743=(( struct Cyc_Absyn_KnownTunion_struct*) _temp741)->f1;
_temp745=* _temp743; goto _LL740;} else{ goto _LL733;} _LL740: _temp739=
_temp735.targs; goto _LL738; _LL738: _temp737=( void*) _temp735.rgn; goto _LL732;}
else{ goto _LL733;} _LL733: goto _LL734; _LL732: { struct Cyc_Absyn_Tuniondecl*
_temp746=* Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp745->name); struct Cyc_List_List*
fields= 0; if( _temp746->fields ==  0){({ struct Cyc_Std_String_pa_struct
_temp748; _temp748.tag= Cyc_Std_String_pa; _temp748.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp723);{ void* _temp747[ 1u]={& _temp748}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u), _tag_arr( _temp747, sizeof( void*), 1u));}});}
else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp746->fields))->v;} if( ! Cyc_Tcutil_unify( _temp737, Cyc_Tcenv_curr_rgn( te))){({
void* _temp749[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp749, sizeof( void*), 0u));});}{
struct _RegionHandle _temp750= _new_region(); struct _RegionHandle* rgn=&
_temp750; _push_region( rgn);{ struct Cyc_List_List* _temp751=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp746->tvs, _temp739);
for( 0; _temp671 !=  0; _temp671=(( struct Cyc_List_List*) _check_null( _temp671))->tl){((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp725))->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp671))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{ struct Cyc_Absyn_Tunionfield*
_temp752= Cyc_Tcexp_tcInjection( te, e1, _temp723, rgn, _temp751, fields); if(
_temp752 !=  0){(( struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp725))->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp725))->injectors,({ struct
Cyc_List_List* _temp753=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp753->hd=( void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp752));
_temp753->tl= 0; _temp753;}));}}}}}; _pop_region( rgn);} goto _LL730;} _LL734:({
void* _temp754[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type",
sizeof( unsigned char), 23u), _tag_arr( _temp754, sizeof( void*), 0u));}); goto
_LL730; _LL730:;}}}}}}{ struct Cyc_List_List* a= _temp698; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ void* _temp756=( void*)(( struct
Cyc_List_List*) _check_null( a))->hd; int _temp762; int _temp764; void* _temp766;
_LL758: if(( unsigned int) _temp756 >  16u?*(( int*) _temp756) ==  Cyc_Absyn_Format_att:
0){ _LL767: _temp766=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp756)->f1;
goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_Format_att_struct*) _temp756)->f2;
goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_Format_att_struct*) _temp756)->f3;
goto _LL759;} else{ goto _LL760;} _LL760: goto _LL761; _LL759:{ struct
_handler_cons _temp768; _push_handler(& _temp768);{ int _temp770= 0; if( setjmp(
_temp768.handler)){ _temp770= 1;} if( ! _temp770){{ struct Cyc_Absyn_Exp*
_temp771=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)(
args, _temp764 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp762 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp772=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp772->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp762 -  1);
_temp772;});}{ void* _temp773= _temp766; _LL775: if( _temp773 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL776;} else{ goto _LL777;} _LL777: if( _temp773 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL778;} else{ goto _LL774;} _LL776: Cyc_Tcexp_check_format_args( te,
_temp771, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL774; _LL778: Cyc_Tcexp_check_format_args(
te, _temp771, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL774; _LL774:;}};
_pop_handler();} else{ void* _temp769=( void*) _exn_thrown; void* _temp780=
_temp769; _LL782: if( _temp780 ==  Cyc_List_Nth){ goto _LL783;} else{ goto
_LL784;} _LL784: goto _LL785; _LL783:({ void* _temp786[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp786, sizeof( void*), 0u));}); goto _LL781; _LL785:( void) _throw( _temp780);
_LL781:;}}} goto _LL757; _LL761: goto _LL757; _LL757:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp710))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp700); return _temp708; _LL695: return({ void* _temp787[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp787, sizeof( void*), 0u));}); _LL691:;}
_LL677: return({ void* _temp788[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp788, sizeof( void*), 0u));});
_LL673:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_arg( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp790; _temp790.tag= Cyc_Std_String_pa; _temp790.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp789[ 1u]={&
_temp790}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp789, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp791=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp791->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp791;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp792= t1;
struct Cyc_Absyn_PtrInfo _temp798; struct Cyc_Absyn_Conref* _temp800; struct Cyc_Absyn_Tqual
_temp802; struct Cyc_Absyn_Conref* _temp804; void* _temp806; void* _temp808;
_LL794: if(( unsigned int) _temp792 >  4u?*(( int*) _temp792) ==  Cyc_Absyn_PointerType:
0){ _LL799: _temp798=(( struct Cyc_Absyn_PointerType_struct*) _temp792)->f1;
_LL809: _temp808=( void*) _temp798.elt_typ; goto _LL807; _LL807: _temp806=( void*)
_temp798.rgn_typ; goto _LL805; _LL805: _temp804= _temp798.nullable; goto _LL803;
_LL803: _temp802= _temp798.tq; goto _LL801; _LL801: _temp800= _temp798.bounds;
goto _LL795;} else{ goto _LL796;} _LL796: goto _LL797; _LL795:{ void* _temp810=
Cyc_Tcutil_compress( _temp808); struct Cyc_Absyn_FnInfo _temp816; struct Cyc_List_List*
_temp818; struct Cyc_List_List* _temp820; struct Cyc_Absyn_VarargInfo* _temp822;
int _temp824; struct Cyc_List_List* _temp826; void* _temp828; struct Cyc_Core_Opt*
_temp830; struct Cyc_List_List* _temp832; _LL812: if(( unsigned int) _temp810 > 
4u?*(( int*) _temp810) ==  Cyc_Absyn_FnType: 0){ _LL817: _temp816=(( struct Cyc_Absyn_FnType_struct*)
_temp810)->f1; _LL833: _temp832= _temp816.tvars; goto _LL831; _LL831: _temp830=
_temp816.effect; goto _LL829; _LL829: _temp828=( void*) _temp816.ret_typ; goto
_LL827; _LL827: _temp826= _temp816.args; goto _LL825; _LL825: _temp824= _temp816.c_varargs;
goto _LL823; _LL823: _temp822= _temp816.cyc_varargs; goto _LL821; _LL821:
_temp820= _temp816.rgn_po; goto _LL819; _LL819: _temp818= _temp816.attributes;
goto _LL813;} else{ goto _LL814;} _LL814: goto _LL815; _LL813: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp832)){ return({ void* _temp834[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp834, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts=(( struct
Cyc_List_List*) _check_null( ts))->tl, _temp832=(( struct Cyc_List_List*)
_check_null( _temp832))->tl)){ void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp832))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp835=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp835->hd=( void*)({
struct _tuple5* _temp836=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp836->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp832))->hd; _temp836->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp836;}); _temp835->tl= instantiation; _temp835;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp840=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp840[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp841; _temp841.tag= Cyc_Absyn_FnType; _temp841.f1=({ struct Cyc_Absyn_FnInfo
_temp842; _temp842.tvars= 0; _temp842.effect= _temp830; _temp842.ret_typ=( void*)
_temp828; _temp842.args= _temp826; _temp842.c_varargs= _temp824; _temp842.cyc_varargs=
_temp822; _temp842.rgn_po= _temp820; _temp842.attributes= _temp818; _temp842;});
_temp841;}); _temp840;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp837=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp837[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp838; _temp838.tag= Cyc_Absyn_PointerType;
_temp838.f1=({ struct Cyc_Absyn_PtrInfo _temp839; _temp839.elt_typ=( void*)
new_fn_typ; _temp839.rgn_typ=( void*) _temp806; _temp839.nullable= _temp804;
_temp839.tq= _temp802; _temp839.bounds= _temp800; _temp839;}); _temp838;});
_temp837;}); return new_typ;}} _LL815: goto _LL811; _LL811:;} goto _LL793;
_LL797: goto _LL793; _LL793:;} return({ struct Cyc_Std_String_pa_struct _temp844;
_temp844.tag= Cyc_Std_String_pa; _temp844.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp843[ 1u]={& _temp844}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u), _tag_arr( _temp843, sizeof( void*), 1u));}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return({ struct Cyc_Std_String_pa_struct _temp847; _temp847.tag=
Cyc_Std_String_pa; _temp847.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp846; _temp846.tag= Cyc_Std_String_pa;
_temp846.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp845[
2u]={& _temp846,& _temp847}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp845, sizeof( void*), 2u));}}});}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void**
_temp848= 0; struct Cyc_Absyn_Tqual _temp849= Cyc_Absyn_empty_tqual(); if( topt
!=  0){ void* _temp850= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp856; struct Cyc_Absyn_Tqual _temp858; void*
_temp860; _LL852: if(( unsigned int) _temp850 >  4u?*(( int*) _temp850) ==  Cyc_Absyn_PointerType:
0){ _LL857: _temp856=(( struct Cyc_Absyn_PointerType_struct*) _temp850)->f1;
_LL861: _temp860=( void*) _temp856.elt_typ; goto _LL859; _LL859: _temp858=
_temp856.tq; goto _LL853;} else{ goto _LL854;} _LL854: goto _LL855; _LL853:
_temp848=({ void** _temp862=( void**) _cycalloc( sizeof( void*)); _temp862[ 0]=
_temp860; _temp862;}); _temp849= _temp858; goto _LL851; _LL855: goto _LL851;
_LL851:;} Cyc_Tcexp_tcExpNoInst( te, _temp848, e);{ void* _temp863=( void*) e->r;
_LL865: if(*(( int*) _temp863) ==  Cyc_Absyn_Struct_e){ goto _LL866;} else{ goto
_LL867;} _LL867: if(*(( int*) _temp863) ==  Cyc_Absyn_Tuple_e){ goto _LL868;}
else{ goto _LL869;} _LL869: goto _LL870; _LL866: goto _LL868; _LL868:({ void*
_temp871[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate", sizeof(
unsigned char), 19u), _tag_arr( _temp871, sizeof( void*), 0u));});{ void*(*
_temp872)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt !=  0){ void* _temp873= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp879; struct Cyc_Absyn_Conref* _temp881; _LL875:
if(( unsigned int) _temp873 >  4u?*(( int*) _temp873) ==  Cyc_Absyn_PointerType:
0){ _LL880: _temp879=(( struct Cyc_Absyn_PointerType_struct*) _temp873)->f1;
_LL882: _temp881= _temp879.nullable; goto _LL876;} else{ goto _LL877;} _LL877:
goto _LL878; _LL876: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp881)){ _temp872= Cyc_Absyn_star_typ;} goto _LL874; _LL878: goto _LL874;
_LL874:;} return _temp872(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp849);} _LL870: goto _LL864; _LL864:;}{ void*
_temp885; int _temp887; struct _tuple6 _temp883= Cyc_Tcutil_addressof_props( te,
e); _LL888: _temp887= _temp883.f1; goto _LL886; _LL886: _temp885= _temp883.f2;
goto _LL884; _LL884: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp887){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp885, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Std_String_pa_struct
_temp890; _temp890.tag= Cyc_Std_String_pa; _temp890.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp889[ 1u]={& _temp890}; Cyc_Tcutil_terr( loc, _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u), _tag_arr( _temp889, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;} int Cyc_Tcexp_structfield_has_name( struct _tagged_arr*
n, struct Cyc_Absyn_Structfield* sf){ return Cyc_Std_strcmp(* n,* sf->name) == 
0;} static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, void* n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp891= n; struct _tagged_arr*
_temp897; unsigned int _temp899; _LL893: if(*(( int*) _temp891) ==  Cyc_Absyn_StructField){
_LL898: _temp897=(( struct Cyc_Absyn_StructField_struct*) _temp891)->f1; goto
_LL894;} else{ goto _LL895;} _LL895: if(*(( int*) _temp891) ==  Cyc_Absyn_TupleIndex){
_LL900: _temp899=(( struct Cyc_Absyn_TupleIndex_struct*) _temp891)->f1; goto
_LL896;} else{ goto _LL892;} _LL894: { int bad_type= 1;{ void* _temp901= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp913; struct _tuple1* _temp915; struct Cyc_List_List*
_temp917; struct Cyc_Absyn_Uniondecl** _temp919; struct _tuple1* _temp921;
struct Cyc_List_List* _temp923; _LL903: if(( unsigned int) _temp901 >  4u?*((
int*) _temp901) ==  Cyc_Absyn_StructType: 0){ _LL916: _temp915=(( struct Cyc_Absyn_StructType_struct*)
_temp901)->f1; goto _LL914; _LL914: _temp913=(( struct Cyc_Absyn_StructType_struct*)
_temp901)->f3; goto _LL904;} else{ goto _LL905;} _LL905: if(( unsigned int)
_temp901 >  4u?*(( int*) _temp901) ==  Cyc_Absyn_AnonStructType: 0){ _LL918:
_temp917=(( struct Cyc_Absyn_AnonStructType_struct*) _temp901)->f1; goto _LL906;}
else{ goto _LL907;} _LL907: if(( unsigned int) _temp901 >  4u?*(( int*) _temp901)
==  Cyc_Absyn_UnionType: 0){ _LL922: _temp921=(( struct Cyc_Absyn_UnionType_struct*)
_temp901)->f1; goto _LL920; _LL920: _temp919=(( struct Cyc_Absyn_UnionType_struct*)
_temp901)->f3; goto _LL908;} else{ goto _LL909;} _LL909: if(( unsigned int)
_temp901 >  4u?*(( int*) _temp901) ==  Cyc_Absyn_AnonUnionType: 0){ _LL924:
_temp923=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp901)->f1; goto _LL910;}
else{ goto _LL911;} _LL911: goto _LL912; _LL904: if( _temp913 ==  0){ return({
void* _temp925[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u), _tag_arr( _temp925, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp913)))->fields ==  0){ goto
_LL902;} if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp897,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp913)))->fields))->v)){({ struct
Cyc_Std_String_pa_struct _temp927; _temp927.tag= Cyc_Std_String_pa; _temp927.f1=(
struct _tagged_arr)* _temp897;{ void* _temp926[ 1u]={& _temp927}; Cyc_Tcutil_terr(
loc, _tag_arr("no field of struct has name %s", sizeof( unsigned char), 31u),
_tag_arr( _temp926, sizeof( void*), 1u));}});} bad_type= 0; goto _LL902; _LL906:
if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp897, _temp917)){({ struct Cyc_Std_String_pa_struct _temp929; _temp929.tag=
Cyc_Std_String_pa; _temp929.f1=( struct _tagged_arr)* _temp897;{ void* _temp928[
1u]={& _temp929}; Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s",
sizeof( unsigned char), 31u), _tag_arr( _temp928, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL902; _LL908: if( _temp919 ==  0){ return({ void* _temp930[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp930, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp919)))->fields ==  0){ goto
_LL902;} if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp897,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp919)))->fields))->v)){({ struct
Cyc_Std_String_pa_struct _temp932; _temp932.tag= Cyc_Std_String_pa; _temp932.f1=(
struct _tagged_arr)* _temp897;{ void* _temp931[ 1u]={& _temp932}; Cyc_Tcutil_terr(
loc, _tag_arr("no field of union has name %s", sizeof( unsigned char), 30u),
_tag_arr( _temp931, sizeof( void*), 1u));}});} bad_type= 0; goto _LL902; _LL910:
if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp897, _temp923)){({ struct Cyc_Std_String_pa_struct _temp934; _temp934.tag=
Cyc_Std_String_pa; _temp934.f1=( struct _tagged_arr)* _temp897;{ void* _temp933[
1u]={& _temp934}; Cyc_Tcutil_terr( loc, _tag_arr("no field of anonymous union has name %s",
sizeof( unsigned char), 40u), _tag_arr( _temp933, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL902; _LL912: goto _LL902; _LL902:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp936; _temp936.tag= Cyc_Std_String_pa; _temp936.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp935[ 1u]={& _temp936};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union type", sizeof(
unsigned char), 36u), _tag_arr( _temp935, sizeof( void*), 1u));}});} goto _LL892;}
_LL896: { int bad_type= 1;{ void* _temp937= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp953; struct Cyc_List_List* _temp955; struct Cyc_Absyn_Uniondecl** _temp957;
struct Cyc_List_List* _temp959; struct Cyc_List_List* _temp961; struct Cyc_Absyn_TunionFieldInfo
_temp963; void* _temp965; struct Cyc_Absyn_Tunionfield* _temp967; struct Cyc_Absyn_Tuniondecl*
_temp969; _LL939: if(( unsigned int) _temp937 >  4u?*(( int*) _temp937) ==  Cyc_Absyn_StructType:
0){ _LL954: _temp953=(( struct Cyc_Absyn_StructType_struct*) _temp937)->f3; goto
_LL940;} else{ goto _LL941;} _LL941: if(( unsigned int) _temp937 >  4u?*(( int*)
_temp937) ==  Cyc_Absyn_AnonStructType: 0){ _LL956: _temp955=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp937)->f1; goto _LL942;} else{ goto _LL943;} _LL943: if(( unsigned int)
_temp937 >  4u?*(( int*) _temp937) ==  Cyc_Absyn_UnionType: 0){ _LL958: _temp957=((
struct Cyc_Absyn_UnionType_struct*) _temp937)->f3; goto _LL944;} else{ goto
_LL945;} _LL945: if(( unsigned int) _temp937 >  4u?*(( int*) _temp937) ==  Cyc_Absyn_AnonUnionType:
0){ _LL960: _temp959=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp937)->f1;
goto _LL946;} else{ goto _LL947;} _LL947: if(( unsigned int) _temp937 >  4u?*((
int*) _temp937) ==  Cyc_Absyn_TupleType: 0){ _LL962: _temp961=(( struct Cyc_Absyn_TupleType_struct*)
_temp937)->f1; goto _LL948;} else{ goto _LL949;} _LL949: if(( unsigned int)
_temp937 >  4u?*(( int*) _temp937) ==  Cyc_Absyn_TunionFieldType: 0){ _LL964:
_temp963=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp937)->f1; _LL966:
_temp965=( void*) _temp963.field_info; if(*(( int*) _temp965) ==  Cyc_Absyn_KnownTunionfield){
_LL970: _temp969=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp965)->f1;
goto _LL968; _LL968: _temp967=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp965)->f2; goto _LL950;} else{ goto _LL951;}} else{ goto _LL951;} _LL951:
goto _LL952; _LL940: if( _temp953 ==  0){ return({ void* _temp971[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u),
_tag_arr( _temp971, sizeof( void*), 0u));});} if((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp953)))->fields ==  0){ goto _LL938;} if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp953)))->fields))->v)
<=  _temp899){({ struct Cyc_Std_Int_pa_struct _temp974; _temp974.tag= Cyc_Std_Int_pa;
_temp974.f1=( int) _temp899;{ struct Cyc_Std_Int_pa_struct _temp973; _temp973.tag=
Cyc_Std_Int_pa; _temp973.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp953)))->fields))->v));{
void* _temp972[ 2u]={& _temp973,& _temp974}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp972, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL938; _LL942: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp955) <=  _temp899){({ struct Cyc_Std_Int_pa_struct _temp977; _temp977.tag=
Cyc_Std_Int_pa; _temp977.f1=( int) _temp899;{ struct Cyc_Std_Int_pa_struct
_temp976; _temp976.tag= Cyc_Std_Int_pa; _temp976.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp955));{ void* _temp975[ 2u]={&
_temp976,& _temp977}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp975, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL938; _LL944: if( _temp957 ==  0){ return({ void* _temp978[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp978, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp957)))->fields ==  0){ goto
_LL938;} if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((*(( struct Cyc_Absyn_Uniondecl**) _check_null(
_temp957)))->fields))->v) <=  _temp899){({ struct Cyc_Std_Int_pa_struct _temp981;
_temp981.tag= Cyc_Std_Int_pa; _temp981.f1=( int) _temp899;{ struct Cyc_Std_Int_pa_struct
_temp980; _temp980.tag= Cyc_Std_Int_pa; _temp980.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp957)))->fields))->v));{
void* _temp979[ 2u]={& _temp980,& _temp981}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp979, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL938; _LL946: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp959) <=  _temp899){({ struct Cyc_Std_Int_pa_struct _temp984; _temp984.tag=
Cyc_Std_Int_pa; _temp984.f1=( int) _temp899;{ struct Cyc_Std_Int_pa_struct
_temp983; _temp983.tag= Cyc_Std_Int_pa; _temp983.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp959));{ void* _temp982[ 2u]={&
_temp983,& _temp984}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp982, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL938; _LL948: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp961) <=  _temp899){({ struct Cyc_Std_Int_pa_struct _temp987; _temp987.tag=
Cyc_Std_Int_pa; _temp987.f1=( int) _temp899;{ struct Cyc_Std_Int_pa_struct
_temp986; _temp986.tag= Cyc_Std_Int_pa; _temp986.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp961));{ void* _temp985[ 2u]={&
_temp986,& _temp987}; Cyc_Tcutil_terr( loc, _tag_arr("tuple has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp985, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL938; _LL950: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp967->typs) <  _temp899){({ struct Cyc_Std_Int_pa_struct _temp990; _temp990.tag=
Cyc_Std_Int_pa; _temp990.f1=( int) _temp899;{ struct Cyc_Std_Int_pa_struct
_temp989; _temp989.tag= Cyc_Std_Int_pa; _temp989.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp967->typs));{ void* _temp988[ 2u]={&
_temp989,& _temp990}; Cyc_Tcutil_terr( loc, _tag_arr("tunionfield has too few components: %d < %d",
sizeof( unsigned char), 44u), _tag_arr( _temp988, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL938; _LL952: goto _LL938; _LL938:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp992; _temp992.tag= Cyc_Std_String_pa; _temp992.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp991[ 1u]={& _temp992};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof( unsigned char), 49u), _tag_arr( _temp991, sizeof( void*), 1u));}});}
goto _LL892;} _LL892:;} return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp993= t;
struct Cyc_Absyn_PtrInfo _temp999; struct Cyc_Absyn_Conref* _temp1001; void*
_temp1003; void* _temp1005; _LL995: if(( unsigned int) _temp993 >  4u?*(( int*)
_temp993) ==  Cyc_Absyn_PointerType: 0){ _LL1000: _temp999=(( struct Cyc_Absyn_PointerType_struct*)
_temp993)->f1; _LL1006: _temp1005=( void*) _temp999.elt_typ; goto _LL1004;
_LL1004: _temp1003=( void*) _temp999.rgn_typ; goto _LL1002; _LL1002: _temp1001=
_temp999.bounds; goto _LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL996:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1003); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1001); return _temp1005; _LL998: return({ struct Cyc_Std_String_pa_struct
_temp1008; _temp1008.tag= Cyc_Std_String_pa; _temp1008.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1007[ 1u]={& _temp1008}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting * or @ type but found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1007, sizeof( void*), 1u));}}); _LL994:;}} static void* Cyc_Tcexp_tcStructMember(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp1009= t; struct Cyc_Absyn_Structdecl**
_temp1025; struct Cyc_Absyn_Structdecl* _temp1027; struct Cyc_List_List*
_temp1028; struct _tuple1* _temp1030; struct Cyc_List_List* _temp1032; struct
Cyc_List_List* _temp1034; struct Cyc_Absyn_Uniondecl** _temp1036; struct Cyc_Absyn_Uniondecl*
_temp1038; struct Cyc_List_List* _temp1039; struct _tuple1* _temp1041; _LL1011:
if(( unsigned int) _temp1009 >  4u?*(( int*) _temp1009) ==  Cyc_Absyn_StructType:
0){ _LL1031: _temp1030=(( struct Cyc_Absyn_StructType_struct*) _temp1009)->f1;
goto _LL1029; _LL1029: _temp1028=(( struct Cyc_Absyn_StructType_struct*)
_temp1009)->f2; goto _LL1026; _LL1026: _temp1025=(( struct Cyc_Absyn_StructType_struct*)
_temp1009)->f3; if( _temp1025 ==  0){ goto _LL1013;} else{ _temp1027=* _temp1025;
goto _LL1012;}} else{ goto _LL1013;} _LL1013: if(( unsigned int) _temp1009 >  4u?*((
int*) _temp1009) ==  Cyc_Absyn_AnonStructType: 0){ _LL1033: _temp1032=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1009)->f1; goto _LL1014;} else{ goto
_LL1015;} _LL1015: if(( unsigned int) _temp1009 >  4u?*(( int*) _temp1009) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1035: _temp1034=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1009)->f1; goto _LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int)
_temp1009 >  4u?*(( int*) _temp1009) ==  Cyc_Absyn_UnionType: 0){ _LL1042:
_temp1041=(( struct Cyc_Absyn_UnionType_struct*) _temp1009)->f1; goto _LL1040;
_LL1040: _temp1039=(( struct Cyc_Absyn_UnionType_struct*) _temp1009)->f2; goto
_LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_UnionType_struct*) _temp1009)->f3;
if( _temp1036 ==  0){ goto _LL1019;} else{ _temp1038=* _temp1036; goto _LL1018;}}
else{ goto _LL1019;} _LL1019: if(( unsigned int) _temp1009 >  4u?*(( int*)
_temp1009) ==  Cyc_Absyn_ArrayType: 0){ goto _LL1043;} else{ goto _LL1021;}
_LL1043: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL1020;} else{ goto _LL1021;} _LL1021: if(( unsigned int)
_temp1009 >  4u?*(( int*) _temp1009) ==  Cyc_Absyn_PointerType: 0){ goto _LL1044;}
else{ goto _LL1023;} _LL1044: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) ==  0){ goto _LL1022;} else{ goto _LL1023;} _LL1023: goto
_LL1024; _LL1012: if( _temp1030 ==  0){ return({ void* _temp1045[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u), _tag_arr( _temp1045, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1046= Cyc_Absyn_lookup_struct_field( _temp1027, f); if( _temp1046 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1049; _temp1049.tag= Cyc_Std_String_pa;
_temp1049.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1048; _temp1048.tag= Cyc_Std_String_pa; _temp1048.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1030));{ void*
_temp1047[ 2u]={& _temp1048,& _temp1049}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1047, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp1050= _new_region(); struct _RegionHandle*
rgn=& _temp1050; _push_region( rgn);{ struct Cyc_List_List* _temp1051=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1027->tvs,
_temp1028); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1051,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1046))->type);}; _pop_region( rgn);} return t2;}} _LL1014: {
struct Cyc_Absyn_Structfield* _temp1052= Cyc_Absyn_lookup_field( _temp1032, f);
if( _temp1052 ==  0){ return({ struct Cyc_Std_String_pa_struct _temp1054;
_temp1054.tag= Cyc_Std_String_pa; _temp1054.f1=( struct _tagged_arr)* f;{ void*
_temp1053[ 1u]={& _temp1054}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u), _tag_arr( _temp1053, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1052))->type;}
_LL1016: { struct Cyc_Absyn_Structfield* _temp1055= Cyc_Absyn_lookup_field(
_temp1034, f); if( _temp1055 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1057; _temp1057.tag= Cyc_Std_String_pa; _temp1057.f1=( struct _tagged_arr)*
f;{ void* _temp1056[ 1u]={& _temp1057}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1056, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1055))->type;}
_LL1018: if( _temp1041 ==  0){ return({ void* _temp1058[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u), _tag_arr( _temp1058, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1059= Cyc_Absyn_lookup_union_field( _temp1038, f); if( _temp1059 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1062; _temp1062.tag= Cyc_Std_String_pa;
_temp1062.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1061; _temp1061.tag= Cyc_Std_String_pa; _temp1061.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1041));{ void*
_temp1060[ 2u]={& _temp1061,& _temp1062}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1060, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1063=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1038->tvs, _temp1039); void*
_temp1064= Cyc_Tcutil_substitute( _temp1063,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1059))->type); return _temp1064;}} _LL1020:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1065=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1065[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1066; _temp1066.tag= Cyc_Absyn_Primop_e;
_temp1066.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1066.f2=({ struct Cyc_List_List*
_temp1067=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1067->hd=( void*) e; _temp1067->tl= 0; _temp1067;}); _temp1066;});
_temp1065;}))); return Cyc_Absyn_uint_t; _LL1022:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1068=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1068[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1069; _temp1069.tag= Cyc_Absyn_Primop_e;
_temp1069.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1069.f2=({ struct Cyc_List_List*
_temp1070=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1070->hd=( void*) e; _temp1070->tl= 0; _temp1070;}); _temp1069;});
_temp1068;}))); return Cyc_Absyn_uint_t; _LL1024: if( Cyc_Std_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1072; _temp1072.tag= Cyc_Std_String_pa; _temp1072.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1071[ 1u]={& _temp1072}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp1071, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Std_String_pa_struct _temp1074; _temp1074.tag= Cyc_Std_String_pa;
_temp1074.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1073[
1u]={& _temp1074}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp1073, sizeof( void*), 1u));}});}
_LL1010:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1075= t;
struct Cyc_Absyn_PtrInfo _temp1081; struct Cyc_Absyn_Conref* _temp1083; void*
_temp1085; void* _temp1087; _LL1077: if(( unsigned int) _temp1075 >  4u?*(( int*)
_temp1075) ==  Cyc_Absyn_PointerType: 0){ _LL1082: _temp1081=(( struct Cyc_Absyn_PointerType_struct*)
_temp1075)->f1; _LL1088: _temp1087=( void*) _temp1081.elt_typ; goto _LL1086;
_LL1086: _temp1085=( void*) _temp1081.rgn_typ; goto _LL1084; _LL1084: _temp1083=
_temp1081.bounds; goto _LL1078;} else{ goto _LL1079;} _LL1079: goto _LL1080;
_LL1078: Cyc_Tcutil_check_nonzero_bound( loc, _temp1083);{ void* _temp1089= Cyc_Tcutil_compress(
_temp1087); struct Cyc_Absyn_Structdecl** _temp1101; struct Cyc_Absyn_Structdecl*
_temp1103; struct Cyc_List_List* _temp1104; struct _tuple1* _temp1106; struct
Cyc_Absyn_Uniondecl** _temp1108; struct Cyc_Absyn_Uniondecl* _temp1110; struct
Cyc_List_List* _temp1111; struct _tuple1* _temp1113; struct Cyc_List_List*
_temp1115; struct Cyc_List_List* _temp1117; _LL1091: if(( unsigned int)
_temp1089 >  4u?*(( int*) _temp1089) ==  Cyc_Absyn_StructType: 0){ _LL1107:
_temp1106=(( struct Cyc_Absyn_StructType_struct*) _temp1089)->f1; goto _LL1105;
_LL1105: _temp1104=(( struct Cyc_Absyn_StructType_struct*) _temp1089)->f2; goto
_LL1102; _LL1102: _temp1101=(( struct Cyc_Absyn_StructType_struct*) _temp1089)->f3;
if( _temp1101 ==  0){ goto _LL1093;} else{ _temp1103=* _temp1101; goto _LL1092;}}
else{ goto _LL1093;} _LL1093: if(( unsigned int) _temp1089 >  4u?*(( int*)
_temp1089) ==  Cyc_Absyn_UnionType: 0){ _LL1114: _temp1113=(( struct Cyc_Absyn_UnionType_struct*)
_temp1089)->f1; goto _LL1112; _LL1112: _temp1111=(( struct Cyc_Absyn_UnionType_struct*)
_temp1089)->f2; goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_UnionType_struct*)
_temp1089)->f3; if( _temp1108 ==  0){ goto _LL1095;} else{ _temp1110=* _temp1108;
goto _LL1094;}} else{ goto _LL1095;} _LL1095: if(( unsigned int) _temp1089 >  4u?*((
int*) _temp1089) ==  Cyc_Absyn_AnonStructType: 0){ _LL1116: _temp1115=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1089)->f1; goto _LL1096;} else{ goto
_LL1097;} _LL1097: if(( unsigned int) _temp1089 >  4u?*(( int*) _temp1089) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1118: _temp1117=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1089)->f1; goto _LL1098;} else{ goto _LL1099;} _LL1099: goto _LL1100;
_LL1092: { struct Cyc_Absyn_Structfield* _temp1119= Cyc_Absyn_lookup_struct_field(
_temp1103, f); if( _temp1119 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1122; _temp1122.tag= Cyc_Std_String_pa; _temp1122.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1121; _temp1121.tag= Cyc_Std_String_pa;
_temp1121.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1106));{ void* _temp1120[ 2u]={& _temp1121,& _temp1122}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct %s has no %s field", sizeof( unsigned char), 26u),
_tag_arr( _temp1120, sizeof( void*), 2u));}}});}{ void* t3;{ struct
_RegionHandle _temp1123= _new_region(); struct _RegionHandle* rgn=& _temp1123;
_push_region( rgn);{ struct Cyc_List_List* _temp1124=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1103->tvs, _temp1104);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1124,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1119))->type);}; _pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1085); return t3;}} _LL1094: { struct Cyc_Absyn_Structfield*
_temp1125= Cyc_Absyn_lookup_union_field( _temp1110, f); if( _temp1125 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1128; _temp1128.tag= Cyc_Std_String_pa;
_temp1128.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1127; _temp1127.tag= Cyc_Std_String_pa; _temp1127.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1113));{ void*
_temp1126[ 2u]={& _temp1127,& _temp1128}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1126, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1129=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1110->tvs, _temp1111); void*
_temp1130= Cyc_Tcutil_substitute( _temp1129,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1125))->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1085); return _temp1130;}} _LL1096: { struct Cyc_Absyn_Structfield*
_temp1131= Cyc_Absyn_lookup_field( _temp1115, f); if( _temp1131 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1133; _temp1133.tag= Cyc_Std_String_pa;
_temp1133.f1=( struct _tagged_arr)* f;{ void* _temp1132[ 1u]={& _temp1133}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1132, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1131))->type;} _LL1098: { struct Cyc_Absyn_Structfield*
_temp1134= Cyc_Absyn_lookup_field( _temp1117, f); if( _temp1134 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1136; _temp1136.tag= Cyc_Std_String_pa;
_temp1136.f1=( struct _tagged_arr)* f;{ void* _temp1135[ 1u]={& _temp1136}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union has no %s field", sizeof( unsigned char), 22u),
_tag_arr( _temp1135, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1134))->type;} _LL1100: return({ struct Cyc_Std_String_pa_struct
_temp1138; _temp1138.tag= Cyc_Std_String_pa; _temp1138.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1137[ 1u]={& _temp1138}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1137, sizeof( void*), 1u));}}); _LL1090:;} _LL1080: return({
struct Cyc_Std_String_pa_struct _temp1140; _temp1140.tag= Cyc_Std_String_pa;
_temp1140.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1139[
1u]={& _temp1140}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1139, sizeof( void*), 1u));}});
_LL1076:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1141; _push_handler(& _temp1141);{ int _temp1143= 0; if(
setjmp( _temp1141.handler)){ _temp1143= 1;} if( ! _temp1143){{ void* _temp1144=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1144;}; _pop_handler();} else{ void* _temp1142=(
void*) _exn_thrown; void* _temp1146= _temp1142; _LL1148: if( _temp1146 ==  Cyc_List_Nth){
goto _LL1149;} else{ goto _LL1150;} _LL1150: goto _LL1151; _LL1149: return({
struct Cyc_Std_Int_pa_struct _temp1154; _temp1154.tag= Cyc_Std_Int_pa; _temp1154.f1=(
int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts));{
struct Cyc_Std_Int_pa_struct _temp1153; _temp1153.tag= Cyc_Std_Int_pa; _temp1153.f1=(
int) i;{ void* _temp1152[ 2u]={& _temp1153,& _temp1154}; Cyc_Tcexp_expr_err( te,
loc, _tag_arr("index is %d but tuple has only %d fields", sizeof( unsigned char),
41u), _tag_arr( _temp1152, sizeof( void*), 2u));}}}); _LL1151:( void) _throw(
_temp1146); _LL1147:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Std_String_pa_struct _temp1156; _temp1156.tag= Cyc_Std_String_pa;
_temp1156.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1155[
1u]={& _temp1156}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1155, sizeof( void*), 1u));}});}{
void* _temp1157= t1; struct Cyc_Absyn_PtrInfo _temp1165; struct Cyc_Absyn_Conref*
_temp1167; struct Cyc_Absyn_Tqual _temp1169; void* _temp1171; void* _temp1173;
struct Cyc_List_List* _temp1175; _LL1159: if(( unsigned int) _temp1157 >  4u?*((
int*) _temp1157) ==  Cyc_Absyn_PointerType: 0){ _LL1166: _temp1165=(( struct Cyc_Absyn_PointerType_struct*)
_temp1157)->f1; _LL1174: _temp1173=( void*) _temp1165.elt_typ; goto _LL1172;
_LL1172: _temp1171=( void*) _temp1165.rgn_typ; goto _LL1170; _LL1170: _temp1169=
_temp1165.tq; goto _LL1168; _LL1168: _temp1167= _temp1165.bounds; goto _LL1160;}
else{ goto _LL1161;} _LL1161: if(( unsigned int) _temp1157 >  4u?*(( int*)
_temp1157) ==  Cyc_Absyn_TupleType: 0){ _LL1176: _temp1175=(( struct Cyc_Absyn_TupleType_struct*)
_temp1157)->f1; goto _LL1162;} else{ goto _LL1163;} _LL1163: goto _LL1164;
_LL1160: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1167);} else{ if( Cyc_Tcutil_is_bound_one( _temp1167)){({ void*
_temp1177[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1177, sizeof( void*), 0u));});} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1167);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1171); return
_temp1173; _LL1162: return Cyc_Tcexp_ithTupleType( te, loc, _temp1175, e2);
_LL1164: return({ struct Cyc_Std_String_pa_struct _temp1179; _temp1179.tag= Cyc_Std_String_pa;
_temp1179.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp1178[
1u]={& _temp1179}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1178, sizeof( void*), 1u));}});
_LL1158:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1180= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp1186; _LL1182: if((
unsigned int) _temp1180 >  4u?*(( int*) _temp1180) ==  Cyc_Absyn_TupleType: 0){
_LL1187: _temp1186=(( struct Cyc_Absyn_TupleType_struct*) _temp1180)->f1; goto
_LL1183;} else{ goto _LL1184;} _LL1184: goto _LL1185; _LL1183: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1186) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){({ void* _temp1188[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("tuple expression has the wrong number of fields", sizeof(
unsigned char), 48u), _tag_arr( _temp1188, sizeof( void*), 0u));}); goto _LL1181;}
for( 0; es !=  0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1186=((
struct Cyc_List_List*) _check_null( _temp1186))->tl)){ void* _temp1189=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1186))->hd)).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1189,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1190=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1190->hd=( void*)({ struct
_tuple8* _temp1191=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp1191->f1=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1186))->hd)).f1; _temp1191->f2=( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1191;}); _temp1190->tl= fields; _temp1190;});} done= 1; goto _LL1181;
_LL1185: goto _LL1181; _LL1181:;} if( ! done){ for( 0; es !=  0; es=(( struct
Cyc_List_List*) _check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); fields=({
struct Cyc_List_List* _temp1192=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1192->hd=( void*)({ struct _tuple8* _temp1193=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1193->f1= Cyc_Absyn_empty_tqual();
_temp1193->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1193;});
_temp1192->tl= fields; _temp1192;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1194=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1194[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1195; _temp1195.tag= Cyc_Absyn_TupleType;
_temp1195.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1195;}); _temp1194;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1196[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1196, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1219=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1219->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1219;})); struct Cyc_Absyn_Const_e_struct* _temp1197=({ struct Cyc_Absyn_Const_e_struct*
_temp1215=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1215[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1216; _temp1216.tag= Cyc_Absyn_Const_e;
_temp1216.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1217=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1217[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1218; _temp1218.tag= Cyc_Absyn_Int_c;
_temp1218.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1218.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1218;}); _temp1217;}));
_temp1216;}); _temp1215;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1197, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1198=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1198->v=( void*)
Cyc_Absyn_uint_t; _temp1198;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1213=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1213[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1214; _temp1214.tag= Cyc_Absyn_ArrayType;
_temp1214.f1=( void*) res; _temp1214.f2= Cyc_Absyn_empty_tqual(); _temp1214.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1214;}); _temp1213;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){({ struct
Cyc_Std_String_pa_struct _temp1200; _temp1200.tag= Cyc_Std_String_pa; _temp1200.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void* _temp1199[ 1u]={&
_temp1200}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1199, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des=(( struct Cyc_List_List*)
_check_null( des))->tl)){ struct Cyc_List_List* ds=(*(( struct _tuple7*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1; if( ds !=  0){ void*
_temp1201=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp1207; _LL1203: if(*(( int*) _temp1201) ==  Cyc_Absyn_FieldName){ goto
_LL1204;} else{ goto _LL1205;} _LL1205: if(*(( int*) _temp1201) ==  Cyc_Absyn_ArrayElement){
_LL1208: _temp1207=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1201)->f1;
goto _LL1206;} else{ goto _LL1202;} _LL1204:({ void* _temp1209[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u), _tag_arr( _temp1209, sizeof( void*), 0u));}); goto _LL1202;
_LL1206: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1207);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1207); if( i !=  offset){({ struct Cyc_Std_Int_pa_struct _temp1212;
_temp1212.tag= Cyc_Std_Int_pa; _temp1212.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp1211; _temp1211.tag= Cyc_Std_Int_pa; _temp1211.f1=( int)(( unsigned int)
offset);{ void* _temp1210[ 2u]={& _temp1211,& _temp1212}; Cyc_Tcutil_terr(
_temp1207->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1210, sizeof( void*), 2u));}}});} goto
_LL1202;} _LL1202:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Std_String_pa_struct _temp1221; _temp1221.tag= Cyc_Std_String_pa;
_temp1221.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1220[ 1u]={& _temp1221}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1220, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1222[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1222, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);}{ struct Cyc_Absyn_PtrInfo pinfo; void** _temp1223= 0; struct Cyc_Absyn_Tqual*
_temp1224= 0; if( topt !=  0){ void* _temp1225= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1233; struct Cyc_Absyn_Exp*
_temp1235; struct Cyc_Absyn_Tqual _temp1237; struct Cyc_Absyn_Tqual* _temp1239;
void* _temp1240; void** _temp1242; _LL1227: if(( unsigned int) _temp1225 >  4u?*((
int*) _temp1225) ==  Cyc_Absyn_PointerType: 0){ _LL1234: _temp1233=(( struct Cyc_Absyn_PointerType_struct*)
_temp1225)->f1; goto _LL1228;} else{ goto _LL1229;} _LL1229: if(( unsigned int)
_temp1225 >  4u?*(( int*) _temp1225) ==  Cyc_Absyn_ArrayType: 0){ _LL1241:
_temp1240=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1225)->f1;
_temp1242=( void**)&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1225)->f1);
goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1225)->f2; _temp1239=( struct Cyc_Absyn_Tqual*)&(( struct Cyc_Absyn_ArrayType_struct*)
_temp1225)->f2; goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1225)->f3; goto _LL1230;} else{ goto _LL1231;} _LL1231: goto _LL1232;
_LL1228: pinfo= _temp1233; _temp1223=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1224=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1226; _LL1230: _temp1223=(
void**) _temp1242; _temp1224=( struct Cyc_Absyn_Tqual*) _temp1239; goto _LL1226;
_LL1232: goto _LL1226; _LL1226:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1223, body);
if( te->le ==  0){ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void* _temp1243[
0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant", sizeof(
unsigned char), 22u), _tag_arr( _temp1243, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_is_const_exp(
te, body)){({ void* _temp1244[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("body is not constant",
sizeof( unsigned char), 21u), _tag_arr( _temp1244, sizeof( void*), 0u));});}}
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1245=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1245[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1246; _temp1246.tag= Cyc_Absyn_ArrayType;
_temp1246.f1=( void*) t; _temp1246.f2= _temp1224 ==  0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1224)); _temp1246.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1246;}); _temp1245;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1247; _push_handler(&
_temp1247);{ int _temp1249= 0; if( setjmp( _temp1247.handler)){ _temp1249= 1;}
if( ! _temp1249){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1248=( void*) _exn_thrown; void* _temp1251= _temp1248; _LL1253:
if( _temp1251 ==  Cyc_Dict_Absent){ goto _LL1254;} else{ goto _LL1255;} _LL1255:
goto _LL1256; _LL1254:({ struct Cyc_Std_String_pa_struct _temp1258; _temp1258.tag=
Cyc_Std_String_pa; _temp1258.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1257[ 1u]={& _temp1258}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1257, sizeof( void*), 1u));}});
return topt !=  0?*(( void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType;
_LL1256:( void) _throw( _temp1251); _LL1252:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*)
sd; if( sd->name !=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v;}}{ struct _RegionHandle _temp1259= _new_region();
struct _RegionHandle* rgn=& _temp1259; _push_region( rgn);{ struct _tuple4
_temp1260=({ struct _tuple4 _temp1284; _temp1284.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1284.f2= rgn; _temp1284;}); struct Cyc_List_List* _temp1261=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1260, sd->tvs); struct Cyc_List_List*
_temp1262=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1261); struct
Cyc_Absyn_StructType_struct* _temp1263=({ struct Cyc_Absyn_StructType_struct*
_temp1281=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1281[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1282; _temp1282.tag=
Cyc_Absyn_StructType; _temp1282.f1=( struct _tuple1*)* tn; _temp1282.f2=
_temp1262; _temp1282.f3=({ struct Cyc_Absyn_Structdecl** _temp1283=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1283[ 0]= sd; _temp1283;});
_temp1282;}); _temp1281;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1263,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1264=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1264->v=( void*)
_temp1262; _temp1264;}); if( sd->fields ==  0){({ void* _temp1265[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1265, sizeof( void*), 0u));});{ void* _temp1266=( void*)
_temp1263; _npop_handler( 0u); return _temp1266;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1269; struct Cyc_Absyn_Exp* _temp1270; struct Cyc_Absyn_Structfield*
_temp1272; struct _tuple9* _temp1267=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1269=* _temp1267; _LL1273: _temp1272= _temp1269.f1;
goto _LL1271; _LL1271: _temp1270= _temp1269.f2; goto _LL1268; _LL1268: { void*
_temp1274= Cyc_Tcutil_rsubstitute( rgn, _temp1261,( void*) _temp1272->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1274, _temp1270); if( ! Cyc_Tcutil_coerce_arg( te, _temp1270,
_temp1274)){({ struct Cyc_Std_String_pa_struct _temp1279; _temp1279.tag= Cyc_Std_String_pa;
_temp1279.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1270->topt))->v);{ struct Cyc_Std_String_pa_struct _temp1278;
_temp1278.tag= Cyc_Std_String_pa; _temp1278.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1274);{ struct Cyc_Std_String_pa_struct _temp1277; _temp1277.tag= Cyc_Std_String_pa;
_temp1277.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Std_String_pa_struct
_temp1276; _temp1276.tag= Cyc_Std_String_pa; _temp1276.f1=( struct _tagged_arr)*
_temp1272->name;{ void* _temp1275[ 4u]={& _temp1276,& _temp1277,& _temp1278,&
_temp1279}; Cyc_Tcutil_terr( _temp1270->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1275, sizeof( void*), 4u));}}}}});}}}{
void* _temp1280=( void*) _temp1263; _npop_handler( 0u); return _temp1280;}}};
_pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1285= _new_region(); struct _RegionHandle* rgn=&
_temp1285; _push_region( rgn);{ void* _temp1286= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1292; _LL1288: if(( unsigned int) _temp1286 >  4u?*((
int*) _temp1286) ==  Cyc_Absyn_AnonStructType: 0){ _LL1293: _temp1292=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1286)->f1; goto _LL1289;} else{ goto
_LL1290;} _LL1290: goto _LL1291; _LL1289: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1292); for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1296; struct Cyc_Absyn_Exp*
_temp1297; struct Cyc_Absyn_Structfield* _temp1299; struct _tuple9* _temp1294=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1296=*
_temp1294; _LL1300: _temp1299= _temp1296.f1; goto _LL1298; _LL1298: _temp1297=
_temp1296.f2; goto _LL1295; _LL1295: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1299->type)), _temp1297); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1297,( void*) _temp1299->type)){({ struct Cyc_Std_String_pa_struct
_temp1304; _temp1304.tag= Cyc_Std_String_pa; _temp1304.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1297->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1303; _temp1303.tag= Cyc_Std_String_pa;
_temp1303.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1299->type);{
struct Cyc_Std_String_pa_struct _temp1302; _temp1302.tag= Cyc_Std_String_pa;
_temp1302.f1=( struct _tagged_arr)* _temp1299->name;{ void* _temp1301[ 3u]={&
_temp1302,& _temp1303,& _temp1304}; Cyc_Tcutil_terr( _temp1297->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1301, sizeof( void*), 3u));}}}});}}
goto _LL1287;} _LL1291:({ void* _temp1305[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u), _tag_arr( _temp1305, sizeof( void*), 0u));}); goto
_LL1287; _LL1287:;}; _pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1306= _new_region(); struct _RegionHandle* rgn=&
_temp1306; _push_region( rgn);{ struct _tuple4 _temp1307=({ struct _tuple4
_temp1353; _temp1353.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1353.f2= rgn;
_temp1353;}); struct Cyc_List_List* _temp1308=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1307, tud->tvs); struct Cyc_List_List* _temp1309=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1307, tuf->tvs); struct Cyc_List_List* _temp1310=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1308); struct Cyc_List_List* _temp1311=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1309); struct Cyc_List_List*
_temp1312=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1308, _temp1309);*
all_ref=({ struct Cyc_Core_Opt* _temp1313=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1313->v=( void*) _temp1310; _temp1313;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1314=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1314->v=( void*) _temp1311; _temp1314;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1348=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1348[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1349; _temp1349.tag=
Cyc_Absyn_TunionFieldType; _temp1349.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1350; _temp1350.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1351=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1351[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1352; _temp1352.tag= Cyc_Absyn_KnownTunionfield; _temp1352.f1= tud;
_temp1352.f2= tuf; _temp1352;}); _temp1351;})); _temp1350.targs= _temp1310;
_temp1350;}); _temp1349;}); _temp1348;}); if( topt !=  0){ void* _temp1315= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo _temp1323; void*
_temp1325; struct Cyc_List_List* _temp1327; _LL1317: if(( unsigned int)
_temp1315 >  4u?*(( int*) _temp1315) ==  Cyc_Absyn_TunionFieldType: 0){ goto
_LL1318;} else{ goto _LL1319;} _LL1319: if(( unsigned int) _temp1315 >  4u?*((
int*) _temp1315) ==  Cyc_Absyn_TunionType: 0){ _LL1324: _temp1323=(( struct Cyc_Absyn_TunionType_struct*)
_temp1315)->f1; _LL1328: _temp1327= _temp1323.targs; goto _LL1326; _LL1326:
_temp1325=( void*) _temp1323.rgn; goto _LL1320;} else{ goto _LL1321;} _LL1321:
goto _LL1322; _LL1318: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res);
goto _LL1316; _LL1320:{ struct Cyc_List_List* a= _temp1310; for( 0; a !=  0?
_temp1327 !=  0: 0;( a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1327=((
struct Cyc_List_List*) _check_null( _temp1327))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( a))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp1327))->hd);}} if( tuf->typs ==  0? es ==  0: 0){ e->topt=({
struct Cyc_Core_Opt* _temp1329=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1329->v=( void*) res; _temp1329;}); res=( void*)({ struct
Cyc_Absyn_TunionType_struct* _temp1330=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1330[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1331; _temp1331.tag= Cyc_Absyn_TunionType;
_temp1331.f1=({ struct Cyc_Absyn_TunionInfo _temp1332; _temp1332.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1333=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1333[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1334; _temp1334.tag= Cyc_Absyn_KnownTunion;
_temp1334.f1=({ struct Cyc_Absyn_Tuniondecl** _temp1335=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp1335[ 0]= tud; _temp1335;});
_temp1334;}); _temp1333;})); _temp1332.targs= _temp1310; _temp1332.rgn=( void*)
_temp1325; _temp1332;}); _temp1331;}); _temp1330;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1336= res; _npop_handler( 0u); return _temp1336;}} goto
_LL1316; _LL1322: goto _LL1316; _LL1316:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es !=  0? ts !=  0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1312,(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Std_String_pa_struct
_temp1340; _temp1340.tag= Cyc_Std_String_pa; _temp1340.f1=( struct _tagged_arr)(
e->topt ==  0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ struct Cyc_Std_String_pa_struct
_temp1339; _temp1339.tag= Cyc_Std_String_pa; _temp1339.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp1338;
_temp1338.tag= Cyc_Std_String_pa; _temp1338.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1337[ 3u]={& _temp1338,& _temp1339,& _temp1340}; Cyc_Tcutil_terr(
e->loc, _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1337, sizeof( void*), 3u));}}}});}}
if( es !=  0){ void* _temp1343=({ struct Cyc_Std_String_pa_struct _temp1342;
_temp1342.tag= Cyc_Std_String_pa; _temp1342.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1341[ 1u]={& _temp1342}; Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,
_tag_arr("too many arguments for tunion constructor %s", sizeof( unsigned char),
45u), _tag_arr( _temp1341, sizeof( void*), 1u));}}); _npop_handler( 0u); return
_temp1343;} if( ts !=  0){ void* _temp1346=({ struct Cyc_Std_String_pa_struct
_temp1345; _temp1345.tag= Cyc_Std_String_pa; _temp1345.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1344[ 1u]={& _temp1345}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("too few arguments for tunion constructor %s", sizeof(
unsigned char), 44u), _tag_arr( _temp1344, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1346;}{ void* _temp1347= res; _npop_handler( 0u);
return _temp1347;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1364=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1364[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1365; _temp1365.tag= Cyc_Absyn_RgnHandleType; _temp1365.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1366=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1366->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1366;})); _temp1365;}); _temp1364;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1354= Cyc_Tcutil_compress( handle_type); void*
_temp1360; _LL1356: if(( unsigned int) _temp1354 >  4u?*(( int*) _temp1354) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1361: _temp1360=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1354)->f1; goto _LL1357;} else{ goto _LL1358;} _LL1358: goto _LL1359;
_LL1357: rgn= _temp1360; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1355; _LL1359:({ struct Cyc_Std_String_pa_struct _temp1363; _temp1363.tag=
Cyc_Std_String_pa; _temp1363.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1362[ 1u]={& _temp1363}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1362, sizeof( void*), 1u));}});
goto _LL1355; _LL1355:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp1367= t; struct Cyc_Absyn_TunionFieldInfo
_temp1373; void* _temp1375; struct Cyc_Absyn_Tunionfield* _temp1377; _LL1369:
if(( unsigned int) _temp1367 >  4u?*(( int*) _temp1367) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1374: _temp1373=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1367)->f1;
_LL1376: _temp1375=( void*) _temp1373.field_info; if(*(( int*) _temp1375) == 
Cyc_Absyn_KnownTunionfield){ _LL1378: _temp1377=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1375)->f2; goto _LL1370;} else{ goto _LL1371;}} else{ goto _LL1371;}
_LL1371: goto _LL1372; _LL1370: if( _temp1377->tvs !=  0){({ void* _temp1379[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u), _tag_arr( _temp1379, sizeof( void*), 0u));});}
goto _LL1368; _LL1372: goto _LL1368; _LL1368:;}{ void*(* _temp1380)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1381= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1387; struct Cyc_Absyn_Conref* _temp1389; _LL1383: if(( unsigned int)
_temp1381 >  4u?*(( int*) _temp1381) ==  Cyc_Absyn_PointerType: 0){ _LL1388:
_temp1387=(( struct Cyc_Absyn_PointerType_struct*) _temp1381)->f1; _LL1390:
_temp1389= _temp1387.nullable; goto _LL1384;} else{ goto _LL1385;} _LL1385: goto
_LL1386; _LL1384: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1389)){ _temp1380= Cyc_Absyn_star_typ;} goto _LL1382; _LL1386: goto _LL1382;
_LL1382:;} return _temp1380( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1391=( void*) s->r; struct Cyc_Absyn_Exp* _temp1401;
struct Cyc_Absyn_Stmt* _temp1403; struct Cyc_Absyn_Stmt* _temp1405; struct Cyc_Absyn_Stmt*
_temp1407; struct Cyc_Absyn_Decl* _temp1409; _LL1393: if(( unsigned int)
_temp1391 >  1u?*(( int*) _temp1391) ==  Cyc_Absyn_Exp_s: 0){ _LL1402: _temp1401=((
struct Cyc_Absyn_Exp_s_struct*) _temp1391)->f1; goto _LL1394;} else{ goto
_LL1395;} _LL1395: if(( unsigned int) _temp1391 >  1u?*(( int*) _temp1391) == 
Cyc_Absyn_Seq_s: 0){ _LL1406: _temp1405=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1391)->f1; goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1391)->f2; goto _LL1396;} else{ goto _LL1397;} _LL1397: if(( unsigned int)
_temp1391 >  1u?*(( int*) _temp1391) ==  Cyc_Absyn_Decl_s: 0){ _LL1410:
_temp1409=(( struct Cyc_Absyn_Decl_s_struct*) _temp1391)->f1; goto _LL1408;
_LL1408: _temp1407=(( struct Cyc_Absyn_Decl_s_struct*) _temp1391)->f2; goto
_LL1398;} else{ goto _LL1399;} _LL1399: goto _LL1400; _LL1394: return( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1401->topt))->v; _LL1396: s= _temp1403;
continue; _LL1398: s= _temp1407; continue; _LL1400: return({ void* _temp1411[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1411, sizeof( void*), 0u));});
_LL1392:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1412[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1412, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1413[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1413, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1424=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1424[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1425; _temp1425.tag= Cyc_Absyn_RgnHandleType;
_temp1425.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1426=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1426->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1426;}));
_temp1425;}); _temp1424;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1414= Cyc_Tcutil_compress( handle_type); void* _temp1420; _LL1416: if((
unsigned int) _temp1414 >  4u?*(( int*) _temp1414) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1421: _temp1420=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1414)->f1; goto _LL1417;} else{ goto _LL1418;} _LL1418: goto _LL1419;
_LL1417: rgn= _temp1420; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1415; _LL1419:({ struct Cyc_Std_String_pa_struct _temp1423; _temp1423.tag=
Cyc_Std_String_pa; _temp1423.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1422[ 1u]={& _temp1423}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1422, sizeof( void*), 1u));}});
goto _LL1415; _LL1415:;}{ void* _temp1427=( void*) e1->r; struct Cyc_Absyn_Exp*
_temp1439; struct Cyc_Absyn_Exp* _temp1441; struct Cyc_Absyn_Vardecl* _temp1443;
struct Cyc_List_List* _temp1445; struct Cyc_Core_Opt* _temp1447; struct Cyc_List_List*
_temp1449; void* _temp1451; struct _tagged_arr _temp1453; _LL1429: if(*(( int*)
_temp1427) ==  Cyc_Absyn_Comprehension_e){ _LL1444: _temp1443=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1427)->f1; goto _LL1442; _LL1442: _temp1441=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1427)->f2; goto _LL1440; _LL1440: _temp1439=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1427)->f3; goto _LL1430;} else{ goto _LL1431;} _LL1431: if(*(( int*)
_temp1427) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1448: _temp1447=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1427)->f1; goto _LL1446; _LL1446: _temp1445=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1427)->f2; goto _LL1432;} else{ goto _LL1433;} _LL1433: if(*(( int*)
_temp1427) ==  Cyc_Absyn_Array_e){ _LL1450: _temp1449=(( struct Cyc_Absyn_Array_e_struct*)
_temp1427)->f1; goto _LL1434;} else{ goto _LL1435;} _LL1435: if(*(( int*)
_temp1427) ==  Cyc_Absyn_Const_e){ _LL1452: _temp1451=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1427)->f1; if(( unsigned int) _temp1451 >  1u?*(( int*) _temp1451) ==  Cyc_Absyn_String_c:
0){ _LL1454: _temp1453=(( struct Cyc_Absyn_String_c_struct*) _temp1451)->f1;
goto _LL1436;} else{ goto _LL1437;}} else{ goto _LL1437;} _LL1437: goto _LL1438;
_LL1430: { void* _temp1455= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1456= Cyc_Tcutil_compress( _temp1455); struct Cyc_Absyn_Tqual _temp1462;
void* _temp1464; _LL1458: if(( unsigned int) _temp1456 >  4u?*(( int*) _temp1456)
==  Cyc_Absyn_ArrayType: 0){ _LL1465: _temp1464=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1456)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1456)->f2; goto _LL1459;} else{ goto _LL1460;} _LL1460: goto _LL1461;
_LL1459: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1441)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1470=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1470[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1471; _temp1471.tag= Cyc_Absyn_Upper_b; _temp1471.f1=
_temp1441; _temp1471;}); _temp1470;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1467=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1467[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1468; _temp1468.tag= Cyc_Absyn_PointerType;
_temp1468.f1=({ struct Cyc_Absyn_PtrInfo _temp1469; _temp1469.elt_typ=( void*)
_temp1464; _temp1469.rgn_typ=( void*) rgn; _temp1469.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1469.tq= _temp1462; _temp1469.bounds= Cyc_Absyn_new_conref(
b); _temp1469;}); _temp1468;}); _temp1467;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1466=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1466->v=( void*) res_typ; _temp1466;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1461: return({ void* _temp1472[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1472, sizeof( void*), 0u));});
_LL1457:;} _LL1432:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1473=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1473[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1474; _temp1474.tag= Cyc_Absyn_Array_e;
_temp1474.f1= _temp1445; _temp1474;}); _temp1473;}))); _temp1449= _temp1445;
goto _LL1434; _LL1434: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1475=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1481; struct Cyc_Absyn_Tqual _temp1483; void* _temp1485; void** _temp1487;
_LL1477: if(( unsigned int) _temp1475 >  4u?*(( int*) _temp1475) ==  Cyc_Absyn_PointerType:
0){ _LL1482: _temp1481=(( struct Cyc_Absyn_PointerType_struct*) _temp1475)->f1;
_LL1486: _temp1485=( void*) _temp1481.elt_typ; _temp1487=( void**)&((( struct
Cyc_Absyn_PointerType_struct*) _temp1475)->f1).elt_typ; goto _LL1484; _LL1484:
_temp1483= _temp1481.tq; goto _LL1478;} else{ goto _LL1479;} _LL1479: goto
_LL1480; _LL1478: elt_typ_opt=( void**) _temp1487; goto _LL1476; _LL1480: goto
_LL1476; _LL1476:;}{ void* _temp1488= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt,
e1); void* res_typ;{ void* _temp1489= Cyc_Tcutil_compress( _temp1488); struct
Cyc_Absyn_Exp* _temp1495; struct Cyc_Absyn_Tqual _temp1497; void* _temp1499;
_LL1491: if(( unsigned int) _temp1489 >  4u?*(( int*) _temp1489) ==  Cyc_Absyn_ArrayType:
0){ _LL1500: _temp1499=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1489)->f1;
goto _LL1498; _LL1498: _temp1497=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1489)->f2; goto _LL1496; _LL1496: _temp1495=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1489)->f3; goto _LL1492;} else{ goto _LL1493;} _LL1493: goto _LL1494;
_LL1492: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1501=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1501[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1502; _temp1502.tag=
Cyc_Absyn_PointerType; _temp1502.f1=({ struct Cyc_Absyn_PtrInfo _temp1503;
_temp1503.elt_typ=( void*) _temp1499; _temp1503.rgn_typ=( void*) rgn; _temp1503.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1503.tq=
_temp1497; _temp1503.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1504=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1504[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1505; _temp1505.tag= Cyc_Absyn_Upper_b;
_temp1505.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1495); _temp1505;});
_temp1504;})); _temp1503;}); _temp1502;}); _temp1501;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1506=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1506->v=( void*) res_typ; _temp1506;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1490; _LL1494: return({ void* _temp1507[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1507, sizeof( void*), 0u));});
_LL1490:;} return res_typ;}} _LL1436: { void* _temp1508= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1509= Cyc_Tcexp_tcExp(
te,( void**)& _temp1508, e1); return Cyc_Absyn_atb_typ( _temp1509, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1510=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1510[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1511; _temp1511.tag= Cyc_Absyn_Upper_b; _temp1511.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1511;}); _temp1510;}));} _LL1438: { void** topt2=
0; if( topt !=  0){ void* _temp1512= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1520; struct Cyc_Absyn_Conref* _temp1522;
void* _temp1524; void** _temp1526; _LL1514: if(( unsigned int) _temp1512 >  4u?*((
int*) _temp1512) ==  Cyc_Absyn_PointerType: 0){ _LL1521: _temp1520=(( struct Cyc_Absyn_PointerType_struct*)
_temp1512)->f1; _LL1525: _temp1524=( void*) _temp1520.elt_typ; _temp1526=( void**)&(((
struct Cyc_Absyn_PointerType_struct*) _temp1512)->f1).elt_typ; goto _LL1523;
_LL1523: _temp1522= _temp1520.nullable; goto _LL1515;} else{ goto _LL1516;}
_LL1516: if(( unsigned int) _temp1512 >  4u?*(( int*) _temp1512) ==  Cyc_Absyn_TunionType:
0){ goto _LL1517;} else{ goto _LL1518;} _LL1518: goto _LL1519; _LL1515: topt2=(
void**) _temp1526; goto _LL1513; _LL1517: topt2=({ void** _temp1527=( void**)
_cycalloc( sizeof( void*)); _temp1527[ 0]=*(( void**) _check_null( topt));
_temp1527;}); goto _LL1513; _LL1519: goto _LL1513; _LL1513:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1529=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1529[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1530; _temp1530.tag=
Cyc_Absyn_PointerType; _temp1530.f1=({ struct Cyc_Absyn_PtrInfo _temp1531;
_temp1531.elt_typ=( void*) telt; _temp1531.rgn_typ=( void*) rgn; _temp1531.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1531.tq= Cyc_Absyn_empty_tqual();
_temp1531.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1532=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1532[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1533; _temp1533.tag= Cyc_Absyn_Upper_b;
_temp1533.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1533;}); _temp1532;})); _temp1531;});
_temp1530;}); _temp1529;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1528=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1528->v=(
void*) res_typ; _temp1528;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1428:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress( Cyc_Tcexp_tcExpNoPromote(
te, topt, e)); void* _temp1534= t; struct Cyc_Absyn_Exp* _temp1540; struct Cyc_Absyn_Tqual
_temp1542; void* _temp1544; _LL1536: if(( unsigned int) _temp1534 >  4u?*(( int*)
_temp1534) ==  Cyc_Absyn_ArrayType: 0){ _LL1545: _temp1544=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1534)->f1; goto _LL1543; _LL1543: _temp1542=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1534)->f2; goto _LL1541; _LL1541: _temp1540=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1534)->f3; goto _LL1537;} else{ goto _LL1538;} _LL1538: goto _LL1539;
_LL1537: { void* _temp1548; struct _tuple6 _temp1546= Cyc_Tcutil_addressof_props(
te, e); _LL1549: _temp1548= _temp1546.f2; goto _LL1547; _LL1547: { void*
_temp1550= _temp1540 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1551=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1551[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1552; _temp1552.tag= Cyc_Absyn_Upper_b;
_temp1552.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1540); _temp1552;});
_temp1551;}); t= Cyc_Absyn_atb_typ( _temp1544, _temp1548, _temp1542, _temp1550);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1539: return t; _LL1535:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1553=( void*) e->r; void* _temp1563; _LL1555: if(*(( int*)
_temp1553) ==  Cyc_Absyn_Array_e){ goto _LL1556;} else{ goto _LL1557;} _LL1557:
if(*(( int*) _temp1553) ==  Cyc_Absyn_Comprehension_e){ goto _LL1558;} else{
goto _LL1559;} _LL1559: if(*(( int*) _temp1553) ==  Cyc_Absyn_Const_e){ _LL1564:
_temp1563=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1553)->f1; if((
unsigned int) _temp1563 >  1u?*(( int*) _temp1563) ==  Cyc_Absyn_String_c: 0){
goto _LL1560;} else{ goto _LL1561;}} else{ goto _LL1561;} _LL1561: goto _LL1562;
_LL1556: return t; _LL1558: return t; _LL1560: return t; _LL1562: t= Cyc_Tcutil_compress(
t);{ void* _temp1565= t; struct Cyc_Absyn_Exp* _temp1571; struct Cyc_Absyn_Tqual
_temp1573; void* _temp1575; _LL1567: if(( unsigned int) _temp1565 >  4u?*(( int*)
_temp1565) ==  Cyc_Absyn_ArrayType: 0){ _LL1576: _temp1575=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1565)->f1; goto _LL1574; _LL1574: _temp1573=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1565)->f2; goto _LL1572; _LL1572: _temp1571=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1565)->f3; goto _LL1568;} else{ goto _LL1569;} _LL1569: goto _LL1570;
_LL1568: { void* _temp1579; struct _tuple6 _temp1577= Cyc_Tcutil_addressof_props(
te, e); _LL1580: _temp1579= _temp1577.f2; goto _LL1578; _LL1578: { void* b=
_temp1571 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1581=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1581[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1582; _temp1582.tag= Cyc_Absyn_Upper_b;
_temp1582.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1571); _temp1582;});
_temp1581;}); t= Cyc_Absyn_atb_typ( _temp1575, _temp1579, _temp1573, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1570: return t; _LL1566:;} _LL1554:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1583=( void*) e->r; struct Cyc_Absyn_Exp* _temp1589; _LL1585: if(*(( int*)
_temp1583) ==  Cyc_Absyn_NoInstantiate_e){ _LL1590: _temp1589=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1583)->f1; goto _LL1586;} else{ goto _LL1587;} _LL1587: goto _LL1588;
_LL1586: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1589);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1589->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1589->topt))->v)); e->topt= _temp1589->topt;
goto _LL1584; _LL1588: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1591=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1597; struct Cyc_Absyn_Conref* _temp1599; struct
Cyc_Absyn_Tqual _temp1601; struct Cyc_Absyn_Conref* _temp1603; void* _temp1605;
void* _temp1607; _LL1593: if(( unsigned int) _temp1591 >  4u?*(( int*) _temp1591)
==  Cyc_Absyn_PointerType: 0){ _LL1598: _temp1597=(( struct Cyc_Absyn_PointerType_struct*)
_temp1591)->f1; _LL1608: _temp1607=( void*) _temp1597.elt_typ; goto _LL1606;
_LL1606: _temp1605=( void*) _temp1597.rgn_typ; goto _LL1604; _LL1604: _temp1603=
_temp1597.nullable; goto _LL1602; _LL1602: _temp1601= _temp1597.tq; goto _LL1600;
_LL1600: _temp1599= _temp1597.bounds; goto _LL1594;} else{ goto _LL1595;}
_LL1595: goto _LL1596; _LL1594:{ void* _temp1609= Cyc_Tcutil_compress( _temp1607);
struct Cyc_Absyn_FnInfo _temp1615; struct Cyc_List_List* _temp1617; struct Cyc_List_List*
_temp1619; struct Cyc_Absyn_VarargInfo* _temp1621; int _temp1623; struct Cyc_List_List*
_temp1625; void* _temp1627; struct Cyc_Core_Opt* _temp1629; struct Cyc_List_List*
_temp1631; _LL1611: if(( unsigned int) _temp1609 >  4u?*(( int*) _temp1609) == 
Cyc_Absyn_FnType: 0){ _LL1616: _temp1615=(( struct Cyc_Absyn_FnType_struct*)
_temp1609)->f1; _LL1632: _temp1631= _temp1615.tvars; goto _LL1630; _LL1630:
_temp1629= _temp1615.effect; goto _LL1628; _LL1628: _temp1627=( void*) _temp1615.ret_typ;
goto _LL1626; _LL1626: _temp1625= _temp1615.args; goto _LL1624; _LL1624:
_temp1623= _temp1615.c_varargs; goto _LL1622; _LL1622: _temp1621= _temp1615.cyc_varargs;
goto _LL1620; _LL1620: _temp1619= _temp1615.rgn_po; goto _LL1618; _LL1618:
_temp1617= _temp1615.attributes; goto _LL1612;} else{ goto _LL1613;} _LL1613:
goto _LL1614; _LL1612: if( _temp1631 !=  0){ struct _RegionHandle _temp1633=
_new_region(); struct _RegionHandle* rgn=& _temp1633; _push_region( rgn);{
struct _tuple4 _temp1634=({ struct _tuple4 _temp1644; _temp1644.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1644.f2= rgn; _temp1644;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1634, _temp1631); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1641=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1641[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1642; _temp1642.tag= Cyc_Absyn_FnType; _temp1642.f1=({ struct Cyc_Absyn_FnInfo
_temp1643; _temp1643.tvars= 0; _temp1643.effect= _temp1629; _temp1643.ret_typ=(
void*) _temp1627; _temp1643.args= _temp1625; _temp1643.c_varargs= _temp1623;
_temp1643.cyc_varargs= _temp1621; _temp1643.rgn_po= _temp1619; _temp1643.attributes=
_temp1617; _temp1643;}); _temp1642;}); _temp1641;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1638=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1638[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1639; _temp1639.tag= Cyc_Absyn_PointerType;
_temp1639.f1=({ struct Cyc_Absyn_PtrInfo _temp1640; _temp1640.elt_typ=( void*)
ftyp; _temp1640.rgn_typ=( void*) _temp1605; _temp1640.nullable= _temp1603;
_temp1640.tq= _temp1601; _temp1640.bounds= _temp1599; _temp1640;}); _temp1639;});
_temp1638;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1635=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1635[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1636; _temp1636.tag= Cyc_Absyn_Instantiate_e;
_temp1636.f1= inner; _temp1636.f2= ts; _temp1636;}); _temp1635;}))); e->topt=({
struct Cyc_Core_Opt* _temp1637=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1637->v=( void*) new_typ; _temp1637;});}; _pop_region( rgn);}
goto _LL1610; _LL1614: goto _LL1610; _LL1610:;} goto _LL1592; _LL1596: goto
_LL1592; _LL1592:;} goto _LL1584; _LL1584:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1645=( void*) e->r; struct Cyc_Absyn_Exp* _temp1721; struct
_tuple1* _temp1723; struct Cyc_List_List* _temp1725; struct Cyc_Absyn_Exp*
_temp1727; struct Cyc_List_List* _temp1729; struct Cyc_Core_Opt* _temp1731; void*
_temp1733; void* _temp1735; struct _tuple1* _temp1737; struct Cyc_List_List*
_temp1739; void* _temp1741; void* _temp1743; struct Cyc_Absyn_Exp* _temp1745;
struct Cyc_Absyn_Exp* _temp1747; struct Cyc_Core_Opt* _temp1749; struct Cyc_Absyn_Exp*
_temp1751; struct Cyc_Absyn_Exp* _temp1753; struct Cyc_Absyn_Exp* _temp1755;
struct Cyc_Absyn_Exp* _temp1757; struct Cyc_Absyn_Exp* _temp1759; struct Cyc_Absyn_Exp*
_temp1761; struct Cyc_Absyn_VarargCallInfo* _temp1763; struct Cyc_Absyn_VarargCallInfo**
_temp1765; struct Cyc_List_List* _temp1766; struct Cyc_Absyn_Exp* _temp1768;
struct Cyc_Absyn_Exp* _temp1770; struct Cyc_List_List* _temp1772; struct Cyc_Absyn_Exp*
_temp1774; struct Cyc_Absyn_Exp* _temp1776; void* _temp1778; struct Cyc_Absyn_Exp*
_temp1780; struct Cyc_Absyn_Exp* _temp1782; struct Cyc_Absyn_Exp* _temp1784;
struct Cyc_Absyn_Exp* _temp1786; void* _temp1788; void* _temp1790; void*
_temp1792; struct Cyc_Absyn_Exp* _temp1794; struct _tagged_arr* _temp1796;
struct Cyc_Absyn_Exp* _temp1798; struct _tagged_arr* _temp1800; struct Cyc_Absyn_Exp*
_temp1802; struct Cyc_Absyn_Exp* _temp1804; struct Cyc_Absyn_Exp* _temp1806;
struct Cyc_List_List* _temp1808; struct Cyc_List_List* _temp1810; struct _tuple2*
_temp1812; struct Cyc_List_List* _temp1814; struct Cyc_Absyn_Stmt* _temp1816;
struct Cyc_Absyn_Fndecl* _temp1818; struct Cyc_Absyn_Exp* _temp1820; struct Cyc_Absyn_Exp*
_temp1822; struct Cyc_Absyn_Exp* _temp1824; struct Cyc_Absyn_Vardecl* _temp1826;
struct Cyc_Absyn_Structdecl* _temp1828; struct Cyc_Absyn_Structdecl** _temp1830;
struct Cyc_List_List* _temp1831; struct Cyc_Core_Opt* _temp1833; struct Cyc_Core_Opt**
_temp1835; struct _tuple1* _temp1836; struct _tuple1** _temp1838; struct Cyc_List_List*
_temp1839; void* _temp1841; struct Cyc_Absyn_Tunionfield* _temp1843; struct Cyc_Absyn_Tuniondecl*
_temp1845; struct Cyc_List_List* _temp1847; struct Cyc_Core_Opt* _temp1849;
struct Cyc_Core_Opt** _temp1851; struct Cyc_Core_Opt* _temp1852; struct Cyc_Core_Opt**
_temp1854; struct Cyc_Absyn_Enumfield* _temp1855; struct Cyc_Absyn_Enumdecl*
_temp1857; struct _tuple1* _temp1859; struct _tuple1** _temp1861; void*
_temp1862; struct Cyc_Absyn_Exp* _temp1864; _LL1647: if(*(( int*) _temp1645) == 
Cyc_Absyn_NoInstantiate_e){ _LL1722: _temp1721=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1645)->f1; goto _LL1648;} else{ goto _LL1649;} _LL1649: if(*(( int*)
_temp1645) ==  Cyc_Absyn_UnknownId_e){ _LL1724: _temp1723=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1645)->f1; goto _LL1650;} else{ goto _LL1651;} _LL1651: if(*(( int*)
_temp1645) ==  Cyc_Absyn_UnknownCall_e){ _LL1728: _temp1727=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1645)->f1; goto _LL1726; _LL1726: _temp1725=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1645)->f2; goto _LL1652;} else{ goto _LL1653;} _LL1653: if(*(( int*)
_temp1645) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1732: _temp1731=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1645)->f1; goto _LL1730; _LL1730: _temp1729=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1645)->f2; goto _LL1654;} else{ goto _LL1655;} _LL1655: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Const_e){ _LL1734: _temp1733=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1645)->f1; goto _LL1656;} else{ goto _LL1657;} _LL1657: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Var_e){ _LL1738: _temp1737=(( struct Cyc_Absyn_Var_e_struct*)
_temp1645)->f1; goto _LL1736; _LL1736: _temp1735=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1645)->f2; goto _LL1658;} else{ goto _LL1659;} _LL1659: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Primop_e){ _LL1742: _temp1741=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1645)->f1; goto _LL1740; _LL1740: _temp1739=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1645)->f2; goto _LL1660;} else{ goto _LL1661;} _LL1661: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Increment_e){ _LL1746: _temp1745=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1645)->f1; goto _LL1744; _LL1744: _temp1743=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1645)->f2; goto _LL1662;} else{ goto _LL1663;} _LL1663: if(*(( int*)
_temp1645) ==  Cyc_Absyn_AssignOp_e){ _LL1752: _temp1751=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1645)->f1; goto _LL1750; _LL1750: _temp1749=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1645)->f2; goto _LL1748; _LL1748: _temp1747=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1645)->f3; goto _LL1664;} else{ goto _LL1665;} _LL1665: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Conditional_e){ _LL1758: _temp1757=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1645)->f1; goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1645)->f2; goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1645)->f3; goto _LL1666;} else{ goto _LL1667;} _LL1667: if(*(( int*)
_temp1645) ==  Cyc_Absyn_SeqExp_e){ _LL1762: _temp1761=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1645)->f1; goto _LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1645)->f2; goto _LL1668;} else{ goto _LL1669;} _LL1669: if(*(( int*)
_temp1645) ==  Cyc_Absyn_FnCall_e){ _LL1769: _temp1768=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1645)->f1; goto _LL1767; _LL1767: _temp1766=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1645)->f2; goto _LL1764; _LL1764: _temp1763=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1645)->f3; _temp1765=( struct Cyc_Absyn_VarargCallInfo**)&(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1645)->f3; goto _LL1670;} else{ goto _LL1671;} _LL1671: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Throw_e){ _LL1771: _temp1770=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1645)->f1; goto _LL1672;} else{ goto _LL1673;} _LL1673: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Instantiate_e){ _LL1775: _temp1774=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1645)->f1; goto _LL1773; _LL1773: _temp1772=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1645)->f2; goto _LL1674;} else{ goto _LL1675;} _LL1675: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Cast_e){ _LL1779: _temp1778=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1645)->f1; goto _LL1777; _LL1777: _temp1776=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1645)->f2; goto _LL1676;} else{ goto _LL1677;} _LL1677: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Address_e){ _LL1781: _temp1780=(( struct Cyc_Absyn_Address_e_struct*)
_temp1645)->f1; goto _LL1678;} else{ goto _LL1679;} _LL1679: if(*(( int*)
_temp1645) ==  Cyc_Absyn_New_e){ _LL1785: _temp1784=(( struct Cyc_Absyn_New_e_struct*)
_temp1645)->f1; goto _LL1783; _LL1783: _temp1782=(( struct Cyc_Absyn_New_e_struct*)
_temp1645)->f2; goto _LL1680;} else{ goto _LL1681;} _LL1681: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Sizeofexp_e){ _LL1787: _temp1786=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1645)->f1; goto _LL1682;} else{ goto _LL1683;} _LL1683: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1789: _temp1788=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1645)->f1; goto _LL1684;} else{ goto _LL1685;} _LL1685: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Offsetof_e){ _LL1793: _temp1792=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1645)->f1; goto _LL1791; _LL1791: _temp1790=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1645)->f2; goto _LL1686;} else{ goto _LL1687;} _LL1687: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Gentyp_e){ goto _LL1688;} else{ goto _LL1689;} _LL1689:
if(*(( int*) _temp1645) ==  Cyc_Absyn_Deref_e){ _LL1795: _temp1794=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1645)->f1; goto _LL1690;} else{ goto _LL1691;} _LL1691: if(*(( int*)
_temp1645) ==  Cyc_Absyn_StructMember_e){ _LL1799: _temp1798=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1645)->f1; goto _LL1797; _LL1797: _temp1796=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1645)->f2; goto _LL1692;} else{ goto _LL1693;} _LL1693: if(*(( int*)
_temp1645) ==  Cyc_Absyn_StructArrow_e){ _LL1803: _temp1802=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1645)->f1; goto _LL1801; _LL1801: _temp1800=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1645)->f2; goto _LL1694;} else{ goto _LL1695;} _LL1695: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Subscript_e){ _LL1807: _temp1806=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1645)->f1; goto _LL1805; _LL1805: _temp1804=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1645)->f2; goto _LL1696;} else{ goto _LL1697;} _LL1697: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Tuple_e){ _LL1809: _temp1808=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1645)->f1; goto _LL1698;} else{ goto _LL1699;} _LL1699: if(*(( int*)
_temp1645) ==  Cyc_Absyn_CompoundLit_e){ _LL1813: _temp1812=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1645)->f1; goto _LL1811; _LL1811: _temp1810=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1645)->f2; goto _LL1700;} else{ goto _LL1701;} _LL1701: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Array_e){ _LL1815: _temp1814=(( struct Cyc_Absyn_Array_e_struct*)
_temp1645)->f1; goto _LL1702;} else{ goto _LL1703;} _LL1703: if(*(( int*)
_temp1645) ==  Cyc_Absyn_StmtExp_e){ _LL1817: _temp1816=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1645)->f1; goto _LL1704;} else{ goto _LL1705;} _LL1705: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Codegen_e){ _LL1819: _temp1818=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1645)->f1; goto _LL1706;} else{ goto _LL1707;} _LL1707: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Fill_e){ _LL1821: _temp1820=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1645)->f1; goto _LL1708;} else{ goto _LL1709;} _LL1709: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Comprehension_e){ _LL1827: _temp1826=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1645)->f1; goto _LL1825; _LL1825: _temp1824=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1645)->f2; goto _LL1823; _LL1823: _temp1822=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1645)->f3; goto _LL1710;} else{ goto _LL1711;} _LL1711: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Struct_e){ _LL1837: _temp1836=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1645)->f1; _temp1838=( struct _tuple1**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1645)->f1; goto _LL1834; _LL1834: _temp1833=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1645)->f2; _temp1835=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1645)->f2; goto _LL1832; _LL1832: _temp1831=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1645)->f3; goto _LL1829; _LL1829: _temp1828=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1645)->f4; _temp1830=( struct Cyc_Absyn_Structdecl**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1645)->f4; goto _LL1712;} else{ goto _LL1713;} _LL1713: if(*(( int*)
_temp1645) ==  Cyc_Absyn_AnonStruct_e){ _LL1842: _temp1841=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1645)->f1; goto _LL1840; _LL1840: _temp1839=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1645)->f2; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Tunion_e){ _LL1853: _temp1852=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1645)->f1; _temp1854=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1645)->f1; goto _LL1850; _LL1850: _temp1849=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1645)->f2; _temp1851=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1645)->f2; goto _LL1848; _LL1848: _temp1847=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1645)->f3; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1645)->f4; goto _LL1844; _LL1844: _temp1843=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1645)->f5; goto _LL1716;} else{ goto _LL1717;} _LL1717: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Enum_e){ _LL1860: _temp1859=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1645)->f1; _temp1861=( struct _tuple1**)&(( struct Cyc_Absyn_Enum_e_struct*)
_temp1645)->f1; goto _LL1858; _LL1858: _temp1857=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1645)->f2; goto _LL1856; _LL1856: _temp1855=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1645)->f3; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(*(( int*)
_temp1645) ==  Cyc_Absyn_Malloc_e){ _LL1865: _temp1864=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1645)->f1; goto _LL1863; _LL1863: _temp1862=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1645)->f2; goto _LL1720;} else{ goto _LL1646;} _LL1648: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1721); return; _LL1650: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1723); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1652: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1727, _temp1725); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1654: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1729); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1656: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1733, e);
goto _LL1646; _LL1658: t= Cyc_Tcexp_tcVar( te, loc, _temp1737, _temp1735); goto
_LL1646; _LL1660: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1741, _temp1739);
goto _LL1646; _LL1662: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1745,
_temp1743); goto _LL1646; _LL1664: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1751, _temp1749, _temp1747); goto _LL1646; _LL1666: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1757, _temp1755, _temp1753); goto _LL1646; _LL1668: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1761, _temp1759); goto _LL1646; _LL1670: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1768, _temp1766, _temp1765); goto _LL1646; _LL1672: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1770); goto _LL1646; _LL1674: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1774, _temp1772); goto _LL1646; _LL1676: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1778, _temp1776); goto _LL1646; _LL1678: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1780); goto _LL1646; _LL1680: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1784, e, _temp1782); goto _LL1646; _LL1682: { void* _temp1866= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1786); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1866); goto
_LL1646;} _LL1684: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1788); goto
_LL1646; _LL1686: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1792, _temp1790);
goto _LL1646; _LL1688:({ void* _temp1867[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen() not in top-level initializer",
sizeof( unsigned char), 35u), _tag_arr( _temp1867, sizeof( void*), 0u));});
return; _LL1690: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1794); goto _LL1646;
_LL1692: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1798, _temp1796);
goto _LL1646; _LL1694: t= Cyc_Tcexp_tcStructArrow( te, loc, topt, _temp1802,
_temp1800); goto _LL1646; _LL1696: t= Cyc_Tcexp_tcSubscript( te, loc, topt,
_temp1806, _temp1804); goto _LL1646; _LL1698: t= Cyc_Tcexp_tcTuple( te, loc,
topt, _temp1808); goto _LL1646; _LL1700: t= Cyc_Tcexp_tcCompoundLit( te, loc,
topt, _temp1812, _temp1810); goto _LL1646; _LL1702: { void** elt_topt= 0; if(
topt !=  0){ void* _temp1868= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
void* _temp1874; void** _temp1876; _LL1870: if(( unsigned int) _temp1868 >  4u?*((
int*) _temp1868) ==  Cyc_Absyn_ArrayType: 0){ _LL1875: _temp1874=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1868)->f1; _temp1876=( void**)&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1868)->f1); goto _LL1871;} else{ goto
_LL1872;} _LL1872: goto _LL1873; _LL1871: elt_topt=( void**) _temp1876; goto
_LL1869; _LL1873: goto _LL1869; _LL1869:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1814); goto _LL1646;} _LL1704: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1816); goto _LL1646; _LL1706: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1818); goto _LL1646; _LL1708: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1820);
goto _LL1646; _LL1710: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1826,
_temp1824, _temp1822); goto _LL1646; _LL1712: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1838, _temp1835, _temp1831, _temp1830); goto _LL1646; _LL1714: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1841, _temp1839); goto _LL1646; _LL1716: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1854, _temp1851, _temp1847, _temp1845, _temp1843); goto
_LL1646; _LL1718:* _temp1861=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1855))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1877=(
struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1877[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1878; _temp1878.tag= Cyc_Absyn_EnumType;
_temp1878.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1857))->name;
_temp1878.f2= _temp1857; _temp1878;}); _temp1877;}); goto _LL1646; _LL1720: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1864, _temp1862); goto _LL1646; _LL1646:;}
e->topt=({ struct Cyc_Core_Opt* _temp1879=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1879->v=( void*) t; _temp1879;});}
