 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; struct _tagged_ptr1{ void** curr; void** base; void**
last_plus_one; } ; extern struct Cyc_List_List* Cyc_List_list( struct
_tagged_ptr1); extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern struct
Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* l);
extern unsigned char Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{ unsigned char*
tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Arg_Error[ 10u];
static const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{ int tag;
void(* f1)(); } ; static const int Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_FlagString_spec=
2; struct Cyc_Arg_FlagString_spec_struct{ int tag; void(* f1)( struct
_tagged_string, struct _tagged_string); } ; static const int Cyc_Arg_Set_spec= 3;
struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec=
4; struct Cyc_Arg_Clear_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_String_spec=
5; struct Cyc_Arg_String_spec_struct{ int tag; void(* f1)( struct _tagged_string);
} ; static const int Cyc_Arg_Int_spec= 6; struct Cyc_Arg_Int_spec_struct{ int
tag; void(* f1)( int); } ; static const int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; extern void Cyc_Arg_parse(
struct Cyc_List_List* specs, void(* anonfun)( struct _tagged_string), struct
_tagged_string errmsg, struct _tagged_ptr0 args); struct Cyc_Stdio___sFILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; extern int Cyc_Stdio_remove( struct _tagged_string); extern
int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush(
struct Cyc_Stdio___sFILE*); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_fopen(
struct _tagged_string _name, struct _tagged_string _type); extern unsigned char
Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct
_tagged_string fname, struct _tagged_string mode); extern void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE*); struct Cyc_Time_tm{ int tm_sec; int tm_min; int
tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int tm_yday; int
tm_isdst; } ; extern unsigned int clock()  __attribute__(( cdecl )) ; extern
double difftime( int _time2, int _time1)  __attribute__(( cdecl )) ; extern int
mktime( struct Cyc_Time_tm* _timeptr)  __attribute__(( cdecl )) ; extern int
time( int* _timer)  __attribute__(( cdecl )) ; extern struct Cyc_Time_tm* gmtime(
const int* _timer)  __attribute__(( cdecl )) ; extern struct Cyc_Time_tm*
localtime( const int* _timer)  __attribute__(( cdecl )) ; extern struct Cyc_Time_tm*
gmtime_r( const int*, struct Cyc_Time_tm*)  __attribute__(( cdecl )) ; extern
struct Cyc_Time_tm* localtime_r( const int*, struct Cyc_Time_tm*)
 __attribute__(( cdecl )) ; extern int _timezone  __attribute__(( dllimport )) ;
extern int _daylight  __attribute__(( dllimport )) ; extern unsigned char*
_tzname[ 2u]  __attribute__(( dllimport )) ; extern void tzset()
 __attribute__(( cdecl )) ; struct Cyc___sFILE; struct Cyc__reent; struct Cyc__glue{
struct Cyc__glue* _next; int _niobs; struct Cyc___sFILE* _iobs; } ; struct Cyc__Bigint{
struct Cyc__Bigint* _next; int _k; int _maxwds; int _sign; int _wds;
unsigned int _x[ 1u]; } ; struct Cyc__atexit{ struct Cyc__atexit* _next; int
_ind; void(* _fns[ 32u])(); } ; struct Cyc___sbuf{ unsigned char* _base; int
_size; } ; struct Cyc___sFILE{ unsigned char* _p; int _r; int _w; short _flags;
short _file; struct Cyc___sbuf _bf; int _lbfsize; void* _cookie; int(* _read)(
void* _cookie, unsigned char* _buf, int _n)  __attribute__(( cdecl )) ; int(*
_write)( void* _cookie, const unsigned char* _buf, int _n)  __attribute__((
cdecl )) ; int(* _seek)( void* _cookie, int _offset, int _whence)
 __attribute__(( cdecl )) ; int(* _close)( void* _cookie)  __attribute__(( cdecl
)) ; struct Cyc___sbuf _ub; unsigned char* _up; int _ur; unsigned char _ubuf[ 3u];
unsigned char _nbuf[ 1u]; struct Cyc___sbuf _lb; int _blksize; int _offset;
struct Cyc__reent* _data; } ; struct Cyc__reent_u1{ unsigned int _unused_rand;
int _strtok_last; unsigned char _asctime_buf[ 26u]; struct Cyc_Time_tm
_localtime_buf; int _gamma_signgam; unsigned long long _rand_next; } ; struct
Cyc__reent_u2{ unsigned int _nextf[ 30u]; unsigned int _nmalloc[ 30u]; } ; union
Cyc__reent_union{ struct Cyc__reent_u1 _reent; struct Cyc__reent_u2 _unused; } ;
struct Cyc__reent{ int _errno; struct Cyc___sFILE* _stdin; struct Cyc___sFILE*
_stdout; struct Cyc___sFILE* _stderr; int _inc; unsigned char _emergency[ 25u];
int _current_category; const unsigned char* _current_locale; int __sdidinit;
void(* __cleanup)( struct Cyc__reent*)  __attribute__(( cdecl )) ; struct Cyc__Bigint*
_result; int _result_k; struct Cyc__Bigint* _p5s; struct Cyc__Bigint** _freelist;
int _cvtlen; unsigned char* _cvtbuf; union Cyc__reent_union _new; struct Cyc__atexit*
_atexit; struct Cyc__atexit _atexit0; void(** _sig_func)( int); struct Cyc__glue
__sglue; struct Cyc___sFILE __sf[ 3u]; } ; extern struct Cyc__reent* _impure_ptr;
extern void _reclaim_reent( struct Cyc__reent*); struct Cyc_Stdlib__Div{ int
quot; int rem; } ; struct Cyc_Stdlib__Ldiv{ int quot; int rem; } ; extern int
__mb_cur_max  __attribute__(( dllimport )) ; extern int abs( int)
 __attribute__(( cdecl )) ; extern int atexit( void(* __func)())
 __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Div div( int __numer, int
__denom)  __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Ldiv ldiv( int
__numer, int __denom)  __attribute__(( cdecl )) ; extern int rand()
 __attribute__(( cdecl )) ; extern void srand( unsigned int __seed)
 __attribute__(( cdecl )) ; extern int rand_r( unsigned int* __seed)
 __attribute__(( cdecl )) ; extern int random()  __attribute__(( cdecl )) ;
extern int srandom( unsigned int __seed)  __attribute__(( cdecl )) ; extern int
grantpt( int)  __attribute__(( cdecl )) ; extern int unlockpt( int)
 __attribute__(( cdecl )) ; extern struct _tagged_string Cyc_Stdlib_getenv(
struct _tagged_string); extern unsigned int Cyc_String_strlen( struct
_tagged_string s); extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_String_str_sepstr(
struct Cyc_List_List*, struct _tagged_string); extern struct _tagged_string Cyc_String_substring(
struct _tagged_string, int ofs, unsigned int n); extern struct _tagged_string
Cyc_String_strchr( struct _tagged_string s, unsigned char c); struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct _tagged_string Cyc_Filename_concat( struct _tagged_string,
struct _tagged_string); extern struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string); struct Cyc_Id_id; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; extern unsigned char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
struct _tagged_string); struct Cyc_Position_Segment; static const int Cyc_Position_Lex=
0; static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; extern int Cyc_Position_error_p(); struct _tuple0{ void* f1; struct
_tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct
Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct
Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_SwitchC_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Static= 0;
static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_string* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
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
static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static const int
Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
8; static const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att=
10; static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
12; static const int Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att=
14; static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int
Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod= 0; struct
Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{ int tag;
void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
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
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
static const int Cyc_Absyn_Fscanf= 24; static const int Cyc_Absyn_Sscanf= 25;
static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
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
int tag; void* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const
int Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
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
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_string
f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_string, int, void*); void* read_fun_state; } ;
struct _tagged_ptr2{ int* curr; int* base; int* last_plus_one; } ; struct Cyc_Lexing_lex_tables{
struct _tagged_ptr2 lex_base; struct _tagged_ptr2 lex_backtrk; struct
_tagged_ptr2 lex_default; struct _tagged_ptr2 lex_trans; struct _tagged_ptr2
lex_check; } ; extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE*
f); struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_BlockItem_tok[ 18u]; struct Cyc_BlockItem_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Bool_tok[ 13u];
struct Cyc_Bool_tok_struct{ unsigned char* tag; int f1; } ; extern unsigned char
Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{ unsigned char* tag;
unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclSpec_tok[
17u]; struct Cyc_DeclSpec_tok_struct{ unsigned char* tag; struct Cyc_Declaration_spec*
f1; } ; extern unsigned char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple3{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple3* f1;
} ; extern unsigned char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple4{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple4* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple5{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple5* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple6{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple6* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple7{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple7* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
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
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple8{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple8* f1; } ; extern unsigned char Cyc_Rgn_tok[ 12u];
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Rgnorder_tok[ 17u]; struct Cyc_Rgnorder_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Scope_tok[
14u]; struct Cyc_Scope_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ unsigned char*
tag; short f1; } ; extern unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[
21u]; struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
extern unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Stringopt_tok[
18u]; struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt*
f1; } ; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
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
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct
Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids:
1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params* fs); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); struct Cyc_Set_Set;
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
Cyc_List_List* _s); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
_s); extern struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* decls);
struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct
Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF= 0;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; struct Cyc_NewControlFlow_AnalEnv{ struct
Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* _s); struct Cyc_Interface_I; extern struct Cyc_Interface_I*
Cyc_Interface_empty(); extern struct Cyc_Interface_I* Cyc_Interface_final();
extern struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae);
struct _tuple9{ struct _tagged_string f1; struct _tagged_string f2; } ; extern
int Cyc_Interface_is_subinterface( struct Cyc_Interface_I* i1, struct Cyc_Interface_I*
i2, struct _tuple9* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); extern void Cyc_Interface_save(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_load( struct Cyc_Stdio___sFILE*); extern void Cyc_Lex_lex_init();
static int Cyc_pp_r= 0; static int Cyc_noexpand_r= 0; static int Cyc_noshake_r=
0; static int Cyc_stop_after_cpp_r= 0; static int Cyc_parseonly_r= 0; static int
Cyc_tc_r= 0; static int Cyc_ic_r= 0; static int Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r=
0; static int Cyc_stop_after_asmfile_r= 0; static int Cyc_tovc_r= 0; static int
Cyc_v_r= 0; static int Cyc_save_temps_r= 0; static int Cyc_save_c_r= 0; static
int Cyc_add_cyc_namespace_r= 1; static int Cyc_print_full_evars_r= 0; static int
Cyc_print_all_tvars_r= 0; static int Cyc_print_all_kinds_r= 0; static struct
_tagged_string* Cyc_output_file= 0; static void Cyc_set_output_file( struct
_tagged_string s){ Cyc_output_file=({ struct _tagged_string* _temp0=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp0[ 0]= s;
_temp0;});} static struct Cyc_List_List* Cyc_cppargs= 0; static void Cyc_add_cpparg(
struct _tagged_string s){ Cyc_cppargs=({ struct Cyc_List_List* _temp1=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)({
struct _tagged_string* _temp2=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp2[ 0]= s; _temp2;}); _temp1->tl= Cyc_cppargs;
_temp1;});} static int Cyc_is_cyclone_sourcefile( struct _tagged_string s){
unsigned int _temp3= Cyc_String_strlen( s); goto _LL4; _LL4: if( _temp3 <= 4){
return 0;} else{ return Cyc_String_strcmp(({ struct _tagged_string _temp5= s;
_temp5.curr +=( int)( _temp3 - 4); _temp5;}),({ unsigned char* _temp6=(
unsigned char*)".cyc"; struct _tagged_string _temp7; _temp7.curr= _temp6; _temp7.base=
_temp6; _temp7.last_plus_one= _temp6 + 5; _temp7;})) == 0;}} static struct Cyc_List_List*
Cyc_cyclone_exec_prefix_path= 0; static void Cyc_add_cyclone_exec_prefix_path(
struct _tagged_string s){ unsigned int _temp8= Cyc_String_strlen( s); goto _LL9;
_LL9: if( _temp8 <= 2){ return;}{ struct _tagged_string _temp10=( struct
_tagged_string) Cyc_String_substring( s, 2, _temp8 - 2); goto _LL11; _LL11: Cyc_cyclone_exec_prefix_path=({
struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp12->hd=( void*)({ struct _tagged_string* _temp13=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp13[ 0]=
_temp10; _temp13;}); _temp12->tl= Cyc_cyclone_exec_prefix_path; _temp12;});}}
static struct Cyc_List_List* Cyc_ccargs= 0; static void Cyc_add_ccarg( struct
_tagged_string s){ Cyc_ccargs=({ struct Cyc_List_List* _temp14=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp14->hd=( void*)({ struct
_tagged_string* _temp15=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp15[ 0]= s; _temp15;}); _temp14->tl= Cyc_ccargs; _temp14;});}
static void Cyc_add_ccarg2( struct _tagged_string flag, struct _tagged_string
arg){ Cyc_ccargs=({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp16->hd=( void*)({ struct
_tagged_string* _temp19=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp19[ 0]= arg; _temp19;}); _temp16->tl=({ struct Cyc_List_List*
_temp17=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp17->hd=( void*)({ struct _tagged_string* _temp18=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp18[ 0]= flag; _temp18;});
_temp17->tl= Cyc_ccargs; _temp17;}); _temp16;});} static struct Cyc_List_List*
Cyc_libargs= 0; static void Cyc_add_libarg( struct _tagged_string s){ Cyc_libargs=({
struct Cyc_List_List* _temp20=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp20->hd=( void*)({ struct _tagged_string* _temp21=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp21[ 0]= s;
_temp21;}); _temp20->tl= Cyc_libargs; _temp20;});} static void Cyc_set_save_temps(){
Cyc_save_temps_r= 1; Cyc_add_ccarg(({ unsigned char* _temp22=( unsigned char*)"-save-temps";
struct _tagged_string _temp23; _temp23.curr= _temp22; _temp23.base= _temp22;
_temp23.last_plus_one= _temp22 + 12; _temp23;}));} static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r= 1; Cyc_add_cpparg(({ unsigned char* _temp24=(
unsigned char*)"-M"; struct _tagged_string _temp25; _temp25.curr= _temp24;
_temp25.base= _temp24; _temp25.last_plus_one= _temp24 + 3; _temp25;}));} static
void Cyc_set_stop_after_objectfile(){ Cyc_stop_after_objectfile_r= 1; Cyc_add_ccarg(({
unsigned char* _temp26=( unsigned char*)"-c"; struct _tagged_string _temp27;
_temp27.curr= _temp26; _temp27.base= _temp26; _temp27.last_plus_one= _temp26 + 3;
_temp27;}));} static void Cyc_set_stop_after_asmfile(){ Cyc_stop_after_asmfile_r=
1; Cyc_add_ccarg(({ unsigned char* _temp28=( unsigned char*)"-S"; struct
_tagged_string _temp29; _temp29.curr= _temp28; _temp29.base= _temp28; _temp29.last_plus_one=
_temp28 + 3; _temp29;}));} static struct Cyc_List_List* Cyc_cyclone_files= 0;
static void Cyc_add_other( struct _tagged_string s){ if( Cyc_is_cyclone_sourcefile(
s)){ Cyc_cyclone_files=({ struct Cyc_List_List* _temp30=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp30->hd=( void*)({ struct
_tagged_string* _temp31=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp31[ 0]= s; _temp31;}); _temp30->tl= Cyc_cyclone_files;
_temp30;});{ struct _tagged_string _temp32= Cyc_Filename_chop_extension( s);
goto _LL33; _LL33: { struct _tagged_string _temp36= Cyc_String_strconcat((
struct _tagged_string) _temp32,({ unsigned char* _temp34=( unsigned char*)".c";
struct _tagged_string _temp35; _temp35.curr= _temp34; _temp35.base= _temp34;
_temp35.last_plus_one= _temp34 + 3; _temp35;})); goto _LL37; _LL37: Cyc_add_ccarg((
struct _tagged_string) _temp36);}}} else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file(
struct _tagged_string s){ if( Cyc_save_temps_r){ return;} else{ Cyc_Stdio_remove(
s);}} int Cyc_compile_failure= 0; struct Cyc_Stdio___sFILE* Cyc_try_file_open(
struct _tagged_string filename, struct _tagged_string mode, struct
_tagged_string msg_part){ struct _handler_cons _temp38; _push_handler(& _temp38);{
int _temp40= 0; if( setjmp( _temp38.handler)){ _temp40= 1;} if( ! _temp40){{
struct Cyc_Stdio___sFILE* _temp41=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open(
filename, mode); _npop_handler( 0u); return _temp41;}; _pop_handler();} else{
void* _temp39=( void*) _exn_thrown; void* _temp43= _temp39; _LL45: goto _LL46;
_LL47: goto _LL48; _LL46: Cyc_compile_failure= 1;({ struct _tagged_string
_temp49= msg_part; struct _tagged_string _temp50= filename; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\nError: couldn't open %.*s %.*s\n", _temp49.last_plus_one
- _temp49.curr, _temp49.curr, _temp50.last_plus_one - _temp50.curr, _temp50.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); return 0; _LL48:(
void) _throw( _temp43); _LL44:;}}} struct Cyc_List_List* Cyc_do_stage( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( void*, struct Cyc_List_List*),
void* env, struct Cyc_List_List* tds){ struct _tagged_string exn_string=({
unsigned char* _temp93=( unsigned char*)""; struct _tagged_string _temp94;
_temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one= _temp93 + 1;
_temp94;}); struct _tagged_string explain_string=({ unsigned char* _temp91=(
unsigned char*)""; struct _tagged_string _temp92; _temp92.curr= _temp91; _temp92.base=
_temp91; _temp92.last_plus_one= _temp91 + 1; _temp92;}); int other_exn= 0; void*
ex=( void*)({ struct Cyc_Core_Impossible_struct* _temp87=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp87[ 0]=({ struct
Cyc_Core_Impossible_struct _temp88; _temp88.tag= Cyc_Core_Impossible; _temp88.f1=({
unsigned char* _temp89=( unsigned char*)""; struct _tagged_string _temp90;
_temp90.curr= _temp89; _temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 1;
_temp90;}); _temp88;}); _temp87;}); struct Cyc_List_List* _temp51= 0; goto _LL52;
_LL52:{ struct _handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0;
if( setjmp( _temp53.handler)){ _temp55= 1;} if( ! _temp55){ _temp51= f( env, tds);;
_pop_handler();} else{ void* _temp54=( void*) _exn_thrown; void* _temp57=
_temp54; struct _tagged_string _temp69; struct _tagged_string _temp71; _LL59:
if(*(( void**) _temp57) == Cyc_Core_Impossible){ _LL70: _temp69=(( struct Cyc_Core_Impossible_struct*)
_temp57)->f1; goto _LL60;} else{ goto _LL61;} _LL61: if( _temp57 == Cyc_Dict_Absent){
goto _LL62;} else{ goto _LL63;} _LL63: if(*(( void**) _temp57) == Cyc_Core_InvalidArg){
_LL72: _temp71=(( struct Cyc_Core_InvalidArg_struct*) _temp57)->f1; goto _LL64;}
else{ goto _LL65;} _LL65: goto _LL66; _LL67: goto _LL68; _LL60: exn_string=({
unsigned char* _temp73=( unsigned char*)"Exception Core::Impossible"; struct
_tagged_string _temp74; _temp74.curr= _temp73; _temp74.base= _temp73; _temp74.last_plus_one=
_temp73 + 27; _temp74;}); explain_string= _temp69; goto _LL58; _LL62: exn_string=({
unsigned char* _temp75=( unsigned char*)"Exception Dict::Absent"; struct
_tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75; _temp76.last_plus_one=
_temp75 + 23; _temp76;}); goto _LL58; _LL64: exn_string=({ unsigned char*
_temp77=( unsigned char*)"Exception Core::InvalidArg"; struct _tagged_string
_temp78; _temp78.curr= _temp77; _temp78.base= _temp77; _temp78.last_plus_one=
_temp77 + 27; _temp78;}); explain_string= _temp71; goto _LL58; _LL66: ex=
_temp57; other_exn= 1; exn_string=({ unsigned char* _temp79=( unsigned char*)"Uncaught exception";
struct _tagged_string _temp80; _temp80.curr= _temp79; _temp80.base= _temp79;
_temp80.last_plus_one= _temp79 + 19; _temp80;}); goto _LL58; _LL68:( void)
_throw( _temp57); _LL58:;}}} if( Cyc_Position_error_p()){ Cyc_compile_failure= 1;}
if( Cyc_String_strcmp( exn_string,({ unsigned char* _temp81=( unsigned char*)"";
struct _tagged_string _temp82; _temp82.curr= _temp81; _temp82.base= _temp81;
_temp82.last_plus_one= _temp81 + 1; _temp82;})) != 0){ Cyc_compile_failure= 1;({
struct _tagged_string _temp83= exn_string; struct _tagged_string _temp84=
stage_name; struct _tagged_string _temp85= explain_string; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\n%.*s thrown during %.*s: %.*s", _temp83.last_plus_one
- _temp83.curr, _temp83.curr, _temp84.last_plus_one - _temp84.curr, _temp84.curr,
_temp85.last_plus_one - _temp85.curr, _temp85.curr);});} if( Cyc_compile_failure){
fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nCOMPILATION FAILED!\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); if( other_exn){( void) _throw( ex);}
return _temp51;} else{ if( Cyc_v_r){({ struct _tagged_string _temp86= stage_name;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s completed.\n", _temp86.last_plus_one
- _temp86.curr, _temp86.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return _temp51;}} return _temp51;} struct Cyc_List_List* Cyc_do_parse(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp95= Cyc_Parse_parse_file( f); goto _LL96; _LL96: Cyc_Lex_lex_init();
return _temp95;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds); if( ! Cyc_noshake_r){
tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck(
int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds);
return tds;} struct _tuple10{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Stdio___sFILE*
f2; struct Cyc_Stdio___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple10* params, struct Cyc_List_List* tds){ struct _tuple10 _temp99;
struct Cyc_Stdio___sFILE* _temp100; struct Cyc_Stdio___sFILE* _temp102; struct
Cyc_Tcenv_Tenv* _temp104; struct _tuple10* _temp97= params; _temp99=* _temp97;
_LL105: _temp104= _temp99.f1; goto _LL103; _LL103: _temp102= _temp99.f2; goto
_LL101; _LL101: _temp100= _temp99.f3; goto _LL98; _LL98: { struct Cyc_Interface_I*
_temp106= Cyc_Interface_extract( _temp104->ae); goto _LL107; _LL107: if(
_temp102 == 0){ Cyc_Interface_save( _temp106, _temp100);} else{ struct Cyc_Interface_I*
_temp108= Cyc_Interface_parse(( struct Cyc_Stdio___sFILE*) _check_null( _temp102));
goto _LL109; _LL109: if( ! Cyc_Interface_is_subinterface( _temp108, _temp106,({
struct _tuple9* _temp110=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp110->f1=({ unsigned char* _temp113=( unsigned char*)"written interface";
struct _tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 18; _temp114;}); _temp110->f2=({
unsigned char* _temp111=( unsigned char*)"maximal interface"; struct
_tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 18; _temp112;}); _temp110;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp108, _temp100);}} return tds;}} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_tovc( int ignore, struct Cyc_List_List* tds){
return Cyc_Tovc_tovc( tds);} struct Cyc_List_List* Cyc_do_print( struct Cyc_Stdio___sFILE*
out_file, struct Cyc_List_List* tds){ struct Cyc_Absynpp_Params params_r; if(
Cyc_tc_r){ params_r= Cyc_Absynpp_cyc_params_r;} else{ params_r= Cyc_Absynpp_c_params_r;}
params_r.expand_typedefs= ! Cyc_noexpand_r; params_r.to_VC= Cyc_tovc_r; params_r.add_cyc_prefix=
Cyc_add_cyc_namespace_r; params_r.print_full_evars= Cyc_print_full_evars_r;
params_r.print_all_tvars= Cyc_print_all_tvars_r; params_r.print_all_kinds= Cyc_print_all_kinds_r;
if( Cyc_pp_r){ Cyc_Absynpp_set_params(& params_r); Cyc_Absynpp_decllist2file(
tds, out_file);} else{ Cyc_Absyndump_set_params(& params_r); Cyc_Absyndump_dumpdecllist2file(
tds, out_file);} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) out_file); return
tds;} static struct Cyc_List_List* Cyc_cfiles= 0; static void Cyc_remove_cfiles(){
if( ! Cyc_save_c_r){ for( 0; Cyc_cfiles != 0; Cyc_cfiles=(( struct Cyc_List_List*)
_check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}} static struct Cyc_List_List*
Cyc_split_by_char( struct _tagged_string s, unsigned char c){ if( s.curr ==((
struct _tagged_string)({ struct _tagged_string _temp115={ 0, 0, 0}; _temp115;})).curr){
return 0;}{ struct Cyc_List_List* _temp116= 0; goto _LL117; _LL117: {
unsigned int _temp118= Cyc_String_strlen( s); goto _LL119; _LL119: while(
_temp118 > 0) { struct _tagged_string _temp120= Cyc_String_strchr( s, c); goto
_LL121; _LL121: if( _temp120.curr ==(( struct _tagged_string)({ struct
_tagged_string _temp122={ 0, 0, 0}; _temp122;})).curr){ _temp116=({ struct Cyc_List_List*
_temp123=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp123->hd=( void*)({ struct _tagged_string* _temp124=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp124[ 0]= s; _temp124;});
_temp123->tl= _temp116; _temp123;}); break;} else{ _temp116=({ struct Cyc_List_List*
_temp125=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp125->hd=( void*)({ struct _tagged_string* _temp126=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp126[ 0]=( struct _tagged_string)
Cyc_String_substring( s, 0,( unsigned int)((( struct _tagged_string) _temp120).curr
- s.curr)); _temp126;}); _temp125->tl= _temp116; _temp125;}); _temp118 -=((
struct _tagged_string) _temp120).curr - s.curr; s=({ struct _tagged_string
_temp127= _temp120; _temp127.curr += 1; _temp127;});}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp116);}}} int Cyc_file_exists(
struct _tagged_string file){ struct Cyc_Stdio___sFILE* f= 0;{ struct
_handler_cons _temp128; _push_handler(& _temp128);{ int _temp130= 0; if( setjmp(
_temp128.handler)){ _temp130= 1;} if( ! _temp130){ f=( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_file_open( file,({ unsigned char* _temp131=( unsigned char*)"r";
struct _tagged_string _temp132; _temp132.curr= _temp131; _temp132.base= _temp131;
_temp132.last_plus_one= _temp131 + 2; _temp132;}));; _pop_handler();} else{ void*
_temp129=( void*) _exn_thrown; void* _temp134= _temp129; _LL136: goto _LL137;
_LL138: goto _LL139; _LL137: goto _LL135; _LL139:( void) _throw( _temp134);
_LL135:;}}} if( f == 0){ return 0;} else{ Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f)); return 1;}} struct _tagged_string* Cyc_find( struct Cyc_List_List*
dirs, struct _tagged_string file){ if( file.curr ==(( struct _tagged_string)({
struct _tagged_string _temp140={ 0, 0, 0}; _temp140;})).curr){ return 0;} for( 0;
dirs != 0; dirs=(( struct Cyc_List_List*) _check_null( dirs))->tl){ struct
_tagged_string _temp141=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( dirs))->hd); goto _LL142; _LL142: if( _temp141.curr ==(( struct
_tagged_string)({ struct _tagged_string _temp143={ 0, 0, 0}; _temp143;})).curr?
1: Cyc_String_strlen( _temp141) == 0){ continue;}{ struct _tagged_string s=(
struct _tagged_string) Cyc_Filename_concat( _temp141, file); if( Cyc_file_exists(
s)){ return({ struct _tagged_string* _temp144=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp144[ 0]= s; _temp144;});}}}
return 0;} struct _tagged_string Cyc_do_find( struct Cyc_List_List* dirs, struct
_tagged_string file){ struct _tagged_string* _temp145= Cyc_find( dirs, file);
goto _LL146; _LL146: if( _temp145 == 0){({ struct _tagged_string _temp147= file;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file %.*s",
_temp147.last_plus_one - _temp147.curr, _temp147.curr);}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); exit( 1);} return*(( struct _tagged_string*) _check_null(
_temp145));} void Cyc_process_file( struct _tagged_string filename){ struct
_tagged_string _temp148= Cyc_Filename_chop_extension( filename); goto _LL149;
_LL149: { struct _tagged_string _temp152= Cyc_String_strconcat(( struct
_tagged_string) _temp148,({ unsigned char* _temp150=( unsigned char*)".cyp";
struct _tagged_string _temp151; _temp151.curr= _temp150; _temp151.base= _temp150;
_temp151.last_plus_one= _temp150 + 5; _temp151;})); goto _LL153; _LL153: {
struct _tagged_string _temp156= Cyc_String_strconcat(( struct _tagged_string)
_temp148,({ unsigned char* _temp154=( unsigned char*)".cyci"; struct
_tagged_string _temp155; _temp155.curr= _temp154; _temp155.base= _temp154;
_temp155.last_plus_one= _temp154 + 6; _temp155;})); goto _LL157; _LL157: {
struct _tagged_string _temp160= Cyc_String_strconcat(( struct _tagged_string)
_temp148,({ unsigned char* _temp158=( unsigned char*)".cycio"; struct
_tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 7; _temp159;})); goto _LL161; _LL161: {
struct _tagged_string _temp164= Cyc_String_strconcat(( struct _tagged_string)
_temp148,({ unsigned char* _temp162=( unsigned char*)".c"; struct _tagged_string
_temp163; _temp163.curr= _temp162; _temp163.base= _temp162; _temp163.last_plus_one=
_temp162 + 3; _temp163;})); goto _LL165; _LL165: if( Cyc_v_r){({ struct
_tagged_string _temp166= filename; fprintf( sfile_to_file( Cyc_Stdio_stderr),"Compiling %.*s\n",
_temp166.last_plus_one - _temp166.curr, _temp166.curr);});}{ struct Cyc_Stdio___sFILE*
f0= Cyc_try_file_open( filename,({ unsigned char* _temp268=( unsigned char*)"r";
struct _tagged_string _temp269; _temp269.curr= _temp268; _temp269.base= _temp268;
_temp269.last_plus_one= _temp268 + 2; _temp269;}),({ unsigned char* _temp270=(
unsigned char*)"input file"; struct _tagged_string _temp271; _temp271.curr=
_temp270; _temp271.base= _temp270; _temp271.last_plus_one= _temp270 + 11;
_temp271;})); if( Cyc_compile_failure){ return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f0));{ struct _tagged_string _temp173= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp167=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp167->hd=( void*)({ struct _tagged_string* _temp168=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp168[ 0]=({
unsigned char* _temp169=( unsigned char*)""; struct _tagged_string _temp170;
_temp170.curr= _temp169; _temp170.base= _temp169; _temp170.last_plus_one=
_temp169 + 1; _temp170;}); _temp168;}); _temp167->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs); _temp167;}),({
unsigned char* _temp171=( unsigned char*)" "; struct _tagged_string _temp172;
_temp172.curr= _temp171; _temp172.base= _temp171; _temp172.last_plus_one=
_temp171 + 2; _temp172;})); goto _LL174; _LL174: { struct _tagged_string
stdinc_string=({ unsigned char* _temp266=( unsigned char*)" -Ic:/cyclone/include";
struct _tagged_string _temp267; _temp267.curr= _temp266; _temp267.base= _temp266;
_temp267.last_plus_one= _temp266 + 22; _temp267;}); struct _tagged_string
_temp177= Cyc_Stdlib_getenv(({ unsigned char* _temp175=( unsigned char*)"CYCLONE_INCLUDE_PATH";
struct _tagged_string _temp176; _temp176.curr= _temp175; _temp176.base= _temp175;
_temp176.last_plus_one= _temp175 + 21; _temp176;})); goto _LL178; _LL178: if(
_temp177.curr !=({ struct _tagged_string _temp179={ 0, 0, 0}; _temp179;}).curr){
struct Cyc_List_List* _temp180= Cyc_split_by_char(( struct _tagged_string)
_temp177,':'); goto _LL181; _LL181: { struct _tagged_string _temp188= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp182=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp182->hd=( void*)({ struct _tagged_string* _temp183=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp183[ 0]=({
unsigned char* _temp184=( unsigned char*)""; struct _tagged_string _temp185;
_temp185.curr= _temp184; _temp185.base= _temp184; _temp185.last_plus_one=
_temp184 + 1; _temp185;}); _temp183;}); _temp182->tl= _temp180; _temp182;}),({
unsigned char* _temp186=( unsigned char*)" -I"; struct _tagged_string _temp187;
_temp187.curr= _temp186; _temp187.base= _temp186; _temp187.last_plus_one=
_temp186 + 4; _temp187;})); goto _LL189; _LL189: stdinc_string=( struct
_tagged_string)({ struct _tagged_string _temp190=( struct _tagged_string)
_temp188; struct _tagged_string _temp191= stdinc_string; xprintf("%.*s%.*s",
_temp190.last_plus_one - _temp190.curr, _temp190.curr, _temp191.last_plus_one -
_temp191.curr, _temp191.curr);});}}{ struct _tagged_string ofile_string; if( Cyc_stop_after_cpp_r){
if( Cyc_output_file != 0){ ofile_string=( struct _tagged_string)({ struct
_tagged_string _temp192=*(( struct _tagged_string*) _check_null( Cyc_output_file));
xprintf(" -o %.*s", _temp192.last_plus_one - _temp192.curr, _temp192.curr);});}
else{ ofile_string=({ unsigned char* _temp193=( unsigned char*)""; struct
_tagged_string _temp194; _temp194.curr= _temp193; _temp194.base= _temp193;
_temp194.last_plus_one= _temp193 + 1; _temp194;});}} else{ ofile_string=( struct
_tagged_string)({ struct _tagged_string _temp195=( struct _tagged_string)
_temp152; xprintf(" -o %.*s", _temp195.last_plus_one - _temp195.curr, _temp195.curr);});}{
struct _tagged_string _temp200=({ struct _tagged_string _temp196=( struct
_tagged_string) _temp173; struct _tagged_string _temp197= stdinc_string; struct
_tagged_string _temp198= ofile_string; struct _tagged_string _temp199= filename;
xprintf("gcc -x c -E -U__GNUC__ -nostdinc%.*s%.*s%.*s %.*s", _temp196.last_plus_one
- _temp196.curr, _temp196.curr, _temp197.last_plus_one - _temp197.curr, _temp197.curr,
_temp198.last_plus_one - _temp198.curr, _temp198.curr, _temp199.last_plus_one -
_temp199.curr, _temp199.curr);}); goto _LL201; _LL201: if( Cyc_v_r){({ struct
_tagged_string _temp202=( struct _tagged_string) _temp200; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp202.last_plus_one - _temp202.curr,
_temp202.curr);});} if( system( string_to_Cstring(( struct _tagged_string)
_temp200)) != 0){ Cyc_compile_failure= 1; fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nError: preprocessing\n");
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_string) _temp152);{ struct Cyc_Stdio___sFILE* in_file= Cyc_try_file_open((
struct _tagged_string) _temp152,({ unsigned char* _temp262=( unsigned char*)"r";
struct _tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 2; _temp263;}),({ unsigned char* _temp264=(
unsigned char*)"file"; struct _tagged_string _temp265; _temp265.curr= _temp264;
_temp265.base= _temp264; _temp265.last_plus_one= _temp264 + 5; _temp265;})); if(
Cyc_compile_failure){ return;}{ struct Cyc_List_List* tds= 0;{ struct
_handler_cons _temp203; _push_handler(& _temp203);{ int _temp205= 0; if( setjmp(
_temp203.handler)){ _temp205= 1;} if( ! _temp205){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp206=( unsigned char*)"parsing"; struct
_tagged_string _temp207; _temp207.curr= _temp206; _temp207.base= _temp206;
_temp207.last_plus_one= _temp206 + 8; _temp207;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)
_check_null( in_file), tds);; _pop_handler();} else{ void* _temp204=( void*)
_exn_thrown; void* _temp209= _temp204; _LL211: goto _LL212; _LL213: goto _LL214;
_LL212: Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*) _check_null( in_file));
Cyc_remove_file(( struct _tagged_string) _temp152);( void) _throw( _temp209);
_LL214:( void) _throw( _temp209); _LL210:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( in_file)); if( Cyc_compile_failure){ Cyc_remove_file(( struct
_tagged_string) _temp152); return;}{ struct Cyc_Tcenv_Tenv* _temp215= Cyc_Tcenv_tc_init();
goto _LL216; _LL216: if( Cyc_parseonly_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp217=( unsigned char*)"type checking";
struct _tagged_string _temp218; _temp218.curr= _temp217; _temp218.base= _temp217;
_temp218.last_plus_one= _temp217 + 14; _temp218;}), Cyc_do_typecheck, _temp215,
tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_string)
_temp152); return;} tds=(( struct Cyc_List_List*(*)( struct _tagged_string
stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env,
struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp219=(
unsigned char*)"control-flow checking"; struct _tagged_string _temp220; _temp220.curr=
_temp219; _temp220.base= _temp219; _temp220.last_plus_one= _temp219 + 22;
_temp220;}), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();}
Cyc_remove_file(( struct _tagged_string) _temp152); if( Cyc_compile_failure){
return;} if( Cyc_ic_r){ struct Cyc_Stdio___sFILE* inter_file= Cyc_Stdio_fopen((
struct _tagged_string) _temp156,({ unsigned char* _temp228=( unsigned char*)"r";
struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 2; _temp229;})); struct Cyc_Stdio___sFILE*
inter_objfile= Cyc_try_file_open(( struct _tagged_string) _temp160,({
unsigned char* _temp224=( unsigned char*)"w"; struct _tagged_string _temp225;
_temp225.curr= _temp224; _temp225.base= _temp224; _temp225.last_plus_one=
_temp224 + 2; _temp225;}),({ unsigned char* _temp226=( unsigned char*)"interface object file";
struct _tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 22; _temp227;})); if( inter_objfile == 0){
Cyc_compile_failure= 1; return;} Cyc_Position_reset_position(( struct
_tagged_string) _temp156); tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct _tuple10*, struct
Cyc_List_List*), struct _tuple10* env, struct Cyc_List_List* tds)) Cyc_do_stage)(({
unsigned char* _temp221=( unsigned char*)"interface checking"; struct
_tagged_string _temp222; _temp222.curr= _temp221; _temp222.base= _temp221;
_temp222.last_plus_one= _temp221 + 19; _temp222;}), Cyc_do_interface,({ struct
_tuple10* _temp223=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp223->f1= _temp215; _temp223->f2= inter_file; _temp223->f3=( struct Cyc_Stdio___sFILE*)
_check_null( inter_objfile); _temp223;}), tds); if( inter_file != 0){ Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_file));} Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto
PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds)) Cyc_do_stage)(({ unsigned char* _temp230=( unsigned char*)"translation to C";
struct _tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 17; _temp231;}), Cyc_do_translate, 1, tds);
if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp232=(
unsigned char*)"post-pass to VC"; struct _tagged_string _temp233; _temp233.curr=
_temp232; _temp233.base= _temp232; _temp233.last_plus_one= _temp232 + 16;
_temp233;}), Cyc_do_tovc, 1, tds);} if( Cyc_compile_failure){ return;} PRINTC:
if( tds != 0){ struct Cyc_Stdio___sFILE* out_file; if( Cyc_parseonly_r? 1: Cyc_tc_r){
if( Cyc_output_file != 0){ out_file= Cyc_try_file_open(*(( struct _tagged_string*)
_check_null( Cyc_output_file)),({ unsigned char* _temp234=( unsigned char*)"w";
struct _tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 2; _temp235;}),({ unsigned char* _temp236=(
unsigned char*)"output file"; struct _tagged_string _temp237; _temp237.curr=
_temp236; _temp237.base= _temp236; _temp237.last_plus_one= _temp236 + 12;
_temp237;}));} else{ out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;}}
else{ if( Cyc_toc_r? Cyc_output_file != 0: 0){ out_file= Cyc_try_file_open(*((
struct _tagged_string*) _check_null( Cyc_output_file)),({ unsigned char*
_temp238=( unsigned char*)"w"; struct _tagged_string _temp239; _temp239.curr=
_temp238; _temp239.base= _temp238; _temp239.last_plus_one= _temp238 + 2;
_temp239;}),({ unsigned char* _temp240=( unsigned char*)"output file"; struct
_tagged_string _temp241; _temp241.curr= _temp240; _temp241.base= _temp240;
_temp241.last_plus_one= _temp240 + 12; _temp241;}));} else{ out_file= Cyc_try_file_open((
struct _tagged_string) _temp164,({ unsigned char* _temp242=( unsigned char*)"w";
struct _tagged_string _temp243; _temp243.curr= _temp242; _temp243.base= _temp242;
_temp243.last_plus_one= _temp242 + 2; _temp243;}),({ unsigned char* _temp244=(
unsigned char*)"output file"; struct _tagged_string _temp245; _temp245.curr=
_temp244; _temp245.base= _temp244; _temp245.last_plus_one= _temp244 + 12;
_temp245;}));}} if( Cyc_compile_failure){ return;}{ struct _handler_cons
_temp246; _push_handler(& _temp246);{ int _temp248= 0; if( setjmp( _temp246.handler)){
_temp248= 1;} if( ! _temp248){ tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp249=( unsigned char*)"printing"; struct
_tagged_string _temp250; _temp250.curr= _temp249; _temp250.base= _temp249;
_temp250.last_plus_one= _temp249 + 9; _temp250;}), Cyc_do_print,( struct Cyc_Stdio___sFILE*)
_check_null( out_file), tds);; _pop_handler();} else{ void* _temp247=( void*)
_exn_thrown; void* _temp252= _temp247; _LL254: goto _LL255; _LL256: goto _LL257;
_LL255: Cyc_compile_failure= 1; Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp258=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp258->hd=( void*)({
struct _tagged_string* _temp259=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp259[ 0]=( struct _tagged_string) _temp164;
_temp259;}); _temp258->tl= Cyc_cfiles; _temp258;});( void) _throw( _temp252);
_LL257:( void) _throw( _temp252); _LL253:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp260=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp260->hd=( void*)({
struct _tagged_string* _temp261=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp261[ 0]=( struct _tagged_string) _temp164;
_temp261;}); _temp260->tl= Cyc_cfiles; _temp260;});}}}}}}}}}}}}}} static struct
Cyc_Interface_I* Cyc_read_cycio( struct _tagged_string* n){ struct
_tagged_string basename;{ struct _handler_cons _temp272; _push_handler(&
_temp272);{ int _temp274= 0; if( setjmp( _temp272.handler)){ _temp274= 1;} if( !
_temp274){ basename=( struct _tagged_string) Cyc_Filename_chop_extension(* n);;
_pop_handler();} else{ void* _temp273=( void*) _exn_thrown; void* _temp276=
_temp273; struct _tagged_string _temp282; _LL278: if(*(( void**) _temp276) ==
Cyc_Core_InvalidArg){ _LL283: _temp282=(( struct Cyc_Core_InvalidArg_struct*)
_temp276)->f1; goto _LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279:
basename=* n; goto _LL277; _LL281:( void) _throw( _temp276); _LL277:;}}}{ struct
_tagged_string _temp286= Cyc_String_strconcat( basename,({ unsigned char*
_temp284=( unsigned char*)".cycio"; struct _tagged_string _temp285; _temp285.curr=
_temp284; _temp285.base= _temp284; _temp285.last_plus_one= _temp284 + 7;
_temp285;})); goto _LL287; _LL287: { struct Cyc_Stdio___sFILE* _temp292= Cyc_try_file_open((
struct _tagged_string) _temp286,({ unsigned char* _temp288=( unsigned char*)"r";
struct _tagged_string _temp289; _temp289.curr= _temp288; _temp289.base= _temp288;
_temp289.last_plus_one= _temp288 + 2; _temp289;}),({ unsigned char* _temp290=(
unsigned char*)"interface object file"; struct _tagged_string _temp291; _temp291.curr=
_temp290; _temp291.base= _temp290; _temp291.last_plus_one= _temp290 + 22;
_temp291;})); goto _LL293; _LL293: if( _temp292 == 0){ Cyc_compile_failure= 1;
Cyc_remove_cfiles(); exit( 1);} Cyc_Position_reset_position(( struct
_tagged_string) _temp286);{ struct Cyc_Interface_I* _temp294= Cyc_Interface_load((
struct Cyc_Stdio___sFILE*) _check_null( _temp292)); goto _LL295; _LL295: Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( _temp292)); return _temp294;}}}} static
int Cyc_is_cfile( struct _tagged_string* n){ return*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(*
n, sizeof( unsigned char), 0) !='-';} struct _tagged_ptr3{ struct _tagged_string**
curr; struct _tagged_string** base; struct _tagged_string** last_plus_one; } ;
struct _tuple11{ struct _tagged_string f1; void* f2; struct _tagged_string f3; }
; struct _tagged_ptr4{ struct _tuple11** curr; struct _tuple11** base; struct
_tuple11** last_plus_one; } ; int Cyc_main( int argc, struct _tagged_ptr0 argv){
struct Cyc_List_List* options=({ struct Cyc_List_List*(* _temp378)( struct
_tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4)) Cyc_List_list;
struct _tuple11* _temp381=({ struct _tuple11* _temp630=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp630->f1=({ unsigned char* _temp635=(
unsigned char*)"-v"; struct _tagged_string _temp636; _temp636.curr= _temp635;
_temp636.base= _temp635; _temp636.last_plus_one= _temp635 + 3; _temp636;});
_temp630->f2=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp633=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp633[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp634; _temp634.tag= Cyc_Arg_Set_spec; _temp634.f1=& Cyc_v_r; _temp634;});
_temp633;}); _temp630->f3=({ unsigned char* _temp631=( unsigned char*)"    Print compilation stages verbosely";
struct _tagged_string _temp632; _temp632.curr= _temp631; _temp632.base= _temp631;
_temp632.last_plus_one= _temp631 + 39; _temp632;}); _temp630;}); struct _tuple11*
_temp382=({ struct _tuple11* _temp623=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp623->f1=({ unsigned char* _temp628=( unsigned char*)"-o";
struct _tagged_string _temp629; _temp629.curr= _temp628; _temp629.base= _temp628;
_temp629.last_plus_one= _temp628 + 3; _temp629;}); _temp623->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp626=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp626[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp627; _temp627.tag= Cyc_Arg_String_spec; _temp627.f1=
Cyc_set_output_file; _temp627;}); _temp626;}); _temp623->f3=({ unsigned char*
_temp624=( unsigned char*)"    Output file name"; struct _tagged_string _temp625;
_temp625.curr= _temp624; _temp625.base= _temp624; _temp625.last_plus_one=
_temp624 + 21; _temp625;}); _temp623;}); struct _tuple11* _temp383=({ struct
_tuple11* _temp616=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp616->f1=({ unsigned char* _temp621=( unsigned char*)"-D*"; struct
_tagged_string _temp622; _temp622.curr= _temp621; _temp622.base= _temp621;
_temp622.last_plus_one= _temp621 + 4; _temp622;}); _temp616->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp619=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp619[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp620; _temp620.tag= Cyc_Arg_Flag_spec; _temp620.f1= Cyc_add_cpparg; _temp620;});
_temp619;}); _temp616->f3=({ unsigned char* _temp617=( unsigned char*)"   Pass definition to preprocessor";
struct _tagged_string _temp618; _temp618.curr= _temp617; _temp618.base= _temp617;
_temp618.last_plus_one= _temp617 + 35; _temp618;}); _temp616;}); struct _tuple11*
_temp384=({ struct _tuple11* _temp609=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp609->f1=({ unsigned char* _temp614=( unsigned char*)"-B*";
struct _tagged_string _temp615; _temp615.curr= _temp614; _temp615.base= _temp614;
_temp615.last_plus_one= _temp614 + 4; _temp615;}); _temp609->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp612=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp612[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp613; _temp613.tag= Cyc_Arg_Flag_spec; _temp613.f1= Cyc_add_cyclone_exec_prefix_path;
_temp613;}); _temp612;}); _temp609->f3=({ unsigned char* _temp610=(
unsigned char*)"   Add to the list of directories to search for compiler files";
struct _tagged_string _temp611; _temp611.curr= _temp610; _temp611.base= _temp610;
_temp611.last_plus_one= _temp610 + 63; _temp611;}); _temp609;}); struct _tuple11*
_temp385=({ struct _tuple11* _temp602=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp602->f1=({ unsigned char* _temp607=( unsigned char*)"-I*";
struct _tagged_string _temp608; _temp608.curr= _temp607; _temp608.base= _temp607;
_temp608.last_plus_one= _temp607 + 4; _temp608;}); _temp602->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp605=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp605[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp606; _temp606.tag= Cyc_Arg_Flag_spec; _temp606.f1= Cyc_add_cpparg; _temp606;});
_temp605;}); _temp602->f3=({ unsigned char* _temp603=( unsigned char*)"   Add to the list of directories to search for include files";
struct _tagged_string _temp604; _temp604.curr= _temp603; _temp604.base= _temp603;
_temp604.last_plus_one= _temp603 + 62; _temp604;}); _temp602;}); struct _tuple11*
_temp386=({ struct _tuple11* _temp595=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp595->f1=({ unsigned char* _temp600=( unsigned char*)"-L*";
struct _tagged_string _temp601; _temp601.curr= _temp600; _temp601.base= _temp600;
_temp601.last_plus_one= _temp600 + 4; _temp601;}); _temp595->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp598=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp598[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp599; _temp599.tag= Cyc_Arg_Flag_spec; _temp599.f1= Cyc_add_cpparg; _temp599;});
_temp598;}); _temp595->f3=({ unsigned char* _temp596=( unsigned char*)"   Add to the list of directories for -l";
struct _tagged_string _temp597; _temp597.curr= _temp596; _temp597.base= _temp596;
_temp597.last_plus_one= _temp596 + 41; _temp597;}); _temp595;}); struct _tuple11*
_temp387=({ struct _tuple11* _temp588=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp588->f1=({ unsigned char* _temp593=( unsigned char*)"-l*";
struct _tagged_string _temp594; _temp594.curr= _temp593; _temp594.base= _temp593;
_temp594.last_plus_one= _temp593 + 4; _temp594;}); _temp588->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp591=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp591[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp592; _temp592.tag= Cyc_Arg_Flag_spec; _temp592.f1= Cyc_add_libarg; _temp592;});
_temp591;}); _temp588->f3=({ unsigned char* _temp589=( unsigned char*)"   Library file";
struct _tagged_string _temp590; _temp590.curr= _temp589; _temp590.base= _temp589;
_temp590.last_plus_one= _temp589 + 16; _temp590;}); _temp588;}); struct _tuple11*
_temp388=({ struct _tuple11* _temp581=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp581->f1=({ unsigned char* _temp586=( unsigned char*)"-c";
struct _tagged_string _temp587; _temp587.curr= _temp586; _temp587.base= _temp586;
_temp587.last_plus_one= _temp586 + 3; _temp587;}); _temp581->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp584=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp584[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp585; _temp585.tag= Cyc_Arg_Unit_spec; _temp585.f1= Cyc_set_stop_after_objectfile;
_temp585;}); _temp584;}); _temp581->f3=({ unsigned char* _temp582=(
unsigned char*)"    Produce object file"; struct _tagged_string _temp583;
_temp583.curr= _temp582; _temp583.base= _temp582; _temp583.last_plus_one=
_temp582 + 24; _temp583;}); _temp581;}); struct _tuple11* _temp389=({ struct
_tuple11* _temp574=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp574->f1=({ unsigned char* _temp579=( unsigned char*)"-O"; struct
_tagged_string _temp580; _temp580.curr= _temp579; _temp580.base= _temp579;
_temp580.last_plus_one= _temp579 + 3; _temp580;}); _temp574->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp577=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp577[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp578; _temp578.tag= Cyc_Arg_Flag_spec; _temp578.f1= Cyc_add_ccarg; _temp578;});
_temp577;}); _temp574->f3=({ unsigned char* _temp575=( unsigned char*)"    Optimize";
struct _tagged_string _temp576; _temp576.curr= _temp575; _temp576.base= _temp575;
_temp576.last_plus_one= _temp575 + 13; _temp576;}); _temp574;}); struct _tuple11*
_temp390=({ struct _tuple11* _temp567=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp567->f1=({ unsigned char* _temp572=( unsigned char*)"-O2";
struct _tagged_string _temp573; _temp573.curr= _temp572; _temp573.base= _temp572;
_temp573.last_plus_one= _temp572 + 4; _temp573;}); _temp567->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp570=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp570[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp571; _temp571.tag= Cyc_Arg_Flag_spec; _temp571.f1= Cyc_add_ccarg; _temp571;});
_temp570;}); _temp567->f3=({ unsigned char* _temp568=( unsigned char*)"   Optimize";
struct _tagged_string _temp569; _temp569.curr= _temp568; _temp569.base= _temp568;
_temp569.last_plus_one= _temp568 + 12; _temp569;}); _temp567;}); struct _tuple11*
_temp391=({ struct _tuple11* _temp560=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp560->f1=({ unsigned char* _temp565=( unsigned char*)"-O3";
struct _tagged_string _temp566; _temp566.curr= _temp565; _temp566.base= _temp565;
_temp566.last_plus_one= _temp565 + 4; _temp566;}); _temp560->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp563=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp563[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp564; _temp564.tag= Cyc_Arg_Flag_spec; _temp564.f1= Cyc_add_ccarg; _temp564;});
_temp563;}); _temp560->f3=({ unsigned char* _temp561=( unsigned char*)"   Optimize";
struct _tagged_string _temp562; _temp562.curr= _temp561; _temp562.base= _temp561;
_temp562.last_plus_one= _temp561 + 12; _temp562;}); _temp560;}); struct _tuple11*
_temp392=({ struct _tuple11* _temp553=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp553->f1=({ unsigned char* _temp558=( unsigned char*)"-g";
struct _tagged_string _temp559; _temp559.curr= _temp558; _temp559.base= _temp558;
_temp559.last_plus_one= _temp558 + 3; _temp559;}); _temp553->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp556=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp556[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp557; _temp557.tag= Cyc_Arg_Flag_spec; _temp557.f1= Cyc_add_ccarg; _temp557;});
_temp556;}); _temp553->f3=({ unsigned char* _temp554=( unsigned char*)"    Compile for debugging";
struct _tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 26; _temp555;}); _temp553;}); struct _tuple11*
_temp393=({ struct _tuple11* _temp546=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp546->f1=({ unsigned char* _temp551=( unsigned char*)"-p";
struct _tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 3; _temp552;}); _temp546->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp549=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp549[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp550; _temp550.tag= Cyc_Arg_Flag_spec; _temp550.f1= Cyc_add_ccarg; _temp550;});
_temp549;}); _temp546->f3=({ unsigned char* _temp547=( unsigned char*)"    Compile for prof";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 21; _temp548;}); _temp546;}); struct _tuple11*
_temp394=({ struct _tuple11* _temp539=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp539->f1=({ unsigned char* _temp544=( unsigned char*)"-pg";
struct _tagged_string _temp545; _temp545.curr= _temp544; _temp545.base= _temp544;
_temp545.last_plus_one= _temp544 + 4; _temp545;}); _temp539->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp542=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp542[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp543; _temp543.tag= Cyc_Arg_Flag_spec; _temp543.f1= Cyc_add_ccarg; _temp543;});
_temp542;}); _temp539->f3=({ unsigned char* _temp540=( unsigned char*)"   Compile for gprof";
struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 21; _temp541;}); _temp539;}); struct _tuple11*
_temp395=({ struct _tuple11* _temp532=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp532->f1=({ unsigned char* _temp537=( unsigned char*)"-S";
struct _tagged_string _temp538; _temp538.curr= _temp537; _temp538.base= _temp537;
_temp538.last_plus_one= _temp537 + 3; _temp538;}); _temp532->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp535=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp535[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp536; _temp536.tag= Cyc_Arg_Unit_spec; _temp536.f1= Cyc_set_stop_after_asmfile;
_temp536;}); _temp535;}); _temp532->f3=({ unsigned char* _temp533=(
unsigned char*)"    Stop after producing assembly code"; struct _tagged_string
_temp534; _temp534.curr= _temp533; _temp534.base= _temp533; _temp534.last_plus_one=
_temp533 + 39; _temp534;}); _temp532;}); struct _tuple11* _temp396=({ struct
_tuple11* _temp525=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp525->f1=({ unsigned char* _temp530=( unsigned char*)"-M"; struct
_tagged_string _temp531; _temp531.curr= _temp530; _temp531.base= _temp530;
_temp531.last_plus_one= _temp530 + 3; _temp531;}); _temp525->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp528=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp528[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp529; _temp529.tag= Cyc_Arg_Unit_spec; _temp529.f1= Cyc_set_produce_dependencies;
_temp529;}); _temp528;}); _temp525->f3=({ unsigned char* _temp526=(
unsigned char*)"    Produce dependencies"; struct _tagged_string _temp527;
_temp527.curr= _temp526; _temp527.base= _temp526; _temp527.last_plus_one=
_temp526 + 25; _temp527;}); _temp525;}); struct _tuple11* _temp397=({ struct
_tuple11* _temp518=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp518->f1=({ unsigned char* _temp523=( unsigned char*)"-E"; struct
_tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 3; _temp524;}); _temp518->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp521=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp521[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp522; _temp522.tag= Cyc_Arg_Set_spec; _temp522.f1=& Cyc_stop_after_cpp_r;
_temp522;}); _temp521;}); _temp518->f3=({ unsigned char* _temp519=(
unsigned char*)"    Stop after preprocessing"; struct _tagged_string _temp520;
_temp520.curr= _temp519; _temp520.base= _temp519; _temp520.last_plus_one=
_temp519 + 29; _temp520;}); _temp518;}); struct _tuple11* _temp398=({ struct
_tuple11* _temp511=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp511->f1=({ unsigned char* _temp516=( unsigned char*)"-parseonly"; struct
_tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 11; _temp517;}); _temp511->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp514=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp514[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp515; _temp515.tag= Cyc_Arg_Set_spec; _temp515.f1=& Cyc_parseonly_r;
_temp515;}); _temp514;}); _temp511->f3=({ unsigned char* _temp512=(
unsigned char*)"\n        Stop after parsing"; struct _tagged_string _temp513;
_temp513.curr= _temp512; _temp513.base= _temp512; _temp513.last_plus_one=
_temp512 + 28; _temp513;}); _temp511;}); struct _tuple11* _temp399=({ struct
_tuple11* _temp504=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp504->f1=({ unsigned char* _temp509=( unsigned char*)"-tc"; struct
_tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 4; _temp510;}); _temp504->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp507=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp507[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp508; _temp508.tag= Cyc_Arg_Set_spec; _temp508.f1=& Cyc_tc_r; _temp508;});
_temp507;}); _temp504->f3=({ unsigned char* _temp505=( unsigned char*)"   Stop after type checking";
struct _tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 28; _temp506;}); _temp504;}); struct _tuple11*
_temp400=({ struct _tuple11* _temp497=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp497->f1=({ unsigned char* _temp502=( unsigned char*)"-toc";
struct _tagged_string _temp503; _temp503.curr= _temp502; _temp503.base= _temp502;
_temp503.last_plus_one= _temp502 + 5; _temp503;}); _temp497->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp500=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp500[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp501; _temp501.tag= Cyc_Arg_Set_spec; _temp501.f1=& Cyc_toc_r; _temp501;});
_temp500;}); _temp497->f3=({ unsigned char* _temp498=( unsigned char*)"  Stop after translation to C";
struct _tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 30; _temp499;}); _temp497;}); struct _tuple11*
_temp401=({ struct _tuple11* _temp490=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp490->f1=({ unsigned char* _temp495=( unsigned char*)"-ic";
struct _tagged_string _temp496; _temp496.curr= _temp495; _temp496.base= _temp495;
_temp496.last_plus_one= _temp495 + 4; _temp496;}); _temp490->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp493=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp493[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp494; _temp494.tag= Cyc_Arg_Set_spec; _temp494.f1=& Cyc_ic_r; _temp494;});
_temp493;}); _temp490->f3=({ unsigned char* _temp491=( unsigned char*)"   Activate the link-checker";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 29; _temp492;}); _temp490;}); struct _tuple11*
_temp402=({ struct _tuple11* _temp483=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp483->f1=({ unsigned char* _temp488=( unsigned char*)"-pp";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 4; _temp489;}); _temp483->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp486=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp486[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp487; _temp487.tag= Cyc_Arg_Set_spec; _temp487.f1=& Cyc_pp_r; _temp487;});
_temp486;}); _temp483->f3=({ unsigned char* _temp484=( unsigned char*)"   Pretty print";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 16; _temp485;}); _temp483;}); struct _tuple11*
_temp403=({ struct _tuple11* _temp476=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp476->f1=({ unsigned char* _temp481=( unsigned char*)"-up";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 4; _temp482;}); _temp476->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp479=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp479[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp480; _temp480.tag= Cyc_Arg_Clear_spec; _temp480.f1=&
Cyc_pp_r; _temp480;}); _temp479;}); _temp476->f3=({ unsigned char* _temp477=(
unsigned char*)"   Ugly print"; struct _tagged_string _temp478; _temp478.curr=
_temp477; _temp478.base= _temp477; _temp478.last_plus_one= _temp477 + 14;
_temp478;}); _temp476;}); struct _tuple11* _temp404=({ struct _tuple11* _temp469=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp469->f1=({
unsigned char* _temp474=( unsigned char*)"-tovc"; struct _tagged_string _temp475;
_temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 6; _temp475;}); _temp469->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp472=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp472[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp473; _temp473.tag= Cyc_Arg_Set_spec;
_temp473.f1=& Cyc_tovc_r; _temp473;}); _temp472;}); _temp469->f3=({
unsigned char* _temp470=( unsigned char*)" Avoid gcc extensions in C output";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 34; _temp471;}); _temp469;}); struct _tuple11*
_temp405=({ struct _tuple11* _temp462=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp462->f1=({ unsigned char* _temp467=( unsigned char*)"-save-temps";
struct _tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 12; _temp468;}); _temp462->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp465=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp465[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp466; _temp466.tag= Cyc_Arg_Unit_spec; _temp466.f1= Cyc_set_save_temps;
_temp466;}); _temp465;}); _temp462->f3=({ unsigned char* _temp463=(
unsigned char*)"\n        Don't delete temporary files"; struct _tagged_string
_temp464; _temp464.curr= _temp463; _temp464.base= _temp463; _temp464.last_plus_one=
_temp463 + 38; _temp464;}); _temp462;}); struct _tuple11* _temp406=({ struct
_tuple11* _temp455=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp455->f1=({ unsigned char* _temp460=( unsigned char*)"-save-c"; struct
_tagged_string _temp461; _temp461.curr= _temp460; _temp461.base= _temp460;
_temp461.last_plus_one= _temp460 + 8; _temp461;}); _temp455->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp458=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp458[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp459; _temp459.tag= Cyc_Arg_Set_spec; _temp459.f1=& Cyc_save_c_r; _temp459;});
_temp458;}); _temp455->f3=({ unsigned char* _temp456=( unsigned char*)"\n        Don't delete temporary C files";
struct _tagged_string _temp457; _temp457.curr= _temp456; _temp457.base= _temp456;
_temp457.last_plus_one= _temp456 + 40; _temp457;}); _temp455;}); struct _tuple11*
_temp407=({ struct _tuple11* _temp448=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp448->f1=({ unsigned char* _temp453=( unsigned char*)"-nocyc";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 7; _temp454;}); _temp448->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp451=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp451[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp452; _temp452.tag= Cyc_Arg_Clear_spec; _temp452.f1=&
Cyc_add_cyc_namespace_r; _temp452;}); _temp451;}); _temp448->f3=({ unsigned char*
_temp449=( unsigned char*)"Don't add implicit namespace Cyc"; struct
_tagged_string _temp450; _temp450.curr= _temp449; _temp450.base= _temp449;
_temp450.last_plus_one= _temp449 + 33; _temp450;}); _temp448;}); struct _tuple11*
_temp408=({ struct _tuple11* _temp441=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp441->f1=({ unsigned char* _temp446=( unsigned char*)"-noremoveunused";
struct _tagged_string _temp447; _temp447.curr= _temp446; _temp447.base= _temp446;
_temp447.last_plus_one= _temp446 + 16; _temp447;}); _temp441->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp444=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp444[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp445; _temp445.tag= Cyc_Arg_Set_spec; _temp445.f1=& Cyc_noshake_r; _temp445;});
_temp444;}); _temp441->f3=({ unsigned char* _temp442=( unsigned char*)"\n        Don't remove externed variables that aren't used";
struct _tagged_string _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 58; _temp443;}); _temp441;}); struct _tuple11*
_temp409=({ struct _tuple11* _temp434=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp434->f1=({ unsigned char* _temp439=( unsigned char*)"-noexpandtypedefs";
struct _tagged_string _temp440; _temp440.curr= _temp439; _temp440.base= _temp439;
_temp440.last_plus_one= _temp439 + 18; _temp440;}); _temp434->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp437=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp437[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp438; _temp438.tag= Cyc_Arg_Set_spec; _temp438.f1=& Cyc_noexpand_r; _temp438;});
_temp437;}); _temp434->f3=({ unsigned char* _temp435=( unsigned char*)"\n        Don't expand typedefs in pretty printing";
struct _tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 50; _temp436;}); _temp434;}); struct _tuple11*
_temp410=({ struct _tuple11* _temp427=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp427->f1=({ unsigned char* _temp432=( unsigned char*)"-printalltvars";
struct _tagged_string _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 15; _temp433;}); _temp427->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp430=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp430[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp431; _temp431.tag= Cyc_Arg_Set_spec; _temp431.f1=& Cyc_print_all_tvars_r;
_temp431;}); _temp430;}); _temp427->f3=({ unsigned char* _temp428=(
unsigned char*)"\n        Print all type variables (even implicit default effects)";
struct _tagged_string _temp429; _temp429.curr= _temp428; _temp429.base= _temp428;
_temp429.last_plus_one= _temp428 + 66; _temp429;}); _temp427;}); struct _tuple11*
_temp411=({ struct _tuple11* _temp420=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp420->f1=({ unsigned char* _temp425=( unsigned char*)"-printallkinds";
struct _tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 15; _temp426;}); _temp420->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp423=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp423[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp424; _temp424.tag= Cyc_Arg_Set_spec; _temp424.f1=& Cyc_print_all_kinds_r;
_temp424;}); _temp423;}); _temp420->f3=({ unsigned char* _temp421=(
unsigned char*)"\n        Always print kinds of type variables"; struct
_tagged_string _temp422; _temp422.curr= _temp421; _temp422.base= _temp421;
_temp422.last_plus_one= _temp421 + 46; _temp422;}); _temp420;}); struct _tuple11*
_temp412=({ struct _tuple11* _temp413=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp413->f1=({ unsigned char* _temp418=( unsigned char*)"-printfullevars";
struct _tagged_string _temp419; _temp419.curr= _temp418; _temp419.base= _temp418;
_temp419.last_plus_one= _temp418 + 16; _temp419;}); _temp413->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp416=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp416[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp417; _temp417.tag= Cyc_Arg_Set_spec; _temp417.f1=& Cyc_print_full_evars_r;
_temp417;}); _temp416;}); _temp413->f3=({ unsigned char* _temp414=(
unsigned char*)"\n       Print full information for evars (type debugging)";
struct _tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 58; _temp415;}); _temp413;}); struct _tuple11*
_temp379[ 32u]={ _temp381, _temp382, _temp383, _temp384, _temp385, _temp386,
_temp387, _temp388, _temp389, _temp390, _temp391, _temp392, _temp393, _temp394,
_temp395, _temp396, _temp397, _temp398, _temp399, _temp400, _temp401, _temp402,
_temp403, _temp404, _temp405, _temp406, _temp407, _temp408, _temp409, _temp410,
_temp411, _temp412}; struct _tagged_ptr4 _temp380={ _temp379, _temp379, _temp379
+ 32u}; _temp378( _temp380);}); Cyc_Arg_parse( options, Cyc_add_other,({
unsigned char* _temp296=( unsigned char*)"Options:"; struct _tagged_string
_temp297; _temp297.curr= _temp296; _temp297.base= _temp296; _temp297.last_plus_one=
_temp296 + 9; _temp297;}), argv);{ struct Cyc_List_List* _temp298=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); goto _LL299; _LL299:
for( 0; _temp298 != 0; _temp298=(( struct Cyc_List_List*) _check_null( _temp298))->tl){
Cyc_process_file(*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp298))->hd)); if( Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r?
1: Cyc_parseonly_r)? 1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs == 0){
return 0;}{ struct _tagged_string cyclone_exec_prefix=( struct _tagged_string)
Cyc_Stdlib_getenv(({ unsigned char* _temp376=( unsigned char*)"CYCLONE_EXEC_PREFIX";
struct _tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 20; _temp377;})); if( cyclone_exec_prefix.curr
!=(( struct _tagged_string)({ struct _tagged_string _temp300={ 0, 0, 0};
_temp300;})).curr){ Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List*
_temp301=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp301->hd=( void*)({ struct _tagged_string* _temp302=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp302[ 0]= cyclone_exec_prefix;
_temp302;}); _temp301->tl= Cyc_cyclone_exec_prefix_path; _temp301;});} Cyc_cyclone_exec_prefix_path=({
struct Cyc_List_List* _temp303=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp303->hd=( void*)({ struct _tagged_string* _temp304=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp304[ 0]=({
unsigned char* _temp305=( unsigned char*)"c:/cyclone/bin/cyc-lib"; struct
_tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 23; _temp306;}); _temp304;}); _temp303->tl=
Cyc_cyclone_exec_prefix_path; _temp303;}); Cyc_cyclone_exec_prefix_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_exec_prefix_path);{
struct _tagged_string _temp309= Cyc_do_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp307=( unsigned char*)"include/cyc_include.h"; struct
_tagged_string _temp308; _temp308.curr= _temp307; _temp308.base= _temp307;
_temp308.last_plus_one= _temp307 + 22; _temp308;})); goto _LL310; _LL310: Cyc_ccargs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{
struct _tagged_string _temp313= Cyc_String_str_sepstr( Cyc_ccargs,({
unsigned char* _temp311=( unsigned char*)" "; struct _tagged_string _temp312;
_temp312.curr= _temp311; _temp312.base= _temp311; _temp312.last_plus_one=
_temp311 + 2; _temp312;})); goto _LL314; _LL314: Cyc_libargs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_libargs);{ struct _tagged_string
_temp321= Cyc_String_str_sepstr(({ struct Cyc_List_List* _temp315=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp315->hd=( void*)({ struct
_tagged_string* _temp316=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp316[ 0]=({ unsigned char* _temp317=( unsigned char*)"";
struct _tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 1; _temp318;}); _temp316;}); _temp315->tl=
Cyc_libargs; _temp315;}),({ unsigned char* _temp319=( unsigned char*)" "; struct
_tagged_string _temp320; _temp320.curr= _temp319; _temp320.base= _temp319;
_temp320.last_plus_one= _temp319 + 2; _temp320;})); goto _LL322; _LL322: {
struct Cyc_List_List* stdlib; struct _tagged_string stdlib_string; if( Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r){ stdlib= 0; stdlib_string=({ unsigned char*
_temp323=( unsigned char*)""; struct _tagged_string _temp324; _temp324.curr=
_temp323; _temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 1;
_temp324;});} else{ struct _tagged_string _temp327= Cyc_do_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp325=( unsigned char*)"cyclib.a"; struct _tagged_string
_temp326; _temp326.curr= _temp325; _temp326.base= _temp325; _temp326.last_plus_one=
_temp325 + 9; _temp326;})); goto _LL328; _LL328: { struct _tagged_string
_temp331= Cyc_do_find( Cyc_cyclone_exec_prefix_path,({ unsigned char* _temp329=(
unsigned char*)"gc.a"; struct _tagged_string _temp330; _temp330.curr= _temp329;
_temp330.base= _temp329; _temp330.last_plus_one= _temp329 + 5; _temp330;}));
goto _LL332; _LL332: stdlib=({ struct Cyc_List_List*(* _temp333)( struct
_tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3)) Cyc_List_list;
struct _tagged_string* _temp336=({ struct _tagged_string* _temp337=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp337[ 0]=
_temp327; _temp337;}); struct _tagged_string* _temp334[ 1u]={ _temp336}; struct
_tagged_ptr3 _temp335={ _temp334, _temp334, _temp334 + 1u}; _temp333( _temp335);});
stdlib_string=( struct _tagged_string)({ struct _tagged_string _temp338=
_temp327; struct _tagged_string _temp339= _temp331; xprintf(" %.*s %.*s",
_temp338.last_plus_one - _temp338.curr, _temp338.curr, _temp339.last_plus_one -
_temp339.curr, _temp339.curr);});}} if( Cyc_ic_r){ Cyc_ccargs=(( struct Cyc_List_List*(*)(
int(* f)( struct _tagged_string*), struct Cyc_List_List* l)) Cyc_List_filter)(
Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=(( struct Cyc_List_List*(*)( int(* f)(
struct _tagged_string*), struct Cyc_List_List* l)) Cyc_List_filter)( Cyc_is_cfile,
Cyc_libargs);{ struct Cyc_List_List* _temp340=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( stdlib,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
Cyc_ccargs, Cyc_libargs)); goto _LL341; _LL341: { struct Cyc_List_List* _temp342=((
struct Cyc_List_List*(*)( struct Cyc_Interface_I*(* f)( struct _tagged_string*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_read_cycio, _temp340); goto _LL343;
_LL343: if( ! Cyc_stop_after_objectfile_r){ _temp340=({ struct Cyc_List_List*
_temp344=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp344->hd=( void*)({ struct _tagged_string* _temp345=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp345[ 0]=({ unsigned char*
_temp346=( unsigned char*)"<final>"; struct _tagged_string _temp347; _temp347.curr=
_temp346; _temp347.base= _temp346; _temp347.last_plus_one= _temp346 + 8;
_temp347;}); _temp345;}); _temp344->tl= _temp340; _temp344;}); _temp342=({
struct Cyc_List_List* _temp348=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp348->hd=( void*) Cyc_Interface_final(); _temp348->tl=
_temp342; _temp348;});}{ struct Cyc_Interface_I* _temp349= Cyc_Interface_merge_list(
_temp342, _temp340); goto _LL350; _LL350: if( _temp349 == 0){ fprintf(
sfile_to_file( Cyc_Stdio_stderr),"Error: interfaces incompatible\n"); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1;} if( Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r){
if( Cyc_output_file != 0){ struct _tagged_string _temp352=({ struct
_tagged_string _temp351=( struct _tagged_string) Cyc_Filename_chop_extension(*((
struct _tagged_string*) _check_null( Cyc_output_file))); xprintf("%.*s.cycio",
_temp351.last_plus_one - _temp351.curr, _temp351.curr);}); goto _LL353; _LL353: {
struct Cyc_Stdio___sFILE* _temp358= Cyc_try_file_open(( struct _tagged_string)
_temp352,({ unsigned char* _temp354=( unsigned char*)"w"; struct _tagged_string
_temp355; _temp355.curr= _temp354; _temp355.base= _temp354; _temp355.last_plus_one=
_temp354 + 2; _temp355;}),({ unsigned char* _temp356=( unsigned char*)"interface object file";
struct _tagged_string _temp357; _temp357.curr= _temp356; _temp357.base= _temp356;
_temp357.last_plus_one= _temp356 + 22; _temp357;})); goto _LL359; _LL359: if(
_temp358 == 0){ Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} Cyc_Interface_save((
struct Cyc_Interface_I*) _check_null( _temp349),( struct Cyc_Stdio___sFILE*)
_check_null( _temp358)); Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( _temp358));}}} else{ if( ! Cyc_Interface_is_subinterface( Cyc_Interface_empty(),(
struct Cyc_Interface_I*) _check_null( _temp349),({ struct _tuple9* _temp360=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp360->f1=({
unsigned char* _temp363=( unsigned char*)"empty interface"; struct
_tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 16; _temp364;}); _temp360->f2=({
unsigned char* _temp361=( unsigned char*)"global interface"; struct
_tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 17; _temp362;}); _temp360;}))){ fprintf(
sfile_to_file( Cyc_Stdio_stderr),"Error: some objects are still undefined\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;}}}}}}{ struct
_tagged_string _temp373=({ struct _tagged_string _temp368= _temp309; struct
_tagged_string _temp369= Cyc_output_file == 0?({ unsigned char* _temp365=(
unsigned char*)""; struct _tagged_string _temp366; _temp366.curr= _temp365;
_temp366.base= _temp365; _temp366.last_plus_one= _temp365 + 1; _temp366;}):(
struct _tagged_string)({ struct _tagged_string _temp367=*(( struct
_tagged_string*) _check_null( Cyc_output_file)); xprintf(" -o %.*s", _temp367.last_plus_one
- _temp367.curr, _temp367.curr);}); struct _tagged_string _temp370=( struct
_tagged_string) _temp313; struct _tagged_string _temp371= stdlib_string; struct
_tagged_string _temp372=( struct _tagged_string) _temp321; xprintf("gcc -include %.*s%.*s %.*s%.*s%.*s",
_temp368.last_plus_one - _temp368.curr, _temp368.curr, _temp369.last_plus_one -
_temp369.curr, _temp369.curr, _temp370.last_plus_one - _temp370.curr, _temp370.curr,
_temp371.last_plus_one - _temp371.curr, _temp371.curr, _temp372.last_plus_one -
_temp372.curr, _temp372.curr);}); goto _LL374; _LL374: if( Cyc_v_r){({ struct
_tagged_string _temp375=( struct _tagged_string) _temp373; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp375.last_plus_one - _temp375.curr,
_temp375.curr);});} if( system( string_to_Cstring(( struct _tagged_string)
_temp373)) != 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: C compiler failed\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} Cyc_remove_cfiles();
return Cyc_compile_failure? 1: 0;}}}}}}}