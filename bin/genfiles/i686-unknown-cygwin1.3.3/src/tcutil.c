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
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern int Cyc_Core_intcmp(
int, int); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
struct _tagged_arr); extern int Cyc_Std_printf( struct _tagged_arr fmt, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_vrprintf( struct
_RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static const
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
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y); extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern void*
Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_exists_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct
Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct _RegionHandle* r2,
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_rsplit( struct
_RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x); extern
int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l, void*
x); extern void* Cyc_List_assoc_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l, void* x); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List*); static const int Cyc_Position_Lex= 0; static const int
Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
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
Cyc_Absyn_AnonStructType= 13; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 14;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 15; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
16; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 17; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_compress_kb(
void*); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern
void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_empty_effect;
extern struct _tuple1* Cyc_Absyn_tunion_print_arg_qvar; extern struct _tuple1*
Cyc_Absyn_tunion_scanf_arg_qvar; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple4*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct
Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_kind2string( void*); extern struct
_tagged_arr Cyc_Absynpp_kindbound2string( void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* k); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern int Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv*,
void* r1, void* r2); extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat( struct _tagged_arr,
struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct
Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void*
Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap); extern void
Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_copy_type(
void* t); extern int Cyc_Tcutil_kind_leq( void* k1, void* k2); extern void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* t); extern void* Cyc_Tcutil_typ_kind( void* t); extern
void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
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
extern int Cyc_Tcutil_is_function_type( void* t); extern int Cyc_Tcutil_is_pointer_type(
void* t); extern int Cyc_Tcutil_is_zero( struct Cyc_Absyn_Exp* e); extern struct
Cyc_Core_Opt Cyc_Tcutil_rk; extern struct Cyc_Core_Opt Cyc_Tcutil_ak; extern
struct Cyc_Core_Opt Cyc_Tcutil_bk; extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_zero_to_null( struct Cyc_Tcenv_Tenv*, void* t, struct Cyc_Absyn_Exp*
e); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify( void*, void*); extern int Cyc_Tcutil_typecmp( void*,
void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
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
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct
Cyc_Position_Segment*, struct Cyc_Absyn_Structdecl*); extern void Cyc_Tc_tcUniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*,
struct Cyc_Absyn_Uniondecl*); extern void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*, struct Cyc_Absyn_Tuniondecl*);
extern void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*, struct Cyc_Absyn_Enumdecl*); unsigned char Cyc_Tcutil_TypeErr[
12u]="\000\000\000\000TypeErr"; extern void Cyc_Tcutil_unify_it( void* t1, void*
t2); void* Cyc_Tcutil_t1_failure=( void*) 0u; void* Cyc_Tcutil_t2_failure=( void*)
0u; struct _tagged_arr Cyc_Tcutil_failure_reason=( struct _tagged_arr){( void*)
0u,( void*) 0u,( void*)( 0u +  0u)}; void Cyc_Tcutil_explain_failure(){ Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);{ struct _tagged_arr s1= Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure); struct _tagged_arr s2= Cyc_Absynpp_typ2string( Cyc_Tcutil_t2_failure);
int pos= 8;({ struct Cyc_Std_String_pa_struct _temp1; _temp1.tag= Cyc_Std_String_pa;
_temp1.f1=( struct _tagged_arr) s1;{ void* _temp0[ 1u]={& _temp1}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\t%s and ", sizeof( unsigned char), 9u), _tag_arr(
_temp0, sizeof( void*), 1u));}}); pos += _get_arr_size( s1, sizeof(
unsigned char)) +  5; if( pos >=  80){({ void* _temp2[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n\t", sizeof( unsigned char), 3u), _tag_arr( _temp2,
sizeof( void*), 0u));}); pos= 8;}({ struct Cyc_Std_String_pa_struct _temp4;
_temp4.tag= Cyc_Std_String_pa; _temp4.f1=( struct _tagged_arr) s2;{ void* _temp3[
1u]={& _temp4}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s ", sizeof(
unsigned char), 4u), _tag_arr( _temp3, sizeof( void*), 1u));}}); pos +=
_get_arr_size( s2, sizeof( unsigned char)) +  1; if( pos >=  80){({ void* _temp5[
0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n\t", sizeof( unsigned char),
3u), _tag_arr( _temp5, sizeof( void*), 0u));}); pos= 8;}({ void* _temp6[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("failed to unify. ", sizeof(
unsigned char), 18u), _tag_arr( _temp6, sizeof( void*), 0u));}); pos += 17; if(
Cyc_Tcutil_failure_reason.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
if( pos >=  80){({ void* _temp7[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\n\t", sizeof( unsigned char), 3u), _tag_arr( _temp7, sizeof( void*),
0u));}); pos= 8;}({ struct Cyc_Std_String_pa_struct _temp9; _temp9.tag= Cyc_Std_String_pa;
_temp9.f1=( struct _tagged_arr) Cyc_Tcutil_failure_reason;{ void* _temp8[ 1u]={&
_temp9}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp8, sizeof( void*), 1u));}});}({ void* _temp10[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp10,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}}
void Cyc_Tcutil_terr( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap){ Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap)));} void*
Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap){ struct
_tagged_arr msg=( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region,
fmt, ap);({ struct Cyc_Std_String_pa_struct _temp12; _temp12.tag= Cyc_Std_String_pa;
_temp12.f1=( struct _tagged_arr) msg;{ void* _temp11[ 1u]={& _temp12}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp11, sizeof( void*), 1u));}}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);(
int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs !=  0; tvs= tvs->tl){({ struct Cyc_Std_String_pa_struct
_temp15; _temp15.tag= Cyc_Std_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_Absynpp_kindbound2string((
void*)(( struct Cyc_Absyn_Tvar*) tvs->hd)->kind);{ struct Cyc_Std_String_pa_struct
_temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*) tvs->hd);{ void* _temp13[ 2u]={& _temp14,& _temp15}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s::%s ", sizeof( unsigned char), 8u), _tag_arr(
_temp13, sizeof( void*), 2u));}}});}({ void* _temp16[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp16,
sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_arr fmt, struct _tagged_arr ap){ struct _tagged_arr msg=(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, fmt, ap); Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp17=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp17->hd=( void*) sg; _temp17->tl= Cyc_Tcutil_warning_segs;
_temp17;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp18=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp18->hd=( void*)({
struct _tagged_arr* _temp19=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp19[ 0]= msg; _temp19;}); _temp18->tl= Cyc_Tcutil_warning_msgs;
_temp18;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs ==  0){
return;}({ void* _temp20[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("***Warnings***\n",
sizeof( unsigned char), 16u), _tag_arr( _temp20, sizeof( void*), 0u));});{
struct Cyc_List_List* _temp21= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs !=  0) {({ struct Cyc_Std_String_pa_struct _temp24;
_temp24.tag= Cyc_Std_String_pa; _temp24.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Std_String_pa_struct _temp23; _temp23.tag= Cyc_Std_String_pa; _temp23.f1=(
struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
_temp21))->hd);{ void* _temp22[ 2u]={& _temp23,& _temp24}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp22, sizeof(
void*), 2u));}}}); _temp21= _temp21->tl; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->tl;}({ void* _temp25[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("**************\n", sizeof( unsigned char), 16u),
_tag_arr( _temp25, sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*)
Cyc_Std_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; static int
Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar* tv2){
return*(( int*) _check_null( tv1->identity)) - *(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp26= t; struct Cyc_Core_Opt*
_temp38; struct Cyc_Core_Opt* _temp40; struct Cyc_Core_Opt** _temp42; struct Cyc_Core_Opt*
_temp43; struct Cyc_Core_Opt* _temp45; struct Cyc_Core_Opt** _temp47; _LL28: if((
unsigned int) _temp26 >  3u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL39:
_temp38=(( struct Cyc_Absyn_Evar_struct*) _temp26)->f2; if( _temp38 ==  0){ goto
_LL29;} else{ goto _LL30;}} else{ goto _LL30;} _LL30: if(( unsigned int) _temp26
>  3u?*(( int*) _temp26) ==  Cyc_Absyn_Evar: 0){ _LL41: _temp40=(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; _temp42=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp26)->f2; goto _LL31;} else{ goto _LL32;} _LL32: if(( unsigned int) _temp26
>  3u?*(( int*) _temp26) ==  Cyc_Absyn_TypedefType: 0){ _LL44: _temp43=(( struct
Cyc_Absyn_TypedefType_struct*) _temp26)->f3; if( _temp43 ==  0){ goto _LL33;}
else{ goto _LL34;}} else{ goto _LL34;} _LL34: if(( unsigned int) _temp26 >  3u?*((
int*) _temp26) ==  Cyc_Absyn_TypedefType: 0){ _LL46: _temp45=(( struct Cyc_Absyn_TypedefType_struct*)
_temp26)->f3; _temp47=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp26)->f3; goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37; _LL29: return t;
_LL31: { void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp42))->v); if( t2 != ( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp42))->v){* _temp42=({ struct Cyc_Core_Opt* _temp48=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp48->v=( void*) t2;
_temp48;});} return t2;} _LL33: return t; _LL35: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp47))->v); if( t2 != ( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp47))->v){* _temp47=({ struct Cyc_Core_Opt*
_temp49=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp49->v=( void*) t2; _temp49;});} return t2;} _LL37: return t; _LL27:;} void*
Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp50=( void*) c->v; void* _temp58; struct Cyc_Absyn_Conref* _temp60;
_LL52: if( _temp50 == ( void*) Cyc_Absyn_No_constr){ goto _LL53;} else{ goto
_LL54;} _LL54: if(( unsigned int) _temp50 >  1u?*(( int*) _temp50) ==  Cyc_Absyn_Eq_constr:
0){ _LL59: _temp58=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp50)->f1;
goto _LL55;} else{ goto _LL56;} _LL56: if(( unsigned int) _temp50 >  1u?*(( int*)
_temp50) ==  Cyc_Absyn_Forward_constr: 0){ _LL61: _temp60=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp50)->f1; goto _LL57;} else{ goto _LL51;} _LL53: return Cyc_Absyn_empty_conref();
_LL55: return Cyc_Absyn_new_conref( _temp58); _LL57: return Cyc_Tcutil_copy_conref(
_temp60); _LL51:;} static void* Cyc_Tcutil_copy_kindbound( void* kb){ void*
_temp62= Cyc_Absyn_compress_kb( kb); void* _temp70; void* _temp72; _LL64: if(*((
int*) _temp62) ==  Cyc_Absyn_Eq_kb){ _LL71: _temp70=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp62)->f1; goto _LL65;} else{ goto _LL66;} _LL66: if(*(( int*) _temp62) == 
Cyc_Absyn_Unknown_kb){ goto _LL67;} else{ goto _LL68;} _LL68: if(*(( int*)
_temp62) ==  Cyc_Absyn_Less_kb){ _LL73: _temp72=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp62)->f2; goto _LL69;} else{ goto _LL63;} _LL65: return( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp74=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp74[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp75; _temp75.tag= Cyc_Absyn_Eq_kb;
_temp75.f1=( void*) _temp70; _temp75;}); _temp74;}); _LL67: return( void*)({
struct Cyc_Absyn_Unknown_kb_struct* _temp76=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp76[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp77; _temp77.tag= Cyc_Absyn_Unknown_kb; _temp77.f1=
0; _temp77;}); _temp76;}); _LL69: return( void*)({ struct Cyc_Absyn_Less_kb_struct*
_temp78=( struct Cyc_Absyn_Less_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct));
_temp78[ 0]=({ struct Cyc_Absyn_Less_kb_struct _temp79; _temp79.tag= Cyc_Absyn_Less_kb;
_temp79.f1= 0; _temp79.f2=( void*) _temp72; _temp79;}); _temp78;}); _LL63:;}
static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct Cyc_Absyn_Tvar* tv){
return({ struct Cyc_Absyn_Tvar* _temp80=( struct Cyc_Absyn_Tvar*) _cycalloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp80->name= tv->name; _temp80->identity= 0;
_temp80->kind=( void*) Cyc_Tcutil_copy_kindbound(( void*) tv->kind); _temp80;});}
static struct _tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp83;
struct Cyc_Absyn_Tqual _temp85; struct Cyc_Core_Opt* _temp87; struct _tuple2
_temp81=* arg; _LL88: _temp87= _temp81.f1; goto _LL86; _LL86: _temp85= _temp81.f2;
goto _LL84; _LL84: _temp83= _temp81.f3; goto _LL82; _LL82: return({ struct
_tuple2* _temp89=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp89->f1=
_temp87; _temp89->f2= _temp85; _temp89->f3= Cyc_Tcutil_copy_type( _temp83);
_temp89;});} static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){
void* _temp92; struct Cyc_Absyn_Tqual _temp94; struct _tuple4 _temp90=* arg;
_LL95: _temp94= _temp90.f1; goto _LL93; _LL93: _temp92= _temp90.f2; goto _LL91;
_LL91: return({ struct _tuple4* _temp96=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp96->f1= _temp94; _temp96->f2= Cyc_Tcutil_copy_type(
_temp92); _temp96;});} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp97=(
struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp97->name= f->name; _temp97->tq= f->tq; _temp97->type=( void*) Cyc_Tcutil_copy_type((
void*) f->type); _temp97->width= f->width; _temp97->attributes= f->attributes;
_temp97;});} struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp100; void* _temp101; void* _temp103;
struct _tuple8* _temp98= x; _temp100=* _temp98; _LL104: _temp103= _temp100.f1;
goto _LL102; _LL102: _temp101= _temp100.f2; goto _LL99; _LL99: return({ struct
_tuple8* _temp105=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp105->f1= Cyc_Tcutil_copy_type( _temp103); _temp105->f2= Cyc_Tcutil_copy_type(
_temp101); _temp105;});} static struct Cyc_Absyn_Enumfield* Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield* f){ return({ struct Cyc_Absyn_Enumfield* _temp106=(
struct Cyc_Absyn_Enumfield*) _cycalloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp106->name= f->name; _temp106->tag= f->tag; _temp106->loc= f->loc; _temp106;});}
void* Cyc_Tcutil_copy_type( void* t){ void* _temp107= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp157; struct Cyc_Absyn_TunionInfo _temp159; void*
_temp161; struct Cyc_List_List* _temp163; void* _temp165; struct Cyc_Absyn_TunionFieldInfo
_temp167; struct Cyc_List_List* _temp169; void* _temp171; struct Cyc_Absyn_PtrInfo
_temp173; struct Cyc_Absyn_Conref* _temp175; struct Cyc_Absyn_Tqual _temp177;
struct Cyc_Absyn_Conref* _temp179; void* _temp181; void* _temp183; void*
_temp185; void* _temp187; int _temp189; struct Cyc_Absyn_Exp* _temp191; struct
Cyc_Absyn_Tqual _temp193; void* _temp195; struct Cyc_Absyn_FnInfo _temp197;
struct Cyc_List_List* _temp199; struct Cyc_List_List* _temp201; struct Cyc_Absyn_VarargInfo*
_temp203; int _temp205; struct Cyc_List_List* _temp207; void* _temp209; struct
Cyc_Core_Opt* _temp211; struct Cyc_List_List* _temp213; struct Cyc_List_List*
_temp215; struct Cyc_List_List* _temp217; struct _tuple1* _temp219; struct Cyc_List_List*
_temp221; struct _tuple1* _temp223; struct Cyc_List_List* _temp225; struct Cyc_List_List*
_temp227; struct Cyc_Absyn_Enumdecl* _temp229; struct _tuple1* _temp231; struct
Cyc_List_List* _temp233; void* _temp235; struct Cyc_List_List* _temp237; struct
_tuple1* _temp239; void* _temp241; struct Cyc_List_List* _temp243; void*
_temp245; _LL109: if( _temp107 == ( void*) Cyc_Absyn_VoidType){ goto _LL110;}
else{ goto _LL111;} _LL111: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107)
==  Cyc_Absyn_Evar: 0){ goto _LL112;} else{ goto _LL113;} _LL113: if((
unsigned int) _temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_VarType: 0){
_LL158: _temp157=(( struct Cyc_Absyn_VarType_struct*) _temp107)->f1; goto _LL114;}
else{ goto _LL115;} _LL115: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107)
==  Cyc_Absyn_TunionType: 0){ _LL160: _temp159=(( struct Cyc_Absyn_TunionType_struct*)
_temp107)->f1; _LL166: _temp165=( void*) _temp159.tunion_info; goto _LL164;
_LL164: _temp163= _temp159.targs; goto _LL162; _LL162: _temp161=( void*)
_temp159.rgn; goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_TunionFieldType: 0){ _LL168:
_temp167=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp107)->f1; _LL172:
_temp171=( void*) _temp167.field_info; goto _LL170; _LL170: _temp169= _temp167.targs;
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_PointerType: 0){ _LL174: _temp173=(( struct Cyc_Absyn_PointerType_struct*)
_temp107)->f1; _LL184: _temp183=( void*) _temp173.elt_typ; goto _LL182; _LL182:
_temp181=( void*) _temp173.rgn_typ; goto _LL180; _LL180: _temp179= _temp173.nullable;
goto _LL178; _LL178: _temp177= _temp173.tq; goto _LL176; _LL176: _temp175=
_temp173.bounds; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_IntType: 0){ _LL188: _temp187=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp107)->f1; goto _LL186; _LL186:
_temp185=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp107)->f2; goto _LL122;}
else{ goto _LL123;} _LL123: if( _temp107 == ( void*) Cyc_Absyn_FloatType){ goto
_LL124;} else{ goto _LL125;} _LL125: if(( unsigned int) _temp107 >  3u?*(( int*)
_temp107) ==  Cyc_Absyn_DoubleType: 0){ _LL190: _temp189=(( struct Cyc_Absyn_DoubleType_struct*)
_temp107)->f1; goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_ArrayType: 0){ _LL196: _temp195=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp107)->f1; goto _LL194; _LL194:
_temp193=(( struct Cyc_Absyn_ArrayType_struct*) _temp107)->f2; goto _LL192;
_LL192: _temp191=(( struct Cyc_Absyn_ArrayType_struct*) _temp107)->f3; goto
_LL128;} else{ goto _LL129;} _LL129: if(( unsigned int) _temp107 >  3u?*(( int*)
_temp107) ==  Cyc_Absyn_FnType: 0){ _LL198: _temp197=(( struct Cyc_Absyn_FnType_struct*)
_temp107)->f1; _LL214: _temp213= _temp197.tvars; goto _LL212; _LL212: _temp211=
_temp197.effect; goto _LL210; _LL210: _temp209=( void*) _temp197.ret_typ; goto
_LL208; _LL208: _temp207= _temp197.args; goto _LL206; _LL206: _temp205= _temp197.c_varargs;
goto _LL204; _LL204: _temp203= _temp197.cyc_varargs; goto _LL202; _LL202:
_temp201= _temp197.rgn_po; goto _LL200; _LL200: _temp199= _temp197.attributes;
goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_TupleType: 0){ _LL216: _temp215=(( struct Cyc_Absyn_TupleType_struct*)
_temp107)->f1; goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_StructType: 0){ _LL220:
_temp219=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f1; goto _LL218;
_LL218: _temp217=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f2; goto
_LL134;} else{ goto _LL135;} _LL135: if(( unsigned int) _temp107 >  3u?*(( int*)
_temp107) ==  Cyc_Absyn_UnionType: 0){ _LL224: _temp223=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f1; goto _LL222; _LL222: _temp221=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f2; goto _LL136;} else{ goto _LL137;} _LL137: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_AnonStructType: 0){ _LL226:
_temp225=(( struct Cyc_Absyn_AnonStructType_struct*) _temp107)->f1; goto _LL138;}
else{ goto _LL139;} _LL139: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107)
==  Cyc_Absyn_AnonUnionType: 0){ _LL228: _temp227=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp107)->f1; goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_EnumType: 0){ _LL232: _temp231=((
struct Cyc_Absyn_EnumType_struct*) _temp107)->f1; goto _LL230; _LL230: _temp229=((
struct Cyc_Absyn_EnumType_struct*) _temp107)->f2; goto _LL142;} else{ goto
_LL143;} _LL143: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_AnonEnumType:
0){ _LL234: _temp233=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp107)->f1;
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_RgnHandleType: 0){ _LL236: _temp235=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp107)->f1; goto _LL146;} else{ goto
_LL147;} _LL147: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_TypedefType:
0){ _LL240: _temp239=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f1;
goto _LL238; _LL238: _temp237=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f2;
goto _LL148;} else{ goto _LL149;} _LL149: if( _temp107 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_AccessEff: 0){ _LL242: _temp241=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp107)->f1; goto _LL152;} else{ goto _LL153;}
_LL153: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_JoinEff:
0){ _LL244: _temp243=(( struct Cyc_Absyn_JoinEff_struct*) _temp107)->f1; goto
_LL154;} else{ goto _LL155;} _LL155: if(( unsigned int) _temp107 >  3u?*(( int*)
_temp107) ==  Cyc_Absyn_RgnsEff: 0){ _LL246: _temp245=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp107)->f1; goto _LL156;} else{ goto _LL108;} _LL110: goto _LL112; _LL112:
return t; _LL114: return( void*)({ struct Cyc_Absyn_VarType_struct* _temp247=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp247[ 0]=({ struct Cyc_Absyn_VarType_struct _temp248; _temp248.tag= Cyc_Absyn_VarType;
_temp248.f1= Cyc_Tcutil_copy_tvar( _temp157); _temp248;}); _temp247;}); _LL116:
return( void*)({ struct Cyc_Absyn_TunionType_struct* _temp249=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp249[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp250; _temp250.tag= Cyc_Absyn_TunionType;
_temp250.f1=({ struct Cyc_Absyn_TunionInfo _temp251; _temp251.tunion_info=( void*)
_temp165; _temp251.targs= Cyc_Tcutil_copy_types( _temp163); _temp251.rgn=( void*)
Cyc_Tcutil_copy_type( _temp161); _temp251;}); _temp250;}); _temp249;}); _LL118:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp252=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp252[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp253; _temp253.tag= Cyc_Absyn_TunionFieldType;
_temp253.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp254; _temp254.field_info=(
void*) _temp171; _temp254.targs= Cyc_Tcutil_copy_types( _temp169); _temp254;});
_temp253;}); _temp252;}); _LL120: { void* _temp255= Cyc_Tcutil_copy_type(
_temp183); void* _temp256= Cyc_Tcutil_copy_type( _temp181); struct Cyc_Absyn_Conref*
_temp257=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp179); struct Cyc_Absyn_Tqual _temp258= _temp177; struct Cyc_Absyn_Conref*
_temp259= Cyc_Tcutil_copy_conref( _temp175); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp260=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp260[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp261; _temp261.tag= Cyc_Absyn_PointerType;
_temp261.f1=({ struct Cyc_Absyn_PtrInfo _temp262; _temp262.elt_typ=( void*)
_temp255; _temp262.rgn_typ=( void*) _temp256; _temp262.nullable= _temp257;
_temp262.tq= _temp258; _temp262.bounds= _temp259; _temp262;}); _temp261;});
_temp260;});} _LL122: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp263=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp263[ 0]=({ struct Cyc_Absyn_IntType_struct _temp264; _temp264.tag= Cyc_Absyn_IntType;
_temp264.f1=( void*) _temp187; _temp264.f2=( void*) _temp185; _temp264;});
_temp263;}); _LL124: return t; _LL126: return( void*)({ struct Cyc_Absyn_DoubleType_struct*
_temp265=( struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_DoubleType_struct)); _temp265[ 0]=({ struct Cyc_Absyn_DoubleType_struct
_temp266; _temp266.tag= Cyc_Absyn_DoubleType; _temp266.f1= _temp189; _temp266;});
_temp265;}); _LL128: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp267=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp267[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp268; _temp268.tag= Cyc_Absyn_ArrayType;
_temp268.f1=( void*) Cyc_Tcutil_copy_type( _temp195); _temp268.f2= _temp193;
_temp268.f3= _temp191; _temp268;}); _temp267;}); _LL130: { struct Cyc_List_List*
_temp269=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp213); struct
Cyc_Core_Opt* _temp270= _temp211 ==  0? 0:({ struct Cyc_Core_Opt* _temp280=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp280->v=(
void*) Cyc_Tcutil_copy_type(( void*) _temp211->v); _temp280;}); void* _temp271=
Cyc_Tcutil_copy_type( _temp209); struct Cyc_List_List* _temp272=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_copy_arg, _temp207); int _temp273= _temp205; struct Cyc_Absyn_VarargInfo*
cyc_varargs2= 0; if( _temp203 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp203); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp274=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp274->name= cv->name; _temp274->tq= cv->tq; _temp274->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp274->inject= cv->inject; _temp274;});}{ struct Cyc_List_List*
_temp275=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp201);
struct Cyc_List_List* _temp276= _temp199; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp277=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp277[ 0]=({ struct Cyc_Absyn_FnType_struct _temp278; _temp278.tag= Cyc_Absyn_FnType;
_temp278.f1=({ struct Cyc_Absyn_FnInfo _temp279; _temp279.tvars= _temp269;
_temp279.effect= _temp270; _temp279.ret_typ=( void*) _temp271; _temp279.args=
_temp272; _temp279.c_varargs= _temp273; _temp279.cyc_varargs= cyc_varargs2;
_temp279.rgn_po= _temp275; _temp279.attributes= _temp276; _temp279;}); _temp278;});
_temp277;});}} _LL132: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp281=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp281[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp282; _temp282.tag= Cyc_Absyn_TupleType;
_temp282.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp215);
_temp282;}); _temp281;}); _LL134: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp283=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp283[ 0]=({ struct Cyc_Absyn_StructType_struct _temp284; _temp284.tag= Cyc_Absyn_StructType;
_temp284.f1= _temp219; _temp284.f2= Cyc_Tcutil_copy_types( _temp217); _temp284.f3=
0; _temp284;}); _temp283;}); _LL136: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp285=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp285[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp286; _temp286.tag= Cyc_Absyn_UnionType;
_temp286.f1= _temp223; _temp286.f2= Cyc_Tcutil_copy_types( _temp221); _temp286.f3=
0; _temp286;}); _temp285;}); _LL138: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp287=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp287[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp288; _temp288.tag= Cyc_Absyn_AnonStructType; _temp288.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp225); _temp288;}); _temp287;});
_LL140: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp289=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp289[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp290; _temp290.tag=
Cyc_Absyn_AnonUnionType; _temp290.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp227); _temp290;}); _temp289;}); _LL142: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp291=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp291[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp292; _temp292.tag= Cyc_Absyn_EnumType;
_temp292.f1= _temp231; _temp292.f2= _temp229; _temp292;}); _temp291;}); _LL144:
return( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp293=( struct Cyc_Absyn_AnonEnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct)); _temp293[ 0]=({
struct Cyc_Absyn_AnonEnumType_struct _temp294; _temp294.tag= Cyc_Absyn_AnonEnumType;
_temp294.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Enumfield*(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_enumfield,
_temp233); _temp294;}); _temp293;}); _LL146: return( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp295=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp295[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp296; _temp296.tag=
Cyc_Absyn_RgnHandleType; _temp296.f1=( void*) Cyc_Tcutil_copy_type( _temp235);
_temp296;}); _temp295;}); _LL148: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp297=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp297[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp298; _temp298.tag= Cyc_Absyn_TypedefType;
_temp298.f1= _temp239; _temp298.f2= Cyc_Tcutil_copy_types( _temp237); _temp298.f3=
0; _temp298;}); _temp297;}); _LL150: return t; _LL152: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp299=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp299[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp300; _temp300.tag= Cyc_Absyn_AccessEff; _temp300.f1=(
void*) Cyc_Tcutil_copy_type( _temp241); _temp300;}); _temp299;}); _LL154: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp301=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp301[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp302; _temp302.tag= Cyc_Absyn_JoinEff; _temp302.f1= Cyc_Tcutil_copy_types(
_temp243); _temp302;}); _temp301;}); _LL156: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp303=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp303[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp304; _temp304.tag= Cyc_Absyn_RgnsEff;
_temp304.f1=( void*) Cyc_Tcutil_copy_type( _temp245); _temp304;}); _temp303;});
_LL108:;} int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){ return 1;}{
struct _tuple8 _temp306=({ struct _tuple8 _temp305; _temp305.f1= k1; _temp305.f2=
k2; _temp305;}); void* _temp316; void* _temp318; void* _temp320; void* _temp322;
void* _temp324; void* _temp326; _LL308: _LL319: _temp318= _temp306.f1; if(
_temp318 == ( void*) Cyc_Absyn_BoxKind){ goto _LL317;} else{ goto _LL310;}
_LL317: _temp316= _temp306.f2; if( _temp316 == ( void*) Cyc_Absyn_MemKind){ goto
_LL309;} else{ goto _LL310;} _LL310: _LL323: _temp322= _temp306.f1; if( _temp322
== ( void*) Cyc_Absyn_BoxKind){ goto _LL321;} else{ goto _LL312;} _LL321:
_temp320= _temp306.f2; if( _temp320 == ( void*) Cyc_Absyn_AnyKind){ goto _LL311;}
else{ goto _LL312;} _LL312: _LL327: _temp326= _temp306.f1; if( _temp326 == (
void*) Cyc_Absyn_MemKind){ goto _LL325;} else{ goto _LL314;} _LL325: _temp324=
_temp306.f2; if( _temp324 == ( void*) Cyc_Absyn_AnyKind){ goto _LL313;} else{
goto _LL314;} _LL314: goto _LL315; _LL309: goto _LL311; _LL311: goto _LL313;
_LL313: return 1; _LL315: return 0; _LL307:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ void* _temp328= Cyc_Absyn_compress_kb(( void*) tv->kind);
void* _temp336; void* _temp338; _LL330: if(*(( int*) _temp328) ==  Cyc_Absyn_Eq_kb){
_LL337: _temp336=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp328)->f1; goto
_LL331;} else{ goto _LL332;} _LL332: if(*(( int*) _temp328) ==  Cyc_Absyn_Less_kb){
_LL339: _temp338=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp328)->f2;
goto _LL333;} else{ goto _LL334;} _LL334: goto _LL335; _LL331: return _temp336;
_LL333: return _temp338; _LL335: return({ void* _temp340[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("kind not suitably constrained!", sizeof( unsigned char), 31u),
_tag_arr( _temp340, sizeof( void*), 0u));}); _LL329:;} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp341= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp399;
struct Cyc_Core_Opt* _temp401; struct Cyc_Absyn_Tvar* _temp403; void* _temp405;
struct Cyc_Absyn_TunionFieldInfo _temp407; void* _temp409; struct Cyc_Absyn_Tunionfield*
_temp411; struct Cyc_Absyn_TunionFieldInfo _temp413; void* _temp415; struct Cyc_Absyn_Structdecl**
_temp417; struct Cyc_Absyn_Uniondecl** _temp419; struct Cyc_Absyn_Enumdecl*
_temp421; struct Cyc_Absyn_Enumdecl* _temp423; struct Cyc_Absyn_PtrInfo _temp425;
struct Cyc_Core_Opt* _temp427; _LL343: if(( unsigned int) _temp341 >  3u?*(( int*)
_temp341) ==  Cyc_Absyn_Evar: 0){ _LL402: _temp401=(( struct Cyc_Absyn_Evar_struct*)
_temp341)->f1; goto _LL400; _LL400: _temp399=(( struct Cyc_Absyn_Evar_struct*)
_temp341)->f2; goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int)
_temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_VarType: 0){ _LL404: _temp403=((
struct Cyc_Absyn_VarType_struct*) _temp341)->f1; goto _LL346;} else{ goto _LL347;}
_LL347: if( _temp341 == ( void*) Cyc_Absyn_VoidType){ goto _LL348;} else{ goto
_LL349;} _LL349: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_IntType:
0){ _LL406: _temp405=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp341)->f2;
goto _LL350;} else{ goto _LL351;} _LL351: if( _temp341 == ( void*) Cyc_Absyn_FloatType){
goto _LL352;} else{ goto _LL353;} _LL353: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_DoubleType: 0){ goto _LL354;} else{ goto _LL355;}
_LL355: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_FnType:
0){ goto _LL356;} else{ goto _LL357;} _LL357: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL358;} else{ goto _LL359;}
_LL359: if( _temp341 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL360;} else{ goto
_LL361;} _LL361: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_TunionType:
0){ goto _LL362;} else{ goto _LL363;} _LL363: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_TunionFieldType: 0){ _LL408: _temp407=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp341)->f1; _LL410: _temp409=( void*)
_temp407.field_info; if(*(( int*) _temp409) ==  Cyc_Absyn_KnownTunionfield){
_LL412: _temp411=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp409)->f2;
goto _LL364;} else{ goto _LL365;}} else{ goto _LL365;} _LL365: if(( unsigned int)
_temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_TunionFieldType: 0){ _LL414:
_temp413=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp341)->f1; _LL416:
_temp415=( void*) _temp413.field_info; if(*(( int*) _temp415) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL366;} else{ goto _LL367;}} else{ goto _LL367;} _LL367: if(( unsigned int)
_temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_StructType: 0){ _LL418:
_temp417=(( struct Cyc_Absyn_StructType_struct*) _temp341)->f3; if( _temp417 == 
0){ goto _LL368;} else{ goto _LL369;}} else{ goto _LL369;} _LL369: if((
unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_UnionType: 0){
_LL420: _temp419=(( struct Cyc_Absyn_UnionType_struct*) _temp341)->f3; if(
_temp419 ==  0){ goto _LL370;} else{ goto _LL371;}} else{ goto _LL371;} _LL371:
if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_EnumType: 0){
_LL422: _temp421=(( struct Cyc_Absyn_EnumType_struct*) _temp341)->f2; if(
_temp421 ==  0){ goto _LL372;} else{ goto _LL373;}} else{ goto _LL373;} _LL373:
if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_StructType:
0){ goto _LL374;} else{ goto _LL375;} _LL375: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_UnionType: 0){ goto _LL376;} else{ goto _LL377;}
_LL377: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL378;} else{ goto _LL379;} _LL379: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL380;} else{ goto _LL381;}
_LL381: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL382;} else{ goto _LL383;} _LL383: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_EnumType: 0){ _LL424: _temp423=(( struct Cyc_Absyn_EnumType_struct*)
_temp341)->f2; goto _LL384;} else{ goto _LL385;} _LL385: if(( unsigned int)
_temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_PointerType: 0){ _LL426:
_temp425=(( struct Cyc_Absyn_PointerType_struct*) _temp341)->f1; goto _LL386;}
else{ goto _LL387;} _LL387: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341)
==  Cyc_Absyn_ArrayType: 0){ goto _LL388;} else{ goto _LL389;} _LL389: if((
unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_TupleType: 0){
goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_TypedefType: 0){ _LL428: _temp427=(( struct Cyc_Absyn_TypedefType_struct*)
_temp341)->f3; goto _LL392;} else{ goto _LL393;} _LL393: if(( unsigned int)
_temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_AccessEff: 0){ goto _LL394;}
else{ goto _LL395;} _LL395: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341)
==  Cyc_Absyn_JoinEff: 0){ goto _LL396;} else{ goto _LL397;} _LL397: if((
unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL398;} else{ goto _LL342;} _LL344: return( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp401))->v; _LL346: return Cyc_Tcutil_tvar_kind( _temp403);
_LL348: return( void*) Cyc_Absyn_MemKind; _LL350: return _temp405 == ( void*)
Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL352: goto
_LL354; _LL354: goto _LL356; _LL356: return( void*) Cyc_Absyn_MemKind; _LL358:
return( void*) Cyc_Absyn_BoxKind; _LL360: return( void*) Cyc_Absyn_RgnKind;
_LL362: return( void*) Cyc_Absyn_BoxKind; _LL364: if( _temp411->typs ==  0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL366: return({ void* _temp429[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u), _tag_arr( _temp429, sizeof( void*), 0u));});
_LL368: goto _LL370; _LL370: goto _LL372; _LL372: return( void*) Cyc_Absyn_AnyKind;
_LL374: goto _LL376; _LL376: goto _LL378; _LL378: goto _LL380; _LL380: goto
_LL382; _LL382: return( void*) Cyc_Absyn_MemKind; _LL384: if( _temp423->fields
==  0){ return( void*) Cyc_Absyn_AnyKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL386: { void* _temp430=( void*)( Cyc_Absyn_compress_conref( _temp425.bounds))->v;
void* _temp440; void* _temp442; _LL432: if(( unsigned int) _temp430 >  1u?*((
int*) _temp430) ==  Cyc_Absyn_Eq_constr: 0){ _LL441: _temp440=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp430)->f1; if( _temp440 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL433;} else{ goto _LL434;}} else{ goto _LL434;} _LL434: if(( unsigned int)
_temp430 >  1u?*(( int*) _temp430) ==  Cyc_Absyn_Eq_constr: 0){ _LL443: _temp442=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp430)->f1; if(( unsigned int)
_temp442 >  1u?*(( int*) _temp442) ==  Cyc_Absyn_Upper_b: 0){ goto _LL435;}
else{ goto _LL436;}} else{ goto _LL436;} _LL436: if( _temp430 == ( void*) Cyc_Absyn_No_constr){
goto _LL437;} else{ goto _LL438;} _LL438: if(( unsigned int) _temp430 >  1u?*((
int*) _temp430) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL439;} else{ goto
_LL431;} _LL433: return( void*) Cyc_Absyn_MemKind; _LL435: return( void*) Cyc_Absyn_BoxKind;
_LL437: return( void*) Cyc_Absyn_MemKind; _LL439: return({ void* _temp444[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds", sizeof(
unsigned char), 43u), _tag_arr( _temp444, sizeof( void*), 0u));}); _LL431:;}
_LL388: goto _LL390; _LL390: return( void*) Cyc_Absyn_MemKind; _LL392: return({
struct Cyc_Std_String_pa_struct _temp446; _temp446.tag= Cyc_Std_String_pa;
_temp446.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp445[ 1u]={&
_temp446}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp445, sizeof( void*), 1u));}}); _LL394: goto
_LL396; _LL396: goto _LL398; _LL398: return( void*) Cyc_Absyn_EffKind; _LL342:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp447; _push_handler(& _temp447);{
int _temp449= 0; if( setjmp( _temp447.handler)){ _temp449= 1;} if( ! _temp449){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp450= 1; _npop_handler( 0u); return
_temp450;}; _pop_handler();} else{ void* _temp448=( void*) _exn_thrown; void*
_temp452= _temp448; _LL454: if( _temp452 ==  Cyc_Tcutil_Unify){ goto _LL455;}
else{ goto _LL456;} _LL456: goto _LL457; _LL455: return 0; _LL457:( void) _throw(
_temp452); _LL453:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp458= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp494; struct Cyc_Core_Opt* _temp496; struct Cyc_Core_Opt**
_temp498; struct Cyc_Core_Opt* _temp499; struct Cyc_Absyn_PtrInfo _temp501; void*
_temp503; struct Cyc_Absyn_FnInfo _temp505; struct Cyc_List_List* _temp507;
struct Cyc_List_List* _temp509; struct Cyc_Absyn_VarargInfo* _temp511; int
_temp513; struct Cyc_List_List* _temp515; void* _temp517; struct Cyc_Core_Opt*
_temp519; struct Cyc_List_List* _temp521; struct Cyc_List_List* _temp523; void*
_temp525; struct Cyc_Absyn_TunionInfo _temp527; void* _temp529; struct Cyc_List_List*
_temp531; struct Cyc_Core_Opt* _temp533; struct Cyc_List_List* _temp535; struct
Cyc_Absyn_TunionFieldInfo _temp537; struct Cyc_List_List* _temp539; struct Cyc_List_List*
_temp541; struct Cyc_List_List* _temp543; struct Cyc_List_List* _temp545; void*
_temp547; struct Cyc_List_List* _temp549; void* _temp551; _LL460: if((
unsigned int) _temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_VarType: 0){
_LL495: _temp494=(( struct Cyc_Absyn_VarType_struct*) _temp458)->f1; goto _LL461;}
else{ goto _LL462;} _LL462: if(( unsigned int) _temp458 >  3u?*(( int*) _temp458)
==  Cyc_Absyn_Evar: 0){ _LL500: _temp499=(( struct Cyc_Absyn_Evar_struct*)
_temp458)->f2; goto _LL497; _LL497: _temp496=(( struct Cyc_Absyn_Evar_struct*)
_temp458)->f4; _temp498=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp458)->f4; goto _LL463;} else{ goto _LL464;} _LL464: if(( unsigned int)
_temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_PointerType: 0){ _LL502:
_temp501=(( struct Cyc_Absyn_PointerType_struct*) _temp458)->f1; goto _LL465;}
else{ goto _LL466;} _LL466: if(( unsigned int) _temp458 >  3u?*(( int*) _temp458)
==  Cyc_Absyn_ArrayType: 0){ _LL504: _temp503=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp458)->f1; goto _LL467;} else{ goto _LL468;} _LL468: if(( unsigned int)
_temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_FnType: 0){ _LL506: _temp505=((
struct Cyc_Absyn_FnType_struct*) _temp458)->f1; _LL522: _temp521= _temp505.tvars;
goto _LL520; _LL520: _temp519= _temp505.effect; goto _LL518; _LL518: _temp517=(
void*) _temp505.ret_typ; goto _LL516; _LL516: _temp515= _temp505.args; goto
_LL514; _LL514: _temp513= _temp505.c_varargs; goto _LL512; _LL512: _temp511=
_temp505.cyc_varargs; goto _LL510; _LL510: _temp509= _temp505.rgn_po; goto
_LL508; _LL508: _temp507= _temp505.attributes; goto _LL469;} else{ goto _LL470;}
_LL470: if(( unsigned int) _temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_TupleType:
0){ _LL524: _temp523=(( struct Cyc_Absyn_TupleType_struct*) _temp458)->f1; goto
_LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp458 >  3u?*(( int*)
_temp458) ==  Cyc_Absyn_RgnHandleType: 0){ _LL526: _temp525=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp458)->f1; goto _LL473;} else{ goto _LL474;}
_LL474: if(( unsigned int) _temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_TunionType:
0){ _LL528: _temp527=(( struct Cyc_Absyn_TunionType_struct*) _temp458)->f1;
_LL532: _temp531= _temp527.targs; goto _LL530; _LL530: _temp529=( void*)
_temp527.rgn; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_TypedefType: 0){ _LL536:
_temp535=(( struct Cyc_Absyn_TypedefType_struct*) _temp458)->f2; goto _LL534;
_LL534: _temp533=(( struct Cyc_Absyn_TypedefType_struct*) _temp458)->f3; goto
_LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp458 >  3u?*(( int*)
_temp458) ==  Cyc_Absyn_TunionFieldType: 0){ _LL538: _temp537=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp458)->f1; _LL540: _temp539= _temp537.targs; goto _LL479;} else{ goto _LL480;}
_LL480: if(( unsigned int) _temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_StructType:
0){ _LL542: _temp541=(( struct Cyc_Absyn_StructType_struct*) _temp458)->f2; goto
_LL481;} else{ goto _LL482;} _LL482: if(( unsigned int) _temp458 >  3u?*(( int*)
_temp458) ==  Cyc_Absyn_AnonStructType: 0){ _LL544: _temp543=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp458)->f1; goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int)
_temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_AnonUnionType: 0){ _LL546:
_temp545=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp458)->f1; goto _LL485;}
else{ goto _LL486;} _LL486: if(( unsigned int) _temp458 >  3u?*(( int*) _temp458)
==  Cyc_Absyn_AccessEff: 0){ _LL548: _temp547=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp458)->f1; goto _LL487;} else{ goto _LL488;} _LL488: if(( unsigned int)
_temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_JoinEff: 0){ _LL550: _temp549=((
struct Cyc_Absyn_JoinEff_struct*) _temp458)->f1; goto _LL489;} else{ goto _LL490;}
_LL490: if(( unsigned int) _temp458 >  3u?*(( int*) _temp458) ==  Cyc_Absyn_RgnsEff:
0){ _LL552: _temp551=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp458)->f1;
goto _LL491;} else{ goto _LL492;} _LL492: goto _LL493; _LL461: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp494)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);} goto
_LL459; _LL463: if( t ==  evar){ Cyc_Tcutil_failure_reason= _tag_arr("(occurs check)",
sizeof( unsigned char), 15u);( int) _throw(( void*) Cyc_Tcutil_Unify);} else{
if( _temp499 !=  0){ Cyc_Tcutil_occurs( evar, env,( void*) _temp499->v);} else{
int problem= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp498))->v; for( 0; s !=  0; s= s->tl){ if( !(( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*) s->hd)){ problem= 1; break;}}} if( problem){ struct Cyc_List_List*
_temp553= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp498))->v; for( 0; s !=  0; s= s->tl){ if((( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*) s->hd)){ _temp553=({ struct Cyc_List_List* _temp554=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp554->hd=(
void*)(( struct Cyc_Absyn_Tvar*) s->hd); _temp554->tl= _temp553; _temp554;});}}}*
_temp498=({ struct Cyc_Core_Opt* _temp555=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp555->v=( void*) _temp553; _temp555;});}}}
goto _LL459; _LL465: Cyc_Tcutil_occurs( evar, env,( void*) _temp501.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp501.rgn_typ); goto _LL459; _LL467:
Cyc_Tcutil_occurs( evar, env, _temp503); goto _LL459; _LL469: if( _temp519 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*) _temp519->v);} Cyc_Tcutil_occurs( evar,
env, _temp517); for( 0; _temp515 !=  0; _temp515= _temp515->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*) _temp515->hd)).f3);} if( _temp511 !=  0){ void*
_temp558; struct Cyc_Absyn_VarargInfo _temp556=* _temp511; _LL559: _temp558=(
void*) _temp556.type; goto _LL557; _LL557: Cyc_Tcutil_occurs( evar, env,
_temp558);} for( 0; _temp509 !=  0; _temp509= _temp509->tl){ struct _tuple8
_temp562; void* _temp563; void* _temp565; struct _tuple8* _temp560=( struct
_tuple8*) _temp509->hd; _temp562=* _temp560; _LL566: _temp565= _temp562.f1; goto
_LL564; _LL564: _temp563= _temp562.f2; goto _LL561; _LL561: Cyc_Tcutil_occurs(
evar, env, _temp565); Cyc_Tcutil_occurs( evar, env, _temp563);} goto _LL459;
_LL471: for( 0; _temp523 !=  0; _temp523= _temp523->tl){ Cyc_Tcutil_occurs( evar,
env,(*(( struct _tuple4*) _temp523->hd)).f2);} goto _LL459; _LL473: Cyc_Tcutil_occurs(
evar, env, _temp525); goto _LL459; _LL475: Cyc_Tcutil_occurs( evar, env,
_temp529); Cyc_Tcutil_occurslist( evar, env, _temp531); goto _LL459; _LL477:
_temp539= _temp535; goto _LL479; _LL479: _temp541= _temp539; goto _LL481; _LL481:
Cyc_Tcutil_occurslist( evar, env, _temp541); goto _LL459; _LL483: _temp545=
_temp543; goto _LL485; _LL485: for( 0; _temp545 !=  0; _temp545= _temp545->tl){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*) _temp545->hd)->type);}
goto _LL459; _LL487: Cyc_Tcutil_occurs( evar, env, _temp547); goto _LL459;
_LL489: Cyc_Tcutil_occurslist( evar, env, _temp549); goto _LL459; _LL491: Cyc_Tcutil_occurs(
evar, env, _temp551); goto _LL459; _LL493: goto _LL459; _LL459:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts !=  0; ts= ts->tl){ Cyc_Tcutil_occurs( evar, env,( void*) ts->hd);}}
static void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List*
t2){ for( 0; t1 !=  0? t2 !=  0: 0;( t1= t1->tl, t2= t2->tl)){ Cyc_Tcutil_unify_it((
void*) t1->hd,( void*) t2->hd);} if( t1 !=  0? 1: t2 !=  0){( int) _throw(( void*)
Cyc_Tcutil_Unify);}} static void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const !=  tq2.q_const? 1: tq1.q_volatile
!=  tq2.q_volatile)? 1: tq1.q_restrict !=  tq2.q_restrict){ Cyc_Tcutil_failure_reason=
_tag_arr("(qualifiers don't match)", sizeof( unsigned char), 25u);( int) _throw((
void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const ==  tq2.q_const? tq1.q_volatile
==  tq2.q_volatile: 0)? tq1.q_restrict ==  tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y, struct _tagged_arr reason){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref(
y); if( x ==  y){ return;}{ void* _temp567=( void*) x->v; void* _temp575; _LL569:
if( _temp567 == ( void*) Cyc_Absyn_No_constr){ goto _LL570;} else{ goto _LL571;}
_LL571: if(( unsigned int) _temp567 >  1u?*(( int*) _temp567) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL572;} else{ goto _LL573;} _LL573: if(( unsigned int) _temp567 >  1u?*((
int*) _temp567) ==  Cyc_Absyn_Eq_constr: 0){ _LL576: _temp575=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp567)->f1; goto _LL574;} else{ goto _LL568;}
_LL570:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp577=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp577[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp578; _temp578.tag= Cyc_Absyn_Forward_constr; _temp578.f1= y; _temp578;});
_temp577;}))); return; _LL572:( int) _throw(({ void* _temp579[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp579, sizeof( void*), 0u));})); _LL574: { void* _temp580=( void*)
y->v; void* _temp588; _LL582: if( _temp580 == ( void*) Cyc_Absyn_No_constr){
goto _LL583;} else{ goto _LL584;} _LL584: if(( unsigned int) _temp580 >  1u?*((
int*) _temp580) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL585;} else{ goto
_LL586;} _LL586: if(( unsigned int) _temp580 >  1u?*(( int*) _temp580) ==  Cyc_Absyn_Eq_constr:
0){ _LL589: _temp588=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp580)->f1;
goto _LL587;} else{ goto _LL581;} _LL583:( void*)( y->v=( void*)(( void*) x->v));
return; _LL585:( int) _throw(({ void* _temp590[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp590, sizeof( void*), 0u));})); _LL587: if( cmp( _temp575,
_temp588) !=  0){ Cyc_Tcutil_failure_reason= reason;( int) _throw(( void*) Cyc_Tcutil_Unify);}
return; _LL581:;} _LL568:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp591; _push_handler(& _temp591);{ int _temp593= 0; if( setjmp(
_temp591.handler)){ _temp593= 1;} if( ! _temp593){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u));{ int _temp594= 1;
_npop_handler( 0u); return _temp594;}; _pop_handler();} else{ void* _temp592=(
void*) _exn_thrown; void* _temp596= _temp592; _LL598: if( _temp596 ==  Cyc_Tcutil_Unify){
goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL599: return 0; _LL601:(
void) _throw( _temp596); _LL597:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp603=({ struct _tuple8 _temp602; _temp602.f1= b1;
_temp602.f2= b2; _temp602;}); void* _temp613; void* _temp615; void* _temp617;
void* _temp619; void* _temp621; struct Cyc_Absyn_Exp* _temp623; void* _temp625;
struct Cyc_Absyn_Exp* _temp627; _LL605: _LL616: _temp615= _temp603.f1; if(
_temp615 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL614;} else{ goto _LL607;}
_LL614: _temp613= _temp603.f2; if( _temp613 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL606;} else{ goto _LL607;} _LL607: _LL618: _temp617= _temp603.f1; if(
_temp617 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL608;} else{ goto _LL609;}
_LL609: _LL620: _temp619= _temp603.f2; if( _temp619 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL610;} else{ goto _LL611;} _LL611: _LL626: _temp625= _temp603.f1; if((
unsigned int) _temp625 >  1u?*(( int*) _temp625) ==  Cyc_Absyn_Upper_b: 0){
_LL628: _temp627=(( struct Cyc_Absyn_Upper_b_struct*) _temp625)->f1; goto _LL622;}
else{ goto _LL604;} _LL622: _temp621= _temp603.f2; if(( unsigned int) _temp621 > 
1u?*(( int*) _temp621) ==  Cyc_Absyn_Upper_b: 0){ _LL624: _temp623=(( struct Cyc_Absyn_Upper_b_struct*)
_temp621)->f1; goto _LL612;} else{ goto _LL604;} _LL606: return 0; _LL608:
return - 1; _LL610: return 1; _LL612: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp627); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp623); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL604:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp630=({
struct _tuple8 _temp629; _temp629.f1= a1; _temp629.f2= a2; _temp629;}); void*
_temp642; int _temp644; int _temp646; void* _temp648; void* _temp650; int
_temp652; int _temp654; void* _temp656; void* _temp658; int _temp660; void*
_temp662; int _temp664; void* _temp666; int _temp668; void* _temp670; int
_temp672; void* _temp674; struct _tagged_arr _temp676; void* _temp678; struct
_tagged_arr _temp680; _LL632: _LL651: _temp650= _temp630.f1; if(( unsigned int)
_temp650 >  16u?*(( int*) _temp650) ==  Cyc_Absyn_Format_att: 0){ _LL657:
_temp656=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp650)->f1; goto
_LL655; _LL655: _temp654=(( struct Cyc_Absyn_Format_att_struct*) _temp650)->f2;
goto _LL653; _LL653: _temp652=(( struct Cyc_Absyn_Format_att_struct*) _temp650)->f3;
goto _LL643;} else{ goto _LL634;} _LL643: _temp642= _temp630.f2; if((
unsigned int) _temp642 >  16u?*(( int*) _temp642) ==  Cyc_Absyn_Format_att: 0){
_LL649: _temp648=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp642)->f1;
goto _LL647; _LL647: _temp646=(( struct Cyc_Absyn_Format_att_struct*) _temp642)->f2;
goto _LL645; _LL645: _temp644=(( struct Cyc_Absyn_Format_att_struct*) _temp642)->f3;
goto _LL633;} else{ goto _LL634;} _LL634: _LL663: _temp662= _temp630.f1; if((
unsigned int) _temp662 >  16u?*(( int*) _temp662) ==  Cyc_Absyn_Regparm_att: 0){
_LL665: _temp664=(( struct Cyc_Absyn_Regparm_att_struct*) _temp662)->f1; goto
_LL659;} else{ goto _LL636;} _LL659: _temp658= _temp630.f2; if(( unsigned int)
_temp658 >  16u?*(( int*) _temp658) ==  Cyc_Absyn_Regparm_att: 0){ _LL661:
_temp660=(( struct Cyc_Absyn_Regparm_att_struct*) _temp658)->f1; goto _LL635;}
else{ goto _LL636;} _LL636: _LL671: _temp670= _temp630.f1; if(( unsigned int)
_temp670 >  16u?*(( int*) _temp670) ==  Cyc_Absyn_Aligned_att: 0){ _LL673:
_temp672=(( struct Cyc_Absyn_Aligned_att_struct*) _temp670)->f1; goto _LL667;}
else{ goto _LL638;} _LL667: _temp666= _temp630.f2; if(( unsigned int) _temp666 > 
16u?*(( int*) _temp666) ==  Cyc_Absyn_Aligned_att: 0){ _LL669: _temp668=((
struct Cyc_Absyn_Aligned_att_struct*) _temp666)->f1; goto _LL637;} else{ goto
_LL638;} _LL638: _LL679: _temp678= _temp630.f1; if(( unsigned int) _temp678 > 
16u?*(( int*) _temp678) ==  Cyc_Absyn_Section_att: 0){ _LL681: _temp680=((
struct Cyc_Absyn_Section_att_struct*) _temp678)->f1; goto _LL675;} else{ goto
_LL640;} _LL675: _temp674= _temp630.f2; if(( unsigned int) _temp674 >  16u?*((
int*) _temp674) ==  Cyc_Absyn_Section_att: 0){ _LL677: _temp676=(( struct Cyc_Absyn_Section_att_struct*)
_temp674)->f1; goto _LL639;} else{ goto _LL640;} _LL640: goto _LL641; _LL633:
return( _temp656 ==  _temp648? _temp654 ==  _temp646: 0)? _temp652 ==  _temp644:
0; _LL635: _temp672= _temp664; _temp668= _temp660; goto _LL637; _LL637: return
_temp672 ==  _temp668; _LL639: return Cyc_Std_strcmp( _temp680, _temp676) ==  0;
_LL641: return 0; _LL631:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a= a->tl){
if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*) a->hd, a2)){ return 0;}}}{
struct Cyc_List_List* a= a2; for( 0; a !=  0; a= a->tl){ if( ! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,( void*) a->hd, a1)){ return 0;}}} return 1;} static void*
Cyc_Tcutil_normalize_effect( void* e); static void* Cyc_Tcutil_rgns_of( void* t);
static void* Cyc_Tcutil_rgns_of_field( struct Cyc_Absyn_Structfield* sf){ return
Cyc_Tcutil_rgns_of(( void*) sf->type);} static struct _tuple5* Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar* tv){ void* t;{ void* _temp682= Cyc_Tcutil_tvar_kind( tv);
_LL684: if( _temp682 == ( void*) Cyc_Absyn_RgnKind){ goto _LL685;} else{ goto
_LL686;} _LL686: if( _temp682 == ( void*) Cyc_Absyn_EffKind){ goto _LL687;}
else{ goto _LL688;} _LL688: goto _LL689; _LL685: t=( void*) Cyc_Absyn_HeapRgn;
goto _LL683; _LL687: t= Cyc_Absyn_empty_effect; goto _LL683; _LL689: t= Cyc_Absyn_sint_t;
goto _LL683; _LL683:;} return({ struct _tuple5* _temp690=( struct _tuple5*)
_cycalloc( sizeof( struct _tuple5)); _temp690->f1= tv; _temp690->f2= t; _temp690;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp691= Cyc_Tcutil_compress(
t); void* _temp741; struct Cyc_Absyn_TunionInfo _temp743; void* _temp745; struct
Cyc_List_List* _temp747; struct Cyc_Absyn_PtrInfo _temp749; void* _temp751; void*
_temp753; void* _temp755; struct Cyc_List_List* _temp757; struct Cyc_Absyn_TunionFieldInfo
_temp759; struct Cyc_List_List* _temp761; struct Cyc_List_List* _temp763; struct
Cyc_List_List* _temp765; struct Cyc_List_List* _temp767; struct Cyc_List_List*
_temp769; struct Cyc_Absyn_FnInfo _temp771; struct Cyc_List_List* _temp773;
struct Cyc_Absyn_VarargInfo* _temp775; struct Cyc_List_List* _temp777; void*
_temp779; struct Cyc_Core_Opt* _temp781; struct Cyc_List_List* _temp783; void*
_temp785; _LL693: if( _temp691 == ( void*) Cyc_Absyn_VoidType){ goto _LL694;}
else{ goto _LL695;} _LL695: if( _temp691 == ( void*) Cyc_Absyn_FloatType){ goto
_LL696;} else{ goto _LL697;} _LL697: if(( unsigned int) _temp691 >  3u?*(( int*)
_temp691) ==  Cyc_Absyn_DoubleType: 0){ goto _LL698;} else{ goto _LL699;} _LL699:
if(( unsigned int) _temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_EnumType: 0){
goto _LL700;} else{ goto _LL701;} _LL701: if(( unsigned int) _temp691 >  3u?*((
int*) _temp691) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL702;} else{ goto _LL703;}
_LL703: if(( unsigned int) _temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_IntType:
0){ goto _LL704;} else{ goto _LL705;} _LL705: if(( unsigned int) _temp691 >  3u?*((
int*) _temp691) ==  Cyc_Absyn_Evar: 0){ goto _LL706;} else{ goto _LL707;} _LL707:
if(( unsigned int) _temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_VarType: 0){
goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int) _temp691 >  3u?*((
int*) _temp691) ==  Cyc_Absyn_RgnHandleType: 0){ _LL742: _temp741=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp691)->f1; goto _LL710;} else{ goto
_LL711;} _LL711: if(( unsigned int) _temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_TunionType:
0){ _LL744: _temp743=(( struct Cyc_Absyn_TunionType_struct*) _temp691)->f1;
_LL748: _temp747= _temp743.targs; goto _LL746; _LL746: _temp745=( void*)
_temp743.rgn; goto _LL712;} else{ goto _LL713;} _LL713: if(( unsigned int)
_temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_PointerType: 0){ _LL750:
_temp749=(( struct Cyc_Absyn_PointerType_struct*) _temp691)->f1; _LL754:
_temp753=( void*) _temp749.elt_typ; goto _LL752; _LL752: _temp751=( void*)
_temp749.rgn_typ; goto _LL714;} else{ goto _LL715;} _LL715: if(( unsigned int)
_temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_ArrayType: 0){ _LL756: _temp755=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp691)->f1; goto _LL716;} else{
goto _LL717;} _LL717: if(( unsigned int) _temp691 >  3u?*(( int*) _temp691) == 
Cyc_Absyn_TupleType: 0){ _LL758: _temp757=(( struct Cyc_Absyn_TupleType_struct*)
_temp691)->f1; goto _LL718;} else{ goto _LL719;} _LL719: if(( unsigned int)
_temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_TunionFieldType: 0){ _LL760:
_temp759=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp691)->f1; _LL762:
_temp761= _temp759.targs; goto _LL720;} else{ goto _LL721;} _LL721: if((
unsigned int) _temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_StructType: 0){
_LL764: _temp763=(( struct Cyc_Absyn_StructType_struct*) _temp691)->f2; goto
_LL722;} else{ goto _LL723;} _LL723: if(( unsigned int) _temp691 >  3u?*(( int*)
_temp691) ==  Cyc_Absyn_UnionType: 0){ _LL766: _temp765=(( struct Cyc_Absyn_UnionType_struct*)
_temp691)->f2; goto _LL724;} else{ goto _LL725;} _LL725: if(( unsigned int)
_temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_AnonStructType: 0){ _LL768:
_temp767=(( struct Cyc_Absyn_AnonStructType_struct*) _temp691)->f1; goto _LL726;}
else{ goto _LL727;} _LL727: if(( unsigned int) _temp691 >  3u?*(( int*) _temp691)
==  Cyc_Absyn_AnonUnionType: 0){ _LL770: _temp769=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp691)->f1; goto _LL728;} else{ goto _LL729;} _LL729: if(( unsigned int)
_temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_FnType: 0){ _LL772: _temp771=((
struct Cyc_Absyn_FnType_struct*) _temp691)->f1; _LL784: _temp783= _temp771.tvars;
goto _LL782; _LL782: _temp781= _temp771.effect; goto _LL780; _LL780: _temp779=(
void*) _temp771.ret_typ; goto _LL778; _LL778: _temp777= _temp771.args; goto
_LL776; _LL776: _temp775= _temp771.cyc_varargs; goto _LL774; _LL774: _temp773=
_temp771.rgn_po; goto _LL730;} else{ goto _LL731;} _LL731: if( _temp691 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL732;} else{ goto _LL733;} _LL733: if((
unsigned int) _temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_AccessEff: 0){
goto _LL734;} else{ goto _LL735;} _LL735: if(( unsigned int) _temp691 >  3u?*((
int*) _temp691) ==  Cyc_Absyn_JoinEff: 0){ goto _LL736;} else{ goto _LL737;}
_LL737: if(( unsigned int) _temp691 >  3u?*(( int*) _temp691) ==  Cyc_Absyn_RgnsEff:
0){ _LL786: _temp785=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp691)->f1;
goto _LL738;} else{ goto _LL739;} _LL739: if(( unsigned int) _temp691 >  3u?*((
int*) _temp691) ==  Cyc_Absyn_TypedefType: 0){ goto _LL740;} else{ goto _LL692;}
_LL694: goto _LL696; _LL696: goto _LL698; _LL698: goto _LL700; _LL700: goto
_LL702; _LL702: goto _LL704; _LL704: return Cyc_Absyn_empty_effect; _LL706: goto
_LL708; _LL708: { void* _temp787= Cyc_Tcutil_typ_kind( t); _LL789: if( _temp787
== ( void*) Cyc_Absyn_RgnKind){ goto _LL790;} else{ goto _LL791;} _LL791: if(
_temp787 == ( void*) Cyc_Absyn_EffKind){ goto _LL792;} else{ goto _LL793;}
_LL793: goto _LL794; _LL790: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp795=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp795[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp796; _temp796.tag= Cyc_Absyn_AccessEff;
_temp796.f1=( void*) t; _temp796;}); _temp795;}); _LL792: return t; _LL794:
return( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp797=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp797[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp798; _temp798.tag= Cyc_Absyn_RgnsEff; _temp798.f1=( void*) t; _temp798;});
_temp797;}); _LL788:;} _LL710: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp799=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp799[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp800; _temp800.tag= Cyc_Absyn_AccessEff;
_temp800.f1=( void*) _temp741; _temp800;}); _temp799;}); _LL712: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp803=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp803->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp804=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp804[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp805; _temp805.tag= Cyc_Absyn_AccessEff;
_temp805.f1=( void*) _temp745; _temp805;}); _temp804;})); _temp803->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp747); _temp803;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp801=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp801[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp802; _temp802.tag= Cyc_Absyn_JoinEff; _temp802.f1= ts; _temp802;});
_temp801;}));} _LL714: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp806=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp806[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp807; _temp807.tag= Cyc_Absyn_JoinEff;
_temp807.f1=({ void* _temp808[ 2u]; _temp808[ 1u]= Cyc_Tcutil_rgns_of( _temp753);
_temp808[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp809=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp809[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp810; _temp810.tag= Cyc_Absyn_AccessEff;
_temp810.f1=( void*) _temp751; _temp810;}); _temp809;}); Cyc_List_list( _tag_arr(
_temp808, sizeof( void*), 2u));}); _temp807;}); _temp806;})); _LL716: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp755)); _LL718: { struct Cyc_List_List* _temp811= 0;
for( 0; _temp757 !=  0; _temp757= _temp757->tl){ _temp811=({ struct Cyc_List_List*
_temp812=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp812->hd=( void*)(*(( struct _tuple4*) _temp757->hd)).f2; _temp812->tl=
_temp811; _temp812;});} _temp761= _temp811; goto _LL720;} _LL720: _temp763=
_temp761; goto _LL722; _LL722: _temp765= _temp763; goto _LL724; _LL724: return
Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp813=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp813[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp814; _temp814.tag= Cyc_Absyn_JoinEff;
_temp814.f1= Cyc_List_map( Cyc_Tcutil_rgns_of, _temp765); _temp814;}); _temp813;}));
_LL726: _temp769= _temp767; goto _LL728; _LL728: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp815=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp815[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp816; _temp816.tag= Cyc_Absyn_JoinEff; _temp816.f1=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_rgns_of_field, _temp769); _temp816;}); _temp815;})); _LL730: { void*
_temp817= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)( struct _tuple5*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst,
_temp783),( void*)(( struct Cyc_Core_Opt*) _check_null( _temp781))->v); return
Cyc_Tcutil_normalize_effect( _temp817);} _LL732: return Cyc_Absyn_empty_effect;
_LL734: goto _LL736; _LL736: return t; _LL738: return Cyc_Tcutil_rgns_of(
_temp785); _LL740: return({ void* _temp818[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp818, sizeof( void*), 0u));});
_LL692:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp819= e; struct Cyc_List_List* _temp827; struct Cyc_List_List**
_temp829; void* _temp830; _LL821: if(( unsigned int) _temp819 >  3u?*(( int*)
_temp819) ==  Cyc_Absyn_JoinEff: 0){ _LL828: _temp827=(( struct Cyc_Absyn_JoinEff_struct*)
_temp819)->f1; _temp829=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp819)->f1; goto _LL822;} else{ goto _LL823;} _LL823: if(( unsigned int)
_temp819 >  3u?*(( int*) _temp819) ==  Cyc_Absyn_RgnsEff: 0){ _LL831: _temp830=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp819)->f1; goto _LL824;} else{
goto _LL825;} _LL825: goto _LL826; _LL822: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp829; for( 0; effs !=  0; effs= effs->tl){ void* _temp832=( void*)
effs->hd;( void*)( effs->hd=( void*) Cyc_Tcutil_compress( Cyc_Tcutil_normalize_effect(
_temp832)));{ void* _temp833=( void*) effs->hd; _LL835: if(( unsigned int)
_temp833 >  3u?*(( int*) _temp833) ==  Cyc_Absyn_JoinEff: 0){ goto _LL836;}
else{ goto _LL837;} _LL837: goto _LL838; _LL836: nested_join= 1; goto _LL834;
_LL838: goto _LL834; _LL834:;}}} if( ! nested_join){ return e;}{ struct Cyc_List_List*
effects= 0;{ struct Cyc_List_List* effs=* _temp829; for( 0; effs !=  0; effs=
effs->tl){ void* _temp839= Cyc_Tcutil_compress(( void*) effs->hd); struct Cyc_List_List*
_temp847; void* _temp849; _LL841: if(( unsigned int) _temp839 >  3u?*(( int*)
_temp839) ==  Cyc_Absyn_JoinEff: 0){ _LL848: _temp847=(( struct Cyc_Absyn_JoinEff_struct*)
_temp839)->f1; goto _LL842;} else{ goto _LL843;} _LL843: if(( unsigned int)
_temp839 >  3u?*(( int*) _temp839) ==  Cyc_Absyn_AccessEff: 0){ _LL850: _temp849=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp839)->f1; if( _temp849 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL844;} else{ goto _LL845;}} else{ goto _LL845;}
_LL845: goto _LL846; _LL842: effects= Cyc_List_revappend( _temp847, effects);
goto _LL840; _LL844: goto _LL840; _LL846: effects=({ struct Cyc_List_List*
_temp851=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp851->hd=( void*) _temp839; _temp851->tl= effects; _temp851;}); goto _LL840;
_LL840:;}}* _temp829= Cyc_List_imp_rev( effects); return e;}} _LL824: return Cyc_Tcutil_rgns_of(
_temp830); _LL826: return e; _LL820:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp852= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp860; struct Cyc_List_List _temp862; struct Cyc_List_List* _temp863; void*
_temp865; struct Cyc_Core_Opt* _temp867; struct Cyc_Core_Opt* _temp869; _LL854:
if(( unsigned int) _temp852 >  3u?*(( int*) _temp852) ==  Cyc_Absyn_JoinEff: 0){
_LL861: _temp860=(( struct Cyc_Absyn_JoinEff_struct*) _temp852)->f1; if(
_temp860 ==  0){ goto _LL856;} else{ _temp862=* _temp860; _LL866: _temp865=(
void*) _temp862.hd; goto _LL864; _LL864: _temp863= _temp862.tl; goto _LL855;}}
else{ goto _LL856;} _LL856: if(( unsigned int) _temp852 >  3u?*(( int*) _temp852)
==  Cyc_Absyn_Evar: 0){ _LL870: _temp869=(( struct Cyc_Absyn_Evar_struct*)
_temp852)->f1; goto _LL868; _LL868: _temp867=(( struct Cyc_Absyn_Evar_struct*)
_temp852)->f4; goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL855: {
void* _temp871= Cyc_Tcutil_compress( _temp865); struct Cyc_Core_Opt* _temp877;
_LL873: if(( unsigned int) _temp871 >  3u?*(( int*) _temp871) ==  Cyc_Absyn_Evar:
0){ _LL878: _temp877=(( struct Cyc_Absyn_Evar_struct*) _temp871)->f4; goto
_LL874;} else{ goto _LL875;} _LL875: goto _LL876; _LL874: return({ struct
_tuple9* _temp879=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp879->f1= _temp865; _temp879->f2= _temp863; _temp879->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp877))->v; _temp879;}); _LL876: return 0;
_LL872:;} _LL857: if( _temp869 ==  0? 1:( void*) _temp869->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp880[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp880, sizeof( void*), 0u));});} return({ struct _tuple9*
_temp881=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp881->f1= t;
_temp881->f2= 0; _temp881->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp867))->v; _temp881;}); _LL859: return 0; _LL853:;} static
struct Cyc_Core_Opt Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp882=({ struct Cyc_Absyn_FnType_struct*
_temp883=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp883[ 0]=({ struct Cyc_Absyn_FnType_struct _temp884; _temp884.tag= Cyc_Absyn_FnType;
_temp884.f1=({ struct Cyc_Absyn_FnInfo _temp885; _temp885.tvars= 0; _temp885.effect=({
struct Cyc_Core_Opt* _temp886=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp886->v=( void*) eff; _temp886;}); _temp885.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp885.args= 0; _temp885.c_varargs= 0; _temp885.cyc_varargs=
0; _temp885.rgn_po= 0; _temp885.attributes= 0; _temp885;}); _temp884;});
_temp883;}); return Cyc_Absyn_atb_typ(( void*) _temp882,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp887= Cyc_Tcutil_compress( e); void*
_temp899; struct Cyc_List_List* _temp901; void* _temp903; struct Cyc_Core_Opt*
_temp905; struct Cyc_Core_Opt* _temp907; struct Cyc_Core_Opt** _temp909; struct
Cyc_Core_Opt* _temp910; _LL889: if(( unsigned int) _temp887 >  3u?*(( int*)
_temp887) ==  Cyc_Absyn_AccessEff: 0){ _LL900: _temp899=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp887)->f1; goto _LL890;} else{ goto _LL891;} _LL891: if(( unsigned int)
_temp887 >  3u?*(( int*) _temp887) ==  Cyc_Absyn_JoinEff: 0){ _LL902: _temp901=((
struct Cyc_Absyn_JoinEff_struct*) _temp887)->f1; goto _LL892;} else{ goto _LL893;}
_LL893: if(( unsigned int) _temp887 >  3u?*(( int*) _temp887) ==  Cyc_Absyn_RgnsEff:
0){ _LL904: _temp903=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp887)->f1;
goto _LL894;} else{ goto _LL895;} _LL895: if(( unsigned int) _temp887 >  3u?*((
int*) _temp887) ==  Cyc_Absyn_Evar: 0){ _LL911: _temp910=(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f1; goto _LL908; _LL908: _temp907=(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f2; _temp909=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f2; goto _LL906; _LL906: _temp905=(( struct Cyc_Absyn_Evar_struct*)
_temp887)->f4; goto _LL896;} else{ goto _LL897;} _LL897: goto _LL898; _LL890:
if( constrain){ return Cyc_Tcutil_unify( r, _temp899);} _temp899= Cyc_Tcutil_compress(
_temp899); if( r ==  _temp899){ return 1;}{ struct _tuple8 _temp913=({ struct
_tuple8 _temp912; _temp912.f1= r; _temp912.f2= _temp899; _temp912;}); void*
_temp919; struct Cyc_Absyn_Tvar* _temp921; void* _temp923; struct Cyc_Absyn_Tvar*
_temp925; _LL915: _LL924: _temp923= _temp913.f1; if(( unsigned int) _temp923 > 
3u?*(( int*) _temp923) ==  Cyc_Absyn_VarType: 0){ _LL926: _temp925=(( struct Cyc_Absyn_VarType_struct*)
_temp923)->f1; goto _LL920;} else{ goto _LL917;} _LL920: _temp919= _temp913.f2;
if(( unsigned int) _temp919 >  3u?*(( int*) _temp919) ==  Cyc_Absyn_VarType: 0){
_LL922: _temp921=(( struct Cyc_Absyn_VarType_struct*) _temp919)->f1; goto _LL916;}
else{ goto _LL917;} _LL917: goto _LL918; _LL916: return Cyc_Absyn_tvar_cmp(
_temp925, _temp921) ==  0; _LL918: return 0; _LL914:;} _LL892: for( 0; _temp901
!=  0; _temp901= _temp901->tl){ if( Cyc_Tcutil_region_in_effect( constrain, r,(
void*) _temp901->hd)){ return 1;}} return 0; _LL894: { void* _temp927= Cyc_Tcutil_rgns_of(
_temp903); void* _temp933; _LL929: if(( unsigned int) _temp927 >  3u?*(( int*)
_temp927) ==  Cyc_Absyn_RgnsEff: 0){ _LL934: _temp933=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp927)->f1; goto _LL930;} else{ goto _LL931;} _LL931: goto _LL932; _LL930:
if( ! constrain){ return 0;}{ void* _temp935= Cyc_Tcutil_compress( _temp933);
struct Cyc_Core_Opt* _temp941; struct Cyc_Core_Opt* _temp943; struct Cyc_Core_Opt**
_temp945; struct Cyc_Core_Opt* _temp946; _LL937: if(( unsigned int) _temp935 > 
3u?*(( int*) _temp935) ==  Cyc_Absyn_Evar: 0){ _LL947: _temp946=(( struct Cyc_Absyn_Evar_struct*)
_temp935)->f1; goto _LL944; _LL944: _temp943=(( struct Cyc_Absyn_Evar_struct*)
_temp935)->f2; _temp945=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp935)->f2; goto _LL942; _LL942: _temp941=(( struct Cyc_Absyn_Evar_struct*)
_temp935)->f4; goto _LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938: {
void* _temp948= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp941); Cyc_Tcutil_occurs( _temp948,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp941))->v, r);{ void* _temp949= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp951=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp951[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp952; _temp952.tag= Cyc_Absyn_JoinEff; _temp952.f1=({ void* _temp953[ 2u];
_temp953[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp954=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp954[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp955; _temp955.tag= Cyc_Absyn_AccessEff;
_temp955.f1=( void*) r; _temp955;}); _temp954;}); _temp953[ 0u]= _temp948; Cyc_List_list(
_tag_arr( _temp953, sizeof( void*), 2u));}); _temp952;}); _temp951;}));*
_temp945=({ struct Cyc_Core_Opt* _temp950=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp950->v=( void*) _temp949; _temp950;});
return 1;}} _LL940: return 0; _LL936:;} _LL932: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp927); _LL928:;} _LL896: if( _temp910 ==  0? 1:( void*)
_temp910->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp956[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp956, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp957= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp905); Cyc_Tcutil_occurs( _temp957,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp905))->v, r);{ struct Cyc_Absyn_JoinEff_struct*
_temp958=({ struct Cyc_Absyn_JoinEff_struct* _temp960=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp960[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp961; _temp961.tag= Cyc_Absyn_JoinEff; _temp961.f1=({ struct Cyc_List_List*
_temp962=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp962->hd=( void*) _temp957; _temp962->tl=({ struct Cyc_List_List* _temp963=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp963->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp964=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp964[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp965; _temp965.tag= Cyc_Absyn_AccessEff; _temp965.f1=(
void*) r; _temp965;}); _temp964;})); _temp963->tl= 0; _temp963;}); _temp962;});
_temp961;}); _temp960;});* _temp909=({ struct Cyc_Core_Opt* _temp959=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp959->v=( void*)((
void*) _temp958); _temp959;}); return 1;}} _LL898: return 0; _LL888:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp966= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp978;
void* _temp980; struct Cyc_Core_Opt* _temp982; struct Cyc_Core_Opt* _temp984;
struct Cyc_Core_Opt** _temp986; struct Cyc_Core_Opt* _temp987; _LL968: if((
unsigned int) _temp966 >  3u?*(( int*) _temp966) ==  Cyc_Absyn_AccessEff: 0){
goto _LL969;} else{ goto _LL970;} _LL970: if(( unsigned int) _temp966 >  3u?*((
int*) _temp966) ==  Cyc_Absyn_JoinEff: 0){ _LL979: _temp978=(( struct Cyc_Absyn_JoinEff_struct*)
_temp966)->f1; goto _LL971;} else{ goto _LL972;} _LL972: if(( unsigned int)
_temp966 >  3u?*(( int*) _temp966) ==  Cyc_Absyn_RgnsEff: 0){ _LL981: _temp980=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp966)->f1; goto _LL973;} else{
goto _LL974;} _LL974: if(( unsigned int) _temp966 >  3u?*(( int*) _temp966) == 
Cyc_Absyn_Evar: 0){ _LL988: _temp987=(( struct Cyc_Absyn_Evar_struct*) _temp966)->f1;
goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_Evar_struct*) _temp966)->f2;
_temp986=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp966)->f2;
goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_Evar_struct*) _temp966)->f4;
goto _LL975;} else{ goto _LL976;} _LL976: goto _LL977; _LL969: return 0; _LL971:
for( 0; _temp978 !=  0; _temp978= _temp978->tl){ if( Cyc_Tcutil_type_in_effect(
constrain, t,( void*) _temp978->hd)){ return 1;}} return 0; _LL973: _temp980=
Cyc_Tcutil_compress( _temp980); if( t ==  _temp980){ return 1;} if( constrain){
return Cyc_Tcutil_unify( t, _temp980);}{ void* _temp989= Cyc_Tcutil_rgns_of( t);
void* _temp995; _LL991: if(( unsigned int) _temp989 >  3u?*(( int*) _temp989) == 
Cyc_Absyn_RgnsEff: 0){ _LL996: _temp995=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp989)->f1; goto _LL992;} else{ goto _LL993;} _LL993: goto _LL994; _LL992: {
struct _tuple8 _temp998=({ struct _tuple8 _temp997; _temp997.f1= t; _temp997.f2=
Cyc_Tcutil_compress( _temp995); _temp997;}); void* _temp1004; struct Cyc_Absyn_Tvar*
_temp1006; void* _temp1008; struct Cyc_Absyn_Tvar* _temp1010; _LL1000: _LL1009:
_temp1008= _temp998.f1; if(( unsigned int) _temp1008 >  3u?*(( int*) _temp1008)
==  Cyc_Absyn_VarType: 0){ _LL1011: _temp1010=(( struct Cyc_Absyn_VarType_struct*)
_temp1008)->f1; goto _LL1005;} else{ goto _LL1002;} _LL1005: _temp1004= _temp998.f2;
if(( unsigned int) _temp1004 >  3u?*(( int*) _temp1004) ==  Cyc_Absyn_VarType: 0){
_LL1007: _temp1006=(( struct Cyc_Absyn_VarType_struct*) _temp1004)->f1; goto
_LL1001;} else{ goto _LL1002;} _LL1002: goto _LL1003; _LL1001: return Cyc_Tcutil_unify(
t, _temp995); _LL1003: return t ==  _temp995; _LL999:;} _LL994: return Cyc_Tcutil_type_in_effect(
constrain, t, _temp989); _LL990:;} _LL975: if( _temp987 ==  0? 1:( void*)
_temp987->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp1012[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp1012, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp1013= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp982); Cyc_Tcutil_occurs( _temp1013,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp982))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp1014=({ struct Cyc_Absyn_JoinEff_struct* _temp1016=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1016[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1017; _temp1017.tag= Cyc_Absyn_JoinEff; _temp1017.f1=({
struct Cyc_List_List* _temp1018=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1018->hd=( void*) _temp1013; _temp1018->tl=({
struct Cyc_List_List* _temp1019=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1019->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp1020=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp1020[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp1021; _temp1021.tag= Cyc_Absyn_RgnsEff;
_temp1021.f1=( void*) t; _temp1021;}); _temp1020;})); _temp1019->tl= 0;
_temp1019;}); _temp1018;}); _temp1017;}); _temp1016;});* _temp986=({ struct Cyc_Core_Opt*
_temp1015=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1015->v=( void*)(( void*) _temp1014); _temp1015;}); return 1;}} _LL977:
return 0; _LL967:;}} static int Cyc_Tcutil_variable_in_effect( int constrain,
struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp1022=
e; struct Cyc_Absyn_Tvar* _temp1034; struct Cyc_List_List* _temp1036; void*
_temp1038; struct Cyc_Core_Opt* _temp1040; struct Cyc_Core_Opt* _temp1042;
struct Cyc_Core_Opt** _temp1044; struct Cyc_Core_Opt* _temp1045; _LL1024: if((
unsigned int) _temp1022 >  3u?*(( int*) _temp1022) ==  Cyc_Absyn_VarType: 0){
_LL1035: _temp1034=(( struct Cyc_Absyn_VarType_struct*) _temp1022)->f1; goto
_LL1025;} else{ goto _LL1026;} _LL1026: if(( unsigned int) _temp1022 >  3u?*((
int*) _temp1022) ==  Cyc_Absyn_JoinEff: 0){ _LL1037: _temp1036=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1022)->f1; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( unsigned int)
_temp1022 >  3u?*(( int*) _temp1022) ==  Cyc_Absyn_RgnsEff: 0){ _LL1039:
_temp1038=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1022)->f1; goto
_LL1029;} else{ goto _LL1030;} _LL1030: if(( unsigned int) _temp1022 >  3u?*((
int*) _temp1022) ==  Cyc_Absyn_Evar: 0){ _LL1046: _temp1045=(( struct Cyc_Absyn_Evar_struct*)
_temp1022)->f1; goto _LL1043; _LL1043: _temp1042=(( struct Cyc_Absyn_Evar_struct*)
_temp1022)->f2; _temp1044=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1022)->f2; goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_Evar_struct*)
_temp1022)->f4; goto _LL1031;} else{ goto _LL1032;} _LL1032: goto _LL1033;
_LL1025: return Cyc_Absyn_tvar_cmp( v, _temp1034) ==  0; _LL1027: for( 0;
_temp1036 !=  0; _temp1036= _temp1036->tl){ if( Cyc_Tcutil_variable_in_effect(
constrain, v,( void*) _temp1036->hd)){ return 1;}} return 0; _LL1029: { void*
_temp1047= Cyc_Tcutil_rgns_of( _temp1038); void* _temp1053; _LL1049: if((
unsigned int) _temp1047 >  3u?*(( int*) _temp1047) ==  Cyc_Absyn_RgnsEff: 0){
_LL1054: _temp1053=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1047)->f1;
goto _LL1050;} else{ goto _LL1051;} _LL1051: goto _LL1052; _LL1050: if( !
constrain){ return 0;}{ void* _temp1055= Cyc_Tcutil_compress( _temp1053); struct
Cyc_Core_Opt* _temp1061; struct Cyc_Core_Opt* _temp1063; struct Cyc_Core_Opt**
_temp1065; struct Cyc_Core_Opt* _temp1066; _LL1057: if(( unsigned int) _temp1055
>  3u?*(( int*) _temp1055) ==  Cyc_Absyn_Evar: 0){ _LL1067: _temp1066=(( struct
Cyc_Absyn_Evar_struct*) _temp1055)->f1; goto _LL1064; _LL1064: _temp1063=((
struct Cyc_Absyn_Evar_struct*) _temp1055)->f2; _temp1065=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1055)->f2; goto _LL1062; _LL1062: _temp1061=((
struct Cyc_Absyn_Evar_struct*) _temp1055)->f4; goto _LL1058;} else{ goto _LL1059;}
_LL1059: goto _LL1060; _LL1058: { void* _temp1068= Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp1061); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1061))->v, v)){ return 0;}{ void*
_temp1069= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1071=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1071[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1072; _temp1072.tag= Cyc_Absyn_JoinEff;
_temp1072.f1=({ void* _temp1073[ 2u]; _temp1073[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1074=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1074[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1075; _temp1075.tag= Cyc_Absyn_VarType;
_temp1075.f1= v; _temp1075;}); _temp1074;}); _temp1073[ 0u]= _temp1068; Cyc_List_list(
_tag_arr( _temp1073, sizeof( void*), 2u));}); _temp1072;}); _temp1071;}));*
_temp1065=({ struct Cyc_Core_Opt* _temp1070=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1070->v=( void*) _temp1069; _temp1070;});
return 1;}} _LL1060: return 0; _LL1056:;} _LL1052: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp1047); _LL1048:;} _LL1031: if( _temp1045 ==  0? 1:( void*)
_temp1045->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp1076[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp1076, sizeof( void*), 0u));});}{
void* _temp1077= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp1040); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1040))->v, v)){
return 0;}{ struct Cyc_Absyn_JoinEff_struct* _temp1078=({ struct Cyc_Absyn_JoinEff_struct*
_temp1080=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1080[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1081; _temp1081.tag= Cyc_Absyn_JoinEff;
_temp1081.f1=({ struct Cyc_List_List* _temp1082=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1082->hd=( void*) _temp1077;
_temp1082->tl=({ struct Cyc_List_List* _temp1083=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1083->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp1084=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1084[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1085; _temp1085.tag= Cyc_Absyn_VarType; _temp1085.f1=
v; _temp1085;}); _temp1084;})); _temp1083->tl= 0; _temp1083;}); _temp1082;});
_temp1081;}); _temp1080;});* _temp1044=({ struct Cyc_Core_Opt* _temp1079=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1079->v=(
void*)(( void*) _temp1078); _temp1079;}); return 1;}} _LL1033: return 0; _LL1023:;}}
static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp1086= e; struct Cyc_List_List* _temp1096; void* _temp1098;
_LL1088: if(( unsigned int) _temp1086 >  3u?*(( int*) _temp1086) ==  Cyc_Absyn_JoinEff:
0){ _LL1097: _temp1096=(( struct Cyc_Absyn_JoinEff_struct*) _temp1086)->f1; goto
_LL1089;} else{ goto _LL1090;} _LL1090: if(( unsigned int) _temp1086 >  3u?*((
int*) _temp1086) ==  Cyc_Absyn_RgnsEff: 0){ _LL1099: _temp1098=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp1086)->f1; goto _LL1091;} else{ goto _LL1092;}
_LL1092: if(( unsigned int) _temp1086 >  3u?*(( int*) _temp1086) ==  Cyc_Absyn_Evar:
0){ goto _LL1093;} else{ goto _LL1094;} _LL1094: goto _LL1095; _LL1089: for( 0;
_temp1096 !=  0; _temp1096= _temp1096->tl){ if( Cyc_Tcutil_evar_in_effect( evar,(
void*) _temp1096->hd)){ return 1;}} return 0; _LL1091: { void* _temp1100= Cyc_Tcutil_rgns_of(
_temp1098); void* _temp1106; _LL1102: if(( unsigned int) _temp1100 >  3u?*(( int*)
_temp1100) ==  Cyc_Absyn_RgnsEff: 0){ _LL1107: _temp1106=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1100)->f1; goto _LL1103;} else{ goto _LL1104;} _LL1104: goto _LL1105;
_LL1103: return 0; _LL1105: return Cyc_Tcutil_evar_in_effect( evar, _temp1100);
_LL1101:;} _LL1093: return evar ==  e; _LL1095: return 0; _LL1087:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1108= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1122; void* _temp1124; struct Cyc_Absyn_Tvar*
_temp1126; void* _temp1128; struct Cyc_Core_Opt* _temp1130; struct Cyc_Core_Opt*
_temp1132; struct Cyc_Core_Opt** _temp1134; _LL1110: if(( unsigned int)
_temp1108 >  3u?*(( int*) _temp1108) ==  Cyc_Absyn_JoinEff: 0){ _LL1123:
_temp1122=(( struct Cyc_Absyn_JoinEff_struct*) _temp1108)->f1; goto _LL1111;}
else{ goto _LL1112;} _LL1112: if(( unsigned int) _temp1108 >  3u?*(( int*)
_temp1108) ==  Cyc_Absyn_AccessEff: 0){ _LL1125: _temp1124=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1108)->f1; goto _LL1113;} else{ goto _LL1114;} _LL1114: if(( unsigned int)
_temp1108 >  3u?*(( int*) _temp1108) ==  Cyc_Absyn_VarType: 0){ _LL1127:
_temp1126=(( struct Cyc_Absyn_VarType_struct*) _temp1108)->f1; goto _LL1115;}
else{ goto _LL1116;} _LL1116: if(( unsigned int) _temp1108 >  3u?*(( int*)
_temp1108) ==  Cyc_Absyn_RgnsEff: 0){ _LL1129: _temp1128=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1108)->f1; goto _LL1117;} else{ goto _LL1118;} _LL1118: if(( unsigned int)
_temp1108 >  3u?*(( int*) _temp1108) ==  Cyc_Absyn_Evar: 0){ _LL1133: _temp1132=((
struct Cyc_Absyn_Evar_struct*) _temp1108)->f2; _temp1134=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1108)->f2; goto _LL1131; _LL1131: _temp1130=((
struct Cyc_Absyn_Evar_struct*) _temp1108)->f4; goto _LL1119;} else{ goto _LL1120;}
_LL1120: goto _LL1121; _LL1111: for( 0; _temp1122 !=  0; _temp1122= _temp1122->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*) _temp1122->hd, e2)){
return 0;}} return 1; _LL1113: return( Cyc_Tcutil_region_in_effect( 0, _temp1124,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp1124, e2))? 1: Cyc_Tcutil_unify(
_temp1124,( void*) Cyc_Absyn_HeapRgn); _LL1115: return Cyc_Tcutil_variable_in_effect(
0, _temp1126, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1126, e2); _LL1117: {
void* _temp1135= Cyc_Tcutil_rgns_of( _temp1128); void* _temp1141; _LL1137: if((
unsigned int) _temp1135 >  3u?*(( int*) _temp1135) ==  Cyc_Absyn_RgnsEff: 0){
_LL1142: _temp1141=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1135)->f1;
goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140; _LL1138: return( Cyc_Tcutil_type_in_effect(
0, _temp1141, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1141, e2))? 1: Cyc_Tcutil_unify(
_temp1141, Cyc_Absyn_sint_t); _LL1140: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1135, e2); _LL1136:;} _LL1119: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1134=({ struct Cyc_Core_Opt* _temp1143=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1143->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1144=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1144[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1145; _temp1145.tag= Cyc_Absyn_JoinEff; _temp1145.f1=
0; _temp1145;}); _temp1144;})); _temp1143;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1130))->v, e2);*
_temp1134=({ struct Cyc_Core_Opt* _temp1146=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1146->v=( void*) e2; _temp1146;});}} return
1; _LL1121: return({ struct Cyc_Std_String_pa_struct _temp1148; _temp1148.tag=
Cyc_Std_String_pa; _temp1148.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1147[ 1u]={& _temp1148};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1147, sizeof( void*), 1u));}});
_LL1109:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1150=({ struct
_tuple10 _temp1149; _temp1149.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1149.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1149;}); _LL1152: goto _LL1153; _LL1153:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1151:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1= r1->tl){ struct _tuple8 _temp1156; void* _temp1157; void* _temp1159; struct
_tuple8* _temp1154=( struct _tuple8*) r1->hd; _temp1156=* _temp1154; _LL1160:
_temp1159= _temp1156.f1; goto _LL1158; _LL1158: _temp1157= _temp1156.f2; goto
_LL1155; _LL1155: { int found= _temp1159 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2= r2->tl){ struct
_tuple8 _temp1163; void* _temp1164; void* _temp1166; struct _tuple8* _temp1161=(
struct _tuple8*) r2->hd; _temp1163=* _temp1161; _LL1167: _temp1166= _temp1163.f1;
goto _LL1165; _LL1165: _temp1164= _temp1163.f2; goto _LL1162; _LL1162: if( Cyc_Tcutil_unify(
_temp1159, _temp1166)? Cyc_Tcutil_unify( _temp1157, _temp1164): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason=(
struct _tagged_arr) _tag_arr( 0u, 0u, 0u); t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2); if( t1 ==  t2){ return;}{ void* _temp1168= t1; struct Cyc_Core_Opt*
_temp1174; struct Cyc_Core_Opt* _temp1176; struct Cyc_Core_Opt** _temp1178;
struct Cyc_Core_Opt* _temp1179; _LL1170: if(( unsigned int) _temp1168 >  3u?*((
int*) _temp1168) ==  Cyc_Absyn_Evar: 0){ _LL1180: _temp1179=(( struct Cyc_Absyn_Evar_struct*)
_temp1168)->f1; goto _LL1177; _LL1177: _temp1176=(( struct Cyc_Absyn_Evar_struct*)
_temp1168)->f2; _temp1178=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1168)->f2; goto _LL1175; _LL1175: _temp1174=(( struct Cyc_Absyn_Evar_struct*)
_temp1168)->f4; goto _LL1171;} else{ goto _LL1172;} _LL1172: goto _LL1173;
_LL1171: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1174))->v, t2);{ void* _temp1181= Cyc_Tcutil_typ_kind( t2);
if( Cyc_Tcutil_kind_leq( _temp1181,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1179))->v)){* _temp1178=({ struct Cyc_Core_Opt* _temp1182=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1182->v=( void*) t2; _temp1182;});
return;} else{{ void* _temp1183= t2; struct Cyc_Core_Opt* _temp1191; struct Cyc_Core_Opt*
_temp1193; struct Cyc_Core_Opt** _temp1195; struct Cyc_Absyn_PtrInfo _temp1197;
_LL1185: if(( unsigned int) _temp1183 >  3u?*(( int*) _temp1183) ==  Cyc_Absyn_Evar:
0){ _LL1194: _temp1193=(( struct Cyc_Absyn_Evar_struct*) _temp1183)->f2;
_temp1195=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1183)->f2;
goto _LL1192; _LL1192: _temp1191=(( struct Cyc_Absyn_Evar_struct*) _temp1183)->f4;
goto _LL1186;} else{ goto _LL1187;} _LL1187: if(( unsigned int) _temp1183 >  3u?*((
int*) _temp1183) ==  Cyc_Absyn_PointerType: 0){ _LL1198: _temp1197=(( struct Cyc_Absyn_PointerType_struct*)
_temp1183)->f1; goto _LL1196;} else{ goto _LL1189;} _LL1196: if(( void*)
_temp1179->v == ( void*) Cyc_Absyn_BoxKind){ goto _LL1188;} else{ goto _LL1189;}
_LL1189: goto _LL1190; _LL1186: { struct Cyc_List_List* _temp1199=( struct Cyc_List_List*)
_temp1174->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1191))->v; for( 0; s2 !=  0; s2= s2->tl){ if( !(( int(*)( int(*
compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp1199,(
struct Cyc_Absyn_Tvar*) s2->hd)){ Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if(
Cyc_Tcutil_kind_leq(( void*) _temp1179->v, _temp1181)){* _temp1195=({ struct Cyc_Core_Opt*
_temp1200=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1200->v=( void*) t1; _temp1200;}); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)", sizeof( unsigned char), 25u); goto _LL1184;}
_LL1188: { struct Cyc_Absyn_Conref* _temp1201= Cyc_Absyn_compress_conref(
_temp1197.bounds);{ void* _temp1202=( void*) _temp1201->v; _LL1204: if(
_temp1202 == ( void*) Cyc_Absyn_No_constr){ goto _LL1205;} else{ goto _LL1206;}
_LL1206: goto _LL1207; _LL1205:( void*)( _temp1201->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp1208=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1208[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1209; _temp1209.tag= Cyc_Absyn_Eq_constr;
_temp1209.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1210=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1210[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1211; _temp1211.tag= Cyc_Absyn_Upper_b;
_temp1211.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1211;}); _temp1210;}));
_temp1209;}); _temp1208;})));* _temp1178=({ struct Cyc_Core_Opt* _temp1212=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1212->v=(
void*) t2; _temp1212;}); return; _LL1207: goto _LL1203; _LL1203:;} goto _LL1184;}
_LL1190: goto _LL1184; _LL1184:;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} _LL1173:
goto _LL1169; _LL1169:;}{ struct _tuple8 _temp1214=({ struct _tuple8 _temp1213;
_temp1213.f1= t2; _temp1213.f2= t1; _temp1213;}); void* _temp1270; void*
_temp1272; void* _temp1274; void* _temp1276; struct Cyc_Absyn_Tvar* _temp1278;
void* _temp1280; struct Cyc_Absyn_Tvar* _temp1282; void* _temp1284; struct Cyc_Absyn_Structdecl**
_temp1286; struct Cyc_List_List* _temp1288; struct _tuple1* _temp1290; void*
_temp1292; struct Cyc_Absyn_Structdecl** _temp1294; struct Cyc_List_List*
_temp1296; struct _tuple1* _temp1298; void* _temp1300; struct _tuple1* _temp1302;
void* _temp1304; struct _tuple1* _temp1306; void* _temp1308; struct Cyc_List_List*
_temp1310; void* _temp1312; struct Cyc_List_List* _temp1314; void* _temp1316;
struct Cyc_Absyn_Uniondecl** _temp1318; struct Cyc_List_List* _temp1320; struct
_tuple1* _temp1322; void* _temp1324; struct Cyc_Absyn_Uniondecl** _temp1326;
struct Cyc_List_List* _temp1328; struct _tuple1* _temp1330; void* _temp1332;
struct Cyc_Absyn_TunionInfo _temp1334; void* _temp1336; struct Cyc_List_List*
_temp1338; void* _temp1340; struct Cyc_Absyn_Tuniondecl** _temp1342; struct Cyc_Absyn_Tuniondecl*
_temp1344; void* _temp1345; struct Cyc_Absyn_TunionInfo _temp1347; void*
_temp1349; struct Cyc_List_List* _temp1351; void* _temp1353; struct Cyc_Absyn_Tuniondecl**
_temp1355; struct Cyc_Absyn_Tuniondecl* _temp1357; void* _temp1358; struct Cyc_Absyn_TunionFieldInfo
_temp1360; struct Cyc_List_List* _temp1362; void* _temp1364; struct Cyc_Absyn_Tunionfield*
_temp1366; struct Cyc_Absyn_Tuniondecl* _temp1368; void* _temp1370; struct Cyc_Absyn_TunionFieldInfo
_temp1372; struct Cyc_List_List* _temp1374; void* _temp1376; struct Cyc_Absyn_Tunionfield*
_temp1378; struct Cyc_Absyn_Tuniondecl* _temp1380; void* _temp1382; struct Cyc_Absyn_PtrInfo
_temp1384; struct Cyc_Absyn_Conref* _temp1386; struct Cyc_Absyn_Tqual _temp1388;
struct Cyc_Absyn_Conref* _temp1390; void* _temp1392; void* _temp1394; void*
_temp1396; struct Cyc_Absyn_PtrInfo _temp1398; struct Cyc_Absyn_Conref*
_temp1400; struct Cyc_Absyn_Tqual _temp1402; struct Cyc_Absyn_Conref* _temp1404;
void* _temp1406; void* _temp1408; void* _temp1410; void* _temp1412; void*
_temp1414; void* _temp1416; void* _temp1418; void* _temp1420; void* _temp1422;
void* _temp1424; void* _temp1426; int _temp1428; void* _temp1430; int _temp1432;
void* _temp1434; struct Cyc_Absyn_Exp* _temp1436; struct Cyc_Absyn_Tqual
_temp1438; void* _temp1440; void* _temp1442; struct Cyc_Absyn_Exp* _temp1444;
struct Cyc_Absyn_Tqual _temp1446; void* _temp1448; void* _temp1450; struct Cyc_Absyn_FnInfo
_temp1452; struct Cyc_List_List* _temp1454; struct Cyc_List_List* _temp1456;
struct Cyc_Absyn_VarargInfo* _temp1458; int _temp1460; struct Cyc_List_List*
_temp1462; void* _temp1464; struct Cyc_Core_Opt* _temp1466; struct Cyc_List_List*
_temp1468; void* _temp1470; struct Cyc_Absyn_FnInfo _temp1472; struct Cyc_List_List*
_temp1474; struct Cyc_List_List* _temp1476; struct Cyc_Absyn_VarargInfo*
_temp1478; int _temp1480; struct Cyc_List_List* _temp1482; void* _temp1484;
struct Cyc_Core_Opt* _temp1486; struct Cyc_List_List* _temp1488; void* _temp1490;
struct Cyc_List_List* _temp1492; void* _temp1494; struct Cyc_List_List*
_temp1496; void* _temp1498; struct Cyc_List_List* _temp1500; void* _temp1502;
struct Cyc_List_List* _temp1504; void* _temp1506; struct Cyc_List_List*
_temp1508; void* _temp1510; struct Cyc_List_List* _temp1512; void* _temp1514;
void* _temp1516; void* _temp1518; void* _temp1520; void* _temp1522; void*
_temp1524; void* _temp1526; void* _temp1528; void* _temp1530; void* _temp1532;
void* _temp1534; void* _temp1536; _LL1216: _LL1271: _temp1270= _temp1214.f1; if((
unsigned int) _temp1270 >  3u?*(( int*) _temp1270) ==  Cyc_Absyn_Evar: 0){ goto
_LL1217;} else{ goto _LL1218;} _LL1218: _LL1275: _temp1274= _temp1214.f1; if(
_temp1274 == ( void*) Cyc_Absyn_VoidType){ goto _LL1273;} else{ goto _LL1220;}
_LL1273: _temp1272= _temp1214.f2; if( _temp1272 == ( void*) Cyc_Absyn_VoidType){
goto _LL1219;} else{ goto _LL1220;} _LL1220: _LL1281: _temp1280= _temp1214.f1;
if(( unsigned int) _temp1280 >  3u?*(( int*) _temp1280) ==  Cyc_Absyn_VarType: 0){
_LL1283: _temp1282=(( struct Cyc_Absyn_VarType_struct*) _temp1280)->f1; goto
_LL1277;} else{ goto _LL1222;} _LL1277: _temp1276= _temp1214.f2; if((
unsigned int) _temp1276 >  3u?*(( int*) _temp1276) ==  Cyc_Absyn_VarType: 0){
_LL1279: _temp1278=(( struct Cyc_Absyn_VarType_struct*) _temp1276)->f1; goto
_LL1221;} else{ goto _LL1222;} _LL1222: _LL1293: _temp1292= _temp1214.f1; if((
unsigned int) _temp1292 >  3u?*(( int*) _temp1292) ==  Cyc_Absyn_StructType: 0){
_LL1299: _temp1298=(( struct Cyc_Absyn_StructType_struct*) _temp1292)->f1; goto
_LL1297; _LL1297: _temp1296=(( struct Cyc_Absyn_StructType_struct*) _temp1292)->f2;
goto _LL1295; _LL1295: _temp1294=(( struct Cyc_Absyn_StructType_struct*)
_temp1292)->f3; goto _LL1285;} else{ goto _LL1224;} _LL1285: _temp1284=
_temp1214.f2; if(( unsigned int) _temp1284 >  3u?*(( int*) _temp1284) ==  Cyc_Absyn_StructType:
0){ _LL1291: _temp1290=(( struct Cyc_Absyn_StructType_struct*) _temp1284)->f1;
goto _LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_StructType_struct*)
_temp1284)->f2; goto _LL1287; _LL1287: _temp1286=(( struct Cyc_Absyn_StructType_struct*)
_temp1284)->f3; goto _LL1223;} else{ goto _LL1224;} _LL1224: _LL1305: _temp1304=
_temp1214.f1; if(( unsigned int) _temp1304 >  3u?*(( int*) _temp1304) ==  Cyc_Absyn_EnumType:
0){ _LL1307: _temp1306=(( struct Cyc_Absyn_EnumType_struct*) _temp1304)->f1;
goto _LL1301;} else{ goto _LL1226;} _LL1301: _temp1300= _temp1214.f2; if((
unsigned int) _temp1300 >  3u?*(( int*) _temp1300) ==  Cyc_Absyn_EnumType: 0){
_LL1303: _temp1302=(( struct Cyc_Absyn_EnumType_struct*) _temp1300)->f1; goto
_LL1225;} else{ goto _LL1226;} _LL1226: _LL1313: _temp1312= _temp1214.f1; if((
unsigned int) _temp1312 >  3u?*(( int*) _temp1312) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1315: _temp1314=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1312)->f1;
goto _LL1309;} else{ goto _LL1228;} _LL1309: _temp1308= _temp1214.f2; if((
unsigned int) _temp1308 >  3u?*(( int*) _temp1308) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1311: _temp1310=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1308)->f1;
goto _LL1227;} else{ goto _LL1228;} _LL1228: _LL1325: _temp1324= _temp1214.f1;
if(( unsigned int) _temp1324 >  3u?*(( int*) _temp1324) ==  Cyc_Absyn_UnionType:
0){ _LL1331: _temp1330=(( struct Cyc_Absyn_UnionType_struct*) _temp1324)->f1;
goto _LL1329; _LL1329: _temp1328=(( struct Cyc_Absyn_UnionType_struct*)
_temp1324)->f2; goto _LL1327; _LL1327: _temp1326=(( struct Cyc_Absyn_UnionType_struct*)
_temp1324)->f3; goto _LL1317;} else{ goto _LL1230;} _LL1317: _temp1316=
_temp1214.f2; if(( unsigned int) _temp1316 >  3u?*(( int*) _temp1316) ==  Cyc_Absyn_UnionType:
0){ _LL1323: _temp1322=(( struct Cyc_Absyn_UnionType_struct*) _temp1316)->f1;
goto _LL1321; _LL1321: _temp1320=(( struct Cyc_Absyn_UnionType_struct*)
_temp1316)->f2; goto _LL1319; _LL1319: _temp1318=(( struct Cyc_Absyn_UnionType_struct*)
_temp1316)->f3; goto _LL1229;} else{ goto _LL1230;} _LL1230: _LL1346: _temp1345=
_temp1214.f1; if(( unsigned int) _temp1345 >  3u?*(( int*) _temp1345) ==  Cyc_Absyn_TunionType:
0){ _LL1348: _temp1347=(( struct Cyc_Absyn_TunionType_struct*) _temp1345)->f1;
_LL1354: _temp1353=( void*) _temp1347.tunion_info; if(*(( int*) _temp1353) == 
Cyc_Absyn_KnownTunion){ _LL1356: _temp1355=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1353)->f1; _temp1357=* _temp1355; goto _LL1352;} else{ goto _LL1232;}
_LL1352: _temp1351= _temp1347.targs; goto _LL1350; _LL1350: _temp1349=( void*)
_temp1347.rgn; goto _LL1333;} else{ goto _LL1232;} _LL1333: _temp1332= _temp1214.f2;
if(( unsigned int) _temp1332 >  3u?*(( int*) _temp1332) ==  Cyc_Absyn_TunionType:
0){ _LL1335: _temp1334=(( struct Cyc_Absyn_TunionType_struct*) _temp1332)->f1;
_LL1341: _temp1340=( void*) _temp1334.tunion_info; if(*(( int*) _temp1340) == 
Cyc_Absyn_KnownTunion){ _LL1343: _temp1342=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1340)->f1; _temp1344=* _temp1342; goto _LL1339;} else{ goto _LL1232;}
_LL1339: _temp1338= _temp1334.targs; goto _LL1337; _LL1337: _temp1336=( void*)
_temp1334.rgn; goto _LL1231;} else{ goto _LL1232;} _LL1232: _LL1371: _temp1370=
_temp1214.f1; if(( unsigned int) _temp1370 >  3u?*(( int*) _temp1370) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1373: _temp1372=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1370)->f1;
_LL1377: _temp1376=( void*) _temp1372.field_info; if(*(( int*) _temp1376) == 
Cyc_Absyn_KnownTunionfield){ _LL1381: _temp1380=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1376)->f1; goto _LL1379; _LL1379: _temp1378=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1376)->f2; goto _LL1375;} else{ goto _LL1234;} _LL1375: _temp1374=
_temp1372.targs; goto _LL1359;} else{ goto _LL1234;} _LL1359: _temp1358=
_temp1214.f2; if(( unsigned int) _temp1358 >  3u?*(( int*) _temp1358) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1361: _temp1360=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1358)->f1;
_LL1365: _temp1364=( void*) _temp1360.field_info; if(*(( int*) _temp1364) == 
Cyc_Absyn_KnownTunionfield){ _LL1369: _temp1368=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1364)->f1; goto _LL1367; _LL1367: _temp1366=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1364)->f2; goto _LL1363;} else{ goto _LL1234;} _LL1363: _temp1362=
_temp1360.targs; goto _LL1233;} else{ goto _LL1234;} _LL1234: _LL1397: _temp1396=
_temp1214.f1; if(( unsigned int) _temp1396 >  3u?*(( int*) _temp1396) ==  Cyc_Absyn_PointerType:
0){ _LL1399: _temp1398=(( struct Cyc_Absyn_PointerType_struct*) _temp1396)->f1;
_LL1409: _temp1408=( void*) _temp1398.elt_typ; goto _LL1407; _LL1407: _temp1406=(
void*) _temp1398.rgn_typ; goto _LL1405; _LL1405: _temp1404= _temp1398.nullable;
goto _LL1403; _LL1403: _temp1402= _temp1398.tq; goto _LL1401; _LL1401: _temp1400=
_temp1398.bounds; goto _LL1383;} else{ goto _LL1236;} _LL1383: _temp1382=
_temp1214.f2; if(( unsigned int) _temp1382 >  3u?*(( int*) _temp1382) ==  Cyc_Absyn_PointerType:
0){ _LL1385: _temp1384=(( struct Cyc_Absyn_PointerType_struct*) _temp1382)->f1;
_LL1395: _temp1394=( void*) _temp1384.elt_typ; goto _LL1393; _LL1393: _temp1392=(
void*) _temp1384.rgn_typ; goto _LL1391; _LL1391: _temp1390= _temp1384.nullable;
goto _LL1389; _LL1389: _temp1388= _temp1384.tq; goto _LL1387; _LL1387: _temp1386=
_temp1384.bounds; goto _LL1235;} else{ goto _LL1236;} _LL1236: _LL1417:
_temp1416= _temp1214.f1; if(( unsigned int) _temp1416 >  3u?*(( int*) _temp1416)
==  Cyc_Absyn_IntType: 0){ _LL1421: _temp1420=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1416)->f1; goto _LL1419; _LL1419: _temp1418=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1416)->f2; goto _LL1411;} else{ goto _LL1238;} _LL1411: _temp1410=
_temp1214.f2; if(( unsigned int) _temp1410 >  3u?*(( int*) _temp1410) ==  Cyc_Absyn_IntType:
0){ _LL1415: _temp1414=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1410)->f1;
goto _LL1413; _LL1413: _temp1412=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1410)->f2; goto _LL1237;} else{ goto _LL1238;} _LL1238: _LL1425: _temp1424=
_temp1214.f1; if( _temp1424 == ( void*) Cyc_Absyn_FloatType){ goto _LL1423;}
else{ goto _LL1240;} _LL1423: _temp1422= _temp1214.f2; if( _temp1422 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1239;} else{ goto _LL1240;} _LL1240: _LL1431:
_temp1430= _temp1214.f1; if(( unsigned int) _temp1430 >  3u?*(( int*) _temp1430)
==  Cyc_Absyn_DoubleType: 0){ _LL1433: _temp1432=(( struct Cyc_Absyn_DoubleType_struct*)
_temp1430)->f1; goto _LL1427;} else{ goto _LL1242;} _LL1427: _temp1426=
_temp1214.f2; if(( unsigned int) _temp1426 >  3u?*(( int*) _temp1426) ==  Cyc_Absyn_DoubleType:
0){ _LL1429: _temp1428=(( struct Cyc_Absyn_DoubleType_struct*) _temp1426)->f1;
goto _LL1241;} else{ goto _LL1242;} _LL1242: _LL1443: _temp1442= _temp1214.f1;
if(( unsigned int) _temp1442 >  3u?*(( int*) _temp1442) ==  Cyc_Absyn_ArrayType:
0){ _LL1449: _temp1448=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1442)->f1;
goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1442)->f2; goto _LL1445; _LL1445: _temp1444=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1442)->f3; goto _LL1435;} else{ goto _LL1244;} _LL1435: _temp1434=
_temp1214.f2; if(( unsigned int) _temp1434 >  3u?*(( int*) _temp1434) ==  Cyc_Absyn_ArrayType:
0){ _LL1441: _temp1440=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1434)->f1;
goto _LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1434)->f2; goto _LL1437; _LL1437: _temp1436=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1434)->f3; goto _LL1243;} else{ goto _LL1244;} _LL1244: _LL1471: _temp1470=
_temp1214.f1; if(( unsigned int) _temp1470 >  3u?*(( int*) _temp1470) ==  Cyc_Absyn_FnType:
0){ _LL1473: _temp1472=(( struct Cyc_Absyn_FnType_struct*) _temp1470)->f1;
_LL1489: _temp1488= _temp1472.tvars; goto _LL1487; _LL1487: _temp1486= _temp1472.effect;
goto _LL1485; _LL1485: _temp1484=( void*) _temp1472.ret_typ; goto _LL1483;
_LL1483: _temp1482= _temp1472.args; goto _LL1481; _LL1481: _temp1480= _temp1472.c_varargs;
goto _LL1479; _LL1479: _temp1478= _temp1472.cyc_varargs; goto _LL1477; _LL1477:
_temp1476= _temp1472.rgn_po; goto _LL1475; _LL1475: _temp1474= _temp1472.attributes;
goto _LL1451;} else{ goto _LL1246;} _LL1451: _temp1450= _temp1214.f2; if((
unsigned int) _temp1450 >  3u?*(( int*) _temp1450) ==  Cyc_Absyn_FnType: 0){
_LL1453: _temp1452=(( struct Cyc_Absyn_FnType_struct*) _temp1450)->f1; _LL1469:
_temp1468= _temp1452.tvars; goto _LL1467; _LL1467: _temp1466= _temp1452.effect;
goto _LL1465; _LL1465: _temp1464=( void*) _temp1452.ret_typ; goto _LL1463;
_LL1463: _temp1462= _temp1452.args; goto _LL1461; _LL1461: _temp1460= _temp1452.c_varargs;
goto _LL1459; _LL1459: _temp1458= _temp1452.cyc_varargs; goto _LL1457; _LL1457:
_temp1456= _temp1452.rgn_po; goto _LL1455; _LL1455: _temp1454= _temp1452.attributes;
goto _LL1245;} else{ goto _LL1246;} _LL1246: _LL1495: _temp1494= _temp1214.f1;
if(( unsigned int) _temp1494 >  3u?*(( int*) _temp1494) ==  Cyc_Absyn_TupleType:
0){ _LL1497: _temp1496=(( struct Cyc_Absyn_TupleType_struct*) _temp1494)->f1;
goto _LL1491;} else{ goto _LL1248;} _LL1491: _temp1490= _temp1214.f2; if((
unsigned int) _temp1490 >  3u?*(( int*) _temp1490) ==  Cyc_Absyn_TupleType: 0){
_LL1493: _temp1492=(( struct Cyc_Absyn_TupleType_struct*) _temp1490)->f1; goto
_LL1247;} else{ goto _LL1248;} _LL1248: _LL1503: _temp1502= _temp1214.f1; if((
unsigned int) _temp1502 >  3u?*(( int*) _temp1502) ==  Cyc_Absyn_AnonStructType:
0){ _LL1505: _temp1504=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1502)->f1;
goto _LL1499;} else{ goto _LL1250;} _LL1499: _temp1498= _temp1214.f2; if((
unsigned int) _temp1498 >  3u?*(( int*) _temp1498) ==  Cyc_Absyn_AnonStructType:
0){ _LL1501: _temp1500=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1498)->f1;
goto _LL1249;} else{ goto _LL1250;} _LL1250: _LL1511: _temp1510= _temp1214.f1;
if(( unsigned int) _temp1510 >  3u?*(( int*) _temp1510) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1513: _temp1512=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1510)->f1;
goto _LL1507;} else{ goto _LL1252;} _LL1507: _temp1506= _temp1214.f2; if((
unsigned int) _temp1506 >  3u?*(( int*) _temp1506) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1509: _temp1508=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1506)->f1;
goto _LL1251;} else{ goto _LL1252;} _LL1252: _LL1517: _temp1516= _temp1214.f1;
if( _temp1516 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1515;} else{ goto _LL1254;}
_LL1515: _temp1514= _temp1214.f2; if( _temp1514 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1253;} else{ goto _LL1254;} _LL1254: _LL1523: _temp1522= _temp1214.f1;
if(( unsigned int) _temp1522 >  3u?*(( int*) _temp1522) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1525: _temp1524=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1522)->f1; goto _LL1519;} else{ goto _LL1256;} _LL1519: _temp1518=
_temp1214.f2; if(( unsigned int) _temp1518 >  3u?*(( int*) _temp1518) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1521: _temp1520=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1518)->f1; goto _LL1255;} else{ goto _LL1256;} _LL1256: _LL1527: _temp1526=
_temp1214.f1; if(( unsigned int) _temp1526 >  3u?*(( int*) _temp1526) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1257;} else{ goto _LL1258;} _LL1258: _LL1529: _temp1528= _temp1214.f2;
if(( unsigned int) _temp1528 >  3u?*(( int*) _temp1528) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1259;} else{ goto _LL1260;} _LL1260: _LL1531: _temp1530= _temp1214.f1;
if(( unsigned int) _temp1530 >  3u?*(( int*) _temp1530) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1261;} else{ goto _LL1262;} _LL1262: _LL1533: _temp1532= _temp1214.f1;
if(( unsigned int) _temp1532 >  3u?*(( int*) _temp1532) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1263;} else{ goto _LL1264;} _LL1264: _LL1535: _temp1534= _temp1214.f2;
if(( unsigned int) _temp1534 >  3u?*(( int*) _temp1534) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1265;} else{ goto _LL1266;} _LL1266: _LL1537: _temp1536= _temp1214.f2;
if(( unsigned int) _temp1536 >  3u?*(( int*) _temp1536) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1267;} else{ goto _LL1268;} _LL1268: goto _LL1269; _LL1217: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1219: return; _LL1221: { struct _tagged_arr* _temp1538=
_temp1282->name; struct _tagged_arr* _temp1539= _temp1278->name; int _temp1540=*((
int*) _check_null( _temp1282->identity)); int _temp1541=*(( int*) _check_null(
_temp1278->identity)); void* _temp1542= Cyc_Tcutil_tvar_kind( _temp1282); void*
_temp1543= Cyc_Tcutil_tvar_kind( _temp1278); if( _temp1541 ==  _temp1540? Cyc_Std_zstrptrcmp(
_temp1538, _temp1539) ==  0: 0){ if( _temp1542 !=  _temp1543){({ struct Cyc_Std_String_pa_struct
_temp1547; _temp1547.tag= Cyc_Std_String_pa; _temp1547.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( _temp1543);{ struct Cyc_Std_String_pa_struct _temp1546;
_temp1546.tag= Cyc_Std_String_pa; _temp1546.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string(
_temp1542);{ struct Cyc_Std_String_pa_struct _temp1545; _temp1545.tag= Cyc_Std_String_pa;
_temp1545.f1=( struct _tagged_arr)* _temp1538;{ void* _temp1544[ 3u]={&
_temp1545,& _temp1546,& _temp1547};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1544, sizeof( void*), 3u));}}}});}
return;} Cyc_Tcutil_failure_reason= _tag_arr("(variable types are not the same)",
sizeof( unsigned char), 34u); goto _LL1215;} _LL1223: if((( _temp1290 !=  0?
_temp1298 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1290),( struct _tuple1*) _check_null( _temp1298)) ==  0: 0)? 1:( _temp1290
==  0? _temp1298 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1288, _temp1296);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different struct types)", sizeof(
unsigned char), 25u); goto _LL1215; _LL1225: if( Cyc_Absyn_qvar_cmp( _temp1306,
_temp1302) ==  0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different enum types)",
sizeof( unsigned char), 23u); goto _LL1215; _LL1227: { int bad= 0; for( 0;
_temp1314 !=  0? _temp1310 !=  0: 0;( _temp1314= _temp1314->tl, _temp1310=
_temp1310->tl)){ struct Cyc_Absyn_Enumfield* _temp1548=( struct Cyc_Absyn_Enumfield*)
_temp1314->hd; struct Cyc_Absyn_Enumfield* _temp1549=( struct Cyc_Absyn_Enumfield*)
_temp1310->hd; if( Cyc_Absyn_qvar_cmp( _temp1548->name, _temp1549->name) !=  0){
Cyc_Tcutil_failure_reason= _tag_arr("(different names for enum fields)", sizeof(
unsigned char), 34u); bad= 1; break;} if( _temp1548->tag ==  _temp1549->tag){
continue;} if( _temp1548->tag ==  0? 1: _temp1549->tag ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)", sizeof( unsigned char), 39u);
bad= 1; break;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1548->tag)) !=  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1549->tag))){ Cyc_Tcutil_failure_reason= _tag_arr("(different tag values for enum fields)",
sizeof( unsigned char), 39u); bad= 1; break;}} if( bad){ goto _LL1215;} if(
_temp1314 ==  0? _temp1310 ==  0: 0){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)", sizeof( unsigned char), 39u);
goto _LL1215;} _LL1229: if((( _temp1322 !=  0? _temp1330 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1322),( struct _tuple1*) _check_null(
_temp1330)) ==  0: 0)? 1:( _temp1322 ==  0? _temp1330 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1320, _temp1328); return;} Cyc_Tcutil_failure_reason= _tag_arr("(different union types)",
sizeof( unsigned char), 24u); goto _LL1215; _LL1231: if( _temp1357 ==  _temp1344?
1: Cyc_Absyn_qvar_cmp( _temp1357->name, _temp1344->name) ==  0){ Cyc_Tcutil_unify_it(
_temp1336, _temp1349); Cyc_Tcutil_unify_list( _temp1338, _temp1351); return;}
Cyc_Tcutil_failure_reason= _tag_arr("(different tunion types)", sizeof(
unsigned char), 25u); goto _LL1215; _LL1233: if(( _temp1380 ==  _temp1368? 1:
Cyc_Absyn_qvar_cmp( _temp1380->name, _temp1368->name) ==  0)? _temp1378 == 
_temp1366? 1: Cyc_Absyn_qvar_cmp( _temp1378->name, _temp1366->name) ==  0: 0){
Cyc_Tcutil_unify_list( _temp1362, _temp1374); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)", sizeof( unsigned char), 31u); goto
_LL1215; _LL1235: Cyc_Tcutil_unify_it( _temp1394, _temp1408); Cyc_Tcutil_unify_it(
_temp1406, _temp1392); Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_unify_tqual(
_temp1388, _temp1402); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1386, _temp1400, _tag_arr("(different pointer bounds)", sizeof(
unsigned char), 27u));{ void* _temp1550=( void*)( Cyc_Absyn_compress_conref(
_temp1386))->v; void* _temp1556; _LL1552: if(( unsigned int) _temp1550 >  1u?*((
int*) _temp1550) ==  Cyc_Absyn_Eq_constr: 0){ _LL1557: _temp1556=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1550)->f1; if( _temp1556 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1553;} else{ goto _LL1554;}} else{ goto _LL1554;}
_LL1554: goto _LL1555; _LL1553: return; _LL1555: goto _LL1551; _LL1551:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y,
struct _tagged_arr reason)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp1390, _temp1404, _tag_arr("(different pointer types)", sizeof(
unsigned char), 26u)); return; _LL1237: if( _temp1414 ==  _temp1420? _temp1412
==  _temp1418: 0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different integral types)",
sizeof( unsigned char), 27u); goto _LL1215; _LL1239: return; _LL1241: if(
_temp1432 ==  _temp1428){ return;} goto _LL1215; _LL1243: Cyc_Tcutil_unify_tqual(
_temp1438, _temp1446); Cyc_Tcutil_unify_it( _temp1440, _temp1448); if( _temp1444
==  _temp1436){ return;} if( _temp1444 ==  0? 1: _temp1436 ==  0){ goto _LL1215;}
if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1444)) ==  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1436))){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different array sizes)",
sizeof( unsigned char), 24u); goto _LL1215; _LL1245: { int done= 0;{ struct
_RegionHandle _temp1558= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1558; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1468
!=  0) { if( _temp1488 ==  0){ Cyc_Tcutil_failure_reason= _tag_arr("(second function type has too few type variables)",
sizeof( unsigned char), 50u);( int) _throw(( void*) Cyc_Tcutil_Unify);}{ void*
_temp1559= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) _temp1468->hd); void*
_temp1560= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) _temp1488->hd); if(
_temp1559 !=  _temp1560){ Cyc_Tcutil_failure_reason=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp1564; _temp1564.tag= Cyc_Std_String_pa;
_temp1564.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1560);{ struct
Cyc_Std_String_pa_struct _temp1563; _temp1563.tag= Cyc_Std_String_pa; _temp1563.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1559);{ struct Cyc_Std_String_pa_struct
_temp1562; _temp1562.tag= Cyc_Std_String_pa; _temp1562.f1=( struct _tagged_arr)
Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*) _temp1468->hd);{ void*
_temp1561[ 3u]={& _temp1562,& _temp1563,& _temp1564}; Cyc_Std_aprintf( _tag_arr("(type var %s has different kinds %s and %s)",
sizeof( unsigned char), 44u), _tag_arr( _temp1561, sizeof( void*), 3u));}}}});(
int) _throw(( void*) Cyc_Tcutil_Unify);} inst=({ struct Cyc_List_List* _temp1565=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1565->hd=( void*)({ struct _tuple5* _temp1566=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1566->f1=( struct Cyc_Absyn_Tvar*)
_temp1488->hd; _temp1566->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1567=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1567[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1568; _temp1568.tag= Cyc_Absyn_VarType;
_temp1568.f1=( struct Cyc_Absyn_Tvar*) _temp1468->hd; _temp1568;}); _temp1567;});
_temp1566;}); _temp1565->tl= inst; _temp1565;}); _temp1468= _temp1468->tl;
_temp1488= _temp1488->tl;}} if( _temp1488 !=  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too many type variables)", sizeof(
unsigned char), 51u); _npop_handler( 0u); goto _LL1215;} if( inst !=  0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1569=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1569[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1570; _temp1570.tag= Cyc_Absyn_FnType; _temp1570.f1=({ struct Cyc_Absyn_FnInfo
_temp1571; _temp1571.tvars= 0; _temp1571.effect= _temp1466; _temp1571.ret_typ=(
void*) _temp1464; _temp1571.args= _temp1462; _temp1571.c_varargs= _temp1460;
_temp1571.cyc_varargs= _temp1458; _temp1571.rgn_po= _temp1456; _temp1571.attributes=
_temp1454; _temp1571;}); _temp1570;}); _temp1569;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1572=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1572[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1573; _temp1573.tag= Cyc_Absyn_FnType; _temp1573.f1=({ struct Cyc_Absyn_FnInfo
_temp1574; _temp1574.tvars= 0; _temp1574.effect= _temp1486; _temp1574.ret_typ=(
void*) _temp1484; _temp1574.args= _temp1482; _temp1574.c_varargs= _temp1480;
_temp1574.cyc_varargs= _temp1478; _temp1574.rgn_po= _temp1476; _temp1574.attributes=
_temp1474; _temp1574;}); _temp1573;}); _temp1572;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1464, _temp1484); for( 0;
_temp1462 !=  0? _temp1482 !=  0: 0;( _temp1462= _temp1462->tl, _temp1482=
_temp1482->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*) _temp1462->hd)).f2,(*((
struct _tuple2*) _temp1482->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)
_temp1462->hd)).f3,(*(( struct _tuple2*) _temp1482->hd)).f3);} Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; if( _temp1462 !=  0? 1: _temp1482 !=  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)", sizeof(
unsigned char), 52u); goto _LL1215;} if( _temp1460 !=  _temp1480){ Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)", sizeof( unsigned char), 41u);
goto _LL1215;}{ int bad_cyc_vararg= 0;{ struct _tuple11 _temp1576=({ struct
_tuple11 _temp1575; _temp1575.f1= _temp1458; _temp1575.f2= _temp1478; _temp1575;});
struct Cyc_Absyn_VarargInfo* _temp1586; struct Cyc_Absyn_VarargInfo* _temp1588;
struct Cyc_Absyn_VarargInfo* _temp1590; struct Cyc_Absyn_VarargInfo* _temp1592;
struct Cyc_Absyn_VarargInfo* _temp1594; struct Cyc_Absyn_VarargInfo _temp1596;
int _temp1597; void* _temp1599; struct Cyc_Absyn_Tqual _temp1601; struct Cyc_Core_Opt*
_temp1603; struct Cyc_Absyn_VarargInfo* _temp1605; struct Cyc_Absyn_VarargInfo
_temp1607; int _temp1608; void* _temp1610; struct Cyc_Absyn_Tqual _temp1612;
struct Cyc_Core_Opt* _temp1614; _LL1578: _LL1589: _temp1588= _temp1576.f1; if(
_temp1588 ==  0){ goto _LL1587;} else{ goto _LL1580;} _LL1587: _temp1586=
_temp1576.f2; if( _temp1586 ==  0){ goto _LL1579;} else{ goto _LL1580;} _LL1580:
_LL1591: _temp1590= _temp1576.f1; if( _temp1590 ==  0){ goto _LL1581;} else{
goto _LL1582;} _LL1582: _LL1593: _temp1592= _temp1576.f2; if( _temp1592 ==  0){
goto _LL1583;} else{ goto _LL1584;} _LL1584: _LL1606: _temp1605= _temp1576.f1;
if( _temp1605 ==  0){ goto _LL1577;} else{ _temp1607=* _temp1605; _LL1615:
_temp1614= _temp1607.name; goto _LL1613; _LL1613: _temp1612= _temp1607.tq; goto
_LL1611; _LL1611: _temp1610=( void*) _temp1607.type; goto _LL1609; _LL1609:
_temp1608= _temp1607.inject; goto _LL1595;} _LL1595: _temp1594= _temp1576.f2;
if( _temp1594 ==  0){ goto _LL1577;} else{ _temp1596=* _temp1594; _LL1604:
_temp1603= _temp1596.name; goto _LL1602; _LL1602: _temp1601= _temp1596.tq; goto
_LL1600; _LL1600: _temp1599=( void*) _temp1596.type; goto _LL1598; _LL1598:
_temp1597= _temp1596.inject; goto _LL1585;} _LL1579: goto _LL1577; _LL1581: goto
_LL1583; _LL1583: bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type takes varargs)",
sizeof( unsigned char), 39u); goto _LL1577; _LL1585: Cyc_Tcutil_unify_tqual(
_temp1612, _temp1601); Cyc_Tcutil_unify_it( _temp1610, _temp1599); if( _temp1608
!=  _temp1597){ bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type injects varargs)",
sizeof( unsigned char), 41u);} goto _LL1577; _LL1577:;} if( bad_cyc_vararg){
goto _LL1215;}{ int bad_effect= 0;{ struct _tuple12 _temp1617=({ struct _tuple12
_temp1616; _temp1616.f1= _temp1466; _temp1616.f2= _temp1486; _temp1616;});
struct Cyc_Core_Opt* _temp1627; struct Cyc_Core_Opt* _temp1629; struct Cyc_Core_Opt*
_temp1631; struct Cyc_Core_Opt* _temp1633; _LL1619: _LL1630: _temp1629=
_temp1617.f1; if( _temp1629 ==  0){ goto _LL1628;} else{ goto _LL1621;} _LL1628:
_temp1627= _temp1617.f2; if( _temp1627 ==  0){ goto _LL1620;} else{ goto _LL1621;}
_LL1621: _LL1632: _temp1631= _temp1617.f1; if( _temp1631 ==  0){ goto _LL1622;}
else{ goto _LL1623;} _LL1623: _LL1634: _temp1633= _temp1617.f2; if( _temp1633 == 
0){ goto _LL1624;} else{ goto _LL1625;} _LL1625: goto _LL1626; _LL1620: goto
_LL1618; _LL1622: goto _LL1624; _LL1624: bad_effect= 1; goto _LL1618; _LL1626:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1466))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1486))->v); goto _LL1618; _LL1618:;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; if( bad_effect){ Cyc_Tcutil_failure_reason= _tag_arr("(function type effects do not unify)",
sizeof( unsigned char), 37u); goto _LL1215;} if( ! Cyc_Tcutil_same_atts(
_temp1474, _temp1454)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different attributes)",
sizeof( unsigned char), 43u); goto _LL1215;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1476, _temp1456)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different region lifetime orderings)",
sizeof( unsigned char), 58u); goto _LL1215;} return;}}} _LL1247: for( 0;
_temp1492 !=  0? _temp1496 !=  0: 0;( _temp1492= _temp1492->tl, _temp1496=
_temp1496->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*) _temp1492->hd)).f1,(*((
struct _tuple4*) _temp1496->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)
_temp1492->hd)).f2,(*(( struct _tuple4*) _temp1496->hd)).f2);} if( _temp1492 == 
0? _temp1496 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(tuple types have different numbers of components)",
sizeof( unsigned char), 51u); goto _LL1215; _LL1249: _temp1512= _temp1504;
_temp1508= _temp1500; goto _LL1251; _LL1251: for( 0; _temp1508 !=  0? _temp1512
!=  0: 0;( _temp1508= _temp1508->tl, _temp1512= _temp1512->tl)){ struct Cyc_Absyn_Structfield*
_temp1635=( struct Cyc_Absyn_Structfield*) _temp1508->hd; struct Cyc_Absyn_Structfield*
_temp1636=( struct Cyc_Absyn_Structfield*) _temp1512->hd; if( Cyc_Std_zstrptrcmp(
_temp1635->name, _temp1636->name) !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(different member names in anonymous structs/unions)",
sizeof( unsigned char), 53u);( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1635->tq, _temp1636->tq); Cyc_Tcutil_unify_it(( void*) _temp1635->type,(
void*) _temp1636->type); if( ! Cyc_Tcutil_same_atts( _temp1635->attributes,
_temp1636->attributes)){ Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2;
Cyc_Tcutil_failure_reason= _tag_arr("(different attributes on member)", sizeof(
unsigned char), 33u);( int) _throw(( void*) Cyc_Tcutil_Unify);} if((( _temp1635->width
!=  0? _temp1636->width ==  0: 0)? 1:( _temp1636->width !=  0? _temp1635->width
==  0: 0))? 1:(( _temp1635->width !=  0? _temp1636->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1635->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1636->width)): 0)){ Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason= _tag_arr("(different bitfield widths on member)",
sizeof( unsigned char), 38u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} if(
_temp1508 ==  0? _temp1512 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(different number of members)", sizeof(
unsigned char), 30u); goto _LL1215; _LL1253: return; _LL1255: Cyc_Tcutil_unify_it(
_temp1524, _temp1520); return; _LL1257: goto _LL1259; _LL1259: goto _LL1261;
_LL1261: goto _LL1263; _LL1263: goto _LL1265; _LL1265: goto _LL1267; _LL1267:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)", sizeof( unsigned char), 22u); goto _LL1215;
_LL1269: goto _LL1215; _LL1215:;}( int) _throw(( void*) Cyc_Tcutil_Unify);} int
Cyc_Tcutil_opt_cmp( int(* cmp)( void*, void*), struct Cyc_Core_Opt* a1, struct
Cyc_Core_Opt* a2){ if( a1 ==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return
- 1;} if( a1 !=  0? a2 ==  0: 0){ return 1;} return cmp(( void*)(( struct Cyc_Core_Opt*)
_check_null( a1))->v,( void*)(( struct Cyc_Core_Opt*) _check_null( a2))->v);}
int Cyc_Tcutil_star_cmp( int(* cmp)( void**, void**), void** a1, void** a2){ if(
a1 ==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return - 1;} if( a1 !=  0? a2
==  0: 0){ return 1;} return cmp(( void**) _check_null( a1),( void**)
_check_null( a2));} int Cyc_Tcutil_list_cmp( int(* cmp)( void*, void*), struct
Cyc_List_List* l1, struct Cyc_List_List* l2){ if( l1 ==  l2){ return 0;} for( 0;
l1 !=  0? l2 !=  0: 0;( l1= l1->tl, l2= l2->tl)){ int _temp1637= cmp(( void*) l1->hd,(
void*) l2->hd); if( _temp1637 !=  0){ return _temp1637;}} if( l2 !=  0){ return
- 1;} if( l1 !=  0){ return 1;} return 0;} static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ int _temp1638=( tq1.q_const
+ ( tq1.q_volatile <<  1)) + ( tq1.q_restrict <<  2); int _temp1639=( tq2.q_const
+ ( tq2.q_volatile <<  1)) + ( tq2.q_restrict <<  2); return Cyc_Core_intcmp(
_temp1638, _temp1639);} static int Cyc_Tcutil_conrefs_cmp( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ x= Cyc_Absyn_compress_conref(
x); y= Cyc_Absyn_compress_conref( y); if( x ==  y){ return 0;}{ void* _temp1640=(
void*) x->v; void* _temp1648; _LL1642: if( _temp1640 == ( void*) Cyc_Absyn_No_constr){
goto _LL1643;} else{ goto _LL1644;} _LL1644: if(( unsigned int) _temp1640 >  1u?*((
int*) _temp1640) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL1645;} else{ goto
_LL1646;} _LL1646: if(( unsigned int) _temp1640 >  1u?*(( int*) _temp1640) == 
Cyc_Absyn_Eq_constr: 0){ _LL1649: _temp1648=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1640)->f1; goto _LL1647;} else{ goto _LL1641;} _LL1643: return - 1; _LL1645:(
int) _throw(({ void* _temp1650[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress",
sizeof( unsigned char), 37u), _tag_arr( _temp1650, sizeof( void*), 0u));}));
_LL1647: { void* _temp1651=( void*) y->v; void* _temp1659; _LL1653: if(
_temp1651 == ( void*) Cyc_Absyn_No_constr){ goto _LL1654;} else{ goto _LL1655;}
_LL1655: if(( unsigned int) _temp1651 >  1u?*(( int*) _temp1651) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL1656;} else{ goto _LL1657;} _LL1657: if(( unsigned int) _temp1651 > 
1u?*(( int*) _temp1651) ==  Cyc_Absyn_Eq_constr: 0){ _LL1660: _temp1659=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1651)->f1; goto _LL1658;} else{ goto
_LL1652;} _LL1654: return 1; _LL1656:( int) _throw(({ void* _temp1661[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress(2)", sizeof(
unsigned char), 40u), _tag_arr( _temp1661, sizeof( void*), 0u));})); _LL1658:
return cmp( _temp1648, _temp1659); _LL1652:;} _LL1641:;}} static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4* tqt1, struct _tuple4* tqt2){ struct _tuple4 _temp1664; void*
_temp1665; struct Cyc_Absyn_Tqual _temp1667; struct _tuple4* _temp1662= tqt1;
_temp1664=* _temp1662; _LL1668: _temp1667= _temp1664.f1; goto _LL1666; _LL1666:
_temp1665= _temp1664.f2; goto _LL1663; _LL1663: { struct _tuple4 _temp1671; void*
_temp1672; struct Cyc_Absyn_Tqual _temp1674; struct _tuple4* _temp1669= tqt2;
_temp1671=* _temp1669; _LL1675: _temp1674= _temp1671.f1; goto _LL1673; _LL1673:
_temp1672= _temp1671.f2; goto _LL1670; _LL1670: { int _temp1676= Cyc_Tcutil_tqual_cmp(
_temp1667, _temp1674); if( _temp1676 !=  0){ return _temp1676;} return Cyc_Tcutil_typecmp(
_temp1665, _temp1672);}}} static int Cyc_Tcutil_const_uint_exp_cmp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ return Cyc_Core_intcmp(( int) Cyc_Evexp_eval_const_uint_exp(
e1),( int) Cyc_Evexp_eval_const_uint_exp( e2));} static int Cyc_Tcutil_attribute_case_number(
void* att){ void* _temp1677= att; _LL1679: if(( unsigned int) _temp1677 >  16u?*((
int*) _temp1677) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL1680;} else{ goto
_LL1681;} _LL1681: if( _temp1677 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1682;} else{ goto _LL1683;} _LL1683: if( _temp1677 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1684;} else{ goto _LL1685;} _LL1685: if( _temp1677 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1686;} else{ goto _LL1687;} _LL1687: if( _temp1677 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL1688;} else{ goto _LL1689;} _LL1689: if( _temp1677 == ( void*) Cyc_Absyn_Const_att){
goto _LL1690;} else{ goto _LL1691;} _LL1691: if(( unsigned int) _temp1677 >  16u?*((
int*) _temp1677) ==  Cyc_Absyn_Aligned_att: 0){ goto _LL1692;} else{ goto
_LL1693;} _LL1693: if( _temp1677 == ( void*) Cyc_Absyn_Packed_att){ goto _LL1694;}
else{ goto _LL1695;} _LL1695: if(( unsigned int) _temp1677 >  16u?*(( int*)
_temp1677) ==  Cyc_Absyn_Section_att: 0){ goto _LL1696;} else{ goto _LL1697;}
_LL1697: if( _temp1677 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL1698;} else{
goto _LL1699;} _LL1699: if( _temp1677 == ( void*) Cyc_Absyn_Shared_att){ goto
_LL1700;} else{ goto _LL1701;} _LL1701: if( _temp1677 == ( void*) Cyc_Absyn_Unused_att){
goto _LL1702;} else{ goto _LL1703;} _LL1703: if( _temp1677 == ( void*) Cyc_Absyn_Weak_att){
goto _LL1704;} else{ goto _LL1705;} _LL1705: if( _temp1677 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL1706;} else{ goto _LL1707;} _LL1707: if( _temp1677 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL1708;} else{ goto _LL1709;} _LL1709: if( _temp1677 == ( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL1710;} else{ goto _LL1711;} _LL1711: if( _temp1677 == ( void*) Cyc_Absyn_Constructor_att){
goto _LL1712;} else{ goto _LL1713;} _LL1713: if( _temp1677 == ( void*) Cyc_Absyn_Destructor_att){
goto _LL1714;} else{ goto _LL1715;} _LL1715: if( _temp1677 == ( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL1716;} else{ goto _LL1717;} _LL1717: if(( unsigned int) _temp1677 >  16u?*((
int*) _temp1677) ==  Cyc_Absyn_Format_att: 0){ goto _LL1718;} else{ goto _LL1678;}
_LL1680: return 0; _LL1682: return 1; _LL1684: return 2; _LL1686: return 3;
_LL1688: return 4; _LL1690: return 5; _LL1692: return 6; _LL1694: return 7;
_LL1696: return 8; _LL1698: return 9; _LL1700: return 10; _LL1702: return 11;
_LL1704: return 12; _LL1706: return 13; _LL1708: return 14; _LL1710: return 15;
_LL1712: return 16; _LL1714: return 17; _LL1716: return 18; _LL1718: return 19;
_LL1678:;} static int Cyc_Tcutil_attribute_cmp( void* att1, void* att2){ struct
_tuple8 _temp1720=({ struct _tuple8 _temp1719; _temp1719.f1= att1; _temp1719.f2=
att2; _temp1719;}); void* _temp1732; int _temp1734; void* _temp1736; int
_temp1738; void* _temp1740; int _temp1742; void* _temp1744; int _temp1746; void*
_temp1748; struct _tagged_arr _temp1750; void* _temp1752; struct _tagged_arr
_temp1754; void* _temp1756; int _temp1758; int _temp1760; void* _temp1762; void*
_temp1764; int _temp1766; int _temp1768; void* _temp1770; _LL1722: _LL1737:
_temp1736= _temp1720.f1; if(( unsigned int) _temp1736 >  16u?*(( int*) _temp1736)
==  Cyc_Absyn_Regparm_att: 0){ _LL1739: _temp1738=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp1736)->f1; goto _LL1733;} else{ goto _LL1724;} _LL1733: _temp1732=
_temp1720.f2; if(( unsigned int) _temp1732 >  16u?*(( int*) _temp1732) ==  Cyc_Absyn_Regparm_att:
0){ _LL1735: _temp1734=(( struct Cyc_Absyn_Regparm_att_struct*) _temp1732)->f1;
goto _LL1723;} else{ goto _LL1724;} _LL1724: _LL1745: _temp1744= _temp1720.f1;
if(( unsigned int) _temp1744 >  16u?*(( int*) _temp1744) ==  Cyc_Absyn_Aligned_att:
0){ _LL1747: _temp1746=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1744)->f1;
goto _LL1741;} else{ goto _LL1726;} _LL1741: _temp1740= _temp1720.f2; if((
unsigned int) _temp1740 >  16u?*(( int*) _temp1740) ==  Cyc_Absyn_Aligned_att: 0){
_LL1743: _temp1742=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1740)->f1; goto
_LL1725;} else{ goto _LL1726;} _LL1726: _LL1753: _temp1752= _temp1720.f1; if((
unsigned int) _temp1752 >  16u?*(( int*) _temp1752) ==  Cyc_Absyn_Section_att: 0){
_LL1755: _temp1754=(( struct Cyc_Absyn_Section_att_struct*) _temp1752)->f1; goto
_LL1749;} else{ goto _LL1728;} _LL1749: _temp1748= _temp1720.f2; if((
unsigned int) _temp1748 >  16u?*(( int*) _temp1748) ==  Cyc_Absyn_Section_att: 0){
_LL1751: _temp1750=(( struct Cyc_Absyn_Section_att_struct*) _temp1748)->f1; goto
_LL1727;} else{ goto _LL1728;} _LL1728: _LL1765: _temp1764= _temp1720.f1; if((
unsigned int) _temp1764 >  16u?*(( int*) _temp1764) ==  Cyc_Absyn_Format_att: 0){
_LL1771: _temp1770=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1764)->f1;
goto _LL1769; _LL1769: _temp1768=(( struct Cyc_Absyn_Format_att_struct*)
_temp1764)->f2; goto _LL1767; _LL1767: _temp1766=(( struct Cyc_Absyn_Format_att_struct*)
_temp1764)->f3; goto _LL1757;} else{ goto _LL1730;} _LL1757: _temp1756=
_temp1720.f2; if(( unsigned int) _temp1756 >  16u?*(( int*) _temp1756) ==  Cyc_Absyn_Format_att:
0){ _LL1763: _temp1762=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1756)->f1;
goto _LL1761; _LL1761: _temp1760=(( struct Cyc_Absyn_Format_att_struct*)
_temp1756)->f2; goto _LL1759; _LL1759: _temp1758=(( struct Cyc_Absyn_Format_att_struct*)
_temp1756)->f3; goto _LL1729;} else{ goto _LL1730;} _LL1730: goto _LL1731;
_LL1723: _temp1746= _temp1738; _temp1742= _temp1734; goto _LL1725; _LL1725:
return Cyc_Core_intcmp( _temp1746, _temp1742); _LL1727: return Cyc_Std_strcmp(
_temp1754, _temp1750); _LL1729: { int _temp1772= Cyc_Core_intcmp(( int)((
unsigned int) _temp1770),( int)(( unsigned int) _temp1762)); if( _temp1772 !=  0){
return _temp1772;}{ int _temp1773= Cyc_Core_intcmp( _temp1768, _temp1760); if(
_temp1773 !=  0){ return _temp1773;} return Cyc_Core_intcmp( _temp1766,
_temp1758);}} _LL1731: { int _temp1774= Cyc_Tcutil_attribute_case_number( att1);
int _temp1775= Cyc_Tcutil_attribute_case_number( att2); return Cyc_Core_intcmp(
_temp1774, _temp1775);} _LL1721:;} static int Cyc_Tcutil_structfield_cmp( struct
Cyc_Absyn_Structfield* f1, struct Cyc_Absyn_Structfield* f2){ int _temp1776= Cyc_Std_zstrptrcmp(
f1->name, f2->name); if( _temp1776 !=  0){ return _temp1776;}{ int _temp1777=
Cyc_Tcutil_tqual_cmp( f1->tq, f2->tq); if( _temp1777 !=  0){ return _temp1777;}{
int _temp1778= Cyc_Tcutil_typecmp(( void*) f1->type,( void*) f2->type); if(
_temp1778 !=  0){ return _temp1778;}{ int _temp1779= Cyc_Tcutil_list_cmp( Cyc_Tcutil_attribute_cmp,
f1->attributes, f2->attributes); if( _temp1779 !=  0){ return _temp1779;} return((
int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp*
a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
f1->width, f2->width);}}}} static int Cyc_Tcutil_enumfield_cmp( struct Cyc_Absyn_Enumfield*
e1, struct Cyc_Absyn_Enumfield* e2){ int _temp1780= Cyc_Absyn_qvar_cmp( e1->name,
e2->name); if( _temp1780 !=  0){ return _temp1780;} return(( int(*)( int(* cmp)(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp* a1, struct
Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp, e1->tag,
e2->tag);} extern int Cyc_Tcutil_typecmp( void* t1, void* t2); int Cyc_Tcutil_type_case_number(
void* t){ void* _temp1781= t; _LL1783: if( _temp1781 == ( void*) Cyc_Absyn_VoidType){
goto _LL1784;} else{ goto _LL1785;} _LL1785: if(( unsigned int) _temp1781 >  3u?*((
int*) _temp1781) ==  Cyc_Absyn_Evar: 0){ goto _LL1786;} else{ goto _LL1787;}
_LL1787: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781) ==  Cyc_Absyn_VarType:
0){ goto _LL1788;} else{ goto _LL1789;} _LL1789: if(( unsigned int) _temp1781 > 
3u?*(( int*) _temp1781) ==  Cyc_Absyn_TunionType: 0){ goto _LL1790;} else{ goto
_LL1791;} _LL1791: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1792;} else{ goto _LL1793;} _LL1793: if((
unsigned int) _temp1781 >  3u?*(( int*) _temp1781) ==  Cyc_Absyn_PointerType: 0){
goto _LL1794;} else{ goto _LL1795;} _LL1795: if(( unsigned int) _temp1781 >  3u?*((
int*) _temp1781) ==  Cyc_Absyn_IntType: 0){ goto _LL1796;} else{ goto _LL1797;}
_LL1797: if( _temp1781 == ( void*) Cyc_Absyn_FloatType){ goto _LL1798;} else{
goto _LL1799;} _LL1799: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781)
==  Cyc_Absyn_DoubleType: 0){ goto _LL1800;} else{ goto _LL1801;} _LL1801: if((
unsigned int) _temp1781 >  3u?*(( int*) _temp1781) ==  Cyc_Absyn_ArrayType: 0){
goto _LL1802;} else{ goto _LL1803;} _LL1803: if(( unsigned int) _temp1781 >  3u?*((
int*) _temp1781) ==  Cyc_Absyn_FnType: 0){ goto _LL1804;} else{ goto _LL1805;}
_LL1805: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781) ==  Cyc_Absyn_TupleType:
0){ goto _LL1806;} else{ goto _LL1807;} _LL1807: if(( unsigned int) _temp1781 > 
3u?*(( int*) _temp1781) ==  Cyc_Absyn_StructType: 0){ goto _LL1808;} else{ goto
_LL1809;} _LL1809: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781) == 
Cyc_Absyn_UnionType: 0){ goto _LL1810;} else{ goto _LL1811;} _LL1811: if((
unsigned int) _temp1781 >  3u?*(( int*) _temp1781) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL1812;} else{ goto _LL1813;} _LL1813: if(( unsigned int) _temp1781 > 
3u?*(( int*) _temp1781) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL1814;} else{
goto _LL1815;} _LL1815: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781)
==  Cyc_Absyn_EnumType: 0){ goto _LL1816;} else{ goto _LL1817;} _LL1817: if((
unsigned int) _temp1781 >  3u?*(( int*) _temp1781) ==  Cyc_Absyn_AnonEnumType: 0){
goto _LL1818;} else{ goto _LL1819;} _LL1819: if(( unsigned int) _temp1781 >  3u?*((
int*) _temp1781) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL1820;} else{ goto
_LL1821;} _LL1821: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781) == 
Cyc_Absyn_TypedefType: 0){ goto _LL1822;} else{ goto _LL1823;} _LL1823: if(
_temp1781 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1824;} else{ goto _LL1825;}
_LL1825: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1826;} else{ goto _LL1827;} _LL1827: if(( unsigned int) _temp1781 > 
3u?*(( int*) _temp1781) ==  Cyc_Absyn_JoinEff: 0){ goto _LL1828;} else{ goto
_LL1829;} _LL1829: if(( unsigned int) _temp1781 >  3u?*(( int*) _temp1781) == 
Cyc_Absyn_RgnsEff: 0){ goto _LL1830;} else{ goto _LL1782;} _LL1784: return 0;
_LL1786: return 1; _LL1788: return 2; _LL1790: return 3; _LL1792: return 4;
_LL1794: return 5; _LL1796: return 6; _LL1798: return 7; _LL1800: return 8;
_LL1802: return 9; _LL1804: return 10; _LL1806: return 11; _LL1808: return 12;
_LL1810: return 13; _LL1812: return 14; _LL1814: return 15; _LL1816: return 16;
_LL1818: return 17; _LL1820: return 18; _LL1822: return 19; _LL1824: return 20;
_LL1826: return 21; _LL1828: return 22; _LL1830: return 23; _LL1782:;} int Cyc_Tcutil_shallow_typecmp_it(
void* t1, void* t2){ return Cyc_Core_intcmp( Cyc_Tcutil_type_case_number( t1),
Cyc_Tcutil_type_case_number( t2));} int Cyc_Tcutil_typecmp( void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 ==  t2){
return 0;}{ int _temp1831= Cyc_Tcutil_shallow_typecmp_it( t1, t2); if( _temp1831
!=  0){ return _temp1831;}{ struct _tuple8 _temp1833=({ struct _tuple8 _temp1832;
_temp1832.f1= t2; _temp1832.f2= t1; _temp1832;}); void* _temp1889; void*
_temp1891; void* _temp1893; void* _temp1895; void* _temp1897; struct Cyc_Absyn_Tvar*
_temp1899; void* _temp1901; struct Cyc_Absyn_Tvar* _temp1903; void* _temp1905;
struct Cyc_List_List* _temp1907; struct _tuple1* _temp1909; void* _temp1911;
struct Cyc_List_List* _temp1913; struct _tuple1* _temp1915; void* _temp1917;
struct _tuple1* _temp1919; void* _temp1921; struct _tuple1* _temp1923; void*
_temp1925; struct Cyc_List_List* _temp1927; void* _temp1929; struct Cyc_List_List*
_temp1931; void* _temp1933; struct Cyc_Absyn_Uniondecl** _temp1935; struct Cyc_List_List*
_temp1937; struct _tuple1* _temp1939; void* _temp1941; struct Cyc_Absyn_Uniondecl**
_temp1943; struct Cyc_List_List* _temp1945; struct _tuple1* _temp1947; void*
_temp1949; struct Cyc_Absyn_TunionInfo _temp1951; void* _temp1953; struct Cyc_List_List*
_temp1955; void* _temp1957; struct Cyc_Absyn_Tuniondecl** _temp1959; struct Cyc_Absyn_Tuniondecl*
_temp1961; void* _temp1962; struct Cyc_Absyn_TunionInfo _temp1964; void*
_temp1966; struct Cyc_List_List* _temp1968; void* _temp1970; struct Cyc_Absyn_Tuniondecl**
_temp1972; struct Cyc_Absyn_Tuniondecl* _temp1974; void* _temp1975; struct Cyc_Absyn_TunionFieldInfo
_temp1977; struct Cyc_List_List* _temp1979; void* _temp1981; struct Cyc_Absyn_Tunionfield*
_temp1983; struct Cyc_Absyn_Tuniondecl* _temp1985; void* _temp1987; struct Cyc_Absyn_TunionFieldInfo
_temp1989; struct Cyc_List_List* _temp1991; void* _temp1993; struct Cyc_Absyn_Tunionfield*
_temp1995; struct Cyc_Absyn_Tuniondecl* _temp1997; void* _temp1999; struct Cyc_Absyn_PtrInfo
_temp2001; struct Cyc_Absyn_Conref* _temp2003; struct Cyc_Absyn_Tqual _temp2005;
struct Cyc_Absyn_Conref* _temp2007; void* _temp2009; void* _temp2011; void*
_temp2013; struct Cyc_Absyn_PtrInfo _temp2015; struct Cyc_Absyn_Conref*
_temp2017; struct Cyc_Absyn_Tqual _temp2019; struct Cyc_Absyn_Conref* _temp2021;
void* _temp2023; void* _temp2025; void* _temp2027; void* _temp2029; void*
_temp2031; void* _temp2033; void* _temp2035; void* _temp2037; void* _temp2039;
void* _temp2041; void* _temp2043; int _temp2045; void* _temp2047; int _temp2049;
void* _temp2051; struct Cyc_Absyn_Exp* _temp2053; struct Cyc_Absyn_Tqual
_temp2055; void* _temp2057; void* _temp2059; struct Cyc_Absyn_Exp* _temp2061;
struct Cyc_Absyn_Tqual _temp2063; void* _temp2065; void* _temp2067; struct Cyc_Absyn_FnInfo
_temp2069; struct Cyc_List_List* _temp2071; struct Cyc_List_List* _temp2073;
struct Cyc_Absyn_VarargInfo* _temp2075; int _temp2077; struct Cyc_List_List*
_temp2079; void* _temp2081; struct Cyc_Core_Opt* _temp2083; struct Cyc_List_List*
_temp2085; void* _temp2087; struct Cyc_Absyn_FnInfo _temp2089; struct Cyc_List_List*
_temp2091; struct Cyc_List_List* _temp2093; struct Cyc_Absyn_VarargInfo*
_temp2095; int _temp2097; struct Cyc_List_List* _temp2099; void* _temp2101;
struct Cyc_Core_Opt* _temp2103; struct Cyc_List_List* _temp2105; void* _temp2107;
struct Cyc_List_List* _temp2109; void* _temp2111; struct Cyc_List_List*
_temp2113; void* _temp2115; struct Cyc_List_List* _temp2117; void* _temp2119;
struct Cyc_List_List* _temp2121; void* _temp2123; struct Cyc_List_List*
_temp2125; void* _temp2127; struct Cyc_List_List* _temp2129; void* _temp2131;
void* _temp2133; void* _temp2135; void* _temp2137; void* _temp2139; void*
_temp2141; void* _temp2143; void* _temp2145; void* _temp2147; void* _temp2149;
void* _temp2151; void* _temp2153; _LL1835: _LL1892: _temp1891= _temp1833.f1; if((
unsigned int) _temp1891 >  3u?*(( int*) _temp1891) ==  Cyc_Absyn_Evar: 0){ goto
_LL1890;} else{ goto _LL1837;} _LL1890: _temp1889= _temp1833.f2; if((
unsigned int) _temp1889 >  3u?*(( int*) _temp1889) ==  Cyc_Absyn_Evar: 0){ goto
_LL1836;} else{ goto _LL1837;} _LL1837: _LL1896: _temp1895= _temp1833.f1; if(
_temp1895 == ( void*) Cyc_Absyn_VoidType){ goto _LL1894;} else{ goto _LL1839;}
_LL1894: _temp1893= _temp1833.f2; if( _temp1893 == ( void*) Cyc_Absyn_VoidType){
goto _LL1838;} else{ goto _LL1839;} _LL1839: _LL1902: _temp1901= _temp1833.f1;
if(( unsigned int) _temp1901 >  3u?*(( int*) _temp1901) ==  Cyc_Absyn_VarType: 0){
_LL1904: _temp1903=(( struct Cyc_Absyn_VarType_struct*) _temp1901)->f1; goto
_LL1898;} else{ goto _LL1841;} _LL1898: _temp1897= _temp1833.f2; if((
unsigned int) _temp1897 >  3u?*(( int*) _temp1897) ==  Cyc_Absyn_VarType: 0){
_LL1900: _temp1899=(( struct Cyc_Absyn_VarType_struct*) _temp1897)->f1; goto
_LL1840;} else{ goto _LL1841;} _LL1841: _LL1912: _temp1911= _temp1833.f1; if((
unsigned int) _temp1911 >  3u?*(( int*) _temp1911) ==  Cyc_Absyn_StructType: 0){
_LL1916: _temp1915=(( struct Cyc_Absyn_StructType_struct*) _temp1911)->f1; goto
_LL1914; _LL1914: _temp1913=(( struct Cyc_Absyn_StructType_struct*) _temp1911)->f2;
goto _LL1906;} else{ goto _LL1843;} _LL1906: _temp1905= _temp1833.f2; if((
unsigned int) _temp1905 >  3u?*(( int*) _temp1905) ==  Cyc_Absyn_StructType: 0){
_LL1910: _temp1909=(( struct Cyc_Absyn_StructType_struct*) _temp1905)->f1; goto
_LL1908; _LL1908: _temp1907=(( struct Cyc_Absyn_StructType_struct*) _temp1905)->f2;
goto _LL1842;} else{ goto _LL1843;} _LL1843: _LL1922: _temp1921= _temp1833.f1;
if(( unsigned int) _temp1921 >  3u?*(( int*) _temp1921) ==  Cyc_Absyn_EnumType:
0){ _LL1924: _temp1923=(( struct Cyc_Absyn_EnumType_struct*) _temp1921)->f1;
goto _LL1918;} else{ goto _LL1845;} _LL1918: _temp1917= _temp1833.f2; if((
unsigned int) _temp1917 >  3u?*(( int*) _temp1917) ==  Cyc_Absyn_EnumType: 0){
_LL1920: _temp1919=(( struct Cyc_Absyn_EnumType_struct*) _temp1917)->f1; goto
_LL1844;} else{ goto _LL1845;} _LL1845: _LL1930: _temp1929= _temp1833.f1; if((
unsigned int) _temp1929 >  3u?*(( int*) _temp1929) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1932: _temp1931=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1929)->f1;
goto _LL1926;} else{ goto _LL1847;} _LL1926: _temp1925= _temp1833.f2; if((
unsigned int) _temp1925 >  3u?*(( int*) _temp1925) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1928: _temp1927=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1925)->f1;
goto _LL1846;} else{ goto _LL1847;} _LL1847: _LL1942: _temp1941= _temp1833.f1;
if(( unsigned int) _temp1941 >  3u?*(( int*) _temp1941) ==  Cyc_Absyn_UnionType:
0){ _LL1948: _temp1947=(( struct Cyc_Absyn_UnionType_struct*) _temp1941)->f1;
goto _LL1946; _LL1946: _temp1945=(( struct Cyc_Absyn_UnionType_struct*)
_temp1941)->f2; goto _LL1944; _LL1944: _temp1943=(( struct Cyc_Absyn_UnionType_struct*)
_temp1941)->f3; goto _LL1934;} else{ goto _LL1849;} _LL1934: _temp1933=
_temp1833.f2; if(( unsigned int) _temp1933 >  3u?*(( int*) _temp1933) ==  Cyc_Absyn_UnionType:
0){ _LL1940: _temp1939=(( struct Cyc_Absyn_UnionType_struct*) _temp1933)->f1;
goto _LL1938; _LL1938: _temp1937=(( struct Cyc_Absyn_UnionType_struct*)
_temp1933)->f2; goto _LL1936; _LL1936: _temp1935=(( struct Cyc_Absyn_UnionType_struct*)
_temp1933)->f3; goto _LL1848;} else{ goto _LL1849;} _LL1849: _LL1963: _temp1962=
_temp1833.f1; if(( unsigned int) _temp1962 >  3u?*(( int*) _temp1962) ==  Cyc_Absyn_TunionType:
0){ _LL1965: _temp1964=(( struct Cyc_Absyn_TunionType_struct*) _temp1962)->f1;
_LL1971: _temp1970=( void*) _temp1964.tunion_info; if(*(( int*) _temp1970) == 
Cyc_Absyn_KnownTunion){ _LL1973: _temp1972=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1970)->f1; _temp1974=* _temp1972; goto _LL1969;} else{ goto _LL1851;}
_LL1969: _temp1968= _temp1964.targs; goto _LL1967; _LL1967: _temp1966=( void*)
_temp1964.rgn; goto _LL1950;} else{ goto _LL1851;} _LL1950: _temp1949= _temp1833.f2;
if(( unsigned int) _temp1949 >  3u?*(( int*) _temp1949) ==  Cyc_Absyn_TunionType:
0){ _LL1952: _temp1951=(( struct Cyc_Absyn_TunionType_struct*) _temp1949)->f1;
_LL1958: _temp1957=( void*) _temp1951.tunion_info; if(*(( int*) _temp1957) == 
Cyc_Absyn_KnownTunion){ _LL1960: _temp1959=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1957)->f1; _temp1961=* _temp1959; goto _LL1956;} else{ goto _LL1851;}
_LL1956: _temp1955= _temp1951.targs; goto _LL1954; _LL1954: _temp1953=( void*)
_temp1951.rgn; goto _LL1850;} else{ goto _LL1851;} _LL1851: _LL1988: _temp1987=
_temp1833.f1; if(( unsigned int) _temp1987 >  3u?*(( int*) _temp1987) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1990: _temp1989=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1987)->f1;
_LL1994: _temp1993=( void*) _temp1989.field_info; if(*(( int*) _temp1993) == 
Cyc_Absyn_KnownTunionfield){ _LL1998: _temp1997=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1993)->f1; goto _LL1996; _LL1996: _temp1995=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1993)->f2; goto _LL1992;} else{ goto _LL1853;} _LL1992: _temp1991=
_temp1989.targs; goto _LL1976;} else{ goto _LL1853;} _LL1976: _temp1975=
_temp1833.f2; if(( unsigned int) _temp1975 >  3u?*(( int*) _temp1975) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1978: _temp1977=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1975)->f1;
_LL1982: _temp1981=( void*) _temp1977.field_info; if(*(( int*) _temp1981) == 
Cyc_Absyn_KnownTunionfield){ _LL1986: _temp1985=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1981)->f1; goto _LL1984; _LL1984: _temp1983=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1981)->f2; goto _LL1980;} else{ goto _LL1853;} _LL1980: _temp1979=
_temp1977.targs; goto _LL1852;} else{ goto _LL1853;} _LL1853: _LL2014: _temp2013=
_temp1833.f1; if(( unsigned int) _temp2013 >  3u?*(( int*) _temp2013) ==  Cyc_Absyn_PointerType:
0){ _LL2016: _temp2015=(( struct Cyc_Absyn_PointerType_struct*) _temp2013)->f1;
_LL2026: _temp2025=( void*) _temp2015.elt_typ; goto _LL2024; _LL2024: _temp2023=(
void*) _temp2015.rgn_typ; goto _LL2022; _LL2022: _temp2021= _temp2015.nullable;
goto _LL2020; _LL2020: _temp2019= _temp2015.tq; goto _LL2018; _LL2018: _temp2017=
_temp2015.bounds; goto _LL2000;} else{ goto _LL1855;} _LL2000: _temp1999=
_temp1833.f2; if(( unsigned int) _temp1999 >  3u?*(( int*) _temp1999) ==  Cyc_Absyn_PointerType:
0){ _LL2002: _temp2001=(( struct Cyc_Absyn_PointerType_struct*) _temp1999)->f1;
_LL2012: _temp2011=( void*) _temp2001.elt_typ; goto _LL2010; _LL2010: _temp2009=(
void*) _temp2001.rgn_typ; goto _LL2008; _LL2008: _temp2007= _temp2001.nullable;
goto _LL2006; _LL2006: _temp2005= _temp2001.tq; goto _LL2004; _LL2004: _temp2003=
_temp2001.bounds; goto _LL1854;} else{ goto _LL1855;} _LL1855: _LL2034:
_temp2033= _temp1833.f1; if(( unsigned int) _temp2033 >  3u?*(( int*) _temp2033)
==  Cyc_Absyn_IntType: 0){ _LL2038: _temp2037=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2033)->f1; goto _LL2036; _LL2036: _temp2035=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2033)->f2; goto _LL2028;} else{ goto _LL1857;} _LL2028: _temp2027=
_temp1833.f2; if(( unsigned int) _temp2027 >  3u?*(( int*) _temp2027) ==  Cyc_Absyn_IntType:
0){ _LL2032: _temp2031=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2027)->f1;
goto _LL2030; _LL2030: _temp2029=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2027)->f2; goto _LL1856;} else{ goto _LL1857;} _LL1857: _LL2042: _temp2041=
_temp1833.f1; if( _temp2041 == ( void*) Cyc_Absyn_FloatType){ goto _LL2040;}
else{ goto _LL1859;} _LL2040: _temp2039= _temp1833.f2; if( _temp2039 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1858;} else{ goto _LL1859;} _LL1859: _LL2048:
_temp2047= _temp1833.f1; if(( unsigned int) _temp2047 >  3u?*(( int*) _temp2047)
==  Cyc_Absyn_DoubleType: 0){ _LL2050: _temp2049=(( struct Cyc_Absyn_DoubleType_struct*)
_temp2047)->f1; goto _LL2044;} else{ goto _LL1861;} _LL2044: _temp2043=
_temp1833.f2; if(( unsigned int) _temp2043 >  3u?*(( int*) _temp2043) ==  Cyc_Absyn_DoubleType:
0){ _LL2046: _temp2045=(( struct Cyc_Absyn_DoubleType_struct*) _temp2043)->f1;
goto _LL1860;} else{ goto _LL1861;} _LL1861: _LL2060: _temp2059= _temp1833.f1;
if(( unsigned int) _temp2059 >  3u?*(( int*) _temp2059) ==  Cyc_Absyn_ArrayType:
0){ _LL2066: _temp2065=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2059)->f1;
goto _LL2064; _LL2064: _temp2063=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2059)->f2; goto _LL2062; _LL2062: _temp2061=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2059)->f3; goto _LL2052;} else{ goto _LL1863;} _LL2052: _temp2051=
_temp1833.f2; if(( unsigned int) _temp2051 >  3u?*(( int*) _temp2051) ==  Cyc_Absyn_ArrayType:
0){ _LL2058: _temp2057=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2051)->f1;
goto _LL2056; _LL2056: _temp2055=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2051)->f2; goto _LL2054; _LL2054: _temp2053=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2051)->f3; goto _LL1862;} else{ goto _LL1863;} _LL1863: _LL2088: _temp2087=
_temp1833.f1; if(( unsigned int) _temp2087 >  3u?*(( int*) _temp2087) ==  Cyc_Absyn_FnType:
0){ _LL2090: _temp2089=(( struct Cyc_Absyn_FnType_struct*) _temp2087)->f1;
_LL2106: _temp2105= _temp2089.tvars; goto _LL2104; _LL2104: _temp2103= _temp2089.effect;
goto _LL2102; _LL2102: _temp2101=( void*) _temp2089.ret_typ; goto _LL2100;
_LL2100: _temp2099= _temp2089.args; goto _LL2098; _LL2098: _temp2097= _temp2089.c_varargs;
goto _LL2096; _LL2096: _temp2095= _temp2089.cyc_varargs; goto _LL2094; _LL2094:
_temp2093= _temp2089.rgn_po; goto _LL2092; _LL2092: _temp2091= _temp2089.attributes;
goto _LL2068;} else{ goto _LL1865;} _LL2068: _temp2067= _temp1833.f2; if((
unsigned int) _temp2067 >  3u?*(( int*) _temp2067) ==  Cyc_Absyn_FnType: 0){
_LL2070: _temp2069=(( struct Cyc_Absyn_FnType_struct*) _temp2067)->f1; _LL2086:
_temp2085= _temp2069.tvars; goto _LL2084; _LL2084: _temp2083= _temp2069.effect;
goto _LL2082; _LL2082: _temp2081=( void*) _temp2069.ret_typ; goto _LL2080;
_LL2080: _temp2079= _temp2069.args; goto _LL2078; _LL2078: _temp2077= _temp2069.c_varargs;
goto _LL2076; _LL2076: _temp2075= _temp2069.cyc_varargs; goto _LL2074; _LL2074:
_temp2073= _temp2069.rgn_po; goto _LL2072; _LL2072: _temp2071= _temp2069.attributes;
goto _LL1864;} else{ goto _LL1865;} _LL1865: _LL2112: _temp2111= _temp1833.f1;
if(( unsigned int) _temp2111 >  3u?*(( int*) _temp2111) ==  Cyc_Absyn_TupleType:
0){ _LL2114: _temp2113=(( struct Cyc_Absyn_TupleType_struct*) _temp2111)->f1;
goto _LL2108;} else{ goto _LL1867;} _LL2108: _temp2107= _temp1833.f2; if((
unsigned int) _temp2107 >  3u?*(( int*) _temp2107) ==  Cyc_Absyn_TupleType: 0){
_LL2110: _temp2109=(( struct Cyc_Absyn_TupleType_struct*) _temp2107)->f1; goto
_LL1866;} else{ goto _LL1867;} _LL1867: _LL2120: _temp2119= _temp1833.f1; if((
unsigned int) _temp2119 >  3u?*(( int*) _temp2119) ==  Cyc_Absyn_AnonStructType:
0){ _LL2122: _temp2121=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2119)->f1;
goto _LL2116;} else{ goto _LL1869;} _LL2116: _temp2115= _temp1833.f2; if((
unsigned int) _temp2115 >  3u?*(( int*) _temp2115) ==  Cyc_Absyn_AnonStructType:
0){ _LL2118: _temp2117=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2115)->f1;
goto _LL1868;} else{ goto _LL1869;} _LL1869: _LL2128: _temp2127= _temp1833.f1;
if(( unsigned int) _temp2127 >  3u?*(( int*) _temp2127) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2130: _temp2129=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2127)->f1;
goto _LL2124;} else{ goto _LL1871;} _LL2124: _temp2123= _temp1833.f2; if((
unsigned int) _temp2123 >  3u?*(( int*) _temp2123) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2126: _temp2125=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2123)->f1;
goto _LL1870;} else{ goto _LL1871;} _LL1871: _LL2134: _temp2133= _temp1833.f1;
if( _temp2133 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL2132;} else{ goto _LL1873;}
_LL2132: _temp2131= _temp1833.f2; if( _temp2131 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1872;} else{ goto _LL1873;} _LL1873: _LL2140: _temp2139= _temp1833.f1;
if(( unsigned int) _temp2139 >  3u?*(( int*) _temp2139) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2142: _temp2141=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2139)->f1; goto _LL2136;} else{ goto _LL1875;} _LL2136: _temp2135=
_temp1833.f2; if(( unsigned int) _temp2135 >  3u?*(( int*) _temp2135) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2138: _temp2137=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2135)->f1; goto _LL1874;} else{ goto _LL1875;} _LL1875: _LL2144: _temp2143=
_temp1833.f1; if(( unsigned int) _temp2143 >  3u?*(( int*) _temp2143) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1876;} else{ goto _LL1877;} _LL1877: _LL2146: _temp2145= _temp1833.f2;
if(( unsigned int) _temp2145 >  3u?*(( int*) _temp2145) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1878;} else{ goto _LL1879;} _LL1879: _LL2148: _temp2147= _temp1833.f1;
if(( unsigned int) _temp2147 >  3u?*(( int*) _temp2147) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1880;} else{ goto _LL1881;} _LL1881: _LL2150: _temp2149= _temp1833.f1;
if(( unsigned int) _temp2149 >  3u?*(( int*) _temp2149) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1882;} else{ goto _LL1883;} _LL1883: _LL2152: _temp2151= _temp1833.f2;
if(( unsigned int) _temp2151 >  3u?*(( int*) _temp2151) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1884;} else{ goto _LL1885;} _LL1885: _LL2154: _temp2153= _temp1833.f2;
if(( unsigned int) _temp2153 >  3u?*(( int*) _temp2153) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1886;} else{ goto _LL1887;} _LL1887: goto _LL1888; _LL1836:({ void*
_temp2155[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types", sizeof( unsigned char), 39u),
_tag_arr( _temp2155, sizeof( void*), 0u));}); return 0; _LL1838: return 0;
_LL1840: { struct _tagged_arr* _temp2156= _temp1899->name; struct _tagged_arr*
_temp2157= _temp1903->name; int _temp2158=*(( int*) _check_null( _temp1899->identity));
int _temp2159=*(( int*) _check_null( _temp1903->identity)); return Cyc_Core_intcmp(
_temp2158, _temp2159);} _LL1842: { int _temp2160=(( int(*)( int(* cmp)( struct
_tuple1*, struct _tuple1*), struct _tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)(
Cyc_Absyn_qvar_cmp, _temp1909, _temp1915); if( _temp2160 !=  0){ return
_temp2160;} else{ return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp1907,
_temp1913);}} _LL1844: { int _temp2161= Cyc_Absyn_qvar_cmp( _temp1923, _temp1919);
return _temp2161;} _LL1846: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l1, struct Cyc_List_List* l2))
Cyc_Tcutil_list_cmp)( Cyc_Tcutil_enumfield_cmp, _temp1931, _temp1927); _LL1848: {
int _temp2162=(( int(*)( int(* cmp)( struct _tuple1*, struct _tuple1*), struct
_tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)( Cyc_Absyn_qvar_cmp,
_temp1939, _temp1947); if( _temp2162 !=  0){ return _temp2162;} else{ return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1937, _temp1945);}} _LL1850: if( _temp1961 == 
_temp1974){ return 0;}{ int _temp2163= Cyc_Absyn_qvar_cmp( _temp1961->name,
_temp1974->name); if( _temp2163 !=  0){ return _temp2163;}{ int _temp2164= Cyc_Tcutil_typecmp(
_temp1953, _temp1966); if( _temp2164 !=  0){ return _temp2164;} return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1955, _temp1968);}} _LL1852: if( _temp1985 == 
_temp1997){ return 0;}{ int _temp2165= Cyc_Absyn_qvar_cmp( _temp1997->name,
_temp1985->name); if( _temp2165 !=  0){ return _temp2165;}{ int _temp2166= Cyc_Absyn_qvar_cmp(
_temp1995->name, _temp1983->name); if( _temp2166 !=  0){ return _temp2166;}
return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp1979, _temp1991);}} _LL1854: {
int _temp2167= Cyc_Tcutil_typecmp( _temp2011, _temp2025); if( _temp2167 !=  0){
return _temp2167;}{ int _temp2168= Cyc_Tcutil_typecmp( _temp2009, _temp2023);
if( _temp2168 !=  0){ return _temp2168;}{ int _temp2169= Cyc_Tcutil_tqual_cmp(
_temp2005, _temp2019); if( _temp2169 !=  0){ return _temp2169;}{ int _temp2170=
Cyc_Tcutil_conrefs_cmp( Cyc_Tcutil_boundscmp, _temp2003, _temp2017); if(
_temp2170 !=  0){ return _temp2170;}{ void* _temp2171=( void*)( Cyc_Absyn_compress_conref(
_temp2003))->v; void* _temp2177; _LL2173: if(( unsigned int) _temp2171 >  1u?*((
int*) _temp2171) ==  Cyc_Absyn_Eq_constr: 0){ _LL2178: _temp2177=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2171)->f1; if( _temp2177 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2174;} else{ goto _LL2175;}} else{ goto _LL2175;}
_LL2175: goto _LL2176; _LL2174: return 0; _LL2176: goto _LL2172; _LL2172:;}
return(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_conrefs_cmp)( Cyc_Core_intcmp, _temp2007, _temp2021);}}}} _LL1856:
if( _temp2031 !=  _temp2037){ return Cyc_Core_intcmp(( int)(( unsigned int)
_temp2031),( int)(( unsigned int) _temp2037));} if( _temp2029 !=  _temp2035){
return Cyc_Core_intcmp(( int)(( unsigned int) _temp2029),( int)(( unsigned int)
_temp2035));} return 0; _LL1858: return 0; _LL1860: if( _temp2049 ==  _temp2045){
return 0;} else{ if( _temp2049){ return - 1;} else{ return 1;}} _LL1862: { int
_temp2179= Cyc_Tcutil_tqual_cmp( _temp2055, _temp2063); if( _temp2179 !=  0){
return _temp2179;}{ int _temp2180= Cyc_Tcutil_typecmp( _temp2057, _temp2065);
if( _temp2180 !=  0){ return _temp2180;} if( _temp2061 ==  _temp2053){ return 0;}
if( _temp2061 ==  0? 1: _temp2053 ==  0){({ void* _temp2181[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("missing expression in array index",
sizeof( unsigned char), 34u), _tag_arr( _temp2181, sizeof( void*), 0u));});}
return(( int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp* a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
_temp2061, _temp2053);}} _LL1864:({ void* _temp2182[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: function types not handled",
sizeof( unsigned char), 36u), _tag_arr( _temp2182, sizeof( void*), 0u));}); goto
_LL1834; _LL1866: return(( int(*)( int(* cmp)( struct _tuple4*, struct _tuple4*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_tqual_type_cmp,
_temp2109, _temp2113); _LL1868: _temp2129= _temp2121; _temp2125= _temp2117; goto
_LL1870; _LL1870: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Structfield*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_structfield_cmp, _temp2125, _temp2129);
_LL1872: return 0; _LL1874: return Cyc_Tcutil_typecmp( _temp2141, _temp2137);
_LL1876: goto _LL1878; _LL1878: goto _LL1880; _LL1880: goto _LL1882; _LL1882:
goto _LL1884; _LL1884: goto _LL1886; _LL1886:({ void* _temp2183[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: effects not handled",
sizeof( unsigned char), 29u), _tag_arr( _temp2183, sizeof( void*), 0u));}); goto
_LL1834; _LL1888: goto _LL1834; _LL1834:;} return({ void* _temp2184[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("Unmatched case in typecmp", sizeof( unsigned char), 26u), _tag_arr(
_temp2184, sizeof( void*), 0u));});}} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp2185= Cyc_Tcutil_compress( t); _LL2187: if(( unsigned int) _temp2185
>  3u?*(( int*) _temp2185) ==  Cyc_Absyn_IntType: 0){ goto _LL2188;} else{ goto
_LL2189;} _LL2189: if( _temp2185 == ( void*) Cyc_Absyn_FloatType){ goto _LL2190;}
else{ goto _LL2191;} _LL2191: if(( unsigned int) _temp2185 >  3u?*(( int*)
_temp2185) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2192;} else{ goto _LL2193;}
_LL2193: if(( unsigned int) _temp2185 >  3u?*(( int*) _temp2185) ==  Cyc_Absyn_EnumType:
0){ goto _LL2194;} else{ goto _LL2195;} _LL2195: if(( unsigned int) _temp2185 > 
3u?*(( int*) _temp2185) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL2196;} else{
goto _LL2197;} _LL2197: goto _LL2198; _LL2188: goto _LL2190; _LL2190: goto
_LL2192; _LL2192: goto _LL2194; _LL2194: return 1; _LL2196: return 1; _LL2198:
return 0; _LL2186:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp2200=({ struct _tuple8 _temp2199; _temp2199.f1= t1; _temp2199.f2= t2;
_temp2199;}); void* _temp2224; int _temp2226; void* _temp2228; int _temp2230;
void* _temp2232; void* _temp2234; void* _temp2236; void* _temp2238; void*
_temp2240; void* _temp2242; void* _temp2244; void* _temp2246; void* _temp2248;
void* _temp2250; void* _temp2252; void* _temp2254; void* _temp2256; void*
_temp2258; void* _temp2260; void* _temp2262; void* _temp2264; void* _temp2266;
void* _temp2268; void* _temp2270; void* _temp2272; void* _temp2274; void*
_temp2276; void* _temp2278; void* _temp2280; void* _temp2282; void* _temp2284;
_LL2202: _LL2229: _temp2228= _temp2200.f1; if(( unsigned int) _temp2228 >  3u?*((
int*) _temp2228) ==  Cyc_Absyn_DoubleType: 0){ _LL2231: _temp2230=(( struct Cyc_Absyn_DoubleType_struct*)
_temp2228)->f1; goto _LL2225;} else{ goto _LL2204;} _LL2225: _temp2224=
_temp2200.f2; if(( unsigned int) _temp2224 >  3u?*(( int*) _temp2224) ==  Cyc_Absyn_DoubleType:
0){ _LL2227: _temp2226=(( struct Cyc_Absyn_DoubleType_struct*) _temp2224)->f1;
goto _LL2203;} else{ goto _LL2204;} _LL2204: _LL2235: _temp2234= _temp2200.f1;
if(( unsigned int) _temp2234 >  3u?*(( int*) _temp2234) ==  Cyc_Absyn_DoubleType:
0){ goto _LL2233;} else{ goto _LL2206;} _LL2233: _temp2232= _temp2200.f2; if(
_temp2232 == ( void*) Cyc_Absyn_FloatType){ goto _LL2205;} else{ goto _LL2206;}
_LL2206: _LL2239: _temp2238= _temp2200.f1; if(( unsigned int) _temp2238 >  3u?*((
int*) _temp2238) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2237;} else{ goto _LL2208;}
_LL2237: _temp2236= _temp2200.f2; if(( unsigned int) _temp2236 >  3u?*(( int*)
_temp2236) ==  Cyc_Absyn_IntType: 0){ goto _LL2207;} else{ goto _LL2208;}
_LL2208: _LL2243: _temp2242= _temp2200.f1; if( _temp2242 == ( void*) Cyc_Absyn_FloatType){
goto _LL2241;} else{ goto _LL2210;} _LL2241: _temp2240= _temp2200.f2; if((
unsigned int) _temp2240 >  3u?*(( int*) _temp2240) ==  Cyc_Absyn_IntType: 0){
goto _LL2209;} else{ goto _LL2210;} _LL2210: _LL2249: _temp2248= _temp2200.f1;
if(( unsigned int) _temp2248 >  3u?*(( int*) _temp2248) ==  Cyc_Absyn_IntType: 0){
_LL2251: _temp2250=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2248)->f2;
if( _temp2250 == ( void*) Cyc_Absyn_B8){ goto _LL2245;} else{ goto _LL2212;}}
else{ goto _LL2212;} _LL2245: _temp2244= _temp2200.f2; if(( unsigned int)
_temp2244 >  3u?*(( int*) _temp2244) ==  Cyc_Absyn_IntType: 0){ _LL2247:
_temp2246=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2244)->f2; if(
_temp2246 == ( void*) Cyc_Absyn_B8){ goto _LL2211;} else{ goto _LL2212;}} else{
goto _LL2212;} _LL2212: _LL2253: _temp2252= _temp2200.f1; if(( unsigned int)
_temp2252 >  3u?*(( int*) _temp2252) ==  Cyc_Absyn_IntType: 0){ _LL2255:
_temp2254=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2252)->f2; if(
_temp2254 == ( void*) Cyc_Absyn_B8){ goto _LL2213;} else{ goto _LL2214;}} else{
goto _LL2214;} _LL2214: _LL2259: _temp2258= _temp2200.f1; if(( unsigned int)
_temp2258 >  3u?*(( int*) _temp2258) ==  Cyc_Absyn_IntType: 0){ _LL2261:
_temp2260=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2258)->f2; if(
_temp2260 == ( void*) Cyc_Absyn_B4){ goto _LL2257;} else{ goto _LL2216;}} else{
goto _LL2216;} _LL2257: _temp2256= _temp2200.f2; if( _temp2256 == ( void*) Cyc_Absyn_FloatType){
goto _LL2215;} else{ goto _LL2216;} _LL2216: _LL2267: _temp2266= _temp2200.f1;
if(( unsigned int) _temp2266 >  3u?*(( int*) _temp2266) ==  Cyc_Absyn_IntType: 0){
_LL2269: _temp2268=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2266)->f2;
if( _temp2268 == ( void*) Cyc_Absyn_B4){ goto _LL2263;} else{ goto _LL2218;}}
else{ goto _LL2218;} _LL2263: _temp2262= _temp2200.f2; if(( unsigned int)
_temp2262 >  3u?*(( int*) _temp2262) ==  Cyc_Absyn_IntType: 0){ _LL2265:
_temp2264=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2262)->f2; if(
_temp2264 == ( void*) Cyc_Absyn_B2){ goto _LL2217;} else{ goto _LL2218;}} else{
goto _LL2218;} _LL2218: _LL2275: _temp2274= _temp2200.f1; if(( unsigned int)
_temp2274 >  3u?*(( int*) _temp2274) ==  Cyc_Absyn_IntType: 0){ _LL2277:
_temp2276=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2274)->f2; if(
_temp2276 == ( void*) Cyc_Absyn_B4){ goto _LL2271;} else{ goto _LL2220;}} else{
goto _LL2220;} _LL2271: _temp2270= _temp2200.f2; if(( unsigned int) _temp2270 > 
3u?*(( int*) _temp2270) ==  Cyc_Absyn_IntType: 0){ _LL2273: _temp2272=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2270)->f2; if( _temp2272 == ( void*) Cyc_Absyn_B1){
goto _LL2219;} else{ goto _LL2220;}} else{ goto _LL2220;} _LL2220: _LL2283:
_temp2282= _temp2200.f1; if(( unsigned int) _temp2282 >  3u?*(( int*) _temp2282)
==  Cyc_Absyn_IntType: 0){ _LL2285: _temp2284=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2282)->f2; if( _temp2284 == ( void*) Cyc_Absyn_B2){ goto _LL2279;} else{
goto _LL2222;}} else{ goto _LL2222;} _LL2279: _temp2278= _temp2200.f2; if((
unsigned int) _temp2278 >  3u?*(( int*) _temp2278) ==  Cyc_Absyn_IntType: 0){
_LL2281: _temp2280=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2278)->f2;
if( _temp2280 == ( void*) Cyc_Absyn_B1){ goto _LL2221;} else{ goto _LL2222;}}
else{ goto _LL2222;} _LL2222: goto _LL2223; _LL2203: return ! _temp2226?
_temp2230: 0; _LL2205: goto _LL2207; _LL2207: goto _LL2209; _LL2209: return 1;
_LL2211: return 0; _LL2213: goto _LL2215; _LL2215: goto _LL2217; _LL2217: goto
_LL2219; _LL2219: goto _LL2221; _LL2221: return 1; _LL2223: return 0; _LL2201:;}}
int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_List_List*
es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List* el= es; for(
0; el !=  0; el= el->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) el->hd)->topt))->v); if( Cyc_Tcutil_is_arithmetic_type(
t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)
max_arith_type->v)){ max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2286=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2286->v=( void*) t1; _temp2286;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify(
t,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*) el->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp2289; _temp2289.tag= Cyc_Std_String_pa; _temp2289.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*) el->hd)->topt))->v);{ struct Cyc_Std_String_pa_struct _temp2288;
_temp2288.tag= Cyc_Std_String_pa; _temp2288.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2287[ 2u]={& _temp2288,& _temp2289}; Cyc_Tcutil_terr((( struct
Cyc_Absyn_Exp*) el->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp2287, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp2290= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL2292: if(( unsigned int) _temp2290 >  3u?*(( int*) _temp2290) ==  Cyc_Absyn_PointerType:
0){ goto _LL2293;} else{ goto _LL2294;} _LL2294: goto _LL2295; _LL2293: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL2291; _LL2295: return 0; _LL2291:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp2296= Cyc_Tcutil_compress(
t); _LL2298: if(( unsigned int) _temp2296 >  3u?*(( int*) _temp2296) ==  Cyc_Absyn_IntType:
0){ goto _LL2299;} else{ goto _LL2300;} _LL2300: if(( unsigned int) _temp2296 > 
3u?*(( int*) _temp2296) ==  Cyc_Absyn_EnumType: 0){ goto _LL2301;} else{ goto
_LL2302;} _LL2302: if(( unsigned int) _temp2296 >  3u?*(( int*) _temp2296) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2303;} else{ goto _LL2304;} _LL2304: goto
_LL2305; _LL2299: goto _LL2301; _LL2301: return 1; _LL2303: return 1; _LL2305:
return 0; _LL2297:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp2306[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2306, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp2307[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp2307, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp2309=({ struct _tuple8 _temp2308; _temp2308.f1= t1; _temp2308.f2= t2;
_temp2308;}); void* _temp2321; struct Cyc_Absyn_PtrInfo _temp2323; void*
_temp2325; struct Cyc_Absyn_PtrInfo _temp2327; void* _temp2329; struct Cyc_Absyn_Exp*
_temp2331; struct Cyc_Absyn_Tqual _temp2333; void* _temp2335; void* _temp2337;
struct Cyc_Absyn_Exp* _temp2339; struct Cyc_Absyn_Tqual _temp2341; void*
_temp2343; void* _temp2345; struct Cyc_Absyn_TunionInfo _temp2347; struct Cyc_List_List*
_temp2349; void* _temp2351; struct Cyc_Absyn_Tuniondecl** _temp2353; struct Cyc_Absyn_Tuniondecl*
_temp2355; void* _temp2356; struct Cyc_Absyn_TunionFieldInfo _temp2358; struct
Cyc_List_List* _temp2360; void* _temp2362; struct Cyc_Absyn_Tunionfield*
_temp2364; struct Cyc_Absyn_Tuniondecl* _temp2366; void* _temp2368; struct Cyc_Absyn_TunionInfo
_temp2370; void* _temp2372; struct Cyc_List_List* _temp2374; void* _temp2376;
struct Cyc_Absyn_Tuniondecl** _temp2378; struct Cyc_Absyn_Tuniondecl* _temp2380;
void* _temp2381; struct Cyc_Absyn_PtrInfo _temp2383; struct Cyc_Absyn_Conref*
_temp2385; struct Cyc_Absyn_Tqual _temp2387; struct Cyc_Absyn_Conref* _temp2389;
void* _temp2391; void* _temp2393; _LL2311: _LL2326: _temp2325= _temp2309.f1; if((
unsigned int) _temp2325 >  3u?*(( int*) _temp2325) ==  Cyc_Absyn_PointerType: 0){
_LL2328: _temp2327=(( struct Cyc_Absyn_PointerType_struct*) _temp2325)->f1; goto
_LL2322;} else{ goto _LL2313;} _LL2322: _temp2321= _temp2309.f2; if((
unsigned int) _temp2321 >  3u?*(( int*) _temp2321) ==  Cyc_Absyn_PointerType: 0){
_LL2324: _temp2323=(( struct Cyc_Absyn_PointerType_struct*) _temp2321)->f1; goto
_LL2312;} else{ goto _LL2313;} _LL2313: _LL2338: _temp2337= _temp2309.f1; if((
unsigned int) _temp2337 >  3u?*(( int*) _temp2337) ==  Cyc_Absyn_ArrayType: 0){
_LL2344: _temp2343=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2337)->f1;
goto _LL2342; _LL2342: _temp2341=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2337)->f2; goto _LL2340; _LL2340: _temp2339=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2337)->f3; goto _LL2330;} else{ goto _LL2315;} _LL2330: _temp2329=
_temp2309.f2; if(( unsigned int) _temp2329 >  3u?*(( int*) _temp2329) ==  Cyc_Absyn_ArrayType:
0){ _LL2336: _temp2335=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2329)->f1;
goto _LL2334; _LL2334: _temp2333=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2329)->f2; goto _LL2332; _LL2332: _temp2331=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2329)->f3; goto _LL2314;} else{ goto _LL2315;} _LL2315: _LL2357: _temp2356=
_temp2309.f1; if(( unsigned int) _temp2356 >  3u?*(( int*) _temp2356) ==  Cyc_Absyn_TunionFieldType:
0){ _LL2359: _temp2358=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2356)->f1;
_LL2363: _temp2362=( void*) _temp2358.field_info; if(*(( int*) _temp2362) == 
Cyc_Absyn_KnownTunionfield){ _LL2367: _temp2366=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2362)->f1; goto _LL2365; _LL2365: _temp2364=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2362)->f2; goto _LL2361;} else{ goto _LL2317;} _LL2361: _temp2360=
_temp2358.targs; goto _LL2346;} else{ goto _LL2317;} _LL2346: _temp2345=
_temp2309.f2; if(( unsigned int) _temp2345 >  3u?*(( int*) _temp2345) ==  Cyc_Absyn_TunionType:
0){ _LL2348: _temp2347=(( struct Cyc_Absyn_TunionType_struct*) _temp2345)->f1;
_LL2352: _temp2351=( void*) _temp2347.tunion_info; if(*(( int*) _temp2351) == 
Cyc_Absyn_KnownTunion){ _LL2354: _temp2353=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2351)->f1; _temp2355=* _temp2353; goto _LL2350;} else{ goto _LL2317;}
_LL2350: _temp2349= _temp2347.targs; goto _LL2316;} else{ goto _LL2317;} _LL2317:
_LL2382: _temp2381= _temp2309.f1; if(( unsigned int) _temp2381 >  3u?*(( int*)
_temp2381) ==  Cyc_Absyn_PointerType: 0){ _LL2384: _temp2383=(( struct Cyc_Absyn_PointerType_struct*)
_temp2381)->f1; _LL2394: _temp2393=( void*) _temp2383.elt_typ; goto _LL2392;
_LL2392: _temp2391=( void*) _temp2383.rgn_typ; goto _LL2390; _LL2390: _temp2389=
_temp2383.nullable; goto _LL2388; _LL2388: _temp2387= _temp2383.tq; goto _LL2386;
_LL2386: _temp2385= _temp2383.bounds; goto _LL2369;} else{ goto _LL2319;}
_LL2369: _temp2368= _temp2309.f2; if(( unsigned int) _temp2368 >  3u?*(( int*)
_temp2368) ==  Cyc_Absyn_TunionType: 0){ _LL2371: _temp2370=(( struct Cyc_Absyn_TunionType_struct*)
_temp2368)->f1; _LL2377: _temp2376=( void*) _temp2370.tunion_info; if(*(( int*)
_temp2376) ==  Cyc_Absyn_KnownTunion){ _LL2379: _temp2378=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2376)->f1; _temp2380=* _temp2378; goto _LL2375;} else{ goto _LL2319;}
_LL2375: _temp2374= _temp2370.targs; goto _LL2373; _LL2373: _temp2372=( void*)
_temp2370.rgn; goto _LL2318;} else{ goto _LL2319;} _LL2319: goto _LL2320;
_LL2312: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp2327.nullable, _temp2323.nullable)){ void* _temp2395=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2327.nullable))->v;
int _temp2401; _LL2397: if(( unsigned int) _temp2395 >  1u?*(( int*) _temp2395)
==  Cyc_Absyn_Eq_constr: 0){ _LL2402: _temp2401=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2395)->f1; goto _LL2398;} else{ goto _LL2399;} _LL2399: goto _LL2400;
_LL2398: okay= ! _temp2401; goto _LL2396; _LL2400:( int) _throw(({ void*
_temp2403[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u), _tag_arr( _temp2403, sizeof( void*), 0u));}));
_LL2396:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2327.bounds,
_temp2323.bounds)){ struct _tuple8 _temp2405=({ struct _tuple8 _temp2404;
_temp2404.f1=( void*)( Cyc_Absyn_compress_conref( _temp2327.bounds))->v;
_temp2404.f2=( void*)( Cyc_Absyn_compress_conref( _temp2323.bounds))->v;
_temp2404;}); void* _temp2415; void* _temp2417; void* _temp2419; void* _temp2421;
void* _temp2423; void* _temp2425; struct Cyc_Absyn_Exp* _temp2427; void*
_temp2429; void* _temp2431; struct Cyc_Absyn_Exp* _temp2433; void* _temp2435;
void* _temp2437; void* _temp2439; void* _temp2441; _LL2407: _LL2420: _temp2419=
_temp2405.f1; if(( unsigned int) _temp2419 >  1u?*(( int*) _temp2419) ==  Cyc_Absyn_Eq_constr:
0){ _LL2422: _temp2421=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2419)->f1;
if(( unsigned int) _temp2421 >  1u?*(( int*) _temp2421) ==  Cyc_Absyn_Upper_b: 0){
goto _LL2416;} else{ goto _LL2409;}} else{ goto _LL2409;} _LL2416: _temp2415=
_temp2405.f2; if(( unsigned int) _temp2415 >  1u?*(( int*) _temp2415) ==  Cyc_Absyn_Eq_constr:
0){ _LL2418: _temp2417=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2415)->f1;
if( _temp2417 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2408;} else{ goto
_LL2409;}} else{ goto _LL2409;} _LL2409: _LL2430: _temp2429= _temp2405.f1; if((
unsigned int) _temp2429 >  1u?*(( int*) _temp2429) ==  Cyc_Absyn_Eq_constr: 0){
_LL2432: _temp2431=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2429)->f1;
if(( unsigned int) _temp2431 >  1u?*(( int*) _temp2431) ==  Cyc_Absyn_Upper_b: 0){
_LL2434: _temp2433=(( struct Cyc_Absyn_Upper_b_struct*) _temp2431)->f1; goto
_LL2424;} else{ goto _LL2411;}} else{ goto _LL2411;} _LL2424: _temp2423=
_temp2405.f2; if(( unsigned int) _temp2423 >  1u?*(( int*) _temp2423) ==  Cyc_Absyn_Eq_constr:
0){ _LL2426: _temp2425=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2423)->f1;
if(( unsigned int) _temp2425 >  1u?*(( int*) _temp2425) ==  Cyc_Absyn_Upper_b: 0){
_LL2428: _temp2427=(( struct Cyc_Absyn_Upper_b_struct*) _temp2425)->f1; goto
_LL2410;} else{ goto _LL2411;}} else{ goto _LL2411;} _LL2411: _LL2440: _temp2439=
_temp2405.f1; if(( unsigned int) _temp2439 >  1u?*(( int*) _temp2439) ==  Cyc_Absyn_Eq_constr:
0){ _LL2442: _temp2441=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2439)->f1;
if( _temp2441 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2436;} else{ goto
_LL2413;}} else{ goto _LL2413;} _LL2436: _temp2435= _temp2405.f2; if((
unsigned int) _temp2435 >  1u?*(( int*) _temp2435) ==  Cyc_Absyn_Eq_constr: 0){
_LL2438: _temp2437=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2435)->f1;
if( _temp2437 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2412;} else{ goto
_LL2413;}} else{ goto _LL2413;} _LL2413: goto _LL2414; _LL2408: okay= 1; goto
_LL2406; _LL2410: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp2433) >=  Cyc_Evexp_eval_const_uint_exp(
_temp2427): 0;({ void* _temp2443[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp2443, sizeof( void*), 0u));}); goto
_LL2406; _LL2412: okay= 1; goto _LL2406; _LL2414: okay= 0; goto _LL2406; _LL2406:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp2327.elt_typ,( void*) _temp2323.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp2327.rgn_typ,( void*) _temp2323.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp2327.rgn_typ,( void*) _temp2323.rgn_typ):
0; okay= okay? !( _temp2327.tq).q_const? 1:( _temp2323.tq).q_const: 0; return
okay;} _LL2314: { int okay; okay=( _temp2339 !=  0? _temp2331 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2339)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2331)): 0; return( okay? Cyc_Tcutil_unify(
_temp2343, _temp2335): 0)? ! _temp2341.q_const? 1: _temp2333.q_const: 0;}
_LL2316: if(( _temp2366 ==  _temp2355? 1: Cyc_Absyn_qvar_cmp( _temp2366->name,
_temp2355->name) ==  0)? _temp2364->typs ==  0: 0){ for( 0; _temp2360 !=  0?
_temp2349 !=  0: 0;( _temp2360= _temp2360->tl, _temp2349= _temp2349->tl)){ if( !
Cyc_Tcutil_unify(( void*) _temp2360->hd,( void*) _temp2349->hd)){ break;}} if(
_temp2360 ==  0? _temp2349 ==  0: 0){ return 1;}} return 0; _LL2318:{ void*
_temp2444= Cyc_Tcutil_compress( _temp2393); struct Cyc_Absyn_TunionFieldInfo
_temp2450; struct Cyc_List_List* _temp2452; void* _temp2454; struct Cyc_Absyn_Tunionfield*
_temp2456; struct Cyc_Absyn_Tuniondecl* _temp2458; _LL2446: if(( unsigned int)
_temp2444 >  3u?*(( int*) _temp2444) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2451:
_temp2450=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2444)->f1; _LL2455:
_temp2454=( void*) _temp2450.field_info; if(*(( int*) _temp2454) ==  Cyc_Absyn_KnownTunionfield){
_LL2459: _temp2458=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2454)->f1;
goto _LL2457; _LL2457: _temp2456=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2454)->f2; goto _LL2453;} else{ goto _LL2448;} _LL2453: _temp2452=
_temp2450.targs; goto _LL2447;} else{ goto _LL2448;} _LL2448: goto _LL2449;
_LL2447: if( ! Cyc_Tcutil_unify( _temp2391, _temp2372)? ! Cyc_Tcenv_region_outlives(
te, _temp2391, _temp2372): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp2389,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2385,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp2380->name, _temp2458->name) ==  0? _temp2456->typs !=  0: 0){ int okay= 1;
for( 0; _temp2452 !=  0? _temp2374 !=  0: 0;( _temp2452= _temp2452->tl,
_temp2374= _temp2374->tl)){ if( ! Cyc_Tcutil_unify(( void*) _temp2452->hd,( void*)
_temp2374->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp2452 !=  0)? 1:
_temp2374 !=  0){ return 0;} return 1;} goto _LL2445; _LL2449: goto _LL2445;
_LL2445:;} return 0; _LL2320: return Cyc_Tcutil_unify( t1, t2); _LL2310:;}} int
Cyc_Tcutil_is_pointer_type( void* t){ void* _temp2460= Cyc_Tcutil_compress( t);
_LL2462: if(( unsigned int) _temp2460 >  3u?*(( int*) _temp2460) ==  Cyc_Absyn_PointerType:
0){ goto _LL2463;} else{ goto _LL2464;} _LL2464: goto _LL2465; _LL2463: return 1;
_LL2465: return 0; _LL2461:;} int Cyc_Tcutil_is_zero( struct Cyc_Absyn_Exp* e){
void* _temp2466=( void*) e->r; void* _temp2472; int _temp2474; _LL2468: if(*((
int*) _temp2466) ==  Cyc_Absyn_Const_e){ _LL2473: _temp2472=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp2466)->f1; if(( unsigned int) _temp2472 >  1u?*((
int*) _temp2472) ==  Cyc_Absyn_Int_c: 0){ _LL2475: _temp2474=(( struct Cyc_Absyn_Int_c_struct*)
_temp2472)->f2; if( _temp2474 ==  0){ goto _LL2469;} else{ goto _LL2470;}} else{
goto _LL2470;}} else{ goto _LL2470;} _LL2470: goto _LL2471; _LL2469: return 1;
_LL2471: return 0; _LL2467:;} struct Cyc_Core_Opt Cyc_Tcutil_rk={( void*)(( void*)
3u)}; struct Cyc_Core_Opt Cyc_Tcutil_ak={( void*)(( void*) 0u)}; struct Cyc_Core_Opt
Cyc_Tcutil_bk={( void*)(( void*) 2u)}; struct Cyc_Core_Opt Cyc_Tcutil_mk={( void*)((
void*) 1u)}; int Cyc_Tcutil_zero_to_null( struct Cyc_Tcenv_Tenv* te, void* t2,
struct Cyc_Absyn_Exp* e1){ if( Cyc_Tcutil_is_pointer_type( t2)? Cyc_Tcutil_is_zero(
e1): 0){( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2476=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2476[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2477; _temp2477.tag= Cyc_Absyn_Const_e;
_temp2477.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2477;}); _temp2476;})));{
struct Cyc_List_List* _temp2478= Cyc_Tcenv_lookup_type_vars( te); struct Cyc_Absyn_PointerType_struct*
_temp2479=({ struct Cyc_Absyn_PointerType_struct* _temp2480=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2480[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2481; _temp2481.tag= Cyc_Absyn_PointerType;
_temp2481.f1=({ struct Cyc_Absyn_PtrInfo _temp2482; _temp2482.elt_typ=( void*)
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt*
_temp2484=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2484->v=( void*) _temp2478; _temp2484;})); _temp2482.rgn_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp2483=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2483->v=( void*)
_temp2478; _temp2483;})); _temp2482.nullable=(( struct Cyc_Absyn_Conref*(*)( int
x)) Cyc_Absyn_new_conref)( 1); _temp2482.tq= Cyc_Absyn_empty_tqual(); _temp2482.bounds=
Cyc_Absyn_empty_conref(); _temp2482;}); _temp2481;}); _temp2480;});( void*)(((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v=( void*)(( void*) _temp2479));
return Cyc_Tcutil_coerce_arg( te, e1, t2);}} return 0;} int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type( t1)? Cyc_Tcutil_is_arithmetic_type(
t2): 0){ if( Cyc_Tcutil_will_lose_precision( t1, t2)){({ void* _temp2485[ 0u]={};
Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2485, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_zero_to_null(
te, t2, e)){ return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2);({ struct Cyc_Std_String_pa_struct _temp2488; _temp2488.tag= Cyc_Std_String_pa;
_temp2488.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp2487; _temp2487.tag= Cyc_Std_String_pa; _temp2487.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp2486[ 2u]={& _temp2487,& _temp2488};
Cyc_Tcutil_warn( e->loc, _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u), _tag_arr( _temp2486, sizeof( void*), 2u));}}}); return 1;}
else{ return 0;}}}}} int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg( te, e, t);} int
Cyc_Tcutil_coerceable( void* t){ void* _temp2489= Cyc_Tcutil_compress( t);
_LL2491: if(( unsigned int) _temp2489 >  3u?*(( int*) _temp2489) ==  Cyc_Absyn_IntType:
0){ goto _LL2492;} else{ goto _LL2493;} _LL2493: if( _temp2489 == ( void*) Cyc_Absyn_FloatType){
goto _LL2494;} else{ goto _LL2495;} _LL2495: if(( unsigned int) _temp2489 >  3u?*((
int*) _temp2489) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2496;} else{ goto _LL2497;}
_LL2497: goto _LL2498; _LL2492: goto _LL2494; _LL2494: goto _LL2496; _LL2496:
return 1; _LL2498: return 0; _LL2490:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp2499=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp2499->f1= x->tq; _temp2499->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp2499;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp2500= t1; struct Cyc_List_List*
_temp2512; struct Cyc_Absyn_Structdecl** _temp2514; struct Cyc_List_List*
_temp2516; struct _tuple1* _temp2518; struct Cyc_List_List* _temp2520; _LL2502:
if( _temp2500 == ( void*) Cyc_Absyn_VoidType){ goto _LL2503;} else{ goto _LL2504;}
_LL2504: if(( unsigned int) _temp2500 >  3u?*(( int*) _temp2500) ==  Cyc_Absyn_TupleType:
0){ _LL2513: _temp2512=(( struct Cyc_Absyn_TupleType_struct*) _temp2500)->f1;
goto _LL2505;} else{ goto _LL2506;} _LL2506: if(( unsigned int) _temp2500 >  3u?*((
int*) _temp2500) ==  Cyc_Absyn_StructType: 0){ _LL2519: _temp2518=(( struct Cyc_Absyn_StructType_struct*)
_temp2500)->f1; goto _LL2517; _LL2517: _temp2516=(( struct Cyc_Absyn_StructType_struct*)
_temp2500)->f2; goto _LL2515; _LL2515: _temp2514=(( struct Cyc_Absyn_StructType_struct*)
_temp2500)->f3; goto _LL2507;} else{ goto _LL2508;} _LL2508: if(( unsigned int)
_temp2500 >  3u?*(( int*) _temp2500) ==  Cyc_Absyn_AnonStructType: 0){ _LL2521:
_temp2520=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2500)->f1; goto
_LL2509;} else{ goto _LL2510;} _LL2510: goto _LL2511; _LL2503: return 0; _LL2505:
return _temp2512; _LL2507: if( _temp2514 ==  0? 1: _temp2518 ==  0){ return({
struct Cyc_List_List* _temp2522=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2522->hd=( void*)({ struct _tuple4* _temp2523=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2523->f1= Cyc_Absyn_empty_tqual();
_temp2523->f2= t1; _temp2523;}); _temp2522->tl= 0; _temp2522;});}{ struct Cyc_Absyn_Structdecl*
_temp2524=* _temp2514; struct _tuple1 _temp2525=* _temp2518; struct Cyc_List_List*
_temp2526=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp2524->tvs, _temp2516); if( _temp2524->fields ==  0){
return({ struct Cyc_List_List* _temp2527=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2527->hd=( void*)({ struct _tuple4*
_temp2528=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2528->f1=
Cyc_Absyn_empty_tqual(); _temp2528->f2= t1; _temp2528;}); _temp2527->tl= 0;
_temp2527;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp2526,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2524->fields))->v);}
_LL2509: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp2520); _LL2511: return({
struct Cyc_List_List* _temp2529=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2529->hd=( void*)({ struct _tuple4* _temp2530=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2530->f1= Cyc_Absyn_empty_tqual();
_temp2530->f2= t1; _temp2530;}); _temp2529->tl= 0; _temp2529;}); _LL2501:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a= a->tl){
if( Cyc_Tcutil_unify( t1,(*(( struct _tuple8*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*((
struct _tuple8*) a->hd)).f2): 0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp2532=({ struct _tuple8 _temp2531;
_temp2531.f1= t1; _temp2531.f2= t2; _temp2531;}); void* _temp2538; struct Cyc_Absyn_PtrInfo
_temp2540; struct Cyc_Absyn_Conref* _temp2542; struct Cyc_Absyn_Tqual _temp2544;
struct Cyc_Absyn_Conref* _temp2546; void* _temp2548; void* _temp2550; void*
_temp2552; struct Cyc_Absyn_PtrInfo _temp2554; struct Cyc_Absyn_Conref*
_temp2556; struct Cyc_Absyn_Tqual _temp2558; struct Cyc_Absyn_Conref* _temp2560;
void* _temp2562; void* _temp2564; _LL2534: _LL2553: _temp2552= _temp2532.f1; if((
unsigned int) _temp2552 >  3u?*(( int*) _temp2552) ==  Cyc_Absyn_PointerType: 0){
_LL2555: _temp2554=(( struct Cyc_Absyn_PointerType_struct*) _temp2552)->f1;
_LL2565: _temp2564=( void*) _temp2554.elt_typ; goto _LL2563; _LL2563: _temp2562=(
void*) _temp2554.rgn_typ; goto _LL2561; _LL2561: _temp2560= _temp2554.nullable;
goto _LL2559; _LL2559: _temp2558= _temp2554.tq; goto _LL2557; _LL2557: _temp2556=
_temp2554.bounds; goto _LL2539;} else{ goto _LL2536;} _LL2539: _temp2538=
_temp2532.f2; if(( unsigned int) _temp2538 >  3u?*(( int*) _temp2538) ==  Cyc_Absyn_PointerType:
0){ _LL2541: _temp2540=(( struct Cyc_Absyn_PointerType_struct*) _temp2538)->f1;
_LL2551: _temp2550=( void*) _temp2540.elt_typ; goto _LL2549; _LL2549: _temp2548=(
void*) _temp2540.rgn_typ; goto _LL2547; _LL2547: _temp2546= _temp2540.nullable;
goto _LL2545; _LL2545: _temp2544= _temp2540.tq; goto _LL2543; _LL2543: _temp2542=
_temp2540.bounds; goto _LL2535;} else{ goto _LL2536;} _LL2536: goto _LL2537;
_LL2535: if( _temp2558.q_const? ! _temp2544.q_const: 0){ return 0;} if(( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp2560, _temp2546)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2560): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2546): 0){ return 0;}
if( ! Cyc_Tcutil_unify( _temp2562, _temp2548)? ! Cyc_Tcenv_region_outlives( te,
_temp2562, _temp2548): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2556, _temp2542)){ struct _tuple8 _temp2567=({ struct _tuple8 _temp2566;
_temp2566.f1= Cyc_Absyn_conref_val( _temp2556); _temp2566.f2= Cyc_Absyn_conref_val(
_temp2542); _temp2566;}); void* _temp2575; void* _temp2577; void* _temp2579;
struct Cyc_Absyn_Exp* _temp2581; void* _temp2583; struct Cyc_Absyn_Exp*
_temp2585; _LL2569: _LL2578: _temp2577= _temp2567.f1; if(( unsigned int)
_temp2577 >  1u?*(( int*) _temp2577) ==  Cyc_Absyn_Upper_b: 0){ goto _LL2576;}
else{ goto _LL2571;} _LL2576: _temp2575= _temp2567.f2; if( _temp2575 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2570;} else{ goto _LL2571;} _LL2571: _LL2584:
_temp2583= _temp2567.f1; if(( unsigned int) _temp2583 >  1u?*(( int*) _temp2583)
==  Cyc_Absyn_Upper_b: 0){ _LL2586: _temp2585=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2583)->f1; goto _LL2580;} else{ goto _LL2573;} _LL2580: _temp2579=
_temp2567.f2; if(( unsigned int) _temp2579 >  1u?*(( int*) _temp2579) ==  Cyc_Absyn_Upper_b:
0){ _LL2582: _temp2581=(( struct Cyc_Absyn_Upper_b_struct*) _temp2579)->f1; goto
_LL2572;} else{ goto _LL2573;} _LL2573: goto _LL2574; _LL2570: goto _LL2568;
_LL2572: if( Cyc_Evexp_eval_const_uint_exp( _temp2585) <  Cyc_Evexp_eval_const_uint_exp(
_temp2581)){ return 0;} goto _LL2568; _LL2574: return 0; _LL2568:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp2587=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2587->hd=( void*)({ struct _tuple8*
_temp2588=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2588->f1=
t1; _temp2588->f2= t2; _temp2588;}); _temp2587->tl= assume; _temp2587;}),
_temp2564, _temp2550); _LL2537: return 0; _LL2533:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp2590=({ struct _tuple8 _temp2589;
_temp2589.f1= Cyc_Tcutil_compress( t1); _temp2589.f2= Cyc_Tcutil_compress( t2);
_temp2589;}); void* _temp2596; void* _temp2598; void* _temp2600; void* _temp2602;
_LL2592: _LL2601: _temp2600= _temp2590.f1; if(( unsigned int) _temp2600 >  3u?*((
int*) _temp2600) ==  Cyc_Absyn_IntType: 0){ _LL2603: _temp2602=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2600)->f2; goto _LL2597;} else{ goto _LL2594;}
_LL2597: _temp2596= _temp2590.f2; if(( unsigned int) _temp2596 >  3u?*(( int*)
_temp2596) ==  Cyc_Absyn_IntType: 0){ _LL2599: _temp2598=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2596)->f2; goto _LL2593;} else{ goto _LL2594;} _LL2594: goto _LL2595;
_LL2593: return _temp2602 ==  _temp2598; _LL2595: return 0; _LL2591:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2= tqs2->tl, tqs1= tqs1->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp2606; void* _temp2607; struct Cyc_Absyn_Tqual _temp2609;
struct _tuple4* _temp2604=( struct _tuple4*) tqs1->hd; _temp2606=* _temp2604;
_LL2610: _temp2609= _temp2606.f1; goto _LL2608; _LL2608: _temp2607= _temp2606.f2;
goto _LL2605; _LL2605: { struct _tuple4 _temp2613; void* _temp2614; struct Cyc_Absyn_Tqual
_temp2616; struct _tuple4* _temp2611=( struct _tuple4*) tqs2->hd; _temp2613=*
_temp2611; _LL2617: _temp2616= _temp2613.f1; goto _LL2615; _LL2615: _temp2614=
_temp2613.f2; goto _LL2612; _LL2612: if( _temp2616.q_const? Cyc_Tcutil_subtype(
te, assume, _temp2607, _temp2614): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp2607, _temp2614)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp2607,
_temp2614)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp2618= t2; struct Cyc_Absyn_Uniondecl** _temp2626; struct Cyc_List_List*
_temp2628; void* _temp2630; _LL2620: if(( unsigned int) _temp2618 >  3u?*(( int*)
_temp2618) ==  Cyc_Absyn_UnionType: 0){ _LL2629: _temp2628=(( struct Cyc_Absyn_UnionType_struct*)
_temp2618)->f2; goto _LL2627; _LL2627: _temp2626=(( struct Cyc_Absyn_UnionType_struct*)
_temp2618)->f3; goto _LL2621;} else{ goto _LL2622;} _LL2622: if(( unsigned int)
_temp2618 >  3u?*(( int*) _temp2618) ==  Cyc_Absyn_IntType: 0){ _LL2631:
_temp2630=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2618)->f2; if(
_temp2630 == ( void*) Cyc_Absyn_B4){ goto _LL2623;} else{ goto _LL2624;}} else{
goto _LL2624;} _LL2624: goto _LL2625; _LL2621: if( _temp2626 ==  0){ goto
_LL2619;}{ struct Cyc_Absyn_Uniondecl* _temp2632=* _temp2626; if( _temp2632->fields
==  0){ goto _LL2619;}{ struct Cyc_List_List* _temp2633=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2632->tvs,
_temp2628);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2632->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp2633,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ return 1;}}} goto _LL2619;}} _LL2623: if( Cyc_Tcutil_typ_kind(
t2) == ( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL2619; _LL2625: goto
_LL2619; _LL2619:;}{ void* _temp2634= t1; struct Cyc_Absyn_PtrInfo _temp2652;
struct Cyc_Absyn_Conref* _temp2654; struct Cyc_Absyn_Tqual _temp2656; struct Cyc_Absyn_Conref*
_temp2658; void* _temp2660; void* _temp2662; struct Cyc_Absyn_Exp* _temp2664;
struct Cyc_Absyn_Tqual _temp2666; void* _temp2668; struct Cyc_Absyn_Enumdecl*
_temp2670; _LL2636: if(( unsigned int) _temp2634 >  3u?*(( int*) _temp2634) == 
Cyc_Absyn_PointerType: 0){ _LL2653: _temp2652=(( struct Cyc_Absyn_PointerType_struct*)
_temp2634)->f1; _LL2663: _temp2662=( void*) _temp2652.elt_typ; goto _LL2661;
_LL2661: _temp2660=( void*) _temp2652.rgn_typ; goto _LL2659; _LL2659: _temp2658=
_temp2652.nullable; goto _LL2657; _LL2657: _temp2656= _temp2652.tq; goto _LL2655;
_LL2655: _temp2654= _temp2652.bounds; goto _LL2637;} else{ goto _LL2638;}
_LL2638: if(( unsigned int) _temp2634 >  3u?*(( int*) _temp2634) ==  Cyc_Absyn_ArrayType:
0){ _LL2669: _temp2668=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2634)->f1;
goto _LL2667; _LL2667: _temp2666=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2634)->f2; goto _LL2665; _LL2665: _temp2664=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2634)->f3; goto _LL2639;} else{ goto _LL2640;} _LL2640: if(( unsigned int)
_temp2634 >  3u?*(( int*) _temp2634) ==  Cyc_Absyn_EnumType: 0){ _LL2671:
_temp2670=(( struct Cyc_Absyn_EnumType_struct*) _temp2634)->f2; goto _LL2641;}
else{ goto _LL2642;} _LL2642: if(( unsigned int) _temp2634 >  3u?*(( int*)
_temp2634) ==  Cyc_Absyn_IntType: 0){ goto _LL2643;} else{ goto _LL2644;}
_LL2644: if( _temp2634 == ( void*) Cyc_Absyn_FloatType){ goto _LL2645;} else{
goto _LL2646;} _LL2646: if(( unsigned int) _temp2634 >  3u?*(( int*) _temp2634)
==  Cyc_Absyn_DoubleType: 0){ goto _LL2647;} else{ goto _LL2648;} _LL2648: if(
_temp2634 == ( void*) Cyc_Absyn_VoidType){ goto _LL2649;} else{ goto _LL2650;}
_LL2650: goto _LL2651; _LL2637:{ void* _temp2672= t2; struct Cyc_Absyn_PtrInfo
_temp2680; struct Cyc_Absyn_Conref* _temp2682; struct Cyc_Absyn_Tqual _temp2684;
struct Cyc_Absyn_Conref* _temp2686; void* _temp2688; void* _temp2690; _LL2674:
if(( unsigned int) _temp2672 >  3u?*(( int*) _temp2672) ==  Cyc_Absyn_PointerType:
0){ _LL2681: _temp2680=(( struct Cyc_Absyn_PointerType_struct*) _temp2672)->f1;
_LL2691: _temp2690=( void*) _temp2680.elt_typ; goto _LL2689; _LL2689: _temp2688=(
void*) _temp2680.rgn_typ; goto _LL2687; _LL2687: _temp2686= _temp2680.nullable;
goto _LL2685; _LL2685: _temp2684= _temp2680.tq; goto _LL2683; _LL2683: _temp2682=
_temp2680.bounds; goto _LL2675;} else{ goto _LL2676;} _LL2676: if( _temp2672 == (
void*) Cyc_Absyn_VoidType){ goto _LL2677;} else{ goto _LL2678;} _LL2678: goto
_LL2679; _LL2675: { struct Cyc_List_List* _temp2692=({ struct Cyc_List_List*
_temp2709=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2709->hd=( void*)({ struct _tuple8* _temp2710=( struct _tuple8*) _cycalloc(
sizeof( struct _tuple8)); _temp2710->f1= t1; _temp2710->f2= t2; _temp2710;});
_temp2709->tl= 0; _temp2709;}); int _temp2693= Cyc_Tcutil_ptrsubtype( te,
_temp2692, _temp2662, _temp2690)? ! _temp2656.q_const? 1: _temp2684.q_const: 0;
int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2654,
_temp2682); if( ! bounds_ok){ struct _tuple8 _temp2695=({ struct _tuple8
_temp2694; _temp2694.f1= Cyc_Absyn_conref_val( _temp2654); _temp2694.f2= Cyc_Absyn_conref_val(
_temp2682); _temp2694;}); void* _temp2701; struct Cyc_Absyn_Exp* _temp2703; void*
_temp2705; struct Cyc_Absyn_Exp* _temp2707; _LL2697: _LL2706: _temp2705=
_temp2695.f1; if(( unsigned int) _temp2705 >  1u?*(( int*) _temp2705) ==  Cyc_Absyn_Upper_b:
0){ _LL2708: _temp2707=(( struct Cyc_Absyn_Upper_b_struct*) _temp2705)->f1; goto
_LL2702;} else{ goto _LL2699;} _LL2702: _temp2701= _temp2695.f2; if((
unsigned int) _temp2701 >  1u?*(( int*) _temp2701) ==  Cyc_Absyn_Upper_b: 0){
_LL2704: _temp2703=(( struct Cyc_Absyn_Upper_b_struct*) _temp2701)->f1; goto
_LL2698;} else{ goto _LL2699;} _LL2699: goto _LL2700; _LL2698: if( Cyc_Evexp_eval_const_uint_exp(
_temp2707) >=  Cyc_Evexp_eval_const_uint_exp( _temp2703)){ bounds_ok= 1;} goto
_LL2696; _LL2700: bounds_ok= 1; goto _LL2696; _LL2696:;} return( bounds_ok?
_temp2693: 0)? Cyc_Tcutil_unify( _temp2660, _temp2688)? 1: Cyc_Tcenv_region_outlives(
te, _temp2660, _temp2688): 0;} _LL2677: return 1; _LL2679: goto _LL2673; _LL2673:;}
return 0; _LL2639:{ void* _temp2711= t2; struct Cyc_Absyn_Exp* _temp2717; struct
Cyc_Absyn_Tqual _temp2719; void* _temp2721; _LL2713: if(( unsigned int)
_temp2711 >  3u?*(( int*) _temp2711) ==  Cyc_Absyn_ArrayType: 0){ _LL2722:
_temp2721=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2711)->f1; goto
_LL2720; _LL2720: _temp2719=(( struct Cyc_Absyn_ArrayType_struct*) _temp2711)->f2;
goto _LL2718; _LL2718: _temp2717=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2711)->f3; goto _LL2714;} else{ goto _LL2715;} _LL2715: goto _LL2716;
_LL2714: { int okay; okay=( _temp2664 !=  0? _temp2717 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2664)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2717)): 0; return( okay? Cyc_Tcutil_unify(
_temp2668, _temp2721): 0)? ! _temp2666.q_const? 1: _temp2719.q_const: 0;}
_LL2716: return 0; _LL2712:;} return 0; _LL2641:{ void* _temp2723= t2; struct
Cyc_Absyn_Enumdecl* _temp2729; _LL2725: if(( unsigned int) _temp2723 >  3u?*((
int*) _temp2723) ==  Cyc_Absyn_EnumType: 0){ _LL2730: _temp2729=(( struct Cyc_Absyn_EnumType_struct*)
_temp2723)->f2; goto _LL2726;} else{ goto _LL2727;} _LL2727: goto _LL2728;
_LL2726: if(( _temp2670->fields !=  0? _temp2729->fields !=  0: 0)?(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2670->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2729->fields))->v):
0){ return 1;} goto _LL2724; _LL2728: goto _LL2724; _LL2724:;} goto _LL2643;
_LL2643: goto _LL2645; _LL2645: goto _LL2647; _LL2647: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType; _LL2649: return t2 == ( void*) Cyc_Absyn_VoidType;
_LL2651: return 0; _LL2635:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2731=
Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2732=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2732[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2733; _temp2733.tag= Cyc_Absyn_Cast_e;
_temp2733.f1=( void*) t; _temp2733.f2= _temp2731; _temp2733;}); _temp2732;})));
e->topt=({ struct Cyc_Core_Opt* _temp2734=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2734->v=( void*) t; _temp2734;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2735= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2737: if(( unsigned int) _temp2735
>  3u?*(( int*) _temp2735) ==  Cyc_Absyn_EnumType: 0){ goto _LL2738;} else{ goto
_LL2739;} _LL2739: if(( unsigned int) _temp2735 >  3u?*(( int*) _temp2735) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2740;} else{ goto _LL2741;} _LL2741: if((
unsigned int) _temp2735 >  3u?*(( int*) _temp2735) ==  Cyc_Absyn_IntType: 0){
goto _LL2742;} else{ goto _LL2743;} _LL2743: if(( unsigned int) _temp2735 >  3u?*((
int*) _temp2735) ==  Cyc_Absyn_Evar: 0){ goto _LL2744;} else{ goto _LL2745;}
_LL2745: goto _LL2746; _LL2738: goto _LL2740; _LL2740: goto _LL2742; _LL2742:
return 1; _LL2744: return Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t); _LL2746: return 0; _LL2736:;} int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){
return 1;}{ void* _temp2747= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2749: if( _temp2747 == ( void*) Cyc_Absyn_FloatType){
goto _LL2750;} else{ goto _LL2751;} _LL2751: if(( unsigned int) _temp2747 >  3u?*((
int*) _temp2747) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2752;} else{ goto _LL2753;}
_LL2753: goto _LL2754; _LL2750: goto _LL2752; _LL2752: return 1; _LL2754: return
0; _LL2748:;}} int Cyc_Tcutil_is_function_type( void* t){ void* _temp2755= Cyc_Tcutil_compress(
t); _LL2757: if(( unsigned int) _temp2755 >  3u?*(( int*) _temp2755) ==  Cyc_Absyn_FnType:
0){ goto _LL2758;} else{ goto _LL2759;} _LL2759: goto _LL2760; _LL2758: return 1;
_LL2760: return 0; _LL2756:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1,
void* t2){ struct _tuple8 _temp2762=({ struct _tuple8 _temp2761; _temp2761.f1=
t1; _temp2761.f2= t2; _temp2761;}); void* _temp2788; int _temp2790; void*
_temp2792; int _temp2794; void* _temp2796; void* _temp2798; void* _temp2800;
void* _temp2802; void* _temp2804; void* _temp2806; void* _temp2808; void*
_temp2810; void* _temp2812; void* _temp2814; void* _temp2816; void* _temp2818;
void* _temp2820; void* _temp2822; void* _temp2824; void* _temp2826; void*
_temp2828; void* _temp2830; void* _temp2832; void* _temp2834; void* _temp2836;
void* _temp2838; _LL2764: _LL2793: _temp2792= _temp2762.f1; if(( unsigned int)
_temp2792 >  3u?*(( int*) _temp2792) ==  Cyc_Absyn_DoubleType: 0){ _LL2795:
_temp2794=(( struct Cyc_Absyn_DoubleType_struct*) _temp2792)->f1; goto _LL2789;}
else{ goto _LL2766;} _LL2789: _temp2788= _temp2762.f2; if(( unsigned int)
_temp2788 >  3u?*(( int*) _temp2788) ==  Cyc_Absyn_DoubleType: 0){ _LL2791:
_temp2790=(( struct Cyc_Absyn_DoubleType_struct*) _temp2788)->f1; goto _LL2765;}
else{ goto _LL2766;} _LL2766: _LL2797: _temp2796= _temp2762.f1; if((
unsigned int) _temp2796 >  3u?*(( int*) _temp2796) ==  Cyc_Absyn_DoubleType: 0){
goto _LL2767;} else{ goto _LL2768;} _LL2768: _LL2799: _temp2798= _temp2762.f2;
if(( unsigned int) _temp2798 >  3u?*(( int*) _temp2798) ==  Cyc_Absyn_DoubleType:
0){ goto _LL2769;} else{ goto _LL2770;} _LL2770: _LL2801: _temp2800= _temp2762.f1;
if( _temp2800 == ( void*) Cyc_Absyn_FloatType){ goto _LL2771;} else{ goto
_LL2772;} _LL2772: _LL2803: _temp2802= _temp2762.f2; if( _temp2802 == ( void*)
Cyc_Absyn_FloatType){ goto _LL2773;} else{ goto _LL2774;} _LL2774: _LL2805:
_temp2804= _temp2762.f1; if(( unsigned int) _temp2804 >  3u?*(( int*) _temp2804)
==  Cyc_Absyn_IntType: 0){ _LL2809: _temp2808=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2804)->f1; if( _temp2808 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2807;}
else{ goto _LL2776;} _LL2807: _temp2806=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2804)->f2; if( _temp2806 == ( void*) Cyc_Absyn_B8){ goto _LL2775;} else{
goto _LL2776;}} else{ goto _LL2776;} _LL2776: _LL2811: _temp2810= _temp2762.f2;
if(( unsigned int) _temp2810 >  3u?*(( int*) _temp2810) ==  Cyc_Absyn_IntType: 0){
_LL2815: _temp2814=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2810)->f1;
if( _temp2814 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2813;} else{ goto _LL2778;}
_LL2813: _temp2812=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2810)->f2;
if( _temp2812 == ( void*) Cyc_Absyn_B8){ goto _LL2777;} else{ goto _LL2778;}}
else{ goto _LL2778;} _LL2778: _LL2817: _temp2816= _temp2762.f1; if((
unsigned int) _temp2816 >  3u?*(( int*) _temp2816) ==  Cyc_Absyn_IntType: 0){
_LL2821: _temp2820=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2816)->f1;
if( _temp2820 == ( void*) Cyc_Absyn_Signed){ goto _LL2819;} else{ goto _LL2780;}
_LL2819: _temp2818=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2816)->f2;
if( _temp2818 == ( void*) Cyc_Absyn_B8){ goto _LL2779;} else{ goto _LL2780;}}
else{ goto _LL2780;} _LL2780: _LL2823: _temp2822= _temp2762.f2; if((
unsigned int) _temp2822 >  3u?*(( int*) _temp2822) ==  Cyc_Absyn_IntType: 0){
_LL2827: _temp2826=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2822)->f1;
if( _temp2826 == ( void*) Cyc_Absyn_Signed){ goto _LL2825;} else{ goto _LL2782;}
_LL2825: _temp2824=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2822)->f2;
if( _temp2824 == ( void*) Cyc_Absyn_B8){ goto _LL2781;} else{ goto _LL2782;}}
else{ goto _LL2782;} _LL2782: _LL2829: _temp2828= _temp2762.f1; if((
unsigned int) _temp2828 >  3u?*(( int*) _temp2828) ==  Cyc_Absyn_IntType: 0){
_LL2833: _temp2832=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2828)->f1;
if( _temp2832 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2831;} else{ goto _LL2784;}
_LL2831: _temp2830=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2828)->f2;
if( _temp2830 == ( void*) Cyc_Absyn_B4){ goto _LL2783;} else{ goto _LL2784;}}
else{ goto _LL2784;} _LL2784: _LL2835: _temp2834= _temp2762.f2; if((
unsigned int) _temp2834 >  3u?*(( int*) _temp2834) ==  Cyc_Absyn_IntType: 0){
_LL2839: _temp2838=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2834)->f1;
if( _temp2838 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2837;} else{ goto _LL2786;}
_LL2837: _temp2836=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2834)->f2;
if( _temp2836 == ( void*) Cyc_Absyn_B4){ goto _LL2785;} else{ goto _LL2786;}}
else{ goto _LL2786;} _LL2786: goto _LL2787; _LL2765: if( _temp2794){ return t1;}
else{ return t2;} _LL2767: return t1; _LL2769: return t2; _LL2771: goto _LL2773;
_LL2773: return( void*) Cyc_Absyn_FloatType; _LL2775: goto _LL2777; _LL2777:
return Cyc_Absyn_ulonglong_t; _LL2779: goto _LL2781; _LL2781: return Cyc_Absyn_slonglong_t;
_LL2783: goto _LL2785; _LL2785: return Cyc_Absyn_uint_t; _LL2787: return Cyc_Absyn_sint_t;
_LL2763:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2840=( void*) e->r; struct Cyc_Core_Opt* _temp2846; _LL2842: if(*(( int*)
_temp2840) ==  Cyc_Absyn_AssignOp_e){ _LL2847: _temp2846=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2840)->f2; if( _temp2846 ==  0){ goto _LL2843;} else{ goto _LL2844;}} else{
goto _LL2844;} _LL2844: goto _LL2845; _LL2843:({ void* _temp2848[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2848, sizeof( void*), 0u));}); goto _LL2841; _LL2845: goto _LL2841; _LL2841:;}
static int Cyc_Tcutil_constrain_kinds( void* c1, void* c2){ c1= Cyc_Absyn_compress_kb(
c1); c2= Cyc_Absyn_compress_kb( c2);{ struct _tuple8 _temp2850=({ struct _tuple8
_temp2849; _temp2849.f1= c1; _temp2849.f2= c2; _temp2849;}); void* _temp2864;
void* _temp2866; void* _temp2868; void* _temp2870; void* _temp2872; struct Cyc_Core_Opt*
_temp2874; struct Cyc_Core_Opt** _temp2876; void* _temp2877; struct Cyc_Core_Opt*
_temp2879; struct Cyc_Core_Opt** _temp2881; void* _temp2882; void* _temp2884;
void* _temp2886; void* _temp2888; struct Cyc_Core_Opt* _temp2890; struct Cyc_Core_Opt**
_temp2892; void* _temp2893; void* _temp2895; struct Cyc_Core_Opt* _temp2897;
struct Cyc_Core_Opt** _temp2899; void* _temp2900; void* _temp2902; void*
_temp2904; void* _temp2906; struct Cyc_Core_Opt* _temp2908; struct Cyc_Core_Opt**
_temp2910; void* _temp2911; void* _temp2913; struct Cyc_Core_Opt* _temp2915;
struct Cyc_Core_Opt** _temp2917; _LL2852: _LL2869: _temp2868= _temp2850.f1; if(*((
int*) _temp2868) ==  Cyc_Absyn_Eq_kb){ _LL2871: _temp2870=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp2868)->f1; goto _LL2865;} else{ goto _LL2854;} _LL2865: _temp2864=
_temp2850.f2; if(*(( int*) _temp2864) ==  Cyc_Absyn_Eq_kb){ _LL2867: _temp2866=(
void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp2864)->f1; goto _LL2853;} else{
goto _LL2854;} _LL2854: _LL2873: _temp2872= _temp2850.f2; if(*(( int*) _temp2872)
==  Cyc_Absyn_Unknown_kb){ _LL2875: _temp2874=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp2872)->f1; _temp2876=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp2872)->f1; goto _LL2855;} else{ goto _LL2856;} _LL2856: _LL2878: _temp2877=
_temp2850.f1; if(*(( int*) _temp2877) ==  Cyc_Absyn_Unknown_kb){ _LL2880:
_temp2879=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp2877)->f1; _temp2881=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*) _temp2877)->f1;
goto _LL2857;} else{ goto _LL2858;} _LL2858: _LL2887: _temp2886= _temp2850.f1;
if(*(( int*) _temp2886) ==  Cyc_Absyn_Less_kb){ _LL2891: _temp2890=(( struct Cyc_Absyn_Less_kb_struct*)
_temp2886)->f1; _temp2892=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp2886)->f1; goto _LL2889; _LL2889: _temp2888=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp2886)->f2; goto _LL2883;} else{ goto _LL2860;} _LL2883: _temp2882=
_temp2850.f2; if(*(( int*) _temp2882) ==  Cyc_Absyn_Eq_kb){ _LL2885: _temp2884=(
void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp2882)->f1; goto _LL2859;} else{
goto _LL2860;} _LL2860: _LL2901: _temp2900= _temp2850.f1; if(*(( int*) _temp2900)
==  Cyc_Absyn_Eq_kb){ _LL2903: _temp2902=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp2900)->f1; goto _LL2894;} else{ goto _LL2862;} _LL2894: _temp2893=
_temp2850.f2; if(*(( int*) _temp2893) ==  Cyc_Absyn_Less_kb){ _LL2898: _temp2897=((
struct Cyc_Absyn_Less_kb_struct*) _temp2893)->f1; _temp2899=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*) _temp2893)->f1; goto _LL2896; _LL2896:
_temp2895=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp2893)->f2; goto
_LL2861;} else{ goto _LL2862;} _LL2862: _LL2912: _temp2911= _temp2850.f1; if(*((
int*) _temp2911) ==  Cyc_Absyn_Less_kb){ _LL2916: _temp2915=(( struct Cyc_Absyn_Less_kb_struct*)
_temp2911)->f1; _temp2917=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp2911)->f1; goto _LL2914; _LL2914: _temp2913=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp2911)->f2; goto _LL2905;} else{ goto _LL2851;} _LL2905: _temp2904=
_temp2850.f2; if(*(( int*) _temp2904) ==  Cyc_Absyn_Less_kb){ _LL2909: _temp2908=((
struct Cyc_Absyn_Less_kb_struct*) _temp2904)->f1; _temp2910=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*) _temp2904)->f1; goto _LL2907; _LL2907:
_temp2906=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp2904)->f2; goto
_LL2863;} else{ goto _LL2851;} _LL2853: return _temp2870 ==  _temp2866; _LL2855:*
_temp2876=({ struct Cyc_Core_Opt* _temp2918=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2918->v=( void*) c1; _temp2918;}); return 1;
_LL2857:* _temp2881=({ struct Cyc_Core_Opt* _temp2919=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2919->v=( void*) c2; _temp2919;});
return 1; _LL2859: if( Cyc_Tcutil_kind_leq( _temp2884, _temp2888)){* _temp2892=({
struct Cyc_Core_Opt* _temp2920=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2920->v=( void*) c2; _temp2920;}); return 1;} else{ return
0;} _LL2861: if( Cyc_Tcutil_kind_leq( _temp2902, _temp2895)){* _temp2899=({
struct Cyc_Core_Opt* _temp2921=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2921->v=( void*) c1; _temp2921;}); return 1;} else{ return
0;} _LL2863: if( Cyc_Tcutil_kind_leq( _temp2913, _temp2906)){* _temp2910=({
struct Cyc_Core_Opt* _temp2922=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2922->v=( void*) c1; _temp2922;}); return 1;} else{ if( Cyc_Tcutil_kind_leq(
_temp2906, _temp2913)){* _temp2917=({ struct Cyc_Core_Opt* _temp2923=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2923->v=( void*)
c2; _temp2923;}); return 1;} else{ return 0;}} _LL2851:;}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2924=( int*)
_cycalloc_atomic( sizeof( int)); _temp2924[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2924;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_arr s=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp2928; _temp2928.tag= Cyc_Std_Int_pa;
_temp2928.f1=( int)(( unsigned int) i);{ void* _temp2927[ 1u]={& _temp2928}; Cyc_Std_aprintf(
_tag_arr("#%d", sizeof( unsigned char), 4u), _tag_arr( _temp2927, sizeof( void*),
1u));}}); return({ struct Cyc_Absyn_Tvar* _temp2925=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2925->name=({ struct
_tagged_arr* _temp2926=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp2926[ 0]= s; _temp2926;}); _temp2925->identity= 0;
_temp2925->kind=( void*) k; _temp2925;});} int Cyc_Tcutil_is_temp_tvar( struct
Cyc_Absyn_Tvar* t){ struct _tagged_arr _temp2929=* t->name; return*(( const
unsigned char*) _check_unknown_subscript( _temp2929, sizeof( unsigned char), 0))
== '#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct
Cyc_Std_String_pa_struct _temp2931; _temp2931.tag= Cyc_Std_String_pa; _temp2931.f1=(
struct _tagged_arr)* t->name;{ void* _temp2930[ 1u]={& _temp2931}; Cyc_Std_printf(
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp2930, sizeof( void*),
1u));}}); if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr
_temp2932= Cyc_Std_strconcat( _tag_arr("`", sizeof( unsigned char), 2u),* t->name);*((
unsigned char*) _check_unknown_subscript( _temp2932, sizeof( unsigned char), 1))='t';
t->name=({ struct _tagged_arr* _temp2933=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr) *  1); _temp2933[ 0]=( struct _tagged_arr) _temp2932;
_temp2933;});}} struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct
_tuple13* x){ return({ struct _tuple2* _temp2934=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp2934->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2935=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2935->v=( void*)(* x).f1; _temp2935;}); _temp2934->f2=(* x).f2; _temp2934->f3=(*
x).f3; _temp2934;});} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){
if( fd->cached_typ ==  0){ struct Cyc_List_List* _temp2936= 0;{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts !=  0; atts= atts->tl){ if( Cyc_Absyn_fntype_att((
void*) atts->hd)){ _temp2936=({ struct Cyc_List_List* _temp2937=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2937->hd=( void*)(( void*) atts->hd);
_temp2937->tl= _temp2936; _temp2937;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2938=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2938[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2939; _temp2939.tag= Cyc_Absyn_FnType;
_temp2939.f1=({ struct Cyc_Absyn_FnInfo _temp2940; _temp2940.tvars= fd->tvs;
_temp2940.effect= fd->effect; _temp2940.ret_typ=( void*)(( void*) fd->ret_type);
_temp2940.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2940.c_varargs= fd->c_varargs; _temp2940.cyc_varargs= fd->cyc_varargs;
_temp2940.rgn_po= fd->rgn_po; _temp2940.attributes= _temp2936; _temp2940;});
_temp2939;}); _temp2938;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp2941=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp2941->f1=(* pr).f1; _temp2941->f2= t; _temp2941;});} struct _tuple15{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct
_tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2942=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2942->f1=({ struct _tuple15* _temp2943=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2943->f1=(* y).f1; _temp2943->f2=(* y).f2;
_temp2943;}); _temp2942->f2=(* y).f3; _temp2942;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2946; struct _tuple15* _temp2948; struct
_tuple16 _temp2944=* w; _LL2949: _temp2948= _temp2944.f1; goto _LL2947; _LL2947:
_temp2946= _temp2944.f2; goto _LL2945; _LL2945: { struct Cyc_Absyn_Tqual
_temp2952; struct Cyc_Core_Opt* _temp2954; struct _tuple15 _temp2950=* _temp2948;
_LL2955: _temp2954= _temp2950.f1; goto _LL2953; _LL2953: _temp2952= _temp2950.f2;
goto _LL2951; _LL2951: return({ struct _tuple2* _temp2956=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp2956->f1= _temp2954; _temp2956->f2=
_temp2952; _temp2956->f3= _temp2946; _temp2956;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2957=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2957->name= f->name;
_temp2957->tq= f->tq; _temp2957->type=( void*) t; _temp2957->width= f->width;
_temp2957->attributes= f->attributes; _temp2957;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2958= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp3008; struct Cyc_Absyn_Structdecl** _temp3010;
struct Cyc_List_List* _temp3012; struct _tuple1* _temp3014; struct Cyc_Absyn_Uniondecl**
_temp3016; struct Cyc_List_List* _temp3018; struct _tuple1* _temp3020; struct
Cyc_Absyn_TunionInfo _temp3022; void* _temp3024; struct Cyc_List_List* _temp3026;
void* _temp3028; struct Cyc_Absyn_TunionFieldInfo _temp3030; struct Cyc_List_List*
_temp3032; void* _temp3034; struct Cyc_Core_Opt* _temp3036; struct Cyc_List_List*
_temp3038; struct _tuple1* _temp3040; struct Cyc_Absyn_Exp* _temp3042; struct
Cyc_Absyn_Tqual _temp3044; void* _temp3046; struct Cyc_Absyn_PtrInfo _temp3048;
struct Cyc_Absyn_Conref* _temp3050; struct Cyc_Absyn_Tqual _temp3052; struct Cyc_Absyn_Conref*
_temp3054; void* _temp3056; void* _temp3058; struct Cyc_Absyn_FnInfo _temp3060;
struct Cyc_List_List* _temp3062; struct Cyc_List_List* _temp3064; struct Cyc_Absyn_VarargInfo*
_temp3066; int _temp3068; struct Cyc_List_List* _temp3070; void* _temp3072;
struct Cyc_Core_Opt* _temp3074; struct Cyc_List_List* _temp3076; struct Cyc_List_List*
_temp3078; struct Cyc_List_List* _temp3080; struct Cyc_List_List* _temp3082;
struct Cyc_Core_Opt* _temp3084; void* _temp3086; void* _temp3088; void*
_temp3090; struct Cyc_List_List* _temp3092; _LL2960: if(( unsigned int)
_temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_VarType: 0){ _LL3009:
_temp3008=(( struct Cyc_Absyn_VarType_struct*) _temp2958)->f1; goto _LL2961;}
else{ goto _LL2962;} _LL2962: if(( unsigned int) _temp2958 >  3u?*(( int*)
_temp2958) ==  Cyc_Absyn_StructType: 0){ _LL3015: _temp3014=(( struct Cyc_Absyn_StructType_struct*)
_temp2958)->f1; goto _LL3013; _LL3013: _temp3012=(( struct Cyc_Absyn_StructType_struct*)
_temp2958)->f2; goto _LL3011; _LL3011: _temp3010=(( struct Cyc_Absyn_StructType_struct*)
_temp2958)->f3; goto _LL2963;} else{ goto _LL2964;} _LL2964: if(( unsigned int)
_temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_UnionType: 0){ _LL3021:
_temp3020=(( struct Cyc_Absyn_UnionType_struct*) _temp2958)->f1; goto _LL3019;
_LL3019: _temp3018=(( struct Cyc_Absyn_UnionType_struct*) _temp2958)->f2; goto
_LL3017; _LL3017: _temp3016=(( struct Cyc_Absyn_UnionType_struct*) _temp2958)->f3;
goto _LL2965;} else{ goto _LL2966;} _LL2966: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_TunionType: 0){ _LL3023: _temp3022=(( struct Cyc_Absyn_TunionType_struct*)
_temp2958)->f1; _LL3029: _temp3028=( void*) _temp3022.tunion_info; goto _LL3027;
_LL3027: _temp3026= _temp3022.targs; goto _LL3025; _LL3025: _temp3024=( void*)
_temp3022.rgn; goto _LL2967;} else{ goto _LL2968;} _LL2968: if(( unsigned int)
_temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_TunionFieldType: 0){ _LL3031:
_temp3030=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2958)->f1; _LL3035:
_temp3034=( void*) _temp3030.field_info; goto _LL3033; _LL3033: _temp3032=
_temp3030.targs; goto _LL2969;} else{ goto _LL2970;} _LL2970: if(( unsigned int)
_temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_TypedefType: 0){ _LL3041:
_temp3040=(( struct Cyc_Absyn_TypedefType_struct*) _temp2958)->f1; goto _LL3039;
_LL3039: _temp3038=(( struct Cyc_Absyn_TypedefType_struct*) _temp2958)->f2; goto
_LL3037; _LL3037: _temp3036=(( struct Cyc_Absyn_TypedefType_struct*) _temp2958)->f3;
goto _LL2971;} else{ goto _LL2972;} _LL2972: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_ArrayType: 0){ _LL3047: _temp3046=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2958)->f1; goto _LL3045; _LL3045:
_temp3044=(( struct Cyc_Absyn_ArrayType_struct*) _temp2958)->f2; goto _LL3043;
_LL3043: _temp3042=(( struct Cyc_Absyn_ArrayType_struct*) _temp2958)->f3; goto
_LL2973;} else{ goto _LL2974;} _LL2974: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_PointerType: 0){ _LL3049: _temp3048=(( struct Cyc_Absyn_PointerType_struct*)
_temp2958)->f1; _LL3059: _temp3058=( void*) _temp3048.elt_typ; goto _LL3057;
_LL3057: _temp3056=( void*) _temp3048.rgn_typ; goto _LL3055; _LL3055: _temp3054=
_temp3048.nullable; goto _LL3053; _LL3053: _temp3052= _temp3048.tq; goto _LL3051;
_LL3051: _temp3050= _temp3048.bounds; goto _LL2975;} else{ goto _LL2976;}
_LL2976: if(( unsigned int) _temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_FnType:
0){ _LL3061: _temp3060=(( struct Cyc_Absyn_FnType_struct*) _temp2958)->f1;
_LL3077: _temp3076= _temp3060.tvars; goto _LL3075; _LL3075: _temp3074= _temp3060.effect;
goto _LL3073; _LL3073: _temp3072=( void*) _temp3060.ret_typ; goto _LL3071;
_LL3071: _temp3070= _temp3060.args; goto _LL3069; _LL3069: _temp3068= _temp3060.c_varargs;
goto _LL3067; _LL3067: _temp3066= _temp3060.cyc_varargs; goto _LL3065; _LL3065:
_temp3064= _temp3060.rgn_po; goto _LL3063; _LL3063: _temp3062= _temp3060.attributes;
goto _LL2977;} else{ goto _LL2978;} _LL2978: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_TupleType: 0){ _LL3079: _temp3078=(( struct Cyc_Absyn_TupleType_struct*)
_temp2958)->f1; goto _LL2979;} else{ goto _LL2980;} _LL2980: if(( unsigned int)
_temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_AnonStructType: 0){ _LL3081:
_temp3080=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2958)->f1; goto
_LL2981;} else{ goto _LL2982;} _LL2982: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3083: _temp3082=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2958)->f1; goto _LL2983;} else{ goto
_LL2984;} _LL2984: if(( unsigned int) _temp2958 >  3u?*(( int*) _temp2958) == 
Cyc_Absyn_Evar: 0){ _LL3085: _temp3084=(( struct Cyc_Absyn_Evar_struct*)
_temp2958)->f2; goto _LL2985;} else{ goto _LL2986;} _LL2986: if(( unsigned int)
_temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_RgnHandleType: 0){ _LL3087:
_temp3086=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2958)->f1;
goto _LL2987;} else{ goto _LL2988;} _LL2988: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_EnumType: 0){ goto _LL2989;} else{ goto _LL2990;}
_LL2990: if(( unsigned int) _temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL2991;} else{ goto _LL2992;} _LL2992: if( _temp2958 == ( void*) Cyc_Absyn_VoidType){
goto _LL2993;} else{ goto _LL2994;} _LL2994: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_IntType: 0){ goto _LL2995;} else{ goto _LL2996;}
_LL2996: if( _temp2958 == ( void*) Cyc_Absyn_FloatType){ goto _LL2997;} else{
goto _LL2998;} _LL2998: if(( unsigned int) _temp2958 >  3u?*(( int*) _temp2958)
==  Cyc_Absyn_DoubleType: 0){ goto _LL2999;} else{ goto _LL3000;} _LL3000: if(
_temp2958 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL3001;} else{ goto _LL3002;}
_LL3002: if(( unsigned int) _temp2958 >  3u?*(( int*) _temp2958) ==  Cyc_Absyn_RgnsEff:
0){ _LL3089: _temp3088=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp2958)->f1;
goto _LL3003;} else{ goto _LL3004;} _LL3004: if(( unsigned int) _temp2958 >  3u?*((
int*) _temp2958) ==  Cyc_Absyn_AccessEff: 0){ _LL3091: _temp3090=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2958)->f1; goto _LL3005;} else{ goto
_LL3006;} _LL3006: if(( unsigned int) _temp2958 >  3u?*(( int*) _temp2958) == 
Cyc_Absyn_JoinEff: 0){ _LL3093: _temp3092=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2958)->f1; goto _LL3007;} else{ goto _LL2959;} _LL2961: { struct
_handler_cons _temp3094; _push_handler(& _temp3094);{ int _temp3096= 0; if(
setjmp( _temp3094.handler)){ _temp3096= 1;} if( ! _temp3096){{ void* _temp3097=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp3008); _npop_handler( 0u); return _temp3097;}; _pop_handler();} else{
void* _temp3095=( void*) _exn_thrown; void* _temp3099= _temp3095; _LL3101: if(
_temp3099 ==  Cyc_Core_Not_found){ goto _LL3102;} else{ goto _LL3103;} _LL3103:
goto _LL3104; _LL3102: return t; _LL3104:( void) _throw( _temp3099); _LL3100:;}}}
_LL2963: { struct Cyc_List_List* _temp3105= Cyc_Tcutil_substs( rgn, inst,
_temp3012); return _temp3105 ==  _temp3012? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp3106=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3106[ 0]=({ struct Cyc_Absyn_StructType_struct _temp3107; _temp3107.tag=
Cyc_Absyn_StructType; _temp3107.f1= _temp3014; _temp3107.f2= _temp3105;
_temp3107.f3= _temp3010; _temp3107;}); _temp3106;});} _LL2965: { struct Cyc_List_List*
_temp3108= Cyc_Tcutil_substs( rgn, inst, _temp3018); return _temp3108 == 
_temp3018? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp3109=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3109[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3110; _temp3110.tag= Cyc_Absyn_UnionType;
_temp3110.f1= _temp3020; _temp3110.f2= _temp3108; _temp3110.f3= _temp3016;
_temp3110;}); _temp3109;});} _LL2967: { struct Cyc_List_List* _temp3111= Cyc_Tcutil_substs(
rgn, inst, _temp3026); void* _temp3112= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3024); return( _temp3111 ==  _temp3026? _temp3112 ==  _temp3024: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp3113=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3113[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3114; _temp3114.tag= Cyc_Absyn_TunionType;
_temp3114.f1=({ struct Cyc_Absyn_TunionInfo _temp3115; _temp3115.tunion_info=(
void*) _temp3028; _temp3115.targs= _temp3111; _temp3115.rgn=( void*) _temp3112;
_temp3115;}); _temp3114;}); _temp3113;});} _LL2969: { struct Cyc_List_List*
_temp3116= Cyc_Tcutil_substs( rgn, inst, _temp3032); return _temp3116 == 
_temp3032? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp3117=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp3117[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp3118; _temp3118.tag=
Cyc_Absyn_TunionFieldType; _temp3118.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp3119; _temp3119.field_info=( void*) _temp3034; _temp3119.targs= _temp3116;
_temp3119;}); _temp3118;}); _temp3117;});} _LL2971: { struct Cyc_List_List*
_temp3120= Cyc_Tcutil_substs( rgn, inst, _temp3038); return _temp3120 == 
_temp3038? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp3121=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3121[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3122; _temp3122.tag=
Cyc_Absyn_TypedefType; _temp3122.f1= _temp3040; _temp3122.f2= _temp3120;
_temp3122.f3= _temp3036; _temp3122;}); _temp3121;});} _LL2973: { void* _temp3123=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp3046); return _temp3123 ==  _temp3046? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp3124=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp3124[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp3125; _temp3125.tag= Cyc_Absyn_ArrayType;
_temp3125.f1=( void*) _temp3123; _temp3125.f2= _temp3044; _temp3125.f3=
_temp3042; _temp3125;}); _temp3124;});} _LL2975: { void* _temp3126= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3058); void* _temp3127= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3056); if( _temp3126 ==  _temp3058? _temp3127 ==  _temp3056: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp3128=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp3128[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp3129; _temp3129.tag= Cyc_Absyn_PointerType;
_temp3129.f1=({ struct Cyc_Absyn_PtrInfo _temp3130; _temp3130.elt_typ=( void*)
_temp3126; _temp3130.rgn_typ=( void*) _temp3127; _temp3130.nullable= _temp3054;
_temp3130.tq= _temp3052; _temp3130.bounds= _temp3050; _temp3130;}); _temp3129;});
_temp3128;});} _LL2977:{ struct Cyc_List_List* _temp3131= _temp3076; for( 0;
_temp3131 !=  0; _temp3131= _temp3131->tl){ inst=({ struct Cyc_List_List*
_temp3132=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3132->hd=( void*)({ struct _tuple5* _temp3133=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp3133->f1=( struct Cyc_Absyn_Tvar*)
_temp3131->hd; _temp3133->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3134=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3134[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3135; _temp3135.tag= Cyc_Absyn_VarType;
_temp3135.f1=( struct Cyc_Absyn_Tvar*) _temp3131->hd; _temp3135;}); _temp3134;});
_temp3133;}); _temp3132->tl= inst; _temp3132;});}}{ struct Cyc_List_List*
_temp3138; struct Cyc_List_List* _temp3140; struct _tuple0 _temp3136=(( struct
_tuple0(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp3070)); _LL3141: _temp3140= _temp3136.f1; goto _LL3139; _LL3139:
_temp3138= _temp3136.f2; goto _LL3137; _LL3137: { struct Cyc_List_List*
_temp3142= Cyc_Tcutil_substs( rgn, inst, _temp3138); struct Cyc_List_List*
_temp3143=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple16*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3140, _temp3142));
struct Cyc_Core_Opt* eff2; if( _temp3074 ==  0){ eff2= 0;} else{ void* _temp3144=
Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp3074->v); if( _temp3144 == (
void*) _temp3074->v){ eff2= _temp3074;} else{ eff2=({ struct Cyc_Core_Opt*
_temp3145=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3145->v=( void*) _temp3144; _temp3145;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp3066 ==  0){ cyc_varargs2= 0;} else{ int _temp3148; void*
_temp3150; struct Cyc_Absyn_Tqual _temp3152; struct Cyc_Core_Opt* _temp3154;
struct Cyc_Absyn_VarargInfo _temp3146=* _temp3066; _LL3155: _temp3154= _temp3146.name;
goto _LL3153; _LL3153: _temp3152= _temp3146.tq; goto _LL3151; _LL3151: _temp3150=(
void*) _temp3146.type; goto _LL3149; _LL3149: _temp3148= _temp3146.inject; goto
_LL3147; _LL3147: { void* _temp3156= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3150); if( _temp3156 ==  _temp3150){ cyc_varargs2= _temp3066;} else{
cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp3157=( struct Cyc_Absyn_VarargInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp3157->name= _temp3154;
_temp3157->tq= _temp3152; _temp3157->type=( void*) _temp3156; _temp3157->inject=
_temp3148; _temp3157;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List*
_temp3160; struct Cyc_List_List* _temp3162; struct _tuple0 _temp3158= Cyc_List_rsplit(
rgn, rgn, _temp3064); _LL3163: _temp3162= _temp3158.f1; goto _LL3161; _LL3161:
_temp3160= _temp3158.f2; goto _LL3159; _LL3159: { struct Cyc_List_List*
_temp3164= Cyc_Tcutil_substs( rgn, inst, _temp3162); struct Cyc_List_List*
_temp3165= Cyc_Tcutil_substs( rgn, inst, _temp3160); if( _temp3164 ==  _temp3162?
_temp3165 ==  _temp3160: 0){ rgn_po2= _temp3064;} else{ rgn_po2= Cyc_List_zip(
_temp3164, _temp3165);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp3166=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp3166[ 0]=({ struct Cyc_Absyn_FnType_struct _temp3167; _temp3167.tag= Cyc_Absyn_FnType;
_temp3167.f1=({ struct Cyc_Absyn_FnInfo _temp3168; _temp3168.tvars= _temp3076;
_temp3168.effect= eff2; _temp3168.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp3072); _temp3168.args= _temp3143; _temp3168.c_varargs= _temp3068;
_temp3168.cyc_varargs= cyc_varargs2; _temp3168.rgn_po= rgn_po2; _temp3168.attributes=
_temp3062; _temp3168;}); _temp3167;}); _temp3166;});}}}}} _LL2979: { struct Cyc_List_List*
_temp3169=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp3078); struct Cyc_List_List* _temp3170= Cyc_Tcutil_substs( rgn, inst,
_temp3169); if( _temp3170 ==  _temp3169){ return t;}{ struct Cyc_List_List*
_temp3171=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp3078, _temp3170); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3172=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3172[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3173; _temp3173.tag= Cyc_Absyn_TupleType;
_temp3173.f1= _temp3171; _temp3173;}); _temp3172;});}} _LL2981: { struct Cyc_List_List*
_temp3174=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp3080); struct Cyc_List_List* _temp3175= Cyc_Tcutil_substs( rgn, inst,
_temp3174); if( _temp3175 ==  _temp3174){ return t;}{ struct Cyc_List_List*
_temp3176=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp3080, _temp3175); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3177=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3177[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3178; _temp3178.tag= Cyc_Absyn_AnonStructType;
_temp3178.f1= _temp3176; _temp3178;}); _temp3177;});}} _LL2983: { struct Cyc_List_List*
_temp3179=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp3082); struct Cyc_List_List* _temp3180= Cyc_Tcutil_substs( rgn, inst,
_temp3179); if( _temp3180 ==  _temp3179){ return t;}{ struct Cyc_List_List*
_temp3181=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp3082, _temp3180); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3182=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3182[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3183; _temp3183.tag= Cyc_Absyn_AnonStructType;
_temp3183.f1= _temp3181; _temp3183;}); _temp3182;});}} _LL2985: if( _temp3084 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp3084->v);} else{
return t;} _LL2987: { void* _temp3184= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3086); return _temp3184 ==  _temp3086? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3185=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3185[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3186; _temp3186.tag= Cyc_Absyn_RgnHandleType; _temp3186.f1=( void*)
_temp3184; _temp3186;}); _temp3185;});} _LL2989: return t; _LL2991: return t;
_LL2993: return t; _LL2995: return t; _LL2997: return t; _LL2999: return t;
_LL3001: return t; _LL3003: { void* _temp3187= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3088); return _temp3187 ==  _temp3088? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp3188=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3188[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3189; _temp3189.tag= Cyc_Absyn_RgnsEff;
_temp3189.f1=( void*) _temp3187; _temp3189;}); _temp3188;});} _LL3005: { void*
_temp3190= Cyc_Tcutil_rsubstitute( rgn, inst, _temp3090); return _temp3190 == 
_temp3090? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3191=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3191[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3192; _temp3192.tag= Cyc_Absyn_AccessEff;
_temp3192.f1=( void*) _temp3190; _temp3192;}); _temp3191;});} _LL3007: { struct
Cyc_List_List* _temp3193= Cyc_Tcutil_substs( rgn, inst, _temp3092); return
_temp3193 ==  _temp3092? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3194=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3194[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3195; _temp3195.tag= Cyc_Absyn_JoinEff;
_temp3195.f1= _temp3193; _temp3195;}); _temp3194;});} _LL2959:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp3196=(
void*) ts->hd; struct Cyc_List_List* _temp3197= ts->tl; void* _temp3198= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3196); struct Cyc_List_List* _temp3199= Cyc_Tcutil_substs( rgn,
inst, _temp3197); if( _temp3196 ==  _temp3198? _temp3197 ==  _temp3199: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp3200=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3200->hd=(
void*) _temp3198; _temp3200->tl= _temp3199; _temp3200;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp3245=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3245[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3246; _temp3246.tag= Cyc_Absyn_Const_e; _temp3246.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp3246;}); _temp3245;}), loc);{ void*
_temp3201= Cyc_Tcutil_compress( t); void* _temp3215; void* _temp3217; void*
_temp3219; void* _temp3221; _LL3203: if(( unsigned int) _temp3201 >  3u?*(( int*)
_temp3201) ==  Cyc_Absyn_PointerType: 0){ goto _LL3204;} else{ goto _LL3205;}
_LL3205: if(( unsigned int) _temp3201 >  3u?*(( int*) _temp3201) ==  Cyc_Absyn_IntType:
0){ _LL3218: _temp3217=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3201)->f1;
goto _LL3216; _LL3216: _temp3215=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3201)->f2; if( _temp3215 == ( void*) Cyc_Absyn_B1){ goto _LL3206;} else{
goto _LL3207;}} else{ goto _LL3207;} _LL3207: if(( unsigned int) _temp3201 >  3u?*((
int*) _temp3201) ==  Cyc_Absyn_IntType: 0){ _LL3222: _temp3221=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp3201)->f1; goto _LL3220; _LL3220: _temp3219=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3201)->f2; goto _LL3208;} else{
goto _LL3209;} _LL3209: if( _temp3201 == ( void*) Cyc_Absyn_FloatType){ goto
_LL3210;} else{ goto _LL3211;} _LL3211: if(( unsigned int) _temp3201 >  3u?*((
int*) _temp3201) ==  Cyc_Absyn_DoubleType: 0){ goto _LL3212;} else{ goto _LL3213;}
_LL3213: goto _LL3214; _LL3204: goto _LL3202; _LL3206:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp3223=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3223[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3224; _temp3224.tag= Cyc_Absyn_Const_e; _temp3224.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp3225=( struct Cyc_Absyn_Char_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp3225[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp3226; _temp3226.tag= Cyc_Absyn_Char_c; _temp3226.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3226.f2='\000'; _temp3226;}); _temp3225;})); _temp3224;}); _temp3223;})));
goto _LL3202; _LL3208:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp3227=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3227[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3228; _temp3228.tag= Cyc_Absyn_Const_e;
_temp3228.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp3229=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp3229[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp3230; _temp3230.tag= Cyc_Absyn_Int_c;
_temp3230.f1=( void*) _temp3221; _temp3230.f2= 0; _temp3230;}); _temp3229;}));
_temp3228;}); _temp3227;}))); if( _temp3219 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp3231=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp3231[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp3232; _temp3232.tag= Cyc_Absyn_Cast_e; _temp3232.f1=( void*) t; _temp3232.f2=
e; _temp3232;}); _temp3231;}), loc);} goto _LL3202; _LL3210:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3233=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3233[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3234; _temp3234.tag= Cyc_Absyn_Const_e; _temp3234.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3235=( struct Cyc_Absyn_Float_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp3235[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp3236; _temp3236.tag= Cyc_Absyn_Float_c; _temp3236.f1=
_tag_arr("0.0", sizeof( unsigned char), 4u); _temp3236;}); _temp3235;}));
_temp3234;}); _temp3233;}))); goto _LL3202; _LL3212:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp3237=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp3237[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp3238; _temp3238.tag= Cyc_Absyn_Cast_e; _temp3238.f1=( void*) t; _temp3238.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3239=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3239[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3240; _temp3240.tag= Cyc_Absyn_Const_e;
_temp3240.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3241=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp3241[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp3242; _temp3242.tag= Cyc_Absyn_Float_c;
_temp3242.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp3242;});
_temp3241;})); _temp3240;}); _temp3239;}), loc); _temp3238;}); _temp3237;})));
goto _LL3202; _LL3214:({ struct Cyc_Std_String_pa_struct _temp3244; _temp3244.tag=
Cyc_Std_String_pa; _temp3244.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp3243[ 1u]={& _temp3244}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp3243, sizeof( void*), 1u));}});
goto _LL3202; _LL3202:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Tcutil_tvar_kind(
tv); return({ struct _tuple5* _temp3247=( struct _tuple5*) _cycalloc( sizeof(
struct _tuple5)); _temp3247->f1= tv; _temp3247->f2= Cyc_Absyn_new_evar(({ struct
Cyc_Core_Opt* _temp3248=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3248->v=( void*) k; _temp3248;}),({ struct Cyc_Core_Opt* _temp3249=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3249->v=( void*) s;
_temp3249;})); _temp3247;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp3252; struct
_RegionHandle* _temp3253; struct Cyc_List_List* _temp3255; struct _tuple6*
_temp3250= env; _temp3252=* _temp3250; _LL3256: _temp3255= _temp3252.f1; goto
_LL3254; _LL3254: _temp3253= _temp3252.f2; goto _LL3251; _LL3251: { void* k= Cyc_Tcutil_tvar_kind(
tv); return({ struct _tuple5* _temp3257=( struct _tuple5*) _region_malloc(
_temp3253, sizeof( struct _tuple5)); _temp3257->f1= tv; _temp3257->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3258=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3258->v=( void*) k; _temp3258;}),({ struct Cyc_Core_Opt*
_temp3259=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3259->v=( void*) _temp3255; _temp3259;})); _temp3257;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if( Cyc_Std_zstrptrcmp((( struct
Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) ==  0){ void* k1=( void*)(( struct
Cyc_Absyn_Tvar*) tvs2->hd)->kind; void* k2=( void*) tv->kind; if( ! Cyc_Tcutil_constrain_kinds(
k1, k2)){({ struct Cyc_Std_String_pa_struct _temp3263; _temp3263.tag= Cyc_Std_String_pa;
_temp3263.f1=( struct _tagged_arr) Cyc_Absynpp_kindbound2string( k2);{ struct
Cyc_Std_String_pa_struct _temp3262; _temp3262.tag= Cyc_Std_String_pa; _temp3262.f1=(
struct _tagged_arr) Cyc_Absynpp_kindbound2string( k1);{ struct Cyc_Std_String_pa_struct
_temp3261; _temp3261.tag= Cyc_Std_String_pa; _temp3261.f1=( struct _tagged_arr)*
tv->name;{ void* _temp3260[ 3u]={& _temp3261,& _temp3262,& _temp3263}; Cyc_Tcutil_terr(
loc, _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u), _tag_arr( _temp3260, sizeof( void*), 3u));}}}});}
if( tv->identity ==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) != *(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity))){({ void* _temp3264[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp3264, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp3265=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3265->hd=( void*) tv; _temp3265->tl= tvs; _temp3265;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp3266[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp3266, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity ==  0){({ void* _temp3267[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u),
_tag_arr( _temp3267, sizeof( void*), 0u));});} if(*(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp3268=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3268->hd=( void*) tv; _temp3268->tl=
tvs; _temp3268;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp3270; _temp3270.tag= Cyc_Std_String_pa;
_temp3270.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp3269[
1u]={& _temp3270};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp3269, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp3271=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3271->hd=(
void*) tv; _temp3271->tl= tvs; _temp3271;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp3272= Cyc_Tcutil_compress( e);
int _temp3278; _LL3274: if(( unsigned int) _temp3272 >  3u?*(( int*) _temp3272)
==  Cyc_Absyn_Evar: 0){ _LL3279: _temp3278=(( struct Cyc_Absyn_Evar_struct*)
_temp3272)->f3; goto _LL3275;} else{ goto _LL3276;} _LL3276: goto _LL3277;
_LL3275:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2= es2->tl){ void*
_temp3280= Cyc_Tcutil_compress(( void*) es2->hd); int _temp3286; _LL3282: if((
unsigned int) _temp3280 >  3u?*(( int*) _temp3280) ==  Cyc_Absyn_Evar: 0){
_LL3287: _temp3286=(( struct Cyc_Absyn_Evar_struct*) _temp3280)->f3; goto
_LL3283;} else{ goto _LL3284;} _LL3284: goto _LL3285; _LL3283: if( _temp3278 == 
_temp3286){ return es;} goto _LL3281; _LL3285: goto _LL3281; _LL3281:;}} return({
struct Cyc_List_List* _temp3288=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3288->hd=( void*) e; _temp3288->tl= es; _temp3288;});
_LL3277: return es; _LL3273:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs= tvs->tl){ int present= 0;{ struct Cyc_List_List* b=
btvs; for( 0; b !=  0; b= b->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)
tvs->hd)->identity)) == *(( int*) _check_null((( struct Cyc_Absyn_Tvar*) b->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp3289=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3289->hd=(
void*)(( struct Cyc_Absyn_Tvar*) tvs->hd); _temp3289->tl= r; _temp3289;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp3291; _temp3291.tag= Cyc_Std_String_pa; _temp3291.f1=( struct _tagged_arr)*
fn;{ void* _temp3290[ 1u]={& _temp3291}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp3290, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp3292= Cyc_Tcutil_compress( field_typ); void* _temp3298;
_LL3294: if(( unsigned int) _temp3292 >  3u?*(( int*) _temp3292) ==  Cyc_Absyn_IntType:
0){ _LL3299: _temp3298=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3292)->f2;
goto _LL3295;} else{ goto _LL3296;} _LL3296: goto _LL3297; _LL3295:{ void*
_temp3300= _temp3298; _LL3302: if( _temp3300 == ( void*) Cyc_Absyn_B1){ goto
_LL3303;} else{ goto _LL3304;} _LL3304: if( _temp3300 == ( void*) Cyc_Absyn_B2){
goto _LL3305;} else{ goto _LL3306;} _LL3306: if( _temp3300 == ( void*) Cyc_Absyn_B4){
goto _LL3307;} else{ goto _LL3308;} _LL3308: if( _temp3300 == ( void*) Cyc_Absyn_B8){
goto _LL3309;} else{ goto _LL3301;} _LL3303: if( w >  8){({ void* _temp3310[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp3310, sizeof( void*), 0u));});} goto
_LL3301; _LL3305: if( w >  16){({ void* _temp3311[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp3311, sizeof( void*), 0u));});} goto _LL3301; _LL3307: if( w >  32){({ void*
_temp3312[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp3312, sizeof( void*), 0u));});}
goto _LL3301; _LL3309: if( w >  64){({ void* _temp3313[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp3313, sizeof( void*), 0u));});} goto _LL3301; _LL3301:;} goto
_LL3293; _LL3297:({ struct Cyc_Std_String_pa_struct _temp3316; _temp3316.tag=
Cyc_Std_String_pa; _temp3316.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp3315; _temp3315.tag= Cyc_Std_String_pa;
_temp3315.f1=( struct _tagged_arr)* fn;{ void* _temp3314[ 2u]={& _temp3315,&
_temp3316}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3314, sizeof( void*), 2u));}}});
goto _LL3293; _LL3293:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts= atts->tl){ void* _temp3317=( void*) atts->hd; _LL3319: if( _temp3317 == (
void*) Cyc_Absyn_Packed_att){ goto _LL3320;} else{ goto _LL3321;} _LL3321: if((
unsigned int) _temp3317 >  16u?*(( int*) _temp3317) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL3322;} else{ goto _LL3323;} _LL3323: goto _LL3324; _LL3320: continue;
_LL3322: continue; _LL3324:({ struct Cyc_Std_String_pa_struct _temp3327;
_temp3327.tag= Cyc_Std_String_pa; _temp3327.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp3326; _temp3326.tag= Cyc_Std_String_pa;
_temp3326.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp3325[ 2u]={& _temp3326,& _temp3327}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp3325, sizeof( void*), 2u));}}});
goto _LL3318; _LL3318:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; struct _tuple17{ void* f1; int f2; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp3328= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3378; struct Cyc_Core_Opt** _temp3380; struct Cyc_Core_Opt* _temp3381;
struct Cyc_Core_Opt** _temp3383; struct Cyc_Absyn_Tvar* _temp3384; struct Cyc_List_List*
_temp3386; struct Cyc_Absyn_Enumdecl* _temp3388; struct Cyc_Absyn_Enumdecl**
_temp3390; struct _tuple1* _temp3391; struct Cyc_Absyn_TunionInfo _temp3393;
void* _temp3395; struct Cyc_List_List* _temp3397; void* _temp3399; void**
_temp3401; struct Cyc_Absyn_TunionFieldInfo _temp3402; struct Cyc_List_List*
_temp3404; void* _temp3406; void** _temp3408; struct Cyc_Absyn_PtrInfo _temp3409;
struct Cyc_Absyn_Conref* _temp3411; struct Cyc_Absyn_Tqual _temp3413; struct Cyc_Absyn_Conref*
_temp3415; void* _temp3417; void* _temp3419; struct Cyc_Absyn_Exp* _temp3421;
struct Cyc_Absyn_Tqual _temp3423; void* _temp3425; struct Cyc_Absyn_FnInfo
_temp3427; struct Cyc_List_List* _temp3429; struct Cyc_List_List* _temp3431;
struct Cyc_Absyn_VarargInfo* _temp3433; int _temp3435; struct Cyc_List_List*
_temp3437; void* _temp3439; struct Cyc_Core_Opt* _temp3441; struct Cyc_Core_Opt**
_temp3443; struct Cyc_List_List* _temp3444; struct Cyc_List_List** _temp3446;
struct Cyc_List_List* _temp3447; struct Cyc_List_List* _temp3449; struct Cyc_List_List*
_temp3451; struct Cyc_Absyn_Structdecl** _temp3453; struct Cyc_Absyn_Structdecl***
_temp3455; struct Cyc_List_List* _temp3456; struct _tuple1* _temp3458; struct
Cyc_Absyn_Uniondecl** _temp3460; struct Cyc_Absyn_Uniondecl*** _temp3462; struct
Cyc_List_List* _temp3463; struct _tuple1* _temp3465; struct Cyc_Core_Opt*
_temp3467; struct Cyc_Core_Opt** _temp3469; struct Cyc_List_List* _temp3470;
struct Cyc_List_List** _temp3472; struct _tuple1* _temp3473; void* _temp3475;
void* _temp3477; void* _temp3479; struct Cyc_List_List* _temp3481; _LL3330: if(
_temp3328 == ( void*) Cyc_Absyn_VoidType){ goto _LL3331;} else{ goto _LL3332;}
_LL3332: if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_Evar:
0){ _LL3382: _temp3381=(( struct Cyc_Absyn_Evar_struct*) _temp3328)->f1;
_temp3383=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3328)->f1;
goto _LL3379; _LL3379: _temp3378=(( struct Cyc_Absyn_Evar_struct*) _temp3328)->f2;
_temp3380=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3328)->f2;
goto _LL3333;} else{ goto _LL3334;} _LL3334: if(( unsigned int) _temp3328 >  3u?*((
int*) _temp3328) ==  Cyc_Absyn_VarType: 0){ _LL3385: _temp3384=(( struct Cyc_Absyn_VarType_struct*)
_temp3328)->f1; goto _LL3335;} else{ goto _LL3336;} _LL3336: if(( unsigned int)
_temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_AnonEnumType: 0){ _LL3387:
_temp3386=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp3328)->f1; goto _LL3337;}
else{ goto _LL3338;} _LL3338: if(( unsigned int) _temp3328 >  3u?*(( int*)
_temp3328) ==  Cyc_Absyn_EnumType: 0){ _LL3392: _temp3391=(( struct Cyc_Absyn_EnumType_struct*)
_temp3328)->f1; goto _LL3389; _LL3389: _temp3388=(( struct Cyc_Absyn_EnumType_struct*)
_temp3328)->f2; _temp3390=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp3328)->f2; goto _LL3339;} else{ goto _LL3340;} _LL3340: if(( unsigned int)
_temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_TunionType: 0){ _LL3394:
_temp3393=(( struct Cyc_Absyn_TunionType_struct*) _temp3328)->f1; _LL3400:
_temp3399=( void*) _temp3393.tunion_info; _temp3401=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp3328)->f1).tunion_info; goto _LL3398; _LL3398: _temp3397= _temp3393.targs;
goto _LL3396; _LL3396: _temp3395=( void*) _temp3393.rgn; goto _LL3341;} else{
goto _LL3342;} _LL3342: if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328)
==  Cyc_Absyn_TunionFieldType: 0){ _LL3403: _temp3402=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp3328)->f1; _LL3407: _temp3406=( void*) _temp3402.field_info; _temp3408=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp3328)->f1).field_info;
goto _LL3405; _LL3405: _temp3404= _temp3402.targs; goto _LL3343;} else{ goto
_LL3344;} _LL3344: if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328) == 
Cyc_Absyn_PointerType: 0){ _LL3410: _temp3409=(( struct Cyc_Absyn_PointerType_struct*)
_temp3328)->f1; _LL3420: _temp3419=( void*) _temp3409.elt_typ; goto _LL3418;
_LL3418: _temp3417=( void*) _temp3409.rgn_typ; goto _LL3416; _LL3416: _temp3415=
_temp3409.nullable; goto _LL3414; _LL3414: _temp3413= _temp3409.tq; goto _LL3412;
_LL3412: _temp3411= _temp3409.bounds; goto _LL3345;} else{ goto _LL3346;}
_LL3346: if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_IntType:
0){ goto _LL3347;} else{ goto _LL3348;} _LL3348: if( _temp3328 == ( void*) Cyc_Absyn_FloatType){
goto _LL3349;} else{ goto _LL3350;} _LL3350: if(( unsigned int) _temp3328 >  3u?*((
int*) _temp3328) ==  Cyc_Absyn_DoubleType: 0){ goto _LL3351;} else{ goto _LL3352;}
_LL3352: if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_ArrayType:
0){ _LL3426: _temp3425=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3328)->f1;
goto _LL3424; _LL3424: _temp3423=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3328)->f2; goto _LL3422; _LL3422: _temp3421=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3328)->f3; goto _LL3353;} else{ goto _LL3354;} _LL3354: if(( unsigned int)
_temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_FnType: 0){ _LL3428:
_temp3427=(( struct Cyc_Absyn_FnType_struct*) _temp3328)->f1; _LL3445: _temp3444=
_temp3427.tvars; _temp3446=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3328)->f1).tvars; goto _LL3442; _LL3442: _temp3441= _temp3427.effect;
_temp3443=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3328)->f1).effect; goto _LL3440; _LL3440: _temp3439=( void*) _temp3427.ret_typ;
goto _LL3438; _LL3438: _temp3437= _temp3427.args; goto _LL3436; _LL3436:
_temp3435= _temp3427.c_varargs; goto _LL3434; _LL3434: _temp3433= _temp3427.cyc_varargs;
goto _LL3432; _LL3432: _temp3431= _temp3427.rgn_po; goto _LL3430; _LL3430:
_temp3429= _temp3427.attributes; goto _LL3355;} else{ goto _LL3356;} _LL3356:
if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_TupleType:
0){ _LL3448: _temp3447=(( struct Cyc_Absyn_TupleType_struct*) _temp3328)->f1;
goto _LL3357;} else{ goto _LL3358;} _LL3358: if(( unsigned int) _temp3328 >  3u?*((
int*) _temp3328) ==  Cyc_Absyn_AnonStructType: 0){ _LL3450: _temp3449=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3328)->f1; goto _LL3359;} else{ goto
_LL3360;} _LL3360: if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328) == 
Cyc_Absyn_AnonUnionType: 0){ _LL3452: _temp3451=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3328)->f1; goto _LL3361;} else{ goto _LL3362;} _LL3362: if(( unsigned int)
_temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_StructType: 0){ _LL3459:
_temp3458=(( struct Cyc_Absyn_StructType_struct*) _temp3328)->f1; goto _LL3457;
_LL3457: _temp3456=(( struct Cyc_Absyn_StructType_struct*) _temp3328)->f2; goto
_LL3454; _LL3454: _temp3453=(( struct Cyc_Absyn_StructType_struct*) _temp3328)->f3;
_temp3455=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp3328)->f3; goto _LL3363;} else{ goto _LL3364;} _LL3364: if(( unsigned int)
_temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_UnionType: 0){ _LL3466:
_temp3465=(( struct Cyc_Absyn_UnionType_struct*) _temp3328)->f1; goto _LL3464;
_LL3464: _temp3463=(( struct Cyc_Absyn_UnionType_struct*) _temp3328)->f2; goto
_LL3461; _LL3461: _temp3460=(( struct Cyc_Absyn_UnionType_struct*) _temp3328)->f3;
_temp3462=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp3328)->f3; goto _LL3365;} else{ goto _LL3366;} _LL3366: if(( unsigned int)
_temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_TypedefType: 0){ _LL3474:
_temp3473=(( struct Cyc_Absyn_TypedefType_struct*) _temp3328)->f1; goto _LL3471;
_LL3471: _temp3470=(( struct Cyc_Absyn_TypedefType_struct*) _temp3328)->f2;
_temp3472=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3328)->f2; goto _LL3468; _LL3468: _temp3467=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3328)->f3; _temp3469=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3328)->f3; goto _LL3367;} else{ goto _LL3368;} _LL3368: if( _temp3328 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL3369;} else{ goto _LL3370;} _LL3370: if((
unsigned int) _temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_RgnHandleType:
0){ _LL3476: _temp3475=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp3328)->f1; goto _LL3371;} else{ goto _LL3372;} _LL3372: if(( unsigned int)
_temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_AccessEff: 0){ _LL3478:
_temp3477=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp3328)->f1; goto
_LL3373;} else{ goto _LL3374;} _LL3374: if(( unsigned int) _temp3328 >  3u?*((
int*) _temp3328) ==  Cyc_Absyn_RgnsEff: 0){ _LL3480: _temp3479=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp3328)->f1; goto _LL3375;} else{ goto _LL3376;}
_LL3376: if(( unsigned int) _temp3328 >  3u?*(( int*) _temp3328) ==  Cyc_Absyn_JoinEff:
0){ _LL3482: _temp3481=(( struct Cyc_Absyn_JoinEff_struct*) _temp3328)->f1; goto
_LL3377;} else{ goto _LL3329;} _LL3331: goto _LL3329; _LL3333: if(* _temp3383 == 
0){* _temp3383=({ struct Cyc_Core_Opt* _temp3483=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3483->v=( void*) expected_kind;
_temp3483;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp3380=({ struct Cyc_Core_Opt* _temp3484=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3484->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp3484;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp3485= Cyc_Tcutil_new_tvar(( void*)({ struct Cyc_Absyn_Less_kb_struct*
_temp3489=( struct Cyc_Absyn_Less_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct));
_temp3489[ 0]=({ struct Cyc_Absyn_Less_kb_struct _temp3490; _temp3490.tag= Cyc_Absyn_Less_kb;
_temp3490.f1= 0; _temp3490.f2=( void*) expected_kind; _temp3490;}); _temp3489;}));*
_temp3380=({ struct Cyc_Core_Opt* _temp3486=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3486->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3487=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3487[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3488; _temp3488.tag= Cyc_Absyn_VarType;
_temp3488.f1= _temp3485; _temp3488;}); _temp3487;})); _temp3486;}); _temp3384=
_temp3485; goto _LL3335;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL3329; _LL3335:{ void* _temp3491= Cyc_Absyn_compress_kb((
void*) _temp3384->kind); struct Cyc_Core_Opt* _temp3497; struct Cyc_Core_Opt**
_temp3499; _LL3493: if(*(( int*) _temp3491) ==  Cyc_Absyn_Unknown_kb){ _LL3498:
_temp3497=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp3491)->f1; _temp3499=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*) _temp3491)->f1;
goto _LL3494;} else{ goto _LL3495;} _LL3495: goto _LL3496; _LL3494:* _temp3499=({
struct Cyc_Core_Opt* _temp3500=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3500->v=( void*)(( void*)({ struct Cyc_Absyn_Less_kb_struct*
_temp3501=( struct Cyc_Absyn_Less_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct));
_temp3501[ 0]=({ struct Cyc_Absyn_Less_kb_struct _temp3502; _temp3502.tag= Cyc_Absyn_Less_kb;
_temp3502.f1= 0; _temp3502.f2=( void*) expected_kind; _temp3502;}); _temp3501;}));
_temp3500;}); goto _LL3492; _LL3496: goto _LL3492; _LL3492:;} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp3384); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp3384); goto _LL3329;
_LL3337: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);{ struct
_RegionHandle _temp3503= _new_region("uprev_rgn"); struct _RegionHandle*
uprev_rgn=& _temp3503; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0; for( 0; _temp3386 !=  0; _temp3386=
_temp3386->tl){ struct Cyc_Absyn_Enumfield* _temp3504=( struct Cyc_Absyn_Enumfield*)
_temp3386->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_zstrptrcmp, prev_fields,(* _temp3504->name).f2)){({ struct Cyc_Std_String_pa_struct
_temp3506; _temp3506.tag= Cyc_Std_String_pa; _temp3506.f1=( struct _tagged_arr)*(*
_temp3504->name).f2;{ void* _temp3505[ 1u]={& _temp3506}; Cyc_Tcutil_terr(
_temp3504->loc, _tag_arr("duplicate enum field name %s", sizeof( unsigned char),
29u), _tag_arr( _temp3505, sizeof( void*), 1u));}});} else{ prev_fields=({
struct Cyc_List_List* _temp3507=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp3507->hd=( void*)(* _temp3504->name).f2;
_temp3507->tl= prev_fields; _temp3507;});} if( _temp3504->tag ==  0){ _temp3504->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp3504->loc);} else{
if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null(
_temp3504->tag))){({ struct Cyc_Std_String_pa_struct _temp3509; _temp3509.tag=
Cyc_Std_String_pa; _temp3509.f1=( struct _tagged_arr)*(* _temp3504->name).f2;{
void* _temp3508[ 1u]={& _temp3509}; Cyc_Tcutil_terr( loc, _tag_arr("enum field %s: expression is not constant",
sizeof( unsigned char), 42u), _tag_arr( _temp3508, sizeof( void*), 1u));}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp3504->tag)); tag_count= t1 +  1;(* _temp3504->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp3510=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp3510[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp3511; _temp3511.tag= Cyc_Absyn_Abs_n; _temp3511.f1= te->ns; _temp3511;});
_temp3510;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple17* v)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp3504->name).f2,({ struct _tuple17* _temp3512=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp3512->f1=( void*)({ struct Cyc_Tcenv_AnonEnumRes_struct*
_temp3513=( struct Cyc_Tcenv_AnonEnumRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_AnonEnumRes_struct));
_temp3513[ 0]=({ struct Cyc_Tcenv_AnonEnumRes_struct _temp3514; _temp3514.tag=
Cyc_Tcenv_AnonEnumRes; _temp3514.f1=( void*) t; _temp3514.f2= _temp3504;
_temp3514;}); _temp3513;}); _temp3512->f2= 1; _temp3512;}));}}}; _pop_region(
uprev_rgn);} goto _LL3329;} _LL3339: if(* _temp3390 ==  0){ struct _handler_cons
_temp3515; _push_handler(& _temp3515);{ int _temp3517= 0; if( setjmp( _temp3515.handler)){
_temp3517= 1;} if( ! _temp3517){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3391);* _temp3390=( struct Cyc_Absyn_Enumdecl*)* ed;};
_pop_handler();} else{ void* _temp3516=( void*) _exn_thrown; void* _temp3519=
_temp3516; _LL3521: if( _temp3519 ==  Cyc_Dict_Absent){ goto _LL3522;} else{
goto _LL3523;} _LL3523: goto _LL3524; _LL3522: { struct Cyc_Tcenv_Genv*
_temp3525=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Enumdecl* _temp3526=({
struct Cyc_Absyn_Enumdecl* _temp3527=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp3527->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp3527->name= _temp3391; _temp3527->fields= 0; _temp3527;}); Cyc_Tc_tcEnumdecl(
te, _temp3525, loc, _temp3526);{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3391);* _temp3390=( struct Cyc_Absyn_Enumdecl*)* ed; goto _LL3520;}}
_LL3524:( void) _throw( _temp3519); _LL3520:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp3390);* _temp3391=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL3329;} _LL3341:{ void*
_temp3528=* _temp3401; struct Cyc_Absyn_UnknownTunionInfo _temp3534; int
_temp3536; struct _tuple1* _temp3538; struct Cyc_Absyn_Tuniondecl** _temp3540;
struct Cyc_Absyn_Tuniondecl* _temp3542; _LL3530: if(*(( int*) _temp3528) ==  Cyc_Absyn_UnknownTunion){
_LL3535: _temp3534=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp3528)->f1;
_LL3539: _temp3538= _temp3534.name; goto _LL3537; _LL3537: _temp3536= _temp3534.is_xtunion;
goto _LL3531;} else{ goto _LL3532;} _LL3532: if(*(( int*) _temp3528) ==  Cyc_Absyn_KnownTunion){
_LL3541: _temp3540=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3528)->f1;
_temp3542=* _temp3540; goto _LL3533;} else{ goto _LL3529;} _LL3531: { struct Cyc_Absyn_Tuniondecl**
tudp;{ struct _handler_cons _temp3543; _push_handler(& _temp3543);{ int
_temp3545= 0; if( setjmp( _temp3543.handler)){ _temp3545= 1;} if( ! _temp3545){
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp3538);; _pop_handler();} else{
void* _temp3544=( void*) _exn_thrown; void* _temp3547= _temp3544; _LL3549: if(
_temp3547 ==  Cyc_Dict_Absent){ goto _LL3550;} else{ goto _LL3551;} _LL3551:
goto _LL3552; _LL3550: { struct Cyc_Tcenv_Genv* _temp3553=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Tuniondecl* _temp3554=({ struct Cyc_Absyn_Tuniondecl* _temp3558=(
struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp3558->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3558->name= _temp3538;
_temp3558->tvs= 0; _temp3558->fields= 0; _temp3558->is_xtunion= _temp3536;
_temp3558;}); Cyc_Tc_tcTuniondecl( te, _temp3553, loc, _temp3554); tudp= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp3538); if( _temp3397 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3557; _temp3557.tag= Cyc_Std_String_pa; _temp3557.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3538);{ struct Cyc_Std_String_pa_struct _temp3556;
_temp3556.tag= Cyc_Std_String_pa; _temp3556.f1=( struct _tagged_arr)( _temp3536?
_tag_arr("xtunion", sizeof( unsigned char), 8u): _tag_arr("tunion", sizeof(
unsigned char), 7u));{ void* _temp3555[ 2u]={& _temp3556,& _temp3557}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized %s %s before using", sizeof(
unsigned char), 48u), _tag_arr( _temp3555, sizeof( void*), 2u));}}}); return
cvtenv;} goto _LL3548;} _LL3552:( void) _throw( _temp3547); _LL3548:;}}} if((*
tudp)->is_xtunion !=  _temp3536){({ struct Cyc_Std_String_pa_struct _temp3560;
_temp3560.tag= Cyc_Std_String_pa; _temp3560.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3538);{ void* _temp3559[ 1u]={& _temp3560}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp3559, sizeof( void*), 1u));}});}*
_temp3401=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp3561=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp3561[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp3562; _temp3562.tag= Cyc_Absyn_KnownTunion;
_temp3562.f1= tudp; _temp3562;}); _temp3561;}); _temp3542=* tudp; goto _LL3533;}
_LL3533: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3395);{ struct Cyc_List_List* tvs= _temp3542->tvs; for( 0; _temp3397 !=  0?
tvs !=  0: 0;( _temp3397= _temp3397->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3397->hd; void* k1= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) tvs->hd);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp3397
!=  0){({ struct Cyc_Std_String_pa_struct _temp3564; _temp3564.tag= Cyc_Std_String_pa;
_temp3564.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3542->name);{
void* _temp3563[ 1u]={& _temp3564}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp3563, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp3566; _temp3566.tag= Cyc_Std_String_pa;
_temp3566.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3542->name);{
void* _temp3565[ 1u]={& _temp3566}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp3565, sizeof( void*), 1u));}});}
goto _LL3529;} _LL3529:;} goto _LL3329; _LL3343:{ void* _temp3567=* _temp3408;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp3573; int _temp3575; struct _tuple1*
_temp3577; struct _tuple1* _temp3579; struct Cyc_Absyn_Tunionfield* _temp3581;
struct Cyc_Absyn_Tuniondecl* _temp3583; _LL3569: if(*(( int*) _temp3567) ==  Cyc_Absyn_UnknownTunionfield){
_LL3574: _temp3573=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp3567)->f1;
_LL3580: _temp3579= _temp3573.tunion_name; goto _LL3578; _LL3578: _temp3577=
_temp3573.field_name; goto _LL3576; _LL3576: _temp3575= _temp3573.is_xtunion;
goto _LL3570;} else{ goto _LL3571;} _LL3571: if(*(( int*) _temp3567) ==  Cyc_Absyn_KnownTunionfield){
_LL3584: _temp3583=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp3567)->f1;
goto _LL3582; _LL3582: _temp3581=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp3567)->f2; goto _LL3572;} else{ goto _LL3568;} _LL3570: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp3585;
_push_handler(& _temp3585);{ int _temp3587= 0; if( setjmp( _temp3585.handler)){
_temp3587= 1;} if( ! _temp3587){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp3579);; _pop_handler();} else{ void* _temp3586=( void*) _exn_thrown; void*
_temp3589= _temp3586; _LL3591: if( _temp3589 ==  Cyc_Dict_Absent){ goto _LL3592;}
else{ goto _LL3593;} _LL3593: goto _LL3594; _LL3592:({ struct Cyc_Std_String_pa_struct
_temp3596; _temp3596.tag= Cyc_Std_String_pa; _temp3596.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3579);{ void* _temp3595[ 1u]={& _temp3596}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp3595, sizeof( void*), 1u));}}); return cvtenv; _LL3594:( void) _throw(
_temp3589); _LL3590:;}}}{ struct _handler_cons _temp3597; _push_handler(&
_temp3597);{ int _temp3599= 0; if( setjmp( _temp3597.handler)){ _temp3599= 1;}
if( ! _temp3599){{ void* _temp3600= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp3577); struct Cyc_Absyn_Tunionfield* _temp3606; struct Cyc_Absyn_Tuniondecl*
_temp3608; _LL3602: if(*(( int*) _temp3600) ==  Cyc_Tcenv_TunionRes){ _LL3609:
_temp3608=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3600)->f1; goto _LL3607;
_LL3607: _temp3606=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3600)->f2; goto
_LL3603;} else{ goto _LL3604;} _LL3604: goto _LL3605; _LL3603: tuf= _temp3606;
tud= _temp3608; if( tud->is_xtunion !=  _temp3575){({ struct Cyc_Std_String_pa_struct
_temp3611; _temp3611.tag= Cyc_Std_String_pa; _temp3611.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3579);{ void* _temp3610[ 1u]={& _temp3611}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp3610, sizeof( void*), 1u));}});} goto
_LL3601; _LL3605:({ struct Cyc_Std_String_pa_struct _temp3614; _temp3614.tag=
Cyc_Std_String_pa; _temp3614.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3579);{ struct Cyc_Std_String_pa_struct _temp3613; _temp3613.tag= Cyc_Std_String_pa;
_temp3613.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3577);{ void*
_temp3612[ 2u]={& _temp3613,& _temp3614}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp3612, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp3615= cvtenv; _npop_handler( 0u); return _temp3615;}
_LL3601:;}; _pop_handler();} else{ void* _temp3598=( void*) _exn_thrown; void*
_temp3617= _temp3598; _LL3619: if( _temp3617 ==  Cyc_Dict_Absent){ goto _LL3620;}
else{ goto _LL3621;} _LL3621: goto _LL3622; _LL3620:({ struct Cyc_Std_String_pa_struct
_temp3625; _temp3625.tag= Cyc_Std_String_pa; _temp3625.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3579);{ struct Cyc_Std_String_pa_struct _temp3624;
_temp3624.tag= Cyc_Std_String_pa; _temp3624.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3577);{ void* _temp3623[ 2u]={& _temp3624,& _temp3625}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3623, sizeof( void*), 2u));}}}); return cvtenv; _LL3622:( void)
_throw( _temp3617); _LL3618:;}}}* _temp3408=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp3626=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp3626[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp3627; _temp3627.tag= Cyc_Absyn_KnownTunionfield; _temp3627.f1= tud;
_temp3627.f2= tuf; _temp3627;}); _temp3626;}); _temp3583= tud; _temp3581= tuf;
goto _LL3572;} _LL3572: { struct Cyc_List_List* tvs= _temp3583->tvs; for( 0;
_temp3404 !=  0? tvs !=  0: 0;( _temp3404= _temp3404->tl, tvs= tvs->tl)){ void*
t1=( void*) _temp3404->hd; void* k1= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*)
tvs->hd); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if(
_temp3404 !=  0){({ struct Cyc_Std_String_pa_struct _temp3630; _temp3630.tag=
Cyc_Std_String_pa; _temp3630.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3581->name);{ struct Cyc_Std_String_pa_struct _temp3629; _temp3629.tag= Cyc_Std_String_pa;
_temp3629.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3583->name);{
void* _temp3628[ 2u]={& _temp3629,& _temp3630}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp3628, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp3633; _temp3633.tag= Cyc_Std_String_pa;
_temp3633.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3581->name);{
struct Cyc_Std_String_pa_struct _temp3632; _temp3632.tag= Cyc_Std_String_pa;
_temp3632.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3583->name);{
void* _temp3631[ 2u]={& _temp3632,& _temp3633}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp3631, sizeof( void*), 2u));}}});}
goto _LL3568;} _LL3568:;} goto _LL3329; _LL3345: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3419); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp3417);{ void* _temp3634=( void*)(
Cyc_Absyn_compress_conref( _temp3411))->v; void* _temp3640; struct Cyc_Absyn_Exp*
_temp3642; _LL3636: if(( unsigned int) _temp3634 >  1u?*(( int*) _temp3634) == 
Cyc_Absyn_Eq_constr: 0){ _LL3641: _temp3640=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3634)->f1; if(( unsigned int) _temp3640 >  1u?*(( int*) _temp3640) ==  Cyc_Absyn_Upper_b:
0){ _LL3643: _temp3642=(( struct Cyc_Absyn_Upper_b_struct*) _temp3640)->f1; goto
_LL3637;} else{ goto _LL3638;}} else{ goto _LL3638;} _LL3638: goto _LL3639;
_LL3637: if( ! Cyc_Tcutil_is_const_exp( te, _temp3642)){({ void* _temp3644[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp3644, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp3642); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp3642)){({ void*
_temp3645[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp3645, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp3642); goto _LL3635; _LL3639: goto _LL3635; _LL3635:;} goto _LL3329;
_LL3347: goto _LL3329; _LL3349: goto _LL3329; _LL3351: goto _LL3329; _LL3353:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3425); if( _temp3421 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp3421))){({ void* _temp3646[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp3646, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp3421)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp3421))){({ void* _temp3647[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp3647, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp3421)); goto _LL3329; _LL3355: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp3429 !=  0; _temp3429= _temp3429->tl){ if(
! Cyc_Absyn_fntype_att(( void*) _temp3429->hd)){({ struct Cyc_Std_String_pa_struct
_temp3649; _temp3649.tag= Cyc_Std_String_pa; _temp3649.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*) _temp3429->hd);{ void* _temp3648[ 1u]={&
_temp3649}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp3648, sizeof( void*), 1u));}});}{
void* _temp3650=( void*) _temp3429->hd; int _temp3662; int _temp3664; void*
_temp3666; _LL3652: if( _temp3650 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL3653;} else{ goto _LL3654;} _LL3654: if( _temp3650 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL3655;} else{ goto _LL3656;} _LL3656: if( _temp3650 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL3657;} else{ goto _LL3658;} _LL3658: if(( unsigned int) _temp3650 >  16u?*((
int*) _temp3650) ==  Cyc_Absyn_Format_att: 0){ _LL3667: _temp3666=( void*)((
struct Cyc_Absyn_Format_att_struct*) _temp3650)->f1; goto _LL3665; _LL3665:
_temp3664=(( struct Cyc_Absyn_Format_att_struct*) _temp3650)->f2; goto _LL3663;
_LL3663: _temp3662=(( struct Cyc_Absyn_Format_att_struct*) _temp3650)->f3; goto
_LL3659;} else{ goto _LL3660;} _LL3660: goto _LL3661; _LL3653: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL3651; _LL3655: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL3651; _LL3657: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL3651; _LL3659: if( !
seen_format){ seen_format= 1; ft= _temp3666; fmt_desc_arg= _temp3664;
fmt_arg_start= _temp3662;} else{({ void* _temp3668[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp3668, sizeof( void*), 0u));});} goto _LL3651; _LL3661: goto
_LL3651; _LL3651:;}} if( num_convs >  1){({ void* _temp3669[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp3669, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp3446);{ struct Cyc_List_List* b=* _temp3446; for( 0; b !=  0; b= b->tl){((
struct Cyc_Absyn_Tvar*) b->hd)->identity= Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env=
Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*) b->hd);{
void* _temp3670= Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*) b->hd)->kind);
void* _temp3676; _LL3672: if(*(( int*) _temp3670) ==  Cyc_Absyn_Eq_kb){ _LL3677:
_temp3676=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp3670)->f1; if(
_temp3676 == ( void*) Cyc_Absyn_MemKind){ goto _LL3673;} else{ goto _LL3674;}}
else{ goto _LL3674;} _LL3674: goto _LL3675; _LL3673:({ struct Cyc_Std_String_pa_struct
_temp3679; _temp3679.tag= Cyc_Std_String_pa; _temp3679.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*) b->hd)->name;{ void* _temp3678[ 1u]={& _temp3679}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp3678, sizeof( void*), 1u));}}); goto
_LL3671; _LL3675: goto _LL3671; _LL3671:;}}}{ struct Cyc_Tcutil_CVTEnv _temp3680=({
struct Cyc_Tcutil_CVTEnv _temp3895; _temp3895.kind_env= cvtenv.kind_env;
_temp3895.free_vars= 0; _temp3895.free_evars= 0; _temp3895.generalize_evars=
cvtenv.generalize_evars; _temp3895.fn_result= 1; _temp3895;}); _temp3680= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3680,( void*) Cyc_Absyn_MemKind, _temp3439); _temp3680.fn_result=
0;{ struct Cyc_List_List* a= _temp3437; for( 0; a !=  0; a= a->tl){ _temp3680=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3680,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*) a->hd)).f3);}} if( _temp3433 !=  0){ if( _temp3435){({ void*
_temp3681[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp3681, sizeof( void*), 0u));});}{ int _temp3684; void* _temp3686; struct Cyc_Absyn_Tqual
_temp3688; struct Cyc_Core_Opt* _temp3690; struct Cyc_Absyn_VarargInfo _temp3682=*
_temp3433; _LL3691: _temp3690= _temp3682.name; goto _LL3689; _LL3689: _temp3688=
_temp3682.tq; goto _LL3687; _LL3687: _temp3686=( void*) _temp3682.type; goto
_LL3685; _LL3685: _temp3684= _temp3682.inject; goto _LL3683; _LL3683: _temp3680=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3680,( void*) Cyc_Absyn_MemKind,
_temp3686); if( _temp3684){ void* _temp3692= Cyc_Tcutil_compress( _temp3686);
struct Cyc_Absyn_TunionInfo _temp3698; void* _temp3700; void* _temp3702; _LL3694:
if(( unsigned int) _temp3692 >  3u?*(( int*) _temp3692) ==  Cyc_Absyn_TunionType:
0){ _LL3699: _temp3698=(( struct Cyc_Absyn_TunionType_struct*) _temp3692)->f1;
_LL3703: _temp3702=( void*) _temp3698.tunion_info; if(*(( int*) _temp3702) == 
Cyc_Absyn_KnownTunion){ goto _LL3701;} else{ goto _LL3696;} _LL3701: _temp3700=(
void*) _temp3698.rgn; goto _LL3695;} else{ goto _LL3696;} _LL3696: goto _LL3697;
_LL3695: goto _LL3693; _LL3697:({ void* _temp3704[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("can't inject a non-[x]tunion type", sizeof( unsigned char), 34u),
_tag_arr( _temp3704, sizeof( void*), 0u));}); goto _LL3693; _LL3693:;}}} if(
seen_format){ int _temp3705=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp3437); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg >  _temp3705)? 1:
fmt_arg_start <  0)? 1:( _temp3433 ==  0? fmt_arg_start !=  0: 0))? 1:(
_temp3433 !=  0? fmt_arg_start !=  _temp3705 +  1: 0)){({ void* _temp3706[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp3706, sizeof( void*), 0u));});} else{ void* _temp3709;
struct _tuple2 _temp3707=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp3437, fmt_desc_arg -  1); _LL3710: _temp3709= _temp3707.f3;
goto _LL3708; _LL3708:{ void* _temp3711= Cyc_Tcutil_compress( _temp3709); struct
Cyc_Absyn_PtrInfo _temp3717; struct Cyc_Absyn_Conref* _temp3719; void* _temp3721;
_LL3713: if(( unsigned int) _temp3711 >  3u?*(( int*) _temp3711) ==  Cyc_Absyn_PointerType:
0){ _LL3718: _temp3717=(( struct Cyc_Absyn_PointerType_struct*) _temp3711)->f1;
_LL3722: _temp3721=( void*) _temp3717.elt_typ; goto _LL3720; _LL3720: _temp3719=
_temp3717.bounds; goto _LL3714;} else{ goto _LL3715;} _LL3715: goto _LL3716;
_LL3714:{ struct _tuple8 _temp3724=({ struct _tuple8 _temp3723; _temp3723.f1=
Cyc_Tcutil_compress( _temp3721); _temp3723.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp3719); _temp3723;}); void* _temp3730; void* _temp3732; void* _temp3734;
void* _temp3736; _LL3726: _LL3733: _temp3732= _temp3724.f1; if(( unsigned int)
_temp3732 >  3u?*(( int*) _temp3732) ==  Cyc_Absyn_IntType: 0){ _LL3737:
_temp3736=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3732)->f1; if(
_temp3736 == ( void*) Cyc_Absyn_Unsigned){ goto _LL3735;} else{ goto _LL3728;}
_LL3735: _temp3734=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3732)->f2;
if( _temp3734 == ( void*) Cyc_Absyn_B1){ goto _LL3731;} else{ goto _LL3728;}}
else{ goto _LL3728;} _LL3731: _temp3730= _temp3724.f2; if( _temp3730 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL3727;} else{ goto _LL3728;} _LL3728: goto _LL3729;
_LL3727: goto _LL3725; _LL3729:({ void* _temp3738[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp3738, sizeof( void*), 0u));}); goto _LL3725; _LL3725:;} goto
_LL3712; _LL3716:({ void* _temp3739[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp3739, sizeof( void*), 0u));}); goto
_LL3712; _LL3712:;} if( fmt_arg_start !=  0){ void* _temp3740= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp3433))->type); int
problem;{ void* _temp3741= ft; _LL3743: if( _temp3741 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL3744;} else{ goto _LL3745;} _LL3745: if( _temp3741 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL3746;} else{ goto _LL3742;} _LL3744:{ void* _temp3747= _temp3740; struct
Cyc_Absyn_TunionInfo _temp3753; void* _temp3755; struct Cyc_Absyn_Tuniondecl**
_temp3757; struct Cyc_Absyn_Tuniondecl* _temp3759; _LL3749: if(( unsigned int)
_temp3747 >  3u?*(( int*) _temp3747) ==  Cyc_Absyn_TunionType: 0){ _LL3754:
_temp3753=(( struct Cyc_Absyn_TunionType_struct*) _temp3747)->f1; _LL3756:
_temp3755=( void*) _temp3753.tunion_info; if(*(( int*) _temp3755) ==  Cyc_Absyn_KnownTunion){
_LL3758: _temp3757=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3755)->f1;
_temp3759=* _temp3757; goto _LL3750;} else{ goto _LL3751;}} else{ goto _LL3751;}
_LL3751: goto _LL3752; _LL3750: problem= Cyc_Absyn_qvar_cmp( _temp3759->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL3748; _LL3752: problem= 1; goto
_LL3748; _LL3748:;} goto _LL3742; _LL3746:{ void* _temp3760= _temp3740; struct
Cyc_Absyn_TunionInfo _temp3766; void* _temp3768; struct Cyc_Absyn_Tuniondecl**
_temp3770; struct Cyc_Absyn_Tuniondecl* _temp3772; _LL3762: if(( unsigned int)
_temp3760 >  3u?*(( int*) _temp3760) ==  Cyc_Absyn_TunionType: 0){ _LL3767:
_temp3766=(( struct Cyc_Absyn_TunionType_struct*) _temp3760)->f1; _LL3769:
_temp3768=( void*) _temp3766.tunion_info; if(*(( int*) _temp3768) ==  Cyc_Absyn_KnownTunion){
_LL3771: _temp3770=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3768)->f1;
_temp3772=* _temp3770; goto _LL3763;} else{ goto _LL3764;}} else{ goto _LL3764;}
_LL3764: goto _LL3765; _LL3763: problem= Cyc_Absyn_qvar_cmp( _temp3772->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL3761; _LL3765: problem= 1; goto
_LL3761; _LL3761:;} goto _LL3742; _LL3742:;} if( problem){({ void* _temp3773[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3773, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp3431; for( 0; rpo !=  0; rpo= rpo->tl){ struct
_tuple8 _temp3776; void* _temp3777; void* _temp3779; struct _tuple8* _temp3774=(
struct _tuple8*) rpo->hd; _temp3776=* _temp3774; _LL3780: _temp3779= _temp3776.f1;
goto _LL3778; _LL3778: _temp3777= _temp3776.f2; goto _LL3775; _LL3775: _temp3680=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3680,( void*) Cyc_Absyn_RgnKind,
_temp3779); _temp3680= Cyc_Tcutil_i_check_valid_type( loc, te, _temp3680,( void*)
Cyc_Absyn_RgnKind, _temp3777);}} if(* _temp3443 !=  0){ _temp3680= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3680,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp3443))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp3680.free_vars; for(
0; tvs !=  0; tvs= tvs->tl){ void* _temp3781= Cyc_Absyn_compress_kb(( void*)((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); void* _temp3795; struct Cyc_Core_Opt*
_temp3797; struct Cyc_Core_Opt** _temp3799; void* _temp3800; void* _temp3802;
struct Cyc_Core_Opt* _temp3804; struct Cyc_Core_Opt** _temp3806; void* _temp3807;
struct Cyc_Core_Opt* _temp3809; struct Cyc_Core_Opt** _temp3811; _LL3783: if(*((
int*) _temp3781) ==  Cyc_Absyn_Less_kb){ _LL3798: _temp3797=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3781)->f1; _temp3799=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3781)->f1; goto _LL3796; _LL3796: _temp3795=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3781)->f2; if( _temp3795 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3784;}
else{ goto _LL3785;}} else{ goto _LL3785;} _LL3785: if(*(( int*) _temp3781) == 
Cyc_Absyn_Eq_kb){ _LL3801: _temp3800=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp3781)->f1; if( _temp3800 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3786;}
else{ goto _LL3787;}} else{ goto _LL3787;} _LL3787: if(*(( int*) _temp3781) == 
Cyc_Absyn_Less_kb){ _LL3805: _temp3804=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3781)->f1; _temp3806=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3781)->f1; goto _LL3803; _LL3803: _temp3802=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3781)->f2; if( _temp3802 == ( void*) Cyc_Absyn_EffKind){ goto _LL3788;}
else{ goto _LL3789;}} else{ goto _LL3789;} _LL3789: if(*(( int*) _temp3781) == 
Cyc_Absyn_Eq_kb){ _LL3808: _temp3807=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp3781)->f1; if( _temp3807 == ( void*) Cyc_Absyn_EffKind){ goto _LL3790;}
else{ goto _LL3791;}} else{ goto _LL3791;} _LL3791: if(*(( int*) _temp3781) == 
Cyc_Absyn_Unknown_kb){ _LL3810: _temp3809=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp3781)->f1; _temp3811=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp3781)->f1; goto _LL3792;} else{ goto _LL3793;} _LL3793: goto _LL3794;
_LL3784:* _temp3799=({ struct Cyc_Core_Opt* _temp3812=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3812->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp3813=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp3813[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp3814; _temp3814.tag= Cyc_Absyn_Eq_kb; _temp3814.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp3814;}); _temp3813;})); _temp3812;}); goto _LL3786; _LL3786: effect=({
struct Cyc_List_List* _temp3815=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3815->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3816=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3816[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3817; _temp3817.tag= Cyc_Absyn_AccessEff;
_temp3817.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3818=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3818[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3819; _temp3819.tag= Cyc_Absyn_VarType;
_temp3819.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3819;}); _temp3818;}));
_temp3817;}); _temp3816;})); _temp3815->tl= effect; _temp3815;}); goto _LL3782;
_LL3788:* _temp3806=({ struct Cyc_Core_Opt* _temp3820=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3820->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp3821=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp3821[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp3822; _temp3822.tag= Cyc_Absyn_Eq_kb; _temp3822.f1=( void*)(( void*) Cyc_Absyn_EffKind);
_temp3822;}); _temp3821;})); _temp3820;}); goto _LL3790; _LL3790: effect=({
struct Cyc_List_List* _temp3823=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3823->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3824=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3824[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3825; _temp3825.tag= Cyc_Absyn_VarType;
_temp3825.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3825;}); _temp3824;}));
_temp3823->tl= effect; _temp3823;}); goto _LL3782; _LL3792:* _temp3811=({ struct
Cyc_Core_Opt* _temp3826=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3826->v=( void*)(( void*)({ struct Cyc_Absyn_Less_kb_struct* _temp3827=(
struct Cyc_Absyn_Less_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct));
_temp3827[ 0]=({ struct Cyc_Absyn_Less_kb_struct _temp3828; _temp3828.tag= Cyc_Absyn_Less_kb;
_temp3828.f1= 0; _temp3828.f2=( void*)(( void*) Cyc_Absyn_AnyKind); _temp3828;});
_temp3827;})); _temp3826;}); goto _LL3794; _LL3794: effect=({ struct Cyc_List_List*
_temp3829=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3829->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp3830=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3830[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3831; _temp3831.tag= Cyc_Absyn_RgnsEff;
_temp3831.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3832=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3832[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3833; _temp3833.tag= Cyc_Absyn_VarType;
_temp3833.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3833;}); _temp3832;}));
_temp3831;}); _temp3830;})); _temp3829->tl= effect; _temp3829;}); goto _LL3782;
_LL3782:;}} effect= Cyc_List_imp_rev( effect);{ struct Cyc_List_List* ts=
_temp3680.free_evars; for( 0; ts !=  0; ts= ts->tl){ void* _temp3834= Cyc_Tcutil_typ_kind((
void*) ts->hd); _LL3836: if( _temp3834 == ( void*) Cyc_Absyn_RgnKind){ goto
_LL3837;} else{ goto _LL3838;} _LL3838: if( _temp3834 == ( void*) Cyc_Absyn_EffKind){
goto _LL3839;} else{ goto _LL3840;} _LL3840: goto _LL3841; _LL3837: effect=({
struct Cyc_List_List* _temp3842=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3842->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3843=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3843[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3844; _temp3844.tag= Cyc_Absyn_AccessEff;
_temp3844.f1=( void*)(( void*) ts->hd); _temp3844;}); _temp3843;})); _temp3842->tl=
effect; _temp3842;}); goto _LL3835; _LL3839: effect=({ struct Cyc_List_List*
_temp3845=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3845->hd=( void*)(( void*) ts->hd); _temp3845->tl= effect; _temp3845;});
goto _LL3835; _LL3841: effect=({ struct Cyc_List_List* _temp3846=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3846->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3847=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3847[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3848; _temp3848.tag= Cyc_Absyn_RgnsEff; _temp3848.f1=(
void*)(( void*) ts->hd); _temp3848;}); _temp3847;})); _temp3846->tl= effect;
_temp3846;}); goto _LL3835; _LL3835:;}}* _temp3443=({ struct Cyc_Core_Opt*
_temp3849=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3849->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3850=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3850[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3851; _temp3851.tag= Cyc_Absyn_JoinEff;
_temp3851.f1= Cyc_List_imp_rev( effect); _temp3851;}); _temp3850;})); _temp3849;});}}
if(* _temp3446 !=  0){ struct Cyc_List_List* bs=* _temp3446; for( 0; bs !=  0;
bs= bs->tl){ void* _temp3852= Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*)
bs->hd)->kind); struct Cyc_Core_Opt* _temp3866; struct Cyc_Core_Opt** _temp3868;
void* _temp3869; struct Cyc_Core_Opt* _temp3871; struct Cyc_Core_Opt** _temp3873;
void* _temp3874; struct Cyc_Core_Opt* _temp3876; struct Cyc_Core_Opt** _temp3878;
void* _temp3879; struct Cyc_Core_Opt* _temp3881; struct Cyc_Core_Opt** _temp3883;
void* _temp3884; _LL3854: if(*(( int*) _temp3852) ==  Cyc_Absyn_Unknown_kb){
_LL3867: _temp3866=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp3852)->f1;
_temp3868=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp3852)->f1; goto _LL3855;} else{ goto _LL3856;} _LL3856: if(*(( int*)
_temp3852) ==  Cyc_Absyn_Less_kb){ _LL3872: _temp3871=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f1; _temp3873=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f1; goto _LL3870; _LL3870: _temp3869=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f2; if( _temp3869 == ( void*) Cyc_Absyn_MemKind){ goto _LL3857;}
else{ goto _LL3858;}} else{ goto _LL3858;} _LL3858: if(*(( int*) _temp3852) == 
Cyc_Absyn_Less_kb){ _LL3877: _temp3876=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f1; _temp3878=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f1; goto _LL3875; _LL3875: _temp3874=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f2; if( _temp3874 == ( void*) Cyc_Absyn_AnyKind){ goto _LL3859;}
else{ goto _LL3860;}} else{ goto _LL3860;} _LL3860: if(*(( int*) _temp3852) == 
Cyc_Absyn_Less_kb){ _LL3882: _temp3881=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f1; _temp3883=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f1; goto _LL3880; _LL3880: _temp3879=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3852)->f2; goto _LL3861;} else{ goto _LL3862;} _LL3862: if(*(( int*)
_temp3852) ==  Cyc_Absyn_Eq_kb){ _LL3885: _temp3884=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp3852)->f1; if( _temp3884 == ( void*) Cyc_Absyn_MemKind){ goto _LL3863;}
else{ goto _LL3864;}} else{ goto _LL3864;} _LL3864: goto _LL3865; _LL3855:({
struct Cyc_Std_String_pa_struct _temp3887; _temp3887.tag= Cyc_Std_String_pa;
_temp3887.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) bs->hd)->name;{
void* _temp3886[ 1u]={& _temp3887}; Cyc_Tcutil_warn( loc, _tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof( unsigned char), 47u), _tag_arr( _temp3886, sizeof( void*), 1u));}});
_temp3873= _temp3868; goto _LL3857; _LL3857: _temp3878= _temp3873; goto _LL3859;
_LL3859:* _temp3878=({ struct Cyc_Core_Opt* _temp3888=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3888->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp3889=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp3889[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp3890; _temp3890.tag= Cyc_Absyn_Eq_kb; _temp3890.f1=( void*)(( void*) Cyc_Absyn_BoxKind);
_temp3890;}); _temp3889;})); _temp3888;}); goto _LL3853; _LL3861:* _temp3883=({
struct Cyc_Core_Opt* _temp3891=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3891->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp3892=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp3892[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp3893; _temp3893.tag= Cyc_Absyn_Eq_kb;
_temp3893.f1=( void*) _temp3879; _temp3893;}); _temp3892;})); _temp3891;}); goto
_LL3853; _LL3863:({ void* _temp3894[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("function's can't abstract M types",
sizeof( unsigned char), 34u), _tag_arr( _temp3894, sizeof( void*), 0u));}); goto
_LL3853; _LL3865: goto _LL3853; _LL3853:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp3680.kind_env,* _temp3446); _temp3680.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp3680.free_vars,* _temp3446);{ struct Cyc_List_List* tvs= _temp3680.free_vars;
for( 0; tvs !=  0; tvs= tvs->tl){ cvtenv.free_vars= Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,( struct Cyc_Absyn_Tvar*) tvs->hd);}}{ struct Cyc_List_List*
evs= _temp3680.free_evars; for( 0; evs !=  0; evs= evs->tl){ cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars,( void*) evs->hd);}} goto _LL3329;}}
_LL3357: for( 0; _temp3447 !=  0; _temp3447= _temp3447->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp3447->hd)).f2);}
goto _LL3329; _LL3359:{ struct _RegionHandle _temp3896= _new_region("sprev_rgn");
struct _RegionHandle* sprev_rgn=& _temp3896; _push_region( sprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp3449 !=  0; _temp3449= _temp3449->tl){
struct Cyc_Absyn_Structfield _temp3899; struct Cyc_List_List* _temp3900; struct
Cyc_Absyn_Exp* _temp3902; void* _temp3904; struct Cyc_Absyn_Tqual _temp3906;
struct _tagged_arr* _temp3908; struct Cyc_Absyn_Structfield* _temp3897=( struct
Cyc_Absyn_Structfield*) _temp3449->hd; _temp3899=* _temp3897; _LL3909: _temp3908=
_temp3899.name; goto _LL3907; _LL3907: _temp3906= _temp3899.tq; goto _LL3905;
_LL3905: _temp3904=( void*) _temp3899.type; goto _LL3903; _LL3903: _temp3902=
_temp3899.width; goto _LL3901; _LL3901: _temp3900= _temp3899.attributes; goto
_LL3898; _LL3898: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_zstrptrcmp, prev_fields, _temp3908)){({ struct Cyc_Std_String_pa_struct
_temp3911; _temp3911.tag= Cyc_Std_String_pa; _temp3911.f1=( struct _tagged_arr)*
_temp3908;{ void* _temp3910[ 1u]={& _temp3911}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3910, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3908, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3912=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3912->hd=( void*)
_temp3908; _temp3912->tl= prev_fields; _temp3912;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3904); Cyc_Tcutil_check_bitfield(
loc, te, _temp3904, _temp3902, _temp3908); Cyc_Tcutil_check_field_atts( loc,
_temp3908, _temp3900);}}; _pop_region( sprev_rgn);} goto _LL3329; _LL3361:{
struct _RegionHandle _temp3913= _new_region("uprev_rgn"); struct _RegionHandle*
uprev_rgn=& _temp3913; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp3451 !=  0; _temp3451= _temp3451->tl){ struct Cyc_Absyn_Structfield
_temp3916; struct Cyc_List_List* _temp3917; struct Cyc_Absyn_Exp* _temp3919;
void* _temp3921; struct Cyc_Absyn_Tqual _temp3923; struct _tagged_arr* _temp3925;
struct Cyc_Absyn_Structfield* _temp3914=( struct Cyc_Absyn_Structfield*)
_temp3451->hd; _temp3916=* _temp3914; _LL3926: _temp3925= _temp3916.name; goto
_LL3924; _LL3924: _temp3923= _temp3916.tq; goto _LL3922; _LL3922: _temp3921=(
void*) _temp3916.type; goto _LL3920; _LL3920: _temp3919= _temp3916.width; goto
_LL3918; _LL3918: _temp3917= _temp3916.attributes; goto _LL3915; _LL3915: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp3925)){({ struct Cyc_Std_String_pa_struct _temp3928; _temp3928.tag= Cyc_Std_String_pa;
_temp3928.f1=( struct _tagged_arr)* _temp3925;{ void* _temp3927[ 1u]={&
_temp3928}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3927, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp3925, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3929=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3929->hd=( void*)
_temp3925; _temp3929->tl= prev_fields; _temp3929;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3921); if( ! Cyc_Tcutil_bits_only(
_temp3921)){({ struct Cyc_Std_String_pa_struct _temp3931; _temp3931.tag= Cyc_Std_String_pa;
_temp3931.f1=( struct _tagged_arr)* _temp3925;{ void* _temp3930[ 1u]={&
_temp3931}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3930, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3921, _temp3919, _temp3925); Cyc_Tcutil_check_field_atts(
loc, _temp3925, _temp3917);}}; _pop_region( uprev_rgn);} goto _LL3329; _LL3363:
if( _temp3458 ==  0){ if(* _temp3455 ==  0){({ void* _temp3932[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3932, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp3455));;}} if(*
_temp3455 ==  0){ struct _handler_cons _temp3933; _push_handler(& _temp3933);{
int _temp3935= 0; if( setjmp( _temp3933.handler)){ _temp3935= 1;} if( !
_temp3935){* _temp3455=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp3458));; _pop_handler();} else{
void* _temp3934=( void*) _exn_thrown; void* _temp3937= _temp3934; _LL3939: if(
_temp3937 ==  Cyc_Dict_Absent){ goto _LL3940;} else{ goto _LL3941;} _LL3941:
goto _LL3942; _LL3940: { struct _tuple1* tdn=( struct _tuple1*) _check_null(
_temp3458); struct Cyc_Tcenv_Genv* _temp3943=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Structdecl* _temp3944=({ struct Cyc_Absyn_Structdecl* _temp3947=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp3947->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3947->name=({ struct Cyc_Core_Opt*
_temp3948=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3948->v=( void*) tdn; _temp3948;}); _temp3947->tvs= 0; _temp3947->fields= 0;
_temp3947->attributes= 0; _temp3947;}); Cyc_Tc_tcStructdecl( te, _temp3943, loc,
_temp3944);* _temp3455=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc, tdn); if( _temp3456 !=  0){({ struct Cyc_Std_String_pa_struct _temp3946;
_temp3946.tag= Cyc_Std_String_pa; _temp3946.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tdn);{ void* _temp3945[ 1u]={& _temp3946}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized struct %s before using",
sizeof( unsigned char), 52u), _tag_arr( _temp3945, sizeof( void*), 1u));}});
return cvtenv;} goto _LL3938;} _LL3942:( void) _throw( _temp3937); _LL3938:;}}}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp3455)); if( sd->name !=  0){*(( struct _tuple1*) _check_null( _temp3458))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp3456); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3952; _temp3952.tag= Cyc_Std_Int_pa; _temp3952.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3951; _temp3951.tag= Cyc_Std_Int_pa;
_temp3951.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3950; _temp3950.tag= Cyc_Std_String_pa; _temp3950.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp3458));{ void*
_temp3949[ 3u]={& _temp3950,& _temp3951,& _temp3952}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3949, sizeof( void*), 3u));}}}});} for( 0;
_temp3456 !=  0;( _temp3456= _temp3456->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3456->hd; void* k1= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL3329;}} _LL3365: if( _temp3465 ==  0){({ void*
_temp3953[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("missing union name", sizeof(
unsigned char), 19u), _tag_arr( _temp3953, sizeof( void*), 0u));}); return
cvtenv;} if(* _temp3462 ==  0){ struct _handler_cons _temp3954; _push_handler(&
_temp3954);{ int _temp3956= 0; if( setjmp( _temp3954.handler)){ _temp3956= 1;}
if( ! _temp3956){* _temp3462=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp3465));; _pop_handler();} else{
void* _temp3955=( void*) _exn_thrown; void* _temp3958= _temp3955; _LL3960: if(
_temp3958 ==  Cyc_Dict_Absent){ goto _LL3961;} else{ goto _LL3962;} _LL3962:
goto _LL3963; _LL3961: { struct _tuple1* _temp3964=( struct _tuple1*)
_check_null( _temp3465); struct Cyc_Tcenv_Genv* _temp3965=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Uniondecl* _temp3966=({ struct Cyc_Absyn_Uniondecl* _temp3969=(
struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp3969->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp3969->name=({ struct Cyc_Core_Opt*
_temp3970=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3970->v=( void*) _temp3964; _temp3970;}); _temp3969->tvs= 0; _temp3969->fields=
0; _temp3969->attributes= 0; _temp3969;}); Cyc_Tc_tcUniondecl( te, _temp3965,
loc, _temp3966);* _temp3462=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc, _temp3964); if( _temp3463 !=  0){({ struct Cyc_Std_String_pa_struct
_temp3968; _temp3968.tag= Cyc_Std_String_pa; _temp3968.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3964);{ void* _temp3967[ 1u]={& _temp3968}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized union %s before using", sizeof(
unsigned char), 51u), _tag_arr( _temp3967, sizeof( void*), 1u));}}); return
cvtenv;} goto _LL3959;} _LL3963:( void) _throw( _temp3958); _LL3959:;}}}{ struct
Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp3462)); if( ud->name !=  0){* _temp3465=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp3463); if( lvs !=  largs){({ struct Cyc_Std_Int_pa_struct
_temp3974; _temp3974.tag= Cyc_Std_Int_pa; _temp3974.f1=( int)(( unsigned int)
largs);{ struct Cyc_Std_Int_pa_struct _temp3973; _temp3973.tag= Cyc_Std_Int_pa;
_temp3973.f1=( int)(( unsigned int) lvs);{ struct Cyc_Std_String_pa_struct
_temp3972; _temp3972.tag= Cyc_Std_String_pa; _temp3972.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp3465));{ void*
_temp3971[ 3u]={& _temp3972,& _temp3973,& _temp3974}; Cyc_Tcutil_terr( loc,
_tag_arr("union %s expects %d type arguments but was given %d", sizeof(
unsigned char), 52u), _tag_arr( _temp3971, sizeof( void*), 3u));}}}});} for( 0;
_temp3463 !=  0;( _temp3463= _temp3463->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3463->hd; void* k1= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL3329;}} _LL3367: { struct Cyc_List_List*
targs=* _temp3472; struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons
_temp3975; _push_handler(& _temp3975);{ int _temp3977= 0; if( setjmp( _temp3975.handler)){
_temp3977= 1;} if( ! _temp3977){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp3473);; _pop_handler();} else{ void* _temp3976=( void*) _exn_thrown; void*
_temp3979= _temp3976; _LL3981: if( _temp3979 ==  Cyc_Dict_Absent){ goto _LL3982;}
else{ goto _LL3983;} _LL3983: goto _LL3984; _LL3982:({ struct Cyc_Std_String_pa_struct
_temp3986; _temp3986.tag= Cyc_Std_String_pa; _temp3986.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3473);{ void* _temp3985[ 1u]={& _temp3986}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3985, sizeof( void*), 1u));}}); return cvtenv; _LL3984:( void) _throw(
_temp3979); _LL3980:;}}} _temp3473[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts= ts->tl, tvs= tvs->tl)){ void* k= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*) tvs->hd); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp3987=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3987->hd=( void*)({ struct
_tuple5* _temp3988=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp3988->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3988->f2=( void*) ts->hd;
_temp3988;}); _temp3987->tl= inst; _temp3987;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp3990; _temp3990.tag= Cyc_Std_String_pa; _temp3990.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3473);{ void* _temp3989[ 1u]={& _temp3990}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3989, sizeof( void*), 1u));}});} if( tvs !=  0){ struct Cyc_List_List*
hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){ void* k= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*) tvs->hd); void* e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({
struct Cyc_List_List* _temp3991=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3991->hd=( void*) e; _temp3991->tl= hidden_ts;
_temp3991;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k, e);
inst=({ struct Cyc_List_List* _temp3992=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp3992->hd=( void*)({ struct _tuple5*
_temp3993=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp3993->f1=(
struct Cyc_Absyn_Tvar*) tvs->hd; _temp3993->f2= e; _temp3993;}); _temp3992->tl=
inst; _temp3992;});}* _temp3472= Cyc_List_imp_append( targs, Cyc_List_imp_rev(
hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);*
_temp3469=({ struct Cyc_Core_Opt* _temp3994=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3994->v=( void*) new_typ; _temp3994;}); goto
_LL3329;}}} _LL3369: goto _LL3329; _LL3371: _temp3477= _temp3475; goto _LL3373;
_LL3373: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3477); goto _LL3329; _LL3375: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3479); goto _LL3329; _LL3377: for( 0;
_temp3481 !=  0; _temp3481= _temp3481->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*) _temp3481->hd);} goto
_LL3329; _LL3329:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){({ struct Cyc_Std_String_pa_struct _temp3998; _temp3998.tag= Cyc_Std_String_pa;
_temp3998.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_Std_String_pa_struct _temp3997; _temp3997.tag= Cyc_Std_String_pa;
_temp3997.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_Std_String_pa_struct _temp3996; _temp3996.tag= Cyc_Std_String_pa;
_temp3996.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3995[
3u]={& _temp3996,& _temp3997,& _temp3998}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3995, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3999= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp4014; _temp4014.kind_env= kind_env; _temp4014.free_vars= 0; _temp4014.free_evars=
0; _temp4014.generalize_evars= generalize_evars; _temp4014.fn_result= 0;
_temp4014;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3999.free_vars;
for( 0; vs !=  0; vs= vs->tl){ _temp3999.kind_env= Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*) vs->hd);}}{ struct Cyc_List_List* evs=
_temp3999.free_evars; for( 0; evs !=  0; evs= evs->tl){ void* _temp4000= Cyc_Tcutil_compress((
void*) evs->hd); struct Cyc_Core_Opt* _temp4006; struct Cyc_Core_Opt** _temp4008;
_LL4002: if(( unsigned int) _temp4000 >  3u?*(( int*) _temp4000) ==  Cyc_Absyn_Evar:
0){ _LL4007: _temp4006=(( struct Cyc_Absyn_Evar_struct*) _temp4000)->f4;
_temp4008=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp4000)->f4;
goto _LL4003;} else{ goto _LL4004;} _LL4004: goto _LL4005; _LL4003: if(*
_temp4008 ==  0){* _temp4008=({ struct Cyc_Core_Opt* _temp4009=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4009->v=( void*) kind_env;
_temp4009;});} else{ struct Cyc_List_List* _temp4010=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp4008))->v; struct Cyc_List_List*
_temp4011= 0; for( 0; _temp4010 !=  0; _temp4010= _temp4010->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, kind_env,(
struct Cyc_Absyn_Tvar*) _temp4010->hd)){ _temp4011=({ struct Cyc_List_List*
_temp4012=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4012->hd=( void*)(( struct Cyc_Absyn_Tvar*) _temp4010->hd); _temp4012->tl=
_temp4011; _temp4012;});}}* _temp4008=({ struct Cyc_Core_Opt* _temp4013=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4013->v=( void*)
_temp4011; _temp4013;});} goto _LL4001; _LL4005: goto _LL4001; _LL4001:;}}
return _temp3999;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ int generalize_evars= Cyc_Tcutil_is_function_type(
t); struct Cyc_Tcutil_CVTEnv _temp4015= Cyc_Tcutil_check_valid_type( loc, te, 0,(
void*) Cyc_Absyn_MemKind, generalize_evars, t); struct Cyc_List_List* _temp4016=
_temp4015.free_vars; struct Cyc_List_List* _temp4017= _temp4015.free_evars;{
struct Cyc_List_List* x= _temp4016; for( 0; x !=  0; x= x->tl){ void* _temp4018=
Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*) x->hd)->kind); struct
Cyc_Core_Opt* _temp4032; struct Cyc_Core_Opt** _temp4034; void* _temp4035;
struct Cyc_Core_Opt* _temp4037; struct Cyc_Core_Opt** _temp4039; void* _temp4040;
struct Cyc_Core_Opt* _temp4042; struct Cyc_Core_Opt** _temp4044; void* _temp4045;
struct Cyc_Core_Opt* _temp4047; struct Cyc_Core_Opt** _temp4049; void* _temp4050;
_LL4020: if(*(( int*) _temp4018) ==  Cyc_Absyn_Unknown_kb){ _LL4033: _temp4032=((
struct Cyc_Absyn_Unknown_kb_struct*) _temp4018)->f1; _temp4034=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*) _temp4018)->f1; goto _LL4021;} else{ goto
_LL4022;} _LL4022: if(*(( int*) _temp4018) ==  Cyc_Absyn_Less_kb){ _LL4038:
_temp4037=(( struct Cyc_Absyn_Less_kb_struct*) _temp4018)->f1; _temp4039=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp4018)->f1; goto
_LL4036; _LL4036: _temp4035=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp4018)->f2; if( _temp4035 == ( void*) Cyc_Absyn_MemKind){ goto _LL4023;}
else{ goto _LL4024;}} else{ goto _LL4024;} _LL4024: if(*(( int*) _temp4018) == 
Cyc_Absyn_Less_kb){ _LL4043: _temp4042=(( struct Cyc_Absyn_Less_kb_struct*)
_temp4018)->f1; _temp4044=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp4018)->f1; goto _LL4041; _LL4041: _temp4040=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp4018)->f2; if( _temp4040 == ( void*) Cyc_Absyn_AnyKind){ goto _LL4025;}
else{ goto _LL4026;}} else{ goto _LL4026;} _LL4026: if(*(( int*) _temp4018) == 
Cyc_Absyn_Less_kb){ _LL4048: _temp4047=(( struct Cyc_Absyn_Less_kb_struct*)
_temp4018)->f1; _temp4049=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp4018)->f1; goto _LL4046; _LL4046: _temp4045=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp4018)->f2; goto _LL4027;} else{ goto _LL4028;} _LL4028: if(*(( int*)
_temp4018) ==  Cyc_Absyn_Eq_kb){ _LL4051: _temp4050=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp4018)->f1; if( _temp4050 == ( void*) Cyc_Absyn_MemKind){ goto _LL4029;}
else{ goto _LL4030;}} else{ goto _LL4030;} _LL4030: goto _LL4031; _LL4021:
_temp4039= _temp4034; goto _LL4023; _LL4023: _temp4044= _temp4039; goto _LL4025;
_LL4025:* _temp4044=({ struct Cyc_Core_Opt* _temp4052=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4052->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp4053=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp4053[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp4054; _temp4054.tag= Cyc_Absyn_Eq_kb; _temp4054.f1=( void*)(( void*) Cyc_Absyn_BoxKind);
_temp4054;}); _temp4053;})); _temp4052;}); goto _LL4019; _LL4027:* _temp4049=({
struct Cyc_Core_Opt* _temp4055=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp4055->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp4056=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp4056[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp4057; _temp4057.tag= Cyc_Absyn_Eq_kb;
_temp4057.f1=( void*) _temp4045; _temp4057;}); _temp4056;})); _temp4055;}); goto
_LL4019; _LL4029:({ struct Cyc_Std_String_pa_struct _temp4059; _temp4059.tag=
Cyc_Std_String_pa; _temp4059.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*) x->hd);{ void* _temp4058[ 1u]={& _temp4059}; Cyc_Tcutil_terr(
loc, _tag_arr("type variable %s cannot have kind M", sizeof( unsigned char), 36u),
_tag_arr( _temp4058, sizeof( void*), 1u));}}); goto _LL4019; _LL4031: goto
_LL4019; _LL4019:;}} if( _temp4016 !=  0? 1: _temp4017 !=  0){{ void* _temp4060=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp4066; struct Cyc_List_List*
_temp4068; struct Cyc_List_List* _temp4070; struct Cyc_Absyn_VarargInfo*
_temp4072; int _temp4074; struct Cyc_List_List* _temp4076; void* _temp4078;
struct Cyc_Core_Opt* _temp4080; struct Cyc_List_List* _temp4082; struct Cyc_List_List**
_temp4084; _LL4062: if(( unsigned int) _temp4060 >  3u?*(( int*) _temp4060) == 
Cyc_Absyn_FnType: 0){ _LL4067: _temp4066=(( struct Cyc_Absyn_FnType_struct*)
_temp4060)->f1; _LL4083: _temp4082= _temp4066.tvars; _temp4084=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp4060)->f1).tvars; goto _LL4081; _LL4081:
_temp4080= _temp4066.effect; goto _LL4079; _LL4079: _temp4078=( void*) _temp4066.ret_typ;
goto _LL4077; _LL4077: _temp4076= _temp4066.args; goto _LL4075; _LL4075:
_temp4074= _temp4066.c_varargs; goto _LL4073; _LL4073: _temp4072= _temp4066.cyc_varargs;
goto _LL4071; _LL4071: _temp4070= _temp4066.rgn_po; goto _LL4069; _LL4069:
_temp4068= _temp4066.attributes; goto _LL4063;} else{ goto _LL4064;} _LL4064:
goto _LL4065; _LL4063: if(* _temp4084 ==  0){* _temp4084= _temp4016; _temp4016=
0;} goto _LL4061; _LL4065: goto _LL4061; _LL4061:;} if( _temp4016 !=  0){({
struct Cyc_Std_String_pa_struct _temp4086; _temp4086.tag= Cyc_Std_String_pa;
_temp4086.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) _temp4016->hd)->name;{
void* _temp4085[ 1u]={& _temp4086}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp4085, sizeof( void*), 1u));}});}
if( _temp4017 !=  0){ for( 0; _temp4017 !=  0; _temp4017= _temp4017->tl){ void*
e=( void*) _temp4017->hd; void* _temp4087= Cyc_Tcutil_typ_kind( e); _LL4089: if(
_temp4087 == ( void*) Cyc_Absyn_RgnKind){ goto _LL4090;} else{ goto _LL4091;}
_LL4091: if( _temp4087 == ( void*) Cyc_Absyn_EffKind){ goto _LL4092;} else{ goto
_LL4093;} _LL4093: goto _LL4094; _LL4090: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp4095[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp4095, sizeof( void*), 0u));});} goto _LL4088; _LL4092: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp4096[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp4096, sizeof( void*), 0u));});} goto _LL4088; _LL4094:({ struct Cyc_Std_String_pa_struct
_temp4099; _temp4099.tag= Cyc_Std_String_pa; _temp4099.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp4098;
_temp4098.tag= Cyc_Std_String_pa; _temp4098.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp4097[ 2u]={& _temp4098,& _temp4099}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp4097, sizeof( void*), 2u));}}}); goto
_LL4088; _LL4088:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp4100= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp4106; struct Cyc_List_List* _temp4108; struct
Cyc_List_List* _temp4110; struct Cyc_Absyn_VarargInfo* _temp4112; int _temp4114;
struct Cyc_List_List* _temp4116; void* _temp4118; struct Cyc_Core_Opt* _temp4120;
struct Cyc_List_List* _temp4122; _LL4102: if(( unsigned int) _temp4100 >  3u?*((
int*) _temp4100) ==  Cyc_Absyn_FnType: 0){ _LL4107: _temp4106=(( struct Cyc_Absyn_FnType_struct*)
_temp4100)->f1; _LL4123: _temp4122= _temp4106.tvars; goto _LL4121; _LL4121:
_temp4120= _temp4106.effect; goto _LL4119; _LL4119: _temp4118=( void*) _temp4106.ret_typ;
goto _LL4117; _LL4117: _temp4116= _temp4106.args; goto _LL4115; _LL4115:
_temp4114= _temp4106.c_varargs; goto _LL4113; _LL4113: _temp4112= _temp4106.cyc_varargs;
goto _LL4111; _LL4111: _temp4110= _temp4106.rgn_po; goto _LL4109; _LL4109:
_temp4108= _temp4106.attributes; goto _LL4103;} else{ goto _LL4104;} _LL4104:
goto _LL4105; _LL4103: fd->tvs= _temp4122; fd->effect= _temp4120; goto _LL4101;
_LL4105:({ void* _temp4124[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp4124, sizeof( void*), 0u));});
return; _LL4101:;}{ struct _RegionHandle _temp4125= _new_region("r"); struct
_RegionHandle* r=& _temp4125; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp4126=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4126->v=(
void*) t; _temp4126;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp4127=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp4128= Cyc_Tcutil_remove_bound_tvars( _temp4127.free_vars,
bound_tvars); struct Cyc_List_List* _temp4129= _temp4127.free_evars;{ struct Cyc_List_List*
fs= _temp4128; for( 0; fs !=  0; fs= fs->tl){ struct _tagged_arr* _temp4130=((
struct Cyc_Absyn_Tvar*) fs->hd)->name;({ struct Cyc_Std_String_pa_struct
_temp4133; _temp4133.tag= Cyc_Std_String_pa; _temp4133.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp4132;
_temp4132.tag= Cyc_Std_String_pa; _temp4132.f1=( struct _tagged_arr)* _temp4130;{
void* _temp4131[ 2u]={& _temp4132,& _temp4133}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp4131, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp4129 !=  0: 0){ for( 0; _temp4129 !=  0; _temp4129=
_temp4129->tl){ void* e=( void*) _temp4129->hd; void* _temp4134= Cyc_Tcutil_typ_kind(
e); _LL4136: if( _temp4134 == ( void*) Cyc_Absyn_RgnKind){ goto _LL4137;} else{
goto _LL4138;} _LL4138: if( _temp4134 == ( void*) Cyc_Absyn_EffKind){ goto
_LL4139;} else{ goto _LL4140;} _LL4140: goto _LL4141; _LL4137: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp4142[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp4142, sizeof( void*), 0u));});}
goto _LL4135; _LL4139: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4143=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4143[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4144; _temp4144.tag= Cyc_Absyn_JoinEff;
_temp4144.f1= 0; _temp4144;}); _temp4143;}))){({ void* _temp4145[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp4145, sizeof( void*), 0u));});}
goto _LL4135; _LL4141:({ struct Cyc_Std_String_pa_struct _temp4148; _temp4148.tag=
Cyc_Std_String_pa; _temp4148.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp4147; _temp4147.tag= Cyc_Std_String_pa;
_temp4147.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp4146[
2u]={& _temp4147,& _temp4148}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp4146, sizeof( void*), 2u));}}});
goto _LL4135; _LL4135:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs= vs->tl){
struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 !=  0; vs2= vs2->tl){ if( cmp((
void*) vs->hd,( void*) vs2->hd) ==  0){({ struct Cyc_Std_String_pa_struct
_temp4151; _temp4151.tag= Cyc_Std_String_pa; _temp4151.f1=( struct _tagged_arr)
a2string(( void*) vs->hd);{ struct Cyc_Std_String_pa_struct _temp4150; _temp4150.tag=
Cyc_Std_String_pa; _temp4150.f1=( struct _tagged_arr) msg;{ void* _temp4149[ 2u]={&
_temp4150,& _temp4151}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp4149, sizeof( void*), 2u));}}});}}}} static
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
_tuple18{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple20{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields !=  0; sd_fields=
sd_fields->tl){ if( Cyc_Std_strcmp(*(( struct Cyc_Absyn_Structfield*) sd_fields->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List*
_temp4152=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4152->hd=( void*)({ struct _tuple18* _temp4153=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp4153->f1=( struct Cyc_Absyn_Structfield*)
sd_fields->hd; _temp4153->f2= 0; _temp4153;}); _temp4152->tl= fields; _temp4152;});}}}
fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0; des= des->tl){ struct
_tuple19 _temp4156; void* _temp4157; struct Cyc_List_List* _temp4159; struct
_tuple19* _temp4154=( struct _tuple19*) des->hd; _temp4156=* _temp4154; _LL4160:
_temp4159= _temp4156.f1; goto _LL4158; _LL4158: _temp4157= _temp4156.f2; goto
_LL4155; _LL4155: if( _temp4159 ==  0){ struct Cyc_List_List* _temp4161= fields;
for( 0; _temp4161 !=  0; _temp4161= _temp4161->tl){ if( !(*(( struct _tuple18*)
_temp4161->hd)).f2){(*(( struct _tuple18*) _temp4161->hd)).f2= 1;(*(( struct
_tuple19*) des->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp4162=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4162->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4163=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4163[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4164; _temp4164.tag= Cyc_Absyn_FieldName;
_temp4164.f1=((*(( struct _tuple18*) _temp4161->hd)).f1)->name; _temp4164;});
_temp4163;})); _temp4162->tl= 0; _temp4162;}); ans=({ struct Cyc_List_List*
_temp4165=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp4165->hd=( void*)({ struct _tuple20* _temp4166=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp4166->f1=(*(( struct
_tuple18*) _temp4161->hd)).f1; _temp4166->f2= _temp4157; _temp4166;}); _temp4165->tl=
ans; _temp4165;}); break;}} if( _temp4161 ==  0){({ void* _temp4167[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u),
_tag_arr( _temp4167, sizeof( void*), 0u));});}} else{ if( _temp4159->tl !=  0){({
void* _temp4168[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp4168, sizeof( void*), 0u));});}
else{ void* _temp4169=( void*) _temp4159->hd; struct _tagged_arr* _temp4175;
_LL4171: if(*(( int*) _temp4169) ==  Cyc_Absyn_ArrayElement){ goto _LL4172;}
else{ goto _LL4173;} _LL4173: if(*(( int*) _temp4169) ==  Cyc_Absyn_FieldName){
_LL4176: _temp4175=(( struct Cyc_Absyn_FieldName_struct*) _temp4169)->f1; goto
_LL4174;} else{ goto _LL4170;} _LL4172:({ void* _temp4177[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp4177, sizeof( void*), 0u));}); goto _LL4170;
_LL4174: { struct Cyc_List_List* _temp4178= fields; for( 0; _temp4178 !=  0;
_temp4178= _temp4178->tl){ if( Cyc_Std_zstrptrcmp( _temp4175,((*(( struct
_tuple18*) _temp4178->hd)).f1)->name) ==  0){ if((*(( struct _tuple18*)
_temp4178->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp4180; _temp4180.tag=
Cyc_Std_String_pa; _temp4180.f1=( struct _tagged_arr)* _temp4175;{ void*
_temp4179[ 1u]={& _temp4180}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp4179, sizeof( void*), 1u));}});}(*((
struct _tuple18*) _temp4178->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp4181=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp4181->hd=( void*)({ struct _tuple20* _temp4182=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp4182->f1=(*(( struct
_tuple18*) _temp4178->hd)).f1; _temp4182->f2= _temp4157; _temp4182;}); _temp4181->tl=
ans; _temp4181;}); break;}} if( _temp4178 ==  0){({ struct Cyc_Std_String_pa_struct
_temp4184; _temp4184.tag= Cyc_Std_String_pa; _temp4184.f1=( struct _tagged_arr)*
_temp4175;{ void* _temp4183[ 1u]={& _temp4184}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp4183, sizeof( void*), 1u));}});}
goto _LL4170;} _LL4170:;}}} for( 0; fields !=  0; fields= fields->tl){ if( !(*((
struct _tuple18*) fields->hd)).f2){({ void* _temp4185[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments to struct", sizeof( unsigned char), 28u),
_tag_arr( _temp4185, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp4186= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp4192; struct Cyc_Absyn_Conref* _temp4194; void* _temp4196;
_LL4188: if(( unsigned int) _temp4186 >  3u?*(( int*) _temp4186) ==  Cyc_Absyn_PointerType:
0){ _LL4193: _temp4192=(( struct Cyc_Absyn_PointerType_struct*) _temp4186)->f1;
_LL4197: _temp4196=( void*) _temp4192.elt_typ; goto _LL4195; _LL4195: _temp4194=
_temp4192.bounds; goto _LL4189;} else{ goto _LL4190;} _LL4190: goto _LL4191;
_LL4189: { struct Cyc_Absyn_Conref* _temp4198= Cyc_Absyn_compress_conref(
_temp4194); void* _temp4199=( void*)( Cyc_Absyn_compress_conref( _temp4198))->v;
void* _temp4207; _LL4201: if(( unsigned int) _temp4199 >  1u?*(( int*) _temp4199)
==  Cyc_Absyn_Eq_constr: 0){ _LL4208: _temp4207=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4199)->f1; if( _temp4207 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4202;}
else{ goto _LL4203;}} else{ goto _LL4203;} _LL4203: if( _temp4199 == ( void*)
Cyc_Absyn_No_constr){ goto _LL4204;} else{ goto _LL4205;} _LL4205: goto _LL4206;
_LL4202:* elt_typ_dest= _temp4196; return 1; _LL4204:( void*)( _temp4198->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp4209=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp4209[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp4210; _temp4210.tag= Cyc_Absyn_Eq_constr;
_temp4210.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp4210;}); _temp4209;})));*
elt_typ_dest= _temp4196; return 1; _LL4206: return 0; _LL4200:;} _LL4191: return
0; _LL4187:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp4393;
_temp4393.f1= 0; _temp4393.f2=( void*) Cyc_Absyn_HeapRgn; _temp4393;}); void*
_temp4211=( void*) e->r; void* _temp4225; struct _tuple1* _temp4227; struct
_tagged_arr* _temp4229; struct Cyc_Absyn_Exp* _temp4231; struct _tagged_arr*
_temp4233; struct Cyc_Absyn_Exp* _temp4235; struct Cyc_Absyn_Exp* _temp4237;
struct Cyc_Absyn_Exp* _temp4239; struct Cyc_Absyn_Exp* _temp4241; _LL4213: if(*((
int*) _temp4211) ==  Cyc_Absyn_Var_e){ _LL4228: _temp4227=(( struct Cyc_Absyn_Var_e_struct*)
_temp4211)->f1; goto _LL4226; _LL4226: _temp4225=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp4211)->f2; goto _LL4214;} else{ goto _LL4215;} _LL4215: if(*(( int*)
_temp4211) ==  Cyc_Absyn_StructMember_e){ _LL4232: _temp4231=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp4211)->f1; goto _LL4230; _LL4230: _temp4229=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp4211)->f2; goto _LL4216;} else{ goto _LL4217;} _LL4217: if(*(( int*)
_temp4211) ==  Cyc_Absyn_StructArrow_e){ _LL4236: _temp4235=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp4211)->f1; goto _LL4234; _LL4234: _temp4233=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp4211)->f2; goto _LL4218;} else{ goto _LL4219;} _LL4219: if(*(( int*)
_temp4211) ==  Cyc_Absyn_Deref_e){ _LL4238: _temp4237=(( struct Cyc_Absyn_Deref_e_struct*)
_temp4211)->f1; goto _LL4220;} else{ goto _LL4221;} _LL4221: if(*(( int*)
_temp4211) ==  Cyc_Absyn_Subscript_e){ _LL4242: _temp4241=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp4211)->f1; goto _LL4240; _LL4240: _temp4239=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp4211)->f2; goto _LL4222;} else{ goto _LL4223;} _LL4223: goto _LL4224;
_LL4214: { void* _temp4243= _temp4225; struct Cyc_Absyn_Vardecl* _temp4257;
struct Cyc_Absyn_Vardecl* _temp4259; struct Cyc_Absyn_Vardecl* _temp4261; struct
Cyc_Absyn_Vardecl* _temp4263; _LL4245: if( _temp4243 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL4246;} else{ goto _LL4247;} _LL4247: if(( unsigned int) _temp4243 >  1u?*((
int*) _temp4243) ==  Cyc_Absyn_Funname_b: 0){ goto _LL4248;} else{ goto _LL4249;}
_LL4249: if(( unsigned int) _temp4243 >  1u?*(( int*) _temp4243) ==  Cyc_Absyn_Global_b:
0){ _LL4258: _temp4257=(( struct Cyc_Absyn_Global_b_struct*) _temp4243)->f1;
goto _LL4250;} else{ goto _LL4251;} _LL4251: if(( unsigned int) _temp4243 >  1u?*((
int*) _temp4243) ==  Cyc_Absyn_Local_b: 0){ _LL4260: _temp4259=(( struct Cyc_Absyn_Local_b_struct*)
_temp4243)->f1; goto _LL4252;} else{ goto _LL4253;} _LL4253: if(( unsigned int)
_temp4243 >  1u?*(( int*) _temp4243) ==  Cyc_Absyn_Pat_b: 0){ _LL4262: _temp4261=((
struct Cyc_Absyn_Pat_b_struct*) _temp4243)->f1; goto _LL4254;} else{ goto
_LL4255;} _LL4255: if(( unsigned int) _temp4243 >  1u?*(( int*) _temp4243) == 
Cyc_Absyn_Param_b: 0){ _LL4264: _temp4263=(( struct Cyc_Absyn_Param_b_struct*)
_temp4243)->f1; goto _LL4256;} else{ goto _LL4244;} _LL4246: return bogus_ans;
_LL4248: return({ struct _tuple7 _temp4265; _temp4265.f1= 0; _temp4265.f2=( void*)
Cyc_Absyn_HeapRgn; _temp4265;}); _LL4250: { void* _temp4266= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL4268: if((
unsigned int) _temp4266 >  3u?*(( int*) _temp4266) ==  Cyc_Absyn_ArrayType: 0){
goto _LL4269;} else{ goto _LL4270;} _LL4270: goto _LL4271; _LL4269: return({
struct _tuple7 _temp4272; _temp4272.f1= 1; _temp4272.f2=( void*) Cyc_Absyn_HeapRgn;
_temp4272;}); _LL4271: return({ struct _tuple7 _temp4273; _temp4273.f1=(
_temp4257->tq).q_const; _temp4273.f2=( void*) Cyc_Absyn_HeapRgn; _temp4273;});
_LL4267:;} _LL4252: { void* _temp4274= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL4276: if(( unsigned int) _temp4274 >  3u?*(( int*)
_temp4274) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4277;} else{ goto _LL4278;}
_LL4278: goto _LL4279; _LL4277: return({ struct _tuple7 _temp4280; _temp4280.f1=
1; _temp4280.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp4259->rgn))->v;
_temp4280;}); _LL4279: return({ struct _tuple7 _temp4281; _temp4281.f1=(
_temp4259->tq).q_const; _temp4281.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4259->rgn))->v; _temp4281;}); _LL4275:;} _LL4254: _temp4263=
_temp4261; goto _LL4256; _LL4256: return({ struct _tuple7 _temp4282; _temp4282.f1=(
_temp4263->tq).q_const; _temp4282.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4263->rgn))->v; _temp4282;}); _LL4244:;} _LL4216: { void*
_temp4283= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp4231->topt))->v); struct Cyc_List_List* _temp4295; struct Cyc_List_List*
_temp4297; struct Cyc_Absyn_Structdecl** _temp4299; struct Cyc_Absyn_Structdecl*
_temp4301; struct Cyc_Absyn_Uniondecl** _temp4302; struct Cyc_Absyn_Uniondecl*
_temp4304; _LL4285: if(( unsigned int) _temp4283 >  3u?*(( int*) _temp4283) == 
Cyc_Absyn_AnonStructType: 0){ _LL4296: _temp4295=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp4283)->f1; goto _LL4286;} else{ goto _LL4287;} _LL4287: if(( unsigned int)
_temp4283 >  3u?*(( int*) _temp4283) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4298:
_temp4297=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp4283)->f1; goto
_LL4288;} else{ goto _LL4289;} _LL4289: if(( unsigned int) _temp4283 >  3u?*((
int*) _temp4283) ==  Cyc_Absyn_StructType: 0){ _LL4300: _temp4299=(( struct Cyc_Absyn_StructType_struct*)
_temp4283)->f3; if( _temp4299 ==  0){ goto _LL4291;} else{ _temp4301=* _temp4299;
goto _LL4290;}} else{ goto _LL4291;} _LL4291: if(( unsigned int) _temp4283 >  3u?*((
int*) _temp4283) ==  Cyc_Absyn_UnionType: 0){ _LL4303: _temp4302=(( struct Cyc_Absyn_UnionType_struct*)
_temp4283)->f3; if( _temp4302 ==  0){ goto _LL4293;} else{ _temp4304=* _temp4302;
goto _LL4292;}} else{ goto _LL4293;} _LL4293: goto _LL4294; _LL4286: _temp4297=
_temp4295; goto _LL4288; _LL4288: { struct Cyc_Absyn_Structfield* _temp4305= Cyc_Absyn_lookup_field(
_temp4297, _temp4229); if( _temp4305 !=  0? _temp4305->width !=  0: 0){ return({
struct _tuple7 _temp4306; _temp4306.f1=( _temp4305->tq).q_const; _temp4306.f2=(
Cyc_Tcutil_addressof_props( te, _temp4231)).f2; _temp4306;});} return bogus_ans;}
_LL4290: { struct Cyc_Absyn_Structfield* _temp4307= Cyc_Absyn_lookup_struct_field(
_temp4301, _temp4229); if( _temp4307 !=  0? _temp4307->width !=  0: 0){ return({
struct _tuple7 _temp4308; _temp4308.f1=( _temp4307->tq).q_const; _temp4308.f2=(
Cyc_Tcutil_addressof_props( te, _temp4231)).f2; _temp4308;});} return bogus_ans;}
_LL4292: { struct Cyc_Absyn_Structfield* _temp4309= Cyc_Absyn_lookup_union_field(
_temp4304, _temp4229); if( _temp4309 !=  0){ return({ struct _tuple7 _temp4310;
_temp4310.f1=( _temp4309->tq).q_const; _temp4310.f2=( Cyc_Tcutil_addressof_props(
te, _temp4231)).f2; _temp4310;});} goto _LL4294;} _LL4294: return bogus_ans;
_LL4284:;} _LL4218: { void* _temp4311= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4235->topt))->v); struct Cyc_Absyn_PtrInfo _temp4317; void*
_temp4319; void* _temp4321; _LL4313: if(( unsigned int) _temp4311 >  3u?*(( int*)
_temp4311) ==  Cyc_Absyn_PointerType: 0){ _LL4318: _temp4317=(( struct Cyc_Absyn_PointerType_struct*)
_temp4311)->f1; _LL4322: _temp4321=( void*) _temp4317.elt_typ; goto _LL4320;
_LL4320: _temp4319=( void*) _temp4317.rgn_typ; goto _LL4314;} else{ goto _LL4315;}
_LL4315: goto _LL4316; _LL4314: { void* _temp4323= Cyc_Tcutil_compress(
_temp4321); struct Cyc_List_List* _temp4335; struct Cyc_List_List* _temp4337;
struct Cyc_Absyn_Structdecl** _temp4339; struct Cyc_Absyn_Structdecl* _temp4341;
struct Cyc_Absyn_Uniondecl** _temp4342; struct Cyc_Absyn_Uniondecl* _temp4344;
_LL4325: if(( unsigned int) _temp4323 >  3u?*(( int*) _temp4323) ==  Cyc_Absyn_AnonStructType:
0){ _LL4336: _temp4335=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4323)->f1;
goto _LL4326;} else{ goto _LL4327;} _LL4327: if(( unsigned int) _temp4323 >  3u?*((
int*) _temp4323) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4338: _temp4337=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4323)->f1; goto _LL4328;} else{ goto
_LL4329;} _LL4329: if(( unsigned int) _temp4323 >  3u?*(( int*) _temp4323) == 
Cyc_Absyn_StructType: 0){ _LL4340: _temp4339=(( struct Cyc_Absyn_StructType_struct*)
_temp4323)->f3; if( _temp4339 ==  0){ goto _LL4331;} else{ _temp4341=* _temp4339;
goto _LL4330;}} else{ goto _LL4331;} _LL4331: if(( unsigned int) _temp4323 >  3u?*((
int*) _temp4323) ==  Cyc_Absyn_UnionType: 0){ _LL4343: _temp4342=(( struct Cyc_Absyn_UnionType_struct*)
_temp4323)->f3; if( _temp4342 ==  0){ goto _LL4333;} else{ _temp4344=* _temp4342;
goto _LL4332;}} else{ goto _LL4333;} _LL4333: goto _LL4334; _LL4326: _temp4337=
_temp4335; goto _LL4328; _LL4328: { struct Cyc_Absyn_Structfield* _temp4345= Cyc_Absyn_lookup_field(
_temp4337, _temp4233); if( _temp4345 !=  0? _temp4345->width !=  0: 0){ return({
struct _tuple7 _temp4346; _temp4346.f1=( _temp4345->tq).q_const; _temp4346.f2=
_temp4319; _temp4346;});} return bogus_ans;} _LL4330: { struct Cyc_Absyn_Structfield*
_temp4347= Cyc_Absyn_lookup_struct_field( _temp4341, _temp4233); if( _temp4347
!=  0? _temp4347->width !=  0: 0){ return({ struct _tuple7 _temp4348; _temp4348.f1=(
_temp4347->tq).q_const; _temp4348.f2= _temp4319; _temp4348;});} return bogus_ans;}
_LL4332: { struct Cyc_Absyn_Structfield* _temp4349= Cyc_Absyn_lookup_union_field(
_temp4344, _temp4233); if( _temp4349 !=  0){ return({ struct _tuple7 _temp4350;
_temp4350.f1=( _temp4349->tq).q_const; _temp4350.f2= _temp4319; _temp4350;});}
return bogus_ans;} _LL4334: return bogus_ans; _LL4324:;} _LL4316: return
bogus_ans; _LL4312:;} _LL4220: { void* _temp4351= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp4237->topt))->v); struct Cyc_Absyn_PtrInfo
_temp4357; struct Cyc_Absyn_Tqual _temp4359; void* _temp4361; _LL4353: if((
unsigned int) _temp4351 >  3u?*(( int*) _temp4351) ==  Cyc_Absyn_PointerType: 0){
_LL4358: _temp4357=(( struct Cyc_Absyn_PointerType_struct*) _temp4351)->f1;
_LL4362: _temp4361=( void*) _temp4357.rgn_typ; goto _LL4360; _LL4360: _temp4359=
_temp4357.tq; goto _LL4354;} else{ goto _LL4355;} _LL4355: goto _LL4356; _LL4354:
return({ struct _tuple7 _temp4363; _temp4363.f1= _temp4359.q_const; _temp4363.f2=
_temp4361; _temp4363;}); _LL4356: return bogus_ans; _LL4352:;} _LL4222: { void*
t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp4241->topt))->v);
void* _temp4364= t; struct Cyc_Absyn_Tqual _temp4374; struct Cyc_List_List*
_temp4376; struct Cyc_Absyn_PtrInfo _temp4378; struct Cyc_Absyn_Conref*
_temp4380; struct Cyc_Absyn_Tqual _temp4382; void* _temp4384; void* _temp4386;
_LL4366: if(( unsigned int) _temp4364 >  3u?*(( int*) _temp4364) ==  Cyc_Absyn_ArrayType:
0){ _LL4375: _temp4374=(( struct Cyc_Absyn_ArrayType_struct*) _temp4364)->f2;
goto _LL4367;} else{ goto _LL4368;} _LL4368: if(( unsigned int) _temp4364 >  3u?*((
int*) _temp4364) ==  Cyc_Absyn_TupleType: 0){ _LL4377: _temp4376=(( struct Cyc_Absyn_TupleType_struct*)
_temp4364)->f1; goto _LL4369;} else{ goto _LL4370;} _LL4370: if(( unsigned int)
_temp4364 >  3u?*(( int*) _temp4364) ==  Cyc_Absyn_PointerType: 0){ _LL4379:
_temp4378=(( struct Cyc_Absyn_PointerType_struct*) _temp4364)->f1; _LL4387:
_temp4386=( void*) _temp4378.elt_typ; goto _LL4385; _LL4385: _temp4384=( void*)
_temp4378.rgn_typ; goto _LL4383; _LL4383: _temp4382= _temp4378.tq; goto _LL4381;
_LL4381: _temp4380= _temp4378.bounds; goto _LL4371;} else{ goto _LL4372;}
_LL4372: goto _LL4373; _LL4367: return({ struct _tuple7 _temp4388; _temp4388.f1=
_temp4374.q_const; _temp4388.f2=( Cyc_Tcutil_addressof_props( te, _temp4241)).f2;
_temp4388;}); _LL4369: { struct _tuple4* _temp4389= Cyc_Absyn_lookup_tuple_field(
_temp4376,( int) Cyc_Evexp_eval_const_uint_exp( _temp4239)); if( _temp4389 !=  0){
return({ struct _tuple7 _temp4390; _temp4390.f1=((* _temp4389).f1).q_const;
_temp4390.f2=( Cyc_Tcutil_addressof_props( te, _temp4241)).f2; _temp4390;});}
return bogus_ans;} _LL4371: return({ struct _tuple7 _temp4391; _temp4391.f1=
_temp4382.q_const; _temp4391.f2= _temp4384; _temp4391;}); _LL4373: return
bogus_ans; _LL4365:;} _LL4224:({ void* _temp4392[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u), _tag_arr(
_temp4392, sizeof( void*), 0u));}); return bogus_ans; _LL4212:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp4394= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Tqual _temp4400; void*
_temp4402; _LL4396: if(( unsigned int) _temp4394 >  3u?*(( int*) _temp4394) == 
Cyc_Absyn_ArrayType: 0){ _LL4403: _temp4402=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp4394)->f1; goto _LL4401; _LL4401: _temp4400=(( struct Cyc_Absyn_ArrayType_struct*)
_temp4394)->f2; goto _LL4397;} else{ goto _LL4398;} _LL4398: goto _LL4399;
_LL4397: { void* _temp4406; struct _tuple7 _temp4404= Cyc_Tcutil_addressof_props(
te, e); _LL4407: _temp4406= _temp4404.f2; goto _LL4405; _LL4405: return Cyc_Absyn_atb_typ(
_temp4402, _temp4406, _temp4400,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp4408=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4408[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4409; _temp4409.tag= Cyc_Absyn_Upper_b;
_temp4409.f1= e; _temp4409;}); _temp4408;}));} _LL4399: return e_typ; _LL4395:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp4410=(
void*) b->v; void* _temp4418; void* _temp4420; struct Cyc_Absyn_Exp* _temp4422;
_LL4412: if(( unsigned int) _temp4410 >  1u?*(( int*) _temp4410) ==  Cyc_Absyn_Eq_constr:
0){ _LL4419: _temp4418=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4410)->f1;
if( _temp4418 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4413;} else{ goto
_LL4414;}} else{ goto _LL4414;} _LL4414: if(( unsigned int) _temp4410 >  1u?*((
int*) _temp4410) ==  Cyc_Absyn_Eq_constr: 0){ _LL4421: _temp4420=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp4410)->f1; if(( unsigned int) _temp4420
>  1u?*(( int*) _temp4420) ==  Cyc_Absyn_Upper_b: 0){ _LL4423: _temp4422=((
struct Cyc_Absyn_Upper_b_struct*) _temp4420)->f1; goto _LL4415;} else{ goto
_LL4416;}} else{ goto _LL4416;} _LL4416: goto _LL4417; _LL4413: return; _LL4415:
if( Cyc_Evexp_eval_const_uint_exp( _temp4422) <=  i){({ struct Cyc_Std_Int_pa_struct
_temp4426; _temp4426.tag= Cyc_Std_Int_pa; _temp4426.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp4425; _temp4425.tag= Cyc_Std_Int_pa; _temp4425.f1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp4422);{ void* _temp4424[ 2u]={& _temp4425,& _temp4426}; Cyc_Tcutil_terr(
loc, _tag_arr("dereference is out of bounds: %d <= %d", sizeof( unsigned char),
39u), _tag_arr( _temp4424, sizeof( void*), 2u));}}});} return; _LL4417:( void*)(
b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp4427=( struct
Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp4427[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp4428; _temp4428.tag= Cyc_Absyn_Eq_constr;
_temp4428.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp4429=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4429[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4430; _temp4430.tag= Cyc_Absyn_Upper_b;
_temp4430.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp4430;}); _temp4429;}));
_temp4428;}); _temp4427;}))); return; _LL4411:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp4431=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp4437; struct
Cyc_Absyn_Exp* _temp4439; _LL4433: if(( unsigned int) _temp4431 >  1u?*(( int*)
_temp4431) ==  Cyc_Absyn_Eq_constr: 0){ _LL4438: _temp4437=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4431)->f1; if(( unsigned int) _temp4437 >  1u?*(( int*) _temp4437) ==  Cyc_Absyn_Upper_b:
0){ _LL4440: _temp4439=(( struct Cyc_Absyn_Upper_b_struct*) _temp4437)->f1; goto
_LL4434;} else{ goto _LL4435;}} else{ goto _LL4435;} _LL4435: goto _LL4436;
_LL4434: return Cyc_Evexp_eval_const_uint_exp( _temp4439) ==  1; _LL4436: return
0; _LL4432:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp4441= Cyc_Tcutil_compress(
t); void* _temp4469; struct Cyc_List_List* _temp4471; struct Cyc_Absyn_Structdecl**
_temp4473; struct Cyc_List_List* _temp4475; struct Cyc_Absyn_Uniondecl**
_temp4477; struct Cyc_List_List* _temp4479; struct Cyc_List_List* _temp4481;
struct Cyc_List_List* _temp4483; _LL4443: if( _temp4441 == ( void*) Cyc_Absyn_VoidType){
goto _LL4444;} else{ goto _LL4445;} _LL4445: if(( unsigned int) _temp4441 >  3u?*((
int*) _temp4441) ==  Cyc_Absyn_IntType: 0){ goto _LL4446;} else{ goto _LL4447;}
_LL4447: if( _temp4441 == ( void*) Cyc_Absyn_FloatType){ goto _LL4448;} else{
goto _LL4449;} _LL4449: if(( unsigned int) _temp4441 >  3u?*(( int*) _temp4441)
==  Cyc_Absyn_DoubleType: 0){ goto _LL4450;} else{ goto _LL4451;} _LL4451: if((
unsigned int) _temp4441 >  3u?*(( int*) _temp4441) ==  Cyc_Absyn_EnumType: 0){
goto _LL4452;} else{ goto _LL4453;} _LL4453: if(( unsigned int) _temp4441 >  3u?*((
int*) _temp4441) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL4454;} else{ goto
_LL4455;} _LL4455: if(( unsigned int) _temp4441 >  3u?*(( int*) _temp4441) == 
Cyc_Absyn_ArrayType: 0){ _LL4470: _temp4469=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp4441)->f1; goto _LL4456;} else{ goto _LL4457;} _LL4457: if(( unsigned int)
_temp4441 >  3u?*(( int*) _temp4441) ==  Cyc_Absyn_TupleType: 0){ _LL4472:
_temp4471=(( struct Cyc_Absyn_TupleType_struct*) _temp4441)->f1; goto _LL4458;}
else{ goto _LL4459;} _LL4459: if(( unsigned int) _temp4441 >  3u?*(( int*)
_temp4441) ==  Cyc_Absyn_StructType: 0){ _LL4476: _temp4475=(( struct Cyc_Absyn_StructType_struct*)
_temp4441)->f2; goto _LL4474; _LL4474: _temp4473=(( struct Cyc_Absyn_StructType_struct*)
_temp4441)->f3; goto _LL4460;} else{ goto _LL4461;} _LL4461: if(( unsigned int)
_temp4441 >  3u?*(( int*) _temp4441) ==  Cyc_Absyn_UnionType: 0){ _LL4480:
_temp4479=(( struct Cyc_Absyn_UnionType_struct*) _temp4441)->f2; goto _LL4478;
_LL4478: _temp4477=(( struct Cyc_Absyn_UnionType_struct*) _temp4441)->f3; goto
_LL4462;} else{ goto _LL4463;} _LL4463: if(( unsigned int) _temp4441 >  3u?*((
int*) _temp4441) ==  Cyc_Absyn_AnonStructType: 0){ _LL4482: _temp4481=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp4441)->f1; goto _LL4464;} else{ goto
_LL4465;} _LL4465: if(( unsigned int) _temp4441 >  3u?*(( int*) _temp4441) == 
Cyc_Absyn_AnonUnionType: 0){ _LL4484: _temp4483=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp4441)->f1; goto _LL4466;} else{ goto _LL4467;} _LL4467: goto _LL4468;
_LL4444: goto _LL4446; _LL4446: goto _LL4448; _LL4448: goto _LL4450; _LL4450:
return 1; _LL4452: return 0; _LL4454: return 0; _LL4456: return Cyc_Tcutil_bits_only(
_temp4469); _LL4458: for( 0; _temp4471 !=  0; _temp4471= _temp4471->tl){ if( !
Cyc_Tcutil_bits_only((*(( struct _tuple4*) _temp4471->hd)).f2)){ return 0;}}
return 1; _LL4460: if( _temp4473 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp4485=* _temp4473; if( _temp4485->fields ==  0){ return 0;}{ struct
_RegionHandle _temp4486= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp4486; _push_region( rgn);{ struct Cyc_List_List* _temp4487=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp4485->tvs, _temp4475);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp4485->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp4487,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ int _temp4488= 0; _npop_handler( 0u); return _temp4488;}}}{
int _temp4489= 1; _npop_handler( 0u); return _temp4489;}}; _pop_region( rgn);}}
_LL4462: if( _temp4477 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* _temp4490=*
_temp4477; if( _temp4490->fields ==  0){ return 0;}{ struct _RegionHandle
_temp4491= _new_region("rgn"); struct _RegionHandle* rgn=& _temp4491;
_push_region( rgn);{ struct Cyc_List_List* _temp4492=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp4490->tvs, _temp4479);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp4490->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp4492,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ int _temp4493= 0; _npop_handler( 0u); return _temp4493;}}}{
int _temp4494= 1; _npop_handler( 0u); return _temp4494;}}; _pop_region( rgn);}}
_LL4464: _temp4483= _temp4481; goto _LL4466; _LL4466: for( 0; _temp4483 !=  0;
_temp4483= _temp4483->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)
_temp4483->hd)->type)){ return 0;}} return 1; _LL4468: return 0; _LL4442:;}
struct _tuple21{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static
int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te, int var_okay, struct Cyc_Absyn_Exp*
e){ void* _temp4495=( void*) e->r; void* _temp4541; struct _tuple1* _temp4543;
struct Cyc_Absyn_Exp* _temp4545; struct Cyc_Absyn_Exp* _temp4547; struct Cyc_Absyn_Exp*
_temp4549; struct Cyc_Absyn_Exp* _temp4551; struct Cyc_Absyn_Exp* _temp4553;
struct Cyc_Absyn_Exp* _temp4555; struct Cyc_Absyn_Exp* _temp4557; struct Cyc_Absyn_Exp*
_temp4559; void* _temp4561; struct Cyc_Absyn_Exp* _temp4563; struct Cyc_Absyn_Exp*
_temp4565; struct Cyc_Absyn_Exp* _temp4567; struct Cyc_List_List* _temp4569;
struct Cyc_List_List* _temp4571; struct Cyc_List_List* _temp4573; struct Cyc_List_List*
_temp4575; void* _temp4577; struct Cyc_List_List* _temp4579; struct Cyc_List_List*
_temp4581; _LL4497: if(*(( int*) _temp4495) ==  Cyc_Absyn_Const_e){ goto _LL4498;}
else{ goto _LL4499;} _LL4499: if(*(( int*) _temp4495) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL4500;} else{ goto _LL4501;} _LL4501: if(*(( int*) _temp4495) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL4502;} else{ goto _LL4503;} _LL4503: if(*(( int*) _temp4495) ==  Cyc_Absyn_Offsetof_e){
goto _LL4504;} else{ goto _LL4505;} _LL4505: if(*(( int*) _temp4495) ==  Cyc_Absyn_Gentyp_e){
goto _LL4506;} else{ goto _LL4507;} _LL4507: if(*(( int*) _temp4495) ==  Cyc_Absyn_Enum_e){
goto _LL4508;} else{ goto _LL4509;} _LL4509: if(*(( int*) _temp4495) ==  Cyc_Absyn_AnonEnum_e){
goto _LL4510;} else{ goto _LL4511;} _LL4511: if(*(( int*) _temp4495) ==  Cyc_Absyn_Var_e){
_LL4544: _temp4543=(( struct Cyc_Absyn_Var_e_struct*) _temp4495)->f1; goto
_LL4542; _LL4542: _temp4541=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp4495)->f2;
goto _LL4512;} else{ goto _LL4513;} _LL4513: if(*(( int*) _temp4495) ==  Cyc_Absyn_Conditional_e){
_LL4550: _temp4549=(( struct Cyc_Absyn_Conditional_e_struct*) _temp4495)->f1;
goto _LL4548; _LL4548: _temp4547=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4495)->f2; goto _LL4546; _LL4546: _temp4545=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4495)->f3; goto _LL4514;} else{ goto _LL4515;} _LL4515: if(*(( int*)
_temp4495) ==  Cyc_Absyn_SeqExp_e){ _LL4554: _temp4553=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4495)->f1; goto _LL4552; _LL4552: _temp4551=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4495)->f2; goto _LL4516;} else{ goto _LL4517;} _LL4517: if(*(( int*)
_temp4495) ==  Cyc_Absyn_NoInstantiate_e){ _LL4556: _temp4555=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp4495)->f1; goto _LL4518;} else{ goto _LL4519;} _LL4519: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Instantiate_e){ _LL4558: _temp4557=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp4495)->f1; goto _LL4520;} else{ goto _LL4521;} _LL4521: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Cast_e){ _LL4562: _temp4561=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp4495)->f1; goto _LL4560; _LL4560: _temp4559=(( struct Cyc_Absyn_Cast_e_struct*)
_temp4495)->f2; goto _LL4522;} else{ goto _LL4523;} _LL4523: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Address_e){ _LL4564: _temp4563=(( struct Cyc_Absyn_Address_e_struct*)
_temp4495)->f1; goto _LL4524;} else{ goto _LL4525;} _LL4525: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Comprehension_e){ _LL4568: _temp4567=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4495)->f2; goto _LL4566; _LL4566: _temp4565=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4495)->f3; goto _LL4526;} else{ goto _LL4527;} _LL4527: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Array_e){ _LL4570: _temp4569=(( struct Cyc_Absyn_Array_e_struct*)
_temp4495)->f1; goto _LL4528;} else{ goto _LL4529;} _LL4529: if(*(( int*)
_temp4495) ==  Cyc_Absyn_AnonStruct_e){ _LL4572: _temp4571=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp4495)->f2; goto _LL4530;} else{ goto _LL4531;} _LL4531: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Struct_e){ _LL4574: _temp4573=(( struct Cyc_Absyn_Struct_e_struct*)
_temp4495)->f3; goto _LL4532;} else{ goto _LL4533;} _LL4533: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Primop_e){ _LL4578: _temp4577=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp4495)->f1; goto _LL4576; _LL4576: _temp4575=(( struct Cyc_Absyn_Primop_e_struct*)
_temp4495)->f2; goto _LL4534;} else{ goto _LL4535;} _LL4535: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Tuple_e){ _LL4580: _temp4579=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp4495)->f1; goto _LL4536;} else{ goto _LL4537;} _LL4537: if(*(( int*)
_temp4495) ==  Cyc_Absyn_Tunion_e){ _LL4582: _temp4581=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp4495)->f3; goto _LL4538;} else{ goto _LL4539;} _LL4539: goto _LL4540;
_LL4498: return 1; _LL4500: return 1; _LL4502: return 1; _LL4504: return 1;
_LL4506: return 1; _LL4508: return 1; _LL4510: return 1; _LL4512: { void*
_temp4583= _temp4541; struct Cyc_Absyn_Vardecl* _temp4593; _LL4585: if((
unsigned int) _temp4583 >  1u?*(( int*) _temp4583) ==  Cyc_Absyn_Funname_b: 0){
goto _LL4586;} else{ goto _LL4587;} _LL4587: if(( unsigned int) _temp4583 >  1u?*((
int*) _temp4583) ==  Cyc_Absyn_Global_b: 0){ _LL4594: _temp4593=(( struct Cyc_Absyn_Global_b_struct*)
_temp4583)->f1; goto _LL4588;} else{ goto _LL4589;} _LL4589: if( _temp4583 == (
void*) Cyc_Absyn_Unresolved_b){ goto _LL4590;} else{ goto _LL4591;} _LL4591:
goto _LL4592; _LL4586: return 1; _LL4588: { void* _temp4595= Cyc_Tcutil_compress((
void*) _temp4593->type); _LL4597: if(( unsigned int) _temp4595 >  3u?*(( int*)
_temp4595) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4598;} else{ goto _LL4599;}
_LL4599: if(( unsigned int) _temp4595 >  3u?*(( int*) _temp4595) ==  Cyc_Absyn_FnType:
0){ goto _LL4600;} else{ goto _LL4601;} _LL4601: goto _LL4602; _LL4598: return 1;
_LL4600: return 1; _LL4602: return var_okay; _LL4596:;} _LL4590: return 0;
_LL4592: return var_okay; _LL4584:;} _LL4514: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp4549)? Cyc_Tcutil_cnst_exp( te, 0, _temp4547): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp4545): 0; _LL4516: return Cyc_Tcutil_cnst_exp( te, 0, _temp4553)? Cyc_Tcutil_cnst_exp(
te, 0, _temp4551): 0; _LL4518: _temp4557= _temp4555; goto _LL4520; _LL4520:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp4557); _LL4522: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp4559); _LL4524: return Cyc_Tcutil_cnst_exp( te, 1, _temp4563);
_LL4526: return Cyc_Tcutil_cnst_exp( te, 0, _temp4567)? Cyc_Tcutil_cnst_exp( te,
0, _temp4565): 0; _LL4528: _temp4571= _temp4569; goto _LL4530; _LL4530:
_temp4573= _temp4571; goto _LL4532; _LL4532: for( 0; _temp4573 !=  0; _temp4573=
_temp4573->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple21*)
_temp4573->hd)).f2)){ return 0;}} return 1; _LL4534: _temp4579= _temp4575; goto
_LL4536; _LL4536: _temp4581= _temp4579; goto _LL4538; _LL4538: for( 0; _temp4581
!=  0; _temp4581= _temp4581->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp4581->hd)){ return 0;}} return 1; _LL4540: return 0; _LL4496:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp4603= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp4633; struct Cyc_Absyn_Conref* _temp4635; struct Cyc_Absyn_Conref*
_temp4637; void* _temp4639; struct Cyc_List_List* _temp4641; struct Cyc_Absyn_Structdecl**
_temp4643; struct Cyc_List_List* _temp4645; struct Cyc_Absyn_Uniondecl**
_temp4647; struct Cyc_List_List* _temp4649; struct Cyc_List_List* _temp4651;
struct Cyc_List_List* _temp4653; _LL4605: if( _temp4603 == ( void*) Cyc_Absyn_VoidType){
goto _LL4606;} else{ goto _LL4607;} _LL4607: if(( unsigned int) _temp4603 >  3u?*((
int*) _temp4603) ==  Cyc_Absyn_IntType: 0){ goto _LL4608;} else{ goto _LL4609;}
_LL4609: if( _temp4603 == ( void*) Cyc_Absyn_FloatType){ goto _LL4610;} else{
goto _LL4611;} _LL4611: if(( unsigned int) _temp4603 >  3u?*(( int*) _temp4603)
==  Cyc_Absyn_DoubleType: 0){ goto _LL4612;} else{ goto _LL4613;} _LL4613: if((
unsigned int) _temp4603 >  3u?*(( int*) _temp4603) ==  Cyc_Absyn_PointerType: 0){
_LL4634: _temp4633=(( struct Cyc_Absyn_PointerType_struct*) _temp4603)->f1;
_LL4638: _temp4637= _temp4633.nullable; goto _LL4636; _LL4636: _temp4635=
_temp4633.bounds; goto _LL4614;} else{ goto _LL4615;} _LL4615: if(( unsigned int)
_temp4603 >  3u?*(( int*) _temp4603) ==  Cyc_Absyn_ArrayType: 0){ _LL4640:
_temp4639=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp4603)->f1; goto
_LL4616;} else{ goto _LL4617;} _LL4617: if(( unsigned int) _temp4603 >  3u?*((
int*) _temp4603) ==  Cyc_Absyn_TupleType: 0){ _LL4642: _temp4641=(( struct Cyc_Absyn_TupleType_struct*)
_temp4603)->f1; goto _LL4618;} else{ goto _LL4619;} _LL4619: if(( unsigned int)
_temp4603 >  3u?*(( int*) _temp4603) ==  Cyc_Absyn_StructType: 0){ _LL4646:
_temp4645=(( struct Cyc_Absyn_StructType_struct*) _temp4603)->f2; goto _LL4644;
_LL4644: _temp4643=(( struct Cyc_Absyn_StructType_struct*) _temp4603)->f3; goto
_LL4620;} else{ goto _LL4621;} _LL4621: if(( unsigned int) _temp4603 >  3u?*((
int*) _temp4603) ==  Cyc_Absyn_UnionType: 0){ _LL4650: _temp4649=(( struct Cyc_Absyn_UnionType_struct*)
_temp4603)->f2; goto _LL4648; _LL4648: _temp4647=(( struct Cyc_Absyn_UnionType_struct*)
_temp4603)->f3; goto _LL4622;} else{ goto _LL4623;} _LL4623: if(( unsigned int)
_temp4603 >  3u?*(( int*) _temp4603) ==  Cyc_Absyn_AnonStructType: 0){ _LL4652:
_temp4651=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4603)->f1; goto
_LL4624;} else{ goto _LL4625;} _LL4625: if(( unsigned int) _temp4603 >  3u?*((
int*) _temp4603) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4654: _temp4653=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4603)->f1; goto _LL4626;} else{ goto
_LL4627;} _LL4627: if(( unsigned int) _temp4603 >  3u?*(( int*) _temp4603) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL4628;} else{ goto _LL4629;} _LL4629: if((
unsigned int) _temp4603 >  3u?*(( int*) _temp4603) ==  Cyc_Absyn_EnumType: 0){
goto _LL4630;} else{ goto _LL4631;} _LL4631: goto _LL4632; _LL4606: goto _LL4608;
_LL4608: goto _LL4610; _LL4610: goto _LL4612; _LL4612: return 1; _LL4614: { void*
_temp4655=( void*)( Cyc_Absyn_compress_conref( _temp4635))->v; void* _temp4663;
void* _temp4665; _LL4657: if(( unsigned int) _temp4655 >  1u?*(( int*) _temp4655)
==  Cyc_Absyn_Eq_constr: 0){ _LL4664: _temp4663=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4655)->f1; if( _temp4663 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4658;}
else{ goto _LL4659;}} else{ goto _LL4659;} _LL4659: if(( unsigned int) _temp4655
>  1u?*(( int*) _temp4655) ==  Cyc_Absyn_Eq_constr: 0){ _LL4666: _temp4665=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4655)->f1; if(( unsigned int)
_temp4665 >  1u?*(( int*) _temp4665) ==  Cyc_Absyn_Upper_b: 0){ goto _LL4660;}
else{ goto _LL4661;}} else{ goto _LL4661;} _LL4661: goto _LL4662; _LL4658:
return 1; _LL4660: { void* _temp4667=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp4637))->v; int
_temp4673; _LL4669: if(( unsigned int) _temp4667 >  1u?*(( int*) _temp4667) == 
Cyc_Absyn_Eq_constr: 0){ _LL4674: _temp4673=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4667)->f1; goto _LL4670;} else{ goto _LL4671;} _LL4671: goto _LL4672;
_LL4670: return _temp4673; _LL4672: return 0; _LL4668:;} _LL4662: return 0;
_LL4656:;} _LL4616: return Cyc_Tcutil_supports_default( _temp4639); _LL4618:
for( 0; _temp4641 !=  0; _temp4641= _temp4641->tl){ if( ! Cyc_Tcutil_supports_default((*((
struct _tuple4*) _temp4641->hd)).f2)){ return 0;}} return 1; _LL4620: if(
_temp4643 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=* _temp4643; if(
sd->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs,
_temp4645,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);}
_LL4622: if( _temp4647 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*
_temp4647; if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp4649,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL4624: _temp4653= _temp4651; goto _LL4626; _LL4626: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp4653); _LL4628: goto _LL4630;
_LL4630: return 1; _LL4632: return 0; _LL4604:;} static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List* tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{
struct _RegionHandle _temp4675= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp4675; _push_region( rgn);{ struct Cyc_List_List* _temp4676=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0;
fs= fs->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4676,( void*)(( struct
Cyc_Absyn_Structfield*) fs->hd)->type); if( ! Cyc_Tcutil_supports_default( t)){
int _temp4677= 0; _npop_handler( 0u); return _temp4677;}}}; _pop_region( rgn);}
return 1;}
