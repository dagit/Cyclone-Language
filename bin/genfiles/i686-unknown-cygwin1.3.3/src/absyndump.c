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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stdout; extern int Cyc_Std_fputc( int __c,
struct Cyc_Std___sFILE* __stream); extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_Std_file_string_write(
struct Cyc_Std___sFILE* fd, struct _tagged_arr src, int src_offset, int
max_count); static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern int Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
struct _tagged_arr Cyc_Position_get_line_directive( struct Cyc_Position_Segment*
loc); static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0;
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
15u]; extern void* Cyc_Absyn_compress_kb( void*); extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int generate_line_directives: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern int Cyc_Absynpp_is_anon_structtype( void*
t); extern struct _tagged_arr* Cyc_Absynpp_cyc_stringptr; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char); extern struct _tagged_arr Cyc_Absynpp_string_escape( struct
_tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str( void* p); extern
int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{ struct
Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); static int Cyc_Absyndump_expand_typedefs; static int Cyc_Absyndump_qvar_to_Cids;
static int Cyc_Absyndump_add_cyc_prefix; static int Cyc_Absyndump_generate_line_directives;
static int Cyc_Absyndump_to_VC; void Cyc_Absyndump_set_params( struct Cyc_Absynpp_Params*
fs){ Cyc_Absyndump_expand_typedefs= fs->expand_typedefs; Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids; Cyc_Absyndump_add_cyc_prefix= fs->add_cyc_prefix; Cyc_Absyndump_to_VC=
fs->to_VC; Cyc_Absyndump_generate_line_directives= fs->generate_line_directives;
Cyc_Absynpp_set_params( fs);} void Cyc_Absyndump_dumptyp( void*); void Cyc_Absyndump_dumpntyp(
void* t); void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumpexp_prec(
int, struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt*); void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*); void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms,
void(* f)( void*), void* a); void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual,
void*, void(* f)( void*), void*); void Cyc_Absyndump_dumpstructfields( struct
Cyc_List_List* fields); void Cyc_Absyndump_dumpenumfields( struct Cyc_List_List*
fields); void Cyc_Absyndump_dumploc( struct Cyc_Position_Segment*); struct Cyc_Std___sFILE**
Cyc_Absyndump_dump_file=& Cyc_Std_stdout; void Cyc_Absyndump_ignore( void* x){
return;} static unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump(
struct _tagged_arr s){ int sz=( int) _get_arr_size( s, sizeof( unsigned char));
if( !(( int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), sz -  1)))){ -- sz;} Cyc_Absyndump_pos += sz +  1; if( Cyc_Absyndump_pos
>  80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Std_fputc(( int)'\n',* Cyc_Absyndump_dump_file);}
else{ Cyc_Std_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Std_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dumploc( struct Cyc_Position_Segment*
loc){ if( loc ==  0){ return;} if( ! Cyc_Absyndump_generate_line_directives){
return;}{ struct _tagged_arr _temp0= Cyc_Position_get_line_directive( loc); Cyc_Absyndump_dump(
_temp0);}} void Cyc_Absyndump_dump_nospace( struct _tagged_arr s){ int sz=( int)
_get_arr_size( s, sizeof( unsigned char)); if( !(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), sz -  1)))){ -- sz;} Cyc_Absyndump_pos
+= sz; Cyc_Std_file_string_write(* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char(
int c){ ++ Cyc_Absyndump_pos; Cyc_Std_fputc( c,* Cyc_Absyndump_dump_file);} void
Cyc_Absyndump_dump_str( struct _tagged_arr* s){ Cyc_Absyndump_dump(* s);} void
Cyc_Absyndump_dump_semi(){ Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep(
void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr sep){ if( l ==  0){
return;} for( 0; l->tl !=  0; l= l->tl){ f(( void*) l->hd); Cyc_Absyndump_dump_nospace(
sep);} f(( void*) l->hd);} void Cyc_Absyndump_dump_sep_c( void(* f)( void*, void*),
void* env, struct Cyc_List_List* l, struct _tagged_arr sep){ if( l ==  0){
return;} for( 0; l->tl !=  0; l= l->tl){ f( env,( void*) l->hd); Cyc_Absyndump_dump_nospace(
sep);} f( env,( void*) l->hd);} void Cyc_Absyndump_group( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep){ Cyc_Absyndump_dump_nospace( start); Cyc_Absyndump_dump_sep(
f, l, sep); Cyc_Absyndump_dump_nospace( end);} void Cyc_Absyndump_group_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_arr start,
struct _tagged_arr end, struct _tagged_arr sep){ Cyc_Absyndump_dump_nospace(
start); Cyc_Absyndump_dump_sep_c( f, env, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep){ if( l
!=  0){ Cyc_Absyndump_group( f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar(
struct _tuple0* v){ struct Cyc_List_List* _temp1= 0;{ void* _temp2=(* v).f1;
struct Cyc_List_List* _temp10; struct Cyc_List_List* _temp12; _LL4: if( _temp2
== ( void*) Cyc_Absyn_Loc_n){ goto _LL5;} else{ goto _LL6;} _LL6: if((
unsigned int) _temp2 >  1u?*(( int*) _temp2) ==  Cyc_Absyn_Rel_n: 0){ _LL11:
_temp10=(( struct Cyc_Absyn_Rel_n_struct*) _temp2)->f1; goto _LL7;} else{ goto
_LL8;} _LL8: if(( unsigned int) _temp2 >  1u?*(( int*) _temp2) ==  Cyc_Absyn_Abs_n:
0){ _LL13: _temp12=(( struct Cyc_Absyn_Abs_n_struct*) _temp2)->f1; goto _LL9;}
else{ goto _LL3;} _LL5: _temp10= 0; goto _LL7; _LL7: _temp1= _temp10; goto _LL3;
_LL9: _temp1=( Cyc_Absyndump_qvar_to_Cids? Cyc_Absyndump_add_cyc_prefix: 0)?({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp14->tl=
_temp12; _temp14;}): _temp12; goto _LL3; _LL3:;} if( _temp1 !=  0){ Cyc_Absyndump_dump_str((
struct _tagged_arr*) _temp1->hd); for( _temp1= _temp1->tl; _temp1 !=  0; _temp1=
_temp1->tl){ if( Cyc_Absyndump_qvar_to_Cids){ Cyc_Absyndump_dump_char(( int)'_');}
else{ Cyc_Absyndump_dump_nospace( _tag_arr("::", sizeof( unsigned char), 3u));}
Cyc_Absyndump_dump_nospace(*(( struct _tagged_arr*) _temp1->hd));} if( Cyc_Absyndump_qvar_to_Cids){
Cyc_Absyndump_dump_nospace( _tag_arr("_", sizeof( unsigned char), 2u));} else{
Cyc_Absyndump_dump_nospace( _tag_arr("::", sizeof( unsigned char), 3u));} Cyc_Absyndump_dump_nospace(*(*
v).f2);} else{ Cyc_Absyndump_dump_str((* v).f2);}} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump( _tag_arr("restrict",
sizeof( unsigned char), 9u));} if( tq.q_volatile){ Cyc_Absyndump_dump( _tag_arr("volatile",
sizeof( unsigned char), 9u));} if( tq.q_const){ Cyc_Absyndump_dump( _tag_arr("const",
sizeof( unsigned char), 6u));}} void Cyc_Absyndump_dumpscope( void* sc){ void*
_temp15= sc; _LL17: if( _temp15 == ( void*) Cyc_Absyn_Static){ goto _LL18;}
else{ goto _LL19;} _LL19: if( _temp15 == ( void*) Cyc_Absyn_Public){ goto _LL20;}
else{ goto _LL21;} _LL21: if( _temp15 == ( void*) Cyc_Absyn_Extern){ goto _LL22;}
else{ goto _LL23;} _LL23: if( _temp15 == ( void*) Cyc_Absyn_ExternC){ goto _LL24;}
else{ goto _LL25;} _LL25: if( _temp15 == ( void*) Cyc_Absyn_Abstract){ goto
_LL26;} else{ goto _LL16;} _LL18: Cyc_Absyndump_dump( _tag_arr("static", sizeof(
unsigned char), 7u)); return; _LL20: return; _LL22: Cyc_Absyndump_dump( _tag_arr("extern",
sizeof( unsigned char), 7u)); return; _LL24: Cyc_Absyndump_dump( _tag_arr("extern \"C\"",
sizeof( unsigned char), 11u)); return; _LL26: Cyc_Absyndump_dump( _tag_arr("abstract",
sizeof( unsigned char), 9u)); return; _LL16:;} void Cyc_Absyndump_dumpkind( void*
k){ void* _temp27= k; _LL29: if( _temp27 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL30;} else{ goto _LL31;} _LL31: if( _temp27 == ( void*) Cyc_Absyn_MemKind){
goto _LL32;} else{ goto _LL33;} _LL33: if( _temp27 == ( void*) Cyc_Absyn_BoxKind){
goto _LL34;} else{ goto _LL35;} _LL35: if( _temp27 == ( void*) Cyc_Absyn_RgnKind){
goto _LL36;} else{ goto _LL37;} _LL37: if( _temp27 == ( void*) Cyc_Absyn_EffKind){
goto _LL38;} else{ goto _LL28;} _LL30: Cyc_Absyndump_dump( _tag_arr("A", sizeof(
unsigned char), 2u)); return; _LL32: Cyc_Absyndump_dump( _tag_arr("M", sizeof(
unsigned char), 2u)); return; _LL34: Cyc_Absyndump_dump( _tag_arr("B", sizeof(
unsigned char), 2u)); return; _LL36: Cyc_Absyndump_dump( _tag_arr("R", sizeof(
unsigned char), 2u)); return; _LL38: Cyc_Absyndump_dump( _tag_arr("E", sizeof(
unsigned char), 2u)); return; _LL28:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List*
ts){ Cyc_Absyndump_egroup( Cyc_Absyndump_dumptyp, ts, _tag_arr("<", sizeof(
unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar*
tv){ Cyc_Absyndump_dump_str( tv->name);} void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);{ void* _temp39=
Cyc_Absyn_compress_kb(( void*) tv->kind); void* _temp49; void* _temp51; _LL41:
if(*(( int*) _temp39) ==  Cyc_Absyn_Eq_kb){ _LL50: _temp49=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp39)->f1; if( _temp49 == ( void*) Cyc_Absyn_BoxKind){ goto _LL42;} else{
goto _LL43;}} else{ goto _LL43;} _LL43: if(*(( int*) _temp39) ==  Cyc_Absyn_Less_kb){
goto _LL44;} else{ goto _LL45;} _LL45: if(*(( int*) _temp39) ==  Cyc_Absyn_Unknown_kb){
goto _LL46;} else{ goto _LL47;} _LL47: if(*(( int*) _temp39) ==  Cyc_Absyn_Eq_kb){
_LL52: _temp51=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp39)->f1; goto
_LL48;} else{ goto _LL40;} _LL42: goto _LL44; _LL44: goto _LL46; _LL46: Cyc_Absyndump_dump(
_tag_arr("::?", sizeof( unsigned char), 4u)); goto _LL40; _LL48: Cyc_Absyndump_dump(
_tag_arr("::", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpkind( _temp51);
goto _LL40; _LL40:;}} void Cyc_Absyndump_dumptvars( struct Cyc_List_List* tvs){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar, tvs, _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} void
Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List* tvs){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct _tagged_arr start,
struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar,
tvs, _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} struct _tuple5{
struct Cyc_Absyn_Tqual f1; void* f2; } ; void Cyc_Absyndump_dumparg( struct
_tuple5* pr){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int))
Cyc_Absyndump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)(
struct _tuple5*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg,
ts, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts= atts->tl){ void* _temp53=(
void*) atts->hd; _LL55: if( _temp53 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL56;} else{ goto _LL57;} _LL57: if( _temp53 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL58;} else{ goto _LL59;} _LL59: if( _temp53 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL60;} else{ goto _LL61;} _LL61: goto _LL62; _LL56: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); return; _LL58: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); return; _LL60: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); return; _LL62: goto _LL54;
_LL54:;}} void Cyc_Absyndump_dump_noncallconv( struct Cyc_List_List* atts){ int
hasatt= 0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 !=  0; atts2= atts2->tl){
void* _temp63=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL65:
if( _temp63 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL66;} else{ goto _LL67;}
_LL67: if( _temp63 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL68;} else{ goto
_LL69;} _LL69: if( _temp63 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL70;}
else{ goto _LL71;} _LL71: goto _LL72; _LL66: goto _LL64; _LL68: goto _LL64;
_LL70: goto _LL64; _LL72: hasatt= 1; goto _LL64; _LL64:;}} if( ! hasatt){
return;} Cyc_Absyndump_dump( _tag_arr("__declspec(", sizeof( unsigned char), 12u));
for( 0; atts !=  0; atts= atts->tl){ void* _temp73=( void*) atts->hd; _LL75: if(
_temp73 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL76;} else{ goto _LL77;}
_LL77: if( _temp73 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL78;} else{ goto
_LL79;} _LL79: if( _temp73 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL80;}
else{ goto _LL81;} _LL81: goto _LL82; _LL76: goto _LL74; _LL78: goto _LL74;
_LL80: goto _LL74; _LL82: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)
atts->hd)); goto _LL74; _LL74:;} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumpatts(
struct Cyc_List_List* atts){ if( atts ==  0){ return;} if( Cyc_Absyndump_to_VC){
Cyc_Absyndump_dump_noncallconv( atts); return;} Cyc_Absyndump_dump( _tag_arr(" __attribute__((",
sizeof( unsigned char), 17u)); for( 0; atts !=  0; atts= atts->tl){ Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string(( void*) atts->hd)); if( atts->tl !=  0){ Cyc_Absyndump_dump(
_tag_arr(",", sizeof( unsigned char), 2u));}} Cyc_Absyndump_dump( _tag_arr(")) ",
sizeof( unsigned char), 4u));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms ==  0){ return 0;}{ void* _temp83=( void*) tms->hd; _LL85: if((
unsigned int) _temp83 >  1u?*(( int*) _temp83) ==  Cyc_Absyn_Pointer_mod: 0){
goto _LL86;} else{ goto _LL87;} _LL87: goto _LL88; _LL86: return 1; _LL88:
return 0; _LL84:;}} static void Cyc_Absyndump_dumprgn( void* t){ void* _temp89=
Cyc_Tcutil_compress( t); _LL91: if( _temp89 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL92: Cyc_Absyndump_dump(
_tag_arr("`H", sizeof( unsigned char), 3u)); goto _LL90; _LL94: Cyc_Absyndump_dumpntyp(
t); goto _LL90; _LL90:;} struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
rgions= 0; struct Cyc_List_List* effects= 0;{ void* _temp95= Cyc_Tcutil_compress(
t); void* _temp103; struct Cyc_List_List* _temp105; _LL97: if(( unsigned int)
_temp95 >  3u?*(( int*) _temp95) ==  Cyc_Absyn_AccessEff: 0){ _LL104: _temp103=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp95)->f1; goto _LL98;} else{
goto _LL99;} _LL99: if(( unsigned int) _temp95 >  3u?*(( int*) _temp95) ==  Cyc_Absyn_JoinEff:
0){ _LL106: _temp105=(( struct Cyc_Absyn_JoinEff_struct*) _temp95)->f1; goto
_LL100;} else{ goto _LL101;} _LL101: goto _LL102; _LL98: rgions=({ struct Cyc_List_List*
_temp107=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp107->hd=( void*) _temp103; _temp107->tl= rgions; _temp107;}); goto _LL96;
_LL100: for( 0; _temp105 !=  0; _temp105= _temp105->tl){ struct Cyc_List_List*
_temp110; struct Cyc_List_List* _temp112; struct _tuple6 _temp108= Cyc_Absyndump_effects_split((
void*) _temp105->hd); _LL113: _temp112= _temp108.f1; goto _LL111; _LL111:
_temp110= _temp108.f2; goto _LL109; _LL109: rgions= Cyc_List_imp_append(
_temp112, rgions); effects= Cyc_List_imp_append( _temp110, effects);} goto _LL96;
_LL102: effects=({ struct Cyc_List_List* _temp114=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp114->hd=( void*) t; _temp114->tl=
effects; _temp114;}); goto _LL96; _LL96:;} return({ struct _tuple6 _temp115;
_temp115.f1= rgions; _temp115.f2= effects; _temp115;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp118; struct Cyc_List_List* _temp120; struct
_tuple6 _temp116= Cyc_Absyndump_effects_split( t); _LL121: _temp120= _temp116.f1;
goto _LL119; _LL119: _temp118= _temp116.f2; goto _LL117; _LL117: _temp120= Cyc_List_imp_rev(
_temp120); _temp118= Cyc_List_imp_rev( _temp118); for( 0; _temp118 !=  0;
_temp118= _temp118->tl){ Cyc_Absyndump_dumpntyp(( void*) _temp118->hd); Cyc_Absyndump_dump_char((
int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp120 !=  0; _temp120=
_temp120->tl){ Cyc_Absyndump_dumprgn(( void*) _temp120->hd); if( _temp120->tl != 
0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');}
void Cyc_Absyndump_dumpntyp( void* t){ void* _temp122= t; struct Cyc_Absyn_Tvar*
_temp198; int _temp200; struct Cyc_Core_Opt* _temp202; struct Cyc_Core_Opt*
_temp204; int _temp206; struct Cyc_Core_Opt* _temp208; struct Cyc_Core_Opt
_temp210; void* _temp211; struct Cyc_Core_Opt* _temp213; struct Cyc_Absyn_TunionInfo
_temp215; void* _temp217; struct Cyc_List_List* _temp219; void* _temp221; struct
Cyc_Absyn_TunionFieldInfo _temp223; struct Cyc_List_List* _temp225; void*
_temp227; struct _tuple0* _temp229; void* _temp231; void* _temp233; void*
_temp235; void* _temp237; void* _temp239; void* _temp241; void* _temp243; void*
_temp245; void* _temp247; void* _temp249; void* _temp251; void* _temp253; void*
_temp256; void* _temp258; void* _temp260; void* _temp262; void* _temp265; void*
_temp267; void* _temp269; void* _temp271; int _temp273; struct Cyc_List_List*
_temp275; struct Cyc_List_List* _temp277; struct _tuple0* _temp279; struct Cyc_List_List*
_temp281; struct _tuple0* _temp283; struct Cyc_List_List* _temp285; struct
_tuple0* _temp287; struct Cyc_List_List* _temp289; struct _tuple0* _temp291;
struct Cyc_List_List* _temp293; struct Cyc_List_List* _temp295; struct Cyc_List_List*
_temp297; struct Cyc_Core_Opt* _temp299; struct Cyc_List_List* _temp301; struct
_tuple0* _temp303; void* _temp305; void* _temp307; _LL124: if(( unsigned int)
_temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_ArrayType: 0){ goto _LL125;}
else{ goto _LL126;} _LL126: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122)
==  Cyc_Absyn_FnType: 0){ goto _LL127;} else{ goto _LL128;} _LL128: if((
unsigned int) _temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_PointerType: 0){
goto _LL129;} else{ goto _LL130;} _LL130: if( _temp122 == ( void*) Cyc_Absyn_VoidType){
goto _LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp122 >  3u?*((
int*) _temp122) ==  Cyc_Absyn_VarType: 0){ _LL199: _temp198=(( struct Cyc_Absyn_VarType_struct*)
_temp122)->f1; goto _LL133;} else{ goto _LL134;} _LL134: if(( unsigned int)
_temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_Evar: 0){ _LL205: _temp204=((
struct Cyc_Absyn_Evar_struct*) _temp122)->f1; goto _LL203; _LL203: _temp202=((
struct Cyc_Absyn_Evar_struct*) _temp122)->f2; if( _temp202 ==  0){ goto _LL201;}
else{ goto _LL136;} _LL201: _temp200=(( struct Cyc_Absyn_Evar_struct*) _temp122)->f3;
goto _LL135;} else{ goto _LL136;} _LL136: if(( unsigned int) _temp122 >  3u?*((
int*) _temp122) ==  Cyc_Absyn_Evar: 0){ _LL214: _temp213=(( struct Cyc_Absyn_Evar_struct*)
_temp122)->f1; goto _LL209; _LL209: _temp208=(( struct Cyc_Absyn_Evar_struct*)
_temp122)->f2; if( _temp208 ==  0){ goto _LL138;} else{ _temp210=* _temp208;
_LL212: _temp211=( void*) _temp210.v; goto _LL207;} _LL207: _temp206=(( struct
Cyc_Absyn_Evar_struct*) _temp122)->f3; goto _LL137;} else{ goto _LL138;} _LL138:
if(( unsigned int) _temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_TunionType:
0){ _LL216: _temp215=(( struct Cyc_Absyn_TunionType_struct*) _temp122)->f1;
_LL222: _temp221=( void*) _temp215.tunion_info; goto _LL220; _LL220: _temp219=
_temp215.targs; goto _LL218; _LL218: _temp217=( void*) _temp215.rgn; goto _LL139;}
else{ goto _LL140;} _LL140: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122)
==  Cyc_Absyn_TunionFieldType: 0){ _LL224: _temp223=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp122)->f1; _LL228: _temp227=( void*) _temp223.field_info; goto _LL226;
_LL226: _temp225= _temp223.targs; goto _LL141;} else{ goto _LL142;} _LL142: if((
unsigned int) _temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_EnumType: 0){
_LL230: _temp229=(( struct Cyc_Absyn_EnumType_struct*) _temp122)->f1; goto
_LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL234: _temp233=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp233 == ( void*) Cyc_Absyn_Signed){ goto _LL232;} else{
goto _LL146;} _LL232: _temp231=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp231 == ( void*) Cyc_Absyn_B4){ goto _LL145;} else{ goto
_LL146;}} else{ goto _LL146;} _LL146: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL238: _temp237=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp237 == ( void*) Cyc_Absyn_Signed){ goto _LL236;} else{
goto _LL148;} _LL236: _temp235=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp235 == ( void*) Cyc_Absyn_B1){ goto _LL147;} else{ goto
_LL148;}} else{ goto _LL148;} _LL148: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL242: _temp241=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp241 == ( void*) Cyc_Absyn_Unsigned){ goto _LL240;} else{
goto _LL150;} _LL240: _temp239=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp239 == ( void*) Cyc_Absyn_B1){ goto _LL149;} else{ goto
_LL150;}} else{ goto _LL150;} _LL150: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL246: _temp245=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp245 == ( void*) Cyc_Absyn_Signed){ goto _LL244;} else{
goto _LL152;} _LL244: _temp243=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp243 == ( void*) Cyc_Absyn_B2){ goto _LL151;} else{ goto
_LL152;}} else{ goto _LL152;} _LL152: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL250: _temp249=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp249 == ( void*) Cyc_Absyn_Unsigned){ goto _LL248;} else{
goto _LL154;} _LL248: _temp247=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp247 == ( void*) Cyc_Absyn_B2){ goto _LL153;} else{ goto
_LL154;}} else{ goto _LL154;} _LL154: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL254: _temp253=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp253 == ( void*) Cyc_Absyn_Unsigned){ goto _LL252;} else{
goto _LL156;} _LL252: _temp251=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp251 == ( void*) Cyc_Absyn_B4){ goto _LL155;} else{ goto
_LL156;}} else{ goto _LL156;} _LL156: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL259: _temp258=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp258 == ( void*) Cyc_Absyn_Signed){ goto _LL257;} else{
goto _LL158;} _LL257: _temp256=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp256 == ( void*) Cyc_Absyn_B8){ goto _LL255;} else{ goto
_LL158;}} else{ goto _LL158;} _LL255: if( Cyc_Absyndump_to_VC){ goto _LL157;}
else{ goto _LL158;} _LL158: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122)
==  Cyc_Absyn_IntType: 0){ _LL263: _temp262=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp262 == ( void*) Cyc_Absyn_Signed){ goto _LL261;} else{
goto _LL160;} _LL261: _temp260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp260 == ( void*) Cyc_Absyn_B8){ goto _LL159;} else{ goto
_LL160;}} else{ goto _LL160;} _LL160: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_IntType: 0){ _LL268: _temp267=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp267 == ( void*) Cyc_Absyn_Unsigned){ goto _LL266;} else{
goto _LL162;} _LL266: _temp265=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp265 == ( void*) Cyc_Absyn_B8){ goto _LL264;} else{ goto
_LL162;}} else{ goto _LL162;} _LL264: if( Cyc_Absyndump_to_VC){ goto _LL161;}
else{ goto _LL162;} _LL162: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122)
==  Cyc_Absyn_IntType: 0){ _LL272: _temp271=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f1; if( _temp271 == ( void*) Cyc_Absyn_Unsigned){ goto _LL270;} else{
goto _LL164;} _LL270: _temp269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp122)->f2; if( _temp269 == ( void*) Cyc_Absyn_B8){ goto _LL163;} else{ goto
_LL164;}} else{ goto _LL164;} _LL164: if( _temp122 == ( void*) Cyc_Absyn_FloatType){
goto _LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp122 >  3u?*((
int*) _temp122) ==  Cyc_Absyn_DoubleType: 0){ _LL274: _temp273=(( struct Cyc_Absyn_DoubleType_struct*)
_temp122)->f1; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_TupleType: 0){ _LL276: _temp275=((
struct Cyc_Absyn_TupleType_struct*) _temp122)->f1; goto _LL169;} else{ goto
_LL170;} _LL170: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_StructType:
0){ _LL280: _temp279=(( struct Cyc_Absyn_StructType_struct*) _temp122)->f1; if(
_temp279 ==  0){ goto _LL278;} else{ goto _LL172;} _LL278: _temp277=(( struct
Cyc_Absyn_StructType_struct*) _temp122)->f2; goto _LL171;} else{ goto _LL172;}
_LL172: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_StructType:
0){ _LL284: _temp283=(( struct Cyc_Absyn_StructType_struct*) _temp122)->f1; goto
_LL282; _LL282: _temp281=(( struct Cyc_Absyn_StructType_struct*) _temp122)->f2;
goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int) _temp122 >  3u?*((
int*) _temp122) ==  Cyc_Absyn_UnionType: 0){ _LL288: _temp287=(( struct Cyc_Absyn_UnionType_struct*)
_temp122)->f1; if( _temp287 ==  0){ goto _LL286;} else{ goto _LL176;} _LL286:
_temp285=(( struct Cyc_Absyn_UnionType_struct*) _temp122)->f2; goto _LL175;}
else{ goto _LL176;} _LL176: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122)
==  Cyc_Absyn_UnionType: 0){ _LL292: _temp291=(( struct Cyc_Absyn_UnionType_struct*)
_temp122)->f1; goto _LL290; _LL290: _temp289=(( struct Cyc_Absyn_UnionType_struct*)
_temp122)->f2; goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int)
_temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_AnonStructType: 0){ _LL294:
_temp293=(( struct Cyc_Absyn_AnonStructType_struct*) _temp122)->f1; goto _LL179;}
else{ goto _LL180;} _LL180: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122)
==  Cyc_Absyn_AnonUnionType: 0){ _LL296: _temp295=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp122)->f1; goto _LL181;} else{ goto _LL182;} _LL182: if(( unsigned int)
_temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_AnonEnumType: 0){ _LL298:
_temp297=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp122)->f1; goto _LL183;}
else{ goto _LL184;} _LL184: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122)
==  Cyc_Absyn_TypedefType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_TypedefType_struct*)
_temp122)->f1; goto _LL302; _LL302: _temp301=(( struct Cyc_Absyn_TypedefType_struct*)
_temp122)->f2; goto _LL300; _LL300: _temp299=(( struct Cyc_Absyn_TypedefType_struct*)
_temp122)->f3; goto _LL185;} else{ goto _LL186;} _LL186: if(( unsigned int)
_temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_SizeofType: 0){ _LL306:
_temp305=( void*)(( struct Cyc_Absyn_SizeofType_struct*) _temp122)->f1; goto
_LL187;} else{ goto _LL188;} _LL188: if(( unsigned int) _temp122 >  3u?*(( int*)
_temp122) ==  Cyc_Absyn_RgnHandleType: 0){ _LL308: _temp307=( void*)(( struct
Cyc_Absyn_RgnHandleType_struct*) _temp122)->f1; goto _LL189;} else{ goto _LL190;}
_LL190: if( _temp122 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL191;} else{ goto
_LL192;} _LL192: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_AccessEff:
0){ goto _LL193;} else{ goto _LL194;} _LL194: if(( unsigned int) _temp122 >  3u?*((
int*) _temp122) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL195;} else{ goto _LL196;}
_LL196: if(( unsigned int) _temp122 >  3u?*(( int*) _temp122) ==  Cyc_Absyn_JoinEff:
0){ goto _LL197;} else{ goto _LL123;} _LL125: return; _LL127: return; _LL129:
return; _LL131: Cyc_Absyndump_dump( _tag_arr("void", sizeof( unsigned char), 5u));
return; _LL133: Cyc_Absyndump_dump_str( _temp198->name); return; _LL135: Cyc_Absyndump_dump(
_tag_arr("%", sizeof( unsigned char), 2u)); if( _temp204 ==  0){ Cyc_Absyndump_dump(
_tag_arr("?", sizeof( unsigned char), 2u));} else{ Cyc_Absyndump_dumpkind(( void*)
_temp204->v);} Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp310; _temp310.tag= Cyc_Std_Int_pa; _temp310.f1=( int)(( unsigned int)
_temp200);{ void* _temp309[ 1u]={& _temp310}; Cyc_Std_aprintf( _tag_arr("(%d)",
sizeof( unsigned char), 5u), _tag_arr( _temp309, sizeof( void*), 1u));}}));
return; _LL137: Cyc_Absyndump_dumpntyp( _temp211); return; _LL139:{ void*
_temp311= _temp221; struct Cyc_Absyn_UnknownTunionInfo _temp317; int _temp319;
struct _tuple0* _temp321; struct Cyc_Absyn_Tuniondecl** _temp323; struct Cyc_Absyn_Tuniondecl*
_temp325; struct Cyc_Absyn_Tuniondecl _temp326; int _temp327; struct _tuple0*
_temp329; _LL313: if(*(( int*) _temp311) ==  Cyc_Absyn_UnknownTunion){ _LL318:
_temp317=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp311)->f1; _LL322:
_temp321= _temp317.name; goto _LL320; _LL320: _temp319= _temp317.is_xtunion;
goto _LL314;} else{ goto _LL315;} _LL315: if(*(( int*) _temp311) ==  Cyc_Absyn_KnownTunion){
_LL324: _temp323=(( struct Cyc_Absyn_KnownTunion_struct*) _temp311)->f1;
_temp325=* _temp323; _temp326=* _temp325; _LL330: _temp329= _temp326.name; goto
_LL328; _LL328: _temp327= _temp326.is_xtunion; goto _LL316;} else{ goto _LL312;}
_LL314: _temp329= _temp321; _temp327= _temp319; goto _LL316; _LL316: if(
_temp327){ Cyc_Absyndump_dump( _tag_arr("xtunion ", sizeof( unsigned char), 9u));}
else{ Cyc_Absyndump_dump( _tag_arr("tunion ", sizeof( unsigned char), 8u));}{
void* _temp331= Cyc_Tcutil_compress( _temp217); _LL333: if( _temp331 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL334;} else{ goto _LL335;} _LL335: goto _LL336;
_LL334: goto _LL332; _LL336: Cyc_Absyndump_dumptyp( _temp217); Cyc_Absyndump_dump(
_tag_arr(" ", sizeof( unsigned char), 2u)); goto _LL332; _LL332:;} Cyc_Absyndump_dumpqvar(
_temp329); Cyc_Absyndump_dumptps( _temp219); goto _LL312; _LL312:;} goto _LL123;
_LL141:{ void* _temp337= _temp227; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp343; int _temp345; struct _tuple0* _temp347; struct _tuple0* _temp349;
struct Cyc_Absyn_Tunionfield* _temp351; struct Cyc_Absyn_Tunionfield _temp353;
struct _tuple0* _temp354; struct Cyc_Absyn_Tuniondecl* _temp356; struct Cyc_Absyn_Tuniondecl
_temp358; int _temp359; struct _tuple0* _temp361; _LL339: if(*(( int*) _temp337)
==  Cyc_Absyn_UnknownTunionfield){ _LL344: _temp343=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp337)->f1; _LL350: _temp349= _temp343.tunion_name; goto _LL348; _LL348:
_temp347= _temp343.field_name; goto _LL346; _LL346: _temp345= _temp343.is_xtunion;
goto _LL340;} else{ goto _LL341;} _LL341: if(*(( int*) _temp337) ==  Cyc_Absyn_KnownTunionfield){
_LL357: _temp356=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp337)->f1;
_temp358=* _temp356; _LL362: _temp361= _temp358.name; goto _LL360; _LL360:
_temp359= _temp358.is_xtunion; goto _LL352; _LL352: _temp351=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp337)->f2; _temp353=* _temp351; _LL355: _temp354= _temp353.name; goto _LL342;}
else{ goto _LL338;} _LL340: _temp361= _temp349; _temp359= _temp345; _temp354=
_temp347; goto _LL342; _LL342: if( _temp359){ Cyc_Absyndump_dump( _tag_arr("xtunion ",
sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("tunion ",
sizeof( unsigned char), 8u));} Cyc_Absyndump_dumpqvar( _temp361); Cyc_Absyndump_dump(
_tag_arr(".", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar( _temp354);
Cyc_Absyndump_dumptps( _temp225); goto _LL338; _LL338:;} goto _LL123; _LL143:
Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp229); return; _LL145: Cyc_Absyndump_dump( _tag_arr("int", sizeof(
unsigned char), 4u)); return; _LL147: Cyc_Absyndump_dump( _tag_arr("signed char",
sizeof( unsigned char), 12u)); return; _LL149: Cyc_Absyndump_dump( _tag_arr("unsigned char",
sizeof( unsigned char), 14u)); return; _LL151: Cyc_Absyndump_dump( _tag_arr("short",
sizeof( unsigned char), 6u)); return; _LL153: Cyc_Absyndump_dump( _tag_arr("unsigned short",
sizeof( unsigned char), 15u)); return; _LL155: Cyc_Absyndump_dump( _tag_arr("unsigned int",
sizeof( unsigned char), 13u)); return; _LL157: Cyc_Absyndump_dump( _tag_arr("__int64",
sizeof( unsigned char), 8u)); return; _LL159: Cyc_Absyndump_dump( _tag_arr("long long",
sizeof( unsigned char), 10u)); return; _LL161: Cyc_Absyndump_dump( _tag_arr("unsigned __int64",
sizeof( unsigned char), 17u)); return; _LL163: Cyc_Absyndump_dump( _tag_arr("unsigned long long",
sizeof( unsigned char), 19u)); return; _LL165: Cyc_Absyndump_dump( _tag_arr("float",
sizeof( unsigned char), 6u)); return; _LL167: if( _temp273){ Cyc_Absyndump_dump(
_tag_arr("long double", sizeof( unsigned char), 12u));} else{ Cyc_Absyndump_dump(
_tag_arr("double", sizeof( unsigned char), 7u));} return; _LL169: Cyc_Absyndump_dump_char((
int)'$'); Cyc_Absyndump_dumpargs( _temp275); return; _LL171: Cyc_Absyndump_dump(
_tag_arr("struct", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumptps( _temp277);
return; _LL173: Cyc_Absyndump_dump( _tag_arr("struct", sizeof( unsigned char), 7u));
Cyc_Absyndump_dumpqvar(( struct _tuple0*) _check_null( _temp283)); Cyc_Absyndump_dumptps(
_temp281); return; _LL175: Cyc_Absyndump_dump( _tag_arr("union", sizeof(
unsigned char), 6u)); Cyc_Absyndump_dumptps( _temp285); return; _LL177: Cyc_Absyndump_dump(
_tag_arr("union", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(( struct
_tuple0*) _check_null( _temp291)); Cyc_Absyndump_dumptps( _temp289); return;
_LL179: Cyc_Absyndump_dump( _tag_arr("struct {", sizeof( unsigned char), 9u));
Cyc_Absyndump_dumpstructfields( _temp293); Cyc_Absyndump_dump( _tag_arr("}",
sizeof( unsigned char), 2u)); return; _LL181: Cyc_Absyndump_dump( _tag_arr("union {",
sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpstructfields( _temp295); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); return; _LL183: Cyc_Absyndump_dump(
_tag_arr("enum {", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpenumfields(
_temp297); Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u));
return; _LL185:( Cyc_Absyndump_dumpqvar( _temp303), Cyc_Absyndump_dumptps(
_temp301)); return; _LL187: Cyc_Absyndump_dump( _tag_arr("sizeof_t<", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumpntyp( _temp305); Cyc_Absyndump_dump(
_tag_arr(">", sizeof( unsigned char), 2u)); return; _LL189: Cyc_Absyndump_dumprgn(
_temp307); return; _LL191: return; _LL193: return; _LL195: return; _LL197:
return; _LL123:;} void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if(
vo !=  0){ Cyc_Absyndump_dump_str(( struct _tagged_arr*) vo->v);}} void Cyc_Absyndump_dumpfunarg(
struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3,
Cyc_Absyndump_dumpvaropt,(* t).f1);} struct _tuple7{ void* f1; void* f2; } ;
void Cyc_Absyndump_dump_rgncmp( struct _tuple7* cmp){ struct _tuple7 _temp365;
void* _temp366; void* _temp368; struct _tuple7* _temp363= cmp; _temp365=*
_temp363; _LL369: _temp368= _temp365.f1; goto _LL367; _LL367: _temp366= _temp365.f2;
goto _LL364; _LL364: Cyc_Absyndump_dumptyp( _temp368); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumptyp( _temp366);} void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List* rgn_po){(( void(*)( void(* f)( struct _tuple7*), struct
Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dump_rgncmp,
rgn_po, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args !=  0; args= args->tl){ Cyc_Absyndump_dumpfunarg(( struct
_tuple1*) args->hd); if(( args->tl !=  0? 1: c_varargs)? 1: cyc_varargs !=  0){
Cyc_Absyndump_dump_char(( int)',');}} if( c_varargs){ Cyc_Absyndump_dump(
_tag_arr("...", sizeof( unsigned char), 4u));} else{ if( cyc_varargs !=  0){
struct _tuple1* _temp370=({ struct _tuple1* _temp371=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp371->f1= cyc_varargs->name; _temp371->f2=
cyc_varargs->tq; _temp371->f3=( void*) cyc_varargs->type; _temp371;}); Cyc_Absyndump_dump(
_tag_arr("...", sizeof( unsigned char), 4u)); if( cyc_varargs->inject){ Cyc_Absyndump_dump(
_tag_arr(" inject ", sizeof( unsigned char), 9u));} Cyc_Absyndump_dumpfunarg(
_temp370);}} if( effopt !=  0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff((
void*) effopt->v);} if( rgn_po !=  0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dump_rgnpo(
rgn_po);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp( void*
t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp372; _temp372.q_const= 0; _temp372.q_volatile= 0;
_temp372.q_restrict= 0; _temp372;}), t,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpdesignator( void* d){ void* _temp373= d; struct Cyc_Absyn_Exp*
_temp379; struct _tagged_arr* _temp381; _LL375: if(*(( int*) _temp373) ==  Cyc_Absyn_ArrayElement){
_LL380: _temp379=(( struct Cyc_Absyn_ArrayElement_struct*) _temp373)->f1; goto
_LL376;} else{ goto _LL377;} _LL377: if(*(( int*) _temp373) ==  Cyc_Absyn_FieldName){
_LL382: _temp381=(( struct Cyc_Absyn_FieldName_struct*) _temp373)->f1; goto
_LL378;} else{ goto _LL374;} _LL376: Cyc_Absyndump_dump( _tag_arr(".[", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp( _temp379); Cyc_Absyndump_dump_char((
int)']'); goto _LL374; _LL378: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp381); goto _LL374; _LL374:;} struct _tuple8{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple8* de){
Cyc_Absyndump_egroup( Cyc_Absyndump_dumpdesignator,(* de).f1, _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=",
sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpexp((* de).f2);} void Cyc_Absyndump_dumpexps_prec(
int inprec, struct Cyc_List_List* es){(( void(*)( void(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group_c)( Cyc_Absyndump_dumpexp_prec,
inprec, es, _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >=  myprec){ Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof(
unsigned char), 2u));}{ void* _temp383=( void*) e->r; void* _temp483;
unsigned char _temp485; void* _temp487; void* _temp489; short _temp491; void*
_temp493; void* _temp495; int _temp497; void* _temp499; void* _temp501; int
_temp503; void* _temp505; void* _temp507; long long _temp509; void* _temp511;
void* _temp513; struct _tagged_arr _temp515; void* _temp517; void* _temp519;
struct _tagged_arr _temp521; struct _tuple0* _temp523; struct _tuple0* _temp525;
struct Cyc_List_List* _temp527; void* _temp529; struct Cyc_Absyn_Exp* _temp531;
struct Cyc_Core_Opt* _temp533; struct Cyc_Absyn_Exp* _temp535; void* _temp537;
struct Cyc_Absyn_Exp* _temp539; void* _temp541; struct Cyc_Absyn_Exp* _temp543;
void* _temp545; struct Cyc_Absyn_Exp* _temp547; void* _temp549; struct Cyc_Absyn_Exp*
_temp551; struct Cyc_Absyn_Exp* _temp553; struct Cyc_Absyn_Exp* _temp555; struct
Cyc_Absyn_Exp* _temp557; struct Cyc_Absyn_Exp* _temp559; struct Cyc_Absyn_Exp*
_temp561; struct Cyc_List_List* _temp563; struct Cyc_Absyn_Exp* _temp565; struct
Cyc_List_List* _temp567; struct Cyc_Absyn_Exp* _temp569; struct Cyc_Absyn_Exp*
_temp571; struct Cyc_Absyn_Exp* _temp573; struct Cyc_Absyn_Exp* _temp575; struct
Cyc_Absyn_Exp* _temp577; void* _temp579; struct Cyc_Absyn_Exp* _temp581; struct
Cyc_Absyn_Exp* _temp583; struct Cyc_Absyn_Exp* _temp585; void* _temp587; struct
Cyc_Absyn_Exp* _temp589; void* _temp591; struct _tagged_arr* _temp593; void*
_temp595; void* _temp597; unsigned int _temp599; void* _temp601; void* _temp603;
struct Cyc_List_List* _temp605; struct Cyc_Absyn_Exp* _temp607; struct
_tagged_arr* _temp609; struct Cyc_Absyn_Exp* _temp611; struct _tagged_arr*
_temp613; struct Cyc_Absyn_Exp* _temp615; struct Cyc_Absyn_Exp* _temp617; struct
Cyc_Absyn_Exp* _temp619; struct Cyc_List_List* _temp621; struct Cyc_List_List*
_temp623; struct _tuple1* _temp625; struct Cyc_List_List* _temp627; struct Cyc_Absyn_Exp*
_temp629; struct Cyc_Absyn_Exp* _temp631; struct Cyc_Absyn_Vardecl* _temp633;
struct Cyc_List_List* _temp635; struct _tuple0* _temp637; struct Cyc_List_List*
_temp639; struct Cyc_Absyn_Tunionfield* _temp641; struct Cyc_List_List* _temp643;
struct _tuple0* _temp645; struct _tuple0* _temp647; void* _temp649; struct Cyc_Absyn_Exp*
_temp651; struct Cyc_List_List* _temp653; struct Cyc_Core_Opt* _temp655; struct
Cyc_Absyn_Stmt* _temp657; struct Cyc_Absyn_Fndecl* _temp659; struct Cyc_Absyn_Exp*
_temp661; _LL385: if(*(( int*) _temp383) ==  Cyc_Absyn_Const_e){ _LL484:
_temp483=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp383)->f1; if((
unsigned int) _temp483 >  1u?*(( int*) _temp483) ==  Cyc_Absyn_Char_c: 0){
_LL488: _temp487=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp483)->f1; goto
_LL486; _LL486: _temp485=(( struct Cyc_Absyn_Char_c_struct*) _temp483)->f2; goto
_LL386;} else{ goto _LL387;}} else{ goto _LL387;} _LL387: if(*(( int*) _temp383)
==  Cyc_Absyn_Const_e){ _LL490: _temp489=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp383)->f1; if(( unsigned int) _temp489 >  1u?*(( int*) _temp489) ==  Cyc_Absyn_Short_c:
0){ _LL494: _temp493=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp489)->f1;
goto _LL492; _LL492: _temp491=(( struct Cyc_Absyn_Short_c_struct*) _temp489)->f2;
goto _LL388;} else{ goto _LL389;}} else{ goto _LL389;} _LL389: if(*(( int*)
_temp383) ==  Cyc_Absyn_Const_e){ _LL496: _temp495=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp383)->f1; if(( unsigned int) _temp495 >  1u?*(( int*) _temp495) ==  Cyc_Absyn_Int_c:
0){ _LL500: _temp499=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp495)->f1;
if( _temp499 == ( void*) Cyc_Absyn_Signed){ goto _LL498;} else{ goto _LL391;}
_LL498: _temp497=(( struct Cyc_Absyn_Int_c_struct*) _temp495)->f2; goto _LL390;}
else{ goto _LL391;}} else{ goto _LL391;} _LL391: if(*(( int*) _temp383) ==  Cyc_Absyn_Const_e){
_LL502: _temp501=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp383)->f1; if((
unsigned int) _temp501 >  1u?*(( int*) _temp501) ==  Cyc_Absyn_Int_c: 0){ _LL506:
_temp505=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp501)->f1; if( _temp505
== ( void*) Cyc_Absyn_Unsigned){ goto _LL504;} else{ goto _LL393;} _LL504:
_temp503=(( struct Cyc_Absyn_Int_c_struct*) _temp501)->f2; goto _LL392;} else{
goto _LL393;}} else{ goto _LL393;} _LL393: if(*(( int*) _temp383) ==  Cyc_Absyn_Const_e){
_LL508: _temp507=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp383)->f1; if((
unsigned int) _temp507 >  1u?*(( int*) _temp507) ==  Cyc_Absyn_LongLong_c: 0){
_LL512: _temp511=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp507)->f1;
goto _LL510; _LL510: _temp509=(( struct Cyc_Absyn_LongLong_c_struct*) _temp507)->f2;
goto _LL394;} else{ goto _LL395;}} else{ goto _LL395;} _LL395: if(*(( int*)
_temp383) ==  Cyc_Absyn_Const_e){ _LL514: _temp513=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp383)->f1; if(( unsigned int) _temp513 >  1u?*(( int*) _temp513) ==  Cyc_Absyn_Float_c:
0){ _LL516: _temp515=(( struct Cyc_Absyn_Float_c_struct*) _temp513)->f1; goto
_LL396;} else{ goto _LL397;}} else{ goto _LL397;} _LL397: if(*(( int*) _temp383)
==  Cyc_Absyn_Const_e){ _LL518: _temp517=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp383)->f1; if( _temp517 == ( void*) Cyc_Absyn_Null_c){ goto _LL398;} else{
goto _LL399;}} else{ goto _LL399;} _LL399: if(*(( int*) _temp383) ==  Cyc_Absyn_Const_e){
_LL520: _temp519=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp383)->f1; if((
unsigned int) _temp519 >  1u?*(( int*) _temp519) ==  Cyc_Absyn_String_c: 0){
_LL522: _temp521=(( struct Cyc_Absyn_String_c_struct*) _temp519)->f1; goto
_LL400;} else{ goto _LL401;}} else{ goto _LL401;} _LL401: if(*(( int*) _temp383)
==  Cyc_Absyn_UnknownId_e){ _LL524: _temp523=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp383)->f1; goto _LL402;} else{ goto _LL403;} _LL403: if(*(( int*) _temp383)
==  Cyc_Absyn_Var_e){ _LL526: _temp525=(( struct Cyc_Absyn_Var_e_struct*)
_temp383)->f1; goto _LL404;} else{ goto _LL405;} _LL405: if(*(( int*) _temp383)
==  Cyc_Absyn_Primop_e){ _LL530: _temp529=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp383)->f1; goto _LL528; _LL528: _temp527=(( struct Cyc_Absyn_Primop_e_struct*)
_temp383)->f2; goto _LL406;} else{ goto _LL407;} _LL407: if(*(( int*) _temp383)
==  Cyc_Absyn_AssignOp_e){ _LL536: _temp535=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp383)->f1; goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp383)->f2; goto _LL532; _LL532: _temp531=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp383)->f3; goto _LL408;} else{ goto _LL409;} _LL409: if(*(( int*) _temp383)
==  Cyc_Absyn_Increment_e){ _LL540: _temp539=(( struct Cyc_Absyn_Increment_e_struct*)
_temp383)->f1; goto _LL538; _LL538: _temp537=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp383)->f2; if( _temp537 == ( void*) Cyc_Absyn_PreInc){ goto _LL410;} else{
goto _LL411;}} else{ goto _LL411;} _LL411: if(*(( int*) _temp383) ==  Cyc_Absyn_Increment_e){
_LL544: _temp543=(( struct Cyc_Absyn_Increment_e_struct*) _temp383)->f1; goto
_LL542; _LL542: _temp541=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp383)->f2; if( _temp541 == ( void*) Cyc_Absyn_PreDec){ goto _LL412;} else{
goto _LL413;}} else{ goto _LL413;} _LL413: if(*(( int*) _temp383) ==  Cyc_Absyn_Increment_e){
_LL548: _temp547=(( struct Cyc_Absyn_Increment_e_struct*) _temp383)->f1; goto
_LL546; _LL546: _temp545=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp383)->f2; if( _temp545 == ( void*) Cyc_Absyn_PostInc){ goto _LL414;} else{
goto _LL415;}} else{ goto _LL415;} _LL415: if(*(( int*) _temp383) ==  Cyc_Absyn_Increment_e){
_LL552: _temp551=(( struct Cyc_Absyn_Increment_e_struct*) _temp383)->f1; goto
_LL550; _LL550: _temp549=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp383)->f2; if( _temp549 == ( void*) Cyc_Absyn_PostDec){ goto _LL416;} else{
goto _LL417;}} else{ goto _LL417;} _LL417: if(*(( int*) _temp383) ==  Cyc_Absyn_Conditional_e){
_LL558: _temp557=(( struct Cyc_Absyn_Conditional_e_struct*) _temp383)->f1; goto
_LL556; _LL556: _temp555=(( struct Cyc_Absyn_Conditional_e_struct*) _temp383)->f2;
goto _LL554; _LL554: _temp553=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp383)->f3; goto _LL418;} else{ goto _LL419;} _LL419: if(*(( int*) _temp383)
==  Cyc_Absyn_SeqExp_e){ _LL562: _temp561=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp383)->f1; goto _LL560; _LL560: _temp559=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp383)->f2; goto _LL420;} else{ goto _LL421;} _LL421: if(*(( int*) _temp383)
==  Cyc_Absyn_UnknownCall_e){ _LL566: _temp565=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp383)->f1; goto _LL564; _LL564: _temp563=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp383)->f2; goto _LL422;} else{ goto _LL423;} _LL423: if(*(( int*) _temp383)
==  Cyc_Absyn_FnCall_e){ _LL570: _temp569=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp383)->f1; goto _LL568; _LL568: _temp567=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp383)->f2; goto _LL424;} else{ goto _LL425;} _LL425: if(*(( int*) _temp383)
==  Cyc_Absyn_Throw_e){ _LL572: _temp571=(( struct Cyc_Absyn_Throw_e_struct*)
_temp383)->f1; goto _LL426;} else{ goto _LL427;} _LL427: if(*(( int*) _temp383)
==  Cyc_Absyn_NoInstantiate_e){ _LL574: _temp573=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp383)->f1; goto _LL428;} else{ goto _LL429;} _LL429: if(*(( int*) _temp383)
==  Cyc_Absyn_Instantiate_e){ _LL576: _temp575=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp383)->f1; goto _LL430;} else{ goto _LL431;} _LL431: if(*(( int*) _temp383)
==  Cyc_Absyn_Cast_e){ _LL580: _temp579=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp383)->f1; goto _LL578; _LL578: _temp577=(( struct Cyc_Absyn_Cast_e_struct*)
_temp383)->f2; goto _LL432;} else{ goto _LL433;} _LL433: if(*(( int*) _temp383)
==  Cyc_Absyn_Address_e){ _LL582: _temp581=(( struct Cyc_Absyn_Address_e_struct*)
_temp383)->f1; goto _LL434;} else{ goto _LL435;} _LL435: if(*(( int*) _temp383)
==  Cyc_Absyn_New_e){ _LL586: _temp585=(( struct Cyc_Absyn_New_e_struct*)
_temp383)->f1; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_New_e_struct*)
_temp383)->f2; goto _LL436;} else{ goto _LL437;} _LL437: if(*(( int*) _temp383)
==  Cyc_Absyn_Sizeoftyp_e){ _LL588: _temp587=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp383)->f1; goto _LL438;} else{ goto _LL439;} _LL439: if(*(( int*) _temp383)
==  Cyc_Absyn_Sizeofexp_e){ _LL590: _temp589=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp383)->f1; goto _LL440;} else{ goto _LL441;} _LL441: if(*(( int*) _temp383)
==  Cyc_Absyn_Offsetof_e){ _LL596: _temp595=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp383)->f1; goto _LL592; _LL592: _temp591=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp383)->f2; if(*(( int*) _temp591) ==  Cyc_Absyn_StructField){ _LL594:
_temp593=(( struct Cyc_Absyn_StructField_struct*) _temp591)->f1; goto _LL442;}
else{ goto _LL443;}} else{ goto _LL443;} _LL443: if(*(( int*) _temp383) ==  Cyc_Absyn_Offsetof_e){
_LL602: _temp601=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp383)->f1;
goto _LL598; _LL598: _temp597=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp383)->f2; if(*(( int*) _temp597) ==  Cyc_Absyn_TupleIndex){ _LL600:
_temp599=(( struct Cyc_Absyn_TupleIndex_struct*) _temp597)->f1; goto _LL444;}
else{ goto _LL445;}} else{ goto _LL445;} _LL445: if(*(( int*) _temp383) ==  Cyc_Absyn_Gentyp_e){
_LL606: _temp605=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp383)->f1; goto
_LL604; _LL604: _temp603=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp383)->f2;
goto _LL446;} else{ goto _LL447;} _LL447: if(*(( int*) _temp383) ==  Cyc_Absyn_Deref_e){
_LL608: _temp607=(( struct Cyc_Absyn_Deref_e_struct*) _temp383)->f1; goto _LL448;}
else{ goto _LL449;} _LL449: if(*(( int*) _temp383) ==  Cyc_Absyn_StructMember_e){
_LL612: _temp611=(( struct Cyc_Absyn_StructMember_e_struct*) _temp383)->f1; goto
_LL610; _LL610: _temp609=(( struct Cyc_Absyn_StructMember_e_struct*) _temp383)->f2;
goto _LL450;} else{ goto _LL451;} _LL451: if(*(( int*) _temp383) ==  Cyc_Absyn_StructArrow_e){
_LL616: _temp615=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp383)->f1; goto
_LL614; _LL614: _temp613=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp383)->f2;
goto _LL452;} else{ goto _LL453;} _LL453: if(*(( int*) _temp383) ==  Cyc_Absyn_Subscript_e){
_LL620: _temp619=(( struct Cyc_Absyn_Subscript_e_struct*) _temp383)->f1; goto
_LL618; _LL618: _temp617=(( struct Cyc_Absyn_Subscript_e_struct*) _temp383)->f2;
goto _LL454;} else{ goto _LL455;} _LL455: if(*(( int*) _temp383) ==  Cyc_Absyn_Tuple_e){
_LL622: _temp621=(( struct Cyc_Absyn_Tuple_e_struct*) _temp383)->f1; goto _LL456;}
else{ goto _LL457;} _LL457: if(*(( int*) _temp383) ==  Cyc_Absyn_CompoundLit_e){
_LL626: _temp625=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp383)->f1; goto
_LL624; _LL624: _temp623=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp383)->f2;
goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp383) ==  Cyc_Absyn_Array_e){
_LL628: _temp627=(( struct Cyc_Absyn_Array_e_struct*) _temp383)->f1; goto _LL460;}
else{ goto _LL461;} _LL461: if(*(( int*) _temp383) ==  Cyc_Absyn_Comprehension_e){
_LL634: _temp633=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp383)->f1;
goto _LL632; _LL632: _temp631=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp383)->f2; goto _LL630; _LL630: _temp629=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp383)->f3; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp383)
==  Cyc_Absyn_Struct_e){ _LL638: _temp637=(( struct Cyc_Absyn_Struct_e_struct*)
_temp383)->f1; goto _LL636; _LL636: _temp635=(( struct Cyc_Absyn_Struct_e_struct*)
_temp383)->f3; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp383)
==  Cyc_Absyn_AnonStruct_e){ _LL640: _temp639=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp383)->f2; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp383)
==  Cyc_Absyn_Tunion_e){ _LL644: _temp643=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp383)->f3; goto _LL642; _LL642: _temp641=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp383)->f5; goto _LL468;} else{ goto _LL469;} _LL469: if(*(( int*) _temp383)
==  Cyc_Absyn_Enum_e){ _LL646: _temp645=(( struct Cyc_Absyn_Enum_e_struct*)
_temp383)->f1; goto _LL470;} else{ goto _LL471;} _LL471: if(*(( int*) _temp383)
==  Cyc_Absyn_AnonEnum_e){ _LL648: _temp647=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp383)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if(*(( int*) _temp383)
==  Cyc_Absyn_Malloc_e){ _LL652: _temp651=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp383)->f1; goto _LL650; _LL650: _temp649=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp383)->f2; goto _LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp383)
==  Cyc_Absyn_UnresolvedMem_e){ _LL656: _temp655=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp383)->f1; goto _LL654; _LL654: _temp653=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp383)->f2; goto _LL476;} else{ goto _LL477;} _LL477: if(*(( int*) _temp383)
==  Cyc_Absyn_StmtExp_e){ _LL658: _temp657=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp383)->f1; goto _LL478;} else{ goto _LL479;} _LL479: if(*(( int*) _temp383)
==  Cyc_Absyn_Codegen_e){ _LL660: _temp659=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp383)->f1; goto _LL480;} else{ goto _LL481;} _LL481: if(*(( int*) _temp383)
==  Cyc_Absyn_Fill_e){ _LL662: _temp661=(( struct Cyc_Absyn_Fill_e_struct*)
_temp383)->f1; goto _LL482;} else{ goto _LL384;} _LL386: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp485)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL384; _LL388: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp664; _temp664.tag= Cyc_Std_Int_pa; _temp664.f1=(
int)(( unsigned int)(( int) _temp491));{ void* _temp663[ 1u]={& _temp664}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp663, sizeof( void*),
1u));}})); goto _LL384; _LL390: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp666; _temp666.tag= Cyc_Std_Int_pa; _temp666.f1=(
int)(( unsigned int) _temp497);{ void* _temp665[ 1u]={& _temp666}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp665, sizeof( void*),
1u));}})); goto _LL384; _LL392: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp668; _temp668.tag= Cyc_Std_Int_pa; _temp668.f1=(
int)(( unsigned int) _temp503);{ void* _temp667[ 1u]={& _temp668}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp667, sizeof( void*),
1u));}})); Cyc_Absyndump_dump_nospace( _tag_arr("u", sizeof( unsigned char), 2u));
goto _LL384; _LL394: Cyc_Absyndump_dump( _tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof( unsigned char), 27u)); goto _LL384; _LL396: Cyc_Absyndump_dump( _temp515);
goto _LL384; _LL398: Cyc_Absyndump_dump( _tag_arr("NULL", sizeof( unsigned char),
5u)); goto _LL384; _LL400: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp521)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL384; _LL402: _temp525= _temp523; goto _LL404; _LL404: Cyc_Absyndump_dumpqvar(
_temp525); goto _LL384; _LL406: { struct _tagged_arr _temp669= Cyc_Absynpp_prim2str(
_temp529); switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp527)){ case 1: _LL670: if( _temp529 == ( void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp527))->hd);
Cyc_Absyndump_dump( _tag_arr(".size", sizeof( unsigned char), 6u));} else{ Cyc_Absyndump_dump(
_temp669); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp527))->hd);} break; case 2: _LL671: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp527))->hd);
Cyc_Absyndump_dump( _temp669); Cyc_Absyndump_dump_char(( int)' '); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp527->tl))->hd);
break; default: _LL672:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp674=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp674[ 0]=({ struct Cyc_Core_Failure_struct _temp675; _temp675.tag= Cyc_Core_Failure;
_temp675.f1= _tag_arr("Absyndump -- Bad number of arguments to primop", sizeof(
unsigned char), 47u); _temp675;}); _temp674;}));} goto _LL384;} _LL408: Cyc_Absyndump_dumpexp_prec(
myprec, _temp535); if( _temp533 !=  0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*) _temp533->v));} Cyc_Absyndump_dump_nospace( _tag_arr("=", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp531); goto _LL384;
_LL410: Cyc_Absyndump_dump( _tag_arr("++", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp539); goto _LL384; _LL412: Cyc_Absyndump_dump( _tag_arr("--",
sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp543);
goto _LL384; _LL414: Cyc_Absyndump_dumpexp_prec( myprec, _temp547); Cyc_Absyndump_dump(
_tag_arr("++", sizeof( unsigned char), 3u)); goto _LL384; _LL416: Cyc_Absyndump_dumpexp_prec(
myprec, _temp551); Cyc_Absyndump_dump( _tag_arr("--", sizeof( unsigned char), 3u));
goto _LL384; _LL418: Cyc_Absyndump_dumpexp_prec( myprec, _temp557); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp555); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp553); goto _LL384; _LL420:
Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec( myprec, _temp561);
Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec( myprec, _temp559);
Cyc_Absyndump_dump_char(( int)')'); goto _LL384; _LL422: _temp569= _temp565;
_temp567= _temp563; goto _LL424; _LL424: Cyc_Absyndump_dumpexp_prec( myprec,
_temp569); Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpexps_prec( 20, _temp567); Cyc_Absyndump_dump_nospace( _tag_arr(")",
sizeof( unsigned char), 2u)); goto _LL384; _LL426: Cyc_Absyndump_dump( _tag_arr("throw",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp571);
goto _LL384; _LL428: _temp575= _temp573; goto _LL430; _LL430: Cyc_Absyndump_dumpexp_prec(
inprec, _temp575); goto _LL384; _LL432: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp579); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp577); goto _LL384; _LL434: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp581); goto _LL384; _LL436: Cyc_Absyndump_dump( _tag_arr("new ",
sizeof( unsigned char), 5u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp583);
goto _LL384; _LL438: Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumptyp( _temp587); Cyc_Absyndump_dump_char((
int)')'); goto _LL384; _LL440: Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp589); Cyc_Absyndump_dump_char((
int)')'); goto _LL384; _LL442: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp595); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump_nospace(* _temp593); Cyc_Absyndump_dump_char(( int)')');
goto _LL384; _LL444: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp601); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp677; _temp677.tag= Cyc_Std_Int_pa; _temp677.f1=( int) _temp599;{ void*
_temp676[ 1u]={& _temp677}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp676, sizeof( void*), 1u));}})); Cyc_Absyndump_dump_char((
int)')'); goto _LL384; _LL446: Cyc_Absyndump_dump( _tag_arr("__gen(", sizeof(
unsigned char), 7u)); Cyc_Absyndump_dumptvars( _temp605); Cyc_Absyndump_dumptyp(
_temp603); Cyc_Absyndump_dump_char(( int)')'); goto _LL384; _LL448: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dumpexp_prec( myprec, _temp607); goto _LL384; _LL450:
Cyc_Absyndump_dumpexp_prec( myprec, _temp611); Cyc_Absyndump_dump_char(( int)'.');
Cyc_Absyndump_dump_nospace(* _temp609); goto _LL384; _LL452: Cyc_Absyndump_dumpexp_prec(
myprec, _temp615); Cyc_Absyndump_dump_nospace( _tag_arr("->", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dump_nospace(* _temp613); goto _LL384;
_LL454: Cyc_Absyndump_dumpexp_prec( myprec, _temp619); Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp617); Cyc_Absyndump_dump_char(( int)']');
goto _LL384; _LL456: Cyc_Absyndump_dump( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_Absyndump_dumpexps_prec( 20, _temp621); Cyc_Absyndump_dump_char(( int)')');
goto _LL384; _LL458: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp625).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp623, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL384;
_LL460:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp627, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL384; _LL462: Cyc_Absyndump_dump( _tag_arr("new {for", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dump_str((* _temp633->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp631); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp629); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL384; _LL464: Cyc_Absyndump_dumpqvar( _temp637);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp635, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL384;
_LL466:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp639, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL384; _LL468: Cyc_Absyndump_dumpqvar( _temp641->name); if( _temp643
!=  0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp643, _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u));} goto _LL384; _LL470: Cyc_Absyndump_dumpqvar( _temp645); goto _LL384;
_LL472: Cyc_Absyndump_dumpqvar( _temp647); goto _LL384; _LL474: if( _temp651 != 
0){ Cyc_Absyndump_dump( _tag_arr("rmalloc(", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp651)); Cyc_Absyndump_dump( _tag_arr(",",
sizeof( unsigned char), 2u));} else{ Cyc_Absyndump_dump( _tag_arr("malloc(",
sizeof( unsigned char), 8u));} Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumptyp( _temp649); Cyc_Absyndump_dump(
_tag_arr("))", sizeof( unsigned char), 3u)); goto _LL384; _LL476:(( void(*)(
void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct _tagged_arr start,
struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp653, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL384;
_LL478: Cyc_Absyndump_dump_nospace( _tag_arr("({", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpstmt( _temp657); Cyc_Absyndump_dump_nospace( _tag_arr("})",
sizeof( unsigned char), 3u)); goto _LL384; _LL480: Cyc_Absyndump_dump( _tag_arr("codegen(",
sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp678=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp678->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp679=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp679[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp680; _temp680.tag= Cyc_Absyn_Fn_d;
_temp680.f1= _temp659; _temp680;}); _temp679;})); _temp678->loc= e->loc;
_temp678;})); Cyc_Absyndump_dump( _tag_arr(")", sizeof( unsigned char), 2u));
goto _LL384; _LL482: Cyc_Absyndump_dump( _tag_arr("fill(", sizeof( unsigned char),
6u)); Cyc_Absyndump_dumpexp( _temp661); Cyc_Absyndump_dump( _tag_arr(")",
sizeof( unsigned char), 2u)); goto _LL384; _LL384:;} if( inprec >=  myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs !=  0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause* _temp681=( struct Cyc_Absyn_Switch_clause*)
scs->hd; if( _temp681->where_clause ==  0?( void*)( _temp681->pattern)->r == (
void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump( _tag_arr("default:", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("case", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumppat( _temp681->pattern); if( _temp681->where_clause
!=  0){ Cyc_Absyndump_dump( _tag_arr("&&", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp681->where_clause));} Cyc_Absyndump_dump_nospace(
_tag_arr(":", sizeof( unsigned char), 2u));} Cyc_Absyndump_dumpstmt( _temp681->body);}}
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt* s){ void* _temp682=( void*)
s->r; struct Cyc_Absyn_Exp* _temp728; struct Cyc_Absyn_Stmt* _temp730; struct
Cyc_Absyn_Stmt* _temp732; struct Cyc_Absyn_Exp* _temp734; struct Cyc_Absyn_Exp*
_temp736; struct Cyc_Absyn_Stmt* _temp738; struct Cyc_Absyn_Stmt* _temp740;
struct Cyc_Absyn_Exp* _temp742; struct Cyc_Absyn_Stmt* _temp744; struct _tuple2
_temp746; struct Cyc_Absyn_Exp* _temp748; struct _tagged_arr* _temp750; struct
Cyc_Absyn_Stmt* _temp752; struct _tuple2 _temp754; struct Cyc_Absyn_Exp*
_temp756; struct _tuple2 _temp758; struct Cyc_Absyn_Exp* _temp760; struct Cyc_Absyn_Exp*
_temp762; struct Cyc_List_List* _temp764; struct Cyc_Absyn_Exp* _temp766; struct
Cyc_Absyn_Stmt* _temp768; struct Cyc_Absyn_Decl* _temp770; struct Cyc_Absyn_Stmt*
_temp772; struct _tagged_arr* _temp774; struct _tuple2 _temp776; struct Cyc_Absyn_Exp*
_temp778; struct Cyc_Absyn_Stmt* _temp780; struct Cyc_List_List* _temp782;
struct Cyc_Absyn_Exp* _temp784; struct Cyc_List_List* _temp786; struct Cyc_List_List*
_temp788; struct Cyc_List_List* _temp790; struct Cyc_Absyn_Stmt* _temp792;
struct Cyc_Absyn_Stmt* _temp794; struct Cyc_Absyn_Vardecl* _temp796; struct Cyc_Absyn_Tvar*
_temp798; struct Cyc_Absyn_Stmt* _temp800; struct Cyc_Absyn_Stmt* _temp802;
_LL684: if( _temp682 == ( void*) Cyc_Absyn_Skip_s){ goto _LL685;} else{ goto
_LL686;} _LL686: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Exp_s:
0){ _LL729: _temp728=(( struct Cyc_Absyn_Exp_s_struct*) _temp682)->f1; goto
_LL687;} else{ goto _LL688;} _LL688: if(( unsigned int) _temp682 >  1u?*(( int*)
_temp682) ==  Cyc_Absyn_Seq_s: 0){ _LL733: _temp732=(( struct Cyc_Absyn_Seq_s_struct*)
_temp682)->f1; goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_Seq_s_struct*)
_temp682)->f2; goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int)
_temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Return_s: 0){ _LL735: _temp734=((
struct Cyc_Absyn_Return_s_struct*) _temp682)->f1; if( _temp734 ==  0){ goto
_LL691;} else{ goto _LL692;}} else{ goto _LL692;} _LL692: if(( unsigned int)
_temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Return_s: 0){ _LL737: _temp736=((
struct Cyc_Absyn_Return_s_struct*) _temp682)->f1; goto _LL693;} else{ goto
_LL694;} _LL694: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_IfThenElse_s:
0){ _LL743: _temp742=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp682)->f1;
goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp682)->f2;
goto _LL739; _LL739: _temp738=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp682)->f3;
goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int) _temp682 >  1u?*((
int*) _temp682) ==  Cyc_Absyn_While_s: 0){ _LL747: _temp746=(( struct Cyc_Absyn_While_s_struct*)
_temp682)->f1; _LL749: _temp748= _temp746.f1; goto _LL745; _LL745: _temp744=((
struct Cyc_Absyn_While_s_struct*) _temp682)->f2; goto _LL697;} else{ goto _LL698;}
_LL698: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Break_s:
0){ goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int) _temp682 >  1u?*((
int*) _temp682) ==  Cyc_Absyn_Continue_s: 0){ goto _LL701;} else{ goto _LL702;}
_LL702: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Goto_s:
0){ _LL751: _temp750=(( struct Cyc_Absyn_Goto_s_struct*) _temp682)->f1; goto
_LL703;} else{ goto _LL704;} _LL704: if(( unsigned int) _temp682 >  1u?*(( int*)
_temp682) ==  Cyc_Absyn_For_s: 0){ _LL763: _temp762=(( struct Cyc_Absyn_For_s_struct*)
_temp682)->f1; goto _LL759; _LL759: _temp758=(( struct Cyc_Absyn_For_s_struct*)
_temp682)->f2; _LL761: _temp760= _temp758.f1; goto _LL755; _LL755: _temp754=((
struct Cyc_Absyn_For_s_struct*) _temp682)->f3; _LL757: _temp756= _temp754.f1;
goto _LL753; _LL753: _temp752=(( struct Cyc_Absyn_For_s_struct*) _temp682)->f4;
goto _LL705;} else{ goto _LL706;} _LL706: if(( unsigned int) _temp682 >  1u?*((
int*) _temp682) ==  Cyc_Absyn_Switch_s: 0){ _LL767: _temp766=(( struct Cyc_Absyn_Switch_s_struct*)
_temp682)->f1; goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_Switch_s_struct*)
_temp682)->f2; goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int)
_temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Decl_s: 0){ _LL771: _temp770=((
struct Cyc_Absyn_Decl_s_struct*) _temp682)->f1; goto _LL769; _LL769: _temp768=((
struct Cyc_Absyn_Decl_s_struct*) _temp682)->f2; goto _LL709;} else{ goto _LL710;}
_LL710: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Label_s:
0){ _LL775: _temp774=(( struct Cyc_Absyn_Label_s_struct*) _temp682)->f1; goto
_LL773; _LL773: _temp772=(( struct Cyc_Absyn_Label_s_struct*) _temp682)->f2;
goto _LL711;} else{ goto _LL712;} _LL712: if(( unsigned int) _temp682 >  1u?*((
int*) _temp682) ==  Cyc_Absyn_Do_s: 0){ _LL781: _temp780=(( struct Cyc_Absyn_Do_s_struct*)
_temp682)->f1; goto _LL777; _LL777: _temp776=(( struct Cyc_Absyn_Do_s_struct*)
_temp682)->f2; _LL779: _temp778= _temp776.f1; goto _LL713;} else{ goto _LL714;}
_LL714: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_SwitchC_s:
0){ _LL785: _temp784=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp682)->f1; goto
_LL783; _LL783: _temp782=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp682)->f2;
goto _LL715;} else{ goto _LL716;} _LL716: if(( unsigned int) _temp682 >  1u?*((
int*) _temp682) ==  Cyc_Absyn_Fallthru_s: 0){ _LL787: _temp786=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp682)->f1; if( _temp786 ==  0){ goto _LL717;} else{ goto _LL718;}} else{
goto _LL718;} _LL718: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) == 
Cyc_Absyn_Fallthru_s: 0){ _LL789: _temp788=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp682)->f1; goto _LL719;} else{ goto _LL720;} _LL720: if(( unsigned int)
_temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_TryCatch_s: 0){ _LL793:
_temp792=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp682)->f1; goto _LL791;
_LL791: _temp790=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp682)->f2; goto
_LL721;} else{ goto _LL722;} _LL722: if(( unsigned int) _temp682 >  1u?*(( int*)
_temp682) ==  Cyc_Absyn_Region_s: 0){ _LL799: _temp798=(( struct Cyc_Absyn_Region_s_struct*)
_temp682)->f1; goto _LL797; _LL797: _temp796=(( struct Cyc_Absyn_Region_s_struct*)
_temp682)->f2; goto _LL795; _LL795: _temp794=(( struct Cyc_Absyn_Region_s_struct*)
_temp682)->f3; goto _LL723;} else{ goto _LL724;} _LL724: if(( unsigned int)
_temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Cut_s: 0){ _LL801: _temp800=((
struct Cyc_Absyn_Cut_s_struct*) _temp682)->f1; goto _LL725;} else{ goto _LL726;}
_LL726: if(( unsigned int) _temp682 >  1u?*(( int*) _temp682) ==  Cyc_Absyn_Splice_s:
0){ _LL803: _temp802=(( struct Cyc_Absyn_Splice_s_struct*) _temp682)->f1; goto
_LL727;} else{ goto _LL683;} _LL685: Cyc_Absyndump_dump_semi(); goto _LL683;
_LL687: Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dumpexp( _temp728); Cyc_Absyndump_dump_semi();
goto _LL683; _LL689: if( Cyc_Absynpp_is_declaration( _temp732)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp732); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp732);} if( Cyc_Absynpp_is_declaration(
_temp730)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp730);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp730);}
goto _LL683; _LL691: Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dump(
_tag_arr("return;", sizeof( unsigned char), 8u)); goto _LL683; _LL693: Cyc_Absyndump_dumploc(
s->loc); Cyc_Absyndump_dump( _tag_arr("return", sizeof( unsigned char), 7u));
Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp736)); Cyc_Absyndump_dump_semi();
goto _LL683; _LL695: Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dump(
_tag_arr("if(", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp( _temp742);
Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt(
_temp740); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp804=( void*) _temp738->r;
_LL806: if( _temp804 == ( void*) Cyc_Absyn_Skip_s){ goto _LL807;} else{ goto
_LL808;} _LL808: goto _LL809; _LL807: goto _LL805; _LL809: Cyc_Absyndump_dump(
_tag_arr("else{", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpstmt( _temp738);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL805; _LL805:;} goto _LL683; _LL697:
Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dump( _tag_arr("while(", sizeof(
unsigned char), 7u)); Cyc_Absyndump_dumpexp( _temp748); Cyc_Absyndump_dump_nospace(
_tag_arr(") {", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp744);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL683; _LL699: Cyc_Absyndump_dumploc(
s->loc); Cyc_Absyndump_dump( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL683; _LL701: Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dump(
_tag_arr("continue;", sizeof( unsigned char), 10u)); goto _LL683; _LL703: Cyc_Absyndump_dumploc(
s->loc); Cyc_Absyndump_dump( _tag_arr("goto", sizeof( unsigned char), 5u)); Cyc_Absyndump_dump_str(
_temp750); Cyc_Absyndump_dump_semi(); goto _LL683; _LL705: Cyc_Absyndump_dumploc(
s->loc); Cyc_Absyndump_dump( _tag_arr("for(", sizeof( unsigned char), 5u)); Cyc_Absyndump_dumpexp(
_temp762); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp760); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp756); Cyc_Absyndump_dump_nospace( _tag_arr("){",
sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp752); Cyc_Absyndump_dump_char((
int)'}'); goto _LL683; _LL707: Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dump(
_tag_arr("switch(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpexp(
_temp766); Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpswitchclauses( _temp764); Cyc_Absyndump_dump_char(( int)'}');
goto _LL683; _LL709: Cyc_Absyndump_dumpdecl( _temp770); Cyc_Absyndump_dumpstmt(
_temp768); goto _LL683; _LL711: if( Cyc_Absynpp_is_declaration( _temp772)){ Cyc_Absyndump_dump_str(
_temp774); Cyc_Absyndump_dump_nospace( _tag_arr(": {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp772); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dump_str( _temp774); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt(
_temp772);} goto _LL683; _LL713: Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dump(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_Absyndump_dumpstmt( _temp780);
Cyc_Absyndump_dump_nospace( _tag_arr("} while (", sizeof( unsigned char), 10u));
Cyc_Absyndump_dumpexp( _temp778); Cyc_Absyndump_dump_nospace( _tag_arr(");",
sizeof( unsigned char), 3u)); goto _LL683; _LL715: Cyc_Absyndump_dumploc( s->loc);
Cyc_Absyndump_dump( _tag_arr("switch \"C\" (", sizeof( unsigned char), 13u));
Cyc_Absyndump_dumpexp( _temp784); Cyc_Absyndump_dump_nospace( _tag_arr("){",
sizeof( unsigned char), 3u)); for( 0; _temp782 !=  0; _temp782= _temp782->tl){
struct Cyc_Absyn_SwitchC_clause _temp812; struct Cyc_Absyn_Stmt* _temp813;
struct Cyc_Absyn_Exp* _temp815; struct Cyc_Absyn_SwitchC_clause* _temp810=(
struct Cyc_Absyn_SwitchC_clause*) _temp782->hd; _temp812=* _temp810; _LL816:
_temp815= _temp812.cnst_exp; goto _LL814; _LL814: _temp813= _temp812.body; goto
_LL811; _LL811: if( _temp815 ==  0){ Cyc_Absyndump_dump( _tag_arr("default: ",
sizeof( unsigned char), 10u));} else{ Cyc_Absyndump_dump( _tag_arr("case ",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp815)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp813);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL683; _LL717: Cyc_Absyndump_dump(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL683; _LL719: Cyc_Absyndump_dump(
_tag_arr("fallthru(", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexps_prec(
20, _temp788); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char),
3u)); goto _LL683; _LL721: Cyc_Absyndump_dumploc( s->loc); Cyc_Absyndump_dump(
_tag_arr("try", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp792);
Cyc_Absyndump_dump( _tag_arr("catch {", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpswitchclauses(
_temp790); Cyc_Absyndump_dump_char(( int)'}'); goto _LL683; _LL723: Cyc_Absyndump_dumploc(
s->loc); Cyc_Absyndump_dump( _tag_arr("region<", sizeof( unsigned char), 8u));
Cyc_Absyndump_dumptvar( _temp798); Cyc_Absyndump_dump( _tag_arr("> ", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpqvar( _temp796->name); Cyc_Absyndump_dump(
_tag_arr("{", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpstmt( _temp794);
Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u)); goto _LL683;
_LL725: Cyc_Absyndump_dump( _tag_arr("cut", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpstmt(
_temp800); goto _LL683; _LL727: Cyc_Absyndump_dump( _tag_arr("splice", sizeof(
unsigned char), 7u)); Cyc_Absyndump_dumpstmt( _temp802); goto _LL683; _LL683:;}
struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp(
struct _tuple9* dp){ Cyc_Absyndump_egroup( Cyc_Absyndump_dumpdesignator,(* dp).f1,
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumppat((* dp).f2);}
void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp817=( void*) p->r;
int _temp853; void* _temp855; int _temp857; void* _temp859; unsigned char
_temp861; struct _tagged_arr _temp863; struct Cyc_Absyn_Vardecl* _temp865;
struct Cyc_List_List* _temp867; struct Cyc_Absyn_Pat* _temp869; struct Cyc_Absyn_Vardecl*
_temp871; struct _tuple0* _temp873; struct Cyc_List_List* _temp875; struct Cyc_List_List*
_temp877; struct _tuple0* _temp879; struct Cyc_List_List* _temp881; struct Cyc_List_List*
_temp883; struct _tuple0* _temp885; struct Cyc_List_List* _temp887; struct Cyc_List_List*
_temp889; struct Cyc_Absyn_Structdecl* _temp891; struct Cyc_List_List* _temp893;
struct Cyc_List_List* _temp895; struct Cyc_Absyn_Tunionfield* _temp897; struct
Cyc_Absyn_Enumfield* _temp899; struct Cyc_Absyn_Enumfield* _temp901; _LL819: if(
_temp817 == ( void*) Cyc_Absyn_Wild_p){ goto _LL820;} else{ goto _LL821;} _LL821:
if( _temp817 == ( void*) Cyc_Absyn_Null_p){ goto _LL822;} else{ goto _LL823;}
_LL823: if(( unsigned int) _temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_Int_p:
0){ _LL856: _temp855=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp817)->f1;
if( _temp855 == ( void*) Cyc_Absyn_Signed){ goto _LL854;} else{ goto _LL825;}
_LL854: _temp853=(( struct Cyc_Absyn_Int_p_struct*) _temp817)->f2; goto _LL824;}
else{ goto _LL825;} _LL825: if(( unsigned int) _temp817 >  2u?*(( int*) _temp817)
==  Cyc_Absyn_Int_p: 0){ _LL860: _temp859=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp817)->f1; if( _temp859 == ( void*) Cyc_Absyn_Unsigned){ goto _LL858;} else{
goto _LL827;} _LL858: _temp857=(( struct Cyc_Absyn_Int_p_struct*) _temp817)->f2;
goto _LL826;} else{ goto _LL827;} _LL827: if(( unsigned int) _temp817 >  2u?*((
int*) _temp817) ==  Cyc_Absyn_Char_p: 0){ _LL862: _temp861=(( struct Cyc_Absyn_Char_p_struct*)
_temp817)->f1; goto _LL828;} else{ goto _LL829;} _LL829: if(( unsigned int)
_temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_Float_p: 0){ _LL864: _temp863=((
struct Cyc_Absyn_Float_p_struct*) _temp817)->f1; goto _LL830;} else{ goto _LL831;}
_LL831: if(( unsigned int) _temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_Var_p:
0){ _LL866: _temp865=(( struct Cyc_Absyn_Var_p_struct*) _temp817)->f1; goto
_LL832;} else{ goto _LL833;} _LL833: if(( unsigned int) _temp817 >  2u?*(( int*)
_temp817) ==  Cyc_Absyn_Tuple_p: 0){ _LL868: _temp867=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp817)->f1; goto _LL834;} else{ goto _LL835;} _LL835: if(( unsigned int)
_temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_Pointer_p: 0){ _LL870: _temp869=((
struct Cyc_Absyn_Pointer_p_struct*) _temp817)->f1; goto _LL836;} else{ goto
_LL837;} _LL837: if(( unsigned int) _temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_Reference_p:
0){ _LL872: _temp871=(( struct Cyc_Absyn_Reference_p_struct*) _temp817)->f1;
goto _LL838;} else{ goto _LL839;} _LL839: if(( unsigned int) _temp817 >  2u?*((
int*) _temp817) ==  Cyc_Absyn_UnknownId_p: 0){ _LL874: _temp873=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp817)->f1; goto _LL840;} else{ goto _LL841;} _LL841: if(( unsigned int)
_temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL880:
_temp879=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp817)->f1; goto _LL878;
_LL878: _temp877=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp817)->f2; goto
_LL876; _LL876: _temp875=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp817)->f3;
goto _LL842;} else{ goto _LL843;} _LL843: if(( unsigned int) _temp817 >  2u?*((
int*) _temp817) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL886: _temp885=(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp817)->f1; goto _LL884; _LL884: _temp883=((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp817)->f2; goto _LL882; _LL882:
_temp881=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp817)->f3; goto _LL844;}
else{ goto _LL845;} _LL845: if(( unsigned int) _temp817 >  2u?*(( int*) _temp817)
==  Cyc_Absyn_Struct_p: 0){ _LL892: _temp891=(( struct Cyc_Absyn_Struct_p_struct*)
_temp817)->f1; goto _LL890; _LL890: _temp889=(( struct Cyc_Absyn_Struct_p_struct*)
_temp817)->f3; goto _LL888; _LL888: _temp887=(( struct Cyc_Absyn_Struct_p_struct*)
_temp817)->f4; goto _LL846;} else{ goto _LL847;} _LL847: if(( unsigned int)
_temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_Tunion_p: 0){ _LL898: _temp897=((
struct Cyc_Absyn_Tunion_p_struct*) _temp817)->f2; goto _LL896; _LL896: _temp895=((
struct Cyc_Absyn_Tunion_p_struct*) _temp817)->f3; goto _LL894; _LL894: _temp893=((
struct Cyc_Absyn_Tunion_p_struct*) _temp817)->f4; goto _LL848;} else{ goto
_LL849;} _LL849: if(( unsigned int) _temp817 >  2u?*(( int*) _temp817) ==  Cyc_Absyn_Enum_p:
0){ _LL900: _temp899=(( struct Cyc_Absyn_Enum_p_struct*) _temp817)->f2; goto
_LL850;} else{ goto _LL851;} _LL851: if(( unsigned int) _temp817 >  2u?*(( int*)
_temp817) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL902: _temp901=(( struct Cyc_Absyn_AnonEnum_p_struct*)
_temp817)->f2; goto _LL852;} else{ goto _LL818;} _LL820: Cyc_Absyndump_dump_char((
int)'_'); goto _LL818; _LL822: Cyc_Absyndump_dump( _tag_arr("NULL", sizeof(
unsigned char), 5u)); goto _LL818; _LL824: Cyc_Absyndump_dump(( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp904; _temp904.tag= Cyc_Std_Int_pa;
_temp904.f1=( int)(( unsigned int) _temp853);{ void* _temp903[ 1u]={& _temp904};
Cyc_Std_aprintf( _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp903,
sizeof( void*), 1u));}})); goto _LL818; _LL826: Cyc_Absyndump_dump(( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp906; _temp906.tag= Cyc_Std_Int_pa;
_temp906.f1=( unsigned int) _temp857;{ void* _temp905[ 1u]={& _temp906}; Cyc_Std_aprintf(
_tag_arr("%u", sizeof( unsigned char), 3u), _tag_arr( _temp905, sizeof( void*),
1u));}})); goto _LL818; _LL828: Cyc_Absyndump_dump( _tag_arr("'", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp861)); Cyc_Absyndump_dump_nospace( _tag_arr("'", sizeof( unsigned char), 2u));
goto _LL818; _LL830: Cyc_Absyndump_dump( _temp863); goto _LL818; _LL832: Cyc_Absyndump_dumpqvar(
_temp865->name); goto _LL818; _LL834:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp867,
_tag_arr("$(", sizeof( unsigned char), 3u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL818; _LL836: Cyc_Absyndump_dump(
_tag_arr("&", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumppat( _temp869);
goto _LL818; _LL838: Cyc_Absyndump_dump( _tag_arr("*", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpqvar( _temp871->name); goto _LL818; _LL840: Cyc_Absyndump_dumpqvar(
_temp873); goto _LL818; _LL842: Cyc_Absyndump_dumpqvar( _temp879); Cyc_Absyndump_dumptvars(
_temp877);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp875, _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL818; _LL844: Cyc_Absyndump_dumpqvar( _temp885); Cyc_Absyndump_dumptvars(
_temp883);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp881, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL818; _LL846: if( _temp891->name !=  0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp891->name))->v);} Cyc_Absyndump_dumptvars(
_temp889);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp887, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL818; _LL848: Cyc_Absyndump_dumpqvar( _temp897->name); Cyc_Absyndump_dumptvars(
_temp895); if( _temp893 !=  0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp893,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u));} goto _LL818; _LL850: Cyc_Absyndump_dumpqvar(
_temp899->name); goto _LL818; _LL852: Cyc_Absyndump_dumpqvar( _temp901->name);
goto _LL818; _LL818:;} void Cyc_Absyndump_dumptunionfield( struct Cyc_Absyn_Tunionfield*
ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs !=  0){ Cyc_Absyndump_dumpargs(
ef->typs);}} void Cyc_Absyndump_dumptunionfields( struct Cyc_List_List* fields){((
void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* l,
struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumptunionfield,
fields, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!=  0){ Cyc_Absyndump_dump( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield,
fields, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields !=  0; fields= fields->tl){ struct
Cyc_Absyn_Structfield _temp909; struct Cyc_List_List* _temp910; struct Cyc_Absyn_Exp*
_temp912; void* _temp914; struct Cyc_Absyn_Tqual _temp916; struct _tagged_arr*
_temp918; struct Cyc_Absyn_Structfield* _temp907=( struct Cyc_Absyn_Structfield*)
fields->hd; _temp909=* _temp907; _LL919: _temp918= _temp909.name; goto _LL917;
_LL917: _temp916= _temp909.tq; goto _LL915; _LL915: _temp914=( void*) _temp909.type;
goto _LL913; _LL913: _temp912= _temp909.width; goto _LL911; _LL911: _temp910=
_temp909.attributes; goto _LL908; _LL908:(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tagged_arr*), struct _tagged_arr*)) Cyc_Absyndump_dumptqtd)(
_temp916, _temp914, Cyc_Absyndump_dump_str, _temp918); Cyc_Absyndump_dumpatts(
_temp910); if( _temp912 !=  0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp912));} Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar(
td->name); Cyc_Absyndump_dumptvars( td->tvs);} static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp920=(* pr).f1; _LL922: if( _temp920 == ( void*)
Cyc_Absyn_Unused_att){ goto _LL923;} else{ goto _LL924;} _LL924: if( _temp920 == (
void*) Cyc_Absyn_Stdcall_att){ goto _LL925;} else{ goto _LL926;} _LL926: if(
_temp920 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL927;} else{ goto _LL928;}
_LL928: if( _temp920 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL929;} else{
goto _LL930;} _LL930: goto _LL931; _LL923: goto _LL921; _LL925: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); goto _LL921; _LL927: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); goto _LL921; _LL929: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); goto _LL921; _LL931: goto
_LL921; _LL921:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for(
0; vds !=  0; vds= vds->tl){ Cyc_Absyndump_dumpqvar((( struct Cyc_Absyn_Vardecl*)
vds->hd)->name); if( vds->tl !=  0){ Cyc_Absyndump_dump_char(( int)',');}}} void
Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void* _temp932=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp958; struct Cyc_Absyn_Structdecl* _temp960; struct
Cyc_Absyn_Uniondecl* _temp962; struct Cyc_Absyn_Vardecl* _temp964; struct Cyc_Absyn_Vardecl
_temp966; struct Cyc_List_List* _temp967; struct Cyc_Absyn_Exp* _temp969; void*
_temp971; struct Cyc_Absyn_Tqual _temp973; struct _tuple0* _temp975; void*
_temp977; struct Cyc_Absyn_Tuniondecl* _temp979; struct Cyc_Absyn_Tuniondecl
_temp981; int _temp982; struct Cyc_Core_Opt* _temp984; struct Cyc_List_List*
_temp986; struct _tuple0* _temp988; void* _temp990; struct Cyc_Absyn_Enumdecl*
_temp992; struct Cyc_Absyn_Enumdecl _temp994; struct Cyc_Core_Opt* _temp995;
struct _tuple0* _temp997; void* _temp999; struct Cyc_Absyn_Exp* _temp1001;
struct Cyc_Absyn_Pat* _temp1003; struct Cyc_List_List* _temp1005; struct Cyc_Absyn_Typedefdecl*
_temp1007; struct Cyc_List_List* _temp1009; struct _tagged_arr* _temp1011;
struct Cyc_List_List* _temp1013; struct _tuple0* _temp1015; struct Cyc_List_List*
_temp1017; _LL934: if(*(( int*) _temp932) ==  Cyc_Absyn_Fn_d){ _LL959: _temp958=((
struct Cyc_Absyn_Fn_d_struct*) _temp932)->f1; goto _LL935;} else{ goto _LL936;}
_LL936: if(*(( int*) _temp932) ==  Cyc_Absyn_Struct_d){ _LL961: _temp960=((
struct Cyc_Absyn_Struct_d_struct*) _temp932)->f1; goto _LL937;} else{ goto
_LL938;} _LL938: if(*(( int*) _temp932) ==  Cyc_Absyn_Union_d){ _LL963: _temp962=((
struct Cyc_Absyn_Union_d_struct*) _temp932)->f1; goto _LL939;} else{ goto _LL940;}
_LL940: if(*(( int*) _temp932) ==  Cyc_Absyn_Var_d){ _LL965: _temp964=(( struct
Cyc_Absyn_Var_d_struct*) _temp932)->f1; _temp966=* _temp964; _LL978: _temp977=(
void*) _temp966.sc; goto _LL976; _LL976: _temp975= _temp966.name; goto _LL974;
_LL974: _temp973= _temp966.tq; goto _LL972; _LL972: _temp971=( void*) _temp966.type;
goto _LL970; _LL970: _temp969= _temp966.initializer; goto _LL968; _LL968:
_temp967= _temp966.attributes; goto _LL941;} else{ goto _LL942;} _LL942: if(*((
int*) _temp932) ==  Cyc_Absyn_Tunion_d){ _LL980: _temp979=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp932)->f1; _temp981=* _temp979; _LL991: _temp990=( void*) _temp981.sc; goto
_LL989; _LL989: _temp988= _temp981.name; goto _LL987; _LL987: _temp986= _temp981.tvs;
goto _LL985; _LL985: _temp984= _temp981.fields; goto _LL983; _LL983: _temp982=
_temp981.is_xtunion; goto _LL943;} else{ goto _LL944;} _LL944: if(*(( int*)
_temp932) ==  Cyc_Absyn_Enum_d){ _LL993: _temp992=(( struct Cyc_Absyn_Enum_d_struct*)
_temp932)->f1; _temp994=* _temp992; _LL1000: _temp999=( void*) _temp994.sc; goto
_LL998; _LL998: _temp997= _temp994.name; goto _LL996; _LL996: _temp995= _temp994.fields;
goto _LL945;} else{ goto _LL946;} _LL946: if(*(( int*) _temp932) ==  Cyc_Absyn_Let_d){
_LL1004: _temp1003=(( struct Cyc_Absyn_Let_d_struct*) _temp932)->f1; goto
_LL1002; _LL1002: _temp1001=(( struct Cyc_Absyn_Let_d_struct*) _temp932)->f4;
goto _LL947;} else{ goto _LL948;} _LL948: if(*(( int*) _temp932) ==  Cyc_Absyn_Letv_d){
_LL1006: _temp1005=(( struct Cyc_Absyn_Letv_d_struct*) _temp932)->f1; goto
_LL949;} else{ goto _LL950;} _LL950: if(*(( int*) _temp932) ==  Cyc_Absyn_Typedef_d){
_LL1008: _temp1007=(( struct Cyc_Absyn_Typedef_d_struct*) _temp932)->f1; goto
_LL951;} else{ goto _LL952;} _LL952: if(*(( int*) _temp932) ==  Cyc_Absyn_Namespace_d){
_LL1012: _temp1011=(( struct Cyc_Absyn_Namespace_d_struct*) _temp932)->f1; goto
_LL1010; _LL1010: _temp1009=(( struct Cyc_Absyn_Namespace_d_struct*) _temp932)->f2;
goto _LL953;} else{ goto _LL954;} _LL954: if(*(( int*) _temp932) ==  Cyc_Absyn_Using_d){
_LL1016: _temp1015=(( struct Cyc_Absyn_Using_d_struct*) _temp932)->f1; goto
_LL1014; _LL1014: _temp1013=(( struct Cyc_Absyn_Using_d_struct*) _temp932)->f2;
goto _LL955;} else{ goto _LL956;} _LL956: if(*(( int*) _temp932) ==  Cyc_Absyn_ExternC_d){
_LL1018: _temp1017=(( struct Cyc_Absyn_ExternC_d_struct*) _temp932)->f1; goto
_LL957;} else{ goto _LL933;} _LL935: Cyc_Absyndump_dumploc( d->loc); if( Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts( _temp958->attributes);} if( _temp958->is_inline){ if(
Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump( _tag_arr("__inline", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("inline", sizeof(
unsigned char), 7u));}} Cyc_Absyndump_dumpscope(( void*) _temp958->sc);{ void* t=(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1020=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1020[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1021; _temp1021.tag= Cyc_Absyn_FnType; _temp1021.f1=({ struct Cyc_Absyn_FnInfo
_temp1022; _temp1022.tvars= _temp958->tvs; _temp1022.effect= _temp958->effect;
_temp1022.ret_typ=( void*)(( void*) _temp958->ret_type); _temp1022.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp958->args); _temp1022.c_varargs=
_temp958->c_varargs; _temp1022.cyc_varargs= _temp958->cyc_varargs; _temp1022.rgn_po=
_temp958->rgn_po; _temp1022.attributes= 0; _temp1022;}); _temp1021;}); _temp1020;});((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1019; _temp1019.q_const= 0; _temp1019.q_volatile= 0; _temp1019.q_restrict=
0; _temp1019;}), t, Cyc_Absyndump_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_temp958); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp958->body);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL933;} _LL937: Cyc_Absyndump_dumpscope((
void*) _temp960->sc); Cyc_Absyndump_dump( _tag_arr("struct", sizeof(
unsigned char), 7u)); if( _temp960->name !=  0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp960->name))->v);} Cyc_Absyndump_dumptvars(
_temp960->tvs); if( _temp960->fields ==  0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp960->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp960->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL933;
_LL939: Cyc_Absyndump_dumpscope(( void*) _temp962->sc); Cyc_Absyndump_dump(
_tag_arr("union", sizeof( unsigned char), 6u)); if( _temp962->name !=  0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp962->name))->v);} Cyc_Absyndump_dumptvars(
_temp962->tvs); if( _temp962->fields ==  0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp962->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp962->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL933;
_LL941: Cyc_Absyndump_dumploc( d->loc); if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts(
_temp967); Cyc_Absyndump_dumpscope( _temp977);{ struct Cyc_List_List* _temp1025;
void* _temp1027; struct Cyc_Absyn_Tqual _temp1029; struct _tuple4 _temp1023= Cyc_Absynpp_to_tms(
_temp973, _temp971); _LL1030: _temp1029= _temp1023.f1; goto _LL1028; _LL1028:
_temp1027= _temp1023.f2; goto _LL1026; _LL1026: _temp1025= _temp1023.f3; goto
_LL1024; _LL1024: { void* call_conv=( void*) Cyc_Absyn_Unused_att;{ struct Cyc_List_List*
tms2= _temp1025; for( 0; tms2 !=  0; tms2= tms2->tl){ void* _temp1031=( void*)
tms2->hd; struct Cyc_List_List* _temp1037; _LL1033: if(( unsigned int) _temp1031
>  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Attributes_mod: 0){ _LL1038: _temp1037=((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1031)->f2; goto _LL1034;} else{
goto _LL1035;} _LL1035: goto _LL1036; _LL1034: for( 0; _temp1037 !=  0;
_temp1037= _temp1037->tl){ void* _temp1039=( void*) _temp1037->hd; _LL1041: if(
_temp1039 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL1042;} else{ goto _LL1043;}
_LL1043: if( _temp1039 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL1044;} else{
goto _LL1045;} _LL1045: if( _temp1039 == ( void*) Cyc_Absyn_Fastcall_att){ goto
_LL1046;} else{ goto _LL1047;} _LL1047: goto _LL1048; _LL1042: call_conv=( void*)
Cyc_Absyn_Stdcall_att; goto _LL1040; _LL1044: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1040; _LL1046: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1040;
_LL1048: goto _LL1040; _LL1040:;} goto _LL1032; _LL1036: goto _LL1032; _LL1032:;}}
Cyc_Absyndump_dumptq( _temp1029); Cyc_Absyndump_dumpntyp( _temp1027);{ struct
_tuple10 _temp1049=({ struct _tuple10 _temp1050; _temp1050.f1= call_conv;
_temp1050.f2= _temp975; _temp1050;});(( void(*)( struct Cyc_List_List* tms, void(*
f)( struct _tuple10*), struct _tuple10* a)) Cyc_Absyndump_dumptms)( Cyc_List_imp_rev(
_temp1025), Cyc_Absyndump_dump_callconv_qvar,& _temp1049);}}}} else{ Cyc_Absyndump_dumpscope(
_temp977);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp973, _temp971, Cyc_Absyndump_dumpqvar,
_temp975); Cyc_Absyndump_dumpatts( _temp967);} if( _temp969 !=  0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp969));}
Cyc_Absyndump_dump_semi(); goto _LL933; _LL943: Cyc_Absyndump_dumpscope(
_temp990); if( _temp982){ Cyc_Absyndump_dump( _tag_arr("xtunion ", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("tunion ", sizeof(
unsigned char), 8u));} Cyc_Absyndump_dumpqvar( _temp988); Cyc_Absyndump_dumptvars(
_temp986); if( _temp984 ==  0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*) _temp984->v);
Cyc_Absyndump_dump_nospace( _tag_arr("};", sizeof( unsigned char), 3u));} goto
_LL933; _LL945: Cyc_Absyndump_dumpscope( _temp999); Cyc_Absyndump_dump( _tag_arr("enum ",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar( _temp997); if( _temp995 == 
0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*) _temp995->v); Cyc_Absyndump_dump_nospace( _tag_arr("};",
sizeof( unsigned char), 3u));} return; _LL947: Cyc_Absyndump_dump( _tag_arr("let",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumppat( _temp1003); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1001); Cyc_Absyndump_dump_semi(); goto
_LL933; _LL949: Cyc_Absyndump_dump( _tag_arr("let ", sizeof( unsigned char), 5u));
Cyc_Absyndump_dumpids( _temp1005); Cyc_Absyndump_dump_semi(); goto _LL933;
_LL951: if( ! Cyc_Absyndump_expand_typedefs? 1: Cyc_Absynpp_is_anon_structtype((
void*) _temp1007->defn)){ Cyc_Absyndump_dump( _tag_arr("typedef", sizeof(
unsigned char), 8u));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1051; _temp1051.q_const= 0; _temp1051.q_volatile= 0;
_temp1051.q_restrict= 0; _temp1051;}),( void*) _temp1007->defn, Cyc_Absyndump_dumptypedefname,
_temp1007); Cyc_Absyndump_dump_semi();} goto _LL933; _LL953: Cyc_Absyndump_dump(
_tag_arr("namespace", sizeof( unsigned char), 10u)); Cyc_Absyndump_dump_str(
_temp1011); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1009 !=  0;
_temp1009= _temp1009->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1009->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL933; _LL955: Cyc_Absyndump_dump(
_tag_arr("using", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp1015); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1013 !=  0;
_temp1013= _temp1013->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1013->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL933; _LL957: Cyc_Absyndump_dump(
_tag_arr("extern \"C\" {", sizeof( unsigned char), 13u)); for( 0; _temp1017 != 
0; _temp1017= _temp1017->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1017->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL933; _LL933:;}
static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i !=  1){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char(( int)'}');}} void
Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void* a){
if( tms ==  0){ f( a); return;}{ void* _temp1052=( void*) tms->hd; struct Cyc_Absyn_Tqual
_temp1070; void* _temp1072; void* _temp1074; struct Cyc_Absyn_Exp* _temp1076;
struct Cyc_Absyn_Tqual _temp1078; void* _temp1080; void* _temp1082; struct Cyc_Absyn_Exp*
_temp1084; struct Cyc_Absyn_Tqual _temp1086; void* _temp1088; void* _temp1090;
struct Cyc_Absyn_Tqual _temp1092; void* _temp1094; struct Cyc_Absyn_Tvar*
_temp1096; void* _temp1098; struct Cyc_Absyn_Exp* _temp1100; struct Cyc_Absyn_Tqual
_temp1102; void* _temp1104; struct Cyc_Absyn_Tvar* _temp1106; void* _temp1108;
struct Cyc_Absyn_Exp* _temp1110; struct Cyc_Absyn_Tqual _temp1112; void*
_temp1114; struct Cyc_Absyn_Tvar* _temp1116; void* _temp1118; _LL1054: if((
unsigned int) _temp1052 >  1u?*(( int*) _temp1052) ==  Cyc_Absyn_Pointer_mod: 0){
_LL1075: _temp1074=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1052)->f1;
if(( unsigned int) _temp1074 >  1u?*(( int*) _temp1074) ==  Cyc_Absyn_Nullable_ps:
0){ _LL1077: _temp1076=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1074)->f1;
goto _LL1073;} else{ goto _LL1056;} _LL1073: _temp1072=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f2; if( _temp1072 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1071;}
else{ goto _LL1056;} _LL1071: _temp1070=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f3; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(( unsigned int)
_temp1052 >  1u?*(( int*) _temp1052) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1083:
_temp1082=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1052)->f1; if((
unsigned int) _temp1082 >  1u?*(( int*) _temp1082) ==  Cyc_Absyn_NonNullable_ps:
0){ _LL1085: _temp1084=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1082)->f1;
goto _LL1081;} else{ goto _LL1058;} _LL1081: _temp1080=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f2; if( _temp1080 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1079;}
else{ goto _LL1058;} _LL1079: _temp1078=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f3; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(( unsigned int)
_temp1052 >  1u?*(( int*) _temp1052) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1091:
_temp1090=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1052)->f1; if(
_temp1090 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1089;} else{ goto
_LL1060;} _LL1089: _temp1088=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f2; if( _temp1088 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1087;}
else{ goto _LL1060;} _LL1087: _temp1086=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f3; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int)
_temp1052 >  1u?*(( int*) _temp1052) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1099:
_temp1098=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1052)->f1; if((
unsigned int) _temp1098 >  1u?*(( int*) _temp1098) ==  Cyc_Absyn_Nullable_ps: 0){
_LL1101: _temp1100=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1098)->f1; goto
_LL1095;} else{ goto _LL1062;} _LL1095: _temp1094=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f2; if(( unsigned int) _temp1094 >  3u?*(( int*) _temp1094) ==  Cyc_Absyn_VarType:
0){ _LL1097: _temp1096=(( struct Cyc_Absyn_VarType_struct*) _temp1094)->f1; goto
_LL1093;} else{ goto _LL1062;} _LL1093: _temp1092=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f3; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(( unsigned int)
_temp1052 >  1u?*(( int*) _temp1052) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1109:
_temp1108=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1052)->f1; if((
unsigned int) _temp1108 >  1u?*(( int*) _temp1108) ==  Cyc_Absyn_NonNullable_ps:
0){ _LL1111: _temp1110=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1108)->f1;
goto _LL1105;} else{ goto _LL1064;} _LL1105: _temp1104=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f2; if(( unsigned int) _temp1104 >  3u?*(( int*) _temp1104) ==  Cyc_Absyn_VarType:
0){ _LL1107: _temp1106=(( struct Cyc_Absyn_VarType_struct*) _temp1104)->f1; goto
_LL1103;} else{ goto _LL1064;} _LL1103: _temp1102=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f3; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(( unsigned int)
_temp1052 >  1u?*(( int*) _temp1052) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1119:
_temp1118=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1052)->f1; if(
_temp1118 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1115;} else{ goto
_LL1066;} _LL1115: _temp1114=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f2; if(( unsigned int) _temp1114 >  3u?*(( int*) _temp1114) ==  Cyc_Absyn_VarType:
0){ _LL1117: _temp1116=(( struct Cyc_Absyn_VarType_struct*) _temp1114)->f1; goto
_LL1113;} else{ goto _LL1066;} _LL1113: _temp1112=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1052)->f3; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(( unsigned int)
_temp1052 >  1u?*(( int*) _temp1052) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL1067;}
else{ goto _LL1068;} _LL1068: goto _LL1069; _LL1055: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1076); Cyc_Absyndump_dumptms( tms->tl,
f, a); return; _LL1057: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1084); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1059: Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1061: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1100); Cyc_Absyndump_dump_str(
_temp1096->name); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1063: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1110); Cyc_Absyndump_dump_str(
_temp1106->name); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1065: Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dump_str( _temp1116->name); Cyc_Absyndump_dumptms( tms->tl,
f, a); return; _LL1067:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1120=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1120[ 0]=({ struct Cyc_Core_Impossible_struct _temp1121; _temp1121.tag= Cyc_Core_Impossible;
_temp1121.f1= _tag_arr("dumptms: bad Pointer_mod", sizeof( unsigned char), 25u);
_temp1121;}); _temp1120;})); _LL1069: { int next_is_pointer= 0; if( tms->tl != 
0){ void* _temp1122=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd;
_LL1124: if(( unsigned int) _temp1122 >  1u?*(( int*) _temp1122) ==  Cyc_Absyn_Pointer_mod:
0){ goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1125:
next_is_pointer= 1; goto _LL1123; _LL1127: goto _LL1123; _LL1123:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');} Cyc_Absyndump_dumptms(
tms->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char(( int)')');}{ void*
_temp1128=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp1144; void* _temp1146;
struct Cyc_List_List* _temp1148; struct Cyc_Core_Opt* _temp1150; struct Cyc_Absyn_VarargInfo*
_temp1152; int _temp1154; struct Cyc_List_List* _temp1156; void* _temp1158;
struct Cyc_Position_Segment* _temp1160; struct Cyc_List_List* _temp1162; int
_temp1164; struct Cyc_Position_Segment* _temp1166; struct Cyc_List_List*
_temp1168; struct Cyc_List_List* _temp1170; void* _temp1172; void* _temp1174;
_LL1130: if( _temp1128 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL1131;} else{
goto _LL1132;} _LL1132: if(( unsigned int) _temp1128 >  1u?*(( int*) _temp1128)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL1145: _temp1144=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1128)->f1; goto _LL1133;} else{ goto _LL1134;} _LL1134: if(( unsigned int)
_temp1128 >  1u?*(( int*) _temp1128) ==  Cyc_Absyn_Function_mod: 0){ _LL1147:
_temp1146=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1128)->f1; if(*((
int*) _temp1146) ==  Cyc_Absyn_WithTypes){ _LL1157: _temp1156=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1146)->f1; goto _LL1155; _LL1155: _temp1154=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1146)->f2; goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1146)->f3; goto _LL1151; _LL1151: _temp1150=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1146)->f4; goto _LL1149; _LL1149: _temp1148=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1146)->f5; goto _LL1135;} else{ goto _LL1136;}} else{ goto _LL1136;}
_LL1136: if(( unsigned int) _temp1128 >  1u?*(( int*) _temp1128) ==  Cyc_Absyn_Function_mod:
0){ _LL1159: _temp1158=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1128)->f1; if(*(( int*) _temp1158) ==  Cyc_Absyn_NoTypes){ _LL1163:
_temp1162=(( struct Cyc_Absyn_NoTypes_struct*) _temp1158)->f1; goto _LL1161;
_LL1161: _temp1160=(( struct Cyc_Absyn_NoTypes_struct*) _temp1158)->f2; goto
_LL1137;} else{ goto _LL1138;}} else{ goto _LL1138;} _LL1138: if(( unsigned int)
_temp1128 >  1u?*(( int*) _temp1128) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL1169:
_temp1168=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1128)->f1; goto
_LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1128)->f2; goto _LL1165; _LL1165: _temp1164=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1128)->f3; goto _LL1139;} else{ goto _LL1140;} _LL1140: if(( unsigned int)
_temp1128 >  1u?*(( int*) _temp1128) ==  Cyc_Absyn_Attributes_mod: 0){ _LL1171:
_temp1170=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1128)->f2; goto
_LL1141;} else{ goto _LL1142;} _LL1142: if(( unsigned int) _temp1128 >  1u?*((
int*) _temp1128) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1175: _temp1174=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1128)->f1; goto _LL1173; _LL1173:
_temp1172=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1128)->f2; goto
_LL1143;} else{ goto _LL1129;} _LL1131: Cyc_Absyndump_dump( _tag_arr("[]",
sizeof( unsigned char), 3u)); goto _LL1129; _LL1133: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1144); Cyc_Absyndump_dump_char(( int)']');
goto _LL1129; _LL1135: Cyc_Absyndump_dumpfunargs( _temp1156, _temp1154,
_temp1152, _temp1150, _temp1148); goto _LL1129; _LL1137:(( void(*)( void(* f)(
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1162, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL1129;
_LL1139: if( _temp1164){ Cyc_Absyndump_dumpkindedtvars( _temp1168);} else{ Cyc_Absyndump_dumptvars(
_temp1168);} goto _LL1129; _LL1141: Cyc_Absyndump_dumpatts( _temp1170); goto
_LL1129; _LL1143:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1176=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1176[ 0]=({ struct Cyc_Core_Impossible_struct _temp1177; _temp1177.tag= Cyc_Core_Impossible;
_temp1177.f1= _tag_arr("dumptms", sizeof( unsigned char), 8u); _temp1177;});
_temp1176;})); _LL1129:;} return;} _LL1053:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1180; void* _temp1182; struct Cyc_Absyn_Tqual _temp1184; struct _tuple4
_temp1178= Cyc_Absynpp_to_tms( tq, t); _LL1185: _temp1184= _temp1178.f1; goto
_LL1183; _LL1183: _temp1182= _temp1178.f2; goto _LL1181; _LL1181: _temp1180=
_temp1178.f3; goto _LL1179; _LL1179: Cyc_Absyndump_dumptq( _temp1184); Cyc_Absyndump_dumpntyp(
_temp1182); Cyc_Absyndump_dumptms( Cyc_List_imp_rev( _temp1180), f, a);} void
Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct Cyc_Std___sFILE*
f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0; tdl !=  0; tdl=
tdl->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*) tdl->hd);}({ void*
_temp1186[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof( unsigned char), 2u),
_tag_arr( _temp1186, sizeof( void*), 0u));});}
