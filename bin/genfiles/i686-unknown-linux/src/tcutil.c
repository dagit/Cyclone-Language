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
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_Invalid_argument[
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
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_vrprintf(
struct _RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static
const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int tag;
short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
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
x); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y); struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r1, struct _RegionHandle* r2,
struct Cyc_List_List* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(* cmp)(
void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; extern
struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
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
struct Cyc_Absyn_Tvar* _temp159; struct Cyc_Absyn_TunionInfo _temp161; void*
_temp163; struct Cyc_List_List* _temp165; void* _temp167; struct Cyc_Absyn_TunionFieldInfo
_temp169; struct Cyc_List_List* _temp171; void* _temp173; struct Cyc_Absyn_PtrInfo
_temp175; struct Cyc_Absyn_Conref* _temp177; struct Cyc_Absyn_Tqual _temp179;
struct Cyc_Absyn_Conref* _temp181; void* _temp183; void* _temp185; void*
_temp187; void* _temp189; int _temp191; struct Cyc_Absyn_Exp* _temp193; struct
Cyc_Absyn_Tqual _temp195; void* _temp197; struct Cyc_Absyn_FnInfo _temp199;
struct Cyc_List_List* _temp201; struct Cyc_List_List* _temp203; struct Cyc_Absyn_VarargInfo*
_temp205; int _temp207; struct Cyc_List_List* _temp209; void* _temp211; struct
Cyc_Core_Opt* _temp213; struct Cyc_List_List* _temp215; struct Cyc_List_List*
_temp217; struct Cyc_List_List* _temp219; struct _tuple1* _temp221; struct Cyc_List_List*
_temp223; struct _tuple1* _temp225; struct Cyc_List_List* _temp227; struct Cyc_List_List*
_temp229; struct Cyc_Absyn_Enumdecl* _temp231; struct _tuple1* _temp233; struct
Cyc_List_List* _temp235; void* _temp237; void* _temp239; struct Cyc_List_List*
_temp241; struct _tuple1* _temp243; void* _temp245; struct Cyc_List_List*
_temp247; void* _temp249; _LL109: if( _temp107 == ( void*) Cyc_Absyn_VoidType){
goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_Evar: 0){ goto _LL112;} else{ goto _LL113;} _LL113:
if(( unsigned int) _temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_VarType: 0){
_LL160: _temp159=(( struct Cyc_Absyn_VarType_struct*) _temp107)->f1; goto _LL114;}
else{ goto _LL115;} _LL115: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107)
==  Cyc_Absyn_TunionType: 0){ _LL162: _temp161=(( struct Cyc_Absyn_TunionType_struct*)
_temp107)->f1; _LL168: _temp167=( void*) _temp161.tunion_info; goto _LL166;
_LL166: _temp165= _temp161.targs; goto _LL164; _LL164: _temp163=( void*)
_temp161.rgn; goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_TunionFieldType: 0){ _LL170:
_temp169=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp107)->f1; _LL174:
_temp173=( void*) _temp169.field_info; goto _LL172; _LL172: _temp171= _temp169.targs;
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_PointerType: 0){ _LL176: _temp175=(( struct Cyc_Absyn_PointerType_struct*)
_temp107)->f1; _LL186: _temp185=( void*) _temp175.elt_typ; goto _LL184; _LL184:
_temp183=( void*) _temp175.rgn_typ; goto _LL182; _LL182: _temp181= _temp175.nullable;
goto _LL180; _LL180: _temp179= _temp175.tq; goto _LL178; _LL178: _temp177=
_temp175.bounds; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_IntType: 0){ _LL190: _temp189=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp107)->f1; goto _LL188; _LL188:
_temp187=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp107)->f2; goto _LL122;}
else{ goto _LL123;} _LL123: if( _temp107 == ( void*) Cyc_Absyn_FloatType){ goto
_LL124;} else{ goto _LL125;} _LL125: if(( unsigned int) _temp107 >  3u?*(( int*)
_temp107) ==  Cyc_Absyn_DoubleType: 0){ _LL192: _temp191=(( struct Cyc_Absyn_DoubleType_struct*)
_temp107)->f1; goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_ArrayType: 0){ _LL198: _temp197=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp107)->f1; goto _LL196; _LL196:
_temp195=(( struct Cyc_Absyn_ArrayType_struct*) _temp107)->f2; goto _LL194;
_LL194: _temp193=(( struct Cyc_Absyn_ArrayType_struct*) _temp107)->f3; goto
_LL128;} else{ goto _LL129;} _LL129: if(( unsigned int) _temp107 >  3u?*(( int*)
_temp107) ==  Cyc_Absyn_FnType: 0){ _LL200: _temp199=(( struct Cyc_Absyn_FnType_struct*)
_temp107)->f1; _LL216: _temp215= _temp199.tvars; goto _LL214; _LL214: _temp213=
_temp199.effect; goto _LL212; _LL212: _temp211=( void*) _temp199.ret_typ; goto
_LL210; _LL210: _temp209= _temp199.args; goto _LL208; _LL208: _temp207= _temp199.c_varargs;
goto _LL206; _LL206: _temp205= _temp199.cyc_varargs; goto _LL204; _LL204:
_temp203= _temp199.rgn_po; goto _LL202; _LL202: _temp201= _temp199.attributes;
goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_TupleType: 0){ _LL218: _temp217=(( struct Cyc_Absyn_TupleType_struct*)
_temp107)->f1; goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_StructType: 0){ _LL222:
_temp221=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f1; goto _LL220;
_LL220: _temp219=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f2; goto
_LL134;} else{ goto _LL135;} _LL135: if(( unsigned int) _temp107 >  3u?*(( int*)
_temp107) ==  Cyc_Absyn_UnionType: 0){ _LL226: _temp225=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f1; goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f2; goto _LL136;} else{ goto _LL137;} _LL137: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_AnonStructType: 0){ _LL228:
_temp227=(( struct Cyc_Absyn_AnonStructType_struct*) _temp107)->f1; goto _LL138;}
else{ goto _LL139;} _LL139: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107)
==  Cyc_Absyn_AnonUnionType: 0){ _LL230: _temp229=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp107)->f1; goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_EnumType: 0){ _LL234: _temp233=((
struct Cyc_Absyn_EnumType_struct*) _temp107)->f1; goto _LL232; _LL232: _temp231=((
struct Cyc_Absyn_EnumType_struct*) _temp107)->f2; goto _LL142;} else{ goto
_LL143;} _LL143: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_AnonEnumType:
0){ _LL236: _temp235=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp107)->f1;
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_SizeofType: 0){ _LL238: _temp237=( void*)(( struct
Cyc_Absyn_SizeofType_struct*) _temp107)->f1; goto _LL146;} else{ goto _LL147;}
_LL147: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_RgnHandleType:
0){ _LL240: _temp239=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp107)->f1;
goto _LL148;} else{ goto _LL149;} _LL149: if(( unsigned int) _temp107 >  3u?*((
int*) _temp107) ==  Cyc_Absyn_TypedefType: 0){ _LL244: _temp243=(( struct Cyc_Absyn_TypedefType_struct*)
_temp107)->f1; goto _LL242; _LL242: _temp241=(( struct Cyc_Absyn_TypedefType_struct*)
_temp107)->f2; goto _LL150;} else{ goto _LL151;} _LL151: if( _temp107 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_AccessEff: 0){ _LL246: _temp245=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp107)->f1; goto _LL154;} else{
goto _LL155;} _LL155: if(( unsigned int) _temp107 >  3u?*(( int*) _temp107) == 
Cyc_Absyn_JoinEff: 0){ _LL248: _temp247=(( struct Cyc_Absyn_JoinEff_struct*)
_temp107)->f1; goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int)
_temp107 >  3u?*(( int*) _temp107) ==  Cyc_Absyn_RgnsEff: 0){ _LL250: _temp249=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp107)->f1; goto _LL158;} else{
goto _LL108;} _LL110: goto _LL112; _LL112: return t; _LL114: return( void*)({
struct Cyc_Absyn_VarType_struct* _temp251=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp251[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp252; _temp252.tag= Cyc_Absyn_VarType; _temp252.f1= Cyc_Tcutil_copy_tvar(
_temp159); _temp252;}); _temp251;}); _LL116: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp253=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp253[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp254; _temp254.tag= Cyc_Absyn_TunionType;
_temp254.f1=({ struct Cyc_Absyn_TunionInfo _temp255; _temp255.tunion_info=( void*)
_temp167; _temp255.targs= Cyc_Tcutil_copy_types( _temp165); _temp255.rgn=( void*)
Cyc_Tcutil_copy_type( _temp163); _temp255;}); _temp254;}); _temp253;}); _LL118:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp256=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp256[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp257; _temp257.tag= Cyc_Absyn_TunionFieldType;
_temp257.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp258; _temp258.field_info=(
void*) _temp173; _temp258.targs= Cyc_Tcutil_copy_types( _temp171); _temp258;});
_temp257;}); _temp256;}); _LL120: { void* _temp259= Cyc_Tcutil_copy_type(
_temp185); void* _temp260= Cyc_Tcutil_copy_type( _temp183); struct Cyc_Absyn_Conref*
_temp261=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp181); struct Cyc_Absyn_Tqual _temp262= _temp179; struct Cyc_Absyn_Conref*
_temp263= Cyc_Tcutil_copy_conref( _temp177); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp264=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp264[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp265; _temp265.tag= Cyc_Absyn_PointerType;
_temp265.f1=({ struct Cyc_Absyn_PtrInfo _temp266; _temp266.elt_typ=( void*)
_temp259; _temp266.rgn_typ=( void*) _temp260; _temp266.nullable= _temp261;
_temp266.tq= _temp262; _temp266.bounds= _temp263; _temp266;}); _temp265;});
_temp264;});} _LL122: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp267=(
struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp267[ 0]=({ struct Cyc_Absyn_IntType_struct _temp268; _temp268.tag= Cyc_Absyn_IntType;
_temp268.f1=( void*) _temp189; _temp268.f2=( void*) _temp187; _temp268;});
_temp267;}); _LL124: return t; _LL126: return( void*)({ struct Cyc_Absyn_DoubleType_struct*
_temp269=( struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_DoubleType_struct)); _temp269[ 0]=({ struct Cyc_Absyn_DoubleType_struct
_temp270; _temp270.tag= Cyc_Absyn_DoubleType; _temp270.f1= _temp191; _temp270;});
_temp269;}); _LL128: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp271=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp271[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp272; _temp272.tag= Cyc_Absyn_ArrayType;
_temp272.f1=( void*) Cyc_Tcutil_copy_type( _temp197); _temp272.f2= _temp195;
_temp272.f3= _temp193; _temp272;}); _temp271;}); _LL130: { struct Cyc_List_List*
_temp273=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp215); struct
Cyc_Core_Opt* _temp274= _temp213 ==  0? 0:({ struct Cyc_Core_Opt* _temp284=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp284->v=(
void*) Cyc_Tcutil_copy_type(( void*) _temp213->v); _temp284;}); void* _temp275=
Cyc_Tcutil_copy_type( _temp211); struct Cyc_List_List* _temp276=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_copy_arg, _temp209); int _temp277= _temp207; struct Cyc_Absyn_VarargInfo*
cyc_varargs2= 0; if( _temp205 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp205); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp278=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp278->name= cv->name; _temp278->tq= cv->tq; _temp278->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp278->inject= cv->inject; _temp278;});}{ struct Cyc_List_List*
_temp279=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp203);
struct Cyc_List_List* _temp280= _temp201; return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp281=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp281[ 0]=({ struct Cyc_Absyn_FnType_struct _temp282; _temp282.tag= Cyc_Absyn_FnType;
_temp282.f1=({ struct Cyc_Absyn_FnInfo _temp283; _temp283.tvars= _temp273;
_temp283.effect= _temp274; _temp283.ret_typ=( void*) _temp275; _temp283.args=
_temp276; _temp283.c_varargs= _temp277; _temp283.cyc_varargs= cyc_varargs2;
_temp283.rgn_po= _temp279; _temp283.attributes= _temp280; _temp283;}); _temp282;});
_temp281;});}} _LL132: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp285=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp285[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp286; _temp286.tag= Cyc_Absyn_TupleType;
_temp286.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp217);
_temp286;}); _temp285;}); _LL134: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp287=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp287[ 0]=({ struct Cyc_Absyn_StructType_struct _temp288; _temp288.tag= Cyc_Absyn_StructType;
_temp288.f1= _temp221; _temp288.f2= Cyc_Tcutil_copy_types( _temp219); _temp288.f3=
0; _temp288;}); _temp287;}); _LL136: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp289=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp289[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp290; _temp290.tag= Cyc_Absyn_UnionType;
_temp290.f1= _temp225; _temp290.f2= Cyc_Tcutil_copy_types( _temp223); _temp290.f3=
0; _temp290;}); _temp289;}); _LL138: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp291=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp291[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp292; _temp292.tag= Cyc_Absyn_AnonStructType; _temp292.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp227); _temp292;}); _temp291;});
_LL140: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp293=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp293[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp294; _temp294.tag=
Cyc_Absyn_AnonUnionType; _temp294.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp229); _temp294;}); _temp293;}); _LL142: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp295=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp295[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp296; _temp296.tag= Cyc_Absyn_EnumType;
_temp296.f1= _temp233; _temp296.f2= _temp231; _temp296;}); _temp295;}); _LL144:
return( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp297=( struct Cyc_Absyn_AnonEnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct)); _temp297[ 0]=({
struct Cyc_Absyn_AnonEnumType_struct _temp298; _temp298.tag= Cyc_Absyn_AnonEnumType;
_temp298.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Enumfield*(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_enumfield,
_temp235); _temp298;}); _temp297;}); _LL146: return( void*)({ struct Cyc_Absyn_SizeofType_struct*
_temp299=( struct Cyc_Absyn_SizeofType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct));
_temp299[ 0]=({ struct Cyc_Absyn_SizeofType_struct _temp300; _temp300.tag= Cyc_Absyn_SizeofType;
_temp300.f1=( void*) Cyc_Tcutil_copy_type( _temp237); _temp300;}); _temp299;});
_LL148: return( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp301=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp301[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp302; _temp302.tag=
Cyc_Absyn_RgnHandleType; _temp302.f1=( void*) Cyc_Tcutil_copy_type( _temp239);
_temp302;}); _temp301;}); _LL150: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp303=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp303[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp304; _temp304.tag= Cyc_Absyn_TypedefType;
_temp304.f1= _temp243; _temp304.f2= Cyc_Tcutil_copy_types( _temp241); _temp304.f3=
0; _temp304;}); _temp303;}); _LL152: return t; _LL154: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp305=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp305[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp306; _temp306.tag= Cyc_Absyn_AccessEff; _temp306.f1=(
void*) Cyc_Tcutil_copy_type( _temp245); _temp306;}); _temp305;}); _LL156: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp307=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp307[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp308; _temp308.tag= Cyc_Absyn_JoinEff; _temp308.f1= Cyc_Tcutil_copy_types(
_temp247); _temp308;}); _temp307;}); _LL158: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp309=( struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp309[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp310; _temp310.tag= Cyc_Absyn_RgnsEff;
_temp310.f1=( void*) Cyc_Tcutil_copy_type( _temp249); _temp310;}); _temp309;});
_LL108:;} int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){ return 1;}{
struct _tuple8 _temp312=({ struct _tuple8 _temp311; _temp311.f1= k1; _temp311.f2=
k2; _temp311;}); void* _temp322; void* _temp324; void* _temp326; void* _temp328;
void* _temp330; void* _temp332; _LL314: _LL325: _temp324= _temp312.f1; if(
_temp324 == ( void*) Cyc_Absyn_BoxKind){ goto _LL323;} else{ goto _LL316;}
_LL323: _temp322= _temp312.f2; if( _temp322 == ( void*) Cyc_Absyn_MemKind){ goto
_LL315;} else{ goto _LL316;} _LL316: _LL329: _temp328= _temp312.f1; if( _temp328
== ( void*) Cyc_Absyn_BoxKind){ goto _LL327;} else{ goto _LL318;} _LL327:
_temp326= _temp312.f2; if( _temp326 == ( void*) Cyc_Absyn_AnyKind){ goto _LL317;}
else{ goto _LL318;} _LL318: _LL333: _temp332= _temp312.f1; if( _temp332 == (
void*) Cyc_Absyn_MemKind){ goto _LL331;} else{ goto _LL320;} _LL331: _temp330=
_temp312.f2; if( _temp330 == ( void*) Cyc_Absyn_AnyKind){ goto _LL319;} else{
goto _LL320;} _LL320: goto _LL321; _LL315: goto _LL317; _LL317: goto _LL319;
_LL319: return 1; _LL321: return 0; _LL313:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ void* _temp334= Cyc_Absyn_compress_kb(( void*) tv->kind);
void* _temp342; void* _temp344; _LL336: if(*(( int*) _temp334) ==  Cyc_Absyn_Eq_kb){
_LL343: _temp342=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp334)->f1; goto
_LL337;} else{ goto _LL338;} _LL338: if(*(( int*) _temp334) ==  Cyc_Absyn_Less_kb){
_LL345: _temp344=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp334)->f2;
goto _LL339;} else{ goto _LL340;} _LL340: goto _LL341; _LL337: return _temp342;
_LL339: return _temp344; _LL341: return({ void* _temp346[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("kind not suitably constrained!", sizeof( unsigned char), 31u),
_tag_arr( _temp346, sizeof( void*), 0u));}); _LL335:;} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp347= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp407;
struct Cyc_Core_Opt* _temp409; struct Cyc_Absyn_Tvar* _temp411; void* _temp413;
struct Cyc_Absyn_TunionFieldInfo _temp415; void* _temp417; struct Cyc_Absyn_Tunionfield*
_temp419; struct Cyc_Absyn_TunionFieldInfo _temp421; void* _temp423; struct Cyc_Absyn_Structdecl**
_temp425; struct Cyc_Absyn_Uniondecl** _temp427; struct Cyc_Absyn_Enumdecl*
_temp429; struct Cyc_Absyn_Enumdecl* _temp431; struct Cyc_Absyn_PtrInfo _temp433;
struct Cyc_Core_Opt* _temp435; _LL349: if(( unsigned int) _temp347 >  3u?*(( int*)
_temp347) ==  Cyc_Absyn_Evar: 0){ _LL410: _temp409=(( struct Cyc_Absyn_Evar_struct*)
_temp347)->f1; goto _LL408; _LL408: _temp407=(( struct Cyc_Absyn_Evar_struct*)
_temp347)->f2; goto _LL350;} else{ goto _LL351;} _LL351: if(( unsigned int)
_temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_VarType: 0){ _LL412: _temp411=((
struct Cyc_Absyn_VarType_struct*) _temp347)->f1; goto _LL352;} else{ goto _LL353;}
_LL353: if( _temp347 == ( void*) Cyc_Absyn_VoidType){ goto _LL354;} else{ goto
_LL355;} _LL355: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_IntType:
0){ _LL414: _temp413=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp347)->f2;
goto _LL356;} else{ goto _LL357;} _LL357: if( _temp347 == ( void*) Cyc_Absyn_FloatType){
goto _LL358;} else{ goto _LL359;} _LL359: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_DoubleType: 0){ goto _LL360;} else{ goto _LL361;}
_LL361: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_FnType:
0){ goto _LL362;} else{ goto _LL363;} _LL363: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL364;} else{ goto _LL365;}
_LL365: if( _temp347 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL366;} else{ goto
_LL367;} _LL367: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_TunionType:
0){ goto _LL368;} else{ goto _LL369;} _LL369: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_TunionFieldType: 0){ _LL416: _temp415=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp347)->f1; _LL418: _temp417=( void*)
_temp415.field_info; if(*(( int*) _temp417) ==  Cyc_Absyn_KnownTunionfield){
_LL420: _temp419=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp417)->f2;
goto _LL370;} else{ goto _LL371;}} else{ goto _LL371;} _LL371: if(( unsigned int)
_temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_TunionFieldType: 0){ _LL422:
_temp421=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp347)->f1; _LL424:
_temp423=( void*) _temp421.field_info; if(*(( int*) _temp423) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL372;} else{ goto _LL373;}} else{ goto _LL373;} _LL373: if(( unsigned int)
_temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_StructType: 0){ _LL426:
_temp425=(( struct Cyc_Absyn_StructType_struct*) _temp347)->f3; if( _temp425 == 
0){ goto _LL374;} else{ goto _LL375;}} else{ goto _LL375;} _LL375: if((
unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_UnionType: 0){
_LL428: _temp427=(( struct Cyc_Absyn_UnionType_struct*) _temp347)->f3; if(
_temp427 ==  0){ goto _LL376;} else{ goto _LL377;}} else{ goto _LL377;} _LL377:
if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_EnumType: 0){
_LL430: _temp429=(( struct Cyc_Absyn_EnumType_struct*) _temp347)->f2; if(
_temp429 ==  0){ goto _LL378;} else{ goto _LL379;}} else{ goto _LL379;} _LL379:
if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_StructType:
0){ goto _LL380;} else{ goto _LL381;} _LL381: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_UnionType: 0){ goto _LL382;} else{ goto _LL383;}
_LL383: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL384;} else{ goto _LL385;} _LL385: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL386;} else{ goto _LL387;}
_LL387: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL388;} else{ goto _LL389;} _LL389: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_EnumType: 0){ _LL432: _temp431=(( struct Cyc_Absyn_EnumType_struct*)
_temp347)->f2; goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int)
_temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_PointerType: 0){ _LL434:
_temp433=(( struct Cyc_Absyn_PointerType_struct*) _temp347)->f1; goto _LL392;}
else{ goto _LL393;} _LL393: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347)
==  Cyc_Absyn_SizeofType: 0){ goto _LL394;} else{ goto _LL395;} _LL395: if((
unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_ArrayType: 0){
goto _LL396;} else{ goto _LL397;} _LL397: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_TupleType: 0){ goto _LL398;} else{ goto _LL399;}
_LL399: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_TypedefType:
0){ _LL436: _temp435=(( struct Cyc_Absyn_TypedefType_struct*) _temp347)->f3;
goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_AccessEff: 0){ goto _LL402;} else{ goto _LL403;}
_LL403: if(( unsigned int) _temp347 >  3u?*(( int*) _temp347) ==  Cyc_Absyn_JoinEff:
0){ goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int) _temp347 >  3u?*((
int*) _temp347) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL406;} else{ goto _LL348;}
_LL350: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp409))->v;
_LL352: return Cyc_Tcutil_tvar_kind( _temp411); _LL354: return( void*) Cyc_Absyn_MemKind;
_LL356: return _temp413 == ( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL358: goto _LL360; _LL360: goto _LL362; _LL362:
return( void*) Cyc_Absyn_MemKind; _LL364: return( void*) Cyc_Absyn_BoxKind;
_LL366: return( void*) Cyc_Absyn_RgnKind; _LL368: return( void*) Cyc_Absyn_BoxKind;
_LL370: if( _temp419->typs ==  0){ return( void*) Cyc_Absyn_BoxKind;} else{
return( void*) Cyc_Absyn_MemKind;} _LL372: return({ void* _temp437[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("typ_kind: Unresolved TunionFieldType", sizeof( unsigned char), 37u),
_tag_arr( _temp437, sizeof( void*), 0u));}); _LL374: goto _LL376; _LL376: goto
_LL378; _LL378: return( void*) Cyc_Absyn_AnyKind; _LL380: goto _LL382; _LL382:
goto _LL384; _LL384: goto _LL386; _LL386: goto _LL388; _LL388: return( void*)
Cyc_Absyn_MemKind; _LL390: if( _temp431->fields ==  0){ return( void*) Cyc_Absyn_AnyKind;}
else{ return( void*) Cyc_Absyn_MemKind;} _LL392: { void* _temp438=( void*)( Cyc_Absyn_compress_conref(
_temp433.bounds))->v; void* _temp448; void* _temp450; _LL440: if(( unsigned int)
_temp438 >  1u?*(( int*) _temp438) ==  Cyc_Absyn_Eq_constr: 0){ _LL449: _temp448=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp438)->f1; if( _temp448 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL441;} else{ goto _LL442;}} else{ goto
_LL442;} _LL442: if(( unsigned int) _temp438 >  1u?*(( int*) _temp438) ==  Cyc_Absyn_Eq_constr:
0){ _LL451: _temp450=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp438)->f1;
if(( unsigned int) _temp450 >  1u?*(( int*) _temp450) ==  Cyc_Absyn_Upper_b: 0){
goto _LL443;} else{ goto _LL444;}} else{ goto _LL444;} _LL444: if( _temp438 == (
void*) Cyc_Absyn_No_constr){ goto _LL445;} else{ goto _LL446;} _LL446: if((
unsigned int) _temp438 >  1u?*(( int*) _temp438) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL447;} else{ goto _LL439;} _LL441: return( void*) Cyc_Absyn_MemKind;
_LL443: return( void*) Cyc_Absyn_BoxKind; _LL445: return( void*) Cyc_Absyn_MemKind;
_LL447: return({ void* _temp452[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof( unsigned char), 43u), _tag_arr( _temp452, sizeof( void*), 0u));});
_LL439:;} _LL394: return( void*) Cyc_Absyn_BoxKind; _LL396: goto _LL398; _LL398:
return( void*) Cyc_Absyn_MemKind; _LL400: return({ struct Cyc_Std_String_pa_struct
_temp454; _temp454.tag= Cyc_Std_String_pa; _temp454.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp453[ 1u]={& _temp454}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s",
sizeof( unsigned char), 28u), _tag_arr( _temp453, sizeof( void*), 1u));}});
_LL402: goto _LL404; _LL404: goto _LL406; _LL406: return( void*) Cyc_Absyn_EffKind;
_LL348:;} unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp455; _push_handler(& _temp455);{
int _temp457= 0; if( setjmp( _temp455.handler)){ _temp457= 1;} if( ! _temp457){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp458= 1; _npop_handler( 0u); return
_temp458;}; _pop_handler();} else{ void* _temp456=( void*) _exn_thrown; void*
_temp460= _temp456; _LL462: if( _temp460 ==  Cyc_Tcutil_Unify){ goto _LL463;}
else{ goto _LL464;} _LL464: goto _LL465; _LL463: return 0; _LL465:( void) _throw(
_temp460); _LL461:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct
_RegionHandle* r, struct Cyc_List_List* env, struct Cyc_List_List* ts); static
void Cyc_Tcutil_occurs( void* evar, struct _RegionHandle* r, struct Cyc_List_List*
env, void* t){ void* _temp466= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp504; struct Cyc_Core_Opt* _temp506; struct Cyc_Core_Opt** _temp508; struct
Cyc_Core_Opt* _temp509; struct Cyc_Absyn_PtrInfo _temp511; void* _temp513;
struct Cyc_Absyn_FnInfo _temp515; struct Cyc_List_List* _temp517; struct Cyc_List_List*
_temp519; struct Cyc_Absyn_VarargInfo* _temp521; int _temp523; struct Cyc_List_List*
_temp525; void* _temp527; struct Cyc_Core_Opt* _temp529; struct Cyc_List_List*
_temp531; struct Cyc_List_List* _temp533; void* _temp535; struct Cyc_Absyn_TunionInfo
_temp537; void* _temp539; struct Cyc_List_List* _temp541; struct Cyc_Core_Opt*
_temp543; struct Cyc_List_List* _temp545; struct Cyc_Absyn_TunionFieldInfo
_temp547; struct Cyc_List_List* _temp549; struct Cyc_List_List* _temp551; struct
Cyc_List_List* _temp553; struct Cyc_List_List* _temp555; void* _temp557; void*
_temp559; struct Cyc_List_List* _temp561; void* _temp563; _LL468: if((
unsigned int) _temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_VarType: 0){
_LL505: _temp504=(( struct Cyc_Absyn_VarType_struct*) _temp466)->f1; goto _LL469;}
else{ goto _LL470;} _LL470: if(( unsigned int) _temp466 >  3u?*(( int*) _temp466)
==  Cyc_Absyn_Evar: 0){ _LL510: _temp509=(( struct Cyc_Absyn_Evar_struct*)
_temp466)->f2; goto _LL507; _LL507: _temp506=(( struct Cyc_Absyn_Evar_struct*)
_temp466)->f4; _temp508=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp466)->f4; goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int)
_temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_PointerType: 0){ _LL512:
_temp511=(( struct Cyc_Absyn_PointerType_struct*) _temp466)->f1; goto _LL473;}
else{ goto _LL474;} _LL474: if(( unsigned int) _temp466 >  3u?*(( int*) _temp466)
==  Cyc_Absyn_ArrayType: 0){ _LL514: _temp513=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp466)->f1; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_FnType: 0){ _LL516: _temp515=((
struct Cyc_Absyn_FnType_struct*) _temp466)->f1; _LL532: _temp531= _temp515.tvars;
goto _LL530; _LL530: _temp529= _temp515.effect; goto _LL528; _LL528: _temp527=(
void*) _temp515.ret_typ; goto _LL526; _LL526: _temp525= _temp515.args; goto
_LL524; _LL524: _temp523= _temp515.c_varargs; goto _LL522; _LL522: _temp521=
_temp515.cyc_varargs; goto _LL520; _LL520: _temp519= _temp515.rgn_po; goto
_LL518; _LL518: _temp517= _temp515.attributes; goto _LL477;} else{ goto _LL478;}
_LL478: if(( unsigned int) _temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_TupleType:
0){ _LL534: _temp533=(( struct Cyc_Absyn_TupleType_struct*) _temp466)->f1; goto
_LL479;} else{ goto _LL480;} _LL480: if(( unsigned int) _temp466 >  3u?*(( int*)
_temp466) ==  Cyc_Absyn_RgnHandleType: 0){ _LL536: _temp535=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp466)->f1; goto _LL481;} else{ goto _LL482;}
_LL482: if(( unsigned int) _temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_TunionType:
0){ _LL538: _temp537=(( struct Cyc_Absyn_TunionType_struct*) _temp466)->f1;
_LL542: _temp541= _temp537.targs; goto _LL540; _LL540: _temp539=( void*)
_temp537.rgn; goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int)
_temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_TypedefType: 0){ _LL546:
_temp545=(( struct Cyc_Absyn_TypedefType_struct*) _temp466)->f2; goto _LL544;
_LL544: _temp543=(( struct Cyc_Absyn_TypedefType_struct*) _temp466)->f3; goto
_LL485;} else{ goto _LL486;} _LL486: if(( unsigned int) _temp466 >  3u?*(( int*)
_temp466) ==  Cyc_Absyn_TunionFieldType: 0){ _LL548: _temp547=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp466)->f1; _LL550: _temp549= _temp547.targs; goto _LL487;} else{ goto _LL488;}
_LL488: if(( unsigned int) _temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_StructType:
0){ _LL552: _temp551=(( struct Cyc_Absyn_StructType_struct*) _temp466)->f2; goto
_LL489;} else{ goto _LL490;} _LL490: if(( unsigned int) _temp466 >  3u?*(( int*)
_temp466) ==  Cyc_Absyn_AnonStructType: 0){ _LL554: _temp553=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp466)->f1; goto _LL491;} else{ goto _LL492;} _LL492: if(( unsigned int)
_temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_AnonUnionType: 0){ _LL556:
_temp555=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp466)->f1; goto _LL493;}
else{ goto _LL494;} _LL494: if(( unsigned int) _temp466 >  3u?*(( int*) _temp466)
==  Cyc_Absyn_SizeofType: 0){ _LL558: _temp557=( void*)(( struct Cyc_Absyn_SizeofType_struct*)
_temp466)->f1; goto _LL495;} else{ goto _LL496;} _LL496: if(( unsigned int)
_temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_AccessEff: 0){ _LL560: _temp559=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp466)->f1; goto _LL497;} else{
goto _LL498;} _LL498: if(( unsigned int) _temp466 >  3u?*(( int*) _temp466) == 
Cyc_Absyn_JoinEff: 0){ _LL562: _temp561=(( struct Cyc_Absyn_JoinEff_struct*)
_temp466)->f1; goto _LL499;} else{ goto _LL500;} _LL500: if(( unsigned int)
_temp466 >  3u?*(( int*) _temp466) ==  Cyc_Absyn_RgnsEff: 0){ _LL564: _temp563=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp466)->f1; goto _LL501;} else{
goto _LL502;} _LL502: goto _LL503; _LL469: if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env, _temp504)){
Cyc_Tcutil_failure_reason= _tag_arr("(type variable would escape scope)",
sizeof( unsigned char), 35u);( int) _throw(( void*) Cyc_Tcutil_Unify);} goto
_LL467; _LL471: if( t ==  evar){ Cyc_Tcutil_failure_reason= _tag_arr("(occurs check)",
sizeof( unsigned char), 15u);( int) _throw(( void*) Cyc_Tcutil_Unify);} else{
if( _temp509 !=  0){ Cyc_Tcutil_occurs( evar, r, env,( void*) _temp509->v);}
else{ int problem= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null(* _temp508))->v; for( 0; s !=  0; s= s->tl){ if( !((
int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,
env,( struct Cyc_Absyn_Tvar*) s->hd)){ problem= 1; break;}}} if( problem){
struct Cyc_List_List* _temp565= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp508))->v; for( 0; s !=  0; s= s->tl){
if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,
env,( struct Cyc_Absyn_Tvar*) s->hd)){ _temp565=({ struct Cyc_List_List*
_temp566=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp566->hd=( void*)(( struct Cyc_Absyn_Tvar*) s->hd); _temp566->tl= _temp565;
_temp566;});}}}* _temp508=({ struct Cyc_Core_Opt* _temp567=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp567->v=( void*) _temp565;
_temp567;});}}} goto _LL467; _LL473: Cyc_Tcutil_occurs( evar, r, env,( void*)
_temp511.elt_typ); Cyc_Tcutil_occurs( evar, r, env,( void*) _temp511.rgn_typ);
goto _LL467; _LL475: Cyc_Tcutil_occurs( evar, r, env, _temp513); goto _LL467;
_LL477: env=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( r, _temp531, env); if( _temp529
!=  0){ Cyc_Tcutil_occurs( evar, r, env,( void*) _temp529->v);} Cyc_Tcutil_occurs(
evar, r, env, _temp527); for( 0; _temp525 !=  0; _temp525= _temp525->tl){ Cyc_Tcutil_occurs(
evar, r, env,(*(( struct _tuple2*) _temp525->hd)).f3);} if( _temp521 !=  0){
void* _temp570; struct Cyc_Absyn_VarargInfo _temp568=* _temp521; _LL571:
_temp570=( void*) _temp568.type; goto _LL569; _LL569: Cyc_Tcutil_occurs( evar, r,
env, _temp570);} for( 0; _temp519 !=  0; _temp519= _temp519->tl){ struct _tuple8
_temp574; void* _temp575; void* _temp577; struct _tuple8* _temp572=( struct
_tuple8*) _temp519->hd; _temp574=* _temp572; _LL578: _temp577= _temp574.f1; goto
_LL576; _LL576: _temp575= _temp574.f2; goto _LL573; _LL573: Cyc_Tcutil_occurs(
evar, r, env, _temp577); Cyc_Tcutil_occurs( evar, r, env, _temp575);} goto
_LL467; _LL479: for( 0; _temp533 !=  0; _temp533= _temp533->tl){ Cyc_Tcutil_occurs(
evar, r, env,(*(( struct _tuple4*) _temp533->hd)).f2);} goto _LL467; _LL481: Cyc_Tcutil_occurs(
evar, r, env, _temp535); goto _LL467; _LL483: Cyc_Tcutil_occurs( evar, r, env,
_temp539); Cyc_Tcutil_occurslist( evar, r, env, _temp541); goto _LL467; _LL485:
_temp549= _temp545; goto _LL487; _LL487: _temp551= _temp549; goto _LL489; _LL489:
Cyc_Tcutil_occurslist( evar, r, env, _temp551); goto _LL467; _LL491: _temp555=
_temp553; goto _LL493; _LL493: for( 0; _temp555 !=  0; _temp555= _temp555->tl){
Cyc_Tcutil_occurs( evar, r, env,( void*)(( struct Cyc_Absyn_Structfield*)
_temp555->hd)->type);} goto _LL467; _LL495: Cyc_Tcutil_occurs( evar, r, env,
_temp557); goto _LL467; _LL497: Cyc_Tcutil_occurs( evar, r, env, _temp559); goto
_LL467; _LL499: Cyc_Tcutil_occurslist( evar, r, env, _temp561); goto _LL467;
_LL501: Cyc_Tcutil_occurs( evar, r, env, _temp563); goto _LL467; _LL503: goto
_LL467; _LL467:;} static void Cyc_Tcutil_occurslist( void* evar, struct
_RegionHandle* r, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0;
ts !=  0; ts= ts->tl){ Cyc_Tcutil_occurs( evar, r, env,( void*) ts->hd);}}
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
y); if( x ==  y){ return;}{ void* _temp579=( void*) x->v; void* _temp587; _LL581:
if( _temp579 == ( void*) Cyc_Absyn_No_constr){ goto _LL582;} else{ goto _LL583;}
_LL583: if(( unsigned int) _temp579 >  1u?*(( int*) _temp579) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL584;} else{ goto _LL585;} _LL585: if(( unsigned int) _temp579 >  1u?*((
int*) _temp579) ==  Cyc_Absyn_Eq_constr: 0){ _LL588: _temp587=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp579)->f1; goto _LL586;} else{ goto _LL580;}
_LL582:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp589=( struct Cyc_Absyn_Forward_constr_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp589[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp590; _temp590.tag= Cyc_Absyn_Forward_constr; _temp590.f1= y; _temp590;});
_temp589;}))); return; _LL584:( int) _throw(({ void* _temp591[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp591, sizeof( void*), 0u));})); _LL586: { void* _temp592=( void*)
y->v; void* _temp600; _LL594: if( _temp592 == ( void*) Cyc_Absyn_No_constr){
goto _LL595;} else{ goto _LL596;} _LL596: if(( unsigned int) _temp592 >  1u?*((
int*) _temp592) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL597;} else{ goto
_LL598;} _LL598: if(( unsigned int) _temp592 >  1u?*(( int*) _temp592) ==  Cyc_Absyn_Eq_constr:
0){ _LL601: _temp600=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp592)->f1;
goto _LL599;} else{ goto _LL593;} _LL595:( void*)( y->v=( void*)(( void*) x->v));
return; _LL597:( int) _throw(({ void* _temp602[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp602, sizeof( void*), 0u));})); _LL599: if( cmp( _temp587,
_temp600) !=  0){ Cyc_Tcutil_failure_reason= reason;( int) _throw(( void*) Cyc_Tcutil_Unify);}
return; _LL593:;} _LL580:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp603; _push_handler(& _temp603);{ int _temp605= 0; if( setjmp(
_temp603.handler)){ _temp605= 1;} if( ! _temp605){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u));{ int _temp606= 1;
_npop_handler( 0u); return _temp606;}; _pop_handler();} else{ void* _temp604=(
void*) _exn_thrown; void* _temp608= _temp604; _LL610: if( _temp608 ==  Cyc_Tcutil_Unify){
goto _LL611;} else{ goto _LL612;} _LL612: goto _LL613; _LL611: return 0; _LL613:(
void) _throw( _temp608); _LL609:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp615=({ struct _tuple8 _temp614; _temp614.f1= b1;
_temp614.f2= b2; _temp614;}); void* _temp625; void* _temp627; void* _temp629;
void* _temp631; void* _temp633; struct Cyc_Absyn_Exp* _temp635; void* _temp637;
struct Cyc_Absyn_Exp* _temp639; _LL617: _LL628: _temp627= _temp615.f1; if(
_temp627 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL626;} else{ goto _LL619;}
_LL626: _temp625= _temp615.f2; if( _temp625 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL618;} else{ goto _LL619;} _LL619: _LL630: _temp629= _temp615.f1; if(
_temp629 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL620;} else{ goto _LL621;}
_LL621: _LL632: _temp631= _temp615.f2; if( _temp631 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL622;} else{ goto _LL623;} _LL623: _LL638: _temp637= _temp615.f1; if((
unsigned int) _temp637 >  1u?*(( int*) _temp637) ==  Cyc_Absyn_Upper_b: 0){
_LL640: _temp639=(( struct Cyc_Absyn_Upper_b_struct*) _temp637)->f1; goto _LL634;}
else{ goto _LL616;} _LL634: _temp633= _temp615.f2; if(( unsigned int) _temp633 > 
1u?*(( int*) _temp633) ==  Cyc_Absyn_Upper_b: 0){ _LL636: _temp635=(( struct Cyc_Absyn_Upper_b_struct*)
_temp633)->f1; goto _LL624;} else{ goto _LL616;} _LL618: return 0; _LL620:
return - 1; _LL622: return 1; _LL624: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp639); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp635); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL616:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp642=({
struct _tuple8 _temp641; _temp641.f1= a1; _temp641.f2= a2; _temp641;}); void*
_temp654; int _temp656; int _temp658; void* _temp660; void* _temp662; int
_temp664; int _temp666; void* _temp668; void* _temp670; int _temp672; void*
_temp674; int _temp676; void* _temp678; int _temp680; void* _temp682; int
_temp684; void* _temp686; struct _tagged_arr _temp688; void* _temp690; struct
_tagged_arr _temp692; _LL644: _LL663: _temp662= _temp642.f1; if(( unsigned int)
_temp662 >  16u?*(( int*) _temp662) ==  Cyc_Absyn_Format_att: 0){ _LL669:
_temp668=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp662)->f1; goto
_LL667; _LL667: _temp666=(( struct Cyc_Absyn_Format_att_struct*) _temp662)->f2;
goto _LL665; _LL665: _temp664=(( struct Cyc_Absyn_Format_att_struct*) _temp662)->f3;
goto _LL655;} else{ goto _LL646;} _LL655: _temp654= _temp642.f2; if((
unsigned int) _temp654 >  16u?*(( int*) _temp654) ==  Cyc_Absyn_Format_att: 0){
_LL661: _temp660=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp654)->f1;
goto _LL659; _LL659: _temp658=(( struct Cyc_Absyn_Format_att_struct*) _temp654)->f2;
goto _LL657; _LL657: _temp656=(( struct Cyc_Absyn_Format_att_struct*) _temp654)->f3;
goto _LL645;} else{ goto _LL646;} _LL646: _LL675: _temp674= _temp642.f1; if((
unsigned int) _temp674 >  16u?*(( int*) _temp674) ==  Cyc_Absyn_Regparm_att: 0){
_LL677: _temp676=(( struct Cyc_Absyn_Regparm_att_struct*) _temp674)->f1; goto
_LL671;} else{ goto _LL648;} _LL671: _temp670= _temp642.f2; if(( unsigned int)
_temp670 >  16u?*(( int*) _temp670) ==  Cyc_Absyn_Regparm_att: 0){ _LL673:
_temp672=(( struct Cyc_Absyn_Regparm_att_struct*) _temp670)->f1; goto _LL647;}
else{ goto _LL648;} _LL648: _LL683: _temp682= _temp642.f1; if(( unsigned int)
_temp682 >  16u?*(( int*) _temp682) ==  Cyc_Absyn_Aligned_att: 0){ _LL685:
_temp684=(( struct Cyc_Absyn_Aligned_att_struct*) _temp682)->f1; goto _LL679;}
else{ goto _LL650;} _LL679: _temp678= _temp642.f2; if(( unsigned int) _temp678 > 
16u?*(( int*) _temp678) ==  Cyc_Absyn_Aligned_att: 0){ _LL681: _temp680=((
struct Cyc_Absyn_Aligned_att_struct*) _temp678)->f1; goto _LL649;} else{ goto
_LL650;} _LL650: _LL691: _temp690= _temp642.f1; if(( unsigned int) _temp690 > 
16u?*(( int*) _temp690) ==  Cyc_Absyn_Section_att: 0){ _LL693: _temp692=((
struct Cyc_Absyn_Section_att_struct*) _temp690)->f1; goto _LL687;} else{ goto
_LL652;} _LL687: _temp686= _temp642.f2; if(( unsigned int) _temp686 >  16u?*((
int*) _temp686) ==  Cyc_Absyn_Section_att: 0){ _LL689: _temp688=(( struct Cyc_Absyn_Section_att_struct*)
_temp686)->f1; goto _LL651;} else{ goto _LL652;} _LL652: goto _LL653; _LL645:
return( _temp668 ==  _temp660? _temp666 ==  _temp658: 0)? _temp664 ==  _temp656:
0; _LL647: _temp684= _temp676; _temp680= _temp672; goto _LL649; _LL649: return
_temp684 ==  _temp680; _LL651: return Cyc_Std_strcmp( _temp692, _temp688) ==  0;
_LL653: return 0; _LL643:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a= a->tl){
if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*) a->hd, a2)){ return 0;}}}{
struct Cyc_List_List* a= a2; for( 0; a !=  0; a= a->tl){ if( ! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,( void*) a->hd, a1)){ return 0;}}} return 1;} static void*
Cyc_Tcutil_normalize_effect( void* e); static void* Cyc_Tcutil_rgns_of( void* t);
static void* Cyc_Tcutil_rgns_of_field( struct Cyc_Absyn_Structfield* sf){ return
Cyc_Tcutil_rgns_of(( void*) sf->type);} static struct _tuple5* Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar* tv){ void* t;{ void* _temp694= Cyc_Tcutil_tvar_kind( tv);
_LL696: if( _temp694 == ( void*) Cyc_Absyn_RgnKind){ goto _LL697;} else{ goto
_LL698;} _LL698: if( _temp694 == ( void*) Cyc_Absyn_EffKind){ goto _LL699;}
else{ goto _LL700;} _LL700: goto _LL701; _LL697: t=( void*) Cyc_Absyn_HeapRgn;
goto _LL695; _LL699: t= Cyc_Absyn_empty_effect; goto _LL695; _LL701: t= Cyc_Absyn_sint_t;
goto _LL695; _LL695:;} return({ struct _tuple5* _temp702=( struct _tuple5*)
_cycalloc( sizeof( struct _tuple5)); _temp702->f1= tv; _temp702->f2= t; _temp702;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp703= Cyc_Tcutil_compress(
t); void* _temp755; struct Cyc_Absyn_TunionInfo _temp757; void* _temp759; struct
Cyc_List_List* _temp761; struct Cyc_Absyn_PtrInfo _temp763; void* _temp765; void*
_temp767; void* _temp769; struct Cyc_List_List* _temp771; struct Cyc_Absyn_TunionFieldInfo
_temp773; struct Cyc_List_List* _temp775; struct Cyc_List_List* _temp777; struct
Cyc_List_List* _temp779; struct Cyc_List_List* _temp781; struct Cyc_List_List*
_temp783; void* _temp785; struct Cyc_Absyn_FnInfo _temp787; struct Cyc_List_List*
_temp789; struct Cyc_Absyn_VarargInfo* _temp791; struct Cyc_List_List* _temp793;
void* _temp795; struct Cyc_Core_Opt* _temp797; struct Cyc_List_List* _temp799;
void* _temp801; _LL705: if( _temp703 == ( void*) Cyc_Absyn_VoidType){ goto
_LL706;} else{ goto _LL707;} _LL707: if( _temp703 == ( void*) Cyc_Absyn_FloatType){
goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int) _temp703 >  3u?*((
int*) _temp703) ==  Cyc_Absyn_DoubleType: 0){ goto _LL710;} else{ goto _LL711;}
_LL711: if(( unsigned int) _temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_EnumType:
0){ goto _LL712;} else{ goto _LL713;} _LL713: if(( unsigned int) _temp703 >  3u?*((
int*) _temp703) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL714;} else{ goto _LL715;}
_LL715: if(( unsigned int) _temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_IntType:
0){ goto _LL716;} else{ goto _LL717;} _LL717: if(( unsigned int) _temp703 >  3u?*((
int*) _temp703) ==  Cyc_Absyn_Evar: 0){ goto _LL718;} else{ goto _LL719;} _LL719:
if(( unsigned int) _temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_VarType: 0){
goto _LL720;} else{ goto _LL721;} _LL721: if(( unsigned int) _temp703 >  3u?*((
int*) _temp703) ==  Cyc_Absyn_RgnHandleType: 0){ _LL756: _temp755=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp703)->f1; goto _LL722;} else{ goto
_LL723;} _LL723: if(( unsigned int) _temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_TunionType:
0){ _LL758: _temp757=(( struct Cyc_Absyn_TunionType_struct*) _temp703)->f1;
_LL762: _temp761= _temp757.targs; goto _LL760; _LL760: _temp759=( void*)
_temp757.rgn; goto _LL724;} else{ goto _LL725;} _LL725: if(( unsigned int)
_temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_PointerType: 0){ _LL764:
_temp763=(( struct Cyc_Absyn_PointerType_struct*) _temp703)->f1; _LL768:
_temp767=( void*) _temp763.elt_typ; goto _LL766; _LL766: _temp765=( void*)
_temp763.rgn_typ; goto _LL726;} else{ goto _LL727;} _LL727: if(( unsigned int)
_temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_ArrayType: 0){ _LL770: _temp769=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp703)->f1; goto _LL728;} else{
goto _LL729;} _LL729: if(( unsigned int) _temp703 >  3u?*(( int*) _temp703) == 
Cyc_Absyn_TupleType: 0){ _LL772: _temp771=(( struct Cyc_Absyn_TupleType_struct*)
_temp703)->f1; goto _LL730;} else{ goto _LL731;} _LL731: if(( unsigned int)
_temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_TunionFieldType: 0){ _LL774:
_temp773=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp703)->f1; _LL776:
_temp775= _temp773.targs; goto _LL732;} else{ goto _LL733;} _LL733: if((
unsigned int) _temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_StructType: 0){
_LL778: _temp777=(( struct Cyc_Absyn_StructType_struct*) _temp703)->f2; goto
_LL734;} else{ goto _LL735;} _LL735: if(( unsigned int) _temp703 >  3u?*(( int*)
_temp703) ==  Cyc_Absyn_UnionType: 0){ _LL780: _temp779=(( struct Cyc_Absyn_UnionType_struct*)
_temp703)->f2; goto _LL736;} else{ goto _LL737;} _LL737: if(( unsigned int)
_temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_AnonStructType: 0){ _LL782:
_temp781=(( struct Cyc_Absyn_AnonStructType_struct*) _temp703)->f1; goto _LL738;}
else{ goto _LL739;} _LL739: if(( unsigned int) _temp703 >  3u?*(( int*) _temp703)
==  Cyc_Absyn_AnonUnionType: 0){ _LL784: _temp783=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp703)->f1; goto _LL740;} else{ goto _LL741;} _LL741: if(( unsigned int)
_temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_SizeofType: 0){ _LL786:
_temp785=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp703)->f1; goto
_LL742;} else{ goto _LL743;} _LL743: if(( unsigned int) _temp703 >  3u?*(( int*)
_temp703) ==  Cyc_Absyn_FnType: 0){ _LL788: _temp787=(( struct Cyc_Absyn_FnType_struct*)
_temp703)->f1; _LL800: _temp799= _temp787.tvars; goto _LL798; _LL798: _temp797=
_temp787.effect; goto _LL796; _LL796: _temp795=( void*) _temp787.ret_typ; goto
_LL794; _LL794: _temp793= _temp787.args; goto _LL792; _LL792: _temp791= _temp787.cyc_varargs;
goto _LL790; _LL790: _temp789= _temp787.rgn_po; goto _LL744;} else{ goto _LL745;}
_LL745: if( _temp703 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL746;} else{ goto
_LL747;} _LL747: if(( unsigned int) _temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_AccessEff:
0){ goto _LL748;} else{ goto _LL749;} _LL749: if(( unsigned int) _temp703 >  3u?*((
int*) _temp703) ==  Cyc_Absyn_JoinEff: 0){ goto _LL750;} else{ goto _LL751;}
_LL751: if(( unsigned int) _temp703 >  3u?*(( int*) _temp703) ==  Cyc_Absyn_RgnsEff:
0){ _LL802: _temp801=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp703)->f1;
goto _LL752;} else{ goto _LL753;} _LL753: if(( unsigned int) _temp703 >  3u?*((
int*) _temp703) ==  Cyc_Absyn_TypedefType: 0){ goto _LL754;} else{ goto _LL704;}
_LL706: goto _LL708; _LL708: goto _LL710; _LL710: goto _LL712; _LL712: goto
_LL714; _LL714: goto _LL716; _LL716: return Cyc_Absyn_empty_effect; _LL718: goto
_LL720; _LL720: { void* _temp803= Cyc_Tcutil_typ_kind( t); _LL805: if( _temp803
== ( void*) Cyc_Absyn_RgnKind){ goto _LL806;} else{ goto _LL807;} _LL807: if(
_temp803 == ( void*) Cyc_Absyn_EffKind){ goto _LL808;} else{ goto _LL809;}
_LL809: goto _LL810; _LL806: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp811=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp811[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp812; _temp812.tag= Cyc_Absyn_AccessEff;
_temp812.f1=( void*) t; _temp812;}); _temp811;}); _LL808: return t; _LL810:
return( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp813=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp813[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp814; _temp814.tag= Cyc_Absyn_RgnsEff; _temp814.f1=( void*) t; _temp814;});
_temp813;}); _LL804:;} _LL722: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp815=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp815[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp816; _temp816.tag= Cyc_Absyn_AccessEff;
_temp816.f1=( void*) _temp755; _temp816;}); _temp815;}); _LL724: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp819=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp819->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp820=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp820[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp821; _temp821.tag= Cyc_Absyn_AccessEff;
_temp821.f1=( void*) _temp759; _temp821;}); _temp820;})); _temp819->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp761); _temp819;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp817=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp817[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp818; _temp818.tag= Cyc_Absyn_JoinEff; _temp818.f1= ts; _temp818;});
_temp817;}));} _LL726: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp822=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp822[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp823; _temp823.tag= Cyc_Absyn_JoinEff;
_temp823.f1=({ void* _temp824[ 2u]; _temp824[ 1u]= Cyc_Tcutil_rgns_of( _temp767);
_temp824[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp825=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp825[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp826; _temp826.tag= Cyc_Absyn_AccessEff;
_temp826.f1=( void*) _temp765; _temp826;}); _temp825;}); Cyc_List_list( _tag_arr(
_temp824, sizeof( void*), 2u));}); _temp823;}); _temp822;})); _LL728: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp769)); _LL730: { struct Cyc_List_List* _temp827= 0;
for( 0; _temp771 !=  0; _temp771= _temp771->tl){ _temp827=({ struct Cyc_List_List*
_temp828=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp828->hd=( void*)(*(( struct _tuple4*) _temp771->hd)).f2; _temp828->tl=
_temp827; _temp828;});} _temp775= _temp827; goto _LL732;} _LL732: _temp777=
_temp775; goto _LL734; _LL734: _temp779= _temp777; goto _LL736; _LL736: return
Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp829=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp829[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp830; _temp830.tag= Cyc_Absyn_JoinEff;
_temp830.f1= Cyc_List_map( Cyc_Tcutil_rgns_of, _temp779); _temp830;}); _temp829;}));
_LL738: _temp783= _temp781; goto _LL740; _LL740: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp831=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp831[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp832; _temp832.tag= Cyc_Absyn_JoinEff; _temp832.f1=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_rgns_of_field, _temp783); _temp832;}); _temp831;})); _LL742: return
Cyc_Tcutil_rgns_of( _temp785); _LL744: { void* _temp833= Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst, _temp799),( void*)((
struct Cyc_Core_Opt*) _check_null( _temp797))->v); return Cyc_Tcutil_normalize_effect(
_temp833);} _LL746: return Cyc_Absyn_empty_effect; _LL748: goto _LL750; _LL750:
return t; _LL752: return Cyc_Tcutil_rgns_of( _temp801); _LL754: return({ void*
_temp834[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp834, sizeof( void*), 0u));});
_LL704:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp835= e; struct Cyc_List_List* _temp843; struct Cyc_List_List**
_temp845; void* _temp846; _LL837: if(( unsigned int) _temp835 >  3u?*(( int*)
_temp835) ==  Cyc_Absyn_JoinEff: 0){ _LL844: _temp843=(( struct Cyc_Absyn_JoinEff_struct*)
_temp835)->f1; _temp845=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp835)->f1; goto _LL838;} else{ goto _LL839;} _LL839: if(( unsigned int)
_temp835 >  3u?*(( int*) _temp835) ==  Cyc_Absyn_RgnsEff: 0){ _LL847: _temp846=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp835)->f1; goto _LL840;} else{
goto _LL841;} _LL841: goto _LL842; _LL838: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp845; for( 0; effs !=  0; effs= effs->tl){ void* _temp848=( void*)
effs->hd;( void*)( effs->hd=( void*) Cyc_Tcutil_compress( Cyc_Tcutil_normalize_effect(
_temp848)));{ void* _temp849=( void*) effs->hd; _LL851: if(( unsigned int)
_temp849 >  3u?*(( int*) _temp849) ==  Cyc_Absyn_JoinEff: 0){ goto _LL852;}
else{ goto _LL853;} _LL853: goto _LL854; _LL852: nested_join= 1; goto _LL850;
_LL854: goto _LL850; _LL850:;}}} if( ! nested_join){ return e;}{ struct Cyc_List_List*
effects= 0;{ struct Cyc_List_List* effs=* _temp845; for( 0; effs !=  0; effs=
effs->tl){ void* _temp855= Cyc_Tcutil_compress(( void*) effs->hd); struct Cyc_List_List*
_temp863; void* _temp865; _LL857: if(( unsigned int) _temp855 >  3u?*(( int*)
_temp855) ==  Cyc_Absyn_JoinEff: 0){ _LL864: _temp863=(( struct Cyc_Absyn_JoinEff_struct*)
_temp855)->f1; goto _LL858;} else{ goto _LL859;} _LL859: if(( unsigned int)
_temp855 >  3u?*(( int*) _temp855) ==  Cyc_Absyn_AccessEff: 0){ _LL866: _temp865=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp855)->f1; if( _temp865 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL860;} else{ goto _LL861;}} else{ goto _LL861;}
_LL861: goto _LL862; _LL858: effects= Cyc_List_revappend( _temp863, effects);
goto _LL856; _LL860: goto _LL856; _LL862: effects=({ struct Cyc_List_List*
_temp867=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp867->hd=( void*) _temp855; _temp867->tl= effects; _temp867;}); goto _LL856;
_LL856:;}}* _temp845= Cyc_List_imp_rev( effects); return e;}} _LL840: return Cyc_Tcutil_rgns_of(
_temp846); _LL842: return e; _LL836:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp868= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp876; struct Cyc_List_List _temp878; struct Cyc_List_List* _temp879; void*
_temp881; struct Cyc_Core_Opt* _temp883; struct Cyc_Core_Opt* _temp885; _LL870:
if(( unsigned int) _temp868 >  3u?*(( int*) _temp868) ==  Cyc_Absyn_JoinEff: 0){
_LL877: _temp876=(( struct Cyc_Absyn_JoinEff_struct*) _temp868)->f1; if(
_temp876 ==  0){ goto _LL872;} else{ _temp878=* _temp876; _LL882: _temp881=(
void*) _temp878.hd; goto _LL880; _LL880: _temp879= _temp878.tl; goto _LL871;}}
else{ goto _LL872;} _LL872: if(( unsigned int) _temp868 >  3u?*(( int*) _temp868)
==  Cyc_Absyn_Evar: 0){ _LL886: _temp885=(( struct Cyc_Absyn_Evar_struct*)
_temp868)->f1; goto _LL884; _LL884: _temp883=(( struct Cyc_Absyn_Evar_struct*)
_temp868)->f4; goto _LL873;} else{ goto _LL874;} _LL874: goto _LL875; _LL871: {
void* _temp887= Cyc_Tcutil_compress( _temp881); struct Cyc_Core_Opt* _temp893;
_LL889: if(( unsigned int) _temp887 >  3u?*(( int*) _temp887) ==  Cyc_Absyn_Evar:
0){ _LL894: _temp893=(( struct Cyc_Absyn_Evar_struct*) _temp887)->f4; goto
_LL890;} else{ goto _LL891;} _LL891: goto _LL892; _LL890: return({ struct
_tuple9* _temp895=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp895->f1= _temp881; _temp895->f2= _temp879; _temp895->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp893))->v; _temp895;}); _LL892: return 0;
_LL888:;} _LL873: if( _temp885 ==  0? 1:( void*) _temp885->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp896[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp896, sizeof( void*), 0u));});} return({ struct _tuple9*
_temp897=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp897->f1= t;
_temp897->f2= 0; _temp897->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp883))->v; _temp897;}); _LL875: return 0; _LL869:;} static
struct Cyc_Core_Opt Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp898=({ struct Cyc_Absyn_FnType_struct*
_temp899=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp899[ 0]=({ struct Cyc_Absyn_FnType_struct _temp900; _temp900.tag= Cyc_Absyn_FnType;
_temp900.f1=({ struct Cyc_Absyn_FnInfo _temp901; _temp901.tvars= 0; _temp901.effect=({
struct Cyc_Core_Opt* _temp902=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp902->v=( void*) eff; _temp902;}); _temp901.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp901.args= 0; _temp901.c_varargs= 0; _temp901.cyc_varargs=
0; _temp901.rgn_po= 0; _temp901.attributes= 0; _temp901;}); _temp900;});
_temp899;}); return Cyc_Absyn_atb_typ(( void*) _temp898,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp903= Cyc_Tcutil_compress( e); void*
_temp915; struct Cyc_List_List* _temp917; void* _temp919; struct Cyc_Core_Opt*
_temp921; struct Cyc_Core_Opt* _temp923; struct Cyc_Core_Opt** _temp925; struct
Cyc_Core_Opt* _temp926; _LL905: if(( unsigned int) _temp903 >  3u?*(( int*)
_temp903) ==  Cyc_Absyn_AccessEff: 0){ _LL916: _temp915=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp903)->f1; goto _LL906;} else{ goto _LL907;} _LL907: if(( unsigned int)
_temp903 >  3u?*(( int*) _temp903) ==  Cyc_Absyn_JoinEff: 0){ _LL918: _temp917=((
struct Cyc_Absyn_JoinEff_struct*) _temp903)->f1; goto _LL908;} else{ goto _LL909;}
_LL909: if(( unsigned int) _temp903 >  3u?*(( int*) _temp903) ==  Cyc_Absyn_RgnsEff:
0){ _LL920: _temp919=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp903)->f1;
goto _LL910;} else{ goto _LL911;} _LL911: if(( unsigned int) _temp903 >  3u?*((
int*) _temp903) ==  Cyc_Absyn_Evar: 0){ _LL927: _temp926=(( struct Cyc_Absyn_Evar_struct*)
_temp903)->f1; goto _LL924; _LL924: _temp923=(( struct Cyc_Absyn_Evar_struct*)
_temp903)->f2; _temp925=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp903)->f2; goto _LL922; _LL922: _temp921=(( struct Cyc_Absyn_Evar_struct*)
_temp903)->f4; goto _LL912;} else{ goto _LL913;} _LL913: goto _LL914; _LL906:
if( constrain){ return Cyc_Tcutil_unify( r, _temp915);} _temp915= Cyc_Tcutil_compress(
_temp915); if( r ==  _temp915){ return 1;}{ struct _tuple8 _temp929=({ struct
_tuple8 _temp928; _temp928.f1= r; _temp928.f2= _temp915; _temp928;}); void*
_temp935; struct Cyc_Absyn_Tvar* _temp937; void* _temp939; struct Cyc_Absyn_Tvar*
_temp941; _LL931: _LL940: _temp939= _temp929.f1; if(( unsigned int) _temp939 > 
3u?*(( int*) _temp939) ==  Cyc_Absyn_VarType: 0){ _LL942: _temp941=(( struct Cyc_Absyn_VarType_struct*)
_temp939)->f1; goto _LL936;} else{ goto _LL933;} _LL936: _temp935= _temp929.f2;
if(( unsigned int) _temp935 >  3u?*(( int*) _temp935) ==  Cyc_Absyn_VarType: 0){
_LL938: _temp937=(( struct Cyc_Absyn_VarType_struct*) _temp935)->f1; goto _LL932;}
else{ goto _LL933;} _LL933: goto _LL934; _LL932: return Cyc_Absyn_tvar_cmp(
_temp941, _temp937) ==  0; _LL934: return 0; _LL930:;} _LL908: for( 0; _temp917
!=  0; _temp917= _temp917->tl){ if( Cyc_Tcutil_region_in_effect( constrain, r,(
void*) _temp917->hd)){ return 1;}} return 0; _LL910: { void* _temp943= Cyc_Tcutil_rgns_of(
_temp919); void* _temp949; _LL945: if(( unsigned int) _temp943 >  3u?*(( int*)
_temp943) ==  Cyc_Absyn_RgnsEff: 0){ _LL950: _temp949=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp943)->f1; goto _LL946;} else{ goto _LL947;} _LL947: goto _LL948; _LL946:
if( ! constrain){ return 0;}{ void* _temp951= Cyc_Tcutil_compress( _temp949);
struct Cyc_Core_Opt* _temp957; struct Cyc_Core_Opt* _temp959; struct Cyc_Core_Opt**
_temp961; struct Cyc_Core_Opt* _temp962; _LL953: if(( unsigned int) _temp951 > 
3u?*(( int*) _temp951) ==  Cyc_Absyn_Evar: 0){ _LL963: _temp962=(( struct Cyc_Absyn_Evar_struct*)
_temp951)->f1; goto _LL960; _LL960: _temp959=(( struct Cyc_Absyn_Evar_struct*)
_temp951)->f2; _temp961=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp951)->f2; goto _LL958; _LL958: _temp957=(( struct Cyc_Absyn_Evar_struct*)
_temp951)->f4; goto _LL954;} else{ goto _LL955;} _LL955: goto _LL956; _LL954: {
void* _temp964= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp957); Cyc_Tcutil_occurs( _temp964, Cyc_Core_heap_region,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp957))->v, r);{ void* _temp965= Cyc_Tcutil_dummy_fntype((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp967=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp967[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp968; _temp968.tag= Cyc_Absyn_JoinEff; _temp968.f1=({ void* _temp969[ 2u];
_temp969[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp970=( struct
Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp970[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp971; _temp971.tag= Cyc_Absyn_AccessEff;
_temp971.f1=( void*) r; _temp971;}); _temp970;}); _temp969[ 0u]= _temp964; Cyc_List_list(
_tag_arr( _temp969, sizeof( void*), 2u));}); _temp968;}); _temp967;}));*
_temp961=({ struct Cyc_Core_Opt* _temp966=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp966->v=( void*) _temp965; _temp966;});
return 1;}} _LL956: return 0; _LL952:;} _LL948: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp943); _LL944:;} _LL912: if( _temp926 ==  0? 1:( void*)
_temp926->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp972[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp972, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp973= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp921); Cyc_Tcutil_occurs( _temp973, Cyc_Core_heap_region,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp921))->v, r);{
struct Cyc_Absyn_JoinEff_struct* _temp974=({ struct Cyc_Absyn_JoinEff_struct*
_temp976=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp976[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp977; _temp977.tag= Cyc_Absyn_JoinEff;
_temp977.f1=({ struct Cyc_List_List* _temp978=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp978->hd=( void*) _temp973; _temp978->tl=({
struct Cyc_List_List* _temp979=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp979->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp980=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp980[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp981; _temp981.tag= Cyc_Absyn_AccessEff;
_temp981.f1=( void*) r; _temp981;}); _temp980;})); _temp979->tl= 0; _temp979;});
_temp978;}); _temp977;}); _temp976;});* _temp925=({ struct Cyc_Core_Opt*
_temp975=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp975->v=( void*)(( void*) _temp974); _temp975;}); return 1;}} _LL914: return
0; _LL904:;}} static int Cyc_Tcutil_type_in_effect( int constrain, void* t, void*
e){ t= Cyc_Tcutil_compress( t);{ void* _temp982= Cyc_Tcutil_compress( e); struct
Cyc_List_List* _temp994; void* _temp996; struct Cyc_Core_Opt* _temp998; struct
Cyc_Core_Opt* _temp1000; struct Cyc_Core_Opt** _temp1002; struct Cyc_Core_Opt*
_temp1003; _LL984: if(( unsigned int) _temp982 >  3u?*(( int*) _temp982) ==  Cyc_Absyn_AccessEff:
0){ goto _LL985;} else{ goto _LL986;} _LL986: if(( unsigned int) _temp982 >  3u?*((
int*) _temp982) ==  Cyc_Absyn_JoinEff: 0){ _LL995: _temp994=(( struct Cyc_Absyn_JoinEff_struct*)
_temp982)->f1; goto _LL987;} else{ goto _LL988;} _LL988: if(( unsigned int)
_temp982 >  3u?*(( int*) _temp982) ==  Cyc_Absyn_RgnsEff: 0){ _LL997: _temp996=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp982)->f1; goto _LL989;} else{
goto _LL990;} _LL990: if(( unsigned int) _temp982 >  3u?*(( int*) _temp982) == 
Cyc_Absyn_Evar: 0){ _LL1004: _temp1003=(( struct Cyc_Absyn_Evar_struct*)
_temp982)->f1; goto _LL1001; _LL1001: _temp1000=(( struct Cyc_Absyn_Evar_struct*)
_temp982)->f2; _temp1002=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp982)->f2; goto _LL999; _LL999: _temp998=(( struct Cyc_Absyn_Evar_struct*)
_temp982)->f4; goto _LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL985:
return 0; _LL987: for( 0; _temp994 !=  0; _temp994= _temp994->tl){ if( Cyc_Tcutil_type_in_effect(
constrain, t,( void*) _temp994->hd)){ return 1;}} return 0; _LL989: _temp996=
Cyc_Tcutil_compress( _temp996); if( t ==  _temp996){ return 1;} if( constrain){
return Cyc_Tcutil_unify( t, _temp996);}{ void* _temp1005= Cyc_Tcutil_rgns_of( t);
void* _temp1011; _LL1007: if(( unsigned int) _temp1005 >  3u?*(( int*) _temp1005)
==  Cyc_Absyn_RgnsEff: 0){ _LL1012: _temp1011=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1005)->f1; goto _LL1008;} else{ goto _LL1009;} _LL1009: goto _LL1010;
_LL1008: { struct _tuple8 _temp1014=({ struct _tuple8 _temp1013; _temp1013.f1= t;
_temp1013.f2= Cyc_Tcutil_compress( _temp1011); _temp1013;}); void* _temp1020;
struct Cyc_Absyn_Tvar* _temp1022; void* _temp1024; struct Cyc_Absyn_Tvar*
_temp1026; _LL1016: _LL1025: _temp1024= _temp1014.f1; if(( unsigned int)
_temp1024 >  3u?*(( int*) _temp1024) ==  Cyc_Absyn_VarType: 0){ _LL1027:
_temp1026=(( struct Cyc_Absyn_VarType_struct*) _temp1024)->f1; goto _LL1021;}
else{ goto _LL1018;} _LL1021: _temp1020= _temp1014.f2; if(( unsigned int)
_temp1020 >  3u?*(( int*) _temp1020) ==  Cyc_Absyn_VarType: 0){ _LL1023:
_temp1022=(( struct Cyc_Absyn_VarType_struct*) _temp1020)->f1; goto _LL1017;}
else{ goto _LL1018;} _LL1018: goto _LL1019; _LL1017: return Cyc_Tcutil_unify( t,
_temp1011); _LL1019: return t ==  _temp1011; _LL1015:;} _LL1010: return Cyc_Tcutil_type_in_effect(
constrain, t, _temp1005); _LL1006:;} _LL991: if( _temp1003 ==  0? 1:( void*)
_temp1003->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp1028[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp1028, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp1029= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp998); Cyc_Tcutil_occurs( _temp1029, Cyc_Core_heap_region,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp998))->v, t);{
struct Cyc_Absyn_JoinEff_struct* _temp1030=({ struct Cyc_Absyn_JoinEff_struct*
_temp1032=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1032[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1033; _temp1033.tag= Cyc_Absyn_JoinEff;
_temp1033.f1=({ struct Cyc_List_List* _temp1034=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1034->hd=( void*) _temp1029;
_temp1034->tl=({ struct Cyc_List_List* _temp1035=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1035->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp1036=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp1036[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp1037; _temp1037.tag= Cyc_Absyn_RgnsEff; _temp1037.f1=(
void*) t; _temp1037;}); _temp1036;})); _temp1035->tl= 0; _temp1035;}); _temp1034;});
_temp1033;}); _temp1032;});* _temp1002=({ struct Cyc_Core_Opt* _temp1031=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1031->v=(
void*)(( void*) _temp1030); _temp1031;}); return 1;}} _LL993: return 0; _LL983:;}}
static int Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar*
v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp1038= e; struct Cyc_Absyn_Tvar*
_temp1050; struct Cyc_List_List* _temp1052; void* _temp1054; struct Cyc_Core_Opt*
_temp1056; struct Cyc_Core_Opt* _temp1058; struct Cyc_Core_Opt** _temp1060;
struct Cyc_Core_Opt* _temp1061; _LL1040: if(( unsigned int) _temp1038 >  3u?*((
int*) _temp1038) ==  Cyc_Absyn_VarType: 0){ _LL1051: _temp1050=(( struct Cyc_Absyn_VarType_struct*)
_temp1038)->f1; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(( unsigned int)
_temp1038 >  3u?*(( int*) _temp1038) ==  Cyc_Absyn_JoinEff: 0){ _LL1053:
_temp1052=(( struct Cyc_Absyn_JoinEff_struct*) _temp1038)->f1; goto _LL1043;}
else{ goto _LL1044;} _LL1044: if(( unsigned int) _temp1038 >  3u?*(( int*)
_temp1038) ==  Cyc_Absyn_RgnsEff: 0){ _LL1055: _temp1054=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1038)->f1; goto _LL1045;} else{ goto _LL1046;} _LL1046: if(( unsigned int)
_temp1038 >  3u?*(( int*) _temp1038) ==  Cyc_Absyn_Evar: 0){ _LL1062: _temp1061=((
struct Cyc_Absyn_Evar_struct*) _temp1038)->f1; goto _LL1059; _LL1059: _temp1058=((
struct Cyc_Absyn_Evar_struct*) _temp1038)->f2; _temp1060=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1038)->f2; goto _LL1057; _LL1057: _temp1056=((
struct Cyc_Absyn_Evar_struct*) _temp1038)->f4; goto _LL1047;} else{ goto _LL1048;}
_LL1048: goto _LL1049; _LL1041: return Cyc_Absyn_tvar_cmp( v, _temp1050) ==  0;
_LL1043: for( 0; _temp1052 !=  0; _temp1052= _temp1052->tl){ if( Cyc_Tcutil_variable_in_effect(
constrain, v,( void*) _temp1052->hd)){ return 1;}} return 0; _LL1045: { void*
_temp1063= Cyc_Tcutil_rgns_of( _temp1054); void* _temp1069; _LL1065: if((
unsigned int) _temp1063 >  3u?*(( int*) _temp1063) ==  Cyc_Absyn_RgnsEff: 0){
_LL1070: _temp1069=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1063)->f1;
goto _LL1066;} else{ goto _LL1067;} _LL1067: goto _LL1068; _LL1066: if( !
constrain){ return 0;}{ void* _temp1071= Cyc_Tcutil_compress( _temp1069); struct
Cyc_Core_Opt* _temp1077; struct Cyc_Core_Opt* _temp1079; struct Cyc_Core_Opt**
_temp1081; struct Cyc_Core_Opt* _temp1082; _LL1073: if(( unsigned int) _temp1071
>  3u?*(( int*) _temp1071) ==  Cyc_Absyn_Evar: 0){ _LL1083: _temp1082=(( struct
Cyc_Absyn_Evar_struct*) _temp1071)->f1; goto _LL1080; _LL1080: _temp1079=((
struct Cyc_Absyn_Evar_struct*) _temp1071)->f2; _temp1081=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1071)->f2; goto _LL1078; _LL1078: _temp1077=((
struct Cyc_Absyn_Evar_struct*) _temp1071)->f4; goto _LL1074;} else{ goto _LL1075;}
_LL1075: goto _LL1076; _LL1074: { void* _temp1084= Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp1077); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1077))->v, v)){ return 0;}{ void*
_temp1085= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1087=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1087[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1088; _temp1088.tag= Cyc_Absyn_JoinEff;
_temp1088.f1=({ void* _temp1089[ 2u]; _temp1089[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1090=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1090[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1091; _temp1091.tag= Cyc_Absyn_VarType;
_temp1091.f1= v; _temp1091;}); _temp1090;}); _temp1089[ 0u]= _temp1084; Cyc_List_list(
_tag_arr( _temp1089, sizeof( void*), 2u));}); _temp1088;}); _temp1087;}));*
_temp1081=({ struct Cyc_Core_Opt* _temp1086=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1086->v=( void*) _temp1085; _temp1086;});
return 1;}} _LL1076: return 0; _LL1072:;} _LL1068: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp1063); _LL1064:;} _LL1047: if( _temp1061 ==  0? 1:( void*)
_temp1061->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp1092[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp1092, sizeof( void*), 0u));});}{
void* _temp1093= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp1056); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1056))->v, v)){
return 0;}{ struct Cyc_Absyn_JoinEff_struct* _temp1094=({ struct Cyc_Absyn_JoinEff_struct*
_temp1096=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1096[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1097; _temp1097.tag= Cyc_Absyn_JoinEff;
_temp1097.f1=({ struct Cyc_List_List* _temp1098=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1098->hd=( void*) _temp1093;
_temp1098->tl=({ struct Cyc_List_List* _temp1099=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1099->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp1100=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1100[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1101; _temp1101.tag= Cyc_Absyn_VarType; _temp1101.f1=
v; _temp1101;}); _temp1100;})); _temp1099->tl= 0; _temp1099;}); _temp1098;});
_temp1097;}); _temp1096;});* _temp1060=({ struct Cyc_Core_Opt* _temp1095=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1095->v=(
void*)(( void*) _temp1094); _temp1095;}); return 1;}} _LL1049: return 0; _LL1039:;}}
static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp1102= e; struct Cyc_List_List* _temp1112; void* _temp1114;
_LL1104: if(( unsigned int) _temp1102 >  3u?*(( int*) _temp1102) ==  Cyc_Absyn_JoinEff:
0){ _LL1113: _temp1112=(( struct Cyc_Absyn_JoinEff_struct*) _temp1102)->f1; goto
_LL1105;} else{ goto _LL1106;} _LL1106: if(( unsigned int) _temp1102 >  3u?*((
int*) _temp1102) ==  Cyc_Absyn_RgnsEff: 0){ _LL1115: _temp1114=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp1102)->f1; goto _LL1107;} else{ goto _LL1108;}
_LL1108: if(( unsigned int) _temp1102 >  3u?*(( int*) _temp1102) ==  Cyc_Absyn_Evar:
0){ goto _LL1109;} else{ goto _LL1110;} _LL1110: goto _LL1111; _LL1105: for( 0;
_temp1112 !=  0; _temp1112= _temp1112->tl){ if( Cyc_Tcutil_evar_in_effect( evar,(
void*) _temp1112->hd)){ return 1;}} return 0; _LL1107: { void* _temp1116= Cyc_Tcutil_rgns_of(
_temp1114); void* _temp1122; _LL1118: if(( unsigned int) _temp1116 >  3u?*(( int*)
_temp1116) ==  Cyc_Absyn_RgnsEff: 0){ _LL1123: _temp1122=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1116)->f1; goto _LL1119;} else{ goto _LL1120;} _LL1120: goto _LL1121;
_LL1119: return 0; _LL1121: return Cyc_Tcutil_evar_in_effect( evar, _temp1116);
_LL1117:;} _LL1109: return evar ==  e; _LL1111: return 0; _LL1103:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1124= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1138; void* _temp1140; struct Cyc_Absyn_Tvar*
_temp1142; void* _temp1144; struct Cyc_Core_Opt* _temp1146; struct Cyc_Core_Opt*
_temp1148; struct Cyc_Core_Opt** _temp1150; _LL1126: if(( unsigned int)
_temp1124 >  3u?*(( int*) _temp1124) ==  Cyc_Absyn_JoinEff: 0){ _LL1139:
_temp1138=(( struct Cyc_Absyn_JoinEff_struct*) _temp1124)->f1; goto _LL1127;}
else{ goto _LL1128;} _LL1128: if(( unsigned int) _temp1124 >  3u?*(( int*)
_temp1124) ==  Cyc_Absyn_AccessEff: 0){ _LL1141: _temp1140=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1124)->f1; goto _LL1129;} else{ goto _LL1130;} _LL1130: if(( unsigned int)
_temp1124 >  3u?*(( int*) _temp1124) ==  Cyc_Absyn_VarType: 0){ _LL1143:
_temp1142=(( struct Cyc_Absyn_VarType_struct*) _temp1124)->f1; goto _LL1131;}
else{ goto _LL1132;} _LL1132: if(( unsigned int) _temp1124 >  3u?*(( int*)
_temp1124) ==  Cyc_Absyn_RgnsEff: 0){ _LL1145: _temp1144=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1124)->f1; goto _LL1133;} else{ goto _LL1134;} _LL1134: if(( unsigned int)
_temp1124 >  3u?*(( int*) _temp1124) ==  Cyc_Absyn_Evar: 0){ _LL1149: _temp1148=((
struct Cyc_Absyn_Evar_struct*) _temp1124)->f2; _temp1150=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1124)->f2; goto _LL1147; _LL1147: _temp1146=((
struct Cyc_Absyn_Evar_struct*) _temp1124)->f4; goto _LL1135;} else{ goto _LL1136;}
_LL1136: goto _LL1137; _LL1127: for( 0; _temp1138 !=  0; _temp1138= _temp1138->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*) _temp1138->hd, e2)){
return 0;}} return 1; _LL1129: return( Cyc_Tcutil_region_in_effect( 0, _temp1140,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp1140, e2))? 1: Cyc_Tcutil_unify(
_temp1140,( void*) Cyc_Absyn_HeapRgn); _LL1131: return Cyc_Tcutil_variable_in_effect(
0, _temp1142, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1142, e2); _LL1133: {
void* _temp1151= Cyc_Tcutil_rgns_of( _temp1144); void* _temp1157; _LL1153: if((
unsigned int) _temp1151 >  3u?*(( int*) _temp1151) ==  Cyc_Absyn_RgnsEff: 0){
_LL1158: _temp1157=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1151)->f1;
goto _LL1154;} else{ goto _LL1155;} _LL1155: goto _LL1156; _LL1154: return( Cyc_Tcutil_type_in_effect(
0, _temp1157, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1157, e2))? 1: Cyc_Tcutil_unify(
_temp1157, Cyc_Absyn_sint_t); _LL1156: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1151, e2); _LL1152:;} _LL1135: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1150=({ struct Cyc_Core_Opt* _temp1159=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1159->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1160=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1160[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1161; _temp1161.tag= Cyc_Absyn_JoinEff; _temp1161.f1=
0; _temp1161;}); _temp1160;})); _temp1159;});} else{ Cyc_Tcutil_occurs( e1, Cyc_Core_heap_region,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1146))->v, e2);*
_temp1150=({ struct Cyc_Core_Opt* _temp1162=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1162->v=( void*) e2; _temp1162;});}} return
1; _LL1137: return({ struct Cyc_Std_String_pa_struct _temp1164; _temp1164.tag=
Cyc_Std_String_pa; _temp1164.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1163[ 1u]={& _temp1164};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1163, sizeof( void*), 1u));}});
_LL1125:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1166=({ struct
_tuple10 _temp1165; _temp1165.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1165.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1165;}); _LL1168: goto _LL1169; _LL1169:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1167:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1= r1->tl){ struct _tuple8 _temp1172; void* _temp1173; void* _temp1175; struct
_tuple8* _temp1170=( struct _tuple8*) r1->hd; _temp1172=* _temp1170; _LL1176:
_temp1175= _temp1172.f1; goto _LL1174; _LL1174: _temp1173= _temp1172.f2; goto
_LL1171; _LL1171: { int found= _temp1175 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2= r2->tl){ struct
_tuple8 _temp1179; void* _temp1180; void* _temp1182; struct _tuple8* _temp1177=(
struct _tuple8*) r2->hd; _temp1179=* _temp1177; _LL1183: _temp1182= _temp1179.f1;
goto _LL1181; _LL1181: _temp1180= _temp1179.f2; goto _LL1178; _LL1178: if( Cyc_Tcutil_unify(
_temp1175, _temp1182)? Cyc_Tcutil_unify( _temp1173, _temp1180): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason=(
struct _tagged_arr) _tag_arr( 0u, 0u, 0u); t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2); if( t1 ==  t2){ return;}{ void* _temp1184= t1; struct Cyc_Core_Opt*
_temp1190; struct Cyc_Core_Opt* _temp1192; struct Cyc_Core_Opt** _temp1194;
struct Cyc_Core_Opt* _temp1195; _LL1186: if(( unsigned int) _temp1184 >  3u?*((
int*) _temp1184) ==  Cyc_Absyn_Evar: 0){ _LL1196: _temp1195=(( struct Cyc_Absyn_Evar_struct*)
_temp1184)->f1; goto _LL1193; _LL1193: _temp1192=(( struct Cyc_Absyn_Evar_struct*)
_temp1184)->f2; _temp1194=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1184)->f2; goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_Evar_struct*)
_temp1184)->f4; goto _LL1187;} else{ goto _LL1188;} _LL1188: goto _LL1189;
_LL1187: Cyc_Tcutil_occurs( t1, Cyc_Core_heap_region,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1190))->v, t2);{ void* _temp1197= Cyc_Tcutil_typ_kind(
t2); if( Cyc_Tcutil_kind_leq( _temp1197,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1195))->v)){* _temp1194=({ struct Cyc_Core_Opt* _temp1198=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1198->v=(
void*) t2; _temp1198;}); return;} else{{ void* _temp1199= t2; struct Cyc_Core_Opt*
_temp1207; struct Cyc_Core_Opt* _temp1209; struct Cyc_Core_Opt** _temp1211;
struct Cyc_Absyn_PtrInfo _temp1213; _LL1201: if(( unsigned int) _temp1199 >  3u?*((
int*) _temp1199) ==  Cyc_Absyn_Evar: 0){ _LL1210: _temp1209=(( struct Cyc_Absyn_Evar_struct*)
_temp1199)->f2; _temp1211=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp1199)->f2; goto _LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_Evar_struct*)
_temp1199)->f4; goto _LL1202;} else{ goto _LL1203;} _LL1203: if(( unsigned int)
_temp1199 >  3u?*(( int*) _temp1199) ==  Cyc_Absyn_PointerType: 0){ _LL1214:
_temp1213=(( struct Cyc_Absyn_PointerType_struct*) _temp1199)->f1; goto _LL1212;}
else{ goto _LL1205;} _LL1212: if(( void*) _temp1195->v == ( void*) Cyc_Absyn_BoxKind){
goto _LL1204;} else{ goto _LL1205;} _LL1205: goto _LL1206; _LL1202: { struct Cyc_List_List*
_temp1215=( struct Cyc_List_List*) _temp1190->v;{ struct Cyc_List_List* s2=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1207))->v; for(
0; s2 !=  0; s2= s2->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1215,( struct Cyc_Absyn_Tvar*) s2->hd)){ Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)", sizeof( unsigned char), 35u);(
int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq(( void*)
_temp1195->v, _temp1197)){* _temp1211=({ struct Cyc_Core_Opt* _temp1216=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1216->v=( void*)
t1; _temp1216;}); return;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u); goto _LL1200;} _LL1204: { struct Cyc_Absyn_Conref*
_temp1217= Cyc_Absyn_compress_conref( _temp1213.bounds);{ void* _temp1218=( void*)
_temp1217->v; _LL1220: if( _temp1218 == ( void*) Cyc_Absyn_No_constr){ goto
_LL1221;} else{ goto _LL1222;} _LL1222: goto _LL1223; _LL1221:( void*)(
_temp1217->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1224=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1224[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp1225; _temp1225.tag= Cyc_Absyn_Eq_constr;
_temp1225.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1226=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1226[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1227; _temp1227.tag= Cyc_Absyn_Upper_b;
_temp1227.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1227;}); _temp1226;}));
_temp1225;}); _temp1224;})));* _temp1194=({ struct Cyc_Core_Opt* _temp1228=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1228->v=(
void*) t2; _temp1228;}); return; _LL1223: goto _LL1219; _LL1219:;} goto _LL1200;}
_LL1206: goto _LL1200; _LL1200:;} Cyc_Tcutil_failure_reason= _tag_arr("(kinds are incompatible)",
sizeof( unsigned char), 25u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} _LL1189:
goto _LL1185; _LL1185:;}{ struct _tuple8 _temp1230=({ struct _tuple8 _temp1229;
_temp1229.f1= t2; _temp1229.f2= t1; _temp1229;}); void* _temp1288; void*
_temp1290; void* _temp1292; void* _temp1294; struct Cyc_Absyn_Tvar* _temp1296;
void* _temp1298; struct Cyc_Absyn_Tvar* _temp1300; void* _temp1302; struct Cyc_Absyn_Structdecl**
_temp1304; struct Cyc_List_List* _temp1306; struct _tuple1* _temp1308; void*
_temp1310; struct Cyc_Absyn_Structdecl** _temp1312; struct Cyc_List_List*
_temp1314; struct _tuple1* _temp1316; void* _temp1318; struct _tuple1* _temp1320;
void* _temp1322; struct _tuple1* _temp1324; void* _temp1326; struct Cyc_List_List*
_temp1328; void* _temp1330; struct Cyc_List_List* _temp1332; void* _temp1334;
struct Cyc_Absyn_Uniondecl** _temp1336; struct Cyc_List_List* _temp1338; struct
_tuple1* _temp1340; void* _temp1342; struct Cyc_Absyn_Uniondecl** _temp1344;
struct Cyc_List_List* _temp1346; struct _tuple1* _temp1348; void* _temp1350;
struct Cyc_Absyn_TunionInfo _temp1352; void* _temp1354; struct Cyc_List_List*
_temp1356; void* _temp1358; struct Cyc_Absyn_Tuniondecl** _temp1360; struct Cyc_Absyn_Tuniondecl*
_temp1362; void* _temp1363; struct Cyc_Absyn_TunionInfo _temp1365; void*
_temp1367; struct Cyc_List_List* _temp1369; void* _temp1371; struct Cyc_Absyn_Tuniondecl**
_temp1373; struct Cyc_Absyn_Tuniondecl* _temp1375; void* _temp1376; struct Cyc_Absyn_TunionFieldInfo
_temp1378; struct Cyc_List_List* _temp1380; void* _temp1382; struct Cyc_Absyn_Tunionfield*
_temp1384; struct Cyc_Absyn_Tuniondecl* _temp1386; void* _temp1388; struct Cyc_Absyn_TunionFieldInfo
_temp1390; struct Cyc_List_List* _temp1392; void* _temp1394; struct Cyc_Absyn_Tunionfield*
_temp1396; struct Cyc_Absyn_Tuniondecl* _temp1398; void* _temp1400; struct Cyc_Absyn_PtrInfo
_temp1402; struct Cyc_Absyn_Conref* _temp1404; struct Cyc_Absyn_Tqual _temp1406;
struct Cyc_Absyn_Conref* _temp1408; void* _temp1410; void* _temp1412; void*
_temp1414; struct Cyc_Absyn_PtrInfo _temp1416; struct Cyc_Absyn_Conref*
_temp1418; struct Cyc_Absyn_Tqual _temp1420; struct Cyc_Absyn_Conref* _temp1422;
void* _temp1424; void* _temp1426; void* _temp1428; void* _temp1430; void*
_temp1432; void* _temp1434; void* _temp1436; void* _temp1438; void* _temp1440;
void* _temp1442; void* _temp1444; int _temp1446; void* _temp1448; int _temp1450;
void* _temp1452; void* _temp1454; void* _temp1456; void* _temp1458; void*
_temp1460; struct Cyc_Absyn_Exp* _temp1462; struct Cyc_Absyn_Tqual _temp1464;
void* _temp1466; void* _temp1468; struct Cyc_Absyn_Exp* _temp1470; struct Cyc_Absyn_Tqual
_temp1472; void* _temp1474; void* _temp1476; struct Cyc_Absyn_FnInfo _temp1478;
struct Cyc_List_List* _temp1480; struct Cyc_List_List* _temp1482; struct Cyc_Absyn_VarargInfo*
_temp1484; int _temp1486; struct Cyc_List_List* _temp1488; void* _temp1490;
struct Cyc_Core_Opt* _temp1492; struct Cyc_List_List* _temp1494; void* _temp1496;
struct Cyc_Absyn_FnInfo _temp1498; struct Cyc_List_List* _temp1500; struct Cyc_List_List*
_temp1502; struct Cyc_Absyn_VarargInfo* _temp1504; int _temp1506; struct Cyc_List_List*
_temp1508; void* _temp1510; struct Cyc_Core_Opt* _temp1512; struct Cyc_List_List*
_temp1514; void* _temp1516; struct Cyc_List_List* _temp1518; void* _temp1520;
struct Cyc_List_List* _temp1522; void* _temp1524; struct Cyc_List_List*
_temp1526; void* _temp1528; struct Cyc_List_List* _temp1530; void* _temp1532;
struct Cyc_List_List* _temp1534; void* _temp1536; struct Cyc_List_List*
_temp1538; void* _temp1540; void* _temp1542; void* _temp1544; void* _temp1546;
void* _temp1548; void* _temp1550; void* _temp1552; void* _temp1554; void*
_temp1556; void* _temp1558; void* _temp1560; void* _temp1562; _LL1232: _LL1289:
_temp1288= _temp1230.f1; if(( unsigned int) _temp1288 >  3u?*(( int*) _temp1288)
==  Cyc_Absyn_Evar: 0){ goto _LL1233;} else{ goto _LL1234;} _LL1234: _LL1293:
_temp1292= _temp1230.f1; if( _temp1292 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1291;} else{ goto _LL1236;} _LL1291: _temp1290= _temp1230.f2; if( _temp1290
== ( void*) Cyc_Absyn_VoidType){ goto _LL1235;} else{ goto _LL1236;} _LL1236:
_LL1299: _temp1298= _temp1230.f1; if(( unsigned int) _temp1298 >  3u?*(( int*)
_temp1298) ==  Cyc_Absyn_VarType: 0){ _LL1301: _temp1300=(( struct Cyc_Absyn_VarType_struct*)
_temp1298)->f1; goto _LL1295;} else{ goto _LL1238;} _LL1295: _temp1294=
_temp1230.f2; if(( unsigned int) _temp1294 >  3u?*(( int*) _temp1294) ==  Cyc_Absyn_VarType:
0){ _LL1297: _temp1296=(( struct Cyc_Absyn_VarType_struct*) _temp1294)->f1; goto
_LL1237;} else{ goto _LL1238;} _LL1238: _LL1311: _temp1310= _temp1230.f1; if((
unsigned int) _temp1310 >  3u?*(( int*) _temp1310) ==  Cyc_Absyn_StructType: 0){
_LL1317: _temp1316=(( struct Cyc_Absyn_StructType_struct*) _temp1310)->f1; goto
_LL1315; _LL1315: _temp1314=(( struct Cyc_Absyn_StructType_struct*) _temp1310)->f2;
goto _LL1313; _LL1313: _temp1312=(( struct Cyc_Absyn_StructType_struct*)
_temp1310)->f3; goto _LL1303;} else{ goto _LL1240;} _LL1303: _temp1302=
_temp1230.f2; if(( unsigned int) _temp1302 >  3u?*(( int*) _temp1302) ==  Cyc_Absyn_StructType:
0){ _LL1309: _temp1308=(( struct Cyc_Absyn_StructType_struct*) _temp1302)->f1;
goto _LL1307; _LL1307: _temp1306=(( struct Cyc_Absyn_StructType_struct*)
_temp1302)->f2; goto _LL1305; _LL1305: _temp1304=(( struct Cyc_Absyn_StructType_struct*)
_temp1302)->f3; goto _LL1239;} else{ goto _LL1240;} _LL1240: _LL1323: _temp1322=
_temp1230.f1; if(( unsigned int) _temp1322 >  3u?*(( int*) _temp1322) ==  Cyc_Absyn_EnumType:
0){ _LL1325: _temp1324=(( struct Cyc_Absyn_EnumType_struct*) _temp1322)->f1;
goto _LL1319;} else{ goto _LL1242;} _LL1319: _temp1318= _temp1230.f2; if((
unsigned int) _temp1318 >  3u?*(( int*) _temp1318) ==  Cyc_Absyn_EnumType: 0){
_LL1321: _temp1320=(( struct Cyc_Absyn_EnumType_struct*) _temp1318)->f1; goto
_LL1241;} else{ goto _LL1242;} _LL1242: _LL1331: _temp1330= _temp1230.f1; if((
unsigned int) _temp1330 >  3u?*(( int*) _temp1330) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1333: _temp1332=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1330)->f1;
goto _LL1327;} else{ goto _LL1244;} _LL1327: _temp1326= _temp1230.f2; if((
unsigned int) _temp1326 >  3u?*(( int*) _temp1326) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1329: _temp1328=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1326)->f1;
goto _LL1243;} else{ goto _LL1244;} _LL1244: _LL1343: _temp1342= _temp1230.f1;
if(( unsigned int) _temp1342 >  3u?*(( int*) _temp1342) ==  Cyc_Absyn_UnionType:
0){ _LL1349: _temp1348=(( struct Cyc_Absyn_UnionType_struct*) _temp1342)->f1;
goto _LL1347; _LL1347: _temp1346=(( struct Cyc_Absyn_UnionType_struct*)
_temp1342)->f2; goto _LL1345; _LL1345: _temp1344=(( struct Cyc_Absyn_UnionType_struct*)
_temp1342)->f3; goto _LL1335;} else{ goto _LL1246;} _LL1335: _temp1334=
_temp1230.f2; if(( unsigned int) _temp1334 >  3u?*(( int*) _temp1334) ==  Cyc_Absyn_UnionType:
0){ _LL1341: _temp1340=(( struct Cyc_Absyn_UnionType_struct*) _temp1334)->f1;
goto _LL1339; _LL1339: _temp1338=(( struct Cyc_Absyn_UnionType_struct*)
_temp1334)->f2; goto _LL1337; _LL1337: _temp1336=(( struct Cyc_Absyn_UnionType_struct*)
_temp1334)->f3; goto _LL1245;} else{ goto _LL1246;} _LL1246: _LL1364: _temp1363=
_temp1230.f1; if(( unsigned int) _temp1363 >  3u?*(( int*) _temp1363) ==  Cyc_Absyn_TunionType:
0){ _LL1366: _temp1365=(( struct Cyc_Absyn_TunionType_struct*) _temp1363)->f1;
_LL1372: _temp1371=( void*) _temp1365.tunion_info; if(*(( int*) _temp1371) == 
Cyc_Absyn_KnownTunion){ _LL1374: _temp1373=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1371)->f1; _temp1375=* _temp1373; goto _LL1370;} else{ goto _LL1248;}
_LL1370: _temp1369= _temp1365.targs; goto _LL1368; _LL1368: _temp1367=( void*)
_temp1365.rgn; goto _LL1351;} else{ goto _LL1248;} _LL1351: _temp1350= _temp1230.f2;
if(( unsigned int) _temp1350 >  3u?*(( int*) _temp1350) ==  Cyc_Absyn_TunionType:
0){ _LL1353: _temp1352=(( struct Cyc_Absyn_TunionType_struct*) _temp1350)->f1;
_LL1359: _temp1358=( void*) _temp1352.tunion_info; if(*(( int*) _temp1358) == 
Cyc_Absyn_KnownTunion){ _LL1361: _temp1360=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1358)->f1; _temp1362=* _temp1360; goto _LL1357;} else{ goto _LL1248;}
_LL1357: _temp1356= _temp1352.targs; goto _LL1355; _LL1355: _temp1354=( void*)
_temp1352.rgn; goto _LL1247;} else{ goto _LL1248;} _LL1248: _LL1389: _temp1388=
_temp1230.f1; if(( unsigned int) _temp1388 >  3u?*(( int*) _temp1388) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1391: _temp1390=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1388)->f1;
_LL1395: _temp1394=( void*) _temp1390.field_info; if(*(( int*) _temp1394) == 
Cyc_Absyn_KnownTunionfield){ _LL1399: _temp1398=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1394)->f1; goto _LL1397; _LL1397: _temp1396=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1394)->f2; goto _LL1393;} else{ goto _LL1250;} _LL1393: _temp1392=
_temp1390.targs; goto _LL1377;} else{ goto _LL1250;} _LL1377: _temp1376=
_temp1230.f2; if(( unsigned int) _temp1376 >  3u?*(( int*) _temp1376) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1379: _temp1378=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1376)->f1;
_LL1383: _temp1382=( void*) _temp1378.field_info; if(*(( int*) _temp1382) == 
Cyc_Absyn_KnownTunionfield){ _LL1387: _temp1386=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1382)->f1; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1382)->f2; goto _LL1381;} else{ goto _LL1250;} _LL1381: _temp1380=
_temp1378.targs; goto _LL1249;} else{ goto _LL1250;} _LL1250: _LL1415: _temp1414=
_temp1230.f1; if(( unsigned int) _temp1414 >  3u?*(( int*) _temp1414) ==  Cyc_Absyn_PointerType:
0){ _LL1417: _temp1416=(( struct Cyc_Absyn_PointerType_struct*) _temp1414)->f1;
_LL1427: _temp1426=( void*) _temp1416.elt_typ; goto _LL1425; _LL1425: _temp1424=(
void*) _temp1416.rgn_typ; goto _LL1423; _LL1423: _temp1422= _temp1416.nullable;
goto _LL1421; _LL1421: _temp1420= _temp1416.tq; goto _LL1419; _LL1419: _temp1418=
_temp1416.bounds; goto _LL1401;} else{ goto _LL1252;} _LL1401: _temp1400=
_temp1230.f2; if(( unsigned int) _temp1400 >  3u?*(( int*) _temp1400) ==  Cyc_Absyn_PointerType:
0){ _LL1403: _temp1402=(( struct Cyc_Absyn_PointerType_struct*) _temp1400)->f1;
_LL1413: _temp1412=( void*) _temp1402.elt_typ; goto _LL1411; _LL1411: _temp1410=(
void*) _temp1402.rgn_typ; goto _LL1409; _LL1409: _temp1408= _temp1402.nullable;
goto _LL1407; _LL1407: _temp1406= _temp1402.tq; goto _LL1405; _LL1405: _temp1404=
_temp1402.bounds; goto _LL1251;} else{ goto _LL1252;} _LL1252: _LL1435:
_temp1434= _temp1230.f1; if(( unsigned int) _temp1434 >  3u?*(( int*) _temp1434)
==  Cyc_Absyn_IntType: 0){ _LL1439: _temp1438=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1434)->f1; goto _LL1437; _LL1437: _temp1436=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1434)->f2; goto _LL1429;} else{ goto _LL1254;} _LL1429: _temp1428=
_temp1230.f2; if(( unsigned int) _temp1428 >  3u?*(( int*) _temp1428) ==  Cyc_Absyn_IntType:
0){ _LL1433: _temp1432=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1428)->f1;
goto _LL1431; _LL1431: _temp1430=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1428)->f2; goto _LL1253;} else{ goto _LL1254;} _LL1254: _LL1443: _temp1442=
_temp1230.f1; if( _temp1442 == ( void*) Cyc_Absyn_FloatType){ goto _LL1441;}
else{ goto _LL1256;} _LL1441: _temp1440= _temp1230.f2; if( _temp1440 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1255;} else{ goto _LL1256;} _LL1256: _LL1449:
_temp1448= _temp1230.f1; if(( unsigned int) _temp1448 >  3u?*(( int*) _temp1448)
==  Cyc_Absyn_DoubleType: 0){ _LL1451: _temp1450=(( struct Cyc_Absyn_DoubleType_struct*)
_temp1448)->f1; goto _LL1445;} else{ goto _LL1258;} _LL1445: _temp1444=
_temp1230.f2; if(( unsigned int) _temp1444 >  3u?*(( int*) _temp1444) ==  Cyc_Absyn_DoubleType:
0){ _LL1447: _temp1446=(( struct Cyc_Absyn_DoubleType_struct*) _temp1444)->f1;
goto _LL1257;} else{ goto _LL1258;} _LL1258: _LL1457: _temp1456= _temp1230.f1;
if(( unsigned int) _temp1456 >  3u?*(( int*) _temp1456) ==  Cyc_Absyn_SizeofType:
0){ _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp1456)->f1;
goto _LL1453;} else{ goto _LL1260;} _LL1453: _temp1452= _temp1230.f2; if((
unsigned int) _temp1452 >  3u?*(( int*) _temp1452) ==  Cyc_Absyn_SizeofType: 0){
_LL1455: _temp1454=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp1452)->f1;
goto _LL1259;} else{ goto _LL1260;} _LL1260: _LL1469: _temp1468= _temp1230.f1;
if(( unsigned int) _temp1468 >  3u?*(( int*) _temp1468) ==  Cyc_Absyn_ArrayType:
0){ _LL1475: _temp1474=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1468)->f1;
goto _LL1473; _LL1473: _temp1472=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1468)->f2; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1468)->f3; goto _LL1461;} else{ goto _LL1262;} _LL1461: _temp1460=
_temp1230.f2; if(( unsigned int) _temp1460 >  3u?*(( int*) _temp1460) ==  Cyc_Absyn_ArrayType:
0){ _LL1467: _temp1466=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1460)->f1;
goto _LL1465; _LL1465: _temp1464=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1460)->f2; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1460)->f3; goto _LL1261;} else{ goto _LL1262;} _LL1262: _LL1497: _temp1496=
_temp1230.f1; if(( unsigned int) _temp1496 >  3u?*(( int*) _temp1496) ==  Cyc_Absyn_FnType:
0){ _LL1499: _temp1498=(( struct Cyc_Absyn_FnType_struct*) _temp1496)->f1;
_LL1515: _temp1514= _temp1498.tvars; goto _LL1513; _LL1513: _temp1512= _temp1498.effect;
goto _LL1511; _LL1511: _temp1510=( void*) _temp1498.ret_typ; goto _LL1509;
_LL1509: _temp1508= _temp1498.args; goto _LL1507; _LL1507: _temp1506= _temp1498.c_varargs;
goto _LL1505; _LL1505: _temp1504= _temp1498.cyc_varargs; goto _LL1503; _LL1503:
_temp1502= _temp1498.rgn_po; goto _LL1501; _LL1501: _temp1500= _temp1498.attributes;
goto _LL1477;} else{ goto _LL1264;} _LL1477: _temp1476= _temp1230.f2; if((
unsigned int) _temp1476 >  3u?*(( int*) _temp1476) ==  Cyc_Absyn_FnType: 0){
_LL1479: _temp1478=(( struct Cyc_Absyn_FnType_struct*) _temp1476)->f1; _LL1495:
_temp1494= _temp1478.tvars; goto _LL1493; _LL1493: _temp1492= _temp1478.effect;
goto _LL1491; _LL1491: _temp1490=( void*) _temp1478.ret_typ; goto _LL1489;
_LL1489: _temp1488= _temp1478.args; goto _LL1487; _LL1487: _temp1486= _temp1478.c_varargs;
goto _LL1485; _LL1485: _temp1484= _temp1478.cyc_varargs; goto _LL1483; _LL1483:
_temp1482= _temp1478.rgn_po; goto _LL1481; _LL1481: _temp1480= _temp1478.attributes;
goto _LL1263;} else{ goto _LL1264;} _LL1264: _LL1521: _temp1520= _temp1230.f1;
if(( unsigned int) _temp1520 >  3u?*(( int*) _temp1520) ==  Cyc_Absyn_TupleType:
0){ _LL1523: _temp1522=(( struct Cyc_Absyn_TupleType_struct*) _temp1520)->f1;
goto _LL1517;} else{ goto _LL1266;} _LL1517: _temp1516= _temp1230.f2; if((
unsigned int) _temp1516 >  3u?*(( int*) _temp1516) ==  Cyc_Absyn_TupleType: 0){
_LL1519: _temp1518=(( struct Cyc_Absyn_TupleType_struct*) _temp1516)->f1; goto
_LL1265;} else{ goto _LL1266;} _LL1266: _LL1529: _temp1528= _temp1230.f1; if((
unsigned int) _temp1528 >  3u?*(( int*) _temp1528) ==  Cyc_Absyn_AnonStructType:
0){ _LL1531: _temp1530=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1528)->f1;
goto _LL1525;} else{ goto _LL1268;} _LL1525: _temp1524= _temp1230.f2; if((
unsigned int) _temp1524 >  3u?*(( int*) _temp1524) ==  Cyc_Absyn_AnonStructType:
0){ _LL1527: _temp1526=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1524)->f1;
goto _LL1267;} else{ goto _LL1268;} _LL1268: _LL1537: _temp1536= _temp1230.f1;
if(( unsigned int) _temp1536 >  3u?*(( int*) _temp1536) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1539: _temp1538=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1536)->f1;
goto _LL1533;} else{ goto _LL1270;} _LL1533: _temp1532= _temp1230.f2; if((
unsigned int) _temp1532 >  3u?*(( int*) _temp1532) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1535: _temp1534=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1532)->f1;
goto _LL1269;} else{ goto _LL1270;} _LL1270: _LL1543: _temp1542= _temp1230.f1;
if( _temp1542 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1541;} else{ goto _LL1272;}
_LL1541: _temp1540= _temp1230.f2; if( _temp1540 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1271;} else{ goto _LL1272;} _LL1272: _LL1549: _temp1548= _temp1230.f1;
if(( unsigned int) _temp1548 >  3u?*(( int*) _temp1548) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1551: _temp1550=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1548)->f1; goto _LL1545;} else{ goto _LL1274;} _LL1545: _temp1544=
_temp1230.f2; if(( unsigned int) _temp1544 >  3u?*(( int*) _temp1544) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1547: _temp1546=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1544)->f1; goto _LL1273;} else{ goto _LL1274;} _LL1274: _LL1553: _temp1552=
_temp1230.f1; if(( unsigned int) _temp1552 >  3u?*(( int*) _temp1552) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1275;} else{ goto _LL1276;} _LL1276: _LL1555: _temp1554= _temp1230.f2;
if(( unsigned int) _temp1554 >  3u?*(( int*) _temp1554) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1277;} else{ goto _LL1278;} _LL1278: _LL1557: _temp1556= _temp1230.f1;
if(( unsigned int) _temp1556 >  3u?*(( int*) _temp1556) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1279;} else{ goto _LL1280;} _LL1280: _LL1559: _temp1558= _temp1230.f1;
if(( unsigned int) _temp1558 >  3u?*(( int*) _temp1558) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1281;} else{ goto _LL1282;} _LL1282: _LL1561: _temp1560= _temp1230.f2;
if(( unsigned int) _temp1560 >  3u?*(( int*) _temp1560) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1283;} else{ goto _LL1284;} _LL1284: _LL1563: _temp1562= _temp1230.f2;
if(( unsigned int) _temp1562 >  3u?*(( int*) _temp1562) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1285;} else{ goto _LL1286;} _LL1286: goto _LL1287; _LL1233: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1235: return; _LL1237: { struct _tagged_arr* _temp1564=
_temp1300->name; struct _tagged_arr* _temp1565= _temp1296->name; int _temp1566=*((
int*) _check_null( _temp1300->identity)); int _temp1567=*(( int*) _check_null(
_temp1296->identity)); void* _temp1568= Cyc_Tcutil_tvar_kind( _temp1300); void*
_temp1569= Cyc_Tcutil_tvar_kind( _temp1296); if( _temp1567 ==  _temp1566? Cyc_Std_zstrptrcmp(
_temp1564, _temp1565) ==  0: 0){ if( _temp1568 !=  _temp1569){({ struct Cyc_Std_String_pa_struct
_temp1573; _temp1573.tag= Cyc_Std_String_pa; _temp1573.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( _temp1569);{ struct Cyc_Std_String_pa_struct _temp1572;
_temp1572.tag= Cyc_Std_String_pa; _temp1572.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string(
_temp1568);{ struct Cyc_Std_String_pa_struct _temp1571; _temp1571.tag= Cyc_Std_String_pa;
_temp1571.f1=( struct _tagged_arr)* _temp1564;{ void* _temp1570[ 3u]={&
_temp1571,& _temp1572,& _temp1573};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1570, sizeof( void*), 3u));}}}});}
return;} Cyc_Tcutil_failure_reason= _tag_arr("(variable types are not the same)",
sizeof( unsigned char), 34u); goto _LL1231;} _LL1239: if((( _temp1308 !=  0?
_temp1316 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1308),( struct _tuple1*) _check_null( _temp1316)) ==  0: 0)? 1:( _temp1308
==  0? _temp1316 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1306, _temp1314);
return;} Cyc_Tcutil_failure_reason= _tag_arr("(different struct types)", sizeof(
unsigned char), 25u); goto _LL1231; _LL1241: if( Cyc_Absyn_qvar_cmp( _temp1324,
_temp1320) ==  0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different enum types)",
sizeof( unsigned char), 23u); goto _LL1231; _LL1243: { int bad= 0; for( 0;
_temp1332 !=  0? _temp1328 !=  0: 0;( _temp1332= _temp1332->tl, _temp1328=
_temp1328->tl)){ struct Cyc_Absyn_Enumfield* _temp1574=( struct Cyc_Absyn_Enumfield*)
_temp1332->hd; struct Cyc_Absyn_Enumfield* _temp1575=( struct Cyc_Absyn_Enumfield*)
_temp1328->hd; if( Cyc_Absyn_qvar_cmp( _temp1574->name, _temp1575->name) !=  0){
Cyc_Tcutil_failure_reason= _tag_arr("(different names for enum fields)", sizeof(
unsigned char), 34u); bad= 1; break;} if( _temp1574->tag ==  _temp1575->tag){
continue;} if( _temp1574->tag ==  0? 1: _temp1575->tag ==  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)", sizeof( unsigned char), 39u);
bad= 1; break;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1574->tag)) !=  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1575->tag))){ Cyc_Tcutil_failure_reason= _tag_arr("(different tag values for enum fields)",
sizeof( unsigned char), 39u); bad= 1; break;}} if( bad){ goto _LL1231;} if(
_temp1332 ==  0? _temp1328 ==  0: 0){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)", sizeof( unsigned char), 39u);
goto _LL1231;} _LL1245: if((( _temp1340 !=  0? _temp1348 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1340),( struct _tuple1*) _check_null(
_temp1348)) ==  0: 0)? 1:( _temp1340 ==  0? _temp1348 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1338, _temp1346); return;} Cyc_Tcutil_failure_reason= _tag_arr("(different union types)",
sizeof( unsigned char), 24u); goto _LL1231; _LL1247: if( _temp1375 ==  _temp1362?
1: Cyc_Absyn_qvar_cmp( _temp1375->name, _temp1362->name) ==  0){ Cyc_Tcutil_unify_it(
_temp1354, _temp1367); Cyc_Tcutil_unify_list( _temp1356, _temp1369); return;}
Cyc_Tcutil_failure_reason= _tag_arr("(different tunion types)", sizeof(
unsigned char), 25u); goto _LL1231; _LL1249: if(( _temp1398 ==  _temp1386? 1:
Cyc_Absyn_qvar_cmp( _temp1398->name, _temp1386->name) ==  0)? _temp1396 == 
_temp1384? 1: Cyc_Absyn_qvar_cmp( _temp1396->name, _temp1384->name) ==  0: 0){
Cyc_Tcutil_unify_list( _temp1380, _temp1392); return;} Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)", sizeof( unsigned char), 31u); goto
_LL1231; _LL1251: Cyc_Tcutil_unify_it( _temp1412, _temp1426); Cyc_Tcutil_unify_it(
_temp1424, _temp1410); Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_unify_tqual(
_temp1406, _temp1420); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1404, _temp1418, _tag_arr("(different pointer bounds)", sizeof(
unsigned char), 27u));{ void* _temp1576=( void*)( Cyc_Absyn_compress_conref(
_temp1404))->v; void* _temp1582; _LL1578: if(( unsigned int) _temp1576 >  1u?*((
int*) _temp1576) ==  Cyc_Absyn_Eq_constr: 0){ _LL1583: _temp1582=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1576)->f1; if( _temp1582 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1579;} else{ goto _LL1580;}} else{ goto _LL1580;}
_LL1580: goto _LL1581; _LL1579: return; _LL1581: goto _LL1577; _LL1577:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y,
struct _tagged_arr reason)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp1408, _temp1422, _tag_arr("(different pointer types)", sizeof(
unsigned char), 26u)); return; _LL1253: if( _temp1432 ==  _temp1438? _temp1430
==  _temp1436: 0){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different integral types)",
sizeof( unsigned char), 27u); goto _LL1231; _LL1255: return; _LL1257: if(
_temp1450 ==  _temp1446){ return;} goto _LL1231; _LL1259: Cyc_Tcutil_unify_it(
_temp1458, _temp1454); return; _LL1261: Cyc_Tcutil_unify_tqual( _temp1464,
_temp1472); Cyc_Tcutil_unify_it( _temp1466, _temp1474); if( _temp1470 == 
_temp1462){ return;} if( _temp1470 ==  0? 1: _temp1462 ==  0){ goto _LL1231;}
if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1470)) ==  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1462))){ return;} Cyc_Tcutil_failure_reason= _tag_arr("(different array sizes)",
sizeof( unsigned char), 24u); goto _LL1231; _LL1263: { int done= 0;{ struct
_RegionHandle _temp1584= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1584; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1494
!=  0) { if( _temp1514 ==  0){ Cyc_Tcutil_failure_reason= _tag_arr("(second function type has too few type variables)",
sizeof( unsigned char), 50u);( int) _throw(( void*) Cyc_Tcutil_Unify);}{ void*
_temp1585= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) _temp1494->hd); void*
_temp1586= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) _temp1514->hd); if(
_temp1585 !=  _temp1586){ Cyc_Tcutil_failure_reason=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp1590; _temp1590.tag= Cyc_Std_String_pa;
_temp1590.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1586);{ struct
Cyc_Std_String_pa_struct _temp1589; _temp1589.tag= Cyc_Std_String_pa; _temp1589.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1585);{ struct Cyc_Std_String_pa_struct
_temp1588; _temp1588.tag= Cyc_Std_String_pa; _temp1588.f1=( struct _tagged_arr)
Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*) _temp1494->hd);{ void*
_temp1587[ 3u]={& _temp1588,& _temp1589,& _temp1590}; Cyc_Std_aprintf( _tag_arr("(type var %s has different kinds %s and %s)",
sizeof( unsigned char), 44u), _tag_arr( _temp1587, sizeof( void*), 3u));}}}});(
int) _throw(( void*) Cyc_Tcutil_Unify);} inst=({ struct Cyc_List_List* _temp1591=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1591->hd=( void*)({ struct _tuple5* _temp1592=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1592->f1=( struct Cyc_Absyn_Tvar*)
_temp1514->hd; _temp1592->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1593=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1593[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1594; _temp1594.tag= Cyc_Absyn_VarType;
_temp1594.f1=( struct Cyc_Absyn_Tvar*) _temp1494->hd; _temp1594;}); _temp1593;});
_temp1592;}); _temp1591->tl= inst; _temp1591;}); _temp1494= _temp1494->tl;
_temp1514= _temp1514->tl;}} if( _temp1514 !=  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too many type variables)", sizeof(
unsigned char), 51u); _npop_handler( 0u); goto _LL1231;} if( inst !=  0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1595=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1595[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1596; _temp1596.tag= Cyc_Absyn_FnType; _temp1596.f1=({ struct Cyc_Absyn_FnInfo
_temp1597; _temp1597.tvars= 0; _temp1597.effect= _temp1492; _temp1597.ret_typ=(
void*) _temp1490; _temp1597.args= _temp1488; _temp1597.c_varargs= _temp1486;
_temp1597.cyc_varargs= _temp1484; _temp1597.rgn_po= _temp1482; _temp1597.attributes=
_temp1480; _temp1597;}); _temp1596;}); _temp1595;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1598=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1598[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1599; _temp1599.tag= Cyc_Absyn_FnType; _temp1599.f1=({ struct Cyc_Absyn_FnInfo
_temp1600; _temp1600.tvars= 0; _temp1600.effect= _temp1512; _temp1600.ret_typ=(
void*) _temp1510; _temp1600.args= _temp1508; _temp1600.c_varargs= _temp1506;
_temp1600.cyc_varargs= _temp1504; _temp1600.rgn_po= _temp1502; _temp1600.attributes=
_temp1500; _temp1600;}); _temp1599;}); _temp1598;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1490, _temp1510); for( 0;
_temp1488 !=  0? _temp1508 !=  0: 0;( _temp1488= _temp1488->tl, _temp1508=
_temp1508->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*) _temp1488->hd)).f2,(*((
struct _tuple2*) _temp1508->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)
_temp1488->hd)).f3,(*(( struct _tuple2*) _temp1508->hd)).f3);} Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; if( _temp1488 !=  0? 1: _temp1508 !=  0){ Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)", sizeof(
unsigned char), 52u); goto _LL1231;} if( _temp1486 !=  _temp1506){ Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)", sizeof( unsigned char), 41u);
goto _LL1231;}{ int bad_cyc_vararg= 0;{ struct _tuple11 _temp1602=({ struct
_tuple11 _temp1601; _temp1601.f1= _temp1484; _temp1601.f2= _temp1504; _temp1601;});
struct Cyc_Absyn_VarargInfo* _temp1612; struct Cyc_Absyn_VarargInfo* _temp1614;
struct Cyc_Absyn_VarargInfo* _temp1616; struct Cyc_Absyn_VarargInfo* _temp1618;
struct Cyc_Absyn_VarargInfo* _temp1620; struct Cyc_Absyn_VarargInfo _temp1622;
int _temp1623; void* _temp1625; struct Cyc_Absyn_Tqual _temp1627; struct Cyc_Core_Opt*
_temp1629; struct Cyc_Absyn_VarargInfo* _temp1631; struct Cyc_Absyn_VarargInfo
_temp1633; int _temp1634; void* _temp1636; struct Cyc_Absyn_Tqual _temp1638;
struct Cyc_Core_Opt* _temp1640; _LL1604: _LL1615: _temp1614= _temp1602.f1; if(
_temp1614 ==  0){ goto _LL1613;} else{ goto _LL1606;} _LL1613: _temp1612=
_temp1602.f2; if( _temp1612 ==  0){ goto _LL1605;} else{ goto _LL1606;} _LL1606:
_LL1617: _temp1616= _temp1602.f1; if( _temp1616 ==  0){ goto _LL1607;} else{
goto _LL1608;} _LL1608: _LL1619: _temp1618= _temp1602.f2; if( _temp1618 ==  0){
goto _LL1609;} else{ goto _LL1610;} _LL1610: _LL1632: _temp1631= _temp1602.f1;
if( _temp1631 ==  0){ goto _LL1603;} else{ _temp1633=* _temp1631; _LL1641:
_temp1640= _temp1633.name; goto _LL1639; _LL1639: _temp1638= _temp1633.tq; goto
_LL1637; _LL1637: _temp1636=( void*) _temp1633.type; goto _LL1635; _LL1635:
_temp1634= _temp1633.inject; goto _LL1621;} _LL1621: _temp1620= _temp1602.f2;
if( _temp1620 ==  0){ goto _LL1603;} else{ _temp1622=* _temp1620; _LL1630:
_temp1629= _temp1622.name; goto _LL1628; _LL1628: _temp1627= _temp1622.tq; goto
_LL1626; _LL1626: _temp1625=( void*) _temp1622.type; goto _LL1624; _LL1624:
_temp1623= _temp1622.inject; goto _LL1611;} _LL1605: goto _LL1603; _LL1607: goto
_LL1609; _LL1609: bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type takes varargs)",
sizeof( unsigned char), 39u); goto _LL1603; _LL1611: Cyc_Tcutil_unify_tqual(
_temp1638, _temp1627); Cyc_Tcutil_unify_it( _temp1636, _temp1625); if( _temp1634
!=  _temp1623){ bad_cyc_vararg= 1; Cyc_Tcutil_failure_reason= _tag_arr("(only one function type injects varargs)",
sizeof( unsigned char), 41u);} goto _LL1603; _LL1603:;} if( bad_cyc_vararg){
goto _LL1231;}{ int bad_effect= 0;{ struct _tuple12 _temp1643=({ struct _tuple12
_temp1642; _temp1642.f1= _temp1492; _temp1642.f2= _temp1512; _temp1642;});
struct Cyc_Core_Opt* _temp1653; struct Cyc_Core_Opt* _temp1655; struct Cyc_Core_Opt*
_temp1657; struct Cyc_Core_Opt* _temp1659; _LL1645: _LL1656: _temp1655=
_temp1643.f1; if( _temp1655 ==  0){ goto _LL1654;} else{ goto _LL1647;} _LL1654:
_temp1653= _temp1643.f2; if( _temp1653 ==  0){ goto _LL1646;} else{ goto _LL1647;}
_LL1647: _LL1658: _temp1657= _temp1643.f1; if( _temp1657 ==  0){ goto _LL1648;}
else{ goto _LL1649;} _LL1649: _LL1660: _temp1659= _temp1643.f2; if( _temp1659 == 
0){ goto _LL1650;} else{ goto _LL1651;} _LL1651: goto _LL1652; _LL1646: goto
_LL1644; _LL1648: goto _LL1650; _LL1650: bad_effect= 1; goto _LL1644; _LL1652:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1492))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1512))->v); goto _LL1644; _LL1644:;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; if( bad_effect){ Cyc_Tcutil_failure_reason= _tag_arr("(function type effects do not unify)",
sizeof( unsigned char), 37u); goto _LL1231;} if( ! Cyc_Tcutil_same_atts(
_temp1500, _temp1480)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different attributes)",
sizeof( unsigned char), 43u); goto _LL1231;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1502, _temp1482)){ Cyc_Tcutil_failure_reason= _tag_arr("(function types have different region lifetime orderings)",
sizeof( unsigned char), 58u); goto _LL1231;} return;}}} _LL1265: for( 0;
_temp1518 !=  0? _temp1522 !=  0: 0;( _temp1518= _temp1518->tl, _temp1522=
_temp1522->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*) _temp1518->hd)).f1,(*((
struct _tuple4*) _temp1522->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)
_temp1518->hd)).f2,(*(( struct _tuple4*) _temp1522->hd)).f2);} if( _temp1518 == 
0? _temp1522 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(tuple types have different numbers of components)",
sizeof( unsigned char), 51u); goto _LL1231; _LL1267: _temp1538= _temp1530;
_temp1534= _temp1526; goto _LL1269; _LL1269: for( 0; _temp1534 !=  0? _temp1538
!=  0: 0;( _temp1534= _temp1534->tl, _temp1538= _temp1538->tl)){ struct Cyc_Absyn_Structfield*
_temp1661=( struct Cyc_Absyn_Structfield*) _temp1534->hd; struct Cyc_Absyn_Structfield*
_temp1662=( struct Cyc_Absyn_Structfield*) _temp1538->hd; if( Cyc_Std_zstrptrcmp(
_temp1661->name, _temp1662->name) !=  0){ Cyc_Tcutil_failure_reason= _tag_arr("(different member names in anonymous structs/unions)",
sizeof( unsigned char), 53u);( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1661->tq, _temp1662->tq); Cyc_Tcutil_unify_it(( void*) _temp1661->type,(
void*) _temp1662->type); if( ! Cyc_Tcutil_same_atts( _temp1661->attributes,
_temp1662->attributes)){ Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure= t2;
Cyc_Tcutil_failure_reason= _tag_arr("(different attributes on member)", sizeof(
unsigned char), 33u);( int) _throw(( void*) Cyc_Tcutil_Unify);} if((( _temp1661->width
!=  0? _temp1662->width ==  0: 0)? 1:( _temp1662->width !=  0? _temp1661->width
==  0: 0))? 1:(( _temp1661->width !=  0? _temp1662->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1661->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1662->width)): 0)){ Cyc_Tcutil_t1_failure=
t1; Cyc_Tcutil_t2_failure= t2; Cyc_Tcutil_failure_reason= _tag_arr("(different bitfield widths on member)",
sizeof( unsigned char), 38u);( int) _throw(( void*) Cyc_Tcutil_Unify);}} if(
_temp1534 ==  0? _temp1538 ==  0: 0){ return;} Cyc_Tcutil_t1_failure= t1; Cyc_Tcutil_t2_failure=
t2; Cyc_Tcutil_failure_reason= _tag_arr("(different number of members)", sizeof(
unsigned char), 30u); goto _LL1231; _LL1271: return; _LL1273: Cyc_Tcutil_unify_it(
_temp1550, _temp1546); return; _LL1275: goto _LL1277; _LL1277: goto _LL1279;
_LL1279: goto _LL1281; _LL1281: goto _LL1283; _LL1283: goto _LL1285; _LL1285:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)", sizeof( unsigned char), 22u); goto _LL1231;
_LL1287: goto _LL1231; _LL1231:;}( int) _throw(( void*) Cyc_Tcutil_Unify);} int
Cyc_Tcutil_opt_cmp( int(* cmp)( void*, void*), struct Cyc_Core_Opt* a1, struct
Cyc_Core_Opt* a2){ if( a1 ==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return
- 1;} if( a1 !=  0? a2 ==  0: 0){ return 1;} return cmp(( void*)(( struct Cyc_Core_Opt*)
_check_null( a1))->v,( void*)(( struct Cyc_Core_Opt*) _check_null( a2))->v);}
int Cyc_Tcutil_star_cmp( int(* cmp)( void*, void*), void* a1, void* a2){ if( a1
==  a2){ return 0;} if( a1 ==  0? a2 !=  0: 0){ return - 1;} if( a1 !=  0? a2 == 
0: 0){ return 1;} return cmp(( void*) _check_null( a1),( void*) _check_null( a2));}
int Cyc_Tcutil_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2){ if( l1 ==  l2){ return 0;} for( 0; l1 !=  0? l2 !=  0:
0;( l1= l1->tl, l2= l2->tl)){ int _temp1663= cmp(( void*) l1->hd,( void*) l2->hd);
if( _temp1663 !=  0){ return _temp1663;}} if( l2 !=  0){ return - 1;} if( l1 != 
0){ return 1;} return 0;} static int Cyc_Tcutil_tqual_cmp( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ int _temp1664=( tq1.q_const + ( tq1.q_volatile
<<  1)) + ( tq1.q_restrict <<  2); int _temp1665=( tq2.q_const + ( tq2.q_volatile
<<  1)) + ( tq2.q_restrict <<  2); return Cyc_Core_intcmp( _temp1664, _temp1665);}
static int Cyc_Tcutil_conrefs_cmp( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref(
y); if( x ==  y){ return 0;}{ void* _temp1666=( void*) x->v; void* _temp1674;
_LL1668: if( _temp1666 == ( void*) Cyc_Absyn_No_constr){ goto _LL1669;} else{
goto _LL1670;} _LL1670: if(( unsigned int) _temp1666 >  1u?*(( int*) _temp1666)
==  Cyc_Absyn_Forward_constr: 0){ goto _LL1671;} else{ goto _LL1672;} _LL1672:
if(( unsigned int) _temp1666 >  1u?*(( int*) _temp1666) ==  Cyc_Absyn_Eq_constr:
0){ _LL1675: _temp1674=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1666)->f1;
goto _LL1673;} else{ goto _LL1667;} _LL1669: return - 1; _LL1671:( int) _throw(({
void* _temp1676[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress",
sizeof( unsigned char), 37u), _tag_arr( _temp1676, sizeof( void*), 0u));}));
_LL1673: { void* _temp1677=( void*) y->v; void* _temp1685; _LL1679: if(
_temp1677 == ( void*) Cyc_Absyn_No_constr){ goto _LL1680;} else{ goto _LL1681;}
_LL1681: if(( unsigned int) _temp1677 >  1u?*(( int*) _temp1677) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL1682;} else{ goto _LL1683;} _LL1683: if(( unsigned int) _temp1677 > 
1u?*(( int*) _temp1677) ==  Cyc_Absyn_Eq_constr: 0){ _LL1686: _temp1685=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1677)->f1; goto _LL1684;} else{ goto
_LL1678;} _LL1680: return 1; _LL1682:( int) _throw(({ void* _temp1687[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress(2)", sizeof(
unsigned char), 40u), _tag_arr( _temp1687, sizeof( void*), 0u));})); _LL1684:
return cmp( _temp1674, _temp1685); _LL1678:;} _LL1667:;}} static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4* tqt1, struct _tuple4* tqt2){ struct _tuple4 _temp1690; void*
_temp1691; struct Cyc_Absyn_Tqual _temp1693; struct _tuple4* _temp1688= tqt1;
_temp1690=* _temp1688; _LL1694: _temp1693= _temp1690.f1; goto _LL1692; _LL1692:
_temp1691= _temp1690.f2; goto _LL1689; _LL1689: { struct _tuple4 _temp1697; void*
_temp1698; struct Cyc_Absyn_Tqual _temp1700; struct _tuple4* _temp1695= tqt2;
_temp1697=* _temp1695; _LL1701: _temp1700= _temp1697.f1; goto _LL1699; _LL1699:
_temp1698= _temp1697.f2; goto _LL1696; _LL1696: { int _temp1702= Cyc_Tcutil_tqual_cmp(
_temp1693, _temp1700); if( _temp1702 !=  0){ return _temp1702;} return Cyc_Tcutil_typecmp(
_temp1691, _temp1698);}}} static int Cyc_Tcutil_const_uint_exp_cmp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ return Cyc_Core_intcmp(( int) Cyc_Evexp_eval_const_uint_exp(
e1),( int) Cyc_Evexp_eval_const_uint_exp( e2));} static int Cyc_Tcutil_attribute_case_number(
void* att){ void* _temp1703= att; _LL1705: if(( unsigned int) _temp1703 >  16u?*((
int*) _temp1703) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL1706;} else{ goto
_LL1707;} _LL1707: if( _temp1703 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1708;} else{ goto _LL1709;} _LL1709: if( _temp1703 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1710;} else{ goto _LL1711;} _LL1711: if( _temp1703 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1712;} else{ goto _LL1713;} _LL1713: if( _temp1703 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL1714;} else{ goto _LL1715;} _LL1715: if( _temp1703 == ( void*) Cyc_Absyn_Const_att){
goto _LL1716;} else{ goto _LL1717;} _LL1717: if(( unsigned int) _temp1703 >  16u?*((
int*) _temp1703) ==  Cyc_Absyn_Aligned_att: 0){ goto _LL1718;} else{ goto
_LL1719;} _LL1719: if( _temp1703 == ( void*) Cyc_Absyn_Packed_att){ goto _LL1720;}
else{ goto _LL1721;} _LL1721: if(( unsigned int) _temp1703 >  16u?*(( int*)
_temp1703) ==  Cyc_Absyn_Section_att: 0){ goto _LL1722;} else{ goto _LL1723;}
_LL1723: if( _temp1703 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL1724;} else{
goto _LL1725;} _LL1725: if( _temp1703 == ( void*) Cyc_Absyn_Shared_att){ goto
_LL1726;} else{ goto _LL1727;} _LL1727: if( _temp1703 == ( void*) Cyc_Absyn_Unused_att){
goto _LL1728;} else{ goto _LL1729;} _LL1729: if( _temp1703 == ( void*) Cyc_Absyn_Weak_att){
goto _LL1730;} else{ goto _LL1731;} _LL1731: if( _temp1703 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL1732;} else{ goto _LL1733;} _LL1733: if( _temp1703 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL1734;} else{ goto _LL1735;} _LL1735: if( _temp1703 == ( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL1736;} else{ goto _LL1737;} _LL1737: if( _temp1703 == ( void*) Cyc_Absyn_Constructor_att){
goto _LL1738;} else{ goto _LL1739;} _LL1739: if( _temp1703 == ( void*) Cyc_Absyn_Destructor_att){
goto _LL1740;} else{ goto _LL1741;} _LL1741: if( _temp1703 == ( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL1742;} else{ goto _LL1743;} _LL1743: if(( unsigned int) _temp1703 >  16u?*((
int*) _temp1703) ==  Cyc_Absyn_Format_att: 0){ goto _LL1744;} else{ goto _LL1704;}
_LL1706: return 0; _LL1708: return 1; _LL1710: return 2; _LL1712: return 3;
_LL1714: return 4; _LL1716: return 5; _LL1718: return 6; _LL1720: return 7;
_LL1722: return 8; _LL1724: return 9; _LL1726: return 10; _LL1728: return 11;
_LL1730: return 12; _LL1732: return 13; _LL1734: return 14; _LL1736: return 15;
_LL1738: return 16; _LL1740: return 17; _LL1742: return 18; _LL1744: return 19;
_LL1704:;} static int Cyc_Tcutil_attribute_cmp( void* att1, void* att2){ struct
_tuple8 _temp1746=({ struct _tuple8 _temp1745; _temp1745.f1= att1; _temp1745.f2=
att2; _temp1745;}); void* _temp1758; int _temp1760; void* _temp1762; int
_temp1764; void* _temp1766; int _temp1768; void* _temp1770; int _temp1772; void*
_temp1774; struct _tagged_arr _temp1776; void* _temp1778; struct _tagged_arr
_temp1780; void* _temp1782; int _temp1784; int _temp1786; void* _temp1788; void*
_temp1790; int _temp1792; int _temp1794; void* _temp1796; _LL1748: _LL1763:
_temp1762= _temp1746.f1; if(( unsigned int) _temp1762 >  16u?*(( int*) _temp1762)
==  Cyc_Absyn_Regparm_att: 0){ _LL1765: _temp1764=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp1762)->f1; goto _LL1759;} else{ goto _LL1750;} _LL1759: _temp1758=
_temp1746.f2; if(( unsigned int) _temp1758 >  16u?*(( int*) _temp1758) ==  Cyc_Absyn_Regparm_att:
0){ _LL1761: _temp1760=(( struct Cyc_Absyn_Regparm_att_struct*) _temp1758)->f1;
goto _LL1749;} else{ goto _LL1750;} _LL1750: _LL1771: _temp1770= _temp1746.f1;
if(( unsigned int) _temp1770 >  16u?*(( int*) _temp1770) ==  Cyc_Absyn_Aligned_att:
0){ _LL1773: _temp1772=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1770)->f1;
goto _LL1767;} else{ goto _LL1752;} _LL1767: _temp1766= _temp1746.f2; if((
unsigned int) _temp1766 >  16u?*(( int*) _temp1766) ==  Cyc_Absyn_Aligned_att: 0){
_LL1769: _temp1768=(( struct Cyc_Absyn_Aligned_att_struct*) _temp1766)->f1; goto
_LL1751;} else{ goto _LL1752;} _LL1752: _LL1779: _temp1778= _temp1746.f1; if((
unsigned int) _temp1778 >  16u?*(( int*) _temp1778) ==  Cyc_Absyn_Section_att: 0){
_LL1781: _temp1780=(( struct Cyc_Absyn_Section_att_struct*) _temp1778)->f1; goto
_LL1775;} else{ goto _LL1754;} _LL1775: _temp1774= _temp1746.f2; if((
unsigned int) _temp1774 >  16u?*(( int*) _temp1774) ==  Cyc_Absyn_Section_att: 0){
_LL1777: _temp1776=(( struct Cyc_Absyn_Section_att_struct*) _temp1774)->f1; goto
_LL1753;} else{ goto _LL1754;} _LL1754: _LL1791: _temp1790= _temp1746.f1; if((
unsigned int) _temp1790 >  16u?*(( int*) _temp1790) ==  Cyc_Absyn_Format_att: 0){
_LL1797: _temp1796=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1790)->f1;
goto _LL1795; _LL1795: _temp1794=(( struct Cyc_Absyn_Format_att_struct*)
_temp1790)->f2; goto _LL1793; _LL1793: _temp1792=(( struct Cyc_Absyn_Format_att_struct*)
_temp1790)->f3; goto _LL1783;} else{ goto _LL1756;} _LL1783: _temp1782=
_temp1746.f2; if(( unsigned int) _temp1782 >  16u?*(( int*) _temp1782) ==  Cyc_Absyn_Format_att:
0){ _LL1789: _temp1788=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp1782)->f1;
goto _LL1787; _LL1787: _temp1786=(( struct Cyc_Absyn_Format_att_struct*)
_temp1782)->f2; goto _LL1785; _LL1785: _temp1784=(( struct Cyc_Absyn_Format_att_struct*)
_temp1782)->f3; goto _LL1755;} else{ goto _LL1756;} _LL1756: goto _LL1757;
_LL1749: _temp1772= _temp1764; _temp1768= _temp1760; goto _LL1751; _LL1751:
return Cyc_Core_intcmp( _temp1772, _temp1768); _LL1753: return Cyc_Std_strcmp(
_temp1780, _temp1776); _LL1755: { int _temp1798= Cyc_Core_intcmp(( int)((
unsigned int) _temp1796),( int)(( unsigned int) _temp1788)); if( _temp1798 !=  0){
return _temp1798;}{ int _temp1799= Cyc_Core_intcmp( _temp1794, _temp1786); if(
_temp1799 !=  0){ return _temp1799;} return Cyc_Core_intcmp( _temp1792,
_temp1784);}} _LL1757: { int _temp1800= Cyc_Tcutil_attribute_case_number( att1);
int _temp1801= Cyc_Tcutil_attribute_case_number( att2); return Cyc_Core_intcmp(
_temp1800, _temp1801);} _LL1747:;} static int Cyc_Tcutil_structfield_cmp( struct
Cyc_Absyn_Structfield* f1, struct Cyc_Absyn_Structfield* f2){ int _temp1802= Cyc_Std_zstrptrcmp(
f1->name, f2->name); if( _temp1802 !=  0){ return _temp1802;}{ int _temp1803=
Cyc_Tcutil_tqual_cmp( f1->tq, f2->tq); if( _temp1803 !=  0){ return _temp1803;}{
int _temp1804= Cyc_Tcutil_typecmp(( void*) f1->type,( void*) f2->type); if(
_temp1804 !=  0){ return _temp1804;}{ int _temp1805= Cyc_Tcutil_list_cmp( Cyc_Tcutil_attribute_cmp,
f1->attributes, f2->attributes); if( _temp1805 !=  0){ return _temp1805;} return((
int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp*
a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
f1->width, f2->width);}}}} static int Cyc_Tcutil_enumfield_cmp( struct Cyc_Absyn_Enumfield*
e1, struct Cyc_Absyn_Enumfield* e2){ int _temp1806= Cyc_Absyn_qvar_cmp( e1->name,
e2->name); if( _temp1806 !=  0){ return _temp1806;} return(( int(*)( int(* cmp)(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*), struct Cyc_Absyn_Exp* a1, struct
Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp, e1->tag,
e2->tag);} extern int Cyc_Tcutil_typecmp( void* t1, void* t2); int Cyc_Tcutil_type_case_number(
void* t){ void* _temp1807= t; _LL1809: if( _temp1807 == ( void*) Cyc_Absyn_VoidType){
goto _LL1810;} else{ goto _LL1811;} _LL1811: if(( unsigned int) _temp1807 >  3u?*((
int*) _temp1807) ==  Cyc_Absyn_Evar: 0){ goto _LL1812;} else{ goto _LL1813;}
_LL1813: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_VarType:
0){ goto _LL1814;} else{ goto _LL1815;} _LL1815: if(( unsigned int) _temp1807 > 
3u?*(( int*) _temp1807) ==  Cyc_Absyn_TunionType: 0){ goto _LL1816;} else{ goto
_LL1817;} _LL1817: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1818;} else{ goto _LL1819;} _LL1819: if((
unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_PointerType: 0){
goto _LL1820;} else{ goto _LL1821;} _LL1821: if(( unsigned int) _temp1807 >  3u?*((
int*) _temp1807) ==  Cyc_Absyn_IntType: 0){ goto _LL1822;} else{ goto _LL1823;}
_LL1823: if( _temp1807 == ( void*) Cyc_Absyn_FloatType){ goto _LL1824;} else{
goto _LL1825;} _LL1825: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807)
==  Cyc_Absyn_DoubleType: 0){ goto _LL1826;} else{ goto _LL1827;} _LL1827: if((
unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_ArrayType: 0){
goto _LL1828;} else{ goto _LL1829;} _LL1829: if(( unsigned int) _temp1807 >  3u?*((
int*) _temp1807) ==  Cyc_Absyn_FnType: 0){ goto _LL1830;} else{ goto _LL1831;}
_LL1831: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_TupleType:
0){ goto _LL1832;} else{ goto _LL1833;} _LL1833: if(( unsigned int) _temp1807 > 
3u?*(( int*) _temp1807) ==  Cyc_Absyn_StructType: 0){ goto _LL1834;} else{ goto
_LL1835;} _LL1835: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807) == 
Cyc_Absyn_UnionType: 0){ goto _LL1836;} else{ goto _LL1837;} _LL1837: if((
unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL1838;} else{ goto _LL1839;} _LL1839: if(( unsigned int) _temp1807 > 
3u?*(( int*) _temp1807) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL1840;} else{
goto _LL1841;} _LL1841: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807)
==  Cyc_Absyn_EnumType: 0){ goto _LL1842;} else{ goto _LL1843;} _LL1843: if((
unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_AnonEnumType: 0){
goto _LL1844;} else{ goto _LL1845;} _LL1845: if(( unsigned int) _temp1807 >  3u?*((
int*) _temp1807) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL1846;} else{ goto
_LL1847;} _LL1847: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807) == 
Cyc_Absyn_TypedefType: 0){ goto _LL1848;} else{ goto _LL1849;} _LL1849: if(
_temp1807 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1850;} else{ goto _LL1851;}
_LL1851: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1852;} else{ goto _LL1853;} _LL1853: if(( unsigned int) _temp1807 > 
3u?*(( int*) _temp1807) ==  Cyc_Absyn_JoinEff: 0){ goto _LL1854;} else{ goto
_LL1855;} _LL1855: if(( unsigned int) _temp1807 >  3u?*(( int*) _temp1807) == 
Cyc_Absyn_RgnsEff: 0){ goto _LL1856;} else{ goto _LL1857;} _LL1857: if((
unsigned int) _temp1807 >  3u?*(( int*) _temp1807) ==  Cyc_Absyn_SizeofType: 0){
goto _LL1858;} else{ goto _LL1808;} _LL1810: return 0; _LL1812: return 1;
_LL1814: return 2; _LL1816: return 3; _LL1818: return 4; _LL1820: return 5;
_LL1822: return 6; _LL1824: return 7; _LL1826: return 8; _LL1828: return 9;
_LL1830: return 10; _LL1832: return 11; _LL1834: return 12; _LL1836: return 13;
_LL1838: return 14; _LL1840: return 15; _LL1842: return 16; _LL1844: return 17;
_LL1846: return 18; _LL1848: return 19; _LL1850: return 20; _LL1852: return 21;
_LL1854: return 22; _LL1856: return 23; _LL1858: return 24; _LL1808:;} int Cyc_Tcutil_shallow_typecmp_it(
void* t1, void* t2){ return Cyc_Core_intcmp( Cyc_Tcutil_type_case_number( t1),
Cyc_Tcutil_type_case_number( t2));} int Cyc_Tcutil_typecmp( void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 ==  t2){
return 0;}{ int _temp1859= Cyc_Tcutil_shallow_typecmp_it( t1, t2); if( _temp1859
!=  0){ return _temp1859;}{ struct _tuple8 _temp1861=({ struct _tuple8 _temp1860;
_temp1860.f1= t2; _temp1860.f2= t1; _temp1860;}); void* _temp1919; void*
_temp1921; void* _temp1923; void* _temp1925; void* _temp1927; struct Cyc_Absyn_Tvar*
_temp1929; void* _temp1931; struct Cyc_Absyn_Tvar* _temp1933; void* _temp1935;
struct Cyc_List_List* _temp1937; struct _tuple1* _temp1939; void* _temp1941;
struct Cyc_List_List* _temp1943; struct _tuple1* _temp1945; void* _temp1947;
struct _tuple1* _temp1949; void* _temp1951; struct _tuple1* _temp1953; void*
_temp1955; struct Cyc_List_List* _temp1957; void* _temp1959; struct Cyc_List_List*
_temp1961; void* _temp1963; struct Cyc_Absyn_Uniondecl** _temp1965; struct Cyc_List_List*
_temp1967; struct _tuple1* _temp1969; void* _temp1971; struct Cyc_Absyn_Uniondecl**
_temp1973; struct Cyc_List_List* _temp1975; struct _tuple1* _temp1977; void*
_temp1979; struct Cyc_Absyn_TunionInfo _temp1981; void* _temp1983; struct Cyc_List_List*
_temp1985; void* _temp1987; struct Cyc_Absyn_Tuniondecl** _temp1989; struct Cyc_Absyn_Tuniondecl*
_temp1991; void* _temp1992; struct Cyc_Absyn_TunionInfo _temp1994; void*
_temp1996; struct Cyc_List_List* _temp1998; void* _temp2000; struct Cyc_Absyn_Tuniondecl**
_temp2002; struct Cyc_Absyn_Tuniondecl* _temp2004; void* _temp2005; struct Cyc_Absyn_TunionFieldInfo
_temp2007; struct Cyc_List_List* _temp2009; void* _temp2011; struct Cyc_Absyn_Tunionfield*
_temp2013; struct Cyc_Absyn_Tuniondecl* _temp2015; void* _temp2017; struct Cyc_Absyn_TunionFieldInfo
_temp2019; struct Cyc_List_List* _temp2021; void* _temp2023; struct Cyc_Absyn_Tunionfield*
_temp2025; struct Cyc_Absyn_Tuniondecl* _temp2027; void* _temp2029; struct Cyc_Absyn_PtrInfo
_temp2031; struct Cyc_Absyn_Conref* _temp2033; struct Cyc_Absyn_Tqual _temp2035;
struct Cyc_Absyn_Conref* _temp2037; void* _temp2039; void* _temp2041; void*
_temp2043; struct Cyc_Absyn_PtrInfo _temp2045; struct Cyc_Absyn_Conref*
_temp2047; struct Cyc_Absyn_Tqual _temp2049; struct Cyc_Absyn_Conref* _temp2051;
void* _temp2053; void* _temp2055; void* _temp2057; void* _temp2059; void*
_temp2061; void* _temp2063; void* _temp2065; void* _temp2067; void* _temp2069;
void* _temp2071; void* _temp2073; int _temp2075; void* _temp2077; int _temp2079;
void* _temp2081; struct Cyc_Absyn_Exp* _temp2083; struct Cyc_Absyn_Tqual
_temp2085; void* _temp2087; void* _temp2089; struct Cyc_Absyn_Exp* _temp2091;
struct Cyc_Absyn_Tqual _temp2093; void* _temp2095; void* _temp2097; struct Cyc_Absyn_FnInfo
_temp2099; struct Cyc_List_List* _temp2101; struct Cyc_List_List* _temp2103;
struct Cyc_Absyn_VarargInfo* _temp2105; int _temp2107; struct Cyc_List_List*
_temp2109; void* _temp2111; struct Cyc_Core_Opt* _temp2113; struct Cyc_List_List*
_temp2115; void* _temp2117; struct Cyc_Absyn_FnInfo _temp2119; struct Cyc_List_List*
_temp2121; struct Cyc_List_List* _temp2123; struct Cyc_Absyn_VarargInfo*
_temp2125; int _temp2127; struct Cyc_List_List* _temp2129; void* _temp2131;
struct Cyc_Core_Opt* _temp2133; struct Cyc_List_List* _temp2135; void* _temp2137;
struct Cyc_List_List* _temp2139; void* _temp2141; struct Cyc_List_List*
_temp2143; void* _temp2145; struct Cyc_List_List* _temp2147; void* _temp2149;
struct Cyc_List_List* _temp2151; void* _temp2153; struct Cyc_List_List*
_temp2155; void* _temp2157; struct Cyc_List_List* _temp2159; void* _temp2161;
void* _temp2163; void* _temp2165; void* _temp2167; void* _temp2169; void*
_temp2171; void* _temp2173; void* _temp2175; void* _temp2177; void* _temp2179;
void* _temp2181; void* _temp2183; void* _temp2185; void* _temp2187; void*
_temp2189; void* _temp2191; _LL1863: _LL1922: _temp1921= _temp1861.f1; if((
unsigned int) _temp1921 >  3u?*(( int*) _temp1921) ==  Cyc_Absyn_Evar: 0){ goto
_LL1920;} else{ goto _LL1865;} _LL1920: _temp1919= _temp1861.f2; if((
unsigned int) _temp1919 >  3u?*(( int*) _temp1919) ==  Cyc_Absyn_Evar: 0){ goto
_LL1864;} else{ goto _LL1865;} _LL1865: _LL1926: _temp1925= _temp1861.f1; if(
_temp1925 == ( void*) Cyc_Absyn_VoidType){ goto _LL1924;} else{ goto _LL1867;}
_LL1924: _temp1923= _temp1861.f2; if( _temp1923 == ( void*) Cyc_Absyn_VoidType){
goto _LL1866;} else{ goto _LL1867;} _LL1867: _LL1932: _temp1931= _temp1861.f1;
if(( unsigned int) _temp1931 >  3u?*(( int*) _temp1931) ==  Cyc_Absyn_VarType: 0){
_LL1934: _temp1933=(( struct Cyc_Absyn_VarType_struct*) _temp1931)->f1; goto
_LL1928;} else{ goto _LL1869;} _LL1928: _temp1927= _temp1861.f2; if((
unsigned int) _temp1927 >  3u?*(( int*) _temp1927) ==  Cyc_Absyn_VarType: 0){
_LL1930: _temp1929=(( struct Cyc_Absyn_VarType_struct*) _temp1927)->f1; goto
_LL1868;} else{ goto _LL1869;} _LL1869: _LL1942: _temp1941= _temp1861.f1; if((
unsigned int) _temp1941 >  3u?*(( int*) _temp1941) ==  Cyc_Absyn_StructType: 0){
_LL1946: _temp1945=(( struct Cyc_Absyn_StructType_struct*) _temp1941)->f1; goto
_LL1944; _LL1944: _temp1943=(( struct Cyc_Absyn_StructType_struct*) _temp1941)->f2;
goto _LL1936;} else{ goto _LL1871;} _LL1936: _temp1935= _temp1861.f2; if((
unsigned int) _temp1935 >  3u?*(( int*) _temp1935) ==  Cyc_Absyn_StructType: 0){
_LL1940: _temp1939=(( struct Cyc_Absyn_StructType_struct*) _temp1935)->f1; goto
_LL1938; _LL1938: _temp1937=(( struct Cyc_Absyn_StructType_struct*) _temp1935)->f2;
goto _LL1870;} else{ goto _LL1871;} _LL1871: _LL1952: _temp1951= _temp1861.f1;
if(( unsigned int) _temp1951 >  3u?*(( int*) _temp1951) ==  Cyc_Absyn_EnumType:
0){ _LL1954: _temp1953=(( struct Cyc_Absyn_EnumType_struct*) _temp1951)->f1;
goto _LL1948;} else{ goto _LL1873;} _LL1948: _temp1947= _temp1861.f2; if((
unsigned int) _temp1947 >  3u?*(( int*) _temp1947) ==  Cyc_Absyn_EnumType: 0){
_LL1950: _temp1949=(( struct Cyc_Absyn_EnumType_struct*) _temp1947)->f1; goto
_LL1872;} else{ goto _LL1873;} _LL1873: _LL1960: _temp1959= _temp1861.f1; if((
unsigned int) _temp1959 >  3u?*(( int*) _temp1959) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1962: _temp1961=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1959)->f1;
goto _LL1956;} else{ goto _LL1875;} _LL1956: _temp1955= _temp1861.f2; if((
unsigned int) _temp1955 >  3u?*(( int*) _temp1955) ==  Cyc_Absyn_AnonEnumType: 0){
_LL1958: _temp1957=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp1955)->f1;
goto _LL1874;} else{ goto _LL1875;} _LL1875: _LL1972: _temp1971= _temp1861.f1;
if(( unsigned int) _temp1971 >  3u?*(( int*) _temp1971) ==  Cyc_Absyn_UnionType:
0){ _LL1978: _temp1977=(( struct Cyc_Absyn_UnionType_struct*) _temp1971)->f1;
goto _LL1976; _LL1976: _temp1975=(( struct Cyc_Absyn_UnionType_struct*)
_temp1971)->f2; goto _LL1974; _LL1974: _temp1973=(( struct Cyc_Absyn_UnionType_struct*)
_temp1971)->f3; goto _LL1964;} else{ goto _LL1877;} _LL1964: _temp1963=
_temp1861.f2; if(( unsigned int) _temp1963 >  3u?*(( int*) _temp1963) ==  Cyc_Absyn_UnionType:
0){ _LL1970: _temp1969=(( struct Cyc_Absyn_UnionType_struct*) _temp1963)->f1;
goto _LL1968; _LL1968: _temp1967=(( struct Cyc_Absyn_UnionType_struct*)
_temp1963)->f2; goto _LL1966; _LL1966: _temp1965=(( struct Cyc_Absyn_UnionType_struct*)
_temp1963)->f3; goto _LL1876;} else{ goto _LL1877;} _LL1877: _LL1993: _temp1992=
_temp1861.f1; if(( unsigned int) _temp1992 >  3u?*(( int*) _temp1992) ==  Cyc_Absyn_TunionType:
0){ _LL1995: _temp1994=(( struct Cyc_Absyn_TunionType_struct*) _temp1992)->f1;
_LL2001: _temp2000=( void*) _temp1994.tunion_info; if(*(( int*) _temp2000) == 
Cyc_Absyn_KnownTunion){ _LL2003: _temp2002=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2000)->f1; _temp2004=* _temp2002; goto _LL1999;} else{ goto _LL1879;}
_LL1999: _temp1998= _temp1994.targs; goto _LL1997; _LL1997: _temp1996=( void*)
_temp1994.rgn; goto _LL1980;} else{ goto _LL1879;} _LL1980: _temp1979= _temp1861.f2;
if(( unsigned int) _temp1979 >  3u?*(( int*) _temp1979) ==  Cyc_Absyn_TunionType:
0){ _LL1982: _temp1981=(( struct Cyc_Absyn_TunionType_struct*) _temp1979)->f1;
_LL1988: _temp1987=( void*) _temp1981.tunion_info; if(*(( int*) _temp1987) == 
Cyc_Absyn_KnownTunion){ _LL1990: _temp1989=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1987)->f1; _temp1991=* _temp1989; goto _LL1986;} else{ goto _LL1879;}
_LL1986: _temp1985= _temp1981.targs; goto _LL1984; _LL1984: _temp1983=( void*)
_temp1981.rgn; goto _LL1878;} else{ goto _LL1879;} _LL1879: _LL2018: _temp2017=
_temp1861.f1; if(( unsigned int) _temp2017 >  3u?*(( int*) _temp2017) ==  Cyc_Absyn_TunionFieldType:
0){ _LL2020: _temp2019=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2017)->f1;
_LL2024: _temp2023=( void*) _temp2019.field_info; if(*(( int*) _temp2023) == 
Cyc_Absyn_KnownTunionfield){ _LL2028: _temp2027=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2023)->f1; goto _LL2026; _LL2026: _temp2025=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2023)->f2; goto _LL2022;} else{ goto _LL1881;} _LL2022: _temp2021=
_temp2019.targs; goto _LL2006;} else{ goto _LL1881;} _LL2006: _temp2005=
_temp1861.f2; if(( unsigned int) _temp2005 >  3u?*(( int*) _temp2005) ==  Cyc_Absyn_TunionFieldType:
0){ _LL2008: _temp2007=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2005)->f1;
_LL2012: _temp2011=( void*) _temp2007.field_info; if(*(( int*) _temp2011) == 
Cyc_Absyn_KnownTunionfield){ _LL2016: _temp2015=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2011)->f1; goto _LL2014; _LL2014: _temp2013=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2011)->f2; goto _LL2010;} else{ goto _LL1881;} _LL2010: _temp2009=
_temp2007.targs; goto _LL1880;} else{ goto _LL1881;} _LL1881: _LL2044: _temp2043=
_temp1861.f1; if(( unsigned int) _temp2043 >  3u?*(( int*) _temp2043) ==  Cyc_Absyn_PointerType:
0){ _LL2046: _temp2045=(( struct Cyc_Absyn_PointerType_struct*) _temp2043)->f1;
_LL2056: _temp2055=( void*) _temp2045.elt_typ; goto _LL2054; _LL2054: _temp2053=(
void*) _temp2045.rgn_typ; goto _LL2052; _LL2052: _temp2051= _temp2045.nullable;
goto _LL2050; _LL2050: _temp2049= _temp2045.tq; goto _LL2048; _LL2048: _temp2047=
_temp2045.bounds; goto _LL2030;} else{ goto _LL1883;} _LL2030: _temp2029=
_temp1861.f2; if(( unsigned int) _temp2029 >  3u?*(( int*) _temp2029) ==  Cyc_Absyn_PointerType:
0){ _LL2032: _temp2031=(( struct Cyc_Absyn_PointerType_struct*) _temp2029)->f1;
_LL2042: _temp2041=( void*) _temp2031.elt_typ; goto _LL2040; _LL2040: _temp2039=(
void*) _temp2031.rgn_typ; goto _LL2038; _LL2038: _temp2037= _temp2031.nullable;
goto _LL2036; _LL2036: _temp2035= _temp2031.tq; goto _LL2034; _LL2034: _temp2033=
_temp2031.bounds; goto _LL1882;} else{ goto _LL1883;} _LL1883: _LL2064:
_temp2063= _temp1861.f1; if(( unsigned int) _temp2063 >  3u?*(( int*) _temp2063)
==  Cyc_Absyn_IntType: 0){ _LL2068: _temp2067=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2063)->f1; goto _LL2066; _LL2066: _temp2065=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2063)->f2; goto _LL2058;} else{ goto _LL1885;} _LL2058: _temp2057=
_temp1861.f2; if(( unsigned int) _temp2057 >  3u?*(( int*) _temp2057) ==  Cyc_Absyn_IntType:
0){ _LL2062: _temp2061=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2057)->f1;
goto _LL2060; _LL2060: _temp2059=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2057)->f2; goto _LL1884;} else{ goto _LL1885;} _LL1885: _LL2072: _temp2071=
_temp1861.f1; if( _temp2071 == ( void*) Cyc_Absyn_FloatType){ goto _LL2070;}
else{ goto _LL1887;} _LL2070: _temp2069= _temp1861.f2; if( _temp2069 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1886;} else{ goto _LL1887;} _LL1887: _LL2078:
_temp2077= _temp1861.f1; if(( unsigned int) _temp2077 >  3u?*(( int*) _temp2077)
==  Cyc_Absyn_DoubleType: 0){ _LL2080: _temp2079=(( struct Cyc_Absyn_DoubleType_struct*)
_temp2077)->f1; goto _LL2074;} else{ goto _LL1889;} _LL2074: _temp2073=
_temp1861.f2; if(( unsigned int) _temp2073 >  3u?*(( int*) _temp2073) ==  Cyc_Absyn_DoubleType:
0){ _LL2076: _temp2075=(( struct Cyc_Absyn_DoubleType_struct*) _temp2073)->f1;
goto _LL1888;} else{ goto _LL1889;} _LL1889: _LL2090: _temp2089= _temp1861.f1;
if(( unsigned int) _temp2089 >  3u?*(( int*) _temp2089) ==  Cyc_Absyn_ArrayType:
0){ _LL2096: _temp2095=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2089)->f1;
goto _LL2094; _LL2094: _temp2093=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2089)->f2; goto _LL2092; _LL2092: _temp2091=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2089)->f3; goto _LL2082;} else{ goto _LL1891;} _LL2082: _temp2081=
_temp1861.f2; if(( unsigned int) _temp2081 >  3u?*(( int*) _temp2081) ==  Cyc_Absyn_ArrayType:
0){ _LL2088: _temp2087=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2081)->f1;
goto _LL2086; _LL2086: _temp2085=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2081)->f2; goto _LL2084; _LL2084: _temp2083=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2081)->f3; goto _LL1890;} else{ goto _LL1891;} _LL1891: _LL2118: _temp2117=
_temp1861.f1; if(( unsigned int) _temp2117 >  3u?*(( int*) _temp2117) ==  Cyc_Absyn_FnType:
0){ _LL2120: _temp2119=(( struct Cyc_Absyn_FnType_struct*) _temp2117)->f1;
_LL2136: _temp2135= _temp2119.tvars; goto _LL2134; _LL2134: _temp2133= _temp2119.effect;
goto _LL2132; _LL2132: _temp2131=( void*) _temp2119.ret_typ; goto _LL2130;
_LL2130: _temp2129= _temp2119.args; goto _LL2128; _LL2128: _temp2127= _temp2119.c_varargs;
goto _LL2126; _LL2126: _temp2125= _temp2119.cyc_varargs; goto _LL2124; _LL2124:
_temp2123= _temp2119.rgn_po; goto _LL2122; _LL2122: _temp2121= _temp2119.attributes;
goto _LL2098;} else{ goto _LL1893;} _LL2098: _temp2097= _temp1861.f2; if((
unsigned int) _temp2097 >  3u?*(( int*) _temp2097) ==  Cyc_Absyn_FnType: 0){
_LL2100: _temp2099=(( struct Cyc_Absyn_FnType_struct*) _temp2097)->f1; _LL2116:
_temp2115= _temp2099.tvars; goto _LL2114; _LL2114: _temp2113= _temp2099.effect;
goto _LL2112; _LL2112: _temp2111=( void*) _temp2099.ret_typ; goto _LL2110;
_LL2110: _temp2109= _temp2099.args; goto _LL2108; _LL2108: _temp2107= _temp2099.c_varargs;
goto _LL2106; _LL2106: _temp2105= _temp2099.cyc_varargs; goto _LL2104; _LL2104:
_temp2103= _temp2099.rgn_po; goto _LL2102; _LL2102: _temp2101= _temp2099.attributes;
goto _LL1892;} else{ goto _LL1893;} _LL1893: _LL2142: _temp2141= _temp1861.f1;
if(( unsigned int) _temp2141 >  3u?*(( int*) _temp2141) ==  Cyc_Absyn_TupleType:
0){ _LL2144: _temp2143=(( struct Cyc_Absyn_TupleType_struct*) _temp2141)->f1;
goto _LL2138;} else{ goto _LL1895;} _LL2138: _temp2137= _temp1861.f2; if((
unsigned int) _temp2137 >  3u?*(( int*) _temp2137) ==  Cyc_Absyn_TupleType: 0){
_LL2140: _temp2139=(( struct Cyc_Absyn_TupleType_struct*) _temp2137)->f1; goto
_LL1894;} else{ goto _LL1895;} _LL1895: _LL2150: _temp2149= _temp1861.f1; if((
unsigned int) _temp2149 >  3u?*(( int*) _temp2149) ==  Cyc_Absyn_AnonStructType:
0){ _LL2152: _temp2151=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2149)->f1;
goto _LL2146;} else{ goto _LL1897;} _LL2146: _temp2145= _temp1861.f2; if((
unsigned int) _temp2145 >  3u?*(( int*) _temp2145) ==  Cyc_Absyn_AnonStructType:
0){ _LL2148: _temp2147=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2145)->f1;
goto _LL1896;} else{ goto _LL1897;} _LL1897: _LL2158: _temp2157= _temp1861.f1;
if(( unsigned int) _temp2157 >  3u?*(( int*) _temp2157) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2160: _temp2159=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2157)->f1;
goto _LL2154;} else{ goto _LL1899;} _LL2154: _temp2153= _temp1861.f2; if((
unsigned int) _temp2153 >  3u?*(( int*) _temp2153) ==  Cyc_Absyn_AnonUnionType:
0){ _LL2156: _temp2155=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2153)->f1;
goto _LL1898;} else{ goto _LL1899;} _LL1899: _LL2164: _temp2163= _temp1861.f1;
if( _temp2163 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL2162;} else{ goto _LL1901;}
_LL2162: _temp2161= _temp1861.f2; if( _temp2161 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1900;} else{ goto _LL1901;} _LL1901: _LL2170: _temp2169= _temp1861.f1;
if(( unsigned int) _temp2169 >  3u?*(( int*) _temp2169) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2172: _temp2171=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2169)->f1; goto _LL2166;} else{ goto _LL1903;} _LL2166: _temp2165=
_temp1861.f2; if(( unsigned int) _temp2165 >  3u?*(( int*) _temp2165) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2168: _temp2167=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2165)->f1; goto _LL1902;} else{ goto _LL1903;} _LL1903: _LL2178: _temp2177=
_temp1861.f1; if(( unsigned int) _temp2177 >  3u?*(( int*) _temp2177) ==  Cyc_Absyn_SizeofType:
0){ _LL2180: _temp2179=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp2177)->f1;
goto _LL2174;} else{ goto _LL1905;} _LL2174: _temp2173= _temp1861.f2; if((
unsigned int) _temp2173 >  3u?*(( int*) _temp2173) ==  Cyc_Absyn_SizeofType: 0){
_LL2176: _temp2175=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp2173)->f1;
goto _LL1904;} else{ goto _LL1905;} _LL1905: _LL2182: _temp2181= _temp1861.f1;
if(( unsigned int) _temp2181 >  3u?*(( int*) _temp2181) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1906;} else{ goto _LL1907;} _LL1907: _LL2184: _temp2183= _temp1861.f2;
if(( unsigned int) _temp2183 >  3u?*(( int*) _temp2183) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1908;} else{ goto _LL1909;} _LL1909: _LL2186: _temp2185= _temp1861.f1;
if(( unsigned int) _temp2185 >  3u?*(( int*) _temp2185) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1910;} else{ goto _LL1911;} _LL1911: _LL2188: _temp2187= _temp1861.f1;
if(( unsigned int) _temp2187 >  3u?*(( int*) _temp2187) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1912;} else{ goto _LL1913;} _LL1913: _LL2190: _temp2189= _temp1861.f2;
if(( unsigned int) _temp2189 >  3u?*(( int*) _temp2189) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1914;} else{ goto _LL1915;} _LL1915: _LL2192: _temp2191= _temp1861.f2;
if(( unsigned int) _temp2191 >  3u?*(( int*) _temp2191) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1916;} else{ goto _LL1917;} _LL1917: goto _LL1918; _LL1864:({ void*
_temp2193[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types", sizeof( unsigned char), 39u),
_tag_arr( _temp2193, sizeof( void*), 0u));}); return 0; _LL1866: return 0;
_LL1868: { struct _tagged_arr* _temp2194= _temp1929->name; struct _tagged_arr*
_temp2195= _temp1933->name; int _temp2196=*(( int*) _check_null( _temp1929->identity));
int _temp2197=*(( int*) _check_null( _temp1933->identity)); return Cyc_Core_intcmp(
_temp2196, _temp2197);} _LL1870: { int _temp2198=(( int(*)( int(* cmp)( struct
_tuple1*, struct _tuple1*), struct _tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)(
Cyc_Absyn_qvar_cmp, _temp1939, _temp1945); if( _temp2198 !=  0){ return
_temp2198;} else{ return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp1937,
_temp1943);}} _LL1872: { int _temp2199= Cyc_Absyn_qvar_cmp( _temp1953, _temp1949);
return _temp2199;} _LL1874: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l1, struct Cyc_List_List* l2))
Cyc_Tcutil_list_cmp)( Cyc_Tcutil_enumfield_cmp, _temp1961, _temp1957); _LL1876: {
int _temp2200=(( int(*)( int(* cmp)( struct _tuple1*, struct _tuple1*), struct
_tuple1* a1, struct _tuple1* a2)) Cyc_Tcutil_star_cmp)( Cyc_Absyn_qvar_cmp,
_temp1969, _temp1977); if( _temp2200 !=  0){ return _temp2200;} else{ return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1967, _temp1975);}} _LL1878: if( _temp1991 == 
_temp2004){ return 0;}{ int _temp2201= Cyc_Absyn_qvar_cmp( _temp1991->name,
_temp2004->name); if( _temp2201 !=  0){ return _temp2201;}{ int _temp2202= Cyc_Tcutil_typecmp(
_temp1983, _temp1996); if( _temp2202 !=  0){ return _temp2202;} return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp, _temp1985, _temp1998);}} _LL1880: if( _temp2015 == 
_temp2027){ return 0;}{ int _temp2203= Cyc_Absyn_qvar_cmp( _temp2027->name,
_temp2015->name); if( _temp2203 !=  0){ return _temp2203;}{ int _temp2204= Cyc_Absyn_qvar_cmp(
_temp2025->name, _temp2013->name); if( _temp2204 !=  0){ return _temp2204;}
return Cyc_Tcutil_list_cmp( Cyc_Tcutil_typecmp, _temp2009, _temp2021);}} _LL1882: {
int _temp2205= Cyc_Tcutil_typecmp( _temp2041, _temp2055); if( _temp2205 !=  0){
return _temp2205;}{ int _temp2206= Cyc_Tcutil_typecmp( _temp2039, _temp2053);
if( _temp2206 !=  0){ return _temp2206;}{ int _temp2207= Cyc_Tcutil_tqual_cmp(
_temp2035, _temp2049); if( _temp2207 !=  0){ return _temp2207;}{ int _temp2208=
Cyc_Tcutil_conrefs_cmp( Cyc_Tcutil_boundscmp, _temp2033, _temp2047); if(
_temp2208 !=  0){ return _temp2208;}{ void* _temp2209=( void*)( Cyc_Absyn_compress_conref(
_temp2033))->v; void* _temp2215; _LL2211: if(( unsigned int) _temp2209 >  1u?*((
int*) _temp2209) ==  Cyc_Absyn_Eq_constr: 0){ _LL2216: _temp2215=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2209)->f1; if( _temp2215 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2212;} else{ goto _LL2213;}} else{ goto _LL2213;}
_LL2213: goto _LL2214; _LL2212: return 0; _LL2214: goto _LL2210; _LL2210:;}
return(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_conrefs_cmp)( Cyc_Core_intcmp, _temp2037, _temp2051);}}}} _LL1884:
if( _temp2061 !=  _temp2067){ return Cyc_Core_intcmp(( int)(( unsigned int)
_temp2061),( int)(( unsigned int) _temp2067));} if( _temp2059 !=  _temp2065){
return Cyc_Core_intcmp(( int)(( unsigned int) _temp2059),( int)(( unsigned int)
_temp2065));} return 0; _LL1886: return 0; _LL1888: if( _temp2079 ==  _temp2075){
return 0;} else{ if( _temp2079){ return - 1;} else{ return 1;}} _LL1890: { int
_temp2217= Cyc_Tcutil_tqual_cmp( _temp2085, _temp2093); if( _temp2217 !=  0){
return _temp2217;}{ int _temp2218= Cyc_Tcutil_typecmp( _temp2087, _temp2095);
if( _temp2218 !=  0){ return _temp2218;} if( _temp2091 ==  _temp2083){ return 0;}
if( _temp2091 ==  0? 1: _temp2083 ==  0){({ void* _temp2219[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("missing expression in array index",
sizeof( unsigned char), 34u), _tag_arr( _temp2219, sizeof( void*), 0u));});}
return(( int(*)( int(* cmp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp* a1, struct Cyc_Absyn_Exp* a2)) Cyc_Tcutil_star_cmp)( Cyc_Tcutil_const_uint_exp_cmp,
_temp2091, _temp2083);}} _LL1892:({ void* _temp2220[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: function types not handled",
sizeof( unsigned char), 36u), _tag_arr( _temp2220, sizeof( void*), 0u));}); goto
_LL1862; _LL1894: return(( int(*)( int(* cmp)( struct _tuple4*, struct _tuple4*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_tqual_type_cmp,
_temp2139, _temp2143); _LL1896: _temp2159= _temp2151; _temp2155= _temp2147; goto
_LL1898; _LL1898: return(( int(*)( int(* cmp)( struct Cyc_Absyn_Structfield*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_Tcutil_list_cmp)( Cyc_Tcutil_structfield_cmp, _temp2155, _temp2159);
_LL1900: return 0; _LL1902: return Cyc_Tcutil_typecmp( _temp2171, _temp2167);
_LL1904: return Cyc_Tcutil_typecmp( _temp2179, _temp2175); _LL1906: goto _LL1908;
_LL1908: goto _LL1910; _LL1910: goto _LL1912; _LL1912: goto _LL1914; _LL1914:
goto _LL1916; _LL1916:({ void* _temp2221[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("typecmp: effects not handled",
sizeof( unsigned char), 29u), _tag_arr( _temp2221, sizeof( void*), 0u));}); goto
_LL1862; _LL1918: goto _LL1862; _LL1862:;} return({ void* _temp2222[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("Unmatched case in typecmp", sizeof( unsigned char), 26u), _tag_arr(
_temp2222, sizeof( void*), 0u));});}} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp2223= Cyc_Tcutil_compress( t); _LL2225: if(( unsigned int) _temp2223
>  3u?*(( int*) _temp2223) ==  Cyc_Absyn_IntType: 0){ goto _LL2226;} else{ goto
_LL2227;} _LL2227: if( _temp2223 == ( void*) Cyc_Absyn_FloatType){ goto _LL2228;}
else{ goto _LL2229;} _LL2229: if(( unsigned int) _temp2223 >  3u?*(( int*)
_temp2223) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2230;} else{ goto _LL2231;}
_LL2231: if(( unsigned int) _temp2223 >  3u?*(( int*) _temp2223) ==  Cyc_Absyn_EnumType:
0){ goto _LL2232;} else{ goto _LL2233;} _LL2233: if(( unsigned int) _temp2223 > 
3u?*(( int*) _temp2223) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL2234;} else{
goto _LL2235;} _LL2235: goto _LL2236; _LL2226: goto _LL2228; _LL2228: goto
_LL2230; _LL2230: goto _LL2232; _LL2232: goto _LL2234; _LL2234: return 1;
_LL2236: return 0; _LL2224:;} int Cyc_Tcutil_will_lose_precision( void* t1, void*
t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp2238=({ struct _tuple8 _temp2237; _temp2237.f1= t1; _temp2237.f2= t2;
_temp2237;}); void* _temp2270; int _temp2272; void* _temp2274; int _temp2276;
void* _temp2278; void* _temp2280; void* _temp2282; void* _temp2284; void*
_temp2286; void* _temp2288; void* _temp2290; void* _temp2292; void* _temp2294;
void* _temp2296; void* _temp2298; void* _temp2300; void* _temp2302; void*
_temp2304; void* _temp2306; void* _temp2308; void* _temp2310; void* _temp2312;
void* _temp2314; void* _temp2316; void* _temp2318; void* _temp2320; void*
_temp2322; void* _temp2324; void* _temp2326; void* _temp2328; void* _temp2330;
void* _temp2332; void* _temp2334; void* _temp2336; void* _temp2338; void*
_temp2340; void* _temp2342; void* _temp2344; void* _temp2346; void* _temp2348;
void* _temp2350; _LL2240: _LL2275: _temp2274= _temp2238.f1; if(( unsigned int)
_temp2274 >  3u?*(( int*) _temp2274) ==  Cyc_Absyn_DoubleType: 0){ _LL2277:
_temp2276=(( struct Cyc_Absyn_DoubleType_struct*) _temp2274)->f1; goto _LL2271;}
else{ goto _LL2242;} _LL2271: _temp2270= _temp2238.f2; if(( unsigned int)
_temp2270 >  3u?*(( int*) _temp2270) ==  Cyc_Absyn_DoubleType: 0){ _LL2273:
_temp2272=(( struct Cyc_Absyn_DoubleType_struct*) _temp2270)->f1; goto _LL2241;}
else{ goto _LL2242;} _LL2242: _LL2281: _temp2280= _temp2238.f1; if((
unsigned int) _temp2280 >  3u?*(( int*) _temp2280) ==  Cyc_Absyn_DoubleType: 0){
goto _LL2279;} else{ goto _LL2244;} _LL2279: _temp2278= _temp2238.f2; if(
_temp2278 == ( void*) Cyc_Absyn_FloatType){ goto _LL2243;} else{ goto _LL2244;}
_LL2244: _LL2285: _temp2284= _temp2238.f1; if(( unsigned int) _temp2284 >  3u?*((
int*) _temp2284) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2283;} else{ goto _LL2246;}
_LL2283: _temp2282= _temp2238.f2; if(( unsigned int) _temp2282 >  3u?*(( int*)
_temp2282) ==  Cyc_Absyn_IntType: 0){ goto _LL2245;} else{ goto _LL2246;}
_LL2246: _LL2289: _temp2288= _temp2238.f1; if(( unsigned int) _temp2288 >  3u?*((
int*) _temp2288) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2287;} else{ goto _LL2248;}
_LL2287: _temp2286= _temp2238.f2; if(( unsigned int) _temp2286 >  3u?*(( int*)
_temp2286) ==  Cyc_Absyn_SizeofType: 0){ goto _LL2247;} else{ goto _LL2248;}
_LL2248: _LL2293: _temp2292= _temp2238.f1; if( _temp2292 == ( void*) Cyc_Absyn_FloatType){
goto _LL2291;} else{ goto _LL2250;} _LL2291: _temp2290= _temp2238.f2; if((
unsigned int) _temp2290 >  3u?*(( int*) _temp2290) ==  Cyc_Absyn_SizeofType: 0){
goto _LL2249;} else{ goto _LL2250;} _LL2250: _LL2297: _temp2296= _temp2238.f1;
if( _temp2296 == ( void*) Cyc_Absyn_FloatType){ goto _LL2295;} else{ goto
_LL2252;} _LL2295: _temp2294= _temp2238.f2; if(( unsigned int) _temp2294 >  3u?*((
int*) _temp2294) ==  Cyc_Absyn_IntType: 0){ goto _LL2251;} else{ goto _LL2252;}
_LL2252: _LL2303: _temp2302= _temp2238.f1; if(( unsigned int) _temp2302 >  3u?*((
int*) _temp2302) ==  Cyc_Absyn_IntType: 0){ _LL2305: _temp2304=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2302)->f2; if( _temp2304 == ( void*) Cyc_Absyn_B8){
goto _LL2299;} else{ goto _LL2254;}} else{ goto _LL2254;} _LL2299: _temp2298=
_temp2238.f2; if(( unsigned int) _temp2298 >  3u?*(( int*) _temp2298) ==  Cyc_Absyn_IntType:
0){ _LL2301: _temp2300=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2298)->f2;
if( _temp2300 == ( void*) Cyc_Absyn_B8){ goto _LL2253;} else{ goto _LL2254;}}
else{ goto _LL2254;} _LL2254: _LL2307: _temp2306= _temp2238.f1; if((
unsigned int) _temp2306 >  3u?*(( int*) _temp2306) ==  Cyc_Absyn_IntType: 0){
_LL2309: _temp2308=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2306)->f2;
if( _temp2308 == ( void*) Cyc_Absyn_B8){ goto _LL2255;} else{ goto _LL2256;}}
else{ goto _LL2256;} _LL2256: _LL2313: _temp2312= _temp2238.f1; if((
unsigned int) _temp2312 >  3u?*(( int*) _temp2312) ==  Cyc_Absyn_IntType: 0){
_LL2315: _temp2314=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2312)->f2;
if( _temp2314 == ( void*) Cyc_Absyn_B4){ goto _LL2311;} else{ goto _LL2258;}}
else{ goto _LL2258;} _LL2311: _temp2310= _temp2238.f2; if( _temp2310 == ( void*)
Cyc_Absyn_FloatType){ goto _LL2257;} else{ goto _LL2258;} _LL2258: _LL2321:
_temp2320= _temp2238.f1; if(( unsigned int) _temp2320 >  3u?*(( int*) _temp2320)
==  Cyc_Absyn_IntType: 0){ _LL2323: _temp2322=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2320)->f2; if( _temp2322 == ( void*) Cyc_Absyn_B4){ goto _LL2317;} else{
goto _LL2260;}} else{ goto _LL2260;} _LL2317: _temp2316= _temp2238.f2; if((
unsigned int) _temp2316 >  3u?*(( int*) _temp2316) ==  Cyc_Absyn_IntType: 0){
_LL2319: _temp2318=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2316)->f2;
if( _temp2318 == ( void*) Cyc_Absyn_B2){ goto _LL2259;} else{ goto _LL2260;}}
else{ goto _LL2260;} _LL2260: _LL2329: _temp2328= _temp2238.f1; if((
unsigned int) _temp2328 >  3u?*(( int*) _temp2328) ==  Cyc_Absyn_IntType: 0){
_LL2331: _temp2330=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2328)->f2;
if( _temp2330 == ( void*) Cyc_Absyn_B4){ goto _LL2325;} else{ goto _LL2262;}}
else{ goto _LL2262;} _LL2325: _temp2324= _temp2238.f2; if(( unsigned int)
_temp2324 >  3u?*(( int*) _temp2324) ==  Cyc_Absyn_IntType: 0){ _LL2327:
_temp2326=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2324)->f2; if(
_temp2326 == ( void*) Cyc_Absyn_B1){ goto _LL2261;} else{ goto _LL2262;}} else{
goto _LL2262;} _LL2262: _LL2337: _temp2336= _temp2238.f1; if(( unsigned int)
_temp2336 >  3u?*(( int*) _temp2336) ==  Cyc_Absyn_IntType: 0){ _LL2339:
_temp2338=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2336)->f2; if(
_temp2338 == ( void*) Cyc_Absyn_B2){ goto _LL2333;} else{ goto _LL2264;}} else{
goto _LL2264;} _LL2333: _temp2332= _temp2238.f2; if(( unsigned int) _temp2332 > 
3u?*(( int*) _temp2332) ==  Cyc_Absyn_IntType: 0){ _LL2335: _temp2334=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp2332)->f2; if( _temp2334 == ( void*) Cyc_Absyn_B1){
goto _LL2263;} else{ goto _LL2264;}} else{ goto _LL2264;} _LL2264: _LL2345:
_temp2344= _temp2238.f1; if(( unsigned int) _temp2344 >  3u?*(( int*) _temp2344)
==  Cyc_Absyn_SizeofType: 0){ goto _LL2341;} else{ goto _LL2266;} _LL2341:
_temp2340= _temp2238.f2; if(( unsigned int) _temp2340 >  3u?*(( int*) _temp2340)
==  Cyc_Absyn_IntType: 0){ _LL2343: _temp2342=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2340)->f2; if( _temp2342 == ( void*) Cyc_Absyn_B2){ goto _LL2265;} else{
goto _LL2266;}} else{ goto _LL2266;} _LL2266: _LL2351: _temp2350= _temp2238.f1;
if(( unsigned int) _temp2350 >  3u?*(( int*) _temp2350) ==  Cyc_Absyn_SizeofType:
0){ goto _LL2347;} else{ goto _LL2268;} _LL2347: _temp2346= _temp2238.f2; if((
unsigned int) _temp2346 >  3u?*(( int*) _temp2346) ==  Cyc_Absyn_IntType: 0){
_LL2349: _temp2348=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2346)->f2;
if( _temp2348 == ( void*) Cyc_Absyn_B1){ goto _LL2267;} else{ goto _LL2268;}}
else{ goto _LL2268;} _LL2268: goto _LL2269; _LL2241: return ! _temp2272?
_temp2276: 0; _LL2243: goto _LL2245; _LL2245: goto _LL2247; _LL2247: goto
_LL2249; _LL2249: goto _LL2251; _LL2251: return 1; _LL2253: return 0; _LL2255:
goto _LL2257; _LL2257: goto _LL2259; _LL2259: goto _LL2261; _LL2261: goto
_LL2263; _LL2263: goto _LL2265; _LL2265: goto _LL2267; _LL2267: return 1;
_LL2269: return 0; _LL2239:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) el->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*) max_arith_type->v)){ max_arith_type=( struct Cyc_Core_Opt*)({ struct
Cyc_Core_Opt* _temp2352=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2352->v=( void*) t1; _temp2352;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify(
t,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el= el->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*) el->hd, t)){({ struct Cyc_Std_String_pa_struct
_temp2355; _temp2355.tag= Cyc_Std_String_pa; _temp2355.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*) el->hd)->topt))->v);{ struct Cyc_Std_String_pa_struct _temp2354;
_temp2354.tag= Cyc_Std_String_pa; _temp2354.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2353[ 2u]={& _temp2354,& _temp2355}; Cyc_Tcutil_terr((( struct
Cyc_Absyn_Exp*) el->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp2353, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp2356= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL2358: if(( unsigned int) _temp2356 >  3u?*(( int*) _temp2356) ==  Cyc_Absyn_PointerType:
0){ goto _LL2359;} else{ goto _LL2360;} _LL2360: goto _LL2361; _LL2359: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL2357; _LL2361: return 0; _LL2357:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp2362= Cyc_Tcutil_compress(
t); _LL2364: if(( unsigned int) _temp2362 >  3u?*(( int*) _temp2362) ==  Cyc_Absyn_IntType:
0){ goto _LL2365;} else{ goto _LL2366;} _LL2366: if(( unsigned int) _temp2362 > 
3u?*(( int*) _temp2362) ==  Cyc_Absyn_SizeofType: 0){ goto _LL2367;} else{ goto
_LL2368;} _LL2368: if(( unsigned int) _temp2362 >  3u?*(( int*) _temp2362) == 
Cyc_Absyn_EnumType: 0){ goto _LL2369;} else{ goto _LL2370;} _LL2370: if((
unsigned int) _temp2362 >  3u?*(( int*) _temp2362) ==  Cyc_Absyn_AnonEnumType: 0){
goto _LL2371;} else{ goto _LL2372;} _LL2372: goto _LL2373; _LL2365: goto _LL2367;
_LL2367: goto _LL2369; _LL2369: goto _LL2371; _LL2371: return 1; _LL2373: return
0; _LL2363:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp2374[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2374, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp2375[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp2375, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp2377=({ struct _tuple8 _temp2376; _temp2376.f1= t1; _temp2376.f2= t2;
_temp2376;}); void* _temp2391; struct Cyc_Absyn_PtrInfo _temp2393; void*
_temp2395; struct Cyc_Absyn_PtrInfo _temp2397; void* _temp2399; struct Cyc_Absyn_Exp*
_temp2401; struct Cyc_Absyn_Tqual _temp2403; void* _temp2405; void* _temp2407;
struct Cyc_Absyn_Exp* _temp2409; struct Cyc_Absyn_Tqual _temp2411; void*
_temp2413; void* _temp2415; struct Cyc_Absyn_TunionInfo _temp2417; struct Cyc_List_List*
_temp2419; void* _temp2421; struct Cyc_Absyn_Tuniondecl** _temp2423; struct Cyc_Absyn_Tuniondecl*
_temp2425; void* _temp2426; struct Cyc_Absyn_TunionFieldInfo _temp2428; struct
Cyc_List_List* _temp2430; void* _temp2432; struct Cyc_Absyn_Tunionfield*
_temp2434; struct Cyc_Absyn_Tuniondecl* _temp2436; void* _temp2438; struct Cyc_Absyn_TunionInfo
_temp2440; void* _temp2442; struct Cyc_List_List* _temp2444; void* _temp2446;
struct Cyc_Absyn_Tuniondecl** _temp2448; struct Cyc_Absyn_Tuniondecl* _temp2450;
void* _temp2451; struct Cyc_Absyn_PtrInfo _temp2453; struct Cyc_Absyn_Conref*
_temp2455; struct Cyc_Absyn_Tqual _temp2457; struct Cyc_Absyn_Conref* _temp2459;
void* _temp2461; void* _temp2463; void* _temp2465; void* _temp2467; void*
_temp2469; _LL2379: _LL2396: _temp2395= _temp2377.f1; if(( unsigned int)
_temp2395 >  3u?*(( int*) _temp2395) ==  Cyc_Absyn_PointerType: 0){ _LL2398:
_temp2397=(( struct Cyc_Absyn_PointerType_struct*) _temp2395)->f1; goto _LL2392;}
else{ goto _LL2381;} _LL2392: _temp2391= _temp2377.f2; if(( unsigned int)
_temp2391 >  3u?*(( int*) _temp2391) ==  Cyc_Absyn_PointerType: 0){ _LL2394:
_temp2393=(( struct Cyc_Absyn_PointerType_struct*) _temp2391)->f1; goto _LL2380;}
else{ goto _LL2381;} _LL2381: _LL2408: _temp2407= _temp2377.f1; if((
unsigned int) _temp2407 >  3u?*(( int*) _temp2407) ==  Cyc_Absyn_ArrayType: 0){
_LL2414: _temp2413=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2407)->f1;
goto _LL2412; _LL2412: _temp2411=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2407)->f2; goto _LL2410; _LL2410: _temp2409=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2407)->f3; goto _LL2400;} else{ goto _LL2383;} _LL2400: _temp2399=
_temp2377.f2; if(( unsigned int) _temp2399 >  3u?*(( int*) _temp2399) ==  Cyc_Absyn_ArrayType:
0){ _LL2406: _temp2405=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2399)->f1;
goto _LL2404; _LL2404: _temp2403=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2399)->f2; goto _LL2402; _LL2402: _temp2401=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2399)->f3; goto _LL2382;} else{ goto _LL2383;} _LL2383: _LL2427: _temp2426=
_temp2377.f1; if(( unsigned int) _temp2426 >  3u?*(( int*) _temp2426) ==  Cyc_Absyn_TunionFieldType:
0){ _LL2429: _temp2428=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2426)->f1;
_LL2433: _temp2432=( void*) _temp2428.field_info; if(*(( int*) _temp2432) == 
Cyc_Absyn_KnownTunionfield){ _LL2437: _temp2436=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2432)->f1; goto _LL2435; _LL2435: _temp2434=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2432)->f2; goto _LL2431;} else{ goto _LL2385;} _LL2431: _temp2430=
_temp2428.targs; goto _LL2416;} else{ goto _LL2385;} _LL2416: _temp2415=
_temp2377.f2; if(( unsigned int) _temp2415 >  3u?*(( int*) _temp2415) ==  Cyc_Absyn_TunionType:
0){ _LL2418: _temp2417=(( struct Cyc_Absyn_TunionType_struct*) _temp2415)->f1;
_LL2422: _temp2421=( void*) _temp2417.tunion_info; if(*(( int*) _temp2421) == 
Cyc_Absyn_KnownTunion){ _LL2424: _temp2423=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2421)->f1; _temp2425=* _temp2423; goto _LL2420;} else{ goto _LL2385;}
_LL2420: _temp2419= _temp2417.targs; goto _LL2384;} else{ goto _LL2385;} _LL2385:
_LL2452: _temp2451= _temp2377.f1; if(( unsigned int) _temp2451 >  3u?*(( int*)
_temp2451) ==  Cyc_Absyn_PointerType: 0){ _LL2454: _temp2453=(( struct Cyc_Absyn_PointerType_struct*)
_temp2451)->f1; _LL2464: _temp2463=( void*) _temp2453.elt_typ; goto _LL2462;
_LL2462: _temp2461=( void*) _temp2453.rgn_typ; goto _LL2460; _LL2460: _temp2459=
_temp2453.nullable; goto _LL2458; _LL2458: _temp2457= _temp2453.tq; goto _LL2456;
_LL2456: _temp2455= _temp2453.bounds; goto _LL2439;} else{ goto _LL2387;}
_LL2439: _temp2438= _temp2377.f2; if(( unsigned int) _temp2438 >  3u?*(( int*)
_temp2438) ==  Cyc_Absyn_TunionType: 0){ _LL2441: _temp2440=(( struct Cyc_Absyn_TunionType_struct*)
_temp2438)->f1; _LL2447: _temp2446=( void*) _temp2440.tunion_info; if(*(( int*)
_temp2446) ==  Cyc_Absyn_KnownTunion){ _LL2449: _temp2448=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2446)->f1; _temp2450=* _temp2448; goto _LL2445;} else{ goto _LL2387;}
_LL2445: _temp2444= _temp2440.targs; goto _LL2443; _LL2443: _temp2442=( void*)
_temp2440.rgn; goto _LL2386;} else{ goto _LL2387;} _LL2387: _LL2470: _temp2469=
_temp2377.f1; if(( unsigned int) _temp2469 >  3u?*(( int*) _temp2469) ==  Cyc_Absyn_SizeofType:
0){ goto _LL2466;} else{ goto _LL2389;} _LL2466: _temp2465= _temp2377.f2; if((
unsigned int) _temp2465 >  3u?*(( int*) _temp2465) ==  Cyc_Absyn_IntType: 0){
_LL2468: _temp2467=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2465)->f2;
if( _temp2467 == ( void*) Cyc_Absyn_B4){ goto _LL2388;} else{ goto _LL2389;}}
else{ goto _LL2389;} _LL2389: goto _LL2390; _LL2380: { int okay= 1; if( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp2397.nullable, _temp2393.nullable)){
void* _temp2471=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2397.nullable))->v; int _temp2477; _LL2473:
if(( unsigned int) _temp2471 >  1u?*(( int*) _temp2471) ==  Cyc_Absyn_Eq_constr:
0){ _LL2478: _temp2477=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2471)->f1;
goto _LL2474;} else{ goto _LL2475;} _LL2475: goto _LL2476; _LL2474: okay= !
_temp2477; goto _LL2472; _LL2476:( int) _throw(({ void* _temp2479[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("silent_castable conref not eq", sizeof( unsigned char), 30u), _tag_arr(
_temp2479, sizeof( void*), 0u));})); _LL2472:;} if( ! Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp2397.bounds, _temp2393.bounds)){ struct _tuple8
_temp2481=({ struct _tuple8 _temp2480; _temp2480.f1=( void*)( Cyc_Absyn_compress_conref(
_temp2397.bounds))->v; _temp2480.f2=( void*)( Cyc_Absyn_compress_conref(
_temp2393.bounds))->v; _temp2480;}); void* _temp2491; void* _temp2493; void*
_temp2495; void* _temp2497; void* _temp2499; void* _temp2501; struct Cyc_Absyn_Exp*
_temp2503; void* _temp2505; void* _temp2507; struct Cyc_Absyn_Exp* _temp2509;
void* _temp2511; void* _temp2513; void* _temp2515; void* _temp2517; _LL2483:
_LL2496: _temp2495= _temp2481.f1; if(( unsigned int) _temp2495 >  1u?*(( int*)
_temp2495) ==  Cyc_Absyn_Eq_constr: 0){ _LL2498: _temp2497=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2495)->f1; if(( unsigned int) _temp2497 >  1u?*(( int*) _temp2497) ==  Cyc_Absyn_Upper_b:
0){ goto _LL2492;} else{ goto _LL2485;}} else{ goto _LL2485;} _LL2492: _temp2491=
_temp2481.f2; if(( unsigned int) _temp2491 >  1u?*(( int*) _temp2491) ==  Cyc_Absyn_Eq_constr:
0){ _LL2494: _temp2493=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2491)->f1;
if( _temp2493 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2484;} else{ goto
_LL2485;}} else{ goto _LL2485;} _LL2485: _LL2506: _temp2505= _temp2481.f1; if((
unsigned int) _temp2505 >  1u?*(( int*) _temp2505) ==  Cyc_Absyn_Eq_constr: 0){
_LL2508: _temp2507=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2505)->f1;
if(( unsigned int) _temp2507 >  1u?*(( int*) _temp2507) ==  Cyc_Absyn_Upper_b: 0){
_LL2510: _temp2509=(( struct Cyc_Absyn_Upper_b_struct*) _temp2507)->f1; goto
_LL2500;} else{ goto _LL2487;}} else{ goto _LL2487;} _LL2500: _temp2499=
_temp2481.f2; if(( unsigned int) _temp2499 >  1u?*(( int*) _temp2499) ==  Cyc_Absyn_Eq_constr:
0){ _LL2502: _temp2501=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2499)->f1;
if(( unsigned int) _temp2501 >  1u?*(( int*) _temp2501) ==  Cyc_Absyn_Upper_b: 0){
_LL2504: _temp2503=(( struct Cyc_Absyn_Upper_b_struct*) _temp2501)->f1; goto
_LL2486;} else{ goto _LL2487;}} else{ goto _LL2487;} _LL2487: _LL2516: _temp2515=
_temp2481.f1; if(( unsigned int) _temp2515 >  1u?*(( int*) _temp2515) ==  Cyc_Absyn_Eq_constr:
0){ _LL2518: _temp2517=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2515)->f1;
if( _temp2517 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2512;} else{ goto
_LL2489;}} else{ goto _LL2489;} _LL2512: _temp2511= _temp2481.f2; if((
unsigned int) _temp2511 >  1u?*(( int*) _temp2511) ==  Cyc_Absyn_Eq_constr: 0){
_LL2514: _temp2513=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2511)->f1;
if( _temp2513 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL2488;} else{ goto
_LL2489;}} else{ goto _LL2489;} _LL2489: goto _LL2490; _LL2484: okay= 1; goto
_LL2482; _LL2486: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp2509) >=  Cyc_Evexp_eval_const_uint_exp(
_temp2503): 0;({ void* _temp2519[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp2519, sizeof( void*), 0u));}); goto
_LL2482; _LL2488: okay= 1; goto _LL2482; _LL2490: okay= 0; goto _LL2482; _LL2482:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp2397.elt_typ,( void*) _temp2393.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp2397.rgn_typ,( void*) _temp2393.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp2397.rgn_typ,( void*) _temp2393.rgn_typ):
0; okay= okay? !( _temp2397.tq).q_const? 1:( _temp2393.tq).q_const: 0; return
okay;} _LL2382: { int okay; okay=( _temp2409 !=  0? _temp2401 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2409)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2401)): 0; return( okay? Cyc_Tcutil_unify(
_temp2413, _temp2405): 0)? ! _temp2411.q_const? 1: _temp2403.q_const: 0;}
_LL2384: if(( _temp2436 ==  _temp2425? 1: Cyc_Absyn_qvar_cmp( _temp2436->name,
_temp2425->name) ==  0)? _temp2434->typs ==  0: 0){ for( 0; _temp2430 !=  0?
_temp2419 !=  0: 0;( _temp2430= _temp2430->tl, _temp2419= _temp2419->tl)){ if( !
Cyc_Tcutil_unify(( void*) _temp2430->hd,( void*) _temp2419->hd)){ break;}} if(
_temp2430 ==  0? _temp2419 ==  0: 0){ return 1;}} return 0; _LL2386:{ void*
_temp2520= Cyc_Tcutil_compress( _temp2463); struct Cyc_Absyn_TunionFieldInfo
_temp2526; struct Cyc_List_List* _temp2528; void* _temp2530; struct Cyc_Absyn_Tunionfield*
_temp2532; struct Cyc_Absyn_Tuniondecl* _temp2534; _LL2522: if(( unsigned int)
_temp2520 >  3u?*(( int*) _temp2520) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2527:
_temp2526=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2520)->f1; _LL2531:
_temp2530=( void*) _temp2526.field_info; if(*(( int*) _temp2530) ==  Cyc_Absyn_KnownTunionfield){
_LL2535: _temp2534=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2530)->f1;
goto _LL2533; _LL2533: _temp2532=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2530)->f2; goto _LL2529;} else{ goto _LL2524;} _LL2529: _temp2528=
_temp2526.targs; goto _LL2523;} else{ goto _LL2524;} _LL2524: goto _LL2525;
_LL2523: if( ! Cyc_Tcutil_unify( _temp2461, _temp2442)? ! Cyc_Tcenv_region_outlives(
te, _temp2461, _temp2442): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp2459,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2455,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp2450->name, _temp2534->name) ==  0? _temp2532->typs !=  0: 0){ int okay= 1;
for( 0; _temp2528 !=  0? _temp2444 !=  0: 0;( _temp2528= _temp2528->tl,
_temp2444= _temp2444->tl)){ if( ! Cyc_Tcutil_unify(( void*) _temp2528->hd,( void*)
_temp2444->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp2528 !=  0)? 1:
_temp2444 !=  0){ return 0;} return 1;} goto _LL2521; _LL2525: goto _LL2521;
_LL2521:;} return 0; _LL2388: return 1; _LL2390: return Cyc_Tcutil_unify( t1, t2);
_LL2378:;}} int Cyc_Tcutil_is_pointer_type( void* t){ void* _temp2536= Cyc_Tcutil_compress(
t); _LL2538: if(( unsigned int) _temp2536 >  3u?*(( int*) _temp2536) ==  Cyc_Absyn_PointerType:
0){ goto _LL2539;} else{ goto _LL2540;} _LL2540: goto _LL2541; _LL2539: return 1;
_LL2541: return 0; _LL2537:;} int Cyc_Tcutil_is_zero( struct Cyc_Absyn_Exp* e){
void* _temp2542=( void*) e->r; void* _temp2548; int _temp2550; _LL2544: if(*((
int*) _temp2542) ==  Cyc_Absyn_Const_e){ _LL2549: _temp2548=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp2542)->f1; if(( unsigned int) _temp2548 >  1u?*((
int*) _temp2548) ==  Cyc_Absyn_Int_c: 0){ _LL2551: _temp2550=(( struct Cyc_Absyn_Int_c_struct*)
_temp2548)->f2; if( _temp2550 ==  0){ goto _LL2545;} else{ goto _LL2546;}} else{
goto _LL2546;}} else{ goto _LL2546;} _LL2546: goto _LL2547; _LL2545: return 1;
_LL2547: return 0; _LL2543:;} struct Cyc_Core_Opt Cyc_Tcutil_rk={( void*)(( void*)
3u)}; struct Cyc_Core_Opt Cyc_Tcutil_ak={( void*)(( void*) 0u)}; struct Cyc_Core_Opt
Cyc_Tcutil_bk={( void*)(( void*) 2u)}; struct Cyc_Core_Opt Cyc_Tcutil_mk={( void*)((
void*) 1u)}; int Cyc_Tcutil_zero_to_null( struct Cyc_Tcenv_Tenv* te, void* t2,
struct Cyc_Absyn_Exp* e1){ if( Cyc_Tcutil_is_pointer_type( t2)? Cyc_Tcutil_is_zero(
e1): 0){( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2552=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2552[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2553; _temp2553.tag= Cyc_Absyn_Const_e;
_temp2553.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2553;}); _temp2552;})));{
struct Cyc_List_List* _temp2554= Cyc_Tcenv_lookup_type_vars( te); struct Cyc_Absyn_PointerType_struct*
_temp2555=({ struct Cyc_Absyn_PointerType_struct* _temp2556=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2556[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2557; _temp2557.tag= Cyc_Absyn_PointerType;
_temp2557.f1=({ struct Cyc_Absyn_PtrInfo _temp2558; _temp2558.elt_typ=( void*)
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt*
_temp2560=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2560->v=( void*) _temp2554; _temp2560;})); _temp2558.rgn_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp2559=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2559->v=( void*)
_temp2554; _temp2559;})); _temp2558.nullable=(( struct Cyc_Absyn_Conref*(*)( int
x)) Cyc_Absyn_new_conref)( 1); _temp2558.tq= Cyc_Absyn_empty_tqual(); _temp2558.bounds=
Cyc_Absyn_empty_conref(); _temp2558;}); _temp2557;}); _temp2556;});( void*)(((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v=( void*)(( void*) _temp2555));
return Cyc_Tcutil_coerce_arg( te, e1, t2);}} return 0;} static int Cyc_Tcutil_is_sizeof_type(
void* t1){ void* _temp2561= Cyc_Tcutil_compress( t1); _LL2563: if(( unsigned int)
_temp2561 >  3u?*(( int*) _temp2561) ==  Cyc_Absyn_SizeofType: 0){ goto _LL2564;}
else{ goto _LL2565;} _LL2565: goto _LL2566; _LL2564: return 1; _LL2566: return 0;
_LL2562:;} static struct _tagged_arr Cyc_Tcutil_b2s( int b){ return b? _tag_arr("true",
sizeof( unsigned char), 5u): _tag_arr("false", sizeof( unsigned char), 6u);} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if(( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0)? 1:( Cyc_Tcutil_is_sizeof_type( t1)?
Cyc_Tcutil_is_arithmetic_type( t2): 0)){ if( Cyc_Tcutil_will_lose_precision( t1,
t2)){({ void* _temp2567[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp2567, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_zero_to_null(
te, t2, e)){ return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2);({ struct Cyc_Std_String_pa_struct _temp2570; _temp2570.tag= Cyc_Std_String_pa;
_temp2570.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp2569; _temp2569.tag= Cyc_Std_String_pa; _temp2569.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp2568[ 2u]={& _temp2569,& _temp2570};
Cyc_Tcutil_warn( e->loc, _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u), _tag_arr( _temp2568, sizeof( void*), 2u));}}}); return 1;}
else{ return 0;}}}}} int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg( te, e, t);} int
Cyc_Tcutil_coerceable( void* t){ void* _temp2571= Cyc_Tcutil_compress( t);
_LL2573: if(( unsigned int) _temp2571 >  3u?*(( int*) _temp2571) ==  Cyc_Absyn_IntType:
0){ goto _LL2574;} else{ goto _LL2575;} _LL2575: if( _temp2571 == ( void*) Cyc_Absyn_FloatType){
goto _LL2576;} else{ goto _LL2577;} _LL2577: if(( unsigned int) _temp2571 >  3u?*((
int*) _temp2571) ==  Cyc_Absyn_DoubleType: 0){ goto _LL2578;} else{ goto _LL2579;}
_LL2579: goto _LL2580; _LL2574: goto _LL2576; _LL2576: goto _LL2578; _LL2578:
return 1; _LL2580: return 0; _LL2572:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp2581=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp2581->f1= x->tq; _temp2581->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp2581;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp2582= t1; struct Cyc_List_List*
_temp2594; struct Cyc_Absyn_Structdecl** _temp2596; struct Cyc_List_List*
_temp2598; struct _tuple1* _temp2600; struct Cyc_List_List* _temp2602; _LL2584:
if( _temp2582 == ( void*) Cyc_Absyn_VoidType){ goto _LL2585;} else{ goto _LL2586;}
_LL2586: if(( unsigned int) _temp2582 >  3u?*(( int*) _temp2582) ==  Cyc_Absyn_TupleType:
0){ _LL2595: _temp2594=(( struct Cyc_Absyn_TupleType_struct*) _temp2582)->f1;
goto _LL2587;} else{ goto _LL2588;} _LL2588: if(( unsigned int) _temp2582 >  3u?*((
int*) _temp2582) ==  Cyc_Absyn_StructType: 0){ _LL2601: _temp2600=(( struct Cyc_Absyn_StructType_struct*)
_temp2582)->f1; goto _LL2599; _LL2599: _temp2598=(( struct Cyc_Absyn_StructType_struct*)
_temp2582)->f2; goto _LL2597; _LL2597: _temp2596=(( struct Cyc_Absyn_StructType_struct*)
_temp2582)->f3; goto _LL2589;} else{ goto _LL2590;} _LL2590: if(( unsigned int)
_temp2582 >  3u?*(( int*) _temp2582) ==  Cyc_Absyn_AnonStructType: 0){ _LL2603:
_temp2602=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2582)->f1; goto
_LL2591;} else{ goto _LL2592;} _LL2592: goto _LL2593; _LL2585: return 0; _LL2587:
return _temp2594; _LL2589: if( _temp2596 ==  0? 1: _temp2600 ==  0){ return({
struct Cyc_List_List* _temp2604=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2604->hd=( void*)({ struct _tuple4* _temp2605=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2605->f1= Cyc_Absyn_empty_tqual();
_temp2605->f2= t1; _temp2605;}); _temp2604->tl= 0; _temp2604;});}{ struct Cyc_Absyn_Structdecl*
_temp2606=* _temp2596; struct _tuple1 _temp2607=* _temp2600; struct Cyc_List_List*
_temp2608=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp2606->tvs, _temp2598); if( _temp2606->fields ==  0){
return({ struct Cyc_List_List* _temp2609=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2609->hd=( void*)({ struct _tuple4*
_temp2610=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2610->f1=
Cyc_Absyn_empty_tqual(); _temp2610->f2= t1; _temp2610;}); _temp2609->tl= 0;
_temp2609;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp2608,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2606->fields))->v);}
_LL2591: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp2602); _LL2593: return({
struct Cyc_List_List* _temp2611=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2611->hd=( void*)({ struct _tuple4* _temp2612=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp2612->f1= Cyc_Absyn_empty_tqual();
_temp2612->f2= t1; _temp2612;}); _temp2611->tl= 0; _temp2611;}); _LL2583:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a= a->tl){
if( Cyc_Tcutil_unify( t1,(*(( struct _tuple8*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*((
struct _tuple8*) a->hd)).f2): 0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp2614=({ struct _tuple8 _temp2613;
_temp2613.f1= t1; _temp2613.f2= t2; _temp2613;}); void* _temp2620; struct Cyc_Absyn_PtrInfo
_temp2622; struct Cyc_Absyn_Conref* _temp2624; struct Cyc_Absyn_Tqual _temp2626;
struct Cyc_Absyn_Conref* _temp2628; void* _temp2630; void* _temp2632; void*
_temp2634; struct Cyc_Absyn_PtrInfo _temp2636; struct Cyc_Absyn_Conref*
_temp2638; struct Cyc_Absyn_Tqual _temp2640; struct Cyc_Absyn_Conref* _temp2642;
void* _temp2644; void* _temp2646; _LL2616: _LL2635: _temp2634= _temp2614.f1; if((
unsigned int) _temp2634 >  3u?*(( int*) _temp2634) ==  Cyc_Absyn_PointerType: 0){
_LL2637: _temp2636=(( struct Cyc_Absyn_PointerType_struct*) _temp2634)->f1;
_LL2647: _temp2646=( void*) _temp2636.elt_typ; goto _LL2645; _LL2645: _temp2644=(
void*) _temp2636.rgn_typ; goto _LL2643; _LL2643: _temp2642= _temp2636.nullable;
goto _LL2641; _LL2641: _temp2640= _temp2636.tq; goto _LL2639; _LL2639: _temp2638=
_temp2636.bounds; goto _LL2621;} else{ goto _LL2618;} _LL2621: _temp2620=
_temp2614.f2; if(( unsigned int) _temp2620 >  3u?*(( int*) _temp2620) ==  Cyc_Absyn_PointerType:
0){ _LL2623: _temp2622=(( struct Cyc_Absyn_PointerType_struct*) _temp2620)->f1;
_LL2633: _temp2632=( void*) _temp2622.elt_typ; goto _LL2631; _LL2631: _temp2630=(
void*) _temp2622.rgn_typ; goto _LL2629; _LL2629: _temp2628= _temp2622.nullable;
goto _LL2627; _LL2627: _temp2626= _temp2622.tq; goto _LL2625; _LL2625: _temp2624=
_temp2622.bounds; goto _LL2617;} else{ goto _LL2618;} _LL2618: goto _LL2619;
_LL2617: if( _temp2640.q_const? ! _temp2626.q_const: 0){ return 0;} if(( !(( int(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp2642, _temp2628)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2642): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2628): 0){ return 0;}
if( ! Cyc_Tcutil_unify( _temp2644, _temp2630)? ! Cyc_Tcenv_region_outlives( te,
_temp2644, _temp2630): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2638, _temp2624)){ struct _tuple8 _temp2649=({ struct _tuple8 _temp2648;
_temp2648.f1= Cyc_Absyn_conref_val( _temp2638); _temp2648.f2= Cyc_Absyn_conref_val(
_temp2624); _temp2648;}); void* _temp2657; void* _temp2659; void* _temp2661;
struct Cyc_Absyn_Exp* _temp2663; void* _temp2665; struct Cyc_Absyn_Exp*
_temp2667; _LL2651: _LL2660: _temp2659= _temp2649.f1; if(( unsigned int)
_temp2659 >  1u?*(( int*) _temp2659) ==  Cyc_Absyn_Upper_b: 0){ goto _LL2658;}
else{ goto _LL2653;} _LL2658: _temp2657= _temp2649.f2; if( _temp2657 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2652;} else{ goto _LL2653;} _LL2653: _LL2666:
_temp2665= _temp2649.f1; if(( unsigned int) _temp2665 >  1u?*(( int*) _temp2665)
==  Cyc_Absyn_Upper_b: 0){ _LL2668: _temp2667=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2665)->f1; goto _LL2662;} else{ goto _LL2655;} _LL2662: _temp2661=
_temp2649.f2; if(( unsigned int) _temp2661 >  1u?*(( int*) _temp2661) ==  Cyc_Absyn_Upper_b:
0){ _LL2664: _temp2663=(( struct Cyc_Absyn_Upper_b_struct*) _temp2661)->f1; goto
_LL2654;} else{ goto _LL2655;} _LL2655: goto _LL2656; _LL2652: goto _LL2650;
_LL2654: if( Cyc_Evexp_eval_const_uint_exp( _temp2667) <  Cyc_Evexp_eval_const_uint_exp(
_temp2663)){ return 0;} goto _LL2650; _LL2656: return 0; _LL2650:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp2669=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp2669->hd=( void*)({ struct _tuple8*
_temp2670=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp2670->f1=
t1; _temp2670->f2= t2; _temp2670;}); _temp2669->tl= assume; _temp2669;}),
_temp2646, _temp2632); _LL2619: return 0; _LL2615:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp2672=({ struct _tuple8 _temp2671;
_temp2671.f1= Cyc_Tcutil_compress( t1); _temp2671.f2= Cyc_Tcutil_compress( t2);
_temp2671;}); void* _temp2678; void* _temp2680; void* _temp2682; void* _temp2684;
_LL2674: _LL2683: _temp2682= _temp2672.f1; if(( unsigned int) _temp2682 >  3u?*((
int*) _temp2682) ==  Cyc_Absyn_IntType: 0){ _LL2685: _temp2684=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2682)->f2; goto _LL2679;} else{ goto _LL2676;}
_LL2679: _temp2678= _temp2672.f2; if(( unsigned int) _temp2678 >  3u?*(( int*)
_temp2678) ==  Cyc_Absyn_IntType: 0){ _LL2681: _temp2680=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2678)->f2; goto _LL2675;} else{ goto _LL2676;} _LL2676: goto _LL2677;
_LL2675: return _temp2684 ==  _temp2680; _LL2677: return 0; _LL2673:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2= tqs2->tl, tqs1= tqs1->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp2688; void* _temp2689; struct Cyc_Absyn_Tqual _temp2691;
struct _tuple4* _temp2686=( struct _tuple4*) tqs1->hd; _temp2688=* _temp2686;
_LL2692: _temp2691= _temp2688.f1; goto _LL2690; _LL2690: _temp2689= _temp2688.f2;
goto _LL2687; _LL2687: { struct _tuple4 _temp2695; void* _temp2696; struct Cyc_Absyn_Tqual
_temp2698; struct _tuple4* _temp2693=( struct _tuple4*) tqs2->hd; _temp2695=*
_temp2693; _LL2699: _temp2698= _temp2695.f1; goto _LL2697; _LL2697: _temp2696=
_temp2695.f2; goto _LL2694; _LL2694: if( _temp2698.q_const? Cyc_Tcutil_subtype(
te, assume, _temp2689, _temp2696): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp2689, _temp2696)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp2689,
_temp2696)){ continue;} else{ return 0;}}}}}} return 1;} static int Cyc_Tcutil_is_char_type(
void* t){ void* _temp2700= Cyc_Tcutil_compress( t); void* _temp2706; _LL2702:
if(( unsigned int) _temp2700 >  3u?*(( int*) _temp2700) ==  Cyc_Absyn_IntType: 0){
_LL2707: _temp2706=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2700)->f2;
if( _temp2706 == ( void*) Cyc_Absyn_B1){ goto _LL2703;} else{ goto _LL2704;}}
else{ goto _LL2704;} _LL2704: goto _LL2705; _LL2703: return 1; _LL2705: return 0;
_LL2701:;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ void* _temp2708= t2; struct Cyc_Absyn_Uniondecl**
_temp2716; struct Cyc_List_List* _temp2718; void* _temp2720; _LL2710: if((
unsigned int) _temp2708 >  3u?*(( int*) _temp2708) ==  Cyc_Absyn_UnionType: 0){
_LL2719: _temp2718=(( struct Cyc_Absyn_UnionType_struct*) _temp2708)->f2; goto
_LL2717; _LL2717: _temp2716=(( struct Cyc_Absyn_UnionType_struct*) _temp2708)->f3;
goto _LL2711;} else{ goto _LL2712;} _LL2712: if(( unsigned int) _temp2708 >  3u?*((
int*) _temp2708) ==  Cyc_Absyn_IntType: 0){ _LL2721: _temp2720=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2708)->f2; if( _temp2720 == ( void*) Cyc_Absyn_B4){
goto _LL2713;} else{ goto _LL2714;}} else{ goto _LL2714;} _LL2714: goto _LL2715;
_LL2711: if( _temp2716 ==  0){ goto _LL2709;}{ struct Cyc_Absyn_Uniondecl*
_temp2722=* _temp2716; if( _temp2722->fields ==  0){ goto _LL2709;}{ struct Cyc_List_List*
_temp2723=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp2722->tvs, _temp2718);{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2722->fields))->v;
for( 0; fs !=  0; fs= fs->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp2723,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 1;}}}
goto _LL2709;}} _LL2713: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL2709; _LL2715: goto _LL2709; _LL2709:;}{ void* _temp2724= t1;
struct Cyc_Absyn_PtrInfo _temp2742; struct Cyc_Absyn_Conref* _temp2744; struct
Cyc_Absyn_Tqual _temp2746; struct Cyc_Absyn_Conref* _temp2748; void* _temp2750;
void* _temp2752; struct Cyc_Absyn_Exp* _temp2754; struct Cyc_Absyn_Tqual
_temp2756; void* _temp2758; struct Cyc_Absyn_Enumdecl* _temp2760; _LL2726: if((
unsigned int) _temp2724 >  3u?*(( int*) _temp2724) ==  Cyc_Absyn_PointerType: 0){
_LL2743: _temp2742=(( struct Cyc_Absyn_PointerType_struct*) _temp2724)->f1;
_LL2753: _temp2752=( void*) _temp2742.elt_typ; goto _LL2751; _LL2751: _temp2750=(
void*) _temp2742.rgn_typ; goto _LL2749; _LL2749: _temp2748= _temp2742.nullable;
goto _LL2747; _LL2747: _temp2746= _temp2742.tq; goto _LL2745; _LL2745: _temp2744=
_temp2742.bounds; goto _LL2727;} else{ goto _LL2728;} _LL2728: if(( unsigned int)
_temp2724 >  3u?*(( int*) _temp2724) ==  Cyc_Absyn_ArrayType: 0){ _LL2759:
_temp2758=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2724)->f1; goto
_LL2757; _LL2757: _temp2756=(( struct Cyc_Absyn_ArrayType_struct*) _temp2724)->f2;
goto _LL2755; _LL2755: _temp2754=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2724)->f3; goto _LL2729;} else{ goto _LL2730;} _LL2730: if(( unsigned int)
_temp2724 >  3u?*(( int*) _temp2724) ==  Cyc_Absyn_EnumType: 0){ _LL2761:
_temp2760=(( struct Cyc_Absyn_EnumType_struct*) _temp2724)->f2; goto _LL2731;}
else{ goto _LL2732;} _LL2732: if(( unsigned int) _temp2724 >  3u?*(( int*)
_temp2724) ==  Cyc_Absyn_IntType: 0){ goto _LL2733;} else{ goto _LL2734;}
_LL2734: if( _temp2724 == ( void*) Cyc_Absyn_FloatType){ goto _LL2735;} else{
goto _LL2736;} _LL2736: if(( unsigned int) _temp2724 >  3u?*(( int*) _temp2724)
==  Cyc_Absyn_DoubleType: 0){ goto _LL2737;} else{ goto _LL2738;} _LL2738: if(
_temp2724 == ( void*) Cyc_Absyn_VoidType){ goto _LL2739;} else{ goto _LL2740;}
_LL2740: goto _LL2741; _LL2727:{ void* _temp2762= t2; struct Cyc_Absyn_PtrInfo
_temp2770; struct Cyc_Absyn_Conref* _temp2772; struct Cyc_Absyn_Tqual _temp2774;
struct Cyc_Absyn_Conref* _temp2776; void* _temp2778; void* _temp2780; _LL2764:
if(( unsigned int) _temp2762 >  3u?*(( int*) _temp2762) ==  Cyc_Absyn_PointerType:
0){ _LL2771: _temp2770=(( struct Cyc_Absyn_PointerType_struct*) _temp2762)->f1;
_LL2781: _temp2780=( void*) _temp2770.elt_typ; goto _LL2779; _LL2779: _temp2778=(
void*) _temp2770.rgn_typ; goto _LL2777; _LL2777: _temp2776= _temp2770.nullable;
goto _LL2775; _LL2775: _temp2774= _temp2770.tq; goto _LL2773; _LL2773: _temp2772=
_temp2770.bounds; goto _LL2765;} else{ goto _LL2766;} _LL2766: if( _temp2762 == (
void*) Cyc_Absyn_VoidType){ goto _LL2767;} else{ goto _LL2768;} _LL2768: goto
_LL2769; _LL2765: { struct Cyc_List_List* _temp2782=({ struct Cyc_List_List*
_temp2800=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2800->hd=( void*)({ struct _tuple8* _temp2801=( struct _tuple8*) _cycalloc(
sizeof( struct _tuple8)); _temp2801->f1= t1; _temp2801->f2= t2; _temp2801;});
_temp2800->tl= 0; _temp2800;}); int _temp2783= Cyc_Tcutil_ptrsubtype( te,
_temp2782, _temp2752, _temp2780)? ! _temp2746.q_const? 1: _temp2774.q_const: 0;
int _temp2784= _temp2783? 0:(( Cyc_Tcutil_bits_only( _temp2752)? Cyc_Tcutil_is_char_type(
_temp2780): 0)? _temp2774.q_const? 1: ! _temp2746.q_const: 0); int bounds_ok=
Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp2744, _temp2772); if( !
bounds_ok? ! _temp2784: 0){ struct _tuple8 _temp2786=({ struct _tuple8 _temp2785;
_temp2785.f1= Cyc_Absyn_conref_val( _temp2744); _temp2785.f2= Cyc_Absyn_conref_val(
_temp2772); _temp2785;}); void* _temp2792; struct Cyc_Absyn_Exp* _temp2794; void*
_temp2796; struct Cyc_Absyn_Exp* _temp2798; _LL2788: _LL2797: _temp2796=
_temp2786.f1; if(( unsigned int) _temp2796 >  1u?*(( int*) _temp2796) ==  Cyc_Absyn_Upper_b:
0){ _LL2799: _temp2798=(( struct Cyc_Absyn_Upper_b_struct*) _temp2796)->f1; goto
_LL2793;} else{ goto _LL2790;} _LL2793: _temp2792= _temp2786.f2; if((
unsigned int) _temp2792 >  1u?*(( int*) _temp2792) ==  Cyc_Absyn_Upper_b: 0){
_LL2795: _temp2794=(( struct Cyc_Absyn_Upper_b_struct*) _temp2792)->f1; goto
_LL2789;} else{ goto _LL2790;} _LL2790: goto _LL2791; _LL2789: if( Cyc_Evexp_eval_const_uint_exp(
_temp2798) >=  Cyc_Evexp_eval_const_uint_exp( _temp2794)){ bounds_ok= 1;} goto
_LL2787; _LL2791: bounds_ok= 1; goto _LL2787; _LL2787:;} return( bounds_ok?
_temp2783? 1: _temp2784: 0)? Cyc_Tcutil_unify( _temp2750, _temp2778)? 1: Cyc_Tcenv_region_outlives(
te, _temp2750, _temp2778): 0;} _LL2767: return 1; _LL2769: goto _LL2763; _LL2763:;}
return 0; _LL2729:{ void* _temp2802= t2; struct Cyc_Absyn_Exp* _temp2808; struct
Cyc_Absyn_Tqual _temp2810; void* _temp2812; _LL2804: if(( unsigned int)
_temp2802 >  3u?*(( int*) _temp2802) ==  Cyc_Absyn_ArrayType: 0){ _LL2813:
_temp2812=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2802)->f1; goto
_LL2811; _LL2811: _temp2810=(( struct Cyc_Absyn_ArrayType_struct*) _temp2802)->f2;
goto _LL2809; _LL2809: _temp2808=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2802)->f3; goto _LL2805;} else{ goto _LL2806;} _LL2806: goto _LL2807;
_LL2805: { int okay; okay=( _temp2754 !=  0? _temp2808 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2754)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2808)): 0; return( okay? Cyc_Tcutil_unify(
_temp2758, _temp2812): 0)? ! _temp2756.q_const? 1: _temp2810.q_const: 0;}
_LL2807: return 0; _LL2803:;} return 0; _LL2731:{ void* _temp2814= t2; struct
Cyc_Absyn_Enumdecl* _temp2820; _LL2816: if(( unsigned int) _temp2814 >  3u?*((
int*) _temp2814) ==  Cyc_Absyn_EnumType: 0){ _LL2821: _temp2820=(( struct Cyc_Absyn_EnumType_struct*)
_temp2814)->f2; goto _LL2817;} else{ goto _LL2818;} _LL2818: goto _LL2819;
_LL2817: if(( _temp2760->fields !=  0? _temp2820->fields !=  0: 0)?(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2760->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2820->fields))->v):
0){ return 1;} goto _LL2815; _LL2819: goto _LL2815; _LL2815:;} goto _LL2733;
_LL2733: goto _LL2735; _LL2735: goto _LL2737; _LL2737: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType; _LL2739: return t2 == ( void*) Cyc_Absyn_VoidType;
_LL2741: return 0; _LL2725:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2822=
Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2823=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2823[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2824; _temp2824.tag= Cyc_Absyn_Cast_e;
_temp2824.f1=( void*) t; _temp2824.f2= _temp2822; _temp2824;}); _temp2823;})));
e->topt=({ struct Cyc_Core_Opt* _temp2825=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2825->v=( void*) t; _temp2825;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2826= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2828: if(( unsigned int) _temp2826
>  3u?*(( int*) _temp2826) ==  Cyc_Absyn_EnumType: 0){ goto _LL2829;} else{ goto
_LL2830;} _LL2830: if(( unsigned int) _temp2826 >  3u?*(( int*) _temp2826) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL2831;} else{ goto _LL2832;} _LL2832: if((
unsigned int) _temp2826 >  3u?*(( int*) _temp2826) ==  Cyc_Absyn_IntType: 0){
goto _LL2833;} else{ goto _LL2834;} _LL2834: if(( unsigned int) _temp2826 >  3u?*((
int*) _temp2826) ==  Cyc_Absyn_Evar: 0){ goto _LL2835;} else{ goto _LL2836;}
_LL2836: if(( unsigned int) _temp2826 >  3u?*(( int*) _temp2826) ==  Cyc_Absyn_SizeofType:
0){ goto _LL2837;} else{ goto _LL2838;} _LL2838: goto _LL2839; _LL2829: goto
_LL2831; _LL2831: goto _LL2833; _LL2833: return 1; _LL2835: return Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t);
_LL2837: return 1; _LL2839: return 0; _LL2827:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp2840= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL2842: if( _temp2840 == ( void*) Cyc_Absyn_FloatType){ goto _LL2843;} else{
goto _LL2844;} _LL2844: if(( unsigned int) _temp2840 >  3u?*(( int*) _temp2840)
==  Cyc_Absyn_DoubleType: 0){ goto _LL2845;} else{ goto _LL2846;} _LL2846: goto
_LL2847; _LL2843: goto _LL2845; _LL2845: return 1; _LL2847: return 0; _LL2841:;}}
int Cyc_Tcutil_is_function_type( void* t){ void* _temp2848= Cyc_Tcutil_compress(
t); _LL2850: if(( unsigned int) _temp2848 >  3u?*(( int*) _temp2848) ==  Cyc_Absyn_FnType:
0){ goto _LL2851;} else{ goto _LL2852;} _LL2852: goto _LL2853; _LL2851: return 1;
_LL2853: return 0; _LL2849:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1,
void* t2){ struct _tuple8 _temp2855=({ struct _tuple8 _temp2854; _temp2854.f1=
t1; _temp2854.f2= t2; _temp2854;}); void* _temp2881; int _temp2883; void*
_temp2885; int _temp2887; void* _temp2889; void* _temp2891; void* _temp2893;
void* _temp2895; void* _temp2897; void* _temp2899; void* _temp2901; void*
_temp2903; void* _temp2905; void* _temp2907; void* _temp2909; void* _temp2911;
void* _temp2913; void* _temp2915; void* _temp2917; void* _temp2919; void*
_temp2921; void* _temp2923; void* _temp2925; void* _temp2927; void* _temp2929;
void* _temp2931; _LL2857: _LL2886: _temp2885= _temp2855.f1; if(( unsigned int)
_temp2885 >  3u?*(( int*) _temp2885) ==  Cyc_Absyn_DoubleType: 0){ _LL2888:
_temp2887=(( struct Cyc_Absyn_DoubleType_struct*) _temp2885)->f1; goto _LL2882;}
else{ goto _LL2859;} _LL2882: _temp2881= _temp2855.f2; if(( unsigned int)
_temp2881 >  3u?*(( int*) _temp2881) ==  Cyc_Absyn_DoubleType: 0){ _LL2884:
_temp2883=(( struct Cyc_Absyn_DoubleType_struct*) _temp2881)->f1; goto _LL2858;}
else{ goto _LL2859;} _LL2859: _LL2890: _temp2889= _temp2855.f1; if((
unsigned int) _temp2889 >  3u?*(( int*) _temp2889) ==  Cyc_Absyn_DoubleType: 0){
goto _LL2860;} else{ goto _LL2861;} _LL2861: _LL2892: _temp2891= _temp2855.f2;
if(( unsigned int) _temp2891 >  3u?*(( int*) _temp2891) ==  Cyc_Absyn_DoubleType:
0){ goto _LL2862;} else{ goto _LL2863;} _LL2863: _LL2894: _temp2893= _temp2855.f1;
if( _temp2893 == ( void*) Cyc_Absyn_FloatType){ goto _LL2864;} else{ goto
_LL2865;} _LL2865: _LL2896: _temp2895= _temp2855.f2; if( _temp2895 == ( void*)
Cyc_Absyn_FloatType){ goto _LL2866;} else{ goto _LL2867;} _LL2867: _LL2898:
_temp2897= _temp2855.f1; if(( unsigned int) _temp2897 >  3u?*(( int*) _temp2897)
==  Cyc_Absyn_IntType: 0){ _LL2902: _temp2901=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2897)->f1; if( _temp2901 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2900;}
else{ goto _LL2869;} _LL2900: _temp2899=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2897)->f2; if( _temp2899 == ( void*) Cyc_Absyn_B8){ goto _LL2868;} else{
goto _LL2869;}} else{ goto _LL2869;} _LL2869: _LL2904: _temp2903= _temp2855.f2;
if(( unsigned int) _temp2903 >  3u?*(( int*) _temp2903) ==  Cyc_Absyn_IntType: 0){
_LL2908: _temp2907=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2903)->f1;
if( _temp2907 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2906;} else{ goto _LL2871;}
_LL2906: _temp2905=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2903)->f2;
if( _temp2905 == ( void*) Cyc_Absyn_B8){ goto _LL2870;} else{ goto _LL2871;}}
else{ goto _LL2871;} _LL2871: _LL2910: _temp2909= _temp2855.f1; if((
unsigned int) _temp2909 >  3u?*(( int*) _temp2909) ==  Cyc_Absyn_IntType: 0){
_LL2914: _temp2913=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2909)->f1;
if( _temp2913 == ( void*) Cyc_Absyn_Signed){ goto _LL2912;} else{ goto _LL2873;}
_LL2912: _temp2911=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2909)->f2;
if( _temp2911 == ( void*) Cyc_Absyn_B8){ goto _LL2872;} else{ goto _LL2873;}}
else{ goto _LL2873;} _LL2873: _LL2916: _temp2915= _temp2855.f2; if((
unsigned int) _temp2915 >  3u?*(( int*) _temp2915) ==  Cyc_Absyn_IntType: 0){
_LL2920: _temp2919=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2915)->f1;
if( _temp2919 == ( void*) Cyc_Absyn_Signed){ goto _LL2918;} else{ goto _LL2875;}
_LL2918: _temp2917=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2915)->f2;
if( _temp2917 == ( void*) Cyc_Absyn_B8){ goto _LL2874;} else{ goto _LL2875;}}
else{ goto _LL2875;} _LL2875: _LL2922: _temp2921= _temp2855.f1; if((
unsigned int) _temp2921 >  3u?*(( int*) _temp2921) ==  Cyc_Absyn_IntType: 0){
_LL2926: _temp2925=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2921)->f1;
if( _temp2925 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2924;} else{ goto _LL2877;}
_LL2924: _temp2923=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2921)->f2;
if( _temp2923 == ( void*) Cyc_Absyn_B4){ goto _LL2876;} else{ goto _LL2877;}}
else{ goto _LL2877;} _LL2877: _LL2928: _temp2927= _temp2855.f2; if((
unsigned int) _temp2927 >  3u?*(( int*) _temp2927) ==  Cyc_Absyn_IntType: 0){
_LL2932: _temp2931=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2927)->f1;
if( _temp2931 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2930;} else{ goto _LL2879;}
_LL2930: _temp2929=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2927)->f2;
if( _temp2929 == ( void*) Cyc_Absyn_B4){ goto _LL2878;} else{ goto _LL2879;}}
else{ goto _LL2879;} _LL2879: goto _LL2880; _LL2858: if( _temp2887){ return t1;}
else{ return t2;} _LL2860: return t1; _LL2862: return t2; _LL2864: goto _LL2866;
_LL2866: return( void*) Cyc_Absyn_FloatType; _LL2868: goto _LL2870; _LL2870:
return Cyc_Absyn_ulonglong_t; _LL2872: goto _LL2874; _LL2874: return Cyc_Absyn_slonglong_t;
_LL2876: goto _LL2878; _LL2878: return Cyc_Absyn_uint_t; _LL2880: return Cyc_Absyn_sint_t;
_LL2856:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2933=( void*) e->r; struct Cyc_Core_Opt* _temp2939; _LL2935: if(*(( int*)
_temp2933) ==  Cyc_Absyn_AssignOp_e){ _LL2940: _temp2939=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2933)->f2; if( _temp2939 ==  0){ goto _LL2936;} else{ goto _LL2937;}} else{
goto _LL2937;} _LL2937: goto _LL2938; _LL2936:({ void* _temp2941[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2941, sizeof( void*), 0u));}); goto _LL2934; _LL2938: goto _LL2934; _LL2934:;}
static int Cyc_Tcutil_constrain_kinds( void* c1, void* c2){ c1= Cyc_Absyn_compress_kb(
c1); c2= Cyc_Absyn_compress_kb( c2);{ struct _tuple8 _temp2943=({ struct _tuple8
_temp2942; _temp2942.f1= c1; _temp2942.f2= c2; _temp2942;}); void* _temp2957;
void* _temp2959; void* _temp2961; void* _temp2963; void* _temp2965; struct Cyc_Core_Opt*
_temp2967; struct Cyc_Core_Opt** _temp2969; void* _temp2970; struct Cyc_Core_Opt*
_temp2972; struct Cyc_Core_Opt** _temp2974; void* _temp2975; void* _temp2977;
void* _temp2979; void* _temp2981; struct Cyc_Core_Opt* _temp2983; struct Cyc_Core_Opt**
_temp2985; void* _temp2986; void* _temp2988; struct Cyc_Core_Opt* _temp2990;
struct Cyc_Core_Opt** _temp2992; void* _temp2993; void* _temp2995; void*
_temp2997; void* _temp2999; struct Cyc_Core_Opt* _temp3001; struct Cyc_Core_Opt**
_temp3003; void* _temp3004; void* _temp3006; struct Cyc_Core_Opt* _temp3008;
struct Cyc_Core_Opt** _temp3010; _LL2945: _LL2962: _temp2961= _temp2943.f1; if(*((
int*) _temp2961) ==  Cyc_Absyn_Eq_kb){ _LL2964: _temp2963=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp2961)->f1; goto _LL2958;} else{ goto _LL2947;} _LL2958: _temp2957=
_temp2943.f2; if(*(( int*) _temp2957) ==  Cyc_Absyn_Eq_kb){ _LL2960: _temp2959=(
void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp2957)->f1; goto _LL2946;} else{
goto _LL2947;} _LL2947: _LL2966: _temp2965= _temp2943.f2; if(*(( int*) _temp2965)
==  Cyc_Absyn_Unknown_kb){ _LL2968: _temp2967=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp2965)->f1; _temp2969=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp2965)->f1; goto _LL2948;} else{ goto _LL2949;} _LL2949: _LL2971: _temp2970=
_temp2943.f1; if(*(( int*) _temp2970) ==  Cyc_Absyn_Unknown_kb){ _LL2973:
_temp2972=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp2970)->f1; _temp2974=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*) _temp2970)->f1;
goto _LL2950;} else{ goto _LL2951;} _LL2951: _LL2980: _temp2979= _temp2943.f1;
if(*(( int*) _temp2979) ==  Cyc_Absyn_Less_kb){ _LL2984: _temp2983=(( struct Cyc_Absyn_Less_kb_struct*)
_temp2979)->f1; _temp2985=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp2979)->f1; goto _LL2982; _LL2982: _temp2981=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp2979)->f2; goto _LL2976;} else{ goto _LL2953;} _LL2976: _temp2975=
_temp2943.f2; if(*(( int*) _temp2975) ==  Cyc_Absyn_Eq_kb){ _LL2978: _temp2977=(
void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp2975)->f1; goto _LL2952;} else{
goto _LL2953;} _LL2953: _LL2994: _temp2993= _temp2943.f1; if(*(( int*) _temp2993)
==  Cyc_Absyn_Eq_kb){ _LL2996: _temp2995=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp2993)->f1; goto _LL2987;} else{ goto _LL2955;} _LL2987: _temp2986=
_temp2943.f2; if(*(( int*) _temp2986) ==  Cyc_Absyn_Less_kb){ _LL2991: _temp2990=((
struct Cyc_Absyn_Less_kb_struct*) _temp2986)->f1; _temp2992=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*) _temp2986)->f1; goto _LL2989; _LL2989:
_temp2988=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp2986)->f2; goto
_LL2954;} else{ goto _LL2955;} _LL2955: _LL3005: _temp3004= _temp2943.f1; if(*((
int*) _temp3004) ==  Cyc_Absyn_Less_kb){ _LL3009: _temp3008=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3004)->f1; _temp3010=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3004)->f1; goto _LL3007; _LL3007: _temp3006=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3004)->f2; goto _LL2998;} else{ goto _LL2944;} _LL2998: _temp2997=
_temp2943.f2; if(*(( int*) _temp2997) ==  Cyc_Absyn_Less_kb){ _LL3002: _temp3001=((
struct Cyc_Absyn_Less_kb_struct*) _temp2997)->f1; _temp3003=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*) _temp2997)->f1; goto _LL3000; _LL3000:
_temp2999=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp2997)->f2; goto
_LL2956;} else{ goto _LL2944;} _LL2946: return _temp2963 ==  _temp2959; _LL2948:*
_temp2969=({ struct Cyc_Core_Opt* _temp3011=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3011->v=( void*) c1; _temp3011;}); return 1;
_LL2950:* _temp2974=({ struct Cyc_Core_Opt* _temp3012=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3012->v=( void*) c2; _temp3012;});
return 1; _LL2952: if( Cyc_Tcutil_kind_leq( _temp2977, _temp2981)){* _temp2985=({
struct Cyc_Core_Opt* _temp3013=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3013->v=( void*) c2; _temp3013;}); return 1;} else{ return
0;} _LL2954: if( Cyc_Tcutil_kind_leq( _temp2995, _temp2988)){* _temp2992=({
struct Cyc_Core_Opt* _temp3014=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3014->v=( void*) c1; _temp3014;}); return 1;} else{ return
0;} _LL2956: if( Cyc_Tcutil_kind_leq( _temp3006, _temp2999)){* _temp3003=({
struct Cyc_Core_Opt* _temp3015=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3015->v=( void*) c1; _temp3015;}); return 1;} else{ if( Cyc_Tcutil_kind_leq(
_temp2999, _temp3006)){* _temp3010=({ struct Cyc_Core_Opt* _temp3016=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3016->v=( void*)
c2; _temp3016;}); return 1;} else{ return 0;}} _LL2944:;}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp3017=( int*)
_cycalloc_atomic( sizeof( int)); _temp3017[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp3017;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_arr s=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp3021; _temp3021.tag= Cyc_Std_Int_pa;
_temp3021.f1=( int)(( unsigned int) i);{ void* _temp3020[ 1u]={& _temp3021}; Cyc_Std_aprintf(
_tag_arr("#%d", sizeof( unsigned char), 4u), _tag_arr( _temp3020, sizeof( void*),
1u));}}); return({ struct Cyc_Absyn_Tvar* _temp3018=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3018->name=({ struct
_tagged_arr* _temp3019=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr) *  1); _temp3019[ 0]= s; _temp3019;}); _temp3018->identity= 0;
_temp3018->kind=( void*) k; _temp3018;});} int Cyc_Tcutil_is_temp_tvar( struct
Cyc_Absyn_Tvar* t){ struct _tagged_arr _temp3022=* t->name; return*(( const
unsigned char*) _check_unknown_subscript( _temp3022, sizeof( unsigned char), 0))
== '#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){({ struct
Cyc_Std_String_pa_struct _temp3024; _temp3024.tag= Cyc_Std_String_pa; _temp3024.f1=(
struct _tagged_arr)* t->name;{ void* _temp3023[ 1u]={& _temp3024}; Cyc_Std_printf(
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp3023, sizeof( void*),
1u));}}); if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr
_temp3025= Cyc_Std_strconcat( _tag_arr("`", sizeof( unsigned char), 2u),* t->name);*((
unsigned char*) _check_unknown_subscript( _temp3025, sizeof( unsigned char), 1))='t';
t->name=({ struct _tagged_arr* _temp3026=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr) *  1); _temp3026[ 0]=( struct _tagged_arr) _temp3025;
_temp3026;});}} struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct
_tuple13* x){ return({ struct _tuple2* _temp3027=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp3027->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp3028=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3028->v=( void*)(* x).f1; _temp3028;}); _temp3027->f2=(* x).f2; _temp3027->f3=(*
x).f3; _temp3027;});} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){
if( fd->cached_typ ==  0){ struct Cyc_List_List* _temp3029= 0;{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts !=  0; atts= atts->tl){ if( Cyc_Absyn_fntype_att((
void*) atts->hd)){ _temp3029=({ struct Cyc_List_List* _temp3030=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3030->hd=( void*)(( void*) atts->hd);
_temp3030->tl= _temp3029; _temp3030;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp3031=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp3031[ 0]=({ struct Cyc_Absyn_FnType_struct _temp3032; _temp3032.tag= Cyc_Absyn_FnType;
_temp3032.f1=({ struct Cyc_Absyn_FnInfo _temp3033; _temp3033.tvars= fd->tvs;
_temp3033.effect= fd->effect; _temp3033.ret_typ=( void*)(( void*) fd->ret_type);
_temp3033.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp3033.c_varargs= fd->c_varargs; _temp3033.cyc_varargs= fd->cyc_varargs;
_temp3033.rgn_po= fd->rgn_po; _temp3033.attributes= _temp3029; _temp3033;});
_temp3032;}); _temp3031;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp3034=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp3034->f1=(* pr).f1; _temp3034->f2= t; _temp3034;});} struct _tuple15{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct
_tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp3035=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp3035->f1=({ struct _tuple15* _temp3036=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp3036->f1=(* y).f1; _temp3036->f2=(* y).f2;
_temp3036;}); _temp3035->f2=(* y).f3; _temp3035;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp3039; struct _tuple15* _temp3041; struct
_tuple16 _temp3037=* w; _LL3042: _temp3041= _temp3037.f1; goto _LL3040; _LL3040:
_temp3039= _temp3037.f2; goto _LL3038; _LL3038: { struct Cyc_Absyn_Tqual
_temp3045; struct Cyc_Core_Opt* _temp3047; struct _tuple15 _temp3043=* _temp3041;
_LL3048: _temp3047= _temp3043.f1; goto _LL3046; _LL3046: _temp3045= _temp3043.f2;
goto _LL3044; _LL3044: return({ struct _tuple2* _temp3049=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp3049->f1= _temp3047; _temp3049->f2=
_temp3045; _temp3049->f3= _temp3039; _temp3049;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp3050=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp3050->name= f->name;
_temp3050->tq= f->tq; _temp3050->type=( void*) t; _temp3050->width= f->width;
_temp3050->attributes= f->attributes; _temp3050;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp3051= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp3103; struct Cyc_Absyn_Structdecl** _temp3105;
struct Cyc_List_List* _temp3107; struct _tuple1* _temp3109; struct Cyc_Absyn_Uniondecl**
_temp3111; struct Cyc_List_List* _temp3113; struct _tuple1* _temp3115; struct
Cyc_Absyn_TunionInfo _temp3117; void* _temp3119; struct Cyc_List_List* _temp3121;
void* _temp3123; struct Cyc_Absyn_TunionFieldInfo _temp3125; struct Cyc_List_List*
_temp3127; void* _temp3129; struct Cyc_Core_Opt* _temp3131; struct Cyc_List_List*
_temp3133; struct _tuple1* _temp3135; struct Cyc_Absyn_Exp* _temp3137; struct
Cyc_Absyn_Tqual _temp3139; void* _temp3141; struct Cyc_Absyn_PtrInfo _temp3143;
struct Cyc_Absyn_Conref* _temp3145; struct Cyc_Absyn_Tqual _temp3147; struct Cyc_Absyn_Conref*
_temp3149; void* _temp3151; void* _temp3153; struct Cyc_Absyn_FnInfo _temp3155;
struct Cyc_List_List* _temp3157; struct Cyc_List_List* _temp3159; struct Cyc_Absyn_VarargInfo*
_temp3161; int _temp3163; struct Cyc_List_List* _temp3165; void* _temp3167;
struct Cyc_Core_Opt* _temp3169; struct Cyc_List_List* _temp3171; struct Cyc_List_List*
_temp3173; struct Cyc_List_List* _temp3175; struct Cyc_List_List* _temp3177;
struct Cyc_Core_Opt* _temp3179; void* _temp3181; void* _temp3183; void*
_temp3185; void* _temp3187; struct Cyc_List_List* _temp3189; _LL3053: if((
unsigned int) _temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_VarType: 0){
_LL3104: _temp3103=(( struct Cyc_Absyn_VarType_struct*) _temp3051)->f1; goto
_LL3054;} else{ goto _LL3055;} _LL3055: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_StructType: 0){ _LL3110: _temp3109=(( struct Cyc_Absyn_StructType_struct*)
_temp3051)->f1; goto _LL3108; _LL3108: _temp3107=(( struct Cyc_Absyn_StructType_struct*)
_temp3051)->f2; goto _LL3106; _LL3106: _temp3105=(( struct Cyc_Absyn_StructType_struct*)
_temp3051)->f3; goto _LL3056;} else{ goto _LL3057;} _LL3057: if(( unsigned int)
_temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_UnionType: 0){ _LL3116:
_temp3115=(( struct Cyc_Absyn_UnionType_struct*) _temp3051)->f1; goto _LL3114;
_LL3114: _temp3113=(( struct Cyc_Absyn_UnionType_struct*) _temp3051)->f2; goto
_LL3112; _LL3112: _temp3111=(( struct Cyc_Absyn_UnionType_struct*) _temp3051)->f3;
goto _LL3058;} else{ goto _LL3059;} _LL3059: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_TunionType: 0){ _LL3118: _temp3117=(( struct Cyc_Absyn_TunionType_struct*)
_temp3051)->f1; _LL3124: _temp3123=( void*) _temp3117.tunion_info; goto _LL3122;
_LL3122: _temp3121= _temp3117.targs; goto _LL3120; _LL3120: _temp3119=( void*)
_temp3117.rgn; goto _LL3060;} else{ goto _LL3061;} _LL3061: if(( unsigned int)
_temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_TunionFieldType: 0){ _LL3126:
_temp3125=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp3051)->f1; _LL3130:
_temp3129=( void*) _temp3125.field_info; goto _LL3128; _LL3128: _temp3127=
_temp3125.targs; goto _LL3062;} else{ goto _LL3063;} _LL3063: if(( unsigned int)
_temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_TypedefType: 0){ _LL3136:
_temp3135=(( struct Cyc_Absyn_TypedefType_struct*) _temp3051)->f1; goto _LL3134;
_LL3134: _temp3133=(( struct Cyc_Absyn_TypedefType_struct*) _temp3051)->f2; goto
_LL3132; _LL3132: _temp3131=(( struct Cyc_Absyn_TypedefType_struct*) _temp3051)->f3;
goto _LL3064;} else{ goto _LL3065;} _LL3065: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_ArrayType: 0){ _LL3142: _temp3141=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3051)->f1; goto _LL3140; _LL3140:
_temp3139=(( struct Cyc_Absyn_ArrayType_struct*) _temp3051)->f2; goto _LL3138;
_LL3138: _temp3137=(( struct Cyc_Absyn_ArrayType_struct*) _temp3051)->f3; goto
_LL3066;} else{ goto _LL3067;} _LL3067: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_PointerType: 0){ _LL3144: _temp3143=(( struct Cyc_Absyn_PointerType_struct*)
_temp3051)->f1; _LL3154: _temp3153=( void*) _temp3143.elt_typ; goto _LL3152;
_LL3152: _temp3151=( void*) _temp3143.rgn_typ; goto _LL3150; _LL3150: _temp3149=
_temp3143.nullable; goto _LL3148; _LL3148: _temp3147= _temp3143.tq; goto _LL3146;
_LL3146: _temp3145= _temp3143.bounds; goto _LL3068;} else{ goto _LL3069;}
_LL3069: if(( unsigned int) _temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_FnType:
0){ _LL3156: _temp3155=(( struct Cyc_Absyn_FnType_struct*) _temp3051)->f1;
_LL3172: _temp3171= _temp3155.tvars; goto _LL3170; _LL3170: _temp3169= _temp3155.effect;
goto _LL3168; _LL3168: _temp3167=( void*) _temp3155.ret_typ; goto _LL3166;
_LL3166: _temp3165= _temp3155.args; goto _LL3164; _LL3164: _temp3163= _temp3155.c_varargs;
goto _LL3162; _LL3162: _temp3161= _temp3155.cyc_varargs; goto _LL3160; _LL3160:
_temp3159= _temp3155.rgn_po; goto _LL3158; _LL3158: _temp3157= _temp3155.attributes;
goto _LL3070;} else{ goto _LL3071;} _LL3071: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_TupleType: 0){ _LL3174: _temp3173=(( struct Cyc_Absyn_TupleType_struct*)
_temp3051)->f1; goto _LL3072;} else{ goto _LL3073;} _LL3073: if(( unsigned int)
_temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_AnonStructType: 0){ _LL3176:
_temp3175=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3051)->f1; goto
_LL3074;} else{ goto _LL3075;} _LL3075: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3178: _temp3177=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3051)->f1; goto _LL3076;} else{ goto
_LL3077;} _LL3077: if(( unsigned int) _temp3051 >  3u?*(( int*) _temp3051) == 
Cyc_Absyn_Evar: 0){ _LL3180: _temp3179=(( struct Cyc_Absyn_Evar_struct*)
_temp3051)->f2; goto _LL3078;} else{ goto _LL3079;} _LL3079: if(( unsigned int)
_temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_RgnHandleType: 0){ _LL3182:
_temp3181=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp3051)->f1;
goto _LL3080;} else{ goto _LL3081;} _LL3081: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_SizeofType: 0){ _LL3184: _temp3183=( void*)((
struct Cyc_Absyn_SizeofType_struct*) _temp3051)->f1; goto _LL3082;} else{ goto
_LL3083;} _LL3083: if(( unsigned int) _temp3051 >  3u?*(( int*) _temp3051) == 
Cyc_Absyn_EnumType: 0){ goto _LL3084;} else{ goto _LL3085;} _LL3085: if((
unsigned int) _temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_AnonEnumType: 0){
goto _LL3086;} else{ goto _LL3087;} _LL3087: if( _temp3051 == ( void*) Cyc_Absyn_VoidType){
goto _LL3088;} else{ goto _LL3089;} _LL3089: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_IntType: 0){ goto _LL3090;} else{ goto _LL3091;}
_LL3091: if( _temp3051 == ( void*) Cyc_Absyn_FloatType){ goto _LL3092;} else{
goto _LL3093;} _LL3093: if(( unsigned int) _temp3051 >  3u?*(( int*) _temp3051)
==  Cyc_Absyn_DoubleType: 0){ goto _LL3094;} else{ goto _LL3095;} _LL3095: if(
_temp3051 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL3096;} else{ goto _LL3097;}
_LL3097: if(( unsigned int) _temp3051 >  3u?*(( int*) _temp3051) ==  Cyc_Absyn_RgnsEff:
0){ _LL3186: _temp3185=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp3051)->f1;
goto _LL3098;} else{ goto _LL3099;} _LL3099: if(( unsigned int) _temp3051 >  3u?*((
int*) _temp3051) ==  Cyc_Absyn_AccessEff: 0){ _LL3188: _temp3187=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp3051)->f1; goto _LL3100;} else{ goto
_LL3101;} _LL3101: if(( unsigned int) _temp3051 >  3u?*(( int*) _temp3051) == 
Cyc_Absyn_JoinEff: 0){ _LL3190: _temp3189=(( struct Cyc_Absyn_JoinEff_struct*)
_temp3051)->f1; goto _LL3102;} else{ goto _LL3052;} _LL3054: { struct
_handler_cons _temp3191; _push_handler(& _temp3191);{ int _temp3193= 0; if(
setjmp( _temp3191.handler)){ _temp3193= 1;} if( ! _temp3193){{ void* _temp3194=((
void*(*)( int(* cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp3103); _npop_handler( 0u); return _temp3194;}; _pop_handler();} else{
void* _temp3192=( void*) _exn_thrown; void* _temp3196= _temp3192; _LL3198: if(
_temp3196 ==  Cyc_Core_Not_found){ goto _LL3199;} else{ goto _LL3200;} _LL3200:
goto _LL3201; _LL3199: return t; _LL3201:( void) _throw( _temp3196); _LL3197:;}}}
_LL3056: { struct Cyc_List_List* _temp3202= Cyc_Tcutil_substs( rgn, inst,
_temp3107); return _temp3202 ==  _temp3107? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp3203=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3203[ 0]=({ struct Cyc_Absyn_StructType_struct _temp3204; _temp3204.tag=
Cyc_Absyn_StructType; _temp3204.f1= _temp3109; _temp3204.f2= _temp3202;
_temp3204.f3= _temp3105; _temp3204;}); _temp3203;});} _LL3058: { struct Cyc_List_List*
_temp3205= Cyc_Tcutil_substs( rgn, inst, _temp3113); return _temp3205 == 
_temp3113? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp3206=( struct
Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3206[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3207; _temp3207.tag= Cyc_Absyn_UnionType;
_temp3207.f1= _temp3115; _temp3207.f2= _temp3205; _temp3207.f3= _temp3111;
_temp3207;}); _temp3206;});} _LL3060: { struct Cyc_List_List* _temp3208= Cyc_Tcutil_substs(
rgn, inst, _temp3121); void* _temp3209= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3119); return( _temp3208 ==  _temp3121? _temp3209 ==  _temp3119: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp3210=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3210[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3211; _temp3211.tag= Cyc_Absyn_TunionType;
_temp3211.f1=({ struct Cyc_Absyn_TunionInfo _temp3212; _temp3212.tunion_info=(
void*) _temp3123; _temp3212.targs= _temp3208; _temp3212.rgn=( void*) _temp3209;
_temp3212;}); _temp3211;}); _temp3210;});} _LL3062: { struct Cyc_List_List*
_temp3213= Cyc_Tcutil_substs( rgn, inst, _temp3127); return _temp3213 == 
_temp3127? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp3214=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp3214[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp3215; _temp3215.tag=
Cyc_Absyn_TunionFieldType; _temp3215.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp3216; _temp3216.field_info=( void*) _temp3129; _temp3216.targs= _temp3213;
_temp3216;}); _temp3215;}); _temp3214;});} _LL3064: { struct Cyc_List_List*
_temp3217= Cyc_Tcutil_substs( rgn, inst, _temp3133); return _temp3217 == 
_temp3133? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp3218=( struct
Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3218[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3219; _temp3219.tag=
Cyc_Absyn_TypedefType; _temp3219.f1= _temp3135; _temp3219.f2= _temp3217;
_temp3219.f3= _temp3131; _temp3219;}); _temp3218;});} _LL3066: { void* _temp3220=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp3141); return _temp3220 ==  _temp3141? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp3221=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp3221[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp3222; _temp3222.tag= Cyc_Absyn_ArrayType;
_temp3222.f1=( void*) _temp3220; _temp3222.f2= _temp3139; _temp3222.f3=
_temp3137; _temp3222;}); _temp3221;});} _LL3068: { void* _temp3223= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3153); void* _temp3224= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3151); if( _temp3223 ==  _temp3153? _temp3224 ==  _temp3151: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp3225=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp3225[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp3226; _temp3226.tag= Cyc_Absyn_PointerType;
_temp3226.f1=({ struct Cyc_Absyn_PtrInfo _temp3227; _temp3227.elt_typ=( void*)
_temp3223; _temp3227.rgn_typ=( void*) _temp3224; _temp3227.nullable= _temp3149;
_temp3227.tq= _temp3147; _temp3227.bounds= _temp3145; _temp3227;}); _temp3226;});
_temp3225;});} _LL3070:{ struct Cyc_List_List* _temp3228= _temp3171; for( 0;
_temp3228 !=  0; _temp3228= _temp3228->tl){ inst=({ struct Cyc_List_List*
_temp3229=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3229->hd=( void*)({ struct _tuple5* _temp3230=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp3230->f1=( struct Cyc_Absyn_Tvar*)
_temp3228->hd; _temp3230->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3231=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3231[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3232; _temp3232.tag= Cyc_Absyn_VarType;
_temp3232.f1=( struct Cyc_Absyn_Tvar*) _temp3228->hd; _temp3232;}); _temp3231;});
_temp3230;}); _temp3229->tl= inst; _temp3229;});}}{ struct Cyc_List_List*
_temp3235; struct Cyc_List_List* _temp3237; struct _tuple0 _temp3233=(( struct
_tuple0(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp3165)); _LL3238: _temp3237= _temp3233.f1; goto _LL3236; _LL3236:
_temp3235= _temp3233.f2; goto _LL3234; _LL3234: { struct Cyc_List_List*
_temp3239= Cyc_Tcutil_substs( rgn, inst, _temp3235); struct Cyc_List_List*
_temp3240=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple16*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3237, _temp3239));
struct Cyc_Core_Opt* eff2; if( _temp3169 ==  0){ eff2= 0;} else{ void* _temp3241=
Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp3169->v); if( _temp3241 == (
void*) _temp3169->v){ eff2= _temp3169;} else{ eff2=({ struct Cyc_Core_Opt*
_temp3242=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3242->v=( void*) _temp3241; _temp3242;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp3161 ==  0){ cyc_varargs2= 0;} else{ int _temp3245; void*
_temp3247; struct Cyc_Absyn_Tqual _temp3249; struct Cyc_Core_Opt* _temp3251;
struct Cyc_Absyn_VarargInfo _temp3243=* _temp3161; _LL3252: _temp3251= _temp3243.name;
goto _LL3250; _LL3250: _temp3249= _temp3243.tq; goto _LL3248; _LL3248: _temp3247=(
void*) _temp3243.type; goto _LL3246; _LL3246: _temp3245= _temp3243.inject; goto
_LL3244; _LL3244: { void* _temp3253= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3247); if( _temp3253 ==  _temp3247){ cyc_varargs2= _temp3161;} else{
cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp3254=( struct Cyc_Absyn_VarargInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp3254->name= _temp3251;
_temp3254->tq= _temp3249; _temp3254->type=( void*) _temp3253; _temp3254->inject=
_temp3245; _temp3254;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List*
_temp3257; struct Cyc_List_List* _temp3259; struct _tuple0 _temp3255= Cyc_List_rsplit(
rgn, rgn, _temp3159); _LL3260: _temp3259= _temp3255.f1; goto _LL3258; _LL3258:
_temp3257= _temp3255.f2; goto _LL3256; _LL3256: { struct Cyc_List_List*
_temp3261= Cyc_Tcutil_substs( rgn, inst, _temp3259); struct Cyc_List_List*
_temp3262= Cyc_Tcutil_substs( rgn, inst, _temp3257); if( _temp3261 ==  _temp3259?
_temp3262 ==  _temp3257: 0){ rgn_po2= _temp3159;} else{ rgn_po2= Cyc_List_zip(
_temp3261, _temp3262);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp3263=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp3263[ 0]=({ struct Cyc_Absyn_FnType_struct _temp3264; _temp3264.tag= Cyc_Absyn_FnType;
_temp3264.f1=({ struct Cyc_Absyn_FnInfo _temp3265; _temp3265.tvars= _temp3171;
_temp3265.effect= eff2; _temp3265.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp3167); _temp3265.args= _temp3240; _temp3265.c_varargs= _temp3163;
_temp3265.cyc_varargs= cyc_varargs2; _temp3265.rgn_po= rgn_po2; _temp3265.attributes=
_temp3157; _temp3265;}); _temp3264;}); _temp3263;});}}}}} _LL3072: { struct Cyc_List_List*
_temp3266=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp3173); struct Cyc_List_List* _temp3267= Cyc_Tcutil_substs( rgn, inst,
_temp3266); if( _temp3267 ==  _temp3266){ return t;}{ struct Cyc_List_List*
_temp3268=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp3173, _temp3267); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3269=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3269[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3270; _temp3270.tag= Cyc_Absyn_TupleType;
_temp3270.f1= _temp3268; _temp3270;}); _temp3269;});}} _LL3074: { struct Cyc_List_List*
_temp3271=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp3175); struct Cyc_List_List* _temp3272= Cyc_Tcutil_substs( rgn, inst,
_temp3271); if( _temp3272 ==  _temp3271){ return t;}{ struct Cyc_List_List*
_temp3273=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp3175, _temp3272); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3274=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3274[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3275; _temp3275.tag= Cyc_Absyn_AnonStructType;
_temp3275.f1= _temp3273; _temp3275;}); _temp3274;});}} _LL3076: { struct Cyc_List_List*
_temp3276=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp3177); struct Cyc_List_List* _temp3277= Cyc_Tcutil_substs( rgn, inst,
_temp3276); if( _temp3277 ==  _temp3276){ return t;}{ struct Cyc_List_List*
_temp3278=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp3177, _temp3277); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp3279=( struct Cyc_Absyn_AnonStructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp3279[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp3280; _temp3280.tag= Cyc_Absyn_AnonStructType;
_temp3280.f1= _temp3278; _temp3280;}); _temp3279;});}} _LL3078: if( _temp3179 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp3179->v);} else{
return t;} _LL3080: { void* _temp3281= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp3181); return _temp3281 ==  _temp3181? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3282=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3282[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3283; _temp3283.tag= Cyc_Absyn_RgnHandleType; _temp3283.f1=( void*)
_temp3281; _temp3283;}); _temp3282;});} _LL3082: { void* _temp3284= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3183); return _temp3284 ==  _temp3183? t:( void*)({ struct Cyc_Absyn_SizeofType_struct*
_temp3285=( struct Cyc_Absyn_SizeofType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct));
_temp3285[ 0]=({ struct Cyc_Absyn_SizeofType_struct _temp3286; _temp3286.tag=
Cyc_Absyn_SizeofType; _temp3286.f1=( void*) _temp3284; _temp3286;}); _temp3285;});}
_LL3084: return t; _LL3086: return t; _LL3088: return t; _LL3090: return t;
_LL3092: return t; _LL3094: return t; _LL3096: return t; _LL3098: { void*
_temp3287= Cyc_Tcutil_rsubstitute( rgn, inst, _temp3185); return _temp3287 == 
_temp3185? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp3288=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3288[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3289; _temp3289.tag= Cyc_Absyn_RgnsEff; _temp3289.f1=(
void*) _temp3287; _temp3289;}); _temp3288;});} _LL3100: { void* _temp3290= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3187); return _temp3290 ==  _temp3187? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3291=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3291[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3292; _temp3292.tag= Cyc_Absyn_AccessEff;
_temp3292.f1=( void*) _temp3290; _temp3292;}); _temp3291;});} _LL3102: { struct
Cyc_List_List* _temp3293= Cyc_Tcutil_substs( rgn, inst, _temp3189); return
_temp3293 ==  _temp3189? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3294=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3294[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3295; _temp3295.tag= Cyc_Absyn_JoinEff;
_temp3295.f1= _temp3293; _temp3295;}); _temp3294;});} _LL3052:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp3296=(
void*) ts->hd; struct Cyc_List_List* _temp3297= ts->tl; void* _temp3298= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp3296); struct Cyc_List_List* _temp3299= Cyc_Tcutil_substs( rgn,
inst, _temp3297); if( _temp3296 ==  _temp3298? _temp3297 ==  _temp3299: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp3300=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3300->hd=(
void*) _temp3298; _temp3300->tl= _temp3299; _temp3300;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp3345=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3345[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3346; _temp3346.tag= Cyc_Absyn_Const_e; _temp3346.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp3346;}); _temp3345;}), loc);{ void*
_temp3301= Cyc_Tcutil_compress( t); void* _temp3315; void* _temp3317; void*
_temp3319; void* _temp3321; _LL3303: if(( unsigned int) _temp3301 >  3u?*(( int*)
_temp3301) ==  Cyc_Absyn_PointerType: 0){ goto _LL3304;} else{ goto _LL3305;}
_LL3305: if(( unsigned int) _temp3301 >  3u?*(( int*) _temp3301) ==  Cyc_Absyn_IntType:
0){ _LL3318: _temp3317=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3301)->f1;
goto _LL3316; _LL3316: _temp3315=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3301)->f2; if( _temp3315 == ( void*) Cyc_Absyn_B1){ goto _LL3306;} else{
goto _LL3307;}} else{ goto _LL3307;} _LL3307: if(( unsigned int) _temp3301 >  3u?*((
int*) _temp3301) ==  Cyc_Absyn_IntType: 0){ _LL3322: _temp3321=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp3301)->f1; goto _LL3320; _LL3320: _temp3319=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3301)->f2; goto _LL3308;} else{
goto _LL3309;} _LL3309: if( _temp3301 == ( void*) Cyc_Absyn_FloatType){ goto
_LL3310;} else{ goto _LL3311;} _LL3311: if(( unsigned int) _temp3301 >  3u?*((
int*) _temp3301) ==  Cyc_Absyn_DoubleType: 0){ goto _LL3312;} else{ goto _LL3313;}
_LL3313: goto _LL3314; _LL3304: goto _LL3302; _LL3306:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp3323=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3323[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3324; _temp3324.tag= Cyc_Absyn_Const_e; _temp3324.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp3325=( struct Cyc_Absyn_Char_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp3325[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp3326; _temp3326.tag= Cyc_Absyn_Char_c; _temp3326.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3326.f2='\000'; _temp3326;}); _temp3325;})); _temp3324;}); _temp3323;})));
goto _LL3302; _LL3308:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp3327=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3327[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3328; _temp3328.tag= Cyc_Absyn_Const_e;
_temp3328.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp3329=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp3329[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp3330; _temp3330.tag= Cyc_Absyn_Int_c;
_temp3330.f1=( void*) _temp3321; _temp3330.f2= 0; _temp3330;}); _temp3329;}));
_temp3328;}); _temp3327;}))); if( _temp3319 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp3331=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp3331[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp3332; _temp3332.tag= Cyc_Absyn_Cast_e; _temp3332.f1=( void*) t; _temp3332.f2=
e; _temp3332;}); _temp3331;}), loc);} goto _LL3302; _LL3310:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3333=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp3333[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp3334; _temp3334.tag= Cyc_Absyn_Const_e; _temp3334.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3335=( struct Cyc_Absyn_Float_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp3335[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp3336; _temp3336.tag= Cyc_Absyn_Float_c; _temp3336.f1=
_tag_arr("0.0", sizeof( unsigned char), 4u); _temp3336;}); _temp3335;}));
_temp3334;}); _temp3333;}))); goto _LL3302; _LL3312:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp3337=( struct Cyc_Absyn_Cast_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp3337[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp3338; _temp3338.tag= Cyc_Absyn_Cast_e; _temp3338.f1=( void*) t; _temp3338.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp3339=( struct
Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp3339[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp3340; _temp3340.tag= Cyc_Absyn_Const_e;
_temp3340.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp3341=(
struct Cyc_Absyn_Float_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp3341[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp3342; _temp3342.tag= Cyc_Absyn_Float_c;
_temp3342.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp3342;});
_temp3341;})); _temp3340;}); _temp3339;}), loc); _temp3338;}); _temp3337;})));
goto _LL3302; _LL3314:({ struct Cyc_Std_String_pa_struct _temp3344; _temp3344.tag=
Cyc_Std_String_pa; _temp3344.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp3343[ 1u]={& _temp3344}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp3343, sizeof( void*), 1u));}});
goto _LL3302; _LL3302:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Tcutil_tvar_kind(
tv); return({ struct _tuple5* _temp3347=( struct _tuple5*) _cycalloc( sizeof(
struct _tuple5)); _temp3347->f1= tv; _temp3347->f2= Cyc_Absyn_new_evar(({ struct
Cyc_Core_Opt* _temp3348=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3348->v=( void*) k; _temp3348;}),({ struct Cyc_Core_Opt* _temp3349=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3349->v=( void*) s;
_temp3349;})); _temp3347;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp3352; struct
_RegionHandle* _temp3353; struct Cyc_List_List* _temp3355; struct _tuple6*
_temp3350= env; _temp3352=* _temp3350; _LL3356: _temp3355= _temp3352.f1; goto
_LL3354; _LL3354: _temp3353= _temp3352.f2; goto _LL3351; _LL3351: { void* k= Cyc_Tcutil_tvar_kind(
tv); return({ struct _tuple5* _temp3357=( struct _tuple5*) _region_malloc(
_temp3353, sizeof( struct _tuple5)); _temp3357->f1= tv; _temp3357->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3358=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3358->v=( void*) k; _temp3358;}),({ struct Cyc_Core_Opt*
_temp3359=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3359->v=( void*) _temp3355; _temp3359;})); _temp3357;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if( Cyc_Std_zstrptrcmp((( struct
Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) ==  0){ void* k1=( void*)(( struct
Cyc_Absyn_Tvar*) tvs2->hd)->kind; void* k2=( void*) tv->kind; if( ! Cyc_Tcutil_constrain_kinds(
k1, k2)){({ struct Cyc_Std_String_pa_struct _temp3363; _temp3363.tag= Cyc_Std_String_pa;
_temp3363.f1=( struct _tagged_arr) Cyc_Absynpp_kindbound2string( k2);{ struct
Cyc_Std_String_pa_struct _temp3362; _temp3362.tag= Cyc_Std_String_pa; _temp3362.f1=(
struct _tagged_arr) Cyc_Absynpp_kindbound2string( k1);{ struct Cyc_Std_String_pa_struct
_temp3361; _temp3361.tag= Cyc_Std_String_pa; _temp3361.f1=( struct _tagged_arr)*
tv->name;{ void* _temp3360[ 3u]={& _temp3361,& _temp3362,& _temp3363}; Cyc_Tcutil_terr(
loc, _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u), _tag_arr( _temp3360, sizeof( void*), 3u));}}}});}
if( tv->identity ==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) != *(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity))){({ void* _temp3364[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp3364, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp3365=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3365->hd=( void*) tv; _temp3365->tl= tvs; _temp3365;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp3366[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp3366, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ if(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity ==  0){({ void* _temp3367[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u),
_tag_arr( _temp3367, sizeof( void*), 0u));});} if(*(( int*) _check_null(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp3368=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3368->hd=( void*) tv; _temp3368->tl=
tvs; _temp3368;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_Std_String_pa_struct _temp3370; _temp3370.tag= Cyc_Std_String_pa;
_temp3370.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp3369[
1u]={& _temp3370};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp3369, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp3371=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3371->hd=(
void*) tv; _temp3371->tl= tvs; _temp3371;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp3372= Cyc_Tcutil_compress( e);
int _temp3378; _LL3374: if(( unsigned int) _temp3372 >  3u?*(( int*) _temp3372)
==  Cyc_Absyn_Evar: 0){ _LL3379: _temp3378=(( struct Cyc_Absyn_Evar_struct*)
_temp3372)->f3; goto _LL3375;} else{ goto _LL3376;} _LL3376: goto _LL3377;
_LL3375:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2= es2->tl){ void*
_temp3380= Cyc_Tcutil_compress(( void*) es2->hd); int _temp3386; _LL3382: if((
unsigned int) _temp3380 >  3u?*(( int*) _temp3380) ==  Cyc_Absyn_Evar: 0){
_LL3387: _temp3386=(( struct Cyc_Absyn_Evar_struct*) _temp3380)->f3; goto
_LL3383;} else{ goto _LL3384;} _LL3384: goto _LL3385; _LL3383: if( _temp3378 == 
_temp3386){ return es;} goto _LL3381; _LL3385: goto _LL3381; _LL3381:;}} return({
struct Cyc_List_List* _temp3388=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3388->hd=( void*) e; _temp3388->tl= es; _temp3388;});
_LL3377: return es; _LL3373:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs= tvs->tl){ int present= 0;{ struct Cyc_List_List* b=
btvs; for( 0; b !=  0; b= b->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)
tvs->hd)->identity)) == *(( int*) _check_null((( struct Cyc_Absyn_Tvar*) b->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp3389=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3389->hd=(
void*)(( struct Cyc_Absyn_Tvar*) tvs->hd); _temp3389->tl= r; _temp3389;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_Std_String_pa_struct
_temp3391; _temp3391.tag= Cyc_Std_String_pa; _temp3391.f1=( struct _tagged_arr)*
fn;{ void* _temp3390[ 1u]={& _temp3391}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp3390, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp3392= Cyc_Tcutil_compress( field_typ); void* _temp3398;
_LL3394: if(( unsigned int) _temp3392 >  3u?*(( int*) _temp3392) ==  Cyc_Absyn_IntType:
0){ _LL3399: _temp3398=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3392)->f2;
goto _LL3395;} else{ goto _LL3396;} _LL3396: goto _LL3397; _LL3395:{ void*
_temp3400= _temp3398; _LL3402: if( _temp3400 == ( void*) Cyc_Absyn_B1){ goto
_LL3403;} else{ goto _LL3404;} _LL3404: if( _temp3400 == ( void*) Cyc_Absyn_B2){
goto _LL3405;} else{ goto _LL3406;} _LL3406: if( _temp3400 == ( void*) Cyc_Absyn_B4){
goto _LL3407;} else{ goto _LL3408;} _LL3408: if( _temp3400 == ( void*) Cyc_Absyn_B8){
goto _LL3409;} else{ goto _LL3401;} _LL3403: if( w >  8){({ void* _temp3410[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp3410, sizeof( void*), 0u));});} goto
_LL3401; _LL3405: if( w >  16){({ void* _temp3411[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp3411, sizeof( void*), 0u));});} goto _LL3401; _LL3407: if( w >  32){({ void*
_temp3412[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp3412, sizeof( void*), 0u));});}
goto _LL3401; _LL3409: if( w >  64){({ void* _temp3413[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp3413, sizeof( void*), 0u));});} goto _LL3401; _LL3401:;} goto
_LL3393; _LL3397:({ struct Cyc_Std_String_pa_struct _temp3416; _temp3416.tag=
Cyc_Std_String_pa; _temp3416.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_Std_String_pa_struct _temp3415; _temp3415.tag= Cyc_Std_String_pa;
_temp3415.f1=( struct _tagged_arr)* fn;{ void* _temp3414[ 2u]={& _temp3415,&
_temp3416}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3414, sizeof( void*), 2u));}}});
goto _LL3393; _LL3393:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts= atts->tl){ void* _temp3417=( void*) atts->hd; _LL3419: if( _temp3417 == (
void*) Cyc_Absyn_Packed_att){ goto _LL3420;} else{ goto _LL3421;} _LL3421: if((
unsigned int) _temp3417 >  16u?*(( int*) _temp3417) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL3422;} else{ goto _LL3423;} _LL3423: goto _LL3424; _LL3420: continue;
_LL3422: continue; _LL3424:({ struct Cyc_Std_String_pa_struct _temp3427;
_temp3427.tag= Cyc_Std_String_pa; _temp3427.f1=( struct _tagged_arr)* fn;{
struct Cyc_Std_String_pa_struct _temp3426; _temp3426.tag= Cyc_Std_String_pa;
_temp3426.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp3425[ 2u]={& _temp3426,& _temp3427}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp3425, sizeof( void*), 2u));}}});
goto _LL3418; _LL3418:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; struct _tuple17{ void* f1; int f2; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp3428= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3480; struct Cyc_Core_Opt** _temp3482; struct Cyc_Core_Opt* _temp3483;
struct Cyc_Core_Opt** _temp3485; struct Cyc_Absyn_Tvar* _temp3486; struct Cyc_List_List*
_temp3488; struct Cyc_Absyn_Enumdecl* _temp3490; struct Cyc_Absyn_Enumdecl**
_temp3492; struct _tuple1* _temp3493; struct Cyc_Absyn_TunionInfo _temp3495;
void* _temp3497; struct Cyc_List_List* _temp3499; struct Cyc_List_List**
_temp3501; void* _temp3502; void** _temp3504; struct Cyc_Absyn_TunionFieldInfo
_temp3505; struct Cyc_List_List* _temp3507; void* _temp3509; void** _temp3511;
struct Cyc_Absyn_PtrInfo _temp3512; struct Cyc_Absyn_Conref* _temp3514; struct
Cyc_Absyn_Tqual _temp3516; struct Cyc_Absyn_Conref* _temp3518; void* _temp3520;
void* _temp3522; void* _temp3524; struct Cyc_Absyn_Exp* _temp3526; struct Cyc_Absyn_Tqual
_temp3528; void* _temp3530; struct Cyc_Absyn_FnInfo _temp3532; struct Cyc_List_List*
_temp3534; struct Cyc_List_List* _temp3536; struct Cyc_Absyn_VarargInfo*
_temp3538; int _temp3540; struct Cyc_List_List* _temp3542; void* _temp3544;
struct Cyc_Core_Opt* _temp3546; struct Cyc_Core_Opt** _temp3548; struct Cyc_List_List*
_temp3549; struct Cyc_List_List** _temp3551; struct Cyc_List_List* _temp3552;
struct Cyc_List_List* _temp3554; struct Cyc_List_List* _temp3556; struct Cyc_Absyn_Structdecl**
_temp3558; struct Cyc_Absyn_Structdecl*** _temp3560; struct Cyc_List_List*
_temp3561; struct Cyc_List_List** _temp3563; struct _tuple1* _temp3564; struct
Cyc_Absyn_Uniondecl** _temp3566; struct Cyc_Absyn_Uniondecl*** _temp3568; struct
Cyc_List_List* _temp3569; struct Cyc_List_List** _temp3571; struct _tuple1*
_temp3572; struct Cyc_Core_Opt* _temp3574; struct Cyc_Core_Opt** _temp3576;
struct Cyc_List_List* _temp3577; struct Cyc_List_List** _temp3579; struct
_tuple1* _temp3580; void* _temp3582; void* _temp3584; void* _temp3586; struct
Cyc_List_List* _temp3588; _LL3430: if( _temp3428 == ( void*) Cyc_Absyn_VoidType){
goto _LL3431;} else{ goto _LL3432;} _LL3432: if(( unsigned int) _temp3428 >  3u?*((
int*) _temp3428) ==  Cyc_Absyn_Evar: 0){ _LL3484: _temp3483=(( struct Cyc_Absyn_Evar_struct*)
_temp3428)->f1; _temp3485=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp3428)->f1; goto _LL3481; _LL3481: _temp3480=(( struct Cyc_Absyn_Evar_struct*)
_temp3428)->f2; _temp3482=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp3428)->f2; goto _LL3433;} else{ goto _LL3434;} _LL3434: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_VarType: 0){ _LL3487:
_temp3486=(( struct Cyc_Absyn_VarType_struct*) _temp3428)->f1; goto _LL3435;}
else{ goto _LL3436;} _LL3436: if(( unsigned int) _temp3428 >  3u?*(( int*)
_temp3428) ==  Cyc_Absyn_AnonEnumType: 0){ _LL3489: _temp3488=(( struct Cyc_Absyn_AnonEnumType_struct*)
_temp3428)->f1; goto _LL3437;} else{ goto _LL3438;} _LL3438: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_EnumType: 0){ _LL3494:
_temp3493=(( struct Cyc_Absyn_EnumType_struct*) _temp3428)->f1; goto _LL3491;
_LL3491: _temp3490=(( struct Cyc_Absyn_EnumType_struct*) _temp3428)->f2;
_temp3492=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp3428)->f2; goto _LL3439;} else{ goto _LL3440;} _LL3440: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_TunionType: 0){ _LL3496:
_temp3495=(( struct Cyc_Absyn_TunionType_struct*) _temp3428)->f1; _LL3503:
_temp3502=( void*) _temp3495.tunion_info; _temp3504=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp3428)->f1).tunion_info; goto _LL3500; _LL3500: _temp3499= _temp3495.targs;
_temp3501=( struct Cyc_List_List**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp3428)->f1).targs; goto _LL3498; _LL3498: _temp3497=( void*) _temp3495.rgn;
goto _LL3441;} else{ goto _LL3442;} _LL3442: if(( unsigned int) _temp3428 >  3u?*((
int*) _temp3428) ==  Cyc_Absyn_TunionFieldType: 0){ _LL3506: _temp3505=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3428)->f1; _LL3510: _temp3509=( void*)
_temp3505.field_info; _temp3511=( void**)&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp3428)->f1).field_info; goto _LL3508; _LL3508: _temp3507= _temp3505.targs;
goto _LL3443;} else{ goto _LL3444;} _LL3444: if(( unsigned int) _temp3428 >  3u?*((
int*) _temp3428) ==  Cyc_Absyn_PointerType: 0){ _LL3513: _temp3512=(( struct Cyc_Absyn_PointerType_struct*)
_temp3428)->f1; _LL3523: _temp3522=( void*) _temp3512.elt_typ; goto _LL3521;
_LL3521: _temp3520=( void*) _temp3512.rgn_typ; goto _LL3519; _LL3519: _temp3518=
_temp3512.nullable; goto _LL3517; _LL3517: _temp3516= _temp3512.tq; goto _LL3515;
_LL3515: _temp3514= _temp3512.bounds; goto _LL3445;} else{ goto _LL3446;}
_LL3446: if(( unsigned int) _temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_SizeofType:
0){ _LL3525: _temp3524=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp3428)->f1;
goto _LL3447;} else{ goto _LL3448;} _LL3448: if(( unsigned int) _temp3428 >  3u?*((
int*) _temp3428) ==  Cyc_Absyn_IntType: 0){ goto _LL3449;} else{ goto _LL3450;}
_LL3450: if( _temp3428 == ( void*) Cyc_Absyn_FloatType){ goto _LL3451;} else{
goto _LL3452;} _LL3452: if(( unsigned int) _temp3428 >  3u?*(( int*) _temp3428)
==  Cyc_Absyn_DoubleType: 0){ goto _LL3453;} else{ goto _LL3454;} _LL3454: if((
unsigned int) _temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_ArrayType: 0){
_LL3531: _temp3530=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3428)->f1;
goto _LL3529; _LL3529: _temp3528=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3428)->f2; goto _LL3527; _LL3527: _temp3526=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3428)->f3; goto _LL3455;} else{ goto _LL3456;} _LL3456: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_FnType: 0){ _LL3533:
_temp3532=(( struct Cyc_Absyn_FnType_struct*) _temp3428)->f1; _LL3550: _temp3549=
_temp3532.tvars; _temp3551=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3428)->f1).tvars; goto _LL3547; _LL3547: _temp3546= _temp3532.effect;
_temp3548=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3428)->f1).effect; goto _LL3545; _LL3545: _temp3544=( void*) _temp3532.ret_typ;
goto _LL3543; _LL3543: _temp3542= _temp3532.args; goto _LL3541; _LL3541:
_temp3540= _temp3532.c_varargs; goto _LL3539; _LL3539: _temp3538= _temp3532.cyc_varargs;
goto _LL3537; _LL3537: _temp3536= _temp3532.rgn_po; goto _LL3535; _LL3535:
_temp3534= _temp3532.attributes; goto _LL3457;} else{ goto _LL3458;} _LL3458:
if(( unsigned int) _temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_TupleType:
0){ _LL3553: _temp3552=(( struct Cyc_Absyn_TupleType_struct*) _temp3428)->f1;
goto _LL3459;} else{ goto _LL3460;} _LL3460: if(( unsigned int) _temp3428 >  3u?*((
int*) _temp3428) ==  Cyc_Absyn_AnonStructType: 0){ _LL3555: _temp3554=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3428)->f1; goto _LL3461;} else{ goto
_LL3462;} _LL3462: if(( unsigned int) _temp3428 >  3u?*(( int*) _temp3428) == 
Cyc_Absyn_AnonUnionType: 0){ _LL3557: _temp3556=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3428)->f1; goto _LL3463;} else{ goto _LL3464;} _LL3464: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_StructType: 0){ _LL3565:
_temp3564=(( struct Cyc_Absyn_StructType_struct*) _temp3428)->f1; goto _LL3562;
_LL3562: _temp3561=(( struct Cyc_Absyn_StructType_struct*) _temp3428)->f2;
_temp3563=( struct Cyc_List_List**)&(( struct Cyc_Absyn_StructType_struct*)
_temp3428)->f2; goto _LL3559; _LL3559: _temp3558=(( struct Cyc_Absyn_StructType_struct*)
_temp3428)->f3; _temp3560=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp3428)->f3; goto _LL3465;} else{ goto _LL3466;} _LL3466: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_UnionType: 0){ _LL3573:
_temp3572=(( struct Cyc_Absyn_UnionType_struct*) _temp3428)->f1; goto _LL3570;
_LL3570: _temp3569=(( struct Cyc_Absyn_UnionType_struct*) _temp3428)->f2;
_temp3571=( struct Cyc_List_List**)&(( struct Cyc_Absyn_UnionType_struct*)
_temp3428)->f2; goto _LL3567; _LL3567: _temp3566=(( struct Cyc_Absyn_UnionType_struct*)
_temp3428)->f3; _temp3568=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp3428)->f3; goto _LL3467;} else{ goto _LL3468;} _LL3468: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_TypedefType: 0){ _LL3581:
_temp3580=(( struct Cyc_Absyn_TypedefType_struct*) _temp3428)->f1; goto _LL3578;
_LL3578: _temp3577=(( struct Cyc_Absyn_TypedefType_struct*) _temp3428)->f2;
_temp3579=( struct Cyc_List_List**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3428)->f2; goto _LL3575; _LL3575: _temp3574=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3428)->f3; _temp3576=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp3428)->f3; goto _LL3469;} else{ goto _LL3470;} _LL3470: if( _temp3428 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL3471;} else{ goto _LL3472;} _LL3472: if((
unsigned int) _temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_RgnHandleType:
0){ _LL3583: _temp3582=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp3428)->f1; goto _LL3473;} else{ goto _LL3474;} _LL3474: if(( unsigned int)
_temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_AccessEff: 0){ _LL3585:
_temp3584=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp3428)->f1; goto
_LL3475;} else{ goto _LL3476;} _LL3476: if(( unsigned int) _temp3428 >  3u?*((
int*) _temp3428) ==  Cyc_Absyn_RgnsEff: 0){ _LL3587: _temp3586=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp3428)->f1; goto _LL3477;} else{ goto _LL3478;}
_LL3478: if(( unsigned int) _temp3428 >  3u?*(( int*) _temp3428) ==  Cyc_Absyn_JoinEff:
0){ _LL3589: _temp3588=(( struct Cyc_Absyn_JoinEff_struct*) _temp3428)->f1; goto
_LL3479;} else{ goto _LL3429;} _LL3431: goto _LL3429; _LL3433: if(* _temp3485 == 
0){* _temp3485=({ struct Cyc_Core_Opt* _temp3590=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3590->v=( void*) expected_kind;
_temp3590;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp3482=({ struct Cyc_Core_Opt* _temp3591=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3591->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp3591;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp3592= Cyc_Tcutil_new_tvar(( void*)({ struct Cyc_Absyn_Less_kb_struct*
_temp3596=( struct Cyc_Absyn_Less_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct));
_temp3596[ 0]=({ struct Cyc_Absyn_Less_kb_struct _temp3597; _temp3597.tag= Cyc_Absyn_Less_kb;
_temp3597.f1= 0; _temp3597.f2=( void*) expected_kind; _temp3597;}); _temp3596;}));*
_temp3482=({ struct Cyc_Core_Opt* _temp3593=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp3593->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3594=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3594[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3595; _temp3595.tag= Cyc_Absyn_VarType;
_temp3595.f1= _temp3592; _temp3595;}); _temp3594;})); _temp3593;}); _temp3486=
_temp3592; goto _LL3435;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL3429; _LL3435:{ void* _temp3598= Cyc_Absyn_compress_kb((
void*) _temp3486->kind); struct Cyc_Core_Opt* _temp3604; struct Cyc_Core_Opt**
_temp3606; _LL3600: if(*(( int*) _temp3598) ==  Cyc_Absyn_Unknown_kb){ _LL3605:
_temp3604=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp3598)->f1; _temp3606=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*) _temp3598)->f1;
goto _LL3601;} else{ goto _LL3602;} _LL3602: goto _LL3603; _LL3601:* _temp3606=({
struct Cyc_Core_Opt* _temp3607=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3607->v=( void*)(( void*)({ struct Cyc_Absyn_Less_kb_struct*
_temp3608=( struct Cyc_Absyn_Less_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct));
_temp3608[ 0]=({ struct Cyc_Absyn_Less_kb_struct _temp3609; _temp3609.tag= Cyc_Absyn_Less_kb;
_temp3609.f1= 0; _temp3609.f2=( void*) expected_kind; _temp3609;}); _temp3608;}));
_temp3607;}); goto _LL3599; _LL3603: goto _LL3599; _LL3599:;} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp3486); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp3486); goto _LL3429;
_LL3437: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);{ struct
_RegionHandle _temp3610= _new_region("uprev_rgn"); struct _RegionHandle*
uprev_rgn=& _temp3610; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0; for( 0; _temp3488 !=  0; _temp3488=
_temp3488->tl){ struct Cyc_Absyn_Enumfield* _temp3611=( struct Cyc_Absyn_Enumfield*)
_temp3488->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_zstrptrcmp, prev_fields,(* _temp3611->name).f2)){({ struct Cyc_Std_String_pa_struct
_temp3613; _temp3613.tag= Cyc_Std_String_pa; _temp3613.f1=( struct _tagged_arr)*(*
_temp3611->name).f2;{ void* _temp3612[ 1u]={& _temp3613}; Cyc_Tcutil_terr(
_temp3611->loc, _tag_arr("duplicate enum field name %s", sizeof( unsigned char),
29u), _tag_arr( _temp3612, sizeof( void*), 1u));}});} else{ prev_fields=({
struct Cyc_List_List* _temp3614=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp3614->hd=( void*)(* _temp3611->name).f2;
_temp3614->tl= prev_fields; _temp3614;});} if( _temp3611->tag ==  0){ _temp3611->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp3611->loc);} else{
if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null(
_temp3611->tag))){({ struct Cyc_Std_String_pa_struct _temp3616; _temp3616.tag=
Cyc_Std_String_pa; _temp3616.f1=( struct _tagged_arr)*(* _temp3611->name).f2;{
void* _temp3615[ 1u]={& _temp3616}; Cyc_Tcutil_terr( loc, _tag_arr("enum field %s: expression is not constant",
sizeof( unsigned char), 42u), _tag_arr( _temp3615, sizeof( void*), 1u));}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp3611->tag)); tag_count= t1 +  1;(* _temp3611->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp3617=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp3617[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp3618; _temp3618.tag= Cyc_Absyn_Abs_n; _temp3618.f1= te->ns; _temp3618;});
_temp3617;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple17* v)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp3611->name).f2,({ struct _tuple17* _temp3619=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp3619->f1=( void*)({ struct Cyc_Tcenv_AnonEnumRes_struct*
_temp3620=( struct Cyc_Tcenv_AnonEnumRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_AnonEnumRes_struct));
_temp3620[ 0]=({ struct Cyc_Tcenv_AnonEnumRes_struct _temp3621; _temp3621.tag=
Cyc_Tcenv_AnonEnumRes; _temp3621.f1=( void*) t; _temp3621.f2= _temp3611;
_temp3621;}); _temp3620;}); _temp3619->f2= 1; _temp3619;}));}}}; _pop_region(
uprev_rgn);} goto _LL3429;} _LL3439: if(* _temp3492 ==  0){ struct _handler_cons
_temp3622; _push_handler(& _temp3622);{ int _temp3624= 0; if( setjmp( _temp3622.handler)){
_temp3624= 1;} if( ! _temp3624){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3493);* _temp3492=( struct Cyc_Absyn_Enumdecl*)* ed;};
_pop_handler();} else{ void* _temp3623=( void*) _exn_thrown; void* _temp3626=
_temp3623; _LL3628: if( _temp3626 ==  Cyc_Dict_Absent){ goto _LL3629;} else{
goto _LL3630;} _LL3630: goto _LL3631; _LL3629: { struct Cyc_Tcenv_Genv*
_temp3632=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Enumdecl* _temp3633=({
struct Cyc_Absyn_Enumdecl* _temp3634=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp3634->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp3634->name= _temp3493; _temp3634->fields= 0; _temp3634;}); Cyc_Tc_tcEnumdecl(
te, _temp3632, loc, _temp3633);{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl(
te, loc, _temp3493);* _temp3492=( struct Cyc_Absyn_Enumdecl*)* ed; goto _LL3627;}}
_LL3631:( void) _throw( _temp3626); _LL3627:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp3492);* _temp3493=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL3429;} _LL3441: { struct Cyc_List_List*
_temp3635=* _temp3501;{ void* _temp3636=* _temp3504; struct Cyc_Absyn_UnknownTunionInfo
_temp3642; int _temp3644; struct _tuple1* _temp3646; struct Cyc_Absyn_Tuniondecl**
_temp3648; struct Cyc_Absyn_Tuniondecl* _temp3650; _LL3638: if(*(( int*)
_temp3636) ==  Cyc_Absyn_UnknownTunion){ _LL3643: _temp3642=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp3636)->f1; _LL3647: _temp3646= _temp3642.name; goto _LL3645; _LL3645:
_temp3644= _temp3642.is_xtunion; goto _LL3639;} else{ goto _LL3640;} _LL3640:
if(*(( int*) _temp3636) ==  Cyc_Absyn_KnownTunion){ _LL3649: _temp3648=(( struct
Cyc_Absyn_KnownTunion_struct*) _temp3636)->f1; _temp3650=* _temp3648; goto
_LL3641;} else{ goto _LL3637;} _LL3639: { struct Cyc_Absyn_Tuniondecl** tudp;{
struct _handler_cons _temp3651; _push_handler(& _temp3651);{ int _temp3653= 0;
if( setjmp( _temp3651.handler)){ _temp3653= 1;} if( ! _temp3653){ tudp= Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp3646);; _pop_handler();} else{ void* _temp3652=( void*)
_exn_thrown; void* _temp3655= _temp3652; _LL3657: if( _temp3655 ==  Cyc_Dict_Absent){
goto _LL3658;} else{ goto _LL3659;} _LL3659: goto _LL3660; _LL3658: { struct Cyc_Tcenv_Genv*
_temp3661=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Tuniondecl* _temp3662=({
struct Cyc_Absyn_Tuniondecl* _temp3666=( struct Cyc_Absyn_Tuniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp3666->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp3666->name= _temp3646; _temp3666->tvs= 0; _temp3666->fields= 0; _temp3666->is_xtunion=
_temp3644; _temp3666;}); Cyc_Tc_tcTuniondecl( te, _temp3661, loc, _temp3662);
tudp= Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp3646); if( _temp3635 !=  0){({
struct Cyc_Std_String_pa_struct _temp3665; _temp3665.tag= Cyc_Std_String_pa;
_temp3665.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3646);{ struct
Cyc_Std_String_pa_struct _temp3664; _temp3664.tag= Cyc_Std_String_pa; _temp3664.f1=(
struct _tagged_arr)( _temp3644? _tag_arr("xtunion", sizeof( unsigned char), 8u):
_tag_arr("tunion", sizeof( unsigned char), 7u));{ void* _temp3663[ 2u]={&
_temp3664,& _temp3665}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized %s %s before using",
sizeof( unsigned char), 48u), _tag_arr( _temp3663, sizeof( void*), 2u));}}});
return cvtenv;} goto _LL3656;} _LL3660:( void) _throw( _temp3655); _LL3656:;}}}
if((* tudp)->is_xtunion !=  _temp3644){({ struct Cyc_Std_String_pa_struct
_temp3668; _temp3668.tag= Cyc_Std_String_pa; _temp3668.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3646);{ void* _temp3667[ 1u]={& _temp3668}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp3667, sizeof( void*), 1u));}});}* _temp3504=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp3669=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp3669[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp3670; _temp3670.tag= Cyc_Absyn_KnownTunion;
_temp3670.f1= tudp; _temp3670;}); _temp3669;}); _temp3650=* tudp; goto _LL3641;}
_LL3641: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3497);{ struct Cyc_List_List* tvs= _temp3650->tvs; for( 0; _temp3635 !=  0?
tvs !=  0: 0;( _temp3635= _temp3635->tl, tvs= tvs->tl)){ void* t1=( void*)
_temp3635->hd; void* k1= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) tvs->hd);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp3635
!=  0){({ struct Cyc_Std_String_pa_struct _temp3672; _temp3672.tag= Cyc_Std_String_pa;
_temp3672.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3650->name);{
void* _temp3671[ 1u]={& _temp3672}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp3671, sizeof( void*), 1u));}});}
if( tvs !=  0){ struct Cyc_List_List* hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){
void* k1= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) tvs->hd); void* e= Cyc_Absyn_new_evar(
0, 0); hidden_ts=({ struct Cyc_List_List* _temp3673=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3673->hd=( void*) e; _temp3673->tl=
hidden_ts; _temp3673;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,
k1, e);}* _temp3501= Cyc_List_imp_append(* _temp3501, Cyc_List_imp_rev(
hidden_ts));} goto _LL3637;} _LL3637:;} goto _LL3429;} _LL3443:{ void* _temp3674=*
_temp3511; struct Cyc_Absyn_UnknownTunionFieldInfo _temp3680; int _temp3682;
struct _tuple1* _temp3684; struct _tuple1* _temp3686; struct Cyc_Absyn_Tunionfield*
_temp3688; struct Cyc_Absyn_Tuniondecl* _temp3690; _LL3676: if(*(( int*)
_temp3674) ==  Cyc_Absyn_UnknownTunionfield){ _LL3681: _temp3680=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp3674)->f1; _LL3687: _temp3686= _temp3680.tunion_name; goto _LL3685; _LL3685:
_temp3684= _temp3680.field_name; goto _LL3683; _LL3683: _temp3682= _temp3680.is_xtunion;
goto _LL3677;} else{ goto _LL3678;} _LL3678: if(*(( int*) _temp3674) ==  Cyc_Absyn_KnownTunionfield){
_LL3691: _temp3690=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp3674)->f1;
goto _LL3689; _LL3689: _temp3688=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp3674)->f2; goto _LL3679;} else{ goto _LL3675;} _LL3677: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp3692;
_push_handler(& _temp3692);{ int _temp3694= 0; if( setjmp( _temp3692.handler)){
_temp3694= 1;} if( ! _temp3694){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp3686);; _pop_handler();} else{ void* _temp3693=( void*) _exn_thrown; void*
_temp3696= _temp3693; _LL3698: if( _temp3696 ==  Cyc_Dict_Absent){ goto _LL3699;}
else{ goto _LL3700;} _LL3700: goto _LL3701; _LL3699:({ struct Cyc_Std_String_pa_struct
_temp3703; _temp3703.tag= Cyc_Std_String_pa; _temp3703.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3686);{ void* _temp3702[ 1u]={& _temp3703}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp3702, sizeof( void*), 1u));}}); return cvtenv; _LL3701:( void) _throw(
_temp3696); _LL3697:;}}}{ struct _handler_cons _temp3704; _push_handler(&
_temp3704);{ int _temp3706= 0; if( setjmp( _temp3704.handler)){ _temp3706= 1;}
if( ! _temp3706){{ void* _temp3707= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp3684); struct Cyc_Absyn_Tunionfield* _temp3713; struct Cyc_Absyn_Tuniondecl*
_temp3715; _LL3709: if(*(( int*) _temp3707) ==  Cyc_Tcenv_TunionRes){ _LL3716:
_temp3715=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3707)->f1; goto _LL3714;
_LL3714: _temp3713=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3707)->f2; goto
_LL3710;} else{ goto _LL3711;} _LL3711: goto _LL3712; _LL3710: tuf= _temp3713;
tud= _temp3715; if( tud->is_xtunion !=  _temp3682){({ struct Cyc_Std_String_pa_struct
_temp3718; _temp3718.tag= Cyc_Std_String_pa; _temp3718.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3686);{ void* _temp3717[ 1u]={& _temp3718}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp3717, sizeof( void*), 1u));}});} goto
_LL3708; _LL3712:({ struct Cyc_Std_String_pa_struct _temp3721; _temp3721.tag=
Cyc_Std_String_pa; _temp3721.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3686);{ struct Cyc_Std_String_pa_struct _temp3720; _temp3720.tag= Cyc_Std_String_pa;
_temp3720.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3684);{ void*
_temp3719[ 2u]={& _temp3720,& _temp3721}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp3719, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp3722= cvtenv; _npop_handler( 0u); return _temp3722;}
_LL3708:;}; _pop_handler();} else{ void* _temp3705=( void*) _exn_thrown; void*
_temp3724= _temp3705; _LL3726: if( _temp3724 ==  Cyc_Dict_Absent){ goto _LL3727;}
else{ goto _LL3728;} _LL3728: goto _LL3729; _LL3727:({ struct Cyc_Std_String_pa_struct
_temp3732; _temp3732.tag= Cyc_Std_String_pa; _temp3732.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3686);{ struct Cyc_Std_String_pa_struct _temp3731;
_temp3731.tag= Cyc_Std_String_pa; _temp3731.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3684);{ void* _temp3730[ 2u]={& _temp3731,& _temp3732}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3730, sizeof( void*), 2u));}}}); return cvtenv; _LL3729:( void)
_throw( _temp3724); _LL3725:;}}}* _temp3511=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp3733=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp3733[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp3734; _temp3734.tag= Cyc_Absyn_KnownTunionfield; _temp3734.f1= tud;
_temp3734.f2= tuf; _temp3734;}); _temp3733;}); _temp3690= tud; _temp3688= tuf;
goto _LL3679;} _LL3679: { struct Cyc_List_List* tvs= _temp3690->tvs; for( 0;
_temp3507 !=  0? tvs !=  0: 0;( _temp3507= _temp3507->tl, tvs= tvs->tl)){ void*
t1=( void*) _temp3507->hd; void* k1= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*)
tvs->hd); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if(
_temp3507 !=  0){({ struct Cyc_Std_String_pa_struct _temp3737; _temp3737.tag=
Cyc_Std_String_pa; _temp3737.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp3688->name);{ struct Cyc_Std_String_pa_struct _temp3736; _temp3736.tag= Cyc_Std_String_pa;
_temp3736.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3690->name);{
void* _temp3735[ 2u]={& _temp3736,& _temp3737}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp3735, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_Std_String_pa_struct _temp3740; _temp3740.tag= Cyc_Std_String_pa;
_temp3740.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3688->name);{
struct Cyc_Std_String_pa_struct _temp3739; _temp3739.tag= Cyc_Std_String_pa;
_temp3739.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp3690->name);{
void* _temp3738[ 2u]={& _temp3739,& _temp3740}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp3738, sizeof( void*), 2u));}}});}
goto _LL3675;} _LL3675:;} goto _LL3429; _LL3445: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3522); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp3520);{ void* _temp3741=( void*)(
Cyc_Absyn_compress_conref( _temp3514))->v; void* _temp3747; struct Cyc_Absyn_Exp*
_temp3749; _LL3743: if(( unsigned int) _temp3741 >  1u?*(( int*) _temp3741) == 
Cyc_Absyn_Eq_constr: 0){ _LL3748: _temp3747=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3741)->f1; if(( unsigned int) _temp3747 >  1u?*(( int*) _temp3747) ==  Cyc_Absyn_Upper_b:
0){ _LL3750: _temp3749=(( struct Cyc_Absyn_Upper_b_struct*) _temp3747)->f1; goto
_LL3744;} else{ goto _LL3745;}} else{ goto _LL3745;} _LL3745: goto _LL3746;
_LL3744: if( ! Cyc_Tcutil_is_const_exp( te, _temp3749)){({ void* _temp3751[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp3751, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp3749); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp3749)){({ void*
_temp3752[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp3752, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp3749); goto _LL3742; _LL3746: goto _LL3742; _LL3742:;} goto _LL3429;
_LL3447: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp3524); goto _LL3429; _LL3449: goto _LL3429; _LL3451: goto _LL3429; _LL3453:
goto _LL3429; _LL3455: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,(
void*) Cyc_Absyn_MemKind, _temp3530); if( _temp3526 ==  0? 1: ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( _temp3526))){({ void* _temp3753[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not constant",
sizeof( unsigned char), 40u), _tag_arr( _temp3753, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) _check_null( _temp3526)); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*) _check_null( _temp3526))){({ void* _temp3754[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not an unsigned int",
sizeof( unsigned char), 47u), _tag_arr( _temp3754, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp3526)); goto _LL3429; _LL3457: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp3534 !=  0; _temp3534= _temp3534->tl){ if(
! Cyc_Absyn_fntype_att(( void*) _temp3534->hd)){({ struct Cyc_Std_String_pa_struct
_temp3756; _temp3756.tag= Cyc_Std_String_pa; _temp3756.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*) _temp3534->hd);{ void* _temp3755[ 1u]={&
_temp3756}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp3755, sizeof( void*), 1u));}});}{
void* _temp3757=( void*) _temp3534->hd; int _temp3769; int _temp3771; void*
_temp3773; _LL3759: if( _temp3757 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL3760;} else{ goto _LL3761;} _LL3761: if( _temp3757 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL3762;} else{ goto _LL3763;} _LL3763: if( _temp3757 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL3764;} else{ goto _LL3765;} _LL3765: if(( unsigned int) _temp3757 >  16u?*((
int*) _temp3757) ==  Cyc_Absyn_Format_att: 0){ _LL3774: _temp3773=( void*)((
struct Cyc_Absyn_Format_att_struct*) _temp3757)->f1; goto _LL3772; _LL3772:
_temp3771=(( struct Cyc_Absyn_Format_att_struct*) _temp3757)->f2; goto _LL3770;
_LL3770: _temp3769=(( struct Cyc_Absyn_Format_att_struct*) _temp3757)->f3; goto
_LL3766;} else{ goto _LL3767;} _LL3767: goto _LL3768; _LL3760: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL3758; _LL3762: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL3758; _LL3764: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL3758; _LL3766: if( !
seen_format){ seen_format= 1; ft= _temp3773; fmt_desc_arg= _temp3771;
fmt_arg_start= _temp3769;} else{({ void* _temp3775[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp3775, sizeof( void*), 0u));});} goto _LL3758; _LL3768: goto
_LL3758; _LL3758:;}} if( num_convs >  1){({ void* _temp3776[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp3776, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp3551);{ struct Cyc_List_List* b=* _temp3551; for( 0; b !=  0; b= b->tl){((
struct Cyc_Absyn_Tvar*) b->hd)->identity= Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env=
Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*) b->hd);{
void* _temp3777= Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*) b->hd)->kind);
void* _temp3783; _LL3779: if(*(( int*) _temp3777) ==  Cyc_Absyn_Eq_kb){ _LL3784:
_temp3783=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp3777)->f1; if(
_temp3783 == ( void*) Cyc_Absyn_MemKind){ goto _LL3780;} else{ goto _LL3781;}}
else{ goto _LL3781;} _LL3781: goto _LL3782; _LL3780:({ struct Cyc_Std_String_pa_struct
_temp3786; _temp3786.tag= Cyc_Std_String_pa; _temp3786.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*) b->hd)->name;{ void* _temp3785[ 1u]={& _temp3786}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp3785, sizeof( void*), 1u));}}); goto
_LL3778; _LL3782: goto _LL3778; _LL3778:;}}}{ struct Cyc_Tcutil_CVTEnv _temp3787=({
struct Cyc_Tcutil_CVTEnv _temp4002; _temp4002.kind_env= cvtenv.kind_env;
_temp4002.free_vars= 0; _temp4002.free_evars= 0; _temp4002.generalize_evars=
cvtenv.generalize_evars; _temp4002.fn_result= 1; _temp4002;}); _temp3787= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3787,( void*) Cyc_Absyn_MemKind, _temp3544); _temp3787.fn_result=
0;{ struct Cyc_List_List* a= _temp3542; for( 0; a !=  0; a= a->tl){ _temp3787=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3787,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*) a->hd)).f3);}} if( _temp3538 !=  0){ if( _temp3540){({ void*
_temp3788[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp3788, sizeof( void*), 0u));});}{ int _temp3791; void* _temp3793; struct Cyc_Absyn_Tqual
_temp3795; struct Cyc_Core_Opt* _temp3797; struct Cyc_Absyn_VarargInfo _temp3789=*
_temp3538; _LL3798: _temp3797= _temp3789.name; goto _LL3796; _LL3796: _temp3795=
_temp3789.tq; goto _LL3794; _LL3794: _temp3793=( void*) _temp3789.type; goto
_LL3792; _LL3792: _temp3791= _temp3789.inject; goto _LL3790; _LL3790: _temp3787=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3787,( void*) Cyc_Absyn_MemKind,
_temp3793); if( _temp3791){ void* _temp3799= Cyc_Tcutil_compress( _temp3793);
struct Cyc_Absyn_TunionInfo _temp3805; void* _temp3807; void* _temp3809; _LL3801:
if(( unsigned int) _temp3799 >  3u?*(( int*) _temp3799) ==  Cyc_Absyn_TunionType:
0){ _LL3806: _temp3805=(( struct Cyc_Absyn_TunionType_struct*) _temp3799)->f1;
_LL3810: _temp3809=( void*) _temp3805.tunion_info; if(*(( int*) _temp3809) == 
Cyc_Absyn_KnownTunion){ goto _LL3808;} else{ goto _LL3803;} _LL3808: _temp3807=(
void*) _temp3805.rgn; goto _LL3802;} else{ goto _LL3803;} _LL3803: goto _LL3804;
_LL3802: goto _LL3800; _LL3804:({ void* _temp3811[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("can't inject a non-[x]tunion type", sizeof( unsigned char), 34u),
_tag_arr( _temp3811, sizeof( void*), 0u));}); goto _LL3800; _LL3800:;}}} if(
seen_format){ int _temp3812=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp3542); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg >  _temp3812)? 1:
fmt_arg_start <  0)? 1:( _temp3538 ==  0? fmt_arg_start !=  0: 0))? 1:(
_temp3538 !=  0? fmt_arg_start !=  _temp3812 +  1: 0)){({ void* _temp3813[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp3813, sizeof( void*), 0u));});} else{ void* _temp3816;
struct _tuple2 _temp3814=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int n))
Cyc_List_nth)( _temp3542, fmt_desc_arg -  1); _LL3817: _temp3816= _temp3814.f3;
goto _LL3815; _LL3815:{ void* _temp3818= Cyc_Tcutil_compress( _temp3816); struct
Cyc_Absyn_PtrInfo _temp3824; struct Cyc_Absyn_Conref* _temp3826; void* _temp3828;
_LL3820: if(( unsigned int) _temp3818 >  3u?*(( int*) _temp3818) ==  Cyc_Absyn_PointerType:
0){ _LL3825: _temp3824=(( struct Cyc_Absyn_PointerType_struct*) _temp3818)->f1;
_LL3829: _temp3828=( void*) _temp3824.elt_typ; goto _LL3827; _LL3827: _temp3826=
_temp3824.bounds; goto _LL3821;} else{ goto _LL3822;} _LL3822: goto _LL3823;
_LL3821:{ struct _tuple8 _temp3831=({ struct _tuple8 _temp3830; _temp3830.f1=
Cyc_Tcutil_compress( _temp3828); _temp3830.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp3826); _temp3830;}); void* _temp3837; void* _temp3839; void* _temp3841;
void* _temp3843; _LL3833: _LL3840: _temp3839= _temp3831.f1; if(( unsigned int)
_temp3839 >  3u?*(( int*) _temp3839) ==  Cyc_Absyn_IntType: 0){ _LL3844:
_temp3843=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3839)->f1; if(
_temp3843 == ( void*) Cyc_Absyn_Unsigned){ goto _LL3842;} else{ goto _LL3835;}
_LL3842: _temp3841=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3839)->f2;
if( _temp3841 == ( void*) Cyc_Absyn_B1){ goto _LL3838;} else{ goto _LL3835;}}
else{ goto _LL3835;} _LL3838: _temp3837= _temp3831.f2; if( _temp3837 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL3834;} else{ goto _LL3835;} _LL3835: goto _LL3836;
_LL3834: goto _LL3832; _LL3836:({ void* _temp3845[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp3845, sizeof( void*), 0u));}); goto _LL3832; _LL3832:;} goto
_LL3819; _LL3823:({ void* _temp3846[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp3846, sizeof( void*), 0u));}); goto
_LL3819; _LL3819:;} if( fmt_arg_start !=  0){ void* _temp3847= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp3538))->type); int
problem;{ void* _temp3848= ft; _LL3850: if( _temp3848 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL3851;} else{ goto _LL3852;} _LL3852: if( _temp3848 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL3853;} else{ goto _LL3849;} _LL3851:{ void* _temp3854= _temp3847; struct
Cyc_Absyn_TunionInfo _temp3860; void* _temp3862; struct Cyc_Absyn_Tuniondecl**
_temp3864; struct Cyc_Absyn_Tuniondecl* _temp3866; _LL3856: if(( unsigned int)
_temp3854 >  3u?*(( int*) _temp3854) ==  Cyc_Absyn_TunionType: 0){ _LL3861:
_temp3860=(( struct Cyc_Absyn_TunionType_struct*) _temp3854)->f1; _LL3863:
_temp3862=( void*) _temp3860.tunion_info; if(*(( int*) _temp3862) ==  Cyc_Absyn_KnownTunion){
_LL3865: _temp3864=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3862)->f1;
_temp3866=* _temp3864; goto _LL3857;} else{ goto _LL3858;}} else{ goto _LL3858;}
_LL3858: goto _LL3859; _LL3857: problem= Cyc_Absyn_qvar_cmp( _temp3866->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL3855; _LL3859: problem= 1; goto
_LL3855; _LL3855:;} goto _LL3849; _LL3853:{ void* _temp3867= _temp3847; struct
Cyc_Absyn_TunionInfo _temp3873; void* _temp3875; struct Cyc_Absyn_Tuniondecl**
_temp3877; struct Cyc_Absyn_Tuniondecl* _temp3879; _LL3869: if(( unsigned int)
_temp3867 >  3u?*(( int*) _temp3867) ==  Cyc_Absyn_TunionType: 0){ _LL3874:
_temp3873=(( struct Cyc_Absyn_TunionType_struct*) _temp3867)->f1; _LL3876:
_temp3875=( void*) _temp3873.tunion_info; if(*(( int*) _temp3875) ==  Cyc_Absyn_KnownTunion){
_LL3878: _temp3877=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3875)->f1;
_temp3879=* _temp3877; goto _LL3870;} else{ goto _LL3871;}} else{ goto _LL3871;}
_LL3871: goto _LL3872; _LL3870: problem= Cyc_Absyn_qvar_cmp( _temp3879->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL3868; _LL3872: problem= 1; goto
_LL3868; _LL3868:;} goto _LL3849; _LL3849:;} if( problem){({ void* _temp3880[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3880, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp3536; for( 0; rpo !=  0; rpo= rpo->tl){ struct
_tuple8 _temp3883; void* _temp3884; void* _temp3886; struct _tuple8* _temp3881=(
struct _tuple8*) rpo->hd; _temp3883=* _temp3881; _LL3887: _temp3886= _temp3883.f1;
goto _LL3885; _LL3885: _temp3884= _temp3883.f2; goto _LL3882; _LL3882: _temp3787=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp3787,( void*) Cyc_Absyn_RgnKind,
_temp3886); _temp3787= Cyc_Tcutil_i_check_valid_type( loc, te, _temp3787,( void*)
Cyc_Absyn_RgnKind, _temp3884);}} if(* _temp3548 !=  0){ _temp3787= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3787,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp3548))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp3787.free_vars; for(
0; tvs !=  0; tvs= tvs->tl){ void* _temp3888= Cyc_Absyn_compress_kb(( void*)((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); void* _temp3902; struct Cyc_Core_Opt*
_temp3904; struct Cyc_Core_Opt** _temp3906; void* _temp3907; void* _temp3909;
struct Cyc_Core_Opt* _temp3911; struct Cyc_Core_Opt** _temp3913; void* _temp3914;
struct Cyc_Core_Opt* _temp3916; struct Cyc_Core_Opt** _temp3918; _LL3890: if(*((
int*) _temp3888) ==  Cyc_Absyn_Less_kb){ _LL3905: _temp3904=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3888)->f1; _temp3906=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3888)->f1; goto _LL3903; _LL3903: _temp3902=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3888)->f2; if( _temp3902 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3891;}
else{ goto _LL3892;}} else{ goto _LL3892;} _LL3892: if(*(( int*) _temp3888) == 
Cyc_Absyn_Eq_kb){ _LL3908: _temp3907=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp3888)->f1; if( _temp3907 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3893;}
else{ goto _LL3894;}} else{ goto _LL3894;} _LL3894: if(*(( int*) _temp3888) == 
Cyc_Absyn_Less_kb){ _LL3912: _temp3911=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3888)->f1; _temp3913=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3888)->f1; goto _LL3910; _LL3910: _temp3909=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3888)->f2; if( _temp3909 == ( void*) Cyc_Absyn_EffKind){ goto _LL3895;}
else{ goto _LL3896;}} else{ goto _LL3896;} _LL3896: if(*(( int*) _temp3888) == 
Cyc_Absyn_Eq_kb){ _LL3915: _temp3914=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp3888)->f1; if( _temp3914 == ( void*) Cyc_Absyn_EffKind){ goto _LL3897;}
else{ goto _LL3898;}} else{ goto _LL3898;} _LL3898: if(*(( int*) _temp3888) == 
Cyc_Absyn_Unknown_kb){ _LL3917: _temp3916=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp3888)->f1; _temp3918=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp3888)->f1; goto _LL3899;} else{ goto _LL3900;} _LL3900: goto _LL3901;
_LL3891:* _temp3906=({ struct Cyc_Core_Opt* _temp3919=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3919->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp3920=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp3920[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp3921; _temp3921.tag= Cyc_Absyn_Eq_kb; _temp3921.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp3921;}); _temp3920;})); _temp3919;}); goto _LL3893; _LL3893: effect=({
struct Cyc_List_List* _temp3922=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3922->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3923=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3923[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3924; _temp3924.tag= Cyc_Absyn_AccessEff;
_temp3924.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3925=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3925[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3926; _temp3926.tag= Cyc_Absyn_VarType;
_temp3926.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3926;}); _temp3925;}));
_temp3924;}); _temp3923;})); _temp3922->tl= effect; _temp3922;}); goto _LL3889;
_LL3895:* _temp3913=({ struct Cyc_Core_Opt* _temp3927=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3927->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp3928=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp3928[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp3929; _temp3929.tag= Cyc_Absyn_Eq_kb; _temp3929.f1=( void*)(( void*) Cyc_Absyn_EffKind);
_temp3929;}); _temp3928;})); _temp3927;}); goto _LL3897; _LL3897: effect=({
struct Cyc_List_List* _temp3930=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3930->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3931=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3931[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3932; _temp3932.tag= Cyc_Absyn_VarType;
_temp3932.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3932;}); _temp3931;}));
_temp3930->tl= effect; _temp3930;}); goto _LL3889; _LL3899:* _temp3918=({ struct
Cyc_Core_Opt* _temp3933=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3933->v=( void*)(( void*)({ struct Cyc_Absyn_Less_kb_struct* _temp3934=(
struct Cyc_Absyn_Less_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct));
_temp3934[ 0]=({ struct Cyc_Absyn_Less_kb_struct _temp3935; _temp3935.tag= Cyc_Absyn_Less_kb;
_temp3935.f1= 0; _temp3935.f2=( void*)(( void*) Cyc_Absyn_AnyKind); _temp3935;});
_temp3934;})); _temp3933;}); goto _LL3901; _LL3901: effect=({ struct Cyc_List_List*
_temp3936=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3936->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp3937=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3937[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3938; _temp3938.tag= Cyc_Absyn_RgnsEff;
_temp3938.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3939=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3939[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3940; _temp3940.tag= Cyc_Absyn_VarType;
_temp3940.f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp3940;}); _temp3939;}));
_temp3938;}); _temp3937;})); _temp3936->tl= effect; _temp3936;}); goto _LL3889;
_LL3889:;}} effect= Cyc_List_imp_rev( effect);{ struct Cyc_List_List* ts=
_temp3787.free_evars; for( 0; ts !=  0; ts= ts->tl){ void* _temp3941= Cyc_Tcutil_typ_kind((
void*) ts->hd); _LL3943: if( _temp3941 == ( void*) Cyc_Absyn_RgnKind){ goto
_LL3944;} else{ goto _LL3945;} _LL3945: if( _temp3941 == ( void*) Cyc_Absyn_EffKind){
goto _LL3946;} else{ goto _LL3947;} _LL3947: goto _LL3948; _LL3944: effect=({
struct Cyc_List_List* _temp3949=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp3949->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3950=( struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3950[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3951; _temp3951.tag= Cyc_Absyn_AccessEff;
_temp3951.f1=( void*)(( void*) ts->hd); _temp3951;}); _temp3950;})); _temp3949->tl=
effect; _temp3949;}); goto _LL3942; _LL3946: effect=({ struct Cyc_List_List*
_temp3952=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3952->hd=( void*)(( void*) ts->hd); _temp3952->tl= effect; _temp3952;});
goto _LL3942; _LL3948: effect=({ struct Cyc_List_List* _temp3953=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3953->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3954=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3954[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3955; _temp3955.tag= Cyc_Absyn_RgnsEff; _temp3955.f1=(
void*)(( void*) ts->hd); _temp3955;}); _temp3954;})); _temp3953->tl= effect;
_temp3953;}); goto _LL3942; _LL3942:;}}* _temp3548=({ struct Cyc_Core_Opt*
_temp3956=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3956->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3957=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3957[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3958; _temp3958.tag= Cyc_Absyn_JoinEff;
_temp3958.f1= Cyc_List_imp_rev( effect); _temp3958;}); _temp3957;})); _temp3956;});}}
if(* _temp3551 !=  0){ struct Cyc_List_List* bs=* _temp3551; for( 0; bs !=  0;
bs= bs->tl){ void* _temp3959= Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*)
bs->hd)->kind); struct Cyc_Core_Opt* _temp3973; struct Cyc_Core_Opt** _temp3975;
void* _temp3976; struct Cyc_Core_Opt* _temp3978; struct Cyc_Core_Opt** _temp3980;
void* _temp3981; struct Cyc_Core_Opt* _temp3983; struct Cyc_Core_Opt** _temp3985;
void* _temp3986; struct Cyc_Core_Opt* _temp3988; struct Cyc_Core_Opt** _temp3990;
void* _temp3991; _LL3961: if(*(( int*) _temp3959) ==  Cyc_Absyn_Unknown_kb){
_LL3974: _temp3973=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp3959)->f1;
_temp3975=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp3959)->f1; goto _LL3962;} else{ goto _LL3963;} _LL3963: if(*(( int*)
_temp3959) ==  Cyc_Absyn_Less_kb){ _LL3979: _temp3978=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f1; _temp3980=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f1; goto _LL3977; _LL3977: _temp3976=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f2; if( _temp3976 == ( void*) Cyc_Absyn_MemKind){ goto _LL3964;}
else{ goto _LL3965;}} else{ goto _LL3965;} _LL3965: if(*(( int*) _temp3959) == 
Cyc_Absyn_Less_kb){ _LL3984: _temp3983=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f1; _temp3985=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f1; goto _LL3982; _LL3982: _temp3981=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f2; if( _temp3981 == ( void*) Cyc_Absyn_AnyKind){ goto _LL3966;}
else{ goto _LL3967;}} else{ goto _LL3967;} _LL3967: if(*(( int*) _temp3959) == 
Cyc_Absyn_Less_kb){ _LL3989: _temp3988=(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f1; _temp3990=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f1; goto _LL3987; _LL3987: _temp3986=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp3959)->f2; goto _LL3968;} else{ goto _LL3969;} _LL3969: if(*(( int*)
_temp3959) ==  Cyc_Absyn_Eq_kb){ _LL3992: _temp3991=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp3959)->f1; if( _temp3991 == ( void*) Cyc_Absyn_MemKind){ goto _LL3970;}
else{ goto _LL3971;}} else{ goto _LL3971;} _LL3971: goto _LL3972; _LL3962:({
struct Cyc_Std_String_pa_struct _temp3994; _temp3994.tag= Cyc_Std_String_pa;
_temp3994.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) bs->hd)->name;{
void* _temp3993[ 1u]={& _temp3994}; Cyc_Tcutil_warn( loc, _tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof( unsigned char), 47u), _tag_arr( _temp3993, sizeof( void*), 1u));}});
_temp3980= _temp3975; goto _LL3964; _LL3964: _temp3985= _temp3980; goto _LL3966;
_LL3966:* _temp3985=({ struct Cyc_Core_Opt* _temp3995=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3995->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp3996=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp3996[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp3997; _temp3997.tag= Cyc_Absyn_Eq_kb; _temp3997.f1=( void*)(( void*) Cyc_Absyn_BoxKind);
_temp3997;}); _temp3996;})); _temp3995;}); goto _LL3960; _LL3968:* _temp3990=({
struct Cyc_Core_Opt* _temp3998=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3998->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp3999=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp3999[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp4000; _temp4000.tag= Cyc_Absyn_Eq_kb;
_temp4000.f1=( void*) _temp3986; _temp4000;}); _temp3999;})); _temp3998;}); goto
_LL3960; _LL3970:({ void* _temp4001[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("function's can't abstract M types",
sizeof( unsigned char), 34u), _tag_arr( _temp4001, sizeof( void*), 0u));}); goto
_LL3960; _LL3972: goto _LL3960; _LL3960:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp3787.kind_env,* _temp3551); _temp3787.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp3787.free_vars,* _temp3551);{ struct Cyc_List_List* tvs= _temp3787.free_vars;
for( 0; tvs !=  0; tvs= tvs->tl){ cvtenv.free_vars= Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,( struct Cyc_Absyn_Tvar*) tvs->hd);}}{ struct Cyc_List_List*
evs= _temp3787.free_evars; for( 0; evs !=  0; evs= evs->tl){ cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars,( void*) evs->hd);}} goto _LL3429;}}
_LL3459: for( 0; _temp3552 !=  0; _temp3552= _temp3552->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp3552->hd)).f2);}
goto _LL3429; _LL3461:{ struct _RegionHandle _temp4003= _new_region("sprev_rgn");
struct _RegionHandle* sprev_rgn=& _temp4003; _push_region( sprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp3554 !=  0; _temp3554= _temp3554->tl){
struct Cyc_Absyn_Structfield _temp4006; struct Cyc_List_List* _temp4007; struct
Cyc_Absyn_Exp* _temp4009; void* _temp4011; struct Cyc_Absyn_Tqual _temp4013;
struct _tagged_arr* _temp4015; struct Cyc_Absyn_Structfield* _temp4004=( struct
Cyc_Absyn_Structfield*) _temp3554->hd; _temp4006=* _temp4004; _LL4016: _temp4015=
_temp4006.name; goto _LL4014; _LL4014: _temp4013= _temp4006.tq; goto _LL4012;
_LL4012: _temp4011=( void*) _temp4006.type; goto _LL4010; _LL4010: _temp4009=
_temp4006.width; goto _LL4008; _LL4008: _temp4007= _temp4006.attributes; goto
_LL4005; _LL4005: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_zstrptrcmp, prev_fields, _temp4015)){({ struct Cyc_Std_String_pa_struct
_temp4018; _temp4018.tag= Cyc_Std_String_pa; _temp4018.f1=( struct _tagged_arr)*
_temp4015;{ void* _temp4017[ 1u]={& _temp4018}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp4017, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp4015, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp4019=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp4019->hd=( void*)
_temp4015; _temp4019->tl= prev_fields; _temp4019;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp4011); Cyc_Tcutil_check_bitfield(
loc, te, _temp4011, _temp4009, _temp4015); Cyc_Tcutil_check_field_atts( loc,
_temp4015, _temp4007);}}; _pop_region( sprev_rgn);} goto _LL3429; _LL3463:{
struct _RegionHandle _temp4020= _new_region("uprev_rgn"); struct _RegionHandle*
uprev_rgn=& _temp4020; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp3556 !=  0; _temp3556= _temp3556->tl){ struct Cyc_Absyn_Structfield
_temp4023; struct Cyc_List_List* _temp4024; struct Cyc_Absyn_Exp* _temp4026;
void* _temp4028; struct Cyc_Absyn_Tqual _temp4030; struct _tagged_arr* _temp4032;
struct Cyc_Absyn_Structfield* _temp4021=( struct Cyc_Absyn_Structfield*)
_temp3556->hd; _temp4023=* _temp4021; _LL4033: _temp4032= _temp4023.name; goto
_LL4031; _LL4031: _temp4030= _temp4023.tq; goto _LL4029; _LL4029: _temp4028=(
void*) _temp4023.type; goto _LL4027; _LL4027: _temp4026= _temp4023.width; goto
_LL4025; _LL4025: _temp4024= _temp4023.attributes; goto _LL4022; _LL4022: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp4032)){({ struct Cyc_Std_String_pa_struct _temp4035; _temp4035.tag= Cyc_Std_String_pa;
_temp4035.f1=( struct _tagged_arr)* _temp4032;{ void* _temp4034[ 1u]={&
_temp4035}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp4034, sizeof( void*), 1u));}});}
if( Cyc_Std_strcmp(* _temp4032, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp4036=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp4036->hd=( void*)
_temp4032; _temp4036->tl= prev_fields; _temp4036;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp4028); if( ! Cyc_Tcutil_bits_only(
_temp4028)){({ struct Cyc_Std_String_pa_struct _temp4038; _temp4038.tag= Cyc_Std_String_pa;
_temp4038.f1=( struct _tagged_arr)* _temp4032;{ void* _temp4037[ 1u]={&
_temp4038}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp4037, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp4028, _temp4026, _temp4032); Cyc_Tcutil_check_field_atts(
loc, _temp4032, _temp4024);}}; _pop_region( uprev_rgn);} goto _LL3429; _LL3465:
if( _temp3564 ==  0){ if(* _temp3560 ==  0){({ void* _temp4039[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp4039, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp3560));;}} if(*
_temp3560 ==  0){ struct _handler_cons _temp4040; _push_handler(& _temp4040);{
int _temp4042= 0; if( setjmp( _temp4040.handler)){ _temp4042= 1;} if( !
_temp4042){* _temp3560=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp3564));; _pop_handler();} else{
void* _temp4041=( void*) _exn_thrown; void* _temp4044= _temp4041; _LL4046: if(
_temp4044 ==  Cyc_Dict_Absent){ goto _LL4047;} else{ goto _LL4048;} _LL4048:
goto _LL4049; _LL4047: { struct _tuple1* tdn=( struct _tuple1*) _check_null(
_temp3564); struct Cyc_Tcenv_Genv* _temp4050=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_Absyn_Structdecl* _temp4051=({ struct Cyc_Absyn_Structdecl* _temp4054=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp4054->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp4054->name=({ struct Cyc_Core_Opt*
_temp4055=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp4055->v=( void*) tdn; _temp4055;}); _temp4054->tvs= 0; _temp4054->fields= 0;
_temp4054->attributes= 0; _temp4054;}); Cyc_Tc_tcStructdecl( te, _temp4050, loc,
_temp4051);* _temp3560=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc, tdn); if(* _temp3563 !=  0){({ struct Cyc_Std_String_pa_struct
_temp4053; _temp4053.tag= Cyc_Std_String_pa; _temp4053.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tdn);{ void* _temp4052[ 1u]={& _temp4053}; Cyc_Tcutil_terr(
loc, _tag_arr("please declare parameterized struct %s before using", sizeof(
unsigned char), 52u), _tag_arr( _temp4052, sizeof( void*), 1u));}}); return
cvtenv;} goto _LL4045;} _LL4049:( void) _throw( _temp4044); _LL4045:;}}}{ struct
Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp3560)); if( sd->name !=  0){*(( struct _tuple1*) _check_null( _temp3564))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
struct Cyc_List_List* ts=* _temp3563; for( 0; ts !=  0? tvs !=  0: 0;( ts= ts->tl,
tvs= tvs->tl)){ void* k= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) tvs->hd);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*) ts->hd);} if(
ts !=  0){({ struct Cyc_Std_String_pa_struct _temp4057; _temp4057.tag= Cyc_Std_String_pa;
_temp4057.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp3564));{ void* _temp4056[ 1u]={& _temp4057}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for struct %s", sizeof( unsigned char), 34u),
_tag_arr( _temp4056, sizeof( void*), 1u));}});} if( tvs !=  0){ struct Cyc_List_List*
hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){ void* k= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*) tvs->hd); void* e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({
struct Cyc_List_List* _temp4058=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp4058->hd=( void*) e; _temp4058->tl= hidden_ts;
_temp4058;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k, e);}*
_temp3563= Cyc_List_imp_append(* _temp3563, Cyc_List_imp_rev( hidden_ts));} goto
_LL3429;}} _LL3467: if( _temp3572 ==  0){({ void* _temp4059[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr(
_temp4059, sizeof( void*), 0u));}); return cvtenv;} if(* _temp3568 ==  0){
struct _handler_cons _temp4060; _push_handler(& _temp4060);{ int _temp4062= 0;
if( setjmp( _temp4060.handler)){ _temp4062= 1;} if( ! _temp4062){* _temp3568=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp3572));; _pop_handler();} else{ void* _temp4061=(
void*) _exn_thrown; void* _temp4064= _temp4061; _LL4066: if( _temp4064 ==  Cyc_Dict_Absent){
goto _LL4067;} else{ goto _LL4068;} _LL4068: goto _LL4069; _LL4067: { struct
_tuple1* _temp4070=( struct _tuple1*) _check_null( _temp3572); struct Cyc_Tcenv_Genv*
_temp4071=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_Absyn_Uniondecl* _temp4072=({
struct Cyc_Absyn_Uniondecl* _temp4075=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp4075->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp4075->name=({ struct Cyc_Core_Opt* _temp4076=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4076->v=( void*) _temp4070;
_temp4076;}); _temp4075->tvs= 0; _temp4075->fields= 0; _temp4075->attributes= 0;
_temp4075;}); Cyc_Tc_tcUniondecl( te, _temp4071, loc, _temp4072);* _temp3568=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc, _temp4070);
if(* _temp3571 !=  0){({ struct Cyc_Std_String_pa_struct _temp4074; _temp4074.tag=
Cyc_Std_String_pa; _temp4074.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp4070);{ void* _temp4073[ 1u]={& _temp4074}; Cyc_Tcutil_terr( loc, _tag_arr("please declare parameterized union %s before using",
sizeof( unsigned char), 51u), _tag_arr( _temp4073, sizeof( void*), 1u));}});
return cvtenv;} goto _LL4065;} _LL4069:( void) _throw( _temp4064); _LL4065:;}}}{
struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp3568)); if( ud->name !=  0){* _temp3572=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= ud->tvs; struct Cyc_List_List* ts=* _temp3571; for( 0; ts != 
0? tvs !=  0: 0;( ts= ts->tl, tvs= tvs->tl)){ void* k= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*) tvs->hd); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*) ts->hd);} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp4078; _temp4078.tag= Cyc_Std_String_pa; _temp4078.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp3572));{ void*
_temp4077[ 1u]={& _temp4078}; Cyc_Tcutil_terr( loc, _tag_arr("too many parameters for union %s",
sizeof( unsigned char), 33u), _tag_arr( _temp4077, sizeof( void*), 1u));}});}
if( tvs !=  0){ struct Cyc_List_List* hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){
void* k= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*) tvs->hd); void* e= Cyc_Absyn_new_evar(
0, 0); hidden_ts=({ struct Cyc_List_List* _temp4079=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4079->hd=( void*) e; _temp4079->tl=
hidden_ts; _temp4079;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,
k, e);}* _temp3571= Cyc_List_imp_append(* _temp3571, Cyc_List_imp_rev( hidden_ts));}
goto _LL3429;}} _LL3469: { struct Cyc_List_List* targs=* _temp3579; struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp4080; _push_handler(& _temp4080);{ int _temp4082=
0; if( setjmp( _temp4080.handler)){ _temp4082= 1;} if( ! _temp4082){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp3580);; _pop_handler();} else{ void* _temp4081=( void*)
_exn_thrown; void* _temp4084= _temp4081; _LL4086: if( _temp4084 ==  Cyc_Dict_Absent){
goto _LL4087;} else{ goto _LL4088;} _LL4088: goto _LL4089; _LL4087:({ struct Cyc_Std_String_pa_struct
_temp4091; _temp4091.tag= Cyc_Std_String_pa; _temp4091.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3580);{ void* _temp4090[ 1u]={& _temp4091}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp4090, sizeof( void*), 1u));}}); return cvtenv; _LL4089:( void) _throw(
_temp4084); _LL4085:;}}} _temp3580[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= targs; struct Cyc_List_List* inst= 0; for( 0; ts !=  0?
tvs !=  0: 0;( ts= ts->tl, tvs= tvs->tl)){ void* k= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*) tvs->hd); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp4092=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4092->hd=( void*)({ struct
_tuple5* _temp4093=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp4093->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp4093->f2=( void*) ts->hd;
_temp4093;}); _temp4092->tl= inst; _temp4092;});} if( ts !=  0){({ struct Cyc_Std_String_pa_struct
_temp4095; _temp4095.tag= Cyc_Std_String_pa; _temp4095.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp3580);{ void* _temp4094[ 1u]={& _temp4095}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp4094, sizeof( void*), 1u));}});} if( tvs !=  0){ struct Cyc_List_List*
hidden_ts= 0; for( 0; tvs !=  0; tvs= tvs->tl){ void* k= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*) tvs->hd); void* e= Cyc_Absyn_new_evar( 0, 0); hidden_ts=({
struct Cyc_List_List* _temp4096=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp4096->hd=( void*) e; _temp4096->tl= hidden_ts;
_temp4096;}); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k, e);
inst=({ struct Cyc_List_List* _temp4097=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp4097->hd=( void*)({ struct _tuple5*
_temp4098=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp4098->f1=(
struct Cyc_Absyn_Tvar*) tvs->hd; _temp4098->f2= e; _temp4098;}); _temp4097->tl=
inst; _temp4097;});}* _temp3579= Cyc_List_imp_append( targs, Cyc_List_imp_rev(
hidden_ts));}{ void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);*
_temp3576=({ struct Cyc_Core_Opt* _temp4099=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp4099->v=( void*) new_typ; _temp4099;}); goto
_LL3429;}}} _LL3471: goto _LL3429; _LL3473: _temp3584= _temp3582; goto _LL3475;
_LL3475: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp3584); goto _LL3429; _LL3477: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp3586); goto _LL3429; _LL3479: for( 0;
_temp3588 !=  0; _temp3588= _temp3588->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*) _temp3588->hd);} goto
_LL3429; _LL3429:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){({ struct Cyc_Std_String_pa_struct _temp4103; _temp4103.tag= Cyc_Std_String_pa;
_temp4103.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_Std_String_pa_struct _temp4102; _temp4102.tag= Cyc_Std_String_pa;
_temp4102.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_Std_String_pa_struct _temp4101; _temp4101.tag= Cyc_Std_String_pa;
_temp4101.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp4100[
3u]={& _temp4101,& _temp4102,& _temp4103}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp4100, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp4104= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp4119; _temp4119.kind_env= kind_env; _temp4119.free_vars= 0; _temp4119.free_evars=
0; _temp4119.generalize_evars= generalize_evars; _temp4119.fn_result= 0;
_temp4119;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp4104.free_vars;
for( 0; vs !=  0; vs= vs->tl){ _temp4104.kind_env= Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*) vs->hd);}}{ struct Cyc_List_List* evs=
_temp4104.free_evars; for( 0; evs !=  0; evs= evs->tl){ void* _temp4105= Cyc_Tcutil_compress((
void*) evs->hd); struct Cyc_Core_Opt* _temp4111; struct Cyc_Core_Opt** _temp4113;
_LL4107: if(( unsigned int) _temp4105 >  3u?*(( int*) _temp4105) ==  Cyc_Absyn_Evar:
0){ _LL4112: _temp4111=(( struct Cyc_Absyn_Evar_struct*) _temp4105)->f4;
_temp4113=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp4105)->f4;
goto _LL4108;} else{ goto _LL4109;} _LL4109: goto _LL4110; _LL4108: if(*
_temp4113 ==  0){* _temp4113=({ struct Cyc_Core_Opt* _temp4114=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4114->v=( void*) kind_env;
_temp4114;});} else{ struct Cyc_List_List* _temp4115=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp4113))->v; struct Cyc_List_List*
_temp4116= 0; for( 0; _temp4115 !=  0; _temp4115= _temp4115->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, kind_env,(
struct Cyc_Absyn_Tvar*) _temp4115->hd)){ _temp4116=({ struct Cyc_List_List*
_temp4117=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4117->hd=( void*)(( struct Cyc_Absyn_Tvar*) _temp4115->hd); _temp4117->tl=
_temp4116; _temp4117;});}}* _temp4113=({ struct Cyc_Core_Opt* _temp4118=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4118->v=( void*)
_temp4116; _temp4118;});} goto _LL4106; _LL4110: goto _LL4106; _LL4106:;}}
return _temp4104;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ int generalize_evars= Cyc_Tcutil_is_function_type(
t); struct Cyc_Tcutil_CVTEnv _temp4120= Cyc_Tcutil_check_valid_type( loc, te, 0,(
void*) Cyc_Absyn_MemKind, generalize_evars, t); struct Cyc_List_List* _temp4121=
_temp4120.free_vars; struct Cyc_List_List* _temp4122= _temp4120.free_evars;{
struct Cyc_List_List* x= _temp4121; for( 0; x !=  0; x= x->tl){ void* _temp4123=
Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*) x->hd)->kind); struct
Cyc_Core_Opt* _temp4137; struct Cyc_Core_Opt** _temp4139; void* _temp4140;
struct Cyc_Core_Opt* _temp4142; struct Cyc_Core_Opt** _temp4144; void* _temp4145;
struct Cyc_Core_Opt* _temp4147; struct Cyc_Core_Opt** _temp4149; void* _temp4150;
struct Cyc_Core_Opt* _temp4152; struct Cyc_Core_Opt** _temp4154; void* _temp4155;
_LL4125: if(*(( int*) _temp4123) ==  Cyc_Absyn_Unknown_kb){ _LL4138: _temp4137=((
struct Cyc_Absyn_Unknown_kb_struct*) _temp4123)->f1; _temp4139=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*) _temp4123)->f1; goto _LL4126;} else{ goto
_LL4127;} _LL4127: if(*(( int*) _temp4123) ==  Cyc_Absyn_Less_kb){ _LL4143:
_temp4142=(( struct Cyc_Absyn_Less_kb_struct*) _temp4123)->f1; _temp4144=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp4123)->f1; goto
_LL4141; _LL4141: _temp4140=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp4123)->f2; if( _temp4140 == ( void*) Cyc_Absyn_MemKind){ goto _LL4128;}
else{ goto _LL4129;}} else{ goto _LL4129;} _LL4129: if(*(( int*) _temp4123) == 
Cyc_Absyn_Less_kb){ _LL4148: _temp4147=(( struct Cyc_Absyn_Less_kb_struct*)
_temp4123)->f1; _temp4149=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp4123)->f1; goto _LL4146; _LL4146: _temp4145=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp4123)->f2; if( _temp4145 == ( void*) Cyc_Absyn_AnyKind){ goto _LL4130;}
else{ goto _LL4131;}} else{ goto _LL4131;} _LL4131: if(*(( int*) _temp4123) == 
Cyc_Absyn_Less_kb){ _LL4153: _temp4152=(( struct Cyc_Absyn_Less_kb_struct*)
_temp4123)->f1; _temp4154=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp4123)->f1; goto _LL4151; _LL4151: _temp4150=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp4123)->f2; goto _LL4132;} else{ goto _LL4133;} _LL4133: if(*(( int*)
_temp4123) ==  Cyc_Absyn_Eq_kb){ _LL4156: _temp4155=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp4123)->f1; if( _temp4155 == ( void*) Cyc_Absyn_MemKind){ goto _LL4134;}
else{ goto _LL4135;}} else{ goto _LL4135;} _LL4135: goto _LL4136; _LL4126:
_temp4144= _temp4139; goto _LL4128; _LL4128: _temp4149= _temp4144; goto _LL4130;
_LL4130:* _temp4149=({ struct Cyc_Core_Opt* _temp4157=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4157->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp4158=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp4158[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp4159; _temp4159.tag= Cyc_Absyn_Eq_kb; _temp4159.f1=( void*)(( void*) Cyc_Absyn_BoxKind);
_temp4159;}); _temp4158;})); _temp4157;}); goto _LL4124; _LL4132:* _temp4154=({
struct Cyc_Core_Opt* _temp4160=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp4160->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp4161=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp4161[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp4162; _temp4162.tag= Cyc_Absyn_Eq_kb;
_temp4162.f1=( void*) _temp4150; _temp4162;}); _temp4161;})); _temp4160;}); goto
_LL4124; _LL4134:({ struct Cyc_Std_String_pa_struct _temp4164; _temp4164.tag=
Cyc_Std_String_pa; _temp4164.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*) x->hd);{ void* _temp4163[ 1u]={& _temp4164}; Cyc_Tcutil_terr(
loc, _tag_arr("type variable %s cannot have kind M", sizeof( unsigned char), 36u),
_tag_arr( _temp4163, sizeof( void*), 1u));}}); goto _LL4124; _LL4136: goto
_LL4124; _LL4124:;}} if( _temp4121 !=  0? 1: _temp4122 !=  0){{ void* _temp4165=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp4171; struct Cyc_List_List*
_temp4173; struct Cyc_List_List* _temp4175; struct Cyc_Absyn_VarargInfo*
_temp4177; int _temp4179; struct Cyc_List_List* _temp4181; void* _temp4183;
struct Cyc_Core_Opt* _temp4185; struct Cyc_List_List* _temp4187; struct Cyc_List_List**
_temp4189; _LL4167: if(( unsigned int) _temp4165 >  3u?*(( int*) _temp4165) == 
Cyc_Absyn_FnType: 0){ _LL4172: _temp4171=(( struct Cyc_Absyn_FnType_struct*)
_temp4165)->f1; _LL4188: _temp4187= _temp4171.tvars; _temp4189=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp4165)->f1).tvars; goto _LL4186; _LL4186:
_temp4185= _temp4171.effect; goto _LL4184; _LL4184: _temp4183=( void*) _temp4171.ret_typ;
goto _LL4182; _LL4182: _temp4181= _temp4171.args; goto _LL4180; _LL4180:
_temp4179= _temp4171.c_varargs; goto _LL4178; _LL4178: _temp4177= _temp4171.cyc_varargs;
goto _LL4176; _LL4176: _temp4175= _temp4171.rgn_po; goto _LL4174; _LL4174:
_temp4173= _temp4171.attributes; goto _LL4168;} else{ goto _LL4169;} _LL4169:
goto _LL4170; _LL4168: if(* _temp4189 ==  0){* _temp4189= _temp4121; _temp4121=
0;} goto _LL4166; _LL4170: goto _LL4166; _LL4166:;} if( _temp4121 !=  0){({
struct Cyc_Std_String_pa_struct _temp4191; _temp4191.tag= Cyc_Std_String_pa;
_temp4191.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) _temp4121->hd)->name;{
void* _temp4190[ 1u]={& _temp4191}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp4190, sizeof( void*), 1u));}});}
if( _temp4122 !=  0){ for( 0; _temp4122 !=  0; _temp4122= _temp4122->tl){ void*
e=( void*) _temp4122->hd; void* _temp4192= Cyc_Tcutil_typ_kind( e); _LL4194: if(
_temp4192 == ( void*) Cyc_Absyn_RgnKind){ goto _LL4195;} else{ goto _LL4196;}
_LL4196: if( _temp4192 == ( void*) Cyc_Absyn_EffKind){ goto _LL4197;} else{ goto
_LL4198;} _LL4198: goto _LL4199; _LL4195: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp4200[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp4200, sizeof( void*), 0u));});} goto _LL4193; _LL4197: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp4201[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp4201, sizeof( void*), 0u));});} goto _LL4193; _LL4199:({ struct Cyc_Std_String_pa_struct
_temp4204; _temp4204.tag= Cyc_Std_String_pa; _temp4204.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp4203;
_temp4203.tag= Cyc_Std_String_pa; _temp4203.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp4202[ 2u]={& _temp4203,& _temp4204}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp4202, sizeof( void*), 2u));}}}); goto
_LL4193; _LL4193:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp4205= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp4211; struct Cyc_List_List* _temp4213; struct
Cyc_List_List* _temp4215; struct Cyc_Absyn_VarargInfo* _temp4217; int _temp4219;
struct Cyc_List_List* _temp4221; void* _temp4223; struct Cyc_Core_Opt* _temp4225;
struct Cyc_List_List* _temp4227; _LL4207: if(( unsigned int) _temp4205 >  3u?*((
int*) _temp4205) ==  Cyc_Absyn_FnType: 0){ _LL4212: _temp4211=(( struct Cyc_Absyn_FnType_struct*)
_temp4205)->f1; _LL4228: _temp4227= _temp4211.tvars; goto _LL4226; _LL4226:
_temp4225= _temp4211.effect; goto _LL4224; _LL4224: _temp4223=( void*) _temp4211.ret_typ;
goto _LL4222; _LL4222: _temp4221= _temp4211.args; goto _LL4220; _LL4220:
_temp4219= _temp4211.c_varargs; goto _LL4218; _LL4218: _temp4217= _temp4211.cyc_varargs;
goto _LL4216; _LL4216: _temp4215= _temp4211.rgn_po; goto _LL4214; _LL4214:
_temp4213= _temp4211.attributes; goto _LL4208;} else{ goto _LL4209;} _LL4209:
goto _LL4210; _LL4208: fd->tvs= _temp4227; fd->effect= _temp4225; goto _LL4206;
_LL4210:({ void* _temp4229[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp4229, sizeof( void*), 0u));});
return; _LL4206:;}{ struct _RegionHandle _temp4230= _new_region("r"); struct
_RegionHandle* r=& _temp4230; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp4231=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp4231->v=(
void*) t; _temp4231;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp4232=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp4233= Cyc_Tcutil_remove_bound_tvars( _temp4232.free_vars,
bound_tvars); struct Cyc_List_List* _temp4234= _temp4232.free_evars;{ struct Cyc_List_List*
fs= _temp4233; for( 0; fs !=  0; fs= fs->tl){ struct _tagged_arr* _temp4235=((
struct Cyc_Absyn_Tvar*) fs->hd)->name;({ struct Cyc_Std_String_pa_struct
_temp4238; _temp4238.tag= Cyc_Std_String_pa; _temp4238.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp4237;
_temp4237.tag= Cyc_Std_String_pa; _temp4237.f1=( struct _tagged_arr)* _temp4235;{
void* _temp4236[ 2u]={& _temp4237,& _temp4238}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp4236, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp4234 !=  0: 0){ for( 0; _temp4234 !=  0; _temp4234=
_temp4234->tl){ void* e=( void*) _temp4234->hd; void* _temp4239= Cyc_Tcutil_typ_kind(
e); _LL4241: if( _temp4239 == ( void*) Cyc_Absyn_RgnKind){ goto _LL4242;} else{
goto _LL4243;} _LL4243: if( _temp4239 == ( void*) Cyc_Absyn_EffKind){ goto
_LL4244;} else{ goto _LL4245;} _LL4245: goto _LL4246; _LL4242: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp4247[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp4247, sizeof( void*), 0u));});}
goto _LL4240; _LL4244: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4248=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4248[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4249; _temp4249.tag= Cyc_Absyn_JoinEff;
_temp4249.f1= 0; _temp4249;}); _temp4248;}))){({ void* _temp4250[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp4250, sizeof( void*), 0u));});}
goto _LL4240; _LL4246:({ struct Cyc_Std_String_pa_struct _temp4253; _temp4253.tag=
Cyc_Std_String_pa; _temp4253.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp4252; _temp4252.tag= Cyc_Std_String_pa;
_temp4252.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp4251[
2u]={& _temp4252,& _temp4253}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp4251, sizeof( void*), 2u));}}});
goto _LL4240; _LL4240:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs= vs->tl){
struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 !=  0; vs2= vs2->tl){ if( cmp((
void*) vs->hd,( void*) vs2->hd) ==  0){({ struct Cyc_Std_String_pa_struct
_temp4256; _temp4256.tag= Cyc_Std_String_pa; _temp4256.f1=( struct _tagged_arr)
a2string(( void*) vs->hd);{ struct Cyc_Std_String_pa_struct _temp4255; _temp4255.tag=
Cyc_Std_String_pa; _temp4255.f1=( struct _tagged_arr) msg;{ void* _temp4254[ 2u]={&
_temp4255,& _temp4256}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp4254, sizeof( void*), 2u));}}});}}}} static
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
_temp4257=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4257->hd=( void*)({ struct _tuple18* _temp4258=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp4258->f1=( struct Cyc_Absyn_Structfield*)
sd_fields->hd; _temp4258->f2= 0; _temp4258;}); _temp4257->tl= fields; _temp4257;});}}}
fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0; des= des->tl){ struct
_tuple19 _temp4261; void* _temp4262; struct Cyc_List_List* _temp4264; struct
_tuple19* _temp4259=( struct _tuple19*) des->hd; _temp4261=* _temp4259; _LL4265:
_temp4264= _temp4261.f1; goto _LL4263; _LL4263: _temp4262= _temp4261.f2; goto
_LL4260; _LL4260: if( _temp4264 ==  0){ struct Cyc_List_List* _temp4266= fields;
for( 0; _temp4266 !=  0; _temp4266= _temp4266->tl){ if( !(*(( struct _tuple18*)
_temp4266->hd)).f2){(*(( struct _tuple18*) _temp4266->hd)).f2= 1;(*(( struct
_tuple19*) des->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp4267=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4267->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4268=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4268[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4269; _temp4269.tag= Cyc_Absyn_FieldName;
_temp4269.f1=((*(( struct _tuple18*) _temp4266->hd)).f1)->name; _temp4269;});
_temp4268;})); _temp4267->tl= 0; _temp4267;}); ans=({ struct Cyc_List_List*
_temp4270=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp4270->hd=( void*)({ struct _tuple20* _temp4271=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp4271->f1=(*(( struct
_tuple18*) _temp4266->hd)).f1; _temp4271->f2= _temp4262; _temp4271;}); _temp4270->tl=
ans; _temp4270;}); break;}} if( _temp4266 ==  0){({ void* _temp4272[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u),
_tag_arr( _temp4272, sizeof( void*), 0u));});}} else{ if( _temp4264->tl !=  0){({
void* _temp4273[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp4273, sizeof( void*), 0u));});}
else{ void* _temp4274=( void*) _temp4264->hd; struct _tagged_arr* _temp4280;
_LL4276: if(*(( int*) _temp4274) ==  Cyc_Absyn_ArrayElement){ goto _LL4277;}
else{ goto _LL4278;} _LL4278: if(*(( int*) _temp4274) ==  Cyc_Absyn_FieldName){
_LL4281: _temp4280=(( struct Cyc_Absyn_FieldName_struct*) _temp4274)->f1; goto
_LL4279;} else{ goto _LL4275;} _LL4277:({ void* _temp4282[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp4282, sizeof( void*), 0u));}); goto _LL4275;
_LL4279: { struct Cyc_List_List* _temp4283= fields; for( 0; _temp4283 !=  0;
_temp4283= _temp4283->tl){ if( Cyc_Std_zstrptrcmp( _temp4280,((*(( struct
_tuple18*) _temp4283->hd)).f1)->name) ==  0){ if((*(( struct _tuple18*)
_temp4283->hd)).f2){({ struct Cyc_Std_String_pa_struct _temp4285; _temp4285.tag=
Cyc_Std_String_pa; _temp4285.f1=( struct _tagged_arr)* _temp4280;{ void*
_temp4284[ 1u]={& _temp4285}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp4284, sizeof( void*), 1u));}});}(*((
struct _tuple18*) _temp4283->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp4286=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp4286->hd=( void*)({ struct _tuple20* _temp4287=( struct _tuple20*)
_region_malloc( rgn, sizeof( struct _tuple20)); _temp4287->f1=(*(( struct
_tuple18*) _temp4283->hd)).f1; _temp4287->f2= _temp4262; _temp4287;}); _temp4286->tl=
ans; _temp4286;}); break;}} if( _temp4283 ==  0){({ struct Cyc_Std_String_pa_struct
_temp4289; _temp4289.tag= Cyc_Std_String_pa; _temp4289.f1=( struct _tagged_arr)*
_temp4280;{ void* _temp4288[ 1u]={& _temp4289}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp4288, sizeof( void*), 1u));}});}
goto _LL4275;} _LL4275:;}}} for( 0; fields !=  0; fields= fields->tl){ if( !(*((
struct _tuple18*) fields->hd)).f2){({ void* _temp4290[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments to struct", sizeof( unsigned char), 28u),
_tag_arr( _temp4290, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp4291= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp4297; struct Cyc_Absyn_Conref* _temp4299; void* _temp4301;
_LL4293: if(( unsigned int) _temp4291 >  3u?*(( int*) _temp4291) ==  Cyc_Absyn_PointerType:
0){ _LL4298: _temp4297=(( struct Cyc_Absyn_PointerType_struct*) _temp4291)->f1;
_LL4302: _temp4301=( void*) _temp4297.elt_typ; goto _LL4300; _LL4300: _temp4299=
_temp4297.bounds; goto _LL4294;} else{ goto _LL4295;} _LL4295: goto _LL4296;
_LL4294: { struct Cyc_Absyn_Conref* _temp4303= Cyc_Absyn_compress_conref(
_temp4299); void* _temp4304=( void*)( Cyc_Absyn_compress_conref( _temp4303))->v;
void* _temp4312; _LL4306: if(( unsigned int) _temp4304 >  1u?*(( int*) _temp4304)
==  Cyc_Absyn_Eq_constr: 0){ _LL4313: _temp4312=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4304)->f1; if( _temp4312 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4307;}
else{ goto _LL4308;}} else{ goto _LL4308;} _LL4308: if( _temp4304 == ( void*)
Cyc_Absyn_No_constr){ goto _LL4309;} else{ goto _LL4310;} _LL4310: goto _LL4311;
_LL4307:* elt_typ_dest= _temp4301; return 1; _LL4309:( void*)( _temp4303->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp4314=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp4314[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp4315; _temp4315.tag= Cyc_Absyn_Eq_constr;
_temp4315.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp4315;}); _temp4314;})));*
elt_typ_dest= _temp4301; return 1; _LL4311: return 0; _LL4305:;} _LL4296: return
0; _LL4292:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp4498;
_temp4498.f1= 0; _temp4498.f2=( void*) Cyc_Absyn_HeapRgn; _temp4498;}); void*
_temp4316=( void*) e->r; void* _temp4330; struct _tuple1* _temp4332; struct
_tagged_arr* _temp4334; struct Cyc_Absyn_Exp* _temp4336; struct _tagged_arr*
_temp4338; struct Cyc_Absyn_Exp* _temp4340; struct Cyc_Absyn_Exp* _temp4342;
struct Cyc_Absyn_Exp* _temp4344; struct Cyc_Absyn_Exp* _temp4346; _LL4318: if(*((
int*) _temp4316) ==  Cyc_Absyn_Var_e){ _LL4333: _temp4332=(( struct Cyc_Absyn_Var_e_struct*)
_temp4316)->f1; goto _LL4331; _LL4331: _temp4330=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp4316)->f2; goto _LL4319;} else{ goto _LL4320;} _LL4320: if(*(( int*)
_temp4316) ==  Cyc_Absyn_StructMember_e){ _LL4337: _temp4336=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp4316)->f1; goto _LL4335; _LL4335: _temp4334=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp4316)->f2; goto _LL4321;} else{ goto _LL4322;} _LL4322: if(*(( int*)
_temp4316) ==  Cyc_Absyn_StructArrow_e){ _LL4341: _temp4340=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp4316)->f1; goto _LL4339; _LL4339: _temp4338=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp4316)->f2; goto _LL4323;} else{ goto _LL4324;} _LL4324: if(*(( int*)
_temp4316) ==  Cyc_Absyn_Deref_e){ _LL4343: _temp4342=(( struct Cyc_Absyn_Deref_e_struct*)
_temp4316)->f1; goto _LL4325;} else{ goto _LL4326;} _LL4326: if(*(( int*)
_temp4316) ==  Cyc_Absyn_Subscript_e){ _LL4347: _temp4346=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp4316)->f1; goto _LL4345; _LL4345: _temp4344=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp4316)->f2; goto _LL4327;} else{ goto _LL4328;} _LL4328: goto _LL4329;
_LL4319: { void* _temp4348= _temp4330; struct Cyc_Absyn_Vardecl* _temp4362;
struct Cyc_Absyn_Vardecl* _temp4364; struct Cyc_Absyn_Vardecl* _temp4366; struct
Cyc_Absyn_Vardecl* _temp4368; _LL4350: if( _temp4348 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL4351;} else{ goto _LL4352;} _LL4352: if(( unsigned int) _temp4348 >  1u?*((
int*) _temp4348) ==  Cyc_Absyn_Funname_b: 0){ goto _LL4353;} else{ goto _LL4354;}
_LL4354: if(( unsigned int) _temp4348 >  1u?*(( int*) _temp4348) ==  Cyc_Absyn_Global_b:
0){ _LL4363: _temp4362=(( struct Cyc_Absyn_Global_b_struct*) _temp4348)->f1;
goto _LL4355;} else{ goto _LL4356;} _LL4356: if(( unsigned int) _temp4348 >  1u?*((
int*) _temp4348) ==  Cyc_Absyn_Local_b: 0){ _LL4365: _temp4364=(( struct Cyc_Absyn_Local_b_struct*)
_temp4348)->f1; goto _LL4357;} else{ goto _LL4358;} _LL4358: if(( unsigned int)
_temp4348 >  1u?*(( int*) _temp4348) ==  Cyc_Absyn_Pat_b: 0){ _LL4367: _temp4366=((
struct Cyc_Absyn_Pat_b_struct*) _temp4348)->f1; goto _LL4359;} else{ goto
_LL4360;} _LL4360: if(( unsigned int) _temp4348 >  1u?*(( int*) _temp4348) == 
Cyc_Absyn_Param_b: 0){ _LL4369: _temp4368=(( struct Cyc_Absyn_Param_b_struct*)
_temp4348)->f1; goto _LL4361;} else{ goto _LL4349;} _LL4351: return bogus_ans;
_LL4353: return({ struct _tuple7 _temp4370; _temp4370.f1= 0; _temp4370.f2=( void*)
Cyc_Absyn_HeapRgn; _temp4370;}); _LL4355: { void* _temp4371= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL4373: if((
unsigned int) _temp4371 >  3u?*(( int*) _temp4371) ==  Cyc_Absyn_ArrayType: 0){
goto _LL4374;} else{ goto _LL4375;} _LL4375: goto _LL4376; _LL4374: return({
struct _tuple7 _temp4377; _temp4377.f1= 1; _temp4377.f2=( void*) Cyc_Absyn_HeapRgn;
_temp4377;}); _LL4376: return({ struct _tuple7 _temp4378; _temp4378.f1=(
_temp4362->tq).q_const; _temp4378.f2=( void*) Cyc_Absyn_HeapRgn; _temp4378;});
_LL4372:;} _LL4357: { void* _temp4379= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL4381: if(( unsigned int) _temp4379 >  3u?*(( int*)
_temp4379) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4382;} else{ goto _LL4383;}
_LL4383: goto _LL4384; _LL4382: return({ struct _tuple7 _temp4385; _temp4385.f1=
1; _temp4385.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp4364->rgn))->v;
_temp4385;}); _LL4384: return({ struct _tuple7 _temp4386; _temp4386.f1=(
_temp4364->tq).q_const; _temp4386.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4364->rgn))->v; _temp4386;}); _LL4380:;} _LL4359: _temp4368=
_temp4366; goto _LL4361; _LL4361: return({ struct _tuple7 _temp4387; _temp4387.f1=(
_temp4368->tq).q_const; _temp4387.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4368->rgn))->v; _temp4387;}); _LL4349:;} _LL4321: { void*
_temp4388= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp4336->topt))->v); struct Cyc_List_List* _temp4400; struct Cyc_List_List*
_temp4402; struct Cyc_Absyn_Structdecl** _temp4404; struct Cyc_Absyn_Structdecl*
_temp4406; struct Cyc_Absyn_Uniondecl** _temp4407; struct Cyc_Absyn_Uniondecl*
_temp4409; _LL4390: if(( unsigned int) _temp4388 >  3u?*(( int*) _temp4388) == 
Cyc_Absyn_AnonStructType: 0){ _LL4401: _temp4400=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp4388)->f1; goto _LL4391;} else{ goto _LL4392;} _LL4392: if(( unsigned int)
_temp4388 >  3u?*(( int*) _temp4388) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4403:
_temp4402=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp4388)->f1; goto
_LL4393;} else{ goto _LL4394;} _LL4394: if(( unsigned int) _temp4388 >  3u?*((
int*) _temp4388) ==  Cyc_Absyn_StructType: 0){ _LL4405: _temp4404=(( struct Cyc_Absyn_StructType_struct*)
_temp4388)->f3; if( _temp4404 ==  0){ goto _LL4396;} else{ _temp4406=* _temp4404;
goto _LL4395;}} else{ goto _LL4396;} _LL4396: if(( unsigned int) _temp4388 >  3u?*((
int*) _temp4388) ==  Cyc_Absyn_UnionType: 0){ _LL4408: _temp4407=(( struct Cyc_Absyn_UnionType_struct*)
_temp4388)->f3; if( _temp4407 ==  0){ goto _LL4398;} else{ _temp4409=* _temp4407;
goto _LL4397;}} else{ goto _LL4398;} _LL4398: goto _LL4399; _LL4391: _temp4402=
_temp4400; goto _LL4393; _LL4393: { struct Cyc_Absyn_Structfield* _temp4410= Cyc_Absyn_lookup_field(
_temp4402, _temp4334); if( _temp4410 !=  0? _temp4410->width !=  0: 0){ return({
struct _tuple7 _temp4411; _temp4411.f1=( _temp4410->tq).q_const; _temp4411.f2=(
Cyc_Tcutil_addressof_props( te, _temp4336)).f2; _temp4411;});} return bogus_ans;}
_LL4395: { struct Cyc_Absyn_Structfield* _temp4412= Cyc_Absyn_lookup_struct_field(
_temp4406, _temp4334); if( _temp4412 !=  0? _temp4412->width !=  0: 0){ return({
struct _tuple7 _temp4413; _temp4413.f1=( _temp4412->tq).q_const; _temp4413.f2=(
Cyc_Tcutil_addressof_props( te, _temp4336)).f2; _temp4413;});} return bogus_ans;}
_LL4397: { struct Cyc_Absyn_Structfield* _temp4414= Cyc_Absyn_lookup_union_field(
_temp4409, _temp4334); if( _temp4414 !=  0){ return({ struct _tuple7 _temp4415;
_temp4415.f1=( _temp4414->tq).q_const; _temp4415.f2=( Cyc_Tcutil_addressof_props(
te, _temp4336)).f2; _temp4415;});} goto _LL4399;} _LL4399: return bogus_ans;
_LL4389:;} _LL4323: { void* _temp4416= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp4340->topt))->v); struct Cyc_Absyn_PtrInfo _temp4422; void*
_temp4424; void* _temp4426; _LL4418: if(( unsigned int) _temp4416 >  3u?*(( int*)
_temp4416) ==  Cyc_Absyn_PointerType: 0){ _LL4423: _temp4422=(( struct Cyc_Absyn_PointerType_struct*)
_temp4416)->f1; _LL4427: _temp4426=( void*) _temp4422.elt_typ; goto _LL4425;
_LL4425: _temp4424=( void*) _temp4422.rgn_typ; goto _LL4419;} else{ goto _LL4420;}
_LL4420: goto _LL4421; _LL4419: { void* _temp4428= Cyc_Tcutil_compress(
_temp4426); struct Cyc_List_List* _temp4440; struct Cyc_List_List* _temp4442;
struct Cyc_Absyn_Structdecl** _temp4444; struct Cyc_Absyn_Structdecl* _temp4446;
struct Cyc_Absyn_Uniondecl** _temp4447; struct Cyc_Absyn_Uniondecl* _temp4449;
_LL4430: if(( unsigned int) _temp4428 >  3u?*(( int*) _temp4428) ==  Cyc_Absyn_AnonStructType:
0){ _LL4441: _temp4440=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4428)->f1;
goto _LL4431;} else{ goto _LL4432;} _LL4432: if(( unsigned int) _temp4428 >  3u?*((
int*) _temp4428) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4443: _temp4442=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4428)->f1; goto _LL4433;} else{ goto
_LL4434;} _LL4434: if(( unsigned int) _temp4428 >  3u?*(( int*) _temp4428) == 
Cyc_Absyn_StructType: 0){ _LL4445: _temp4444=(( struct Cyc_Absyn_StructType_struct*)
_temp4428)->f3; if( _temp4444 ==  0){ goto _LL4436;} else{ _temp4446=* _temp4444;
goto _LL4435;}} else{ goto _LL4436;} _LL4436: if(( unsigned int) _temp4428 >  3u?*((
int*) _temp4428) ==  Cyc_Absyn_UnionType: 0){ _LL4448: _temp4447=(( struct Cyc_Absyn_UnionType_struct*)
_temp4428)->f3; if( _temp4447 ==  0){ goto _LL4438;} else{ _temp4449=* _temp4447;
goto _LL4437;}} else{ goto _LL4438;} _LL4438: goto _LL4439; _LL4431: _temp4442=
_temp4440; goto _LL4433; _LL4433: { struct Cyc_Absyn_Structfield* _temp4450= Cyc_Absyn_lookup_field(
_temp4442, _temp4338); if( _temp4450 !=  0? _temp4450->width !=  0: 0){ return({
struct _tuple7 _temp4451; _temp4451.f1=( _temp4450->tq).q_const; _temp4451.f2=
_temp4424; _temp4451;});} return bogus_ans;} _LL4435: { struct Cyc_Absyn_Structfield*
_temp4452= Cyc_Absyn_lookup_struct_field( _temp4446, _temp4338); if( _temp4452
!=  0? _temp4452->width !=  0: 0){ return({ struct _tuple7 _temp4453; _temp4453.f1=(
_temp4452->tq).q_const; _temp4453.f2= _temp4424; _temp4453;});} return bogus_ans;}
_LL4437: { struct Cyc_Absyn_Structfield* _temp4454= Cyc_Absyn_lookup_union_field(
_temp4449, _temp4338); if( _temp4454 !=  0){ return({ struct _tuple7 _temp4455;
_temp4455.f1=( _temp4454->tq).q_const; _temp4455.f2= _temp4424; _temp4455;});}
return bogus_ans;} _LL4439: return bogus_ans; _LL4429:;} _LL4421: return
bogus_ans; _LL4417:;} _LL4325: { void* _temp4456= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp4342->topt))->v); struct Cyc_Absyn_PtrInfo
_temp4462; struct Cyc_Absyn_Tqual _temp4464; void* _temp4466; _LL4458: if((
unsigned int) _temp4456 >  3u?*(( int*) _temp4456) ==  Cyc_Absyn_PointerType: 0){
_LL4463: _temp4462=(( struct Cyc_Absyn_PointerType_struct*) _temp4456)->f1;
_LL4467: _temp4466=( void*) _temp4462.rgn_typ; goto _LL4465; _LL4465: _temp4464=
_temp4462.tq; goto _LL4459;} else{ goto _LL4460;} _LL4460: goto _LL4461; _LL4459:
return({ struct _tuple7 _temp4468; _temp4468.f1= _temp4464.q_const; _temp4468.f2=
_temp4466; _temp4468;}); _LL4461: return bogus_ans; _LL4457:;} _LL4327: { void*
t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp4346->topt))->v);
void* _temp4469= t; struct Cyc_Absyn_Tqual _temp4479; struct Cyc_List_List*
_temp4481; struct Cyc_Absyn_PtrInfo _temp4483; struct Cyc_Absyn_Conref*
_temp4485; struct Cyc_Absyn_Tqual _temp4487; void* _temp4489; void* _temp4491;
_LL4471: if(( unsigned int) _temp4469 >  3u?*(( int*) _temp4469) ==  Cyc_Absyn_ArrayType:
0){ _LL4480: _temp4479=(( struct Cyc_Absyn_ArrayType_struct*) _temp4469)->f2;
goto _LL4472;} else{ goto _LL4473;} _LL4473: if(( unsigned int) _temp4469 >  3u?*((
int*) _temp4469) ==  Cyc_Absyn_TupleType: 0){ _LL4482: _temp4481=(( struct Cyc_Absyn_TupleType_struct*)
_temp4469)->f1; goto _LL4474;} else{ goto _LL4475;} _LL4475: if(( unsigned int)
_temp4469 >  3u?*(( int*) _temp4469) ==  Cyc_Absyn_PointerType: 0){ _LL4484:
_temp4483=(( struct Cyc_Absyn_PointerType_struct*) _temp4469)->f1; _LL4492:
_temp4491=( void*) _temp4483.elt_typ; goto _LL4490; _LL4490: _temp4489=( void*)
_temp4483.rgn_typ; goto _LL4488; _LL4488: _temp4487= _temp4483.tq; goto _LL4486;
_LL4486: _temp4485= _temp4483.bounds; goto _LL4476;} else{ goto _LL4477;}
_LL4477: goto _LL4478; _LL4472: return({ struct _tuple7 _temp4493; _temp4493.f1=
_temp4479.q_const; _temp4493.f2=( Cyc_Tcutil_addressof_props( te, _temp4346)).f2;
_temp4493;}); _LL4474: { struct _tuple4* _temp4494= Cyc_Absyn_lookup_tuple_field(
_temp4481,( int) Cyc_Evexp_eval_const_uint_exp( _temp4344)); if( _temp4494 !=  0){
return({ struct _tuple7 _temp4495; _temp4495.f1=((* _temp4494).f1).q_const;
_temp4495.f2=( Cyc_Tcutil_addressof_props( te, _temp4346)).f2; _temp4495;});}
return bogus_ans;} _LL4476: return({ struct _tuple7 _temp4496; _temp4496.f1=
_temp4487.q_const; _temp4496.f2= _temp4489; _temp4496;}); _LL4478: return
bogus_ans; _LL4470:;} _LL4329:({ void* _temp4497[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u), _tag_arr(
_temp4497, sizeof( void*), 0u));}); return bogus_ans; _LL4317:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp4499= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Tqual _temp4505; void*
_temp4507; _LL4501: if(( unsigned int) _temp4499 >  3u?*(( int*) _temp4499) == 
Cyc_Absyn_ArrayType: 0){ _LL4508: _temp4507=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp4499)->f1; goto _LL4506; _LL4506: _temp4505=(( struct Cyc_Absyn_ArrayType_struct*)
_temp4499)->f2; goto _LL4502;} else{ goto _LL4503;} _LL4503: goto _LL4504;
_LL4502: { void* _temp4511; struct _tuple7 _temp4509= Cyc_Tcutil_addressof_props(
te, e); _LL4512: _temp4511= _temp4509.f2; goto _LL4510; _LL4510: return Cyc_Absyn_atb_typ(
_temp4507, _temp4511, _temp4505,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp4513=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4513[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4514; _temp4514.tag= Cyc_Absyn_Upper_b;
_temp4514.f1= e; _temp4514;}); _temp4513;}));} _LL4504: return e_typ; _LL4500:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp4515=(
void*) b->v; void* _temp4523; void* _temp4525; struct Cyc_Absyn_Exp* _temp4527;
_LL4517: if(( unsigned int) _temp4515 >  1u?*(( int*) _temp4515) ==  Cyc_Absyn_Eq_constr:
0){ _LL4524: _temp4523=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4515)->f1;
if( _temp4523 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4518;} else{ goto
_LL4519;}} else{ goto _LL4519;} _LL4519: if(( unsigned int) _temp4515 >  1u?*((
int*) _temp4515) ==  Cyc_Absyn_Eq_constr: 0){ _LL4526: _temp4525=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp4515)->f1; if(( unsigned int) _temp4525
>  1u?*(( int*) _temp4525) ==  Cyc_Absyn_Upper_b: 0){ _LL4528: _temp4527=((
struct Cyc_Absyn_Upper_b_struct*) _temp4525)->f1; goto _LL4520;} else{ goto
_LL4521;}} else{ goto _LL4521;} _LL4521: goto _LL4522; _LL4518: return; _LL4520:
if( Cyc_Evexp_eval_const_uint_exp( _temp4527) <=  i){({ struct Cyc_Std_Int_pa_struct
_temp4531; _temp4531.tag= Cyc_Std_Int_pa; _temp4531.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp4530; _temp4530.tag= Cyc_Std_Int_pa; _temp4530.f1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp4527);{ void* _temp4529[ 2u]={& _temp4530,& _temp4531}; Cyc_Tcutil_terr(
loc, _tag_arr("dereference is out of bounds: %d <= %d", sizeof( unsigned char),
39u), _tag_arr( _temp4529, sizeof( void*), 2u));}}});} return; _LL4522:( void*)(
b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp4532=( struct
Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp4532[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp4533; _temp4533.tag= Cyc_Absyn_Eq_constr;
_temp4533.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp4534=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp4534[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp4535; _temp4535.tag= Cyc_Absyn_Upper_b;
_temp4535.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp4535;}); _temp4534;}));
_temp4533;}); _temp4532;}))); return; _LL4516:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b){ void*
_temp4536=( void*)( Cyc_Absyn_compress_conref( b))->v; void* _temp4542; struct
Cyc_Absyn_Exp* _temp4544; _LL4538: if(( unsigned int) _temp4536 >  1u?*(( int*)
_temp4536) ==  Cyc_Absyn_Eq_constr: 0){ _LL4543: _temp4542=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4536)->f1; if(( unsigned int) _temp4542 >  1u?*(( int*) _temp4542) ==  Cyc_Absyn_Upper_b:
0){ _LL4545: _temp4544=(( struct Cyc_Absyn_Upper_b_struct*) _temp4542)->f1; goto
_LL4539;} else{ goto _LL4540;}} else{ goto _LL4540;} _LL4540: goto _LL4541;
_LL4539: return Cyc_Evexp_eval_const_uint_exp( _temp4544) ==  1; _LL4541: return
0; _LL4537:;} int Cyc_Tcutil_bits_only( void* t){ void* _temp4546= Cyc_Tcutil_compress(
t); void* _temp4574; struct Cyc_List_List* _temp4576; struct Cyc_Absyn_Structdecl**
_temp4578; struct Cyc_List_List* _temp4580; struct Cyc_Absyn_Uniondecl**
_temp4582; struct Cyc_List_List* _temp4584; struct Cyc_List_List* _temp4586;
struct Cyc_List_List* _temp4588; _LL4548: if( _temp4546 == ( void*) Cyc_Absyn_VoidType){
goto _LL4549;} else{ goto _LL4550;} _LL4550: if(( unsigned int) _temp4546 >  3u?*((
int*) _temp4546) ==  Cyc_Absyn_IntType: 0){ goto _LL4551;} else{ goto _LL4552;}
_LL4552: if( _temp4546 == ( void*) Cyc_Absyn_FloatType){ goto _LL4553;} else{
goto _LL4554;} _LL4554: if(( unsigned int) _temp4546 >  3u?*(( int*) _temp4546)
==  Cyc_Absyn_DoubleType: 0){ goto _LL4555;} else{ goto _LL4556;} _LL4556: if((
unsigned int) _temp4546 >  3u?*(( int*) _temp4546) ==  Cyc_Absyn_EnumType: 0){
goto _LL4557;} else{ goto _LL4558;} _LL4558: if(( unsigned int) _temp4546 >  3u?*((
int*) _temp4546) ==  Cyc_Absyn_AnonEnumType: 0){ goto _LL4559;} else{ goto
_LL4560;} _LL4560: if(( unsigned int) _temp4546 >  3u?*(( int*) _temp4546) == 
Cyc_Absyn_ArrayType: 0){ _LL4575: _temp4574=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp4546)->f1; goto _LL4561;} else{ goto _LL4562;} _LL4562: if(( unsigned int)
_temp4546 >  3u?*(( int*) _temp4546) ==  Cyc_Absyn_TupleType: 0){ _LL4577:
_temp4576=(( struct Cyc_Absyn_TupleType_struct*) _temp4546)->f1; goto _LL4563;}
else{ goto _LL4564;} _LL4564: if(( unsigned int) _temp4546 >  3u?*(( int*)
_temp4546) ==  Cyc_Absyn_StructType: 0){ _LL4581: _temp4580=(( struct Cyc_Absyn_StructType_struct*)
_temp4546)->f2; goto _LL4579; _LL4579: _temp4578=(( struct Cyc_Absyn_StructType_struct*)
_temp4546)->f3; goto _LL4565;} else{ goto _LL4566;} _LL4566: if(( unsigned int)
_temp4546 >  3u?*(( int*) _temp4546) ==  Cyc_Absyn_UnionType: 0){ _LL4585:
_temp4584=(( struct Cyc_Absyn_UnionType_struct*) _temp4546)->f2; goto _LL4583;
_LL4583: _temp4582=(( struct Cyc_Absyn_UnionType_struct*) _temp4546)->f3; goto
_LL4567;} else{ goto _LL4568;} _LL4568: if(( unsigned int) _temp4546 >  3u?*((
int*) _temp4546) ==  Cyc_Absyn_AnonStructType: 0){ _LL4587: _temp4586=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp4546)->f1; goto _LL4569;} else{ goto
_LL4570;} _LL4570: if(( unsigned int) _temp4546 >  3u?*(( int*) _temp4546) == 
Cyc_Absyn_AnonUnionType: 0){ _LL4589: _temp4588=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp4546)->f1; goto _LL4571;} else{ goto _LL4572;} _LL4572: goto _LL4573;
_LL4549: goto _LL4551; _LL4551: goto _LL4553; _LL4553: goto _LL4555; _LL4555:
return 1; _LL4557: return 0; _LL4559: return 0; _LL4561: return Cyc_Tcutil_bits_only(
_temp4574); _LL4563: for( 0; _temp4576 !=  0; _temp4576= _temp4576->tl){ if( !
Cyc_Tcutil_bits_only((*(( struct _tuple4*) _temp4576->hd)).f2)){ return 0;}}
return 1; _LL4565: if( _temp4578 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp4590=* _temp4578; if( _temp4590->fields ==  0){ return 0;}{ struct
_RegionHandle _temp4591= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp4591; _push_region( rgn);{ struct Cyc_List_List* _temp4592=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp4590->tvs, _temp4580);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp4590->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp4592,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ int _temp4593= 0; _npop_handler( 0u); return _temp4593;}}}{
int _temp4594= 1; _npop_handler( 0u); return _temp4594;}}; _pop_region( rgn);}}
_LL4567: if( _temp4582 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* _temp4595=*
_temp4582; if( _temp4595->fields ==  0){ return 0;}{ struct _RegionHandle
_temp4596= _new_region("rgn"); struct _RegionHandle* rgn=& _temp4596;
_push_region( rgn);{ struct Cyc_List_List* _temp4597=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp4595->tvs, _temp4584);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp4595->fields))->v; for( 0; fs !=  0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp4597,( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type))){ int _temp4598= 0; _npop_handler( 0u); return _temp4598;}}}{
int _temp4599= 1; _npop_handler( 0u); return _temp4599;}}; _pop_region( rgn);}}
_LL4569: _temp4588= _temp4586; goto _LL4571; _LL4571: for( 0; _temp4588 !=  0;
_temp4588= _temp4588->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)
_temp4588->hd)->type)){ return 0;}} return 1; _LL4573: return 0; _LL4547:;}
struct _tuple21{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static
int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te, int var_okay, struct Cyc_Absyn_Exp*
e){ void* _temp4600=( void*) e->r; void* _temp4646; struct _tuple1* _temp4648;
struct Cyc_Absyn_Exp* _temp4650; struct Cyc_Absyn_Exp* _temp4652; struct Cyc_Absyn_Exp*
_temp4654; struct Cyc_Absyn_Exp* _temp4656; struct Cyc_Absyn_Exp* _temp4658;
struct Cyc_Absyn_Exp* _temp4660; struct Cyc_Absyn_Exp* _temp4662; struct Cyc_Absyn_Exp*
_temp4664; void* _temp4666; struct Cyc_Absyn_Exp* _temp4668; struct Cyc_Absyn_Exp*
_temp4670; struct Cyc_Absyn_Exp* _temp4672; struct Cyc_List_List* _temp4674;
struct Cyc_List_List* _temp4676; struct Cyc_List_List* _temp4678; struct Cyc_List_List*
_temp4680; void* _temp4682; struct Cyc_List_List* _temp4684; struct Cyc_List_List*
_temp4686; _LL4602: if(*(( int*) _temp4600) ==  Cyc_Absyn_Const_e){ goto _LL4603;}
else{ goto _LL4604;} _LL4604: if(*(( int*) _temp4600) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL4605;} else{ goto _LL4606;} _LL4606: if(*(( int*) _temp4600) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL4607;} else{ goto _LL4608;} _LL4608: if(*(( int*) _temp4600) ==  Cyc_Absyn_Offsetof_e){
goto _LL4609;} else{ goto _LL4610;} _LL4610: if(*(( int*) _temp4600) ==  Cyc_Absyn_Gentyp_e){
goto _LL4611;} else{ goto _LL4612;} _LL4612: if(*(( int*) _temp4600) ==  Cyc_Absyn_Enum_e){
goto _LL4613;} else{ goto _LL4614;} _LL4614: if(*(( int*) _temp4600) ==  Cyc_Absyn_AnonEnum_e){
goto _LL4615;} else{ goto _LL4616;} _LL4616: if(*(( int*) _temp4600) ==  Cyc_Absyn_Var_e){
_LL4649: _temp4648=(( struct Cyc_Absyn_Var_e_struct*) _temp4600)->f1; goto
_LL4647; _LL4647: _temp4646=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp4600)->f2;
goto _LL4617;} else{ goto _LL4618;} _LL4618: if(*(( int*) _temp4600) ==  Cyc_Absyn_Conditional_e){
_LL4655: _temp4654=(( struct Cyc_Absyn_Conditional_e_struct*) _temp4600)->f1;
goto _LL4653; _LL4653: _temp4652=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4600)->f2; goto _LL4651; _LL4651: _temp4650=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp4600)->f3; goto _LL4619;} else{ goto _LL4620;} _LL4620: if(*(( int*)
_temp4600) ==  Cyc_Absyn_SeqExp_e){ _LL4659: _temp4658=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4600)->f1; goto _LL4657; _LL4657: _temp4656=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp4600)->f2; goto _LL4621;} else{ goto _LL4622;} _LL4622: if(*(( int*)
_temp4600) ==  Cyc_Absyn_NoInstantiate_e){ _LL4661: _temp4660=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp4600)->f1; goto _LL4623;} else{ goto _LL4624;} _LL4624: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Instantiate_e){ _LL4663: _temp4662=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp4600)->f1; goto _LL4625;} else{ goto _LL4626;} _LL4626: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Cast_e){ _LL4667: _temp4666=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp4600)->f1; goto _LL4665; _LL4665: _temp4664=(( struct Cyc_Absyn_Cast_e_struct*)
_temp4600)->f2; goto _LL4627;} else{ goto _LL4628;} _LL4628: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Address_e){ _LL4669: _temp4668=(( struct Cyc_Absyn_Address_e_struct*)
_temp4600)->f1; goto _LL4629;} else{ goto _LL4630;} _LL4630: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Comprehension_e){ _LL4673: _temp4672=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4600)->f2; goto _LL4671; _LL4671: _temp4670=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp4600)->f3; goto _LL4631;} else{ goto _LL4632;} _LL4632: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Array_e){ _LL4675: _temp4674=(( struct Cyc_Absyn_Array_e_struct*)
_temp4600)->f1; goto _LL4633;} else{ goto _LL4634;} _LL4634: if(*(( int*)
_temp4600) ==  Cyc_Absyn_AnonStruct_e){ _LL4677: _temp4676=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp4600)->f2; goto _LL4635;} else{ goto _LL4636;} _LL4636: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Struct_e){ _LL4679: _temp4678=(( struct Cyc_Absyn_Struct_e_struct*)
_temp4600)->f3; goto _LL4637;} else{ goto _LL4638;} _LL4638: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Primop_e){ _LL4683: _temp4682=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp4600)->f1; goto _LL4681; _LL4681: _temp4680=(( struct Cyc_Absyn_Primop_e_struct*)
_temp4600)->f2; goto _LL4639;} else{ goto _LL4640;} _LL4640: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Tuple_e){ _LL4685: _temp4684=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp4600)->f1; goto _LL4641;} else{ goto _LL4642;} _LL4642: if(*(( int*)
_temp4600) ==  Cyc_Absyn_Tunion_e){ _LL4687: _temp4686=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp4600)->f3; goto _LL4643;} else{ goto _LL4644;} _LL4644: goto _LL4645;
_LL4603: return 1; _LL4605: return 1; _LL4607: return 1; _LL4609: return 1;
_LL4611: return 1; _LL4613: return 1; _LL4615: return 1; _LL4617: { void*
_temp4688= _temp4646; struct Cyc_Absyn_Vardecl* _temp4698; _LL4690: if((
unsigned int) _temp4688 >  1u?*(( int*) _temp4688) ==  Cyc_Absyn_Funname_b: 0){
goto _LL4691;} else{ goto _LL4692;} _LL4692: if(( unsigned int) _temp4688 >  1u?*((
int*) _temp4688) ==  Cyc_Absyn_Global_b: 0){ _LL4699: _temp4698=(( struct Cyc_Absyn_Global_b_struct*)
_temp4688)->f1; goto _LL4693;} else{ goto _LL4694;} _LL4694: if( _temp4688 == (
void*) Cyc_Absyn_Unresolved_b){ goto _LL4695;} else{ goto _LL4696;} _LL4696:
goto _LL4697; _LL4691: return 1; _LL4693: { void* _temp4700= Cyc_Tcutil_compress((
void*) _temp4698->type); _LL4702: if(( unsigned int) _temp4700 >  3u?*(( int*)
_temp4700) ==  Cyc_Absyn_ArrayType: 0){ goto _LL4703;} else{ goto _LL4704;}
_LL4704: if(( unsigned int) _temp4700 >  3u?*(( int*) _temp4700) ==  Cyc_Absyn_FnType:
0){ goto _LL4705;} else{ goto _LL4706;} _LL4706: goto _LL4707; _LL4703: return 1;
_LL4705: return 1; _LL4707: return var_okay; _LL4701:;} _LL4695: return 0;
_LL4697: return var_okay; _LL4689:;} _LL4619: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp4654)? Cyc_Tcutil_cnst_exp( te, 0, _temp4652): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp4650): 0; _LL4621: return Cyc_Tcutil_cnst_exp( te, 0, _temp4658)? Cyc_Tcutil_cnst_exp(
te, 0, _temp4656): 0; _LL4623: _temp4662= _temp4660; goto _LL4625; _LL4625:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp4662); _LL4627: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp4664); _LL4629: return Cyc_Tcutil_cnst_exp( te, 1, _temp4668);
_LL4631: return Cyc_Tcutil_cnst_exp( te, 0, _temp4672)? Cyc_Tcutil_cnst_exp( te,
0, _temp4670): 0; _LL4633: _temp4676= _temp4674; goto _LL4635; _LL4635:
_temp4678= _temp4676; goto _LL4637; _LL4637: for( 0; _temp4678 !=  0; _temp4678=
_temp4678->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple21*)
_temp4678->hd)).f2)){ return 0;}} return 1; _LL4639: _temp4684= _temp4680; goto
_LL4641; _LL4641: _temp4686= _temp4684; goto _LL4643; _LL4643: for( 0; _temp4686
!=  0; _temp4686= _temp4686->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp4686->hd)){ return 0;}} return 1; _LL4645: return 0; _LL4601:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp4708= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp4738; struct Cyc_Absyn_Conref* _temp4740; struct Cyc_Absyn_Conref*
_temp4742; void* _temp4744; struct Cyc_List_List* _temp4746; struct Cyc_Absyn_Structdecl**
_temp4748; struct Cyc_List_List* _temp4750; struct Cyc_Absyn_Uniondecl**
_temp4752; struct Cyc_List_List* _temp4754; struct Cyc_List_List* _temp4756;
struct Cyc_List_List* _temp4758; _LL4710: if( _temp4708 == ( void*) Cyc_Absyn_VoidType){
goto _LL4711;} else{ goto _LL4712;} _LL4712: if(( unsigned int) _temp4708 >  3u?*((
int*) _temp4708) ==  Cyc_Absyn_IntType: 0){ goto _LL4713;} else{ goto _LL4714;}
_LL4714: if( _temp4708 == ( void*) Cyc_Absyn_FloatType){ goto _LL4715;} else{
goto _LL4716;} _LL4716: if(( unsigned int) _temp4708 >  3u?*(( int*) _temp4708)
==  Cyc_Absyn_DoubleType: 0){ goto _LL4717;} else{ goto _LL4718;} _LL4718: if((
unsigned int) _temp4708 >  3u?*(( int*) _temp4708) ==  Cyc_Absyn_PointerType: 0){
_LL4739: _temp4738=(( struct Cyc_Absyn_PointerType_struct*) _temp4708)->f1;
_LL4743: _temp4742= _temp4738.nullable; goto _LL4741; _LL4741: _temp4740=
_temp4738.bounds; goto _LL4719;} else{ goto _LL4720;} _LL4720: if(( unsigned int)
_temp4708 >  3u?*(( int*) _temp4708) ==  Cyc_Absyn_ArrayType: 0){ _LL4745:
_temp4744=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp4708)->f1; goto
_LL4721;} else{ goto _LL4722;} _LL4722: if(( unsigned int) _temp4708 >  3u?*((
int*) _temp4708) ==  Cyc_Absyn_TupleType: 0){ _LL4747: _temp4746=(( struct Cyc_Absyn_TupleType_struct*)
_temp4708)->f1; goto _LL4723;} else{ goto _LL4724;} _LL4724: if(( unsigned int)
_temp4708 >  3u?*(( int*) _temp4708) ==  Cyc_Absyn_StructType: 0){ _LL4751:
_temp4750=(( struct Cyc_Absyn_StructType_struct*) _temp4708)->f2; goto _LL4749;
_LL4749: _temp4748=(( struct Cyc_Absyn_StructType_struct*) _temp4708)->f3; goto
_LL4725;} else{ goto _LL4726;} _LL4726: if(( unsigned int) _temp4708 >  3u?*((
int*) _temp4708) ==  Cyc_Absyn_UnionType: 0){ _LL4755: _temp4754=(( struct Cyc_Absyn_UnionType_struct*)
_temp4708)->f2; goto _LL4753; _LL4753: _temp4752=(( struct Cyc_Absyn_UnionType_struct*)
_temp4708)->f3; goto _LL4727;} else{ goto _LL4728;} _LL4728: if(( unsigned int)
_temp4708 >  3u?*(( int*) _temp4708) ==  Cyc_Absyn_AnonStructType: 0){ _LL4757:
_temp4756=(( struct Cyc_Absyn_AnonStructType_struct*) _temp4708)->f1; goto
_LL4729;} else{ goto _LL4730;} _LL4730: if(( unsigned int) _temp4708 >  3u?*((
int*) _temp4708) ==  Cyc_Absyn_AnonUnionType: 0){ _LL4759: _temp4758=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp4708)->f1; goto _LL4731;} else{ goto
_LL4732;} _LL4732: if(( unsigned int) _temp4708 >  3u?*(( int*) _temp4708) == 
Cyc_Absyn_AnonEnumType: 0){ goto _LL4733;} else{ goto _LL4734;} _LL4734: if((
unsigned int) _temp4708 >  3u?*(( int*) _temp4708) ==  Cyc_Absyn_EnumType: 0){
goto _LL4735;} else{ goto _LL4736;} _LL4736: goto _LL4737; _LL4711: goto _LL4713;
_LL4713: goto _LL4715; _LL4715: goto _LL4717; _LL4717: return 1; _LL4719: { void*
_temp4760=( void*)( Cyc_Absyn_compress_conref( _temp4740))->v; void* _temp4768;
void* _temp4770; _LL4762: if(( unsigned int) _temp4760 >  1u?*(( int*) _temp4760)
==  Cyc_Absyn_Eq_constr: 0){ _LL4769: _temp4768=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4760)->f1; if( _temp4768 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL4763;}
else{ goto _LL4764;}} else{ goto _LL4764;} _LL4764: if(( unsigned int) _temp4760
>  1u?*(( int*) _temp4760) ==  Cyc_Absyn_Eq_constr: 0){ _LL4771: _temp4770=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4760)->f1; if(( unsigned int)
_temp4770 >  1u?*(( int*) _temp4770) ==  Cyc_Absyn_Upper_b: 0){ goto _LL4765;}
else{ goto _LL4766;}} else{ goto _LL4766;} _LL4766: goto _LL4767; _LL4763:
return 1; _LL4765: { void* _temp4772=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp4742))->v; int
_temp4778; _LL4774: if(( unsigned int) _temp4772 >  1u?*(( int*) _temp4772) == 
Cyc_Absyn_Eq_constr: 0){ _LL4779: _temp4778=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4772)->f1; goto _LL4775;} else{ goto _LL4776;} _LL4776: goto _LL4777;
_LL4775: return _temp4778; _LL4777: return 0; _LL4773:;} _LL4767: return 0;
_LL4761:;} _LL4721: return Cyc_Tcutil_supports_default( _temp4744); _LL4723:
for( 0; _temp4746 !=  0; _temp4746= _temp4746->tl){ if( ! Cyc_Tcutil_supports_default((*((
struct _tuple4*) _temp4746->hd)).f2)){ return 0;}} return 1; _LL4725: if(
_temp4748 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=* _temp4748; if(
sd->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default( sd->tvs,
_temp4750,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);}
_LL4727: if( _temp4752 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=*
_temp4752; if( ud->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
ud->tvs, _temp4754,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
ud->fields))->v);} _LL4729: _temp4758= _temp4756; goto _LL4731; _LL4731: return
Cyc_Tcutil_fields_support_default( 0, 0, _temp4758); _LL4733: goto _LL4735;
_LL4735: return 1; _LL4737: return 0; _LL4709:;} static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List* tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{
struct _RegionHandle _temp4780= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp4780; _push_region( rgn);{ struct Cyc_List_List* _temp4781=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs !=  0;
fs= fs->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4781,( void*)(( struct
Cyc_Absyn_Structfield*) fs->hd)->type); if( ! Cyc_Tcutil_supports_default( t)){
int _temp4782= 0; _npop_handler( 0u); return _temp4782;}}}; _pop_region( rgn);}
return 1;}
