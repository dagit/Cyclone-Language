 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
_tagged_ptr1); extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned char
Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Arg_Error[ 10u]; static const
int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)();
} ; static const int Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{ int
tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_FlagString_spec=
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
struct Cyc_Stdio___sFILE*); extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_file_open( struct _tagged_string fname, struct _tagged_string mode);
extern void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE*); struct Cyc_Time_tm{
int tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int
tm_wday; int tm_yday; int tm_isdst; } ; extern unsigned int clock()
 __attribute__(( cdecl )) ; extern double difftime( int _time2, int _time1)
 __attribute__(( cdecl )) ; extern int mktime( struct Cyc_Time_tm* _timeptr)
 __attribute__(( cdecl )) ; extern int time( int* _timer)  __attribute__(( cdecl
)) ; extern struct Cyc_Time_tm* gmtime( const int* _timer)  __attribute__((
cdecl )) ; extern struct Cyc_Time_tm* localtime( const int* _timer)
 __attribute__(( cdecl )) ; extern struct Cyc_Time_tm* gmtime_r( const int*,
struct Cyc_Time_tm*)  __attribute__(( cdecl )) ; extern struct Cyc_Time_tm*
localtime_r( const int*, struct Cyc_Time_tm*)  __attribute__(( cdecl )) ; extern
int _timezone  __attribute__(( dllimport )) ; extern int _daylight
 __attribute__(( dllimport )) ; extern unsigned char* _tzname[ 2u]
 __attribute__(( dllimport )) ; extern void tzset()  __attribute__(( cdecl )) ;
struct Cyc___sFILE; struct Cyc__reent; struct Cyc__glue{ struct Cyc__glue* _next;
int _niobs; struct Cyc___sFILE* _iobs; } ; struct Cyc__Bigint{ struct Cyc__Bigint*
_next; int _k; int _maxwds; int _sign; int _wds; unsigned int _x[ 1u]; } ;
struct Cyc__atexit{ struct Cyc__atexit* _next; int _ind; void(* _fns[ 32u])(); }
; struct Cyc___sbuf{ unsigned char* _base; int _size; } ; struct Cyc___sFILE{
unsigned char* _p; int _r; int _w; short _flags; short _file; struct Cyc___sbuf
_bf; int _lbfsize; void* _cookie; int(* _read)( void* _cookie, unsigned char*
_buf, int _n)  __attribute__(( cdecl )) ; int(* _write)( void* _cookie, const
unsigned char* _buf, int _n)  __attribute__(( cdecl )) ; int(* _seek)( void*
_cookie, int _offset, int _whence)  __attribute__(( cdecl )) ; int(* _close)(
void* _cookie)  __attribute__(( cdecl )) ; struct Cyc___sbuf _ub; unsigned char*
_up; int _ur; unsigned char _ubuf[ 3u]; unsigned char _nbuf[ 1u]; struct Cyc___sbuf
_lb; int _blksize; int _offset; struct Cyc__reent* _data; } ; struct Cyc__reent_u1{
unsigned int _unused_rand; int _strtok_last; unsigned char _asctime_buf[ 26u];
struct Cyc_Time_tm _localtime_buf; int _gamma_signgam; unsigned long long
_rand_next; } ; struct Cyc__reent_u2{ unsigned int _nextf[ 30u]; unsigned int
_nmalloc[ 30u]; } ; union Cyc__reent_union{ struct Cyc__reent_u1 _reent; struct
Cyc__reent_u2 _unused; } ; struct Cyc__reent{ int _errno; struct Cyc___sFILE*
_stdin; struct Cyc___sFILE* _stdout; struct Cyc___sFILE* _stderr; int _inc;
unsigned char _emergency[ 25u]; int _current_category; const unsigned char*
_current_locale; int __sdidinit; void(* __cleanup)( struct Cyc__reent*)
 __attribute__(( cdecl )) ; struct Cyc__Bigint* _result; int _result_k; struct
Cyc__Bigint* _p5s; struct Cyc__Bigint** _freelist; int _cvtlen; unsigned char*
_cvtbuf; union Cyc__reent_union _new; struct Cyc__atexit* _atexit; struct Cyc__atexit
_atexit0; void(** _sig_func)( int); struct Cyc__glue __sglue; struct Cyc___sFILE
__sf[ 3u]; } ; extern struct Cyc__reent* _impure_ptr; extern void _reclaim_reent(
struct Cyc__reent*); struct Cyc_Stdlib__Div{ int quot; int rem; } ; struct Cyc_Stdlib__Ldiv{
int quot; int rem; } ; extern int __mb_cur_max  __attribute__(( dllimport )) ;
extern int abs( int)  __attribute__(( cdecl )) ; extern int atexit( void(*
__func)())  __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Div div( int
__numer, int __denom)  __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Ldiv
ldiv( int __numer, int __denom)  __attribute__(( cdecl )) ; extern int rand()
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
cyc_varargs; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static const int
Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; static
const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
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
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern void* Cyc_Absyn_file_typ_nocyc();
extern void* Cyc_Absyn_file_typ(); extern unsigned char Cyc_Lexing_Error[ 10u];
struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_string f1; }
; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
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
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; struct
Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct _tuple7* f1; } ;
extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
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
unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{ unsigned char*
tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{
unsigned char* tag; short f1; } ; extern unsigned char Cyc_Stmt_tok[ 13u];
struct Cyc_Stmt_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Stmt* f1; } ;
extern unsigned char Cyc_StorageClass_tok[ 21u]; struct Cyc_StorageClass_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_String_tok[ 15u];
struct Cyc_String_tok_struct{ unsigned char* tag; struct _tagged_string f1; } ;
extern unsigned char Cyc_Stringopt_tok[ 18u]; struct Cyc_Stringopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_StructFieldDeclListList_tok[
32u]; struct Cyc_StructFieldDeclListList_tok_struct{ unsigned char* tag; struct
Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[ 28u];
struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
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
Cyc_PP_Doc; extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_to_VC; extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern int Cyc_Absynpp_print_full_evars;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern void Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl,
struct Cyc_Stdio___sFILE* f); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern unsigned char Cyc_Tcutil_TypeErr[
12u]; extern void Cyc_Tcutil_flush_warnings(); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, int var_default_init, struct Cyc_List_List* _s);
extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
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
Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* _s); struct Cyc_Interface_I; extern struct Cyc_Interface_I*
Cyc_Interface_extract( struct Cyc_Dict_Dict* ae); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern void Cyc_Lex_lex_init();
static int Cyc_pp_r= 0; static int Cyc_noexpand_r= 0; static int Cyc_noshake_r=
0; static int Cyc_stop_after_cpp_r= 0; static int Cyc_parseonly_r= 0; static int
Cyc_tc_r= 0; static int Cyc_ic_r= 0; static int Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r=
0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0; static int Cyc_save_temps_r=
0; static int Cyc_save_c_r= 0; static int Cyc_add_cyc_namespace_r= 1; static
struct _tagged_string* Cyc_output_file= 0; static void Cyc_set_output_file(
struct _tagged_string s){ Cyc_output_file=({ struct _tagged_string* _temp0=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp0[ 0]=
s; _temp0;});} static struct Cyc_List_List* Cyc_cppargs= 0; static void Cyc_add_cpparg(
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
_temp27;}));} static struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other(
struct _tagged_string s){ if( Cyc_is_cyclone_sourcefile( s)){ Cyc_cyclone_files=({
struct Cyc_List_List* _temp28=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp28->hd=( void*)({ struct _tagged_string* _temp29=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp29[ 0]= s;
_temp29;}); _temp28->tl= Cyc_cyclone_files; _temp28;});{ struct _tagged_string
_temp30= Cyc_Filename_chop_extension( s); goto _LL31; _LL31: { struct
_tagged_string _temp34= Cyc_String_strconcat(( struct _tagged_string) _temp30,({
unsigned char* _temp32=( unsigned char*)".c"; struct _tagged_string _temp33;
_temp33.curr= _temp32; _temp33.base= _temp32; _temp33.last_plus_one= _temp32 + 3;
_temp33;})); goto _LL35; _LL35: Cyc_add_ccarg(( struct _tagged_string) _temp34);}}}
else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct _tagged_string s){
if( Cyc_save_temps_r){ return;} else{ Cyc_Stdio_remove( s);}} int Cyc_compile_failure=
0; struct Cyc_Stdio___sFILE* Cyc_try_file_open( struct _tagged_string filename,
struct _tagged_string mode, struct _tagged_string msg_part){ struct
_handler_cons _temp36; _push_handler(& _temp36);{ int _temp38= 0; if( setjmp(
_temp36.handler)){ _temp38= 1;} if( ! _temp38){{ struct Cyc_Stdio___sFILE*
_temp39=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open( filename, mode);
_npop_handler( 0u); return _temp39;}; _pop_handler();} else{ void* _temp37=(
void*) _exn_thrown; void* _temp41= _temp37; _LL43: goto _LL44; _LL45: goto _LL46;
_LL44: Cyc_compile_failure= 1;({ struct _tagged_string _temp47= msg_part; struct
_tagged_string _temp48= filename; fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nError: couldn't open %.*s %.*s\n",
_temp47.last_plus_one - _temp47.curr, _temp47.curr, _temp48.last_plus_one -
_temp48.curr, _temp48.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return 0; _LL46:( void) _throw( _temp41); _LL42:;}}} struct
Cyc_List_List* Cyc_do_stage( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( void*, struct Cyc_List_List*), void* env, struct Cyc_List_List* tds){ struct
_tagged_string exn_string=({ unsigned char* _temp85=( unsigned char*)""; struct
_tagged_string _temp86; _temp86.curr= _temp85; _temp86.base= _temp85; _temp86.last_plus_one=
_temp85 + 1; _temp86;}); struct _tagged_string explain_string=({ unsigned char*
_temp83=( unsigned char*)""; struct _tagged_string _temp84; _temp84.curr=
_temp83; _temp84.base= _temp83; _temp84.last_plus_one= _temp83 + 1; _temp84;});
int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct* _temp79=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp79[ 0]=({ struct Cyc_Core_Impossible_struct _temp80; _temp80.tag= Cyc_Core_Impossible;
_temp80.f1=({ unsigned char* _temp81=( unsigned char*)""; struct _tagged_string
_temp82; _temp82.curr= _temp81; _temp82.base= _temp81; _temp82.last_plus_one=
_temp81 + 1; _temp82;}); _temp80;}); _temp79;}); struct Cyc_List_List* _temp49=
0; goto _LL50; _LL50:{ struct _handler_cons _temp51; _push_handler(& _temp51);{
int _temp53= 0; if( setjmp( _temp51.handler)){ _temp53= 1;} if( ! _temp53){
_temp49= f( env, tds);; _pop_handler();} else{ void* _temp52=( void*)
_exn_thrown; void* _temp55= _temp52; struct _tagged_string _temp65; _LL57: if(*((
void**) _temp55) == Cyc_Core_Impossible){ _LL66: _temp65=(( struct Cyc_Core_Impossible_struct*)
_temp55)->f1; goto _LL58;} else{ goto _LL59;} _LL59: if( _temp55 == Cyc_Dict_Absent){
goto _LL60;} else{ goto _LL61;} _LL61: goto _LL62; _LL63: goto _LL64; _LL58:
exn_string=({ unsigned char* _temp67=( unsigned char*)"Exception Core::Impossible";
struct _tagged_string _temp68; _temp68.curr= _temp67; _temp68.base= _temp67;
_temp68.last_plus_one= _temp67 + 27; _temp68;}); explain_string= _temp65; goto
_LL56; _LL60: exn_string=({ unsigned char* _temp69=( unsigned char*)"Exception Dict::Absent";
struct _tagged_string _temp70; _temp70.curr= _temp69; _temp70.base= _temp69;
_temp70.last_plus_one= _temp69 + 23; _temp70;}); goto _LL56; _LL62: ex= _temp55;
other_exn= 1; exn_string=({ unsigned char* _temp71=( unsigned char*)"Uncaught exception";
struct _tagged_string _temp72; _temp72.curr= _temp71; _temp72.base= _temp71;
_temp72.last_plus_one= _temp71 + 19; _temp72;}); goto _LL56; _LL64:( void)
_throw( _temp55); _LL56:;}}} if( Cyc_Position_error_p()){ Cyc_compile_failure= 1;}
if( Cyc_String_strcmp( exn_string,({ unsigned char* _temp73=( unsigned char*)"";
struct _tagged_string _temp74; _temp74.curr= _temp73; _temp74.base= _temp73;
_temp74.last_plus_one= _temp73 + 1; _temp74;})) != 0){ Cyc_compile_failure= 1;({
struct _tagged_string _temp75= exn_string; struct _tagged_string _temp76=
stage_name; struct _tagged_string _temp77= explain_string; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\n%.*s thrown during %.*s: %.*s", _temp75.last_plus_one
- _temp75.curr, _temp75.curr, _temp76.last_plus_one - _temp76.curr, _temp76.curr,
_temp77.last_plus_one - _temp77.curr, _temp77.curr);});} if( Cyc_compile_failure){
fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nCOMPILATION FAILED!\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); if( other_exn){( void) _throw( ex);}
return _temp49;} else{ if( Cyc_v_r){({ struct _tagged_string _temp78= stage_name;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s completed.\n", _temp78.last_plus_one
- _temp78.curr, _temp78.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return _temp49;}} return _temp49;} struct Cyc_List_List* Cyc_do_parse(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp87= Cyc_Parse_parse_file( f); goto _LL88; _LL88: Cyc_Lex_lex_init();
return _temp87;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= ! Cyc_noexpand_r;
Cyc_add_cyc_namespace_r= Cyc_add_cyc_namespace_r? ! Cyc_tc_r: 0; if( Cyc_add_cyc_namespace_r){
Cyc_Absyn_file_typ();} else{ Cyc_Absyn_file_typ_nocyc();} Cyc_Tc_tc( te, Cyc_add_cyc_namespace_r,
1, tds); if( ! Cyc_noshake_r){ tds= Cyc_Tc_treeshake( te, tds);} return tds;}
struct Cyc_List_List* Cyc_do_cfcheck( int ignore, struct Cyc_List_List* tds){
Cyc_NewControlFlow_cf_check( tds); return tds;} struct _tuple9{ struct Cyc_Tcenv_Tenv*
f1; struct Cyc_Stdio___sFILE* f2; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple9* params, struct Cyc_List_List* tds){ struct Cyc_Tcenv_Tenv*
_temp89=(* params).f1; goto _LL90; _LL90: { struct Cyc_Stdio___sFILE* _temp91=(*
params).f2; goto _LL92; _LL92: { struct Cyc_Interface_I* _temp93= Cyc_Interface_extract(
_temp89->ae); goto _LL94; _LL94: Cyc_Interface_print( _temp93, _temp91); return
tds;}}} struct Cyc_List_List* Cyc_do_translate( int ignore, struct Cyc_List_List*
tds){ return Cyc_Toc_toc( tds);} struct Cyc_List_List* Cyc_do_tovc( int ignore,
struct Cyc_List_List* tds){ return Cyc_Tovc_tovc( tds);} struct Cyc_List_List*
Cyc_do_print( struct Cyc_Stdio___sFILE* out_file, struct Cyc_List_List* tds){
Cyc_Absynpp_expand_typedefs= ! Cyc_noexpand_r; Cyc_Absynpp_decls_first=( Cyc_Absynpp_qvar_to_Cids=
! Cyc_tc_r); Cyc_Absynpp_to_VC= Cyc_tovc_r; if( Cyc_pp_r){ Cyc_Absynpp_decllist2file(
tds, out_file);} else{ Cyc_Absyndump_dumpdecllist2file( tds, out_file);} Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) out_file); return tds;} static struct Cyc_List_List*
Cyc_cfiles= 0; static void Cyc_remove_cfiles(){ if( ! Cyc_save_c_r){ for( 0; Cyc_cfiles
!= 0; Cyc_cfiles=(( struct Cyc_List_List*) _check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}}
static struct Cyc_List_List* Cyc_split_by_char( struct _tagged_string s,
unsigned char c){ if( s.curr ==(( struct _tagged_string)({ struct _tagged_string
_temp95={ 0, 0, 0}; _temp95;})).curr){ return 0;}{ struct Cyc_List_List* _temp96=
0; goto _LL97; _LL97: { unsigned int _temp98= Cyc_String_strlen( s); goto _LL99;
_LL99: while( _temp98 > 0) { struct _tagged_string _temp100= Cyc_String_strchr(
s, c); goto _LL101; _LL101: if( _temp100.curr ==(( struct _tagged_string)({
struct _tagged_string _temp102={ 0, 0, 0}; _temp102;})).curr){ _temp96=({ struct
Cyc_List_List* _temp103=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp103->hd=( void*)({ struct _tagged_string* _temp104=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp104[ 0]= s; _temp104;});
_temp103->tl= _temp96; _temp103;}); break;} else{ _temp96=({ struct Cyc_List_List*
_temp105=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp105->hd=( void*)({ struct _tagged_string* _temp106=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp106[ 0]=( struct _tagged_string)
Cyc_String_substring( s, 0,( unsigned int)((( struct _tagged_string) _temp100).curr
- s.curr)); _temp106;}); _temp105->tl= _temp96; _temp105;}); _temp98 -=(( struct
_tagged_string) _temp100).curr - s.curr; s=({ struct _tagged_string _temp107=
_temp100; _temp107.curr += 1; _temp107;});}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp96);}}} int Cyc_file_exists(
struct _tagged_string file){ struct Cyc_Stdio___sFILE* f= 0;{ struct
_handler_cons _temp108; _push_handler(& _temp108);{ int _temp110= 0; if( setjmp(
_temp108.handler)){ _temp110= 1;} if( ! _temp110){ f=( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_file_open( file,({ unsigned char* _temp111=( unsigned char*)"r";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 2; _temp112;}));; _pop_handler();} else{ void*
_temp109=( void*) _exn_thrown; void* _temp114= _temp109; _LL116: goto _LL117;
_LL118: goto _LL119; _LL117: goto _LL115; _LL119:( void) _throw( _temp114);
_LL115:;}}} if( f == 0){ return 0;} else{ Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f)); return 1;}} struct _tagged_string* Cyc_find( struct Cyc_List_List*
dirs, struct _tagged_string file){ if( file.curr ==(( struct _tagged_string)({
struct _tagged_string _temp120={ 0, 0, 0}; _temp120;})).curr){ return 0;} for( 0;
dirs != 0; dirs=(( struct Cyc_List_List*) _check_null( dirs))->tl){ struct
_tagged_string _temp121=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( dirs))->hd); goto _LL122; _LL122: if( _temp121.curr ==(( struct
_tagged_string)({ struct _tagged_string _temp123={ 0, 0, 0}; _temp123;})).curr?
1: Cyc_String_strlen( _temp121) == 0){ continue;}{ struct _tagged_string s=(
struct _tagged_string) Cyc_Filename_concat( _temp121, file); if( Cyc_file_exists(
s)){ return({ struct _tagged_string* _temp124=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp124[ 0]= s; _temp124;});}}}
return 0;} void Cyc_process_file( struct _tagged_string filename){ struct
_tagged_string _temp125= Cyc_Filename_chop_extension( filename); goto _LL126;
_LL126: { struct _tagged_string _temp129= Cyc_String_strconcat(( struct
_tagged_string) _temp125,({ unsigned char* _temp127=( unsigned char*)".cyp";
struct _tagged_string _temp128; _temp128.curr= _temp127; _temp128.base= _temp127;
_temp128.last_plus_one= _temp127 + 5; _temp128;})); goto _LL130; _LL130: {
struct _tagged_string _temp133= Cyc_String_strconcat(( struct _tagged_string)
_temp125,({ unsigned char* _temp131=( unsigned char*)".cyci"; struct
_tagged_string _temp132; _temp132.curr= _temp131; _temp132.base= _temp131;
_temp132.last_plus_one= _temp131 + 6; _temp132;})); goto _LL134; _LL134: {
struct _tagged_string _temp137= Cyc_String_strconcat(( struct _tagged_string)
_temp125,({ unsigned char* _temp135=( unsigned char*)".c"; struct _tagged_string
_temp136; _temp136.curr= _temp135; _temp136.base= _temp135; _temp136.last_plus_one=
_temp135 + 3; _temp136;})); goto _LL138; _LL138: if( Cyc_v_r){({ struct
_tagged_string _temp139= filename; fprintf( sfile_to_file( Cyc_Stdio_stderr),"Compiling %.*s\n",
_temp139.last_plus_one - _temp139.curr, _temp139.curr);});}{ struct Cyc_Stdio___sFILE*
f0= Cyc_try_file_open( filename,({ unsigned char* _temp239=( unsigned char*)"r";
struct _tagged_string _temp240; _temp240.curr= _temp239; _temp240.base= _temp239;
_temp240.last_plus_one= _temp239 + 2; _temp240;}),({ unsigned char* _temp241=(
unsigned char*)"input file"; struct _tagged_string _temp242; _temp242.curr=
_temp241; _temp242.base= _temp241; _temp242.last_plus_one= _temp241 + 11;
_temp242;})); if( Cyc_compile_failure){ return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f0));{ struct _tagged_string _temp146= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp140=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp140->hd=( void*)({ struct _tagged_string* _temp141=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp141[ 0]=({
unsigned char* _temp142=( unsigned char*)""; struct _tagged_string _temp143;
_temp143.curr= _temp142; _temp143.base= _temp142; _temp143.last_plus_one=
_temp142 + 1; _temp143;}); _temp141;}); _temp140->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs); _temp140;}),({
unsigned char* _temp144=( unsigned char*)" "; struct _tagged_string _temp145;
_temp145.curr= _temp144; _temp145.base= _temp144; _temp145.last_plus_one=
_temp144 + 2; _temp145;})); goto _LL147; _LL147: { struct _tagged_string
stdinc_string=({ unsigned char* _temp237=( unsigned char*)" -Ic:/cyclone/include";
struct _tagged_string _temp238; _temp238.curr= _temp237; _temp238.base= _temp237;
_temp238.last_plus_one= _temp237 + 22; _temp238;}); struct _tagged_string
_temp150= Cyc_Stdlib_getenv(({ unsigned char* _temp148=( unsigned char*)"CYCLONE_INCLUDE_PATH";
struct _tagged_string _temp149; _temp149.curr= _temp148; _temp149.base= _temp148;
_temp149.last_plus_one= _temp148 + 21; _temp149;})); goto _LL151; _LL151: if(
_temp150.curr !=({ struct _tagged_string _temp152={ 0, 0, 0}; _temp152;}).curr){
struct Cyc_List_List* _temp153= Cyc_split_by_char(( struct _tagged_string)
_temp150,':'); goto _LL154; _LL154: { struct _tagged_string _temp161= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp155=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp155->hd=( void*)({ struct _tagged_string* _temp156=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp156[ 0]=({
unsigned char* _temp157=( unsigned char*)""; struct _tagged_string _temp158;
_temp158.curr= _temp157; _temp158.base= _temp157; _temp158.last_plus_one=
_temp157 + 1; _temp158;}); _temp156;}); _temp155->tl= _temp153; _temp155;}),({
unsigned char* _temp159=( unsigned char*)" -I"; struct _tagged_string _temp160;
_temp160.curr= _temp159; _temp160.base= _temp159; _temp160.last_plus_one=
_temp159 + 4; _temp160;})); goto _LL162; _LL162: stdinc_string=( struct
_tagged_string)({ struct _tagged_string _temp163=( struct _tagged_string)
_temp161; struct _tagged_string _temp164= stdinc_string; xprintf("%.*s%.*s",
_temp163.last_plus_one - _temp163.curr, _temp163.curr, _temp164.last_plus_one -
_temp164.curr, _temp164.curr);});}}{ struct _tagged_string ofile_string; if( Cyc_stop_after_cpp_r){
if( Cyc_output_file != 0){ ofile_string=( struct _tagged_string)({ struct
_tagged_string _temp165=*(( struct _tagged_string*) _check_null( Cyc_output_file));
xprintf(" -o %.*s", _temp165.last_plus_one - _temp165.curr, _temp165.curr);});}
else{ ofile_string=({ unsigned char* _temp166=( unsigned char*)""; struct
_tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 1; _temp167;});}} else{ ofile_string=( struct
_tagged_string)({ struct _tagged_string _temp168=( struct _tagged_string)
_temp129; xprintf(" -o %.*s", _temp168.last_plus_one - _temp168.curr, _temp168.curr);});}{
struct _tagged_string _temp173=({ struct _tagged_string _temp169=( struct
_tagged_string) _temp146; struct _tagged_string _temp170= stdinc_string; struct
_tagged_string _temp171= ofile_string; struct _tagged_string _temp172= filename;
xprintf("gcc -x c -E -U__GNUC__ -nostdinc%.*s%.*s%.*s %.*s", _temp169.last_plus_one
- _temp169.curr, _temp169.curr, _temp170.last_plus_one - _temp170.curr, _temp170.curr,
_temp171.last_plus_one - _temp171.curr, _temp171.curr, _temp172.last_plus_one -
_temp172.curr, _temp172.curr);}); goto _LL174; _LL174: if( Cyc_v_r){({ struct
_tagged_string _temp175=( struct _tagged_string) _temp173; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp175.last_plus_one - _temp175.curr,
_temp175.curr);});} if( system( string_to_Cstring(( struct _tagged_string)
_temp173)) != 0){ Cyc_compile_failure= 1; fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nError: preprocessing\n");
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_string) _temp129); Cyc_Absynpp_print_using_stmts=( Cyc_Absynpp_print_externC_stmts=
1);{ struct Cyc_Stdio___sFILE* in_file= Cyc_try_file_open(( struct
_tagged_string) _temp129,({ unsigned char* _temp233=( unsigned char*)"r"; struct
_tagged_string _temp234; _temp234.curr= _temp233; _temp234.base= _temp233;
_temp234.last_plus_one= _temp233 + 2; _temp234;}),({ unsigned char* _temp235=(
unsigned char*)"file"; struct _tagged_string _temp236; _temp236.curr= _temp235;
_temp236.base= _temp235; _temp236.last_plus_one= _temp235 + 5; _temp236;})); if(
Cyc_compile_failure){ return;}{ struct Cyc_List_List* tds= 0;{ struct
_handler_cons _temp176; _push_handler(& _temp176);{ int _temp178= 0; if( setjmp(
_temp176.handler)){ _temp178= 1;} if( ! _temp178){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp179=( unsigned char*)"parsing"; struct
_tagged_string _temp180; _temp180.curr= _temp179; _temp180.base= _temp179;
_temp180.last_plus_one= _temp179 + 8; _temp180;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)
_check_null( in_file), tds);; _pop_handler();} else{ void* _temp177=( void*)
_exn_thrown; void* _temp182= _temp177; _LL184: goto _LL185; _LL186: goto _LL187;
_LL185: Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*) _check_null( in_file));
Cyc_remove_file(( struct _tagged_string) _temp129);( void) _throw( _temp182);
_LL187:( void) _throw( _temp182); _LL183:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( in_file)); if( Cyc_compile_failure){ Cyc_remove_file(( struct
_tagged_string) _temp129); return;}{ struct Cyc_Tcenv_Tenv* _temp188= Cyc_Tcenv_tc_init();
goto _LL189; _LL189: if( Cyc_parseonly_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp190=( unsigned char*)"type checking";
struct _tagged_string _temp191; _temp191.curr= _temp190; _temp191.base= _temp190;
_temp191.last_plus_one= _temp190 + 14; _temp191;}), Cyc_do_typecheck, _temp188,
tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_string)
_temp129); return;} tds=(( struct Cyc_List_List*(*)( struct _tagged_string
stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env,
struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp192=(
unsigned char*)"control-flow checking"; struct _tagged_string _temp193; _temp193.curr=
_temp192; _temp193.base= _temp192; _temp193.last_plus_one= _temp192 + 22;
_temp193;}), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();}
Cyc_remove_file(( struct _tagged_string) _temp129); if( Cyc_compile_failure){
return;} Cyc_Absynpp_print_using_stmts=( Cyc_Absynpp_print_externC_stmts= 0);
if( Cyc_ic_r){ struct Cyc_Stdio___sFILE* inter_file; inter_file= Cyc_try_file_open((
struct _tagged_string) _temp133,({ unsigned char* _temp194=( unsigned char*)"w";
struct _tagged_string _temp195; _temp195.curr= _temp194; _temp195.base= _temp194;
_temp195.last_plus_one= _temp194 + 2; _temp195;}),({ unsigned char* _temp196=(
unsigned char*)"interface file"; struct _tagged_string _temp197; _temp197.curr=
_temp196; _temp197.base= _temp196; _temp197.last_plus_one= _temp196 + 15;
_temp197;})); if( Cyc_compile_failure){ return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct _tuple9*,
struct Cyc_List_List*), struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)(({
unsigned char* _temp198=( unsigned char*)"interface checking"; struct
_tagged_string _temp199; _temp199.curr= _temp198; _temp199.base= _temp198;
_temp199.last_plus_one= _temp198 + 19; _temp199;}), Cyc_do_interface,({ struct
_tuple9* _temp200=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp200->f1= _temp188; _temp200->f2=( struct Cyc_Stdio___sFILE*) _check_null(
inter_file); _temp200;}), tds); if( inter_file != 0){ Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_file));} if( Cyc_compile_failure){
return;}} if( Cyc_tc_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp201=(
unsigned char*)"translation to C"; struct _tagged_string _temp202; _temp202.curr=
_temp201; _temp202.base= _temp201; _temp202.last_plus_one= _temp201 + 17;
_temp202;}), Cyc_do_translate, 1, tds); if( Cyc_compile_failure){ return;} if(
Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds)) Cyc_do_stage)(({ unsigned char* _temp203=( unsigned char*)"post-pass to VC";
struct _tagged_string _temp204; _temp204.curr= _temp203; _temp204.base= _temp203;
_temp204.last_plus_one= _temp203 + 16; _temp204;}), Cyc_do_tovc, 1, tds);} if(
Cyc_compile_failure){ return;} PRINTC: if( tds != 0){ struct Cyc_Stdio___sFILE*
out_file; if( Cyc_parseonly_r? 1: Cyc_tc_r){ if( Cyc_output_file != 0){ out_file=
Cyc_try_file_open(*(( struct _tagged_string*) _check_null( Cyc_output_file)),({
unsigned char* _temp205=( unsigned char*)"w"; struct _tagged_string _temp206;
_temp206.curr= _temp205; _temp206.base= _temp205; _temp206.last_plus_one=
_temp205 + 2; _temp206;}),({ unsigned char* _temp207=( unsigned char*)"output file";
struct _tagged_string _temp208; _temp208.curr= _temp207; _temp208.base= _temp207;
_temp208.last_plus_one= _temp207 + 12; _temp208;}));} else{ out_file=( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;}} else{ if( Cyc_toc_r? Cyc_output_file !=
0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_string*) _check_null( Cyc_output_file)),({
unsigned char* _temp209=( unsigned char*)"w"; struct _tagged_string _temp210;
_temp210.curr= _temp209; _temp210.base= _temp209; _temp210.last_plus_one=
_temp209 + 2; _temp210;}),({ unsigned char* _temp211=( unsigned char*)"output file";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 12; _temp212;}));} else{ out_file= Cyc_try_file_open((
struct _tagged_string) _temp137,({ unsigned char* _temp213=( unsigned char*)"w";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 2; _temp214;}),({ unsigned char* _temp215=(
unsigned char*)"output file"; struct _tagged_string _temp216; _temp216.curr=
_temp215; _temp216.base= _temp215; _temp216.last_plus_one= _temp215 + 12;
_temp216;}));}} if( Cyc_compile_failure){ return;}{ struct _handler_cons
_temp217; _push_handler(& _temp217);{ int _temp219= 0; if( setjmp( _temp217.handler)){
_temp219= 1;} if( ! _temp219){ tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp220=( unsigned char*)"printing"; struct
_tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 9; _temp221;}), Cyc_do_print,( struct Cyc_Stdio___sFILE*)
_check_null( out_file), tds);; _pop_handler();} else{ void* _temp218=( void*)
_exn_thrown; void* _temp223= _temp218; _LL225: goto _LL226; _LL227: goto _LL228;
_LL226: Cyc_compile_failure= 1; Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp229=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp229->hd=( void*)({
struct _tagged_string* _temp230=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp230[ 0]=( struct _tagged_string) _temp137;
_temp230;}); _temp229->tl= Cyc_cfiles; _temp229;});( void) _throw( _temp223);
_LL228:( void) _throw( _temp223); _LL224:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp231=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp231->hd=( void*)({
struct _tagged_string* _temp232=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp232[ 0]=( struct _tagged_string) _temp137;
_temp232;}); _temp231->tl= Cyc_cfiles; _temp231;});}}}}}}}}}}}}} struct _tuple10{
struct _tagged_string f1; void* f2; struct _tagged_string f3; } ; struct
_tagged_ptr3{ struct _tuple10** curr; struct _tuple10** base; struct _tuple10**
last_plus_one; } ; int Cyc_main( int argc, struct _tagged_ptr0 argv){ struct Cyc_List_List*
options=({ struct Cyc_List_List*(* _temp295)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple10* _temp298=({ struct
_tuple10* _temp547=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp547->f1=({ unsigned char* _temp552=( unsigned char*)"-v"; struct
_tagged_string _temp553; _temp553.curr= _temp552; _temp553.base= _temp552;
_temp553.last_plus_one= _temp552 + 3; _temp553;}); _temp547->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp550=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp550[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp551; _temp551.tag= Cyc_Arg_Set_spec; _temp551.f1=& Cyc_v_r; _temp551;});
_temp550;}); _temp547->f3=({ unsigned char* _temp548=( unsigned char*)"    Print compilation stages verbosely";
struct _tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 39; _temp549;}); _temp547;}); struct _tuple10*
_temp299=({ struct _tuple10* _temp540=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp540->f1=({ unsigned char* _temp545=( unsigned char*)"-o";
struct _tagged_string _temp546; _temp546.curr= _temp545; _temp546.base= _temp545;
_temp546.last_plus_one= _temp545 + 3; _temp546;}); _temp540->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp543=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp543[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp544; _temp544.tag= Cyc_Arg_String_spec; _temp544.f1=
Cyc_set_output_file; _temp544;}); _temp543;}); _temp540->f3=({ unsigned char*
_temp541=( unsigned char*)"    Output file name"; struct _tagged_string _temp542;
_temp542.curr= _temp541; _temp542.base= _temp541; _temp542.last_plus_one=
_temp541 + 21; _temp542;}); _temp540;}); struct _tuple10* _temp300=({ struct
_tuple10* _temp533=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp533->f1=({ unsigned char* _temp538=( unsigned char*)"-D*"; struct
_tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 4; _temp539;}); _temp533->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp536=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp536[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp537; _temp537.tag= Cyc_Arg_Flag_spec; _temp537.f1= Cyc_add_cpparg; _temp537;});
_temp536;}); _temp533->f3=({ unsigned char* _temp534=( unsigned char*)"   Pass definition to preprocessor";
struct _tagged_string _temp535; _temp535.curr= _temp534; _temp535.base= _temp534;
_temp535.last_plus_one= _temp534 + 35; _temp535;}); _temp533;}); struct _tuple10*
_temp301=({ struct _tuple10* _temp526=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp526->f1=({ unsigned char* _temp531=( unsigned char*)"-B*";
struct _tagged_string _temp532; _temp532.curr= _temp531; _temp532.base= _temp531;
_temp532.last_plus_one= _temp531 + 4; _temp532;}); _temp526->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp529=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp529[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp530; _temp530.tag= Cyc_Arg_Flag_spec; _temp530.f1= Cyc_add_cyclone_exec_prefix_path;
_temp530;}); _temp529;}); _temp526->f3=({ unsigned char* _temp527=(
unsigned char*)"   Add to the list of directories to search for compiler files";
struct _tagged_string _temp528; _temp528.curr= _temp527; _temp528.base= _temp527;
_temp528.last_plus_one= _temp527 + 63; _temp528;}); _temp526;}); struct _tuple10*
_temp302=({ struct _tuple10* _temp519=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp519->f1=({ unsigned char* _temp524=( unsigned char*)"-I*";
struct _tagged_string _temp525; _temp525.curr= _temp524; _temp525.base= _temp524;
_temp525.last_plus_one= _temp524 + 4; _temp525;}); _temp519->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp522=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp522[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp523; _temp523.tag= Cyc_Arg_Flag_spec; _temp523.f1= Cyc_add_cpparg; _temp523;});
_temp522;}); _temp519->f3=({ unsigned char* _temp520=( unsigned char*)"   Add to the list of directories to search for include files";
struct _tagged_string _temp521; _temp521.curr= _temp520; _temp521.base= _temp520;
_temp521.last_plus_one= _temp520 + 62; _temp521;}); _temp519;}); struct _tuple10*
_temp303=({ struct _tuple10* _temp512=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp512->f1=({ unsigned char* _temp517=( unsigned char*)"-L*";
struct _tagged_string _temp518; _temp518.curr= _temp517; _temp518.base= _temp517;
_temp518.last_plus_one= _temp517 + 4; _temp518;}); _temp512->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp515=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp515[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp516; _temp516.tag= Cyc_Arg_Flag_spec; _temp516.f1= Cyc_add_cpparg; _temp516;});
_temp515;}); _temp512->f3=({ unsigned char* _temp513=( unsigned char*)"   Add to the list of directories for -l";
struct _tagged_string _temp514; _temp514.curr= _temp513; _temp514.base= _temp513;
_temp514.last_plus_one= _temp513 + 41; _temp514;}); _temp512;}); struct _tuple10*
_temp304=({ struct _tuple10* _temp505=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp505->f1=({ unsigned char* _temp510=( unsigned char*)"-l*";
struct _tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 4; _temp511;}); _temp505->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp508=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp508[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp509; _temp509.tag= Cyc_Arg_Flag_spec; _temp509.f1= Cyc_add_libarg; _temp509;});
_temp508;}); _temp505->f3=({ unsigned char* _temp506=( unsigned char*)"   Library file";
struct _tagged_string _temp507; _temp507.curr= _temp506; _temp507.base= _temp506;
_temp507.last_plus_one= _temp506 + 16; _temp507;}); _temp505;}); struct _tuple10*
_temp305=({ struct _tuple10* _temp498=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp498->f1=({ unsigned char* _temp503=( unsigned char*)"-c";
struct _tagged_string _temp504; _temp504.curr= _temp503; _temp504.base= _temp503;
_temp504.last_plus_one= _temp503 + 3; _temp504;}); _temp498->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp501=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp501[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp502; _temp502.tag= Cyc_Arg_Unit_spec; _temp502.f1= Cyc_set_stop_after_objectfile;
_temp502;}); _temp501;}); _temp498->f3=({ unsigned char* _temp499=(
unsigned char*)"    Produce object file"; struct _tagged_string _temp500;
_temp500.curr= _temp499; _temp500.base= _temp499; _temp500.last_plus_one=
_temp499 + 24; _temp500;}); _temp498;}); struct _tuple10* _temp306=({ struct
_tuple10* _temp491=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp491->f1=({ unsigned char* _temp496=( unsigned char*)"-O"; struct
_tagged_string _temp497; _temp497.curr= _temp496; _temp497.base= _temp496;
_temp497.last_plus_one= _temp496 + 3; _temp497;}); _temp491->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp494=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp494[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp495; _temp495.tag= Cyc_Arg_Flag_spec; _temp495.f1= Cyc_add_ccarg; _temp495;});
_temp494;}); _temp491->f3=({ unsigned char* _temp492=( unsigned char*)"    Optimize";
struct _tagged_string _temp493; _temp493.curr= _temp492; _temp493.base= _temp492;
_temp493.last_plus_one= _temp492 + 13; _temp493;}); _temp491;}); struct _tuple10*
_temp307=({ struct _tuple10* _temp484=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp484->f1=({ unsigned char* _temp489=( unsigned char*)"-O2";
struct _tagged_string _temp490; _temp490.curr= _temp489; _temp490.base= _temp489;
_temp490.last_plus_one= _temp489 + 4; _temp490;}); _temp484->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp487=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp487[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp488; _temp488.tag= Cyc_Arg_Flag_spec; _temp488.f1= Cyc_add_ccarg; _temp488;});
_temp487;}); _temp484->f3=({ unsigned char* _temp485=( unsigned char*)"   Optimize";
struct _tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 12; _temp486;}); _temp484;}); struct _tuple10*
_temp308=({ struct _tuple10* _temp477=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp477->f1=({ unsigned char* _temp482=( unsigned char*)"-O3";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 4; _temp483;}); _temp477->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp480=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp480[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp481; _temp481.tag= Cyc_Arg_Flag_spec; _temp481.f1= Cyc_add_ccarg; _temp481;});
_temp480;}); _temp477->f3=({ unsigned char* _temp478=( unsigned char*)"   Optimize";
struct _tagged_string _temp479; _temp479.curr= _temp478; _temp479.base= _temp478;
_temp479.last_plus_one= _temp478 + 12; _temp479;}); _temp477;}); struct _tuple10*
_temp309=({ struct _tuple10* _temp470=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp470->f1=({ unsigned char* _temp475=( unsigned char*)"-g";
struct _tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 3; _temp476;}); _temp470->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp473=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp473[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp474; _temp474.tag= Cyc_Arg_Flag_spec; _temp474.f1= Cyc_add_ccarg; _temp474;});
_temp473;}); _temp470->f3=({ unsigned char* _temp471=( unsigned char*)"    Compile for debugging";
struct _tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 26; _temp472;}); _temp470;}); struct _tuple10*
_temp310=({ struct _tuple10* _temp463=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp463->f1=({ unsigned char* _temp468=( unsigned char*)"-p";
struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 3; _temp469;}); _temp463->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp466=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp466[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp467; _temp467.tag= Cyc_Arg_Flag_spec; _temp467.f1= Cyc_add_ccarg; _temp467;});
_temp466;}); _temp463->f3=({ unsigned char* _temp464=( unsigned char*)"    Compile for prof";
struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 21; _temp465;}); _temp463;}); struct _tuple10*
_temp311=({ struct _tuple10* _temp456=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp456->f1=({ unsigned char* _temp461=( unsigned char*)"-pg";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 4; _temp462;}); _temp456->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp459=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp459[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp460; _temp460.tag= Cyc_Arg_Flag_spec; _temp460.f1= Cyc_add_ccarg; _temp460;});
_temp459;}); _temp456->f3=({ unsigned char* _temp457=( unsigned char*)"   Compile for gprof";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 21; _temp458;}); _temp456;}); struct _tuple10*
_temp312=({ struct _tuple10* _temp449=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp449->f1=({ unsigned char* _temp454=( unsigned char*)"-S";
struct _tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 3; _temp455;}); _temp449->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp452=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp452[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp453; _temp453.tag= Cyc_Arg_Flag_spec; _temp453.f1= Cyc_add_ccarg; _temp453;});
_temp452;}); _temp449->f3=({ unsigned char* _temp450=( unsigned char*)"    Stop after producing assembly code";
struct _tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 39; _temp451;}); _temp449;}); struct _tuple10*
_temp313=({ struct _tuple10* _temp442=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp442->f1=({ unsigned char* _temp447=( unsigned char*)"-M";
struct _tagged_string _temp448; _temp448.curr= _temp447; _temp448.base= _temp447;
_temp448.last_plus_one= _temp447 + 3; _temp448;}); _temp442->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp445=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp445[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp446; _temp446.tag= Cyc_Arg_Unit_spec; _temp446.f1= Cyc_set_produce_dependencies;
_temp446;}); _temp445;}); _temp442->f3=({ unsigned char* _temp443=(
unsigned char*)"    Produce dependencies"; struct _tagged_string _temp444;
_temp444.curr= _temp443; _temp444.base= _temp443; _temp444.last_plus_one=
_temp443 + 25; _temp444;}); _temp442;}); struct _tuple10* _temp314=({ struct
_tuple10* _temp435=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp435->f1=({ unsigned char* _temp440=( unsigned char*)"-E"; struct
_tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 3; _temp441;}); _temp435->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp438=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp438[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp439; _temp439.tag= Cyc_Arg_Set_spec; _temp439.f1=& Cyc_stop_after_cpp_r;
_temp439;}); _temp438;}); _temp435->f3=({ unsigned char* _temp436=(
unsigned char*)"    Stop after preprocessing"; struct _tagged_string _temp437;
_temp437.curr= _temp436; _temp437.base= _temp436; _temp437.last_plus_one=
_temp436 + 29; _temp437;}); _temp435;}); struct _tuple10* _temp315=({ struct
_tuple10* _temp428=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp428->f1=({ unsigned char* _temp433=( unsigned char*)"-parseonly"; struct
_tagged_string _temp434; _temp434.curr= _temp433; _temp434.base= _temp433;
_temp434.last_plus_one= _temp433 + 11; _temp434;}); _temp428->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp431=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp431[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp432; _temp432.tag= Cyc_Arg_Set_spec; _temp432.f1=& Cyc_parseonly_r;
_temp432;}); _temp431;}); _temp428->f3=({ unsigned char* _temp429=(
unsigned char*)"\n        Stop after parsing"; struct _tagged_string _temp430;
_temp430.curr= _temp429; _temp430.base= _temp429; _temp430.last_plus_one=
_temp429 + 28; _temp430;}); _temp428;}); struct _tuple10* _temp316=({ struct
_tuple10* _temp421=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp421->f1=({ unsigned char* _temp426=( unsigned char*)"-tc"; struct
_tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 4; _temp427;}); _temp421->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp424=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp424[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp425; _temp425.tag= Cyc_Arg_Set_spec; _temp425.f1=& Cyc_tc_r; _temp425;});
_temp424;}); _temp421->f3=({ unsigned char* _temp422=( unsigned char*)"   Stop after type checking";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 28; _temp423;}); _temp421;}); struct _tuple10*
_temp317=({ struct _tuple10* _temp414=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp414->f1=({ unsigned char* _temp419=( unsigned char*)"-toc";
struct _tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 5; _temp420;}); _temp414->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp417=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp417[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp418; _temp418.tag= Cyc_Arg_Set_spec; _temp418.f1=& Cyc_toc_r; _temp418;});
_temp417;}); _temp414->f3=({ unsigned char* _temp415=( unsigned char*)"  Stop after translation to C";
struct _tagged_string _temp416; _temp416.curr= _temp415; _temp416.base= _temp415;
_temp416.last_plus_one= _temp415 + 30; _temp416;}); _temp414;}); struct _tuple10*
_temp318=({ struct _tuple10* _temp407=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp407->f1=({ unsigned char* _temp412=( unsigned char*)"-ic";
struct _tagged_string _temp413; _temp413.curr= _temp412; _temp413.base= _temp412;
_temp413.last_plus_one= _temp412 + 4; _temp413;}); _temp407->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp410=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp410[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp411; _temp411.tag= Cyc_Arg_Set_spec; _temp411.f1=& Cyc_ic_r; _temp411;});
_temp410;}); _temp407->f3=({ unsigned char* _temp408=( unsigned char*)"   Interface check";
struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 19; _temp409;}); _temp407;}); struct _tuple10*
_temp319=({ struct _tuple10* _temp400=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp400->f1=({ unsigned char* _temp405=( unsigned char*)"-pp";
struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 4; _temp406;}); _temp400->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp403=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp403[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp404; _temp404.tag= Cyc_Arg_Set_spec; _temp404.f1=& Cyc_pp_r; _temp404;});
_temp403;}); _temp400->f3=({ unsigned char* _temp401=( unsigned char*)"   Pretty print";
struct _tagged_string _temp402; _temp402.curr= _temp401; _temp402.base= _temp401;
_temp402.last_plus_one= _temp401 + 16; _temp402;}); _temp400;}); struct _tuple10*
_temp320=({ struct _tuple10* _temp393=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp393->f1=({ unsigned char* _temp398=( unsigned char*)"-up";
struct _tagged_string _temp399; _temp399.curr= _temp398; _temp399.base= _temp398;
_temp399.last_plus_one= _temp398 + 4; _temp399;}); _temp393->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp396=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp396[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp397; _temp397.tag= Cyc_Arg_Clear_spec; _temp397.f1=&
Cyc_pp_r; _temp397;}); _temp396;}); _temp393->f3=({ unsigned char* _temp394=(
unsigned char*)"   Ugly print"; struct _tagged_string _temp395; _temp395.curr=
_temp394; _temp395.base= _temp394; _temp395.last_plus_one= _temp394 + 14;
_temp395;}); _temp393;}); struct _tuple10* _temp321=({ struct _tuple10* _temp386=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp386->f1=({
unsigned char* _temp391=( unsigned char*)"-tovc"; struct _tagged_string _temp392;
_temp392.curr= _temp391; _temp392.base= _temp391; _temp392.last_plus_one=
_temp391 + 6; _temp392;}); _temp386->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp389=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp389[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp390; _temp390.tag= Cyc_Arg_Set_spec;
_temp390.f1=& Cyc_tovc_r; _temp390;}); _temp389;}); _temp386->f3=({
unsigned char* _temp387=( unsigned char*)" Avoid gcc extensions in C output";
struct _tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 34; _temp388;}); _temp386;}); struct _tuple10*
_temp322=({ struct _tuple10* _temp379=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp379->f1=({ unsigned char* _temp384=( unsigned char*)"-save-temps";
struct _tagged_string _temp385; _temp385.curr= _temp384; _temp385.base= _temp384;
_temp385.last_plus_one= _temp384 + 12; _temp385;}); _temp379->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp382=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp382[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp383; _temp383.tag= Cyc_Arg_Unit_spec; _temp383.f1= Cyc_set_save_temps;
_temp383;}); _temp382;}); _temp379->f3=({ unsigned char* _temp380=(
unsigned char*)"\n        Don't delete temporary files"; struct _tagged_string
_temp381; _temp381.curr= _temp380; _temp381.base= _temp380; _temp381.last_plus_one=
_temp380 + 38; _temp381;}); _temp379;}); struct _tuple10* _temp323=({ struct
_tuple10* _temp372=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp372->f1=({ unsigned char* _temp377=( unsigned char*)"-save-c"; struct
_tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 8; _temp378;}); _temp372->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp375=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp375[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp376; _temp376.tag= Cyc_Arg_Set_spec; _temp376.f1=& Cyc_save_c_r; _temp376;});
_temp375;}); _temp372->f3=({ unsigned char* _temp373=( unsigned char*)"\n        Don't delete temporary C files";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 40; _temp374;}); _temp372;}); struct _tuple10*
_temp324=({ struct _tuple10* _temp365=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp365->f1=({ unsigned char* _temp370=( unsigned char*)"-nocyc";
struct _tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 7; _temp371;}); _temp365->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp368=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp368[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp369; _temp369.tag= Cyc_Arg_Clear_spec; _temp369.f1=&
Cyc_add_cyc_namespace_r; _temp369;}); _temp368;}); _temp365->f3=({ unsigned char*
_temp366=( unsigned char*)"Don't add implicit namespace Cyc"; struct
_tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 33; _temp367;}); _temp365;}); struct _tuple10*
_temp325=({ struct _tuple10* _temp358=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp358->f1=({ unsigned char* _temp363=( unsigned char*)"-noremoveunused";
struct _tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 16; _temp364;}); _temp358->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp361=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp361[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp362; _temp362.tag= Cyc_Arg_Set_spec; _temp362.f1=& Cyc_noshake_r; _temp362;});
_temp361;}); _temp358->f3=({ unsigned char* _temp359=( unsigned char*)"\n        Don't remove externed variables that aren't used";
struct _tagged_string _temp360; _temp360.curr= _temp359; _temp360.base= _temp359;
_temp360.last_plus_one= _temp359 + 58; _temp360;}); _temp358;}); struct _tuple10*
_temp326=({ struct _tuple10* _temp351=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp351->f1=({ unsigned char* _temp356=( unsigned char*)"-noexpandtypedefs";
struct _tagged_string _temp357; _temp357.curr= _temp356; _temp357.base= _temp356;
_temp357.last_plus_one= _temp356 + 18; _temp357;}); _temp351->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp354=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp354[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp355; _temp355.tag= Cyc_Arg_Set_spec; _temp355.f1=& Cyc_noexpand_r; _temp355;});
_temp354;}); _temp351->f3=({ unsigned char* _temp352=( unsigned char*)"\n        Don't expand typedefs in pretty printing";
struct _tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 50; _temp353;}); _temp351;}); struct _tuple10*
_temp327=({ struct _tuple10* _temp344=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp344->f1=({ unsigned char* _temp349=( unsigned char*)"-printalltvars";
struct _tagged_string _temp350; _temp350.curr= _temp349; _temp350.base= _temp349;
_temp350.last_plus_one= _temp349 + 15; _temp350;}); _temp344->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp347=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp347[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp348; _temp348.tag= Cyc_Arg_Set_spec; _temp348.f1=& Cyc_Absynpp_print_all_tvars;
_temp348;}); _temp347;}); _temp344->f3=({ unsigned char* _temp345=(
unsigned char*)"\n        Print all type variables (even implicit default effects)";
struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 66; _temp346;}); _temp344;}); struct _tuple10*
_temp328=({ struct _tuple10* _temp337=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp337->f1=({ unsigned char* _temp342=( unsigned char*)"-printallkinds";
struct _tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 15; _temp343;}); _temp337->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp340=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp340[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp341; _temp341.tag= Cyc_Arg_Set_spec; _temp341.f1=& Cyc_Absynpp_print_all_kinds;
_temp341;}); _temp340;}); _temp337->f3=({ unsigned char* _temp338=(
unsigned char*)"\n        Always print kinds of type variables"; struct
_tagged_string _temp339; _temp339.curr= _temp338; _temp339.base= _temp338;
_temp339.last_plus_one= _temp338 + 46; _temp339;}); _temp337;}); struct _tuple10*
_temp329=({ struct _tuple10* _temp330=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp330->f1=({ unsigned char* _temp335=( unsigned char*)"-printfullevars";
struct _tagged_string _temp336; _temp336.curr= _temp335; _temp336.base= _temp335;
_temp336.last_plus_one= _temp335 + 16; _temp336;}); _temp330->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp333=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp333[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp334; _temp334.tag= Cyc_Arg_Set_spec; _temp334.f1=& Cyc_Absynpp_print_full_evars;
_temp334;}); _temp333;}); _temp330->f3=({ unsigned char* _temp331=(
unsigned char*)"\n       Print full information for evars (type debugging)";
struct _tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 58; _temp332;}); _temp330;}); struct _tuple10*
_temp296[ 32u]={ _temp298, _temp299, _temp300, _temp301, _temp302, _temp303,
_temp304, _temp305, _temp306, _temp307, _temp308, _temp309, _temp310, _temp311,
_temp312, _temp313, _temp314, _temp315, _temp316, _temp317, _temp318, _temp319,
_temp320, _temp321, _temp322, _temp323, _temp324, _temp325, _temp326, _temp327,
_temp328, _temp329}; struct _tagged_ptr3 _temp297={ _temp296, _temp296, _temp296
+ 32u}; _temp295( _temp297);}); Cyc_Arg_parse( options, Cyc_add_other,({
unsigned char* _temp243=( unsigned char*)"Options:"; struct _tagged_string
_temp244; _temp244.curr= _temp243; _temp244.base= _temp243; _temp244.last_plus_one=
_temp243 + 9; _temp244;}), argv);{ struct Cyc_List_List* _temp245=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); goto _LL246; _LL246:
for( 0; _temp245 != 0; _temp245=(( struct Cyc_List_List*) _check_null( _temp245))->tl){
Cyc_process_file(*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp245))->hd)); if( Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r?
1: Cyc_parseonly_r)? 1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs == 0){
return 0;}{ struct _tagged_string cyclone_exec_prefix=( struct _tagged_string)
Cyc_Stdlib_getenv(({ unsigned char* _temp293=( unsigned char*)"CYCLONE_EXEC_PREFIX";
struct _tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 20; _temp294;})); if( cyclone_exec_prefix.curr
!=(( struct _tagged_string)({ struct _tagged_string _temp247={ 0, 0, 0};
_temp247;})).curr){ Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List*
_temp248=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp248->hd=( void*)({ struct _tagged_string* _temp249=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp249[ 0]= cyclone_exec_prefix;
_temp249;}); _temp248->tl= Cyc_cyclone_exec_prefix_path; _temp248;});} Cyc_cyclone_exec_prefix_path=({
struct Cyc_List_List* _temp250=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp250->hd=( void*)({ struct _tagged_string* _temp251=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp251[ 0]=({
unsigned char* _temp252=( unsigned char*)"c:/cyclone/bin/cyc-lib"; struct
_tagged_string _temp253; _temp253.curr= _temp252; _temp253.base= _temp252;
_temp253.last_plus_one= _temp252 + 23; _temp253;}); _temp251;}); _temp250->tl=
Cyc_cyclone_exec_prefix_path; _temp250;}); Cyc_cyclone_exec_prefix_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_exec_prefix_path);{
struct _tagged_string* _temp256= Cyc_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp254=( unsigned char*)"include/cyc_include.h"; struct
_tagged_string _temp255; _temp255.curr= _temp254; _temp255.base= _temp254;
_temp255.last_plus_one= _temp254 + 22; _temp255;})); goto _LL257; _LL257: if(
_temp256 == 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file cyc_include.h");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;}{ struct _tagged_string
_temp260= Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_ccargs),({ unsigned char* _temp258=( unsigned char*)" ";
struct _tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 2; _temp259;})); goto _LL261; _LL261: {
struct _tagged_string _temp268= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp262=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp262->hd=( void*)({ struct _tagged_string* _temp263=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp263[ 0]=({ unsigned char*
_temp264=( unsigned char*)""; struct _tagged_string _temp265; _temp265.curr=
_temp264; _temp265.base= _temp264; _temp265.last_plus_one= _temp264 + 1;
_temp265;}); _temp263;}); _temp262->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_libargs); _temp262;}),({ unsigned char* _temp266=(
unsigned char*)" "; struct _tagged_string _temp267; _temp267.curr= _temp266;
_temp267.base= _temp266; _temp267.last_plus_one= _temp266 + 2; _temp267;}));
goto _LL269; _LL269: { struct _tagged_string stdlib_string; if( Cyc_stop_after_objectfile_r){
stdlib_string=({ unsigned char* _temp270=( unsigned char*)""; struct
_tagged_string _temp271; _temp271.curr= _temp270; _temp271.base= _temp270;
_temp271.last_plus_one= _temp270 + 1; _temp271;});} else{ struct _tagged_string*
_temp274= Cyc_find( Cyc_cyclone_exec_prefix_path,({ unsigned char* _temp272=(
unsigned char*)"cyclib.a"; struct _tagged_string _temp273; _temp273.curr=
_temp272; _temp273.base= _temp272; _temp273.last_plus_one= _temp272 + 9;
_temp273;})); goto _LL275; _LL275: { struct _tagged_string* _temp278= Cyc_find(
Cyc_cyclone_exec_prefix_path,({ unsigned char* _temp276=( unsigned char*)"gc.a";
struct _tagged_string _temp277; _temp277.curr= _temp276; _temp277.base= _temp276;
_temp277.last_plus_one= _temp276 + 5; _temp277;})); goto _LL279; _LL279: if(
_temp274 == 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file cyclib.a");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} if( _temp278 == 0){
fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file gc.a");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} stdlib_string=( struct
_tagged_string)({ struct _tagged_string _temp280=*(( struct _tagged_string*)
_check_null( _temp274)); struct _tagged_string _temp281=*(( struct
_tagged_string*) _check_null( _temp278)); xprintf(" %.*s %.*s", _temp280.last_plus_one
- _temp280.curr, _temp280.curr, _temp281.last_plus_one - _temp281.curr, _temp281.curr);});}}{
struct _tagged_string _temp290=({ struct _tagged_string _temp285=*(( struct
_tagged_string*) _check_null( _temp256)); struct _tagged_string _temp286= Cyc_output_file
== 0?({ unsigned char* _temp282=( unsigned char*)""; struct _tagged_string
_temp283; _temp283.curr= _temp282; _temp283.base= _temp282; _temp283.last_plus_one=
_temp282 + 1; _temp283;}):( struct _tagged_string)({ struct _tagged_string
_temp284=*(( struct _tagged_string*) _check_null( Cyc_output_file)); xprintf(" -o %.*s",
_temp284.last_plus_one - _temp284.curr, _temp284.curr);}); struct _tagged_string
_temp287=( struct _tagged_string) _temp260; struct _tagged_string _temp288=
stdlib_string; struct _tagged_string _temp289=( struct _tagged_string) _temp268;
xprintf("gcc -include %.*s%.*s %.*s%.*s%.*s", _temp285.last_plus_one - _temp285.curr,
_temp285.curr, _temp286.last_plus_one - _temp286.curr, _temp286.curr, _temp287.last_plus_one
- _temp287.curr, _temp287.curr, _temp288.last_plus_one - _temp288.curr, _temp288.curr,
_temp289.last_plus_one - _temp289.curr, _temp289.curr);}); goto _LL291; _LL291:
if( Cyc_v_r){({ struct _tagged_string _temp292=( struct _tagged_string) _temp290;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp292.last_plus_one -
_temp292.curr, _temp292.curr);});} if( system( string_to_Cstring(( struct
_tagged_string) _temp290)) != 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: C compiler failed\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} Cyc_remove_cfiles();
return Cyc_compile_failure? 1: 0;}}}}}}}