// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
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

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
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
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
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
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_vfprintf( struct
Cyc_Std___sFILE*, struct _tagged_arr fmt, struct _tagged_arr ap); static const
int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short*
f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
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
14u]; static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0;
struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1;
static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
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
print_full_evars: 1; int generate_line_directives: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*);
extern struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); static
const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar*
t); extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_unify( void*, void*); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern
void* Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ;
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; extern unsigned char Cyc_CfFlowInfo_IsZero[
11u]; extern unsigned char Cyc_CfFlowInfo_NotZero[ 12u]; extern unsigned char
Cyc_CfFlowInfo_UnknownZ[ 13u]; static const int Cyc_CfFlowInfo_PlaceL= 0; struct
Cyc_CfFlowInfo_PlaceL_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1; } ;
static const int Cyc_CfFlowInfo_UnknownL= 0; static const int Cyc_CfFlowInfo_Zero=
0; static const int Cyc_CfFlowInfo_NotZeroAll= 1; static const int Cyc_CfFlowInfo_NotZeroThis=
2; static const int Cyc_CfFlowInfo_UnknownR= 0; struct Cyc_CfFlowInfo_UnknownR_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
} ; extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List* ds); extern
struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List* Cyc_Toc_result_decls=
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
void*)& Cyc_Toc__throw_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static
struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=& Cyc_Toc__throw_ev; static unsigned char
_temp9[ 7u]="setjmp"; static struct _tagged_arr Cyc_Toc_setjmp_str={ _temp9,
_temp9, _temp9 +  7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc_setjmp_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,&
Cyc_Toc_setjmp_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,( void*)(( void*)& Cyc_Toc_setjmp_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev; static
unsigned char _temp11[ 14u]="_push_handler"; static struct _tagged_arr Cyc_Toc__push_handler_str={
_temp11, _temp11, _temp11 +  14u}; static struct _tuple0 Cyc_Toc__push_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={ 1u,& Cyc_Toc__push_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__push_handler_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp13[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp13, _temp13, _temp13 +  13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__pop_handler_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;
static unsigned char _temp15[ 12u]="_exn_thrown"; static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_temp15, _temp15, _temp15 +  12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__exn_thrown_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={ 1u,& Cyc_Toc__exn_thrown_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={ 0,(
void*)(( void*)& Cyc_Toc__exn_thrown_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp17[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp17, _temp17, _temp17 +  14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static unsigned char _temp19[ 12u]="_check_null"; static struct _tagged_arr Cyc_Toc__check_null_str={
_temp19, _temp19, _temp19 +  12u}; static struct _tuple0 Cyc_Toc__check_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={ 1u,& Cyc_Toc__check_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_null_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;
static unsigned char _temp21[ 28u]="_check_known_subscript_null"; static struct
_tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp21, _temp21, _temp21
+  28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str}; static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,& Cyc_Toc__check_known_subscript_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re), 0,( void*)(( void*)
Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=&
Cyc_Toc__check_known_subscript_null_ev; static unsigned char _temp23[ 31u]="_check_known_subscript_notnull";
static struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp23,
_temp23, _temp23 +  31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp25[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp25, _temp25, _temp25 + 
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev; static unsigned char _temp27[ 12u]="_tagged_arr";
static struct _tagged_arr Cyc_Toc__tagged_arr_str={ _temp27, _temp27, _temp27 + 
12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={
1u,& Cyc_Toc__tagged_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_re), 0,( void*)((
void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=&
Cyc_Toc__tagged_arr_ev; static unsigned char _temp29[ 9u]="_tag_arr"; static
struct _tagged_arr Cyc_Toc__tag_arr_str={ _temp29, _temp29, _temp29 +  9u};
static struct _tuple0 Cyc_Toc__tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={
1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,( void*)(( void*)& Cyc_Toc__tag_arr_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev; static
unsigned char _temp31[ 14u]="_init_tag_arr"; static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_temp31, _temp31, _temp31 +  14u}; static struct _tuple0 Cyc_Toc__init_tag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={ 1u,& Cyc_Toc__init_tag_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=& Cyc_Toc__init_tag_arr_ev;
static unsigned char _temp33[ 11u]="_untag_arr"; static struct _tagged_arr Cyc_Toc__untag_arr_str={
_temp33, _temp33, _temp33 +  11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__untag_arr_re),
0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=&
Cyc_Toc__untag_arr_ev; static unsigned char _temp35[ 14u]="_get_arr_size";
static struct _tagged_arr Cyc_Toc__get_arr_size_str={ _temp35, _temp35, _temp35
+  14u}; static struct _tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1u,& Cyc_Toc__get_arr_size_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__get_arr_size_ev={ 0,( void*)(( void*)& Cyc_Toc__get_arr_size_re), 0,(
void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev; static unsigned char _temp37[ 17u]="_tagged_arr_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={ _temp37, _temp37,
_temp37 +  17u}; static struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={ 1u,& Cyc_Toc__tagged_arr_plus_pr,( void*)(( void*)
0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_plus_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev;
static unsigned char _temp39[ 25u]="_tagged_arr_inplace_plus"; static struct
_tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={ _temp39, _temp39, _temp39 + 
25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_inplace_plus_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=&
Cyc_Toc__tagged_arr_inplace_plus_ev; static unsigned char _temp41[ 30u]="_tagged_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp41,
_temp41, _temp41 +  30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp43[ 10u]="_cycalloc"; static struct _tagged_arr Cyc_Toc__cycalloc_str={
_temp43, _temp43, _temp43 +  10u}; static struct _tuple0 Cyc_Toc__cycalloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={ 1u,& Cyc_Toc__cycalloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={ 0,( void*)(( void*)& Cyc_Toc__cycalloc_re),
0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__cycalloc_e=&
Cyc_Toc__cycalloc_ev; static unsigned char _temp45[ 11u]="_cyccalloc"; static
struct _tagged_arr Cyc_Toc__cyccalloc_str={ _temp45, _temp45, _temp45 +  11u};
static struct _tuple0 Cyc_Toc__cyccalloc_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1u,& Cyc_Toc__cyccalloc_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__cyccalloc_ev={ 0,( void*)(( void*)& Cyc_Toc__cyccalloc_re), 0,( void*)((
void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev; static unsigned char _temp47[ 17u]="_cycalloc_atomic";
static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={ _temp47, _temp47,
_temp47 +  17u}; static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={ 1u,& Cyc_Toc__cycalloc_atomic_pr,( void*)(( void*)
0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={ 0,( void*)((
void*)& Cyc_Toc__cycalloc_atomic_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static unsigned char _temp49[ 18u]="_cyccalloc_atomic"; static struct
_tagged_arr Cyc_Toc__cyccalloc_atomic_str={ _temp49, _temp49, _temp49 +  18u};
static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1u,& Cyc_Toc__cyccalloc_atomic_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__cyccalloc_atomic_ev={ 0,( void*)(( void*)& Cyc_Toc__cyccalloc_atomic_re),
0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__cyccalloc_atomic_e=&
Cyc_Toc__cyccalloc_atomic_ev; static unsigned char _temp51[ 15u]="_region_malloc";
static struct _tagged_arr Cyc_Toc__region_malloc_str={ _temp51, _temp51, _temp51
+  15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__region_malloc_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={
1u,& Cyc_Toc__region_malloc_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__region_malloc_ev={ 0,( void*)(( void*)& Cyc_Toc__region_malloc_re), 0,(
void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev; static unsigned char _temp53[ 15u]="_region_calloc";
static struct _tagged_arr Cyc_Toc__region_calloc_str={ _temp53, _temp53, _temp53
+  15u}; static struct _tuple0 Cyc_Toc__region_calloc_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__region_calloc_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1u,& Cyc_Toc__region_calloc_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__region_calloc_ev={ 0,( void*)(( void*)& Cyc_Toc__region_calloc_re), 0,(
void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__region_calloc_e=&
Cyc_Toc__region_calloc_ev; static unsigned char _temp55[ 13u]="_check_times";
static struct _tagged_arr Cyc_Toc__check_times_str={ _temp55, _temp55, _temp55 + 
13u}; static struct _tuple0 Cyc_Toc__check_times_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_times_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={
1u,& Cyc_Toc__check_times_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_times_ev={ 0,( void*)(( void*)& Cyc_Toc__check_times_re), 0,(
void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev; static unsigned char _temp57[ 12u]="_new_region";
static struct _tagged_arr Cyc_Toc__new_region_str={ _temp57, _temp57, _temp57 + 
12u}; static struct _tuple0 Cyc_Toc__new_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__new_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re), 0,( void*)((
void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev; static unsigned char _temp59[ 13u]="_push_region";
static struct _tagged_arr Cyc_Toc__push_region_str={ _temp59, _temp59, _temp59 + 
13u}; static struct _tuple0 Cyc_Toc__push_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={
1u,& Cyc_Toc__push_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_region_ev={ 0,( void*)(( void*)& Cyc_Toc__push_region_re), 0,(
void*)(( void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev; static unsigned char _temp61[ 12u]="_pop_region";
static struct _tagged_arr Cyc_Toc__pop_region_str={ _temp61, _temp61, _temp61 + 
12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re), 0,( void*)((
void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev; static struct Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={
10u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0}; static void* Cyc_Toc_tagged_arr_typ=(
void*)& Cyc_Toc_tagged_arr_typ_v; static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={
0, 0, 0}; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; }
; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp64=( void*) e->r;
void* _temp90; unsigned char _temp92; void* _temp94; short _temp96; void*
_temp98; int _temp100; void* _temp102; long long _temp104; void* _temp106;
struct Cyc_Absyn_Exp* _temp108; struct Cyc_List_List* _temp110; struct Cyc_List_List*
_temp112; struct Cyc_List_List* _temp114; struct Cyc_List_List* _temp116; struct
Cyc_List_List* _temp118; _LL66: if(*(( int*) _temp64) ==  Cyc_Absyn_Const_e){
_LL91: _temp90=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp90 >  1u?*(( int*) _temp90) ==  Cyc_Absyn_Char_c: 0){ _LL93:
_temp92=(( struct Cyc_Absyn_Char_c_struct*) _temp90)->f2; goto _LL67;} else{
goto _LL68;}} else{ goto _LL68;} _LL68: if(*(( int*) _temp64) ==  Cyc_Absyn_Const_e){
_LL95: _temp94=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp94 >  1u?*(( int*) _temp94) ==  Cyc_Absyn_Short_c: 0){ _LL97:
_temp96=(( struct Cyc_Absyn_Short_c_struct*) _temp94)->f2; goto _LL69;} else{
goto _LL70;}} else{ goto _LL70;} _LL70: if(*(( int*) _temp64) ==  Cyc_Absyn_Const_e){
_LL99: _temp98=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp98 >  1u?*(( int*) _temp98) ==  Cyc_Absyn_Int_c: 0){ _LL101:
_temp100=(( struct Cyc_Absyn_Int_c_struct*) _temp98)->f2; goto _LL71;} else{
goto _LL72;}} else{ goto _LL72;} _LL72: if(*(( int*) _temp64) ==  Cyc_Absyn_Const_e){
_LL103: _temp102=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp102 >  1u?*(( int*) _temp102) ==  Cyc_Absyn_LongLong_c: 0){
_LL105: _temp104=(( struct Cyc_Absyn_LongLong_c_struct*) _temp102)->f2; goto
_LL73;} else{ goto _LL74;}} else{ goto _LL74;} _LL74: if(*(( int*) _temp64) == 
Cyc_Absyn_Const_e){ _LL107: _temp106=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp64)->f1; if( _temp106 == ( void*) Cyc_Absyn_Null_c){ goto _LL75;} else{
goto _LL76;}} else{ goto _LL76;} _LL76: if(*(( int*) _temp64) ==  Cyc_Absyn_Cast_e){
_LL109: _temp108=(( struct Cyc_Absyn_Cast_e_struct*) _temp64)->f2; goto _LL77;}
else{ goto _LL78;} _LL78: if(*(( int*) _temp64) ==  Cyc_Absyn_Tuple_e){ _LL111:
_temp110=(( struct Cyc_Absyn_Tuple_e_struct*) _temp64)->f1; goto _LL79;} else{
goto _LL80;} _LL80: if(*(( int*) _temp64) ==  Cyc_Absyn_Array_e){ _LL113:
_temp112=(( struct Cyc_Absyn_Array_e_struct*) _temp64)->f1; goto _LL81;} else{
goto _LL82;} _LL82: if(*(( int*) _temp64) ==  Cyc_Absyn_Struct_e){ _LL115:
_temp114=(( struct Cyc_Absyn_Struct_e_struct*) _temp64)->f3; goto _LL83;} else{
goto _LL84;} _LL84: if(*(( int*) _temp64) ==  Cyc_Absyn_CompoundLit_e){ _LL117:
_temp116=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp64)->f2; goto _LL85;}
else{ goto _LL86;} _LL86: if(*(( int*) _temp64) ==  Cyc_Absyn_UnresolvedMem_e){
_LL119: _temp118=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp64)->f2; goto
_LL87;} else{ goto _LL88;} _LL88: goto _LL89; _LL67: return _temp92 == '\000';
_LL69: return _temp96 ==  0; _LL71: return _temp100 ==  0; _LL73: return
_temp104 ==  0; _LL75: return 1; _LL77: return Cyc_Toc_is_zero( _temp108); _LL79:
return(( int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_forall)( Cyc_Toc_is_zero, _temp110); _LL81: _temp114= _temp112; goto
_LL83; _LL83: _temp116= _temp114; goto _LL85; _LL85: _temp118= _temp116; goto
_LL87; _LL87: for( 0; _temp118 !=  0; _temp118= _temp118->tl){ if( ! Cyc_Toc_is_zero((*((
struct _tuple4*) _temp118->hd)).f2)){ return 0;}} return 1; _LL89: return 0;
_LL65:;} static int Cyc_Toc_is_nullable( void* t){ void* _temp120= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp126; struct Cyc_Absyn_Conref* _temp128; _LL122:
if(( unsigned int) _temp120 >  3u?*(( int*) _temp120) ==  Cyc_Absyn_PointerType:
0){ _LL127: _temp126=(( struct Cyc_Absyn_PointerType_struct*) _temp120)->f1;
_LL129: _temp128= _temp126.nullable; goto _LL123;} else{ goto _LL124;} _LL124:
goto _LL125; _LL123: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp128); _LL125:({ void* _temp130[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable", sizeof(
unsigned char), 12u), _tag_arr( _temp130, sizeof( void*), 0u));}); return 0;
_LL121:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x,
struct _tagged_arr tag){ return({ struct _tuple0* _temp131=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp131->f1=(* x).f1; _temp131->f2=({
struct _tagged_arr* _temp132=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp132[ 0]=( struct _tagged_arr) Cyc_Std_strconcat(*(* x).f2,
tag); _temp132;}); _temp131;});} struct _tuple5{ struct _tagged_arr* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct
Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp133= Cyc_Toc_tuple_types; for(
0; _temp133 !=  0; _temp133= _temp133->tl){ struct _tuple5 _temp136; struct Cyc_List_List*
_temp137; struct _tagged_arr* _temp139; struct _tuple5* _temp134=( struct
_tuple5*) _temp133->hd; _temp136=* _temp134; _LL140: _temp139= _temp136.f1; goto
_LL138; _LL138: _temp137= _temp136.f2; goto _LL135; _LL135: { struct Cyc_List_List*
_temp141= tqs0; for( 0; _temp141 !=  0? _temp137 !=  0: 0;( _temp141= _temp141->tl,
_temp137= _temp137->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*) _temp141->hd)).f2,(
void*) _temp137->hd)){ break;}} if( _temp141 ==  0? _temp137 ==  0: 0){ return
_temp139;}}}}{ struct _tagged_arr* x=({ struct _tagged_arr* _temp155=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp155[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp157; _temp157.tag= Cyc_Std_Int_pa;
_temp157.f1=( int)(( unsigned int) Cyc_Toc_tuple_type_counter ++);{ void*
_temp156[ 1u]={& _temp157}; Cyc_Std_aprintf( _tag_arr("_tuple%d", sizeof(
unsigned char), 9u), _tag_arr( _temp156, sizeof( void*), 1u));}}); _temp155;});
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List*
_temp142= 0; struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 !=  0;( ts2=
ts2->tl, i ++)){ _temp142=({ struct Cyc_List_List* _temp143=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp143->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp144=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp144->name= Cyc_Absyn_fieldname( i); _temp144->tq= Cyc_Toc_mt_tq; _temp144->type=(
void*)(( void*) ts2->hd); _temp144->width= 0; _temp144->attributes= 0; _temp144;});
_temp143->tl= _temp142; _temp143;});}} _temp142=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp142);{ struct Cyc_Absyn_Structdecl*
_temp145=({ struct Cyc_Absyn_Structdecl* _temp151=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp151->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp151->name=({ struct Cyc_Core_Opt* _temp153=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp153->v=( void*)({ struct _tuple0*
_temp154=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp154->f1=
Cyc_Absyn_rel_ns_null; _temp154->f2= x; _temp154;}); _temp153;}); _temp151->tvs=
0; _temp151->fields=({ struct Cyc_Core_Opt* _temp152=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp152->v=( void*) _temp142;
_temp152;}); _temp151->attributes= 0; _temp151;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp146=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp146->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp147=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp147[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp148; _temp148.tag= Cyc_Absyn_Struct_d; _temp148.f1=
_temp145; _temp148;}); _temp147;}), 0); _temp146->tl= Cyc_Toc_result_decls;
_temp146;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp149=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp149->hd=( void*)({ struct
_tuple5* _temp150=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp150->f1= x; _temp150->f2= ts; _temp150;}); _temp149->tl= Cyc_Toc_tuple_types;
_temp149;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp158=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp158->f1=( void*) Cyc_Absyn_Loc_n; _temp158->f2=({ struct _tagged_arr*
_temp159=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp159[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp161;
_temp161.tag= Cyc_Std_Int_pa; _temp161.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp160[ 1u]={& _temp161}; Cyc_Std_aprintf( _tag_arr("_temp%d",
sizeof( unsigned char), 8u), _tag_arr( _temp160, sizeof( void*), 1u));}});
_temp159;}); _temp158;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp162=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp162[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp164; _temp164.tag= Cyc_Std_Int_pa; _temp164.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp163[ 1u]={& _temp164}; Cyc_Std_aprintf( _tag_arr("_LL%d",
sizeof( unsigned char), 6u), _tag_arr( _temp163, sizeof( void*), 1u));}});
_temp162;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp167; struct Cyc_Absyn_Tqual
_temp169; struct Cyc_Core_Opt* _temp171; struct _tuple1 _temp165=* a; _LL172:
_temp171= _temp165.f1; goto _LL170; _LL170: _temp169= _temp165.f2; goto _LL168;
_LL168: _temp167= _temp165.f3; goto _LL166; _LL166: return({ struct _tuple1*
_temp173=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp173->f1=
_temp171; _temp173->f2= _temp169; _temp173->f3= Cyc_Toc_typ_to_c( _temp167);
_temp173;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp176; struct Cyc_Absyn_Tqual _temp178; struct _tuple3 _temp174=* x;
_LL179: _temp178= _temp174.f1; goto _LL177; _LL177: _temp176= _temp174.f2; goto
_LL175; _LL175: return({ struct _tuple3* _temp180=( struct _tuple3*) _cycalloc(
sizeof( struct _tuple3)); _temp180->f1= _temp178; _temp180->f2= Cyc_Toc_typ_to_c(
_temp176); _temp180;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp181= t; struct Cyc_Absyn_Exp* _temp189; struct Cyc_Absyn_Tqual _temp191;
void* _temp193; struct Cyc_Core_Opt* _temp195; struct Cyc_Core_Opt _temp197;
void* _temp198; _LL183: if(( unsigned int) _temp181 >  3u?*(( int*) _temp181) == 
Cyc_Absyn_ArrayType: 0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp181)->f1; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_ArrayType_struct*)
_temp181)->f2; goto _LL190; _LL190: _temp189=(( struct Cyc_Absyn_ArrayType_struct*)
_temp181)->f3; goto _LL184;} else{ goto _LL185;} _LL185: if(( unsigned int)
_temp181 >  3u?*(( int*) _temp181) ==  Cyc_Absyn_Evar: 0){ _LL196: _temp195=((
struct Cyc_Absyn_Evar_struct*) _temp181)->f2; if( _temp195 ==  0){ goto _LL187;}
else{ _temp197=* _temp195; _LL199: _temp198=( void*) _temp197.v; goto _LL186;}}
else{ goto _LL187;} _LL187: goto _LL188; _LL184: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp200=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp200[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp201; _temp201.tag= Cyc_Absyn_ArrayType;
_temp201.f1=( void*) Cyc_Toc_typ_to_c_array( _temp193); _temp201.f2= _temp191;
_temp201.f3= _temp189; _temp201;}); _temp200;}); _LL186: return Cyc_Toc_typ_to_c_array(
_temp198); _LL188: return Cyc_Toc_typ_to_c( t); _LL182:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp202=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp202->name= f->name; _temp202->tq= f->tq; _temp202->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp202->width= f->width; _temp202->attributes= f->attributes;
_temp202;});} static void Cyc_Toc_enumfields_to_c( struct Cyc_List_List* fs){
struct Cyc_List_List* _temp203= fs; for( 0; _temp203 !=  0; _temp203= _temp203->tl){
if((( struct Cyc_Absyn_Enumfield*) _temp203->hd)->tag !=  0){ unsigned int i=
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)
_temp203->hd)->tag));(( struct Cyc_Absyn_Enumfield*) _temp203->hd)->tag=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( i, 0);}}} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp204= t; struct Cyc_Core_Opt* _temp262; struct Cyc_Core_Opt*
_temp264; struct Cyc_Core_Opt _temp266; void* _temp267; struct Cyc_Absyn_Tvar*
_temp269; struct Cyc_Absyn_TunionInfo _temp271; void* _temp273; struct Cyc_Absyn_TunionFieldInfo
_temp275; void* _temp277; struct Cyc_Absyn_Tunionfield* _temp279; struct Cyc_Absyn_Tuniondecl*
_temp281; struct Cyc_Absyn_PtrInfo _temp283; struct Cyc_Absyn_Conref* _temp285;
struct Cyc_Absyn_Tqual _temp287; void* _temp289; struct Cyc_Absyn_Exp* _temp291;
struct Cyc_Absyn_Tqual _temp293; void* _temp295; struct Cyc_Absyn_FnInfo
_temp297; struct Cyc_List_List* _temp299; struct Cyc_Absyn_VarargInfo* _temp301;
int _temp303; struct Cyc_List_List* _temp305; void* _temp307; struct Cyc_List_List*
_temp309; struct Cyc_List_List* _temp311; struct Cyc_List_List* _temp313; struct
_tuple0* _temp315; struct _tuple0* _temp317; struct _tuple0* _temp319; struct
Cyc_List_List* _temp321; struct Cyc_Core_Opt* _temp323; struct Cyc_List_List*
_temp325; struct _tuple0* _temp327; void* _temp329; _LL206: if( _temp204 == (
void*) Cyc_Absyn_VoidType){ goto _LL207;} else{ goto _LL208;} _LL208: if((
unsigned int) _temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_Evar: 0){ _LL263:
_temp262=(( struct Cyc_Absyn_Evar_struct*) _temp204)->f2; if( _temp262 ==  0){
goto _LL209;} else{ goto _LL210;}} else{ goto _LL210;} _LL210: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_Evar: 0){ _LL265: _temp264=((
struct Cyc_Absyn_Evar_struct*) _temp204)->f2; if( _temp264 ==  0){ goto _LL212;}
else{ _temp266=* _temp264; _LL268: _temp267=( void*) _temp266.v; goto _LL211;}}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204)
==  Cyc_Absyn_VarType: 0){ _LL270: _temp269=(( struct Cyc_Absyn_VarType_struct*)
_temp204)->f1; goto _LL213;} else{ goto _LL214;} _LL214: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_TunionType: 0){ _LL272:
_temp271=(( struct Cyc_Absyn_TunionType_struct*) _temp204)->f1; _LL274: _temp273=(
void*) _temp271.tunion_info; if(*(( int*) _temp273) ==  Cyc_Absyn_KnownTunion){
goto _LL215;} else{ goto _LL216;}} else{ goto _LL216;} _LL216: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_TunionType: 0){ goto _LL217;}
else{ goto _LL218;} _LL218: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204)
==  Cyc_Absyn_TunionFieldType: 0){ _LL276: _temp275=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp204)->f1; _LL278: _temp277=( void*) _temp275.field_info; if(*(( int*)
_temp277) ==  Cyc_Absyn_KnownTunionfield){ _LL282: _temp281=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp277)->f1; goto _LL280; _LL280: _temp279=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp277)->f2; goto _LL219;} else{ goto _LL220;}} else{ goto _LL220;} _LL220:
if(( unsigned int) _temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL221;} else{ goto _LL222;} _LL222: if(( unsigned int) _temp204 >  3u?*((
int*) _temp204) ==  Cyc_Absyn_PointerType: 0){ _LL284: _temp283=(( struct Cyc_Absyn_PointerType_struct*)
_temp204)->f1; _LL290: _temp289=( void*) _temp283.elt_typ; goto _LL288; _LL288:
_temp287= _temp283.tq; goto _LL286; _LL286: _temp285= _temp283.bounds; goto
_LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp204 >  3u?*(( int*)
_temp204) ==  Cyc_Absyn_IntType: 0){ goto _LL225;} else{ goto _LL226;} _LL226:
if( _temp204 == ( void*) Cyc_Absyn_FloatType){ goto _LL227;} else{ goto _LL228;}
_LL228: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_DoubleType:
0){ goto _LL229;} else{ goto _LL230;} _LL230: if(( unsigned int) _temp204 >  3u?*((
int*) _temp204) ==  Cyc_Absyn_ArrayType: 0){ _LL296: _temp295=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp204)->f1; goto _LL294; _LL294: _temp293=((
struct Cyc_Absyn_ArrayType_struct*) _temp204)->f2; goto _LL292; _LL292: _temp291=((
struct Cyc_Absyn_ArrayType_struct*) _temp204)->f3; goto _LL231;} else{ goto
_LL232;} _LL232: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_FnType:
0){ _LL298: _temp297=(( struct Cyc_Absyn_FnType_struct*) _temp204)->f1; _LL308:
_temp307=( void*) _temp297.ret_typ; goto _LL306; _LL306: _temp305= _temp297.args;
goto _LL304; _LL304: _temp303= _temp297.c_varargs; goto _LL302; _LL302: _temp301=
_temp297.cyc_varargs; goto _LL300; _LL300: _temp299= _temp297.attributes; goto
_LL233;} else{ goto _LL234;} _LL234: if(( unsigned int) _temp204 >  3u?*(( int*)
_temp204) ==  Cyc_Absyn_TupleType: 0){ _LL310: _temp309=(( struct Cyc_Absyn_TupleType_struct*)
_temp204)->f1; goto _LL235;} else{ goto _LL236;} _LL236: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_AnonStructType: 0){ _LL312:
_temp311=(( struct Cyc_Absyn_AnonStructType_struct*) _temp204)->f1; goto _LL237;}
else{ goto _LL238;} _LL238: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204)
==  Cyc_Absyn_AnonUnionType: 0){ _LL314: _temp313=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp204)->f1; goto _LL239;} else{ goto _LL240;} _LL240: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_UnionType: 0){ _LL316: _temp315=((
struct Cyc_Absyn_UnionType_struct*) _temp204)->f1; goto _LL241;} else{ goto
_LL242;} _LL242: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_StructType:
0){ _LL318: _temp317=(( struct Cyc_Absyn_StructType_struct*) _temp204)->f1; goto
_LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp204 >  3u?*(( int*)
_temp204) ==  Cyc_Absyn_EnumType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_EnumType_struct*)
_temp204)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_AnonEnumType: 0){ _LL322:
_temp321=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp204)->f1; goto _LL247;}
else{ goto _LL248;} _LL248: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204)
==  Cyc_Absyn_TypedefType: 0){ _LL328: _temp327=(( struct Cyc_Absyn_TypedefType_struct*)
_temp204)->f1; goto _LL326; _LL326: _temp325=(( struct Cyc_Absyn_TypedefType_struct*)
_temp204)->f2; goto _LL324; _LL324: _temp323=(( struct Cyc_Absyn_TypedefType_struct*)
_temp204)->f3; goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_SizeofType: 0){ goto _LL251;}
else{ goto _LL252;} _LL252: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204)
==  Cyc_Absyn_RgnHandleType: 0){ _LL330: _temp329=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp204)->f1; goto _LL253;} else{ goto _LL254;} _LL254: if( _temp204 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int)
_temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_AccessEff: 0){ goto _LL257;}
else{ goto _LL258;} _LL258: if(( unsigned int) _temp204 >  3u?*(( int*) _temp204)
==  Cyc_Absyn_JoinEff: 0){ goto _LL259;} else{ goto _LL260;} _LL260: if((
unsigned int) _temp204 >  3u?*(( int*) _temp204) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL261;} else{ goto _LL205;} _LL207: return t; _LL209: return Cyc_Absyn_sint_t;
_LL211: return Cyc_Toc_typ_to_c( _temp267); _LL213: if( Cyc_Tcutil_tvar_kind(
_temp269) == ( void*) Cyc_Absyn_AnyKind){ return( void*) Cyc_Absyn_VoidType;}
else{ return Cyc_Absyn_void_star_typ();} _LL215: return Cyc_Absyn_void_star_typ();
_LL217:( int) _throw(({ void* _temp331[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp331, sizeof( void*), 0u));}));
_LL219: if( _temp279->typs ==  0){ if( _temp281->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp279->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL221:( int) _throw(({ void*
_temp332[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp332, sizeof( void*), 0u));}));
_LL223: _temp289= Cyc_Toc_typ_to_c_array( _temp289);{ void* _temp333=( void*)(
Cyc_Absyn_compress_conref( _temp285))->v; void* _temp339; _LL335: if((
unsigned int) _temp333 >  1u?*(( int*) _temp333) ==  Cyc_Absyn_Eq_constr: 0){
_LL340: _temp339=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp333)->f1;
if( _temp339 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL336;} else{ goto _LL337;}}
else{ goto _LL337;} _LL337: goto _LL338; _LL336: return Cyc_Toc_tagged_arr_typ;
_LL338: return Cyc_Absyn_star_typ( _temp289,( void*) Cyc_Absyn_HeapRgn, _temp287);
_LL334:;} _LL225: return t; _LL227: return t; _LL229: return t; _LL231: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp341=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp341[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp342; _temp342.tag= Cyc_Absyn_ArrayType; _temp342.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp295); _temp342.f2= _temp293; _temp342.f3=
_temp291; _temp342;}); _temp341;}); _LL233: { struct Cyc_List_List* _temp343= 0;
for( 0; _temp299 !=  0; _temp299= _temp299->tl){ void* _temp344=( void*)
_temp299->hd; _LL346: if( _temp344 == ( void*) Cyc_Absyn_Noreturn_att){ goto
_LL347;} else{ goto _LL348;} _LL348: if( _temp344 == ( void*) Cyc_Absyn_Const_att){
goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int) _temp344 >  16u?*((
int*) _temp344) ==  Cyc_Absyn_Format_att: 0){ goto _LL351;} else{ goto _LL352;}
_LL352: goto _LL353; _LL347: goto _LL349; _LL349: goto _LL351; _LL351: continue;
_LL353: _temp343=({ struct Cyc_List_List* _temp354=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp354->hd=( void*)(( void*)
_temp299->hd); _temp354->tl= _temp343; _temp354;}); goto _LL345; _LL345:;}{
struct Cyc_List_List* _temp355=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp305); if( _temp301 !=  0){ void* _temp356= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*) _temp301->type,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq)); struct
_tuple1* _temp357=({ struct _tuple1* _temp359=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp359->f1= _temp301->name; _temp359->f2= _temp301->tq;
_temp359->f3= _temp356; _temp359;}); _temp355=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp355,({ struct Cyc_List_List* _temp358=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp358->hd=( void*) _temp357; _temp358->tl= 0;
_temp358;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp360=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp360[ 0]=({ struct Cyc_Absyn_FnType_struct _temp361; _temp361.tag= Cyc_Absyn_FnType;
_temp361.f1=({ struct Cyc_Absyn_FnInfo _temp362; _temp362.tvars= 0; _temp362.effect=
0; _temp362.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp307); _temp362.args=
_temp355; _temp362.c_varargs= _temp303; _temp362.cyc_varargs= 0; _temp362.rgn_po=
0; _temp362.attributes= _temp343; _temp362;}); _temp361;}); _temp360;});}}
_LL235: _temp309=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp309);{
struct _tagged_arr* _temp363= Cyc_Toc_add_tuple_type( _temp309); return Cyc_Absyn_strct(
_temp363);} _LL237: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp364=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp364[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp365; _temp365.tag= Cyc_Absyn_AnonStructType; _temp365.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp311); _temp365;}); _temp364;});
_LL239: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp366=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp366[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp367; _temp367.tag=
Cyc_Absyn_AnonUnionType; _temp367.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp313); _temp367;}); _temp366;}); _LL241: if( _temp315 ==  0){({ void*
_temp368[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp368,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp315)); _LL243: if( _temp317 ==  0){({ void* _temp369[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp369, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp317)); _LL245:
return t; _LL247: Cyc_Toc_enumfields_to_c( _temp321); return t; _LL249: if(
_temp323 ==  0){ return({ void* _temp370[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u), _tag_arr( _temp370, sizeof( void*), 0u));});}
return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp371=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp371[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp372; _temp372.tag= Cyc_Absyn_TypedefType;
_temp372.f1= _temp327; _temp372.f2= 0; _temp372.f3=({ struct Cyc_Core_Opt*
_temp373=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp373->v=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp323->v); _temp373;});
_temp372;}); _temp371;}); _LL251: return Cyc_Absyn_uint_t; _LL253: return Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq); _LL255: return({
void* _temp374[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp374, sizeof( void*), 0u));});
_LL257: return({ void* _temp375[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp375, sizeof( void*), 0u));});
_LL259: return({ void* _temp376[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp376, sizeof( void*), 0u));});
_LL261: return({ void* _temp377[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp377, sizeof( void*), 0u));});
_LL205:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp378= t;
struct Cyc_Absyn_Tqual _temp384; void* _temp386; _LL380: if(( unsigned int)
_temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_ArrayType: 0){ _LL387: _temp386=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp378)->f1; goto _LL385; _LL385:
_temp384=(( struct Cyc_Absyn_ArrayType_struct*) _temp378)->f2; goto _LL381;}
else{ goto _LL382;} _LL382: goto _LL383; _LL381: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp386,( void*) Cyc_Absyn_HeapRgn, _temp384), e, l); _LL383: return Cyc_Absyn_cast_exp(
t, e, l); _LL379:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp388= Cyc_Tcutil_compress(
t); void* _temp430; struct Cyc_Absyn_Structdecl** _temp432; struct Cyc_List_List*
_temp434; struct Cyc_List_List* _temp436; struct Cyc_Absyn_TunionFieldInfo
_temp438; void* _temp440; struct Cyc_Absyn_Tunionfield* _temp442; struct Cyc_Absyn_Tuniondecl*
_temp444; struct Cyc_List_List* _temp446; _LL390: if( _temp388 == ( void*) Cyc_Absyn_VoidType){
goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int) _temp388 >  3u?*((
int*) _temp388) ==  Cyc_Absyn_VarType: 0){ goto _LL393;} else{ goto _LL394;}
_LL394: if(( unsigned int) _temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_IntType:
0){ goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp388 >  3u?*((
int*) _temp388) ==  Cyc_Absyn_EnumType: 0){ goto _LL397;} else{ goto _LL398;}
_LL398: if(( unsigned int) _temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL399;} else{ goto _LL400;} _LL400: if( _temp388 == ( void*) Cyc_Absyn_FloatType){
goto _LL401;} else{ goto _LL402;} _LL402: if(( unsigned int) _temp388 >  3u?*((
int*) _temp388) ==  Cyc_Absyn_DoubleType: 0){ goto _LL403;} else{ goto _LL404;}
_LL404: if(( unsigned int) _temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_FnType:
0){ goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int) _temp388 >  3u?*((
int*) _temp388) ==  Cyc_Absyn_SizeofType: 0){ goto _LL407;} else{ goto _LL408;}
_LL408: if(( unsigned int) _temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_ArrayType:
0){ _LL431: _temp430=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp388)->f1;
goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int) _temp388 >  3u?*((
int*) _temp388) ==  Cyc_Absyn_StructType: 0){ _LL433: _temp432=(( struct Cyc_Absyn_StructType_struct*)
_temp388)->f3; goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int)
_temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_AnonStructType: 0){ _LL435:
_temp434=(( struct Cyc_Absyn_AnonStructType_struct*) _temp388)->f1; goto _LL413;}
else{ goto _LL414;} _LL414: if(( unsigned int) _temp388 >  3u?*(( int*) _temp388)
==  Cyc_Absyn_AnonUnionType: 0){ _LL437: _temp436=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp388)->f1; goto _LL415;} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_TunionFieldType: 0){ _LL439:
_temp438=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp388)->f1; _LL441:
_temp440=( void*) _temp438.field_info; if(*(( int*) _temp440) ==  Cyc_Absyn_KnownTunionfield){
_LL445: _temp444=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp440)->f1;
goto _LL443; _LL443: _temp442=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp440)->f2; goto _LL417;} else{ goto _LL418;}} else{ goto _LL418;} _LL418:
if(( unsigned int) _temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_TupleType: 0){
_LL447: _temp446=(( struct Cyc_Absyn_TupleType_struct*) _temp388)->f1; goto
_LL419;} else{ goto _LL420;} _LL420: if(( unsigned int) _temp388 >  3u?*(( int*)
_temp388) ==  Cyc_Absyn_TunionType: 0){ goto _LL421;} else{ goto _LL422;} _LL422:
if(( unsigned int) _temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_UnionType: 0){
goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int) _temp388 >  3u?*((
int*) _temp388) ==  Cyc_Absyn_PointerType: 0){ goto _LL425;} else{ goto _LL426;}
_LL426: if(( unsigned int) _temp388 >  3u?*(( int*) _temp388) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL427;} else{ goto _LL428;} _LL428: goto _LL429; _LL391: return 1;
_LL393: return 0; _LL395: return 1; _LL397: return 1; _LL399: return 1; _LL401:
return 1; _LL403: return 1; _LL405: return 1; _LL407: return 1; _LL409: return
Cyc_Toc_atomic_typ( _temp430); _LL411: if( _temp432 ==  0? 1:(* _temp432)->fields
==  0){ return 0;}{ struct Cyc_List_List* _temp448=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp432)->fields))->v; for( 0; _temp448 != 
0; _temp448= _temp448->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)
_temp448->hd)->type)){ return 0;}}} return 1; _LL413: _temp436= _temp434; goto
_LL415; _LL415: for( 0; _temp436 !=  0; _temp436= _temp436->tl){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*) _temp436->hd)->type)){ return 0;}}
return 1; _LL417: _temp446= _temp442->typs; goto _LL419; _LL419: for( 0;
_temp446 !=  0; _temp446= _temp446->tl){ if( ! Cyc_Toc_atomic_typ((*(( struct
_tuple3*) _temp446->hd)).f2)){ return 0;}} return 1; _LL421: return 0; _LL423:
return 0; _LL425: return 0; _LL427: return 0; _LL429:({ struct Cyc_Std_String_pa_struct
_temp450; _temp450.tag= Cyc_Std_String_pa; _temp450.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp449[ 1u]={& _temp450};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp449, sizeof( void*), 1u));}});
return 0; _LL389:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp451=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp457; void* _temp459;
_LL453: if(( unsigned int) _temp451 >  3u?*(( int*) _temp451) ==  Cyc_Absyn_PointerType:
0){ _LL458: _temp457=(( struct Cyc_Absyn_PointerType_struct*) _temp451)->f1;
_LL460: _temp459=( void*) _temp457.elt_typ; goto _LL454;} else{ goto _LL455;}
_LL455: goto _LL456; _LL454: { void* _temp461= Cyc_Tcutil_compress( _temp459);
_LL463: if( _temp461 == ( void*) Cyc_Absyn_VoidType){ goto _LL464;} else{ goto
_LL465;} _LL465: goto _LL466; _LL464: return 1; _LL466: return 0; _LL462:;}
_LL456: return 0; _LL452:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp467= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp479; struct Cyc_Absyn_Structdecl* _temp481; struct Cyc_List_List* _temp482;
_LL469: if(( unsigned int) _temp467 >  3u?*(( int*) _temp467) ==  Cyc_Absyn_StructType:
0){ _LL480: _temp479=(( struct Cyc_Absyn_StructType_struct*) _temp467)->f3; if(
_temp479 ==  0){ goto _LL471;} else{ _temp481=* _temp479; goto _LL470;}} else{
goto _LL471;} _LL471: if(( unsigned int) _temp467 >  3u?*(( int*) _temp467) == 
Cyc_Absyn_AnonStructType: 0){ _LL483: _temp482=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp467)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if(( unsigned int)
_temp467 >  3u?*(( int*) _temp467) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL474;}
else{ goto _LL475;} _LL475: if(( unsigned int) _temp467 >  3u?*(( int*) _temp467)
==  Cyc_Absyn_UnionType: 0){ goto _LL476;} else{ goto _LL477;} _LL477: goto
_LL478; _LL470: if( _temp481->fields ==  0){( int) _throw(({ void* _temp484[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp484, sizeof( void*), 0u));}));} _temp482=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp481->fields))->v;
goto _LL472; _LL472: { struct Cyc_Absyn_Structfield* _temp485= Cyc_Absyn_lookup_field(
_temp482, f); if( _temp485 ==  0){({ struct Cyc_Std_String_pa_struct _temp487;
_temp487.tag= Cyc_Std_String_pa; _temp487.f1=( struct _tagged_arr)* f;{ void*
_temp486[ 1u]={& _temp487};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_field: bad field %s", sizeof(
unsigned char), 28u), _tag_arr( _temp486, sizeof( void*), 1u));}});} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp485))->type);} _LL474:
return 0; _LL476: return 0; _LL478:( int) _throw(({ struct Cyc_Std_String_pa_struct
_temp489; _temp489.tag= Cyc_Std_String_pa; _temp489.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp488[ 1u]={& _temp489}; Cyc_Toc_toc_impos( _tag_arr("is_poly_field: bad type %s",
sizeof( unsigned char), 27u), _tag_arr( _temp488, sizeof( void*), 1u));}}));
_LL468:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void*
_temp490=( void*) e->r; struct _tagged_arr* _temp498; struct Cyc_Absyn_Exp*
_temp500; struct _tagged_arr* _temp502; struct Cyc_Absyn_Exp* _temp504; _LL492:
if(*(( int*) _temp490) ==  Cyc_Absyn_StructMember_e){ _LL501: _temp500=(( struct
Cyc_Absyn_StructMember_e_struct*) _temp490)->f1; goto _LL499; _LL499: _temp498=((
struct Cyc_Absyn_StructMember_e_struct*) _temp490)->f2; goto _LL493;} else{ goto
_LL494;} _LL494: if(*(( int*) _temp490) ==  Cyc_Absyn_StructArrow_e){ _LL505:
_temp504=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp490)->f1; goto _LL503;
_LL503: _temp502=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp490)->f2; goto
_LL495;} else{ goto _LL496;} _LL496: goto _LL497; _LL493: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp500->topt))->v, _temp498);
_LL495: { void* _temp506= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp504->topt))->v); struct Cyc_Absyn_PtrInfo _temp512; void*
_temp514; _LL508: if(( unsigned int) _temp506 >  3u?*(( int*) _temp506) ==  Cyc_Absyn_PointerType:
0){ _LL513: _temp512=(( struct Cyc_Absyn_PointerType_struct*) _temp506)->f1;
_LL515: _temp514=( void*) _temp512.elt_typ; goto _LL509;} else{ goto _LL510;}
_LL510: goto _LL511; _LL509: return Cyc_Toc_is_poly_field( _temp514, _temp502);
_LL511:({ struct Cyc_Std_String_pa_struct _temp517; _temp517.tag= Cyc_Std_String_pa;
_temp517.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp504->topt))->v);{ void* _temp516[ 1u]={& _temp517};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s",
sizeof( unsigned char), 30u), _tag_arr( _temp516, sizeof( void*), 1u));}});
return 0; _LL507:;} _LL497: return 0; _LL491:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_e,({
struct Cyc_List_List* _temp518=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp518->hd=( void*) s; _temp518->tl= 0; _temp518;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_atomic_e,({ struct Cyc_List_List*
_temp519=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp519->hd=( void*) s; _temp519->tl= 0; _temp519;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp520[ 2u]; _temp520[ 1u]= s; _temp520[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp520, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_calloc_exp( void* elt_type,
struct Cyc_Absyn_Exp* s, struct Cyc_Absyn_Exp* n){ if( Cyc_Toc_atomic_typ(
elt_type)){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cyccalloc_atomic_e,({ struct
Cyc_Absyn_Exp* _temp521[ 2u]; _temp521[ 1u]= n; _temp521[ 0u]= s;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp521, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} else{ return Cyc_Absyn_fncall_exp( Cyc_Toc__cyccalloc_e,({ struct
Cyc_Absyn_Exp* _temp522[ 2u]; _temp522[ 1u]= n; _temp522[ 0u]= s;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp522, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_rcalloc_exp( struct Cyc_Absyn_Exp*
rgn, struct Cyc_Absyn_Exp* s, struct Cyc_Absyn_Exp* n){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({ struct Cyc_Absyn_Exp* _temp523[ 3u]; _temp523[ 2u]=
n; _temp523[ 1u]= s; _temp523[ 0u]= rgn;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp523, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp524=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp524->hd=( void*) e; _temp524->tl= 0; _temp524;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp525=( void*) e->r; void* _temp531; _LL527: if(*((
int*) _temp525) ==  Cyc_Absyn_Const_e){ _LL532: _temp531=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp525)->f1; if(( unsigned int) _temp531 >  1u?*(( int*) _temp531) ==  Cyc_Absyn_String_c:
0){ goto _LL528;} else{ goto _LL529;}} else{ goto _LL529;} _LL529: goto _LL530;
_LL528: is_string= 1; goto _LL526; _LL530: goto _LL526; _LL526:;}{ struct Cyc_Absyn_Exp*
xexp; struct Cyc_Absyn_Exp* xplussz; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp536=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp536[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp537; _temp537.tag= Cyc_Absyn_ArrayType; _temp537.f1=(
void*) Cyc_Absyn_uchar_t; _temp537.f2= Cyc_Toc_mt_tq; _temp537.f3=( struct Cyc_Absyn_Exp*)
sz; _temp537;}); _temp536;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp533=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp533->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp534=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp534[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp535; _temp535.tag= Cyc_Absyn_Var_d;
_temp535.f1= vd; _temp535;}); _temp534;}), 0); _temp533->tl= Cyc_Toc_result_decls;
_temp533;}); xexp= Cyc_Absyn_var_exp( x, 0); xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0);} else{ xexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), e, 0);
xplussz= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_add_exp( e, sz,
0), 0);}{ struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp538[ 3u]; _temp538[ 2u]=({ struct _tuple4* _temp541=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp541->f1= 0; _temp541->f2=
xplussz; _temp541;}); _temp538[ 1u]=({ struct _tuple4* _temp540=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp540->f1= 0; _temp540->f2= xexp;
_temp540;}); _temp538[ 0u]=({ struct _tuple4* _temp539=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp539->f1= 0; _temp539->f2= xexp;
_temp539;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp538, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp542=( struct Cyc_Toc_Env*) _cycalloc( sizeof( struct
Cyc_Toc_Env)); _temp542->break_lab= 0; _temp542->continue_lab= 0; _temp542->fallthru_info=
0; _temp542->varmap=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp542->toplevel= 1;
_temp542;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp543=( struct Cyc_Toc_Env*) _cycalloc( sizeof(
struct Cyc_Toc_Env)); _temp543->break_lab= e->break_lab; _temp543->continue_lab=
e->continue_lab; _temp543->fallthru_info= e->fallthru_info; _temp543->varmap= e->varmap;
_temp543->toplevel= e->toplevel; _temp543;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp544=(* x).f1; _LL546:
if(( unsigned int) _temp544 >  1u?*(( int*) _temp544) ==  Cyc_Absyn_Rel_n: 0){
goto _LL547;} else{ goto _LL548;} _LL548: goto _LL549; _LL547:( int) _throw(({
struct Cyc_Std_String_pa_struct _temp551; _temp551.tag= Cyc_Std_String_pa;
_temp551.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp550[
1u]={& _temp551}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp550, sizeof( void*), 1u));}}));
_LL549: goto _LL545; _LL545:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
k, struct Cyc_Absyn_Exp* v)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict*
f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp552= 0; for( 0; fallthru_binders !=  0; fallthru_binders= fallthru_binders->tl){
_temp552=({ struct Cyc_List_List* _temp553=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp553->hd=( void*)(( struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name; _temp553->tl= _temp552; _temp553;});} _temp552=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp552);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp554=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp554->v=( void*) break_l; _temp554;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp555=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp555->v=( void*)({ struct _tuple6* _temp556=( struct _tuple6*) _cycalloc(
sizeof( struct _tuple6)); _temp556->f1= fallthru_l; _temp556->f2= _temp552;
_temp556->f3= next_case_env->varmap; _temp556;}); _temp555;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp557=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp557->v=( void*) break_l; _temp557;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp558=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp558->v=( void*)({
struct _tuple6* _temp559=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp559->f1= next_l; _temp559->f2= 0; _temp559->f3=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp559;}); _temp558;}); return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s); static int Cyc_Toc_need_null_check( struct Cyc_Absyn_Exp*
e){ void* _temp560=( void*) e->annot; _LL562: if( _temp560 ==  Cyc_CfFlowInfo_UnknownZ){
goto _LL563;} else{ goto _LL564;} _LL564: if( _temp560 ==  Cyc_CfFlowInfo_NotZero){
goto _LL565;} else{ goto _LL566;} _LL566: if( _temp560 ==  Cyc_CfFlowInfo_IsZero){
goto _LL567;} else{ goto _LL568;} _LL568: if( _temp560 ==  Cyc_Absyn_EmptyAnnot){
goto _LL569;} else{ goto _LL570;} _LL570: goto _LL571; _LL563: return Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL565: return 0;
_LL567:({ void* _temp572[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("dereference of NULL pointer",
sizeof( unsigned char), 28u), _tag_arr( _temp572, sizeof( void*), 0u));});
return 0; _LL569:({ void* _temp573[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("!", sizeof( unsigned char), 2u), _tag_arr( _temp573, sizeof( void*), 0u));});({
void* _temp574[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("unreachable dereference",
sizeof( unsigned char), 24u), _tag_arr( _temp574, sizeof( void*), 0u));});
return 0; _LL571:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp575=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp575[ 0]=({ struct Cyc_Core_Impossible_struct _temp576; _temp576.tag= Cyc_Core_Impossible;
_temp576.f1= _tag_arr("need_null_check", sizeof( unsigned char), 16u); _temp576;});
_temp575;})); _LL561:;} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){
if( e->topt ==  0){({ void* _temp577[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp577, sizeof( void*), 0u));});}
return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp578[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp578, sizeof( void*), 0u));});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp579=( struct _tuple3*) _cycalloc( sizeof(
struct _tuple3)); _temp579->f1= Cyc_Toc_mt_tq; _temp579->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp579;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp580=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp580->f1= 0; _temp580->f2= e; _temp580;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp581= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp581): Cyc_Toc_malloc_ptr( _temp581), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp581), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp582=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp582 !=  0; _temp582=
_temp582->tl){ struct _tuple4 _temp585; struct Cyc_Absyn_Exp* _temp586; struct
Cyc_List_List* _temp588; struct _tuple4* _temp583=( struct _tuple4*) _temp582->hd;
_temp585=* _temp583; _LL589: _temp588= _temp585.f1; goto _LL587; _LL587:
_temp586= _temp585.f2; goto _LL584; _LL584: { struct Cyc_Absyn_Exp* e_index; if(
_temp588 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(
_temp588->tl !=  0){({ void* _temp590[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp590, sizeof( void*), 0u));});}{
void* _temp591=( void*) _temp588->hd; void* _temp592= _temp591; struct Cyc_Absyn_Exp*
_temp598; _LL594: if(*(( int*) _temp592) ==  Cyc_Absyn_ArrayElement){ _LL599:
_temp598=(( struct Cyc_Absyn_ArrayElement_struct*) _temp592)->f1; goto _LL595;}
else{ goto _LL596;} _LL596: if(*(( int*) _temp592) ==  Cyc_Absyn_FieldName){
goto _LL597;} else{ goto _LL593;} _LL595: Cyc_Toc_exp_to_c( nv, _temp598);
e_index= _temp598; goto _LL593; _LL597: e_index=({ void* _temp600[ 0u]={};((
struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("field name designators in array", sizeof( unsigned char), 32u),
_tag_arr( _temp600, sizeof( void*), 0u));}); goto _LL593; _LL593:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp601=( void*)
_temp586->r; struct Cyc_List_List* _temp611; struct Cyc_Absyn_Exp* _temp613;
struct Cyc_Absyn_Exp* _temp615; struct Cyc_Absyn_Vardecl* _temp617; struct Cyc_List_List*
_temp619; void* _temp621; _LL603: if(*(( int*) _temp601) ==  Cyc_Absyn_Array_e){
_LL612: _temp611=(( struct Cyc_Absyn_Array_e_struct*) _temp601)->f1; goto _LL604;}
else{ goto _LL605;} _LL605: if(*(( int*) _temp601) ==  Cyc_Absyn_Comprehension_e){
_LL618: _temp617=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp601)->f1;
goto _LL616; _LL616: _temp615=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp601)->f2; goto _LL614; _LL614: _temp613=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp601)->f3; goto _LL606;} else{ goto _LL607;} _LL607: if(*(( int*) _temp601)
==  Cyc_Absyn_AnonStruct_e){ _LL622: _temp621=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp601)->f1; goto _LL620; _LL620: _temp619=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp601)->f2; goto _LL608;} else{ goto _LL609;} _LL609: goto _LL610; _LL604: s=
Cyc_Toc_init_array( nv, lval, _temp611, s); goto _LL602; _LL606: s= Cyc_Toc_init_comprehension(
nv, lval, _temp617, _temp615, _temp613, s, 0); goto _LL602; _LL608: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp621, _temp619, s); goto _LL602; _LL610: Cyc_Toc_exp_to_c( nv,
_temp586); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp586, 0), s, 0); goto _LL602; _LL602:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp623= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp623, Cyc_Absyn_varb_exp( _temp623,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp646=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp646[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp647; _temp647.tag= Cyc_Absyn_Local_b; _temp647.f1= vd; _temp647;});
_temp646;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp623, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp623, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp623, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp623, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp624=( void*) e2->r; struct Cyc_List_List* _temp634; struct Cyc_Absyn_Exp*
_temp636; struct Cyc_Absyn_Exp* _temp638; struct Cyc_Absyn_Vardecl* _temp640;
struct Cyc_List_List* _temp642; void* _temp644; _LL626: if(*(( int*) _temp624)
==  Cyc_Absyn_Array_e){ _LL635: _temp634=(( struct Cyc_Absyn_Array_e_struct*)
_temp624)->f1; goto _LL627;} else{ goto _LL628;} _LL628: if(*(( int*) _temp624)
==  Cyc_Absyn_Comprehension_e){ _LL641: _temp640=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp624)->f1; goto _LL639; _LL639: _temp638=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp624)->f2; goto _LL637; _LL637: _temp636=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp624)->f3; goto _LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp624)
==  Cyc_Absyn_AnonStruct_e){ _LL645: _temp644=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp624)->f1; goto _LL643; _LL643: _temp642=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp624)->f2; goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL627:
body= Cyc_Toc_init_array( nv2, lval, _temp634, Cyc_Absyn_skip_stmt( 0)); goto
_LL625; _LL629: body= Cyc_Toc_init_comprehension( nv2, lval, _temp640, _temp638,
_temp636, Cyc_Absyn_skip_stmt( 0), 0); goto _LL625; _LL631: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp644, _temp642, Cyc_Absyn_skip_stmt( 0)); goto _LL625; _LL633: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL625; _LL625:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp623, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp648=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp648 !=  0; _temp648= _temp648->tl){
struct _tuple4 _temp651; struct Cyc_Absyn_Exp* _temp652; struct Cyc_List_List*
_temp654; struct _tuple4* _temp649=( struct _tuple4*) _temp648->hd; _temp651=*
_temp649; _LL655: _temp654= _temp651.f1; goto _LL653; _LL653: _temp652= _temp651.f2;
goto _LL650; _LL650: if( _temp654 ==  0){({ void* _temp656[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp656, sizeof( void*), 0u));});} if(
_temp654->tl !=  0){({ void* _temp657[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u), _tag_arr( _temp657, sizeof( void*), 0u));});}{
void* _temp658=( void*) _temp654->hd; struct _tagged_arr* _temp664; _LL660: if(*((
int*) _temp658) ==  Cyc_Absyn_FieldName){ _LL665: _temp664=(( struct Cyc_Absyn_FieldName_struct*)
_temp658)->f1; goto _LL661;} else{ goto _LL662;} _LL662: goto _LL663; _LL661: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp664, 0);{ void*
_temp666=( void*) _temp652->r; struct Cyc_List_List* _temp676; struct Cyc_Absyn_Exp*
_temp678; struct Cyc_Absyn_Exp* _temp680; struct Cyc_Absyn_Vardecl* _temp682;
struct Cyc_List_List* _temp684; void* _temp686; _LL668: if(*(( int*) _temp666)
==  Cyc_Absyn_Array_e){ _LL677: _temp676=(( struct Cyc_Absyn_Array_e_struct*)
_temp666)->f1; goto _LL669;} else{ goto _LL670;} _LL670: if(*(( int*) _temp666)
==  Cyc_Absyn_Comprehension_e){ _LL683: _temp682=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp666)->f1; goto _LL681; _LL681: _temp680=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp666)->f2; goto _LL679; _LL679: _temp678=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp666)->f3; goto _LL671;} else{ goto _LL672;} _LL672: if(*(( int*) _temp666)
==  Cyc_Absyn_AnonStruct_e){ _LL687: _temp686=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp666)->f1; goto _LL685; _LL685: _temp684=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp666)->f2; goto _LL673;} else{ goto _LL674;} _LL674: goto _LL675; _LL669: s=
Cyc_Toc_init_array( nv, lval, _temp676, s); goto _LL667; _LL671: s= Cyc_Toc_init_comprehension(
nv, lval, _temp682, _temp680, _temp678, s, 0); goto _LL667; _LL673: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp686, _temp684, s); goto _LL667; _LL675: Cyc_Toc_exp_to_c( nv,
_temp652); if( Cyc_Toc_is_poly_field( struct_type, _temp664)){ _temp652= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp652, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp652, 0), 0), s, 0); goto _LL667; _LL667:;} goto
_LL659;} _LL663:( int) _throw(({ void* _temp688[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp688, sizeof( void*), 0u));})); _LL659:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp689= _new_region("r");
struct _RegionHandle* r=& _temp689; _push_region( r);{ struct Cyc_List_List*
_temp690=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp691= Cyc_Toc_add_tuple_type( _temp690); struct
_tuple0* _temp692= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp693= Cyc_Absyn_var_exp(
_temp692, 0); struct Cyc_Absyn_Stmt* _temp694= Cyc_Absyn_exp_stmt( _temp693, 0);
struct Cyc_Absyn_Exp*(* _temp695)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp696=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp696); for( 0; _temp696
!=  0;( _temp696= _temp696->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
_temp696->hd; struct Cyc_Absyn_Exp* lval= _temp695( _temp693, Cyc_Absyn_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp697=( void*) e->r; struct Cyc_List_List*
_temp705; struct Cyc_Absyn_Exp* _temp707; struct Cyc_Absyn_Exp* _temp709; struct
Cyc_Absyn_Vardecl* _temp711; _LL699: if(*(( int*) _temp697) ==  Cyc_Absyn_Array_e){
_LL706: _temp705=(( struct Cyc_Absyn_Array_e_struct*) _temp697)->f1; goto _LL700;}
else{ goto _LL701;} _LL701: if(*(( int*) _temp697) ==  Cyc_Absyn_Comprehension_e){
_LL712: _temp711=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp697)->f1;
goto _LL710; _LL710: _temp709=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp697)->f2; goto _LL708; _LL708: _temp707=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp697)->f3; goto _LL702;} else{ goto _LL703;} _LL703: goto _LL704; _LL700:
_temp694= Cyc_Toc_init_array( nv, lval, _temp705, _temp694); goto _LL698; _LL702:
_temp694= Cyc_Toc_init_comprehension( nv, lval, _temp711, _temp709, _temp707,
_temp694, 0); goto _LL698; _LL704: Cyc_Toc_exp_to_c( nv, e); _temp694= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp695( _temp693, Cyc_Absyn_fieldname(
i), 0), e, 0), 0), _temp694, 0); goto _LL698; _LL698:;}}}{ struct Cyc_Absyn_Exp*
_temp713= Cyc_Toc_make_struct( nv, _temp692, Cyc_Absyn_strct( _temp691),
_temp694, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp713;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp714= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp715= Cyc_Absyn_var_exp( _temp714, 0); struct Cyc_Absyn_Stmt*
_temp716= Cyc_Absyn_exp_stmt( _temp715, 0); struct Cyc_Absyn_Exp*(* _temp717)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp718= _new_region("r"); struct _RegionHandle* r=&
_temp718; _push_region( r);{ struct Cyc_List_List* _temp719=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp719 !=  0; _temp719= _temp719->tl){ struct _tuple4 _temp722; struct Cyc_Absyn_Exp*
_temp723; struct Cyc_List_List* _temp725; struct _tuple4* _temp720=( struct
_tuple4*) _temp719->hd; _temp722=* _temp720; _LL726: _temp725= _temp722.f1; goto
_LL724; _LL724: _temp723= _temp722.f2; goto _LL721; _LL721: is_atomic= is_atomic?
Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp723->topt))->v):
0; if( _temp725 ==  0){({ void* _temp727[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp727, sizeof( void*), 0u));});} if(
_temp725->tl !=  0){ struct _tuple0* _temp728= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp729= Cyc_Absyn_var_exp( _temp728, 0); for( 0; _temp725 !=  0; _temp725=
_temp725->tl){ void* _temp730=( void*) _temp725->hd; struct _tagged_arr*
_temp736; _LL732: if(*(( int*) _temp730) ==  Cyc_Absyn_FieldName){ _LL737:
_temp736=(( struct Cyc_Absyn_FieldName_struct*) _temp730)->f1; goto _LL733;}
else{ goto _LL734;} _LL734: goto _LL735; _LL733: if( Cyc_Toc_is_poly_field(
struct_type, _temp736)){ _temp729= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp729, 0);} _temp716= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp717( _temp715, _temp736, 0), _temp729, 0), 0), _temp716, 0); goto _LL731;
_LL735:( int) _throw(({ void* _temp738[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u), _tag_arr( _temp738, sizeof( void*), 0u));}));
_LL731:;} Cyc_Toc_exp_to_c( nv, _temp723); _temp716= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp729, _temp723, 0), 0), _temp716, 0);} else{ void*
_temp739=( void*) _temp725->hd; struct _tagged_arr* _temp745; _LL741: if(*(( int*)
_temp739) ==  Cyc_Absyn_FieldName){ _LL746: _temp745=(( struct Cyc_Absyn_FieldName_struct*)
_temp739)->f1; goto _LL742;} else{ goto _LL743;} _LL743: goto _LL744; _LL742: {
struct Cyc_Absyn_Exp* lval= _temp717( _temp715, _temp745, 0);{ void* _temp747=(
void*) _temp723->r; struct Cyc_List_List* _temp757; struct Cyc_Absyn_Exp*
_temp759; struct Cyc_Absyn_Exp* _temp761; struct Cyc_Absyn_Vardecl* _temp763;
struct Cyc_List_List* _temp765; void* _temp767; _LL749: if(*(( int*) _temp747)
==  Cyc_Absyn_Array_e){ _LL758: _temp757=(( struct Cyc_Absyn_Array_e_struct*)
_temp747)->f1; goto _LL750;} else{ goto _LL751;} _LL751: if(*(( int*) _temp747)
==  Cyc_Absyn_Comprehension_e){ _LL764: _temp763=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp747)->f1; goto _LL762; _LL762: _temp761=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp747)->f2; goto _LL760; _LL760: _temp759=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp747)->f3; goto _LL752;} else{ goto _LL753;} _LL753: if(*(( int*) _temp747)
==  Cyc_Absyn_AnonStruct_e){ _LL768: _temp767=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp747)->f1; goto _LL766; _LL766: _temp765=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp747)->f2; goto _LL754;} else{ goto _LL755;} _LL755: goto _LL756; _LL750:
_temp716= Cyc_Toc_init_array( nv, lval, _temp757, _temp716); goto _LL748; _LL752:
_temp716= Cyc_Toc_init_comprehension( nv, lval, _temp763, _temp761, _temp759,
_temp716, 0); goto _LL748; _LL754: _temp716= Cyc_Toc_init_anon_struct( nv, lval,
_temp767, _temp765, _temp716); goto _LL748; _LL756: Cyc_Toc_exp_to_c( nv,
_temp723); if( Cyc_Toc_is_poly_field( struct_type, _temp745)){ _temp723= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp723, 0);} _temp716= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp723, 0), 0), _temp716, 0); goto _LL748; _LL748:;}
goto _LL740;} _LL744:( int) _throw(({ void* _temp769[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp769, sizeof( void*), 0u));})); _LL740:;}}}{ struct Cyc_Absyn_Exp* _temp770=
Cyc_Toc_make_struct( nv, _temp714, Cyc_Absyn_strctq( tdn), _temp716, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp770;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp771=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp771[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp772; _temp772.tag= Cyc_Absyn_Increment_e;
_temp772.f1= e1; _temp772.f2=( void*) incr; _temp772;}); _temp771;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp773=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp783; struct Cyc_Absyn_Exp*
_temp785; void* _temp787; struct _tagged_arr* _temp789; struct Cyc_Absyn_Exp*
_temp791; _LL775: if(*(( int*) _temp773) ==  Cyc_Absyn_StmtExp_e){ _LL784:
_temp783=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp773)->f1; goto _LL776;}
else{ goto _LL777;} _LL777: if(*(( int*) _temp773) ==  Cyc_Absyn_Cast_e){ _LL788:
_temp787=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp773)->f1; goto _LL786;
_LL786: _temp785=(( struct Cyc_Absyn_Cast_e_struct*) _temp773)->f2; goto _LL778;}
else{ goto _LL779;} _LL779: if(*(( int*) _temp773) ==  Cyc_Absyn_StructMember_e){
_LL792: _temp791=(( struct Cyc_Absyn_StructMember_e_struct*) _temp773)->f1; goto
_LL790; _LL790: _temp789=(( struct Cyc_Absyn_StructMember_e_struct*) _temp773)->f2;
goto _LL780;} else{ goto _LL781;} _LL781: goto _LL782; _LL776: Cyc_Toc_lvalue_assign_stmt(
_temp783, fs, f, f_env); goto _LL774; _LL778: Cyc_Toc_lvalue_assign( _temp785,
fs, f, f_env); goto _LL774; _LL780:( void*)( e1->r=( void*)(( void*) _temp791->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp793=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp793->hd=( void*) _temp789; _temp793->tl= fs; _temp793;}), f, f_env); goto
_LL774; _LL782: { struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0;
fs !=  0; fs= fs->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*) fs->hd, e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f(
e1_copy, f_env))->r)); goto _LL774;} _LL774:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp794=( void*) s->r;
struct Cyc_Absyn_Exp* _temp804; struct Cyc_Absyn_Stmt* _temp806; struct Cyc_Absyn_Decl*
_temp808; struct Cyc_Absyn_Stmt* _temp810; _LL796: if(( unsigned int) _temp794 > 
1u?*(( int*) _temp794) ==  Cyc_Absyn_Exp_s: 0){ _LL805: _temp804=(( struct Cyc_Absyn_Exp_s_struct*)
_temp794)->f1; goto _LL797;} else{ goto _LL798;} _LL798: if(( unsigned int)
_temp794 >  1u?*(( int*) _temp794) ==  Cyc_Absyn_Decl_s: 0){ _LL809: _temp808=((
struct Cyc_Absyn_Decl_s_struct*) _temp794)->f1; goto _LL807; _LL807: _temp806=((
struct Cyc_Absyn_Decl_s_struct*) _temp794)->f2; goto _LL799;} else{ goto _LL800;}
_LL800: if(( unsigned int) _temp794 >  1u?*(( int*) _temp794) ==  Cyc_Absyn_Seq_s:
0){ _LL811: _temp810=(( struct Cyc_Absyn_Seq_s_struct*) _temp794)->f2; goto
_LL801;} else{ goto _LL802;} _LL802: goto _LL803; _LL797: Cyc_Toc_lvalue_assign(
_temp804, fs, f, f_env); goto _LL795; _LL799: Cyc_Toc_lvalue_assign_stmt(
_temp806, fs, f, f_env); goto _LL795; _LL801: Cyc_Toc_lvalue_assign_stmt(
_temp810, fs, f, f_env); goto _LL795; _LL803:({ struct Cyc_Std_String_pa_struct
_temp813; _temp813.tag= Cyc_Std_String_pa; _temp813.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp812[ 1u]={& _temp813};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s", sizeof(
unsigned char), 23u), _tag_arr( _temp812, sizeof( void*), 1u));}}); goto _LL795;
_LL795:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2,
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x ==  0){ return 0;} result=({ struct
Cyc_List_List* _temp814=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp814->hd=( void*) f(( void*) x->hd, env); _temp814->tl=
0; _temp814;}); prev= result; for( x= x->tl; x !=  0; x= x->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp815=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp815->hd=( void*) f((
void*) x->hd, env); _temp815->tl= 0; _temp815;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp816=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp816->f1= 0; _temp816->f2= e; _temp816;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp817=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp823; _LL819: if((
unsigned int) _temp817 >  3u?*(( int*) _temp817) ==  Cyc_Absyn_PointerType: 0){
_LL824: _temp823=(( struct Cyc_Absyn_PointerType_struct*) _temp817)->f1; goto
_LL820;} else{ goto _LL821;} _LL821: goto _LL822; _LL820: return _temp823;
_LL822:( int) _throw(({ void* _temp825[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("get_ptr_typ: not a pointer!",
sizeof( unsigned char), 28u), _tag_arr( _temp825, sizeof( void*), 0u));}));
_LL818:;} struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp826=( void*) e->r; if( e->topt ==  0){({
struct Cyc_Std_String_pa_struct _temp828; _temp828.tag= Cyc_Std_String_pa;
_temp828.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp827[ 1u]={&
_temp828};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u), _tag_arr(
_temp827, sizeof( void*), 1u));}}); return;}{ void* old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp829= _temp826; void*
_temp915; void* _temp917; struct _tuple0* _temp919; struct _tuple0* _temp921;
struct Cyc_List_List* _temp923; void* _temp925; void* _temp927; struct Cyc_Absyn_Exp*
_temp929; struct Cyc_Absyn_Exp* _temp931; struct Cyc_Core_Opt* _temp933; struct
Cyc_Absyn_Exp* _temp935; struct Cyc_Absyn_Exp* _temp937; struct Cyc_Absyn_Exp*
_temp939; struct Cyc_Absyn_Exp* _temp941; struct Cyc_Absyn_Exp* _temp943; struct
Cyc_Absyn_Exp* _temp945; struct Cyc_List_List* _temp947; struct Cyc_Absyn_Exp*
_temp949; struct Cyc_Absyn_VarargCallInfo* _temp951; struct Cyc_List_List*
_temp953; struct Cyc_Absyn_Exp* _temp955; struct Cyc_Absyn_VarargCallInfo*
_temp957; struct Cyc_Absyn_VarargCallInfo _temp959; struct Cyc_Absyn_VarargInfo*
_temp960; struct Cyc_List_List* _temp962; int _temp964; struct Cyc_List_List*
_temp966; struct Cyc_Absyn_Exp* _temp968; struct Cyc_Absyn_Exp* _temp970; struct
Cyc_Absyn_Exp* _temp972; struct Cyc_List_List* _temp974; struct Cyc_Absyn_Exp*
_temp976; struct Cyc_Absyn_Exp* _temp978; void* _temp980; void** _temp982;
struct Cyc_Absyn_Exp* _temp983; struct Cyc_Absyn_Exp* _temp985; struct Cyc_Absyn_Exp*
_temp987; struct Cyc_Absyn_Exp* _temp989; void* _temp991; void* _temp993; struct
_tagged_arr* _temp995; void* _temp997; void* _temp999; unsigned int _temp1001;
void* _temp1003; struct Cyc_Absyn_Exp* _temp1005; struct _tagged_arr* _temp1007;
struct Cyc_Absyn_Exp* _temp1009; struct _tagged_arr* _temp1011; struct Cyc_Absyn_Exp*
_temp1013; struct Cyc_Absyn_Exp* _temp1015; struct Cyc_Absyn_Exp* _temp1017;
struct Cyc_List_List* _temp1019; struct Cyc_List_List* _temp1021; struct Cyc_Absyn_Exp*
_temp1023; struct Cyc_Absyn_Exp* _temp1025; struct Cyc_Absyn_Vardecl* _temp1027;
struct Cyc_Absyn_Structdecl* _temp1029; struct Cyc_List_List* _temp1031; struct
Cyc_Core_Opt* _temp1033; struct _tuple0* _temp1035; struct Cyc_List_List*
_temp1037; void* _temp1039; struct Cyc_Absyn_Tunionfield* _temp1041; struct Cyc_Absyn_Tuniondecl*
_temp1043; struct Cyc_List_List* _temp1045; struct Cyc_Absyn_Tunionfield*
_temp1047; struct Cyc_Absyn_Tuniondecl* _temp1049; struct Cyc_List_List*
_temp1051; struct Cyc_Absyn_MallocInfo _temp1053; int _temp1055; struct Cyc_Absyn_Exp*
_temp1057; void** _temp1059; struct Cyc_Absyn_Exp* _temp1061; int _temp1063;
struct Cyc_Absyn_Stmt* _temp1065; struct Cyc_Absyn_Fndecl* _temp1067; _LL831:
if(*(( int*) _temp829) ==  Cyc_Absyn_Const_e){ _LL916: _temp915=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp829)->f1; if( _temp915 == ( void*) Cyc_Absyn_Null_c){
goto _LL832;} else{ goto _LL833;}} else{ goto _LL833;} _LL833: if(*(( int*)
_temp829) ==  Cyc_Absyn_Const_e){ goto _LL834;} else{ goto _LL835;} _LL835: if(*((
int*) _temp829) ==  Cyc_Absyn_Var_e){ _LL920: _temp919=(( struct Cyc_Absyn_Var_e_struct*)
_temp829)->f1; goto _LL918; _LL918: _temp917=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp829)->f2; goto _LL836;} else{ goto _LL837;} _LL837: if(*(( int*) _temp829)
==  Cyc_Absyn_UnknownId_e){ _LL922: _temp921=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp829)->f1; goto _LL838;} else{ goto _LL839;} _LL839: if(*(( int*) _temp829)
==  Cyc_Absyn_Primop_e){ _LL926: _temp925=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp829)->f1; goto _LL924; _LL924: _temp923=(( struct Cyc_Absyn_Primop_e_struct*)
_temp829)->f2; goto _LL840;} else{ goto _LL841;} _LL841: if(*(( int*) _temp829)
==  Cyc_Absyn_Increment_e){ _LL930: _temp929=(( struct Cyc_Absyn_Increment_e_struct*)
_temp829)->f1; goto _LL928; _LL928: _temp927=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp829)->f2; goto _LL842;} else{ goto _LL843;} _LL843: if(*(( int*) _temp829)
==  Cyc_Absyn_AssignOp_e){ _LL936: _temp935=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp829)->f1; goto _LL934; _LL934: _temp933=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp829)->f2; goto _LL932; _LL932: _temp931=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp829)->f3; goto _LL844;} else{ goto _LL845;} _LL845: if(*(( int*) _temp829)
==  Cyc_Absyn_Conditional_e){ _LL942: _temp941=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp829)->f1; goto _LL940; _LL940: _temp939=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp829)->f2; goto _LL938; _LL938: _temp937=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp829)->f3; goto _LL846;} else{ goto _LL847;} _LL847: if(*(( int*) _temp829)
==  Cyc_Absyn_SeqExp_e){ _LL946: _temp945=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp829)->f1; goto _LL944; _LL944: _temp943=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp829)->f2; goto _LL848;} else{ goto _LL849;} _LL849: if(*(( int*) _temp829)
==  Cyc_Absyn_UnknownCall_e){ _LL950: _temp949=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp829)->f1; goto _LL948; _LL948: _temp947=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp829)->f2; goto _LL850;} else{ goto _LL851;} _LL851: if(*(( int*) _temp829)
==  Cyc_Absyn_FnCall_e){ _LL956: _temp955=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp829)->f1; goto _LL954; _LL954: _temp953=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp829)->f2; goto _LL952; _LL952: _temp951=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp829)->f3; if( _temp951 ==  0){ goto _LL852;} else{ goto _LL853;}} else{
goto _LL853;} _LL853: if(*(( int*) _temp829) ==  Cyc_Absyn_FnCall_e){ _LL969:
_temp968=(( struct Cyc_Absyn_FnCall_e_struct*) _temp829)->f1; goto _LL967;
_LL967: _temp966=(( struct Cyc_Absyn_FnCall_e_struct*) _temp829)->f2; goto
_LL958; _LL958: _temp957=(( struct Cyc_Absyn_FnCall_e_struct*) _temp829)->f3;
if( _temp957 ==  0){ goto _LL855;} else{ _temp959=* _temp957; _LL965: _temp964=
_temp959.num_varargs; goto _LL963; _LL963: _temp962= _temp959.injectors; goto
_LL961; _LL961: _temp960= _temp959.vai; goto _LL854;}} else{ goto _LL855;}
_LL855: if(*(( int*) _temp829) ==  Cyc_Absyn_Throw_e){ _LL971: _temp970=((
struct Cyc_Absyn_Throw_e_struct*) _temp829)->f1; goto _LL856;} else{ goto _LL857;}
_LL857: if(*(( int*) _temp829) ==  Cyc_Absyn_NoInstantiate_e){ _LL973: _temp972=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp829)->f1; goto _LL858;} else{
goto _LL859;} _LL859: if(*(( int*) _temp829) ==  Cyc_Absyn_Instantiate_e){
_LL977: _temp976=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp829)->f1; goto
_LL975; _LL975: _temp974=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp829)->f2;
goto _LL860;} else{ goto _LL861;} _LL861: if(*(( int*) _temp829) ==  Cyc_Absyn_Cast_e){
_LL981: _temp980=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp829)->f1;
_temp982=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp829)->f1);
goto _LL979; _LL979: _temp978=(( struct Cyc_Absyn_Cast_e_struct*) _temp829)->f2;
goto _LL862;} else{ goto _LL863;} _LL863: if(*(( int*) _temp829) ==  Cyc_Absyn_Address_e){
_LL984: _temp983=(( struct Cyc_Absyn_Address_e_struct*) _temp829)->f1; goto
_LL864;} else{ goto _LL865;} _LL865: if(*(( int*) _temp829) ==  Cyc_Absyn_New_e){
_LL988: _temp987=(( struct Cyc_Absyn_New_e_struct*) _temp829)->f1; goto _LL986;
_LL986: _temp985=(( struct Cyc_Absyn_New_e_struct*) _temp829)->f2; goto _LL866;}
else{ goto _LL867;} _LL867: if(*(( int*) _temp829) ==  Cyc_Absyn_Sizeofexp_e){
_LL990: _temp989=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp829)->f1; goto
_LL868;} else{ goto _LL869;} _LL869: if(*(( int*) _temp829) ==  Cyc_Absyn_Sizeoftyp_e){
_LL992: _temp991=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp829)->f1;
goto _LL870;} else{ goto _LL871;} _LL871: if(*(( int*) _temp829) ==  Cyc_Absyn_Gentyp_e){
goto _LL872;} else{ goto _LL873;} _LL873: if(*(( int*) _temp829) ==  Cyc_Absyn_Offsetof_e){
_LL998: _temp997=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp829)->f1;
goto _LL994; _LL994: _temp993=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp829)->f2; if(*(( int*) _temp993) ==  Cyc_Absyn_StructField){ _LL996:
_temp995=(( struct Cyc_Absyn_StructField_struct*) _temp993)->f1; goto _LL874;}
else{ goto _LL875;}} else{ goto _LL875;} _LL875: if(*(( int*) _temp829) ==  Cyc_Absyn_Offsetof_e){
_LL1004: _temp1003=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp829)->f1;
goto _LL1000; _LL1000: _temp999=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp829)->f2; if(*(( int*) _temp999) ==  Cyc_Absyn_TupleIndex){ _LL1002:
_temp1001=(( struct Cyc_Absyn_TupleIndex_struct*) _temp999)->f1; goto _LL876;}
else{ goto _LL877;}} else{ goto _LL877;} _LL877: if(*(( int*) _temp829) ==  Cyc_Absyn_Deref_e){
_LL1006: _temp1005=(( struct Cyc_Absyn_Deref_e_struct*) _temp829)->f1; goto
_LL878;} else{ goto _LL879;} _LL879: if(*(( int*) _temp829) ==  Cyc_Absyn_StructMember_e){
_LL1010: _temp1009=(( struct Cyc_Absyn_StructMember_e_struct*) _temp829)->f1;
goto _LL1008; _LL1008: _temp1007=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp829)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if(*(( int*) _temp829)
==  Cyc_Absyn_StructArrow_e){ _LL1014: _temp1013=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp829)->f1; goto _LL1012; _LL1012: _temp1011=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp829)->f2; goto _LL882;} else{ goto _LL883;} _LL883: if(*(( int*) _temp829)
==  Cyc_Absyn_Subscript_e){ _LL1018: _temp1017=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp829)->f1; goto _LL1016; _LL1016: _temp1015=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp829)->f2; goto _LL884;} else{ goto _LL885;} _LL885: if(*(( int*) _temp829)
==  Cyc_Absyn_Tuple_e){ _LL1020: _temp1019=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp829)->f1; goto _LL886;} else{ goto _LL887;} _LL887: if(*(( int*) _temp829)
==  Cyc_Absyn_Array_e){ _LL1022: _temp1021=(( struct Cyc_Absyn_Array_e_struct*)
_temp829)->f1; goto _LL888;} else{ goto _LL889;} _LL889: if(*(( int*) _temp829)
==  Cyc_Absyn_Comprehension_e){ _LL1028: _temp1027=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp829)->f1; goto _LL1026; _LL1026: _temp1025=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp829)->f2; goto _LL1024; _LL1024: _temp1023=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp829)->f3; goto _LL890;} else{ goto _LL891;} _LL891: if(*(( int*) _temp829)
==  Cyc_Absyn_Struct_e){ _LL1036: _temp1035=(( struct Cyc_Absyn_Struct_e_struct*)
_temp829)->f1; goto _LL1034; _LL1034: _temp1033=(( struct Cyc_Absyn_Struct_e_struct*)
_temp829)->f2; goto _LL1032; _LL1032: _temp1031=(( struct Cyc_Absyn_Struct_e_struct*)
_temp829)->f3; goto _LL1030; _LL1030: _temp1029=(( struct Cyc_Absyn_Struct_e_struct*)
_temp829)->f4; goto _LL892;} else{ goto _LL893;} _LL893: if(*(( int*) _temp829)
==  Cyc_Absyn_AnonStruct_e){ _LL1040: _temp1039=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp829)->f1; goto _LL1038; _LL1038: _temp1037=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp829)->f2; goto _LL894;} else{ goto _LL895;} _LL895: if(*(( int*) _temp829)
==  Cyc_Absyn_Tunion_e){ _LL1046: _temp1045=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp829)->f3; if( _temp1045 ==  0){ goto _LL1044;} else{ goto _LL897;} _LL1044:
_temp1043=(( struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f4; goto _LL1042;
_LL1042: _temp1041=(( struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f5; goto
_LL896;} else{ goto _LL897;} _LL897: if(*(( int*) _temp829) ==  Cyc_Absyn_Tunion_e){
_LL1052: _temp1051=(( struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f3; goto
_LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f4;
goto _LL1048; _LL1048: _temp1047=(( struct Cyc_Absyn_Tunion_e_struct*) _temp829)->f5;
goto _LL898;} else{ goto _LL899;} _LL899: if(*(( int*) _temp829) ==  Cyc_Absyn_Enum_e){
goto _LL900;} else{ goto _LL901;} _LL901: if(*(( int*) _temp829) ==  Cyc_Absyn_AnonEnum_e){
goto _LL902;} else{ goto _LL903;} _LL903: if(*(( int*) _temp829) ==  Cyc_Absyn_Malloc_e){
_LL1054: _temp1053=(( struct Cyc_Absyn_Malloc_e_struct*) _temp829)->f1; _LL1064:
_temp1063= _temp1053.is_calloc; goto _LL1062; _LL1062: _temp1061= _temp1053.rgn;
goto _LL1060; _LL1060: _temp1059= _temp1053.elt_type; goto _LL1058; _LL1058:
_temp1057= _temp1053.num_elts; goto _LL1056; _LL1056: _temp1055= _temp1053.fat_result;
goto _LL904;} else{ goto _LL905;} _LL905: if(*(( int*) _temp829) ==  Cyc_Absyn_StmtExp_e){
_LL1066: _temp1065=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp829)->f1; goto
_LL906;} else{ goto _LL907;} _LL907: if(*(( int*) _temp829) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL908;} else{ goto _LL909;} _LL909: if(*(( int*) _temp829) ==  Cyc_Absyn_CompoundLit_e){
goto _LL910;} else{ goto _LL911;} _LL911: if(*(( int*) _temp829) ==  Cyc_Absyn_Codegen_e){
_LL1068: _temp1067=(( struct Cyc_Absyn_Codegen_e_struct*) _temp829)->f1; goto
_LL912;} else{ goto _LL913;} _LL913: if(*(( int*) _temp829) ==  Cyc_Absyn_Fill_e){
goto _LL914;} else{ goto _LL830;} _LL832: { struct Cyc_Absyn_Exp* _temp1069= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1069,
_temp1069))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1070[ 3u]; _temp1070[ 2u]=
_temp1069; _temp1070[ 1u]= _temp1069; _temp1070[ 0u]= _temp1069;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1070, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL830;} _LL834: goto _LL830; _LL836:{ struct _handler_cons
_temp1071; _push_handler(& _temp1071);{ int _temp1073= 0; if( setjmp( _temp1071.handler)){
_temp1073= 1;} if( ! _temp1073){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp919))->r));; _pop_handler();} else{ void* _temp1072=( void*) _exn_thrown;
void* _temp1075= _temp1072; _LL1077: if( _temp1075 ==  Cyc_Dict_Absent){ goto
_LL1078;} else{ goto _LL1079;} _LL1079: goto _LL1080; _LL1078:({ struct Cyc_Std_String_pa_struct
_temp1082; _temp1082.tag= Cyc_Std_String_pa; _temp1082.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp919);{ void* _temp1081[ 1u]={& _temp1082};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1081, sizeof( void*), 1u));}});
return; _LL1080:( void) _throw( _temp1075); _LL1076:;}}} goto _LL830; _LL838:({
void* _temp1083[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1083, sizeof( void*), 0u));}); goto _LL830; _LL840: { struct Cyc_List_List*
_temp1084=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp923);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp923);{
void* _temp1085= _temp925; _LL1087: if( _temp1085 == ( void*) Cyc_Absyn_Size){
goto _LL1088;} else{ goto _LL1089;} _LL1089: if( _temp1085 == ( void*) Cyc_Absyn_Plus){
goto _LL1090;} else{ goto _LL1091;} _LL1091: if( _temp1085 == ( void*) Cyc_Absyn_Minus){
goto _LL1092;} else{ goto _LL1093;} _LL1093: if( _temp1085 == ( void*) Cyc_Absyn_Eq){
goto _LL1094;} else{ goto _LL1095;} _LL1095: if( _temp1085 == ( void*) Cyc_Absyn_Neq){
goto _LL1096;} else{ goto _LL1097;} _LL1097: if( _temp1085 == ( void*) Cyc_Absyn_Gt){
goto _LL1098;} else{ goto _LL1099;} _LL1099: if( _temp1085 == ( void*) Cyc_Absyn_Gte){
goto _LL1100;} else{ goto _LL1101;} _LL1101: if( _temp1085 == ( void*) Cyc_Absyn_Lt){
goto _LL1102;} else{ goto _LL1103;} _LL1103: if( _temp1085 == ( void*) Cyc_Absyn_Lte){
goto _LL1104;} else{ goto _LL1105;} _LL1105: goto _LL1106; _LL1088: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp923))->hd;{
void* _temp1107= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1115; struct Cyc_Absyn_PtrInfo
_temp1117; struct Cyc_Absyn_Conref* _temp1119; void* _temp1121; _LL1109: if((
unsigned int) _temp1107 >  3u?*(( int*) _temp1107) ==  Cyc_Absyn_ArrayType: 0){
_LL1116: _temp1115=(( struct Cyc_Absyn_ArrayType_struct*) _temp1107)->f3; goto
_LL1110;} else{ goto _LL1111;} _LL1111: if(( unsigned int) _temp1107 >  3u?*((
int*) _temp1107) ==  Cyc_Absyn_PointerType: 0){ _LL1118: _temp1117=(( struct Cyc_Absyn_PointerType_struct*)
_temp1107)->f1; _LL1122: _temp1121=( void*) _temp1117.elt_typ; goto _LL1120;
_LL1120: _temp1119= _temp1117.bounds; goto _LL1112;} else{ goto _LL1113;}
_LL1113: goto _LL1114; _LL1110:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1115))->r)); goto _LL1108; _LL1112:{ void* _temp1123= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1119); struct Cyc_Absyn_Exp* _temp1129; _LL1125: if(
_temp1123 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1126;} else{ goto _LL1127;}
_LL1127: if(( unsigned int) _temp1123 >  1u?*(( int*) _temp1123) ==  Cyc_Absyn_Upper_b:
0){ _LL1130: _temp1129=(( struct Cyc_Absyn_Upper_b_struct*) _temp1123)->f1; goto
_LL1128;} else{ goto _LL1124;} _LL1126:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1131[ 2u]; _temp1131[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1121), 0); _temp1131[ 0u]=(
struct Cyc_Absyn_Exp*) _temp923->hd;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1131, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0))->r)); goto _LL1124; _LL1128:( void*)( e->r=( void*)(( void*)
_temp1129->r)); goto _LL1124; _LL1124:;} goto _LL1108; _LL1114:({ struct Cyc_Std_String_pa_struct
_temp1134; _temp1134.tag= Cyc_Std_String_pa; _temp1134.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1133; _temp1133.tag= Cyc_Std_String_pa;
_temp1133.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v);{ void* _temp1132[ 2u]={& _temp1133,& _temp1134};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("size primop applied to non-array %s (%s)", sizeof( unsigned char), 41u),
_tag_arr( _temp1132, sizeof( void*), 2u));}}}); return; _LL1108:;} goto _LL1086;}
_LL1090: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1084))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp923))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp923->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1135[ 3u]; _temp1135[ 2u]=
e2; _temp1135[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1135[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1135, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1086;} _LL1092: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1084))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp923))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp923->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null( _temp1084->tl))->hd)){( void*)( e1->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp,
0))->r));( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_divide_exp( Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0), 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1136[ 3u]; _temp1136[ 2u]=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, e2, 0); _temp1136[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1136[ 0u]= e1;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1136, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} goto _LL1086;} _LL1094: goto _LL1096; _LL1096: goto _LL1098;
_LL1098: goto _LL1100; _LL1100: goto _LL1102; _LL1102: goto _LL1104; _LL1104: {
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp923))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp923->tl))->hd; void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp1084))->hd; void* t2=( void*)(( struct
Cyc_List_List*) _check_null( _temp1084->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1086;} _LL1106: goto
_LL1086; _LL1086:;} goto _LL830;} _LL842: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp929->topt))->v; Cyc_Toc_exp_to_c( nv, _temp929);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1137=( _temp927 == ( void*) Cyc_Absyn_PostInc?
1: _temp927 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1138=( _temp927 == ( void*) Cyc_Absyn_PreInc?
1: _temp927 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1137,({ struct Cyc_Absyn_Exp* _temp1139[ 3u];
_temp1139[ 2u]= Cyc_Absyn_signed_int_exp( _temp1138, 0); _temp1139[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1139[ 0u]= Cyc_Absyn_address_exp( _temp929,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1139, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp929): 0){ Cyc_Toc_lvalue_assign(
_temp929, 0, Cyc_Toc_incr_lvalue, _temp927);( void*)( e->r=( void*)(( void*)
_temp929->r));} goto _LL830;}} _LL844: { int e1_poly= Cyc_Toc_is_poly_project(
_temp935); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp935->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp931->topt))->v; Cyc_Toc_exp_to_c( nv, _temp935); Cyc_Toc_exp_to_c(
nv, _temp931);{ int done= 0; if( _temp933 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1140=( void*) _temp933->v; _LL1142: if( _temp1140 == ( void*)
Cyc_Absyn_Plus){ goto _LL1143;} else{ goto _LL1144;} _LL1144: if( _temp1140 == (
void*) Cyc_Absyn_Minus){ goto _LL1145;} else{ goto _LL1146;} _LL1146: goto
_LL1147; _LL1143: change= _temp931; goto _LL1141; _LL1145: change= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, _temp931, 0); goto _LL1141; _LL1147:( int) _throw(({
void* _temp1148[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u), _tag_arr( _temp1148, sizeof( void*), 0u));}));
goto _LL1141; _LL1141:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_Absyn_Exp* _temp1149[ 3u];
_temp1149[ 2u]= change; _temp1149[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1149[ 0u]= Cyc_Absyn_address_exp( _temp935, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1149, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp931->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp931->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp935)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)(
_temp935, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1150=( struct
_tuple7*) _cycalloc( sizeof( struct _tuple7) *  1); _temp1150[ 0]=({ struct
_tuple7 _temp1151; _temp1151.f1= _temp933; _temp1151.f2= _temp931; _temp1151;});
_temp1150;}));( void*)( e->r=( void*)(( void*) _temp935->r));}} goto _LL830;}}
_LL846: Cyc_Toc_exp_to_c( nv, _temp941); Cyc_Toc_exp_to_c( nv, _temp939); Cyc_Toc_exp_to_c(
nv, _temp937); goto _LL830; _LL848: Cyc_Toc_exp_to_c( nv, _temp945); Cyc_Toc_exp_to_c(
nv, _temp943); goto _LL830; _LL850: _temp955= _temp949; _temp953= _temp947; goto
_LL852; _LL852: Cyc_Toc_exp_to_c( nv, _temp955);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp953); goto _LL830; _LL854:{
struct _RegionHandle _temp1152= _new_region("r"); struct _RegionHandle* r=&
_temp1152; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp964, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp960->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1169=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1169[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1170; _temp1170.tag= Cyc_Absyn_ArrayType;
_temp1170.f1=( void*) cva_type; _temp1170.f2= Cyc_Toc_mt_tq; _temp1170.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1170;}); _temp1169;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp966); int num_normargs=
num_args -  _temp964; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp966= _temp966->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp966))->hd); new_args=({ struct Cyc_List_List*
_temp1153=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1153->hd=( void*)(( struct Cyc_Absyn_Exp*) _temp966->hd); _temp1153->tl=
new_args; _temp1153;});}} new_args=({ struct Cyc_List_List* _temp1154=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1154->hd=( void*)
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1155[ 3u];
_temp1155[ 2u]= num_varargs_exp; _temp1155[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1155[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1155, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1154->tl= new_args; _temp1154;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp968);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp968, new_args, 0), 0); if( _temp960->inject){ struct _tagged_arr vs=({
unsigned int _temp1165=( unsigned int) _temp964; struct _tuple0** _temp1166=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1165)); struct _tagged_arr _temp1168= _tag_arr( _temp1166, sizeof( struct
_tuple0*),( unsigned int) _temp964);{ unsigned int _temp1167= _temp1165;
unsigned int i; for( i= 0; i <  _temp1167; i ++){ _temp1166[ i]= Cyc_Toc_temp_var();}};
_temp1168;}); struct Cyc_List_List* _temp1156= 0;{ int i= _temp964 -  1; for( 0;
i >=  0; -- i){ _temp1156=({ struct Cyc_List_List* _temp1157=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1157->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1157->tl=
_temp1156; _temp1157;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1156, 0), s, 0);{ int i= 0;
for( 0; _temp966 !=  0;((( _temp966= _temp966->tl, _temp962= _temp962->tl)), ++
i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*) _temp966->hd; void*
arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; struct
_tuple0* var=*(( struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct
_tuple0*), i)); struct Cyc_Absyn_Exp* varexp= Cyc_Absyn_var_exp( var, 0); struct
Cyc_Absyn_Tunionfield _temp1160; struct Cyc_List_List* _temp1161; struct _tuple0*
_temp1163; struct Cyc_Absyn_Tunionfield* _temp1158=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp962))->hd; _temp1160=* _temp1158;
_LL1164: _temp1163= _temp1160.name; goto _LL1162; _LL1162: _temp1161= _temp1160.typs;
goto _LL1159; _LL1159: { void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1161))->hd)).f2); Cyc_Toc_exp_to_c( nv,
arg); if( Cyc_Toc_is_void_star( field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ,
arg, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Absyn_fieldname( 1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1163, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt( var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1163, _tag_arr("_struct", sizeof( unsigned char), 8u))), 0, s, 0);}}}}
else{{ int i= 0; for( 0; _temp966 !=  0;( _temp966= _temp966->tl, ++ i)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp966->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),(
struct Cyc_Absyn_Exp*) _temp966->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL830; _LL856: Cyc_Toc_exp_to_c( nv,
_temp970);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp970), 0))->r)); goto _LL830; _LL858: Cyc_Toc_exp_to_c(
nv, _temp972); goto _LL830; _LL860: Cyc_Toc_exp_to_c( nv, _temp976); for( 0;
_temp974 !=  0; _temp974= _temp974->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)
_temp974->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1171= Cyc_Tcutil_compress(( void*) _temp974->hd); _LL1173: if((
unsigned int) _temp1171 >  3u?*(( int*) _temp1171) ==  Cyc_Absyn_VarType: 0){
goto _LL1174;} else{ goto _LL1175;} _LL1175: if(( unsigned int) _temp1171 >  3u?*((
int*) _temp1171) ==  Cyc_Absyn_TunionType: 0){ goto _LL1176;} else{ goto _LL1177;}
_LL1177: goto _LL1178; _LL1174: continue; _LL1176: continue; _LL1178:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), _temp976, 0))->r)); goto _LL1172;
_LL1172:;} break;}} goto _LL830; _LL862: { void* old_t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp978->topt))->v; void* new_typ=* _temp982;* _temp982= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv, _temp978);{ struct _tuple8 _temp1180=({ struct
_tuple8 _temp1179; _temp1179.f1= Cyc_Tcutil_compress( old_t2); _temp1179.f2= Cyc_Tcutil_compress(
new_typ); _temp1179;}); void* _temp1188; struct Cyc_Absyn_PtrInfo _temp1190;
void* _temp1192; struct Cyc_Absyn_PtrInfo _temp1194; void* _temp1196; void*
_temp1198; struct Cyc_Absyn_PtrInfo _temp1200; _LL1182: _LL1193: _temp1192=
_temp1180.f1; if(( unsigned int) _temp1192 >  3u?*(( int*) _temp1192) ==  Cyc_Absyn_PointerType:
0){ _LL1195: _temp1194=(( struct Cyc_Absyn_PointerType_struct*) _temp1192)->f1;
goto _LL1189;} else{ goto _LL1184;} _LL1189: _temp1188= _temp1180.f2; if((
unsigned int) _temp1188 >  3u?*(( int*) _temp1188) ==  Cyc_Absyn_PointerType: 0){
_LL1191: _temp1190=(( struct Cyc_Absyn_PointerType_struct*) _temp1188)->f1; goto
_LL1183;} else{ goto _LL1184;} _LL1184: _LL1199: _temp1198= _temp1180.f1; if((
unsigned int) _temp1198 >  3u?*(( int*) _temp1198) ==  Cyc_Absyn_PointerType: 0){
_LL1201: _temp1200=(( struct Cyc_Absyn_PointerType_struct*) _temp1198)->f1; goto
_LL1197;} else{ goto _LL1186;} _LL1197: _temp1196= _temp1180.f2; if((
unsigned int) _temp1196 >  3u?*(( int*) _temp1196) ==  Cyc_Absyn_IntType: 0){
goto _LL1185;} else{ goto _LL1186;} _LL1186: goto _LL1187; _LL1183: { int
_temp1202=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1194.nullable); int _temp1203=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1190.nullable); void* _temp1204= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1194.bounds); void* _temp1205= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1190.bounds);{ struct _tuple8 _temp1207=({ struct
_tuple8 _temp1206; _temp1206.f1= _temp1204; _temp1206.f2= _temp1205; _temp1206;});
void* _temp1217; void* _temp1219; void* _temp1221; void* _temp1223; struct Cyc_Absyn_Exp*
_temp1225; void* _temp1227; struct Cyc_Absyn_Exp* _temp1229; void* _temp1231;
_LL1209: _LL1220: _temp1219= _temp1207.f1; if(( unsigned int) _temp1219 >  1u?*((
int*) _temp1219) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1218;} else{ goto _LL1211;}
_LL1218: _temp1217= _temp1207.f2; if(( unsigned int) _temp1217 >  1u?*(( int*)
_temp1217) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1210;} else{ goto _LL1211;}
_LL1211: _LL1224: _temp1223= _temp1207.f1; if(( unsigned int) _temp1223 >  1u?*((
int*) _temp1223) ==  Cyc_Absyn_Upper_b: 0){ _LL1226: _temp1225=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1223)->f1; goto _LL1222;} else{ goto _LL1213;} _LL1222: _temp1221=
_temp1207.f2; if( _temp1221 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1212;}
else{ goto _LL1213;} _LL1213: _LL1232: _temp1231= _temp1207.f1; if( _temp1231 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1228;} else{ goto _LL1215;} _LL1228:
_temp1227= _temp1207.f2; if(( unsigned int) _temp1227 >  1u?*(( int*) _temp1227)
==  Cyc_Absyn_Upper_b: 0){ _LL1230: _temp1229=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1227)->f1; goto _LL1214;} else{ goto _LL1215;} _LL1215: goto _LL1216;
_LL1210: if( _temp1202? ! _temp1203: 0){ if( nv->toplevel){({ void* _temp1233[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1233, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp982, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1234=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1234->hd=( void*) _temp978;
_temp1234->tl= 0; _temp1234;}), 0), 0))->r));} goto _LL1208; _LL1212: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1225, _temp978))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1235[ 3u]; _temp1235[ 2u]=
_temp1225; _temp1235[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1190.elt_typ), 0); _temp1235[ 0u]= _temp978;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1235, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1208; _LL1214: if( nv->toplevel){({ void* _temp1236[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1236, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1237= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1239[ 3u]; _temp1239[ 2u]= _temp1229; _temp1239[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1194.elt_typ), 0); _temp1239[ 0u]= _temp978;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1239, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1203){( void*)(
_temp1237->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1238=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1238->hd=( void*) Cyc_Absyn_copy_exp( _temp1237);
_temp1238->tl= 0; _temp1238;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp982, _temp1237, 0))->r)); goto _LL1208;} _LL1216: goto _LL1208; _LL1208:;}
goto _LL1181;} _LL1185:{ void* _temp1240= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1200.bounds); _LL1242: if( _temp1240 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1243;} else{ goto _LL1244;} _LL1244: goto _LL1245; _LL1243:( void*)( _temp978->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp978->r,
_temp978->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1241; _LL1245: goto _LL1241;
_LL1241:;} goto _LL1181; _LL1187: goto _LL1181; _LL1181:;} goto _LL830;} _LL864:{
void* _temp1246=( void*) _temp983->r; struct Cyc_List_List* _temp1254; struct
_tuple0* _temp1256; struct Cyc_List_List* _temp1258; _LL1248: if(*(( int*)
_temp1246) ==  Cyc_Absyn_Struct_e){ _LL1257: _temp1256=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1246)->f1; goto _LL1255; _LL1255: _temp1254=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1246)->f3; goto _LL1249;} else{ goto _LL1250;} _LL1250: if(*(( int*)
_temp1246) ==  Cyc_Absyn_Tuple_e){ _LL1259: _temp1258=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1246)->f1; goto _LL1251;} else{ goto _LL1252;} _LL1252: goto _LL1253;
_LL1249: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1261;
_temp1261.tag= Cyc_Std_String_pa; _temp1261.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp983->loc);{ void* _temp1260[ 1u]={& _temp1261};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1260, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp983->topt))->v, 1, 0, _temp1254, _temp1256))->r)); goto
_LL1247; _LL1251: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1263;
_temp1263.tag= Cyc_Std_String_pa; _temp1263.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp983->loc);{ void* _temp1262[ 1u]={& _temp1263};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1262, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1258))->r));
goto _LL1247; _LL1253: Cyc_Toc_exp_to_c( nv, _temp983); if( ! Cyc_Absyn_is_lvalue(
_temp983)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp983, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp983, 0))->r));} goto _LL1247; _LL1247:;} goto _LL830; _LL866: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1265; _temp1265.tag= Cyc_Std_String_pa;
_temp1265.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp985->loc);{
void* _temp1264[ 1u]={& _temp1265};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1264, sizeof( void*), 1u));}});}{ void*
_temp1266=( void*) _temp985->r; struct Cyc_List_List* _temp1278; struct Cyc_Absyn_Exp*
_temp1280; struct Cyc_Absyn_Exp* _temp1282; struct Cyc_Absyn_Vardecl* _temp1284;
struct Cyc_List_List* _temp1286; struct _tuple0* _temp1288; struct Cyc_List_List*
_temp1290; _LL1268: if(*(( int*) _temp1266) ==  Cyc_Absyn_Array_e){ _LL1279:
_temp1278=(( struct Cyc_Absyn_Array_e_struct*) _temp1266)->f1; goto _LL1269;}
else{ goto _LL1270;} _LL1270: if(*(( int*) _temp1266) ==  Cyc_Absyn_Comprehension_e){
_LL1285: _temp1284=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1266)->f1;
goto _LL1283; _LL1283: _temp1282=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1266)->f2; goto _LL1281; _LL1281: _temp1280=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1266)->f3; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*)
_temp1266) ==  Cyc_Absyn_Struct_e){ _LL1289: _temp1288=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1266)->f1; goto _LL1287; _LL1287: _temp1286=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1266)->f3; goto _LL1273;} else{ goto _LL1274;} _LL1274: if(*(( int*)
_temp1266) ==  Cyc_Absyn_Tuple_e){ _LL1291: _temp1290=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1266)->f1; goto _LL1275;} else{ goto _LL1276;} _LL1276: goto _LL1277;
_LL1269: { struct _tuple0* _temp1292= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1293= Cyc_Absyn_var_exp( _temp1292, 0); struct Cyc_Absyn_Stmt* _temp1294=
Cyc_Toc_init_array( nv, _temp1293, _temp1278, Cyc_Absyn_exp_stmt( _temp1293, 0));
void* old_elt_typ;{ void* _temp1295= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1301; struct Cyc_Absyn_Tqual _temp1303; void* _temp1305; _LL1297: if((
unsigned int) _temp1295 >  3u?*(( int*) _temp1295) ==  Cyc_Absyn_PointerType: 0){
_LL1302: _temp1301=(( struct Cyc_Absyn_PointerType_struct*) _temp1295)->f1;
_LL1306: _temp1305=( void*) _temp1301.elt_typ; goto _LL1304; _LL1304: _temp1303=
_temp1301.tq; goto _LL1298;} else{ goto _LL1299;} _LL1299: goto _LL1300; _LL1298:
old_elt_typ= _temp1305; goto _LL1296; _LL1300: old_elt_typ=({ void* _temp1307[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1307, sizeof( void*), 0u));}); goto
_LL1296; _LL1296:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1308= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1309= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1278), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp987 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1309);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp987); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1309);}{
struct Cyc_Absyn_Exp* _temp1310= Cyc_Absyn_cast_exp( _temp1308, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1292,
_temp1308,( struct Cyc_Absyn_Exp*) _temp1310, _temp1294, 0), 0))->r)); goto
_LL1267;}}} _LL1271: { int is_tagged_ptr= 0;{ void* _temp1311= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1317; struct Cyc_Absyn_Conref* _temp1319;
struct Cyc_Absyn_Tqual _temp1321; void* _temp1323; _LL1313: if(( unsigned int)
_temp1311 >  3u?*(( int*) _temp1311) ==  Cyc_Absyn_PointerType: 0){ _LL1318:
_temp1317=(( struct Cyc_Absyn_PointerType_struct*) _temp1311)->f1; _LL1324:
_temp1323=( void*) _temp1317.elt_typ; goto _LL1322; _LL1322: _temp1321=
_temp1317.tq; goto _LL1320; _LL1320: _temp1319= _temp1317.bounds; goto _LL1314;}
else{ goto _LL1315;} _LL1315: goto _LL1316; _LL1314: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1319) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1312;
_LL1316:({ void* _temp1325[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1325, sizeof( void*), 0u));}); goto
_LL1312; _LL1312:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1280->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1282);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1284, Cyc_Absyn_var_exp( max, 0), _temp1280, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1326= _new_region("r"); struct _RegionHandle* r=&
_temp1326; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1346=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1346->hd=( void*)({ struct _tuple9* _temp1347=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1347->f1= max; _temp1347->f2=
Cyc_Absyn_uint_t; _temp1347->f3=( struct Cyc_Absyn_Exp*) _temp1282; _temp1347;});
_temp1346->tl= 0; _temp1346;}); struct Cyc_Absyn_Exp* ai; if( _temp987 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1327[ 2u]; _temp1327[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1327[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1327, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp987); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1328[
2u]; _temp1328[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1328[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1328, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1329=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1329->hd=( void*)({ struct _tuple9* _temp1330=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1330->f1= a; _temp1330->f2=
ptr_typ; _temp1330->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1330;}); _temp1329->tl=
decls; _temp1329;}); if( is_tagged_ptr){ struct _tuple0* _temp1331= Cyc_Toc_temp_var();
void* _temp1332= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1333=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1336[ 3u];
_temp1336[ 2u]= _temp1282; _temp1336[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1336[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1336, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1334=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1334->hd=( void*)({
struct _tuple9* _temp1335=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1335->f1= _temp1331; _temp1335->f2= _temp1332; _temp1335->f3=(
struct Cyc_Absyn_Exp*) _temp1333; _temp1335;}); _temp1334->tl= decls; _temp1334;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1331, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1337= decls; for( 0; _temp1337 !=  0;
_temp1337= _temp1337->tl){ struct Cyc_Absyn_Exp* _temp1340; void* _temp1342;
struct _tuple0* _temp1344; struct _tuple9 _temp1338=*(( struct _tuple9*)
_temp1337->hd); _LL1345: _temp1344= _temp1338.f1; goto _LL1343; _LL1343:
_temp1342= _temp1338.f2; goto _LL1341; _LL1341: _temp1340= _temp1338.f3; goto
_LL1339; _LL1339: s= Cyc_Absyn_declare_stmt( _temp1344, _temp1342, _temp1340, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region( r);} goto _LL1267;}}} _LL1273:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp985->topt))->v, 1,
_temp987, _temp1286, _temp1288))->r)); goto _LL1267; _LL1275:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp987, _temp1290))->r)); goto
_LL1267; _LL1277: { void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp985->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); if( _temp987
==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp987); Cyc_Toc_exp_to_c( nv, r);
mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1348=( void*)
_temp985->r; struct Cyc_Absyn_Exp* _temp1354; void* _temp1356; _LL1350: if(*((
int*) _temp1348) ==  Cyc_Absyn_Cast_e){ _LL1357: _temp1356=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1348)->f1; goto _LL1355; _LL1355: _temp1354=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1348)->f2; goto _LL1351;} else{ goto _LL1352;} _LL1352: goto _LL1353;
_LL1351:{ struct _tuple8 _temp1359=({ struct _tuple8 _temp1358; _temp1358.f1=
Cyc_Tcutil_compress( _temp1356); _temp1358.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1354->topt))->v); _temp1358;}); void*
_temp1365; struct Cyc_Absyn_PtrInfo _temp1367; struct Cyc_Absyn_Conref*
_temp1369; void* _temp1371; struct Cyc_Absyn_PtrInfo _temp1373; struct Cyc_Absyn_Conref*
_temp1375; void* _temp1377; _LL1361: _LL1372: _temp1371= _temp1359.f1; if((
unsigned int) _temp1371 >  3u?*(( int*) _temp1371) ==  Cyc_Absyn_PointerType: 0){
_LL1374: _temp1373=(( struct Cyc_Absyn_PointerType_struct*) _temp1371)->f1;
_LL1378: _temp1377=( void*) _temp1373.elt_typ; goto _LL1376; _LL1376: _temp1375=
_temp1373.bounds; goto _LL1366;} else{ goto _LL1363;} _LL1366: _temp1365=
_temp1359.f2; if(( unsigned int) _temp1365 >  3u?*(( int*) _temp1365) ==  Cyc_Absyn_PointerType:
0){ _LL1368: _temp1367=(( struct Cyc_Absyn_PointerType_struct*) _temp1365)->f1;
_LL1370: _temp1369= _temp1367.bounds; goto _LL1362;} else{ goto _LL1363;}
_LL1363: goto _LL1364; _LL1362:{ struct _tuple8 _temp1380=({ struct _tuple8
_temp1379; _temp1379.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1375);
_temp1379.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1369); _temp1379;});
void* _temp1386; struct Cyc_Absyn_Exp* _temp1388; void* _temp1390; _LL1382:
_LL1391: _temp1390= _temp1380.f1; if( _temp1390 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1387;} else{ goto _LL1384;} _LL1387: _temp1386= _temp1380.f2; if((
unsigned int) _temp1386 >  1u?*(( int*) _temp1386) ==  Cyc_Absyn_Upper_b: 0){
_LL1389: _temp1388=(( struct Cyc_Absyn_Upper_b_struct*) _temp1386)->f1; goto
_LL1383;} else{ goto _LL1384;} _LL1384: goto _LL1385; _LL1383: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1392[ 4u]; _temp1392[ 3u]= _temp1388; _temp1392[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1377, 0); _temp1392[ 1u]= _temp1354; _temp1392[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1392, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1381; _LL1385: goto _LL1381; _LL1381:;} goto _LL1360;
_LL1364: goto _LL1360; _LL1360:;} goto _LL1349; _LL1353: goto _LL1349; _LL1349:;}
if( ! done){ struct _tuple0* _temp1393= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1394= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1393, 0), 0); struct Cyc_Absyn_Exp*
_temp1395= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp985);
_temp1394= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1393, 0), _temp1395, 0), _temp985, 0), _temp1394, 0);{
void* _temp1396= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1393,
_temp1396,( struct Cyc_Absyn_Exp*) mexp, _temp1394, 0), 0))->r));}} goto _LL1267;}}
_LL1267:;} goto _LL830; _LL868: Cyc_Toc_exp_to_c( nv, _temp989); goto _LL830;
_LL870:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1397=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1397[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1398; _temp1398.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1398.f1=( void*) Cyc_Toc_typ_to_c_array( _temp991);
_temp1398;}); _temp1397;}))); goto _LL830; _LL872:( int) _throw(({ void*
_temp1399[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("__gen() in code generator",
sizeof( unsigned char), 26u), _tag_arr( _temp1399, sizeof( void*), 0u));}));
_LL874:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1400=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1400[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1401; _temp1401.tag=
Cyc_Absyn_Offsetof_e; _temp1401.f1=( void*) Cyc_Toc_typ_to_c_array( _temp997);
_temp1401.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1402=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1402[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1403; _temp1403.tag=
Cyc_Absyn_StructField; _temp1403.f1= _temp995; _temp1403;}); _temp1402;}));
_temp1401;}); _temp1400;}))); goto _LL830; _LL876:{ void* _temp1404= Cyc_Tcutil_compress(
_temp1003); struct Cyc_Absyn_Structdecl** _temp1420; struct Cyc_List_List*
_temp1422; struct Cyc_Absyn_Uniondecl** _temp1424; struct Cyc_List_List*
_temp1426; _LL1406: if(( unsigned int) _temp1404 >  3u?*(( int*) _temp1404) == 
Cyc_Absyn_StructType: 0){ _LL1421: _temp1420=(( struct Cyc_Absyn_StructType_struct*)
_temp1404)->f3; goto _LL1407;} else{ goto _LL1408;} _LL1408: if(( unsigned int)
_temp1404 >  3u?*(( int*) _temp1404) ==  Cyc_Absyn_AnonStructType: 0){ _LL1423:
_temp1422=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1404)->f1; goto
_LL1409;} else{ goto _LL1410;} _LL1410: if(( unsigned int) _temp1404 >  3u?*((
int*) _temp1404) ==  Cyc_Absyn_UnionType: 0){ _LL1425: _temp1424=(( struct Cyc_Absyn_UnionType_struct*)
_temp1404)->f3; goto _LL1411;} else{ goto _LL1412;} _LL1412: if(( unsigned int)
_temp1404 >  3u?*(( int*) _temp1404) ==  Cyc_Absyn_AnonUnionType: 0){ _LL1427:
_temp1426=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1404)->f1; goto
_LL1413;} else{ goto _LL1414;} _LL1414: if(( unsigned int) _temp1404 >  3u?*((
int*) _temp1404) ==  Cyc_Absyn_TupleType: 0){ goto _LL1415;} else{ goto _LL1416;}
_LL1416: if(( unsigned int) _temp1404 >  3u?*(( int*) _temp1404) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL1417;} else{ goto _LL1418;} _LL1418: goto _LL1419; _LL1407: if(
_temp1420 ==  0? 1:(* _temp1420)->fields ==  0){( int) _throw(({ void* _temp1428[
0u]={}; Cyc_Toc_toc_impos( _tag_arr("struct fields must be known", sizeof(
unsigned char), 28u), _tag_arr( _temp1428, sizeof( void*), 0u));}));} _temp1422=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp1420)->fields))->v;
goto _LL1409; _LL1409: { struct Cyc_Absyn_Structfield* _temp1429=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1422,( int) _temp1001);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1430=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1430[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1431; _temp1431.tag=
Cyc_Absyn_Offsetof_e; _temp1431.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1003);
_temp1431.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1432=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1432[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1433; _temp1433.tag=
Cyc_Absyn_StructField; _temp1433.f1= _temp1429->name; _temp1433;}); _temp1432;}));
_temp1431;}); _temp1430;}))); goto _LL1405;} _LL1411: if( _temp1424 ==  0? 1:(*
_temp1424)->fields ==  0){( int) _throw(({ void* _temp1434[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("union fields must be known", sizeof( unsigned char), 27u), _tag_arr(
_temp1434, sizeof( void*), 0u));}));} _temp1426=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp1424)->fields))->v; goto _LL1413;
_LL1413: { struct Cyc_Absyn_Structfield* _temp1435=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1426,( int) _temp1001);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1436=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1436[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1437; _temp1437.tag=
Cyc_Absyn_Offsetof_e; _temp1437.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1003);
_temp1437.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1438=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1438[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1439; _temp1439.tag=
Cyc_Absyn_StructField; _temp1439.f1= _temp1435->name; _temp1439;}); _temp1438;}));
_temp1437;}); _temp1436;}))); goto _LL1405;} _LL1415:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1440=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1440[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1441; _temp1441.tag= Cyc_Absyn_Offsetof_e;
_temp1441.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1003); _temp1441.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1442=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1442[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1443; _temp1443.tag= Cyc_Absyn_StructField;
_temp1443.f1= Cyc_Absyn_fieldname(( int)( _temp1001 +  1)); _temp1443;});
_temp1442;})); _temp1441;}); _temp1440;}))); goto _LL1405; _LL1417: if(
_temp1001 ==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1444=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1444[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1445; _temp1445.tag=
Cyc_Absyn_Offsetof_e; _temp1445.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1003);
_temp1445.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1446=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1446[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1447; _temp1447.tag=
Cyc_Absyn_StructField; _temp1447.f1= Cyc_Toc_tag_sp; _temp1447;}); _temp1446;}));
_temp1445;}); _temp1444;})));} else{( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1448=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1448[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1449; _temp1449.tag= Cyc_Absyn_Offsetof_e;
_temp1449.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1003); _temp1449.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1450=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1450[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1451; _temp1451.tag= Cyc_Absyn_StructField;
_temp1451.f1= Cyc_Absyn_fieldname(( int) _temp1001); _temp1451;}); _temp1450;}));
_temp1449;}); _temp1448;})));} goto _LL1405; _LL1419:( int) _throw(({ void*
_temp1452[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("impossible type for offsetof tuple index",
sizeof( unsigned char), 41u), _tag_arr( _temp1452, sizeof( void*), 0u));}));
_LL1405:;} goto _LL830; _LL878: { void* _temp1453= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1005->topt))->v);{ void* _temp1454=
_temp1453; struct Cyc_Absyn_PtrInfo _temp1460; struct Cyc_Absyn_Conref*
_temp1462; struct Cyc_Absyn_Tqual _temp1464; struct Cyc_Absyn_Conref* _temp1466;
void* _temp1468; void* _temp1470; _LL1456: if(( unsigned int) _temp1454 >  3u?*((
int*) _temp1454) ==  Cyc_Absyn_PointerType: 0){ _LL1461: _temp1460=(( struct Cyc_Absyn_PointerType_struct*)
_temp1454)->f1; _LL1471: _temp1470=( void*) _temp1460.elt_typ; goto _LL1469;
_LL1469: _temp1468=( void*) _temp1460.rgn_typ; goto _LL1467; _LL1467: _temp1466=
_temp1460.nullable; goto _LL1465; _LL1465: _temp1464= _temp1460.tq; goto _LL1463;
_LL1463: _temp1462= _temp1460.bounds; goto _LL1457;} else{ goto _LL1458;}
_LL1458: goto _LL1459; _LL1457:{ void* _temp1472= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1462); _LL1474: if(( unsigned int) _temp1472 >  1u?*(( int*) _temp1472) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1475;} else{ goto _LL1476;} _LL1476: if(
_temp1472 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1477;} else{ goto _LL1473;}
_LL1475: { int do_null_check= Cyc_Toc_need_null_check( _temp1005); Cyc_Toc_exp_to_c(
nv, _temp1005); if( do_null_check){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1005->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1478=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1478->hd=( void*) _temp1005; _temp1478->tl= 0;
_temp1478;}), 0), 0), 0))->r));} goto _LL1473;} _LL1477: { struct Cyc_Absyn_Exp*
_temp1479= Cyc_Absyn_uint_exp( 0, 0); _temp1479->topt=({ struct Cyc_Core_Opt*
_temp1480=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1480->v=( void*) Cyc_Absyn_uint_t; _temp1480;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp1005, _temp1479, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1473;} _LL1473:;} goto _LL1455; _LL1459:({ void* _temp1481[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1481, sizeof( void*), 0u));}); goto _LL1455; _LL1455:;} goto _LL830;}
_LL880: Cyc_Toc_exp_to_c( nv, _temp1009); if( Cyc_Toc_is_poly_project( e)){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL830; _LL882: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1013->topt))->v); int
do_null_check= Cyc_Toc_need_null_check( _temp1013); Cyc_Toc_exp_to_c( nv,
_temp1013);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1484; struct Cyc_Absyn_Tqual _temp1486; struct Cyc_Absyn_Conref* _temp1488;
void* _temp1490; void* _temp1492; struct Cyc_Absyn_PtrInfo _temp1482= Cyc_Toc_get_ptr_type(
e1typ); _LL1493: _temp1492=( void*) _temp1482.elt_typ; goto _LL1491; _LL1491:
_temp1490=( void*) _temp1482.rgn_typ; goto _LL1489; _LL1489: _temp1488=
_temp1482.nullable; goto _LL1487; _LL1487: _temp1486= _temp1482.tq; goto _LL1485;
_LL1485: _temp1484= _temp1482.bounds; goto _LL1483; _LL1483:{ void* _temp1494=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1484); struct Cyc_Absyn_Exp*
_temp1500; _LL1496: if(( unsigned int) _temp1494 >  1u?*(( int*) _temp1494) == 
Cyc_Absyn_Upper_b: 0){ _LL1501: _temp1500=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1494)->f1; goto _LL1497;} else{ goto _LL1498;} _LL1498: if( _temp1494 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1499;} else{ goto _LL1495;} _LL1497: if(
Cyc_Evexp_eval_const_uint_exp( _temp1500) <  1){({ void* _temp1502[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1502, sizeof( void*), 0u));});} if( do_null_check){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1013->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1503=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1503->hd=( void*) _temp1013;
_temp1503->tl= 0; _temp1503;}), 0), 0), _temp1011, 0))->r));} goto _LL1495;
_LL1499: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1492);( void*)( _temp1013->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1486), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1504[ 3u];
_temp1504[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1504[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1504[ 0u]= Cyc_Absyn_copy_exp( _temp1013);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1504, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1495;} _LL1495:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL830;}} _LL884: { void* _temp1505= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1017->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1017); Cyc_Toc_exp_to_c( nv, _temp1015);{ void* _temp1506= _temp1505;
struct Cyc_List_List* _temp1514; struct Cyc_Absyn_PtrInfo _temp1516; struct Cyc_Absyn_Conref*
_temp1518; struct Cyc_Absyn_Tqual _temp1520; struct Cyc_Absyn_Conref* _temp1522;
void* _temp1524; void* _temp1526; _LL1508: if(( unsigned int) _temp1506 >  3u?*((
int*) _temp1506) ==  Cyc_Absyn_TupleType: 0){ _LL1515: _temp1514=(( struct Cyc_Absyn_TupleType_struct*)
_temp1506)->f1; goto _LL1509;} else{ goto _LL1510;} _LL1510: if(( unsigned int)
_temp1506 >  3u?*(( int*) _temp1506) ==  Cyc_Absyn_PointerType: 0){ _LL1517:
_temp1516=(( struct Cyc_Absyn_PointerType_struct*) _temp1506)->f1; _LL1527:
_temp1526=( void*) _temp1516.elt_typ; goto _LL1525; _LL1525: _temp1524=( void*)
_temp1516.rgn_typ; goto _LL1523; _LL1523: _temp1522= _temp1516.nullable; goto
_LL1521; _LL1521: _temp1520= _temp1516.tq; goto _LL1519; _LL1519: _temp1518=
_temp1516.bounds; goto _LL1511;} else{ goto _LL1512;} _LL1512: goto _LL1513;
_LL1509: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1015) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1017, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1507;} _LL1511:{ void* _temp1528= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1518); struct Cyc_Absyn_Exp* _temp1534; _LL1530: if((
unsigned int) _temp1528 >  1u?*(( int*) _temp1528) ==  Cyc_Absyn_Upper_b: 0){
_LL1535: _temp1534=(( struct Cyc_Absyn_Upper_b_struct*) _temp1528)->f1; goto
_LL1531;} else{ goto _LL1532;} _LL1532: if( _temp1528 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1533;} else{ goto _LL1529;} _LL1531: _temp1534= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1534), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1522); void* ta1= Cyc_Toc_typ_to_c( _temp1526);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1520); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1536[ 4u];
_temp1536[ 3u]= _temp1015; _temp1536[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1536[ 1u]= _temp1534; _temp1536[ 0u]= _temp1017;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1536, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp1015->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1537[ 2u];
_temp1537[ 1u]= Cyc_Absyn_copy_exp( _temp1015); _temp1537[ 0u]= _temp1534;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1537, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1529;}
_LL1533: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1526);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1520), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1538[ 3u]; _temp1538[ 2u]= _temp1015; _temp1538[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1538[ 0u]= _temp1017;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1538, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1529;} _LL1529:;} goto _LL1507; _LL1513:({
void* _temp1539[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1539, sizeof( void*), 0u));}); goto
_LL1507; _LL1507:;} goto _LL830;} _LL886: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1019))->r));} else{ struct
Cyc_List_List* _temp1540=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp1019); struct _tagged_arr* _temp1541= Cyc_Toc_add_tuple_type( _temp1540);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1019 !=  0;( _temp1019=
_temp1019->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _temp1019->hd);
dles=({ struct Cyc_List_List* _temp1542=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1542->hd=( void*)({ struct _tuple4*
_temp1543=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1543->f1=
0; _temp1543->f2=( struct Cyc_Absyn_Exp*) _temp1019->hd; _temp1543;}); _temp1542->tl=
dles; _temp1542;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));} goto _LL830; _LL888:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1021, 0))->r));{ struct Cyc_List_List* _temp1544= _temp1021; for( 0;
_temp1544 !=  0; _temp1544= _temp1544->tl){ struct _tuple4 _temp1547; struct Cyc_Absyn_Exp*
_temp1548; struct _tuple4* _temp1545=( struct _tuple4*) _temp1544->hd; _temp1547=*
_temp1545; _LL1549: _temp1548= _temp1547.f2; goto _LL1546; _LL1546: Cyc_Toc_exp_to_c(
nv, _temp1548);}} goto _LL830; _LL890: { unsigned int _temp1550= Cyc_Evexp_eval_const_uint_exp(
_temp1025); Cyc_Toc_exp_to_c( nv, _temp1023);{ struct Cyc_List_List* es= 0; if(
! Cyc_Toc_is_zero( _temp1023)){ unsigned int i= 0; for( 0; i <  _temp1550; i ++){
es=({ struct Cyc_List_List* _temp1551=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1551->hd=( void*)({ struct _tuple4*
_temp1552=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1552->f1=
0; _temp1552->f2= _temp1023; _temp1552;}); _temp1551->tl= es; _temp1551;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL830;}} _LL892: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, 0, _temp1031, _temp1035))->r));} else{ if( _temp1029 ==  0){({
void* _temp1553[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer", sizeof(
unsigned char), 37u), _tag_arr( _temp1553, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1029); struct
_RegionHandle _temp1554= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1554; _push_region( rgn);{ struct Cyc_List_List* _temp1555=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1031,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); struct Cyc_List_List* _temp1556= 0;{ struct Cyc_List_List*
_temp1557=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1557 !=  0; _temp1557= _temp1557->tl){ struct Cyc_List_List*
_temp1558= _temp1555; for( 0; _temp1558 !=  0; _temp1558= _temp1558->tl){ if((*((
struct _tuple10*) _temp1558->hd)).f1 == ( struct Cyc_Absyn_Structfield*)
_temp1557->hd){ struct _tuple10 _temp1561; struct Cyc_Absyn_Exp* _temp1562;
struct Cyc_Absyn_Structfield* _temp1564; struct _tuple10* _temp1559=( struct
_tuple10*) _temp1558->hd; _temp1561=* _temp1559; _LL1565: _temp1564= _temp1561.f1;
goto _LL1563; _LL1563: _temp1562= _temp1561.f2; goto _LL1560; _LL1560: { void*
_temp1566=( void*) _temp1564->type; Cyc_Toc_exp_to_c( nv, _temp1562); if( Cyc_Toc_is_void_star(
_temp1566)){( void*)( _temp1562->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1562->r, 0), 0))->r));} _temp1556=({ struct Cyc_List_List*
_temp1567=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1567->hd=( void*)({ struct _tuple4* _temp1568=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1568->f1= 0; _temp1568->f2= _temp1562; _temp1568;});
_temp1567->tl= _temp1556; _temp1567;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1556), 0))->r));}; _pop_region( rgn);}} goto _LL830;
_LL894: { struct Cyc_List_List* fs;{ void* _temp1569= Cyc_Tcutil_compress(
_temp1039); struct Cyc_List_List* _temp1575; _LL1571: if(( unsigned int)
_temp1569 >  3u?*(( int*) _temp1569) ==  Cyc_Absyn_AnonStructType: 0){ _LL1576:
_temp1575=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1569)->f1; goto
_LL1572;} else{ goto _LL1573;} _LL1573: goto _LL1574; _LL1572: fs= _temp1575;
goto _LL1570; _LL1574: fs=({ struct Cyc_Std_String_pa_struct _temp1578;
_temp1578.tag= Cyc_Std_String_pa; _temp1578.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1039);{ void* _temp1577[ 1u]={& _temp1578};(( struct Cyc_List_List*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("anon struct has type %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1577, sizeof( void*), 1u));}});
goto _LL1570; _LL1570:;}{ struct _RegionHandle _temp1579= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1579; _push_region( rgn);{ struct Cyc_List_List*
_temp1580=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1037, fs); for( 0; _temp1580 !=  0; _temp1580= _temp1580->tl){
struct _tuple10 _temp1583; struct Cyc_Absyn_Exp* _temp1584; struct Cyc_Absyn_Structfield*
_temp1586; struct _tuple10* _temp1581=( struct _tuple10*) _temp1580->hd;
_temp1583=* _temp1581; _LL1587: _temp1586= _temp1583.f1; goto _LL1585; _LL1585:
_temp1584= _temp1583.f2; goto _LL1582; _LL1582: { void* _temp1588=( void*)
_temp1586->type; Cyc_Toc_exp_to_c( nv, _temp1584); if( Cyc_Toc_is_void_star(
_temp1588)){( void*)( _temp1584->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1584->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1037, 0))->r));}; _pop_region( rgn);}
goto _LL830;} _LL896: { struct _tuple0* qv= _temp1041->name; if( _temp1043->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1589= _temp1043->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1043->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1589))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1589->hd)->typs ==  0){ tag_count ++;} _temp1589= _temp1589->tl;}( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL830;} _LL898: { struct _tuple0* _temp1590= _temp1047->name; struct Cyc_List_List*
_temp1591= _temp1047->typs; struct _tuple0* _temp1592= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1593= Cyc_Absyn_var_exp( _temp1592, 0); void*
_temp1594= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1590, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1049->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1590, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1595= _temp1049->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1049->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1590,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1595))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1595->hd)->typs !=  0){ tag_count ++;} _temp1595= _temp1595->tl;} tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles=
0; for( 0; _temp1051 !=  0;( _temp1051= _temp1051->tl, _temp1591= _temp1591->tl)){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1051->hd; void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1591))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1596=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1596->hd=( void*)({ struct _tuple4* _temp1597=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1597->f1= 0; _temp1597->f2= cur_e; _temp1597;});
_temp1596->tl= dles; _temp1596;});} dles=({ struct Cyc_List_List* _temp1598=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1598->hd=(
void*)({ struct _tuple4* _temp1599=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1599->f1= 0; _temp1599->f2= tag_exp; _temp1599;}); _temp1598->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1598;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1600=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1600->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1590,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1600;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1601=({ struct Cyc_List_List* _temp1607=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1607->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1593, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1590, 0), 0); _temp1607->tl= 0; _temp1607;});{ int i=
1; for( 0; _temp1051 !=  0;((( _temp1051= _temp1051->tl, i ++)), _temp1591=
_temp1591->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1051->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1591))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1602= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1593, Cyc_Absyn_fieldname(
i), 0), cur_e, 0); _temp1601=({ struct Cyc_List_List* _temp1603=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1603->hd=( void*) _temp1602;
_temp1603->tl= _temp1601; _temp1603;});}}}{ struct Cyc_Absyn_Stmt* _temp1604=
Cyc_Absyn_exp_stmt( _temp1593, 0); struct Cyc_Absyn_Stmt* _temp1605= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1606=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1606->hd=( void*) _temp1604; _temp1606->tl= _temp1601; _temp1606;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1592, _temp1594, 0, _temp1605, 0), 0))->r));}} goto _LL830;} _LL900: goto
_LL830; _LL902: goto _LL830; _LL904: { void* t_c= Cyc_Toc_typ_to_c(*(( void**)
_check_null( _temp1059))); Cyc_Toc_exp_to_c( nv, _temp1057); if( _temp1055){
struct _tuple0* _temp1608= Cyc_Toc_temp_var(); struct _tuple0* _temp1609= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* pexp; struct Cyc_Absyn_Exp* xexp; struct Cyc_Absyn_Exp*
rexp; if( _temp1063){ xexp= _temp1057; if( _temp1061 !=  0){ struct Cyc_Absyn_Exp*
rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1061); Cyc_Toc_exp_to_c( nv, rgn);
pexp= Cyc_Toc_rcalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0), Cyc_Absyn_var_exp(
_temp1608, 0));} else{ pexp= Cyc_Toc_calloc_exp(* _temp1059, Cyc_Absyn_sizeoftyp_exp(
t_c, 0), Cyc_Absyn_var_exp( _temp1608, 0));} rexp= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp* _temp1610[ 3u]; _temp1610[ 2u]= Cyc_Absyn_var_exp(
_temp1608, 0); _temp1610[ 1u]= Cyc_Absyn_sizeoftyp_exp( t_c, 0); _temp1610[ 0u]=
Cyc_Absyn_var_exp( _temp1609, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1610, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0);}
else{ xexp= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( t_c, 0), _temp1057, 0);
if( _temp1061 !=  0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)
_check_null( _temp1061); Cyc_Toc_exp_to_c( nv, rgn); pexp= Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_var_exp( _temp1608, 0));} else{ pexp= Cyc_Toc_malloc_exp(*
_temp1059, Cyc_Absyn_var_exp( _temp1608, 0));} rexp= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp* _temp1611[ 3u]; _temp1611[ 2u]= Cyc_Absyn_var_exp(
_temp1608, 0); _temp1611[ 1u]= Cyc_Absyn_uint_exp( 1, 0); _temp1611[ 0u]= Cyc_Absyn_var_exp(
_temp1609, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1611, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0);}{ struct Cyc_Absyn_Stmt*
_temp1612= Cyc_Absyn_declare_stmt( _temp1608, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
xexp, Cyc_Absyn_declare_stmt( _temp1609, Cyc_Absyn_cstar_typ( t_c, Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*) pexp, Cyc_Absyn_exp_stmt( rexp, 0), 0), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1612, 0))->r));}} else{ if( _temp1061
!=  0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null(
_temp1061); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp(* _temp1059, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}}
goto _LL830;} _LL906: Cyc_Toc_stmt_to_c( nv, _temp1065); goto _LL830; _LL908:({
void* _temp1613[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem", sizeof( unsigned char), 14u),
_tag_arr( _temp1613, sizeof( void*), 0u));}); goto _LL830; _LL910:({ void*
_temp1614[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("compoundlit", sizeof( unsigned char), 12u), _tag_arr( _temp1614,
sizeof( void*), 0u));}); goto _LL830; _LL912:({ void* _temp1615[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen",
sizeof( unsigned char), 8u), _tag_arr( _temp1615, sizeof( void*), 0u));}); goto
_LL830; _LL914:({ void* _temp1616[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("fill", sizeof( unsigned char),
5u), _tag_arr( _temp1616, sizeof( void*), 0u));}); goto _LL830; _LL830:;}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab, struct _tagged_arr*
fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} struct _tuple11{ struct
Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Stmt* f3; } ; struct
_tuple12{ struct _tuple0* f1; void* f2; } ; struct _tuple13{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static struct _tuple11 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env* nv, struct _RegionHandle* rgn, void* t, struct Cyc_Absyn_Exp*
r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat* p, struct _tagged_arr*
succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List* decls){ struct Cyc_Absyn_Stmt*
s;{ void* _temp1617=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp1655; struct
Cyc_Absyn_Vardecl _temp1657; struct _tuple0* _temp1658; struct Cyc_Absyn_Vardecl*
_temp1660; int _temp1662; void* _temp1664; unsigned char _temp1666; struct
_tagged_arr _temp1668; struct Cyc_Absyn_Enumfield* _temp1670; struct Cyc_Absyn_Enumdecl*
_temp1672; struct Cyc_Absyn_Enumfield* _temp1674; void* _temp1676; struct Cyc_List_List*
_temp1678; struct Cyc_List_List* _temp1680; struct Cyc_Absyn_Tunionfield*
_temp1682; struct Cyc_Absyn_Tuniondecl* _temp1684; struct Cyc_Absyn_Pat*
_temp1687; struct Cyc_Absyn_Pat _temp1689; void* _temp1690; struct Cyc_List_List*
_temp1692; struct Cyc_List_List* _temp1694; struct Cyc_Absyn_Tunionfield*
_temp1696; struct Cyc_Absyn_Tuniondecl* _temp1698; struct Cyc_List_List*
_temp1700; struct Cyc_List_List* _temp1702; struct Cyc_List_List* _temp1704;
struct Cyc_List_List* _temp1706; struct Cyc_Core_Opt* _temp1708; struct Cyc_Absyn_Structdecl*
_temp1710; struct Cyc_Absyn_Pat* _temp1712; _LL1619: if(( unsigned int)
_temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_Var_p: 0){ _LL1656: _temp1655=((
struct Cyc_Absyn_Var_p_struct*) _temp1617)->f1; _temp1657=* _temp1655; _LL1659:
_temp1658= _temp1657.name; goto _LL1620;} else{ goto _LL1621;} _LL1621: if(
_temp1617 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1622;} else{ goto _LL1623;}
_LL1623: if(( unsigned int) _temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_Reference_p:
0){ _LL1661: _temp1660=(( struct Cyc_Absyn_Reference_p_struct*) _temp1617)->f1;
goto _LL1624;} else{ goto _LL1625;} _LL1625: if( _temp1617 == ( void*) Cyc_Absyn_Null_p){
goto _LL1626;} else{ goto _LL1627;} _LL1627: if(( unsigned int) _temp1617 >  2u?*((
int*) _temp1617) ==  Cyc_Absyn_Int_p: 0){ _LL1665: _temp1664=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1617)->f1; goto _LL1663; _LL1663: _temp1662=((
struct Cyc_Absyn_Int_p_struct*) _temp1617)->f2; goto _LL1628;} else{ goto
_LL1629;} _LL1629: if(( unsigned int) _temp1617 >  2u?*(( int*) _temp1617) == 
Cyc_Absyn_Char_p: 0){ _LL1667: _temp1666=(( struct Cyc_Absyn_Char_p_struct*)
_temp1617)->f1; goto _LL1630;} else{ goto _LL1631;} _LL1631: if(( unsigned int)
_temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_Float_p: 0){ _LL1669:
_temp1668=(( struct Cyc_Absyn_Float_p_struct*) _temp1617)->f1; goto _LL1632;}
else{ goto _LL1633;} _LL1633: if(( unsigned int) _temp1617 >  2u?*(( int*)
_temp1617) ==  Cyc_Absyn_Enum_p: 0){ _LL1673: _temp1672=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1617)->f1; goto _LL1671; _LL1671: _temp1670=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1617)->f2; goto _LL1634;} else{ goto _LL1635;} _LL1635: if(( unsigned int)
_temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1677:
_temp1676=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1617)->f1; goto
_LL1675; _LL1675: _temp1674=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1617)->f2;
goto _LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int) _temp1617 >  2u?*((
int*) _temp1617) ==  Cyc_Absyn_Tunion_p: 0){ _LL1685: _temp1684=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1617)->f1; goto _LL1683; _LL1683: _temp1682=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1617)->f2; goto _LL1681; _LL1681: _temp1680=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1617)->f3; goto _LL1679; _LL1679: _temp1678=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1617)->f4; if( _temp1678 ==  0){ goto _LL1638;} else{ goto _LL1639;}} else{
goto _LL1639;} _LL1639: if(( unsigned int) _temp1617 >  2u?*(( int*) _temp1617)
==  Cyc_Absyn_Pointer_p: 0){ _LL1688: _temp1687=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1617)->f1; _temp1689=* _temp1687; _LL1691: _temp1690=( void*) _temp1689.r;
if(( unsigned int) _temp1690 >  2u?*(( int*) _temp1690) ==  Cyc_Absyn_Tunion_p:
0){ _LL1699: _temp1698=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1690)->f1;
goto _LL1697; _LL1697: _temp1696=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1690)->f2;
goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1690)->f3;
goto _LL1693; _LL1693: _temp1692=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1690)->f4;
goto _LL1686;} else{ goto _LL1641;}} else{ goto _LL1641;} _LL1686: if( _temp1692
!=  0){ goto _LL1640;} else{ goto _LL1641;} _LL1641: if(( unsigned int)
_temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_Tunion_p: 0){ _LL1701:
_temp1700=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1617)->f4; goto _LL1642;}
else{ goto _LL1643;} _LL1643: if(( unsigned int) _temp1617 >  2u?*(( int*)
_temp1617) ==  Cyc_Absyn_Tuple_p: 0){ _LL1703: _temp1702=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1617)->f1; goto _LL1644;} else{ goto _LL1645;} _LL1645: if(( unsigned int)
_temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_Struct_p: 0){ _LL1711:
_temp1710=(( struct Cyc_Absyn_Struct_p_struct*) _temp1617)->f1; goto _LL1709;
_LL1709: _temp1708=(( struct Cyc_Absyn_Struct_p_struct*) _temp1617)->f2; goto
_LL1707; _LL1707: _temp1706=(( struct Cyc_Absyn_Struct_p_struct*) _temp1617)->f3;
goto _LL1705; _LL1705: _temp1704=(( struct Cyc_Absyn_Struct_p_struct*) _temp1617)->f4;
goto _LL1646;} else{ goto _LL1647;} _LL1647: if(( unsigned int) _temp1617 >  2u?*((
int*) _temp1617) ==  Cyc_Absyn_Pointer_p: 0){ _LL1713: _temp1712=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1617)->f1; goto _LL1648;} else{ goto _LL1649;} _LL1649: if(( unsigned int)
_temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1650;}
else{ goto _LL1651;} _LL1651: if(( unsigned int) _temp1617 >  2u?*(( int*)
_temp1617) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1652;} else{ goto _LL1653;}
_LL1653: if(( unsigned int) _temp1617 >  2u?*(( int*) _temp1617) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1654;} else{ goto _LL1618;} _LL1620: nv= Cyc_Toc_add_varmap( nv,
_temp1658, r); goto _LL1622; _LL1622: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1618; _LL1624: { struct _tuple0* _temp1714= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1715=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1715->hd=( void*)({ struct _tuple12*
_temp1716=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1716->f1= _temp1714; _temp1716->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1716;}); _temp1715->tl= decls; _temp1715;}); nv= Cyc_Toc_add_varmap(
nv, _temp1660->name, Cyc_Absyn_var_exp( _temp1714, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1714, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1618;} _LL1626: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1618; _LL1628:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1664, _temp1662, 0),
succ_lab, fail_lab); goto _LL1618; _LL1630: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1666, 0), succ_lab, fail_lab); goto _LL1618; _LL1632: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1668, 0), succ_lab, fail_lab); goto _LL1618;
_LL1634: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1717=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1717[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1718; _temp1718.tag= Cyc_Absyn_Enum_e;
_temp1718.f1= _temp1670->name; _temp1718.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1672; _temp1718.f3=( struct Cyc_Absyn_Enumfield*) _temp1670; _temp1718;});
_temp1717;}), 0), succ_lab, fail_lab); goto _LL1618; _LL1636: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_AnonEnum_e_struct* _temp1719=(
struct Cyc_Absyn_AnonEnum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_e_struct));
_temp1719[ 0]=({ struct Cyc_Absyn_AnonEnum_e_struct _temp1720; _temp1720.tag=
Cyc_Absyn_AnonEnum_e; _temp1720.f1= _temp1674->name; _temp1720.f2=( void*)
_temp1676; _temp1720.f3=( struct Cyc_Absyn_Enumfield*) _temp1674; _temp1720;});
_temp1719;}), 0), succ_lab, fail_lab); goto _LL1618; _LL1638: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1682->name, 0); if( ! _temp1684->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1618;} _LL1640: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1692);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1696->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1721=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1696->typs); struct Cyc_List_List*
_temp1722=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1692); for( 0; _temp1722 !=  0;((( _temp1722=
_temp1722->tl, _temp1721=(( struct Cyc_List_List*) _check_null( _temp1721))->tl)),
cnt --)){ struct Cyc_Absyn_Pat* _temp1723=( struct Cyc_Absyn_Pat*) _temp1722->hd;
if(( void*) _temp1723->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1724=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1721))->hd)).f2;
struct _tuple0* _temp1725= Cyc_Toc_temp_var(); void* _temp1726=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1723->topt))->v; void* _temp1727= Cyc_Toc_typ_to_c(
_temp1726); struct _tagged_arr* _temp1728= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1729= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1724))){ _temp1729= Cyc_Absyn_cast_exp(
_temp1727, _temp1729, 0);} decls=({ struct Cyc_List_List* _temp1730=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1730->hd=( void*)({
struct _tuple12* _temp1731=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1731->f1= _temp1725; _temp1731->f2= _temp1727; _temp1731;});
_temp1730->tl= decls; _temp1730;});{ struct _tuple11 _temp1732= Cyc_Toc_xlate_pat(
nv, rgn, _temp1726, Cyc_Absyn_var_exp( _temp1725, 0), _temp1729, _temp1723,
succ_lab, fail_lab, decls); nv= _temp1732.f1; decls= _temp1732.f2;{ struct Cyc_Absyn_Stmt*
_temp1733= _temp1732.f3; struct Cyc_Absyn_Stmt* _temp1734= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1725, 0), _temp1729, 0); struct Cyc_Absyn_Stmt*
_temp1735= Cyc_Absyn_seq_stmt( _temp1734, _temp1733, 0); ss=({ struct Cyc_List_List*
_temp1736=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1736->hd=( void*) Cyc_Absyn_label_stmt( _temp1728, _temp1735, 0); _temp1736->tl=
ss; _temp1736;}); succ_lab= _temp1728;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1737=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1737->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1737->tl= 0;
_temp1737;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1698->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1696->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1738=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1698->fields))->v; for( 0; _temp1738 !=  0; _temp1738=
_temp1738->tl){ struct Cyc_Absyn_Tunionfield* _temp1739=( struct Cyc_Absyn_Tunionfield*)
_temp1738->hd; if( _temp1739->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1696->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1618;}} _LL1642: _temp1702= _temp1700; goto _LL1644; _LL1644: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1702);{ struct Cyc_List_List* _temp1740=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1702); for(
0; _temp1740 !=  0;( _temp1740= _temp1740->tl, cnt --)){ struct Cyc_Absyn_Pat*
_temp1741=( struct Cyc_Absyn_Pat*) _temp1740->hd; if(( void*) _temp1741->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tuple0* _temp1742= Cyc_Toc_temp_var();
void* _temp1743=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1741->topt))->v;
struct _tagged_arr* _temp1744= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1745=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1745->hd=( void*)({ struct _tuple12* _temp1746=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1746->f1= _temp1742;
_temp1746->f2= Cyc_Toc_typ_to_c( _temp1743); _temp1746;}); _temp1745->tl= decls;
_temp1745;});{ struct _tuple11 _temp1747= Cyc_Toc_xlate_pat( nv, rgn, _temp1743,
Cyc_Absyn_var_exp( _temp1742, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1741, succ_lab, fail_lab, decls); nv= _temp1747.f1; decls=
_temp1747.f2;{ struct Cyc_Absyn_Stmt* _temp1748= _temp1747.f3; struct Cyc_Absyn_Stmt*
_temp1749= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1742, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp1748, 0); ss=({ struct Cyc_List_List* _temp1750=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1750->hd=( void*) Cyc_Absyn_label_stmt(
_temp1744, _temp1749, 0); _temp1750->tl= ss; _temp1750;}); succ_lab= _temp1744;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1751=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1751->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1751->tl= 0; _temp1751;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1618;} _LL1646: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1752=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1704); for( 0; _temp1752 !=  0; _temp1752=
_temp1752->tl){ struct _tuple13* _temp1753=( struct _tuple13*) _temp1752->hd;
struct Cyc_Absyn_Pat* _temp1754=(* _temp1753).f2; if(( void*) _temp1754->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1755=(
void*)(( struct Cyc_List_List*) _check_null((* _temp1753).f1))->hd; struct
_tagged_arr* _temp1761; _LL1757: if(*(( int*) _temp1755) ==  Cyc_Absyn_FieldName){
_LL1762: _temp1761=(( struct Cyc_Absyn_FieldName_struct*) _temp1755)->f1; goto
_LL1758;} else{ goto _LL1759;} _LL1759: goto _LL1760; _LL1758: f= _temp1761;
goto _LL1756; _LL1760:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1756:;}{
struct _tuple0* _temp1763= Cyc_Toc_temp_var(); void* _temp1764=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1754->topt))->v; void* _temp1765= Cyc_Toc_typ_to_c(
_temp1764); struct _tagged_arr* _temp1766= Cyc_Toc_fresh_label(); decls=({
struct Cyc_List_List* _temp1767=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1767->hd=( void*)({ struct _tuple12*
_temp1768=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1768->f1= _temp1763; _temp1768->f2= _temp1765; _temp1768;}); _temp1767->tl=
decls; _temp1767;});{ struct _tuple11 _temp1769= Cyc_Toc_xlate_pat( nv, rgn,
_temp1764, Cyc_Absyn_var_exp( _temp1763, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp1754, succ_lab, fail_lab, decls); nv= _temp1769.f1; decls= _temp1769.f2;{
struct Cyc_Absyn_Exp* _temp1770= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1710->fields))->v,
f)))->type)){ _temp1770= Cyc_Absyn_cast_exp( _temp1765, _temp1770, 0);}{ struct
Cyc_Absyn_Stmt* _temp1771= _temp1769.f3; struct Cyc_Absyn_Stmt* _temp1772= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1763, 0), _temp1770, 0),
_temp1771, 0); ss=({ struct Cyc_List_List* _temp1773=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1773->hd=( void*) Cyc_Absyn_label_stmt(
_temp1766, _temp1772, 0); _temp1773->tl= ss; _temp1773;}); succ_lab= _temp1766;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1774=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1774->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1774->tl= 0; _temp1774;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1618;} _LL1648: { struct _tuple0* _temp1775= Cyc_Toc_temp_var(); void*
_temp1776=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1712->topt))->v;
decls=({ struct Cyc_List_List* _temp1777=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1777->hd=( void*)({ struct _tuple12*
_temp1778=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1778->f1= _temp1775; _temp1778->f2= Cyc_Toc_typ_to_c( _temp1776); _temp1778;});
_temp1777->tl= decls; _temp1777;});{ struct _tuple11 _temp1779= Cyc_Toc_xlate_pat(
nv, rgn, _temp1776, Cyc_Absyn_var_exp( _temp1775, 0), Cyc_Absyn_deref_exp( path,
0), _temp1712, succ_lab, fail_lab, decls); nv= _temp1779.f1; decls= _temp1779.f2;{
struct Cyc_Absyn_Stmt* _temp1780= _temp1779.f3; struct Cyc_Absyn_Stmt* _temp1781=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1775, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1780, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1781, 0);} else{ s= _temp1781;} goto _LL1618;}}} _LL1650: s=({
void* _temp1782[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1782, sizeof( void*), 0u));}); goto _LL1618;
_LL1652: s=({ void* _temp1783[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1783, sizeof( void*), 0u));}); goto
_LL1618; _LL1654: s=({ void* _temp1784[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1784, sizeof( void*), 0u));}); goto
_LL1618; _LL1618:;} return({ struct _tuple11 _temp1785; _temp1785.f1= nv;
_temp1785.f2= decls; _temp1785.f3= s; _temp1785;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1786=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1786->f1= Cyc_Toc_fresh_label(); _temp1786->f2=
Cyc_Toc_fresh_label(); _temp1786->f3= sc; _temp1786;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1787=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1788= Cyc_Tcutil_compress( _temp1787); _LL1790: if(( unsigned int)
_temp1788 >  3u?*(( int*) _temp1788) ==  Cyc_Absyn_IntType: 0){ goto _LL1791;}
else{ goto _LL1792;} _LL1792: if(( unsigned int) _temp1788 >  3u?*(( int*)
_temp1788) ==  Cyc_Absyn_EnumType: 0){ goto _LL1793;} else{ goto _LL1794;}
_LL1794: goto _LL1795; _LL1791: goto _LL1793; _LL1793: leave_as_switch= 1; goto
_LL1789; _LL1795: leave_as_switch= 0; goto _LL1789; _LL1789:;}{ struct Cyc_List_List*
_temp1796= scs; for( 0; _temp1796 !=  0; _temp1796= _temp1796->tl){ if(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)
_temp1796->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)
_temp1796->hd)->where_clause !=  0){ leave_as_switch= 0; break;}}} if(
leave_as_switch){ struct _tagged_arr* next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List*
_temp1797= scs; for( 0; _temp1797 !=  0; _temp1797= _temp1797->tl){ struct Cyc_Absyn_Stmt*
_temp1798=(( struct Cyc_Absyn_Switch_clause*) _temp1797->hd)->body;(( struct Cyc_Absyn_Switch_clause*)
_temp1797->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp1798, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp1798);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct _tagged_arr*
end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp1799= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1799; _push_region( rgn);{ struct Cyc_List_List*
lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple14*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*), struct _RegionHandle*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label, rgn,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1800= lscs; for( 0;
_temp1800 !=  0; _temp1800= _temp1800->tl){ struct Cyc_Absyn_Switch_clause* sc=(*((
struct _tuple14*) _temp1800->hd)).f3; struct _tagged_arr* fail_lab= _temp1800->tl
==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(
_temp1800->tl))->hd)).f1; struct _tagged_arr* succ_lab=(*(( struct _tuple14*)
_temp1800->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11 _temp1801= Cyc_Toc_xlate_pat(
nv, rgn, _temp1787, r, path, sc->pattern, succ_lab, fail_lab, decls); nvs=({
struct Cyc_List_List* _temp1802=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1802->hd=( void*) _temp1801.f1; _temp1802->tl=
nvs; _temp1802;}); decls= _temp1801.f2; test_stmts=({ struct Cyc_List_List*
_temp1803=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1803->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple14*) _temp1800->hd)).f1,
_temp1801.f3, 0); _temp1803->tl= test_stmts; _temp1803;});} else{ struct Cyc_Absyn_Exp*
_temp1804=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1805= Cyc_Toc_fresh_label(); struct _tuple11 _temp1806= Cyc_Toc_xlate_pat(
nv, rgn, _temp1787, r, path, sc->pattern, _temp1805, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1806.f1, _temp1804); nvs=({ struct Cyc_List_List* _temp1807=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1807->hd=( void*)
_temp1806.f1; _temp1807->tl= nvs; _temp1807;}); decls= _temp1806.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1804, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1805, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1808=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1808->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*) _temp1800->hd)).f1, Cyc_Absyn_seq_stmt( _temp1806.f3, s3, 0),
0); _temp1808->tl= test_stmts; _temp1808;});}}}}{ struct Cyc_Absyn_Stmt*
_temp1809= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs !=  0){ for( 0; lscs->tl !=  0;( lscs= lscs->tl, nvs= nvs->tl)){
struct Cyc_Toc_Env* _temp1810=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1810, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f2,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f3)->pat_vars))->v,( struct
Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null( nvs->tl))->hd), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}{ struct Cyc_Toc_Env* _temp1811=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp1811, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt( _temp1809, Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for(
decls; decls !=  0; decls= decls->tl){ struct _tuple12 _temp1814; void*
_temp1815; struct _tuple0* _temp1817; struct _tuple12* _temp1812=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp1814=*
_temp1812; _LL1818: _temp1817= _temp1814.f1; goto _LL1816; _LL1816: _temp1815=
_temp1814.f2; goto _LL1813; _LL1813: res= Cyc_Absyn_declare_stmt( _temp1817,
_temp1815, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1819=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1819[ 0]=({ struct Cyc_List_List
_temp1820; _temp1820.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1820.tl= 0; _temp1820;}); _temp1819;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1821=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1865; struct Cyc_Absyn_Stmt* _temp1867; struct Cyc_Absyn_Stmt* _temp1869;
struct Cyc_Absyn_Exp* _temp1871; struct Cyc_Absyn_Stmt* _temp1873; struct Cyc_Absyn_Stmt*
_temp1875; struct Cyc_Absyn_Exp* _temp1877; struct Cyc_Absyn_Stmt* _temp1879;
struct _tuple2 _temp1881; struct Cyc_Absyn_Exp* _temp1883; struct Cyc_Absyn_Stmt*
_temp1885; struct Cyc_Absyn_Stmt* _temp1887; struct Cyc_Absyn_Stmt* _temp1889;
struct Cyc_Absyn_Stmt* _temp1891; struct _tuple2 _temp1893; struct Cyc_Absyn_Exp*
_temp1895; struct _tuple2 _temp1897; struct Cyc_Absyn_Exp* _temp1899; struct Cyc_Absyn_Exp*
_temp1901; struct Cyc_List_List* _temp1903; struct Cyc_Absyn_Exp* _temp1905;
struct Cyc_Absyn_Switch_clause** _temp1907; struct Cyc_List_List* _temp1909;
struct Cyc_Absyn_Stmt* _temp1911; struct Cyc_Absyn_Decl* _temp1913; struct Cyc_Absyn_Stmt*
_temp1915; struct _tagged_arr* _temp1917; struct _tuple2 _temp1919; struct Cyc_Absyn_Exp*
_temp1921; struct Cyc_Absyn_Stmt* _temp1923; struct Cyc_List_List* _temp1925;
struct Cyc_Absyn_Stmt* _temp1927; struct Cyc_Absyn_Stmt* _temp1929; struct Cyc_Absyn_Vardecl*
_temp1931; struct Cyc_Absyn_Tvar* _temp1933; _LL1823: if( _temp1821 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1824;} else{ goto _LL1825;} _LL1825: if((
unsigned int) _temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Exp_s: 0){
_LL1866: _temp1865=(( struct Cyc_Absyn_Exp_s_struct*) _temp1821)->f1; goto
_LL1826;} else{ goto _LL1827;} _LL1827: if(( unsigned int) _temp1821 >  1u?*((
int*) _temp1821) ==  Cyc_Absyn_Seq_s: 0){ _LL1870: _temp1869=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1821)->f1; goto _LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1821)->f2; goto _LL1828;} else{ goto _LL1829;} _LL1829: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Return_s: 0){ _LL1872:
_temp1871=(( struct Cyc_Absyn_Return_s_struct*) _temp1821)->f1; goto _LL1830;}
else{ goto _LL1831;} _LL1831: if(( unsigned int) _temp1821 >  1u?*(( int*)
_temp1821) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1878: _temp1877=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1821)->f1; goto _LL1876; _LL1876: _temp1875=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1821)->f2; goto _LL1874; _LL1874: _temp1873=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1821)->f3; goto _LL1832;} else{ goto _LL1833;} _LL1833: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_While_s: 0){ _LL1882:
_temp1881=(( struct Cyc_Absyn_While_s_struct*) _temp1821)->f1; _LL1884:
_temp1883= _temp1881.f1; goto _LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_While_s_struct*)
_temp1821)->f2; goto _LL1834;} else{ goto _LL1835;} _LL1835: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Break_s: 0){ _LL1886:
_temp1885=(( struct Cyc_Absyn_Break_s_struct*) _temp1821)->f1; goto _LL1836;}
else{ goto _LL1837;} _LL1837: if(( unsigned int) _temp1821 >  1u?*(( int*)
_temp1821) ==  Cyc_Absyn_Continue_s: 0){ _LL1888: _temp1887=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1821)->f1; goto _LL1838;} else{ goto _LL1839;} _LL1839: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Goto_s: 0){ _LL1890:
_temp1889=(( struct Cyc_Absyn_Goto_s_struct*) _temp1821)->f2; goto _LL1840;}
else{ goto _LL1841;} _LL1841: if(( unsigned int) _temp1821 >  1u?*(( int*)
_temp1821) ==  Cyc_Absyn_For_s: 0){ _LL1902: _temp1901=(( struct Cyc_Absyn_For_s_struct*)
_temp1821)->f1; goto _LL1898; _LL1898: _temp1897=(( struct Cyc_Absyn_For_s_struct*)
_temp1821)->f2; _LL1900: _temp1899= _temp1897.f1; goto _LL1894; _LL1894:
_temp1893=(( struct Cyc_Absyn_For_s_struct*) _temp1821)->f3; _LL1896: _temp1895=
_temp1893.f1; goto _LL1892; _LL1892: _temp1891=(( struct Cyc_Absyn_For_s_struct*)
_temp1821)->f4; goto _LL1842;} else{ goto _LL1843;} _LL1843: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_ForArray_s: 0){ goto _LL1844;}
else{ goto _LL1845;} _LL1845: if(( unsigned int) _temp1821 >  1u?*(( int*)
_temp1821) ==  Cyc_Absyn_Switch_s: 0){ _LL1906: _temp1905=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1821)->f1; goto _LL1904; _LL1904: _temp1903=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1821)->f2; goto _LL1846;} else{ goto _LL1847;} _LL1847: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1910:
_temp1909=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1821)->f1; goto _LL1908;
_LL1908: _temp1907=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1821)->f2; goto
_LL1848;} else{ goto _LL1849;} _LL1849: if(( unsigned int) _temp1821 >  1u?*((
int*) _temp1821) ==  Cyc_Absyn_Decl_s: 0){ _LL1914: _temp1913=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1821)->f1; goto _LL1912; _LL1912: _temp1911=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1821)->f2; goto _LL1850;} else{ goto _LL1851;} _LL1851: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Label_s: 0){ _LL1918:
_temp1917=(( struct Cyc_Absyn_Label_s_struct*) _temp1821)->f1; goto _LL1916;
_LL1916: _temp1915=(( struct Cyc_Absyn_Label_s_struct*) _temp1821)->f2; goto
_LL1852;} else{ goto _LL1853;} _LL1853: if(( unsigned int) _temp1821 >  1u?*((
int*) _temp1821) ==  Cyc_Absyn_Do_s: 0){ _LL1924: _temp1923=(( struct Cyc_Absyn_Do_s_struct*)
_temp1821)->f1; goto _LL1920; _LL1920: _temp1919=(( struct Cyc_Absyn_Do_s_struct*)
_temp1821)->f2; _LL1922: _temp1921= _temp1919.f1; goto _LL1854;} else{ goto
_LL1855;} _LL1855: if(( unsigned int) _temp1821 >  1u?*(( int*) _temp1821) == 
Cyc_Absyn_TryCatch_s: 0){ _LL1928: _temp1927=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1821)->f1; goto _LL1926; _LL1926: _temp1925=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1821)->f2; goto _LL1856;} else{ goto _LL1857;} _LL1857: if(( unsigned int)
_temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Region_s: 0){ _LL1934:
_temp1933=(( struct Cyc_Absyn_Region_s_struct*) _temp1821)->f1; goto _LL1932;
_LL1932: _temp1931=(( struct Cyc_Absyn_Region_s_struct*) _temp1821)->f2; goto
_LL1930; _LL1930: _temp1929=(( struct Cyc_Absyn_Region_s_struct*) _temp1821)->f3;
goto _LL1858;} else{ goto _LL1859;} _LL1859: if(( unsigned int) _temp1821 >  1u?*((
int*) _temp1821) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1860;} else{ goto _LL1861;}
_LL1861: if(( unsigned int) _temp1821 >  1u?*(( int*) _temp1821) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1862;} else{ goto _LL1863;} _LL1863: if(( unsigned int) _temp1821 > 
1u?*(( int*) _temp1821) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1864;} else{ goto
_LL1822;} _LL1824: return; _LL1826: Cyc_Toc_exp_to_c( nv, _temp1865); return;
_LL1828: Cyc_Toc_stmt_to_c( nv, _temp1869); s= _temp1867; continue; _LL1830: {
struct Cyc_Core_Opt* topt= 0; if( _temp1871 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1935=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1935->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1871->topt))->v); _temp1935;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp1871));} if( s->try_depth >  0){ if( topt != 
0){ struct _tuple0* _temp1936= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1937= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp(
_temp1936, 0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp1936,( void*) topt->v, _temp1871, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1937, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1832: Cyc_Toc_exp_to_c( nv, _temp1877); Cyc_Toc_stmt_to_c( nv,
_temp1875); s= _temp1873; continue; _LL1834: Cyc_Toc_exp_to_c( nv, _temp1883);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1879); return; _LL1836: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1885 ==  0? 0: _temp1885->try_depth; Cyc_Toc_do_npop_before(
s->try_depth -  dest_depth, s); return;} _LL1838: if( nv->continue_lab !=  0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp1889=
_temp1887; goto _LL1840; _LL1840: Cyc_Toc_do_npop_before( s->try_depth - ((
struct Cyc_Absyn_Stmt*) _check_null( _temp1889))->try_depth, s); return; _LL1842:
Cyc_Toc_exp_to_c( nv, _temp1901); Cyc_Toc_exp_to_c( nv, _temp1899); Cyc_Toc_exp_to_c(
nv, _temp1895); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1891); return;
_LL1844:({ void* _temp1938[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("forarray not yet implemented",
sizeof( unsigned char), 29u), _tag_arr( _temp1938, sizeof( void*), 0u));});
return; _LL1846: Cyc_Toc_xlate_switch( nv, s, _temp1905, _temp1903); return;
_LL1848: if( nv->fallthru_info ==  0){( int) _throw(({ void* _temp1939[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place", sizeof(
unsigned char), 29u), _tag_arr( _temp1939, sizeof( void*), 0u));}));}{ struct
_tuple6 _temp1942; struct Cyc_Dict_Dict* _temp1943; struct Cyc_List_List*
_temp1945; struct _tagged_arr* _temp1947; struct _tuple6* _temp1940=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp1942=*
_temp1940; _LL1948: _temp1947= _temp1942.f1; goto _LL1946; _LL1946: _temp1945=
_temp1942.f2; goto _LL1944; _LL1944: _temp1943= _temp1942.f3; goto _LL1941;
_LL1941: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1947, 0); Cyc_Toc_do_npop_before(
s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp1907)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp1949=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1945); struct Cyc_List_List* _temp1950=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1909); for( 0; _temp1949 !=  0;(
_temp1949= _temp1949->tl, _temp1950= _temp1950->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1950))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp1943,(
struct _tuple0*) _temp1949->hd),( struct Cyc_Absyn_Exp*) _temp1950->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1850:{ void*
_temp1951=( void*) _temp1913->r; struct Cyc_Absyn_Vardecl* _temp1961; int
_temp1963; struct Cyc_Absyn_Exp* _temp1965; struct Cyc_Core_Opt* _temp1967;
struct Cyc_Absyn_Pat* _temp1969; struct Cyc_List_List* _temp1971; _LL1953: if(*((
int*) _temp1951) ==  Cyc_Absyn_Var_d){ _LL1962: _temp1961=(( struct Cyc_Absyn_Var_d_struct*)
_temp1951)->f1; goto _LL1954;} else{ goto _LL1955;} _LL1955: if(*(( int*)
_temp1951) ==  Cyc_Absyn_Let_d){ _LL1970: _temp1969=(( struct Cyc_Absyn_Let_d_struct*)
_temp1951)->f1; goto _LL1968; _LL1968: _temp1967=(( struct Cyc_Absyn_Let_d_struct*)
_temp1951)->f3; goto _LL1966; _LL1966: _temp1965=(( struct Cyc_Absyn_Let_d_struct*)
_temp1951)->f4; goto _LL1964; _LL1964: _temp1963=(( struct Cyc_Absyn_Let_d_struct*)
_temp1951)->f5; goto _LL1956;} else{ goto _LL1957;} _LL1957: if(*(( int*)
_temp1951) ==  Cyc_Absyn_Letv_d){ _LL1972: _temp1971=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1951)->f1; goto _LL1958;} else{ goto _LL1959;} _LL1959: goto _LL1960;
_LL1954: { struct Cyc_Toc_Env* _temp1973= Cyc_Toc_add_varmap( nv, _temp1961->name,
Cyc_Absyn_varb_exp( _temp1961->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1974=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1974[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1975; _temp1975.tag= Cyc_Absyn_Local_b;
_temp1975.f1= _temp1961; _temp1975;}); _temp1974;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1973, _temp1973, _temp1961, _temp1911); goto _LL1952;} _LL1956:{ void*
_temp1976=( void*) _temp1969->r; struct Cyc_Absyn_Vardecl* _temp1982; _LL1978:
if(( unsigned int) _temp1976 >  2u?*(( int*) _temp1976) ==  Cyc_Absyn_Var_p: 0){
_LL1983: _temp1982=(( struct Cyc_Absyn_Var_p_struct*) _temp1976)->f1; goto
_LL1979;} else{ goto _LL1980;} _LL1980: goto _LL1981; _LL1979: { struct _tuple0*
old_name= _temp1982->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1982->name= new_name; _temp1982->initializer=( struct Cyc_Absyn_Exp*)
_temp1965;( void*)( _temp1913->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1984=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1984[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1985; _temp1985.tag= Cyc_Absyn_Var_d;
_temp1985.f1= _temp1982; _temp1985;}); _temp1984;})));{ struct Cyc_Toc_Env*
_temp1986= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1987=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1987[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1988; _temp1988.tag= Cyc_Absyn_Local_b; _temp1988.f1=
_temp1982; _temp1988;}); _temp1987;}), 0)); Cyc_Toc_local_decl_to_c( _temp1986,
nv, _temp1982, _temp1911); goto _LL1977;}} _LL1981:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1969, _temp1967, _temp1965, _temp1963,
_temp1911))->r)); goto _LL1977; _LL1977:;} goto _LL1952; _LL1958: { struct Cyc_List_List*
_temp1989=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1971); if( _temp1989 ==  0){({ void* _temp1990[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1990, sizeof( void*), 0u));});}(
void*)( _temp1913->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1991=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1991[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1992; _temp1992.tag= Cyc_Absyn_Var_d;
_temp1992.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1989))->hd; _temp1992;}); _temp1991;}))); _temp1989= _temp1989->tl; for( 0;
_temp1989 !=  0; _temp1989= _temp1989->tl){ struct Cyc_Absyn_Decl* _temp1993=
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp1994=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1994[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1995; _temp1995.tag= Cyc_Absyn_Var_d;
_temp1995.f1=( struct Cyc_Absyn_Vardecl*) _temp1989->hd; _temp1995;}); _temp1994;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1993, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1952;} _LL1960:({
void* _temp1996[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1996, sizeof( void*), 0u));}); goto _LL1952;
_LL1952:;} return; _LL1852: s= _temp1915; continue; _LL1854: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1923); Cyc_Toc_exp_to_c( nv, _temp1921); return;
_LL1856: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1997=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1997->v=( void*) e_typ; _temp1997;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1927);{
struct Cyc_Absyn_Stmt* _temp1998= Cyc_Absyn_seq_stmt( _temp1927, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1999= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2000= Cyc_Absyn_var_exp(
_temp1999, 0); struct Cyc_Absyn_Vardecl* _temp2001= Cyc_Absyn_new_vardecl(
_temp1999, Cyc_Absyn_exn_typ, 0); _temp2000->topt=({ struct Cyc_Core_Opt*
_temp2002=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2002->v=( void*) Cyc_Absyn_exn_typ; _temp2002;});{ struct Cyc_Absyn_Pat*
_temp2003=({ struct Cyc_Absyn_Pat* _temp2019=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2019->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2021=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2021[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2022; _temp2022.tag= Cyc_Absyn_Var_p;
_temp2022.f1= _temp2001; _temp2022;}); _temp2021;})); _temp2019->topt=({ struct
Cyc_Core_Opt* _temp2020=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2020->v=( void*) Cyc_Absyn_exn_typ; _temp2020;}); _temp2019->loc= 0;
_temp2019;}); struct Cyc_Absyn_Exp* _temp2004= Cyc_Absyn_throw_exp( _temp2000, 0);
_temp2004->topt=({ struct Cyc_Core_Opt* _temp2005=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2005->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2005;});{ struct Cyc_Absyn_Stmt* _temp2006= Cyc_Absyn_exp_stmt( _temp2004,
0); struct Cyc_Absyn_Switch_clause* _temp2007=({ struct Cyc_Absyn_Switch_clause*
_temp2016=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2016->pattern= _temp2003; _temp2016->pat_vars=({ struct Cyc_Core_Opt*
_temp2017=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2017->v=( void*)({ struct Cyc_List_List* _temp2018=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2018->hd=( void*) _temp2001;
_temp2018->tl= 0; _temp2018;}); _temp2017;}); _temp2016->where_clause= 0;
_temp2016->body= _temp2006; _temp2016->loc= 0; _temp2016;}); struct Cyc_Absyn_Stmt*
_temp2008= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1925,({ struct Cyc_List_List*
_temp2015=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2015->hd=( void*) _temp2007; _temp2015->tl= 0; _temp2015;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2008);{ struct Cyc_Absyn_Exp* _temp2009= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2014=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2014->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2014->tl= 0; _temp2014;}), 0); struct Cyc_Absyn_Stmt*
_temp2010= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2013=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2013->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2013->tl= 0; _temp2013;}), 0), 0); struct Cyc_Absyn_Exp* _temp2011= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2012= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2010, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2011, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2009, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2012, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1998, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2008, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1858: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1931->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1929);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,({
struct Cyc_List_List* _temp2023=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2023->hd=( void*) Cyc_Absyn_string_exp( Cyc_Absynpp_qvar2string(
x_var), 0); _temp2023->tl= 0; _temp2023;}), 0), Cyc_Absyn_declare_stmt( x_var,
rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( rh_exp, 0),
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp* _temp2024[ 1u]; _temp2024[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp2024, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), Cyc_Absyn_seq_stmt( _temp1929, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({ struct Cyc_Absyn_Exp* _temp2025[ 1u]; _temp2025[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp2025, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL1860:({ void* _temp2026[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s", sizeof(
unsigned char), 10u), _tag_arr( _temp2026, sizeof( void*), 0u));}); return;
_LL1862:({ void* _temp2027[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u),
_tag_arr( _temp2027, sizeof( void*), 0u));}); return; _LL1864:({ void* _temp2028[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("splice", sizeof( unsigned char), 7u), _tag_arr( _temp2028, sizeof(
void*), 0u));}); return; _LL1822:;}} struct _tuple15{ struct _tagged_arr* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2029= f->args; for( 0; _temp2029 !=  0; _temp2029=
_temp2029->tl){(*(( struct _tuple15*) _temp2029->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple15*) _temp2029->hd)).f3);{ struct _tuple0* _temp2030=({ struct
_tuple0* _temp2031=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2031->f1=( void*) Cyc_Absyn_Loc_n; _temp2031->f2=(*(( struct _tuple15*)
_temp2029->hd)).f1; _temp2031;}); nv= Cyc_Toc_add_varmap( nv, _temp2030, Cyc_Absyn_var_exp(
_temp2030, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp2034; void* _temp2036;
struct Cyc_Absyn_Tqual _temp2038; struct Cyc_Core_Opt* _temp2040; struct Cyc_Absyn_VarargInfo
_temp2032=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL2041: _temp2040= _temp2032.name; goto _LL2039; _LL2039: _temp2038= _temp2032.tq;
goto _LL2037; _LL2037: _temp2036=( void*) _temp2032.type; goto _LL2035; _LL2035:
_temp2034= _temp2032.inject; goto _LL2033; _LL2033: { void* _temp2042= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp2036,( void*) Cyc_Absyn_HeapRgn, _temp2038)); struct
_tuple0* _temp2043=({ struct _tuple0* _temp2046=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp2046->f1=( void*) Cyc_Absyn_Loc_n; _temp2046->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2040))->v;
_temp2046;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2044=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2044->hd=( void*)({ struct _tuple15* _temp2045=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp2045->f1=( struct _tagged_arr*)
_temp2040->v; _temp2045->f2= _temp2038; _temp2045->f3= _temp2042; _temp2045;});
_temp2044->tl= 0; _temp2044;})); nv= Cyc_Toc_add_varmap( nv, _temp2043, Cyc_Absyn_var_exp(
_temp2043, 0)); f->cyc_varargs= 0;}}{ struct Cyc_List_List* _temp2047=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( f->param_vardecls))->v;
for( 0; _temp2047 !=  0; _temp2047= _temp2047->tl){( void*)((( struct Cyc_Absyn_Vardecl*)
_temp2047->hd)->type=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)
_temp2047->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2048= s; _LL2050: if(
_temp2048 == ( void*) Cyc_Absyn_Abstract){ goto _LL2051;} else{ goto _LL2052;}
_LL2052: if( _temp2048 == ( void*) Cyc_Absyn_ExternC){ goto _LL2053;} else{ goto
_LL2054;} _LL2054: goto _LL2055; _LL2051: return( void*) Cyc_Absyn_Public;
_LL2053: return( void*) Cyc_Absyn_Extern; _LL2055: return s; _LL2049:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2056=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2057=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2057->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2057;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2058=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2056).f2); if( _temp2058 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2056).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*) _temp2058->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2056).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(( void*) Cyc_Absyn_Public));
s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2059=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2059->v=( void*) _temp2056;
_temp2059;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields= 0;} else{
struct Cyc_List_List* _temp2060=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2060 !=  0; _temp2060= _temp2060->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2060->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2060->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2061=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({
struct Cyc_Core_Opt* _temp2062=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2062->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2062;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2063=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2061).f2); if( _temp2063 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2061).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*) _temp2063->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2061).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(( void*) Cyc_Absyn_Public));
u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2064=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2064->v=( void*) _temp2061;
_temp2064;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields= 0;} else{
struct Cyc_List_List* _temp2065=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2065 !=  0; _temp2065= _temp2065->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2065->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2065->hd)->type));}}}}} static
struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far= 0; static void Cyc_Toc_tuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl* tud){ if( Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt* _temp2066=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2066->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)(
struct _tuple0*, struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp);
_temp2066;});}{ struct _tuple0* _temp2067= tud->name; if( tud->fields ==  0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2067)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2067));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2068=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; for( 0; _temp2068 !=  0; _temp2068=
_temp2068->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)
_temp2068->hd; if( f->typs ==  0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2069;
_temp2069.q_const= 1; _temp2069.q_volatile= 0; _temp2069.q_restrict= 0;
_temp2069;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2070=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2070->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2071=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2071[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2072; _temp2072.tag= Cyc_Absyn_Var_d;
_temp2072.f1= vd; _temp2072;}); _temp2071;}), 0); _temp2070->tl= Cyc_Toc_result_decls;
_temp2070;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2073; _temp2073.q_const=
1; _temp2073.q_volatile= 0; _temp2073.q_restrict= 0; _temp2073;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2074=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2074->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2075=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2075[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2076; _temp2076.tag= Cyc_Absyn_Var_d; _temp2076.f1= vd; _temp2076;});
_temp2075;}), 0); _temp2074->tl= Cyc_Toc_result_decls; _temp2074;});{ struct Cyc_List_List*
_temp2077= 0; int i= 1;{ struct Cyc_List_List* _temp2078= f->typs; for( 0;
_temp2078 !=  0;( _temp2078= _temp2078->tl, i ++)){ struct _tagged_arr*
_temp2079= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield* _temp2080=({
struct Cyc_Absyn_Structfield* _temp2082=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2082->name= _temp2079;
_temp2082->tq=(*(( struct _tuple3*) _temp2078->hd)).f1; _temp2082->type=( void*)
Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2078->hd)).f2); _temp2082->width=
0; _temp2082->attributes= 0; _temp2082;}); _temp2077=({ struct Cyc_List_List*
_temp2081=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2081->hd=( void*) _temp2080; _temp2081->tl= _temp2077; _temp2081;});}}
_temp2077=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2077); _temp2077=({ struct Cyc_List_List* _temp2083=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2083->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2084=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2084->name= Cyc_Toc_tag_sp; _temp2084->tq= Cyc_Toc_mt_tq; _temp2084->type=(
void*) Cyc_Absyn_sint_t; _temp2084->width= 0; _temp2084->attributes= 0;
_temp2084;}); _temp2083->tl= _temp2077; _temp2083;});{ struct Cyc_Absyn_Structdecl*
_temp2085=({ struct Cyc_Absyn_Structdecl* _temp2089=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2089->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2089->name=({ struct Cyc_Core_Opt* _temp2091=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2091->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2091;}); _temp2089->tvs= 0; _temp2089->fields=({ struct Cyc_Core_Opt*
_temp2090=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2090->v=( void*) _temp2077; _temp2090;}); _temp2089->attributes= 0;
_temp2089;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2086=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2086->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2087=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2087[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2088; _temp2088.tag= Cyc_Absyn_Struct_d;
_temp2088.f1= _temp2085; _temp2088;}); _temp2087;}), 0); _temp2086->tl= Cyc_Toc_result_decls;
_temp2086;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp2092=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2092->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2092;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp2093= xd->name; struct Cyc_List_List*
_temp2094=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2094 !=  0; _temp2094= _temp2094->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*) _temp2094->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2095= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2096=({ struct
Cyc_Absyn_ArrayType_struct* _temp2146=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2146[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2147; _temp2147.tag= Cyc_Absyn_ArrayType;
_temp2147.f1=( void*) Cyc_Absyn_uchar_t; _temp2147.f2= Cyc_Toc_mt_tq; _temp2147.f3=(
struct Cyc_Absyn_Exp*) _temp2095; _temp2147;}); _temp2146;}); struct Cyc_Core_Opt*
_temp2097=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2105; int _temp2106; _LL2099: if( _temp2097 ==  0){ goto _LL2100;} else{
goto _LL2101;} _LL2101: if( _temp2097 ==  0){ goto _LL2103;} else{ _temp2105=*
_temp2097; _LL2107: _temp2106=( int) _temp2105.v; if( _temp2106 ==  0){ goto
_LL2102;} else{ goto _LL2103;}} _LL2103: goto _LL2104; _LL2100: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2113; _temp2113.tag= Cyc_Std_String_pa; _temp2113.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp2112; _temp2112.tag= Cyc_Std_Int_pa;
_temp2112.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2111; _temp2111.tag= Cyc_Std_Int_pa; _temp2111.f1=( int)(( unsigned int)((
int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2110; _temp2110.tag= Cyc_Std_Int_pa;
_temp2110.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2109; _temp2109.tag= Cyc_Std_Int_pa; _temp2109.f1=( int)(( unsigned int)((
int)'\000'));{ void* _temp2108[ 5u]={& _temp2109,& _temp2110,& _temp2111,&
_temp2112,& _temp2113}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof(
unsigned char), 11u), _tag_arr( _temp2108, sizeof( void*), 5u));}}}}}}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2114= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2096, initopt);( void*)( _temp2114->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2115=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2115->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2116=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2116[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2117; _temp2117.tag= Cyc_Absyn_Var_d; _temp2117.f1= _temp2114; _temp2117;});
_temp2116;}), 0); _temp2115->tl= Cyc_Toc_result_decls; _temp2115;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2118= f->typs; for( 0; _temp2118 !=  0;( _temp2118= _temp2118->tl, i ++)){
struct _tagged_arr* _temp2119=({ struct _tagged_arr* _temp2123=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2123[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2125; _temp2125.tag= Cyc_Std_Int_pa;
_temp2125.f1=( int)(( unsigned int) i);{ void* _temp2124[ 1u]={& _temp2125}; Cyc_Std_aprintf(
_tag_arr("f%d", sizeof( unsigned char), 4u), _tag_arr( _temp2124, sizeof( void*),
1u));}}); _temp2123;}); struct Cyc_Absyn_Structfield* _temp2120=({ struct Cyc_Absyn_Structfield*
_temp2122=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2122->name= _temp2119; _temp2122->tq=(*(( struct _tuple3*) _temp2118->hd)).f1;
_temp2122->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2118->hd)).f2);
_temp2122->width= 0; _temp2122->attributes= 0; _temp2122;}); fields=({ struct
Cyc_List_List* _temp2121=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2121->hd=( void*) _temp2120; _temp2121->tl= fields; _temp2121;});}} fields=({
struct Cyc_List_List* _temp2126=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2126->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2127=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2127->name= Cyc_Toc_tag_sp; _temp2127->tq= Cyc_Toc_mt_tq; _temp2127->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2127->width=
0; _temp2127->attributes= 0; _temp2127;}); _temp2126->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2126;});{ struct Cyc_Absyn_Structdecl*
_temp2128=({ struct Cyc_Absyn_Structdecl* _temp2132=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2132->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2132->name=({ struct Cyc_Core_Opt* _temp2134=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2134->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2134;}); _temp2132->tvs= 0; _temp2132->fields=({ struct Cyc_Core_Opt*
_temp2133=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2133->v=( void*) fields; _temp2133;}); _temp2132->attributes= 0; _temp2132;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2129=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2129->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2130=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2130[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2131; _temp2131.tag= Cyc_Absyn_Struct_d;
_temp2131.f1= _temp2128; _temp2131;}); _temp2130;}), 0); _temp2129->tl= Cyc_Toc_result_decls;
_temp2129;});}} goto _LL2098;}} _LL2102: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2135= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2145; _temp2145.tag= Cyc_Std_String_pa;
_temp2145.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2144;
_temp2144.tag= Cyc_Std_Int_pa; _temp2144.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2143; _temp2143.tag= Cyc_Std_Int_pa; _temp2143.f1=(
int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2142;
_temp2142.tag= Cyc_Std_Int_pa; _temp2142.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2141; _temp2141.tag= Cyc_Std_Int_pa; _temp2141.f1=(
int)(( unsigned int)(( int)'\000'));{ void* _temp2140[ 5u]={& _temp2141,&
_temp2142,& _temp2143,& _temp2144,& _temp2145}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s",
sizeof( unsigned char), 11u), _tag_arr( _temp2140, sizeof( void*), 5u));}}}}}}),
0); struct Cyc_Absyn_Vardecl* _temp2136= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2096,( struct Cyc_Absyn_Exp*) _temp2135);( void*)( _temp2136->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2137=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2137->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2138=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2138[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2139; _temp2139.tag= Cyc_Absyn_Var_d;
_temp2139.f1= _temp2136; _temp2139;}); _temp2138;}), 0); _temp2137->tl= Cyc_Toc_result_decls;
_temp2137;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k,
int v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2098;
_LL2104: goto _LL2098; _LL2098:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2148= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v);}}
static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s){( void*)( vd->type=(
void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c( body_nv, s);
if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2149=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2155; struct Cyc_Absyn_Exp* _temp2157; struct Cyc_Absyn_Vardecl* _temp2159;
_LL2151: if(*(( int*) _temp2149) ==  Cyc_Absyn_Comprehension_e){ _LL2160:
_temp2159=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2149)->f1; goto
_LL2158; _LL2158: _temp2157=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2149)->f2; goto _LL2156; _LL2156: _temp2155=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2149)->f3; goto _LL2152;} else{ goto _LL2153;} _LL2153: goto _LL2154;
_LL2152: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2159, _temp2157, _temp2155, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2150; _LL2154: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2161= Cyc_Toc_copy_env( init_nv);
_temp2161->toplevel= 1; Cyc_Toc_exp_to_c( _temp2161, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2150; _LL2150:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2162= _new_region("prgn"); struct _RegionHandle* prgn=&
_temp2162; _push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2165; struct Cyc_List_List*
_temp2167; struct Cyc_Toc_Env* _temp2169; struct _tuple11 _temp2163= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2170: _temp2169= _temp2163.f1; goto _LL2168; _LL2168: _temp2167=
_temp2163.f2; goto _LL2166; _LL2166: _temp2165= _temp2163.f3; goto _LL2164;
_LL2164: Cyc_Toc_stmt_to_c( _temp2169, s);{ struct Cyc_Absyn_Stmt* _temp2171=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2165, _temp2171, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2172= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2173= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2172, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2165,
Cyc_Absyn_seq_stmt( _temp2173, _temp2171, 0), 0), 0);} for( 0; _temp2167 !=  0;
_temp2167= _temp2167->tl){ struct _tuple12 _temp2176; void* _temp2177; struct
_tuple0* _temp2179; struct _tuple12* _temp2174=( struct _tuple12*) _temp2167->hd;
_temp2176=* _temp2174; _LL2180: _temp2179= _temp2176.f1; goto _LL2178; _LL2178:
_temp2177= _temp2176.f2; goto _LL2175; _LL2175: s= Cyc_Absyn_declare_stmt(
_temp2179, _temp2177, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds= ds->tl){ if( ! nv->toplevel){({ void*
_temp2181[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("decls_to_c: not at toplevel!", sizeof( unsigned char), 29u), _tag_arr(
_temp2181, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
ds->hd; void* _temp2182=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp2208;
struct Cyc_Absyn_Fndecl* _temp2210; struct Cyc_Absyn_Structdecl* _temp2212;
struct Cyc_Absyn_Uniondecl* _temp2214; struct Cyc_Absyn_Tuniondecl* _temp2216;
struct Cyc_Absyn_Enumdecl* _temp2218; struct Cyc_Absyn_Typedefdecl* _temp2220;
struct Cyc_List_List* _temp2222; struct Cyc_List_List* _temp2224; struct Cyc_List_List*
_temp2226; _LL2184: if(*(( int*) _temp2182) ==  Cyc_Absyn_Var_d){ _LL2209:
_temp2208=(( struct Cyc_Absyn_Var_d_struct*) _temp2182)->f1; goto _LL2185;}
else{ goto _LL2186;} _LL2186: if(*(( int*) _temp2182) ==  Cyc_Absyn_Fn_d){
_LL2211: _temp2210=(( struct Cyc_Absyn_Fn_d_struct*) _temp2182)->f1; goto
_LL2187;} else{ goto _LL2188;} _LL2188: if(*(( int*) _temp2182) ==  Cyc_Absyn_Let_d){
goto _LL2189;} else{ goto _LL2190;} _LL2190: if(*(( int*) _temp2182) ==  Cyc_Absyn_Letv_d){
goto _LL2191;} else{ goto _LL2192;} _LL2192: if(*(( int*) _temp2182) ==  Cyc_Absyn_Struct_d){
_LL2213: _temp2212=(( struct Cyc_Absyn_Struct_d_struct*) _temp2182)->f1; goto
_LL2193;} else{ goto _LL2194;} _LL2194: if(*(( int*) _temp2182) ==  Cyc_Absyn_Union_d){
_LL2215: _temp2214=(( struct Cyc_Absyn_Union_d_struct*) _temp2182)->f1; goto
_LL2195;} else{ goto _LL2196;} _LL2196: if(*(( int*) _temp2182) ==  Cyc_Absyn_Tunion_d){
_LL2217: _temp2216=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2182)->f1; goto
_LL2197;} else{ goto _LL2198;} _LL2198: if(*(( int*) _temp2182) ==  Cyc_Absyn_Enum_d){
_LL2219: _temp2218=(( struct Cyc_Absyn_Enum_d_struct*) _temp2182)->f1; goto
_LL2199;} else{ goto _LL2200;} _LL2200: if(*(( int*) _temp2182) ==  Cyc_Absyn_Typedef_d){
_LL2221: _temp2220=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2182)->f1; goto
_LL2201;} else{ goto _LL2202;} _LL2202: if(*(( int*) _temp2182) ==  Cyc_Absyn_Namespace_d){
_LL2223: _temp2222=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2182)->f2; goto
_LL2203;} else{ goto _LL2204;} _LL2204: if(*(( int*) _temp2182) ==  Cyc_Absyn_Using_d){
_LL2225: _temp2224=(( struct Cyc_Absyn_Using_d_struct*) _temp2182)->f2; goto
_LL2205;} else{ goto _LL2206;} _LL2206: if(*(( int*) _temp2182) ==  Cyc_Absyn_ExternC_d){
_LL2227: _temp2226=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2182)->f1; goto
_LL2207;} else{ goto _LL2183;} _LL2185: { struct _tuple0* _temp2228= _temp2208->name;
if(( void*) _temp2208->sc == ( void*) Cyc_Absyn_ExternC){ _temp2228=({ struct
_tuple0* _temp2229=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2229->f1= Cyc_Absyn_rel_ns_null; _temp2229->f2=(* _temp2228).f2; _temp2229;});}
if( _temp2208->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2208->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2208->name,
Cyc_Absyn_varb_exp( _temp2228,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2230=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2230[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2231; _temp2231.tag= Cyc_Absyn_Global_b;
_temp2231.f1= _temp2208; _temp2231;}); _temp2230;}), 0)); _temp2208->name=
_temp2228;( void*)( _temp2208->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2208->sc));(
void*)( _temp2208->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2208->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2232=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2232->hd=( void*) d; _temp2232->tl=
Cyc_Toc_result_decls; _temp2232;}); goto _LL2183;} _LL2187: { struct _tuple0*
_temp2233= _temp2210->name; if(( void*) _temp2210->sc == ( void*) Cyc_Absyn_ExternC){
_temp2233=({ struct _tuple0* _temp2234=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2234->f1= Cyc_Absyn_rel_ns_null; _temp2234->f2=(*
_temp2233).f2; _temp2234;});} nv= Cyc_Toc_add_varmap( nv, _temp2210->name, Cyc_Absyn_var_exp(
_temp2233, 0)); _temp2210->name= _temp2233; Cyc_Toc_fndecl_to_c( nv, _temp2210);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2235=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2235->hd=( void*) d; _temp2235->tl=
Cyc_Toc_result_decls; _temp2235;}); goto _LL2183;} _LL2189: goto _LL2191;
_LL2191:({ void* _temp2236[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2236, sizeof( void*), 0u));}); goto _LL2183;
_LL2193: Cyc_Toc_structdecl_to_c( _temp2212); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2237=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2237->hd=( void*) d; _temp2237->tl= Cyc_Toc_result_decls; _temp2237;});
goto _LL2183; _LL2195: Cyc_Toc_uniondecl_to_c( _temp2214); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2238=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2238->hd=( void*) d; _temp2238->tl= Cyc_Toc_result_decls;
_temp2238;}); goto _LL2183; _LL2197: if( _temp2216->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2216);} else{ Cyc_Toc_tuniondecl_to_c( _temp2216);} goto _LL2183; _LL2199:
Cyc_Toc_enumdecl_to_c( nv, _temp2218); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2239=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2239->hd=( void*) d; _temp2239->tl= Cyc_Toc_result_decls; _temp2239;});
goto _LL2183; _LL2201: _temp2220->name= _temp2220->name; _temp2220->tvs= 0;(
void*)( _temp2220->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2220->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2240=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2240->hd=( void*) d; _temp2240->tl=
Cyc_Toc_result_decls; _temp2240;}); goto _LL2183; _LL2203: _temp2224= _temp2222;
goto _LL2205; _LL2205: _temp2226= _temp2224; goto _LL2207; _LL2207: nv= Cyc_Toc_decls_to_c(
nv, _temp2226, top); goto _LL2183; _LL2183:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
