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
Cyc_Absyn_AnonStructType= 13; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 14;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 15; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
16; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 17; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 18;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
19; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 20; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple0* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
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
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_typ_kind( void* t); extern
void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify( void*, void*);
extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void* Cyc_Tcutil_snd_tqt(
struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
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
Cyc_Toc__cycalloc_ev; static unsigned char _temp45[ 17u]="_cycalloc_atomic";
static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={ _temp45, _temp45,
_temp45 +  17u}; static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={ 1u,& Cyc_Toc__cycalloc_atomic_pr,( void*)(( void*)
0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={ 0,( void*)((
void*)& Cyc_Toc__cycalloc_atomic_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static unsigned char _temp47[ 15u]="_region_malloc"; static struct _tagged_arr
Cyc_Toc__region_malloc_str={ _temp47, _temp47, _temp47 +  15u}; static struct
_tuple0 Cyc_Toc__region_malloc_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;
static unsigned char _temp49[ 13u]="_check_times"; static struct _tagged_arr Cyc_Toc__check_times_str={
_temp49, _temp49, _temp49 +  13u}; static struct _tuple0 Cyc_Toc__check_times_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={ 1u,& Cyc_Toc__check_times_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_times_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;
static unsigned char _temp51[ 12u]="_new_region"; static struct _tagged_arr Cyc_Toc__new_region_str={
_temp51, _temp51, _temp51 +  12u}; static struct _tuple0 Cyc_Toc__new_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={ 1u,& Cyc_Toc__new_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__new_region_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp53[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp53, _temp53, _temp53 +  13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__push_region_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;
static unsigned char _temp55[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_str={
_temp55, _temp55, _temp55 +  12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={ 1u,& Cyc_Toc__pop_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__pop_region_re), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static struct Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={ 10u,(
struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0}; static void* Cyc_Toc_tagged_arr_typ=(
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
for( 0; _temp112 !=  0; _temp112= _temp112->tl){ if( ! Cyc_Toc_is_zero((*((
struct _tuple4*) _temp112->hd)).f2)){ return 0;}} return 1; _LL83: return 0;
_LL59:;} static int Cyc_Toc_is_nullable( void* t){ void* _temp114= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp120; struct Cyc_Absyn_Conref* _temp122; _LL116:
if(( unsigned int) _temp114 >  3u?*(( int*) _temp114) ==  Cyc_Absyn_PointerType:
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
0; _temp127 !=  0; _temp127= _temp127->tl){ struct _tuple5 _temp130; struct Cyc_List_List*
_temp131; struct _tagged_arr* _temp133; struct _tuple5* _temp128=( struct
_tuple5*) _temp127->hd; _temp130=* _temp128; _LL134: _temp133= _temp130.f1; goto
_LL132; _LL132: _temp131= _temp130.f2; goto _LL129; _LL129: { struct Cyc_List_List*
_temp135= tqs0; for( 0; _temp135 !=  0? _temp131 !=  0: 0;( _temp135= _temp135->tl,
_temp131= _temp131->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*) _temp135->hd)).f2,(
void*) _temp131->hd)){ break;}} if( _temp135 ==  0? _temp131 ==  0: 0){ return
_temp133;}}}}{ struct _tagged_arr* x=({ struct _tagged_arr* _temp149=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp149[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp151; _temp151.tag= Cyc_Std_Int_pa;
_temp151.f1=( int)(( unsigned int) Cyc_Toc_tuple_type_counter ++);{ void*
_temp150[ 1u]={& _temp151}; Cyc_Std_aprintf( _tag_arr("_tuple%d", sizeof(
unsigned char), 9u), _tag_arr( _temp150, sizeof( void*), 1u));}}); _temp149;});
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List*
_temp136= 0; struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 !=  0;( ts2=
ts2->tl, i ++)){ _temp136=({ struct Cyc_List_List* _temp137=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp137->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp138=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp138->name= Cyc_Absyn_fieldname( i); _temp138->tq= Cyc_Toc_mt_tq; _temp138->type=(
void*)(( void*) ts2->hd); _temp138->width= 0; _temp138->attributes= 0; _temp138;});
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
void* _temp192; _LL177: if(( unsigned int) _temp175 >  3u?*(( int*) _temp175) == 
Cyc_Absyn_ArrayType: 0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f1; goto _LL186; _LL186: _temp185=(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f2; goto _LL184; _LL184: _temp183=(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f3; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp175 >  3u?*(( int*) _temp175) ==  Cyc_Absyn_Evar: 0){ _LL190: _temp189=((
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
_temp196;});} static void Cyc_Toc_enumfields_to_c( struct Cyc_List_List* fs){
struct Cyc_List_List* _temp197= fs; for( 0; _temp197 !=  0; _temp197= _temp197->tl){
if((( struct Cyc_Absyn_Enumfield*) _temp197->hd)->tag !=  0){ unsigned int i=
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)
_temp197->hd)->tag));(( struct Cyc_Absyn_Enumfield*) _temp197->hd)->tag=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( i, 0);}}} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp198= t; struct Cyc_Core_Opt* _temp254; struct Cyc_Core_Opt*
_temp256; struct Cyc_Core_Opt _temp258; void* _temp259; struct Cyc_Absyn_TunionInfo
_temp261; void* _temp263; struct Cyc_Absyn_TunionFieldInfo _temp265; void*
_temp267; struct Cyc_Absyn_Tunionfield* _temp269; struct Cyc_Absyn_Tuniondecl*
_temp271; struct Cyc_Absyn_PtrInfo _temp273; struct Cyc_Absyn_Conref* _temp275;
struct Cyc_Absyn_Tqual _temp277; void* _temp279; struct Cyc_Absyn_Exp* _temp281;
struct Cyc_Absyn_Tqual _temp283; void* _temp285; struct Cyc_Absyn_FnInfo
_temp287; struct Cyc_List_List* _temp289; struct Cyc_Absyn_VarargInfo* _temp291;
int _temp293; struct Cyc_List_List* _temp295; void* _temp297; struct Cyc_List_List*
_temp299; struct Cyc_List_List* _temp301; struct Cyc_List_List* _temp303; struct
_tuple0* _temp305; struct _tuple0* _temp307; struct _tuple0* _temp309; struct
Cyc_List_List* _temp311; struct Cyc_Core_Opt* _temp313; struct Cyc_List_List*
_temp315; struct _tuple0* _temp317; void* _temp319; _LL200: if( _temp198 == (
void*) Cyc_Absyn_VoidType){ goto _LL201;} else{ goto _LL202;} _LL202: if((
unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_Evar: 0){ _LL255:
_temp254=(( struct Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp254 ==  0){
goto _LL203;} else{ goto _LL204;}} else{ goto _LL204;} _LL204: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_Evar: 0){ _LL257: _temp256=((
struct Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp256 ==  0){ goto _LL206;}
else{ _temp258=* _temp256; _LL260: _temp259=( void*) _temp258.v; goto _LL205;}}
else{ goto _LL206;} _LL206: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_VarType: 0){ goto _LL207;} else{ goto _LL208;} _LL208: if((
unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_TunionType: 0){
_LL262: _temp261=(( struct Cyc_Absyn_TunionType_struct*) _temp198)->f1; _LL264:
_temp263=( void*) _temp261.tunion_info; if(*(( int*) _temp263) ==  Cyc_Absyn_KnownTunion){
goto _LL209;} else{ goto _LL210;}} else{ goto _LL210;} _LL210: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_TunionType: 0){ goto _LL211;}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_TunionFieldType: 0){ _LL266: _temp265=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp198)->f1; _LL268: _temp267=( void*) _temp265.field_info; if(*(( int*)
_temp267) ==  Cyc_Absyn_KnownTunionfield){ _LL272: _temp271=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp267)->f1; goto _LL270; _LL270: _temp269=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp267)->f2; goto _LL213;} else{ goto _LL214;}} else{ goto _LL214;} _LL214:
if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL215;} else{ goto _LL216;} _LL216: if(( unsigned int) _temp198 >  3u?*((
int*) _temp198) ==  Cyc_Absyn_PointerType: 0){ _LL274: _temp273=(( struct Cyc_Absyn_PointerType_struct*)
_temp198)->f1; _LL280: _temp279=( void*) _temp273.elt_typ; goto _LL278; _LL278:
_temp277= _temp273.tq; goto _LL276; _LL276: _temp275= _temp273.bounds; goto
_LL217;} else{ goto _LL218;} _LL218: if(( unsigned int) _temp198 >  3u?*(( int*)
_temp198) ==  Cyc_Absyn_IntType: 0){ goto _LL219;} else{ goto _LL220;} _LL220:
if( _temp198 == ( void*) Cyc_Absyn_FloatType){ goto _LL221;} else{ goto _LL222;}
_LL222: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_DoubleType:
0){ goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp198 >  3u?*((
int*) _temp198) ==  Cyc_Absyn_ArrayType: 0){ _LL286: _temp285=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp198)->f1; goto _LL284; _LL284: _temp283=((
struct Cyc_Absyn_ArrayType_struct*) _temp198)->f2; goto _LL282; _LL282: _temp281=((
struct Cyc_Absyn_ArrayType_struct*) _temp198)->f3; goto _LL225;} else{ goto
_LL226;} _LL226: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_FnType:
0){ _LL288: _temp287=(( struct Cyc_Absyn_FnType_struct*) _temp198)->f1; _LL298:
_temp297=( void*) _temp287.ret_typ; goto _LL296; _LL296: _temp295= _temp287.args;
goto _LL294; _LL294: _temp293= _temp287.c_varargs; goto _LL292; _LL292: _temp291=
_temp287.cyc_varargs; goto _LL290; _LL290: _temp289= _temp287.attributes; goto
_LL227;} else{ goto _LL228;} _LL228: if(( unsigned int) _temp198 >  3u?*(( int*)
_temp198) ==  Cyc_Absyn_TupleType: 0){ _LL300: _temp299=(( struct Cyc_Absyn_TupleType_struct*)
_temp198)->f1; goto _LL229;} else{ goto _LL230;} _LL230: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_AnonStructType: 0){ _LL302:
_temp301=(( struct Cyc_Absyn_AnonStructType_struct*) _temp198)->f1; goto _LL231;}
else{ goto _LL232;} _LL232: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_AnonUnionType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp198)->f1; goto _LL233;} else{ goto _LL234;} _LL234: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_UnionType: 0){ _LL306: _temp305=((
struct Cyc_Absyn_UnionType_struct*) _temp198)->f1; goto _LL235;} else{ goto
_LL236;} _LL236: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_StructType:
0){ _LL308: _temp307=(( struct Cyc_Absyn_StructType_struct*) _temp198)->f1; goto
_LL237;} else{ goto _LL238;} _LL238: if(( unsigned int) _temp198 >  3u?*(( int*)
_temp198) ==  Cyc_Absyn_EnumType: 0){ _LL310: _temp309=(( struct Cyc_Absyn_EnumType_struct*)
_temp198)->f1; goto _LL239;} else{ goto _LL240;} _LL240: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_AnonEnumType: 0){ _LL312:
_temp311=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp198)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_TypedefType: 0){ _LL318: _temp317=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f1; goto _LL316; _LL316: _temp315=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f2; goto _LL314; _LL314: _temp313=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f3; goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_RgnHandleType: 0){ _LL320:
_temp319=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp198)->f1; goto
_LL245;} else{ goto _LL246;} _LL246: if( _temp198 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int) _temp198 >  3u?*((
int*) _temp198) ==  Cyc_Absyn_AccessEff: 0){ goto _LL249;} else{ goto _LL250;}
_LL250: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_JoinEff:
0){ goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp198 >  3u?*((
int*) _temp198) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL253;} else{ goto _LL199;}
_LL201: return t; _LL203: return Cyc_Absyn_sint_t; _LL205: return Cyc_Toc_typ_to_c(
_temp259); _LL207: return Cyc_Absyn_void_star_typ(); _LL209: return Cyc_Absyn_void_star_typ();
_LL211:( int) _throw(({ void* _temp321[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp321, sizeof( void*), 0u));}));
_LL213: if( _temp269->typs ==  0){ if( _temp271->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp269->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL215:( int) _throw(({ void*
_temp322[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp322, sizeof( void*), 0u));}));
_LL217: _temp279= Cyc_Toc_typ_to_c_array( _temp279);{ void* _temp323=( void*)(
Cyc_Absyn_compress_conref( _temp275))->v; void* _temp329; _LL325: if((
unsigned int) _temp323 >  1u?*(( int*) _temp323) ==  Cyc_Absyn_Eq_constr: 0){
_LL330: _temp329=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp323)->f1;
if( _temp329 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL326;} else{ goto _LL327;}}
else{ goto _LL327;} _LL327: goto _LL328; _LL326: return Cyc_Toc_tagged_arr_typ;
_LL328: return Cyc_Absyn_star_typ( _temp279,( void*) Cyc_Absyn_HeapRgn, _temp277);
_LL324:;} _LL219: return t; _LL221: return t; _LL223: return t; _LL225: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp331=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp331[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp332; _temp332.tag= Cyc_Absyn_ArrayType; _temp332.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp285); _temp332.f2= _temp283; _temp332.f3=
_temp281; _temp332;}); _temp331;}); _LL227: { struct Cyc_List_List* _temp333= 0;
for( 0; _temp289 !=  0; _temp289= _temp289->tl){ void* _temp334=( void*)
_temp289->hd; _LL336: if( _temp334 == ( void*) Cyc_Absyn_Noreturn_att){ goto
_LL337;} else{ goto _LL338;} _LL338: if( _temp334 == ( void*) Cyc_Absyn_Const_att){
goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int) _temp334 >  16u?*((
int*) _temp334) ==  Cyc_Absyn_Format_att: 0){ goto _LL341;} else{ goto _LL342;}
_LL342: goto _LL343; _LL337: goto _LL339; _LL339: goto _LL341; _LL341: continue;
_LL343: _temp333=({ struct Cyc_List_List* _temp344=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp344->hd=( void*)(( void*)
_temp289->hd); _temp344->tl= _temp333; _temp344;}); goto _LL335; _LL335:;}{
struct Cyc_List_List* _temp345=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp295); if( _temp291 !=  0){ void* _temp346= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*) _temp291->type,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq)); struct
_tuple1* _temp347=({ struct _tuple1* _temp349=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp349->f1= _temp291->name; _temp349->f2= _temp291->tq;
_temp349->f3= _temp346; _temp349;}); _temp345=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp345,({ struct Cyc_List_List* _temp348=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp348->hd=( void*) _temp347; _temp348->tl= 0;
_temp348;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp350=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp350[ 0]=({ struct Cyc_Absyn_FnType_struct _temp351; _temp351.tag= Cyc_Absyn_FnType;
_temp351.f1=({ struct Cyc_Absyn_FnInfo _temp352; _temp352.tvars= 0; _temp352.effect=
0; _temp352.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp297); _temp352.args=
_temp345; _temp352.c_varargs= _temp293; _temp352.cyc_varargs= 0; _temp352.rgn_po=
0; _temp352.attributes= _temp333; _temp352;}); _temp351;}); _temp350;});}}
_LL229: _temp299=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp299);{
struct _tagged_arr* _temp353= Cyc_Toc_add_tuple_type( _temp299); return Cyc_Absyn_strct(
_temp353);} _LL231: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp354=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp354[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp355; _temp355.tag= Cyc_Absyn_AnonStructType; _temp355.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp301); _temp355;}); _temp354;});
_LL233: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp356=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp356[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp357; _temp357.tag=
Cyc_Absyn_AnonUnionType; _temp357.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp303); _temp357;}); _temp356;}); _LL235: if( _temp305 ==  0){({ void*
_temp358[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp358,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp305)); _LL237: if( _temp307 ==  0){({ void* _temp359[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp359, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp307)); _LL239:
return t; _LL241: Cyc_Toc_enumfields_to_c( _temp311); return t; _LL243: if(
_temp313 ==  0){ return({ void* _temp360[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u), _tag_arr( _temp360, sizeof( void*), 0u));});}
return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp361=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp361[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp362; _temp362.tag= Cyc_Absyn_TypedefType;
_temp362.f1= _temp317; _temp362.f2= 0; _temp362.f3=({ struct Cyc_Core_Opt*
_temp363=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp363->v=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp313->v); _temp363;});
_temp362;}); _temp361;}); _LL245: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq); _LL247: return({ void* _temp364[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp364, sizeof( void*), 0u));});
_LL249: return({ void* _temp365[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp365, sizeof( void*), 0u));});
_LL251: return({ void* _temp366[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp366, sizeof( void*), 0u));});
_LL253: return({ void* _temp367[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp367, sizeof( void*), 0u));});
_LL199:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp368= t;
struct Cyc_Absyn_Tqual _temp374; void* _temp376; _LL370: if(( unsigned int)
_temp368 >  3u?*(( int*) _temp368) ==  Cyc_Absyn_ArrayType: 0){ _LL377: _temp376=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp368)->f1; goto _LL375; _LL375:
_temp374=(( struct Cyc_Absyn_ArrayType_struct*) _temp368)->f2; goto _LL371;}
else{ goto _LL372;} _LL372: goto _LL373; _LL371: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp376,( void*) Cyc_Absyn_HeapRgn, _temp374), e, l); _LL373: return Cyc_Absyn_cast_exp(
t, e, l); _LL369:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp378= Cyc_Tcutil_compress(
t); void* _temp418; struct Cyc_Absyn_Structdecl** _temp420; struct Cyc_List_List*
_temp422; struct Cyc_List_List* _temp424; struct Cyc_Absyn_TunionFieldInfo
_temp426; void* _temp428; struct Cyc_Absyn_Tunionfield* _temp430; struct Cyc_Absyn_Tuniondecl*
_temp432; struct Cyc_List_List* _temp434; _LL380: if( _temp378 == ( void*) Cyc_Absyn_VoidType){
goto _LL381;} else{ goto _LL382;} _LL382: if(( unsigned int) _temp378 >  3u?*((
int*) _temp378) ==  Cyc_Absyn_VarType: 0){ goto _LL383;} else{ goto _LL384;}
_LL384: if(( unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_IntType:
0){ goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp378 >  3u?*((
int*) _temp378) ==  Cyc_Absyn_EnumType: 0){ goto _LL387;} else{ goto _LL388;}
_LL388: if(( unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL389;} else{ goto _LL390;} _LL390: if( _temp378 == ( void*) Cyc_Absyn_FloatType){
goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int) _temp378 >  3u?*((
int*) _temp378) ==  Cyc_Absyn_DoubleType: 0){ goto _LL393;} else{ goto _LL394;}
_LL394: if(( unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_FnType:
0){ goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp378 >  3u?*((
int*) _temp378) ==  Cyc_Absyn_ArrayType: 0){ _LL419: _temp418=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp378)->f1; goto _LL397;} else{ goto _LL398;}
_LL398: if(( unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_StructType:
0){ _LL421: _temp420=(( struct Cyc_Absyn_StructType_struct*) _temp378)->f3; goto
_LL399;} else{ goto _LL400;} _LL400: if(( unsigned int) _temp378 >  3u?*(( int*)
_temp378) ==  Cyc_Absyn_AnonStructType: 0){ _LL423: _temp422=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp378)->f1; goto _LL401;} else{ goto _LL402;} _LL402: if(( unsigned int)
_temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_AnonUnionType: 0){ _LL425:
_temp424=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp378)->f1; goto _LL403;}
else{ goto _LL404;} _LL404: if(( unsigned int) _temp378 >  3u?*(( int*) _temp378)
==  Cyc_Absyn_TunionFieldType: 0){ _LL427: _temp426=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp378)->f1; _LL429: _temp428=( void*) _temp426.field_info; if(*(( int*)
_temp428) ==  Cyc_Absyn_KnownTunionfield){ _LL433: _temp432=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp428)->f1; goto _LL431; _LL431: _temp430=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp428)->f2; goto _LL405;} else{ goto _LL406;}} else{ goto _LL406;} _LL406:
if(( unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_TupleType: 0){
_LL435: _temp434=(( struct Cyc_Absyn_TupleType_struct*) _temp378)->f1; goto
_LL407;} else{ goto _LL408;} _LL408: if(( unsigned int) _temp378 >  3u?*(( int*)
_temp378) ==  Cyc_Absyn_TunionType: 0){ goto _LL409;} else{ goto _LL410;} _LL410:
if(( unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_UnionType: 0){
goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int) _temp378 >  3u?*((
int*) _temp378) ==  Cyc_Absyn_PointerType: 0){ goto _LL413;} else{ goto _LL414;}
_LL414: if(( unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL415;} else{ goto _LL416;} _LL416: goto _LL417; _LL381: return 1;
_LL383: return 0; _LL385: return 1; _LL387: return 1; _LL389: return 1; _LL391:
return 1; _LL393: return 1; _LL395: return 1; _LL397: return Cyc_Toc_atomic_typ(
_temp418); _LL399: if( _temp420 ==  0? 1:(* _temp420)->fields ==  0){ return 0;}{
struct Cyc_List_List* _temp436=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((* _temp420)->fields))->v; for( 0; _temp436 !=  0; _temp436=
_temp436->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)
_temp436->hd)->type)){ return 0;}}} return 1; _LL401: _temp424= _temp422; goto
_LL403; _LL403: for( 0; _temp424 !=  0; _temp424= _temp424->tl){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*) _temp424->hd)->type)){ return 0;}}
return 1; _LL405: _temp434= _temp430->typs; goto _LL407; _LL407: for( 0;
_temp434 !=  0; _temp434= _temp434->tl){ if( ! Cyc_Toc_atomic_typ((*(( struct
_tuple3*) _temp434->hd)).f2)){ return 0;}} return 1; _LL409: return 0; _LL411:
return 0; _LL413: return 0; _LL415: return 0; _LL417:({ struct Cyc_Std_String_pa_struct
_temp438; _temp438.tag= Cyc_Std_String_pa; _temp438.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp437[ 1u]={& _temp438};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp437, sizeof( void*), 1u));}});
return 0; _LL379:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp439=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp445; void* _temp447;
_LL441: if(( unsigned int) _temp439 >  3u?*(( int*) _temp439) ==  Cyc_Absyn_PointerType:
0){ _LL446: _temp445=(( struct Cyc_Absyn_PointerType_struct*) _temp439)->f1;
_LL448: _temp447=( void*) _temp445.elt_typ; goto _LL442;} else{ goto _LL443;}
_LL443: goto _LL444; _LL442: { void* _temp449= Cyc_Tcutil_compress( _temp447);
_LL451: if( _temp449 == ( void*) Cyc_Absyn_VoidType){ goto _LL452;} else{ goto
_LL453;} _LL453: goto _LL454; _LL452: return 1; _LL454: return 0; _LL450:;}
_LL444: return 0; _LL440:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp455= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp467; struct Cyc_Absyn_Structdecl* _temp469; struct Cyc_List_List* _temp470;
_LL457: if(( unsigned int) _temp455 >  3u?*(( int*) _temp455) ==  Cyc_Absyn_StructType:
0){ _LL468: _temp467=(( struct Cyc_Absyn_StructType_struct*) _temp455)->f3; if(
_temp467 ==  0){ goto _LL459;} else{ _temp469=* _temp467; goto _LL458;}} else{
goto _LL459;} _LL459: if(( unsigned int) _temp455 >  3u?*(( int*) _temp455) == 
Cyc_Absyn_AnonStructType: 0){ _LL471: _temp470=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp455)->f1; goto _LL460;} else{ goto _LL461;} _LL461: if(( unsigned int)
_temp455 >  3u?*(( int*) _temp455) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL462;}
else{ goto _LL463;} _LL463: if(( unsigned int) _temp455 >  3u?*(( int*) _temp455)
==  Cyc_Absyn_UnionType: 0){ goto _LL464;} else{ goto _LL465;} _LL465: goto
_LL466; _LL458: if( _temp469->fields ==  0){( int) _throw(({ void* _temp472[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp472, sizeof( void*), 0u));}));} _temp470=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp469->fields))->v;
goto _LL460; _LL460: { struct Cyc_Absyn_Structfield* _temp473= Cyc_Absyn_lookup_field(
_temp470, f); if( _temp473 ==  0){({ struct Cyc_Std_String_pa_struct _temp475;
_temp475.tag= Cyc_Std_String_pa; _temp475.f1=( struct _tagged_arr)* f;{ void*
_temp474[ 1u]={& _temp475};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_field: bad field %s", sizeof(
unsigned char), 28u), _tag_arr( _temp474, sizeof( void*), 1u));}});} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp473))->type);} _LL462:
return 0; _LL464: return 0; _LL466:( int) _throw(({ struct Cyc_Std_String_pa_struct
_temp477; _temp477.tag= Cyc_Std_String_pa; _temp477.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp476[ 1u]={& _temp477}; Cyc_Toc_toc_impos( _tag_arr("is_poly_field: bad type %s",
sizeof( unsigned char), 27u), _tag_arr( _temp476, sizeof( void*), 1u));}}));
_LL456:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void*
_temp478=( void*) e->r; struct _tagged_arr* _temp486; struct Cyc_Absyn_Exp*
_temp488; struct _tagged_arr* _temp490; struct Cyc_Absyn_Exp* _temp492; _LL480:
if(*(( int*) _temp478) ==  Cyc_Absyn_StructMember_e){ _LL489: _temp488=(( struct
Cyc_Absyn_StructMember_e_struct*) _temp478)->f1; goto _LL487; _LL487: _temp486=((
struct Cyc_Absyn_StructMember_e_struct*) _temp478)->f2; goto _LL481;} else{ goto
_LL482;} _LL482: if(*(( int*) _temp478) ==  Cyc_Absyn_StructArrow_e){ _LL493:
_temp492=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp478)->f1; goto _LL491;
_LL491: _temp490=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp478)->f2; goto
_LL483;} else{ goto _LL484;} _LL484: goto _LL485; _LL481: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp488->topt))->v, _temp486);
_LL483: { void* _temp494= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp492->topt))->v); struct Cyc_Absyn_PtrInfo _temp500; void*
_temp502; _LL496: if(( unsigned int) _temp494 >  3u?*(( int*) _temp494) ==  Cyc_Absyn_PointerType:
0){ _LL501: _temp500=(( struct Cyc_Absyn_PointerType_struct*) _temp494)->f1;
_LL503: _temp502=( void*) _temp500.elt_typ; goto _LL497;} else{ goto _LL498;}
_LL498: goto _LL499; _LL497: return Cyc_Toc_is_poly_field( _temp502, _temp490);
_LL499:({ struct Cyc_Std_String_pa_struct _temp505; _temp505.tag= Cyc_Std_String_pa;
_temp505.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp492->topt))->v);{ void* _temp504[ 1u]={& _temp505};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s",
sizeof( unsigned char), 30u), _tag_arr( _temp504, sizeof( void*), 1u));}});
return 0; _LL495:;} _LL485: return 0; _LL479:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_e,({
struct Cyc_List_List* _temp506=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp506->hd=( void*) s; _temp506->tl= 0; _temp506;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_atomic_e,({ struct Cyc_List_List*
_temp507=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp507->hd=( void*) s; _temp507->tl= 0; _temp507;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp508[ 2u]; _temp508[ 1u]= s; _temp508[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp508, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp509=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp509->hd=( void*) e; _temp509->tl= 0; _temp509;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp510=( void*) e->r; void* _temp516; _LL512: if(*((
int*) _temp510) ==  Cyc_Absyn_Const_e){ _LL517: _temp516=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp510)->f1; if(( unsigned int) _temp516 >  1u?*(( int*) _temp516) ==  Cyc_Absyn_String_c:
0){ goto _LL513;} else{ goto _LL514;}} else{ goto _LL514;} _LL514: goto _LL515;
_LL513: is_string= 1; goto _LL511; _LL515: goto _LL511; _LL511:;}{ struct Cyc_Absyn_Exp*
xexp; struct Cyc_Absyn_Exp* xplussz; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp521=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp521[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp522; _temp522.tag= Cyc_Absyn_ArrayType; _temp522.f1=(
void*) Cyc_Absyn_uchar_t; _temp522.f2= Cyc_Toc_mt_tq; _temp522.f3=( struct Cyc_Absyn_Exp*)
sz; _temp522;}); _temp521;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp518=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp518->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp519=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp519[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp520; _temp520.tag= Cyc_Absyn_Var_d;
_temp520.f1= vd; _temp520;}); _temp519;}), 0); _temp518->tl= Cyc_Toc_result_decls;
_temp518;}); xexp= Cyc_Absyn_var_exp( x, 0); xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0);} else{ xexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), e, 0);
xplussz= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_add_exp( e, sz,
0), 0);}{ struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp523[ 3u]; _temp523[ 2u]=({ struct _tuple4* _temp526=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp526->f1= 0; _temp526->f2=
xplussz; _temp526;}); _temp523[ 1u]=({ struct _tuple4* _temp525=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp525->f1= 0; _temp525->f2= xexp;
_temp525;}); _temp523[ 0u]=({ struct _tuple4* _temp524=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp524->f1= 0; _temp524->f2= xexp;
_temp524;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp523, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp527=( struct Cyc_Toc_Env*) _cycalloc( sizeof( struct
Cyc_Toc_Env)); _temp527->break_lab= 0; _temp527->continue_lab= 0; _temp527->fallthru_info=
0; _temp527->varmap=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp527->toplevel= 1;
_temp527;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp528=( struct Cyc_Toc_Env*) _cycalloc( sizeof(
struct Cyc_Toc_Env)); _temp528->break_lab= e->break_lab; _temp528->continue_lab=
e->continue_lab; _temp528->fallthru_info= e->fallthru_info; _temp528->varmap= e->varmap;
_temp528->toplevel= e->toplevel; _temp528;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp529=(* x).f1; _LL531:
if(( unsigned int) _temp529 >  1u?*(( int*) _temp529) ==  Cyc_Absyn_Rel_n: 0){
goto _LL532;} else{ goto _LL533;} _LL533: goto _LL534; _LL532:( int) _throw(({
struct Cyc_Std_String_pa_struct _temp536; _temp536.tag= Cyc_Std_String_pa;
_temp536.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp535[
1u]={& _temp536}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp535, sizeof( void*), 1u));}}));
_LL534: goto _LL530; _LL530:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
k, struct Cyc_Absyn_Exp* v)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict*
f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp537= 0; for( 0; fallthru_binders !=  0; fallthru_binders= fallthru_binders->tl){
_temp537=({ struct Cyc_List_List* _temp538=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp538->hd=( void*)(( struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name; _temp538->tl= _temp537; _temp538;});} _temp537=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp537);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp539=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp539->v=( void*) break_l; _temp539;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp540=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp540->v=( void*)({ struct _tuple6* _temp541=( struct _tuple6*) _cycalloc(
sizeof( struct _tuple6)); _temp541->f1= fallthru_l; _temp541->f2= _temp537;
_temp541->f3= next_case_env->varmap; _temp541;}); _temp540;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp542=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp542->v=( void*) break_l; _temp542;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp543=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp543->v=( void*)({
struct _tuple6* _temp544=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp544->f1= next_l; _temp544->f2= 0; _temp544->f3=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp544;}); _temp543;}); return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s); static int Cyc_Toc_need_null_check( struct Cyc_Absyn_Exp*
e){ void* _temp545=( void*) e->annot; _LL547: if( _temp545 ==  Cyc_CfFlowInfo_UnknownZ){
goto _LL548;} else{ goto _LL549;} _LL549: if( _temp545 ==  Cyc_CfFlowInfo_NotZero){
goto _LL550;} else{ goto _LL551;} _LL551: if( _temp545 ==  Cyc_CfFlowInfo_IsZero){
goto _LL552;} else{ goto _LL553;} _LL553: if( _temp545 ==  Cyc_Absyn_EmptyAnnot){
goto _LL554;} else{ goto _LL555;} _LL555: goto _LL556; _LL548: return Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL550: return 0;
_LL552:({ void* _temp557[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("dereference of NULL pointer",
sizeof( unsigned char), 28u), _tag_arr( _temp557, sizeof( void*), 0u));});
return 0; _LL554:({ void* _temp558[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("!", sizeof( unsigned char), 2u), _tag_arr( _temp558, sizeof( void*), 0u));});({
void* _temp559[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("unreachable dereference",
sizeof( unsigned char), 24u), _tag_arr( _temp559, sizeof( void*), 0u));});
return 0; _LL556:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp560=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp560[ 0]=({ struct Cyc_Core_Impossible_struct _temp561; _temp561.tag= Cyc_Core_Impossible;
_temp561.f1= _tag_arr("need_null_check", sizeof( unsigned char), 16u); _temp561;});
_temp560;})); _LL546:;} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){
if( e->topt ==  0){({ void* _temp562[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp562, sizeof( void*), 0u));});}
return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp563[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp563, sizeof( void*), 0u));});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp564=( struct _tuple3*) _cycalloc( sizeof(
struct _tuple3)); _temp564->f1= Cyc_Toc_mt_tq; _temp564->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp564;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp565=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp565->f1= 0; _temp565->f2= e; _temp565;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp566= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp566): Cyc_Toc_malloc_ptr( _temp566), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp566), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp567=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp567 !=  0; _temp567=
_temp567->tl){ struct _tuple4 _temp570; struct Cyc_Absyn_Exp* _temp571; struct
Cyc_List_List* _temp573; struct _tuple4* _temp568=( struct _tuple4*) _temp567->hd;
_temp570=* _temp568; _LL574: _temp573= _temp570.f1; goto _LL572; _LL572:
_temp571= _temp570.f2; goto _LL569; _LL569: { struct Cyc_Absyn_Exp* e_index; if(
_temp573 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(
_temp573->tl !=  0){({ void* _temp575[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp575, sizeof( void*), 0u));});}{
void* _temp576=( void*) _temp573->hd; void* _temp577= _temp576; struct Cyc_Absyn_Exp*
_temp583; _LL579: if(*(( int*) _temp577) ==  Cyc_Absyn_ArrayElement){ _LL584:
_temp583=(( struct Cyc_Absyn_ArrayElement_struct*) _temp577)->f1; goto _LL580;}
else{ goto _LL581;} _LL581: if(*(( int*) _temp577) ==  Cyc_Absyn_FieldName){
goto _LL582;} else{ goto _LL578;} _LL580: Cyc_Toc_exp_to_c( nv, _temp583);
e_index= _temp583; goto _LL578; _LL582: e_index=({ void* _temp585[ 0u]={};((
struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("field name designators in array", sizeof( unsigned char), 32u),
_tag_arr( _temp585, sizeof( void*), 0u));}); goto _LL578; _LL578:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp586=( void*)
_temp571->r; struct Cyc_List_List* _temp596; struct Cyc_Absyn_Exp* _temp598;
struct Cyc_Absyn_Exp* _temp600; struct Cyc_Absyn_Vardecl* _temp602; struct Cyc_List_List*
_temp604; void* _temp606; _LL588: if(*(( int*) _temp586) ==  Cyc_Absyn_Array_e){
_LL597: _temp596=(( struct Cyc_Absyn_Array_e_struct*) _temp586)->f1; goto _LL589;}
else{ goto _LL590;} _LL590: if(*(( int*) _temp586) ==  Cyc_Absyn_Comprehension_e){
_LL603: _temp602=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp586)->f1;
goto _LL601; _LL601: _temp600=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp586)->f2; goto _LL599; _LL599: _temp598=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp586)->f3; goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*) _temp586)
==  Cyc_Absyn_AnonStruct_e){ _LL607: _temp606=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp586)->f1; goto _LL605; _LL605: _temp604=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp586)->f2; goto _LL593;} else{ goto _LL594;} _LL594: goto _LL595; _LL589: s=
Cyc_Toc_init_array( nv, lval, _temp596, s); goto _LL587; _LL591: s= Cyc_Toc_init_comprehension(
nv, lval, _temp602, _temp600, _temp598, s, 0); goto _LL587; _LL593: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp606, _temp604, s); goto _LL587; _LL595: Cyc_Toc_exp_to_c( nv,
_temp571); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp571, 0), s, 0); goto _LL587; _LL587:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp608= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp608, Cyc_Absyn_varb_exp( _temp608,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp631=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp631[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp632; _temp632.tag= Cyc_Absyn_Local_b; _temp632.f1= vd; _temp632;});
_temp631;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp608, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp608, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp608, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp608, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp609=( void*) e2->r; struct Cyc_List_List* _temp619; struct Cyc_Absyn_Exp*
_temp621; struct Cyc_Absyn_Exp* _temp623; struct Cyc_Absyn_Vardecl* _temp625;
struct Cyc_List_List* _temp627; void* _temp629; _LL611: if(*(( int*) _temp609)
==  Cyc_Absyn_Array_e){ _LL620: _temp619=(( struct Cyc_Absyn_Array_e_struct*)
_temp609)->f1; goto _LL612;} else{ goto _LL613;} _LL613: if(*(( int*) _temp609)
==  Cyc_Absyn_Comprehension_e){ _LL626: _temp625=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp609)->f1; goto _LL624; _LL624: _temp623=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp609)->f2; goto _LL622; _LL622: _temp621=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp609)->f3; goto _LL614;} else{ goto _LL615;} _LL615: if(*(( int*) _temp609)
==  Cyc_Absyn_AnonStruct_e){ _LL630: _temp629=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp609)->f1; goto _LL628; _LL628: _temp627=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp609)->f2; goto _LL616;} else{ goto _LL617;} _LL617: goto _LL618; _LL612:
body= Cyc_Toc_init_array( nv2, lval, _temp619, Cyc_Absyn_skip_stmt( 0)); goto
_LL610; _LL614: body= Cyc_Toc_init_comprehension( nv2, lval, _temp625, _temp623,
_temp621, Cyc_Absyn_skip_stmt( 0), 0); goto _LL610; _LL616: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp629, _temp627, Cyc_Absyn_skip_stmt( 0)); goto _LL610; _LL618: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL610; _LL610:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp608, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp633=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp633 !=  0; _temp633= _temp633->tl){
struct _tuple4 _temp636; struct Cyc_Absyn_Exp* _temp637; struct Cyc_List_List*
_temp639; struct _tuple4* _temp634=( struct _tuple4*) _temp633->hd; _temp636=*
_temp634; _LL640: _temp639= _temp636.f1; goto _LL638; _LL638: _temp637= _temp636.f2;
goto _LL635; _LL635: if( _temp639 ==  0){({ void* _temp641[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp641, sizeof( void*), 0u));});} if(
_temp639->tl !=  0){({ void* _temp642[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u), _tag_arr( _temp642, sizeof( void*), 0u));});}{
void* _temp643=( void*) _temp639->hd; struct _tagged_arr* _temp649; _LL645: if(*((
int*) _temp643) ==  Cyc_Absyn_FieldName){ _LL650: _temp649=(( struct Cyc_Absyn_FieldName_struct*)
_temp643)->f1; goto _LL646;} else{ goto _LL647;} _LL647: goto _LL648; _LL646: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp649, 0);{ void*
_temp651=( void*) _temp637->r; struct Cyc_List_List* _temp661; struct Cyc_Absyn_Exp*
_temp663; struct Cyc_Absyn_Exp* _temp665; struct Cyc_Absyn_Vardecl* _temp667;
struct Cyc_List_List* _temp669; void* _temp671; _LL653: if(*(( int*) _temp651)
==  Cyc_Absyn_Array_e){ _LL662: _temp661=(( struct Cyc_Absyn_Array_e_struct*)
_temp651)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*) _temp651)
==  Cyc_Absyn_Comprehension_e){ _LL668: _temp667=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp651)->f1; goto _LL666; _LL666: _temp665=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp651)->f2; goto _LL664; _LL664: _temp663=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp651)->f3; goto _LL656;} else{ goto _LL657;} _LL657: if(*(( int*) _temp651)
==  Cyc_Absyn_AnonStruct_e){ _LL672: _temp671=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp651)->f1; goto _LL670; _LL670: _temp669=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp651)->f2; goto _LL658;} else{ goto _LL659;} _LL659: goto _LL660; _LL654: s=
Cyc_Toc_init_array( nv, lval, _temp661, s); goto _LL652; _LL656: s= Cyc_Toc_init_comprehension(
nv, lval, _temp667, _temp665, _temp663, s, 0); goto _LL652; _LL658: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp671, _temp669, s); goto _LL652; _LL660: Cyc_Toc_exp_to_c( nv,
_temp637); if( Cyc_Toc_is_poly_field( struct_type, _temp649)){ _temp637= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp637, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp637, 0), 0), s, 0); goto _LL652; _LL652:;} goto
_LL644;} _LL648:( int) _throw(({ void* _temp673[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp673, sizeof( void*), 0u));})); _LL644:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp674= _new_region("r");
struct _RegionHandle* r=& _temp674; _push_region( r);{ struct Cyc_List_List*
_temp675=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp676= Cyc_Toc_add_tuple_type( _temp675); struct
_tuple0* _temp677= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp678= Cyc_Absyn_var_exp(
_temp677, 0); struct Cyc_Absyn_Stmt* _temp679= Cyc_Absyn_exp_stmt( _temp678, 0);
struct Cyc_Absyn_Exp*(* _temp680)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp681=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp681); for( 0; _temp681
!=  0;( _temp681= _temp681->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
_temp681->hd; struct Cyc_Absyn_Exp* lval= _temp680( _temp678, Cyc_Absyn_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp682=( void*) e->r; struct Cyc_List_List*
_temp690; struct Cyc_Absyn_Exp* _temp692; struct Cyc_Absyn_Exp* _temp694; struct
Cyc_Absyn_Vardecl* _temp696; _LL684: if(*(( int*) _temp682) ==  Cyc_Absyn_Array_e){
_LL691: _temp690=(( struct Cyc_Absyn_Array_e_struct*) _temp682)->f1; goto _LL685;}
else{ goto _LL686;} _LL686: if(*(( int*) _temp682) ==  Cyc_Absyn_Comprehension_e){
_LL697: _temp696=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp682)->f1;
goto _LL695; _LL695: _temp694=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp682)->f2; goto _LL693; _LL693: _temp692=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp682)->f3; goto _LL687;} else{ goto _LL688;} _LL688: goto _LL689; _LL685:
_temp679= Cyc_Toc_init_array( nv, lval, _temp690, _temp679); goto _LL683; _LL687:
_temp679= Cyc_Toc_init_comprehension( nv, lval, _temp696, _temp694, _temp692,
_temp679, 0); goto _LL683; _LL689: Cyc_Toc_exp_to_c( nv, e); _temp679= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp680( _temp678, Cyc_Absyn_fieldname(
i), 0), e, 0), 0), _temp679, 0); goto _LL683; _LL683:;}}}{ struct Cyc_Absyn_Exp*
_temp698= Cyc_Toc_make_struct( nv, _temp677, Cyc_Absyn_strct( _temp676),
_temp679, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp698;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp699= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp700= Cyc_Absyn_var_exp( _temp699, 0); struct Cyc_Absyn_Stmt*
_temp701= Cyc_Absyn_exp_stmt( _temp700, 0); struct Cyc_Absyn_Exp*(* _temp702)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp703= _new_region("r"); struct _RegionHandle* r=&
_temp703; _push_region( r);{ struct Cyc_List_List* _temp704=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp704 !=  0; _temp704= _temp704->tl){ struct _tuple4 _temp707; struct Cyc_Absyn_Exp*
_temp708; struct Cyc_List_List* _temp710; struct _tuple4* _temp705=( struct
_tuple4*) _temp704->hd; _temp707=* _temp705; _LL711: _temp710= _temp707.f1; goto
_LL709; _LL709: _temp708= _temp707.f2; goto _LL706; _LL706: is_atomic= is_atomic?
Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp708->topt))->v):
0; if( _temp710 ==  0){({ void* _temp712[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp712, sizeof( void*), 0u));});} if(
_temp710->tl !=  0){ struct _tuple0* _temp713= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp714= Cyc_Absyn_var_exp( _temp713, 0); for( 0; _temp710 !=  0; _temp710=
_temp710->tl){ void* _temp715=( void*) _temp710->hd; struct _tagged_arr*
_temp721; _LL717: if(*(( int*) _temp715) ==  Cyc_Absyn_FieldName){ _LL722:
_temp721=(( struct Cyc_Absyn_FieldName_struct*) _temp715)->f1; goto _LL718;}
else{ goto _LL719;} _LL719: goto _LL720; _LL718: if( Cyc_Toc_is_poly_field(
struct_type, _temp721)){ _temp714= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp714, 0);} _temp701= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp702( _temp700, _temp721, 0), _temp714, 0), 0), _temp701, 0); goto _LL716;
_LL720:( int) _throw(({ void* _temp723[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u), _tag_arr( _temp723, sizeof( void*), 0u));}));
_LL716:;} Cyc_Toc_exp_to_c( nv, _temp708); _temp701= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp714, _temp708, 0), 0), _temp701, 0);} else{ void*
_temp724=( void*) _temp710->hd; struct _tagged_arr* _temp730; _LL726: if(*(( int*)
_temp724) ==  Cyc_Absyn_FieldName){ _LL731: _temp730=(( struct Cyc_Absyn_FieldName_struct*)
_temp724)->f1; goto _LL727;} else{ goto _LL728;} _LL728: goto _LL729; _LL727: {
struct Cyc_Absyn_Exp* lval= _temp702( _temp700, _temp730, 0);{ void* _temp732=(
void*) _temp708->r; struct Cyc_List_List* _temp742; struct Cyc_Absyn_Exp*
_temp744; struct Cyc_Absyn_Exp* _temp746; struct Cyc_Absyn_Vardecl* _temp748;
struct Cyc_List_List* _temp750; void* _temp752; _LL734: if(*(( int*) _temp732)
==  Cyc_Absyn_Array_e){ _LL743: _temp742=(( struct Cyc_Absyn_Array_e_struct*)
_temp732)->f1; goto _LL735;} else{ goto _LL736;} _LL736: if(*(( int*) _temp732)
==  Cyc_Absyn_Comprehension_e){ _LL749: _temp748=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp732)->f1; goto _LL747; _LL747: _temp746=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp732)->f2; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp732)->f3; goto _LL737;} else{ goto _LL738;} _LL738: if(*(( int*) _temp732)
==  Cyc_Absyn_AnonStruct_e){ _LL753: _temp752=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp732)->f1; goto _LL751; _LL751: _temp750=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp732)->f2; goto _LL739;} else{ goto _LL740;} _LL740: goto _LL741; _LL735:
_temp701= Cyc_Toc_init_array( nv, lval, _temp742, _temp701); goto _LL733; _LL737:
_temp701= Cyc_Toc_init_comprehension( nv, lval, _temp748, _temp746, _temp744,
_temp701, 0); goto _LL733; _LL739: _temp701= Cyc_Toc_init_anon_struct( nv, lval,
_temp752, _temp750, _temp701); goto _LL733; _LL741: Cyc_Toc_exp_to_c( nv,
_temp708); if( Cyc_Toc_is_poly_field( struct_type, _temp730)){ _temp708= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp708, 0);} _temp701= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp708, 0), 0), _temp701, 0); goto _LL733; _LL733:;}
goto _LL725;} _LL729:( int) _throw(({ void* _temp754[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp754, sizeof( void*), 0u));})); _LL725:;}}}{ struct Cyc_Absyn_Exp* _temp755=
Cyc_Toc_make_struct( nv, _temp699, Cyc_Absyn_strctq( tdn), _temp701, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp755;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp756=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp756[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp757; _temp757.tag= Cyc_Absyn_Increment_e;
_temp757.f1= e1; _temp757.f2=( void*) incr; _temp757;}); _temp756;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp758=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp768; struct Cyc_Absyn_Exp*
_temp770; void* _temp772; struct _tagged_arr* _temp774; struct Cyc_Absyn_Exp*
_temp776; _LL760: if(*(( int*) _temp758) ==  Cyc_Absyn_StmtExp_e){ _LL769:
_temp768=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp758)->f1; goto _LL761;}
else{ goto _LL762;} _LL762: if(*(( int*) _temp758) ==  Cyc_Absyn_Cast_e){ _LL773:
_temp772=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp758)->f1; goto _LL771;
_LL771: _temp770=(( struct Cyc_Absyn_Cast_e_struct*) _temp758)->f2; goto _LL763;}
else{ goto _LL764;} _LL764: if(*(( int*) _temp758) ==  Cyc_Absyn_StructMember_e){
_LL777: _temp776=(( struct Cyc_Absyn_StructMember_e_struct*) _temp758)->f1; goto
_LL775; _LL775: _temp774=(( struct Cyc_Absyn_StructMember_e_struct*) _temp758)->f2;
goto _LL765;} else{ goto _LL766;} _LL766: goto _LL767; _LL761: Cyc_Toc_lvalue_assign_stmt(
_temp768, fs, f, f_env); goto _LL759; _LL763: Cyc_Toc_lvalue_assign( _temp770,
fs, f, f_env); goto _LL759; _LL765:( void*)( e1->r=( void*)(( void*) _temp776->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp778=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp778->hd=( void*) _temp774; _temp778->tl= fs; _temp778;}), f, f_env); goto
_LL759; _LL767: if( ! Cyc_Absyn_is_lvalue( e1)){({ struct Cyc_Std_String_pa_struct
_temp780; _temp780.tag= Cyc_Std_String_pa; _temp780.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp779[ 1u]={& _temp780};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u), _tag_arr( _temp779, sizeof( void*), 1u));}});}{
struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs !=  0; fs= fs->tl){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_arr*) fs->hd,
e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto
_LL759;} _LL759:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp781=( void*) s->r; struct Cyc_Absyn_Exp*
_temp791; struct Cyc_Absyn_Stmt* _temp793; struct Cyc_Absyn_Decl* _temp795;
struct Cyc_Absyn_Stmt* _temp797; _LL783: if(( unsigned int) _temp781 >  1u?*((
int*) _temp781) ==  Cyc_Absyn_Exp_s: 0){ _LL792: _temp791=(( struct Cyc_Absyn_Exp_s_struct*)
_temp781)->f1; goto _LL784;} else{ goto _LL785;} _LL785: if(( unsigned int)
_temp781 >  1u?*(( int*) _temp781) ==  Cyc_Absyn_Decl_s: 0){ _LL796: _temp795=((
struct Cyc_Absyn_Decl_s_struct*) _temp781)->f1; goto _LL794; _LL794: _temp793=((
struct Cyc_Absyn_Decl_s_struct*) _temp781)->f2; goto _LL786;} else{ goto _LL787;}
_LL787: if(( unsigned int) _temp781 >  1u?*(( int*) _temp781) ==  Cyc_Absyn_Seq_s:
0){ _LL798: _temp797=(( struct Cyc_Absyn_Seq_s_struct*) _temp781)->f2; goto
_LL788;} else{ goto _LL789;} _LL789: goto _LL790; _LL784: Cyc_Toc_lvalue_assign(
_temp791, fs, f, f_env); goto _LL782; _LL786: Cyc_Toc_lvalue_assign_stmt(
_temp793, fs, f, f_env); goto _LL782; _LL788: Cyc_Toc_lvalue_assign_stmt(
_temp797, fs, f, f_env); goto _LL782; _LL790:({ struct Cyc_Std_String_pa_struct
_temp800; _temp800.tag= Cyc_Std_String_pa; _temp800.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp799[ 1u]={& _temp800};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s", sizeof(
unsigned char), 23u), _tag_arr( _temp799, sizeof( void*), 1u));}}); goto _LL782;
_LL782:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2,
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x ==  0){ return 0;} result=({ struct
Cyc_List_List* _temp801=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp801->hd=( void*) f(( void*) x->hd, env); _temp801->tl=
0; _temp801;}); prev= result; for( x= x->tl; x !=  0; x= x->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp802=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp802->hd=( void*) f((
void*) x->hd, env); _temp802->tl= 0; _temp802;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp803=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp803->f1= 0; _temp803->f2= e; _temp803;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp804=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp810; _LL806: if((
unsigned int) _temp804 >  3u?*(( int*) _temp804) ==  Cyc_Absyn_PointerType: 0){
_LL811: _temp810=(( struct Cyc_Absyn_PointerType_struct*) _temp804)->f1; goto
_LL807;} else{ goto _LL808;} _LL808: goto _LL809; _LL807: return _temp810;
_LL809:( int) _throw(({ void* _temp812[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("get_ptr_typ: not a pointer!",
sizeof( unsigned char), 28u), _tag_arr( _temp812, sizeof( void*), 0u));}));
_LL805:;} struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp813=( void*) e->r; if( e->topt ==  0){({
struct Cyc_Std_String_pa_struct _temp815; _temp815.tag= Cyc_Std_String_pa;
_temp815.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp814[ 1u]={&
_temp815};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u), _tag_arr(
_temp814, sizeof( void*), 1u));}}); return;}{ void* old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp816= _temp813; void*
_temp902; void* _temp904; struct _tuple0* _temp906; struct _tuple0* _temp908;
struct Cyc_List_List* _temp910; void* _temp912; void* _temp914; struct Cyc_Absyn_Exp*
_temp916; struct Cyc_Absyn_Exp* _temp918; struct Cyc_Core_Opt* _temp920; struct
Cyc_Absyn_Exp* _temp922; struct Cyc_Absyn_Exp* _temp924; struct Cyc_Absyn_Exp*
_temp926; struct Cyc_Absyn_Exp* _temp928; struct Cyc_Absyn_Exp* _temp930; struct
Cyc_Absyn_Exp* _temp932; struct Cyc_List_List* _temp934; struct Cyc_Absyn_Exp*
_temp936; struct Cyc_Absyn_VarargCallInfo* _temp938; struct Cyc_List_List*
_temp940; struct Cyc_Absyn_Exp* _temp942; struct Cyc_Absyn_VarargCallInfo*
_temp944; struct Cyc_Absyn_VarargCallInfo _temp946; struct Cyc_Absyn_VarargInfo*
_temp947; struct Cyc_List_List* _temp949; int _temp951; struct Cyc_List_List*
_temp953; struct Cyc_Absyn_Exp* _temp955; struct Cyc_Absyn_Exp* _temp957; struct
Cyc_Absyn_Exp* _temp959; struct Cyc_List_List* _temp961; struct Cyc_Absyn_Exp*
_temp963; struct Cyc_Absyn_Exp* _temp965; void* _temp967; void** _temp969;
struct Cyc_Absyn_Exp* _temp970; struct Cyc_Absyn_Exp* _temp972; struct Cyc_Absyn_Exp*
_temp974; struct Cyc_Absyn_Exp* _temp976; void* _temp978; void* _temp980; struct
_tagged_arr* _temp982; void* _temp984; void* _temp986; unsigned int _temp988;
void* _temp990; struct Cyc_Absyn_Exp* _temp992; struct _tagged_arr* _temp994;
struct Cyc_Absyn_Exp* _temp996; struct _tagged_arr* _temp998; struct Cyc_Absyn_Exp*
_temp1000; struct Cyc_Absyn_Exp* _temp1002; struct Cyc_Absyn_Exp* _temp1004;
struct Cyc_List_List* _temp1006; struct Cyc_List_List* _temp1008; struct Cyc_Absyn_Exp*
_temp1010; struct Cyc_Absyn_Exp* _temp1012; struct Cyc_Absyn_Vardecl* _temp1014;
struct Cyc_Absyn_Structdecl* _temp1016; struct Cyc_List_List* _temp1018; struct
Cyc_Core_Opt* _temp1020; struct _tuple0* _temp1022; struct Cyc_List_List*
_temp1024; void* _temp1026; struct Cyc_Absyn_Tunionfield* _temp1028; struct Cyc_Absyn_Tuniondecl*
_temp1030; struct Cyc_List_List* _temp1032; struct Cyc_Absyn_Tunionfield*
_temp1034; struct Cyc_Absyn_Tuniondecl* _temp1036; struct Cyc_List_List*
_temp1038; void* _temp1040; struct Cyc_Absyn_Exp* _temp1042; struct Cyc_Absyn_Stmt*
_temp1044; struct Cyc_Absyn_Fndecl* _temp1046; _LL818: if(*(( int*) _temp816) == 
Cyc_Absyn_Const_e){ _LL903: _temp902=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp816)->f1; if( _temp902 == ( void*) Cyc_Absyn_Null_c){ goto _LL819;} else{
goto _LL820;}} else{ goto _LL820;} _LL820: if(*(( int*) _temp816) ==  Cyc_Absyn_Const_e){
goto _LL821;} else{ goto _LL822;} _LL822: if(*(( int*) _temp816) ==  Cyc_Absyn_Var_e){
_LL907: _temp906=(( struct Cyc_Absyn_Var_e_struct*) _temp816)->f1; goto _LL905;
_LL905: _temp904=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp816)->f2; goto
_LL823;} else{ goto _LL824;} _LL824: if(*(( int*) _temp816) ==  Cyc_Absyn_UnknownId_e){
_LL909: _temp908=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp816)->f1; goto
_LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp816) ==  Cyc_Absyn_Primop_e){
_LL913: _temp912=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp816)->f1;
goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_Primop_e_struct*) _temp816)->f2;
goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp816) ==  Cyc_Absyn_Increment_e){
_LL917: _temp916=(( struct Cyc_Absyn_Increment_e_struct*) _temp816)->f1; goto
_LL915; _LL915: _temp914=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp816)->f2; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp816)
==  Cyc_Absyn_AssignOp_e){ _LL923: _temp922=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp816)->f1; goto _LL921; _LL921: _temp920=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp816)->f2; goto _LL919; _LL919: _temp918=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp816)->f3; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp816)
==  Cyc_Absyn_Conditional_e){ _LL929: _temp928=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp816)->f1; goto _LL927; _LL927: _temp926=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp816)->f2; goto _LL925; _LL925: _temp924=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp816)->f3; goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp816)
==  Cyc_Absyn_SeqExp_e){ _LL933: _temp932=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp816)->f1; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp816)->f2; goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp816)
==  Cyc_Absyn_UnknownCall_e){ _LL937: _temp936=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp816)->f1; goto _LL935; _LL935: _temp934=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp816)->f2; goto _LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp816)
==  Cyc_Absyn_FnCall_e){ _LL943: _temp942=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp816)->f1; goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp816)->f2; goto _LL939; _LL939: _temp938=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp816)->f3; if( _temp938 ==  0){ goto _LL839;} else{ goto _LL840;}} else{
goto _LL840;} _LL840: if(*(( int*) _temp816) ==  Cyc_Absyn_FnCall_e){ _LL956:
_temp955=(( struct Cyc_Absyn_FnCall_e_struct*) _temp816)->f1; goto _LL954;
_LL954: _temp953=(( struct Cyc_Absyn_FnCall_e_struct*) _temp816)->f2; goto
_LL945; _LL945: _temp944=(( struct Cyc_Absyn_FnCall_e_struct*) _temp816)->f3;
if( _temp944 ==  0){ goto _LL842;} else{ _temp946=* _temp944; _LL952: _temp951=
_temp946.num_varargs; goto _LL950; _LL950: _temp949= _temp946.injectors; goto
_LL948; _LL948: _temp947= _temp946.vai; goto _LL841;}} else{ goto _LL842;}
_LL842: if(*(( int*) _temp816) ==  Cyc_Absyn_Throw_e){ _LL958: _temp957=((
struct Cyc_Absyn_Throw_e_struct*) _temp816)->f1; goto _LL843;} else{ goto _LL844;}
_LL844: if(*(( int*) _temp816) ==  Cyc_Absyn_NoInstantiate_e){ _LL960: _temp959=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp816)->f1; goto _LL845;} else{
goto _LL846;} _LL846: if(*(( int*) _temp816) ==  Cyc_Absyn_Instantiate_e){
_LL964: _temp963=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp816)->f1; goto
_LL962; _LL962: _temp961=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp816)->f2;
goto _LL847;} else{ goto _LL848;} _LL848: if(*(( int*) _temp816) ==  Cyc_Absyn_Cast_e){
_LL968: _temp967=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp816)->f1;
_temp969=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp816)->f1);
goto _LL966; _LL966: _temp965=(( struct Cyc_Absyn_Cast_e_struct*) _temp816)->f2;
goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp816) ==  Cyc_Absyn_Address_e){
_LL971: _temp970=(( struct Cyc_Absyn_Address_e_struct*) _temp816)->f1; goto
_LL851;} else{ goto _LL852;} _LL852: if(*(( int*) _temp816) ==  Cyc_Absyn_New_e){
_LL975: _temp974=(( struct Cyc_Absyn_New_e_struct*) _temp816)->f1; goto _LL973;
_LL973: _temp972=(( struct Cyc_Absyn_New_e_struct*) _temp816)->f2; goto _LL853;}
else{ goto _LL854;} _LL854: if(*(( int*) _temp816) ==  Cyc_Absyn_Sizeofexp_e){
_LL977: _temp976=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp816)->f1; goto
_LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp816) ==  Cyc_Absyn_Sizeoftyp_e){
_LL979: _temp978=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp816)->f1;
goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp816) ==  Cyc_Absyn_Gentyp_e){
goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp816) ==  Cyc_Absyn_Offsetof_e){
_LL985: _temp984=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp816)->f1;
goto _LL981; _LL981: _temp980=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp816)->f2; if(*(( int*) _temp980) ==  Cyc_Absyn_StructField){ _LL983:
_temp982=(( struct Cyc_Absyn_StructField_struct*) _temp980)->f1; goto _LL861;}
else{ goto _LL862;}} else{ goto _LL862;} _LL862: if(*(( int*) _temp816) ==  Cyc_Absyn_Offsetof_e){
_LL991: _temp990=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp816)->f1;
goto _LL987; _LL987: _temp986=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp816)->f2; if(*(( int*) _temp986) ==  Cyc_Absyn_TupleIndex){ _LL989:
_temp988=(( struct Cyc_Absyn_TupleIndex_struct*) _temp986)->f1; goto _LL863;}
else{ goto _LL864;}} else{ goto _LL864;} _LL864: if(*(( int*) _temp816) ==  Cyc_Absyn_Deref_e){
_LL993: _temp992=(( struct Cyc_Absyn_Deref_e_struct*) _temp816)->f1; goto _LL865;}
else{ goto _LL866;} _LL866: if(*(( int*) _temp816) ==  Cyc_Absyn_StructMember_e){
_LL997: _temp996=(( struct Cyc_Absyn_StructMember_e_struct*) _temp816)->f1; goto
_LL995; _LL995: _temp994=(( struct Cyc_Absyn_StructMember_e_struct*) _temp816)->f2;
goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp816) ==  Cyc_Absyn_StructArrow_e){
_LL1001: _temp1000=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp816)->f1;
goto _LL999; _LL999: _temp998=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp816)->f2; goto _LL869;} else{ goto _LL870;} _LL870: if(*(( int*) _temp816)
==  Cyc_Absyn_Subscript_e){ _LL1005: _temp1004=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp816)->f1; goto _LL1003; _LL1003: _temp1002=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp816)->f2; goto _LL871;} else{ goto _LL872;} _LL872: if(*(( int*) _temp816)
==  Cyc_Absyn_Tuple_e){ _LL1007: _temp1006=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp816)->f1; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp816)
==  Cyc_Absyn_Array_e){ _LL1009: _temp1008=(( struct Cyc_Absyn_Array_e_struct*)
_temp816)->f1; goto _LL875;} else{ goto _LL876;} _LL876: if(*(( int*) _temp816)
==  Cyc_Absyn_Comprehension_e){ _LL1015: _temp1014=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp816)->f1; goto _LL1013; _LL1013: _temp1012=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp816)->f2; goto _LL1011; _LL1011: _temp1010=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp816)->f3; goto _LL877;} else{ goto _LL878;} _LL878: if(*(( int*) _temp816)
==  Cyc_Absyn_Struct_e){ _LL1023: _temp1022=(( struct Cyc_Absyn_Struct_e_struct*)
_temp816)->f1; goto _LL1021; _LL1021: _temp1020=(( struct Cyc_Absyn_Struct_e_struct*)
_temp816)->f2; goto _LL1019; _LL1019: _temp1018=(( struct Cyc_Absyn_Struct_e_struct*)
_temp816)->f3; goto _LL1017; _LL1017: _temp1016=(( struct Cyc_Absyn_Struct_e_struct*)
_temp816)->f4; goto _LL879;} else{ goto _LL880;} _LL880: if(*(( int*) _temp816)
==  Cyc_Absyn_AnonStruct_e){ _LL1027: _temp1026=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp816)->f1; goto _LL1025; _LL1025: _temp1024=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp816)->f2; goto _LL881;} else{ goto _LL882;} _LL882: if(*(( int*) _temp816)
==  Cyc_Absyn_Tunion_e){ _LL1033: _temp1032=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp816)->f3; if( _temp1032 ==  0){ goto _LL1031;} else{ goto _LL884;} _LL1031:
_temp1030=(( struct Cyc_Absyn_Tunion_e_struct*) _temp816)->f4; goto _LL1029;
_LL1029: _temp1028=(( struct Cyc_Absyn_Tunion_e_struct*) _temp816)->f5; goto
_LL883;} else{ goto _LL884;} _LL884: if(*(( int*) _temp816) ==  Cyc_Absyn_Tunion_e){
_LL1039: _temp1038=(( struct Cyc_Absyn_Tunion_e_struct*) _temp816)->f3; goto
_LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_Tunion_e_struct*) _temp816)->f4;
goto _LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_Tunion_e_struct*) _temp816)->f5;
goto _LL885;} else{ goto _LL886;} _LL886: if(*(( int*) _temp816) ==  Cyc_Absyn_Enum_e){
goto _LL887;} else{ goto _LL888;} _LL888: if(*(( int*) _temp816) ==  Cyc_Absyn_AnonEnum_e){
goto _LL889;} else{ goto _LL890;} _LL890: if(*(( int*) _temp816) ==  Cyc_Absyn_Malloc_e){
_LL1043: _temp1042=(( struct Cyc_Absyn_Malloc_e_struct*) _temp816)->f1; goto
_LL1041; _LL1041: _temp1040=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp816)->f2; goto _LL891;} else{ goto _LL892;} _LL892: if(*(( int*) _temp816)
==  Cyc_Absyn_StmtExp_e){ _LL1045: _temp1044=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp816)->f1; goto _LL893;} else{ goto _LL894;} _LL894: if(*(( int*) _temp816)
==  Cyc_Absyn_UnresolvedMem_e){ goto _LL895;} else{ goto _LL896;} _LL896: if(*((
int*) _temp816) ==  Cyc_Absyn_CompoundLit_e){ goto _LL897;} else{ goto _LL898;}
_LL898: if(*(( int*) _temp816) ==  Cyc_Absyn_Codegen_e){ _LL1047: _temp1046=((
struct Cyc_Absyn_Codegen_e_struct*) _temp816)->f1; goto _LL899;} else{ goto
_LL900;} _LL900: if(*(( int*) _temp816) ==  Cyc_Absyn_Fill_e){ goto _LL901;}
else{ goto _LL817;} _LL819: { struct Cyc_Absyn_Exp* _temp1048= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1048,
_temp1048))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1049[ 3u]; _temp1049[ 2u]=
_temp1048; _temp1049[ 1u]= _temp1048; _temp1049[ 0u]= _temp1048;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1049, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL817;} _LL821: goto _LL817; _LL823:{ struct _handler_cons
_temp1050; _push_handler(& _temp1050);{ int _temp1052= 0; if( setjmp( _temp1050.handler)){
_temp1052= 1;} if( ! _temp1052){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp906))->r));; _pop_handler();} else{ void* _temp1051=( void*) _exn_thrown;
void* _temp1054= _temp1051; _LL1056: if( _temp1054 ==  Cyc_Dict_Absent){ goto
_LL1057;} else{ goto _LL1058;} _LL1058: goto _LL1059; _LL1057:({ struct Cyc_Std_String_pa_struct
_temp1061; _temp1061.tag= Cyc_Std_String_pa; _temp1061.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp906);{ void* _temp1060[ 1u]={& _temp1061};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1060, sizeof( void*), 1u));}});
return; _LL1059:( void) _throw( _temp1054); _LL1055:;}}} goto _LL817; _LL825:({
void* _temp1062[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1062, sizeof( void*), 0u));}); goto _LL817; _LL827: { struct Cyc_List_List*
_temp1063=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp910);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp910);{
void* _temp1064= _temp912; _LL1066: if( _temp1064 == ( void*) Cyc_Absyn_Size){
goto _LL1067;} else{ goto _LL1068;} _LL1068: if( _temp1064 == ( void*) Cyc_Absyn_Plus){
goto _LL1069;} else{ goto _LL1070;} _LL1070: if( _temp1064 == ( void*) Cyc_Absyn_Minus){
goto _LL1071;} else{ goto _LL1072;} _LL1072: if( _temp1064 == ( void*) Cyc_Absyn_Eq){
goto _LL1073;} else{ goto _LL1074;} _LL1074: if( _temp1064 == ( void*) Cyc_Absyn_Neq){
goto _LL1075;} else{ goto _LL1076;} _LL1076: if( _temp1064 == ( void*) Cyc_Absyn_Gt){
goto _LL1077;} else{ goto _LL1078;} _LL1078: if( _temp1064 == ( void*) Cyc_Absyn_Gte){
goto _LL1079;} else{ goto _LL1080;} _LL1080: if( _temp1064 == ( void*) Cyc_Absyn_Lt){
goto _LL1081;} else{ goto _LL1082;} _LL1082: if( _temp1064 == ( void*) Cyc_Absyn_Lte){
goto _LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085; _LL1067: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp910))->hd;{
void* _temp1086= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1094; struct Cyc_Absyn_PtrInfo
_temp1096; struct Cyc_Absyn_Conref* _temp1098; void* _temp1100; _LL1088: if((
unsigned int) _temp1086 >  3u?*(( int*) _temp1086) ==  Cyc_Absyn_ArrayType: 0){
_LL1095: _temp1094=(( struct Cyc_Absyn_ArrayType_struct*) _temp1086)->f3; goto
_LL1089;} else{ goto _LL1090;} _LL1090: if(( unsigned int) _temp1086 >  3u?*((
int*) _temp1086) ==  Cyc_Absyn_PointerType: 0){ _LL1097: _temp1096=(( struct Cyc_Absyn_PointerType_struct*)
_temp1086)->f1; _LL1101: _temp1100=( void*) _temp1096.elt_typ; goto _LL1099;
_LL1099: _temp1098= _temp1096.bounds; goto _LL1091;} else{ goto _LL1092;}
_LL1092: goto _LL1093; _LL1089:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1094))->r)); goto _LL1087; _LL1091:{ void* _temp1102= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1098); struct Cyc_Absyn_Exp* _temp1108; _LL1104: if(
_temp1102 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1105;} else{ goto _LL1106;}
_LL1106: if(( unsigned int) _temp1102 >  1u?*(( int*) _temp1102) ==  Cyc_Absyn_Upper_b:
0){ _LL1109: _temp1108=(( struct Cyc_Absyn_Upper_b_struct*) _temp1102)->f1; goto
_LL1107;} else{ goto _LL1103;} _LL1105:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1110[ 2u]; _temp1110[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1100), 0); _temp1110[ 0u]=(
struct Cyc_Absyn_Exp*) _temp910->hd;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1110, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0))->r)); goto _LL1103; _LL1107:( void*)( e->r=( void*)(( void*)
_temp1108->r)); goto _LL1103; _LL1103:;} goto _LL1087; _LL1093:({ struct Cyc_Std_String_pa_struct
_temp1113; _temp1113.tag= Cyc_Std_String_pa; _temp1113.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1112; _temp1112.tag= Cyc_Std_String_pa;
_temp1112.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v);{ void* _temp1111[ 2u]={& _temp1112,& _temp1113};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("size primop applied to non-array %s (%s)", sizeof( unsigned char), 41u),
_tag_arr( _temp1111, sizeof( void*), 2u));}}}); return; _LL1087:;} goto _LL1065;}
_LL1069: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1063))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp910))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp910->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1114[ 3u]; _temp1114[ 2u]=
e2; _temp1114[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1114[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1114, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1065;} _LL1071: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1063))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp910))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp910->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null( _temp1063->tl))->hd)){( void*)( e1->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp,
0))->r));( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_divide_exp( Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0), 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1115[ 3u]; _temp1115[ 2u]=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, e2, 0); _temp1115[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1115[ 0u]= e1;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1115, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} goto _LL1065;} _LL1073: goto _LL1075; _LL1075: goto _LL1077;
_LL1077: goto _LL1079; _LL1079: goto _LL1081; _LL1081: goto _LL1083; _LL1083: {
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp910))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp910->tl))->hd; void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp1063))->hd; void* t2=( void*)(( struct
Cyc_List_List*) _check_null( _temp1063->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1065;} _LL1085: goto
_LL1065; _LL1065:;} goto _LL817;} _LL829: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp916->topt))->v; Cyc_Toc_exp_to_c( nv, _temp916);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1116=( _temp914 == ( void*) Cyc_Absyn_PostInc?
1: _temp914 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1117=( _temp914 == ( void*) Cyc_Absyn_PreInc?
1: _temp914 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1116,({ struct Cyc_Absyn_Exp* _temp1118[ 3u];
_temp1118[ 2u]= Cyc_Absyn_signed_int_exp( _temp1117, 0); _temp1118[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1118[ 0u]= Cyc_Absyn_address_exp( _temp916,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1118, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp916): 0){ Cyc_Toc_lvalue_assign(
_temp916, 0, Cyc_Toc_incr_lvalue, _temp914);( void*)( e->r=( void*)(( void*)
_temp916->r));} goto _LL817;}} _LL831: { int e1_poly= Cyc_Toc_is_poly_project(
_temp922); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp922->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp918->topt))->v; Cyc_Toc_exp_to_c( nv, _temp922); Cyc_Toc_exp_to_c(
nv, _temp918);{ int done= 0; if( _temp920 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1119=( void*) _temp920->v; _LL1121: if( _temp1119 == ( void*)
Cyc_Absyn_Plus){ goto _LL1122;} else{ goto _LL1123;} _LL1123: if( _temp1119 == (
void*) Cyc_Absyn_Minus){ goto _LL1124;} else{ goto _LL1125;} _LL1125: goto
_LL1126; _LL1122: change= _temp918; goto _LL1120; _LL1124: change= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, _temp918, 0); goto _LL1120; _LL1126:( int) _throw(({
void* _temp1127[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u), _tag_arr( _temp1127, sizeof( void*), 0u));}));
goto _LL1120; _LL1120:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_Absyn_Exp* _temp1128[ 3u];
_temp1128[ 2u]= change; _temp1128[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1128[ 0u]= Cyc_Absyn_address_exp( _temp922, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1128, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp918->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp918->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp922)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)(
_temp922, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1129=( struct
_tuple7*) _cycalloc( sizeof( struct _tuple7) *  1); _temp1129[ 0]=({ struct
_tuple7 _temp1130; _temp1130.f1= _temp920; _temp1130.f2= _temp918; _temp1130;});
_temp1129;}));( void*)( e->r=( void*)(( void*) _temp922->r));}} goto _LL817;}}
_LL833: Cyc_Toc_exp_to_c( nv, _temp928); Cyc_Toc_exp_to_c( nv, _temp926); Cyc_Toc_exp_to_c(
nv, _temp924); goto _LL817; _LL835: Cyc_Toc_exp_to_c( nv, _temp932); Cyc_Toc_exp_to_c(
nv, _temp930); goto _LL817; _LL837: _temp942= _temp936; _temp940= _temp934; goto
_LL839; _LL839: Cyc_Toc_exp_to_c( nv, _temp942);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp940); goto _LL817; _LL841:{
struct _RegionHandle _temp1131= _new_region("r"); struct _RegionHandle* r=&
_temp1131; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp951, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp947->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1148=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1148[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1149; _temp1149.tag= Cyc_Absyn_ArrayType;
_temp1149.f1=( void*) cva_type; _temp1149.f2= Cyc_Toc_mt_tq; _temp1149.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1149;}); _temp1148;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp953); int num_normargs=
num_args -  _temp951; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp953= _temp953->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp953))->hd); new_args=({ struct Cyc_List_List*
_temp1132=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1132->hd=( void*)(( struct Cyc_Absyn_Exp*) _temp953->hd); _temp1132->tl=
new_args; _temp1132;});}} new_args=({ struct Cyc_List_List* _temp1133=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1133->hd=( void*)
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1134[ 3u];
_temp1134[ 2u]= num_varargs_exp; _temp1134[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1134[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1134, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1133->tl= new_args; _temp1133;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp955);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp955, new_args, 0), 0); if( _temp947->inject){ struct _tagged_arr vs=({
unsigned int _temp1144=( unsigned int) _temp951; struct _tuple0** _temp1145=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1144)); struct _tagged_arr _temp1147= _tag_arr( _temp1145, sizeof( struct
_tuple0*),( unsigned int) _temp951);{ unsigned int _temp1146= _temp1144;
unsigned int i; for( i= 0; i <  _temp1146; i ++){ _temp1145[ i]= Cyc_Toc_temp_var();}};
_temp1147;}); struct Cyc_List_List* _temp1135= 0;{ int i= _temp951 -  1; for( 0;
i >=  0; -- i){ _temp1135=({ struct Cyc_List_List* _temp1136=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1136->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1136->tl=
_temp1135; _temp1136;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1135, 0), s, 0);{ int i= 0;
for( 0; _temp953 !=  0;((( _temp953= _temp953->tl, _temp949= _temp949->tl)), ++
i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*) _temp953->hd; void*
arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; struct
_tuple0* var=*(( struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct
_tuple0*), i)); struct Cyc_Absyn_Exp* varexp= Cyc_Absyn_var_exp( var, 0); struct
Cyc_Absyn_Tunionfield _temp1139; struct Cyc_List_List* _temp1140; struct _tuple0*
_temp1142; struct Cyc_Absyn_Tunionfield* _temp1137=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp949))->hd; _temp1139=* _temp1137;
_LL1143: _temp1142= _temp1139.name; goto _LL1141; _LL1141: _temp1140= _temp1139.typs;
goto _LL1138; _LL1138: { void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1140))->hd)).f2); Cyc_Toc_exp_to_c( nv,
arg); if( Cyc_Toc_is_void_star( field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ,
arg, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Absyn_fieldname( 1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1142, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt( var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1142, _tag_arr("_struct", sizeof( unsigned char), 8u))), 0, s, 0);}}}}
else{{ int i= 0; for( 0; _temp953 !=  0;( _temp953= _temp953->tl, ++ i)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp953->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),(
struct Cyc_Absyn_Exp*) _temp953->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL817; _LL843: Cyc_Toc_exp_to_c( nv,
_temp957);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp957), 0))->r)); goto _LL817; _LL845: Cyc_Toc_exp_to_c(
nv, _temp959); goto _LL817; _LL847: Cyc_Toc_exp_to_c( nv, _temp963); for( 0;
_temp961 !=  0; _temp961= _temp961->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)
_temp961->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1150= Cyc_Tcutil_compress(( void*) _temp961->hd); _LL1152: if((
unsigned int) _temp1150 >  3u?*(( int*) _temp1150) ==  Cyc_Absyn_VarType: 0){
goto _LL1153;} else{ goto _LL1154;} _LL1154: if(( unsigned int) _temp1150 >  3u?*((
int*) _temp1150) ==  Cyc_Absyn_TunionType: 0){ goto _LL1155;} else{ goto _LL1156;}
_LL1156: goto _LL1157; _LL1153: continue; _LL1155: continue; _LL1157:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), _temp963, 0))->r)); goto _LL1151;
_LL1151:;} break;}} goto _LL817; _LL849: { void* old_t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp965->topt))->v; void* new_typ=* _temp969;* _temp969= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv, _temp965);{ struct _tuple8 _temp1159=({ struct
_tuple8 _temp1158; _temp1158.f1= Cyc_Tcutil_compress( old_t2); _temp1158.f2= Cyc_Tcutil_compress(
new_typ); _temp1158;}); void* _temp1167; struct Cyc_Absyn_PtrInfo _temp1169;
void* _temp1171; struct Cyc_Absyn_PtrInfo _temp1173; void* _temp1175; void*
_temp1177; struct Cyc_Absyn_PtrInfo _temp1179; _LL1161: _LL1172: _temp1171=
_temp1159.f1; if(( unsigned int) _temp1171 >  3u?*(( int*) _temp1171) ==  Cyc_Absyn_PointerType:
0){ _LL1174: _temp1173=(( struct Cyc_Absyn_PointerType_struct*) _temp1171)->f1;
goto _LL1168;} else{ goto _LL1163;} _LL1168: _temp1167= _temp1159.f2; if((
unsigned int) _temp1167 >  3u?*(( int*) _temp1167) ==  Cyc_Absyn_PointerType: 0){
_LL1170: _temp1169=(( struct Cyc_Absyn_PointerType_struct*) _temp1167)->f1; goto
_LL1162;} else{ goto _LL1163;} _LL1163: _LL1178: _temp1177= _temp1159.f1; if((
unsigned int) _temp1177 >  3u?*(( int*) _temp1177) ==  Cyc_Absyn_PointerType: 0){
_LL1180: _temp1179=(( struct Cyc_Absyn_PointerType_struct*) _temp1177)->f1; goto
_LL1176;} else{ goto _LL1165;} _LL1176: _temp1175= _temp1159.f2; if((
unsigned int) _temp1175 >  3u?*(( int*) _temp1175) ==  Cyc_Absyn_IntType: 0){
goto _LL1164;} else{ goto _LL1165;} _LL1165: goto _LL1166; _LL1162: { int
_temp1181=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1173.nullable); int _temp1182=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1169.nullable); void* _temp1183= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1173.bounds); void* _temp1184= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1169.bounds);{ struct _tuple8 _temp1186=({ struct
_tuple8 _temp1185; _temp1185.f1= _temp1183; _temp1185.f2= _temp1184; _temp1185;});
void* _temp1196; void* _temp1198; void* _temp1200; void* _temp1202; struct Cyc_Absyn_Exp*
_temp1204; void* _temp1206; struct Cyc_Absyn_Exp* _temp1208; void* _temp1210;
_LL1188: _LL1199: _temp1198= _temp1186.f1; if(( unsigned int) _temp1198 >  1u?*((
int*) _temp1198) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1197;} else{ goto _LL1190;}
_LL1197: _temp1196= _temp1186.f2; if(( unsigned int) _temp1196 >  1u?*(( int*)
_temp1196) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1189;} else{ goto _LL1190;}
_LL1190: _LL1203: _temp1202= _temp1186.f1; if(( unsigned int) _temp1202 >  1u?*((
int*) _temp1202) ==  Cyc_Absyn_Upper_b: 0){ _LL1205: _temp1204=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1202)->f1; goto _LL1201;} else{ goto _LL1192;} _LL1201: _temp1200=
_temp1186.f2; if( _temp1200 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1191;}
else{ goto _LL1192;} _LL1192: _LL1211: _temp1210= _temp1186.f1; if( _temp1210 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1207;} else{ goto _LL1194;} _LL1207:
_temp1206= _temp1186.f2; if(( unsigned int) _temp1206 >  1u?*(( int*) _temp1206)
==  Cyc_Absyn_Upper_b: 0){ _LL1209: _temp1208=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1206)->f1; goto _LL1193;} else{ goto _LL1194;} _LL1194: goto _LL1195;
_LL1189: if( _temp1181? ! _temp1182: 0){ if( nv->toplevel){({ void* _temp1212[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1212, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp969, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1213=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1213->hd=( void*) _temp965;
_temp1213->tl= 0; _temp1213;}), 0), 0))->r));} goto _LL1187; _LL1191: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1204, _temp965))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1214[ 3u]; _temp1214[ 2u]=
_temp1204; _temp1214[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1169.elt_typ), 0); _temp1214[ 0u]= _temp965;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1214, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1187; _LL1193: if( nv->toplevel){({ void* _temp1215[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1215, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1216= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1218[ 3u]; _temp1218[ 2u]= _temp1208; _temp1218[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1173.elt_typ), 0); _temp1218[ 0u]= _temp965;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1218, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1182){( void*)(
_temp1216->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1217=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1217->hd=( void*) Cyc_Absyn_copy_exp( _temp1216);
_temp1217->tl= 0; _temp1217;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp969, _temp1216, 0))->r)); goto _LL1187;} _LL1195: goto _LL1187; _LL1187:;}
goto _LL1160;} _LL1164:{ void* _temp1219= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1179.bounds); _LL1221: if( _temp1219 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1222;} else{ goto _LL1223;} _LL1223: goto _LL1224; _LL1222:( void*)( _temp965->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp965->r,
_temp965->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1220; _LL1224: goto _LL1220;
_LL1220:;} goto _LL1160; _LL1166: goto _LL1160; _LL1160:;} goto _LL817;} _LL851:{
void* _temp1225=( void*) _temp970->r; struct Cyc_List_List* _temp1233; struct
_tuple0* _temp1235; struct Cyc_List_List* _temp1237; _LL1227: if(*(( int*)
_temp1225) ==  Cyc_Absyn_Struct_e){ _LL1236: _temp1235=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1225)->f1; goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1225)->f3; goto _LL1228;} else{ goto _LL1229;} _LL1229: if(*(( int*)
_temp1225) ==  Cyc_Absyn_Tuple_e){ _LL1238: _temp1237=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1225)->f1; goto _LL1230;} else{ goto _LL1231;} _LL1231: goto _LL1232;
_LL1228: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1240;
_temp1240.tag= Cyc_Std_String_pa; _temp1240.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp970->loc);{ void* _temp1239[ 1u]={& _temp1240};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1239, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp970->topt))->v, 1, 0, _temp1233, _temp1235))->r)); goto
_LL1226; _LL1230: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1242;
_temp1242.tag= Cyc_Std_String_pa; _temp1242.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp970->loc);{ void* _temp1241[ 1u]={& _temp1242};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1241, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1237))->r));
goto _LL1226; _LL1232: Cyc_Toc_exp_to_c( nv, _temp970); if( ! Cyc_Absyn_is_lvalue(
_temp970)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp970, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp970, 0))->r));} goto _LL1226; _LL1226:;} goto _LL817; _LL853: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1244; _temp1244.tag= Cyc_Std_String_pa;
_temp1244.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp972->loc);{
void* _temp1243[ 1u]={& _temp1244};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1243, sizeof( void*), 1u));}});}{ void*
_temp1245=( void*) _temp972->r; struct Cyc_List_List* _temp1257; struct Cyc_Absyn_Exp*
_temp1259; struct Cyc_Absyn_Exp* _temp1261; struct Cyc_Absyn_Vardecl* _temp1263;
struct Cyc_List_List* _temp1265; struct _tuple0* _temp1267; struct Cyc_List_List*
_temp1269; _LL1247: if(*(( int*) _temp1245) ==  Cyc_Absyn_Array_e){ _LL1258:
_temp1257=(( struct Cyc_Absyn_Array_e_struct*) _temp1245)->f1; goto _LL1248;}
else{ goto _LL1249;} _LL1249: if(*(( int*) _temp1245) ==  Cyc_Absyn_Comprehension_e){
_LL1264: _temp1263=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1245)->f1;
goto _LL1262; _LL1262: _temp1261=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1245)->f2; goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1245)->f3; goto _LL1250;} else{ goto _LL1251;} _LL1251: if(*(( int*)
_temp1245) ==  Cyc_Absyn_Struct_e){ _LL1268: _temp1267=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1245)->f1; goto _LL1266; _LL1266: _temp1265=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1245)->f3; goto _LL1252;} else{ goto _LL1253;} _LL1253: if(*(( int*)
_temp1245) ==  Cyc_Absyn_Tuple_e){ _LL1270: _temp1269=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1245)->f1; goto _LL1254;} else{ goto _LL1255;} _LL1255: goto _LL1256;
_LL1248: { struct _tuple0* _temp1271= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1272= Cyc_Absyn_var_exp( _temp1271, 0); struct Cyc_Absyn_Stmt* _temp1273=
Cyc_Toc_init_array( nv, _temp1272, _temp1257, Cyc_Absyn_exp_stmt( _temp1272, 0));
void* old_elt_typ;{ void* _temp1274= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1280; struct Cyc_Absyn_Tqual _temp1282; void* _temp1284; _LL1276: if((
unsigned int) _temp1274 >  3u?*(( int*) _temp1274) ==  Cyc_Absyn_PointerType: 0){
_LL1281: _temp1280=(( struct Cyc_Absyn_PointerType_struct*) _temp1274)->f1;
_LL1285: _temp1284=( void*) _temp1280.elt_typ; goto _LL1283; _LL1283: _temp1282=
_temp1280.tq; goto _LL1277;} else{ goto _LL1278;} _LL1278: goto _LL1279; _LL1277:
old_elt_typ= _temp1284; goto _LL1275; _LL1279: old_elt_typ=({ void* _temp1286[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1286, sizeof( void*), 0u));}); goto
_LL1275; _LL1275:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1287= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1288= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1257), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp974 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1288);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp974); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1288);}{
struct Cyc_Absyn_Exp* _temp1289= Cyc_Absyn_cast_exp( _temp1287, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1271,
_temp1287,( struct Cyc_Absyn_Exp*) _temp1289, _temp1273, 0), 0))->r)); goto
_LL1246;}}} _LL1250: { int is_tagged_ptr= 0;{ void* _temp1290= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1296; struct Cyc_Absyn_Conref* _temp1298;
struct Cyc_Absyn_Tqual _temp1300; void* _temp1302; _LL1292: if(( unsigned int)
_temp1290 >  3u?*(( int*) _temp1290) ==  Cyc_Absyn_PointerType: 0){ _LL1297:
_temp1296=(( struct Cyc_Absyn_PointerType_struct*) _temp1290)->f1; _LL1303:
_temp1302=( void*) _temp1296.elt_typ; goto _LL1301; _LL1301: _temp1300=
_temp1296.tq; goto _LL1299; _LL1299: _temp1298= _temp1296.bounds; goto _LL1293;}
else{ goto _LL1294;} _LL1294: goto _LL1295; _LL1293: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1298) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1291;
_LL1295:({ void* _temp1304[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1304, sizeof( void*), 0u));}); goto
_LL1291; _LL1291:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1259->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1261);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1263, Cyc_Absyn_var_exp( max, 0), _temp1259, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1305= _new_region("r"); struct _RegionHandle* r=&
_temp1305; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1325=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1325->hd=( void*)({ struct _tuple9* _temp1326=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1326->f1= max; _temp1326->f2=
Cyc_Absyn_uint_t; _temp1326->f3=( struct Cyc_Absyn_Exp*) _temp1261; _temp1326;});
_temp1325->tl= 0; _temp1325;}); struct Cyc_Absyn_Exp* ai; if( _temp974 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1306[ 2u]; _temp1306[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1306[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1306, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp974); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1307[
2u]; _temp1307[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1307[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1307, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1308=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1308->hd=( void*)({ struct _tuple9* _temp1309=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1309->f1= a; _temp1309->f2=
ptr_typ; _temp1309->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1309;}); _temp1308->tl=
decls; _temp1308;}); if( is_tagged_ptr){ struct _tuple0* _temp1310= Cyc_Toc_temp_var();
void* _temp1311= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1312=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1315[ 3u];
_temp1315[ 2u]= _temp1261; _temp1315[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1315[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1315, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1313=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1313->hd=( void*)({
struct _tuple9* _temp1314=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1314->f1= _temp1310; _temp1314->f2= _temp1311; _temp1314->f3=(
struct Cyc_Absyn_Exp*) _temp1312; _temp1314;}); _temp1313->tl= decls; _temp1313;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1310, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1316= decls; for( 0; _temp1316 !=  0;
_temp1316= _temp1316->tl){ struct Cyc_Absyn_Exp* _temp1319; void* _temp1321;
struct _tuple0* _temp1323; struct _tuple9 _temp1317=*(( struct _tuple9*)
_temp1316->hd); _LL1324: _temp1323= _temp1317.f1; goto _LL1322; _LL1322:
_temp1321= _temp1317.f2; goto _LL1320; _LL1320: _temp1319= _temp1317.f3; goto
_LL1318; _LL1318: s= Cyc_Absyn_declare_stmt( _temp1323, _temp1321, _temp1319, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region( r);} goto _LL1246;}}} _LL1252:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp972->topt))->v, 1,
_temp974, _temp1265, _temp1267))->r)); goto _LL1246; _LL1254:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp974, _temp1269))->r)); goto
_LL1246; _LL1256: { void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp972->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); if( _temp974
==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp974); Cyc_Toc_exp_to_c( nv, r);
mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1327=( void*)
_temp972->r; struct Cyc_Absyn_Exp* _temp1333; void* _temp1335; _LL1329: if(*((
int*) _temp1327) ==  Cyc_Absyn_Cast_e){ _LL1336: _temp1335=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1327)->f1; goto _LL1334; _LL1334: _temp1333=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1327)->f2; goto _LL1330;} else{ goto _LL1331;} _LL1331: goto _LL1332;
_LL1330:{ struct _tuple8 _temp1338=({ struct _tuple8 _temp1337; _temp1337.f1=
Cyc_Tcutil_compress( _temp1335); _temp1337.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1333->topt))->v); _temp1337;}); void*
_temp1344; struct Cyc_Absyn_PtrInfo _temp1346; struct Cyc_Absyn_Conref*
_temp1348; void* _temp1350; struct Cyc_Absyn_PtrInfo _temp1352; struct Cyc_Absyn_Conref*
_temp1354; void* _temp1356; _LL1340: _LL1351: _temp1350= _temp1338.f1; if((
unsigned int) _temp1350 >  3u?*(( int*) _temp1350) ==  Cyc_Absyn_PointerType: 0){
_LL1353: _temp1352=(( struct Cyc_Absyn_PointerType_struct*) _temp1350)->f1;
_LL1357: _temp1356=( void*) _temp1352.elt_typ; goto _LL1355; _LL1355: _temp1354=
_temp1352.bounds; goto _LL1345;} else{ goto _LL1342;} _LL1345: _temp1344=
_temp1338.f2; if(( unsigned int) _temp1344 >  3u?*(( int*) _temp1344) ==  Cyc_Absyn_PointerType:
0){ _LL1347: _temp1346=(( struct Cyc_Absyn_PointerType_struct*) _temp1344)->f1;
_LL1349: _temp1348= _temp1346.bounds; goto _LL1341;} else{ goto _LL1342;}
_LL1342: goto _LL1343; _LL1341:{ struct _tuple8 _temp1359=({ struct _tuple8
_temp1358; _temp1358.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1354);
_temp1358.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1348); _temp1358;});
void* _temp1365; struct Cyc_Absyn_Exp* _temp1367; void* _temp1369; _LL1361:
_LL1370: _temp1369= _temp1359.f1; if( _temp1369 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1366;} else{ goto _LL1363;} _LL1366: _temp1365= _temp1359.f2; if((
unsigned int) _temp1365 >  1u?*(( int*) _temp1365) ==  Cyc_Absyn_Upper_b: 0){
_LL1368: _temp1367=(( struct Cyc_Absyn_Upper_b_struct*) _temp1365)->f1; goto
_LL1362;} else{ goto _LL1363;} _LL1363: goto _LL1364; _LL1362: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1371[ 4u]; _temp1371[ 3u]= _temp1367; _temp1371[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1356, 0); _temp1371[ 1u]= _temp1333; _temp1371[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1371, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1360; _LL1364: goto _LL1360; _LL1360:;} goto _LL1339;
_LL1343: goto _LL1339; _LL1339:;} goto _LL1328; _LL1332: goto _LL1328; _LL1328:;}
if( ! done){ struct _tuple0* _temp1372= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1373= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1372, 0), 0); struct Cyc_Absyn_Exp*
_temp1374= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp972);
_temp1373= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1372, 0), _temp1374, 0), _temp972, 0), _temp1373, 0);{
void* _temp1375= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1372,
_temp1375,( struct Cyc_Absyn_Exp*) mexp, _temp1373, 0), 0))->r));}} goto _LL1246;}}
_LL1246:;} goto _LL817; _LL855: Cyc_Toc_exp_to_c( nv, _temp976); goto _LL817;
_LL857:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1376=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1376[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1377; _temp1377.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1377.f1=( void*) Cyc_Toc_typ_to_c_array( _temp978);
_temp1377;}); _temp1376;}))); goto _LL817; _LL859:( int) _throw(({ void*
_temp1378[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("__gen() in code generator",
sizeof( unsigned char), 26u), _tag_arr( _temp1378, sizeof( void*), 0u));}));
_LL861:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1379=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1379[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1380; _temp1380.tag=
Cyc_Absyn_Offsetof_e; _temp1380.f1=( void*) Cyc_Toc_typ_to_c_array( _temp984);
_temp1380.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1381=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1381[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1382; _temp1382.tag=
Cyc_Absyn_StructField; _temp1382.f1= _temp982; _temp1382;}); _temp1381;}));
_temp1380;}); _temp1379;}))); goto _LL817; _LL863:{ void* _temp1383= Cyc_Tcutil_compress(
_temp990); struct Cyc_Absyn_Structdecl** _temp1399; struct Cyc_List_List*
_temp1401; struct Cyc_Absyn_Uniondecl** _temp1403; struct Cyc_List_List*
_temp1405; _LL1385: if(( unsigned int) _temp1383 >  3u?*(( int*) _temp1383) == 
Cyc_Absyn_StructType: 0){ _LL1400: _temp1399=(( struct Cyc_Absyn_StructType_struct*)
_temp1383)->f3; goto _LL1386;} else{ goto _LL1387;} _LL1387: if(( unsigned int)
_temp1383 >  3u?*(( int*) _temp1383) ==  Cyc_Absyn_AnonStructType: 0){ _LL1402:
_temp1401=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1383)->f1; goto
_LL1388;} else{ goto _LL1389;} _LL1389: if(( unsigned int) _temp1383 >  3u?*((
int*) _temp1383) ==  Cyc_Absyn_UnionType: 0){ _LL1404: _temp1403=(( struct Cyc_Absyn_UnionType_struct*)
_temp1383)->f3; goto _LL1390;} else{ goto _LL1391;} _LL1391: if(( unsigned int)
_temp1383 >  3u?*(( int*) _temp1383) ==  Cyc_Absyn_AnonUnionType: 0){ _LL1406:
_temp1405=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1383)->f1; goto
_LL1392;} else{ goto _LL1393;} _LL1393: if(( unsigned int) _temp1383 >  3u?*((
int*) _temp1383) ==  Cyc_Absyn_TupleType: 0){ goto _LL1394;} else{ goto _LL1395;}
_LL1395: if(( unsigned int) _temp1383 >  3u?*(( int*) _temp1383) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL1396;} else{ goto _LL1397;} _LL1397: goto _LL1398; _LL1386: if(
_temp1399 ==  0? 1:(* _temp1399)->fields ==  0){( int) _throw(({ void* _temp1407[
0u]={}; Cyc_Toc_toc_impos( _tag_arr("struct fields must be known", sizeof(
unsigned char), 28u), _tag_arr( _temp1407, sizeof( void*), 0u));}));} _temp1401=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp1399)->fields))->v;
goto _LL1388; _LL1388: { struct Cyc_Absyn_Structfield* _temp1408=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1401,( int) _temp988);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1409=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1409[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1410; _temp1410.tag=
Cyc_Absyn_Offsetof_e; _temp1410.f1=( void*) Cyc_Toc_typ_to_c_array( _temp990);
_temp1410.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1411=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1411[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1412; _temp1412.tag=
Cyc_Absyn_StructField; _temp1412.f1= _temp1408->name; _temp1412;}); _temp1411;}));
_temp1410;}); _temp1409;}))); goto _LL1384;} _LL1390: if( _temp1403 ==  0? 1:(*
_temp1403)->fields ==  0){( int) _throw(({ void* _temp1413[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("union fields must be known", sizeof( unsigned char), 27u), _tag_arr(
_temp1413, sizeof( void*), 0u));}));} _temp1405=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp1403)->fields))->v; goto _LL1392;
_LL1392: { struct Cyc_Absyn_Structfield* _temp1414=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1405,( int) _temp988);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1415=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1415[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1416; _temp1416.tag=
Cyc_Absyn_Offsetof_e; _temp1416.f1=( void*) Cyc_Toc_typ_to_c_array( _temp990);
_temp1416.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1417=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1417[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1418; _temp1418.tag=
Cyc_Absyn_StructField; _temp1418.f1= _temp1414->name; _temp1418;}); _temp1417;}));
_temp1416;}); _temp1415;}))); goto _LL1384;} _LL1394:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1419=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1419[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1420; _temp1420.tag= Cyc_Absyn_Offsetof_e;
_temp1420.f1=( void*) Cyc_Toc_typ_to_c_array( _temp990); _temp1420.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1421=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1421[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1422; _temp1422.tag= Cyc_Absyn_StructField;
_temp1422.f1= Cyc_Absyn_fieldname(( int)( _temp988 +  1)); _temp1422;});
_temp1421;})); _temp1420;}); _temp1419;}))); goto _LL1384; _LL1396: if( _temp988
==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1423=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1423[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1424; _temp1424.tag=
Cyc_Absyn_Offsetof_e; _temp1424.f1=( void*) Cyc_Toc_typ_to_c_array( _temp990);
_temp1424.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1425=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1425[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1426; _temp1426.tag=
Cyc_Absyn_StructField; _temp1426.f1= Cyc_Toc_tag_sp; _temp1426;}); _temp1425;}));
_temp1424;}); _temp1423;})));} else{( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1427=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1427[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1428; _temp1428.tag= Cyc_Absyn_Offsetof_e;
_temp1428.f1=( void*) Cyc_Toc_typ_to_c_array( _temp990); _temp1428.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1429=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1429[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1430; _temp1430.tag= Cyc_Absyn_StructField;
_temp1430.f1= Cyc_Absyn_fieldname(( int) _temp988); _temp1430;}); _temp1429;}));
_temp1428;}); _temp1427;})));} goto _LL1384; _LL1398:( int) _throw(({ void*
_temp1431[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("impossible type for offsetof tuple index",
sizeof( unsigned char), 41u), _tag_arr( _temp1431, sizeof( void*), 0u));}));
_LL1384:;} goto _LL817; _LL865: { void* _temp1432= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp992->topt))->v);{ void* _temp1433=
_temp1432; struct Cyc_Absyn_PtrInfo _temp1439; struct Cyc_Absyn_Conref*
_temp1441; struct Cyc_Absyn_Tqual _temp1443; struct Cyc_Absyn_Conref* _temp1445;
void* _temp1447; void* _temp1449; _LL1435: if(( unsigned int) _temp1433 >  3u?*((
int*) _temp1433) ==  Cyc_Absyn_PointerType: 0){ _LL1440: _temp1439=(( struct Cyc_Absyn_PointerType_struct*)
_temp1433)->f1; _LL1450: _temp1449=( void*) _temp1439.elt_typ; goto _LL1448;
_LL1448: _temp1447=( void*) _temp1439.rgn_typ; goto _LL1446; _LL1446: _temp1445=
_temp1439.nullable; goto _LL1444; _LL1444: _temp1443= _temp1439.tq; goto _LL1442;
_LL1442: _temp1441= _temp1439.bounds; goto _LL1436;} else{ goto _LL1437;}
_LL1437: goto _LL1438; _LL1436:{ void* _temp1451= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1441); _LL1453: if(( unsigned int) _temp1451 >  1u?*(( int*) _temp1451) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1454;} else{ goto _LL1455;} _LL1455: if(
_temp1451 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1456;} else{ goto _LL1452;}
_LL1454: { int do_null_check= Cyc_Toc_need_null_check( _temp992); Cyc_Toc_exp_to_c(
nv, _temp992); if( do_null_check){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp992->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1457=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1457->hd=( void*) _temp992; _temp1457->tl= 0;
_temp1457;}), 0), 0), 0))->r));} goto _LL1452;} _LL1456: { struct Cyc_Absyn_Exp*
_temp1458= Cyc_Absyn_uint_exp( 0, 0); _temp1458->topt=({ struct Cyc_Core_Opt*
_temp1459=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1459->v=( void*) Cyc_Absyn_uint_t; _temp1459;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp992, _temp1458, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1452;} _LL1452:;} goto _LL1434; _LL1438:({ void* _temp1460[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1460, sizeof( void*), 0u));}); goto _LL1434; _LL1434:;} goto _LL817;}
_LL867: Cyc_Toc_exp_to_c( nv, _temp996); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL817; _LL869: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1000->topt))->v); int do_null_check= Cyc_Toc_need_null_check(
_temp1000); Cyc_Toc_exp_to_c( nv, _temp1000);{ int is_poly= Cyc_Toc_is_poly_project(
e); struct Cyc_Absyn_Conref* _temp1463; struct Cyc_Absyn_Tqual _temp1465; struct
Cyc_Absyn_Conref* _temp1467; void* _temp1469; void* _temp1471; struct Cyc_Absyn_PtrInfo
_temp1461= Cyc_Toc_get_ptr_type( e1typ); _LL1472: _temp1471=( void*) _temp1461.elt_typ;
goto _LL1470; _LL1470: _temp1469=( void*) _temp1461.rgn_typ; goto _LL1468;
_LL1468: _temp1467= _temp1461.nullable; goto _LL1466; _LL1466: _temp1465=
_temp1461.tq; goto _LL1464; _LL1464: _temp1463= _temp1461.bounds; goto _LL1462;
_LL1462:{ void* _temp1473= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1463);
struct Cyc_Absyn_Exp* _temp1479; _LL1475: if(( unsigned int) _temp1473 >  1u?*((
int*) _temp1473) ==  Cyc_Absyn_Upper_b: 0){ _LL1480: _temp1479=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1473)->f1; goto _LL1476;} else{ goto _LL1477;} _LL1477: if( _temp1473 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1478;} else{ goto _LL1474;} _LL1476: if(
Cyc_Evexp_eval_const_uint_exp( _temp1479) <  1){({ void* _temp1481[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1481, sizeof( void*), 0u));});} if( do_null_check){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1000->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1482=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1482->hd=( void*) _temp1000;
_temp1482->tl= 0; _temp1482;}), 0), 0), _temp998, 0))->r));} goto _LL1474;
_LL1478: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1471);( void*)( _temp1000->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1465), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1483[ 3u];
_temp1483[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1483[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1483[ 0u]= Cyc_Absyn_copy_exp( _temp1000);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1483, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1474;} _LL1474:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL817;}} _LL871: { void* _temp1484= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1004->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1004); Cyc_Toc_exp_to_c( nv, _temp1002);{ void* _temp1485= _temp1484;
struct Cyc_List_List* _temp1493; struct Cyc_Absyn_PtrInfo _temp1495; struct Cyc_Absyn_Conref*
_temp1497; struct Cyc_Absyn_Tqual _temp1499; struct Cyc_Absyn_Conref* _temp1501;
void* _temp1503; void* _temp1505; _LL1487: if(( unsigned int) _temp1485 >  3u?*((
int*) _temp1485) ==  Cyc_Absyn_TupleType: 0){ _LL1494: _temp1493=(( struct Cyc_Absyn_TupleType_struct*)
_temp1485)->f1; goto _LL1488;} else{ goto _LL1489;} _LL1489: if(( unsigned int)
_temp1485 >  3u?*(( int*) _temp1485) ==  Cyc_Absyn_PointerType: 0){ _LL1496:
_temp1495=(( struct Cyc_Absyn_PointerType_struct*) _temp1485)->f1; _LL1506:
_temp1505=( void*) _temp1495.elt_typ; goto _LL1504; _LL1504: _temp1503=( void*)
_temp1495.rgn_typ; goto _LL1502; _LL1502: _temp1501= _temp1495.nullable; goto
_LL1500; _LL1500: _temp1499= _temp1495.tq; goto _LL1498; _LL1498: _temp1497=
_temp1495.bounds; goto _LL1490;} else{ goto _LL1491;} _LL1491: goto _LL1492;
_LL1488: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1002) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1004, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1486;} _LL1490:{ void* _temp1507= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1497); struct Cyc_Absyn_Exp* _temp1513; _LL1509: if((
unsigned int) _temp1507 >  1u?*(( int*) _temp1507) ==  Cyc_Absyn_Upper_b: 0){
_LL1514: _temp1513=(( struct Cyc_Absyn_Upper_b_struct*) _temp1507)->f1; goto
_LL1510;} else{ goto _LL1511;} _LL1511: if( _temp1507 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1512;} else{ goto _LL1508;} _LL1510: _temp1513= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1513), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1501); void* ta1= Cyc_Toc_typ_to_c( _temp1505);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1499); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1515[ 4u];
_temp1515[ 3u]= _temp1002; _temp1515[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1515[ 1u]= _temp1513; _temp1515[ 0u]= _temp1004;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1515, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp1002->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1516[ 2u];
_temp1516[ 1u]= Cyc_Absyn_copy_exp( _temp1002); _temp1516[ 0u]= _temp1513;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1516, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1508;}
_LL1512: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1505);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1499), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1517[ 3u]; _temp1517[ 2u]= _temp1002; _temp1517[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1517[ 0u]= _temp1004;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1517, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1508;} _LL1508:;} goto _LL1486; _LL1492:({
void* _temp1518[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1518, sizeof( void*), 0u));}); goto
_LL1486; _LL1486:;} goto _LL817;} _LL873: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1006))->r));} else{ struct
Cyc_List_List* _temp1519=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp1006); struct _tagged_arr* _temp1520= Cyc_Toc_add_tuple_type( _temp1519);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1006 !=  0;( _temp1006=
_temp1006->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _temp1006->hd);
dles=({ struct Cyc_List_List* _temp1521=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1521->hd=( void*)({ struct _tuple4*
_temp1522=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1522->f1=
0; _temp1522->f2=( struct Cyc_Absyn_Exp*) _temp1006->hd; _temp1522;}); _temp1521->tl=
dles; _temp1521;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));} goto _LL817; _LL875:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1008, 0))->r));{ struct Cyc_List_List* _temp1523= _temp1008; for( 0;
_temp1523 !=  0; _temp1523= _temp1523->tl){ struct _tuple4 _temp1526; struct Cyc_Absyn_Exp*
_temp1527; struct _tuple4* _temp1524=( struct _tuple4*) _temp1523->hd; _temp1526=*
_temp1524; _LL1528: _temp1527= _temp1526.f2; goto _LL1525; _LL1525: Cyc_Toc_exp_to_c(
nv, _temp1527);}} goto _LL817; _LL877: { unsigned int _temp1529= Cyc_Evexp_eval_const_uint_exp(
_temp1012); Cyc_Toc_exp_to_c( nv, _temp1010);{ struct Cyc_List_List* es= 0; if(
! Cyc_Toc_is_zero( _temp1010)){ unsigned int i= 0; for( 0; i <  _temp1529; i ++){
es=({ struct Cyc_List_List* _temp1530=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1530->hd=( void*)({ struct _tuple4*
_temp1531=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1531->f1=
0; _temp1531->f2= _temp1010; _temp1531;}); _temp1530->tl= es; _temp1530;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL817;}} _LL879: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, 0, _temp1018, _temp1022))->r));} else{ if( _temp1016 ==  0){({
void* _temp1532[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer", sizeof(
unsigned char), 37u), _tag_arr( _temp1532, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1016); struct
_RegionHandle _temp1533= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1533; _push_region( rgn);{ struct Cyc_List_List* _temp1534=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1018,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); struct Cyc_List_List* _temp1535= 0;{ struct Cyc_List_List*
_temp1536=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1536 !=  0; _temp1536= _temp1536->tl){ struct Cyc_List_List*
_temp1537= _temp1534; for( 0; _temp1537 !=  0; _temp1537= _temp1537->tl){ if((*((
struct _tuple10*) _temp1537->hd)).f1 == ( struct Cyc_Absyn_Structfield*)
_temp1536->hd){ struct _tuple10 _temp1540; struct Cyc_Absyn_Exp* _temp1541;
struct Cyc_Absyn_Structfield* _temp1543; struct _tuple10* _temp1538=( struct
_tuple10*) _temp1537->hd; _temp1540=* _temp1538; _LL1544: _temp1543= _temp1540.f1;
goto _LL1542; _LL1542: _temp1541= _temp1540.f2; goto _LL1539; _LL1539: { void*
_temp1545=( void*) _temp1543->type; Cyc_Toc_exp_to_c( nv, _temp1541); if( Cyc_Toc_is_void_star(
_temp1545)){( void*)( _temp1541->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1541->r, 0), 0))->r));} _temp1535=({ struct Cyc_List_List*
_temp1546=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1546->hd=( void*)({ struct _tuple4* _temp1547=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1547->f1= 0; _temp1547->f2= _temp1541; _temp1547;});
_temp1546->tl= _temp1535; _temp1546;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1535), 0))->r));}; _pop_region( rgn);}} goto _LL817;
_LL881: { struct Cyc_List_List* fs;{ void* _temp1548= Cyc_Tcutil_compress(
_temp1026); struct Cyc_List_List* _temp1554; _LL1550: if(( unsigned int)
_temp1548 >  3u?*(( int*) _temp1548) ==  Cyc_Absyn_AnonStructType: 0){ _LL1555:
_temp1554=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1548)->f1; goto
_LL1551;} else{ goto _LL1552;} _LL1552: goto _LL1553; _LL1551: fs= _temp1554;
goto _LL1549; _LL1553: fs=({ struct Cyc_Std_String_pa_struct _temp1557;
_temp1557.tag= Cyc_Std_String_pa; _temp1557.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1026);{ void* _temp1556[ 1u]={& _temp1557};(( struct Cyc_List_List*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("anon struct has type %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1556, sizeof( void*), 1u));}});
goto _LL1549; _LL1549:;}{ struct _RegionHandle _temp1558= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1558; _push_region( rgn);{ struct Cyc_List_List*
_temp1559=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1024, fs); for( 0; _temp1559 !=  0; _temp1559= _temp1559->tl){
struct _tuple10 _temp1562; struct Cyc_Absyn_Exp* _temp1563; struct Cyc_Absyn_Structfield*
_temp1565; struct _tuple10* _temp1560=( struct _tuple10*) _temp1559->hd;
_temp1562=* _temp1560; _LL1566: _temp1565= _temp1562.f1; goto _LL1564; _LL1564:
_temp1563= _temp1562.f2; goto _LL1561; _LL1561: { void* _temp1567=( void*)
_temp1565->type; Cyc_Toc_exp_to_c( nv, _temp1563); if( Cyc_Toc_is_void_star(
_temp1567)){( void*)( _temp1563->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1563->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1024, 0))->r));}; _pop_region( rgn);}
goto _LL817;} _LL883: { struct _tuple0* qv= _temp1028->name; if( _temp1030->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1568= _temp1030->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1030->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1568))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1568->hd)->typs ==  0){ tag_count ++;} _temp1568= _temp1568->tl;}( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL817;} _LL885: { struct _tuple0* _temp1569= _temp1034->name; struct Cyc_List_List*
_temp1570= _temp1034->typs; struct _tuple0* _temp1571= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1572= Cyc_Absyn_var_exp( _temp1571, 0); void*
_temp1573= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1569, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1036->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1569, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1574= _temp1036->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1036->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1569,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1574))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1574->hd)->typs !=  0){ tag_count ++;} _temp1574= _temp1574->tl;} tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles=
0; for( 0; _temp1038 !=  0;( _temp1038= _temp1038->tl, _temp1570= _temp1570->tl)){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1038->hd; void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1570))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1575=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1575->hd=( void*)({ struct _tuple4* _temp1576=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1576->f1= 0; _temp1576->f2= cur_e; _temp1576;});
_temp1575->tl= dles; _temp1575;});} dles=({ struct Cyc_List_List* _temp1577=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1577->hd=(
void*)({ struct _tuple4* _temp1578=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1578->f1= 0; _temp1578->f2= tag_exp; _temp1578;}); _temp1577->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1577;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1579=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1579->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1569,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1579;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1580=({ struct Cyc_List_List* _temp1586=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1586->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1572, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1569, 0), 0); _temp1586->tl= 0; _temp1586;});{ int i=
1; for( 0; _temp1038 !=  0;((( _temp1038= _temp1038->tl, i ++)), _temp1570=
_temp1570->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1038->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1570))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1581= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1572, Cyc_Absyn_fieldname(
i), 0), cur_e, 0); _temp1580=({ struct Cyc_List_List* _temp1582=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1582->hd=( void*) _temp1581;
_temp1582->tl= _temp1580; _temp1582;});}}}{ struct Cyc_Absyn_Stmt* _temp1583=
Cyc_Absyn_exp_stmt( _temp1572, 0); struct Cyc_Absyn_Stmt* _temp1584= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1585=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1585->hd=( void*) _temp1583; _temp1585->tl= _temp1580; _temp1585;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1571, _temp1573, 0, _temp1584, 0), 0))->r));}} goto _LL817;} _LL887: goto
_LL817; _LL889: goto _LL817; _LL891: { void* t_c= Cyc_Toc_typ_to_c( _temp1040);
if( _temp1042 !=  0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)
_check_null( _temp1042); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1040, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL817;} _LL893: Cyc_Toc_stmt_to_c( nv, _temp1044); goto
_LL817; _LL895:({ void* _temp1587[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem", sizeof(
unsigned char), 14u), _tag_arr( _temp1587, sizeof( void*), 0u));}); goto _LL817;
_LL897:({ void* _temp1588[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof( unsigned char),
12u), _tag_arr( _temp1588, sizeof( void*), 0u));}); goto _LL817; _LL899:({ void*
_temp1589[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("codegen", sizeof( unsigned char), 8u), _tag_arr( _temp1589, sizeof(
void*), 0u));}); goto _LL817; _LL901:({ void* _temp1590[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u), _tag_arr( _temp1590, sizeof( void*), 0u));}); goto
_LL817; _LL817:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1591=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1629; struct Cyc_Absyn_Vardecl _temp1631; struct _tuple0* _temp1632; struct
Cyc_Absyn_Vardecl* _temp1634; int _temp1636; void* _temp1638; unsigned char
_temp1640; struct _tagged_arr _temp1642; struct Cyc_Absyn_Enumfield* _temp1644;
struct Cyc_Absyn_Enumdecl* _temp1646; struct Cyc_Absyn_Enumfield* _temp1648;
void* _temp1650; struct Cyc_List_List* _temp1652; struct Cyc_List_List*
_temp1654; struct Cyc_Absyn_Tunionfield* _temp1656; struct Cyc_Absyn_Tuniondecl*
_temp1658; struct Cyc_Absyn_Pat* _temp1661; struct Cyc_Absyn_Pat _temp1663; void*
_temp1664; struct Cyc_List_List* _temp1666; struct Cyc_List_List* _temp1668;
struct Cyc_Absyn_Tunionfield* _temp1670; struct Cyc_Absyn_Tuniondecl* _temp1672;
struct Cyc_List_List* _temp1674; struct Cyc_List_List* _temp1676; struct Cyc_List_List*
_temp1678; struct Cyc_List_List* _temp1680; struct Cyc_Core_Opt* _temp1682;
struct Cyc_Absyn_Structdecl* _temp1684; struct Cyc_Absyn_Pat* _temp1686; _LL1593:
if(( unsigned int) _temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_Var_p: 0){
_LL1630: _temp1629=(( struct Cyc_Absyn_Var_p_struct*) _temp1591)->f1; _temp1631=*
_temp1629; _LL1633: _temp1632= _temp1631.name; goto _LL1594;} else{ goto _LL1595;}
_LL1595: if( _temp1591 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1596;} else{ goto
_LL1597;} _LL1597: if(( unsigned int) _temp1591 >  2u?*(( int*) _temp1591) == 
Cyc_Absyn_Reference_p: 0){ _LL1635: _temp1634=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1591)->f1; goto _LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1591 == (
void*) Cyc_Absyn_Null_p){ goto _LL1600;} else{ goto _LL1601;} _LL1601: if((
unsigned int) _temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_Int_p: 0){
_LL1639: _temp1638=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1591)->f1;
goto _LL1637; _LL1637: _temp1636=(( struct Cyc_Absyn_Int_p_struct*) _temp1591)->f2;
goto _LL1602;} else{ goto _LL1603;} _LL1603: if(( unsigned int) _temp1591 >  2u?*((
int*) _temp1591) ==  Cyc_Absyn_Char_p: 0){ _LL1641: _temp1640=(( struct Cyc_Absyn_Char_p_struct*)
_temp1591)->f1; goto _LL1604;} else{ goto _LL1605;} _LL1605: if(( unsigned int)
_temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_Float_p: 0){ _LL1643:
_temp1642=(( struct Cyc_Absyn_Float_p_struct*) _temp1591)->f1; goto _LL1606;}
else{ goto _LL1607;} _LL1607: if(( unsigned int) _temp1591 >  2u?*(( int*)
_temp1591) ==  Cyc_Absyn_Enum_p: 0){ _LL1647: _temp1646=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1591)->f1; goto _LL1645; _LL1645: _temp1644=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1591)->f2; goto _LL1608;} else{ goto _LL1609;} _LL1609: if(( unsigned int)
_temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1651:
_temp1650=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1591)->f1; goto
_LL1649; _LL1649: _temp1648=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1591)->f2;
goto _LL1610;} else{ goto _LL1611;} _LL1611: if(( unsigned int) _temp1591 >  2u?*((
int*) _temp1591) ==  Cyc_Absyn_Tunion_p: 0){ _LL1659: _temp1658=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1591)->f1; goto _LL1657; _LL1657: _temp1656=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1591)->f2; goto _LL1655; _LL1655: _temp1654=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1591)->f3; goto _LL1653; _LL1653: _temp1652=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1591)->f4; if( _temp1652 ==  0){ goto _LL1612;} else{ goto _LL1613;}} else{
goto _LL1613;} _LL1613: if(( unsigned int) _temp1591 >  2u?*(( int*) _temp1591)
==  Cyc_Absyn_Pointer_p: 0){ _LL1662: _temp1661=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1591)->f1; _temp1663=* _temp1661; _LL1665: _temp1664=( void*) _temp1663.r;
if(( unsigned int) _temp1664 >  2u?*(( int*) _temp1664) ==  Cyc_Absyn_Tunion_p:
0){ _LL1673: _temp1672=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1664)->f1;
goto _LL1671; _LL1671: _temp1670=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1664)->f2;
goto _LL1669; _LL1669: _temp1668=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1664)->f3;
goto _LL1667; _LL1667: _temp1666=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1664)->f4;
goto _LL1660;} else{ goto _LL1615;}} else{ goto _LL1615;} _LL1660: if( _temp1666
!=  0){ goto _LL1614;} else{ goto _LL1615;} _LL1615: if(( unsigned int)
_temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_Tunion_p: 0){ _LL1675:
_temp1674=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1591)->f4; goto _LL1616;}
else{ goto _LL1617;} _LL1617: if(( unsigned int) _temp1591 >  2u?*(( int*)
_temp1591) ==  Cyc_Absyn_Tuple_p: 0){ _LL1677: _temp1676=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1591)->f1; goto _LL1618;} else{ goto _LL1619;} _LL1619: if(( unsigned int)
_temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_Struct_p: 0){ _LL1685:
_temp1684=(( struct Cyc_Absyn_Struct_p_struct*) _temp1591)->f1; goto _LL1683;
_LL1683: _temp1682=(( struct Cyc_Absyn_Struct_p_struct*) _temp1591)->f2; goto
_LL1681; _LL1681: _temp1680=(( struct Cyc_Absyn_Struct_p_struct*) _temp1591)->f3;
goto _LL1679; _LL1679: _temp1678=(( struct Cyc_Absyn_Struct_p_struct*) _temp1591)->f4;
goto _LL1620;} else{ goto _LL1621;} _LL1621: if(( unsigned int) _temp1591 >  2u?*((
int*) _temp1591) ==  Cyc_Absyn_Pointer_p: 0){ _LL1687: _temp1686=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1591)->f1; goto _LL1622;} else{ goto _LL1623;} _LL1623: if(( unsigned int)
_temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1624;}
else{ goto _LL1625;} _LL1625: if(( unsigned int) _temp1591 >  2u?*(( int*)
_temp1591) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1626;} else{ goto _LL1627;}
_LL1627: if(( unsigned int) _temp1591 >  2u?*(( int*) _temp1591) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1628;} else{ goto _LL1592;} _LL1594: nv= Cyc_Toc_add_varmap( nv,
_temp1632, r); goto _LL1596; _LL1596: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1592; _LL1598: { struct _tuple0* _temp1688= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1689=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1689->hd=( void*)({ struct _tuple12*
_temp1690=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1690->f1= _temp1688; _temp1690->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1690;}); _temp1689->tl= decls; _temp1689;}); nv= Cyc_Toc_add_varmap(
nv, _temp1634->name, Cyc_Absyn_var_exp( _temp1688, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1688, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1592;} _LL1600: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1592; _LL1602:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1638, _temp1636, 0),
succ_lab, fail_lab); goto _LL1592; _LL1604: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1640, 0), succ_lab, fail_lab); goto _LL1592; _LL1606: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1642, 0), succ_lab, fail_lab); goto _LL1592;
_LL1608: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1691=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1691[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1692; _temp1692.tag= Cyc_Absyn_Enum_e;
_temp1692.f1= _temp1644->name; _temp1692.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1646; _temp1692.f3=( struct Cyc_Absyn_Enumfield*) _temp1644; _temp1692;});
_temp1691;}), 0), succ_lab, fail_lab); goto _LL1592; _LL1610: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_AnonEnum_e_struct* _temp1693=(
struct Cyc_Absyn_AnonEnum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_e_struct));
_temp1693[ 0]=({ struct Cyc_Absyn_AnonEnum_e_struct _temp1694; _temp1694.tag=
Cyc_Absyn_AnonEnum_e; _temp1694.f1= _temp1648->name; _temp1694.f2=( void*)
_temp1650; _temp1694.f3=( struct Cyc_Absyn_Enumfield*) _temp1648; _temp1694;});
_temp1693;}), 0), succ_lab, fail_lab); goto _LL1592; _LL1612: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1656->name, 0); if( ! _temp1658->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1592;} _LL1614: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1666);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1670->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1695=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1670->typs); struct Cyc_List_List*
_temp1696=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1666); for( 0; _temp1696 !=  0;((( _temp1696=
_temp1696->tl, _temp1695=(( struct Cyc_List_List*) _check_null( _temp1695))->tl)),
cnt --)){ struct Cyc_Absyn_Pat* _temp1697=( struct Cyc_Absyn_Pat*) _temp1696->hd;
if(( void*) _temp1697->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1698=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1695))->hd)).f2;
struct _tuple0* _temp1699= Cyc_Toc_temp_var(); void* _temp1700=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1697->topt))->v; void* _temp1701= Cyc_Toc_typ_to_c(
_temp1700); struct _tagged_arr* _temp1702= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1703= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1698))){ _temp1703= Cyc_Absyn_cast_exp(
_temp1701, _temp1703, 0);} decls=({ struct Cyc_List_List* _temp1704=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1704->hd=( void*)({
struct _tuple12* _temp1705=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1705->f1= _temp1699; _temp1705->f2= _temp1701; _temp1705;});
_temp1704->tl= decls; _temp1704;});{ struct _tuple11 _temp1706= Cyc_Toc_xlate_pat(
nv, rgn, _temp1700, Cyc_Absyn_var_exp( _temp1699, 0), _temp1703, _temp1697,
succ_lab, fail_lab, decls); nv= _temp1706.f1; decls= _temp1706.f2;{ struct Cyc_Absyn_Stmt*
_temp1707= _temp1706.f3; struct Cyc_Absyn_Stmt* _temp1708= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1699, 0), _temp1703, 0); struct Cyc_Absyn_Stmt*
_temp1709= Cyc_Absyn_seq_stmt( _temp1708, _temp1707, 0); ss=({ struct Cyc_List_List*
_temp1710=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1710->hd=( void*) Cyc_Absyn_label_stmt( _temp1702, _temp1709, 0); _temp1710->tl=
ss; _temp1710;}); succ_lab= _temp1702;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1711=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1711->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1711->tl= 0;
_temp1711;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1672->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1670->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1712=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1672->fields))->v; for( 0; _temp1712 !=  0; _temp1712=
_temp1712->tl){ struct Cyc_Absyn_Tunionfield* _temp1713=( struct Cyc_Absyn_Tunionfield*)
_temp1712->hd; if( _temp1713->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1670->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1592;}} _LL1616: _temp1676= _temp1674; goto _LL1618; _LL1618: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1676);{ struct Cyc_List_List* _temp1714=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1676); for(
0; _temp1714 !=  0;( _temp1714= _temp1714->tl, cnt --)){ struct Cyc_Absyn_Pat*
_temp1715=( struct Cyc_Absyn_Pat*) _temp1714->hd; if(( void*) _temp1715->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tuple0* _temp1716= Cyc_Toc_temp_var();
void* _temp1717=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1715->topt))->v;
struct _tagged_arr* _temp1718= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1719=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1719->hd=( void*)({ struct _tuple12* _temp1720=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1720->f1= _temp1716;
_temp1720->f2= Cyc_Toc_typ_to_c( _temp1717); _temp1720;}); _temp1719->tl= decls;
_temp1719;});{ struct _tuple11 _temp1721= Cyc_Toc_xlate_pat( nv, rgn, _temp1717,
Cyc_Absyn_var_exp( _temp1716, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1715, succ_lab, fail_lab, decls); nv= _temp1721.f1; decls=
_temp1721.f2;{ struct Cyc_Absyn_Stmt* _temp1722= _temp1721.f3; struct Cyc_Absyn_Stmt*
_temp1723= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1716, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp1722, 0); ss=({ struct Cyc_List_List* _temp1724=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1724->hd=( void*) Cyc_Absyn_label_stmt(
_temp1718, _temp1723, 0); _temp1724->tl= ss; _temp1724;}); succ_lab= _temp1718;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1725=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1725->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1725->tl= 0; _temp1725;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1592;} _LL1620: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1726=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1678); for( 0; _temp1726 !=  0; _temp1726=
_temp1726->tl){ struct _tuple13* _temp1727=( struct _tuple13*) _temp1726->hd;
struct Cyc_Absyn_Pat* _temp1728=(* _temp1727).f2; if(( void*) _temp1728->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1729=(
void*)(( struct Cyc_List_List*) _check_null((* _temp1727).f1))->hd; struct
_tagged_arr* _temp1735; _LL1731: if(*(( int*) _temp1729) ==  Cyc_Absyn_FieldName){
_LL1736: _temp1735=(( struct Cyc_Absyn_FieldName_struct*) _temp1729)->f1; goto
_LL1732;} else{ goto _LL1733;} _LL1733: goto _LL1734; _LL1732: f= _temp1735;
goto _LL1730; _LL1734:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1730:;}{
struct _tuple0* _temp1737= Cyc_Toc_temp_var(); void* _temp1738=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1728->topt))->v; void* _temp1739= Cyc_Toc_typ_to_c(
_temp1738); struct _tagged_arr* _temp1740= Cyc_Toc_fresh_label(); decls=({
struct Cyc_List_List* _temp1741=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1741->hd=( void*)({ struct _tuple12*
_temp1742=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1742->f1= _temp1737; _temp1742->f2= _temp1739; _temp1742;}); _temp1741->tl=
decls; _temp1741;});{ struct _tuple11 _temp1743= Cyc_Toc_xlate_pat( nv, rgn,
_temp1738, Cyc_Absyn_var_exp( _temp1737, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp1728, succ_lab, fail_lab, decls); nv= _temp1743.f1; decls= _temp1743.f2;{
struct Cyc_Absyn_Exp* _temp1744= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1684->fields))->v,
f)))->type)){ _temp1744= Cyc_Absyn_cast_exp( _temp1739, _temp1744, 0);}{ struct
Cyc_Absyn_Stmt* _temp1745= _temp1743.f3; struct Cyc_Absyn_Stmt* _temp1746= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1737, 0), _temp1744, 0),
_temp1745, 0); ss=({ struct Cyc_List_List* _temp1747=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1747->hd=( void*) Cyc_Absyn_label_stmt(
_temp1740, _temp1746, 0); _temp1747->tl= ss; _temp1747;}); succ_lab= _temp1740;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1748=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1748->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1748->tl= 0; _temp1748;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1592;} _LL1622: { struct _tuple0* _temp1749= Cyc_Toc_temp_var(); void*
_temp1750=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1686->topt))->v;
decls=({ struct Cyc_List_List* _temp1751=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1751->hd=( void*)({ struct _tuple12*
_temp1752=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1752->f1= _temp1749; _temp1752->f2= Cyc_Toc_typ_to_c( _temp1750); _temp1752;});
_temp1751->tl= decls; _temp1751;});{ struct _tuple11 _temp1753= Cyc_Toc_xlate_pat(
nv, rgn, _temp1750, Cyc_Absyn_var_exp( _temp1749, 0), Cyc_Absyn_deref_exp( path,
0), _temp1686, succ_lab, fail_lab, decls); nv= _temp1753.f1; decls= _temp1753.f2;{
struct Cyc_Absyn_Stmt* _temp1754= _temp1753.f3; struct Cyc_Absyn_Stmt* _temp1755=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1749, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1754, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1755, 0);} else{ s= _temp1755;} goto _LL1592;}}} _LL1624: s=({
void* _temp1756[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1756, sizeof( void*), 0u));}); goto _LL1592;
_LL1626: s=({ void* _temp1757[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1757, sizeof( void*), 0u));}); goto
_LL1592; _LL1628: s=({ void* _temp1758[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1758, sizeof( void*), 0u));}); goto
_LL1592; _LL1592:;} return({ struct _tuple11 _temp1759; _temp1759.f1= nv;
_temp1759.f2= decls; _temp1759.f3= s; _temp1759;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1760=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1760->f1= Cyc_Toc_fresh_label(); _temp1760->f2=
Cyc_Toc_fresh_label(); _temp1760->f3= sc; _temp1760;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1761=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1762= Cyc_Tcutil_compress( _temp1761); _LL1764: if(( unsigned int)
_temp1762 >  3u?*(( int*) _temp1762) ==  Cyc_Absyn_IntType: 0){ goto _LL1765;}
else{ goto _LL1766;} _LL1766: if(( unsigned int) _temp1762 >  3u?*(( int*)
_temp1762) ==  Cyc_Absyn_EnumType: 0){ goto _LL1767;} else{ goto _LL1768;}
_LL1768: goto _LL1769; _LL1765: goto _LL1767; _LL1767: leave_as_switch= 1; goto
_LL1763; _LL1769: leave_as_switch= 0; goto _LL1763; _LL1763:;}{ struct Cyc_List_List*
_temp1770= scs; for( 0; _temp1770 !=  0; _temp1770= _temp1770->tl){ if(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)
_temp1770->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)
_temp1770->hd)->where_clause !=  0){ leave_as_switch= 0; break;}}} if(
leave_as_switch){ struct _tagged_arr* next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List*
_temp1771= scs; for( 0; _temp1771 !=  0; _temp1771= _temp1771->tl){ struct Cyc_Absyn_Stmt*
_temp1772=(( struct Cyc_Absyn_Switch_clause*) _temp1771->hd)->body;(( struct Cyc_Absyn_Switch_clause*)
_temp1771->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp1772, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp1772);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct _tagged_arr*
end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp1773= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1773; _push_region( rgn);{ struct Cyc_List_List*
lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple14*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*), struct _RegionHandle*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label, rgn,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1774= lscs; for( 0;
_temp1774 !=  0; _temp1774= _temp1774->tl){ struct Cyc_Absyn_Switch_clause* sc=(*((
struct _tuple14*) _temp1774->hd)).f3; struct _tagged_arr* fail_lab= _temp1774->tl
==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(
_temp1774->tl))->hd)).f1; struct _tagged_arr* succ_lab=(*(( struct _tuple14*)
_temp1774->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11 _temp1775= Cyc_Toc_xlate_pat(
nv, rgn, _temp1761, r, path, sc->pattern, succ_lab, fail_lab, decls); nvs=({
struct Cyc_List_List* _temp1776=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1776->hd=( void*) _temp1775.f1; _temp1776->tl=
nvs; _temp1776;}); decls= _temp1775.f2; test_stmts=({ struct Cyc_List_List*
_temp1777=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1777->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple14*) _temp1774->hd)).f1,
_temp1775.f3, 0); _temp1777->tl= test_stmts; _temp1777;});} else{ struct Cyc_Absyn_Exp*
_temp1778=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1779= Cyc_Toc_fresh_label(); struct _tuple11 _temp1780= Cyc_Toc_xlate_pat(
nv, rgn, _temp1761, r, path, sc->pattern, _temp1779, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1780.f1, _temp1778); nvs=({ struct Cyc_List_List* _temp1781=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1781->hd=( void*)
_temp1780.f1; _temp1781->tl= nvs; _temp1781;}); decls= _temp1780.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1778, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1779, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1782=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1782->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*) _temp1774->hd)).f1, Cyc_Absyn_seq_stmt( _temp1780.f3, s3, 0),
0); _temp1782->tl= test_stmts; _temp1782;});}}}}{ struct Cyc_Absyn_Stmt*
_temp1783= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs !=  0){ for( 0; lscs->tl !=  0;( lscs= lscs->tl, nvs= nvs->tl)){
struct Cyc_Toc_Env* _temp1784=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1784, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f2,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f3)->pat_vars))->v,( struct
Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null( nvs->tl))->hd), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}{ struct Cyc_Toc_Env* _temp1785=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp1785, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt( _temp1783, Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for(
decls; decls !=  0; decls= decls->tl){ struct _tuple12 _temp1788; void*
_temp1789; struct _tuple0* _temp1791; struct _tuple12* _temp1786=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp1788=*
_temp1786; _LL1792: _temp1791= _temp1788.f1; goto _LL1790; _LL1790: _temp1789=
_temp1788.f2; goto _LL1787; _LL1787: res= Cyc_Absyn_declare_stmt( _temp1791,
_temp1789, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1793=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1793[ 0]=({ struct Cyc_List_List
_temp1794; _temp1794.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1794.tl= 0; _temp1794;}); _temp1793;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1795=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1837; struct Cyc_Absyn_Stmt* _temp1839; struct Cyc_Absyn_Stmt* _temp1841;
struct Cyc_Absyn_Exp* _temp1843; struct Cyc_Absyn_Stmt* _temp1845; struct Cyc_Absyn_Stmt*
_temp1847; struct Cyc_Absyn_Exp* _temp1849; struct Cyc_Absyn_Stmt* _temp1851;
struct _tuple2 _temp1853; struct Cyc_Absyn_Exp* _temp1855; struct Cyc_Absyn_Stmt*
_temp1857; struct Cyc_Absyn_Stmt* _temp1859; struct Cyc_Absyn_Stmt* _temp1861;
struct Cyc_Absyn_Stmt* _temp1863; struct _tuple2 _temp1865; struct Cyc_Absyn_Exp*
_temp1867; struct _tuple2 _temp1869; struct Cyc_Absyn_Exp* _temp1871; struct Cyc_Absyn_Exp*
_temp1873; struct Cyc_List_List* _temp1875; struct Cyc_Absyn_Exp* _temp1877;
struct Cyc_Absyn_Switch_clause** _temp1879; struct Cyc_List_List* _temp1881;
struct Cyc_Absyn_Stmt* _temp1883; struct Cyc_Absyn_Decl* _temp1885; struct Cyc_Absyn_Stmt*
_temp1887; struct _tagged_arr* _temp1889; struct _tuple2 _temp1891; struct Cyc_Absyn_Exp*
_temp1893; struct Cyc_Absyn_Stmt* _temp1895; struct Cyc_List_List* _temp1897;
struct Cyc_Absyn_Stmt* _temp1899; struct Cyc_Absyn_Stmt* _temp1901; struct Cyc_Absyn_Vardecl*
_temp1903; struct Cyc_Absyn_Tvar* _temp1905; _LL1797: if( _temp1795 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1798;} else{ goto _LL1799;} _LL1799: if((
unsigned int) _temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Exp_s: 0){
_LL1838: _temp1837=(( struct Cyc_Absyn_Exp_s_struct*) _temp1795)->f1; goto
_LL1800;} else{ goto _LL1801;} _LL1801: if(( unsigned int) _temp1795 >  1u?*((
int*) _temp1795) ==  Cyc_Absyn_Seq_s: 0){ _LL1842: _temp1841=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1795)->f1; goto _LL1840; _LL1840: _temp1839=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1795)->f2; goto _LL1802;} else{ goto _LL1803;} _LL1803: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Return_s: 0){ _LL1844:
_temp1843=(( struct Cyc_Absyn_Return_s_struct*) _temp1795)->f1; goto _LL1804;}
else{ goto _LL1805;} _LL1805: if(( unsigned int) _temp1795 >  1u?*(( int*)
_temp1795) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1850: _temp1849=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1795)->f1; goto _LL1848; _LL1848: _temp1847=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1795)->f2; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1795)->f3; goto _LL1806;} else{ goto _LL1807;} _LL1807: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_While_s: 0){ _LL1854:
_temp1853=(( struct Cyc_Absyn_While_s_struct*) _temp1795)->f1; _LL1856:
_temp1855= _temp1853.f1; goto _LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_While_s_struct*)
_temp1795)->f2; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Break_s: 0){ _LL1858:
_temp1857=(( struct Cyc_Absyn_Break_s_struct*) _temp1795)->f1; goto _LL1810;}
else{ goto _LL1811;} _LL1811: if(( unsigned int) _temp1795 >  1u?*(( int*)
_temp1795) ==  Cyc_Absyn_Continue_s: 0){ _LL1860: _temp1859=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1795)->f1; goto _LL1812;} else{ goto _LL1813;} _LL1813: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Goto_s: 0){ _LL1862:
_temp1861=(( struct Cyc_Absyn_Goto_s_struct*) _temp1795)->f2; goto _LL1814;}
else{ goto _LL1815;} _LL1815: if(( unsigned int) _temp1795 >  1u?*(( int*)
_temp1795) ==  Cyc_Absyn_For_s: 0){ _LL1874: _temp1873=(( struct Cyc_Absyn_For_s_struct*)
_temp1795)->f1; goto _LL1870; _LL1870: _temp1869=(( struct Cyc_Absyn_For_s_struct*)
_temp1795)->f2; _LL1872: _temp1871= _temp1869.f1; goto _LL1866; _LL1866:
_temp1865=(( struct Cyc_Absyn_For_s_struct*) _temp1795)->f3; _LL1868: _temp1867=
_temp1865.f1; goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_For_s_struct*)
_temp1795)->f4; goto _LL1816;} else{ goto _LL1817;} _LL1817: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Switch_s: 0){ _LL1878:
_temp1877=(( struct Cyc_Absyn_Switch_s_struct*) _temp1795)->f1; goto _LL1876;
_LL1876: _temp1875=(( struct Cyc_Absyn_Switch_s_struct*) _temp1795)->f2; goto
_LL1818;} else{ goto _LL1819;} _LL1819: if(( unsigned int) _temp1795 >  1u?*((
int*) _temp1795) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1882: _temp1881=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1795)->f1; goto _LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1795)->f2; goto _LL1820;} else{ goto _LL1821;} _LL1821: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Decl_s: 0){ _LL1886:
_temp1885=(( struct Cyc_Absyn_Decl_s_struct*) _temp1795)->f1; goto _LL1884;
_LL1884: _temp1883=(( struct Cyc_Absyn_Decl_s_struct*) _temp1795)->f2; goto
_LL1822;} else{ goto _LL1823;} _LL1823: if(( unsigned int) _temp1795 >  1u?*((
int*) _temp1795) ==  Cyc_Absyn_Label_s: 0){ _LL1890: _temp1889=(( struct Cyc_Absyn_Label_s_struct*)
_temp1795)->f1; goto _LL1888; _LL1888: _temp1887=(( struct Cyc_Absyn_Label_s_struct*)
_temp1795)->f2; goto _LL1824;} else{ goto _LL1825;} _LL1825: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Do_s: 0){ _LL1896: _temp1895=((
struct Cyc_Absyn_Do_s_struct*) _temp1795)->f1; goto _LL1892; _LL1892: _temp1891=((
struct Cyc_Absyn_Do_s_struct*) _temp1795)->f2; _LL1894: _temp1893= _temp1891.f1;
goto _LL1826;} else{ goto _LL1827;} _LL1827: if(( unsigned int) _temp1795 >  1u?*((
int*) _temp1795) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1900: _temp1899=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1795)->f1; goto _LL1898; _LL1898: _temp1897=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1795)->f2; goto _LL1828;} else{ goto _LL1829;} _LL1829: if(( unsigned int)
_temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Region_s: 0){ _LL1906:
_temp1905=(( struct Cyc_Absyn_Region_s_struct*) _temp1795)->f1; goto _LL1904;
_LL1904: _temp1903=(( struct Cyc_Absyn_Region_s_struct*) _temp1795)->f2; goto
_LL1902; _LL1902: _temp1901=(( struct Cyc_Absyn_Region_s_struct*) _temp1795)->f3;
goto _LL1830;} else{ goto _LL1831;} _LL1831: if(( unsigned int) _temp1795 >  1u?*((
int*) _temp1795) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1832;} else{ goto _LL1833;}
_LL1833: if(( unsigned int) _temp1795 >  1u?*(( int*) _temp1795) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1834;} else{ goto _LL1835;} _LL1835: if(( unsigned int) _temp1795 > 
1u?*(( int*) _temp1795) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1836;} else{ goto
_LL1796;} _LL1798: return; _LL1800: Cyc_Toc_exp_to_c( nv, _temp1837); return;
_LL1802: Cyc_Toc_stmt_to_c( nv, _temp1841); s= _temp1839; continue; _LL1804: {
struct Cyc_Core_Opt* topt= 0; if( _temp1843 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1907=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1907->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1843->topt))->v); _temp1907;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp1843));} if( s->try_depth >  0){ if( topt != 
0){ struct _tuple0* _temp1908= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1909= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp(
_temp1908, 0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp1908,( void*) topt->v, _temp1843, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1909, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1806: Cyc_Toc_exp_to_c( nv, _temp1849); Cyc_Toc_stmt_to_c( nv,
_temp1847); s= _temp1845; continue; _LL1808: Cyc_Toc_exp_to_c( nv, _temp1855);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1851); return; _LL1810: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1857 ==  0? 0: _temp1857->try_depth; Cyc_Toc_do_npop_before(
s->try_depth -  dest_depth, s); return;} _LL1812: if( nv->continue_lab !=  0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp1861=
_temp1859; goto _LL1814; _LL1814: Cyc_Toc_do_npop_before( s->try_depth - ((
struct Cyc_Absyn_Stmt*) _check_null( _temp1861))->try_depth, s); return; _LL1816:
Cyc_Toc_exp_to_c( nv, _temp1873); Cyc_Toc_exp_to_c( nv, _temp1871); Cyc_Toc_exp_to_c(
nv, _temp1867); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1863); return;
_LL1818: Cyc_Toc_xlate_switch( nv, s, _temp1877, _temp1875); return; _LL1820:
if( nv->fallthru_info ==  0){( int) _throw(({ void* _temp1910[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("fallthru in unexpected place", sizeof( unsigned char), 29u), _tag_arr(
_temp1910, sizeof( void*), 0u));}));}{ struct _tuple6 _temp1913; struct Cyc_Dict_Dict*
_temp1914; struct Cyc_List_List* _temp1916; struct _tagged_arr* _temp1918;
struct _tuple6* _temp1911=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp1913=* _temp1911; _LL1919: _temp1918= _temp1913.f1;
goto _LL1917; _LL1917: _temp1916= _temp1913.f2; goto _LL1915; _LL1915: _temp1914=
_temp1913.f3; goto _LL1912; _LL1912: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp1918, 0); Cyc_Toc_do_npop_before( s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp1879)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp1920=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1916); struct Cyc_List_List* _temp1921=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1881); for( 0; _temp1920 !=  0;(
_temp1920= _temp1920->tl, _temp1921= _temp1921->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1921))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp1914,(
struct _tuple0*) _temp1920->hd),( struct Cyc_Absyn_Exp*) _temp1921->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1822:{ void*
_temp1922=( void*) _temp1885->r; struct Cyc_Absyn_Vardecl* _temp1932; int
_temp1934; struct Cyc_Absyn_Exp* _temp1936; struct Cyc_Core_Opt* _temp1938;
struct Cyc_Absyn_Pat* _temp1940; struct Cyc_List_List* _temp1942; _LL1924: if(*((
int*) _temp1922) ==  Cyc_Absyn_Var_d){ _LL1933: _temp1932=(( struct Cyc_Absyn_Var_d_struct*)
_temp1922)->f1; goto _LL1925;} else{ goto _LL1926;} _LL1926: if(*(( int*)
_temp1922) ==  Cyc_Absyn_Let_d){ _LL1941: _temp1940=(( struct Cyc_Absyn_Let_d_struct*)
_temp1922)->f1; goto _LL1939; _LL1939: _temp1938=(( struct Cyc_Absyn_Let_d_struct*)
_temp1922)->f3; goto _LL1937; _LL1937: _temp1936=(( struct Cyc_Absyn_Let_d_struct*)
_temp1922)->f4; goto _LL1935; _LL1935: _temp1934=(( struct Cyc_Absyn_Let_d_struct*)
_temp1922)->f5; goto _LL1927;} else{ goto _LL1928;} _LL1928: if(*(( int*)
_temp1922) ==  Cyc_Absyn_Letv_d){ _LL1943: _temp1942=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1922)->f1; goto _LL1929;} else{ goto _LL1930;} _LL1930: goto _LL1931;
_LL1925: { struct Cyc_Toc_Env* _temp1944= Cyc_Toc_add_varmap( nv, _temp1932->name,
Cyc_Absyn_varb_exp( _temp1932->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1945=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1945[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1946; _temp1946.tag= Cyc_Absyn_Local_b;
_temp1946.f1= _temp1932; _temp1946;}); _temp1945;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1944, _temp1944, _temp1932, _temp1883); goto _LL1923;} _LL1927:{ void*
_temp1947=( void*) _temp1940->r; struct Cyc_Absyn_Vardecl* _temp1953; _LL1949:
if(( unsigned int) _temp1947 >  2u?*(( int*) _temp1947) ==  Cyc_Absyn_Var_p: 0){
_LL1954: _temp1953=(( struct Cyc_Absyn_Var_p_struct*) _temp1947)->f1; goto
_LL1950;} else{ goto _LL1951;} _LL1951: goto _LL1952; _LL1950: { struct _tuple0*
old_name= _temp1953->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1953->name= new_name; _temp1953->initializer=( struct Cyc_Absyn_Exp*)
_temp1936;( void*)( _temp1885->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1955=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1955[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1956; _temp1956.tag= Cyc_Absyn_Var_d;
_temp1956.f1= _temp1953; _temp1956;}); _temp1955;})));{ struct Cyc_Toc_Env*
_temp1957= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1958=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1958[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1959; _temp1959.tag= Cyc_Absyn_Local_b; _temp1959.f1=
_temp1953; _temp1959;}); _temp1958;}), 0)); Cyc_Toc_local_decl_to_c( _temp1957,
nv, _temp1953, _temp1883); goto _LL1948;}} _LL1952:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1940, _temp1938, _temp1936, _temp1934,
_temp1883))->r)); goto _LL1948; _LL1948:;} goto _LL1923; _LL1929: { struct Cyc_List_List*
_temp1960=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1942); if( _temp1960 ==  0){({ void* _temp1961[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1961, sizeof( void*), 0u));});}(
void*)( _temp1885->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1962=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1962[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1963; _temp1963.tag= Cyc_Absyn_Var_d;
_temp1963.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1960))->hd; _temp1963;}); _temp1962;}))); _temp1960= _temp1960->tl; for( 0;
_temp1960 !=  0; _temp1960= _temp1960->tl){ struct Cyc_Absyn_Decl* _temp1964=
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp1965=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1965[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1966; _temp1966.tag= Cyc_Absyn_Var_d;
_temp1966.f1=( struct Cyc_Absyn_Vardecl*) _temp1960->hd; _temp1966;}); _temp1965;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1964, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1923;} _LL1931:({
void* _temp1967[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1967, sizeof( void*), 0u));}); goto _LL1923;
_LL1923:;} return; _LL1824: s= _temp1887; continue; _LL1826: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1895); Cyc_Toc_exp_to_c( nv, _temp1893); return;
_LL1828: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1968=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1968->v=( void*) e_typ; _temp1968;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1899);{
struct Cyc_Absyn_Stmt* _temp1969= Cyc_Absyn_seq_stmt( _temp1899, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1970= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1971= Cyc_Absyn_var_exp(
_temp1970, 0); struct Cyc_Absyn_Vardecl* _temp1972= Cyc_Absyn_new_vardecl(
_temp1970, Cyc_Absyn_exn_typ, 0); _temp1971->topt=({ struct Cyc_Core_Opt*
_temp1973=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1973->v=( void*) Cyc_Absyn_exn_typ; _temp1973;});{ struct Cyc_Absyn_Pat*
_temp1974=({ struct Cyc_Absyn_Pat* _temp1990=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1990->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1992=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1992[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1993; _temp1993.tag= Cyc_Absyn_Var_p;
_temp1993.f1= _temp1972; _temp1993;}); _temp1992;})); _temp1990->topt=({ struct
Cyc_Core_Opt* _temp1991=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1991->v=( void*) Cyc_Absyn_exn_typ; _temp1991;}); _temp1990->loc= 0;
_temp1990;}); struct Cyc_Absyn_Exp* _temp1975= Cyc_Absyn_throw_exp( _temp1971, 0);
_temp1975->topt=({ struct Cyc_Core_Opt* _temp1976=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1976->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1976;});{ struct Cyc_Absyn_Stmt* _temp1977= Cyc_Absyn_exp_stmt( _temp1975,
0); struct Cyc_Absyn_Switch_clause* _temp1978=({ struct Cyc_Absyn_Switch_clause*
_temp1987=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1987->pattern= _temp1974; _temp1987->pat_vars=({ struct Cyc_Core_Opt*
_temp1988=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1988->v=( void*)({ struct Cyc_List_List* _temp1989=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1989->hd=( void*) _temp1972;
_temp1989->tl= 0; _temp1989;}); _temp1988;}); _temp1987->where_clause= 0;
_temp1987->body= _temp1977; _temp1987->loc= 0; _temp1987;}); struct Cyc_Absyn_Stmt*
_temp1979= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1897,({ struct Cyc_List_List*
_temp1986=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1986->hd=( void*) _temp1978; _temp1986->tl= 0; _temp1986;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1979);{ struct Cyc_Absyn_Exp* _temp1980= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1985=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1985->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1985->tl= 0; _temp1985;}), 0); struct Cyc_Absyn_Stmt*
_temp1981= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1984=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1984->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1984->tl= 0; _temp1984;}), 0), 0); struct Cyc_Absyn_Exp* _temp1982= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1983= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1981, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1982, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1980, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1983, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1969, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1979, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1830: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1903->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1901);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,({
struct Cyc_List_List* _temp1994=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1994->hd=( void*) Cyc_Absyn_string_exp( Cyc_Absynpp_qvar2string(
x_var), 0); _temp1994->tl= 0; _temp1994;}), 0), Cyc_Absyn_declare_stmt( x_var,
rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( rh_exp, 0),
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp* _temp1995[ 1u]; _temp1995[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1995, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), Cyc_Absyn_seq_stmt( _temp1901, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({ struct Cyc_Absyn_Exp* _temp1996[ 1u]; _temp1996[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1996, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL1832:({ void* _temp1997[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s", sizeof(
unsigned char), 10u), _tag_arr( _temp1997, sizeof( void*), 0u));}); return;
_LL1834:({ void* _temp1998[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u),
_tag_arr( _temp1998, sizeof( void*), 0u));}); return; _LL1836:({ void* _temp1999[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("splice", sizeof( unsigned char), 7u), _tag_arr( _temp1999, sizeof(
void*), 0u));}); return; _LL1796:;}} struct _tuple15{ struct _tagged_arr* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2000= f->args; for( 0; _temp2000 !=  0; _temp2000=
_temp2000->tl){(*(( struct _tuple15*) _temp2000->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple15*) _temp2000->hd)).f3);{ struct _tuple0* _temp2001=({ struct
_tuple0* _temp2002=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2002->f1=( void*) Cyc_Absyn_Loc_n; _temp2002->f2=(*(( struct _tuple15*)
_temp2000->hd)).f1; _temp2002;}); nv= Cyc_Toc_add_varmap( nv, _temp2001, Cyc_Absyn_var_exp(
_temp2001, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp2005; void* _temp2007;
struct Cyc_Absyn_Tqual _temp2009; struct Cyc_Core_Opt* _temp2011; struct Cyc_Absyn_VarargInfo
_temp2003=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL2012: _temp2011= _temp2003.name; goto _LL2010; _LL2010: _temp2009= _temp2003.tq;
goto _LL2008; _LL2008: _temp2007=( void*) _temp2003.type; goto _LL2006; _LL2006:
_temp2005= _temp2003.inject; goto _LL2004; _LL2004: { void* _temp2013= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp2007,( void*) Cyc_Absyn_HeapRgn, _temp2009)); struct
_tuple0* _temp2014=({ struct _tuple0* _temp2017=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp2017->f1=( void*) Cyc_Absyn_Loc_n; _temp2017->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2011))->v;
_temp2017;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2015=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2015->hd=( void*)({ struct _tuple15* _temp2016=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp2016->f1=( struct _tagged_arr*)
_temp2011->v; _temp2016->f2= _temp2009; _temp2016->f3= _temp2013; _temp2016;});
_temp2015->tl= 0; _temp2015;})); nv= Cyc_Toc_add_varmap( nv, _temp2014, Cyc_Absyn_var_exp(
_temp2014, 0)); f->cyc_varargs= 0;}}{ struct Cyc_List_List* _temp2018=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( f->param_vardecls))->v;
for( 0; _temp2018 !=  0; _temp2018= _temp2018->tl){( void*)((( struct Cyc_Absyn_Vardecl*)
_temp2018->hd)->type=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)
_temp2018->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2019= s; _LL2021: if(
_temp2019 == ( void*) Cyc_Absyn_Abstract){ goto _LL2022;} else{ goto _LL2023;}
_LL2023: if( _temp2019 == ( void*) Cyc_Absyn_ExternC){ goto _LL2024;} else{ goto
_LL2025;} _LL2025: goto _LL2026; _LL2022: return( void*) Cyc_Absyn_Public;
_LL2024: return( void*) Cyc_Absyn_Extern; _LL2026: return s; _LL2020:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2027=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2028=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2028->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2028;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2029=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2027).f2); if( _temp2029 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2027).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*) _temp2029->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2027).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(( void*) Cyc_Absyn_Public));
s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2030=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2030->v=( void*) _temp2027;
_temp2030;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields= 0;} else{
struct Cyc_List_List* _temp2031=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2031 !=  0; _temp2031= _temp2031->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2031->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2031->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2032=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({
struct Cyc_Core_Opt* _temp2033=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2033->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2033;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2034=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2032).f2); if( _temp2034 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2032).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*) _temp2034->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2032).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(( void*) Cyc_Absyn_Public));
u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2035=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2035->v=( void*) _temp2032;
_temp2035;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields= 0;} else{
struct Cyc_List_List* _temp2036=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2036 !=  0; _temp2036= _temp2036->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2036->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2036->hd)->type));}}}}} static
struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far= 0; static void Cyc_Toc_tuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl* tud){ if( Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt* _temp2037=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2037->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)(
struct _tuple0*, struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp);
_temp2037;});}{ struct _tuple0* _temp2038= tud->name; if( tud->fields ==  0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2038)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2038));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2039=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; for( 0; _temp2039 !=  0; _temp2039=
_temp2039->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)
_temp2039->hd; if( f->typs ==  0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2040;
_temp2040.q_const= 1; _temp2040.q_volatile= 0; _temp2040.q_restrict= 0;
_temp2040;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2041=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2041->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2042=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2042[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2043; _temp2043.tag= Cyc_Absyn_Var_d;
_temp2043.f1= vd; _temp2043;}); _temp2042;}), 0); _temp2041->tl= Cyc_Toc_result_decls;
_temp2041;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2044; _temp2044.q_const=
1; _temp2044.q_volatile= 0; _temp2044.q_restrict= 0; _temp2044;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2045=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2045->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2046=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2046[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2047; _temp2047.tag= Cyc_Absyn_Var_d; _temp2047.f1= vd; _temp2047;});
_temp2046;}), 0); _temp2045->tl= Cyc_Toc_result_decls; _temp2045;});{ struct Cyc_List_List*
_temp2048= 0; int i= 1;{ struct Cyc_List_List* _temp2049= f->typs; for( 0;
_temp2049 !=  0;( _temp2049= _temp2049->tl, i ++)){ struct _tagged_arr*
_temp2050= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield* _temp2051=({
struct Cyc_Absyn_Structfield* _temp2053=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2053->name= _temp2050;
_temp2053->tq=(*(( struct _tuple3*) _temp2049->hd)).f1; _temp2053->type=( void*)
Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2049->hd)).f2); _temp2053->width=
0; _temp2053->attributes= 0; _temp2053;}); _temp2048=({ struct Cyc_List_List*
_temp2052=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2052->hd=( void*) _temp2051; _temp2052->tl= _temp2048; _temp2052;});}}
_temp2048=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2048); _temp2048=({ struct Cyc_List_List* _temp2054=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2054->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2055=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2055->name= Cyc_Toc_tag_sp; _temp2055->tq= Cyc_Toc_mt_tq; _temp2055->type=(
void*) Cyc_Absyn_sint_t; _temp2055->width= 0; _temp2055->attributes= 0;
_temp2055;}); _temp2054->tl= _temp2048; _temp2054;});{ struct Cyc_Absyn_Structdecl*
_temp2056=({ struct Cyc_Absyn_Structdecl* _temp2060=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2060->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2060->name=({ struct Cyc_Core_Opt* _temp2062=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2062->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2062;}); _temp2060->tvs= 0; _temp2060->fields=({ struct Cyc_Core_Opt*
_temp2061=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2061->v=( void*) _temp2048; _temp2061;}); _temp2060->attributes= 0;
_temp2060;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2057=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2057->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2058=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2058[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2059; _temp2059.tag= Cyc_Absyn_Struct_d;
_temp2059.f1= _temp2056; _temp2059;}); _temp2058;}), 0); _temp2057->tl= Cyc_Toc_result_decls;
_temp2057;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp2063=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2063->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2063;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp2064= xd->name; struct Cyc_List_List*
_temp2065=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2065 !=  0; _temp2065= _temp2065->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*) _temp2065->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2066= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2067=({ struct
Cyc_Absyn_ArrayType_struct* _temp2117=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2117[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2118; _temp2118.tag= Cyc_Absyn_ArrayType;
_temp2118.f1=( void*) Cyc_Absyn_uchar_t; _temp2118.f2= Cyc_Toc_mt_tq; _temp2118.f3=(
struct Cyc_Absyn_Exp*) _temp2066; _temp2118;}); _temp2117;}); struct Cyc_Core_Opt*
_temp2068=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2076; int _temp2077; _LL2070: if( _temp2068 ==  0){ goto _LL2071;} else{
goto _LL2072;} _LL2072: if( _temp2068 ==  0){ goto _LL2074;} else{ _temp2076=*
_temp2068; _LL2078: _temp2077=( int) _temp2076.v; if( _temp2077 ==  0){ goto
_LL2073;} else{ goto _LL2074;}} _LL2074: goto _LL2075; _LL2071: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2084; _temp2084.tag= Cyc_Std_String_pa; _temp2084.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp2083; _temp2083.tag= Cyc_Std_Int_pa;
_temp2083.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2082; _temp2082.tag= Cyc_Std_Int_pa; _temp2082.f1=( int)(( unsigned int)((
int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2081; _temp2081.tag= Cyc_Std_Int_pa;
_temp2081.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2080; _temp2080.tag= Cyc_Std_Int_pa; _temp2080.f1=( int)(( unsigned int)((
int)'\000'));{ void* _temp2079[ 5u]={& _temp2080,& _temp2081,& _temp2082,&
_temp2083,& _temp2084}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof(
unsigned char), 11u), _tag_arr( _temp2079, sizeof( void*), 5u));}}}}}}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2085= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2067, initopt);( void*)( _temp2085->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2086=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2086->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2087=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2087[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2088; _temp2088.tag= Cyc_Absyn_Var_d; _temp2088.f1= _temp2085; _temp2088;});
_temp2087;}), 0); _temp2086->tl= Cyc_Toc_result_decls; _temp2086;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2089= f->typs; for( 0; _temp2089 !=  0;( _temp2089= _temp2089->tl, i ++)){
struct _tagged_arr* _temp2090=({ struct _tagged_arr* _temp2094=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2094[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2096; _temp2096.tag= Cyc_Std_Int_pa;
_temp2096.f1=( int)(( unsigned int) i);{ void* _temp2095[ 1u]={& _temp2096}; Cyc_Std_aprintf(
_tag_arr("f%d", sizeof( unsigned char), 4u), _tag_arr( _temp2095, sizeof( void*),
1u));}}); _temp2094;}); struct Cyc_Absyn_Structfield* _temp2091=({ struct Cyc_Absyn_Structfield*
_temp2093=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2093->name= _temp2090; _temp2093->tq=(*(( struct _tuple3*) _temp2089->hd)).f1;
_temp2093->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2089->hd)).f2);
_temp2093->width= 0; _temp2093->attributes= 0; _temp2093;}); fields=({ struct
Cyc_List_List* _temp2092=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2092->hd=( void*) _temp2091; _temp2092->tl= fields; _temp2092;});}} fields=({
struct Cyc_List_List* _temp2097=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2097->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2098=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2098->name= Cyc_Toc_tag_sp; _temp2098->tq= Cyc_Toc_mt_tq; _temp2098->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2098->width=
0; _temp2098->attributes= 0; _temp2098;}); _temp2097->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2097;});{ struct Cyc_Absyn_Structdecl*
_temp2099=({ struct Cyc_Absyn_Structdecl* _temp2103=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2103->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2103->name=({ struct Cyc_Core_Opt* _temp2105=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2105->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2105;}); _temp2103->tvs= 0; _temp2103->fields=({ struct Cyc_Core_Opt*
_temp2104=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2104->v=( void*) fields; _temp2104;}); _temp2103->attributes= 0; _temp2103;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2100=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2100->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2101=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2101[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2102; _temp2102.tag= Cyc_Absyn_Struct_d;
_temp2102.f1= _temp2099; _temp2102;}); _temp2101;}), 0); _temp2100->tl= Cyc_Toc_result_decls;
_temp2100;});}} goto _LL2069;}} _LL2073: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2106= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2116; _temp2116.tag= Cyc_Std_String_pa;
_temp2116.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2115;
_temp2115.tag= Cyc_Std_Int_pa; _temp2115.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2114; _temp2114.tag= Cyc_Std_Int_pa; _temp2114.f1=(
int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2113;
_temp2113.tag= Cyc_Std_Int_pa; _temp2113.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2112; _temp2112.tag= Cyc_Std_Int_pa; _temp2112.f1=(
int)(( unsigned int)(( int)'\000'));{ void* _temp2111[ 5u]={& _temp2112,&
_temp2113,& _temp2114,& _temp2115,& _temp2116}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s",
sizeof( unsigned char), 11u), _tag_arr( _temp2111, sizeof( void*), 5u));}}}}}}),
0); struct Cyc_Absyn_Vardecl* _temp2107= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2067,( struct Cyc_Absyn_Exp*) _temp2106);( void*)( _temp2107->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2108=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2108->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2109=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2109[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2110; _temp2110.tag= Cyc_Absyn_Var_d;
_temp2110.f1= _temp2107; _temp2110;}); _temp2109;}), 0); _temp2108->tl= Cyc_Toc_result_decls;
_temp2108;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k,
int v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2069;
_LL2075: goto _LL2069; _LL2069:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2119= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v);}}
static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s){( void*)( vd->type=(
void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c( body_nv, s);
if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2120=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2126; struct Cyc_Absyn_Exp* _temp2128; struct Cyc_Absyn_Vardecl* _temp2130;
_LL2122: if(*(( int*) _temp2120) ==  Cyc_Absyn_Comprehension_e){ _LL2131:
_temp2130=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2120)->f1; goto
_LL2129; _LL2129: _temp2128=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2120)->f2; goto _LL2127; _LL2127: _temp2126=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2120)->f3; goto _LL2123;} else{ goto _LL2124;} _LL2124: goto _LL2125;
_LL2123: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2130, _temp2128, _temp2126, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2121; _LL2125: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2132= Cyc_Toc_copy_env( init_nv);
_temp2132->toplevel= 1; Cyc_Toc_exp_to_c( _temp2132, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2121; _LL2121:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2133= _new_region("prgn"); struct _RegionHandle* prgn=&
_temp2133; _push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2136; struct Cyc_List_List*
_temp2138; struct Cyc_Toc_Env* _temp2140; struct _tuple11 _temp2134= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2141: _temp2140= _temp2134.f1; goto _LL2139; _LL2139: _temp2138=
_temp2134.f2; goto _LL2137; _LL2137: _temp2136= _temp2134.f3; goto _LL2135;
_LL2135: Cyc_Toc_stmt_to_c( _temp2140, s);{ struct Cyc_Absyn_Stmt* _temp2142=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2136, _temp2142, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2143= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2144= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2143, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2136,
Cyc_Absyn_seq_stmt( _temp2144, _temp2142, 0), 0), 0);} for( 0; _temp2138 !=  0;
_temp2138= _temp2138->tl){ struct _tuple12 _temp2147; void* _temp2148; struct
_tuple0* _temp2150; struct _tuple12* _temp2145=( struct _tuple12*) _temp2138->hd;
_temp2147=* _temp2145; _LL2151: _temp2150= _temp2147.f1; goto _LL2149; _LL2149:
_temp2148= _temp2147.f2; goto _LL2146; _LL2146: s= Cyc_Absyn_declare_stmt(
_temp2150, _temp2148, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds= ds->tl){ if( ! nv->toplevel){({ void*
_temp2152[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("decls_to_c: not at toplevel!", sizeof( unsigned char), 29u), _tag_arr(
_temp2152, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
ds->hd; void* _temp2153=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp2179;
struct Cyc_Absyn_Fndecl* _temp2181; struct Cyc_Absyn_Structdecl* _temp2183;
struct Cyc_Absyn_Uniondecl* _temp2185; struct Cyc_Absyn_Tuniondecl* _temp2187;
struct Cyc_Absyn_Enumdecl* _temp2189; struct Cyc_Absyn_Typedefdecl* _temp2191;
struct Cyc_List_List* _temp2193; struct Cyc_List_List* _temp2195; struct Cyc_List_List*
_temp2197; _LL2155: if(*(( int*) _temp2153) ==  Cyc_Absyn_Var_d){ _LL2180:
_temp2179=(( struct Cyc_Absyn_Var_d_struct*) _temp2153)->f1; goto _LL2156;}
else{ goto _LL2157;} _LL2157: if(*(( int*) _temp2153) ==  Cyc_Absyn_Fn_d){
_LL2182: _temp2181=(( struct Cyc_Absyn_Fn_d_struct*) _temp2153)->f1; goto
_LL2158;} else{ goto _LL2159;} _LL2159: if(*(( int*) _temp2153) ==  Cyc_Absyn_Let_d){
goto _LL2160;} else{ goto _LL2161;} _LL2161: if(*(( int*) _temp2153) ==  Cyc_Absyn_Letv_d){
goto _LL2162;} else{ goto _LL2163;} _LL2163: if(*(( int*) _temp2153) ==  Cyc_Absyn_Struct_d){
_LL2184: _temp2183=(( struct Cyc_Absyn_Struct_d_struct*) _temp2153)->f1; goto
_LL2164;} else{ goto _LL2165;} _LL2165: if(*(( int*) _temp2153) ==  Cyc_Absyn_Union_d){
_LL2186: _temp2185=(( struct Cyc_Absyn_Union_d_struct*) _temp2153)->f1; goto
_LL2166;} else{ goto _LL2167;} _LL2167: if(*(( int*) _temp2153) ==  Cyc_Absyn_Tunion_d){
_LL2188: _temp2187=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2153)->f1; goto
_LL2168;} else{ goto _LL2169;} _LL2169: if(*(( int*) _temp2153) ==  Cyc_Absyn_Enum_d){
_LL2190: _temp2189=(( struct Cyc_Absyn_Enum_d_struct*) _temp2153)->f1; goto
_LL2170;} else{ goto _LL2171;} _LL2171: if(*(( int*) _temp2153) ==  Cyc_Absyn_Typedef_d){
_LL2192: _temp2191=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2153)->f1; goto
_LL2172;} else{ goto _LL2173;} _LL2173: if(*(( int*) _temp2153) ==  Cyc_Absyn_Namespace_d){
_LL2194: _temp2193=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2153)->f2; goto
_LL2174;} else{ goto _LL2175;} _LL2175: if(*(( int*) _temp2153) ==  Cyc_Absyn_Using_d){
_LL2196: _temp2195=(( struct Cyc_Absyn_Using_d_struct*) _temp2153)->f2; goto
_LL2176;} else{ goto _LL2177;} _LL2177: if(*(( int*) _temp2153) ==  Cyc_Absyn_ExternC_d){
_LL2198: _temp2197=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2153)->f1; goto
_LL2178;} else{ goto _LL2154;} _LL2156: { struct _tuple0* _temp2199= _temp2179->name;
if(( void*) _temp2179->sc == ( void*) Cyc_Absyn_ExternC){ _temp2199=({ struct
_tuple0* _temp2200=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2200->f1= Cyc_Absyn_rel_ns_null; _temp2200->f2=(* _temp2199).f2; _temp2200;});}
if( _temp2179->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2179->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2179->name,
Cyc_Absyn_varb_exp( _temp2199,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2201=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2201[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2202; _temp2202.tag= Cyc_Absyn_Global_b;
_temp2202.f1= _temp2179; _temp2202;}); _temp2201;}), 0)); _temp2179->name=
_temp2199;( void*)( _temp2179->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2179->sc));(
void*)( _temp2179->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2179->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2203=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2203->hd=( void*) d; _temp2203->tl=
Cyc_Toc_result_decls; _temp2203;}); goto _LL2154;} _LL2158: { struct _tuple0*
_temp2204= _temp2181->name; if(( void*) _temp2181->sc == ( void*) Cyc_Absyn_ExternC){
_temp2204=({ struct _tuple0* _temp2205=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2205->f1= Cyc_Absyn_rel_ns_null; _temp2205->f2=(*
_temp2204).f2; _temp2205;});} nv= Cyc_Toc_add_varmap( nv, _temp2181->name, Cyc_Absyn_var_exp(
_temp2204, 0)); _temp2181->name= _temp2204; Cyc_Toc_fndecl_to_c( nv, _temp2181);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2206=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2206->hd=( void*) d; _temp2206->tl=
Cyc_Toc_result_decls; _temp2206;}); goto _LL2154;} _LL2160: goto _LL2162;
_LL2162:({ void* _temp2207[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2207, sizeof( void*), 0u));}); goto _LL2154;
_LL2164: Cyc_Toc_structdecl_to_c( _temp2183); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2208=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2208->hd=( void*) d; _temp2208->tl= Cyc_Toc_result_decls; _temp2208;});
goto _LL2154; _LL2166: Cyc_Toc_uniondecl_to_c( _temp2185); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2209=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2209->hd=( void*) d; _temp2209->tl= Cyc_Toc_result_decls;
_temp2209;}); goto _LL2154; _LL2168: if( _temp2187->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2187);} else{ Cyc_Toc_tuniondecl_to_c( _temp2187);} goto _LL2154; _LL2170:
Cyc_Toc_enumdecl_to_c( nv, _temp2189); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2210=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2210->hd=( void*) d; _temp2210->tl= Cyc_Toc_result_decls; _temp2210;});
goto _LL2154; _LL2172: _temp2191->name= _temp2191->name; _temp2191->tvs= 0;(
void*)( _temp2191->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2191->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2211=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2211->hd=( void*) d; _temp2211->tl=
Cyc_Toc_result_decls; _temp2211;}); goto _LL2154; _LL2174: _temp2195= _temp2193;
goto _LL2176; _LL2176: _temp2197= _temp2195; goto _LL2178; _LL2178: nv= Cyc_Toc_decls_to_c(
nv, _temp2197, top); goto _LL2154; _LL2154:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
