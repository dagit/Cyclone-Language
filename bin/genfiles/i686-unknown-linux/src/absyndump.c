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

#ifdef CYC_REGION_PROFILE
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
#define _cycalloc(n) _profile_cycalloc(n,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_cycalloc_atomic(n,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
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
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
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
const int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType= 6;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual f2;
struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_StructType= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
f3; } ; static const int Cyc_Absyn_Malloc_e= 32; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
33; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 34; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 35; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 36; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
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
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
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
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_SwitchC_clause{
struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment*
loc; } ; static const int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b= 1; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b= 2;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b= 3; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b= 4; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
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
Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
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
Cyc_List_List* fields); struct Cyc_Std___sFILE** Cyc_Absyndump_dump_file=& Cyc_Std_stdout;
void Cyc_Absyndump_ignore( void* x){ return;} static unsigned int Cyc_Absyndump_pos=
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
return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl !=  0; l=((
struct Cyc_List_List*) _check_null( l))->tl){ f(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_arr sep){ if(
l ==  0){ return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl !=  0;
l=(( struct Cyc_List_List*) _check_null( l))->tl){ f( env,( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_group( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep){ Cyc_Absyndump_dump_nospace( start);
Cyc_Absyndump_dump_sep( f, l, sep); Cyc_Absyndump_dump_nospace( end);} void Cyc_Absyndump_group_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep){ Cyc_Absyndump_dump_nospace(
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
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp0))->hd); for( _temp0=((
struct Cyc_List_List*) _check_null( _temp0))->tl; _temp0 !=  0; _temp0=(( struct
Cyc_List_List*) _check_null( _temp0))->tl){ if( Cyc_Absyndump_qvar_to_Cids){ Cyc_Absyndump_dump_char((
int)'_');} else{ Cyc_Absyndump_dump_nospace( _tag_arr("::", sizeof(
unsigned char), 3u));} Cyc_Absyndump_dump_nospace(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( _temp0))->hd));} if( Cyc_Absyndump_qvar_to_Cids){
Cyc_Absyndump_dump_nospace( _tag_arr("_", sizeof( unsigned char), 2u));} else{
Cyc_Absyndump_dump_nospace( _tag_arr("::", sizeof( unsigned char), 3u));} Cyc_Absyndump_dump_nospace(*(*
v).f2);} else{ Cyc_Absyndump_dump_str((* v).f2);}} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump( _tag_arr("restrict",
sizeof( unsigned char), 9u));} if( tq.q_volatile){ Cyc_Absyndump_dump( _tag_arr("volatile",
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
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp50=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL52: if( _temp50 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL53;} else{ goto _LL54;} _LL54: if( _temp50 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL55;} else{ goto _LL56;} _LL56: if( _temp50 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL53: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); return; _LL55: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); return; _LL57: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); return; _LL59: goto _LL51;
_LL51:;}} void Cyc_Absyndump_dump_noncallconv( struct Cyc_List_List* atts){ int
hasatt= 0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 !=  0; atts2=((
struct Cyc_List_List*) _check_null( atts2))->tl){ void* _temp60=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL62: if( _temp60 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL63;} else{ goto _LL64;} _LL64: if( _temp60 == (
void*) Cyc_Absyn_Cdecl_att){ goto _LL65;} else{ goto _LL66;} _LL66: if( _temp60
== ( void*) Cyc_Absyn_Fastcall_att){ goto _LL67;} else{ goto _LL68;} _LL68: goto
_LL69; _LL63: goto _LL61; _LL65: goto _LL61; _LL67: goto _LL61; _LL69: hasatt= 1;
goto _LL61; _LL61:;}} if( ! hasatt){ return;} Cyc_Absyndump_dump( _tag_arr("__declspec(",
sizeof( unsigned char), 12u)); for( 0; atts !=  0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp70=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL72: if( _temp70 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL73;} else{ goto _LL74;} _LL74: if( _temp70 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL75;} else{ goto _LL76;} _LL76: if( _temp70 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL77;} else{ goto _LL78;} _LL78: goto _LL79; _LL73: goto _LL71; _LL75:
goto _LL71; _LL77: goto _LL71; _LL79: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); goto _LL71; _LL71:;}
Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumpatts( struct Cyc_List_List*
atts){ if( atts ==  0){ return;} if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump_noncallconv(
atts); return;} Cyc_Absyndump_dump( _tag_arr(" __attribute__((", sizeof(
unsigned char), 17u)); for( 0; atts !=  0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)((
struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl !=  0){ Cyc_Absyndump_dump( _tag_arr(",", sizeof(
unsigned char), 2u));}} Cyc_Absyndump_dump( _tag_arr(")) ", sizeof(
unsigned char), 4u));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms ==  0){ return 0;}{ void* _temp80=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; _LL82: if(( unsigned int) _temp80 >  1u?*(( int*)
_temp80) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL83;} else{ goto _LL84;} _LL84:
goto _LL85; _LL83: return 1; _LL85: return 0; _LL81:;}} static void Cyc_Absyndump_dumprgn(
void* t){ void* _temp86= Cyc_Tcutil_compress( t); _LL88: if( _temp86 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL89;} else{ goto _LL90;} _LL90: goto _LL91; _LL89:
Cyc_Absyndump_dump( _tag_arr("`H", sizeof( unsigned char), 3u)); goto _LL87;
_LL91: Cyc_Absyndump_dumpntyp( t); goto _LL87; _LL87:;} struct _tuple6{ struct
Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct _tuple6 Cyc_Absyndump_effects_split(
void* t){ struct Cyc_List_List* rgions= 0; struct Cyc_List_List* effects= 0;{
void* _temp92= Cyc_Tcutil_compress( t); void* _temp100; struct Cyc_List_List*
_temp102; _LL94: if(( unsigned int) _temp92 >  4u?*(( int*) _temp92) ==  Cyc_Absyn_AccessEff:
0){ _LL101: _temp100=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp92)->f1;
goto _LL95;} else{ goto _LL96;} _LL96: if(( unsigned int) _temp92 >  4u?*(( int*)
_temp92) ==  Cyc_Absyn_JoinEff: 0){ _LL103: _temp102=(( struct Cyc_Absyn_JoinEff_struct*)
_temp92)->f1; goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL95: rgions=({
struct Cyc_List_List* _temp104=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp104->hd=( void*) _temp100; _temp104->tl= rgions;
_temp104;}); goto _LL93; _LL97: for( 0; _temp102 !=  0; _temp102=(( struct Cyc_List_List*)
_check_null( _temp102))->tl){ struct Cyc_List_List* _temp107; struct Cyc_List_List*
_temp109; struct _tuple6 _temp105= Cyc_Absyndump_effects_split(( void*)(( struct
Cyc_List_List*) _check_null( _temp102))->hd); _LL110: _temp109= _temp105.f1;
goto _LL108; _LL108: _temp107= _temp105.f2; goto _LL106; _LL106: rgions= Cyc_List_imp_append(
_temp109, rgions); effects= Cyc_List_imp_append( _temp107, effects);} goto _LL93;
_LL99: effects=({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp111->hd=( void*) t; _temp111->tl=
effects; _temp111;}); goto _LL93; _LL93:;} return({ struct _tuple6 _temp112;
_temp112.f1= rgions; _temp112.f2= effects; _temp112;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp115; struct Cyc_List_List* _temp117; struct
_tuple6 _temp113= Cyc_Absyndump_effects_split( t); _LL118: _temp117= _temp113.f1;
goto _LL116; _LL116: _temp115= _temp113.f2; goto _LL114; _LL114: _temp117= Cyc_List_imp_rev(
_temp117); _temp115= Cyc_List_imp_rev( _temp115); for( 0; _temp115 !=  0;
_temp115=(( struct Cyc_List_List*) _check_null( _temp115))->tl){ Cyc_Absyndump_dumpntyp((
void*)(( struct Cyc_List_List*) _check_null( _temp115))->hd); Cyc_Absyndump_dump_char((
int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp117 !=  0; _temp117=((
struct Cyc_List_List*) _check_null( _temp117))->tl){ Cyc_Absyndump_dumprgn((
void*)(( struct Cyc_List_List*) _check_null( _temp117))->hd); if((( struct Cyc_List_List*)
_check_null( _temp117))->tl !=  0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char((
int)'}');} void Cyc_Absyndump_dumpntyp( void* t){ void* _temp119= t; struct Cyc_Absyn_Tvar*
_temp191; int _temp193; struct Cyc_Core_Opt* _temp195; struct Cyc_Core_Opt*
_temp197; int _temp199; struct Cyc_Core_Opt* _temp201; struct Cyc_Core_Opt
_temp203; void* _temp204; struct Cyc_Core_Opt* _temp206; struct Cyc_Absyn_TunionInfo
_temp208; void* _temp210; struct Cyc_List_List* _temp212; void* _temp214; struct
Cyc_Absyn_TunionFieldInfo _temp216; struct Cyc_List_List* _temp218; void*
_temp220; struct _tuple0* _temp222; void* _temp224; void* _temp226; void*
_temp228; void* _temp230; void* _temp232; void* _temp234; void* _temp236; void*
_temp238; void* _temp240; void* _temp242; void* _temp244; void* _temp246; void*
_temp249; void* _temp251; void* _temp253; void* _temp255; void* _temp258; void*
_temp260; void* _temp262; void* _temp264; struct Cyc_List_List* _temp266; struct
Cyc_List_List* _temp268; struct _tuple0* _temp270; struct Cyc_List_List*
_temp272; struct _tuple0* _temp274; struct Cyc_List_List* _temp276; struct
_tuple0* _temp278; struct Cyc_List_List* _temp280; struct _tuple0* _temp282;
struct Cyc_List_List* _temp284; struct Cyc_List_List* _temp286; struct Cyc_Core_Opt*
_temp288; struct Cyc_List_List* _temp290; struct _tuple0* _temp292; void*
_temp294; _LL121: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_ArrayType:
0){ goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int) _temp119 >  4u?*((
int*) _temp119) ==  Cyc_Absyn_FnType: 0){ goto _LL124;} else{ goto _LL125;}
_LL125: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_PointerType:
0){ goto _LL126;} else{ goto _LL127;} _LL127: if( _temp119 == ( void*) Cyc_Absyn_VoidType){
goto _LL128;} else{ goto _LL129;} _LL129: if(( unsigned int) _temp119 >  4u?*((
int*) _temp119) ==  Cyc_Absyn_VarType: 0){ _LL192: _temp191=(( struct Cyc_Absyn_VarType_struct*)
_temp119)->f1; goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int)
_temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_Evar: 0){ _LL198: _temp197=((
struct Cyc_Absyn_Evar_struct*) _temp119)->f1; goto _LL196; _LL196: _temp195=((
struct Cyc_Absyn_Evar_struct*) _temp119)->f2; if( _temp195 ==  0){ goto _LL194;}
else{ goto _LL133;} _LL194: _temp193=(( struct Cyc_Absyn_Evar_struct*) _temp119)->f3;
goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int) _temp119 >  4u?*((
int*) _temp119) ==  Cyc_Absyn_Evar: 0){ _LL207: _temp206=(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f1; goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f2; if( _temp201 ==  0){ goto _LL135;} else{ _temp203=* _temp201;
_LL205: _temp204=( void*) _temp203.v; goto _LL200;} _LL200: _temp199=(( struct
Cyc_Absyn_Evar_struct*) _temp119)->f3; goto _LL134;} else{ goto _LL135;} _LL135:
if(( unsigned int) _temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_TunionType:
0){ _LL209: _temp208=(( struct Cyc_Absyn_TunionType_struct*) _temp119)->f1;
_LL215: _temp214=( void*) _temp208.tunion_info; goto _LL213; _LL213: _temp212=
_temp208.targs; goto _LL211; _LL211: _temp210=( void*) _temp208.rgn; goto _LL136;}
else{ goto _LL137;} _LL137: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119)
==  Cyc_Absyn_TunionFieldType: 0){ _LL217: _temp216=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp119)->f1; _LL221: _temp220=( void*) _temp216.field_info; goto _LL219;
_LL219: _temp218= _temp216.targs; goto _LL138;} else{ goto _LL139;} _LL139: if((
unsigned int) _temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_EnumType: 0){
_LL223: _temp222=(( struct Cyc_Absyn_EnumType_struct*) _temp119)->f1; goto
_LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL227: _temp226=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp226 == ( void*) Cyc_Absyn_Signed){ goto _LL225;} else{
goto _LL143;} _LL225: _temp224=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp224 == ( void*) Cyc_Absyn_B4){ goto _LL142;} else{ goto
_LL143;}} else{ goto _LL143;} _LL143: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL231: _temp230=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp230 == ( void*) Cyc_Absyn_Signed){ goto _LL229;} else{
goto _LL145;} _LL229: _temp228=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp228 == ( void*) Cyc_Absyn_B1){ goto _LL144;} else{ goto
_LL145;}} else{ goto _LL145;} _LL145: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL235: _temp234=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp234 == ( void*) Cyc_Absyn_Unsigned){ goto _LL233;} else{
goto _LL147;} _LL233: _temp232=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp232 == ( void*) Cyc_Absyn_B1){ goto _LL146;} else{ goto
_LL147;}} else{ goto _LL147;} _LL147: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL239: _temp238=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp238 == ( void*) Cyc_Absyn_Signed){ goto _LL237;} else{
goto _LL149;} _LL237: _temp236=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp236 == ( void*) Cyc_Absyn_B2){ goto _LL148;} else{ goto
_LL149;}} else{ goto _LL149;} _LL149: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL243: _temp242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp242 == ( void*) Cyc_Absyn_Unsigned){ goto _LL241;} else{
goto _LL151;} _LL241: _temp240=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp240 == ( void*) Cyc_Absyn_B2){ goto _LL150;} else{ goto
_LL151;}} else{ goto _LL151;} _LL151: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL247: _temp246=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp246 == ( void*) Cyc_Absyn_Unsigned){ goto _LL245;} else{
goto _LL153;} _LL245: _temp244=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp244 == ( void*) Cyc_Absyn_B4){ goto _LL152;} else{ goto
_LL153;}} else{ goto _LL153;} _LL153: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL252: _temp251=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp251 == ( void*) Cyc_Absyn_Signed){ goto _LL250;} else{
goto _LL155;} _LL250: _temp249=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp249 == ( void*) Cyc_Absyn_B8){ goto _LL248;} else{ goto
_LL155;}} else{ goto _LL155;} _LL248: if( Cyc_Absyndump_to_VC){ goto _LL154;}
else{ goto _LL155;} _LL155: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119)
==  Cyc_Absyn_IntType: 0){ _LL256: _temp255=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp255 == ( void*) Cyc_Absyn_Signed){ goto _LL254;} else{
goto _LL157;} _LL254: _temp253=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp253 == ( void*) Cyc_Absyn_B8){ goto _LL156;} else{ goto
_LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_IntType: 0){ _LL261: _temp260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp260 == ( void*) Cyc_Absyn_Unsigned){ goto _LL259;} else{
goto _LL159;} _LL259: _temp258=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp258 == ( void*) Cyc_Absyn_B8){ goto _LL257;} else{ goto
_LL159;}} else{ goto _LL159;} _LL257: if( Cyc_Absyndump_to_VC){ goto _LL158;}
else{ goto _LL159;} _LL159: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119)
==  Cyc_Absyn_IntType: 0){ _LL265: _temp264=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; if( _temp264 == ( void*) Cyc_Absyn_Unsigned){ goto _LL263;} else{
goto _LL161;} _LL263: _temp262=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp262 == ( void*) Cyc_Absyn_B8){ goto _LL160;} else{ goto
_LL161;}} else{ goto _LL161;} _LL161: if( _temp119 == ( void*) Cyc_Absyn_FloatType){
goto _LL162;} else{ goto _LL163;} _LL163: if( _temp119 == ( void*) Cyc_Absyn_DoubleType){
goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int) _temp119 >  4u?*((
int*) _temp119) ==  Cyc_Absyn_TupleType: 0){ _LL267: _temp266=(( struct Cyc_Absyn_TupleType_struct*)
_temp119)->f1; goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int)
_temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_StructType: 0){ _LL271:
_temp270=(( struct Cyc_Absyn_StructType_struct*) _temp119)->f1; if( _temp270 == 
0){ goto _LL269;} else{ goto _LL169;} _LL269: _temp268=(( struct Cyc_Absyn_StructType_struct*)
_temp119)->f2; goto _LL168;} else{ goto _LL169;} _LL169: if(( unsigned int)
_temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_StructType: 0){ _LL275:
_temp274=(( struct Cyc_Absyn_StructType_struct*) _temp119)->f1; goto _LL273;
_LL273: _temp272=(( struct Cyc_Absyn_StructType_struct*) _temp119)->f2; goto
_LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_UnionType: 0){ _LL279: _temp278=(( struct Cyc_Absyn_UnionType_struct*)
_temp119)->f1; if( _temp278 ==  0){ goto _LL277;} else{ goto _LL173;} _LL277:
_temp276=(( struct Cyc_Absyn_UnionType_struct*) _temp119)->f2; goto _LL172;}
else{ goto _LL173;} _LL173: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119)
==  Cyc_Absyn_UnionType: 0){ _LL283: _temp282=(( struct Cyc_Absyn_UnionType_struct*)
_temp119)->f1; goto _LL281; _LL281: _temp280=(( struct Cyc_Absyn_UnionType_struct*)
_temp119)->f2; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_AnonStructType: 0){ _LL285:
_temp284=(( struct Cyc_Absyn_AnonStructType_struct*) _temp119)->f1; goto _LL176;}
else{ goto _LL177;} _LL177: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119)
==  Cyc_Absyn_AnonUnionType: 0){ _LL287: _temp286=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp119)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_TypedefType: 0){ _LL293:
_temp292=(( struct Cyc_Absyn_TypedefType_struct*) _temp119)->f1; goto _LL291;
_LL291: _temp290=(( struct Cyc_Absyn_TypedefType_struct*) _temp119)->f2; goto
_LL289; _LL289: _temp288=(( struct Cyc_Absyn_TypedefType_struct*) _temp119)->f3;
goto _LL180;} else{ goto _LL181;} _LL181: if(( unsigned int) _temp119 >  4u?*((
int*) _temp119) ==  Cyc_Absyn_RgnHandleType: 0){ _LL295: _temp294=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp119)->f1; goto _LL182;} else{ goto
_LL183;} _LL183: if( _temp119 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL184;}
else{ goto _LL185;} _LL185: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119)
==  Cyc_Absyn_AccessEff: 0){ goto _LL186;} else{ goto _LL187;} _LL187: if((
unsigned int) _temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL188;} else{ goto _LL189;} _LL189: if(( unsigned int) _temp119 >  4u?*(( int*)
_temp119) ==  Cyc_Absyn_JoinEff: 0){ goto _LL190;} else{ goto _LL120;} _LL122:
return; _LL124: return; _LL126: return; _LL128: Cyc_Absyndump_dump( _tag_arr("void",
sizeof( unsigned char), 5u)); return; _LL130: Cyc_Absyndump_dump_str( _temp191->name);
return; _LL132: Cyc_Absyndump_dump( _tag_arr("%", sizeof( unsigned char), 2u));
if( _temp197 ==  0){ Cyc_Absyndump_dump( _tag_arr("?", sizeof( unsigned char), 2u));}
else{ Cyc_Absyndump_dumpkind(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp197))->v);} Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp297; _temp297.tag= Cyc_Std_Int_pa; _temp297.f1=( int)(( unsigned int)
_temp193);{ void* _temp296[ 1u]={& _temp297}; Cyc_Std_aprintf( _tag_arr("(%d)",
sizeof( unsigned char), 5u), _tag_arr( _temp296, sizeof( void*), 1u));}}));
return; _LL134: Cyc_Absyndump_dumpntyp( _temp204); return; _LL136:{ void*
_temp298= _temp214; struct Cyc_Absyn_UnknownTunionInfo _temp304; int _temp306;
struct _tuple0* _temp308; struct Cyc_Absyn_Tuniondecl** _temp310; struct Cyc_Absyn_Tuniondecl*
_temp312; struct Cyc_Absyn_Tuniondecl _temp313; int _temp314; struct _tuple0*
_temp316; _LL300: if(*(( int*) _temp298) ==  Cyc_Absyn_UnknownTunion){ _LL305:
_temp304=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp298)->f1; _LL309:
_temp308= _temp304.name; goto _LL307; _LL307: _temp306= _temp304.is_xtunion;
goto _LL301;} else{ goto _LL302;} _LL302: if(*(( int*) _temp298) ==  Cyc_Absyn_KnownTunion){
_LL311: _temp310=(( struct Cyc_Absyn_KnownTunion_struct*) _temp298)->f1;
_temp312=* _temp310; _temp313=* _temp312; _LL317: _temp316= _temp313.name; goto
_LL315; _LL315: _temp314= _temp313.is_xtunion; goto _LL303;} else{ goto _LL299;}
_LL301: _temp316= _temp308; _temp314= _temp306; goto _LL303; _LL303: if(
_temp314){ Cyc_Absyndump_dump( _tag_arr("xtunion ", sizeof( unsigned char), 9u));}
else{ Cyc_Absyndump_dump( _tag_arr("tunion ", sizeof( unsigned char), 8u));}{
void* _temp318= Cyc_Tcutil_compress( _temp210); _LL320: if( _temp318 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL321;} else{ goto _LL322;} _LL322: goto _LL323;
_LL321: goto _LL319; _LL323: Cyc_Absyndump_dumptyp( _temp210); Cyc_Absyndump_dump(
_tag_arr(" ", sizeof( unsigned char), 2u)); goto _LL319; _LL319:;} Cyc_Absyndump_dumpqvar(
_temp316); Cyc_Absyndump_dumptps( _temp212); goto _LL299; _LL299:;} goto _LL120;
_LL138:{ void* _temp324= _temp220; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp330; int _temp332; struct _tuple0* _temp334; struct _tuple0* _temp336;
struct Cyc_Absyn_Tunionfield* _temp338; struct Cyc_Absyn_Tunionfield _temp340;
struct _tuple0* _temp341; struct Cyc_Absyn_Tuniondecl* _temp343; struct Cyc_Absyn_Tuniondecl
_temp345; int _temp346; struct _tuple0* _temp348; _LL326: if(*(( int*) _temp324)
==  Cyc_Absyn_UnknownTunionfield){ _LL331: _temp330=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp324)->f1; _LL337: _temp336= _temp330.tunion_name; goto _LL335; _LL335:
_temp334= _temp330.field_name; goto _LL333; _LL333: _temp332= _temp330.is_xtunion;
goto _LL327;} else{ goto _LL328;} _LL328: if(*(( int*) _temp324) ==  Cyc_Absyn_KnownTunionfield){
_LL344: _temp343=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp324)->f1;
_temp345=* _temp343; _LL349: _temp348= _temp345.name; goto _LL347; _LL347:
_temp346= _temp345.is_xtunion; goto _LL339; _LL339: _temp338=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp324)->f2; _temp340=* _temp338; _LL342: _temp341= _temp340.name; goto _LL329;}
else{ goto _LL325;} _LL327: _temp348= _temp336; _temp346= _temp332; _temp341=
_temp334; goto _LL329; _LL329: if( _temp346){ Cyc_Absyndump_dump( _tag_arr("xtunion ",
sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("tunion ",
sizeof( unsigned char), 8u));} Cyc_Absyndump_dumpqvar( _temp348); Cyc_Absyndump_dump(
_tag_arr(".", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar( _temp341);
Cyc_Absyndump_dumptps( _temp218); goto _LL325; _LL325:;} goto _LL120; _LL140:
Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp222); return; _LL142: Cyc_Absyndump_dump( _tag_arr("int", sizeof(
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
sizeof( unsigned char), 6u)); return; _LL164: Cyc_Absyndump_dump( _tag_arr("double",
sizeof( unsigned char), 7u)); return; _LL166: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp266); return; _LL168: Cyc_Absyndump_dump( _tag_arr("struct",
sizeof( unsigned char), 7u)); Cyc_Absyndump_dumptps( _temp268); return; _LL170:
Cyc_Absyndump_dump( _tag_arr("struct", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp274)); Cyc_Absyndump_dumptps( _temp272);
return; _LL172: Cyc_Absyndump_dump( _tag_arr("union", sizeof( unsigned char), 6u));
Cyc_Absyndump_dumptps( _temp276); return; _LL174: Cyc_Absyndump_dump( _tag_arr("union",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(( struct _tuple0*)
_check_null( _temp282)); Cyc_Absyndump_dumptps( _temp280); return; _LL176: Cyc_Absyndump_dump(
_tag_arr("struct {", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpstructfields(
_temp284); Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u));
return; _LL178: Cyc_Absyndump_dump( _tag_arr("union {", sizeof( unsigned char),
8u)); Cyc_Absyndump_dumpstructfields( _temp286); Cyc_Absyndump_dump( _tag_arr("}",
sizeof( unsigned char), 2u)); return; _LL180:( Cyc_Absyndump_dumpqvar( _temp292),
Cyc_Absyndump_dumptps( _temp290)); return; _LL182: Cyc_Absyndump_dumprgn(
_temp294); return; _LL184: return; _LL186: return; _LL188: return; _LL190:
return; _LL120:;} void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if(
vo !=  0){ Cyc_Absyndump_dump_str(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null( vo))->v);}} void Cyc_Absyndump_dumpfunarg( struct _tuple1* t){((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Core_Opt*), struct
Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(*
t).f1);} struct _tuple7{ void* f1; void* f2; } ; void Cyc_Absyndump_dump_rgncmp(
struct _tuple7* cmp){ struct _tuple7 _temp352; void* _temp353; void* _temp355;
struct _tuple7* _temp350= cmp; _temp352=* _temp350; _LL356: _temp355= _temp352.f1;
goto _LL354; _LL354: _temp353= _temp352.f2; goto _LL351; _LL351: Cyc_Absyndump_dumptyp(
_temp355); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumptyp( _temp353);}
void Cyc_Absyndump_dump_rgnpo( struct Cyc_List_List* rgn_po){(( void(*)( void(*
f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp, rgn_po, _tag_arr(",", sizeof( unsigned char), 2u));}
void Cyc_Absyndump_dumpfunargs( struct Cyc_List_List* args, int c_varargs,
struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt* effopt, struct
Cyc_List_List* rgn_po){ Cyc_Absyndump_dump_char(( int)'('); for( 0; args !=  0;
args=(( struct Cyc_List_List*) _check_null( args))->tl){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)(( struct Cyc_List_List*) _check_null( args))->hd); if((((
struct Cyc_List_List*) _check_null( args))->tl !=  0? 1: c_varargs)? 1:
cyc_varargs !=  0){ Cyc_Absyndump_dump_char(( int)',');}} if( c_varargs){ Cyc_Absyndump_dump(
_tag_arr("...", sizeof( unsigned char), 4u));} else{ if( cyc_varargs !=  0){
struct _tuple1* _temp357=({ struct _tuple1* _temp358=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp358->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp358->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp358->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp358;}); Cyc_Absyndump_dump( _tag_arr("...",
sizeof( unsigned char), 4u)); if((( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->inject){ Cyc_Absyndump_dump( _tag_arr(" inject ", sizeof(
unsigned char), 9u));} Cyc_Absyndump_dumpfunarg( _temp357);}} if( effopt !=  0){
Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)(( struct Cyc_Core_Opt*)
_check_null( effopt))->v);} if( rgn_po !=  0){ Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dump_rgnpo( rgn_po);} Cyc_Absyndump_dump_char(( int)')');} void
Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(*
f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp359;
_temp359.q_const= 0; _temp359.q_volatile= 0; _temp359.q_restrict= 0; _temp359;}),
t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp360= d; struct Cyc_Absyn_Exp* _temp366; struct _tagged_arr*
_temp368; _LL362: if(*(( int*) _temp360) ==  Cyc_Absyn_ArrayElement){ _LL367:
_temp366=(( struct Cyc_Absyn_ArrayElement_struct*) _temp360)->f1; goto _LL363;}
else{ goto _LL364;} _LL364: if(*(( int*) _temp360) ==  Cyc_Absyn_FieldName){
_LL369: _temp368=(( struct Cyc_Absyn_FieldName_struct*) _temp360)->f1; goto
_LL365;} else{ goto _LL361;} _LL363: Cyc_Absyndump_dump( _tag_arr(".[", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp( _temp366); Cyc_Absyndump_dump_char((
int)']'); goto _LL361; _LL365: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp368); goto _LL361; _LL361:;} struct _tuple8{ struct Cyc_List_List* f1;
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
unsigned char), 2u));}{ void* _temp370=( void*) e->r; void* _temp468;
unsigned char _temp470; void* _temp472; void* _temp474; short _temp476; void*
_temp478; void* _temp480; int _temp482; void* _temp484; void* _temp486; int
_temp488; void* _temp490; void* _temp492; long long _temp494; void* _temp496;
void* _temp498; struct _tagged_arr _temp500; void* _temp502; void* _temp504;
struct _tagged_arr _temp506; struct _tuple0* _temp508; struct _tuple0* _temp510;
struct Cyc_List_List* _temp512; void* _temp514; struct Cyc_Absyn_Exp* _temp516;
struct Cyc_Core_Opt* _temp518; struct Cyc_Absyn_Exp* _temp520; void* _temp522;
struct Cyc_Absyn_Exp* _temp524; void* _temp526; struct Cyc_Absyn_Exp* _temp528;
void* _temp530; struct Cyc_Absyn_Exp* _temp532; void* _temp534; struct Cyc_Absyn_Exp*
_temp536; struct Cyc_Absyn_Exp* _temp538; struct Cyc_Absyn_Exp* _temp540; struct
Cyc_Absyn_Exp* _temp542; struct Cyc_Absyn_Exp* _temp544; struct Cyc_Absyn_Exp*
_temp546; struct Cyc_List_List* _temp548; struct Cyc_Absyn_Exp* _temp550; struct
Cyc_List_List* _temp552; struct Cyc_Absyn_Exp* _temp554; struct Cyc_Absyn_Exp*
_temp556; struct Cyc_Absyn_Exp* _temp558; struct Cyc_Absyn_Exp* _temp560; struct
Cyc_Absyn_Exp* _temp562; void* _temp564; struct Cyc_Absyn_Exp* _temp566; struct
Cyc_Absyn_Exp* _temp568; struct Cyc_Absyn_Exp* _temp570; void* _temp572; struct
Cyc_Absyn_Exp* _temp574; void* _temp576; struct _tagged_arr* _temp578; void*
_temp580; void* _temp582; unsigned int _temp584; void* _temp586; void* _temp588;
struct Cyc_List_List* _temp590; struct Cyc_Absyn_Exp* _temp592; struct
_tagged_arr* _temp594; struct Cyc_Absyn_Exp* _temp596; struct _tagged_arr*
_temp598; struct Cyc_Absyn_Exp* _temp600; struct Cyc_Absyn_Exp* _temp602; struct
Cyc_Absyn_Exp* _temp604; struct Cyc_List_List* _temp606; struct Cyc_List_List*
_temp608; struct _tuple1* _temp610; struct Cyc_List_List* _temp612; struct Cyc_Absyn_Exp*
_temp614; struct Cyc_Absyn_Exp* _temp616; struct Cyc_Absyn_Vardecl* _temp618;
struct Cyc_List_List* _temp620; struct _tuple0* _temp622; struct Cyc_List_List*
_temp624; struct Cyc_Absyn_Tunionfield* _temp626; struct Cyc_List_List* _temp628;
struct _tuple0* _temp630; void* _temp632; struct Cyc_Absyn_Exp* _temp634; struct
Cyc_List_List* _temp636; struct Cyc_Core_Opt* _temp638; struct Cyc_Absyn_Stmt*
_temp640; struct Cyc_Absyn_Fndecl* _temp642; struct Cyc_Absyn_Exp* _temp644;
_LL372: if(*(( int*) _temp370) ==  Cyc_Absyn_Const_e){ _LL469: _temp468=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp370)->f1; if(( unsigned int) _temp468 > 
1u?*(( int*) _temp468) ==  Cyc_Absyn_Char_c: 0){ _LL473: _temp472=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp468)->f1; goto _LL471; _LL471: _temp470=((
struct Cyc_Absyn_Char_c_struct*) _temp468)->f2; goto _LL373;} else{ goto _LL374;}}
else{ goto _LL374;} _LL374: if(*(( int*) _temp370) ==  Cyc_Absyn_Const_e){
_LL475: _temp474=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp370)->f1; if((
unsigned int) _temp474 >  1u?*(( int*) _temp474) ==  Cyc_Absyn_Short_c: 0){
_LL479: _temp478=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp474)->f1;
goto _LL477; _LL477: _temp476=(( struct Cyc_Absyn_Short_c_struct*) _temp474)->f2;
goto _LL375;} else{ goto _LL376;}} else{ goto _LL376;} _LL376: if(*(( int*)
_temp370) ==  Cyc_Absyn_Const_e){ _LL481: _temp480=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp370)->f1; if(( unsigned int) _temp480 >  1u?*(( int*) _temp480) ==  Cyc_Absyn_Int_c:
0){ _LL485: _temp484=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp480)->f1;
if( _temp484 == ( void*) Cyc_Absyn_Signed){ goto _LL483;} else{ goto _LL378;}
_LL483: _temp482=(( struct Cyc_Absyn_Int_c_struct*) _temp480)->f2; goto _LL377;}
else{ goto _LL378;}} else{ goto _LL378;} _LL378: if(*(( int*) _temp370) ==  Cyc_Absyn_Const_e){
_LL487: _temp486=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp370)->f1; if((
unsigned int) _temp486 >  1u?*(( int*) _temp486) ==  Cyc_Absyn_Int_c: 0){ _LL491:
_temp490=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp486)->f1; if( _temp490
== ( void*) Cyc_Absyn_Unsigned){ goto _LL489;} else{ goto _LL380;} _LL489:
_temp488=(( struct Cyc_Absyn_Int_c_struct*) _temp486)->f2; goto _LL379;} else{
goto _LL380;}} else{ goto _LL380;} _LL380: if(*(( int*) _temp370) ==  Cyc_Absyn_Const_e){
_LL493: _temp492=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp370)->f1; if((
unsigned int) _temp492 >  1u?*(( int*) _temp492) ==  Cyc_Absyn_LongLong_c: 0){
_LL497: _temp496=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp492)->f1;
goto _LL495; _LL495: _temp494=(( struct Cyc_Absyn_LongLong_c_struct*) _temp492)->f2;
goto _LL381;} else{ goto _LL382;}} else{ goto _LL382;} _LL382: if(*(( int*)
_temp370) ==  Cyc_Absyn_Const_e){ _LL499: _temp498=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp370)->f1; if(( unsigned int) _temp498 >  1u?*(( int*) _temp498) ==  Cyc_Absyn_Float_c:
0){ _LL501: _temp500=(( struct Cyc_Absyn_Float_c_struct*) _temp498)->f1; goto
_LL383;} else{ goto _LL384;}} else{ goto _LL384;} _LL384: if(*(( int*) _temp370)
==  Cyc_Absyn_Const_e){ _LL503: _temp502=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp370)->f1; if( _temp502 == ( void*) Cyc_Absyn_Null_c){ goto _LL385;} else{
goto _LL386;}} else{ goto _LL386;} _LL386: if(*(( int*) _temp370) ==  Cyc_Absyn_Const_e){
_LL505: _temp504=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp370)->f1; if((
unsigned int) _temp504 >  1u?*(( int*) _temp504) ==  Cyc_Absyn_String_c: 0){
_LL507: _temp506=(( struct Cyc_Absyn_String_c_struct*) _temp504)->f1; goto
_LL387;} else{ goto _LL388;}} else{ goto _LL388;} _LL388: if(*(( int*) _temp370)
==  Cyc_Absyn_UnknownId_e){ _LL509: _temp508=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp370)->f1; goto _LL389;} else{ goto _LL390;} _LL390: if(*(( int*) _temp370)
==  Cyc_Absyn_Var_e){ _LL511: _temp510=(( struct Cyc_Absyn_Var_e_struct*)
_temp370)->f1; goto _LL391;} else{ goto _LL392;} _LL392: if(*(( int*) _temp370)
==  Cyc_Absyn_Primop_e){ _LL515: _temp514=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp370)->f1; goto _LL513; _LL513: _temp512=(( struct Cyc_Absyn_Primop_e_struct*)
_temp370)->f2; goto _LL393;} else{ goto _LL394;} _LL394: if(*(( int*) _temp370)
==  Cyc_Absyn_AssignOp_e){ _LL521: _temp520=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp370)->f1; goto _LL519; _LL519: _temp518=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp370)->f2; goto _LL517; _LL517: _temp516=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp370)->f3; goto _LL395;} else{ goto _LL396;} _LL396: if(*(( int*) _temp370)
==  Cyc_Absyn_Increment_e){ _LL525: _temp524=(( struct Cyc_Absyn_Increment_e_struct*)
_temp370)->f1; goto _LL523; _LL523: _temp522=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp370)->f2; if( _temp522 == ( void*) Cyc_Absyn_PreInc){ goto _LL397;} else{
goto _LL398;}} else{ goto _LL398;} _LL398: if(*(( int*) _temp370) ==  Cyc_Absyn_Increment_e){
_LL529: _temp528=(( struct Cyc_Absyn_Increment_e_struct*) _temp370)->f1; goto
_LL527; _LL527: _temp526=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp370)->f2; if( _temp526 == ( void*) Cyc_Absyn_PreDec){ goto _LL399;} else{
goto _LL400;}} else{ goto _LL400;} _LL400: if(*(( int*) _temp370) ==  Cyc_Absyn_Increment_e){
_LL533: _temp532=(( struct Cyc_Absyn_Increment_e_struct*) _temp370)->f1; goto
_LL531; _LL531: _temp530=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp370)->f2; if( _temp530 == ( void*) Cyc_Absyn_PostInc){ goto _LL401;} else{
goto _LL402;}} else{ goto _LL402;} _LL402: if(*(( int*) _temp370) ==  Cyc_Absyn_Increment_e){
_LL537: _temp536=(( struct Cyc_Absyn_Increment_e_struct*) _temp370)->f1; goto
_LL535; _LL535: _temp534=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp370)->f2; if( _temp534 == ( void*) Cyc_Absyn_PostDec){ goto _LL403;} else{
goto _LL404;}} else{ goto _LL404;} _LL404: if(*(( int*) _temp370) ==  Cyc_Absyn_Conditional_e){
_LL543: _temp542=(( struct Cyc_Absyn_Conditional_e_struct*) _temp370)->f1; goto
_LL541; _LL541: _temp540=(( struct Cyc_Absyn_Conditional_e_struct*) _temp370)->f2;
goto _LL539; _LL539: _temp538=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp370)->f3; goto _LL405;} else{ goto _LL406;} _LL406: if(*(( int*) _temp370)
==  Cyc_Absyn_SeqExp_e){ _LL547: _temp546=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp370)->f1; goto _LL545; _LL545: _temp544=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp370)->f2; goto _LL407;} else{ goto _LL408;} _LL408: if(*(( int*) _temp370)
==  Cyc_Absyn_UnknownCall_e){ _LL551: _temp550=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp370)->f1; goto _LL549; _LL549: _temp548=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp370)->f2; goto _LL409;} else{ goto _LL410;} _LL410: if(*(( int*) _temp370)
==  Cyc_Absyn_FnCall_e){ _LL555: _temp554=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp370)->f1; goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp370)->f2; goto _LL411;} else{ goto _LL412;} _LL412: if(*(( int*) _temp370)
==  Cyc_Absyn_Throw_e){ _LL557: _temp556=(( struct Cyc_Absyn_Throw_e_struct*)
_temp370)->f1; goto _LL413;} else{ goto _LL414;} _LL414: if(*(( int*) _temp370)
==  Cyc_Absyn_NoInstantiate_e){ _LL559: _temp558=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp370)->f1; goto _LL415;} else{ goto _LL416;} _LL416: if(*(( int*) _temp370)
==  Cyc_Absyn_Instantiate_e){ _LL561: _temp560=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp370)->f1; goto _LL417;} else{ goto _LL418;} _LL418: if(*(( int*) _temp370)
==  Cyc_Absyn_Cast_e){ _LL565: _temp564=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp370)->f1; goto _LL563; _LL563: _temp562=(( struct Cyc_Absyn_Cast_e_struct*)
_temp370)->f2; goto _LL419;} else{ goto _LL420;} _LL420: if(*(( int*) _temp370)
==  Cyc_Absyn_Address_e){ _LL567: _temp566=(( struct Cyc_Absyn_Address_e_struct*)
_temp370)->f1; goto _LL421;} else{ goto _LL422;} _LL422: if(*(( int*) _temp370)
==  Cyc_Absyn_New_e){ _LL571: _temp570=(( struct Cyc_Absyn_New_e_struct*)
_temp370)->f1; goto _LL569; _LL569: _temp568=(( struct Cyc_Absyn_New_e_struct*)
_temp370)->f2; goto _LL423;} else{ goto _LL424;} _LL424: if(*(( int*) _temp370)
==  Cyc_Absyn_Sizeoftyp_e){ _LL573: _temp572=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp370)->f1; goto _LL425;} else{ goto _LL426;} _LL426: if(*(( int*) _temp370)
==  Cyc_Absyn_Sizeofexp_e){ _LL575: _temp574=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp370)->f1; goto _LL427;} else{ goto _LL428;} _LL428: if(*(( int*) _temp370)
==  Cyc_Absyn_Offsetof_e){ _LL581: _temp580=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp370)->f1; goto _LL577; _LL577: _temp576=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp370)->f2; if(*(( int*) _temp576) ==  Cyc_Absyn_StructField){ _LL579:
_temp578=(( struct Cyc_Absyn_StructField_struct*) _temp576)->f1; goto _LL429;}
else{ goto _LL430;}} else{ goto _LL430;} _LL430: if(*(( int*) _temp370) ==  Cyc_Absyn_Offsetof_e){
_LL587: _temp586=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp370)->f1;
goto _LL583; _LL583: _temp582=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp370)->f2; if(*(( int*) _temp582) ==  Cyc_Absyn_TupleIndex){ _LL585:
_temp584=(( struct Cyc_Absyn_TupleIndex_struct*) _temp582)->f1; goto _LL431;}
else{ goto _LL432;}} else{ goto _LL432;} _LL432: if(*(( int*) _temp370) ==  Cyc_Absyn_Gentyp_e){
_LL591: _temp590=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp370)->f1; goto
_LL589; _LL589: _temp588=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp370)->f2;
goto _LL433;} else{ goto _LL434;} _LL434: if(*(( int*) _temp370) ==  Cyc_Absyn_Deref_e){
_LL593: _temp592=(( struct Cyc_Absyn_Deref_e_struct*) _temp370)->f1; goto _LL435;}
else{ goto _LL436;} _LL436: if(*(( int*) _temp370) ==  Cyc_Absyn_StructMember_e){
_LL597: _temp596=(( struct Cyc_Absyn_StructMember_e_struct*) _temp370)->f1; goto
_LL595; _LL595: _temp594=(( struct Cyc_Absyn_StructMember_e_struct*) _temp370)->f2;
goto _LL437;} else{ goto _LL438;} _LL438: if(*(( int*) _temp370) ==  Cyc_Absyn_StructArrow_e){
_LL601: _temp600=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp370)->f1; goto
_LL599; _LL599: _temp598=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp370)->f2;
goto _LL439;} else{ goto _LL440;} _LL440: if(*(( int*) _temp370) ==  Cyc_Absyn_Subscript_e){
_LL605: _temp604=(( struct Cyc_Absyn_Subscript_e_struct*) _temp370)->f1; goto
_LL603; _LL603: _temp602=(( struct Cyc_Absyn_Subscript_e_struct*) _temp370)->f2;
goto _LL441;} else{ goto _LL442;} _LL442: if(*(( int*) _temp370) ==  Cyc_Absyn_Tuple_e){
_LL607: _temp606=(( struct Cyc_Absyn_Tuple_e_struct*) _temp370)->f1; goto _LL443;}
else{ goto _LL444;} _LL444: if(*(( int*) _temp370) ==  Cyc_Absyn_CompoundLit_e){
_LL611: _temp610=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp370)->f1; goto
_LL609; _LL609: _temp608=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp370)->f2;
goto _LL445;} else{ goto _LL446;} _LL446: if(*(( int*) _temp370) ==  Cyc_Absyn_Array_e){
_LL613: _temp612=(( struct Cyc_Absyn_Array_e_struct*) _temp370)->f1; goto _LL447;}
else{ goto _LL448;} _LL448: if(*(( int*) _temp370) ==  Cyc_Absyn_Comprehension_e){
_LL619: _temp618=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp370)->f1;
goto _LL617; _LL617: _temp616=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp370)->f2; goto _LL615; _LL615: _temp614=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp370)->f3; goto _LL449;} else{ goto _LL450;} _LL450: if(*(( int*) _temp370)
==  Cyc_Absyn_Struct_e){ _LL623: _temp622=(( struct Cyc_Absyn_Struct_e_struct*)
_temp370)->f1; goto _LL621; _LL621: _temp620=(( struct Cyc_Absyn_Struct_e_struct*)
_temp370)->f3; goto _LL451;} else{ goto _LL452;} _LL452: if(*(( int*) _temp370)
==  Cyc_Absyn_AnonStruct_e){ _LL625: _temp624=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp370)->f2; goto _LL453;} else{ goto _LL454;} _LL454: if(*(( int*) _temp370)
==  Cyc_Absyn_Tunion_e){ _LL629: _temp628=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp370)->f3; goto _LL627; _LL627: _temp626=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp370)->f5; goto _LL455;} else{ goto _LL456;} _LL456: if(*(( int*) _temp370)
==  Cyc_Absyn_Enum_e){ _LL631: _temp630=(( struct Cyc_Absyn_Enum_e_struct*)
_temp370)->f1; goto _LL457;} else{ goto _LL458;} _LL458: if(*(( int*) _temp370)
==  Cyc_Absyn_Malloc_e){ _LL635: _temp634=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp370)->f1; goto _LL633; _LL633: _temp632=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp370)->f2; goto _LL459;} else{ goto _LL460;} _LL460: if(*(( int*) _temp370)
==  Cyc_Absyn_UnresolvedMem_e){ _LL639: _temp638=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp370)->f1; goto _LL637; _LL637: _temp636=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp370)->f2; goto _LL461;} else{ goto _LL462;} _LL462: if(*(( int*) _temp370)
==  Cyc_Absyn_StmtExp_e){ _LL641: _temp640=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp370)->f1; goto _LL463;} else{ goto _LL464;} _LL464: if(*(( int*) _temp370)
==  Cyc_Absyn_Codegen_e){ _LL643: _temp642=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp370)->f1; goto _LL465;} else{ goto _LL466;} _LL466: if(*(( int*) _temp370)
==  Cyc_Absyn_Fill_e){ _LL645: _temp644=(( struct Cyc_Absyn_Fill_e_struct*)
_temp370)->f1; goto _LL467;} else{ goto _LL371;} _LL373: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp470)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL371; _LL375: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp647; _temp647.tag= Cyc_Std_Int_pa; _temp647.f1=(
int)(( unsigned int)(( int) _temp476));{ void* _temp646[ 1u]={& _temp647}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp646, sizeof( void*),
1u));}})); goto _LL371; _LL377: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp649; _temp649.tag= Cyc_Std_Int_pa; _temp649.f1=(
int)(( unsigned int) _temp482);{ void* _temp648[ 1u]={& _temp649}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp648, sizeof( void*),
1u));}})); goto _LL371; _LL379: Cyc_Absyndump_dump(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp651; _temp651.tag= Cyc_Std_Int_pa; _temp651.f1=(
int)(( unsigned int) _temp488);{ void* _temp650[ 1u]={& _temp651}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp650, sizeof( void*),
1u));}})); Cyc_Absyndump_dump_nospace( _tag_arr("u", sizeof( unsigned char), 2u));
goto _LL371; _LL381: Cyc_Absyndump_dump( _tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof( unsigned char), 27u)); goto _LL371; _LL383: Cyc_Absyndump_dump( _temp500);
goto _LL371; _LL385: Cyc_Absyndump_dump( _tag_arr("NULL", sizeof( unsigned char),
5u)); goto _LL371; _LL387: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp506)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL371; _LL389: _temp510= _temp508; goto _LL391; _LL391: Cyc_Absyndump_dumpqvar(
_temp510); goto _LL371; _LL393: { struct _tagged_arr _temp652= Cyc_Absynpp_prim2str(
_temp514); switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp512)){ case 1: _LL653: if( _temp514 == ( void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp512))->hd);
Cyc_Absyndump_dump( _tag_arr(".size", sizeof( unsigned char), 6u));} else{ Cyc_Absyndump_dump(
_temp652); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp512))->hd);} break; case 2: _LL654: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp512))->hd);
Cyc_Absyndump_dump( _temp652); Cyc_Absyndump_dump_char(( int)' '); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( _temp512))->tl))->hd); break; default: _LL655:( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp657=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp657[ 0]=({ struct Cyc_Core_Failure_struct
_temp658; _temp658.tag= Cyc_Core_Failure; _temp658.f1= _tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof( unsigned char), 47u); _temp658;}); _temp657;}));} goto _LL371;} _LL395:
Cyc_Absyndump_dumpexp_prec( myprec, _temp520); if( _temp518 !=  0){ Cyc_Absyndump_dump(
Cyc_Absynpp_prim2str(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp518))->v));}
Cyc_Absyndump_dump_nospace( _tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp516); goto _LL371; _LL397: Cyc_Absyndump_dump( _tag_arr("++",
sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp524);
goto _LL371; _LL399: Cyc_Absyndump_dump( _tag_arr("--", sizeof( unsigned char),
3u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp528); goto _LL371; _LL401: Cyc_Absyndump_dumpexp_prec(
myprec, _temp532); Cyc_Absyndump_dump( _tag_arr("++", sizeof( unsigned char), 3u));
goto _LL371; _LL403: Cyc_Absyndump_dumpexp_prec( myprec, _temp536); Cyc_Absyndump_dump(
_tag_arr("--", sizeof( unsigned char), 3u)); goto _LL371; _LL405: Cyc_Absyndump_dumpexp_prec(
myprec, _temp542); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp540); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp538); goto _LL371; _LL407: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp546); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp544); Cyc_Absyndump_dump_char(( int)')'); goto _LL371; _LL409:
_temp554= _temp550; _temp552= _temp548; goto _LL411; _LL411: Cyc_Absyndump_dumpexp_prec(
myprec, _temp554); Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumpexps_prec( 20, _temp552); Cyc_Absyndump_dump_nospace(
_tag_arr(")", sizeof( unsigned char), 2u)); goto _LL371; _LL413: Cyc_Absyndump_dump(
_tag_arr("throw", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp556); goto _LL371; _LL415: _temp560= _temp558; goto _LL417; _LL417:
Cyc_Absyndump_dumpexp_prec( inprec, _temp560); goto _LL371; _LL419: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp564); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp562); goto _LL371; _LL421: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp566); goto _LL371; _LL423:
Cyc_Absyndump_dump( _tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp568); goto _LL371; _LL425: Cyc_Absyndump_dump( _tag_arr("sizeof(",
sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptyp( _temp572); Cyc_Absyndump_dump_char((
int)')'); goto _LL371; _LL427: Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp574); Cyc_Absyndump_dump_char((
int)')'); goto _LL371; _LL429: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp580); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump_nospace(* _temp578); Cyc_Absyndump_dump_char(( int)')');
goto _LL371; _LL431: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp586); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp660; _temp660.tag= Cyc_Std_Int_pa; _temp660.f1=( int) _temp584;{ void*
_temp659[ 1u]={& _temp660}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp659, sizeof( void*), 1u));}})); Cyc_Absyndump_dump_char((
int)')'); goto _LL371; _LL433: Cyc_Absyndump_dump( _tag_arr("__gen(", sizeof(
unsigned char), 7u)); Cyc_Absyndump_dumptvars( _temp590); Cyc_Absyndump_dumptyp(
_temp588); Cyc_Absyndump_dump_char(( int)')'); goto _LL371; _LL435: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dumpexp_prec( myprec, _temp592); goto _LL371; _LL437:
Cyc_Absyndump_dumpexp_prec( myprec, _temp596); Cyc_Absyndump_dump_char(( int)'.');
Cyc_Absyndump_dump_nospace(* _temp594); goto _LL371; _LL439: Cyc_Absyndump_dumpexp_prec(
myprec, _temp600); Cyc_Absyndump_dump_nospace( _tag_arr("->", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dump_nospace(* _temp598); goto _LL371;
_LL441: Cyc_Absyndump_dumpexp_prec( myprec, _temp604); Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp602); Cyc_Absyndump_dump_char(( int)']');
goto _LL371; _LL443: Cyc_Absyndump_dump( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_Absyndump_dumpexps_prec( 20, _temp606); Cyc_Absyndump_dump_char(( int)')');
goto _LL371; _LL445: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp610).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp608, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL371;
_LL447:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp612, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL371; _LL449: Cyc_Absyndump_dump( _tag_arr("new {for", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dump_str((* _temp618->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp616); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp614); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL371; _LL451: Cyc_Absyndump_dumpqvar( _temp622);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp620, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL371;
_LL453:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp624, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL371; _LL455: Cyc_Absyndump_dumpqvar( _temp626->name); if( _temp628
!=  0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp628, _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u));} goto _LL371; _LL457: Cyc_Absyndump_dumpqvar( _temp630); goto _LL371;
_LL459: if( _temp634 !=  0){ Cyc_Absyndump_dump( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp634)); Cyc_Absyndump_dump( _tag_arr(",", sizeof( unsigned char), 2u));}
else{ Cyc_Absyndump_dump( _tag_arr("malloc(", sizeof( unsigned char), 8u));} Cyc_Absyndump_dump(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptyp(
_temp632); Cyc_Absyndump_dump( _tag_arr("))", sizeof( unsigned char), 3u)); goto
_LL371; _LL461:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp636, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL371; _LL463: Cyc_Absyndump_dump_nospace( _tag_arr("({", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp640); Cyc_Absyndump_dump_nospace(
_tag_arr("})", sizeof( unsigned char), 3u)); goto _LL371; _LL465: Cyc_Absyndump_dump(
_tag_arr("codegen(", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpdecl(({
struct Cyc_Absyn_Decl* _temp661=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp661->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp662=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp662[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp663; _temp663.tag= Cyc_Absyn_Fn_d;
_temp663.f1= _temp642; _temp663;}); _temp662;})); _temp661->loc= e->loc;
_temp661;})); Cyc_Absyndump_dump( _tag_arr(")", sizeof( unsigned char), 2u));
goto _LL371; _LL467: Cyc_Absyndump_dump( _tag_arr("fill(", sizeof( unsigned char),
6u)); Cyc_Absyndump_dumpexp( _temp644); Cyc_Absyndump_dump( _tag_arr(")",
sizeof( unsigned char), 2u)); goto _LL371; _LL371:;} if( inprec >=  myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs !=  0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp664=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; if( _temp664->where_clause ==  0?( void*)( _temp664->pattern)->r == (
void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump( _tag_arr("default:", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("case", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumppat( _temp664->pattern); if( _temp664->where_clause
!=  0){ Cyc_Absyndump_dump( _tag_arr("&&", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp664->where_clause));} Cyc_Absyndump_dump_nospace(
_tag_arr(":", sizeof( unsigned char), 2u));} Cyc_Absyndump_dumpstmt( _temp664->body);}}
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt* s){ void* _temp665=( void*)
s->r; struct Cyc_Absyn_Exp* _temp711; struct Cyc_Absyn_Stmt* _temp713; struct
Cyc_Absyn_Stmt* _temp715; struct Cyc_Absyn_Exp* _temp717; struct Cyc_Absyn_Exp*
_temp719; struct Cyc_Absyn_Stmt* _temp721; struct Cyc_Absyn_Stmt* _temp723;
struct Cyc_Absyn_Exp* _temp725; struct Cyc_Absyn_Stmt* _temp727; struct _tuple2
_temp729; struct Cyc_Absyn_Exp* _temp731; struct _tagged_arr* _temp733; struct
Cyc_Absyn_Stmt* _temp735; struct _tuple2 _temp737; struct Cyc_Absyn_Exp*
_temp739; struct _tuple2 _temp741; struct Cyc_Absyn_Exp* _temp743; struct Cyc_Absyn_Exp*
_temp745; struct Cyc_List_List* _temp747; struct Cyc_Absyn_Exp* _temp749; struct
Cyc_Absyn_Stmt* _temp751; struct Cyc_Absyn_Decl* _temp753; struct Cyc_Absyn_Stmt*
_temp755; struct _tagged_arr* _temp757; struct _tuple2 _temp759; struct Cyc_Absyn_Exp*
_temp761; struct Cyc_Absyn_Stmt* _temp763; struct Cyc_List_List* _temp765;
struct Cyc_Absyn_Exp* _temp767; struct Cyc_List_List* _temp769; struct Cyc_List_List*
_temp771; struct Cyc_List_List* _temp773; struct Cyc_Absyn_Stmt* _temp775;
struct Cyc_Absyn_Stmt* _temp777; struct Cyc_Absyn_Vardecl* _temp779; struct Cyc_Absyn_Tvar*
_temp781; struct Cyc_Absyn_Stmt* _temp783; struct Cyc_Absyn_Stmt* _temp785;
_LL667: if( _temp665 == ( void*) Cyc_Absyn_Skip_s){ goto _LL668;} else{ goto
_LL669;} _LL669: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Exp_s:
0){ _LL712: _temp711=(( struct Cyc_Absyn_Exp_s_struct*) _temp665)->f1; goto
_LL670;} else{ goto _LL671;} _LL671: if(( unsigned int) _temp665 >  1u?*(( int*)
_temp665) ==  Cyc_Absyn_Seq_s: 0){ _LL716: _temp715=(( struct Cyc_Absyn_Seq_s_struct*)
_temp665)->f1; goto _LL714; _LL714: _temp713=(( struct Cyc_Absyn_Seq_s_struct*)
_temp665)->f2; goto _LL672;} else{ goto _LL673;} _LL673: if(( unsigned int)
_temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Return_s: 0){ _LL718: _temp717=((
struct Cyc_Absyn_Return_s_struct*) _temp665)->f1; if( _temp717 ==  0){ goto
_LL674;} else{ goto _LL675;}} else{ goto _LL675;} _LL675: if(( unsigned int)
_temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Return_s: 0){ _LL720: _temp719=((
struct Cyc_Absyn_Return_s_struct*) _temp665)->f1; goto _LL676;} else{ goto
_LL677;} _LL677: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_IfThenElse_s:
0){ _LL726: _temp725=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp665)->f1;
goto _LL724; _LL724: _temp723=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp665)->f2;
goto _LL722; _LL722: _temp721=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp665)->f3;
goto _LL678;} else{ goto _LL679;} _LL679: if(( unsigned int) _temp665 >  1u?*((
int*) _temp665) ==  Cyc_Absyn_While_s: 0){ _LL730: _temp729=(( struct Cyc_Absyn_While_s_struct*)
_temp665)->f1; _LL732: _temp731= _temp729.f1; goto _LL728; _LL728: _temp727=((
struct Cyc_Absyn_While_s_struct*) _temp665)->f2; goto _LL680;} else{ goto _LL681;}
_LL681: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Break_s:
0){ goto _LL682;} else{ goto _LL683;} _LL683: if(( unsigned int) _temp665 >  1u?*((
int*) _temp665) ==  Cyc_Absyn_Continue_s: 0){ goto _LL684;} else{ goto _LL685;}
_LL685: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Goto_s:
0){ _LL734: _temp733=(( struct Cyc_Absyn_Goto_s_struct*) _temp665)->f1; goto
_LL686;} else{ goto _LL687;} _LL687: if(( unsigned int) _temp665 >  1u?*(( int*)
_temp665) ==  Cyc_Absyn_For_s: 0){ _LL746: _temp745=(( struct Cyc_Absyn_For_s_struct*)
_temp665)->f1; goto _LL742; _LL742: _temp741=(( struct Cyc_Absyn_For_s_struct*)
_temp665)->f2; _LL744: _temp743= _temp741.f1; goto _LL738; _LL738: _temp737=((
struct Cyc_Absyn_For_s_struct*) _temp665)->f3; _LL740: _temp739= _temp737.f1;
goto _LL736; _LL736: _temp735=(( struct Cyc_Absyn_For_s_struct*) _temp665)->f4;
goto _LL688;} else{ goto _LL689;} _LL689: if(( unsigned int) _temp665 >  1u?*((
int*) _temp665) ==  Cyc_Absyn_Switch_s: 0){ _LL750: _temp749=(( struct Cyc_Absyn_Switch_s_struct*)
_temp665)->f1; goto _LL748; _LL748: _temp747=(( struct Cyc_Absyn_Switch_s_struct*)
_temp665)->f2; goto _LL690;} else{ goto _LL691;} _LL691: if(( unsigned int)
_temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Decl_s: 0){ _LL754: _temp753=((
struct Cyc_Absyn_Decl_s_struct*) _temp665)->f1; goto _LL752; _LL752: _temp751=((
struct Cyc_Absyn_Decl_s_struct*) _temp665)->f2; goto _LL692;} else{ goto _LL693;}
_LL693: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Label_s:
0){ _LL758: _temp757=(( struct Cyc_Absyn_Label_s_struct*) _temp665)->f1; goto
_LL756; _LL756: _temp755=(( struct Cyc_Absyn_Label_s_struct*) _temp665)->f2;
goto _LL694;} else{ goto _LL695;} _LL695: if(( unsigned int) _temp665 >  1u?*((
int*) _temp665) ==  Cyc_Absyn_Do_s: 0){ _LL764: _temp763=(( struct Cyc_Absyn_Do_s_struct*)
_temp665)->f1; goto _LL760; _LL760: _temp759=(( struct Cyc_Absyn_Do_s_struct*)
_temp665)->f2; _LL762: _temp761= _temp759.f1; goto _LL696;} else{ goto _LL697;}
_LL697: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_SwitchC_s:
0){ _LL768: _temp767=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp665)->f1; goto
_LL766; _LL766: _temp765=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp665)->f2;
goto _LL698;} else{ goto _LL699;} _LL699: if(( unsigned int) _temp665 >  1u?*((
int*) _temp665) ==  Cyc_Absyn_Fallthru_s: 0){ _LL770: _temp769=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp665)->f1; if( _temp769 ==  0){ goto _LL700;} else{ goto _LL701;}} else{
goto _LL701;} _LL701: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) == 
Cyc_Absyn_Fallthru_s: 0){ _LL772: _temp771=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp665)->f1; goto _LL702;} else{ goto _LL703;} _LL703: if(( unsigned int)
_temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_TryCatch_s: 0){ _LL776:
_temp775=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp665)->f1; goto _LL774;
_LL774: _temp773=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp665)->f2; goto
_LL704;} else{ goto _LL705;} _LL705: if(( unsigned int) _temp665 >  1u?*(( int*)
_temp665) ==  Cyc_Absyn_Region_s: 0){ _LL782: _temp781=(( struct Cyc_Absyn_Region_s_struct*)
_temp665)->f1; goto _LL780; _LL780: _temp779=(( struct Cyc_Absyn_Region_s_struct*)
_temp665)->f2; goto _LL778; _LL778: _temp777=(( struct Cyc_Absyn_Region_s_struct*)
_temp665)->f3; goto _LL706;} else{ goto _LL707;} _LL707: if(( unsigned int)
_temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Cut_s: 0){ _LL784: _temp783=((
struct Cyc_Absyn_Cut_s_struct*) _temp665)->f1; goto _LL708;} else{ goto _LL709;}
_LL709: if(( unsigned int) _temp665 >  1u?*(( int*) _temp665) ==  Cyc_Absyn_Splice_s:
0){ _LL786: _temp785=(( struct Cyc_Absyn_Splice_s_struct*) _temp665)->f1; goto
_LL710;} else{ goto _LL666;} _LL668: Cyc_Absyndump_dump_semi(); goto _LL666;
_LL670: Cyc_Absyndump_dumpexp( _temp711); Cyc_Absyndump_dump_semi(); goto _LL666;
_LL672: if( Cyc_Absynpp_is_declaration( _temp715)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp715); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp715);} if( Cyc_Absynpp_is_declaration(
_temp713)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp713);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp713);}
goto _LL666; _LL674: Cyc_Absyndump_dump( _tag_arr("return;", sizeof(
unsigned char), 8u)); goto _LL666; _LL676: Cyc_Absyndump_dump( _tag_arr("return",
sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp719)); Cyc_Absyndump_dump_semi(); goto _LL666; _LL678: Cyc_Absyndump_dump(
_tag_arr("if(", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp( _temp725);
Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt(
_temp723); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp787=( void*) _temp721->r;
_LL789: if( _temp787 == ( void*) Cyc_Absyn_Skip_s){ goto _LL790;} else{ goto
_LL791;} _LL791: goto _LL792; _LL790: goto _LL788; _LL792: Cyc_Absyndump_dump(
_tag_arr("else{", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpstmt( _temp721);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL788; _LL788:;} goto _LL666; _LL680:
Cyc_Absyndump_dump( _tag_arr("while(", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(
_temp731); Cyc_Absyndump_dump_nospace( _tag_arr(") {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp727); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL666; _LL682: Cyc_Absyndump_dump( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL666; _LL684: Cyc_Absyndump_dump( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL666; _LL686: Cyc_Absyndump_dump( _tag_arr("goto",
sizeof( unsigned char), 5u)); Cyc_Absyndump_dump_str( _temp733); Cyc_Absyndump_dump_semi();
goto _LL666; _LL688: Cyc_Absyndump_dump( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_Absyndump_dumpexp( _temp745); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp743); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp739); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp735);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL666; _LL690: Cyc_Absyndump_dump(
_tag_arr("switch(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpexp(
_temp749); Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpswitchclauses( _temp747); Cyc_Absyndump_dump_char(( int)'}');
goto _LL666; _LL692: Cyc_Absyndump_dumpdecl( _temp753); Cyc_Absyndump_dumpstmt(
_temp751); goto _LL666; _LL694: if( Cyc_Absynpp_is_declaration( _temp755)){ Cyc_Absyndump_dump_str(
_temp757); Cyc_Absyndump_dump_nospace( _tag_arr(": {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp755); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dump_str( _temp757); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt(
_temp755);} goto _LL666; _LL696: Cyc_Absyndump_dump( _tag_arr("do {", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumpstmt( _temp763); Cyc_Absyndump_dump_nospace(
_tag_arr("} while (", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexp(
_temp761); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char), 3u));
goto _LL666; _LL698: Cyc_Absyndump_dump( _tag_arr("switch \"C\" (", sizeof(
unsigned char), 13u)); Cyc_Absyndump_dumpexp( _temp767); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); for( 0; _temp765 !=  0; _temp765=((
struct Cyc_List_List*) _check_null( _temp765))->tl){ struct Cyc_Absyn_SwitchC_clause
_temp795; struct Cyc_Absyn_Stmt* _temp796; struct Cyc_Absyn_Exp* _temp798;
struct Cyc_Absyn_SwitchC_clause* _temp793=( struct Cyc_Absyn_SwitchC_clause*)((
struct Cyc_List_List*) _check_null( _temp765))->hd; _temp795=* _temp793; _LL799:
_temp798= _temp795.cnst_exp; goto _LL797; _LL797: _temp796= _temp795.body; goto
_LL794; _LL794: if( _temp798 ==  0){ Cyc_Absyndump_dump( _tag_arr("default: ",
sizeof( unsigned char), 10u));} else{ Cyc_Absyndump_dump( _tag_arr("case ",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp798)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp796);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL666; _LL700: Cyc_Absyndump_dump(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL666; _LL702: Cyc_Absyndump_dump(
_tag_arr("fallthru(", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexps_prec(
20, _temp771); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char),
3u)); goto _LL666; _LL704: Cyc_Absyndump_dump( _tag_arr("try", sizeof(
unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp775); Cyc_Absyndump_dump(
_tag_arr("catch {", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpswitchclauses(
_temp773); Cyc_Absyndump_dump_char(( int)'}'); goto _LL666; _LL706: Cyc_Absyndump_dump(
_tag_arr("region<", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptvar(
_temp781); Cyc_Absyndump_dump( _tag_arr("> ", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpqvar(
_temp779->name); Cyc_Absyndump_dump( _tag_arr("{", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpstmt( _temp777); Cyc_Absyndump_dump( _tag_arr("}", sizeof(
unsigned char), 2u)); goto _LL666; _LL708: Cyc_Absyndump_dump( _tag_arr("cut",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp783); goto _LL666;
_LL710: Cyc_Absyndump_dump( _tag_arr("splice", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpstmt(
_temp785); goto _LL666; _LL666:;} struct _tuple9{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple9* dp){
Cyc_Absyndump_egroup( Cyc_Absyndump_dumpdesignator,(* dp).f1, _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=",
sizeof( unsigned char), 2u)); Cyc_Absyndump_dumppat((* dp).f2);} void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat* p){ void* _temp800=( void*) p->r; int _temp834; void*
_temp836; int _temp838; void* _temp840; unsigned char _temp842; struct
_tagged_arr _temp844; struct Cyc_Absyn_Vardecl* _temp846; struct Cyc_List_List*
_temp848; struct Cyc_Absyn_Pat* _temp850; struct Cyc_Absyn_Vardecl* _temp852;
struct _tuple0* _temp854; struct Cyc_List_List* _temp856; struct Cyc_List_List*
_temp858; struct _tuple0* _temp860; struct Cyc_List_List* _temp862; struct Cyc_List_List*
_temp864; struct _tuple0* _temp866; struct Cyc_List_List* _temp868; struct Cyc_List_List*
_temp870; struct Cyc_Absyn_Structdecl* _temp872; struct Cyc_List_List* _temp874;
struct Cyc_List_List* _temp876; struct Cyc_Absyn_Tunionfield* _temp878; struct
Cyc_Absyn_Enumfield* _temp880; _LL802: if( _temp800 == ( void*) Cyc_Absyn_Wild_p){
goto _LL803;} else{ goto _LL804;} _LL804: if( _temp800 == ( void*) Cyc_Absyn_Null_p){
goto _LL805;} else{ goto _LL806;} _LL806: if(( unsigned int) _temp800 >  2u?*((
int*) _temp800) ==  Cyc_Absyn_Int_p: 0){ _LL837: _temp836=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp800)->f1; if( _temp836 == ( void*) Cyc_Absyn_Signed){ goto _LL835;} else{
goto _LL808;} _LL835: _temp834=(( struct Cyc_Absyn_Int_p_struct*) _temp800)->f2;
goto _LL807;} else{ goto _LL808;} _LL808: if(( unsigned int) _temp800 >  2u?*((
int*) _temp800) ==  Cyc_Absyn_Int_p: 0){ _LL841: _temp840=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp800)->f1; if( _temp840 == ( void*) Cyc_Absyn_Unsigned){ goto _LL839;} else{
goto _LL810;} _LL839: _temp838=(( struct Cyc_Absyn_Int_p_struct*) _temp800)->f2;
goto _LL809;} else{ goto _LL810;} _LL810: if(( unsigned int) _temp800 >  2u?*((
int*) _temp800) ==  Cyc_Absyn_Char_p: 0){ _LL843: _temp842=(( struct Cyc_Absyn_Char_p_struct*)
_temp800)->f1; goto _LL811;} else{ goto _LL812;} _LL812: if(( unsigned int)
_temp800 >  2u?*(( int*) _temp800) ==  Cyc_Absyn_Float_p: 0){ _LL845: _temp844=((
struct Cyc_Absyn_Float_p_struct*) _temp800)->f1; goto _LL813;} else{ goto _LL814;}
_LL814: if(( unsigned int) _temp800 >  2u?*(( int*) _temp800) ==  Cyc_Absyn_Var_p:
0){ _LL847: _temp846=(( struct Cyc_Absyn_Var_p_struct*) _temp800)->f1; goto
_LL815;} else{ goto _LL816;} _LL816: if(( unsigned int) _temp800 >  2u?*(( int*)
_temp800) ==  Cyc_Absyn_Tuple_p: 0){ _LL849: _temp848=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp800)->f1; goto _LL817;} else{ goto _LL818;} _LL818: if(( unsigned int)
_temp800 >  2u?*(( int*) _temp800) ==  Cyc_Absyn_Pointer_p: 0){ _LL851: _temp850=((
struct Cyc_Absyn_Pointer_p_struct*) _temp800)->f1; goto _LL819;} else{ goto
_LL820;} _LL820: if(( unsigned int) _temp800 >  2u?*(( int*) _temp800) ==  Cyc_Absyn_Reference_p:
0){ _LL853: _temp852=(( struct Cyc_Absyn_Reference_p_struct*) _temp800)->f1;
goto _LL821;} else{ goto _LL822;} _LL822: if(( unsigned int) _temp800 >  2u?*((
int*) _temp800) ==  Cyc_Absyn_UnknownId_p: 0){ _LL855: _temp854=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp800)->f1; goto _LL823;} else{ goto _LL824;} _LL824: if(( unsigned int)
_temp800 >  2u?*(( int*) _temp800) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL861:
_temp860=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp800)->f1; goto _LL859;
_LL859: _temp858=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp800)->f2; goto
_LL857; _LL857: _temp856=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp800)->f3;
goto _LL825;} else{ goto _LL826;} _LL826: if(( unsigned int) _temp800 >  2u?*((
int*) _temp800) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL867: _temp866=(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp800)->f1; goto _LL865; _LL865: _temp864=((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp800)->f2; goto _LL863; _LL863:
_temp862=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp800)->f3; goto _LL827;}
else{ goto _LL828;} _LL828: if(( unsigned int) _temp800 >  2u?*(( int*) _temp800)
==  Cyc_Absyn_Struct_p: 0){ _LL873: _temp872=(( struct Cyc_Absyn_Struct_p_struct*)
_temp800)->f1; goto _LL871; _LL871: _temp870=(( struct Cyc_Absyn_Struct_p_struct*)
_temp800)->f3; goto _LL869; _LL869: _temp868=(( struct Cyc_Absyn_Struct_p_struct*)
_temp800)->f4; goto _LL829;} else{ goto _LL830;} _LL830: if(( unsigned int)
_temp800 >  2u?*(( int*) _temp800) ==  Cyc_Absyn_Tunion_p: 0){ _LL879: _temp878=((
struct Cyc_Absyn_Tunion_p_struct*) _temp800)->f2; goto _LL877; _LL877: _temp876=((
struct Cyc_Absyn_Tunion_p_struct*) _temp800)->f3; goto _LL875; _LL875: _temp874=((
struct Cyc_Absyn_Tunion_p_struct*) _temp800)->f4; goto _LL831;} else{ goto
_LL832;} _LL832: if(( unsigned int) _temp800 >  2u?*(( int*) _temp800) ==  Cyc_Absyn_Enum_p:
0){ _LL881: _temp880=(( struct Cyc_Absyn_Enum_p_struct*) _temp800)->f2; goto
_LL833;} else{ goto _LL801;} _LL803: Cyc_Absyndump_dump_char(( int)'_'); goto
_LL801; _LL805: Cyc_Absyndump_dump( _tag_arr("NULL", sizeof( unsigned char), 5u));
goto _LL801; _LL807: Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp883; _temp883.tag= Cyc_Std_Int_pa; _temp883.f1=( int)(( unsigned int)
_temp834);{ void* _temp882[ 1u]={& _temp883}; Cyc_Std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp882, sizeof( void*), 1u));}})); goto
_LL801; _LL809: Cyc_Absyndump_dump(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp885; _temp885.tag= Cyc_Std_Int_pa; _temp885.f1=( unsigned int) _temp838;{
void* _temp884[ 1u]={& _temp885}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp884, sizeof( void*), 1u));}})); goto _LL801;
_LL811: Cyc_Absyndump_dump( _tag_arr("'", sizeof( unsigned char), 2u)); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp842)); Cyc_Absyndump_dump_nospace( _tag_arr("'",
sizeof( unsigned char), 2u)); goto _LL801; _LL813: Cyc_Absyndump_dump( _temp844);
goto _LL801; _LL815: Cyc_Absyndump_dumpqvar( _temp846->name); goto _LL801;
_LL817:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp848, _tag_arr("$(", sizeof( unsigned char), 3u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL801; _LL819: Cyc_Absyndump_dump( _tag_arr("&", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumppat( _temp850); goto _LL801; _LL821: Cyc_Absyndump_dump(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar( _temp852->name);
goto _LL801; _LL823: Cyc_Absyndump_dumpqvar( _temp854); goto _LL801; _LL825: Cyc_Absyndump_dumpqvar(
_temp860); Cyc_Absyndump_dumptvars( _temp858);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp856,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL801; _LL827: Cyc_Absyndump_dumpqvar(
_temp866); Cyc_Absyndump_dumptvars( _temp864);(( void(*)( void(* f)( struct
_tuple9*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp862, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL801;
_LL829: if( _temp872->name !=  0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp872->name))->v);} Cyc_Absyndump_dumptvars(
_temp870);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp868, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL801; _LL831: Cyc_Absyndump_dumpqvar( _temp878->name); Cyc_Absyndump_dumptvars(
_temp876); if( _temp874 !=  0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp874,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u));} goto _LL801; _LL833: Cyc_Absyndump_dumpqvar(
_temp880->name); goto _LL801; _LL801:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!=  0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumptunionfield,
fields, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!=  0){ Cyc_Absyndump_dump( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield,
fields, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct Cyc_Absyn_Structfield _temp888; struct Cyc_List_List*
_temp889; struct Cyc_Absyn_Exp* _temp891; void* _temp893; struct Cyc_Absyn_Tqual
_temp895; struct _tagged_arr* _temp897; struct Cyc_Absyn_Structfield* _temp886=(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd;
_temp888=* _temp886; _LL898: _temp897= _temp888.name; goto _LL896; _LL896:
_temp895= _temp888.tq; goto _LL894; _LL894: _temp893=( void*) _temp888.type;
goto _LL892; _LL892: _temp891= _temp888.width; goto _LL890; _LL890: _temp889=
_temp888.attributes; goto _LL887; _LL887:(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tagged_arr*), struct _tagged_arr*)) Cyc_Absyndump_dumptqtd)(
_temp895, _temp893, Cyc_Absyndump_dump_str, _temp897); Cyc_Absyndump_dumpatts(
_temp889); if( _temp891 !=  0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp891));} Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar(
td->name); Cyc_Absyndump_dumptvars( td->tvs);} static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp899=(* pr).f1; _LL901: if( _temp899 == ( void*)
Cyc_Absyn_Unused_att){ goto _LL902;} else{ goto _LL903;} _LL903: if( _temp899 == (
void*) Cyc_Absyn_Stdcall_att){ goto _LL904;} else{ goto _LL905;} _LL905: if(
_temp899 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL906;} else{ goto _LL907;}
_LL907: if( _temp899 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL908;} else{
goto _LL909;} _LL909: goto _LL910; _LL902: goto _LL900; _LL904: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); goto _LL900; _LL906: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); goto _LL900; _LL908: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); goto _LL900; _LL910: goto
_LL900; _LL900:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for(
0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->name);
if((( struct Cyc_List_List*) _check_null( vds))->tl !=  0){ Cyc_Absyndump_dump_char((
int)',');}}} void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void*
_temp911=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp937; struct Cyc_Absyn_Structdecl*
_temp939; struct Cyc_Absyn_Uniondecl* _temp941; struct Cyc_Absyn_Vardecl*
_temp943; struct Cyc_Absyn_Vardecl _temp945; struct Cyc_List_List* _temp946;
struct Cyc_Absyn_Exp* _temp948; void* _temp950; struct Cyc_Absyn_Tqual _temp952;
struct _tuple0* _temp954; void* _temp956; struct Cyc_Absyn_Tuniondecl* _temp958;
struct Cyc_Absyn_Tuniondecl _temp960; int _temp961; struct Cyc_Core_Opt*
_temp963; struct Cyc_List_List* _temp965; struct _tuple0* _temp967; void*
_temp969; struct Cyc_Absyn_Enumdecl* _temp971; struct Cyc_Absyn_Enumdecl
_temp973; struct Cyc_Core_Opt* _temp974; struct _tuple0* _temp976; void*
_temp978; struct Cyc_Absyn_Exp* _temp980; struct Cyc_Absyn_Pat* _temp982; struct
Cyc_List_List* _temp984; struct Cyc_Absyn_Typedefdecl* _temp986; struct Cyc_List_List*
_temp988; struct _tagged_arr* _temp990; struct Cyc_List_List* _temp992; struct
_tuple0* _temp994; struct Cyc_List_List* _temp996; _LL913: if(*(( int*) _temp911)
==  Cyc_Absyn_Fn_d){ _LL938: _temp937=(( struct Cyc_Absyn_Fn_d_struct*) _temp911)->f1;
goto _LL914;} else{ goto _LL915;} _LL915: if(*(( int*) _temp911) ==  Cyc_Absyn_Struct_d){
_LL940: _temp939=(( struct Cyc_Absyn_Struct_d_struct*) _temp911)->f1; goto
_LL916;} else{ goto _LL917;} _LL917: if(*(( int*) _temp911) ==  Cyc_Absyn_Union_d){
_LL942: _temp941=(( struct Cyc_Absyn_Union_d_struct*) _temp911)->f1; goto _LL918;}
else{ goto _LL919;} _LL919: if(*(( int*) _temp911) ==  Cyc_Absyn_Var_d){ _LL944:
_temp943=(( struct Cyc_Absyn_Var_d_struct*) _temp911)->f1; _temp945=* _temp943;
_LL957: _temp956=( void*) _temp945.sc; goto _LL955; _LL955: _temp954= _temp945.name;
goto _LL953; _LL953: _temp952= _temp945.tq; goto _LL951; _LL951: _temp950=( void*)
_temp945.type; goto _LL949; _LL949: _temp948= _temp945.initializer; goto _LL947;
_LL947: _temp946= _temp945.attributes; goto _LL920;} else{ goto _LL921;} _LL921:
if(*(( int*) _temp911) ==  Cyc_Absyn_Tunion_d){ _LL959: _temp958=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp911)->f1; _temp960=* _temp958; _LL970: _temp969=( void*) _temp960.sc; goto
_LL968; _LL968: _temp967= _temp960.name; goto _LL966; _LL966: _temp965= _temp960.tvs;
goto _LL964; _LL964: _temp963= _temp960.fields; goto _LL962; _LL962: _temp961=
_temp960.is_xtunion; goto _LL922;} else{ goto _LL923;} _LL923: if(*(( int*)
_temp911) ==  Cyc_Absyn_Enum_d){ _LL972: _temp971=(( struct Cyc_Absyn_Enum_d_struct*)
_temp911)->f1; _temp973=* _temp971; _LL979: _temp978=( void*) _temp973.sc; goto
_LL977; _LL977: _temp976= _temp973.name; goto _LL975; _LL975: _temp974= _temp973.fields;
goto _LL924;} else{ goto _LL925;} _LL925: if(*(( int*) _temp911) ==  Cyc_Absyn_Let_d){
_LL983: _temp982=(( struct Cyc_Absyn_Let_d_struct*) _temp911)->f1; goto _LL981;
_LL981: _temp980=(( struct Cyc_Absyn_Let_d_struct*) _temp911)->f4; goto _LL926;}
else{ goto _LL927;} _LL927: if(*(( int*) _temp911) ==  Cyc_Absyn_Letv_d){ _LL985:
_temp984=(( struct Cyc_Absyn_Letv_d_struct*) _temp911)->f1; goto _LL928;} else{
goto _LL929;} _LL929: if(*(( int*) _temp911) ==  Cyc_Absyn_Typedef_d){ _LL987:
_temp986=(( struct Cyc_Absyn_Typedef_d_struct*) _temp911)->f1; goto _LL930;}
else{ goto _LL931;} _LL931: if(*(( int*) _temp911) ==  Cyc_Absyn_Namespace_d){
_LL991: _temp990=(( struct Cyc_Absyn_Namespace_d_struct*) _temp911)->f1; goto
_LL989; _LL989: _temp988=(( struct Cyc_Absyn_Namespace_d_struct*) _temp911)->f2;
goto _LL932;} else{ goto _LL933;} _LL933: if(*(( int*) _temp911) ==  Cyc_Absyn_Using_d){
_LL995: _temp994=(( struct Cyc_Absyn_Using_d_struct*) _temp911)->f1; goto _LL993;
_LL993: _temp992=(( struct Cyc_Absyn_Using_d_struct*) _temp911)->f2; goto _LL934;}
else{ goto _LL935;} _LL935: if(*(( int*) _temp911) ==  Cyc_Absyn_ExternC_d){
_LL997: _temp996=(( struct Cyc_Absyn_ExternC_d_struct*) _temp911)->f1; goto
_LL936;} else{ goto _LL912;} _LL914: if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts(
_temp937->attributes);} if( _temp937->is_inline){ if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump(
_tag_arr("__inline", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("inline", sizeof( unsigned char), 7u));}} Cyc_Absyndump_dumpscope((
void*) _temp937->sc);{ void* t=( void*)({ struct Cyc_Absyn_FnType_struct*
_temp999=( struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp999[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1000; _temp1000.tag= Cyc_Absyn_FnType;
_temp1000.f1=({ struct Cyc_Absyn_FnInfo _temp1001; _temp1001.tvars= _temp937->tvs;
_temp1001.effect= _temp937->effect; _temp1001.ret_typ=( void*)(( void*) _temp937->ret_type);
_temp1001.args=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp937->args);
_temp1001.c_varargs= _temp937->c_varargs; _temp1001.cyc_varargs= _temp937->cyc_varargs;
_temp1001.rgn_po= _temp937->rgn_po; _temp1001.attributes= 0; _temp1001;});
_temp1000;}); _temp999;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp998; _temp998.q_const= 0; _temp998.q_volatile= 0;
_temp998.q_restrict= 0; _temp998;}), t, Cyc_Absyndump_to_VC? Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar, _temp937); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp937->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL912;} _LL916: Cyc_Absyndump_dumpscope((
void*) _temp939->sc); Cyc_Absyndump_dump( _tag_arr("struct", sizeof(
unsigned char), 7u)); if( _temp939->name !=  0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp939->name))->v);} Cyc_Absyndump_dumptvars(
_temp939->tvs); if( _temp939->fields ==  0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp939->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp939->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL912;
_LL918: Cyc_Absyndump_dumpscope(( void*) _temp941->sc); Cyc_Absyndump_dump(
_tag_arr("union", sizeof( unsigned char), 6u)); if( _temp941->name !=  0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp941->name))->v);} Cyc_Absyndump_dumptvars(
_temp941->tvs); if( _temp941->fields ==  0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp941->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp941->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL912;
_LL920: if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts( _temp946); Cyc_Absyndump_dumpscope(
_temp956);{ struct Cyc_List_List* _temp1004; void* _temp1006; struct Cyc_Absyn_Tqual
_temp1008; struct _tuple4 _temp1002= Cyc_Absynpp_to_tms( _temp952, _temp950);
_LL1009: _temp1008= _temp1002.f1; goto _LL1007; _LL1007: _temp1006= _temp1002.f2;
goto _LL1005; _LL1005: _temp1004= _temp1002.f3; goto _LL1003; _LL1003: { void*
call_conv=( void*) Cyc_Absyn_Unused_att;{ struct Cyc_List_List* tms2= _temp1004;
for( 0; tms2 !=  0; tms2=(( struct Cyc_List_List*) _check_null( tms2))->tl){
void* _temp1010=( void*)(( struct Cyc_List_List*) _check_null( tms2))->hd;
struct Cyc_List_List* _temp1016; _LL1012: if(( unsigned int) _temp1010 >  1u?*((
int*) _temp1010) ==  Cyc_Absyn_Attributes_mod: 0){ _LL1017: _temp1016=(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp1010)->f2; goto _LL1013;} else{ goto
_LL1014;} _LL1014: goto _LL1015; _LL1013: for( 0; _temp1016 !=  0; _temp1016=((
struct Cyc_List_List*) _check_null( _temp1016))->tl){ void* _temp1018=( void*)((
struct Cyc_List_List*) _check_null( _temp1016))->hd; _LL1020: if( _temp1018 == (
void*) Cyc_Absyn_Stdcall_att){ goto _LL1021;} else{ goto _LL1022;} _LL1022: if(
_temp1018 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL1023;} else{ goto _LL1024;}
_LL1024: if( _temp1018 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL1025;} else{
goto _LL1026;} _LL1026: goto _LL1027; _LL1021: call_conv=( void*) Cyc_Absyn_Stdcall_att;
goto _LL1019; _LL1023: call_conv=( void*) Cyc_Absyn_Cdecl_att; goto _LL1019;
_LL1025: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1019; _LL1027: goto
_LL1019; _LL1019:;} goto _LL1011; _LL1015: goto _LL1011; _LL1011:;}} Cyc_Absyndump_dumptq(
_temp1008); Cyc_Absyndump_dumpntyp( _temp1006);{ struct _tuple10 _temp1028=({
struct _tuple10 _temp1029; _temp1029.f1= call_conv; _temp1029.f2= _temp954;
_temp1029;});(( void(*)( struct Cyc_List_List* tms, void(* f)( struct _tuple10*),
struct _tuple10* a)) Cyc_Absyndump_dumptms)( Cyc_List_imp_rev( _temp1004), Cyc_Absyndump_dump_callconv_qvar,&
_temp1028);}}}} else{ Cyc_Absyndump_dumpscope( _temp956);(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tuple0*), struct _tuple0*)) Cyc_Absyndump_dumptqtd)(
_temp952, _temp950, Cyc_Absyndump_dumpqvar, _temp954); Cyc_Absyndump_dumpatts(
_temp946);} if( _temp948 !=  0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp948));} Cyc_Absyndump_dump_semi(); goto
_LL912; _LL922: Cyc_Absyndump_dumpscope( _temp969); if( _temp961){ Cyc_Absyndump_dump(
_tag_arr("xtunion ", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("tunion ", sizeof( unsigned char), 8u));} Cyc_Absyndump_dumpqvar(
_temp967); Cyc_Absyndump_dumptvars( _temp965); if( _temp963 ==  0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumptunionfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp963))->v); Cyc_Absyndump_dump_nospace(
_tag_arr("};", sizeof( unsigned char), 3u));} goto _LL912; _LL924: Cyc_Absyndump_dumpscope(
_temp978); Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u));
Cyc_Absyndump_dumpqvar( _temp976); if( _temp974 ==  0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp974))->v); Cyc_Absyndump_dump_nospace(
_tag_arr("};", sizeof( unsigned char), 3u));} return; _LL926: Cyc_Absyndump_dump(
_tag_arr("let", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumppat( _temp982);
Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp( _temp980); Cyc_Absyndump_dump_semi();
goto _LL912; _LL928: Cyc_Absyndump_dump( _tag_arr("let ", sizeof( unsigned char),
5u)); Cyc_Absyndump_dumpids( _temp984); Cyc_Absyndump_dump_semi(); goto _LL912;
_LL930: if( ! Cyc_Absyndump_expand_typedefs){ Cyc_Absyndump_dump( _tag_arr("typedef",
sizeof( unsigned char), 8u));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1030; _temp1030.q_const= 0; _temp1030.q_volatile= 0;
_temp1030.q_restrict= 0; _temp1030;}),( void*) _temp986->defn, Cyc_Absyndump_dumptypedefname,
_temp986); Cyc_Absyndump_dump_semi();} goto _LL912; _LL932: Cyc_Absyndump_dump(
_tag_arr("namespace", sizeof( unsigned char), 10u)); Cyc_Absyndump_dump_str(
_temp990); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp988 !=  0; _temp988=((
struct Cyc_List_List*) _check_null( _temp988))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp988))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL912; _LL934: Cyc_Absyndump_dump(
_tag_arr("using", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar( _temp994);
Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp992 !=  0; _temp992=(( struct
Cyc_List_List*) _check_null( _temp992))->tl){ Cyc_Absyndump_dumpdecl(( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp992))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL912; _LL936: Cyc_Absyndump_dump( _tag_arr("extern \"C\" {",
sizeof( unsigned char), 13u)); for( 0; _temp996 !=  0; _temp996=(( struct Cyc_List_List*)
_check_null( _temp996))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp996))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL912; _LL912:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i !=  1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms ==  0){ f( a); return;}{ void* _temp1031=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1049;
void* _temp1051; void* _temp1053; struct Cyc_Absyn_Exp* _temp1055; struct Cyc_Absyn_Tqual
_temp1057; void* _temp1059; void* _temp1061; struct Cyc_Absyn_Exp* _temp1063;
struct Cyc_Absyn_Tqual _temp1065; void* _temp1067; void* _temp1069; struct Cyc_Absyn_Tqual
_temp1071; void* _temp1073; struct Cyc_Absyn_Tvar* _temp1075; void* _temp1077;
struct Cyc_Absyn_Exp* _temp1079; struct Cyc_Absyn_Tqual _temp1081; void*
_temp1083; struct Cyc_Absyn_Tvar* _temp1085; void* _temp1087; struct Cyc_Absyn_Exp*
_temp1089; struct Cyc_Absyn_Tqual _temp1091; void* _temp1093; struct Cyc_Absyn_Tvar*
_temp1095; void* _temp1097; _LL1033: if(( unsigned int) _temp1031 >  1u?*(( int*)
_temp1031) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1054: _temp1053=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp1031)->f1; if(( unsigned int) _temp1053 >  1u?*((
int*) _temp1053) ==  Cyc_Absyn_Nullable_ps: 0){ _LL1056: _temp1055=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1053)->f1; goto _LL1052;} else{ goto _LL1035;} _LL1052: _temp1051=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1031)->f2; if( _temp1051 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL1050;} else{ goto _LL1035;} _LL1050: _temp1049=((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1031)->f3; goto _LL1034;} else{ goto
_LL1035;} _LL1035: if(( unsigned int) _temp1031 >  1u?*(( int*) _temp1031) == 
Cyc_Absyn_Pointer_mod: 0){ _LL1062: _temp1061=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f1; if(( unsigned int) _temp1061 >  1u?*(( int*) _temp1061) ==  Cyc_Absyn_NonNullable_ps:
0){ _LL1064: _temp1063=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1061)->f1;
goto _LL1060;} else{ goto _LL1037;} _LL1060: _temp1059=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f2; if( _temp1059 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1058;}
else{ goto _LL1037;} _LL1058: _temp1057=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f3; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1070:
_temp1069=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1031)->f1; if(
_temp1069 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1068;} else{ goto
_LL1039;} _LL1068: _temp1067=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f2; if( _temp1067 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1066;}
else{ goto _LL1039;} _LL1066: _temp1065=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f3; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1078:
_temp1077=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1031)->f1; if((
unsigned int) _temp1077 >  1u?*(( int*) _temp1077) ==  Cyc_Absyn_Nullable_ps: 0){
_LL1080: _temp1079=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1077)->f1; goto
_LL1074;} else{ goto _LL1041;} _LL1074: _temp1073=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f2; if(( unsigned int) _temp1073 >  4u?*(( int*) _temp1073) ==  Cyc_Absyn_VarType:
0){ _LL1076: _temp1075=(( struct Cyc_Absyn_VarType_struct*) _temp1073)->f1; goto
_LL1072;} else{ goto _LL1041;} _LL1072: _temp1071=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f3; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1088:
_temp1087=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1031)->f1; if((
unsigned int) _temp1087 >  1u?*(( int*) _temp1087) ==  Cyc_Absyn_NonNullable_ps:
0){ _LL1090: _temp1089=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1087)->f1;
goto _LL1084;} else{ goto _LL1043;} _LL1084: _temp1083=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f2; if(( unsigned int) _temp1083 >  4u?*(( int*) _temp1083) ==  Cyc_Absyn_VarType:
0){ _LL1086: _temp1085=(( struct Cyc_Absyn_VarType_struct*) _temp1083)->f1; goto
_LL1082;} else{ goto _LL1043;} _LL1082: _temp1081=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f3; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1098:
_temp1097=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1031)->f1; if(
_temp1097 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1094;} else{ goto
_LL1045;} _LL1094: _temp1093=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f2; if(( unsigned int) _temp1093 >  4u?*(( int*) _temp1093) ==  Cyc_Absyn_VarType:
0){ _LL1096: _temp1095=(( struct Cyc_Absyn_VarType_struct*) _temp1093)->f1; goto
_LL1092;} else{ goto _LL1045;} _LL1092: _temp1091=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1031)->f3; goto _LL1044;} else{ goto _LL1045;} _LL1045: if(( unsigned int)
_temp1031 >  1u?*(( int*) _temp1031) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL1046;}
else{ goto _LL1047;} _LL1047: goto _LL1048; _LL1034: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1055); Cyc_Absyndump_dumptms(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1036: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1063); Cyc_Absyndump_dumptms(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1038: Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumptms((( struct Cyc_List_List*) _check_null( tms))->tl,
f, a); return; _LL1040: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1079); Cyc_Absyndump_dump_str( _temp1075->name); Cyc_Absyndump_dumptms(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1042: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1089); Cyc_Absyndump_dump_str(
_temp1085->name); Cyc_Absyndump_dumptms((( struct Cyc_List_List*) _check_null(
tms))->tl, f, a); return; _LL1044: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1095->name); Cyc_Absyndump_dumptms((( struct Cyc_List_List*) _check_null(
tms))->tl, f, a); return; _LL1046:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1099=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1099[ 0]=({ struct Cyc_Core_Impossible_struct _temp1100; _temp1100.tag= Cyc_Core_Impossible;
_temp1100.f1= _tag_arr("dumptms: bad Pointer_mod", sizeof( unsigned char), 25u);
_temp1100;}); _temp1099;})); _LL1048: { int next_is_pointer= 0; if((( struct Cyc_List_List*)
_check_null( tms))->tl !=  0){ void* _temp1101=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd; _LL1103: if((
unsigned int) _temp1101 >  1u?*(( int*) _temp1101) ==  Cyc_Absyn_Pointer_mod: 0){
goto _LL1104;} else{ goto _LL1105;} _LL1105: goto _LL1106; _LL1104:
next_is_pointer= 1; goto _LL1102; _LL1106: goto _LL1102; _LL1102:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');} Cyc_Absyndump_dumptms(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1107=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1123; void* _temp1125; struct Cyc_List_List*
_temp1127; struct Cyc_Core_Opt* _temp1129; struct Cyc_Absyn_VarargInfo*
_temp1131; int _temp1133; struct Cyc_List_List* _temp1135; void* _temp1137;
struct Cyc_Position_Segment* _temp1139; struct Cyc_List_List* _temp1141; int
_temp1143; struct Cyc_Position_Segment* _temp1145; struct Cyc_List_List*
_temp1147; struct Cyc_List_List* _temp1149; void* _temp1151; void* _temp1153;
_LL1109: if( _temp1107 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL1110;} else{
goto _LL1111;} _LL1111: if(( unsigned int) _temp1107 >  1u?*(( int*) _temp1107)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL1124: _temp1123=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1107)->f1; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(( unsigned int)
_temp1107 >  1u?*(( int*) _temp1107) ==  Cyc_Absyn_Function_mod: 0){ _LL1126:
_temp1125=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1107)->f1; if(*((
int*) _temp1125) ==  Cyc_Absyn_WithTypes){ _LL1136: _temp1135=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1125)->f1; goto _LL1134; _LL1134: _temp1133=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1125)->f2; goto _LL1132; _LL1132: _temp1131=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1125)->f3; goto _LL1130; _LL1130: _temp1129=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1125)->f4; goto _LL1128; _LL1128: _temp1127=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1125)->f5; goto _LL1114;} else{ goto _LL1115;}} else{ goto _LL1115;}
_LL1115: if(( unsigned int) _temp1107 >  1u?*(( int*) _temp1107) ==  Cyc_Absyn_Function_mod:
0){ _LL1138: _temp1137=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1107)->f1; if(*(( int*) _temp1137) ==  Cyc_Absyn_NoTypes){ _LL1142:
_temp1141=(( struct Cyc_Absyn_NoTypes_struct*) _temp1137)->f1; goto _LL1140;
_LL1140: _temp1139=(( struct Cyc_Absyn_NoTypes_struct*) _temp1137)->f2; goto
_LL1116;} else{ goto _LL1117;}} else{ goto _LL1117;} _LL1117: if(( unsigned int)
_temp1107 >  1u?*(( int*) _temp1107) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL1148:
_temp1147=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1107)->f1; goto
_LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1107)->f2; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1107)->f3; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(( unsigned int)
_temp1107 >  1u?*(( int*) _temp1107) ==  Cyc_Absyn_Attributes_mod: 0){ _LL1150:
_temp1149=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1107)->f2; goto
_LL1120;} else{ goto _LL1121;} _LL1121: if(( unsigned int) _temp1107 >  1u?*((
int*) _temp1107) ==  Cyc_Absyn_Pointer_mod: 0){ _LL1154: _temp1153=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1107)->f1; goto _LL1152; _LL1152:
_temp1151=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1107)->f2; goto
_LL1122;} else{ goto _LL1108;} _LL1110: Cyc_Absyndump_dump( _tag_arr("[]",
sizeof( unsigned char), 3u)); goto _LL1108; _LL1112: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1123); Cyc_Absyndump_dump_char(( int)']');
goto _LL1108; _LL1114: Cyc_Absyndump_dumpfunargs( _temp1135, _temp1133,
_temp1131, _temp1129, _temp1127); goto _LL1108; _LL1116:(( void(*)( void(* f)(
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1141, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL1108;
_LL1118: if( _temp1143){ Cyc_Absyndump_dumpkindedtvars( _temp1147);} else{ Cyc_Absyndump_dumptvars(
_temp1147);} goto _LL1108; _LL1120: Cyc_Absyndump_dumpatts( _temp1149); goto
_LL1108; _LL1122:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1155=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1155[ 0]=({ struct Cyc_Core_Impossible_struct _temp1156; _temp1156.tag= Cyc_Core_Impossible;
_temp1156.f1= _tag_arr("dumptms", sizeof( unsigned char), 8u); _temp1156;});
_temp1155;})); _LL1108:;} return;} _LL1032:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1159; void* _temp1161; struct Cyc_Absyn_Tqual _temp1163; struct _tuple4
_temp1157= Cyc_Absynpp_to_tms( tq, t); _LL1164: _temp1163= _temp1157.f1; goto
_LL1162; _LL1162: _temp1161= _temp1157.f2; goto _LL1160; _LL1160: _temp1159=
_temp1157.f3; goto _LL1158; _LL1158: Cyc_Absyndump_dumptq( _temp1163); Cyc_Absyndump_dumpntyp(
_temp1161); Cyc_Absyndump_dumptms( Cyc_List_imp_rev( _temp1159), f, a);} void
Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct Cyc_Std___sFILE*
f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0; tdl !=  0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl(( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd);}({ void*
_temp1165[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof( unsigned char), 2u),
_tag_arr( _temp1165, sizeof( void*), 0u));});}
