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
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern
struct Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* l);
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
struct _tagged_string); extern int Cyc_Filename_check_suffix( struct
_tagged_string, struct _tagged_string); struct Cyc_Id_id; struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
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
i2, struct _tuple9* info); extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(* get)( void*), struct Cyc_List_List* la, struct Cyc_List_List*
linfo); extern struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*);
extern void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Stdio___sFILE*);
extern void Cyc_Lex_lex_init(); static int Cyc_pp_r= 0; static int Cyc_noexpand_r=
0; static int Cyc_noshake_r= 0; static int Cyc_stop_after_cpp_r= 0; static int
Cyc_parseonly_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0; static int
Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r= 0; static int Cyc_stop_after_asmfile_r=
0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0; static int Cyc_save_temps_r=
0; static int Cyc_save_c_r= 0; static int Cyc_add_cyc_namespace_r= 1; static int
Cyc_print_full_evars_r= 0; static int Cyc_print_all_tvars_r= 0; static int Cyc_print_all_kinds_r=
0; static struct _tagged_string* Cyc_output_file= 0; static void Cyc_set_output_file(
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
_temp23.last_plus_one= _temp22 + 12; _temp23;}));} static int Cyc_produce_dependencies=
0; static void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r= 1; Cyc_produce_dependencies=
1; Cyc_add_cpparg(({ unsigned char* _temp24=( unsigned char*)"-M"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}));} static struct _tagged_string* Cyc_dependencies_target=
0; static void Cyc_set_dependencies_target( struct _tagged_string s){ Cyc_dependencies_target=({
struct _tagged_string* _temp26=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp26[ 0]= s; _temp26;});} static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r= 1; Cyc_add_ccarg(({ unsigned char* _temp27=(
unsigned char*)"-c"; struct _tagged_string _temp28; _temp28.curr= _temp27;
_temp28.base= _temp27; _temp28.last_plus_one= _temp27 + 3; _temp28;}));} static
void Cyc_set_stop_after_asmfile(){ Cyc_stop_after_asmfile_r= 1; Cyc_add_ccarg(({
unsigned char* _temp29=( unsigned char*)"-S"; struct _tagged_string _temp30;
_temp30.curr= _temp29; _temp30.base= _temp29; _temp30.last_plus_one= _temp29 + 3;
_temp30;}));} static struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other(
struct _tagged_string s){ if( Cyc_is_cyclone_sourcefile( s)){ Cyc_cyclone_files=({
struct Cyc_List_List* _temp31=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp31->hd=( void*)({ struct _tagged_string* _temp32=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp32[ 0]= s;
_temp32;}); _temp31->tl= Cyc_cyclone_files; _temp31;});{ struct _tagged_string
_temp33= Cyc_Filename_chop_extension( s); goto _LL34; _LL34: { struct
_tagged_string _temp37= Cyc_String_strconcat(( struct _tagged_string) _temp33,({
unsigned char* _temp35=( unsigned char*)".c"; struct _tagged_string _temp36;
_temp36.curr= _temp35; _temp36.base= _temp35; _temp36.last_plus_one= _temp35 + 3;
_temp36;})); goto _LL38; _LL38: Cyc_add_ccarg(( struct _tagged_string) _temp37);}}}
else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct _tagged_string s){
if( Cyc_save_temps_r){ return;} else{ Cyc_Stdio_remove( s);}} int Cyc_compile_failure=
0; struct Cyc_Stdio___sFILE* Cyc_try_file_open( struct _tagged_string filename,
struct _tagged_string mode, struct _tagged_string msg_part){ struct
_handler_cons _temp39; _push_handler(& _temp39);{ int _temp41= 0; if( setjmp(
_temp39.handler)){ _temp41= 1;} if( ! _temp41){{ struct Cyc_Stdio___sFILE*
_temp42=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open( filename, mode);
_npop_handler( 0u); return _temp42;}; _pop_handler();} else{ void* _temp40=(
void*) _exn_thrown; void* _temp44= _temp40; _LL46: goto _LL47; _LL48: goto _LL49;
_LL47: Cyc_compile_failure= 1;({ struct _tagged_string _temp50= msg_part; struct
_tagged_string _temp51= filename; fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nError: couldn't open %.*s %.*s\n",
_temp50.last_plus_one - _temp50.curr, _temp50.curr, _temp51.last_plus_one -
_temp51.curr, _temp51.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return 0; _LL49:( void) _throw( _temp44); _LL45:;}}} struct
Cyc_List_List* Cyc_do_stage( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( void*, struct Cyc_List_List*), void* env, struct Cyc_List_List* tds){ struct
_tagged_string exn_string=({ unsigned char* _temp94=( unsigned char*)""; struct
_tagged_string _temp95; _temp95.curr= _temp94; _temp95.base= _temp94; _temp95.last_plus_one=
_temp94 + 1; _temp95;}); struct _tagged_string explain_string=({ unsigned char*
_temp92=( unsigned char*)""; struct _tagged_string _temp93; _temp93.curr=
_temp92; _temp93.base= _temp92; _temp93.last_plus_one= _temp92 + 1; _temp93;});
int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct* _temp88=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp88[ 0]=({ struct Cyc_Core_Impossible_struct _temp89; _temp89.tag= Cyc_Core_Impossible;
_temp89.f1=({ unsigned char* _temp90=( unsigned char*)""; struct _tagged_string
_temp91; _temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one=
_temp90 + 1; _temp91;}); _temp89;}); _temp88;}); struct Cyc_List_List* _temp52=
0; goto _LL53; _LL53:{ struct _handler_cons _temp54; _push_handler(& _temp54);{
int _temp56= 0; if( setjmp( _temp54.handler)){ _temp56= 1;} if( ! _temp56){
_temp52= f( env, tds);; _pop_handler();} else{ void* _temp55=( void*)
_exn_thrown; void* _temp58= _temp55; struct _tagged_string _temp70; struct
_tagged_string _temp72; _LL60: if(*(( void**) _temp58) == Cyc_Core_Impossible){
_LL71: _temp70=(( struct Cyc_Core_Impossible_struct*) _temp58)->f1; goto _LL61;}
else{ goto _LL62;} _LL62: if( _temp58 == Cyc_Dict_Absent){ goto _LL63;} else{
goto _LL64;} _LL64: if(*(( void**) _temp58) == Cyc_Core_InvalidArg){ _LL73:
_temp72=(( struct Cyc_Core_InvalidArg_struct*) _temp58)->f1; goto _LL65;} else{
goto _LL66;} _LL66: goto _LL67; _LL68: goto _LL69; _LL61: exn_string=({
unsigned char* _temp74=( unsigned char*)"Exception Core::Impossible"; struct
_tagged_string _temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
_temp74 + 27; _temp75;}); explain_string= _temp70; goto _LL59; _LL63: exn_string=({
unsigned char* _temp76=( unsigned char*)"Exception Dict::Absent"; struct
_tagged_string _temp77; _temp77.curr= _temp76; _temp77.base= _temp76; _temp77.last_plus_one=
_temp76 + 23; _temp77;}); goto _LL59; _LL65: exn_string=({ unsigned char*
_temp78=( unsigned char*)"Exception Core::InvalidArg"; struct _tagged_string
_temp79; _temp79.curr= _temp78; _temp79.base= _temp78; _temp79.last_plus_one=
_temp78 + 27; _temp79;}); explain_string= _temp72; goto _LL59; _LL67: ex=
_temp58; other_exn= 1; exn_string=({ unsigned char* _temp80=( unsigned char*)"Uncaught exception";
struct _tagged_string _temp81; _temp81.curr= _temp80; _temp81.base= _temp80;
_temp81.last_plus_one= _temp80 + 19; _temp81;}); goto _LL59; _LL69:( void)
_throw( _temp58); _LL59:;}}} if( Cyc_Position_error_p()){ Cyc_compile_failure= 1;}
if( Cyc_String_strcmp( exn_string,({ unsigned char* _temp82=( unsigned char*)"";
struct _tagged_string _temp83; _temp83.curr= _temp82; _temp83.base= _temp82;
_temp83.last_plus_one= _temp82 + 1; _temp83;})) != 0){ Cyc_compile_failure= 1;({
struct _tagged_string _temp84= exn_string; struct _tagged_string _temp85=
stage_name; struct _tagged_string _temp86= explain_string; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\n%.*s thrown during %.*s: %.*s", _temp84.last_plus_one
- _temp84.curr, _temp84.curr, _temp85.last_plus_one - _temp85.curr, _temp85.curr,
_temp86.last_plus_one - _temp86.curr, _temp86.curr);});} if( Cyc_compile_failure){
fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nCOMPILATION FAILED!\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); if( other_exn){( void) _throw( ex);}
return _temp52;} else{ if( Cyc_v_r){({ struct _tagged_string _temp87= stage_name;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s completed.\n", _temp87.last_plus_one
- _temp87.curr, _temp87.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return _temp52;}} return _temp52;} struct Cyc_List_List* Cyc_do_parse(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp96= Cyc_Parse_parse_file( f); goto _LL97; _LL97: Cyc_Lex_lex_init();
return _temp96;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds); if( ! Cyc_noshake_r){
tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck(
int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds);
return tds;} struct _tuple10{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Stdio___sFILE*
f2; struct Cyc_Stdio___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple10* params, struct Cyc_List_List* tds){ struct _tuple10 _temp100;
struct Cyc_Stdio___sFILE* _temp101; struct Cyc_Stdio___sFILE* _temp103; struct
Cyc_Tcenv_Tenv* _temp105; struct _tuple10* _temp98= params; _temp100=* _temp98;
_LL106: _temp105= _temp100.f1; goto _LL104; _LL104: _temp103= _temp100.f2; goto
_LL102; _LL102: _temp101= _temp100.f3; goto _LL99; _LL99: { struct Cyc_Interface_I*
_temp107= Cyc_Interface_extract( _temp105->ae); goto _LL108; _LL108: if(
_temp103 == 0){ Cyc_Interface_save( _temp107, _temp101);} else{ struct Cyc_Interface_I*
_temp109= Cyc_Interface_parse(( struct Cyc_Stdio___sFILE*) _check_null( _temp103));
goto _LL110; _LL110: if( ! Cyc_Interface_is_subinterface( _temp109, _temp107,({
struct _tuple9* _temp111=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp111->f1=({ unsigned char* _temp114=( unsigned char*)"written interface";
struct _tagged_string _temp115; _temp115.curr= _temp114; _temp115.base= _temp114;
_temp115.last_plus_one= _temp114 + 18; _temp115;}); _temp111->f2=({
unsigned char* _temp112=( unsigned char*)"maximal interface"; struct
_tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 18; _temp113;}); _temp111;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp109, _temp101);}} return tds;}} struct Cyc_List_List*
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
struct _tagged_string)({ struct _tagged_string _temp116={ 0, 0, 0}; _temp116;})).curr){
return 0;}{ struct Cyc_List_List* _temp117= 0; goto _LL118; _LL118: {
unsigned int _temp119= Cyc_String_strlen( s); goto _LL120; _LL120: while(
_temp119 > 0) { struct _tagged_string _temp121= Cyc_String_strchr( s, c); goto
_LL122; _LL122: if( _temp121.curr ==(( struct _tagged_string)({ struct
_tagged_string _temp123={ 0, 0, 0}; _temp123;})).curr){ _temp117=({ struct Cyc_List_List*
_temp124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp124->hd=( void*)({ struct _tagged_string* _temp125=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp125[ 0]= s; _temp125;});
_temp124->tl= _temp117; _temp124;}); break;} else{ _temp117=({ struct Cyc_List_List*
_temp126=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp126->hd=( void*)({ struct _tagged_string* _temp127=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp127[ 0]=( struct _tagged_string)
Cyc_String_substring( s, 0,( unsigned int)((( struct _tagged_string) _temp121).curr
- s.curr)); _temp127;}); _temp126->tl= _temp117; _temp126;}); _temp119 -=((
struct _tagged_string) _temp121).curr - s.curr; s=({ struct _tagged_string
_temp128= _temp121; _temp128.curr += 1; _temp128;});}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp117);}}} int Cyc_file_exists(
struct _tagged_string file){ struct Cyc_Stdio___sFILE* f= 0;{ struct
_handler_cons _temp129; _push_handler(& _temp129);{ int _temp131= 0; if( setjmp(
_temp129.handler)){ _temp131= 1;} if( ! _temp131){ f=( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_file_open( file,({ unsigned char* _temp132=( unsigned char*)"r";
struct _tagged_string _temp133; _temp133.curr= _temp132; _temp133.base= _temp132;
_temp133.last_plus_one= _temp132 + 2; _temp133;}));; _pop_handler();} else{ void*
_temp130=( void*) _exn_thrown; void* _temp135= _temp130; _LL137: goto _LL138;
_LL139: goto _LL140; _LL138: goto _LL136; _LL140:( void) _throw( _temp135);
_LL136:;}}} if( f == 0){ return 0;} else{ Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f)); return 1;}} struct _tagged_string* Cyc_find( struct Cyc_List_List*
dirs, struct _tagged_string file){ if( file.curr ==(( struct _tagged_string)({
struct _tagged_string _temp141={ 0, 0, 0}; _temp141;})).curr){ return 0;} for( 0;
dirs != 0; dirs=(( struct Cyc_List_List*) _check_null( dirs))->tl){ struct
_tagged_string _temp142=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( dirs))->hd); goto _LL143; _LL143: if( _temp142.curr ==(( struct
_tagged_string)({ struct _tagged_string _temp144={ 0, 0, 0}; _temp144;})).curr?
1: Cyc_String_strlen( _temp142) == 0){ continue;}{ struct _tagged_string s=(
struct _tagged_string) Cyc_Filename_concat( _temp142, file); if( Cyc_file_exists(
s)){ return({ struct _tagged_string* _temp145=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp145[ 0]= s; _temp145;});}}}
return 0;} struct _tagged_string Cyc_do_find( struct Cyc_List_List* dirs, struct
_tagged_string file){ struct _tagged_string* _temp146= Cyc_find( dirs, file);
goto _LL147; _LL147: if( _temp146 == 0){({ struct _tagged_string _temp148= file;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file %.*s",
_temp148.last_plus_one - _temp148.curr, _temp148.curr);}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); exit( 1);} return*(( struct _tagged_string*) _check_null(
_temp146));} void Cyc_process_file( struct _tagged_string filename){ struct
_tagged_string _temp149= Cyc_Filename_chop_extension( filename); goto _LL150;
_LL150: { struct _tagged_string _temp153= Cyc_String_strconcat(( struct
_tagged_string) _temp149,({ unsigned char* _temp151=( unsigned char*)".cyp";
struct _tagged_string _temp152; _temp152.curr= _temp151; _temp152.base= _temp151;
_temp152.last_plus_one= _temp151 + 5; _temp152;})); goto _LL154; _LL154: {
struct _tagged_string _temp157= Cyc_String_strconcat(( struct _tagged_string)
_temp149,({ unsigned char* _temp155=( unsigned char*)".cyci"; struct
_tagged_string _temp156; _temp156.curr= _temp155; _temp156.base= _temp155;
_temp156.last_plus_one= _temp155 + 6; _temp156;})); goto _LL158; _LL158: {
struct _tagged_string _temp161= Cyc_String_strconcat(( struct _tagged_string)
_temp149,({ unsigned char* _temp159=( unsigned char*)".cycio"; struct
_tagged_string _temp160; _temp160.curr= _temp159; _temp160.base= _temp159;
_temp160.last_plus_one= _temp159 + 7; _temp160;})); goto _LL162; _LL162: {
struct _tagged_string _temp165= Cyc_String_strconcat(( struct _tagged_string)
_temp149,({ unsigned char* _temp163=( unsigned char*)".c"; struct _tagged_string
_temp164; _temp164.curr= _temp163; _temp164.base= _temp163; _temp164.last_plus_one=
_temp163 + 3; _temp164;})); goto _LL166; _LL166: if( Cyc_v_r){({ struct
_tagged_string _temp167= filename; fprintf( sfile_to_file( Cyc_Stdio_stderr),"Compiling %.*s\n",
_temp167.last_plus_one - _temp167.curr, _temp167.curr);});}{ struct Cyc_Stdio___sFILE*
f0= Cyc_try_file_open( filename,({ unsigned char* _temp275=( unsigned char*)"r";
struct _tagged_string _temp276; _temp276.curr= _temp275; _temp276.base= _temp275;
_temp276.last_plus_one= _temp275 + 2; _temp276;}),({ unsigned char* _temp277=(
unsigned char*)"input file"; struct _tagged_string _temp278; _temp278.curr=
_temp277; _temp278.base= _temp277; _temp278.last_plus_one= _temp277 + 11;
_temp278;})); if( Cyc_compile_failure){ return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f0));{ struct _tagged_string _temp174= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp168=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp168->hd=( void*)({ struct _tagged_string* _temp169=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp169[ 0]=({
unsigned char* _temp170=( unsigned char*)""; struct _tagged_string _temp171;
_temp171.curr= _temp170; _temp171.base= _temp170; _temp171.last_plus_one=
_temp170 + 1; _temp171;}); _temp169;}); _temp168->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs); _temp168;}),({
unsigned char* _temp172=( unsigned char*)" "; struct _tagged_string _temp173;
_temp173.curr= _temp172; _temp173.base= _temp172; _temp173.last_plus_one=
_temp172 + 2; _temp173;})); goto _LL175; _LL175: { struct _tagged_string
stdinc_string=({ unsigned char* _temp273=( unsigned char*)" -Ic:/cyclone/include";
struct _tagged_string _temp274; _temp274.curr= _temp273; _temp274.base= _temp273;
_temp274.last_plus_one= _temp273 + 22; _temp274;}); struct _tagged_string
_temp178= Cyc_Stdlib_getenv(({ unsigned char* _temp176=( unsigned char*)"CYCLONE_INCLUDE_PATH";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 21; _temp177;})); goto _LL179; _LL179: if(
_temp178.curr !=({ struct _tagged_string _temp180={ 0, 0, 0}; _temp180;}).curr){
struct Cyc_List_List* _temp181= Cyc_split_by_char(( struct _tagged_string)
_temp178,':'); goto _LL182; _LL182: { struct _tagged_string _temp189= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp183=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp183->hd=( void*)({ struct _tagged_string* _temp184=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp184[ 0]=({
unsigned char* _temp185=( unsigned char*)""; struct _tagged_string _temp186;
_temp186.curr= _temp185; _temp186.base= _temp185; _temp186.last_plus_one=
_temp185 + 1; _temp186;}); _temp184;}); _temp183->tl= _temp181; _temp183;}),({
unsigned char* _temp187=( unsigned char*)" -I"; struct _tagged_string _temp188;
_temp188.curr= _temp187; _temp188.base= _temp187; _temp188.last_plus_one=
_temp187 + 4; _temp188;})); goto _LL190; _LL190: stdinc_string=( struct
_tagged_string)({ struct _tagged_string _temp191=( struct _tagged_string)
_temp189; struct _tagged_string _temp192= stdinc_string; xprintf("%.*s%.*s",
_temp191.last_plus_one - _temp191.curr, _temp191.curr, _temp192.last_plus_one -
_temp192.curr, _temp192.curr);});}}{ struct _tagged_string ofile_string; if( Cyc_stop_after_cpp_r){
if( Cyc_output_file != 0){ ofile_string=( struct _tagged_string)({ struct
_tagged_string _temp193=*(( struct _tagged_string*) _check_null( Cyc_output_file));
xprintf(" > %.*s", _temp193.last_plus_one - _temp193.curr, _temp193.curr);});}
else{ ofile_string=({ unsigned char* _temp194=( unsigned char*)""; struct
_tagged_string _temp195; _temp195.curr= _temp194; _temp195.base= _temp194;
_temp195.last_plus_one= _temp194 + 1; _temp195;});}} else{ ofile_string=( struct
_tagged_string)({ struct _tagged_string _temp196=( struct _tagged_string)
_temp153; xprintf(" > %.*s", _temp196.last_plus_one - _temp196.curr, _temp196.curr);});}{
struct _tagged_string fixup_string; if( Cyc_produce_dependencies){ if( Cyc_dependencies_target
== 0){ fixup_string=({ unsigned char* _temp197=( unsigned char*)" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
struct _tagged_string _temp198; _temp198.curr= _temp197; _temp198.base= _temp197;
_temp198.last_plus_one= _temp197 + 35; _temp198;});} else{ fixup_string=( struct
_tagged_string)({ struct _tagged_string _temp199=*(( struct _tagged_string*)
_check_null( Cyc_dependencies_target)); xprintf(" | sed 's/^.*\\.cyc\\.o:/%.*s:/'",
_temp199.last_plus_one - _temp199.curr, _temp199.curr);});}} else{ fixup_string=({
unsigned char* _temp200=( unsigned char*)""; struct _tagged_string _temp201;
_temp201.curr= _temp200; _temp201.base= _temp200; _temp201.last_plus_one=
_temp200 + 1; _temp201;});}{ struct _tagged_string _temp207=({ struct
_tagged_string _temp202=( struct _tagged_string) _temp174; struct _tagged_string
_temp203= stdinc_string; struct _tagged_string _temp204= filename; struct
_tagged_string _temp205= fixup_string; struct _tagged_string _temp206=
ofile_string; xprintf("gcc -x c -E -U__GNUC__ -nostdinc%.*s%.*s %.*s%.*s%.*s",
_temp202.last_plus_one - _temp202.curr, _temp202.curr, _temp203.last_plus_one -
_temp203.curr, _temp203.curr, _temp204.last_plus_one - _temp204.curr, _temp204.curr,
_temp205.last_plus_one - _temp205.curr, _temp205.curr, _temp206.last_plus_one -
_temp206.curr, _temp206.curr);}); goto _LL208; _LL208: if( Cyc_v_r){({ struct
_tagged_string _temp209=( struct _tagged_string) _temp207; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp209.last_plus_one - _temp209.curr,
_temp209.curr);});} if( system( string_to_Cstring(( struct _tagged_string)
_temp207)) != 0){ Cyc_compile_failure= 1; fprintf( sfile_to_file( Cyc_Stdio_stderr),"\nError: preprocessing\n");
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_string) _temp153);{ struct Cyc_Stdio___sFILE* in_file= Cyc_try_file_open((
struct _tagged_string) _temp153,({ unsigned char* _temp269=( unsigned char*)"r";
struct _tagged_string _temp270; _temp270.curr= _temp269; _temp270.base= _temp269;
_temp270.last_plus_one= _temp269 + 2; _temp270;}),({ unsigned char* _temp271=(
unsigned char*)"file"; struct _tagged_string _temp272; _temp272.curr= _temp271;
_temp272.base= _temp271; _temp272.last_plus_one= _temp271 + 5; _temp272;})); if(
Cyc_compile_failure){ return;}{ struct Cyc_List_List* tds= 0;{ struct
_handler_cons _temp210; _push_handler(& _temp210);{ int _temp212= 0; if( setjmp(
_temp210.handler)){ _temp212= 1;} if( ! _temp212){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp213=( unsigned char*)"parsing"; struct
_tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 8; _temp214;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)
_check_null( in_file), tds);; _pop_handler();} else{ void* _temp211=( void*)
_exn_thrown; void* _temp216= _temp211; _LL218: goto _LL219; _LL220: goto _LL221;
_LL219: Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*) _check_null( in_file));
Cyc_remove_file(( struct _tagged_string) _temp153);( void) _throw( _temp216);
_LL221:( void) _throw( _temp216); _LL217:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( in_file)); if( Cyc_compile_failure){ Cyc_remove_file(( struct
_tagged_string) _temp153); return;}{ struct Cyc_Tcenv_Tenv* _temp222= Cyc_Tcenv_tc_init();
goto _LL223; _LL223: if( Cyc_parseonly_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp224=( unsigned char*)"type checking";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 14; _temp225;}), Cyc_do_typecheck, _temp222,
tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_string)
_temp153); return;} tds=(( struct Cyc_List_List*(*)( struct _tagged_string
stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env,
struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp226=(
unsigned char*)"control-flow checking"; struct _tagged_string _temp227; _temp227.curr=
_temp226; _temp227.base= _temp226; _temp227.last_plus_one= _temp226 + 22;
_temp227;}), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();}
Cyc_remove_file(( struct _tagged_string) _temp153); if( Cyc_compile_failure){
return;} if( Cyc_ic_r){ struct Cyc_Stdio___sFILE* inter_file= Cyc_Stdio_fopen((
struct _tagged_string) _temp157,({ unsigned char* _temp235=( unsigned char*)"r";
struct _tagged_string _temp236; _temp236.curr= _temp235; _temp236.base= _temp235;
_temp236.last_plus_one= _temp235 + 2; _temp236;})); struct Cyc_Stdio___sFILE*
inter_objfile= Cyc_try_file_open(( struct _tagged_string) _temp161,({
unsigned char* _temp231=( unsigned char*)"w"; struct _tagged_string _temp232;
_temp232.curr= _temp231; _temp232.base= _temp231; _temp232.last_plus_one=
_temp231 + 2; _temp232;}),({ unsigned char* _temp233=( unsigned char*)"interface object file";
struct _tagged_string _temp234; _temp234.curr= _temp233; _temp234.base= _temp233;
_temp234.last_plus_one= _temp233 + 22; _temp234;})); if( inter_objfile == 0){
Cyc_compile_failure= 1; return;} Cyc_Position_reset_position(( struct
_tagged_string) _temp157); tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct _tuple10*, struct
Cyc_List_List*), struct _tuple10* env, struct Cyc_List_List* tds)) Cyc_do_stage)(({
unsigned char* _temp228=( unsigned char*)"interface checking"; struct
_tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 19; _temp229;}), Cyc_do_interface,({ struct
_tuple10* _temp230=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp230->f1= _temp222; _temp230->f2= inter_file; _temp230->f3=( struct Cyc_Stdio___sFILE*)
_check_null( inter_objfile); _temp230;}), tds); if( inter_file != 0){ Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_file));} Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto
PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds)) Cyc_do_stage)(({ unsigned char* _temp237=( unsigned char*)"translation to C";
struct _tagged_string _temp238; _temp238.curr= _temp237; _temp238.base= _temp237;
_temp238.last_plus_one= _temp237 + 17; _temp238;}), Cyc_do_translate, 1, tds);
if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp239=(
unsigned char*)"post-pass to VC"; struct _tagged_string _temp240; _temp240.curr=
_temp239; _temp240.base= _temp239; _temp240.last_plus_one= _temp239 + 16;
_temp240;}), Cyc_do_tovc, 1, tds);} if( Cyc_compile_failure){ return;} PRINTC:
if( tds != 0){ struct Cyc_Stdio___sFILE* out_file; if( Cyc_parseonly_r? 1: Cyc_tc_r){
if( Cyc_output_file != 0){ out_file= Cyc_try_file_open(*(( struct _tagged_string*)
_check_null( Cyc_output_file)),({ unsigned char* _temp241=( unsigned char*)"w";
struct _tagged_string _temp242; _temp242.curr= _temp241; _temp242.base= _temp241;
_temp242.last_plus_one= _temp241 + 2; _temp242;}),({ unsigned char* _temp243=(
unsigned char*)"output file"; struct _tagged_string _temp244; _temp244.curr=
_temp243; _temp244.base= _temp243; _temp244.last_plus_one= _temp243 + 12;
_temp244;}));} else{ out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;}}
else{ if( Cyc_toc_r? Cyc_output_file != 0: 0){ out_file= Cyc_try_file_open(*((
struct _tagged_string*) _check_null( Cyc_output_file)),({ unsigned char*
_temp245=( unsigned char*)"w"; struct _tagged_string _temp246; _temp246.curr=
_temp245; _temp246.base= _temp245; _temp246.last_plus_one= _temp245 + 2;
_temp246;}),({ unsigned char* _temp247=( unsigned char*)"output file"; struct
_tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 12; _temp248;}));} else{ out_file= Cyc_try_file_open((
struct _tagged_string) _temp165,({ unsigned char* _temp249=( unsigned char*)"w";
struct _tagged_string _temp250; _temp250.curr= _temp249; _temp250.base= _temp249;
_temp250.last_plus_one= _temp249 + 2; _temp250;}),({ unsigned char* _temp251=(
unsigned char*)"output file"; struct _tagged_string _temp252; _temp252.curr=
_temp251; _temp252.base= _temp251; _temp252.last_plus_one= _temp251 + 12;
_temp252;}));}} if( Cyc_compile_failure){ return;}{ struct _handler_cons
_temp253; _push_handler(& _temp253);{ int _temp255= 0; if( setjmp( _temp253.handler)){
_temp255= 1;} if( ! _temp255){ tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp256=( unsigned char*)"printing"; struct
_tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 9; _temp257;}), Cyc_do_print,( struct Cyc_Stdio___sFILE*)
_check_null( out_file), tds);; _pop_handler();} else{ void* _temp254=( void*)
_exn_thrown; void* _temp259= _temp254; _LL261: goto _LL262; _LL263: goto _LL264;
_LL262: Cyc_compile_failure= 1; Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp265=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp265->hd=( void*)({
struct _tagged_string* _temp266=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp266[ 0]=( struct _tagged_string) _temp165;
_temp266;}); _temp265->tl= Cyc_cfiles; _temp265;});( void) _throw( _temp259);
_LL264:( void) _throw( _temp259); _LL260:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp267=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp267->hd=( void*)({
struct _tagged_string* _temp268=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp268[ 0]=( struct _tagged_string) _temp165;
_temp268;}); _temp267->tl= Cyc_cfiles; _temp267;});}}}}}}}}}}}}}}} static
unsigned char _temp281[ 8u]="<final>"; static struct _tagged_string Cyc_final_str={
_temp281, _temp281, _temp281 + 8u}; static struct _tagged_string* Cyc_final_strptr=&
Cyc_final_str; static struct Cyc_Interface_I* Cyc_read_cycio( struct
_tagged_string* n){ if( n == Cyc_final_strptr){ return Cyc_Interface_final();}{
struct _tagged_string basename;{ struct _handler_cons _temp282; _push_handler(&
_temp282);{ int _temp284= 0; if( setjmp( _temp282.handler)){ _temp284= 1;} if( !
_temp284){ basename=( struct _tagged_string) Cyc_Filename_chop_extension(* n);;
_pop_handler();} else{ void* _temp283=( void*) _exn_thrown; void* _temp286=
_temp283; struct _tagged_string _temp292; _LL288: if(*(( void**) _temp286) ==
Cyc_Core_InvalidArg){ _LL293: _temp292=(( struct Cyc_Core_InvalidArg_struct*)
_temp286)->f1; goto _LL289;} else{ goto _LL290;} _LL290: goto _LL291; _LL289:
basename=* n; goto _LL287; _LL291:( void) _throw( _temp286); _LL287:;}}}{ struct
_tagged_string _temp296= Cyc_String_strconcat( basename,({ unsigned char*
_temp294=( unsigned char*)".cycio"; struct _tagged_string _temp295; _temp295.curr=
_temp294; _temp295.base= _temp294; _temp295.last_plus_one= _temp294 + 7;
_temp295;})); goto _LL297; _LL297: { struct Cyc_Stdio___sFILE* _temp302= Cyc_try_file_open((
struct _tagged_string) _temp296,({ unsigned char* _temp298=( unsigned char*)"r";
struct _tagged_string _temp299; _temp299.curr= _temp298; _temp299.base= _temp298;
_temp299.last_plus_one= _temp298 + 2; _temp299;}),({ unsigned char* _temp300=(
unsigned char*)"interface object file"; struct _tagged_string _temp301; _temp301.curr=
_temp300; _temp301.base= _temp300; _temp301.last_plus_one= _temp300 + 22;
_temp301;})); goto _LL303; _LL303: if( _temp302 == 0){ Cyc_compile_failure= 1;
Cyc_remove_cfiles(); exit( 1);} Cyc_Position_reset_position(( struct
_tagged_string) _temp296);{ struct Cyc_Interface_I* _temp304= Cyc_Interface_load((
struct Cyc_Stdio___sFILE*) _check_null( _temp302)); goto _LL305; _LL305: Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( _temp302)); return _temp304;}}}}} static
int Cyc_is_cfile( struct _tagged_string* n){ return*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(*
n, sizeof( unsigned char), 0) !='-';} struct _tagged_ptr3{ struct _tagged_string**
curr; struct _tagged_string** base; struct _tagged_string** last_plus_one; } ;
struct _tuple11{ struct _tagged_string f1; void* f2; struct _tagged_string f3; }
; struct _tagged_ptr4{ struct _tuple11** curr; struct _tuple11** base; struct
_tuple11** last_plus_one; } ; int Cyc_main( int argc, struct _tagged_ptr0 argv){
struct Cyc_List_List* options=({ struct Cyc_List_List*(* _temp413)( struct
_tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4)) Cyc_List_list;
struct _tuple11* _temp416=({ struct _tuple11* _temp689=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp689->f1=({ unsigned char* _temp694=(
unsigned char*)"-v"; struct _tagged_string _temp695; _temp695.curr= _temp694;
_temp695.base= _temp694; _temp695.last_plus_one= _temp694 + 3; _temp695;});
_temp689->f2=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp692=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp692[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp693; _temp693.tag= Cyc_Arg_Set_spec; _temp693.f1=& Cyc_v_r; _temp693;});
_temp692;}); _temp689->f3=({ unsigned char* _temp690=( unsigned char*)"    Print compilation stages verbosely";
struct _tagged_string _temp691; _temp691.curr= _temp690; _temp691.base= _temp690;
_temp691.last_plus_one= _temp690 + 39; _temp691;}); _temp689;}); struct _tuple11*
_temp417=({ struct _tuple11* _temp682=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp682->f1=({ unsigned char* _temp687=( unsigned char*)"-o";
struct _tagged_string _temp688; _temp688.curr= _temp687; _temp688.base= _temp687;
_temp688.last_plus_one= _temp687 + 3; _temp688;}); _temp682->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp685=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp685[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp686; _temp686.tag= Cyc_Arg_String_spec; _temp686.f1=
Cyc_set_output_file; _temp686;}); _temp685;}); _temp682->f3=({ unsigned char*
_temp683=( unsigned char*)"    Output file name"; struct _tagged_string _temp684;
_temp684.curr= _temp683; _temp684.base= _temp683; _temp684.last_plus_one=
_temp683 + 21; _temp684;}); _temp682;}); struct _tuple11* _temp418=({ struct
_tuple11* _temp675=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp675->f1=({ unsigned char* _temp680=( unsigned char*)"-D*"; struct
_tagged_string _temp681; _temp681.curr= _temp680; _temp681.base= _temp680;
_temp681.last_plus_one= _temp680 + 4; _temp681;}); _temp675->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp678=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp678[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp679; _temp679.tag= Cyc_Arg_Flag_spec; _temp679.f1= Cyc_add_cpparg; _temp679;});
_temp678;}); _temp675->f3=({ unsigned char* _temp676=( unsigned char*)"   Pass definition to preprocessor";
struct _tagged_string _temp677; _temp677.curr= _temp676; _temp677.base= _temp676;
_temp677.last_plus_one= _temp676 + 35; _temp677;}); _temp675;}); struct _tuple11*
_temp419=({ struct _tuple11* _temp668=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp668->f1=({ unsigned char* _temp673=( unsigned char*)"-B*";
struct _tagged_string _temp674; _temp674.curr= _temp673; _temp674.base= _temp673;
_temp674.last_plus_one= _temp673 + 4; _temp674;}); _temp668->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp671=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp671[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp672; _temp672.tag= Cyc_Arg_Flag_spec; _temp672.f1= Cyc_add_cyclone_exec_prefix_path;
_temp672;}); _temp671;}); _temp668->f3=({ unsigned char* _temp669=(
unsigned char*)"   Add to the list of directories to search for compiler files";
struct _tagged_string _temp670; _temp670.curr= _temp669; _temp670.base= _temp669;
_temp670.last_plus_one= _temp669 + 63; _temp670;}); _temp668;}); struct _tuple11*
_temp420=({ struct _tuple11* _temp661=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp661->f1=({ unsigned char* _temp666=( unsigned char*)"-I*";
struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 4; _temp667;}); _temp661->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp664=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp664[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp665; _temp665.tag= Cyc_Arg_Flag_spec; _temp665.f1= Cyc_add_cpparg; _temp665;});
_temp664;}); _temp661->f3=({ unsigned char* _temp662=( unsigned char*)"   Add to the list of directories to search for include files";
struct _tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 62; _temp663;}); _temp661;}); struct _tuple11*
_temp421=({ struct _tuple11* _temp654=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp654->f1=({ unsigned char* _temp659=( unsigned char*)"-L*";
struct _tagged_string _temp660; _temp660.curr= _temp659; _temp660.base= _temp659;
_temp660.last_plus_one= _temp659 + 4; _temp660;}); _temp654->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp657=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp657[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp658; _temp658.tag= Cyc_Arg_Flag_spec; _temp658.f1= Cyc_add_cpparg; _temp658;});
_temp657;}); _temp654->f3=({ unsigned char* _temp655=( unsigned char*)"   Add to the list of directories for -l";
struct _tagged_string _temp656; _temp656.curr= _temp655; _temp656.base= _temp655;
_temp656.last_plus_one= _temp655 + 41; _temp656;}); _temp654;}); struct _tuple11*
_temp422=({ struct _tuple11* _temp647=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp647->f1=({ unsigned char* _temp652=( unsigned char*)"-l*";
struct _tagged_string _temp653; _temp653.curr= _temp652; _temp653.base= _temp652;
_temp653.last_plus_one= _temp652 + 4; _temp653;}); _temp647->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp650=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp650[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp651; _temp651.tag= Cyc_Arg_Flag_spec; _temp651.f1= Cyc_add_libarg; _temp651;});
_temp650;}); _temp647->f3=({ unsigned char* _temp648=( unsigned char*)"   Library file";
struct _tagged_string _temp649; _temp649.curr= _temp648; _temp649.base= _temp648;
_temp649.last_plus_one= _temp648 + 16; _temp649;}); _temp647;}); struct _tuple11*
_temp423=({ struct _tuple11* _temp640=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp640->f1=({ unsigned char* _temp645=( unsigned char*)"-c";
struct _tagged_string _temp646; _temp646.curr= _temp645; _temp646.base= _temp645;
_temp646.last_plus_one= _temp645 + 3; _temp646;}); _temp640->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp643=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp643[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp644; _temp644.tag= Cyc_Arg_Unit_spec; _temp644.f1= Cyc_set_stop_after_objectfile;
_temp644;}); _temp643;}); _temp640->f3=({ unsigned char* _temp641=(
unsigned char*)"    Produce object file"; struct _tagged_string _temp642;
_temp642.curr= _temp641; _temp642.base= _temp641; _temp642.last_plus_one=
_temp641 + 24; _temp642;}); _temp640;}); struct _tuple11* _temp424=({ struct
_tuple11* _temp633=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp633->f1=({ unsigned char* _temp638=( unsigned char*)"-s"; struct
_tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 3; _temp639;}); _temp633->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp636=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp636[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp637; _temp637.tag= Cyc_Arg_Flag_spec; _temp637.f1= Cyc_add_ccarg; _temp637;});
_temp636;}); _temp633->f3=({ unsigned char* _temp634=( unsigned char*)"    Remove all symbol table and relocation info from executable";
struct _tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 64; _temp635;}); _temp633;}); struct _tuple11*
_temp425=({ struct _tuple11* _temp626=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp626->f1=({ unsigned char* _temp631=( unsigned char*)"-O";
struct _tagged_string _temp632; _temp632.curr= _temp631; _temp632.base= _temp631;
_temp632.last_plus_one= _temp631 + 3; _temp632;}); _temp626->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp629=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp629[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp630; _temp630.tag= Cyc_Arg_Flag_spec; _temp630.f1= Cyc_add_ccarg; _temp630;});
_temp629;}); _temp626->f3=({ unsigned char* _temp627=( unsigned char*)"    Optimize";
struct _tagged_string _temp628; _temp628.curr= _temp627; _temp628.base= _temp627;
_temp628.last_plus_one= _temp627 + 13; _temp628;}); _temp626;}); struct _tuple11*
_temp426=({ struct _tuple11* _temp619=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp619->f1=({ unsigned char* _temp624=( unsigned char*)"-O2";
struct _tagged_string _temp625; _temp625.curr= _temp624; _temp625.base= _temp624;
_temp625.last_plus_one= _temp624 + 4; _temp625;}); _temp619->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp622=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp622[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp623; _temp623.tag= Cyc_Arg_Flag_spec; _temp623.f1= Cyc_add_ccarg; _temp623;});
_temp622;}); _temp619->f3=({ unsigned char* _temp620=( unsigned char*)"   Optimize";
struct _tagged_string _temp621; _temp621.curr= _temp620; _temp621.base= _temp620;
_temp621.last_plus_one= _temp620 + 12; _temp621;}); _temp619;}); struct _tuple11*
_temp427=({ struct _tuple11* _temp612=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp612->f1=({ unsigned char* _temp617=( unsigned char*)"-O3";
struct _tagged_string _temp618; _temp618.curr= _temp617; _temp618.base= _temp617;
_temp618.last_plus_one= _temp617 + 4; _temp618;}); _temp612->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp615=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp615[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp616; _temp616.tag= Cyc_Arg_Flag_spec; _temp616.f1= Cyc_add_ccarg; _temp616;});
_temp615;}); _temp612->f3=({ unsigned char* _temp613=( unsigned char*)"   Optimize";
struct _tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 12; _temp614;}); _temp612;}); struct _tuple11*
_temp428=({ struct _tuple11* _temp605=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp605->f1=({ unsigned char* _temp610=( unsigned char*)"-g";
struct _tagged_string _temp611; _temp611.curr= _temp610; _temp611.base= _temp610;
_temp611.last_plus_one= _temp610 + 3; _temp611;}); _temp605->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp608=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp608[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp609; _temp609.tag= Cyc_Arg_Flag_spec; _temp609.f1= Cyc_add_ccarg; _temp609;});
_temp608;}); _temp605->f3=({ unsigned char* _temp606=( unsigned char*)"    Compile for debugging";
struct _tagged_string _temp607; _temp607.curr= _temp606; _temp607.base= _temp606;
_temp607.last_plus_one= _temp606 + 26; _temp607;}); _temp605;}); struct _tuple11*
_temp429=({ struct _tuple11* _temp598=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp598->f1=({ unsigned char* _temp603=( unsigned char*)"-p";
struct _tagged_string _temp604; _temp604.curr= _temp603; _temp604.base= _temp603;
_temp604.last_plus_one= _temp603 + 3; _temp604;}); _temp598->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp601=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp601[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp602; _temp602.tag= Cyc_Arg_Flag_spec; _temp602.f1= Cyc_add_ccarg; _temp602;});
_temp601;}); _temp598->f3=({ unsigned char* _temp599=( unsigned char*)"    Compile for prof";
struct _tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 21; _temp600;}); _temp598;}); struct _tuple11*
_temp430=({ struct _tuple11* _temp591=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp591->f1=({ unsigned char* _temp596=( unsigned char*)"-pg";
struct _tagged_string _temp597; _temp597.curr= _temp596; _temp597.base= _temp596;
_temp597.last_plus_one= _temp596 + 4; _temp597;}); _temp591->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp594=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp594[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp595; _temp595.tag= Cyc_Arg_Flag_spec; _temp595.f1= Cyc_add_ccarg; _temp595;});
_temp594;}); _temp591->f3=({ unsigned char* _temp592=( unsigned char*)"   Compile for gprof";
struct _tagged_string _temp593; _temp593.curr= _temp592; _temp593.base= _temp592;
_temp593.last_plus_one= _temp592 + 21; _temp593;}); _temp591;}); struct _tuple11*
_temp431=({ struct _tuple11* _temp584=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp584->f1=({ unsigned char* _temp589=( unsigned char*)"-S";
struct _tagged_string _temp590; _temp590.curr= _temp589; _temp590.base= _temp589;
_temp590.last_plus_one= _temp589 + 3; _temp590;}); _temp584->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp587=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp587[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp588; _temp588.tag= Cyc_Arg_Unit_spec; _temp588.f1= Cyc_set_stop_after_asmfile;
_temp588;}); _temp587;}); _temp584->f3=({ unsigned char* _temp585=(
unsigned char*)"    Stop after producing assembly code"; struct _tagged_string
_temp586; _temp586.curr= _temp585; _temp586.base= _temp585; _temp586.last_plus_one=
_temp585 + 39; _temp586;}); _temp584;}); struct _tuple11* _temp432=({ struct
_tuple11* _temp577=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp577->f1=({ unsigned char* _temp582=( unsigned char*)"-M"; struct
_tagged_string _temp583; _temp583.curr= _temp582; _temp583.base= _temp582;
_temp583.last_plus_one= _temp582 + 3; _temp583;}); _temp577->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp580=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp580[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp581; _temp581.tag= Cyc_Arg_Unit_spec; _temp581.f1= Cyc_set_produce_dependencies;
_temp581;}); _temp580;}); _temp577->f3=({ unsigned char* _temp578=(
unsigned char*)"    Produce dependencies"; struct _tagged_string _temp579;
_temp579.curr= _temp578; _temp579.base= _temp578; _temp579.last_plus_one=
_temp578 + 25; _temp579;}); _temp577;}); struct _tuple11* _temp433=({ struct
_tuple11* _temp570=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp570->f1=({ unsigned char* _temp575=( unsigned char*)"-MG"; struct
_tagged_string _temp576; _temp576.curr= _temp575; _temp576.base= _temp575;
_temp576.last_plus_one= _temp575 + 4; _temp576;}); _temp570->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp573=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp573[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp574; _temp574.tag= Cyc_Arg_Flag_spec; _temp574.f1= Cyc_add_cpparg; _temp574;});
_temp573;}); _temp570->f3=({ unsigned char* _temp571=( unsigned char*)"    When producing dependencies assume missing files are generated";
struct _tagged_string _temp572; _temp572.curr= _temp571; _temp572.base= _temp571;
_temp572.last_plus_one= _temp571 + 67; _temp572;}); _temp570;}); struct _tuple11*
_temp434=({ struct _tuple11* _temp563=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp563->f1=({ unsigned char* _temp568=( unsigned char*)"-MT";
struct _tagged_string _temp569; _temp569.curr= _temp568; _temp569.base= _temp568;
_temp569.last_plus_one= _temp568 + 4; _temp569;}); _temp563->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp566=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp566[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp567; _temp567.tag= Cyc_Arg_String_spec; _temp567.f1=
Cyc_set_dependencies_target; _temp567;}); _temp566;}); _temp563->f3=({
unsigned char* _temp564=( unsigned char*)"   Give target for dependencies";
struct _tagged_string _temp565; _temp565.curr= _temp564; _temp565.base= _temp564;
_temp565.last_plus_one= _temp564 + 32; _temp565;}); _temp563;}); struct _tuple11*
_temp435=({ struct _tuple11* _temp556=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp556->f1=({ unsigned char* _temp561=( unsigned char*)"-E";
struct _tagged_string _temp562; _temp562.curr= _temp561; _temp562.base= _temp561;
_temp562.last_plus_one= _temp561 + 3; _temp562;}); _temp556->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp559=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp559[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp560; _temp560.tag= Cyc_Arg_Set_spec; _temp560.f1=& Cyc_stop_after_cpp_r;
_temp560;}); _temp559;}); _temp556->f3=({ unsigned char* _temp557=(
unsigned char*)"    Stop after preprocessing"; struct _tagged_string _temp558;
_temp558.curr= _temp557; _temp558.base= _temp557; _temp558.last_plus_one=
_temp557 + 29; _temp558;}); _temp556;}); struct _tuple11* _temp436=({ struct
_tuple11* _temp549=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp549->f1=({ unsigned char* _temp554=( unsigned char*)"-parseonly"; struct
_tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 11; _temp555;}); _temp549->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp552=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp552[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp553; _temp553.tag= Cyc_Arg_Set_spec; _temp553.f1=& Cyc_parseonly_r;
_temp553;}); _temp552;}); _temp549->f3=({ unsigned char* _temp550=(
unsigned char*)"\n        Stop after parsing"; struct _tagged_string _temp551;
_temp551.curr= _temp550; _temp551.base= _temp550; _temp551.last_plus_one=
_temp550 + 28; _temp551;}); _temp549;}); struct _tuple11* _temp437=({ struct
_tuple11* _temp542=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp542->f1=({ unsigned char* _temp547=( unsigned char*)"-tc"; struct
_tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 4; _temp548;}); _temp542->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp545=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp545[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp546; _temp546.tag= Cyc_Arg_Set_spec; _temp546.f1=& Cyc_tc_r; _temp546;});
_temp545;}); _temp542->f3=({ unsigned char* _temp543=( unsigned char*)"   Stop after type checking";
struct _tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 28; _temp544;}); _temp542;}); struct _tuple11*
_temp438=({ struct _tuple11* _temp535=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp535->f1=({ unsigned char* _temp540=( unsigned char*)"-toc";
struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 5; _temp541;}); _temp535->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp538=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp538[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp539; _temp539.tag= Cyc_Arg_Set_spec; _temp539.f1=& Cyc_toc_r; _temp539;});
_temp538;}); _temp535->f3=({ unsigned char* _temp536=( unsigned char*)"  Stop after translation to C";
struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 30; _temp537;}); _temp535;}); struct _tuple11*
_temp439=({ struct _tuple11* _temp528=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp528->f1=({ unsigned char* _temp533=( unsigned char*)"-ic";
struct _tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 4; _temp534;}); _temp528->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp531=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp531[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp532; _temp532.tag= Cyc_Arg_Set_spec; _temp532.f1=& Cyc_ic_r; _temp532;});
_temp531;}); _temp528->f3=({ unsigned char* _temp529=( unsigned char*)"   Activate the link-checker";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 29; _temp530;}); _temp528;}); struct _tuple11*
_temp440=({ struct _tuple11* _temp521=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp521->f1=({ unsigned char* _temp526=( unsigned char*)"-pp";
struct _tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 4; _temp527;}); _temp521->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp524=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp524[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp525; _temp525.tag= Cyc_Arg_Set_spec; _temp525.f1=& Cyc_pp_r; _temp525;});
_temp524;}); _temp521->f3=({ unsigned char* _temp522=( unsigned char*)"   Pretty print";
struct _tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 16; _temp523;}); _temp521;}); struct _tuple11*
_temp441=({ struct _tuple11* _temp514=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp514->f1=({ unsigned char* _temp519=( unsigned char*)"-up";
struct _tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 4; _temp520;}); _temp514->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp517=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp517[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp518; _temp518.tag= Cyc_Arg_Clear_spec; _temp518.f1=&
Cyc_pp_r; _temp518;}); _temp517;}); _temp514->f3=({ unsigned char* _temp515=(
unsigned char*)"   Ugly print"; struct _tagged_string _temp516; _temp516.curr=
_temp515; _temp516.base= _temp515; _temp516.last_plus_one= _temp515 + 14;
_temp516;}); _temp514;}); struct _tuple11* _temp442=({ struct _tuple11* _temp507=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp507->f1=({
unsigned char* _temp512=( unsigned char*)"-tovc"; struct _tagged_string _temp513;
_temp513.curr= _temp512; _temp513.base= _temp512; _temp513.last_plus_one=
_temp512 + 6; _temp513;}); _temp507->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp510=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp510[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp511; _temp511.tag= Cyc_Arg_Set_spec;
_temp511.f1=& Cyc_tovc_r; _temp511;}); _temp510;}); _temp507->f3=({
unsigned char* _temp508=( unsigned char*)" Avoid gcc extensions in C output";
struct _tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 34; _temp509;}); _temp507;}); struct _tuple11*
_temp443=({ struct _tuple11* _temp500=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp500->f1=({ unsigned char* _temp505=( unsigned char*)"-save-temps";
struct _tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 12; _temp506;}); _temp500->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp503=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp503[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp504; _temp504.tag= Cyc_Arg_Unit_spec; _temp504.f1= Cyc_set_save_temps;
_temp504;}); _temp503;}); _temp500->f3=({ unsigned char* _temp501=(
unsigned char*)"\n        Don't delete temporary files"; struct _tagged_string
_temp502; _temp502.curr= _temp501; _temp502.base= _temp501; _temp502.last_plus_one=
_temp501 + 38; _temp502;}); _temp500;}); struct _tuple11* _temp444=({ struct
_tuple11* _temp493=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp493->f1=({ unsigned char* _temp498=( unsigned char*)"-save-c"; struct
_tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 8; _temp499;}); _temp493->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp496=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp496[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp497; _temp497.tag= Cyc_Arg_Set_spec; _temp497.f1=& Cyc_save_c_r; _temp497;});
_temp496;}); _temp493->f3=({ unsigned char* _temp494=( unsigned char*)"\n        Don't delete temporary C files";
struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 40; _temp495;}); _temp493;}); struct _tuple11*
_temp445=({ struct _tuple11* _temp486=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp486->f1=({ unsigned char* _temp491=( unsigned char*)"-nocyc";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 7; _temp492;}); _temp486->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp489=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp489[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp490; _temp490.tag= Cyc_Arg_Clear_spec; _temp490.f1=&
Cyc_add_cyc_namespace_r; _temp490;}); _temp489;}); _temp486->f3=({ unsigned char*
_temp487=( unsigned char*)"Don't add implicit namespace Cyc"; struct
_tagged_string _temp488; _temp488.curr= _temp487; _temp488.base= _temp487;
_temp488.last_plus_one= _temp487 + 33; _temp488;}); _temp486;}); struct _tuple11*
_temp446=({ struct _tuple11* _temp479=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp479->f1=({ unsigned char* _temp484=( unsigned char*)"-noremoveunused";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 16; _temp485;}); _temp479->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp482=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp482[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp483; _temp483.tag= Cyc_Arg_Set_spec; _temp483.f1=& Cyc_noshake_r; _temp483;});
_temp482;}); _temp479->f3=({ unsigned char* _temp480=( unsigned char*)"\n        Don't remove externed variables that aren't used";
struct _tagged_string _temp481; _temp481.curr= _temp480; _temp481.base= _temp480;
_temp481.last_plus_one= _temp480 + 58; _temp481;}); _temp479;}); struct _tuple11*
_temp447=({ struct _tuple11* _temp472=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp472->f1=({ unsigned char* _temp477=( unsigned char*)"-noexpandtypedefs";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 18; _temp478;}); _temp472->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp475=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp475[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp476; _temp476.tag= Cyc_Arg_Set_spec; _temp476.f1=& Cyc_noexpand_r; _temp476;});
_temp475;}); _temp472->f3=({ unsigned char* _temp473=( unsigned char*)"\n        Don't expand typedefs in pretty printing";
struct _tagged_string _temp474; _temp474.curr= _temp473; _temp474.base= _temp473;
_temp474.last_plus_one= _temp473 + 50; _temp474;}); _temp472;}); struct _tuple11*
_temp448=({ struct _tuple11* _temp465=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp465->f1=({ unsigned char* _temp470=( unsigned char*)"-printalltvars";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 15; _temp471;}); _temp465->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp468=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp468[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp469; _temp469.tag= Cyc_Arg_Set_spec; _temp469.f1=& Cyc_print_all_tvars_r;
_temp469;}); _temp468;}); _temp465->f3=({ unsigned char* _temp466=(
unsigned char*)"\n        Print all type variables (even implicit default effects)";
struct _tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 66; _temp467;}); _temp465;}); struct _tuple11*
_temp449=({ struct _tuple11* _temp458=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp458->f1=({ unsigned char* _temp463=( unsigned char*)"-printallkinds";
struct _tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 15; _temp464;}); _temp458->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp461=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp461[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp462; _temp462.tag= Cyc_Arg_Set_spec; _temp462.f1=& Cyc_print_all_kinds_r;
_temp462;}); _temp461;}); _temp458->f3=({ unsigned char* _temp459=(
unsigned char*)"\n        Always print kinds of type variables"; struct
_tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 46; _temp460;}); _temp458;}); struct _tuple11*
_temp450=({ struct _tuple11* _temp451=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp451->f1=({ unsigned char* _temp456=( unsigned char*)"-printfullevars";
struct _tagged_string _temp457; _temp457.curr= _temp456; _temp457.base= _temp456;
_temp457.last_plus_one= _temp456 + 16; _temp457;}); _temp451->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp454=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp454[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp455; _temp455.tag= Cyc_Arg_Set_spec; _temp455.f1=& Cyc_print_full_evars_r;
_temp455;}); _temp454;}); _temp451->f3=({ unsigned char* _temp452=(
unsigned char*)"\n       Print full information for evars (type debugging)";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 58; _temp453;}); _temp451;}); struct _tuple11*
_temp414[ 35u]={ _temp416, _temp417, _temp418, _temp419, _temp420, _temp421,
_temp422, _temp423, _temp424, _temp425, _temp426, _temp427, _temp428, _temp429,
_temp430, _temp431, _temp432, _temp433, _temp434, _temp435, _temp436, _temp437,
_temp438, _temp439, _temp440, _temp441, _temp442, _temp443, _temp444, _temp445,
_temp446, _temp447, _temp448, _temp449, _temp450}; struct _tagged_ptr4 _temp415={
_temp414, _temp414, _temp414 + 35u}; _temp413( _temp415);}); Cyc_Arg_parse(
options, Cyc_add_other,({ unsigned char* _temp306=( unsigned char*)"Options:";
struct _tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 9; _temp307;}), argv);{ struct Cyc_List_List*
_temp308=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
Cyc_cyclone_files); goto _LL309; _LL309: for( 0; _temp308 != 0; _temp308=((
struct Cyc_List_List*) _check_null( _temp308))->tl){ Cyc_process_file(*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( _temp308))->hd)); if( Cyc_compile_failure){
return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)? 1: Cyc_tc_r)? 1:
Cyc_toc_r){ return 0;} if( Cyc_ccargs == 0){ return 0;}{ struct _tagged_string
cyclone_exec_prefix=( struct _tagged_string) Cyc_Stdlib_getenv(({ unsigned char*
_temp411=( unsigned char*)"CYCLONE_EXEC_PREFIX"; struct _tagged_string _temp412;
_temp412.curr= _temp411; _temp412.base= _temp411; _temp412.last_plus_one=
_temp411 + 20; _temp412;})); if( cyclone_exec_prefix.curr !=(( struct
_tagged_string)({ struct _tagged_string _temp310={ 0, 0, 0}; _temp310;})).curr){
Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp311=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp311->hd=( void*)({ struct
_tagged_string* _temp312=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp312[ 0]= cyclone_exec_prefix; _temp312;}); _temp311->tl=
Cyc_cyclone_exec_prefix_path; _temp311;});} Cyc_cyclone_exec_prefix_path=({
struct Cyc_List_List* _temp313=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp313->hd=( void*)({ struct _tagged_string* _temp314=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp314[ 0]=({
unsigned char* _temp315=( unsigned char*)"c:/cyclone/bin/cyc-lib"; struct
_tagged_string _temp316; _temp316.curr= _temp315; _temp316.base= _temp315;
_temp316.last_plus_one= _temp315 + 23; _temp316;}); _temp314;}); _temp313->tl=
Cyc_cyclone_exec_prefix_path; _temp313;}); Cyc_cyclone_exec_prefix_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_exec_prefix_path);{
struct _tagged_string _temp319= Cyc_do_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp317=( unsigned char*)"include/cyc_include.h"; struct
_tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 22; _temp318;})); goto _LL320; _LL320: Cyc_ccargs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{
struct _tagged_string _temp323= Cyc_String_str_sepstr( Cyc_ccargs,({
unsigned char* _temp321=( unsigned char*)" "; struct _tagged_string _temp322;
_temp322.curr= _temp321; _temp322.base= _temp321; _temp322.last_plus_one=
_temp321 + 2; _temp322;})); goto _LL324; _LL324: Cyc_libargs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_libargs);{ struct _tagged_string
_temp331= Cyc_String_str_sepstr(({ struct Cyc_List_List* _temp325=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp325->hd=( void*)({ struct
_tagged_string* _temp326=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp326[ 0]=({ unsigned char* _temp327=( unsigned char*)"";
struct _tagged_string _temp328; _temp328.curr= _temp327; _temp328.base= _temp327;
_temp328.last_plus_one= _temp327 + 1; _temp328;}); _temp326;}); _temp325->tl=
Cyc_libargs; _temp325;}),({ unsigned char* _temp329=( unsigned char*)" "; struct
_tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 2; _temp330;})); goto _LL332; _LL332: {
struct Cyc_List_List* stdlib; struct _tagged_string stdlib_string; int _temp337=((
Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)? 1: Cyc_Filename_check_suffix(*((
struct _tagged_string*) _check_null( Cyc_output_file)),({ unsigned char*
_temp333=( unsigned char*)".a"; struct _tagged_string _temp334; _temp334.curr=
_temp333; _temp334.base= _temp333; _temp334.last_plus_one= _temp333 + 3;
_temp334;})))? 1: Cyc_Filename_check_suffix(*(( struct _tagged_string*)
_check_null( Cyc_output_file)),({ unsigned char* _temp335=( unsigned char*)".lib";
struct _tagged_string _temp336; _temp336.curr= _temp335; _temp336.base= _temp335;
_temp336.last_plus_one= _temp335 + 5; _temp336;})); goto _LL338; _LL338: if(
_temp337){ stdlib= 0; stdlib_string=({ unsigned char* _temp339=( unsigned char*)"";
struct _tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 1; _temp340;});} else{ struct _tagged_string
_temp343= Cyc_do_find( Cyc_cyclone_exec_prefix_path,({ unsigned char* _temp341=(
unsigned char*)"cyclib.a"; struct _tagged_string _temp342; _temp342.curr=
_temp341; _temp342.base= _temp341; _temp342.last_plus_one= _temp341 + 9;
_temp342;})); goto _LL344; _LL344: { struct _tagged_string _temp347= Cyc_do_find(
Cyc_cyclone_exec_prefix_path,({ unsigned char* _temp345=( unsigned char*)"gc.a";
struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 5; _temp346;})); goto _LL348; _LL348: stdlib=({
struct Cyc_List_List*(* _temp349)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tagged_string* _temp352=({ struct
_tagged_string* _temp353=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp353[ 0]= _temp343; _temp353;}); struct _tagged_string*
_temp350[ 1u]={ _temp352}; struct _tagged_ptr3 _temp351={ _temp350, _temp350,
_temp350 + 1u}; _temp349( _temp351);}); stdlib_string=( struct _tagged_string)({
struct _tagged_string _temp354= _temp343; struct _tagged_string _temp355=
_temp347; xprintf(" %.*s %.*s", _temp354.last_plus_one - _temp354.curr, _temp354.curr,
_temp355.last_plus_one - _temp355.curr, _temp355.curr);});}} if( Cyc_ic_r){
struct _handler_cons _temp356; _push_handler(& _temp356);{ int _temp358= 0; if(
setjmp( _temp356.handler)){ _temp358= 1;} if( ! _temp358){ Cyc_ccargs=(( struct
Cyc_List_List*(*)( int(* f)( struct _tagged_string*), struct Cyc_List_List* l))
Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=(( struct Cyc_List_List*(*)(
int(* f)( struct _tagged_string*), struct Cyc_List_List* l)) Cyc_List_filter)(
Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List* _temp359=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( stdlib,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
Cyc_ccargs, Cyc_libargs)); goto _LL360; _LL360: if( ! _temp337){ _temp359=({
struct Cyc_List_List* _temp361=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp361->hd=( void*) Cyc_final_strptr; _temp361->tl=
_temp359; _temp361;});}{ struct Cyc_Interface_I* _temp362=(( struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(* get)( struct _tagged_string*), struct Cyc_List_List*
la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)( Cyc_read_cycio,
_temp359, _temp359); goto _LL363; _LL363: if( _temp362 == 0){ fprintf(
sfile_to_file( Cyc_Stdio_stderr),"Error: interfaces incompatible\n"); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp364= 1; _npop_handler( 0u); return _temp364;}}
if( _temp337){ if( Cyc_output_file != 0){ struct _tagged_string _temp366=({
struct _tagged_string _temp365=( struct _tagged_string) Cyc_Filename_chop_extension(*((
struct _tagged_string*) _check_null( Cyc_output_file))); xprintf("%.*s.cycio",
_temp365.last_plus_one - _temp365.curr, _temp365.curr);}); goto _LL367; _LL367: {
struct Cyc_Stdio___sFILE* _temp372= Cyc_try_file_open(( struct _tagged_string)
_temp366,({ unsigned char* _temp368=( unsigned char*)"w"; struct _tagged_string
_temp369; _temp369.curr= _temp368; _temp369.base= _temp368; _temp369.last_plus_one=
_temp368 + 2; _temp369;}),({ unsigned char* _temp370=( unsigned char*)"interface object file";
struct _tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 22; _temp371;})); goto _LL373; _LL373: if(
_temp372 == 0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{ int _temp374= 1;
_npop_handler( 0u); return _temp374;}} Cyc_Interface_save(( struct Cyc_Interface_I*)
_check_null( _temp362),( struct Cyc_Stdio___sFILE*) _check_null( _temp372)); Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( _temp372));}}} else{ if( ! Cyc_Interface_is_subinterface(
Cyc_Interface_empty(),( struct Cyc_Interface_I*) _check_null( _temp362),({
struct _tuple9* _temp375=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp375->f1=({ unsigned char* _temp378=( unsigned char*)"empty interface";
struct _tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 16; _temp379;}); _temp375->f2=({
unsigned char* _temp376=( unsigned char*)"global interface"; struct
_tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 17; _temp377;}); _temp375;}))){ fprintf(
sfile_to_file( Cyc_Stdio_stderr),"Error: some objects are still undefined\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles();{ int _temp380= 1; _npop_handler( 0u);
return _temp380;}}}}}; _pop_handler();} else{ void* _temp357=( void*)
_exn_thrown; void* _temp382= _temp357; struct _tagged_string _temp394; struct
_tagged_string _temp396; _LL384: if(*(( void**) _temp382) == Cyc_Core_Impossible){
_LL395: _temp394=(( struct Cyc_Core_Impossible_struct*) _temp382)->f1; goto
_LL385;} else{ goto _LL386;} _LL386: if( _temp382 == Cyc_Dict_Absent){ goto
_LL387;} else{ goto _LL388;} _LL388: if(*(( void**) _temp382) == Cyc_Core_InvalidArg){
_LL397: _temp396=(( struct Cyc_Core_InvalidArg_struct*) _temp382)->f1; goto
_LL389;} else{ goto _LL390;} _LL390: goto _LL391; _LL392: goto _LL393; _LL385:({
struct _tagged_string _temp398= _temp394; printf("Exception Core::Impossible %.*s\n",
_temp398.last_plus_one - _temp398.curr, _temp398.curr);}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL387: printf("Exception Dict::Absent\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1; _LL389:({ struct
_tagged_string _temp399= _temp396; printf("Exception Core::InvalidArg %.*s\n",
_temp399.last_plus_one - _temp399.curr, _temp399.curr);}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL391: printf("Uncaught exception\n"); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL393:( void) _throw( _temp382); _LL383:;}}}{
struct _tagged_string _temp408=({ struct _tagged_string _temp403= _temp319;
struct _tagged_string _temp404= Cyc_output_file == 0?({ unsigned char* _temp400=(
unsigned char*)""; struct _tagged_string _temp401; _temp401.curr= _temp400;
_temp401.base= _temp400; _temp401.last_plus_one= _temp400 + 1; _temp401;}):(
struct _tagged_string)({ struct _tagged_string _temp402=*(( struct
_tagged_string*) _check_null( Cyc_output_file)); xprintf(" -o %.*s", _temp402.last_plus_one
- _temp402.curr, _temp402.curr);}); struct _tagged_string _temp405=( struct
_tagged_string) _temp323; struct _tagged_string _temp406= stdlib_string; struct
_tagged_string _temp407=( struct _tagged_string) _temp331; xprintf("gcc -include %.*s%.*s %.*s%.*s%.*s",
_temp403.last_plus_one - _temp403.curr, _temp403.curr, _temp404.last_plus_one -
_temp404.curr, _temp404.curr, _temp405.last_plus_one - _temp405.curr, _temp405.curr,
_temp406.last_plus_one - _temp406.curr, _temp406.curr, _temp407.last_plus_one -
_temp407.curr, _temp407.curr);}); goto _LL409; _LL409: if( Cyc_v_r){({ struct
_tagged_string _temp410=( struct _tagged_string) _temp408; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp410.last_plus_one - _temp410.curr,
_temp410.curr);});} if( system( string_to_Cstring(( struct _tagged_string)
_temp408)) != 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: C compiler failed\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} Cyc_remove_cfiles();
return Cyc_compile_failure? 1: 0;}}}}}}}