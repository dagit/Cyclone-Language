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
  struct _RegionPage *next;
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
};

extern struct _RegionHandle _new_region();
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
#  if !defined(RUNTIME_CYC)
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stdout; extern int Cyc_Std_fputc( int __c, struct Cyc_Std___sFILE*
__stream); extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct _tagged_arr* Cyc_Absynpp_cyc_stringptr; extern int Cyc_Absynpp_exp_prec(
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
static int Cyc_Absyndump_add_cyc_prefix; static int Cyc_Absyndump_to_VC; void
Cyc_Absyndump_set_params( struct Cyc_Absynpp_Params* fs){ Cyc_Absyndump_expand_typedefs=
fs->expand_typedefs; Cyc_Absyndump_qvar_to_Cids= fs->qvar_to_Cids; Cyc_Absyndump_add_cyc_prefix=
fs->add_cyc_prefix; Cyc_Absyndump_to_VC= fs->to_VC; Cyc_Absynpp_set_params( fs);}
void Cyc_Absyndump_dumptyp( void*); void Cyc_Absyndump_dumpntyp( void* t); void
Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumpexp_prec(
int, struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt*); void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*); void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms,
void(* f)( void*), void* a); void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual,
void*, void(* f)( void*), void*); void Cyc_Absyndump_dumpstructfields( struct
Cyc_List_List* fields); void Cyc_Absyndump_dumpenumfields( struct Cyc_List_List*
fields); struct Cyc_Std___sFILE** Cyc_Absyndump_dump_file=& Cyc_Std_stdout; void
Cyc_Absyndump_ignore( void* x){ return;} static unsigned int Cyc_Absyndump_pos=
0; void Cyc_Absyndump_dump( struct _tagged_arr s){ int sz=( int) _get_arr_size(
s, sizeof( unsigned char)); if( !(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), sz -  1)))){ -- sz;} Cyc_Absyndump_pos
+= sz +  1; if( Cyc_Absyndump_pos >  80){ Cyc_Absyndump_pos=( unsigned int) sz;
Cyc_Std_fputc(( int)'\n',* Cyc_Absyndump_dump_file);} else{ Cyc_Std_fputc(( int)' ',*
Cyc_Absyndump_dump_file);} Cyc_Std_file_string_write(* Cyc_Absyndump_dump_file,
s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct _tagged_arr s){ int sz=( int)
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
struct _tuple0* v){ struct Cyc_List_List* _temp0= 0;{ void* _temp1=(* v).f1;
struct Cyc_List_List* _temp9; struct Cyc_List_List* _temp11; _LL3: if( _temp1 == (
void*) Cyc_Absyn_Loc_n){ goto _LL4;} else{ goto _LL5;} _LL5: if(( unsigned int)
_temp1 >  1u?*(( int*) _temp1) ==  Cyc_Absyn_Rel_n: 0){ _LL10: _temp9=(( struct
Cyc_Absyn_Rel_n_struct*) _temp1)->f1; goto _LL6;} else{ goto _LL7;} _LL7: if((
unsigned int) _temp1 >  1u?*(( int*) _temp1) ==  Cyc_Absyn_Abs_n: 0){ _LL12:
_temp11=(( struct Cyc_Absyn_Abs_n_struct*) _temp1)->f1; goto _LL8;} else{ goto
_LL2;} _LL4: _temp9= 0; goto _LL6; _LL6: _temp0= _temp9; goto _LL2; _LL8: _temp0=(
Cyc_Absyndump_qvar_to_Cids? Cyc_Absyndump_add_cyc_prefix: 0)?({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp13->tl= _temp11; _temp13;}):
_temp11; goto _LL2; _LL2:;} if( _temp0 !=  0){ Cyc_Absyndump_dump_str(( struct
_tagged_arr*) _temp0->hd); for( _temp0= _temp0->tl; _temp0 !=  0; _temp0= _temp0->tl){
if( Cyc_Absyndump_qvar_to_Cids){ Cyc_Absyndump_dump_char(( int)'_');} else{ Cyc_Absyndump_dump_nospace(
_tag_arr("::", sizeof( unsigned char), 3u));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_arr*) _temp0->hd));} if( Cyc_Absyndump_qvar_to_Cids){ Cyc_Absyndump_dump_nospace(
_tag_arr("_", sizeof( unsigned char), 2u));} else{ Cyc_Absyndump_dump_nospace(
_tag_arr("::", sizeof( unsigned char), 3u));} Cyc_Absyndump_dump_nospace(*(* v).f2);}
else{ Cyc_Absyndump_dump_str((* v).f2);}} void Cyc_Absyndump_dumptq( struct Cyc_Absyn_Tqual
tq){ if( tq.q_restrict){ Cyc_Absyndump_dump( _tag_arr("restrict", sizeof(
unsigned char), 9u));} if( tq.q_volatile){ Cyc_Absyndump_dump( _tag_arr("volatile",
sizeof( unsigned char), 9u));} if( tq.q_const){ Cyc_Absyndump_dump( _tag_arr("const",
sizeof( unsigned char), 6u));}} void Cyc_Absyndump_dumpscope( void* sc){ void*
_temp14= sc; _LL16: if( _temp14 == ( void*) Cyc_Absyn_Static){ goto _LL17;}
else{ goto _LL18;} _LL18: if( _temp14 == ( void*) Cyc_Absyn_Public){ goto _LL19;}
else{ goto _LL20;} _LL20: if( _temp14 == ( void*) Cyc_Absyn_Extern){ goto _LL21;}
else{ goto _LL22;} _LL22: if( _temp14 == ( void*) Cyc_Absyn_ExternC){ goto _LL23;}
else{ goto _LL24;} _LL24: if( _temp14 == ( void*) Cyc_Absyn_Abstract){ goto
_LL25;} else{ goto _LL15;} _LL17: Cyc_Absyndump_dump( _tag_arr("static", sizeof(
unsigned char), 7u)); return; _LL19: return; _LL21: Cyc_Absyndump_dump( _tag_arr("extern",
sizeof( unsigned char), 7u)); return; _LL23: Cyc_Absyndump_dump( _tag_arr("extern \"C\"",
sizeof( unsigned char), 11u)); return; _LL25: Cyc_Absyndump_dump( _tag_arr("abstract",
sizeof( unsigned char), 9u)); return; _LL15:;} void Cyc_Absyndump_dumpkind( void*
k){ void* _temp26= k; _LL28: if( _temp26 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL29;} else{ goto _LL30;} _LL30: if( _temp26 == ( void*) Cyc_Absyn_MemKind){
goto _LL31;} else{ goto _LL32;} _LL32: if( _temp26 == ( void*) Cyc_Absyn_BoxKind){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp26 == ( void*) Cyc_Absyn_RgnKind){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp26 == ( void*) Cyc_Absyn_EffKind){
goto _LL37;} else{ goto _LL27;} _LL29: Cyc_Absyndump_dump( _tag_arr("A", sizeof(
unsigned char), 2u)); return; _LL31: Cyc_Absyndump_dump( _tag_arr("M", sizeof(
unsigned char), 2u)); return; _LL33: Cyc_Absyndump_dump( _tag_arr("B", sizeof(
unsigned char), 2u)); return; _LL35: Cyc_Absyndump_dump( _tag_arr("R", sizeof(
unsigned char), 2u)); return; _LL37: Cyc_Absyndump_dump( _tag_arr("E", sizeof(
unsigned char), 2u)); return; _LL27:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List*
ts){ Cyc_Absyndump_egroup( Cyc_Absyndump_dumptyp, ts, _tag_arr("<", sizeof(
unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar*
tv){ Cyc_Absyndump_dump_str( tv->name);} void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);{ void* _temp38=(
void*)( Cyc_Absyn_compress_conref( tv->kind))->v; void* _temp46; void* _temp48;
_LL40: if(( unsigned int) _temp38 >  1u?*(( int*) _temp38) ==  Cyc_Absyn_Eq_constr:
0){ _LL47: _temp46=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp38)->f1;
if( _temp46 == ( void*) Cyc_Absyn_BoxKind){ goto _LL41;} else{ goto _LL42;}}
else{ goto _LL42;} _LL42: if(( unsigned int) _temp38 >  1u?*(( int*) _temp38) == 
Cyc_Absyn_Eq_constr: 0){ _LL49: _temp48=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp38)->f1; goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL41: goto
_LL39; _LL43: Cyc_Absyndump_dump( _tag_arr("::", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpkind( _temp48); goto _LL39; _LL45: Cyc_Absyndump_dump(
_tag_arr("::?", sizeof( unsigned char), 4u)); goto _LL39; _LL39:;}} void Cyc_Absyndump_dumptvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar,
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
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts= atts->tl){ void* _temp50=(
void*) atts->hd; _LL52: if( _temp50 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL53;} else{ goto _LL54;} _LL54: if( _temp50 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL55;} else{ goto _LL56;} _LL56: if( _temp50 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL53: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); return; _LL55: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); return; _LL57: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); return; _LL59: goto _LL51;
_LL51:;}} void Cyc_Absyndump_dump_noncallconv( struct Cyc_List_List* atts){ int
hasatt= 0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 !=  0; atts2= atts2->tl){
void* _temp60=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL62:
if( _temp60 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL63;} else{ goto _LL64;}
_LL64: if( _temp60 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL65;} else{ goto
_LL66;} _LL66: if( _temp60 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL67;}
else{ goto _LL68;} _LL68: goto _LL69; _LL63: goto _LL61; _LL65: goto _LL61;
_LL67: goto _LL61; _LL69: hasatt= 1; goto _LL61; _LL61:;}} if( ! hasatt){
return;} Cyc_Absyndump_dump( _tag_arr("__declspec(", sizeof( unsigned char), 12u));
for( 0; atts !=  0; atts= atts->tl){ void* _temp70=( void*) atts->hd; _LL72: if(
_temp70 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL73;} else{ goto _LL74;}
_LL74: if( _temp70 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL75;} else{ goto
_LL76;} _LL76: if( _temp70 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL77;}
else{ goto _LL78;} _LL78: goto _LL79; _LL73: goto _LL71; _LL75: goto _LL71;
_LL77: goto _LL71; _LL79: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)
atts->hd)); goto _LL71; _LL71:;} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumpatts(
struct Cyc_List_List* atts){ if( atts ==  0){ return;} if( Cyc_Absyndump_to_VC){
Cyc_Absyndump_dump_noncallconv( atts); return;} Cyc_Absyndump_dump( _tag_arr(" __attribute__((",
sizeof( unsigned char), 17u)); for( 0; atts !=  0; atts= atts->tl){ Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string(( void*) atts->hd)); if( atts->tl !=  0){ Cyc_Absyndump_dump(
_tag_arr(",", sizeof( unsigned char), 2u));}} Cyc_Absyndump_dump( _tag_arr(")) ",
sizeof( unsigned char), 4u));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms ==  0){ return 0;}{ void* _temp80=( void*) tms->hd; _LL82: if((
unsigned int) _temp80 >  1u?*(( int*) _temp80) ==  Cyc_Absyn_Pointer_mod: 0){
goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL83: return 1; _LL85:
return 0; _LL81:;}} static void Cyc_Absyndump_dumprgn( void* t){ void* _temp86=
Cyc_Tcutil_compress( t); _LL88: if( _temp86 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL89;} else{ goto _LL90;} _LL90: goto _LL91; _LL89: Cyc_Absyndump_dump(
_tag_arr("`H", sizeof( unsigned char), 3u)); goto _LL87; _LL91: Cyc_Absyndump_dumpntyp(
t); goto _LL87; _LL87:;} struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
rgions= 0; struct Cyc_List_List* effects= 0;{ void* _temp92= Cyc_Tcutil_compress(
t); void* _temp100; struct Cyc_List_List* _temp102; _LL94: if(( unsigned int)
_temp92 >  3u?*(( int*) _temp92) ==  Cyc_Absyn_AccessEff: 0){ _LL101: _temp100=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp92)->f1; goto _LL95;} else{
goto _LL96;} _LL96: if(( unsigned int) _temp92 >  3u?*(( int*) _temp92) ==  Cyc_Absyn_JoinEff:
0){ _LL103: _temp102=(( struct Cyc_Absyn_JoinEff_struct*) _temp92)->f1; goto
_LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL95: rgions=({ struct Cyc_List_List*
_temp104=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp104->hd=( void*) _temp100; _temp104->tl= rgions; _temp104;}); goto _LL93;
_LL97: for( 0; _temp102 !=  0; _temp102= _temp102->tl){ struct Cyc_List_List*
_temp107; struct Cyc_List_List* _temp109; struct _tuple6 _temp105= Cyc_Absyndump_effects_split((
void*) _temp102->hd); _LL110: _temp109= _temp105.f1; goto _LL108; _LL108:
_temp107= _temp105.f2; goto _LL106; _LL106: rgions= Cyc_List_imp_append(
_temp109, rgions); effects= Cyc_List_imp_append( _temp107, effects);} goto _LL93;
_LL99: effects=({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp111->hd=( void*) t; _temp111->tl=
effects; _temp111;}); goto _LL93; _LL93:;} return({ struct _tuple6 _temp112;
_temp112.f1= rgions; _temp112.f2= effects; _temp112;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp115; struct Cyc_List_List* _temp117; struct
_tuple6 _temp113= Cyc_Absyndump_effects_split( t); _LL118: _temp117= _temp113.f1;
goto _LL116; _LL116: _temp115= _temp113.f2; goto _LL114; _LL114: _temp117= Cyc_List_imp_rev(
_temp117); _temp115= Cyc_List_imp_rev( _temp115); for( 0; _temp115 !=  0;
_temp115= _temp115->tl){ Cyc_Absyndump_dumpntyp(( void*) _temp115->hd); Cyc_Absyndump_dump_char((
int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp117 !=  0; _temp117=
_temp117->tl){ Cyc_Absyndump_dumprgn(( void*) _temp117->hd); if( _temp117->tl != 
0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');}
void Cyc_Absyndump_dumpntyp( void* t){ void* _temp119= t; struct Cyc_Absyn_Tvar*
_temp193; int _temp195; struct Cyc_Core_Opt* _temp197; struct Cyc_Core_Opt*
_temp199; int _temp201; struct Cyc_Core_Opt* _temp203; struct Cyc_Core_Opt
_temp205; void* _temp206; struct Cyc_Core_Opt* _temp208; struct Cyc_Absyn_TunionInfo
_temp210; void* _temp212; struct Cyc_List_List* _temp214; void* _temp216; struct
Cyc_Absyn_TunionFieldInfo _temp218; struct Cyc_List_List* _temp220; void*
_temp222; struct _tuple0* _temp224; void* _temp226; void* _temp228; void*
_temp230; void* _temp232; void* _temp234; void* _temp236; void* _temp238; void*
_temp240; void* _temp242; void* _temp244; void* _temp246; void* _temp248; void*
_temp251; void* _temp253; void* _temp255; void* _temp257; void* _temp260; void*
_temp262; void* _temp264; void* _temp266; int _temp268; struct Cyc_List_List*
_temp270; struct Cyc_List_List* _temp272; struct _tuple0* _temp274; struct Cyc_List_List*
_temp276; struct _tuple0* _temp278; struct Cyc_List_List* _temp280; struct
_tuple0* _temp282; struct Cyc_List_List* _temp284; struct _tuple0* _temp286;
struct Cyc_List_List* _temp288; struct Cyc_List_List* _temp290; struct Cyc_List_List*
_temp292; struct Cyc_Core_Opt* _temp294; struct Cyc_List_List* _temp296; struct
_tuple0* _temp298; void* _temp300; _LL121: if(( unsigned int) _temp119 >  3u?*((
int*) _temp119) ==  Cyc_Absyn_ArrayType: 0){ goto _LL122;} else{ goto _LL123;}
_LL123: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_FnType:
0){ goto _LL124;} else{ goto _LL125;} _LL125: if(( unsigned int) _temp119 >  3u?*((
int*) _temp119) ==  Cyc_Absyn_PointerType: 0){ goto _LL126;} else{ goto _LL127;}
_LL127: if( _temp119 == ( void*) Cyc_Absyn_VoidType){ goto _LL128;} else{ goto
_LL129;} _LL129: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_VarType:
0){ _LL194: _temp193=(( struct Cyc_Absyn_VarType_struct*) _temp119)->f1; goto
_LL130;} else{ goto _LL131;} _LL131: if(( unsigned int) _temp119 >  3u?*(( int*)
_temp119) ==  Cyc_Absyn_Evar: 0){ _LL200: _temp199=(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f1; goto _LL198; _LL198: _temp197=(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f2; if( _temp197 ==  0){ goto _LL196;} else{ goto _LL133;} _LL196:
_temp195=(( struct Cyc_Absyn_Evar_struct*) _temp119)->f3; goto _LL132;} else{
goto _LL133;} _LL133: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) == 
Cyc_Absyn_Evar: 0){ _LL209: _temp208=(( struct Cyc_Absyn_Evar_struct*) _temp119)->f1;
goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_Evar_struct*) _temp119)->f2;
if( _temp203 ==  0){ goto _LL135;} else{ _temp205=* _temp203; _LL207: _temp206=(
void*) _temp205.v; goto _LL202;} _LL202: _temp201=(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f3; goto _LL134;} else{ goto _LL135;} _LL135: if(( unsigned int)
_temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_TunionType: 0){ _LL211:
_temp210=(( struct Cyc_Absyn_TunionType_struct*) _temp119)->f1; _LL217: _temp216=(
void*) _temp210.tunion_info; goto _LL215; _LL215: _temp214= _temp210.targs; goto
_LL213; _LL213: _temp212=( void*) _temp210.rgn; goto _LL136;} else{ goto _LL137;}
_LL137: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_TunionFieldType:
0){ _LL219: _temp218=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp119)->f1;
_LL223: _temp222=( void*) _temp218.field_info; goto _LL221; _LL221: _temp220=
_temp218.targs; goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_EnumType: 0){ _LL225: _temp224=((
struct Cyc_Absyn_EnumType_struct*) _temp119)->f1; goto _LL140;} else{ goto
_LL141;} _LL141: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_IntType:
0){ _LL229: _temp228=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp119)->f1;
if( _temp228 == ( void*) Cyc_Absyn_Signed){ goto _LL227;} else{ goto _LL143;}
_LL227: _temp226=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp119)->f2; if(
_temp226 == ( void*) Cyc_Absyn_B4){ goto _LL142;} else{ goto _LL143;}} else{
goto _LL143;} _LL143: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) == 
Cyc_Absyn_IntType: 0){ _LL233: _temp232=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp232 == ( void*) Cyc_Absyn_Signed){ goto _LL231;} else{
goto _LL145;} _LL231: _temp230=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp230 == ( void*) Cyc_Absyn_B1){ goto _LL144;} else{ goto
_LL145;}} else{ goto _LL145;} _LL145: if(( unsigned int) _temp119 >  3u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL237: _temp236=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp236 == ( void*) Cyc_Absyn_Unsigned){ goto _LL235;} else{
goto _LL147;} _LL235: _temp234=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp234 == ( void*) Cyc_Absyn_B1){ goto _LL146;} else{ goto
_LL147;}} else{ goto _LL147;} _LL147: if(( unsigned int) _temp119 >  3u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL241: _temp240=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp240 == ( void*) Cyc_Absyn_Signed){ goto _LL239;} else{
goto _LL149;} _LL239: _temp238=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp238 == ( void*) Cyc_Absyn_B2){ goto _LL148;} else{ goto
_LL149;}} else{ goto _LL149;} _LL149: if(( unsigned int) _temp119 >  3u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL245: _temp244=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp244 == ( void*) Cyc_Absyn_Unsigned){ goto _LL243;} else{
goto _LL151;} _LL243: _temp242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp242 == ( void*) Cyc_Absyn_B2){ goto _LL150;} else{ goto
_LL151;}} else{ goto _LL151;} _LL151: if(( unsigned int) _temp119 >  3u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL249: _temp248=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp248 == ( void*) Cyc_Absyn_Unsigned){ goto _LL247;} else{
goto _LL153;} _LL247: _temp246=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp246 == ( void*) Cyc_Absyn_B4){ goto _LL152;} else{ goto
_LL153;}} else{ goto _LL153;} _LL153: if(( unsigned int) _temp119 >  3u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL254: _temp253=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp253 == ( void*) Cyc_Absyn_Signed){ goto _LL252;} else{
goto _LL155;} _LL252: _temp251=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp251 == ( void*) Cyc_Absyn_B8){ goto _LL250;} else{ goto
_LL155;}} else{ goto _LL155;} _LL250: if( Cyc_Absyndump_to_VC){ goto _LL154;}
else{ goto _LL155;} _LL155: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119)
==  Cyc_Absyn_IntType: 0){ _LL258: _temp257=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp257 == ( void*) Cyc_Absyn_Signed){ goto _LL256;} else{
goto _LL157;} _LL256: _temp255=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp255 == ( void*) Cyc_Absyn_B8){ goto _LL156;} else{ goto
_LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int) _temp119 >  3u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL263: _temp262=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp262 == ( void*) Cyc_Absyn_Unsigned){ goto _LL261;} else{
goto _LL159;} _LL261: _temp260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp260 == ( void*) Cyc_Absyn_B8){ goto _LL259;} else{ goto
_LL159;}} else{ goto _LL159;} _LL259: if( Cyc_Absyndump_to_VC){ goto _LL158;}
else{ goto _LL159;} _LL159: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119)
==  Cyc_Absyn_IntType: 0){ _LL267: _temp266=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp266 == ( void*) Cyc_Absyn_Unsigned){ goto _LL265;} else{
goto _LL161;} _LL265: _temp264=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp264 == ( void*) Cyc_Absyn_B8){ goto _LL160;} else{ goto
_LL161;}} else{ goto _LL161;} _LL161: if( _temp119 == ( void*) Cyc_Absyn_FloatType){
goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int) _temp119 >  3u?*((
int*) _temp119) ==  Cyc_Absyn_DoubleType: 0){ _LL269: _temp268=(( struct Cyc_Absyn_DoubleType_struct*)
_temp119)->f1; goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int)
_temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_TupleType: 0){ _LL271: _temp270=((
struct Cyc_Absyn_TupleType_struct*) _temp119)->f1; goto _LL166;} else{ goto
_LL167;} _LL167: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_StructType:
0){ _LL275: _temp274=(( struct Cyc_Absyn_StructType_struct*) _temp119)->f1; if(
_temp274 ==  0){ goto _LL273;} else{ goto _LL169;} _LL273: _temp272=(( struct
Cyc_Absyn_StructType_struct*) _temp119)->f2; goto _LL168;} else{ goto _LL169;}
_LL169: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_StructType:
0){ _LL279: _temp278=(( struct Cyc_Absyn_StructType_struct*) _temp119)->f1; goto
_LL277; _LL277: _temp276=(( struct Cyc_Absyn_StructType_struct*) _temp119)->f2;
goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp119 >  3u?*((
int*) _temp119) ==  Cyc_Absyn_UnionType: 0){ _LL283: _temp282=(( struct Cyc_Absyn_UnionType_struct*)
_temp119)->f1; if( _temp282 ==  0){ goto _LL281;} else{ goto _LL173;} _LL281:
_temp280=(( struct Cyc_Absyn_UnionType_struct*) _temp119)->f2; goto _LL172;}
else{ goto _LL173;} _LL173: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119)
==  Cyc_Absyn_UnionType: 0){ _LL287: _temp286=(( struct Cyc_Absyn_UnionType_struct*)
_temp119)->f1; goto _LL285; _LL285: _temp284=(( struct Cyc_Absyn_UnionType_struct*)
_temp119)->f2; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_AnonStructType: 0){ _LL289:
_temp288=(( struct Cyc_Absyn_AnonStructType_struct*) _temp119)->f1; goto _LL176;}
else{ goto _LL177;} _LL177: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119)
==  Cyc_Absyn_AnonUnionType: 0){ _LL291: _temp290=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp119)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_AnonEnumType: 0){ _LL293:
_temp292=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp119)->f1; goto _LL180;}
else{ goto _LL181;} _LL181: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119)
==  Cyc_Absyn_TypedefType: 0){ _LL299: _temp298=(( struct Cyc_Absyn_TypedefType_struct*)
_temp119)->f1; goto _LL297; _LL297: _temp296=(( struct Cyc_Absyn_TypedefType_struct*)
_temp119)->f2; goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_TypedefType_struct*)
_temp119)->f3; goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int)
_temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_RgnHandleType: 0){ _LL301:
_temp300=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp119)->f1; goto
_LL184;} else{ goto _LL185;} _LL185: if( _temp119 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int) _temp119 >  3u?*((
int*) _temp119) ==  Cyc_Absyn_AccessEff: 0){ goto _LL188;} else{ goto _LL189;}
_LL189: if(( unsigned int) _temp119 >  3u?*(( int*) _temp119) ==  Cyc_Absyn_RgnsEff:
0){ goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int) _temp119 >  3u?*((
int*) _temp119) ==  Cyc_Absyn_JoinEff: 0){ goto _LL192;} else{ goto _LL120;}
_LL122: return; _LL124: return; _LL126: return; _LL128: Cyc_Absyndump_dump(
_tag_arr("void", sizeof( unsigned char), 5u)); return; _LL130: Cyc_Absyndump_dump_str(
_temp193->name); return; _LL132: Cyc_Absyndump_dump( _tag_arr("%", sizeof(
unsigned char), 2u)); if( _temp199 ==  0){ Cyc_Absyndump_dump( _tag_arr("?",
sizeof( unsigned char), 2u));} else{ Cyc_Absyndump_dumpkind(( void*) _temp199->v);}
Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp303;
_temp303.tag= Cyc_Std_Int_pa; _temp303.f1=( int)(( unsigned int) _temp195);{
void* _temp302[ 1u]={& _temp303}; Cyc_Std_aprintf( _tag_arr("(%d)", sizeof(
unsigned char), 5u), _tag_arr( _temp302, sizeof( void*), 1u));}})); return;
_LL134: Cyc_Absyndump_dumpntyp( _temp206); return; _LL136:{ void* _temp304=
_temp216; struct Cyc_Absyn_UnknownTunionInfo _temp310; int _temp312; struct
_tuple0* _temp314; struct Cyc_Absyn_Tuniondecl** _temp316; struct Cyc_Absyn_Tuniondecl*
_temp318; struct Cyc_Absyn_Tuniondecl _temp319; int _temp320; struct _tuple0*
_temp322; _LL306: if(*(( int*) _temp304) ==  Cyc_Absyn_UnknownTunion){ _LL311:
_temp310=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp304)->f1; _LL315:
_temp314= _temp310.name; goto _LL313; _LL313: _temp312= _temp310.is_xtunion;
goto _LL307;} else{ goto _LL308;} _LL308: if(*(( int*) _temp304) ==  Cyc_Absyn_KnownTunion){
_LL317: _temp316=(( struct Cyc_Absyn_KnownTunion_struct*) _temp304)->f1;
_temp318=* _temp316; _temp319=* _temp318; _LL323: _temp322= _temp319.name; goto
_LL321; _LL321: _temp320= _temp319.is_xtunion; goto _LL309;} else{ goto _LL305;}
_LL307: _temp322= _temp314; _temp320= _temp312; goto _LL309; _LL309: if(
_temp320){ Cyc_Absyndump_dump( _tag_arr("xtunion ", sizeof( unsigned char), 9u));}
else{ Cyc_Absyndump_dump( _tag_arr("tunion ", sizeof( unsigned char), 8u));}{
void* _temp324= Cyc_Tcutil_compress( _temp212); _LL326: if( _temp324 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL327;} else{ goto _LL328;} _LL328: goto _LL329;
_LL327: goto _LL325; _LL329: Cyc_Absyndump_dumptyp( _temp212); Cyc_Absyndump_dump(
_tag_arr(" ", sizeof( unsigned char), 2u)); goto _LL325; _LL325:;} Cyc_Absyndump_dumpqvar(
_temp322); Cyc_Absyndump_dumptps( _temp214); goto _LL305; _LL305:;} goto _LL120;
_LL138:{ void* _temp330= _temp222; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp336; int _temp338; struct _tuple0* _temp340; struct _tuple0* _temp342;
struct Cyc_Absyn_Tunionfield* _temp344; struct Cyc_Absyn_Tunionfield _temp346;
struct _tuple0* _temp347; struct Cyc_Absyn_Tuniondecl* _temp349; struct Cyc_Absyn_Tuniondecl
_temp351; int _temp352; struct _tuple0* _temp354; _LL332: if(*(( int*) _temp330)
==  Cyc_Absyn_UnknownTunionfield){ _LL337: _temp336=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp330)->f1; _LL343: _temp342= _temp336.tunion_name; goto _LL341; _LL341:
_temp340= _temp336.field_name; goto _LL339; _LL339: _temp338= _temp336.is_xtunion;
goto _LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp330) ==  Cyc_Absyn_KnownTunionfield){
_LL350: _temp349=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp330)->f1;
_temp351=* _temp349; _LL355: _temp354= _temp351.name; goto _LL353; _LL353:
_temp352= _temp351.is_xtunion; goto _LL345; _LL345: _temp344=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp330)->f2; _temp346=* _temp344; _LL348: _temp347= _temp346.name; goto _LL335;}
else{ goto _LL331;} _LL333: _temp354= _temp342; _temp352= _temp338; _temp347=
_temp340; goto _LL335; _LL335: if( _temp352){ Cyc_Absyndump_dump( _tag_arr("xtunion ",
sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("tunion ",
sizeof( unsigned char), 8u));} Cyc_Absyndump_dumpqvar( _temp354); Cyc_Absyndump_dump(
_tag_arr(".", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar( _temp347);
Cyc_Absyndump_dumptps( _temp220); goto _LL331; _LL331:;} goto _LL120; _LL140:
Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp224); return; _LL142: Cyc_Absyndump_dump( _tag_arr("int", sizeof(
unsigned char), 4u)); return; _LL144: Cyc_Absyndump_dump( _tag_arr("signed char",
sizeof( unsigned char), 12u)); return; _LL146: Cyc_Absyndump_dump( _tag_arr("unsigned char",
sizeof( unsigned char), 14u)); return; _LL148: Cyc_Absyndump_dump( _tag_arr("short",
sizeof( unsigned char), 6u)); return; _LL150: Cyc_Absyndump_dump( _tag_arr("unsigned short",
sizeof( unsigned char), 15u)); return; _LL152: Cyc_Absyndump_dump( _tag_arr("unsigned int",
sizeof( unsigned char), 13u)); return; _LL154: Cyc_Absyndump_dump( _tag_arr("__int64",
sizeof( unsigned char), 8u)); return; _LL156: Cyc_Absyndump_dump( _tag_arr("long long",
sizeof( unsigned char), 10u)); return; _LL158: Cyc_Absyndump_dump( _tag_arr("unsigned __int64",
sizeof( unsigned char), 17u)); return; _LL160: Cyc_Absyndump_dump( _tag_arr("unsigned long long",
sizeof( unsigned char), 19u)); return; _LL162: Cyc_Absyndump_dump( _tag_arr("float",
sizeof( unsigned char), 6u)); return; _LL164: if( _temp268){ Cyc_Absyndump_dump(
_tag_arr("long double", sizeof( unsigned char), 12u));} else{ Cyc_Absyndump_dump(
_tag_arr("double", sizeof( unsigned char), 7u));} return; _LL166: Cyc_Absyndump_dump_char((
int)'$'); Cyc_Absyndump_dumpargs( _temp270); return; _LL168: Cyc_Absyndump_dump(
_tag_arr("struct", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumptps( _temp272);
return; _LL170: Cyc_Absyndump_dump( _tag_arr("struct", sizeof( unsigned char), 7u));
Cyc_Absyndump_dumpqvar(( struct _tuple0*) _check_null( _temp278)); Cyc_Absyndump_dumptps(
_temp276); return; _LL172: Cyc_Absyndump_dump( _tag_arr("union", sizeof(
unsigned char), 6u)); Cyc_Absyndump_dumptps( _temp280); return; _LL174: Cyc_Absyndump_dump(
_tag_arr("union", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(( struct
_tuple0*) _check_null( _temp286)); Cyc_Absyndump_dumptps( _temp284); return;
_LL176: Cyc_Absyndump_dump( _tag_arr("struct {", sizeof( unsigned char), 9u));
Cyc_Absyndump_dumpstructfields( _temp288); Cyc_Absyndump_dump( _tag_arr("}",
sizeof( unsigned char), 2u)); return; _LL178: Cyc_Absyndump_dump( _tag_arr("union {",
sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpstructfields( _temp290); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); return; _LL180: Cyc_Absyndump_dump(
_tag_arr("enum {", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpenumfields(
_temp292); Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u));
return; _LL182:( Cyc_Absyndump_dumpqvar( _temp298), Cyc_Absyndump_dumptps(
_temp296)); return; _LL184: Cyc_Absyndump_dumprgn( _temp300); return; _LL186:
return; _LL188: return; _LL190: return; _LL192: return; _LL120:;} void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt* vo){ if( vo !=  0){ Cyc_Absyndump_dump_str(( struct
_tagged_arr*) vo->v);}} void Cyc_Absyndump_dumpfunarg( struct _tuple1* t){((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Core_Opt*), struct
Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(*
t).f1);} struct _tuple7{ void* f1; void* f2; } ; void Cyc_Absyndump_dump_rgncmp(
struct _tuple7* cmp){ struct _tuple7 _temp358; void* _temp359; void* _temp361;
struct _tuple7* _temp356= cmp; _temp358=* _temp356; _LL362: _temp361= _temp358.f1;
goto _LL360; _LL360: _temp359= _temp358.f2; goto _LL357; _LL357: Cyc_Absyndump_dumptyp(
_temp361); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumptyp( _temp359);}
void Cyc_Absyndump_dump_rgnpo( struct Cyc_List_List* rgn_po){(( void(*)( void(*
f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp, rgn_po, _tag_arr(",", sizeof( unsigned char), 2u));}
void Cyc_Absyndump_dumpfunargs( struct Cyc_List_List* args, int c_varargs,
struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt* effopt, struct
Cyc_List_List* rgn_po){ Cyc_Absyndump_dump_char(( int)'('); for( 0; args !=  0;
args= args->tl){ Cyc_Absyndump_dumpfunarg(( struct _tuple1*) args->hd); if((
args->tl !=  0? 1: c_varargs)? 1: cyc_varargs !=  0){ Cyc_Absyndump_dump_char((
int)',');}} if( c_varargs){ Cyc_Absyndump_dump( _tag_arr("...", sizeof(
unsigned char), 4u));} else{ if( cyc_varargs !=  0){ struct _tuple1* _temp363=({
struct _tuple1* _temp364=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp364->f1= cyc_varargs->name; _temp364->f2= cyc_varargs->tq; _temp364->f3=(
void*) cyc_varargs->type; _temp364;}); Cyc_Absyndump_dump( _tag_arr("...",
sizeof( unsigned char), 4u)); if( cyc_varargs->inject){ Cyc_Absyndump_dump(
_tag_arr(" inject ", sizeof( unsigned char), 9u));} Cyc_Absyndump_dumpfunarg(
_temp363);}} if( effopt !=  0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff((
void*) effopt->v);} if( rgn_po !=  0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dump_rgnpo(
rgn_po);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp( void*
t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp365; _temp365.q_const= 0; _temp365.q_volatile= 0;
_temp365.q_restrict= 0; _temp365;}), t,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpdesignator( void* d){ void* _temp366= d; struct Cyc_Absyn_Exp*
_temp372; struct _tagged_arr* _temp374; _LL368: if(*(( int*) _temp366) ==  Cyc_Absyn_ArrayElement){
_LL373: _temp372=(( struct Cyc_Absyn_ArrayElement_struct*) _temp366)->f1; goto
_LL369;} else{ goto _LL370;} _LL370: if(*(( int*) _temp366) ==  Cyc_Absyn_FieldName){
_LL375: _temp374=(( struct Cyc_Absyn_FieldName_struct*) _temp366)->f1; goto
_LL371;} else{ goto _LL367;} _LL369: Cyc_Absyndump_dump( _tag_arr(".[", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp( _temp372); Cyc_Absyndump_dump_char((
int)']'); goto _LL367; _LL371: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp374); goto _LL367; _LL367:;} struct _tuple8{ struct Cyc_List_List* f1;
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
unsigned char), 2u));}{ void* _temp376=( void*) e->r; void* _temp476;
unsigned char _temp478; void* _temp480; void* _temp482; short _temp484; void*
_temp486; void* _temp488; int _temp490; void* _temp492; void* _temp494; int
_temp496; void* _temp498; void* _temp500; long long _temp502; void* _temp504;
void* _temp506; struct _tagged_arr _temp508; void* _temp510; void* _temp512;
struct _tagged_arr _temp514; struct _tuple0* _temp516; struct _tuple0* _temp518;
struct Cyc_List_List* _temp520; void* _temp522; struct Cyc_Absyn_Exp* _temp524;
struct Cyc_Core_Opt* _temp526; struct Cyc_Absyn_Exp* _temp528; void* _temp530;
struct Cyc_Absyn_Exp* _temp532; void* _temp534; struct Cyc_Absyn_Exp* _temp536;
void* _temp538; struct Cyc_Absyn_Exp* _temp540; void* _temp542; struct Cyc_Absyn_Exp*
_temp544; struct Cyc_Absyn_Exp* _temp546; struct Cyc_Absyn_Exp* _temp548; struct
Cyc_Absyn_Exp* _temp550; struct Cyc_Absyn_Exp* _temp552; struct Cyc_Absyn_Exp*
_temp554; struct Cyc_List_List* _temp556; struct Cyc_Absyn_Exp* _temp558; struct
Cyc_List_List* _temp560; struct Cyc_Absyn_Exp* _temp562; struct Cyc_Absyn_Exp*
_temp564; struct Cyc_Absyn_Exp* _temp566; struct Cyc_Absyn_Exp* _temp568; struct
Cyc_Absyn_Exp* _temp570; void* _temp572; struct Cyc_Absyn_Exp* _temp574; struct
Cyc_Absyn_Exp* _temp576; struct Cyc_Absyn_Exp* _temp578; void* _temp580; struct
Cyc_Absyn_Exp* _temp582; void* _temp584; struct _tagged_arr* _temp586; void*
_temp588; void* _temp590; unsigned int _temp592; void* _temp594; void* _temp596;
struct Cyc_List_List* _temp598; struct Cyc_Absyn_Exp* _temp600; struct
_tagged_arr* _temp602; struct Cyc_Absyn_Exp* _temp604; struct _tagged_arr*
_temp606; struct Cyc_Absyn_Exp* _temp608; struct Cyc_Absyn_Exp* _temp610; struct
Cyc_Absyn_Exp* _temp612; struct Cyc_List_List* _temp614; struct Cyc_List_List*
_temp616; struct _tuple1* _temp618; struct Cyc_List_List* _temp620; struct Cyc_Absyn_Exp*
_temp622; struct Cyc_Absyn_Exp* _temp624; struct Cyc_Absyn_Vardecl* _temp626;
struct Cyc_List_List* _temp628; struct _tuple0* _temp630; struct Cyc_List_List*
_temp632; struct Cyc_Absyn_Tunionfield* _temp634; struct Cyc_List_List* _temp636;
struct _tuple0* _temp638; struct _tuple0* _temp640; void* _temp642; struct Cyc_Absyn_Exp*
_temp644; struct Cyc_List_List* _temp646; struct Cyc_Core_Opt* _temp648; struct
Cyc_Absyn_Stmt* _temp650; struct Cyc_Absyn_Fndecl* _temp652; struct Cyc_Absyn_Exp*
_temp654; _LL378: if(*(( int*) _temp376) ==  Cyc_Absyn_Const_e){ _LL477:
_temp476=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp376)->f1; if((
unsigned int) _temp476 >  1u?*(( int*) _temp476) ==  Cyc_Absyn_Char_c: 0){
_LL481: _temp480=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp476)->f1; goto
_LL479; _LL479: _temp478=(( struct Cyc_Absyn_Char_c_struct*) _temp476)->f2; goto
_LL379;} else{ goto _LL380;}} else{ goto _LL380;} _LL380: if(*(( int*) _temp376)
==  Cyc_Absyn_Const_e){ _LL483: _temp482=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp376)->f1; if(( unsigned int) _temp482 >  1u?*(( int*) _temp482) ==  Cyc_Absyn_Short_c:
0){ _LL487: _temp486=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp482)->f1;
goto _LL485; _LL485: _temp484=(( struct Cyc_Absyn_Short_c_struct*) _temp482)->f2;
goto _LL381;} else{ goto _LL382;}} else{ goto _LL382;} _LL382: if(*(( int*)
_temp376) ==  Cyc_Absyn_Const_e){ _LL489: _temp488=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp376)->f1; if(( unsigned int) _temp488 >  1u?*(( int*) _temp488) ==  Cyc_Absyn_Int_c:
0){ _LL493: _temp492=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp488)->f1;
if( _temp492 == ( void*) Cyc_Absyn_Signed){ goto _LL491;} else{ goto _LL384;}
_LL491: _temp490=(( struct Cyc_Absyn_Int_c_struct*) _temp488)->f2; goto _LL383;}
else{ goto _LL384;}} else{ goto _LL384;} _LL384: if(*(( int*) _temp376) ==  Cyc_Absyn_Const_e){
_LL495: _temp494=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp376)->f1; if((
unsigned int) _temp494 >  1u?*(( int*) _temp494) ==  Cyc_Absyn_Int_c: 0){ _LL499:
_temp498=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp494)->f1; if( _temp498
== ( void*) Cyc_Absyn_Unsigned){ goto _LL497;} else{ goto _LL386;} _LL497:
_temp496=(( struct Cyc_Absyn_Int_c_struct*) _temp494)->f2; goto _LL385;} else{
goto _LL386;}} else{ goto _LL386;} _LL386: if(*(( int*) _temp376) ==  Cyc_Absyn_Const_e){
_LL501: _temp500=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp376)->f1; if((
unsigned int) _temp500 >  1u?*(( int*) _temp500) ==  Cyc_Absyn_LongLong_c: 0){
_LL505: _temp504=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp500)->f1;
goto _LL503; _LL503: _temp502=(( struct Cyc_Absyn_LongLong_c_struct*) _temp500)->f2;
goto _LL387;} else{ goto _LL388;}} else{ goto _LL388;} _LL388: if(*(( int*)
_temp376) ==  Cyc_Absyn_Const_e){ _LL507: _temp506=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp376)->f1; if(( unsigned int) _temp506 >  1u?*(( int*) _temp506) ==  Cyc_Absyn_Float_c:
0){ _LL509: _temp508=(( struct Cyc_Absyn_Float_c_struct*) _temp506)->f1; goto
_LL389;} else{ goto _LL390;}} else{ goto _LL390;} _LL390: if(*(( int*) _temp376)
==  Cyc_Absyn_Const_e){ _LL511: _temp510=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp376)->f1; if( _temp510 == ( void*) Cyc_Absyn_Null_c){ goto _LL391;} else{
goto _LL392;}} else{ goto _LL392;} _LL392: if(*(( int*) _temp376) ==  Cyc_Absyn_Const_e){
_LL513: _temp512=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp376)->f1; if((
unsigned int) _temp512 >  1u?*(( int*) _temp512) ==  Cyc_Absyn_String_c: 0){
_LL515: _temp514=(( struct Cyc_Absyn_String_c_struct*) _temp512)->f1; goto
_LL393;} else{ goto _LL394;}} else{ goto _LL394;} _LL394: if(*(( int*) _temp376)
==  Cyc_Absyn_UnknownId_e){ _LL517: _temp516=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp376)->f1; goto _LL395;} else{ goto _LL396;} _LL396: if(*(( int*) _temp376)
==  Cyc_Absyn_Var_e){ _LL519: _temp518=(( struct Cyc_Absyn_Var_e_struct*)
_temp376)->f1; goto _LL397;} else{ goto _LL398;} _LL398: if(*(( int*) _temp376)
==  Cyc_Absyn_Primop_e){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp376)->f1; goto _LL521; _LL521: _temp520=(( struct Cyc_Absyn_Primop_e_struct*)
_temp376)->f2; goto _LL399;} else{ goto _LL400;} _LL400: if(*(( int*) _temp376)
==  Cyc_Absyn_AssignOp_e){ _LL529: _temp528=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp376)->f1; goto _LL527; _LL527: _temp526=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp376)->f2; goto _LL525; _LL525: _temp524=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp376)->f3; goto _LL401;} else{ goto _LL402;} _LL402: if(*(( int*) _temp376)
==  Cyc_Absyn_Increment_e){ _LL533: _temp532=(( struct Cyc_Absyn_Increment_e_struct*)
_temp376)->f1; goto _LL531; _LL531: _temp530=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp376)->f2; if( _temp530 == ( void*) Cyc_Absyn_PreInc){ goto _LL403;} else{
goto _LL404;}} else{ goto _LL404;} _LL404: if(*(( int*) _temp376) ==  Cyc_Absyn_Increment_e){
_LL537: _temp536=(( struct Cyc_Absyn_Increment_e_struct*) _temp376)->f1; goto
_LL535; _LL535: _temp534=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp376)->f2; if( _temp534 == ( void*) Cyc_Absyn_PreDec){ goto _LL405;} else{
goto _LL406;}} else{ goto _LL406;} _LL406: if(*(( int*) _temp376) ==  Cyc_Absyn_Increment_e){
_LL541: _temp540=(( struct Cyc_Absyn_Increment_e_struct*) _temp376)->f1; goto
_LL539; _LL539: _temp538=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp376)->f2; if( _temp538 == ( void*) Cyc_Absyn_PostInc){ goto _LL407;} else{
goto _LL408;}} else{ goto _LL408;} _LL408: if(*(( int*) _temp376) ==  Cyc_Absyn_Increment_e){
_LL545: _temp544=(( struct Cyc_Absyn_Increment_e_struct*) _temp376)->f1; goto
_LL543; _LL543: _temp542=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp376)->f2; if( _temp542 == ( void*) Cyc_Absyn_PostDec){ goto _LL409;} else{
goto _LL410;}} else{ goto _LL410;} _LL410: if(*(( int*) _temp376) ==  Cyc_Absyn_Conditional_e){
_LL551: _temp550=(( struct Cyc_Absyn_Conditional_e_struct*) _temp376)->f1; goto
_LL549; _LL549: _temp548=(( struct Cyc_Absyn_Conditional_e_struct*) _temp376)->f2;
goto _LL547; _LL547: _temp546=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp376)->f3; goto _LL411;} else{ goto _LL412;} _LL412: if(*(( int*) _temp376)
==  Cyc_Absyn_SeqExp_e){ _LL555: _temp554=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp376)->f1; goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp376)->f2; goto _LL413;} else{ goto _LL414;} _LL414: if(*(( int*) _temp376)
==  Cyc_Absyn_UnknownCall_e){ _LL559: _temp558=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp376)->f1; goto _LL557; _LL557: _temp556=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp376)->f2; goto _LL415;} else{ goto _LL416;} _LL416: if(*(( int*) _temp376)
==  Cyc_Absyn_FnCall_e){ _LL563: _temp562=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp376)->f1; goto _LL561; _LL561: _temp560=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp376)->f2; goto _LL417;} else{ goto _LL418;} _LL418: if(*(( int*) _temp376)
==  Cyc_Absyn_Throw_e){ _LL565: _temp564=(( struct Cyc_Absyn_Throw_e_struct*)
_temp376)->f1; goto _LL419;} else{ goto _LL420;} _LL420: if(*(( int*) _temp376)
==  Cyc_Absyn_NoInstantiate_e){ _LL567: _temp566=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp376)->f1; goto _LL421;} else{ goto _LL422;} _LL422: if(*(( int*) _temp376)
==  Cyc_Absyn_Instantiate_e){ _LL569: _temp568=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp376)->f1; goto _LL423;} else{ goto _LL424;} _LL424: if(*(( int*) _temp376)
==  Cyc_Absyn_Cast_e){ _LL573: _temp572=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp376)->f1; goto _LL571; _LL571: _temp570=(( struct Cyc_Absyn_Cast_e_struct*)
_temp376)->f2; goto _LL425;} else{ goto _LL426;} _LL426: if(*(( int*) _temp376)
==  Cyc_Absyn_Address_e){ _LL575: _temp574=(( struct Cyc_Absyn_Address_e_struct*)
_temp376)->f1; goto _LL427;} else{ goto _LL428;} _LL428: if(*(( int*) _temp376)
==  Cyc_Absyn_New_e){ _LL579: _temp578=(( struct Cyc_Absyn_New_e_struct*)
_temp376)->f1; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_New_e_struct*)
_temp376)->f2; goto _LL429;} else{ goto _LL430;} _LL430: if(*(( int*) _temp376)
==  Cyc_Absyn_Sizeoftyp_e){ _LL581: _temp580=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp376)->f1; goto _LL431;} else{ goto _LL432;} _LL432: if(*(( int*) _temp376)
==  Cyc_Absyn_Sizeofexp_e){ _LL583: _temp582=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp376)->f1; goto _LL433;} else{ goto _LL434;} _LL434: if(*(( int*) _temp376)
==  Cyc_Absyn_Offsetof_e){ _LL589: _temp588=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp376)->f1; goto _LL585; _LL585: _temp584=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp376)->f2; if(*(( int*) _temp584) ==  Cyc_Absyn_StructField){ _LL587:
_temp586=(( struct Cyc_Absyn_StructField_struct*) _temp584)->f1; goto _LL435;}
else{ goto _LL436;}} else{ goto _LL436;} _LL436: if(*(( int*) _temp376) ==  Cyc_Absyn_Offsetof_e){
_LL595: _temp594=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp376)->f1;
goto _LL591; _LL591: _temp590=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp376)->f2; if(*(( int*) _temp590) ==  Cyc_Absyn_TupleIndex){ _LL593:
_temp592=(( struct Cyc_Absyn_TupleIndex_struct*) _temp590)->f1; goto _LL437;}
else{ goto _LL438;}} else{ goto _LL438;} _LL438: if(*(( int*) _temp376) ==  Cyc_Absyn_Gentyp_e){
_LL599: _temp598=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp376)->f1; goto
_LL597; _LL597: _temp596=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp376)->f2;
goto _LL439;} else{ goto _LL440;} _LL440: if(*(( int*) _temp376) ==  Cyc_Absyn_Deref_e){
_LL601: _temp600=(( struct Cyc_Absyn_Deref_e_struct*) _temp376)->f1; goto _LL441;}
else{ goto _LL442;} _LL442: if(*(( int*) _temp376) ==  Cyc_Absyn_StructMember_e){
_LL605: _temp604=(( struct Cyc_Absyn_StructMember_e_struct*) _temp376)->f1; goto
_LL603; _LL603: _temp602=(( struct Cyc_Absyn_StructMember_e_struct*) _temp376)->f2;
goto _LL443;} else{ goto _LL444;} _LL444: if(*(( int*) _temp376) ==  Cyc_Absyn_StructArrow_e){
_LL609: _temp608=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp376)->f1; goto
_LL607; _LL607: _temp606=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp376)->f2;
goto _LL445;} else{ goto _LL446;} _LL446: if(*(( int*) _temp376) ==  Cyc_Absyn_Subscript_e){
_LL613: _temp612=(( struct Cyc_Absyn_Subscript_e_struct*) _temp376)->f1; goto
_LL611; _LL611: _temp610=(( struct Cyc_Absyn_Subscript_e_struct*) _temp376)->f2;
goto _LL447;} else{ goto _LL448;} _LL448: if(*(( int*) _temp376) ==  Cyc_Absyn_Tuple_e){
_LL615: _temp614=(( struct Cyc_Absyn_Tuple_e_struct*) _temp376)->f1; goto _LL449;}
else{ goto _LL450;} _LL450: if(*(( int*) _temp376) ==  Cyc_Absyn_CompoundLit_e){
_LL619: _temp618=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp376)->f1; goto
_LL617; _LL617: _temp616=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp376)->f2;
goto _LL451;} else{ goto _LL452;} _LL452: if(*(( int*) _temp376) ==  Cyc_Absyn_Array_e){
_LL621: _temp620=(( struct Cyc_Absyn_Array_e_struct*) _temp376)->f1; goto _LL453;}
else{ goto _LL454;} _LL454: if(*(( int*) _temp376) ==  Cyc_Absyn_Comprehension_e){
_LL627: _temp626=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp376)->f1;
goto _LL625; _LL625: _temp624=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp376)->f2; goto _LL623; _LL623: _temp622=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp376)->f3; goto _LL455;} else{ goto _LL456;} _LL456: if(*(( int*) _temp376)
==  Cyc_Absyn_Struct_e){ _LL631: _temp630=(( struct Cyc_Absyn_Struct_e_struct*)
_temp376)->f1; goto _LL629; _LL629: _temp628=(( struct Cyc_Absyn_Struct_e_struct*)
_temp376)->f3; goto _LL457;} else{ goto _LL458;} _LL458: if(*(( int*) _temp376)
==  Cyc_Absyn_AnonStruct_e){ _LL633: _temp632=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp376)->f2; goto _LL459;} else{ goto _LL460;} _LL460: if(*(( int*) _temp376)
==  Cyc_Absyn_Tunion_e){ _LL637: _temp636=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp376)->f3; goto _LL635; _LL635: _temp634=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp376)->f5; goto _LL461;} else{ goto _LL462;} _LL462: if(*(( int*) _temp376)
==  Cyc_Absyn_Enum_e){ _LL639: _temp638=(( struct Cyc_Absyn_Enum_e_struct*)
_temp376)->f1; goto _LL463;} else{ goto _LL464;} _LL464: if(*(( int*) _temp376)
==  Cyc_Absyn_AnonEnum_e){ _LL641: _temp640=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp376)->f1; goto _LL465;} else{ goto _LL466;} _LL466: if(*(( int*) _temp376)
==  Cyc_Absyn_Malloc_e){ _LL645: _temp644=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp376)->f1; goto _LL643; _LL643: _temp642=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp376)->f2; goto _LL467;} else{ goto _LL468;} _LL468: if(*(( int*) _temp376)
==  Cyc_Absyn_UnresolvedMem_e){ _LL649: _temp648=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp376)->f1; goto _LL647; _LL647: _temp646=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp376)->f2; goto _LL469;} else{ goto _LL470;} _LL470: if(*(( int*) _temp376)
==  Cyc_Absyn_StmtExp_e){ _LL651: _temp650=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp376)->f1; goto _LL471;} else{ goto _LL472;} _LL472: if(*(( int*) _temp376)
==  Cyc_Absyn_Codegen_e){ _LL653: _temp652=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp376)->f1; goto _LL473;} else{ goto _LL474;} _LL474: if(*(( int*) _temp376)
==  Cyc_Absyn_Fill_e){ _LL655: _temp654=(( struct Cyc_Absyn_Fill_e_struct*)
_temp376)->f1; goto _LL475;} else{ goto _LL377;} _LL379: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp478)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL377; _LL381: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp657; _temp657.tag= Cyc_Std_Int_pa; _temp657.f1=(
int)(( unsigned int)(( int) _temp484));{ void* _temp656[ 1u]={& _temp657}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp656, sizeof( void*),
1u));}})); goto _LL377; _LL383: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp659; _temp659.tag= Cyc_Std_Int_pa; _temp659.f1=(
int)(( unsigned int) _temp490);{ void* _temp658[ 1u]={& _temp659}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp658, sizeof( void*),
1u));}})); goto _LL377; _LL385: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp661; _temp661.tag= Cyc_Std_Int_pa; _temp661.f1=(
int)(( unsigned int) _temp496);{ void* _temp660[ 1u]={& _temp661}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp660, sizeof( void*),
1u));}})); Cyc_Absyndump_dump_nospace( _tag_arr("u", sizeof( unsigned char), 2u));
goto _LL377; _LL387: Cyc_Absyndump_dump( _tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof( unsigned char), 27u)); goto _LL377; _LL389: Cyc_Absyndump_dump( _temp508);
goto _LL377; _LL391: Cyc_Absyndump_dump( _tag_arr("NULL", sizeof( unsigned char),
5u)); goto _LL377; _LL393: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp514)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL377; _LL395: _temp518= _temp516; goto _LL397; _LL397: Cyc_Absyndump_dumpqvar(
_temp518); goto _LL377; _LL399: { struct _tagged_arr _temp662= Cyc_Absynpp_prim2str(
_temp522); switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp520)){ case 1: _LL663: if( _temp522 == ( void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp520))->hd);
Cyc_Absyndump_dump( _tag_arr(".size", sizeof( unsigned char), 6u));} else{ Cyc_Absyndump_dump(
_temp662); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp520))->hd);} break; case 2: _LL664: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp520))->hd);
Cyc_Absyndump_dump( _temp662); Cyc_Absyndump_dump_char(( int)' '); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp520->tl))->hd);
break; default: _LL665:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp667=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp667[ 0]=({ struct Cyc_Core_Failure_struct _temp668; _temp668.tag= Cyc_Core_Failure;
_temp668.f1= _tag_arr("Absyndump -- Bad number of arguments to primop", sizeof(
unsigned char), 47u); _temp668;}); _temp667;}));} goto _LL377;} _LL401: Cyc_Absyndump_dumpexp_prec(
myprec, _temp528); if( _temp526 !=  0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*) _temp526->v));} Cyc_Absyndump_dump_nospace( _tag_arr("=", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp524); goto _LL377;
_LL403: Cyc_Absyndump_dump( _tag_arr("++", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp532); goto _LL377; _LL405: Cyc_Absyndump_dump( _tag_arr("--",
sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp536);
goto _LL377; _LL407: Cyc_Absyndump_dumpexp_prec( myprec, _temp540); Cyc_Absyndump_dump(
_tag_arr("++", sizeof( unsigned char), 3u)); goto _LL377; _LL409: Cyc_Absyndump_dumpexp_prec(
myprec, _temp544); Cyc_Absyndump_dump( _tag_arr("--", sizeof( unsigned char), 3u));
goto _LL377; _LL411: Cyc_Absyndump_dumpexp_prec( myprec, _temp550); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp548); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp546); goto _LL377; _LL413:
Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec( myprec, _temp554);
Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec( myprec, _temp552);
Cyc_Absyndump_dump_char(( int)')'); goto _LL377; _LL415: _temp562= _temp558;
_temp560= _temp556; goto _LL417; _LL417: Cyc_Absyndump_dumpexp_prec( myprec,
_temp562); Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpexps_prec( 20, _temp560); Cyc_Absyndump_dump_nospace( _tag_arr(")",
sizeof( unsigned char), 2u)); goto _LL377; _LL419: Cyc_Absyndump_dump( _tag_arr("throw",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp564);
goto _LL377; _LL421: _temp568= _temp566; goto _LL423; _LL423: Cyc_Absyndump_dumpexp_prec(
inprec, _temp568); goto _LL377; _LL425: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp572); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp570); goto _LL377; _LL427: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp574); goto _LL377; _LL429: Cyc_Absyndump_dump( _tag_arr("new ",
sizeof( unsigned char), 5u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp576);
goto _LL377; _LL431: Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumptyp( _temp580); Cyc_Absyndump_dump_char((
int)')'); goto _LL377; _LL433: Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp582); Cyc_Absyndump_dump_char((
int)')'); goto _LL377; _LL435: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp588); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump_nospace(* _temp586); Cyc_Absyndump_dump_char(( int)')');
goto _LL377; _LL437: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp594); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp670; _temp670.tag= Cyc_Std_Int_pa; _temp670.f1=( int) _temp592;{ void*
_temp669[ 1u]={& _temp670}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp669, sizeof( void*), 1u));}})); Cyc_Absyndump_dump_char((
int)')'); goto _LL377; _LL439: Cyc_Absyndump_dump( _tag_arr("__gen(", sizeof(
unsigned char), 7u)); Cyc_Absyndump_dumptvars( _temp598); Cyc_Absyndump_dumptyp(
_temp596); Cyc_Absyndump_dump_char(( int)')'); goto _LL377; _LL441: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dumpexp_prec( myprec, _temp600); goto _LL377; _LL443:
Cyc_Absyndump_dumpexp_prec( myprec, _temp604); Cyc_Absyndump_dump_char(( int)'.');
Cyc_Absyndump_dump_nospace(* _temp602); goto _LL377; _LL445: Cyc_Absyndump_dumpexp_prec(
myprec, _temp608); Cyc_Absyndump_dump_nospace( _tag_arr("->", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dump_nospace(* _temp606); goto _LL377;
_LL447: Cyc_Absyndump_dumpexp_prec( myprec, _temp612); Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp610); Cyc_Absyndump_dump_char(( int)']');
goto _LL377; _LL449: Cyc_Absyndump_dump( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_Absyndump_dumpexps_prec( 20, _temp614); Cyc_Absyndump_dump_char(( int)')');
goto _LL377; _LL451: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp618).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp616, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL377;
_LL453:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp620, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL377; _LL455: Cyc_Absyndump_dump( _tag_arr("new {for", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dump_str((* _temp626->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp624); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp622); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL377; _LL457: Cyc_Absyndump_dumpqvar( _temp630);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp628, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL377;
_LL459:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp632, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL377; _LL461: Cyc_Absyndump_dumpqvar( _temp634->name); if( _temp636
!=  0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp636, _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u));} goto _LL377; _LL463: Cyc_Absyndump_dumpqvar( _temp638); goto _LL377;
_LL465: Cyc_Absyndump_dumpqvar( _temp640); goto _LL377; _LL467: if( _temp644 != 
0){ Cyc_Absyndump_dump( _tag_arr("rmalloc(", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp644)); Cyc_Absyndump_dump( _tag_arr(",",
sizeof( unsigned char), 2u));} else{ Cyc_Absyndump_dump( _tag_arr("malloc(",
sizeof( unsigned char), 8u));} Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumptyp( _temp642); Cyc_Absyndump_dump(
_tag_arr("))", sizeof( unsigned char), 3u)); goto _LL377; _LL469:(( void(*)(
void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct _tagged_arr start,
struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp646, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL377;
_LL471: Cyc_Absyndump_dump_nospace( _tag_arr("({", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpstmt( _temp650); Cyc_Absyndump_dump_nospace( _tag_arr("})",
sizeof( unsigned char), 3u)); goto _LL377; _LL473: Cyc_Absyndump_dump( _tag_arr("codegen(",
sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp671=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp671->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp672=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp672[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp673; _temp673.tag= Cyc_Absyn_Fn_d;
_temp673.f1= _temp652; _temp673;}); _temp672;})); _temp671->loc= e->loc;
_temp671;})); Cyc_Absyndump_dump( _tag_arr(")", sizeof( unsigned char), 2u));
goto _LL377; _LL475: Cyc_Absyndump_dump( _tag_arr("fill(", sizeof( unsigned char),
6u)); Cyc_Absyndump_dumpexp( _temp654); Cyc_Absyndump_dump( _tag_arr(")",
sizeof( unsigned char), 2u)); goto _LL377; _LL377:;} if( inprec >=  myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs !=  0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause* _temp674=( struct Cyc_Absyn_Switch_clause*)
scs->hd; if( _temp674->where_clause ==  0?( void*)( _temp674->pattern)->r == (
void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump( _tag_arr("default:", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("case", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumppat( _temp674->pattern); if( _temp674->where_clause
!=  0){ Cyc_Absyndump_dump( _tag_arr("&&", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp674->where_clause));} Cyc_Absyndump_dump_nospace(
_tag_arr(":", sizeof( unsigned char), 2u));} Cyc_Absyndump_dumpstmt( _temp674->body);}}
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt* s){ void* _temp675=( void*)
s->r; struct Cyc_Absyn_Exp* _temp721; struct Cyc_Absyn_Stmt* _temp723; struct
Cyc_Absyn_Stmt* _temp725; struct Cyc_Absyn_Exp* _temp727; struct Cyc_Absyn_Exp*
_temp729; struct Cyc_Absyn_Stmt* _temp731; struct Cyc_Absyn_Stmt* _temp733;
struct Cyc_Absyn_Exp* _temp735; struct Cyc_Absyn_Stmt* _temp737; struct _tuple2
_temp739; struct Cyc_Absyn_Exp* _temp741; struct _tagged_arr* _temp743; struct
Cyc_Absyn_Stmt* _temp745; struct _tuple2 _temp747; struct Cyc_Absyn_Exp*
_temp749; struct _tuple2 _temp751; struct Cyc_Absyn_Exp* _temp753; struct Cyc_Absyn_Exp*
_temp755; struct Cyc_List_List* _temp757; struct Cyc_Absyn_Exp* _temp759; struct
Cyc_Absyn_Stmt* _temp761; struct Cyc_Absyn_Decl* _temp763; struct Cyc_Absyn_Stmt*
_temp765; struct _tagged_arr* _temp767; struct _tuple2 _temp769; struct Cyc_Absyn_Exp*
_temp771; struct Cyc_Absyn_Stmt* _temp773; struct Cyc_List_List* _temp775;
struct Cyc_Absyn_Exp* _temp777; struct Cyc_List_List* _temp779; struct Cyc_List_List*
_temp781; struct Cyc_List_List* _temp783; struct Cyc_Absyn_Stmt* _temp785;
struct Cyc_Absyn_Stmt* _temp787; struct Cyc_Absyn_Vardecl* _temp789; struct Cyc_Absyn_Tvar*
_temp791; struct Cyc_Absyn_Stmt* _temp793; struct Cyc_Absyn_Stmt* _temp795;
_LL677: if( _temp675 == ( void*) Cyc_Absyn_Skip_s){ goto _LL678;} else{ goto
_LL679;} _LL679: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Exp_s:
0){ _LL722: _temp721=(( struct Cyc_Absyn_Exp_s_struct*) _temp675)->f1; goto
_LL680;} else{ goto _LL681;} _LL681: if(( unsigned int) _temp675 >  1u?*(( int*)
_temp675) ==  Cyc_Absyn_Seq_s: 0){ _LL726: _temp725=(( struct Cyc_Absyn_Seq_s_struct*)
_temp675)->f1; goto _LL724; _LL724: _temp723=(( struct Cyc_Absyn_Seq_s_struct*)
_temp675)->f2; goto _LL682;} else{ goto _LL683;} _LL683: if(( unsigned int)
_temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Return_s: 0){ _LL728: _temp727=((
struct Cyc_Absyn_Return_s_struct*) _temp675)->f1; if( _temp727 ==  0){ goto
_LL684;} else{ goto _LL685;}} else{ goto _LL685;} _LL685: if(( unsigned int)
_temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Return_s: 0){ _LL730: _temp729=((
struct Cyc_Absyn_Return_s_struct*) _temp675)->f1; goto _LL686;} else{ goto
_LL687;} _LL687: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_IfThenElse_s:
0){ _LL736: _temp735=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp675)->f1;
goto _LL734; _LL734: _temp733=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp675)->f2;
goto _LL732; _LL732: _temp731=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp675)->f3;
goto _LL688;} else{ goto _LL689;} _LL689: if(( unsigned int) _temp675 >  1u?*((
int*) _temp675) ==  Cyc_Absyn_While_s: 0){ _LL740: _temp739=(( struct Cyc_Absyn_While_s_struct*)
_temp675)->f1; _LL742: _temp741= _temp739.f1; goto _LL738; _LL738: _temp737=((
struct Cyc_Absyn_While_s_struct*) _temp675)->f2; goto _LL690;} else{ goto _LL691;}
_LL691: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Break_s:
0){ goto _LL692;} else{ goto _LL693;} _LL693: if(( unsigned int) _temp675 >  1u?*((
int*) _temp675) ==  Cyc_Absyn_Continue_s: 0){ goto _LL694;} else{ goto _LL695;}
_LL695: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Goto_s:
0){ _LL744: _temp743=(( struct Cyc_Absyn_Goto_s_struct*) _temp675)->f1; goto
_LL696;} else{ goto _LL697;} _LL697: if(( unsigned int) _temp675 >  1u?*(( int*)
_temp675) ==  Cyc_Absyn_For_s: 0){ _LL756: _temp755=(( struct Cyc_Absyn_For_s_struct*)
_temp675)->f1; goto _LL752; _LL752: _temp751=(( struct Cyc_Absyn_For_s_struct*)
_temp675)->f2; _LL754: _temp753= _temp751.f1; goto _LL748; _LL748: _temp747=((
struct Cyc_Absyn_For_s_struct*) _temp675)->f3; _LL750: _temp749= _temp747.f1;
goto _LL746; _LL746: _temp745=(( struct Cyc_Absyn_For_s_struct*) _temp675)->f4;
goto _LL698;} else{ goto _LL699;} _LL699: if(( unsigned int) _temp675 >  1u?*((
int*) _temp675) ==  Cyc_Absyn_Switch_s: 0){ _LL760: _temp759=(( struct Cyc_Absyn_Switch_s_struct*)
_temp675)->f1; goto _LL758; _LL758: _temp757=(( struct Cyc_Absyn_Switch_s_struct*)
_temp675)->f2; goto _LL700;} else{ goto _LL701;} _LL701: if(( unsigned int)
_temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Decl_s: 0){ _LL764: _temp763=((
struct Cyc_Absyn_Decl_s_struct*) _temp675)->f1; goto _LL762; _LL762: _temp761=((
struct Cyc_Absyn_Decl_s_struct*) _temp675)->f2; goto _LL702;} else{ goto _LL703;}
_LL703: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Label_s:
0){ _LL768: _temp767=(( struct Cyc_Absyn_Label_s_struct*) _temp675)->f1; goto
_LL766; _LL766: _temp765=(( struct Cyc_Absyn_Label_s_struct*) _temp675)->f2;
goto _LL704;} else{ goto _LL705;} _LL705: if(( unsigned int) _temp675 >  1u?*((
int*) _temp675) ==  Cyc_Absyn_Do_s: 0){ _LL774: _temp773=(( struct Cyc_Absyn_Do_s_struct*)
_temp675)->f1; goto _LL770; _LL770: _temp769=(( struct Cyc_Absyn_Do_s_struct*)
_temp675)->f2; _LL772: _temp771= _temp769.f1; goto _LL706;} else{ goto _LL707;}
_LL707: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_SwitchC_s:
0){ _LL778: _temp777=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp675)->f1; goto
_LL776; _LL776: _temp775=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp675)->f2;
goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int) _temp675 >  1u?*((
int*) _temp675) ==  Cyc_Absyn_Fallthru_s: 0){ _LL780: _temp779=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp675)->f1; if( _temp779 ==  0){ goto _LL710;} else{ goto _LL711;}} else{
goto _LL711;} _LL711: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) == 
Cyc_Absyn_Fallthru_s: 0){ _LL782: _temp781=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp675)->f1; goto _LL712;} else{ goto _LL713;} _LL713: if(( unsigned int)
_temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_TryCatch_s: 0){ _LL786:
_temp785=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp675)->f1; goto _LL784;
_LL784: _temp783=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp675)->f2; goto
_LL714;} else{ goto _LL715;} _LL715: if(( unsigned int) _temp675 >  1u?*(( int*)
_temp675) ==  Cyc_Absyn_Region_s: 0){ _LL792: _temp791=(( struct Cyc_Absyn_Region_s_struct*)
_temp675)->f1; goto _LL790; _LL790: _temp789=(( struct Cyc_Absyn_Region_s_struct*)
_temp675)->f2; goto _LL788; _LL788: _temp787=(( struct Cyc_Absyn_Region_s_struct*)
_temp675)->f3; goto _LL716;} else{ goto _LL717;} _LL717: if(( unsigned int)
_temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Cut_s: 0){ _LL794: _temp793=((
struct Cyc_Absyn_Cut_s_struct*) _temp675)->f1; goto _LL718;} else{ goto _LL719;}
_LL719: if(( unsigned int) _temp675 >  1u?*(( int*) _temp675) ==  Cyc_Absyn_Splice_s:
0){ _LL796: _temp795=(( struct Cyc_Absyn_Splice_s_struct*) _temp675)->f1; goto
_LL720;} else{ goto _LL676;} _LL678: Cyc_Absyndump_dump_semi(); goto _LL676;
_LL680: Cyc_Absyndump_dumpexp( _temp721); Cyc_Absyndump_dump_semi(); goto _LL676;
_LL682: if( Cyc_Absynpp_is_declaration( _temp725)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp725); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp725);} if( Cyc_Absynpp_is_declaration(
_temp723)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp723);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp723);}
goto _LL676; _LL684: Cyc_Absyndump_dump( _tag_arr("return;", sizeof(
unsigned char), 8u)); goto _LL676; _LL686: Cyc_Absyndump_dump( _tag_arr("return",
sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp729)); Cyc_Absyndump_dump_semi(); goto _LL676; _LL688: Cyc_Absyndump_dump(
_tag_arr("if(", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp( _temp735);
Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt(
_temp733); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp797=( void*) _temp731->r;
_LL799: if( _temp797 == ( void*) Cyc_Absyn_Skip_s){ goto _LL800;} else{ goto
_LL801;} _LL801: goto _LL802; _LL800: goto _LL798; _LL802: Cyc_Absyndump_dump(
_tag_arr("else{", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpstmt( _temp731);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL798; _LL798:;} goto _LL676; _LL690:
Cyc_Absyndump_dump( _tag_arr("while(", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(
_temp741); Cyc_Absyndump_dump_nospace( _tag_arr(") {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp737); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL676; _LL692: Cyc_Absyndump_dump( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL676; _LL694: Cyc_Absyndump_dump( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL676; _LL696: Cyc_Absyndump_dump( _tag_arr("goto",
sizeof( unsigned char), 5u)); Cyc_Absyndump_dump_str( _temp743); Cyc_Absyndump_dump_semi();
goto _LL676; _LL698: Cyc_Absyndump_dump( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_Absyndump_dumpexp( _temp755); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp753); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp749); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp745);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL676; _LL700: Cyc_Absyndump_dump(
_tag_arr("switch(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpexp(
_temp759); Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpswitchclauses( _temp757); Cyc_Absyndump_dump_char(( int)'}');
goto _LL676; _LL702: Cyc_Absyndump_dumpdecl( _temp763); Cyc_Absyndump_dumpstmt(
_temp761); goto _LL676; _LL704: if( Cyc_Absynpp_is_declaration( _temp765)){ Cyc_Absyndump_dump_str(
_temp767); Cyc_Absyndump_dump_nospace( _tag_arr(": {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp765); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dump_str( _temp767); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt(
_temp765);} goto _LL676; _LL706: Cyc_Absyndump_dump( _tag_arr("do {", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumpstmt( _temp773); Cyc_Absyndump_dump_nospace(
_tag_arr("} while (", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexp(
_temp771); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char), 3u));
goto _LL676; _LL708: Cyc_Absyndump_dump( _tag_arr("switch \"C\" (", sizeof(
unsigned char), 13u)); Cyc_Absyndump_dumpexp( _temp777); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); for( 0; _temp775 !=  0; _temp775=
_temp775->tl){ struct Cyc_Absyn_SwitchC_clause _temp805; struct Cyc_Absyn_Stmt*
_temp806; struct Cyc_Absyn_Exp* _temp808; struct Cyc_Absyn_SwitchC_clause*
_temp803=( struct Cyc_Absyn_SwitchC_clause*) _temp775->hd; _temp805=* _temp803;
_LL809: _temp808= _temp805.cnst_exp; goto _LL807; _LL807: _temp806= _temp805.body;
goto _LL804; _LL804: if( _temp808 ==  0){ Cyc_Absyndump_dump( _tag_arr("default: ",
sizeof( unsigned char), 10u));} else{ Cyc_Absyndump_dump( _tag_arr("case ",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp808)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp806);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL676; _LL710: Cyc_Absyndump_dump(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL676; _LL712: Cyc_Absyndump_dump(
_tag_arr("fallthru(", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexps_prec(
20, _temp781); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char),
3u)); goto _LL676; _LL714: Cyc_Absyndump_dump( _tag_arr("try", sizeof(
unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp785); Cyc_Absyndump_dump(
_tag_arr("catch {", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpswitchclauses(
_temp783); Cyc_Absyndump_dump_char(( int)'}'); goto _LL676; _LL716: Cyc_Absyndump_dump(
_tag_arr("region<", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptvar(
_temp791); Cyc_Absyndump_dump( _tag_arr("> ", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpqvar(
_temp789->name); Cyc_Absyndump_dump( _tag_arr("{", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpstmt( _temp787); Cyc_Absyndump_dump( _tag_arr("}", sizeof(
unsigned char), 2u)); goto _LL676; _LL718: Cyc_Absyndump_dump( _tag_arr("cut",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp793); goto _LL676;
_LL720: Cyc_Absyndump_dump( _tag_arr("splice", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpstmt(
_temp795); goto _LL676; _LL676:;} struct _tuple9{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple9* dp){
Cyc_Absyndump_egroup( Cyc_Absyndump_dumpdesignator,(* dp).f1, _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=",
sizeof( unsigned char), 2u)); Cyc_Absyndump_dumppat((* dp).f2);} void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat* p){ void* _temp810=( void*) p->r; int _temp846; void*
_temp848; int _temp850; void* _temp852; unsigned char _temp854; struct
_tagged_arr _temp856; struct Cyc_Absyn_Vardecl* _temp858; struct Cyc_List_List*
_temp860; struct Cyc_Absyn_Pat* _temp862; struct Cyc_Absyn_Vardecl* _temp864;
struct _tuple0* _temp866; struct Cyc_List_List* _temp868; struct Cyc_List_List*
_temp870; struct _tuple0* _temp872; struct Cyc_List_List* _temp874; struct Cyc_List_List*
_temp876; struct _tuple0* _temp878; struct Cyc_List_List* _temp880; struct Cyc_List_List*
_temp882; struct Cyc_Absyn_Structdecl* _temp884; struct Cyc_List_List* _temp886;
struct Cyc_List_List* _temp888; struct Cyc_Absyn_Tunionfield* _temp890; struct
Cyc_Absyn_Enumfield* _temp892; struct Cyc_Absyn_Enumfield* _temp894; _LL812: if(
_temp810 == ( void*) Cyc_Absyn_Wild_p){ goto _LL813;} else{ goto _LL814;} _LL814:
if( _temp810 == ( void*) Cyc_Absyn_Null_p){ goto _LL815;} else{ goto _LL816;}
_LL816: if(( unsigned int) _temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_Int_p:
0){ _LL849: _temp848=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp810)->f1;
if( _temp848 == ( void*) Cyc_Absyn_Signed){ goto _LL847;} else{ goto _LL818;}
_LL847: _temp846=(( struct Cyc_Absyn_Int_p_struct*) _temp810)->f2; goto _LL817;}
else{ goto _LL818;} _LL818: if(( unsigned int) _temp810 >  2u?*(( int*) _temp810)
==  Cyc_Absyn_Int_p: 0){ _LL853: _temp852=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp810)->f1; if( _temp852 == ( void*) Cyc_Absyn_Unsigned){ goto _LL851;} else{
goto _LL820;} _LL851: _temp850=(( struct Cyc_Absyn_Int_p_struct*) _temp810)->f2;
goto _LL819;} else{ goto _LL820;} _LL820: if(( unsigned int) _temp810 >  2u?*((
int*) _temp810) ==  Cyc_Absyn_Char_p: 0){ _LL855: _temp854=(( struct Cyc_Absyn_Char_p_struct*)
_temp810)->f1; goto _LL821;} else{ goto _LL822;} _LL822: if(( unsigned int)
_temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_Float_p: 0){ _LL857: _temp856=((
struct Cyc_Absyn_Float_p_struct*) _temp810)->f1; goto _LL823;} else{ goto _LL824;}
_LL824: if(( unsigned int) _temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_Var_p:
0){ _LL859: _temp858=(( struct Cyc_Absyn_Var_p_struct*) _temp810)->f1; goto
_LL825;} else{ goto _LL826;} _LL826: if(( unsigned int) _temp810 >  2u?*(( int*)
_temp810) ==  Cyc_Absyn_Tuple_p: 0){ _LL861: _temp860=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp810)->f1; goto _LL827;} else{ goto _LL828;} _LL828: if(( unsigned int)
_temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_Pointer_p: 0){ _LL863: _temp862=((
struct Cyc_Absyn_Pointer_p_struct*) _temp810)->f1; goto _LL829;} else{ goto
_LL830;} _LL830: if(( unsigned int) _temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_Reference_p:
0){ _LL865: _temp864=(( struct Cyc_Absyn_Reference_p_struct*) _temp810)->f1;
goto _LL831;} else{ goto _LL832;} _LL832: if(( unsigned int) _temp810 >  2u?*((
int*) _temp810) ==  Cyc_Absyn_UnknownId_p: 0){ _LL867: _temp866=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp810)->f1; goto _LL833;} else{ goto _LL834;} _LL834: if(( unsigned int)
_temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL873:
_temp872=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp810)->f1; goto _LL871;
_LL871: _temp870=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp810)->f2; goto
_LL869; _LL869: _temp868=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp810)->f3;
goto _LL835;} else{ goto _LL836;} _LL836: if(( unsigned int) _temp810 >  2u?*((
int*) _temp810) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL879: _temp878=(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp810)->f1; goto _LL877; _LL877: _temp876=((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp810)->f2; goto _LL875; _LL875:
_temp874=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp810)->f3; goto _LL837;}
else{ goto _LL838;} _LL838: if(( unsigned int) _temp810 >  2u?*(( int*) _temp810)
==  Cyc_Absyn_Struct_p: 0){ _LL885: _temp884=(( struct Cyc_Absyn_Struct_p_struct*)
_temp810)->f1; goto _LL883; _LL883: _temp882=(( struct Cyc_Absyn_Struct_p_struct*)
_temp810)->f3; goto _LL881; _LL881: _temp880=(( struct Cyc_Absyn_Struct_p_struct*)
_temp810)->f4; goto _LL839;} else{ goto _LL840;} _LL840: if(( unsigned int)
_temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_Tunion_p: 0){ _LL891: _temp890=((
struct Cyc_Absyn_Tunion_p_struct*) _temp810)->f2; goto _LL889; _LL889: _temp888=((
struct Cyc_Absyn_Tunion_p_struct*) _temp810)->f3; goto _LL887; _LL887: _temp886=((
struct Cyc_Absyn_Tunion_p_struct*) _temp810)->f4; goto _LL841;} else{ goto
_LL842;} _LL842: if(( unsigned int) _temp810 >  2u?*(( int*) _temp810) ==  Cyc_Absyn_Enum_p:
0){ _LL893: _temp892=(( struct Cyc_Absyn_Enum_p_struct*) _temp810)->f2; goto
_LL843;} else{ goto _LL844;} _LL844: if(( unsigned int) _temp810 >  2u?*(( int*)
_temp810) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL895: _temp894=(( struct Cyc_Absyn_AnonEnum_p_struct*)
_temp810)->f2; goto _LL845;} else{ goto _LL811;} _LL813: Cyc_Absyndump_dump_char((
int)'_'); goto _LL811; _LL815: Cyc_Absyndump_dump( _tag_arr("NULL", sizeof(
unsigned char), 5u)); goto _LL811; _LL817: Cyc_Absyndump_dump(( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp897; _temp897.tag= Cyc_Std_Int_pa;
_temp897.f1=( int)(( unsigned int) _temp846);{ void* _temp896[ 1u]={& _temp897};
Cyc_Std_aprintf( _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp896,
sizeof( void*), 1u));}})); goto _LL811; _LL819: Cyc_Absyndump_dump(( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp899; _temp899.tag= Cyc_Std_Int_pa;
_temp899.f1=( unsigned int) _temp850;{ void* _temp898[ 1u]={& _temp899}; Cyc_Std_aprintf(
_tag_arr("%u", sizeof( unsigned char), 3u), _tag_arr( _temp898, sizeof( void*),
1u));}})); goto _LL811; _LL821: Cyc_Absyndump_dump( _tag_arr("'", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp854)); Cyc_Absyndump_dump_nospace( _tag_arr("'", sizeof( unsigned char), 2u));
goto _LL811; _LL823: Cyc_Absyndump_dump( _temp856); goto _LL811; _LL825: Cyc_Absyndump_dumpqvar(
_temp858->name); goto _LL811; _LL827:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp860,
_tag_arr("$(", sizeof( unsigned char), 3u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL811; _LL829: Cyc_Absyndump_dump(
_tag_arr("&", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumppat( _temp862);
goto _LL811; _LL831: Cyc_Absyndump_dump( _tag_arr("*", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpqvar( _temp864->name); goto _LL811; _LL833: Cyc_Absyndump_dumpqvar(
_temp866); goto _LL811; _LL835: Cyc_Absyndump_dumpqvar( _temp872); Cyc_Absyndump_dumptvars(
_temp870);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp868, _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL811; _LL837: Cyc_Absyndump_dumpqvar( _temp878); Cyc_Absyndump_dumptvars(
_temp876);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp874, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL811; _LL839: if( _temp884->name !=  0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp884->name))->v);} Cyc_Absyndump_dumptvars(
_temp882);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp880, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL811; _LL841: Cyc_Absyndump_dumpqvar( _temp890->name); Cyc_Absyndump_dumptvars(
_temp888); if( _temp886 !=  0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp886,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u));} goto _LL811; _LL843: Cyc_Absyndump_dumpqvar(
_temp892->name); goto _LL811; _LL845: Cyc_Absyndump_dumpqvar( _temp894->name);
goto _LL811; _LL811:;} void Cyc_Absyndump_dumptunionfield( struct Cyc_Absyn_Tunionfield*
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
Cyc_Absyn_Structfield _temp902; struct Cyc_List_List* _temp903; struct Cyc_Absyn_Exp*
_temp905; void* _temp907; struct Cyc_Absyn_Tqual _temp909; struct _tagged_arr*
_temp911; struct Cyc_Absyn_Structfield* _temp900=( struct Cyc_Absyn_Structfield*)
fields->hd; _temp902=* _temp900; _LL912: _temp911= _temp902.name; goto _LL910;
_LL910: _temp909= _temp902.tq; goto _LL908; _LL908: _temp907=( void*) _temp902.type;
goto _LL906; _LL906: _temp905= _temp902.width; goto _LL904; _LL904: _temp903=
_temp902.attributes; goto _LL901; _LL901:(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tagged_arr*), struct _tagged_arr*)) Cyc_Absyndump_dumptqtd)(
_temp909, _temp907, Cyc_Absyndump_dump_str, _temp911); Cyc_Absyndump_dumpatts(
_temp903); if( _temp905 !=  0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp905));} Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar(
td->name); Cyc_Absyndump_dumptvars( td->tvs);} static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp913=(* pr).f1; _LL915: if( _temp913 == ( void*)
Cyc_Absyn_Unused_att){ goto _LL916;} else{ goto _LL917;} _LL917: if( _temp913 == (
void*) Cyc_Absyn_Stdcall_att){ goto _LL918;} else{ goto _LL919;} _LL919: if(
_temp913 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL920;} else{ goto _LL921;}
_LL921: if( _temp913 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL922;} else{
goto _LL923;} _LL923: goto _LL924; _LL916: goto _LL914; _LL918: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); goto _LL914; _LL920: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); goto _LL914; _LL922: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); goto _LL914; _LL924: goto
_LL914; _LL914:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for(
0; vds !=  0; vds= vds->tl){ Cyc_Absyndump_dumpqvar((( struct Cyc_Absyn_Vardecl*)
vds->hd)->name); if( vds->tl !=  0){ Cyc_Absyndump_dump_char(( int)',');}}} void
Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void* _temp925=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp951; struct Cyc_Absyn_Structdecl* _temp953; struct
Cyc_Absyn_Uniondecl* _temp955; struct Cyc_Absyn_Vardecl* _temp957; struct Cyc_Absyn_Vardecl
_temp959; struct Cyc_List_List* _temp960; struct Cyc_Absyn_Exp* _temp962; void*
_temp964; struct Cyc_Absyn_Tqual _temp966; struct _tuple0* _temp968; void*
_temp970; struct Cyc_Absyn_Tuniondecl* _temp972; struct Cyc_Absyn_Tuniondecl
_temp974; int _temp975; struct Cyc_Core_Opt* _temp977; struct Cyc_List_List*
_temp979; struct _tuple0* _temp981; void* _temp983; struct Cyc_Absyn_Enumdecl*
_temp985; struct Cyc_Absyn_Enumdecl _temp987; struct Cyc_Core_Opt* _temp988;
struct _tuple0* _temp990; void* _temp992; struct Cyc_Absyn_Exp* _temp994; struct
Cyc_Absyn_Pat* _temp996; struct Cyc_List_List* _temp998; struct Cyc_Absyn_Typedefdecl*
_temp1000; struct Cyc_List_List* _temp1002; struct _tagged_arr* _temp1004;
struct Cyc_List_List* _temp1006; struct _tuple0* _temp1008; struct Cyc_List_List*
_temp1010; _LL927: if(*(( int*) _temp925) ==  Cyc_Absyn_Fn_d){ _LL952: _temp951=((
struct Cyc_Absyn_Fn_d_struct*) _temp925)->f1; goto _LL928;} else{ goto _LL929;}
_LL929: if(*(( int*) _temp925) ==  Cyc_Absyn_Struct_d){ _LL954: _temp953=((
struct Cyc_Absyn_Struct_d_struct*) _temp925)->f1; goto _LL930;} else{ goto
_LL931;} _LL931: if(*(( int*) _temp925) ==  Cyc_Absyn_Union_d){ _LL956: _temp955=((
struct Cyc_Absyn_Union_d_struct*) _temp925)->f1; goto _LL932;} else{ goto _LL933;}
_LL933: if(*(( int*) _temp925) ==  Cyc_Absyn_Var_d){ _LL958: _temp957=(( struct
Cyc_Absyn_Var_d_struct*) _temp925)->f1; _temp959=* _temp957; _LL971: _temp970=(
void*) _temp959.sc; goto _LL969; _LL969: _temp968= _temp959.name; goto _LL967;
_LL967: _temp966= _temp959.tq; goto _LL965; _LL965: _temp964=( void*) _temp959.type;
goto _LL963; _LL963: _temp962= _temp959.initializer; goto _LL961; _LL961:
_temp960= _temp959.attributes; goto _LL934;} else{ goto _LL935;} _LL935: if(*((
int*) _temp925) ==  Cyc_Absyn_Tunion_d){ _LL973: _temp972=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp925)->f1; _temp974=* _temp972; _LL984: _temp983=( void*) _temp974.sc; goto
_LL982; _LL982: _temp981= _temp974.name; goto _LL980; _LL980: _temp979= _temp974.tvs;
goto _LL978; _LL978: _temp977= _temp974.fields; goto _LL976; _LL976: _temp975=
_temp974.is_xtunion; goto _LL936;} else{ goto _LL937;} _LL937: if(*(( int*)
_temp925) ==  Cyc_Absyn_Enum_d){ _LL986: _temp985=(( struct Cyc_Absyn_Enum_d_struct*)
_temp925)->f1; _temp987=* _temp985; _LL993: _temp992=( void*) _temp987.sc; goto
_LL991; _LL991: _temp990= _temp987.name; goto _LL989; _LL989: _temp988= _temp987.fields;
goto _LL938;} else{ goto _LL939;} _LL939: if(*(( int*) _temp925) ==  Cyc_Absyn_Let_d){
_LL997: _temp996=(( struct Cyc_Absyn_Let_d_struct*) _temp925)->f1; goto _LL995;
_LL995: _temp994=(( struct Cyc_Absyn_Let_d_struct*) _temp925)->f4; goto _LL940;}
else{ goto _LL941;} _LL941: if(*(( int*) _temp925) ==  Cyc_Absyn_Letv_d){ _LL999:
_temp998=(( struct Cyc_Absyn_Letv_d_struct*) _temp925)->f1; goto _LL942;} else{
goto _LL943;} _LL943: if(*(( int*) _temp925) ==  Cyc_Absyn_Typedef_d){ _LL1001:
_temp1000=(( struct Cyc_Absyn_Typedef_d_struct*) _temp925)->f1; goto _LL944;}
else{ goto _LL945;} _LL945: if(*(( int*) _temp925) ==  Cyc_Absyn_Namespace_d){
_LL1005: _temp1004=(( struct Cyc_Absyn_Namespace_d_struct*) _temp925)->f1; goto
_LL1003; _LL1003: _temp1002=(( struct Cyc_Absyn_Namespace_d_struct*) _temp925)->f2;
goto _LL946;} else{ goto _LL947;} _LL947: if(*(( int*) _temp925) ==  Cyc_Absyn_Using_d){
_LL1009: _temp1008=(( struct Cyc_Absyn_Using_d_struct*) _temp925)->f1; goto
_LL1007; _LL1007: _temp1006=(( struct Cyc_Absyn_Using_d_struct*) _temp925)->f2;
goto _LL948;} else{ goto _LL949;} _LL949: if(*(( int*) _temp925) ==  Cyc_Absyn_ExternC_d){
_LL1011: _temp1010=(( struct Cyc_Absyn_ExternC_d_struct*) _temp925)->f1; goto
_LL950;} else{ goto _LL926;} _LL928: if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts(
_temp951->attributes);} if( _temp951->is_inline){ if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump(
_tag_arr("__inline", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("inline", sizeof( unsigned char), 7u));}} Cyc_Absyndump_dumpscope((
void*) _temp951->sc);{ void* t=( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1013=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1013[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1014; _temp1014.tag= Cyc_Absyn_FnType;
_temp1014.f1=({ struct Cyc_Absyn_FnInfo _temp1015; _temp1015.tvars= _temp951->tvs;
_temp1015.effect= _temp951->effect; _temp1015.ret_typ=( void*)(( void*) _temp951->ret_type);
_temp1015.args=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp951->args);
_temp1015.c_varargs= _temp951->c_varargs; _temp1015.cyc_varargs= _temp951->cyc_varargs;
_temp1015.rgn_po= _temp951->rgn_po; _temp1015.attributes= 0; _temp1015;});
_temp1014;}); _temp1013;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1012; _temp1012.q_const= 0; _temp1012.q_volatile= 0;
_temp1012.q_restrict= 0; _temp1012;}), t, Cyc_Absyndump_to_VC? Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar, _temp951); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp951->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL926;} _LL930: Cyc_Absyndump_dumpscope((
void*) _temp953->sc); Cyc_Absyndump_dump( _tag_arr("struct", sizeof(
unsigned char), 7u)); if( _temp953->name !=  0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp953->name))->v);} Cyc_Absyndump_dumptvars(
_temp953->tvs); if( _temp953->fields ==  0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp953->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp953->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL926;
_LL932: Cyc_Absyndump_dumpscope(( void*) _temp955->sc); Cyc_Absyndump_dump(
_tag_arr("union", sizeof( unsigned char), 6u)); if( _temp955->name !=  0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp955->name))->v);} Cyc_Absyndump_dumptvars(
_temp955->tvs); if( _temp955->fields ==  0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp955->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp955->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL926;
_LL934: if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts( _temp960); Cyc_Absyndump_dumpscope(
_temp970);{ struct Cyc_List_List* _temp1018; void* _temp1020; struct Cyc_Absyn_Tqual
_temp1022; struct _tuple4 _temp1016= Cyc_Absynpp_to_tms( _temp966, _temp964);
_LL1023: _temp1022= _temp1016.f1; goto _LL1021; _LL1021: _temp1020= _temp1016.f2;
goto _LL1019; _LL1019: _temp1018= _temp1016.f3; goto _LL1017; _LL1017: { void*
call_conv=( void*) Cyc_Absyn_Unused_att;{ struct Cyc_List_List* tms2= _temp1018;
for( 0; tms2 !=  0; tms2= tms2->tl){ void* _temp1024=( void*) tms2->hd; struct
Cyc_List_List* _temp1030; _LL1026: if(( unsigned int) _temp1024 >  1u?*(( int*)
_temp1024) ==  Cyc_Absyn_Attributes_mod: 0){ _LL1031: _temp1030=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1024)->f2; goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029;
_LL1027: for( 0; _temp1030 !=  0; _temp1030= _temp1030->tl){ void* _temp1032=(
void*) _temp1030->hd; _LL1034: if( _temp1032 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL1035;} else{ goto _LL1036;} _LL1036: if( _temp1032 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1037;} else{ goto _LL1038;} _LL1038: if( _temp1032 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1039;} else{ goto _LL1040;} _LL1040: goto _LL1041; _LL1035: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1033; _LL1037: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1033; _LL1039: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1033;
_LL1041: goto _LL1033; _LL1033:;} goto _LL1025; _LL1029: goto _LL1025; _LL1025:;}}
Cyc_Absyndump_dumptq( _temp1022); Cyc_Absyndump_dumpntyp( _temp1020);{ struct
_tuple10 _temp1042=({ struct _tuple10 _temp1043; _temp1043.f1= call_conv;
_temp1043.f2= _temp968; _temp1043;});(( void(*)( struct Cyc_List_List* tms, void(*
f)( struct _tuple10*), struct _tuple10* a)) Cyc_Absyndump_dumptms)( Cyc_List_imp_rev(
_temp1018), Cyc_Absyndump_dump_callconv_qvar,& _temp1042);}}}} else{ Cyc_Absyndump_dumpscope(
_temp970);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp966, _temp964, Cyc_Absyndump_dumpqvar,
_temp968); Cyc_Absyndump_dumpatts( _temp960);} if( _temp962 !=  0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp962));}
Cyc_Absyndump_dump_semi(); goto _LL926; _LL936: Cyc_Absyndump_dumpscope(
_temp983); if( _temp975){ Cyc_Absyndump_dump( _tag_arr("xtunion ", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("tunion ", sizeof(
unsigned char), 8u));} Cyc_Absyndump_dumpqvar( _temp981); Cyc_Absyndump_dumptvars(
_temp979); if( _temp977 ==  0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*) _temp977->v);
Cyc_Absyndump_dump_nospace( _tag_arr("};", sizeof( unsigned char), 3u));} goto
_LL926; _LL938: Cyc_Absyndump_dumpscope( _temp992); Cyc_Absyndump_dump( _tag_arr("enum ",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar( _temp990); if( _temp988 == 
0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*) _temp988->v); Cyc_Absyndump_dump_nospace( _tag_arr("};",
sizeof( unsigned char), 3u));} return; _LL940: Cyc_Absyndump_dump( _tag_arr("let",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumppat( _temp996); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp994); Cyc_Absyndump_dump_semi(); goto
_LL926; _LL942: Cyc_Absyndump_dump( _tag_arr("let ", sizeof( unsigned char), 5u));
Cyc_Absyndump_dumpids( _temp998); Cyc_Absyndump_dump_semi(); goto _LL926; _LL944:
if( ! Cyc_Absyndump_expand_typedefs){ Cyc_Absyndump_dump( _tag_arr("typedef",
sizeof( unsigned char), 8u));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1044; _temp1044.q_const= 0; _temp1044.q_volatile= 0;
_temp1044.q_restrict= 0; _temp1044;}),( void*) _temp1000->defn, Cyc_Absyndump_dumptypedefname,
_temp1000); Cyc_Absyndump_dump_semi();} goto _LL926; _LL946: Cyc_Absyndump_dump(
_tag_arr("namespace", sizeof( unsigned char), 10u)); Cyc_Absyndump_dump_str(
_temp1004); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1002 !=  0;
_temp1002= _temp1002->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1002->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL926; _LL948: Cyc_Absyndump_dump(
_tag_arr("using", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp1008); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1006 !=  0;
_temp1006= _temp1006->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1006->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL926; _LL950: Cyc_Absyndump_dump(
_tag_arr("extern \"C\" {", sizeof( unsigned char), 13u)); for( 0; _temp1010 != 
0; _temp1010= _temp1010->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1010->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL926; _LL926:;}
static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i !=  1){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char(( int)'}');}} void
Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void* a){
if( tms ==  0){ f( a); return;}{ void* _temp1045=( void*) tms->hd; struct Cyc_Absyn_Tqual
_temp1063; void* _temp1065; void* _temp1067; struct Cyc_Absyn_Exp* _temp1069;
struct Cyc_Absyn_Tqual _temp1071; void* _temp1073; void* _temp1075; struct Cyc_Absyn_Exp*
_temp1077; struct Cyc_Absyn_Tqual _temp1079; void* _temp1081; void* _temp1083;
struct Cyc_Absyn_Tqual _temp1085; void* _temp1087; struct Cyc_Absyn_Tvar*
_temp1089; void* _temp1091; struct Cyc_Absyn_Exp* _temp1093; struct Cyc_Absyn_Tqual
_temp1095; void* _temp1097; struct Cyc_Absyn_Tvar* _temp1099; void* _temp1101;
struct Cyc_Absyn_Exp* _temp1103; struct Cyc_Absyn_Tqual _temp1105; void*
_temp1107; struct Cyc_Absyn_Tvar* _temp1109; void* _temp1111; _LL1047: if((
unsigned int) _temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Pointer_mod: 0){
_LL1068: _temp1067=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1045)->f1;
if(( unsigned int) _temp1067 >  1u?*(( int*) _temp1067) ==  Cyc_Absyn_Nullable_ps:
0){ _LL1070: _temp1069=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1067)->f1;
goto _LL1066;} else{ goto _LL1049;} _LL1066: _temp1065=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f2; if( _temp1065 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1064;}
else{ goto _LL1049;} _LL1064: _temp1063=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f3; goto _LL1048;} else{ goto _LL1049;} _LL1049: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1076:
_temp1075=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1045)->f1; if((
unsigned int) _temp1075 >  1u?*(( int*) _temp1075) ==  Cyc_Absyn_NonNullable_ps:
0){ _LL1078: _temp1077=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1075)->f1;
goto _LL1074;} else{ goto _LL1051;} _LL1074: _temp1073=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f2; if( _temp1073 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1072;}
else{ goto _LL1051;} _LL1072: _temp1071=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f3; goto _LL1050;} else{ goto _LL1051;} _LL1051: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1084:
_temp1083=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1045)->f1; if(
_temp1083 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1082;} else{ goto
_LL1053;} _LL1082: _temp1081=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f2; if( _temp1081 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1080;}
else{ goto _LL1053;} _LL1080: _temp1079=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f3; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1092:
_temp1091=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1045)->f1; if((
unsigned int) _temp1091 >  1u?*(( int*) _temp1091) ==  Cyc_Absyn_Nullable_ps: 0){
_LL1094: _temp1093=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1091)->f1; goto
_LL1088;} else{ goto _LL1055;} _LL1088: _temp1087=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f2; if(( unsigned int) _temp1087 >  3u?*(( int*) _temp1087) ==  Cyc_Absyn_VarType:
0){ _LL1090: _temp1089=(( struct Cyc_Absyn_VarType_struct*) _temp1087)->f1; goto
_LL1086;} else{ goto _LL1055;} _LL1086: _temp1085=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f3; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1102:
_temp1101=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1045)->f1; if((
unsigned int) _temp1101 >  1u?*(( int*) _temp1101) ==  Cyc_Absyn_NonNullable_ps:
0){ _LL1104: _temp1103=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1101)->f1;
goto _LL1098;} else{ goto _LL1057;} _LL1098: _temp1097=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f2; if(( unsigned int) _temp1097 >  3u?*(( int*) _temp1097) ==  Cyc_Absyn_VarType:
0){ _LL1100: _temp1099=(( struct Cyc_Absyn_VarType_struct*) _temp1097)->f1; goto
_LL1096;} else{ goto _LL1057;} _LL1096: _temp1095=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f3; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1112:
_temp1111=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1045)->f1; if(
_temp1111 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1108;} else{ goto
_LL1059;} _LL1108: _temp1107=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f2; if(( unsigned int) _temp1107 >  3u?*(( int*) _temp1107) ==  Cyc_Absyn_VarType:
0){ _LL1110: _temp1109=(( struct Cyc_Absyn_VarType_struct*) _temp1107)->f1; goto
_LL1106;} else{ goto _LL1059;} _LL1106: _temp1105=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1045)->f3; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(( unsigned int)
_temp1045 >  1u?*(( int*) _temp1045) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL1060;}
else{ goto _LL1061;} _LL1061: goto _LL1062; _LL1048: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1069); Cyc_Absyndump_dumptms( tms->tl,
f, a); return; _LL1050: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1077); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1052: Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1054: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1093); Cyc_Absyndump_dump_str(
_temp1089->name); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1056: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1103); Cyc_Absyndump_dump_str(
_temp1099->name); Cyc_Absyndump_dumptms( tms->tl, f, a); return; _LL1058: Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dump_str( _temp1109->name); Cyc_Absyndump_dumptms( tms->tl,
f, a); return; _LL1060:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1113=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1113[ 0]=({ struct Cyc_Core_Impossible_struct _temp1114; _temp1114.tag= Cyc_Core_Impossible;
_temp1114.f1= _tag_arr("dumptms: bad Pointer_mod", sizeof( unsigned char), 25u);
_temp1114;}); _temp1113;})); _LL1062: { int next_is_pointer= 0; if( tms->tl != 
0){ void* _temp1115=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd;
_LL1117: if(( unsigned int) _temp1115 >  1u?*(( int*) _temp1115) ==  Cyc_Absyn_Pointer_mod:
0){ goto _LL1118;} else{ goto _LL1119;} _LL1119: goto _LL1120; _LL1118:
next_is_pointer= 1; goto _LL1116; _LL1120: goto _LL1116; _LL1116:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');} Cyc_Absyndump_dumptms(
tms->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char(( int)')');}{ void*
_temp1121=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp1137; void* _temp1139;
struct Cyc_List_List* _temp1141; struct Cyc_Core_Opt* _temp1143; struct Cyc_Absyn_VarargInfo*
_temp1145; int _temp1147; struct Cyc_List_List* _temp1149; void* _temp1151;
struct Cyc_Position_Segment* _temp1153; struct Cyc_List_List* _temp1155; int
_temp1157; struct Cyc_Position_Segment* _temp1159; struct Cyc_List_List*
_temp1161; struct Cyc_List_List* _temp1163; void* _temp1165; void* _temp1167;
_LL1123: if( _temp1121 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL1124;} else{
goto _LL1125;} _LL1125: if(( unsigned int) _temp1121 >  1u?*(( int*) _temp1121)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL1138: _temp1137=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1121)->f1; goto _LL1126;} else{ goto _LL1127;} _LL1127: if(( unsigned int)
_temp1121 >  1u?*(( int*) _temp1121) ==  Cyc_Absyn_Function_mod: 0){ _LL1140:
_temp1139=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1121)->f1; if(*((
int*) _temp1139) ==  Cyc_Absyn_WithTypes){ _LL1150: _temp1149=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1139)->f1; goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1139)->f2; goto _LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1139)->f3; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1139)->f4; goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1139)->f5; goto _LL1128;} else{ goto _LL1129;}} else{ goto _LL1129;}
_LL1129: if(( unsigned int) _temp1121 >  1u?*(( int*) _temp1121) ==  Cyc_Absyn_Function_mod:
0){ _LL1152: _temp1151=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1121)->f1; if(*(( int*) _temp1151) ==  Cyc_Absyn_NoTypes){ _LL1156:
_temp1155=(( struct Cyc_Absyn_NoTypes_struct*) _temp1151)->f1; goto _LL1154;
_LL1154: _temp1153=(( struct Cyc_Absyn_NoTypes_struct*) _temp1151)->f2; goto
_LL1130;} else{ goto _LL1131;}} else{ goto _LL1131;} _LL1131: if(( unsigned int)
_temp1121 >  1u?*(( int*) _temp1121) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL1162:
_temp1161=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1121)->f1; goto
_LL1160; _LL1160: _temp1159=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1121)->f2; goto _LL1158; _LL1158: _temp1157=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1121)->f3; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(( unsigned int)
_temp1121 >  1u?*(( int*) _temp1121) ==  Cyc_Absyn_Attributes_mod: 0){ _LL1164:
_temp1163=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1121)->f2; goto
_LL1134;} else{ goto _LL1135;} _LL1135: if(( unsigned int) _temp1121 >  1u?*((
int*) _temp1121) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1168: _temp1167=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1121)->f1; goto _LL1166; _LL1166:
_temp1165=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1121)->f2; goto
_LL1136;} else{ goto _LL1122;} _LL1124: Cyc_Absyndump_dump( _tag_arr("[]",
sizeof( unsigned char), 3u)); goto _LL1122; _LL1126: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1137); Cyc_Absyndump_dump_char(( int)']');
goto _LL1122; _LL1128: Cyc_Absyndump_dumpfunargs( _temp1149, _temp1147,
_temp1145, _temp1143, _temp1141); goto _LL1122; _LL1130:(( void(*)( void(* f)(
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1155, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL1122;
_LL1132: if( _temp1157){ Cyc_Absyndump_dumpkindedtvars( _temp1161);} else{ Cyc_Absyndump_dumptvars(
_temp1161);} goto _LL1122; _LL1134: Cyc_Absyndump_dumpatts( _temp1163); goto
_LL1122; _LL1136:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1169=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1169[ 0]=({ struct Cyc_Core_Impossible_struct _temp1170; _temp1170.tag= Cyc_Core_Impossible;
_temp1170.f1= _tag_arr("dumptms", sizeof( unsigned char), 8u); _temp1170;});
_temp1169;})); _LL1122:;} return;} _LL1046:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1173; void* _temp1175; struct Cyc_Absyn_Tqual _temp1177; struct _tuple4
_temp1171= Cyc_Absynpp_to_tms( tq, t); _LL1178: _temp1177= _temp1171.f1; goto
_LL1176; _LL1176: _temp1175= _temp1171.f2; goto _LL1174; _LL1174: _temp1173=
_temp1171.f3; goto _LL1172; _LL1172: Cyc_Absyndump_dumptq( _temp1177); Cyc_Absyndump_dumpntyp(
_temp1175); Cyc_Absyndump_dumptms( Cyc_List_imp_rev( _temp1173), f, a);} void
Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct Cyc_Std___sFILE*
f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0; tdl !=  0; tdl=
tdl->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*) tdl->hd);}({ void*
_temp1179[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof( unsigned char), 2u),
_tag_arr( _temp1179, sizeof( void*), 0u));});}
