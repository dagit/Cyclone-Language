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
#define _new_region(n) _profile_new_region(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern int Cyc_Core_ptrcmp(
void*, void*); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_rcopy(
struct _RegionHandle*, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_rempty( struct _RegionHandle*, int(*
cmp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void*
k); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void*
k, void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k);
extern struct Cyc_Dict_Dict* Cyc_Dict_rdelete_same( struct Cyc_Dict_Dict*, void*);
extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); struct
Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
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
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; static const
int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_IfThenElse_s=
3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; static const int Cyc_Absyn_While_s=
4; struct _tuple2{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
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
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s=
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt(
struct _tuple0*, void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc); struct Cyc_Set_Set; extern struct
Cyc_Set_Set* Cyc_Set_rempty( struct _RegionHandle* r, int(* cmp)( void*, void*));
extern struct Cyc_Set_Set* Cyc_Set_rinsert( struct _RegionHandle* r, struct Cyc_Set_Set*
s, void* elt); extern struct Cyc_List_List* Cyc_Set_elements( struct Cyc_Set_Set*
s); extern unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes=
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); extern struct _tuple0* Cyc_Toc_temp_var(); extern struct Cyc_List_List*
Cyc_Tovc_tovc( struct Cyc_List_List* decls); struct Cyc_Tovc_BoxingEnv{ struct
_RegionHandle* rgn; struct Cyc_List_List* all_locals; struct Cyc_Dict_Dict*
varmap; struct Cyc_List_List* boundvars; struct Cyc_Set_Set** freevars; } ;
struct Cyc_Tovc_ToExpEnv{ struct _RegionHandle* rgn; struct Cyc_List_List*
all_locals; struct Cyc_Dict_Dict* varmap; struct Cyc_Absyn_Stmt* encloser;
struct Cyc_List_List** gen_ds; } ; struct _tuple3{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Tovc_box_free_vars_exp( struct Cyc_Tovc_BoxingEnv
env, struct Cyc_Absyn_Exp* e){ while( 1) { void* _temp0=( void*) e->r; struct
_tuple0* _temp50; struct _tuple0 _temp52; struct _tagged_arr* _temp53; void*
_temp55; struct Cyc_List_List* _temp57; struct Cyc_Absyn_Exp* _temp59; struct
Cyc_List_List* _temp61; struct Cyc_Absyn_Exp* _temp63; struct Cyc_Absyn_Exp*
_temp65; struct Cyc_Absyn_Exp* _temp67; struct Cyc_Absyn_Exp* _temp69; struct
Cyc_Absyn_Exp* _temp71; struct Cyc_Absyn_Exp* _temp73; struct Cyc_Absyn_Exp*
_temp75; struct Cyc_Absyn_Exp* _temp77; struct Cyc_Absyn_Exp* _temp79; struct
Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp* _temp83; struct Cyc_Absyn_Exp*
_temp85; struct Cyc_Absyn_Exp* _temp87; struct Cyc_Absyn_Exp* _temp89; struct
Cyc_Absyn_Exp* _temp91; struct Cyc_Absyn_Exp* _temp93; struct Cyc_Absyn_Exp*
_temp95; struct Cyc_Absyn_Exp* _temp97; struct Cyc_List_List* _temp99; _LL2: if(*((
int*) _temp0) ==  Cyc_Absyn_Var_e){ _LL51: _temp50=(( struct Cyc_Absyn_Var_e_struct*)
_temp0)->f1; _temp52=* _temp50; _LL56: _temp55= _temp52.f1; if( _temp55 == (
void*) Cyc_Absyn_Loc_n){ goto _LL54;} else{ goto _LL4;} _LL54: _temp53= _temp52.f2;
goto _LL3;} else{ goto _LL4;} _LL4: if(*(( int*) _temp0) ==  Cyc_Absyn_Var_e){
goto _LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp0) ==  Cyc_Absyn_Const_e){
goto _LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp0) ==  Cyc_Absyn_FnCall_e){
_LL60: _temp59=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f1; goto _LL58;
_LL58: _temp57=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f2; goto _LL9;}
else{ goto _LL10;} _LL10: if(*(( int*) _temp0) ==  Cyc_Absyn_Primop_e){ _LL62:
_temp61=(( struct Cyc_Absyn_Primop_e_struct*) _temp0)->f2; goto _LL11;} else{
goto _LL12;} _LL12: if(*(( int*) _temp0) ==  Cyc_Absyn_Conditional_e){ _LL68:
_temp67=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f1; goto _LL66;
_LL66: _temp65=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f2; goto
_LL64; _LL64: _temp63=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f3;
goto _LL13;} else{ goto _LL14;} _LL14: if(*(( int*) _temp0) ==  Cyc_Absyn_Subscript_e){
_LL72: _temp71=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f1; goto _LL70;
_LL70: _temp69=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f2; goto _LL15;}
else{ goto _LL16;} _LL16: if(*(( int*) _temp0) ==  Cyc_Absyn_SeqExp_e){ _LL76:
_temp75=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f1; goto _LL74; _LL74:
_temp73=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f2; goto _LL17;} else{
goto _LL18;} _LL18: if(*(( int*) _temp0) ==  Cyc_Absyn_AssignOp_e){ _LL80:
_temp79=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f1; goto _LL78; _LL78:
_temp77=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f3; goto _LL19;} else{
goto _LL20;} _LL20: if(*(( int*) _temp0) ==  Cyc_Absyn_NoInstantiate_e){ _LL82:
_temp81=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp0)->f1; goto _LL21;}
else{ goto _LL22;} _LL22: if(*(( int*) _temp0) ==  Cyc_Absyn_Instantiate_e){
_LL84: _temp83=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp0)->f1; goto
_LL23;} else{ goto _LL24;} _LL24: if(*(( int*) _temp0) ==  Cyc_Absyn_Cast_e){
_LL86: _temp85=(( struct Cyc_Absyn_Cast_e_struct*) _temp0)->f2; goto _LL25;}
else{ goto _LL26;} _LL26: if(*(( int*) _temp0) ==  Cyc_Absyn_Address_e){ _LL88:
_temp87=(( struct Cyc_Absyn_Address_e_struct*) _temp0)->f1; goto _LL27;} else{
goto _LL28;} _LL28: if(*(( int*) _temp0) ==  Cyc_Absyn_Sizeofexp_e){ _LL90:
_temp89=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp0)->f1; goto _LL29;} else{
goto _LL30;} _LL30: if(*(( int*) _temp0) ==  Cyc_Absyn_Deref_e){ _LL92: _temp91=((
struct Cyc_Absyn_Deref_e_struct*) _temp0)->f1; goto _LL31;} else{ goto _LL32;}
_LL32: if(*(( int*) _temp0) ==  Cyc_Absyn_StructMember_e){ _LL94: _temp93=((
struct Cyc_Absyn_StructMember_e_struct*) _temp0)->f1; goto _LL33;} else{ goto
_LL34;} _LL34: if(*(( int*) _temp0) ==  Cyc_Absyn_StructArrow_e){ _LL96: _temp95=((
struct Cyc_Absyn_StructArrow_e_struct*) _temp0)->f1; goto _LL35;} else{ goto
_LL36;} _LL36: if(*(( int*) _temp0) ==  Cyc_Absyn_Increment_e){ _LL98: _temp97=((
struct Cyc_Absyn_Increment_e_struct*) _temp0)->f1; goto _LL37;} else{ goto _LL38;}
_LL38: if(*(( int*) _temp0) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL39;} else{ goto
_LL40;} _LL40: if(*(( int*) _temp0) ==  Cyc_Absyn_Offsetof_e){ goto _LL41;}
else{ goto _LL42;} _LL42: if(*(( int*) _temp0) ==  Cyc_Absyn_Gentyp_e){ goto
_LL43;} else{ goto _LL44;} _LL44: if(*(( int*) _temp0) ==  Cyc_Absyn_Enum_e){
goto _LL45;} else{ goto _LL46;} _LL46: if(*(( int*) _temp0) ==  Cyc_Absyn_UnresolvedMem_e){
_LL100: _temp99=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp0)->f2; goto
_LL47;} else{ goto _LL48;} _LL48: goto _LL49; _LL3: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_member)( env.varmap, _temp53)){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( env.varmap, _temp53))->r)); continue;} for( 0;
env.boundvars !=  0; env.boundvars=( env.boundvars)->tl){ if( Cyc_Std_zstrptrcmp(
_temp53,(*(( struct Cyc_Absyn_Vardecl*)( env.boundvars)->hd)->name).f2) ==  0){
return;}} for( 0; env.all_locals !=  0; env.all_locals=( env.all_locals)->tl){
struct Cyc_Absyn_Vardecl* _temp101=( struct Cyc_Absyn_Vardecl*)( env.all_locals)->hd;
if( Cyc_Std_zstrptrcmp( _temp53,(* _temp101->name).f2) ==  0){{ void* _temp102=(
void*) _temp101->type; _LL104: if(( unsigned int) _temp102 >  3u?*(( int*)
_temp102) ==  Cyc_Absyn_ArrayType: 0){ goto _LL105;} else{ goto _LL106;} _LL106:
goto _LL107; _LL105: goto _LL103; _LL107:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_var_exp( _temp101->name, 0), 0))->r)); goto _LL103; _LL103:;}* env.freevars=((
struct Cyc_Set_Set*(*)( struct _RegionHandle* r, struct Cyc_Set_Set* s, struct
Cyc_Absyn_Vardecl* elt)) Cyc_Set_rinsert)( env.rgn,* env.freevars, _temp101);
return;}}( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp108=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp108[ 0]=({ struct Cyc_Core_Impossible_struct _temp109; _temp109.tag= Cyc_Core_Impossible;
_temp109.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp111;
_temp111.tag= Cyc_Std_String_pa; _temp111.f1=( struct _tagged_arr)* _temp53;{
void* _temp110[ 1u]={& _temp111}; Cyc_Std_aprintf( _tag_arr("unbound variable %s found in box_free_vars_exp",
sizeof( unsigned char), 47u), _tag_arr( _temp110, sizeof( void*), 1u));}});
_temp109;}); _temp108;})); _LL5: return; _LL7: return; _LL9: Cyc_Tovc_box_free_vars_exp(
env, _temp59); _temp61= _temp57; goto _LL11; _LL11: for( 0; _temp61 !=  0;
_temp61= _temp61->tl){ Cyc_Tovc_box_free_vars_exp( env,( struct Cyc_Absyn_Exp*)
_temp61->hd);} return; _LL13: Cyc_Tovc_box_free_vars_exp( env, _temp63); _temp71=
_temp67; _temp69= _temp65; goto _LL15; _LL15: _temp75= _temp71; _temp73= _temp69;
goto _LL17; _LL17: _temp79= _temp75; _temp77= _temp73; goto _LL19; _LL19: Cyc_Tovc_box_free_vars_exp(
env, _temp77); _temp81= _temp79; goto _LL21; _LL21: _temp83= _temp81; goto _LL23;
_LL23: _temp85= _temp83; goto _LL25; _LL25: _temp87= _temp85; goto _LL27; _LL27:
_temp89= _temp87; goto _LL29; _LL29: _temp91= _temp89; goto _LL31; _LL31:
_temp93= _temp91; goto _LL33; _LL33: _temp95= _temp93; goto _LL35; _LL35:
_temp97= _temp95; goto _LL37; _LL37: Cyc_Tovc_box_free_vars_exp( env, _temp97);
return; _LL39: return; _LL41: return; _LL43: return; _LL45: return; _LL47: for(
0; _temp99 !=  0; _temp99= _temp99->tl){ Cyc_Tovc_box_free_vars_exp( env,(*((
struct _tuple3*) _temp99->hd)).f2);} return; _LL49:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp112=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp112[ 0]=({ struct
Cyc_Core_Impossible_struct _temp113; _temp113.tag= Cyc_Core_Impossible; _temp113.f1=
_tag_arr("bad exp form in Tocv::box_free_vars_exp", sizeof( unsigned char), 40u);
_temp113;}); _temp112;})); _LL1:;}} static void Cyc_Tovc_box_free_vars_stmt(
struct Cyc_Tovc_BoxingEnv env, struct Cyc_Absyn_Stmt* s){ while( 1) { void*
_temp114=( void*) s->r; struct Cyc_Absyn_Stmt* _temp146; struct Cyc_Absyn_Exp*
_temp148; struct Cyc_Absyn_Exp* _temp150; struct Cyc_Absyn_Stmt* _temp152;
struct Cyc_Absyn_Stmt* _temp154; struct Cyc_Absyn_Stmt* _temp156; struct Cyc_Absyn_Stmt*
_temp158; struct Cyc_Absyn_Exp* _temp160; struct Cyc_Absyn_Stmt* _temp162;
struct _tuple2 _temp164; struct Cyc_Absyn_Exp* _temp166; struct Cyc_Absyn_Stmt*
_temp168; struct _tuple2 _temp170; struct Cyc_Absyn_Exp* _temp172; struct
_tuple2 _temp174; struct Cyc_Absyn_Exp* _temp176; struct Cyc_Absyn_Exp* _temp178;
struct _tuple2 _temp180; struct Cyc_Absyn_Exp* _temp182; struct Cyc_Absyn_Stmt*
_temp184; struct Cyc_List_List* _temp186; struct Cyc_Absyn_Exp* _temp188; struct
Cyc_Absyn_Stmt* _temp190; struct Cyc_Absyn_Decl* _temp192; _LL116: if( _temp114
== ( void*) Cyc_Absyn_Skip_s){ goto _LL117;} else{ goto _LL118;} _LL118: if((
unsigned int) _temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_Break_s: 0){ goto
_LL119;} else{ goto _LL120;} _LL120: if(( unsigned int) _temp114 >  1u?*(( int*)
_temp114) ==  Cyc_Absyn_Continue_s: 0){ goto _LL121;} else{ goto _LL122;} _LL122:
if(( unsigned int) _temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_Goto_s: 0){
goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int) _temp114 >  1u?*((
int*) _temp114) ==  Cyc_Absyn_Label_s: 0){ _LL147: _temp146=(( struct Cyc_Absyn_Label_s_struct*)
_temp114)->f2; goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_Return_s: 0){ _LL149: _temp148=((
struct Cyc_Absyn_Return_s_struct*) _temp114)->f1; goto _LL127;} else{ goto
_LL128;} _LL128: if(( unsigned int) _temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_Exp_s:
0){ _LL151: _temp150=(( struct Cyc_Absyn_Exp_s_struct*) _temp114)->f1; goto
_LL129;} else{ goto _LL130;} _LL130: if(( unsigned int) _temp114 >  1u?*(( int*)
_temp114) ==  Cyc_Absyn_Seq_s: 0){ _LL155: _temp154=(( struct Cyc_Absyn_Seq_s_struct*)
_temp114)->f1; goto _LL153; _LL153: _temp152=(( struct Cyc_Absyn_Seq_s_struct*)
_temp114)->f2; goto _LL131;} else{ goto _LL132;} _LL132: if(( unsigned int)
_temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL161:
_temp160=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp114)->f1; goto _LL159;
_LL159: _temp158=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp114)->f2; goto
_LL157; _LL157: _temp156=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp114)->f3;
goto _LL133;} else{ goto _LL134;} _LL134: if(( unsigned int) _temp114 >  1u?*((
int*) _temp114) ==  Cyc_Absyn_While_s: 0){ _LL165: _temp164=(( struct Cyc_Absyn_While_s_struct*)
_temp114)->f1; _LL167: _temp166= _temp164.f1; goto _LL163; _LL163: _temp162=((
struct Cyc_Absyn_While_s_struct*) _temp114)->f2; goto _LL135;} else{ goto _LL136;}
_LL136: if(( unsigned int) _temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_For_s:
0){ _LL179: _temp178=(( struct Cyc_Absyn_For_s_struct*) _temp114)->f1; goto
_LL175; _LL175: _temp174=(( struct Cyc_Absyn_For_s_struct*) _temp114)->f2;
_LL177: _temp176= _temp174.f1; goto _LL171; _LL171: _temp170=(( struct Cyc_Absyn_For_s_struct*)
_temp114)->f3; _LL173: _temp172= _temp170.f1; goto _LL169; _LL169: _temp168=((
struct Cyc_Absyn_For_s_struct*) _temp114)->f4; goto _LL137;} else{ goto _LL138;}
_LL138: if(( unsigned int) _temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_Do_s:
0){ _LL185: _temp184=(( struct Cyc_Absyn_Do_s_struct*) _temp114)->f1; goto
_LL181; _LL181: _temp180=(( struct Cyc_Absyn_Do_s_struct*) _temp114)->f2; _LL183:
_temp182= _temp180.f1; goto _LL139;} else{ goto _LL140;} _LL140: if((
unsigned int) _temp114 >  1u?*(( int*) _temp114) ==  Cyc_Absyn_Switch_s: 0){
_LL189: _temp188=(( struct Cyc_Absyn_Switch_s_struct*) _temp114)->f1; goto
_LL187; _LL187: _temp186=(( struct Cyc_Absyn_Switch_s_struct*) _temp114)->f2;
goto _LL141;} else{ goto _LL142;} _LL142: if(( unsigned int) _temp114 >  1u?*((
int*) _temp114) ==  Cyc_Absyn_Decl_s: 0){ _LL193: _temp192=(( struct Cyc_Absyn_Decl_s_struct*)
_temp114)->f1; goto _LL191; _LL191: _temp190=(( struct Cyc_Absyn_Decl_s_struct*)
_temp114)->f2; goto _LL143;} else{ goto _LL144;} _LL144: goto _LL145; _LL117:
return; _LL119: return; _LL121: return; _LL123: return; _LL125: s= _temp146;
continue; _LL127: if( _temp148 ==  0){ return;} _temp150=( struct Cyc_Absyn_Exp*)
_check_null( _temp148); goto _LL129; _LL129: Cyc_Tovc_box_free_vars_exp( env,
_temp150); return; _LL131: Cyc_Tovc_box_free_vars_stmt( env, _temp154); s=
_temp152; continue; _LL133: Cyc_Tovc_box_free_vars_exp( env, _temp160); Cyc_Tovc_box_free_vars_stmt(
env, _temp158); s= _temp156; continue; _LL135: Cyc_Tovc_box_free_vars_exp( env,
_temp166); s= _temp162; continue; _LL137: Cyc_Tovc_box_free_vars_exp( env,
_temp178); Cyc_Tovc_box_free_vars_exp( env, _temp176); Cyc_Tovc_box_free_vars_exp(
env, _temp172); s= _temp168; continue; _LL139: Cyc_Tovc_box_free_vars_exp( env,
_temp182); s= _temp184; continue; _LL141: Cyc_Tovc_box_free_vars_exp( env,
_temp188); for( 0; _temp186 !=  0; _temp186= _temp186->tl){ Cyc_Tovc_box_free_vars_stmt(
env,(( struct Cyc_Absyn_Switch_clause*) _temp186->hd)->body);} return; _LL143:{
void* _temp194=( void*) _temp192->r; struct Cyc_Absyn_Vardecl* _temp200; _LL196:
if(*(( int*) _temp194) ==  Cyc_Absyn_Var_d){ _LL201: _temp200=(( struct Cyc_Absyn_Var_d_struct*)
_temp194)->f1; goto _LL197;} else{ goto _LL198;} _LL198: goto _LL199; _LL197:
env.boundvars=({ struct Cyc_List_List* _temp202=( struct Cyc_List_List*)
_region_malloc( env.rgn, sizeof( struct Cyc_List_List)); _temp202->hd=( void*)
_temp200; _temp202->tl= env.boundvars; _temp202;}); env.varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*, struct _tagged_arr*)) Cyc_Dict_rdelete_same)( env.varmap,(*
_temp200->name).f2); if( _temp200->initializer !=  0){ Cyc_Tovc_box_free_vars_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp200->initializer));} s= _temp190;
continue; _LL199: goto _LL195; _LL195:;} goto _LL145; _LL145:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp203=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp203[ 0]=({ struct
Cyc_Core_Impossible_struct _temp204; _temp204.tag= Cyc_Core_Impossible; _temp204.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp204;});
_temp203;})); _LL115:;}} struct _tuple4{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_fun( struct Cyc_Tovc_ToExpEnv
env, struct Cyc_Absyn_Stmt* s, void* rettype){ struct Cyc_Set_Set* freevars=((
struct Cyc_Set_Set*(*)( struct _RegionHandle* r, int(* cmp)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*))) Cyc_Set_rempty)( env.rgn,( int(*)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*)) Cyc_Core_ptrcmp); struct Cyc_Tovc_BoxingEnv _temp205=({
struct Cyc_Tovc_BoxingEnv _temp224; _temp224.rgn= env.rgn; _temp224.all_locals=
env.all_locals; _temp224.varmap= env.varmap; _temp224.boundvars= 0; _temp224.freevars=&
freevars; _temp224;}); Cyc_Tovc_box_free_vars_stmt( _temp205, s);{ struct Cyc_List_List*
params= 0; struct Cyc_List_List* args= 0; struct Cyc_List_List* free_vars=((
struct Cyc_List_List*(*)( struct Cyc_Set_Set* s)) Cyc_Set_elements)( freevars);
for( 0; free_vars !=  0; free_vars= free_vars->tl){ struct Cyc_Absyn_Vardecl* vd=(
struct Cyc_Absyn_Vardecl*) free_vars->hd; void* _temp206=( void*) vd->type; void*
_temp212; _LL208: if(( unsigned int) _temp206 >  3u?*(( int*) _temp206) ==  Cyc_Absyn_ArrayType:
0){ _LL213: _temp212=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp206)->f1;
goto _LL209;} else{ goto _LL210;} _LL210: goto _LL211; _LL209: params=({ struct
Cyc_List_List* _temp214=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp214->hd=( void*)({ struct _tuple4* _temp215=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp215->f1=(* vd->name).f2; _temp215->f2= vd->tq;
_temp215->f3= Cyc_Absyn_cstar_typ( _temp212, Cyc_Absyn_empty_tqual()); _temp215;});
_temp214->tl= params; _temp214;}); args=({ struct Cyc_List_List* _temp216=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp216->hd=(
void*) Cyc_Absyn_var_exp( vd->name, 0); _temp216->tl= args; _temp216;}); goto
_LL207; _LL211: params=({ struct Cyc_List_List* _temp217=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp217->hd=( void*)({ struct
_tuple4* _temp218=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp218->f1=(* vd->name).f2; _temp218->f2= vd->tq; _temp218->f3= Cyc_Absyn_cstar_typ((
void*) vd->type, Cyc_Absyn_empty_tqual()); _temp218;}); _temp217->tl= params;
_temp217;}); args=({ struct Cyc_List_List* _temp219=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp219->hd=( void*) Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp( vd->name, 0), 0); _temp219->tl= args; _temp219;}); goto
_LL207; _LL207:;}{ struct _tuple0* funname= Cyc_Toc_temp_var();* env.gen_ds=({
struct Cyc_List_List* _temp220=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp220->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp221=( struct Cyc_Absyn_Fn_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp221[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp222; _temp222.tag= Cyc_Absyn_Fn_d; _temp222.f1=({ struct Cyc_Absyn_Fndecl*
_temp223=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp223->sc=( void*)(( void*) Cyc_Absyn_Static); _temp223->is_inline= 1;
_temp223->name= funname; _temp223->tvs= 0; _temp223->effect= 0; _temp223->ret_type=(
void*) rettype; _temp223->args= params; _temp223->c_varargs= 0; _temp223->cyc_varargs=
0; _temp223->rgn_po= 0; _temp223->body= s; _temp223->cached_typ= 0; _temp223->param_vardecls=
0; _temp223->attributes= 0; _temp223;}); _temp222;}); _temp221;}), 0); _temp220->tl=*
env.gen_ds; _temp220;}); return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( funname,
0), args, 0);}}} static void Cyc_Tovc_apply_varmap( struct Cyc_Dict_Dict* varmap,
struct Cyc_Absyn_Exp* e){ void* _temp225=( void*) e->r; struct _tuple0* _temp275;
struct _tuple0 _temp277; struct _tagged_arr* _temp278; void* _temp280; struct
Cyc_List_List* _temp282; struct Cyc_Absyn_Exp* _temp284; struct Cyc_List_List*
_temp286; struct Cyc_Absyn_Exp* _temp288; struct Cyc_Absyn_Exp* _temp290; struct
Cyc_Absyn_Exp* _temp292; struct Cyc_Absyn_Exp* _temp294; struct Cyc_Absyn_Exp*
_temp296; struct Cyc_Absyn_Exp* _temp298; struct Cyc_Absyn_Exp* _temp300; struct
Cyc_Absyn_Exp* _temp302; struct Cyc_Absyn_Exp* _temp304; struct Cyc_Absyn_Exp*
_temp306; struct Cyc_Absyn_Exp* _temp308; struct Cyc_Absyn_Exp* _temp310; struct
Cyc_Absyn_Exp* _temp312; struct Cyc_Absyn_Exp* _temp314; struct Cyc_Absyn_Exp*
_temp316; struct Cyc_Absyn_Exp* _temp318; struct Cyc_Absyn_Exp* _temp320; struct
Cyc_Absyn_Exp* _temp322; struct Cyc_List_List* _temp324; _LL227: if(*(( int*)
_temp225) ==  Cyc_Absyn_Var_e){ _LL276: _temp275=(( struct Cyc_Absyn_Var_e_struct*)
_temp225)->f1; _temp277=* _temp275; _LL281: _temp280= _temp277.f1; if( _temp280
== ( void*) Cyc_Absyn_Loc_n){ goto _LL279;} else{ goto _LL229;} _LL279: _temp278=
_temp277.f2; goto _LL228;} else{ goto _LL229;} _LL229: if(*(( int*) _temp225) == 
Cyc_Absyn_Var_e){ goto _LL230;} else{ goto _LL231;} _LL231: if(*(( int*)
_temp225) ==  Cyc_Absyn_Const_e){ goto _LL232;} else{ goto _LL233;} _LL233: if(*((
int*) _temp225) ==  Cyc_Absyn_FnCall_e){ _LL285: _temp284=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp225)->f1; goto _LL283; _LL283: _temp282=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp225)->f2; goto _LL234;} else{ goto _LL235;} _LL235: if(*(( int*) _temp225)
==  Cyc_Absyn_Primop_e){ _LL287: _temp286=(( struct Cyc_Absyn_Primop_e_struct*)
_temp225)->f2; goto _LL236;} else{ goto _LL237;} _LL237: if(*(( int*) _temp225)
==  Cyc_Absyn_Conditional_e){ _LL293: _temp292=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp225)->f1; goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp225)->f2; goto _LL289; _LL289: _temp288=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp225)->f3; goto _LL238;} else{ goto _LL239;} _LL239: if(*(( int*) _temp225)
==  Cyc_Absyn_Subscript_e){ _LL297: _temp296=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp225)->f1; goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp225)->f2; goto _LL240;} else{ goto _LL241;} _LL241: if(*(( int*) _temp225)
==  Cyc_Absyn_SeqExp_e){ _LL301: _temp300=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp225)->f1; goto _LL299; _LL299: _temp298=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp225)->f2; goto _LL242;} else{ goto _LL243;} _LL243: if(*(( int*) _temp225)
==  Cyc_Absyn_AssignOp_e){ _LL305: _temp304=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp225)->f1; goto _LL303; _LL303: _temp302=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp225)->f3; goto _LL244;} else{ goto _LL245;} _LL245: if(*(( int*) _temp225)
==  Cyc_Absyn_NoInstantiate_e){ _LL307: _temp306=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp225)->f1; goto _LL246;} else{ goto _LL247;} _LL247: if(*(( int*) _temp225)
==  Cyc_Absyn_Instantiate_e){ _LL309: _temp308=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp225)->f1; goto _LL248;} else{ goto _LL249;} _LL249: if(*(( int*) _temp225)
==  Cyc_Absyn_Cast_e){ _LL311: _temp310=(( struct Cyc_Absyn_Cast_e_struct*)
_temp225)->f2; goto _LL250;} else{ goto _LL251;} _LL251: if(*(( int*) _temp225)
==  Cyc_Absyn_Address_e){ _LL313: _temp312=(( struct Cyc_Absyn_Address_e_struct*)
_temp225)->f1; goto _LL252;} else{ goto _LL253;} _LL253: if(*(( int*) _temp225)
==  Cyc_Absyn_Sizeofexp_e){ _LL315: _temp314=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp225)->f1; goto _LL254;} else{ goto _LL255;} _LL255: if(*(( int*) _temp225)
==  Cyc_Absyn_Deref_e){ _LL317: _temp316=(( struct Cyc_Absyn_Deref_e_struct*)
_temp225)->f1; goto _LL256;} else{ goto _LL257;} _LL257: if(*(( int*) _temp225)
==  Cyc_Absyn_StructMember_e){ _LL319: _temp318=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp225)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(*(( int*) _temp225)
==  Cyc_Absyn_StructArrow_e){ _LL321: _temp320=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp225)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp225)
==  Cyc_Absyn_Increment_e){ _LL323: _temp322=(( struct Cyc_Absyn_Increment_e_struct*)
_temp225)->f1; goto _LL262;} else{ goto _LL263;} _LL263: if(*(( int*) _temp225)
==  Cyc_Absyn_Sizeoftyp_e){ goto _LL264;} else{ goto _LL265;} _LL265: if(*(( int*)
_temp225) ==  Cyc_Absyn_Offsetof_e){ goto _LL266;} else{ goto _LL267;} _LL267:
if(*(( int*) _temp225) ==  Cyc_Absyn_Gentyp_e){ goto _LL268;} else{ goto _LL269;}
_LL269: if(*(( int*) _temp225) ==  Cyc_Absyn_Enum_e){ goto _LL270;} else{ goto
_LL271;} _LL271: if(*(( int*) _temp225) ==  Cyc_Absyn_UnresolvedMem_e){ _LL325:
_temp324=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp225)->f2; goto _LL272;}
else{ goto _LL273;} _LL273: goto _LL274; _LL228: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_member)( varmap, _temp278)){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( varmap, _temp278))->r));} return; _LL230:
return; _LL232: return; _LL234: Cyc_Tovc_apply_varmap( varmap, _temp284);
_temp286= _temp282; goto _LL236; _LL236: for( 0; _temp286 !=  0; _temp286=
_temp286->tl){ Cyc_Tovc_apply_varmap( varmap,( struct Cyc_Absyn_Exp*) _temp286->hd);}
return; _LL238: Cyc_Tovc_apply_varmap( varmap, _temp288); _temp296= _temp292;
_temp294= _temp290; goto _LL240; _LL240: _temp300= _temp296; _temp298= _temp294;
goto _LL242; _LL242: _temp304= _temp300; _temp302= _temp298; goto _LL244; _LL244:
Cyc_Tovc_apply_varmap( varmap, _temp302); _temp306= _temp304; goto _LL246;
_LL246: _temp308= _temp306; goto _LL248; _LL248: _temp310= _temp308; goto _LL250;
_LL250: _temp312= _temp310; goto _LL252; _LL252: _temp314= _temp312; goto _LL254;
_LL254: _temp316= _temp314; goto _LL256; _LL256: _temp318= _temp316; goto _LL258;
_LL258: _temp320= _temp318; goto _LL260; _LL260: _temp322= _temp320; goto _LL262;
_LL262: Cyc_Tovc_apply_varmap( varmap, _temp322); return; _LL264: return; _LL266:
return; _LL268: return; _LL270: return; _LL272: for( 0; _temp324 !=  0; _temp324=
_temp324->tl){ Cyc_Tovc_apply_varmap( varmap,(*(( struct _tuple3*) _temp324->hd)).f2);}
return; _LL274:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp326=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp326[ 0]=({ struct Cyc_Core_Impossible_struct _temp327; _temp327.tag= Cyc_Core_Impossible;
_temp327.f1= _tag_arr("bad exp form in Tocv::apply_varmap", sizeof(
unsigned char), 35u); _temp327;}); _temp326;})); _LL226:;} static struct Cyc_Absyn_Exp*
Cyc_Tovc_stmt_to_exp( struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s){
void* _temp328=( void*) s->r; struct Cyc_Absyn_Exp* _temp342; struct Cyc_Absyn_Stmt*
_temp344; struct Cyc_Absyn_Stmt* _temp346; struct Cyc_Absyn_Stmt* _temp348;
struct Cyc_Absyn_Stmt* _temp350; struct Cyc_Absyn_Exp* _temp352; struct Cyc_Absyn_Stmt*
_temp354; struct Cyc_Absyn_Decl* _temp356; _LL330: if( _temp328 == ( void*) Cyc_Absyn_Skip_s){
goto _LL331;} else{ goto _LL332;} _LL332: if(( unsigned int) _temp328 >  1u?*((
int*) _temp328) ==  Cyc_Absyn_Exp_s: 0){ _LL343: _temp342=(( struct Cyc_Absyn_Exp_s_struct*)
_temp328)->f1; goto _LL333;} else{ goto _LL334;} _LL334: if(( unsigned int)
_temp328 >  1u?*(( int*) _temp328) ==  Cyc_Absyn_Seq_s: 0){ _LL347: _temp346=((
struct Cyc_Absyn_Seq_s_struct*) _temp328)->f1; goto _LL345; _LL345: _temp344=((
struct Cyc_Absyn_Seq_s_struct*) _temp328)->f2; goto _LL335;} else{ goto _LL336;}
_LL336: if(( unsigned int) _temp328 >  1u?*(( int*) _temp328) ==  Cyc_Absyn_IfThenElse_s:
0){ _LL353: _temp352=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp328)->f1;
goto _LL351; _LL351: _temp350=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp328)->f2;
goto _LL349; _LL349: _temp348=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp328)->f3;
goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int) _temp328 >  1u?*((
int*) _temp328) ==  Cyc_Absyn_Decl_s: 0){ _LL357: _temp356=(( struct Cyc_Absyn_Decl_s_struct*)
_temp328)->f1; goto _LL355; _LL355: _temp354=(( struct Cyc_Absyn_Decl_s_struct*)
_temp328)->f2; goto _LL339;} else{ goto _LL340;} _LL340: goto _LL341; _LL331:
return Cyc_Absyn_true_exp( 0); _LL333: Cyc_Tovc_apply_varmap( env.varmap,
_temp342); return _temp342; _LL335: return Cyc_Absyn_seq_exp( Cyc_Tovc_stmt_to_exp(
env, _temp346), Cyc_Tovc_stmt_to_exp( env, _temp344), 0); _LL337: Cyc_Tovc_apply_varmap(
env.varmap, _temp352); return Cyc_Absyn_conditional_exp( _temp352, Cyc_Tovc_stmt_to_exp(
env, _temp350), Cyc_Tovc_stmt_to_exp( env, _temp348), 0); _LL339: { void*
_temp358=( void*) _temp356->r; struct Cyc_Absyn_Vardecl* _temp364; _LL360: if(*((
int*) _temp358) ==  Cyc_Absyn_Var_d){ _LL365: _temp364=(( struct Cyc_Absyn_Var_d_struct*)
_temp358)->f1; goto _LL361;} else{ goto _LL362;} _LL362: goto _LL363; _LL361: {
struct _tagged_arr* _temp366=(* _temp364->name).f2; struct _tuple0* _temp367=
Cyc_Toc_temp_var(); _temp364->name= _temp367; env.varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Exp* v)) Cyc_Dict_insert)(
env.varmap, _temp366, Cyc_Absyn_var_exp( _temp367, 0)); if( _temp364->initializer
!=  0){ Cyc_Tovc_apply_varmap( env.varmap,( struct Cyc_Absyn_Exp*) _check_null(
_temp364->initializer));{ void* _temp368=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp364->initializer))->r; _LL370: if(*(( int*) _temp368) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL371;} else{ goto _LL372;} _LL372: goto _LL373; _LL371: { struct _tuple0*
tmp= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* body= Cyc_Absyn_declare_stmt(
tmp,( void*) _temp364->type, _temp364->initializer, Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( tmp, 0), 0), 0); _temp364->initializer=(
struct Cyc_Absyn_Exp*) Cyc_Tovc_stmt_to_fun( env, body,( void*) _temp364->type);
goto _LL369;} _LL373: goto _LL369; _LL369:;}} env.all_locals=({ struct Cyc_List_List*
_temp374=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp374->hd=( void*) _temp364; _temp374->tl= env.all_locals; _temp374;});{
struct Cyc_Absyn_Exp* _temp375= _temp364->initializer; _temp364->initializer= 0;(
void*)(( env.encloser)->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp356, Cyc_Absyn_new_stmt((
void*)( env.encloser)->r, 0), 0))->r));{ struct Cyc_Absyn_Exp* _temp376= Cyc_Tovc_stmt_to_exp(
env, _temp354); if( _temp375 !=  0){ _temp376= Cyc_Absyn_seq_exp( Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp364->name, 0),( struct Cyc_Absyn_Exp*) _check_null(
_temp375), 0), _temp376, 0);} return _temp376;}}} _LL363:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp377=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp377[ 0]=({ struct
Cyc_Core_Impossible_struct _temp378; _temp378.tag= Cyc_Core_Impossible; _temp378.f1=
_tag_arr("bad local decl in Tovc::stmt_to_exp", sizeof( unsigned char), 36u);
_temp378;}); _temp377;})); _LL359:;} _LL341: return Cyc_Tovc_stmt_to_fun( env, s,(
void*) Cyc_Absyn_VoidType); _LL329:;} static void Cyc_Tovc_stmt_to_vc( struct
Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s); static void Cyc_Tovc_exp_to_vc(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Exp* e){ void* _temp379=( void*)
e->r; struct Cyc_List_List* _temp429; struct Cyc_Absyn_Exp* _temp431; struct Cyc_List_List*
_temp433; struct Cyc_Absyn_Exp* _temp435; struct Cyc_Absyn_Exp* _temp437; struct
Cyc_Absyn_Exp* _temp439; struct Cyc_Absyn_Exp* _temp441; struct Cyc_Absyn_Exp*
_temp443; struct Cyc_Absyn_Exp* _temp445; struct Cyc_Absyn_Exp* _temp447; struct
Cyc_Absyn_Exp* _temp449; struct Cyc_Absyn_Exp* _temp451; struct Cyc_Absyn_Exp*
_temp453; struct Cyc_Absyn_Exp* _temp455; struct Cyc_Absyn_Exp* _temp457; struct
Cyc_Absyn_Exp* _temp459; struct Cyc_Absyn_Exp* _temp461; struct Cyc_Absyn_Exp*
_temp463; struct Cyc_Absyn_Exp* _temp465; struct Cyc_Absyn_Exp* _temp467; struct
Cyc_Absyn_Exp* _temp469; struct Cyc_List_List* _temp471; struct Cyc_Absyn_Stmt*
_temp473; _LL381: if(*(( int*) _temp379) ==  Cyc_Absyn_Const_e){ goto _LL382;}
else{ goto _LL383;} _LL383: if(*(( int*) _temp379) ==  Cyc_Absyn_Var_e){ goto
_LL384;} else{ goto _LL385;} _LL385: if(*(( int*) _temp379) ==  Cyc_Absyn_FnCall_e){
_LL432: _temp431=(( struct Cyc_Absyn_FnCall_e_struct*) _temp379)->f1; goto
_LL430; _LL430: _temp429=(( struct Cyc_Absyn_FnCall_e_struct*) _temp379)->f2;
goto _LL386;} else{ goto _LL387;} _LL387: if(*(( int*) _temp379) ==  Cyc_Absyn_Primop_e){
_LL434: _temp433=(( struct Cyc_Absyn_Primop_e_struct*) _temp379)->f2; goto
_LL388;} else{ goto _LL389;} _LL389: if(*(( int*) _temp379) ==  Cyc_Absyn_Conditional_e){
_LL440: _temp439=(( struct Cyc_Absyn_Conditional_e_struct*) _temp379)->f1; goto
_LL438; _LL438: _temp437=(( struct Cyc_Absyn_Conditional_e_struct*) _temp379)->f2;
goto _LL436; _LL436: _temp435=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp379)->f3; goto _LL390;} else{ goto _LL391;} _LL391: if(*(( int*) _temp379)
==  Cyc_Absyn_Subscript_e){ _LL444: _temp443=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp379)->f1; goto _LL442; _LL442: _temp441=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp379)->f2; goto _LL392;} else{ goto _LL393;} _LL393: if(*(( int*) _temp379)
==  Cyc_Absyn_SeqExp_e){ _LL448: _temp447=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp379)->f1; goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp379)->f2; goto _LL394;} else{ goto _LL395;} _LL395: if(*(( int*) _temp379)
==  Cyc_Absyn_AssignOp_e){ _LL452: _temp451=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp379)->f1; goto _LL450; _LL450: _temp449=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp379)->f3; goto _LL396;} else{ goto _LL397;} _LL397: if(*(( int*) _temp379)
==  Cyc_Absyn_NoInstantiate_e){ _LL454: _temp453=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp379)->f1; goto _LL398;} else{ goto _LL399;} _LL399: if(*(( int*) _temp379)
==  Cyc_Absyn_Instantiate_e){ _LL456: _temp455=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp379)->f1; goto _LL400;} else{ goto _LL401;} _LL401: if(*(( int*) _temp379)
==  Cyc_Absyn_Cast_e){ _LL458: _temp457=(( struct Cyc_Absyn_Cast_e_struct*)
_temp379)->f2; goto _LL402;} else{ goto _LL403;} _LL403: if(*(( int*) _temp379)
==  Cyc_Absyn_Address_e){ _LL460: _temp459=(( struct Cyc_Absyn_Address_e_struct*)
_temp379)->f1; goto _LL404;} else{ goto _LL405;} _LL405: if(*(( int*) _temp379)
==  Cyc_Absyn_Sizeofexp_e){ _LL462: _temp461=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp379)->f1; goto _LL406;} else{ goto _LL407;} _LL407: if(*(( int*) _temp379)
==  Cyc_Absyn_Deref_e){ _LL464: _temp463=(( struct Cyc_Absyn_Deref_e_struct*)
_temp379)->f1; goto _LL408;} else{ goto _LL409;} _LL409: if(*(( int*) _temp379)
==  Cyc_Absyn_StructMember_e){ _LL466: _temp465=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp379)->f1; goto _LL410;} else{ goto _LL411;} _LL411: if(*(( int*) _temp379)
==  Cyc_Absyn_StructArrow_e){ _LL468: _temp467=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp379)->f1; goto _LL412;} else{ goto _LL413;} _LL413: if(*(( int*) _temp379)
==  Cyc_Absyn_Increment_e){ _LL470: _temp469=(( struct Cyc_Absyn_Increment_e_struct*)
_temp379)->f1; goto _LL414;} else{ goto _LL415;} _LL415: if(*(( int*) _temp379)
==  Cyc_Absyn_Sizeoftyp_e){ goto _LL416;} else{ goto _LL417;} _LL417: if(*(( int*)
_temp379) ==  Cyc_Absyn_Offsetof_e){ goto _LL418;} else{ goto _LL419;} _LL419:
if(*(( int*) _temp379) ==  Cyc_Absyn_Gentyp_e){ goto _LL420;} else{ goto _LL421;}
_LL421: if(*(( int*) _temp379) ==  Cyc_Absyn_Enum_e){ goto _LL422;} else{ goto
_LL423;} _LL423: if(*(( int*) _temp379) ==  Cyc_Absyn_UnresolvedMem_e){ _LL472:
_temp471=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp379)->f2; goto _LL424;}
else{ goto _LL425;} _LL425: if(*(( int*) _temp379) ==  Cyc_Absyn_StmtExp_e){
_LL474: _temp473=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp379)->f1; goto
_LL426;} else{ goto _LL427;} _LL427: goto _LL428; _LL382: return; _LL384:
return; _LL386: Cyc_Tovc_exp_to_vc( env, _temp431); _temp433= _temp429; goto
_LL388; _LL388: for( 0; _temp433 !=  0; _temp433= _temp433->tl){ Cyc_Tovc_exp_to_vc(
env,( struct Cyc_Absyn_Exp*) _temp433->hd);} return; _LL390: Cyc_Tovc_exp_to_vc(
env, _temp435); _temp443= _temp439; _temp441= _temp437; goto _LL392; _LL392:
_temp447= _temp443; _temp445= _temp441; goto _LL394; _LL394: _temp451= _temp447;
_temp449= _temp445; goto _LL396; _LL396: Cyc_Tovc_exp_to_vc( env, _temp449);
_temp453= _temp451; goto _LL398; _LL398: _temp455= _temp453; goto _LL400; _LL400:
_temp457= _temp455; goto _LL402; _LL402: _temp459= _temp457; goto _LL404; _LL404:
_temp461= _temp459; goto _LL406; _LL406: _temp463= _temp461; goto _LL408; _LL408:
_temp465= _temp463; goto _LL410; _LL410: _temp467= _temp465; goto _LL412; _LL412:
_temp469= _temp467; goto _LL414; _LL414: Cyc_Tovc_exp_to_vc( env, _temp469);
return; _LL416: return; _LL418: return; _LL420: return; _LL422: return; _LL424:
for( 0; _temp471 !=  0; _temp471= _temp471->tl){ Cyc_Tovc_exp_to_vc( env,(*((
struct _tuple3*) _temp471->hd)).f2);} return; _LL426: Cyc_Tovc_stmt_to_vc( env,
_temp473);( void*)( e->r=( void*)(( void*)( Cyc_Tovc_stmt_to_exp( env, _temp473))->r));
return; _LL428:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp475=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp475[ 0]=({ struct Cyc_Core_Impossible_struct _temp476; _temp476.tag= Cyc_Core_Impossible;
_temp476.f1= _tag_arr("bad exp form after xlation to C", sizeof( unsigned char),
32u); _temp476;}); _temp475;})); _LL380:;} static void Cyc_Tovc_stmt_to_vc(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s){ while( 1) { env.encloser=
s;{ void* _temp477=( void*) s->r; struct Cyc_Absyn_Stmt* _temp509; struct Cyc_Absyn_Exp*
_temp511; struct Cyc_Absyn_Exp* _temp513; struct Cyc_Absyn_Stmt* _temp515;
struct Cyc_Absyn_Stmt* _temp517; struct Cyc_Absyn_Stmt* _temp519; struct Cyc_Absyn_Stmt*
_temp521; struct Cyc_Absyn_Exp* _temp523; struct Cyc_Absyn_Stmt* _temp525;
struct _tuple2 _temp527; struct Cyc_Absyn_Exp* _temp529; struct Cyc_Absyn_Stmt*
_temp531; struct _tuple2 _temp533; struct Cyc_Absyn_Exp* _temp535; struct
_tuple2 _temp537; struct Cyc_Absyn_Exp* _temp539; struct Cyc_Absyn_Exp* _temp541;
struct _tuple2 _temp543; struct Cyc_Absyn_Exp* _temp545; struct Cyc_Absyn_Stmt*
_temp547; struct Cyc_List_List* _temp549; struct Cyc_Absyn_Exp* _temp551; struct
Cyc_Absyn_Stmt* _temp553; struct Cyc_Absyn_Decl* _temp555; _LL479: if( _temp477
== ( void*) Cyc_Absyn_Skip_s){ goto _LL480;} else{ goto _LL481;} _LL481: if((
unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Break_s: 0){ goto
_LL482;} else{ goto _LL483;} _LL483: if(( unsigned int) _temp477 >  1u?*(( int*)
_temp477) ==  Cyc_Absyn_Continue_s: 0){ goto _LL484;} else{ goto _LL485;} _LL485:
if(( unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Goto_s: 0){
goto _LL486;} else{ goto _LL487;} _LL487: if(( unsigned int) _temp477 >  1u?*((
int*) _temp477) ==  Cyc_Absyn_Label_s: 0){ _LL510: _temp509=(( struct Cyc_Absyn_Label_s_struct*)
_temp477)->f2; goto _LL488;} else{ goto _LL489;} _LL489: if(( unsigned int)
_temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Return_s: 0){ _LL512: _temp511=((
struct Cyc_Absyn_Return_s_struct*) _temp477)->f1; goto _LL490;} else{ goto
_LL491;} _LL491: if(( unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Exp_s:
0){ _LL514: _temp513=(( struct Cyc_Absyn_Exp_s_struct*) _temp477)->f1; goto
_LL492;} else{ goto _LL493;} _LL493: if(( unsigned int) _temp477 >  1u?*(( int*)
_temp477) ==  Cyc_Absyn_Seq_s: 0){ _LL518: _temp517=(( struct Cyc_Absyn_Seq_s_struct*)
_temp477)->f1; goto _LL516; _LL516: _temp515=(( struct Cyc_Absyn_Seq_s_struct*)
_temp477)->f2; goto _LL494;} else{ goto _LL495;} _LL495: if(( unsigned int)
_temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL524:
_temp523=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp477)->f1; goto _LL522;
_LL522: _temp521=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp477)->f2; goto
_LL520; _LL520: _temp519=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp477)->f3;
goto _LL496;} else{ goto _LL497;} _LL497: if(( unsigned int) _temp477 >  1u?*((
int*) _temp477) ==  Cyc_Absyn_While_s: 0){ _LL528: _temp527=(( struct Cyc_Absyn_While_s_struct*)
_temp477)->f1; _LL530: _temp529= _temp527.f1; goto _LL526; _LL526: _temp525=((
struct Cyc_Absyn_While_s_struct*) _temp477)->f2; goto _LL498;} else{ goto _LL499;}
_LL499: if(( unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_For_s:
0){ _LL542: _temp541=(( struct Cyc_Absyn_For_s_struct*) _temp477)->f1; goto
_LL538; _LL538: _temp537=(( struct Cyc_Absyn_For_s_struct*) _temp477)->f2;
_LL540: _temp539= _temp537.f1; goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_For_s_struct*)
_temp477)->f3; _LL536: _temp535= _temp533.f1; goto _LL532; _LL532: _temp531=((
struct Cyc_Absyn_For_s_struct*) _temp477)->f4; goto _LL500;} else{ goto _LL501;}
_LL501: if(( unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Do_s:
0){ _LL548: _temp547=(( struct Cyc_Absyn_Do_s_struct*) _temp477)->f1; goto
_LL544; _LL544: _temp543=(( struct Cyc_Absyn_Do_s_struct*) _temp477)->f2; _LL546:
_temp545= _temp543.f1; goto _LL502;} else{ goto _LL503;} _LL503: if((
unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Switch_s: 0){
_LL552: _temp551=(( struct Cyc_Absyn_Switch_s_struct*) _temp477)->f1; goto
_LL550; _LL550: _temp549=(( struct Cyc_Absyn_Switch_s_struct*) _temp477)->f2;
goto _LL504;} else{ goto _LL505;} _LL505: if(( unsigned int) _temp477 >  1u?*((
int*) _temp477) ==  Cyc_Absyn_Decl_s: 0){ _LL556: _temp555=(( struct Cyc_Absyn_Decl_s_struct*)
_temp477)->f1; goto _LL554; _LL554: _temp553=(( struct Cyc_Absyn_Decl_s_struct*)
_temp477)->f2; goto _LL506;} else{ goto _LL507;} _LL507: goto _LL508; _LL480:
return; _LL482: return; _LL484: return; _LL486: return; _LL488: s= _temp509;
continue; _LL490: if( _temp511 ==  0){ return;} _temp513=( struct Cyc_Absyn_Exp*)
_check_null( _temp511); goto _LL492; _LL492: Cyc_Tovc_exp_to_vc( env, _temp513);
return; _LL494: Cyc_Tovc_stmt_to_vc( env, _temp517); s= _temp515; continue;
_LL496: Cyc_Tovc_exp_to_vc( env, _temp523); Cyc_Tovc_stmt_to_vc( env, _temp521);
s= _temp519; continue; _LL498: Cyc_Tovc_exp_to_vc( env, _temp529); s= _temp525;
continue; _LL500: Cyc_Tovc_exp_to_vc( env, _temp541); Cyc_Tovc_exp_to_vc( env,
_temp539); Cyc_Tovc_exp_to_vc( env, _temp535); s= _temp531; continue; _LL502:
Cyc_Tovc_exp_to_vc( env, _temp545); s= _temp547; continue; _LL504: Cyc_Tovc_exp_to_vc(
env, _temp551); for( 0; _temp549 !=  0; _temp549= _temp549->tl){ Cyc_Tovc_stmt_to_vc(
env,(( struct Cyc_Absyn_Switch_clause*) _temp549->hd)->body);} return; _LL506:{
void* _temp557=( void*) _temp555->r; struct Cyc_Absyn_Vardecl* _temp563; _LL559:
if(*(( int*) _temp557) ==  Cyc_Absyn_Var_d){ _LL564: _temp563=(( struct Cyc_Absyn_Var_d_struct*)
_temp557)->f1; goto _LL560;} else{ goto _LL561;} _LL561: goto _LL562; _LL560:{
void* _temp565= Cyc_Tcutil_compress(( void*) _temp563->type); _LL567: if((
unsigned int) _temp565 >  3u?*(( int*) _temp565) ==  Cyc_Absyn_ArrayType: 0){
goto _LL568;} else{ goto _LL569;} _LL569: goto _LL570; _LL568: if( _temp563->initializer
!=  0){ void* _temp571=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp563->initializer))->r;
struct Cyc_List_List* _temp577; _LL573: if(*(( int*) _temp571) ==  Cyc_Absyn_UnresolvedMem_e){
_LL578: _temp577=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp571)->f2;
goto _LL574;} else{ goto _LL575;} _LL575: goto _LL576; _LL574: _temp563->initializer=
0;{ struct Cyc_Absyn_Exp* arr= Cyc_Absyn_var_exp( _temp563->name, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_new_stmt(( void*) _temp553->r, 0);{ unsigned int i= 0; for( 0;
_temp577 !=  0;( _temp577= _temp577->tl, ++ i)){ s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( Cyc_Absyn_subscript_exp( arr, Cyc_Absyn_uint_exp( i, 0), 0),(*((
struct _tuple3*) _temp577->hd)).f2, 0), 0), s3, 0);}}( void*)( _temp553->r=(
void*)(( void*) s3->r)); goto _LL572;} _LL576:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp579=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp579[ 0]=({ struct
Cyc_Core_Impossible_struct _temp580; _temp580.tag= Cyc_Core_Impossible; _temp580.f1=
_tag_arr("array type bad initializer", sizeof( unsigned char), 27u); _temp580;});
_temp579;})); _LL572:;} goto _LL566; _LL570: goto _LL566; _LL566:;} env.all_locals=({
struct Cyc_List_List* _temp581=( struct Cyc_List_List*) _region_malloc( env.rgn,
sizeof( struct Cyc_List_List)); _temp581->hd=( void*) _temp563; _temp581->tl=
env.all_locals; _temp581;}); if( _temp563->initializer !=  0){ Cyc_Tovc_exp_to_vc(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp563->initializer));} s= _temp553;
continue; _LL562: goto _LL558; _LL558:;} goto _LL508; _LL508:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp582=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp582[ 0]=({ struct
Cyc_Core_Impossible_struct _temp583; _temp583.tag= Cyc_Core_Impossible; _temp583.f1=
_tag_arr("bad stmt after xlation to C", sizeof( unsigned char), 28u); _temp583;});
_temp582;})); _LL478:;}}} struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List*
old_ds){ struct Cyc_List_List* _temp584= 0; for( 0; old_ds !=  0; old_ds= old_ds->tl){
struct Cyc_Absyn_Decl* _temp585=( struct Cyc_Absyn_Decl*) old_ds->hd; void*
_temp586=( void*) _temp585->r; struct Cyc_Absyn_Fndecl* _temp612; struct Cyc_List_List*
_temp614; struct Cyc_List_List* _temp616; struct Cyc_List_List* _temp618; _LL588:
if(*(( int*) _temp586) ==  Cyc_Absyn_Fn_d){ _LL613: _temp612=(( struct Cyc_Absyn_Fn_d_struct*)
_temp586)->f1; goto _LL589;} else{ goto _LL590;} _LL590: if(*(( int*) _temp586)
==  Cyc_Absyn_Var_d){ goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*)
_temp586) ==  Cyc_Absyn_Let_d){ goto _LL593;} else{ goto _LL594;} _LL594: if(*((
int*) _temp586) ==  Cyc_Absyn_Letv_d){ goto _LL595;} else{ goto _LL596;} _LL596:
if(*(( int*) _temp586) ==  Cyc_Absyn_Struct_d){ goto _LL597;} else{ goto _LL598;}
_LL598: if(*(( int*) _temp586) ==  Cyc_Absyn_Union_d){ goto _LL599;} else{ goto
_LL600;} _LL600: if(*(( int*) _temp586) ==  Cyc_Absyn_Tunion_d){ goto _LL601;}
else{ goto _LL602;} _LL602: if(*(( int*) _temp586) ==  Cyc_Absyn_Enum_d){ goto
_LL603;} else{ goto _LL604;} _LL604: if(*(( int*) _temp586) ==  Cyc_Absyn_Typedef_d){
goto _LL605;} else{ goto _LL606;} _LL606: if(*(( int*) _temp586) ==  Cyc_Absyn_Namespace_d){
_LL615: _temp614=(( struct Cyc_Absyn_Namespace_d_struct*) _temp586)->f2; goto
_LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp586) ==  Cyc_Absyn_Using_d){
_LL617: _temp616=(( struct Cyc_Absyn_Using_d_struct*) _temp586)->f2; goto _LL609;}
else{ goto _LL610;} _LL610: if(*(( int*) _temp586) ==  Cyc_Absyn_ExternC_d){
_LL619: _temp618=(( struct Cyc_Absyn_ExternC_d_struct*) _temp586)->f1; goto
_LL611;} else{ goto _LL587;} _LL589:{ struct _RegionHandle _temp620= _new_region("rgn");
struct _RegionHandle* rgn=& _temp620; _push_region( rgn);{ struct Cyc_List_List**
_temp621=({ struct Cyc_List_List** _temp623=( struct Cyc_List_List**)
_region_malloc( rgn, sizeof( struct Cyc_List_List*)); _temp623[ 0]= 0; _temp623;});
Cyc_Tovc_stmt_to_vc(({ struct Cyc_Tovc_ToExpEnv _temp622; _temp622.rgn= rgn;
_temp622.all_locals=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
Cyc_List_List* x)) Cyc_List_rcopy)( rgn,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp612->param_vardecls))->v); _temp622.varmap=(( struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*, int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_rempty)( rgn, Cyc_Std_zstrptrcmp); _temp622.encloser= _temp612->body;
_temp622.gen_ds= _temp621; _temp622;}), _temp612->body); _temp584=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(* _temp621,
_temp584);}; _pop_region( rgn);} goto _LL591; _LL591: goto _LL593; _LL593: goto
_LL595; _LL595: goto _LL597; _LL597: goto _LL599; _LL599: goto _LL601; _LL601:
goto _LL603; _LL603: goto _LL605; _LL605: _temp584=({ struct Cyc_List_List*
_temp624=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp624->hd=( void*) _temp585; _temp624->tl= _temp584; _temp624;}); goto _LL587;
_LL607: _temp616= _temp614; goto _LL609; _LL609: _temp618= _temp616; goto _LL611;
_LL611:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp625=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp625[ 0]=({ struct Cyc_Core_Impossible_struct _temp626; _temp626.tag= Cyc_Core_Impossible;
_temp626.f1= _tag_arr("nested translation unit after translation to C", sizeof(
unsigned char), 47u); _temp626;}); _temp625;})); _LL587:;} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp584);}
