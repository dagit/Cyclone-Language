 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list( struct
_tagged_arr); extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern
struct Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* l);
extern unsigned char Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Arg_Error[ 10u]; static
const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{ int tag; void(*
f1)(); } ; static const int Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_FlagString_spec=
2; struct Cyc_Arg_FlagString_spec_struct{ int tag; void(* f1)( struct
_tagged_arr, struct _tagged_arr); } ; static const int Cyc_Arg_Set_spec= 3;
struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec=
4; struct Cyc_Arg_Clear_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_String_spec=
5; struct Cyc_Arg_String_spec_struct{ int tag; void(* f1)( struct _tagged_arr);
} ; static const int Cyc_Arg_Int_spec= 6; struct Cyc_Arg_Int_spec_struct{ int
tag; void(* f1)( int); } ; static const int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct
Cyc_List_List* specs, void(* anonfun)( struct _tagged_arr), struct _tagged_arr
errmsg, struct _tagged_arr args); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int
Cyc_Stdio_remove( struct _tagged_arr); extern int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_fopen( struct _tagged_arr _name, struct _tagged_arr _type); extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct
_tagged_arr fname, struct _tagged_arr mode); extern void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE*); struct Cyc_Time_tm{ int tm_sec; int tm_min; int
tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int tm_yday; int
tm_isdst; } ; extern unsigned int time( unsigned int* t); struct Cyc___sFILE;
struct Cyc__reent; struct Cyc__glue{ struct Cyc__glue* _next; int _niobs; struct
Cyc___sFILE* _iobs; } ; struct Cyc__Bigint{ struct Cyc__Bigint* _next; int _k;
int _maxwds; int _sign; int _wds; unsigned int _x[ 1u]; } ; struct Cyc__atexit{
struct Cyc__atexit* _next; int _ind; void(* _fns[ 32u])(); } ; struct Cyc___sbuf{
unsigned char* _base; int _size; } ; struct Cyc___sFILE{ unsigned char* _p; int
_r; int _w; short _flags; short _file; struct Cyc___sbuf _bf; int _lbfsize; void*
_cookie; int(* _read)( void* _cookie, unsigned char* _buf, int _n); int(* _write)(
void* _cookie, const unsigned char* _buf, int _n); int(* _seek)( void* _cookie,
int _offset, int _whence); int(* _close)( void* _cookie); struct Cyc___sbuf _ub;
unsigned char* _up; int _ur; unsigned char _ubuf[ 3u]; unsigned char _nbuf[ 1u];
struct Cyc___sbuf _lb; int _blksize; int _offset; struct Cyc__reent* _data; } ;
struct Cyc__reent_u1{ unsigned int _unused_rand; int _strtok_last; unsigned char
_asctime_buf[ 26u]; struct Cyc_Time_tm _localtime_buf; int _gamma_signgam;
unsigned long long _rand_next; } ; struct Cyc__reent_u2{ unsigned int _nextf[ 30u];
unsigned int _nmalloc[ 30u]; } ; union Cyc__reent_union{ struct Cyc__reent_u1
_reent; struct Cyc__reent_u2 _unused; } ; struct Cyc__reent{ int _errno; struct
Cyc___sFILE* _stdin; struct Cyc___sFILE* _stdout; struct Cyc___sFILE* _stderr;
int _inc; unsigned char _emergency[ 25u]; int _current_category; const
unsigned char* _current_locale; int __sdidinit; void(* __cleanup)( struct Cyc__reent*);
struct Cyc__Bigint* _result; int _result_k; struct Cyc__Bigint* _p5s; struct Cyc__Bigint**
_freelist; int _cvtlen; unsigned char* _cvtbuf; union Cyc__reent_union _new;
struct Cyc__atexit* _atexit; struct Cyc__atexit _atexit0; void(** _sig_func)(
int); struct Cyc__glue __sglue; struct Cyc___sFILE __sf[ 3u]; } ; extern struct
Cyc__reent* _impure_ptr; extern void _reclaim_reent( struct Cyc__reent*); struct
Cyc_Stdlib__Div{ int quot; int rem; } ; struct Cyc_Stdlib__Ldiv{ int quot; int
rem; } ; extern int __mb_cur_max; extern int abs( int); extern int atexit( void(*
__func)()); extern struct Cyc_Stdlib__Div div( int __numer, int __denom); extern
struct Cyc_Stdlib__Ldiv ldiv( int __numer, int __denom); extern int rand();
extern void srand( unsigned int __seed); extern int rand_r( unsigned int* __seed);
extern struct _tagged_arr Cyc_Stdlib_getenv( struct _tagged_arr); extern int Cyc_String_strlen(
struct _tagged_arr s); extern int Cyc_String_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); extern struct _tagged_arr Cyc_String_strconcat( struct
_tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_String_str_sepstr(
struct Cyc_List_List*, struct _tagged_arr); extern struct _tagged_arr Cyc_String_substring(
struct _tagged_arr, int ofs, int n); extern struct _tagged_arr Cyc_String_strchr(
struct _tagged_arr s, unsigned char c); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct _tagged_arr Cyc_Filename_concat( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension( struct
_tagged_arr); extern int Cyc_Filename_check_suffix( struct _tagged_arr, struct
_tagged_arr); struct Cyc_Id_id; struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
struct _tagged_arr); struct Cyc_Position_Segment; static const int Cyc_Position_Lex=
0; static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
struct Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; extern int Cyc_Position_error_p(); struct _tuple0{ void* f1; struct
_tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref;
struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
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
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Nocommon_att= 6;
static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
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
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[
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
struct _tagged_arr text; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct
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
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
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
struct _tuple9{ struct _tagged_arr f1; struct _tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple9* info);
extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo); extern
struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); extern
void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Stdio___sFILE*);
extern void Cyc_Lex_lex_init(); static int Cyc_pp_r= 0; static int Cyc_noexpand_r=
0; static int Cyc_noshake_r= 0; static int Cyc_stop_after_cpp_r= 0; static int
Cyc_parseonly_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0; static int
Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r= 0; static int Cyc_stop_after_asmfile_r=
0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0; static int Cyc_save_temps_r=
0; static int Cyc_save_c_r= 0; static int Cyc_nogc_r= 0; static int Cyc_add_cyc_namespace_r=
1; static int Cyc_print_full_evars_r= 0; static int Cyc_print_all_tvars_r= 0;
static int Cyc_print_all_kinds_r= 0; static struct _tagged_arr* Cyc_output_file=
0; static void Cyc_set_output_file( struct _tagged_arr s){ Cyc_output_file=({
struct _tagged_arr* _temp0=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp0[ 0]= s; _temp0;});} static struct Cyc_List_List* Cyc_cppargs=
0; static void Cyc_add_cpparg( struct _tagged_arr s){ Cyc_cppargs=({ struct Cyc_List_List*
_temp1=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1->hd=( void*)({ struct _tagged_arr* _temp2=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp2[ 0]= s; _temp2;}); _temp1->tl=
Cyc_cppargs; _temp1;});} static int Cyc_is_cyclone_sourcefile( struct
_tagged_arr s){ int _temp3= Cyc_String_strlen( s); if( _temp3 <= 4){ return 0;}
else{ return Cyc_String_strcmp( _tagged_arr_plus( s, sizeof( unsigned char),
_temp3 - 4), _tag_arr(".cyc", sizeof( unsigned char), 5u)) == 0;}} static struct
Cyc_List_List* Cyc_cyclone_exec_prefix_path= 0; static void Cyc_add_cyclone_exec_prefix_path(
struct _tagged_arr s){ int _temp4= Cyc_String_strlen( s); if( _temp4 <= 2){
return;}{ struct _tagged_arr _temp5=( struct _tagged_arr) Cyc_String_substring(
s, 2, _temp4 - 2); Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp6=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6->hd=(
void*)({ struct _tagged_arr* _temp7=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp7[ 0]= _temp5; _temp7;}); _temp6->tl= Cyc_cyclone_exec_prefix_path;
_temp6;});}} static struct Cyc_List_List* Cyc_ccargs= 0; static void Cyc_add_ccarg(
struct _tagged_arr s){ Cyc_ccargs=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp8->hd=( void*)({ struct
_tagged_arr* _temp9=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp9[ 0]= s; _temp9;}); _temp8->tl= Cyc_ccargs; _temp8;});} static void Cyc_add_ccarg2(
struct _tagged_arr flag, struct _tagged_arr arg){ Cyc_ccargs=({ struct Cyc_List_List*
_temp10=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp10->hd=( void*)({ struct _tagged_arr* _temp13=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp13[ 0]= arg; _temp13;}); _temp10->tl=({
struct Cyc_List_List* _temp11=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp11->hd=( void*)({ struct _tagged_arr* _temp12=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp12[ 0]= flag;
_temp12;}); _temp11->tl= Cyc_ccargs; _temp11;}); _temp10;});} static struct Cyc_List_List*
Cyc_libargs= 0; static void Cyc_add_libarg( struct _tagged_arr s){ Cyc_libargs=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)({ struct _tagged_arr* _temp15=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp15[ 0]= s; _temp15;});
_temp14->tl= Cyc_libargs; _temp14;});} static void Cyc_set_save_temps(){ Cyc_save_temps_r=
1; Cyc_add_ccarg( _tag_arr("-save-temps", sizeof( unsigned char), 12u));} static
int Cyc_produce_dependencies= 0; static void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r=
1; Cyc_produce_dependencies= 1; Cyc_add_cpparg( _tag_arr("-M", sizeof(
unsigned char), 3u));} static struct _tagged_arr* Cyc_dependencies_target= 0;
static void Cyc_set_dependencies_target( struct _tagged_arr s){ Cyc_dependencies_target=({
struct _tagged_arr* _temp16=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp16[ 0]= s; _temp16;});} static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r= 1; Cyc_add_ccarg( _tag_arr("-c", sizeof(
unsigned char), 3u));} static void Cyc_set_nocyc(){ Cyc_add_cyc_namespace_r= 0;
Cyc_add_ccarg( _tag_arr("-D NO_CYC_PREFIX", sizeof( unsigned char), 17u));}
static void Cyc_set_stop_after_asmfile(){ Cyc_stop_after_asmfile_r= 1; Cyc_add_ccarg(
_tag_arr("-S", sizeof( unsigned char), 3u));} static struct Cyc_List_List* Cyc_cyclone_files=
0; static void Cyc_add_other( struct _tagged_arr s){ if( Cyc_is_cyclone_sourcefile(
s)){ Cyc_cyclone_files=({ struct Cyc_List_List* _temp17=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp17->hd=( void*)({ struct
_tagged_arr* _temp18=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp18[ 0]= s; _temp18;}); _temp17->tl= Cyc_cyclone_files;
_temp17;});{ struct _tagged_arr _temp19= Cyc_Filename_chop_extension( s); struct
_tagged_arr _temp20= Cyc_String_strconcat(( struct _tagged_arr) _temp19,
_tag_arr(".c", sizeof( unsigned char), 3u)); Cyc_add_ccarg(( struct _tagged_arr)
_temp20);}} else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct
_tagged_arr s){ if( Cyc_save_temps_r){ return;} else{ Cyc_Stdio_remove( s);}}
int Cyc_compile_failure= 0; struct Cyc_Stdio___sFILE* Cyc_try_file_open( struct
_tagged_arr filename, struct _tagged_arr mode, struct _tagged_arr msg_part){
struct _handler_cons _temp21; _push_handler(& _temp21);{ int _temp23= 0; if(
setjmp( _temp21.handler)){ _temp23= 1;} if( ! _temp23){{ struct Cyc_Stdio___sFILE*
_temp24=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open( filename, mode);
_npop_handler( 0u); return _temp24;}; _pop_handler();} else{ void* _temp22=(
void*) _exn_thrown; void* _temp26= _temp22; _LL28: goto _LL29; _LL30: goto _LL31;
_LL29: Cyc_compile_failure= 1;({ struct _tagged_arr _temp32= msg_part; struct
_tagged_arr _temp33= filename; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\nError: couldn't open %.*s %.*s\n",
_get_arr_size( _temp32, 1u), _temp32.curr, _get_arr_size( _temp33, 1u), _temp33.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); return 0; _LL31:(
void) _throw( _temp26); _LL27:;}}} struct Cyc_List_List* Cyc_do_stage( struct
_tagged_arr stage_name, struct Cyc_List_List*(* f)( void*, struct Cyc_List_List*),
void* env, struct Cyc_List_List* tds){ struct _tagged_arr exn_string= _tag_arr("",
sizeof( unsigned char), 1u); struct _tagged_arr explain_string= _tag_arr("",
sizeof( unsigned char), 1u); int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct*
_temp59=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp59[ 0]=({ struct Cyc_Core_Impossible_struct _temp60; _temp60.tag= Cyc_Core_Impossible;
_temp60.f1= _tag_arr("", sizeof( unsigned char), 1u); _temp60;}); _temp59;});
struct Cyc_List_List* _temp34= 0;{ struct _handler_cons _temp35; _push_handler(&
_temp35);{ int _temp37= 0; if( setjmp( _temp35.handler)){ _temp37= 1;} if( !
_temp37){ _temp34= f( env, tds);; _pop_handler();} else{ void* _temp36=( void*)
_exn_thrown; void* _temp39= _temp36; struct _tagged_arr _temp51; struct
_tagged_arr _temp53; _LL41: if(*(( void**) _temp39) == Cyc_Core_Impossible){
_LL52: _temp51=(( struct Cyc_Core_Impossible_struct*) _temp39)->f1; goto _LL42;}
else{ goto _LL43;} _LL43: if( _temp39 == Cyc_Dict_Absent){ goto _LL44;} else{
goto _LL45;} _LL45: if(*(( void**) _temp39) == Cyc_Core_InvalidArg){ _LL54:
_temp53=(( struct Cyc_Core_InvalidArg_struct*) _temp39)->f1; goto _LL46;} else{
goto _LL47;} _LL47: goto _LL48; _LL49: goto _LL50; _LL42: exn_string= _tag_arr("Exception Core::Impossible",
sizeof( unsigned char), 27u); explain_string= _temp51; goto _LL40; _LL44:
exn_string= _tag_arr("Exception Dict::Absent", sizeof( unsigned char), 23u);
goto _LL40; _LL46: exn_string= _tag_arr("Exception Core::InvalidArg", sizeof(
unsigned char), 27u); explain_string= _temp53; goto _LL40; _LL48: ex= _temp39;
other_exn= 1; exn_string= _tag_arr("Uncaught exception", sizeof( unsigned char),
19u); goto _LL40; _LL50:( void) _throw( _temp39); _LL40:;}}} if( Cyc_Position_error_p()){
Cyc_compile_failure= 1;} if( Cyc_String_strcmp( exn_string, _tag_arr("", sizeof(
unsigned char), 1u)) != 0){ Cyc_compile_failure= 1;({ struct _tagged_arr _temp55=
exn_string; struct _tagged_arr _temp56= stage_name; struct _tagged_arr _temp57=
explain_string; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\n%.*s thrown during %.*s: %.*s",
_get_arr_size( _temp55, 1u), _temp55.curr, _get_arr_size( _temp56, 1u), _temp56.curr,
_get_arr_size( _temp57, 1u), _temp57.curr);});} if( Cyc_compile_failure){
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\nCOMPILATION FAILED!\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); if( other_exn){( int) _throw( ex);}
return _temp34;} else{ if( Cyc_v_r){({ struct _tagged_arr _temp58= stage_name;
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s completed.\n", _get_arr_size(
_temp58, 1u), _temp58.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return _temp34;}} return _temp34;} struct Cyc_List_List* Cyc_do_parse(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp61= Cyc_Parse_parse_file( f); Cyc_Lex_lex_init();
return _temp61;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds); if( ! Cyc_noshake_r){
tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck(
int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds);
return tds;} struct _tuple10{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Stdio___sFILE*
f2; struct Cyc_Stdio___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple10* params, struct Cyc_List_List* tds){ struct _tuple10 _temp64;
struct Cyc_Stdio___sFILE* _temp65; struct Cyc_Stdio___sFILE* _temp67; struct Cyc_Tcenv_Tenv*
_temp69; struct _tuple10* _temp62= params; _temp64=* _temp62; _LL70: _temp69=
_temp64.f1; goto _LL68; _LL68: _temp67= _temp64.f2; goto _LL66; _LL66: _temp65=
_temp64.f3; goto _LL63; _LL63: { struct Cyc_Interface_I* _temp71= Cyc_Interface_extract(
_temp69->ae); if( _temp67 == 0){ Cyc_Interface_save( _temp71, _temp65);} else{
struct Cyc_Interface_I* _temp72= Cyc_Interface_parse(( struct Cyc_Stdio___sFILE*)
_check_null( _temp67)); if( ! Cyc_Interface_is_subinterface( _temp72, _temp71,({
struct _tuple9* _temp73=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp73->f1= _tag_arr("written interface", sizeof( unsigned char), 18u); _temp73->f2=
_tag_arr("maximal interface", sizeof( unsigned char), 18u); _temp73;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp72, _temp65);}} return tds;}} struct Cyc_List_List*
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
_check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}} static struct Cyc_List_List*
Cyc_split_by_char( struct _tagged_arr s, unsigned char c){ if( s.curr ==((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ return 0;}{ struct Cyc_List_List*
_temp74= 0; int _temp75= Cyc_String_strlen( s); while( _temp75 > 0) { struct
_tagged_arr _temp76= Cyc_String_strchr( s, c); if( _temp76.curr ==(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ _temp74=({ struct Cyc_List_List*
_temp77=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp77->hd=( void*)({ struct _tagged_arr* _temp78=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp78[ 0]= s; _temp78;}); _temp77->tl=
_temp74; _temp77;}); break;} else{ _temp74=({ struct Cyc_List_List* _temp79=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp79->hd=(
void*)({ struct _tagged_arr* _temp80=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp80[ 0]=( struct _tagged_arr) Cyc_String_substring( s,
0,(( struct _tagged_arr) _temp76).curr - s.curr); _temp80;}); _temp79->tl=
_temp74; _temp79;}); _temp75 -=(( struct _tagged_arr) _temp76).curr - s.curr; s=
_tagged_arr_plus( _temp76, sizeof( unsigned char), 1);}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp74);}} int Cyc_file_exists(
struct _tagged_arr file){ struct Cyc_Stdio___sFILE* f= 0;{ struct _handler_cons
_temp81; _push_handler(& _temp81);{ int _temp83= 0; if( setjmp( _temp81.handler)){
_temp83= 1;} if( ! _temp83){ f=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open(
file, _tag_arr("r", sizeof( unsigned char), 2u));; _pop_handler();} else{ void*
_temp82=( void*) _exn_thrown; void* _temp85= _temp82; _LL87: goto _LL88; _LL89:
goto _LL90; _LL88: goto _LL86; _LL90:( void) _throw( _temp85); _LL86:;}}} if( f
== 0){ return 0;} else{ Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f)); return 1;}} struct _tagged_arr* Cyc_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ if( file.curr ==(( struct _tagged_arr) _tag_arr(
0u, 0u, 0u)).curr){ return 0;} for( 0; dirs != 0; dirs=(( struct Cyc_List_List*)
_check_null( dirs))->tl){ struct _tagged_arr _temp91=*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( dirs))->hd); if( _temp91.curr ==(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: Cyc_String_strlen( _temp91) == 0){
continue;}{ struct _tagged_arr s=( struct _tagged_arr) Cyc_Filename_concat(
_temp91, file); if( Cyc_file_exists( s)){ return({ struct _tagged_arr* _temp92=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp92[ 0]= s;
_temp92;});}}} return 0;} struct _tagged_arr Cyc_do_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ struct _tagged_arr* _temp93= Cyc_find( dirs,
file); if( _temp93 == 0){({ struct _tagged_arr _temp94= file; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file %.*s",
_get_arr_size( _temp94, 1u), _temp94.curr);}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} return*(( struct _tagged_arr*) _check_null( _temp93));} void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp95= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp96= Cyc_String_strconcat(( struct _tagged_arr)
_temp95, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct _tagged_arr
_temp97= Cyc_String_strconcat(( struct _tagged_arr) _temp95, _tag_arr(".cyci",
sizeof( unsigned char), 6u)); struct _tagged_arr _temp98= Cyc_String_strconcat((
struct _tagged_arr) _temp95, _tag_arr(".cycio", sizeof( unsigned char), 7u));
struct _tagged_arr _temp99= Cyc_String_strconcat(( struct _tagged_arr) _temp95,
_tag_arr(".c", sizeof( unsigned char), 3u)); if( Cyc_v_r){({ struct _tagged_arr
_temp100= filename; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"Compiling %.*s\n",
_get_arr_size( _temp100, 1u), _temp100.curr);});}{ struct Cyc_Stdio___sFILE* f0=
Cyc_try_file_open( filename, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("input file",
sizeof( unsigned char), 11u)); if( Cyc_compile_failure){ return;} Cyc_Stdio_fclose((
struct Cyc_Stdio___sFILE*) _check_null( f0));{ struct _tagged_arr _temp101= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp144=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp144->hd=( void*) _init_tag_arr(( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp144->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
Cyc_cppargs); _temp144;}), _tag_arr(" ", sizeof( unsigned char), 2u)); struct
_tagged_arr stdinc_string= _tag_arr(" -Ic:/cyclone/include", sizeof(
unsigned char), 22u); struct _tagged_arr _temp102= Cyc_Stdlib_getenv( _tag_arr("CYCLONE_INCLUDE_PATH",
sizeof( unsigned char), 21u)); if( _temp102.curr !=( _tag_arr( 0u, 0u, 0u)).curr){
struct Cyc_List_List* _temp103= Cyc_split_by_char(( struct _tagged_arr) _temp102,':');
struct _tagged_arr _temp104= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp107=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp107->hd=( void*) _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)),"", sizeof( unsigned char), 1u); _temp107->tl= _temp103;
_temp107;}), _tag_arr(" -I", sizeof( unsigned char), 4u)); stdinc_string=(
struct _tagged_arr)({ struct _tagged_arr _temp105=( struct _tagged_arr) _temp104;
struct _tagged_arr _temp106= stdinc_string; xprintf("%.*s%.*s", _get_arr_size(
_temp105, 1u), _temp105.curr, _get_arr_size( _temp106, 1u), _temp106.curr);});}{
struct _tagged_arr ofile_string; if( Cyc_stop_after_cpp_r){ if( Cyc_output_file
!= 0){ ofile_string=( struct _tagged_arr)({ struct _tagged_arr _temp108=*((
struct _tagged_arr*) _check_null( Cyc_output_file)); xprintf(" > %.*s",
_get_arr_size( _temp108, 1u), _temp108.curr);});} else{ ofile_string= _tag_arr("",
sizeof( unsigned char), 1u);}} else{ ofile_string=( struct _tagged_arr)({ struct
_tagged_arr _temp109=( struct _tagged_arr) _temp96; xprintf(" > %.*s",
_get_arr_size( _temp109, 1u), _temp109.curr);});}{ struct _tagged_arr
fixup_string; if( Cyc_produce_dependencies){ if( Cyc_dependencies_target == 0){
fixup_string= _tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'", sizeof(
unsigned char), 35u);} else{ fixup_string=( struct _tagged_arr)({ struct
_tagged_arr _temp110=*(( struct _tagged_arr*) _check_null( Cyc_dependencies_target));
xprintf(" | sed 's/^.*\\.cyc\\.o:/%.*s:/'", _get_arr_size( _temp110, 1u),
_temp110.curr);});}} else{ fixup_string= _tag_arr("", sizeof( unsigned char), 1u);}{
struct _tagged_arr _temp111=({ struct _tagged_arr _temp139=( struct _tagged_arr)
_temp101; struct _tagged_arr _temp140= stdinc_string; struct _tagged_arr
_temp141= filename; struct _tagged_arr _temp142= fixup_string; struct
_tagged_arr _temp143= ofile_string; xprintf("gcc -x c -E -U__GNUC__ -nostdinc%.*s%.*s %.*s%.*s%.*s",
_get_arr_size( _temp139, 1u), _temp139.curr, _get_arr_size( _temp140, 1u),
_temp140.curr, _get_arr_size( _temp141, 1u), _temp141.curr, _get_arr_size(
_temp142, 1u), _temp142.curr, _get_arr_size( _temp143, 1u), _temp143.curr);});
if( Cyc_v_r){({ struct _tagged_arr _temp112=( struct _tagged_arr) _temp111;
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _get_arr_size( _temp112, 1u),
_temp112.curr);});} if( system( string_to_Cstring(( struct _tagged_arr) _temp111))
!= 0){ Cyc_compile_failure= 1; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\nError: preprocessing\n");
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp96);{ struct Cyc_Stdio___sFILE* in_file= Cyc_try_file_open((
struct _tagged_arr) _temp96, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("file",
sizeof( unsigned char), 5u)); if( Cyc_compile_failure){ return;}{ struct Cyc_List_List*
tds= 0;{ struct _handler_cons _temp113; _push_handler(& _temp113);{ int _temp115=
0; if( setjmp( _temp113.handler)){ _temp115= 1;} if( ! _temp115){ tds=(( struct
Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(* f)(
struct Cyc_Stdio___sFILE*, struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("parsing", sizeof(
unsigned char), 8u), Cyc_do_parse,( struct Cyc_Stdio___sFILE*) _check_null(
in_file), tds);; _pop_handler();} else{ void* _temp114=( void*) _exn_thrown;
void* _temp117= _temp114; _LL119: goto _LL120; _LL121: goto _LL122; _LL120: Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( in_file)); Cyc_remove_file(( struct
_tagged_arr) _temp96);( int) _throw( _temp117); _LL122:( void) _throw( _temp117);
_LL118:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*) _check_null(
in_file)); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr)
_temp96); return;}{ struct Cyc_Tcenv_Tenv* _temp123= Cyc_Tcenv_tc_init(); if(
Cyc_parseonly_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct
_tagged_arr stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds))
Cyc_do_stage)( _tag_arr("type checking", sizeof( unsigned char), 14u), Cyc_do_typecheck,
_temp123, tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr)
_temp96); return;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr
stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("control-flow checking",
sizeof( unsigned char), 22u), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){
Cyc_Tcutil_flush_warnings();} Cyc_remove_file(( struct _tagged_arr) _temp96);
if( Cyc_compile_failure){ return;} if( Cyc_ic_r){ struct Cyc_Stdio___sFILE*
inter_file= Cyc_Stdio_fopen(( struct _tagged_arr) _temp97, _tag_arr("r", sizeof(
unsigned char), 2u)); struct Cyc_Stdio___sFILE* inter_objfile= Cyc_try_file_open((
struct _tagged_arr) _temp98, _tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( inter_objfile == 0){ Cyc_compile_failure= 1;
return;} Cyc_Position_reset_position(( struct _tagged_arr) _temp97); tds=((
struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( struct _tuple10*, struct Cyc_List_List*), struct _tuple10* env, struct Cyc_List_List*
tds)) Cyc_do_stage)( _tag_arr("interface checking", sizeof( unsigned char), 19u),
Cyc_do_interface,({ struct _tuple10* _temp124=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp124->f1= _temp123; _temp124->f2= inter_file;
_temp124->f3=( struct Cyc_Stdio___sFILE*) _check_null( inter_objfile); _temp124;}),
tds); if( inter_file != 0){ Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( inter_file));} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( inter_objfile));} if( Cyc_tc_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("translation to C",
sizeof( unsigned char), 17u), Cyc_do_translate, 1, tds); if( Cyc_compile_failure){
return;} if( Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)( struct _tagged_arr
stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("post-pass to VC", sizeof(
unsigned char), 16u), Cyc_do_tovc, 1, tds);} if( Cyc_compile_failure){ return;}
PRINTC: if( tds != 0){ struct Cyc_Stdio___sFILE* out_file; if( Cyc_parseonly_r?
1: Cyc_tc_r){ if( Cyc_output_file != 0){ out_file= Cyc_try_file_open(*(( struct
_tagged_arr*) _check_null( Cyc_output_file)), _tag_arr("w", sizeof(
unsigned char), 2u), _tag_arr("output file", sizeof( unsigned char), 12u));}
else{ out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;}} else{ if( Cyc_toc_r?
Cyc_output_file != 0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));} else{ out_file= Cyc_try_file_open((
struct _tagged_arr) _temp99, _tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("output file",
sizeof( unsigned char), 12u));}} if( Cyc_compile_failure){ return;}{ struct
_handler_cons _temp125; _push_handler(& _temp125);{ int _temp127= 0; if( setjmp(
_temp125.handler)){ _temp127= 1;} if( ! _temp127){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)( _tag_arr("printing", sizeof( unsigned char), 9u), Cyc_do_print,(
struct Cyc_Stdio___sFILE*) _check_null( out_file), tds);; _pop_handler();} else{
void* _temp126=( void*) _exn_thrown; void* _temp129= _temp126; _LL131: goto
_LL132; _LL133: goto _LL134; _LL132: Cyc_compile_failure= 1; Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List*
_temp135=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp135->hd=( void*)({ struct _tagged_arr* _temp136=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp136[ 0]=( struct _tagged_arr)
_temp99; _temp136;}); _temp135->tl= Cyc_cfiles; _temp135;});( int) _throw(
_temp129); _LL134:( void) _throw( _temp129); _LL130:;}}} Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List*
_temp137=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp137->hd=( void*)({ struct _tagged_arr* _temp138=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp138[ 0]=( struct _tagged_arr)
_temp99; _temp138;}); _temp137->tl= Cyc_cfiles; _temp137;});}}}}}}}}}} static
unsigned char _temp145[ 8u]="<final>"; static struct _tagged_arr Cyc_final_str={
_temp145, _temp145, _temp145 + 8u}; static struct _tagged_arr* Cyc_final_strptr=&
Cyc_final_str; static struct Cyc_Interface_I* Cyc_read_cycio( struct _tagged_arr*
n){ if( n == Cyc_final_strptr){ return Cyc_Interface_final();}{ struct
_tagged_arr basename;{ struct _handler_cons _temp146; _push_handler(& _temp146);{
int _temp148= 0; if( setjmp( _temp146.handler)){ _temp148= 1;} if( ! _temp148){
basename=( struct _tagged_arr) Cyc_Filename_chop_extension(* n);; _pop_handler();}
else{ void* _temp147=( void*) _exn_thrown; void* _temp150= _temp147; struct
_tagged_arr _temp156; _LL152: if(*(( void**) _temp150) == Cyc_Core_InvalidArg){
_LL157: _temp156=(( struct Cyc_Core_InvalidArg_struct*) _temp150)->f1; goto
_LL153;} else{ goto _LL154;} _LL154: goto _LL155; _LL153: basename=* n; goto
_LL151; _LL155:( void) _throw( _temp150); _LL151:;}}}{ struct _tagged_arr
_temp158= Cyc_String_strconcat( basename, _tag_arr(".cycio", sizeof(
unsigned char), 7u)); struct Cyc_Stdio___sFILE* _temp159= Cyc_try_file_open((
struct _tagged_arr) _temp158, _tag_arr("r", sizeof( unsigned char), 2u),
_tag_arr("interface object file", sizeof( unsigned char), 22u)); if( _temp159 ==
0){ Cyc_compile_failure= 1; Cyc_remove_cfiles(); exit( 1);} Cyc_Position_reset_position((
struct _tagged_arr) _temp158);{ struct Cyc_Interface_I* _temp160= Cyc_Interface_load((
struct Cyc_Stdio___sFILE*) _check_null( _temp159)); Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( _temp159)); return _temp160;}}}} static
int Cyc_is_cfile( struct _tagged_arr* n){ return*(( const unsigned char*)
_check_unknown_subscript(* n, sizeof( unsigned char), 0)) !='-';} struct
_tuple11{ struct _tagged_arr f1; void* f2; struct _tagged_arr f3; } ; int Cyc_main(
int argc, struct _tagged_arr argv){ struct Cyc_List_List* options=({ struct Cyc_List_List*(*
_temp224)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple11* _temp227=({ struct _tuple11* _temp368=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp368->f1= _tag_arr("-v",
sizeof( unsigned char), 3u); _temp368->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp369=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp369[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp370; _temp370.tag= Cyc_Arg_Set_spec;
_temp370.f1=& Cyc_v_r; _temp370;}); _temp369;}); _temp368->f3= _tag_arr("    Print compilation stages verbosely",
sizeof( unsigned char), 39u); _temp368;}); struct _tuple11* _temp228=({ struct
_tuple11* _temp365=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp365->f1= _tag_arr("-o", sizeof( unsigned char), 3u); _temp365->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp366=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp366[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp367; _temp367.tag= Cyc_Arg_String_spec; _temp367.f1=
Cyc_set_output_file; _temp367;}); _temp366;}); _temp365->f3= _tag_arr("    Output file name",
sizeof( unsigned char), 21u); _temp365;}); struct _tuple11* _temp229=({ struct
_tuple11* _temp362=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp362->f1= _tag_arr("-D*", sizeof( unsigned char), 4u); _temp362->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp363=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp363[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp364; _temp364.tag= Cyc_Arg_Flag_spec; _temp364.f1= Cyc_add_cpparg; _temp364;});
_temp363;}); _temp362->f3= _tag_arr("   Pass definition to preprocessor",
sizeof( unsigned char), 35u); _temp362;}); struct _tuple11* _temp230=({ struct
_tuple11* _temp359=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp359->f1= _tag_arr("-B*", sizeof( unsigned char), 4u); _temp359->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp360=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp360[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp361; _temp361.tag= Cyc_Arg_Flag_spec; _temp361.f1= Cyc_add_cyclone_exec_prefix_path;
_temp361;}); _temp360;}); _temp359->f3= _tag_arr("   Add to the list of directories to search for compiler files",
sizeof( unsigned char), 63u); _temp359;}); struct _tuple11* _temp231=({ struct
_tuple11* _temp356=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp356->f1= _tag_arr("-I*", sizeof( unsigned char), 4u); _temp356->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp357=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp357[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp358; _temp358.tag= Cyc_Arg_Flag_spec; _temp358.f1= Cyc_add_cpparg; _temp358;});
_temp357;}); _temp356->f3= _tag_arr("   Add to the list of directories to search for include files",
sizeof( unsigned char), 62u); _temp356;}); struct _tuple11* _temp232=({ struct
_tuple11* _temp353=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp353->f1= _tag_arr("-L*", sizeof( unsigned char), 4u); _temp353->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp354=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp354[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp355; _temp355.tag= Cyc_Arg_Flag_spec; _temp355.f1= Cyc_add_cpparg; _temp355;});
_temp354;}); _temp353->f3= _tag_arr("   Add to the list of directories for -l",
sizeof( unsigned char), 41u); _temp353;}); struct _tuple11* _temp233=({ struct
_tuple11* _temp350=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp350->f1= _tag_arr("-l*", sizeof( unsigned char), 4u); _temp350->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp351=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp351[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp352; _temp352.tag= Cyc_Arg_Flag_spec; _temp352.f1= Cyc_add_libarg; _temp352;});
_temp351;}); _temp350->f3= _tag_arr("   Library file", sizeof( unsigned char),
16u); _temp350;}); struct _tuple11* _temp234=({ struct _tuple11* _temp347=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp347->f1= _tag_arr("-c",
sizeof( unsigned char), 3u); _temp347->f2=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp348=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp348[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp349; _temp349.tag= Cyc_Arg_Unit_spec;
_temp349.f1= Cyc_set_stop_after_objectfile; _temp349;}); _temp348;}); _temp347->f3=
_tag_arr("    Produce object file", sizeof( unsigned char), 24u); _temp347;});
struct _tuple11* _temp235=({ struct _tuple11* _temp344=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp344->f1= _tag_arr("-s", sizeof(
unsigned char), 3u); _temp344->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp345=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp345[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp346; _temp346.tag= Cyc_Arg_Flag_spec;
_temp346.f1= Cyc_add_ccarg; _temp346;}); _temp345;}); _temp344->f3= _tag_arr("    Remove all symbol table and relocation info from executable",
sizeof( unsigned char), 64u); _temp344;}); struct _tuple11* _temp236=({ struct
_tuple11* _temp341=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp341->f1= _tag_arr("-O", sizeof( unsigned char), 3u); _temp341->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp342=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp342[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp343; _temp343.tag= Cyc_Arg_Flag_spec; _temp343.f1= Cyc_add_ccarg; _temp343;});
_temp342;}); _temp341->f3= _tag_arr("    Optimize", sizeof( unsigned char), 13u);
_temp341;}); struct _tuple11* _temp237=({ struct _tuple11* _temp338=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp338->f1= _tag_arr("-O2",
sizeof( unsigned char), 4u); _temp338->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp339=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp339[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp340; _temp340.tag= Cyc_Arg_Flag_spec;
_temp340.f1= Cyc_add_ccarg; _temp340;}); _temp339;}); _temp338->f3= _tag_arr("   Optimize",
sizeof( unsigned char), 12u); _temp338;}); struct _tuple11* _temp238=({ struct
_tuple11* _temp335=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp335->f1= _tag_arr("-O3", sizeof( unsigned char), 4u); _temp335->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp336=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp336[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp337; _temp337.tag= Cyc_Arg_Flag_spec; _temp337.f1= Cyc_add_ccarg; _temp337;});
_temp336;}); _temp335->f3= _tag_arr("   Optimize", sizeof( unsigned char), 12u);
_temp335;}); struct _tuple11* _temp239=({ struct _tuple11* _temp332=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp332->f1= _tag_arr("-g",
sizeof( unsigned char), 3u); _temp332->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp333=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp333[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp334; _temp334.tag= Cyc_Arg_Flag_spec;
_temp334.f1= Cyc_add_ccarg; _temp334;}); _temp333;}); _temp332->f3= _tag_arr("    Compile for debugging",
sizeof( unsigned char), 26u); _temp332;}); struct _tuple11* _temp240=({ struct
_tuple11* _temp329=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp329->f1= _tag_arr("-p", sizeof( unsigned char), 3u); _temp329->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp330=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp330[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp331; _temp331.tag= Cyc_Arg_Flag_spec; _temp331.f1= Cyc_add_ccarg; _temp331;});
_temp330;}); _temp329->f3= _tag_arr("    Compile for prof", sizeof(
unsigned char), 21u); _temp329;}); struct _tuple11* _temp241=({ struct _tuple11*
_temp326=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp326->f1=
_tag_arr("-pg", sizeof( unsigned char), 4u); _temp326->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp327=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp327[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp328; _temp328.tag= Cyc_Arg_Flag_spec;
_temp328.f1= Cyc_add_ccarg; _temp328;}); _temp327;}); _temp326->f3= _tag_arr("   Compile for gprof",
sizeof( unsigned char), 21u); _temp326;}); struct _tuple11* _temp242=({ struct
_tuple11* _temp323=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp323->f1= _tag_arr("-S", sizeof( unsigned char), 3u); _temp323->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp324=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp324[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp325; _temp325.tag= Cyc_Arg_Unit_spec; _temp325.f1= Cyc_set_stop_after_asmfile;
_temp325;}); _temp324;}); _temp323->f3= _tag_arr("    Stop after producing assembly code",
sizeof( unsigned char), 39u); _temp323;}); struct _tuple11* _temp243=({ struct
_tuple11* _temp320=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp320->f1= _tag_arr("-M", sizeof( unsigned char), 3u); _temp320->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp321=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp321[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp322; _temp322.tag= Cyc_Arg_Unit_spec; _temp322.f1= Cyc_set_produce_dependencies;
_temp322;}); _temp321;}); _temp320->f3= _tag_arr("    Produce dependencies",
sizeof( unsigned char), 25u); _temp320;}); struct _tuple11* _temp244=({ struct
_tuple11* _temp317=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp317->f1= _tag_arr("-MG", sizeof( unsigned char), 4u); _temp317->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp318=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp318[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp319; _temp319.tag= Cyc_Arg_Flag_spec; _temp319.f1= Cyc_add_cpparg; _temp319;});
_temp318;}); _temp317->f3= _tag_arr("    When producing dependencies assume missing files are generated",
sizeof( unsigned char), 67u); _temp317;}); struct _tuple11* _temp245=({ struct
_tuple11* _temp314=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp314->f1= _tag_arr("-MT", sizeof( unsigned char), 4u); _temp314->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp315=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp315[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp316; _temp316.tag= Cyc_Arg_String_spec; _temp316.f1=
Cyc_set_dependencies_target; _temp316;}); _temp315;}); _temp314->f3= _tag_arr("   Give target for dependencies",
sizeof( unsigned char), 32u); _temp314;}); struct _tuple11* _temp246=({ struct
_tuple11* _temp311=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp311->f1= _tag_arr("-E", sizeof( unsigned char), 3u); _temp311->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp312=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp312[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp313; _temp313.tag= Cyc_Arg_Set_spec; _temp313.f1=& Cyc_stop_after_cpp_r;
_temp313;}); _temp312;}); _temp311->f3= _tag_arr("    Stop after preprocessing",
sizeof( unsigned char), 29u); _temp311;}); struct _tuple11* _temp247=({ struct
_tuple11* _temp308=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp308->f1= _tag_arr("-parseonly", sizeof( unsigned char), 11u); _temp308->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp309=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp309[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp310; _temp310.tag= Cyc_Arg_Set_spec; _temp310.f1=& Cyc_parseonly_r;
_temp310;}); _temp309;}); _temp308->f3= _tag_arr("\n        Stop after parsing",
sizeof( unsigned char), 28u); _temp308;}); struct _tuple11* _temp248=({ struct
_tuple11* _temp305=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp305->f1= _tag_arr("-tc", sizeof( unsigned char), 4u); _temp305->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp306=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp306[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp307; _temp307.tag= Cyc_Arg_Set_spec; _temp307.f1=& Cyc_tc_r; _temp307;});
_temp306;}); _temp305->f3= _tag_arr("   Stop after type checking", sizeof(
unsigned char), 28u); _temp305;}); struct _tuple11* _temp249=({ struct _tuple11*
_temp302=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp302->f1=
_tag_arr("-toc", sizeof( unsigned char), 5u); _temp302->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp303=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp303[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp304; _temp304.tag= Cyc_Arg_Set_spec;
_temp304.f1=& Cyc_toc_r; _temp304;}); _temp303;}); _temp302->f3= _tag_arr("  Stop after translation to C",
sizeof( unsigned char), 30u); _temp302;}); struct _tuple11* _temp250=({ struct
_tuple11* _temp299=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp299->f1= _tag_arr("-ic", sizeof( unsigned char), 4u); _temp299->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp300=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp300[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp301; _temp301.tag= Cyc_Arg_Set_spec; _temp301.f1=& Cyc_ic_r; _temp301;});
_temp300;}); _temp299->f3= _tag_arr("   Activate the link-checker", sizeof(
unsigned char), 29u); _temp299;}); struct _tuple11* _temp251=({ struct _tuple11*
_temp296=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp296->f1=
_tag_arr("-pp", sizeof( unsigned char), 4u); _temp296->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp297=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp297[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp298; _temp298.tag= Cyc_Arg_Set_spec;
_temp298.f1=& Cyc_pp_r; _temp298;}); _temp297;}); _temp296->f3= _tag_arr("   Pretty print",
sizeof( unsigned char), 16u); _temp296;}); struct _tuple11* _temp252=({ struct
_tuple11* _temp293=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp293->f1= _tag_arr("-up", sizeof( unsigned char), 4u); _temp293->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp294=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp294[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp295; _temp295.tag= Cyc_Arg_Clear_spec; _temp295.f1=&
Cyc_pp_r; _temp295;}); _temp294;}); _temp293->f3= _tag_arr("   Ugly print",
sizeof( unsigned char), 14u); _temp293;}); struct _tuple11* _temp253=({ struct
_tuple11* _temp290=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp290->f1= _tag_arr("-tovc", sizeof( unsigned char), 6u); _temp290->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp291=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp291[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp292; _temp292.tag= Cyc_Arg_Set_spec; _temp292.f1=& Cyc_tovc_r; _temp292;});
_temp291;}); _temp290->f3= _tag_arr(" Avoid gcc extensions in C output", sizeof(
unsigned char), 34u); _temp290;}); struct _tuple11* _temp254=({ struct _tuple11*
_temp287=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp287->f1=
_tag_arr("-save-temps", sizeof( unsigned char), 12u); _temp287->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp288=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp288[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp289; _temp289.tag= Cyc_Arg_Unit_spec; _temp289.f1= Cyc_set_save_temps;
_temp289;}); _temp288;}); _temp287->f3= _tag_arr("\n        Don't delete temporary files",
sizeof( unsigned char), 38u); _temp287;}); struct _tuple11* _temp255=({ struct
_tuple11* _temp284=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp284->f1= _tag_arr("-save-c", sizeof( unsigned char), 8u); _temp284->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp285=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp285[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp286; _temp286.tag= Cyc_Arg_Set_spec; _temp286.f1=& Cyc_save_c_r; _temp286;});
_temp285;}); _temp284->f3= _tag_arr("\n        Don't delete temporary C files",
sizeof( unsigned char), 40u); _temp284;}); struct _tuple11* _temp256=({ struct
_tuple11* _temp281=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp281->f1= _tag_arr("-nocyc", sizeof( unsigned char), 7u); _temp281->f2=(
void*)({ struct Cyc_Arg_Unit_spec_struct* _temp282=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp282[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp283; _temp283.tag= Cyc_Arg_Unit_spec; _temp283.f1= Cyc_set_nocyc; _temp283;});
_temp282;}); _temp281->f3= _tag_arr("Don't add implicit namespace Cyc", sizeof(
unsigned char), 33u); _temp281;}); struct _tuple11* _temp257=({ struct _tuple11*
_temp278=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp278->f1=
_tag_arr("-nogc", sizeof( unsigned char), 6u); _temp278->f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp279=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp279[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp280; _temp280.tag= Cyc_Arg_Set_spec; _temp280.f1=& Cyc_nogc_r; _temp280;});
_temp279;}); _temp278->f3= _tag_arr(" Don't link in the garbage collector",
sizeof( unsigned char), 37u); _temp278;}); struct _tuple11* _temp258=({ struct
_tuple11* _temp275=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp275->f1= _tag_arr("-noremoveunused", sizeof( unsigned char), 16u); _temp275->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp276=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp276[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp277; _temp277.tag= Cyc_Arg_Set_spec; _temp277.f1=& Cyc_noshake_r; _temp277;});
_temp276;}); _temp275->f3= _tag_arr("\n        Don't remove externed variables that aren't used",
sizeof( unsigned char), 58u); _temp275;}); struct _tuple11* _temp259=({ struct
_tuple11* _temp272=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp272->f1= _tag_arr("-noexpandtypedefs", sizeof( unsigned char), 18u);
_temp272->f2=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp273=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp273[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp274; _temp274.tag= Cyc_Arg_Set_spec; _temp274.f1=& Cyc_noexpand_r; _temp274;});
_temp273;}); _temp272->f3= _tag_arr("\n        Don't expand typedefs in pretty printing",
sizeof( unsigned char), 50u); _temp272;}); struct _tuple11* _temp260=({ struct
_tuple11* _temp269=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp269->f1= _tag_arr("-printalltvars", sizeof( unsigned char), 15u); _temp269->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp270=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp270[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp271; _temp271.tag= Cyc_Arg_Set_spec; _temp271.f1=& Cyc_print_all_tvars_r;
_temp271;}); _temp270;}); _temp269->f3= _tag_arr("\n        Print all type variables (even implicit default effects)",
sizeof( unsigned char), 66u); _temp269;}); struct _tuple11* _temp261=({ struct
_tuple11* _temp266=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp266->f1= _tag_arr("-printallkinds", sizeof( unsigned char), 15u); _temp266->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp267=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp267[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp268; _temp268.tag= Cyc_Arg_Set_spec; _temp268.f1=& Cyc_print_all_kinds_r;
_temp268;}); _temp267;}); _temp266->f3= _tag_arr("\n        Always print kinds of type variables",
sizeof( unsigned char), 46u); _temp266;}); struct _tuple11* _temp262=({ struct
_tuple11* _temp263=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp263->f1= _tag_arr("-printfullevars", sizeof( unsigned char), 16u); _temp263->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp264=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp264[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp265; _temp265.tag= Cyc_Arg_Set_spec; _temp265.f1=& Cyc_print_full_evars_r;
_temp265;}); _temp264;}); _temp263->f3= _tag_arr("\n       Print full information for evars (type debugging)",
sizeof( unsigned char), 58u); _temp263;}); struct _tuple11* _temp225[ 36u]={
_temp227, _temp228, _temp229, _temp230, _temp231, _temp232, _temp233, _temp234,
_temp235, _temp236, _temp237, _temp238, _temp239, _temp240, _temp241, _temp242,
_temp243, _temp244, _temp245, _temp246, _temp247, _temp248, _temp249, _temp250,
_temp251, _temp252, _temp253, _temp254, _temp255, _temp256, _temp257, _temp258,
_temp259, _temp260, _temp261, _temp262}; struct _tagged_arr _temp226={( void*)
_temp225,( void*) _temp225,( void*)( _temp225 + 36u)}; _temp224( _temp226);});
Cyc_Arg_parse( options, Cyc_add_other, _tag_arr("Options:", sizeof(
unsigned char), 9u), argv);{ struct Cyc_List_List* _temp161=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); for( 0; _temp161 !=
0; _temp161=(( struct Cyc_List_List*) _check_null( _temp161))->tl){ Cyc_process_file(*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp161))->hd)); if(
Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)?
1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs == 0){ return 0;}{ struct
_tagged_arr cyclone_exec_prefix=( struct _tagged_arr) Cyc_Stdlib_getenv(
_tag_arr("CYCLONE_EXEC_PREFIX", sizeof( unsigned char), 20u)); if(
cyclone_exec_prefix.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp162=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp162->hd=( void*)({ struct
_tagged_arr* _temp163=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp163[ 0]= cyclone_exec_prefix; _temp163;}); _temp162->tl= Cyc_cyclone_exec_prefix_path;
_temp162;});} Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp164=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp164->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"c:/cyclone/bin/cyc-lib", sizeof( unsigned char), 23u); _temp164->tl=
Cyc_cyclone_exec_prefix_path; _temp164;}); Cyc_cyclone_exec_prefix_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_exec_prefix_path);{
struct _tagged_arr _temp165= Cyc_do_find( Cyc_cyclone_exec_prefix_path, _tag_arr("include/cyc_include.h",
sizeof( unsigned char), 22u)); Cyc_ccargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{ struct _tagged_arr _temp166= Cyc_String_str_sepstr(
Cyc_ccargs, _tag_arr(" ", sizeof( unsigned char), 2u)); Cyc_libargs=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_libargs);{
struct _tagged_arr _temp167= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp223=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp223->hd=( void*) _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)),"", sizeof( unsigned char), 1u); _temp223->tl= Cyc_libargs;
_temp223;}), _tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_List_List*
stdlib; struct _tagged_arr stdlib_string; int _temp168=(( Cyc_stop_after_asmfile_r?
1: Cyc_stop_after_objectfile_r)? 1:( Cyc_output_file != 0? Cyc_Filename_check_suffix(*((
struct _tagged_arr*) _check_null( Cyc_output_file)), _tag_arr(".a", sizeof(
unsigned char), 3u)): 0))? 1:( Cyc_output_file != 0? Cyc_Filename_check_suffix(*((
struct _tagged_arr*) _check_null( Cyc_output_file)), _tag_arr(".lib", sizeof(
unsigned char), 5u)): 0); if( _temp168){ stdlib= 0; stdlib_string= _tag_arr("",
sizeof( unsigned char), 1u);} else{ struct _tagged_arr _temp169= Cyc_do_find(
Cyc_cyclone_exec_prefix_path, _tag_arr("cyclib.a", sizeof( unsigned char), 9u));
struct _tagged_arr _temp170= Cyc_nogc_r? Cyc_do_find( Cyc_cyclone_exec_prefix_path,
_tag_arr("nogc.a", sizeof( unsigned char), 7u)): Cyc_do_find( Cyc_cyclone_exec_prefix_path,
_tag_arr("gc.a", sizeof( unsigned char), 5u)); stdlib=({ struct Cyc_List_List*(*
_temp171)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tagged_arr* _temp174=({ struct _tagged_arr* _temp175=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp175[ 0]=
_temp169; _temp175;}); struct _tagged_arr* _temp172[ 1u]={ _temp174}; struct
_tagged_arr _temp173={( void*) _temp172,( void*) _temp172,( void*)( _temp172 + 1u)};
_temp171( _temp173);}); stdlib_string=( struct _tagged_arr)({ struct _tagged_arr
_temp176= _temp169; struct _tagged_arr _temp177= _temp170; xprintf(" %.*s %.*s",
_get_arr_size( _temp176, 1u), _temp176.curr, _get_arr_size( _temp177, 1u),
_temp177.curr);});} if( Cyc_ic_r){ struct _handler_cons _temp178; _push_handler(&
_temp178);{ int _temp180= 0; if( setjmp( _temp178.handler)){ _temp180= 1;} if( !
_temp180){ Cyc_ccargs=(( struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*),
struct Cyc_List_List* l)) Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=((
struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*), struct Cyc_List_List*
l)) Cyc_List_filter)( Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List*
_temp181=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( stdlib,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_ccargs, Cyc_libargs)); if( !
_temp168){ _temp181=({ struct Cyc_List_List* _temp182=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp182->hd=( void*) Cyc_final_strptr;
_temp182->tl= _temp181; _temp182;});}{ struct Cyc_Interface_I* _temp183=((
struct Cyc_Interface_I*(*)( struct Cyc_Interface_I*(* get)( struct _tagged_arr*),
struct Cyc_List_List* la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio, _temp181, _temp181); if( _temp183 == 0){ fprintf( _sfile_to_file(
Cyc_Stdio_stderr),"Error: interfaces incompatible\n"); Cyc_compile_failure= 1;
Cyc_remove_cfiles();{ int _temp184= 1; _npop_handler( 0u); return _temp184;}}
if( _temp168){ if( Cyc_output_file != 0){ struct _tagged_arr _temp185=({ struct
_tagged_arr _temp188=( struct _tagged_arr) Cyc_Filename_chop_extension(*((
struct _tagged_arr*) _check_null( Cyc_output_file))); xprintf("%.*s.cycio",
_get_arr_size( _temp188, 1u), _temp188.curr);}); struct Cyc_Stdio___sFILE*
_temp186= Cyc_try_file_open(( struct _tagged_arr) _temp185, _tag_arr("w",
sizeof( unsigned char), 2u), _tag_arr("interface object file", sizeof(
unsigned char), 22u)); if( _temp186 == 0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{
int _temp187= 1; _npop_handler( 0u); return _temp187;}} Cyc_Interface_save((
struct Cyc_Interface_I*) _check_null( _temp183),( struct Cyc_Stdio___sFILE*)
_check_null( _temp186)); Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( _temp186));}} else{ if( ! Cyc_Interface_is_subinterface( Cyc_Interface_empty(),(
struct Cyc_Interface_I*) _check_null( _temp183),({ struct _tuple9* _temp189=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp189->f1= _tag_arr("empty interface",
sizeof( unsigned char), 16u); _temp189->f2= _tag_arr("global interface", sizeof(
unsigned char), 17u); _temp189;}))){ fprintf( _sfile_to_file( Cyc_Stdio_stderr),"Error: some objects are still undefined\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles();{ int _temp190= 1; _npop_handler( 0u);
return _temp190;}}}}}; _pop_handler();} else{ void* _temp179=( void*)
_exn_thrown; void* _temp192= _temp179; struct _tagged_arr _temp206; struct
_tagged_arr _temp208; struct _tagged_arr _temp210; _LL194: if(*(( void**)
_temp192) == Cyc_Core_Failure){ _LL207: _temp206=(( struct Cyc_Core_Failure_struct*)
_temp192)->f1; goto _LL195;} else{ goto _LL196;} _LL196: if(*(( void**) _temp192)
== Cyc_Core_Impossible){ _LL209: _temp208=(( struct Cyc_Core_Impossible_struct*)
_temp192)->f1; goto _LL197;} else{ goto _LL198;} _LL198: if( _temp192 == Cyc_Dict_Absent){
goto _LL199;} else{ goto _LL200;} _LL200: if(*(( void**) _temp192) == Cyc_Core_InvalidArg){
_LL211: _temp210=(( struct Cyc_Core_InvalidArg_struct*) _temp192)->f1; goto
_LL201;} else{ goto _LL202;} _LL202: goto _LL203; _LL204: goto _LL205; _LL195:({
struct _tagged_arr _temp212= _temp206; printf("Exception Core::Failure %.*s\n",
_get_arr_size( _temp212, 1u), _temp212.curr);}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL197:({ struct _tagged_arr _temp213= _temp208; printf("Exception Core::Impossible %.*s\n",
_get_arr_size( _temp213, 1u), _temp213.curr);}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL199: printf("Exception Dict::Absent\n"); Cyc_compile_failure= 1;
Cyc_remove_cfiles(); return 1; _LL201:({ struct _tagged_arr _temp214= _temp210;
printf("Exception Core::InvalidArg %.*s\n", _get_arr_size( _temp214, 1u),
_temp214.curr);}); Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1; _LL203:
printf("Uncaught exception\n"); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL205:( void) _throw( _temp192); _LL193:;}}}{ struct _tagged_arr
_temp215=({ struct _tagged_arr _temp218= _temp165; struct _tagged_arr _temp219=
Cyc_output_file == 0? _tag_arr("", sizeof( unsigned char), 1u):( struct
_tagged_arr)({ struct _tagged_arr _temp217=*(( struct _tagged_arr*) _check_null(
Cyc_output_file)); xprintf(" -o %.*s", _get_arr_size( _temp217, 1u), _temp217.curr);});
struct _tagged_arr _temp220=( struct _tagged_arr) _temp166; struct _tagged_arr
_temp221= stdlib_string; struct _tagged_arr _temp222=( struct _tagged_arr)
_temp167; xprintf("gcc -include %.*s%.*s %.*s%.*s%.*s", _get_arr_size( _temp218,
1u), _temp218.curr, _get_arr_size( _temp219, 1u), _temp219.curr, _get_arr_size(
_temp220, 1u), _temp220.curr, _get_arr_size( _temp221, 1u), _temp221.curr,
_get_arr_size( _temp222, 1u), _temp222.curr);}); if( Cyc_v_r){({ struct
_tagged_arr _temp216=( struct _tagged_arr) _temp215; fprintf( _sfile_to_file(
Cyc_Stdio_stderr),"%.*s\n", _get_arr_size( _temp216, 1u), _temp216.curr);});}
if( system( string_to_Cstring(( struct _tagged_arr) _temp215)) != 0){ fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"Error: C compiler failed\n"); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1;} Cyc_remove_cfiles(); return Cyc_compile_failure?
1: 0;}}}}}}