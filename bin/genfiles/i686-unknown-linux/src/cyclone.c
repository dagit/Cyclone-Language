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
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_filter(
int(* f)( void*), struct Cyc_List_List* x); extern unsigned char Cyc_Arg_Bad[ 8u];
struct Cyc_Arg_Bad_struct{ unsigned char* tag; struct _tagged_arr f1; } ; extern
unsigned char Cyc_Arg_Error[ 10u]; static const int Cyc_Arg_Unit_spec= 0; struct
Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)(); } ; static const int Cyc_Arg_Flag_spec=
1; struct Cyc_Arg_Flag_spec_struct{ int tag; void(* f1)( struct _tagged_arr); }
; static const int Cyc_Arg_FlagString_spec= 2; struct Cyc_Arg_FlagString_spec_struct{
int tag; void(* f1)( struct _tagged_arr, struct _tagged_arr); } ; static const
int Cyc_Arg_Set_spec= 3; struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ;
static const int Cyc_Arg_Clear_spec= 4; struct Cyc_Arg_Clear_spec_struct{ int
tag; int* f1; } ; static const int Cyc_Arg_String_spec= 5; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_Int_spec=
6; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct Cyc_List_List* specs,
void(* anonfun)( struct _tagged_arr), struct _tagged_arr errmsg, struct
_tagged_arr args); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stdout;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_remove( struct
_tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*); extern int
Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern struct Cyc_Std___sFILE* Cyc_Std_fopen(
struct _tagged_arr __filename, struct _tagged_arr __modes); extern unsigned int
Cyc_Std_fread( struct _tagged_arr __ptr, unsigned int __size, unsigned int __n,
struct Cyc_Std___sFILE* __stream); extern unsigned int Cyc_Std_fwrite( struct
_tagged_arr __ptr, unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE*
__s); extern int Cyc_Std_feof( struct Cyc_Std___sFILE* __stream); extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern struct Cyc_Std___sFILE* Cyc_Std_file_open( struct
_tagged_arr fname, struct _tagged_arr mode); extern void Cyc_Std_file_close(
struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Std_printf( struct _tagged_arr fmt, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt,
struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int system( unsigned char*); struct Cyc_Std__Div{
int quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int
abs( int __x); extern int atexit( void(* __func)()); extern struct Cyc_Std__Div
div( int __numer, int __denom); extern struct Cyc_Std__Ldiv ldiv( int __numer,
int __denom); extern int random(); extern void srandom( unsigned int __seed);
extern int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern struct _tagged_arr Cyc_Std_getenv( struct _tagged_arr); extern
unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern int Cyc_Std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_strconcat_l(
struct Cyc_List_List*); extern struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_substring( struct
_tagged_arr, int ofs, unsigned int n); extern struct _tagged_arr Cyc_Std_strchr(
struct _tagged_arr s, unsigned char c); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct _tagged_arr Cyc_Filename_concat( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension( struct
_tagged_arr); extern int Cyc_Filename_check_suffix( struct _tagged_arr, struct
_tagged_arr); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; extern void Cyc_Position_reset_position( struct _tagged_arr); struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_error_p(); struct _tuple0{ void* f1; struct _tagged_arr* f2; }
; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
22; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 23; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
24; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 31; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
32; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 33; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 34; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 35; struct Cyc_Absyn_Fill_e_struct{
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
15u]; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern int Cyc_yyparse(); extern unsigned char Cyc_AbstractDeclarator_tok[ 27u];
struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern unsigned char Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[
18u]; struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{ unsigned char* tag;
int f1; } ; extern unsigned char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
unsigned char* tag; unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Declaration_spec* f1; } ; extern unsigned char
Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{ unsigned char* tag;
struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[ 23u];
struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple3* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple4{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple4* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple5{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple5* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple6{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple6* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple7{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple7* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
17u]; struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u];
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; extern
unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; extern int Cyc_yyparse();
extern unsigned char Cyc_YY1[ 8u]; struct Cyc_YY1_struct{ unsigned char* tag;
struct _tuple6* f1; } ; extern unsigned char Cyc_YYINITIALSVAL[ 18u]; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r; extern struct
Cyc_Absynpp_Params Cyc_Absynpp_c_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); extern void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params* fs); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init();
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_flush_warnings();
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct
Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* decls);
struct Cyc_CfFlowInfo_Place; enum  Cyc_CfFlowInfo_Escaped; struct Cyc_CfFlowInfo_InitState;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ; enum 
Cyc_CfFlowInfo_Escaped{ Cyc_CfFlowInfo_Esc  =  0u, Cyc_CfFlowInfo_Unesc  =  1u};
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_MustPointTo=
0; struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; struct Cyc_CfFlowInfo_InitState{ enum  Cyc_CfFlowInfo_Escaped esc; void*
level; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; struct Cyc_CfFlowInfo_InitState f1; } ; static const int Cyc_CfFlowInfo_DictPI=
1; struct Cyc_CfFlowInfo_DictPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict* roots; int in_try; void*
tryflow; struct Cyc_Set_Set** all_changed; } ; extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); struct Cyc_Interface_I; extern struct Cyc_Interface_I*
Cyc_Interface_empty(); extern struct Cyc_Interface_I* Cyc_Interface_final();
extern struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae);
struct _tuple8{ struct _tagged_arr f1; struct _tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple8* info);
extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo); extern
struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Std___sFILE*); extern
void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Std___sFILE*);
static unsigned char _temp0[ 6u]="0.1.2"; static struct _tagged_arr Cyc_version={
_temp0, _temp0, _temp0 +  6u}; extern void Cyc_Lex_lex_init(); static int Cyc_pp_r=
0; static int Cyc_noexpand_r= 0; static int Cyc_noshake_r= 0; static int Cyc_stop_after_cpp_r=
0; static int Cyc_parseonly_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0;
static int Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r= 0; static int
Cyc_stop_after_asmfile_r= 0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0;
static int Cyc_save_temps_r= 0; static int Cyc_save_c_r= 0; static int Cyc_nogc_r=
0; static int Cyc_pa_r= 0; static int Cyc_add_cyc_namespace_r= 1; static int Cyc_print_full_evars_r=
0; static int Cyc_print_all_tvars_r= 0; static int Cyc_print_all_kinds_r= 0;
static struct _tagged_arr* Cyc_output_file= 0; static void Cyc_set_output_file(
struct _tagged_arr s){ Cyc_output_file=({ struct _tagged_arr* _temp1=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1[ 0]= s; _temp1;});}
extern unsigned char* Cdef_inc_path; extern unsigned char* Cdef_lib_path; extern
unsigned char* Carch; extern unsigned char* Ccomp; extern unsigned char* Ccflags;
static unsigned char _temp2[ 1u]=""; static struct _tagged_arr Cyc_cpp={ _temp2,
_temp2, _temp2 +  1u}; static void Cyc_set_cpp( struct _tagged_arr s){ Cyc_cpp=
s;} static struct Cyc_List_List* Cyc_cppargs= 0; static void Cyc_add_cpparg(
struct _tagged_arr s){ Cyc_cppargs=({ struct Cyc_List_List* _temp3=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3->hd=( void*)({ struct
_tagged_arr* _temp4=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp4[ 0]= s; _temp4;}); _temp3->tl= Cyc_cppargs; _temp3;});} static void Cyc_print_version(){({
struct Cyc_Std_String_pa_struct _temp6; _temp6.tag= Cyc_Std_String_pa; _temp6.f1=(
struct _tagged_arr) Cyc_version;{ void* _temp5[ 1u]={& _temp6}; Cyc_Std_printf(
_tag_arr("The Cyclone compiler, version %s\n", sizeof( unsigned char), 34u),
_tag_arr( _temp5, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp10; _temp10.tag= Cyc_Std_String_pa; _temp10.f1=( struct _tagged_arr)
Cstring_to_string( Ccflags);{ struct Cyc_Std_String_pa_struct _temp9; _temp9.tag=
Cyc_Std_String_pa; _temp9.f1=( struct _tagged_arr) Cstring_to_string( Ccomp);{
struct Cyc_Std_String_pa_struct _temp8; _temp8.tag= Cyc_Std_String_pa; _temp8.f1=(
struct _tagged_arr) Cstring_to_string( Carch);{ void* _temp7[ 3u]={& _temp8,&
_temp9,& _temp10}; Cyc_Std_printf( _tag_arr("Compiled for architecture: %s CC=\"%s %s\"\n",
sizeof( unsigned char), 42u), _tag_arr( _temp7, sizeof( void*), 3u));}}}});({
struct Cyc_Std_String_pa_struct _temp12; _temp12.tag= Cyc_Std_String_pa; _temp12.f1=(
struct _tagged_arr) Cstring_to_string( Cdef_lib_path);{ void* _temp11[ 1u]={&
_temp12}; Cyc_Std_printf( _tag_arr("Standard library directory: %s\n", sizeof(
unsigned char), 32u), _tag_arr( _temp11, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path);{ void* _temp13[ 1u]={& _temp14}; Cyc_Std_printf(
_tag_arr("Standard include directory: %s\n", sizeof( unsigned char), 32u),
_tag_arr( _temp13, sizeof( void*), 1u));}}); exit( 0);} static int Cyc_is_cyclone_sourcefile(
struct _tagged_arr s){ unsigned int _temp15= Cyc_Std_strlen( s); if( _temp15 <= 
4){ return 0;} else{ return Cyc_Std_strcmp( _tagged_arr_plus( s, sizeof(
unsigned char),( int)( _temp15 -  4)), _tag_arr(".cyc", sizeof( unsigned char),
5u)) ==  0;}} static struct Cyc_List_List* Cyc_cyclone_lib_path= 0; static void
Cyc_add_cyclone_lib_path( struct _tagged_arr s){ unsigned int _temp16= Cyc_Std_strlen(
s); if( _temp16 <=  2){ return;}{ struct _tagged_arr _temp17=( struct
_tagged_arr) Cyc_Std_substring( s, 2, _temp16 -  2); Cyc_cyclone_lib_path=({
struct Cyc_List_List* _temp18=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp18->hd=( void*)({ struct _tagged_arr* _temp19=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp19[ 0]= _temp17;
_temp19;}); _temp18->tl= Cyc_cyclone_lib_path; _temp18;});}} static struct Cyc_List_List*
Cyc_ccargs= 0; static void Cyc_add_ccarg( struct _tagged_arr s){ Cyc_ccargs=({
struct Cyc_List_List* _temp20=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp20->hd=( void*)({ struct _tagged_arr* _temp21=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp21[ 0]= s; _temp21;});
_temp20->tl= Cyc_ccargs; _temp20;});} static void Cyc_add_ccarg2( struct
_tagged_arr flag, struct _tagged_arr arg){ Cyc_ccargs=({ struct Cyc_List_List*
_temp22=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp22->hd=( void*)({ struct _tagged_arr* _temp25=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp25[ 0]= arg; _temp25;}); _temp22->tl=({
struct Cyc_List_List* _temp23=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp23->hd=( void*)({ struct _tagged_arr* _temp24=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp24[ 0]= flag;
_temp24;}); _temp23->tl= Cyc_ccargs; _temp23;}); _temp22;});} static struct Cyc_List_List*
Cyc_libargs= 0; static void Cyc_add_libarg( struct _tagged_arr s){ Cyc_libargs=({
struct Cyc_List_List* _temp26=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp26->hd=( void*)({ struct _tagged_arr* _temp27=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp27[ 0]= s; _temp27;});
_temp26->tl= Cyc_libargs; _temp26;});} static void Cyc_set_save_temps(){ Cyc_save_temps_r=
1; Cyc_add_ccarg( _tag_arr("-save-temps", sizeof( unsigned char), 12u));} static
int Cyc_produce_dependencies= 0; static void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r=
1; Cyc_produce_dependencies= 1; Cyc_add_cpparg( _tag_arr("-M", sizeof(
unsigned char), 3u));} static struct _tagged_arr* Cyc_dependencies_target= 0;
static void Cyc_set_dependencies_target( struct _tagged_arr s){ Cyc_dependencies_target=({
struct _tagged_arr* _temp28=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp28[ 0]= s; _temp28;});} static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r= 1; Cyc_add_ccarg( _tag_arr("-c", sizeof(
unsigned char), 3u));} static void Cyc_set_nocppprecomp(){ Cyc_add_cpparg(
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u)); Cyc_add_ccarg(
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u));} static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r= 0; Cyc_add_ccarg( _tag_arr("-DNO_CYC_PREFIX", sizeof(
unsigned char), 16u));} static void Cyc_set_pa(){ Cyc_pa_r= 1; Cyc_add_ccarg(
_tag_arr("-DCYC_REGION_PROFILE", sizeof( unsigned char), 21u));} static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r= 1; Cyc_add_ccarg( _tag_arr("-S", sizeof( unsigned char),
3u));} static const int Cyc_DEFAULTINPUT= 0; static const int Cyc_CYCLONEFILE= 1;
static void* Cyc_intype=( void*) 0u; static void Cyc_set_inputtype( struct
_tagged_arr s){ if( Cyc_Std_strcmp( s, _tag_arr("cyc", sizeof( unsigned char), 4u))
==  0){ Cyc_intype=( void*) Cyc_CYCLONEFILE;} else{ if( Cyc_Std_strcmp( s,
_tag_arr("none", sizeof( unsigned char), 5u)) ==  0){ Cyc_intype=( void*) Cyc_DEFAULTINPUT;}
else{({ struct Cyc_Std_String_pa_struct _temp30; _temp30.tag= Cyc_Std_String_pa;
_temp30.f1=( struct _tagged_arr) s;{ void* _temp29[ 1u]={& _temp30}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Input type '%s' not supported\n", sizeof(
unsigned char), 31u), _tag_arr( _temp29, sizeof( void*), 1u));}});}}} static
struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other( struct
_tagged_arr s){ if( Cyc_intype == ( void*) Cyc_CYCLONEFILE? 1: Cyc_is_cyclone_sourcefile(
s)){ Cyc_cyclone_files=({ struct Cyc_List_List* _temp31=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp31->hd=( void*)({ struct
_tagged_arr* _temp32=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp32[ 0]= s; _temp32;}); _temp31->tl= Cyc_cyclone_files;
_temp31;});{ struct _tagged_arr _temp33= Cyc_Filename_chop_extension( s); struct
_tagged_arr _temp34= Cyc_Std_strconcat(( struct _tagged_arr) _temp33, _tag_arr(".c",
sizeof( unsigned char), 3u)); Cyc_add_ccarg(( struct _tagged_arr) _temp34);}}
else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct _tagged_arr s){
if( Cyc_save_temps_r){ return;} else{ Cyc_Std_remove( s);}} int Cyc_compile_failure=
0; struct Cyc_Std___sFILE* Cyc_try_file_open( struct _tagged_arr filename,
struct _tagged_arr mode, struct _tagged_arr msg_part){ struct _handler_cons
_temp35; _push_handler(& _temp35);{ int _temp37= 0; if( setjmp( _temp35.handler)){
_temp37= 1;} if( ! _temp37){{ struct Cyc_Std___sFILE* _temp38=( struct Cyc_Std___sFILE*)
Cyc_Std_file_open( filename, mode); _npop_handler( 0u); return _temp38;};
_pop_handler();} else{ void* _temp36=( void*) _exn_thrown; void* _temp40=
_temp36; _LL42: goto _LL43; _LL44: goto _LL45; _LL43: Cyc_compile_failure= 1;({
struct Cyc_Std_String_pa_struct _temp48; _temp48.tag= Cyc_Std_String_pa; _temp48.f1=(
struct _tagged_arr) filename;{ struct Cyc_Std_String_pa_struct _temp47; _temp47.tag=
Cyc_Std_String_pa; _temp47.f1=( struct _tagged_arr) msg_part;{ void* _temp46[ 2u]={&
_temp47,& _temp48}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: couldn't open %s %s\n",
sizeof( unsigned char), 29u), _tag_arr( _temp46, sizeof( void*), 2u));}}}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return 0; _LL45:( void) _throw(
_temp40); _LL41:;}}} struct Cyc_List_List* Cyc_do_stage( struct _tagged_arr
stage_name, struct Cyc_List_List*(* f)( void*, struct Cyc_List_List*), void* env,
struct Cyc_List_List* tds){ struct _tagged_arr exn_string= _tag_arr("", sizeof(
unsigned char), 1u); struct _tagged_arr explain_string= _tag_arr("", sizeof(
unsigned char), 1u); int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct*
_temp77=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp77[ 0]=({ struct Cyc_Core_Impossible_struct _temp78; _temp78.tag= Cyc_Core_Impossible;
_temp78.f1= _tag_arr("", sizeof( unsigned char), 1u); _temp78;}); _temp77;});
struct Cyc_List_List* _temp49= 0;{ struct _handler_cons _temp50; _push_handler(&
_temp50);{ int _temp52= 0; if( setjmp( _temp50.handler)){ _temp52= 1;} if( !
_temp52){ _temp49= f( env, tds);; _pop_handler();} else{ void* _temp51=( void*)
_exn_thrown; void* _temp54= _temp51; struct _tagged_arr _temp66; struct
_tagged_arr _temp68; _LL56: if(*(( void**) _temp54) ==  Cyc_Core_Impossible){
_LL67: _temp66=(( struct Cyc_Core_Impossible_struct*) _temp54)->f1; goto _LL57;}
else{ goto _LL58;} _LL58: if( _temp54 ==  Cyc_Dict_Absent){ goto _LL59;} else{
goto _LL60;} _LL60: if(*(( void**) _temp54) ==  Cyc_Core_Invalid_argument){
_LL69: _temp68=(( struct Cyc_Core_Invalid_argument_struct*) _temp54)->f1; goto
_LL61;} else{ goto _LL62;} _LL62: goto _LL63; _LL64: goto _LL65; _LL57:
exn_string= _tag_arr("Exception Core::Impossible", sizeof( unsigned char), 27u);
explain_string= _temp66; goto _LL55; _LL59: exn_string= _tag_arr("Exception Dict::Absent",
sizeof( unsigned char), 23u); goto _LL55; _LL61: exn_string= _tag_arr("Exception Core::Invalid_argument",
sizeof( unsigned char), 33u); explain_string= _temp68; goto _LL55; _LL63: ex=
_temp54; other_exn= 1; exn_string= _tag_arr("Uncaught exception", sizeof(
unsigned char), 19u); goto _LL55; _LL65:( void) _throw( _temp54); _LL55:;}}} if(
Cyc_Position_error_p()){ Cyc_compile_failure= 1;} if( Cyc_Std_strcmp( exn_string,
_tag_arr("", sizeof( unsigned char), 1u)) !=  0){ Cyc_compile_failure= 1;({
struct Cyc_Std_String_pa_struct _temp73; _temp73.tag= Cyc_Std_String_pa; _temp73.f1=(
struct _tagged_arr) explain_string;{ struct Cyc_Std_String_pa_struct _temp72;
_temp72.tag= Cyc_Std_String_pa; _temp72.f1=( struct _tagged_arr) stage_name;{
struct Cyc_Std_String_pa_struct _temp71; _temp71.tag= Cyc_Std_String_pa; _temp71.f1=(
struct _tagged_arr) exn_string;{ void* _temp70[ 3u]={& _temp71,& _temp72,&
_temp73}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n%s thrown during %s: %s",
sizeof( unsigned char), 25u), _tag_arr( _temp70, sizeof( void*), 3u));}}}});}
if( Cyc_compile_failure){({ void* _temp74[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\nCOMPILATION FAILED!\n", sizeof( unsigned char), 22u), _tag_arr(
_temp74, sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);
if( other_exn){( int) _throw( ex);} return _temp49;} else{ if( Cyc_v_r){({
struct Cyc_Std_String_pa_struct _temp76; _temp76.tag= Cyc_Std_String_pa; _temp76.f1=(
struct _tagged_arr) stage_name;{ void* _temp75[ 1u]={& _temp76}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s completed.\n", sizeof( unsigned char), 15u),
_tag_arr( _temp75, sizeof( void*), 1u));}}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*)
Cyc_Std_stderr); return _temp49;}} return _temp49;} struct Cyc_List_List* Cyc_do_parse(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp79= Cyc_Parse_parse_file( f); Cyc_Lex_lex_init();
return _temp79;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds); if( ! Cyc_noshake_r){
tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck(
int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds);
return tds;} struct _tuple9{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Std___sFILE*
f2; struct Cyc_Std___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple9* params, struct Cyc_List_List* tds){ struct _tuple9 _temp82;
struct Cyc_Std___sFILE* _temp83; struct Cyc_Std___sFILE* _temp85; struct Cyc_Tcenv_Tenv*
_temp87; struct _tuple9* _temp80= params; _temp82=* _temp80; _LL88: _temp87=
_temp82.f1; goto _LL86; _LL86: _temp85= _temp82.f2; goto _LL84; _LL84: _temp83=
_temp82.f3; goto _LL81; _LL81: { struct Cyc_Interface_I* _temp89= Cyc_Interface_extract(
_temp87->ae); if( _temp85 ==  0){ Cyc_Interface_save( _temp89, _temp83);} else{
struct Cyc_Interface_I* _temp90= Cyc_Interface_parse(( struct Cyc_Std___sFILE*)
_check_null( _temp85)); if( ! Cyc_Interface_is_subinterface( _temp90, _temp89,({
struct _tuple8* _temp91=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp91->f1= _tag_arr("written interface", sizeof( unsigned char), 18u); _temp91->f2=
_tag_arr("maximal interface", sizeof( unsigned char), 18u); _temp91;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp90, _temp83);}} return tds;}} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_tovc( int ignore, struct Cyc_List_List* tds){
return Cyc_Tovc_tovc( tds);} static unsigned char _temp92[ 1u]=""; static struct
_tagged_arr Cyc_cyc_include={ _temp92, _temp92, _temp92 +  1u}; struct Cyc_List_List*
Cyc_do_print( struct Cyc_Std___sFILE* out_file, struct Cyc_List_List* tds){
struct Cyc_Absynpp_Params params_r; if( Cyc_tc_r){ params_r= Cyc_Absynpp_cyc_params_r;}
else{ params_r= Cyc_Absynpp_c_params_r;} params_r.expand_typedefs= ! Cyc_noexpand_r;
params_r.to_VC= Cyc_tovc_r; params_r.add_cyc_prefix= Cyc_add_cyc_namespace_r;
params_r.print_full_evars= Cyc_print_full_evars_r; params_r.print_all_tvars= Cyc_print_all_tvars_r;
params_r.print_all_kinds= Cyc_print_all_kinds_r;{ struct Cyc_Std___sFILE*
_temp93= Cyc_try_file_open( Cyc_cyc_include, _tag_arr("r", sizeof( unsigned char),
2u), _tag_arr("internal compiler file", sizeof( unsigned char), 23u)); if(
_temp93 ==  0){ return tds;}{ struct Cyc_Std___sFILE* _temp94=( struct Cyc_Std___sFILE*)
_check_null( _temp93); unsigned int n_read= 1024; unsigned int n_written;
unsigned char buf[ 1024u];{ unsigned int _temp97= 1024u; unsigned int i; for( i=
0; i <  _temp97; i ++){ buf[ i]='\000';}} while( n_read ==  1024) { n_read= Cyc_Std_fread(
_tag_arr( buf, sizeof( unsigned char), 1024u), 1, 1024, _temp94); if( n_read != 
1024? ! Cyc_Std_feof( _temp94): 0){ Cyc_compile_failure= 1;({ void* _temp95[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: problem copying cyc_include.h\n",
sizeof( unsigned char), 39u), _tag_arr( _temp95, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return tds;} n_written= Cyc_Std_fwrite(
_tag_arr( buf, sizeof( unsigned char), 1024u), 1, n_read, out_file); if( n_read
!=  n_written){ Cyc_compile_failure= 1;({ void* _temp96[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\nError: problem copying cyc_include.h\n", sizeof(
unsigned char), 39u), _tag_arr( _temp96, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return tds;}} Cyc_Std_fclose( _temp94);
if( Cyc_pp_r){ Cyc_Absynpp_set_params(& params_r); Cyc_Absynpp_decllist2file(
tds, out_file);} else{ Cyc_Absyndump_set_params(& params_r); Cyc_Absyndump_dumpdecllist2file(
tds, out_file);} Cyc_Std_fflush(( struct Cyc_Std___sFILE*) out_file); return tds;}}}
static struct Cyc_List_List* Cyc_cfiles= 0; static void Cyc_remove_cfiles(){ if(
! Cyc_save_c_r){ for( 0; Cyc_cfiles !=  0; Cyc_cfiles=(( struct Cyc_List_List*)
_check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}} static struct Cyc_List_List*
Cyc_split_by_char( struct _tagged_arr s, unsigned char c){ if( s.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ return 0;}{ struct Cyc_List_List*
_temp98= 0; unsigned int _temp99= Cyc_Std_strlen( s); while( _temp99 >  0) {
struct _tagged_arr _temp100= Cyc_Std_strchr( s, c); if( _temp100.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ _temp98=({ struct Cyc_List_List*
_temp101=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp101->hd=( void*)({ struct _tagged_arr* _temp102=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp102[ 0]= s; _temp102;}); _temp101->tl=
_temp98; _temp101;}); break;} else{ _temp98=({ struct Cyc_List_List* _temp103=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp103->hd=(
void*)({ struct _tagged_arr* _temp104=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp104[ 0]=( struct _tagged_arr) Cyc_Std_substring( s, 0,(
unsigned int)(((( struct _tagged_arr) _temp100).curr -  s.curr) /  sizeof(
unsigned char))); _temp104;}); _temp103->tl= _temp98; _temp103;}); _temp99 -=(((
struct _tagged_arr) _temp100).curr -  s.curr) /  sizeof( unsigned char); s=
_tagged_arr_plus( _temp100, sizeof( unsigned char), 1);}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp98);}} static int Cyc_file_exists(
struct _tagged_arr file){ struct Cyc_Std___sFILE* f= 0;{ struct _handler_cons
_temp105; _push_handler(& _temp105);{ int _temp107= 0; if( setjmp( _temp105.handler)){
_temp107= 1;} if( ! _temp107){ f=( struct Cyc_Std___sFILE*) Cyc_Std_file_open(
file, _tag_arr("r", sizeof( unsigned char), 2u));; _pop_handler();} else{ void*
_temp106=( void*) _exn_thrown; void* _temp109= _temp106; _LL111: goto _LL112;
_LL113: goto _LL114; _LL112: goto _LL110; _LL114:( void) _throw( _temp109);
_LL110:;}}} if( f ==  0){ return 0;} else{ Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( f)); return 1;}} static struct _tagged_arr Cyc_sprint_list( struct
Cyc_List_List* dirs){ struct _tagged_arr tmp= _tag_arr("", sizeof( unsigned char),
1u); for( 0; dirs !=  0; dirs=(( struct Cyc_List_List*) _check_null( dirs))->tl){
struct _tagged_arr _temp115=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( dirs))->hd); if( _temp115.curr == (( struct _tagged_arr) _tag_arr(
0u, 0u, 0u)).curr? 1: Cyc_Std_strlen( _temp115) ==  0){ continue;} _temp115=(
struct _tagged_arr) Cyc_Std_strconcat( _temp115, _tag_arr(":", sizeof(
unsigned char), 2u)); tmp=( struct _tagged_arr) Cyc_Std_strconcat( _temp115, tmp);}
return tmp;} static struct _tagged_arr* Cyc_find( struct Cyc_List_List* dirs,
struct _tagged_arr file){ if( file.curr == (( struct _tagged_arr) _tag_arr( 0u,
0u, 0u)).curr){ return 0;} for( 0; dirs !=  0; dirs=(( struct Cyc_List_List*)
_check_null( dirs))->tl){ struct _tagged_arr _temp116=*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( dirs))->hd); if( _temp116.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: Cyc_Std_strlen( _temp116) ==  0){
continue;}{ struct _tagged_arr s=( struct _tagged_arr) Cyc_Filename_concat(
_temp116, file); if( Cyc_file_exists( s)){ return({ struct _tagged_arr* _temp117=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp117[ 0]= s;
_temp117;});}}} return 0;} static struct _tagged_arr Cyc_do_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ struct _tagged_arr* _temp118= Cyc_find( dirs,
file); if( _temp118 ==  0){({ struct Cyc_Std_String_pa_struct _temp121; _temp121.tag=
Cyc_Std_String_pa; _temp121.f1=( struct _tagged_arr) Cyc_sprint_list( dirs);{
struct Cyc_Std_String_pa_struct _temp120; _temp120.tag= Cyc_Std_String_pa;
_temp120.f1=( struct _tagged_arr) file;{ void* _temp119[ 2u]={& _temp120,&
_temp121}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: can't find internal compiler file %s in path %s\n",
sizeof( unsigned char), 56u), _tag_arr( _temp119, sizeof( void*), 2u));}}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); exit( 1);} return*(( struct _tagged_arr*) _check_null(
_temp118));} static int Cyc_is_other_special( unsigned char c){ switch( c){ case
'\\': _LL122: goto _LL123; case '"': _LL123: goto _LL124; case ';': _LL124: goto
_LL125; case '&': _LL125: goto _LL126; case '(': _LL126: goto _LL127; case ')':
_LL127: goto _LL128; case '|': _LL128: goto _LL129; case '^': _LL129: goto
_LL130; case '<': _LL130: goto _LL131; case '>': _LL131: goto _LL132; case ' ':
_LL132: goto _LL133; case '\n': _LL133: goto _LL134; case '\t': _LL134: return 1;
default: _LL135: return 0;}} static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){ unsigned int _temp137= Cyc_Std_strlen( s); int _temp138=
0; int _temp139= 0;{ int i= 0; for( 0; i <  _temp137; i ++){ unsigned char
_temp140=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp140 == '\''){ _temp138 ++;} else{ if( Cyc_is_other_special(
_temp140)){ _temp139 ++;}}}} if( _temp138 ==  0? _temp139 ==  0: 0){ return s;}
if( _temp138 ==  0){ return( struct _tagged_arr) Cyc_Std_strconcat_l(({ struct
_tagged_arr* _temp141[ 3u]; _temp141[ 2u]= _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"'", sizeof( unsigned char), 2u);
_temp141[ 1u]=({ struct _tagged_arr* _temp142=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp142[ 0]=( struct _tagged_arr) s; _temp142;});
_temp141[ 0u]= _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"'", sizeof( unsigned char), 2u);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp141, sizeof( struct
_tagged_arr*), 3u));}));}{ unsigned int _temp143=( _temp137 +  _temp138) + 
_temp139; struct _tagged_arr _temp144=({ unsigned int _temp148= _temp143 +  1;
unsigned char* _temp149=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp148)); struct _tagged_arr _temp151= _tag_arr(
_temp149, sizeof( unsigned char), _temp143 +  1);{ unsigned int _temp150=
_temp148; unsigned int i; for( i= 0; i <  _temp150; i ++){ _temp149[ i]='\000';}};
_temp151;}); int _temp145= 0; int _temp146= 0; for( 0; _temp145 <  _temp137;
_temp145 ++){ unsigned char _temp147=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), _temp145)); if( _temp147 == '\''?
1: Cyc_is_other_special( _temp147)){*(( unsigned char*) _check_unknown_subscript(
_temp144, sizeof( unsigned char), _temp146 ++))='\\';}*(( unsigned char*)
_check_unknown_subscript( _temp144, sizeof( unsigned char), _temp146 ++))=
_temp147;} return( struct _tagged_arr) _temp144;}} static struct _tagged_arr*
Cyc_sh_escape_stringptr( struct _tagged_arr* sp){ return({ struct _tagged_arr*
_temp152=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp152[ 0]= Cyc_sh_escape_string(* sp); _temp152;});} static void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp153= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp154= Cyc_Std_strconcat(( struct _tagged_arr)
_temp153, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct _tagged_arr
_temp155= Cyc_Std_strconcat(( struct _tagged_arr) _temp153, _tag_arr(".cyci",
sizeof( unsigned char), 6u)); struct _tagged_arr _temp156= Cyc_Std_strconcat((
struct _tagged_arr) _temp153, _tag_arr(".cycio", sizeof( unsigned char), 7u));
struct _tagged_arr _temp157= Cyc_Std_strconcat(( struct _tagged_arr) _temp153,
_tag_arr(".c", sizeof( unsigned char), 3u)); if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct
_temp159; _temp159.tag= Cyc_Std_String_pa; _temp159.f1=( struct _tagged_arr)
filename;{ void* _temp158[ 1u]={& _temp159}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Compiling %s\n", sizeof( unsigned char), 14u), _tag_arr( _temp158,
sizeof( void*), 1u));}});}{ struct Cyc_Std___sFILE* f0= Cyc_try_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("input file",
sizeof( unsigned char), 11u)); if( Cyc_compile_failure){ return;} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( f0));{ struct _tagged_arr _temp160= Cyc_Std_str_sepstr(({
struct Cyc_List_List* _temp214=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp214->hd=( void*) _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp214->tl=(( struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct
_tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs));
_temp214;}), _tag_arr(" ", sizeof( unsigned char), 2u)); struct _tagged_arr
stdinc_string; struct _tagged_arr def_inc_path=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path); if( Cyc_Std_strlen( def_inc_path) >  0){
stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp162;
_temp162.tag= Cyc_Std_String_pa; _temp162.f1=( struct _tagged_arr) def_inc_path;{
void* _temp161[ 1u]={& _temp162}; Cyc_Std_aprintf( _tag_arr(" -I%s", sizeof(
unsigned char), 6u), _tag_arr( _temp161, sizeof( void*), 1u));}});} else{
stdinc_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct _tagged_arr
_temp163= Cyc_Std_getenv( _tag_arr("CYCLONE_INCLUDE_PATH", sizeof( unsigned char),
21u)); if( _temp163.curr != ( _tag_arr( 0u, 0u, 0u)).curr){ struct Cyc_List_List*
_temp164= Cyc_split_by_char(( struct _tagged_arr) _temp163,':'); struct
_tagged_arr _temp165= Cyc_Std_str_sepstr(({ struct Cyc_List_List* _temp169=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp169->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"", sizeof( unsigned char), 1u); _temp169->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, _temp164); _temp169;}), _tag_arr(" -I", sizeof(
unsigned char), 4u)); stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp168; _temp168.tag= Cyc_Std_String_pa; _temp168.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp167; _temp167.tag= Cyc_Std_String_pa;
_temp167.f1=( struct _tagged_arr) _temp165;{ void* _temp166[ 2u]={& _temp167,&
_temp168}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp166, sizeof( void*), 2u));}}});}{ struct _tagged_arr ofile_string;
if( Cyc_stop_after_cpp_r){ if( Cyc_output_file !=  0){ ofile_string=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp171; _temp171.tag= Cyc_Std_String_pa;
_temp171.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_output_file));{
void* _temp170[ 1u]={& _temp171}; Cyc_Std_aprintf( _tag_arr(" > %s", sizeof(
unsigned char), 6u), _tag_arr( _temp170, sizeof( void*), 1u));}});} else{
ofile_string= _tag_arr("", sizeof( unsigned char), 1u);}} else{ ofile_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp173; _temp173.tag=
Cyc_Std_String_pa; _temp173.f1=( struct _tagged_arr) Cyc_sh_escape_string((
struct _tagged_arr) _temp154);{ void* _temp172[ 1u]={& _temp173}; Cyc_Std_aprintf(
_tag_arr(" > %s", sizeof( unsigned char), 6u), _tag_arr( _temp172, sizeof( void*),
1u));}});}{ struct _tagged_arr fixup_string; if( Cyc_produce_dependencies){ if(
Cyc_dependencies_target ==  0){ fixup_string= _tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof( unsigned char), 35u);} else{ fixup_string=( struct _tagged_arr)({ struct
Cyc_Std_String_pa_struct _temp175; _temp175.tag= Cyc_Std_String_pa; _temp175.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_dependencies_target));{
void* _temp174[ 1u]={& _temp175}; Cyc_Std_aprintf( _tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",
sizeof( unsigned char), 29u), _tag_arr( _temp174, sizeof( void*), 1u));}});}}
else{ fixup_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct
_tagged_arr arch=( struct _tagged_arr) Cstring_to_string( Carch); struct
_tagged_arr _temp176=({ struct Cyc_Std_String_pa_struct _temp213; _temp213.tag=
Cyc_Std_String_pa; _temp213.f1=( struct _tagged_arr) ofile_string;{ struct Cyc_Std_String_pa_struct
_temp212; _temp212.tag= Cyc_Std_String_pa; _temp212.f1=( struct _tagged_arr)
fixup_string;{ struct Cyc_Std_String_pa_struct _temp211; _temp211.tag= Cyc_Std_String_pa;
_temp211.f1=( struct _tagged_arr) Cyc_sh_escape_string( filename);{ struct Cyc_Std_String_pa_struct
_temp210; _temp210.tag= Cyc_Std_String_pa; _temp210.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp209; _temp209.tag= Cyc_Std_String_pa;
_temp209.f1=( struct _tagged_arr) _temp160;{ struct Cyc_Std_String_pa_struct
_temp208; _temp208.tag= Cyc_Std_String_pa; _temp208.f1=( struct _tagged_arr)
arch;{ struct Cyc_Std_String_pa_struct _temp207; _temp207.tag= Cyc_Std_String_pa;
_temp207.f1=( struct _tagged_arr) Cyc_cpp;{ void* _temp206[ 7u]={& _temp207,&
_temp208,& _temp209,& _temp210,& _temp211,& _temp212,& _temp213}; Cyc_Std_aprintf(
_tag_arr("%s '-D_PLATFORM_INCLUDES_=\"arch/%s.h\"'%s%s %s%s%s", sizeof(
unsigned char), 50u), _tag_arr( _temp206, sizeof( void*), 7u));}}}}}}}}); if(
Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp178; _temp178.tag= Cyc_Std_String_pa;
_temp178.f1=( struct _tagged_arr) _temp176;{ void* _temp177[ 1u]={& _temp178};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp177, sizeof( void*), 1u));}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp176)) !=  0){ Cyc_compile_failure= 1;({ void* _temp179[
0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: preprocessing\n",
sizeof( unsigned char), 23u), _tag_arr( _temp179, sizeof( void*), 0u));});
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp154);{ struct Cyc_Std___sFILE* in_file= Cyc_try_file_open((
struct _tagged_arr) _temp154, _tag_arr("r", sizeof( unsigned char), 2u),
_tag_arr("file", sizeof( unsigned char), 5u)); if( Cyc_compile_failure){ return;}{
struct Cyc_List_List* tds= 0;{ struct _handler_cons _temp180; _push_handler(&
_temp180);{ int _temp182= 0; if( setjmp( _temp180.handler)){ _temp182= 1;} if( !
_temp182){ tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*),
struct Cyc_Std___sFILE* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("parsing",
sizeof( unsigned char), 8u), Cyc_do_parse,( struct Cyc_Std___sFILE*) _check_null(
in_file), tds);; _pop_handler();} else{ void* _temp181=( void*) _exn_thrown;
void* _temp184= _temp181; _LL186: goto _LL187; _LL188: goto _LL189; _LL187: Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( in_file)); Cyc_remove_file(( struct
_tagged_arr) _temp154);( int) _throw( _temp184); _LL189:( void) _throw( _temp184);
_LL185:;}}} Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( in_file));
if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr) _temp154);
return;}{ struct Cyc_Tcenv_Tenv* _temp190= Cyc_Tcenv_tc_init(); if( Cyc_parseonly_r){
goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("type checking",
sizeof( unsigned char), 14u), Cyc_do_typecheck, _temp190, tds); if( Cyc_compile_failure){
Cyc_remove_file(( struct _tagged_arr) _temp154); return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("control-flow checking",
sizeof( unsigned char), 22u), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){
Cyc_Tcutil_flush_warnings();} Cyc_remove_file(( struct _tagged_arr) _temp154);
if( Cyc_compile_failure){ return;} if( Cyc_ic_r){ struct Cyc_Std___sFILE*
inter_file= Cyc_Std_fopen(( struct _tagged_arr) _temp155, _tag_arr("r", sizeof(
unsigned char), 2u)); struct Cyc_Std___sFILE* inter_objfile= Cyc_try_file_open((
struct _tagged_arr) _temp156, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("interface object file", sizeof( unsigned char), 22u)); if(
inter_objfile ==  0){ Cyc_compile_failure= 1; return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp155); tds=(( struct Cyc_List_List*(*)( struct
_tagged_arr stage_name, struct Cyc_List_List*(* f)( struct _tuple9*, struct Cyc_List_List*),
struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("interface checking",
sizeof( unsigned char), 19u), Cyc_do_interface,({ struct _tuple9* _temp191=(
struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp191->f1= _temp190;
_temp191->f2= inter_file; _temp191->f3=( struct Cyc_Std___sFILE*) _check_null(
inter_objfile); _temp191;}), tds); if( inter_file !=  0){ Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( inter_file));} Cyc_Std_file_close(( struct
Cyc_Std___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto PRINTC;}
tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("translation to C", sizeof( unsigned char), 17u), Cyc_do_translate, 1,
tds); if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("post-pass to VC",
sizeof( unsigned char), 16u), Cyc_do_tovc, 1, tds);} if( Cyc_compile_failure){
return;} PRINTC: if( tds !=  0){ struct Cyc_Std___sFILE* out_file; if( Cyc_parseonly_r?
1: Cyc_tc_r){ if( Cyc_output_file !=  0){ out_file= Cyc_try_file_open(*(( struct
_tagged_arr*) _check_null( Cyc_output_file)), _tag_arr("w", sizeof(
unsigned char), 2u), _tag_arr("output file", sizeof( unsigned char), 12u));}
else{ out_file=( struct Cyc_Std___sFILE*) Cyc_Std_stdout;}} else{ if( Cyc_toc_r?
Cyc_output_file !=  0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));} else{ out_file= Cyc_try_file_open((
struct _tagged_arr) _temp157, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));}} if( Cyc_compile_failure){
return;}{ struct _handler_cons _temp192; _push_handler(& _temp192);{ int
_temp194= 0; if( setjmp( _temp192.handler)){ _temp194= 1;} if( ! _temp194){ tds=((
struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*), struct Cyc_Std___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("printing", sizeof(
unsigned char), 9u), Cyc_do_print,( struct Cyc_Std___sFILE*) _check_null(
out_file), tds);; _pop_handler();} else{ void* _temp193=( void*) _exn_thrown;
void* _temp196= _temp193; _LL198: goto _LL199; _LL200: goto _LL201; _LL199: Cyc_compile_failure=
1; Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({
struct Cyc_List_List* _temp202=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp202->hd=( void*)({ struct _tagged_arr* _temp203=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp203[ 0]=(
struct _tagged_arr) _temp157; _temp203;}); _temp202->tl= Cyc_cfiles; _temp202;});(
int) _throw( _temp196); _LL201:( void) _throw( _temp196); _LL197:;}}} Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List*
_temp204=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp204->hd=( void*)({ struct _tagged_arr* _temp205=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp205[ 0]=( struct _tagged_arr)
_temp157; _temp205;}); _temp204->tl= Cyc_cfiles; _temp204;});}}}}}}}}}}} static
unsigned char _temp215[ 8u]="<final>"; static struct _tagged_arr Cyc_final_str={
_temp215, _temp215, _temp215 +  8u}; static struct _tagged_arr* Cyc_final_strptr=&
Cyc_final_str; static struct Cyc_Interface_I* Cyc_read_cycio( struct _tagged_arr*
n){ if( n == ( struct _tagged_arr*) Cyc_final_strptr){ return Cyc_Interface_final();}{
struct _tagged_arr basename;{ struct _handler_cons _temp216; _push_handler(&
_temp216);{ int _temp218= 0; if( setjmp( _temp216.handler)){ _temp218= 1;} if( !
_temp218){ basename=( struct _tagged_arr) Cyc_Filename_chop_extension(* n);;
_pop_handler();} else{ void* _temp217=( void*) _exn_thrown; void* _temp220=
_temp217; _LL222: if(*(( void**) _temp220) ==  Cyc_Core_Invalid_argument){ goto
_LL223;} else{ goto _LL224;} _LL224: goto _LL225; _LL223: basename=* n; goto
_LL221; _LL225:( void) _throw( _temp220); _LL221:;}}}{ struct _tagged_arr
_temp226= Cyc_Std_strconcat( basename, _tag_arr(".cycio", sizeof( unsigned char),
7u)); struct Cyc_Std___sFILE* _temp227= Cyc_try_file_open(( struct _tagged_arr)
_temp226, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp227 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} Cyc_Position_reset_position(( struct _tagged_arr) _temp226);{ struct
Cyc_Interface_I* _temp228= Cyc_Interface_load(( struct Cyc_Std___sFILE*)
_check_null( _temp227)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp227)); return _temp228;}}}} static int Cyc_is_cfile( struct
_tagged_arr* n){ return*(( const unsigned char*) _check_unknown_subscript(* n,
sizeof( unsigned char), 0)) != '-';} struct _tuple10{ struct _tagged_arr f1; int
f2; struct _tagged_arr f3; void* f4; struct _tagged_arr f5; } ; int Cyc_main(
int argc, struct _tagged_arr argv){ struct _tagged_arr comp=( struct _tagged_arr)
Cstring_to_string( Ccomp); Cyc_set_cpp(( struct _tagged_arr) Cyc_Std_strconcat(
comp, _tag_arr(" -x c -E -U__GNUC__ -nostdinc", sizeof( unsigned char), 30u)));{
struct _tagged_arr cflags=( struct _tagged_arr) Cstring_to_string( Ccflags); if(
Cyc_Std_strlen( cflags) !=  0){ Cyc_add_ccarg( cflags); Cyc_add_cpparg( cflags);}{
struct Cyc_List_List* options=({ struct _tuple10* _temp302[ 41u]; _temp302[ 40u]=({
struct _tuple10* _temp423=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp423->f1= _tag_arr("-printfullevars", sizeof( unsigned char), 16u); _temp423->f2=
0; _temp423->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp423->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp424=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp424[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp425; _temp425.tag= Cyc_Arg_Set_spec; _temp425.f1=& Cyc_print_full_evars_r;
_temp425;}); _temp424;}); _temp423->f5= _tag_arr("Print full information for evars (type debugging)",
sizeof( unsigned char), 50u); _temp423;}); _temp302[ 39u]=({ struct _tuple10*
_temp420=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp420->f1=
_tag_arr("-printallkinds", sizeof( unsigned char), 15u); _temp420->f2= 0;
_temp420->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp420->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp421=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp421[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp422; _temp422.tag= Cyc_Arg_Set_spec; _temp422.f1=& Cyc_print_all_kinds_r;
_temp422;}); _temp421;}); _temp420->f5= _tag_arr("Always print kinds of type variables",
sizeof( unsigned char), 37u); _temp420;}); _temp302[ 38u]=({ struct _tuple10*
_temp417=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp417->f1=
_tag_arr("-printalltvars", sizeof( unsigned char), 15u); _temp417->f2= 0;
_temp417->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp417->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp418=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp418[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp419; _temp419.tag= Cyc_Arg_Set_spec; _temp419.f1=& Cyc_print_all_tvars_r;
_temp419;}); _temp418;}); _temp417->f5= _tag_arr("Print all type variables (even implicit default effects)",
sizeof( unsigned char), 57u); _temp417;}); _temp302[ 37u]=({ struct _tuple10*
_temp414=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp414->f1=
_tag_arr("-noexpandtypedefs", sizeof( unsigned char), 18u); _temp414->f2= 0;
_temp414->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp414->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp415=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp415[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp416; _temp416.tag= Cyc_Arg_Set_spec; _temp416.f1=& Cyc_noexpand_r; _temp416;});
_temp415;}); _temp414->f5= _tag_arr("Don't expand typedefs in pretty printing",
sizeof( unsigned char), 41u); _temp414;}); _temp302[ 36u]=({ struct _tuple10*
_temp411=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp411->f1=
_tag_arr("-noremoveunused", sizeof( unsigned char), 16u); _temp411->f2= 0;
_temp411->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp411->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp412=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp412[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp413; _temp413.tag= Cyc_Arg_Set_spec; _temp413.f1=& Cyc_noshake_r; _temp413;});
_temp412;}); _temp411->f5= _tag_arr("Don't remove externed variables that aren't used",
sizeof( unsigned char), 49u); _temp411;}); _temp302[ 35u]=({ struct _tuple10*
_temp408=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp408->f1=
_tag_arr("-nogc", sizeof( unsigned char), 6u); _temp408->f2= 0; _temp408->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp408->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp409=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp409[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp410; _temp410.tag= Cyc_Arg_Set_spec;
_temp410.f1=& Cyc_nogc_r; _temp410;}); _temp409;}); _temp408->f5= _tag_arr("Don't link in the garbage collector",
sizeof( unsigned char), 36u); _temp408;}); _temp302[ 34u]=({ struct _tuple10*
_temp405=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp405->f1=
_tag_arr("-nocyc", sizeof( unsigned char), 7u); _temp405->f2= 0; _temp405->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp405->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp406=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp406[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp407; _temp407.tag= Cyc_Arg_Unit_spec;
_temp407.f1= Cyc_set_nocyc; _temp407;}); _temp406;}); _temp405->f5= _tag_arr("Don't add implicit namespace Cyc",
sizeof( unsigned char), 33u); _temp405;}); _temp302[ 33u]=({ struct _tuple10*
_temp402=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp402->f1=
_tag_arr("-no-cpp-precomp", sizeof( unsigned char), 16u); _temp402->f2= 0;
_temp402->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp402->f4=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp403=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp403[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp404; _temp404.tag= Cyc_Arg_Unit_spec; _temp404.f1= Cyc_set_nocppprecomp;
_temp404;}); _temp403;}); _temp402->f5= _tag_arr("Don't do smart preprocessing (mac only)",
sizeof( unsigned char), 40u); _temp402;}); _temp302[ 32u]=({ struct _tuple10*
_temp399=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp399->f1=
_tag_arr("-use-cpp", sizeof( unsigned char), 9u); _temp399->f2= 0; _temp399->f3=
_tag_arr("<path>", sizeof( unsigned char), 7u); _temp399->f4=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp400=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp400[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp401; _temp401.tag= Cyc_Arg_String_spec; _temp401.f1=
Cyc_set_cpp; _temp401;}); _temp400;}); _temp399->f5= _tag_arr("Indicate which preprocessor to use",
sizeof( unsigned char), 35u); _temp399;}); _temp302[ 31u]=({ struct _tuple10*
_temp396=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp396->f1=
_tag_arr("-save-c", sizeof( unsigned char), 8u); _temp396->f2= 0; _temp396->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp396->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp397=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp397[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp398; _temp398.tag= Cyc_Arg_Set_spec;
_temp398.f1=& Cyc_save_c_r; _temp398;}); _temp397;}); _temp396->f5= _tag_arr("Don't delete temporary C files",
sizeof( unsigned char), 31u); _temp396;}); _temp302[ 30u]=({ struct _tuple10*
_temp393=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp393->f1=
_tag_arr("-save-temps", sizeof( unsigned char), 12u); _temp393->f2= 0; _temp393->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp393->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp394=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp394[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp395; _temp395.tag= Cyc_Arg_Unit_spec;
_temp395.f1= Cyc_set_save_temps; _temp395;}); _temp394;}); _temp393->f5=
_tag_arr("Don't delete temporary files", sizeof( unsigned char), 29u); _temp393;});
_temp302[ 29u]=({ struct _tuple10* _temp390=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp390->f1= _tag_arr("-tovc", sizeof( unsigned char),
6u); _temp390->f2= 0; _temp390->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp390->f4=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp391=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp391[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp392; _temp392.tag= Cyc_Arg_Set_spec; _temp392.f1=& Cyc_tovc_r; _temp392;});
_temp391;}); _temp390->f5= _tag_arr("Avoid gcc extensions in C output", sizeof(
unsigned char), 33u); _temp390;}); _temp302[ 28u]=({ struct _tuple10* _temp387=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp387->f1= _tag_arr("-up",
sizeof( unsigned char), 4u); _temp387->f2= 0; _temp387->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp387->f4=( void*)({ struct Cyc_Arg_Clear_spec_struct*
_temp388=( struct Cyc_Arg_Clear_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Clear_spec_struct));
_temp388[ 0]=({ struct Cyc_Arg_Clear_spec_struct _temp389; _temp389.tag= Cyc_Arg_Clear_spec;
_temp389.f1=& Cyc_pp_r; _temp389;}); _temp388;}); _temp387->f5= _tag_arr("Ugly print",
sizeof( unsigned char), 11u); _temp387;}); _temp302[ 27u]=({ struct _tuple10*
_temp384=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp384->f1=
_tag_arr("-pp", sizeof( unsigned char), 4u); _temp384->f2= 0; _temp384->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp384->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp385=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp385[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp386; _temp386.tag= Cyc_Arg_Set_spec;
_temp386.f1=& Cyc_pp_r; _temp386;}); _temp385;}); _temp384->f5= _tag_arr("Pretty print",
sizeof( unsigned char), 13u); _temp384;}); _temp302[ 26u]=({ struct _tuple10*
_temp381=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp381->f1=
_tag_arr("-ic", sizeof( unsigned char), 4u); _temp381->f2= 0; _temp381->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp381->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp382=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp382[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp383; _temp383.tag= Cyc_Arg_Set_spec;
_temp383.f1=& Cyc_ic_r; _temp383;}); _temp382;}); _temp381->f5= _tag_arr("Activate the link-checker",
sizeof( unsigned char), 26u); _temp381;}); _temp302[ 25u]=({ struct _tuple10*
_temp378=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp378->f1=
_tag_arr("-stopafter-toc", sizeof( unsigned char), 15u); _temp378->f2= 0;
_temp378->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp378->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp379=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp379[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp380; _temp380.tag= Cyc_Arg_Set_spec; _temp380.f1=& Cyc_toc_r; _temp380;});
_temp379;}); _temp378->f5= _tag_arr("Stop after translation to C", sizeof(
unsigned char), 28u); _temp378;}); _temp302[ 24u]=({ struct _tuple10* _temp375=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp375->f1= _tag_arr("-stopafter-tc",
sizeof( unsigned char), 14u); _temp375->f2= 0; _temp375->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp375->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp376=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp376[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp377; _temp377.tag= Cyc_Arg_Set_spec;
_temp377.f1=& Cyc_tc_r; _temp377;}); _temp376;}); _temp375->f5= _tag_arr("Stop after type checking",
sizeof( unsigned char), 25u); _temp375;}); _temp302[ 23u]=({ struct _tuple10*
_temp372=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp372->f1=
_tag_arr("-stopafter-parse", sizeof( unsigned char), 17u); _temp372->f2= 0;
_temp372->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp372->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp373=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp373[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp374; _temp374.tag= Cyc_Arg_Set_spec; _temp374.f1=& Cyc_parseonly_r;
_temp374;}); _temp373;}); _temp372->f5= _tag_arr("Stop after parsing", sizeof(
unsigned char), 19u); _temp372;}); _temp302[ 22u]=({ struct _tuple10* _temp369=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp369->f1= _tag_arr("-E",
sizeof( unsigned char), 3u); _temp369->f2= 0; _temp369->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp369->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp370=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp370[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp371; _temp371.tag= Cyc_Arg_Set_spec;
_temp371.f1=& Cyc_stop_after_cpp_r; _temp371;}); _temp370;}); _temp369->f5=
_tag_arr("Stop after preprocessing", sizeof( unsigned char), 25u); _temp369;});
_temp302[ 21u]=({ struct _tuple10* _temp366=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp366->f1= _tag_arr("-MT", sizeof( unsigned char),
4u); _temp366->f2= 0; _temp366->f3= _tag_arr(" <target>", sizeof( unsigned char),
10u); _temp366->f4=( void*)({ struct Cyc_Arg_String_spec_struct* _temp367=(
struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp367[ 0]=({ struct Cyc_Arg_String_spec_struct _temp368; _temp368.tag= Cyc_Arg_String_spec;
_temp368.f1= Cyc_set_dependencies_target; _temp368;}); _temp367;}); _temp366->f5=
_tag_arr("Give target for dependencies", sizeof( unsigned char), 29u); _temp366;});
_temp302[ 20u]=({ struct _tuple10* _temp363=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp363->f1= _tag_arr("-MG", sizeof( unsigned char),
4u); _temp363->f2= 0; _temp363->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp363->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct* _temp364=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp364[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp365; _temp365.tag= Cyc_Arg_Flag_spec; _temp365.f1= Cyc_add_cpparg; _temp365;});
_temp364;}); _temp363->f5= _tag_arr("When producing dependencies assume that missing files are generated",
sizeof( unsigned char), 68u); _temp363;}); _temp302[ 19u]=({ struct _tuple10*
_temp360=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp360->f1=
_tag_arr("-M", sizeof( unsigned char), 3u); _temp360->f2= 0; _temp360->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp360->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp361=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp361[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp362; _temp362.tag= Cyc_Arg_Unit_spec;
_temp362.f1= Cyc_set_produce_dependencies; _temp362;}); _temp361;}); _temp360->f5=
_tag_arr("Produce dependencies", sizeof( unsigned char), 21u); _temp360;});
_temp302[ 18u]=({ struct _tuple10* _temp357=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp357->f1= _tag_arr("-S", sizeof( unsigned char),
3u); _temp357->f2= 0; _temp357->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp357->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct* _temp358=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp358[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp359; _temp359.tag= Cyc_Arg_Unit_spec; _temp359.f1= Cyc_set_stop_after_asmfile;
_temp359;}); _temp358;}); _temp357->f5= _tag_arr("Stop after producing assembly code",
sizeof( unsigned char), 35u); _temp357;}); _temp302[ 17u]=({ struct _tuple10*
_temp354=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp354->f1=
_tag_arr("-pa", sizeof( unsigned char), 4u); _temp354->f2= 0; _temp354->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp354->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp355=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp355[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp356; _temp356.tag= Cyc_Arg_Unit_spec;
_temp356.f1= Cyc_set_pa; _temp356;}); _temp355;}); _temp354->f5= _tag_arr("Compile for profiling with aprof",
sizeof( unsigned char), 33u); _temp354;}); _temp302[ 16u]=({ struct _tuple10*
_temp351=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp351->f1=
_tag_arr("-pg", sizeof( unsigned char), 4u); _temp351->f2= 0; _temp351->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp351->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp352=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp352[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp353; _temp353.tag= Cyc_Arg_Flag_spec;
_temp353.f1= Cyc_add_ccarg; _temp353;}); _temp352;}); _temp351->f5= _tag_arr("Compile for profiling with gprof",
sizeof( unsigned char), 33u); _temp351;}); _temp302[ 15u]=({ struct _tuple10*
_temp348=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp348->f1=
_tag_arr("-p", sizeof( unsigned char), 3u); _temp348->f2= 0; _temp348->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp348->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp349=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp349[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp350; _temp350.tag= Cyc_Arg_Flag_spec;
_temp350.f1= Cyc_add_ccarg; _temp350;}); _temp349;}); _temp348->f5= _tag_arr("Compile for profiling with prof",
sizeof( unsigned char), 32u); _temp348;}); _temp302[ 14u]=({ struct _tuple10*
_temp345=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp345->f1=
_tag_arr("-g", sizeof( unsigned char), 3u); _temp345->f2= 0; _temp345->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp345->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp346=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp346[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp347; _temp347.tag= Cyc_Arg_Flag_spec;
_temp347.f1= Cyc_add_ccarg; _temp347;}); _temp346;}); _temp345->f5= _tag_arr("Compile for debugging",
sizeof( unsigned char), 22u); _temp345;}); _temp302[ 13u]=({ struct _tuple10*
_temp342=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp342->f1=
_tag_arr("-O3", sizeof( unsigned char), 4u); _temp342->f2= 0; _temp342->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp342->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp343=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp343[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp344; _temp344.tag= Cyc_Arg_Flag_spec;
_temp344.f1= Cyc_add_ccarg; _temp344;}); _temp343;}); _temp342->f5= _tag_arr("Even more optimization",
sizeof( unsigned char), 23u); _temp342;}); _temp302[ 12u]=({ struct _tuple10*
_temp339=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp339->f1=
_tag_arr("-O2", sizeof( unsigned char), 4u); _temp339->f2= 0; _temp339->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp339->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp340=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp340[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp341; _temp341.tag= Cyc_Arg_Flag_spec;
_temp341.f1= Cyc_add_ccarg; _temp341;}); _temp340;}); _temp339->f5= _tag_arr("A higher level of optimization",
sizeof( unsigned char), 31u); _temp339;}); _temp302[ 11u]=({ struct _tuple10*
_temp336=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp336->f1=
_tag_arr("-O", sizeof( unsigned char), 3u); _temp336->f2= 0; _temp336->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp336->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp337=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp337[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp338; _temp338.tag= Cyc_Arg_Flag_spec;
_temp338.f1= Cyc_add_ccarg; _temp338;}); _temp337;}); _temp336->f5= _tag_arr("Optimize",
sizeof( unsigned char), 9u); _temp336;}); _temp302[ 10u]=({ struct _tuple10*
_temp333=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp333->f1=
_tag_arr("-s", sizeof( unsigned char), 3u); _temp333->f2= 0; _temp333->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp333->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp334=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp334[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp335; _temp335.tag= Cyc_Arg_Flag_spec;
_temp335.f1= Cyc_add_ccarg; _temp335;}); _temp334;}); _temp333->f5= _tag_arr("Remove all symbol table and relocation info from executable",
sizeof( unsigned char), 60u); _temp333;}); _temp302[ 9u]=({ struct _tuple10*
_temp330=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp330->f1=
_tag_arr("-x", sizeof( unsigned char), 3u); _temp330->f2= 0; _temp330->f3=
_tag_arr(" <language>", sizeof( unsigned char), 12u); _temp330->f4=( void*)({
struct Cyc_Arg_String_spec_struct* _temp331=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp331[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp332; _temp332.tag= Cyc_Arg_String_spec; _temp332.f1=
Cyc_set_inputtype; _temp332;}); _temp331;}); _temp330->f5= _tag_arr("Specify <language> for the following input files",
sizeof( unsigned char), 49u); _temp330;}); _temp302[ 8u]=({ struct _tuple10*
_temp327=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp327->f1=
_tag_arr("-c", sizeof( unsigned char), 3u); _temp327->f2= 0; _temp327->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp327->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp328=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp328[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp329; _temp329.tag= Cyc_Arg_Unit_spec;
_temp329.f1= Cyc_set_stop_after_objectfile; _temp329;}); _temp328;}); _temp327->f5=
_tag_arr("Produce an object file instead of an executable; do not link", sizeof(
unsigned char), 61u); _temp327;}); _temp302[ 7u]=({ struct _tuple10* _temp324=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp324->f1= _tag_arr("-l",
sizeof( unsigned char), 3u); _temp324->f2= 1; _temp324->f3= _tag_arr("<file>",
sizeof( unsigned char), 7u); _temp324->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp325=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp325[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp326; _temp326.tag= Cyc_Arg_Flag_spec;
_temp326.f1= Cyc_add_libarg; _temp326;}); _temp325;}); _temp324->f5= _tag_arr("Library file",
sizeof( unsigned char), 13u); _temp324;}); _temp302[ 6u]=({ struct _tuple10*
_temp321=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp321->f1=
_tag_arr("-L", sizeof( unsigned char), 3u); _temp321->f2= 1; _temp321->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp321->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp322=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp322[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp323; _temp323.tag= Cyc_Arg_Flag_spec; _temp323.f1= Cyc_add_cpparg; _temp323;});
_temp322;}); _temp321->f5= _tag_arr("Add to the list of directories for -l",
sizeof( unsigned char), 38u); _temp321;}); _temp302[ 5u]=({ struct _tuple10*
_temp318=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp318->f1=
_tag_arr("-I", sizeof( unsigned char), 3u); _temp318->f2= 1; _temp318->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp318->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp319=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp319[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp320; _temp320.tag= Cyc_Arg_Flag_spec; _temp320.f1= Cyc_add_cpparg; _temp320;});
_temp319;}); _temp318->f5= _tag_arr("Add to the list of directories to search for include files",
sizeof( unsigned char), 59u); _temp318;}); _temp302[ 4u]=({ struct _tuple10*
_temp315=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp315->f1=
_tag_arr("-B", sizeof( unsigned char), 3u); _temp315->f2= 1; _temp315->f3=
_tag_arr("<file>", sizeof( unsigned char), 7u); _temp315->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp316=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp316[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp317; _temp317.tag= Cyc_Arg_Flag_spec; _temp317.f1= Cyc_add_cyclone_lib_path;
_temp317;}); _temp316;}); _temp315->f5= _tag_arr("Add to the list of directories to search for compiler files",
sizeof( unsigned char), 60u); _temp315;}); _temp302[ 3u]=({ struct _tuple10*
_temp312=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp312->f1=
_tag_arr("-D", sizeof( unsigned char), 3u); _temp312->f2= 1; _temp312->f3=
_tag_arr("<name>[=<value>]", sizeof( unsigned char), 17u); _temp312->f4=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp313=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp313[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp314; _temp314.tag= Cyc_Arg_Flag_spec; _temp314.f1= Cyc_add_cpparg; _temp314;});
_temp313;}); _temp312->f5= _tag_arr("Pass definition to preprocessor", sizeof(
unsigned char), 32u); _temp312;}); _temp302[ 2u]=({ struct _tuple10* _temp309=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp309->f1= _tag_arr("-o",
sizeof( unsigned char), 3u); _temp309->f2= 0; _temp309->f3= _tag_arr(" <file>",
sizeof( unsigned char), 8u); _temp309->f4=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp310=( struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp310[ 0]=({ struct Cyc_Arg_String_spec_struct _temp311; _temp311.tag= Cyc_Arg_String_spec;
_temp311.f1= Cyc_set_output_file; _temp311;}); _temp310;}); _temp309->f5=
_tag_arr("Set the output file name to <file>", sizeof( unsigned char), 35u);
_temp309;}); _temp302[ 1u]=({ struct _tuple10* _temp306=( struct _tuple10*)
_cycalloc( sizeof( struct _tuple10)); _temp306->f1= _tag_arr("--version",
sizeof( unsigned char), 10u); _temp306->f2= 0; _temp306->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp306->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp307=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp307[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp308; _temp308.tag= Cyc_Arg_Unit_spec;
_temp308.f1= Cyc_print_version; _temp308;}); _temp307;}); _temp306->f5= _tag_arr("Print version number and exit",
sizeof( unsigned char), 30u); _temp306;}); _temp302[ 0u]=({ struct _tuple10*
_temp303=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp303->f1=
_tag_arr("-v", sizeof( unsigned char), 3u); _temp303->f2= 0; _temp303->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp303->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp304=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp304[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp305; _temp305.tag= Cyc_Arg_Set_spec;
_temp305.f1=& Cyc_v_r; _temp305;}); _temp304;}); _temp303->f5= _tag_arr("Print compilation stages verbosely",
sizeof( unsigned char), 35u); _temp303;});(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp302, sizeof( struct _tuple10*), 41u));});
Cyc_Arg_parse( options, Cyc_add_other, _tag_arr("Options:", sizeof(
unsigned char), 9u), argv);{ struct _tagged_arr cyclone_exec_prefix=( struct
_tagged_arr) Cyc_Std_getenv( _tag_arr("CYCLONE_EXEC_PREFIX", sizeof(
unsigned char), 20u)); if( cyclone_exec_prefix.curr != (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr){ Cyc_cyclone_lib_path=({ struct Cyc_List_List*
_temp229=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp229->hd=( void*)({ struct _tagged_arr* _temp230=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp230[ 0]= cyclone_exec_prefix;
_temp230;}); _temp229->tl= Cyc_cyclone_lib_path; _temp229;});}{ struct
_tagged_arr def_lib_path=( struct _tagged_arr) Cstring_to_string( Cdef_lib_path);
if( Cyc_Std_strlen( def_lib_path) >  0){ Cyc_cyclone_lib_path=({ struct Cyc_List_List*
_temp231=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp231->hd=( void*)({ struct _tagged_arr* _temp232=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp232[ 0]= def_lib_path; _temp232;});
_temp231->tl= Cyc_cyclone_lib_path; _temp231;});} Cyc_cyclone_lib_path=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_lib_path);
Cyc_cyc_include= Cyc_do_find( Cyc_cyclone_lib_path, _tag_arr("include/cyc_include.h",
sizeof( unsigned char), 22u));{ struct Cyc_List_List* _temp233=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); for( 0; _temp233 != 
0; _temp233=(( struct Cyc_List_List*) _check_null( _temp233))->tl){ Cyc_process_file(*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp233))->hd)); if(
Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)?
1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs ==  0){ return 0;} Cyc_ccargs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{
struct _tagged_arr _temp234= Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_ccargs), _tag_arr(" ", sizeof( unsigned char), 2u));
Cyc_libargs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
Cyc_libargs);{ struct _tagged_arr _temp235= Cyc_Std_str_sepstr(({ struct Cyc_List_List*
_temp301=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp301->hd=( void*) _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)),"", sizeof( unsigned char), 1u); _temp301->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_libargs); _temp301;}), _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_List_List* stdlib; struct _tagged_arr
stdlib_string; int _temp236=(( Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".a", sizeof( unsigned char), 3u)): 0))?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".lib", sizeof( unsigned char), 5u)): 0);
if( _temp236){ stdlib= 0; stdlib_string= _tag_arr("", sizeof( unsigned char), 1u);}
else{ struct _tagged_arr libcyc_filename= Cyc_pa_r? _tag_arr("libcyc_a.a",
sizeof( unsigned char), 11u): _tag_arr("libcyc.a", sizeof( unsigned char), 9u);
struct _tagged_arr nogc_filename= Cyc_pa_r? _tag_arr("nogc_a.a", sizeof(
unsigned char), 9u): _tag_arr("nogc.a", sizeof( unsigned char), 7u); struct
_tagged_arr gc_filename= _tag_arr("gc.a", sizeof( unsigned char), 5u); struct
_tagged_arr _temp237= Cyc_do_find( Cyc_cyclone_lib_path, libcyc_filename);
struct _tagged_arr _temp238= Cyc_nogc_r? Cyc_do_find( Cyc_cyclone_lib_path,
nogc_filename): Cyc_do_find( Cyc_cyclone_lib_path, gc_filename); stdlib=({
struct _tagged_arr* _temp239[ 1u]; _temp239[ 0u]=({ struct _tagged_arr* _temp240=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp240[ 0]=
_temp237; _temp240;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp239, sizeof( struct _tagged_arr*), 1u));}); stdlib_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp243; _temp243.tag=
Cyc_Std_String_pa; _temp243.f1=( struct _tagged_arr) _temp238;{ struct Cyc_Std_String_pa_struct
_temp242; _temp242.tag= Cyc_Std_String_pa; _temp242.f1=( struct _tagged_arr)
_temp237;{ void* _temp241[ 2u]={& _temp242,& _temp243}; Cyc_Std_aprintf(
_tag_arr(" %s %s", sizeof( unsigned char), 7u), _tag_arr( _temp241, sizeof( void*),
2u));}}});} if( Cyc_ic_r){ struct _handler_cons _temp244; _push_handler(&
_temp244);{ int _temp246= 0; if( setjmp( _temp244.handler)){ _temp246= 1;} if( !
_temp246){ Cyc_ccargs=(( struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*),
struct Cyc_List_List* x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=((
struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List*
_temp247=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( stdlib,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_ccargs, Cyc_libargs)); if( !
_temp236){ _temp247=({ struct Cyc_List_List* _temp248=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp248->hd=( void*) Cyc_final_strptr;
_temp248->tl= _temp247; _temp248;});}{ struct Cyc_Interface_I* _temp249=((
struct Cyc_Interface_I*(*)( struct Cyc_Interface_I*(* get)( struct _tagged_arr*),
struct Cyc_List_List* la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio, _temp247, _temp247); if( _temp249 ==  0){({ void* _temp250[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: interfaces incompatible\n",
sizeof( unsigned char), 32u), _tag_arr( _temp250, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp251= 1; _npop_handler( 0u); return _temp251;}}
if( _temp236){ if( Cyc_output_file !=  0){ struct _tagged_arr _temp252=({ struct
Cyc_Std_String_pa_struct _temp256; _temp256.tag= Cyc_Std_String_pa; _temp256.f1=(
struct _tagged_arr) Cyc_Filename_chop_extension(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp255[ 1u]={& _temp256}; Cyc_Std_aprintf(
_tag_arr("%s.cycio", sizeof( unsigned char), 9u), _tag_arr( _temp255, sizeof(
void*), 1u));}}); struct Cyc_Std___sFILE* _temp253= Cyc_try_file_open(( struct
_tagged_arr) _temp252, _tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp253 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{
int _temp254= 1; _npop_handler( 0u); return _temp254;}} Cyc_Interface_save((
struct Cyc_Interface_I*) _check_null( _temp249),( struct Cyc_Std___sFILE*)
_check_null( _temp253)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp253));}} else{ if( ! Cyc_Interface_is_subinterface( Cyc_Interface_empty(),(
struct Cyc_Interface_I*) _check_null( _temp249),({ struct _tuple8* _temp257=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp257->f1= _tag_arr("empty interface",
sizeof( unsigned char), 16u); _temp257->f2= _tag_arr("global interface", sizeof(
unsigned char), 17u); _temp257;}))){({ void* _temp258[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: some objects are still undefined\n", sizeof(
unsigned char), 41u), _tag_arr( _temp258, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp259= 1; _npop_handler( 0u); return _temp259;}}}}};
_pop_handler();} else{ void* _temp245=( void*) _exn_thrown; void* _temp261=
_temp245; struct _tagged_arr _temp275; struct _tagged_arr _temp277; struct
_tagged_arr _temp279; _LL263: if(*(( void**) _temp261) ==  Cyc_Core_Failure){
_LL276: _temp275=(( struct Cyc_Core_Failure_struct*) _temp261)->f1; goto _LL264;}
else{ goto _LL265;} _LL265: if(*(( void**) _temp261) ==  Cyc_Core_Impossible){
_LL278: _temp277=(( struct Cyc_Core_Impossible_struct*) _temp261)->f1; goto
_LL266;} else{ goto _LL267;} _LL267: if( _temp261 ==  Cyc_Dict_Absent){ goto
_LL268;} else{ goto _LL269;} _LL269: if(*(( void**) _temp261) ==  Cyc_Core_Invalid_argument){
_LL280: _temp279=(( struct Cyc_Core_Invalid_argument_struct*) _temp261)->f1;
goto _LL270;} else{ goto _LL271;} _LL271: goto _LL272; _LL273: goto _LL274;
_LL264:({ struct Cyc_Std_String_pa_struct _temp282; _temp282.tag= Cyc_Std_String_pa;
_temp282.f1=( struct _tagged_arr) _temp275;{ void* _temp281[ 1u]={& _temp282};
Cyc_Std_printf( _tag_arr("Exception Core::Failure %s\n", sizeof( unsigned char),
28u), _tag_arr( _temp281, sizeof( void*), 1u));}}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL266:({ struct Cyc_Std_String_pa_struct _temp284; _temp284.tag= Cyc_Std_String_pa;
_temp284.f1=( struct _tagged_arr) _temp277;{ void* _temp283[ 1u]={& _temp284};
Cyc_Std_printf( _tag_arr("Exception Core::Impossible %s\n", sizeof(
unsigned char), 31u), _tag_arr( _temp283, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL268:({ void* _temp285[ 0u]={}; Cyc_Std_printf(
_tag_arr("Exception Dict::Absent\n", sizeof( unsigned char), 24u), _tag_arr(
_temp285, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL270:({ struct Cyc_Std_String_pa_struct _temp287; _temp287.tag= Cyc_Std_String_pa;
_temp287.f1=( struct _tagged_arr) _temp279;{ void* _temp286[ 1u]={& _temp287};
Cyc_Std_printf( _tag_arr("Exception Core::Invalid_argument %s\n", sizeof(
unsigned char), 37u), _tag_arr( _temp286, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL272:({ void* _temp288[ 0u]={}; Cyc_Std_printf(
_tag_arr("Uncaught exception\n", sizeof( unsigned char), 20u), _tag_arr(
_temp288, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL274:( void) _throw( _temp261); _LL262:;}}}{ struct _tagged_arr
_temp289=({ struct Cyc_Std_String_pa_struct _temp298; _temp298.tag= Cyc_Std_String_pa;
_temp298.f1=( struct _tagged_arr) _temp235;{ struct Cyc_Std_String_pa_struct
_temp297; _temp297.tag= Cyc_Std_String_pa; _temp297.f1=( struct _tagged_arr)
stdlib_string;{ struct Cyc_Std_String_pa_struct _temp296; _temp296.tag= Cyc_Std_String_pa;
_temp296.f1=( struct _tagged_arr) _temp234;{ struct Cyc_Std_String_pa_struct
_temp295; _temp295.tag= Cyc_Std_String_pa; _temp295.f1=( struct _tagged_arr)(
Cyc_output_file ==  0? _tag_arr("", sizeof( unsigned char), 1u):( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp300; _temp300.tag= Cyc_Std_String_pa;
_temp300.f1=( struct _tagged_arr) Cyc_sh_escape_string(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp299[ 1u]={& _temp300}; Cyc_Std_aprintf(
_tag_arr(" -o %s", sizeof( unsigned char), 7u), _tag_arr( _temp299, sizeof( void*),
1u));}}));{ struct Cyc_Std_String_pa_struct _temp294; _temp294.tag= Cyc_Std_String_pa;
_temp294.f1=( struct _tagged_arr) comp;{ void* _temp293[ 5u]={& _temp294,&
_temp295,& _temp296,& _temp297,& _temp298}; Cyc_Std_aprintf( _tag_arr("%s %s %s%s%s",
sizeof( unsigned char), 13u), _tag_arr( _temp293, sizeof( void*), 5u));}}}}}});
if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp291; _temp291.tag= Cyc_Std_String_pa;
_temp291.f1=( struct _tagged_arr) _temp289;{ void* _temp290[ 1u]={& _temp291};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp290, sizeof( void*), 1u));}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp289)) !=  0){({ void* _temp292[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: C compiler failed\n", sizeof( unsigned char),
26u), _tag_arr( _temp292, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1;} Cyc_remove_cfiles(); return Cyc_compile_failure? 1: 0;}}}}}}}}
