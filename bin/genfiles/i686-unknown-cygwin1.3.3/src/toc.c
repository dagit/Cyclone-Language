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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern
int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
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
struct Cyc_List_List* attributes; int escapes; } ; struct Cyc_Absyn_Fndecl{ void*
sc; int is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
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
Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_EqualConst=
0; struct Cyc_CfFlowInfo_EqualConst_struct{ int tag; unsigned int f1; } ; static
const int Cyc_CfFlowInfo_LessVar= 1; struct Cyc_CfFlowInfo_LessVar_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_LessSize=
2; struct Cyc_CfFlowInfo_LessSize_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_LessConst= 3; struct Cyc_CfFlowInfo_LessConst_struct{
int tag; unsigned int f1; } ; static const int Cyc_CfFlowInfo_LessEqSize= 4;
struct Cyc_CfFlowInfo_LessEqSize_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; struct Cyc_CfFlowInfo_Reln{ struct Cyc_Absyn_Vardecl* vd; void* rop; } ;
extern unsigned char Cyc_CfFlowInfo_IsZero[ 11u]; struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_CfFlowInfo_NotZero[
12u]; struct Cyc_CfFlowInfo_NotZero_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_CfFlowInfo_UnknownZ[ 13u]; struct Cyc_CfFlowInfo_UnknownZ_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; static const int Cyc_CfFlowInfo_PlaceL=
0; struct Cyc_CfFlowInfo_PlaceL_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1;
} ; static const int Cyc_CfFlowInfo_UnknownL= 0; static const int Cyc_CfFlowInfo_Zero=
0; static const int Cyc_CfFlowInfo_NotZeroAll= 1; static const int Cyc_CfFlowInfo_NotZeroThis=
2; static const int Cyc_CfFlowInfo_UnknownR= 0; struct Cyc_CfFlowInfo_UnknownR_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
struct Cyc_List_List* f2; } ; extern struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds); extern struct _tuple0* Cyc_Toc_temp_var(); unsigned int Cyc_Toc_total_bounds_checks=
0; unsigned int Cyc_Toc_bounds_checks_eliminated= 0; static struct Cyc_List_List*
Cyc_Toc_result_decls= 0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0;
static int Cyc_Toc_tuple_type_counter= 0; static int Cyc_Toc_temp_var_counter= 0;
static int Cyc_Toc_fresh_label_counter= 0; unsigned char Cyc_Toc_Toc_Unimplemented[
22u]="\000\000\000\000Toc_Unimplemented"; unsigned char Cyc_Toc_Toc_Impossible[
19u]="\000\000\000\000Toc_Impossible"; static void* Cyc_Toc_unimp( struct
_tagged_arr fmt, struct _tagged_arr ap){ Cyc_Std_vfprintf( Cyc_Std_stderr, fmt,
ap);({ void* _temp0[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n",
sizeof( unsigned char), 2u), _tag_arr( _temp0, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Unimplemented);}
static void* Cyc_Toc_toc_impos( struct _tagged_arr fmt, struct _tagged_arr ap){
Cyc_Std_vfprintf( Cyc_Std_stderr, fmt, ap);({ void* _temp1[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp1,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);(
int) _throw(( void*) Cyc_Toc_Toc_Impossible);} unsigned char Cyc_Toc_Match_error[
16u]="\000\000\000\000Match_error"; static unsigned char _temp2[ 5u]="curr";
static struct _tagged_arr Cyc_Toc_curr_string={ _temp2, _temp2, _temp2 +  5u};
static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string; static
unsigned char _temp3[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
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
e){ void* _temp560=( void*) e->annot; _LL562: if(*(( void**) _temp560) ==  Cyc_CfFlowInfo_UnknownZ){
goto _LL563;} else{ goto _LL564;} _LL564: if(*(( void**) _temp560) ==  Cyc_CfFlowInfo_NotZero){
goto _LL565;} else{ goto _LL566;} _LL566: if(*(( void**) _temp560) ==  Cyc_CfFlowInfo_IsZero){
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
_temp575;})); _LL561:;} static struct Cyc_List_List* Cyc_Toc_get_relns( struct
Cyc_Absyn_Exp* e){ void* _temp577=( void*) e->annot; struct Cyc_List_List*
_temp589; struct Cyc_List_List* _temp591; _LL579: if(*(( void**) _temp577) == 
Cyc_CfFlowInfo_UnknownZ){ _LL590: _temp589=(( struct Cyc_CfFlowInfo_UnknownZ_struct*)
_temp577)->f1; goto _LL580;} else{ goto _LL581;} _LL581: if(*(( void**) _temp577)
==  Cyc_CfFlowInfo_NotZero){ _LL592: _temp591=(( struct Cyc_CfFlowInfo_NotZero_struct*)
_temp577)->f1; goto _LL582;} else{ goto _LL583;} _LL583: if(*(( void**) _temp577)
==  Cyc_CfFlowInfo_IsZero){ goto _LL584;} else{ goto _LL585;} _LL585: if(
_temp577 ==  Cyc_Absyn_EmptyAnnot){ goto _LL586;} else{ goto _LL587;} _LL587:
goto _LL588; _LL580: return _temp589; _LL582: return _temp591; _LL584:({ void*
_temp593[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("dereference of NULL pointer",
sizeof( unsigned char), 28u), _tag_arr( _temp593, sizeof( void*), 0u));});
return 0; _LL586: return 0; _LL588:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp594=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp594[ 0]=({ struct Cyc_Core_Impossible_struct _temp595; _temp595.tag= Cyc_Core_Impossible;
_temp595.f1= _tag_arr("get_relns", sizeof( unsigned char), 10u); _temp595;});
_temp594;})); _LL578:;} static int Cyc_Toc_check_const_array( unsigned int i,
void* t){ void* _temp596= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp604; struct Cyc_Absyn_Conref* _temp606; struct Cyc_Absyn_Exp* _temp608;
_LL598: if(( unsigned int) _temp596 >  3u?*(( int*) _temp596) ==  Cyc_Absyn_PointerType:
0){ _LL605: _temp604=(( struct Cyc_Absyn_PointerType_struct*) _temp596)->f1;
_LL607: _temp606= _temp604.bounds; goto _LL599;} else{ goto _LL600;} _LL600: if((
unsigned int) _temp596 >  3u?*(( int*) _temp596) ==  Cyc_Absyn_ArrayType: 0){
_LL609: _temp608=(( struct Cyc_Absyn_ArrayType_struct*) _temp596)->f3; goto
_LL601;} else{ goto _LL602;} _LL602: goto _LL603; _LL599: { void* _temp610= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp606); struct Cyc_Absyn_Exp* _temp616; _LL612: if(
_temp610 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL613;} else{ goto _LL614;}
_LL614: if(( unsigned int) _temp610 >  1u?*(( int*) _temp610) ==  Cyc_Absyn_Upper_b:
0){ _LL617: _temp616=(( struct Cyc_Absyn_Upper_b_struct*) _temp610)->f1; goto
_LL615;} else{ goto _LL611;} _LL613: return 0; _LL615: { unsigned int j= Cyc_Evexp_eval_const_uint_exp(
_temp616); return i <=  j;} _LL611:;} _LL601: if( _temp608 ==  0){ return 0;}{
unsigned int j= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp608)); return i <=  j;} _LL603: return 0; _LL597:;} static int
Cyc_Toc_check_leq_size_var( struct Cyc_List_List* relns, struct Cyc_Absyn_Vardecl*
v, struct Cyc_Absyn_Vardecl* y){ for( 0; relns !=  0; relns= relns->tl){ struct
Cyc_CfFlowInfo_Reln* _temp618=( struct Cyc_CfFlowInfo_Reln*) relns->hd; if(
_temp618->vd !=  y){ continue;}{ void* _temp619=( void*) _temp618->rop; struct
Cyc_Absyn_Vardecl* _temp627; struct Cyc_Absyn_Vardecl* _temp629; _LL621: if(*((
int*) _temp619) ==  Cyc_CfFlowInfo_LessSize){ _LL628: _temp627=(( struct Cyc_CfFlowInfo_LessSize_struct*)
_temp619)->f1; goto _LL622;} else{ goto _LL623;} _LL623: if(*(( int*) _temp619)
==  Cyc_CfFlowInfo_LessEqSize){ _LL630: _temp629=(( struct Cyc_CfFlowInfo_LessEqSize_struct*)
_temp619)->f1; goto _LL624;} else{ goto _LL625;} _LL625: goto _LL626; _LL622:
_temp629= _temp627; goto _LL624; _LL624: if( _temp629 ==  v){ return 1;} else{
goto _LL620;} _LL626: continue; _LL620:;}} return 0;} static int Cyc_Toc_check_leq_size(
struct Cyc_List_List* relns, struct Cyc_Absyn_Vardecl* v, struct Cyc_Absyn_Exp*
e){{ void* _temp631=( void*) e->r; void* _temp645; struct Cyc_Absyn_Vardecl*
_temp647; void* _temp649; struct Cyc_Absyn_Vardecl* _temp651; void* _temp653;
struct Cyc_Absyn_Vardecl* _temp655; void* _temp657; struct Cyc_Absyn_Vardecl*
_temp659; struct Cyc_List_List* _temp661; struct Cyc_List_List _temp663; struct
Cyc_Absyn_Exp* _temp664; void* _temp666; _LL633: if(*(( int*) _temp631) ==  Cyc_Absyn_Var_e){
_LL646: _temp645=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp631)->f2; if((
unsigned int) _temp645 >  1u?*(( int*) _temp645) ==  Cyc_Absyn_Pat_b: 0){ _LL648:
_temp647=(( struct Cyc_Absyn_Pat_b_struct*) _temp645)->f1; goto _LL634;} else{
goto _LL635;}} else{ goto _LL635;} _LL635: if(*(( int*) _temp631) ==  Cyc_Absyn_Var_e){
_LL650: _temp649=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp631)->f2; if((
unsigned int) _temp649 >  1u?*(( int*) _temp649) ==  Cyc_Absyn_Local_b: 0){
_LL652: _temp651=(( struct Cyc_Absyn_Local_b_struct*) _temp649)->f1; goto _LL636;}
else{ goto _LL637;}} else{ goto _LL637;} _LL637: if(*(( int*) _temp631) ==  Cyc_Absyn_Var_e){
_LL654: _temp653=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp631)->f2; if((
unsigned int) _temp653 >  1u?*(( int*) _temp653) ==  Cyc_Absyn_Global_b: 0){
_LL656: _temp655=(( struct Cyc_Absyn_Global_b_struct*) _temp653)->f1; goto
_LL638;} else{ goto _LL639;}} else{ goto _LL639;} _LL639: if(*(( int*) _temp631)
==  Cyc_Absyn_Var_e){ _LL658: _temp657=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp631)->f2; if(( unsigned int) _temp657 >  1u?*(( int*) _temp657) ==  Cyc_Absyn_Param_b:
0){ _LL660: _temp659=(( struct Cyc_Absyn_Param_b_struct*) _temp657)->f1; goto
_LL640;} else{ goto _LL641;}} else{ goto _LL641;} _LL641: if(*(( int*) _temp631)
==  Cyc_Absyn_Primop_e){ _LL667: _temp666=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp631)->f1; if( _temp666 == ( void*) Cyc_Absyn_Size){ goto _LL662;} else{
goto _LL643;} _LL662: _temp661=(( struct Cyc_Absyn_Primop_e_struct*) _temp631)->f2;
if( _temp661 ==  0){ goto _LL643;} else{ _temp663=* _temp661; _LL665: _temp664=(
struct Cyc_Absyn_Exp*) _temp663.hd; goto _LL642;}} else{ goto _LL643;} _LL643:
goto _LL644; _LL634: _temp651= _temp647; goto _LL636; _LL636: _temp655= _temp651;
goto _LL638; _LL638: _temp659= _temp655; goto _LL640; _LL640: if( _temp659->escapes){
return 0;} if( Cyc_Toc_check_leq_size_var( relns, v, _temp659)){ return 1;} goto
_LL632; _LL642:{ void* _temp668=( void*) _temp664->r; void* _temp680; struct Cyc_Absyn_Vardecl*
_temp682; void* _temp684; struct Cyc_Absyn_Vardecl* _temp686; void* _temp688;
struct Cyc_Absyn_Vardecl* _temp690; void* _temp692; struct Cyc_Absyn_Vardecl*
_temp694; _LL670: if(*(( int*) _temp668) ==  Cyc_Absyn_Var_e){ _LL681: _temp680=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp668)->f2; if(( unsigned int)
_temp680 >  1u?*(( int*) _temp680) ==  Cyc_Absyn_Pat_b: 0){ _LL683: _temp682=((
struct Cyc_Absyn_Pat_b_struct*) _temp680)->f1; goto _LL671;} else{ goto _LL672;}}
else{ goto _LL672;} _LL672: if(*(( int*) _temp668) ==  Cyc_Absyn_Var_e){ _LL685:
_temp684=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp668)->f2; if((
unsigned int) _temp684 >  1u?*(( int*) _temp684) ==  Cyc_Absyn_Local_b: 0){
_LL687: _temp686=(( struct Cyc_Absyn_Local_b_struct*) _temp684)->f1; goto _LL673;}
else{ goto _LL674;}} else{ goto _LL674;} _LL674: if(*(( int*) _temp668) ==  Cyc_Absyn_Var_e){
_LL689: _temp688=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp668)->f2; if((
unsigned int) _temp688 >  1u?*(( int*) _temp688) ==  Cyc_Absyn_Global_b: 0){
_LL691: _temp690=(( struct Cyc_Absyn_Global_b_struct*) _temp688)->f1; goto
_LL675;} else{ goto _LL676;}} else{ goto _LL676;} _LL676: if(*(( int*) _temp668)
==  Cyc_Absyn_Var_e){ _LL693: _temp692=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp668)->f2; if(( unsigned int) _temp692 >  1u?*(( int*) _temp692) ==  Cyc_Absyn_Param_b:
0){ _LL695: _temp694=(( struct Cyc_Absyn_Param_b_struct*) _temp692)->f1; goto
_LL677;} else{ goto _LL678;}} else{ goto _LL678;} _LL678: goto _LL679; _LL671:
_temp686= _temp682; goto _LL673; _LL673: _temp690= _temp686; goto _LL675; _LL675:
_temp694= _temp690; goto _LL677; _LL677: return _temp694 ==  v; _LL679: goto
_LL669; _LL669:;} goto _LL632; _LL644: goto _LL632; _LL632:;} return 0;} static
int Cyc_Toc_check_bounds( struct Cyc_List_List* relns, struct Cyc_Absyn_Exp* a,
struct Cyc_Absyn_Exp* i){{ void* _temp696=( void*) a->r; void* _temp708; struct
Cyc_Absyn_Vardecl* _temp710; void* _temp712; struct Cyc_Absyn_Vardecl* _temp714;
void* _temp716; struct Cyc_Absyn_Vardecl* _temp718; void* _temp720; struct Cyc_Absyn_Vardecl*
_temp722; _LL698: if(*(( int*) _temp696) ==  Cyc_Absyn_Var_e){ _LL709: _temp708=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp696)->f2; if(( unsigned int)
_temp708 >  1u?*(( int*) _temp708) ==  Cyc_Absyn_Pat_b: 0){ _LL711: _temp710=((
struct Cyc_Absyn_Pat_b_struct*) _temp708)->f1; goto _LL699;} else{ goto _LL700;}}
else{ goto _LL700;} _LL700: if(*(( int*) _temp696) ==  Cyc_Absyn_Var_e){ _LL713:
_temp712=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp696)->f2; if((
unsigned int) _temp712 >  1u?*(( int*) _temp712) ==  Cyc_Absyn_Local_b: 0){
_LL715: _temp714=(( struct Cyc_Absyn_Local_b_struct*) _temp712)->f1; goto _LL701;}
else{ goto _LL702;}} else{ goto _LL702;} _LL702: if(*(( int*) _temp696) ==  Cyc_Absyn_Var_e){
_LL717: _temp716=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp696)->f2; if((
unsigned int) _temp716 >  1u?*(( int*) _temp716) ==  Cyc_Absyn_Global_b: 0){
_LL719: _temp718=(( struct Cyc_Absyn_Global_b_struct*) _temp716)->f1; goto
_LL703;} else{ goto _LL704;}} else{ goto _LL704;} _LL704: if(*(( int*) _temp696)
==  Cyc_Absyn_Var_e){ _LL721: _temp720=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp696)->f2; if(( unsigned int) _temp720 >  1u?*(( int*) _temp720) ==  Cyc_Absyn_Param_b:
0){ _LL723: _temp722=(( struct Cyc_Absyn_Param_b_struct*) _temp720)->f1; goto
_LL705;} else{ goto _LL706;}} else{ goto _LL706;} _LL706: goto _LL707; _LL699:
_temp714= _temp710; goto _LL701; _LL701: _temp718= _temp714; goto _LL703; _LL703:
_temp722= _temp718; goto _LL705; _LL705: if( _temp722->escapes){ return 0;}
inner_loop: { void* _temp724=( void*) i->r; struct Cyc_Absyn_Exp* _temp744; void*
_temp746; void* _temp748; int _temp750; void* _temp752; void* _temp754; int
_temp756; void* _temp758; struct Cyc_List_List* _temp760; struct Cyc_List_List
_temp762; struct Cyc_List_List* _temp763; struct Cyc_List_List _temp765; struct
Cyc_Absyn_Exp* _temp766; struct Cyc_Absyn_Exp* _temp768; void* _temp770; void*
_temp772; struct Cyc_Absyn_Vardecl* _temp774; void* _temp776; struct Cyc_Absyn_Vardecl*
_temp778; void* _temp780; struct Cyc_Absyn_Vardecl* _temp782; void* _temp784;
struct Cyc_Absyn_Vardecl* _temp786; _LL726: if(*(( int*) _temp724) ==  Cyc_Absyn_Cast_e){
_LL747: _temp746=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp724)->f1; goto
_LL745; _LL745: _temp744=(( struct Cyc_Absyn_Cast_e_struct*) _temp724)->f2; goto
_LL727;} else{ goto _LL728;} _LL728: if(*(( int*) _temp724) ==  Cyc_Absyn_Const_e){
_LL749: _temp748=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp724)->f1; if((
unsigned int) _temp748 >  1u?*(( int*) _temp748) ==  Cyc_Absyn_Int_c: 0){ _LL753:
_temp752=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp748)->f1; if( _temp752
== ( void*) Cyc_Absyn_Signed){ goto _LL751;} else{ goto _LL730;} _LL751:
_temp750=(( struct Cyc_Absyn_Int_c_struct*) _temp748)->f2; goto _LL729;} else{
goto _LL730;}} else{ goto _LL730;} _LL730: if(*(( int*) _temp724) ==  Cyc_Absyn_Const_e){
_LL755: _temp754=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp724)->f1; if((
unsigned int) _temp754 >  1u?*(( int*) _temp754) ==  Cyc_Absyn_Int_c: 0){ _LL759:
_temp758=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp754)->f1; if( _temp758
== ( void*) Cyc_Absyn_Unsigned){ goto _LL757;} else{ goto _LL732;} _LL757:
_temp756=(( struct Cyc_Absyn_Int_c_struct*) _temp754)->f2; goto _LL731;} else{
goto _LL732;}} else{ goto _LL732;} _LL732: if(*(( int*) _temp724) ==  Cyc_Absyn_Primop_e){
_LL771: _temp770=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp724)->f1;
if( _temp770 == ( void*) Cyc_Absyn_Mod){ goto _LL761;} else{ goto _LL734;}
_LL761: _temp760=(( struct Cyc_Absyn_Primop_e_struct*) _temp724)->f2; if(
_temp760 ==  0){ goto _LL734;} else{ _temp762=* _temp760; _LL769: _temp768=(
struct Cyc_Absyn_Exp*) _temp762.hd; goto _LL764; _LL764: _temp763= _temp762.tl;
if( _temp763 ==  0){ goto _LL734;} else{ _temp765=* _temp763; _LL767: _temp766=(
struct Cyc_Absyn_Exp*) _temp765.hd; goto _LL733;}}} else{ goto _LL734;} _LL734:
if(*(( int*) _temp724) ==  Cyc_Absyn_Var_e){ _LL773: _temp772=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp724)->f2; if(( unsigned int) _temp772 >  1u?*((
int*) _temp772) ==  Cyc_Absyn_Pat_b: 0){ _LL775: _temp774=(( struct Cyc_Absyn_Pat_b_struct*)
_temp772)->f1; goto _LL735;} else{ goto _LL736;}} else{ goto _LL736;} _LL736:
if(*(( int*) _temp724) ==  Cyc_Absyn_Var_e){ _LL777: _temp776=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp724)->f2; if(( unsigned int) _temp776 >  1u?*((
int*) _temp776) ==  Cyc_Absyn_Local_b: 0){ _LL779: _temp778=(( struct Cyc_Absyn_Local_b_struct*)
_temp776)->f1; goto _LL737;} else{ goto _LL738;}} else{ goto _LL738;} _LL738:
if(*(( int*) _temp724) ==  Cyc_Absyn_Var_e){ _LL781: _temp780=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp724)->f2; if(( unsigned int) _temp780 >  1u?*((
int*) _temp780) ==  Cyc_Absyn_Global_b: 0){ _LL783: _temp782=(( struct Cyc_Absyn_Global_b_struct*)
_temp780)->f1; goto _LL739;} else{ goto _LL740;}} else{ goto _LL740;} _LL740:
if(*(( int*) _temp724) ==  Cyc_Absyn_Var_e){ _LL785: _temp784=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp724)->f2; if(( unsigned int) _temp784 >  1u?*((
int*) _temp784) ==  Cyc_Absyn_Param_b: 0){ _LL787: _temp786=(( struct Cyc_Absyn_Param_b_struct*)
_temp784)->f1; goto _LL741;} else{ goto _LL742;}} else{ goto _LL742;} _LL742:
goto _LL743; _LL727: i= _temp744; goto inner_loop; _LL729: return _temp750 >=  0?
Cyc_Toc_check_const_array(( unsigned int)( _temp750 +  1),( void*) _temp722->type):
0; _LL731: return Cyc_Toc_check_const_array(( unsigned int)( _temp756 +  1),(
void*) _temp722->type); _LL733: return Cyc_Toc_check_leq_size( relns, _temp722,
_temp766); _LL735: _temp778= _temp774; goto _LL737; _LL737: _temp782= _temp778;
goto _LL739; _LL739: _temp786= _temp782; goto _LL741; _LL741: if( _temp786->escapes){
return 0;}{ struct Cyc_List_List* _temp788= relns; for( 0; _temp788 !=  0;
_temp788= _temp788->tl){ struct Cyc_CfFlowInfo_Reln* _temp789=( struct Cyc_CfFlowInfo_Reln*)
_temp788->hd; if( _temp789->vd ==  _temp786){ void* _temp790=( void*) _temp789->rop;
struct Cyc_Absyn_Vardecl* _temp800; struct Cyc_Absyn_Vardecl* _temp802;
unsigned int _temp804; _LL792: if(*(( int*) _temp790) ==  Cyc_CfFlowInfo_LessSize){
_LL801: _temp800=(( struct Cyc_CfFlowInfo_LessSize_struct*) _temp790)->f1; goto
_LL793;} else{ goto _LL794;} _LL794: if(*(( int*) _temp790) ==  Cyc_CfFlowInfo_LessVar){
_LL803: _temp802=(( struct Cyc_CfFlowInfo_LessVar_struct*) _temp790)->f1; goto
_LL795;} else{ goto _LL796;} _LL796: if(*(( int*) _temp790) ==  Cyc_CfFlowInfo_LessConst){
_LL805: _temp804=(( struct Cyc_CfFlowInfo_LessConst_struct*) _temp790)->f1; goto
_LL797;} else{ goto _LL798;} _LL798: goto _LL799; _LL793: if( _temp722 == 
_temp800){ return 1;} else{ goto _LL791;} _LL795:{ struct Cyc_List_List*
_temp806= relns; for( 0; _temp806 !=  0; _temp806= _temp806->tl){ struct Cyc_CfFlowInfo_Reln*
_temp807=( struct Cyc_CfFlowInfo_Reln*) _temp806->hd; if( _temp807->vd == 
_temp802){ void* _temp808=( void*) _temp807->rop; struct Cyc_Absyn_Vardecl*
_temp820; struct Cyc_Absyn_Vardecl* _temp822; unsigned int _temp824; struct Cyc_Absyn_Vardecl*
_temp826; _LL810: if(*(( int*) _temp808) ==  Cyc_CfFlowInfo_LessEqSize){ _LL821:
_temp820=(( struct Cyc_CfFlowInfo_LessEqSize_struct*) _temp808)->f1; goto _LL811;}
else{ goto _LL812;} _LL812: if(*(( int*) _temp808) ==  Cyc_CfFlowInfo_LessSize){
_LL823: _temp822=(( struct Cyc_CfFlowInfo_LessSize_struct*) _temp808)->f1; goto
_LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp808) ==  Cyc_CfFlowInfo_EqualConst){
_LL825: _temp824=(( struct Cyc_CfFlowInfo_EqualConst_struct*) _temp808)->f1;
goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp808) ==  Cyc_CfFlowInfo_LessVar){
_LL827: _temp826=(( struct Cyc_CfFlowInfo_LessVar_struct*) _temp808)->f1; goto
_LL817;} else{ goto _LL818;} _LL818: goto _LL819; _LL811: _temp822= _temp820;
goto _LL813; _LL813: if( _temp722 ==  _temp822){ return 1;} else{ goto _LL809;}
_LL815: return Cyc_Toc_check_const_array( _temp824,( void*) _temp722->type);
_LL817: if( Cyc_Toc_check_leq_size_var( relns, _temp722, _temp826)){ return 1;}
else{ goto _LL809;} _LL819: goto _LL809; _LL809:;}}} goto _LL791; _LL797: return
Cyc_Toc_check_const_array( _temp804,( void*) _temp722->type); _LL799: goto
_LL791; _LL791:;}}} goto _LL725; _LL743: goto _LL725; _LL725:;} goto _LL697;
_LL707: goto _LL697; _LL697:;} return 0;} static void* Cyc_Toc_get_c_typ( struct
Cyc_Absyn_Exp* e){ if( e->topt ==  0){({ void* _temp828[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp828, sizeof( void*), 0u));});}
return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp829[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp829, sizeof( void*), 0u));});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp830=( struct _tuple3*) _cycalloc( sizeof(
struct _tuple3)); _temp830->f1= Cyc_Toc_mt_tq; _temp830->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp830;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp831=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp831->f1= 0; _temp831->f2= e; _temp831;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp832= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp832): Cyc_Toc_malloc_ptr( _temp832), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp832), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp833=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp833 !=  0; _temp833=
_temp833->tl){ struct _tuple4 _temp836; struct Cyc_Absyn_Exp* _temp837; struct
Cyc_List_List* _temp839; struct _tuple4* _temp834=( struct _tuple4*) _temp833->hd;
_temp836=* _temp834; _LL840: _temp839= _temp836.f1; goto _LL838; _LL838:
_temp837= _temp836.f2; goto _LL835; _LL835: { struct Cyc_Absyn_Exp* e_index; if(
_temp839 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(
_temp839->tl !=  0){({ void* _temp841[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp841, sizeof( void*), 0u));});}{
void* _temp842=( void*) _temp839->hd; void* _temp843= _temp842; struct Cyc_Absyn_Exp*
_temp849; _LL845: if(*(( int*) _temp843) ==  Cyc_Absyn_ArrayElement){ _LL850:
_temp849=(( struct Cyc_Absyn_ArrayElement_struct*) _temp843)->f1; goto _LL846;}
else{ goto _LL847;} _LL847: if(*(( int*) _temp843) ==  Cyc_Absyn_FieldName){
goto _LL848;} else{ goto _LL844;} _LL846: Cyc_Toc_exp_to_c( nv, _temp849);
e_index= _temp849; goto _LL844; _LL848: e_index=({ void* _temp851[ 0u]={};((
struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("field name designators in array", sizeof( unsigned char), 32u),
_tag_arr( _temp851, sizeof( void*), 0u));}); goto _LL844; _LL844:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp852=( void*)
_temp837->r; struct Cyc_List_List* _temp862; struct Cyc_Absyn_Exp* _temp864;
struct Cyc_Absyn_Exp* _temp866; struct Cyc_Absyn_Vardecl* _temp868; struct Cyc_List_List*
_temp870; void* _temp872; _LL854: if(*(( int*) _temp852) ==  Cyc_Absyn_Array_e){
_LL863: _temp862=(( struct Cyc_Absyn_Array_e_struct*) _temp852)->f1; goto _LL855;}
else{ goto _LL856;} _LL856: if(*(( int*) _temp852) ==  Cyc_Absyn_Comprehension_e){
_LL869: _temp868=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp852)->f1;
goto _LL867; _LL867: _temp866=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp852)->f2; goto _LL865; _LL865: _temp864=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp852)->f3; goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp852)
==  Cyc_Absyn_AnonStruct_e){ _LL873: _temp872=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp852)->f1; goto _LL871; _LL871: _temp870=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp852)->f2; goto _LL859;} else{ goto _LL860;} _LL860: goto _LL861; _LL855: s=
Cyc_Toc_init_array( nv, lval, _temp862, s); goto _LL853; _LL857: s= Cyc_Toc_init_comprehension(
nv, lval, _temp868, _temp866, _temp864, s, 0); goto _LL853; _LL859: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp872, _temp870, s); goto _LL853; _LL861: Cyc_Toc_exp_to_c( nv,
_temp837); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp837, 0), s, 0); goto _LL853; _LL853:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp874= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp874, Cyc_Absyn_varb_exp( _temp874,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp897=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp897[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp898; _temp898.tag= Cyc_Absyn_Local_b; _temp898.f1= vd; _temp898;});
_temp897;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp874, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp874, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp874, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp874, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp875=( void*) e2->r; struct Cyc_List_List* _temp885; struct Cyc_Absyn_Exp*
_temp887; struct Cyc_Absyn_Exp* _temp889; struct Cyc_Absyn_Vardecl* _temp891;
struct Cyc_List_List* _temp893; void* _temp895; _LL877: if(*(( int*) _temp875)
==  Cyc_Absyn_Array_e){ _LL886: _temp885=(( struct Cyc_Absyn_Array_e_struct*)
_temp875)->f1; goto _LL878;} else{ goto _LL879;} _LL879: if(*(( int*) _temp875)
==  Cyc_Absyn_Comprehension_e){ _LL892: _temp891=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp875)->f1; goto _LL890; _LL890: _temp889=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp875)->f2; goto _LL888; _LL888: _temp887=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp875)->f3; goto _LL880;} else{ goto _LL881;} _LL881: if(*(( int*) _temp875)
==  Cyc_Absyn_AnonStruct_e){ _LL896: _temp895=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp875)->f1; goto _LL894; _LL894: _temp893=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp875)->f2; goto _LL882;} else{ goto _LL883;} _LL883: goto _LL884; _LL878:
body= Cyc_Toc_init_array( nv2, lval, _temp885, Cyc_Absyn_skip_stmt( 0)); goto
_LL876; _LL880: body= Cyc_Toc_init_comprehension( nv2, lval, _temp891, _temp889,
_temp887, Cyc_Absyn_skip_stmt( 0), 0); goto _LL876; _LL882: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp895, _temp893, Cyc_Absyn_skip_stmt( 0)); goto _LL876; _LL884: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL876; _LL876:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp874, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp899=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp899 !=  0; _temp899= _temp899->tl){
struct _tuple4 _temp902; struct Cyc_Absyn_Exp* _temp903; struct Cyc_List_List*
_temp905; struct _tuple4* _temp900=( struct _tuple4*) _temp899->hd; _temp902=*
_temp900; _LL906: _temp905= _temp902.f1; goto _LL904; _LL904: _temp903= _temp902.f2;
goto _LL901; _LL901: if( _temp905 ==  0){({ void* _temp907[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp907, sizeof( void*), 0u));});} if(
_temp905->tl !=  0){({ void* _temp908[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u), _tag_arr( _temp908, sizeof( void*), 0u));});}{
void* _temp909=( void*) _temp905->hd; struct _tagged_arr* _temp915; _LL911: if(*((
int*) _temp909) ==  Cyc_Absyn_FieldName){ _LL916: _temp915=(( struct Cyc_Absyn_FieldName_struct*)
_temp909)->f1; goto _LL912;} else{ goto _LL913;} _LL913: goto _LL914; _LL912: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp915, 0);{ void*
_temp917=( void*) _temp903->r; struct Cyc_List_List* _temp927; struct Cyc_Absyn_Exp*
_temp929; struct Cyc_Absyn_Exp* _temp931; struct Cyc_Absyn_Vardecl* _temp933;
struct Cyc_List_List* _temp935; void* _temp937; _LL919: if(*(( int*) _temp917)
==  Cyc_Absyn_Array_e){ _LL928: _temp927=(( struct Cyc_Absyn_Array_e_struct*)
_temp917)->f1; goto _LL920;} else{ goto _LL921;} _LL921: if(*(( int*) _temp917)
==  Cyc_Absyn_Comprehension_e){ _LL934: _temp933=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp917)->f1; goto _LL932; _LL932: _temp931=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp917)->f2; goto _LL930; _LL930: _temp929=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp917)->f3; goto _LL922;} else{ goto _LL923;} _LL923: if(*(( int*) _temp917)
==  Cyc_Absyn_AnonStruct_e){ _LL938: _temp937=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp917)->f1; goto _LL936; _LL936: _temp935=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp917)->f2; goto _LL924;} else{ goto _LL925;} _LL925: goto _LL926; _LL920: s=
Cyc_Toc_init_array( nv, lval, _temp927, s); goto _LL918; _LL922: s= Cyc_Toc_init_comprehension(
nv, lval, _temp933, _temp931, _temp929, s, 0); goto _LL918; _LL924: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp937, _temp935, s); goto _LL918; _LL926: Cyc_Toc_exp_to_c( nv,
_temp903); if( Cyc_Toc_is_poly_field( struct_type, _temp915)){ _temp903= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp903, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp903, 0), 0), s, 0); goto _LL918; _LL918:;} goto
_LL910;} _LL914:( int) _throw(({ void* _temp939[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp939, sizeof( void*), 0u));})); _LL910:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp940= _new_region("r");
struct _RegionHandle* r=& _temp940; _push_region( r);{ struct Cyc_List_List*
_temp941=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp942= Cyc_Toc_add_tuple_type( _temp941); struct
_tuple0* _temp943= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp944= Cyc_Absyn_var_exp(
_temp943, 0); struct Cyc_Absyn_Stmt* _temp945= Cyc_Absyn_exp_stmt( _temp944, 0);
struct Cyc_Absyn_Exp*(* _temp946)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp947=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp947); for( 0; _temp947
!=  0;( _temp947= _temp947->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
_temp947->hd; struct Cyc_Absyn_Exp* lval= _temp946( _temp944, Cyc_Absyn_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp948=( void*) e->r; struct Cyc_List_List*
_temp956; struct Cyc_Absyn_Exp* _temp958; struct Cyc_Absyn_Exp* _temp960; struct
Cyc_Absyn_Vardecl* _temp962; _LL950: if(*(( int*) _temp948) ==  Cyc_Absyn_Array_e){
_LL957: _temp956=(( struct Cyc_Absyn_Array_e_struct*) _temp948)->f1; goto _LL951;}
else{ goto _LL952;} _LL952: if(*(( int*) _temp948) ==  Cyc_Absyn_Comprehension_e){
_LL963: _temp962=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp948)->f1;
goto _LL961; _LL961: _temp960=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp948)->f2; goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp948)->f3; goto _LL953;} else{ goto _LL954;} _LL954: goto _LL955; _LL951:
_temp945= Cyc_Toc_init_array( nv, lval, _temp956, _temp945); goto _LL949; _LL953:
_temp945= Cyc_Toc_init_comprehension( nv, lval, _temp962, _temp960, _temp958,
_temp945, 0); goto _LL949; _LL955: Cyc_Toc_exp_to_c( nv, e); _temp945= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp946( _temp944, Cyc_Absyn_fieldname(
i), 0), e, 0), 0), _temp945, 0); goto _LL949; _LL949:;}}}{ struct Cyc_Absyn_Exp*
_temp964= Cyc_Toc_make_struct( nv, _temp943, Cyc_Absyn_strct( _temp942),
_temp945, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp964;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp965= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp966= Cyc_Absyn_var_exp( _temp965, 0); struct Cyc_Absyn_Stmt*
_temp967= Cyc_Absyn_exp_stmt( _temp966, 0); struct Cyc_Absyn_Exp*(* _temp968)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp969= _new_region("r"); struct _RegionHandle* r=&
_temp969; _push_region( r);{ struct Cyc_List_List* _temp970=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp970 !=  0; _temp970= _temp970->tl){ struct _tuple4 _temp973; struct Cyc_Absyn_Exp*
_temp974; struct Cyc_List_List* _temp976; struct _tuple4* _temp971=( struct
_tuple4*) _temp970->hd; _temp973=* _temp971; _LL977: _temp976= _temp973.f1; goto
_LL975; _LL975: _temp974= _temp973.f2; goto _LL972; _LL972: is_atomic= is_atomic?
Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp974->topt))->v):
0; if( _temp976 ==  0){({ void* _temp978[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp978, sizeof( void*), 0u));});} if(
_temp976->tl !=  0){ struct _tuple0* _temp979= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp980= Cyc_Absyn_var_exp( _temp979, 0); for( 0; _temp976 !=  0; _temp976=
_temp976->tl){ void* _temp981=( void*) _temp976->hd; struct _tagged_arr*
_temp987; _LL983: if(*(( int*) _temp981) ==  Cyc_Absyn_FieldName){ _LL988:
_temp987=(( struct Cyc_Absyn_FieldName_struct*) _temp981)->f1; goto _LL984;}
else{ goto _LL985;} _LL985: goto _LL986; _LL984: if( Cyc_Toc_is_poly_field(
struct_type, _temp987)){ _temp980= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp980, 0);} _temp967= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp968( _temp966, _temp987, 0), _temp980, 0), 0), _temp967, 0); goto _LL982;
_LL986:( int) _throw(({ void* _temp989[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u), _tag_arr( _temp989, sizeof( void*), 0u));}));
_LL982:;} Cyc_Toc_exp_to_c( nv, _temp974); _temp967= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp980, _temp974, 0), 0), _temp967, 0);} else{ void*
_temp990=( void*) _temp976->hd; struct _tagged_arr* _temp996; _LL992: if(*(( int*)
_temp990) ==  Cyc_Absyn_FieldName){ _LL997: _temp996=(( struct Cyc_Absyn_FieldName_struct*)
_temp990)->f1; goto _LL993;} else{ goto _LL994;} _LL994: goto _LL995; _LL993: {
struct Cyc_Absyn_Exp* lval= _temp968( _temp966, _temp996, 0);{ void* _temp998=(
void*) _temp974->r; struct Cyc_List_List* _temp1008; struct Cyc_Absyn_Exp*
_temp1010; struct Cyc_Absyn_Exp* _temp1012; struct Cyc_Absyn_Vardecl* _temp1014;
struct Cyc_List_List* _temp1016; void* _temp1018; _LL1000: if(*(( int*) _temp998)
==  Cyc_Absyn_Array_e){ _LL1009: _temp1008=(( struct Cyc_Absyn_Array_e_struct*)
_temp998)->f1; goto _LL1001;} else{ goto _LL1002;} _LL1002: if(*(( int*)
_temp998) ==  Cyc_Absyn_Comprehension_e){ _LL1015: _temp1014=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp998)->f1; goto _LL1013; _LL1013: _temp1012=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp998)->f2; goto _LL1011; _LL1011: _temp1010=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp998)->f3; goto _LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*)
_temp998) ==  Cyc_Absyn_AnonStruct_e){ _LL1019: _temp1018=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp998)->f1; goto _LL1017; _LL1017: _temp1016=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp998)->f2; goto _LL1005;} else{ goto _LL1006;} _LL1006: goto _LL1007;
_LL1001: _temp967= Cyc_Toc_init_array( nv, lval, _temp1008, _temp967); goto
_LL999; _LL1003: _temp967= Cyc_Toc_init_comprehension( nv, lval, _temp1014,
_temp1012, _temp1010, _temp967, 0); goto _LL999; _LL1005: _temp967= Cyc_Toc_init_anon_struct(
nv, lval, _temp1018, _temp1016, _temp967); goto _LL999; _LL1007: Cyc_Toc_exp_to_c(
nv, _temp974); if( Cyc_Toc_is_poly_field( struct_type, _temp996)){ _temp974= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp974, 0);} _temp967= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp974, 0), 0), _temp967, 0); goto _LL999; _LL999:;}
goto _LL991;} _LL995:( int) _throw(({ void* _temp1020[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp1020, sizeof( void*), 0u));})); _LL991:;}}}{ struct Cyc_Absyn_Exp*
_temp1021= Cyc_Toc_make_struct( nv, _temp965, Cyc_Absyn_strctq( tdn), _temp967,
pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp1021;}; _pop_region(
r);} struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el,
struct _tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1,
int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1022=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1022[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1023; _temp1023.tag= Cyc_Absyn_Increment_e;
_temp1023.f1= e1; _temp1023.f2=( void*) incr; _temp1023;}); _temp1022;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1024=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1034; struct Cyc_Absyn_Exp*
_temp1036; void* _temp1038; struct _tagged_arr* _temp1040; struct Cyc_Absyn_Exp*
_temp1042; _LL1026: if(*(( int*) _temp1024) ==  Cyc_Absyn_StmtExp_e){ _LL1035:
_temp1034=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1024)->f1; goto _LL1027;}
else{ goto _LL1028;} _LL1028: if(*(( int*) _temp1024) ==  Cyc_Absyn_Cast_e){
_LL1039: _temp1038=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1024)->f1;
goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_Cast_e_struct*) _temp1024)->f2;
goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*) _temp1024) ==  Cyc_Absyn_StructMember_e){
_LL1043: _temp1042=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1024)->f1;
goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1024)->f2; goto _LL1031;} else{ goto _LL1032;} _LL1032: goto _LL1033;
_LL1027: Cyc_Toc_lvalue_assign_stmt( _temp1034, fs, f, f_env); goto _LL1025;
_LL1029: Cyc_Toc_lvalue_assign( _temp1036, fs, f, f_env); goto _LL1025; _LL1031:(
void*)( e1->r=( void*)(( void*) _temp1042->r)); Cyc_Toc_lvalue_assign( e1,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1044=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1044->hd=( void*) _temp1040;
_temp1044->tl= fs; _temp1044;}), f, f_env); goto _LL1025; _LL1033: { struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs !=  0; fs= fs->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_arr*) fs->hd, e1_copy->loc);}( void*)( e1->r=( void*)((
void*)( f( e1_copy, f_env))->r)); goto _LL1025;} _LL1025:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1045=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1055; struct Cyc_Absyn_Stmt* _temp1057; struct Cyc_Absyn_Decl*
_temp1059; struct Cyc_Absyn_Stmt* _temp1061; _LL1047: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Exp_s: 0){ _LL1056: _temp1055=((
struct Cyc_Absyn_Exp_s_struct*) _temp1045)->f1; goto _LL1048;} else{ goto
_LL1049;} _LL1049: if(( unsigned int) _temp1045 >  1u?*(( int*) _temp1045) == 
Cyc_Absyn_Decl_s: 0){ _LL1060: _temp1059=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1045)->f1; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1045)->f2; goto _LL1050;} else{ goto _LL1051;} _LL1051: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Seq_s: 0){ _LL1062: _temp1061=((
struct Cyc_Absyn_Seq_s_struct*) _temp1045)->f2; goto _LL1052;} else{ goto
_LL1053;} _LL1053: goto _LL1054; _LL1048: Cyc_Toc_lvalue_assign( _temp1055, fs,
f, f_env); goto _LL1046; _LL1050: Cyc_Toc_lvalue_assign_stmt( _temp1057, fs, f,
f_env); goto _LL1046; _LL1052: Cyc_Toc_lvalue_assign_stmt( _temp1061, fs, f,
f_env); goto _LL1046; _LL1054:({ struct Cyc_Std_String_pa_struct _temp1064;
_temp1064.tag= Cyc_Std_String_pa; _temp1064.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp1063[ 1u]={& _temp1064};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1063, sizeof( void*), 1u));}});
goto _LL1046; _LL1046:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x ==  0){ return 0;}
result=({ struct Cyc_List_List* _temp1065=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1065->hd=( void*) f((
void*) x->hd, env); _temp1065->tl= 0; _temp1065;}); prev= result; for( x= x->tl;
x !=  0; x= x->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({ struct
Cyc_List_List* _temp1066=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp1066->hd=( void*) f(( void*) x->hd, env); _temp1066->tl=
0; _temp1066;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c( Cyc_Core_heap_region,
f, env, x);} static struct _tuple4* Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){
return({ struct _tuple4* _temp1067=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1067->f1= 0; _temp1067->f2= e; _temp1067;});} static struct Cyc_Absyn_PtrInfo
Cyc_Toc_get_ptr_type( void* t){ void* _temp1068= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp1074; _LL1070: if(( unsigned int) _temp1068 >  3u?*(( int*)
_temp1068) ==  Cyc_Absyn_PointerType: 0){ _LL1075: _temp1074=(( struct Cyc_Absyn_PointerType_struct*)
_temp1068)->f1; goto _LL1071;} else{ goto _LL1072;} _LL1072: goto _LL1073;
_LL1071: return _temp1074; _LL1073:( int) _throw(({ void* _temp1076[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("get_ptr_typ: not a pointer!", sizeof( unsigned char), 28u), _tag_arr(
_temp1076, sizeof( void*), 0u));})); _LL1069:;} struct _tuple8{ void* f1; void*
f2; } ; struct _tuple9{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3;
} ; struct _tuple10{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2;
} ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ void* _temp1077=( void*) e->r; if( e->topt ==  0){({ struct Cyc_Std_String_pa_struct
_temp1079; _temp1079.tag= Cyc_Std_String_pa; _temp1079.f1=( struct _tagged_arr)
Cyc_Absynpp_exp2string( e);{ void* _temp1078[ 1u]={& _temp1079};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: no type for %s",
sizeof( unsigned char), 25u), _tag_arr( _temp1078, sizeof( void*), 1u));}});
return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
void* _temp1080= _temp1077; void* _temp1166; void* _temp1168; struct _tuple0*
_temp1170; struct _tuple0* _temp1172; struct Cyc_List_List* _temp1174; void*
_temp1176; void* _temp1178; struct Cyc_Absyn_Exp* _temp1180; struct Cyc_Absyn_Exp*
_temp1182; struct Cyc_Core_Opt* _temp1184; struct Cyc_Absyn_Exp* _temp1186;
struct Cyc_Absyn_Exp* _temp1188; struct Cyc_Absyn_Exp* _temp1190; struct Cyc_Absyn_Exp*
_temp1192; struct Cyc_Absyn_Exp* _temp1194; struct Cyc_Absyn_Exp* _temp1196;
struct Cyc_List_List* _temp1198; struct Cyc_Absyn_Exp* _temp1200; struct Cyc_Absyn_VarargCallInfo*
_temp1202; struct Cyc_List_List* _temp1204; struct Cyc_Absyn_Exp* _temp1206;
struct Cyc_Absyn_VarargCallInfo* _temp1208; struct Cyc_Absyn_VarargCallInfo
_temp1210; struct Cyc_Absyn_VarargInfo* _temp1211; struct Cyc_List_List*
_temp1213; int _temp1215; struct Cyc_List_List* _temp1217; struct Cyc_Absyn_Exp*
_temp1219; struct Cyc_Absyn_Exp* _temp1221; struct Cyc_Absyn_Exp* _temp1223;
struct Cyc_List_List* _temp1225; struct Cyc_Absyn_Exp* _temp1227; struct Cyc_Absyn_Exp*
_temp1229; void* _temp1231; void** _temp1233; struct Cyc_Absyn_Exp* _temp1234;
struct Cyc_Absyn_Exp* _temp1236; struct Cyc_Absyn_Exp* _temp1238; struct Cyc_Absyn_Exp*
_temp1240; void* _temp1242; void* _temp1244; struct _tagged_arr* _temp1246; void*
_temp1248; void* _temp1250; unsigned int _temp1252; void* _temp1254; struct Cyc_Absyn_Exp*
_temp1256; struct _tagged_arr* _temp1258; struct Cyc_Absyn_Exp* _temp1260;
struct _tagged_arr* _temp1262; struct Cyc_Absyn_Exp* _temp1264; struct Cyc_Absyn_Exp*
_temp1266; struct Cyc_Absyn_Exp* _temp1268; struct Cyc_List_List* _temp1270;
struct Cyc_List_List* _temp1272; struct Cyc_Absyn_Exp* _temp1274; struct Cyc_Absyn_Exp*
_temp1276; struct Cyc_Absyn_Vardecl* _temp1278; struct Cyc_Absyn_Structdecl*
_temp1280; struct Cyc_List_List* _temp1282; struct Cyc_Core_Opt* _temp1284;
struct _tuple0* _temp1286; struct Cyc_List_List* _temp1288; void* _temp1290;
struct Cyc_Absyn_Tunionfield* _temp1292; struct Cyc_Absyn_Tuniondecl* _temp1294;
struct Cyc_List_List* _temp1296; struct Cyc_Absyn_Tunionfield* _temp1298; struct
Cyc_Absyn_Tuniondecl* _temp1300; struct Cyc_List_List* _temp1302; struct Cyc_Absyn_MallocInfo
_temp1304; int _temp1306; struct Cyc_Absyn_Exp* _temp1308; void** _temp1310;
struct Cyc_Absyn_Exp* _temp1312; int _temp1314; struct Cyc_Absyn_Stmt* _temp1316;
struct Cyc_Absyn_Fndecl* _temp1318; _LL1082: if(*(( int*) _temp1080) ==  Cyc_Absyn_Const_e){
_LL1167: _temp1166=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1080)->f1;
if( _temp1166 == ( void*) Cyc_Absyn_Null_c){ goto _LL1083;} else{ goto _LL1084;}}
else{ goto _LL1084;} _LL1084: if(*(( int*) _temp1080) ==  Cyc_Absyn_Const_e){
goto _LL1085;} else{ goto _LL1086;} _LL1086: if(*(( int*) _temp1080) ==  Cyc_Absyn_Var_e){
_LL1171: _temp1170=(( struct Cyc_Absyn_Var_e_struct*) _temp1080)->f1; goto
_LL1169; _LL1169: _temp1168=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1080)->f2;
goto _LL1087;} else{ goto _LL1088;} _LL1088: if(*(( int*) _temp1080) ==  Cyc_Absyn_UnknownId_e){
_LL1173: _temp1172=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1080)->f1; goto
_LL1089;} else{ goto _LL1090;} _LL1090: if(*(( int*) _temp1080) ==  Cyc_Absyn_Primop_e){
_LL1177: _temp1176=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1080)->f1;
goto _LL1175; _LL1175: _temp1174=(( struct Cyc_Absyn_Primop_e_struct*) _temp1080)->f2;
goto _LL1091;} else{ goto _LL1092;} _LL1092: if(*(( int*) _temp1080) ==  Cyc_Absyn_Increment_e){
_LL1181: _temp1180=(( struct Cyc_Absyn_Increment_e_struct*) _temp1080)->f1; goto
_LL1179; _LL1179: _temp1178=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1080)->f2; goto _LL1093;} else{ goto _LL1094;} _LL1094: if(*(( int*)
_temp1080) ==  Cyc_Absyn_AssignOp_e){ _LL1187: _temp1186=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1080)->f1; goto _LL1185; _LL1185: _temp1184=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1080)->f2; goto _LL1183; _LL1183: _temp1182=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1080)->f3; goto _LL1095;} else{ goto _LL1096;} _LL1096: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Conditional_e){ _LL1193: _temp1192=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1080)->f1; goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1080)->f2; goto _LL1189; _LL1189: _temp1188=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1080)->f3; goto _LL1097;} else{ goto _LL1098;} _LL1098: if(*(( int*)
_temp1080) ==  Cyc_Absyn_SeqExp_e){ _LL1197: _temp1196=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1080)->f1; goto _LL1195; _LL1195: _temp1194=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1080)->f2; goto _LL1099;} else{ goto _LL1100;} _LL1100: if(*(( int*)
_temp1080) ==  Cyc_Absyn_UnknownCall_e){ _LL1201: _temp1200=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1080)->f1; goto _LL1199; _LL1199: _temp1198=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1080)->f2; goto _LL1101;} else{ goto _LL1102;} _LL1102: if(*(( int*)
_temp1080) ==  Cyc_Absyn_FnCall_e){ _LL1207: _temp1206=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1080)->f1; goto _LL1205; _LL1205: _temp1204=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1080)->f2; goto _LL1203; _LL1203: _temp1202=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1080)->f3; if( _temp1202 ==  0){ goto _LL1103;} else{ goto _LL1104;}} else{
goto _LL1104;} _LL1104: if(*(( int*) _temp1080) ==  Cyc_Absyn_FnCall_e){ _LL1220:
_temp1219=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1080)->f1; goto _LL1218;
_LL1218: _temp1217=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1080)->f2; goto
_LL1209; _LL1209: _temp1208=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1080)->f3;
if( _temp1208 ==  0){ goto _LL1106;} else{ _temp1210=* _temp1208; _LL1216:
_temp1215= _temp1210.num_varargs; goto _LL1214; _LL1214: _temp1213= _temp1210.injectors;
goto _LL1212; _LL1212: _temp1211= _temp1210.vai; goto _LL1105;}} else{ goto
_LL1106;} _LL1106: if(*(( int*) _temp1080) ==  Cyc_Absyn_Throw_e){ _LL1222:
_temp1221=(( struct Cyc_Absyn_Throw_e_struct*) _temp1080)->f1; goto _LL1107;}
else{ goto _LL1108;} _LL1108: if(*(( int*) _temp1080) ==  Cyc_Absyn_NoInstantiate_e){
_LL1224: _temp1223=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1080)->f1;
goto _LL1109;} else{ goto _LL1110;} _LL1110: if(*(( int*) _temp1080) ==  Cyc_Absyn_Instantiate_e){
_LL1228: _temp1227=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1080)->f1;
goto _LL1226; _LL1226: _temp1225=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1080)->f2; goto _LL1111;} else{ goto _LL1112;} _LL1112: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Cast_e){ _LL1232: _temp1231=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1080)->f1; _temp1233=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1080)->f1); goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1080)->f2; goto _LL1113;} else{ goto _LL1114;} _LL1114: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Address_e){ _LL1235: _temp1234=(( struct Cyc_Absyn_Address_e_struct*)
_temp1080)->f1; goto _LL1115;} else{ goto _LL1116;} _LL1116: if(*(( int*)
_temp1080) ==  Cyc_Absyn_New_e){ _LL1239: _temp1238=(( struct Cyc_Absyn_New_e_struct*)
_temp1080)->f1; goto _LL1237; _LL1237: _temp1236=(( struct Cyc_Absyn_New_e_struct*)
_temp1080)->f2; goto _LL1117;} else{ goto _LL1118;} _LL1118: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Sizeofexp_e){ _LL1241: _temp1240=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1080)->f1; goto _LL1119;} else{ goto _LL1120;} _LL1120: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1243: _temp1242=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1080)->f1; goto _LL1121;} else{ goto _LL1122;} _LL1122: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Gentyp_e){ goto _LL1123;} else{ goto _LL1124;} _LL1124:
if(*(( int*) _temp1080) ==  Cyc_Absyn_Offsetof_e){ _LL1249: _temp1248=( void*)((
struct Cyc_Absyn_Offsetof_e_struct*) _temp1080)->f1; goto _LL1245; _LL1245:
_temp1244=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp1080)->f2; if(*((
int*) _temp1244) ==  Cyc_Absyn_StructField){ _LL1247: _temp1246=(( struct Cyc_Absyn_StructField_struct*)
_temp1244)->f1; goto _LL1125;} else{ goto _LL1126;}} else{ goto _LL1126;}
_LL1126: if(*(( int*) _temp1080) ==  Cyc_Absyn_Offsetof_e){ _LL1255: _temp1254=(
void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp1080)->f1; goto _LL1251;
_LL1251: _temp1250=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp1080)->f2;
if(*(( int*) _temp1250) ==  Cyc_Absyn_TupleIndex){ _LL1253: _temp1252=(( struct
Cyc_Absyn_TupleIndex_struct*) _temp1250)->f1; goto _LL1127;} else{ goto _LL1128;}}
else{ goto _LL1128;} _LL1128: if(*(( int*) _temp1080) ==  Cyc_Absyn_Deref_e){
_LL1257: _temp1256=(( struct Cyc_Absyn_Deref_e_struct*) _temp1080)->f1; goto
_LL1129;} else{ goto _LL1130;} _LL1130: if(*(( int*) _temp1080) ==  Cyc_Absyn_StructMember_e){
_LL1261: _temp1260=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1080)->f1;
goto _LL1259; _LL1259: _temp1258=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1080)->f2; goto _LL1131;} else{ goto _LL1132;} _LL1132: if(*(( int*)
_temp1080) ==  Cyc_Absyn_StructArrow_e){ _LL1265: _temp1264=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1080)->f1; goto _LL1263; _LL1263: _temp1262=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1080)->f2; goto _LL1133;} else{ goto _LL1134;} _LL1134: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Subscript_e){ _LL1269: _temp1268=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1080)->f1; goto _LL1267; _LL1267: _temp1266=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1080)->f2; goto _LL1135;} else{ goto _LL1136;} _LL1136: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Tuple_e){ _LL1271: _temp1270=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1080)->f1; goto _LL1137;} else{ goto _LL1138;} _LL1138: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Array_e){ _LL1273: _temp1272=(( struct Cyc_Absyn_Array_e_struct*)
_temp1080)->f1; goto _LL1139;} else{ goto _LL1140;} _LL1140: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Comprehension_e){ _LL1279: _temp1278=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1080)->f1; goto _LL1277; _LL1277: _temp1276=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1080)->f2; goto _LL1275; _LL1275: _temp1274=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1080)->f3; goto _LL1141;} else{ goto _LL1142;} _LL1142: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Struct_e){ _LL1287: _temp1286=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1080)->f1; goto _LL1285; _LL1285: _temp1284=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1080)->f2; goto _LL1283; _LL1283: _temp1282=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1080)->f3; goto _LL1281; _LL1281: _temp1280=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1080)->f4; goto _LL1143;} else{ goto _LL1144;} _LL1144: if(*(( int*)
_temp1080) ==  Cyc_Absyn_AnonStruct_e){ _LL1291: _temp1290=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1080)->f1; goto _LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1080)->f2; goto _LL1145;} else{ goto _LL1146;} _LL1146: if(*(( int*)
_temp1080) ==  Cyc_Absyn_Tunion_e){ _LL1297: _temp1296=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1080)->f3; if( _temp1296 ==  0){ goto _LL1295;} else{ goto _LL1148;}
_LL1295: _temp1294=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1080)->f4; goto
_LL1293; _LL1293: _temp1292=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1080)->f5;
goto _LL1147;} else{ goto _LL1148;} _LL1148: if(*(( int*) _temp1080) ==  Cyc_Absyn_Tunion_e){
_LL1303: _temp1302=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1080)->f3; goto
_LL1301; _LL1301: _temp1300=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1080)->f4;
goto _LL1299; _LL1299: _temp1298=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1080)->f5;
goto _LL1149;} else{ goto _LL1150;} _LL1150: if(*(( int*) _temp1080) ==  Cyc_Absyn_Enum_e){
goto _LL1151;} else{ goto _LL1152;} _LL1152: if(*(( int*) _temp1080) ==  Cyc_Absyn_AnonEnum_e){
goto _LL1153;} else{ goto _LL1154;} _LL1154: if(*(( int*) _temp1080) ==  Cyc_Absyn_Malloc_e){
_LL1305: _temp1304=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1080)->f1; _LL1315:
_temp1314= _temp1304.is_calloc; goto _LL1313; _LL1313: _temp1312= _temp1304.rgn;
goto _LL1311; _LL1311: _temp1310= _temp1304.elt_type; goto _LL1309; _LL1309:
_temp1308= _temp1304.num_elts; goto _LL1307; _LL1307: _temp1306= _temp1304.fat_result;
goto _LL1155;} else{ goto _LL1156;} _LL1156: if(*(( int*) _temp1080) ==  Cyc_Absyn_StmtExp_e){
_LL1317: _temp1316=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1080)->f1; goto
_LL1157;} else{ goto _LL1158;} _LL1158: if(*(( int*) _temp1080) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL1159;} else{ goto _LL1160;} _LL1160: if(*(( int*) _temp1080) ==  Cyc_Absyn_CompoundLit_e){
goto _LL1161;} else{ goto _LL1162;} _LL1162: if(*(( int*) _temp1080) ==  Cyc_Absyn_Codegen_e){
_LL1319: _temp1318=(( struct Cyc_Absyn_Codegen_e_struct*) _temp1080)->f1; goto
_LL1163;} else{ goto _LL1164;} _LL1164: if(*(( int*) _temp1080) ==  Cyc_Absyn_Fill_e){
goto _LL1165;} else{ goto _LL1081;} _LL1083: { struct Cyc_Absyn_Exp* _temp1320=
Cyc_Absyn_uint_exp( 0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if(
nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_typ, _temp1320, _temp1320))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1321[ 3u];
_temp1321[ 2u]= _temp1320; _temp1321[ 1u]= _temp1320; _temp1321[ 0u]= _temp1320;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1321, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} else{( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r));} goto _LL1081;} _LL1085:
goto _LL1081; _LL1087:{ struct _handler_cons _temp1322; _push_handler(&
_temp1322);{ int _temp1324= 0; if( setjmp( _temp1322.handler)){ _temp1324= 1;}
if( ! _temp1324){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp1170))->r));; _pop_handler();} else{ void* _temp1323=( void*) _exn_thrown;
void* _temp1326= _temp1323; _LL1328: if( _temp1326 ==  Cyc_Dict_Absent){ goto
_LL1329;} else{ goto _LL1330;} _LL1330: goto _LL1331; _LL1329:({ struct Cyc_Std_String_pa_struct
_temp1333; _temp1333.tag= Cyc_Std_String_pa; _temp1333.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp1170);{ void* _temp1332[ 1u]={& _temp1333};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1332, sizeof( void*), 1u));}});
return; _LL1331:( void) _throw( _temp1326); _LL1327:;}}} goto _LL1081; _LL1089:({
void* _temp1334[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1334, sizeof( void*), 0u));}); goto _LL1081; _LL1091: { struct Cyc_List_List*
_temp1335=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1174);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1174);{
void* _temp1336= _temp1176; _LL1338: if( _temp1336 == ( void*) Cyc_Absyn_Size){
goto _LL1339;} else{ goto _LL1340;} _LL1340: if( _temp1336 == ( void*) Cyc_Absyn_Plus){
goto _LL1341;} else{ goto _LL1342;} _LL1342: if( _temp1336 == ( void*) Cyc_Absyn_Minus){
goto _LL1343;} else{ goto _LL1344;} _LL1344: if( _temp1336 == ( void*) Cyc_Absyn_Eq){
goto _LL1345;} else{ goto _LL1346;} _LL1346: if( _temp1336 == ( void*) Cyc_Absyn_Neq){
goto _LL1347;} else{ goto _LL1348;} _LL1348: if( _temp1336 == ( void*) Cyc_Absyn_Gt){
goto _LL1349;} else{ goto _LL1350;} _LL1350: if( _temp1336 == ( void*) Cyc_Absyn_Gte){
goto _LL1351;} else{ goto _LL1352;} _LL1352: if( _temp1336 == ( void*) Cyc_Absyn_Lt){
goto _LL1353;} else{ goto _LL1354;} _LL1354: if( _temp1336 == ( void*) Cyc_Absyn_Lte){
goto _LL1355;} else{ goto _LL1356;} _LL1356: goto _LL1357; _LL1339: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1174))->hd;{
void* _temp1358= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1366; struct Cyc_Absyn_PtrInfo
_temp1368; struct Cyc_Absyn_Conref* _temp1370; struct Cyc_Absyn_Conref*
_temp1372; void* _temp1374; _LL1360: if(( unsigned int) _temp1358 >  3u?*(( int*)
_temp1358) ==  Cyc_Absyn_ArrayType: 0){ _LL1367: _temp1366=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1358)->f3; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(( unsigned int)
_temp1358 >  3u?*(( int*) _temp1358) ==  Cyc_Absyn_PointerType: 0){ _LL1369:
_temp1368=(( struct Cyc_Absyn_PointerType_struct*) _temp1358)->f1; _LL1375:
_temp1374=( void*) _temp1368.elt_typ; goto _LL1373; _LL1373: _temp1372=
_temp1368.nullable; goto _LL1371; _LL1371: _temp1370= _temp1368.bounds; goto
_LL1363;} else{ goto _LL1364;} _LL1364: goto _LL1365; _LL1361:( void*)( e->r=(
void*)(( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp1366))->r)); goto
_LL1359; _LL1363:{ void* _temp1376= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1370); struct Cyc_Absyn_Exp* _temp1382; _LL1378: if( _temp1376 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1379;} else{ goto _LL1380;} _LL1380: if((
unsigned int) _temp1376 >  1u?*(( int*) _temp1376) ==  Cyc_Absyn_Upper_b: 0){
_LL1383: _temp1382=(( struct Cyc_Absyn_Upper_b_struct*) _temp1376)->f1; goto
_LL1381;} else{ goto _LL1377;} _LL1379:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1384[ 2u]; _temp1384[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1374), 0); _temp1384[ 0u]=(
struct Cyc_Absyn_Exp*) _temp1174->hd;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1384, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0))->r)); goto _LL1377; _LL1381: if((( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1372)){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_conditional_exp(
arg, _temp1382, Cyc_Absyn_uint_exp( 0, 0), 0))->r));} else{( void*)( e->r=( void*)((
void*) _temp1382->r)); goto _LL1377;} _LL1377:;} goto _LL1359; _LL1365:({ struct
Cyc_Std_String_pa_struct _temp1387; _temp1387.tag= Cyc_Std_String_pa; _temp1387.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v);{ struct Cyc_Std_String_pa_struct _temp1386;
_temp1386.tag= Cyc_Std_String_pa; _temp1386.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ void* _temp1385[ 2u]={&
_temp1386,& _temp1387};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)", sizeof(
unsigned char), 41u), _tag_arr( _temp1385, sizeof( void*), 2u));}}}); return;
_LL1359:;} goto _LL1337;} _LL1341: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1335))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1174))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1174->tl))->hd;(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1388[ 3u]; _temp1388[ 2u]= e2; _temp1388[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1388[ 0u]= e1;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1388, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1337;} _LL1343: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1335))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1174))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1174->tl))->hd;
if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_List_List*)
_check_null( _temp1335->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1389[ 3u]; _temp1389[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1389[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1389[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1389, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1337;} _LL1345: goto _LL1347; _LL1347: goto _LL1349; _LL1349: goto
_LL1351; _LL1351: goto _LL1353; _LL1353: goto _LL1355; _LL1355: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1174))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1174->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp1335))->hd; void* t2=( void*)(( struct Cyc_List_List*)
_check_null( _temp1335->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1337;} _LL1357: goto
_LL1337; _LL1337:;} goto _LL1081;} _LL1093: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1180->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1180);{ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1390=( _temp1178 == ( void*) Cyc_Absyn_PostInc?
1: _temp1178 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1391=( _temp1178 == ( void*) Cyc_Absyn_PreInc?
1: _temp1178 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1390,({ struct Cyc_Absyn_Exp* _temp1392[ 3u];
_temp1392[ 2u]= Cyc_Absyn_signed_int_exp( _temp1391, 0); _temp1392[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1392[ 0u]= Cyc_Absyn_address_exp( _temp1180,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1392, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp1180): 0){ Cyc_Toc_lvalue_assign(
_temp1180, 0, Cyc_Toc_incr_lvalue, _temp1178);( void*)( e->r=( void*)(( void*)
_temp1180->r));} goto _LL1081;}} _LL1095: { int e1_poly= Cyc_Toc_is_poly_project(
_temp1186); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1186->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1182->topt))->v; Cyc_Toc_exp_to_c( nv, _temp1186); Cyc_Toc_exp_to_c(
nv, _temp1182);{ int done= 0; if( _temp1184 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1393=( void*) _temp1184->v; _LL1395: if( _temp1393 == ( void*)
Cyc_Absyn_Plus){ goto _LL1396;} else{ goto _LL1397;} _LL1397: if( _temp1393 == (
void*) Cyc_Absyn_Minus){ goto _LL1398;} else{ goto _LL1399;} _LL1399: goto
_LL1400; _LL1396: change= _temp1182; goto _LL1394; _LL1398: change= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, _temp1182, 0); goto _LL1394; _LL1400:( int) _throw(({
void* _temp1401[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u), _tag_arr( _temp1401, sizeof( void*), 0u));}));
goto _LL1394; _LL1394:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_Absyn_Exp* _temp1402[ 3u];
_temp1402[ 2u]= change; _temp1402[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1402[ 0u]= Cyc_Absyn_address_exp( _temp1186, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1402, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp1182->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1182->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1186)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)(
_temp1186, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1403=( struct
_tuple7*) _cycalloc( sizeof( struct _tuple7) *  1); _temp1403[ 0]=({ struct
_tuple7 _temp1404; _temp1404.f1= _temp1184; _temp1404.f2= _temp1182; _temp1404;});
_temp1403;}));( void*)( e->r=( void*)(( void*) _temp1186->r));}} goto _LL1081;}}
_LL1097: Cyc_Toc_exp_to_c( nv, _temp1192); Cyc_Toc_exp_to_c( nv, _temp1190); Cyc_Toc_exp_to_c(
nv, _temp1188); goto _LL1081; _LL1099: Cyc_Toc_exp_to_c( nv, _temp1196); Cyc_Toc_exp_to_c(
nv, _temp1194); goto _LL1081; _LL1101: _temp1206= _temp1200; _temp1204=
_temp1198; goto _LL1103; _LL1103: Cyc_Toc_exp_to_c( nv, _temp1206);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1204);
goto _LL1081; _LL1105:{ struct _RegionHandle _temp1405= _new_region("r"); struct
_RegionHandle* r=& _temp1405; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp1215, 0); void*
cva_type= Cyc_Toc_typ_to_c(( void*) _temp1211->type); void* arr_type=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1422=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1422[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1423; _temp1423.tag= Cyc_Absyn_ArrayType;
_temp1423.f1=( void*) cva_type; _temp1423.f2= Cyc_Toc_mt_tq; _temp1423.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1423;}); _temp1422;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1217); int num_normargs=
num_args -  _temp1215; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp1217= _temp1217->tl)){ Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1217))->hd); new_args=({
struct Cyc_List_List* _temp1406=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1406->hd=( void*)(( struct Cyc_Absyn_Exp*)
_temp1217->hd); _temp1406->tl= new_args; _temp1406;});}} new_args=({ struct Cyc_List_List*
_temp1407=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1407->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1408[ 3u]; _temp1408[ 2u]= num_varargs_exp; _temp1408[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1408[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1408, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1407->tl= new_args; _temp1407;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp1219);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp1219, new_args, 0), 0); if( _temp1211->inject){ struct _tagged_arr vs=({
unsigned int _temp1418=( unsigned int) _temp1215; struct _tuple0** _temp1419=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1418)); struct _tagged_arr _temp1421= _tag_arr( _temp1419, sizeof( struct
_tuple0*),( unsigned int) _temp1215);{ unsigned int _temp1420= _temp1418;
unsigned int i; for( i= 0; i <  _temp1420; i ++){ _temp1419[ i]= Cyc_Toc_temp_var();}};
_temp1421;}); struct Cyc_List_List* _temp1409= 0;{ int i= _temp1215 -  1; for( 0;
i >=  0; -- i){ _temp1409=({ struct Cyc_List_List* _temp1410=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1410->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp((( struct _tuple0**) vs.curr)[ i], 0),
0)); _temp1410->tl= _temp1409; _temp1410;});}} s= Cyc_Absyn_declare_stmt( argv,
arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1409, 0),
s, 0);{ int i= 0; for( 0; _temp1217 !=  0;((( _temp1217= _temp1217->tl,
_temp1213= _temp1213->tl)), ++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)
_temp1217->hd; void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct _tuple0* var=(( struct _tuple0**) vs.curr)[ i]; struct Cyc_Absyn_Exp*
varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield _temp1413;
struct Cyc_List_List* _temp1414; struct _tuple0* _temp1416; struct Cyc_Absyn_Tunionfield*
_temp1411=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1213))->hd; _temp1413=* _temp1411; _LL1417: _temp1416= _temp1413.name; goto
_LL1415; _LL1415: _temp1414= _temp1413.typs; goto _LL1412; _LL1412: { void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1414))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Absyn_fieldname(
1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1416, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1416, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp1217
!=  0;( _temp1217= _temp1217->tl, ++ i)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_temp1217->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)
_temp1217->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt( argv, arr_type, 0, s, 0);}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}}; _pop_region(
r);} goto _LL1081; _LL1107: Cyc_Toc_exp_to_c( nv, _temp1221);( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp(
_temp1221), 0))->r)); goto _LL1081; _LL1109: Cyc_Toc_exp_to_c( nv, _temp1223);
goto _LL1081; _LL1111: Cyc_Toc_exp_to_c( nv, _temp1227); for( 0; _temp1225 !=  0;
_temp1225= _temp1225->tl){ void* k= Cyc_Tcutil_typ_kind(( void*) _temp1225->hd);
if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind: 0){{ void*
_temp1424= Cyc_Tcutil_compress(( void*) _temp1225->hd); _LL1426: if((
unsigned int) _temp1424 >  3u?*(( int*) _temp1424) ==  Cyc_Absyn_VarType: 0){
goto _LL1427;} else{ goto _LL1428;} _LL1428: if(( unsigned int) _temp1424 >  3u?*((
int*) _temp1424) ==  Cyc_Absyn_TunionType: 0){ goto _LL1429;} else{ goto _LL1430;}
_LL1430: goto _LL1431; _LL1427: continue; _LL1429: continue; _LL1431:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), _temp1227, 0))->r)); goto _LL1425;
_LL1425:;} break;}} goto _LL1081; _LL1113: { void* old_t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1229->topt))->v; void* new_typ=* _temp1233;* _temp1233= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv, _temp1229);{ struct _tuple8 _temp1433=({ struct
_tuple8 _temp1432; _temp1432.f1= Cyc_Tcutil_compress( old_t2); _temp1432.f2= Cyc_Tcutil_compress(
new_typ); _temp1432;}); void* _temp1441; struct Cyc_Absyn_PtrInfo _temp1443;
void* _temp1445; struct Cyc_Absyn_PtrInfo _temp1447; void* _temp1449; void*
_temp1451; struct Cyc_Absyn_PtrInfo _temp1453; _LL1435: _LL1446: _temp1445=
_temp1433.f1; if(( unsigned int) _temp1445 >  3u?*(( int*) _temp1445) ==  Cyc_Absyn_PointerType:
0){ _LL1448: _temp1447=(( struct Cyc_Absyn_PointerType_struct*) _temp1445)->f1;
goto _LL1442;} else{ goto _LL1437;} _LL1442: _temp1441= _temp1433.f2; if((
unsigned int) _temp1441 >  3u?*(( int*) _temp1441) ==  Cyc_Absyn_PointerType: 0){
_LL1444: _temp1443=(( struct Cyc_Absyn_PointerType_struct*) _temp1441)->f1; goto
_LL1436;} else{ goto _LL1437;} _LL1437: _LL1452: _temp1451= _temp1433.f1; if((
unsigned int) _temp1451 >  3u?*(( int*) _temp1451) ==  Cyc_Absyn_PointerType: 0){
_LL1454: _temp1453=(( struct Cyc_Absyn_PointerType_struct*) _temp1451)->f1; goto
_LL1450;} else{ goto _LL1439;} _LL1450: _temp1449= _temp1433.f2; if((
unsigned int) _temp1449 >  3u?*(( int*) _temp1449) ==  Cyc_Absyn_IntType: 0){
goto _LL1438;} else{ goto _LL1439;} _LL1439: goto _LL1440; _LL1436: { int
_temp1455=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1447.nullable); int _temp1456=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1443.nullable); void* _temp1457= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1447.bounds); void* _temp1458= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1443.bounds);{ struct _tuple8 _temp1460=({ struct
_tuple8 _temp1459; _temp1459.f1= _temp1457; _temp1459.f2= _temp1458; _temp1459;});
void* _temp1470; void* _temp1472; void* _temp1474; void* _temp1476; struct Cyc_Absyn_Exp*
_temp1478; void* _temp1480; struct Cyc_Absyn_Exp* _temp1482; void* _temp1484;
_LL1462: _LL1473: _temp1472= _temp1460.f1; if(( unsigned int) _temp1472 >  1u?*((
int*) _temp1472) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1471;} else{ goto _LL1464;}
_LL1471: _temp1470= _temp1460.f2; if(( unsigned int) _temp1470 >  1u?*(( int*)
_temp1470) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1463;} else{ goto _LL1464;}
_LL1464: _LL1477: _temp1476= _temp1460.f1; if(( unsigned int) _temp1476 >  1u?*((
int*) _temp1476) ==  Cyc_Absyn_Upper_b: 0){ _LL1479: _temp1478=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1476)->f1; goto _LL1475;} else{ goto _LL1466;} _LL1475: _temp1474=
_temp1460.f2; if( _temp1474 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1465;}
else{ goto _LL1466;} _LL1466: _LL1485: _temp1484= _temp1460.f1; if( _temp1484 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1481;} else{ goto _LL1468;} _LL1481:
_temp1480= _temp1460.f2; if(( unsigned int) _temp1480 >  1u?*(( int*) _temp1480)
==  Cyc_Absyn_Upper_b: 0){ _LL1483: _temp1482=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1480)->f1; goto _LL1467;} else{ goto _LL1468;} _LL1468: goto _LL1469;
_LL1463: if( _temp1455? ! _temp1456: 0){ if( nv->toplevel){({ void* _temp1486[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1486, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1233, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1487=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1487->hd=( void*) _temp1229;
_temp1487->tl= 0; _temp1487;}), 0), 0))->r));} goto _LL1461; _LL1465: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1478, _temp1229))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1488[ 3u]; _temp1488[ 2u]=
_temp1478; _temp1488[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1443.elt_typ), 0); _temp1488[ 0u]= _temp1229;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1488, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1461; _LL1467: if( nv->toplevel){({ void* _temp1489[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1489, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1490= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1492[ 3u]; _temp1492[ 2u]= _temp1482; _temp1492[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1447.elt_typ), 0); _temp1492[ 0u]= _temp1229;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1492, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1456){( void*)(
_temp1490->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1491=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1491->hd=( void*) Cyc_Absyn_copy_exp( _temp1490);
_temp1491->tl= 0; _temp1491;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1233, _temp1490, 0))->r)); goto _LL1461;} _LL1469: goto _LL1461; _LL1461:;}
goto _LL1434;} _LL1438:{ void* _temp1493= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1453.bounds); _LL1495: if( _temp1493 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1496;} else{ goto _LL1497;} _LL1497: goto _LL1498; _LL1496:( void*)(
_temp1229->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1229->r, _temp1229->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1494;
_LL1498: goto _LL1494; _LL1494:;} goto _LL1434; _LL1440: goto _LL1434; _LL1434:;}
goto _LL1081;} _LL1115:{ void* _temp1499=( void*) _temp1234->r; struct Cyc_List_List*
_temp1507; struct _tuple0* _temp1509; struct Cyc_List_List* _temp1511; _LL1501:
if(*(( int*) _temp1499) ==  Cyc_Absyn_Struct_e){ _LL1510: _temp1509=(( struct
Cyc_Absyn_Struct_e_struct*) _temp1499)->f1; goto _LL1508; _LL1508: _temp1507=((
struct Cyc_Absyn_Struct_e_struct*) _temp1499)->f3; goto _LL1502;} else{ goto
_LL1503;} _LL1503: if(*(( int*) _temp1499) ==  Cyc_Absyn_Tuple_e){ _LL1512:
_temp1511=(( struct Cyc_Absyn_Tuple_e_struct*) _temp1499)->f1; goto _LL1504;}
else{ goto _LL1505;} _LL1505: goto _LL1506; _LL1502: if( nv->toplevel){({ struct
Cyc_Std_String_pa_struct _temp1514; _temp1514.tag= Cyc_Std_String_pa; _temp1514.f1=(
struct _tagged_arr) Cyc_Position_string_of_segment( _temp1234->loc);{ void*
_temp1513[ 1u]={& _temp1514};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1513, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1234->topt))->v, 1, 0, _temp1507, _temp1509))->r)); goto
_LL1500; _LL1504: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1516;
_temp1516.tag= Cyc_Std_String_pa; _temp1516.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1234->loc);{ void* _temp1515[ 1u]={& _temp1516};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1515, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1511))->r));
goto _LL1500; _LL1506: Cyc_Toc_exp_to_c( nv, _temp1234); if( ! Cyc_Absyn_is_lvalue(
_temp1234)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp1234, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1234, 0))->r));} goto _LL1500; _LL1500:;} goto _LL1081; _LL1117: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1518; _temp1518.tag= Cyc_Std_String_pa;
_temp1518.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp1236->loc);{
void* _temp1517[ 1u]={& _temp1518};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1517, sizeof( void*), 1u));}});}{ void*
_temp1519=( void*) _temp1236->r; struct Cyc_List_List* _temp1531; struct Cyc_Absyn_Exp*
_temp1533; struct Cyc_Absyn_Exp* _temp1535; struct Cyc_Absyn_Vardecl* _temp1537;
struct Cyc_List_List* _temp1539; struct _tuple0* _temp1541; struct Cyc_List_List*
_temp1543; _LL1521: if(*(( int*) _temp1519) ==  Cyc_Absyn_Array_e){ _LL1532:
_temp1531=(( struct Cyc_Absyn_Array_e_struct*) _temp1519)->f1; goto _LL1522;}
else{ goto _LL1523;} _LL1523: if(*(( int*) _temp1519) ==  Cyc_Absyn_Comprehension_e){
_LL1538: _temp1537=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1519)->f1;
goto _LL1536; _LL1536: _temp1535=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1519)->f2; goto _LL1534; _LL1534: _temp1533=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1519)->f3; goto _LL1524;} else{ goto _LL1525;} _LL1525: if(*(( int*)
_temp1519) ==  Cyc_Absyn_Struct_e){ _LL1542: _temp1541=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1519)->f1; goto _LL1540; _LL1540: _temp1539=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1519)->f3; goto _LL1526;} else{ goto _LL1527;} _LL1527: if(*(( int*)
_temp1519) ==  Cyc_Absyn_Tuple_e){ _LL1544: _temp1543=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1519)->f1; goto _LL1528;} else{ goto _LL1529;} _LL1529: goto _LL1530;
_LL1522: { struct _tuple0* _temp1545= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1546= Cyc_Absyn_var_exp( _temp1545, 0); struct Cyc_Absyn_Stmt* _temp1547=
Cyc_Toc_init_array( nv, _temp1546, _temp1531, Cyc_Absyn_exp_stmt( _temp1546, 0));
void* old_elt_typ;{ void* _temp1548= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1554; struct Cyc_Absyn_Tqual _temp1556; void* _temp1558; _LL1550: if((
unsigned int) _temp1548 >  3u?*(( int*) _temp1548) ==  Cyc_Absyn_PointerType: 0){
_LL1555: _temp1554=(( struct Cyc_Absyn_PointerType_struct*) _temp1548)->f1;
_LL1559: _temp1558=( void*) _temp1554.elt_typ; goto _LL1557; _LL1557: _temp1556=
_temp1554.tq; goto _LL1551;} else{ goto _LL1552;} _LL1552: goto _LL1553; _LL1551:
old_elt_typ= _temp1558; goto _LL1549; _LL1553: old_elt_typ=({ void* _temp1560[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1560, sizeof( void*), 0u));}); goto
_LL1549; _LL1549:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1561= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1562= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1531), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp1238 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1562);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp1238); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1562);}{
struct Cyc_Absyn_Exp* _temp1563= Cyc_Absyn_cast_exp( _temp1561, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1545,
_temp1561,( struct Cyc_Absyn_Exp*) _temp1563, _temp1547, 0), 0))->r)); goto
_LL1520;}}} _LL1524: { int is_tagged_ptr= 0;{ void* _temp1564= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1570; struct Cyc_Absyn_Conref* _temp1572;
struct Cyc_Absyn_Tqual _temp1574; void* _temp1576; _LL1566: if(( unsigned int)
_temp1564 >  3u?*(( int*) _temp1564) ==  Cyc_Absyn_PointerType: 0){ _LL1571:
_temp1570=(( struct Cyc_Absyn_PointerType_struct*) _temp1564)->f1; _LL1577:
_temp1576=( void*) _temp1570.elt_typ; goto _LL1575; _LL1575: _temp1574=
_temp1570.tq; goto _LL1573; _LL1573: _temp1572= _temp1570.bounds; goto _LL1567;}
else{ goto _LL1568;} _LL1568: goto _LL1569; _LL1567: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1572) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1565;
_LL1569:({ void* _temp1578[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1578, sizeof( void*), 0u));}); goto
_LL1565; _LL1565:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1533->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1535);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1537, Cyc_Absyn_var_exp( max, 0), _temp1533, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1579= _new_region("r"); struct _RegionHandle* r=&
_temp1579; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1599=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1599->hd=( void*)({ struct _tuple9* _temp1600=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1600->f1= max; _temp1600->f2=
Cyc_Absyn_uint_t; _temp1600->f3=( struct Cyc_Absyn_Exp*) _temp1535; _temp1600;});
_temp1599->tl= 0; _temp1599;}); struct Cyc_Absyn_Exp* ai; if( _temp1238 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1580[ 2u]; _temp1580[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1580[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1580, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1238); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1581[
2u]; _temp1581[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1581[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1581, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1582=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1582->hd=( void*)({ struct _tuple9* _temp1583=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1583->f1= a; _temp1583->f2=
ptr_typ; _temp1583->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1583;}); _temp1582->tl=
decls; _temp1582;}); if( is_tagged_ptr){ struct _tuple0* _temp1584= Cyc_Toc_temp_var();
void* _temp1585= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1586=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1589[ 3u];
_temp1589[ 2u]= _temp1535; _temp1589[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1589[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1589, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1587=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1587->hd=( void*)({
struct _tuple9* _temp1588=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1588->f1= _temp1584; _temp1588->f2= _temp1585; _temp1588->f3=(
struct Cyc_Absyn_Exp*) _temp1586; _temp1588;}); _temp1587->tl= decls; _temp1587;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1584, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1590= decls; for( 0; _temp1590 !=  0;
_temp1590= _temp1590->tl){ struct Cyc_Absyn_Exp* _temp1593; void* _temp1595;
struct _tuple0* _temp1597; struct _tuple9 _temp1591=*(( struct _tuple9*)
_temp1590->hd); _LL1598: _temp1597= _temp1591.f1; goto _LL1596; _LL1596:
_temp1595= _temp1591.f2; goto _LL1594; _LL1594: _temp1593= _temp1591.f3; goto
_LL1592; _LL1592: s= Cyc_Absyn_declare_stmt( _temp1597, _temp1595, _temp1593, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region( r);} goto _LL1520;}}} _LL1526:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1236->topt))->v, 1,
_temp1238, _temp1539, _temp1541))->r)); goto _LL1520; _LL1528:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1238, _temp1543))->r)); goto
_LL1520; _LL1530: { void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1236->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); if( _temp1238
==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1238); Cyc_Toc_exp_to_c( nv, r);
mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1601=( void*)
_temp1236->r; struct Cyc_Absyn_Exp* _temp1607; void* _temp1609; _LL1603: if(*((
int*) _temp1601) ==  Cyc_Absyn_Cast_e){ _LL1610: _temp1609=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1601)->f1; goto _LL1608; _LL1608: _temp1607=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1601)->f2; goto _LL1604;} else{ goto _LL1605;} _LL1605: goto _LL1606;
_LL1604:{ struct _tuple8 _temp1612=({ struct _tuple8 _temp1611; _temp1611.f1=
Cyc_Tcutil_compress( _temp1609); _temp1611.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1607->topt))->v); _temp1611;}); void*
_temp1618; struct Cyc_Absyn_PtrInfo _temp1620; struct Cyc_Absyn_Conref*
_temp1622; void* _temp1624; struct Cyc_Absyn_PtrInfo _temp1626; struct Cyc_Absyn_Conref*
_temp1628; void* _temp1630; _LL1614: _LL1625: _temp1624= _temp1612.f1; if((
unsigned int) _temp1624 >  3u?*(( int*) _temp1624) ==  Cyc_Absyn_PointerType: 0){
_LL1627: _temp1626=(( struct Cyc_Absyn_PointerType_struct*) _temp1624)->f1;
_LL1631: _temp1630=( void*) _temp1626.elt_typ; goto _LL1629; _LL1629: _temp1628=
_temp1626.bounds; goto _LL1619;} else{ goto _LL1616;} _LL1619: _temp1618=
_temp1612.f2; if(( unsigned int) _temp1618 >  3u?*(( int*) _temp1618) ==  Cyc_Absyn_PointerType:
0){ _LL1621: _temp1620=(( struct Cyc_Absyn_PointerType_struct*) _temp1618)->f1;
_LL1623: _temp1622= _temp1620.bounds; goto _LL1615;} else{ goto _LL1616;}
_LL1616: goto _LL1617; _LL1615:{ struct _tuple8 _temp1633=({ struct _tuple8
_temp1632; _temp1632.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1628);
_temp1632.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1622); _temp1632;});
void* _temp1639; struct Cyc_Absyn_Exp* _temp1641; void* _temp1643; _LL1635:
_LL1644: _temp1643= _temp1633.f1; if( _temp1643 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1640;} else{ goto _LL1637;} _LL1640: _temp1639= _temp1633.f2; if((
unsigned int) _temp1639 >  1u?*(( int*) _temp1639) ==  Cyc_Absyn_Upper_b: 0){
_LL1642: _temp1641=(( struct Cyc_Absyn_Upper_b_struct*) _temp1639)->f1; goto
_LL1636;} else{ goto _LL1637;} _LL1637: goto _LL1638; _LL1636: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1645[ 4u]; _temp1645[ 3u]= _temp1641; _temp1645[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1630, 0); _temp1645[ 1u]= _temp1607; _temp1645[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1645, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1634; _LL1638: goto _LL1634; _LL1634:;} goto _LL1613;
_LL1617: goto _LL1613; _LL1613:;} goto _LL1602; _LL1606: goto _LL1602; _LL1602:;}
if( ! done){ struct _tuple0* _temp1646= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1647= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1646, 0), 0); struct Cyc_Absyn_Exp*
_temp1648= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp1236);
_temp1647= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1646, 0), _temp1648, 0), _temp1236, 0), _temp1647, 0);{
void* _temp1649= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1646,
_temp1649,( struct Cyc_Absyn_Exp*) mexp, _temp1647, 0), 0))->r));}} goto _LL1520;}}
_LL1520:;} goto _LL1081; _LL1119: Cyc_Toc_exp_to_c( nv, _temp1240); goto _LL1081;
_LL1121:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1650=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1650[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1651; _temp1651.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1651.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1242);
_temp1651;}); _temp1650;}))); goto _LL1081; _LL1123:( int) _throw(({ void*
_temp1652[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("__gen() in code generator",
sizeof( unsigned char), 26u), _tag_arr( _temp1652, sizeof( void*), 0u));}));
_LL1125:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1653=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1653[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1654; _temp1654.tag=
Cyc_Absyn_Offsetof_e; _temp1654.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1248);
_temp1654.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1655=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1655[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1656; _temp1656.tag=
Cyc_Absyn_StructField; _temp1656.f1= _temp1246; _temp1656;}); _temp1655;}));
_temp1654;}); _temp1653;}))); goto _LL1081; _LL1127:{ void* _temp1657= Cyc_Tcutil_compress(
_temp1254); struct Cyc_Absyn_Structdecl** _temp1673; struct Cyc_List_List*
_temp1675; struct Cyc_Absyn_Uniondecl** _temp1677; struct Cyc_List_List*
_temp1679; _LL1659: if(( unsigned int) _temp1657 >  3u?*(( int*) _temp1657) == 
Cyc_Absyn_StructType: 0){ _LL1674: _temp1673=(( struct Cyc_Absyn_StructType_struct*)
_temp1657)->f3; goto _LL1660;} else{ goto _LL1661;} _LL1661: if(( unsigned int)
_temp1657 >  3u?*(( int*) _temp1657) ==  Cyc_Absyn_AnonStructType: 0){ _LL1676:
_temp1675=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1657)->f1; goto
_LL1662;} else{ goto _LL1663;} _LL1663: if(( unsigned int) _temp1657 >  3u?*((
int*) _temp1657) ==  Cyc_Absyn_UnionType: 0){ _LL1678: _temp1677=(( struct Cyc_Absyn_UnionType_struct*)
_temp1657)->f3; goto _LL1664;} else{ goto _LL1665;} _LL1665: if(( unsigned int)
_temp1657 >  3u?*(( int*) _temp1657) ==  Cyc_Absyn_AnonUnionType: 0){ _LL1680:
_temp1679=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1657)->f1; goto
_LL1666;} else{ goto _LL1667;} _LL1667: if(( unsigned int) _temp1657 >  3u?*((
int*) _temp1657) ==  Cyc_Absyn_TupleType: 0){ goto _LL1668;} else{ goto _LL1669;}
_LL1669: if(( unsigned int) _temp1657 >  3u?*(( int*) _temp1657) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL1670;} else{ goto _LL1671;} _LL1671: goto _LL1672; _LL1660: if(
_temp1673 ==  0? 1:(* _temp1673)->fields ==  0){( int) _throw(({ void* _temp1681[
0u]={}; Cyc_Toc_toc_impos( _tag_arr("struct fields must be known", sizeof(
unsigned char), 28u), _tag_arr( _temp1681, sizeof( void*), 0u));}));} _temp1675=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp1673)->fields))->v;
goto _LL1662; _LL1662: { struct Cyc_Absyn_Structfield* _temp1682=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1675,( int) _temp1252);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1683=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1683[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1684; _temp1684.tag=
Cyc_Absyn_Offsetof_e; _temp1684.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1254);
_temp1684.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1685=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1685[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1686; _temp1686.tag=
Cyc_Absyn_StructField; _temp1686.f1= _temp1682->name; _temp1686;}); _temp1685;}));
_temp1684;}); _temp1683;}))); goto _LL1658;} _LL1664: if( _temp1677 ==  0? 1:(*
_temp1677)->fields ==  0){( int) _throw(({ void* _temp1687[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("union fields must be known", sizeof( unsigned char), 27u), _tag_arr(
_temp1687, sizeof( void*), 0u));}));} _temp1679=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp1677)->fields))->v; goto _LL1666;
_LL1666: { struct Cyc_Absyn_Structfield* _temp1688=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1679,( int) _temp1252);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1689=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1689[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1690; _temp1690.tag=
Cyc_Absyn_Offsetof_e; _temp1690.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1254);
_temp1690.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1691=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1691[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1692; _temp1692.tag=
Cyc_Absyn_StructField; _temp1692.f1= _temp1688->name; _temp1692;}); _temp1691;}));
_temp1690;}); _temp1689;}))); goto _LL1658;} _LL1668:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1693=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1693[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1694; _temp1694.tag= Cyc_Absyn_Offsetof_e;
_temp1694.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1254); _temp1694.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1695=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1695[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1696; _temp1696.tag= Cyc_Absyn_StructField;
_temp1696.f1= Cyc_Absyn_fieldname(( int)( _temp1252 +  1)); _temp1696;});
_temp1695;})); _temp1694;}); _temp1693;}))); goto _LL1658; _LL1670: if(
_temp1252 ==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1697=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1697[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1698; _temp1698.tag=
Cyc_Absyn_Offsetof_e; _temp1698.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1254);
_temp1698.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1699=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1699[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1700; _temp1700.tag=
Cyc_Absyn_StructField; _temp1700.f1= Cyc_Toc_tag_sp; _temp1700;}); _temp1699;}));
_temp1698;}); _temp1697;})));} else{( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1701=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1701[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1702; _temp1702.tag= Cyc_Absyn_Offsetof_e;
_temp1702.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1254); _temp1702.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1703=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1703[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1704; _temp1704.tag= Cyc_Absyn_StructField;
_temp1704.f1= Cyc_Absyn_fieldname(( int) _temp1252); _temp1704;}); _temp1703;}));
_temp1702;}); _temp1701;})));} goto _LL1658; _LL1672:( int) _throw(({ void*
_temp1705[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("impossible type for offsetof tuple index",
sizeof( unsigned char), 41u), _tag_arr( _temp1705, sizeof( void*), 0u));}));
_LL1658:;} goto _LL1081; _LL1129: { void* _temp1706= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1256->topt))->v);{ void* _temp1707=
_temp1706; struct Cyc_Absyn_PtrInfo _temp1713; struct Cyc_Absyn_Conref*
_temp1715; struct Cyc_Absyn_Tqual _temp1717; struct Cyc_Absyn_Conref* _temp1719;
void* _temp1721; void* _temp1723; _LL1709: if(( unsigned int) _temp1707 >  3u?*((
int*) _temp1707) ==  Cyc_Absyn_PointerType: 0){ _LL1714: _temp1713=(( struct Cyc_Absyn_PointerType_struct*)
_temp1707)->f1; _LL1724: _temp1723=( void*) _temp1713.elt_typ; goto _LL1722;
_LL1722: _temp1721=( void*) _temp1713.rgn_typ; goto _LL1720; _LL1720: _temp1719=
_temp1713.nullable; goto _LL1718; _LL1718: _temp1717= _temp1713.tq; goto _LL1716;
_LL1716: _temp1715= _temp1713.bounds; goto _LL1710;} else{ goto _LL1711;}
_LL1711: goto _LL1712; _LL1710:{ void* _temp1725= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1715); _LL1727: if(( unsigned int) _temp1725 >  1u?*(( int*) _temp1725) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1728;} else{ goto _LL1729;} _LL1729: if(
_temp1725 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1730;} else{ goto _LL1726;}
_LL1728: { int do_null_check= Cyc_Toc_need_null_check( _temp1256); Cyc_Toc_exp_to_c(
nv, _temp1256); if( do_null_check){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1256->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1731=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1731->hd=( void*) _temp1256; _temp1731->tl= 0;
_temp1731;}), 0), 0), 0))->r));} goto _LL1726;} _LL1730: { struct Cyc_Absyn_Exp*
_temp1732= Cyc_Absyn_uint_exp( 0, 0); _temp1732->topt=({ struct Cyc_Core_Opt*
_temp1733=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1733->v=( void*) Cyc_Absyn_uint_t; _temp1733;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp1256, _temp1732, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1726;} _LL1726:;} goto _LL1708; _LL1712:({ void* _temp1734[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1734, sizeof( void*), 0u));}); goto _LL1708; _LL1708:;} goto _LL1081;}
_LL1131: Cyc_Toc_exp_to_c( nv, _temp1260); if( Cyc_Toc_is_poly_project( e)){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1081; _LL1133: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1264->topt))->v); int
do_null_check= Cyc_Toc_need_null_check( _temp1264); Cyc_Toc_exp_to_c( nv,
_temp1264);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1737; struct Cyc_Absyn_Tqual _temp1739; struct Cyc_Absyn_Conref* _temp1741;
void* _temp1743; void* _temp1745; struct Cyc_Absyn_PtrInfo _temp1735= Cyc_Toc_get_ptr_type(
e1typ); _LL1746: _temp1745=( void*) _temp1735.elt_typ; goto _LL1744; _LL1744:
_temp1743=( void*) _temp1735.rgn_typ; goto _LL1742; _LL1742: _temp1741=
_temp1735.nullable; goto _LL1740; _LL1740: _temp1739= _temp1735.tq; goto _LL1738;
_LL1738: _temp1737= _temp1735.bounds; goto _LL1736; _LL1736:{ void* _temp1747=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1737); struct Cyc_Absyn_Exp*
_temp1753; _LL1749: if(( unsigned int) _temp1747 >  1u?*(( int*) _temp1747) == 
Cyc_Absyn_Upper_b: 0){ _LL1754: _temp1753=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1747)->f1; goto _LL1750;} else{ goto _LL1751;} _LL1751: if( _temp1747 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1752;} else{ goto _LL1748;} _LL1750: if(
Cyc_Evexp_eval_const_uint_exp( _temp1753) <  1){({ void* _temp1755[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1755, sizeof( void*), 0u));});} if( do_null_check){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1264->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1756=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1756->hd=( void*) _temp1264;
_temp1756->tl= 0; _temp1756;}), 0), 0), _temp1262, 0))->r));} goto _LL1748;
_LL1752: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1745);( void*)( _temp1264->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1739), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1757[ 3u];
_temp1757[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1757[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1757[ 0u]= Cyc_Absyn_copy_exp( _temp1264);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1757, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1748;} _LL1748:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1081;}} _LL1135: { void* _temp1758= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1268->topt))->v);{ void*
_temp1759= _temp1758; struct Cyc_List_List* _temp1767; struct Cyc_Absyn_PtrInfo
_temp1769; struct Cyc_Absyn_Conref* _temp1771; struct Cyc_Absyn_Tqual _temp1773;
struct Cyc_Absyn_Conref* _temp1775; void* _temp1777; void* _temp1779; _LL1761:
if(( unsigned int) _temp1759 >  3u?*(( int*) _temp1759) ==  Cyc_Absyn_TupleType:
0){ _LL1768: _temp1767=(( struct Cyc_Absyn_TupleType_struct*) _temp1759)->f1;
goto _LL1762;} else{ goto _LL1763;} _LL1763: if(( unsigned int) _temp1759 >  3u?*((
int*) _temp1759) ==  Cyc_Absyn_PointerType: 0){ _LL1770: _temp1769=(( struct Cyc_Absyn_PointerType_struct*)
_temp1759)->f1; _LL1780: _temp1779=( void*) _temp1769.elt_typ; goto _LL1778;
_LL1778: _temp1777=( void*) _temp1769.rgn_typ; goto _LL1776; _LL1776: _temp1775=
_temp1769.nullable; goto _LL1774; _LL1774: _temp1773= _temp1769.tq; goto _LL1772;
_LL1772: _temp1771= _temp1769.bounds; goto _LL1764;} else{ goto _LL1765;}
_LL1765: goto _LL1766; _LL1762: Cyc_Toc_exp_to_c( nv, _temp1268); Cyc_Toc_exp_to_c(
nv, _temp1266);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1266) +  1;(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1268, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1760;} _LL1764: { struct Cyc_List_List* _temp1781=
Cyc_Toc_get_relns( _temp1268); int _temp1782= Cyc_Toc_check_bounds( _temp1781,
_temp1268, _temp1266); Cyc_Toc_exp_to_c( nv, _temp1268); Cyc_Toc_exp_to_c( nv,
_temp1266); Cyc_Toc_total_bounds_checks ++;{ void* _temp1783= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1771); struct Cyc_Absyn_Exp* _temp1789; _LL1785: if((
unsigned int) _temp1783 >  1u?*(( int*) _temp1783) ==  Cyc_Absyn_Upper_b: 0){
_LL1790: _temp1789=(( struct Cyc_Absyn_Upper_b_struct*) _temp1783)->f1; goto
_LL1786;} else{ goto _LL1787;} _LL1787: if( _temp1783 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1788;} else{ goto _LL1784;} _LL1786: _temp1789= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1789), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1775); void* ta1= Cyc_Toc_typ_to_c( _temp1779);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1773); if( _temp1782){ Cyc_Toc_bounds_checks_eliminated
++;} else{ if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp* _temp1791[ 4u]; _temp1791[ 3u]= _temp1266; _temp1791[ 2u]=
Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1791[ 1u]= _temp1789; _temp1791[ 0u]=
_temp1268;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1791, sizeof( struct Cyc_Absyn_Exp*), 4u));}), 0), 0), 0))->r));}
else{( void*)( _temp1266->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp* _temp1792[ 2u]; _temp1792[ 1u]= Cyc_Absyn_copy_exp(
_temp1266); _temp1792[ 0u]= _temp1789;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1792, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0))->r));}} goto _LL1784;} _LL1788: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1779); if( _temp1782){ Cyc_Toc_bounds_checks_eliminated ++;( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
ta1, _temp1773), Cyc_Absyn_structmember_exp( _temp1268, Cyc_Toc_curr_sp, 0), 0),
_temp1266, 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1773), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1793[ 3u];
_temp1793[ 2u]= _temp1266; _temp1793[ 1u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1793[ 0u]= _temp1268;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1793, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0), 0), 0))->r));}
goto _LL1784;} _LL1784:;} goto _LL1760;} _LL1766:({ void* _temp1794[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr", sizeof(
unsigned char), 49u), _tag_arr( _temp1794, sizeof( void*), 0u));}); goto _LL1760;
_LL1760:;} goto _LL1081;} _LL1137: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1270))->r));} else{ struct Cyc_List_List*
_temp1795=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1270); struct
_tagged_arr* _temp1796= Cyc_Toc_add_tuple_type( _temp1795); struct Cyc_List_List*
dles= 0;{ int i= 1; for( 0; _temp1270 !=  0;( _temp1270= _temp1270->tl, i ++)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _temp1270->hd); dles=({ struct Cyc_List_List*
_temp1797=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1797->hd=( void*)({ struct _tuple4* _temp1798=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1798->f1= 0; _temp1798->f2=( struct Cyc_Absyn_Exp*)
_temp1270->hd; _temp1798;}); _temp1797->tl= dles; _temp1797;});}} dles=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));} goto
_LL1081; _LL1139:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1272, 0))->r));{ struct Cyc_List_List* _temp1799= _temp1272; for( 0;
_temp1799 !=  0; _temp1799= _temp1799->tl){ struct _tuple4 _temp1802; struct Cyc_Absyn_Exp*
_temp1803; struct _tuple4* _temp1800=( struct _tuple4*) _temp1799->hd; _temp1802=*
_temp1800; _LL1804: _temp1803= _temp1802.f2; goto _LL1801; _LL1801: Cyc_Toc_exp_to_c(
nv, _temp1803);}} goto _LL1081; _LL1141: { unsigned int _temp1805= Cyc_Evexp_eval_const_uint_exp(
_temp1276); Cyc_Toc_exp_to_c( nv, _temp1274);{ struct Cyc_List_List* es= 0; if(
! Cyc_Toc_is_zero( _temp1274)){ unsigned int i= 0; for( 0; i <  _temp1805; i ++){
es=({ struct Cyc_List_List* _temp1806=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1806->hd=( void*)({ struct _tuple4*
_temp1807=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1807->f1=
0; _temp1807->f2= _temp1274; _temp1807;}); _temp1806->tl= es; _temp1806;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1081;}} _LL1143: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1282, _temp1286))->r));} else{ if(
_temp1280 ==  0){({ void* _temp1808[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1808, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1280); struct _RegionHandle _temp1809= _new_region("rgn"); struct
_RegionHandle* rgn=& _temp1809; _push_region( rgn);{ struct Cyc_List_List*
_temp1810=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1282,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1811= 0;{ struct Cyc_List_List*
_temp1812=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1812 !=  0; _temp1812= _temp1812->tl){ struct Cyc_List_List*
_temp1813= _temp1810; for( 0; _temp1813 !=  0; _temp1813= _temp1813->tl){ if((*((
struct _tuple10*) _temp1813->hd)).f1 == ( struct Cyc_Absyn_Structfield*)
_temp1812->hd){ struct _tuple10 _temp1816; struct Cyc_Absyn_Exp* _temp1817;
struct Cyc_Absyn_Structfield* _temp1819; struct _tuple10* _temp1814=( struct
_tuple10*) _temp1813->hd; _temp1816=* _temp1814; _LL1820: _temp1819= _temp1816.f1;
goto _LL1818; _LL1818: _temp1817= _temp1816.f2; goto _LL1815; _LL1815: { void*
_temp1821=( void*) _temp1819->type; Cyc_Toc_exp_to_c( nv, _temp1817); if( Cyc_Toc_is_void_star(
_temp1821)){( void*)( _temp1817->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1817->r, 0), 0))->r));} _temp1811=({ struct Cyc_List_List*
_temp1822=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1822->hd=( void*)({ struct _tuple4* _temp1823=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1823->f1= 0; _temp1823->f2= _temp1817; _temp1823;});
_temp1822->tl= _temp1811; _temp1822;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1811), 0))->r));}; _pop_region( rgn);}} goto _LL1081;
_LL1145: { struct Cyc_List_List* fs;{ void* _temp1824= Cyc_Tcutil_compress(
_temp1290); struct Cyc_List_List* _temp1830; _LL1826: if(( unsigned int)
_temp1824 >  3u?*(( int*) _temp1824) ==  Cyc_Absyn_AnonStructType: 0){ _LL1831:
_temp1830=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1824)->f1; goto
_LL1827;} else{ goto _LL1828;} _LL1828: goto _LL1829; _LL1827: fs= _temp1830;
goto _LL1825; _LL1829: fs=({ struct Cyc_Std_String_pa_struct _temp1833;
_temp1833.tag= Cyc_Std_String_pa; _temp1833.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1290);{ void* _temp1832[ 1u]={& _temp1833};(( struct Cyc_List_List*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("anon struct has type %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1832, sizeof( void*), 1u));}});
goto _LL1825; _LL1825:;}{ struct _RegionHandle _temp1834= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1834; _push_region( rgn);{ struct Cyc_List_List*
_temp1835=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1288, fs); for( 0; _temp1835 !=  0; _temp1835= _temp1835->tl){
struct _tuple10 _temp1838; struct Cyc_Absyn_Exp* _temp1839; struct Cyc_Absyn_Structfield*
_temp1841; struct _tuple10* _temp1836=( struct _tuple10*) _temp1835->hd;
_temp1838=* _temp1836; _LL1842: _temp1841= _temp1838.f1; goto _LL1840; _LL1840:
_temp1839= _temp1838.f2; goto _LL1837; _LL1837: { void* _temp1843=( void*)
_temp1841->type; Cyc_Toc_exp_to_c( nv, _temp1839); if( Cyc_Toc_is_void_star(
_temp1843)){( void*)( _temp1839->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1839->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1288, 0))->r));}; _pop_region( rgn);}
goto _LL1081;} _LL1147: { struct _tuple0* qv= _temp1292->name; if( _temp1294->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1844= _temp1294->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1294->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1844))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1844->hd)->typs ==  0){ tag_count ++;} _temp1844= _temp1844->tl;}( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1081;} _LL1149: { struct _tuple0* _temp1845= _temp1298->name; struct Cyc_List_List*
_temp1846= _temp1298->typs; struct _tuple0* _temp1847= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1848= Cyc_Absyn_var_exp( _temp1847, 0); void*
_temp1849= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1845, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1300->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1845, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1850= _temp1300->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1300->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1845,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1850))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1850->hd)->typs !=  0){ tag_count ++;} _temp1850= _temp1850->tl;} tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles=
0; for( 0; _temp1302 !=  0;( _temp1302= _temp1302->tl, _temp1846= _temp1846->tl)){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1302->hd; void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1846))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1851=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1851->hd=( void*)({ struct _tuple4* _temp1852=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1852->f1= 0; _temp1852->f2= cur_e; _temp1852;});
_temp1851->tl= dles; _temp1851;});} dles=({ struct Cyc_List_List* _temp1853=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1853->hd=(
void*)({ struct _tuple4* _temp1854=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1854->f1= 0; _temp1854->f2= tag_exp; _temp1854;}); _temp1853->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1853;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1855=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1855->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1845,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1855;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1856=({ struct Cyc_List_List* _temp1862=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1862->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1848, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1845, 0), 0); _temp1862->tl= 0; _temp1862;});{ int i=
1; for( 0; _temp1302 !=  0;((( _temp1302= _temp1302->tl, i ++)), _temp1846=
_temp1846->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1302->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1846))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1857= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1848, Cyc_Absyn_fieldname(
i), 0), cur_e, 0); _temp1856=({ struct Cyc_List_List* _temp1858=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1858->hd=( void*) _temp1857;
_temp1858->tl= _temp1856; _temp1858;});}}}{ struct Cyc_Absyn_Stmt* _temp1859=
Cyc_Absyn_exp_stmt( _temp1848, 0); struct Cyc_Absyn_Stmt* _temp1860= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1861=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1861->hd=( void*) _temp1859; _temp1861->tl= _temp1856; _temp1861;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1847, _temp1849, 0, _temp1860, 0), 0))->r));}} goto _LL1081;} _LL1151: goto
_LL1081; _LL1153: goto _LL1081; _LL1155: { void* t_c= Cyc_Toc_typ_to_c(*(( void**)
_check_null( _temp1310))); Cyc_Toc_exp_to_c( nv, _temp1308); if( _temp1306){
struct _tuple0* _temp1863= Cyc_Toc_temp_var(); struct _tuple0* _temp1864= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* pexp; struct Cyc_Absyn_Exp* xexp; struct Cyc_Absyn_Exp*
rexp; if( _temp1314){ xexp= _temp1308; if( _temp1312 !=  0){ struct Cyc_Absyn_Exp*
rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1312); Cyc_Toc_exp_to_c( nv, rgn);
pexp= Cyc_Toc_rcalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0), Cyc_Absyn_var_exp(
_temp1863, 0));} else{ pexp= Cyc_Toc_calloc_exp(* _temp1310, Cyc_Absyn_sizeoftyp_exp(
t_c, 0), Cyc_Absyn_var_exp( _temp1863, 0));} rexp= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp* _temp1865[ 3u]; _temp1865[ 2u]= Cyc_Absyn_var_exp(
_temp1863, 0); _temp1865[ 1u]= Cyc_Absyn_sizeoftyp_exp( t_c, 0); _temp1865[ 0u]=
Cyc_Absyn_var_exp( _temp1864, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1865, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0);}
else{ xexp= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( t_c, 0), _temp1308, 0);
if( _temp1312 !=  0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)
_check_null( _temp1312); Cyc_Toc_exp_to_c( nv, rgn); pexp= Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_var_exp( _temp1863, 0));} else{ pexp= Cyc_Toc_malloc_exp(*
_temp1310, Cyc_Absyn_var_exp( _temp1863, 0));} rexp= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp* _temp1866[ 3u]; _temp1866[ 2u]= Cyc_Absyn_var_exp(
_temp1863, 0); _temp1866[ 1u]= Cyc_Absyn_uint_exp( 1, 0); _temp1866[ 0u]= Cyc_Absyn_var_exp(
_temp1864, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1866, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0);}{ struct Cyc_Absyn_Stmt*
_temp1867= Cyc_Absyn_declare_stmt( _temp1863, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
xexp, Cyc_Absyn_declare_stmt( _temp1864, Cyc_Absyn_cstar_typ( t_c, Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*) pexp, Cyc_Absyn_exp_stmt( rexp, 0), 0), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1867, 0))->r));}} else{ if( _temp1312
!=  0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null(
_temp1312); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp(* _temp1310, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}}
goto _LL1081;} _LL1157: Cyc_Toc_stmt_to_c( nv, _temp1316); goto _LL1081; _LL1159:({
void* _temp1868[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem", sizeof( unsigned char), 14u),
_tag_arr( _temp1868, sizeof( void*), 0u));}); goto _LL1081; _LL1161:({ void*
_temp1869[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("compoundlit", sizeof( unsigned char), 12u), _tag_arr( _temp1869,
sizeof( void*), 0u));}); goto _LL1081; _LL1163:({ void* _temp1870[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen",
sizeof( unsigned char), 8u), _tag_arr( _temp1870, sizeof( void*), 0u));}); goto
_LL1081; _LL1165:({ void* _temp1871[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("fill", sizeof( unsigned char),
5u), _tag_arr( _temp1871, sizeof( void*), 0u));}); goto _LL1081; _LL1081:;}}
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
s;{ void* _temp1872=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp1910; struct
Cyc_Absyn_Vardecl _temp1912; struct _tuple0* _temp1913; struct Cyc_Absyn_Vardecl*
_temp1915; int _temp1917; void* _temp1919; unsigned char _temp1921; struct
_tagged_arr _temp1923; struct Cyc_Absyn_Enumfield* _temp1925; struct Cyc_Absyn_Enumdecl*
_temp1927; struct Cyc_Absyn_Enumfield* _temp1929; void* _temp1931; struct Cyc_List_List*
_temp1933; struct Cyc_List_List* _temp1935; struct Cyc_Absyn_Tunionfield*
_temp1937; struct Cyc_Absyn_Tuniondecl* _temp1939; struct Cyc_Absyn_Pat*
_temp1942; struct Cyc_Absyn_Pat _temp1944; void* _temp1945; struct Cyc_List_List*
_temp1947; struct Cyc_List_List* _temp1949; struct Cyc_Absyn_Tunionfield*
_temp1951; struct Cyc_Absyn_Tuniondecl* _temp1953; struct Cyc_List_List*
_temp1955; struct Cyc_List_List* _temp1957; struct Cyc_List_List* _temp1959;
struct Cyc_List_List* _temp1961; struct Cyc_Core_Opt* _temp1963; struct Cyc_Absyn_Structdecl*
_temp1965; struct Cyc_Absyn_Pat* _temp1967; _LL1874: if(( unsigned int)
_temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_Var_p: 0){ _LL1911: _temp1910=((
struct Cyc_Absyn_Var_p_struct*) _temp1872)->f1; _temp1912=* _temp1910; _LL1914:
_temp1913= _temp1912.name; goto _LL1875;} else{ goto _LL1876;} _LL1876: if(
_temp1872 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1877;} else{ goto _LL1878;}
_LL1878: if(( unsigned int) _temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_Reference_p:
0){ _LL1916: _temp1915=(( struct Cyc_Absyn_Reference_p_struct*) _temp1872)->f1;
goto _LL1879;} else{ goto _LL1880;} _LL1880: if( _temp1872 == ( void*) Cyc_Absyn_Null_p){
goto _LL1881;} else{ goto _LL1882;} _LL1882: if(( unsigned int) _temp1872 >  2u?*((
int*) _temp1872) ==  Cyc_Absyn_Int_p: 0){ _LL1920: _temp1919=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1872)->f1; goto _LL1918; _LL1918: _temp1917=((
struct Cyc_Absyn_Int_p_struct*) _temp1872)->f2; goto _LL1883;} else{ goto
_LL1884;} _LL1884: if(( unsigned int) _temp1872 >  2u?*(( int*) _temp1872) == 
Cyc_Absyn_Char_p: 0){ _LL1922: _temp1921=(( struct Cyc_Absyn_Char_p_struct*)
_temp1872)->f1; goto _LL1885;} else{ goto _LL1886;} _LL1886: if(( unsigned int)
_temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_Float_p: 0){ _LL1924:
_temp1923=(( struct Cyc_Absyn_Float_p_struct*) _temp1872)->f1; goto _LL1887;}
else{ goto _LL1888;} _LL1888: if(( unsigned int) _temp1872 >  2u?*(( int*)
_temp1872) ==  Cyc_Absyn_Enum_p: 0){ _LL1928: _temp1927=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1872)->f1; goto _LL1926; _LL1926: _temp1925=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1872)->f2; goto _LL1889;} else{ goto _LL1890;} _LL1890: if(( unsigned int)
_temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1932:
_temp1931=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1872)->f1; goto
_LL1930; _LL1930: _temp1929=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1872)->f2;
goto _LL1891;} else{ goto _LL1892;} _LL1892: if(( unsigned int) _temp1872 >  2u?*((
int*) _temp1872) ==  Cyc_Absyn_Tunion_p: 0){ _LL1940: _temp1939=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f1; goto _LL1938; _LL1938: _temp1937=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f2; goto _LL1936; _LL1936: _temp1935=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f3; goto _LL1934; _LL1934: _temp1933=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1872)->f4; if( _temp1933 ==  0){ goto _LL1893;} else{ goto _LL1894;}} else{
goto _LL1894;} _LL1894: if(( unsigned int) _temp1872 >  2u?*(( int*) _temp1872)
==  Cyc_Absyn_Pointer_p: 0){ _LL1943: _temp1942=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1872)->f1; _temp1944=* _temp1942; _LL1946: _temp1945=( void*) _temp1944.r;
if(( unsigned int) _temp1945 >  2u?*(( int*) _temp1945) ==  Cyc_Absyn_Tunion_p:
0){ _LL1954: _temp1953=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f1;
goto _LL1952; _LL1952: _temp1951=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f2;
goto _LL1950; _LL1950: _temp1949=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f3;
goto _LL1948; _LL1948: _temp1947=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f4;
goto _LL1941;} else{ goto _LL1896;}} else{ goto _LL1896;} _LL1941: if( _temp1947
!=  0){ goto _LL1895;} else{ goto _LL1896;} _LL1896: if(( unsigned int)
_temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_Tunion_p: 0){ _LL1956:
_temp1955=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1872)->f4; goto _LL1897;}
else{ goto _LL1898;} _LL1898: if(( unsigned int) _temp1872 >  2u?*(( int*)
_temp1872) ==  Cyc_Absyn_Tuple_p: 0){ _LL1958: _temp1957=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1872)->f1; goto _LL1899;} else{ goto _LL1900;} _LL1900: if(( unsigned int)
_temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_Struct_p: 0){ _LL1966:
_temp1965=(( struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f1; goto _LL1964;
_LL1964: _temp1963=(( struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f2; goto
_LL1962; _LL1962: _temp1961=(( struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f3;
goto _LL1960; _LL1960: _temp1959=(( struct Cyc_Absyn_Struct_p_struct*) _temp1872)->f4;
goto _LL1901;} else{ goto _LL1902;} _LL1902: if(( unsigned int) _temp1872 >  2u?*((
int*) _temp1872) ==  Cyc_Absyn_Pointer_p: 0){ _LL1968: _temp1967=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1872)->f1; goto _LL1903;} else{ goto _LL1904;} _LL1904: if(( unsigned int)
_temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1905;}
else{ goto _LL1906;} _LL1906: if(( unsigned int) _temp1872 >  2u?*(( int*)
_temp1872) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1907;} else{ goto _LL1908;}
_LL1908: if(( unsigned int) _temp1872 >  2u?*(( int*) _temp1872) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1909;} else{ goto _LL1873;} _LL1875: nv= Cyc_Toc_add_varmap( nv,
_temp1913, r); goto _LL1877; _LL1877: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1873; _LL1879: { struct _tuple0* _temp1969= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1970=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1970->hd=( void*)({ struct _tuple12*
_temp1971=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1971->f1= _temp1969; _temp1971->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1971;}); _temp1970->tl= decls; _temp1970;}); nv= Cyc_Toc_add_varmap(
nv, _temp1915->name, Cyc_Absyn_var_exp( _temp1969, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1969, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1873;} _LL1881: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1873; _LL1883:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1919, _temp1917, 0),
succ_lab, fail_lab); goto _LL1873; _LL1885: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1921, 0), succ_lab, fail_lab); goto _LL1873; _LL1887: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1923, 0), succ_lab, fail_lab); goto _LL1873;
_LL1889: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1972=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1972[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1973; _temp1973.tag= Cyc_Absyn_Enum_e;
_temp1973.f1= _temp1925->name; _temp1973.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1927; _temp1973.f3=( struct Cyc_Absyn_Enumfield*) _temp1925; _temp1973;});
_temp1972;}), 0), succ_lab, fail_lab); goto _LL1873; _LL1891: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_AnonEnum_e_struct* _temp1974=(
struct Cyc_Absyn_AnonEnum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_e_struct));
_temp1974[ 0]=({ struct Cyc_Absyn_AnonEnum_e_struct _temp1975; _temp1975.tag=
Cyc_Absyn_AnonEnum_e; _temp1975.f1= _temp1929->name; _temp1975.f2=( void*)
_temp1931; _temp1975.f3=( struct Cyc_Absyn_Enumfield*) _temp1929; _temp1975;});
_temp1974;}), 0), succ_lab, fail_lab); goto _LL1873; _LL1893: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1937->name, 0); if( ! _temp1939->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1873;} _LL1895: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1947);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1951->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1976=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1951->typs); struct Cyc_List_List*
_temp1977=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1947); for( 0; _temp1977 !=  0;((( _temp1977=
_temp1977->tl, _temp1976=(( struct Cyc_List_List*) _check_null( _temp1976))->tl)),
cnt --)){ struct Cyc_Absyn_Pat* _temp1978=( struct Cyc_Absyn_Pat*) _temp1977->hd;
if(( void*) _temp1978->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1979=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1976))->hd)).f2;
struct _tuple0* _temp1980= Cyc_Toc_temp_var(); void* _temp1981=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1978->topt))->v; void* _temp1982= Cyc_Toc_typ_to_c(
_temp1981); struct _tagged_arr* _temp1983= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1984= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1979))){ _temp1984= Cyc_Absyn_cast_exp(
_temp1982, _temp1984, 0);} decls=({ struct Cyc_List_List* _temp1985=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1985->hd=( void*)({
struct _tuple12* _temp1986=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1986->f1= _temp1980; _temp1986->f2= _temp1982; _temp1986;});
_temp1985->tl= decls; _temp1985;});{ struct _tuple11 _temp1987= Cyc_Toc_xlate_pat(
nv, rgn, _temp1981, Cyc_Absyn_var_exp( _temp1980, 0), _temp1984, _temp1978,
succ_lab, fail_lab, decls); nv= _temp1987.f1; decls= _temp1987.f2;{ struct Cyc_Absyn_Stmt*
_temp1988= _temp1987.f3; struct Cyc_Absyn_Stmt* _temp1989= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1980, 0), _temp1984, 0); struct Cyc_Absyn_Stmt*
_temp1990= Cyc_Absyn_seq_stmt( _temp1989, _temp1988, 0); ss=({ struct Cyc_List_List*
_temp1991=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1991->hd=( void*) Cyc_Absyn_label_stmt( _temp1983, _temp1990, 0); _temp1991->tl=
ss; _temp1991;}); succ_lab= _temp1983;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1992=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1992->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1992->tl= 0;
_temp1992;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1953->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1951->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1993=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1953->fields))->v; for( 0; _temp1993 !=  0; _temp1993=
_temp1993->tl){ struct Cyc_Absyn_Tunionfield* _temp1994=( struct Cyc_Absyn_Tunionfield*)
_temp1993->hd; if( _temp1994->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1951->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1873;}} _LL1897: _temp1957= _temp1955; goto _LL1899; _LL1899: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1957);{ struct Cyc_List_List* _temp1995=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1957); for(
0; _temp1995 !=  0;( _temp1995= _temp1995->tl, cnt --)){ struct Cyc_Absyn_Pat*
_temp1996=( struct Cyc_Absyn_Pat*) _temp1995->hd; if(( void*) _temp1996->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tuple0* _temp1997= Cyc_Toc_temp_var();
void* _temp1998=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1996->topt))->v;
struct _tagged_arr* _temp1999= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2000=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2000->hd=( void*)({ struct _tuple12* _temp2001=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp2001->f1= _temp1997;
_temp2001->f2= Cyc_Toc_typ_to_c( _temp1998); _temp2001;}); _temp2000->tl= decls;
_temp2000;});{ struct _tuple11 _temp2002= Cyc_Toc_xlate_pat( nv, rgn, _temp1998,
Cyc_Absyn_var_exp( _temp1997, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1996, succ_lab, fail_lab, decls); nv= _temp2002.f1; decls=
_temp2002.f2;{ struct Cyc_Absyn_Stmt* _temp2003= _temp2002.f3; struct Cyc_Absyn_Stmt*
_temp2004= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1997, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp2003, 0); ss=({ struct Cyc_List_List* _temp2005=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2005->hd=( void*) Cyc_Absyn_label_stmt(
_temp1999, _temp2004, 0); _temp2005->tl= ss; _temp2005;}); succ_lab= _temp1999;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp2006=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2006->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp2006->tl= 0; _temp2006;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1873;} _LL1901: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2007=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1959); for( 0; _temp2007 !=  0; _temp2007=
_temp2007->tl){ struct _tuple13* _temp2008=( struct _tuple13*) _temp2007->hd;
struct Cyc_Absyn_Pat* _temp2009=(* _temp2008).f2; if(( void*) _temp2009->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp2010=(
void*)(( struct Cyc_List_List*) _check_null((* _temp2008).f1))->hd; struct
_tagged_arr* _temp2016; _LL2012: if(*(( int*) _temp2010) ==  Cyc_Absyn_FieldName){
_LL2017: _temp2016=(( struct Cyc_Absyn_FieldName_struct*) _temp2010)->f1; goto
_LL2013;} else{ goto _LL2014;} _LL2014: goto _LL2015; _LL2013: f= _temp2016;
goto _LL2011; _LL2015:( int) _throw(( void*) Cyc_Toc_Match_error); _LL2011:;}{
struct _tuple0* _temp2018= Cyc_Toc_temp_var(); void* _temp2019=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2009->topt))->v; void* _temp2020= Cyc_Toc_typ_to_c(
_temp2019); struct _tagged_arr* _temp2021= Cyc_Toc_fresh_label(); decls=({
struct Cyc_List_List* _temp2022=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2022->hd=( void*)({ struct _tuple12*
_temp2023=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp2023->f1= _temp2018; _temp2023->f2= _temp2020; _temp2023;}); _temp2022->tl=
decls; _temp2022;});{ struct _tuple11 _temp2024= Cyc_Toc_xlate_pat( nv, rgn,
_temp2019, Cyc_Absyn_var_exp( _temp2018, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp2009, succ_lab, fail_lab, decls); nv= _temp2024.f1; decls= _temp2024.f2;{
struct Cyc_Absyn_Exp* _temp2025= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1965->fields))->v,
f)))->type)){ _temp2025= Cyc_Absyn_cast_exp( _temp2020, _temp2025, 0);}{ struct
Cyc_Absyn_Stmt* _temp2026= _temp2024.f3; struct Cyc_Absyn_Stmt* _temp2027= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2018, 0), _temp2025, 0),
_temp2026, 0); ss=({ struct Cyc_List_List* _temp2028=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2028->hd=( void*) Cyc_Absyn_label_stmt(
_temp2021, _temp2027, 0); _temp2028->tl= ss; _temp2028;}); succ_lab= _temp2021;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp2029=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2029->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp2029->tl= 0; _temp2029;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1873;} _LL1903: { struct _tuple0* _temp2030= Cyc_Toc_temp_var(); void*
_temp2031=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1967->topt))->v;
decls=({ struct Cyc_List_List* _temp2032=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2032->hd=( void*)({ struct _tuple12*
_temp2033=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp2033->f1= _temp2030; _temp2033->f2= Cyc_Toc_typ_to_c( _temp2031); _temp2033;});
_temp2032->tl= decls; _temp2032;});{ struct _tuple11 _temp2034= Cyc_Toc_xlate_pat(
nv, rgn, _temp2031, Cyc_Absyn_var_exp( _temp2030, 0), Cyc_Absyn_deref_exp( path,
0), _temp1967, succ_lab, fail_lab, decls); nv= _temp2034.f1; decls= _temp2034.f2;{
struct Cyc_Absyn_Stmt* _temp2035= _temp2034.f3; struct Cyc_Absyn_Stmt* _temp2036=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2030, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp2035, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2036, 0);} else{ s= _temp2036;} goto _LL1873;}}} _LL1905: s=({
void* _temp2037[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp2037, sizeof( void*), 0u));}); goto _LL1873;
_LL1907: s=({ void* _temp2038[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp2038, sizeof( void*), 0u));}); goto
_LL1873; _LL1909: s=({ void* _temp2039[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp2039, sizeof( void*), 0u));}); goto
_LL1873; _LL1873:;} return({ struct _tuple11 _temp2040; _temp2040.f1= nv;
_temp2040.f2= decls; _temp2040.f3= s; _temp2040;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp2041=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp2041->f1= Cyc_Toc_fresh_label(); _temp2041->f2=
Cyc_Toc_fresh_label(); _temp2041->f3= sc; _temp2041;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2042=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp2043= Cyc_Tcutil_compress( _temp2042); _LL2045: if(( unsigned int)
_temp2043 >  3u?*(( int*) _temp2043) ==  Cyc_Absyn_IntType: 0){ goto _LL2046;}
else{ goto _LL2047;} _LL2047: if(( unsigned int) _temp2043 >  3u?*(( int*)
_temp2043) ==  Cyc_Absyn_EnumType: 0){ goto _LL2048;} else{ goto _LL2049;}
_LL2049: goto _LL2050; _LL2046: goto _LL2048; _LL2048: leave_as_switch= 1; goto
_LL2044; _LL2050: leave_as_switch= 0; goto _LL2044; _LL2044:;}{ struct Cyc_List_List*
_temp2051= scs; for( 0; _temp2051 !=  0; _temp2051= _temp2051->tl){ if(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)
_temp2051->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)
_temp2051->hd)->where_clause !=  0){ leave_as_switch= 0; break;}}} if(
leave_as_switch){ struct _tagged_arr* next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List*
_temp2052= scs; for( 0; _temp2052 !=  0; _temp2052= _temp2052->tl){ struct Cyc_Absyn_Stmt*
_temp2053=(( struct Cyc_Absyn_Switch_clause*) _temp2052->hd)->body;(( struct Cyc_Absyn_Switch_clause*)
_temp2052->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2053, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2053);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct _tagged_arr*
end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2054= _new_region("rgn");
struct _RegionHandle* rgn=& _temp2054; _push_region( rgn);{ struct Cyc_List_List*
lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple14*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*), struct _RegionHandle*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label, rgn,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2055= lscs; for( 0;
_temp2055 !=  0; _temp2055= _temp2055->tl){ struct Cyc_Absyn_Switch_clause* sc=(*((
struct _tuple14*) _temp2055->hd)).f3; struct _tagged_arr* fail_lab= _temp2055->tl
==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(
_temp2055->tl))->hd)).f1; struct _tagged_arr* succ_lab=(*(( struct _tuple14*)
_temp2055->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11 _temp2056= Cyc_Toc_xlate_pat(
nv, rgn, _temp2042, r, path, sc->pattern, succ_lab, fail_lab, decls); nvs=({
struct Cyc_List_List* _temp2057=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2057->hd=( void*) _temp2056.f1; _temp2057->tl=
nvs; _temp2057;}); decls= _temp2056.f2; test_stmts=({ struct Cyc_List_List*
_temp2058=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2058->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple14*) _temp2055->hd)).f1,
_temp2056.f3, 0); _temp2058->tl= test_stmts; _temp2058;});} else{ struct Cyc_Absyn_Exp*
_temp2059=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp2060= Cyc_Toc_fresh_label(); struct _tuple11 _temp2061= Cyc_Toc_xlate_pat(
nv, rgn, _temp2042, r, path, sc->pattern, _temp2060, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp2061.f1, _temp2059); nvs=({ struct Cyc_List_List* _temp2062=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2062->hd=( void*)
_temp2061.f1; _temp2062->tl= nvs; _temp2062;}); decls= _temp2061.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2059, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2060, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2063=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2063->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*) _temp2055->hd)).f1, Cyc_Absyn_seq_stmt( _temp2061.f3, s3, 0),
0); _temp2063->tl= test_stmts; _temp2063;});}}}}{ struct Cyc_Absyn_Stmt*
_temp2064= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs !=  0){ for( 0; lscs->tl !=  0;( lscs= lscs->tl, nvs= nvs->tl)){
struct Cyc_Toc_Env* _temp2065=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp2065, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f2,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f3)->pat_vars))->v,( struct
Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null( nvs->tl))->hd), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}{ struct Cyc_Toc_Env* _temp2066=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2066, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt( _temp2064, Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for(
decls; decls !=  0; decls= decls->tl){ struct _tuple12 _temp2069; void*
_temp2070; struct _tuple0* _temp2072; struct _tuple12* _temp2067=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2069=*
_temp2067; _LL2073: _temp2072= _temp2069.f1; goto _LL2071; _LL2071: _temp2070=
_temp2069.f2; goto _LL2068; _LL2068: res= Cyc_Absyn_declare_stmt( _temp2072,
_temp2070, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp2074=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp2074[ 0]=({ struct Cyc_List_List
_temp2075; _temp2075.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp2075.tl= 0; _temp2075;}); _temp2074;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2076=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2120; struct Cyc_Absyn_Stmt* _temp2122; struct Cyc_Absyn_Stmt* _temp2124;
struct Cyc_Absyn_Exp* _temp2126; struct Cyc_Absyn_Stmt* _temp2128; struct Cyc_Absyn_Stmt*
_temp2130; struct Cyc_Absyn_Exp* _temp2132; struct Cyc_Absyn_Stmt* _temp2134;
struct _tuple2 _temp2136; struct Cyc_Absyn_Exp* _temp2138; struct Cyc_Absyn_Stmt*
_temp2140; struct Cyc_Absyn_Stmt* _temp2142; struct Cyc_Absyn_Stmt* _temp2144;
struct Cyc_Absyn_Stmt* _temp2146; struct _tuple2 _temp2148; struct Cyc_Absyn_Exp*
_temp2150; struct _tuple2 _temp2152; struct Cyc_Absyn_Exp* _temp2154; struct Cyc_Absyn_Exp*
_temp2156; struct Cyc_List_List* _temp2158; struct Cyc_Absyn_Exp* _temp2160;
struct Cyc_Absyn_Switch_clause** _temp2162; struct Cyc_List_List* _temp2164;
struct Cyc_Absyn_Stmt* _temp2166; struct Cyc_Absyn_Decl* _temp2168; struct Cyc_Absyn_Stmt*
_temp2170; struct _tagged_arr* _temp2172; struct _tuple2 _temp2174; struct Cyc_Absyn_Exp*
_temp2176; struct Cyc_Absyn_Stmt* _temp2178; struct Cyc_List_List* _temp2180;
struct Cyc_Absyn_Stmt* _temp2182; struct Cyc_Absyn_Stmt* _temp2184; struct Cyc_Absyn_Vardecl*
_temp2186; struct Cyc_Absyn_Tvar* _temp2188; _LL2078: if( _temp2076 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL2079;} else{ goto _LL2080;} _LL2080: if((
unsigned int) _temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Exp_s: 0){
_LL2121: _temp2120=(( struct Cyc_Absyn_Exp_s_struct*) _temp2076)->f1; goto
_LL2081;} else{ goto _LL2082;} _LL2082: if(( unsigned int) _temp2076 >  1u?*((
int*) _temp2076) ==  Cyc_Absyn_Seq_s: 0){ _LL2125: _temp2124=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2076)->f1; goto _LL2123; _LL2123: _temp2122=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2076)->f2; goto _LL2083;} else{ goto _LL2084;} _LL2084: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Return_s: 0){ _LL2127:
_temp2126=(( struct Cyc_Absyn_Return_s_struct*) _temp2076)->f1; goto _LL2085;}
else{ goto _LL2086;} _LL2086: if(( unsigned int) _temp2076 >  1u?*(( int*)
_temp2076) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL2133: _temp2132=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2076)->f1; goto _LL2131; _LL2131: _temp2130=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2076)->f2; goto _LL2129; _LL2129: _temp2128=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2076)->f3; goto _LL2087;} else{ goto _LL2088;} _LL2088: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_While_s: 0){ _LL2137:
_temp2136=(( struct Cyc_Absyn_While_s_struct*) _temp2076)->f1; _LL2139:
_temp2138= _temp2136.f1; goto _LL2135; _LL2135: _temp2134=(( struct Cyc_Absyn_While_s_struct*)
_temp2076)->f2; goto _LL2089;} else{ goto _LL2090;} _LL2090: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Break_s: 0){ _LL2141:
_temp2140=(( struct Cyc_Absyn_Break_s_struct*) _temp2076)->f1; goto _LL2091;}
else{ goto _LL2092;} _LL2092: if(( unsigned int) _temp2076 >  1u?*(( int*)
_temp2076) ==  Cyc_Absyn_Continue_s: 0){ _LL2143: _temp2142=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2076)->f1; goto _LL2093;} else{ goto _LL2094;} _LL2094: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Goto_s: 0){ _LL2145:
_temp2144=(( struct Cyc_Absyn_Goto_s_struct*) _temp2076)->f2; goto _LL2095;}
else{ goto _LL2096;} _LL2096: if(( unsigned int) _temp2076 >  1u?*(( int*)
_temp2076) ==  Cyc_Absyn_For_s: 0){ _LL2157: _temp2156=(( struct Cyc_Absyn_For_s_struct*)
_temp2076)->f1; goto _LL2153; _LL2153: _temp2152=(( struct Cyc_Absyn_For_s_struct*)
_temp2076)->f2; _LL2155: _temp2154= _temp2152.f1; goto _LL2149; _LL2149:
_temp2148=(( struct Cyc_Absyn_For_s_struct*) _temp2076)->f3; _LL2151: _temp2150=
_temp2148.f1; goto _LL2147; _LL2147: _temp2146=(( struct Cyc_Absyn_For_s_struct*)
_temp2076)->f4; goto _LL2097;} else{ goto _LL2098;} _LL2098: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_ForArray_s: 0){ goto _LL2099;}
else{ goto _LL2100;} _LL2100: if(( unsigned int) _temp2076 >  1u?*(( int*)
_temp2076) ==  Cyc_Absyn_Switch_s: 0){ _LL2161: _temp2160=(( struct Cyc_Absyn_Switch_s_struct*)
_temp2076)->f1; goto _LL2159; _LL2159: _temp2158=(( struct Cyc_Absyn_Switch_s_struct*)
_temp2076)->f2; goto _LL2101;} else{ goto _LL2102;} _LL2102: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Fallthru_s: 0){ _LL2165:
_temp2164=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2076)->f1; goto _LL2163;
_LL2163: _temp2162=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2076)->f2; goto
_LL2103;} else{ goto _LL2104;} _LL2104: if(( unsigned int) _temp2076 >  1u?*((
int*) _temp2076) ==  Cyc_Absyn_Decl_s: 0){ _LL2169: _temp2168=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2076)->f1; goto _LL2167; _LL2167: _temp2166=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2076)->f2; goto _LL2105;} else{ goto _LL2106;} _LL2106: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Label_s: 0){ _LL2173:
_temp2172=(( struct Cyc_Absyn_Label_s_struct*) _temp2076)->f1; goto _LL2171;
_LL2171: _temp2170=(( struct Cyc_Absyn_Label_s_struct*) _temp2076)->f2; goto
_LL2107;} else{ goto _LL2108;} _LL2108: if(( unsigned int) _temp2076 >  1u?*((
int*) _temp2076) ==  Cyc_Absyn_Do_s: 0){ _LL2179: _temp2178=(( struct Cyc_Absyn_Do_s_struct*)
_temp2076)->f1; goto _LL2175; _LL2175: _temp2174=(( struct Cyc_Absyn_Do_s_struct*)
_temp2076)->f2; _LL2177: _temp2176= _temp2174.f1; goto _LL2109;} else{ goto
_LL2110;} _LL2110: if(( unsigned int) _temp2076 >  1u?*(( int*) _temp2076) == 
Cyc_Absyn_TryCatch_s: 0){ _LL2183: _temp2182=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2076)->f1; goto _LL2181; _LL2181: _temp2180=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2076)->f2; goto _LL2111;} else{ goto _LL2112;} _LL2112: if(( unsigned int)
_temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Region_s: 0){ _LL2189:
_temp2188=(( struct Cyc_Absyn_Region_s_struct*) _temp2076)->f1; goto _LL2187;
_LL2187: _temp2186=(( struct Cyc_Absyn_Region_s_struct*) _temp2076)->f2; goto
_LL2185; _LL2185: _temp2184=(( struct Cyc_Absyn_Region_s_struct*) _temp2076)->f3;
goto _LL2113;} else{ goto _LL2114;} _LL2114: if(( unsigned int) _temp2076 >  1u?*((
int*) _temp2076) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL2115;} else{ goto _LL2116;}
_LL2116: if(( unsigned int) _temp2076 >  1u?*(( int*) _temp2076) ==  Cyc_Absyn_Cut_s:
0){ goto _LL2117;} else{ goto _LL2118;} _LL2118: if(( unsigned int) _temp2076 > 
1u?*(( int*) _temp2076) ==  Cyc_Absyn_Splice_s: 0){ goto _LL2119;} else{ goto
_LL2077;} _LL2079: return; _LL2081: Cyc_Toc_exp_to_c( nv, _temp2120); return;
_LL2083: Cyc_Toc_stmt_to_c( nv, _temp2124); s= _temp2122; continue; _LL2085: {
struct Cyc_Core_Opt* topt= 0; if( _temp2126 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp2190=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2190->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2126->topt))->v); _temp2190;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp2126));} if( s->try_depth >  0){ if( topt != 
0){ struct _tuple0* _temp2191= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp2192= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp(
_temp2191, 0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2191,( void*) topt->v, _temp2126, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2192, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2087: Cyc_Toc_exp_to_c( nv, _temp2132); Cyc_Toc_stmt_to_c( nv,
_temp2130); s= _temp2128; continue; _LL2089: Cyc_Toc_exp_to_c( nv, _temp2138);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2134); return; _LL2091: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp2140 ==  0? 0: _temp2140->try_depth; Cyc_Toc_do_npop_before(
s->try_depth -  dest_depth, s); return;} _LL2093: if( nv->continue_lab !=  0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp2144=
_temp2142; goto _LL2095; _LL2095: Cyc_Toc_do_npop_before( s->try_depth - ((
struct Cyc_Absyn_Stmt*) _check_null( _temp2144))->try_depth, s); return; _LL2097:
Cyc_Toc_exp_to_c( nv, _temp2156); Cyc_Toc_exp_to_c( nv, _temp2154); Cyc_Toc_exp_to_c(
nv, _temp2150); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2146); return;
_LL2099:({ void* _temp2193[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("forarray not yet implemented",
sizeof( unsigned char), 29u), _tag_arr( _temp2193, sizeof( void*), 0u));});
return; _LL2101: Cyc_Toc_xlate_switch( nv, s, _temp2160, _temp2158); return;
_LL2103: if( nv->fallthru_info ==  0){( int) _throw(({ void* _temp2194[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place", sizeof(
unsigned char), 29u), _tag_arr( _temp2194, sizeof( void*), 0u));}));}{ struct
_tuple6 _temp2197; struct Cyc_Dict_Dict* _temp2198; struct Cyc_List_List*
_temp2200; struct _tagged_arr* _temp2202; struct _tuple6* _temp2195=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2197=*
_temp2195; _LL2203: _temp2202= _temp2197.f1; goto _LL2201; _LL2201: _temp2200=
_temp2197.f2; goto _LL2199; _LL2199: _temp2198= _temp2197.f3; goto _LL2196;
_LL2196: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2202, 0); Cyc_Toc_do_npop_before(
s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2162)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2204=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2200); struct Cyc_List_List* _temp2205=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp2164); for( 0; _temp2204 !=  0;(
_temp2204= _temp2204->tl, _temp2205= _temp2205->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2205))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp2198,(
struct _tuple0*) _temp2204->hd),( struct Cyc_Absyn_Exp*) _temp2205->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL2105:{ void*
_temp2206=( void*) _temp2168->r; struct Cyc_Absyn_Vardecl* _temp2216; int
_temp2218; struct Cyc_Absyn_Exp* _temp2220; struct Cyc_Core_Opt* _temp2222;
struct Cyc_Absyn_Pat* _temp2224; struct Cyc_List_List* _temp2226; _LL2208: if(*((
int*) _temp2206) ==  Cyc_Absyn_Var_d){ _LL2217: _temp2216=(( struct Cyc_Absyn_Var_d_struct*)
_temp2206)->f1; goto _LL2209;} else{ goto _LL2210;} _LL2210: if(*(( int*)
_temp2206) ==  Cyc_Absyn_Let_d){ _LL2225: _temp2224=(( struct Cyc_Absyn_Let_d_struct*)
_temp2206)->f1; goto _LL2223; _LL2223: _temp2222=(( struct Cyc_Absyn_Let_d_struct*)
_temp2206)->f3; goto _LL2221; _LL2221: _temp2220=(( struct Cyc_Absyn_Let_d_struct*)
_temp2206)->f4; goto _LL2219; _LL2219: _temp2218=(( struct Cyc_Absyn_Let_d_struct*)
_temp2206)->f5; goto _LL2211;} else{ goto _LL2212;} _LL2212: if(*(( int*)
_temp2206) ==  Cyc_Absyn_Letv_d){ _LL2227: _temp2226=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2206)->f1; goto _LL2213;} else{ goto _LL2214;} _LL2214: goto _LL2215;
_LL2209: { struct Cyc_Toc_Env* _temp2228= Cyc_Toc_add_varmap( nv, _temp2216->name,
Cyc_Absyn_varb_exp( _temp2216->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2229=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2229[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2230; _temp2230.tag= Cyc_Absyn_Local_b;
_temp2230.f1= _temp2216; _temp2230;}); _temp2229;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2228, _temp2228, _temp2216, _temp2166); goto _LL2207;} _LL2211:{ void*
_temp2231=( void*) _temp2224->r; struct Cyc_Absyn_Vardecl* _temp2237; _LL2233:
if(( unsigned int) _temp2231 >  2u?*(( int*) _temp2231) ==  Cyc_Absyn_Var_p: 0){
_LL2238: _temp2237=(( struct Cyc_Absyn_Var_p_struct*) _temp2231)->f1; goto
_LL2234;} else{ goto _LL2235;} _LL2235: goto _LL2236; _LL2234: { struct _tuple0*
old_name= _temp2237->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2237->name= new_name; _temp2237->initializer=( struct Cyc_Absyn_Exp*)
_temp2220;( void*)( _temp2168->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2239=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2239[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2240; _temp2240.tag= Cyc_Absyn_Var_d;
_temp2240.f1= _temp2237; _temp2240;}); _temp2239;})));{ struct Cyc_Toc_Env*
_temp2241= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2242=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2242[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2243; _temp2243.tag= Cyc_Absyn_Local_b; _temp2243.f1=
_temp2237; _temp2243;}); _temp2242;}), 0)); Cyc_Toc_local_decl_to_c( _temp2241,
nv, _temp2237, _temp2166); goto _LL2232;}} _LL2236:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2224, _temp2222, _temp2220, _temp2218,
_temp2166))->r)); goto _LL2232; _LL2232:;} goto _LL2207; _LL2213: { struct Cyc_List_List*
_temp2244=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2226); if( _temp2244 ==  0){({ void* _temp2245[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp2245, sizeof( void*), 0u));});}(
void*)( _temp2168->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2246=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2246[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2247; _temp2247.tag= Cyc_Absyn_Var_d;
_temp2247.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2244))->hd; _temp2247;}); _temp2246;}))); _temp2244= _temp2244->tl; for( 0;
_temp2244 !=  0; _temp2244= _temp2244->tl){ struct Cyc_Absyn_Decl* _temp2248=
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2249=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2249[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2250; _temp2250.tag= Cyc_Absyn_Var_d;
_temp2250.f1=( struct Cyc_Absyn_Vardecl*) _temp2244->hd; _temp2250;}); _temp2249;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp2248, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL2207;} _LL2215:({
void* _temp2251[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp2251, sizeof( void*), 0u));}); goto _LL2207;
_LL2207:;} return; _LL2107: s= _temp2170; continue; _LL2109: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2178); Cyc_Toc_exp_to_c( nv, _temp2176); return;
_LL2111: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp2252=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2252->v=( void*) e_typ; _temp2252;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2182);{
struct Cyc_Absyn_Stmt* _temp2253= Cyc_Absyn_seq_stmt( _temp2182, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp2254= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2255= Cyc_Absyn_var_exp(
_temp2254, 0); struct Cyc_Absyn_Vardecl* _temp2256= Cyc_Absyn_new_vardecl(
_temp2254, Cyc_Absyn_exn_typ, 0); _temp2255->topt=({ struct Cyc_Core_Opt*
_temp2257=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2257->v=( void*) Cyc_Absyn_exn_typ; _temp2257;});{ struct Cyc_Absyn_Pat*
_temp2258=({ struct Cyc_Absyn_Pat* _temp2274=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2274->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2276=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2276[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2277; _temp2277.tag= Cyc_Absyn_Var_p;
_temp2277.f1= _temp2256; _temp2277;}); _temp2276;})); _temp2274->topt=({ struct
Cyc_Core_Opt* _temp2275=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2275->v=( void*) Cyc_Absyn_exn_typ; _temp2275;}); _temp2274->loc= 0;
_temp2274;}); struct Cyc_Absyn_Exp* _temp2259= Cyc_Absyn_throw_exp( _temp2255, 0);
_temp2259->topt=({ struct Cyc_Core_Opt* _temp2260=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2260->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2260;});{ struct Cyc_Absyn_Stmt* _temp2261= Cyc_Absyn_exp_stmt( _temp2259,
0); struct Cyc_Absyn_Switch_clause* _temp2262=({ struct Cyc_Absyn_Switch_clause*
_temp2271=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2271->pattern= _temp2258; _temp2271->pat_vars=({ struct Cyc_Core_Opt*
_temp2272=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2272->v=( void*)({ struct Cyc_List_List* _temp2273=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2273->hd=( void*) _temp2256;
_temp2273->tl= 0; _temp2273;}); _temp2272;}); _temp2271->where_clause= 0;
_temp2271->body= _temp2261; _temp2271->loc= 0; _temp2271;}); struct Cyc_Absyn_Stmt*
_temp2263= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2180,({ struct Cyc_List_List*
_temp2270=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2270->hd=( void*) _temp2262; _temp2270->tl= 0; _temp2270;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2263);{ struct Cyc_Absyn_Exp* _temp2264= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2269=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2269->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2269->tl= 0; _temp2269;}), 0); struct Cyc_Absyn_Stmt*
_temp2265= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2268=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2268->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2268->tl= 0; _temp2268;}), 0), 0); struct Cyc_Absyn_Exp* _temp2266= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2267= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2265, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2266, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2264, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2267, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2253, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2263, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL2113: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2186->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2184);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,({
struct Cyc_List_List* _temp2278=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2278->hd=( void*) Cyc_Absyn_string_exp( Cyc_Absynpp_qvar2string(
x_var), 0); _temp2278->tl= 0; _temp2278;}), 0), Cyc_Absyn_declare_stmt( x_var,
rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( rh_exp, 0),
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp* _temp2279[ 1u]; _temp2279[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp2279, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), Cyc_Absyn_seq_stmt( _temp2184, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({ struct Cyc_Absyn_Exp* _temp2280[ 1u]; _temp2280[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp2280, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL2115:({ void* _temp2281[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s", sizeof(
unsigned char), 10u), _tag_arr( _temp2281, sizeof( void*), 0u));}); return;
_LL2117:({ void* _temp2282[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u),
_tag_arr( _temp2282, sizeof( void*), 0u));}); return; _LL2119:({ void* _temp2283[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("splice", sizeof( unsigned char), 7u), _tag_arr( _temp2283, sizeof(
void*), 0u));}); return; _LL2077:;}} struct _tuple15{ struct _tagged_arr* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2284= f->args; for( 0; _temp2284 !=  0; _temp2284=
_temp2284->tl){(*(( struct _tuple15*) _temp2284->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple15*) _temp2284->hd)).f3);{ struct _tuple0* _temp2285=({ struct
_tuple0* _temp2286=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2286->f1=( void*) Cyc_Absyn_Loc_n; _temp2286->f2=(*(( struct _tuple15*)
_temp2284->hd)).f1; _temp2286;}); nv= Cyc_Toc_add_varmap( nv, _temp2285, Cyc_Absyn_var_exp(
_temp2285, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp2289; void* _temp2291;
struct Cyc_Absyn_Tqual _temp2293; struct Cyc_Core_Opt* _temp2295; struct Cyc_Absyn_VarargInfo
_temp2287=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL2296: _temp2295= _temp2287.name; goto _LL2294; _LL2294: _temp2293= _temp2287.tq;
goto _LL2292; _LL2292: _temp2291=( void*) _temp2287.type; goto _LL2290; _LL2290:
_temp2289= _temp2287.inject; goto _LL2288; _LL2288: { void* _temp2297= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp2291,( void*) Cyc_Absyn_HeapRgn, _temp2293)); struct
_tuple0* _temp2298=({ struct _tuple0* _temp2301=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp2301->f1=( void*) Cyc_Absyn_Loc_n; _temp2301->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2295))->v;
_temp2301;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2299=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2299->hd=( void*)({ struct _tuple15* _temp2300=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp2300->f1=( struct _tagged_arr*)
_temp2295->v; _temp2300->f2= _temp2293; _temp2300->f3= _temp2297; _temp2300;});
_temp2299->tl= 0; _temp2299;})); nv= Cyc_Toc_add_varmap( nv, _temp2298, Cyc_Absyn_var_exp(
_temp2298, 0)); f->cyc_varargs= 0;}}{ struct Cyc_List_List* _temp2302=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( f->param_vardecls))->v;
for( 0; _temp2302 !=  0; _temp2302= _temp2302->tl){( void*)((( struct Cyc_Absyn_Vardecl*)
_temp2302->hd)->type=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)
_temp2302->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2303= s; _LL2305: if(
_temp2303 == ( void*) Cyc_Absyn_Abstract){ goto _LL2306;} else{ goto _LL2307;}
_LL2307: if( _temp2303 == ( void*) Cyc_Absyn_ExternC){ goto _LL2308;} else{ goto
_LL2309;} _LL2309: goto _LL2310; _LL2306: return( void*) Cyc_Absyn_Public;
_LL2308: return( void*) Cyc_Absyn_Extern; _LL2310: return s; _LL2304:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2311=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2312=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2312->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2312;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2313=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2311).f2); if( _temp2313 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2311).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*) _temp2313->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2311).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(( void*) Cyc_Absyn_Public));
s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2314=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2314->v=( void*) _temp2311;
_temp2314;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields= 0;} else{
struct Cyc_List_List* _temp2315=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2315 !=  0; _temp2315= _temp2315->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2315->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2315->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2316=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({
struct Cyc_Core_Opt* _temp2317=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2317->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2317;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2318=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2316).f2); if( _temp2318 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2316).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*) _temp2318->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2316).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(( void*) Cyc_Absyn_Public));
u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2319=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2319->v=( void*) _temp2316;
_temp2319;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields= 0;} else{
struct Cyc_List_List* _temp2320=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2320 !=  0; _temp2320= _temp2320->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2320->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2320->hd)->type));}}}}} static
struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far= 0; static void Cyc_Toc_tuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl* tud){ if( Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt* _temp2321=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2321->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)(
struct _tuple0*, struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp);
_temp2321;});}{ struct _tuple0* _temp2322= tud->name; if( tud->fields ==  0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2322)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2322));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2323=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; for( 0; _temp2323 !=  0; _temp2323=
_temp2323->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)
_temp2323->hd; if( f->typs ==  0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2324;
_temp2324.q_const= 1; _temp2324.q_volatile= 0; _temp2324.q_restrict= 0;
_temp2324;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2325=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2325->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2326=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2326[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2327; _temp2327.tag= Cyc_Absyn_Var_d;
_temp2327.f1= vd; _temp2327;}); _temp2326;}), 0); _temp2325->tl= Cyc_Toc_result_decls;
_temp2325;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2328; _temp2328.q_const=
1; _temp2328.q_volatile= 0; _temp2328.q_restrict= 0; _temp2328;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2329=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2329->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2330=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2330[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2331; _temp2331.tag= Cyc_Absyn_Var_d; _temp2331.f1= vd; _temp2331;});
_temp2330;}), 0); _temp2329->tl= Cyc_Toc_result_decls; _temp2329;});{ struct Cyc_List_List*
_temp2332= 0; int i= 1;{ struct Cyc_List_List* _temp2333= f->typs; for( 0;
_temp2333 !=  0;( _temp2333= _temp2333->tl, i ++)){ struct _tagged_arr*
_temp2334= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield* _temp2335=({
struct Cyc_Absyn_Structfield* _temp2337=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2337->name= _temp2334;
_temp2337->tq=(*(( struct _tuple3*) _temp2333->hd)).f1; _temp2337->type=( void*)
Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2333->hd)).f2); _temp2337->width=
0; _temp2337->attributes= 0; _temp2337;}); _temp2332=({ struct Cyc_List_List*
_temp2336=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2336->hd=( void*) _temp2335; _temp2336->tl= _temp2332; _temp2336;});}}
_temp2332=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2332); _temp2332=({ struct Cyc_List_List* _temp2338=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2338->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2339=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2339->name= Cyc_Toc_tag_sp; _temp2339->tq= Cyc_Toc_mt_tq; _temp2339->type=(
void*) Cyc_Absyn_sint_t; _temp2339->width= 0; _temp2339->attributes= 0;
_temp2339;}); _temp2338->tl= _temp2332; _temp2338;});{ struct Cyc_Absyn_Structdecl*
_temp2340=({ struct Cyc_Absyn_Structdecl* _temp2344=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2344->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2344->name=({ struct Cyc_Core_Opt* _temp2346=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2346->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2346;}); _temp2344->tvs= 0; _temp2344->fields=({ struct Cyc_Core_Opt*
_temp2345=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2345->v=( void*) _temp2332; _temp2345;}); _temp2344->attributes= 0;
_temp2344;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2341=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2341->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2342=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2342[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2343; _temp2343.tag= Cyc_Absyn_Struct_d;
_temp2343.f1= _temp2340; _temp2343;}); _temp2342;}), 0); _temp2341->tl= Cyc_Toc_result_decls;
_temp2341;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp2347=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2347->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2347;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp2348= xd->name; struct Cyc_List_List*
_temp2349=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2349 !=  0; _temp2349= _temp2349->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*) _temp2349->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2350= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2351=({ struct
Cyc_Absyn_ArrayType_struct* _temp2401=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2401[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2402; _temp2402.tag= Cyc_Absyn_ArrayType;
_temp2402.f1=( void*) Cyc_Absyn_uchar_t; _temp2402.f2= Cyc_Toc_mt_tq; _temp2402.f3=(
struct Cyc_Absyn_Exp*) _temp2350; _temp2402;}); _temp2401;}); struct Cyc_Core_Opt*
_temp2352=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2360; int _temp2361; _LL2354: if( _temp2352 ==  0){ goto _LL2355;} else{
goto _LL2356;} _LL2356: if( _temp2352 ==  0){ goto _LL2358;} else{ _temp2360=*
_temp2352; _LL2362: _temp2361=( int) _temp2360.v; if( _temp2361 ==  0){ goto
_LL2357;} else{ goto _LL2358;}} _LL2358: goto _LL2359; _LL2355: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2368; _temp2368.tag= Cyc_Std_String_pa; _temp2368.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp2367; _temp2367.tag= Cyc_Std_Int_pa;
_temp2367.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2366; _temp2366.tag= Cyc_Std_Int_pa; _temp2366.f1=( int)(( unsigned int)((
int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2365; _temp2365.tag= Cyc_Std_Int_pa;
_temp2365.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2364; _temp2364.tag= Cyc_Std_Int_pa; _temp2364.f1=( int)(( unsigned int)((
int)'\000'));{ void* _temp2363[ 5u]={& _temp2364,& _temp2365,& _temp2366,&
_temp2367,& _temp2368}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof(
unsigned char), 11u), _tag_arr( _temp2363, sizeof( void*), 5u));}}}}}}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2369= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2351, initopt);( void*)( _temp2369->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2370=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2370->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2371=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2371[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2372; _temp2372.tag= Cyc_Absyn_Var_d; _temp2372.f1= _temp2369; _temp2372;});
_temp2371;}), 0); _temp2370->tl= Cyc_Toc_result_decls; _temp2370;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2373= f->typs; for( 0; _temp2373 !=  0;( _temp2373= _temp2373->tl, i ++)){
struct _tagged_arr* _temp2374=({ struct _tagged_arr* _temp2378=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2378[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2380; _temp2380.tag= Cyc_Std_Int_pa;
_temp2380.f1=( int)(( unsigned int) i);{ void* _temp2379[ 1u]={& _temp2380}; Cyc_Std_aprintf(
_tag_arr("f%d", sizeof( unsigned char), 4u), _tag_arr( _temp2379, sizeof( void*),
1u));}}); _temp2378;}); struct Cyc_Absyn_Structfield* _temp2375=({ struct Cyc_Absyn_Structfield*
_temp2377=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2377->name= _temp2374; _temp2377->tq=(*(( struct _tuple3*) _temp2373->hd)).f1;
_temp2377->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2373->hd)).f2);
_temp2377->width= 0; _temp2377->attributes= 0; _temp2377;}); fields=({ struct
Cyc_List_List* _temp2376=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2376->hd=( void*) _temp2375; _temp2376->tl= fields; _temp2376;});}} fields=({
struct Cyc_List_List* _temp2381=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2381->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2382=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2382->name= Cyc_Toc_tag_sp; _temp2382->tq= Cyc_Toc_mt_tq; _temp2382->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2382->width=
0; _temp2382->attributes= 0; _temp2382;}); _temp2381->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2381;});{ struct Cyc_Absyn_Structdecl*
_temp2383=({ struct Cyc_Absyn_Structdecl* _temp2387=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2387->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2387->name=({ struct Cyc_Core_Opt* _temp2389=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2389->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2389;}); _temp2387->tvs= 0; _temp2387->fields=({ struct Cyc_Core_Opt*
_temp2388=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2388->v=( void*) fields; _temp2388;}); _temp2387->attributes= 0; _temp2387;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2384=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2384->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2385=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2385[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2386; _temp2386.tag= Cyc_Absyn_Struct_d;
_temp2386.f1= _temp2383; _temp2386;}); _temp2385;}), 0); _temp2384->tl= Cyc_Toc_result_decls;
_temp2384;});}} goto _LL2353;}} _LL2357: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2390= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2400; _temp2400.tag= Cyc_Std_String_pa;
_temp2400.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2399;
_temp2399.tag= Cyc_Std_Int_pa; _temp2399.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2398; _temp2398.tag= Cyc_Std_Int_pa; _temp2398.f1=(
int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2397;
_temp2397.tag= Cyc_Std_Int_pa; _temp2397.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2396; _temp2396.tag= Cyc_Std_Int_pa; _temp2396.f1=(
int)(( unsigned int)(( int)'\000'));{ void* _temp2395[ 5u]={& _temp2396,&
_temp2397,& _temp2398,& _temp2399,& _temp2400}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s",
sizeof( unsigned char), 11u), _tag_arr( _temp2395, sizeof( void*), 5u));}}}}}}),
0); struct Cyc_Absyn_Vardecl* _temp2391= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2351,( struct Cyc_Absyn_Exp*) _temp2390);( void*)( _temp2391->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2392=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2392->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2393=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2393[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2394; _temp2394.tag= Cyc_Absyn_Var_d;
_temp2394.f1= _temp2391; _temp2394;}); _temp2393;}), 0); _temp2392->tl= Cyc_Toc_result_decls;
_temp2392;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k,
int v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2353;
_LL2359: goto _LL2353; _LL2353:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2403= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v);}}
static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s){( void*)( vd->type=(
void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c( body_nv, s);
if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2404=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2410; struct Cyc_Absyn_Exp* _temp2412; struct Cyc_Absyn_Vardecl* _temp2414;
_LL2406: if(*(( int*) _temp2404) ==  Cyc_Absyn_Comprehension_e){ _LL2415:
_temp2414=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2404)->f1; goto
_LL2413; _LL2413: _temp2412=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2404)->f2; goto _LL2411; _LL2411: _temp2410=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2404)->f3; goto _LL2407;} else{ goto _LL2408;} _LL2408: goto _LL2409;
_LL2407: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2414, _temp2412, _temp2410, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2405; _LL2409: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2416= Cyc_Toc_copy_env( init_nv);
_temp2416->toplevel= 1; Cyc_Toc_exp_to_c( _temp2416, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2405; _LL2405:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2417= _new_region("prgn"); struct _RegionHandle* prgn=&
_temp2417; _push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2420; struct Cyc_List_List*
_temp2422; struct Cyc_Toc_Env* _temp2424; struct _tuple11 _temp2418= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2425: _temp2424= _temp2418.f1; goto _LL2423; _LL2423: _temp2422=
_temp2418.f2; goto _LL2421; _LL2421: _temp2420= _temp2418.f3; goto _LL2419;
_LL2419: Cyc_Toc_stmt_to_c( _temp2424, s);{ struct Cyc_Absyn_Stmt* _temp2426=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2420, _temp2426, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2427= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2428= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2427, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2420,
Cyc_Absyn_seq_stmt( _temp2428, _temp2426, 0), 0), 0);} for( 0; _temp2422 !=  0;
_temp2422= _temp2422->tl){ struct _tuple12 _temp2431; void* _temp2432; struct
_tuple0* _temp2434; struct _tuple12* _temp2429=( struct _tuple12*) _temp2422->hd;
_temp2431=* _temp2429; _LL2435: _temp2434= _temp2431.f1; goto _LL2433; _LL2433:
_temp2432= _temp2431.f2; goto _LL2430; _LL2430: s= Cyc_Absyn_declare_stmt(
_temp2434, _temp2432, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds= ds->tl){ if( ! nv->toplevel){({ void*
_temp2436[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("decls_to_c: not at toplevel!", sizeof( unsigned char), 29u), _tag_arr(
_temp2436, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
ds->hd; void* _temp2437=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp2463;
struct Cyc_Absyn_Fndecl* _temp2465; struct Cyc_Absyn_Structdecl* _temp2467;
struct Cyc_Absyn_Uniondecl* _temp2469; struct Cyc_Absyn_Tuniondecl* _temp2471;
struct Cyc_Absyn_Enumdecl* _temp2473; struct Cyc_Absyn_Typedefdecl* _temp2475;
struct Cyc_List_List* _temp2477; struct Cyc_List_List* _temp2479; struct Cyc_List_List*
_temp2481; _LL2439: if(*(( int*) _temp2437) ==  Cyc_Absyn_Var_d){ _LL2464:
_temp2463=(( struct Cyc_Absyn_Var_d_struct*) _temp2437)->f1; goto _LL2440;}
else{ goto _LL2441;} _LL2441: if(*(( int*) _temp2437) ==  Cyc_Absyn_Fn_d){
_LL2466: _temp2465=(( struct Cyc_Absyn_Fn_d_struct*) _temp2437)->f1; goto
_LL2442;} else{ goto _LL2443;} _LL2443: if(*(( int*) _temp2437) ==  Cyc_Absyn_Let_d){
goto _LL2444;} else{ goto _LL2445;} _LL2445: if(*(( int*) _temp2437) ==  Cyc_Absyn_Letv_d){
goto _LL2446;} else{ goto _LL2447;} _LL2447: if(*(( int*) _temp2437) ==  Cyc_Absyn_Struct_d){
_LL2468: _temp2467=(( struct Cyc_Absyn_Struct_d_struct*) _temp2437)->f1; goto
_LL2448;} else{ goto _LL2449;} _LL2449: if(*(( int*) _temp2437) ==  Cyc_Absyn_Union_d){
_LL2470: _temp2469=(( struct Cyc_Absyn_Union_d_struct*) _temp2437)->f1; goto
_LL2450;} else{ goto _LL2451;} _LL2451: if(*(( int*) _temp2437) ==  Cyc_Absyn_Tunion_d){
_LL2472: _temp2471=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2437)->f1; goto
_LL2452;} else{ goto _LL2453;} _LL2453: if(*(( int*) _temp2437) ==  Cyc_Absyn_Enum_d){
_LL2474: _temp2473=(( struct Cyc_Absyn_Enum_d_struct*) _temp2437)->f1; goto
_LL2454;} else{ goto _LL2455;} _LL2455: if(*(( int*) _temp2437) ==  Cyc_Absyn_Typedef_d){
_LL2476: _temp2475=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2437)->f1; goto
_LL2456;} else{ goto _LL2457;} _LL2457: if(*(( int*) _temp2437) ==  Cyc_Absyn_Namespace_d){
_LL2478: _temp2477=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2437)->f2; goto
_LL2458;} else{ goto _LL2459;} _LL2459: if(*(( int*) _temp2437) ==  Cyc_Absyn_Using_d){
_LL2480: _temp2479=(( struct Cyc_Absyn_Using_d_struct*) _temp2437)->f2; goto
_LL2460;} else{ goto _LL2461;} _LL2461: if(*(( int*) _temp2437) ==  Cyc_Absyn_ExternC_d){
_LL2482: _temp2481=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2437)->f1; goto
_LL2462;} else{ goto _LL2438;} _LL2440: { struct _tuple0* _temp2483= _temp2463->name;
if(( void*) _temp2463->sc == ( void*) Cyc_Absyn_ExternC){ _temp2483=({ struct
_tuple0* _temp2484=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2484->f1= Cyc_Absyn_rel_ns_null; _temp2484->f2=(* _temp2483).f2; _temp2484;});}
if( _temp2463->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2463->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2463->name,
Cyc_Absyn_varb_exp( _temp2483,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2485=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2485[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2486; _temp2486.tag= Cyc_Absyn_Global_b;
_temp2486.f1= _temp2463; _temp2486;}); _temp2485;}), 0)); _temp2463->name=
_temp2483;( void*)( _temp2463->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2463->sc));(
void*)( _temp2463->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2463->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2487=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2487->hd=( void*) d; _temp2487->tl=
Cyc_Toc_result_decls; _temp2487;}); goto _LL2438;} _LL2442: { struct _tuple0*
_temp2488= _temp2465->name; if(( void*) _temp2465->sc == ( void*) Cyc_Absyn_ExternC){
_temp2488=({ struct _tuple0* _temp2489=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2489->f1= Cyc_Absyn_rel_ns_null; _temp2489->f2=(*
_temp2488).f2; _temp2489;});} nv= Cyc_Toc_add_varmap( nv, _temp2465->name, Cyc_Absyn_var_exp(
_temp2488, 0)); _temp2465->name= _temp2488; Cyc_Toc_fndecl_to_c( nv, _temp2465);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2490=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2490->hd=( void*) d; _temp2490->tl=
Cyc_Toc_result_decls; _temp2490;}); goto _LL2438;} _LL2444: goto _LL2446;
_LL2446:({ void* _temp2491[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2491, sizeof( void*), 0u));}); goto _LL2438;
_LL2448: Cyc_Toc_structdecl_to_c( _temp2467); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2492=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2492->hd=( void*) d; _temp2492->tl= Cyc_Toc_result_decls; _temp2492;});
goto _LL2438; _LL2450: Cyc_Toc_uniondecl_to_c( _temp2469); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2493=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2493->hd=( void*) d; _temp2493->tl= Cyc_Toc_result_decls;
_temp2493;}); goto _LL2438; _LL2452: if( _temp2471->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2471);} else{ Cyc_Toc_tuniondecl_to_c( _temp2471);} goto _LL2438; _LL2454:
Cyc_Toc_enumdecl_to_c( nv, _temp2473); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2494=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2494->hd=( void*) d; _temp2494->tl= Cyc_Toc_result_decls; _temp2494;});
goto _LL2438; _LL2456: _temp2475->name= _temp2475->name; _temp2475->tvs= 0;(
void*)( _temp2475->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2475->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2495=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2495->hd=( void*) d; _temp2495->tl=
Cyc_Toc_result_decls; _temp2495;}); goto _LL2438; _LL2458: _temp2479= _temp2477;
goto _LL2460; _LL2460: _temp2481= _temp2479; goto _LL2462; _LL2462: nv= Cyc_Toc_decls_to_c(
nv, _temp2481, top); goto _LL2438; _LL2438:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0; Cyc_Toc_total_bounds_checks=
0; Cyc_Toc_bounds_checks_eliminated= 0;} struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
