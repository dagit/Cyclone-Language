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
void* t){ void* _temp198= t; struct Cyc_Core_Opt* _temp256; struct Cyc_Core_Opt*
_temp258; struct Cyc_Core_Opt _temp260; void* _temp261; struct Cyc_Absyn_Tvar*
_temp263; struct Cyc_Absyn_TunionInfo _temp265; void* _temp267; struct Cyc_Absyn_TunionFieldInfo
_temp269; void* _temp271; struct Cyc_Absyn_Tunionfield* _temp273; struct Cyc_Absyn_Tuniondecl*
_temp275; struct Cyc_Absyn_PtrInfo _temp277; struct Cyc_Absyn_Conref* _temp279;
struct Cyc_Absyn_Tqual _temp281; void* _temp283; struct Cyc_Absyn_Exp* _temp285;
struct Cyc_Absyn_Tqual _temp287; void* _temp289; struct Cyc_Absyn_FnInfo
_temp291; struct Cyc_List_List* _temp293; struct Cyc_Absyn_VarargInfo* _temp295;
int _temp297; struct Cyc_List_List* _temp299; void* _temp301; struct Cyc_List_List*
_temp303; struct Cyc_List_List* _temp305; struct Cyc_List_List* _temp307; struct
_tuple0* _temp309; struct _tuple0* _temp311; struct _tuple0* _temp313; struct
Cyc_List_List* _temp315; struct Cyc_Core_Opt* _temp317; struct Cyc_List_List*
_temp319; struct _tuple0* _temp321; void* _temp323; _LL200: if( _temp198 == (
void*) Cyc_Absyn_VoidType){ goto _LL201;} else{ goto _LL202;} _LL202: if((
unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_Evar: 0){ _LL257:
_temp256=(( struct Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp256 ==  0){
goto _LL203;} else{ goto _LL204;}} else{ goto _LL204;} _LL204: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_Evar: 0){ _LL259: _temp258=((
struct Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp258 ==  0){ goto _LL206;}
else{ _temp260=* _temp258; _LL262: _temp261=( void*) _temp260.v; goto _LL205;}}
else{ goto _LL206;} _LL206: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_VarType: 0){ _LL264: _temp263=(( struct Cyc_Absyn_VarType_struct*)
_temp198)->f1; goto _LL207;} else{ goto _LL208;} _LL208: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_TunionType: 0){ _LL266:
_temp265=(( struct Cyc_Absyn_TunionType_struct*) _temp198)->f1; _LL268: _temp267=(
void*) _temp265.tunion_info; if(*(( int*) _temp267) ==  Cyc_Absyn_KnownTunion){
goto _LL209;} else{ goto _LL210;}} else{ goto _LL210;} _LL210: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_TunionType: 0){ goto _LL211;}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_TunionFieldType: 0){ _LL270: _temp269=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp198)->f1; _LL272: _temp271=( void*) _temp269.field_info; if(*(( int*)
_temp271) ==  Cyc_Absyn_KnownTunionfield){ _LL276: _temp275=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp271)->f1; goto _LL274; _LL274: _temp273=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp271)->f2; goto _LL213;} else{ goto _LL214;}} else{ goto _LL214;} _LL214:
if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL215;} else{ goto _LL216;} _LL216: if(( unsigned int) _temp198 >  3u?*((
int*) _temp198) ==  Cyc_Absyn_PointerType: 0){ _LL278: _temp277=(( struct Cyc_Absyn_PointerType_struct*)
_temp198)->f1; _LL284: _temp283=( void*) _temp277.elt_typ; goto _LL282; _LL282:
_temp281= _temp277.tq; goto _LL280; _LL280: _temp279= _temp277.bounds; goto
_LL217;} else{ goto _LL218;} _LL218: if(( unsigned int) _temp198 >  3u?*(( int*)
_temp198) ==  Cyc_Absyn_IntType: 0){ goto _LL219;} else{ goto _LL220;} _LL220:
if( _temp198 == ( void*) Cyc_Absyn_FloatType){ goto _LL221;} else{ goto _LL222;}
_LL222: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_DoubleType:
0){ goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp198 >  3u?*((
int*) _temp198) ==  Cyc_Absyn_ArrayType: 0){ _LL290: _temp289=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp198)->f1; goto _LL288; _LL288: _temp287=((
struct Cyc_Absyn_ArrayType_struct*) _temp198)->f2; goto _LL286; _LL286: _temp285=((
struct Cyc_Absyn_ArrayType_struct*) _temp198)->f3; goto _LL225;} else{ goto
_LL226;} _LL226: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_FnType:
0){ _LL292: _temp291=(( struct Cyc_Absyn_FnType_struct*) _temp198)->f1; _LL302:
_temp301=( void*) _temp291.ret_typ; goto _LL300; _LL300: _temp299= _temp291.args;
goto _LL298; _LL298: _temp297= _temp291.c_varargs; goto _LL296; _LL296: _temp295=
_temp291.cyc_varargs; goto _LL294; _LL294: _temp293= _temp291.attributes; goto
_LL227;} else{ goto _LL228;} _LL228: if(( unsigned int) _temp198 >  3u?*(( int*)
_temp198) ==  Cyc_Absyn_TupleType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_TupleType_struct*)
_temp198)->f1; goto _LL229;} else{ goto _LL230;} _LL230: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_AnonStructType: 0){ _LL306:
_temp305=(( struct Cyc_Absyn_AnonStructType_struct*) _temp198)->f1; goto _LL231;}
else{ goto _LL232;} _LL232: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_AnonUnionType: 0){ _LL308: _temp307=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp198)->f1; goto _LL233;} else{ goto _LL234;} _LL234: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_UnionType: 0){ _LL310: _temp309=((
struct Cyc_Absyn_UnionType_struct*) _temp198)->f1; goto _LL235;} else{ goto
_LL236;} _LL236: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_StructType:
0){ _LL312: _temp311=(( struct Cyc_Absyn_StructType_struct*) _temp198)->f1; goto
_LL237;} else{ goto _LL238;} _LL238: if(( unsigned int) _temp198 >  3u?*(( int*)
_temp198) ==  Cyc_Absyn_EnumType: 0){ _LL314: _temp313=(( struct Cyc_Absyn_EnumType_struct*)
_temp198)->f1; goto _LL239;} else{ goto _LL240;} _LL240: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_AnonEnumType: 0){ _LL316:
_temp315=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp198)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_TypedefType: 0){ _LL322: _temp321=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f1; goto _LL320; _LL320: _temp319=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f2; goto _LL318; _LL318: _temp317=(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f3; goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_SizeofType: 0){ goto _LL245;}
else{ goto _LL246;} _LL246: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_RgnHandleType: 0){ _LL324: _temp323=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp198)->f1; goto _LL247;} else{ goto _LL248;} _LL248: if( _temp198 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int)
_temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_AccessEff: 0){ goto _LL251;}
else{ goto _LL252;} _LL252: if(( unsigned int) _temp198 >  3u?*(( int*) _temp198)
==  Cyc_Absyn_JoinEff: 0){ goto _LL253;} else{ goto _LL254;} _LL254: if((
unsigned int) _temp198 >  3u?*(( int*) _temp198) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL255;} else{ goto _LL199;} _LL201: return t; _LL203: return Cyc_Absyn_sint_t;
_LL205: return Cyc_Toc_typ_to_c( _temp261); _LL207: if( Cyc_Tcutil_tvar_kind(
_temp263) == ( void*) Cyc_Absyn_AnyKind){ return( void*) Cyc_Absyn_VoidType;}
else{ return Cyc_Absyn_void_star_typ();} _LL209: return Cyc_Absyn_void_star_typ();
_LL211:( int) _throw(({ void* _temp325[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp325, sizeof( void*), 0u));}));
_LL213: if( _temp273->typs ==  0){ if( _temp275->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp273->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL215:( int) _throw(({ void*
_temp326[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp326, sizeof( void*), 0u));}));
_LL217: _temp283= Cyc_Toc_typ_to_c_array( _temp283);{ void* _temp327=( void*)(
Cyc_Absyn_compress_conref( _temp279))->v; void* _temp333; _LL329: if((
unsigned int) _temp327 >  1u?*(( int*) _temp327) ==  Cyc_Absyn_Eq_constr: 0){
_LL334: _temp333=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp327)->f1;
if( _temp333 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL330;} else{ goto _LL331;}}
else{ goto _LL331;} _LL331: goto _LL332; _LL330: return Cyc_Toc_tagged_arr_typ;
_LL332: return Cyc_Absyn_star_typ( _temp283,( void*) Cyc_Absyn_HeapRgn, _temp281);
_LL328:;} _LL219: return t; _LL221: return t; _LL223: return t; _LL225: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp335=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp335[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp336; _temp336.tag= Cyc_Absyn_ArrayType; _temp336.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp289); _temp336.f2= _temp287; _temp336.f3=
_temp285; _temp336;}); _temp335;}); _LL227: { struct Cyc_List_List* _temp337= 0;
for( 0; _temp293 !=  0; _temp293= _temp293->tl){ void* _temp338=( void*)
_temp293->hd; _LL340: if( _temp338 == ( void*) Cyc_Absyn_Noreturn_att){ goto
_LL341;} else{ goto _LL342;} _LL342: if( _temp338 == ( void*) Cyc_Absyn_Const_att){
goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int) _temp338 >  16u?*((
int*) _temp338) ==  Cyc_Absyn_Format_att: 0){ goto _LL345;} else{ goto _LL346;}
_LL346: goto _LL347; _LL341: goto _LL343; _LL343: goto _LL345; _LL345: continue;
_LL347: _temp337=({ struct Cyc_List_List* _temp348=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp348->hd=( void*)(( void*)
_temp293->hd); _temp348->tl= _temp337; _temp348;}); goto _LL339; _LL339:;}{
struct Cyc_List_List* _temp349=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp299); if( _temp295 !=  0){ void* _temp350= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*) _temp295->type,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq)); struct
_tuple1* _temp351=({ struct _tuple1* _temp353=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp353->f1= _temp295->name; _temp353->f2= _temp295->tq;
_temp353->f3= _temp350; _temp353;}); _temp349=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp349,({ struct Cyc_List_List* _temp352=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp352->hd=( void*) _temp351; _temp352->tl= 0;
_temp352;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp354=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp354[ 0]=({ struct Cyc_Absyn_FnType_struct _temp355; _temp355.tag= Cyc_Absyn_FnType;
_temp355.f1=({ struct Cyc_Absyn_FnInfo _temp356; _temp356.tvars= 0; _temp356.effect=
0; _temp356.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp301); _temp356.args=
_temp349; _temp356.c_varargs= _temp297; _temp356.cyc_varargs= 0; _temp356.rgn_po=
0; _temp356.attributes= _temp337; _temp356;}); _temp355;}); _temp354;});}}
_LL229: _temp303=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp303);{
struct _tagged_arr* _temp357= Cyc_Toc_add_tuple_type( _temp303); return Cyc_Absyn_strct(
_temp357);} _LL231: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp358=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp358[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp359; _temp359.tag= Cyc_Absyn_AnonStructType; _temp359.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp305); _temp359;}); _temp358;});
_LL233: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp360=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp360[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp361; _temp361.tag=
Cyc_Absyn_AnonUnionType; _temp361.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp307); _temp361;}); _temp360;}); _LL235: if( _temp309 ==  0){({ void*
_temp362[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp362,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp309)); _LL237: if( _temp311 ==  0){({ void* _temp363[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp363, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp311)); _LL239:
return t; _LL241: Cyc_Toc_enumfields_to_c( _temp315); return t; _LL243: if(
_temp317 ==  0){ return({ void* _temp364[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u), _tag_arr( _temp364, sizeof( void*), 0u));});}
return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp365=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp365[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp366; _temp366.tag= Cyc_Absyn_TypedefType;
_temp366.f1= _temp321; _temp366.f2= 0; _temp366.f3=({ struct Cyc_Core_Opt*
_temp367=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp367->v=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp317->v); _temp367;});
_temp366;}); _temp365;}); _LL245: return Cyc_Absyn_uint_t; _LL247: return Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq); _LL249: return({
void* _temp368[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp368, sizeof( void*), 0u));});
_LL251: return({ void* _temp369[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp369, sizeof( void*), 0u));});
_LL253: return({ void* _temp370[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp370, sizeof( void*), 0u));});
_LL255: return({ void* _temp371[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp371, sizeof( void*), 0u));});
_LL199:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp372= t;
struct Cyc_Absyn_Tqual _temp378; void* _temp380; _LL374: if(( unsigned int)
_temp372 >  3u?*(( int*) _temp372) ==  Cyc_Absyn_ArrayType: 0){ _LL381: _temp380=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp372)->f1; goto _LL379; _LL379:
_temp378=(( struct Cyc_Absyn_ArrayType_struct*) _temp372)->f2; goto _LL375;}
else{ goto _LL376;} _LL376: goto _LL377; _LL375: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp380,( void*) Cyc_Absyn_HeapRgn, _temp378), e, l); _LL377: return Cyc_Absyn_cast_exp(
t, e, l); _LL373:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp382= Cyc_Tcutil_compress(
t); void* _temp424; struct Cyc_Absyn_Structdecl** _temp426; struct Cyc_List_List*
_temp428; struct Cyc_List_List* _temp430; struct Cyc_Absyn_TunionFieldInfo
_temp432; void* _temp434; struct Cyc_Absyn_Tunionfield* _temp436; struct Cyc_Absyn_Tuniondecl*
_temp438; struct Cyc_List_List* _temp440; _LL384: if( _temp382 == ( void*) Cyc_Absyn_VoidType){
goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp382 >  3u?*((
int*) _temp382) ==  Cyc_Absyn_VarType: 0){ goto _LL387;} else{ goto _LL388;}
_LL388: if(( unsigned int) _temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_IntType:
0){ goto _LL389;} else{ goto _LL390;} _LL390: if(( unsigned int) _temp382 >  3u?*((
int*) _temp382) ==  Cyc_Absyn_EnumType: 0){ goto _LL391;} else{ goto _LL392;}
_LL392: if(( unsigned int) _temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL393;} else{ goto _LL394;} _LL394: if( _temp382 == ( void*) Cyc_Absyn_FloatType){
goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp382 >  3u?*((
int*) _temp382) ==  Cyc_Absyn_DoubleType: 0){ goto _LL397;} else{ goto _LL398;}
_LL398: if(( unsigned int) _temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_FnType:
0){ goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int) _temp382 >  3u?*((
int*) _temp382) ==  Cyc_Absyn_SizeofType: 0){ goto _LL401;} else{ goto _LL402;}
_LL402: if(( unsigned int) _temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_ArrayType:
0){ _LL425: _temp424=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp382)->f1;
goto _LL403;} else{ goto _LL404;} _LL404: if(( unsigned int) _temp382 >  3u?*((
int*) _temp382) ==  Cyc_Absyn_StructType: 0){ _LL427: _temp426=(( struct Cyc_Absyn_StructType_struct*)
_temp382)->f3; goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int)
_temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_AnonStructType: 0){ _LL429:
_temp428=(( struct Cyc_Absyn_AnonStructType_struct*) _temp382)->f1; goto _LL407;}
else{ goto _LL408;} _LL408: if(( unsigned int) _temp382 >  3u?*(( int*) _temp382)
==  Cyc_Absyn_AnonUnionType: 0){ _LL431: _temp430=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp382)->f1; goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int)
_temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_TunionFieldType: 0){ _LL433:
_temp432=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp382)->f1; _LL435:
_temp434=( void*) _temp432.field_info; if(*(( int*) _temp434) ==  Cyc_Absyn_KnownTunionfield){
_LL439: _temp438=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp434)->f1;
goto _LL437; _LL437: _temp436=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp434)->f2; goto _LL411;} else{ goto _LL412;}} else{ goto _LL412;} _LL412:
if(( unsigned int) _temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_TupleType: 0){
_LL441: _temp440=(( struct Cyc_Absyn_TupleType_struct*) _temp382)->f1; goto
_LL413;} else{ goto _LL414;} _LL414: if(( unsigned int) _temp382 >  3u?*(( int*)
_temp382) ==  Cyc_Absyn_TunionType: 0){ goto _LL415;} else{ goto _LL416;} _LL416:
if(( unsigned int) _temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_UnionType: 0){
goto _LL417;} else{ goto _LL418;} _LL418: if(( unsigned int) _temp382 >  3u?*((
int*) _temp382) ==  Cyc_Absyn_PointerType: 0){ goto _LL419;} else{ goto _LL420;}
_LL420: if(( unsigned int) _temp382 >  3u?*(( int*) _temp382) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL421;} else{ goto _LL422;} _LL422: goto _LL423; _LL385: return 1;
_LL387: return 0; _LL389: return 1; _LL391: return 1; _LL393: return 1; _LL395:
return 1; _LL397: return 1; _LL399: return 1; _LL401: return 1; _LL403: return
Cyc_Toc_atomic_typ( _temp424); _LL405: if( _temp426 ==  0? 1:(* _temp426)->fields
==  0){ return 0;}{ struct Cyc_List_List* _temp442=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp426)->fields))->v; for( 0; _temp442 != 
0; _temp442= _temp442->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)
_temp442->hd)->type)){ return 0;}}} return 1; _LL407: _temp430= _temp428; goto
_LL409; _LL409: for( 0; _temp430 !=  0; _temp430= _temp430->tl){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*) _temp430->hd)->type)){ return 0;}}
return 1; _LL411: _temp440= _temp436->typs; goto _LL413; _LL413: for( 0;
_temp440 !=  0; _temp440= _temp440->tl){ if( ! Cyc_Toc_atomic_typ((*(( struct
_tuple3*) _temp440->hd)).f2)){ return 0;}} return 1; _LL415: return 0; _LL417:
return 0; _LL419: return 0; _LL421: return 0; _LL423:({ struct Cyc_Std_String_pa_struct
_temp444; _temp444.tag= Cyc_Std_String_pa; _temp444.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp443[ 1u]={& _temp444};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp443, sizeof( void*), 1u));}});
return 0; _LL383:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp445=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp451; void* _temp453;
_LL447: if(( unsigned int) _temp445 >  3u?*(( int*) _temp445) ==  Cyc_Absyn_PointerType:
0){ _LL452: _temp451=(( struct Cyc_Absyn_PointerType_struct*) _temp445)->f1;
_LL454: _temp453=( void*) _temp451.elt_typ; goto _LL448;} else{ goto _LL449;}
_LL449: goto _LL450; _LL448: { void* _temp455= Cyc_Tcutil_compress( _temp453);
_LL457: if( _temp455 == ( void*) Cyc_Absyn_VoidType){ goto _LL458;} else{ goto
_LL459;} _LL459: goto _LL460; _LL458: return 1; _LL460: return 0; _LL456:;}
_LL450: return 0; _LL446:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp461= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp473; struct Cyc_Absyn_Structdecl* _temp475; struct Cyc_List_List* _temp476;
_LL463: if(( unsigned int) _temp461 >  3u?*(( int*) _temp461) ==  Cyc_Absyn_StructType:
0){ _LL474: _temp473=(( struct Cyc_Absyn_StructType_struct*) _temp461)->f3; if(
_temp473 ==  0){ goto _LL465;} else{ _temp475=* _temp473; goto _LL464;}} else{
goto _LL465;} _LL465: if(( unsigned int) _temp461 >  3u?*(( int*) _temp461) == 
Cyc_Absyn_AnonStructType: 0){ _LL477: _temp476=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp461)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if(( unsigned int)
_temp461 >  3u?*(( int*) _temp461) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL468;}
else{ goto _LL469;} _LL469: if(( unsigned int) _temp461 >  3u?*(( int*) _temp461)
==  Cyc_Absyn_UnionType: 0){ goto _LL470;} else{ goto _LL471;} _LL471: goto
_LL472; _LL464: if( _temp475->fields ==  0){( int) _throw(({ void* _temp478[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp478, sizeof( void*), 0u));}));} _temp476=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp475->fields))->v;
goto _LL466; _LL466: { struct Cyc_Absyn_Structfield* _temp479= Cyc_Absyn_lookup_field(
_temp476, f); if( _temp479 ==  0){({ struct Cyc_Std_String_pa_struct _temp481;
_temp481.tag= Cyc_Std_String_pa; _temp481.f1=( struct _tagged_arr)* f;{ void*
_temp480[ 1u]={& _temp481};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_field: bad field %s", sizeof(
unsigned char), 28u), _tag_arr( _temp480, sizeof( void*), 1u));}});} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp479))->type);} _LL468:
return 0; _LL470: return 0; _LL472:( int) _throw(({ struct Cyc_Std_String_pa_struct
_temp483; _temp483.tag= Cyc_Std_String_pa; _temp483.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp482[ 1u]={& _temp483}; Cyc_Toc_toc_impos( _tag_arr("is_poly_field: bad type %s",
sizeof( unsigned char), 27u), _tag_arr( _temp482, sizeof( void*), 1u));}}));
_LL462:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void*
_temp484=( void*) e->r; struct _tagged_arr* _temp492; struct Cyc_Absyn_Exp*
_temp494; struct _tagged_arr* _temp496; struct Cyc_Absyn_Exp* _temp498; _LL486:
if(*(( int*) _temp484) ==  Cyc_Absyn_StructMember_e){ _LL495: _temp494=(( struct
Cyc_Absyn_StructMember_e_struct*) _temp484)->f1; goto _LL493; _LL493: _temp492=((
struct Cyc_Absyn_StructMember_e_struct*) _temp484)->f2; goto _LL487;} else{ goto
_LL488;} _LL488: if(*(( int*) _temp484) ==  Cyc_Absyn_StructArrow_e){ _LL499:
_temp498=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp484)->f1; goto _LL497;
_LL497: _temp496=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp484)->f2; goto
_LL489;} else{ goto _LL490;} _LL490: goto _LL491; _LL487: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp494->topt))->v, _temp492);
_LL489: { void* _temp500= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp498->topt))->v); struct Cyc_Absyn_PtrInfo _temp506; void*
_temp508; _LL502: if(( unsigned int) _temp500 >  3u?*(( int*) _temp500) ==  Cyc_Absyn_PointerType:
0){ _LL507: _temp506=(( struct Cyc_Absyn_PointerType_struct*) _temp500)->f1;
_LL509: _temp508=( void*) _temp506.elt_typ; goto _LL503;} else{ goto _LL504;}
_LL504: goto _LL505; _LL503: return Cyc_Toc_is_poly_field( _temp508, _temp496);
_LL505:({ struct Cyc_Std_String_pa_struct _temp511; _temp511.tag= Cyc_Std_String_pa;
_temp511.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp498->topt))->v);{ void* _temp510[ 1u]={& _temp511};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s",
sizeof( unsigned char), 30u), _tag_arr( _temp510, sizeof( void*), 1u));}});
return 0; _LL501:;} _LL491: return 0; _LL485:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_e,({
struct Cyc_List_List* _temp512=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp512->hd=( void*) s; _temp512->tl= 0; _temp512;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_atomic_e,({ struct Cyc_List_List*
_temp513=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp513->hd=( void*) s; _temp513->tl= 0; _temp513;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp514[ 2u]; _temp514[ 1u]= s; _temp514[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp514, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp515=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp515->hd=( void*) e; _temp515->tl= 0; _temp515;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp516=( void*) e->r; void* _temp522; _LL518: if(*((
int*) _temp516) ==  Cyc_Absyn_Const_e){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp516)->f1; if(( unsigned int) _temp522 >  1u?*(( int*) _temp522) ==  Cyc_Absyn_String_c:
0){ goto _LL519;} else{ goto _LL520;}} else{ goto _LL520;} _LL520: goto _LL521;
_LL519: is_string= 1; goto _LL517; _LL521: goto _LL517; _LL517:;}{ struct Cyc_Absyn_Exp*
xexp; struct Cyc_Absyn_Exp* xplussz; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp527=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp527[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp528; _temp528.tag= Cyc_Absyn_ArrayType; _temp528.f1=(
void*) Cyc_Absyn_uchar_t; _temp528.f2= Cyc_Toc_mt_tq; _temp528.f3=( struct Cyc_Absyn_Exp*)
sz; _temp528;}); _temp527;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp524=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp524->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp525=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp525[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp526; _temp526.tag= Cyc_Absyn_Var_d;
_temp526.f1= vd; _temp526;}); _temp525;}), 0); _temp524->tl= Cyc_Toc_result_decls;
_temp524;}); xexp= Cyc_Absyn_var_exp( x, 0); xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0);} else{ xexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), e, 0);
xplussz= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_add_exp( e, sz,
0), 0);}{ struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp529[ 3u]; _temp529[ 2u]=({ struct _tuple4* _temp532=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp532->f1= 0; _temp532->f2=
xplussz; _temp532;}); _temp529[ 1u]=({ struct _tuple4* _temp531=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp531->f1= 0; _temp531->f2= xexp;
_temp531;}); _temp529[ 0u]=({ struct _tuple4* _temp530=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp530->f1= 0; _temp530->f2= xexp;
_temp530;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp529, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp533=( struct Cyc_Toc_Env*) _cycalloc( sizeof( struct
Cyc_Toc_Env)); _temp533->break_lab= 0; _temp533->continue_lab= 0; _temp533->fallthru_info=
0; _temp533->varmap=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp533->toplevel= 1;
_temp533;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp534=( struct Cyc_Toc_Env*) _cycalloc( sizeof(
struct Cyc_Toc_Env)); _temp534->break_lab= e->break_lab; _temp534->continue_lab=
e->continue_lab; _temp534->fallthru_info= e->fallthru_info; _temp534->varmap= e->varmap;
_temp534->toplevel= e->toplevel; _temp534;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp535=(* x).f1; _LL537:
if(( unsigned int) _temp535 >  1u?*(( int*) _temp535) ==  Cyc_Absyn_Rel_n: 0){
goto _LL538;} else{ goto _LL539;} _LL539: goto _LL540; _LL538:( int) _throw(({
struct Cyc_Std_String_pa_struct _temp542; _temp542.tag= Cyc_Std_String_pa;
_temp542.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp541[
1u]={& _temp542}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp541, sizeof( void*), 1u));}}));
_LL540: goto _LL536; _LL536:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
k, struct Cyc_Absyn_Exp* v)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict*
f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp543= 0; for( 0; fallthru_binders !=  0; fallthru_binders= fallthru_binders->tl){
_temp543=({ struct Cyc_List_List* _temp544=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp544->hd=( void*)(( struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name; _temp544->tl= _temp543; _temp544;});} _temp543=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp543);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp545=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp545->v=( void*) break_l; _temp545;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp546=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp546->v=( void*)({ struct _tuple6* _temp547=( struct _tuple6*) _cycalloc(
sizeof( struct _tuple6)); _temp547->f1= fallthru_l; _temp547->f2= _temp543;
_temp547->f3= next_case_env->varmap; _temp547;}); _temp546;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp548=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp548->v=( void*) break_l; _temp548;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp549=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp549->v=( void*)({
struct _tuple6* _temp550=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp550->f1= next_l; _temp550->f2= 0; _temp550->f3=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp550;}); _temp549;}); return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s); static int Cyc_Toc_need_null_check( struct Cyc_Absyn_Exp*
e){ void* _temp551=( void*) e->annot; _LL553: if( _temp551 ==  Cyc_CfFlowInfo_UnknownZ){
goto _LL554;} else{ goto _LL555;} _LL555: if( _temp551 ==  Cyc_CfFlowInfo_NotZero){
goto _LL556;} else{ goto _LL557;} _LL557: if( _temp551 ==  Cyc_CfFlowInfo_IsZero){
goto _LL558;} else{ goto _LL559;} _LL559: if( _temp551 ==  Cyc_Absyn_EmptyAnnot){
goto _LL560;} else{ goto _LL561;} _LL561: goto _LL562; _LL554: return Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL556: return 0;
_LL558:({ void* _temp563[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("dereference of NULL pointer",
sizeof( unsigned char), 28u), _tag_arr( _temp563, sizeof( void*), 0u));});
return 0; _LL560:({ void* _temp564[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("!", sizeof( unsigned char), 2u), _tag_arr( _temp564, sizeof( void*), 0u));});({
void* _temp565[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("unreachable dereference",
sizeof( unsigned char), 24u), _tag_arr( _temp565, sizeof( void*), 0u));});
return 0; _LL562:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp566=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp566[ 0]=({ struct Cyc_Core_Impossible_struct _temp567; _temp567.tag= Cyc_Core_Impossible;
_temp567.f1= _tag_arr("need_null_check", sizeof( unsigned char), 16u); _temp567;});
_temp566;})); _LL552:;} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){
if( e->topt ==  0){({ void* _temp568[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp568, sizeof( void*), 0u));});}
return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp569[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp569, sizeof( void*), 0u));});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp570=( struct _tuple3*) _cycalloc( sizeof(
struct _tuple3)); _temp570->f1= Cyc_Toc_mt_tq; _temp570->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp570;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp571=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp571->f1= 0; _temp571->f2= e; _temp571;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp572= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp572): Cyc_Toc_malloc_ptr( _temp572), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp572), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp573=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp573 !=  0; _temp573=
_temp573->tl){ struct _tuple4 _temp576; struct Cyc_Absyn_Exp* _temp577; struct
Cyc_List_List* _temp579; struct _tuple4* _temp574=( struct _tuple4*) _temp573->hd;
_temp576=* _temp574; _LL580: _temp579= _temp576.f1; goto _LL578; _LL578:
_temp577= _temp576.f2; goto _LL575; _LL575: { struct Cyc_Absyn_Exp* e_index; if(
_temp579 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(
_temp579->tl !=  0){({ void* _temp581[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp581, sizeof( void*), 0u));});}{
void* _temp582=( void*) _temp579->hd; void* _temp583= _temp582; struct Cyc_Absyn_Exp*
_temp589; _LL585: if(*(( int*) _temp583) ==  Cyc_Absyn_ArrayElement){ _LL590:
_temp589=(( struct Cyc_Absyn_ArrayElement_struct*) _temp583)->f1; goto _LL586;}
else{ goto _LL587;} _LL587: if(*(( int*) _temp583) ==  Cyc_Absyn_FieldName){
goto _LL588;} else{ goto _LL584;} _LL586: Cyc_Toc_exp_to_c( nv, _temp589);
e_index= _temp589; goto _LL584; _LL588: e_index=({ void* _temp591[ 0u]={};((
struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("field name designators in array", sizeof( unsigned char), 32u),
_tag_arr( _temp591, sizeof( void*), 0u));}); goto _LL584; _LL584:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp592=( void*)
_temp577->r; struct Cyc_List_List* _temp602; struct Cyc_Absyn_Exp* _temp604;
struct Cyc_Absyn_Exp* _temp606; struct Cyc_Absyn_Vardecl* _temp608; struct Cyc_List_List*
_temp610; void* _temp612; _LL594: if(*(( int*) _temp592) ==  Cyc_Absyn_Array_e){
_LL603: _temp602=(( struct Cyc_Absyn_Array_e_struct*) _temp592)->f1; goto _LL595;}
else{ goto _LL596;} _LL596: if(*(( int*) _temp592) ==  Cyc_Absyn_Comprehension_e){
_LL609: _temp608=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp592)->f1;
goto _LL607; _LL607: _temp606=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp592)->f2; goto _LL605; _LL605: _temp604=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp592)->f3; goto _LL597;} else{ goto _LL598;} _LL598: if(*(( int*) _temp592)
==  Cyc_Absyn_AnonStruct_e){ _LL613: _temp612=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp592)->f1; goto _LL611; _LL611: _temp610=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp592)->f2; goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL595: s=
Cyc_Toc_init_array( nv, lval, _temp602, s); goto _LL593; _LL597: s= Cyc_Toc_init_comprehension(
nv, lval, _temp608, _temp606, _temp604, s, 0); goto _LL593; _LL599: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp612, _temp610, s); goto _LL593; _LL601: Cyc_Toc_exp_to_c( nv,
_temp577); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp577, 0), s, 0); goto _LL593; _LL593:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp614= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp614, Cyc_Absyn_varb_exp( _temp614,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp637=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp637[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp638; _temp638.tag= Cyc_Absyn_Local_b; _temp638.f1= vd; _temp638;});
_temp637;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp614, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp614, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp614, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp614, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp615=( void*) e2->r; struct Cyc_List_List* _temp625; struct Cyc_Absyn_Exp*
_temp627; struct Cyc_Absyn_Exp* _temp629; struct Cyc_Absyn_Vardecl* _temp631;
struct Cyc_List_List* _temp633; void* _temp635; _LL617: if(*(( int*) _temp615)
==  Cyc_Absyn_Array_e){ _LL626: _temp625=(( struct Cyc_Absyn_Array_e_struct*)
_temp615)->f1; goto _LL618;} else{ goto _LL619;} _LL619: if(*(( int*) _temp615)
==  Cyc_Absyn_Comprehension_e){ _LL632: _temp631=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp615)->f1; goto _LL630; _LL630: _temp629=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp615)->f2; goto _LL628; _LL628: _temp627=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp615)->f3; goto _LL620;} else{ goto _LL621;} _LL621: if(*(( int*) _temp615)
==  Cyc_Absyn_AnonStruct_e){ _LL636: _temp635=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp615)->f1; goto _LL634; _LL634: _temp633=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp615)->f2; goto _LL622;} else{ goto _LL623;} _LL623: goto _LL624; _LL618:
body= Cyc_Toc_init_array( nv2, lval, _temp625, Cyc_Absyn_skip_stmt( 0)); goto
_LL616; _LL620: body= Cyc_Toc_init_comprehension( nv2, lval, _temp631, _temp629,
_temp627, Cyc_Absyn_skip_stmt( 0), 0); goto _LL616; _LL622: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp635, _temp633, Cyc_Absyn_skip_stmt( 0)); goto _LL616; _LL624: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL616; _LL616:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp614, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp639=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp639 !=  0; _temp639= _temp639->tl){
struct _tuple4 _temp642; struct Cyc_Absyn_Exp* _temp643; struct Cyc_List_List*
_temp645; struct _tuple4* _temp640=( struct _tuple4*) _temp639->hd; _temp642=*
_temp640; _LL646: _temp645= _temp642.f1; goto _LL644; _LL644: _temp643= _temp642.f2;
goto _LL641; _LL641: if( _temp645 ==  0){({ void* _temp647[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp647, sizeof( void*), 0u));});} if(
_temp645->tl !=  0){({ void* _temp648[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u), _tag_arr( _temp648, sizeof( void*), 0u));});}{
void* _temp649=( void*) _temp645->hd; struct _tagged_arr* _temp655; _LL651: if(*((
int*) _temp649) ==  Cyc_Absyn_FieldName){ _LL656: _temp655=(( struct Cyc_Absyn_FieldName_struct*)
_temp649)->f1; goto _LL652;} else{ goto _LL653;} _LL653: goto _LL654; _LL652: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp655, 0);{ void*
_temp657=( void*) _temp643->r; struct Cyc_List_List* _temp667; struct Cyc_Absyn_Exp*
_temp669; struct Cyc_Absyn_Exp* _temp671; struct Cyc_Absyn_Vardecl* _temp673;
struct Cyc_List_List* _temp675; void* _temp677; _LL659: if(*(( int*) _temp657)
==  Cyc_Absyn_Array_e){ _LL668: _temp667=(( struct Cyc_Absyn_Array_e_struct*)
_temp657)->f1; goto _LL660;} else{ goto _LL661;} _LL661: if(*(( int*) _temp657)
==  Cyc_Absyn_Comprehension_e){ _LL674: _temp673=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp657)->f1; goto _LL672; _LL672: _temp671=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp657)->f2; goto _LL670; _LL670: _temp669=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp657)->f3; goto _LL662;} else{ goto _LL663;} _LL663: if(*(( int*) _temp657)
==  Cyc_Absyn_AnonStruct_e){ _LL678: _temp677=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp657)->f1; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp657)->f2; goto _LL664;} else{ goto _LL665;} _LL665: goto _LL666; _LL660: s=
Cyc_Toc_init_array( nv, lval, _temp667, s); goto _LL658; _LL662: s= Cyc_Toc_init_comprehension(
nv, lval, _temp673, _temp671, _temp669, s, 0); goto _LL658; _LL664: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp677, _temp675, s); goto _LL658; _LL666: Cyc_Toc_exp_to_c( nv,
_temp643); if( Cyc_Toc_is_poly_field( struct_type, _temp655)){ _temp643= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp643, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp643, 0), 0), s, 0); goto _LL658; _LL658:;} goto
_LL650;} _LL654:( int) _throw(({ void* _temp679[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp679, sizeof( void*), 0u));})); _LL650:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp680= _new_region("r");
struct _RegionHandle* r=& _temp680; _push_region( r);{ struct Cyc_List_List*
_temp681=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp682= Cyc_Toc_add_tuple_type( _temp681); struct
_tuple0* _temp683= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp684= Cyc_Absyn_var_exp(
_temp683, 0); struct Cyc_Absyn_Stmt* _temp685= Cyc_Absyn_exp_stmt( _temp684, 0);
struct Cyc_Absyn_Exp*(* _temp686)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp687=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp687); for( 0; _temp687
!=  0;( _temp687= _temp687->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
_temp687->hd; struct Cyc_Absyn_Exp* lval= _temp686( _temp684, Cyc_Absyn_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp688=( void*) e->r; struct Cyc_List_List*
_temp696; struct Cyc_Absyn_Exp* _temp698; struct Cyc_Absyn_Exp* _temp700; struct
Cyc_Absyn_Vardecl* _temp702; _LL690: if(*(( int*) _temp688) ==  Cyc_Absyn_Array_e){
_LL697: _temp696=(( struct Cyc_Absyn_Array_e_struct*) _temp688)->f1; goto _LL691;}
else{ goto _LL692;} _LL692: if(*(( int*) _temp688) ==  Cyc_Absyn_Comprehension_e){
_LL703: _temp702=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp688)->f1;
goto _LL701; _LL701: _temp700=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp688)->f2; goto _LL699; _LL699: _temp698=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp688)->f3; goto _LL693;} else{ goto _LL694;} _LL694: goto _LL695; _LL691:
_temp685= Cyc_Toc_init_array( nv, lval, _temp696, _temp685); goto _LL689; _LL693:
_temp685= Cyc_Toc_init_comprehension( nv, lval, _temp702, _temp700, _temp698,
_temp685, 0); goto _LL689; _LL695: Cyc_Toc_exp_to_c( nv, e); _temp685= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp686( _temp684, Cyc_Absyn_fieldname(
i), 0), e, 0), 0), _temp685, 0); goto _LL689; _LL689:;}}}{ struct Cyc_Absyn_Exp*
_temp704= Cyc_Toc_make_struct( nv, _temp683, Cyc_Absyn_strct( _temp682),
_temp685, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp704;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp705= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp706= Cyc_Absyn_var_exp( _temp705, 0); struct Cyc_Absyn_Stmt*
_temp707= Cyc_Absyn_exp_stmt( _temp706, 0); struct Cyc_Absyn_Exp*(* _temp708)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp709= _new_region("r"); struct _RegionHandle* r=&
_temp709; _push_region( r);{ struct Cyc_List_List* _temp710=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp710 !=  0; _temp710= _temp710->tl){ struct _tuple4 _temp713; struct Cyc_Absyn_Exp*
_temp714; struct Cyc_List_List* _temp716; struct _tuple4* _temp711=( struct
_tuple4*) _temp710->hd; _temp713=* _temp711; _LL717: _temp716= _temp713.f1; goto
_LL715; _LL715: _temp714= _temp713.f2; goto _LL712; _LL712: is_atomic= is_atomic?
Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp714->topt))->v):
0; if( _temp716 ==  0){({ void* _temp718[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp718, sizeof( void*), 0u));});} if(
_temp716->tl !=  0){ struct _tuple0* _temp719= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp720= Cyc_Absyn_var_exp( _temp719, 0); for( 0; _temp716 !=  0; _temp716=
_temp716->tl){ void* _temp721=( void*) _temp716->hd; struct _tagged_arr*
_temp727; _LL723: if(*(( int*) _temp721) ==  Cyc_Absyn_FieldName){ _LL728:
_temp727=(( struct Cyc_Absyn_FieldName_struct*) _temp721)->f1; goto _LL724;}
else{ goto _LL725;} _LL725: goto _LL726; _LL724: if( Cyc_Toc_is_poly_field(
struct_type, _temp727)){ _temp720= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp720, 0);} _temp707= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp708( _temp706, _temp727, 0), _temp720, 0), 0), _temp707, 0); goto _LL722;
_LL726:( int) _throw(({ void* _temp729[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u), _tag_arr( _temp729, sizeof( void*), 0u));}));
_LL722:;} Cyc_Toc_exp_to_c( nv, _temp714); _temp707= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp720, _temp714, 0), 0), _temp707, 0);} else{ void*
_temp730=( void*) _temp716->hd; struct _tagged_arr* _temp736; _LL732: if(*(( int*)
_temp730) ==  Cyc_Absyn_FieldName){ _LL737: _temp736=(( struct Cyc_Absyn_FieldName_struct*)
_temp730)->f1; goto _LL733;} else{ goto _LL734;} _LL734: goto _LL735; _LL733: {
struct Cyc_Absyn_Exp* lval= _temp708( _temp706, _temp736, 0);{ void* _temp738=(
void*) _temp714->r; struct Cyc_List_List* _temp748; struct Cyc_Absyn_Exp*
_temp750; struct Cyc_Absyn_Exp* _temp752; struct Cyc_Absyn_Vardecl* _temp754;
struct Cyc_List_List* _temp756; void* _temp758; _LL740: if(*(( int*) _temp738)
==  Cyc_Absyn_Array_e){ _LL749: _temp748=(( struct Cyc_Absyn_Array_e_struct*)
_temp738)->f1; goto _LL741;} else{ goto _LL742;} _LL742: if(*(( int*) _temp738)
==  Cyc_Absyn_Comprehension_e){ _LL755: _temp754=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp738)->f1; goto _LL753; _LL753: _temp752=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp738)->f2; goto _LL751; _LL751: _temp750=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp738)->f3; goto _LL743;} else{ goto _LL744;} _LL744: if(*(( int*) _temp738)
==  Cyc_Absyn_AnonStruct_e){ _LL759: _temp758=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp738)->f1; goto _LL757; _LL757: _temp756=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp738)->f2; goto _LL745;} else{ goto _LL746;} _LL746: goto _LL747; _LL741:
_temp707= Cyc_Toc_init_array( nv, lval, _temp748, _temp707); goto _LL739; _LL743:
_temp707= Cyc_Toc_init_comprehension( nv, lval, _temp754, _temp752, _temp750,
_temp707, 0); goto _LL739; _LL745: _temp707= Cyc_Toc_init_anon_struct( nv, lval,
_temp758, _temp756, _temp707); goto _LL739; _LL747: Cyc_Toc_exp_to_c( nv,
_temp714); if( Cyc_Toc_is_poly_field( struct_type, _temp736)){ _temp714= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp714, 0);} _temp707= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp714, 0), 0), _temp707, 0); goto _LL739; _LL739:;}
goto _LL731;} _LL735:( int) _throw(({ void* _temp760[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp760, sizeof( void*), 0u));})); _LL731:;}}}{ struct Cyc_Absyn_Exp* _temp761=
Cyc_Toc_make_struct( nv, _temp705, Cyc_Absyn_strctq( tdn), _temp707, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp761;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp762=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp762[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp763; _temp763.tag= Cyc_Absyn_Increment_e;
_temp763.f1= e1; _temp763.f2=( void*) incr; _temp763;}); _temp762;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp764=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp774; struct Cyc_Absyn_Exp*
_temp776; void* _temp778; struct _tagged_arr* _temp780; struct Cyc_Absyn_Exp*
_temp782; _LL766: if(*(( int*) _temp764) ==  Cyc_Absyn_StmtExp_e){ _LL775:
_temp774=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp764)->f1; goto _LL767;}
else{ goto _LL768;} _LL768: if(*(( int*) _temp764) ==  Cyc_Absyn_Cast_e){ _LL779:
_temp778=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp764)->f1; goto _LL777;
_LL777: _temp776=(( struct Cyc_Absyn_Cast_e_struct*) _temp764)->f2; goto _LL769;}
else{ goto _LL770;} _LL770: if(*(( int*) _temp764) ==  Cyc_Absyn_StructMember_e){
_LL783: _temp782=(( struct Cyc_Absyn_StructMember_e_struct*) _temp764)->f1; goto
_LL781; _LL781: _temp780=(( struct Cyc_Absyn_StructMember_e_struct*) _temp764)->f2;
goto _LL771;} else{ goto _LL772;} _LL772: goto _LL773; _LL767: Cyc_Toc_lvalue_assign_stmt(
_temp774, fs, f, f_env); goto _LL765; _LL769: Cyc_Toc_lvalue_assign( _temp776,
fs, f, f_env); goto _LL765; _LL771:( void*)( e1->r=( void*)(( void*) _temp782->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp784=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp784->hd=( void*) _temp780; _temp784->tl= fs; _temp784;}), f, f_env); goto
_LL765; _LL773: if( ! Cyc_Absyn_is_lvalue( e1)){({ struct Cyc_Std_String_pa_struct
_temp786; _temp786.tag= Cyc_Std_String_pa; _temp786.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp785[ 1u]={& _temp786};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u), _tag_arr( _temp785, sizeof( void*), 1u));}});}{
struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs !=  0; fs= fs->tl){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_arr*) fs->hd,
e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto
_LL765;} _LL765:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp787=( void*) s->r; struct Cyc_Absyn_Exp*
_temp797; struct Cyc_Absyn_Stmt* _temp799; struct Cyc_Absyn_Decl* _temp801;
struct Cyc_Absyn_Stmt* _temp803; _LL789: if(( unsigned int) _temp787 >  1u?*((
int*) _temp787) ==  Cyc_Absyn_Exp_s: 0){ _LL798: _temp797=(( struct Cyc_Absyn_Exp_s_struct*)
_temp787)->f1; goto _LL790;} else{ goto _LL791;} _LL791: if(( unsigned int)
_temp787 >  1u?*(( int*) _temp787) ==  Cyc_Absyn_Decl_s: 0){ _LL802: _temp801=((
struct Cyc_Absyn_Decl_s_struct*) _temp787)->f1; goto _LL800; _LL800: _temp799=((
struct Cyc_Absyn_Decl_s_struct*) _temp787)->f2; goto _LL792;} else{ goto _LL793;}
_LL793: if(( unsigned int) _temp787 >  1u?*(( int*) _temp787) ==  Cyc_Absyn_Seq_s:
0){ _LL804: _temp803=(( struct Cyc_Absyn_Seq_s_struct*) _temp787)->f2; goto
_LL794;} else{ goto _LL795;} _LL795: goto _LL796; _LL790: Cyc_Toc_lvalue_assign(
_temp797, fs, f, f_env); goto _LL788; _LL792: Cyc_Toc_lvalue_assign_stmt(
_temp799, fs, f, f_env); goto _LL788; _LL794: Cyc_Toc_lvalue_assign_stmt(
_temp803, fs, f, f_env); goto _LL788; _LL796:({ struct Cyc_Std_String_pa_struct
_temp806; _temp806.tag= Cyc_Std_String_pa; _temp806.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp805[ 1u]={& _temp806};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s", sizeof(
unsigned char), 23u), _tag_arr( _temp805, sizeof( void*), 1u));}}); goto _LL788;
_LL788:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2,
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x ==  0){ return 0;} result=({ struct
Cyc_List_List* _temp807=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp807->hd=( void*) f(( void*) x->hd, env); _temp807->tl=
0; _temp807;}); prev= result; for( x= x->tl; x !=  0; x= x->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp808=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp808->hd=( void*) f((
void*) x->hd, env); _temp808->tl= 0; _temp808;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp809=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp809->f1= 0; _temp809->f2= e; _temp809;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp810=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp816; _LL812: if((
unsigned int) _temp810 >  3u?*(( int*) _temp810) ==  Cyc_Absyn_PointerType: 0){
_LL817: _temp816=(( struct Cyc_Absyn_PointerType_struct*) _temp810)->f1; goto
_LL813;} else{ goto _LL814;} _LL814: goto _LL815; _LL813: return _temp816;
_LL815:( int) _throw(({ void* _temp818[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("get_ptr_typ: not a pointer!",
sizeof( unsigned char), 28u), _tag_arr( _temp818, sizeof( void*), 0u));}));
_LL811:;} struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp819=( void*) e->r; if( e->topt ==  0){({
struct Cyc_Std_String_pa_struct _temp821; _temp821.tag= Cyc_Std_String_pa;
_temp821.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp820[ 1u]={&
_temp821};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u), _tag_arr(
_temp820, sizeof( void*), 1u));}}); return;}{ void* old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp822= _temp819; void*
_temp908; void* _temp910; struct _tuple0* _temp912; struct _tuple0* _temp914;
struct Cyc_List_List* _temp916; void* _temp918; void* _temp920; struct Cyc_Absyn_Exp*
_temp922; struct Cyc_Absyn_Exp* _temp924; struct Cyc_Core_Opt* _temp926; struct
Cyc_Absyn_Exp* _temp928; struct Cyc_Absyn_Exp* _temp930; struct Cyc_Absyn_Exp*
_temp932; struct Cyc_Absyn_Exp* _temp934; struct Cyc_Absyn_Exp* _temp936; struct
Cyc_Absyn_Exp* _temp938; struct Cyc_List_List* _temp940; struct Cyc_Absyn_Exp*
_temp942; struct Cyc_Absyn_VarargCallInfo* _temp944; struct Cyc_List_List*
_temp946; struct Cyc_Absyn_Exp* _temp948; struct Cyc_Absyn_VarargCallInfo*
_temp950; struct Cyc_Absyn_VarargCallInfo _temp952; struct Cyc_Absyn_VarargInfo*
_temp953; struct Cyc_List_List* _temp955; int _temp957; struct Cyc_List_List*
_temp959; struct Cyc_Absyn_Exp* _temp961; struct Cyc_Absyn_Exp* _temp963; struct
Cyc_Absyn_Exp* _temp965; struct Cyc_List_List* _temp967; struct Cyc_Absyn_Exp*
_temp969; struct Cyc_Absyn_Exp* _temp971; void* _temp973; void** _temp975;
struct Cyc_Absyn_Exp* _temp976; struct Cyc_Absyn_Exp* _temp978; struct Cyc_Absyn_Exp*
_temp980; struct Cyc_Absyn_Exp* _temp982; void* _temp984; void* _temp986; struct
_tagged_arr* _temp988; void* _temp990; void* _temp992; unsigned int _temp994;
void* _temp996; struct Cyc_Absyn_Exp* _temp998; struct _tagged_arr* _temp1000;
struct Cyc_Absyn_Exp* _temp1002; struct _tagged_arr* _temp1004; struct Cyc_Absyn_Exp*
_temp1006; struct Cyc_Absyn_Exp* _temp1008; struct Cyc_Absyn_Exp* _temp1010;
struct Cyc_List_List* _temp1012; struct Cyc_List_List* _temp1014; struct Cyc_Absyn_Exp*
_temp1016; struct Cyc_Absyn_Exp* _temp1018; struct Cyc_Absyn_Vardecl* _temp1020;
struct Cyc_Absyn_Structdecl* _temp1022; struct Cyc_List_List* _temp1024; struct
Cyc_Core_Opt* _temp1026; struct _tuple0* _temp1028; struct Cyc_List_List*
_temp1030; void* _temp1032; struct Cyc_Absyn_Tunionfield* _temp1034; struct Cyc_Absyn_Tuniondecl*
_temp1036; struct Cyc_List_List* _temp1038; struct Cyc_Absyn_Tunionfield*
_temp1040; struct Cyc_Absyn_Tuniondecl* _temp1042; struct Cyc_List_List*
_temp1044; void* _temp1046; struct Cyc_Absyn_Exp* _temp1048; struct Cyc_Absyn_Stmt*
_temp1050; struct Cyc_Absyn_Fndecl* _temp1052; _LL824: if(*(( int*) _temp822) == 
Cyc_Absyn_Const_e){ _LL909: _temp908=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp822)->f1; if( _temp908 == ( void*) Cyc_Absyn_Null_c){ goto _LL825;} else{
goto _LL826;}} else{ goto _LL826;} _LL826: if(*(( int*) _temp822) ==  Cyc_Absyn_Const_e){
goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp822) ==  Cyc_Absyn_Var_e){
_LL913: _temp912=(( struct Cyc_Absyn_Var_e_struct*) _temp822)->f1; goto _LL911;
_LL911: _temp910=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp822)->f2; goto
_LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp822) ==  Cyc_Absyn_UnknownId_e){
_LL915: _temp914=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp822)->f1; goto
_LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp822) ==  Cyc_Absyn_Primop_e){
_LL919: _temp918=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp822)->f1;
goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_Primop_e_struct*) _temp822)->f2;
goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp822) ==  Cyc_Absyn_Increment_e){
_LL923: _temp922=(( struct Cyc_Absyn_Increment_e_struct*) _temp822)->f1; goto
_LL921; _LL921: _temp920=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp822)->f2; goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp822)
==  Cyc_Absyn_AssignOp_e){ _LL929: _temp928=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp822)->f1; goto _LL927; _LL927: _temp926=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp822)->f2; goto _LL925; _LL925: _temp924=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp822)->f3; goto _LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp822)
==  Cyc_Absyn_Conditional_e){ _LL935: _temp934=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp822)->f1; goto _LL933; _LL933: _temp932=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp822)->f2; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp822)->f3; goto _LL839;} else{ goto _LL840;} _LL840: if(*(( int*) _temp822)
==  Cyc_Absyn_SeqExp_e){ _LL939: _temp938=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp822)->f1; goto _LL937; _LL937: _temp936=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp822)->f2; goto _LL841;} else{ goto _LL842;} _LL842: if(*(( int*) _temp822)
==  Cyc_Absyn_UnknownCall_e){ _LL943: _temp942=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp822)->f1; goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp822)->f2; goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp822)
==  Cyc_Absyn_FnCall_e){ _LL949: _temp948=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp822)->f1; goto _LL947; _LL947: _temp946=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp822)->f2; goto _LL945; _LL945: _temp944=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp822)->f3; if( _temp944 ==  0){ goto _LL845;} else{ goto _LL846;}} else{
goto _LL846;} _LL846: if(*(( int*) _temp822) ==  Cyc_Absyn_FnCall_e){ _LL962:
_temp961=(( struct Cyc_Absyn_FnCall_e_struct*) _temp822)->f1; goto _LL960;
_LL960: _temp959=(( struct Cyc_Absyn_FnCall_e_struct*) _temp822)->f2; goto
_LL951; _LL951: _temp950=(( struct Cyc_Absyn_FnCall_e_struct*) _temp822)->f3;
if( _temp950 ==  0){ goto _LL848;} else{ _temp952=* _temp950; _LL958: _temp957=
_temp952.num_varargs; goto _LL956; _LL956: _temp955= _temp952.injectors; goto
_LL954; _LL954: _temp953= _temp952.vai; goto _LL847;}} else{ goto _LL848;}
_LL848: if(*(( int*) _temp822) ==  Cyc_Absyn_Throw_e){ _LL964: _temp963=((
struct Cyc_Absyn_Throw_e_struct*) _temp822)->f1; goto _LL849;} else{ goto _LL850;}
_LL850: if(*(( int*) _temp822) ==  Cyc_Absyn_NoInstantiate_e){ _LL966: _temp965=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp822)->f1; goto _LL851;} else{
goto _LL852;} _LL852: if(*(( int*) _temp822) ==  Cyc_Absyn_Instantiate_e){
_LL970: _temp969=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp822)->f1; goto
_LL968; _LL968: _temp967=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp822)->f2;
goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp822) ==  Cyc_Absyn_Cast_e){
_LL974: _temp973=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp822)->f1;
_temp975=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp822)->f1);
goto _LL972; _LL972: _temp971=(( struct Cyc_Absyn_Cast_e_struct*) _temp822)->f2;
goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp822) ==  Cyc_Absyn_Address_e){
_LL977: _temp976=(( struct Cyc_Absyn_Address_e_struct*) _temp822)->f1; goto
_LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp822) ==  Cyc_Absyn_New_e){
_LL981: _temp980=(( struct Cyc_Absyn_New_e_struct*) _temp822)->f1; goto _LL979;
_LL979: _temp978=(( struct Cyc_Absyn_New_e_struct*) _temp822)->f2; goto _LL859;}
else{ goto _LL860;} _LL860: if(*(( int*) _temp822) ==  Cyc_Absyn_Sizeofexp_e){
_LL983: _temp982=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp822)->f1; goto
_LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp822) ==  Cyc_Absyn_Sizeoftyp_e){
_LL985: _temp984=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp822)->f1;
goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp822) ==  Cyc_Absyn_Gentyp_e){
goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp822) ==  Cyc_Absyn_Offsetof_e){
_LL991: _temp990=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp822)->f1;
goto _LL987; _LL987: _temp986=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp822)->f2; if(*(( int*) _temp986) ==  Cyc_Absyn_StructField){ _LL989:
_temp988=(( struct Cyc_Absyn_StructField_struct*) _temp986)->f1; goto _LL867;}
else{ goto _LL868;}} else{ goto _LL868;} _LL868: if(*(( int*) _temp822) ==  Cyc_Absyn_Offsetof_e){
_LL997: _temp996=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp822)->f1;
goto _LL993; _LL993: _temp992=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp822)->f2; if(*(( int*) _temp992) ==  Cyc_Absyn_TupleIndex){ _LL995:
_temp994=(( struct Cyc_Absyn_TupleIndex_struct*) _temp992)->f1; goto _LL869;}
else{ goto _LL870;}} else{ goto _LL870;} _LL870: if(*(( int*) _temp822) ==  Cyc_Absyn_Deref_e){
_LL999: _temp998=(( struct Cyc_Absyn_Deref_e_struct*) _temp822)->f1; goto _LL871;}
else{ goto _LL872;} _LL872: if(*(( int*) _temp822) ==  Cyc_Absyn_StructMember_e){
_LL1003: _temp1002=(( struct Cyc_Absyn_StructMember_e_struct*) _temp822)->f1;
goto _LL1001; _LL1001: _temp1000=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp822)->f2; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp822)
==  Cyc_Absyn_StructArrow_e){ _LL1007: _temp1006=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp822)->f1; goto _LL1005; _LL1005: _temp1004=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp822)->f2; goto _LL875;} else{ goto _LL876;} _LL876: if(*(( int*) _temp822)
==  Cyc_Absyn_Subscript_e){ _LL1011: _temp1010=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp822)->f1; goto _LL1009; _LL1009: _temp1008=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp822)->f2; goto _LL877;} else{ goto _LL878;} _LL878: if(*(( int*) _temp822)
==  Cyc_Absyn_Tuple_e){ _LL1013: _temp1012=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp822)->f1; goto _LL879;} else{ goto _LL880;} _LL880: if(*(( int*) _temp822)
==  Cyc_Absyn_Array_e){ _LL1015: _temp1014=(( struct Cyc_Absyn_Array_e_struct*)
_temp822)->f1; goto _LL881;} else{ goto _LL882;} _LL882: if(*(( int*) _temp822)
==  Cyc_Absyn_Comprehension_e){ _LL1021: _temp1020=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp822)->f1; goto _LL1019; _LL1019: _temp1018=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp822)->f2; goto _LL1017; _LL1017: _temp1016=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp822)->f3; goto _LL883;} else{ goto _LL884;} _LL884: if(*(( int*) _temp822)
==  Cyc_Absyn_Struct_e){ _LL1029: _temp1028=(( struct Cyc_Absyn_Struct_e_struct*)
_temp822)->f1; goto _LL1027; _LL1027: _temp1026=(( struct Cyc_Absyn_Struct_e_struct*)
_temp822)->f2; goto _LL1025; _LL1025: _temp1024=(( struct Cyc_Absyn_Struct_e_struct*)
_temp822)->f3; goto _LL1023; _LL1023: _temp1022=(( struct Cyc_Absyn_Struct_e_struct*)
_temp822)->f4; goto _LL885;} else{ goto _LL886;} _LL886: if(*(( int*) _temp822)
==  Cyc_Absyn_AnonStruct_e){ _LL1033: _temp1032=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp822)->f1; goto _LL1031; _LL1031: _temp1030=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp822)->f2; goto _LL887;} else{ goto _LL888;} _LL888: if(*(( int*) _temp822)
==  Cyc_Absyn_Tunion_e){ _LL1039: _temp1038=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp822)->f3; if( _temp1038 ==  0){ goto _LL1037;} else{ goto _LL890;} _LL1037:
_temp1036=(( struct Cyc_Absyn_Tunion_e_struct*) _temp822)->f4; goto _LL1035;
_LL1035: _temp1034=(( struct Cyc_Absyn_Tunion_e_struct*) _temp822)->f5; goto
_LL889;} else{ goto _LL890;} _LL890: if(*(( int*) _temp822) ==  Cyc_Absyn_Tunion_e){
_LL1045: _temp1044=(( struct Cyc_Absyn_Tunion_e_struct*) _temp822)->f3; goto
_LL1043; _LL1043: _temp1042=(( struct Cyc_Absyn_Tunion_e_struct*) _temp822)->f4;
goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_Tunion_e_struct*) _temp822)->f5;
goto _LL891;} else{ goto _LL892;} _LL892: if(*(( int*) _temp822) ==  Cyc_Absyn_Enum_e){
goto _LL893;} else{ goto _LL894;} _LL894: if(*(( int*) _temp822) ==  Cyc_Absyn_AnonEnum_e){
goto _LL895;} else{ goto _LL896;} _LL896: if(*(( int*) _temp822) ==  Cyc_Absyn_Malloc_e){
_LL1049: _temp1048=(( struct Cyc_Absyn_Malloc_e_struct*) _temp822)->f1; goto
_LL1047; _LL1047: _temp1046=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp822)->f2; goto _LL897;} else{ goto _LL898;} _LL898: if(*(( int*) _temp822)
==  Cyc_Absyn_StmtExp_e){ _LL1051: _temp1050=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp822)->f1; goto _LL899;} else{ goto _LL900;} _LL900: if(*(( int*) _temp822)
==  Cyc_Absyn_UnresolvedMem_e){ goto _LL901;} else{ goto _LL902;} _LL902: if(*((
int*) _temp822) ==  Cyc_Absyn_CompoundLit_e){ goto _LL903;} else{ goto _LL904;}
_LL904: if(*(( int*) _temp822) ==  Cyc_Absyn_Codegen_e){ _LL1053: _temp1052=((
struct Cyc_Absyn_Codegen_e_struct*) _temp822)->f1; goto _LL905;} else{ goto
_LL906;} _LL906: if(*(( int*) _temp822) ==  Cyc_Absyn_Fill_e){ goto _LL907;}
else{ goto _LL823;} _LL825: { struct Cyc_Absyn_Exp* _temp1054= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1054,
_temp1054))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1055[ 3u]; _temp1055[ 2u]=
_temp1054; _temp1055[ 1u]= _temp1054; _temp1055[ 0u]= _temp1054;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1055, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL823;} _LL827: goto _LL823; _LL829:{ struct _handler_cons
_temp1056; _push_handler(& _temp1056);{ int _temp1058= 0; if( setjmp( _temp1056.handler)){
_temp1058= 1;} if( ! _temp1058){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( nv->varmap,
_temp912))->r));; _pop_handler();} else{ void* _temp1057=( void*) _exn_thrown;
void* _temp1060= _temp1057; _LL1062: if( _temp1060 ==  Cyc_Dict_Absent){ goto
_LL1063;} else{ goto _LL1064;} _LL1064: goto _LL1065; _LL1063:({ struct Cyc_Std_String_pa_struct
_temp1067; _temp1067.tag= Cyc_Std_String_pa; _temp1067.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp912);{ void* _temp1066[ 1u]={& _temp1067};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1066, sizeof( void*), 1u));}});
return; _LL1065:( void) _throw( _temp1060); _LL1061:;}}} goto _LL823; _LL831:({
void* _temp1068[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1068, sizeof( void*), 0u));}); goto _LL823; _LL833: { struct Cyc_List_List*
_temp1069=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp916);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp916);{
void* _temp1070= _temp918; _LL1072: if( _temp1070 == ( void*) Cyc_Absyn_Size){
goto _LL1073;} else{ goto _LL1074;} _LL1074: if( _temp1070 == ( void*) Cyc_Absyn_Plus){
goto _LL1075;} else{ goto _LL1076;} _LL1076: if( _temp1070 == ( void*) Cyc_Absyn_Minus){
goto _LL1077;} else{ goto _LL1078;} _LL1078: if( _temp1070 == ( void*) Cyc_Absyn_Eq){
goto _LL1079;} else{ goto _LL1080;} _LL1080: if( _temp1070 == ( void*) Cyc_Absyn_Neq){
goto _LL1081;} else{ goto _LL1082;} _LL1082: if( _temp1070 == ( void*) Cyc_Absyn_Gt){
goto _LL1083;} else{ goto _LL1084;} _LL1084: if( _temp1070 == ( void*) Cyc_Absyn_Gte){
goto _LL1085;} else{ goto _LL1086;} _LL1086: if( _temp1070 == ( void*) Cyc_Absyn_Lt){
goto _LL1087;} else{ goto _LL1088;} _LL1088: if( _temp1070 == ( void*) Cyc_Absyn_Lte){
goto _LL1089;} else{ goto _LL1090;} _LL1090: goto _LL1091; _LL1073: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp916))->hd;{
void* _temp1092= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1100; struct Cyc_Absyn_PtrInfo
_temp1102; struct Cyc_Absyn_Conref* _temp1104; void* _temp1106; _LL1094: if((
unsigned int) _temp1092 >  3u?*(( int*) _temp1092) ==  Cyc_Absyn_ArrayType: 0){
_LL1101: _temp1100=(( struct Cyc_Absyn_ArrayType_struct*) _temp1092)->f3; goto
_LL1095;} else{ goto _LL1096;} _LL1096: if(( unsigned int) _temp1092 >  3u?*((
int*) _temp1092) ==  Cyc_Absyn_PointerType: 0){ _LL1103: _temp1102=(( struct Cyc_Absyn_PointerType_struct*)
_temp1092)->f1; _LL1107: _temp1106=( void*) _temp1102.elt_typ; goto _LL1105;
_LL1105: _temp1104= _temp1102.bounds; goto _LL1097;} else{ goto _LL1098;}
_LL1098: goto _LL1099; _LL1095:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1100))->r)); goto _LL1093; _LL1097:{ void* _temp1108= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1104); struct Cyc_Absyn_Exp* _temp1114; _LL1110: if(
_temp1108 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1111;} else{ goto _LL1112;}
_LL1112: if(( unsigned int) _temp1108 >  1u?*(( int*) _temp1108) ==  Cyc_Absyn_Upper_b:
0){ _LL1115: _temp1114=(( struct Cyc_Absyn_Upper_b_struct*) _temp1108)->f1; goto
_LL1113;} else{ goto _LL1109;} _LL1111:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1116[ 2u]; _temp1116[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1106), 0); _temp1116[ 0u]=(
struct Cyc_Absyn_Exp*) _temp916->hd;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1116, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0))->r)); goto _LL1109; _LL1113:( void*)( e->r=( void*)(( void*)
_temp1114->r)); goto _LL1109; _LL1109:;} goto _LL1093; _LL1099:({ struct Cyc_Std_String_pa_struct
_temp1119; _temp1119.tag= Cyc_Std_String_pa; _temp1119.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1118; _temp1118.tag= Cyc_Std_String_pa;
_temp1118.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v);{ void* _temp1117[ 2u]={& _temp1118,& _temp1119};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("size primop applied to non-array %s (%s)", sizeof( unsigned char), 41u),
_tag_arr( _temp1117, sizeof( void*), 2u));}}}); return; _LL1093:;} goto _LL1071;}
_LL1075: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1069))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp916))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp916->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1120[ 3u]; _temp1120[ 2u]=
e2; _temp1120[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1120[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1120, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1071;} _LL1077: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1069))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp916))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp916->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null( _temp1069->tl))->hd)){( void*)( e1->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp,
0))->r));( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_divide_exp( Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0), 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1121[ 3u]; _temp1121[ 2u]=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, e2, 0); _temp1121[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1121[ 0u]= e1;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1121, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} goto _LL1071;} _LL1079: goto _LL1081; _LL1081: goto _LL1083;
_LL1083: goto _LL1085; _LL1085: goto _LL1087; _LL1087: goto _LL1089; _LL1089: {
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp916))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp916->tl))->hd; void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp1069))->hd; void* t2=( void*)(( struct
Cyc_List_List*) _check_null( _temp1069->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1071;} _LL1091: goto
_LL1071; _LL1071:;} goto _LL823;} _LL835: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp922->topt))->v; Cyc_Toc_exp_to_c( nv, _temp922);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1122=( _temp920 == ( void*) Cyc_Absyn_PostInc?
1: _temp920 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1123=( _temp920 == ( void*) Cyc_Absyn_PreInc?
1: _temp920 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1122,({ struct Cyc_Absyn_Exp* _temp1124[ 3u];
_temp1124[ 2u]= Cyc_Absyn_signed_int_exp( _temp1123, 0); _temp1124[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1124[ 0u]= Cyc_Absyn_address_exp( _temp922,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1124, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp922): 0){ Cyc_Toc_lvalue_assign(
_temp922, 0, Cyc_Toc_incr_lvalue, _temp920);( void*)( e->r=( void*)(( void*)
_temp922->r));} goto _LL823;}} _LL837: { int e1_poly= Cyc_Toc_is_poly_project(
_temp928); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp928->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp924->topt))->v; Cyc_Toc_exp_to_c( nv, _temp928); Cyc_Toc_exp_to_c(
nv, _temp924);{ int done= 0; if( _temp926 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1125=( void*) _temp926->v; _LL1127: if( _temp1125 == ( void*)
Cyc_Absyn_Plus){ goto _LL1128;} else{ goto _LL1129;} _LL1129: if( _temp1125 == (
void*) Cyc_Absyn_Minus){ goto _LL1130;} else{ goto _LL1131;} _LL1131: goto
_LL1132; _LL1128: change= _temp924; goto _LL1126; _LL1130: change= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, _temp924, 0); goto _LL1126; _LL1132:( int) _throw(({
void* _temp1133[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u), _tag_arr( _temp1133, sizeof( void*), 0u));}));
goto _LL1126; _LL1126:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_Absyn_Exp* _temp1134[ 3u];
_temp1134[ 2u]= change; _temp1134[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1134[ 0u]= Cyc_Absyn_address_exp( _temp928, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1134, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp924->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp924->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp928)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)(
_temp928, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1135=( struct
_tuple7*) _cycalloc( sizeof( struct _tuple7) *  1); _temp1135[ 0]=({ struct
_tuple7 _temp1136; _temp1136.f1= _temp926; _temp1136.f2= _temp924; _temp1136;});
_temp1135;}));( void*)( e->r=( void*)(( void*) _temp928->r));}} goto _LL823;}}
_LL839: Cyc_Toc_exp_to_c( nv, _temp934); Cyc_Toc_exp_to_c( nv, _temp932); Cyc_Toc_exp_to_c(
nv, _temp930); goto _LL823; _LL841: Cyc_Toc_exp_to_c( nv, _temp938); Cyc_Toc_exp_to_c(
nv, _temp936); goto _LL823; _LL843: _temp948= _temp942; _temp946= _temp940; goto
_LL845; _LL845: Cyc_Toc_exp_to_c( nv, _temp948);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp946); goto _LL823; _LL847:{
struct _RegionHandle _temp1137= _new_region("r"); struct _RegionHandle* r=&
_temp1137; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp957, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp953->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1154=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1154[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1155; _temp1155.tag= Cyc_Absyn_ArrayType;
_temp1155.f1=( void*) cva_type; _temp1155.f2= Cyc_Toc_mt_tq; _temp1155.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1155;}); _temp1154;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp959); int num_normargs=
num_args -  _temp957; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp959= _temp959->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp959))->hd); new_args=({ struct Cyc_List_List*
_temp1138=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1138->hd=( void*)(( struct Cyc_Absyn_Exp*) _temp959->hd); _temp1138->tl=
new_args; _temp1138;});}} new_args=({ struct Cyc_List_List* _temp1139=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1139->hd=( void*)
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1140[ 3u];
_temp1140[ 2u]= num_varargs_exp; _temp1140[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1140[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1140, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1139->tl= new_args; _temp1139;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp961);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp961, new_args, 0), 0); if( _temp953->inject){ struct _tagged_arr vs=({
unsigned int _temp1150=( unsigned int) _temp957; struct _tuple0** _temp1151=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1150)); struct _tagged_arr _temp1153= _tag_arr( _temp1151, sizeof( struct
_tuple0*),( unsigned int) _temp957);{ unsigned int _temp1152= _temp1150;
unsigned int i; for( i= 0; i <  _temp1152; i ++){ _temp1151[ i]= Cyc_Toc_temp_var();}};
_temp1153;}); struct Cyc_List_List* _temp1141= 0;{ int i= _temp957 -  1; for( 0;
i >=  0; -- i){ _temp1141=({ struct Cyc_List_List* _temp1142=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1142->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1142->tl=
_temp1141; _temp1142;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1141, 0), s, 0);{ int i= 0;
for( 0; _temp959 !=  0;((( _temp959= _temp959->tl, _temp955= _temp955->tl)), ++
i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*) _temp959->hd; void*
arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; struct
_tuple0* var=*(( struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct
_tuple0*), i)); struct Cyc_Absyn_Exp* varexp= Cyc_Absyn_var_exp( var, 0); struct
Cyc_Absyn_Tunionfield _temp1145; struct Cyc_List_List* _temp1146; struct _tuple0*
_temp1148; struct Cyc_Absyn_Tunionfield* _temp1143=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp955))->hd; _temp1145=* _temp1143;
_LL1149: _temp1148= _temp1145.name; goto _LL1147; _LL1147: _temp1146= _temp1145.typs;
goto _LL1144; _LL1144: { void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1146))->hd)).f2); Cyc_Toc_exp_to_c( nv,
arg); if( Cyc_Toc_is_void_star( field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ,
arg, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Absyn_fieldname( 1), 0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1148, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt( var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1148, _tag_arr("_struct", sizeof( unsigned char), 8u))), 0, s, 0);}}}}
else{{ int i= 0; for( 0; _temp959 !=  0;( _temp959= _temp959->tl, ++ i)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp959->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),(
struct Cyc_Absyn_Exp*) _temp959->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL823; _LL849: Cyc_Toc_exp_to_c( nv,
_temp963);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp963), 0))->r)); goto _LL823; _LL851: Cyc_Toc_exp_to_c(
nv, _temp965); goto _LL823; _LL853: Cyc_Toc_exp_to_c( nv, _temp969); for( 0;
_temp967 !=  0; _temp967= _temp967->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)
_temp967->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1156= Cyc_Tcutil_compress(( void*) _temp967->hd); _LL1158: if((
unsigned int) _temp1156 >  3u?*(( int*) _temp1156) ==  Cyc_Absyn_VarType: 0){
goto _LL1159;} else{ goto _LL1160;} _LL1160: if(( unsigned int) _temp1156 >  3u?*((
int*) _temp1156) ==  Cyc_Absyn_TunionType: 0){ goto _LL1161;} else{ goto _LL1162;}
_LL1162: goto _LL1163; _LL1159: continue; _LL1161: continue; _LL1163:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), _temp969, 0))->r)); goto _LL1157;
_LL1157:;} break;}} goto _LL823; _LL855: { void* old_t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp971->topt))->v; void* new_typ=* _temp975;* _temp975= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv, _temp971);{ struct _tuple8 _temp1165=({ struct
_tuple8 _temp1164; _temp1164.f1= Cyc_Tcutil_compress( old_t2); _temp1164.f2= Cyc_Tcutil_compress(
new_typ); _temp1164;}); void* _temp1173; struct Cyc_Absyn_PtrInfo _temp1175;
void* _temp1177; struct Cyc_Absyn_PtrInfo _temp1179; void* _temp1181; void*
_temp1183; struct Cyc_Absyn_PtrInfo _temp1185; _LL1167: _LL1178: _temp1177=
_temp1165.f1; if(( unsigned int) _temp1177 >  3u?*(( int*) _temp1177) ==  Cyc_Absyn_PointerType:
0){ _LL1180: _temp1179=(( struct Cyc_Absyn_PointerType_struct*) _temp1177)->f1;
goto _LL1174;} else{ goto _LL1169;} _LL1174: _temp1173= _temp1165.f2; if((
unsigned int) _temp1173 >  3u?*(( int*) _temp1173) ==  Cyc_Absyn_PointerType: 0){
_LL1176: _temp1175=(( struct Cyc_Absyn_PointerType_struct*) _temp1173)->f1; goto
_LL1168;} else{ goto _LL1169;} _LL1169: _LL1184: _temp1183= _temp1165.f1; if((
unsigned int) _temp1183 >  3u?*(( int*) _temp1183) ==  Cyc_Absyn_PointerType: 0){
_LL1186: _temp1185=(( struct Cyc_Absyn_PointerType_struct*) _temp1183)->f1; goto
_LL1182;} else{ goto _LL1171;} _LL1182: _temp1181= _temp1165.f2; if((
unsigned int) _temp1181 >  3u?*(( int*) _temp1181) ==  Cyc_Absyn_IntType: 0){
goto _LL1170;} else{ goto _LL1171;} _LL1171: goto _LL1172; _LL1168: { int
_temp1187=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1179.nullable); int _temp1188=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1175.nullable); void* _temp1189= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1179.bounds); void* _temp1190= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1175.bounds);{ struct _tuple8 _temp1192=({ struct
_tuple8 _temp1191; _temp1191.f1= _temp1189; _temp1191.f2= _temp1190; _temp1191;});
void* _temp1202; void* _temp1204; void* _temp1206; void* _temp1208; struct Cyc_Absyn_Exp*
_temp1210; void* _temp1212; struct Cyc_Absyn_Exp* _temp1214; void* _temp1216;
_LL1194: _LL1205: _temp1204= _temp1192.f1; if(( unsigned int) _temp1204 >  1u?*((
int*) _temp1204) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1203;} else{ goto _LL1196;}
_LL1203: _temp1202= _temp1192.f2; if(( unsigned int) _temp1202 >  1u?*(( int*)
_temp1202) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1195;} else{ goto _LL1196;}
_LL1196: _LL1209: _temp1208= _temp1192.f1; if(( unsigned int) _temp1208 >  1u?*((
int*) _temp1208) ==  Cyc_Absyn_Upper_b: 0){ _LL1211: _temp1210=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1208)->f1; goto _LL1207;} else{ goto _LL1198;} _LL1207: _temp1206=
_temp1192.f2; if( _temp1206 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1197;}
else{ goto _LL1198;} _LL1198: _LL1217: _temp1216= _temp1192.f1; if( _temp1216 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1213;} else{ goto _LL1200;} _LL1213:
_temp1212= _temp1192.f2; if(( unsigned int) _temp1212 >  1u?*(( int*) _temp1212)
==  Cyc_Absyn_Upper_b: 0){ _LL1215: _temp1214=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1212)->f1; goto _LL1199;} else{ goto _LL1200;} _LL1200: goto _LL1201;
_LL1195: if( _temp1187? ! _temp1188: 0){ if( nv->toplevel){({ void* _temp1218[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1218, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp975, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1219=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1219->hd=( void*) _temp971;
_temp1219->tl= 0; _temp1219;}), 0), 0))->r));} goto _LL1193; _LL1197: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1210, _temp971))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1220[ 3u]; _temp1220[ 2u]=
_temp1210; _temp1220[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1175.elt_typ), 0); _temp1220[ 0u]= _temp971;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1220, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1193; _LL1199: if( nv->toplevel){({ void* _temp1221[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1221, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1222= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1224[ 3u]; _temp1224[ 2u]= _temp1214; _temp1224[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1179.elt_typ), 0); _temp1224[ 0u]= _temp971;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1224, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1188){( void*)(
_temp1222->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1223=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1223->hd=( void*) Cyc_Absyn_copy_exp( _temp1222);
_temp1223->tl= 0; _temp1223;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp975, _temp1222, 0))->r)); goto _LL1193;} _LL1201: goto _LL1193; _LL1193:;}
goto _LL1166;} _LL1170:{ void* _temp1225= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1185.bounds); _LL1227: if( _temp1225 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1228;} else{ goto _LL1229;} _LL1229: goto _LL1230; _LL1228:( void*)( _temp971->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp971->r,
_temp971->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1226; _LL1230: goto _LL1226;
_LL1226:;} goto _LL1166; _LL1172: goto _LL1166; _LL1166:;} goto _LL823;} _LL857:{
void* _temp1231=( void*) _temp976->r; struct Cyc_List_List* _temp1239; struct
_tuple0* _temp1241; struct Cyc_List_List* _temp1243; _LL1233: if(*(( int*)
_temp1231) ==  Cyc_Absyn_Struct_e){ _LL1242: _temp1241=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1231)->f1; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1231)->f3; goto _LL1234;} else{ goto _LL1235;} _LL1235: if(*(( int*)
_temp1231) ==  Cyc_Absyn_Tuple_e){ _LL1244: _temp1243=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1231)->f1; goto _LL1236;} else{ goto _LL1237;} _LL1237: goto _LL1238;
_LL1234: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1246;
_temp1246.tag= Cyc_Std_String_pa; _temp1246.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp976->loc);{ void* _temp1245[ 1u]={& _temp1246};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1245, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp976->topt))->v, 1, 0, _temp1239, _temp1241))->r)); goto
_LL1232; _LL1236: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1248;
_temp1248.tag= Cyc_Std_String_pa; _temp1248.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp976->loc);{ void* _temp1247[ 1u]={& _temp1248};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1247, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1243))->r));
goto _LL1232; _LL1238: Cyc_Toc_exp_to_c( nv, _temp976); if( ! Cyc_Absyn_is_lvalue(
_temp976)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp976, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp976, 0))->r));} goto _LL1232; _LL1232:;} goto _LL823; _LL859: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1250; _temp1250.tag= Cyc_Std_String_pa;
_temp1250.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp978->loc);{
void* _temp1249[ 1u]={& _temp1250};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1249, sizeof( void*), 1u));}});}{ void*
_temp1251=( void*) _temp978->r; struct Cyc_List_List* _temp1263; struct Cyc_Absyn_Exp*
_temp1265; struct Cyc_Absyn_Exp* _temp1267; struct Cyc_Absyn_Vardecl* _temp1269;
struct Cyc_List_List* _temp1271; struct _tuple0* _temp1273; struct Cyc_List_List*
_temp1275; _LL1253: if(*(( int*) _temp1251) ==  Cyc_Absyn_Array_e){ _LL1264:
_temp1263=(( struct Cyc_Absyn_Array_e_struct*) _temp1251)->f1; goto _LL1254;}
else{ goto _LL1255;} _LL1255: if(*(( int*) _temp1251) ==  Cyc_Absyn_Comprehension_e){
_LL1270: _temp1269=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1251)->f1;
goto _LL1268; _LL1268: _temp1267=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1251)->f2; goto _LL1266; _LL1266: _temp1265=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1251)->f3; goto _LL1256;} else{ goto _LL1257;} _LL1257: if(*(( int*)
_temp1251) ==  Cyc_Absyn_Struct_e){ _LL1274: _temp1273=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1251)->f1; goto _LL1272; _LL1272: _temp1271=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1251)->f3; goto _LL1258;} else{ goto _LL1259;} _LL1259: if(*(( int*)
_temp1251) ==  Cyc_Absyn_Tuple_e){ _LL1276: _temp1275=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1251)->f1; goto _LL1260;} else{ goto _LL1261;} _LL1261: goto _LL1262;
_LL1254: { struct _tuple0* _temp1277= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1278= Cyc_Absyn_var_exp( _temp1277, 0); struct Cyc_Absyn_Stmt* _temp1279=
Cyc_Toc_init_array( nv, _temp1278, _temp1263, Cyc_Absyn_exp_stmt( _temp1278, 0));
void* old_elt_typ;{ void* _temp1280= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1286; struct Cyc_Absyn_Tqual _temp1288; void* _temp1290; _LL1282: if((
unsigned int) _temp1280 >  3u?*(( int*) _temp1280) ==  Cyc_Absyn_PointerType: 0){
_LL1287: _temp1286=(( struct Cyc_Absyn_PointerType_struct*) _temp1280)->f1;
_LL1291: _temp1290=( void*) _temp1286.elt_typ; goto _LL1289; _LL1289: _temp1288=
_temp1286.tq; goto _LL1283;} else{ goto _LL1284;} _LL1284: goto _LL1285; _LL1283:
old_elt_typ= _temp1290; goto _LL1281; _LL1285: old_elt_typ=({ void* _temp1292[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1292, sizeof( void*), 0u));}); goto
_LL1281; _LL1281:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1293= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1294= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1263), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp980 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1294);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp980); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1294);}{
struct Cyc_Absyn_Exp* _temp1295= Cyc_Absyn_cast_exp( _temp1293, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1277,
_temp1293,( struct Cyc_Absyn_Exp*) _temp1295, _temp1279, 0), 0))->r)); goto
_LL1252;}}} _LL1256: { int is_tagged_ptr= 0;{ void* _temp1296= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1302; struct Cyc_Absyn_Conref* _temp1304;
struct Cyc_Absyn_Tqual _temp1306; void* _temp1308; _LL1298: if(( unsigned int)
_temp1296 >  3u?*(( int*) _temp1296) ==  Cyc_Absyn_PointerType: 0){ _LL1303:
_temp1302=(( struct Cyc_Absyn_PointerType_struct*) _temp1296)->f1; _LL1309:
_temp1308=( void*) _temp1302.elt_typ; goto _LL1307; _LL1307: _temp1306=
_temp1302.tq; goto _LL1305; _LL1305: _temp1304= _temp1302.bounds; goto _LL1299;}
else{ goto _LL1300;} _LL1300: goto _LL1301; _LL1299: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1304) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1297;
_LL1301:({ void* _temp1310[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1310, sizeof( void*), 0u));}); goto
_LL1297; _LL1297:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1265->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1267);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1269, Cyc_Absyn_var_exp( max, 0), _temp1265, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1311= _new_region("r"); struct _RegionHandle* r=&
_temp1311; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1331=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1331->hd=( void*)({ struct _tuple9* _temp1332=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1332->f1= max; _temp1332->f2=
Cyc_Absyn_uint_t; _temp1332->f3=( struct Cyc_Absyn_Exp*) _temp1267; _temp1332;});
_temp1331->tl= 0; _temp1331;}); struct Cyc_Absyn_Exp* ai; if( _temp980 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1312[ 2u]; _temp1312[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1312[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1312, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp980); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1313[
2u]; _temp1313[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1313[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1313, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1314=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1314->hd=( void*)({ struct _tuple9* _temp1315=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1315->f1= a; _temp1315->f2=
ptr_typ; _temp1315->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1315;}); _temp1314->tl=
decls; _temp1314;}); if( is_tagged_ptr){ struct _tuple0* _temp1316= Cyc_Toc_temp_var();
void* _temp1317= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1318=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1321[ 3u];
_temp1321[ 2u]= _temp1267; _temp1321[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1321[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1321, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1319=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1319->hd=( void*)({
struct _tuple9* _temp1320=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1320->f1= _temp1316; _temp1320->f2= _temp1317; _temp1320->f3=(
struct Cyc_Absyn_Exp*) _temp1318; _temp1320;}); _temp1319->tl= decls; _temp1319;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1316, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1322= decls; for( 0; _temp1322 !=  0;
_temp1322= _temp1322->tl){ struct Cyc_Absyn_Exp* _temp1325; void* _temp1327;
struct _tuple0* _temp1329; struct _tuple9 _temp1323=*(( struct _tuple9*)
_temp1322->hd); _LL1330: _temp1329= _temp1323.f1; goto _LL1328; _LL1328:
_temp1327= _temp1323.f2; goto _LL1326; _LL1326: _temp1325= _temp1323.f3; goto
_LL1324; _LL1324: s= Cyc_Absyn_declare_stmt( _temp1329, _temp1327, _temp1325, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region( r);} goto _LL1252;}}} _LL1258:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp978->topt))->v, 1,
_temp980, _temp1271, _temp1273))->r)); goto _LL1252; _LL1260:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp980, _temp1275))->r)); goto
_LL1252; _LL1262: { void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp978->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); if( _temp980
==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp980); Cyc_Toc_exp_to_c( nv, r);
mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1333=( void*)
_temp978->r; struct Cyc_Absyn_Exp* _temp1339; void* _temp1341; _LL1335: if(*((
int*) _temp1333) ==  Cyc_Absyn_Cast_e){ _LL1342: _temp1341=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1333)->f1; goto _LL1340; _LL1340: _temp1339=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1333)->f2; goto _LL1336;} else{ goto _LL1337;} _LL1337: goto _LL1338;
_LL1336:{ struct _tuple8 _temp1344=({ struct _tuple8 _temp1343; _temp1343.f1=
Cyc_Tcutil_compress( _temp1341); _temp1343.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1339->topt))->v); _temp1343;}); void*
_temp1350; struct Cyc_Absyn_PtrInfo _temp1352; struct Cyc_Absyn_Conref*
_temp1354; void* _temp1356; struct Cyc_Absyn_PtrInfo _temp1358; struct Cyc_Absyn_Conref*
_temp1360; void* _temp1362; _LL1346: _LL1357: _temp1356= _temp1344.f1; if((
unsigned int) _temp1356 >  3u?*(( int*) _temp1356) ==  Cyc_Absyn_PointerType: 0){
_LL1359: _temp1358=(( struct Cyc_Absyn_PointerType_struct*) _temp1356)->f1;
_LL1363: _temp1362=( void*) _temp1358.elt_typ; goto _LL1361; _LL1361: _temp1360=
_temp1358.bounds; goto _LL1351;} else{ goto _LL1348;} _LL1351: _temp1350=
_temp1344.f2; if(( unsigned int) _temp1350 >  3u?*(( int*) _temp1350) ==  Cyc_Absyn_PointerType:
0){ _LL1353: _temp1352=(( struct Cyc_Absyn_PointerType_struct*) _temp1350)->f1;
_LL1355: _temp1354= _temp1352.bounds; goto _LL1347;} else{ goto _LL1348;}
_LL1348: goto _LL1349; _LL1347:{ struct _tuple8 _temp1365=({ struct _tuple8
_temp1364; _temp1364.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1360);
_temp1364.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1354); _temp1364;});
void* _temp1371; struct Cyc_Absyn_Exp* _temp1373; void* _temp1375; _LL1367:
_LL1376: _temp1375= _temp1365.f1; if( _temp1375 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1372;} else{ goto _LL1369;} _LL1372: _temp1371= _temp1365.f2; if((
unsigned int) _temp1371 >  1u?*(( int*) _temp1371) ==  Cyc_Absyn_Upper_b: 0){
_LL1374: _temp1373=(( struct Cyc_Absyn_Upper_b_struct*) _temp1371)->f1; goto
_LL1368;} else{ goto _LL1369;} _LL1369: goto _LL1370; _LL1368: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1377[ 4u]; _temp1377[ 3u]= _temp1373; _temp1377[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1362, 0); _temp1377[ 1u]= _temp1339; _temp1377[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1377, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1366; _LL1370: goto _LL1366; _LL1366:;} goto _LL1345;
_LL1349: goto _LL1345; _LL1345:;} goto _LL1334; _LL1338: goto _LL1334; _LL1334:;}
if( ! done){ struct _tuple0* _temp1378= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1379= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1378, 0), 0); struct Cyc_Absyn_Exp*
_temp1380= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp978);
_temp1379= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1378, 0), _temp1380, 0), _temp978, 0), _temp1379, 0);{
void* _temp1381= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1378,
_temp1381,( struct Cyc_Absyn_Exp*) mexp, _temp1379, 0), 0))->r));}} goto _LL1252;}}
_LL1252:;} goto _LL823; _LL861: Cyc_Toc_exp_to_c( nv, _temp982); goto _LL823;
_LL863:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1382=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1382[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1383; _temp1383.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1383.f1=( void*) Cyc_Toc_typ_to_c_array( _temp984);
_temp1383;}); _temp1382;}))); goto _LL823; _LL865:( int) _throw(({ void*
_temp1384[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("__gen() in code generator",
sizeof( unsigned char), 26u), _tag_arr( _temp1384, sizeof( void*), 0u));}));
_LL867:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1385=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1385[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1386; _temp1386.tag=
Cyc_Absyn_Offsetof_e; _temp1386.f1=( void*) Cyc_Toc_typ_to_c_array( _temp990);
_temp1386.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1387=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1387[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1388; _temp1388.tag=
Cyc_Absyn_StructField; _temp1388.f1= _temp988; _temp1388;}); _temp1387;}));
_temp1386;}); _temp1385;}))); goto _LL823; _LL869:{ void* _temp1389= Cyc_Tcutil_compress(
_temp996); struct Cyc_Absyn_Structdecl** _temp1405; struct Cyc_List_List*
_temp1407; struct Cyc_Absyn_Uniondecl** _temp1409; struct Cyc_List_List*
_temp1411; _LL1391: if(( unsigned int) _temp1389 >  3u?*(( int*) _temp1389) == 
Cyc_Absyn_StructType: 0){ _LL1406: _temp1405=(( struct Cyc_Absyn_StructType_struct*)
_temp1389)->f3; goto _LL1392;} else{ goto _LL1393;} _LL1393: if(( unsigned int)
_temp1389 >  3u?*(( int*) _temp1389) ==  Cyc_Absyn_AnonStructType: 0){ _LL1408:
_temp1407=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1389)->f1; goto
_LL1394;} else{ goto _LL1395;} _LL1395: if(( unsigned int) _temp1389 >  3u?*((
int*) _temp1389) ==  Cyc_Absyn_UnionType: 0){ _LL1410: _temp1409=(( struct Cyc_Absyn_UnionType_struct*)
_temp1389)->f3; goto _LL1396;} else{ goto _LL1397;} _LL1397: if(( unsigned int)
_temp1389 >  3u?*(( int*) _temp1389) ==  Cyc_Absyn_AnonUnionType: 0){ _LL1412:
_temp1411=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1389)->f1; goto
_LL1398;} else{ goto _LL1399;} _LL1399: if(( unsigned int) _temp1389 >  3u?*((
int*) _temp1389) ==  Cyc_Absyn_TupleType: 0){ goto _LL1400;} else{ goto _LL1401;}
_LL1401: if(( unsigned int) _temp1389 >  3u?*(( int*) _temp1389) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL1402;} else{ goto _LL1403;} _LL1403: goto _LL1404; _LL1392: if(
_temp1405 ==  0? 1:(* _temp1405)->fields ==  0){( int) _throw(({ void* _temp1413[
0u]={}; Cyc_Toc_toc_impos( _tag_arr("struct fields must be known", sizeof(
unsigned char), 28u), _tag_arr( _temp1413, sizeof( void*), 0u));}));} _temp1407=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp1405)->fields))->v;
goto _LL1394; _LL1394: { struct Cyc_Absyn_Structfield* _temp1414=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1407,( int) _temp994);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1415=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1415[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1416; _temp1416.tag=
Cyc_Absyn_Offsetof_e; _temp1416.f1=( void*) Cyc_Toc_typ_to_c_array( _temp996);
_temp1416.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1417=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1417[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1418; _temp1418.tag=
Cyc_Absyn_StructField; _temp1418.f1= _temp1414->name; _temp1418;}); _temp1417;}));
_temp1416;}); _temp1415;}))); goto _LL1390;} _LL1396: if( _temp1409 ==  0? 1:(*
_temp1409)->fields ==  0){( int) _throw(({ void* _temp1419[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("union fields must be known", sizeof( unsigned char), 27u), _tag_arr(
_temp1419, sizeof( void*), 0u));}));} _temp1411=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp1409)->fields))->v; goto _LL1398;
_LL1398: { struct Cyc_Absyn_Structfield* _temp1420=(( struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp1411,( int) _temp994);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1421=(
struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1421[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1422; _temp1422.tag=
Cyc_Absyn_Offsetof_e; _temp1422.f1=( void*) Cyc_Toc_typ_to_c_array( _temp996);
_temp1422.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1423=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1423[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1424; _temp1424.tag=
Cyc_Absyn_StructField; _temp1424.f1= _temp1420->name; _temp1424;}); _temp1423;}));
_temp1422;}); _temp1421;}))); goto _LL1390;} _LL1400:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1425=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1425[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1426; _temp1426.tag= Cyc_Absyn_Offsetof_e;
_temp1426.f1=( void*) Cyc_Toc_typ_to_c_array( _temp996); _temp1426.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1427=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1427[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1428; _temp1428.tag= Cyc_Absyn_StructField;
_temp1428.f1= Cyc_Absyn_fieldname(( int)( _temp994 +  1)); _temp1428;});
_temp1427;})); _temp1426;}); _temp1425;}))); goto _LL1390; _LL1402: if( _temp994
==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Offsetof_e_struct*
_temp1429=( struct Cyc_Absyn_Offsetof_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct));
_temp1429[ 0]=({ struct Cyc_Absyn_Offsetof_e_struct _temp1430; _temp1430.tag=
Cyc_Absyn_Offsetof_e; _temp1430.f1=( void*) Cyc_Toc_typ_to_c_array( _temp996);
_temp1430.f2=( void*)(( void*)({ struct Cyc_Absyn_StructField_struct* _temp1431=(
struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp1431[ 0]=({ struct Cyc_Absyn_StructField_struct _temp1432; _temp1432.tag=
Cyc_Absyn_StructField; _temp1432.f1= Cyc_Toc_tag_sp; _temp1432;}); _temp1431;}));
_temp1430;}); _temp1429;})));} else{( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1433=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1433[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1434; _temp1434.tag= Cyc_Absyn_Offsetof_e;
_temp1434.f1=( void*) Cyc_Toc_typ_to_c_array( _temp996); _temp1434.f2=( void*)((
void*)({ struct Cyc_Absyn_StructField_struct* _temp1435=( struct Cyc_Absyn_StructField_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructField_struct)); _temp1435[ 0]=({
struct Cyc_Absyn_StructField_struct _temp1436; _temp1436.tag= Cyc_Absyn_StructField;
_temp1436.f1= Cyc_Absyn_fieldname(( int) _temp994); _temp1436;}); _temp1435;}));
_temp1434;}); _temp1433;})));} goto _LL1390; _LL1404:( int) _throw(({ void*
_temp1437[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("impossible type for offsetof tuple index",
sizeof( unsigned char), 41u), _tag_arr( _temp1437, sizeof( void*), 0u));}));
_LL1390:;} goto _LL823; _LL871: { void* _temp1438= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp998->topt))->v);{ void* _temp1439=
_temp1438; struct Cyc_Absyn_PtrInfo _temp1445; struct Cyc_Absyn_Conref*
_temp1447; struct Cyc_Absyn_Tqual _temp1449; struct Cyc_Absyn_Conref* _temp1451;
void* _temp1453; void* _temp1455; _LL1441: if(( unsigned int) _temp1439 >  3u?*((
int*) _temp1439) ==  Cyc_Absyn_PointerType: 0){ _LL1446: _temp1445=(( struct Cyc_Absyn_PointerType_struct*)
_temp1439)->f1; _LL1456: _temp1455=( void*) _temp1445.elt_typ; goto _LL1454;
_LL1454: _temp1453=( void*) _temp1445.rgn_typ; goto _LL1452; _LL1452: _temp1451=
_temp1445.nullable; goto _LL1450; _LL1450: _temp1449= _temp1445.tq; goto _LL1448;
_LL1448: _temp1447= _temp1445.bounds; goto _LL1442;} else{ goto _LL1443;}
_LL1443: goto _LL1444; _LL1442:{ void* _temp1457= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1447); _LL1459: if(( unsigned int) _temp1457 >  1u?*(( int*) _temp1457) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1460;} else{ goto _LL1461;} _LL1461: if(
_temp1457 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1462;} else{ goto _LL1458;}
_LL1460: { int do_null_check= Cyc_Toc_need_null_check( _temp998); Cyc_Toc_exp_to_c(
nv, _temp998); if( do_null_check){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp998->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1463=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1463->hd=( void*) _temp998; _temp1463->tl= 0;
_temp1463;}), 0), 0), 0))->r));} goto _LL1458;} _LL1462: { struct Cyc_Absyn_Exp*
_temp1464= Cyc_Absyn_uint_exp( 0, 0); _temp1464->topt=({ struct Cyc_Core_Opt*
_temp1465=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1465->v=( void*) Cyc_Absyn_uint_t; _temp1465;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp998, _temp1464, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1458;} _LL1458:;} goto _LL1440; _LL1444:({ void* _temp1466[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1466, sizeof( void*), 0u));}); goto _LL1440; _LL1440:;} goto _LL823;}
_LL873: Cyc_Toc_exp_to_c( nv, _temp1002); if( Cyc_Toc_is_poly_project( e)){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL823; _LL875: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1006->topt))->v); int
do_null_check= Cyc_Toc_need_null_check( _temp1006); Cyc_Toc_exp_to_c( nv,
_temp1006);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1469; struct Cyc_Absyn_Tqual _temp1471; struct Cyc_Absyn_Conref* _temp1473;
void* _temp1475; void* _temp1477; struct Cyc_Absyn_PtrInfo _temp1467= Cyc_Toc_get_ptr_type(
e1typ); _LL1478: _temp1477=( void*) _temp1467.elt_typ; goto _LL1476; _LL1476:
_temp1475=( void*) _temp1467.rgn_typ; goto _LL1474; _LL1474: _temp1473=
_temp1467.nullable; goto _LL1472; _LL1472: _temp1471= _temp1467.tq; goto _LL1470;
_LL1470: _temp1469= _temp1467.bounds; goto _LL1468; _LL1468:{ void* _temp1479=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1469); struct Cyc_Absyn_Exp*
_temp1485; _LL1481: if(( unsigned int) _temp1479 >  1u?*(( int*) _temp1479) == 
Cyc_Absyn_Upper_b: 0){ _LL1486: _temp1485=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1479)->f1; goto _LL1482;} else{ goto _LL1483;} _LL1483: if( _temp1479 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1484;} else{ goto _LL1480;} _LL1482: if(
Cyc_Evexp_eval_const_uint_exp( _temp1485) <  1){({ void* _temp1487[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1487, sizeof( void*), 0u));});} if( do_null_check){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1006->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1488=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1488->hd=( void*) _temp1006;
_temp1488->tl= 0; _temp1488;}), 0), 0), _temp1004, 0))->r));} goto _LL1480;
_LL1484: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1477);( void*)( _temp1006->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1471), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1489[ 3u];
_temp1489[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1489[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1489[ 0u]= Cyc_Absyn_copy_exp( _temp1006);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1489, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1480;} _LL1480:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL823;}} _LL877: { void* _temp1490= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1010->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1010); Cyc_Toc_exp_to_c( nv, _temp1008);{ void* _temp1491= _temp1490;
struct Cyc_List_List* _temp1499; struct Cyc_Absyn_PtrInfo _temp1501; struct Cyc_Absyn_Conref*
_temp1503; struct Cyc_Absyn_Tqual _temp1505; struct Cyc_Absyn_Conref* _temp1507;
void* _temp1509; void* _temp1511; _LL1493: if(( unsigned int) _temp1491 >  3u?*((
int*) _temp1491) ==  Cyc_Absyn_TupleType: 0){ _LL1500: _temp1499=(( struct Cyc_Absyn_TupleType_struct*)
_temp1491)->f1; goto _LL1494;} else{ goto _LL1495;} _LL1495: if(( unsigned int)
_temp1491 >  3u?*(( int*) _temp1491) ==  Cyc_Absyn_PointerType: 0){ _LL1502:
_temp1501=(( struct Cyc_Absyn_PointerType_struct*) _temp1491)->f1; _LL1512:
_temp1511=( void*) _temp1501.elt_typ; goto _LL1510; _LL1510: _temp1509=( void*)
_temp1501.rgn_typ; goto _LL1508; _LL1508: _temp1507= _temp1501.nullable; goto
_LL1506; _LL1506: _temp1505= _temp1501.tq; goto _LL1504; _LL1504: _temp1503=
_temp1501.bounds; goto _LL1496;} else{ goto _LL1497;} _LL1497: goto _LL1498;
_LL1494: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1008) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1010, Cyc_Absyn_fieldname((
int) i), 0))->r)); goto _LL1492;} _LL1496:{ void* _temp1513= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1503); struct Cyc_Absyn_Exp* _temp1519; _LL1515: if((
unsigned int) _temp1513 >  1u?*(( int*) _temp1513) ==  Cyc_Absyn_Upper_b: 0){
_LL1520: _temp1519=(( struct Cyc_Absyn_Upper_b_struct*) _temp1513)->f1; goto
_LL1516;} else{ goto _LL1517;} _LL1517: if( _temp1513 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1518;} else{ goto _LL1514;} _LL1516: _temp1519= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1519), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1507); void* ta1= Cyc_Toc_typ_to_c( _temp1511);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1505); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1521[ 4u];
_temp1521[ 3u]= _temp1008; _temp1521[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1521[ 1u]= _temp1519; _temp1521[ 0u]= _temp1010;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1521, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp1008->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1522[ 2u];
_temp1522[ 1u]= Cyc_Absyn_copy_exp( _temp1008); _temp1522[ 0u]= _temp1519;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1522, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1514;}
_LL1518: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1511);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1505), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1523[ 3u]; _temp1523[ 2u]= _temp1008; _temp1523[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1523[ 0u]= _temp1010;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1523, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1514;} _LL1514:;} goto _LL1492; _LL1498:({
void* _temp1524[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1524, sizeof( void*), 0u));}); goto
_LL1492; _LL1492:;} goto _LL823;} _LL879: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1012))->r));} else{ struct
Cyc_List_List* _temp1525=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp1012); struct _tagged_arr* _temp1526= Cyc_Toc_add_tuple_type( _temp1525);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1012 !=  0;( _temp1012=
_temp1012->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _temp1012->hd);
dles=({ struct Cyc_List_List* _temp1527=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1527->hd=( void*)({ struct _tuple4*
_temp1528=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1528->f1=
0; _temp1528->f2=( struct Cyc_Absyn_Exp*) _temp1012->hd; _temp1528;}); _temp1527->tl=
dles; _temp1527;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));} goto _LL823; _LL881:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1014, 0))->r));{ struct Cyc_List_List* _temp1529= _temp1014; for( 0;
_temp1529 !=  0; _temp1529= _temp1529->tl){ struct _tuple4 _temp1532; struct Cyc_Absyn_Exp*
_temp1533; struct _tuple4* _temp1530=( struct _tuple4*) _temp1529->hd; _temp1532=*
_temp1530; _LL1534: _temp1533= _temp1532.f2; goto _LL1531; _LL1531: Cyc_Toc_exp_to_c(
nv, _temp1533);}} goto _LL823; _LL883: { unsigned int _temp1535= Cyc_Evexp_eval_const_uint_exp(
_temp1018); Cyc_Toc_exp_to_c( nv, _temp1016);{ struct Cyc_List_List* es= 0; if(
! Cyc_Toc_is_zero( _temp1016)){ unsigned int i= 0; for( 0; i <  _temp1535; i ++){
es=({ struct Cyc_List_List* _temp1536=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1536->hd=( void*)({ struct _tuple4*
_temp1537=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1537->f1=
0; _temp1537->f2= _temp1016; _temp1537;}); _temp1536->tl= es; _temp1536;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL823;}} _LL885: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, 0, _temp1024, _temp1028))->r));} else{ if( _temp1022 ==  0){({
void* _temp1538[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer", sizeof(
unsigned char), 37u), _tag_arr( _temp1538, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1022); struct
_RegionHandle _temp1539= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1539; _push_region( rgn);{ struct Cyc_List_List* _temp1540=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1024,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); struct Cyc_List_List* _temp1541= 0;{ struct Cyc_List_List*
_temp1542=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1542 !=  0; _temp1542= _temp1542->tl){ struct Cyc_List_List*
_temp1543= _temp1540; for( 0; _temp1543 !=  0; _temp1543= _temp1543->tl){ if((*((
struct _tuple10*) _temp1543->hd)).f1 == ( struct Cyc_Absyn_Structfield*)
_temp1542->hd){ struct _tuple10 _temp1546; struct Cyc_Absyn_Exp* _temp1547;
struct Cyc_Absyn_Structfield* _temp1549; struct _tuple10* _temp1544=( struct
_tuple10*) _temp1543->hd; _temp1546=* _temp1544; _LL1550: _temp1549= _temp1546.f1;
goto _LL1548; _LL1548: _temp1547= _temp1546.f2; goto _LL1545; _LL1545: { void*
_temp1551=( void*) _temp1549->type; Cyc_Toc_exp_to_c( nv, _temp1547); if( Cyc_Toc_is_void_star(
_temp1551)){( void*)( _temp1547->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1547->r, 0), 0))->r));} _temp1541=({ struct Cyc_List_List*
_temp1552=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1552->hd=( void*)({ struct _tuple4* _temp1553=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1553->f1= 0; _temp1553->f2= _temp1547; _temp1553;});
_temp1552->tl= _temp1541; _temp1552;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1541), 0))->r));}; _pop_region( rgn);}} goto _LL823;
_LL887: { struct Cyc_List_List* fs;{ void* _temp1554= Cyc_Tcutil_compress(
_temp1032); struct Cyc_List_List* _temp1560; _LL1556: if(( unsigned int)
_temp1554 >  3u?*(( int*) _temp1554) ==  Cyc_Absyn_AnonStructType: 0){ _LL1561:
_temp1560=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1554)->f1; goto
_LL1557;} else{ goto _LL1558;} _LL1558: goto _LL1559; _LL1557: fs= _temp1560;
goto _LL1555; _LL1559: fs=({ struct Cyc_Std_String_pa_struct _temp1563;
_temp1563.tag= Cyc_Std_String_pa; _temp1563.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1032);{ void* _temp1562[ 1u]={& _temp1563};(( struct Cyc_List_List*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("anon struct has type %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1562, sizeof( void*), 1u));}});
goto _LL1555; _LL1555:;}{ struct _RegionHandle _temp1564= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1564; _push_region( rgn);{ struct Cyc_List_List*
_temp1565=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1030, fs); for( 0; _temp1565 !=  0; _temp1565= _temp1565->tl){
struct _tuple10 _temp1568; struct Cyc_Absyn_Exp* _temp1569; struct Cyc_Absyn_Structfield*
_temp1571; struct _tuple10* _temp1566=( struct _tuple10*) _temp1565->hd;
_temp1568=* _temp1566; _LL1572: _temp1571= _temp1568.f1; goto _LL1570; _LL1570:
_temp1569= _temp1568.f2; goto _LL1567; _LL1567: { void* _temp1573=( void*)
_temp1571->type; Cyc_Toc_exp_to_c( nv, _temp1569); if( Cyc_Toc_is_void_star(
_temp1573)){( void*)( _temp1569->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1569->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1030, 0))->r));}; _pop_region( rgn);}
goto _LL823;} _LL889: { struct _tuple0* qv= _temp1034->name; if( _temp1036->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1574= _temp1036->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1036->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1574))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1574->hd)->typs ==  0){ tag_count ++;} _temp1574= _temp1574->tl;}( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL823;} _LL891: { struct _tuple0* _temp1575= _temp1040->name; struct Cyc_List_List*
_temp1576= _temp1040->typs; struct _tuple0* _temp1577= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1578= Cyc_Absyn_var_exp( _temp1577, 0); void*
_temp1579= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1575, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1042->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1575, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1580= _temp1042->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1042->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1575,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1580))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)
_temp1580->hd)->typs !=  0){ tag_count ++;} _temp1580= _temp1580->tl;} tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles=
0; for( 0; _temp1044 !=  0;( _temp1044= _temp1044->tl, _temp1576= _temp1576->tl)){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1044->hd; void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1576))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1581=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1581->hd=( void*)({ struct _tuple4* _temp1582=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1582->f1= 0; _temp1582->f2= cur_e; _temp1582;});
_temp1581->tl= dles; _temp1581;});} dles=({ struct Cyc_List_List* _temp1583=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1583->hd=(
void*)({ struct _tuple4* _temp1584=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1584->f1= 0; _temp1584->f2= tag_exp; _temp1584;}); _temp1583->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1583;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1585=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1585->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1575,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1585;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1586=({ struct Cyc_List_List* _temp1592=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1592->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1578, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1575, 0), 0); _temp1592->tl= 0; _temp1592;});{ int i=
1; for( 0; _temp1044 !=  0;((( _temp1044= _temp1044->tl, i ++)), _temp1576=
_temp1576->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1044->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1576))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1587= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1578, Cyc_Absyn_fieldname(
i), 0), cur_e, 0); _temp1586=({ struct Cyc_List_List* _temp1588=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1588->hd=( void*) _temp1587;
_temp1588->tl= _temp1586; _temp1588;});}}}{ struct Cyc_Absyn_Stmt* _temp1589=
Cyc_Absyn_exp_stmt( _temp1578, 0); struct Cyc_Absyn_Stmt* _temp1590= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1591=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1591->hd=( void*) _temp1589; _temp1591->tl= _temp1586; _temp1591;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1577, _temp1579, 0, _temp1590, 0), 0))->r));}} goto _LL823;} _LL893: goto
_LL823; _LL895: goto _LL823; _LL897: { void* t_c= Cyc_Toc_typ_to_c( _temp1046);
if( _temp1048 !=  0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)
_check_null( _temp1048); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1046, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL823;} _LL899: Cyc_Toc_stmt_to_c( nv, _temp1050); goto
_LL823; _LL901:({ void* _temp1593[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem", sizeof(
unsigned char), 14u), _tag_arr( _temp1593, sizeof( void*), 0u));}); goto _LL823;
_LL903:({ void* _temp1594[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof( unsigned char),
12u), _tag_arr( _temp1594, sizeof( void*), 0u));}); goto _LL823; _LL905:({ void*
_temp1595[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("codegen", sizeof( unsigned char), 8u), _tag_arr( _temp1595, sizeof(
void*), 0u));}); goto _LL823; _LL907:({ void* _temp1596[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u), _tag_arr( _temp1596, sizeof( void*), 0u));}); goto
_LL823; _LL823:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1597=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1635; struct Cyc_Absyn_Vardecl _temp1637; struct _tuple0* _temp1638; struct
Cyc_Absyn_Vardecl* _temp1640; int _temp1642; void* _temp1644; unsigned char
_temp1646; struct _tagged_arr _temp1648; struct Cyc_Absyn_Enumfield* _temp1650;
struct Cyc_Absyn_Enumdecl* _temp1652; struct Cyc_Absyn_Enumfield* _temp1654;
void* _temp1656; struct Cyc_List_List* _temp1658; struct Cyc_List_List*
_temp1660; struct Cyc_Absyn_Tunionfield* _temp1662; struct Cyc_Absyn_Tuniondecl*
_temp1664; struct Cyc_Absyn_Pat* _temp1667; struct Cyc_Absyn_Pat _temp1669; void*
_temp1670; struct Cyc_List_List* _temp1672; struct Cyc_List_List* _temp1674;
struct Cyc_Absyn_Tunionfield* _temp1676; struct Cyc_Absyn_Tuniondecl* _temp1678;
struct Cyc_List_List* _temp1680; struct Cyc_List_List* _temp1682; struct Cyc_List_List*
_temp1684; struct Cyc_List_List* _temp1686; struct Cyc_Core_Opt* _temp1688;
struct Cyc_Absyn_Structdecl* _temp1690; struct Cyc_Absyn_Pat* _temp1692; _LL1599:
if(( unsigned int) _temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_Var_p: 0){
_LL1636: _temp1635=(( struct Cyc_Absyn_Var_p_struct*) _temp1597)->f1; _temp1637=*
_temp1635; _LL1639: _temp1638= _temp1637.name; goto _LL1600;} else{ goto _LL1601;}
_LL1601: if( _temp1597 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1602;} else{ goto
_LL1603;} _LL1603: if(( unsigned int) _temp1597 >  2u?*(( int*) _temp1597) == 
Cyc_Absyn_Reference_p: 0){ _LL1641: _temp1640=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1597)->f1; goto _LL1604;} else{ goto _LL1605;} _LL1605: if( _temp1597 == (
void*) Cyc_Absyn_Null_p){ goto _LL1606;} else{ goto _LL1607;} _LL1607: if((
unsigned int) _temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_Int_p: 0){
_LL1645: _temp1644=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1597)->f1;
goto _LL1643; _LL1643: _temp1642=(( struct Cyc_Absyn_Int_p_struct*) _temp1597)->f2;
goto _LL1608;} else{ goto _LL1609;} _LL1609: if(( unsigned int) _temp1597 >  2u?*((
int*) _temp1597) ==  Cyc_Absyn_Char_p: 0){ _LL1647: _temp1646=(( struct Cyc_Absyn_Char_p_struct*)
_temp1597)->f1; goto _LL1610;} else{ goto _LL1611;} _LL1611: if(( unsigned int)
_temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_Float_p: 0){ _LL1649:
_temp1648=(( struct Cyc_Absyn_Float_p_struct*) _temp1597)->f1; goto _LL1612;}
else{ goto _LL1613;} _LL1613: if(( unsigned int) _temp1597 >  2u?*(( int*)
_temp1597) ==  Cyc_Absyn_Enum_p: 0){ _LL1653: _temp1652=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1597)->f1; goto _LL1651; _LL1651: _temp1650=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1597)->f2; goto _LL1614;} else{ goto _LL1615;} _LL1615: if(( unsigned int)
_temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1657:
_temp1656=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1597)->f1; goto
_LL1655; _LL1655: _temp1654=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1597)->f2;
goto _LL1616;} else{ goto _LL1617;} _LL1617: if(( unsigned int) _temp1597 >  2u?*((
int*) _temp1597) ==  Cyc_Absyn_Tunion_p: 0){ _LL1665: _temp1664=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1597)->f1; goto _LL1663; _LL1663: _temp1662=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1597)->f2; goto _LL1661; _LL1661: _temp1660=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1597)->f3; goto _LL1659; _LL1659: _temp1658=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1597)->f4; if( _temp1658 ==  0){ goto _LL1618;} else{ goto _LL1619;}} else{
goto _LL1619;} _LL1619: if(( unsigned int) _temp1597 >  2u?*(( int*) _temp1597)
==  Cyc_Absyn_Pointer_p: 0){ _LL1668: _temp1667=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1597)->f1; _temp1669=* _temp1667; _LL1671: _temp1670=( void*) _temp1669.r;
if(( unsigned int) _temp1670 >  2u?*(( int*) _temp1670) ==  Cyc_Absyn_Tunion_p:
0){ _LL1679: _temp1678=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1670)->f1;
goto _LL1677; _LL1677: _temp1676=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1670)->f2;
goto _LL1675; _LL1675: _temp1674=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1670)->f3;
goto _LL1673; _LL1673: _temp1672=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1670)->f4;
goto _LL1666;} else{ goto _LL1621;}} else{ goto _LL1621;} _LL1666: if( _temp1672
!=  0){ goto _LL1620;} else{ goto _LL1621;} _LL1621: if(( unsigned int)
_temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_Tunion_p: 0){ _LL1681:
_temp1680=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1597)->f4; goto _LL1622;}
else{ goto _LL1623;} _LL1623: if(( unsigned int) _temp1597 >  2u?*(( int*)
_temp1597) ==  Cyc_Absyn_Tuple_p: 0){ _LL1683: _temp1682=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1597)->f1; goto _LL1624;} else{ goto _LL1625;} _LL1625: if(( unsigned int)
_temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_Struct_p: 0){ _LL1691:
_temp1690=(( struct Cyc_Absyn_Struct_p_struct*) _temp1597)->f1; goto _LL1689;
_LL1689: _temp1688=(( struct Cyc_Absyn_Struct_p_struct*) _temp1597)->f2; goto
_LL1687; _LL1687: _temp1686=(( struct Cyc_Absyn_Struct_p_struct*) _temp1597)->f3;
goto _LL1685; _LL1685: _temp1684=(( struct Cyc_Absyn_Struct_p_struct*) _temp1597)->f4;
goto _LL1626;} else{ goto _LL1627;} _LL1627: if(( unsigned int) _temp1597 >  2u?*((
int*) _temp1597) ==  Cyc_Absyn_Pointer_p: 0){ _LL1693: _temp1692=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1597)->f1; goto _LL1628;} else{ goto _LL1629;} _LL1629: if(( unsigned int)
_temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1630;}
else{ goto _LL1631;} _LL1631: if(( unsigned int) _temp1597 >  2u?*(( int*)
_temp1597) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1632;} else{ goto _LL1633;}
_LL1633: if(( unsigned int) _temp1597 >  2u?*(( int*) _temp1597) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1634;} else{ goto _LL1598;} _LL1600: nv= Cyc_Toc_add_varmap( nv,
_temp1638, r); goto _LL1602; _LL1602: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1598; _LL1604: { struct _tuple0* _temp1694= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1695=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1695->hd=( void*)({ struct _tuple12*
_temp1696=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1696->f1= _temp1694; _temp1696->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1696;}); _temp1695->tl= decls; _temp1695;}); nv= Cyc_Toc_add_varmap(
nv, _temp1640->name, Cyc_Absyn_var_exp( _temp1694, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1694, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1598;} _LL1606: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1598; _LL1608:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1644, _temp1642, 0),
succ_lab, fail_lab); goto _LL1598; _LL1610: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1646, 0), succ_lab, fail_lab); goto _LL1598; _LL1612: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1648, 0), succ_lab, fail_lab); goto _LL1598;
_LL1614: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1697=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1697[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1698; _temp1698.tag= Cyc_Absyn_Enum_e;
_temp1698.f1= _temp1650->name; _temp1698.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1652; _temp1698.f3=( struct Cyc_Absyn_Enumfield*) _temp1650; _temp1698;});
_temp1697;}), 0), succ_lab, fail_lab); goto _LL1598; _LL1616: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_AnonEnum_e_struct* _temp1699=(
struct Cyc_Absyn_AnonEnum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_e_struct));
_temp1699[ 0]=({ struct Cyc_Absyn_AnonEnum_e_struct _temp1700; _temp1700.tag=
Cyc_Absyn_AnonEnum_e; _temp1700.f1= _temp1654->name; _temp1700.f2=( void*)
_temp1656; _temp1700.f3=( struct Cyc_Absyn_Enumfield*) _temp1654; _temp1700;});
_temp1699;}), 0), succ_lab, fail_lab); goto _LL1598; _LL1618: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1662->name, 0); if( ! _temp1664->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1598;} _LL1620: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1672);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1676->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1701=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1676->typs); struct Cyc_List_List*
_temp1702=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1672); for( 0; _temp1702 !=  0;((( _temp1702=
_temp1702->tl, _temp1701=(( struct Cyc_List_List*) _check_null( _temp1701))->tl)),
cnt --)){ struct Cyc_Absyn_Pat* _temp1703=( struct Cyc_Absyn_Pat*) _temp1702->hd;
if(( void*) _temp1703->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1704=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1701))->hd)).f2;
struct _tuple0* _temp1705= Cyc_Toc_temp_var(); void* _temp1706=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1703->topt))->v; void* _temp1707= Cyc_Toc_typ_to_c(
_temp1706); struct _tagged_arr* _temp1708= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1709= Cyc_Absyn_structarrow_exp( rcast, Cyc_Absyn_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1704))){ _temp1709= Cyc_Absyn_cast_exp(
_temp1707, _temp1709, 0);} decls=({ struct Cyc_List_List* _temp1710=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1710->hd=( void*)({
struct _tuple12* _temp1711=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1711->f1= _temp1705; _temp1711->f2= _temp1707; _temp1711;});
_temp1710->tl= decls; _temp1710;});{ struct _tuple11 _temp1712= Cyc_Toc_xlate_pat(
nv, rgn, _temp1706, Cyc_Absyn_var_exp( _temp1705, 0), _temp1709, _temp1703,
succ_lab, fail_lab, decls); nv= _temp1712.f1; decls= _temp1712.f2;{ struct Cyc_Absyn_Stmt*
_temp1713= _temp1712.f3; struct Cyc_Absyn_Stmt* _temp1714= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1705, 0), _temp1709, 0); struct Cyc_Absyn_Stmt*
_temp1715= Cyc_Absyn_seq_stmt( _temp1714, _temp1713, 0); ss=({ struct Cyc_List_List*
_temp1716=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1716->hd=( void*) Cyc_Absyn_label_stmt( _temp1708, _temp1715, 0); _temp1716->tl=
ss; _temp1716;}); succ_lab= _temp1708;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1717=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1717->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1717->tl= 0;
_temp1717;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1678->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1676->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1718=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1678->fields))->v; for( 0; _temp1718 !=  0; _temp1718=
_temp1718->tl){ struct Cyc_Absyn_Tunionfield* _temp1719=( struct Cyc_Absyn_Tunionfield*)
_temp1718->hd; if( _temp1719->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1676->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1598;}} _LL1622: _temp1682= _temp1680; goto _LL1624; _LL1624: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1682);{ struct Cyc_List_List* _temp1720=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1682); for(
0; _temp1720 !=  0;( _temp1720= _temp1720->tl, cnt --)){ struct Cyc_Absyn_Pat*
_temp1721=( struct Cyc_Absyn_Pat*) _temp1720->hd; if(( void*) _temp1721->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tuple0* _temp1722= Cyc_Toc_temp_var();
void* _temp1723=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1721->topt))->v;
struct _tagged_arr* _temp1724= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1725=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1725->hd=( void*)({ struct _tuple12* _temp1726=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1726->f1= _temp1722;
_temp1726->f2= Cyc_Toc_typ_to_c( _temp1723); _temp1726;}); _temp1725->tl= decls;
_temp1725;});{ struct _tuple11 _temp1727= Cyc_Toc_xlate_pat( nv, rgn, _temp1723,
Cyc_Absyn_var_exp( _temp1722, 0), Cyc_Absyn_structmember_exp( path, Cyc_Absyn_fieldname(
cnt), 0), _temp1721, succ_lab, fail_lab, decls); nv= _temp1727.f1; decls=
_temp1727.f2;{ struct Cyc_Absyn_Stmt* _temp1728= _temp1727.f3; struct Cyc_Absyn_Stmt*
_temp1729= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1722, 0), Cyc_Absyn_structmember_exp( r, Cyc_Absyn_fieldname( cnt), 0), 0),
_temp1728, 0); ss=({ struct Cyc_List_List* _temp1730=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1730->hd=( void*) Cyc_Absyn_label_stmt(
_temp1724, _temp1729, 0); _temp1730->tl= ss; _temp1730;}); succ_lab= _temp1724;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1731=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1731->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1731->tl= 0; _temp1731;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1598;} _LL1626: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1732=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1684); for( 0; _temp1732 !=  0; _temp1732=
_temp1732->tl){ struct _tuple13* _temp1733=( struct _tuple13*) _temp1732->hd;
struct Cyc_Absyn_Pat* _temp1734=(* _temp1733).f2; if(( void*) _temp1734->r == (
void*) Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1735=(
void*)(( struct Cyc_List_List*) _check_null((* _temp1733).f1))->hd; struct
_tagged_arr* _temp1741; _LL1737: if(*(( int*) _temp1735) ==  Cyc_Absyn_FieldName){
_LL1742: _temp1741=(( struct Cyc_Absyn_FieldName_struct*) _temp1735)->f1; goto
_LL1738;} else{ goto _LL1739;} _LL1739: goto _LL1740; _LL1738: f= _temp1741;
goto _LL1736; _LL1740:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1736:;}{
struct _tuple0* _temp1743= Cyc_Toc_temp_var(); void* _temp1744=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1734->topt))->v; void* _temp1745= Cyc_Toc_typ_to_c(
_temp1744); struct _tagged_arr* _temp1746= Cyc_Toc_fresh_label(); decls=({
struct Cyc_List_List* _temp1747=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1747->hd=( void*)({ struct _tuple12*
_temp1748=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1748->f1= _temp1743; _temp1748->f2= _temp1745; _temp1748;}); _temp1747->tl=
decls; _temp1747;});{ struct _tuple11 _temp1749= Cyc_Toc_xlate_pat( nv, rgn,
_temp1744, Cyc_Absyn_var_exp( _temp1743, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp1734, succ_lab, fail_lab, decls); nv= _temp1749.f1; decls= _temp1749.f2;{
struct Cyc_Absyn_Exp* _temp1750= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1690->fields))->v,
f)))->type)){ _temp1750= Cyc_Absyn_cast_exp( _temp1745, _temp1750, 0);}{ struct
Cyc_Absyn_Stmt* _temp1751= _temp1749.f3; struct Cyc_Absyn_Stmt* _temp1752= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1743, 0), _temp1750, 0),
_temp1751, 0); ss=({ struct Cyc_List_List* _temp1753=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1753->hd=( void*) Cyc_Absyn_label_stmt(
_temp1746, _temp1752, 0); _temp1753->tl= ss; _temp1753;}); succ_lab= _temp1746;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1754=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1754->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1754->tl= 0; _temp1754;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1598;} _LL1628: { struct _tuple0* _temp1755= Cyc_Toc_temp_var(); void*
_temp1756=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1692->topt))->v;
decls=({ struct Cyc_List_List* _temp1757=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1757->hd=( void*)({ struct _tuple12*
_temp1758=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1758->f1= _temp1755; _temp1758->f2= Cyc_Toc_typ_to_c( _temp1756); _temp1758;});
_temp1757->tl= decls; _temp1757;});{ struct _tuple11 _temp1759= Cyc_Toc_xlate_pat(
nv, rgn, _temp1756, Cyc_Absyn_var_exp( _temp1755, 0), Cyc_Absyn_deref_exp( path,
0), _temp1692, succ_lab, fail_lab, decls); nv= _temp1759.f1; decls= _temp1759.f2;{
struct Cyc_Absyn_Stmt* _temp1760= _temp1759.f3; struct Cyc_Absyn_Stmt* _temp1761=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1755, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1760, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1761, 0);} else{ s= _temp1761;} goto _LL1598;}}} _LL1630: s=({
void* _temp1762[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1762, sizeof( void*), 0u));}); goto _LL1598;
_LL1632: s=({ void* _temp1763[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1763, sizeof( void*), 0u));}); goto
_LL1598; _LL1634: s=({ void* _temp1764[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1764, sizeof( void*), 0u));}); goto
_LL1598; _LL1598:;} return({ struct _tuple11 _temp1765; _temp1765.f1= nv;
_temp1765.f2= decls; _temp1765.f3= s; _temp1765;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1766=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1766->f1= Cyc_Toc_fresh_label(); _temp1766->f2=
Cyc_Toc_fresh_label(); _temp1766->f3= sc; _temp1766;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1767=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1768= Cyc_Tcutil_compress( _temp1767); _LL1770: if(( unsigned int)
_temp1768 >  3u?*(( int*) _temp1768) ==  Cyc_Absyn_IntType: 0){ goto _LL1771;}
else{ goto _LL1772;} _LL1772: if(( unsigned int) _temp1768 >  3u?*(( int*)
_temp1768) ==  Cyc_Absyn_EnumType: 0){ goto _LL1773;} else{ goto _LL1774;}
_LL1774: goto _LL1775; _LL1771: goto _LL1773; _LL1773: leave_as_switch= 1; goto
_LL1769; _LL1775: leave_as_switch= 0; goto _LL1769; _LL1769:;}{ struct Cyc_List_List*
_temp1776= scs; for( 0; _temp1776 !=  0; _temp1776= _temp1776->tl){ if(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)
_temp1776->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)
_temp1776->hd)->where_clause !=  0){ leave_as_switch= 0; break;}}} if(
leave_as_switch){ struct _tagged_arr* next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List*
_temp1777= scs; for( 0; _temp1777 !=  0; _temp1777= _temp1777->tl){ struct Cyc_Absyn_Stmt*
_temp1778=(( struct Cyc_Absyn_Switch_clause*) _temp1777->hd)->body;(( struct Cyc_Absyn_Switch_clause*)
_temp1777->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp1778, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp1778);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct _tagged_arr*
end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp1779= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1779; _push_region( rgn);{ struct Cyc_List_List*
lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple14*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*), struct _RegionHandle*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label, rgn,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1780= lscs; for( 0;
_temp1780 !=  0; _temp1780= _temp1780->tl){ struct Cyc_Absyn_Switch_clause* sc=(*((
struct _tuple14*) _temp1780->hd)).f3; struct _tagged_arr* fail_lab= _temp1780->tl
==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(
_temp1780->tl))->hd)).f1; struct _tagged_arr* succ_lab=(*(( struct _tuple14*)
_temp1780->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11 _temp1781= Cyc_Toc_xlate_pat(
nv, rgn, _temp1767, r, path, sc->pattern, succ_lab, fail_lab, decls); nvs=({
struct Cyc_List_List* _temp1782=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1782->hd=( void*) _temp1781.f1; _temp1782->tl=
nvs; _temp1782;}); decls= _temp1781.f2; test_stmts=({ struct Cyc_List_List*
_temp1783=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1783->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple14*) _temp1780->hd)).f1,
_temp1781.f3, 0); _temp1783->tl= test_stmts; _temp1783;});} else{ struct Cyc_Absyn_Exp*
_temp1784=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1785= Cyc_Toc_fresh_label(); struct _tuple11 _temp1786= Cyc_Toc_xlate_pat(
nv, rgn, _temp1767, r, path, sc->pattern, _temp1785, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1786.f1, _temp1784); nvs=({ struct Cyc_List_List* _temp1787=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1787->hd=( void*)
_temp1786.f1; _temp1787->tl= nvs; _temp1787;}); decls= _temp1786.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1784, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1785, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1788=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1788->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*) _temp1780->hd)).f1, Cyc_Absyn_seq_stmt( _temp1786.f3, s3, 0),
0); _temp1788->tl= test_stmts; _temp1788;});}}}}{ struct Cyc_Absyn_Stmt*
_temp1789= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs !=  0){ for( 0; lscs->tl !=  0;( lscs= lscs->tl, nvs= nvs->tl)){
struct Cyc_Toc_Env* _temp1790=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1790, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f2,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs->tl))->hd)).f3)->pat_vars))->v,( struct
Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null( nvs->tl))->hd), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}{ struct Cyc_Toc_Env* _temp1791=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp1791, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*) lscs->hd)).f2,
s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt( _temp1789, Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for(
decls; decls !=  0; decls= decls->tl){ struct _tuple12 _temp1794; void*
_temp1795; struct _tuple0* _temp1797; struct _tuple12* _temp1792=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp1794=*
_temp1792; _LL1798: _temp1797= _temp1794.f1; goto _LL1796; _LL1796: _temp1795=
_temp1794.f2; goto _LL1793; _LL1793: res= Cyc_Absyn_declare_stmt( _temp1797,
_temp1795, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1799=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1799[ 0]=({ struct Cyc_List_List
_temp1800; _temp1800.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1800.tl= 0; _temp1800;}); _temp1799;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1801=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1843; struct Cyc_Absyn_Stmt* _temp1845; struct Cyc_Absyn_Stmt* _temp1847;
struct Cyc_Absyn_Exp* _temp1849; struct Cyc_Absyn_Stmt* _temp1851; struct Cyc_Absyn_Stmt*
_temp1853; struct Cyc_Absyn_Exp* _temp1855; struct Cyc_Absyn_Stmt* _temp1857;
struct _tuple2 _temp1859; struct Cyc_Absyn_Exp* _temp1861; struct Cyc_Absyn_Stmt*
_temp1863; struct Cyc_Absyn_Stmt* _temp1865; struct Cyc_Absyn_Stmt* _temp1867;
struct Cyc_Absyn_Stmt* _temp1869; struct _tuple2 _temp1871; struct Cyc_Absyn_Exp*
_temp1873; struct _tuple2 _temp1875; struct Cyc_Absyn_Exp* _temp1877; struct Cyc_Absyn_Exp*
_temp1879; struct Cyc_List_List* _temp1881; struct Cyc_Absyn_Exp* _temp1883;
struct Cyc_Absyn_Switch_clause** _temp1885; struct Cyc_List_List* _temp1887;
struct Cyc_Absyn_Stmt* _temp1889; struct Cyc_Absyn_Decl* _temp1891; struct Cyc_Absyn_Stmt*
_temp1893; struct _tagged_arr* _temp1895; struct _tuple2 _temp1897; struct Cyc_Absyn_Exp*
_temp1899; struct Cyc_Absyn_Stmt* _temp1901; struct Cyc_List_List* _temp1903;
struct Cyc_Absyn_Stmt* _temp1905; struct Cyc_Absyn_Stmt* _temp1907; struct Cyc_Absyn_Vardecl*
_temp1909; struct Cyc_Absyn_Tvar* _temp1911; _LL1803: if( _temp1801 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1804;} else{ goto _LL1805;} _LL1805: if((
unsigned int) _temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Exp_s: 0){
_LL1844: _temp1843=(( struct Cyc_Absyn_Exp_s_struct*) _temp1801)->f1; goto
_LL1806;} else{ goto _LL1807;} _LL1807: if(( unsigned int) _temp1801 >  1u?*((
int*) _temp1801) ==  Cyc_Absyn_Seq_s: 0){ _LL1848: _temp1847=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1801)->f1; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1801)->f2; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Return_s: 0){ _LL1850:
_temp1849=(( struct Cyc_Absyn_Return_s_struct*) _temp1801)->f1; goto _LL1810;}
else{ goto _LL1811;} _LL1811: if(( unsigned int) _temp1801 >  1u?*(( int*)
_temp1801) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1856: _temp1855=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1801)->f1; goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1801)->f2; goto _LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1801)->f3; goto _LL1812;} else{ goto _LL1813;} _LL1813: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_While_s: 0){ _LL1860:
_temp1859=(( struct Cyc_Absyn_While_s_struct*) _temp1801)->f1; _LL1862:
_temp1861= _temp1859.f1; goto _LL1858; _LL1858: _temp1857=(( struct Cyc_Absyn_While_s_struct*)
_temp1801)->f2; goto _LL1814;} else{ goto _LL1815;} _LL1815: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Break_s: 0){ _LL1864:
_temp1863=(( struct Cyc_Absyn_Break_s_struct*) _temp1801)->f1; goto _LL1816;}
else{ goto _LL1817;} _LL1817: if(( unsigned int) _temp1801 >  1u?*(( int*)
_temp1801) ==  Cyc_Absyn_Continue_s: 0){ _LL1866: _temp1865=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1801)->f1; goto _LL1818;} else{ goto _LL1819;} _LL1819: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Goto_s: 0){ _LL1868:
_temp1867=(( struct Cyc_Absyn_Goto_s_struct*) _temp1801)->f2; goto _LL1820;}
else{ goto _LL1821;} _LL1821: if(( unsigned int) _temp1801 >  1u?*(( int*)
_temp1801) ==  Cyc_Absyn_For_s: 0){ _LL1880: _temp1879=(( struct Cyc_Absyn_For_s_struct*)
_temp1801)->f1; goto _LL1876; _LL1876: _temp1875=(( struct Cyc_Absyn_For_s_struct*)
_temp1801)->f2; _LL1878: _temp1877= _temp1875.f1; goto _LL1872; _LL1872:
_temp1871=(( struct Cyc_Absyn_For_s_struct*) _temp1801)->f3; _LL1874: _temp1873=
_temp1871.f1; goto _LL1870; _LL1870: _temp1869=(( struct Cyc_Absyn_For_s_struct*)
_temp1801)->f4; goto _LL1822;} else{ goto _LL1823;} _LL1823: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Switch_s: 0){ _LL1884:
_temp1883=(( struct Cyc_Absyn_Switch_s_struct*) _temp1801)->f1; goto _LL1882;
_LL1882: _temp1881=(( struct Cyc_Absyn_Switch_s_struct*) _temp1801)->f2; goto
_LL1824;} else{ goto _LL1825;} _LL1825: if(( unsigned int) _temp1801 >  1u?*((
int*) _temp1801) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1888: _temp1887=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1801)->f1; goto _LL1886; _LL1886: _temp1885=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1801)->f2; goto _LL1826;} else{ goto _LL1827;} _LL1827: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Decl_s: 0){ _LL1892:
_temp1891=(( struct Cyc_Absyn_Decl_s_struct*) _temp1801)->f1; goto _LL1890;
_LL1890: _temp1889=(( struct Cyc_Absyn_Decl_s_struct*) _temp1801)->f2; goto
_LL1828;} else{ goto _LL1829;} _LL1829: if(( unsigned int) _temp1801 >  1u?*((
int*) _temp1801) ==  Cyc_Absyn_Label_s: 0){ _LL1896: _temp1895=(( struct Cyc_Absyn_Label_s_struct*)
_temp1801)->f1; goto _LL1894; _LL1894: _temp1893=(( struct Cyc_Absyn_Label_s_struct*)
_temp1801)->f2; goto _LL1830;} else{ goto _LL1831;} _LL1831: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Do_s: 0){ _LL1902: _temp1901=((
struct Cyc_Absyn_Do_s_struct*) _temp1801)->f1; goto _LL1898; _LL1898: _temp1897=((
struct Cyc_Absyn_Do_s_struct*) _temp1801)->f2; _LL1900: _temp1899= _temp1897.f1;
goto _LL1832;} else{ goto _LL1833;} _LL1833: if(( unsigned int) _temp1801 >  1u?*((
int*) _temp1801) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1906: _temp1905=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1801)->f1; goto _LL1904; _LL1904: _temp1903=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1801)->f2; goto _LL1834;} else{ goto _LL1835;} _LL1835: if(( unsigned int)
_temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Region_s: 0){ _LL1912:
_temp1911=(( struct Cyc_Absyn_Region_s_struct*) _temp1801)->f1; goto _LL1910;
_LL1910: _temp1909=(( struct Cyc_Absyn_Region_s_struct*) _temp1801)->f2; goto
_LL1908; _LL1908: _temp1907=(( struct Cyc_Absyn_Region_s_struct*) _temp1801)->f3;
goto _LL1836;} else{ goto _LL1837;} _LL1837: if(( unsigned int) _temp1801 >  1u?*((
int*) _temp1801) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1838;} else{ goto _LL1839;}
_LL1839: if(( unsigned int) _temp1801 >  1u?*(( int*) _temp1801) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1840;} else{ goto _LL1841;} _LL1841: if(( unsigned int) _temp1801 > 
1u?*(( int*) _temp1801) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1842;} else{ goto
_LL1802;} _LL1804: return; _LL1806: Cyc_Toc_exp_to_c( nv, _temp1843); return;
_LL1808: Cyc_Toc_stmt_to_c( nv, _temp1847); s= _temp1845; continue; _LL1810: {
struct Cyc_Core_Opt* topt= 0; if( _temp1849 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1913=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1913->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1849->topt))->v); _temp1913;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp1849));} if( s->try_depth >  0){ if( topt != 
0){ struct _tuple0* _temp1914= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1915= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp(
_temp1914, 0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp1914,( void*) topt->v, _temp1849, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1915, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1812: Cyc_Toc_exp_to_c( nv, _temp1855); Cyc_Toc_stmt_to_c( nv,
_temp1853); s= _temp1851; continue; _LL1814: Cyc_Toc_exp_to_c( nv, _temp1861);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1857); return; _LL1816: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1863 ==  0? 0: _temp1863->try_depth; Cyc_Toc_do_npop_before(
s->try_depth -  dest_depth, s); return;} _LL1818: if( nv->continue_lab !=  0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp1867=
_temp1865; goto _LL1820; _LL1820: Cyc_Toc_do_npop_before( s->try_depth - ((
struct Cyc_Absyn_Stmt*) _check_null( _temp1867))->try_depth, s); return; _LL1822:
Cyc_Toc_exp_to_c( nv, _temp1879); Cyc_Toc_exp_to_c( nv, _temp1877); Cyc_Toc_exp_to_c(
nv, _temp1873); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1869); return;
_LL1824: Cyc_Toc_xlate_switch( nv, s, _temp1883, _temp1881); return; _LL1826:
if( nv->fallthru_info ==  0){( int) _throw(({ void* _temp1916[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("fallthru in unexpected place", sizeof( unsigned char), 29u), _tag_arr(
_temp1916, sizeof( void*), 0u));}));}{ struct _tuple6 _temp1919; struct Cyc_Dict_Dict*
_temp1920; struct Cyc_List_List* _temp1922; struct _tagged_arr* _temp1924;
struct _tuple6* _temp1917=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp1919=* _temp1917; _LL1925: _temp1924= _temp1919.f1;
goto _LL1923; _LL1923: _temp1922= _temp1919.f2; goto _LL1921; _LL1921: _temp1920=
_temp1919.f3; goto _LL1918; _LL1918: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp1924, 0); Cyc_Toc_do_npop_before( s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp1885)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp1926=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1922); struct Cyc_List_List* _temp1927=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1887); for( 0; _temp1926 !=  0;(
_temp1926= _temp1926->tl, _temp1927= _temp1927->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1927))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* k)) Cyc_Dict_lookup)( _temp1920,(
struct _tuple0*) _temp1926->hd),( struct Cyc_Absyn_Exp*) _temp1927->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1828:{ void*
_temp1928=( void*) _temp1891->r; struct Cyc_Absyn_Vardecl* _temp1938; int
_temp1940; struct Cyc_Absyn_Exp* _temp1942; struct Cyc_Core_Opt* _temp1944;
struct Cyc_Absyn_Pat* _temp1946; struct Cyc_List_List* _temp1948; _LL1930: if(*((
int*) _temp1928) ==  Cyc_Absyn_Var_d){ _LL1939: _temp1938=(( struct Cyc_Absyn_Var_d_struct*)
_temp1928)->f1; goto _LL1931;} else{ goto _LL1932;} _LL1932: if(*(( int*)
_temp1928) ==  Cyc_Absyn_Let_d){ _LL1947: _temp1946=(( struct Cyc_Absyn_Let_d_struct*)
_temp1928)->f1; goto _LL1945; _LL1945: _temp1944=(( struct Cyc_Absyn_Let_d_struct*)
_temp1928)->f3; goto _LL1943; _LL1943: _temp1942=(( struct Cyc_Absyn_Let_d_struct*)
_temp1928)->f4; goto _LL1941; _LL1941: _temp1940=(( struct Cyc_Absyn_Let_d_struct*)
_temp1928)->f5; goto _LL1933;} else{ goto _LL1934;} _LL1934: if(*(( int*)
_temp1928) ==  Cyc_Absyn_Letv_d){ _LL1949: _temp1948=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1928)->f1; goto _LL1935;} else{ goto _LL1936;} _LL1936: goto _LL1937;
_LL1931: { struct Cyc_Toc_Env* _temp1950= Cyc_Toc_add_varmap( nv, _temp1938->name,
Cyc_Absyn_varb_exp( _temp1938->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1951=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1951[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1952; _temp1952.tag= Cyc_Absyn_Local_b;
_temp1952.f1= _temp1938; _temp1952;}); _temp1951;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1950, _temp1950, _temp1938, _temp1889); goto _LL1929;} _LL1933:{ void*
_temp1953=( void*) _temp1946->r; struct Cyc_Absyn_Vardecl* _temp1959; _LL1955:
if(( unsigned int) _temp1953 >  2u?*(( int*) _temp1953) ==  Cyc_Absyn_Var_p: 0){
_LL1960: _temp1959=(( struct Cyc_Absyn_Var_p_struct*) _temp1953)->f1; goto
_LL1956;} else{ goto _LL1957;} _LL1957: goto _LL1958; _LL1956: { struct _tuple0*
old_name= _temp1959->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1959->name= new_name; _temp1959->initializer=( struct Cyc_Absyn_Exp*)
_temp1942;( void*)( _temp1891->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1961=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1961[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1962; _temp1962.tag= Cyc_Absyn_Var_d;
_temp1962.f1= _temp1959; _temp1962;}); _temp1961;})));{ struct Cyc_Toc_Env*
_temp1963= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1964=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1964[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1965; _temp1965.tag= Cyc_Absyn_Local_b; _temp1965.f1=
_temp1959; _temp1965;}); _temp1964;}), 0)); Cyc_Toc_local_decl_to_c( _temp1963,
nv, _temp1959, _temp1889); goto _LL1954;}} _LL1958:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1946, _temp1944, _temp1942, _temp1940,
_temp1889))->r)); goto _LL1954; _LL1954:;} goto _LL1929; _LL1935: { struct Cyc_List_List*
_temp1966=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1948); if( _temp1966 ==  0){({ void* _temp1967[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1967, sizeof( void*), 0u));});}(
void*)( _temp1891->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1968=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1968[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1969; _temp1969.tag= Cyc_Absyn_Var_d;
_temp1969.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1966))->hd; _temp1969;}); _temp1968;}))); _temp1966= _temp1966->tl; for( 0;
_temp1966 !=  0; _temp1966= _temp1966->tl){ struct Cyc_Absyn_Decl* _temp1970=
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp1971=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1971[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1972; _temp1972.tag= Cyc_Absyn_Var_d;
_temp1972.f1=( struct Cyc_Absyn_Vardecl*) _temp1966->hd; _temp1972;}); _temp1971;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1970, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1929;} _LL1937:({
void* _temp1973[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1973, sizeof( void*), 0u));}); goto _LL1929;
_LL1929:;} return; _LL1830: s= _temp1893; continue; _LL1832: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1901); Cyc_Toc_exp_to_c( nv, _temp1899); return;
_LL1834: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1974=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1974->v=( void*) e_typ; _temp1974;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1905);{
struct Cyc_Absyn_Stmt* _temp1975= Cyc_Absyn_seq_stmt( _temp1905, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1976= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1977= Cyc_Absyn_var_exp(
_temp1976, 0); struct Cyc_Absyn_Vardecl* _temp1978= Cyc_Absyn_new_vardecl(
_temp1976, Cyc_Absyn_exn_typ, 0); _temp1977->topt=({ struct Cyc_Core_Opt*
_temp1979=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1979->v=( void*) Cyc_Absyn_exn_typ; _temp1979;});{ struct Cyc_Absyn_Pat*
_temp1980=({ struct Cyc_Absyn_Pat* _temp1996=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1996->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1998=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1998[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1999; _temp1999.tag= Cyc_Absyn_Var_p;
_temp1999.f1= _temp1978; _temp1999;}); _temp1998;})); _temp1996->topt=({ struct
Cyc_Core_Opt* _temp1997=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1997->v=( void*) Cyc_Absyn_exn_typ; _temp1997;}); _temp1996->loc= 0;
_temp1996;}); struct Cyc_Absyn_Exp* _temp1981= Cyc_Absyn_throw_exp( _temp1977, 0);
_temp1981->topt=({ struct Cyc_Core_Opt* _temp1982=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1982->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1982;});{ struct Cyc_Absyn_Stmt* _temp1983= Cyc_Absyn_exp_stmt( _temp1981,
0); struct Cyc_Absyn_Switch_clause* _temp1984=({ struct Cyc_Absyn_Switch_clause*
_temp1993=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1993->pattern= _temp1980; _temp1993->pat_vars=({ struct Cyc_Core_Opt*
_temp1994=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1994->v=( void*)({ struct Cyc_List_List* _temp1995=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1995->hd=( void*) _temp1978;
_temp1995->tl= 0; _temp1995;}); _temp1994;}); _temp1993->where_clause= 0;
_temp1993->body= _temp1983; _temp1993->loc= 0; _temp1993;}); struct Cyc_Absyn_Stmt*
_temp1985= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1903,({ struct Cyc_List_List*
_temp1992=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1992->hd=( void*) _temp1984; _temp1992->tl= 0; _temp1992;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1985);{ struct Cyc_Absyn_Exp* _temp1986= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1991=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1991->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1991->tl= 0; _temp1991;}), 0); struct Cyc_Absyn_Stmt*
_temp1987= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1990=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1990->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1990->tl= 0; _temp1990;}), 0), 0); struct Cyc_Absyn_Exp* _temp1988= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1989= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1987, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1988, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1986, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1989, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1975, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1985, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1836: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1909->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1907);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,({
struct Cyc_List_List* _temp2000=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2000->hd=( void*) Cyc_Absyn_string_exp( Cyc_Absynpp_qvar2string(
x_var), 0); _temp2000->tl= 0; _temp2000;}), 0), Cyc_Absyn_declare_stmt( x_var,
rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( rh_exp, 0),
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp* _temp2001[ 1u]; _temp2001[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp2001, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), Cyc_Absyn_seq_stmt( _temp1907, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({ struct Cyc_Absyn_Exp* _temp2002[ 1u]; _temp2002[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp2002, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL1838:({ void* _temp2003[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s", sizeof(
unsigned char), 10u), _tag_arr( _temp2003, sizeof( void*), 0u));}); return;
_LL1840:({ void* _temp2004[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u),
_tag_arr( _temp2004, sizeof( void*), 0u));}); return; _LL1842:({ void* _temp2005[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("splice", sizeof( unsigned char), 7u), _tag_arr( _temp2005, sizeof(
void*), 0u));}); return; _LL1802:;}} struct _tuple15{ struct _tagged_arr* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2006= f->args; for( 0; _temp2006 !=  0; _temp2006=
_temp2006->tl){(*(( struct _tuple15*) _temp2006->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple15*) _temp2006->hd)).f3);{ struct _tuple0* _temp2007=({ struct
_tuple0* _temp2008=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2008->f1=( void*) Cyc_Absyn_Loc_n; _temp2008->f2=(*(( struct _tuple15*)
_temp2006->hd)).f1; _temp2008;}); nv= Cyc_Toc_add_varmap( nv, _temp2007, Cyc_Absyn_var_exp(
_temp2007, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp2011; void* _temp2013;
struct Cyc_Absyn_Tqual _temp2015; struct Cyc_Core_Opt* _temp2017; struct Cyc_Absyn_VarargInfo
_temp2009=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL2018: _temp2017= _temp2009.name; goto _LL2016; _LL2016: _temp2015= _temp2009.tq;
goto _LL2014; _LL2014: _temp2013=( void*) _temp2009.type; goto _LL2012; _LL2012:
_temp2011= _temp2009.inject; goto _LL2010; _LL2010: { void* _temp2019= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp2013,( void*) Cyc_Absyn_HeapRgn, _temp2015)); struct
_tuple0* _temp2020=({ struct _tuple0* _temp2023=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp2023->f1=( void*) Cyc_Absyn_Loc_n; _temp2023->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2017))->v;
_temp2023;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2021=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2021->hd=( void*)({ struct _tuple15* _temp2022=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp2022->f1=( struct _tagged_arr*)
_temp2017->v; _temp2022->f2= _temp2015; _temp2022->f3= _temp2019; _temp2022;});
_temp2021->tl= 0; _temp2021;})); nv= Cyc_Toc_add_varmap( nv, _temp2020, Cyc_Absyn_var_exp(
_temp2020, 0)); f->cyc_varargs= 0;}}{ struct Cyc_List_List* _temp2024=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( f->param_vardecls))->v;
for( 0; _temp2024 !=  0; _temp2024= _temp2024->tl){( void*)((( struct Cyc_Absyn_Vardecl*)
_temp2024->hd)->type=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)
_temp2024->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2025= s; _LL2027: if(
_temp2025 == ( void*) Cyc_Absyn_Abstract){ goto _LL2028;} else{ goto _LL2029;}
_LL2029: if( _temp2025 == ( void*) Cyc_Absyn_ExternC){ goto _LL2030;} else{ goto
_LL2031;} _LL2031: goto _LL2032; _LL2028: return( void*) Cyc_Absyn_Public;
_LL2030: return( void*) Cyc_Absyn_Extern; _LL2032: return s; _LL2026:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2033=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2034=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2034->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2034;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2035=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2033).f2); if( _temp2035 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2033).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*) _temp2035->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2033).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(( void*) Cyc_Absyn_Public));
s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2036=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2036->v=( void*) _temp2033;
_temp2036;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields= 0;} else{
struct Cyc_List_List* _temp2037=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2037 !=  0; _temp2037= _temp2037->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2037->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2037->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2038=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({
struct Cyc_Core_Opt* _temp2039=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2039->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp2039;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2040=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2038).f2); if( _temp2040 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2038).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*) _temp2040->v)->fields ==  0){( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Uniondecl*
v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2038).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(( void*) Cyc_Absyn_Public));
u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2041=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2041->v=( void*) _temp2038;
_temp2041;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields= 0;} else{
struct Cyc_List_List* _temp2042=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2042 !=  0; _temp2042= _temp2042->tl){(
void*)((( struct Cyc_Absyn_Structfield*) _temp2042->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) _temp2042->hd)->type));}}}}} static
struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far= 0; static void Cyc_Toc_tuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl* tud){ if( Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt* _temp2043=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2043->v=( void*)(( struct Cyc_Set_Set*(*)( int(* cmp)(
struct _tuple0*, struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp);
_temp2043;});}{ struct _tuple0* _temp2044= tud->name; if( tud->fields ==  0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2044)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2044));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2045=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; for( 0; _temp2045 !=  0; _temp2045=
_temp2045->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)
_temp2045->hd; if( f->typs ==  0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2046;
_temp2046.q_const= 1; _temp2046.q_volatile= 0; _temp2046.q_restrict= 0;
_temp2046;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2047=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2047->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2048=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2048[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2049; _temp2049.tag= Cyc_Absyn_Var_d;
_temp2049.f1= vd; _temp2049;}); _temp2048;}), 0); _temp2047->tl= Cyc_Toc_result_decls;
_temp2047;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2050; _temp2050.q_const=
1; _temp2050.q_volatile= 0; _temp2050.q_restrict= 0; _temp2050;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2051=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2051->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2052=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2052[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2053; _temp2053.tag= Cyc_Absyn_Var_d; _temp2053.f1= vd; _temp2053;});
_temp2052;}), 0); _temp2051->tl= Cyc_Toc_result_decls; _temp2051;});{ struct Cyc_List_List*
_temp2054= 0; int i= 1;{ struct Cyc_List_List* _temp2055= f->typs; for( 0;
_temp2055 !=  0;( _temp2055= _temp2055->tl, i ++)){ struct _tagged_arr*
_temp2056= Cyc_Absyn_fieldname( i); struct Cyc_Absyn_Structfield* _temp2057=({
struct Cyc_Absyn_Structfield* _temp2059=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2059->name= _temp2056;
_temp2059->tq=(*(( struct _tuple3*) _temp2055->hd)).f1; _temp2059->type=( void*)
Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2055->hd)).f2); _temp2059->width=
0; _temp2059->attributes= 0; _temp2059;}); _temp2054=({ struct Cyc_List_List*
_temp2058=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2058->hd=( void*) _temp2057; _temp2058->tl= _temp2054; _temp2058;});}}
_temp2054=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2054); _temp2054=({ struct Cyc_List_List* _temp2060=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2060->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2061=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2061->name= Cyc_Toc_tag_sp; _temp2061->tq= Cyc_Toc_mt_tq; _temp2061->type=(
void*) Cyc_Absyn_sint_t; _temp2061->width= 0; _temp2061->attributes= 0;
_temp2061;}); _temp2060->tl= _temp2054; _temp2060;});{ struct Cyc_Absyn_Structdecl*
_temp2062=({ struct Cyc_Absyn_Structdecl* _temp2066=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2066->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2066->name=({ struct Cyc_Core_Opt* _temp2068=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2068->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2068;}); _temp2066->tvs= 0; _temp2066->fields=({ struct Cyc_Core_Opt*
_temp2067=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2067->v=( void*) _temp2054; _temp2067;}); _temp2066->attributes= 0;
_temp2066;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2063=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2063->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2064=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2064[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2065; _temp2065.tag= Cyc_Absyn_Struct_d;
_temp2065.f1= _temp2062; _temp2065;}); _temp2064;}), 0); _temp2063->tl= Cyc_Toc_result_decls;
_temp2063;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp2069=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2069->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2069;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp2070= xd->name; struct Cyc_List_List*
_temp2071=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2071 !=  0; _temp2071= _temp2071->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*) _temp2071->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2072= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2073=({ struct
Cyc_Absyn_ArrayType_struct* _temp2123=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2123[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2124; _temp2124.tag= Cyc_Absyn_ArrayType;
_temp2124.f1=( void*) Cyc_Absyn_uchar_t; _temp2124.f2= Cyc_Toc_mt_tq; _temp2124.f3=(
struct Cyc_Absyn_Exp*) _temp2072; _temp2124;}); _temp2123;}); struct Cyc_Core_Opt*
_temp2074=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2082; int _temp2083; _LL2076: if( _temp2074 ==  0){ goto _LL2077;} else{
goto _LL2078;} _LL2078: if( _temp2074 ==  0){ goto _LL2080;} else{ _temp2082=*
_temp2074; _LL2084: _temp2083=( int) _temp2082.v; if( _temp2083 ==  0){ goto
_LL2079;} else{ goto _LL2080;}} _LL2080: goto _LL2081; _LL2077: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2090; _temp2090.tag= Cyc_Std_String_pa; _temp2090.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp2089; _temp2089.tag= Cyc_Std_Int_pa;
_temp2089.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2088; _temp2088.tag= Cyc_Std_Int_pa; _temp2088.f1=( int)(( unsigned int)((
int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2087; _temp2087.tag= Cyc_Std_Int_pa;
_temp2087.f1=( int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct
_temp2086; _temp2086.tag= Cyc_Std_Int_pa; _temp2086.f1=( int)(( unsigned int)((
int)'\000'));{ void* _temp2085[ 5u]={& _temp2086,& _temp2087,& _temp2088,&
_temp2089,& _temp2090}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof(
unsigned char), 11u), _tag_arr( _temp2085, sizeof( void*), 5u));}}}}}}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2091= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2073, initopt);( void*)( _temp2091->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2092=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2092->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2093=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2093[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2094; _temp2094.tag= Cyc_Absyn_Var_d; _temp2094.f1= _temp2091; _temp2094;});
_temp2093;}), 0); _temp2092->tl= Cyc_Toc_result_decls; _temp2092;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k, int v))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2095= f->typs; for( 0; _temp2095 !=  0;( _temp2095= _temp2095->tl, i ++)){
struct _tagged_arr* _temp2096=({ struct _tagged_arr* _temp2100=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2100[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2102; _temp2102.tag= Cyc_Std_Int_pa;
_temp2102.f1=( int)(( unsigned int) i);{ void* _temp2101[ 1u]={& _temp2102}; Cyc_Std_aprintf(
_tag_arr("f%d", sizeof( unsigned char), 4u), _tag_arr( _temp2101, sizeof( void*),
1u));}}); _temp2100;}); struct Cyc_Absyn_Structfield* _temp2097=({ struct Cyc_Absyn_Structfield*
_temp2099=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2099->name= _temp2096; _temp2099->tq=(*(( struct _tuple3*) _temp2095->hd)).f1;
_temp2099->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*) _temp2095->hd)).f2);
_temp2099->width= 0; _temp2099->attributes= 0; _temp2099;}); fields=({ struct
Cyc_List_List* _temp2098=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2098->hd=( void*) _temp2097; _temp2098->tl= fields; _temp2098;});}} fields=({
struct Cyc_List_List* _temp2103=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2103->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2104=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2104->name= Cyc_Toc_tag_sp; _temp2104->tq= Cyc_Toc_mt_tq; _temp2104->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2104->width=
0; _temp2104->attributes= 0; _temp2104;}); _temp2103->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2103;});{ struct Cyc_Absyn_Structdecl*
_temp2105=({ struct Cyc_Absyn_Structdecl* _temp2109=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2109->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2109->name=({ struct Cyc_Core_Opt* _temp2111=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2111->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2111;}); _temp2109->tvs= 0; _temp2109->fields=({ struct Cyc_Core_Opt*
_temp2110=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2110->v=( void*) fields; _temp2110;}); _temp2109->attributes= 0; _temp2109;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2106=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2106->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2107=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2107[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2108; _temp2108.tag= Cyc_Absyn_Struct_d;
_temp2108.f1= _temp2105; _temp2108;}); _temp2107;}), 0); _temp2106->tl= Cyc_Toc_result_decls;
_temp2106;});}} goto _LL2075;}} _LL2079: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2112= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2122; _temp2122.tag= Cyc_Std_String_pa;
_temp2122.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2121;
_temp2121.tag= Cyc_Std_Int_pa; _temp2121.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2120; _temp2120.tag= Cyc_Std_Int_pa; _temp2120.f1=(
int)(( unsigned int)(( int)'\000'));{ struct Cyc_Std_Int_pa_struct _temp2119;
_temp2119.tag= Cyc_Std_Int_pa; _temp2119.f1=( int)(( unsigned int)(( int)'\000'));{
struct Cyc_Std_Int_pa_struct _temp2118; _temp2118.tag= Cyc_Std_Int_pa; _temp2118.f1=(
int)(( unsigned int)(( int)'\000'));{ void* _temp2117[ 5u]={& _temp2118,&
_temp2119,& _temp2120,& _temp2121,& _temp2122}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s",
sizeof( unsigned char), 11u), _tag_arr( _temp2117, sizeof( void*), 5u));}}}}}}),
0); struct Cyc_Absyn_Vardecl* _temp2113= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2073,( struct Cyc_Absyn_Exp*) _temp2112);( void*)( _temp2113->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2114=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2114->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2115=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2115[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2116; _temp2116.tag= Cyc_Absyn_Var_d;
_temp2116.f1= _temp2113; _temp2116;}); _temp2115;}), 0); _temp2114->tl= Cyc_Toc_result_decls;
_temp2114;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* k,
int v)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2075;
_LL2081: goto _LL2075; _LL2075:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2125= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v);}}
static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s){( void*)( vd->type=(
void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c( body_nv, s);
if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2126=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2132; struct Cyc_Absyn_Exp* _temp2134; struct Cyc_Absyn_Vardecl* _temp2136;
_LL2128: if(*(( int*) _temp2126) ==  Cyc_Absyn_Comprehension_e){ _LL2137:
_temp2136=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2126)->f1; goto
_LL2135; _LL2135: _temp2134=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2126)->f2; goto _LL2133; _LL2133: _temp2132=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2126)->f3; goto _LL2129;} else{ goto _LL2130;} _LL2130: goto _LL2131;
_LL2129: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2136, _temp2134, _temp2132, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2127; _LL2131: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2138= Cyc_Toc_copy_env( init_nv);
_temp2138->toplevel= 1; Cyc_Toc_exp_to_c( _temp2138, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2127; _LL2127:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2139= _new_region("prgn"); struct _RegionHandle* prgn=&
_temp2139; _push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2142; struct Cyc_List_List*
_temp2144; struct Cyc_Toc_Env* _temp2146; struct _tuple11 _temp2140= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2147: _temp2146= _temp2140.f1; goto _LL2145; _LL2145: _temp2144=
_temp2140.f2; goto _LL2143; _LL2143: _temp2142= _temp2140.f3; goto _LL2141;
_LL2141: Cyc_Toc_stmt_to_c( _temp2146, s);{ struct Cyc_Absyn_Stmt* _temp2148=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2142, _temp2148, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2149= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2150= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2149, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2142,
Cyc_Absyn_seq_stmt( _temp2150, _temp2148, 0), 0), 0);} for( 0; _temp2144 !=  0;
_temp2144= _temp2144->tl){ struct _tuple12 _temp2153; void* _temp2154; struct
_tuple0* _temp2156; struct _tuple12* _temp2151=( struct _tuple12*) _temp2144->hd;
_temp2153=* _temp2151; _LL2157: _temp2156= _temp2153.f1; goto _LL2155; _LL2155:
_temp2154= _temp2153.f2; goto _LL2152; _LL2152: s= Cyc_Absyn_declare_stmt(
_temp2156, _temp2154, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds= ds->tl){ if( ! nv->toplevel){({ void*
_temp2158[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("decls_to_c: not at toplevel!", sizeof( unsigned char), 29u), _tag_arr(
_temp2158, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
ds->hd; void* _temp2159=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp2185;
struct Cyc_Absyn_Fndecl* _temp2187; struct Cyc_Absyn_Structdecl* _temp2189;
struct Cyc_Absyn_Uniondecl* _temp2191; struct Cyc_Absyn_Tuniondecl* _temp2193;
struct Cyc_Absyn_Enumdecl* _temp2195; struct Cyc_Absyn_Typedefdecl* _temp2197;
struct Cyc_List_List* _temp2199; struct Cyc_List_List* _temp2201; struct Cyc_List_List*
_temp2203; _LL2161: if(*(( int*) _temp2159) ==  Cyc_Absyn_Var_d){ _LL2186:
_temp2185=(( struct Cyc_Absyn_Var_d_struct*) _temp2159)->f1; goto _LL2162;}
else{ goto _LL2163;} _LL2163: if(*(( int*) _temp2159) ==  Cyc_Absyn_Fn_d){
_LL2188: _temp2187=(( struct Cyc_Absyn_Fn_d_struct*) _temp2159)->f1; goto
_LL2164;} else{ goto _LL2165;} _LL2165: if(*(( int*) _temp2159) ==  Cyc_Absyn_Let_d){
goto _LL2166;} else{ goto _LL2167;} _LL2167: if(*(( int*) _temp2159) ==  Cyc_Absyn_Letv_d){
goto _LL2168;} else{ goto _LL2169;} _LL2169: if(*(( int*) _temp2159) ==  Cyc_Absyn_Struct_d){
_LL2190: _temp2189=(( struct Cyc_Absyn_Struct_d_struct*) _temp2159)->f1; goto
_LL2170;} else{ goto _LL2171;} _LL2171: if(*(( int*) _temp2159) ==  Cyc_Absyn_Union_d){
_LL2192: _temp2191=(( struct Cyc_Absyn_Union_d_struct*) _temp2159)->f1; goto
_LL2172;} else{ goto _LL2173;} _LL2173: if(*(( int*) _temp2159) ==  Cyc_Absyn_Tunion_d){
_LL2194: _temp2193=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2159)->f1; goto
_LL2174;} else{ goto _LL2175;} _LL2175: if(*(( int*) _temp2159) ==  Cyc_Absyn_Enum_d){
_LL2196: _temp2195=(( struct Cyc_Absyn_Enum_d_struct*) _temp2159)->f1; goto
_LL2176;} else{ goto _LL2177;} _LL2177: if(*(( int*) _temp2159) ==  Cyc_Absyn_Typedef_d){
_LL2198: _temp2197=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2159)->f1; goto
_LL2178;} else{ goto _LL2179;} _LL2179: if(*(( int*) _temp2159) ==  Cyc_Absyn_Namespace_d){
_LL2200: _temp2199=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2159)->f2; goto
_LL2180;} else{ goto _LL2181;} _LL2181: if(*(( int*) _temp2159) ==  Cyc_Absyn_Using_d){
_LL2202: _temp2201=(( struct Cyc_Absyn_Using_d_struct*) _temp2159)->f2; goto
_LL2182;} else{ goto _LL2183;} _LL2183: if(*(( int*) _temp2159) ==  Cyc_Absyn_ExternC_d){
_LL2204: _temp2203=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2159)->f1; goto
_LL2184;} else{ goto _LL2160;} _LL2162: { struct _tuple0* _temp2205= _temp2185->name;
if(( void*) _temp2185->sc == ( void*) Cyc_Absyn_ExternC){ _temp2205=({ struct
_tuple0* _temp2206=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2206->f1= Cyc_Absyn_rel_ns_null; _temp2206->f2=(* _temp2205).f2; _temp2206;});}
if( _temp2185->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2185->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2185->name,
Cyc_Absyn_varb_exp( _temp2205,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2207=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2207[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2208; _temp2208.tag= Cyc_Absyn_Global_b;
_temp2208.f1= _temp2185; _temp2208;}); _temp2207;}), 0)); _temp2185->name=
_temp2205;( void*)( _temp2185->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2185->sc));(
void*)( _temp2185->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2185->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2209=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2209->hd=( void*) d; _temp2209->tl=
Cyc_Toc_result_decls; _temp2209;}); goto _LL2160;} _LL2164: { struct _tuple0*
_temp2210= _temp2187->name; if(( void*) _temp2187->sc == ( void*) Cyc_Absyn_ExternC){
_temp2210=({ struct _tuple0* _temp2211=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2211->f1= Cyc_Absyn_rel_ns_null; _temp2211->f2=(*
_temp2210).f2; _temp2211;});} nv= Cyc_Toc_add_varmap( nv, _temp2187->name, Cyc_Absyn_var_exp(
_temp2210, 0)); _temp2187->name= _temp2210; Cyc_Toc_fndecl_to_c( nv, _temp2187);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2212=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2212->hd=( void*) d; _temp2212->tl=
Cyc_Toc_result_decls; _temp2212;}); goto _LL2160;} _LL2166: goto _LL2168;
_LL2168:({ void* _temp2213[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2213, sizeof( void*), 0u));}); goto _LL2160;
_LL2170: Cyc_Toc_structdecl_to_c( _temp2189); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2214=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2214->hd=( void*) d; _temp2214->tl= Cyc_Toc_result_decls; _temp2214;});
goto _LL2160; _LL2172: Cyc_Toc_uniondecl_to_c( _temp2191); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2215=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2215->hd=( void*) d; _temp2215->tl= Cyc_Toc_result_decls;
_temp2215;}); goto _LL2160; _LL2174: if( _temp2193->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2193);} else{ Cyc_Toc_tuniondecl_to_c( _temp2193);} goto _LL2160; _LL2176:
Cyc_Toc_enumdecl_to_c( nv, _temp2195); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2216=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2216->hd=( void*) d; _temp2216->tl= Cyc_Toc_result_decls; _temp2216;});
goto _LL2160; _LL2178: _temp2197->name= _temp2197->name; _temp2197->tvs= 0;(
void*)( _temp2197->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2197->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2217=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2217->hd=( void*) d; _temp2217->tl=
Cyc_Toc_result_decls; _temp2217;}); goto _LL2160; _LL2180: _temp2201= _temp2199;
goto _LL2182; _LL2182: _temp2203= _temp2201; goto _LL2184; _LL2184: nv= Cyc_Toc_decls_to_c(
nv, _temp2203, top); goto _LL2160; _LL2160:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
