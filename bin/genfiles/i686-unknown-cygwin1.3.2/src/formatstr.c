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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; extern int isalnum( int); extern int
isalpha( int); extern int isascii( int); extern int iscntrl( int); extern int
isdigit( int); extern int isgraph( int); extern int islower( int); extern int
isprint( int); extern int ispunct( int); extern int isspace( int); extern int
isupper( int); extern int isxdigit( int); extern int toascii( int); extern int
tolower( int); extern int toupper( int); extern int _tolower( int); extern int
_toupper( int); extern unsigned int Cyc_Std_strlen( struct _tagged_arr s);
extern struct _tagged_arr Cyc_Std_implode( struct Cyc_List_List* c); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_ShortPtr_sa= 0; struct
Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
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
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
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
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Gentyp_e=
19; struct Cyc_Absyn_Gentyp_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_Deref_e= 20; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_StructMember_e= 21; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 22; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
23; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 24; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
25; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 26; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
27; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
28; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 29; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 30;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_const_string_typ( void*
rgn); extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*, struct _tagged_arr, struct
Cyc_Position_Segment*); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s, int i); static struct Cyc_Core_Opt Cyc_Formatstr_rk={(
void*)(( void*) 3u)}; struct _tuple3{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6;
} ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_arr s, int i){ unsigned int _temp0= Cyc_Std_strlen( s); if( i <  0? 1: i
>=  _temp0){ return 0;}{ struct Cyc_List_List* _temp1= 0; unsigned char c=' ';
for( 0; i <  _temp0; i ++){ c=*(( const unsigned char*) _check_unknown_subscript(
s, sizeof( unsigned char), i)); switch( c){ case '+': _LL2: goto _LL3; case '-':
_LL3: goto _LL4; case ' ': _LL4: goto _LL5; case '#': _LL5: goto _LL6; case '0':
_LL6: _temp1=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp8->hd=( void*)(( int) c); _temp8->tl=
_temp1; _temp8;}); continue; default: _LL7: break;} break;} if( i >=  _temp0){
return 0;} _temp1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1);{ struct Cyc_List_List* _temp10= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == '*'){ _temp10=({
struct Cyc_List_List* _temp11=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp11->hd=( void*)(( int) c); _temp11->tl= _temp10; _temp11;});
i ++;} else{ for( 0; i <  _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp10=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp12->hd=( void*)(( int) c); _temp12->tl=
_temp10; _temp12;});} else{ break;}}} if( i >=  _temp0){ return 0;} _temp10=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp10);{
struct Cyc_List_List* _temp13= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == '.'){ _temp13=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)(( int) c); _temp14->tl= _temp13; _temp14;});
i ++; if( i >=  _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == '*'){ _temp13=({
struct Cyc_List_List* _temp15=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp15->hd=( void*)(( int) c); _temp15->tl= _temp13; _temp15;});
i ++;} else{ for( 0; i <  _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp13=({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp16->hd=( void*)(( int) c); _temp16->tl=
_temp13; _temp16;});} else{ break;}}}} if( i >=  _temp0){ return 0;} _temp13=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp13);{
struct Cyc_List_List* _temp17= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( c){ case 'h':
_LL18: _temp17=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp20->hd=( void*)(( int) c);
_temp20->tl= _temp17; _temp20;}); i ++; if( i >=  _temp0){ return 0;} c=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
if( c == 'h'){ _temp17=({ struct Cyc_List_List* _temp21=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp21->hd=( void*)(( int) c);
_temp21->tl= _temp17; _temp21;}); i ++;} break; case 'l': _LL19: _temp17=({
struct Cyc_List_List* _temp23=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp23->hd=( void*)(( int) c); _temp23->tl= _temp17; _temp23;});
i ++; if( i >=  _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == 'l'){ _temp17=({
struct Cyc_List_List* _temp24=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp24->hd=( void*)(( int) c); _temp24->tl= _temp17; _temp24;});
i ++;} break; case 'j': _LL22: goto _LL25; case 'z': _LL25: goto _LL26; case 't':
_LL26: goto _LL27; case 'L': _LL27: _temp17=({ struct Cyc_List_List* _temp29=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp29->hd=(
void*)(( int) c); _temp29->tl= _temp17; _temp29;}); i ++; break; default: _LL28:
break;} if( i >=  _temp0){ return 0;} _temp17=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp17); c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( c){ case 'd':
_LL31: break; case 'i': _LL32: break; case 'o': _LL33: break; case 'u': _LL34:
break; case 'x': _LL35: break; case 'X': _LL36: break; case 'f': _LL37: break;
case 'F': _LL38: break; case 'e': _LL39: break; case 'E': _LL40: break; case 'g':
_LL41: break; case 'G': _LL42: break; case 'a': _LL43: break; case 'A': _LL44:
break; case 'c': _LL45: break; case 's': _LL46: break; case 'p': _LL47: break;
case 'n': _LL48: break; case '%': _LL49: break; default: _LL50: return 0;}
return({ struct Cyc_Core_Opt* _temp52=( struct Cyc_Core_Opt*) _cycalloc( sizeof(
struct Cyc_Core_Opt)); _temp52->v=( void*)({ struct _tuple3* _temp53=( struct
_tuple3*) _cycalloc( sizeof( struct _tuple3)); _temp53->f1= _temp1; _temp53->f2=
_temp10; _temp53->f3= _temp13; _temp53->f4= _temp17; _temp53->f5= c; _temp53->f6=
i +  1; _temp53;}); _temp52;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ unsigned int _temp54= Cyc_Std_strlen( s); struct Cyc_List_List* _temp55= 0;
int i; for( i= 0; i <  _temp54; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) != '%'){ continue;}{
struct Cyc_Core_Opt* _temp56= Cyc_Formatstr_parse_conversionspecification( s, i
+  1); if( _temp56 ==  0){({ void* _temp57[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bad format string", sizeof( unsigned char), 18u), _tag_arr( _temp57,
sizeof( void*), 0u));}); return 0;}{ struct _tuple3 _temp60; int _temp61;
unsigned char _temp63; struct Cyc_List_List* _temp65; struct Cyc_List_List*
_temp67; struct Cyc_List_List* _temp69; struct Cyc_List_List* _temp71; struct
_tuple3* _temp58=( struct _tuple3*)(( struct Cyc_Core_Opt*) _check_null( _temp56))->v;
_temp60=* _temp58; _LL72: _temp71= _temp60.f1; goto _LL70; _LL70: _temp69=
_temp60.f2; goto _LL68; _LL68: _temp67= _temp60.f3; goto _LL66; _LL66: _temp65=
_temp60.f4; goto _LL64; _LL64: _temp63= _temp60.f5; goto _LL62; _LL62: _temp61=
_temp60.f6; goto _LL59; _LL59: i= _temp61 -  1;{ struct Cyc_List_List* _temp73=
_temp65; struct Cyc_List_List _temp80; struct Cyc_List_List* _temp81; int
_temp83; _LL75: if( _temp73 ==  0){ goto _LL77;} else{ _temp80=* _temp73; _LL84:
_temp83=( int) _temp80.hd; goto _LL82; _LL82: _temp81= _temp80.tl; if( _temp81
==  0){ goto _LL79;} else{ goto _LL77;}} _LL79: if(( _temp83 == 'j'? 1: _temp83
== 'z')? 1: _temp83 == 't'){ goto _LL76;} else{ goto _LL77;} _LL77: goto _LL78;
_LL76:({ struct Cyc_Std_Int_pa_struct _temp86; _temp86.tag= Cyc_Std_Int_pa;
_temp86.f1=( int)(( unsigned int) _temp83);{ void* _temp85[ 1u]={& _temp86}; Cyc_Tcutil_terr(
loc, _tag_arr("length modifier '%c' is not supported", sizeof( unsigned char),
38u), _tag_arr( _temp85, sizeof( void*), 1u));}}); return 0; _LL78: goto _LL74;
_LL74:;}{ struct Cyc_List_List* _temp87= _temp69; struct Cyc_List_List _temp94;
struct Cyc_List_List* _temp95; int _temp97; _LL89: if( _temp87 ==  0){ goto
_LL91;} else{ _temp94=* _temp87; _LL98: _temp97=( int) _temp94.hd; goto _LL96;
_LL96: _temp95= _temp94.tl; if( _temp95 ==  0){ goto _LL93;} else{ goto _LL91;}}
_LL93: if( _temp97 == '*'){ goto _LL90;} else{ goto _LL91;} _LL91: goto _LL92;
_LL90: _temp55=({ struct Cyc_List_List* _temp99=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp99->hd=( void*) Cyc_Absyn_sint_t;
_temp99->tl= _temp55; _temp99;}); goto _LL88; _LL92: goto _LL88; _LL88:;}{
struct Cyc_List_List* _temp100= _temp67; struct Cyc_List_List _temp107; struct
Cyc_List_List* _temp108; struct Cyc_List_List _temp110; struct Cyc_List_List*
_temp111; int _temp113; int _temp115; _LL102: if( _temp100 ==  0){ goto _LL104;}
else{ _temp107=* _temp100; _LL116: _temp115=( int) _temp107.hd; goto _LL109;
_LL109: _temp108= _temp107.tl; if( _temp108 ==  0){ goto _LL104;} else{ _temp110=*
_temp108; _LL114: _temp113=( int) _temp110.hd; goto _LL112; _LL112: _temp111=
_temp110.tl; if( _temp111 ==  0){ goto _LL106;} else{ goto _LL104;}}} _LL106:
if( _temp115 == '.'? _temp113 == '*': 0){ goto _LL103;} else{ goto _LL104;}
_LL104: goto _LL105; _LL103: _temp55=({ struct Cyc_List_List* _temp117=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp117->hd=( void*)
Cyc_Absyn_sint_t; _temp117->tl= _temp55; _temp117;}); goto _LL101; _LL105: goto
_LL101; _LL101:;}{ void* t; switch( _temp63){ case 'd': _LL118: goto _LL119;
case 'i': _LL119:{ struct Cyc_List_List* f= _temp71; for( 0; f !=  0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd == '#'){({ struct Cyc_Std_Int_pa_struct _temp122; _temp122.tag=
Cyc_Std_Int_pa; _temp122.f1=( int)(( unsigned int)(( int) _temp63));{ void*
_temp121[ 1u]={& _temp122}; Cyc_Tcutil_terr( loc, _tag_arr("flag '#' is not valid with %%%c",
sizeof( unsigned char), 32u), _tag_arr( _temp121, sizeof( void*), 1u));}});
return 0;}}}{ struct Cyc_List_List* _temp123= _temp65; struct Cyc_List_List
_temp136; struct Cyc_List_List* _temp137; int _temp139; struct Cyc_List_List
_temp142; struct Cyc_List_List* _temp143; int _temp145; struct Cyc_List_List
_temp148; struct Cyc_List_List* _temp149; struct Cyc_List_List _temp151; struct
Cyc_List_List* _temp152; int _temp154; int _temp156; _LL125: if( _temp123 ==  0){
goto _LL126;} else{ goto _LL127;} _LL127: if( _temp123 ==  0){ goto _LL129;}
else{ _temp136=* _temp123; _LL140: _temp139=( int) _temp136.hd; goto _LL138;
_LL138: _temp137= _temp136.tl; if( _temp137 ==  0){ goto _LL135;} else{ goto
_LL129;}} _LL135: if( _temp139 == 'l'){ goto _LL128;} else{ goto _LL129;} _LL129:
if( _temp123 ==  0){ goto _LL131;} else{ _temp142=* _temp123; _LL146: _temp145=(
int) _temp142.hd; goto _LL144; _LL144: _temp143= _temp142.tl; if( _temp143 ==  0){
goto _LL141;} else{ goto _LL131;}} _LL141: if( _temp145 == 'h'){ goto _LL130;}
else{ goto _LL131;} _LL131: if( _temp123 ==  0){ goto _LL133;} else{ _temp148=*
_temp123; _LL157: _temp156=( int) _temp148.hd; goto _LL150; _LL150: _temp149=
_temp148.tl; if( _temp149 ==  0){ goto _LL133;} else{ _temp151=* _temp149;
_LL155: _temp154=( int) _temp151.hd; goto _LL153; _LL153: _temp152= _temp151.tl;
if( _temp152 ==  0){ goto _LL147;} else{ goto _LL133;}}} _LL147: if( _temp156 == 'h'?
_temp154 == 'h': 0){ goto _LL132;} else{ goto _LL133;} _LL133: goto _LL134;
_LL126: t= Cyc_Absyn_sint_t; goto _LL124; _LL128: t= Cyc_Absyn_slong_t; goto
_LL124; _LL130: t= Cyc_Absyn_sshort_t; goto _LL124; _LL132: t= Cyc_Absyn_schar_t;
goto _LL124; _LL134:({ struct Cyc_Std_Int_pa_struct _temp160; _temp160.tag= Cyc_Std_Int_pa;
_temp160.f1=( int)(( unsigned int)(( int) _temp63));{ struct Cyc_Std_String_pa_struct
_temp159; _temp159.tag= Cyc_Std_String_pa; _temp159.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp65);{ void* _temp158[ 2u]={& _temp159,& _temp160}; Cyc_Tcutil_terr( loc,
_tag_arr("length modifier '%s' is not allowed with %%%c", sizeof( unsigned char),
46u), _tag_arr( _temp158, sizeof( void*), 2u));}}}); return 0; _LL124:;} _temp55=({
struct Cyc_List_List* _temp161=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp161->hd=( void*) t; _temp161->tl= _temp55; _temp161;});
break; case 'u': _LL120:{ struct Cyc_List_List* f= _temp71; for( 0; f !=  0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd == '#'){({ void* _temp163[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("Flag '#' not valid with %%u", sizeof( unsigned char), 28u), _tag_arr(
_temp163, sizeof( void*), 0u));}); return 0;}}} goto _LL162; case 'o': _LL162:
goto _LL164; case 'x': _LL164: goto _LL165; case 'X': _LL165:{ struct Cyc_List_List*
_temp167= _temp65; struct Cyc_List_List _temp180; struct Cyc_List_List* _temp181;
int _temp183; struct Cyc_List_List _temp186; struct Cyc_List_List* _temp187; int
_temp189; struct Cyc_List_List _temp192; struct Cyc_List_List* _temp193; struct
Cyc_List_List _temp195; struct Cyc_List_List* _temp196; int _temp198; int
_temp200; _LL169: if( _temp167 ==  0){ goto _LL170;} else{ goto _LL171;} _LL171:
if( _temp167 ==  0){ goto _LL173;} else{ _temp180=* _temp167; _LL184: _temp183=(
int) _temp180.hd; goto _LL182; _LL182: _temp181= _temp180.tl; if( _temp181 ==  0){
goto _LL179;} else{ goto _LL173;}} _LL179: if( _temp183 == 'l'){ goto _LL172;}
else{ goto _LL173;} _LL173: if( _temp167 ==  0){ goto _LL175;} else{ _temp186=*
_temp167; _LL190: _temp189=( int) _temp186.hd; goto _LL188; _LL188: _temp187=
_temp186.tl; if( _temp187 ==  0){ goto _LL185;} else{ goto _LL175;}} _LL185: if(
_temp189 == 'h'){ goto _LL174;} else{ goto _LL175;} _LL175: if( _temp167 ==  0){
goto _LL177;} else{ _temp192=* _temp167; _LL201: _temp200=( int) _temp192.hd;
goto _LL194; _LL194: _temp193= _temp192.tl; if( _temp193 ==  0){ goto _LL177;}
else{ _temp195=* _temp193; _LL199: _temp198=( int) _temp195.hd; goto _LL197;
_LL197: _temp196= _temp195.tl; if( _temp196 ==  0){ goto _LL191;} else{ goto
_LL177;}}} _LL191: if( _temp200 == 'h'? _temp198 == 'h': 0){ goto _LL176;} else{
goto _LL177;} _LL177: goto _LL178; _LL170: t= Cyc_Absyn_uint_t; goto _LL168;
_LL172: t= Cyc_Absyn_ulong_t; goto _LL168; _LL174: t= Cyc_Absyn_ushort_t; goto
_LL168; _LL176: t= Cyc_Absyn_uchar_t; goto _LL168; _LL178:({ struct Cyc_Std_Int_pa_struct
_temp204; _temp204.tag= Cyc_Std_Int_pa; _temp204.f1=( int)(( unsigned int)(( int)
_temp63));{ struct Cyc_Std_String_pa_struct _temp203; _temp203.tag= Cyc_Std_String_pa;
_temp203.f1=( struct _tagged_arr) Cyc_Std_implode( _temp65);{ void* _temp202[ 2u]={&
_temp203,& _temp204}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u), _tag_arr( _temp202, sizeof( void*), 2u));}}});
return 0; _LL168:;} _temp55=({ struct Cyc_List_List* _temp205=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp205->hd=( void*) t; _temp205->tl=
_temp55; _temp205;}); break; case 'f': _LL166: goto _LL206; case 'F': _LL206:
goto _LL207; case 'e': _LL207: goto _LL208; case 'E': _LL208: goto _LL209; case
'g': _LL209: goto _LL210; case 'G': _LL210: goto _LL211; case 'a': _LL211: goto
_LL212; case 'A': _LL212: if( _temp65 !=  0){({ struct Cyc_Std_Int_pa_struct
_temp215; _temp215.tag= Cyc_Std_Int_pa; _temp215.f1=( int)(( unsigned int)(( int)
_temp63));{ void* _temp214[ 1u]={& _temp215}; Cyc_Tcutil_terr( loc, _tag_arr("length modifiers are not allowed with %%%c",
sizeof( unsigned char), 43u), _tag_arr( _temp214, sizeof( void*), 1u));}});
return 0;} _temp55=({ struct Cyc_List_List* _temp216=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp216->hd=( void*) Cyc_Absyn_double_t;
_temp216->tl= _temp55; _temp216;}); break; case 'c': _LL213:{ struct Cyc_List_List*
f= _temp71; for( 0; f !=  0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd == '#'? 1:( int)((
struct Cyc_List_List*) _check_null( f))->hd == '0'){({ struct Cyc_Std_Int_pa_struct
_temp219; _temp219.tag= Cyc_Std_Int_pa; _temp219.f1=( int)(( unsigned int)(( int)((
struct Cyc_List_List*) _check_null( f))->hd));{ void* _temp218[ 1u]={& _temp219};
Cyc_Tcutil_terr( loc, _tag_arr("flag '%c' not allowed with %%c", sizeof(
unsigned char), 31u), _tag_arr( _temp218, sizeof( void*), 1u));}}); return 0;}}}
if( _temp65 !=  0){({ struct Cyc_Std_String_pa_struct _temp221; _temp221.tag=
Cyc_Std_String_pa; _temp221.f1=( struct _tagged_arr) Cyc_Std_implode( _temp65);{
void* _temp220[ 1u]={& _temp221}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%c",
sizeof( unsigned char), 42u), _tag_arr( _temp220, sizeof( void*), 1u));}});
return 0;} if( _temp67 !=  0){({ struct Cyc_Std_String_pa_struct _temp223;
_temp223.tag= Cyc_Std_String_pa; _temp223.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp67);{ void* _temp222[ 1u]={& _temp223}; Cyc_Tcutil_terr( loc, _tag_arr("precision '%s' not allowed with %%c",
sizeof( unsigned char), 36u), _tag_arr( _temp222, sizeof( void*), 1u));}});
return 0;} _temp55=({ struct Cyc_List_List* _temp224=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp224->hd=( void*) Cyc_Absyn_sint_t;
_temp224->tl= _temp55; _temp224;}); break; case 's': _LL217:{ struct Cyc_List_List*
f= _temp71; for( 0; f !=  0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd != '-'){({ void*
_temp226[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("a flag not allowed with %%s",
sizeof( unsigned char), 28u), _tag_arr( _temp226, sizeof( void*), 0u));});
return 0;}}} if( _temp65 !=  0){({ void* _temp227[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("length modifiers not allowed with %%s", sizeof( unsigned char), 38u),
_tag_arr( _temp227, sizeof( void*), 0u));}); return 0;} _temp55=({ struct Cyc_List_List*
_temp228=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp228->hd=( void*) Cyc_Absyn_const_string_typ( Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Formatstr_rk,({ struct Cyc_Core_Opt* _temp229=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp229->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp229;}))); _temp228->tl= _temp55; _temp228;}); break; case 'p': _LL225:
_temp55=({ struct Cyc_List_List* _temp231=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp231->hd=( void*) Cyc_Absyn_uint_t; _temp231->tl=
_temp55; _temp231;}); break; case 'n': _LL230:{ struct Cyc_List_List* f= _temp71;
for( 0; f !=  0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)((
struct Cyc_List_List*) _check_null( f))->hd == '#'? 1:( int)(( struct Cyc_List_List*)
_check_null( f))->hd == '0'){({ struct Cyc_Std_Int_pa_struct _temp234; _temp234.tag=
Cyc_Std_Int_pa; _temp234.f1=( int)(( unsigned int)(( int)(( struct Cyc_List_List*)
_check_null( f))->hd));{ void* _temp233[ 1u]={& _temp234}; Cyc_Tcutil_terr( loc,
_tag_arr("flag '%c' not allowed with %%n", sizeof( unsigned char), 31u),
_tag_arr( _temp233, sizeof( void*), 1u));}}); return 0;}}} if( _temp67 !=  0){({
struct Cyc_Std_String_pa_struct _temp236; _temp236.tag= Cyc_Std_String_pa;
_temp236.f1=( struct _tagged_arr) Cyc_Std_implode( _temp67);{ void* _temp235[ 1u]={&
_temp236}; Cyc_Tcutil_terr( loc, _tag_arr("precision '%s' not allowed with %%n",
sizeof( unsigned char), 36u), _tag_arr( _temp235, sizeof( void*), 1u));}});
return 0;}{ struct Cyc_Core_Opt* _temp237=({ struct Cyc_Core_Opt* _temp276=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp276->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp276;});{ struct Cyc_List_List*
_temp238= _temp65; struct Cyc_List_List _temp251; struct Cyc_List_List* _temp252;
int _temp254; struct Cyc_List_List _temp257; struct Cyc_List_List* _temp258; int
_temp260; struct Cyc_List_List _temp263; struct Cyc_List_List* _temp264; struct
Cyc_List_List _temp266; struct Cyc_List_List* _temp267; int _temp269; int
_temp271; _LL240: if( _temp238 ==  0){ goto _LL241;} else{ goto _LL242;} _LL242:
if( _temp238 ==  0){ goto _LL244;} else{ _temp251=* _temp238; _LL255: _temp254=(
int) _temp251.hd; goto _LL253; _LL253: _temp252= _temp251.tl; if( _temp252 ==  0){
goto _LL250;} else{ goto _LL244;}} _LL250: if( _temp254 == 'l'){ goto _LL243;}
else{ goto _LL244;} _LL244: if( _temp238 ==  0){ goto _LL246;} else{ _temp257=*
_temp238; _LL261: _temp260=( int) _temp257.hd; goto _LL259; _LL259: _temp258=
_temp257.tl; if( _temp258 ==  0){ goto _LL256;} else{ goto _LL246;}} _LL256: if(
_temp260 == 'h'){ goto _LL245;} else{ goto _LL246;} _LL246: if( _temp238 ==  0){
goto _LL248;} else{ _temp263=* _temp238; _LL272: _temp271=( int) _temp263.hd;
goto _LL265; _LL265: _temp264= _temp263.tl; if( _temp264 ==  0){ goto _LL248;}
else{ _temp266=* _temp264; _LL270: _temp269=( int) _temp266.hd; goto _LL268;
_LL268: _temp267= _temp266.tl; if( _temp267 ==  0){ goto _LL262;} else{ goto
_LL248;}}} _LL262: if( _temp271 == 'h'? _temp269 == 'h': 0){ goto _LL247;} else{
goto _LL248;} _LL248: goto _LL249; _LL241: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t,
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp237), Cyc_Absyn_empty_tqual());
goto _LL239; _LL243: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp237), Cyc_Absyn_empty_tqual());
goto _LL239; _LL245: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp237), Cyc_Absyn_empty_tqual());
goto _LL239; _LL247: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp237), Cyc_Absyn_empty_tqual());
goto _LL239; _LL249:({ struct Cyc_Std_String_pa_struct _temp274; _temp274.tag=
Cyc_Std_String_pa; _temp274.f1=( struct _tagged_arr) Cyc_Std_implode( _temp65);{
void* _temp273[ 1u]={& _temp274}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%n",
sizeof( unsigned char), 42u), _tag_arr( _temp273, sizeof( void*), 1u));}});
return 0; _LL239:;} _temp55=({ struct Cyc_List_List* _temp275=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp275->hd=( void*) t; _temp275->tl=
_temp55; _temp275;}); break;} case '%': _LL232: if( _temp71 !=  0){({ struct Cyc_Std_String_pa_struct
_temp279; _temp279.tag= Cyc_Std_String_pa; _temp279.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp71);{ void* _temp278[ 1u]={& _temp279}; Cyc_Tcutil_terr( loc, _tag_arr("flags '%s' not allowed with %%%%",
sizeof( unsigned char), 33u), _tag_arr( _temp278, sizeof( void*), 1u));}});
return 0;} if( _temp69 !=  0){({ struct Cyc_Std_String_pa_struct _temp281;
_temp281.tag= Cyc_Std_String_pa; _temp281.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp69);{ void* _temp280[ 1u]={& _temp281}; Cyc_Tcutil_terr( loc, _tag_arr("width '%s' not allowed with %%%%",
sizeof( unsigned char), 33u), _tag_arr( _temp280, sizeof( void*), 1u));}});
return 0;} if( _temp67 !=  0){({ struct Cyc_Std_String_pa_struct _temp283;
_temp283.tag= Cyc_Std_String_pa; _temp283.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp67);{ void* _temp282[ 1u]={& _temp283}; Cyc_Tcutil_terr( loc, _tag_arr("precision '%s' not allowed with %%%%",
sizeof( unsigned char), 37u), _tag_arr( _temp282, sizeof( void*), 1u));}});
return 0;} if( _temp65 !=  0){({ struct Cyc_Std_String_pa_struct _temp285;
_temp285.tag= Cyc_Std_String_pa; _temp285.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp65);{ void* _temp284[ 1u]={& _temp285}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%%%",
sizeof( unsigned char), 43u), _tag_arr( _temp284, sizeof( void*), 1u));}});
return 0;} break; default: _LL277: return 0;}}}}} return Cyc_List_imp_rev(
_temp55);} struct _tuple4{ int f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; unsigned char f4; int f5; } ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat(
struct _tagged_arr s, int i){ unsigned int _temp287= Cyc_Std_strlen( s); if( i < 
0? 1: i >=  _temp287){ return 0;}{ int _temp288= 0; unsigned char _temp289=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
if( _temp289 == '*'){ _temp288= 1; i ++; if( i >=  _temp287){ return 0;}}{
struct Cyc_List_List* _temp290= 0; for( 0; i <  _temp287; i ++){ _temp289=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
if( isdigit(( int) _temp289)){ _temp290=({ struct Cyc_List_List* _temp291=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp291->hd=(
void*)(( int) _temp289); _temp291->tl= _temp290; _temp291;});} else{ break;}}
if( i >=  _temp287){ return 0;} _temp290=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp290);{ struct Cyc_List_List* _temp292= 0; _temp289=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
switch( _temp289){ case 'h': _LL293: _temp292=({ struct Cyc_List_List* _temp295=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp295->hd=(
void*)(( int) _temp289); _temp295->tl= _temp292; _temp295;}); i ++; if( i >= 
_temp287){ return 0;} _temp289=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp289 == 'h'){
_temp292=({ struct Cyc_List_List* _temp296=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp296->hd=( void*)(( int) _temp289); _temp296->tl=
_temp292; _temp296;}); i ++;} break; case 'l': _LL294: _temp292=({ struct Cyc_List_List*
_temp298=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp298->hd=( void*)(( int) _temp289); _temp298->tl= _temp292; _temp298;}); i
++; if( i >=  _temp287){ return 0;} _temp289=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp289 == 'l'){
_temp292=({ struct Cyc_List_List* _temp299=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp299->hd=( void*)(( int) _temp289); _temp299->tl=
_temp292; _temp299;}); i ++;} break; case 'j': _LL297: goto _LL300; case 'z':
_LL300: goto _LL301; case 't': _LL301: goto _LL302; case 'L': _LL302: _temp292=({
struct Cyc_List_List* _temp304=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp304->hd=( void*)(( int) _temp289); _temp304->tl=
_temp292; _temp304;}); i ++; break; default: _LL303: break;} if( i >=  _temp287){
return 0;} _temp292=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp292); _temp289=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( _temp289){ case 'd': _LL306: break; case 'i':
_LL307: break; case 'o': _LL308: break; case 'u': _LL309: break; case 'x':
_LL310: break; case 'X': _LL311: break; case 'f': _LL312: break; case 'F':
_LL313: break; case 'e': _LL314: break; case 'E': _LL315: break; case 'g':
_LL316: break; case 'G': _LL317: break; case 'a': _LL318: break; case 'A':
_LL319: break; case 'c': _LL320: break; case 's': _LL321: break; case 'p':
_LL322: break; case 'n': _LL323: break; case '%': _LL324: break; default: _LL325:
return 0;} return({ struct Cyc_Core_Opt* _temp327=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp327->v=( void*)({ struct _tuple4*
_temp328=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp328->f1=
_temp288; _temp328->f2= _temp290; _temp328->f3= _temp292; _temp328->f4= _temp289;
_temp328->f5= i +  1; _temp328;}); _temp327;});}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ unsigned int _temp329= Cyc_Std_strlen( s); struct Cyc_List_List* _temp330=
0; int i; for( i= 0; i <  _temp329; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) != '%'){ continue;}{
struct Cyc_Core_Opt* _temp331= Cyc_Formatstr_parse_inputformat( s, i +  1); if(
_temp331 ==  0){({ void* _temp332[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad format string",
sizeof( unsigned char), 18u), _tag_arr( _temp332, sizeof( void*), 0u));});
return 0;}{ struct _tuple4 _temp335; int _temp336; unsigned char _temp338;
struct Cyc_List_List* _temp340; struct Cyc_List_List* _temp342; int _temp344;
struct _tuple4* _temp333=( struct _tuple4*)(( struct Cyc_Core_Opt*) _check_null(
_temp331))->v; _temp335=* _temp333; _LL345: _temp344= _temp335.f1; goto _LL343;
_LL343: _temp342= _temp335.f2; goto _LL341; _LL341: _temp340= _temp335.f3; goto
_LL339; _LL339: _temp338= _temp335.f4; goto _LL337; _LL337: _temp336= _temp335.f5;
goto _LL334; _LL334: i= _temp336 -  1;{ struct Cyc_List_List* _temp346= _temp340;
struct Cyc_List_List _temp353; struct Cyc_List_List* _temp354; int _temp356;
_LL348: if( _temp346 ==  0){ goto _LL350;} else{ _temp353=* _temp346; _LL357:
_temp356=( int) _temp353.hd; goto _LL355; _LL355: _temp354= _temp353.tl; if(
_temp354 ==  0){ goto _LL352;} else{ goto _LL350;}} _LL352: if(( _temp356 == 'j'?
1: _temp356 == 'z')? 1: _temp356 == 't'){ goto _LL349;} else{ goto _LL350;}
_LL350: goto _LL351; _LL349:({ struct Cyc_Std_Int_pa_struct _temp359; _temp359.tag=
Cyc_Std_Int_pa; _temp359.f1=( int)(( unsigned int) _temp356);{ void* _temp358[ 1u]={&
_temp359}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%c' is not supported",
sizeof( unsigned char), 38u), _tag_arr( _temp358, sizeof( void*), 1u));}});
return 0; _LL351: goto _LL347; _LL347:;} if( _temp344){ continue;}{ void* t;
switch( _temp338){ case 'd': _LL360: goto _LL361; case 'i': _LL361: { struct Cyc_Core_Opt*
_temp363=({ struct Cyc_Core_Opt* _temp403=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp403->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp403;});{ struct Cyc_List_List* _temp364= _temp340; struct Cyc_List_List
_temp377; struct Cyc_List_List* _temp378; int _temp380; struct Cyc_List_List
_temp383; struct Cyc_List_List* _temp384; int _temp386; struct Cyc_List_List
_temp389; struct Cyc_List_List* _temp390; struct Cyc_List_List _temp392; struct
Cyc_List_List* _temp393; int _temp395; int _temp397; _LL366: if( _temp364 ==  0){
goto _LL367;} else{ goto _LL368;} _LL368: if( _temp364 ==  0){ goto _LL370;}
else{ _temp377=* _temp364; _LL381: _temp380=( int) _temp377.hd; goto _LL379;
_LL379: _temp378= _temp377.tl; if( _temp378 ==  0){ goto _LL376;} else{ goto
_LL370;}} _LL376: if( _temp380 == 'l'){ goto _LL369;} else{ goto _LL370;} _LL370:
if( _temp364 ==  0){ goto _LL372;} else{ _temp383=* _temp364; _LL387: _temp386=(
int) _temp383.hd; goto _LL385; _LL385: _temp384= _temp383.tl; if( _temp384 ==  0){
goto _LL382;} else{ goto _LL372;}} _LL382: if( _temp386 == 'h'){ goto _LL371;}
else{ goto _LL372;} _LL372: if( _temp364 ==  0){ goto _LL374;} else{ _temp389=*
_temp364; _LL398: _temp397=( int) _temp389.hd; goto _LL391; _LL391: _temp390=
_temp389.tl; if( _temp390 ==  0){ goto _LL374;} else{ _temp392=* _temp390;
_LL396: _temp395=( int) _temp392.hd; goto _LL394; _LL394: _temp393= _temp392.tl;
if( _temp393 ==  0){ goto _LL388;} else{ goto _LL374;}}} _LL388: if( _temp397 == 'h'?
_temp395 == 'h': 0){ goto _LL373;} else{ goto _LL374;} _LL374: goto _LL375;
_LL367: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp363), Cyc_Absyn_empty_tqual()); goto _LL365; _LL369: t=
Cyc_Absyn_at_typ( Cyc_Absyn_slong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp363), Cyc_Absyn_empty_tqual()); goto _LL365; _LL371: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp363), Cyc_Absyn_empty_tqual()); goto _LL365; _LL373: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp363), Cyc_Absyn_empty_tqual()); goto _LL365; _LL375:({
struct Cyc_Std_Int_pa_struct _temp401; _temp401.tag= Cyc_Std_Int_pa; _temp401.f1=(
int)(( unsigned int)(( int) _temp338));{ struct Cyc_Std_String_pa_struct
_temp400; _temp400.tag= Cyc_Std_String_pa; _temp400.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp340);{ void* _temp399[ 2u]={& _temp400,& _temp401}; Cyc_Tcutil_terr( loc,
_tag_arr("length modifier '%s' is not allowed with %%%c", sizeof( unsigned char),
46u), _tag_arr( _temp399, sizeof( void*), 2u));}}}); return 0; _LL365:;}
_temp330=({ struct Cyc_List_List* _temp402=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp402->hd=( void*) t; _temp402->tl= _temp330;
_temp402;}); break;} case 'u': _LL362: goto _LL404; case 'o': _LL404: goto
_LL405; case 'x': _LL405: goto _LL406; case 'X': _LL406: { struct Cyc_Core_Opt*
_temp408=({ struct Cyc_Core_Opt* _temp448=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp448->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp448;});{ struct Cyc_List_List* _temp409= _temp340; struct Cyc_List_List
_temp422; struct Cyc_List_List* _temp423; int _temp425; struct Cyc_List_List
_temp428; struct Cyc_List_List* _temp429; int _temp431; struct Cyc_List_List
_temp434; struct Cyc_List_List* _temp435; struct Cyc_List_List _temp437; struct
Cyc_List_List* _temp438; int _temp440; int _temp442; _LL411: if( _temp409 ==  0){
goto _LL412;} else{ goto _LL413;} _LL413: if( _temp409 ==  0){ goto _LL415;}
else{ _temp422=* _temp409; _LL426: _temp425=( int) _temp422.hd; goto _LL424;
_LL424: _temp423= _temp422.tl; if( _temp423 ==  0){ goto _LL421;} else{ goto
_LL415;}} _LL421: if( _temp425 == 'l'){ goto _LL414;} else{ goto _LL415;} _LL415:
if( _temp409 ==  0){ goto _LL417;} else{ _temp428=* _temp409; _LL432: _temp431=(
int) _temp428.hd; goto _LL430; _LL430: _temp429= _temp428.tl; if( _temp429 ==  0){
goto _LL427;} else{ goto _LL417;}} _LL427: if( _temp431 == 'h'){ goto _LL416;}
else{ goto _LL417;} _LL417: if( _temp409 ==  0){ goto _LL419;} else{ _temp434=*
_temp409; _LL443: _temp442=( int) _temp434.hd; goto _LL436; _LL436: _temp435=
_temp434.tl; if( _temp435 ==  0){ goto _LL419;} else{ _temp437=* _temp435;
_LL441: _temp440=( int) _temp437.hd; goto _LL439; _LL439: _temp438= _temp437.tl;
if( _temp438 ==  0){ goto _LL433;} else{ goto _LL419;}}} _LL433: if( _temp442 == 'h'?
_temp440 == 'h': 0){ goto _LL418;} else{ goto _LL419;} _LL419: goto _LL420;
_LL412: t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp408), Cyc_Absyn_empty_tqual()); goto _LL410; _LL414: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp408), Cyc_Absyn_empty_tqual()); goto _LL410; _LL416: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp408), Cyc_Absyn_empty_tqual()); goto _LL410; _LL418: t=
Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp408), Cyc_Absyn_empty_tqual()); goto _LL410; _LL420:({
struct Cyc_Std_Int_pa_struct _temp446; _temp446.tag= Cyc_Std_Int_pa; _temp446.f1=(
int)(( unsigned int)(( int) _temp338));{ struct Cyc_Std_String_pa_struct
_temp445; _temp445.tag= Cyc_Std_String_pa; _temp445.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp340);{ void* _temp444[ 2u]={& _temp445,& _temp446}; Cyc_Tcutil_terr( loc,
_tag_arr("length modifier '%s' is not allowed with %%%c", sizeof( unsigned char),
46u), _tag_arr( _temp444, sizeof( void*), 2u));}}}); return 0; _LL410:;}
_temp330=({ struct Cyc_List_List* _temp447=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp447->hd=( void*) t; _temp447->tl= _temp330;
_temp447;}); break;} case 'f': _LL407: goto _LL449; case 'F': _LL449: goto
_LL450; case 'e': _LL450: goto _LL451; case 'E': _LL451: goto _LL452; case 'g':
_LL452: goto _LL453; case 'G': _LL453: goto _LL454; case 'a': _LL454: goto
_LL455; case 'A': _LL455: { struct Cyc_Core_Opt* _temp457=({ struct Cyc_Core_Opt*
_temp476=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp476->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp476;});{ struct Cyc_List_List*
_temp458= _temp340; struct Cyc_List_List _temp467; struct Cyc_List_List*
_temp468; int _temp470; _LL460: if( _temp458 ==  0){ goto _LL461;} else{ goto
_LL462;} _LL462: if( _temp458 ==  0){ goto _LL464;} else{ _temp467=* _temp458;
_LL471: _temp470=( int) _temp467.hd; goto _LL469; _LL469: _temp468= _temp467.tl;
if( _temp468 ==  0){ goto _LL466;} else{ goto _LL464;}} _LL466: if( _temp470 == 'l'){
goto _LL463;} else{ goto _LL464;} _LL464: goto _LL465; _LL461: t= Cyc_Absyn_at_typ(
Cyc_Absyn_float_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp457), Cyc_Absyn_empty_tqual()); goto _LL459; _LL463: t= Cyc_Absyn_at_typ(
Cyc_Absyn_double_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp457), Cyc_Absyn_empty_tqual()); goto _LL459; _LL465:({ struct Cyc_Std_Int_pa_struct
_temp474; _temp474.tag= Cyc_Std_Int_pa; _temp474.f1=( int)(( unsigned int)(( int)
_temp338));{ struct Cyc_Std_String_pa_struct _temp473; _temp473.tag= Cyc_Std_String_pa;
_temp473.f1=( struct _tagged_arr) Cyc_Std_implode( _temp340);{ void* _temp472[ 2u]={&
_temp473,& _temp474}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u), _tag_arr( _temp472, sizeof( void*), 2u));}}});
return 0; _LL459:;} _temp330=({ struct Cyc_List_List* _temp475=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp475->hd=( void*) t; _temp475->tl=
_temp330; _temp475;}); break;} case 'c': _LL456:({ void* _temp478[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("%%c is not supported", sizeof( unsigned char), 21u), _tag_arr(
_temp478, sizeof( void*), 0u));}); return 0; case 's': _LL477:({ void* _temp480[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("%%s is not supported", sizeof(
unsigned char), 21u), _tag_arr( _temp480, sizeof( void*), 0u));}); return 0;
case '[': _LL479:({ void* _temp482[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("%%[ is not supported",
sizeof( unsigned char), 21u), _tag_arr( _temp482, sizeof( void*), 0u));});
return 0; case 'p': _LL481:({ void* _temp484[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("%%p is not supported", sizeof( unsigned char), 21u), _tag_arr(
_temp484, sizeof( void*), 0u));}); return 0; case 'n': _LL483: { struct Cyc_Core_Opt*
_temp486=({ struct Cyc_Core_Opt* _temp525=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp525->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp525;});{ struct Cyc_List_List* _temp487= _temp340; struct Cyc_List_List
_temp500; struct Cyc_List_List* _temp501; int _temp503; struct Cyc_List_List
_temp506; struct Cyc_List_List* _temp507; int _temp509; struct Cyc_List_List
_temp512; struct Cyc_List_List* _temp513; struct Cyc_List_List _temp515; struct
Cyc_List_List* _temp516; int _temp518; int _temp520; _LL489: if( _temp487 ==  0){
goto _LL490;} else{ goto _LL491;} _LL491: if( _temp487 ==  0){ goto _LL493;}
else{ _temp500=* _temp487; _LL504: _temp503=( int) _temp500.hd; goto _LL502;
_LL502: _temp501= _temp500.tl; if( _temp501 ==  0){ goto _LL499;} else{ goto
_LL493;}} _LL499: if( _temp503 == 'l'){ goto _LL492;} else{ goto _LL493;} _LL493:
if( _temp487 ==  0){ goto _LL495;} else{ _temp506=* _temp487; _LL510: _temp509=(
int) _temp506.hd; goto _LL508; _LL508: _temp507= _temp506.tl; if( _temp507 ==  0){
goto _LL505;} else{ goto _LL495;}} _LL505: if( _temp509 == 'h'){ goto _LL494;}
else{ goto _LL495;} _LL495: if( _temp487 ==  0){ goto _LL497;} else{ _temp512=*
_temp487; _LL521: _temp520=( int) _temp512.hd; goto _LL514; _LL514: _temp513=
_temp512.tl; if( _temp513 ==  0){ goto _LL497;} else{ _temp515=* _temp513;
_LL519: _temp518=( int) _temp515.hd; goto _LL517; _LL517: _temp516= _temp515.tl;
if( _temp516 ==  0){ goto _LL511;} else{ goto _LL497;}}} _LL511: if( _temp520 == 'h'?
_temp518 == 'h': 0){ goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498;
_LL490: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp486), Cyc_Absyn_empty_tqual()); goto _LL488; _LL492: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp486), Cyc_Absyn_empty_tqual()); goto _LL488; _LL494: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp486), Cyc_Absyn_empty_tqual()); goto _LL488; _LL496: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp486), Cyc_Absyn_empty_tqual()); goto _LL488; _LL498:({
struct Cyc_Std_String_pa_struct _temp523; _temp523.tag= Cyc_Std_String_pa;
_temp523.f1=( struct _tagged_arr) Cyc_Std_implode( _temp340);{ void* _temp522[ 1u]={&
_temp523}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%n",
sizeof( unsigned char), 42u), _tag_arr( _temp522, sizeof( void*), 1u));}});
return 0; _LL488:;} _temp330=({ struct Cyc_List_List* _temp524=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp524->hd=( void*) t; _temp524->tl=
_temp330; _temp524;}); break;} case '%': _LL485: if( _temp344){({ void* _temp527[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Assignment suppression (*) is not allowed with %%%%",
sizeof( unsigned char), 52u), _tag_arr( _temp527, sizeof( void*), 0u));});
return 0;} if( _temp342 !=  0){({ struct Cyc_Std_String_pa_struct _temp529;
_temp529.tag= Cyc_Std_String_pa; _temp529.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp342);{ void* _temp528[ 1u]={& _temp529}; Cyc_Tcutil_terr( loc, _tag_arr("width '%s' not allowed with %%%%",
sizeof( unsigned char), 33u), _tag_arr( _temp528, sizeof( void*), 1u));}});
return 0;} if( _temp340 !=  0){({ struct Cyc_Std_String_pa_struct _temp531;
_temp531.tag= Cyc_Std_String_pa; _temp531.f1=( struct _tagged_arr) Cyc_Std_implode(
_temp340);{ void* _temp530[ 1u]={& _temp531}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%%%",
sizeof( unsigned char), 43u), _tag_arr( _temp530, sizeof( void*), 1u));}});
return 0;} break; default: _LL526: return 0;}}}}} return Cyc_List_imp_rev(
_temp330);}
