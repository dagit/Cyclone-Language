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
0; static int Cyc_stop_after_asmfile_r= 0; static int Cyc_tovc_r= 0; static int
Cyc_v_r= 0; static int Cyc_save_temps_r= 0; static int Cyc_save_c_r= 0; static
int Cyc_add_cyc_namespace_r= 1; static struct _tagged_string* Cyc_output_file= 0;
static void Cyc_set_output_file( struct _tagged_string s){ Cyc_output_file=({
struct _tagged_string* _temp0=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp0[ 0]= s; _temp0;});} static struct Cyc_List_List*
Cyc_cppargs= 0; static void Cyc_add_cpparg( struct _tagged_string s){ Cyc_cppargs=({
struct Cyc_List_List* _temp1=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp1->hd=( void*)({ struct _tagged_string* _temp2=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2[ 0]= s;
_temp2;}); _temp1->tl= Cyc_cppargs; _temp1;});} static int Cyc_is_cyclone_sourcefile(
struct _tagged_string s){ unsigned int _temp3= Cyc_String_strlen( s); goto _LL4;
_LL4: if( _temp3 <= 4){ return 0;} else{ return Cyc_String_strcmp(({ struct
_tagged_string _temp5= s; _temp5.curr +=( int)( _temp3 - 4); _temp5;}),({
unsigned char* _temp6=( unsigned char*)".cyc"; struct _tagged_string _temp7;
_temp7.curr= _temp6; _temp7.base= _temp6; _temp7.last_plus_one= _temp6 + 5;
_temp7;})) == 0;}} static struct Cyc_List_List* Cyc_cyclone_exec_prefix_path= 0;
static void Cyc_add_cyclone_exec_prefix_path( struct _tagged_string s){
unsigned int _temp8= Cyc_String_strlen( s); goto _LL9; _LL9: if( _temp8 <= 2){
return;}{ struct _tagged_string _temp10=( struct _tagged_string) Cyc_String_substring(
s, 2, _temp8 - 2); goto _LL11; _LL11: Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List*
_temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp12->hd=( void*)({ struct _tagged_string* _temp13=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp13[ 0]= _temp10; _temp13;});
_temp12->tl= Cyc_cyclone_exec_prefix_path; _temp12;});}} static struct Cyc_List_List*
Cyc_ccargs= 0; static void Cyc_add_ccarg( struct _tagged_string s){ Cyc_ccargs=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)({ struct _tagged_string* _temp15=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp15[ 0]= s;
_temp15;}); _temp14->tl= Cyc_ccargs; _temp14;});} static void Cyc_add_ccarg2(
struct _tagged_string flag, struct _tagged_string arg){ Cyc_ccargs=({ struct Cyc_List_List*
_temp16=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp16->hd=( void*)({ struct _tagged_string* _temp19=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp19[ 0]= arg; _temp19;});
_temp16->tl=({ struct Cyc_List_List* _temp17=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp17->hd=( void*)({ struct _tagged_string*
_temp18=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp18[ 0]= flag; _temp18;}); _temp17->tl= Cyc_ccargs; _temp17;}); _temp16;});}
static struct Cyc_List_List* Cyc_libargs= 0; static void Cyc_add_libarg( struct
_tagged_string s){ Cyc_libargs=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp20->hd=( void*)({ struct
_tagged_string* _temp21=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp21[ 0]= s; _temp21;}); _temp20->tl= Cyc_libargs; _temp20;});}
static void Cyc_set_save_temps(){ Cyc_save_temps_r= 1; Cyc_add_ccarg(({
unsigned char* _temp22=( unsigned char*)"-save-temps"; struct _tagged_string
_temp23; _temp23.curr= _temp22; _temp23.base= _temp22; _temp23.last_plus_one=
_temp22 + 12; _temp23;}));} static void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r=
1; Cyc_add_cpparg(({ unsigned char* _temp24=( unsigned char*)"-M"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}));} static void Cyc_set_stop_after_objectfile(){ Cyc_stop_after_objectfile_r=
1; Cyc_add_ccarg(({ unsigned char* _temp26=( unsigned char*)"-c"; struct
_tagged_string _temp27; _temp27.curr= _temp26; _temp27.base= _temp26; _temp27.last_plus_one=
_temp26 + 3; _temp27;}));} static void Cyc_set_stop_after_asmfile(){ Cyc_stop_after_asmfile_r=
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
unsigned char* _temp87=( unsigned char*)""; struct _tagged_string _temp88;
_temp88.curr= _temp87; _temp88.base= _temp87; _temp88.last_plus_one= _temp87 + 1;
_temp88;}); struct _tagged_string explain_string=({ unsigned char* _temp85=(
unsigned char*)""; struct _tagged_string _temp86; _temp86.curr= _temp85; _temp86.base=
_temp85; _temp86.last_plus_one= _temp85 + 1; _temp86;}); int other_exn= 0; void*
ex=( void*)({ struct Cyc_Core_Impossible_struct* _temp81=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp81[ 0]=({ struct
Cyc_Core_Impossible_struct _temp82; _temp82.tag= Cyc_Core_Impossible; _temp82.f1=({
unsigned char* _temp83=( unsigned char*)""; struct _tagged_string _temp84;
_temp84.curr= _temp83; _temp84.base= _temp83; _temp84.last_plus_one= _temp83 + 1;
_temp84;}); _temp82;}); _temp81;}); struct Cyc_List_List* _temp51= 0; goto _LL52;
_LL52:{ struct _handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0;
if( setjmp( _temp53.handler)){ _temp55= 1;} if( ! _temp55){ _temp51= f( env, tds);;
_pop_handler();} else{ void* _temp54=( void*) _exn_thrown; void* _temp57=
_temp54; struct _tagged_string _temp67; _LL59: if(*(( void**) _temp57) == Cyc_Core_Impossible){
_LL68: _temp67=(( struct Cyc_Core_Impossible_struct*) _temp57)->f1; goto _LL60;}
else{ goto _LL61;} _LL61: if( _temp57 == Cyc_Dict_Absent){ goto _LL62;} else{
goto _LL63;} _LL63: goto _LL64; _LL65: goto _LL66; _LL60: exn_string=({
unsigned char* _temp69=( unsigned char*)"Exception Core::Impossible"; struct
_tagged_string _temp70; _temp70.curr= _temp69; _temp70.base= _temp69; _temp70.last_plus_one=
_temp69 + 27; _temp70;}); explain_string= _temp67; goto _LL58; _LL62: exn_string=({
unsigned char* _temp71=( unsigned char*)"Exception Dict::Absent"; struct
_tagged_string _temp72; _temp72.curr= _temp71; _temp72.base= _temp71; _temp72.last_plus_one=
_temp71 + 23; _temp72;}); goto _LL58; _LL64: ex= _temp57; other_exn= 1;
exn_string=({ unsigned char* _temp73=( unsigned char*)"Uncaught exception";
struct _tagged_string _temp74; _temp74.curr= _temp73; _temp74.base= _temp73;
_temp74.last_plus_one= _temp73 + 19; _temp74;}); goto _LL58; _LL66:( void)
_throw( _temp57); _LL58:;}}} if( Cyc_Position_error_p()){ Cyc_compile_failure= 1;}
if( Cyc_String_strcmp( exn_string,({ unsigned char* _temp75=( unsigned char*)"";
struct _tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75;
_temp76.last_plus_one= _temp75 + 1; _temp76;})) != 0){ Cyc_compile_failure= 1;({
struct _tagged_string _temp77= exn_string; struct _tagged_string _temp78=
stage_name; struct _tagged_string _temp79= explain_string; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\n%.*s thrown during %.*s: %.*s", _temp77.last_plus_one
- _temp77.curr, _temp77.curr, _temp78.last_plus_one - _temp78.curr, _temp78.curr,
_temp79.last_plus_one - _temp79.curr, _temp79.curr);});} if( Cyc_compile_failure){
fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nCOMPILATION FAILED!\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); if( other_exn){( void) _throw( ex);}
return _temp51;} else{ if( Cyc_v_r){({ struct _tagged_string _temp80= stage_name;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s completed.\n", _temp80.last_plus_one
- _temp80.curr, _temp80.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return _temp51;}} return _temp51;} struct Cyc_List_List* Cyc_do_parse(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp89= Cyc_Parse_parse_file( f); goto _LL90; _LL90: Cyc_Lex_lex_init();
return _temp89;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= ! Cyc_noexpand_r;
Cyc_add_cyc_namespace_r= Cyc_add_cyc_namespace_r? ! Cyc_tc_r: 0; if( Cyc_add_cyc_namespace_r){
Cyc_Absyn_file_typ();} else{ Cyc_Absyn_file_typ_nocyc();} Cyc_Tc_tc( te, Cyc_add_cyc_namespace_r,
1, tds); if( ! Cyc_noshake_r){ tds= Cyc_Tc_treeshake( te, tds);} return tds;}
struct Cyc_List_List* Cyc_do_cfcheck( int ignore, struct Cyc_List_List* tds){
Cyc_NewControlFlow_cf_check( tds); return tds;} struct _tuple9{ struct Cyc_Tcenv_Tenv*
f1; struct Cyc_Stdio___sFILE* f2; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple9* params, struct Cyc_List_List* tds){ struct Cyc_Tcenv_Tenv*
_temp91=(* params).f1; goto _LL92; _LL92: { struct Cyc_Stdio___sFILE* _temp93=(*
params).f2; goto _LL94; _LL94: { struct Cyc_Interface_I* _temp95= Cyc_Interface_extract(
_temp91->ae); goto _LL96; _LL96: Cyc_Interface_print( _temp95, _temp93); return
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
_temp97={ 0, 0, 0}; _temp97;})).curr){ return 0;}{ struct Cyc_List_List* _temp98=
0; goto _LL99; _LL99: { unsigned int _temp100= Cyc_String_strlen( s); goto
_LL101; _LL101: while( _temp100 > 0) { struct _tagged_string _temp102= Cyc_String_strchr(
s, c); goto _LL103; _LL103: if( _temp102.curr ==(( struct _tagged_string)({
struct _tagged_string _temp104={ 0, 0, 0}; _temp104;})).curr){ _temp98=({ struct
Cyc_List_List* _temp105=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp105->hd=( void*)({ struct _tagged_string* _temp106=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp106[ 0]= s; _temp106;});
_temp105->tl= _temp98; _temp105;}); break;} else{ _temp98=({ struct Cyc_List_List*
_temp107=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp107->hd=( void*)({ struct _tagged_string* _temp108=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp108[ 0]=( struct _tagged_string)
Cyc_String_substring( s, 0,( unsigned int)((( struct _tagged_string) _temp102).curr
- s.curr)); _temp108;}); _temp107->tl= _temp98; _temp107;}); _temp100 -=((
struct _tagged_string) _temp102).curr - s.curr; s=({ struct _tagged_string
_temp109= _temp102; _temp109.curr += 1; _temp109;});}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp98);}}} int Cyc_file_exists(
struct _tagged_string file){ struct Cyc_Stdio___sFILE* f= 0;{ struct
_handler_cons _temp110; _push_handler(& _temp110);{ int _temp112= 0; if( setjmp(
_temp110.handler)){ _temp112= 1;} if( ! _temp112){ f=( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_file_open( file,({ unsigned char* _temp113=( unsigned char*)"r";
struct _tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 2; _temp114;}));; _pop_handler();} else{ void*
_temp111=( void*) _exn_thrown; void* _temp116= _temp111; _LL118: goto _LL119;
_LL120: goto _LL121; _LL119: goto _LL117; _LL121:( void) _throw( _temp116);
_LL117:;}}} if( f == 0){ return 0;} else{ Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f)); return 1;}} struct _tagged_string* Cyc_find( struct Cyc_List_List*
dirs, struct _tagged_string file){ if( file.curr ==(( struct _tagged_string)({
struct _tagged_string _temp122={ 0, 0, 0}; _temp122;})).curr){ return 0;} for( 0;
dirs != 0; dirs=(( struct Cyc_List_List*) _check_null( dirs))->tl){ struct
_tagged_string _temp123=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( dirs))->hd); goto _LL124; _LL124: if( _temp123.curr ==(( struct
_tagged_string)({ struct _tagged_string _temp125={ 0, 0, 0}; _temp125;})).curr?
1: Cyc_String_strlen( _temp123) == 0){ continue;}{ struct _tagged_string s=(
struct _tagged_string) Cyc_Filename_concat( _temp123, file); if( Cyc_file_exists(
s)){ return({ struct _tagged_string* _temp126=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp126[ 0]= s; _temp126;});}}}
return 0;} void Cyc_process_file( struct _tagged_string filename){ struct
_tagged_string _temp127= Cyc_Filename_chop_extension( filename); goto _LL128;
_LL128: { struct _tagged_string _temp131= Cyc_String_strconcat(( struct
_tagged_string) _temp127,({ unsigned char* _temp129=( unsigned char*)".cyp";
struct _tagged_string _temp130; _temp130.curr= _temp129; _temp130.base= _temp129;
_temp130.last_plus_one= _temp129 + 5; _temp130;})); goto _LL132; _LL132: {
struct _tagged_string _temp135= Cyc_String_strconcat(( struct _tagged_string)
_temp127,({ unsigned char* _temp133=( unsigned char*)".cyci"; struct
_tagged_string _temp134; _temp134.curr= _temp133; _temp134.base= _temp133;
_temp134.last_plus_one= _temp133 + 6; _temp134;})); goto _LL136; _LL136: {
struct _tagged_string _temp139= Cyc_String_strconcat(( struct _tagged_string)
_temp127,({ unsigned char* _temp137=( unsigned char*)".c"; struct _tagged_string
_temp138; _temp138.curr= _temp137; _temp138.base= _temp137; _temp138.last_plus_one=
_temp137 + 3; _temp138;})); goto _LL140; _LL140: if( Cyc_v_r){({ struct
_tagged_string _temp141= filename; fprintf( sfile_to_file( Cyc_Stdio_stderr),"Compiling %.*s\n",
_temp141.last_plus_one - _temp141.curr, _temp141.curr);});}{ struct Cyc_Stdio___sFILE*
f0= Cyc_try_file_open( filename,({ unsigned char* _temp241=( unsigned char*)"r";
struct _tagged_string _temp242; _temp242.curr= _temp241; _temp242.base= _temp241;
_temp242.last_plus_one= _temp241 + 2; _temp242;}),({ unsigned char* _temp243=(
unsigned char*)"input file"; struct _tagged_string _temp244; _temp244.curr=
_temp243; _temp244.base= _temp243; _temp244.last_plus_one= _temp243 + 11;
_temp244;})); if( Cyc_compile_failure){ return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f0));{ struct _tagged_string _temp148= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp142=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp142->hd=( void*)({ struct _tagged_string* _temp143=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp143[ 0]=({
unsigned char* _temp144=( unsigned char*)""; struct _tagged_string _temp145;
_temp145.curr= _temp144; _temp145.base= _temp144; _temp145.last_plus_one=
_temp144 + 1; _temp145;}); _temp143;}); _temp142->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs); _temp142;}),({
unsigned char* _temp146=( unsigned char*)" "; struct _tagged_string _temp147;
_temp147.curr= _temp146; _temp147.base= _temp146; _temp147.last_plus_one=
_temp146 + 2; _temp147;})); goto _LL149; _LL149: { struct _tagged_string
stdinc_string=({ unsigned char* _temp239=( unsigned char*)" -Ic:/cyclone/include";
struct _tagged_string _temp240; _temp240.curr= _temp239; _temp240.base= _temp239;
_temp240.last_plus_one= _temp239 + 22; _temp240;}); struct _tagged_string
_temp152= Cyc_Stdlib_getenv(({ unsigned char* _temp150=( unsigned char*)"CYCLONE_INCLUDE_PATH";
struct _tagged_string _temp151; _temp151.curr= _temp150; _temp151.base= _temp150;
_temp151.last_plus_one= _temp150 + 21; _temp151;})); goto _LL153; _LL153: if(
_temp152.curr !=({ struct _tagged_string _temp154={ 0, 0, 0}; _temp154;}).curr){
struct Cyc_List_List* _temp155= Cyc_split_by_char(( struct _tagged_string)
_temp152,':'); goto _LL156; _LL156: { struct _tagged_string _temp163= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp157=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp157->hd=( void*)({ struct _tagged_string* _temp158=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp158[ 0]=({
unsigned char* _temp159=( unsigned char*)""; struct _tagged_string _temp160;
_temp160.curr= _temp159; _temp160.base= _temp159; _temp160.last_plus_one=
_temp159 + 1; _temp160;}); _temp158;}); _temp157->tl= _temp155; _temp157;}),({
unsigned char* _temp161=( unsigned char*)" -I"; struct _tagged_string _temp162;
_temp162.curr= _temp161; _temp162.base= _temp161; _temp162.last_plus_one=
_temp161 + 4; _temp162;})); goto _LL164; _LL164: stdinc_string=( struct
_tagged_string)({ struct _tagged_string _temp165=( struct _tagged_string)
_temp163; struct _tagged_string _temp166= stdinc_string; xprintf("%.*s%.*s",
_temp165.last_plus_one - _temp165.curr, _temp165.curr, _temp166.last_plus_one -
_temp166.curr, _temp166.curr);});}}{ struct _tagged_string ofile_string; if( Cyc_stop_after_cpp_r){
if( Cyc_output_file != 0){ ofile_string=( struct _tagged_string)({ struct
_tagged_string _temp167=*(( struct _tagged_string*) _check_null( Cyc_output_file));
xprintf(" -o %.*s", _temp167.last_plus_one - _temp167.curr, _temp167.curr);});}
else{ ofile_string=({ unsigned char* _temp168=( unsigned char*)""; struct
_tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 1; _temp169;});}} else{ ofile_string=( struct
_tagged_string)({ struct _tagged_string _temp170=( struct _tagged_string)
_temp131; xprintf(" -o %.*s", _temp170.last_plus_one - _temp170.curr, _temp170.curr);});}{
struct _tagged_string _temp175=({ struct _tagged_string _temp171=( struct
_tagged_string) _temp148; struct _tagged_string _temp172= stdinc_string; struct
_tagged_string _temp173= ofile_string; struct _tagged_string _temp174= filename;
xprintf("gcc -x c -E -U__GNUC__ -nostdinc%.*s%.*s%.*s %.*s", _temp171.last_plus_one
- _temp171.curr, _temp171.curr, _temp172.last_plus_one - _temp172.curr, _temp172.curr,
_temp173.last_plus_one - _temp173.curr, _temp173.curr, _temp174.last_plus_one -
_temp174.curr, _temp174.curr);}); goto _LL176; _LL176: if( Cyc_v_r){({ struct
_tagged_string _temp177=( struct _tagged_string) _temp175; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp177.last_plus_one - _temp177.curr,
_temp177.curr);});} if( system( string_to_Cstring(( struct _tagged_string)
_temp175)) != 0){ Cyc_compile_failure= 1; fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nError: preprocessing\n");
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_string) _temp131); Cyc_Absynpp_print_using_stmts=( Cyc_Absynpp_print_externC_stmts=
1);{ struct Cyc_Stdio___sFILE* in_file= Cyc_try_file_open(( struct
_tagged_string) _temp131,({ unsigned char* _temp235=( unsigned char*)"r"; struct
_tagged_string _temp236; _temp236.curr= _temp235; _temp236.base= _temp235;
_temp236.last_plus_one= _temp235 + 2; _temp236;}),({ unsigned char* _temp237=(
unsigned char*)"file"; struct _tagged_string _temp238; _temp238.curr= _temp237;
_temp238.base= _temp237; _temp238.last_plus_one= _temp237 + 5; _temp238;})); if(
Cyc_compile_failure){ return;}{ struct Cyc_List_List* tds= 0;{ struct
_handler_cons _temp178; _push_handler(& _temp178);{ int _temp180= 0; if( setjmp(
_temp178.handler)){ _temp180= 1;} if( ! _temp180){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp181=( unsigned char*)"parsing"; struct
_tagged_string _temp182; _temp182.curr= _temp181; _temp182.base= _temp181;
_temp182.last_plus_one= _temp181 + 8; _temp182;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)
_check_null( in_file), tds);; _pop_handler();} else{ void* _temp179=( void*)
_exn_thrown; void* _temp184= _temp179; _LL186: goto _LL187; _LL188: goto _LL189;
_LL187: Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*) _check_null( in_file));
Cyc_remove_file(( struct _tagged_string) _temp131);( void) _throw( _temp184);
_LL189:( void) _throw( _temp184); _LL185:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( in_file)); if( Cyc_compile_failure){ Cyc_remove_file(( struct
_tagged_string) _temp131); return;}{ struct Cyc_Tcenv_Tenv* _temp190= Cyc_Tcenv_tc_init();
goto _LL191; _LL191: if( Cyc_parseonly_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp192=( unsigned char*)"type checking";
struct _tagged_string _temp193; _temp193.curr= _temp192; _temp193.base= _temp192;
_temp193.last_plus_one= _temp192 + 14; _temp193;}), Cyc_do_typecheck, _temp190,
tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_string)
_temp131); return;} tds=(( struct Cyc_List_List*(*)( struct _tagged_string
stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env,
struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp194=(
unsigned char*)"control-flow checking"; struct _tagged_string _temp195; _temp195.curr=
_temp194; _temp195.base= _temp194; _temp195.last_plus_one= _temp194 + 22;
_temp195;}), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();}
Cyc_remove_file(( struct _tagged_string) _temp131); if( Cyc_compile_failure){
return;} Cyc_Absynpp_print_using_stmts=( Cyc_Absynpp_print_externC_stmts= 0);
if( Cyc_ic_r){ struct Cyc_Stdio___sFILE* inter_file; inter_file= Cyc_try_file_open((
struct _tagged_string) _temp135,({ unsigned char* _temp196=( unsigned char*)"w";
struct _tagged_string _temp197; _temp197.curr= _temp196; _temp197.base= _temp196;
_temp197.last_plus_one= _temp196 + 2; _temp197;}),({ unsigned char* _temp198=(
unsigned char*)"interface file"; struct _tagged_string _temp199; _temp199.curr=
_temp198; _temp199.base= _temp198; _temp199.last_plus_one= _temp198 + 15;
_temp199;})); if( Cyc_compile_failure){ return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct _tuple9*,
struct Cyc_List_List*), struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)(({
unsigned char* _temp200=( unsigned char*)"interface checking"; struct
_tagged_string _temp201; _temp201.curr= _temp200; _temp201.base= _temp200;
_temp201.last_plus_one= _temp200 + 19; _temp201;}), Cyc_do_interface,({ struct
_tuple9* _temp202=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp202->f1= _temp190; _temp202->f2=( struct Cyc_Stdio___sFILE*) _check_null(
inter_file); _temp202;}), tds); if( inter_file != 0){ Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_file));} if( Cyc_compile_failure){
return;}} if( Cyc_tc_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp203=(
unsigned char*)"translation to C"; struct _tagged_string _temp204; _temp204.curr=
_temp203; _temp204.base= _temp203; _temp204.last_plus_one= _temp203 + 17;
_temp204;}), Cyc_do_translate, 1, tds); if( Cyc_compile_failure){ return;} if(
Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds)) Cyc_do_stage)(({ unsigned char* _temp205=( unsigned char*)"post-pass to VC";
struct _tagged_string _temp206; _temp206.curr= _temp205; _temp206.base= _temp205;
_temp206.last_plus_one= _temp205 + 16; _temp206;}), Cyc_do_tovc, 1, tds);} if(
Cyc_compile_failure){ return;} PRINTC: if( tds != 0){ struct Cyc_Stdio___sFILE*
out_file; if( Cyc_parseonly_r? 1: Cyc_tc_r){ if( Cyc_output_file != 0){ out_file=
Cyc_try_file_open(*(( struct _tagged_string*) _check_null( Cyc_output_file)),({
unsigned char* _temp207=( unsigned char*)"w"; struct _tagged_string _temp208;
_temp208.curr= _temp207; _temp208.base= _temp207; _temp208.last_plus_one=
_temp207 + 2; _temp208;}),({ unsigned char* _temp209=( unsigned char*)"output file";
struct _tagged_string _temp210; _temp210.curr= _temp209; _temp210.base= _temp209;
_temp210.last_plus_one= _temp209 + 12; _temp210;}));} else{ out_file=( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;}} else{ if( Cyc_toc_r? Cyc_output_file !=
0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_string*) _check_null( Cyc_output_file)),({
unsigned char* _temp211=( unsigned char*)"w"; struct _tagged_string _temp212;
_temp212.curr= _temp211; _temp212.base= _temp211; _temp212.last_plus_one=
_temp211 + 2; _temp212;}),({ unsigned char* _temp213=( unsigned char*)"output file";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 12; _temp214;}));} else{ out_file= Cyc_try_file_open((
struct _tagged_string) _temp139,({ unsigned char* _temp215=( unsigned char*)"w";
struct _tagged_string _temp216; _temp216.curr= _temp215; _temp216.base= _temp215;
_temp216.last_plus_one= _temp215 + 2; _temp216;}),({ unsigned char* _temp217=(
unsigned char*)"output file"; struct _tagged_string _temp218; _temp218.curr=
_temp217; _temp218.base= _temp217; _temp218.last_plus_one= _temp217 + 12;
_temp218;}));}} if( Cyc_compile_failure){ return;}{ struct _handler_cons
_temp219; _push_handler(& _temp219);{ int _temp221= 0; if( setjmp( _temp219.handler)){
_temp221= 1;} if( ! _temp221){ tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp222=( unsigned char*)"printing"; struct
_tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 9; _temp223;}), Cyc_do_print,( struct Cyc_Stdio___sFILE*)
_check_null( out_file), tds);; _pop_handler();} else{ void* _temp220=( void*)
_exn_thrown; void* _temp225= _temp220; _LL227: goto _LL228; _LL229: goto _LL230;
_LL228: Cyc_compile_failure= 1; Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp231=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp231->hd=( void*)({
struct _tagged_string* _temp232=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp232[ 0]=( struct _tagged_string) _temp139;
_temp232;}); _temp231->tl= Cyc_cfiles; _temp231;});( void) _throw( _temp225);
_LL230:( void) _throw( _temp225); _LL226:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp233=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp233->hd=( void*)({
struct _tagged_string* _temp234=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp234[ 0]=( struct _tagged_string) _temp139;
_temp234;}); _temp233->tl= Cyc_cfiles; _temp233;});}}}}}}}}}}}}} struct _tuple10{
struct _tagged_string f1; void* f2; struct _tagged_string f3; } ; struct
_tagged_ptr3{ struct _tuple10** curr; struct _tuple10** base; struct _tuple10**
last_plus_one; } ; int Cyc_main( int argc, struct _tagged_ptr0 argv){ struct Cyc_List_List*
options=({ struct Cyc_List_List*(* _temp297)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple10* _temp300=({ struct
_tuple10* _temp549=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp549->f1=({ unsigned char* _temp554=( unsigned char*)"-v"; struct
_tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 3; _temp555;}); _temp549->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp552=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp552[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp553; _temp553.tag= Cyc_Arg_Set_spec; _temp553.f1=& Cyc_v_r; _temp553;});
_temp552;}); _temp549->f3=({ unsigned char* _temp550=( unsigned char*)"    Print compilation stages verbosely";
struct _tagged_string _temp551; _temp551.curr= _temp550; _temp551.base= _temp550;
_temp551.last_plus_one= _temp550 + 39; _temp551;}); _temp549;}); struct _tuple10*
_temp301=({ struct _tuple10* _temp542=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp542->f1=({ unsigned char* _temp547=( unsigned char*)"-o";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 3; _temp548;}); _temp542->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp545=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp545[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp546; _temp546.tag= Cyc_Arg_String_spec; _temp546.f1=
Cyc_set_output_file; _temp546;}); _temp545;}); _temp542->f3=({ unsigned char*
_temp543=( unsigned char*)"    Output file name"; struct _tagged_string _temp544;
_temp544.curr= _temp543; _temp544.base= _temp543; _temp544.last_plus_one=
_temp543 + 21; _temp544;}); _temp542;}); struct _tuple10* _temp302=({ struct
_tuple10* _temp535=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp535->f1=({ unsigned char* _temp540=( unsigned char*)"-D*"; struct
_tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 4; _temp541;}); _temp535->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp538=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp538[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp539; _temp539.tag= Cyc_Arg_Flag_spec; _temp539.f1= Cyc_add_cpparg; _temp539;});
_temp538;}); _temp535->f3=({ unsigned char* _temp536=( unsigned char*)"   Pass definition to preprocessor";
struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 35; _temp537;}); _temp535;}); struct _tuple10*
_temp303=({ struct _tuple10* _temp528=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp528->f1=({ unsigned char* _temp533=( unsigned char*)"-B*";
struct _tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 4; _temp534;}); _temp528->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp531=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp531[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp532; _temp532.tag= Cyc_Arg_Flag_spec; _temp532.f1= Cyc_add_cyclone_exec_prefix_path;
_temp532;}); _temp531;}); _temp528->f3=({ unsigned char* _temp529=(
unsigned char*)"   Add to the list of directories to search for compiler files";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 63; _temp530;}); _temp528;}); struct _tuple10*
_temp304=({ struct _tuple10* _temp521=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp521->f1=({ unsigned char* _temp526=( unsigned char*)"-I*";
struct _tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 4; _temp527;}); _temp521->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp524=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp524[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp525; _temp525.tag= Cyc_Arg_Flag_spec; _temp525.f1= Cyc_add_cpparg; _temp525;});
_temp524;}); _temp521->f3=({ unsigned char* _temp522=( unsigned char*)"   Add to the list of directories to search for include files";
struct _tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 62; _temp523;}); _temp521;}); struct _tuple10*
_temp305=({ struct _tuple10* _temp514=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp514->f1=({ unsigned char* _temp519=( unsigned char*)"-L*";
struct _tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 4; _temp520;}); _temp514->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp517=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp517[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp518; _temp518.tag= Cyc_Arg_Flag_spec; _temp518.f1= Cyc_add_cpparg; _temp518;});
_temp517;}); _temp514->f3=({ unsigned char* _temp515=( unsigned char*)"   Add to the list of directories for -l";
struct _tagged_string _temp516; _temp516.curr= _temp515; _temp516.base= _temp515;
_temp516.last_plus_one= _temp515 + 41; _temp516;}); _temp514;}); struct _tuple10*
_temp306=({ struct _tuple10* _temp507=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp507->f1=({ unsigned char* _temp512=( unsigned char*)"-l*";
struct _tagged_string _temp513; _temp513.curr= _temp512; _temp513.base= _temp512;
_temp513.last_plus_one= _temp512 + 4; _temp513;}); _temp507->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp510=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp510[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp511; _temp511.tag= Cyc_Arg_Flag_spec; _temp511.f1= Cyc_add_libarg; _temp511;});
_temp510;}); _temp507->f3=({ unsigned char* _temp508=( unsigned char*)"   Library file";
struct _tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 16; _temp509;}); _temp507;}); struct _tuple10*
_temp307=({ struct _tuple10* _temp500=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp500->f1=({ unsigned char* _temp505=( unsigned char*)"-c";
struct _tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 3; _temp506;}); _temp500->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp503=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp503[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp504; _temp504.tag= Cyc_Arg_Unit_spec; _temp504.f1= Cyc_set_stop_after_objectfile;
_temp504;}); _temp503;}); _temp500->f3=({ unsigned char* _temp501=(
unsigned char*)"    Produce object file"; struct _tagged_string _temp502;
_temp502.curr= _temp501; _temp502.base= _temp501; _temp502.last_plus_one=
_temp501 + 24; _temp502;}); _temp500;}); struct _tuple10* _temp308=({ struct
_tuple10* _temp493=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp493->f1=({ unsigned char* _temp498=( unsigned char*)"-O"; struct
_tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 3; _temp499;}); _temp493->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp496=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp496[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp497; _temp497.tag= Cyc_Arg_Flag_spec; _temp497.f1= Cyc_add_ccarg; _temp497;});
_temp496;}); _temp493->f3=({ unsigned char* _temp494=( unsigned char*)"    Optimize";
struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 13; _temp495;}); _temp493;}); struct _tuple10*
_temp309=({ struct _tuple10* _temp486=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp486->f1=({ unsigned char* _temp491=( unsigned char*)"-O2";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 4; _temp492;}); _temp486->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp489=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp489[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp490; _temp490.tag= Cyc_Arg_Flag_spec; _temp490.f1= Cyc_add_ccarg; _temp490;});
_temp489;}); _temp486->f3=({ unsigned char* _temp487=( unsigned char*)"   Optimize";
struct _tagged_string _temp488; _temp488.curr= _temp487; _temp488.base= _temp487;
_temp488.last_plus_one= _temp487 + 12; _temp488;}); _temp486;}); struct _tuple10*
_temp310=({ struct _tuple10* _temp479=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp479->f1=({ unsigned char* _temp484=( unsigned char*)"-O3";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 4; _temp485;}); _temp479->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp482=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp482[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp483; _temp483.tag= Cyc_Arg_Flag_spec; _temp483.f1= Cyc_add_ccarg; _temp483;});
_temp482;}); _temp479->f3=({ unsigned char* _temp480=( unsigned char*)"   Optimize";
struct _tagged_string _temp481; _temp481.curr= _temp480; _temp481.base= _temp480;
_temp481.last_plus_one= _temp480 + 12; _temp481;}); _temp479;}); struct _tuple10*
_temp311=({ struct _tuple10* _temp472=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp472->f1=({ unsigned char* _temp477=( unsigned char*)"-g";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 3; _temp478;}); _temp472->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp475=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp475[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp476; _temp476.tag= Cyc_Arg_Flag_spec; _temp476.f1= Cyc_add_ccarg; _temp476;});
_temp475;}); _temp472->f3=({ unsigned char* _temp473=( unsigned char*)"    Compile for debugging";
struct _tagged_string _temp474; _temp474.curr= _temp473; _temp474.base= _temp473;
_temp474.last_plus_one= _temp473 + 26; _temp474;}); _temp472;}); struct _tuple10*
_temp312=({ struct _tuple10* _temp465=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp465->f1=({ unsigned char* _temp470=( unsigned char*)"-p";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 3; _temp471;}); _temp465->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp468=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp468[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp469; _temp469.tag= Cyc_Arg_Flag_spec; _temp469.f1= Cyc_add_ccarg; _temp469;});
_temp468;}); _temp465->f3=({ unsigned char* _temp466=( unsigned char*)"    Compile for prof";
struct _tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 21; _temp467;}); _temp465;}); struct _tuple10*
_temp313=({ struct _tuple10* _temp458=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp458->f1=({ unsigned char* _temp463=( unsigned char*)"-pg";
struct _tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 4; _temp464;}); _temp458->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp461=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp461[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp462; _temp462.tag= Cyc_Arg_Flag_spec; _temp462.f1= Cyc_add_ccarg; _temp462;});
_temp461;}); _temp458->f3=({ unsigned char* _temp459=( unsigned char*)"   Compile for gprof";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 21; _temp460;}); _temp458;}); struct _tuple10*
_temp314=({ struct _tuple10* _temp451=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp451->f1=({ unsigned char* _temp456=( unsigned char*)"-S";
struct _tagged_string _temp457; _temp457.curr= _temp456; _temp457.base= _temp456;
_temp457.last_plus_one= _temp456 + 3; _temp457;}); _temp451->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp454=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp454[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp455; _temp455.tag= Cyc_Arg_Unit_spec; _temp455.f1= Cyc_set_stop_after_asmfile;
_temp455;}); _temp454;}); _temp451->f3=({ unsigned char* _temp452=(
unsigned char*)"    Stop after producing assembly code"; struct _tagged_string
_temp453; _temp453.curr= _temp452; _temp453.base= _temp452; _temp453.last_plus_one=
_temp452 + 39; _temp453;}); _temp451;}); struct _tuple10* _temp315=({ struct
_tuple10* _temp444=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp444->f1=({ unsigned char* _temp449=( unsigned char*)"-M"; struct
_tagged_string _temp450; _temp450.curr= _temp449; _temp450.base= _temp449;
_temp450.last_plus_one= _temp449 + 3; _temp450;}); _temp444->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp447=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp447[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp448; _temp448.tag= Cyc_Arg_Unit_spec; _temp448.f1= Cyc_set_produce_dependencies;
_temp448;}); _temp447;}); _temp444->f3=({ unsigned char* _temp445=(
unsigned char*)"    Produce dependencies"; struct _tagged_string _temp446;
_temp446.curr= _temp445; _temp446.base= _temp445; _temp446.last_plus_one=
_temp445 + 25; _temp446;}); _temp444;}); struct _tuple10* _temp316=({ struct
_tuple10* _temp437=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp437->f1=({ unsigned char* _temp442=( unsigned char*)"-E"; struct
_tagged_string _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 3; _temp443;}); _temp437->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp440=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp440[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp441; _temp441.tag= Cyc_Arg_Set_spec; _temp441.f1=& Cyc_stop_after_cpp_r;
_temp441;}); _temp440;}); _temp437->f3=({ unsigned char* _temp438=(
unsigned char*)"    Stop after preprocessing"; struct _tagged_string _temp439;
_temp439.curr= _temp438; _temp439.base= _temp438; _temp439.last_plus_one=
_temp438 + 29; _temp439;}); _temp437;}); struct _tuple10* _temp317=({ struct
_tuple10* _temp430=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp430->f1=({ unsigned char* _temp435=( unsigned char*)"-parseonly"; struct
_tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 11; _temp436;}); _temp430->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp433=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp433[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp434; _temp434.tag= Cyc_Arg_Set_spec; _temp434.f1=& Cyc_parseonly_r;
_temp434;}); _temp433;}); _temp430->f3=({ unsigned char* _temp431=(
unsigned char*)"\n        Stop after parsing"; struct _tagged_string _temp432;
_temp432.curr= _temp431; _temp432.base= _temp431; _temp432.last_plus_one=
_temp431 + 28; _temp432;}); _temp430;}); struct _tuple10* _temp318=({ struct
_tuple10* _temp423=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp423->f1=({ unsigned char* _temp428=( unsigned char*)"-tc"; struct
_tagged_string _temp429; _temp429.curr= _temp428; _temp429.base= _temp428;
_temp429.last_plus_one= _temp428 + 4; _temp429;}); _temp423->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp426=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp426[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp427; _temp427.tag= Cyc_Arg_Set_spec; _temp427.f1=& Cyc_tc_r; _temp427;});
_temp426;}); _temp423->f3=({ unsigned char* _temp424=( unsigned char*)"   Stop after type checking";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 28; _temp425;}); _temp423;}); struct _tuple10*
_temp319=({ struct _tuple10* _temp416=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp416->f1=({ unsigned char* _temp421=( unsigned char*)"-toc";
struct _tagged_string _temp422; _temp422.curr= _temp421; _temp422.base= _temp421;
_temp422.last_plus_one= _temp421 + 5; _temp422;}); _temp416->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp419=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp419[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp420; _temp420.tag= Cyc_Arg_Set_spec; _temp420.f1=& Cyc_toc_r; _temp420;});
_temp419;}); _temp416->f3=({ unsigned char* _temp417=( unsigned char*)"  Stop after translation to C";
struct _tagged_string _temp418; _temp418.curr= _temp417; _temp418.base= _temp417;
_temp418.last_plus_one= _temp417 + 30; _temp418;}); _temp416;}); struct _tuple10*
_temp320=({ struct _tuple10* _temp409=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp409->f1=({ unsigned char* _temp414=( unsigned char*)"-ic";
struct _tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 4; _temp415;}); _temp409->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp412=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp412[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp413; _temp413.tag= Cyc_Arg_Set_spec; _temp413.f1=& Cyc_ic_r; _temp413;});
_temp412;}); _temp409->f3=({ unsigned char* _temp410=( unsigned char*)"   Interface check";
struct _tagged_string _temp411; _temp411.curr= _temp410; _temp411.base= _temp410;
_temp411.last_plus_one= _temp410 + 19; _temp411;}); _temp409;}); struct _tuple10*
_temp321=({ struct _tuple10* _temp402=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp402->f1=({ unsigned char* _temp407=( unsigned char*)"-pp";
struct _tagged_string _temp408; _temp408.curr= _temp407; _temp408.base= _temp407;
_temp408.last_plus_one= _temp407 + 4; _temp408;}); _temp402->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp405=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp405[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp406; _temp406.tag= Cyc_Arg_Set_spec; _temp406.f1=& Cyc_pp_r; _temp406;});
_temp405;}); _temp402->f3=({ unsigned char* _temp403=( unsigned char*)"   Pretty print";
struct _tagged_string _temp404; _temp404.curr= _temp403; _temp404.base= _temp403;
_temp404.last_plus_one= _temp403 + 16; _temp404;}); _temp402;}); struct _tuple10*
_temp322=({ struct _tuple10* _temp395=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp395->f1=({ unsigned char* _temp400=( unsigned char*)"-up";
struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 4; _temp401;}); _temp395->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp398=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp398[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp399; _temp399.tag= Cyc_Arg_Clear_spec; _temp399.f1=&
Cyc_pp_r; _temp399;}); _temp398;}); _temp395->f3=({ unsigned char* _temp396=(
unsigned char*)"   Ugly print"; struct _tagged_string _temp397; _temp397.curr=
_temp396; _temp397.base= _temp396; _temp397.last_plus_one= _temp396 + 14;
_temp397;}); _temp395;}); struct _tuple10* _temp323=({ struct _tuple10* _temp388=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp388->f1=({
unsigned char* _temp393=( unsigned char*)"-tovc"; struct _tagged_string _temp394;
_temp394.curr= _temp393; _temp394.base= _temp393; _temp394.last_plus_one=
_temp393 + 6; _temp394;}); _temp388->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp391=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp391[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp392; _temp392.tag= Cyc_Arg_Set_spec;
_temp392.f1=& Cyc_tovc_r; _temp392;}); _temp391;}); _temp388->f3=({
unsigned char* _temp389=( unsigned char*)" Avoid gcc extensions in C output";
struct _tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 34; _temp390;}); _temp388;}); struct _tuple10*
_temp324=({ struct _tuple10* _temp381=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp381->f1=({ unsigned char* _temp386=( unsigned char*)"-save-temps";
struct _tagged_string _temp387; _temp387.curr= _temp386; _temp387.base= _temp386;
_temp387.last_plus_one= _temp386 + 12; _temp387;}); _temp381->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp384=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp384[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp385; _temp385.tag= Cyc_Arg_Unit_spec; _temp385.f1= Cyc_set_save_temps;
_temp385;}); _temp384;}); _temp381->f3=({ unsigned char* _temp382=(
unsigned char*)"\n        Don't delete temporary files"; struct _tagged_string
_temp383; _temp383.curr= _temp382; _temp383.base= _temp382; _temp383.last_plus_one=
_temp382 + 38; _temp383;}); _temp381;}); struct _tuple10* _temp325=({ struct
_tuple10* _temp374=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp374->f1=({ unsigned char* _temp379=( unsigned char*)"-save-c"; struct
_tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 8; _temp380;}); _temp374->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp377=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp377[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp378; _temp378.tag= Cyc_Arg_Set_spec; _temp378.f1=& Cyc_save_c_r; _temp378;});
_temp377;}); _temp374->f3=({ unsigned char* _temp375=( unsigned char*)"\n        Don't delete temporary C files";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 40; _temp376;}); _temp374;}); struct _tuple10*
_temp326=({ struct _tuple10* _temp367=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp367->f1=({ unsigned char* _temp372=( unsigned char*)"-nocyc";
struct _tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 7; _temp373;}); _temp367->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp370=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp370[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp371; _temp371.tag= Cyc_Arg_Clear_spec; _temp371.f1=&
Cyc_add_cyc_namespace_r; _temp371;}); _temp370;}); _temp367->f3=({ unsigned char*
_temp368=( unsigned char*)"Don't add implicit namespace Cyc"; struct
_tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 33; _temp369;}); _temp367;}); struct _tuple10*
_temp327=({ struct _tuple10* _temp360=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp360->f1=({ unsigned char* _temp365=( unsigned char*)"-noremoveunused";
struct _tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 16; _temp366;}); _temp360->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp363=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp363[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp364; _temp364.tag= Cyc_Arg_Set_spec; _temp364.f1=& Cyc_noshake_r; _temp364;});
_temp363;}); _temp360->f3=({ unsigned char* _temp361=( unsigned char*)"\n        Don't remove externed variables that aren't used";
struct _tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 58; _temp362;}); _temp360;}); struct _tuple10*
_temp328=({ struct _tuple10* _temp353=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp353->f1=({ unsigned char* _temp358=( unsigned char*)"-noexpandtypedefs";
struct _tagged_string _temp359; _temp359.curr= _temp358; _temp359.base= _temp358;
_temp359.last_plus_one= _temp358 + 18; _temp359;}); _temp353->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp356=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp356[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp357; _temp357.tag= Cyc_Arg_Set_spec; _temp357.f1=& Cyc_noexpand_r; _temp357;});
_temp356;}); _temp353->f3=({ unsigned char* _temp354=( unsigned char*)"\n        Don't expand typedefs in pretty printing";
struct _tagged_string _temp355; _temp355.curr= _temp354; _temp355.base= _temp354;
_temp355.last_plus_one= _temp354 + 50; _temp355;}); _temp353;}); struct _tuple10*
_temp329=({ struct _tuple10* _temp346=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp346->f1=({ unsigned char* _temp351=( unsigned char*)"-printalltvars";
struct _tagged_string _temp352; _temp352.curr= _temp351; _temp352.base= _temp351;
_temp352.last_plus_one= _temp351 + 15; _temp352;}); _temp346->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp349=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp349[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp350; _temp350.tag= Cyc_Arg_Set_spec; _temp350.f1=& Cyc_Absynpp_print_all_tvars;
_temp350;}); _temp349;}); _temp346->f3=({ unsigned char* _temp347=(
unsigned char*)"\n        Print all type variables (even implicit default effects)";
struct _tagged_string _temp348; _temp348.curr= _temp347; _temp348.base= _temp347;
_temp348.last_plus_one= _temp347 + 66; _temp348;}); _temp346;}); struct _tuple10*
_temp330=({ struct _tuple10* _temp339=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp339->f1=({ unsigned char* _temp344=( unsigned char*)"-printallkinds";
struct _tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 15; _temp345;}); _temp339->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp342=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp342[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp343; _temp343.tag= Cyc_Arg_Set_spec; _temp343.f1=& Cyc_Absynpp_print_all_kinds;
_temp343;}); _temp342;}); _temp339->f3=({ unsigned char* _temp340=(
unsigned char*)"\n        Always print kinds of type variables"; struct
_tagged_string _temp341; _temp341.curr= _temp340; _temp341.base= _temp340;
_temp341.last_plus_one= _temp340 + 46; _temp341;}); _temp339;}); struct _tuple10*
_temp331=({ struct _tuple10* _temp332=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp332->f1=({ unsigned char* _temp337=( unsigned char*)"-printfullevars";
struct _tagged_string _temp338; _temp338.curr= _temp337; _temp338.base= _temp337;
_temp338.last_plus_one= _temp337 + 16; _temp338;}); _temp332->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp335=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp335[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp336; _temp336.tag= Cyc_Arg_Set_spec; _temp336.f1=& Cyc_Absynpp_print_full_evars;
_temp336;}); _temp335;}); _temp332->f3=({ unsigned char* _temp333=(
unsigned char*)"\n       Print full information for evars (type debugging)";
struct _tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 58; _temp334;}); _temp332;}); struct _tuple10*
_temp298[ 32u]={ _temp300, _temp301, _temp302, _temp303, _temp304, _temp305,
_temp306, _temp307, _temp308, _temp309, _temp310, _temp311, _temp312, _temp313,
_temp314, _temp315, _temp316, _temp317, _temp318, _temp319, _temp320, _temp321,
_temp322, _temp323, _temp324, _temp325, _temp326, _temp327, _temp328, _temp329,
_temp330, _temp331}; struct _tagged_ptr3 _temp299={ _temp298, _temp298, _temp298
+ 32u}; _temp297( _temp299);}); Cyc_Arg_parse( options, Cyc_add_other,({
unsigned char* _temp245=( unsigned char*)"Options:"; struct _tagged_string
_temp246; _temp246.curr= _temp245; _temp246.base= _temp245; _temp246.last_plus_one=
_temp245 + 9; _temp246;}), argv);{ struct Cyc_List_List* _temp247=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); goto _LL248; _LL248:
for( 0; _temp247 != 0; _temp247=(( struct Cyc_List_List*) _check_null( _temp247))->tl){
Cyc_process_file(*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp247))->hd)); if( Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r?
1: Cyc_parseonly_r)? 1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs == 0){
return 0;}{ struct _tagged_string cyclone_exec_prefix=( struct _tagged_string)
Cyc_Stdlib_getenv(({ unsigned char* _temp295=( unsigned char*)"CYCLONE_EXEC_PREFIX";
struct _tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 20; _temp296;})); if( cyclone_exec_prefix.curr
!=(( struct _tagged_string)({ struct _tagged_string _temp249={ 0, 0, 0};
_temp249;})).curr){ Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List*
_temp250=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp250->hd=( void*)({ struct _tagged_string* _temp251=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp251[ 0]= cyclone_exec_prefix;
_temp251;}); _temp250->tl= Cyc_cyclone_exec_prefix_path; _temp250;});} Cyc_cyclone_exec_prefix_path=({
struct Cyc_List_List* _temp252=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp252->hd=( void*)({ struct _tagged_string* _temp253=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp253[ 0]=({
unsigned char* _temp254=( unsigned char*)"c:/cyclone/bin/cyc-lib"; struct
_tagged_string _temp255; _temp255.curr= _temp254; _temp255.base= _temp254;
_temp255.last_plus_one= _temp254 + 23; _temp255;}); _temp253;}); _temp252->tl=
Cyc_cyclone_exec_prefix_path; _temp252;}); Cyc_cyclone_exec_prefix_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_exec_prefix_path);{
struct _tagged_string* _temp258= Cyc_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp256=( unsigned char*)"include/cyc_include.h"; struct
_tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 22; _temp257;})); goto _LL259; _LL259: if(
_temp258 == 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file cyc_include.h");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;}{ struct _tagged_string
_temp262= Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_ccargs),({ unsigned char* _temp260=( unsigned char*)" ";
struct _tagged_string _temp261; _temp261.curr= _temp260; _temp261.base= _temp260;
_temp261.last_plus_one= _temp260 + 2; _temp261;})); goto _LL263; _LL263: {
struct _tagged_string _temp270= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp264=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp264->hd=( void*)({ struct _tagged_string* _temp265=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp265[ 0]=({ unsigned char*
_temp266=( unsigned char*)""; struct _tagged_string _temp267; _temp267.curr=
_temp266; _temp267.base= _temp266; _temp267.last_plus_one= _temp266 + 1;
_temp267;}); _temp265;}); _temp264->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_libargs); _temp264;}),({ unsigned char* _temp268=(
unsigned char*)" "; struct _tagged_string _temp269; _temp269.curr= _temp268;
_temp269.base= _temp268; _temp269.last_plus_one= _temp268 + 2; _temp269;}));
goto _LL271; _LL271: { struct _tagged_string stdlib_string; if( Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r){ stdlib_string=({ unsigned char* _temp272=(
unsigned char*)""; struct _tagged_string _temp273; _temp273.curr= _temp272;
_temp273.base= _temp272; _temp273.last_plus_one= _temp272 + 1; _temp273;});}
else{ struct _tagged_string* _temp276= Cyc_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp274=( unsigned char*)"cyclib.a"; struct _tagged_string
_temp275; _temp275.curr= _temp274; _temp275.base= _temp274; _temp275.last_plus_one=
_temp274 + 9; _temp275;})); goto _LL277; _LL277: { struct _tagged_string*
_temp280= Cyc_find( Cyc_cyclone_exec_prefix_path,({ unsigned char* _temp278=(
unsigned char*)"gc.a"; struct _tagged_string _temp279; _temp279.curr= _temp278;
_temp279.base= _temp278; _temp279.last_plus_one= _temp278 + 5; _temp279;}));
goto _LL281; _LL281: if( _temp276 == 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file cyclib.a");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} if( _temp280 == 0){
fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file gc.a");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} stdlib_string=( struct
_tagged_string)({ struct _tagged_string _temp282=*(( struct _tagged_string*)
_check_null( _temp276)); struct _tagged_string _temp283=*(( struct
_tagged_string*) _check_null( _temp280)); xprintf(" %.*s %.*s", _temp282.last_plus_one
- _temp282.curr, _temp282.curr, _temp283.last_plus_one - _temp283.curr, _temp283.curr);});}}{
struct _tagged_string _temp292=({ struct _tagged_string _temp287=*(( struct
_tagged_string*) _check_null( _temp258)); struct _tagged_string _temp288= Cyc_output_file
== 0?({ unsigned char* _temp284=( unsigned char*)""; struct _tagged_string
_temp285; _temp285.curr= _temp284; _temp285.base= _temp284; _temp285.last_plus_one=
_temp284 + 1; _temp285;}):( struct _tagged_string)({ struct _tagged_string
_temp286=*(( struct _tagged_string*) _check_null( Cyc_output_file)); xprintf(" -o %.*s",
_temp286.last_plus_one - _temp286.curr, _temp286.curr);}); struct _tagged_string
_temp289=( struct _tagged_string) _temp262; struct _tagged_string _temp290=
stdlib_string; struct _tagged_string _temp291=( struct _tagged_string) _temp270;
xprintf("gcc -include %.*s%.*s %.*s%.*s%.*s", _temp287.last_plus_one - _temp287.curr,
_temp287.curr, _temp288.last_plus_one - _temp288.curr, _temp288.curr, _temp289.last_plus_one
- _temp289.curr, _temp289.curr, _temp290.last_plus_one - _temp290.curr, _temp290.curr,
_temp291.last_plus_one - _temp291.curr, _temp291.curr);}); goto _LL293; _LL293:
if( Cyc_v_r){({ struct _tagged_string _temp294=( struct _tagged_string) _temp292;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp294.last_plus_one -
_temp294.curr, _temp294.curr);});} if( system( string_to_Cstring(( struct
_tagged_string) _temp292)) != 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: C compiler failed\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} Cyc_remove_cfiles();
return Cyc_compile_failure? 1: 0;}}}}}}}