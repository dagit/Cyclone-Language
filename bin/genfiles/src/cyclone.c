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
_tagged_arr); extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern struct
Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* l);
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
struct Cyc_Stdio___sFILE*); static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Stdio_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Time_tm{ int tm_sec; int tm_min; int tm_hour;
int tm_mday; int tm_mon; int tm_year; int tm_wday; int tm_yday; int tm_isdst; }
; extern unsigned int time( unsigned int* t); struct Cyc___sFILE; struct Cyc__reent;
struct Cyc__glue{ struct Cyc__glue* _next; int _niobs; struct Cyc___sFILE* _iobs;
} ; struct Cyc__Bigint{ struct Cyc__Bigint* _next; int _k; int _maxwds; int
_sign; int _wds; unsigned int _x[ 1u]; } ; struct Cyc__atexit{ struct Cyc__atexit*
_next; int _ind; void(* _fns[ 32u])(); } ; struct Cyc___sbuf{ unsigned char*
_base; int _size; } ; struct Cyc___sFILE{ unsigned char* _p; int _r; int _w;
short _flags; short _file; struct Cyc___sbuf _bf; int _lbfsize; void* _cookie;
int(* _read)( void* _cookie, unsigned char* _buf, int _n)  __attribute__(( cdecl
)) ; int(* _write)( void* _cookie, const unsigned char* _buf, int _n)
 __attribute__(( cdecl )) ; int(* _seek)( void* _cookie, int _offset, int
_whence)  __attribute__(( cdecl )) ; int(* _close)( void* _cookie)
 __attribute__(( cdecl )) ; struct Cyc___sbuf _ub; unsigned char* _up; int _ur;
unsigned char _ubuf[ 3u]; unsigned char _nbuf[ 1u]; struct Cyc___sbuf _lb; int
_blksize; int _offset; struct Cyc__reent* _data; } ; struct Cyc__reent_u1{
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
 __attribute__(( cdecl )) ; extern struct _tagged_arr Cyc_Stdlib_getenv( struct
_tagged_arr); extern int Cyc_String_strlen( struct _tagged_arr s); extern int
Cyc_String_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern struct
_tagged_arr Cyc_String_strconcat( struct _tagged_arr, struct _tagged_arr);
extern struct _tagged_arr Cyc_String_strconcat_l( struct Cyc_List_List*); extern
struct _tagged_arr Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_arr); extern struct _tagged_arr Cyc_String_substring( struct _tagged_arr,
int ofs, int n); extern struct _tagged_arr Cyc_String_strchr( struct _tagged_arr
s, unsigned char c); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct _tagged_arr Cyc_Filename_concat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr); extern int Cyc_Filename_check_suffix( struct _tagged_arr,
struct _tagged_arr); struct Cyc_Id_id; struct Cyc_Lineno_Pos{ struct _tagged_arr
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
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Printf_ft=
0; static const int Cyc_Absyn_Scanf_ft= 1; static const int Cyc_Absyn_Regparm_att=
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
Cyc_Absyn_Format_att= 3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1;
int f2; int f3; } ; static const int Cyc_Absyn_Carray_mod= 0; static const int
Cyc_Absyn_ConstArray_mod= 0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Pointer_mod= 1; struct
Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual
f3; } ; static const int Cyc_Absyn_Function_mod= 2; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
static const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
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
} ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r; extern struct
Cyc_Absynpp_Params Cyc_Absynpp_c_params_r; extern void Cyc_Absynpp_decllist2file(
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
Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* decls);
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
struct Cyc_List_List* ds); struct Cyc_Interface_I; extern struct Cyc_Interface_I*
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
0; static int Cyc_save_c_r= 0; static int Cyc_nogc_r= 0; static int Cyc_pa_r= 0;
static int Cyc_add_cyc_namespace_r= 1; static int Cyc_print_full_evars_r= 0;
static int Cyc_print_all_tvars_r= 0; static int Cyc_print_all_kinds_r= 0; static
struct _tagged_arr* Cyc_output_file= 0; static void Cyc_set_output_file( struct
_tagged_arr s){ Cyc_output_file=({ struct _tagged_arr* _temp0=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp0[ 0]= s; _temp0;});}
static struct Cyc_List_List* Cyc_cppargs= 0; static void Cyc_add_cpparg( struct
_tagged_arr s){ Cyc_cppargs=({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)({ struct
_tagged_arr* _temp2=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp2[ 0]= s; _temp2;}); _temp1->tl= Cyc_cppargs; _temp1;});} static int Cyc_is_cyclone_sourcefile(
struct _tagged_arr s){ int _temp3= Cyc_String_strlen( s); if( _temp3 <= 4){
return 0;} else{ return Cyc_String_strcmp( _tagged_arr_plus( s, sizeof(
unsigned char), _temp3 - 4), _tag_arr(".cyc", sizeof( unsigned char), 5u)) == 0;}}
static struct Cyc_List_List* Cyc_cyclone_exec_prefix_path= 0; static void Cyc_add_cyclone_exec_prefix_path(
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
Cyc_add_ccarg( _tag_arr("-DNO_CYC_PREFIX", sizeof( unsigned char), 16u));}
static void Cyc_set_pa(){ Cyc_pa_r= 1; Cyc_add_ccarg( _tag_arr("-DCYC_REGION_PROFILE",
sizeof( unsigned char), 21u));} static void Cyc_set_stop_after_asmfile(){ Cyc_stop_after_asmfile_r=
1; Cyc_add_ccarg( _tag_arr("-S", sizeof( unsigned char), 3u));} static struct
Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other( struct
_tagged_arr s){ if( Cyc_is_cyclone_sourcefile( s)){ Cyc_cyclone_files=({ struct
Cyc_List_List* _temp17=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp17->hd=( void*)({ struct _tagged_arr* _temp18=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp18[ 0]= s; _temp18;}); _temp17->tl=
Cyc_cyclone_files; _temp17;});{ struct _tagged_arr _temp19= Cyc_Filename_chop_extension(
s); struct _tagged_arr _temp20= Cyc_String_strconcat(( struct _tagged_arr)
_temp19, _tag_arr(".c", sizeof( unsigned char), 3u)); Cyc_add_ccarg(( struct
_tagged_arr) _temp20);}} else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file(
struct _tagged_arr s){ if( Cyc_save_temps_r){ return;} else{ Cyc_Stdio_remove( s);}}
int Cyc_compile_failure= 0; struct Cyc_Stdio___sFILE* Cyc_try_file_open( struct
_tagged_arr filename, struct _tagged_arr mode, struct _tagged_arr msg_part){
struct _handler_cons _temp21; _push_handler(& _temp21);{ int _temp23= 0; if(
setjmp( _temp21.handler)){ _temp23= 1;} if( ! _temp23){{ struct Cyc_Stdio___sFILE*
_temp24=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open( filename, mode);
_npop_handler( 0u); return _temp24;}; _pop_handler();} else{ void* _temp22=(
void*) _exn_thrown; void* _temp26= _temp22; _LL28: goto _LL29; _LL30: goto _LL31;
_LL29: Cyc_compile_failure= 1;({ int(* _temp32)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp37= Cyc_Stdio_stderr; struct _tagged_arr _temp38= _tag_arr("\nError: couldn't open %s %s\n",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp40; _temp40.tag=
Cyc_Stdio_String_pa; _temp40.f1=( struct _tagged_arr) msg_part;{ void* _temp35=(
void*)& _temp40; struct Cyc_Stdio_String_pa_struct _temp39; _temp39.tag= Cyc_Stdio_String_pa;
_temp39.f1=( struct _tagged_arr) filename;{ void* _temp36=( void*)& _temp39;
void* _temp33[ 2u]={ _temp35, _temp36}; struct _tagged_arr _temp34={( void*)
_temp33,( void*) _temp33,( void*)( _temp33 + 2u)}; _temp32( _temp37, _temp38,
_temp34);}}}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);
return 0; _LL31:( void) _throw( _temp26); _LL27:;}}} struct Cyc_List_List* Cyc_do_stage(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( void*, struct Cyc_List_List*),
void* env, struct Cyc_List_List* tds){ struct _tagged_arr exn_string= _tag_arr("",
sizeof( unsigned char), 1u); struct _tagged_arr explain_string= _tag_arr("",
sizeof( unsigned char), 1u); int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct*
_temp85=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp85[ 0]=({ struct Cyc_Core_Impossible_struct _temp86; _temp86.tag= Cyc_Core_Impossible;
_temp86.f1= _tag_arr("", sizeof( unsigned char), 1u); _temp86;}); _temp85;});
struct Cyc_List_List* _temp41= 0;{ struct _handler_cons _temp42; _push_handler(&
_temp42);{ int _temp44= 0; if( setjmp( _temp42.handler)){ _temp44= 1;} if( !
_temp44){ _temp41= f( env, tds);; _pop_handler();} else{ void* _temp43=( void*)
_exn_thrown; void* _temp46= _temp43; struct _tagged_arr _temp58; struct
_tagged_arr _temp60; _LL48: if(*(( void**) _temp46) == Cyc_Core_Impossible){
_LL59: _temp58=(( struct Cyc_Core_Impossible_struct*) _temp46)->f1; goto _LL49;}
else{ goto _LL50;} _LL50: if( _temp46 == Cyc_Dict_Absent){ goto _LL51;} else{
goto _LL52;} _LL52: if(*(( void**) _temp46) == Cyc_Core_InvalidArg){ _LL61:
_temp60=(( struct Cyc_Core_InvalidArg_struct*) _temp46)->f1; goto _LL53;} else{
goto _LL54;} _LL54: goto _LL55; _LL56: goto _LL57; _LL49: exn_string= _tag_arr("Exception Core::Impossible",
sizeof( unsigned char), 27u); explain_string= _temp58; goto _LL47; _LL51:
exn_string= _tag_arr("Exception Dict::Absent", sizeof( unsigned char), 23u);
goto _LL47; _LL53: exn_string= _tag_arr("Exception Core::InvalidArg", sizeof(
unsigned char), 27u); explain_string= _temp60; goto _LL47; _LL55: ex= _temp46;
other_exn= 1; exn_string= _tag_arr("Uncaught exception", sizeof( unsigned char),
19u); goto _LL47; _LL57:( void) _throw( _temp46); _LL47:;}}} if( Cyc_Position_error_p()){
Cyc_compile_failure= 1;} if( Cyc_String_strcmp( exn_string, _tag_arr("", sizeof(
unsigned char), 1u)) != 0){ Cyc_compile_failure= 1;({ int(* _temp62)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp68= Cyc_Stdio_stderr; struct _tagged_arr _temp69= _tag_arr("\n%s thrown during %s: %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp72; _temp72.tag=
Cyc_Stdio_String_pa; _temp72.f1=( struct _tagged_arr) exn_string;{ void* _temp65=(
void*)& _temp72; struct Cyc_Stdio_String_pa_struct _temp71; _temp71.tag= Cyc_Stdio_String_pa;
_temp71.f1=( struct _tagged_arr) stage_name;{ void* _temp66=( void*)& _temp71;
struct Cyc_Stdio_String_pa_struct _temp70; _temp70.tag= Cyc_Stdio_String_pa;
_temp70.f1=( struct _tagged_arr) explain_string;{ void* _temp67=( void*)&
_temp70; void* _temp63[ 3u]={ _temp65, _temp66, _temp67}; struct _tagged_arr
_temp64={( void*) _temp63,( void*) _temp63,( void*)( _temp63 + 3u)}; _temp62(
_temp68, _temp69, _temp64);}}}});} if( Cyc_compile_failure){({ int(* _temp73)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp76= Cyc_Stdio_stderr; struct _tagged_arr _temp77=
_tag_arr("\nCOMPILATION FAILED!\n", sizeof( unsigned char), 22u); void* _temp74[
0u]={}; struct _tagged_arr _temp75={( void*) _temp74,( void*) _temp74,( void*)(
_temp74 + 0u)}; _temp73( _temp76, _temp77, _temp75);}); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); if( other_exn){( int) _throw( ex);}
return _temp41;} else{ if( Cyc_v_r){({ int(* _temp78)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp82= Cyc_Stdio_stderr; struct _tagged_arr _temp83= _tag_arr("%s completed.\n",
sizeof( unsigned char), 15u); struct Cyc_Stdio_String_pa_struct _temp84; _temp84.tag=
Cyc_Stdio_String_pa; _temp84.f1=( struct _tagged_arr) stage_name;{ void* _temp81=(
void*)& _temp84; void* _temp79[ 1u]={ _temp81}; struct _tagged_arr _temp80={(
void*) _temp79,( void*) _temp79,( void*)( _temp79 + 1u)}; _temp78( _temp82,
_temp83, _temp80);}}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);
return _temp41;}} return _temp41;} struct Cyc_List_List* Cyc_do_parse( struct
Cyc_Stdio___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{ struct
Cyc_List_List* _temp87= Cyc_Parse_parse_file( f); Cyc_Lex_lex_init(); return
_temp87;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds); if( ! Cyc_noshake_r){ tds=
Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck(
int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds);
return tds;} struct _tuple10{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Stdio___sFILE*
f2; struct Cyc_Stdio___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple10* params, struct Cyc_List_List* tds){ struct _tuple10 _temp90;
struct Cyc_Stdio___sFILE* _temp91; struct Cyc_Stdio___sFILE* _temp93; struct Cyc_Tcenv_Tenv*
_temp95; struct _tuple10* _temp88= params; _temp90=* _temp88; _LL96: _temp95=
_temp90.f1; goto _LL94; _LL94: _temp93= _temp90.f2; goto _LL92; _LL92: _temp91=
_temp90.f3; goto _LL89; _LL89: { struct Cyc_Interface_I* _temp97= Cyc_Interface_extract(
_temp95->ae); if( _temp93 == 0){ Cyc_Interface_save( _temp97, _temp91);} else{
struct Cyc_Interface_I* _temp98= Cyc_Interface_parse(( struct Cyc_Stdio___sFILE*)
_check_null( _temp93)); if( ! Cyc_Interface_is_subinterface( _temp98, _temp97,({
struct _tuple9* _temp99=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp99->f1= _tag_arr("written interface", sizeof( unsigned char), 18u); _temp99->f2=
_tag_arr("maximal interface", sizeof( unsigned char), 18u); _temp99;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp98, _temp91);}} return tds;}} struct Cyc_List_List*
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
_temp100= 0; int _temp101= Cyc_String_strlen( s); while( _temp101 > 0) { struct
_tagged_arr _temp102= Cyc_String_strchr( s, c); if( _temp102.curr ==(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ _temp100=({ struct Cyc_List_List*
_temp103=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp103->hd=( void*)({ struct _tagged_arr* _temp104=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp104[ 0]= s; _temp104;}); _temp103->tl=
_temp100; _temp103;}); break;} else{ _temp100=({ struct Cyc_List_List* _temp105=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp105->hd=(
void*)({ struct _tagged_arr* _temp106=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp106[ 0]=( struct _tagged_arr) Cyc_String_substring( s,
0,((( struct _tagged_arr) _temp102).curr - s.curr) / sizeof( unsigned char));
_temp106;}); _temp105->tl= _temp100; _temp105;}); _temp101 -=((( struct
_tagged_arr) _temp102).curr - s.curr) / sizeof( unsigned char); s=
_tagged_arr_plus( _temp102, sizeof( unsigned char), 1);}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp100);}} static int Cyc_file_exists(
struct _tagged_arr file){ struct Cyc_Stdio___sFILE* f= 0;{ struct _handler_cons
_temp107; _push_handler(& _temp107);{ int _temp109= 0; if( setjmp( _temp107.handler)){
_temp109= 1;} if( ! _temp109){ f=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open(
file, _tag_arr("r", sizeof( unsigned char), 2u));; _pop_handler();} else{ void*
_temp108=( void*) _exn_thrown; void* _temp111= _temp108; _LL113: goto _LL114;
_LL115: goto _LL116; _LL114: goto _LL112; _LL116:( void) _throw( _temp111);
_LL112:;}}} if( f == 0){ return 0;} else{ Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
_check_null( f)); return 1;}} static struct _tagged_arr* Cyc_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ if( file.curr ==(( struct _tagged_arr) _tag_arr(
0u, 0u, 0u)).curr){ return 0;} for( 0; dirs != 0; dirs=(( struct Cyc_List_List*)
_check_null( dirs))->tl){ struct _tagged_arr _temp117=*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( dirs))->hd); if( _temp117.curr ==(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: Cyc_String_strlen( _temp117) == 0){
continue;}{ struct _tagged_arr s=( struct _tagged_arr) Cyc_Filename_concat(
_temp117, file); if( Cyc_file_exists( s)){ return({ struct _tagged_arr* _temp118=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp118[ 0]= s;
_temp118;});}}} return 0;} static struct _tagged_arr Cyc_do_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ struct _tagged_arr* _temp119= Cyc_find( dirs,
file); if( _temp119 == 0){({ int(* _temp120)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp124= Cyc_Stdio_stderr; struct _tagged_arr _temp125= _tag_arr("Error: can't find internal compiler file %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp126;
_temp126.tag= Cyc_Stdio_String_pa; _temp126.f1=( struct _tagged_arr) file;{ void*
_temp123=( void*)& _temp126; void* _temp121[ 1u]={ _temp123}; struct _tagged_arr
_temp122={( void*) _temp121,( void*) _temp121,( void*)( _temp121 + 1u)};
_temp120( _temp124, _temp125, _temp122);}}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} return*(( struct _tagged_arr*) _check_null( _temp119));} static int
Cyc_is_other_special( unsigned char c){ switch( c){ case '\\': _LL127: goto
_LL128; case '"': _LL128: goto _LL129; case ';': _LL129: goto _LL130; case '&':
_LL130: goto _LL131; case '(': _LL131: goto _LL132; case ')': _LL132: goto
_LL133; case '|': _LL133: goto _LL134; case '^': _LL134: goto _LL135; case '<':
_LL135: goto _LL136; case '>': _LL136: goto _LL137; case ' ': _LL137: goto
_LL138; case '\n': _LL138: goto _LL139; case '\t': _LL139: return 1; default:
_LL140: return 0;}} static struct _tagged_arr Cyc_sh_escape_string( struct
_tagged_arr s){ int _temp142= Cyc_String_strlen( s); int _temp143= 0; int
_temp144= 0;{ int i= 0; for( 0; i < _temp142; i ++){ unsigned char _temp145=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
if( _temp145 =='\''){ _temp143 ++;} else{ if( Cyc_is_other_special( _temp145)){
_temp144 ++;}}}} if( _temp143 == 0? _temp144 == 0: 0){ return s;} if( _temp143
== 0){ return( struct _tagged_arr) Cyc_String_strconcat_l(({ struct Cyc_List_List*(*
_temp146)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tagged_arr* _temp149= _init_tag_arr(( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)),"'", sizeof( unsigned char), 2u); struct
_tagged_arr* _temp150=({ struct _tagged_arr* _temp152=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp152[ 0]=( struct _tagged_arr) s;
_temp152;}); struct _tagged_arr* _temp151= _init_tag_arr(( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)),"'", sizeof( unsigned char), 2u); struct
_tagged_arr* _temp147[ 3u]={ _temp149, _temp150, _temp151}; struct _tagged_arr
_temp148={( void*) _temp147,( void*) _temp147,( void*)( _temp147 + 3u)};
_temp146( _temp148);}));}{ int _temp153=( _temp142 + _temp143) + _temp144;
struct _tagged_arr _temp154=({ unsigned int _temp158=( unsigned int)( _temp153 +
1); unsigned char* _temp159=( unsigned char*) GC_malloc_atomic( sizeof(
unsigned char) * _temp158); struct _tagged_arr _temp161= _tag_arr( _temp159,
sizeof( unsigned char),( unsigned int)( _temp153 + 1));{ unsigned int _temp160=
_temp158; unsigned int i; for( i= 0; i < _temp160; i ++){ _temp159[ i]='\000';}};
_temp161;}); int _temp155= 0; int _temp156= 0; for( 0; _temp155 < _temp142;
_temp155 ++){ unsigned char _temp157=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), _temp155)); if( _temp157 =='\''?
1: Cyc_is_other_special( _temp157)){*(( unsigned char*) _check_unknown_subscript(
_temp154, sizeof( unsigned char), _temp156 ++))='\\';}*(( unsigned char*)
_check_unknown_subscript( _temp154, sizeof( unsigned char), _temp156 ++))=
_temp157;} return( struct _tagged_arr) _temp154;}} static struct _tagged_arr*
Cyc_sh_escape_stringptr( struct _tagged_arr* sp){ return({ struct _tagged_arr*
_temp162=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp162[ 0]= Cyc_sh_escape_string(* sp); _temp162;});} static void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp163= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp164= Cyc_String_strconcat(( struct
_tagged_arr) _temp163, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct
_tagged_arr _temp165= Cyc_String_strconcat(( struct _tagged_arr) _temp163,
_tag_arr(".cyci", sizeof( unsigned char), 6u)); struct _tagged_arr _temp166= Cyc_String_strconcat((
struct _tagged_arr) _temp163, _tag_arr(".cycio", sizeof( unsigned char), 7u));
struct _tagged_arr _temp167= Cyc_String_strconcat(( struct _tagged_arr) _temp163,
_tag_arr(".c", sizeof( unsigned char), 3u)); if( Cyc_v_r){({ int(* _temp168)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp172= Cyc_Stdio_stderr; struct _tagged_arr
_temp173= _tag_arr("Compiling %s\n", sizeof( unsigned char), 14u); struct Cyc_Stdio_String_pa_struct
_temp174; _temp174.tag= Cyc_Stdio_String_pa; _temp174.f1=( struct _tagged_arr)
filename;{ void* _temp171=( void*)& _temp174; void* _temp169[ 1u]={ _temp171};
struct _tagged_arr _temp170={( void*) _temp169,( void*) _temp169,( void*)(
_temp169 + 1u)}; _temp168( _temp172, _temp173, _temp170);}});}{ struct Cyc_Stdio___sFILE*
f0= Cyc_try_file_open( filename, _tag_arr("r", sizeof( unsigned char), 2u),
_tag_arr("input file", sizeof( unsigned char), 11u)); if( Cyc_compile_failure){
return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*) _check_null( f0));{
struct _tagged_arr _temp175= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp259=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp259->hd=( void*) _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)),"", sizeof( unsigned char), 1u); _temp259->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( Cyc_cppargs)); _temp259;}), _tag_arr(" ", sizeof( unsigned char),
2u)); struct _tagged_arr stdinc_string= _tag_arr(" -Ic:/cyclone/include",
sizeof( unsigned char), 22u); struct _tagged_arr _temp176= Cyc_Stdlib_getenv(
_tag_arr("CYCLONE_INCLUDE_PATH", sizeof( unsigned char), 21u)); if( _temp176.curr
!=( _tag_arr( 0u, 0u, 0u)).curr){ struct Cyc_List_List* _temp177= Cyc_split_by_char((
struct _tagged_arr) _temp176,':'); struct _tagged_arr _temp178= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp187->hd=( void*) _init_tag_arr(( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp187->tl=(( struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct
_tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr,
_temp177); _temp187;}), _tag_arr(" -I", sizeof( unsigned char), 4u));
stdinc_string=( struct _tagged_arr)({ struct _tagged_arr(* _temp179)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp184= _tag_arr("%s%s", sizeof( unsigned char), 5u); struct Cyc_Stdio_String_pa_struct
_temp186; _temp186.tag= Cyc_Stdio_String_pa; _temp186.f1=( struct _tagged_arr)
_temp178;{ void* _temp182=( void*)& _temp186; struct Cyc_Stdio_String_pa_struct
_temp185; _temp185.tag= Cyc_Stdio_String_pa; _temp185.f1=( struct _tagged_arr)
stdinc_string;{ void* _temp183=( void*)& _temp185; void* _temp180[ 2u]={
_temp182, _temp183}; struct _tagged_arr _temp181={( void*) _temp180,( void*)
_temp180,( void*)( _temp180 + 2u)}; _temp179( _temp184, _temp181);}}});}{ struct
_tagged_arr ofile_string; if( Cyc_stop_after_cpp_r){ if( Cyc_output_file != 0){
ofile_string=( struct _tagged_arr)({ struct _tagged_arr(* _temp188)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp192= _tag_arr(" > %s", sizeof( unsigned char), 6u); struct Cyc_Stdio_String_pa_struct
_temp193; _temp193.tag= Cyc_Stdio_String_pa; _temp193.f1=( struct _tagged_arr)*((
struct _tagged_arr*) _check_null( Cyc_output_file));{ void* _temp191=( void*)&
_temp193; void* _temp189[ 1u]={ _temp191}; struct _tagged_arr _temp190={( void*)
_temp189,( void*) _temp189,( void*)( _temp189 + 1u)}; _temp188( _temp192,
_temp190);}});} else{ ofile_string= _tag_arr("", sizeof( unsigned char), 1u);}}
else{ ofile_string=( struct _tagged_arr)({ struct _tagged_arr(* _temp194)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp198= _tag_arr(" > %s", sizeof( unsigned char), 6u); struct Cyc_Stdio_String_pa_struct
_temp199; _temp199.tag= Cyc_Stdio_String_pa; _temp199.f1=( struct _tagged_arr)
Cyc_sh_escape_string(( struct _tagged_arr) _temp164);{ void* _temp197=( void*)&
_temp199; void* _temp195[ 1u]={ _temp197}; struct _tagged_arr _temp196={( void*)
_temp195,( void*) _temp195,( void*)( _temp195 + 1u)}; _temp194( _temp198,
_temp196);}});}{ struct _tagged_arr fixup_string; if( Cyc_produce_dependencies){
if( Cyc_dependencies_target == 0){ fixup_string= _tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof( unsigned char), 35u);} else{ fixup_string=( struct _tagged_arr)({ struct
_tagged_arr(* _temp200)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp204= _tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'", sizeof(
unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp205; _temp205.tag=
Cyc_Stdio_String_pa; _temp205.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_null( Cyc_dependencies_target));{ void* _temp203=( void*)& _temp205; void*
_temp201[ 1u]={ _temp203}; struct _tagged_arr _temp202={( void*) _temp201,( void*)
_temp201,( void*)( _temp201 + 1u)}; _temp200( _temp204, _temp202);}});}} else{
fixup_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct _tagged_arr
_temp206=({ struct _tagged_arr(* _temp245)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp253= _tag_arr("gcc -x c -E -U__GNUC__ -nostdinc%s%s %s%s%s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp258;
_temp258.tag= Cyc_Stdio_String_pa; _temp258.f1=( struct _tagged_arr) _temp175;{
void* _temp248=( void*)& _temp258; struct Cyc_Stdio_String_pa_struct _temp257;
_temp257.tag= Cyc_Stdio_String_pa; _temp257.f1=( struct _tagged_arr)
stdinc_string;{ void* _temp249=( void*)& _temp257; struct Cyc_Stdio_String_pa_struct
_temp256; _temp256.tag= Cyc_Stdio_String_pa; _temp256.f1=( struct _tagged_arr)
Cyc_sh_escape_string( filename);{ void* _temp250=( void*)& _temp256; struct Cyc_Stdio_String_pa_struct
_temp255; _temp255.tag= Cyc_Stdio_String_pa; _temp255.f1=( struct _tagged_arr)
fixup_string;{ void* _temp251=( void*)& _temp255; struct Cyc_Stdio_String_pa_struct
_temp254; _temp254.tag= Cyc_Stdio_String_pa; _temp254.f1=( struct _tagged_arr)
ofile_string;{ void* _temp252=( void*)& _temp254; void* _temp246[ 5u]={ _temp248,
_temp249, _temp250, _temp251, _temp252}; struct _tagged_arr _temp247={( void*)
_temp246,( void*) _temp246,( void*)( _temp246 + 5u)}; _temp245( _temp253,
_temp247);}}}}}}); if( Cyc_v_r){({ int(* _temp207)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp211= Cyc_Stdio_stderr; struct _tagged_arr _temp212= _tag_arr("%s\n",
sizeof( unsigned char), 4u); struct Cyc_Stdio_String_pa_struct _temp213;
_temp213.tag= Cyc_Stdio_String_pa; _temp213.f1=( struct _tagged_arr) _temp206;{
void* _temp210=( void*)& _temp213; void* _temp208[ 1u]={ _temp210}; struct
_tagged_arr _temp209={( void*) _temp208,( void*) _temp208,( void*)( _temp208 + 1u)};
_temp207( _temp211, _temp212, _temp209);}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp206)) != 0){ Cyc_compile_failure= 1;({ int(* _temp214)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp217= Cyc_Stdio_stderr; struct _tagged_arr
_temp218= _tag_arr("\nError: preprocessing\n", sizeof( unsigned char), 23u);
void* _temp215[ 0u]={}; struct _tagged_arr _temp216={( void*) _temp215,( void*)
_temp215,( void*)( _temp215 + 0u)}; _temp214( _temp217, _temp218, _temp216);});
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp164);{ struct Cyc_Stdio___sFILE* in_file= Cyc_try_file_open((
struct _tagged_arr) _temp164, _tag_arr("r", sizeof( unsigned char), 2u),
_tag_arr("file", sizeof( unsigned char), 5u)); if( Cyc_compile_failure){ return;}{
struct Cyc_List_List* tds= 0;{ struct _handler_cons _temp219; _push_handler(&
_temp219);{ int _temp221= 0; if( setjmp( _temp219.handler)){ _temp221= 1;} if( !
_temp221){ tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*),
struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("parsing", sizeof( unsigned char), 8u), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)
_check_null( in_file), tds);; _pop_handler();} else{ void* _temp220=( void*)
_exn_thrown; void* _temp223= _temp220; _LL225: goto _LL226; _LL227: goto _LL228;
_LL226: Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*) _check_null( in_file));
Cyc_remove_file(( struct _tagged_arr) _temp164);( int) _throw( _temp223); _LL228:(
void) _throw( _temp223); _LL224:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( in_file)); if( Cyc_compile_failure){ Cyc_remove_file(( struct
_tagged_arr) _temp164); return;}{ struct Cyc_Tcenv_Tenv* _temp229= Cyc_Tcenv_tc_init();
if( Cyc_parseonly_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct
_tagged_arr stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds))
Cyc_do_stage)( _tag_arr("type checking", sizeof( unsigned char), 14u), Cyc_do_typecheck,
_temp229, tds); if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr)
_temp164); return;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr
stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("control-flow checking",
sizeof( unsigned char), 22u), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){
Cyc_Tcutil_flush_warnings();} Cyc_remove_file(( struct _tagged_arr) _temp164);
if( Cyc_compile_failure){ return;} if( Cyc_ic_r){ struct Cyc_Stdio___sFILE*
inter_file= Cyc_Stdio_fopen(( struct _tagged_arr) _temp165, _tag_arr("r",
sizeof( unsigned char), 2u)); struct Cyc_Stdio___sFILE* inter_objfile= Cyc_try_file_open((
struct _tagged_arr) _temp166, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("interface object file", sizeof( unsigned char), 22u)); if(
inter_objfile == 0){ Cyc_compile_failure= 1; return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp165); tds=(( struct Cyc_List_List*(*)( struct
_tagged_arr stage_name, struct Cyc_List_List*(* f)( struct _tuple10*, struct Cyc_List_List*),
struct _tuple10* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("interface checking",
sizeof( unsigned char), 19u), Cyc_do_interface,({ struct _tuple10* _temp230=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp230->f1= _temp229;
_temp230->f2= inter_file; _temp230->f3=( struct Cyc_Stdio___sFILE*) _check_null(
inter_objfile); _temp230;}), tds); if( inter_file != 0){ Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_file));} Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto
PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct
Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds)) Cyc_do_stage)( _tag_arr("translation to C", sizeof( unsigned char), 17u),
Cyc_do_translate, 1, tds); if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){
tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("post-pass to VC", sizeof( unsigned char), 16u), Cyc_do_tovc, 1, tds);}
if( Cyc_compile_failure){ return;} PRINTC: if( tds != 0){ struct Cyc_Stdio___sFILE*
out_file; if( Cyc_parseonly_r? 1: Cyc_tc_r){ if( Cyc_output_file != 0){ out_file=
Cyc_try_file_open(*(( struct _tagged_arr*) _check_null( Cyc_output_file)),
_tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("output file", sizeof(
unsigned char), 12u));} else{ out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;}}
else{ if( Cyc_toc_r? Cyc_output_file != 0: 0){ out_file= Cyc_try_file_open(*((
struct _tagged_arr*) _check_null( Cyc_output_file)), _tag_arr("w", sizeof(
unsigned char), 2u), _tag_arr("output file", sizeof( unsigned char), 12u));}
else{ out_file= Cyc_try_file_open(( struct _tagged_arr) _temp167, _tag_arr("w",
sizeof( unsigned char), 2u), _tag_arr("output file", sizeof( unsigned char), 12u));}}
if( Cyc_compile_failure){ return;}{ struct _handler_cons _temp231; _push_handler(&
_temp231);{ int _temp233= 0; if( setjmp( _temp231.handler)){ _temp233= 1;} if( !
_temp233){ tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*),
struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("printing", sizeof( unsigned char), 9u), Cyc_do_print,( struct Cyc_Stdio___sFILE*)
_check_null( out_file), tds);; _pop_handler();} else{ void* _temp232=( void*)
_exn_thrown; void* _temp235= _temp232; _LL237: goto _LL238; _LL239: goto _LL240;
_LL238: Cyc_compile_failure= 1; Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp241=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp241->hd=( void*)({
struct _tagged_arr* _temp242=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp242[ 0]=( struct _tagged_arr) _temp167; _temp242;});
_temp241->tl= Cyc_cfiles; _temp241;});( int) _throw( _temp235); _LL240:( void)
_throw( _temp235); _LL236:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp243=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp243->hd=( void*)({
struct _tagged_arr* _temp244=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp244[ 0]=( struct _tagged_arr) _temp167; _temp244;});
_temp243->tl= Cyc_cfiles; _temp243;});}}}}}}}}}} static unsigned char _temp260[
8u]="<final>"; static struct _tagged_arr Cyc_final_str={ _temp260, _temp260,
_temp260 + 8u}; static struct _tagged_arr* Cyc_final_strptr=& Cyc_final_str;
static struct Cyc_Interface_I* Cyc_read_cycio( struct _tagged_arr* n){ if( n ==
Cyc_final_strptr){ return Cyc_Interface_final();}{ struct _tagged_arr basename;{
struct _handler_cons _temp261; _push_handler(& _temp261);{ int _temp263= 0; if(
setjmp( _temp261.handler)){ _temp263= 1;} if( ! _temp263){ basename=( struct
_tagged_arr) Cyc_Filename_chop_extension(* n);; _pop_handler();} else{ void*
_temp262=( void*) _exn_thrown; void* _temp265= _temp262; _LL267: if(*(( void**)
_temp265) == Cyc_Core_InvalidArg){ goto _LL268;} else{ goto _LL269;} _LL269:
goto _LL270; _LL268: basename=* n; goto _LL266; _LL270:( void) _throw( _temp265);
_LL266:;}}}{ struct _tagged_arr _temp271= Cyc_String_strconcat( basename,
_tag_arr(".cycio", sizeof( unsigned char), 7u)); struct Cyc_Stdio___sFILE*
_temp272= Cyc_try_file_open(( struct _tagged_arr) _temp271, _tag_arr("r",
sizeof( unsigned char), 2u), _tag_arr("interface object file", sizeof(
unsigned char), 22u)); if( _temp272 == 0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} Cyc_Position_reset_position(( struct _tagged_arr) _temp271);{ struct
Cyc_Interface_I* _temp273= Cyc_Interface_load(( struct Cyc_Stdio___sFILE*)
_check_null( _temp272)); Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( _temp272)); return _temp273;}}}} static int Cyc_is_cfile( struct
_tagged_arr* n){ return*(( const unsigned char*) _check_unknown_subscript(* n,
sizeof( unsigned char), 0)) !='-';} struct _tuple11{ struct _tagged_arr f1; void*
f2; struct _tagged_arr f3; } ; int Cyc_main( int argc, struct _tagged_arr argv){
struct Cyc_List_List* options=({ struct Cyc_List_List*(* _temp406)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct _tuple11* _temp409=({ struct _tuple11* _temp554=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp554->f1= _tag_arr("-v", sizeof(
unsigned char), 3u); _temp554->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp555=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp555[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp556; _temp556.tag= Cyc_Arg_Set_spec;
_temp556.f1=& Cyc_v_r; _temp556;}); _temp555;}); _temp554->f3= _tag_arr("    Print compilation stages verbosely",
sizeof( unsigned char), 39u); _temp554;}); struct _tuple11* _temp410=({ struct
_tuple11* _temp551=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp551->f1= _tag_arr("-o", sizeof( unsigned char), 3u); _temp551->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp552=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp552[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp553; _temp553.tag= Cyc_Arg_String_spec; _temp553.f1=
Cyc_set_output_file; _temp553;}); _temp552;}); _temp551->f3= _tag_arr("    Output file name",
sizeof( unsigned char), 21u); _temp551;}); struct _tuple11* _temp411=({ struct
_tuple11* _temp548=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp548->f1= _tag_arr("-D*", sizeof( unsigned char), 4u); _temp548->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp549=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp549[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp550; _temp550.tag= Cyc_Arg_Flag_spec; _temp550.f1= Cyc_add_cpparg; _temp550;});
_temp549;}); _temp548->f3= _tag_arr("   Pass definition to preprocessor",
sizeof( unsigned char), 35u); _temp548;}); struct _tuple11* _temp412=({ struct
_tuple11* _temp545=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp545->f1= _tag_arr("-B*", sizeof( unsigned char), 4u); _temp545->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp546=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp546[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp547; _temp547.tag= Cyc_Arg_Flag_spec; _temp547.f1= Cyc_add_cyclone_exec_prefix_path;
_temp547;}); _temp546;}); _temp545->f3= _tag_arr("   Add to the list of directories to search for compiler files",
sizeof( unsigned char), 63u); _temp545;}); struct _tuple11* _temp413=({ struct
_tuple11* _temp542=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp542->f1= _tag_arr("-I*", sizeof( unsigned char), 4u); _temp542->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp543=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp543[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp544; _temp544.tag= Cyc_Arg_Flag_spec; _temp544.f1= Cyc_add_cpparg; _temp544;});
_temp543;}); _temp542->f3= _tag_arr("   Add to the list of directories to search for include files",
sizeof( unsigned char), 62u); _temp542;}); struct _tuple11* _temp414=({ struct
_tuple11* _temp539=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp539->f1= _tag_arr("-L*", sizeof( unsigned char), 4u); _temp539->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp540=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp540[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp541; _temp541.tag= Cyc_Arg_Flag_spec; _temp541.f1= Cyc_add_cpparg; _temp541;});
_temp540;}); _temp539->f3= _tag_arr("   Add to the list of directories for -l",
sizeof( unsigned char), 41u); _temp539;}); struct _tuple11* _temp415=({ struct
_tuple11* _temp536=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp536->f1= _tag_arr("-l*", sizeof( unsigned char), 4u); _temp536->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp537=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp537[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp538; _temp538.tag= Cyc_Arg_Flag_spec; _temp538.f1= Cyc_add_libarg; _temp538;});
_temp537;}); _temp536->f3= _tag_arr("   Library file", sizeof( unsigned char),
16u); _temp536;}); struct _tuple11* _temp416=({ struct _tuple11* _temp533=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp533->f1= _tag_arr("-c",
sizeof( unsigned char), 3u); _temp533->f2=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp534=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp534[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp535; _temp535.tag= Cyc_Arg_Unit_spec;
_temp535.f1= Cyc_set_stop_after_objectfile; _temp535;}); _temp534;}); _temp533->f3=
_tag_arr("    Produce object file", sizeof( unsigned char), 24u); _temp533;});
struct _tuple11* _temp417=({ struct _tuple11* _temp530=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp530->f1= _tag_arr("-s", sizeof(
unsigned char), 3u); _temp530->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp531=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp531[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp532; _temp532.tag= Cyc_Arg_Flag_spec;
_temp532.f1= Cyc_add_ccarg; _temp532;}); _temp531;}); _temp530->f3= _tag_arr("    Remove all symbol table and relocation info from executable",
sizeof( unsigned char), 64u); _temp530;}); struct _tuple11* _temp418=({ struct
_tuple11* _temp527=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp527->f1= _tag_arr("-O", sizeof( unsigned char), 3u); _temp527->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp528=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp528[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp529; _temp529.tag= Cyc_Arg_Flag_spec; _temp529.f1= Cyc_add_ccarg; _temp529;});
_temp528;}); _temp527->f3= _tag_arr("    Optimize", sizeof( unsigned char), 13u);
_temp527;}); struct _tuple11* _temp419=({ struct _tuple11* _temp524=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp524->f1= _tag_arr("-O2",
sizeof( unsigned char), 4u); _temp524->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp525=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp525[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp526; _temp526.tag= Cyc_Arg_Flag_spec;
_temp526.f1= Cyc_add_ccarg; _temp526;}); _temp525;}); _temp524->f3= _tag_arr("   Optimize",
sizeof( unsigned char), 12u); _temp524;}); struct _tuple11* _temp420=({ struct
_tuple11* _temp521=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp521->f1= _tag_arr("-O3", sizeof( unsigned char), 4u); _temp521->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp522=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp522[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp523; _temp523.tag= Cyc_Arg_Flag_spec; _temp523.f1= Cyc_add_ccarg; _temp523;});
_temp522;}); _temp521->f3= _tag_arr("   Optimize", sizeof( unsigned char), 12u);
_temp521;}); struct _tuple11* _temp421=({ struct _tuple11* _temp518=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp518->f1= _tag_arr("-g",
sizeof( unsigned char), 3u); _temp518->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp519=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp519[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp520; _temp520.tag= Cyc_Arg_Flag_spec;
_temp520.f1= Cyc_add_ccarg; _temp520;}); _temp519;}); _temp518->f3= _tag_arr("    Compile for debugging",
sizeof( unsigned char), 26u); _temp518;}); struct _tuple11* _temp422=({ struct
_tuple11* _temp515=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp515->f1= _tag_arr("-p", sizeof( unsigned char), 3u); _temp515->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp516=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp516[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp517; _temp517.tag= Cyc_Arg_Flag_spec; _temp517.f1= Cyc_add_ccarg; _temp517;});
_temp516;}); _temp515->f3= _tag_arr("    Compile for prof", sizeof(
unsigned char), 21u); _temp515;}); struct _tuple11* _temp423=({ struct _tuple11*
_temp512=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp512->f1=
_tag_arr("-pg", sizeof( unsigned char), 4u); _temp512->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp513=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp513[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp514; _temp514.tag= Cyc_Arg_Flag_spec;
_temp514.f1= Cyc_add_ccarg; _temp514;}); _temp513;}); _temp512->f3= _tag_arr("   Compile for gprof",
sizeof( unsigned char), 21u); _temp512;}); struct _tuple11* _temp424=({ struct
_tuple11* _temp509=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp509->f1= _tag_arr("-pa", sizeof( unsigned char), 4u); _temp509->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp510=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp510[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp511; _temp511.tag= Cyc_Arg_Unit_spec; _temp511.f1= Cyc_set_pa; _temp511;});
_temp510;}); _temp509->f3= _tag_arr("   Compile for aprof", sizeof(
unsigned char), 21u); _temp509;}); struct _tuple11* _temp425=({ struct _tuple11*
_temp506=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp506->f1=
_tag_arr("-S", sizeof( unsigned char), 3u); _temp506->f2=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp507=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp507[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp508; _temp508.tag= Cyc_Arg_Unit_spec;
_temp508.f1= Cyc_set_stop_after_asmfile; _temp508;}); _temp507;}); _temp506->f3=
_tag_arr("    Stop after producing assembly code", sizeof( unsigned char), 39u);
_temp506;}); struct _tuple11* _temp426=({ struct _tuple11* _temp503=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp503->f1= _tag_arr("-M",
sizeof( unsigned char), 3u); _temp503->f2=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp504=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp504[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp505; _temp505.tag= Cyc_Arg_Unit_spec;
_temp505.f1= Cyc_set_produce_dependencies; _temp505;}); _temp504;}); _temp503->f3=
_tag_arr("    Produce dependencies", sizeof( unsigned char), 25u); _temp503;});
struct _tuple11* _temp427=({ struct _tuple11* _temp500=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp500->f1= _tag_arr("-MG", sizeof(
unsigned char), 4u); _temp500->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp501=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp501[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp502; _temp502.tag= Cyc_Arg_Flag_spec;
_temp502.f1= Cyc_add_cpparg; _temp502;}); _temp501;}); _temp500->f3= _tag_arr("   When producing dependencies assume missing files are generated",
sizeof( unsigned char), 66u); _temp500;}); struct _tuple11* _temp428=({ struct
_tuple11* _temp497=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp497->f1= _tag_arr("-MT", sizeof( unsigned char), 4u); _temp497->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp498=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp498[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp499; _temp499.tag= Cyc_Arg_String_spec; _temp499.f1=
Cyc_set_dependencies_target; _temp499;}); _temp498;}); _temp497->f3= _tag_arr("   Give target for dependencies",
sizeof( unsigned char), 32u); _temp497;}); struct _tuple11* _temp429=({ struct
_tuple11* _temp494=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp494->f1= _tag_arr("-E", sizeof( unsigned char), 3u); _temp494->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp495=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp495[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp496; _temp496.tag= Cyc_Arg_Set_spec; _temp496.f1=& Cyc_stop_after_cpp_r;
_temp496;}); _temp495;}); _temp494->f3= _tag_arr("    Stop after preprocessing",
sizeof( unsigned char), 29u); _temp494;}); struct _tuple11* _temp430=({ struct
_tuple11* _temp491=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp491->f1= _tag_arr("-parseonly", sizeof( unsigned char), 11u); _temp491->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp492=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp492[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp493; _temp493.tag= Cyc_Arg_Set_spec; _temp493.f1=& Cyc_parseonly_r;
_temp493;}); _temp492;}); _temp491->f3= _tag_arr("\n        Stop after parsing",
sizeof( unsigned char), 28u); _temp491;}); struct _tuple11* _temp431=({ struct
_tuple11* _temp488=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp488->f1= _tag_arr("-tc", sizeof( unsigned char), 4u); _temp488->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp489=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp489[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp490; _temp490.tag= Cyc_Arg_Set_spec; _temp490.f1=& Cyc_tc_r; _temp490;});
_temp489;}); _temp488->f3= _tag_arr("   Stop after type checking", sizeof(
unsigned char), 28u); _temp488;}); struct _tuple11* _temp432=({ struct _tuple11*
_temp485=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp485->f1=
_tag_arr("-toc", sizeof( unsigned char), 5u); _temp485->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp486=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp486[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp487; _temp487.tag= Cyc_Arg_Set_spec;
_temp487.f1=& Cyc_toc_r; _temp487;}); _temp486;}); _temp485->f3= _tag_arr("  Stop after translation to C",
sizeof( unsigned char), 30u); _temp485;}); struct _tuple11* _temp433=({ struct
_tuple11* _temp482=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp482->f1= _tag_arr("-ic", sizeof( unsigned char), 4u); _temp482->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp483=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp483[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp484; _temp484.tag= Cyc_Arg_Set_spec; _temp484.f1=& Cyc_ic_r; _temp484;});
_temp483;}); _temp482->f3= _tag_arr("   Activate the link-checker", sizeof(
unsigned char), 29u); _temp482;}); struct _tuple11* _temp434=({ struct _tuple11*
_temp479=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp479->f1=
_tag_arr("-pp", sizeof( unsigned char), 4u); _temp479->f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp480=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp480[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp481; _temp481.tag= Cyc_Arg_Set_spec;
_temp481.f1=& Cyc_pp_r; _temp481;}); _temp480;}); _temp479->f3= _tag_arr("   Pretty print",
sizeof( unsigned char), 16u); _temp479;}); struct _tuple11* _temp435=({ struct
_tuple11* _temp476=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp476->f1= _tag_arr("-up", sizeof( unsigned char), 4u); _temp476->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp477=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp477[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp478; _temp478.tag= Cyc_Arg_Clear_spec; _temp478.f1=&
Cyc_pp_r; _temp478;}); _temp477;}); _temp476->f3= _tag_arr("   Ugly print",
sizeof( unsigned char), 14u); _temp476;}); struct _tuple11* _temp436=({ struct
_tuple11* _temp473=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp473->f1= _tag_arr("-tovc", sizeof( unsigned char), 6u); _temp473->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp474=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp474[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp475; _temp475.tag= Cyc_Arg_Set_spec; _temp475.f1=& Cyc_tovc_r; _temp475;});
_temp474;}); _temp473->f3= _tag_arr(" Avoid gcc extensions in C output", sizeof(
unsigned char), 34u); _temp473;}); struct _tuple11* _temp437=({ struct _tuple11*
_temp470=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp470->f1=
_tag_arr("-save-temps", sizeof( unsigned char), 12u); _temp470->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp471=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp471[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp472; _temp472.tag= Cyc_Arg_Unit_spec; _temp472.f1= Cyc_set_save_temps;
_temp472;}); _temp471;}); _temp470->f3= _tag_arr("\n        Don't delete temporary files",
sizeof( unsigned char), 38u); _temp470;}); struct _tuple11* _temp438=({ struct
_tuple11* _temp467=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp467->f1= _tag_arr("-save-c", sizeof( unsigned char), 8u); _temp467->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp468=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp468[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp469; _temp469.tag= Cyc_Arg_Set_spec; _temp469.f1=& Cyc_save_c_r; _temp469;});
_temp468;}); _temp467->f3= _tag_arr("\n        Don't delete temporary C files",
sizeof( unsigned char), 40u); _temp467;}); struct _tuple11* _temp439=({ struct
_tuple11* _temp464=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp464->f1= _tag_arr("-nocyc", sizeof( unsigned char), 7u); _temp464->f2=(
void*)({ struct Cyc_Arg_Unit_spec_struct* _temp465=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp465[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp466; _temp466.tag= Cyc_Arg_Unit_spec; _temp466.f1= Cyc_set_nocyc; _temp466;});
_temp465;}); _temp464->f3= _tag_arr("Don't add implicit namespace Cyc", sizeof(
unsigned char), 33u); _temp464;}); struct _tuple11* _temp440=({ struct _tuple11*
_temp461=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp461->f1=
_tag_arr("-nogc", sizeof( unsigned char), 6u); _temp461->f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp462=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp462[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp463; _temp463.tag= Cyc_Arg_Set_spec; _temp463.f1=& Cyc_nogc_r; _temp463;});
_temp462;}); _temp461->f3= _tag_arr(" Don't link in the garbage collector",
sizeof( unsigned char), 37u); _temp461;}); struct _tuple11* _temp441=({ struct
_tuple11* _temp458=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp458->f1= _tag_arr("-noremoveunused", sizeof( unsigned char), 16u); _temp458->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp459=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp459[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp460; _temp460.tag= Cyc_Arg_Set_spec; _temp460.f1=& Cyc_noshake_r; _temp460;});
_temp459;}); _temp458->f3= _tag_arr("\n        Don't remove externed variables that aren't used",
sizeof( unsigned char), 58u); _temp458;}); struct _tuple11* _temp442=({ struct
_tuple11* _temp455=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp455->f1= _tag_arr("-noexpandtypedefs", sizeof( unsigned char), 18u);
_temp455->f2=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp456=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp456[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp457; _temp457.tag= Cyc_Arg_Set_spec; _temp457.f1=& Cyc_noexpand_r; _temp457;});
_temp456;}); _temp455->f3= _tag_arr("\n        Don't expand typedefs in pretty printing",
sizeof( unsigned char), 50u); _temp455;}); struct _tuple11* _temp443=({ struct
_tuple11* _temp452=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp452->f1= _tag_arr("-printalltvars", sizeof( unsigned char), 15u); _temp452->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp453=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp453[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp454; _temp454.tag= Cyc_Arg_Set_spec; _temp454.f1=& Cyc_print_all_tvars_r;
_temp454;}); _temp453;}); _temp452->f3= _tag_arr("\n        Print all type variables (even implicit default effects)",
sizeof( unsigned char), 66u); _temp452;}); struct _tuple11* _temp444=({ struct
_tuple11* _temp449=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp449->f1= _tag_arr("-printallkinds", sizeof( unsigned char), 15u); _temp449->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp450=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp450[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp451; _temp451.tag= Cyc_Arg_Set_spec; _temp451.f1=& Cyc_print_all_kinds_r;
_temp451;}); _temp450;}); _temp449->f3= _tag_arr("\n        Always print kinds of type variables",
sizeof( unsigned char), 46u); _temp449;}); struct _tuple11* _temp445=({ struct
_tuple11* _temp446=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp446->f1= _tag_arr("-printfullevars", sizeof( unsigned char), 16u); _temp446->f2=(
void*)({ struct Cyc_Arg_Set_spec_struct* _temp447=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp447[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp448; _temp448.tag= Cyc_Arg_Set_spec; _temp448.f1=& Cyc_print_full_evars_r;
_temp448;}); _temp447;}); _temp446->f3= _tag_arr("\n        Print full information for evars (type debugging)",
sizeof( unsigned char), 59u); _temp446;}); struct _tuple11* _temp407[ 37u]={
_temp409, _temp410, _temp411, _temp412, _temp413, _temp414, _temp415, _temp416,
_temp417, _temp418, _temp419, _temp420, _temp421, _temp422, _temp423, _temp424,
_temp425, _temp426, _temp427, _temp428, _temp429, _temp430, _temp431, _temp432,
_temp433, _temp434, _temp435, _temp436, _temp437, _temp438, _temp439, _temp440,
_temp441, _temp442, _temp443, _temp444, _temp445}; struct _tagged_arr _temp408={(
void*) _temp407,( void*) _temp407,( void*)( _temp407 + 37u)}; _temp406( _temp408);});
Cyc_Arg_parse( options, Cyc_add_other, _tag_arr("Options:", sizeof(
unsigned char), 9u), argv);{ struct Cyc_List_List* _temp274=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); for( 0; _temp274 !=
0; _temp274=(( struct Cyc_List_List*) _check_null( _temp274))->tl){ Cyc_process_file(*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp274))->hd)); if(
Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)?
1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs == 0){ return 0;}{ struct
_tagged_arr cyclone_exec_prefix=( struct _tagged_arr) Cyc_Stdlib_getenv(
_tag_arr("CYCLONE_EXEC_PREFIX", sizeof( unsigned char), 20u)); if(
cyclone_exec_prefix.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp275=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp275->hd=( void*)({ struct
_tagged_arr* _temp276=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp276[ 0]= cyclone_exec_prefix; _temp276;}); _temp275->tl= Cyc_cyclone_exec_prefix_path;
_temp275;});} Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp277=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp277->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"c:/cyclone/bin/cyc-lib", sizeof( unsigned char), 23u); _temp277->tl=
Cyc_cyclone_exec_prefix_path; _temp277;}); Cyc_cyclone_exec_prefix_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_exec_prefix_path);{
struct _tagged_arr _temp278= Cyc_do_find( Cyc_cyclone_exec_prefix_path, _tag_arr("include/cyc_include.h",
sizeof( unsigned char), 22u)); Cyc_ccargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{ struct _tagged_arr _temp279= Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tagged_arr*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr, Cyc_ccargs), _tag_arr(" ",
sizeof( unsigned char), 2u)); Cyc_libargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_libargs);{ struct _tagged_arr _temp280=
Cyc_String_str_sepstr(({ struct Cyc_List_List* _temp405=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp405->hd=( void*) _init_tag_arr((
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"", sizeof(
unsigned char), 1u); _temp405->tl=(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_libargs); _temp405;}), _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_List_List* stdlib; struct _tagged_arr
stdlib_string; int _temp281=(( Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:( Cyc_output_file != 0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".a", sizeof( unsigned char), 3u)): 0))?
1:( Cyc_output_file != 0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".lib", sizeof( unsigned char), 5u)): 0);
if( _temp281){ stdlib= 0; stdlib_string= _tag_arr("", sizeof( unsigned char), 1u);}
else{ struct _tagged_arr libcyc_filename= Cyc_pa_r? _tag_arr("libcyc_a.a",
sizeof( unsigned char), 11u): _tag_arr("libcyc.a", sizeof( unsigned char), 9u);
struct _tagged_arr nogc_filename= Cyc_pa_r? _tag_arr("nogc_a.a", sizeof(
unsigned char), 9u): _tag_arr("nogc.a", sizeof( unsigned char), 7u); struct
_tagged_arr _temp282= Cyc_do_find( Cyc_cyclone_exec_prefix_path, libcyc_filename);
struct _tagged_arr _temp283= Cyc_nogc_r? Cyc_do_find( Cyc_cyclone_exec_prefix_path,
nogc_filename): Cyc_do_find( Cyc_cyclone_exec_prefix_path, _tag_arr("gc.a",
sizeof( unsigned char), 5u)); stdlib=({ struct Cyc_List_List*(* _temp284)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct _tagged_arr* _temp287=({ struct _tagged_arr* _temp288=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp288[ 0]= _temp282;
_temp288;}); struct _tagged_arr* _temp285[ 1u]={ _temp287}; struct _tagged_arr
_temp286={( void*) _temp285,( void*) _temp285,( void*)( _temp285 + 1u)};
_temp284( _temp286);}); stdlib_string=( struct _tagged_arr)({ struct _tagged_arr(*
_temp289)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp294= _tag_arr(" %s %s", sizeof( unsigned char), 7u);
struct Cyc_Stdio_String_pa_struct _temp296; _temp296.tag= Cyc_Stdio_String_pa;
_temp296.f1=( struct _tagged_arr) _temp282;{ void* _temp292=( void*)& _temp296;
struct Cyc_Stdio_String_pa_struct _temp295; _temp295.tag= Cyc_Stdio_String_pa;
_temp295.f1=( struct _tagged_arr) _temp283;{ void* _temp293=( void*)& _temp295;
void* _temp290[ 2u]={ _temp292, _temp293}; struct _tagged_arr _temp291={( void*)
_temp290,( void*) _temp290,( void*)( _temp290 + 2u)}; _temp289( _temp294,
_temp291);}}});} if( Cyc_ic_r){ struct _handler_cons _temp297; _push_handler(&
_temp297);{ int _temp299= 0; if( setjmp( _temp297.handler)){ _temp299= 1;} if( !
_temp299){ Cyc_ccargs=(( struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*),
struct Cyc_List_List* l)) Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=((
struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*), struct Cyc_List_List*
l)) Cyc_List_filter)( Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List*
_temp300=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( stdlib,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_ccargs, Cyc_libargs)); if( !
_temp281){ _temp300=({ struct Cyc_List_List* _temp301=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp301->hd=( void*) Cyc_final_strptr;
_temp301->tl= _temp300; _temp301;});}{ struct Cyc_Interface_I* _temp302=((
struct Cyc_Interface_I*(*)( struct Cyc_Interface_I*(* get)( struct _tagged_arr*),
struct Cyc_List_List* la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio, _temp300, _temp300); if( _temp302 == 0){({ int(* _temp303)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp306= Cyc_Stdio_stderr; struct _tagged_arr
_temp307= _tag_arr("Error: interfaces incompatible\n", sizeof( unsigned char),
32u); void* _temp304[ 0u]={}; struct _tagged_arr _temp305={( void*) _temp304,(
void*) _temp304,( void*)( _temp304 + 0u)}; _temp303( _temp306, _temp307,
_temp305);}); Cyc_compile_failure= 1; Cyc_remove_cfiles();{ int _temp308= 1;
_npop_handler( 0u); return _temp308;}} if( _temp281){ if( Cyc_output_file != 0){
struct _tagged_arr _temp309=({ struct _tagged_arr(* _temp312)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp316= _tag_arr("%s.cycio", sizeof( unsigned char), 9u); struct Cyc_Stdio_String_pa_struct
_temp317; _temp317.tag= Cyc_Stdio_String_pa; _temp317.f1=( struct _tagged_arr)
Cyc_Filename_chop_extension(*(( struct _tagged_arr*) _check_null( Cyc_output_file)));{
void* _temp315=( void*)& _temp317; void* _temp313[ 1u]={ _temp315}; struct
_tagged_arr _temp314={( void*) _temp313,( void*) _temp313,( void*)( _temp313 + 1u)};
_temp312( _temp316, _temp314);}}); struct Cyc_Stdio___sFILE* _temp310= Cyc_try_file_open((
struct _tagged_arr) _temp309, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("interface object file", sizeof( unsigned char), 22u)); if( _temp310 ==
0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{ int _temp311= 1; _npop_handler(
0u); return _temp311;}} Cyc_Interface_save(( struct Cyc_Interface_I*)
_check_null( _temp302),( struct Cyc_Stdio___sFILE*) _check_null( _temp310)); Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( _temp310));}} else{ if( ! Cyc_Interface_is_subinterface(
Cyc_Interface_empty(),( struct Cyc_Interface_I*) _check_null( _temp302),({
struct _tuple9* _temp318=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp318->f1= _tag_arr("empty interface", sizeof( unsigned char), 16u); _temp318->f2=
_tag_arr("global interface", sizeof( unsigned char), 17u); _temp318;}))){({ int(*
_temp319)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp322= Cyc_Stdio_stderr; struct
_tagged_arr _temp323= _tag_arr("Error: some objects are still undefined\n",
sizeof( unsigned char), 41u); void* _temp320[ 0u]={}; struct _tagged_arr
_temp321={( void*) _temp320,( void*) _temp320,( void*)( _temp320 + 0u)};
_temp319( _temp322, _temp323, _temp321);}); Cyc_compile_failure= 1; Cyc_remove_cfiles();{
int _temp324= 1; _npop_handler( 0u); return _temp324;}}}}}; _pop_handler();}
else{ void* _temp298=( void*) _exn_thrown; void* _temp326= _temp298; struct
_tagged_arr _temp340; struct _tagged_arr _temp342; struct _tagged_arr _temp344;
_LL328: if(*(( void**) _temp326) == Cyc_Core_Failure){ _LL341: _temp340=((
struct Cyc_Core_Failure_struct*) _temp326)->f1; goto _LL329;} else{ goto _LL330;}
_LL330: if(*(( void**) _temp326) == Cyc_Core_Impossible){ _LL343: _temp342=((
struct Cyc_Core_Impossible_struct*) _temp326)->f1; goto _LL331;} else{ goto
_LL332;} _LL332: if( _temp326 == Cyc_Dict_Absent){ goto _LL333;} else{ goto
_LL334;} _LL334: if(*(( void**) _temp326) == Cyc_Core_InvalidArg){ _LL345:
_temp344=(( struct Cyc_Core_InvalidArg_struct*) _temp326)->f1; goto _LL335;}
else{ goto _LL336;} _LL336: goto _LL337; _LL338: goto _LL339; _LL329:({ int(*
_temp346)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_printf; struct
_tagged_arr _temp350= _tag_arr("Exception Core::Failure %s\n", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp351; _temp351.tag=
Cyc_Stdio_String_pa; _temp351.f1=( struct _tagged_arr) _temp340;{ void* _temp349=(
void*)& _temp351; void* _temp347[ 1u]={ _temp349}; struct _tagged_arr _temp348={(
void*) _temp347,( void*) _temp347,( void*)( _temp347 + 1u)}; _temp346( _temp350,
_temp348);}}); Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1; _LL331:({
int(* _temp352)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_printf;
struct _tagged_arr _temp356= _tag_arr("Exception Core::Impossible %s\n", sizeof(
unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp357; _temp357.tag=
Cyc_Stdio_String_pa; _temp357.f1=( struct _tagged_arr) _temp342;{ void* _temp355=(
void*)& _temp357; void* _temp353[ 1u]={ _temp355}; struct _tagged_arr _temp354={(
void*) _temp353,( void*) _temp353,( void*)( _temp353 + 1u)}; _temp352( _temp356,
_temp354);}}); Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1; _LL333:({
int(* _temp358)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_printf;
struct _tagged_arr _temp361= _tag_arr("Exception Dict::Absent\n", sizeof(
unsigned char), 24u); void* _temp359[ 0u]={}; struct _tagged_arr _temp360={(
void*) _temp359,( void*) _temp359,( void*)( _temp359 + 0u)}; _temp358( _temp361,
_temp360);}); Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1; _LL335:({
int(* _temp362)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_printf;
struct _tagged_arr _temp366= _tag_arr("Exception Core::InvalidArg %s\n", sizeof(
unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp367; _temp367.tag=
Cyc_Stdio_String_pa; _temp367.f1=( struct _tagged_arr) _temp344;{ void* _temp365=(
void*)& _temp367; void* _temp363[ 1u]={ _temp365}; struct _tagged_arr _temp364={(
void*) _temp363,( void*) _temp363,( void*)( _temp363 + 1u)}; _temp362( _temp366,
_temp364);}}); Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1; _LL337:({
int(* _temp368)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_printf;
struct _tagged_arr _temp371= _tag_arr("Uncaught exception\n", sizeof(
unsigned char), 20u); void* _temp369[ 0u]={}; struct _tagged_arr _temp370={(
void*) _temp369,( void*) _temp369,( void*)( _temp369 + 0u)}; _temp368( _temp371,
_temp370);}); Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1; _LL339:(
void) _throw( _temp326); _LL327:;}}}{ struct _tagged_arr _temp372=({ struct
_tagged_arr(* _temp385)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp393= _tag_arr("gcc -include %s%s %s%s%s", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp404; _temp404.tag=
Cyc_Stdio_String_pa; _temp404.f1=( struct _tagged_arr) _temp278;{ void* _temp388=(
void*)& _temp404; struct Cyc_Stdio_String_pa_struct _temp403; _temp403.tag= Cyc_Stdio_String_pa;
_temp403.f1=( struct _tagged_arr)( Cyc_output_file == 0? _tag_arr("", sizeof(
unsigned char), 1u):( struct _tagged_arr)({ struct _tagged_arr(* _temp397)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp401= _tag_arr(" -o %s", sizeof( unsigned char), 7u); struct Cyc_Stdio_String_pa_struct
_temp402; _temp402.tag= Cyc_Stdio_String_pa; _temp402.f1=( struct _tagged_arr)
Cyc_sh_escape_string(*(( struct _tagged_arr*) _check_null( Cyc_output_file)));{
void* _temp400=( void*)& _temp402; void* _temp398[ 1u]={ _temp400}; struct
_tagged_arr _temp399={( void*) _temp398,( void*) _temp398,( void*)( _temp398 + 1u)};
_temp397( _temp401, _temp399);}}));{ void* _temp389=( void*)& _temp403; struct
Cyc_Stdio_String_pa_struct _temp396; _temp396.tag= Cyc_Stdio_String_pa; _temp396.f1=(
struct _tagged_arr) _temp279;{ void* _temp390=( void*)& _temp396; struct Cyc_Stdio_String_pa_struct
_temp395; _temp395.tag= Cyc_Stdio_String_pa; _temp395.f1=( struct _tagged_arr)
stdlib_string;{ void* _temp391=( void*)& _temp395; struct Cyc_Stdio_String_pa_struct
_temp394; _temp394.tag= Cyc_Stdio_String_pa; _temp394.f1=( struct _tagged_arr)
_temp280;{ void* _temp392=( void*)& _temp394; void* _temp386[ 5u]={ _temp388,
_temp389, _temp390, _temp391, _temp392}; struct _tagged_arr _temp387={( void*)
_temp386,( void*) _temp386,( void*)( _temp386 + 5u)}; _temp385( _temp393,
_temp387);}}}}}}); if( Cyc_v_r){({ int(* _temp373)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp377= Cyc_Stdio_stderr; struct _tagged_arr _temp378= _tag_arr("%s\n",
sizeof( unsigned char), 4u); struct Cyc_Stdio_String_pa_struct _temp379;
_temp379.tag= Cyc_Stdio_String_pa; _temp379.f1=( struct _tagged_arr) _temp372;{
void* _temp376=( void*)& _temp379; void* _temp374[ 1u]={ _temp376}; struct
_tagged_arr _temp375={( void*) _temp374,( void*) _temp374,( void*)( _temp374 + 1u)};
_temp373( _temp377, _temp378, _temp375);}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp372)) != 0){({ int(* _temp380)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp383= Cyc_Stdio_stderr; struct _tagged_arr _temp384= _tag_arr("Error: C compiler failed\n",
sizeof( unsigned char), 26u); void* _temp381[ 0u]={}; struct _tagged_arr
_temp382={( void*) _temp381,( void*) _temp381,( void*)( _temp381 + 0u)};
_temp380( _temp383, _temp384, _temp382);}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1;} Cyc_remove_cfiles(); return Cyc_compile_failure? 1: 0;}}}}}}