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
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern void* Cyc_Core_identity( void*); extern unsigned char Cyc_Core_Invalid_argument[
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
ntCsl_to_ntsl( unsigned char**); extern int isalnum( int); extern int isalpha(
int); extern int isascii( int); extern int iscntrl( int); extern int isdigit(
int); extern int isgraph( int); extern int islower( int); extern int isprint(
int); extern int ispunct( int); extern int isspace( int); extern int isupper(
int); extern int isxdigit( int); extern int toascii( int); extern int tolower(
int); extern int toupper( int); extern int _tolower( int); extern int _toupper(
int); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int Cyc_List_length(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map( void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern void* Cyc_List_fold_left(
void*(* f)( void*, void*), void* accum, struct Cyc_List_List* x); extern void*
Cyc_List_fold_right_c( void*(* f)( void*, void*, void*), void*, struct Cyc_List_List*
x, void* accum); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_flatten(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_forall(
int(* pred)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_split( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_tabulate( int n, void*(*
f)( int)); extern struct Cyc_List_List* Cyc_List_filter( int(* f)( void*),
struct Cyc_List_List* x); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* k); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* k); extern void Cyc_Dict_iter( void(* f)( void*,
void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_delete(
struct Cyc_Dict_Dict*, void*); extern unsigned int Cyc_Std_strlen( struct
_tagged_arr s); extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern struct _tagged_arr Cyc_Std_strconcat( struct _tagged_arr,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_strdup( struct
_tagged_arr src); struct Cyc_Hashtable_Table; extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*));
extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key, void*
val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void*
key); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; static
const int Cyc_Typerep_Var= 0; struct Cyc_Typerep_Var_struct{ int tag; struct
_tagged_arr* f1; } ; static const int Cyc_Typerep_Int= 1; struct Cyc_Typerep_Int_struct{
int tag; unsigned int f1; } ; static const int Cyc_Typerep_Float= 0; static
const int Cyc_Typerep_Double= 1; static const int Cyc_Typerep_ThinPtr= 2; struct
Cyc_Typerep_ThinPtr_struct{ int tag; unsigned int f1; void* f2; } ; static const
int Cyc_Typerep_FatPtr= 3; struct Cyc_Typerep_FatPtr_struct{ int tag; void* f1;
} ; static const int Cyc_Typerep_Tuple= 4; struct _tuple1{ unsigned int f1; void*
f2; } ; struct Cyc_Typerep_Tuple_struct{ int tag; unsigned int f1; struct
_tagged_arr f2; } ; static const int Cyc_Typerep_TUnion= 5; struct Cyc_Typerep_TUnion_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Typerep_XTUnion= 6;
struct _tuple2{ struct _tagged_arr f1; void* f2; } ; struct Cyc_Typerep_XTUnion_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Typerep_Union= 7;
struct Cyc_Typerep_Union_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_Forall= 8; struct Cyc_Typerep_Forall_struct{ int tag;
struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App= 9;
struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
unsigned int Cyc_Typerep_size_type( void* rep); struct Cyc_Std_tm{ int tm_sec;
int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int
tm_yday; int tm_isdst; } ; extern void tzset(); extern int time( int* t); extern
struct Cyc_Std_tm* gmtime( const int* timep); extern struct Cyc_Std_tm*
localtime( const int* timep); extern int timezone; extern int daylight; static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple3{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
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
struct _tuple3* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl** f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple3* tunion_name; struct _tuple3* field_name; int is_xtunion; } ;
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
struct _tuple3* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple3* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple3* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple3* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
int tag; struct _tuple3* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple3* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 25; struct _tuple4{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple4* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
26; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 27; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 28; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple3* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 29;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 30; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple3* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple3* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
static const int Cyc_Absyn_Malloc_e= 33; struct Cyc_Absyn_Malloc_e_struct{ int
tag; struct Cyc_Absyn_MallocInfo f1; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; struct _tuple5{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_ForArrayInfo{
struct Cyc_List_List* defns; struct _tuple5 condition; struct _tuple5 delta;
struct Cyc_Absyn_Stmt* body; } ; static const int Cyc_Absyn_Skip_s= 0; static
const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int
Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple5 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple5 f2; struct _tuple5 f3; struct Cyc_Absyn_Stmt*
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
_tuple5 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 18; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
static const int Cyc_Absyn_ForArray_s= 19; struct Cyc_Absyn_ForArray_s_struct{
int tag; struct Cyc_Absyn_ForArrayInfo f1; } ; struct Cyc_Absyn_Stmt{ void* r;
struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds; int
try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p= 0; static const
int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p=
1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{ int tag; unsigned char
f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
int tag; struct _tuple3* f1; } ; static const int Cyc_Absyn_UnknownCall_p= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple3* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple3* f1; struct Cyc_List_List*
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple3* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple3* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple3* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple3* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple3* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple3* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple3* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple3*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple3*, struct _tuple3*); extern
void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp(
struct _tuple3*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp( void* t, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp( void*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple3* x,
void* t, struct Cyc_Absyn_Exp* init); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_printf( struct _tagged_arr fmt, struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt, struct
_tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int generate_line_directives: 1;
int use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern
struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_typ2cstring(
void*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
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
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_typecmp( void*,
void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); struct Cyc_Tcgenrep_RepInfo{ struct Cyc_List_List*
decls; struct Cyc_Absyn_Exp* exp; struct Cyc_List_List* dependencies; struct Cyc_Core_Opt*
fwd_decl; int emitted; int is_extern; } ; struct Cyc_Dict_Dict* Cyc_Tcgenrep_empty_typerep_dict(){
return(( struct Cyc_Dict_Dict*(*)( int(* cmp)( void*, void*))) Cyc_Dict_empty)(
Cyc_Tcutil_typecmp);} void Cyc_Tcgenrep_print_dict_entry( void* type, struct Cyc_Tcgenrep_RepInfo*
info){({ struct Cyc_Std_Int_pa_struct _temp3; _temp3.tag= Cyc_Std_Int_pa; _temp3.f1=(
int)(( unsigned int) info->emitted);{ struct Cyc_Std_Int_pa_struct _temp2;
_temp2.tag= Cyc_Std_Int_pa; _temp2.f1=( unsigned int) info;{ struct Cyc_Std_String_pa_struct
_temp1; _temp1.tag= Cyc_Std_String_pa; _temp1.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
type);{ void* _temp0[ 3u]={& _temp1,& _temp2,& _temp3}; Cyc_Std_printf( _tag_arr("(%s,%x:%d,",
sizeof( unsigned char), 11u), _tag_arr( _temp0, sizeof( void*), 3u));}}}});{
struct Cyc_List_List* p= info->dependencies; for( 0; p !=  0; p= p->tl){({
struct Cyc_Std_Int_pa_struct _temp5; _temp5.tag= Cyc_Std_Int_pa; _temp5.f1=(
unsigned int)(( struct Cyc_Tcgenrep_RepInfo*) p->hd);{ void* _temp4[ 1u]={&
_temp5}; Cyc_Std_printf( _tag_arr("%x", sizeof( unsigned char), 3u), _tag_arr(
_temp4, sizeof( void*), 1u));}}); if( p->tl !=  0){({ void* _temp6[ 0u]={}; Cyc_Std_printf(
_tag_arr(",", sizeof( unsigned char), 2u), _tag_arr( _temp6, sizeof( void*), 0u));});}}}({
void* _temp7[ 0u]={}; Cyc_Std_printf( _tag_arr(")\n", sizeof( unsigned char), 3u),
_tag_arr( _temp7, sizeof( void*), 0u));});} void Cyc_Tcgenrep_print_typerep_dict(
struct Cyc_Dict_Dict* dict){(( void(*)( void(* f)( void*, struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tcgenrep_print_dict_entry, dict);}
static int Cyc_Tcgenrep_rephash( struct Cyc_Tcgenrep_RepInfo* ri){ return( int)
ri;} static int Cyc_Tcgenrep_repcmp( struct Cyc_Tcgenrep_RepInfo* r1, struct Cyc_Tcgenrep_RepInfo*
r2){ unsigned int r1p; unsigned int r2p; r1p=( unsigned int) r1; r2p=(
unsigned int) r2; if( r1 <  r2){ return - 1;} else{ if( r1 ==  r2){ return 0;}
else{ return 1;}}} static struct Cyc_Absyn_Tunionfield* Cyc_Tcgenrep_getField(
struct Cyc_Absyn_Tuniondecl* td, struct _tuple3* fieldname){ if( td->fields == 
0){({ void* _temp8[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Tcutil_impos)( _tag_arr("Could not find field in tuniondecl", sizeof(
unsigned char), 35u), _tag_arr( _temp8, sizeof( void*), 0u));});} else{ struct
Cyc_List_List* l=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
td->fields))->v; for( 0; l !=  0; l= l->tl){ if( ! Cyc_Absyn_qvar_cmp((( struct
Cyc_Absyn_Tunionfield*) l->hd)->name, fieldname)){ return( struct Cyc_Absyn_Tunionfield*)
l->hd;}}}({ void* _temp9[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("Could not find field in tuniondecl",
sizeof( unsigned char), 35u), _tag_arr( _temp9, sizeof( void*), 0u));});( int)
_throw(( void*) Cyc_Tcutil_TypeErr);} static unsigned char _temp10[ 9u]="Tcgenrep";
static struct _tagged_arr Cyc_Tcgenrep_tcgenrep_nm={ _temp10, _temp10, _temp10 + 
9u}; static struct Cyc_List_List Cyc_Tcgenrep_l={( void*)& Cyc_Tcgenrep_tcgenrep_nm,
0}; static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_tcgenrep_nmspace={ 1u,(
struct Cyc_List_List*)& Cyc_Tcgenrep_l}; static void* Cyc_Tcgenrep_tcgenrep_nmspace2=(
void*)& Cyc_Tcgenrep_tcgenrep_nmspace; static unsigned char _temp12[ 8u]="Typerep";
static struct _tagged_arr Cyc_Tcgenrep_typerep_nm={ _temp12, _temp12, _temp12 + 
8u}; static struct Cyc_List_List Cyc_Tcgenrep_l2={( void*)& Cyc_Tcgenrep_typerep_nm,
0}; static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={ 1u,(
struct Cyc_List_List*)& Cyc_Tcgenrep_l2}; static struct Cyc_List_List* Cyc_Tcgenrep_dfsvisit(
struct Cyc_Tcgenrep_RepInfo* ri, struct Cyc_Hashtable_Table* visited){ struct
_handler_cons _temp14; _push_handler(& _temp14);{ int _temp16= 0; if( setjmp(
_temp14.handler)){ _temp16= 1;} if( ! _temp16){(( int(*)( struct Cyc_Hashtable_Table*
t, struct Cyc_Tcgenrep_RepInfo* key)) Cyc_Hashtable_lookup)( visited, ri);{
struct Cyc_List_List* _temp17= 0; _npop_handler( 0u); return _temp17;};
_pop_handler();} else{ void* _temp15=( void*) _exn_thrown; void* _temp19=
_temp15; _LL21: if( _temp19 ==  Cyc_Core_Not_found){ goto _LL22;} else{ goto
_LL23;} _LL23: goto _LL24; _LL22:(( void(*)( struct Cyc_Hashtable_Table* t,
struct Cyc_Tcgenrep_RepInfo* key, int val)) Cyc_Hashtable_insert)( visited, ri,
1);{ struct Cyc_List_List* ds= 0;{ struct Cyc_List_List* l= ri->dependencies;
for( 0; l !=  0; l= l->tl){ ds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( ds, Cyc_Tcgenrep_dfsvisit((
struct Cyc_Tcgenrep_RepInfo*) l->hd, visited));}} if( ri->emitted ==  0){ if( ri->fwd_decl
!=  0){ ds=({ struct Cyc_List_List* _temp25=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp25->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp26[ 1u]; _temp26[ 0u]=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( ri->fwd_decl))->v;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp26, sizeof( struct Cyc_Absyn_Decl*), 1u));});
_temp25->tl= ds; _temp25;});}{ struct Cyc_List_List* _temp27=({ struct Cyc_List_List*
_temp28=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp28->hd=( void*) ri->decls; _temp28->tl= 0; _temp28;}); ri->emitted= 1;
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( ds, _temp27);}} else{ return ds;}} _LL24:( void)
_throw( _temp19); _LL20:;}}} static struct Cyc_List_List* Cyc_Tcgenrep_dfs(
struct Cyc_Tcgenrep_RepInfo* ri){ struct Cyc_Hashtable_Table* tab=(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct Cyc_Tcgenrep_RepInfo*, struct Cyc_Tcgenrep_RepInfo*),
int(* hash)( struct Cyc_Tcgenrep_RepInfo*))) Cyc_Hashtable_create)( 53, Cyc_Tcgenrep_repcmp,
Cyc_Tcgenrep_rephash); struct Cyc_List_List* ds= Cyc_Tcgenrep_dfsvisit( ri, tab);
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(
ds);} static unsigned char _temp29[ 11u]="Typestruct"; static struct _tagged_arr
Cyc_Tcgenrep_typestruct_str={ _temp29, _temp29, _temp29 +  11u}; static
unsigned char _temp30[ 4u]="Var"; static struct _tagged_arr Cyc_Tcgenrep_var_str={
_temp30, _temp30, _temp30 +  4u}; static unsigned char _temp31[ 4u]="Int";
static struct _tagged_arr Cyc_Tcgenrep_int_str={ _temp31, _temp31, _temp31 +  4u};
static unsigned char _temp32[ 6u]="Float"; static struct _tagged_arr Cyc_Tcgenrep_float_str={
_temp32, _temp32, _temp32 +  6u}; static unsigned char _temp33[ 7u]="Double";
static struct _tagged_arr Cyc_Tcgenrep_double_str={ _temp33, _temp33, _temp33 + 
7u}; static unsigned char _temp34[ 8u]="ThinPtr"; static struct _tagged_arr Cyc_Tcgenrep_thinptr_str={
_temp34, _temp34, _temp34 +  8u}; static unsigned char _temp35[ 7u]="FatPtr";
static struct _tagged_arr Cyc_Tcgenrep_fatptr_str={ _temp35, _temp35, _temp35 + 
7u}; static unsigned char _temp36[ 6u]="Tuple"; static struct _tagged_arr Cyc_Tcgenrep_tuple_str={
_temp36, _temp36, _temp36 +  6u}; static unsigned char _temp37[ 7u]="TUnion";
static struct _tagged_arr Cyc_Tcgenrep_tunion_str={ _temp37, _temp37, _temp37 + 
7u}; static unsigned char _temp38[ 8u]="XTUnion"; static struct _tagged_arr Cyc_Tcgenrep_xtunion_str={
_temp38, _temp38, _temp38 +  8u}; static unsigned char _temp39[ 6u]="Union";
static struct _tagged_arr Cyc_Tcgenrep_union_str={ _temp39, _temp39, _temp39 + 
6u}; static unsigned char _temp40[ 7u]="name_t"; static struct _tagged_arr Cyc_Tcgenrep_name_t_str={
_temp40, _temp40, _temp40 +  7u}; static struct _tuple3* Cyc_Tcgenrep_typerep_name(
struct _tagged_arr* name){ return({ struct _tuple3* _temp41=( struct _tuple3*)
_cycalloc( sizeof( struct _tuple3)); _temp41->f1=( void*)& Cyc_Tcgenrep_typerep_nmspace;
_temp41->f2= name; _temp41;});} static int Cyc_Tcgenrep_gen_id_counter= 0;
static struct _tagged_arr* Cyc_Tcgenrep_new_gen_id( struct _tagged_arr name){
return({ struct _tagged_arr* _temp42=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp42[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp45; _temp45.tag= Cyc_Std_Int_pa; _temp45.f1=( int)(( unsigned int) Cyc_Tcgenrep_gen_id_counter
++);{ struct Cyc_Std_String_pa_struct _temp44; _temp44.tag= Cyc_Std_String_pa;
_temp44.f1=( struct _tagged_arr) name;{ void* _temp43[ 2u]={& _temp44,& _temp45};
Cyc_Std_aprintf( _tag_arr("_gen%s_%d", sizeof( unsigned char), 10u), _tag_arr(
_temp43, sizeof( void*), 2u));}}}); _temp42;});} static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List* l){({ void* _temp46[ 0u]={}; Cyc_Std_printf( _tag_arr("<",
sizeof( unsigned char), 2u), _tag_arr( _temp46, sizeof( void*), 0u));});{ struct
Cyc_List_List* p= l; for( 0; p !=  0; p= p->tl){({ struct Cyc_Std_Int_pa_struct
_temp49; _temp49.tag= Cyc_Std_Int_pa; _temp49.f1=( int)(( unsigned int)(( int)(
p->tl !=  0?',':' ')));{ struct Cyc_Std_String_pa_struct _temp48; _temp48.tag=
Cyc_Std_String_pa; _temp48.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*) p->hd);{ void* _temp47[ 2u]={& _temp48,& _temp49}; Cyc_Std_printf(
_tag_arr("%s%c", sizeof( unsigned char), 5u), _tag_arr( _temp47, sizeof( void*),
2u));}}});}}({ void* _temp50[ 0u]={}; Cyc_Std_printf( _tag_arr(">\n", sizeof(
unsigned char), 3u), _tag_arr( _temp50, sizeof( void*), 0u));});} static void
Cyc_Tcgenrep_print_tvars( struct Cyc_List_List* l){({ void* _temp51[ 0u]={}; Cyc_Std_printf(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr( _temp51, sizeof( void*), 0u));});{
struct Cyc_List_List* p= l; for( 0; p !=  0; p= p->tl){({ struct Cyc_Std_Int_pa_struct
_temp54; _temp54.tag= Cyc_Std_Int_pa; _temp54.f1=( int)(( unsigned int)(( int)(
p->tl !=  0?',':' ')));{ struct Cyc_Std_String_pa_struct _temp53; _temp53.tag=
Cyc_Std_String_pa; _temp53.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*) p->hd)->name;{
void* _temp52[ 2u]={& _temp53,& _temp54}; Cyc_Std_printf( _tag_arr("%s%c",
sizeof( unsigned char), 5u), _tag_arr( _temp52, sizeof( void*), 2u));}}});}}({
void* _temp55[ 0u]={}; Cyc_Std_printf( _tag_arr(">\n", sizeof( unsigned char), 3u),
_tag_arr( _temp55, sizeof( void*), 0u));});} static struct Cyc_Absyn_Rel_n_struct
Cyc_Tcgenrep_top_nm_0={ 0u, 0}; static void* Cyc_Tcgenrep_top_nm=( void*)& Cyc_Tcgenrep_top_nm_0;
static struct _tuple3* Cyc_Tcgenrep_toplevel_name( struct _tagged_arr* name){
return({ struct _tuple3* _temp57=( struct _tuple3*) _cycalloc( sizeof( struct
_tuple3)); _temp57->f1= Cyc_Tcgenrep_top_nm; _temp57->f2=( struct _tagged_arr*)
name; _temp57;});} static struct _tuple3* Cyc_Tcgenrep_relative_name( struct Cyc_List_List*
pathl, struct _tagged_arr* name){ struct Cyc_Absyn_Rel_n_struct* path=({ struct
Cyc_Absyn_Rel_n_struct* _temp59=( struct Cyc_Absyn_Rel_n_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp59[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp60; _temp60.tag= Cyc_Absyn_Rel_n; _temp60.f1= pathl; _temp60;}); _temp59;});
return({ struct _tuple3* _temp58=( struct _tuple3*) _cycalloc( sizeof( struct
_tuple3)); _temp58->f1=( void*) path; _temp58->f2=( struct _tagged_arr*) name;
_temp58;});} static struct Cyc_Absyn_Decl* Cyc_Tcgenrep_gen_decl( struct
_tagged_arr* name, void* type, struct Cyc_Absyn_Exp* init, struct Cyc_Position_Segment*
seg){ struct _tuple3* qvar= Cyc_Tcgenrep_toplevel_name( name); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl( qvar, type, init);( void*)( vd->sc=( void*)(( void*)
Cyc_Absyn_Static));{ struct Cyc_Absyn_Var_d_struct* r1=({ struct Cyc_Absyn_Var_d_struct*
_temp61=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp61[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp62; _temp62.tag= Cyc_Absyn_Var_d;
_temp62.f1= vd; _temp62;}); _temp61;}); void* r2=( void*) r1; return Cyc_Absyn_new_decl(
r2, seg);}} static struct Cyc_Absyn_Decl* Cyc_Tcgenrep_gen_vardecl( struct
_tuple3* name, void* type, struct Cyc_Absyn_Exp* init, void* sc, struct Cyc_Position_Segment*
seg){ struct _tuple3* topname= Cyc_Tcgenrep_toplevel_name((* name).f2); struct
Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( topname, type, init);( void*)( vd->sc=(
void*) sc);{ struct Cyc_Absyn_Var_d_struct* r1=({ struct Cyc_Absyn_Var_d_struct*
_temp63=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp63[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp64; _temp64.tag= Cyc_Absyn_Var_d;
_temp64.f1= vd; _temp64;}); _temp63;}); void* r2=( void*) r1; return Cyc_Absyn_new_decl(
r2, seg);}} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_string( struct
_tagged_arr s, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_String_c_struct* _temp65=( struct Cyc_Absyn_String_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp65[ 0]=({ struct Cyc_Absyn_String_c_struct
_temp66; _temp66.tag= Cyc_Absyn_String_c; _temp66.f1= s; _temp66;}); _temp65;}),
seg);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_string_cls( struct Cyc_Position_Segment*
seg, struct _tagged_arr* s){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_String_c_struct*
_temp67=( struct Cyc_Absyn_String_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp67[ 0]=({ struct Cyc_Absyn_String_c_struct _temp68; _temp68.tag= Cyc_Absyn_String_c;
_temp68.f1=* s; _temp68;}); _temp67;}), seg);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_int(
int i, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp69=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp69[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp70; _temp70.tag= Cyc_Absyn_Int_c; _temp70.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp70.f2= i; _temp70;}); _temp69;}), seg);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_int_cls(
struct Cyc_Position_Segment* seg, int i){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp71=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp71[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp72; _temp72.tag= Cyc_Absyn_Int_c; _temp72.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp72.f2= i; _temp72;}); _temp71;}), seg);} static int Cyc_Tcgenrep_size_of2int(
void* sz){ void* _temp73= sz; _LL75: if( _temp73 == ( void*) Cyc_Absyn_B1){ goto
_LL76;} else{ goto _LL77;} _LL77: if( _temp73 == ( void*) Cyc_Absyn_B2){ goto
_LL78;} else{ goto _LL79;} _LL79: if( _temp73 == ( void*) Cyc_Absyn_B4){ goto
_LL80;} else{ goto _LL81;} _LL81: if( _temp73 == ( void*) Cyc_Absyn_B8){ goto
_LL82;} else{ goto _LL74;} _LL76: return 8; _LL78: return 16; _LL80: return 32;
_LL82: return 64; _LL74:;} static void* Cyc_Tcgenrep_array_typ( void* ty, struct
Cyc_Absyn_Tqual tq, struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp83=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp83[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp84; _temp84.tag= Cyc_Absyn_ArrayType;
_temp84.f1=( void*) ty; _temp84.f2= tq; _temp84.f3= e; _temp84;}); _temp83;});}
static void* Cyc_Tcgenrep_tunion_typ( struct _tuple3* name){ return( void*)({
struct Cyc_Absyn_TunionType_struct* _temp85=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp85[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp86; _temp86.tag= Cyc_Absyn_TunionType; _temp86.f1=({
struct Cyc_Absyn_TunionInfo _temp87; _temp87.tunion_info=( void*)(( void*)({
struct Cyc_Absyn_UnknownTunion_struct* _temp88=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp88[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp89; _temp89.tag= Cyc_Absyn_UnknownTunion;
_temp89.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp90; _temp90.name= name;
_temp90.is_xtunion= 0; _temp90;}); _temp89;}); _temp88;})); _temp87.targs= 0;
_temp87.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp87;}); _temp86;});
_temp85;});} static void* Cyc_Tcgenrep_tunionfield_typ( struct _tuple3* name,
struct _tuple3* fieldname){ return( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp91=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp91[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp92; _temp92.tag= Cyc_Absyn_TunionFieldType; _temp92.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp93; _temp93.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp94=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunionfield_struct)); _temp94[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp95; _temp95.tag= Cyc_Absyn_UnknownTunionfield; _temp95.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp96; _temp96.tunion_name= name; _temp96.field_name= fieldname; _temp96.is_xtunion=
0; _temp96;}); _temp95;}); _temp94;})); _temp93.targs= 0; _temp93;}); _temp92;});
_temp91;});} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_call_exp( struct _tuple3*
name, struct Cyc_List_List* args, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_unknowncall_exp(
Cyc_Absyn_unknownid_exp( name, loc), args, loc);} static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_tunion_constructor_decl( struct _tuple3* tunionname, struct _tuple3*
fieldname, struct _tuple3* varname, struct Cyc_List_List* args, void* sc, struct
Cyc_Position_Segment* loc){ void* _temp97= Cyc_Tcgenrep_tunionfield_typ(
tunionname, fieldname); struct Cyc_Absyn_Exp* _temp98= Cyc_Tcgenrep_call_exp(
fieldname, args, loc); struct Cyc_Absyn_Decl* _temp99= Cyc_Tcgenrep_gen_vardecl(
varname, _temp97,( struct Cyc_Absyn_Exp*) _temp98, sc, loc); return _temp99;}
struct _tuple6{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Tcgenrep_get_second(
struct _tuple6* pair){ return(* pair).f2;} struct _tuple7{ struct _tagged_arr*
f1; struct Cyc_Absyn_Exp* f2; } ; static struct _tagged_arr* Cyc_Tcgenrep_get_first(
struct _tuple7* pair){ return(* pair).f1;} static unsigned char _temp100[ 5u]="list";
static struct _tagged_arr Cyc_Tcgenrep_list_str={ _temp100, _temp100, _temp100 + 
5u}; static unsigned char _temp101[ 5u]="List"; static struct _tagged_arr Cyc_Tcgenrep_List_str={
_temp101, _temp101, _temp101 +  5u}; struct _tuple8{ struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_tuple2_exp_cls(
struct Cyc_Position_Segment* loc, struct _tuple8* es){ struct _tuple8 _temp104;
struct Cyc_Absyn_Exp* _temp105; struct Cyc_Absyn_Exp* _temp107; struct _tuple8*
_temp102= es; _temp104=* _temp102; _LL108: _temp107= _temp104.f1; goto _LL106;
_LL106: _temp105= _temp104.f2; goto _LL103; _LL103: return Cyc_Absyn_tuple_exp(({
struct Cyc_Absyn_Exp* _temp109[ 2u]; _temp109[ 1u]= _temp105; _temp109[ 0u]=
_temp107;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp109, sizeof( struct Cyc_Absyn_Exp*), 2u));}), loc);} static
unsigned char _temp110[ 5u]="NULL"; static struct _tagged_arr Cyc_Tcgenrep_null_str={
_temp110, _temp110, _temp110 +  5u}; static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_list_exp(
struct Cyc_List_List* l, struct Cyc_Position_Segment* loc){ if( l ==  0){ return
Cyc_Absyn_null_exp( loc);} return Cyc_Tcgenrep_call_exp( Cyc_Tcgenrep_toplevel_name(&
Cyc_Tcgenrep_list_str), l, loc);} struct _tuple9{ void* f1; struct Cyc_Position_Segment*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_make_offsetof_exp( struct
_tuple9* typeloc, int index){ struct _tuple9 _temp113; struct Cyc_Position_Segment*
_temp114; void* _temp116; struct _tuple9* _temp111= typeloc; _temp113=* _temp111;
_LL117: _temp116= _temp113.f1; goto _LL115; _LL115: _temp114= _temp113.f2; goto
_LL112; _LL112: return Cyc_Absyn_offsetof_exp( _temp116,( void*)({ struct Cyc_Absyn_TupleIndex_struct*
_temp118=( struct Cyc_Absyn_TupleIndex_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_TupleIndex_struct)); _temp118[ 0]=({ struct Cyc_Absyn_TupleIndex_struct
_temp119; _temp119.tag= Cyc_Absyn_TupleIndex; _temp119.f1=( unsigned int) index;
_temp119;}); _temp118;}), _temp114);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_get_and_cast_ri_exp(
struct Cyc_Position_Segment* loc, struct Cyc_Tcgenrep_RepInfo* info){ return Cyc_Absyn_cast_exp(
Cyc_Tcgenrep_tunion_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),(
struct Cyc_Absyn_Exp*) _check_null( info->exp), loc);} static struct _tuple7*
Cyc_Tcgenrep_gen_id_for_exp( struct Cyc_Absyn_Exp* e){ return({ struct _tuple7*
_temp120=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp120->f1=
Cyc_Tcgenrep_new_gen_id( _tag_arr("tuple", sizeof( unsigned char), 6u));
_temp120->f2= e; _temp120;});} static struct Cyc_Absyn_Decl* Cyc_Tcgenrep_gen_decl_cls(
struct _tuple9* env, struct _tuple7* name_exp){ struct _tuple9 _temp123; struct
Cyc_Position_Segment* _temp124; void* _temp126; struct _tuple9* _temp121= env;
_temp123=* _temp121; _LL127: _temp126= _temp123.f1; goto _LL125; _LL125:
_temp124= _temp123.f2; goto _LL122; _LL122: { struct _tuple7 _temp130; struct
Cyc_Absyn_Exp* _temp131; struct _tagged_arr* _temp133; struct _tuple7* _temp128=
name_exp; _temp130=* _temp128; _LL134: _temp133= _temp130.f1; goto _LL132;
_LL132: _temp131= _temp130.f2; goto _LL129; _LL129: return Cyc_Tcgenrep_gen_decl(
_temp133, _temp126,( struct Cyc_Absyn_Exp*) _temp131, _temp124);}} struct
_tuple10{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple10* Cyc_Tcgenrep_null_designator_exp( struct Cyc_Absyn_Exp* e){ return({
struct _tuple10* _temp135=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp135->f1= 0; _temp135->f2= e; _temp135;});} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_arr_init_exp(
struct Cyc_List_List* l, struct Cyc_Position_Segment* loc){ struct Cyc_List_List*
_temp136=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_null_designator_exp, l);
return Cyc_Absyn_unresolvedmem_exp( 0, _temp136, loc);} static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_address_exp_cls( struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_address_exp( e, loc);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_unknownid_exp_cls(
struct Cyc_Position_Segment* loc, struct _tuple3* e){ return Cyc_Absyn_unknownid_exp(
e, loc);} static int Cyc_Tcgenrep_has_bitfield( struct Cyc_Absyn_Structfield* sf){
return sf->width !=  0;} static int Cyc_Tcgenrep_add_bitfield_sizes( int total,
struct Cyc_Absyn_Structfield* sf){ return( int)( Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( sf->width)) +  total);} static void* Cyc_Tcgenrep_select_structfield_type(
struct Cyc_Absyn_Structfield* sf){ if( Cyc_Std_strcmp(* sf->name, _tag_arr("",
sizeof( unsigned char), 1u)) ==  0){({ void* _temp137[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof( unsigned char), 55u), _tag_arr( _temp137, sizeof( void*), 0u));});} if(
Cyc_Tcgenrep_has_bitfield( sf)){({ void* _temp138[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): mixed struct bitfields and union bitfields not handled",
sizeof( unsigned char), 62u), _tag_arr( _temp138, sizeof( void*), 0u));});}
return( void*) sf->type;} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_update_info(
struct Cyc_Dict_Dict* dict, void* type, struct Cyc_List_List* decls, struct Cyc_Absyn_Exp*
exp, struct Cyc_List_List* dependencies, struct Cyc_Core_Opt* fwd_decl){ struct
Cyc_Core_Opt* _temp139=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, void*
k)) Cyc_Dict_lookup_opt)( dict, type); if( _temp139 !=  0){ if((( struct Cyc_Tcgenrep_RepInfo*)
_temp139->v)->decls !=  0){ Cyc_Tcgenrep_print_typerep_dict( dict);({ void*
_temp140[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("Updating non-forward declaration", sizeof( unsigned char), 33u),
_tag_arr( _temp140, sizeof( void*), 0u));});}(( struct Cyc_Tcgenrep_RepInfo*)
_temp139->v)->decls= decls;(( struct Cyc_Tcgenrep_RepInfo*) _temp139->v)->exp=
exp;(( struct Cyc_Tcgenrep_RepInfo*) _temp139->v)->dependencies= dependencies;
return dict;} else{ return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* k, struct Cyc_Tcgenrep_RepInfo* v)) Cyc_Dict_insert)( dict, type,({ struct
Cyc_Tcgenrep_RepInfo* _temp141=( struct Cyc_Tcgenrep_RepInfo*) _cycalloc(
sizeof( struct Cyc_Tcgenrep_RepInfo)); _temp141->decls= decls; _temp141->exp=
exp; _temp141->dependencies= dependencies; _temp141->fwd_decl= fwd_decl;
_temp141->emitted= 0; _temp141->is_extern= 0; _temp141;}));}} static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_make_fwd_decl_info( struct Cyc_Dict_Dict* dict, void* type, struct
Cyc_Absyn_Exp* exp, struct Cyc_Core_Opt* fwd_decl, int is_extern){ struct Cyc_Core_Opt*
_temp142=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, void* k)) Cyc_Dict_lookup_opt)(
dict, type); if( _temp142 !=  0){({ struct Cyc_Std_String_pa_struct _temp144;
_temp144.tag= Cyc_Std_String_pa; _temp144.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
type);{ void* _temp143[ 1u]={& _temp144};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("Repinfo for fwd declaration of %s already exists",
sizeof( unsigned char), 49u), _tag_arr( _temp143, sizeof( void*), 1u));}});
return dict;} else{ return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* k, struct Cyc_Tcgenrep_RepInfo* v)) Cyc_Dict_insert)( dict, type,({ struct
Cyc_Tcgenrep_RepInfo* _temp145=( struct Cyc_Tcgenrep_RepInfo*) _cycalloc(
sizeof( struct Cyc_Tcgenrep_RepInfo)); _temp145->decls= 0; _temp145->exp= exp;
_temp145->dependencies= 0; _temp145->fwd_decl= fwd_decl; _temp145->emitted= 0;
_temp145->is_extern= is_extern; _temp145;}));}} static struct Cyc_Absyn_Tqual
Cyc_Tcgenrep_tq_none={ 0, 0, 0}; static struct _tuple6* Cyc_Tcgenrep_tqual_type(
struct Cyc_Absyn_Tqual* tq, void* type){ return({ struct _tuple6* _temp146=(
struct _tuple6*) _cycalloc( sizeof( struct _tuple6)); _temp146->f1=*(( struct
Cyc_Absyn_Tqual*) _check_null( tq)); _temp146->f2= type; _temp146;});} static
void* Cyc_Tcgenrep_tuple_typ( struct Cyc_List_List* types){ struct Cyc_List_List*
_temp147=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Tqual*,
void*), struct Cyc_Absyn_Tqual* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcgenrep_tqual_type,( struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none, types);
struct Cyc_Absyn_TupleType_struct* tuple_type_base=({ struct Cyc_Absyn_TupleType_struct*
_temp148=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp148[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp149; _temp149.tag= Cyc_Absyn_TupleType;
_temp149.f1= _temp147; _temp149;}); _temp148;}); void* tuple_type=( void*)
tuple_type_base; return tuple_type;} static struct _tuple10* Cyc_Tcgenrep_array_decls(
void* type, struct Cyc_List_List* exps, struct Cyc_Position_Segment* loc){
struct Cyc_List_List* _temp150=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_gen_id_for_exp,
exps); struct Cyc_List_List* _temp151=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(*
f)( struct _tuple9*, struct _tuple7*), struct _tuple9* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_gen_decl_cls,({ struct _tuple9* _temp164=(
struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp164->f1= type;
_temp164->f2= loc; _temp164;}), _temp150); struct Cyc_List_List* _temp152=((
struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcgenrep_get_first, _temp150); struct Cyc_List_List*
_temp153=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct _tagged_arr*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_toplevel_name, _temp152);
struct Cyc_List_List* _temp154=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Position_Segment*, struct _tuple3*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_unknownid_exp_cls,
loc, _temp153); struct Cyc_List_List* _temp155=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Absyn_Exp*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_address_exp_cls,
loc, _temp154); struct Cyc_Absyn_Exp* _temp156= Cyc_Tcgenrep_arr_init_exp(
_temp155, loc); struct _tagged_arr* _temp157= Cyc_Tcgenrep_new_gen_id( _tag_arr("arr",
sizeof( unsigned char), 4u)); void* _temp158= Cyc_Absyn_at_typ( type,( void*)
Cyc_Absyn_HeapRgn, Cyc_Tcgenrep_tq_none); void* _temp159= Cyc_Tcgenrep_array_typ(
_temp158, Cyc_Tcgenrep_tq_none, 0); struct Cyc_Absyn_Decl* _temp160= Cyc_Tcgenrep_gen_decl(
_temp157, _temp159,( struct Cyc_Absyn_Exp*) _temp156, loc); struct Cyc_Absyn_Exp*
_temp161= Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp157), loc);
return({ struct _tuple10* _temp162=( struct _tuple10*) _cycalloc( sizeof( struct
_tuple10)); _temp162->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp151,({ struct Cyc_Absyn_Decl*
_temp163[ 1u]; _temp163[ 0u]= _temp160;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp163, sizeof( struct Cyc_Absyn_Decl*),
1u));})); _temp162->f2= _temp161; _temp162;});} static void* Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield* tuf){ if( tuf->tvs !=  0){({ void* _temp165[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): tunionfield type varaibles not handled", sizeof( unsigned char),
46u), _tag_arr( _temp165, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp166=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp166[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp167; _temp167.tag= Cyc_Absyn_TupleType;
_temp167.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)(({ struct _tuple6* _temp168[ 1u]; _temp168[ 0u]=({
struct _tuple6* _temp169=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp169->f1= Cyc_Tcgenrep_tq_none; _temp169->f2= Cyc_Absyn_uint_t; _temp169;});((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp168, sizeof( struct _tuple6*), 1u));}), tuf->typs); _temp167;}); _temp166;});}
struct _tuple11{ struct _tagged_arr* f1; void* f2; } ; static struct _tuple11*
Cyc_Tcgenrep_check_xtunionfield_and_get_name_type( struct Cyc_Absyn_Tunionfield*
tuf){ if( tuf->tvs !=  0){({ void* _temp170[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield type varaibles not handled",
sizeof( unsigned char), 46u), _tag_arr( _temp170, sizeof( void*), 0u));});}
return({ struct _tuple11* _temp171=( struct _tuple11*) _cycalloc( sizeof( struct
_tuple11)); _temp171->f1=(* tuf->name).f2; _temp171->f2=( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp172=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp172[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp173; _temp173.tag= Cyc_Absyn_TupleType;
_temp173.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)(({ struct _tuple6* _temp174[ 1u]; _temp174[ 0u]=({
struct _tuple6* _temp175=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6));
_temp175->f1= Cyc_Tcgenrep_tq_none; _temp175->f2= Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Tcgenrep_tq_none); _temp175;});(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp174, sizeof( struct _tuple6*),
1u));}), tuf->typs); _temp173;}); _temp172;}); _temp171;});} static int Cyc_Tcgenrep_filter_empty_tunionfield(
struct Cyc_Absyn_Tunionfield* tuf){ return tuf->typs !=  0;} static struct
_tagged_arr* Cyc_Tcgenrep_get_tunionfield_name( struct Cyc_Absyn_Tunionfield*
tuf){ return(* tuf->name).f2;} struct Cyc_Absyn_Structfield* Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List* subst, struct Cyc_Absyn_Structfield* sf){ return({ struct
Cyc_Absyn_Structfield* _temp176=( struct Cyc_Absyn_Structfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp176->name= sf->name; _temp176->tq=
sf->tq; _temp176->type=( void*) Cyc_Tcutil_substitute( subst,( void*) sf->type);
_temp176->width= sf->width; _temp176->attributes= sf->attributes; _temp176;});}
struct _tuple6* Cyc_Tcgenrep_substitute_tqual_type( struct Cyc_List_List* subst,
struct _tuple6* pair){ struct _tuple6 _temp179; void* _temp180; struct Cyc_Absyn_Tqual
_temp182; struct _tuple6* _temp177= pair; _temp179=* _temp177; _LL183: _temp182=
_temp179.f1; goto _LL181; _LL181: _temp180= _temp179.f2; goto _LL178; _LL178:
return({ struct _tuple6* _temp184=( struct _tuple6*) _cycalloc( sizeof( struct
_tuple6)); _temp184->f1= _temp182; _temp184->f2= Cyc_Tcutil_substitute( subst,
_temp180); _temp184;});} struct Cyc_Absyn_Tunionfield* Cyc_Tcgenrep_substitute_tunionfield_type(
struct Cyc_List_List* subst, struct Cyc_Absyn_Tunionfield* tf){ return({ struct
Cyc_Absyn_Tunionfield* _temp185=( struct Cyc_Absyn_Tunionfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Tunionfield)); _temp185->name= tf->name; _temp185->tvs=
tf->tvs; _temp185->typs=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)(
struct Cyc_List_List*, struct _tuple6*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_tqual_type, subst, tf->typs);
_temp185->loc= tf->loc; _temp185->sc=( void*)(( void*) tf->sc); _temp185;});}
void* Cyc_Tcgenrep_monomorphize_type( void* type){ void* _temp186= Cyc_Tcutil_compress(
type); struct Cyc_Absyn_Structdecl** _temp196; struct Cyc_List_List* _temp198;
struct _tuple3* _temp200; struct Cyc_Absyn_Uniondecl** _temp202; struct Cyc_List_List*
_temp204; struct _tuple3* _temp206; struct Cyc_Absyn_TunionInfo _temp208; void*
_temp210; struct Cyc_List_List* _temp212; void* _temp214; struct Cyc_Absyn_Tuniondecl**
_temp216; struct Cyc_Absyn_Tuniondecl* _temp218; _LL188: if(( unsigned int)
_temp186 >  3u?*(( int*) _temp186) ==  Cyc_Absyn_StructType: 0){ _LL201:
_temp200=(( struct Cyc_Absyn_StructType_struct*) _temp186)->f1; goto _LL199;
_LL199: _temp198=(( struct Cyc_Absyn_StructType_struct*) _temp186)->f2; goto
_LL197; _LL197: _temp196=(( struct Cyc_Absyn_StructType_struct*) _temp186)->f3;
goto _LL189;} else{ goto _LL190;} _LL190: if(( unsigned int) _temp186 >  3u?*((
int*) _temp186) ==  Cyc_Absyn_UnionType: 0){ _LL207: _temp206=(( struct Cyc_Absyn_UnionType_struct*)
_temp186)->f1; goto _LL205; _LL205: _temp204=(( struct Cyc_Absyn_UnionType_struct*)
_temp186)->f2; goto _LL203; _LL203: _temp202=(( struct Cyc_Absyn_UnionType_struct*)
_temp186)->f3; goto _LL191;} else{ goto _LL192;} _LL192: if(( unsigned int)
_temp186 >  3u?*(( int*) _temp186) ==  Cyc_Absyn_TunionType: 0){ _LL209:
_temp208=(( struct Cyc_Absyn_TunionType_struct*) _temp186)->f1; _LL215: _temp214=(
void*) _temp208.tunion_info; if(*(( int*) _temp214) ==  Cyc_Absyn_KnownTunion){
_LL217: _temp216=(( struct Cyc_Absyn_KnownTunion_struct*) _temp214)->f1;
_temp218=* _temp216; goto _LL213;} else{ goto _LL194;} _LL213: _temp212=
_temp208.targs; goto _LL211; _LL211: _temp210=( void*) _temp208.rgn; goto _LL193;}
else{ goto _LL194;} _LL194: goto _LL195; _LL189: if( _temp196 ==  0){({ void*
_temp219[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("struct decl must exist at this point", sizeof( unsigned char), 37u),
_tag_arr( _temp219, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structdecl*
_temp220=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp196)); struct Cyc_List_List*
_temp221= _temp220->tvs; if( Cyc_List_length( _temp198) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp221)){({ struct Cyc_Std_Int_pa_struct _temp224;
_temp224.tag= Cyc_Std_Int_pa; _temp224.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp221));{ struct Cyc_Std_Int_pa_struct
_temp223; _temp223.tag= Cyc_Std_Int_pa; _temp223.f1=( int)(( unsigned int) Cyc_List_length(
_temp198));{ void* _temp222[ 2u]={& _temp223,& _temp224};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof( unsigned char), 60u), _tag_arr( _temp222, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp225=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp221, _temp198); struct Cyc_Core_Opt*
fields= 0; if( _temp220->fields !=  0){ fields=({ struct Cyc_Core_Opt* _temp226=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp226->v=(
void*)(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_structfield_type,
_temp225,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp220->fields))->v);
_temp226;});}{ struct Cyc_Absyn_Structdecl* stdecl2=({ struct Cyc_Absyn_Structdecl*
_temp232=( struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp232->sc=( void*)(( void*) _temp220->sc); _temp232->name= _temp220->name;
_temp232->tvs= 0; _temp232->fields= fields; _temp232->attributes= _temp220->attributes;
_temp232;}); struct Cyc_Absyn_Structdecl** _temp227=({ struct Cyc_Absyn_Structdecl**
_temp231=( struct Cyc_Absyn_Structdecl**) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp231[ 0]= stdecl2; _temp231;}); struct Cyc_Absyn_StructType_struct* _temp228=({
struct Cyc_Absyn_StructType_struct* _temp229=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp229[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp230; _temp230.tag= Cyc_Absyn_StructType;
_temp230.f1= _temp200; _temp230.f2= _temp198; _temp230.f3= _temp227; _temp230;});
_temp229;}); return( void*) _temp228;}}} _LL191: if( _temp202 ==  0){({ void*
_temp233[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("struct decl must exist at this point", sizeof( unsigned char), 37u),
_tag_arr( _temp233, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Uniondecl*
_temp234=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp202)); struct Cyc_List_List*
_temp235= _temp234->tvs; if( Cyc_List_length( _temp204) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp235)){({ struct Cyc_Std_Int_pa_struct _temp238;
_temp238.tag= Cyc_Std_Int_pa; _temp238.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp235));{ struct Cyc_Std_Int_pa_struct
_temp237; _temp237.tag= Cyc_Std_Int_pa; _temp237.f1=( int)(( unsigned int) Cyc_List_length(
_temp204));{ void* _temp236[ 2u]={& _temp237,& _temp238};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof( unsigned char), 60u), _tag_arr( _temp236, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp239=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp235, _temp204); struct Cyc_Core_Opt*
fields= 0; if( _temp234->fields !=  0){ fields=({ struct Cyc_Core_Opt* _temp240=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp240->v=(
void*)(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_structfield_type,
_temp239,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp234->fields))->v);
_temp240;});}{ struct Cyc_Absyn_Uniondecl* udecl2=({ struct Cyc_Absyn_Uniondecl*
_temp246=( struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp246->sc=( void*)(( void*) _temp234->sc); _temp246->name= _temp234->name;
_temp246->tvs= 0; _temp246->fields= fields; _temp246->attributes= _temp234->attributes;
_temp246;}); struct Cyc_Absyn_Uniondecl** _temp241=({ struct Cyc_Absyn_Uniondecl**
_temp245=( struct Cyc_Absyn_Uniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp245[ 0]= udecl2; _temp245;}); struct Cyc_Absyn_UnionType_struct* _temp242=({
struct Cyc_Absyn_UnionType_struct* _temp243=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp243[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp244; _temp244.tag= Cyc_Absyn_UnionType; _temp244.f1=
_temp206; _temp244.f2= _temp204; _temp244.f3= _temp241; _temp244;}); _temp243;});
return( void*) _temp242;}}} _LL193: { struct Cyc_List_List* _temp247= _temp218->tvs;
if( Cyc_List_length( _temp212) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp247)){({ struct Cyc_Std_Int_pa_struct _temp250; _temp250.tag= Cyc_Std_Int_pa;
_temp250.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp247));{ struct Cyc_Std_Int_pa_struct _temp249; _temp249.tag= Cyc_Std_Int_pa;
_temp249.f1=( int)(( unsigned int) Cyc_List_length( _temp212));{ void* _temp248[
2u]={& _temp249,& _temp250};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof( unsigned char), 60u), _tag_arr( _temp248, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp251=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp247, _temp212); struct Cyc_Core_Opt*
fields= 0; if( _temp218->fields !=  0){ fields=({ struct Cyc_Core_Opt* _temp252=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp252->v=(
void*)(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tunionfield*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_tunionfield_type,
_temp251,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp218->fields))->v);
_temp252;});}{ struct Cyc_Absyn_Tuniondecl* _temp253=({ struct Cyc_Absyn_Tuniondecl*
_temp262=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp262->sc=( void*)(( void*) _temp218->sc); _temp262->name= _temp218->name;
_temp262->tvs= 0; _temp262->fields= fields; _temp262->is_xtunion= _temp218->is_xtunion;
_temp262;}); struct Cyc_Absyn_TunionType_struct* _temp254=({ struct Cyc_Absyn_TunionType_struct*
_temp256=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp256[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp257; _temp257.tag= Cyc_Absyn_TunionType;
_temp257.f1=({ struct Cyc_Absyn_TunionInfo _temp258; _temp258.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp259=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp259[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp260; _temp260.tag= Cyc_Absyn_KnownTunion;
_temp260.f1=({ struct Cyc_Absyn_Tuniondecl** _temp261=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp261[ 0]= _temp253;
_temp261;}); _temp260;}); _temp259;})); _temp258.targs= _temp212; _temp258.rgn=(
void*) _temp210; _temp258;}); _temp257;}); _temp256;}); struct Cyc_Position_Segment*
_temp255= Cyc_Position_segment_of_abs( 0, 0); return( void*) _temp254;}}} _LL195:
return type; _LL187:;} struct _tagged_arr Cyc_Tcgenrep_make_type_cstring( void*
t){ struct _tagged_arr s= Cyc_Std_strdup( Cyc_Absynpp_typ2cstring( t));{ int i=
0; for( 0; i <  Cyc_Std_strlen(( struct _tagged_arr) s); i ++){ if(*((
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)) == ' '){*((
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))='_';}
else{ if( ! isalnum(( int)*(( unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)))?*(( unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)) != '_': 0){*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))=( unsigned char)('0' + *((
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)) %  10);}}}}
return( struct _tagged_arr) Cyc_Std_strconcat(( struct _tagged_arr) s, _tag_arr("_rep",
sizeof( unsigned char), 5u));} struct _tuple12{ struct Cyc_Dict_Dict* f1; struct
Cyc_Tcgenrep_RepInfo* f2; } ; static struct _tuple12* Cyc_Tcgenrep_lookupRep(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment*
loc, void* type); struct _tuple13{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List*
f2; } ; struct _tuple14{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Position_Segment*
f2; } ; static struct _tuple13* Cyc_Tcgenrep_lookupRepsCls( struct _tuple14* env,
void* type, struct _tuple13* carry){ struct _tuple12 _temp265; struct Cyc_Tcgenrep_RepInfo*
_temp266; struct Cyc_Dict_Dict* _temp268; struct _tuple12* _temp263= Cyc_Tcgenrep_lookupRep((*
env).f1,(* carry).f1,(* env).f2, type); _temp265=* _temp263; _LL269: _temp268=
_temp265.f1; goto _LL267; _LL267: _temp266= _temp265.f2; goto _LL264; _LL264:
return({ struct _tuple13* _temp270=( struct _tuple13*) _cycalloc( sizeof( struct
_tuple13)); _temp270->f1= _temp268; _temp270->f2=({ struct Cyc_List_List*
_temp271=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp271->hd=( void*) _temp266; _temp271->tl=(* carry).f2; _temp271;}); _temp270;});}
static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepTuple( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, struct _tuple3*
varname, void* sc, void* type, struct Cyc_List_List* types){ struct Cyc_Absyn_Exp*
_temp272= Cyc_Absyn_sizeoftyp_exp( type, loc); struct _tuple14* _temp273=({
struct _tuple14* _temp300=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14));
_temp300->f1= te; _temp300->f2= loc; _temp300;}); struct _tuple13* _temp274=({
struct _tuple13* _temp299=( struct _tuple13*) _cycalloc( sizeof( struct _tuple13));
_temp299->f1= dict; _temp299->f2= 0; _temp299;}); struct _tuple13 _temp277;
struct Cyc_List_List* _temp278; struct Cyc_Dict_Dict* _temp280; struct _tuple13*
_temp275=(( struct _tuple13*(*)( struct _tuple13*(* f)( struct _tuple14*, void*,
struct _tuple13*), struct _tuple14*, struct Cyc_List_List* x, struct _tuple13*
accum)) Cyc_List_fold_right_c)( Cyc_Tcgenrep_lookupRepsCls, _temp273, types,
_temp274); _temp277=* _temp275; _LL281: _temp280= _temp277.f1; goto _LL279;
_LL279: _temp278= _temp277.f2; goto _LL276; _LL276: dict= _temp280;{ struct Cyc_List_List*
_temp282=(( struct Cyc_List_List*(*)( int n, int(* f)( int))) Cyc_List_tabulate)(
Cyc_List_length( types),( int(*)( int)) Cyc_Core_identity); struct Cyc_List_List*
_temp283=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple9*,
int), struct _tuple9* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple9* _temp298=( struct _tuple9*) _cycalloc( sizeof( struct _tuple9));
_temp298->f1= type; _temp298->f2= loc; _temp298;}), _temp282); struct Cyc_List_List*
_temp284=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp, loc, _temp278); struct
Cyc_List_List* _temp285=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Position_Segment*, struct _tuple8*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_tuple2_exp_cls, loc,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp283, _temp284)); void* tuple_type= Cyc_Tcgenrep_tuple_typ(({ void* _temp297[
2u]; _temp297[ 1u]= Cyc_Tcgenrep_tunion_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_temp297[ 0u]= Cyc_Absyn_uint_t; Cyc_List_list( _tag_arr( _temp297, sizeof( void*),
2u));})); struct _tuple10 _temp288; struct Cyc_Absyn_Exp* _temp289; struct Cyc_List_List*
_temp291; struct _tuple10* _temp286= Cyc_Tcgenrep_array_decls( tuple_type,
_temp285, loc); _temp288=* _temp286; _LL292: _temp291= _temp288.f1; goto _LL290;
_LL290: _temp289= _temp288.f2; goto _LL287; _LL287: { struct Cyc_Absyn_Decl*
_temp293= Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str), varname,({ struct Cyc_Absyn_Exp*
_temp296[ 2u]; _temp296[ 1u]= _temp289; _temp296[ 0u]= _temp272;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp296, sizeof( struct Cyc_Absyn_Exp*),
2u));}), sc, loc); struct Cyc_List_List* _temp294=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp291,({ struct Cyc_Absyn_Decl* _temp295[ 1u]; _temp295[ 0u]= _temp293;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp295, sizeof( struct Cyc_Absyn_Decl*), 1u));})); return Cyc_Tcgenrep_update_info(
dict, type, _temp294,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp(
varname, loc), loc), _temp278, 0);}}} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment*
loc, struct _tuple3* varname, void* sc, void* type, struct Cyc_List_List* types){
struct _tuple14* _temp301=({ struct _tuple14* _temp322=( struct _tuple14*)
_cycalloc( sizeof( struct _tuple14)); _temp322->f1= te; _temp322->f2= loc;
_temp322;}); struct _tuple13* _temp302=({ struct _tuple13* _temp321=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp321->f1= dict; _temp321->f2=
0; _temp321;}); struct _tuple13 _temp305; struct Cyc_List_List* _temp306; struct
Cyc_Dict_Dict* _temp308; struct _tuple13* _temp303=(( struct _tuple13*(*)(
struct _tuple13*(* f)( struct _tuple14*, void*, struct _tuple13*), struct
_tuple14*, struct Cyc_List_List* x, struct _tuple13* accum)) Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls, _temp301, types, _temp302); _temp305=* _temp303;
_LL309: _temp308= _temp305.f1; goto _LL307; _LL307: _temp306= _temp305.f2; goto
_LL304; _LL304: dict= _temp308;{ struct Cyc_List_List* _temp310=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp,
loc, _temp306); struct _tuple10 _temp313; struct Cyc_Absyn_Exp* _temp314; struct
Cyc_List_List* _temp316; struct _tuple10* _temp311= Cyc_Tcgenrep_array_decls(
Cyc_Tcgenrep_tunion_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),
_temp310, loc); _temp313=* _temp311; _LL317: _temp316= _temp313.f1; goto _LL315;
_LL315: _temp314= _temp313.f2; goto _LL312; _LL312: { struct Cyc_Absyn_Decl*
_temp318= Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str), varname,({ struct Cyc_Absyn_Exp*
_temp320[ 1u]; _temp320[ 0u]= _temp314;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp320, sizeof( struct Cyc_Absyn_Exp*),
1u));}), sc, loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp316,({ struct Cyc_Absyn_Decl* _temp319[ 1u]; _temp319[ 0u]= _temp318;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp319, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( varname, loc), loc), _temp306, 0);}}} static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepTunion( struct Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict*
dict, struct Cyc_Position_Segment* loc, struct _tuple3* varname, void* sc, void*
type, struct Cyc_List_List* types){ struct _tuple14* _temp323=({ struct _tuple14*
_temp349=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp349->f1=
te; _temp349->f2= loc; _temp349;}); struct _tuple13* _temp324=({ struct _tuple13*
_temp348=( struct _tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp348->f1=
dict; _temp348->f2= 0; _temp348;}); struct _tuple13 _temp327; struct Cyc_List_List*
_temp328; struct Cyc_Dict_Dict* _temp330; struct _tuple13* _temp325=(( struct
_tuple13*(*)( struct _tuple13*(* f)( struct _tuple14*, void*, struct _tuple13*),
struct _tuple14*, struct Cyc_List_List* x, struct _tuple13* accum)) Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls, _temp323, types, _temp324); _temp327=* _temp325;
_LL331: _temp330= _temp327.f1; goto _LL329; _LL329: _temp328= _temp327.f2; goto
_LL326; _LL326: dict= _temp330;{ struct Cyc_List_List* _temp332=(( struct Cyc_List_List*(*)(
int n, int(* f)( int))) Cyc_List_tabulate)((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp328),( int(*)( int)) Cyc_Core_identity); struct Cyc_List_List*
_temp333=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
int), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_int_cls, loc, _temp332); struct Cyc_List_List* _temp334=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp, loc, _temp328); struct
Cyc_List_List* _temp335=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp333, _temp334); struct Cyc_List_List*
_temp336=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcgenrep_tuple2_exp_cls, loc, _temp335); void* tuple_type=
Cyc_Tcgenrep_tuple_typ(({ void* _temp347[ 2u]; _temp347[ 1u]= Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)); _temp347[ 0u]= Cyc_Absyn_uint_t;
Cyc_List_list( _tag_arr( _temp347, sizeof( void*), 2u));})); struct _tuple10
_temp339; struct Cyc_Absyn_Exp* _temp340; struct Cyc_List_List* _temp342; struct
_tuple10* _temp337= Cyc_Tcgenrep_array_decls( tuple_type, _temp336, loc);
_temp339=* _temp337; _LL343: _temp342= _temp339.f1; goto _LL341; _LL341:
_temp340= _temp339.f2; goto _LL338; _LL338: { struct Cyc_Absyn_Decl* _temp344=
Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str), varname,({ struct Cyc_Absyn_Exp*
_temp346[ 1u]; _temp346[ 0u]= _temp340;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp346, sizeof( struct Cyc_Absyn_Exp*),
1u));}), sc, loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp342,({ struct Cyc_Absyn_Decl* _temp345[ 1u]; _temp345[ 0u]= _temp344;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp345, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( varname, loc), loc), _temp328, 0); return dict;}}}
static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepXTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, struct _tuple3*
varname, void* sc, void* type, struct Cyc_List_List* nametypes){ struct Cyc_List_List*
_temp352; struct Cyc_List_List* _temp354; struct _tuple0 _temp350=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( nametypes); _LL355:
_temp354= _temp350.f1; goto _LL353; _LL353: _temp352= _temp350.f2; goto _LL351;
_LL351: { struct _tuple14* _temp356=({ struct _tuple14* _temp382=( struct
_tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp382->f1= te; _temp382->f2=
loc; _temp382;}); struct _tuple13* _temp357=({ struct _tuple13* _temp381=(
struct _tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp381->f1= dict;
_temp381->f2= 0; _temp381;}); struct _tuple13 _temp360; struct Cyc_List_List*
_temp361; struct Cyc_Dict_Dict* _temp363; struct _tuple13* _temp358=(( struct
_tuple13*(*)( struct _tuple13*(* f)( struct _tuple14*, void*, struct _tuple13*),
struct _tuple14*, struct Cyc_List_List* x, struct _tuple13* accum)) Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls, _temp356, _temp352, _temp357); _temp360=* _temp358;
_LL364: _temp363= _temp360.f1; goto _LL362; _LL362: _temp361= _temp360.f2; goto
_LL359; _LL359: dict= _temp363;{ struct Cyc_List_List* _temp365=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct _tagged_arr*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_cnst_string_cls,
loc, _temp354); struct Cyc_List_List* _temp366=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp,
loc, _temp361); struct Cyc_List_List* _temp367=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp365,
_temp366); struct Cyc_List_List* _temp368=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct _tuple8*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_tuple2_exp_cls, loc,
_temp367); void* name_type= Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,({
struct Cyc_Absyn_Tqual _temp380; _temp380.q_const= 1; _temp380.q_volatile= 0;
_temp380.q_restrict= 0; _temp380;})); void* tuple_type= Cyc_Tcgenrep_tuple_typ(({
void* _temp379[ 2u]; _temp379[ 1u]= Cyc_Tcgenrep_tunion_typ( Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str)); _temp379[ 0u]= name_type; Cyc_List_list( _tag_arr(
_temp379, sizeof( void*), 2u));})); struct _tuple10 _temp371; struct Cyc_Absyn_Exp*
_temp372; struct Cyc_List_List* _temp374; struct _tuple10* _temp369= Cyc_Tcgenrep_array_decls(
tuple_type, _temp368, loc); _temp371=* _temp369; _LL375: _temp374= _temp371.f1;
goto _LL373; _LL373: _temp372= _temp371.f2; goto _LL370; _LL370: { struct Cyc_Absyn_Decl*
_temp376= Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_xtunion_str), varname,({ struct Cyc_Absyn_Exp*
_temp378[ 1u]; _temp378[ 0u]= _temp372;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp378, sizeof( struct Cyc_Absyn_Exp*),
1u));}), sc, loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp374,({ struct Cyc_Absyn_Decl* _temp377[ 1u]; _temp377[ 0u]= _temp376;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp377, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( varname, loc), loc), _temp361, 0); return dict;}}}}
static struct _tuple12* Cyc_Tcgenrep_lookupRep( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, void* type){
struct Cyc_Core_Opt* info=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
void* k)) Cyc_Dict_lookup_opt)( dict, type); if( info !=  0){ if((( struct Cyc_Tcgenrep_RepInfo*)
info->v)->is_extern){ dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*,
void*)) Cyc_Dict_delete)( dict, type);} else{ struct _tuple12* _temp383=({
struct _tuple12* _temp384=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp384->f1= dict; _temp384->f2=( struct Cyc_Tcgenrep_RepInfo*) info->v;
_temp384;}); return _temp383;}}{ void* _temp385= Cyc_Tcutil_compress( Cyc_Tcgenrep_monomorphize_type(
type)); void* _temp439; int _temp441; struct Cyc_Absyn_PtrInfo _temp443; struct
Cyc_Absyn_Exp* _temp445; struct Cyc_Absyn_Tqual _temp447; void* _temp449; struct
Cyc_List_List* _temp451; struct Cyc_Core_Opt* _temp453; struct Cyc_List_List*
_temp455; struct _tuple3* _temp457; struct Cyc_Absyn_Structdecl** _temp459;
struct Cyc_List_List* _temp461; struct _tuple3* _temp463; struct Cyc_List_List*
_temp465; struct Cyc_Absyn_Tvar* _temp467; struct Cyc_Absyn_FnInfo _temp469;
struct Cyc_Absyn_TunionInfo _temp471; void* _temp473; struct Cyc_List_List*
_temp475; void* _temp477; struct Cyc_Absyn_Tuniondecl** _temp479; struct Cyc_Absyn_Tuniondecl*
_temp481; struct Cyc_Absyn_TunionFieldInfo _temp482; struct Cyc_Absyn_Uniondecl**
_temp484; struct Cyc_List_List* _temp486; struct _tuple3* _temp488; struct Cyc_List_List*
_temp490; _LL387: if(( unsigned int) _temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_IntType:
0){ _LL440: _temp439=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp385)->f2;
goto _LL388;} else{ goto _LL389;} _LL389: if( _temp385 == ( void*) Cyc_Absyn_FloatType){
goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int) _temp385 >  3u?*((
int*) _temp385) ==  Cyc_Absyn_DoubleType: 0){ _LL442: _temp441=(( struct Cyc_Absyn_DoubleType_struct*)
_temp385)->f1; goto _LL392;} else{ goto _LL393;} _LL393: if(( unsigned int)
_temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_PointerType: 0){ _LL444:
_temp443=(( struct Cyc_Absyn_PointerType_struct*) _temp385)->f1; goto _LL394;}
else{ goto _LL395;} _LL395: if(( unsigned int) _temp385 >  3u?*(( int*) _temp385)
==  Cyc_Absyn_ArrayType: 0){ _LL450: _temp449=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp385)->f1; goto _LL448; _LL448: _temp447=(( struct Cyc_Absyn_ArrayType_struct*)
_temp385)->f2; goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_ArrayType_struct*)
_temp385)->f3; goto _LL396;} else{ goto _LL397;} _LL397: if(( unsigned int)
_temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_TupleType: 0){ _LL452: _temp451=((
struct Cyc_Absyn_TupleType_struct*) _temp385)->f1; goto _LL398;} else{ goto
_LL399;} _LL399: if(( unsigned int) _temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_TypedefType:
0){ _LL458: _temp457=(( struct Cyc_Absyn_TypedefType_struct*) _temp385)->f1;
goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_TypedefType_struct*) _temp385)->f2;
goto _LL454; _LL454: _temp453=(( struct Cyc_Absyn_TypedefType_struct*) _temp385)->f3;
goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp385 >  3u?*((
int*) _temp385) ==  Cyc_Absyn_StructType: 0){ _LL464: _temp463=(( struct Cyc_Absyn_StructType_struct*)
_temp385)->f1; goto _LL462; _LL462: _temp461=(( struct Cyc_Absyn_StructType_struct*)
_temp385)->f2; goto _LL460; _LL460: _temp459=(( struct Cyc_Absyn_StructType_struct*)
_temp385)->f3; goto _LL402;} else{ goto _LL403;} _LL403: if(( unsigned int)
_temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_AnonStructType: 0){ _LL466:
_temp465=(( struct Cyc_Absyn_AnonStructType_struct*) _temp385)->f1; goto _LL404;}
else{ goto _LL405;} _LL405: if( _temp385 == ( void*) Cyc_Absyn_VoidType){ goto
_LL406;} else{ goto _LL407;} _LL407: if(( unsigned int) _temp385 >  3u?*(( int*)
_temp385) ==  Cyc_Absyn_Evar: 0){ goto _LL408;} else{ goto _LL409;} _LL409: if((
unsigned int) _temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_VarType: 0){
_LL468: _temp467=(( struct Cyc_Absyn_VarType_struct*) _temp385)->f1; goto _LL410;}
else{ goto _LL411;} _LL411: if(( unsigned int) _temp385 >  3u?*(( int*) _temp385)
==  Cyc_Absyn_FnType: 0){ _LL470: _temp469=(( struct Cyc_Absyn_FnType_struct*)
_temp385)->f1; goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int)
_temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL414;}
else{ goto _LL415;} _LL415: if( _temp385 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL416;} else{ goto _LL417;} _LL417: if(( unsigned int) _temp385 >  3u?*(( int*)
_temp385) ==  Cyc_Absyn_AccessEff: 0){ goto _LL418;} else{ goto _LL419;} _LL419:
if(( unsigned int) _temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_JoinEff: 0){
goto _LL420;} else{ goto _LL421;} _LL421: if(( unsigned int) _temp385 >  3u?*((
int*) _temp385) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL422;} else{ goto _LL423;}
_LL423: if(( unsigned int) _temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_TunionType:
0){ _LL472: _temp471=(( struct Cyc_Absyn_TunionType_struct*) _temp385)->f1;
_LL478: _temp477=( void*) _temp471.tunion_info; if(*(( int*) _temp477) ==  Cyc_Absyn_KnownTunion){
_LL480: _temp479=(( struct Cyc_Absyn_KnownTunion_struct*) _temp477)->f1;
_temp481=* _temp479; goto _LL476;} else{ goto _LL425;} _LL476: _temp475=
_temp471.targs; goto _LL474; _LL474: _temp473=( void*) _temp471.rgn; goto _LL424;}
else{ goto _LL425;} _LL425: if(( unsigned int) _temp385 >  3u?*(( int*) _temp385)
==  Cyc_Absyn_TunionType: 0){ goto _LL426;} else{ goto _LL427;} _LL427: if((
unsigned int) _temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_TunionFieldType:
0){ _LL483: _temp482=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp385)->f1;
goto _LL428;} else{ goto _LL429;} _LL429: if(( unsigned int) _temp385 >  3u?*((
int*) _temp385) ==  Cyc_Absyn_UnionType: 0){ _LL489: _temp488=(( struct Cyc_Absyn_UnionType_struct*)
_temp385)->f1; goto _LL487; _LL487: _temp486=(( struct Cyc_Absyn_UnionType_struct*)
_temp385)->f2; goto _LL485; _LL485: _temp484=(( struct Cyc_Absyn_UnionType_struct*)
_temp385)->f3; goto _LL430;} else{ goto _LL431;} _LL431: if(( unsigned int)
_temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_AnonUnionType: 0){ _LL491:
_temp490=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp385)->f1; goto _LL432;}
else{ goto _LL433;} _LL433: if(( unsigned int) _temp385 >  3u?*(( int*) _temp385)
==  Cyc_Absyn_EnumType: 0){ goto _LL434;} else{ goto _LL435;} _LL435: if((
unsigned int) _temp385 >  3u?*(( int*) _temp385) ==  Cyc_Absyn_AnonEnumType: 0){
goto _LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp385 >  3u?*((
int*) _temp385) ==  Cyc_Absyn_SizeofType: 0){ goto _LL438;} else{ goto _LL386;}
_LL388: { struct _tuple3* _temp492= Cyc_Tcgenrep_toplevel_name( Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u))); struct Cyc_Absyn_Decl* _temp493=
Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_int_str), _temp492,({ struct Cyc_Absyn_Exp*
_temp495[ 1u]; _temp495[ 0u]= Cyc_Tcgenrep_cnst_int( Cyc_Tcgenrep_size_of2int(
_temp439), loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp495, sizeof( struct Cyc_Absyn_Exp*), 1u));}),( void*) Cyc_Absyn_Static,
loc); dict= Cyc_Tcgenrep_update_info( dict, type,({ struct Cyc_Absyn_Decl*
_temp494[ 1u]; _temp494[ 0u]= _temp493;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp494, sizeof( struct Cyc_Absyn_Decl*),
1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp(
_temp492, loc), loc), 0, 0); goto _LL386;} _LL390: dict= Cyc_Tcgenrep_update_info(
dict, type, 0,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_float_str), loc), 0, 0); goto _LL386; _LL392: dict= Cyc_Tcgenrep_update_info(
dict, type, 0,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_double_str), loc), 0, 0); goto _LL386; _LL394:{ void* _temp496= Cyc_Absyn_conref_val(
_temp443.bounds); struct Cyc_Absyn_Exp* _temp502; _LL498: if(( unsigned int)
_temp496 >  1u?*(( int*) _temp496) ==  Cyc_Absyn_Upper_b: 0){ _LL503: _temp502=((
struct Cyc_Absyn_Upper_b_struct*) _temp496)->f1; goto _LL499;} else{ goto _LL500;}
_LL500: if( _temp496 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL501;} else{ goto
_LL497;} _LL499: { struct _tuple3* _temp504= Cyc_Tcgenrep_toplevel_name( Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u))); struct Cyc_Core_Opt* _temp505=({
struct Cyc_Core_Opt* _temp517=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp517->v=( void*) Cyc_Tcgenrep_gen_vardecl( _temp504, Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str), Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str)), 0,( void*) Cyc_Absyn_Extern, loc); _temp517;}); dict=
Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( _temp504, loc), loc), _temp505, 0);{ struct _tuple12
_temp508; struct Cyc_Tcgenrep_RepInfo* _temp509; struct Cyc_Dict_Dict* _temp511;
struct _tuple12* _temp506= Cyc_Tcgenrep_lookupRep( te, dict, loc,( void*)
_temp443.elt_typ); _temp508=* _temp506; _LL512: _temp511= _temp508.f1; goto
_LL510; _LL510: _temp509= _temp508.f2; goto _LL507; _LL507: dict= _temp511;{
unsigned int bound= Cyc_Evexp_eval_const_uint_exp( _temp502); struct Cyc_Absyn_Decl*
_temp513= Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str), _temp504,({ struct Cyc_Absyn_Exp*
_temp516[ 2u]; _temp516[ 1u]=( struct Cyc_Absyn_Exp*) _check_null( _temp509->exp);
_temp516[ 0u]= Cyc_Tcgenrep_cnst_int(( int) bound, loc);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp516, sizeof( struct Cyc_Absyn_Exp*),
2u));}),( void*) Cyc_Absyn_Static, loc); dict= Cyc_Tcgenrep_update_info( dict,
type,({ struct Cyc_Absyn_Decl* _temp514[ 1u]; _temp514[ 0u]= _temp513;(( struct
Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp514,
sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( _temp504, loc), loc),({ struct Cyc_Tcgenrep_RepInfo*
_temp515[ 1u]; _temp515[ 0u]= _temp509;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp515, sizeof( struct Cyc_Tcgenrep_RepInfo*),
1u));}), 0); goto _LL497;}}} _LL501: { struct _tuple3* _temp518= Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof( unsigned char), 4u))); struct
Cyc_Core_Opt* _temp519=({ struct Cyc_Core_Opt* _temp531=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp531->v=( void*) Cyc_Tcgenrep_gen_vardecl(
_temp518, Cyc_Tcgenrep_tunionfield_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)), 0,( void*) Cyc_Absyn_Extern,
loc); _temp531;}); dict= Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( _temp518, loc),
loc), _temp519, 0);{ struct _tuple12 _temp522; struct Cyc_Tcgenrep_RepInfo*
_temp523; struct Cyc_Dict_Dict* _temp525; struct _tuple12* _temp520= Cyc_Tcgenrep_lookupRep(
te, dict, loc,( void*) _temp443.elt_typ); _temp522=* _temp520; _LL526: _temp525=
_temp522.f1; goto _LL524; _LL524: _temp523= _temp522.f2; goto _LL521; _LL521:
dict= _temp525;{ struct Cyc_Absyn_Decl* _temp527= Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str), Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_fatptr_str), _temp518,({ struct Cyc_Absyn_Exp* _temp530[ 1u];
_temp530[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp523->exp);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp530, sizeof( struct Cyc_Absyn_Exp*),
1u));}),( void*) Cyc_Absyn_Static, loc); dict= Cyc_Tcgenrep_update_info( dict,
type,({ struct Cyc_Absyn_Decl* _temp528[ 1u]; _temp528[ 0u]= _temp527;(( struct
Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp528,
sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( _temp518, loc), loc),({ struct Cyc_Tcgenrep_RepInfo*
_temp529[ 1u]; _temp529[ 0u]= _temp523;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp529, sizeof( struct Cyc_Tcgenrep_RepInfo*),
1u));}), 0); goto _LL497;}}} _LL497:;} goto _LL386; _LL396: if( _temp445 ==  0){({
void* _temp532[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("tcgenrep:At this point, array bounds must be constants",
sizeof( unsigned char), 55u), _tag_arr( _temp532, sizeof( void*), 0u));});}{
struct _tuple12 _temp535; struct Cyc_Tcgenrep_RepInfo* _temp536; struct Cyc_Dict_Dict*
_temp538; struct _tuple12* _temp533= Cyc_Tcgenrep_lookupRep( te, dict, loc,
_temp449); _temp535=* _temp533; _LL539: _temp538= _temp535.f1; goto _LL537;
_LL537: _temp536= _temp535.f2; goto _LL534; _LL534: dict= _temp538;{
unsigned int bound= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp445)); struct _tuple3* _temp540= Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof( unsigned char), 4u))); struct
Cyc_Absyn_Decl* _temp541= Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str), Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),
_temp540,({ struct Cyc_Absyn_Exp* _temp544[ 2u]; _temp544[ 1u]=( struct Cyc_Absyn_Exp*)
_check_null( _temp536->exp); _temp544[ 0u]= Cyc_Tcgenrep_cnst_int(( int) bound,
loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp544, sizeof( struct Cyc_Absyn_Exp*), 2u));}),( void*) Cyc_Absyn_Static, loc);
dict= Cyc_Tcgenrep_update_info( dict, type,({ struct Cyc_Absyn_Decl* _temp542[ 1u];
_temp542[ 0u]= _temp541;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp542, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( _temp540, loc), loc),({ struct
Cyc_Tcgenrep_RepInfo* _temp543[ 1u]; _temp543[ 0u]= _temp536;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp543, sizeof( struct Cyc_Tcgenrep_RepInfo*),
1u));}), 0); goto _LL386;}} _LL398: { struct _tuple3* _temp545= Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof( unsigned char), 4u))); struct
Cyc_Core_Opt* _temp546=({ struct Cyc_Core_Opt* _temp548=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp548->v=( void*) Cyc_Tcgenrep_gen_vardecl(
_temp545, Cyc_Tcgenrep_tunionfield_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)), 0,( void*) Cyc_Absyn_Extern,
loc); _temp548;}); dict= Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( _temp545, loc),
loc), _temp546, 0);{ struct Cyc_List_List* _temp547=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_get_second,
_temp451); dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc, _temp545,( void*)
Cyc_Absyn_Static, type, _temp547); goto _LL386;}} _LL400: if( _temp453 ==  0){({
void* _temp549[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("gen(): typedef definition must be present by now",
sizeof( unsigned char), 49u), _tag_arr( _temp549, sizeof( void*), 0u));});}{
struct _tuple12 _temp552; struct Cyc_Tcgenrep_RepInfo* _temp553; struct Cyc_Dict_Dict*
_temp555; struct _tuple12* _temp550= Cyc_Tcgenrep_lookupRep( te, dict, loc,(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp453))->v); _temp552=* _temp550;
_LL556: _temp555= _temp552.f1; goto _LL554; _LL554: _temp553= _temp552.f2; goto
_LL551; _LL551: dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
k, struct Cyc_Tcgenrep_RepInfo* v)) Cyc_Dict_insert)( _temp555, type, _temp553);
goto _LL386;} _LL402: if( _temp459 ==  0){({ void* _temp557[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): struct declaration must be present by now",
sizeof( unsigned char), 49u), _tag_arr( _temp557, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* _temp558=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp459)); if( _temp558->fields ==  0){ struct _tuple3* _temp559=
Cyc_Tcgenrep_toplevel_name(({ struct _tagged_arr* _temp562=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp562[ 0]= Cyc_Tcgenrep_make_type_cstring(
type); _temp562;})); struct Cyc_Core_Opt* _temp560=({ struct Cyc_Core_Opt*
_temp561=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp561->v=( void*) Cyc_Tcgenrep_gen_vardecl( _temp559, Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str), Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str)), 0,( void*) Cyc_Absyn_Extern, loc); _temp561;}); dict=
Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( _temp559, loc), loc), 0, 1); goto _LL386;} if(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp558->fields))->v !=  0?((
int(*)( int(* pred)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x))
Cyc_List_forall)( Cyc_Tcgenrep_has_bitfield,( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp558->fields))->v): 0){ int _temp563=(( int(*)(
int(* f)( int, struct Cyc_Absyn_Structfield*), int accum, struct Cyc_List_List*
x)) Cyc_List_fold_left)( Cyc_Tcgenrep_add_bitfield_sizes, 0,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp558->fields))->v); int _temp564=
_temp563 /  8 + ( _temp563 %  8 ==  0? 0: 1); struct Cyc_List_List* chars= 0;{
int i= 0; for( 0; i <  _temp564; i ++){ chars=({ struct Cyc_List_List* _temp565=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp565->hd=(
void*)({ struct _tuple6* _temp566=( struct _tuple6*) _cycalloc( sizeof( struct
_tuple6)); _temp566->f1= Cyc_Tcgenrep_tq_none; _temp566->f2= Cyc_Absyn_uchar_t;
_temp566;}); _temp565->tl= chars; _temp565;});}}{ void* base_type=( void*)({
struct Cyc_Absyn_TupleType_struct* _temp574=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp574[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp575; _temp575.tag= Cyc_Absyn_TupleType; _temp575.f1=
chars; _temp575;}); _temp574;}); struct _tuple12 _temp569; struct Cyc_Tcgenrep_RepInfo*
_temp570; struct Cyc_Dict_Dict* _temp572; struct _tuple12* _temp567= Cyc_Tcgenrep_lookupRep(
te, dict, loc, base_type); _temp569=* _temp567; _LL573: _temp572= _temp569.f1;
goto _LL571; _LL571: _temp570= _temp569.f2; goto _LL568; _LL568: dict=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k, struct Cyc_Tcgenrep_RepInfo*
v)) Cyc_Dict_insert)( _temp572, type, _temp570);}} else{ struct _tuple3*
_temp576= Cyc_Tcgenrep_toplevel_name(({ struct _tagged_arr* _temp580=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp580[ 0]= Cyc_Tcgenrep_make_type_cstring(
type); _temp580;})); struct Cyc_Core_Opt* _temp577=({ struct Cyc_Core_Opt*
_temp579=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp579->v=( void*) Cyc_Tcgenrep_gen_vardecl( _temp576, Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str), Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str)), 0,( void*) Cyc_Absyn_Extern, loc); _temp579;}); dict=
Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( _temp576, loc), loc), _temp577, 0);{ struct Cyc_List_List*
_temp578=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp558->fields))->v);
dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc, _temp576,( void*) Cyc_Absyn_Public,
type, _temp578);}} goto _LL386;} _LL404: if( _temp465 !=  0?(( int(*)( int(*
pred)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Tcgenrep_has_bitfield, _temp465): 0){ int _temp581=(( int(*)( int(* f)( int,
struct Cyc_Absyn_Structfield*), int accum, struct Cyc_List_List* x)) Cyc_List_fold_left)(
Cyc_Tcgenrep_add_bitfield_sizes, 0, _temp465); int _temp582= _temp581 /  8 + (
_temp581 %  8 ==  0? 0: 1); void* base_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp590=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp590[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp591; _temp591.tag= Cyc_Absyn_ArrayType;
_temp591.f1=( void*) Cyc_Absyn_uchar_t; _temp591.f2= Cyc_Tcgenrep_tq_none;
_temp591.f3=( struct Cyc_Absyn_Exp*) Cyc_Tcgenrep_cnst_int( _temp582, loc);
_temp591;}); _temp590;}); struct _tuple12 _temp585; struct Cyc_Tcgenrep_RepInfo*
_temp586; struct Cyc_Dict_Dict* _temp588; struct _tuple12* _temp583= Cyc_Tcgenrep_lookupRep(
te, dict, loc, base_type); _temp585=* _temp583; _LL589: _temp588= _temp585.f1;
goto _LL587; _LL587: _temp586= _temp585.f2; goto _LL584; _LL584: dict=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k, struct Cyc_Tcgenrep_RepInfo*
v)) Cyc_Dict_insert)( _temp588, type, _temp586);} else{ struct Cyc_List_List*
_temp592=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,
_temp465); struct _tuple3* _temp593= Cyc_Tcgenrep_toplevel_name( Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u))); dict= Cyc_Tcgenrep_buildRepTuple(
te, dict, loc, _temp593,( void*) Cyc_Absyn_Static, type, _temp592);} goto _LL386;
_LL406:({ void* _temp594[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found void in gen() expression",
sizeof( unsigned char), 31u), _tag_arr( _temp594, sizeof( void*), 0u));}); goto
_LL386; _LL408:({ void* _temp595[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found evar in gen() expression",
sizeof( unsigned char), 31u), _tag_arr( _temp595, sizeof( void*), 0u));}); goto
_LL386; _LL410:({ void* _temp596[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found tyvar in gen() expression",
sizeof( unsigned char), 32u), _tag_arr( _temp596, sizeof( void*), 0u));}); goto
_LL386;({ void* _temp597[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found variable type in gen() expression",
sizeof( unsigned char), 40u), _tag_arr( _temp597, sizeof( void*), 0u));}); goto
_LL386; _LL412:({ void* _temp598[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found function type in gen() expression",
sizeof( unsigned char), 40u), _tag_arr( _temp598, sizeof( void*), 0u));}); goto
_LL386; _LL414: goto _LL416; _LL416: goto _LL418; _LL418: goto _LL420; _LL420:
goto _LL422; _LL422:({ void* _temp599[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen(): unhandled region, handle or effect type",
sizeof( unsigned char), 47u), _tag_arr( _temp599, sizeof( void*), 0u));}); goto
_LL386; _LL424: if( _temp481->tvs !=  0){({ void* _temp600[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunion type variables not handled yet",
sizeof( unsigned char), 45u), _tag_arr( _temp600, sizeof( void*), 0u));});} if(
_temp481->fields ==  0){ struct _tuple3* _temp601= Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr* _temp604=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp604[ 0]= Cyc_Tcgenrep_make_type_cstring( type); _temp604;}));
struct Cyc_Core_Opt* _temp602=({ struct Cyc_Core_Opt* _temp603=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp603->v=( void*) Cyc_Tcgenrep_gen_vardecl(
_temp601, Cyc_Tcgenrep_tunionfield_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)), 0,( void*) Cyc_Absyn_Extern,
loc); _temp603;}); dict= Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( _temp601, loc),
loc), 0, 1); goto _LL386;} if( ! _temp481->is_xtunion){ struct _tuple3* _temp605=
Cyc_Tcgenrep_toplevel_name(({ struct _tagged_arr* _temp611=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp611[ 0]= Cyc_Tcgenrep_make_type_cstring(
type); _temp611;})); struct Cyc_Core_Opt* _temp606=({ struct Cyc_Core_Opt*
_temp610=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp610->v=( void*) Cyc_Tcgenrep_gen_vardecl( _temp605, Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str), Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)), 0,( void*) Cyc_Absyn_Extern, loc); _temp610;}); dict=
Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( _temp605, loc), loc), _temp606, 0);{ struct Cyc_List_List*
_temp607=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp481->fields))->v;
struct Cyc_List_List* _temp608=(( struct Cyc_List_List*(*)( int(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* x)) Cyc_List_filter)( Cyc_Tcgenrep_filter_empty_tunionfield,
_temp607); struct Cyc_List_List* _temp609=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_check_tunionfield_and_get_type,
_temp608); dict= Cyc_Tcgenrep_buildRepTunion( te, dict, loc, _temp605,( void*)
Cyc_Absyn_Public, type, _temp609);}} else{ struct _tuple3* _temp612= Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr* _temp618=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp618[ 0]= Cyc_Tcgenrep_make_type_cstring( type); _temp618;}));
struct Cyc_Core_Opt* _temp613=({ struct Cyc_Core_Opt* _temp617=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp617->v=( void*) Cyc_Tcgenrep_gen_vardecl(
_temp612, Cyc_Tcgenrep_tunionfield_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_xtunion_str)), 0,( void*) Cyc_Absyn_Extern,
loc); _temp617;}); dict= Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( _temp612, loc),
loc), _temp613, 0);{ struct Cyc_List_List* _temp614=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp481->fields))->v; struct Cyc_List_List*
_temp615=(( struct Cyc_List_List*(*)( int(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* x)) Cyc_List_filter)( Cyc_Tcgenrep_filter_empty_tunionfield,
_temp614); struct Cyc_List_List* _temp616=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcgenrep_check_xtunionfield_and_get_name_type, _temp615); dict= Cyc_Tcgenrep_buildRepXTunion(
te, dict, loc, _temp612,( void*) Cyc_Absyn_Public, type, _temp616);}} goto
_LL386; _LL426:({ void* _temp619[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunion must be resolved by now",
sizeof( unsigned char), 38u), _tag_arr( _temp619, sizeof( void*), 0u));}); goto
_LL386; _LL428: if( _temp482.targs !=  0){({ void* _temp620[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof( unsigned char), 51u), _tag_arr( _temp620, sizeof( void*), 0u));});}{
void* _temp621=( void*) _temp482.field_info; struct Cyc_Absyn_Tunionfield*
_temp627; struct Cyc_Absyn_Tuniondecl* _temp629; _LL623: if(*(( int*) _temp621)
==  Cyc_Absyn_KnownTunionfield){ _LL630: _temp629=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp621)->f1; goto _LL628; _LL628: _temp627=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp621)->f2; goto _LL624;} else{ goto _LL625;} _LL625: goto _LL626; _LL624:
if( _temp627->tvs !=  0){({ void* _temp631[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof( unsigned char), 51u), _tag_arr( _temp631, sizeof( void*), 0u));});}{
struct Cyc_List_List* _temp632=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_get_second,
_temp627->typs); struct Cyc_List_List* _temp633=({ struct Cyc_List_List*
_temp635=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp635->hd=( void*) Cyc_Absyn_uint_t; _temp635->tl= _temp632; _temp635;});
struct _tuple3* _temp634= Cyc_Tcgenrep_toplevel_name( Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u))); dict= Cyc_Tcgenrep_buildRepTuple(
te, dict, loc, _temp634,( void*) Cyc_Absyn_Static, type, _temp633); goto _LL622;}
_LL626:({ void* _temp636[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield must be known at this point",
sizeof( unsigned char), 47u), _tag_arr( _temp636, sizeof( void*), 0u));}); goto
_LL622; _LL622:;} goto _LL386; _LL430: if( _temp484 ==  0){({ void* _temp637[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): union declaration must be present by now", sizeof(
unsigned char), 48u), _tag_arr( _temp637, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Uniondecl*
_temp638=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp484)); if( _temp638->tvs
!=  0){({ void* _temp639[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen: unions with parameters not yet supported",
sizeof( unsigned char), 46u), _tag_arr( _temp639, sizeof( void*), 0u));});} if(
_temp638->fields ==  0){ struct _tuple3* _temp640= Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr* _temp643=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp643[ 0]= Cyc_Tcgenrep_make_type_cstring( type); _temp643;}));
struct Cyc_Core_Opt* _temp641=({ struct Cyc_Core_Opt* _temp642=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp642->v=( void*) Cyc_Tcgenrep_gen_vardecl(
_temp640, Cyc_Tcgenrep_tunionfield_typ( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str)), 0,( void*) Cyc_Absyn_Extern,
loc); _temp642;}); dict= Cyc_Tcgenrep_make_fwd_decl_info( dict, type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( _temp640, loc),
loc), 0, 1); goto _LL386;}{ struct Cyc_List_List* _temp644=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_type,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp638->fields))->v); struct _tuple3* _temp645= Cyc_Tcgenrep_toplevel_name(({
struct _tagged_arr* _temp646=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp646[ 0]= Cyc_Tcgenrep_make_type_cstring( type); _temp646;}));
dict= Cyc_Tcgenrep_buildRepUnion( te, dict, loc, _temp645,( void*) Cyc_Absyn_Public,
type, _temp644); goto _LL386;}} _LL432: { struct Cyc_List_List* _temp647=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type, _temp490); struct
_tuple3* _temp648= Cyc_Tcgenrep_toplevel_name( Cyc_Tcgenrep_new_gen_id( _tag_arr("rep",
sizeof( unsigned char), 4u))); dict= Cyc_Tcgenrep_buildRepUnion( te, dict, loc,
_temp648,( void*) Cyc_Absyn_Static, type, _temp647); goto _LL386;} _LL434: goto
_LL436; _LL436: { struct _tuple3* _temp649= Cyc_Tcgenrep_toplevel_name( Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u))); struct Cyc_Absyn_Decl* _temp650=
Cyc_Tcgenrep_tunion_constructor_decl( Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_int_str), _temp649,({ struct Cyc_Absyn_Exp*
_temp652[ 1u]; _temp652[ 0u]= Cyc_Tcgenrep_cnst_int( 32, loc);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp652, sizeof( struct Cyc_Absyn_Exp*),
1u));}),( void*) Cyc_Absyn_Static, loc); dict= Cyc_Tcgenrep_update_info( dict,
type,({ struct Cyc_Absyn_Decl* _temp651[ 1u]; _temp651[ 0u]= _temp650;(( struct
Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp651,
sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( _temp649, loc), loc), 0, 0); goto _LL386;} _LL438:({
void* _temp653[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("gen() for sizeof_t<-> not yet supported", sizeof(
unsigned char), 40u), _tag_arr( _temp653, sizeof( void*), 0u));}); goto _LL386;
_LL386:;} return({ struct _tuple12* _temp654=( struct _tuple12*) _cycalloc(
sizeof( struct _tuple12)); _temp654->f1= dict; _temp654->f2=(( struct Cyc_Tcgenrep_RepInfo*(*)(
struct Cyc_Dict_Dict* d, void* k)) Cyc_Dict_lookup)( dict, type); _temp654;});}
static int Cyc_Tcgenrep_not_emitted_filter( struct Cyc_Tcgenrep_RepInfo* ri){
return ri->emitted ==  0;} static void Cyc_Tcgenrep_mark_emitted( struct Cyc_Tcgenrep_RepInfo*
ri){ ri->emitted= 1;} struct _tuple15{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple15 Cyc_Tcgenrep_tcGenrep( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
void* type, struct Cyc_Dict_Dict* dict){ Cyc_Tcutil_check_valid_toplevel_type(
loc, te, type); Cyc_Tcutil_check_valid_toplevel_type( loc, te, type);{ struct
_tuple12 _temp657; struct Cyc_Tcgenrep_RepInfo* _temp658; struct Cyc_Dict_Dict*
_temp660; struct _tuple12* _temp655= Cyc_Tcgenrep_lookupRep( te, dict, loc, type);
_temp657=* _temp655; _LL661: _temp660= _temp657.f1; goto _LL659; _LL659:
_temp658= _temp657.f2; goto _LL656; _LL656: { struct Cyc_List_List* _temp662=
Cyc_Tcgenrep_dfs( _temp658); return({ struct _tuple15 _temp663; _temp663.f1=
_temp660; _temp663.f2= _temp662; _temp663.f3=( struct Cyc_Absyn_Exp*)
_check_null( _temp658->exp); _temp663;});}}}
