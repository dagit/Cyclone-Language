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
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_xprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter( void(* f)(
void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(* f)( void*,
void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_split( struct
Cyc_List_List* x); extern int Cyc_String_strlen( struct _tagged_arr s); extern
int Cyc_String_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern int
Cyc_String_zstrcmp( struct _tagged_arr, struct _tagged_arr); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_String_strcpy(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_String_substring(
struct _tagged_arr, int ofs, int n); struct Cyc_Set_Set; extern unsigned char
Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct
Cyc_Position_Segment*, struct Cyc_Position_Segment*); extern struct _tagged_arr
Cyc_Position_string_of_segment( struct Cyc_Position_Segment*); static const int
Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const int
Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*,
struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
void Cyc_Position_post_error( struct Cyc_Position_Error*); struct _tuple1{ void*
f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
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
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern void* Cyc_Absyn_rel_ns_null; extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void*
Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern
void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern struct _tuple1* Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one; extern void* Cyc_Absyn_starb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_primop_exp( void*, struct Cyc_List_List* es, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp(
void*, struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct
Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment*
s); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
void* s, struct _tuple1* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs,
int is_xtunion, struct Cyc_Position_Segment* loc); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List* rgn_po, struct Cyc_List_List*); extern struct _tagged_arr
Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att( void* a);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_copy_type(
void* t); extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern void
Cyc_Lex_enter_namespace( struct _tagged_arr*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
static const int Cyc_Parse_Struct_su= 0; static const int Cyc_Parse_Union_su= 1;
static const int Cyc_Parse_TopDecls_bl= 0; struct Cyc_Parse_TopDecls_bl_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Parse_Stmt_bl= 1;
struct Cyc_Parse_Stmt_bl_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Parse_FnDecl_bl= 2; struct Cyc_Parse_FnDecl_bl_struct{ int tag;
struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Parse_Signed_spec= 0;
struct Cyc_Parse_Signed_spec_struct{ int tag; struct Cyc_Position_Segment* f1; }
; static const int Cyc_Parse_Unsigned_spec= 1; struct Cyc_Parse_Unsigned_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Short_spec=
2; struct Cyc_Parse_Short_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Long_spec= 3; struct Cyc_Parse_Long_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Type_spec=
4; struct Cyc_Parse_Type_spec_struct{ int tag; void* f1; struct Cyc_Position_Segment*
f2; } ; static const int Cyc_Parse_Decl_spec= 5; struct Cyc_Parse_Decl_spec_struct{
int tag; struct Cyc_Absyn_Decl* f1; } ; static const int Cyc_Parse_Typedef_sc= 0;
static const int Cyc_Parse_Extern_sc= 1; static const int Cyc_Parse_ExternC_sc=
2; static const int Cyc_Parse_Static_sc= 3; static const int Cyc_Parse_Auto_sc=
4; static const int Cyc_Parse_Register_sc= 5; static const int Cyc_Parse_Abstract_sc=
6; struct Cyc_Parse_Declaration_spec{ struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual
tq; struct Cyc_List_List* type_specs; int is_inline; struct Cyc_List_List*
attributes; } ; struct Cyc_Parse_Declarator{ struct _tuple1* id; struct Cyc_List_List*
tms; } ; struct Cyc_Parse_Abstractdeclarator{ struct Cyc_List_List* tms; } ;
struct _tuple4{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment*
loc, struct _tuple2* t); struct _tuple5{ void* f1; struct Cyc_Core_Opt* f2; } ;
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc); struct _tuple6{ struct Cyc_Absyn_Tqual f1;
void* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static struct
_tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual, void*, struct Cyc_List_List*,
struct Cyc_List_List*); struct _tuple7{ struct _tuple1* f1; struct Cyc_Absyn_Tqual
f2; void* f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; static
struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t); struct Cyc_Core_Opt* Cyc_Parse_lbuf= 0; static struct
Cyc_List_List* Cyc_Parse_parse_result= 0; struct Cyc_Absyn_Aligned_att_struct
Cyc_Parse_att_aligned={ 1u, - 1}; struct _tuple8{ struct _tagged_arr f1; void*
f2; } ; static unsigned char _temp1[ 8u]="stdcall"; static unsigned char _temp2[
6u]="cdecl"; static unsigned char _temp3[ 9u]="fastcall"; static unsigned char
_temp4[ 9u]="noreturn"; static unsigned char _temp5[ 6u]="const"; static
unsigned char _temp6[ 8u]="aligned"; static unsigned char _temp7[ 7u]="packed";
static unsigned char _temp8[ 7u]="shared"; static unsigned char _temp9[ 7u]="unused";
static unsigned char _temp10[ 5u]="weak"; static unsigned char _temp11[ 10u]="dllimport";
static unsigned char _temp12[ 10u]="dllexport"; static unsigned char _temp13[ 23u]="no_instrument_function";
static unsigned char _temp14[ 12u]="constructor"; static unsigned char _temp15[
11u]="destructor"; static unsigned char _temp16[ 22u]="no_check_memory_usage";
struct _tuple8 Cyc_Parse_att_map[ 16u]={{{ _temp1, _temp1, _temp1 + 8u},( void*)
0u},{{ _temp2, _temp2, _temp2 + 6u},( void*) 1u},{{ _temp3, _temp3, _temp3 + 9u},(
void*) 2u},{{ _temp4, _temp4, _temp4 + 9u},( void*) 3u},{{ _temp5, _temp5,
_temp5 + 6u},( void*) 4u},{{ _temp6, _temp6, _temp6 + 8u},( void*)& Cyc_Parse_att_aligned},{{
_temp7, _temp7, _temp7 + 7u},( void*) 5u},{{ _temp8, _temp8, _temp8 + 7u},( void*)
7u},{{ _temp9, _temp9, _temp9 + 7u},( void*) 8u},{{ _temp10, _temp10, _temp10 +
5u},( void*) 9u},{{ _temp11, _temp11, _temp11 + 10u},( void*) 10u},{{ _temp12,
_temp12, _temp12 + 10u},( void*) 11u},{{ _temp13, _temp13, _temp13 + 23u},( void*)
12u},{{ _temp14, _temp14, _temp14 + 12u},( void*) 13u},{{ _temp15, _temp15,
_temp15 + 11u},( void*) 14u},{{ _temp16, _temp16, _temp16 + 22u},( void*) 15u}};
static void Cyc_Parse_err( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse( sg, msg));} static void*
Cyc_Parse_abort( struct _tagged_arr msg, struct Cyc_Position_Segment* sg){ Cyc_Parse_err(
msg, sg);( int) _throw(( void*) Cyc_Position_Exit);} static void Cyc_Parse_warn(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ int(* _temp17)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp22= Cyc_Stdio_stderr; struct _tagged_arr _temp23=
_tag_arr("%s: Warning: %s\n", sizeof( unsigned char), 17u); struct Cyc_Stdio_String_pa_struct
_temp25; _temp25.tag= Cyc_Stdio_String_pa; _temp25.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
sg);{ void* _temp20=( void*)& _temp25; struct Cyc_Stdio_String_pa_struct _temp24;
_temp24.tag= Cyc_Stdio_String_pa; _temp24.f1=( struct _tagged_arr) msg;{ void*
_temp21=( void*)& _temp24; void* _temp18[ 2u]={ _temp20, _temp21}; struct
_tagged_arr _temp19={( void*) _temp18,( void*) _temp18,( void*)( _temp18 + 2u)};
_temp17( _temp22, _temp23, _temp19);}}}); return;} static void* Cyc_Parse_unimp(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){ return Cyc_Parse_abort((
struct _tagged_arr)({ struct _tagged_arr(* _temp26)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp30= _tag_arr("%s unimplemented",
sizeof( unsigned char), 17u); struct Cyc_Stdio_String_pa_struct _temp31; _temp31.tag=
Cyc_Stdio_String_pa; _temp31.f1=( struct _tagged_arr) msg;{ void* _temp29=( void*)&
_temp31; void* _temp27[ 1u]={ _temp29}; struct _tagged_arr _temp28={( void*)
_temp27,( void*) _temp27,( void*)( _temp27 + 1u)}; _temp26( _temp30, _temp28);}}),
sg);} static void Cyc_Parse_unimp2( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){({ int(* _temp32)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp37= Cyc_Stdio_stderr;
struct _tagged_arr _temp38= _tag_arr("%s: Warning: Cyclone does not yet support %s\n",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp40; _temp40.tag=
Cyc_Stdio_String_pa; _temp40.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
sg);{ void* _temp35=( void*)& _temp40; struct Cyc_Stdio_String_pa_struct _temp39;
_temp39.tag= Cyc_Stdio_String_pa; _temp39.f1=( struct _tagged_arr) msg;{ void*
_temp36=( void*)& _temp39; void* _temp33[ 2u]={ _temp35, _temp36}; struct
_tagged_arr _temp34={( void*) _temp33,( void*) _temp33,( void*)( _temp33 + 2u)};
_temp32( _temp37, _temp38, _temp34);}}}); return;} struct _tuple9{ struct
_tuple7* f1; struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield*
Cyc_Parse_make_struct_field( struct Cyc_Position_Segment* loc, struct _tuple9*
field_info){ struct Cyc_Core_Opt* _temp43; struct _tuple7* _temp45; struct
_tuple9 _temp41=* field_info; _LL46: _temp45= _temp41.f1; goto _LL44; _LL44:
_temp43= _temp41.f2; goto _LL42; _LL42: if((* _temp45).f4 != 0){ Cyc_Parse_err(
_tag_arr("bad type params in struct field", sizeof( unsigned char), 32u), loc);}{
struct _tuple1* _temp47=(* _temp45).f1;{ void* _temp48=(* _temp47).f1; struct
Cyc_List_List* _temp58; struct Cyc_List_List* _temp60; _LL50: if(( unsigned int)
_temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Rel_n: 0){ _LL59: _temp58=(( struct
Cyc_Absyn_Rel_n_struct*) _temp48)->f1; if( _temp58 == 0){ goto _LL51;} else{
goto _LL52;}} else{ goto _LL52;} _LL52: if(( unsigned int) _temp48 > 1u?*(( int*)
_temp48) == Cyc_Absyn_Abs_n: 0){ _LL61: _temp60=(( struct Cyc_Absyn_Abs_n_struct*)
_temp48)->f1; if( _temp60 == 0){ goto _LL53;} else{ goto _LL54;}} else{ goto
_LL54;} _LL54: if( _temp48 ==( void*) Cyc_Absyn_Loc_n){ goto _LL55;} else{ goto
_LL56;} _LL56: goto _LL57; _LL51: goto _LL49; _LL53: goto _LL49; _LL55: goto
_LL49; _LL57: Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a module name",
sizeof( unsigned char), 52u), loc); goto _LL49; _LL49:;}{ struct Cyc_List_List*
_temp62=(* _temp45).f5; return({ struct Cyc_Absyn_Structfield* _temp63=( struct
Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp63->name=(* _temp47).f2; _temp63->tq=(* _temp45).f2; _temp63->type=( void*)(*
_temp45).f3; _temp63->width= _temp43; _temp63->attributes= _temp62; _temp63;});}}}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp66; struct Cyc_List_List* _temp67; void* _temp69; struct Cyc_Absyn_Tqual
_temp71; struct Cyc_Core_Opt* _temp73; struct _tuple10* _temp64= field; _temp66=*
_temp64; _LL74: _temp73= _temp66.f1; goto _LL72; _LL72: _temp71= _temp66.f2;
goto _LL70; _LL70: _temp69= _temp66.f3; goto _LL68; _LL68: _temp67= _temp66.f4;
goto _LL65; _LL65: { struct Cyc_Core_Opt* _temp75= 0; if( _temp73 != 0){ _temp75=({
struct Cyc_Core_Opt* _temp76=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp76->v=( void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp73))->v)).f2; _temp76;});{ void* _temp77=(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp73))->v)).f1; struct Cyc_List_List*
_temp87; struct Cyc_List_List* _temp89; _LL79: if(( unsigned int) _temp77 > 1u?*((
int*) _temp77) == Cyc_Absyn_Rel_n: 0){ _LL88: _temp87=(( struct Cyc_Absyn_Rel_n_struct*)
_temp77)->f1; if( _temp87 == 0){ goto _LL80;} else{ goto _LL81;}} else{ goto
_LL81;} _LL81: if(( unsigned int) _temp77 > 1u?*(( int*) _temp77) == Cyc_Absyn_Abs_n:
0){ _LL90: _temp89=(( struct Cyc_Absyn_Abs_n_struct*) _temp77)->f1; if( _temp89
== 0){ goto _LL82;} else{ goto _LL83;}} else{ goto _LL83;} _LL83: if( _temp77 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL84;} else{ goto _LL85;} _LL85: goto _LL86;
_LL80: goto _LL78; _LL82: goto _LL78; _LL84: goto _LL78; _LL86: Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a module name", sizeof(
unsigned char), 49u), loc); goto _LL78; _LL78:;}} if( _temp67 != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters", sizeof( unsigned char), 41u),
loc);} return({ struct _tuple2* _temp91=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp91->f1= _temp75; _temp91->f2= _temp71; _temp91->f3=
_temp69; _temp91;});}} static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment*
loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp92=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp92[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp93; _temp93.tag= Cyc_Parse_Type_spec; _temp93.f1=(
void*) t; _temp93.f2= loc; _temp93;}); _temp92;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp94= t; struct Cyc_Absyn_Exp* _temp100; struct Cyc_Absyn_Tqual
_temp102; void* _temp104; _LL96: if(( unsigned int) _temp94 > 4u?*(( int*)
_temp94) == Cyc_Absyn_ArrayType: 0){ _LL105: _temp104=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp94)->f1; goto _LL103; _LL103: _temp102=(( struct Cyc_Absyn_ArrayType_struct*)
_temp94)->f2; goto _LL101; _LL101: _temp100=(( struct Cyc_Absyn_ArrayType_struct*)
_temp94)->f3; goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL97: return
Cyc_Absyn_starb_typ( _temp104,( void*) Cyc_Absyn_HeapRgn, _temp102, _temp100 ==
0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp106=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp106[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp107; _temp107.tag= Cyc_Absyn_Upper_b;
_temp107.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp100); _temp107;});
_temp106;})); _LL99: return t; _LL95:;} static void Cyc_Parse_arg_array2ptr(
struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple11*
_temp108=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp108->f1=(*
t).f2; _temp108->f2=(* t).f3; _temp108;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_arr
decl_kind;{ void* _temp109=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp135;
_LL111: if(*(( int*) _temp109) == Cyc_Absyn_Var_d){ _LL136: _temp135=(( struct
Cyc_Absyn_Var_d_struct*) _temp109)->f1; goto _LL112;} else{ goto _LL113;} _LL113:
if(*(( int*) _temp109) == Cyc_Absyn_Let_d){ goto _LL114;} else{ goto _LL115;}
_LL115: if(*(( int*) _temp109) == Cyc_Absyn_Letv_d){ goto _LL116;} else{ goto
_LL117;} _LL117: if(*(( int*) _temp109) == Cyc_Absyn_Fn_d){ goto _LL118;} else{
goto _LL119;} _LL119: if(*(( int*) _temp109) == Cyc_Absyn_Struct_d){ goto _LL120;}
else{ goto _LL121;} _LL121: if(*(( int*) _temp109) == Cyc_Absyn_Union_d){ goto
_LL122;} else{ goto _LL123;} _LL123: if(*(( int*) _temp109) == Cyc_Absyn_Tunion_d){
goto _LL124;} else{ goto _LL125;} _LL125: if(*(( int*) _temp109) == Cyc_Absyn_Typedef_d){
goto _LL126;} else{ goto _LL127;} _LL127: if(*(( int*) _temp109) == Cyc_Absyn_Enum_d){
goto _LL128;} else{ goto _LL129;} _LL129: if(*(( int*) _temp109) == Cyc_Absyn_Namespace_d){
goto _LL130;} else{ goto _LL131;} _LL131: if(*(( int*) _temp109) == Cyc_Absyn_Using_d){
goto _LL132;} else{ goto _LL133;} _LL133: if(*(( int*) _temp109) == Cyc_Absyn_ExternC_d){
goto _LL134;} else{ goto _LL110;} _LL112: if( _temp135->initializer != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);}{ void* _temp137=(* _temp135->name).f1; struct
Cyc_List_List* _temp147; struct Cyc_List_List* _temp149; _LL139: if( _temp137 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL140;} else{ goto _LL141;} _LL141: if((
unsigned int) _temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_Rel_n: 0){ _LL148:
_temp147=(( struct Cyc_Absyn_Rel_n_struct*) _temp137)->f1; if( _temp147 == 0){
goto _LL142;} else{ goto _LL143;}} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_Abs_n: 0){ _LL150: _temp149=((
struct Cyc_Absyn_Abs_n_struct*) _temp137)->f1; if( _temp149 == 0){ goto _LL144;}
else{ goto _LL145;}} else{ goto _LL145;} _LL145: goto _LL146; _LL140: goto
_LL138; _LL142: goto _LL138; _LL144: goto _LL138; _LL146: Cyc_Parse_err(
_tag_arr("module names not allowed on parameter declarations", sizeof(
unsigned char), 51u), x->loc); goto _LL138; _LL138:;}{ int found= 0; for( 0;
params != 0; params=(( struct Cyc_List_List*) _check_null( params))->tl){ if(
Cyc_String_zstrptrcmp((* _temp135->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( params))->hd) == 0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct _tagged_arr(* _temp151)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp155= _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp156;
_temp156.tag= Cyc_Stdio_String_pa; _temp156.f1=( struct _tagged_arr)*(* _temp135->name).f2;{
void* _temp154=( void*)& _temp156; void* _temp152[ 1u]={ _temp154}; struct
_tagged_arr _temp153={( void*) _temp152,( void*) _temp152,( void*)( _temp152 + 1u)};
_temp151( _temp155, _temp153);}}), x->loc);} return;} _LL114: decl_kind=
_tag_arr("let declaration", sizeof( unsigned char), 16u); goto _LL110; _LL116:
decl_kind= _tag_arr("let declaration", sizeof( unsigned char), 16u); goto _LL110;
_LL118: decl_kind= _tag_arr("function declaration", sizeof( unsigned char), 21u);
goto _LL110; _LL120: decl_kind= _tag_arr("struct declaration", sizeof(
unsigned char), 19u); goto _LL110; _LL122: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL110; _LL124: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL110; _LL126: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL110; _LL128: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL110; _LL130: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL110; _LL132: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL110; _LL134: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL110; _LL110:;}(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct _tagged_arr(* _temp157)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp161= _tag_arr("%s appears in parameter type",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp162;
_temp162.tag= Cyc_Stdio_String_pa; _temp162.f1=( struct _tagged_arr) decl_kind;{
void* _temp160=( void*)& _temp162; void* _temp158[ 1u]={ _temp160}; struct
_tagged_arr _temp159={( void*) _temp158,( void*) _temp158,( void*)( _temp158 + 1u)};
_temp157( _temp161, _temp159);}}), x->loc); return;} struct _tuple12{ struct Cyc_List_List*
f1; struct Cyc_Position_Segment* f2; } ; static struct _tuple2* Cyc_Parse_get_param_type(
struct _tuple12* env, struct _tagged_arr* x){ struct _tuple12 _temp165; struct
Cyc_Position_Segment* _temp166; struct Cyc_List_List* _temp168; struct _tuple12*
_temp163= env; _temp165=* _temp163; _LL169: _temp168= _temp165.f1; goto _LL167;
_LL167: _temp166= _temp165.f2; goto _LL164; _LL164: if( _temp168 == 0){ return((
struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_arr)({ struct _tagged_arr(* _temp170)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp174= _tag_arr("missing type for parameter %s", sizeof( unsigned char), 30u);
struct Cyc_Stdio_String_pa_struct _temp175; _temp175.tag= Cyc_Stdio_String_pa;
_temp175.f1=( struct _tagged_arr)* x;{ void* _temp173=( void*)& _temp175; void*
_temp171[ 1u]={ _temp173}; struct _tagged_arr _temp172={( void*) _temp171,( void*)
_temp171,( void*)( _temp171 + 1u)}; _temp170( _temp174, _temp172);}}), _temp166);}{
void* _temp176=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp168))->hd)->r; struct Cyc_Absyn_Vardecl* _temp182; _LL178: if(*((
int*) _temp176) == Cyc_Absyn_Var_d){ _LL183: _temp182=(( struct Cyc_Absyn_Var_d_struct*)
_temp176)->f1; goto _LL179;} else{ goto _LL180;} _LL180: goto _LL181; _LL179:{
void* _temp184=(* _temp182->name).f1; struct Cyc_List_List* _temp194; struct Cyc_List_List*
_temp196; _LL186: if( _temp184 ==( void*) Cyc_Absyn_Loc_n){ goto _LL187;} else{
goto _LL188;} _LL188: if(( unsigned int) _temp184 > 1u?*(( int*) _temp184) ==
Cyc_Absyn_Rel_n: 0){ _LL195: _temp194=(( struct Cyc_Absyn_Rel_n_struct*)
_temp184)->f1; if( _temp194 == 0){ goto _LL189;} else{ goto _LL190;}} else{ goto
_LL190;} _LL190: if(( unsigned int) _temp184 > 1u?*(( int*) _temp184) == Cyc_Absyn_Abs_n:
0){ _LL197: _temp196=(( struct Cyc_Absyn_Abs_n_struct*) _temp184)->f1; if(
_temp196 == 0){ goto _LL191;} else{ goto _LL192;}} else{ goto _LL192;} _LL192:
goto _LL193; _LL187: goto _LL185; _LL189: goto _LL185; _LL191: goto _LL185;
_LL193: Cyc_Parse_err( _tag_arr("module name not allowed on parameter", sizeof(
unsigned char), 37u), _temp166); goto _LL185; _LL185:;} if( Cyc_String_zstrptrcmp((*
_temp182->name).f2, x) == 0){ return({ struct _tuple2* _temp198=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2) * 1); _temp198[ 0]=({ struct _tuple2 _temp199;
_temp199.f1=({ struct Cyc_Core_Opt* _temp200=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp200[ 0]=({ struct Cyc_Core_Opt _temp201;
_temp201.v=( void*)(* _temp182->name).f2; _temp201;}); _temp200;}); _temp199.f2=
_temp182->tq; _temp199.f3=( void*) _temp182->type; _temp199;}); _temp198;});}
else{ return Cyc_Parse_get_param_type(({ struct _tuple12* _temp202=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp202->f1=(( struct Cyc_List_List*)
_check_null( _temp168))->tl; _temp202->f2= _temp166; _temp202;}), x);} _LL181:
return(( struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration", sizeof(
unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp168))->hd)->loc); _LL177:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp203= tm; _LL205: if(( unsigned int) _temp203 > 1u?*(( int*)
_temp203) == Cyc_Absyn_TypeParams_mod: 0){ goto _LL206;} else{ goto _LL207;}
_LL207: goto _LL208; _LL206: return 1; _LL208: return 0; _LL204:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) == 0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp209=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp209[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp210; _temp210.tag= Cyc_Absyn_VarType; _temp210.f1=({ struct Cyc_Absyn_Tvar*
_temp211=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp211->name=({ struct _tagged_arr* _temp212=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp212[ 0]= s; _temp212;}); _temp211->identity=
0; _temp211->kind= k; _temp211;}); _temp210;}); _temp209;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp213=
t; struct Cyc_Absyn_Tvar* _temp219; _LL215: if(( unsigned int) _temp213 > 4u?*((
int*) _temp213) == Cyc_Absyn_VarType: 0){ _LL220: _temp219=(( struct Cyc_Absyn_VarType_struct*)
_temp213)->f1; goto _LL216;} else{ goto _LL217;} _LL217: goto _LL218; _LL216:
return _temp219; _LL218: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL214:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp221=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp221[ 0]=({ struct Cyc_Absyn_VarType_struct _temp222; _temp222.tag= Cyc_Absyn_VarType;
_temp222.f1= pr; _temp222;}); _temp221;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp223=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp229; _LL225:
if(( unsigned int) _temp223 > 1u?*(( int*) _temp223) == Cyc_Absyn_Function_mod:
0){ _LL230: _temp229=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp223)->f1;
goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp231= _temp229; struct Cyc_List_List* _temp237; _LL233: if(*((
int*) _temp231) == Cyc_Absyn_WithTypes){ goto _LL234;} else{ goto _LL235;}
_LL235: if(*(( int*) _temp231) == Cyc_Absyn_NoTypes){ _LL238: _temp237=(( struct
Cyc_Absyn_NoTypes_struct*) _temp231)->f1; goto _LL236;} else{ goto _LL232;}
_LL234: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL236:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp237,
tds);{ struct _tuple12* _temp239=({ struct _tuple12* _temp245=( struct _tuple12*)
GC_malloc( sizeof( struct _tuple12)); _temp245->f1= tds; _temp245->f2= loc;
_temp245;}); return({ struct Cyc_List_List* _temp240=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp240->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp241=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp241[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp242; _temp242.tag= Cyc_Absyn_Function_mod;
_temp242.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp243=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp243[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp244; _temp244.tag= Cyc_Absyn_WithTypes;
_temp244.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp239, _temp237); _temp244.f2= 0; _temp244.f3= 0;
_temp244.f4= 0; _temp244.f5= 0; _temp244;}); _temp243;})); _temp242;}); _temp241;}));
_temp240->tl= 0; _temp240;});} _LL232:;} else{ return({ struct Cyc_List_List*
_temp246=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp246->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp246->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp246;});} _LL228: return({ struct Cyc_List_List*
_temp247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp247->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp247->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp247;}); _LL224:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp248=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp248->id= d->id; _temp248->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp248;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp249=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL251: if( _temp249 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL252;} else{ goto _LL253;} _LL253: if(
_temp249 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL254;} else{ goto _LL255;}
_LL255: if( _temp249 ==( void*) Cyc_Parse_Static_sc){ goto _LL256;} else{ goto
_LL257;} _LL257: goto _LL258; _LL252: sc=( void*) Cyc_Absyn_Extern; goto _LL250;
_LL254: sc=( void*) Cyc_Absyn_ExternC; goto _LL250; _LL256: sc=( void*) Cyc_Absyn_Static;
goto _LL250; _LL258: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL250; _LL250:;}}{ struct Cyc_Core_Opt*
_temp261; void* _temp263; struct _tuple5 _temp259= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL264: _temp263= _temp259.f1; goto _LL262; _LL262: _temp261=
_temp259.f2; goto _LL260; _LL260: { struct Cyc_List_List* _temp267; struct Cyc_List_List*
_temp269; void* _temp271; struct Cyc_Absyn_Tqual _temp273; struct _tuple6
_temp265= Cyc_Parse_apply_tms( tq, _temp263, atts, d->tms); _LL274: _temp273=
_temp265.f1; goto _LL272; _LL272: _temp271= _temp265.f2; goto _LL270; _LL270:
_temp269= _temp265.f3; goto _LL268; _LL268: _temp267= _temp265.f4; goto _LL266;
_LL266: if( _temp261 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp269 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp275= _temp271; struct Cyc_Absyn_FnInfo _temp281; struct Cyc_List_List*
_temp283; struct Cyc_List_List* _temp285; struct Cyc_Absyn_VarargInfo* _temp287;
int _temp289; struct Cyc_List_List* _temp291; void* _temp293; struct Cyc_Core_Opt*
_temp295; struct Cyc_List_List* _temp297; _LL277: if(( unsigned int) _temp275 >
4u?*(( int*) _temp275) == Cyc_Absyn_FnType: 0){ _LL282: _temp281=(( struct Cyc_Absyn_FnType_struct*)
_temp275)->f1; _LL298: _temp297= _temp281.tvars; goto _LL296; _LL296: _temp295=
_temp281.effect; goto _LL294; _LL294: _temp293=( void*) _temp281.ret_typ; goto
_LL292; _LL292: _temp291= _temp281.args; goto _LL290; _LL290: _temp289= _temp281.c_varargs;
goto _LL288; _LL288: _temp287= _temp281.cyc_varargs; goto _LL286; _LL286:
_temp285= _temp281.rgn_po; goto _LL284; _LL284: _temp283= _temp281.attributes;
goto _LL278;} else{ goto _LL279;} _LL279: goto _LL280; _LL278: { struct Cyc_List_List*
_temp299=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp291); return({ struct Cyc_Absyn_Fndecl*
_temp300=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp300->sc=( void*) sc; _temp300->name= d->id; _temp300->tvs= _temp297;
_temp300->is_inline= is_inline; _temp300->effect= _temp295; _temp300->ret_type=(
void*) _temp293; _temp300->args= _temp299; _temp300->c_varargs= _temp289;
_temp300->cyc_varargs= _temp287; _temp300->rgn_po= _temp285; _temp300->body=
body; _temp300->cached_typ= 0; _temp300->param_vardecls= 0; _temp300->attributes=
Cyc_List_append( _temp283, _temp267); _temp300;});} _LL280: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL276:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp301=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp301->f1=({ struct _tagged_arr*
_temp302=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp302[ 0]= _tag_arr("?", sizeof( unsigned char), 2u); _temp302;}); _temp301->f2=(*
t).f2; _temp301->f3=(* t).f3; _temp301;});} else{ return({ struct _tuple4*
_temp303=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp303->f1=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp303->f2=(*
t).f2; _temp303->f3=(* t).f3; _temp303;});}} static unsigned char _temp304[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp304, _temp304, _temp304 + 52u};
static unsigned char _temp305[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp305, _temp305, _temp305 + 63u};
static unsigned char _temp306[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp306, _temp306, _temp306 + 50u};
static unsigned char _temp307[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp307, _temp307, _temp307 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp308=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp322; void* _temp324; struct Cyc_Position_Segment*
_temp326; struct Cyc_Position_Segment* _temp328; struct Cyc_Position_Segment*
_temp330; struct Cyc_Position_Segment* _temp332; struct Cyc_Absyn_Decl* _temp334;
_LL310: if(*(( int*) _temp308) == Cyc_Parse_Type_spec){ _LL325: _temp324=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp308)->f1; goto _LL323; _LL323: _temp322=((
struct Cyc_Parse_Type_spec_struct*) _temp308)->f2; goto _LL311;} else{ goto
_LL312;} _LL312: if(*(( int*) _temp308) == Cyc_Parse_Signed_spec){ _LL327:
_temp326=(( struct Cyc_Parse_Signed_spec_struct*) _temp308)->f1; goto _LL313;}
else{ goto _LL314;} _LL314: if(*(( int*) _temp308) == Cyc_Parse_Unsigned_spec){
_LL329: _temp328=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp308)->f1; goto
_LL315;} else{ goto _LL316;} _LL316: if(*(( int*) _temp308) == Cyc_Parse_Short_spec){
_LL331: _temp330=(( struct Cyc_Parse_Short_spec_struct*) _temp308)->f1; goto
_LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp308) == Cyc_Parse_Long_spec){
_LL333: _temp332=(( struct Cyc_Parse_Long_spec_struct*) _temp308)->f1; goto
_LL319;} else{ goto _LL320;} _LL320: if(*(( int*) _temp308) == Cyc_Parse_Decl_spec){
_LL335: _temp334=(( struct Cyc_Parse_Decl_spec_struct*) _temp308)->f1; goto
_LL321;} else{ goto _LL309;} _LL311: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp322);} last_loc= _temp322; seen_type= 1; t= _temp324; goto _LL309; _LL313:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp326);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp326);} last_loc= _temp326; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL309; _LL315: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp328);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp328);} last_loc= _temp328; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL309; _LL317: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp330);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp330);} last_loc= _temp330; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL309; _LL319: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp332);} if( seen_size){ void* _temp336= sz; _LL338: if( _temp336 ==( void*)
Cyc_Absyn_B4){ goto _LL339;} else{ goto _LL340;} _LL340: goto _LL341; _LL339: sz=(
void*) Cyc_Absyn_B8; goto _LL337; _LL341: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp332); goto _LL337; _LL337:;} last_loc=
_temp332; seen_size= 1; goto _LL309; _LL321: last_loc= _temp334->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp342=( void*) _temp334->r;
struct Cyc_Absyn_Structdecl* _temp354; struct Cyc_Absyn_Tuniondecl* _temp356;
struct Cyc_Absyn_Uniondecl* _temp358; struct Cyc_Absyn_Enumdecl* _temp360;
_LL344: if(*(( int*) _temp342) == Cyc_Absyn_Struct_d){ _LL355: _temp354=((
struct Cyc_Absyn_Struct_d_struct*) _temp342)->f1; goto _LL345;} else{ goto
_LL346;} _LL346: if(*(( int*) _temp342) == Cyc_Absyn_Tunion_d){ _LL357: _temp356=((
struct Cyc_Absyn_Tunion_d_struct*) _temp342)->f1; goto _LL347;} else{ goto
_LL348;} _LL348: if(*(( int*) _temp342) == Cyc_Absyn_Union_d){ _LL359: _temp358=((
struct Cyc_Absyn_Union_d_struct*) _temp342)->f1; goto _LL349;} else{ goto _LL350;}
_LL350: if(*(( int*) _temp342) == Cyc_Absyn_Enum_d){ _LL361: _temp360=(( struct
Cyc_Absyn_Enum_d_struct*) _temp342)->f1; goto _LL351;} else{ goto _LL352;}
_LL352: goto _LL353; _LL345: { struct Cyc_List_List* _temp362=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp354->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp363=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp363[ 0]=({ struct Cyc_Absyn_StructType_struct _temp364; _temp364.tag= Cyc_Absyn_StructType;
_temp364.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp354->name))->v); _temp364.f2= _temp362; _temp364.f3= 0;
_temp364;}); _temp363;}); if( _temp354->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp365=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp365->v=( void*) _temp334; _temp365;});} goto _LL343;} _LL347: { struct Cyc_List_List*
_temp366=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp356->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp367=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp367[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp368; _temp368.tag= Cyc_Absyn_TunionType;
_temp368.f1=({ struct Cyc_Absyn_TunionInfo _temp369; _temp369.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp370=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp370[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp371; _temp371.tag= Cyc_Absyn_KnownTunion;
_temp371.f1= _temp356; _temp371;}); _temp370;})); _temp369.targs= _temp366;
_temp369.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp369;}); _temp368;});
_temp367;}); if( _temp356->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp372=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp372->v=( void*) _temp334; _temp372;});} goto _LL343;} _LL349: { struct Cyc_List_List*
_temp373=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp358->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp374=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp374[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp375; _temp375.tag= Cyc_Absyn_UnionType; _temp375.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp358->name))->v); _temp375.f2= _temp373; _temp375.f3= 0; _temp375;});
_temp374;}); if( _temp358->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp376=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp376->v=( void*) _temp334; _temp376;});} goto _LL343;} _LL351: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp377=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp377[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp378; _temp378.tag= Cyc_Absyn_EnumType; _temp378.f1=
_temp360->name; _temp378.f2= 0; _temp378;}); _temp377;}); declopt=({ struct Cyc_Core_Opt*
_temp379=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp379->v=( void*) _temp334; _temp379;}); goto _LL343; _LL353:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp334->loc); goto _LL343; _LL343:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp334->loc);} goto _LL309; _LL309:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp380=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp380[ 0]=({ struct Cyc_Absyn_IntType_struct _temp381; _temp381.tag= Cyc_Absyn_IntType;
_temp381.f1=( void*) sgn; _temp381.f2=( void*) sz; _temp381;}); _temp380;});}
else{ if( seen_sign){ void* _temp382= t; void* _temp388; _LL384: if((
unsigned int) _temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_IntType: 0){ _LL389:
_temp388=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp382)->f2; goto _LL385;}
else{ goto _LL386;} _LL386: goto _LL387; _LL385: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp390=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp390[ 0]=({ struct Cyc_Absyn_IntType_struct _temp391; _temp391.tag= Cyc_Absyn_IntType;
_temp391.f1=( void*) sgn; _temp391.f2=( void*) _temp388; _temp391;}); _temp390;});
goto _LL383; _LL387: Cyc_Parse_err( _tag_arr("sign specification on non-integral type",
sizeof( unsigned char), 40u), last_loc); goto _LL383; _LL383:;} if( seen_size){
void* _temp392= t; void* _temp398; _LL394: if(( unsigned int) _temp392 > 4u?*((
int*) _temp392) == Cyc_Absyn_IntType: 0){ _LL399: _temp398=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp392)->f1; goto _LL395;} else{ goto _LL396;} _LL396: goto _LL397; _LL395: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp400=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp400[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp401; _temp401.tag= Cyc_Absyn_IntType; _temp401.f1=( void*) _temp398;
_temp401.f2=( void*) sz; _temp401;}); _temp400;}); goto _LL393; _LL397: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL393; _LL393:;}} return({ struct _tuple5 _temp402; _temp402.f1=
t; _temp402.f2= declopt; _temp402;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp403=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp404= _temp403->id; struct Cyc_List_List* _temp407; struct
Cyc_List_List* _temp409; void* _temp411; struct Cyc_Absyn_Tqual _temp413; struct
_tuple6 _temp405= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp403->tms);
_LL414: _temp413= _temp405.f1; goto _LL412; _LL412: _temp411= _temp405.f2; goto
_LL410; _LL410: _temp409= _temp405.f3; goto _LL408; _LL408: _temp407= _temp405.f4;
goto _LL406; _LL406: return({ struct Cyc_List_List* _temp415=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp415->hd=( void*)({ struct
_tuple7* _temp416=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp416->f1= _temp404; _temp416->f2= _temp413; _temp416->f3= _temp411; _temp416->f4=
_temp409; _temp416->f5= _temp407; _temp416;}); _temp415->tl= Cyc_Parse_apply_tmss(
_temp413, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp415;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp417; _temp417.f1= tq; _temp417.f2=
t; _temp417.f3= 0; _temp417.f4= atts; _temp417;});}{ void* _temp418=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp432;
void* _temp434; struct Cyc_Position_Segment* _temp436; struct Cyc_List_List*
_temp438; struct Cyc_Absyn_Tqual _temp440; void* _temp442; void* _temp444;
struct Cyc_List_List* _temp446; struct Cyc_Position_Segment* _temp448; _LL420:
if( _temp418 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL421;} else{ goto _LL422;}
_LL422: if(( unsigned int) _temp418 > 1u?*(( int*) _temp418) == Cyc_Absyn_ConstArray_mod:
0){ _LL433: _temp432=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp418)->f1;
goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int) _temp418 > 1u?*((
int*) _temp418) == Cyc_Absyn_Function_mod: 0){ _LL435: _temp434=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp418)->f1; goto _LL425;} else{ goto
_LL426;} _LL426: if(( unsigned int) _temp418 > 1u?*(( int*) _temp418) == Cyc_Absyn_TypeParams_mod:
0){ _LL439: _temp438=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp418)->f1;
goto _LL437; _LL437: _temp436=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp418)->f2; goto _LL427;} else{ goto _LL428;} _LL428: if(( unsigned int)
_temp418 > 1u?*(( int*) _temp418) == Cyc_Absyn_Pointer_mod: 0){ _LL445: _temp444=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp418)->f1; goto _LL443;
_LL443: _temp442=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp418)->f2;
goto _LL441; _LL441: _temp440=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp418)->f3;
goto _LL429;} else{ goto _LL430;} _LL430: if(( unsigned int) _temp418 > 1u?*((
int*) _temp418) == Cyc_Absyn_Attributes_mod: 0){ _LL449: _temp448=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp418)->f1; goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp418)->f2; goto _LL431;} else{ goto _LL419;} _LL421: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp450=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp450[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp451; _temp451.tag= Cyc_Absyn_ArrayType;
_temp451.f1=( void*) t; _temp451.f2= tq; _temp451.f3= 0; _temp451;}); _temp450;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL423: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp452=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp452[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp453; _temp453.tag= Cyc_Absyn_ArrayType;
_temp453.f1=( void*) t; _temp453.f2= tq; _temp453.f3=( struct Cyc_Absyn_Exp*)
_temp432; _temp453;}); _temp452;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL425: { void* _temp454= _temp434; struct Cyc_List_List* _temp460;
struct Cyc_Core_Opt* _temp462; struct Cyc_Absyn_VarargInfo* _temp464; int
_temp466; struct Cyc_List_List* _temp468; struct Cyc_Position_Segment* _temp470;
_LL456: if(*(( int*) _temp454) == Cyc_Absyn_WithTypes){ _LL469: _temp468=((
struct Cyc_Absyn_WithTypes_struct*) _temp454)->f1; goto _LL467; _LL467: _temp466=((
struct Cyc_Absyn_WithTypes_struct*) _temp454)->f2; goto _LL465; _LL465: _temp464=((
struct Cyc_Absyn_WithTypes_struct*) _temp454)->f3; goto _LL463; _LL463: _temp462=((
struct Cyc_Absyn_WithTypes_struct*) _temp454)->f4; goto _LL461; _LL461: _temp460=((
struct Cyc_Absyn_WithTypes_struct*) _temp454)->f5; goto _LL457;} else{ goto
_LL458;} _LL458: if(*(( int*) _temp454) == Cyc_Absyn_NoTypes){ _LL471: _temp470=((
struct Cyc_Absyn_NoTypes_struct*) _temp454)->f2; goto _LL459;} else{ goto _LL455;}
_LL457: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp472=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp472->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp472->tl= fn_atts; _temp472;});} else{ new_atts=({ struct Cyc_List_List*
_temp473=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp473->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp473->tl= new_atts; _temp473;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp474=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp480; _LL476: if(( unsigned int) _temp474 > 1u?*(( int*) _temp474) == Cyc_Absyn_TypeParams_mod:
0){ _LL481: _temp480=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp474)->f1;
goto _LL477;} else{ goto _LL478;} _LL478: goto _LL479; _LL477: typvars= _temp480;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL475; _LL479: goto
_LL475; _LL475:;} if((((( ! _temp466? _temp464 == 0: 0)? _temp468 != 0: 0)?((
struct Cyc_List_List*) _check_null( _temp468))->tl == 0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp468))->hd)).f1 == 0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp468))->hd)).f3 ==( void*)
Cyc_Absyn_VoidType: 0){ _temp468= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp468); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp462, t, _temp468, _temp466, _temp464, _temp460, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL459:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp470)); _LL455:;} _LL427: if((( struct Cyc_List_List*) _check_null(
tms))->tl == 0){ return({ struct _tuple6 _temp482; _temp482.f1= tq; _temp482.f2=
t; _temp482.f3= _temp438; _temp482.f4= atts; _temp482;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp436)); _LL429: { void* _temp483= _temp444;
struct Cyc_Absyn_Exp* _temp491; struct Cyc_Absyn_Exp* _temp493; _LL485: if((
unsigned int) _temp483 > 1u?*(( int*) _temp483) == Cyc_Absyn_NonNullable_ps: 0){
_LL492: _temp491=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp483)->f1; goto
_LL486;} else{ goto _LL487;} _LL487: if(( unsigned int) _temp483 > 1u?*(( int*)
_temp483) == Cyc_Absyn_Nullable_ps: 0){ _LL494: _temp493=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp483)->f1; goto _LL488;} else{ goto _LL489;} _LL489: if( _temp483 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL490;} else{ goto _LL484;} _LL486: return Cyc_Parse_apply_tms(
_temp440, Cyc_Absyn_atb_typ( t, _temp442, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp495=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp495[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp496; _temp496.tag= Cyc_Absyn_Upper_b;
_temp496.f1= _temp491; _temp496;}); _temp495;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL488: return Cyc_Parse_apply_tms( _temp440, Cyc_Absyn_starb_typ(
t, _temp442, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp497=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp497[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp498; _temp498.tag= Cyc_Absyn_Upper_b;
_temp498.f1= _temp493; _temp498;}); _temp497;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL490: return Cyc_Parse_apply_tms( _temp440, Cyc_Absyn_tagged_typ(
t, _temp442, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL484:;}
_LL431: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp446),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL419:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp501; void* _temp503; struct _tuple5 _temp499= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL504: _temp503= _temp499.f1; goto _LL502; _LL502: _temp501=
_temp499.f2; goto _LL500; _LL500: if( _temp501 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp503;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp505=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp505[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp506; _temp506.tag= Cyc_Absyn_Decl_s;
_temp506.f1= d; _temp506.f2= s; _temp506;}); _temp505;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err( _tag_arr("inline is only allowed on function definitions",
sizeof( unsigned char), 47u), loc);} if( tss == 0){ Cyc_Parse_err( _tag_arr("missing type specifiers in declaration",
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc != 0){ void* _temp507=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL509: if( _temp507 ==(
void*) Cyc_Parse_Typedef_sc){ goto _LL510;} else{ goto _LL511;} _LL511: if(
_temp507 ==( void*) Cyc_Parse_Extern_sc){ goto _LL512;} else{ goto _LL513;}
_LL513: if( _temp507 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL514;} else{ goto
_LL515;} _LL515: if( _temp507 ==( void*) Cyc_Parse_Static_sc){ goto _LL516;}
else{ goto _LL517;} _LL517: if( _temp507 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL518;} else{ goto _LL519;} _LL519: if( _temp507 ==( void*) Cyc_Parse_Register_sc){
goto _LL520;} else{ goto _LL521;} _LL521: if( _temp507 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL522;} else{ goto _LL508;} _LL510: istypedef= 1; goto _LL508; _LL512: s=(
void*) Cyc_Absyn_Extern; goto _LL508; _LL514: s=( void*) Cyc_Absyn_ExternC; goto
_LL508; _LL516: s=( void*) Cyc_Absyn_Static; goto _LL508; _LL518: s=( void*) Cyc_Absyn_Public;
goto _LL508; _LL520: s=( void*) Cyc_Absyn_Public; goto _LL508; _LL522: s=( void*)
Cyc_Absyn_Abstract; goto _LL508; _LL508:;}{ struct Cyc_List_List* _temp525;
struct Cyc_List_List* _temp527; struct _tuple0 _temp523=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL528: _temp527= _temp523.f1;
goto _LL526; _LL526: _temp525= _temp523.f2; goto _LL524; _LL524: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp525; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp529= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp527 == 0){ struct Cyc_Core_Opt* _temp532; void* _temp534; struct _tuple5
_temp530= _temp529; _LL535: _temp534= _temp530.f1; goto _LL533; _LL533: _temp532=
_temp530.f2; goto _LL531; _LL531: if( _temp532 != 0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp532))->v;{
void* _temp536=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp548; struct Cyc_Absyn_Structdecl*
_temp550; struct Cyc_Absyn_Uniondecl* _temp552; struct Cyc_Absyn_Tuniondecl*
_temp554; _LL538: if(*(( int*) _temp536) == Cyc_Absyn_Enum_d){ _LL549: _temp548=((
struct Cyc_Absyn_Enum_d_struct*) _temp536)->f1; goto _LL539;} else{ goto _LL540;}
_LL540: if(*(( int*) _temp536) == Cyc_Absyn_Struct_d){ _LL551: _temp550=((
struct Cyc_Absyn_Struct_d_struct*) _temp536)->f1; goto _LL541;} else{ goto
_LL542;} _LL542: if(*(( int*) _temp536) == Cyc_Absyn_Union_d){ _LL553: _temp552=((
struct Cyc_Absyn_Union_d_struct*) _temp536)->f1; goto _LL543;} else{ goto _LL544;}
_LL544: if(*(( int*) _temp536) == Cyc_Absyn_Tunion_d){ _LL555: _temp554=((
struct Cyc_Absyn_Tunion_d_struct*) _temp536)->f1; goto _LL545;} else{ goto
_LL546;} _LL546: goto _LL547; _LL539:( void*)( _temp548->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL537; _LL541:( void*)( _temp550->sc=( void*)
s); _temp550->attributes= atts; goto _LL537; _LL543:( void*)( _temp552->sc=(
void*) s); _temp552->attributes= atts; goto _LL537; _LL545:( void*)( _temp554->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL537; _LL547: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL537:;}
return({ struct Cyc_List_List* _temp556=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp556->hd=( void*) d; _temp556->tl= 0;
_temp556;});} else{ void* _temp557= _temp534; struct Cyc_List_List* _temp571;
struct _tuple1* _temp573; struct Cyc_Absyn_TunionInfo _temp575; void* _temp577;
struct Cyc_Absyn_Tuniondecl* _temp579; struct Cyc_Absyn_TunionInfo _temp581;
struct Cyc_List_List* _temp583; void* _temp585; struct Cyc_Absyn_UnknownTunionInfo
_temp587; int _temp589; struct _tuple1* _temp591; struct Cyc_List_List* _temp593;
struct _tuple1* _temp595; struct _tuple1* _temp597; _LL559: if(( unsigned int)
_temp557 > 4u?*(( int*) _temp557) == Cyc_Absyn_StructType: 0){ _LL574: _temp573=((
struct Cyc_Absyn_StructType_struct*) _temp557)->f1; goto _LL572; _LL572:
_temp571=(( struct Cyc_Absyn_StructType_struct*) _temp557)->f2; goto _LL560;}
else{ goto _LL561;} _LL561: if(( unsigned int) _temp557 > 4u?*(( int*) _temp557)
== Cyc_Absyn_TunionType: 0){ _LL576: _temp575=(( struct Cyc_Absyn_TunionType_struct*)
_temp557)->f1; _LL578: _temp577=( void*) _temp575.tunion_info; if(*(( int*)
_temp577) == Cyc_Absyn_KnownTunion){ _LL580: _temp579=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp577)->f1; goto _LL562;} else{ goto _LL563;}} else{ goto _LL563;} _LL563:
if(( unsigned int) _temp557 > 4u?*(( int*) _temp557) == Cyc_Absyn_TunionType: 0){
_LL582: _temp581=(( struct Cyc_Absyn_TunionType_struct*) _temp557)->f1; _LL586:
_temp585=( void*) _temp581.tunion_info; if(*(( int*) _temp585) == Cyc_Absyn_UnknownTunion){
_LL588: _temp587=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp585)->f1;
_LL592: _temp591= _temp587.name; goto _LL590; _LL590: _temp589= _temp587.is_xtunion;
goto _LL584;} else{ goto _LL565;} _LL584: _temp583= _temp581.targs; goto _LL564;}
else{ goto _LL565;} _LL565: if(( unsigned int) _temp557 > 4u?*(( int*) _temp557)
== Cyc_Absyn_UnionType: 0){ _LL596: _temp595=(( struct Cyc_Absyn_UnionType_struct*)
_temp557)->f1; goto _LL594; _LL594: _temp593=(( struct Cyc_Absyn_UnionType_struct*)
_temp557)->f2; goto _LL566;} else{ goto _LL567;} _LL567: if(( unsigned int)
_temp557 > 4u?*(( int*) _temp557) == Cyc_Absyn_EnumType: 0){ _LL598: _temp597=((
struct Cyc_Absyn_EnumType_struct*) _temp557)->f1; goto _LL568;} else{ goto
_LL569;} _LL569: goto _LL570; _LL560: { struct Cyc_List_List* _temp599=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp571); struct Cyc_Absyn_Structdecl* _temp600=({
struct Cyc_Absyn_Structdecl* _temp604=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp604->sc=( void*) s; _temp604->name=({
struct Cyc_Core_Opt* _temp605=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp605->v=( void*)(( struct _tuple1*) _check_null( _temp573));
_temp605;}); _temp604->tvs= _temp599; _temp604->fields= 0; _temp604->attributes=
0; _temp604;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp601=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp601->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp602=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp602[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp603; _temp603.tag= Cyc_Absyn_Struct_d;
_temp603.f1= _temp600; _temp603;}); _temp602;}), loc); _temp601->tl= 0; _temp601;});}
_LL562: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp606=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp606->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp607=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp607[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp608; _temp608.tag= Cyc_Absyn_Tunion_d;
_temp608.f1= _temp579; _temp608;}); _temp607;}), loc); _temp606->tl= 0; _temp606;});
_LL564: { struct Cyc_List_List* _temp609=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp583);
struct Cyc_Absyn_Decl* _temp610= Cyc_Absyn_tunion_decl( s, _temp591, _temp609, 0,
_temp589, loc); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp611=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp611->hd=(
void*) _temp610; _temp611->tl= 0; _temp611;});} _LL566: { struct Cyc_List_List*
_temp612=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp593); struct Cyc_Absyn_Uniondecl* _temp613=({
struct Cyc_Absyn_Uniondecl* _temp618=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp618->sc=( void*) s; _temp618->name=({
struct Cyc_Core_Opt* _temp619=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp619->v=( void*)(( struct _tuple1*) _check_null( _temp595));
_temp619;}); _temp618->tvs= _temp612; _temp618->fields= 0; _temp618->attributes=
0; _temp618;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp614=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp614->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp615=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp615->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp616=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp616[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp617; _temp617.tag= Cyc_Absyn_Union_d;
_temp617.f1= _temp613; _temp617;}); _temp616;})); _temp615->loc= loc; _temp615;});
_temp614->tl= 0; _temp614;});} _LL568: { struct Cyc_Absyn_Enumdecl* _temp620=({
struct Cyc_Absyn_Enumdecl* _temp625=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp625->sc=( void*) s; _temp625->name=
_temp597; _temp625->fields= 0; _temp625;}); if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp621=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp621->hd=( void*)({ struct Cyc_Absyn_Decl* _temp622=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp622->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp623=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp623[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp624; _temp624.tag= Cyc_Absyn_Enum_d; _temp624.f1= _temp620; _temp624;});
_temp623;})); _temp622->loc= loc; _temp622;}); _temp621->tl= 0; _temp621;});}
_LL570: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL558:;}} else{ void* t= _temp529.f1; struct Cyc_List_List*
_temp626= Cyc_Parse_apply_tmss( tq, t, _temp527, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp626); if( _temp529.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp529.f2))->v;{ void* _temp627=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp639; struct Cyc_Absyn_Tuniondecl* _temp641;
struct Cyc_Absyn_Uniondecl* _temp643; struct Cyc_Absyn_Enumdecl* _temp645;
_LL629: if(*(( int*) _temp627) == Cyc_Absyn_Struct_d){ _LL640: _temp639=((
struct Cyc_Absyn_Struct_d_struct*) _temp627)->f1; goto _LL630;} else{ goto
_LL631;} _LL631: if(*(( int*) _temp627) == Cyc_Absyn_Tunion_d){ _LL642: _temp641=((
struct Cyc_Absyn_Tunion_d_struct*) _temp627)->f1; goto _LL632;} else{ goto
_LL633;} _LL633: if(*(( int*) _temp627) == Cyc_Absyn_Union_d){ _LL644: _temp643=((
struct Cyc_Absyn_Union_d_struct*) _temp627)->f1; goto _LL634;} else{ goto _LL635;}
_LL635: if(*(( int*) _temp627) == Cyc_Absyn_Enum_d){ _LL646: _temp645=(( struct
Cyc_Absyn_Enum_d_struct*) _temp627)->f1; goto _LL636;} else{ goto _LL637;}
_LL637: goto _LL638; _LL630:( void*)( _temp639->sc=( void*) s); _temp639->attributes=
atts; atts= 0; goto _LL628; _LL632:( void*)( _temp641->sc=( void*) s); goto
_LL628; _LL634:( void*)( _temp643->sc=( void*) s); goto _LL628; _LL636:( void*)(
_temp645->sc=( void*) s); goto _LL628; _LL638: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL628; _LL628:;} decls=({ struct Cyc_List_List*
_temp647=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp647->hd=( void*) d; _temp647->tl= decls; _temp647;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct _tagged_arr(* _temp648)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp652= _tag_arr("bad attribute %s in typedef",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp653;
_temp653.tag= Cyc_Stdio_String_pa; _temp653.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp651=( void*)&
_temp653; void* _temp649[ 1u]={ _temp651}; struct _tagged_arr _temp650={( void*)
_temp649,( void*) _temp649,( void*)( _temp649 + 1u)}; _temp648( _temp652,
_temp650);}}), loc);} return decls;}} else{ if( _temp529.f2 != 0){ Cyc_Parse_unimp2(
_tag_arr("nested type declaration within declarator", sizeof( unsigned char), 42u),
loc);}{ struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp654=
_temp626; for( 0; _temp654 != 0;( _temp654=(( struct Cyc_List_List*) _check_null(
_temp654))->tl, _temp525=(( struct Cyc_List_List*) _check_null( _temp525))->tl)){
struct _tuple7 _temp657; struct Cyc_List_List* _temp658; struct Cyc_List_List*
_temp660; void* _temp662; struct Cyc_Absyn_Tqual _temp664; struct _tuple1*
_temp666; struct _tuple7* _temp655=( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp654))->hd; _temp657=* _temp655; _LL667: _temp666= _temp657.f1;
goto _LL665; _LL665: _temp664= _temp657.f2; goto _LL663; _LL663: _temp662=
_temp657.f3; goto _LL661; _LL661: _temp660= _temp657.f4; goto _LL659; _LL659:
_temp658= _temp657.f5; goto _LL656; _LL656: if( _temp660 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);} if(
_temp525 == 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp668=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp525))->hd; struct Cyc_Absyn_Vardecl*
_temp669= Cyc_Absyn_new_vardecl( _temp666, _temp662, _temp668); _temp669->tq=
_temp664;( void*)( _temp669->sc=( void*) s); _temp669->attributes= _temp658;{
struct Cyc_Absyn_Decl* _temp670=({ struct Cyc_Absyn_Decl* _temp672=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp672->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp673=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp673[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp674; _temp674.tag= Cyc_Absyn_Var_d; _temp674.f1= _temp669; _temp674;});
_temp673;})); _temp672->loc= loc; _temp672;}); decls=({ struct Cyc_List_List*
_temp671=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp671->hd=( void*) _temp670; _temp671->tl= decls; _temp671;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({
struct _tagged_arr(* _temp675)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp679= _tag_arr("bad kind: %s", sizeof( unsigned char), 13u);
struct Cyc_Stdio_String_pa_struct _temp680; _temp680.tag= Cyc_Stdio_String_pa;
_temp680.f1=( struct _tagged_arr) s;{ void* _temp678=( void*)& _temp680; void*
_temp676[ 1u]={ _temp678}; struct _tagged_arr _temp677={( void*) _temp676,( void*)
_temp676,( void*)( _temp676 + 1u)}; _temp675( _temp679, _temp677);}}), loc);
return( void*) Cyc_Absyn_BoxKind;} else{ switch(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0))){ case 'A': _LL681:
return( void*) Cyc_Absyn_AnyKind; case 'M': _LL682: return( void*) Cyc_Absyn_MemKind;
case 'B': _LL683: return( void*) Cyc_Absyn_BoxKind; case 'R': _LL684: return(
void*) Cyc_Absyn_RgnKind; case 'E': _LL685: return( void*) Cyc_Absyn_EffKind;
default: _LL686: Cyc_Parse_err(( struct _tagged_arr)({ struct _tagged_arr(*
_temp688)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp692= _tag_arr("bad kind: %s", sizeof( unsigned char), 13u);
struct Cyc_Stdio_String_pa_struct _temp693; _temp693.tag= Cyc_Stdio_String_pa;
_temp693.f1=( struct _tagged_arr) s;{ void* _temp691=( void*)& _temp693; void*
_temp689[ 1u]={ _temp691}; struct _tagged_arr _temp690={( void*) _temp689,( void*)
_temp689,( void*)( _temp689 + 1u)}; _temp688( _temp692, _temp690);}}), loc);
return( void*) Cyc_Absyn_BoxKind;}}} static struct Cyc_List_List* Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( atts == 0){ return tms;} else{ return({ struct Cyc_List_List* _temp694=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp694->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp695=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp695[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp696; _temp696.tag= Cyc_Absyn_Attributes_mod;
_temp696.f1= loc; _temp696.f2= atts; _temp696;}); _temp695;})); _temp694->tl=
tms; _temp694;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(( struct
_tagged_arr)({ struct _tagged_arr(* _temp697)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp701= _tag_arr("bad attribute %s within typedef",
sizeof( unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp702;
_temp702.tag= Cyc_Stdio_String_pa; _temp702.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null((* t).f5))->hd);{ void* _temp700=(
void*)& _temp702; void* _temp698[ 1u]={ _temp700}; struct _tagged_arr _temp699={(
void*) _temp698,( void*) _temp698,( void*)( _temp698 + 1u)}; _temp697( _temp701,
_temp699);}}), loc);} return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp703=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp703[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp704; _temp704.tag= Cyc_Absyn_Typedef_d;
_temp704.f1=({ struct Cyc_Absyn_Typedefdecl* _temp705=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp705->name= x; _temp705->tvs=(*
t).f4; _temp705->defn=( void*)(* t).f3; _temp705;}); _temp704;}); _temp703;}),
loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; unsigned char Cyc_AttributeList_tok[ 22u]="\000\000\000\000AttributeList_tok";
struct Cyc_AttributeList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Attribute_tok[ 18u]="\000\000\000\000Attribute_tok";
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char
Cyc_BlockItem_tok[ 18u]="\000\000\000\000BlockItem_tok"; struct Cyc_BlockItem_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Bool_tok[ 13u]="\000\000\000\000Bool_tok";
struct Cyc_Bool_tok_struct{ unsigned char* tag; int f1; } ; unsigned char Cyc_Char_tok[
13u]="\000\000\000\000Char_tok"; struct Cyc_Char_tok_struct{ unsigned char* tag;
unsigned char f1; } ; unsigned char Cyc_DeclList_tok[ 17u]="\000\000\000\000DeclList_tok";
struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; }
; unsigned char Cyc_DeclSpec_tok[ 17u]="\000\000\000\000DeclSpec_tok"; struct
Cyc_DeclSpec_tok_struct{ unsigned char* tag; struct Cyc_Parse_Declaration_spec*
f1; } ; unsigned char Cyc_DeclaratorExpoptList_tok[ 29u]="\000\000\000\000DeclaratorExpoptList_tok";
struct Cyc_DeclaratorExpoptList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_DeclaratorExpopt_tok[ 25u]="\000\000\000\000DeclaratorExpopt_tok";
struct _tuple13{ struct Cyc_Parse_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple13* f1;
} ; unsigned char Cyc_Declarator_tok[ 19u]="\000\000\000\000Declarator_tok";
struct Cyc_Declarator_tok_struct{ unsigned char* tag; struct Cyc_Parse_Declarator*
f1; } ; unsigned char Cyc_DesignatorList_tok[ 23u]="\000\000\000\000DesignatorList_tok";
struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Designator_tok[ 19u]="\000\000\000\000Designator_tok";
struct Cyc_Designator_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_EnumfieldList_tok[ 22u]="\000\000\000\000EnumfieldList_tok";
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Enumfield_tok[ 18u]="\000\000\000\000Enumfield_tok";
struct Cyc_Enumfield_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Enumfield*
f1; } ; unsigned char Cyc_ExpList_tok[ 16u]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; } ;
unsigned char Cyc_Exp_tok[ 12u]="\000\000\000\000Exp_tok"; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; unsigned char Cyc_FieldPatternList_tok[
25u]="\000\000\000\000FieldPatternList_tok"; struct Cyc_FieldPatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_FieldPattern_tok[
21u]="\000\000\000\000FieldPattern_tok"; struct _tuple14{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple14* f1; } ; unsigned char Cyc_FnDecl_tok[ 15u]="\000\000\000\000FnDecl_tok";
struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl* f1; }
; unsigned char Cyc_IdList_tok[ 15u]="\000\000\000\000IdList_tok"; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_InitDeclList_tok[
21u]="\000\000\000\000InitDeclList_tok"; struct Cyc_InitDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_InitDecl_tok[
17u]="\000\000\000\000InitDecl_tok"; struct _tuple15{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple15* f1; } ; unsigned char Cyc_InitializerList_tok[ 24u]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Int_tok[ 12u]="\000\000\000\000Int_tok"; struct
_tuple16{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{ unsigned char* tag;
struct _tuple16* f1; } ; unsigned char Cyc_Kind_tok[ 13u]="\000\000\000\000Kind_tok";
struct Cyc_Kind_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_Okay_tok[
13u]="\000\000\000\000Okay_tok"; unsigned char Cyc_ParamDeclListBool_tok[ 26u]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple17{ struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo*
f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List* f5; } ; struct Cyc_ParamDeclListBool_tok_struct{
unsigned char* tag; struct _tuple17* f1; } ; unsigned char Cyc_ParamDeclList_tok[
22u]="\000\000\000\000ParamDeclList_tok"; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_ParamDecl_tok[
18u]="\000\000\000\000ParamDecl_tok"; struct Cyc_ParamDecl_tok_struct{
unsigned char* tag; struct _tuple2* f1; } ; unsigned char Cyc_PatternList_tok[
20u]="\000\000\000\000PatternList_tok"; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_Pattern_tok[
16u]="\000\000\000\000Pattern_tok"; struct Cyc_Pattern_tok_struct{ unsigned char*
tag; struct Cyc_Absyn_Pat* f1; } ; unsigned char Cyc_Pointer_Sort_tok[ 21u]="\000\000\000\000Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_Primop_tok[ 15u]="\000\000\000\000Primop_tok"; struct Cyc_Primop_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Primopopt_tok[ 18u]="\000\000\000\000Primopopt_tok";
struct Cyc_Primopopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; unsigned char Cyc_QualId_tok[ 15u]="\000\000\000\000QualId_tok"; struct Cyc_QualId_tok_struct{
unsigned char* tag; struct _tuple1* f1; } ; unsigned char Cyc_QualSpecList_tok[
21u]="\000\000\000\000QualSpecList_tok"; struct _tuple18{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple18* f1; } ; unsigned char Cyc_Rgn_tok[ 12u]="\000\000\000\000Rgn_tok";
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_Rgnorder_tok[
17u]="\000\000\000\000Rgnorder_tok"; struct Cyc_Rgnorder_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_Scope_tok[
14u]="\000\000\000\000Scope_tok"; struct Cyc_Scope_tok_struct{ unsigned char*
tag; void* f1; } ; unsigned char Cyc_Short_tok[ 14u]="\000\000\000\000Short_tok";
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; unsigned char Cyc_Stmt_tok[
13u]="\000\000\000\000Stmt_tok"; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; unsigned char Cyc_StorageClass_tok[ 21u]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_String_tok[ 15u]="\000\000\000\000String_tok"; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_arr f1; } ; unsigned char Cyc_Stringopt_tok[
18u]="\000\000\000\000Stringopt_tok"; struct Cyc_Stringopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; unsigned char Cyc_StructFieldDeclListList_tok[
32u]="\000\000\000\000StructFieldDeclListList_tok"; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_StructFieldDeclList_tok[
28u]="\000\000\000\000StructFieldDeclList_tok"; struct Cyc_StructFieldDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_StructOrUnion_tok[
22u]="\000\000\000\000StructOrUnion_tok"; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_SwitchCClauseList_tok[ 26u]="\000\000\000\000SwitchCClauseList_tok";
struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_SwitchClauseList_tok[ 25u]="\000\000\000\000SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TunionFieldList_tok[ 24u]="\000\000\000\000TunionFieldList_tok";
struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TunionField_tok[ 20u]="\000\000\000\000TunionField_tok";
struct Cyc_TunionField_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tunionfield*
f1; } ; unsigned char Cyc_TypeList_tok[ 17u]="\000\000\000\000TypeList_tok";
struct Cyc_TypeList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; }
; unsigned char Cyc_TypeModifierList_tok[ 25u]="\000\000\000\000TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TypeOpt_tok[ 16u]="\000\000\000\000TypeOpt_tok";
struct Cyc_TypeOpt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; } ;
unsigned char Cyc_TypeQual_tok[ 17u]="\000\000\000\000TypeQual_tok"; struct Cyc_TypeQual_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tqual f1; } ; unsigned char Cyc_TypeSpecifier_tok[
22u]="\000\000\000\000TypeSpecifier_tok"; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Type_tok[ 13u]="\000\000\000\000Type_tok";
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; static
unsigned char _temp707[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp707, _temp707, _temp707 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp708= yy1; struct _tuple16* _temp714; _LL710: if(*(( void**)
_temp708) == Cyc_Int_tok){ _LL715: _temp714=(( struct Cyc_Int_tok_struct*)
_temp708)->f1; goto _LL711;} else{ goto _LL712;} _LL712: goto _LL713; _LL711:
return _temp714; _LL713:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL709:;}
static unsigned char _temp717[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp717, _temp717, _temp717 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp718= yy1; struct
_tagged_arr _temp724; _LL720: if(*(( void**) _temp718) == Cyc_String_tok){
_LL725: _temp724=(( struct Cyc_String_tok_struct*) _temp718)->f1; goto _LL721;}
else{ goto _LL722;} _LL722: goto _LL723; _LL721: return _temp724; _LL723:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL719:;} static unsigned char _temp727[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp727, _temp727, _temp727 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp728= yy1; unsigned char _temp734; _LL730: if(*(( void**) _temp728) ==
Cyc_Char_tok){ _LL735: _temp734=(( struct Cyc_Char_tok_struct*) _temp728)->f1;
goto _LL731;} else{ goto _LL732;} _LL732: goto _LL733; _LL731: return _temp734;
_LL733:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL729:;} static
unsigned char _temp737[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp737, _temp737, _temp737 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp738= yy1; void*
_temp744; _LL740: if(*(( void**) _temp738) == Cyc_Pointer_Sort_tok){ _LL745:
_temp744=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp738)->f1; goto
_LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL741: return _temp744;
_LL743:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL739:;} static
unsigned char _temp747[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp747, _temp747, _temp747 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp748= yy1; struct Cyc_Absyn_Exp*
_temp754; _LL750: if(*(( void**) _temp748) == Cyc_Exp_tok){ _LL755: _temp754=((
struct Cyc_Exp_tok_struct*) _temp748)->f1; goto _LL751;} else{ goto _LL752;}
_LL752: goto _LL753; _LL751: return _temp754; _LL753:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL749:;} static unsigned char _temp757[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp757, _temp757, _temp757 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp758= yy1;
struct Cyc_List_List* _temp764; _LL760: if(*(( void**) _temp758) == Cyc_ExpList_tok){
_LL765: _temp764=(( struct Cyc_ExpList_tok_struct*) _temp758)->f1; goto _LL761;}
else{ goto _LL762;} _LL762: goto _LL763; _LL761: return _temp764; _LL763:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL759:;} static unsigned char
_temp767[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp767, _temp767, _temp767 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp768= yy1; struct Cyc_List_List*
_temp774; _LL770: if(*(( void**) _temp768) == Cyc_InitializerList_tok){ _LL775:
_temp774=(( struct Cyc_InitializerList_tok_struct*) _temp768)->f1; goto _LL771;}
else{ goto _LL772;} _LL772: goto _LL773; _LL771: return _temp774; _LL773:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL769:;} static unsigned char
_temp777[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp777, _temp777, _temp777 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp778= yy1; void* _temp784; _LL780: if(*(( void**) _temp778)
== Cyc_Primop_tok){ _LL785: _temp784=( void*)(( struct Cyc_Primop_tok_struct*)
_temp778)->f1; goto _LL781;} else{ goto _LL782;} _LL782: goto _LL783; _LL781:
return _temp784; _LL783:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL779:;}
static unsigned char _temp787[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp787, _temp787, _temp787 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp788= yy1;
struct Cyc_Core_Opt* _temp794; _LL790: if(*(( void**) _temp788) == Cyc_Primopopt_tok){
_LL795: _temp794=(( struct Cyc_Primopopt_tok_struct*) _temp788)->f1; goto _LL791;}
else{ goto _LL792;} _LL792: goto _LL793; _LL791: return _temp794; _LL793:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL789:;} static unsigned char
_temp797[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp797, _temp797, _temp797 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp798= yy1; struct _tuple1* _temp804; _LL800: if(*(( void**)
_temp798) == Cyc_QualId_tok){ _LL805: _temp804=(( struct Cyc_QualId_tok_struct*)
_temp798)->f1; goto _LL801;} else{ goto _LL802;} _LL802: goto _LL803; _LL801:
return _temp804; _LL803:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL799:;}
static unsigned char _temp807[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp807, _temp807, _temp807 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp808= yy1;
struct Cyc_Absyn_Stmt* _temp814; _LL810: if(*(( void**) _temp808) == Cyc_Stmt_tok){
_LL815: _temp814=(( struct Cyc_Stmt_tok_struct*) _temp808)->f1; goto _LL811;}
else{ goto _LL812;} _LL812: goto _LL813; _LL811: return _temp814; _LL813:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL809:;} static unsigned char _temp817[
14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp817, _temp817, _temp817 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp818= yy1; void* _temp824; _LL820: if(*(( void**) _temp818)
== Cyc_BlockItem_tok){ _LL825: _temp824=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp818)->f1; goto _LL821;} else{ goto _LL822;} _LL822: goto _LL823; _LL821:
return _temp824; _LL823:( int) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL819:;} static unsigned char _temp827[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp827, _temp827, _temp827 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp828= yy1; struct Cyc_List_List* _temp834; _LL830: if(*((
void**) _temp828) == Cyc_SwitchClauseList_tok){ _LL835: _temp834=(( struct Cyc_SwitchClauseList_tok_struct*)
_temp828)->f1; goto _LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL831:
return _temp834; _LL833:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL829:;} static unsigned char _temp837[ 22u]="SwitchCClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp837, _temp837, _temp837 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ void* _temp838= yy1; struct Cyc_List_List* _temp844; _LL840: if(*((
void**) _temp838) == Cyc_SwitchCClauseList_tok){ _LL845: _temp844=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp838)->f1; goto _LL841;} else{ goto _LL842;} _LL842: goto _LL843; _LL841:
return _temp844; _LL843:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL839:;} static unsigned char _temp847[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp847, _temp847, _temp847 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp848= yy1;
struct Cyc_Absyn_Pat* _temp854; _LL850: if(*(( void**) _temp848) == Cyc_Pattern_tok){
_LL855: _temp854=(( struct Cyc_Pattern_tok_struct*) _temp848)->f1; goto _LL851;}
else{ goto _LL852;} _LL852: goto _LL853; _LL851: return _temp854; _LL853:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL849:;} static unsigned char
_temp857[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp857, _temp857, _temp857 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp858= yy1; struct Cyc_List_List*
_temp864; _LL860: if(*(( void**) _temp858) == Cyc_PatternList_tok){ _LL865:
_temp864=(( struct Cyc_PatternList_tok_struct*) _temp858)->f1; goto _LL861;}
else{ goto _LL862;} _LL862: goto _LL863; _LL861: return _temp864; _LL863:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL859:;} static unsigned char
_temp867[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp867, _temp867, _temp867 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp868= yy1; struct _tuple14* _temp874; _LL870: if(*(( void**)
_temp868) == Cyc_FieldPattern_tok){ _LL875: _temp874=(( struct Cyc_FieldPattern_tok_struct*)
_temp868)->f1; goto _LL871;} else{ goto _LL872;} _LL872: goto _LL873; _LL871:
return _temp874; _LL873:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL869:;} static unsigned char _temp877[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp877, _temp877, _temp877 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp878= yy1; struct Cyc_List_List* _temp884; _LL880: if(*((
void**) _temp878) == Cyc_FieldPatternList_tok){ _LL885: _temp884=(( struct Cyc_FieldPatternList_tok_struct*)
_temp878)->f1; goto _LL881;} else{ goto _LL882;} _LL882: goto _LL883; _LL881:
return _temp884; _LL883:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL879:;} static unsigned char _temp887[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp887, _temp887, _temp887 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp888= yy1;
struct Cyc_Absyn_Fndecl* _temp894; _LL890: if(*(( void**) _temp888) == Cyc_FnDecl_tok){
_LL895: _temp894=(( struct Cyc_FnDecl_tok_struct*) _temp888)->f1; goto _LL891;}
else{ goto _LL892;} _LL892: goto _LL893; _LL891: return _temp894; _LL893:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL889:;} static unsigned char _temp897[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp897, _temp897, _temp897 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp898= yy1; struct Cyc_List_List*
_temp904; _LL900: if(*(( void**) _temp898) == Cyc_DeclList_tok){ _LL905:
_temp904=(( struct Cyc_DeclList_tok_struct*) _temp898)->f1; goto _LL901;} else{
goto _LL902;} _LL902: goto _LL903; _LL901: return _temp904; _LL903:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL899:;} static unsigned char _temp907[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp907, _temp907, _temp907 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp908= yy1; struct Cyc_Parse_Declaration_spec* _temp914;
_LL910: if(*(( void**) _temp908) == Cyc_DeclSpec_tok){ _LL915: _temp914=((
struct Cyc_DeclSpec_tok_struct*) _temp908)->f1; goto _LL911;} else{ goto _LL912;}
_LL912: goto _LL913; _LL911: return _temp914; _LL913:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL909:;} static unsigned char _temp917[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp917, _temp917, _temp917 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp918= yy1; struct
_tuple15* _temp924; _LL920: if(*(( void**) _temp918) == Cyc_InitDecl_tok){
_LL925: _temp924=(( struct Cyc_InitDecl_tok_struct*) _temp918)->f1; goto _LL921;}
else{ goto _LL922;} _LL922: goto _LL923; _LL921: return _temp924; _LL923:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL919:;} static unsigned char
_temp927[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp927, _temp927, _temp927 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp928= yy1; struct Cyc_List_List*
_temp934; _LL930: if(*(( void**) _temp928) == Cyc_InitDeclList_tok){ _LL935:
_temp934=(( struct Cyc_InitDeclList_tok_struct*) _temp928)->f1; goto _LL931;}
else{ goto _LL932;} _LL932: goto _LL933; _LL931: return _temp934; _LL933:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL929:;} static unsigned char
_temp937[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp937, _temp937, _temp937 + 17u}}; void* Cyc_yyget_StorageClass_tok(
void* yy1){ void* _temp938= yy1; void* _temp944; _LL940: if(*(( void**) _temp938)
== Cyc_StorageClass_tok){ _LL945: _temp944=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp938)->f1; goto _LL941;} else{ goto _LL942;} _LL942: goto _LL943; _LL941:
return _temp944; _LL943:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL939:;} static unsigned char _temp947[ 18u]="TypeSpecifier_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp947, _temp947, _temp947 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp948= yy1; void* _temp954; _LL950: if(*(( void**) _temp948) ==
Cyc_TypeSpecifier_tok){ _LL955: _temp954=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp948)->f1; goto _LL951;} else{ goto _LL952;} _LL952: goto _LL953; _LL951:
return _temp954; _LL953:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL949:;} static unsigned char _temp957[ 18u]="StructOrUnion_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp957, _temp957, _temp957 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp958= yy1; void* _temp964; _LL960: if(*(( void**) _temp958) ==
Cyc_StructOrUnion_tok){ _LL965: _temp964=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp958)->f1; goto _LL961;} else{ goto _LL962;} _LL962: goto _LL963; _LL961:
return _temp964; _LL963:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL959:;} static unsigned char _temp967[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp967, _temp967, _temp967 + 13u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ void* _temp968= yy1;
struct Cyc_Absyn_Tqual _temp974; _LL970: if(*(( void**) _temp968) == Cyc_TypeQual_tok){
_LL975: _temp974=(( struct Cyc_TypeQual_tok_struct*) _temp968)->f1; goto _LL971;}
else{ goto _LL972;} _LL972: goto _LL973; _LL971: return _temp974; _LL973:( int)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL969:;} static unsigned char
_temp977[ 24u]="StructFieldDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{ _temp977, _temp977,
_temp977 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok( void*
yy1){ void* _temp978= yy1; struct Cyc_List_List* _temp984; _LL980: if(*(( void**)
_temp978) == Cyc_StructFieldDeclList_tok){ _LL985: _temp984=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp978)->f1; goto _LL981;} else{ goto _LL982;} _LL982: goto _LL983; _LL981:
return _temp984; _LL983:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL979:;} static unsigned char _temp987[ 28u]="StructFieldDeclListList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp987, _temp987, _temp987 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ void* _temp988= yy1; struct
Cyc_List_List* _temp994; _LL990: if(*(( void**) _temp988) == Cyc_StructFieldDeclListList_tok){
_LL995: _temp994=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp988)->f1;
goto _LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL991: return _temp994;
_LL993:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL989:;}
static unsigned char _temp997[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp997, _temp997,
_temp997 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp998= yy1; struct Cyc_List_List* _temp1004; _LL1000: if(*(( void**)
_temp998) == Cyc_TypeModifierList_tok){ _LL1005: _temp1004=(( struct Cyc_TypeModifierList_tok_struct*)
_temp998)->f1; goto _LL1001;} else{ goto _LL1002;} _LL1002: goto _LL1003;
_LL1001: return _temp1004; _LL1003:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL999:;} static unsigned char _temp1007[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp1007, _temp1007, _temp1007 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1008= yy1; void* _temp1014;
_LL1010: if(*(( void**) _temp1008) == Cyc_Rgn_tok){ _LL1015: _temp1014=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1008)->f1; goto _LL1011;} else{ goto _LL1012;}
_LL1012: goto _LL1013; _LL1011: return _temp1014; _LL1013:( int) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1009:;} static unsigned char _temp1017[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{
_temp1017, _temp1017, _temp1017 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp1018= yy1; struct Cyc_Parse_Declarator* _temp1024;
_LL1020: if(*(( void**) _temp1018) == Cyc_Declarator_tok){ _LL1025: _temp1024=((
struct Cyc_Declarator_tok_struct*) _temp1018)->f1; goto _LL1021;} else{ goto
_LL1022;} _LL1022: goto _LL1023; _LL1021: return _temp1024; _LL1023:( int)
_throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1019:;} static unsigned char
_temp1027[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1027, _temp1027,
_temp1027 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1028= yy1; struct _tuple13* _temp1034; _LL1030: if(*(( void**)
_temp1028) == Cyc_DeclaratorExpopt_tok){ _LL1035: _temp1034=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1028)->f1; goto _LL1031;} else{ goto _LL1032;} _LL1032: goto _LL1033;
_LL1031: return _temp1034; _LL1033:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1029:;} static unsigned char _temp1037[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1037, _temp1037, _temp1037 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1038= yy1; struct Cyc_List_List* _temp1044; _LL1040: if(*((
void**) _temp1038) == Cyc_DeclaratorExpoptList_tok){ _LL1045: _temp1044=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1038)->f1; goto _LL1041;}
else{ goto _LL1042;} _LL1042: goto _LL1043; _LL1041: return _temp1044; _LL1043:(
int) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1039:;} static
unsigned char _temp1047[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1047, _temp1047,
_temp1047 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1048= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1054;
_LL1050: if(*(( void**) _temp1048) == Cyc_AbstractDeclarator_tok){ _LL1055:
_temp1054=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1048)->f1; goto
_LL1051;} else{ goto _LL1052;} _LL1052: goto _LL1053; _LL1051: return _temp1054;
_LL1053:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1049:;}
static unsigned char _temp1057[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1057, _temp1057, _temp1057 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1058= yy1; int _temp1064; _LL1060:
if(*(( void**) _temp1058) == Cyc_Bool_tok){ _LL1065: _temp1064=(( struct Cyc_Bool_tok_struct*)
_temp1058)->f1; goto _LL1061;} else{ goto _LL1062;} _LL1062: goto _LL1063;
_LL1061: return _temp1064; _LL1063:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1059:;} static unsigned char _temp1067[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1067, _temp1067, _temp1067 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1068= yy1; void* _temp1074;
_LL1070: if(*(( void**) _temp1068) == Cyc_Scope_tok){ _LL1075: _temp1074=( void*)((
struct Cyc_Scope_tok_struct*) _temp1068)->f1; goto _LL1071;} else{ goto _LL1072;}
_LL1072: goto _LL1073; _LL1071: return _temp1074; _LL1073:( int) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1069:;} static unsigned char _temp1077[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1077, _temp1077, _temp1077 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1078= yy1; struct Cyc_Absyn_Tunionfield* _temp1084;
_LL1080: if(*(( void**) _temp1078) == Cyc_TunionField_tok){ _LL1085: _temp1084=((
struct Cyc_TunionField_tok_struct*) _temp1078)->f1; goto _LL1081;} else{ goto
_LL1082;} _LL1082: goto _LL1083; _LL1081: return _temp1084; _LL1083:( int)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1079:;} static unsigned char
_temp1087[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1087, _temp1087, _temp1087 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1088= yy1; struct Cyc_List_List*
_temp1094; _LL1090: if(*(( void**) _temp1088) == Cyc_TunionFieldList_tok){
_LL1095: _temp1094=(( struct Cyc_TunionFieldList_tok_struct*) _temp1088)->f1;
goto _LL1091;} else{ goto _LL1092;} _LL1092: goto _LL1093; _LL1091: return
_temp1094; _LL1093:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1089:;} static unsigned char _temp1097[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1097, _temp1097, _temp1097 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1098= yy1; struct _tuple18* _temp1104; _LL1100: if(*((
void**) _temp1098) == Cyc_QualSpecList_tok){ _LL1105: _temp1104=(( struct Cyc_QualSpecList_tok_struct*)
_temp1098)->f1; goto _LL1101;} else{ goto _LL1102;} _LL1102: goto _LL1103;
_LL1101: return _temp1104; _LL1103:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1099:;} static unsigned char _temp1107[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1107, _temp1107, _temp1107 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1108= yy1;
struct Cyc_List_List* _temp1114; _LL1110: if(*(( void**) _temp1108) == Cyc_IdList_tok){
_LL1115: _temp1114=(( struct Cyc_IdList_tok_struct*) _temp1108)->f1; goto
_LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113; _LL1111: return _temp1114;
_LL1113:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1109:;} static
unsigned char _temp1117[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1117, _temp1117, _temp1117 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1118= yy1;
struct _tuple2* _temp1124; _LL1120: if(*(( void**) _temp1118) == Cyc_ParamDecl_tok){
_LL1125: _temp1124=(( struct Cyc_ParamDecl_tok_struct*) _temp1118)->f1; goto
_LL1121;} else{ goto _LL1122;} _LL1122: goto _LL1123; _LL1121: return _temp1124;
_LL1123:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1119:;} static
unsigned char _temp1127[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1127, _temp1127,
_temp1127 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1128= yy1; struct Cyc_List_List* _temp1134; _LL1130: if(*(( void**)
_temp1128) == Cyc_ParamDeclList_tok){ _LL1135: _temp1134=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1128)->f1; goto _LL1131;} else{ goto _LL1132;} _LL1132: goto _LL1133;
_LL1131: return _temp1134; _LL1133:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1129:;} static unsigned char _temp1137[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1137, _temp1137, _temp1137 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1138= yy1; struct _tuple17* _temp1144; _LL1140: if(*((
void**) _temp1138) == Cyc_ParamDeclListBool_tok){ _LL1145: _temp1144=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1138)->f1; goto _LL1141;} else{ goto
_LL1142;} _LL1142: goto _LL1143; _LL1141: return _temp1144; _LL1143:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1139:;} static
unsigned char _temp1147[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1147, _temp1147, _temp1147 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1148=
yy1; struct Cyc_List_List* _temp1154; _LL1150: if(*(( void**) _temp1148) == Cyc_TypeList_tok){
_LL1155: _temp1154=(( struct Cyc_TypeList_tok_struct*) _temp1148)->f1; goto
_LL1151;} else{ goto _LL1152;} _LL1152: goto _LL1153; _LL1151: return _temp1154;
_LL1153:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1149:;} static
unsigned char _temp1157[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1157, _temp1157,
_temp1157 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1158= yy1; struct Cyc_List_List* _temp1164; _LL1160: if(*(( void**)
_temp1158) == Cyc_DesignatorList_tok){ _LL1165: _temp1164=(( struct Cyc_DesignatorList_tok_struct*)
_temp1158)->f1; goto _LL1161;} else{ goto _LL1162;} _LL1162: goto _LL1163;
_LL1161: return _temp1164; _LL1163:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1159:;} static unsigned char _temp1167[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1167,
_temp1167, _temp1167 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1168= yy1; void* _temp1174; _LL1170: if(*(( void**) _temp1168) == Cyc_Designator_tok){
_LL1175: _temp1174=( void*)(( struct Cyc_Designator_tok_struct*) _temp1168)->f1;
goto _LL1171;} else{ goto _LL1172;} _LL1172: goto _LL1173; _LL1171: return
_temp1174; _LL1173:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1169:;}
static unsigned char _temp1177[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1177, _temp1177, _temp1177 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1178= yy1; void* _temp1184;
_LL1180: if(*(( void**) _temp1178) == Cyc_Kind_tok){ _LL1185: _temp1184=( void*)((
struct Cyc_Kind_tok_struct*) _temp1178)->f1; goto _LL1181;} else{ goto _LL1182;}
_LL1182: goto _LL1183; _LL1181: return _temp1184; _LL1183:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1179:;} static unsigned char _temp1187[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1187, _temp1187, _temp1187 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1188= yy1; void* _temp1194; _LL1190: if(*(( void**) _temp1188) == Cyc_Type_tok){
_LL1195: _temp1194=( void*)(( struct Cyc_Type_tok_struct*) _temp1188)->f1; goto
_LL1191;} else{ goto _LL1192;} _LL1192: goto _LL1193; _LL1191: return _temp1194;
_LL1193:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1189:;} static
unsigned char _temp1197[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1197, _temp1197,
_temp1197 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1198= yy1; struct Cyc_List_List* _temp1204; _LL1200: if(*(( void**)
_temp1198) == Cyc_AttributeList_tok){ _LL1205: _temp1204=(( struct Cyc_AttributeList_tok_struct*)
_temp1198)->f1; goto _LL1201;} else{ goto _LL1202;} _LL1202: goto _LL1203;
_LL1201: return _temp1204; _LL1203:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1199:;} static unsigned char _temp1207[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1207,
_temp1207, _temp1207 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1208= yy1; void* _temp1214; _LL1210: if(*(( void**) _temp1208) == Cyc_Attribute_tok){
_LL1215: _temp1214=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1208)->f1;
goto _LL1211;} else{ goto _LL1212;} _LL1212: goto _LL1213; _LL1211: return
_temp1214; _LL1213:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1209:;}
static unsigned char _temp1217[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1217, _temp1217, _temp1217 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1218= yy1; struct Cyc_Absyn_Enumfield* _temp1224; _LL1220: if(*(( void**)
_temp1218) == Cyc_Enumfield_tok){ _LL1225: _temp1224=(( struct Cyc_Enumfield_tok_struct*)
_temp1218)->f1; goto _LL1221;} else{ goto _LL1222;} _LL1222: goto _LL1223;
_LL1221: return _temp1224; _LL1223:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1219:;} static unsigned char _temp1227[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1227, _temp1227, _temp1227 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1228= yy1; struct Cyc_List_List* _temp1234; _LL1230: if(*((
void**) _temp1228) == Cyc_EnumfieldList_tok){ _LL1235: _temp1234=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1228)->f1; goto _LL1231;} else{ goto _LL1232;} _LL1232: goto _LL1233;
_LL1231: return _temp1234; _LL1233:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1229:;} static unsigned char _temp1237[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1237, _temp1237, _temp1237 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1238= yy1;
struct Cyc_Core_Opt* _temp1244; _LL1240: if(*(( void**) _temp1238) == Cyc_TypeOpt_tok){
_LL1245: _temp1244=(( struct Cyc_TypeOpt_tok_struct*) _temp1238)->f1; goto
_LL1241;} else{ goto _LL1242;} _LL1242: goto _LL1243; _LL1241: return _temp1244;
_LL1243:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1239:;} static
unsigned char _temp1247[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1247, _temp1247, _temp1247 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1248=
yy1; struct Cyc_List_List* _temp1254; _LL1250: if(*(( void**) _temp1248) == Cyc_Rgnorder_tok){
_LL1255: _temp1254=(( struct Cyc_Rgnorder_tok_struct*) _temp1248)->f1; goto
_LL1251;} else{ goto _LL1252;} _LL1252: goto _LL1253; _LL1251: return _temp1254;
_LL1253:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1249:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1256;
_temp1256.timestamp= 0; _temp1256.first_line= 0; _temp1256.first_column= 0;
_temp1256.last_line= 0; _temp1256.last_column= 0; _temp1256;});} struct Cyc_Yyltype
Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[ 353u]={ 0, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 124, 2, 2, 106, 122, 118, 2, 102, 103, 113, 116, 104, 117, 110, 121, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 109, 98, 107, 101, 108, 115, 114, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 111, 2, 112, 120, 105, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 99,
119, 100, 123, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97}; static unsigned char
_temp1257[ 2u]="$"; static unsigned char _temp1258[ 6u]="error"; static
unsigned char _temp1259[ 12u]="$undefined."; static unsigned char _temp1260[ 5u]="AUTO";
static unsigned char _temp1261[ 9u]="REGISTER"; static unsigned char _temp1262[
7u]="STATIC"; static unsigned char _temp1263[ 7u]="EXTERN"; static unsigned char
_temp1264[ 8u]="TYPEDEF"; static unsigned char _temp1265[ 5u]="VOID"; static
unsigned char _temp1266[ 5u]="CHAR"; static unsigned char _temp1267[ 6u]="SHORT";
static unsigned char _temp1268[ 4u]="INT"; static unsigned char _temp1269[ 5u]="LONG";
static unsigned char _temp1270[ 6u]="FLOAT"; static unsigned char _temp1271[ 7u]="DOUBLE";
static unsigned char _temp1272[ 7u]="SIGNED"; static unsigned char _temp1273[ 9u]="UNSIGNED";
static unsigned char _temp1274[ 6u]="CONST"; static unsigned char _temp1275[ 9u]="VOLATILE";
static unsigned char _temp1276[ 9u]="RESTRICT"; static unsigned char _temp1277[
7u]="STRUCT"; static unsigned char _temp1278[ 6u]="UNION"; static unsigned char
_temp1279[ 5u]="CASE"; static unsigned char _temp1280[ 8u]="DEFAULT"; static
unsigned char _temp1281[ 7u]="INLINE"; static unsigned char _temp1282[ 7u]="SIZEOF";
static unsigned char _temp1283[ 9u]="OFFSETOF"; static unsigned char _temp1284[
3u]="IF"; static unsigned char _temp1285[ 5u]="ELSE"; static unsigned char
_temp1286[ 7u]="SWITCH"; static unsigned char _temp1287[ 6u]="WHILE"; static
unsigned char _temp1288[ 3u]="DO"; static unsigned char _temp1289[ 4u]="FOR";
static unsigned char _temp1290[ 5u]="GOTO"; static unsigned char _temp1291[ 9u]="CONTINUE";
static unsigned char _temp1292[ 6u]="BREAK"; static unsigned char _temp1293[ 7u]="RETURN";
static unsigned char _temp1294[ 5u]="ENUM"; static unsigned char _temp1295[ 8u]="NULL_kw";
static unsigned char _temp1296[ 4u]="LET"; static unsigned char _temp1297[ 6u]="THROW";
static unsigned char _temp1298[ 4u]="TRY"; static unsigned char _temp1299[ 6u]="CATCH";
static unsigned char _temp1300[ 4u]="NEW"; static unsigned char _temp1301[ 9u]="ABSTRACT";
static unsigned char _temp1302[ 9u]="FALLTHRU"; static unsigned char _temp1303[
6u]="USING"; static unsigned char _temp1304[ 10u]="NAMESPACE"; static
unsigned char _temp1305[ 7u]="TUNION"; static unsigned char _temp1306[ 8u]="XTUNION";
static unsigned char _temp1307[ 5u]="FILL"; static unsigned char _temp1308[ 8u]="CODEGEN";
static unsigned char _temp1309[ 4u]="CUT"; static unsigned char _temp1310[ 7u]="SPLICE";
static unsigned char _temp1311[ 7u]="PRINTF"; static unsigned char _temp1312[ 8u]="FPRINTF";
static unsigned char _temp1313[ 8u]="XPRINTF"; static unsigned char _temp1314[ 6u]="SCANF";
static unsigned char _temp1315[ 7u]="FSCANF"; static unsigned char _temp1316[ 7u]="SSCANF";
static unsigned char _temp1317[ 7u]="MALLOC"; static unsigned char _temp1318[ 9u]="REGION_T";
static unsigned char _temp1319[ 7u]="REGION"; static unsigned char _temp1320[ 5u]="RNEW";
static unsigned char _temp1321[ 8u]="RMALLOC"; static unsigned char _temp1322[ 7u]="PTR_OP";
static unsigned char _temp1323[ 7u]="INC_OP"; static unsigned char _temp1324[ 7u]="DEC_OP";
static unsigned char _temp1325[ 8u]="LEFT_OP"; static unsigned char _temp1326[ 9u]="RIGHT_OP";
static unsigned char _temp1327[ 6u]="LE_OP"; static unsigned char _temp1328[ 6u]="GE_OP";
static unsigned char _temp1329[ 6u]="EQ_OP"; static unsigned char _temp1330[ 6u]="NE_OP";
static unsigned char _temp1331[ 7u]="AND_OP"; static unsigned char _temp1332[ 6u]="OR_OP";
static unsigned char _temp1333[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1334[ 11u]="DIV_ASSIGN"; static unsigned char _temp1335[ 11u]="MOD_ASSIGN";
static unsigned char _temp1336[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1337[ 11u]="SUB_ASSIGN"; static unsigned char _temp1338[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1339[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1340[ 11u]="AND_ASSIGN"; static unsigned char _temp1341[ 11u]="XOR_ASSIGN";
static unsigned char _temp1342[ 10u]="OR_ASSIGN"; static unsigned char _temp1343[
9u]="ELLIPSIS"; static unsigned char _temp1344[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1345[ 12u]="COLON_COLON"; static unsigned char _temp1346[ 11u]="IDENTIFIER";
static unsigned char _temp1347[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1348[ 7u]="STRING"; static unsigned char _temp1349[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1350[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1351[ 9u]="TYPE_VAR"; static unsigned char _temp1352[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1353[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1354[ 10u]="ATTRIBUTE"; static unsigned char _temp1355[ 4u]="';'"; static
unsigned char _temp1356[ 4u]="'{'"; static unsigned char _temp1357[ 4u]="'}'";
static unsigned char _temp1358[ 4u]="'='"; static unsigned char _temp1359[ 4u]="'('";
static unsigned char _temp1360[ 4u]="')'"; static unsigned char _temp1361[ 4u]="','";
static unsigned char _temp1362[ 4u]="'_'"; static unsigned char _temp1363[ 4u]="'$'";
static unsigned char _temp1364[ 4u]="'<'"; static unsigned char _temp1365[ 4u]="'>'";
static unsigned char _temp1366[ 4u]="':'"; static unsigned char _temp1367[ 4u]="'.'";
static unsigned char _temp1368[ 4u]="'['"; static unsigned char _temp1369[ 4u]="']'";
static unsigned char _temp1370[ 4u]="'*'"; static unsigned char _temp1371[ 4u]="'@'";
static unsigned char _temp1372[ 4u]="'?'"; static unsigned char _temp1373[ 4u]="'+'";
static unsigned char _temp1374[ 4u]="'-'"; static unsigned char _temp1375[ 4u]="'&'";
static unsigned char _temp1376[ 4u]="'|'"; static unsigned char _temp1377[ 4u]="'^'";
static unsigned char _temp1378[ 4u]="'/'"; static unsigned char _temp1379[ 4u]="'%'";
static unsigned char _temp1380[ 4u]="'~'"; static unsigned char _temp1381[ 4u]="'!'";
static unsigned char _temp1382[ 5u]="prog"; static unsigned char _temp1383[ 17u]="translation_unit";
static unsigned char _temp1384[ 21u]="external_declaration"; static
unsigned char _temp1385[ 20u]="function_definition"; static unsigned char
_temp1386[ 21u]="function_definition2"; static unsigned char _temp1387[ 13u]="using_action";
static unsigned char _temp1388[ 15u]="unusing_action"; static unsigned char
_temp1389[ 17u]="namespace_action"; static unsigned char _temp1390[ 19u]="unnamespace_action";
static unsigned char _temp1391[ 12u]="declaration"; static unsigned char
_temp1392[ 17u]="declaration_list"; static unsigned char _temp1393[ 23u]="declaration_specifiers";
static unsigned char _temp1394[ 24u]="storage_class_specifier"; static
unsigned char _temp1395[ 15u]="attributes_opt"; static unsigned char _temp1396[
11u]="attributes"; static unsigned char _temp1397[ 15u]="attribute_list"; static
unsigned char _temp1398[ 10u]="attribute"; static unsigned char _temp1399[ 15u]="type_specifier";
static unsigned char _temp1400[ 5u]="kind"; static unsigned char _temp1401[ 15u]="type_qualifier";
static unsigned char _temp1402[ 15u]="enum_specifier"; static unsigned char
_temp1403[ 11u]="enum_field"; static unsigned char _temp1404[ 22u]="enum_declaration_list";
static unsigned char _temp1405[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1406[ 16u]="type_params_opt"; static unsigned char _temp1407[
16u]="struct_or_union"; static unsigned char _temp1408[ 24u]="struct_declaration_list";
static unsigned char _temp1409[ 25u]="struct_declaration_list0"; static
unsigned char _temp1410[ 21u]="init_declarator_list"; static unsigned char
_temp1411[ 22u]="init_declarator_list0"; static unsigned char _temp1412[ 16u]="init_declarator";
static unsigned char _temp1413[ 19u]="struct_declaration"; static unsigned char
_temp1414[ 25u]="specifier_qualifier_list"; static unsigned char _temp1415[ 23u]="struct_declarator_list";
static unsigned char _temp1416[ 24u]="struct_declarator_list0"; static
unsigned char _temp1417[ 18u]="struct_declarator"; static unsigned char
_temp1418[ 17u]="tunion_specifier"; static unsigned char _temp1419[ 18u]="tunion_or_xtunion";
static unsigned char _temp1420[ 17u]="tunionfield_list"; static unsigned char
_temp1421[ 18u]="tunionfield_scope"; static unsigned char _temp1422[ 12u]="tunionfield";
static unsigned char _temp1423[ 11u]="declarator"; static unsigned char
_temp1424[ 18u]="direct_declarator"; static unsigned char _temp1425[ 8u]="pointer";
static unsigned char _temp1426[ 13u]="pointer_char"; static unsigned char
_temp1427[ 8u]="rgn_opt"; static unsigned char _temp1428[ 4u]="rgn"; static
unsigned char _temp1429[ 20u]="type_qualifier_list"; static unsigned char
_temp1430[ 20u]="parameter_type_list"; static unsigned char _temp1431[ 16u]="optional_effect";
static unsigned char _temp1432[ 19u]="optional_rgn_order"; static unsigned char
_temp1433[ 10u]="rgn_order"; static unsigned char _temp1434[ 16u]="optional_inject";
static unsigned char _temp1435[ 11u]="effect_set"; static unsigned char
_temp1436[ 14u]="atomic_effect"; static unsigned char _temp1437[ 11u]="region_set";
static unsigned char _temp1438[ 15u]="parameter_list"; static unsigned char
_temp1439[ 22u]="parameter_declaration"; static unsigned char _temp1440[ 16u]="identifier_list";
static unsigned char _temp1441[ 17u]="identifier_list0"; static unsigned char
_temp1442[ 12u]="initializer"; static unsigned char _temp1443[ 18u]="array_initializer";
static unsigned char _temp1444[ 17u]="initializer_list"; static unsigned char
_temp1445[ 12u]="designation"; static unsigned char _temp1446[ 16u]="designator_list";
static unsigned char _temp1447[ 11u]="designator"; static unsigned char
_temp1448[ 10u]="type_name"; static unsigned char _temp1449[ 14u]="any_type_name";
static unsigned char _temp1450[ 15u]="type_name_list"; static unsigned char
_temp1451[ 20u]="abstract_declarator"; static unsigned char _temp1452[ 27u]="direct_abstract_declarator";
static unsigned char _temp1453[ 10u]="statement"; static unsigned char _temp1454[
18u]="labeled_statement"; static unsigned char _temp1455[ 21u]="expression_statement";
static unsigned char _temp1456[ 19u]="compound_statement"; static unsigned char
_temp1457[ 16u]="block_item_list"; static unsigned char _temp1458[ 11u]="block_item";
static unsigned char _temp1459[ 20u]="selection_statement"; static unsigned char
_temp1460[ 15u]="switch_clauses"; static unsigned char _temp1461[ 16u]="switchC_clauses";
static unsigned char _temp1462[ 20u]="iteration_statement"; static unsigned char
_temp1463[ 15u]="jump_statement"; static unsigned char _temp1464[ 8u]="pattern";
static unsigned char _temp1465[ 19u]="tuple_pattern_list"; static unsigned char
_temp1466[ 20u]="tuple_pattern_list0"; static unsigned char _temp1467[ 14u]="field_pattern";
static unsigned char _temp1468[ 19u]="field_pattern_list"; static unsigned char
_temp1469[ 20u]="field_pattern_list0"; static unsigned char _temp1470[ 11u]="expression";
static unsigned char _temp1471[ 22u]="assignment_expression"; static
unsigned char _temp1472[ 20u]="assignment_operator"; static unsigned char
_temp1473[ 23u]="conditional_expression"; static unsigned char _temp1474[ 20u]="constant_expression";
static unsigned char _temp1475[ 22u]="logical_or_expression"; static
unsigned char _temp1476[ 23u]="logical_and_expression"; static unsigned char
_temp1477[ 24u]="inclusive_or_expression"; static unsigned char _temp1478[ 24u]="exclusive_or_expression";
static unsigned char _temp1479[ 15u]="and_expression"; static unsigned char
_temp1480[ 20u]="equality_expression"; static unsigned char _temp1481[ 22u]="relational_expression";
static unsigned char _temp1482[ 17u]="shift_expression"; static unsigned char
_temp1483[ 20u]="additive_expression"; static unsigned char _temp1484[ 26u]="multiplicative_expression";
static unsigned char _temp1485[ 16u]="cast_expression"; static unsigned char
_temp1486[ 17u]="unary_expression"; static unsigned char _temp1487[ 14u]="format_primop";
static unsigned char _temp1488[ 15u]="unary_operator"; static unsigned char
_temp1489[ 19u]="postfix_expression"; static unsigned char _temp1490[ 19u]="primary_expression";
static unsigned char _temp1491[ 25u]="argument_expression_list"; static
unsigned char _temp1492[ 26u]="argument_expression_list0"; static unsigned char
_temp1493[ 9u]="constant"; static unsigned char _temp1494[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 238u]={{ _temp1257, _temp1257, _temp1257
+ 2u},{ _temp1258, _temp1258, _temp1258 + 6u},{ _temp1259, _temp1259, _temp1259
+ 12u},{ _temp1260, _temp1260, _temp1260 + 5u},{ _temp1261, _temp1261, _temp1261
+ 9u},{ _temp1262, _temp1262, _temp1262 + 7u},{ _temp1263, _temp1263, _temp1263
+ 7u},{ _temp1264, _temp1264, _temp1264 + 8u},{ _temp1265, _temp1265, _temp1265
+ 5u},{ _temp1266, _temp1266, _temp1266 + 5u},{ _temp1267, _temp1267, _temp1267
+ 6u},{ _temp1268, _temp1268, _temp1268 + 4u},{ _temp1269, _temp1269, _temp1269
+ 5u},{ _temp1270, _temp1270, _temp1270 + 6u},{ _temp1271, _temp1271, _temp1271
+ 7u},{ _temp1272, _temp1272, _temp1272 + 7u},{ _temp1273, _temp1273, _temp1273
+ 9u},{ _temp1274, _temp1274, _temp1274 + 6u},{ _temp1275, _temp1275, _temp1275
+ 9u},{ _temp1276, _temp1276, _temp1276 + 9u},{ _temp1277, _temp1277, _temp1277
+ 7u},{ _temp1278, _temp1278, _temp1278 + 6u},{ _temp1279, _temp1279, _temp1279
+ 5u},{ _temp1280, _temp1280, _temp1280 + 8u},{ _temp1281, _temp1281, _temp1281
+ 7u},{ _temp1282, _temp1282, _temp1282 + 7u},{ _temp1283, _temp1283, _temp1283
+ 9u},{ _temp1284, _temp1284, _temp1284 + 3u},{ _temp1285, _temp1285, _temp1285
+ 5u},{ _temp1286, _temp1286, _temp1286 + 7u},{ _temp1287, _temp1287, _temp1287
+ 6u},{ _temp1288, _temp1288, _temp1288 + 3u},{ _temp1289, _temp1289, _temp1289
+ 4u},{ _temp1290, _temp1290, _temp1290 + 5u},{ _temp1291, _temp1291, _temp1291
+ 9u},{ _temp1292, _temp1292, _temp1292 + 6u},{ _temp1293, _temp1293, _temp1293
+ 7u},{ _temp1294, _temp1294, _temp1294 + 5u},{ _temp1295, _temp1295, _temp1295
+ 8u},{ _temp1296, _temp1296, _temp1296 + 4u},{ _temp1297, _temp1297, _temp1297
+ 6u},{ _temp1298, _temp1298, _temp1298 + 4u},{ _temp1299, _temp1299, _temp1299
+ 6u},{ _temp1300, _temp1300, _temp1300 + 4u},{ _temp1301, _temp1301, _temp1301
+ 9u},{ _temp1302, _temp1302, _temp1302 + 9u},{ _temp1303, _temp1303, _temp1303
+ 6u},{ _temp1304, _temp1304, _temp1304 + 10u},{ _temp1305, _temp1305, _temp1305
+ 7u},{ _temp1306, _temp1306, _temp1306 + 8u},{ _temp1307, _temp1307, _temp1307
+ 5u},{ _temp1308, _temp1308, _temp1308 + 8u},{ _temp1309, _temp1309, _temp1309
+ 4u},{ _temp1310, _temp1310, _temp1310 + 7u},{ _temp1311, _temp1311, _temp1311
+ 7u},{ _temp1312, _temp1312, _temp1312 + 8u},{ _temp1313, _temp1313, _temp1313
+ 8u},{ _temp1314, _temp1314, _temp1314 + 6u},{ _temp1315, _temp1315, _temp1315
+ 7u},{ _temp1316, _temp1316, _temp1316 + 7u},{ _temp1317, _temp1317, _temp1317
+ 7u},{ _temp1318, _temp1318, _temp1318 + 9u},{ _temp1319, _temp1319, _temp1319
+ 7u},{ _temp1320, _temp1320, _temp1320 + 5u},{ _temp1321, _temp1321, _temp1321
+ 8u},{ _temp1322, _temp1322, _temp1322 + 7u},{ _temp1323, _temp1323, _temp1323
+ 7u},{ _temp1324, _temp1324, _temp1324 + 7u},{ _temp1325, _temp1325, _temp1325
+ 8u},{ _temp1326, _temp1326, _temp1326 + 9u},{ _temp1327, _temp1327, _temp1327
+ 6u},{ _temp1328, _temp1328, _temp1328 + 6u},{ _temp1329, _temp1329, _temp1329
+ 6u},{ _temp1330, _temp1330, _temp1330 + 6u},{ _temp1331, _temp1331, _temp1331
+ 7u},{ _temp1332, _temp1332, _temp1332 + 6u},{ _temp1333, _temp1333, _temp1333
+ 11u},{ _temp1334, _temp1334, _temp1334 + 11u},{ _temp1335, _temp1335,
_temp1335 + 11u},{ _temp1336, _temp1336, _temp1336 + 11u},{ _temp1337, _temp1337,
_temp1337 + 11u},{ _temp1338, _temp1338, _temp1338 + 12u},{ _temp1339, _temp1339,
_temp1339 + 13u},{ _temp1340, _temp1340, _temp1340 + 11u},{ _temp1341, _temp1341,
_temp1341 + 11u},{ _temp1342, _temp1342, _temp1342 + 10u},{ _temp1343, _temp1343,
_temp1343 + 9u},{ _temp1344, _temp1344, _temp1344 + 11u},{ _temp1345, _temp1345,
_temp1345 + 12u},{ _temp1346, _temp1346, _temp1346 + 11u},{ _temp1347, _temp1347,
_temp1347 + 17u},{ _temp1348, _temp1348, _temp1348 + 7u},{ _temp1349, _temp1349,
_temp1349 + 19u},{ _temp1350, _temp1350, _temp1350 + 18u},{ _temp1351, _temp1351,
_temp1351 + 9u},{ _temp1352, _temp1352, _temp1352 + 16u},{ _temp1353, _temp1353,
_temp1353 + 18u},{ _temp1354, _temp1354, _temp1354 + 10u},{ _temp1355, _temp1355,
_temp1355 + 4u},{ _temp1356, _temp1356, _temp1356 + 4u},{ _temp1357, _temp1357,
_temp1357 + 4u},{ _temp1358, _temp1358, _temp1358 + 4u},{ _temp1359, _temp1359,
_temp1359 + 4u},{ _temp1360, _temp1360, _temp1360 + 4u},{ _temp1361, _temp1361,
_temp1361 + 4u},{ _temp1362, _temp1362, _temp1362 + 4u},{ _temp1363, _temp1363,
_temp1363 + 4u},{ _temp1364, _temp1364, _temp1364 + 4u},{ _temp1365, _temp1365,
_temp1365 + 4u},{ _temp1366, _temp1366, _temp1366 + 4u},{ _temp1367, _temp1367,
_temp1367 + 4u},{ _temp1368, _temp1368, _temp1368 + 4u},{ _temp1369, _temp1369,
_temp1369 + 4u},{ _temp1370, _temp1370, _temp1370 + 4u},{ _temp1371, _temp1371,
_temp1371 + 4u},{ _temp1372, _temp1372, _temp1372 + 4u},{ _temp1373, _temp1373,
_temp1373 + 4u},{ _temp1374, _temp1374, _temp1374 + 4u},{ _temp1375, _temp1375,
_temp1375 + 4u},{ _temp1376, _temp1376, _temp1376 + 4u},{ _temp1377, _temp1377,
_temp1377 + 4u},{ _temp1378, _temp1378, _temp1378 + 4u},{ _temp1379, _temp1379,
_temp1379 + 4u},{ _temp1380, _temp1380, _temp1380 + 4u},{ _temp1381, _temp1381,
_temp1381 + 4u},{ _temp1382, _temp1382, _temp1382 + 5u},{ _temp1383, _temp1383,
_temp1383 + 17u},{ _temp1384, _temp1384, _temp1384 + 21u},{ _temp1385, _temp1385,
_temp1385 + 20u},{ _temp1386, _temp1386, _temp1386 + 21u},{ _temp1387, _temp1387,
_temp1387 + 13u},{ _temp1388, _temp1388, _temp1388 + 15u},{ _temp1389, _temp1389,
_temp1389 + 17u},{ _temp1390, _temp1390, _temp1390 + 19u},{ _temp1391, _temp1391,
_temp1391 + 12u},{ _temp1392, _temp1392, _temp1392 + 17u},{ _temp1393, _temp1393,
_temp1393 + 23u},{ _temp1394, _temp1394, _temp1394 + 24u},{ _temp1395, _temp1395,
_temp1395 + 15u},{ _temp1396, _temp1396, _temp1396 + 11u},{ _temp1397, _temp1397,
_temp1397 + 15u},{ _temp1398, _temp1398, _temp1398 + 10u},{ _temp1399, _temp1399,
_temp1399 + 15u},{ _temp1400, _temp1400, _temp1400 + 5u},{ _temp1401, _temp1401,
_temp1401 + 15u},{ _temp1402, _temp1402, _temp1402 + 15u},{ _temp1403, _temp1403,
_temp1403 + 11u},{ _temp1404, _temp1404, _temp1404 + 22u},{ _temp1405, _temp1405,
_temp1405 + 26u},{ _temp1406, _temp1406, _temp1406 + 16u},{ _temp1407, _temp1407,
_temp1407 + 16u},{ _temp1408, _temp1408, _temp1408 + 24u},{ _temp1409, _temp1409,
_temp1409 + 25u},{ _temp1410, _temp1410, _temp1410 + 21u},{ _temp1411, _temp1411,
_temp1411 + 22u},{ _temp1412, _temp1412, _temp1412 + 16u},{ _temp1413, _temp1413,
_temp1413 + 19u},{ _temp1414, _temp1414, _temp1414 + 25u},{ _temp1415, _temp1415,
_temp1415 + 23u},{ _temp1416, _temp1416, _temp1416 + 24u},{ _temp1417, _temp1417,
_temp1417 + 18u},{ _temp1418, _temp1418, _temp1418 + 17u},{ _temp1419, _temp1419,
_temp1419 + 18u},{ _temp1420, _temp1420, _temp1420 + 17u},{ _temp1421, _temp1421,
_temp1421 + 18u},{ _temp1422, _temp1422, _temp1422 + 12u},{ _temp1423, _temp1423,
_temp1423 + 11u},{ _temp1424, _temp1424, _temp1424 + 18u},{ _temp1425, _temp1425,
_temp1425 + 8u},{ _temp1426, _temp1426, _temp1426 + 13u},{ _temp1427, _temp1427,
_temp1427 + 8u},{ _temp1428, _temp1428, _temp1428 + 4u},{ _temp1429, _temp1429,
_temp1429 + 20u},{ _temp1430, _temp1430, _temp1430 + 20u},{ _temp1431, _temp1431,
_temp1431 + 16u},{ _temp1432, _temp1432, _temp1432 + 19u},{ _temp1433, _temp1433,
_temp1433 + 10u},{ _temp1434, _temp1434, _temp1434 + 16u},{ _temp1435, _temp1435,
_temp1435 + 11u},{ _temp1436, _temp1436, _temp1436 + 14u},{ _temp1437, _temp1437,
_temp1437 + 11u},{ _temp1438, _temp1438, _temp1438 + 15u},{ _temp1439, _temp1439,
_temp1439 + 22u},{ _temp1440, _temp1440, _temp1440 + 16u},{ _temp1441, _temp1441,
_temp1441 + 17u},{ _temp1442, _temp1442, _temp1442 + 12u},{ _temp1443, _temp1443,
_temp1443 + 18u},{ _temp1444, _temp1444, _temp1444 + 17u},{ _temp1445, _temp1445,
_temp1445 + 12u},{ _temp1446, _temp1446, _temp1446 + 16u},{ _temp1447, _temp1447,
_temp1447 + 11u},{ _temp1448, _temp1448, _temp1448 + 10u},{ _temp1449, _temp1449,
_temp1449 + 14u},{ _temp1450, _temp1450, _temp1450 + 15u},{ _temp1451, _temp1451,
_temp1451 + 20u},{ _temp1452, _temp1452, _temp1452 + 27u},{ _temp1453, _temp1453,
_temp1453 + 10u},{ _temp1454, _temp1454, _temp1454 + 18u},{ _temp1455, _temp1455,
_temp1455 + 21u},{ _temp1456, _temp1456, _temp1456 + 19u},{ _temp1457, _temp1457,
_temp1457 + 16u},{ _temp1458, _temp1458, _temp1458 + 11u},{ _temp1459, _temp1459,
_temp1459 + 20u},{ _temp1460, _temp1460, _temp1460 + 15u},{ _temp1461, _temp1461,
_temp1461 + 16u},{ _temp1462, _temp1462, _temp1462 + 20u},{ _temp1463, _temp1463,
_temp1463 + 15u},{ _temp1464, _temp1464, _temp1464 + 8u},{ _temp1465, _temp1465,
_temp1465 + 19u},{ _temp1466, _temp1466, _temp1466 + 20u},{ _temp1467, _temp1467,
_temp1467 + 14u},{ _temp1468, _temp1468, _temp1468 + 19u},{ _temp1469, _temp1469,
_temp1469 + 20u},{ _temp1470, _temp1470, _temp1470 + 11u},{ _temp1471, _temp1471,
_temp1471 + 22u},{ _temp1472, _temp1472, _temp1472 + 20u},{ _temp1473, _temp1473,
_temp1473 + 23u},{ _temp1474, _temp1474, _temp1474 + 20u},{ _temp1475, _temp1475,
_temp1475 + 22u},{ _temp1476, _temp1476, _temp1476 + 23u},{ _temp1477, _temp1477,
_temp1477 + 24u},{ _temp1478, _temp1478, _temp1478 + 24u},{ _temp1479, _temp1479,
_temp1479 + 15u},{ _temp1480, _temp1480, _temp1480 + 20u},{ _temp1481, _temp1481,
_temp1481 + 22u},{ _temp1482, _temp1482, _temp1482 + 17u},{ _temp1483, _temp1483,
_temp1483 + 20u},{ _temp1484, _temp1484, _temp1484 + 26u},{ _temp1485, _temp1485,
_temp1485 + 16u},{ _temp1486, _temp1486, _temp1486 + 17u},{ _temp1487, _temp1487,
_temp1487 + 14u},{ _temp1488, _temp1488, _temp1488 + 15u},{ _temp1489, _temp1489,
_temp1489 + 19u},{ _temp1490, _temp1490, _temp1490 + 19u},{ _temp1491, _temp1491,
_temp1491 + 25u},{ _temp1492, _temp1492, _temp1492 + 26u},{ _temp1493, _temp1493,
_temp1493 + 9u},{ _temp1494, _temp1494, _temp1494 + 20u}}; static short Cyc_yyr1[
409u]={ 0, 125, 126, 126, 126, 126, 126, 126, 126, 127, 127, 128, 128, 128, 128,
129, 129, 130, 131, 132, 133, 134, 134, 134, 134, 135, 135, 136, 136, 136, 136,
136, 136, 136, 136, 137, 137, 137, 137, 137, 137, 137, 138, 138, 139, 140, 140,
141, 141, 141, 141, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142,
142, 142, 142, 142, 142, 142, 143, 143, 144, 144, 144, 145, 145, 146, 146, 147,
147, 148, 148, 148, 148, 148, 149, 149, 149, 150, 150, 151, 152, 152, 153, 154,
154, 155, 155, 156, 157, 157, 157, 157, 158, 159, 159, 160, 160, 160, 161, 161,
161, 161, 162, 162, 163, 163, 163, 163, 164, 164, 164, 165, 165, 166, 166, 167,
167, 167, 167, 167, 167, 167, 167, 167, 167, 167, 167, 168, 168, 168, 168, 169,
169, 169, 169, 169, 170, 170, 171, 171, 171, 172, 172, 173, 173, 173, 173, 174,
174, 175, 175, 176, 176, 177, 177, 178, 178, 179, 179, 179, 179, 180, 180, 180,
180, 181, 181, 182, 182, 182, 183, 184, 184, 185, 185, 186, 186, 186, 186, 187,
187, 187, 187, 188, 189, 189, 190, 190, 191, 191, 192, 192, 192, 192, 193, 193,
194, 194, 194, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 196,
196, 196, 196, 196, 196, 196, 196, 196, 196, 197, 198, 198, 199, 199, 200, 200,
201, 201, 201, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 204, 204,
204, 204, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
206, 206, 206, 206, 206, 206, 206, 206, 207, 207, 207, 207, 207, 207, 207, 207,
207, 207, 207, 207, 207, 207, 208, 208, 209, 209, 210, 210, 211, 212, 212, 213,
213, 214, 214, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 216, 216,
216, 216, 216, 216, 216, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 222,
223, 223, 223, 224, 224, 224, 224, 224, 225, 225, 225, 226, 226, 226, 227, 227,
227, 227, 228, 228, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
229, 230, 230, 230, 230, 230, 230, 231, 231, 231, 232, 232, 232, 232, 232, 232,
232, 232, 232, 232, 232, 232, 232, 232, 233, 233, 233, 233, 233, 233, 233, 233,
233, 234, 235, 235, 236, 236, 236, 236, 237, 237}; static short Cyc_yyr2[ 409u]={
0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2,
2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 4, 4, 1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4,
6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2,
3, 6, 3, 4, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4,
2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 3, 1, 1, 2, 3, 5, 6, 8, 0, 2, 0, 2,
3, 5, 0, 1, 1, 3, 2, 3, 1, 3, 1, 3, 3, 5, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4,
8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4,
3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 1, 1, 5,
7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7,
8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3,
1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5,
1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3,
3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6, 4, 7, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1,
1, 1, 1, 1}; static short Cyc_yydefact[ 798u]={ 0, 137, 35, 36, 37, 38, 40, 51,
53, 54, 55, 56, 57, 58, 59, 60, 71, 72, 73, 88, 89, 42, 0, 0, 41, 0, 0, 113, 114,
0, 407, 64, 408, 85, 0, 52, 0, 142, 143, 146, 1, 0, 9, 0, 0, 10, 0, 42, 42, 42,
61, 62, 0, 63, 0, 0, 124, 0, 147, 126, 39, 0, 33, 43, 75, 285, 407, 281, 284,
283, 0, 279, 0, 0, 0, 0, 0, 181, 0, 286, 17, 19, 0, 0, 0, 66, 0, 0, 0, 0, 2, 0,
0, 0, 0, 21, 0, 93, 94, 96, 27, 29, 31, 85, 0, 85, 149, 151, 0, 85, 38, 0, 25, 0,
0, 11, 158, 0, 0, 135, 125, 42, 148, 136, 0, 0, 34, 0, 0, 293, 292, 282, 291, 24,
0, 0, 0, 0, 42, 42, 199, 201, 0, 69, 70, 65, 205, 0, 127, 0, 0, 176, 0, 0, 406,
0, 0, 0, 0, 368, 369, 370, 371, 372, 373, 0, 0, 0, 0, 0, 403, 393, 404, 405, 0,
0, 0, 0, 376, 0, 374, 375, 0, 304, 317, 325, 327, 329, 331, 333, 335, 338, 343,
346, 349, 353, 0, 0, 355, 377, 392, 391, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30,
32, 84, 0, 90, 91, 0, 83, 0, 85, 0, 110, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 407, 233, 235, 0, 241, 239, 0, 240, 222, 223, 224, 0, 237, 225, 226, 227,
0, 302, 26, 13, 96, 0, 182, 0, 0, 160, 158, 0, 0, 128, 0, 138, 0, 0, 78, 0, 76,
280, 295, 0, 294, 183, 0, 0, 293, 172, 202, 0, 99, 101, 158, 0, 207, 200, 208,
68, 0, 87, 0, 86, 0, 178, 0, 180, 67, 0, 0, 363, 0, 319, 353, 0, 320, 321, 0, 0,
0, 0, 0, 0, 356, 357, 0, 0, 0, 0, 359, 360, 358, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316,
306, 0, 0, 361, 0, 385, 386, 0, 0, 0, 395, 0, 0, 145, 18, 0, 20, 0, 95, 97, 185,
184, 14, 0, 80, 92, 0, 0, 103, 104, 106, 0, 150, 111, 85, 119, 0, 0, 0, 0, 0, 0,
0, 272, 273, 274, 0, 0, 276, 0, 230, 231, 0, 0, 0, 0, 96, 236, 238, 234, 0, 164,
170, 0, 159, 166, 130, 0, 0, 0, 160, 132, 134, 133, 129, 152, 140, 139, 0, 48,
47, 0, 45, 0, 74, 0, 288, 0, 23, 289, 0, 0, 0, 0, 195, 297, 300, 0, 299, 0, 0, 0,
203, 100, 102, 0, 160, 0, 211, 0, 209, 158, 0, 0, 221, 204, 206, 177, 0, 0, 0,
186, 190, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 394, 401, 0, 400, 326, 0, 328, 330,
332, 334, 336, 337, 341, 342, 339, 340, 344, 345, 347, 348, 350, 351, 352, 305,
0, 383, 384, 379, 0, 381, 382, 0, 0, 0, 4, 6, 0, 324, 107, 98, 0, 0, 0, 112, 121,
120, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 271, 275, 0, 0, 0, 229, 0, 232, 0, 15,
303, 165, 0, 0, 168, 0, 0, 0, 161, 131, 158, 154, 153, 141, 7, 0, 0, 0, 79, 77,
296, 198, 0, 298, 194, 196, 290, 0, 287, 174, 173, 216, 0, 210, 213, 0, 160, 0,
212, 0, 362, 0, 0, 187, 0, 191, 389, 390, 0, 0, 0, 0, 0, 0, 0, 354, 397, 0, 0,
365, 380, 378, 398, 0, 0, 82, 105, 108, 81, 109, 122, 119, 119, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 247, 277, 0, 0, 16, 158, 171, 169, 167, 0, 164, 160, 0, 0, 44, 46,
197, 301, 0, 215, 218, 0, 220, 219, 214, 0, 0, 188, 192, 0, 0, 322, 323, 0, 399,
0, 402, 318, 396, 0, 118, 117, 242, 0, 247, 257, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 278, 0, 160, 162, 0, 155, 49, 50, 175, 217, 364, 0, 193, 0, 0, 387, 0, 0, 0,
253, 0, 0, 259, 0, 0, 0, 267, 0, 0, 0, 0, 0, 0, 0, 0, 246, 228, 156, 0, 158, 0,
366, 0, 388, 0, 243, 0, 0, 0, 244, 258, 260, 261, 0, 269, 268, 0, 263, 0, 0, 0,
0, 247, 248, 163, 160, 0, 0, 123, 0, 0, 245, 262, 270, 264, 265, 0, 0, 247, 249,
157, 189, 367, 253, 254, 266, 247, 250, 253, 255, 247, 251, 256, 252, 0, 0, 0};
static short Cyc_yydefgoto[ 113u]={ 795, 40, 41, 42, 244, 43, 374, 44, 376, 45,
210, 46, 47, 62, 63, 440, 441, 48, 145, 49, 50, 274, 275, 51, 85, 52, 216, 217,
96, 97, 98, 218, 149, 386, 387, 388, 53, 54, 539, 540, 541, 55, 56, 57, 58, 121,
108, 436, 464, 465, 427, 568, 562, 423, 424, 287, 266, 151, 76, 77, 481, 379,
482, 483, 452, 453, 141, 146, 147, 466, 294, 247, 248, 249, 250, 251, 252, 253,
700, 749, 254, 255, 278, 279, 280, 455, 456, 457, 256, 257, 360, 183, 531, 184,
185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 495,
496, 200, 201}; static short Cyc_yypact[ 798u]={ 1989, - -32768, - -32768, -
-32768, - -32768, - 64, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, 55, 120, 651, - -32768, 120, - 17, - -32768, - -32768, 47, -
-32768, 126, - -32768, 247, 374, - -32768, 125, 289, 315, - -32768, - -32768,
908, - -32768, 84, 379, - -32768, 105, 55, 55, 55, - -32768, - -32768, 151, -
-32768, 67, 2505, 66, 76, - 8, 120, 323, 331, 2565, - -32768, 340, - -32768, -
22, - -32768, - -32768, - -32768, 717, - -32768, 345, 274, 368, 717, 367, 366,
390, 187, - -32768, - -32768, 4572, 174, 4572, - -32768, 409, 4686, 4157, 4157,
- -32768, 908, 2104, 908, 2104, - -32768, 395, 393, - -32768, 2442, 2565, 2565,
2565, 247, 4686, 247, 426, - -32768, 120, 59, 427, 1386, - -32768, 2505, 105, -
-32768, 1158, 4572, 3021, - -32768, 66, 55, - -32768, - -32768, 2104, 418, -
-32768, 120, 421, 717, - -32768, - -32768, - -32768, - -32768, 448, 4157, 186,
236, 55, 55, 193, - -32768, 80, - -32768, - -32768, - -32768, 425, 18, - -32768,
503, 381, - -32768, 4370, 450, - -32768, 4157, 4228, 454, 463, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, 467, 481, 493, 4441, 4441, -
-32768, - -32768, - -32768, - -32768, 1747, 511, 4512, 4512, - -32768, 4512, -
-32768, - -32768, 515, - -32768, - 23, 537, 500, 502, 505, 428, 128, 437, 394,
112, - -32768, 771, 518, 4512, 146, - -32768, - -32768, 50, 524, - -32768, 525,
- -32768, 526, - -32768, 374, 3092, 2505, - -32768, - -32768, - -32768, - -32768,
529, 530, 4686, - -32768, 304, 534, 174, 247, 120, 538, - -32768, 527, 22, 540,
2622, 541, 551, 547, 552, 3163, 2622, 83, 2622, 2622, - 36, 545, - -32768, -
-32768, 553, - -32768, - -32768, 105, - -32768, - -32768, - -32768, - -32768,
560, 1508, - -32768, - -32768, - -32768, 234, - -32768, - -32768, - -32768, 548,
- 8, - -32768, 175, 558, 556, 246, 559, 38, - -32768, 554, 91, 567, 16, 565, 570,
572, - -32768, - -32768, 571, 574, - -32768, 249, 546, 717, - 13, - -32768, 575,
4686, 4686, 2382, 3234, - 7, - -32768, 119, - -32768, 175, - -32768, 4572, -
-32768, 2274, - -32768, 339, - -32768, - -32768, 4686, 1747, - -32768, 4686, -
-32768, - -32768, 2693, - -32768, 601, 4157, 2219, 654, 4157, 4157, 1747, -
-32768, - -32768, 1508, 578, 423, 4157, - -32768, - -32768, - -32768, - -32768,
4512, 4157, 4512, 4512, 4512, 4512, 4512, 4512, 4512, 4512, 4512, 4512, 4512,
4512, 4512, 4512, 4512, 4512, 4512, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 4157, 4157,
- -32768, 230, - -32768, - -32768, 3305, 238, 4157, - -32768, 2939, 580, -
-32768, - -32768, 908, - -32768, 908, - -32768, - -32768, - -32768, - -32768, -
-32768, 4686, - -32768, - -32768, 4157, 592, 587, - -32768, 583, 4686, - -32768,
- -32768, 247, 523, 4157, 591, 4157, 4157, 666, 1630, 602, - -32768, - -32768, -
-32768, 258, 659, - -32768, 3376, - -32768, - -32768, 2622, 611, 2622, 1864,
2442, - -32768, - -32768, - -32768, 4157, 620, 625, 265, - -32768, 598, - -32768,
621, 615, 4632, 556, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
91, 908, - -32768, 617, 618, 619, 120, - -32768, 4157, - -32768, 717, - -32768,
- -32768, 633, 4157, 717, 40, - -32768, - -32768, - -32768, 624, 622, 628, 174,
631, - -32768, - -32768, - -32768, 632, 556, 634, - -32768, 616, 119, 1925, 4572,
3447, - -32768, - -32768, 425, - -32768, 635, 630, 650, - -32768, - -32768, 229,
3092, 436, 639, 374, 646, 441, 655, 649, 658, 4299, - -32768, - -32768, 657, 661,
537, 270, 500, 502, 505, 428, 128, 128, 437, 437, 437, 437, 394, 394, 112, 112,
- -32768, - -32768, - -32768, - -32768, 663, - -32768, - -32768, - -32768, 664,
- -32768, - -32768, 140, 242, 4572, - -32768, - -32768, 662, - -32768, - -32768,
- -32768, 304, 4157, 670, - -32768, - -32768, - -32768, 671, 120, 280, 444, 4157,
447, 451, 672, 3518, 3589, 288, - -32768, - -32768, 673, 675, 674, - -32768, 653,
- -32768, 2505, - -32768, - -32768, - -32768, 4686, 174, - -32768, 676, 175, 668,
- -32768, - -32768, 179, - -32768, - -32768, - -32768, - -32768, 468, 678, 16, -
-32768, - -32768, - -32768, - -32768, 667, - -32768, - -32768, - -32768, -
-32768, 593, - -32768, 679, - -32768, - -32768, 681, - -32768, - -32768, 684,
556, 141, - -32768, 682, 683, 699, 686, - -32768, 2775, - -32768, - -32768, -
-32768, 2505, 4686, 4228, 766, 683, 692, 2939, - -32768, - -32768, 4157, 4157, -
-32768, - -32768, - -32768, - -32768, 2939, 382, - -32768, - -32768, - -32768, -
-32768, - -32768, 303, 20, 523, 2622, 457, 697, 2622, 4157, 3660, 297, 3731, 299,
3802, 509, - -32768, 700, 708, - -32768, 701, - -32768, - -32768, - -32768, 707,
620, 556, 702, 705, - -32768, - -32768, - -32768, - -32768, 631, - -32768, -
-32768, 710, - -32768, - -32768, - -32768, 711, 4157, - -32768, - -32768, 3092,
723, - -32768, 601, 709, - -32768, 364, - -32768, - -32768, - -32768, 726, -
-32768, - -32768, 774, 730, 509, - -32768, 460, 2622, 464, 3873, 2622, 472, 3944,
4015, 300, 717, 722, 732, - -32768, 2622, 556, 729, 4686, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, 307, - -32768, 733, 4686, - -32768, 2857,
4686, 2622, 549, 737, 743, - -32768, 2622, 2622, 474, - -32768, 2622, 2622, 476,
2622, 478, 4086, - 44, 1508, - -32768, - -32768, - -32768, 621, 701, 4157, -
-32768, 740, - -32768, 482, - -32768, 4157, 748, 746, - -32768, - -32768, -
-32768, - -32768, 2622, - -32768, - -32768, 2622, - -32768, 2622, 2622, 484,
4157, 1098, - -32768, - -32768, 556, 392, 755, - -32768, 311, 1508, - -32768, -
-32768, - -32768, - -32768, - -32768, 2622, 350, 509, - -32768, - -32768, -
-32768, - -32768, 1264, - -32768, - -32768, 1098, - -32768, 549, - -32768, 509,
- -32768, - -32768, - -32768, 859, 860, - -32768}; static short Cyc_yypgoto[ 113u]={
- -32768, 99, - -32768, 550, - -32768, - -32768, - -32768, - -32768, - -32768, -
51, - 49, - 55, - -32768, 360, - 40, 284, - -32768, 12, - 193, - 47, - -32768, -
-32768, 420, - -32768, - 20, - -32768, - 103, - -32768, - -32768, - -32768, 656,
652, 63, - -32768, - -32768, 330, - -32768, - -32768, - 41, - -32768, - -32768,
- 24, - 38, - 111, - -32768, - -32768, - 53, - -32768, - 102, - 113, - 331, 127,
214, 302, 577, - 386, - 86, - 284, 754, - -32768, - 192, - 144, - 346, - 265, -
-32768, 422, - 132, - 67, - 94, - 19, - 251, 61, - -32768, - -32768, - 35, - 241,
- -32768, - -32768, - 648, - 328, - -32768, - -32768, - 21, 594, - -32768, 290,
- -32768, - -32768, 202, 30, - -32768, - 147, - 377, - 143, 555, 543, 557, 542,
544, 257, - 25, 261, 264, - 65, 648, - -32768, - -32768, - -32768, - -32768, -
273, - -32768, - -32768, 9}; static short Cyc_yytable[ 4793u]={ 114, 150, 78,
265, 112, 122, 113, 126, 309, 59, 86, 417, 312, 313, 264, 142, 119, 378, 451,
120, 115, 476, 99, 268, 525, 537, 538, 60, 391, 292, 762, 64, 79, 438, 80, 139,
565, 139, 302, 721, 139, 469, 323, 59, 114, 212, 213, 214, 112, 128, 59, 469,
330, 411, 132, 59, 246, 139, 114, 136, 245, 105, 258, 109, 211, 763, 59, 579,
123, 139, 139, 412, 81, 582, 590, 459, - 182, 1, 259, 79, 119, 491, - 182, 215,
79, 220, 106, 297, 517, 224, 260, 460, 331, 521, 138, 290, 138, 107, 571, 138,
59, 59, 59, 59, 291, 439, 1, 431, 16, 17, 18, 326, 327, 396, 328, 780, 138, 222,
182, 202, - 116, 293, 298, 59, 397, 301, 299, 139, 138, 138, 303, 788, 362, 59,
592, 554, 276, 369, 79, 792, 90, 584, 298, 794, 476, 140, 432, 140, 270, 370,
449, 450, 61, 429, 82, 114, 30, 627, 59, 258, 435, 106, 32, 61, 371, 30, 84, 219,
116, 223, 139, 32, 107, 117, 477, 381, 478, 118, 34, 292, 140, 407, 91, 92, 260,
408, 138, 490, 295, 302, 203, 204, 205, 206, 30, 389, 296, 246, 338, 339, 32,
245, 392, 95, 37, 38, 39, 34, 420, 30, 665, 363, 364, 365, 83, 32, 61, 59, 37,
38, 39, 470, 415, 272, 434, 346, 471, 87, 59, 138, 472, 475, 393, 347, 348, 340,
341, 140, 530, 380, 30, 139, 139, 139, 419, 298, 32, 103, 366, 666, 104, 139,
621, 139, 473, 59, 367, 368, 139, 139, 486, 139, 454, 143, 120, 664, 589, 246,
678, 421, 144, 245, 139, 106, 422, 709, 86, 263, 648, 529, 219, 513, 514, 515,
107, 283, - 85, 535, 284, - 85, 399, 605, 79, 79, 84, 290, 406, 530, 409, 410,
138, 138, 138, 530, 291, 1, 37, 38, 39, 59, 138, 59, 138, 505, 506, 507, 508,
138, 138, 518, 138, 468, 451, 706, 59, 573, 519, 522, 150, 603, 285, 138, 418,
604, 523, 139, 286, 282, 419, 672, 1, 380, 622, 139, 263, 114, 623, 447, 489,
548, 428, 462, 463, 419, 84, 494, 551, 596, 672, 285, 114, 140, 419, 130, 112,
564, 558, 139, 595, 140, 649, 140, 738, 536, 419, 1, 324, 597, 631, 618, 559,
139, 140, 59, 632, 59, 642, 530, 88, 572, 516, 494, 419, 30, 138, 692, 494, 695,
733, 32, 380, 419, 138, 419, 419, - 85, 34, 100, 101, 102, 84, 419, 671, 385, 89,
419, 741, 37, 38, 39, 784, 740, 124, 139, 139, 580, 138, 615, 30, 473, 583, 671,
624, 125, 32, 781, 405, - 207, 494, 127, 138, 300, - 207, - 207, 494, 219, 59,
129, - 207, 560, 291, 276, 672, 219, 419, 79, 790, 654, 131, 787, 79, 793, 608,
30, 716, 133, 674, 675, 717, 32, 134, 680, 555, 527, 557, 528, 34, 93, 94, 139,
713, 271, 138, 138, 304, 305, 298, 37, 38, 39, 681, 135, 782, 207, 764, 59, 419,
208, 288, 289, 336, 337, 599, 114, 1, 342, 343, 258, 324, 389, 344, 345, 148,
380, 221, 139, 484, 653, 225, 488, 273, 324, 779, 647, 277, 671, 493, 419, 537,
538, 785, 698, 699, 498, 140, 703, 574, 281, 138, 606, 419, 296, 59, 789, 610,
419, 791, 633, 419, 630, 635, 419, 308, 114, 636, 419, 314, 112, 655, 656, 686,
419, 139, 722, 419, 315, 454, 724, 419, 316, 524, 747, 748, 211, 138, 728, 419,
754, 419, 757, 419, 759, 419, 317, 65, 769, 305, 777, 419, 140, 683, 684, 30,
503, 504, 318, 79, 542, 32, 544, 545, - 179, 549, 509, 510, 300, - 179, - 179,
511, 512, 682, 332, - 179, 325, 291, 329, 37, 38, 39, 333, 361, 138, 334, 335,
372, 373, 375, 766, 382, 395, 383, 65, 745, 390, 380, 30, 67, 394, 68, 69, 401,
32, 398, 400, 380, 402, 448, 679, 70, 209, 403, 71, 72, 380, 413, 414, 449, 450,
139, 73, 416, 425, 430, 74, 75, 426, 433, 437, 139, 442, 443, 139, 673, 444, 445,
461, 330, 734, 446, 487, 246, 492, 30, 67, 245, 68, 69, 526, 32, 65, 532, 533,
534, 543, 685, 70, 546, 688, 71, 72, 550, 552, 380, 449, 450, 556, 73, 79, 246,
561, 74, 75, 245, 563, 566, 567, 246, 138, 569, 575, 245, 576, 581, 577, 586,
285, 587, 138, 594, 246, 138, 588, 246, 245, 601, 591, 245, 593, 600, 602, 66,
67, 607, 68, 69, 634, 32, 380, 609, 639, 641, 723, 612, 70, 727, 65, 71, 72, 613,
611, 616, 646, 625, 737, 73, 617, 619, 620, 74, 75, 628, 629, 643, 644, 637, 652,
650, 645, 743, 659, 746, 657, 614, 661, 662, 752, 753, 663, 668, 755, 756, 676,
758, 669, 667, 677, 687, 702, 701, 263, 307, 704, 719, 310, 310, 707, 30, 67,
708, 68, 69, 715, 32, 710, 711, 773, 320, 321, 774, 70, 775, 776, 71, 72, 310,
310, 714, 310, 718, 720, 73, 735, 736, 739, 74, 75, 742, 750, 786, 689, 691, 751,
694, 768, 697, 310, 772, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 771,
783, 796, 797, 658, 578, 626, 377, 485, 765, 705, 651, 384, 267, 712, 359, 474,
585, 499, 501, 660, 458, 502, 0, 0, 0, 0, 0, 497, 0, 0, 0, 0, 500, 0, 0, 0, 726,
0, 0, 730, 732, 0, 0, 0, 0, 0, 0, 0, 0, 0, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 761, 0, 0, 0, 0, 0, 0, 0,
767, 0, 22, 0, 23, 0, 770, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 0, 778, 0,
0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 310, 0, 310, 310, 310, 310, 310, 310, 310,
310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 30, 0, 0, 0, 0, 31, 32, 33, 0,
0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0,
0, 0, 0, 310, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 310, 0, 0, 0, 0, 0, 310, 0, 0, 2, 3, 4, 110, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 698, 699, 21, 152, 153, 226, 0,
227, 228, 229, 230, 231, 232, 233, 234, 22, 154, 23, 155, 235, 310, 156, 24, 236,
0, 0, 27, 28, 157, 158, 237, 238, 159, 160, 161, 162, 163, 164, 165, 29, 239,
166, 167, 0, 168, 169, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
310, 0, 0, 0, 0, 240, 170, 171, 172, 173, 31, 32, 33, 22, 241, 111, 0, 0, 174, 0,
0, 35, 243, 0, 27, 28, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 29, 0, 180, 181,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 0, 0, 262, 0,
0, 0, 0, 31, 0, 33, 0, 263, 0, 310, 0, 0, 0, 0, 35, 36, 0, 310, 2, 3, 4, 110, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 747, 748, 21, 152, 153, 226,
0, 227, 228, 229, 230, 231, 232, 233, 234, 22, 154, 23, 155, 235, 0, 156, 24,
236, 0, 0, 27, 28, 157, 158, 237, 238, 159, 160, 161, 162, 163, 164, 165, 29,
239, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 240, 170, 171, 172, 173, 31, 32, 33, 0, 241, 111, 0, 0, 174, 0, 0, 35,
243, 0, 0, 0, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 0, 0, 180, 181, 2, 3, 4,
110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 152, 153,
226, 0, 227, 228, 229, 230, 231, 232, 233, 234, 22, 154, 23, 155, 235, 0, 156,
24, 236, 0, 0, 27, 28, 157, 158, 237, 238, 159, 160, 161, 162, 163, 164, 165, 29,
239, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 240, 170, 171, 172, 173, 31, 32, 33, 0, 241, 111, 242, 0, 174, 0, 0, 35,
243, 0, 0, 0, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 0, 0, 180, 181, 2, 3, 4,
110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 152, 153,
226, 0, 227, 228, 229, 230, 231, 232, 233, 234, 22, 154, 23, 155, 235, 0, 156,
24, 236, 0, 0, 27, 28, 157, 158, 237, 238, 159, 160, 161, 162, 163, 164, 165, 29,
239, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 240, 170, 171, 172, 173, 31, 32, 33, 0, 241, 111, 0, 0, 174, 0, 0, 35,
243, 0, 0, 0, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 0, 0, 180, 181, 2, 3, 4,
110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 152, 153,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 154, 23, 155, 0, 0, 156, 24, 0, 0, 0, 27, 28,
157, 158, 0, 0, 159, 160, 161, 162, 163, 164, 165, 29, 0, 166, 167, 0, 168, 169,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172,
173, 31, 32, 33, 0, 547, 0, 0, 0, 174, 0, 0, 35, 243, 0, 0, 0, 0, 0, 0, 176, 0,
0, 177, 178, 179, 0, 0, 0, 0, 180, 181, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 154, 0, 155, 0,
0, 156, 0, 0, 0, 0, 27, 28, 157, 158, 0, 0, 159, 160, 161, 162, 163, 164, 165,
29, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 170, 171, 172, 173, 31, 32, 33, 0, 0, 322, 0, 0, 174, 0, 0, 35,
243, 0, 0, 0, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 0, 0, 180, 181, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 22, 154, 0, 155, 0, 0, 156, 0, 0, 0, 0, 27, 28, 157, 158, 0, 0, 159,
160, 161, 162, 163, 164, 165, 29, 0, 166, 167, 0, 168, 169, 0, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 31,
32, 33, 0, 22, 0, 0, 0, 174, 0, 0, 35, 243, 0, 0, 27, 28, 0, 0, 176, 0, 0, 177,
178, 179, 0, 0, 0, 29, 180, 181, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 261, 0, 21, 0, 0, 0, 0, 0, 31, 0, 33, 0, 263, 0,
0, 22, 0, 23, 0, 35, 36, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0,
0, 37, 38, 39, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24,
0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32,
33, 0, 0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 1, 0, 2, 3,
4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0, 29, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0,
0, 34, 27, 28, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 29, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 0, 0, 30, 0, 0, 0, 0, 31,
32, 33, 0, 263, 0, 0, 0, 300, - 158, 0, 35, 36, 0, 0, - 158, 0, 291, 0, 37, 38,
39, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 29, 0, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 0, 21, 0, 261, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 263, 23, 0, 0,
290, 0, 24, 35, 36, 0, 27, 28, 0, 291, 0, 37, 38, 39, 0, 0, 0, 0, 0, 29, 0, 0, 0,
0, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 111, 22, 209, 23, 0, 0, 35, 36, 24, 0, 0,
0, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 2, 3, 4, 110, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31,
0, 33, 22, 0, 111, 0, 0, 0, 0, 24, 35, 36, 0, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 152, 153,
226, 0, 227, 228, 229, 230, 231, 232, 233, 234, 31, 154, 33, 155, 235, 0, 156, 0,
236, 0, 0, 35, 36, 157, 158, 237, 238, 159, 160, 161, 162, 163, 164, 165, 0, 239,
166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 240, 170, 171, 172, 173, 0, 32, 152, 153, 241, 111, 0, 0, 174, 479, 0, 0,
175, 0, 0, 154, 0, 155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181,
159, 160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 0, 0,
0, 311, 480, 0, 174, 0, 0, 0, 175, 152, 153, 0, 449, 450, 0, 176, 0, 0, 177, 178,
179, 0, 154, 0, 155, 180, 181, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 160,
161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 0, 0, 0, 311,
670, 0, 174, 0, 0, 0, 175, 152, 153, 0, 449, 450, 0, 176, 0, 0, 177, 178, 179, 0,
154, 0, 155, 180, 181, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 160, 161, 162,
163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 0, 0, 0, 311, 744, 0,
174, 0, 0, 0, 175, 152, 153, 0, 449, 450, 0, 176, 0, 0, 177, 178, 179, 0, 154, 0,
155, 180, 181, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 160, 161, 162, 163,
164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 0, 0, 0, 311, 0, 0, 174, 0,
0, 0, 175, 152, 153, 0, 449, 450, 0, 176, 0, 0, 177, 178, 179, 0, 154, 0, 155,
180, 181, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 160, 161, 162, 163, 164,
165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 0, 0, 0, 0, 174, 0, 0, 0,
175, 0, 0, 154, 0, 155, 269, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180,
181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32,
152, 153, 0, 311, 0, 0, 174, 0, 0, 0, 175, 0, 0, 154, 0, 155, 0, 176, 156, 0,
177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0,
166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 404, 0, 0, 0, 174, 0, 0, 0, 175,
0, 0, 154, 0, 155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159,
160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152,
153, 0, 0, 0, 0, 174, 0, 0, 0, 175, 0, 0, 154, 0, 155, 467, 176, 156, 0, 177,
178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 166,
167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 170, 171, 172, 173, 0, 32, 152, 153, 0, 0, 0, 0, 174, 520, 0, 0, 175, 0, 0,
154, 0, 155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160,
161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 0,
0, 0, 0, 174, 553, 0, 0, 175, 0, 0, 154, 0, 155, 0, 176, 156, 0, 177, 178, 179,
0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0,
168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170,
171, 172, 173, 0, 32, 152, 153, 0, 0, 0, 0, 174, 0, 0, 0, 175, 0, 0, 154, 0, 155,
598, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162,
163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 638, 0, 0, 0,
174, 0, 0, 0, 175, 0, 0, 154, 0, 155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157,
158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172,
173, 0, 32, 152, 153, 640, 0, 0, 0, 174, 0, 0, 0, 175, 0, 0, 154, 0, 155, 0, 176,
156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164,
165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 0, 0, 0, 0, 174, 690, 0,
0, 175, 0, 0, 154, 0, 155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180,
181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32,
152, 153, 0, 0, 0, 0, 174, 693, 0, 0, 175, 0, 0, 154, 0, 155, 0, 176, 156, 0,
177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0,
166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 696, 0, 0, 0, 174, 0, 0, 0, 175,
0, 0, 154, 0, 155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159,
160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152,
153, 0, 0, 0, 0, 174, 725, 0, 0, 175, 0, 0, 154, 0, 155, 0, 176, 156, 0, 177,
178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 166,
167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 170, 171, 172, 173, 0, 32, 152, 153, 0, 0, 0, 0, 174, 729, 0, 0, 175, 0, 0,
154, 0, 155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160,
161, 162, 163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 0,
0, 0, 0, 174, 731, 0, 0, 175, 0, 0, 154, 0, 155, 0, 176, 156, 0, 177, 178, 179,
0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 166, 167, 0,
168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170,
171, 172, 173, 0, 32, 152, 153, 0, 0, 0, 0, 174, 760, 0, 0, 175, 0, 0, 154, 0,
155, 0, 176, 156, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162,
163, 164, 165, 0, 0, 166, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 0, 0, 0, 0,
174, 0, 0, 0, 175, 0, 0, 154, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 157, 158,
180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 0, 167, 0, 168, 169, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0,
32, 152, 153, 0, 311, 0, 0, 174, 0, 0, 0, 175, 0, 0, 154, 0, 0, 0, 176, 0, 0,
177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0,
0, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 0, 614, 0, 0, 174, 0, 0, 0, 175, 0,
0, 154, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160,
161, 162, 163, 164, 165, 0, 0, 0, 167, 0, 168, 169, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 152, 153, 0, 0,
0, 0, 306, 0, 0, 0, 175, 0, 0, 154, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 157,
158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0, 0, 0, 167, 0, 168, 169, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 170, 171, 172,
173, 0, 32, 152, 153, 0, 0, 0, 0, 319, 0, 0, 0, 175, 0, 0, 154, 0, 0, 0, 176, 0,
0, 177, 178, 179, 0, 0, 157, 158, 180, 181, 159, 160, 161, 162, 163, 164, 165, 0,
0, 0, 167, 0, 168, 169, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
0, 0, 0, 0, 0, 0, 30, 170, 171, 172, 173, 0, 32, 0, 22, 0, 0, 0, 0, 174, 0, 0, 0,
175, 0, 27, 28, 0, 0, 0, 176, 0, 0, 177, 178, 179, 0, 0, 29, 0, 180, 181, 0, 0,
0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 31, 0, 33, 22, 0, 137, 0, 0, 0, 0, 0, 35, 36, 0, 27, 28, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 29, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 570, 0, 0, 0, 0, 22, 0, 0, 31, 0, 33, 0, 0, 0, 0, 0,
27, 28, 0, 35, 36, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0,
0, 0, 0, 0, 0, 0, 35, 36}; static short Cyc_yycheck[ 4793u]={ 55, 87, 23, 116,
55, 58, 55, 62, 155, 0, 34, 252, 156, 156, 116, 82, 56, 209, 283, 57, 55, 305,
46, 117, 370, 5, 6, 91, 221, 140, 74, 22, 23, 17, 25, 82, 422, 84, 149, 687, 87,
292, 174, 34, 99, 100, 101, 102, 99, 70, 41, 302, 75, 89, 75, 46, 111, 104, 113,
79, 111, 52, 113, 54, 99, 109, 57, 444, 59, 116, 117, 107, 89, 450, 460, 88, 98,
1, 113, 70, 120, 322, 104, 103, 75, 105, 94, 69, 361, 109, 114, 104, 115, 366,
82, 102, 84, 105, 429, 87, 91, 92, 93, 94, 111, 89, 1, 69, 17, 18, 19, 176, 177,
91, 179, 763, 104, 108, 88, 89, 100, 140, 104, 114, 102, 149, 108, 174, 116, 117,
149, 779, 197, 124, 465, 408, 127, 87, 129, 787, 41, 101, 104, 791, 428, 82, 108,
84, 118, 99, 110, 111, 97, 266, 107, 210, 89, 534, 149, 210, 271, 94, 95, 97,
114, 89, 107, 104, 102, 110, 217, 95, 105, 107, 306, 210, 308, 111, 102, 290,
117, 98, 98, 99, 208, 102, 174, 319, 108, 300, 91, 92, 93, 94, 89, 219, 116, 252,
70, 71, 95, 252, 222, 98, 113, 114, 115, 102, 261, 89, 69, 65, 66, 67, 88, 95,
97, 208, 113, 114, 115, 102, 246, 124, 271, 113, 107, 102, 219, 217, 111, 298,
223, 121, 122, 107, 108, 174, 385, 209, 89, 288, 289, 290, 104, 104, 95, 96, 102,
108, 99, 298, 112, 300, 294, 246, 110, 111, 305, 306, 315, 308, 283, 89, 302,
596, 459, 322, 614, 94, 96, 322, 319, 94, 99, 661, 300, 98, 562, 382, 217, 346,
347, 348, 105, 99, 99, 390, 102, 102, 229, 483, 283, 284, 107, 102, 235, 444,
237, 238, 288, 289, 290, 450, 111, 1, 113, 114, 115, 300, 298, 302, 300, 338,
339, 340, 341, 305, 306, 89, 308, 291, 587, 654, 315, 436, 96, 89, 414, 100, 94,
319, 98, 104, 96, 382, 100, 135, 104, 604, 1, 311, 100, 390, 98, 400, 104, 98,
318, 400, 104, 288, 289, 104, 107, 325, 98, 470, 623, 94, 415, 298, 104, 89, 415,
100, 415, 414, 470, 306, 563, 308, 703, 393, 104, 1, 174, 471, 98, 109, 415, 428,
319, 374, 104, 376, 98, 534, 99, 436, 360, 361, 104, 89, 382, 98, 366, 98, 98,
95, 370, 104, 390, 104, 104, 102, 102, 47, 48, 49, 107, 104, 604, 109, 99, 104,
109, 113, 114, 115, 109, 705, 99, 470, 471, 446, 414, 492, 89, 469, 451, 623,
526, 102, 95, 766, 234, 98, 408, 99, 428, 102, 103, 104, 414, 382, 437, 102, 109,
419, 111, 442, 717, 390, 104, 446, 784, 570, 90, 109, 451, 789, 486, 89, 100, 98,
610, 610, 104, 95, 104, 618, 411, 374, 413, 376, 102, 98, 99, 526, 672, 121, 470,
471, 103, 104, 104, 113, 114, 115, 108, 101, 100, 98, 735, 486, 104, 104, 138,
139, 72, 73, 472, 558, 1, 68, 69, 558, 306, 533, 116, 117, 103, 483, 88, 562,
314, 570, 91, 317, 102, 319, 763, 558, 103, 717, 103, 104, 5, 6, 771, 22, 23,
331, 471, 648, 437, 89, 526, 103, 104, 116, 533, 784, 103, 104, 787, 103, 104,
540, 103, 104, 102, 608, 103, 104, 102, 608, 90, 91, 103, 104, 609, 103, 104,
102, 587, 103, 104, 102, 368, 22, 23, 608, 562, 103, 104, 103, 104, 103, 104,
103, 104, 102, 38, 103, 104, 103, 104, 526, 631, 632, 89, 336, 337, 102, 587,
395, 95, 397, 398, 98, 400, 342, 343, 102, 103, 104, 344, 345, 630, 74, 109, 102,
111, 100, 113, 114, 115, 119, 102, 609, 120, 118, 100, 100, 100, 740, 99, 102,
100, 38, 718, 99, 604, 89, 90, 99, 92, 93, 89, 95, 102, 102, 614, 98, 100, 617,
102, 101, 98, 105, 106, 623, 109, 102, 110, 111, 705, 113, 100, 103, 103, 117,
118, 109, 112, 100, 715, 104, 100, 718, 609, 101, 103, 100, 75, 698, 104, 25,
735, 103, 89, 90, 735, 92, 93, 107, 95, 38, 98, 104, 109, 102, 633, 102, 30, 636,
105, 106, 98, 42, 672, 110, 111, 94, 113, 698, 763, 89, 117, 118, 763, 88, 116,
94, 771, 705, 103, 102, 771, 103, 89, 104, 100, 94, 104, 715, 112, 784, 718, 103,
787, 784, 104, 103, 787, 103, 103, 89, 89, 90, 103, 92, 93, 543, 95, 717, 102,
547, 548, 690, 103, 102, 693, 38, 105, 106, 100, 104, 103, 108, 100, 702, 113,
104, 103, 103, 117, 118, 100, 100, 99, 98, 102, 107, 100, 103, 715, 112, 719,
103, 99, 104, 103, 724, 725, 103, 89, 728, 729, 25, 731, 107, 112, 103, 99, 89,
98, 98, 152, 94, 28, 155, 156, 103, 89, 90, 103, 92, 93, 102, 95, 103, 103, 754,
168, 169, 757, 102, 759, 760, 105, 106, 176, 177, 103, 179, 102, 99, 113, 109,
100, 104, 117, 118, 103, 100, 777, 637, 638, 98, 640, 103, 642, 197, 100, 76, 77,
78, 79, 80, 81, 82, 83, 84, 85, 109, 103, 0, 0, 577, 442, 533, 208, 315, 739,
653, 566, 217, 116, 669, 101, 296, 452, 332, 334, 587, 284, 335, - 1, - 1, - 1,
- 1, - 1, 330, - 1, - 1, - 1, - 1, 333, - 1, - 1, - 1, 692, - 1, - 1, 695, 696,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, 733, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 741, - 1, 37, - 1, 39, - 1, 747, - 1, - 1, 44, - 1,
46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, 762, - 1, - 1, - 1, - 1, 61, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 330, - 1, 332, 333, 334, 335, 336, 337, 338,
339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 89, - 1, - 1, - 1, - 1, 94, 95,
96, - 1, - 1, - 1, 100, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, -
1, 113, 114, 115, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 385, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 444, - 1, - 1, - 1, - 1, - 1, 450, - 1, -
1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 492, 43,
44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
64, - 1, 66, 67, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
534, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, 37, 98, 99, - 1, - 1,
102, - 1, - 1, 105, 106, - 1, 48, 49, - 1, - 1, - 1, 113, - 1, - 1, 116, 117,
118, - 1, - 1, 61, - 1, 123, 124, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, 89, - 1,
- 1, - 1, - 1, 94, - 1, 96, - 1, 98, - 1, 610, - 1, - 1, - 1, - 1, 105, 106, - 1,
618, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43,
44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98,
99, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, -
1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31,
32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50,
51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, - 1, 66, 67, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98, 99, 100, - 1, 102, - 1, -
1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, -
1, - 1, - 1, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37,
38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
57, 58, 59, 60, 61, 62, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91,
92, 93, 94, 95, 96, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, -
1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24,
25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, 39, 40, - 1, -
1, 43, 44, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60,
61, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94,
95, 96, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1,
- 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, -
1, 48, 49, 50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60, 61, - 1, 63, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, - 1, 99, -
1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1,
116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51,
- 1, - 1, 54, 55, 56, 57, 58, 59, 60, 61, - 1, 63, 64, - 1, 66, 67, - 1, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, 89,
90, 91, 92, 93, 94, 95, 96, - 1, 37, - 1, - 1, - 1, 102, - 1, - 1, 105, 106, - 1,
- 1, 48, 49, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, 61, 123, 124,
0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
86, - 1, 24, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, - 1, 98, - 1, - 1, 37, - 1,
39, - 1, 105, 106, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
89, - 1, - 1, - 1, - 1, 94, 95, 96, - 1, - 1, - 1, - 1, - 1, 102, - 1, - 1, 105,
106, - 1, - 1, - 1, - 1, - 1, - 1, 113, 114, 115, 1, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1,
46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1, - 1, - 1, - 1, 94, 95,
96, - 1, - 1, - 1, 100, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, -
1, 113, 114, 115, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1,
- 1, - 1, - 1, - 1, 1, - 1, - 1, - 1, - 1, 61, - 1, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 89, - 1, - 1, 37, - 1, 94, 95, 96, - 1, - 1, - 1, - 1, - 1, 102, 48, 49, 105,
106, - 1, - 1, - 1, - 1, - 1, - 1, 113, 114, 115, 61, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 86, - 1, - 1, 89, - 1, - 1, - 1, - 1, 94, 95, 96, - 1, 98, - 1, - 1,
- 1, 102, 103, - 1, 105, 106, - 1, - 1, 109, - 1, 111, - 1, 113, 114, 115, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61,
- 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, -
1, 24, - 1, 86, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, 37, 98, 39, - 1,
- 1, 102, - 1, 44, 105, 106, - 1, 48, 49, - 1, 111, - 1, 113, 114, 115, - 1, - 1,
- 1, - 1, - 1, 61, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1,
96, - 1, - 1, 99, 37, 101, 39, - 1, - 1, 105, 106, 44, - 1, - 1, - 1, 48, 49, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, - 1, 3, 4, 5, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, 37, - 1, 99, - 1, - 1, - 1, - 1, 44,
105, 106, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 94, 38, 96,
40, 41, - 1, 43, - 1, 45, - 1, - 1, 105, 106, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, - 1, 62, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92,
93, - 1, 95, 25, 26, 98, 99, - 1, - 1, 102, 32, - 1, - 1, 106, - 1, - 1, 38, - 1,
40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57,
58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91,
92, 93, - 1, 95, - 1, - 1, - 1, 99, 100, - 1, 102, - 1, - 1, - 1, 106, 25, 26, -
1, 110, 111, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43,
- 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60, - 1,
- 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, -
1, - 1, - 1, 99, 100, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, 110, 111, - 1,
113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1,
- 1, - 1, 50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 99,
100, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, 110, 111, - 1, 113, - 1, - 1,
116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50,
51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 99, - 1, - 1, 102,
- 1, - 1, - 1, 106, 25, 26, - 1, 110, 111, - 1, 113, - 1, - 1, 116, 117, 118, -
1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54,
55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106,
- 1, - 1, 38, - 1, 40, 112, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123,
124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, 99, - 1, - 1, 102, - 1, - 1, -
1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50,
51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, 98, - 1, - 1, - 1, 102, -
1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1,
- 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1,
102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, 112, 113, 43, - 1, 116, 117, 118,
- 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1,
- 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116,
117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63,
64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1,
- 1, - 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1,
116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1,
63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26,
- 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, 112, 113, 43,
- 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1,
- 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95,
25, 26, 98, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1,
113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59,
60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93,
- 1, 95, 25, 26, 98, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1,
40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57,
58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91,
92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1,
38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54,
55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106,
- 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123,
124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, 98, - 1, - 1, - 1, 102, - 1, - 1, -
1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50,
51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102,
103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1,
- 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1,
102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118,
- 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1,
- 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116,
117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63,
64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1,
- 1, - 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1,
116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1,
63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26,
- 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, - 1, - 1, 113, -
1, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, -
1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1,
95, 25, 26, - 1, 99, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, - 1,
- 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57,
58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91,
92, 93, - 1, 95, 25, 26, - 1, 99, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1,
38, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54,
55, 56, 57, 58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106,
- 1, - 1, 38, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123,
124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, - 1, - 1,
- 1, 106, - 1, - 1, 38, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1,
50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 89, 90, 91, 92, 93, - 1, 95, - 1, 37, - 1, - 1, - 1, - 1, 102, - 1, - 1, -
1, 106, - 1, 48, 49, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 61,
- 1, 123, 124, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, 37,
- 1, 99, - 1, - 1, - 1, - 1, - 1, 105, 106, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 61, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1,
37, - 1, - 1, 94, - 1, 96, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, 105, 106, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 105, 106}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tuple19{ void* f1; void* f2; } ; struct _tuple20{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short*
yyss=( short*)({ unsigned int _temp3347= 10000u; short* _temp3348=( short*)
GC_malloc_atomic( sizeof( short) * _temp3347);{ unsigned int _temp3349=
_temp3347; unsigned int i; for( i= 0; i < _temp3349; i ++){ _temp3348[ i]=(
short) 0;}}; _temp3348;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3344= 10000u; void** _temp3345=( void**) GC_malloc( sizeof(
void*) * _temp3344);{ unsigned int _temp3346= _temp3344; unsigned int i; for( i=
0; i < _temp3346; i ++){ _temp3345[ i]= Cyc_yylval;}}; _temp3345;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3341= 10000u; struct Cyc_Yyltype* _temp3342=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) * _temp3341);{ unsigned int
_temp3343= _temp3341; unsigned int i; for( i= 0; i < _temp3343; i ++){ _temp3342[
i]= Cyc_yynewloc();}}; _temp3342;}); int yystacksize= 10000; void* yyval= Cyc_yylval;
int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2;
yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate: yyss[
_check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate; if(
yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 798u, yystate)]; if( yyn == - 32768){ goto
yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <=
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
352: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 353u, Cyc_yychar)]:
238;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4792)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4793u, yyn)] != yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4793u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrlab;}} if( yyn == 797){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus != 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 798u, yystate)]; if(
yyn == 0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 409u, yyn)]; if( yylen > 0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset + 1) - yylen)];} switch(
yyn){ case 1: _LL1495: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1496:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1498=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1498[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1499; _temp1499.tag= Cyc_DeclList_tok; _temp1499.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1499;});
_temp1498;}); break; case 3: _LL1497: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1501=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1501[ 0]=({ struct Cyc_DeclList_tok_struct _temp1502; _temp1502.tag= Cyc_DeclList_tok;
_temp1502.f1=({ struct Cyc_List_List* _temp1503=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1503->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1504=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1504->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1505=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1505[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1506; _temp1506.tag= Cyc_Absyn_Using_d;
_temp1506.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1506.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1506;}); _temp1505;}));
_temp1504->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1504;});
_temp1503->tl= 0; _temp1503;}); _temp1502;}); _temp1501;}); Cyc_Lex_leave_using();
break; case 4: _LL1500: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1508=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1508[ 0]=({ struct Cyc_DeclList_tok_struct _temp1509; _temp1509.tag= Cyc_DeclList_tok;
_temp1509.f1=({ struct Cyc_List_List* _temp1510=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1510->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1511=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1511->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1512=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1512[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1513; _temp1513.tag= Cyc_Absyn_Using_d;
_temp1513.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1513.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1513;});
_temp1512;})); _temp1511->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1511;}); _temp1510->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1510;}); _temp1509;});
_temp1508;}); break; case 5: _LL1507: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1515=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1515[ 0]=({ struct Cyc_DeclList_tok_struct _temp1516; _temp1516.tag= Cyc_DeclList_tok;
_temp1516.f1=({ struct Cyc_List_List* _temp1517=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1517->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1518=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1518->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1519=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1519[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1520; _temp1520.tag=
Cyc_Absyn_Namespace_d; _temp1520.f1=({ struct _tagged_arr* _temp1521=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1521[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1521;});
_temp1520.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1520;}); _temp1519;})); _temp1518->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1518;}); _temp1517->tl= 0; _temp1517;}); _temp1516;}); _temp1515;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1514: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1523=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1523[ 0]=({ struct Cyc_DeclList_tok_struct _temp1524; _temp1524.tag= Cyc_DeclList_tok;
_temp1524.f1=({ struct Cyc_List_List* _temp1525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1525->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1526=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1526->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1527=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1527[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1528; _temp1528.tag=
Cyc_Absyn_Namespace_d; _temp1528.f1=({ struct _tagged_arr* _temp1529=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1529[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1529;});
_temp1528.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1528;}); _temp1527;})); _temp1526->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1526;}); _temp1525->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1525;}); _temp1524;});
_temp1523;}); break; case 7: _LL1522: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1531=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1531[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1532; _temp1532.tag= Cyc_DeclList_tok; _temp1532.f1=({ struct Cyc_List_List*
_temp1533=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1533->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1534=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1534->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1535=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1535[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1536; _temp1536.tag= Cyc_Absyn_ExternC_d;
_temp1536.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1536;}); _temp1535;})); _temp1534->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1534;}); _temp1533->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1533;}); _temp1532;});
_temp1531;}); break; case 8: _LL1530: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1538=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1538[ 0]=({ struct Cyc_DeclList_tok_struct _temp1539; _temp1539.tag= Cyc_DeclList_tok;
_temp1539.f1= 0; _temp1539;}); _temp1538;}); break; case 9: _LL1537: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1541=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1541[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1542; _temp1542.tag= Cyc_DeclList_tok; _temp1542.f1=({ struct Cyc_List_List*
_temp1543=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1543->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1544=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1544[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1545; _temp1545.tag= Cyc_Absyn_Fn_d;
_temp1545.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1545;}); _temp1544;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1543->tl=
0; _temp1543;}); _temp1542;}); _temp1541;}); break; case 10: _LL1540: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1546: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1548=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1548[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1549; _temp1549.tag= Cyc_FnDecl_tok; _temp1549.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1549;});
_temp1548;}); break; case 12: _LL1547: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1551=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1551[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1552; _temp1552.tag= Cyc_FnDecl_tok;
_temp1552.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1553=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1553->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1553;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1552;}); _temp1551;}); break; case 13: _LL1550: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1555=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1555[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1556; _temp1556.tag= Cyc_FnDecl_tok;
_temp1556.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1556;}); _temp1555;}); break; case 14: _LL1554: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1558=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1558[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1559; _temp1559.tag= Cyc_FnDecl_tok;
_temp1559.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1560=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1560->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1560;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1559;}); _temp1558;}); break; case 15: _LL1557: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1562=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1562[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1563; _temp1563.tag= Cyc_FnDecl_tok;
_temp1563.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1564=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1564->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1564;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1563;}); _temp1562;}); break; case 16: _LL1561: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1566=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1566[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1567; _temp1567.tag= Cyc_FnDecl_tok;
_temp1567.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1568=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1568->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1568;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1567;}); _temp1566;}); break; case 17: _LL1565: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1569:
Cyc_Lex_leave_using(); break; case 19: _LL1570: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1572=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1572[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1572;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1571: Cyc_Lex_leave_namespace(); break; case 21: _LL1573: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1575=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1575[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1576; _temp1576.tag= Cyc_DeclList_tok; _temp1576.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1576;}); _temp1575;}); break; case 22: _LL1574: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1578=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1578[ 0]=({ struct Cyc_DeclList_tok_struct _temp1579; _temp1579.tag= Cyc_DeclList_tok;
_temp1579.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1579;}); _temp1578;}); break; case 23: _LL1577: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1581=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1581[ 0]=({ struct Cyc_DeclList_tok_struct _temp1582; _temp1582.tag= Cyc_DeclList_tok;
_temp1582.f1=({ struct Cyc_List_List* _temp1583=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1583->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1583->tl=
0; _temp1583;}); _temp1582;}); _temp1581;}); break; case 24: _LL1580: { struct
Cyc_List_List* _temp1585= 0;{ struct Cyc_List_List* _temp1586= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1586 != 0; _temp1586=(( struct Cyc_List_List*) _check_null( _temp1586))->tl){
struct _tagged_arr* _temp1587=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1586))->hd; struct _tuple1* _temp1588=({ struct _tuple1*
_temp1591=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1591->f1=
Cyc_Absyn_rel_ns_null; _temp1591->f2= _temp1587; _temp1591;}); struct Cyc_Absyn_Vardecl*
_temp1589= Cyc_Absyn_new_vardecl( _temp1588, Cyc_Absyn_wildtyp( 0), 0);
_temp1585=({ struct Cyc_List_List* _temp1590=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1590->hd=( void*) _temp1589; _temp1590->tl=
_temp1585; _temp1590;});}} _temp1585=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1585); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1592=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1592[ 0]=({ struct Cyc_DeclList_tok_struct _temp1593; _temp1593.tag= Cyc_DeclList_tok;
_temp1593.f1=({ struct Cyc_List_List* _temp1594=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1594->hd=( void*) Cyc_Absyn_letv_decl(
_temp1585, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1594->tl= 0; _temp1594;}); _temp1593;});
_temp1592;}); break;} case 25: _LL1584: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1595:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1597=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1597[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1598; _temp1598.tag= Cyc_DeclList_tok; _temp1598.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1598;});
_temp1597;}); break; case 27: _LL1596: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1600=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1600[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1601; _temp1601.tag= Cyc_DeclSpec_tok;
_temp1601.f1=({ struct Cyc_Parse_Declaration_spec* _temp1602=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1602->sc=({ struct
Cyc_Core_Opt* _temp1603=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1603->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1603;});
_temp1602->tq= Cyc_Absyn_empty_tqual(); _temp1602->type_specs= 0; _temp1602->is_inline=
0; _temp1602->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1602;}); _temp1601;});
_temp1600;}); break; case 28: _LL1599: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1605=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1605[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1606; _temp1606.tag= Cyc_DeclSpec_tok; _temp1606.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1607=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1607->sc=({ struct Cyc_Core_Opt* _temp1608=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1608->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1608;});
_temp1607->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1607->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1607->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1607->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1607;}); _temp1606;}); _temp1605;}); break; case 29: _LL1604: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1610=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1610[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1611; _temp1611.tag= Cyc_DeclSpec_tok; _temp1611.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1612=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1612->sc= 0; _temp1612->tq= Cyc_Absyn_empty_tqual(); _temp1612->type_specs=({
struct Cyc_List_List* _temp1613=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1613->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1613->tl= 0;
_temp1613;}); _temp1612->is_inline= 0; _temp1612->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1612;});
_temp1611;}); _temp1610;}); break; case 30: _LL1609: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1615=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1615[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1616; _temp1616.tag= Cyc_DeclSpec_tok;
_temp1616.f1=({ struct Cyc_Parse_Declaration_spec* _temp1617=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1617->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1617->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1617->type_specs=({ struct Cyc_List_List* _temp1618=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1618->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1618->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1618;});
_temp1617->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1617->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1617;}); _temp1616;}); _temp1615;}); break; case 31: _LL1614: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1620=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1620[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1621; _temp1621.tag= Cyc_DeclSpec_tok; _temp1621.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1622=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1622->sc= 0; _temp1622->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1622->type_specs=
0; _temp1622->is_inline= 0; _temp1622->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1622;});
_temp1621;}); _temp1620;}); break; case 32: _LL1619: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1624=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1624[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1625; _temp1625.tag= Cyc_DeclSpec_tok;
_temp1625.f1=({ struct Cyc_Parse_Declaration_spec* _temp1626=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1626->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1626->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1626->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1626->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1626->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1626;}); _temp1625;}); _temp1624;}); break; case 33: _LL1623: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1628=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1628[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1629; _temp1629.tag= Cyc_DeclSpec_tok; _temp1629.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1630=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1630->sc= 0; _temp1630->tq= Cyc_Absyn_empty_tqual(); _temp1630->type_specs=
0; _temp1630->is_inline= 1; _temp1630->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1630;});
_temp1629;}); _temp1628;}); break; case 34: _LL1627: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1632=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1632[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1633; _temp1633.tag= Cyc_DeclSpec_tok;
_temp1633.f1=({ struct Cyc_Parse_Declaration_spec* _temp1634=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1634->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1634->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1634->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1634->is_inline=
1; _temp1634->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1634;}); _temp1633;}); _temp1632;}); break; case 35: _LL1631: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1636=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1636[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1637; _temp1637.tag= Cyc_StorageClass_tok;
_temp1637.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1637;}); _temp1636;});
break; case 36: _LL1635: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1639=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1639[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1640; _temp1640.tag=
Cyc_StorageClass_tok; _temp1640.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1640;}); _temp1639;}); break; case 37: _LL1638: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1642=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1642[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1643; _temp1643.tag=
Cyc_StorageClass_tok; _temp1643.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1643;}); _temp1642;}); break; case 38: _LL1641: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1645=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1645[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1646; _temp1646.tag=
Cyc_StorageClass_tok; _temp1646.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1646;}); _temp1645;}); break; case 39: _LL1644: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1648=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1648[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1649; _temp1649.tag= Cyc_StorageClass_tok;
_temp1649.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1649;}); _temp1648;});
break; case 40: _LL1647: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1651=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1651[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1652; _temp1652.tag=
Cyc_StorageClass_tok; _temp1652.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1652;}); _temp1651;}); break; case 41: _LL1650: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1654=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1654[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1655; _temp1655.tag=
Cyc_StorageClass_tok; _temp1655.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1655;}); _temp1654;}); break; case 42: _LL1653: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1657=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1657[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1658; _temp1658.tag=
Cyc_AttributeList_tok; _temp1658.f1= 0; _temp1658;}); _temp1657;}); break; case
43: _LL1656: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1659: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1661=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1661[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1662; _temp1662.tag=
Cyc_AttributeList_tok; _temp1662.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1662;});
_temp1661;}); break; case 45: _LL1660: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1664=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1664[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1665; _temp1665.tag=
Cyc_AttributeList_tok; _temp1665.f1=({ struct Cyc_List_List* _temp1666=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1666->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1666->tl= 0; _temp1666;}); _temp1665;}); _temp1664;});
break; case 46: _LL1663: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1668=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1668[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1669; _temp1669.tag=
Cyc_AttributeList_tok; _temp1669.f1=({ struct Cyc_List_List* _temp1670=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1670->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1670->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1670;}); _temp1669;});
_temp1668;}); break; case 47: _LL1667: { struct _tagged_arr _temp1672= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1672, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1672, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1672, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1672,
sizeof( unsigned char),( int)( _get_arr_size( _temp1672, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1672,
sizeof( unsigned char),( int)( _get_arr_size( _temp1672, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1672=( struct _tagged_arr) Cyc_String_substring(
_temp1672, 2,( int)( _get_arr_size( _temp1672, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1672,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1673=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1673[ 0]=({ struct Cyc_Attribute_tok_struct _temp1674; _temp1674.tag= Cyc_Attribute_tok;
_temp1674.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1674;}); _temp1673;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1675=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1675[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1676; _temp1676.tag= Cyc_Attribute_tok; _temp1676.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1676;}); _temp1675;});} break;}} case
48: _LL1671: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1678=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1678[ 0]=({ struct Cyc_Attribute_tok_struct _temp1679; _temp1679.tag= Cyc_Attribute_tok;
_temp1679.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1679;}); _temp1678;});
break; case 49: _LL1677: { struct _tagged_arr _temp1681= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1684; int _temp1685; struct _tuple16* _temp1682= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1684=*
_temp1682; _LL1686: _temp1685= _temp1684.f2; goto _LL1683; _LL1683: { void* a;
if( Cyc_String_zstrcmp( _temp1681, _tag_arr("regparm", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1681, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) == 0){ if( _temp1685 <= 0? 1: _temp1685 > 3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1687=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1687[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1688; _temp1688.tag= Cyc_Absyn_Regparm_att; _temp1688.f1= _temp1685;
_temp1688;}); _temp1687;});} else{ if( Cyc_String_zstrcmp( _temp1681, _tag_arr("aligned",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1681, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1685 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1685; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1689=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1689[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1690; _temp1690.tag= Cyc_Absyn_Aligned_att;
_temp1690.f1= _temp1685; _temp1690;}); _temp1689;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1691=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1691[ 0]=({ struct Cyc_Attribute_tok_struct _temp1692; _temp1692.tag= Cyc_Attribute_tok;
_temp1692.f1=( void*) a; _temp1692;}); _temp1691;}); break;}} case 50: _LL1680: {
struct _tagged_arr _temp1694= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1695= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1694, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1694, _tag_arr("__section__",
sizeof( unsigned char), 12u))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1696=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1696[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1697; _temp1697.tag=
Cyc_Absyn_Section_att; _temp1697.f1= _temp1695; _temp1697;}); _temp1696;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1698=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1698[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1699; _temp1699.tag= Cyc_Attribute_tok; _temp1699.f1=(
void*) a; _temp1699;}); _temp1698;}); break;} case 51: _LL1693: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1701=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1701[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1702; _temp1702.tag= Cyc_TypeSpecifier_tok;
_temp1702.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1702;});
_temp1701;}); break; case 52: _LL1700: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1704=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1704[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1705; _temp1705.tag=
Cyc_TypeSpecifier_tok; _temp1705.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1705;}); _temp1704;}); break; case 53: _LL1703:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1707=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1707[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1708; _temp1708.tag= Cyc_TypeSpecifier_tok;
_temp1708.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1708;});
_temp1707;}); break; case 54: _LL1706: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1710=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1710[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1711; _temp1711.tag=
Cyc_TypeSpecifier_tok; _temp1711.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1712=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1712[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1713; _temp1713.tag=
Cyc_Parse_Short_spec; _temp1713.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1713;});
_temp1712;})); _temp1711;}); _temp1710;}); break; case 55: _LL1709: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1715=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1715[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1716; _temp1716.tag= Cyc_TypeSpecifier_tok;
_temp1716.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1716;});
_temp1715;}); break; case 56: _LL1714: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1718=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1718[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1719; _temp1719.tag=
Cyc_TypeSpecifier_tok; _temp1719.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1720=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1720[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1721; _temp1721.tag= Cyc_Parse_Long_spec;
_temp1721.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1721;}); _temp1720;})); _temp1719;}); _temp1718;});
break; case 57: _LL1717: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1723=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1723[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1724; _temp1724.tag=
Cyc_TypeSpecifier_tok; _temp1724.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1724;}); _temp1723;}); break; case 58: _LL1722:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1726=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1726[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1727; _temp1727.tag= Cyc_TypeSpecifier_tok;
_temp1727.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1727;});
_temp1726;}); break; case 59: _LL1725: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1729=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1729[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1730; _temp1730.tag=
Cyc_TypeSpecifier_tok; _temp1730.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1731=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1731[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1732; _temp1732.tag=
Cyc_Parse_Signed_spec; _temp1732.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1732;});
_temp1731;})); _temp1730;}); _temp1729;}); break; case 60: _LL1728: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1734=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1734[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1735; _temp1735.tag= Cyc_TypeSpecifier_tok;
_temp1735.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1736=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1736[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1737; _temp1737.tag= Cyc_Parse_Unsigned_spec; _temp1737.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1737;});
_temp1736;})); _temp1735;}); _temp1734;}); break; case 61: _LL1733: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 62: _LL1738:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
63: _LL1739: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 64: _LL1740: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1742=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1742[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1743; _temp1743.tag=
Cyc_TypeSpecifier_tok; _temp1743.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1743;});
_temp1742;}); break; case 65: _LL1741: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1745=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1745[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1746; _temp1746.tag=
Cyc_TypeSpecifier_tok; _temp1746.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1746;}); _temp1745;}); break; case 66: _LL1744: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1748=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1748[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1749; _temp1749.tag=
Cyc_TypeSpecifier_tok; _temp1749.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1750=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1750[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1751; _temp1751.tag= Cyc_Absyn_TypedefType;
_temp1751.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1751.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1751.f3= 0;
_temp1751;}); _temp1750;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1749;});
_temp1748;}); break; case 67: _LL1747: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1753=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1753[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1754; _temp1754.tag=
Cyc_TypeSpecifier_tok; _temp1754.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1755=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1755[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1756; _temp1756.tag= Cyc_Absyn_TupleType;
_temp1756.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1756;});
_temp1755;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1754;}); _temp1753;}); break; case 68:
_LL1752: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1758=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1758[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1759; _temp1759.tag=
Cyc_TypeSpecifier_tok; _temp1759.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1760=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1760[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1761; _temp1761.tag= Cyc_Absyn_RgnHandleType;
_temp1761.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1761;}); _temp1760;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1759;}); _temp1758;}); break; case 69: _LL1757: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1763=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1763[ 0]=({ struct Cyc_Kind_tok_struct _temp1764; _temp1764.tag= Cyc_Kind_tok;
_temp1764.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1764;});
_temp1763;}); break; case 70: _LL1762: { struct _tagged_arr* _temp1768; void*
_temp1770; struct _tuple1 _temp1766=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1771: _temp1770=
_temp1766.f1; goto _LL1769; _LL1769: _temp1768= _temp1766.f2; goto _LL1767;
_LL1767: if( _temp1770 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1772=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1772[ 0]=({ struct Cyc_Kind_tok_struct
_temp1773; _temp1773.tag= Cyc_Kind_tok; _temp1773.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1768, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1773;}); _temp1772;}); break;} case 71:
_LL1765: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1775=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1775[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1776; _temp1776.tag= Cyc_TypeQual_tok;
_temp1776.f1=({ struct Cyc_Absyn_Tqual _temp1777; _temp1777.q_const= 1;
_temp1777.q_volatile= 0; _temp1777.q_restrict= 0; _temp1777;}); _temp1776;});
_temp1775;}); break; case 72: _LL1774: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1779=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1779[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1780; _temp1780.tag= Cyc_TypeQual_tok; _temp1780.f1=({ struct Cyc_Absyn_Tqual
_temp1781; _temp1781.q_const= 0; _temp1781.q_volatile= 1; _temp1781.q_restrict=
0; _temp1781;}); _temp1780;}); _temp1779;}); break; case 73: _LL1778: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1783=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1783[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1784; _temp1784.tag= Cyc_TypeQual_tok;
_temp1784.f1=({ struct Cyc_Absyn_Tqual _temp1785; _temp1785.q_const= 0;
_temp1785.q_volatile= 0; _temp1785.q_restrict= 1; _temp1785;}); _temp1784;});
_temp1783;}); break; case 74: _LL1782: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1787=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1787[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1788; _temp1788.tag=
Cyc_TypeSpecifier_tok; _temp1788.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1789=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1789[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1790; _temp1790.tag= Cyc_Parse_Decl_spec;
_temp1790.f1=({ struct Cyc_Absyn_Decl* _temp1791=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1791->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1792=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1792[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1793; _temp1793.tag= Cyc_Absyn_Enum_d; _temp1793.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1794=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1794->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1794->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1794->fields=({
struct Cyc_Core_Opt* _temp1795=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1795->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1795;});
_temp1794;}); _temp1793;}); _temp1792;})); _temp1791->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1791;}); _temp1790;}); _temp1789;})); _temp1788;}); _temp1787;}); break;
case 75: _LL1786: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1797=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1797[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1798; _temp1798.tag=
Cyc_TypeSpecifier_tok; _temp1798.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1799=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1799[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1800; _temp1800.tag= Cyc_Absyn_EnumType;
_temp1800.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1800.f2= 0; _temp1800;}); _temp1799;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1798;}); _temp1797;}); break; case 76: _LL1796: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1802=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1802[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1803; _temp1803.tag= Cyc_Enumfield_tok;
_temp1803.f1=({ struct Cyc_Absyn_Enumfield* _temp1804=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1804->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1804->tag= 0;
_temp1804->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1804;});
_temp1803;}); _temp1802;}); break; case 77: _LL1801: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1806=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1806[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1807; _temp1807.tag= Cyc_Enumfield_tok;
_temp1807.f1=({ struct Cyc_Absyn_Enumfield* _temp1808=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1808->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1808->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1808->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1808;});
_temp1807;}); _temp1806;}); break; case 78: _LL1805: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1810=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1810[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1811; _temp1811.tag=
Cyc_EnumfieldList_tok; _temp1811.f1=({ struct Cyc_List_List* _temp1812=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1812->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1812->tl= 0; _temp1812;}); _temp1811;}); _temp1810;});
break; case 79: _LL1809: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1814=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1814[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1815; _temp1815.tag=
Cyc_EnumfieldList_tok; _temp1815.f1=({ struct Cyc_List_List* _temp1816=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1816->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1816->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1816;}); _temp1815;});
_temp1814;}); break; case 80: _LL1813: { void* t;{ void* _temp1818= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1820: if(
_temp1818 ==( void*) Cyc_Parse_Struct_su){ goto _LL1821;} else{ goto _LL1822;}
_LL1822: if( _temp1818 ==( void*) Cyc_Parse_Union_su){ goto _LL1823;} else{ goto
_LL1819;} _LL1821: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1824=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1824[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1825; _temp1825.tag= Cyc_Absyn_AnonStructType; _temp1825.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1825;});
_temp1824;}); goto _LL1819; _LL1823: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1826=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1826[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1827; _temp1827.tag= Cyc_Absyn_AnonUnionType; _temp1827.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1827;});
_temp1826;}); goto _LL1819; _LL1819:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1828=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1828[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1829; _temp1829.tag=
Cyc_TypeSpecifier_tok; _temp1829.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1830=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1830[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1831; _temp1831.tag= Cyc_Parse_Type_spec;
_temp1831.f1=( void*) t; _temp1831.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1831;});
_temp1830;})); _temp1829;}); _temp1828;}); break;} case 81: _LL1817: { struct
Cyc_List_List* _temp1833=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1834= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1836: if(
_temp1834 ==( void*) Cyc_Parse_Struct_su){ goto _LL1837;} else{ goto _LL1838;}
_LL1838: if( _temp1834 ==( void*) Cyc_Parse_Union_su){ goto _LL1839;} else{ goto
_LL1835;} _LL1837: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1840=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1840->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1840;}), _temp1833,({ struct Cyc_Core_Opt*
_temp1841=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1841->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1841;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1835; _LL1839: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1842=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1842->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1842;}),
_temp1833,({ struct Cyc_Core_Opt* _temp1843=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1843->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1843;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1835; _LL1835:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1844=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1844[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1845; _temp1845.tag= Cyc_TypeSpecifier_tok;
_temp1845.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1846=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1846[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1847; _temp1847.tag= Cyc_Parse_Decl_spec;
_temp1847.f1= d; _temp1847;}); _temp1846;})); _temp1845;}); _temp1844;}); break;}
case 82: _LL1832: { struct Cyc_List_List* _temp1849=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1850= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1852: if(
_temp1850 ==( void*) Cyc_Parse_Struct_su){ goto _LL1853;} else{ goto _LL1854;}
_LL1854: if( _temp1850 ==( void*) Cyc_Parse_Union_su){ goto _LL1855;} else{ goto
_LL1851;} _LL1853: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1856=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1856->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1856;}), _temp1849,({ struct Cyc_Core_Opt*
_temp1857=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1857->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1857;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1851; _LL1855: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1858=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1858->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1858;}),
_temp1849,({ struct Cyc_Core_Opt* _temp1859=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1859->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1859;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1851; _LL1851:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1860=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1860[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1861; _temp1861.tag= Cyc_TypeSpecifier_tok;
_temp1861.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1862=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1862[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1863; _temp1863.tag= Cyc_Parse_Decl_spec;
_temp1863.f1= d; _temp1863;}); _temp1862;})); _temp1861;}); _temp1860;}); break;}
case 83: _LL1848:{ void* _temp1865= Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1867: if(
_temp1865 ==( void*) Cyc_Parse_Struct_su){ goto _LL1868;} else{ goto _LL1869;}
_LL1869: if( _temp1865 ==( void*) Cyc_Parse_Union_su){ goto _LL1870;} else{ goto
_LL1866;} _LL1868: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1871=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1871[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1872; _temp1872.tag=
Cyc_TypeSpecifier_tok; _temp1872.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1873=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1873[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1874; _temp1874.tag= Cyc_Absyn_StructType;
_temp1874.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1874.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1874.f3= 0;
_temp1874;}); _temp1873;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1872;});
_temp1871;}); goto _LL1866; _LL1870: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1875=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1875[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1876; _temp1876.tag=
Cyc_TypeSpecifier_tok; _temp1876.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1877=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1877[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1878; _temp1878.tag= Cyc_Absyn_UnionType;
_temp1878.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1878.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1878.f3= 0;
_temp1878;}); _temp1877;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1876;});
_temp1875;}); goto _LL1866; _LL1866:;} break; case 84: _LL1864:{ void* _temp1880=
Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL1882: if( _temp1880 ==( void*) Cyc_Parse_Struct_su){
goto _LL1883;} else{ goto _LL1884;} _LL1884: if( _temp1880 ==( void*) Cyc_Parse_Union_su){
goto _LL1885;} else{ goto _LL1881;} _LL1883: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1886=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1886[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1887; _temp1887.tag=
Cyc_TypeSpecifier_tok; _temp1887.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1888=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1888[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1889; _temp1889.tag= Cyc_Absyn_StructType;
_temp1889.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1889.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1889.f3= 0;
_temp1889;}); _temp1888;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1887;});
_temp1886;}); goto _LL1881; _LL1885: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1890=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1890[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1891; _temp1891.tag=
Cyc_TypeSpecifier_tok; _temp1891.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1892=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1892[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1893; _temp1893.tag= Cyc_Absyn_UnionType;
_temp1893.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1893.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1893.f3= 0;
_temp1893;}); _temp1892;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1891;});
_temp1890;}); goto _LL1881; _LL1881:;} break; case 85: _LL1879: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1895=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1895[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1896; _temp1896.tag= Cyc_TypeList_tok; _temp1896.f1= 0; _temp1896;});
_temp1895;}); break; case 86: _LL1894: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1898=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1898[ 0]=({ struct Cyc_TypeList_tok_struct _temp1899; _temp1899.tag= Cyc_TypeList_tok;
_temp1899.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1899;});
_temp1898;}); break; case 87: _LL1897:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1901=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1901[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1902; _temp1902.tag= Cyc_TypeList_tok; _temp1902.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1902;});
_temp1901;}); break; case 88: _LL1900: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1904=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1904[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1905; _temp1905.tag=
Cyc_StructOrUnion_tok; _temp1905.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp1905;}); _temp1904;}); break; case 89: _LL1903: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1907=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1907[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1908; _temp1908.tag=
Cyc_StructOrUnion_tok; _temp1908.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp1908;}); _temp1907;}); break; case 90: _LL1906: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1910=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1910[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1911; _temp1911.tag= Cyc_StructFieldDeclList_tok; _temp1911.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1911;});
_temp1910;}); break; case 91: _LL1909: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1913=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1913[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1914; _temp1914.tag= Cyc_StructFieldDeclListList_tok; _temp1914.f1=({
struct Cyc_List_List* _temp1915=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1915->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1915->tl= 0;
_temp1915;}); _temp1914;}); _temp1913;}); break; case 92: _LL1912: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1917=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1917[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1918; _temp1918.tag= Cyc_StructFieldDeclListList_tok;
_temp1918.f1=({ struct Cyc_List_List* _temp1919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1919->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1919->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1919;}); _temp1918;}); _temp1917;}); break;
case 93: _LL1916: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1921=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1921[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1922; _temp1922.tag=
Cyc_InitDeclList_tok; _temp1922.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1922;});
_temp1921;}); break; case 94: _LL1920: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1924=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1924[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1925; _temp1925.tag=
Cyc_InitDeclList_tok; _temp1925.f1=({ struct Cyc_List_List* _temp1926=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1926->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1926->tl= 0; _temp1926;}); _temp1925;}); _temp1924;});
break; case 95: _LL1923: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1928=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1928[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1929; _temp1929.tag=
Cyc_InitDeclList_tok; _temp1929.f1=({ struct Cyc_List_List* _temp1930=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1930->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1930->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1930;});
_temp1929;}); _temp1928;}); break; case 96: _LL1927: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1932=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1932[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1933; _temp1933.tag= Cyc_InitDecl_tok;
_temp1933.f1=({ struct _tuple15* _temp1934=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1934->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1934->f2= 0;
_temp1934;}); _temp1933;}); _temp1932;}); break; case 97: _LL1931: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1936=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1936[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1937; _temp1937.tag= Cyc_InitDecl_tok; _temp1937.f1=({ struct _tuple15*
_temp1938=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1938->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1938->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1938;});
_temp1937;}); _temp1936;}); break; case 98: _LL1935: { struct Cyc_Absyn_Tqual tq=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])).f1; struct Cyc_List_List* _temp1940=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3; void*
_temp1941= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
struct Cyc_List_List* _temp1944; struct Cyc_List_List* _temp1946; struct _tuple0
_temp1942=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _LL1947:
_temp1946= _temp1942.f1; goto _LL1945; _LL1945: _temp1944= _temp1942.f2; goto
_LL1943; _LL1943: { struct Cyc_List_List* _temp1948=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp1941, _temp1946, _temp1940), _temp1944); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1949=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1949[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1950; _temp1950.tag= Cyc_StructFieldDeclList_tok; _temp1950.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),
_temp1948); _temp1950;}); _temp1949;}); break;}} case 99: _LL1939: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1952=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1952[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1953; _temp1953.tag= Cyc_QualSpecList_tok;
_temp1953.f1=({ struct _tuple18* _temp1954=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1954->f1= Cyc_Absyn_empty_tqual(); _temp1954->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1955=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1955->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1955->tl=
0; _temp1955;}); _temp1954->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1954;}); _temp1953;});
_temp1952;}); break; case 100: _LL1951: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1957=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1957[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1958; _temp1958.tag=
Cyc_QualSpecList_tok; _temp1958.f1=({ struct _tuple18* _temp1959=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1959->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1959->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1960=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1960->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1960->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1960;}); _temp1959->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1959;});
_temp1958;}); _temp1957;}); break; case 101: _LL1956: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1962=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1962[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1963; _temp1963.tag= Cyc_QualSpecList_tok;
_temp1963.f1=({ struct _tuple18* _temp1964=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1964->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1964->f2= 0;
_temp1964->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1964;}); _temp1963;}); _temp1962;}); break; case
102: _LL1961: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1966=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1966[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1967; _temp1967.tag=
Cyc_QualSpecList_tok; _temp1967.f1=({ struct _tuple18* _temp1968=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1968->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1968->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1968->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1968;});
_temp1967;}); _temp1966;}); break; case 103: _LL1965: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1970=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1970[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1971; _temp1971.tag= Cyc_DeclaratorExpoptList_tok;
_temp1971.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1971;}); _temp1970;}); break; case 104: _LL1969: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1973=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1973[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1974; _temp1974.tag= Cyc_DeclaratorExpoptList_tok;
_temp1974.f1=({ struct Cyc_List_List* _temp1975=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1975->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1975->tl= 0;
_temp1975;}); _temp1974;}); _temp1973;}); break; case 105: _LL1972: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp1977=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1977[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1978; _temp1978.tag= Cyc_DeclaratorExpoptList_tok;
_temp1978.f1=({ struct Cyc_List_List* _temp1979=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1979->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1979->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1979;}); _temp1978;}); _temp1977;}); break; case 106:
_LL1976: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp1981=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp1981[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp1982; _temp1982.tag=
Cyc_DeclaratorExpopt_tok; _temp1982.f1=({ struct _tuple13* _temp1983=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1983->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1983->f2= 0;
_temp1983;}); _temp1982;}); _temp1981;}); break; case 107: _LL1980: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1985=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1985[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1986; _temp1986.tag= Cyc_DeclaratorExpopt_tok;
_temp1986.f1=({ struct _tuple13* _temp1987=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1987->f1=({ struct Cyc_Parse_Declarator*
_temp1989=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp1989->id=({ struct _tuple1* _temp1990=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp1990->f1= Cyc_Absyn_rel_ns_null; _temp1990->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1990;}); _temp1989->tms= 0; _temp1989;});
_temp1987->f2=({ struct Cyc_Core_Opt* _temp1988=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1988->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1988;});
_temp1987;}); _temp1986;}); _temp1985;}); break; case 108: _LL1984: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1992=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1992[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1993; _temp1993.tag= Cyc_DeclaratorExpopt_tok;
_temp1993.f1=({ struct _tuple13* _temp1994=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1994->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1994->f2=({
struct Cyc_Core_Opt* _temp1995=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1995->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1995;}); _temp1994;});
_temp1993;}); _temp1992;}); break; case 109: _LL1991: { struct Cyc_List_List*
_temp1997=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1998=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1998[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1999; _temp1999.tag= Cyc_TypeSpecifier_tok;
_temp1999.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2000=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2000[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2001; _temp2001.tag= Cyc_Parse_Decl_spec;
_temp2001.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp1997,({
struct Cyc_Core_Opt* _temp2002=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2002->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2002;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2001;}); _temp2000;})); _temp1999;}); _temp1998;}); break;} case 110:
_LL1996: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2004=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2004[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2005; _temp2005.tag=
Cyc_TypeSpecifier_tok; _temp2005.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2006=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2006[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2007; _temp2007.tag= Cyc_Absyn_TunionType;
_temp2007.f1=({ struct Cyc_Absyn_TunionInfo _temp2008; _temp2008.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2009=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2009[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2010; _temp2010.tag= Cyc_Absyn_UnknownTunion;
_temp2010.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2011; _temp2011.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2011.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2011;});
_temp2010;}); _temp2009;})); _temp2008.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2008.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp2008;}); _temp2007;}); _temp2006;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2005;}); _temp2004;}); break; case 111: _LL2003: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2013=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2013[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2014; _temp2014.tag= Cyc_TypeSpecifier_tok;
_temp2014.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2015=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2015[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2016; _temp2016.tag=
Cyc_Absyn_TunionType; _temp2016.f1=({ struct Cyc_Absyn_TunionInfo _temp2017;
_temp2017.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp2018=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp2018[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp2019; _temp2019.tag= Cyc_Absyn_UnknownTunion; _temp2019.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp2020; _temp2020.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2020.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2020;}); _temp2019;}); _temp2018;})); _temp2017.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2017.rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2017;}); _temp2016;}); _temp2015;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2014;}); _temp2013;}); break; case 112: _LL2012: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2022=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2022[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2023; _temp2023.tag= Cyc_TypeSpecifier_tok;
_temp2023.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2024=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2024[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2025; _temp2025.tag= Cyc_Absyn_TunionFieldType; _temp2025.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2026; _temp2026.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2027=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2027[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2028; _temp2028.tag= Cyc_Absyn_UnknownTunionfield; _temp2028.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2029; _temp2029.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2029.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2029.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2029;});
_temp2028;}); _temp2027;})); _temp2026.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2026;}); _temp2025;});
_temp2024;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2023;}); _temp2022;}); break; case 113:
_LL2021: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2031=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2031[ 0]=({ struct
Cyc_Bool_tok_struct _temp2032; _temp2032.tag= Cyc_Bool_tok; _temp2032.f1= 0;
_temp2032;}); _temp2031;}); break; case 114: _LL2030: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2034=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2034[ 0]=({ struct Cyc_Bool_tok_struct
_temp2035; _temp2035.tag= Cyc_Bool_tok; _temp2035.f1= 1; _temp2035;}); _temp2034;});
break; case 115: _LL2033: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2037=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2037[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2038; _temp2038.tag= Cyc_TunionFieldList_tok; _temp2038.f1=({ struct Cyc_List_List*
_temp2039=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2039->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2039->tl= 0;
_temp2039;}); _temp2038;}); _temp2037;}); break; case 116: _LL2036: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2041=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2041[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2042; _temp2042.tag= Cyc_TunionFieldList_tok;
_temp2042.f1=({ struct Cyc_List_List* _temp2043=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2043->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2043->tl=
0; _temp2043;}); _temp2042;}); _temp2041;}); break; case 117: _LL2040: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2045=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2045[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2046; _temp2046.tag= Cyc_TunionFieldList_tok;
_temp2046.f1=({ struct Cyc_List_List* _temp2047=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2047->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2047->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2047;}); _temp2046;}); _temp2045;}); break; case 118:
_LL2044: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2049=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2049[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2050; _temp2050.tag=
Cyc_TunionFieldList_tok; _temp2050.f1=({ struct Cyc_List_List* _temp2051=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2051->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2051->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2051;}); _temp2050;});
_temp2049;}); break; case 119: _LL2048: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2053=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2053[ 0]=({ struct Cyc_Scope_tok_struct _temp2054; _temp2054.tag= Cyc_Scope_tok;
_temp2054.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2054;}); _temp2053;});
break; case 120: _LL2052: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2056=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2056[ 0]=({ struct Cyc_Scope_tok_struct _temp2057; _temp2057.tag= Cyc_Scope_tok;
_temp2057.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2057;}); _temp2056;});
break; case 121: _LL2055: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2059=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2059[ 0]=({ struct Cyc_Scope_tok_struct _temp2060; _temp2060.tag= Cyc_Scope_tok;
_temp2060.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2060;}); _temp2059;});
break; case 122: _LL2058: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2062=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2062[ 0]=({ struct Cyc_TunionField_tok_struct _temp2063; _temp2063.tag= Cyc_TunionField_tok;
_temp2063.f1=({ struct Cyc_Absyn_Tunionfield* _temp2064=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2064->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2064->tvs= 0;
_temp2064->typs= 0; _temp2064->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2064->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2064;}); _temp2063;}); _temp2062;}); break; case 123:
_LL2061: { struct Cyc_List_List* _temp2066=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2067=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2068=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2068[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2069; _temp2069.tag= Cyc_TunionField_tok;
_temp2069.f1=({ struct Cyc_Absyn_Tunionfield* _temp2070=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2070->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2070->tvs=
_temp2067; _temp2070->typs= _temp2066; _temp2070->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2070->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2070;}); _temp2069;}); _temp2068;}); break;}
case 124: _LL2065: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL2071: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2073=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2073[ 0]=({ struct Cyc_Declarator_tok_struct _temp2074; _temp2074.tag= Cyc_Declarator_tok;
_temp2074.f1=({ struct Cyc_Parse_Declarator* _temp2075=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2075->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2075->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2075;});
_temp2074;}); _temp2073;}); break; case 126: _LL2072: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2077=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2077[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2078; _temp2078.tag= Cyc_Declarator_tok;
_temp2078.f1=({ struct Cyc_Parse_Declarator* _temp2079=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2079->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2079->tms= 0;
_temp2079;}); _temp2078;}); _temp2077;}); break; case 127: _LL2076: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 128:
_LL2080: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2082=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2082[ 0]=({ struct Cyc_Declarator_tok_struct _temp2083; _temp2083.tag= Cyc_Declarator_tok;
_temp2083.f1=({ struct Cyc_Parse_Declarator* _temp2084=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2084->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2084->tms=({ struct Cyc_List_List* _temp2085=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2085->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2085->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2085;}); _temp2084;}); _temp2083;});
_temp2082;}); break; case 129: _LL2081: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2087=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2087[ 0]=({ struct Cyc_Declarator_tok_struct _temp2088; _temp2088.tag= Cyc_Declarator_tok;
_temp2088.f1=({ struct Cyc_Parse_Declarator* _temp2089=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2089->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2089->tms=({ struct Cyc_List_List* _temp2090=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2090->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2091=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2091[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2092; _temp2092.tag= Cyc_Absyn_ConstArray_mod;
_temp2092.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2092;}); _temp2091;})); _temp2090->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2090;}); _temp2089;}); _temp2088;}); _temp2087;}); break; case 130: _LL2086: {
struct _tuple17 _temp2096; struct Cyc_List_List* _temp2097; struct Cyc_Core_Opt*
_temp2099; struct Cyc_Absyn_VarargInfo* _temp2101; int _temp2103; struct Cyc_List_List*
_temp2105; struct _tuple17* _temp2094= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2096=*
_temp2094; _LL2106: _temp2105= _temp2096.f1; goto _LL2104; _LL2104: _temp2103=
_temp2096.f2; goto _LL2102; _LL2102: _temp2101= _temp2096.f3; goto _LL2100;
_LL2100: _temp2099= _temp2096.f4; goto _LL2098; _LL2098: _temp2097= _temp2096.f5;
goto _LL2095; _LL2095: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2107=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2107[ 0]=({ struct Cyc_Declarator_tok_struct _temp2108; _temp2108.tag= Cyc_Declarator_tok;
_temp2108.f1=({ struct Cyc_Parse_Declarator* _temp2109=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2109->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2109->tms=({ struct Cyc_List_List* _temp2110=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2110->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2111=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2111[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2112; _temp2112.tag= Cyc_Absyn_Function_mod;
_temp2112.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2113=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2113[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2114; _temp2114.tag= Cyc_Absyn_WithTypes;
_temp2114.f1= _temp2105; _temp2114.f2= _temp2103; _temp2114.f3= _temp2101;
_temp2114.f4= _temp2099; _temp2114.f5= _temp2097; _temp2114;}); _temp2113;}));
_temp2112;}); _temp2111;})); _temp2110->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2110;});
_temp2109;}); _temp2108;}); _temp2107;}); break;} case 131: _LL2093: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2116=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2116[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2117; _temp2117.tag= Cyc_Declarator_tok;
_temp2117.f1=({ struct Cyc_Parse_Declarator* _temp2118=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2118->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2118->tms=({ struct Cyc_List_List* _temp2119=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2119->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2120=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2120[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2121; _temp2121.tag= Cyc_Absyn_Function_mod;
_temp2121.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2122=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2122[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2123; _temp2123.tag= Cyc_Absyn_WithTypes;
_temp2123.f1= 0; _temp2123.f2= 0; _temp2123.f3= 0; _temp2123.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2123.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2123;}); _temp2122;})); _temp2121;}); _temp2120;}));
_temp2119->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2119;}); _temp2118;}); _temp2117;});
_temp2116;}); break; case 132: _LL2115: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2125=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2125[ 0]=({ struct Cyc_Declarator_tok_struct _temp2126; _temp2126.tag= Cyc_Declarator_tok;
_temp2126.f1=({ struct Cyc_Parse_Declarator* _temp2127=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2127->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2127->tms=({ struct Cyc_List_List* _temp2128=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2128->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2129=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2129[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2130; _temp2130.tag= Cyc_Absyn_Function_mod;
_temp2130.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2131=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2131[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2132; _temp2132.tag= Cyc_Absyn_NoTypes;
_temp2132.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2132.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2132;});
_temp2131;})); _temp2130;}); _temp2129;})); _temp2128->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2128;}); _temp2127;}); _temp2126;}); _temp2125;}); break; case 133: _LL2124: {
struct Cyc_List_List* _temp2134=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2135=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2135[ 0]=({ struct Cyc_Declarator_tok_struct _temp2136; _temp2136.tag= Cyc_Declarator_tok;
_temp2136.f1=({ struct Cyc_Parse_Declarator* _temp2137=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2137->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2137->tms=({ struct Cyc_List_List* _temp2138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2138->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2139=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2139[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2140; _temp2140.tag= Cyc_Absyn_TypeParams_mod;
_temp2140.f1= _temp2134; _temp2140.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2140.f3=
0; _temp2140;}); _temp2139;})); _temp2138->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2138;});
_temp2137;}); _temp2136;}); _temp2135;}); break;} case 134: _LL2133:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2142=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2143=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2143[ 0]=({ struct Cyc_Declarator_tok_struct _temp2144; _temp2144.tag= Cyc_Declarator_tok;
_temp2144.f1=({ struct Cyc_Parse_Declarator* _temp2145=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2145->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2145->tms=({ struct Cyc_List_List* _temp2146=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2146->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2147=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2147[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2148; _temp2148.tag= Cyc_Absyn_TypeParams_mod;
_temp2148.f1= _temp2142; _temp2148.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2148.f3=
0; _temp2148;}); _temp2147;})); _temp2146->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2146;});
_temp2145;}); _temp2144;}); _temp2143;}); break;} case 135: _LL2141: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2150=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2150[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2151; _temp2151.tag= Cyc_Declarator_tok;
_temp2151.f1=({ struct Cyc_Parse_Declarator* _temp2152=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2152->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2152->tms=({ struct Cyc_List_List* _temp2153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2153->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2154=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2154[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2155; _temp2155.tag= Cyc_Absyn_Attributes_mod;
_temp2155.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2155.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2155;}); _temp2154;}));
_temp2153->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2153;}); _temp2152;}); _temp2151;});
_temp2150;}); break; case 136: _LL2149: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2157=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2157[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2158; _temp2158.tag= Cyc_Declarator_tok;
_temp2158.f1=({ struct Cyc_Parse_Declarator* _temp2159=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2159->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2159->tms= 0;
_temp2159;}); _temp2158;}); _temp2157;}); break; case 137: _LL2156: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2161=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2161[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2162; _temp2162.tag= Cyc_Declarator_tok;
_temp2162.f1=({ struct Cyc_Parse_Declarator* _temp2163=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2163->id= Cyc_Absyn_exn_name;
_temp2163->tms= 0; _temp2163;}); _temp2162;}); _temp2161;}); break; case 138:
_LL2160: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2165=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2165[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2166; _temp2166.tag=
Cyc_TypeModifierList_tok; _temp2166.f1=({ struct Cyc_List_List* _temp2167=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2167->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2168=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2168[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2169; _temp2169.tag= Cyc_Absyn_Pointer_mod;
_temp2169.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2169.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2169.f3= Cyc_Absyn_empty_tqual(); _temp2169;}); _temp2168;}));
_temp2167->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2167;});
_temp2166;}); _temp2165;}); break; case 139: _LL2164: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2171=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2171[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2172; _temp2172.tag= Cyc_TypeModifierList_tok;
_temp2172.f1=({ struct Cyc_List_List* _temp2173=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2173->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2174=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2174[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2175; _temp2175.tag= Cyc_Absyn_Pointer_mod;
_temp2175.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2175.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2175.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2175;}); _temp2174;})); _temp2173->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2173;});
_temp2172;}); _temp2171;}); break; case 140: _LL2170: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2177=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2177[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2178; _temp2178.tag= Cyc_TypeModifierList_tok;
_temp2178.f1=({ struct Cyc_List_List* _temp2179=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2179->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2180=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2180[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2181; _temp2181.tag= Cyc_Absyn_Pointer_mod;
_temp2181.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2181.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2181.f3= Cyc_Absyn_empty_tqual(); _temp2181;}); _temp2180;}));
_temp2179->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2179;});
_temp2178;}); _temp2177;}); break; case 141: _LL2176: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2183=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2183[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2184; _temp2184.tag= Cyc_TypeModifierList_tok;
_temp2184.f1=({ struct Cyc_List_List* _temp2185=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2185->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2186=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2186[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2187; _temp2187.tag= Cyc_Absyn_Pointer_mod;
_temp2187.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2187.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2187.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2187;}); _temp2186;})); _temp2185->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2185;});
_temp2184;}); _temp2183;}); break; case 142: _LL2182: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2189=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2189[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2190; _temp2190.tag= Cyc_Pointer_Sort_tok;
_temp2190.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2191=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2191[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2192; _temp2192.tag=
Cyc_Absyn_Nullable_ps; _temp2192.f1= Cyc_Absyn_exp_unsigned_one; _temp2192;});
_temp2191;})); _temp2190;}); _temp2189;}); break; case 143: _LL2188: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2194=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2194[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2195; _temp2195.tag= Cyc_Pointer_Sort_tok;
_temp2195.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2196=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2196[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2197; _temp2197.tag= Cyc_Absyn_NonNullable_ps; _temp2197.f1= Cyc_Absyn_exp_unsigned_one;
_temp2197;}); _temp2196;})); _temp2195;}); _temp2194;}); break; case 144:
_LL2193: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2199=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2199[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2200; _temp2200.tag=
Cyc_Pointer_Sort_tok; _temp2200.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2201=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2201[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2202; _temp2202.tag=
Cyc_Absyn_Nullable_ps; _temp2202.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2202;});
_temp2201;})); _temp2200;}); _temp2199;}); break; case 145: _LL2198: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2204=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2204[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2205; _temp2205.tag= Cyc_Pointer_Sort_tok;
_temp2205.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2206=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2206[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2207; _temp2207.tag= Cyc_Absyn_NonNullable_ps; _temp2207.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2207;});
_temp2206;})); _temp2205;}); _temp2204;}); break; case 146: _LL2203: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2209=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2209[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2210; _temp2210.tag= Cyc_Pointer_Sort_tok;
_temp2210.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2210;});
_temp2209;}); break; case 147: _LL2208: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2212=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2212[ 0]=({ struct Cyc_Rgn_tok_struct _temp2213; _temp2213.tag= Cyc_Rgn_tok;
_temp2213.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2213;}); _temp2212;});
break; case 148: _LL2211: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2214: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2216=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2216[ 0]=({ struct Cyc_Rgn_tok_struct _temp2217; _temp2217.tag= Cyc_Rgn_tok;
_temp2217.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2217;}); _temp2216;}); break; case 150: _LL2215:
if( Cyc_yyget_Kind_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind\n",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Rgn_tok_struct* _temp2219=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp2219[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2220; _temp2220.tag= Cyc_Rgn_tok; _temp2220.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind)); _temp2220;});
_temp2219;}); break; case 151: _LL2218: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2222=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2222[ 0]=({ struct Cyc_Rgn_tok_struct _temp2223; _temp2223.tag= Cyc_Rgn_tok;
_temp2223.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2224=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2224->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2224;}), 0); _temp2223;}); _temp2222;});
break; case 152: _LL2221: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 153: _LL2225: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2227=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2227[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2228; _temp2228.tag= Cyc_TypeQual_tok; _temp2228.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2228;});
_temp2227;}); break; case 154: _LL2226: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2230=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2230[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2231; _temp2231.tag= Cyc_ParamDeclListBool_tok; _temp2231.f1=({ struct
_tuple17* _temp2232=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2232->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2232->f2= 0; _temp2232->f3= 0; _temp2232->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2232->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2232;}); _temp2231;}); _temp2230;}); break; case 155:
_LL2229: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2234=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2234[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2235; _temp2235.tag=
Cyc_ParamDeclListBool_tok; _temp2235.f1=({ struct _tuple17* _temp2236=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2236->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2236->f2= 1;
_temp2236->f3= 0; _temp2236->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2236->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2236;});
_temp2235;}); _temp2234;}); break; case 156: _LL2233: { void* _temp2240; struct
Cyc_Absyn_Tqual _temp2242; struct Cyc_Core_Opt* _temp2244; struct _tuple2
_temp2238=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL2245: _temp2244= _temp2238.f1; goto _LL2243; _LL2243:
_temp2242= _temp2238.f2; goto _LL2241; _LL2241: _temp2240= _temp2238.f3; goto
_LL2239; _LL2239: { struct Cyc_Absyn_VarargInfo* _temp2246=({ struct Cyc_Absyn_VarargInfo*
_temp2250=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2250->name= _temp2244; _temp2250->tq= _temp2242; _temp2250->type=( void*)
_temp2240; _temp2250->rgn=( void*) Cyc_yyget_Rgn_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2250->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2250;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2247=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2247[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2248; _temp2248.tag= Cyc_ParamDeclListBool_tok; _temp2248.f1=({ struct
_tuple17* _temp2249=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2249->f1= 0; _temp2249->f2= 0; _temp2249->f3= _temp2246; _temp2249->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2249->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2249;}); _temp2248;}); _temp2247;}); break;}} case 157:
_LL2237: { void* _temp2254; struct Cyc_Absyn_Tqual _temp2256; struct Cyc_Core_Opt*
_temp2258; struct _tuple2 _temp2252=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL2259: _temp2258=
_temp2252.f1; goto _LL2257; _LL2257: _temp2256= _temp2252.f2; goto _LL2255;
_LL2255: _temp2254= _temp2252.f3; goto _LL2253; _LL2253: { struct Cyc_Absyn_VarargInfo*
_temp2260=({ struct Cyc_Absyn_VarargInfo* _temp2264=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2264->name= _temp2258;
_temp2264->tq= _temp2256; _temp2264->type=( void*) _temp2254; _temp2264->rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2264->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2264;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2261=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2261[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2262; _temp2262.tag= Cyc_ParamDeclListBool_tok;
_temp2262.f1=({ struct _tuple17* _temp2263=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2263->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)])); _temp2263->f2= 0;
_temp2263->f3= _temp2260; _temp2263->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2263->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2263;});
_temp2262;}); _temp2261;}); break;}} case 158: _LL2251: yyval=( void*)({ struct
Cyc_TypeOpt_tok_struct* _temp2266=( struct Cyc_TypeOpt_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2266[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2267; _temp2267.tag= Cyc_TypeOpt_tok; _temp2267.f1= 0; _temp2267;});
_temp2266;}); break; case 159: _LL2265: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2269=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2269[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2270; _temp2270.tag= Cyc_TypeOpt_tok;
_temp2270.f1=({ struct Cyc_Core_Opt* _temp2271=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2271->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2272=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2272[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2273; _temp2273.tag= Cyc_Absyn_JoinEff;
_temp2273.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2273;}); _temp2272;})); _temp2271;}); _temp2270;});
_temp2269;}); break; case 160: _LL2268: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2275=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2275[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2276; _temp2276.tag= Cyc_Rgnorder_tok;
_temp2276.f1= 0; _temp2276;}); _temp2275;}); break; case 161: _LL2274: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 162:
_LL2277: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2279=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2279[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2280; _temp2280.tag= Cyc_Rgnorder_tok; _temp2280.f1=({ struct Cyc_List_List*
_temp2281=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2281->hd=( void*)({ struct _tuple19* _temp2282=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2282->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2282->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2282;}); _temp2281->tl= 0; _temp2281;});
_temp2280;}); _temp2279;}); break; case 163: _LL2278: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2284=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2284[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2285; _temp2285.tag= Cyc_Rgnorder_tok; _temp2285.f1=({ struct Cyc_List_List*
_temp2286=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2286->hd=( void*)({ struct _tuple19* _temp2287=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2287->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2287->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2287;}); _temp2286->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2286;});
_temp2285;}); _temp2284;}); break; case 164: _LL2283: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2289=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2289[ 0]=({ struct Cyc_Bool_tok_struct
_temp2290; _temp2290.tag= Cyc_Bool_tok; _temp2290.f1= 0; _temp2290;}); _temp2289;});
break; case 165: _LL2288: { struct _tagged_arr _temp2292= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2292, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2293=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2293[ 0]=({ struct Cyc_Bool_tok_struct _temp2294; _temp2294.tag= Cyc_Bool_tok;
_temp2294.f1= 1; _temp2294;}); _temp2293;}); break;} case 166: _LL2291: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 167:
_LL2295: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2297=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2297[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2298; _temp2298.tag= Cyc_TypeList_tok; _temp2298.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2298;});
_temp2297;}); break; case 168: _LL2296: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2300=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2300[ 0]=({ struct Cyc_TypeList_tok_struct _temp2301; _temp2301.tag= Cyc_TypeList_tok;
_temp2301.f1= 0; _temp2301;}); _temp2300;}); break; case 169: _LL2299: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
170: _LL2302: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2304=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2304[ 0]=({ struct Cyc_TypeList_tok_struct _temp2305; _temp2305.tag= Cyc_TypeList_tok;
_temp2305.f1=({ struct Cyc_List_List* _temp2306=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2306->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2306->tl= 0; _temp2306;});
_temp2305;}); _temp2304;}); break; case 171: _LL2303: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_EffKind){
Cyc_Parse_err( _tag_arr("expecing effect kind (E)", sizeof( unsigned char), 25u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2308=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2308[ 0]=({ struct Cyc_TypeList_tok_struct _temp2309; _temp2309.tag= Cyc_TypeList_tok;
_temp2309.f1=({ struct Cyc_List_List* _temp2310=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2310->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2310->tl= 0;
_temp2310;}); _temp2309;}); _temp2308;}); break; case 172: _LL2307: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2312=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2312[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2313; _temp2313.tag= Cyc_TypeList_tok; _temp2313.f1=({ struct Cyc_List_List*
_temp2314=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2314->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2315=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2315[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2316; _temp2316.tag= Cyc_Absyn_AccessEff;
_temp2316.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2316;}); _temp2315;})); _temp2314->tl= 0;
_temp2314;}); _temp2313;}); _temp2312;}); break; case 173: _LL2311: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2318=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2318[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2319; _temp2319.tag= Cyc_TypeList_tok; _temp2319.f1=({ struct Cyc_List_List*
_temp2320=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2320->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2321=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2321[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2322; _temp2322.tag= Cyc_Absyn_AccessEff;
_temp2322.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2322;}); _temp2321;})); _temp2320->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2320;});
_temp2319;}); _temp2318;}); break; case 174: _LL2317: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2324=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2324[ 0]=({ struct Cyc_TypeList_tok_struct _temp2325; _temp2325.tag= Cyc_TypeList_tok;
_temp2325.f1=({ struct Cyc_List_List* _temp2326=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2326->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2327=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2327[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2328; _temp2328.tag= Cyc_Absyn_AccessEff;
_temp2328.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2328;}); _temp2327;})); _temp2326->tl= 0;
_temp2326;}); _temp2325;}); _temp2324;}); break; case 175: _LL2323: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2330=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2330[ 0]=({ struct Cyc_TypeList_tok_struct _temp2331; _temp2331.tag= Cyc_TypeList_tok;
_temp2331.f1=({ struct Cyc_List_List* _temp2332=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2332->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2333=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2333[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2334; _temp2334.tag= Cyc_Absyn_AccessEff;
_temp2334.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2334;}); _temp2333;})); _temp2332->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2332;});
_temp2331;}); _temp2330;}); break; case 176: _LL2329: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2336=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2336[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2337; _temp2337.tag= Cyc_ParamDeclList_tok;
_temp2337.f1=({ struct Cyc_List_List* _temp2338=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2338->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2338->tl= 0;
_temp2338;}); _temp2337;}); _temp2336;}); break; case 177: _LL2335: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2340=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2340[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2341; _temp2341.tag= Cyc_ParamDeclList_tok;
_temp2341.f1=({ struct Cyc_List_List* _temp2342=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2342->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2342->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2342;}); _temp2341;}); _temp2340;}); break; case 178:
_LL2339: { void* _temp2344= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2345=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2346=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2347=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2348= Cyc_Parse_apply_tms( _temp2346, _temp2344, _temp2345,
_temp2347); if( _temp2348.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
_tuple1* _temp2349=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id;{ void* _temp2350=(*
_temp2349).f1; struct Cyc_List_List* _temp2360; struct Cyc_List_List* _temp2362;
_LL2352: if( _temp2350 ==( void*) Cyc_Absyn_Loc_n){ goto _LL2353;} else{ goto
_LL2354;} _LL2354: if(( unsigned int) _temp2350 > 1u?*(( int*) _temp2350) == Cyc_Absyn_Rel_n:
0){ _LL2361: _temp2360=(( struct Cyc_Absyn_Rel_n_struct*) _temp2350)->f1; if(
_temp2360 == 0){ goto _LL2355;} else{ goto _LL2356;}} else{ goto _LL2356;}
_LL2356: if(( unsigned int) _temp2350 > 1u?*(( int*) _temp2350) == Cyc_Absyn_Abs_n:
0){ _LL2363: _temp2362=(( struct Cyc_Absyn_Abs_n_struct*) _temp2350)->f1; if(
_temp2362 == 0){ goto _LL2357;} else{ goto _LL2358;}} else{ goto _LL2358;}
_LL2358: goto _LL2359; _LL2353: goto _LL2351; _LL2355: goto _LL2351; _LL2357:
goto _LL2351; _LL2359: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); goto
_LL2351; _LL2351:;}{ struct Cyc_Core_Opt* _temp2364=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2368=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2368->v=( void*)(* _temp2349).f2; _temp2368;}); if(
_temp2348.f4 != 0){ Cyc_Parse_warn( _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2348.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2365=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2365[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2366; _temp2366.tag= Cyc_ParamDecl_tok; _temp2366.f1=({
struct _tuple2* _temp2367=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2367->f1= _temp2364; _temp2367->f2= _temp2348.f1; _temp2367->f3= _temp2348.f2;
_temp2367;}); _temp2366;}); _temp2365;}); break;}}} case 179: _LL2343: { void*
_temp2370= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2371=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; struct Cyc_Absyn_Tqual _temp2372=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; if( _temp2371
!= 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring", sizeof(
unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2373=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2373[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2374; _temp2374.tag= Cyc_ParamDecl_tok; _temp2374.f1=({
struct _tuple2* _temp2375=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2375->f1= 0; _temp2375->f2= _temp2372; _temp2375->f3= _temp2370; _temp2375;});
_temp2374;}); _temp2373;}); break;} case 180: _LL2369: { void* _temp2377= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2378=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2379=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2380=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2381= Cyc_Parse_apply_tms( _temp2379, _temp2377, _temp2378,
_temp2380); if( _temp2381.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2381.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2382=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2382[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2383; _temp2383.tag= Cyc_ParamDecl_tok; _temp2383.f1=({
struct _tuple2* _temp2384=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2384->f1= 0; _temp2384->f2= _temp2381.f1; _temp2384->f3= _temp2381.f2;
_temp2384;}); _temp2383;}); _temp2382;}); break;} case 181: _LL2376: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2386=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2386[ 0]=({ struct Cyc_IdList_tok_struct
_temp2387; _temp2387.tag= Cyc_IdList_tok; _temp2387.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2387;});
_temp2386;}); break; case 182: _LL2385: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2389=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2389[ 0]=({ struct Cyc_IdList_tok_struct _temp2390; _temp2390.tag= Cyc_IdList_tok;
_temp2390.f1=({ struct Cyc_List_List* _temp2391=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2391->hd=( void*)({ struct
_tagged_arr* _temp2392=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2392[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2392;}); _temp2391->tl=
0; _temp2391;}); _temp2390;}); _temp2389;}); break; case 183: _LL2388: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2394=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2394[ 0]=({ struct Cyc_IdList_tok_struct
_temp2395; _temp2395.tag= Cyc_IdList_tok; _temp2395.f1=({ struct Cyc_List_List*
_temp2396=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2396->hd=( void*)({ struct _tagged_arr* _temp2397=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2397[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2397;});
_temp2396->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2396;}); _temp2395;}); _temp2394;}); break; case 184:
_LL2393: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 185: _LL2398: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 186: _LL2399: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2401=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2401[ 0]=({ struct Cyc_Exp_tok_struct _temp2402; _temp2402.tag= Cyc_Exp_tok;
_temp2402.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2403=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2403[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2404; _temp2404.tag= Cyc_Absyn_UnresolvedMem_e; _temp2404.f1= 0; _temp2404.f2=
0; _temp2404;}); _temp2403;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2402;});
_temp2401;}); break; case 187: _LL2400: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2406=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2406[ 0]=({ struct Cyc_Exp_tok_struct _temp2407; _temp2407.tag= Cyc_Exp_tok;
_temp2407.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2408=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2408[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2409; _temp2409.tag= Cyc_Absyn_UnresolvedMem_e; _temp2409.f1= 0; _temp2409.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2409;});
_temp2408;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2407;}); _temp2406;}); break; case 188:
_LL2405: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2411=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2411[ 0]=({ struct Cyc_Exp_tok_struct
_temp2412; _temp2412.tag= Cyc_Exp_tok; _temp2412.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2413=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2413[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2414; _temp2414.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2414.f1= 0; _temp2414.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2414;});
_temp2413;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2412;}); _temp2411;}); break; case 189:
_LL2410: { struct Cyc_Absyn_Vardecl* _temp2416= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2422=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2422->f1=( void*) Cyc_Absyn_Loc_n; _temp2422->f2=({ struct _tagged_arr*
_temp2423=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp2423[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2423;}); _temp2422;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2416->tq=({ struct Cyc_Absyn_Tqual _temp2417; _temp2417.q_const= 1;
_temp2417.q_volatile= 0; _temp2417.q_restrict= 1; _temp2417;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2418=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2418[ 0]=({ struct Cyc_Exp_tok_struct
_temp2419; _temp2419.tag= Cyc_Exp_tok; _temp2419.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2420=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2420[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2421; _temp2421.tag= Cyc_Absyn_Comprehension_e;
_temp2421.f1= _temp2416; _temp2421.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2421.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2421;});
_temp2420;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2419;}); _temp2418;}); break;} case
190: _LL2415: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2425=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2425[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2426; _temp2426.tag=
Cyc_InitializerList_tok; _temp2426.f1=({ struct Cyc_List_List* _temp2427=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2427->hd=(
void*)({ struct _tuple20* _temp2428=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2428->f1= 0; _temp2428->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2428;}); _temp2427->tl=
0; _temp2427;}); _temp2426;}); _temp2425;}); break; case 191: _LL2424: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2430=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2430[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2431; _temp2431.tag= Cyc_InitializerList_tok;
_temp2431.f1=({ struct Cyc_List_List* _temp2432=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2432->hd=( void*)({ struct
_tuple20* _temp2433=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2433->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2433->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2433;});
_temp2432->tl= 0; _temp2432;}); _temp2431;}); _temp2430;}); break; case 192:
_LL2429: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2435=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2435[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2436; _temp2436.tag=
Cyc_InitializerList_tok; _temp2436.f1=({ struct Cyc_List_List* _temp2437=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2437->hd=(
void*)({ struct _tuple20* _temp2438=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2438->f1= 0; _temp2438->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2438;}); _temp2437->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2437;}); _temp2436;}); _temp2435;}); break; case 193:
_LL2434: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2440=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2440[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2441; _temp2441.tag=
Cyc_InitializerList_tok; _temp2441.f1=({ struct Cyc_List_List* _temp2442=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2442->hd=(
void*)({ struct _tuple20* _temp2443=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2443->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2443->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2443;});
_temp2442->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2442;});
_temp2441;}); _temp2440;}); break; case 194: _LL2439: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2445=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2445[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2446; _temp2446.tag= Cyc_DesignatorList_tok;
_temp2446.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2446;});
_temp2445;}); break; case 195: _LL2444: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2448=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2448[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2449; _temp2449.tag=
Cyc_DesignatorList_tok; _temp2449.f1=({ struct Cyc_List_List* _temp2450=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2450->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2450->tl= 0; _temp2450;}); _temp2449;}); _temp2448;});
break; case 196: _LL2447: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2452=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2452[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2453; _temp2453.tag=
Cyc_DesignatorList_tok; _temp2453.f1=({ struct Cyc_List_List* _temp2454=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2454->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2454->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2454;});
_temp2453;}); _temp2452;}); break; case 197: _LL2451: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2456=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2456[ 0]=({ struct
Cyc_Designator_tok_struct _temp2457; _temp2457.tag= Cyc_Designator_tok;
_temp2457.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2458=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2458[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2459; _temp2459.tag=
Cyc_Absyn_ArrayElement; _temp2459.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2459;});
_temp2458;})); _temp2457;}); _temp2456;}); break; case 198: _LL2455: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2461=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2461[ 0]=({ struct
Cyc_Designator_tok_struct _temp2462; _temp2462.tag= Cyc_Designator_tok;
_temp2462.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2463=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2463[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2464; _temp2464.tag= Cyc_Absyn_FieldName;
_temp2464.f1=({ struct _tagged_arr* _temp2465=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2465[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2465;}); _temp2464;});
_temp2463;})); _temp2462;}); _temp2461;}); break; case 199: _LL2460: { void*
_temp2467= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2468=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2468 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2469=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2470=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2470[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2471; _temp2471.tag= Cyc_ParamDecl_tok; _temp2471.f1=({
struct _tuple2* _temp2472=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp2472[ 0]=({ struct _tuple2 _temp2473; _temp2473.f1= 0; _temp2473.f2=
_temp2469; _temp2473.f3= _temp2467; _temp2473;}); _temp2472;}); _temp2471;});
_temp2470;}); break;}} case 200: _LL2466: { void* _temp2475= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2476=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2477=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2478=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2479= Cyc_Parse_apply_tms( _temp2477, _temp2475, _temp2476,
_temp2478); if( _temp2479.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2479.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2480=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2480[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2481; _temp2481.tag= Cyc_ParamDecl_tok; _temp2481.f1=({
struct _tuple2* _temp2482=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2482->f1= 0; _temp2482->f2= _temp2479.f1; _temp2482->f3= _temp2479.f2;
_temp2482;}); _temp2481;}); _temp2480;}); break;} case 201: _LL2474: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2484=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2484[ 0]=({ struct Cyc_Type_tok_struct
_temp2485; _temp2485.tag= Cyc_Type_tok; _temp2485.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2485;});
_temp2484;}); break; case 202: _LL2483: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2487=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2487[ 0]=({ struct Cyc_Type_tok_struct _temp2488; _temp2488.tag= Cyc_Type_tok;
_temp2488.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2489=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2489[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2490; _temp2490.tag= Cyc_Absyn_JoinEff;
_temp2490.f1= 0; _temp2490;}); _temp2489;})); _temp2488;}); _temp2487;}); break;
case 203: _LL2486: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2492=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2492[ 0]=({ struct Cyc_Type_tok_struct _temp2493; _temp2493.tag= Cyc_Type_tok;
_temp2493.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2494=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2494[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2495; _temp2495.tag= Cyc_Absyn_JoinEff;
_temp2495.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2495;}); _temp2494;})); _temp2493;});
_temp2492;}); break; case 204: _LL2491: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2497=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2497[ 0]=({ struct Cyc_Type_tok_struct _temp2498; _temp2498.tag= Cyc_Type_tok;
_temp2498.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2499=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2499[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2500; _temp2500.tag= Cyc_Absyn_JoinEff;
_temp2500.f1=({ struct Cyc_List_List* _temp2501=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2501->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2501->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2501;}); _temp2500;}); _temp2499;})); _temp2498;});
_temp2497;}); break; case 205: _LL2496: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2503=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2503[ 0]=({ struct Cyc_TypeList_tok_struct _temp2504; _temp2504.tag= Cyc_TypeList_tok;
_temp2504.f1=({ struct Cyc_List_List* _temp2505=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2505->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2505->tl= 0;
_temp2505;}); _temp2504;}); _temp2503;}); break; case 206: _LL2502: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2507=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2507[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2508; _temp2508.tag= Cyc_TypeList_tok; _temp2508.f1=({ struct Cyc_List_List*
_temp2509=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2509->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2509->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2509;});
_temp2508;}); _temp2507;}); break; case 207: _LL2506: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2511=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2511[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2512; _temp2512.tag= Cyc_AbstractDeclarator_tok;
_temp2512.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2513=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2513->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2513;});
_temp2512;}); _temp2511;}); break; case 208: _LL2510: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2514:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2516=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2516[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2517; _temp2517.tag=
Cyc_AbstractDeclarator_tok; _temp2517.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2518=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2518->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2518;});
_temp2517;}); _temp2516;}); break; case 210: _LL2515: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 211:
_LL2519: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2521=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2521[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2522; _temp2522.tag=
Cyc_AbstractDeclarator_tok; _temp2522.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2523=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2523->tms=({ struct Cyc_List_List* _temp2524=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2524->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2524->tl= 0; _temp2524;}); _temp2523;}); _temp2522;}); _temp2521;}); break;
case 212: _LL2520: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2526=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2526[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2527; _temp2527.tag= Cyc_AbstractDeclarator_tok; _temp2527.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2528=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2528->tms=({ struct Cyc_List_List* _temp2529=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2529->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2529->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2529;});
_temp2528;}); _temp2527;}); _temp2526;}); break; case 213: _LL2525: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2531=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2531[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2532; _temp2532.tag= Cyc_AbstractDeclarator_tok;
_temp2532.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2533=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2533->tms=({
struct Cyc_List_List* _temp2534=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2534->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2535=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2535[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2536; _temp2536.tag= Cyc_Absyn_ConstArray_mod; _temp2536.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2536;});
_temp2535;})); _temp2534->tl= 0; _temp2534;}); _temp2533;}); _temp2532;});
_temp2531;}); break; case 214: _LL2530: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2538=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2538[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2539; _temp2539.tag= Cyc_AbstractDeclarator_tok; _temp2539.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2540=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2540->tms=({ struct Cyc_List_List* _temp2541=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2541->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2542=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2542[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2543; _temp2543.tag= Cyc_Absyn_ConstArray_mod;
_temp2543.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2543;}); _temp2542;})); _temp2541->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2541;}); _temp2540;}); _temp2539;}); _temp2538;}); break; case 215: _LL2537:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2545=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2545[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2546; _temp2546.tag=
Cyc_AbstractDeclarator_tok; _temp2546.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2547=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2547->tms=({ struct Cyc_List_List* _temp2548=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2548->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2549=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2549[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2550; _temp2550.tag= Cyc_Absyn_Function_mod;
_temp2550.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2551=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2551[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2552; _temp2552.tag= Cyc_Absyn_WithTypes;
_temp2552.f1= 0; _temp2552.f2= 0; _temp2552.f3= 0; _temp2552.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2552.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2552;}); _temp2551;})); _temp2550;}); _temp2549;}));
_temp2548->tl= 0; _temp2548;}); _temp2547;}); _temp2546;}); _temp2545;}); break;
case 216: _LL2544: { struct _tuple17 _temp2556; struct Cyc_List_List* _temp2557;
struct Cyc_Core_Opt* _temp2559; struct Cyc_Absyn_VarargInfo* _temp2561; int
_temp2563; struct Cyc_List_List* _temp2565; struct _tuple17* _temp2554= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2556=*
_temp2554; _LL2566: _temp2565= _temp2556.f1; goto _LL2564; _LL2564: _temp2563=
_temp2556.f2; goto _LL2562; _LL2562: _temp2561= _temp2556.f3; goto _LL2560;
_LL2560: _temp2559= _temp2556.f4; goto _LL2558; _LL2558: _temp2557= _temp2556.f5;
goto _LL2555; _LL2555: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2567=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2567[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2568; _temp2568.tag= Cyc_AbstractDeclarator_tok; _temp2568.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2569=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2569->tms=({ struct Cyc_List_List* _temp2570=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2570->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2571=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2571[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2572; _temp2572.tag= Cyc_Absyn_Function_mod;
_temp2572.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2573=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2573[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2574; _temp2574.tag= Cyc_Absyn_WithTypes;
_temp2574.f1= _temp2565; _temp2574.f2= _temp2563; _temp2574.f3= _temp2561;
_temp2574.f4= _temp2559; _temp2574.f5= _temp2557; _temp2574;}); _temp2573;}));
_temp2572;}); _temp2571;})); _temp2570->tl= 0; _temp2570;}); _temp2569;});
_temp2568;}); _temp2567;}); break;} case 217: _LL2553: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2576=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2576[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2577; _temp2577.tag= Cyc_AbstractDeclarator_tok;
_temp2577.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2578=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2578->tms=({
struct Cyc_List_List* _temp2579=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2579->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2580=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2580[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2581; _temp2581.tag=
Cyc_Absyn_Function_mod; _temp2581.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2582=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2582[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2583; _temp2583.tag= Cyc_Absyn_WithTypes;
_temp2583.f1= 0; _temp2583.f2= 0; _temp2583.f3= 0; _temp2583.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2583.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2583;}); _temp2582;})); _temp2581;}); _temp2580;}));
_temp2579->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2579;});
_temp2578;}); _temp2577;}); _temp2576;}); break; case 218: _LL2575: { struct
_tuple17 _temp2587; struct Cyc_List_List* _temp2588; struct Cyc_Core_Opt*
_temp2590; struct Cyc_Absyn_VarargInfo* _temp2592; int _temp2594; struct Cyc_List_List*
_temp2596; struct _tuple17* _temp2585= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2587=*
_temp2585; _LL2597: _temp2596= _temp2587.f1; goto _LL2595; _LL2595: _temp2594=
_temp2587.f2; goto _LL2593; _LL2593: _temp2592= _temp2587.f3; goto _LL2591;
_LL2591: _temp2590= _temp2587.f4; goto _LL2589; _LL2589: _temp2588= _temp2587.f5;
goto _LL2586; _LL2586: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2598=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2598[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2599; _temp2599.tag= Cyc_AbstractDeclarator_tok; _temp2599.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2600=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2600->tms=({ struct Cyc_List_List* _temp2601=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2601->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2602=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2602[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2603; _temp2603.tag= Cyc_Absyn_Function_mod;
_temp2603.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2604=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2604[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2605; _temp2605.tag= Cyc_Absyn_WithTypes;
_temp2605.f1= _temp2596; _temp2605.f2= _temp2594; _temp2605.f3= _temp2592;
_temp2605.f4= _temp2590; _temp2605.f5= _temp2588; _temp2605;}); _temp2604;}));
_temp2603;}); _temp2602;})); _temp2601->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2601;}); _temp2600;}); _temp2599;}); _temp2598;}); break;} case 219:
_LL2584: { struct Cyc_List_List* _temp2607=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2608=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2608[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2609; _temp2609.tag= Cyc_AbstractDeclarator_tok; _temp2609.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2610=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2610->tms=({ struct Cyc_List_List* _temp2611=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2611->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2612=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2612[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2613; _temp2613.tag= Cyc_Absyn_TypeParams_mod;
_temp2613.f1= _temp2607; _temp2613.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2613.f3=
0; _temp2613;}); _temp2612;})); _temp2611->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2611;}); _temp2610;}); _temp2609;}); _temp2608;}); break;} case 220:
_LL2606:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2615=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2616=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2616[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2617; _temp2617.tag= Cyc_AbstractDeclarator_tok; _temp2617.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2618=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2618->tms=({ struct Cyc_List_List* _temp2619=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2619->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2620=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2620[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2621; _temp2621.tag= Cyc_Absyn_TypeParams_mod;
_temp2621.f1= _temp2615; _temp2621.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2621.f3=
0; _temp2621;}); _temp2620;})); _temp2619->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2619;}); _temp2618;}); _temp2617;}); _temp2616;}); break;} case 221:
_LL2614: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2623=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2623[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2624; _temp2624.tag=
Cyc_AbstractDeclarator_tok; _temp2624.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2625=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2625->tms=({ struct Cyc_List_List* _temp2626=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2626->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2627=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2627[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2628; _temp2628.tag= Cyc_Absyn_Attributes_mod;
_temp2628.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2628.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2628;}); _temp2627;}));
_temp2626->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms; _temp2626;});
_temp2625;}); _temp2624;}); _temp2623;}); break; case 222: _LL2622: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 223: _LL2629:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
224: _LL2630: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 225: _LL2631: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 226: _LL2632: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 227: _LL2633:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
228: _LL2634: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2646=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2646->name=({ struct
_tagged_arr* _temp2647=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2647[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2647;});
_temp2646->identity= 0; _temp2646->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2646;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2644=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2644[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2645; _temp2645.tag= Cyc_Absyn_VarType;
_temp2645.f1= tv; _temp2645;}); _temp2644;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2636=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2636[ 0]=({ struct Cyc_Stmt_tok_struct _temp2637; _temp2637.tag= Cyc_Stmt_tok;
_temp2637.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2638=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2638[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2639; _temp2639.tag= Cyc_Absyn_Region_s;
_temp2639.f1= tv; _temp2639.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2640=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2640->f1=(
void*) Cyc_Absyn_Loc_n; _temp2640->f2=({ struct _tagged_arr* _temp2641=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2641[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2641;});
_temp2640;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2642=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2642[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2643; _temp2643.tag=
Cyc_Absyn_RgnHandleType; _temp2643.f1=( void*) t; _temp2643;}); _temp2642;}), 0);
_temp2639.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2639;}); _temp2638;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2637;});
_temp2636;}); break;} case 229: _LL2635: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2659=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2659->name=({ struct
_tagged_arr* _temp2660=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2660[ 0]=( struct _tagged_arr)({ struct _tagged_arr(*
_temp2661)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp2665= _tag_arr("`%s", sizeof( unsigned char), 4u);
struct Cyc_Stdio_String_pa_struct _temp2666; _temp2666.tag= Cyc_Stdio_String_pa;
_temp2666.f1=( struct _tagged_arr) Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]);{ void* _temp2664=(
void*)& _temp2666; void* _temp2662[ 1u]={ _temp2664}; struct _tagged_arr
_temp2663={( void*) _temp2662,( void*) _temp2662,( void*)( _temp2662 + 1u)};
_temp2661( _temp2665, _temp2663);}}); _temp2660;}); _temp2659->identity= 0;
_temp2659->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind); _temp2659;});
void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2657=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2657[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2658; _temp2658.tag= Cyc_Absyn_VarType; _temp2658.f1=
tv; _temp2658;}); _temp2657;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2649=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2649[ 0]=({ struct Cyc_Stmt_tok_struct _temp2650; _temp2650.tag= Cyc_Stmt_tok;
_temp2650.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2651=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2651[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2652; _temp2652.tag= Cyc_Absyn_Region_s;
_temp2652.f1= tv; _temp2652.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2653=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2653->f1=(
void*) Cyc_Absyn_Loc_n; _temp2653->f2=({ struct _tagged_arr* _temp2654=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2654[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2654;});
_temp2653;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2655=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2655[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2656; _temp2656.tag=
Cyc_Absyn_RgnHandleType; _temp2656.f1=( void*) t; _temp2656;}); _temp2655;}), 0);
_temp2652.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2652;}); _temp2651;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2650;});
_temp2649;}); break;} case 230: _LL2648: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2668=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2668[ 0]=({ struct Cyc_Stmt_tok_struct _temp2669; _temp2669.tag= Cyc_Stmt_tok;
_temp2669.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2670=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2670[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2671; _temp2671.tag= Cyc_Absyn_Cut_s;
_temp2671.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2671;}); _temp2670;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2669;});
_temp2668;}); break; case 231: _LL2667: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2673=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2673[ 0]=({ struct Cyc_Stmt_tok_struct _temp2674; _temp2674.tag= Cyc_Stmt_tok;
_temp2674.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2675=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2675[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2676; _temp2676.tag= Cyc_Absyn_Splice_s;
_temp2676.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2676;}); _temp2675;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2674;});
_temp2673;}); break; case 232: _LL2672: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2678=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2678[ 0]=({ struct Cyc_Stmt_tok_struct _temp2679; _temp2679.tag= Cyc_Stmt_tok;
_temp2679.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2680=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2680[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2681; _temp2681.tag= Cyc_Absyn_Label_s;
_temp2681.f1=({ struct _tagged_arr* _temp2682=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2682[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2682;});
_temp2681.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2681;}); _temp2680;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2679;});
_temp2678;}); break; case 233: _LL2677: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2684=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2684[ 0]=({ struct Cyc_Stmt_tok_struct _temp2685; _temp2685.tag= Cyc_Stmt_tok;
_temp2685.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2685;});
_temp2684;}); break; case 234: _LL2683: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2687=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2687[ 0]=({ struct Cyc_Stmt_tok_struct _temp2688; _temp2688.tag= Cyc_Stmt_tok;
_temp2688.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2688;}); _temp2687;}); break; case 235: _LL2686: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2690=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2690[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2691; _temp2691.tag= Cyc_Stmt_tok; _temp2691.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2691;}); _temp2690;}); break; case 236: _LL2689: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 237:
_LL2692:{ void* _temp2694= Cyc_yyget_BlockItem_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); struct Cyc_List_List*
_temp2702; struct Cyc_Absyn_Fndecl* _temp2704; struct Cyc_Absyn_Stmt* _temp2706;
_LL2696: if(*(( int*) _temp2694) == Cyc_Parse_TopDecls_bl){ _LL2703: _temp2702=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2694)->f1; goto _LL2697;} else{ goto
_LL2698;} _LL2698: if(*(( int*) _temp2694) == Cyc_Parse_FnDecl_bl){ _LL2705:
_temp2704=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2694)->f1; goto _LL2699;}
else{ goto _LL2700;} _LL2700: if(*(( int*) _temp2694) == Cyc_Parse_Stmt_bl){
_LL2707: _temp2706=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2694)->f1; goto
_LL2701;} else{ goto _LL2695;} _LL2697: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2708=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2708[ 0]=({ struct Cyc_Stmt_tok_struct _temp2709; _temp2709.tag= Cyc_Stmt_tok;
_temp2709.f1= Cyc_Parse_flatten_declarations( _temp2702, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2709;}); _temp2708;}); goto _LL2695; _LL2699:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2710=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2710[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2711; _temp2711.tag= Cyc_Stmt_tok; _temp2711.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2712=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2712[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2713; _temp2713.tag= Cyc_Absyn_Fn_d;
_temp2713.f1= _temp2704; _temp2713;}); _temp2712;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2711;}); _temp2710;}); goto _LL2695; _LL2701:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2714=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2714[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2715; _temp2715.tag= Cyc_Stmt_tok; _temp2715.f1= _temp2706; _temp2715;});
_temp2714;}); goto _LL2695; _LL2695:;} break; case 238: _LL2693:{ void*
_temp2717= Cyc_yyget_BlockItem_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); struct Cyc_List_List* _temp2725; struct Cyc_Absyn_Fndecl*
_temp2727; struct Cyc_Absyn_Stmt* _temp2729; _LL2719: if(*(( int*) _temp2717) ==
Cyc_Parse_TopDecls_bl){ _LL2726: _temp2725=(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp2717)->f1; goto _LL2720;} else{ goto _LL2721;} _LL2721: if(*(( int*)
_temp2717) == Cyc_Parse_FnDecl_bl){ _LL2728: _temp2727=(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp2717)->f1; goto _LL2722;} else{ goto _LL2723;} _LL2723: if(*(( int*)
_temp2717) == Cyc_Parse_Stmt_bl){ _LL2730: _temp2729=(( struct Cyc_Parse_Stmt_bl_struct*)
_temp2717)->f1; goto _LL2724;} else{ goto _LL2718;} _LL2720: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2731=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2731[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2732; _temp2732.tag= Cyc_Stmt_tok; _temp2732.f1= Cyc_Parse_flatten_declarations(
_temp2725, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2732;}); _temp2731;}); goto _LL2718; _LL2722: yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2733=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2733[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2734; _temp2734.tag= Cyc_Stmt_tok; _temp2734.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2735=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2735[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2736; _temp2736.tag= Cyc_Absyn_Fn_d;
_temp2736.f1= _temp2727; _temp2736;}); _temp2735;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2734;}); _temp2733;}); goto _LL2718; _LL2724: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2737=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2737[ 0]=({ struct Cyc_Stmt_tok_struct _temp2738; _temp2738.tag= Cyc_Stmt_tok;
_temp2738.f1= Cyc_Absyn_seq_stmt( _temp2729, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2738;}); _temp2737;}); goto _LL2718; _LL2718:;} break; case 239: _LL2716:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2740=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp2740[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp2741; _temp2741.tag= Cyc_BlockItem_tok; _temp2741.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp2742=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp2742[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp2743; _temp2743.tag= Cyc_Parse_TopDecls_bl;
_temp2743.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2743;}); _temp2742;})); _temp2741;}); _temp2740;});
break; case 240: _LL2739: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp2745=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2745[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2746; _temp2746.tag= Cyc_BlockItem_tok;
_temp2746.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp2747=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp2747[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp2748; _temp2748.tag= Cyc_Parse_Stmt_bl;
_temp2748.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2748;}); _temp2747;})); _temp2746;}); _temp2745;}); break;
case 241: _LL2744: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2750=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2750[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2751; _temp2751.tag= Cyc_BlockItem_tok;
_temp2751.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp2752=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp2752[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp2753; _temp2753.tag= Cyc_Parse_FnDecl_bl;
_temp2753.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2753;}); _temp2752;})); _temp2751;}); _temp2750;}); break;
case 242: _LL2749: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2755=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2755[ 0]=({ struct Cyc_Stmt_tok_struct _temp2756; _temp2756.tag= Cyc_Stmt_tok;
_temp2756.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2756;}); _temp2755;}); break; case 243:
_LL2754: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2758=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2758[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2759; _temp2759.tag= Cyc_Stmt_tok; _temp2759.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2759;});
_temp2758;}); break; case 244: _LL2757: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2761=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2761[ 0]=({ struct Cyc_Stmt_tok_struct _temp2762; _temp2762.tag= Cyc_Stmt_tok;
_temp2762.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2762;}); _temp2761;}); break; case 245: _LL2760: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2764=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2764[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2765; _temp2765.tag= Cyc_Stmt_tok; _temp2765.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2766=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2766[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2767; _temp2767.tag= Cyc_Absyn_SwitchC_s;
_temp2767.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2767.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2767;});
_temp2766;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2765;}); _temp2764;}); break; case 246:
_LL2763: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2769=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2769[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2770; _temp2770.tag= Cyc_Stmt_tok; _temp2770.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2770;});
_temp2769;}); break; case 247: _LL2768: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2772=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2772[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2773; _temp2773.tag= Cyc_SwitchClauseList_tok; _temp2773.f1= 0; _temp2773;});
_temp2772;}); break; case 248: _LL2771: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2775=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2775[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2776; _temp2776.tag= Cyc_SwitchClauseList_tok; _temp2776.f1=({ struct Cyc_List_List*
_temp2777=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2777->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2778=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2778->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2778->pat_vars= 0; _temp2778->where_clause= 0; _temp2778->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2778->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2778;}); _temp2777->tl= 0; _temp2777;});
_temp2776;}); _temp2775;}); break; case 249: _LL2774: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2780=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2780[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2781; _temp2781.tag= Cyc_SwitchClauseList_tok;
_temp2781.f1=({ struct Cyc_List_List* _temp2782=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2782->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2783=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2783->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2783->pat_vars= 0; _temp2783->where_clause= 0;
_temp2783->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2783->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2783;});
_temp2782->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2782;}); _temp2781;});
_temp2780;}); break; case 250: _LL2779: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2785=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2785[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2786; _temp2786.tag= Cyc_SwitchClauseList_tok; _temp2786.f1=({ struct Cyc_List_List*
_temp2787=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2787->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2788=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2788->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2788->pat_vars=
0; _temp2788->where_clause= 0; _temp2788->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2788->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2788;}); _temp2787->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2787;}); _temp2786;});
_temp2785;}); break; case 251: _LL2784: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2790=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2790[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2791; _temp2791.tag= Cyc_SwitchClauseList_tok; _temp2791.f1=({ struct Cyc_List_List*
_temp2792=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2792->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2793=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2793->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2793->pat_vars=
0; _temp2793->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2793->body=
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2793->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2793;});
_temp2792->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2792;}); _temp2791;});
_temp2790;}); break; case 252: _LL2789: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2795=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2795[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2796; _temp2796.tag= Cyc_SwitchClauseList_tok; _temp2796.f1=({ struct Cyc_List_List*
_temp2797=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2797->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2798=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2798->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2798->pat_vars=
0; _temp2798->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2798->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2798->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2798;});
_temp2797->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2797;}); _temp2796;});
_temp2795;}); break; case 253: _LL2794: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2800=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2800[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2801; _temp2801.tag= Cyc_SwitchCClauseList_tok; _temp2801.f1= 0; _temp2801;});
_temp2800;}); break; case 254: _LL2799: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2803=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2803[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2804; _temp2804.tag= Cyc_SwitchCClauseList_tok; _temp2804.f1=({ struct Cyc_List_List*
_temp2805=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2805->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2806=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2806->cnst_exp= 0;
_temp2806->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2806->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2806;});
_temp2805->tl= 0; _temp2805;}); _temp2804;}); _temp2803;}); break; case 255:
_LL2802: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2808=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2808[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2809; _temp2809.tag=
Cyc_SwitchCClauseList_tok; _temp2809.f1=({ struct Cyc_List_List* _temp2810=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2810->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2811=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2811->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2811->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2811->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2811;});
_temp2810->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2810;}); _temp2809;});
_temp2808;}); break; case 256: _LL2807: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2813=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2813[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2814; _temp2814.tag= Cyc_SwitchCClauseList_tok; _temp2814.f1=({ struct Cyc_List_List*
_temp2815=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2815->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2816=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2816->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2816->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2816->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2816;});
_temp2815->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2815;}); _temp2814;});
_temp2813;}); break; case 257: _LL2812: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2818=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2818[ 0]=({ struct Cyc_Stmt_tok_struct _temp2819; _temp2819.tag= Cyc_Stmt_tok;
_temp2819.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2819;}); _temp2818;}); break; case 258: _LL2817: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2821=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2821[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2822; _temp2822.tag= Cyc_Stmt_tok; _temp2822.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2822;}); _temp2821;}); break; case 259: _LL2820: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2824=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2824[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2825; _temp2825.tag= Cyc_Stmt_tok; _temp2825.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2825;}); _temp2824;}); break; case 260: _LL2823: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2827=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2827[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2828; _temp2828.tag= Cyc_Stmt_tok; _temp2828.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2828;}); _temp2827;}); break; case 261: _LL2826: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2830=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2830[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2831; _temp2831.tag= Cyc_Stmt_tok; _temp2831.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2831;}); _temp2830;}); break; case 262: _LL2829: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2833=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2833[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2834; _temp2834.tag= Cyc_Stmt_tok; _temp2834.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2834;}); _temp2833;}); break; case 263: _LL2832: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2836=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2836[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2837; _temp2837.tag= Cyc_Stmt_tok; _temp2837.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2837;}); _temp2836;}); break; case 264: _LL2835: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2839=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2839[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2840; _temp2840.tag= Cyc_Stmt_tok; _temp2840.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2840;});
_temp2839;}); break; case 265: _LL2838: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2842=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2842[ 0]=({ struct Cyc_Stmt_tok_struct _temp2843; _temp2843.tag= Cyc_Stmt_tok;
_temp2843.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2843;});
_temp2842;}); break; case 266: _LL2841: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2845=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2845[ 0]=({ struct Cyc_Stmt_tok_struct _temp2846; _temp2846.tag= Cyc_Stmt_tok;
_temp2846.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2846;}); _temp2845;}); break; case 267: _LL2844: { struct Cyc_List_List*
_temp2848= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2849= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2850=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2850[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2851; _temp2851.tag= Cyc_Stmt_tok; _temp2851.f1= Cyc_Parse_flatten_declarations(
_temp2848, _temp2849); _temp2851;}); _temp2850;}); break;} case 268: _LL2847: {
struct Cyc_List_List* _temp2853= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2854= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2855=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2855[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2856; _temp2856.tag= Cyc_Stmt_tok; _temp2856.f1= Cyc_Parse_flatten_declarations(
_temp2853, _temp2854); _temp2856;}); _temp2855;}); break;} case 269: _LL2852: {
struct Cyc_List_List* _temp2858= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2859= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2860=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2860[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2861; _temp2861.tag= Cyc_Stmt_tok; _temp2861.f1= Cyc_Parse_flatten_declarations(
_temp2858, _temp2859); _temp2861;}); _temp2860;}); break;} case 270: _LL2857: {
struct Cyc_List_List* _temp2863= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2864= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2865=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2865[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2866; _temp2866.tag= Cyc_Stmt_tok; _temp2866.f1= Cyc_Parse_flatten_declarations(
_temp2863, _temp2864); _temp2866;}); _temp2865;}); break;} case 271: _LL2862:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2868=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2868[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2869; _temp2869.tag= Cyc_Stmt_tok; _temp2869.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2870=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2870[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2870;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2869;}); _temp2868;}); break; case 272: _LL2867: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2872=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2872[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2873; _temp2873.tag= Cyc_Stmt_tok; _temp2873.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2873;}); _temp2872;}); break; case 273:
_LL2871: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2875=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2875[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2876; _temp2876.tag= Cyc_Stmt_tok; _temp2876.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2876;}); _temp2875;}); break; case 274: _LL2874: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2878=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2878[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2879; _temp2879.tag= Cyc_Stmt_tok; _temp2879.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2879;}); _temp2878;}); break; case 275:
_LL2877: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2881=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2881[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2882; _temp2882.tag= Cyc_Stmt_tok; _temp2882.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2882;}); _temp2881;}); break; case 276: _LL2880: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2884=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2884[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2885; _temp2885.tag= Cyc_Stmt_tok; _temp2885.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2885;}); _temp2884;}); break; case 277:
_LL2883: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2887=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2887[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2888; _temp2888.tag= Cyc_Stmt_tok; _temp2888.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2888;}); _temp2887;}); break; case 278:
_LL2886: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2890=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2890[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2891; _temp2891.tag= Cyc_Stmt_tok; _temp2891.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2891;}); _temp2890;}); break; case 279: _LL2889: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2893=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2893[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2894; _temp2894.tag= Cyc_Pattern_tok; _temp2894.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2894;});
_temp2893;}); break; case 280: _LL2892: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 281:
_LL2895: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2897=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2897[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2898; _temp2898.tag= Cyc_Pattern_tok; _temp2898.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2899=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2899[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2900; _temp2900.tag= Cyc_Absyn_Int_p; _temp2900.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2900.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2900;}); _temp2899;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2898;});
_temp2897;}); break; case 282: _LL2896: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2902=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2902[ 0]=({ struct Cyc_Pattern_tok_struct _temp2903; _temp2903.tag= Cyc_Pattern_tok;
_temp2903.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2904=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2904[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2905; _temp2905.tag= Cyc_Absyn_Int_p;
_temp2905.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2905.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2905;});
_temp2904;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2903;}); _temp2902;}); break; case 283:
_LL2901: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2907=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2907[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2908; _temp2908.tag= Cyc_Pattern_tok; _temp2908.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2909=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2909[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2910; _temp2910.tag= Cyc_Absyn_Float_p; _temp2910.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2910;}); _temp2909;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2908;});
_temp2907;}); break; case 284: _LL2906: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2912=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2912[ 0]=({ struct Cyc_Pattern_tok_struct _temp2913; _temp2913.tag= Cyc_Pattern_tok;
_temp2913.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2914=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2914[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2915; _temp2915.tag= Cyc_Absyn_Char_p; _temp2915.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2915;});
_temp2914;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2913;}); _temp2912;}); break; case 285:
_LL2911: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2917=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2917[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2918; _temp2918.tag= Cyc_Pattern_tok; _temp2918.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2918;});
_temp2917;}); break; case 286: _LL2916: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2920=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2920[ 0]=({ struct Cyc_Pattern_tok_struct _temp2921; _temp2921.tag= Cyc_Pattern_tok;
_temp2921.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2922=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2922[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2923; _temp2923.tag=
Cyc_Absyn_UnknownId_p; _temp2923.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2923;}); _temp2922;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2921;}); _temp2920;}); break; case 287:
_LL2919: { struct Cyc_List_List* _temp2925=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2926=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2926[ 0]=({ struct Cyc_Pattern_tok_struct _temp2927; _temp2927.tag= Cyc_Pattern_tok;
_temp2927.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2928=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2928[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2929; _temp2929.tag= Cyc_Absyn_UnknownCall_p; _temp2929.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2929.f2=
_temp2925; _temp2929.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2929;});
_temp2928;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2927;}); _temp2926;}); break;} case
288: _LL2924: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2931=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2931[ 0]=({ struct Cyc_Pattern_tok_struct _temp2932; _temp2932.tag= Cyc_Pattern_tok;
_temp2932.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2933=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2933[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2934; _temp2934.tag= Cyc_Absyn_Tuple_p;
_temp2934.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2934;}); _temp2933;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2932;}); _temp2931;}); break; case 289: _LL2930: { struct Cyc_List_List*
_temp2936=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2937=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2937[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2938; _temp2938.tag= Cyc_Pattern_tok; _temp2938.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2939=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2939[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2940; _temp2940.tag= Cyc_Absyn_UnknownFields_p;
_temp2940.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2940.f2= _temp2936; _temp2940.f3= 0; _temp2940;});
_temp2939;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2938;}); _temp2937;}); break;} case
290: _LL2935: { struct Cyc_List_List* _temp2942=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2943=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2943[ 0]=({ struct Cyc_Pattern_tok_struct _temp2944; _temp2944.tag= Cyc_Pattern_tok;
_temp2944.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2945=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2945[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2946; _temp2946.tag= Cyc_Absyn_UnknownFields_p; _temp2946.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2946.f2=
_temp2942; _temp2946.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2946;});
_temp2945;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2944;}); _temp2943;}); break;} case
291: _LL2941: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2948=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2948[ 0]=({ struct Cyc_Pattern_tok_struct _temp2949; _temp2949.tag= Cyc_Pattern_tok;
_temp2949.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2950=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2950[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2951; _temp2951.tag= Cyc_Absyn_Pointer_p;
_temp2951.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2951;}); _temp2950;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2949;});
_temp2948;}); break; case 292: _LL2947: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2953=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2953[ 0]=({ struct Cyc_Pattern_tok_struct _temp2954; _temp2954.tag= Cyc_Pattern_tok;
_temp2954.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2955=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2955[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2956; _temp2956.tag=
Cyc_Absyn_Reference_p; _temp2956.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2957=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2957->f1=(
void*) Cyc_Absyn_Loc_n; _temp2957->f2=({ struct _tagged_arr* _temp2958=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2958[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2958;});
_temp2957;}),( void*) Cyc_Absyn_VoidType, 0); _temp2956;}); _temp2955;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2954;}); _temp2953;}); break; case 293: _LL2952: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2960=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2960[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2961; _temp2961.tag= Cyc_PatternList_tok;
_temp2961.f1= 0; _temp2961;}); _temp2960;}); break; case 294: _LL2959: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2963=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2963[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2964; _temp2964.tag= Cyc_PatternList_tok;
_temp2964.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2964;}); _temp2963;}); break; case 295: _LL2962: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2966=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2966[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2967; _temp2967.tag= Cyc_PatternList_tok;
_temp2967.f1=({ struct Cyc_List_List* _temp2968=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2968->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2968->tl= 0;
_temp2968;}); _temp2967;}); _temp2966;}); break; case 296: _LL2965: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2970=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2970[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2971; _temp2971.tag= Cyc_PatternList_tok;
_temp2971.f1=({ struct Cyc_List_List* _temp2972=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2972->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2972->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2972;}); _temp2971;}); _temp2970;}); break; case 297:
_LL2969: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2974=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2974[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2975; _temp2975.tag=
Cyc_FieldPattern_tok; _temp2975.f1=({ struct _tuple14* _temp2976=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2976->f1= 0; _temp2976->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2976;}); _temp2975;}); _temp2974;}); break; case 298:
_LL2973: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2978=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2978[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2979; _temp2979.tag=
Cyc_FieldPattern_tok; _temp2979.f1=({ struct _tuple14* _temp2980=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2980->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2980->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2980;}); _temp2979;}); _temp2978;}); break; case 299:
_LL2977: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2982=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2982[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2983; _temp2983.tag=
Cyc_FieldPatternList_tok; _temp2983.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2983;});
_temp2982;}); break; case 300: _LL2981: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2985=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2985[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2986; _temp2986.tag= Cyc_FieldPatternList_tok; _temp2986.f1=({ struct Cyc_List_List*
_temp2987=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2987->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2987->tl= 0;
_temp2987;}); _temp2986;}); _temp2985;}); break; case 301: _LL2984: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2989=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2989[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2990; _temp2990.tag= Cyc_FieldPatternList_tok;
_temp2990.f1=({ struct Cyc_List_List* _temp2991=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2991->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2991->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2991;}); _temp2990;}); _temp2989;}); break; case 302:
_LL2988: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 303: _LL2992: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2994=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2994[ 0]=({ struct Cyc_Exp_tok_struct _temp2995; _temp2995.tag= Cyc_Exp_tok;
_temp2995.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2995;}); _temp2994;}); break; case 304: _LL2993: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 305: _LL2996:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2998=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2998[ 0]=({ struct Cyc_Exp_tok_struct
_temp2999; _temp2999.tag= Cyc_Exp_tok; _temp2999.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2999;}); _temp2998;}); break; case 306: _LL2997: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3001=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3001[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3002; _temp3002.tag= Cyc_Primopopt_tok; _temp3002.f1=
0; _temp3002;}); _temp3001;}); break; case 307: _LL3000: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3004=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3004[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3005; _temp3005.tag= Cyc_Primopopt_tok; _temp3005.f1=({
struct Cyc_Core_Opt* _temp3006=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3006->v=( void*)(( void*) Cyc_Absyn_Times); _temp3006;});
_temp3005;}); _temp3004;}); break; case 308: _LL3003: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3008=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3008[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3009; _temp3009.tag= Cyc_Primopopt_tok; _temp3009.f1=({
struct Cyc_Core_Opt* _temp3010=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3010->v=( void*)(( void*) Cyc_Absyn_Div); _temp3010;});
_temp3009;}); _temp3008;}); break; case 309: _LL3007: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3012=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3012[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3013; _temp3013.tag= Cyc_Primopopt_tok; _temp3013.f1=({
struct Cyc_Core_Opt* _temp3014=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3014->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3014;});
_temp3013;}); _temp3012;}); break; case 310: _LL3011: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3016=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3016[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3017; _temp3017.tag= Cyc_Primopopt_tok; _temp3017.f1=({
struct Cyc_Core_Opt* _temp3018=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3018->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3018;});
_temp3017;}); _temp3016;}); break; case 311: _LL3015: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3020=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3020[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3021; _temp3021.tag= Cyc_Primopopt_tok; _temp3021.f1=({
struct Cyc_Core_Opt* _temp3022=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3022->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3022;});
_temp3021;}); _temp3020;}); break; case 312: _LL3019: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3024=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3024[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3025; _temp3025.tag= Cyc_Primopopt_tok; _temp3025.f1=({
struct Cyc_Core_Opt* _temp3026=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3026->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3026;});
_temp3025;}); _temp3024;}); break; case 313: _LL3023: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3028=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3028[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3029; _temp3029.tag= Cyc_Primopopt_tok; _temp3029.f1=({
struct Cyc_Core_Opt* _temp3030=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3030->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3030;});
_temp3029;}); _temp3028;}); break; case 314: _LL3027: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3032=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3032[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3033; _temp3033.tag= Cyc_Primopopt_tok; _temp3033.f1=({
struct Cyc_Core_Opt* _temp3034=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3034->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3034;});
_temp3033;}); _temp3032;}); break; case 315: _LL3031: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3036=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3036[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3037; _temp3037.tag= Cyc_Primopopt_tok; _temp3037.f1=({
struct Cyc_Core_Opt* _temp3038=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3038->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3038;});
_temp3037;}); _temp3036;}); break; case 316: _LL3035: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3040=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3040[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3041; _temp3041.tag= Cyc_Primopopt_tok; _temp3041.f1=({
struct Cyc_Core_Opt* _temp3042=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3042->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3042;});
_temp3041;}); _temp3040;}); break; case 317: _LL3039: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 318: _LL3043:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3045=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3045[ 0]=({ struct Cyc_Exp_tok_struct
_temp3046; _temp3046.tag= Cyc_Exp_tok; _temp3046.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3046;});
_temp3045;}); break; case 319: _LL3044: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3048=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3048[ 0]=({ struct Cyc_Exp_tok_struct _temp3049; _temp3049.tag= Cyc_Exp_tok;
_temp3049.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3049;}); _temp3048;}); break; case 320: _LL3047: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3051=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3051[ 0]=({ struct Cyc_Exp_tok_struct
_temp3052; _temp3052.tag= Cyc_Exp_tok; _temp3052.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3052;}); _temp3051;}); break; case 321: _LL3050: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3054=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3054[ 0]=({ struct Cyc_Exp_tok_struct
_temp3055; _temp3055.tag= Cyc_Exp_tok; _temp3055.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3055;}); _temp3054;}); break; case 322: _LL3053: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3057=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3057[ 0]=({ struct Cyc_Exp_tok_struct
_temp3058; _temp3058.tag= Cyc_Exp_tok; _temp3058.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3058;});
_temp3057;}); break; case 323: _LL3056: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3060=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3060[ 0]=({ struct Cyc_Exp_tok_struct _temp3061; _temp3061.tag= Cyc_Exp_tok;
_temp3061.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3061;}); _temp3060;}); break; case 324: _LL3059: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 325: _LL3062:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
326: _LL3063: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3065=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3065[ 0]=({ struct Cyc_Exp_tok_struct
_temp3066; _temp3066.tag= Cyc_Exp_tok; _temp3066.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3066;}); _temp3065;}); break; case 327: _LL3064: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 328: _LL3067:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3069=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3069[ 0]=({ struct Cyc_Exp_tok_struct
_temp3070; _temp3070.tag= Cyc_Exp_tok; _temp3070.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3070;}); _temp3069;}); break; case 329: _LL3068: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL3071:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3073=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3073[ 0]=({ struct Cyc_Exp_tok_struct
_temp3074; _temp3074.tag= Cyc_Exp_tok; _temp3074.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3074;});
_temp3073;}); break; case 331: _LL3072: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL3075:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break; case 333: _LL3076: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3079:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3081=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3081[ 0]=({ struct Cyc_Exp_tok_struct
_temp3082; _temp3082.tag= Cyc_Exp_tok; _temp3082.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3082;});
_temp3081;}); break; case 335: _LL3080: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3083:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3085=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3085[ 0]=({ struct Cyc_Exp_tok_struct
_temp3086; _temp3086.tag= Cyc_Exp_tok; _temp3086.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3086;}); _temp3085;}); break; case 337: _LL3084: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3088=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3088[ 0]=({ struct Cyc_Exp_tok_struct
_temp3089; _temp3089.tag= Cyc_Exp_tok; _temp3089.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3089;}); _temp3088;}); break; case 338: _LL3087: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 339: _LL3090:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3092=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3092[ 0]=({ struct Cyc_Exp_tok_struct
_temp3093; _temp3093.tag= Cyc_Exp_tok; _temp3093.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3093;}); _temp3092;}); break; case 340: _LL3091: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3095=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3095[ 0]=({ struct Cyc_Exp_tok_struct
_temp3096; _temp3096.tag= Cyc_Exp_tok; _temp3096.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3096;}); _temp3095;}); break; case 341: _LL3094: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3098=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3098[ 0]=({ struct Cyc_Exp_tok_struct
_temp3099; _temp3099.tag= Cyc_Exp_tok; _temp3099.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3099;}); _temp3098;}); break; case 342: _LL3097: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3101=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3101[ 0]=({ struct Cyc_Exp_tok_struct
_temp3102; _temp3102.tag= Cyc_Exp_tok; _temp3102.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3102;}); _temp3101;}); break; case 343: _LL3100: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 344: _LL3103:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3105=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3105[ 0]=({ struct Cyc_Exp_tok_struct
_temp3106; _temp3106.tag= Cyc_Exp_tok; _temp3106.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3106;}); _temp3105;}); break; case 345: _LL3104: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3108=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3108[ 0]=({ struct Cyc_Exp_tok_struct
_temp3109; _temp3109.tag= Cyc_Exp_tok; _temp3109.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3109;}); _temp3108;}); break; case 346: _LL3107: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 347: _LL3110:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3112=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3112[ 0]=({ struct Cyc_Exp_tok_struct
_temp3113; _temp3113.tag= Cyc_Exp_tok; _temp3113.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3113;});
_temp3112;}); break; case 348: _LL3111: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3115=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3115[ 0]=({ struct Cyc_Exp_tok_struct _temp3116; _temp3116.tag= Cyc_Exp_tok;
_temp3116.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3116;}); _temp3115;}); break; case 349: _LL3114: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 350: _LL3117:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3119=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3119[ 0]=({ struct Cyc_Exp_tok_struct
_temp3120; _temp3120.tag= Cyc_Exp_tok; _temp3120.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3120;});
_temp3119;}); break; case 351: _LL3118: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3122=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3122[ 0]=({ struct Cyc_Exp_tok_struct _temp3123; _temp3123.tag= Cyc_Exp_tok;
_temp3123.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3123;}); _temp3122;}); break; case 352: _LL3121: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3125=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3125[ 0]=({ struct Cyc_Exp_tok_struct
_temp3126; _temp3126.tag= Cyc_Exp_tok; _temp3126.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3126;});
_temp3125;}); break; case 353: _LL3124: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 354: _LL3127:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3129=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3129[ 0]=({ struct Cyc_Exp_tok_struct
_temp3130; _temp3130.tag= Cyc_Exp_tok; _temp3130.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3130;}); _temp3129;}); break; case 355: _LL3128: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3131:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3133=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3133[ 0]=({ struct Cyc_Exp_tok_struct
_temp3134; _temp3134.tag= Cyc_Exp_tok; _temp3134.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3134;}); _temp3133;}); break; case 357: _LL3132: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3136=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3136[ 0]=({ struct Cyc_Exp_tok_struct
_temp3137; _temp3137.tag= Cyc_Exp_tok; _temp3137.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3137;}); _temp3136;}); break; case 358: _LL3135: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3139=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3139[ 0]=({ struct Cyc_Exp_tok_struct
_temp3140; _temp3140.tag= Cyc_Exp_tok; _temp3140.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3140;}); _temp3139;}); break; case 359: _LL3138: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3142=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3142[ 0]=({ struct Cyc_Exp_tok_struct
_temp3143; _temp3143.tag= Cyc_Exp_tok; _temp3143.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3143;}); _temp3142;}); break; case 360: _LL3141: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 361: _LL3144:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3146=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3146[ 0]=({ struct Cyc_Exp_tok_struct
_temp3147; _temp3147.tag= Cyc_Exp_tok; _temp3147.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3147;}); _temp3146;}); break; case 362: _LL3145: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3149=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3149[ 0]=({ struct Cyc_Exp_tok_struct
_temp3150; _temp3150.tag= Cyc_Exp_tok; _temp3150.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3150;});
_temp3149;}); break; case 363: _LL3148: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3152=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3152[ 0]=({ struct Cyc_Exp_tok_struct _temp3153; _temp3153.tag= Cyc_Exp_tok;
_temp3153.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3153;}); _temp3152;}); break; case 364: _LL3151: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3155=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3155[ 0]=({ struct Cyc_Exp_tok_struct
_temp3156; _temp3156.tag= Cyc_Exp_tok; _temp3156.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3157=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3157[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3157;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3156;}); _temp3155;}); break; case 365:
_LL3154: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3159=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3159[ 0]=({ struct Cyc_Exp_tok_struct
_temp3160; _temp3160.tag= Cyc_Exp_tok; _temp3160.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3160;}); _temp3159;}); break; case 366: _LL3158: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3162=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3162[ 0]=({ struct Cyc_Exp_tok_struct
_temp3163; _temp3163.tag= Cyc_Exp_tok; _temp3163.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3164=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3164[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3165; _temp3165.tag= Cyc_Absyn_Malloc_e;
_temp3165.f1= 0; _temp3165.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3165;}); _temp3164;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3163;});
_temp3162;}); break; case 367: _LL3161: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3167=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3167[ 0]=({ struct Cyc_Exp_tok_struct _temp3168; _temp3168.tag= Cyc_Exp_tok;
_temp3168.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3169=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3169[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3170; _temp3170.tag= Cyc_Absyn_Malloc_e;
_temp3170.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3170.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3170;}); _temp3169;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3168;});
_temp3167;}); break; case 368: _LL3166: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3172=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3172[ 0]=({ struct Cyc_Primop_tok_struct _temp3173; _temp3173.tag= Cyc_Primop_tok;
_temp3173.f1=( void*)(( void*) Cyc_Absyn_Printf); _temp3173;}); _temp3172;});
break; case 369: _LL3171: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3175=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3175[ 0]=({ struct Cyc_Primop_tok_struct _temp3176; _temp3176.tag= Cyc_Primop_tok;
_temp3176.f1=( void*)(( void*) Cyc_Absyn_Fprintf); _temp3176;}); _temp3175;});
break; case 370: _LL3174: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3178=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3178[ 0]=({ struct Cyc_Primop_tok_struct _temp3179; _temp3179.tag= Cyc_Primop_tok;
_temp3179.f1=( void*)(( void*) Cyc_Absyn_Xprintf); _temp3179;}); _temp3178;});
break; case 371: _LL3177: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3181=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3181[ 0]=({ struct Cyc_Primop_tok_struct _temp3182; _temp3182.tag= Cyc_Primop_tok;
_temp3182.f1=( void*)(( void*) Cyc_Absyn_Scanf); _temp3182;}); _temp3181;});
break; case 372: _LL3180: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3184=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3184[ 0]=({ struct Cyc_Primop_tok_struct _temp3185; _temp3185.tag= Cyc_Primop_tok;
_temp3185.f1=( void*)(( void*) Cyc_Absyn_Fscanf); _temp3185;}); _temp3184;});
break; case 373: _LL3183: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3187=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3187[ 0]=({ struct Cyc_Primop_tok_struct _temp3188; _temp3188.tag= Cyc_Primop_tok;
_temp3188.f1=( void*)(( void*) Cyc_Absyn_Sscanf); _temp3188;}); _temp3187;});
break; case 374: _LL3186: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3190=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3190[ 0]=({ struct Cyc_Primop_tok_struct _temp3191; _temp3191.tag= Cyc_Primop_tok;
_temp3191.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3191;}); _temp3190;});
break; case 375: _LL3189: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3193=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3193[ 0]=({ struct Cyc_Primop_tok_struct _temp3194; _temp3194.tag= Cyc_Primop_tok;
_temp3194.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3194;}); _temp3193;});
break; case 376: _LL3192: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3196=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3196[ 0]=({ struct Cyc_Primop_tok_struct _temp3197; _temp3197.tag= Cyc_Primop_tok;
_temp3197.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3197;}); _temp3196;});
break; case 377: _LL3195: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 378: _LL3198: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3200=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3200[ 0]=({ struct Cyc_Exp_tok_struct _temp3201; _temp3201.tag= Cyc_Exp_tok;
_temp3201.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3201;}); _temp3200;}); break; case 379: _LL3199: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3203=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3203[ 0]=({ struct Cyc_Exp_tok_struct
_temp3204; _temp3204.tag= Cyc_Exp_tok; _temp3204.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3204;}); _temp3203;}); break; case 380:
_LL3202: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3206=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3206[ 0]=({ struct Cyc_Exp_tok_struct
_temp3207; _temp3207.tag= Cyc_Exp_tok; _temp3207.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3207;});
_temp3206;}); break; case 381: _LL3205: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3209=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3209[ 0]=({ struct Cyc_Exp_tok_struct _temp3210; _temp3210.tag= Cyc_Exp_tok;
_temp3210.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3211=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3211[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3211;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3210;}); _temp3209;}); break; case 382: _LL3208: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3213=(*
q).f1; struct Cyc_List_List* _temp3223; struct Cyc_List_List* _temp3225; _LL3215:
if( _temp3213 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3216;} else{ goto _LL3217;}
_LL3217: if(( unsigned int) _temp3213 > 1u?*(( int*) _temp3213) == Cyc_Absyn_Rel_n:
0){ _LL3224: _temp3223=(( struct Cyc_Absyn_Rel_n_struct*) _temp3213)->f1; if(
_temp3223 == 0){ goto _LL3218;} else{ goto _LL3219;}} else{ goto _LL3219;}
_LL3219: if(( unsigned int) _temp3213 > 1u?*(( int*) _temp3213) == Cyc_Absyn_Abs_n:
0){ _LL3226: _temp3225=(( struct Cyc_Absyn_Abs_n_struct*) _temp3213)->f1; if(
_temp3225 == 0){ goto _LL3220;} else{ goto _LL3221;}} else{ goto _LL3221;}
_LL3221: goto _LL3222; _LL3216: goto _LL3214; _LL3218: goto _LL3214; _LL3220:
goto _LL3214; _LL3222: Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3214; _LL3214:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3227=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3227[ 0]=({ struct Cyc_Exp_tok_struct _temp3228; _temp3228.tag= Cyc_Exp_tok;
_temp3228.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3228;}); _temp3227;}); break;} case 383: _LL3212: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3230=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3230[ 0]=({ struct Cyc_Exp_tok_struct
_temp3231; _temp3231.tag= Cyc_Exp_tok; _temp3231.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),({ struct _tagged_arr* _temp3232=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp3232[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3232;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3231;}); _temp3230;}); break; case 384: _LL3229: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3234=(*
q).f1; struct Cyc_List_List* _temp3244; struct Cyc_List_List* _temp3246; _LL3236:
if( _temp3234 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3237;} else{ goto _LL3238;}
_LL3238: if(( unsigned int) _temp3234 > 1u?*(( int*) _temp3234) == Cyc_Absyn_Rel_n:
0){ _LL3245: _temp3244=(( struct Cyc_Absyn_Rel_n_struct*) _temp3234)->f1; if(
_temp3244 == 0){ goto _LL3239;} else{ goto _LL3240;}} else{ goto _LL3240;}
_LL3240: if(( unsigned int) _temp3234 > 1u?*(( int*) _temp3234) == Cyc_Absyn_Abs_n:
0){ _LL3247: _temp3246=(( struct Cyc_Absyn_Abs_n_struct*) _temp3234)->f1; if(
_temp3246 == 0){ goto _LL3241;} else{ goto _LL3242;}} else{ goto _LL3242;}
_LL3242: goto _LL3243; _LL3237: goto _LL3235; _LL3239: goto _LL3235; _LL3241:
goto _LL3235; _LL3243: Cyc_Parse_err( _tag_arr("struct field is qualified with module name",
sizeof( unsigned char), 43u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3235; _LL3235:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3248=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3248[ 0]=({ struct Cyc_Exp_tok_struct _temp3249; _temp3249.tag= Cyc_Exp_tok;
_temp3249.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3249;}); _temp3248;}); break;} case 385: _LL3233: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3251=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3251[ 0]=({ struct Cyc_Exp_tok_struct
_temp3252; _temp3252.tag= Cyc_Exp_tok; _temp3252.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3252;}); _temp3251;}); break; case 386: _LL3250: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3254=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3254[ 0]=({ struct Cyc_Exp_tok_struct
_temp3255; _temp3255.tag= Cyc_Exp_tok; _temp3255.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3255;}); _temp3254;}); break; case 387: _LL3253: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3257=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3257[ 0]=({ struct Cyc_Exp_tok_struct
_temp3258; _temp3258.tag= Cyc_Exp_tok; _temp3258.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3259=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3259[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3260; _temp3260.tag= Cyc_Absyn_CompoundLit_e;
_temp3260.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3260.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3260;});
_temp3259;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3258;}); _temp3257;}); break; case 388:
_LL3256: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3262=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3262[ 0]=({ struct Cyc_Exp_tok_struct
_temp3263; _temp3263.tag= Cyc_Exp_tok; _temp3263.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3264=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3264[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3265; _temp3265.tag= Cyc_Absyn_CompoundLit_e;
_temp3265.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3265.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3265;});
_temp3264;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3263;}); _temp3262;}); break; case 389:
_LL3261: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3267=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3267[ 0]=({ struct Cyc_Exp_tok_struct
_temp3268; _temp3268.tag= Cyc_Exp_tok; _temp3268.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3269=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3269[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3270; _temp3270.tag= Cyc_Absyn_Fill_e; _temp3270.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3270;});
_temp3269;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3268;}); _temp3267;}); break; case 390:
_LL3266: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3272=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3272[ 0]=({ struct Cyc_Exp_tok_struct
_temp3273; _temp3273.tag= Cyc_Exp_tok; _temp3273.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3274=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3274[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3275; _temp3275.tag= Cyc_Absyn_Codegen_e;
_temp3275.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3275;}); _temp3274;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3273;}); _temp3272;}); break; case 391: _LL3271: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3277=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3277[ 0]=({ struct Cyc_Exp_tok_struct
_temp3278; _temp3278.tag= Cyc_Exp_tok; _temp3278.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3279=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3279[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3280; _temp3280.tag= Cyc_Absyn_UnknownId_e;
_temp3280.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3280;}); _temp3279;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3278;});
_temp3277;}); break; case 392: _LL3276: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 393: _LL3281:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3283=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3283[ 0]=({ struct Cyc_Exp_tok_struct
_temp3284; _temp3284.tag= Cyc_Exp_tok; _temp3284.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3284;});
_temp3283;}); break; case 394: _LL3282: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 395:
_LL3285: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3287=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3287[ 0]=({ struct Cyc_Exp_tok_struct
_temp3288; _temp3288.tag= Cyc_Exp_tok; _temp3288.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3289=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3289[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3290; _temp3290.tag=
Cyc_Absyn_UnknownId_e; _temp3290.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3290;});
_temp3289;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3288;});
_temp3287;}); break; case 396: _LL3286: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3292=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3292[ 0]=({ struct Cyc_Exp_tok_struct _temp3293; _temp3293.tag= Cyc_Exp_tok;
_temp3293.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3294=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3294[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3295; _temp3295.tag=
Cyc_Absyn_UnknownId_e; _temp3295.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3295;});
_temp3294;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3293;}); _temp3292;}); break; case 397: _LL3291: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3297=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3297[ 0]=({ struct Cyc_Exp_tok_struct
_temp3298; _temp3298.tag= Cyc_Exp_tok; _temp3298.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3298;}); _temp3297;}); break; case 398: _LL3296: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3300=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3300[ 0]=({ struct Cyc_Exp_tok_struct
_temp3301; _temp3301.tag= Cyc_Exp_tok; _temp3301.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3302=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3302[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3303; _temp3303.tag= Cyc_Absyn_Struct_e;
_temp3303.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3303.f2= 0; _temp3303.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3303.f4= 0;
_temp3303;}); _temp3302;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3301;});
_temp3300;}); break; case 399: _LL3299: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3305=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3305[ 0]=({ struct Cyc_Exp_tok_struct _temp3306; _temp3306.tag= Cyc_Exp_tok;
_temp3306.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3306;}); _temp3305;}); break; case 400: _LL3304: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3308=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3308[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3309; _temp3309.tag= Cyc_ExpList_tok; _temp3309.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3309;});
_temp3308;}); break; case 401: _LL3307: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3311=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3311[ 0]=({ struct Cyc_ExpList_tok_struct _temp3312; _temp3312.tag= Cyc_ExpList_tok;
_temp3312.f1=({ struct Cyc_List_List* _temp3313=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3313->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3313->tl= 0;
_temp3313;}); _temp3312;}); _temp3311;}); break; case 402: _LL3310: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3315=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3315[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3316; _temp3316.tag= Cyc_ExpList_tok; _temp3316.f1=({ struct Cyc_List_List*
_temp3317=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3317->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3317->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3317;});
_temp3316;}); _temp3315;}); break; case 403: _LL3314: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3319=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3319[ 0]=({ struct Cyc_Exp_tok_struct
_temp3320; _temp3320.tag= Cyc_Exp_tok; _temp3320.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3320;});
_temp3319;}); break; case 404: _LL3318: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3322=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3322[ 0]=({ struct Cyc_Exp_tok_struct _temp3323; _temp3323.tag= Cyc_Exp_tok;
_temp3323.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3323;});
_temp3322;}); break; case 405: _LL3321: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3325=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3325[ 0]=({ struct Cyc_Exp_tok_struct _temp3326; _temp3326.tag= Cyc_Exp_tok;
_temp3326.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3326;});
_temp3325;}); break; case 406: _LL3324: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3328=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3328[ 0]=({ struct Cyc_Exp_tok_struct _temp3329; _temp3329.tag= Cyc_Exp_tok;
_temp3329.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3329;});
_temp3328;}); break; case 407: _LL3327: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3331=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3331[ 0]=({ struct Cyc_QualId_tok_struct _temp3332; _temp3332.tag= Cyc_QualId_tok;
_temp3332.f1=({ struct _tuple1* _temp3333=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3333->f1= Cyc_Absyn_rel_ns_null; _temp3333->f2=({ struct
_tagged_arr* _temp3334=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3334[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3334;}); _temp3333;});
_temp3332;}); _temp3331;}); break; case 408: _LL3330: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3335:(
int) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen; yyssp_offset -=
yylen; yylsp_offset -= yylen; yyvs[ _check_known_subscript_notnull( 10000u, ++
yyvsp_offset)]= yyval; yylsp_offset ++; if( yylen == 0){( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line= Cyc_yylloc.first_line;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_column= Cyc_yylloc.first_column;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_column;} else{(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset + yylen) - 1)]).last_line;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset + yylen) - 1)]).last_column;}
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 409u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 113u, yyn - 125)] + yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >= 0?
yystate <= 4792: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4793u, yystate)]
== yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4793u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 113u, yyn - 125)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 798u, yystate)]; if( yyn > - 32768? yyn < 4792:
0){ int sze= 0; struct _tagged_arr msg; int x; int count; count= 0; for( x= yyn
< 0? - yyn: 0; x < 238u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4793u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 238u, x)]) + 15, count ++);}} msg=({
unsigned int _temp3337=( unsigned int)( sze + 15); unsigned char* _temp3338=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3337); struct
_tagged_arr _temp3340= _tag_arr( _temp3338, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3339= _temp3337; unsigned int i;
for( i= 0; i < _temp3339; i ++){ _temp3338[ i]='\000';}}; _temp3340;}); Cyc_String_strcpy(
msg, _tag_arr("parse error", sizeof( unsigned char), 12u)); if( count < 5){
count= 0; for( x= yyn < 0? - yyn: 0; x < 238u / sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4793u, x + yyn)] == x){ Cyc_String_strcat(
msg, count == 0? _tag_arr(", expecting `", sizeof( unsigned char), 14u):
_tag_arr(" or `", sizeof( unsigned char), 6u)); Cyc_String_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 238u, x)]); Cyc_String_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset == 0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 798u,
yystate)]; if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1:
yyn > 4792)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4793u, yyn)] != 1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4793u,
yyn)]; if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 797){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3350= v; struct
_tuple16* _temp3370; struct _tuple16 _temp3372; int _temp3373; unsigned char
_temp3375; short _temp3377; struct _tagged_arr _temp3379; struct Cyc_Core_Opt*
_temp3381; struct Cyc_Core_Opt* _temp3383; struct Cyc_Core_Opt _temp3385; struct
_tagged_arr* _temp3386; struct _tuple1* _temp3388; struct _tuple1 _temp3390;
struct _tagged_arr* _temp3391; void* _temp3393; _LL3352: if( _temp3350 == Cyc_Okay_tok){
goto _LL3353;} else{ goto _LL3354;} _LL3354: if(*(( void**) _temp3350) == Cyc_Int_tok){
_LL3371: _temp3370=(( struct Cyc_Int_tok_struct*) _temp3350)->f1; _temp3372=*
_temp3370; _LL3374: _temp3373= _temp3372.f2; goto _LL3355;} else{ goto _LL3356;}
_LL3356: if(*(( void**) _temp3350) == Cyc_Char_tok){ _LL3376: _temp3375=((
struct Cyc_Char_tok_struct*) _temp3350)->f1; goto _LL3357;} else{ goto _LL3358;}
_LL3358: if(*(( void**) _temp3350) == Cyc_Short_tok){ _LL3378: _temp3377=((
struct Cyc_Short_tok_struct*) _temp3350)->f1; goto _LL3359;} else{ goto _LL3360;}
_LL3360: if(*(( void**) _temp3350) == Cyc_String_tok){ _LL3380: _temp3379=((
struct Cyc_String_tok_struct*) _temp3350)->f1; goto _LL3361;} else{ goto _LL3362;}
_LL3362: if(*(( void**) _temp3350) == Cyc_Stringopt_tok){ _LL3382: _temp3381=((
struct Cyc_Stringopt_tok_struct*) _temp3350)->f1; if( _temp3381 == 0){ goto
_LL3363;} else{ goto _LL3364;}} else{ goto _LL3364;} _LL3364: if(*(( void**)
_temp3350) == Cyc_Stringopt_tok){ _LL3384: _temp3383=(( struct Cyc_Stringopt_tok_struct*)
_temp3350)->f1; if( _temp3383 == 0){ goto _LL3366;} else{ _temp3385=* _temp3383;
_LL3387: _temp3386=( struct _tagged_arr*) _temp3385.v; goto _LL3365;}} else{
goto _LL3366;} _LL3366: if(*(( void**) _temp3350) == Cyc_QualId_tok){ _LL3389:
_temp3388=(( struct Cyc_QualId_tok_struct*) _temp3350)->f1; _temp3390=*
_temp3388; _LL3394: _temp3393= _temp3390.f1; goto _LL3392; _LL3392: _temp3391=
_temp3390.f2; goto _LL3367;} else{ goto _LL3368;} _LL3368: goto _LL3369; _LL3353:({
int(* _temp3395)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3398= Cyc_Stdio_stderr;
struct _tagged_arr _temp3399= _tag_arr("ok", sizeof( unsigned char), 3u); void*
_temp3396[ 0u]={}; struct _tagged_arr _temp3397={( void*) _temp3396,( void*)
_temp3396,( void*)( _temp3396 + 0u)}; _temp3395( _temp3398, _temp3399, _temp3397);});
goto _LL3351; _LL3355:({ int(* _temp3400)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3404= Cyc_Stdio_stderr; struct _tagged_arr _temp3405= _tag_arr("%d",
sizeof( unsigned char), 3u); struct Cyc_Stdio_Int_pa_struct _temp3406; _temp3406.tag=
Cyc_Stdio_Int_pa; _temp3406.f1=( unsigned int) _temp3373;{ void* _temp3403=(
void*)& _temp3406; void* _temp3401[ 1u]={ _temp3403}; struct _tagged_arr
_temp3402={( void*) _temp3401,( void*) _temp3401,( void*)( _temp3401 + 1u)};
_temp3400( _temp3404, _temp3405, _temp3402);}}); goto _LL3351; _LL3357:({ int(*
_temp3407)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3411= Cyc_Stdio_stderr;
struct _tagged_arr _temp3412= _tag_arr("%c", sizeof( unsigned char), 3u); struct
Cyc_Stdio_Int_pa_struct _temp3413; _temp3413.tag= Cyc_Stdio_Int_pa; _temp3413.f1=(
unsigned int)(( int) _temp3375);{ void* _temp3410=( void*)& _temp3413; void*
_temp3408[ 1u]={ _temp3410}; struct _tagged_arr _temp3409={( void*) _temp3408,(
void*) _temp3408,( void*)( _temp3408 + 1u)}; _temp3407( _temp3411, _temp3412,
_temp3409);}}); goto _LL3351; _LL3359:({ int(* _temp3414)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3418= Cyc_Stdio_stderr; struct _tagged_arr _temp3419= _tag_arr("%ds",
sizeof( unsigned char), 4u); struct Cyc_Stdio_Int_pa_struct _temp3420; _temp3420.tag=
Cyc_Stdio_Int_pa; _temp3420.f1=( unsigned int)(( int) _temp3377);{ void*
_temp3417=( void*)& _temp3420; void* _temp3415[ 1u]={ _temp3417}; struct
_tagged_arr _temp3416={( void*) _temp3415,( void*) _temp3415,( void*)( _temp3415
+ 1u)}; _temp3414( _temp3418, _temp3419, _temp3416);}}); goto _LL3351; _LL3361:({
int(* _temp3421)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3425= Cyc_Stdio_stderr;
struct _tagged_arr _temp3426= _tag_arr("\"%s\"", sizeof( unsigned char), 5u);
struct Cyc_Stdio_String_pa_struct _temp3427; _temp3427.tag= Cyc_Stdio_String_pa;
_temp3427.f1=( struct _tagged_arr) _temp3379;{ void* _temp3424=( void*)&
_temp3427; void* _temp3422[ 1u]={ _temp3424}; struct _tagged_arr _temp3423={(
void*) _temp3422,( void*) _temp3422,( void*)( _temp3422 + 1u)}; _temp3421(
_temp3425, _temp3426, _temp3423);}}); goto _LL3351; _LL3363:({ int(* _temp3428)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp3431= Cyc_Stdio_stderr; struct _tagged_arr
_temp3432= _tag_arr("null", sizeof( unsigned char), 5u); void* _temp3429[ 0u]={};
struct _tagged_arr _temp3430={( void*) _temp3429,( void*) _temp3429,( void*)(
_temp3429 + 0u)}; _temp3428( _temp3431, _temp3432, _temp3430);}); goto _LL3351;
_LL3365:({ int(* _temp3433)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3437= Cyc_Stdio_stderr;
struct _tagged_arr _temp3438= _tag_arr("\"%s\"", sizeof( unsigned char), 5u);
struct Cyc_Stdio_String_pa_struct _temp3439; _temp3439.tag= Cyc_Stdio_String_pa;
_temp3439.f1=( struct _tagged_arr)* _temp3386;{ void* _temp3436=( void*)&
_temp3439; void* _temp3434[ 1u]={ _temp3436}; struct _tagged_arr _temp3435={(
void*) _temp3434,( void*) _temp3434,( void*)( _temp3434 + 1u)}; _temp3433(
_temp3437, _temp3438, _temp3435);}}); goto _LL3351; _LL3367: { struct Cyc_List_List*
_temp3440= 0;{ void* _temp3441= _temp3393; struct Cyc_List_List* _temp3449;
struct Cyc_List_List* _temp3451; _LL3443: if(( unsigned int) _temp3441 > 1u?*((
int*) _temp3441) == Cyc_Absyn_Rel_n: 0){ _LL3450: _temp3449=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3441)->f1; goto _LL3444;} else{ goto _LL3445;} _LL3445: if(( unsigned int)
_temp3441 > 1u?*(( int*) _temp3441) == Cyc_Absyn_Abs_n: 0){ _LL3452: _temp3451=((
struct Cyc_Absyn_Abs_n_struct*) _temp3441)->f1; goto _LL3446;} else{ goto
_LL3447;} _LL3447: if( _temp3441 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3448;}
else{ goto _LL3442;} _LL3444: _temp3440= _temp3449; goto _LL3442; _LL3446:
_temp3440= _temp3451; goto _LL3442; _LL3448: goto _LL3442; _LL3442:;} for( 0;
_temp3440 != 0; _temp3440=(( struct Cyc_List_List*) _check_null( _temp3440))->tl){({
int(* _temp3453)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3457= Cyc_Stdio_stderr;
struct _tagged_arr _temp3458= _tag_arr("%s::", sizeof( unsigned char), 5u);
struct Cyc_Stdio_String_pa_struct _temp3459; _temp3459.tag= Cyc_Stdio_String_pa;
_temp3459.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3440))->hd);{ void* _temp3456=( void*)& _temp3459; void*
_temp3454[ 1u]={ _temp3456}; struct _tagged_arr _temp3455={( void*) _temp3454,(
void*) _temp3454,( void*)( _temp3454 + 1u)}; _temp3453( _temp3457, _temp3458,
_temp3455);}});}({ int(* _temp3460)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3464= Cyc_Stdio_stderr; struct _tagged_arr _temp3465= _tag_arr("%s::",
sizeof( unsigned char), 5u); struct Cyc_Stdio_String_pa_struct _temp3466;
_temp3466.tag= Cyc_Stdio_String_pa; _temp3466.f1=( struct _tagged_arr)*
_temp3391;{ void* _temp3463=( void*)& _temp3466; void* _temp3461[ 1u]={
_temp3463}; struct _tagged_arr _temp3462={( void*) _temp3461,( void*) _temp3461,(
void*)( _temp3461 + 1u)}; _temp3460( _temp3464, _temp3465, _temp3462);}}); goto
_LL3351;} _LL3369:({ int(* _temp3467)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3470= Cyc_Stdio_stderr; struct _tagged_arr _temp3471= _tag_arr("?", sizeof(
unsigned char), 2u); void* _temp3468[ 0u]={}; struct _tagged_arr _temp3469={(
void*) _temp3468,( void*) _temp3468,( void*)( _temp3468 + 0u)}; _temp3467(
_temp3470, _temp3471, _temp3469);}); goto _LL3351; _LL3351:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3472=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3472->v=( void*) Cyc_Lexing_from_file(
f); _temp3472;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}