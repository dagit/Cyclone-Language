 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; struct _tagged_ptr0{ void** curr; void** base;
void** last_plus_one; } ; extern struct Cyc_List_List* Cyc_List_list( struct
_tagged_ptr0); extern unsigned char Cyc_List_List_empty[ 15u]; extern
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
int tag; void(* f1)( struct _tagged_string); } ; typedef void* Cyc_Arg_gspec_t;
typedef void* Cyc_Arg_spec_t; struct _tagged_ptr1{ struct _tagged_string* curr;
struct _tagged_string* base; struct _tagged_string* last_plus_one; } ; extern
void Cyc_Arg_parse( struct Cyc_List_List* specs, void(* anonfun)( struct
_tagged_string), struct _tagged_string errmsg, struct _tagged_ptr1 args); struct
Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern
struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_remove( struct _tagged_string);
extern int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush(
struct Cyc_Stdio___sFILE*); extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_file_open( struct _tagged_string fname, struct _tagged_string mode);
extern void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE*); typedef int Cyc___int32_t;
typedef unsigned int Cyc___uint32_t; struct Cyc_Time_tm{ int tm_sec; int tm_min;
int tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int tm_yday; int
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
 __attribute__(( cdecl )) ; struct Cyc___sFILE; struct Cyc__reent; typedef
unsigned int Cyc___ULong; struct Cyc__glue{ struct Cyc__glue* _next; int _niobs;
struct Cyc___sFILE* _iobs; } ; struct Cyc__Bigint{ struct Cyc__Bigint* _next;
int _k; int _maxwds; int _sign; int _wds; unsigned int _x[ 1u]; } ; struct Cyc__atexit{
struct Cyc__atexit* _next; int _ind; void(* _fns[ 32u])(); } ; struct Cyc___sbuf{
unsigned char* _base; int _size; } ; typedef int Cyc__fpos_t; struct Cyc___sFILE{
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
struct Cyc__reent*); struct Cyc_Stdlib__Div{ int quot; int rem; } ; typedef
struct Cyc_Stdlib__Div Cyc_Stdlib_div_t; struct Cyc_Stdlib__Ldiv{ int quot; int
rem; } ; typedef struct Cyc_Stdlib__Ldiv Cyc_Stdlib_ldiv_t; extern int
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
typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct _tagged_string Cyc_Filename_concat( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string); struct Cyc_Id_id; typedef struct Cyc_Id_id* Cyc_Id_Id;
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern unsigned char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
struct _tagged_string); struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_error_p(); typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t; struct _tuple0{ void*
f1; struct _tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t;
typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t; typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t; typedef struct Cyc_Absyn_TunionFieldInfo
Cyc_Absyn_tunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void*
Cyc_Absyn_incrementor_t; typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t; typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t; typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t; typedef void* Cyc_Absyn_designator_t; typedef void* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const int
Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
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
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
int Cyc_Absyn_Fallthru_s= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl*
f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s= 12; struct
Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int
Cyc_Absyn_Splice_s= 13; struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Absyn_Label_s= 14; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Do_s= 15; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 17; struct Cyc_Absyn_Region_s_struct{ int tag; struct
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
body; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Unresolved_b=
0; static const int Cyc_Absyn_Global_b= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Funname_b= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
static const int Cyc_Absyn_Param_b= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Local_b= 3; struct
Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Pat_b= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
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
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr2{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr2 lex_base;
struct _tagged_ptr2 lex_backtrk; struct _tagged_ptr2 lex_default; struct
_tagged_ptr2 lex_trans; struct _tagged_ptr2 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_List_List* Cyc_Parse_parse_file( struct
Cyc_Stdio___sFILE* f); typedef void* Cyc_struct_or_union_t; typedef void* Cyc_blockitem_t;
typedef void* Cyc_type_specifier_t; typedef void* Cyc_storage_class_t; struct
Cyc_Declaration_spec; typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t;
struct Cyc_Declarator; typedef struct Cyc_Declarator* Cyc_declarator_t; struct
Cyc_Abstractdeclarator; typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t;
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
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[
25u]; struct Cyc_SwitchClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionFieldList_tok[ 24u]; struct Cyc_TunionFieldList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionField_tok[
20u]; struct Cyc_TunionField_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tunionfield*
f1; } ; extern unsigned char Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[
25u]; struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out;
typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_to_VC; extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern unsigned char Cyc_Set_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j=
0; static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; static const
int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{ int tag;
void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void
Cyc_Tcutil_flush_warnings(); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te,
int add_cyc_namespace, int var_default_init, struct Cyc_List_List* ds); extern
struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*);
extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List* ds); extern
struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* decls); struct Cyc_CfFlowInfo_Place;
typedef void* Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t;
typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
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
Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); struct Cyc_Interface_I; typedef struct Cyc_Interface_I* Cyc_Interface_i_t;
extern struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae);
extern void Cyc_Interface_print( struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*);
typedef struct Cyc_Interface_I* Cyc_interface_t; extern void Cyc_Lex_lex_init();
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
_temp5.curr +=( int)( _temp3 - 4); _temp5;}),({ unsigned char* _temp6=".cyc";
struct _tagged_string _temp7; _temp7.curr= _temp6; _temp7.base= _temp6; _temp7.last_plus_one=
_temp6 + 5; _temp7;})) == 0;}} static struct Cyc_List_List* Cyc_cyclone_exec_prefix_path=
0; static void Cyc_add_cyclone_exec_prefix_path( struct _tagged_string s){
unsigned int _temp8= Cyc_String_strlen( s); goto _LL9; _LL9: if( _temp8 <= 2){
return;}{ struct _tagged_string _temp10= Cyc_String_substring( s, 2, _temp8 - 2);
goto _LL11; _LL11: Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp12=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp12->hd=(
void*)({ struct _tagged_string* _temp13=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp13[ 0]= _temp10; _temp13;}); _temp12->tl=
Cyc_cyclone_exec_prefix_path; _temp12;});}} static struct Cyc_List_List* Cyc_ccargs=
0; static void Cyc_add_ccarg( struct _tagged_string s){ Cyc_ccargs=({ struct Cyc_List_List*
_temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp14->hd=( void*)({ struct _tagged_string* _temp15=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp15[ 0]= s; _temp15;}); _temp14->tl=
Cyc_ccargs; _temp14;});} static void Cyc_add_ccarg2( struct _tagged_string flag,
struct _tagged_string arg){ Cyc_ccargs=({ struct Cyc_List_List* _temp16=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp16->hd=( void*)({
struct _tagged_string* _temp19=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp19[ 0]= arg; _temp19;}); _temp16->tl=({ struct Cyc_List_List*
_temp17=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp17->hd=( void*)({ struct _tagged_string* _temp18=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp18[ 0]= flag; _temp18;});
_temp17->tl= Cyc_ccargs; _temp17;}); _temp16;});} static struct Cyc_List_List*
Cyc_libargs= 0; static void Cyc_add_libarg( struct _tagged_string s){ Cyc_libargs=({
struct Cyc_List_List* _temp20=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp20->hd=( void*)({ struct _tagged_string* _temp21=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp21[ 0]= s;
_temp21;}); _temp20->tl= Cyc_libargs; _temp20;});} static void Cyc_set_save_temps(){
Cyc_save_temps_r= 1; Cyc_add_ccarg(({ unsigned char* _temp22="-save-temps";
struct _tagged_string _temp23; _temp23.curr= _temp22; _temp23.base= _temp22;
_temp23.last_plus_one= _temp22 + 12; _temp23;}));} static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r= 1; Cyc_add_cpparg(({ unsigned char* _temp24="-M"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}));} static void Cyc_set_stop_after_objectfile(){ Cyc_stop_after_objectfile_r=
1; Cyc_add_ccarg(({ unsigned char* _temp26="-c"; struct _tagged_string _temp27;
_temp27.curr= _temp26; _temp27.base= _temp26; _temp27.last_plus_one= _temp26 + 3;
_temp27;}));} static struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other(
struct _tagged_string s){ if( Cyc_is_cyclone_sourcefile( s)){ Cyc_cyclone_files=({
struct Cyc_List_List* _temp28=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp28->hd=( void*)({ struct _tagged_string* _temp29=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp29[ 0]= s;
_temp29;}); _temp28->tl= Cyc_cyclone_files; _temp28;});{ struct _tagged_string
_temp30= Cyc_Filename_chop_extension( s); goto _LL31; _LL31: { struct
_tagged_string _temp34= Cyc_String_strconcat( _temp30,({ unsigned char* _temp32=".c";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 3; _temp33;})); goto _LL35; _LL35: Cyc_add_ccarg(
_temp34);}}} else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct
_tagged_string s){ if( Cyc_save_temps_r){ return;} else{ Cyc_Stdio_remove( s);}}
int Cyc_compile_failure= 0; struct Cyc_Stdio___sFILE* Cyc_try_file_open( struct
_tagged_string filename, struct _tagged_string mode, struct _tagged_string
msg_part){ struct _handler_cons _temp36; _push_handler(& _temp36);{ int _temp38=
0; if( setjmp( _temp36.handler)){ _temp38= 1;} if( ! _temp38){{ struct Cyc_Stdio___sFILE*
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
_tagged_string exn_string=({ unsigned char* _temp85=""; struct _tagged_string
_temp86; _temp86.curr= _temp85; _temp86.base= _temp85; _temp86.last_plus_one=
_temp85 + 1; _temp86;}); struct _tagged_string explain_string=({ unsigned char*
_temp83=""; struct _tagged_string _temp84; _temp84.curr= _temp83; _temp84.base=
_temp83; _temp84.last_plus_one= _temp83 + 1; _temp84;}); int other_exn= 0; void*
ex=( void*)({ struct Cyc_Core_Impossible_struct* _temp79=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp79[ 0]=({ struct
Cyc_Core_Impossible_struct _temp80; _temp80.tag= Cyc_Core_Impossible; _temp80.f1=({
unsigned char* _temp81=""; struct _tagged_string _temp82; _temp82.curr= _temp81;
_temp82.base= _temp81; _temp82.last_plus_one= _temp81 + 1; _temp82;}); _temp80;});
_temp79;}); struct Cyc_List_List* _temp49= 0; goto _LL50; _LL50:{ struct
_handler_cons _temp51; _push_handler(& _temp51);{ int _temp53= 0; if( setjmp(
_temp51.handler)){ _temp53= 1;} if( ! _temp53){ _temp49= f( env, tds);;
_pop_handler();} else{ void* _temp52=( void*) _exn_thrown; void* _temp55=
_temp52; struct _tagged_string _temp65; _LL57: if(*(( void**) _temp55) == Cyc_Core_Impossible){
_LL66: _temp65=(( struct Cyc_Core_Impossible_struct*) _temp55)->f1; goto _LL58;}
else{ goto _LL59;} _LL59: if( _temp55 == Cyc_Dict_Absent){ goto _LL60;} else{
goto _LL61;} _LL61: goto _LL62; _LL63: goto _LL64; _LL58: exn_string=({
unsigned char* _temp67="Exception Core::Impossible"; struct _tagged_string
_temp68; _temp68.curr= _temp67; _temp68.base= _temp67; _temp68.last_plus_one=
_temp67 + 27; _temp68;}); explain_string= _temp65; goto _LL56; _LL60: exn_string=({
unsigned char* _temp69="Exception Dict::Absent"; struct _tagged_string _temp70;
_temp70.curr= _temp69; _temp70.base= _temp69; _temp70.last_plus_one= _temp69 +
23; _temp70;}); goto _LL56; _LL62: ex= _temp55; other_exn= 1; exn_string=({
unsigned char* _temp71="Uncaught exception"; struct _tagged_string _temp72;
_temp72.curr= _temp71; _temp72.base= _temp71; _temp72.last_plus_one= _temp71 +
19; _temp72;}); goto _LL56; _LL64:( void) _throw( _temp55); _LL56:;}}} if( Cyc_Position_error_p()){
Cyc_compile_failure= 1;} if( Cyc_String_strcmp( exn_string,({ unsigned char*
_temp73=""; struct _tagged_string _temp74; _temp74.curr= _temp73; _temp74.base=
_temp73; _temp74.last_plus_one= _temp73 + 1; _temp74;})) != 0){ Cyc_compile_failure=
1;({ struct _tagged_string _temp75= exn_string; struct _tagged_string _temp76=
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
void Cyc_process_file( struct _tagged_string filename){ struct _tagged_string
basename= Cyc_Filename_chop_extension( filename); struct _tagged_string
preprocfile= Cyc_String_strconcat( basename,({ unsigned char* _temp188=".cyp";
struct _tagged_string _temp189; _temp189.curr= _temp188; _temp189.base= _temp188;
_temp189.last_plus_one= _temp188 + 5; _temp189;})); struct _tagged_string
interfacefile= Cyc_String_strconcat( basename,({ unsigned char* _temp186=".cyci";
struct _tagged_string _temp187; _temp187.curr= _temp186; _temp187.base= _temp186;
_temp187.last_plus_one= _temp186 + 6; _temp187;})); struct _tagged_string cfile=
Cyc_String_strconcat( basename,({ unsigned char* _temp184=".c"; struct
_tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 3; _temp185;})); if( Cyc_v_r){({ struct
_tagged_string _temp95= filename; fprintf( sfile_to_file( Cyc_Stdio_stderr),"Compiling %.*s\n",
_temp95.last_plus_one - _temp95.curr, _temp95.curr);});}{ struct Cyc_Stdio___sFILE*
f0= Cyc_try_file_open( filename,({ unsigned char* _temp180="r"; struct
_tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 2; _temp181;}),({ unsigned char* _temp182="input file";
struct _tagged_string _temp183; _temp183.curr= _temp182; _temp183.base= _temp182;
_temp183.last_plus_one= _temp182 + 11; _temp183;})); if( Cyc_compile_failure){
return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*) _check_null( f0));{
struct _tagged_string _temp102= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp96->hd=( void*)({ struct _tagged_string* _temp97=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp97[ 0]=({ unsigned char*
_temp98=""; struct _tagged_string _temp99; _temp99.curr= _temp98; _temp99.base=
_temp98; _temp99.last_plus_one= _temp98 + 1; _temp99;}); _temp97;}); _temp96->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs);
_temp96;}),({ unsigned char* _temp100=" "; struct _tagged_string _temp101;
_temp101.curr= _temp100; _temp101.base= _temp100; _temp101.last_plus_one=
_temp100 + 2; _temp101;})); goto _LL103; _LL103: { unsigned char* _temp104=" -Ic:/cyclone/include";
goto _LL105; _LL105: { struct _tagged_string ofile_string; if( Cyc_stop_after_cpp_r){
if( Cyc_output_file != 0){ ofile_string=({ struct _tagged_string _temp106=*((
struct _tagged_string*) _check_null( Cyc_output_file)); xprintf(" -o %.*s",
_temp106.last_plus_one - _temp106.curr, _temp106.curr);});} else{ ofile_string=({
unsigned char* _temp107=""; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 1;
_temp108;});}} else{ ofile_string=({ struct _tagged_string _temp109= preprocfile;
xprintf(" -o %.*s", _temp109.last_plus_one - _temp109.curr, _temp109.curr);});}{
struct _tagged_string _temp116=({ struct _tagged_string _temp112= _temp102;
struct _tagged_string _temp113=({ unsigned char* _temp110= _temp104; struct
_tagged_string _temp111; _temp111.curr= _temp110; _temp111.base= _temp110;
_temp111.last_plus_one= _temp110 + 22; _temp111;}); struct _tagged_string
_temp114= ofile_string; struct _tagged_string _temp115= filename; xprintf("gcc -x c -E -U__GNUC__%.*s%.*s%.*s %.*s",
_temp112.last_plus_one - _temp112.curr, _temp112.curr, _temp113.last_plus_one -
_temp113.curr, _temp113.curr, _temp114.last_plus_one - _temp114.curr, _temp114.curr,
_temp115.last_plus_one - _temp115.curr, _temp115.curr);}); goto _LL117; _LL117:
if( Cyc_v_r){({ struct _tagged_string _temp118= _temp116; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s\n", _temp118.last_plus_one - _temp118.curr, _temp118.curr);});}
if( system( string_to_Cstring( _temp116)) != 0){ Cyc_compile_failure= 1; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\nError: preprocessing\n"); return;} if( Cyc_stop_after_cpp_r){
return;} Cyc_Position_reset_position( preprocfile); Cyc_Absynpp_print_using_stmts=(
Cyc_Absynpp_print_externC_stmts= 1);{ struct Cyc_Stdio___sFILE* in_file= Cyc_try_file_open(
preprocfile,({ unsigned char* _temp176="r"; struct _tagged_string _temp177;
_temp177.curr= _temp176; _temp177.base= _temp176; _temp177.last_plus_one=
_temp176 + 2; _temp177;}),({ unsigned char* _temp178="file"; struct
_tagged_string _temp179; _temp179.curr= _temp178; _temp179.base= _temp178;
_temp179.last_plus_one= _temp178 + 5; _temp179;})); if( Cyc_compile_failure){
return;}{ struct Cyc_List_List* tds= 0;{ struct _handler_cons _temp119;
_push_handler(& _temp119);{ int _temp121= 0; if( setjmp( _temp119.handler)){
_temp121= 1;} if( ! _temp121){ tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp122="parsing"; struct _tagged_string
_temp123; _temp123.curr= _temp122; _temp123.base= _temp122; _temp123.last_plus_one=
_temp122 + 8; _temp123;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*) _check_null(
in_file), tds);; _pop_handler();} else{ void* _temp120=( void*) _exn_thrown;
void* _temp125= _temp120; _LL127: goto _LL128; _LL129: goto _LL130; _LL128: Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*) _check_null( in_file)); Cyc_Tcutil_flush_warnings();
Cyc_remove_file( preprocfile);( void) _throw( _temp125); _LL130:( void) _throw(
_temp125); _LL126:;}}} Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( in_file)); if( Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();
Cyc_remove_file( preprocfile); return;}{ struct Cyc_Tcenv_Tenv* _temp131= Cyc_Tcenv_tc_init();
goto _LL132; _LL132: if( Cyc_parseonly_r){ goto PRINTC;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds))
Cyc_do_stage)(({ unsigned char* _temp133="type checking"; struct _tagged_string
_temp134; _temp134.curr= _temp133; _temp134.base= _temp133; _temp134.last_plus_one=
_temp133 + 14; _temp134;}), Cyc_do_typecheck, _temp131, tds); if( Cyc_compile_failure){
Cyc_Tcutil_flush_warnings(); Cyc_remove_file( preprocfile); return;} tds=((
struct Cyc_List_List*(*)( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(({
unsigned char* _temp135="control-flow checking"; struct _tagged_string _temp136;
_temp136.curr= _temp135; _temp136.base= _temp135; _temp136.last_plus_one=
_temp135 + 22; _temp136;}), Cyc_do_cfcheck, 1, tds); Cyc_Tcutil_flush_warnings();
Cyc_remove_file( preprocfile); if( Cyc_compile_failure){ return;} Cyc_Absynpp_print_using_stmts=(
Cyc_Absynpp_print_externC_stmts= 0); if( Cyc_ic_r){ struct Cyc_Stdio___sFILE*
inter_file; inter_file= Cyc_try_file_open( interfacefile,({ unsigned char*
_temp137="w"; struct _tagged_string _temp138; _temp138.curr= _temp137; _temp138.base=
_temp137; _temp138.last_plus_one= _temp137 + 2; _temp138;}),({ unsigned char*
_temp139="interface file"; struct _tagged_string _temp140; _temp140.curr=
_temp139; _temp140.base= _temp139; _temp140.last_plus_one= _temp139 + 15;
_temp140;})); if( Cyc_compile_failure){ return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct _tuple9*,
struct Cyc_List_List*), struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)(({
unsigned char* _temp141="interface checking"; struct _tagged_string _temp142;
_temp142.curr= _temp141; _temp142.base= _temp141; _temp142.last_plus_one=
_temp141 + 19; _temp142;}), Cyc_do_interface,({ struct _tuple9* _temp143=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp143->f1= _temp131;
_temp143->f2=( struct Cyc_Stdio___sFILE*) _check_null( inter_file); _temp143;}),
tds); if( inter_file != 0){ Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( inter_file));} if( Cyc_compile_failure){ return;}} if( Cyc_tc_r){
goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds)) Cyc_do_stage)(({ unsigned char* _temp144="translation to C"; struct
_tagged_string _temp145; _temp145.curr= _temp144; _temp145.base= _temp144;
_temp145.last_plus_one= _temp144 + 17; _temp145;}), Cyc_do_translate, 1, tds);
if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp146="post-pass to VC";
struct _tagged_string _temp147; _temp147.curr= _temp146; _temp147.base= _temp146;
_temp147.last_plus_one= _temp146 + 16; _temp147;}), Cyc_do_tovc, 1, tds);} if(
Cyc_compile_failure){ return;} PRINTC: if( tds != 0){ struct Cyc_Stdio___sFILE*
out_file; if( Cyc_parseonly_r? 1: Cyc_tc_r){ if( Cyc_output_file != 0){ out_file=
Cyc_try_file_open(*(( struct _tagged_string*) _check_null( Cyc_output_file)),({
unsigned char* _temp148="w"; struct _tagged_string _temp149; _temp149.curr=
_temp148; _temp149.base= _temp148; _temp149.last_plus_one= _temp148 + 2;
_temp149;}),({ unsigned char* _temp150="output file"; struct _tagged_string
_temp151; _temp151.curr= _temp150; _temp151.base= _temp150; _temp151.last_plus_one=
_temp150 + 12; _temp151;}));} else{ out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;}}
else{ if( Cyc_toc_r? Cyc_output_file != 0: 0){ out_file= Cyc_try_file_open(*((
struct _tagged_string*) _check_null( Cyc_output_file)),({ unsigned char*
_temp152="w"; struct _tagged_string _temp153; _temp153.curr= _temp152; _temp153.base=
_temp152; _temp153.last_plus_one= _temp152 + 2; _temp153;}),({ unsigned char*
_temp154="output file"; struct _tagged_string _temp155; _temp155.curr= _temp154;
_temp155.base= _temp154; _temp155.last_plus_one= _temp154 + 12; _temp155;}));}
else{ out_file= Cyc_try_file_open( cfile,({ unsigned char* _temp156="w"; struct
_tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 2; _temp157;}),({ unsigned char* _temp158="output file";
struct _tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 12; _temp159;}));}} if( Cyc_compile_failure){
return;}{ struct _handler_cons _temp160; _push_handler(& _temp160);{ int
_temp162= 0; if( setjmp( _temp160.handler)){ _temp162= 1;} if( ! _temp162){ tds=((
struct Cyc_List_List*(*)( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*), struct Cyc_Stdio___sFILE*
env, struct Cyc_List_List* tds)) Cyc_do_stage)(({ unsigned char* _temp163="printing";
struct _tagged_string _temp164; _temp164.curr= _temp163; _temp164.base= _temp163;
_temp164.last_plus_one= _temp163 + 9; _temp164;}), Cyc_do_print,( struct Cyc_Stdio___sFILE*)
_check_null( out_file), tds);; _pop_handler();} else{ void* _temp161=( void*)
_exn_thrown; void* _temp166= _temp161; _LL168: goto _LL169; _LL170: goto _LL171;
_LL169: Cyc_compile_failure= 1; Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List* _temp172=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp172->hd=( void*)({
struct _tagged_string* _temp173=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp173[ 0]= cfile; _temp173;}); _temp172->tl= Cyc_cfiles;
_temp172;});( void) _throw( _temp166); _LL171:( void) _throw( _temp166); _LL167:;}}}
Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*) _check_null( out_file)); Cyc_cfiles=({
struct Cyc_List_List* _temp174=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp174->hd=( void*)({ struct _tagged_string* _temp175=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp175[ 0]=
cfile; _temp175;}); _temp174->tl= Cyc_cfiles; _temp174;});}}}}}}}}}} static
struct Cyc_List_List* Cyc_split_by_char( struct _tagged_string s, unsigned char
c){ if( s.curr ==({ struct _tagged_string _temp190={ 0, 0, 0}; _temp190;}).curr){
return 0;}{ struct Cyc_List_List* _temp191= 0; goto _LL192; _LL192: {
unsigned int _temp193= Cyc_String_strlen( s); goto _LL194; _LL194: while(
_temp193 > 0) { struct _tagged_string _temp195= Cyc_String_strchr( s, c); goto
_LL196; _LL196: if( _temp195.curr ==({ struct _tagged_string _temp197={ 0, 0, 0};
_temp197;}).curr){ _temp191=({ struct Cyc_List_List* _temp198=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp198->hd=( void*)({ struct
_tagged_string* _temp199=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp199[ 0]= s; _temp199;}); _temp198->tl= _temp191; _temp198;});
break;} else{ _temp191=({ struct Cyc_List_List* _temp200=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp200->hd=( void*)({ struct
_tagged_string* _temp201=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp201[ 0]= Cyc_String_substring( s, 0,( unsigned int)(
_temp195.curr - s.curr)); _temp201;}); _temp200->tl= _temp191; _temp200;});
_temp193 -= _temp195.curr - s.curr; s=({ struct _tagged_string _temp202=
_temp195; _temp202.curr += 1; _temp202;});}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp191);}}} int Cyc_file_exists(
struct _tagged_string file){ struct Cyc_Stdio___sFILE* f= 0;{ struct
_handler_cons _temp203; _push_handler(& _temp203);{ int _temp205= 0; if( setjmp(
_temp203.handler)){ _temp205= 1;} if( ! _temp205){ f=( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_file_open( file,({ unsigned char* _temp206="r"; struct _tagged_string
_temp207; _temp207.curr= _temp206; _temp207.base= _temp206; _temp207.last_plus_one=
_temp206 + 2; _temp207;}));; _pop_handler();} else{ void* _temp204=( void*)
_exn_thrown; void* _temp209= _temp204; _LL211: goto _LL212; _LL213: goto _LL214;
_LL212: goto _LL210; _LL214:( void) _throw( _temp209); _LL210:;}}} if( f == 0){
return 0;} else{ Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*) _check_null( f));
return 1;}} struct _tagged_string* Cyc_find( struct Cyc_List_List* dirs, struct
_tagged_string file){ if( file.curr ==({ struct _tagged_string _temp215={ 0, 0,
0}; _temp215;}).curr){ return 0;} for( 0; dirs != 0; dirs=(( struct Cyc_List_List*)
_check_null( dirs))->tl){ struct _tagged_string _temp216=*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( dirs))->hd); goto _LL217;
_LL217: if( _temp216.curr ==({ struct _tagged_string _temp218={ 0, 0, 0};
_temp218;}).curr? 1: Cyc_String_strlen( _temp216) == 0){ continue;}{ struct
_tagged_string _temp219= Cyc_Filename_concat( _temp216, file); goto _LL220;
_LL220: if( Cyc_file_exists( _temp219)){ return({ struct _tagged_string*
_temp221=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp221[ 0]= _temp219; _temp221;});}}} return 0;} struct _tuple10{ struct
_tagged_string f1; void* f2; struct _tagged_string f3; } ; struct _tagged_ptr3{
struct _tuple10** curr; struct _tuple10** base; struct _tuple10** last_plus_one;
} ; int Cyc_main( int argc, struct _tagged_ptr1 argv){ struct Cyc_List_List*
options=({ struct Cyc_List_List*(* _temp276)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple10* _temp279=({ struct
_tuple10* _temp520=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp520->f1=({ unsigned char* _temp525="-nocyc"; struct _tagged_string _temp526;
_temp526.curr= _temp525; _temp526.base= _temp525; _temp526.last_plus_one=
_temp525 + 7; _temp526;}); _temp520->f2=( void*)({ struct Cyc_Arg_Clear_spec_struct*
_temp523=( struct Cyc_Arg_Clear_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct));
_temp523[ 0]=({ struct Cyc_Arg_Clear_spec_struct _temp524; _temp524.tag= Cyc_Arg_Clear_spec;
_temp524.f1=& Cyc_add_cyc_namespace_r; _temp524;}); _temp523;}); _temp520->f3=({
unsigned char* _temp521="Do not add implicit namespace Cyc"; struct
_tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 34; _temp522;}); _temp520;}); struct _tuple10*
_temp280=({ struct _tuple10* _temp513=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp513->f1=({ unsigned char* _temp518="-v"; struct
_tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 3; _temp519;}); _temp513->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp516=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp516[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp517; _temp517.tag= Cyc_Arg_Set_spec; _temp517.f1=& Cyc_v_r; _temp517;});
_temp516;}); _temp513->f3=({ unsigned char* _temp514="Print compilation stages verbosely";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 35; _temp515;}); _temp513;}); struct _tuple10*
_temp281=({ struct _tuple10* _temp506=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp506->f1=({ unsigned char* _temp511="-toc"; struct
_tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 5; _temp512;}); _temp506->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp509=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp509[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp510; _temp510.tag= Cyc_Arg_Set_spec; _temp510.f1=& Cyc_toc_r; _temp510;});
_temp509;}); _temp506->f3=({ unsigned char* _temp507="Stop after translation to C";
struct _tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 28; _temp508;}); _temp506;}); struct _tuple10*
_temp282=({ struct _tuple10* _temp499=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp499->f1=({ unsigned char* _temp504="-parseonly"; struct
_tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 11; _temp505;}); _temp499->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp502=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp502[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp503; _temp503.tag= Cyc_Arg_Set_spec; _temp503.f1=& Cyc_parseonly_r;
_temp503;}); _temp502;}); _temp499->f3=({ unsigned char* _temp500="Stop after parsing";
struct _tagged_string _temp501; _temp501.curr= _temp500; _temp501.base= _temp500;
_temp501.last_plus_one= _temp500 + 19; _temp501;}); _temp499;}); struct _tuple10*
_temp283=({ struct _tuple10* _temp492=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp492->f1=({ unsigned char* _temp497="-tc"; struct
_tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 4; _temp498;}); _temp492->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp495=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp495[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp496; _temp496.tag= Cyc_Arg_Set_spec; _temp496.f1=& Cyc_tc_r; _temp496;});
_temp495;}); _temp492->f3=({ unsigned char* _temp493="Stop after type checking";
struct _tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 25; _temp494;}); _temp492;}); struct _tuple10*
_temp284=({ struct _tuple10* _temp485=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp485->f1=({ unsigned char* _temp490="-tovc"; struct
_tagged_string _temp491; _temp491.curr= _temp490; _temp491.base= _temp490;
_temp491.last_plus_one= _temp490 + 6; _temp491;}); _temp485->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp488=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp488[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp489; _temp489.tag= Cyc_Arg_Set_spec; _temp489.f1=& Cyc_tovc_r; _temp489;});
_temp488;}); _temp485->f3=({ unsigned char* _temp486="Avoid gcc extensions in C output";
struct _tagged_string _temp487; _temp487.curr= _temp486; _temp487.base= _temp486;
_temp487.last_plus_one= _temp486 + 33; _temp487;}); _temp485;}); struct _tuple10*
_temp285=({ struct _tuple10* _temp478=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp478->f1=({ unsigned char* _temp483="-ic"; struct
_tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 4; _temp484;}); _temp478->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp481=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp481[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp482; _temp482.tag= Cyc_Arg_Set_spec; _temp482.f1=& Cyc_ic_r; _temp482;});
_temp481;}); _temp478->f3=({ unsigned char* _temp479="Interface check"; struct
_tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 16; _temp480;}); _temp478;}); struct _tuple10*
_temp286=({ struct _tuple10* _temp471=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp471->f1=({ unsigned char* _temp476="-noremoveunused";
struct _tagged_string _temp477; _temp477.curr= _temp476; _temp477.base= _temp476;
_temp477.last_plus_one= _temp476 + 16; _temp477;}); _temp471->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp474=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp474[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp475; _temp475.tag= Cyc_Arg_Set_spec; _temp475.f1=& Cyc_noshake_r; _temp475;});
_temp474;}); _temp471->f3=({ unsigned char* _temp472="Don't remove externed variables that aren't used";
struct _tagged_string _temp473; _temp473.curr= _temp472; _temp473.base= _temp472;
_temp473.last_plus_one= _temp472 + 49; _temp473;}); _temp471;}); struct _tuple10*
_temp287=({ struct _tuple10* _temp464=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp464->f1=({ unsigned char* _temp469="-noexpandtypedefs";
struct _tagged_string _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 18; _temp470;}); _temp464->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp467=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp467[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp468; _temp468.tag= Cyc_Arg_Set_spec; _temp468.f1=& Cyc_noexpand_r; _temp468;});
_temp467;}); _temp464->f3=({ unsigned char* _temp465="Don't expand typedefs in pretty printing";
struct _tagged_string _temp466; _temp466.curr= _temp465; _temp466.base= _temp465;
_temp466.last_plus_one= _temp465 + 41; _temp466;}); _temp464;}); struct _tuple10*
_temp288=({ struct _tuple10* _temp457=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp457->f1=({ unsigned char* _temp462="-printalltvars";
struct _tagged_string _temp463; _temp463.curr= _temp462; _temp463.base= _temp462;
_temp463.last_plus_one= _temp462 + 15; _temp463;}); _temp457->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp460=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp460[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp461; _temp461.tag= Cyc_Arg_Set_spec; _temp461.f1=& Cyc_Absynpp_print_all_tvars;
_temp461;}); _temp460;}); _temp457->f3=({ unsigned char* _temp458="Print all type variables (even implicit default effects)";
struct _tagged_string _temp459; _temp459.curr= _temp458; _temp459.base= _temp458;
_temp459.last_plus_one= _temp458 + 57; _temp459;}); _temp457;}); struct _tuple10*
_temp289=({ struct _tuple10* _temp450=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp450->f1=({ unsigned char* _temp455="-printallkinds";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 15; _temp456;}); _temp450->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp453=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp453[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp454; _temp454.tag= Cyc_Arg_Set_spec; _temp454.f1=& Cyc_Absynpp_print_all_kinds;
_temp454;}); _temp453;}); _temp450->f3=({ unsigned char* _temp451="Always print kinds of type variables";
struct _tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 37; _temp452;}); _temp450;}); struct _tuple10*
_temp290=({ struct _tuple10* _temp443=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp443->f1=({ unsigned char* _temp448="-up"; struct
_tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 4; _temp449;}); _temp443->f2=( void*)({
struct Cyc_Arg_Clear_spec_struct* _temp446=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp446[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp447; _temp447.tag= Cyc_Arg_Clear_spec; _temp447.f1=&
Cyc_pp_r; _temp447;}); _temp446;}); _temp443->f3=({ unsigned char* _temp444="Ugly print";
struct _tagged_string _temp445; _temp445.curr= _temp444; _temp445.base= _temp444;
_temp445.last_plus_one= _temp444 + 11; _temp445;}); _temp443;}); struct _tuple10*
_temp291=({ struct _tuple10* _temp436=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp436->f1=({ unsigned char* _temp441="-pp"; struct
_tagged_string _temp442; _temp442.curr= _temp441; _temp442.base= _temp441;
_temp442.last_plus_one= _temp441 + 4; _temp442;}); _temp436->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp439=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp439[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp440; _temp440.tag= Cyc_Arg_Set_spec; _temp440.f1=& Cyc_pp_r; _temp440;});
_temp439;}); _temp436->f3=({ unsigned char* _temp437="Pretty print"; struct
_tagged_string _temp438; _temp438.curr= _temp437; _temp438.base= _temp437;
_temp438.last_plus_one= _temp437 + 13; _temp438;}); _temp436;}); struct _tuple10*
_temp292=({ struct _tuple10* _temp429=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp429->f1=({ unsigned char* _temp434="-save-temps"; struct
_tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 12; _temp435;}); _temp429->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp432=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp432[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp433; _temp433.tag= Cyc_Arg_Unit_spec; _temp433.f1= Cyc_set_save_temps;
_temp433;}); _temp432;}); _temp429->f3=({ unsigned char* _temp430="Do not delete temporary files";
struct _tagged_string _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 30; _temp431;}); _temp429;}); struct _tuple10*
_temp293=({ struct _tuple10* _temp422=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp422->f1=({ unsigned char* _temp427="-save-c"; struct
_tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 8; _temp428;}); _temp422->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp425=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp425[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp426; _temp426.tag= Cyc_Arg_Set_spec; _temp426.f1=& Cyc_save_c_r; _temp426;});
_temp425;}); _temp422->f3=({ unsigned char* _temp423="Do not delete temporary C files";
struct _tagged_string _temp424; _temp424.curr= _temp423; _temp424.base= _temp423;
_temp424.last_plus_one= _temp423 + 32; _temp424;}); _temp422;}); struct _tuple10*
_temp294=({ struct _tuple10* _temp415=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp415->f1=({ unsigned char* _temp420="-o"; struct
_tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 3; _temp421;}); _temp415->f2=( void*)({
struct Cyc_Arg_String_spec_struct* _temp418=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp418[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp419; _temp419.tag= Cyc_Arg_String_spec; _temp419.f1=
Cyc_set_output_file; _temp419;}); _temp418;}); _temp415->f3=({ unsigned char*
_temp416="Output file name"; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 17;
_temp417;}); _temp415;}); struct _tuple10* _temp295=({ struct _tuple10* _temp408=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp408->f1=({
unsigned char* _temp413="-D*"; struct _tagged_string _temp414; _temp414.curr=
_temp413; _temp414.base= _temp413; _temp414.last_plus_one= _temp413 + 4;
_temp414;}); _temp408->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct* _temp411=(
struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp411[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp412; _temp412.tag= Cyc_Arg_Flag_spec;
_temp412.f1= Cyc_add_cpparg; _temp412;}); _temp411;}); _temp408->f3=({
unsigned char* _temp409="Pass definition to preprocessor"; struct _tagged_string
_temp410; _temp410.curr= _temp409; _temp410.base= _temp409; _temp410.last_plus_one=
_temp409 + 32; _temp410;}); _temp408;}); struct _tuple10* _temp296=({ struct
_tuple10* _temp401=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp401->f1=({ unsigned char* _temp406="-B*"; struct _tagged_string _temp407;
_temp407.curr= _temp406; _temp407.base= _temp406; _temp407.last_plus_one=
_temp406 + 4; _temp407;}); _temp401->f2=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp404=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp404[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp405; _temp405.tag= Cyc_Arg_Flag_spec;
_temp405.f1= Cyc_add_cyclone_exec_prefix_path; _temp405;}); _temp404;});
_temp401->f3=({ unsigned char* _temp402="Add to the list of directories to search for compiler files";
struct _tagged_string _temp403; _temp403.curr= _temp402; _temp403.base= _temp402;
_temp403.last_plus_one= _temp402 + 60; _temp403;}); _temp401;}); struct _tuple10*
_temp297=({ struct _tuple10* _temp394=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp394->f1=({ unsigned char* _temp399="-I*"; struct
_tagged_string _temp400; _temp400.curr= _temp399; _temp400.base= _temp399;
_temp400.last_plus_one= _temp399 + 4; _temp400;}); _temp394->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp397=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp397[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp398; _temp398.tag= Cyc_Arg_Flag_spec; _temp398.f1= Cyc_add_cpparg; _temp398;});
_temp397;}); _temp394->f3=({ unsigned char* _temp395="Add to the list of directories to search for include files";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 59; _temp396;}); _temp394;}); struct _tuple10*
_temp298=({ struct _tuple10* _temp387=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp387->f1=({ unsigned char* _temp392="-L*"; struct
_tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 4; _temp393;}); _temp387->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp390=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp390[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp391; _temp391.tag= Cyc_Arg_Flag_spec; _temp391.f1= Cyc_add_cpparg; _temp391;});
_temp390;}); _temp387->f3=({ unsigned char* _temp388="Add to the list of directories for -l";
struct _tagged_string _temp389; _temp389.curr= _temp388; _temp389.base= _temp388;
_temp389.last_plus_one= _temp388 + 38; _temp389;}); _temp387;}); struct _tuple10*
_temp299=({ struct _tuple10* _temp380=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp380->f1=({ unsigned char* _temp385="-l*"; struct
_tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 4; _temp386;}); _temp380->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp383=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp383[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp384; _temp384.tag= Cyc_Arg_Flag_spec; _temp384.f1= Cyc_add_libarg; _temp384;});
_temp383;}); _temp380->f3=({ unsigned char* _temp381="Library file"; struct
_tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 13; _temp382;}); _temp380;}); struct _tuple10*
_temp300=({ struct _tuple10* _temp373=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp373->f1=({ unsigned char* _temp378="-c"; struct
_tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 3; _temp379;}); _temp373->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp376=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp376[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp377; _temp377.tag= Cyc_Arg_Unit_spec; _temp377.f1= Cyc_set_stop_after_objectfile;
_temp377;}); _temp376;}); _temp373->f3=({ unsigned char* _temp374="Produce object file";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 20; _temp375;}); _temp373;}); struct _tuple10*
_temp301=({ struct _tuple10* _temp366=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp366->f1=({ unsigned char* _temp371="-O"; struct
_tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 3; _temp372;}); _temp366->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp369=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp369[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp370; _temp370.tag= Cyc_Arg_Flag_spec; _temp370.f1= Cyc_add_ccarg; _temp370;});
_temp369;}); _temp366->f3=({ unsigned char* _temp367="Optimize"; struct
_tagged_string _temp368; _temp368.curr= _temp367; _temp368.base= _temp367;
_temp368.last_plus_one= _temp367 + 9; _temp368;}); _temp366;}); struct _tuple10*
_temp302=({ struct _tuple10* _temp359=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp359->f1=({ unsigned char* _temp364="-O2"; struct
_tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 4; _temp365;}); _temp359->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp362=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp362[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp363; _temp363.tag= Cyc_Arg_Flag_spec; _temp363.f1= Cyc_add_ccarg; _temp363;});
_temp362;}); _temp359->f3=({ unsigned char* _temp360="Optimize"; struct
_tagged_string _temp361; _temp361.curr= _temp360; _temp361.base= _temp360;
_temp361.last_plus_one= _temp360 + 9; _temp361;}); _temp359;}); struct _tuple10*
_temp303=({ struct _tuple10* _temp352=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp352->f1=({ unsigned char* _temp357="-O3"; struct
_tagged_string _temp358; _temp358.curr= _temp357; _temp358.base= _temp357;
_temp358.last_plus_one= _temp357 + 4; _temp358;}); _temp352->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp355=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp355[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp356; _temp356.tag= Cyc_Arg_Flag_spec; _temp356.f1= Cyc_add_ccarg; _temp356;});
_temp355;}); _temp352->f3=({ unsigned char* _temp353="Optimize"; struct
_tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 9; _temp354;}); _temp352;}); struct _tuple10*
_temp304=({ struct _tuple10* _temp345=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp345->f1=({ unsigned char* _temp350="-g"; struct
_tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 3; _temp351;}); _temp345->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp348=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp348[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp349; _temp349.tag= Cyc_Arg_Flag_spec; _temp349.f1= Cyc_add_ccarg; _temp349;});
_temp348;}); _temp345->f3=({ unsigned char* _temp346="Compile for debugging";
struct _tagged_string _temp347; _temp347.curr= _temp346; _temp347.base= _temp346;
_temp347.last_plus_one= _temp346 + 22; _temp347;}); _temp345;}); struct _tuple10*
_temp305=({ struct _tuple10* _temp338=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp338->f1=({ unsigned char* _temp343="-p"; struct
_tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 3; _temp344;}); _temp338->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp341=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp341[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp342; _temp342.tag= Cyc_Arg_Flag_spec; _temp342.f1= Cyc_add_ccarg; _temp342;});
_temp341;}); _temp338->f3=({ unsigned char* _temp339="Compile for prof"; struct
_tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 17; _temp340;}); _temp338;}); struct _tuple10*
_temp306=({ struct _tuple10* _temp331=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp331->f1=({ unsigned char* _temp336="-pg"; struct
_tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 4; _temp337;}); _temp331->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp334=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp334[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp335; _temp335.tag= Cyc_Arg_Flag_spec; _temp335.f1= Cyc_add_ccarg; _temp335;});
_temp334;}); _temp331->f3=({ unsigned char* _temp332="Compile for gprof"; struct
_tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 18; _temp333;}); _temp331;}); struct _tuple10*
_temp307=({ struct _tuple10* _temp324=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp324->f1=({ unsigned char* _temp329="-S"; struct
_tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 3; _temp330;}); _temp324->f2=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp327=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp327[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp328; _temp328.tag= Cyc_Arg_Flag_spec; _temp328.f1= Cyc_add_ccarg; _temp328;});
_temp327;}); _temp324->f3=({ unsigned char* _temp325="Stop after producing assembly code";
struct _tagged_string _temp326; _temp326.curr= _temp325; _temp326.base= _temp325;
_temp326.last_plus_one= _temp325 + 35; _temp326;}); _temp324;}); struct _tuple10*
_temp308=({ struct _tuple10* _temp317=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp317->f1=({ unsigned char* _temp322="-M"; struct
_tagged_string _temp323; _temp323.curr= _temp322; _temp323.base= _temp322;
_temp323.last_plus_one= _temp322 + 3; _temp323;}); _temp317->f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp320=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp320[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp321; _temp321.tag= Cyc_Arg_Unit_spec; _temp321.f1= Cyc_set_produce_dependencies;
_temp321;}); _temp320;}); _temp317->f3=({ unsigned char* _temp318="Produce dependencies";
struct _tagged_string _temp319; _temp319.curr= _temp318; _temp319.base= _temp318;
_temp319.last_plus_one= _temp318 + 21; _temp319;}); _temp317;}); struct _tuple10*
_temp309=({ struct _tuple10* _temp310=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp310->f1=({ unsigned char* _temp315="-E"; struct
_tagged_string _temp316; _temp316.curr= _temp315; _temp316.base= _temp315;
_temp316.last_plus_one= _temp315 + 3; _temp316;}); _temp310->f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp313=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp313[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp314; _temp314.tag= Cyc_Arg_Set_spec; _temp314.f1=& Cyc_stop_after_cpp_r;
_temp314;}); _temp313;}); _temp310->f3=({ unsigned char* _temp311="Stop after preprocessing";
struct _tagged_string _temp312; _temp312.curr= _temp311; _temp312.base= _temp311;
_temp312.last_plus_one= _temp311 + 25; _temp312;}); _temp310;}); struct _tuple10*
_temp277[ 31u]={ _temp279, _temp280, _temp281, _temp282, _temp283, _temp284,
_temp285, _temp286, _temp287, _temp288, _temp289, _temp290, _temp291, _temp292,
_temp293, _temp294, _temp295, _temp296, _temp297, _temp298, _temp299, _temp300,
_temp301, _temp302, _temp303, _temp304, _temp305, _temp306, _temp307, _temp308,
_temp309}; struct _tagged_ptr3 _temp278={ _temp277, _temp277, _temp277 + 31u};
_temp276( _temp278);}); Cyc_Arg_parse( options, Cyc_add_other,({ unsigned char*
_temp222="Options:"; struct _tagged_string _temp223; _temp223.curr= _temp222;
_temp223.base= _temp222; _temp223.last_plus_one= _temp222 + 9; _temp223;}), argv);{
struct Cyc_List_List* _temp224=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_cyclone_files); goto _LL225; _LL225: for( 0; _temp224 !=
0; _temp224=(( struct Cyc_List_List*) _check_null( _temp224))->tl){ Cyc_process_file(*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( _temp224))->hd));
if( Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)?
1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;}{ struct _tagged_string _temp228= Cyc_Stdlib_getenv(({
unsigned char* _temp226="CYCLONE_EXEC_PREFIX"; struct _tagged_string _temp227;
_temp227.curr= _temp226; _temp227.base= _temp226; _temp227.last_plus_one=
_temp226 + 20; _temp227;})); goto _LL229; _LL229: if( _temp228.curr !=({ struct
_tagged_string _temp230={ 0, 0, 0}; _temp230;}).curr){ Cyc_cyclone_exec_prefix_path=({
struct Cyc_List_List* _temp231=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp231->hd=( void*)({ struct _tagged_string* _temp232=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp232[ 0]=
_temp228; _temp232;}); _temp231->tl= Cyc_cyclone_exec_prefix_path; _temp231;});}
Cyc_cyclone_exec_prefix_path=({ struct Cyc_List_List* _temp233=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp233->hd=( void*)({ struct
_tagged_string* _temp234=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp234[ 0]=({ unsigned char* _temp235="c:/cyclone/bin/cyc-lib";
struct _tagged_string _temp236; _temp236.curr= _temp235; _temp236.base= _temp235;
_temp236.last_plus_one= _temp235 + 23; _temp236;}); _temp234;}); _temp233->tl=
Cyc_cyclone_exec_prefix_path; _temp233;}); Cyc_cyclone_exec_prefix_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_exec_prefix_path);{
struct _tagged_string* _temp239= Cyc_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp237="include/cyc_include.h"; struct _tagged_string _temp238;
_temp238.curr= _temp237; _temp238.base= _temp237; _temp238.last_plus_one=
_temp237 + 22; _temp238;})); goto _LL240; _LL240: if( _temp239 == 0){ fprintf(
sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file cyc_include.h");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;}{ struct _tagged_string
_temp243= Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_ccargs),({ unsigned char* _temp241=" "; struct
_tagged_string _temp242; _temp242.curr= _temp241; _temp242.base= _temp241;
_temp242.last_plus_one= _temp241 + 2; _temp242;})); goto _LL244; _LL244: {
struct _tagged_string _temp251= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp245=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp245->hd=( void*)({ struct _tagged_string* _temp246=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp246[ 0]=({ unsigned char*
_temp247=""; struct _tagged_string _temp248; _temp248.curr= _temp247; _temp248.base=
_temp247; _temp248.last_plus_one= _temp247 + 1; _temp248;}); _temp246;});
_temp245->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
Cyc_libargs); _temp245;}),({ unsigned char* _temp249=" "; struct _tagged_string
_temp250; _temp250.curr= _temp249; _temp250.base= _temp249; _temp250.last_plus_one=
_temp249 + 2; _temp250;})); goto _LL252; _LL252: { struct _tagged_string
stdlib_string; if( Cyc_stop_after_objectfile_r){ stdlib_string=({ unsigned char*
_temp253=""; struct _tagged_string _temp254; _temp254.curr= _temp253; _temp254.base=
_temp253; _temp254.last_plus_one= _temp253 + 1; _temp254;});} else{ struct
_tagged_string* _temp257= Cyc_find( Cyc_cyclone_exec_prefix_path,({
unsigned char* _temp255="cyclib.a"; struct _tagged_string _temp256; _temp256.curr=
_temp255; _temp256.base= _temp255; _temp256.last_plus_one= _temp255 + 9;
_temp256;})); goto _LL258; _LL258: { struct _tagged_string* _temp261= Cyc_find(
Cyc_cyclone_exec_prefix_path,({ unsigned char* _temp259="gc.a"; struct
_tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 5; _temp260;})); goto _LL262; _LL262: if(
_temp257 == 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file cyclib.a");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} if( _temp261 == 0){
fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: can't find internal compiler file gc.a");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} stdlib_string=({ struct
_tagged_string _temp263=*(( struct _tagged_string*) _check_null( _temp257));
struct _tagged_string _temp264=*(( struct _tagged_string*) _check_null( _temp261));
xprintf(" %.*s %.*s", _temp263.last_plus_one - _temp263.curr, _temp263.curr,
_temp264.last_plus_one - _temp264.curr, _temp264.curr);});}}{ struct
_tagged_string _temp273=({ struct _tagged_string _temp268=*(( struct
_tagged_string*) _check_null( _temp239)); struct _tagged_string _temp269= Cyc_output_file
== 0?({ unsigned char* _temp265=""; struct _tagged_string _temp266; _temp266.curr=
_temp265; _temp266.base= _temp265; _temp266.last_plus_one= _temp265 + 1;
_temp266;}):({ struct _tagged_string _temp267=*(( struct _tagged_string*)
_check_null( Cyc_output_file)); xprintf(" -o %.*s", _temp267.last_plus_one -
_temp267.curr, _temp267.curr);}); struct _tagged_string _temp270= _temp243;
struct _tagged_string _temp271= stdlib_string; struct _tagged_string _temp272=
_temp251; xprintf("gcc -fwritable-strings -include %.*s%.*s %.*s%.*s%.*s",
_temp268.last_plus_one - _temp268.curr, _temp268.curr, _temp269.last_plus_one -
_temp269.curr, _temp269.curr, _temp270.last_plus_one - _temp270.curr, _temp270.curr,
_temp271.last_plus_one - _temp271.curr, _temp271.curr, _temp272.last_plus_one -
_temp272.curr, _temp272.curr);}); goto _LL274; _LL274: if( Cyc_v_r){({ struct
_tagged_string _temp275= _temp273; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s\n",
_temp275.last_plus_one - _temp275.curr, _temp275.curr);});} if( system(
string_to_Cstring( _temp273)) != 0){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: C compiler failed\n");
Cyc_compile_failure= 1; Cyc_remove_cfiles(); return 1;} Cyc_remove_cfiles();
return Cyc_compile_failure? 1: 0;}}}}}}}