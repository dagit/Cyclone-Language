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
extern void * _region_calloc(struct _RegionHandle *, unsigned int t,
                             unsigned int n);
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
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
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
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
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
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern void* Cyc_Core_identity(
void*); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
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
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
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
int tag; unsigned int f1; } ; struct Cyc_Absyn_MallocInfo{ int is_calloc; struct
Cyc_Absyn_Exp* rgn; void** elt_type; struct Cyc_Absyn_Exp* num_elts; int
fat_result; } ; static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{
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
tag; struct Cyc_Absyn_MallocInfo f1; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_ForArrayInfo{
struct Cyc_List_List* defns; struct _tuple2 condition; struct _tuple2 delta;
struct Cyc_Absyn_Stmt* body; } ; static const int Cyc_Absyn_Skip_s= 0; static
const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int
Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
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
static const int Cyc_Absyn_ForArray_s= 19; struct Cyc_Absyn_ForArray_s_struct{
int tag; struct Cyc_Absyn_ForArrayInfo f1; } ; struct Cyc_Absyn_Stmt{ void* r;
struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds; int
try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p= 0; static const
int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p=
1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{ int tag; unsigned char
f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern struct
_tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud;
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List* rgn_po, struct Cyc_List_List*); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* k); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* k); extern void* Cyc_Dict_fold( void*(* f)( void*, void*,
void*), struct Cyc_Dict_Dict* d, void* accum); extern void Cyc_Dict_iter_c( void(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; static const
int Cyc_Tcenv_AnonEnumRes= 4; struct Cyc_Tcenv_AnonEnumRes_struct{ int tag; void*
f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); struct Cyc_Interface_I;
extern struct Cyc_Interface_I* Cyc_Interface_empty(); extern struct Cyc_Interface_I*
Cyc_Interface_final(); extern struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae); struct _tuple3{ struct _tagged_arr f1; struct
_tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)( void*), struct
Cyc_List_List* la, struct Cyc_List_List* linfo); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Std___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Std___sFILE*); extern void Cyc_Interface_save(
struct Cyc_Interface_I*, struct Cyc_Std___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_load( struct Cyc_Std___sFILE*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
generate_line_directives: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); extern struct _tagged_arr
Cyc_Absynpp_qvar2string( struct _tuple0*); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int var_default_init, struct Cyc_List_List* ds); extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_arr lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_arr, int, void*); void* read_fun_state; } ;
struct Cyc_Lexing_lex_tables{ struct _tagged_arr lex_base; struct _tagged_arr
lex_backtrk; struct _tagged_arr lex_default; struct _tagged_arr lex_trans;
struct _tagged_arr lex_check; } ; extern struct Cyc_List_List* Cyc_Parse_parse_file(
struct Cyc_Std___sFILE* f); struct Cyc_Declaration_spec; struct Cyc_Declarator;
struct Cyc_Abstractdeclarator; extern int Cyc_yyparse(); extern unsigned char
Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{ unsigned char* tag;
int f1; } ; extern unsigned char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
unsigned char* tag; unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Declaration_spec* f1; } ; extern unsigned char
Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{ unsigned char* tag;
struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[ 23u];
struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple4{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple4* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple5{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple5* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple6{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple6* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple7{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple7* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple8{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple8* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
17u]; struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u];
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; extern
unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; extern int Cyc_yyparse();
extern unsigned char Cyc_YY1[ 8u]; struct Cyc_YY1_struct{ unsigned char* tag;
struct _tuple7* f1; } ; extern unsigned char Cyc_YYINITIALSVAL[ 18u]; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; extern int Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2);
extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_function_type( void* t); extern unsigned char
Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl*
base; struct Cyc_Absyn_Tunionfield* field; } ; extern void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg1, struct _tagged_arr
fmt, struct _tagged_arr ap); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports;
struct Cyc_Dict_Dict* exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){
return({ struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*)
_cycalloc( sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct
Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0;});} static struct Cyc_Interface_Ienv*
Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv* ie= 0; if( ie
==  0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();} return(
struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp1=( struct Cyc_Interface_I*) _cycalloc(
sizeof( struct Cyc_Interface_I)); _temp1->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I* Cyc_Interface_empty(){
return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*) _cycalloc(
sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp2;});}
struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I* _temp3=
Cyc_Interface_empty(); struct Cyc_Interface_Ienv* _temp4= Cyc_Interface_new_ienv();
struct Cyc_Absyn_Tuniondecl* _temp5=({ struct Cyc_Absyn_Tuniondecl* _temp20=(
struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp20[ 0]=* Cyc_Absyn_exn_tud; _temp20;});( void*)( _temp5->sc=( void*)(( void*)
Cyc_Absyn_Public)); _temp4->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl* v)) Cyc_Dict_insert)(
_temp4->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp5);{ struct Cyc_List_List*
_temp6=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_exn_tud->fields))->v;
for( 0; _temp6 !=  0; _temp6= _temp6->tl){ struct Cyc_Absyn_Tunionfield* _temp7=({
struct Cyc_Absyn_Tunionfield* _temp10=( struct Cyc_Absyn_Tunionfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Tunionfield)); _temp10[ 0]=*(( struct Cyc_Absyn_Tunionfield*)
_temp6->hd); _temp10;});( void*)( _temp7->sc=( void*)(( void*) Cyc_Absyn_Public));{
struct Cyc_Tcdecl_Xtunionfielddecl* _temp8=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp9=( struct Cyc_Tcdecl_Xtunionfielddecl*) _cycalloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp9->base= _temp5; _temp9->field= _temp7; _temp9;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Tcdecl_Xtunionfielddecl* v)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*((
struct Cyc_Absyn_Tunionfield*) _temp6->hd)->name).f2, _temp8);}}} _temp3->exports=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* k,
struct Cyc_Interface_Ienv* v)) Cyc_Dict_insert)( _temp3->exports, 0, _temp4);{
struct Cyc_Interface_Ienv* _temp11= Cyc_Interface_new_ienv(); struct _tuple0*
qmain=({ struct _tuple0* _temp17=( struct _tuple0*) _cycalloc( sizeof( struct
_tuple0)); _temp17->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp18=(
struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp18[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp19; _temp19.tag= Cyc_Absyn_Abs_n;
_temp19.f1= 0; _temp19;}); _temp18;}); _temp17->f2= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"main", sizeof(
unsigned char), 5u); _temp17;}); struct Cyc_Absyn_Vardecl* _temp12= Cyc_Absyn_new_vardecl(
qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*)({ struct _tuple1* _temp16=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp16->f1= 0; _temp16->f2= Cyc_Absyn_empty_tqual();
_temp16->f3= Cyc_Absyn_uint_t; _temp16;}); _temp13->tl=({ struct Cyc_List_List*
_temp14=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp14->hd=( void*)({ struct _tuple1* _temp15=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp15->f1= 0; _temp15->f2= Cyc_Absyn_empty_tqual();
_temp15->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp15;}); _temp14->tl= 0; _temp14;}); _temp13;}), 0,
0, 0, 0), 0);( void*)( _temp12->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp11->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Vardecl* v)) Cyc_Dict_insert)( _temp11->vardecls,(* qmain).f2, _temp12);
_temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k, struct Cyc_Interface_Ienv* v)) Cyc_Dict_insert)( _temp3->exports, 0, _temp11);
return _temp3;}} static void Cyc_Interface_err( struct _tagged_arr msg){({ void*
_temp21[ 0u]={}; Cyc_Tcutil_terr( 0, msg, _tag_arr( _temp21, sizeof( void*), 0u));});}
static void Cyc_Interface_fields_err( struct _tagged_arr sc, struct _tagged_arr
t, struct _tuple0* n){ Cyc_Interface_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp25; _temp25.tag= Cyc_Std_String_pa; _temp25.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
n);{ struct Cyc_Std_String_pa_struct _temp24; _temp24.tag= Cyc_Std_String_pa;
_temp24.f1=( struct _tagged_arr) t;{ struct Cyc_Std_String_pa_struct _temp23;
_temp23.tag= Cyc_Std_String_pa; _temp23.f1=( struct _tagged_arr) sc;{ void*
_temp22[ 3u]={& _temp23,& _temp24,& _temp25}; Cyc_Std_aprintf( _tag_arr("fields of %s %s %s have never been defined",
sizeof( unsigned char), 43u), _tag_arr( _temp22, sizeof( void*), 3u));}}}}));}
static void Cyc_Interface_body_err( struct _tagged_arr sc, struct _tuple0* n){
Cyc_Interface_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp28; _temp28.tag= Cyc_Std_String_pa; _temp28.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
n);{ struct Cyc_Std_String_pa_struct _temp27; _temp27.tag= Cyc_Std_String_pa;
_temp27.f1=( struct _tagged_arr) sc;{ void* _temp26[ 2u]={& _temp27,& _temp28};
Cyc_Std_aprintf( _tag_arr("the body of %s function %s has never been defined",
sizeof( unsigned char), 50u), _tag_arr( _temp26, sizeof( void*), 2u));}}}));}
static void Cyc_Interface_static_err( struct _tagged_arr obj1, struct _tuple0*
name1, struct _tagged_arr obj2, struct _tuple0* name2){ if( obj1.curr != ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Interface_err(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp33; _temp33.tag= Cyc_Std_String_pa;
_temp33.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( name2);{ struct Cyc_Std_String_pa_struct
_temp32; _temp32.tag= Cyc_Std_String_pa; _temp32.f1=( struct _tagged_arr) obj2;{
struct Cyc_Std_String_pa_struct _temp31; _temp31.tag= Cyc_Std_String_pa; _temp31.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( name1);{ struct Cyc_Std_String_pa_struct
_temp30; _temp30.tag= Cyc_Std_String_pa; _temp30.f1=( struct _tagged_arr) obj1;{
void* _temp29[ 4u]={& _temp30,& _temp31,& _temp32,& _temp33}; Cyc_Std_aprintf(
_tag_arr("declaration of %s %s relies on static %s %s", sizeof( unsigned char),
44u), _tag_arr( _temp29, sizeof( void*), 4u));}}}}}));}} static void Cyc_Interface_abstract_err(
struct _tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct
_tuple0* name2){ if( obj1.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_Interface_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp38; _temp38.tag= Cyc_Std_String_pa; _temp38.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
name2);{ struct Cyc_Std_String_pa_struct _temp37; _temp37.tag= Cyc_Std_String_pa;
_temp37.f1=( struct _tagged_arr) obj2;{ struct Cyc_Std_String_pa_struct _temp36;
_temp36.tag= Cyc_Std_String_pa; _temp36.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
name1);{ struct Cyc_Std_String_pa_struct _temp35; _temp35.tag= Cyc_Std_String_pa;
_temp35.f1=( struct _tagged_arr) obj1;{ void* _temp34[ 4u]={& _temp35,& _temp36,&
_temp37,& _temp38}; Cyc_Std_aprintf( _tag_arr("declaration of %s %s relies on fields of abstract %s %s",
sizeof( unsigned char), 56u), _tag_arr( _temp34, sizeof( void*), 4u));}}}}}));}}
struct Cyc_Interface_Seen{ struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict*
unions; struct Cyc_Dict_Dict* tunions; } ; static struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){
return({ struct Cyc_Interface_Seen* _temp39=( struct Cyc_Interface_Seen*)
_cycalloc( sizeof( struct Cyc_Interface_Seen)); _temp39->structs=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp39->unions=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp39->tunions=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp39;});} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void* t); static int Cyc_Interface_check_public_type_list( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l){ int _temp40= 1; for( 0; l != 
0; l= l->tl){ if( ! Cyc_Interface_check_public_type( ae, seen, obj, name, f((
void*) l->hd))){ _temp40= 0;}} return _temp40;} static int Cyc_Interface_check_public_structdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl*
d){ struct _handler_cons _temp41; _push_handler(& _temp41);{ int _temp43= 0; if(
setjmp( _temp41.handler)){ _temp43= 1;} if( ! _temp43){{ int _temp44=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp44;}; _pop_handler();} else{ void* _temp42=(
void*) _exn_thrown; void* _temp46= _temp42; _LL48: if( _temp46 ==  Cyc_Dict_Absent){
goto _LL49;} else{ goto _LL50;} _LL50: goto _LL51; _LL49: { int _temp52= 1; seen->structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)( seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp52); if( d->fields !=  0){ struct Cyc_List_List*
_temp53=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp53 !=  0; _temp53= _temp53->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)
_temp53->hd)->type)){ _temp52= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k, int v)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp52);
return _temp52;} _LL51:( void) _throw( _temp46); _LL47:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl*
d){ struct _handler_cons _temp54; _push_handler(& _temp54);{ int _temp56= 0; if(
setjmp( _temp54.handler)){ _temp56= 1;} if( ! _temp56){{ int _temp57=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp57;}; _pop_handler();} else{ void* _temp55=(
void*) _exn_thrown; void* _temp59= _temp55; _LL61: if( _temp59 ==  Cyc_Dict_Absent){
goto _LL62;} else{ goto _LL63;} _LL63: goto _LL64; _LL62: { int _temp65= 1; seen->unions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)( seen->unions,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp65); if( d->fields !=  0){ struct Cyc_List_List*
_temp66=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp66 !=  0; _temp66= _temp66->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)
_temp66->hd)->type)){ _temp65= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k, int v)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp65);
return _temp65;} _LL64:( void) _throw( _temp59); _LL60:;}}} struct _tuple9{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple9* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp67; _push_handler(& _temp67);{ int _temp69= 0; if(
setjmp( _temp67.handler)){ _temp69= 1;} if( ! _temp69){{ int _temp70=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( seen->tunions, d->name);
_npop_handler( 0u); return _temp70;}; _pop_handler();} else{ void* _temp68=(
void*) _exn_thrown; void* _temp72= _temp68; _LL74: if( _temp72 ==  Cyc_Dict_Absent){
goto _LL75;} else{ goto _LL76;} _LL76: goto _LL77; _LL75: { int _temp78= 1; seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)( seen->tunions, d->name, _temp78); if( d->fields !=  0){ struct
Cyc_List_List* _temp79=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; for( 0; _temp79 !=  0; _temp79= _temp79->tl){ if( !((
int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct
_tagged_arr obj, struct _tuple0* name, void*(* f)( struct _tuple9*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)
_temp79->hd)->typs)){ _temp78= 0;}}} seen->tunions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k, int v)) Cyc_Dict_insert)( seen->tunions,
d->name, _temp78); return _temp78;} _LL77:( void) _throw( _temp72); _LL73:;}}}
static int Cyc_Interface_check_public_enumdecl( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl* d){ return 1;} static int
Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Typedefdecl* d){ return Cyc_Interface_check_public_type(
ae, seen,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u), d->name,( void*) d->defn);}
static int Cyc_Interface_check_public_vardecl( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl* d){ return Cyc_Interface_check_public_type(
ae, seen, _tag_arr("variable", sizeof( unsigned char), 9u), d->name,( void*) d->type);}
static void* Cyc_Interface_get_type2( struct _tuple1* x){ return(* x).f3;}
static struct Cyc_List_List* Cyc_Interface_get_abs_ns( void* ns){ void* _temp80=
ns; struct Cyc_List_List* _temp86; _LL82: if(( unsigned int) _temp80 >  1u?*((
int*) _temp80) ==  Cyc_Absyn_Abs_n: 0){ _LL87: _temp86=(( struct Cyc_Absyn_Abs_n_struct*)
_temp80)->f1; goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL83: return
_temp86; _LL85:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp88=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp88[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp89; _temp89.tag= Cyc_Core_Invalid_argument; _temp89.f1= _tag_arr("get_abs_ns",
sizeof( unsigned char), 11u); _temp89;}); _temp88;})); _LL81:;} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void* t){ void* _temp90= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp114; void* _temp116; void* _temp118; struct Cyc_Core_Opt*
_temp120; struct Cyc_Core_Opt _temp122; void* _temp123; struct Cyc_Absyn_FnInfo
_temp125; struct Cyc_Absyn_VarargInfo* _temp127; struct Cyc_List_List* _temp129;
void* _temp131; struct Cyc_List_List* _temp133; struct Cyc_Absyn_Structdecl**
_temp135; struct Cyc_Absyn_Structdecl* _temp137; struct Cyc_List_List* _temp138;
struct Cyc_Absyn_Uniondecl** _temp140; struct Cyc_Absyn_Uniondecl* _temp142;
struct Cyc_List_List* _temp143; struct _tuple0* _temp145; struct Cyc_Absyn_TunionInfo
_temp147; struct Cyc_List_List* _temp149; void* _temp151; struct Cyc_Absyn_Tuniondecl**
_temp153; struct Cyc_Absyn_Tuniondecl* _temp155; struct Cyc_Absyn_TunionFieldInfo
_temp156; struct Cyc_List_List* _temp158; void* _temp160; struct Cyc_Absyn_Tunionfield*
_temp162; struct Cyc_Absyn_Tuniondecl* _temp164; _LL92: if(( unsigned int)
_temp90 >  3u?*(( int*) _temp90) ==  Cyc_Absyn_PointerType: 0){ _LL115: _temp114=((
struct Cyc_Absyn_PointerType_struct*) _temp90)->f1; _LL117: _temp116=( void*)
_temp114.elt_typ; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int)
_temp90 >  3u?*(( int*) _temp90) ==  Cyc_Absyn_ArrayType: 0){ _LL119: _temp118=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp90)->f1; goto _LL95;} else{
goto _LL96;} _LL96: if(( unsigned int) _temp90 >  3u?*(( int*) _temp90) ==  Cyc_Absyn_TypedefType:
0){ _LL121: _temp120=(( struct Cyc_Absyn_TypedefType_struct*) _temp90)->f3; if(
_temp120 ==  0){ goto _LL98;} else{ _temp122=* _temp120; _LL124: _temp123=( void*)
_temp122.v; goto _LL97;}} else{ goto _LL98;} _LL98: if(( unsigned int) _temp90 > 
3u?*(( int*) _temp90) ==  Cyc_Absyn_FnType: 0){ _LL126: _temp125=(( struct Cyc_Absyn_FnType_struct*)
_temp90)->f1; _LL132: _temp131=( void*) _temp125.ret_typ; goto _LL130; _LL130:
_temp129= _temp125.args; goto _LL128; _LL128: _temp127= _temp125.cyc_varargs;
goto _LL99;} else{ goto _LL100;} _LL100: if(( unsigned int) _temp90 >  3u?*((
int*) _temp90) ==  Cyc_Absyn_TupleType: 0){ _LL134: _temp133=(( struct Cyc_Absyn_TupleType_struct*)
_temp90)->f1; goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int)
_temp90 >  3u?*(( int*) _temp90) ==  Cyc_Absyn_StructType: 0){ _LL139: _temp138=((
struct Cyc_Absyn_StructType_struct*) _temp90)->f2; goto _LL136; _LL136: _temp135=((
struct Cyc_Absyn_StructType_struct*) _temp90)->f3; if( _temp135 ==  0){ goto
_LL104;} else{ _temp137=* _temp135; goto _LL103;}} else{ goto _LL104;} _LL104:
if(( unsigned int) _temp90 >  3u?*(( int*) _temp90) ==  Cyc_Absyn_UnionType: 0){
_LL144: _temp143=(( struct Cyc_Absyn_UnionType_struct*) _temp90)->f2; goto
_LL141; _LL141: _temp140=(( struct Cyc_Absyn_UnionType_struct*) _temp90)->f3;
if( _temp140 ==  0){ goto _LL106;} else{ _temp142=* _temp140; goto _LL105;}}
else{ goto _LL106;} _LL106: if(( unsigned int) _temp90 >  3u?*(( int*) _temp90)
==  Cyc_Absyn_EnumType: 0){ _LL146: _temp145=(( struct Cyc_Absyn_EnumType_struct*)
_temp90)->f1; goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int)
_temp90 >  3u?*(( int*) _temp90) ==  Cyc_Absyn_TunionType: 0){ _LL148: _temp147=((
struct Cyc_Absyn_TunionType_struct*) _temp90)->f1; _LL152: _temp151=( void*)
_temp147.tunion_info; if(*(( int*) _temp151) ==  Cyc_Absyn_KnownTunion){ _LL154:
_temp153=(( struct Cyc_Absyn_KnownTunion_struct*) _temp151)->f1; _temp155=*
_temp153; goto _LL150;} else{ goto _LL110;} _LL150: _temp149= _temp147.targs;
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp90 >  3u?*((
int*) _temp90) ==  Cyc_Absyn_TunionFieldType: 0){ _LL157: _temp156=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp90)->f1; _LL161: _temp160=( void*) _temp156.field_info; if(*(( int*)
_temp160) ==  Cyc_Absyn_KnownTunionfield){ _LL165: _temp164=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp160)->f1; goto _LL163; _LL163: _temp162=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp160)->f2; goto _LL159;} else{ goto _LL112;} _LL159: _temp158= _temp156.targs;
goto _LL111;} else{ goto _LL112;} _LL112: goto _LL113; _LL93: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp116); _LL95: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp118); _LL97: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp123); _LL99: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp129)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp131): 0; if( _temp127 !=  0){ void* _temp168; struct
Cyc_Absyn_VarargInfo _temp166=* _temp127; _LL169: _temp168=( void*) _temp166.type;
goto _LL167; _LL167: b= Cyc_Interface_check_public_type( ae, seen, obj, name,
_temp168);} return b;} _LL101: return(( int(*)( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name, void*(*
f)( struct _tuple9*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp133); _LL103: if(( void*)
_temp137->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp137->name))->v); return 0;} return Cyc_Interface_check_public_type_list(
ae, seen, obj, name, Cyc_Core_identity, _temp138)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp137): 0; _LL105: if(( void*) _temp142->sc == ( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp142->name))->v);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp143)? Cyc_Interface_check_public_uniondecl( ae, seen, _temp142): 0; _LL107: {
struct _tuple0 _temp172; struct _tagged_arr* _temp173; void* _temp175; struct
_tuple0* _temp170= _temp145; _temp172=* _temp170; _LL176: _temp175= _temp172.f1;
goto _LL174; _LL174: _temp173= _temp172.f2; goto _LL171; _LL171: { struct Cyc_List_List*
_temp177= Cyc_Interface_get_abs_ns( _temp175); struct Cyc_Absyn_Enumdecl* ed;{
struct _handler_cons _temp178; _push_handler(& _temp178);{ int _temp180= 0; if(
setjmp( _temp178.handler)){ _temp180= 1;} if( ! _temp180){{ struct Cyc_Tcenv_Genv*
_temp181=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( ae, _temp177); ed=*(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp181->enumdecls,
_temp173);}; _pop_handler();} else{ void* _temp179=( void*) _exn_thrown; void*
_temp183= _temp179; _LL185: if( _temp183 ==  Cyc_Dict_Absent){ goto _LL186;}
else{ goto _LL187;} _LL187: goto _LL188; _LL186:( int) _throw(( void*)({ struct
Cyc_Core_Invalid_argument_struct* _temp189=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp189[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp190; _temp190.tag= Cyc_Core_Invalid_argument;
_temp190.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp192;
_temp192.tag= Cyc_Std_String_pa; _temp192.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp145);{ void* _temp191[ 1u]={& _temp192}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find enum %s)",
sizeof( unsigned char), 39u), _tag_arr( _temp191, sizeof( void*), 1u));}});
_temp190;}); _temp189;})); _LL188:( void) _throw( _temp183); _LL184:;}}} if((
void*) ed->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
_temp145, _tag_arr("enum", sizeof( unsigned char), 5u), ed->name); return 0;}
return 1;}} _LL109: { struct _tuple0 _temp195; struct _tagged_arr* _temp196;
void* _temp198; struct _tuple0* _temp193= _temp155->name; _temp195=* _temp193;
_LL199: _temp198= _temp195.f1; goto _LL197; _LL197: _temp196= _temp195.f2; goto
_LL194; _LL194: { struct Cyc_List_List* _temp200= Cyc_Interface_get_abs_ns(
_temp198); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp201;
_push_handler(& _temp201);{ int _temp203= 0; if( setjmp( _temp201.handler)){
_temp203= 1;} if( ! _temp203){{ struct Cyc_Tcenv_Genv* _temp204=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( ae,
_temp200); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp204->tuniondecls, _temp196);};
_pop_handler();} else{ void* _temp202=( void*) _exn_thrown; void* _temp206=
_temp202; _LL208: if( _temp206 ==  Cyc_Dict_Absent){ goto _LL209;} else{ goto
_LL210;} _LL210: goto _LL211; _LL209:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp212=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp212[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp213; _temp213.tag= Cyc_Core_Invalid_argument; _temp213.f1=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp215; _temp215.tag= Cyc_Std_String_pa;
_temp215.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp155->name);{
void* _temp214[ 1u]={& _temp215}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find [x]tunion %s)",
sizeof( unsigned char), 44u), _tag_arr( _temp214, sizeof( void*), 1u));}});
_temp213;}); _temp212;})); _LL211:( void) _throw( _temp206); _LL207:;}}} if((
void*) tud->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name); return 0;}
return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp149)? Cyc_Interface_check_public_tuniondecl( ae, seen, tud): 0;}} _LL111: {
struct _tuple0 _temp218; struct _tagged_arr* _temp219; void* _temp221; struct
_tuple0* _temp216= _temp164->name; _temp218=* _temp216; _LL222: _temp221=
_temp218.f1; goto _LL220; _LL220: _temp219= _temp218.f2; goto _LL217; _LL217: {
struct Cyc_List_List* _temp223= Cyc_Interface_get_abs_ns( _temp221); struct Cyc_Absyn_Tuniondecl*
tud;{ struct _handler_cons _temp224; _push_handler(& _temp224);{ int _temp226= 0;
if( setjmp( _temp224.handler)){ _temp226= 1;} if( ! _temp226){{ struct Cyc_Tcenv_Genv*
_temp227=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( ae, _temp223); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp227->tuniondecls,
_temp219);}; _pop_handler();} else{ void* _temp225=( void*) _exn_thrown; void*
_temp229= _temp225; _LL231: if( _temp229 ==  Cyc_Dict_Absent){ goto _LL232;}
else{ goto _LL233;} _LL233: goto _LL234; _LL232:( int) _throw(( void*)({ struct
Cyc_Core_Invalid_argument_struct* _temp235=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp235[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp236; _temp236.tag= Cyc_Core_Invalid_argument;
_temp236.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp238;
_temp238.tag= Cyc_Std_String_pa; _temp238.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp164->name);{ void* _temp237[ 1u]={& _temp238}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find [x]tunion %s and search its fields)",
sizeof( unsigned char), 66u), _tag_arr( _temp237, sizeof( void*), 1u));}});
_temp236;}); _temp235;})); _LL234:( void) _throw( _temp229); _LL230:;}}} if( tud->fields
==  0){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp239=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp239[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp240; _temp240.tag= Cyc_Core_Invalid_argument; _temp240.f1=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp242; _temp242.tag= Cyc_Std_String_pa;
_temp242.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp241[ 1u]={& _temp242}; Cyc_Std_aprintf( _tag_arr("check_public_type ([x]tunion %s has no fields)",
sizeof( unsigned char), 47u), _tag_arr( _temp241, sizeof( void*), 1u));}});
_temp240;}); _temp239;}));}{ struct Cyc_Absyn_Tunionfield* tuf1= 0;{ struct Cyc_List_List*
_temp243=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp243 !=  0; _temp243= _temp243->tl){ if( Cyc_Std_strptrcmp((*
_temp162->name).f2,(*(( struct Cyc_Absyn_Tunionfield*) _temp243->hd)->name).f2)
==  0){ tuf1=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)
_temp243->hd); break;}}} if( tuf1 ==  0){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp244=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp244[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp245; _temp245.tag= Cyc_Core_Invalid_argument; _temp245.f1=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp247; _temp247.tag= Cyc_Std_String_pa;
_temp247.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp162->name);{
void* _temp246[ 1u]={& _temp247}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find [x]tunionfield %s)",
sizeof( unsigned char), 49u), _tag_arr( _temp246, sizeof( void*), 1u));}});
_temp245;}); _temp244;}));}{ struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*)
_check_null( tuf1); if(( void*) tud->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tud->sc == ( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tuf->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp249;
_temp249.tag= Cyc_Std_String_pa; _temp249.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp248[ 1u]={& _temp249}; Cyc_Std_aprintf( _tag_arr("field %s of",
sizeof( unsigned char), 12u), _tag_arr( _temp248, sizeof( void*), 1u));}}), tud->name);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp158)? Cyc_Interface_check_public_tuniondecl( ae, seen, tud): 0;}}}} _LL113:
return 1; _LL91:;} struct _tuple10{ struct Cyc_Interface_Ienv* f1; struct Cyc_Interface_Ienv*
f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen* f5; struct Cyc_Interface_I*
f6; } ; static void Cyc_Interface_extract_structdecl( struct _tuple10* env,
struct _tagged_arr* x, struct Cyc_Absyn_Structdecl** dp){ struct _tuple10
_temp252; struct Cyc_Interface_Seen* _temp253; struct Cyc_Dict_Dict* _temp255;
int _temp257; struct Cyc_Interface_Ienv* _temp259; struct Cyc_Interface_Ienv*
_temp261; struct _tuple10* _temp250= env; _temp252=* _temp250; _LL262: _temp261=
_temp252.f1; goto _LL260; _LL260: _temp259= _temp252.f2; goto _LL258; _LL258:
_temp257= _temp252.f3; goto _LL256; _LL256: _temp255= _temp252.f4; goto _LL254;
_LL254: _temp253= _temp252.f5; goto _LL251; _LL251: { struct Cyc_Absyn_Structdecl*
_temp263=* dp; void* _temp264=( void*) _temp263->sc; _LL266: if( _temp264 == (
void*) Cyc_Absyn_Static){ goto _LL267;} else{ goto _LL268;} _LL268: if( _temp264
== ( void*) Cyc_Absyn_Abstract){ goto _LL269;} else{ goto _LL270;} _LL270: if(
_temp264 == ( void*) Cyc_Absyn_Public){ goto _LL271;} else{ goto _LL272;} _LL272:
if( _temp264 == ( void*) Cyc_Absyn_ExternC){ goto _LL273;} else{ goto _LL274;}
_LL274: if( _temp264 == ( void*) Cyc_Absyn_Extern){ goto _LL275;} else{ goto
_LL265;} _LL267: if( _temp257? _temp263->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp263->name))->v);} goto _LL265; _LL269: if( _temp263->fields ==  0){ if(
_temp257){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp263->name))->v);}} else{ _temp263=({ struct Cyc_Absyn_Structdecl*
_temp276=( struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp276[ 0]=* _temp263; _temp276;}); _temp263->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp255, _temp253, _temp263)){ _temp259->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl* v))
Cyc_Dict_insert)( _temp259->structdecls, x, _temp263);} goto _LL265; _LL271: if(
_temp263->fields ==  0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp263->name))->v); _temp263=({
struct Cyc_Absyn_Structdecl* _temp277=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp277[ 0]=* _temp263; _temp277;});(
void*)( _temp263->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl(
_temp255, _temp253, _temp263)){ _temp259->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl* v))
Cyc_Dict_insert)( _temp259->structdecls, x, _temp263);} goto _LL265; _LL273:
goto _LL275; _LL275: if( Cyc_Interface_check_public_structdecl( _temp255,
_temp253, _temp263)){ _temp261->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl* v)) Cyc_Dict_insert)(
_temp261->structdecls, x, _temp263);} goto _LL265; _LL265:;}} static void Cyc_Interface_extract_uniondecl(
struct _tuple10* env, struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl** dp){
struct _tuple10 _temp280; struct Cyc_Interface_Seen* _temp281; struct Cyc_Dict_Dict*
_temp283; int _temp285; struct Cyc_Interface_Ienv* _temp287; struct Cyc_Interface_Ienv*
_temp289; struct _tuple10* _temp278= env; _temp280=* _temp278; _LL290: _temp289=
_temp280.f1; goto _LL288; _LL288: _temp287= _temp280.f2; goto _LL286; _LL286:
_temp285= _temp280.f3; goto _LL284; _LL284: _temp283= _temp280.f4; goto _LL282;
_LL282: _temp281= _temp280.f5; goto _LL279; _LL279: { struct Cyc_Absyn_Uniondecl*
_temp291=* dp; void* _temp292=( void*) _temp291->sc; _LL294: if( _temp292 == (
void*) Cyc_Absyn_Static){ goto _LL295;} else{ goto _LL296;} _LL296: if( _temp292
== ( void*) Cyc_Absyn_Abstract){ goto _LL297;} else{ goto _LL298;} _LL298: if(
_temp292 == ( void*) Cyc_Absyn_Public){ goto _LL299;} else{ goto _LL300;} _LL300:
if( _temp292 == ( void*) Cyc_Absyn_ExternC){ goto _LL301;} else{ goto _LL302;}
_LL302: if( _temp292 == ( void*) Cyc_Absyn_Extern){ goto _LL303;} else{ goto
_LL293;} _LL295: if( _temp285? _temp291->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp291->name))->v);} goto _LL293; _LL297: if( _temp291->fields ==  0){ if(
_temp285){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp291->name))->v);}} else{ _temp291=({ struct Cyc_Absyn_Uniondecl*
_temp304=( struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp304[ 0]=* _temp291; _temp304;}); _temp291->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp283, _temp281, _temp291)){ _temp287->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl* v))
Cyc_Dict_insert)( _temp287->uniondecls, x, _temp291);} goto _LL293; _LL299: if(
_temp291->fields ==  0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("union", sizeof( unsigned char), 6u),( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp291->name))->v); _temp291=({
struct Cyc_Absyn_Uniondecl* _temp305=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp305[ 0]=* _temp291; _temp305;});(
void*)( _temp291->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl(
_temp283, _temp281, _temp291)){ _temp287->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl* v))
Cyc_Dict_insert)( _temp287->uniondecls, x, _temp291);} goto _LL293; _LL301: goto
_LL303; _LL303: if( Cyc_Interface_check_public_uniondecl( _temp283, _temp281,
_temp291)){ _temp289->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl* v)) Cyc_Dict_insert)(
_temp289->uniondecls, x, _temp291);} goto _LL293; _LL293:;}} static void Cyc_Interface_extract_enumdecl(
struct _tuple10* env, struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple10 _temp308; struct Cyc_Interface_Seen* _temp309; struct Cyc_Dict_Dict*
_temp311; int _temp313; struct Cyc_Interface_Ienv* _temp315; struct Cyc_Interface_Ienv*
_temp317; struct _tuple10* _temp306= env; _temp308=* _temp306; _LL318: _temp317=
_temp308.f1; goto _LL316; _LL316: _temp315= _temp308.f2; goto _LL314; _LL314:
_temp313= _temp308.f3; goto _LL312; _LL312: _temp311= _temp308.f4; goto _LL310;
_LL310: _temp309= _temp308.f5; goto _LL307; _LL307: { struct Cyc_Absyn_Enumdecl*
_temp319=* dp; void* _temp320=( void*) _temp319->sc; _LL322: if( _temp320 == (
void*) Cyc_Absyn_Static){ goto _LL323;} else{ goto _LL324;} _LL324: if( _temp320
== ( void*) Cyc_Absyn_Abstract){ goto _LL325;} else{ goto _LL326;} _LL326: if(
_temp320 == ( void*) Cyc_Absyn_Public){ goto _LL327;} else{ goto _LL328;} _LL328:
if( _temp320 == ( void*) Cyc_Absyn_ExternC){ goto _LL329;} else{ goto _LL330;}
_LL330: if( _temp320 == ( void*) Cyc_Absyn_Extern){ goto _LL331;} else{ goto
_LL321;} _LL323: if( _temp313? _temp319->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("enum", sizeof(
unsigned char), 5u), _temp319->name);} goto _LL321; _LL325: if( _temp319->fields
==  0){ if( _temp313){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp319->name);}}
else{ _temp319=({ struct Cyc_Absyn_Enumdecl* _temp332=( struct Cyc_Absyn_Enumdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp332[ 0]=* _temp319;
_temp332;}); _temp319->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp311, _temp309, _temp319)){ _temp315->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Enumdecl* v))
Cyc_Dict_insert)( _temp315->enumdecls, x, _temp319);} goto _LL321; _LL327: if(
_temp319->fields ==  0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp319->name);
_temp319=({ struct Cyc_Absyn_Enumdecl* _temp333=( struct Cyc_Absyn_Enumdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp333[ 0]=* _temp319;
_temp333;});( void*)( _temp319->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_enumdecl( _temp311, _temp309, _temp319)){ _temp315->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Enumdecl* v)) Cyc_Dict_insert)( _temp315->enumdecls, x, _temp319);}
goto _LL321; _LL329: goto _LL331; _LL331: if( Cyc_Interface_check_public_enumdecl(
_temp311, _temp309, _temp319)){ _temp317->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Enumdecl* v))
Cyc_Dict_insert)( _temp317->enumdecls, x, _temp319);} goto _LL321; _LL321:;}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp334= 0;{ void* _temp335=(* f->name).f1; struct Cyc_List_List* _temp343;
struct Cyc_List_List* _temp345; _LL337: if(( unsigned int) _temp335 >  1u?*((
int*) _temp335) ==  Cyc_Absyn_Rel_n: 0){ _LL344: _temp343=(( struct Cyc_Absyn_Rel_n_struct*)
_temp335)->f1; goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int)
_temp335 >  1u?*(( int*) _temp335) ==  Cyc_Absyn_Abs_n: 0){ _LL346: _temp345=((
struct Cyc_Absyn_Abs_n_struct*) _temp335)->f1; goto _LL340;} else{ goto _LL341;}
_LL341: goto _LL342; _LL338: _temp345= _temp343; goto _LL340; _LL340: _temp334=
_temp345; goto _LL336; _LL342: goto _LL336; _LL336:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp347=( void*) f->sc; _LL349: if( _temp347 == ( void*) Cyc_Absyn_Static){
goto _LL350;} else{ goto _LL351;} _LL351: if( _temp347 == ( void*) Cyc_Absyn_Extern){
goto _LL352;} else{ goto _LL353;} _LL353: if( _temp347 == ( void*) Cyc_Absyn_Public){
goto _LL354;} else{ goto _LL355;} _LL355: goto _LL356; _LL350: return; _LL352:
dict=& i->imports; goto _LL348; _LL354: dict=& i->exports; goto _LL348; _LL356:(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp357=(
struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp357[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp358; _temp358.tag=
Cyc_Core_Invalid_argument; _temp358.f1= _tag_arr("add_xtunionfielddecl", sizeof(
unsigned char), 21u); _temp358;}); _temp357;})); _LL348:;}{ struct Cyc_Interface_Ienv*
env;{ struct _handler_cons _temp359; _push_handler(& _temp359);{ int _temp361= 0;
if( setjmp( _temp359.handler)){ _temp361= 1;} if( ! _temp361){ env=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)(* dict,
_temp334);; _pop_handler();} else{ void* _temp360=( void*) _exn_thrown; void*
_temp363= _temp360; _LL365: if( _temp363 ==  Cyc_Dict_Absent){ goto _LL366;}
else{ goto _LL367;} _LL367: goto _LL368; _LL366: env= Cyc_Interface_new_ienv();*
dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k, struct Cyc_Interface_Ienv* v)) Cyc_Dict_insert)(* dict, _temp334, env); goto
_LL364; _LL368:( void) _throw( _temp363); _LL364:;}}} env->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Tcdecl_Xtunionfielddecl* v)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({
struct Cyc_Tcdecl_Xtunionfielddecl* _temp369=( struct Cyc_Tcdecl_Xtunionfielddecl*)
_cycalloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp369->base= d;
_temp369->field= f; _temp369;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple10* env, struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple10 _temp372; struct Cyc_Interface_I* _temp373; struct Cyc_Interface_Seen*
_temp375; struct Cyc_Dict_Dict* _temp377; int _temp379; struct Cyc_Interface_Ienv*
_temp381; struct Cyc_Interface_Ienv* _temp383; struct _tuple10* _temp370= env;
_temp372=* _temp370; _LL384: _temp383= _temp372.f1; goto _LL382; _LL382:
_temp381= _temp372.f2; goto _LL380; _LL380: _temp379= _temp372.f3; goto _LL378;
_LL378: _temp377= _temp372.f4; goto _LL376; _LL376: _temp375= _temp372.f5; goto
_LL374; _LL374: _temp373= _temp372.f6; goto _LL371; _LL371: { struct Cyc_Absyn_Tuniondecl*
_temp385=* dp; void* _temp386=( void*) _temp385->sc; _LL388: if( _temp386 == (
void*) Cyc_Absyn_Static){ goto _LL389;} else{ goto _LL390;} _LL390: if( _temp386
== ( void*) Cyc_Absyn_Abstract){ goto _LL391;} else{ goto _LL392;} _LL392: if(
_temp386 == ( void*) Cyc_Absyn_Public){ goto _LL393;} else{ goto _LL394;} _LL394:
if( _temp386 == ( void*) Cyc_Absyn_ExternC){ goto _LL395;} else{ goto _LL396;}
_LL396: if( _temp386 == ( void*) Cyc_Absyn_Extern){ goto _LL397;} else{ goto
_LL387;} _LL389: if(( ! _temp385->is_xtunion? _temp379: 0)? _temp385->fields == 
0: 0){ Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp385->name);} goto _LL387;
_LL391: if( _temp385->fields ==  0){ if( ! _temp385->is_xtunion? _temp379: 0){
Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char), 9u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp385->name);}} else{
_temp385=({ struct Cyc_Absyn_Tuniondecl* _temp398=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp398[ 0]=* _temp385;
_temp398;}); _temp385->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp377, _temp375, _temp385)){ _temp381->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl* v))
Cyc_Dict_insert)( _temp381->tuniondecls, x, _temp385);} goto _LL387; _LL393:
_temp385=({ struct Cyc_Absyn_Tuniondecl* _temp399=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp399[ 0]=* _temp385;
_temp399;}); if( ! _temp385->is_xtunion? _temp385->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("public", sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof(
unsigned char), 7u), _temp385->name);( void*)( _temp385->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl( _temp377,
_temp375, _temp385)){ if( _temp385->is_xtunion? _temp385->fields !=  0: 0){
struct Cyc_List_List* _temp400=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp385->fields))->v; _temp385->fields= 0;{ struct Cyc_List_List*
_temp401= _temp400; for( 0; _temp401 !=  0; _temp401= _temp401->tl){ Cyc_Interface_extract_xtunionfielddecl(
_temp373, _temp385,( struct Cyc_Absyn_Tunionfield*) _temp401->hd);}}} _temp381->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Tuniondecl* v)) Cyc_Dict_insert)( _temp381->tuniondecls, x, _temp385);}
goto _LL387; _LL395:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp402=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp402[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp403; _temp403.tag= Cyc_Core_Invalid_argument; _temp403.f1= _tag_arr("extract_tuniondecl",
sizeof( unsigned char), 19u); _temp403;}); _temp402;})); _LL397: if( Cyc_Interface_check_public_tuniondecl(
_temp377, _temp375, _temp385)){ if( _temp385->is_xtunion? _temp385->fields !=  0:
0){ _temp385=({ struct Cyc_Absyn_Tuniondecl* _temp404=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp404[ 0]=* _temp385;
_temp404;});{ struct Cyc_List_List* _temp405=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp385->fields))->v; _temp385->fields= 0;{ struct
Cyc_List_List* _temp406= _temp405; for( 0; _temp406 !=  0; _temp406= _temp406->tl){
Cyc_Interface_extract_xtunionfielddecl( _temp373, _temp385,( struct Cyc_Absyn_Tunionfield*)
_temp406->hd);}}}} _temp383->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl* v)) Cyc_Dict_insert)(
_temp383->tuniondecls, x, _temp385);} goto _LL387; _LL387:;}} static void Cyc_Interface_extract_typedef(
struct _tuple10* env, struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d){
struct _tuple10 _temp409; struct Cyc_Interface_Seen* _temp410; struct Cyc_Dict_Dict*
_temp412; struct Cyc_Interface_Ienv* _temp414; struct _tuple10* _temp407= env;
_temp409=* _temp407; _LL415: _temp414= _temp409.f2; goto _LL413; _LL413:
_temp412= _temp409.f4; goto _LL411; _LL411: _temp410= _temp409.f5; goto _LL408;
_LL408: if( Cyc_Interface_check_public_typedefdecl( _temp412, _temp410, d)){
_temp414->typedefdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k, struct Cyc_Absyn_Typedefdecl* v)) Cyc_Dict_insert)(
_temp414->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple10* env, struct _tagged_arr* x, struct _tuple6* v){ struct _tuple10
_temp418; struct Cyc_Interface_Seen* _temp419; struct Cyc_Dict_Dict* _temp421;
int _temp423; struct Cyc_Interface_Ienv* _temp425; struct Cyc_Interface_Ienv*
_temp427; struct _tuple10* _temp416= env; _temp418=* _temp416; _LL428: _temp427=
_temp418.f1; goto _LL426; _LL426: _temp425= _temp418.f2; goto _LL424; _LL424:
_temp423= _temp418.f3; goto _LL422; _LL422: _temp421= _temp418.f4; goto _LL420;
_LL420: _temp419= _temp418.f5; goto _LL417; _LL417: { void* _temp429=(* v).f1;
void* _temp430= _temp429; void* _temp436; _LL432: if(*(( int*) _temp430) ==  Cyc_Tcenv_VarRes){
_LL437: _temp436=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp430)->f1; goto
_LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL433:{ void* _temp438=
_temp436; struct Cyc_Absyn_Fndecl* _temp446; struct Cyc_Absyn_Vardecl* _temp448;
_LL440: if(( unsigned int) _temp438 >  1u?*(( int*) _temp438) ==  Cyc_Absyn_Funname_b:
0){ _LL447: _temp446=(( struct Cyc_Absyn_Funname_b_struct*) _temp438)->f1; goto
_LL441;} else{ goto _LL442;} _LL442: if(( unsigned int) _temp438 >  1u?*(( int*)
_temp438) ==  Cyc_Absyn_Global_b: 0){ _LL449: _temp448=(( struct Cyc_Absyn_Global_b_struct*)
_temp438)->f1; goto _LL443;} else{ goto _LL444;} _LL444: goto _LL445; _LL441: {
struct Cyc_Absyn_Vardecl* _temp450=({ struct Cyc_Absyn_Vardecl* _temp451=(
struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp451->sc=( void*)(( void*) _temp446->sc); _temp451->name= _temp446->name;
_temp451->tq=({ struct Cyc_Absyn_Tqual _temp452; _temp452.q_const= 0; _temp452.q_volatile=
0; _temp452.q_restrict= 0; _temp452;}); _temp451->type=( void*)(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp446->cached_typ))->v); _temp451->initializer=
0; _temp451->rgn= 0; _temp451->attributes= 0; _temp451;}); _temp423= 0; _temp448=
_temp450; goto _LL443;} _LL443: if( _temp448->initializer !=  0){ _temp448=({
struct Cyc_Absyn_Vardecl* _temp453=( struct Cyc_Absyn_Vardecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp453[ 0]=* _temp448; _temp453;});
_temp448->initializer= 0;}{ void* _temp454=( void*) _temp448->sc; _LL456: if(
_temp454 == ( void*) Cyc_Absyn_Static){ goto _LL457;} else{ goto _LL458;} _LL458:
if( _temp454 == ( void*) Cyc_Absyn_Abstract){ goto _LL459;} else{ goto _LL460;}
_LL460: if( _temp454 == ( void*) Cyc_Absyn_Public){ goto _LL461;} else{ goto
_LL462;} _LL462: if( _temp454 == ( void*) Cyc_Absyn_ExternC){ goto _LL463;}
else{ goto _LL464;} _LL464: if( _temp454 == ( void*) Cyc_Absyn_Extern){ goto
_LL465;} else{ goto _LL455;} _LL457: if( _temp423? Cyc_Tcutil_is_function_type((
void*) _temp448->type): 0){ Cyc_Interface_body_err( _tag_arr("static", sizeof(
unsigned char), 7u), _temp448->name);} goto _LL455; _LL459:( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp466=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp466[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp467; _temp467.tag= Cyc_Core_Invalid_argument;
_temp467.f1= _tag_arr("extract_ordinarie", sizeof( unsigned char), 18u);
_temp467;}); _temp466;})); _LL461: if( _temp423? Cyc_Tcutil_is_function_type((
void*) _temp448->type): 0){ Cyc_Interface_body_err( _tag_arr("public", sizeof(
unsigned char), 7u), _temp448->name);} if( Cyc_Interface_check_public_vardecl(
_temp421, _temp419, _temp448)){ _temp425->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Vardecl* v))
Cyc_Dict_insert)( _temp425->vardecls, x, _temp448);} goto _LL455; _LL463: goto
_LL465; _LL465: if( Cyc_Interface_check_public_vardecl( _temp421, _temp419,
_temp448)){ _temp427->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct Cyc_Absyn_Vardecl* v)) Cyc_Dict_insert)(
_temp427->vardecls, x, _temp448);} goto _LL455; _LL455:;} goto _LL439; _LL445:
goto _LL439; _LL439:;} goto _LL431; _LL435: goto _LL431; _LL431:;}} struct
_tuple11{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple11*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple11
_temp470; struct Cyc_Interface_Seen* _temp471; struct Cyc_Dict_Dict* _temp473;
int _temp475; struct Cyc_Interface_I* _temp477; struct _tuple11* _temp468= env_f;
_temp470=* _temp468; _LL478: _temp477= _temp470.f1; goto _LL476; _LL476:
_temp475= _temp470.f2; goto _LL474; _LL474: _temp473= _temp470.f3; goto _LL472;
_LL472: _temp471= _temp470.f4; goto _LL469; _LL469: { struct _tuple10 _temp479=({
struct _tuple10 _temp480; _temp480.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( _temp477->imports,
ns); _temp480.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* k)) Cyc_Dict_lookup)( _temp477->exports, ns); _temp480.f3=
_temp475; _temp480.f4= _temp473; _temp480.f5= _temp471; _temp480.f6= _temp477;
_temp480;});(( void(*)( void(* f)( struct _tuple10*, struct _tagged_arr*, struct
Cyc_Absyn_Structdecl**), struct _tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_structdecl,& _temp479, ge->structdecls);(( void(*)( void(*
f)( struct _tuple10*, struct _tagged_arr*, struct Cyc_Absyn_Uniondecl**), struct
_tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
_temp479, ge->uniondecls);(( void(*)( void(* f)( struct _tuple10*, struct
_tagged_arr*, struct Cyc_Absyn_Tuniondecl**), struct _tuple10* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& _temp479, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple10*, struct _tagged_arr*, struct Cyc_Absyn_Enumdecl**),
struct _tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
_temp479, ge->enumdecls);(( void(*)( void(* f)( struct _tuple10*, struct
_tagged_arr*, struct Cyc_Absyn_Typedefdecl*), struct _tuple10* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& _temp479, ge->typedefs);((
void(*)( void(* f)( struct _tuple10*, struct _tagged_arr*, struct _tuple6*),
struct _tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
_temp479, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple11 _temp481=({
struct _tuple11 _temp482; _temp482.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp482.f2= check_complete_defs; _temp482.f3=
ae; _temp482.f4= Cyc_Interface_new_seen(); _temp482;});(( void(*)( void(* f)(
struct _tuple11*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct
_tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp481, ae); return _temp481.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_arr* msg1, struct
_tagged_arr msg2){({ struct Cyc_Std_String_pa_struct _temp484; _temp484.tag= Cyc_Std_String_pa;
_temp484.f1=( struct _tagged_arr) msg2;{ void* _temp483[ 1u]={& _temp484}; Cyc_Tcdecl_merr(
0, msg1, _tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp483, sizeof(
void*), 1u));}});} struct _tuple12{ int f1; struct Cyc_Dict_Dict* f2; int(* f3)(
void*, void*, struct _tagged_arr*); struct _tagged_arr f4; struct _tagged_arr*
f5; } ; static void Cyc_Interface_incl_dict_f( struct _tuple12* env, struct
_tagged_arr* x, void* y1){ struct _tuple12 _temp487; struct _tagged_arr*
_temp488; struct _tagged_arr _temp490; int(* _temp492)( void*, void*, struct
_tagged_arr*); struct Cyc_Dict_Dict* _temp494; int _temp496; int* _temp498;
struct _tuple12* _temp485= env; _temp487=* _temp485; _LL497: _temp496= _temp487.f1;
_temp498=( int*)&(* _temp485).f1; goto _LL495; _LL495: _temp494= _temp487.f2;
goto _LL493; _LL493: _temp492= _temp487.f3; goto _LL491; _LL491: _temp490=
_temp487.f4; goto _LL489; _LL489: _temp488= _temp487.f5; goto _LL486; _LL486: {
struct _handler_cons _temp499; _push_handler(& _temp499);{ int _temp501= 0; if(
setjmp( _temp499.handler)){ _temp501= 1;} if( ! _temp501){{ void* _temp502=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp494, x); if( ! _temp492( y1, _temp502, _temp488)){* _temp498= 0;}};
_pop_handler();} else{ void* _temp500=( void*) _exn_thrown; void* _temp504=
_temp500; _LL506: if( _temp504 ==  Cyc_Dict_Absent){ goto _LL507;} else{ goto
_LL508;} _LL508: goto _LL509; _LL507: Cyc_Interface_check_err( _temp488,( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp512; _temp512.tag= Cyc_Std_String_pa;
_temp512.f1=( struct _tagged_arr)* x;{ struct Cyc_Std_String_pa_struct _temp511;
_temp511.tag= Cyc_Std_String_pa; _temp511.f1=( struct _tagged_arr) _temp490;{
void* _temp510[ 2u]={& _temp511,& _temp512}; Cyc_Std_aprintf( _tag_arr("%s %s is missing",
sizeof( unsigned char), 17u), _tag_arr( _temp510, sizeof( void*), 2u));}}}));*
_temp498= 0; goto _LL505; _LL509:( void) _throw( _temp504); _LL505:;}}}} static
int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( void*, void*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg){ struct _tuple12 _temp513=({ struct _tuple12 _temp514;
_temp514.f1= 1; _temp514.f2= dic2; _temp514.f3= incl_f; _temp514.f4= t; _temp514.f5=
msg; _temp514;});(( void(*)( void(* f)( struct _tuple12*, struct _tagged_arr*,
void*), struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_dict_f,&
_temp513, dic1); return _temp513.f1;} static int Cyc_Interface_incl_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_arr* msg){ struct Cyc_Absyn_Structdecl* _temp515= Cyc_Tcdecl_merge_structdecl(
d0, d1, 0, msg); if( _temp515 ==  0){ return 0;} if(( struct Cyc_Absyn_Structdecl*)
d0 !=  _temp515){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
Cyc_Std_String_pa_struct _temp517; _temp517.tag= Cyc_Std_String_pa; _temp517.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v);{ void* _temp516[ 1u]={& _temp517}; Cyc_Std_aprintf(
_tag_arr("declaration of struct %s discloses too much information", sizeof(
unsigned char), 56u), _tag_arr( _temp516, sizeof( void*), 1u));}})); return 0;}
return 1;} static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl*
d0, struct Cyc_Absyn_Uniondecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Uniondecl*
_temp518= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); if( _temp518 ==  0){
return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 !=  _temp518){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp520; _temp520.tag=
Cyc_Std_String_pa; _temp520.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v);{ void*
_temp519[ 1u]={& _temp520}; Cyc_Std_aprintf( _tag_arr("declaration of union %s discloses too much information",
sizeof( unsigned char), 55u), _tag_arr( _temp519, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl*
d0, struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Tuniondecl*
_temp521= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); if( _temp521 ==  0){
return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 !=  _temp521){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp523; _temp523.tag=
Cyc_Std_String_pa; _temp523.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp522[ 1u]={& _temp523}; Cyc_Std_aprintf( _tag_arr("declaration of tunion %s discloses too much information",
sizeof( unsigned char), 56u), _tag_arr( _temp522, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Enumdecl*
_temp524= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); if( _temp524 ==  0){
return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 !=  _temp524){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp526; _temp526.tag=
Cyc_Std_String_pa; _temp526.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp525[ 1u]={& _temp526}; Cyc_Std_aprintf( _tag_arr("declaration of enum %s discloses too much information",
sizeof( unsigned char), 54u), _tag_arr( _temp525, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl*
d0, struct Cyc_Absyn_Vardecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Vardecl*
_temp527= Cyc_Tcdecl_merge_vardecl( d0, d1, 0, msg); if( _temp527 ==  0){ return
0;} if(( struct Cyc_Absyn_Vardecl*) d0 !=  _temp527){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp529; _temp529.tag=
Cyc_Std_String_pa; _temp529.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp528[ 1u]={& _temp529}; Cyc_Std_aprintf( _tag_arr("declaration of variable %s discloses too much information",
sizeof( unsigned char), 58u), _tag_arr( _temp528, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl*
d0, struct Cyc_Absyn_Typedefdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Typedefdecl*
_temp530= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); if( _temp530 ==  0){
return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 !=  _temp530){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp532; _temp532.tag=
Cyc_Std_String_pa; _temp532.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp531[ 1u]={& _temp532}; Cyc_Std_aprintf( _tag_arr("declaration of typedef %s discloses too much information",
sizeof( unsigned char), 57u), _tag_arr( _temp531, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl*
d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp533= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1, 0, msg); if( _temp533 ==  0){
return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*) d0 !=  _temp533){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp535; _temp535.tag=
Cyc_Std_String_pa; _temp535.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
d1->field)->name);{ void* _temp534[ 1u]={& _temp535}; Cyc_Std_aprintf( _tag_arr("declaration of xtunionfield %s discloses too much information",
sizeof( unsigned char), 62u), _tag_arr( _temp534, sizeof( void*), 1u));}}));
return 0;} return 1;} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, void*(* merge_f)( void*, void*, struct Cyc_Position_Segment*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_arr* msg){ int _temp536=(( int(*)( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl, _tag_arr("struct",
sizeof( unsigned char), 7u), msg); int _temp537=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl, _tag_arr("union", sizeof( unsigned char), 6u), msg);
int _temp538=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl, _tag_arr("tunion",
sizeof( unsigned char), 7u), msg); int _temp539=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); int _temp540=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg) !=  0; int _temp541=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); int _temp542=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); return((((( _temp536? _temp537: 0)? _temp538: 0)?
_temp539: 0)? _temp540: 0)? _temp541: 0)? _temp542: 0;} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; struct _tagged_arr* f3; } ; static void Cyc_Interface_incl_ns_f(
struct _tuple13* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){
struct _tuple13 _temp545; struct _tagged_arr* _temp546; struct Cyc_Dict_Dict*
_temp548; int _temp550; int* _temp552; struct _tuple13* _temp543= env; _temp545=*
_temp543; _LL551: _temp550= _temp545.f1; _temp552=( int*)&(* _temp543).f1; goto
_LL549; _LL549: _temp548= _temp545.f2; goto _LL547; _LL547: _temp546= _temp545.f3;
goto _LL544; _LL544: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons
_temp553; _push_handler(& _temp553);{ int _temp555= 0; if( setjmp( _temp553.handler)){
_temp555= 1;} if( ! _temp555){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( _temp548, ns);; _pop_handler();}
else{ void* _temp554=( void*) _exn_thrown; void* _temp557= _temp554; _LL559: if(
_temp557 ==  Cyc_Dict_Absent){ goto _LL560;} else{ goto _LL561;} _LL561: goto
_LL562; _LL560: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL558; _LL562:( void)
_throw( _temp557); _LL558:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp546)){*
_temp552= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple13 _temp563=({
struct _tuple13 _temp564; _temp564.f1= 1; _temp564.f2= dic2; _temp564.f3= msg;
_temp564;});(( void(*)( void(* f)( struct _tuple13*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple13* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _temp563, dic1); return _temp563.f1;} int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp565; _push_handler(& _temp565);{ int _temp567= 0; if(
setjmp( _temp565.handler)){ _temp567= 1;} if( ! _temp567){{ int _temp568= 1;
struct _tagged_arr* msg= 0; if( info !=  0){ msg=({ struct _tagged_arr* _temp569=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp569[ 0]=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp572; _temp572.tag=
Cyc_Std_String_pa; _temp572.f1=( struct _tagged_arr)(* info).f2;{ struct Cyc_Std_String_pa_struct
_temp571; _temp571.tag= Cyc_Std_String_pa; _temp571.f1=( struct _tagged_arr)(*
info).f1;{ void* _temp570[ 2u]={& _temp571,& _temp572}; Cyc_Std_aprintf(
_tag_arr("checking inclusion of %s exports into %s exports,", sizeof(
unsigned char), 50u), _tag_arr( _temp570, sizeof( void*), 2u));}}}); _temp569;});}
if( ! Cyc_Interface_incl_ns( i1->exports, i2->exports, msg)){ _temp568= 0;} if(
info !=  0){ msg=({ struct _tagged_arr* _temp573=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp573[ 0]=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp576; _temp576.tag= Cyc_Std_String_pa;
_temp576.f1=( struct _tagged_arr)(* info).f1;{ struct Cyc_Std_String_pa_struct
_temp575; _temp575.tag= Cyc_Std_String_pa; _temp575.f1=( struct _tagged_arr)(*
info).f2;{ void* _temp574[ 2u]={& _temp575,& _temp576}; Cyc_Std_aprintf(
_tag_arr("checking inclusion of %s imports into %s imports,", sizeof(
unsigned char), 50u), _tag_arr( _temp574, sizeof( void*), 2u));}}}); _temp573;});}
if( ! Cyc_Interface_incl_ns( i2->imports, i1->imports, msg)){ _temp568= 0;}{ int
_temp577= _temp568; _npop_handler( 0u); return _temp577;}}; _pop_handler();}
else{ void* _temp566=( void*) _exn_thrown; void* _temp579= _temp566; _LL581: if(
_temp579 ==  Cyc_Tcdecl_Incompatible){ goto _LL582;} else{ goto _LL583;} _LL583:
goto _LL584; _LL582: return 0; _LL584:( void) _throw( _temp579); _LL580:;}}}
struct _tuple14{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; void*(* f5)( void*, void*, struct Cyc_Position_Segment*,
struct _tagged_arr*); struct _tagged_arr f6; struct _tagged_arr* f7; } ; void
Cyc_Interface_compat_merge_dict_f( struct _tuple14* env, struct _tagged_arr* x,
void* y2){ struct _tuple14 _temp587; struct _tagged_arr* _temp588; struct
_tagged_arr _temp590; void*(* _temp592)( void*, void*, struct Cyc_Position_Segment*,
struct _tagged_arr*); struct Cyc_Dict_Dict* _temp594; struct Cyc_Dict_Dict*
_temp596; struct Cyc_Dict_Dict* _temp598; struct Cyc_Dict_Dict** _temp600; int
_temp601; int* _temp603; struct _tuple14* _temp585= env; _temp587=* _temp585;
_LL602: _temp601= _temp587.f1; _temp603=( int*)&(* _temp585).f1; goto _LL599;
_LL599: _temp598= _temp587.f2; _temp600=( struct Cyc_Dict_Dict**)&(* _temp585).f2;
goto _LL597; _LL597: _temp596= _temp587.f3; goto _LL595; _LL595: _temp594=
_temp587.f4; goto _LL593; _LL593: _temp592= _temp587.f5; goto _LL591; _LL591:
_temp590= _temp587.f6; goto _LL589; _LL589: _temp588= _temp587.f7; goto _LL586;
_LL586: { void* y;{ struct _handler_cons _temp604; _push_handler(& _temp604);{
int _temp606= 0; if( setjmp( _temp604.handler)){ _temp606= 1;} if( ! _temp606){{
void* _temp607=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp596, x); void* _temp608= _temp592( _temp607, y2, 0, _temp588); if( !((
unsigned int) _temp608)){* _temp603= 0; _npop_handler( 0u); return;} y=( void*)
_check_null( _temp608);}; _pop_handler();} else{ void* _temp605=( void*)
_exn_thrown; void* _temp610= _temp605; _LL612: if( _temp610 ==  Cyc_Dict_Absent){
goto _LL613;} else{ goto _LL614;} _LL614: goto _LL615; _LL613: y= y2; goto
_LL611; _LL615:( void) _throw( _temp610); _LL611:;}}}{ struct _handler_cons
_temp616; _push_handler(& _temp616);{ int _temp618= 0; if( setjmp( _temp616.handler)){
_temp618= 1;} if( ! _temp618){{ void* _temp619=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp594, x); void* _temp620=
_temp592( _temp619, y, 0, _temp588); if( _temp620 != ( void*) _temp619){ if((
unsigned int) _temp620){ Cyc_Interface_check_err( _temp588,( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp623; _temp623.tag= Cyc_Std_String_pa;
_temp623.f1=( struct _tagged_arr)* x;{ struct Cyc_Std_String_pa_struct _temp622;
_temp622.tag= Cyc_Std_String_pa; _temp622.f1=( struct _tagged_arr) _temp590;{
void* _temp621[ 2u]={& _temp622,& _temp623}; Cyc_Std_aprintf( _tag_arr("abstract %s %s is being imported as non-abstract",
sizeof( unsigned char), 49u), _tag_arr( _temp621, sizeof( void*), 2u));}}}));}*
_temp603= 0;}}; _pop_handler();} else{ void* _temp617=( void*) _exn_thrown; void*
_temp625= _temp617; _LL627: if( _temp625 ==  Cyc_Dict_Absent){ goto _LL628;}
else{ goto _LL629;} _LL629: goto _LL630; _LL628: if(* _temp603){* _temp600=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void*
v)) Cyc_Dict_insert)(* _temp600, x, y);} goto _LL626; _LL630:( void) _throw(
_temp625); _LL626:;}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, void*(* merge_f)( void*, void*, struct Cyc_Position_Segment*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple14
_temp631=({ struct _tuple14 _temp633; _temp633.f1= 1; _temp633.f2= dic1;
_temp633.f3= dic1; _temp633.f4= excl; _temp633.f5= merge_f; _temp633.f6= t;
_temp633.f7= msg; _temp633;});(( void(*)( void(* f)( struct _tuple14*, struct
_tagged_arr*, void*), struct _tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_dict_f,& _temp631, dic2); if( _temp631.f1){ return({
struct Cyc_Core_Opt* _temp632=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp632->v=( void*) _temp631.f2; _temp632;});} else{ return 0;}}
struct Cyc_Interface_Ienv* Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct Cyc_Interface_Ienv* iexcl, struct
_tagged_arr* msg){ struct Cyc_Core_Opt* _temp634=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp635=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,
_tag_arr("union", sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt*
_temp636=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)(
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
_tag_arr("tunion", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp637=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp638=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef", sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt*
_temp639=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp640=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp634)? 1: !((
unsigned int) _temp635))? 1: !(( unsigned int) _temp636))? 1: !(( unsigned int)
_temp637))? 1: !(( unsigned int) _temp638))? 1: !(( unsigned int) _temp639))? 1:
!(( unsigned int) _temp640)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp641=( struct Cyc_Interface_Ienv*) _cycalloc( sizeof( struct Cyc_Interface_Ienv));
_temp641->structdecls=( struct Cyc_Dict_Dict*) _temp634->v; _temp641->uniondecls=(
struct Cyc_Dict_Dict*) _temp635->v; _temp641->tuniondecls=( struct Cyc_Dict_Dict*)
_temp636->v; _temp641->enumdecls=( struct Cyc_Dict_Dict*) _temp637->v; _temp641->typedefdecls=(
struct Cyc_Dict_Dict*) _temp638->v; _temp641->vardecls=( struct Cyc_Dict_Dict*)
_temp639->v; _temp641->xtunionfielddecls=( struct Cyc_Dict_Dict*) _temp640->v;
_temp641;});}} struct _tuple15{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict*
f3; struct Cyc_Dict_Dict* f4; struct _tagged_arr* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple15* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple15 _temp644; struct _tagged_arr* _temp645; struct Cyc_Dict_Dict*
_temp647; struct Cyc_Dict_Dict* _temp649; struct Cyc_Dict_Dict* _temp651; struct
Cyc_Dict_Dict** _temp653; int _temp654; int* _temp656; struct _tuple15* _temp642=
env; _temp644=* _temp642; _LL655: _temp654= _temp644.f1; _temp656=( int*)&(*
_temp642).f1; goto _LL652; _LL652: _temp651= _temp644.f2; _temp653=( struct Cyc_Dict_Dict**)&(*
_temp642).f2; goto _LL650; _LL650: _temp649= _temp644.f3; goto _LL648; _LL648:
_temp647= _temp644.f4; goto _LL646; _LL646: _temp645= _temp644.f5; goto _LL643;
_LL643: { struct Cyc_Interface_Ienv* ie1; struct Cyc_Interface_Ienv* iexcl;{
struct _handler_cons _temp657; _push_handler(& _temp657);{ int _temp659= 0; if(
setjmp( _temp657.handler)){ _temp659= 1;} if( ! _temp659){ iexcl=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( _temp647,
ns);; _pop_handler();} else{ void* _temp658=( void*) _exn_thrown; void* _temp661=
_temp658; _LL663: if( _temp661 ==  Cyc_Dict_Absent){ goto _LL664;} else{ goto
_LL665;} _LL665: goto _LL666; _LL664: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL662; _LL666:( void) _throw( _temp661); _LL662:;}}}{ struct _handler_cons
_temp667; _push_handler(& _temp667);{ int _temp669= 0; if( setjmp( _temp667.handler)){
_temp669= 1;} if( ! _temp669){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( _temp649, ns);{ struct Cyc_Interface_Ienv*
_temp670= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp645); if( _temp670 ==  0){* _temp656= 0; _npop_handler( 0u); return;}
ie1=( struct Cyc_Interface_Ienv*) _check_null( _temp670);}; _pop_handler();}
else{ void* _temp668=( void*) _exn_thrown; void* _temp672= _temp668; _LL674: if(
_temp672 ==  Cyc_Dict_Absent){ goto _LL675;} else{ goto _LL676;} _LL676: goto
_LL677; _LL675: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL673; _LL677:( void)
_throw( _temp672); _LL673:;}}}{ struct Cyc_Interface_Ienv* _temp678= Cyc_Interface_compat_merge_ienv(
ie1, ie2, iexcl, _temp645); if( _temp678 ==  0){* _temp656= 0;} else{ if(*
_temp656){* _temp653=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* k, struct Cyc_Interface_Ienv* v)) Cyc_Dict_insert)(*
_temp653, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp678));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct _tagged_arr* msg){
struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); struct
_tuple15 _temp679=({ struct _tuple15 _temp681; _temp681.f1= 1; _temp681.f2= d;
_temp681.f3= dic1; _temp681.f4= excl; _temp681.f5= msg; _temp681;});(( void(*)(
void(* f)( struct _tuple15*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,&
_temp679, dic2); if( _temp679.f1){ return({ struct Cyc_Core_Opt* _temp680=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp680->v=(
void*) _temp679.f2; _temp680;});} else{ return 0;}} struct _tuple16{ int f1;
struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_arr f4;
struct _tagged_arr* f5; } ; void Cyc_Interface_disj_merge_dict_f( struct
_tuple16* env, struct _tagged_arr* x, void* y){ struct _tuple16 _temp684; struct
_tagged_arr* _temp685; struct _tagged_arr _temp687; struct Cyc_Dict_Dict*
_temp689; struct Cyc_Dict_Dict* _temp691; struct Cyc_Dict_Dict** _temp693; int
_temp694; int* _temp696; struct _tuple16* _temp682= env; _temp684=* _temp682;
_LL695: _temp694= _temp684.f1; _temp696=( int*)&(* _temp682).f1; goto _LL692;
_LL692: _temp691= _temp684.f2; _temp693=( struct Cyc_Dict_Dict**)&(* _temp682).f2;
goto _LL690; _LL690: _temp689= _temp684.f3; goto _LL688; _LL688: _temp687=
_temp684.f4; goto _LL686; _LL686: _temp685= _temp684.f5; goto _LL683; _LL683:
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_member)(
_temp689, x)){ Cyc_Interface_check_err( _temp685,( struct _tagged_arr)({ struct
Cyc_Std_String_pa_struct _temp699; _temp699.tag= Cyc_Std_String_pa; _temp699.f1=(
struct _tagged_arr)* x;{ struct Cyc_Std_String_pa_struct _temp698; _temp698.tag=
Cyc_Std_String_pa; _temp698.f1=( struct _tagged_arr) _temp687;{ void* _temp697[
2u]={& _temp698,& _temp699}; Cyc_Std_aprintf( _tag_arr("%s %s is exported more than once",
sizeof( unsigned char), 33u), _tag_arr( _temp697, sizeof( void*), 2u));}}}));*
_temp696= 0;} else{ if(* _temp696){* _temp693=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(*
_temp693, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct
_tagged_arr* msg){ struct _tuple16 _temp700=({ struct _tuple16 _temp702;
_temp702.f1= 1; _temp702.f2= dic1; _temp702.f3= dic1; _temp702.f4= t; _temp702.f5=
msg; _temp702;});(( void(*)( void(* f)( struct _tuple16*, struct _tagged_arr*,
void*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_dict_f,&
_temp700, dic2); if( _temp700.f1){ return({ struct Cyc_Core_Opt* _temp701=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp701->v=(
void*) _temp700.f2; _temp701;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct _tagged_arr* msg){ struct Cyc_Core_Opt* _temp703=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls,(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp),
Cyc_Tcdecl_merge_structdecl, _tag_arr("struct", sizeof( unsigned char), 7u), msg);
struct Cyc_Core_Opt* _temp704=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls, _tag_arr("union",
sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt* _temp705=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls,
_tag_arr("[x]tunion", sizeof( unsigned char), 10u), msg); struct Cyc_Core_Opt*
_temp706=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->enumdecls, ie2->enumdecls, _tag_arr("enum", sizeof( unsigned char), 5u),
msg); struct Cyc_Core_Opt* _temp707=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp), Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef",
sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt* _temp708=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp709=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp703)? 1: !((
unsigned int) _temp704))? 1: !(( unsigned int) _temp705))? 1: !(( unsigned int)
_temp706))? 1: !(( unsigned int) _temp707))? 1: !(( unsigned int) _temp708))? 1:
!(( unsigned int) _temp709)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp710=( struct Cyc_Interface_Ienv*) _cycalloc( sizeof( struct Cyc_Interface_Ienv));
_temp710->structdecls=( struct Cyc_Dict_Dict*) _temp703->v; _temp710->uniondecls=(
struct Cyc_Dict_Dict*) _temp704->v; _temp710->tuniondecls=( struct Cyc_Dict_Dict*)
_temp705->v; _temp710->enumdecls=( struct Cyc_Dict_Dict*) _temp706->v; _temp710->typedefdecls=(
struct Cyc_Dict_Dict*) _temp707->v; _temp710->vardecls=( struct Cyc_Dict_Dict*)
_temp708->v; _temp710->xtunionfielddecls=( struct Cyc_Dict_Dict*) _temp709->v;
_temp710;});}} struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict*
f3; struct _tagged_arr* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct
_tuple17* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct
_tuple17 _temp713; struct _tagged_arr* _temp714; struct Cyc_Dict_Dict* _temp716;
struct Cyc_Dict_Dict* _temp718; struct Cyc_Dict_Dict** _temp720; int _temp721;
int* _temp723; struct _tuple17* _temp711= env; _temp713=* _temp711; _LL722:
_temp721= _temp713.f1; _temp723=( int*)&(* _temp711).f1; goto _LL719; _LL719:
_temp718= _temp713.f2; _temp720=( struct Cyc_Dict_Dict**)&(* _temp711).f2; goto
_LL717; _LL717: _temp716= _temp713.f3; goto _LL715; _LL715: _temp714= _temp713.f4;
goto _LL712; _LL712: { struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons
_temp724; _push_handler(& _temp724);{ int _temp726= 0; if( setjmp( _temp724.handler)){
_temp726= 1;} if( ! _temp726){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( _temp716, ns);; _pop_handler();}
else{ void* _temp725=( void*) _exn_thrown; void* _temp728= _temp725; _LL730: if(
_temp728 ==  Cyc_Dict_Absent){ goto _LL731;} else{ goto _LL732;} _LL732: goto
_LL733; _LL731: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL729; _LL733:( void)
_throw( _temp728); _LL729:;}}}{ struct Cyc_Interface_Ienv* _temp734= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp714); if( _temp734 ==  0){* _temp723= 0;} else{ if(* _temp723){*
_temp720=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k, struct Cyc_Interface_Ienv* v)) Cyc_Dict_insert)(* _temp720, ns,( struct Cyc_Interface_Ienv*)
_check_null( _temp734));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){
struct _tuple17 _temp735=({ struct _tuple17 _temp737; _temp737.f1= 1; _temp737.f2=
dic1; _temp737.f3= dic1; _temp737.f4= msg; _temp737;});(( void(*)( void(* f)(
struct _tuple17*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct
_tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp735, dic2); if( _temp735.f1){ return({ struct Cyc_Core_Opt* _temp736=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp736->v=(
void*) _temp735.f2; _temp736;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp738; _push_handler(& _temp738);{ int _temp740= 0; if(
setjmp( _temp738.handler)){ _temp740= 1;} if( ! _temp740){{ struct _tagged_arr*
msg= 0; if( info !=  0){ msg=({ struct _tagged_arr* _temp741=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp741[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp744; _temp744.tag= Cyc_Std_String_pa;
_temp744.f1=( struct _tagged_arr)(* info).f2;{ struct Cyc_Std_String_pa_struct
_temp743; _temp743.tag= Cyc_Std_String_pa; _temp743.f1=( struct _tagged_arr)(*
info).f1;{ void* _temp742[ 2u]={& _temp743,& _temp744}; Cyc_Std_aprintf(
_tag_arr("merging exports of %s and %s,", sizeof( unsigned char), 30u), _tag_arr(
_temp742, sizeof( void*), 2u));}}}); _temp741;});}{ struct Cyc_Core_Opt*
_temp745= Cyc_Interface_disj_merge_ns( i1->exports, i2->exports, msg); if(
_temp745 ==  0){ struct Cyc_Interface_I* _temp746= 0; _npop_handler( 0u); return
_temp746;} if( info !=  0){ msg=({ struct _tagged_arr* _temp747=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp747[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp750; _temp750.tag= Cyc_Std_String_pa;
_temp750.f1=( struct _tagged_arr)(* info).f2;{ struct Cyc_Std_String_pa_struct
_temp749; _temp749.tag= Cyc_Std_String_pa; _temp749.f1=( struct _tagged_arr)(*
info).f1;{ void* _temp748[ 2u]={& _temp749,& _temp750}; Cyc_Std_aprintf(
_tag_arr("merging imports of %s and %s,", sizeof( unsigned char), 30u), _tag_arr(
_temp748, sizeof( void*), 2u));}}}); _temp747;});}{ struct Cyc_Core_Opt*
_temp751= Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)
_temp745->v, msg); if( _temp751 ==  0){ struct Cyc_Interface_I* _temp752= 0;
_npop_handler( 0u); return _temp752;}{ struct Cyc_Interface_I* _temp754=({
struct Cyc_Interface_I* _temp753=( struct Cyc_Interface_I*) _cycalloc( sizeof(
struct Cyc_Interface_I)); _temp753->imports=( struct Cyc_Dict_Dict*) _temp751->v;
_temp753->exports=( struct Cyc_Dict_Dict*) _temp745->v; _temp753;});
_npop_handler( 0u); return _temp754;}}}}; _pop_handler();} else{ void* _temp739=(
void*) _exn_thrown; void* _temp756= _temp739; _LL758: if( _temp756 ==  Cyc_Tcdecl_Incompatible){
goto _LL759;} else{ goto _LL760;} _LL760: goto _LL761; _LL759: return 0; _LL761:(
void) _throw( _temp756); _LL757:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li ==  0){ return(
struct Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*) li->hd; struct _tagged_arr* curr_info= linfo !=  0?(
struct _tagged_arr*)(( struct _tagged_arr*) linfo->hd): 0; li= li->tl; if( linfo
!=  0){ linfo= linfo->tl;} for( 0; li !=  0; li= li->tl){ struct Cyc_Interface_I*
_temp762= Cyc_Interface_merge( curr_i,( struct Cyc_Interface_I*) li->hd,(
curr_info !=  0? linfo !=  0: 0)?({ struct _tuple3* _temp767=( struct _tuple3*)
_cycalloc( sizeof( struct _tuple3)); _temp767->f1=* curr_info; _temp767->f2=*((
struct _tagged_arr*) linfo->hd); _temp767;}): 0); if( _temp762 ==  0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp762); if( curr_info !=  0){
curr_info= linfo !=  0?({ struct _tagged_arr* _temp763=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp763[ 0]=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp766; _temp766.tag= Cyc_Std_String_pa;
_temp766.f1=( struct _tagged_arr)*(( struct _tagged_arr*) linfo->hd);{ struct
Cyc_Std_String_pa_struct _temp765; _temp765.tag= Cyc_Std_String_pa; _temp765.f1=(
struct _tagged_arr)* curr_info;{ void* _temp764[ 2u]={& _temp765,& _temp766};
Cyc_Std_aprintf( _tag_arr("%s+%s", sizeof( unsigned char), 6u), _tag_arr(
_temp764, sizeof( void*), 2u));}}}); _temp763;}): 0;} if( linfo !=  0){ linfo=
linfo->tl;}} return( struct Cyc_Interface_I*) curr_i;}} struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)( void*), struct
Cyc_List_List* la, struct Cyc_List_List* linfo){ if( la ==  0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i= get((
void*) la->hd); struct _tagged_arr* curr_info= linfo !=  0?( struct _tagged_arr*)((
struct _tagged_arr*) linfo->hd): 0; la= la->tl; if( linfo !=  0){ linfo= linfo->tl;}
for( 0; la !=  0; la= la->tl){ struct Cyc_Interface_I* _temp768= Cyc_Interface_merge(
curr_i, get(( void*) la->hd),( curr_info !=  0? linfo !=  0: 0)?({ struct
_tuple3* _temp773=( struct _tuple3*) _cycalloc( sizeof( struct _tuple3));
_temp773->f1=* curr_info; _temp773->f2=*(( struct _tagged_arr*) linfo->hd);
_temp773;}): 0); if( _temp768 ==  0){ return 0;} curr_i=( struct Cyc_Interface_I*)
_check_null( _temp768); if( curr_info !=  0){ curr_info= linfo !=  0?({ struct
_tagged_arr* _temp769=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp769[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp772; _temp772.tag= Cyc_Std_String_pa; _temp772.f1=( struct _tagged_arr)*((
struct _tagged_arr*) linfo->hd);{ struct Cyc_Std_String_pa_struct _temp771;
_temp771.tag= Cyc_Std_String_pa; _temp771.f1=( struct _tagged_arr)* curr_info;{
void* _temp770[ 2u]={& _temp771,& _temp772}; Cyc_Std_aprintf( _tag_arr("%s+%s",
sizeof( unsigned char), 6u), _tag_arr( _temp770, sizeof( void*), 2u));}}});
_temp769;}): 0;} if( linfo !=  0){ linfo= linfo->tl;}} return( struct Cyc_Interface_I*)
curr_i;}} static struct Cyc_List_List* Cyc_Interface_add_namespace( struct Cyc_List_List*
tds, struct Cyc_List_List* ns){ if( ns ==  0){ return tds;} return({ struct Cyc_List_List*
_temp774=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp774->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp775=( struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp775[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp776; _temp776.tag= Cyc_Absyn_Namespace_d;
_temp776.f1=( struct _tagged_arr*) ns->hd; _temp776.f2= Cyc_Interface_add_namespace(
tds, ns->tl); _temp776;}); _temp775;}), 0); _temp774->tl= 0; _temp774;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp777=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp777->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp778=( struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp778[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp779; _temp779.tag= Cyc_Absyn_Struct_d;
_temp779.f1= d; _temp779;}); _temp778;}), 0); _temp777->tl= tds; _temp777;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp780=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp780[ 0]=* d; _temp780;}); d->fields=
0; if(( void*) d->sc != ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp781=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp781->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp782=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp782[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp783; _temp783.tag= Cyc_Absyn_Struct_d; _temp783.f1=
d; _temp783;}); _temp782;}), 0); _temp781->tl= tds; _temp781;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp784=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp784->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp785=( struct
Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp785[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp786; _temp786.tag= Cyc_Absyn_Union_d;
_temp786.f1= d; _temp786;}); _temp785;}), 0); _temp784->tl= tds; _temp784;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp787=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp787[ 0]=* d; _temp787;}); d->fields=
0; if(( void*) d->sc != ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp788=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp788->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp789=( struct Cyc_Absyn_Union_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp789[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp790; _temp790.tag= Cyc_Absyn_Union_d; _temp790.f1= d; _temp790;}); _temp789;}),
0); _temp788->tl= tds; _temp788;});} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp791=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp791->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp792=( struct Cyc_Absyn_Tunion_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp792[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp793; _temp793.tag= Cyc_Absyn_Tunion_d; _temp793.f1=
d; _temp793;}); _temp792;}), 0); _temp791->tl= tds; _temp791;});} static
unsigned char _temp794[ 2u]="_"; static struct _tagged_arr Cyc_Interface_us={
_temp794, _temp794, _temp794 +  2u}; static struct _tagged_arr* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple9* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple9* x){ return({ struct _tuple9* _temp795=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp795->f1= Cyc_Absyn_empty_tqual();
_temp795->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp796=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp796[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp797; _temp797.tag= Cyc_Absyn_VarType; _temp797.f1=({ struct Cyc_Absyn_Tvar*
_temp798=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp798->name= Cyc_Interface_us_p; _temp798->identity= 0; _temp798->kind=( void*)((
void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp799=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp799[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp800; _temp800.tag= Cyc_Absyn_Eq_kb; _temp800.f1=( void*)(( void*) Cyc_Absyn_BoxKind);
_temp800;}); _temp799;})); _temp798;}); _temp797;}); _temp796;}); _temp795;});}
static struct Cyc_Absyn_Tunionfield* Cyc_Interface_rewrite_tunionfield( struct
Cyc_Absyn_Tunionfield* f){ f=({ struct Cyc_Absyn_Tunionfield* _temp801=( struct
Cyc_Absyn_Tunionfield*) _cycalloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp801[ 0]=* f; _temp801;}); f->typs=(( struct Cyc_List_List*(*)( struct
_tuple9*(* f)( struct _tuple9*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type,
f->typs); return f;} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
d=({ struct Cyc_Absyn_Tuniondecl* _temp802=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp802[ 0]=* d; _temp802;});
if( d->fields !=  0){ d->fields=({ struct Cyc_Core_Opt* _temp803=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp803->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)( d->fields)->v);
_temp803;});} if(( void*) d->sc != ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=(
void*)(( void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp804=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp804->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp805=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp805[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp806; _temp806.tag= Cyc_Absyn_Tunion_d;
_temp806.f1= d; _temp806;}); _temp805;}), 0); _temp804->tl= tds; _temp804;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp807=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp807->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp808=( struct Cyc_Absyn_Enum_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp808[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp809; _temp809.tag= Cyc_Absyn_Enum_d;
_temp809.f1= d; _temp809;}); _temp808;}), 0); _temp807->tl= tds; _temp807;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp810=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp810[ 0]=* d; _temp810;}); d->fields= 0;
if(( void*) d->sc != ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp811=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp811->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Enum_d_struct* _temp812=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp812[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp813; _temp813.tag= Cyc_Absyn_Enum_d; _temp813.f1= d; _temp813;}); _temp812;}),
0); _temp811->tl= tds; _temp811;});} static struct Cyc_List_List* Cyc_Interface_add_typedef(
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp814=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp814->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp815=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp815[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp816; _temp816.tag= Cyc_Absyn_Typedef_d; _temp816.f1=
d; _temp816;}); _temp815;}), 0); _temp814->tl= tds; _temp814;});} static struct
Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_arr* x, struct Cyc_Absyn_Vardecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp817=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp817->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp818=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp818[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp819; _temp819.tag= Cyc_Absyn_Var_d;
_temp819.f1= d; _temp819;}); _temp818;}), 0); _temp817->tl= tds; _temp817;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp822; struct Cyc_Absyn_Tunionfield*
_temp823; struct Cyc_Absyn_Tuniondecl* _temp825; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp820= d; _temp822=* _temp820; _LL826: _temp825= _temp822.base; goto _LL824;
_LL824: _temp823= _temp822.field; goto _LL821; _LL821: _temp825=({ struct Cyc_Absyn_Tuniondecl*
_temp827=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp827[ 0]=* _temp825; _temp827;}); _temp825->fields=({ struct Cyc_Core_Opt*
_temp828=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp828->v=( void*)({ struct Cyc_List_List* _temp829=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp829->hd=( void*) _temp823;
_temp829->tl= 0; _temp829;}); _temp828;});( void*)( _temp825->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp830=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp830->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp831=( struct Cyc_Absyn_Tunion_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp831[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp832; _temp832.tag= Cyc_Absyn_Tunion_d; _temp832.f1=
_temp825; _temp832;}); _temp831;}), 0); _temp830->tl= tds; _temp830;});} static
struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp835; struct Cyc_Absyn_Tunionfield*
_temp836; struct Cyc_Absyn_Tuniondecl* _temp838; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp833= d; _temp835=* _temp833; _LL839: _temp838= _temp835.base; goto _LL837;
_LL837: _temp836= _temp835.field; goto _LL834; _LL834: _temp838=({ struct Cyc_Absyn_Tuniondecl*
_temp840=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp840[ 0]=* _temp838; _temp840;}); _temp836= Cyc_Interface_rewrite_tunionfield(
_temp836);( void*)( _temp836->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp838->fields=({
struct Cyc_Core_Opt* _temp841=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp841->v=( void*)({ struct Cyc_List_List* _temp842=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp842->hd=( void*)
_temp836; _temp842->tl= 0; _temp842;}); _temp841;});( void*)( _temp838->sc=(
void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp843=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp843->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp844=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp844[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp845; _temp845.tag= Cyc_Absyn_Tunion_d;
_temp845.f1= _temp838; _temp845;}); _temp844;}), 0); _temp843->tl= tds; _temp843;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Std___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp846= 0; _temp846=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, _temp846); _temp846=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp846); _temp846=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp846); _temp846=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp846); if( _temp846 !=  0){ _temp846=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp846); _temp846= Cyc_Interface_add_namespace(
_temp846, ns); Cyc_Absynpp_decllist2file( _temp846, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp847= 0; _temp847=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header, ie->xtunionfielddecls,
_temp847); if( _temp847 !=  0){ _temp847=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp847); _temp847= Cyc_Interface_add_namespace(
_temp847, ns); Cyc_Absynpp_decllist2file( _temp847, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp848= 0; _temp848=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Typedefdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls, _temp848); if(
_temp848 !=  0){ _temp848=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp848); _temp848= Cyc_Interface_add_namespace( _temp848,
ns); Cyc_Absynpp_decllist2file( _temp848, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp849= 0; _temp849=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp849);
_temp849=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp849); _temp849=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp849); _temp849=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp849); _temp849=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp849); _temp849=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp849); if( _temp849 !=  0){ _temp849=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp849); _temp849= Cyc_Interface_add_namespace(
_temp849, ns); Cyc_Absynpp_decllist2file( _temp849, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Std___sFILE* f){ Cyc_Absynpp_set_params(&
Cyc_Absynpp_cyci_params_r);({ void* _temp850[ 0u]={}; Cyc_Std_fprintf( f,
_tag_arr("/****** needed (headers) ******/\n", sizeof( unsigned char), 34u),
_tag_arr( _temp850, sizeof( void*), 0u));});(( void(*)( void(* f)( struct Cyc_Std___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Std___sFILE* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers, f, i->imports);({
void* _temp851[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (headers) ******/\n",
sizeof( unsigned char), 37u), _tag_arr( _temp851, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports);({ void* _temp852[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("/****** needed (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 55u), _tag_arr( _temp852, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->imports);({ void* _temp853[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 58u), _tag_arr( _temp853, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->exports);({ void* _temp854[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (typedefs) ******/\n",
sizeof( unsigned char), 38u), _tag_arr( _temp854, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports);({ void* _temp855[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** needed (declarations) ******/\n",
sizeof( unsigned char), 40u), _tag_arr( _temp855, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->imports);({ void* _temp856[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (declarations) ******/\n",
sizeof( unsigned char), 42u), _tag_arr( _temp856, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Std___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp857= Cyc_Parse_parse_file( f);
struct Cyc_Tcenv_Tenv* _temp858= Cyc_Tcenv_tc_init(); Cyc_Tc_tc( _temp858, 0,
_temp857); return Cyc_Interface_gen_extract( _temp858->ae, 0);}} void Cyc_Interface_save(
struct Cyc_Interface_I* i, struct Cyc_Std___sFILE* f){ Cyc_Interface_print( i, f);}
struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Std___sFILE* f){ return
Cyc_Interface_parse( f);}
