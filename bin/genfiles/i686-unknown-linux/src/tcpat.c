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
struct _tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct
_tuple0*); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int), void*
env); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
s); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern
unsigned char Cyc_Set_Absent[ 11u]; extern int Cyc_Std_strcmp( struct
_tagged_arr s1, struct _tagged_arr s2); extern int Cyc_Std_zstrcmp( struct
_tagged_arr, struct _tagged_arr); extern int Cyc_Std_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern unsigned char Cyc_Position_Nocontext[ 14u]; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; struct _tuple1{ void* f1; struct
_tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static const int Cyc_Absyn_Static=
0; static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
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
} ; static const int Cyc_Absyn_No_constr= 0; static const int Cyc_Absyn_Eq_kb= 0;
struct Cyc_Absyn_Eq_kb_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Unknown_kb=
1; struct Cyc_Absyn_Unknown_kb_struct{ int tag; struct Cyc_Core_Opt* f1; } ;
static const int Cyc_Absyn_Less_kb= 2; struct Cyc_Absyn_Less_kb_struct{ int tag;
struct Cyc_Core_Opt* f1; void* f2; } ; struct Cyc_Absyn_Tvar{ struct _tagged_arr*
name; int* identity; void* kind; } ; static const int Cyc_Absyn_Unknown_b= 0;
static const int Cyc_Absyn_Upper_b= 0; struct Cyc_Absyn_Upper_b_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
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
const int Cyc_Absyn_DoubleType= 6; struct Cyc_Absyn_DoubleType_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_ArrayType= 7; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ;
static const int Cyc_Absyn_FnType= 8; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 9; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static const
int Cyc_Absyn_StructType= 10; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple1* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
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
4; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
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
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s=
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
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_UnknownCall_p= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern void* Cyc_Absyn_compress_kb( void*); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl( struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init);
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_ShortPtr_sa= 0; struct
Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
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
print_externC_stmts: 1; int print_full_evars: 1; int generate_line_directives: 1;
int use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern
struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*);
extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress( void* t); extern
int Cyc_Tcutil_coerceable( void*); extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak; extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_List_List*
f1; struct _RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_arr err_msg); extern struct
Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ;
extern struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple7{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0)
==  Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 >  2u?*((
int*) _temp0) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f1; goto _LL15; _LL15: _temp14=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f2; goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 >  2u?*((
int*) _temp0) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f1; goto _LL21; _LL21: _temp20=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Tunionfield* _temp39; struct Cyc_Absyn_Tuniondecl*
_temp41; struct Cyc_Absyn_Enumfield* _temp43; struct Cyc_Absyn_Enumdecl* _temp45;
struct Cyc_Absyn_Enumfield* _temp47; void* _temp49; _LL29: if(*(( int*) _temp27)
==  Cyc_Tcenv_StructRes){ goto _LL30;} else{ goto _LL31;} _LL31: if(*(( int*)
_temp27) ==  Cyc_Tcenv_TunionRes){ _LL42: _temp41=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f1; goto _LL40; _LL40: _temp39=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(*(( int*) _temp27) == 
Cyc_Tcenv_EnumRes){ _LL46: _temp45=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f1;
goto _LL44; _LL44: _temp43=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f2;
goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp27) ==  Cyc_Tcenv_AnonEnumRes){
_LL50: _temp49=( void*)(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp27)->f1;
goto _LL48; _LL48: _temp47=(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp27)->f2;
goto _LL36;} else{ goto _LL37;} _LL37: if(*(( int*) _temp27) ==  Cyc_Tcenv_VarRes){
goto _LL38;} else{ goto _LL28;} _LL30:({ void* _temp51[ 0u]={}; Cyc_Tcutil_terr(
p->loc, _tag_arr("struct tag used without arguments in pattern", sizeof(
unsigned char), 45u), _tag_arr( _temp51, sizeof( void*), 0u));}); _npop_handler(
0u); return; _LL32:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp52=( struct Cyc_Absyn_Tunion_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp52[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp53; _temp53.tag= Cyc_Absyn_Tunion_p;
_temp53.f1= _temp41; _temp53.f2= _temp39; _temp53.f3= 0; _temp53.f4= 0; _temp53;});
_temp52;}))); _npop_handler( 0u); return; _LL34:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_p_struct* _temp54=( struct Cyc_Absyn_Enum_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_p_struct)); _temp54[ 0]=({ struct Cyc_Absyn_Enum_p_struct
_temp55; _temp55.tag= Cyc_Absyn_Enum_p; _temp55.f1= _temp45; _temp55.f2= _temp43;
_temp55;}); _temp54;}))); _npop_handler( 0u); return; _LL36:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_AnonEnum_p_struct* _temp56=( struct Cyc_Absyn_AnonEnum_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_p_struct)); _temp56[ 0]=({ struct
Cyc_Absyn_AnonEnum_p_struct _temp57; _temp57.tag= Cyc_Absyn_AnonEnum_p; _temp57.f1=(
void*) _temp49; _temp57.f2= _temp47; _temp57;}); _temp56;}))); _npop_handler( 0u);
return; _LL38: goto _LL28; _LL28:;}; _pop_handler();} else{ void* _temp25=( void*)
_exn_thrown; void* _temp59= _temp25; _LL61: if( _temp59 ==  Cyc_Dict_Absent){
goto _LL62;} else{ goto _LL63;} _LL63: goto _LL64; _LL62: goto _LL60; _LL64:(
void) _throw( _temp59); _LL60:;}}}{ void* _temp65=(* _temp10).f1; struct Cyc_List_List*
_temp73; _LL67: if( _temp65 == ( void*) Cyc_Absyn_Loc_n){ goto _LL68;} else{
goto _LL69;} _LL69: if(( unsigned int) _temp65 >  1u?*(( int*) _temp65) ==  Cyc_Absyn_Rel_n:
0){ _LL74: _temp73=(( struct Cyc_Absyn_Rel_n_struct*) _temp65)->f1; if( _temp73
==  0){ goto _LL70;} else{ goto _LL71;}} else{ goto _LL71;} _LL71: goto _LL72;
_LL68: goto _LL70; _LL70:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp75=( struct Cyc_Absyn_Var_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp75[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp76; _temp76.tag= Cyc_Absyn_Var_p; _temp76.f1= Cyc_Absyn_new_vardecl(
_temp10,( void*) Cyc_Absyn_VoidType, 0); _temp76;}); _temp75;}))); return; _LL72:({
void* _temp77[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u), _tag_arr( _temp77, sizeof( void*), 0u));});
return; _LL66:;} _LL5:{ struct _handler_cons _temp78; _push_handler(& _temp78);{
int _temp80= 0; if( setjmp( _temp78.handler)){ _temp80= 1;} if( ! _temp80){{
void* _temp81= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp16); struct Cyc_Absyn_Structdecl*
_temp93; struct Cyc_Absyn_Tunionfield* _temp95; struct Cyc_Absyn_Tuniondecl*
_temp97; _LL83: if(*(( int*) _temp81) ==  Cyc_Tcenv_StructRes){ _LL94: _temp93=((
struct Cyc_Tcenv_StructRes_struct*) _temp81)->f1; goto _LL84;} else{ goto _LL85;}
_LL85: if(*(( int*) _temp81) ==  Cyc_Tcenv_TunionRes){ _LL98: _temp97=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp81)->f1; goto _LL96; _LL96: _temp95=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp81)->f2; goto _LL86;} else{ goto _LL87;} _LL87:
if(*(( int*) _temp81) ==  Cyc_Tcenv_EnumRes){ goto _LL88;} else{ goto _LL89;}
_LL89: if(*(( int*) _temp81) ==  Cyc_Tcenv_AnonEnumRes){ goto _LL90;} else{ goto
_LL91;} _LL91: if(*(( int*) _temp81) ==  Cyc_Tcenv_VarRes){ goto _LL92;} else{
goto _LL82;} _LL84: { struct Cyc_List_List* _temp99= 0; for( 0; _temp12 !=  0;
_temp12= _temp12->tl){ _temp99=({ struct Cyc_List_List* _temp100=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp100->hd=( void*)({ struct
_tuple7* _temp101=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp101->f1= 0; _temp101->f2=( struct Cyc_Absyn_Pat*) _temp12->hd; _temp101;});
_temp100->tl= _temp99; _temp100;});}( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp102=( struct Cyc_Absyn_Struct_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp102[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp103; _temp103.tag= Cyc_Absyn_Struct_p; _temp103.f1=
_temp93; _temp103.f2= 0; _temp103.f3= _temp14; _temp103.f4=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp99); _temp103;}); _temp102;})));
_npop_handler( 0u); return;} _LL86:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp104=( struct Cyc_Absyn_Tunion_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp104[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp105; _temp105.tag= Cyc_Absyn_Tunion_p;
_temp105.f1= _temp97; _temp105.f2= _temp95; _temp105.f3= _temp14; _temp105.f4=
_temp12; _temp105;}); _temp104;}))); _npop_handler( 0u); return; _LL88: goto
_LL90; _LL90:({ struct Cyc_Std_String_pa_struct _temp107; _temp107.tag= Cyc_Std_String_pa;
_temp107.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp16);{ void*
_temp106[ 1u]={& _temp107}; Cyc_Tcutil_terr( p->loc, _tag_arr("bad enum constructor %s in pattern",
sizeof( unsigned char), 35u), _tag_arr( _temp106, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL92: goto _LL82; _LL82:;}; _pop_handler();} else{
void* _temp79=( void*) _exn_thrown; void* _temp109= _temp79; _LL111: if(
_temp109 ==  Cyc_Dict_Absent){ goto _LL112;} else{ goto _LL113;} _LL113: goto
_LL114; _LL112: goto _LL110; _LL114:( void) _throw( _temp109); _LL110:;}}}({
struct Cyc_Std_String_pa_struct _temp116; _temp116.tag= Cyc_Std_String_pa;
_temp116.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp16);{ void*
_temp115[ 1u]={& _temp116}; Cyc_Tcutil_terr( p->loc, _tag_arr("%s is not a constructor in pattern",
sizeof( unsigned char), 35u), _tag_arr( _temp115, sizeof( void*), 1u));}});
return; _LL7:{ struct _handler_cons _temp117; _push_handler(& _temp117);{ int
_temp119= 0; if( setjmp( _temp117.handler)){ _temp119= 1;} if( ! _temp119){{
void* _temp120= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp22); struct Cyc_Absyn_Structdecl*
_temp126; _LL122: if(*(( int*) _temp120) ==  Cyc_Tcenv_StructRes){ _LL127:
_temp126=(( struct Cyc_Tcenv_StructRes_struct*) _temp120)->f1; goto _LL123;}
else{ goto _LL124;} _LL124: goto _LL125; _LL123:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_p_struct* _temp128=( struct Cyc_Absyn_Struct_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp128[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp129; _temp129.tag= Cyc_Absyn_Struct_p; _temp129.f1=
_temp126; _temp129.f2= 0; _temp129.f3= _temp20; _temp129.f4= _temp18; _temp129;});
_temp128;}))); _npop_handler( 0u); return; _LL125: goto _LL121; _LL121:;};
_pop_handler();} else{ void* _temp118=( void*) _exn_thrown; void* _temp131=
_temp118; _LL133: if( _temp131 ==  Cyc_Dict_Absent){ goto _LL134;} else{ goto
_LL135;} _LL135: goto _LL136; _LL134: goto _LL132; _LL136:( void) _throw(
_temp131); _LL132:;}}}({ struct Cyc_Std_String_pa_struct _temp138; _temp138.tag=
Cyc_Std_String_pa; _temp138.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp22);{ void* _temp137[ 1u]={& _temp138}; Cyc_Tcutil_terr( p->loc, _tag_arr("%s is not a struct name, but pattern has designators",
sizeof( unsigned char), 53u), _tag_arr( _temp137, sizeof( void*), 1u));}});
return; _LL9: return; _LL1:;} static struct _tagged_arr* Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl* vd){ return(* vd->name).f2;} static void* Cyc_Tcpat_any_type(
struct Cyc_List_List* s, void** topt){ if( topt !=  0){ return* topt;} return
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({ struct Cyc_Core_Opt*
_temp139=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp139->v=( void*) s; _temp139;}));} static void* Cyc_Tcpat_num_type( void**
topt, void* numt){ if( topt !=  0? Cyc_Tcutil_coerceable(* topt): 0){ return*
topt;}{ void* _temp140= Cyc_Tcutil_compress( numt); _LL142: if(( unsigned int)
_temp140 >  3u?*(( int*) _temp140) ==  Cyc_Absyn_EnumType: 0){ goto _LL143;}
else{ goto _LL144;} _LL144: goto _LL145; _LL143: if( topt !=  0){ return* topt;}
goto _LL141; _LL145: goto _LL141; _LL141:;} return numt;} static void Cyc_Tcpat_set_vd(
struct Cyc_Absyn_Vardecl* vd, struct Cyc_List_List** v_result_ptr, void* t){(
void*)( vd->type=( void*) t); vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp146=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp146->hd=( void*) vd; _temp146->tl=*
v_result_ptr; _temp146;});} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat* f2;
} ; struct _tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp147=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp183; struct Cyc_Absyn_Vardecl*
_temp185; void* _temp187; void* _temp189; struct Cyc_Absyn_Enumfield* _temp191;
struct Cyc_Absyn_Enumdecl* _temp193; struct Cyc_Absyn_Enumfield* _temp195; void*
_temp197; struct Cyc_Absyn_Pat* _temp199; struct Cyc_List_List* _temp201; struct
Cyc_List_List* _temp203; struct Cyc_List_List* _temp205; struct Cyc_Core_Opt*
_temp207; struct Cyc_Core_Opt** _temp209; struct Cyc_Absyn_Structdecl* _temp210;
struct Cyc_List_List* _temp212; struct Cyc_List_List* _temp214; struct Cyc_Absyn_Tunionfield*
_temp216; struct Cyc_Absyn_Tuniondecl* _temp218; _LL149: if( _temp147 == ( void*)
Cyc_Absyn_Wild_p){ goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Var_p: 0){ _LL184: _temp183=((
struct Cyc_Absyn_Var_p_struct*) _temp147)->f1; goto _LL152;} else{ goto _LL153;}
_LL153: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Reference_p:
0){ _LL186: _temp185=(( struct Cyc_Absyn_Reference_p_struct*) _temp147)->f1;
goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Int_p: 0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp147)->f1; if( _temp187 == ( void*) Cyc_Absyn_Unsigned){ goto _LL156;} else{
goto _LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Int_p: 0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp147)->f1; if( _temp189 == ( void*) Cyc_Absyn_Signed){ goto _LL158;} else{
goto _LL159;}} else{ goto _LL159;} _LL159: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Char_p: 0){ goto _LL160;} else{ goto _LL161;}
_LL161: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Float_p:
0){ goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Enum_p: 0){ _LL194: _temp193=(( struct Cyc_Absyn_Enum_p_struct*)
_temp147)->f1; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_Enum_p_struct*)
_temp147)->f2; goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL198:
_temp197=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp147)->f1; goto
_LL196; _LL196: _temp195=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp147)->f2;
goto _LL166;} else{ goto _LL167;} _LL167: if( _temp147 == ( void*) Cyc_Absyn_Null_p){
goto _LL168;} else{ goto _LL169;} _LL169: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Pointer_p: 0){ _LL200: _temp199=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp147)->f1; goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Tuple_p: 0){ _LL202: _temp201=((
struct Cyc_Absyn_Tuple_p_struct*) _temp147)->f1; goto _LL172;} else{ goto _LL173;}
_LL173: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Struct_p:
0){ _LL211: _temp210=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f1; goto
_LL208; _LL208: _temp207=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f2;
_temp209=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f2;
goto _LL206; _LL206: _temp205=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f3;
goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f4;
goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Tunion_p: 0){ _LL219: _temp218=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f1; goto _LL217; _LL217: _temp216=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f2; goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f3; goto _LL213; _LL213: _temp212=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f4; goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL178;}
else{ goto _LL179;} _LL179: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147)
==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL180;} else{ goto _LL181;} _LL181: if((
unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL182;} else{ goto _LL148;} _LL150: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); goto _LL148; _LL152: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); Cyc_Tcpat_set_vd( _temp183,& v_result, t); goto _LL148; _LL154: t=
Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te), topt); if( region_opt ==  0){({
void* _temp220[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u), _tag_arr( _temp220, sizeof( void*), 0u));}); goto
_LL148;} Cyc_Tcpat_set_vd( _temp185,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp221=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp221[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp222; _temp222.tag= Cyc_Absyn_PointerType;
_temp222.f1=({ struct Cyc_Absyn_PtrInfo _temp223; _temp223.elt_typ=( void*) t;
_temp223.rgn_typ=( void*)* region_opt; _temp223.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp223.tq= Cyc_Absyn_empty_tqual();
_temp223.bounds= Cyc_Absyn_empty_conref(); _temp223;}); _temp222;}); _temp221;}));
goto _LL148; _LL156: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uint_t); goto _LL148;
_LL158: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t); goto _LL148; _LL160: t=
Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL148; _LL162: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_float_t); goto _LL148; _LL164: t= Cyc_Tcpat_num_type( topt,(
void*)({ struct Cyc_Absyn_EnumType_struct* _temp224=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp224[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp225; _temp225.tag= Cyc_Absyn_EnumType; _temp225.f1=
_temp193->name; _temp225.f2=( struct Cyc_Absyn_Enumdecl*) _temp193; _temp225;});
_temp224;})); goto _LL148; _LL166: t= Cyc_Tcpat_num_type( topt, _temp197); goto
_LL148; _LL168: if( topt !=  0){ void* _temp226= Cyc_Tcutil_compress(* topt);
_LL228: if(( unsigned int) _temp226 >  3u?*(( int*) _temp226) ==  Cyc_Absyn_PointerType:
0){ goto _LL229;} else{ goto _LL230;} _LL230: goto _LL231; _LL229: t=* topt;
goto tcpat_end; _LL231: goto _LL227; _LL227:;}{ struct Cyc_List_List* _temp232=
Cyc_Tcenv_lookup_type_vars( te); t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp233=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp234; _temp234.tag= Cyc_Absyn_PointerType;
_temp234.f1=({ struct Cyc_Absyn_PtrInfo _temp235; _temp235.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt* _temp237=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp237->v=( void*)
_temp232; _temp237;})); _temp235.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp236=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=( void*) _temp232;
_temp236;})); _temp235.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp235.tq= Cyc_Absyn_empty_tqual(); _temp235.bounds= Cyc_Absyn_empty_conref();
_temp235;}); _temp234;}); _temp233;}); goto _LL148;} _LL170: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** _temp238= 0; if( topt !=  0){ void* _temp239=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp245; void* _temp247;
_LL241: if(( unsigned int) _temp239 >  3u?*(( int*) _temp239) ==  Cyc_Absyn_PointerType:
0){ _LL246: _temp245=(( struct Cyc_Absyn_PointerType_struct*) _temp239)->f1;
_LL248: _temp247=( void*) _temp245.elt_typ; goto _LL242;} else{ goto _LL243;}
_LL243: goto _LL244; _LL242: inner_typ= _temp247; _temp238=( void**)& inner_typ;
goto _LL240; _LL244: goto _LL240; _LL240:;}{ void* ptr_rgn= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp283=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp283->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp283;})); struct Cyc_List_List* _temp251;
struct Cyc_List_List* _temp253; struct _tuple6 _temp249= Cyc_Tcpat_tcPatRec( te,
_temp199, _temp238,( void**)& ptr_rgn); _LL254: _temp253= _temp249.f1; goto
_LL252; _LL252: _temp251= _temp249.f2; goto _LL250; _LL250: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp253); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp251); if( _temp238
==  0){ void* _temp255= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp199->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp261;
struct Cyc_List_List* _temp263; void* _temp265; struct Cyc_Absyn_Tunionfield*
_temp267; struct Cyc_Absyn_Tuniondecl* _temp269; _LL257: if(( unsigned int)
_temp255 >  3u?*(( int*) _temp255) ==  Cyc_Absyn_TunionFieldType: 0){ _LL262:
_temp261=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp255)->f1; _LL266:
_temp265=( void*) _temp261.field_info; if(*(( int*) _temp265) ==  Cyc_Absyn_KnownTunionfield){
_LL270: _temp269=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp265)->f1;
goto _LL268; _LL268: _temp267=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp265)->f2; goto _LL264;} else{ goto _LL259;} _LL264: _temp263= _temp261.targs;
goto _LL258;} else{ goto _LL259;} _LL259: goto _LL260; _LL258: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp271=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp271[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp272; _temp272.tag= Cyc_Absyn_TunionType;
_temp272.f1=({ struct Cyc_Absyn_TunionInfo _temp273; _temp273.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp274=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp274[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp275; _temp275.tag= Cyc_Absyn_KnownTunion;
_temp275.f1=({ struct Cyc_Absyn_Tuniondecl** _temp276=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp276[ 0]= _temp269;
_temp276;}); _temp275;}); _temp274;})); _temp273.targs= _temp263; _temp273.rgn=(
void*) ptr_rgn; _temp273;}); _temp272;}); _temp271;}); goto _LL256; _LL260: t=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp277=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp277[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp278; _temp278.tag= Cyc_Absyn_PointerType;
_temp278.f1=({ struct Cyc_Absyn_PtrInfo _temp279; _temp279.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp199->topt))->v); _temp279.rgn_typ=(
void*) ptr_rgn; _temp279.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp279.tq= Cyc_Absyn_empty_tqual(); _temp279.bounds= Cyc_Absyn_empty_conref();
_temp279;}); _temp278;}); _temp277;}); goto _LL256; _LL256:;} else{ t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp280=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp280[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp281; _temp281.tag= Cyc_Absyn_PointerType;
_temp281.f1=({ struct Cyc_Absyn_PtrInfo _temp282; _temp282.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp199->topt))->v); _temp282.rgn_typ=(
void*) ptr_rgn; _temp282.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp282.tq= Cyc_Absyn_empty_tqual(); _temp282.bounds= Cyc_Absyn_empty_conref();
_temp282;}); _temp281;}); _temp280;});} goto _LL148;}} _LL172: { struct Cyc_List_List*
pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if( topt !=  0){ void* _temp284=
Cyc_Tcutil_compress(* topt); struct Cyc_List_List* _temp290; _LL286: if((
unsigned int) _temp284 >  3u?*(( int*) _temp284) ==  Cyc_Absyn_TupleType: 0){
_LL291: _temp290=(( struct Cyc_Absyn_TupleType_struct*) _temp284)->f1; goto
_LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL287: topt_ts= _temp290;
goto _LL285; _LL289: goto _LL285; _LL285:;} for( 0; _temp201 !=  0; _temp201=
_temp201->tl){ void** _temp292= 0; if( topt_ts !=  0){ _temp292=( void**)&(*((
struct _tuple8*) topt_ts->hd)).f2; topt_ts= topt_ts->tl;}{ struct Cyc_List_List*
_temp295; struct Cyc_List_List* _temp297; struct _tuple6 _temp293= Cyc_Tcpat_tcPatRec(
te,( struct Cyc_Absyn_Pat*) _temp201->hd, _temp292, region_opt); _LL298:
_temp297= _temp293.f1; goto _LL296; _LL296: _temp295= _temp293.f2; goto _LL294;
_LL294: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp297); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp295); pat_ts=({ struct Cyc_List_List* _temp299=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp299->hd=( void*)({ struct
_tuple8* _temp300=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp300->f1= Cyc_Absyn_empty_tqual(); _temp300->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Pat*) _temp201->hd)->topt))->v; _temp300;});
_temp299->tl= pat_ts; _temp299;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp301=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp301[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp302; _temp302.tag= Cyc_Absyn_TupleType;
_temp302.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp302;}); _temp301;}); goto _LL148;} _LL174: if( _temp205 !=  0){(
int) _throw(({ void* _temp303[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcPat: struct<...> not implemented",
sizeof( unsigned char), 35u), _tag_arr( _temp303, sizeof( void*), 0u));}));}{
struct _RegionHandle _temp304= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp304; _push_region( rgn);{ struct _tuple4 _temp305=({ struct _tuple4
_temp331; _temp331.f1= Cyc_Tcenv_lookup_type_vars( te); _temp331.f2= rgn;
_temp331;}); struct Cyc_List_List* _temp306=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp305, _temp210->tvs); struct Cyc_List_List* _temp307=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp306); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp308=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp308[ 0]=({ struct Cyc_Absyn_StructType_struct _temp309; _temp309.tag= Cyc_Absyn_StructType;
_temp309.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp210->name))->v); _temp309.f2= _temp307; _temp309.f3=({ struct
Cyc_Absyn_Structdecl** _temp310=( struct Cyc_Absyn_Structdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp310[ 0]= _temp210; _temp310;});
_temp309;}); _temp308;}); if( topt !=  0){ Cyc_Tcutil_unify( t,* topt);}*
_temp209=({ struct Cyc_Core_Opt* _temp311=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp311->v=( void*) _temp307; _temp311;}); if(
_temp210->fields ==  0){({ void* _temp312[ 0u]={}; Cyc_Tcutil_terr( p->loc,
_tag_arr("can't destructure an abstract struct", sizeof( unsigned char), 37u),
_tag_arr( _temp312, sizeof( void*), 0u));}); _npop_handler( 0u); goto _LL148;}{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle*
rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc, _temp203,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp210->fields))->v);
for( 0; fields !=  0; fields= fields->tl){ struct _tuple9 _temp315; struct Cyc_Absyn_Pat*
_temp316; struct Cyc_Absyn_Structfield* _temp318; struct _tuple9* _temp313=(
struct _tuple9*) fields->hd; _temp315=* _temp313; _LL319: _temp318= _temp315.f1;
goto _LL317; _LL317: _temp316= _temp315.f2; goto _LL314; _LL314: { void*
_temp320= Cyc_Tcutil_rsubstitute( rgn, _temp306,( void*) _temp318->type); struct
Cyc_List_List* _temp323; struct Cyc_List_List* _temp325; struct _tuple6 _temp321=
Cyc_Tcpat_tcPatRec( te, _temp316,( void**)& _temp320, region_opt); _LL326:
_temp325= _temp321.f1; goto _LL324; _LL324: _temp323= _temp321.f2; goto _LL322;
_LL322: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp325); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp323); if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp316->topt))->v, _temp320)){({ struct Cyc_Std_String_pa_struct
_temp330; _temp330.tag= Cyc_Std_String_pa; _temp330.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp316->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp329; _temp329.tag= Cyc_Std_String_pa; _temp329.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp320);{ struct Cyc_Std_String_pa_struct _temp328; _temp328.tag= Cyc_Std_String_pa;
_temp328.f1=( struct _tagged_arr)* _temp318->name;{ void* _temp327[ 3u]={&
_temp328,& _temp329,& _temp330}; Cyc_Tcutil_terr( p->loc, _tag_arr("field %s of struct pattern expects type %s != %s",
sizeof( unsigned char), 49u), _tag_arr( _temp327, sizeof( void*), 3u));}}}});}}}}};
_pop_region( rgn);} goto _LL148; _LL176: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp214) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp216->tvs)){({ void* _temp332[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u), _tag_arr( _temp332, sizeof( void*), 0u));});} if(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp214) !=  0){ region_opt=
0;}{ struct _RegionHandle _temp333= _new_region("rgn"); struct _RegionHandle*
rgn=& _temp333; _push_region( rgn);{ struct Cyc_List_List* _temp334= 0;{ struct
Cyc_List_List* t= _temp214; for( 0; t !=  0; t= t->tl){ struct Cyc_Absyn_Tvar*
tv=( struct Cyc_Absyn_Tvar*) t->hd;{ void* _temp335= Cyc_Absyn_compress_kb((
void*) tv->kind); void* _temp349; struct Cyc_Core_Opt* _temp351; struct Cyc_Core_Opt**
_temp353; void* _temp354; struct Cyc_Core_Opt* _temp356; struct Cyc_Core_Opt**
_temp358; void* _temp359; struct Cyc_Core_Opt* _temp361; struct Cyc_Core_Opt**
_temp363; void* _temp364; void* _temp366; _LL337: if(*(( int*) _temp335) ==  Cyc_Absyn_Eq_kb){
_LL350: _temp349=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp335)->f1; if(
_temp349 == ( void*) Cyc_Absyn_BoxKind){ goto _LL338;} else{ goto _LL339;}}
else{ goto _LL339;} _LL339: if(*(( int*) _temp335) ==  Cyc_Absyn_Unknown_kb){
_LL352: _temp351=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp335)->f1; _temp353=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*) _temp335)->f1;
goto _LL340;} else{ goto _LL341;} _LL341: if(*(( int*) _temp335) ==  Cyc_Absyn_Less_kb){
_LL357: _temp356=(( struct Cyc_Absyn_Less_kb_struct*) _temp335)->f1; _temp358=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp335)->f1; goto
_LL355; _LL355: _temp354=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp335)->f2;
if( _temp354 == ( void*) Cyc_Absyn_MemKind){ goto _LL342;} else{ goto _LL343;}}
else{ goto _LL343;} _LL343: if(*(( int*) _temp335) ==  Cyc_Absyn_Less_kb){
_LL362: _temp361=(( struct Cyc_Absyn_Less_kb_struct*) _temp335)->f1; _temp363=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp335)->f1; goto
_LL360; _LL360: _temp359=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp335)->f2;
if( _temp359 == ( void*) Cyc_Absyn_AnyKind){ goto _LL344;} else{ goto _LL345;}}
else{ goto _LL345;} _LL345: if(*(( int*) _temp335) ==  Cyc_Absyn_Eq_kb){ _LL365:
_temp364=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp335)->f1; goto _LL346;}
else{ goto _LL347;} _LL347: if(*(( int*) _temp335) ==  Cyc_Absyn_Less_kb){
_LL367: _temp366=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp335)->f2;
goto _LL348;} else{ goto _LL336;} _LL338: goto _LL336; _LL340: _temp358=
_temp353; goto _LL342; _LL342: _temp363= _temp358; goto _LL344; _LL344:*
_temp363=({ struct Cyc_Core_Opt* _temp368=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp368->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp369=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp369[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp370; _temp370.tag= Cyc_Absyn_Eq_kb;
_temp370.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp370;}); _temp369;}));
_temp368;}); goto _LL336; _LL346: _temp366= _temp364; goto _LL348; _LL348:({
struct Cyc_Std_String_pa_struct _temp374; _temp374.tag= Cyc_Std_String_pa;
_temp374.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp366);{ struct
Cyc_Std_String_pa_struct _temp373; _temp373.tag= Cyc_Std_String_pa; _temp373.f1=(
struct _tagged_arr)* tv->name;{ struct Cyc_Std_String_pa_struct _temp372;
_temp372.tag= Cyc_Std_String_pa; _temp372.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp218->name);{ void* _temp371[ 3u]={& _temp372,& _temp373,& _temp374}; Cyc_Tcutil_terr(
p->loc, _tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof( unsigned char), 60u), _tag_arr( _temp371, sizeof( void*), 3u));}}}});
goto _LL336; _LL336:;} _temp334=({ struct Cyc_List_List* _temp375=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp375->hd=( void*)((
void*)({ struct Cyc_Absyn_VarType_struct* _temp376=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp376[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp377; _temp377.tag= Cyc_Absyn_VarType; _temp377.f1= tv; _temp377;});
_temp376;})); _temp375->tl= _temp334; _temp375;});}} _temp334= Cyc_List_imp_rev(
_temp334); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp214);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp214); struct Cyc_List_List* tqts=
_temp216->typs; struct Cyc_List_List* _temp378= Cyc_Tcenv_lookup_type_vars( te);
struct _tuple4 _temp379=({ struct _tuple4 _temp431; _temp431.f1= _temp378;
_temp431.f2= rgn; _temp431;}); struct Cyc_List_List* _temp380=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp379, _temp218->tvs); struct Cyc_List_List* _temp381=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp216->tvs, _temp334);
struct Cyc_List_List* _temp382=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp380); struct Cyc_List_List* _temp383=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp381); struct Cyc_List_List* _temp384=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp380, _temp381); if( tqts ==  0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp385=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp385[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp386; _temp386.tag= Cyc_Absyn_TunionType;
_temp386.f1=({ struct Cyc_Absyn_TunionInfo _temp387; _temp387.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp389=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp389[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp390; _temp390.tag= Cyc_Absyn_KnownTunion;
_temp390.f1=({ struct Cyc_Absyn_Tuniondecl** _temp391=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp391[ 0]= _temp218;
_temp391;}); _temp390;}); _temp389;})); _temp387.targs= _temp382; _temp387.rgn=(
void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt*
_temp388=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp388->v=( void*) _temp378; _temp388;})); _temp387;}); _temp386;}); _temp385;});}
else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp392=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp392[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp393; _temp393.tag=
Cyc_Absyn_TunionFieldType; _temp393.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp394; _temp394.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp395=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp395[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp396; _temp396.tag= Cyc_Absyn_KnownTunionfield; _temp396.f1= _temp218;
_temp396.f2= _temp216; _temp396;}); _temp395;})); _temp394.targs= _temp382;
_temp394;}); _temp393;}); _temp392;});} if( topt !=  0? tqts ==  0: 0){ void*
_temp397= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_TunionInfo _temp405;
struct Cyc_List_List* _temp407; _LL399: if(( unsigned int) _temp397 >  3u?*((
int*) _temp397) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL400;} else{ goto
_LL401;} _LL401: if(( unsigned int) _temp397 >  3u?*(( int*) _temp397) ==  Cyc_Absyn_TunionType:
0){ _LL406: _temp405=(( struct Cyc_Absyn_TunionType_struct*) _temp397)->f1;
_LL408: _temp407= _temp405.targs; goto _LL402;} else{ goto _LL403;} _LL403: goto
_LL404; _LL400: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp409=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp409[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp410; _temp410.tag=
Cyc_Absyn_TunionFieldType; _temp410.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp411; _temp411.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp412=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp412[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp413; _temp413.tag= Cyc_Absyn_KnownTunionfield; _temp413.f1= _temp218;
_temp413.f2= _temp216; _temp413;}); _temp412;})); _temp411.targs= _temp382;
_temp411;}); _temp410;}); _temp409;}); goto _LL398; _LL402: { struct Cyc_List_List*
_temp414= _temp382; for( 0; _temp414 !=  0? _temp407 !=  0: 0;( _temp414=
_temp414->tl, _temp407= _temp407->tl)){ Cyc_Tcutil_unify(( void*) _temp414->hd,(
void*) _temp407->hd);} goto _LL398;} _LL404: goto _LL398; _LL398:;} for( 0;
_temp212 !=  0? tqts !=  0: 0;( _temp212= _temp212->tl, tqts= tqts->tl)){ struct
Cyc_Absyn_Pat* _temp415=( struct Cyc_Absyn_Pat*) _temp212->hd; void* _temp416=
Cyc_Tcutil_rsubstitute( rgn, _temp384,(*(( struct _tuple8*) tqts->hd)).f2);
struct Cyc_List_List* _temp419; struct Cyc_List_List* _temp421; struct _tuple6
_temp417= Cyc_Tcpat_tcPatRec( te2, _temp415,( void**)& _temp416, region_opt);
_LL422: _temp421= _temp417.f1; goto _LL420; _LL420: _temp419= _temp417.f2; goto
_LL418; _LL418: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp421); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp419); if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp415->topt))->v, _temp416)){({ struct Cyc_Std_String_pa_struct
_temp426; _temp426.tag= Cyc_Std_String_pa; _temp426.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp415->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp425; _temp425.tag= Cyc_Std_String_pa; _temp425.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp416);{ struct Cyc_Std_String_pa_struct _temp424; _temp424.tag= Cyc_Std_String_pa;
_temp424.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp218->name);{
void* _temp423[ 3u]={& _temp424,& _temp425,& _temp426}; Cyc_Tcutil_terr(
_temp415->loc, _tag_arr("%s expects argument type %s, not %s", sizeof(
unsigned char), 36u), _tag_arr( _temp423, sizeof( void*), 3u));}}}});}} if(
_temp212 !=  0){({ struct Cyc_Std_String_pa_struct _temp428; _temp428.tag= Cyc_Std_String_pa;
_temp428.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp218->name);{
void* _temp427[ 1u]={& _temp428}; Cyc_Tcutil_terr( p->loc, _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u), _tag_arr( _temp427, sizeof( void*), 1u));}});} if(
tqts !=  0){({ struct Cyc_Std_String_pa_struct _temp430; _temp430.tag= Cyc_Std_String_pa;
_temp430.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp218->name);{
void* _temp429[ 1u]={& _temp430}; Cyc_Tcutil_terr( p->loc, _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u), _tag_arr( _temp429, sizeof( void*), 1u));}});}}};
_pop_region( rgn);} goto _LL148; _LL178: goto _LL180; _LL180: goto _LL182;
_LL182: t= Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp432=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp432->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp432;})); goto _LL148; _LL148:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt*
_temp433=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp433->v=( void*) t; _temp433;}); return({ struct _tuple6 _temp434; _temp434.f1=
tv_result; _temp434.f2= v_result; _temp434;});}} struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void**
region_opt){ struct _tuple6 _temp435= Cyc_Tcpat_tcPatRec( te, p, topt,
region_opt);{ struct _RegionHandle _temp436= _new_region("r"); struct
_RegionHandle* r=& _temp436; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name,
_temp435.f2), p->loc, _tag_arr("pattern contains a repeated variable", sizeof(
unsigned char), 37u));; _pop_region( r);} return _temp435;} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp437=( void*) p->r;
struct Cyc_Absyn_Pat* _temp449; struct Cyc_List_List* _temp451; struct Cyc_List_List*
_temp453; struct Cyc_List_List* _temp455; _LL439: if(( unsigned int) _temp437 > 
2u?*(( int*) _temp437) ==  Cyc_Absyn_Pointer_p: 0){ _LL450: _temp449=(( struct
Cyc_Absyn_Pointer_p_struct*) _temp437)->f1; goto _LL440;} else{ goto _LL441;}
_LL441: if(( unsigned int) _temp437 >  2u?*(( int*) _temp437) ==  Cyc_Absyn_Tunion_p:
0){ _LL452: _temp451=(( struct Cyc_Absyn_Tunion_p_struct*) _temp437)->f4; goto
_LL442;} else{ goto _LL443;} _LL443: if(( unsigned int) _temp437 >  2u?*(( int*)
_temp437) ==  Cyc_Absyn_Struct_p: 0){ _LL454: _temp453=(( struct Cyc_Absyn_Struct_p_struct*)
_temp437)->f4; goto _LL444;} else{ goto _LL445;} _LL445: if(( unsigned int)
_temp437 >  2u?*(( int*) _temp437) ==  Cyc_Absyn_Tuple_p: 0){ _LL456: _temp455=((
struct Cyc_Absyn_Tuple_p_struct*) _temp437)->f1; goto _LL446;} else{ goto _LL447;}
_LL447: goto _LL448; _LL440: Cyc_Tcpat_check_pat_regions( te, _temp449);{ void*
_temp457=( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_PtrInfo
_temp465; void* _temp467; struct Cyc_Absyn_TunionInfo _temp469; void* _temp471;
_LL459: if(( unsigned int) _temp457 >  3u?*(( int*) _temp457) ==  Cyc_Absyn_PointerType:
0){ _LL466: _temp465=(( struct Cyc_Absyn_PointerType_struct*) _temp457)->f1;
_LL468: _temp467=( void*) _temp465.rgn_typ; goto _LL460;} else{ goto _LL461;}
_LL461: if(( unsigned int) _temp457 >  3u?*(( int*) _temp457) ==  Cyc_Absyn_TunionType:
0){ _LL470: _temp469=(( struct Cyc_Absyn_TunionType_struct*) _temp457)->f1;
_LL472: _temp471=( void*) _temp469.rgn; goto _LL462;} else{ goto _LL463;} _LL463:
goto _LL464; _LL460: _temp471= _temp467; goto _LL462; _LL462: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp471); return; _LL464:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp473=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp473[ 0]=({ struct Cyc_Core_Impossible_struct _temp474; _temp474.tag= Cyc_Core_Impossible;
_temp474.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp474;}); _temp473;})); _LL458:;} _LL442: for( 0;
_temp451 !=  0; _temp451= _temp451->tl){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*) _temp451->hd);}{ void* _temp475=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp483; void* _temp485;
_LL477: if(( unsigned int) _temp475 >  3u?*(( int*) _temp475) ==  Cyc_Absyn_TunionType:
0){ _LL484: _temp483=(( struct Cyc_Absyn_TunionType_struct*) _temp475)->f1;
_LL486: _temp485=( void*) _temp483.rgn; goto _LL478;} else{ goto _LL479;} _LL479:
if(( unsigned int) _temp475 >  3u?*(( int*) _temp475) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL480;} else{ goto _LL481;} _LL481: goto _LL482; _LL478: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp485); return; _LL480: return; _LL482:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp487=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp487[ 0]=({ struct
Cyc_Core_Impossible_struct _temp488; _temp488.tag= Cyc_Core_Impossible; _temp488.f1=
_tag_arr("check_pat_regions: bad tunion type", sizeof( unsigned char), 35u);
_temp488;}); _temp487;})); _LL476:;} _LL444: _temp455=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Pat*(*)( struct _tuple7*)) Cyc_Core_snd, _temp453); goto _LL446;
_LL446: for( 0; _temp455 !=  0; _temp455= _temp455->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*) _temp455->hd);} return; _LL448: return; _LL438:;}
static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Tcpat_Int_v= 1; struct Cyc_Tcpat_Int_v_struct{
int tag; int f1; } ; struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt*
span; } ; static const int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con= 0;
struct Cyc_Tcpat_Con_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp489=( void*) c1->name; struct _tagged_arr _temp495; int _temp497;
_LL491: if(*(( int*) _temp489) ==  Cyc_Tcpat_Name_v){ _LL496: _temp495=(( struct
Cyc_Tcpat_Name_v_struct*) _temp489)->f1; goto _LL492;} else{ goto _LL493;}
_LL493: if(*(( int*) _temp489) ==  Cyc_Tcpat_Int_v){ _LL498: _temp497=(( struct
Cyc_Tcpat_Int_v_struct*) _temp489)->f1; goto _LL494;} else{ goto _LL490;} _LL492: {
void* _temp499=( void*) c2->name; struct _tagged_arr _temp505; _LL501: if(*((
int*) _temp499) ==  Cyc_Tcpat_Name_v){ _LL506: _temp505=(( struct Cyc_Tcpat_Name_v_struct*)
_temp499)->f1; goto _LL502;} else{ goto _LL503;} _LL503: if(*(( int*) _temp499)
==  Cyc_Tcpat_Int_v){ goto _LL504;} else{ goto _LL500;} _LL502: return Cyc_Std_zstrcmp(
_temp495, _temp505); _LL504: return - 1; _LL500:;} _LL494: { void* _temp507=(
void*) c2->name; int _temp513; _LL509: if(*(( int*) _temp507) ==  Cyc_Tcpat_Name_v){
goto _LL510;} else{ goto _LL511;} _LL511: if(*(( int*) _temp507) ==  Cyc_Tcpat_Int_v){
_LL514: _temp513=(( struct Cyc_Tcpat_Int_v_struct*) _temp507)->f1; goto _LL512;}
else{ goto _LL508;} _LL510: return 1; _LL512: return _temp497 -  _temp513;
_LL508:;} _LL490:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return((
struct Cyc_Set_Set*(*)( int(* cmp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={( void*) 2}; static
struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static unsigned char _temp516[ 5u]="NULL"; static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={ 0u,{ _temp516, _temp516, _temp516 +  5u}}; static
unsigned char _temp518[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,{ _temp518, _temp518, _temp518 +  4u}}; static unsigned char _temp520[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp520,
_temp520, _temp520 +  6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp521=( struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp521->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp522=(
struct Cyc_Tcpat_Int_v_struct*) _cycalloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp522[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp523; _temp523.tag= Cyc_Tcpat_Int_v;
_temp523.f1= i; _temp523;}); _temp522;})); _temp521->arity= 0; _temp521->span= 0;
_temp521;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp524=( struct Cyc_Tcpat_Con_s*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp524->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp525=( struct Cyc_Tcpat_Name_v_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp525[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp526; _temp526.tag= Cyc_Tcpat_Name_v; _temp526.f1= f; _temp526;}); _temp525;}));
_temp524->arity= 0; _temp524->span= 0; _temp524;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp527=(
struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp527->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp529=( struct Cyc_Tcpat_Int_v_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp529[ 0]=({
struct Cyc_Tcpat_Int_v_struct _temp530; _temp530.tag= Cyc_Tcpat_Int_v; _temp530.f1=(
int) c; _temp530;}); _temp529;})); _temp527->arity= 0; _temp527->span=({ struct
Cyc_Core_Opt* _temp528=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp528->v=( void*) 256; _temp528;}); _temp527;});} static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp531=( struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp531->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp531->arity=
i; _temp531->span= Cyc_Tcpat_one_opt_ptr; _temp531;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp532=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp532[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp533; _temp533.tag= Cyc_Tcpat_Con; _temp533.f1= Cyc_Tcpat_null_con; _temp533.f2=
0; _temp533;}); _temp532;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp534=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp534[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp535; _temp535.tag= Cyc_Tcpat_Con; _temp535.f1= Cyc_Tcpat_int_con( i);
_temp535.f2= 0; _temp535;}); _temp534;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp536=(
struct Cyc_Tcpat_Con_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp536[ 0]=({ struct Cyc_Tcpat_Con_struct _temp537; _temp537.tag= Cyc_Tcpat_Con;
_temp537.f1= Cyc_Tcpat_char_con( c); _temp537.f2= 0; _temp537;}); _temp536;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp538=( struct Cyc_Tcpat_Con_struct*) _cycalloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp538[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp539; _temp539.tag= Cyc_Tcpat_Con; _temp539.f1= Cyc_Tcpat_float_con( f);
_temp539.f2= 0; _temp539;}); _temp538;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp540=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp540[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp541; _temp541.tag= Cyc_Tcpat_Con; _temp541.f1= Cyc_Tcpat_null_ptr_con;
_temp541.f2=({ struct Cyc_List_List* _temp542=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp542->hd=( void*) p; _temp542->tl= 0;
_temp542;}); _temp541;}); _temp540;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp543=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp543[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp544; _temp544.tag= Cyc_Tcpat_Con; _temp544.f1= Cyc_Tcpat_ptr_con; _temp544.f2=({
struct Cyc_List_List* _temp545=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp545->hd=( void*) p; _temp545->tl= 0; _temp545;});
_temp544;}); _temp543;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp546=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp546[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp547; _temp547.tag= Cyc_Tcpat_Con; _temp547.f1= Cyc_Tcpat_tuple_con( Cyc_List_length(
ss)); _temp547.f2= ss; _temp547;}); _temp546;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_arr con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){
struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp550=( struct Cyc_Tcpat_Con_s*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp550->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp551=( struct Cyc_Tcpat_Name_v_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp551[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp552; _temp552.tag= Cyc_Tcpat_Name_v; _temp552.f1= con_name; _temp552;});
_temp551;})); _temp550->arity= Cyc_List_length( ps); _temp550->span= span;
_temp550;}); return( void*)({ struct Cyc_Tcpat_Con_struct* _temp548=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp548[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp549; _temp549.tag= Cyc_Tcpat_Con; _temp549.f1= c; _temp549.f2= ps; _temp549;});
_temp548;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp553=( void*) p->r; int _temp589; void* _temp591; unsigned char
_temp593; struct _tagged_arr _temp595; struct Cyc_Absyn_Pat* _temp597; struct
Cyc_List_List* _temp599; struct Cyc_Absyn_Tunionfield* _temp601; struct Cyc_Absyn_Tuniondecl*
_temp603; struct Cyc_List_List* _temp605; struct Cyc_List_List* _temp607; struct
Cyc_List_List* _temp609; struct Cyc_Absyn_Structdecl* _temp611; struct Cyc_Absyn_Enumfield*
_temp613; struct Cyc_Absyn_Enumdecl* _temp615; struct Cyc_Absyn_Enumfield*
_temp617; void* _temp619; _LL555: if( _temp553 == ( void*) Cyc_Absyn_Wild_p){
goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int) _temp553 >  2u?*((
int*) _temp553) ==  Cyc_Absyn_Var_p: 0){ goto _LL558;} else{ goto _LL559;}
_LL559: if( _temp553 == ( void*) Cyc_Absyn_Null_p){ goto _LL560;} else{ goto
_LL561;} _LL561: if(( unsigned int) _temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_Int_p:
0){ _LL592: _temp591=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp553)->f1;
goto _LL590; _LL590: _temp589=(( struct Cyc_Absyn_Int_p_struct*) _temp553)->f2;
goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int) _temp553 >  2u?*((
int*) _temp553) ==  Cyc_Absyn_Char_p: 0){ _LL594: _temp593=(( struct Cyc_Absyn_Char_p_struct*)
_temp553)->f1; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_Float_p: 0){ _LL596: _temp595=((
struct Cyc_Absyn_Float_p_struct*) _temp553)->f1; goto _LL566;} else{ goto _LL567;}
_LL567: if(( unsigned int) _temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_Reference_p:
0){ goto _LL568;} else{ goto _LL569;} _LL569: if(( unsigned int) _temp553 >  2u?*((
int*) _temp553) ==  Cyc_Absyn_Pointer_p: 0){ _LL598: _temp597=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp553)->f1; goto _LL570;} else{ goto _LL571;} _LL571: if(( unsigned int)
_temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_Tunion_p: 0){ _LL604: _temp603=((
struct Cyc_Absyn_Tunion_p_struct*) _temp553)->f1; goto _LL602; _LL602: _temp601=((
struct Cyc_Absyn_Tunion_p_struct*) _temp553)->f2; goto _LL600; _LL600: _temp599=((
struct Cyc_Absyn_Tunion_p_struct*) _temp553)->f4; if( _temp599 ==  0){ goto
_LL572;} else{ goto _LL573;}} else{ goto _LL573;} _LL573: if(( unsigned int)
_temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_Tunion_p: 0){ _LL606: _temp605=((
struct Cyc_Absyn_Tunion_p_struct*) _temp553)->f4; goto _LL574;} else{ goto
_LL575;} _LL575: if(( unsigned int) _temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_Tuple_p:
0){ _LL608: _temp607=(( struct Cyc_Absyn_Tuple_p_struct*) _temp553)->f1; goto
_LL576;} else{ goto _LL577;} _LL577: if(( unsigned int) _temp553 >  2u?*(( int*)
_temp553) ==  Cyc_Absyn_Struct_p: 0){ _LL612: _temp611=(( struct Cyc_Absyn_Struct_p_struct*)
_temp553)->f1; goto _LL610; _LL610: _temp609=(( struct Cyc_Absyn_Struct_p_struct*)
_temp553)->f4; goto _LL578;} else{ goto _LL579;} _LL579: if(( unsigned int)
_temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_Enum_p: 0){ _LL616: _temp615=((
struct Cyc_Absyn_Enum_p_struct*) _temp553)->f1; goto _LL614; _LL614: _temp613=((
struct Cyc_Absyn_Enum_p_struct*) _temp553)->f2; goto _LL580;} else{ goto _LL581;}
_LL581: if(( unsigned int) _temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_AnonEnum_p:
0){ _LL620: _temp619=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp553)->f1;
goto _LL618; _LL618: _temp617=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp553)->f2;
goto _LL582;} else{ goto _LL583;} _LL583: if(( unsigned int) _temp553 >  2u?*((
int*) _temp553) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL584;} else{ goto _LL585;}
_LL585: if(( unsigned int) _temp553 >  2u?*(( int*) _temp553) ==  Cyc_Absyn_UnknownCall_p:
0){ goto _LL586;} else{ goto _LL587;} _LL587: if(( unsigned int) _temp553 >  2u?*((
int*) _temp553) ==  Cyc_Absyn_UnknownFields_p: 0){ goto _LL588;} else{ goto
_LL554;} _LL556: s=( void*) Cyc_Tcpat_Any; goto _LL554; _LL558: s=( void*) Cyc_Tcpat_Any;
goto _LL554; _LL560: s= Cyc_Tcpat_null_pat(); goto _LL554; _LL562: s= Cyc_Tcpat_int_pat(
_temp589); goto _LL554; _LL564: s= Cyc_Tcpat_char_pat( _temp593); goto _LL554;
_LL566: s= Cyc_Tcpat_float_pat( _temp595); goto _LL554; _LL568: s=( void*) Cyc_Tcpat_Any;
goto _LL554; _LL570:{ void* _temp621= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v); struct Cyc_Absyn_PtrInfo _temp629; struct Cyc_Absyn_Conref*
_temp631; _LL623: if(( unsigned int) _temp621 >  3u?*(( int*) _temp621) ==  Cyc_Absyn_PointerType:
0){ _LL630: _temp629=(( struct Cyc_Absyn_PointerType_struct*) _temp621)->f1;
_LL632: _temp631= _temp629.nullable; goto _LL624;} else{ goto _LL625;} _LL625:
if(( unsigned int) _temp621 >  3u?*(( int*) _temp621) ==  Cyc_Absyn_TunionType:
0){ goto _LL626;} else{ goto _LL627;} _LL627: goto _LL628; _LL624: { int
is_nullable= 0; int still_working= 1; while( still_working) { void* _temp633=(
void*) _temp631->v; struct Cyc_Absyn_Conref* _temp641; int _temp643; _LL635: if((
unsigned int) _temp633 >  1u?*(( int*) _temp633) ==  Cyc_Absyn_Forward_constr: 0){
_LL642: _temp641=(( struct Cyc_Absyn_Forward_constr_struct*) _temp633)->f1; goto
_LL636;} else{ goto _LL637;} _LL637: if( _temp633 == ( void*) Cyc_Absyn_No_constr){
goto _LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp633 >  1u?*((
int*) _temp633) ==  Cyc_Absyn_Eq_constr: 0){ _LL644: _temp643=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp633)->f1; goto _LL640;} else{ goto _LL634;}
_LL636:( void*)( _temp631->v=( void*)(( void*) _temp641->v)); continue; _LL638:(
void*)( _temp631->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp645=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp645[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp646; _temp646.tag= Cyc_Absyn_Eq_constr;
_temp646.f1=( void*) 0; _temp646;}); _temp645;}))); is_nullable= 0;
still_working= 0; goto _LL634; _LL640: is_nullable=( int) _temp643;
still_working= 0; goto _LL634; _LL634:;}{ void* ss= Cyc_Tcpat_compile_pat(
_temp597); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat( ss);} else{ s= Cyc_Tcpat_ptr_pat(
ss);} goto _LL622;}} _LL626:{ void* _temp647=( void*) _temp597->r; struct Cyc_List_List*
_temp653; struct Cyc_Absyn_Tunionfield* _temp655; struct Cyc_Absyn_Tuniondecl*
_temp657; _LL649: if(( unsigned int) _temp647 >  2u?*(( int*) _temp647) ==  Cyc_Absyn_Tunion_p:
0){ _LL658: _temp657=(( struct Cyc_Absyn_Tunion_p_struct*) _temp647)->f1; goto
_LL656; _LL656: _temp655=(( struct Cyc_Absyn_Tunion_p_struct*) _temp647)->f2;
goto _LL654; _LL654: _temp653=(( struct Cyc_Absyn_Tunion_p_struct*) _temp647)->f4;
goto _LL650;} else{ goto _LL651;} _LL651: goto _LL652; _LL650: { struct Cyc_Core_Opt*
span; if( _temp657->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp659=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp659->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp657->fields))->v);
_temp659;});} s= Cyc_Tcpat_con_pat(*(* _temp655->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp653)); goto _LL648;} _LL652: s=({ void* _temp660[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("non-[x]tunion pattern has tunion type", sizeof( unsigned char), 38u),
_tag_arr( _temp660, sizeof( void*), 0u));}); goto _LL648; _LL648:;} goto _LL622;
_LL628: s=({ void* _temp661[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u), _tag_arr( _temp661, sizeof( void*), 0u));}); goto
_LL622; _LL622:;} goto _LL554; _LL572: { struct Cyc_Core_Opt* span;{ void*
_temp662= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v);
_LL664: if(( unsigned int) _temp662 >  3u?*(( int*) _temp662) ==  Cyc_Absyn_TunionType:
0){ goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int) _temp662 >  3u?*((
int*) _temp662) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL667;} else{ goto
_LL668;} _LL668: goto _LL669; _LL665: if( _temp603->is_xtunion){ span= 0;} else{
span=({ struct Cyc_Core_Opt* _temp670=( struct Cyc_Core_Opt*) _cycalloc_atomic(
sizeof( struct Cyc_Core_Opt)); _temp670->v=( void*)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp603->fields))->v); _temp670;});} goto _LL663; _LL667: span=({
struct Cyc_Core_Opt* _temp671=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp671->v=( void*) 1; _temp671;}); goto _LL663; _LL669:
span=({ void* _temp672[ 0u]={};(( struct Cyc_Core_Opt*(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("void tunion pattern has bad type",
sizeof( unsigned char), 33u), _tag_arr( _temp672, sizeof( void*), 0u));}); goto
_LL663; _LL663:;} s= Cyc_Tcpat_con_pat(*(* _temp601->name).f2, span, 0); goto
_LL554;} _LL574: _temp607= _temp605; goto _LL576; _LL576: s= Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp607)); goto _LL554; _LL578: {
struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp611->fields))->v; for( 0; fields !=  0;
fields= fields->tl){ int found= Cyc_Std_strcmp(*(( struct Cyc_Absyn_Structfield*)
fields->hd)->name, _tag_arr("", sizeof( unsigned char), 1u)) ==  0;{ struct Cyc_List_List*
dlps0= _temp609; for( 0; ! found? dlps0 !=  0: 0; dlps0= dlps0->tl){ struct
_tuple7 _temp675; struct Cyc_Absyn_Pat* _temp676; struct Cyc_List_List* _temp678;
struct _tuple7* _temp673=( struct _tuple7*) dlps0->hd; _temp675=* _temp673;
_LL679: _temp678= _temp675.f1; goto _LL677; _LL677: _temp676= _temp675.f2; goto
_LL674; _LL674: { struct Cyc_List_List* _temp680= _temp678; struct Cyc_List_List
_temp686; struct Cyc_List_List* _temp687; void* _temp689; struct _tagged_arr*
_temp691; _LL682: if( _temp680 ==  0){ goto _LL684;} else{ _temp686=* _temp680;
_LL690: _temp689=( void*) _temp686.hd; if(*(( int*) _temp689) ==  Cyc_Absyn_FieldName){
_LL692: _temp691=(( struct Cyc_Absyn_FieldName_struct*) _temp689)->f1; goto
_LL688;} else{ goto _LL684;} _LL688: _temp687= _temp686.tl; if( _temp687 ==  0){
goto _LL683;} else{ goto _LL684;}} _LL684: goto _LL685; _LL683: if( Cyc_Std_zstrptrcmp(
_temp691,(( struct Cyc_Absyn_Structfield*) fields->hd)->name) ==  0){ ps=({
struct Cyc_List_List* _temp693=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp693->hd=( void*) Cyc_Tcpat_compile_pat( _temp676);
_temp693->tl= ps; _temp693;}); found= 1;} goto _LL681; _LL685:({ void* _temp694[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bad designator(s)", sizeof( unsigned char), 18u), _tag_arr( _temp694,
sizeof( void*), 0u));}); goto _LL681; _LL681:;}}} if( ! found){({ void* _temp695[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bad designator", sizeof( unsigned char), 15u), _tag_arr( _temp695,
sizeof( void*), 0u));});}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL554;} _LL580: {
int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp615->fields))->v); s= Cyc_Tcpat_con_pat(*(*
_temp613->name).f2,({ struct Cyc_Core_Opt* _temp696=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp696->v=( void*) span;
_temp696;}), 0); goto _LL554;} _LL582: { struct Cyc_List_List* fields;{ void*
_temp697= Cyc_Tcutil_compress( _temp619); struct Cyc_List_List* _temp703; _LL699:
if(( unsigned int) _temp697 >  3u?*(( int*) _temp697) ==  Cyc_Absyn_AnonEnumType:
0){ _LL704: _temp703=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp697)->f1;
goto _LL700;} else{ goto _LL701;} _LL701: goto _LL702; _LL700: fields= _temp703;
goto _LL698; _LL702: fields=({ void* _temp705[ 0u]={};(( struct Cyc_List_List*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("bad type in AnonEnum_p",
sizeof( unsigned char), 23u), _tag_arr( _temp705, sizeof( void*), 0u));}); goto
_LL698; _LL698:;}{ int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
fields); s= Cyc_Tcpat_con_pat(*(* _temp617->name).f2,({ struct Cyc_Core_Opt*
_temp706=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp706->v=( void*) span; _temp706;}), 0); goto _LL554;}} _LL584: s=({ void*
_temp707[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("compile_pat: unknownid", sizeof(
unsigned char), 23u), _tag_arr( _temp707, sizeof( void*), 0u));}); goto _LL554;
_LL586: s=({ void* _temp708[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("compile_pat: unknowncall",
sizeof( unsigned char), 25u), _tag_arr( _temp708, sizeof( void*), 0u));}); goto
_LL554; _LL588: s=({ void* _temp709[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("compile_pat: unknownfields",
sizeof( unsigned char), 27u), _tag_arr( _temp709, sizeof( void*), 0u));}); goto
_LL554; _LL554:;} return s;} static const int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set*
f1; } ; static const int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success=
0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s*
f2; void* f3; void* f4; } ; struct _tuple10{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Tcpat_Yes=
0; static const int Cyc_Tcpat_No= 1; static const int Cyc_Tcpat_Maybe= 2; static
void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp710=
td; struct Cyc_Set_Set* _temp716; _LL712: if(*(( int*) _temp710) ==  Cyc_Tcpat_Neg){
_LL717: _temp716=(( struct Cyc_Tcpat_Neg_struct*) _temp710)->f1; goto _LL713;}
else{ goto _LL714;} _LL714: if(*(( int*) _temp710) ==  Cyc_Tcpat_Pos){ goto
_LL715;} else{ goto _LL711;} _LL713: if((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp716, c)){({ void* _temp718[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set", sizeof( unsigned char),
47u), _tag_arr( _temp718, sizeof( void*), 0u));});} if( c->span !=  0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp716) +  1 >= ( int)(( struct
Cyc_Core_Opt*) _check_null( c->span))->v: 0){({ void* _temp719[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof( unsigned char), 42u), _tag_arr( _temp719, sizeof( void*), 0u));});}
return( void*)({ struct Cyc_Tcpat_Neg_struct* _temp720=( struct Cyc_Tcpat_Neg_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp720[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp721; _temp721.tag= Cyc_Tcpat_Neg; _temp721.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp716,
c); _temp721;}); _temp720;}); _LL715: return({ void* _temp722[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("add_neg called when td is Positive", sizeof( unsigned char), 35u),
_tag_arr( _temp722, sizeof( void*), 0u));}); _LL711:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp723= td; struct Cyc_Tcpat_Con_s*
_temp729; struct Cyc_Set_Set* _temp731; _LL725: if(*(( int*) _temp723) ==  Cyc_Tcpat_Pos){
_LL730: _temp729=(( struct Cyc_Tcpat_Pos_struct*) _temp723)->f1; goto _LL726;}
else{ goto _LL727;} _LL727: if(*(( int*) _temp723) ==  Cyc_Tcpat_Neg){ _LL732:
_temp731=(( struct Cyc_Tcpat_Neg_struct*) _temp723)->f1; goto _LL728;} else{
goto _LL724;} _LL726: if( Cyc_Tcpat_compare_con( c, _temp729) ==  0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL728: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp731,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span !=  0?( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v == (( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp731) +  1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL724:;} struct _tuple11{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp733= ctxt; struct Cyc_List_List _temp739;
struct Cyc_List_List* _temp740; struct _tuple11* _temp742; struct _tuple11
_temp744; struct Cyc_List_List* _temp745; struct Cyc_Tcpat_Con_s* _temp747;
_LL735: if( _temp733 ==  0){ goto _LL736;} else{ goto _LL737;} _LL737: if(
_temp733 ==  0){ goto _LL734;} else{ _temp739=* _temp733; _LL743: _temp742=(
struct _tuple11*) _temp739.hd; _temp744=* _temp742; _LL748: _temp747= _temp744.f1;
goto _LL746; _LL746: _temp745= _temp744.f2; goto _LL741; _LL741: _temp740=
_temp739.tl; goto _LL738;} _LL736: return 0; _LL738: return({ struct Cyc_List_List*
_temp749=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp749->hd=( void*)({ struct _tuple11* _temp750=( struct _tuple11*) _cycalloc(
sizeof( struct _tuple11)); _temp750->f1= _temp747; _temp750->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp751=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp751->hd=( void*) dsc; _temp751->tl= _temp745;
_temp751;}); _temp750;}); _temp749->tl= _temp740; _temp749;}); _LL734:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp752= ctxt; struct Cyc_List_List _temp758; struct Cyc_List_List*
_temp759; struct _tuple11* _temp761; struct _tuple11 _temp763; struct Cyc_List_List*
_temp764; struct Cyc_Tcpat_Con_s* _temp766; _LL754: if( _temp752 ==  0){ goto
_LL755;} else{ goto _LL756;} _LL756: if( _temp752 ==  0){ goto _LL753;} else{
_temp758=* _temp752; _LL762: _temp761=( struct _tuple11*) _temp758.hd; _temp763=*
_temp761; _LL767: _temp766= _temp763.f1; goto _LL765; _LL765: _temp764= _temp763.f2;
goto _LL760; _LL760: _temp759= _temp758.tl; goto _LL757;} _LL755: return({ void*
_temp768[ 0u]={};(( struct Cyc_List_List*(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("norm_context: empty context",
sizeof( unsigned char), 28u), _tag_arr( _temp768, sizeof( void*), 0u));});
_LL757: return Cyc_Tcpat_augment( _temp759,( void*)({ struct Cyc_Tcpat_Pos_struct*
_temp769=( struct Cyc_Tcpat_Pos_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp769[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp770; _temp770.tag= Cyc_Tcpat_Pos;
_temp770.f1= _temp766; _temp770.f2= Cyc_List_rev( _temp764); _temp770;});
_temp769;})); _LL753:;} static void* Cyc_Tcpat_build_desc( struct Cyc_List_List*
ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple6 _temp772=({ struct
_tuple6 _temp771; _temp771.f1= ctxt; _temp771.f2= work; _temp771;}); struct Cyc_List_List*
_temp782; struct Cyc_List_List* _temp784; struct Cyc_List_List* _temp786; struct
Cyc_List_List* _temp788; struct Cyc_List_List* _temp790; struct Cyc_List_List
_temp792; struct Cyc_List_List* _temp793; struct _tuple10* _temp795; struct
_tuple10 _temp797; struct Cyc_List_List* _temp798; struct Cyc_List_List*
_temp800; struct Cyc_List_List _temp802; struct Cyc_List_List* _temp803; struct
_tuple11* _temp805; struct _tuple11 _temp807; struct Cyc_List_List* _temp808;
struct Cyc_Tcpat_Con_s* _temp810; _LL774: _LL785: _temp784= _temp772.f1; if(
_temp784 ==  0){ goto _LL783;} else{ goto _LL776;} _LL783: _temp782= _temp772.f2;
if( _temp782 ==  0){ goto _LL775;} else{ goto _LL776;} _LL776: _LL787: _temp786=
_temp772.f1; if( _temp786 ==  0){ goto _LL777;} else{ goto _LL778;} _LL778:
_LL789: _temp788= _temp772.f2; if( _temp788 ==  0){ goto _LL779;} else{ goto
_LL780;} _LL780: _LL801: _temp800= _temp772.f1; if( _temp800 ==  0){ goto _LL773;}
else{ _temp802=* _temp800; _LL806: _temp805=( struct _tuple11*) _temp802.hd;
_temp807=* _temp805; _LL811: _temp810= _temp807.f1; goto _LL809; _LL809:
_temp808= _temp807.f2; goto _LL804; _LL804: _temp803= _temp802.tl; goto _LL791;}
_LL791: _temp790= _temp772.f2; if( _temp790 ==  0){ goto _LL773;} else{ _temp792=*
_temp790; _LL796: _temp795=( struct _tuple10*) _temp792.hd; _temp797=* _temp795;
_LL799: _temp798= _temp797.f3; goto _LL794; _LL794: _temp793= _temp792.tl; goto
_LL781;} _LL775: return dsc; _LL777: return({ void* _temp812[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("build_desc: ctxt and work don't match", sizeof( unsigned char), 38u),
_tag_arr( _temp812, sizeof( void*), 0u));}); _LL779: return({ void* _temp813[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("build_desc: ctxt and work don't match", sizeof(
unsigned char), 38u), _tag_arr( _temp813, sizeof( void*), 0u));}); _LL781: {
struct Cyc_Tcpat_Pos_struct* _temp814=({ struct Cyc_Tcpat_Pos_struct* _temp815=(
struct Cyc_Tcpat_Pos_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp815[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp816; _temp816.tag= Cyc_Tcpat_Pos;
_temp816.f1= _temp810; _temp816.f2= Cyc_List_append( Cyc_List_rev( _temp808),({
struct Cyc_List_List* _temp817=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp817->hd=( void*) dsc; _temp817->tl= _temp798;
_temp817;})); _temp816;}); _temp815;}); return Cyc_Tcpat_build_desc( _temp803,(
void*) _temp814, _temp793);} _LL773:;} static void* Cyc_Tcpat_match( void* p,
struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp818=
allmrules; struct Cyc_List_List _temp824; struct Cyc_List_List* _temp825; struct
_tuple0* _temp827; struct _tuple0 _temp829; void* _temp830; void* _temp832;
_LL820: if( _temp818 ==  0){ goto _LL821;} else{ goto _LL822;} _LL822: if(
_temp818 ==  0){ goto _LL819;} else{ _temp824=* _temp818; _LL828: _temp827=(
struct _tuple0*) _temp824.hd; _temp829=* _temp827; _LL833: _temp832= _temp829.f1;
goto _LL831; _LL831: _temp830= _temp829.f2; goto _LL826; _LL826: _temp825=
_temp824.tl; goto _LL823;} _LL821: return( void*) Cyc_Tcpat_Failure; _LL823:
return Cyc_Tcpat_match( _temp832, 0, dsc, 0, 0, _temp830, _temp825); _LL819:;}
static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return
Cyc_Tcpat_or_match(( void*)({ struct Cyc_Tcpat_Neg_struct* _temp834=( struct Cyc_Tcpat_Neg_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp834[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp835; _temp835.tag= Cyc_Tcpat_Neg; _temp835.f1= Cyc_Tcpat_empty_con_set();
_temp835;}); _temp834;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp836= work; struct Cyc_List_List
_temp844; struct Cyc_List_List* _temp845; struct _tuple10* _temp847; struct
_tuple10 _temp849; struct Cyc_List_List* _temp850; struct Cyc_List_List*
_temp852; struct Cyc_List_List* _temp854; struct Cyc_List_List _temp856; struct
Cyc_List_List* _temp857; struct _tuple10* _temp859; struct _tuple10 _temp861;
struct Cyc_List_List* _temp862; struct Cyc_List_List* _temp864; struct Cyc_List_List*
_temp866; _LL838: if( _temp836 ==  0){ goto _LL839;} else{ goto _LL840;} _LL840:
if( _temp836 ==  0){ goto _LL842;} else{ _temp844=* _temp836; _LL848: _temp847=(
struct _tuple10*) _temp844.hd; _temp849=* _temp847; _LL855: _temp854= _temp849.f1;
if( _temp854 ==  0){ goto _LL853;} else{ goto _LL842;} _LL853: _temp852=
_temp849.f2; if( _temp852 ==  0){ goto _LL851;} else{ goto _LL842;} _LL851:
_temp850= _temp849.f3; if( _temp850 ==  0){ goto _LL846;} else{ goto _LL842;}
_LL846: _temp845= _temp844.tl; goto _LL841;} _LL842: if( _temp836 ==  0){ goto
_LL837;} else{ _temp856=* _temp836; _LL860: _temp859=( struct _tuple10*)
_temp856.hd; _temp861=* _temp859; _LL867: _temp866= _temp861.f1; goto _LL865;
_LL865: _temp864= _temp861.f2; goto _LL863; _LL863: _temp862= _temp861.f3; goto
_LL858; _LL858: _temp857= _temp856.tl; goto _LL843;} _LL839: return( void*)({
struct Cyc_Tcpat_Success_struct* _temp868=( struct Cyc_Tcpat_Success_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp868[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp869; _temp869.tag= Cyc_Tcpat_Success; _temp869.f1=( void*) right_hand_side;
_temp869;}); _temp868;}); _LL841: return Cyc_Tcpat_and_match( Cyc_Tcpat_norm_context(
ctx), _temp845, right_hand_side, rules); _LL843: if(( _temp866 ==  0? 1:
_temp864 ==  0)? 1: _temp862 ==  0){ return({ void* _temp870[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("tcpat:and_match: malformed work frame", sizeof( unsigned char), 38u),
_tag_arr( _temp870, sizeof( void*), 0u));});}{ struct Cyc_List_List _temp873;
struct Cyc_List_List* _temp874; void* _temp876; struct Cyc_List_List* _temp871=(
struct Cyc_List_List*) _check_null( _temp866); _temp873=* _temp871; _LL877:
_temp876=( void*) _temp873.hd; goto _LL875; _LL875: _temp874= _temp873.tl; goto
_LL872; _LL872: { struct Cyc_List_List _temp880; struct Cyc_List_List* _temp881;
struct Cyc_List_List* _temp883; struct Cyc_List_List* _temp878=( struct Cyc_List_List*)
_check_null( _temp864); _temp880=* _temp878; _LL884: _temp883=( struct Cyc_List_List*)
_temp880.hd; goto _LL882; _LL882: _temp881= _temp880.tl; goto _LL879; _LL879: {
struct Cyc_List_List _temp887; struct Cyc_List_List* _temp888; void* _temp890;
struct Cyc_List_List* _temp885=( struct Cyc_List_List*) _check_null( _temp862);
_temp887=* _temp885; _LL891: _temp890=( void*) _temp887.hd; goto _LL889; _LL889:
_temp888= _temp887.tl; goto _LL886; _LL886: { struct _tuple10* _temp892=({
struct _tuple10* _temp894=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp894->f1= _temp874; _temp894->f2= _temp881; _temp894->f3= _temp888; _temp894;});
return Cyc_Tcpat_match( _temp876, _temp883, _temp890, ctx,({ struct Cyc_List_List*
_temp893=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp893->hd=( void*) _temp892; _temp893->tl= _temp857; _temp893;}),
right_hand_side, rules);}}}} _LL837:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp895= dsc; struct Cyc_Set_Set*
_temp901; struct Cyc_List_List* _temp903; _LL897: if(*(( int*) _temp895) ==  Cyc_Tcpat_Neg){
_LL902: _temp901=(( struct Cyc_Tcpat_Neg_struct*) _temp895)->f1; goto _LL898;}
else{ goto _LL899;} _LL899: if(*(( int*) _temp895) ==  Cyc_Tcpat_Pos){ _LL904:
_temp903=(( struct Cyc_Tcpat_Pos_struct*) _temp895)->f2; goto _LL900;} else{
goto _LL896;} _LL898: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp907=( struct Cyc_Tcpat_Neg_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp907[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp908; _temp908.tag= Cyc_Tcpat_Neg;
_temp908.f1= Cyc_Tcpat_empty_con_set(); _temp908;}); _temp907;}); struct Cyc_List_List*
_temp905= 0;{ int i= 0; for( 0; i <  pcon->arity; ++ i){ _temp905=({ struct Cyc_List_List*
_temp906=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp906->hd=( void*) any; _temp906->tl= _temp905; _temp906;});}} return
_temp905;} _LL900: return _temp903; _LL896:;} static struct Cyc_List_List* Cyc_Tcpat_getoarg(
struct Cyc_List_List* obj, int i){ return({ struct Cyc_List_List* _temp909=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp909->hd=(
void*)( i +  1); _temp909->tl= obj; _temp909;});} static struct Cyc_List_List*
Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon, struct Cyc_List_List* obj){
return(( struct Cyc_List_List*(*)( int n, struct Cyc_List_List*(* f)( struct Cyc_List_List*,
int), struct Cyc_List_List* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void*
dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules){ void* _temp910= p; struct Cyc_List_List*
_temp916; struct Cyc_Tcpat_Con_s* _temp918; _LL912: if( _temp910 == ( void*) Cyc_Tcpat_Any){
goto _LL913;} else{ goto _LL914;} _LL914: if(( unsigned int) _temp910 >  1u?*((
int*) _temp910) ==  Cyc_Tcpat_Con: 0){ _LL919: _temp918=(( struct Cyc_Tcpat_Con_struct*)
_temp910)->f1; goto _LL917; _LL917: _temp916=(( struct Cyc_Tcpat_Con_struct*)
_temp910)->f2; goto _LL915;} else{ goto _LL911;} _LL913: return Cyc_Tcpat_and_match(
Cyc_Tcpat_augment( ctx, dsc), work, right_hand_side, rules); _LL915: { void*
_temp920= Cyc_Tcpat_static_match( _temp918, dsc); _LL922: if( _temp920 == ( void*)
Cyc_Tcpat_Yes){ goto _LL923;} else{ goto _LL924;} _LL924: if( _temp920 == ( void*)
Cyc_Tcpat_No){ goto _LL925;} else{ goto _LL926;} _LL926: if( _temp920 == ( void*)
Cyc_Tcpat_Maybe){ goto _LL927;} else{ goto _LL921;} _LL923: { struct Cyc_List_List*
_temp928=({ struct Cyc_List_List* _temp933=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp933->hd=( void*)({ struct _tuple11*
_temp934=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11)); _temp934->f1=
_temp918; _temp934->f2= 0; _temp934;}); _temp933->tl= ctx; _temp933;}); struct
_tuple10* _temp929=({ struct _tuple10* _temp932=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp932->f1= _temp916; _temp932->f2= Cyc_Tcpat_getoargs(
_temp918, obj); _temp932->f3= Cyc_Tcpat_getdargs( _temp918, dsc); _temp932;});
struct Cyc_List_List* _temp930=({ struct Cyc_List_List* _temp931=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp931->hd=( void*) _temp929;
_temp931->tl= work; _temp931;}); return Cyc_Tcpat_and_match( _temp928, _temp930,
right_hand_side, rules);} _LL925: return Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL927: { struct Cyc_List_List* _temp935=({ struct Cyc_List_List*
_temp944=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp944->hd=( void*)({ struct _tuple11* _temp945=( struct _tuple11*) _cycalloc(
sizeof( struct _tuple11)); _temp945->f1= _temp918; _temp945->f2= 0; _temp945;});
_temp944->tl= ctx; _temp944;}); struct _tuple10* _temp936=({ struct _tuple10*
_temp943=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp943->f1=
_temp916; _temp943->f2= Cyc_Tcpat_getoargs( _temp918, obj); _temp943->f3= Cyc_Tcpat_getdargs(
_temp918, dsc); _temp943;}); struct Cyc_List_List* _temp937=({ struct Cyc_List_List*
_temp942=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp942->hd=( void*) _temp936; _temp942->tl= work; _temp942;}); void* _temp938=
Cyc_Tcpat_and_match( _temp935, _temp937, right_hand_side, rules); void* _temp939=
Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg( dsc, _temp918),
work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp940=( struct
Cyc_Tcpat_IfEq_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp940[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp941; _temp941.tag= Cyc_Tcpat_IfEq;
_temp941.f1= obj; _temp941.f2= _temp918; _temp941.f3=( void*) _temp938; _temp941.f4=(
void*) _temp939; _temp941;}); _temp940;});} _LL921:;} _LL911:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp946= d; void* _temp954; void* _temp956; void*
_temp958; _LL948: if( _temp946 == ( void*) Cyc_Tcpat_Failure){ goto _LL949;}
else{ goto _LL950;} _LL950: if(( unsigned int) _temp946 >  1u?*(( int*) _temp946)
==  Cyc_Tcpat_Success: 0){ _LL955: _temp954=( void*)(( struct Cyc_Tcpat_Success_struct*)
_temp946)->f1; goto _LL951;} else{ goto _LL952;} _LL952: if(( unsigned int)
_temp946 >  1u?*(( int*) _temp946) ==  Cyc_Tcpat_IfEq: 0){ _LL959: _temp958=(
void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp946)->f3; goto _LL957; _LL957:
_temp956=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp946)->f4; goto _LL953;}
else{ goto _LL947;} _LL949: not_exhaust( env1); goto _LL947; _LL951: rhs_appears(
env2, _temp954); goto _LL947; _LL953: Cyc_Tcpat_check_exhaust_overlap( _temp958,
not_exhaust, env1, rhs_appears, env2); Cyc_Tcpat_check_exhaust_overlap( _temp956,
not_exhaust, env1, rhs_appears, env2); goto _LL947; _LL947:;} struct _tuple12{
int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{ void* f1; struct
_tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match( int* r, struct
Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern);
struct _tuple12* rhs=({ struct _tuple12* _temp965=( struct _tuple12*) _cycalloc(
sizeof( struct _tuple12)); _temp965->f1= 0; _temp965->f2=( swc->pattern)->loc;
_temp965;}); void* sp; if( swc->where_clause !=  0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp960=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp960->hd=( void*) sp0; _temp960->tl=({ struct Cyc_List_List*
_temp961=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp961->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp961->tl= 0; _temp961;});
_temp960;}));* r=* r +  1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp962=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp962->hd=( void*) sp0; _temp962->tl=({ struct Cyc_List_List* _temp963=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp963->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp963->tl= 0; _temp963;}); _temp962;}));}
return({ struct _tuple13* _temp964=( struct _tuple13*) _cycalloc( sizeof( struct
_tuple13)); _temp964->f1= sp; _temp964->f2= rhs; _temp964;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){({ void* _temp966[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("patterns may not be exhaustive", sizeof( unsigned char), 31u),
_tag_arr( _temp966, sizeof( void*), 0u));});} static void Cyc_Tcpat_rule_occurs(
int dummy, struct _tuple12* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp967= 0;
struct Cyc_List_List* _temp968=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)(
int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcpat_get_match,& _temp967, swcs); void* _temp969=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp968);(( void(*)( void* d, void(*
not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment* env1,
void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp969, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp968 !=  0; _temp968= _temp968->tl){ struct _tuple13 _temp972; struct
_tuple12* _temp973; struct _tuple12 _temp975; struct Cyc_Position_Segment*
_temp976; int _temp978; struct _tuple13* _temp970=( struct _tuple13*) _temp968->hd;
_temp972=* _temp970; _LL974: _temp973= _temp972.f2; _temp975=* _temp973; _LL979:
_temp978= _temp975.f1; goto _LL977; _LL977: _temp976= _temp975.f2; goto _LL971;
_LL971: if( ! _temp978){({ void* _temp980[ 0u]={}; Cyc_Tcutil_terr( _temp976,
_tag_arr("redundant pattern", sizeof( unsigned char), 18u), _tag_arr( _temp980,
sizeof( void*), 0u));});}}} struct _tuple14{ struct Cyc_Position_Segment* f1;
int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple14* pr){(* pr).f2=
0;({ void* _temp981[ 0u]={}; Cyc_Tcutil_warn((* pr).f1, _tag_arr("pattern not exhaustive",
sizeof( unsigned char), 23u), _tag_arr( _temp981, sizeof( void*), 0u));});}
static void Cyc_Tcpat_dummy_fn( int i, int j){ return;} struct _tuple15{ void*
f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List* _temp982=({ struct Cyc_List_List*
_temp986=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp986->hd=( void*)({ struct _tuple15* _temp987=( struct _tuple15*) _cycalloc(
sizeof( struct _tuple15)); _temp987->f1= Cyc_Tcpat_compile_pat( p); _temp987->f2=
0; _temp987;}); _temp986->tl= 0; _temp986;}); void* _temp983=(( void*(*)( struct
Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( _temp982); struct _tuple14*
_temp984=({ struct _tuple14* _temp985=( struct _tuple14*) _cycalloc( sizeof(
struct _tuple14)); _temp985->f1= loc; _temp985->f2= 1; _temp985;});(( void(*)(
void* d, void(* not_exhaust)( struct _tuple14*), struct _tuple14* env1, void(*
rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp983,
Cyc_Tcpat_not_exhaust_warn, _temp984, Cyc_Tcpat_dummy_fn, 0); return(* _temp984).f2;}
static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp989=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp989->f1= 0; _temp989->f2=( swc->pattern)->loc; _temp989;}); return({ struct
_tuple13* _temp988=( struct _tuple13*) _cycalloc( sizeof( struct _tuple13));
_temp988->f1= sp0; _temp988->f2= rhs; _temp988;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp990=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
_temp991=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp990);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp991, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp990 !=  0; _temp990= _temp990->tl){
struct _tuple13 _temp994; struct _tuple12* _temp995; struct _tuple12 _temp997;
struct Cyc_Position_Segment* _temp998; int _temp1000; struct _tuple13* _temp992=(
struct _tuple13*) _temp990->hd; _temp994=* _temp992; _LL996: _temp995= _temp994.f2;
_temp997=* _temp995; _LL1001: _temp1000= _temp997.f1; goto _LL999; _LL999:
_temp998= _temp997.f2; goto _LL993; _LL993: if( ! _temp1000){({ void* _temp1002[
0u]={}; Cyc_Tcutil_terr( _temp998, _tag_arr("redundant pattern", sizeof(
unsigned char), 18u), _tag_arr( _temp1002, sizeof( void*), 0u));});}}}
