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
extern struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_Invalid_argument[
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stdout; extern
struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*);
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
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
int tag; float* f1; } ; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_of_abs( struct _tagged_arr, int); extern void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places); extern unsigned char
Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position( struct
_tagged_arr); extern void Cyc_Position_set_position_file( struct _tagged_arr);
extern struct _tagged_arr Cyc_Position_get_position_file(); struct Cyc_Position_Segment;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_arr Cyc_Position_string_of_loc(
int); extern struct _tagged_arr Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*, struct _tagged_arr); extern struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*, struct _tagged_arr);
extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*,
struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_print_context; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
extern int Cyc_Position_error_p(); extern unsigned int Cyc_Std_strlen( struct
_tagged_arr s); extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern struct _tagged_arr Cyc_Std_strncpy( struct _tagged_arr,
struct _tagged_arr, unsigned int); extern struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Typerep_Var= 0; struct
Cyc_Typerep_Var_struct{ int tag; struct _tagged_arr* f1; } ; static const int
Cyc_Typerep_Int= 1; struct Cyc_Typerep_Int_struct{ int tag; unsigned int f1; } ;
static const int Cyc_Typerep_Float= 0; static const int Cyc_Typerep_Double= 1;
static const int Cyc_Typerep_ThinPtr= 2; struct Cyc_Typerep_ThinPtr_struct{ int
tag; unsigned int f1; void* f2; } ; static const int Cyc_Typerep_FatPtr= 3;
struct Cyc_Typerep_FatPtr_struct{ int tag; void* f1; } ; static const int Cyc_Typerep_Tuple=
4; struct _tuple0{ unsigned int f1; void* f2; } ; struct Cyc_Typerep_Tuple_struct{
int tag; unsigned int f1; struct _tagged_arr f2; } ; static const int Cyc_Typerep_TUnion=
5; struct Cyc_Typerep_TUnion_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_XTUnion= 6; struct _tuple1{ struct _tagged_arr f1; void*
f2; } ; struct Cyc_Typerep_XTUnion_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Typerep_Union= 7; struct Cyc_Typerep_Union_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Typerep_Forall= 8; struct Cyc_Typerep_Forall_struct{
int tag; struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App=
9; struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
unsigned int Cyc_Typerep_size_type( void* rep); extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Position_Segment_rep; unsigned char Cyc_Position_Exit[ 9u]="\000\000\000\000Exit";
static unsigned char _temp0[ 1u]=""; static struct _tagged_arr Cyc_Position_source={
_temp0, _temp0, _temp0 +  1u}; struct Cyc_Position_Segment{ int start; int end;
} ; struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int start, int end){
return({ struct Cyc_Position_Segment* _temp1=( struct Cyc_Position_Segment*)
_cycalloc_atomic( sizeof( struct Cyc_Position_Segment)); _temp1->start= start;
_temp1->end= end; _temp1;});} struct Cyc_Position_Segment* Cyc_Position_segment_join(
struct Cyc_Position_Segment* s1, struct Cyc_Position_Segment* s2){ if( s1 ==  0){
return s2;} if( s2 ==  0){ return s1;} return({ struct Cyc_Position_Segment*
_temp2=( struct Cyc_Position_Segment*) _cycalloc_atomic( sizeof( struct Cyc_Position_Segment));
_temp2->start= s1->start; _temp2->end= s2->end; _temp2;});} struct _tagged_arr
Cyc_Position_string_of_loc( int loc){ struct Cyc_Lineno_Pos* pos= Cyc_Lineno_pos_of_abs(
Cyc_Position_source, loc); return({ struct Cyc_Std_Int_pa_struct _temp6; _temp6.tag=
Cyc_Std_Int_pa; _temp6.f1=( int)(( unsigned int) pos->col);{ struct Cyc_Std_Int_pa_struct
_temp5; _temp5.tag= Cyc_Std_Int_pa; _temp5.f1=( int)(( unsigned int) pos->line_no);{
struct Cyc_Std_String_pa_struct _temp4; _temp4.tag= Cyc_Std_String_pa; _temp4.f1=(
struct _tagged_arr) pos->logical_file;{ void* _temp3[ 3u]={& _temp4,& _temp5,&
_temp6}; Cyc_Std_aprintf( _tag_arr("%s (%d:%d)", sizeof( unsigned char), 11u),
_tag_arr( _temp3, sizeof( void*), 3u));}}}});} static struct _tagged_arr Cyc_Position_string_of_pos_pr(
struct Cyc_Lineno_Pos* pos_s, struct Cyc_Lineno_Pos* pos_e){ if( Cyc_Std_strcmp(
pos_s->logical_file, pos_e->logical_file) ==  0){ return({ struct Cyc_Std_Int_pa_struct
_temp12; _temp12.tag= Cyc_Std_Int_pa; _temp12.f1=( int)(( unsigned int) pos_e->col);{
struct Cyc_Std_Int_pa_struct _temp11; _temp11.tag= Cyc_Std_Int_pa; _temp11.f1=(
int)(( unsigned int) pos_e->line_no);{ struct Cyc_Std_Int_pa_struct _temp10;
_temp10.tag= Cyc_Std_Int_pa; _temp10.f1=( int)(( unsigned int) pos_s->col);{
struct Cyc_Std_Int_pa_struct _temp9; _temp9.tag= Cyc_Std_Int_pa; _temp9.f1=( int)((
unsigned int) pos_s->line_no);{ struct Cyc_Std_String_pa_struct _temp8; _temp8.tag=
Cyc_Std_String_pa; _temp8.f1=( struct _tagged_arr) pos_s->logical_file;{ void*
_temp7[ 5u]={& _temp8,& _temp9,& _temp10,& _temp11,& _temp12}; Cyc_Std_aprintf(
_tag_arr("%s(%d:%d-%d:%d)", sizeof( unsigned char), 16u), _tag_arr( _temp7,
sizeof( void*), 5u));}}}}}});} else{ return({ struct Cyc_Std_Int_pa_struct
_temp19; _temp19.tag= Cyc_Std_Int_pa; _temp19.f1=( int)(( unsigned int) pos_e->col);{
struct Cyc_Std_Int_pa_struct _temp18; _temp18.tag= Cyc_Std_Int_pa; _temp18.f1=(
int)(( unsigned int) pos_e->line_no);{ struct Cyc_Std_String_pa_struct _temp17;
_temp17.tag= Cyc_Std_String_pa; _temp17.f1=( struct _tagged_arr) pos_e->logical_file;{
struct Cyc_Std_Int_pa_struct _temp16; _temp16.tag= Cyc_Std_Int_pa; _temp16.f1=(
int)(( unsigned int) pos_s->col);{ struct Cyc_Std_Int_pa_struct _temp15; _temp15.tag=
Cyc_Std_Int_pa; _temp15.f1=( int)(( unsigned int) pos_s->line_no);{ struct Cyc_Std_String_pa_struct
_temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=( struct _tagged_arr) pos_s->logical_file;{
void* _temp13[ 6u]={& _temp14,& _temp15,& _temp16,& _temp17,& _temp18,& _temp19};
Cyc_Std_aprintf( _tag_arr("%s(%d:%d)-%s(%d:%d)", sizeof( unsigned char), 20u),
_tag_arr( _temp13, sizeof( void*), 6u));}}}}}}});}} struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment* s){ if( s ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp21; _temp21.tag= Cyc_Std_String_pa; _temp21.f1=( struct _tagged_arr) Cyc_Position_source;{
void* _temp20[ 1u]={& _temp21}; Cyc_Std_aprintf( _tag_arr("%s", sizeof(
unsigned char), 3u), _tag_arr( _temp20, sizeof( void*), 1u));}});}{ struct Cyc_Lineno_Pos*
pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source, s->start); struct Cyc_Lineno_Pos*
pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source, s->end); return Cyc_Position_string_of_pos_pr(
pos_s, pos_e);}} static struct Cyc_Lineno_Pos* Cyc_Position_new_pos(){ return({
struct Cyc_Lineno_Pos* _temp22=( struct Cyc_Lineno_Pos*) _cycalloc( sizeof(
struct Cyc_Lineno_Pos)); _temp22->logical_file= _tag_arr("", sizeof(
unsigned char), 1u); _temp22->line= Cyc_Core_new_string( 0); _temp22->line_no= 0;
_temp22->col= 0; _temp22;});} struct _tuple2{ int f1; struct Cyc_Lineno_Pos* f2;
} ; struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*
segs){ struct Cyc_List_List* places= 0;{ struct Cyc_List_List* _temp23= segs;
for( 0; _temp23 !=  0; _temp23= _temp23->tl){ if(( struct Cyc_Position_Segment*)
_temp23->hd ==  0){ continue;} places=({ struct Cyc_List_List* _temp24=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp24->hd=( void*)({
struct _tuple2* _temp27=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp27->f1=(( struct Cyc_Position_Segment*) _check_null(( struct Cyc_Position_Segment*)
_temp23->hd))->end; _temp27->f2= Cyc_Position_new_pos(); _temp27;}); _temp24->tl=({
struct Cyc_List_List* _temp25=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp25->hd=( void*)({ struct _tuple2* _temp26=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp26->f1=(( struct Cyc_Position_Segment*)
_check_null(( struct Cyc_Position_Segment*) _temp23->hd))->start; _temp26->f2=
Cyc_Position_new_pos(); _temp26;}); _temp25->tl= places; _temp25;}); _temp24;});}}
Cyc_Lineno_poss_of_abss( Cyc_Position_source, places);{ struct Cyc_List_List*
ans= 0; places=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
places); for( 0; segs !=  0; segs= segs->tl){ if(( struct Cyc_Position_Segment*)
segs->hd ==  0){ ans=({ struct Cyc_List_List* _temp28=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp28->hd=( void*)({ struct
_tagged_arr* _temp29=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp29[ 0]=({ struct Cyc_Std_String_pa_struct _temp31; _temp31.tag=
Cyc_Std_String_pa; _temp31.f1=( struct _tagged_arr) Cyc_Position_source;{ void*
_temp30[ 1u]={& _temp31}; Cyc_Std_aprintf( _tag_arr("%s(unknown)", sizeof(
unsigned char), 12u), _tag_arr( _temp30, sizeof( void*), 1u));}}); _temp29;});
_temp28->tl= ans; _temp28;});} else{ ans=({ struct Cyc_List_List* _temp32=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp32->hd=(
void*)({ struct _tagged_arr* _temp33=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp33[ 0]= Cyc_Position_string_of_pos_pr((*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( places))->hd)).f2,(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( places))->tl))->hd)).f2); _temp33;}); _temp32->tl= ans; _temp32;});
places=(( struct Cyc_List_List*) _check_null( places->tl))->tl;}} return ans;}}
struct Cyc_Position_Error; struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment* l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error*
_temp34=( struct Cyc_Position_Error*) _cycalloc( sizeof( struct Cyc_Position_Error));
_temp34->source= Cyc_Position_source; _temp34->seg= l; _temp34->kind=( void*)((
void*) Cyc_Position_Lex); _temp34->desc= desc; _temp34;});} struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment* l, struct _tagged_arr
desc){ return({ struct Cyc_Position_Error* _temp35=( struct Cyc_Position_Error*)
_cycalloc( sizeof( struct Cyc_Position_Error)); _temp35->source= Cyc_Position_source;
_temp35->seg= l; _temp35->kind=( void*)(( void*) Cyc_Position_Parse); _temp35->desc=
desc; _temp35;});} struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct
Cyc_Position_Segment* l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error*
_temp36=( struct Cyc_Position_Error*) _cycalloc( sizeof( struct Cyc_Position_Error));
_temp36->source= Cyc_Position_source; _temp36->seg= l; _temp36->kind=( void*)((
void*) Cyc_Position_Elab); _temp36->desc= desc; _temp36;});} unsigned char Cyc_Position_Nocontext[
14u]="\000\000\000\000Nocontext"; static struct _tagged_arr Cyc_Position_trunc(
int n, struct _tagged_arr s){ int len=( int) Cyc_Std_strlen(( struct _tagged_arr)
s); if( len <  n){ return s;}{ int len_one=( n -  3) /  2; int len_two=( n -  3)
-  len_one; struct _tagged_arr ans= Cyc_Core_new_string( n +  1); Cyc_Std_strncpy(
ans,( struct _tagged_arr) s,( unsigned int) len_one); Cyc_Std_strncpy(
_tagged_arr_plus( ans, sizeof( unsigned char), len_one), _tag_arr("...", sizeof(
unsigned char), 4u), 3); Cyc_Std_strncpy( _tagged_arr_plus( _tagged_arr_plus(
ans, sizeof( unsigned char), len_one), sizeof( unsigned char), 3),( struct
_tagged_arr) _tagged_arr_plus( _tagged_arr_plus( s, sizeof( unsigned char), len),
sizeof( unsigned char), - len_two),( unsigned int) len_two); return ans;}}
static int Cyc_Position_line_length= 76; struct _tuple3{ struct _tagged_arr f1;
int f2; int f3; } ; static struct _tuple3* Cyc_Position_get_context( struct Cyc_Position_Segment*
seg){ if( seg ==  0){( int) _throw(( void*) Cyc_Position_Nocontext);}{ struct
Cyc_Lineno_Pos* pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct _handler_cons
_temp37; _push_handler(& _temp37);{ int _temp39= 0; if( setjmp( _temp37.handler)){
_temp39= 1;} if( ! _temp39){ pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,
seg->start); pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source, seg->end);;
_pop_handler();} else{ void* _temp38=( void*) _exn_thrown; void* _temp41=
_temp38; _LL43: goto _LL44; _LL45: goto _LL46; _LL44:( int) _throw(( void*) Cyc_Position_Nocontext);
_LL46:( void) _throw( _temp41); _LL42:;}}}{ struct Cyc_Lineno_Pos _temp49; int
_temp50; int _temp52; struct _tagged_arr _temp54; struct Cyc_Lineno_Pos* _temp47=
pos_s; _temp49=* _temp47; _LL55: _temp54= _temp49.line; goto _LL53; _LL53:
_temp52= _temp49.line_no; goto _LL51; _LL51: _temp50= _temp49.col; goto _LL48;
_LL48: { struct Cyc_Lineno_Pos _temp58; int _temp59; int _temp61; struct
_tagged_arr _temp63; struct Cyc_Lineno_Pos* _temp56= pos_e; _temp58=* _temp56;
_LL64: _temp63= _temp58.line; goto _LL62; _LL62: _temp61= _temp58.line_no; goto
_LL60; _LL60: _temp59= _temp58.col; goto _LL57; _LL57: if( _temp52 ==  _temp61){
int n= Cyc_Position_line_length /  3; struct _tagged_arr sec_one= Cyc_Position_trunc(
n, Cyc_Std_substring(( struct _tagged_arr) _temp54, 0,( unsigned int) _temp50));
struct _tagged_arr sec_two= Cyc_Position_trunc( n, Cyc_Std_substring(( struct
_tagged_arr) _temp54, _temp50,( unsigned int)( _temp59 -  _temp50))); struct
_tagged_arr sec_three= Cyc_Position_trunc( n, Cyc_Std_substring(( struct
_tagged_arr) _temp54, _temp50, Cyc_Std_strlen(( struct _tagged_arr) _temp54) - 
_temp59)); return({ struct _tuple3* _temp65=( struct _tuple3*) _cycalloc(
sizeof( struct _tuple3)); _temp65->f1=({ struct Cyc_Std_String_pa_struct _temp69;
_temp69.tag= Cyc_Std_String_pa; _temp69.f1=( struct _tagged_arr) sec_three;{
struct Cyc_Std_String_pa_struct _temp68; _temp68.tag= Cyc_Std_String_pa; _temp68.f1=(
struct _tagged_arr) sec_two;{ struct Cyc_Std_String_pa_struct _temp67; _temp67.tag=
Cyc_Std_String_pa; _temp67.f1=( struct _tagged_arr) sec_one;{ void* _temp66[ 3u]={&
_temp67,& _temp68,& _temp69}; Cyc_Std_aprintf( _tag_arr("%s%s%s", sizeof(
unsigned char), 7u), _tag_arr( _temp66, sizeof( void*), 3u));}}}}); _temp65->f2=(
int) Cyc_Std_strlen(( struct _tagged_arr) sec_one); _temp65->f3=( int)( Cyc_Std_strlen((
struct _tagged_arr) sec_one) +  Cyc_Std_strlen(( struct _tagged_arr) sec_two));
_temp65;});} else{ int n=( Cyc_Position_line_length -  3) /  4; struct
_tagged_arr sec_one= Cyc_Position_trunc( n, Cyc_Std_substring(( struct
_tagged_arr) _temp54, 0,( unsigned int) _temp50)); struct _tagged_arr sec_two=
Cyc_Position_trunc( n, Cyc_Std_substring(( struct _tagged_arr) _temp54, _temp50,
Cyc_Std_strlen(( struct _tagged_arr) _temp54) -  _temp50)); struct _tagged_arr
sec_three= Cyc_Position_trunc( n, Cyc_Std_substring(( struct _tagged_arr)
_temp63, 0,( unsigned int) _temp59)); struct _tagged_arr sec_four= Cyc_Position_trunc(
n, Cyc_Std_substring(( struct _tagged_arr) _temp63, _temp59, Cyc_Std_strlen((
struct _tagged_arr) _temp63) -  _temp59)); return({ struct _tuple3* _temp70=(
struct _tuple3*) _cycalloc( sizeof( struct _tuple3)); _temp70->f1=({ struct Cyc_Std_String_pa_struct
_temp75; _temp75.tag= Cyc_Std_String_pa; _temp75.f1=( struct _tagged_arr)
sec_four;{ struct Cyc_Std_String_pa_struct _temp74; _temp74.tag= Cyc_Std_String_pa;
_temp74.f1=( struct _tagged_arr) sec_three;{ struct Cyc_Std_String_pa_struct
_temp73; _temp73.tag= Cyc_Std_String_pa; _temp73.f1=( struct _tagged_arr)
sec_two;{ struct Cyc_Std_String_pa_struct _temp72; _temp72.tag= Cyc_Std_String_pa;
_temp72.f1=( struct _tagged_arr) sec_one;{ void* _temp71[ 4u]={& _temp72,&
_temp73,& _temp74,& _temp75}; Cyc_Std_aprintf( _tag_arr("%s%s.\\.%s%s", sizeof(
unsigned char), 12u), _tag_arr( _temp71, sizeof( void*), 4u));}}}}}); _temp70->f2=(
int) Cyc_Std_strlen(( struct _tagged_arr) sec_one); _temp70->f3=( int)((( Cyc_Std_strlen((
struct _tagged_arr) sec_one) +  Cyc_Std_strlen(( struct _tagged_arr) sec_two)) + 
3) +  Cyc_Std_strlen(( struct _tagged_arr) sec_three)); _temp70;});}}}}} static
int Cyc_Position_error_b= 0; int Cyc_Position_error_p(){ return Cyc_Position_error_b;}
unsigned char Cyc_Position_Error[ 10u]="\000\000\000\000Error"; struct Cyc_Position_Error_struct{
unsigned char* tag; struct Cyc_Position_Error* f1; } ; int Cyc_Position_print_context=
0; int Cyc_Position_first_error= 1; void Cyc_Position_post_error( struct Cyc_Position_Error*
e){ Cyc_Position_error_b= 1; Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stdout);
if( Cyc_Position_first_error){({ void* _temp76[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp76, sizeof( void*), 0u));});
Cyc_Position_first_error= 0;}({ struct Cyc_Std_String_pa_struct _temp79; _temp79.tag=
Cyc_Std_String_pa; _temp79.f1=( struct _tagged_arr) e->desc;{ struct Cyc_Std_String_pa_struct
_temp78; _temp78.tag= Cyc_Std_String_pa; _temp78.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
e->seg);{ void* _temp77[ 2u]={& _temp78,& _temp79}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp77, sizeof(
void*), 2u));}}}); if( Cyc_Position_print_context){ struct _handler_cons _temp80;
_push_handler(& _temp80);{ int _temp82= 0; if( setjmp( _temp80.handler)){
_temp82= 1;} if( ! _temp82){{ struct _tuple3* x= Cyc_Position_get_context( e->seg);
struct _tagged_arr marker_str=({ unsigned int _temp86=( unsigned int)((* x).f3 + 
1); unsigned char* _temp87=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp86)); struct _tagged_arr _temp89= _tag_arr( _temp87,
sizeof( unsigned char),( unsigned int)((* x).f3 +  1));{ unsigned int _temp88=
_temp86; unsigned int i; for( i= 0; i <  _temp88; i ++){ _temp87[ i]='\000';}};
_temp89;}); int i= - 1; while( ++ i < (* x).f2) {*(( unsigned char*)
_check_unknown_subscript( marker_str, sizeof( unsigned char), i))=' ';} while(
++ i < (* x).f3) {*(( unsigned char*) _check_unknown_subscript( marker_str,
sizeof( unsigned char), i))='^';}({ struct Cyc_Std_String_pa_struct _temp85;
_temp85.tag= Cyc_Std_String_pa; _temp85.f1=( struct _tagged_arr) marker_str;{
struct Cyc_Std_String_pa_struct _temp84; _temp84.tag= Cyc_Std_String_pa; _temp84.f1=(
struct _tagged_arr)(* x).f1;{ void* _temp83[ 2u]={& _temp84,& _temp85}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("  %s\n  %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp83, sizeof( void*), 2u));}}});}; _pop_handler();} else{ void* _temp81=(
void*) _exn_thrown; void* _temp91= _temp81; _LL93: if( _temp91 ==  Cyc_Position_Nocontext){
goto _LL94;} else{ goto _LL95;} _LL95: goto _LL96; _LL94: goto _LL92; _LL96:(
void) _throw( _temp91); _LL92:;}}} Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);}
void Cyc_Position_reset_position( struct _tagged_arr s){ Cyc_Position_source= s;
Cyc_Position_error_b= 0;} void Cyc_Position_set_position_file( struct
_tagged_arr s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;} struct
_tagged_arr Cyc_Position_get_position_file(){ return Cyc_Position_source;}
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Position_Segment_rep; static
struct Cyc_Typerep_Int_struct Cyc__genrep_0={ 1u, 32}; static struct _tuple0 Cyc__gentuple_1={
offsetof( struct Cyc_Position_Segment,start),( void*)& Cyc__genrep_0}; static
struct _tuple0 Cyc__gentuple_2={ offsetof( struct Cyc_Position_Segment,end),(
void*)& Cyc__genrep_0}; static struct _tuple0* Cyc__genarr_3[ 2u]={& Cyc__gentuple_1,&
Cyc__gentuple_2}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Position_Segment_rep={
4u, sizeof( struct Cyc_Position_Segment),{( void*)(( struct _tuple0**) Cyc__genarr_3),(
void*)(( struct _tuple0**) Cyc__genarr_3),( void*)(( struct _tuple0**) Cyc__genarr_3
+  2u)}}; void* Cyc_segment_rep=( void*)& Cyc_struct_Position_Segment_rep;
