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
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
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
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct Cyc_Stdio_String_pa_struct
_temp19; _temp19.tag= Cyc_Stdio_String_pa; _temp19.f1=( struct _tagged_arr) msg;{
struct Cyc_Stdio_String_pa_struct _temp18; _temp18.tag= Cyc_Stdio_String_pa;
_temp18.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void*
_temp17[ 2u]={& _temp18,& _temp19}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: Warning: %s\n", sizeof( unsigned char), 17u), _tag_arr( _temp17,
sizeof( void*), 2u));}}}); return;} static void* Cyc_Parse_unimp( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg){ return Cyc_Parse_abort((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp21; _temp21.tag=
Cyc_Stdio_String_pa; _temp21.f1=( struct _tagged_arr) msg;{ void* _temp20[ 1u]={&
_temp21}; Cyc_Stdio_aprintf( _tag_arr("%s unimplemented", sizeof( unsigned char),
17u), _tag_arr( _temp20, sizeof( void*), 1u));}}), sg);} static void Cyc_Parse_unimp2(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct Cyc_Stdio_String_pa_struct
_temp24; _temp24.tag= Cyc_Stdio_String_pa; _temp24.f1=( struct _tagged_arr) msg;{
struct Cyc_Stdio_String_pa_struct _temp23; _temp23.tag= Cyc_Stdio_String_pa;
_temp23.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void*
_temp22[ 2u]={& _temp23,& _temp24}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: Warning: Cyclone does not yet support %s\n", sizeof( unsigned char),
46u), _tag_arr( _temp22, sizeof( void*), 2u));}}}); return;} struct _tuple9{
struct _tuple7* f1; struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield*
Cyc_Parse_make_struct_field( struct Cyc_Position_Segment* loc, struct _tuple9*
field_info){ struct Cyc_Core_Opt* _temp27; struct _tuple7* _temp29; struct
_tuple9 _temp25=* field_info; _LL30: _temp29= _temp25.f1; goto _LL28; _LL28:
_temp27= _temp25.f2; goto _LL26; _LL26: if((* _temp29).f4 != 0){ Cyc_Parse_err(
_tag_arr("bad type params in struct field", sizeof( unsigned char), 32u), loc);}{
struct _tuple1* _temp31=(* _temp29).f1;{ void* _temp32=(* _temp31).f1; struct
Cyc_List_List* _temp42; struct Cyc_List_List* _temp44; _LL34: if(( unsigned int)
_temp32 > 1u?*(( int*) _temp32) == Cyc_Absyn_Rel_n: 0){ _LL43: _temp42=(( struct
Cyc_Absyn_Rel_n_struct*) _temp32)->f1; if( _temp42 == 0){ goto _LL35;} else{
goto _LL36;}} else{ goto _LL36;} _LL36: if(( unsigned int) _temp32 > 1u?*(( int*)
_temp32) == Cyc_Absyn_Abs_n: 0){ _LL45: _temp44=(( struct Cyc_Absyn_Abs_n_struct*)
_temp32)->f1; if( _temp44 == 0){ goto _LL37;} else{ goto _LL38;}} else{ goto
_LL38;} _LL38: if( _temp32 ==( void*) Cyc_Absyn_Loc_n){ goto _LL39;} else{ goto
_LL40;} _LL40: goto _LL41; _LL35: goto _LL33; _LL37: goto _LL33; _LL39: goto
_LL33; _LL41: Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a module name",
sizeof( unsigned char), 52u), loc); goto _LL33; _LL33:;}{ struct Cyc_List_List*
_temp46=(* _temp29).f5; return({ struct Cyc_Absyn_Structfield* _temp47=( struct
Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp47->name=(* _temp31).f2; _temp47->tq=(* _temp29).f2; _temp47->type=( void*)(*
_temp29).f3; _temp47->width= _temp27; _temp47->attributes= _temp46; _temp47;});}}}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp50; struct Cyc_List_List* _temp51; void* _temp53; struct Cyc_Absyn_Tqual
_temp55; struct Cyc_Core_Opt* _temp57; struct _tuple10* _temp48= field; _temp50=*
_temp48; _LL58: _temp57= _temp50.f1; goto _LL56; _LL56: _temp55= _temp50.f2;
goto _LL54; _LL54: _temp53= _temp50.f3; goto _LL52; _LL52: _temp51= _temp50.f4;
goto _LL49; _LL49: { struct Cyc_Core_Opt* _temp59= 0; if( _temp57 != 0){ _temp59=({
struct Cyc_Core_Opt* _temp60=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp60->v=( void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp57))->v)).f2; _temp60;});{ void* _temp61=(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp57))->v)).f1; struct Cyc_List_List*
_temp71; struct Cyc_List_List* _temp73; _LL63: if(( unsigned int) _temp61 > 1u?*((
int*) _temp61) == Cyc_Absyn_Rel_n: 0){ _LL72: _temp71=(( struct Cyc_Absyn_Rel_n_struct*)
_temp61)->f1; if( _temp71 == 0){ goto _LL64;} else{ goto _LL65;}} else{ goto
_LL65;} _LL65: if(( unsigned int) _temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_Abs_n:
0){ _LL74: _temp73=(( struct Cyc_Absyn_Abs_n_struct*) _temp61)->f1; if( _temp73
== 0){ goto _LL66;} else{ goto _LL67;}} else{ goto _LL67;} _LL67: if( _temp61 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70;
_LL64: goto _LL62; _LL66: goto _LL62; _LL68: goto _LL62; _LL70: Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a module name", sizeof(
unsigned char), 49u), loc); goto _LL62; _LL62:;}} if( _temp51 != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters", sizeof( unsigned char), 41u),
loc);} return({ struct _tuple2* _temp75=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp75->f1= _temp59; _temp75->f2= _temp55; _temp75->f3=
_temp53; _temp75;});}} static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment*
loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp76=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp76[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp77; _temp77.tag= Cyc_Parse_Type_spec; _temp77.f1=(
void*) t; _temp77.f2= loc; _temp77;}); _temp76;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp78= t; struct Cyc_Absyn_Exp* _temp84; struct Cyc_Absyn_Tqual
_temp86; void* _temp88; _LL80: if(( unsigned int) _temp78 > 4u?*(( int*) _temp78)
== Cyc_Absyn_ArrayType: 0){ _LL89: _temp88=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp78)->f1; goto _LL87; _LL87: _temp86=(( struct Cyc_Absyn_ArrayType_struct*)
_temp78)->f2; goto _LL85; _LL85: _temp84=(( struct Cyc_Absyn_ArrayType_struct*)
_temp78)->f3; goto _LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL81: return
Cyc_Absyn_starb_typ( _temp88,( void*) Cyc_Absyn_HeapRgn, _temp86, _temp84 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp90=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp90[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp91; _temp91.tag= Cyc_Absyn_Upper_b;
_temp91.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp84); _temp91;}); _temp90;}));
_LL83: return t; _LL79:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp92=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp92->f1=(* t).f2; _temp92->f2=(* t).f3;
_temp92;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp93=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp119; _LL95: if(*(( int*) _temp93) == Cyc_Absyn_Var_d){
_LL120: _temp119=(( struct Cyc_Absyn_Var_d_struct*) _temp93)->f1; goto _LL96;}
else{ goto _LL97;} _LL97: if(*(( int*) _temp93) == Cyc_Absyn_Let_d){ goto _LL98;}
else{ goto _LL99;} _LL99: if(*(( int*) _temp93) == Cyc_Absyn_Letv_d){ goto
_LL100;} else{ goto _LL101;} _LL101: if(*(( int*) _temp93) == Cyc_Absyn_Fn_d){
goto _LL102;} else{ goto _LL103;} _LL103: if(*(( int*) _temp93) == Cyc_Absyn_Struct_d){
goto _LL104;} else{ goto _LL105;} _LL105: if(*(( int*) _temp93) == Cyc_Absyn_Union_d){
goto _LL106;} else{ goto _LL107;} _LL107: if(*(( int*) _temp93) == Cyc_Absyn_Tunion_d){
goto _LL108;} else{ goto _LL109;} _LL109: if(*(( int*) _temp93) == Cyc_Absyn_Typedef_d){
goto _LL110;} else{ goto _LL111;} _LL111: if(*(( int*) _temp93) == Cyc_Absyn_Enum_d){
goto _LL112;} else{ goto _LL113;} _LL113: if(*(( int*) _temp93) == Cyc_Absyn_Namespace_d){
goto _LL114;} else{ goto _LL115;} _LL115: if(*(( int*) _temp93) == Cyc_Absyn_Using_d){
goto _LL116;} else{ goto _LL117;} _LL117: if(*(( int*) _temp93) == Cyc_Absyn_ExternC_d){
goto _LL118;} else{ goto _LL94;} _LL96: if( _temp119->initializer != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);}{ void* _temp121=(* _temp119->name).f1; struct
Cyc_List_List* _temp131; struct Cyc_List_List* _temp133; _LL123: if( _temp121 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL124;} else{ goto _LL125;} _LL125: if((
unsigned int) _temp121 > 1u?*(( int*) _temp121) == Cyc_Absyn_Rel_n: 0){ _LL132:
_temp131=(( struct Cyc_Absyn_Rel_n_struct*) _temp121)->f1; if( _temp131 == 0){
goto _LL126;} else{ goto _LL127;}} else{ goto _LL127;} _LL127: if(( unsigned int)
_temp121 > 1u?*(( int*) _temp121) == Cyc_Absyn_Abs_n: 0){ _LL134: _temp133=((
struct Cyc_Absyn_Abs_n_struct*) _temp121)->f1; if( _temp133 == 0){ goto _LL128;}
else{ goto _LL129;}} else{ goto _LL129;} _LL129: goto _LL130; _LL124: goto
_LL122; _LL126: goto _LL122; _LL128: goto _LL122; _LL130: Cyc_Parse_err(
_tag_arr("module names not allowed on parameter declarations", sizeof(
unsigned char), 51u), x->loc); goto _LL122; _LL122:;}{ int found= 0; for( 0;
params != 0; params=(( struct Cyc_List_List*) _check_null( params))->tl){ if(
Cyc_String_zstrptrcmp((* _temp119->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( params))->hd) == 0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp136; _temp136.tag=
Cyc_Stdio_String_pa; _temp136.f1=( struct _tagged_arr)*(* _temp119->name).f2;{
void* _temp135[ 1u]={& _temp136}; Cyc_Stdio_aprintf( _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u), _tag_arr( _temp135, sizeof( void*), 1u));}}), x->loc);}
return;} _LL98: decl_kind= _tag_arr("let declaration", sizeof( unsigned char),
16u); goto _LL94; _LL100: decl_kind= _tag_arr("let declaration", sizeof(
unsigned char), 16u); goto _LL94; _LL102: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL94; _LL104: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL94; _LL106: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL94; _LL108: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL94; _LL110: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL94; _LL112: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL94; _LL114: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL94; _LL116: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL94; _LL118: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL94; _LL94:;}(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp138; _temp138.tag= Cyc_Stdio_String_pa;
_temp138.f1=( struct _tagged_arr) decl_kind;{ void* _temp137[ 1u]={& _temp138};
Cyc_Stdio_aprintf( _tag_arr("%s appears in parameter type", sizeof(
unsigned char), 29u), _tag_arr( _temp137, sizeof( void*), 1u));}}), x->loc);
return;} struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env,
struct _tagged_arr* x){ struct _tuple12 _temp141; struct Cyc_Position_Segment*
_temp142; struct Cyc_List_List* _temp144; struct _tuple12* _temp139= env;
_temp141=* _temp139; _LL145: _temp144= _temp141.f1; goto _LL143; _LL143:
_temp142= _temp141.f2; goto _LL140; _LL140: if( _temp144 == 0){ return(( struct
_tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp147; _temp147.tag=
Cyc_Stdio_String_pa; _temp147.f1=( struct _tagged_arr)* x;{ void* _temp146[ 1u]={&
_temp147}; Cyc_Stdio_aprintf( _tag_arr("missing type for parameter %s", sizeof(
unsigned char), 30u), _tag_arr( _temp146, sizeof( void*), 1u));}}), _temp142);}{
void* _temp148=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp144))->hd)->r; struct Cyc_Absyn_Vardecl* _temp154; _LL150: if(*((
int*) _temp148) == Cyc_Absyn_Var_d){ _LL155: _temp154=(( struct Cyc_Absyn_Var_d_struct*)
_temp148)->f1; goto _LL151;} else{ goto _LL152;} _LL152: goto _LL153; _LL151:{
void* _temp156=(* _temp154->name).f1; struct Cyc_List_List* _temp166; struct Cyc_List_List*
_temp168; _LL158: if( _temp156 ==( void*) Cyc_Absyn_Loc_n){ goto _LL159;} else{
goto _LL160;} _LL160: if(( unsigned int) _temp156 > 1u?*(( int*) _temp156) ==
Cyc_Absyn_Rel_n: 0){ _LL167: _temp166=(( struct Cyc_Absyn_Rel_n_struct*)
_temp156)->f1; if( _temp166 == 0){ goto _LL161;} else{ goto _LL162;}} else{ goto
_LL162;} _LL162: if(( unsigned int) _temp156 > 1u?*(( int*) _temp156) == Cyc_Absyn_Abs_n:
0){ _LL169: _temp168=(( struct Cyc_Absyn_Abs_n_struct*) _temp156)->f1; if(
_temp168 == 0){ goto _LL163;} else{ goto _LL164;}} else{ goto _LL164;} _LL164:
goto _LL165; _LL159: goto _LL157; _LL161: goto _LL157; _LL163: goto _LL157;
_LL165: Cyc_Parse_err( _tag_arr("module name not allowed on parameter", sizeof(
unsigned char), 37u), _temp142); goto _LL157; _LL157:;} if( Cyc_String_zstrptrcmp((*
_temp154->name).f2, x) == 0){ return({ struct _tuple2* _temp170=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2) * 1); _temp170[ 0]=({ struct _tuple2 _temp171;
_temp171.f1=({ struct Cyc_Core_Opt* _temp172=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp172[ 0]=({ struct Cyc_Core_Opt _temp173;
_temp173.v=( void*)(* _temp154->name).f2; _temp173;}); _temp172;}); _temp171.f2=
_temp154->tq; _temp171.f3=( void*) _temp154->type; _temp171;}); _temp170;});}
else{ return Cyc_Parse_get_param_type(({ struct _tuple12* _temp174=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp174->f1=(( struct Cyc_List_List*)
_check_null( _temp144))->tl; _temp174->f2= _temp142; _temp174;}), x);} _LL153:
return(( struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration", sizeof(
unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp144))->hd)->loc); _LL149:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp175= tm; _LL177: if(( unsigned int) _temp175 > 1u?*(( int*)
_temp175) == Cyc_Absyn_TypeParams_mod: 0){ goto _LL178;} else{ goto _LL179;}
_LL179: goto _LL180; _LL178: return 1; _LL180: return 0; _LL176:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) == 0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp181=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp181[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp182; _temp182.tag= Cyc_Absyn_VarType; _temp182.f1=({ struct Cyc_Absyn_Tvar*
_temp183=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp183->name=({ struct _tagged_arr* _temp184=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp184[ 0]= s; _temp184;}); _temp183->identity=
0; _temp183->kind= k; _temp183;}); _temp182;}); _temp181;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp185=
t; struct Cyc_Absyn_Tvar* _temp191; _LL187: if(( unsigned int) _temp185 > 4u?*((
int*) _temp185) == Cyc_Absyn_VarType: 0){ _LL192: _temp191=(( struct Cyc_Absyn_VarType_struct*)
_temp185)->f1; goto _LL188;} else{ goto _LL189;} _LL189: goto _LL190; _LL188:
return _temp191; _LL190: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL186:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp193=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp193[ 0]=({ struct Cyc_Absyn_VarType_struct _temp194; _temp194.tag= Cyc_Absyn_VarType;
_temp194.f1= pr; _temp194;}); _temp193;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp195=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp201; _LL197:
if(( unsigned int) _temp195 > 1u?*(( int*) _temp195) == Cyc_Absyn_Function_mod:
0){ _LL202: _temp201=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp195)->f1;
goto _LL198;} else{ goto _LL199;} _LL199: goto _LL200; _LL198: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp203= _temp201; struct Cyc_List_List* _temp209; _LL205: if(*((
int*) _temp203) == Cyc_Absyn_WithTypes){ goto _LL206;} else{ goto _LL207;}
_LL207: if(*(( int*) _temp203) == Cyc_Absyn_NoTypes){ _LL210: _temp209=(( struct
Cyc_Absyn_NoTypes_struct*) _temp203)->f1; goto _LL208;} else{ goto _LL204;}
_LL206: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL208:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp209,
tds);{ struct _tuple12* _temp211=({ struct _tuple12* _temp217=( struct _tuple12*)
GC_malloc( sizeof( struct _tuple12)); _temp217->f1= tds; _temp217->f2= loc;
_temp217;}); return({ struct Cyc_List_List* _temp212=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp212->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp213=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp213[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp214; _temp214.tag= Cyc_Absyn_Function_mod;
_temp214.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp215=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp215[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp216; _temp216.tag= Cyc_Absyn_WithTypes;
_temp216.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp211, _temp209); _temp216.f2= 0; _temp216.f3= 0;
_temp216.f4= 0; _temp216.f5= 0; _temp216;}); _temp215;})); _temp214;}); _temp213;}));
_temp212->tl= 0; _temp212;});} _LL204:;} else{ return({ struct Cyc_List_List*
_temp218=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp218->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp218->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp218;});} _LL200: return({ struct Cyc_List_List*
_temp219=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp219->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp219->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp219;}); _LL196:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp220=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp220->id= d->id; _temp220->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp220;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp221=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL223: if( _temp221 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL224;} else{ goto _LL225;} _LL225: if(
_temp221 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL226;} else{ goto _LL227;}
_LL227: if( _temp221 ==( void*) Cyc_Parse_Static_sc){ goto _LL228;} else{ goto
_LL229;} _LL229: goto _LL230; _LL224: sc=( void*) Cyc_Absyn_Extern; goto _LL222;
_LL226: sc=( void*) Cyc_Absyn_ExternC; goto _LL222; _LL228: sc=( void*) Cyc_Absyn_Static;
goto _LL222; _LL230: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL222; _LL222:;}}{ struct Cyc_Core_Opt*
_temp233; void* _temp235; struct _tuple5 _temp231= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL236: _temp235= _temp231.f1; goto _LL234; _LL234: _temp233=
_temp231.f2; goto _LL232; _LL232: { struct Cyc_List_List* _temp239; struct Cyc_List_List*
_temp241; void* _temp243; struct Cyc_Absyn_Tqual _temp245; struct _tuple6
_temp237= Cyc_Parse_apply_tms( tq, _temp235, atts, d->tms); _LL246: _temp245=
_temp237.f1; goto _LL244; _LL244: _temp243= _temp237.f2; goto _LL242; _LL242:
_temp241= _temp237.f3; goto _LL240; _LL240: _temp239= _temp237.f4; goto _LL238;
_LL238: if( _temp233 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp241 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp247= _temp243; struct Cyc_Absyn_FnInfo _temp253; struct Cyc_List_List*
_temp255; struct Cyc_List_List* _temp257; struct Cyc_Absyn_VarargInfo* _temp259;
int _temp261; struct Cyc_List_List* _temp263; void* _temp265; struct Cyc_Core_Opt*
_temp267; struct Cyc_List_List* _temp269; _LL249: if(( unsigned int) _temp247 >
4u?*(( int*) _temp247) == Cyc_Absyn_FnType: 0){ _LL254: _temp253=(( struct Cyc_Absyn_FnType_struct*)
_temp247)->f1; _LL270: _temp269= _temp253.tvars; goto _LL268; _LL268: _temp267=
_temp253.effect; goto _LL266; _LL266: _temp265=( void*) _temp253.ret_typ; goto
_LL264; _LL264: _temp263= _temp253.args; goto _LL262; _LL262: _temp261= _temp253.c_varargs;
goto _LL260; _LL260: _temp259= _temp253.cyc_varargs; goto _LL258; _LL258:
_temp257= _temp253.rgn_po; goto _LL256; _LL256: _temp255= _temp253.attributes;
goto _LL250;} else{ goto _LL251;} _LL251: goto _LL252; _LL250: { struct Cyc_List_List*
_temp271=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp263); return({ struct Cyc_Absyn_Fndecl*
_temp272=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp272->sc=( void*) sc; _temp272->name= d->id; _temp272->tvs= _temp269;
_temp272->is_inline= is_inline; _temp272->effect= _temp267; _temp272->ret_type=(
void*) _temp265; _temp272->args= _temp271; _temp272->c_varargs= _temp261;
_temp272->cyc_varargs= _temp259; _temp272->rgn_po= _temp257; _temp272->body=
body; _temp272->cached_typ= 0; _temp272->param_vardecls= 0; _temp272->attributes=
Cyc_List_append( _temp255, _temp239); _temp272;});} _LL252: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL248:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp273=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp273->f1=({ struct _tagged_arr*
_temp274=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp274[ 0]= _tag_arr("?", sizeof( unsigned char), 2u); _temp274;}); _temp273->f2=(*
t).f2; _temp273->f3=(* t).f3; _temp273;});} else{ return({ struct _tuple4*
_temp275=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp275->f1=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp275->f2=(*
t).f2; _temp275->f3=(* t).f3; _temp275;});}} static unsigned char _temp276[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp276, _temp276, _temp276 + 52u};
static unsigned char _temp277[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp277, _temp277, _temp277 + 63u};
static unsigned char _temp278[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp278, _temp278, _temp278 + 50u};
static unsigned char _temp279[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp279, _temp279, _temp279 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp280=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp294; void* _temp296; struct Cyc_Position_Segment*
_temp298; struct Cyc_Position_Segment* _temp300; struct Cyc_Position_Segment*
_temp302; struct Cyc_Position_Segment* _temp304; struct Cyc_Absyn_Decl* _temp306;
_LL282: if(*(( int*) _temp280) == Cyc_Parse_Type_spec){ _LL297: _temp296=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp280)->f1; goto _LL295; _LL295: _temp294=((
struct Cyc_Parse_Type_spec_struct*) _temp280)->f2; goto _LL283;} else{ goto
_LL284;} _LL284: if(*(( int*) _temp280) == Cyc_Parse_Signed_spec){ _LL299:
_temp298=(( struct Cyc_Parse_Signed_spec_struct*) _temp280)->f1; goto _LL285;}
else{ goto _LL286;} _LL286: if(*(( int*) _temp280) == Cyc_Parse_Unsigned_spec){
_LL301: _temp300=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp280)->f1; goto
_LL287;} else{ goto _LL288;} _LL288: if(*(( int*) _temp280) == Cyc_Parse_Short_spec){
_LL303: _temp302=(( struct Cyc_Parse_Short_spec_struct*) _temp280)->f1; goto
_LL289;} else{ goto _LL290;} _LL290: if(*(( int*) _temp280) == Cyc_Parse_Long_spec){
_LL305: _temp304=(( struct Cyc_Parse_Long_spec_struct*) _temp280)->f1; goto
_LL291;} else{ goto _LL292;} _LL292: if(*(( int*) _temp280) == Cyc_Parse_Decl_spec){
_LL307: _temp306=(( struct Cyc_Parse_Decl_spec_struct*) _temp280)->f1; goto
_LL293;} else{ goto _LL281;} _LL283: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp294);} last_loc= _temp294; seen_type= 1; t= _temp296; goto _LL281; _LL285:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp298);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp298);} last_loc= _temp298; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL281; _LL287: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp300);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp300);} last_loc= _temp300; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL281; _LL289: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp302);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp302);} last_loc= _temp302; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL281; _LL291: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp304);} if( seen_size){ void* _temp308= sz; _LL310: if( _temp308 ==( void*)
Cyc_Absyn_B4){ goto _LL311;} else{ goto _LL312;} _LL312: goto _LL313; _LL311: sz=(
void*) Cyc_Absyn_B8; goto _LL309; _LL313: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp304); goto _LL309; _LL309:;} last_loc=
_temp304; seen_size= 1; goto _LL281; _LL293: last_loc= _temp306->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp314=( void*) _temp306->r;
struct Cyc_Absyn_Structdecl* _temp326; struct Cyc_Absyn_Tuniondecl* _temp328;
struct Cyc_Absyn_Uniondecl* _temp330; struct Cyc_Absyn_Enumdecl* _temp332;
_LL316: if(*(( int*) _temp314) == Cyc_Absyn_Struct_d){ _LL327: _temp326=((
struct Cyc_Absyn_Struct_d_struct*) _temp314)->f1; goto _LL317;} else{ goto
_LL318;} _LL318: if(*(( int*) _temp314) == Cyc_Absyn_Tunion_d){ _LL329: _temp328=((
struct Cyc_Absyn_Tunion_d_struct*) _temp314)->f1; goto _LL319;} else{ goto
_LL320;} _LL320: if(*(( int*) _temp314) == Cyc_Absyn_Union_d){ _LL331: _temp330=((
struct Cyc_Absyn_Union_d_struct*) _temp314)->f1; goto _LL321;} else{ goto _LL322;}
_LL322: if(*(( int*) _temp314) == Cyc_Absyn_Enum_d){ _LL333: _temp332=(( struct
Cyc_Absyn_Enum_d_struct*) _temp314)->f1; goto _LL323;} else{ goto _LL324;}
_LL324: goto _LL325; _LL317: { struct Cyc_List_List* _temp334=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp326->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp335=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp335[ 0]=({ struct Cyc_Absyn_StructType_struct _temp336; _temp336.tag= Cyc_Absyn_StructType;
_temp336.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp326->name))->v); _temp336.f2= _temp334; _temp336.f3= 0;
_temp336;}); _temp335;}); if( _temp326->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp337=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp337->v=( void*) _temp306; _temp337;});} goto _LL315;} _LL319: { struct Cyc_List_List*
_temp338=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp328->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp339=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp339[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp340; _temp340.tag= Cyc_Absyn_TunionType;
_temp340.f1=({ struct Cyc_Absyn_TunionInfo _temp341; _temp341.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp342=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp342[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp343; _temp343.tag= Cyc_Absyn_KnownTunion;
_temp343.f1= _temp328; _temp343;}); _temp342;})); _temp341.targs= _temp338;
_temp341.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp341;}); _temp340;});
_temp339;}); if( _temp328->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp344=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp344->v=( void*) _temp306; _temp344;});} goto _LL315;} _LL321: { struct Cyc_List_List*
_temp345=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp330->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp346=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp346[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp347; _temp347.tag= Cyc_Absyn_UnionType; _temp347.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp330->name))->v); _temp347.f2= _temp345; _temp347.f3= 0; _temp347;});
_temp346;}); if( _temp330->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp348=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp348->v=( void*) _temp306; _temp348;});} goto _LL315;} _LL323: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp349=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp349[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp350; _temp350.tag= Cyc_Absyn_EnumType; _temp350.f1=
_temp332->name; _temp350.f2= 0; _temp350;}); _temp349;}); declopt=({ struct Cyc_Core_Opt*
_temp351=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp351->v=( void*) _temp306; _temp351;}); goto _LL315; _LL325:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp306->loc); goto _LL315; _LL315:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp306->loc);} goto _LL281; _LL281:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp352=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp352[ 0]=({ struct Cyc_Absyn_IntType_struct _temp353; _temp353.tag= Cyc_Absyn_IntType;
_temp353.f1=( void*) sgn; _temp353.f2=( void*) sz; _temp353;}); _temp352;});}
else{ if( seen_sign){ void* _temp354= t; void* _temp360; _LL356: if((
unsigned int) _temp354 > 4u?*(( int*) _temp354) == Cyc_Absyn_IntType: 0){ _LL361:
_temp360=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp354)->f2; goto _LL357;}
else{ goto _LL358;} _LL358: goto _LL359; _LL357: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp362=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp362[ 0]=({ struct Cyc_Absyn_IntType_struct _temp363; _temp363.tag= Cyc_Absyn_IntType;
_temp363.f1=( void*) sgn; _temp363.f2=( void*) _temp360; _temp363;}); _temp362;});
goto _LL355; _LL359: Cyc_Parse_err( _tag_arr("sign specification on non-integral type",
sizeof( unsigned char), 40u), last_loc); goto _LL355; _LL355:;} if( seen_size){
void* _temp364= t; void* _temp370; _LL366: if(( unsigned int) _temp364 > 4u?*((
int*) _temp364) == Cyc_Absyn_IntType: 0){ _LL371: _temp370=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp364)->f1; goto _LL367;} else{ goto _LL368;} _LL368: goto _LL369; _LL367: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp372=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp372[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp373; _temp373.tag= Cyc_Absyn_IntType; _temp373.f1=( void*) _temp370;
_temp373.f2=( void*) sz; _temp373;}); _temp372;}); goto _LL365; _LL369: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL365; _LL365:;}} return({ struct _tuple5 _temp374; _temp374.f1=
t; _temp374.f2= declopt; _temp374;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp375=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp376= _temp375->id; struct Cyc_List_List* _temp379; struct
Cyc_List_List* _temp381; void* _temp383; struct Cyc_Absyn_Tqual _temp385; struct
_tuple6 _temp377= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp375->tms);
_LL386: _temp385= _temp377.f1; goto _LL384; _LL384: _temp383= _temp377.f2; goto
_LL382; _LL382: _temp381= _temp377.f3; goto _LL380; _LL380: _temp379= _temp377.f4;
goto _LL378; _LL378: return({ struct Cyc_List_List* _temp387=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp387->hd=( void*)({ struct
_tuple7* _temp388=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp388->f1= _temp376; _temp388->f2= _temp385; _temp388->f3= _temp383; _temp388->f4=
_temp381; _temp388->f5= _temp379; _temp388;}); _temp387->tl= Cyc_Parse_apply_tmss(
_temp385, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp387;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp389; _temp389.f1= tq; _temp389.f2=
t; _temp389.f3= 0; _temp389.f4= atts; _temp389;});}{ void* _temp390=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp404;
void* _temp406; struct Cyc_Position_Segment* _temp408; struct Cyc_List_List*
_temp410; struct Cyc_Absyn_Tqual _temp412; void* _temp414; void* _temp416;
struct Cyc_List_List* _temp418; struct Cyc_Position_Segment* _temp420; _LL392:
if( _temp390 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL393;} else{ goto _LL394;}
_LL394: if(( unsigned int) _temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_ConstArray_mod:
0){ _LL405: _temp404=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp390)->f1;
goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp390 > 1u?*((
int*) _temp390) == Cyc_Absyn_Function_mod: 0){ _LL407: _temp406=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp390)->f1; goto _LL397;} else{ goto
_LL398;} _LL398: if(( unsigned int) _temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_TypeParams_mod:
0){ _LL411: _temp410=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp390)->f1;
goto _LL409; _LL409: _temp408=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp390)->f2; goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int)
_temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_Pointer_mod: 0){ _LL417: _temp416=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp390)->f1; goto _LL415;
_LL415: _temp414=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp390)->f2;
goto _LL413; _LL413: _temp412=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp390)->f3;
goto _LL401;} else{ goto _LL402;} _LL402: if(( unsigned int) _temp390 > 1u?*((
int*) _temp390) == Cyc_Absyn_Attributes_mod: 0){ _LL421: _temp420=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp390)->f1; goto _LL419; _LL419: _temp418=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp390)->f2; goto _LL403;} else{ goto _LL391;} _LL393: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp422=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp422[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp423; _temp423.tag= Cyc_Absyn_ArrayType;
_temp423.f1=( void*) t; _temp423.f2= tq; _temp423.f3= 0; _temp423;}); _temp422;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL395: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp424=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp424[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp425; _temp425.tag= Cyc_Absyn_ArrayType;
_temp425.f1=( void*) t; _temp425.f2= tq; _temp425.f3=( struct Cyc_Absyn_Exp*)
_temp404; _temp425;}); _temp424;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL397: { void* _temp426= _temp406; struct Cyc_List_List* _temp432;
struct Cyc_Core_Opt* _temp434; struct Cyc_Absyn_VarargInfo* _temp436; int
_temp438; struct Cyc_List_List* _temp440; struct Cyc_Position_Segment* _temp442;
_LL428: if(*(( int*) _temp426) == Cyc_Absyn_WithTypes){ _LL441: _temp440=((
struct Cyc_Absyn_WithTypes_struct*) _temp426)->f1; goto _LL439; _LL439: _temp438=((
struct Cyc_Absyn_WithTypes_struct*) _temp426)->f2; goto _LL437; _LL437: _temp436=((
struct Cyc_Absyn_WithTypes_struct*) _temp426)->f3; goto _LL435; _LL435: _temp434=((
struct Cyc_Absyn_WithTypes_struct*) _temp426)->f4; goto _LL433; _LL433: _temp432=((
struct Cyc_Absyn_WithTypes_struct*) _temp426)->f5; goto _LL429;} else{ goto
_LL430;} _LL430: if(*(( int*) _temp426) == Cyc_Absyn_NoTypes){ _LL443: _temp442=((
struct Cyc_Absyn_NoTypes_struct*) _temp426)->f2; goto _LL431;} else{ goto _LL427;}
_LL429: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp444=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp444->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp444->tl= fn_atts; _temp444;});} else{ new_atts=({ struct Cyc_List_List*
_temp445=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp445->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp445->tl= new_atts; _temp445;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp446=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp452; _LL448: if(( unsigned int) _temp446 > 1u?*(( int*) _temp446) == Cyc_Absyn_TypeParams_mod:
0){ _LL453: _temp452=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp446)->f1;
goto _LL449;} else{ goto _LL450;} _LL450: goto _LL451; _LL449: typvars= _temp452;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL447; _LL451: goto
_LL447; _LL447:;} if((((( ! _temp438? _temp436 == 0: 0)? _temp440 != 0: 0)?((
struct Cyc_List_List*) _check_null( _temp440))->tl == 0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp440))->hd)).f1 == 0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp440))->hd)).f3 ==( void*)
Cyc_Absyn_VoidType: 0){ _temp440= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp440); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp434, t, _temp440, _temp438, _temp436, _temp432, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL431:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp442)); _LL427:;} _LL399: if((( struct Cyc_List_List*) _check_null(
tms))->tl == 0){ return({ struct _tuple6 _temp454; _temp454.f1= tq; _temp454.f2=
t; _temp454.f3= _temp410; _temp454.f4= atts; _temp454;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp408)); _LL401: { void* _temp455= _temp416;
struct Cyc_Absyn_Exp* _temp463; struct Cyc_Absyn_Exp* _temp465; _LL457: if((
unsigned int) _temp455 > 1u?*(( int*) _temp455) == Cyc_Absyn_NonNullable_ps: 0){
_LL464: _temp463=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp455)->f1; goto
_LL458;} else{ goto _LL459;} _LL459: if(( unsigned int) _temp455 > 1u?*(( int*)
_temp455) == Cyc_Absyn_Nullable_ps: 0){ _LL466: _temp465=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp455)->f1; goto _LL460;} else{ goto _LL461;} _LL461: if( _temp455 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL462;} else{ goto _LL456;} _LL458: return Cyc_Parse_apply_tms(
_temp412, Cyc_Absyn_atb_typ( t, _temp414, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp467=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp467[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp468; _temp468.tag= Cyc_Absyn_Upper_b;
_temp468.f1= _temp463; _temp468;}); _temp467;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL460: return Cyc_Parse_apply_tms( _temp412, Cyc_Absyn_starb_typ(
t, _temp414, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp469=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp469[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp470; _temp470.tag= Cyc_Absyn_Upper_b;
_temp470.f1= _temp465; _temp470;}); _temp469;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL462: return Cyc_Parse_apply_tms( _temp412, Cyc_Absyn_tagged_typ(
t, _temp414, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL456:;}
_LL403: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp418),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL391:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp473; void* _temp475; struct _tuple5 _temp471= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL476: _temp475= _temp471.f1; goto _LL474; _LL474: _temp473=
_temp471.f2; goto _LL472; _LL472: if( _temp473 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp475;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp477=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp477[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp478; _temp478.tag= Cyc_Absyn_Decl_s;
_temp478.f1= d; _temp478.f2= s; _temp478;}); _temp477;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc != 0){ void* _temp479=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL481: if( _temp479 ==(
void*) Cyc_Parse_Typedef_sc){ goto _LL482;} else{ goto _LL483;} _LL483: if(
_temp479 ==( void*) Cyc_Parse_Extern_sc){ goto _LL484;} else{ goto _LL485;}
_LL485: if( _temp479 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL486;} else{ goto
_LL487;} _LL487: if( _temp479 ==( void*) Cyc_Parse_Static_sc){ goto _LL488;}
else{ goto _LL489;} _LL489: if( _temp479 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL490;} else{ goto _LL491;} _LL491: if( _temp479 ==( void*) Cyc_Parse_Register_sc){
goto _LL492;} else{ goto _LL493;} _LL493: if( _temp479 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL494;} else{ goto _LL480;} _LL482: istypedef= 1; goto _LL480; _LL484: s=(
void*) Cyc_Absyn_Extern; goto _LL480; _LL486: s=( void*) Cyc_Absyn_ExternC; goto
_LL480; _LL488: s=( void*) Cyc_Absyn_Static; goto _LL480; _LL490: s=( void*) Cyc_Absyn_Public;
goto _LL480; _LL492: s=( void*) Cyc_Absyn_Public; goto _LL480; _LL494: s=( void*)
Cyc_Absyn_Abstract; goto _LL480; _LL480:;}{ struct Cyc_List_List* _temp497;
struct Cyc_List_List* _temp499; struct _tuple0 _temp495=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL500: _temp499= _temp495.f1;
goto _LL498; _LL498: _temp497= _temp495.f2; goto _LL496; _LL496: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp497; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp501= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp499 == 0){ struct Cyc_Core_Opt* _temp504; void* _temp506; struct _tuple5
_temp502= _temp501; _LL507: _temp506= _temp502.f1; goto _LL505; _LL505: _temp504=
_temp502.f2; goto _LL503; _LL503: if( _temp504 != 0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp504))->v;{
void* _temp508=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp520; struct Cyc_Absyn_Structdecl*
_temp522; struct Cyc_Absyn_Uniondecl* _temp524; struct Cyc_Absyn_Tuniondecl*
_temp526; _LL510: if(*(( int*) _temp508) == Cyc_Absyn_Enum_d){ _LL521: _temp520=((
struct Cyc_Absyn_Enum_d_struct*) _temp508)->f1; goto _LL511;} else{ goto _LL512;}
_LL512: if(*(( int*) _temp508) == Cyc_Absyn_Struct_d){ _LL523: _temp522=((
struct Cyc_Absyn_Struct_d_struct*) _temp508)->f1; goto _LL513;} else{ goto
_LL514;} _LL514: if(*(( int*) _temp508) == Cyc_Absyn_Union_d){ _LL525: _temp524=((
struct Cyc_Absyn_Union_d_struct*) _temp508)->f1; goto _LL515;} else{ goto _LL516;}
_LL516: if(*(( int*) _temp508) == Cyc_Absyn_Tunion_d){ _LL527: _temp526=((
struct Cyc_Absyn_Tunion_d_struct*) _temp508)->f1; goto _LL517;} else{ goto
_LL518;} _LL518: goto _LL519; _LL511:( void*)( _temp520->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL509; _LL513:( void*)( _temp522->sc=( void*)
s); _temp522->attributes= atts; goto _LL509; _LL515:( void*)( _temp524->sc=(
void*) s); _temp524->attributes= atts; goto _LL509; _LL517:( void*)( _temp526->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL509; _LL519: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL509:;}
return({ struct Cyc_List_List* _temp528=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp528->hd=( void*) d; _temp528->tl= 0;
_temp528;});} else{ void* _temp529= _temp506; struct Cyc_List_List* _temp543;
struct _tuple1* _temp545; struct Cyc_Absyn_TunionInfo _temp547; void* _temp549;
struct Cyc_Absyn_Tuniondecl* _temp551; struct Cyc_Absyn_TunionInfo _temp553;
struct Cyc_List_List* _temp555; void* _temp557; struct Cyc_Absyn_UnknownTunionInfo
_temp559; int _temp561; struct _tuple1* _temp563; struct Cyc_List_List* _temp565;
struct _tuple1* _temp567; struct _tuple1* _temp569; _LL531: if(( unsigned int)
_temp529 > 4u?*(( int*) _temp529) == Cyc_Absyn_StructType: 0){ _LL546: _temp545=((
struct Cyc_Absyn_StructType_struct*) _temp529)->f1; goto _LL544; _LL544:
_temp543=(( struct Cyc_Absyn_StructType_struct*) _temp529)->f2; goto _LL532;}
else{ goto _LL533;} _LL533: if(( unsigned int) _temp529 > 4u?*(( int*) _temp529)
== Cyc_Absyn_TunionType: 0){ _LL548: _temp547=(( struct Cyc_Absyn_TunionType_struct*)
_temp529)->f1; _LL550: _temp549=( void*) _temp547.tunion_info; if(*(( int*)
_temp549) == Cyc_Absyn_KnownTunion){ _LL552: _temp551=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp549)->f1; goto _LL534;} else{ goto _LL535;}} else{ goto _LL535;} _LL535:
if(( unsigned int) _temp529 > 4u?*(( int*) _temp529) == Cyc_Absyn_TunionType: 0){
_LL554: _temp553=(( struct Cyc_Absyn_TunionType_struct*) _temp529)->f1; _LL558:
_temp557=( void*) _temp553.tunion_info; if(*(( int*) _temp557) == Cyc_Absyn_UnknownTunion){
_LL560: _temp559=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp557)->f1;
_LL564: _temp563= _temp559.name; goto _LL562; _LL562: _temp561= _temp559.is_xtunion;
goto _LL556;} else{ goto _LL537;} _LL556: _temp555= _temp553.targs; goto _LL536;}
else{ goto _LL537;} _LL537: if(( unsigned int) _temp529 > 4u?*(( int*) _temp529)
== Cyc_Absyn_UnionType: 0){ _LL568: _temp567=(( struct Cyc_Absyn_UnionType_struct*)
_temp529)->f1; goto _LL566; _LL566: _temp565=(( struct Cyc_Absyn_UnionType_struct*)
_temp529)->f2; goto _LL538;} else{ goto _LL539;} _LL539: if(( unsigned int)
_temp529 > 4u?*(( int*) _temp529) == Cyc_Absyn_EnumType: 0){ _LL570: _temp569=((
struct Cyc_Absyn_EnumType_struct*) _temp529)->f1; goto _LL540;} else{ goto
_LL541;} _LL541: goto _LL542; _LL532: { struct Cyc_List_List* _temp571=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp543); struct Cyc_Absyn_Structdecl* _temp572=({
struct Cyc_Absyn_Structdecl* _temp576=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp576->sc=( void*) s; _temp576->name=({
struct Cyc_Core_Opt* _temp577=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp577->v=( void*)(( struct _tuple1*) _check_null( _temp545));
_temp577;}); _temp576->tvs= _temp571; _temp576->fields= 0; _temp576->attributes=
0; _temp576;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp573=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp573->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp574=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp574[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp575; _temp575.tag= Cyc_Absyn_Struct_d;
_temp575.f1= _temp572; _temp575;}); _temp574;}), loc); _temp573->tl= 0; _temp573;});}
_LL534: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp578=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp578->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp579=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp579[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp580; _temp580.tag= Cyc_Absyn_Tunion_d;
_temp580.f1= _temp551; _temp580;}); _temp579;}), loc); _temp578->tl= 0; _temp578;});
_LL536: { struct Cyc_List_List* _temp581=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp555);
struct Cyc_Absyn_Decl* _temp582= Cyc_Absyn_tunion_decl( s, _temp563, _temp581, 0,
_temp561, loc); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp583=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp583->hd=(
void*) _temp582; _temp583->tl= 0; _temp583;});} _LL538: { struct Cyc_List_List*
_temp584=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp565); struct Cyc_Absyn_Uniondecl* _temp585=({
struct Cyc_Absyn_Uniondecl* _temp590=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp590->sc=( void*) s; _temp590->name=({
struct Cyc_Core_Opt* _temp591=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp591->v=( void*)(( struct _tuple1*) _check_null( _temp567));
_temp591;}); _temp590->tvs= _temp584; _temp590->fields= 0; _temp590->attributes=
0; _temp590;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp586=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp586->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp587=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp587->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp588=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp588[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp589; _temp589.tag= Cyc_Absyn_Union_d;
_temp589.f1= _temp585; _temp589;}); _temp588;})); _temp587->loc= loc; _temp587;});
_temp586->tl= 0; _temp586;});} _LL540: { struct Cyc_Absyn_Enumdecl* _temp592=({
struct Cyc_Absyn_Enumdecl* _temp597=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp597->sc=( void*) s; _temp597->name=
_temp569; _temp597->fields= 0; _temp597;}); if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp593=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp593->hd=( void*)({ struct Cyc_Absyn_Decl* _temp594=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp594->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp595=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp595[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp596; _temp596.tag= Cyc_Absyn_Enum_d; _temp596.f1= _temp592; _temp596;});
_temp595;})); _temp594->loc= loc; _temp594;}); _temp593->tl= 0; _temp593;});}
_LL542: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL530:;}} else{ void* t= _temp501.f1; struct Cyc_List_List*
_temp598= Cyc_Parse_apply_tmss( tq, t, _temp499, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp598); if( _temp501.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp501.f2))->v;{ void* _temp599=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp611; struct Cyc_Absyn_Tuniondecl* _temp613;
struct Cyc_Absyn_Uniondecl* _temp615; struct Cyc_Absyn_Enumdecl* _temp617;
_LL601: if(*(( int*) _temp599) == Cyc_Absyn_Struct_d){ _LL612: _temp611=((
struct Cyc_Absyn_Struct_d_struct*) _temp599)->f1; goto _LL602;} else{ goto
_LL603;} _LL603: if(*(( int*) _temp599) == Cyc_Absyn_Tunion_d){ _LL614: _temp613=((
struct Cyc_Absyn_Tunion_d_struct*) _temp599)->f1; goto _LL604;} else{ goto
_LL605;} _LL605: if(*(( int*) _temp599) == Cyc_Absyn_Union_d){ _LL616: _temp615=((
struct Cyc_Absyn_Union_d_struct*) _temp599)->f1; goto _LL606;} else{ goto _LL607;}
_LL607: if(*(( int*) _temp599) == Cyc_Absyn_Enum_d){ _LL618: _temp617=(( struct
Cyc_Absyn_Enum_d_struct*) _temp599)->f1; goto _LL608;} else{ goto _LL609;}
_LL609: goto _LL610; _LL602:( void*)( _temp611->sc=( void*) s); _temp611->attributes=
atts; atts= 0; goto _LL600; _LL604:( void*)( _temp613->sc=( void*) s); goto
_LL600; _LL606:( void*)( _temp615->sc=( void*) s); goto _LL600; _LL608:( void*)(
_temp617->sc=( void*) s); goto _LL600; _LL610: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL600; _LL600:;} decls=({ struct Cyc_List_List*
_temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*) d; _temp619->tl= decls; _temp619;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp621; _temp621.tag=
Cyc_Stdio_String_pa; _temp621.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp620[ 1u]={&
_temp621}; Cyc_Stdio_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp620, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp501.f2 != 0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp622= _temp598; for( 0; _temp622 != 0;( _temp622=(( struct
Cyc_List_List*) _check_null( _temp622))->tl, _temp497=(( struct Cyc_List_List*)
_check_null( _temp497))->tl)){ struct _tuple7 _temp625; struct Cyc_List_List*
_temp626; struct Cyc_List_List* _temp628; void* _temp630; struct Cyc_Absyn_Tqual
_temp632; struct _tuple1* _temp634; struct _tuple7* _temp623=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp622))->hd; _temp625=* _temp623; _LL635:
_temp634= _temp625.f1; goto _LL633; _LL633: _temp632= _temp625.f2; goto _LL631;
_LL631: _temp630= _temp625.f3; goto _LL629; _LL629: _temp628= _temp625.f4; goto
_LL627; _LL627: _temp626= _temp625.f5; goto _LL624; _LL624: if( _temp628 != 0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp497 == 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp636=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp497))->hd; struct Cyc_Absyn_Vardecl*
_temp637= Cyc_Absyn_new_vardecl( _temp634, _temp630, _temp636); _temp637->tq=
_temp632;( void*)( _temp637->sc=( void*) s); _temp637->attributes= _temp626;{
struct Cyc_Absyn_Decl* _temp638=({ struct Cyc_Absyn_Decl* _temp640=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp640->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp641=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp641[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp642; _temp642.tag= Cyc_Absyn_Var_d; _temp642.f1= _temp637; _temp642;});
_temp641;})); _temp640->loc= loc; _temp640;}); decls=({ struct Cyc_List_List*
_temp639=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp639->hd=( void*) _temp638; _temp639->tl= decls; _temp639;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp644; _temp644.tag= Cyc_Stdio_String_pa;
_temp644.f1=( struct _tagged_arr) s;{ void* _temp643[ 1u]={& _temp644}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp643,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL645: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL646: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL647: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL648: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL649: return(
void*) Cyc_Absyn_EffKind; default: _LL650: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp653; _temp653.tag= Cyc_Stdio_String_pa;
_temp653.f1=( struct _tagged_arr) s;{ void* _temp652[ 1u]={& _temp653}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp652,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){ return
tms;} else{ return({ struct Cyc_List_List* _temp654=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp654->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp655=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp655[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp656; _temp656.tag= Cyc_Absyn_Attributes_mod;
_temp656.f1= loc; _temp656.f2= atts; _temp656;}); _temp655;})); _temp654->tl=
tms; _temp654;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp658; _temp658.tag= Cyc_Stdio_String_pa;
_temp658.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null((* t).f5))->hd);{ void* _temp657[ 1u]={& _temp658};
Cyc_Stdio_aprintf( _tag_arr("bad attribute %s within typedef", sizeof(
unsigned char), 32u), _tag_arr( _temp657, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp659=(
struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp659[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp660; _temp660.tag= Cyc_Absyn_Typedef_d;
_temp660.f1=({ struct Cyc_Absyn_Typedefdecl* _temp661=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp661->name= x; _temp661->tvs=(*
t).f4; _temp661->defn=( void*)(* t).f3; _temp661;}); _temp660;}); _temp659;}),
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
unsigned char _temp663[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp663, _temp663, _temp663 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp664= yy1; struct _tuple16* _temp670; _LL666: if(*(( void**)
_temp664) == Cyc_Int_tok){ _LL671: _temp670=(( struct Cyc_Int_tok_struct*)
_temp664)->f1; goto _LL667;} else{ goto _LL668;} _LL668: goto _LL669; _LL667:
return _temp670; _LL669:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL665:;}
static unsigned char _temp673[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp673, _temp673, _temp673 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp674= yy1; struct
_tagged_arr _temp680; _LL676: if(*(( void**) _temp674) == Cyc_String_tok){
_LL681: _temp680=(( struct Cyc_String_tok_struct*) _temp674)->f1; goto _LL677;}
else{ goto _LL678;} _LL678: goto _LL679; _LL677: return _temp680; _LL679:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL675:;} static unsigned char _temp683[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp683, _temp683, _temp683 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp684= yy1; unsigned char _temp690; _LL686: if(*(( void**) _temp684) ==
Cyc_Char_tok){ _LL691: _temp690=(( struct Cyc_Char_tok_struct*) _temp684)->f1;
goto _LL687;} else{ goto _LL688;} _LL688: goto _LL689; _LL687: return _temp690;
_LL689:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL685:;} static
unsigned char _temp693[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp693, _temp693, _temp693 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp694= yy1; void*
_temp700; _LL696: if(*(( void**) _temp694) == Cyc_Pointer_Sort_tok){ _LL701:
_temp700=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp694)->f1; goto
_LL697;} else{ goto _LL698;} _LL698: goto _LL699; _LL697: return _temp700;
_LL699:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL695:;} static
unsigned char _temp703[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp703, _temp703, _temp703 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp704= yy1; struct Cyc_Absyn_Exp*
_temp710; _LL706: if(*(( void**) _temp704) == Cyc_Exp_tok){ _LL711: _temp710=((
struct Cyc_Exp_tok_struct*) _temp704)->f1; goto _LL707;} else{ goto _LL708;}
_LL708: goto _LL709; _LL707: return _temp710; _LL709:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL705:;} static unsigned char _temp713[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp713, _temp713, _temp713 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp714= yy1;
struct Cyc_List_List* _temp720; _LL716: if(*(( void**) _temp714) == Cyc_ExpList_tok){
_LL721: _temp720=(( struct Cyc_ExpList_tok_struct*) _temp714)->f1; goto _LL717;}
else{ goto _LL718;} _LL718: goto _LL719; _LL717: return _temp720; _LL719:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL715:;} static unsigned char
_temp723[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp723, _temp723, _temp723 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp724= yy1; struct Cyc_List_List*
_temp730; _LL726: if(*(( void**) _temp724) == Cyc_InitializerList_tok){ _LL731:
_temp730=(( struct Cyc_InitializerList_tok_struct*) _temp724)->f1; goto _LL727;}
else{ goto _LL728;} _LL728: goto _LL729; _LL727: return _temp730; _LL729:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL725:;} static unsigned char
_temp733[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp733, _temp733, _temp733 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp734= yy1; void* _temp740; _LL736: if(*(( void**) _temp734)
== Cyc_Primop_tok){ _LL741: _temp740=( void*)(( struct Cyc_Primop_tok_struct*)
_temp734)->f1; goto _LL737;} else{ goto _LL738;} _LL738: goto _LL739; _LL737:
return _temp740; _LL739:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL735:;}
static unsigned char _temp743[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp743, _temp743, _temp743 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp744= yy1;
struct Cyc_Core_Opt* _temp750; _LL746: if(*(( void**) _temp744) == Cyc_Primopopt_tok){
_LL751: _temp750=(( struct Cyc_Primopopt_tok_struct*) _temp744)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: goto _LL749; _LL747: return _temp750; _LL749:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL745:;} static unsigned char
_temp753[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp753, _temp753, _temp753 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp754= yy1; struct _tuple1* _temp760; _LL756: if(*(( void**)
_temp754) == Cyc_QualId_tok){ _LL761: _temp760=(( struct Cyc_QualId_tok_struct*)
_temp754)->f1; goto _LL757;} else{ goto _LL758;} _LL758: goto _LL759; _LL757:
return _temp760; _LL759:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL755:;}
static unsigned char _temp763[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp763, _temp763, _temp763 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp764= yy1;
struct Cyc_Absyn_Stmt* _temp770; _LL766: if(*(( void**) _temp764) == Cyc_Stmt_tok){
_LL771: _temp770=(( struct Cyc_Stmt_tok_struct*) _temp764)->f1; goto _LL767;}
else{ goto _LL768;} _LL768: goto _LL769; _LL767: return _temp770; _LL769:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL765:;} static unsigned char _temp773[
14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp773, _temp773, _temp773 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp774= yy1; void* _temp780; _LL776: if(*(( void**) _temp774)
== Cyc_BlockItem_tok){ _LL781: _temp780=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp774)->f1; goto _LL777;} else{ goto _LL778;} _LL778: goto _LL779; _LL777:
return _temp780; _LL779:( int) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL775:;} static unsigned char _temp783[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp783, _temp783, _temp783 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp784= yy1; struct Cyc_List_List* _temp790; _LL786: if(*((
void**) _temp784) == Cyc_SwitchClauseList_tok){ _LL791: _temp790=(( struct Cyc_SwitchClauseList_tok_struct*)
_temp784)->f1; goto _LL787;} else{ goto _LL788;} _LL788: goto _LL789; _LL787:
return _temp790; _LL789:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL785:;} static unsigned char _temp793[ 22u]="SwitchCClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp793, _temp793, _temp793 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ void* _temp794= yy1; struct Cyc_List_List* _temp800; _LL796: if(*((
void**) _temp794) == Cyc_SwitchCClauseList_tok){ _LL801: _temp800=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp794)->f1; goto _LL797;} else{ goto _LL798;} _LL798: goto _LL799; _LL797:
return _temp800; _LL799:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL795:;} static unsigned char _temp803[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp803, _temp803, _temp803 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp804= yy1;
struct Cyc_Absyn_Pat* _temp810; _LL806: if(*(( void**) _temp804) == Cyc_Pattern_tok){
_LL811: _temp810=(( struct Cyc_Pattern_tok_struct*) _temp804)->f1; goto _LL807;}
else{ goto _LL808;} _LL808: goto _LL809; _LL807: return _temp810; _LL809:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL805:;} static unsigned char
_temp813[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp813, _temp813, _temp813 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp814= yy1; struct Cyc_List_List*
_temp820; _LL816: if(*(( void**) _temp814) == Cyc_PatternList_tok){ _LL821:
_temp820=(( struct Cyc_PatternList_tok_struct*) _temp814)->f1; goto _LL817;}
else{ goto _LL818;} _LL818: goto _LL819; _LL817: return _temp820; _LL819:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL815:;} static unsigned char
_temp823[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp823, _temp823, _temp823 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp824= yy1; struct _tuple14* _temp830; _LL826: if(*(( void**)
_temp824) == Cyc_FieldPattern_tok){ _LL831: _temp830=(( struct Cyc_FieldPattern_tok_struct*)
_temp824)->f1; goto _LL827;} else{ goto _LL828;} _LL828: goto _LL829; _LL827:
return _temp830; _LL829:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL825:;} static unsigned char _temp833[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp833, _temp833, _temp833 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp834= yy1; struct Cyc_List_List* _temp840; _LL836: if(*((
void**) _temp834) == Cyc_FieldPatternList_tok){ _LL841: _temp840=(( struct Cyc_FieldPatternList_tok_struct*)
_temp834)->f1; goto _LL837;} else{ goto _LL838;} _LL838: goto _LL839; _LL837:
return _temp840; _LL839:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL835:;} static unsigned char _temp843[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp843, _temp843, _temp843 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp844= yy1;
struct Cyc_Absyn_Fndecl* _temp850; _LL846: if(*(( void**) _temp844) == Cyc_FnDecl_tok){
_LL851: _temp850=(( struct Cyc_FnDecl_tok_struct*) _temp844)->f1; goto _LL847;}
else{ goto _LL848;} _LL848: goto _LL849; _LL847: return _temp850; _LL849:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL845:;} static unsigned char _temp853[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp853, _temp853, _temp853 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp854= yy1; struct Cyc_List_List*
_temp860; _LL856: if(*(( void**) _temp854) == Cyc_DeclList_tok){ _LL861:
_temp860=(( struct Cyc_DeclList_tok_struct*) _temp854)->f1; goto _LL857;} else{
goto _LL858;} _LL858: goto _LL859; _LL857: return _temp860; _LL859:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL855:;} static unsigned char _temp863[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp863, _temp863, _temp863 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp864= yy1; struct Cyc_Parse_Declaration_spec* _temp870;
_LL866: if(*(( void**) _temp864) == Cyc_DeclSpec_tok){ _LL871: _temp870=((
struct Cyc_DeclSpec_tok_struct*) _temp864)->f1; goto _LL867;} else{ goto _LL868;}
_LL868: goto _LL869; _LL867: return _temp870; _LL869:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL865:;} static unsigned char _temp873[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp873, _temp873, _temp873 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp874= yy1; struct
_tuple15* _temp880; _LL876: if(*(( void**) _temp874) == Cyc_InitDecl_tok){
_LL881: _temp880=(( struct Cyc_InitDecl_tok_struct*) _temp874)->f1; goto _LL877;}
else{ goto _LL878;} _LL878: goto _LL879; _LL877: return _temp880; _LL879:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL875:;} static unsigned char
_temp883[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp883, _temp883, _temp883 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp884= yy1; struct Cyc_List_List*
_temp890; _LL886: if(*(( void**) _temp884) == Cyc_InitDeclList_tok){ _LL891:
_temp890=(( struct Cyc_InitDeclList_tok_struct*) _temp884)->f1; goto _LL887;}
else{ goto _LL888;} _LL888: goto _LL889; _LL887: return _temp890; _LL889:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL885:;} static unsigned char
_temp893[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp893, _temp893, _temp893 + 17u}}; void* Cyc_yyget_StorageClass_tok(
void* yy1){ void* _temp894= yy1; void* _temp900; _LL896: if(*(( void**) _temp894)
== Cyc_StorageClass_tok){ _LL901: _temp900=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp894)->f1; goto _LL897;} else{ goto _LL898;} _LL898: goto _LL899; _LL897:
return _temp900; _LL899:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL895:;} static unsigned char _temp903[ 18u]="TypeSpecifier_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp903, _temp903, _temp903 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp904= yy1; void* _temp910; _LL906: if(*(( void**) _temp904) ==
Cyc_TypeSpecifier_tok){ _LL911: _temp910=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp904)->f1; goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907:
return _temp910; _LL909:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL905:;} static unsigned char _temp913[ 18u]="StructOrUnion_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp913, _temp913, _temp913 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp914= yy1; void* _temp920; _LL916: if(*(( void**) _temp914) ==
Cyc_StructOrUnion_tok){ _LL921: _temp920=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp914)->f1; goto _LL917;} else{ goto _LL918;} _LL918: goto _LL919; _LL917:
return _temp920; _LL919:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL915:;} static unsigned char _temp923[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp923, _temp923, _temp923 + 13u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ void* _temp924= yy1;
struct Cyc_Absyn_Tqual _temp930; _LL926: if(*(( void**) _temp924) == Cyc_TypeQual_tok){
_LL931: _temp930=(( struct Cyc_TypeQual_tok_struct*) _temp924)->f1; goto _LL927;}
else{ goto _LL928;} _LL928: goto _LL929; _LL927: return _temp930; _LL929:( int)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL925:;} static unsigned char
_temp933[ 24u]="StructFieldDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{ _temp933, _temp933,
_temp933 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok( void*
yy1){ void* _temp934= yy1; struct Cyc_List_List* _temp940; _LL936: if(*(( void**)
_temp934) == Cyc_StructFieldDeclList_tok){ _LL941: _temp940=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp934)->f1; goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937:
return _temp940; _LL939:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL935:;} static unsigned char _temp943[ 28u]="StructFieldDeclListList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp943, _temp943, _temp943 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ void* _temp944= yy1; struct
Cyc_List_List* _temp950; _LL946: if(*(( void**) _temp944) == Cyc_StructFieldDeclListList_tok){
_LL951: _temp950=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp944)->f1;
goto _LL947;} else{ goto _LL948;} _LL948: goto _LL949; _LL947: return _temp950;
_LL949:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL945:;}
static unsigned char _temp953[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp953, _temp953,
_temp953 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp954= yy1; struct Cyc_List_List* _temp960; _LL956: if(*(( void**)
_temp954) == Cyc_TypeModifierList_tok){ _LL961: _temp960=(( struct Cyc_TypeModifierList_tok_struct*)
_temp954)->f1; goto _LL957;} else{ goto _LL958;} _LL958: goto _LL959; _LL957:
return _temp960; _LL959:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL955:;} static unsigned char _temp963[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp963, _temp963, _temp963 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp964= yy1; void* _temp970; _LL966:
if(*(( void**) _temp964) == Cyc_Rgn_tok){ _LL971: _temp970=( void*)(( struct Cyc_Rgn_tok_struct*)
_temp964)->f1; goto _LL967;} else{ goto _LL968;} _LL968: goto _LL969; _LL967:
return _temp970; _LL969:( int) _throw(( void*)& Cyc_yyfail_Rgn_tok); _LL965:;}
static unsigned char _temp973[ 15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp973, _temp973, _temp973 + 15u}};
struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( void* yy1){ void*
_temp974= yy1; struct Cyc_Parse_Declarator* _temp980; _LL976: if(*(( void**)
_temp974) == Cyc_Declarator_tok){ _LL981: _temp980=(( struct Cyc_Declarator_tok_struct*)
_temp974)->f1; goto _LL977;} else{ goto _LL978;} _LL978: goto _LL979; _LL977:
return _temp980; _LL979:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL975:;} static unsigned char _temp983[ 21u]="DeclaratorExpopt_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{
_temp983, _temp983, _temp983 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp984= yy1; struct _tuple13* _temp990; _LL986: if(*(( void**)
_temp984) == Cyc_DeclaratorExpopt_tok){ _LL991: _temp990=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp984)->f1; goto _LL987;} else{ goto _LL988;} _LL988: goto _LL989; _LL987:
return _temp990; _LL989:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL985:;} static unsigned char _temp993[ 25u]="DeclaratorExpoptList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp993, _temp993, _temp993 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp994= yy1; struct Cyc_List_List* _temp1000; _LL996: if(*((
void**) _temp994) == Cyc_DeclaratorExpoptList_tok){ _LL1001: _temp1000=(( struct
Cyc_DeclaratorExpoptList_tok_struct*) _temp994)->f1; goto _LL997;} else{ goto
_LL998;} _LL998: goto _LL999; _LL997: return _temp1000; _LL999:( int) _throw((
void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL995:;} static unsigned char
_temp1003[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1003, _temp1003,
_temp1003 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1004= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1010;
_LL1006: if(*(( void**) _temp1004) == Cyc_AbstractDeclarator_tok){ _LL1011:
_temp1010=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1004)->f1; goto
_LL1007;} else{ goto _LL1008;} _LL1008: goto _LL1009; _LL1007: return _temp1010;
_LL1009:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1005:;}
static unsigned char _temp1013[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1013, _temp1013, _temp1013 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1014= yy1; int _temp1020; _LL1016:
if(*(( void**) _temp1014) == Cyc_Bool_tok){ _LL1021: _temp1020=(( struct Cyc_Bool_tok_struct*)
_temp1014)->f1; goto _LL1017;} else{ goto _LL1018;} _LL1018: goto _LL1019;
_LL1017: return _temp1020; _LL1019:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1015:;} static unsigned char _temp1023[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1023, _temp1023, _temp1023 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1024= yy1; void* _temp1030;
_LL1026: if(*(( void**) _temp1024) == Cyc_Scope_tok){ _LL1031: _temp1030=( void*)((
struct Cyc_Scope_tok_struct*) _temp1024)->f1; goto _LL1027;} else{ goto _LL1028;}
_LL1028: goto _LL1029; _LL1027: return _temp1030; _LL1029:( int) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1025:;} static unsigned char _temp1033[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1033, _temp1033, _temp1033 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1034= yy1; struct Cyc_Absyn_Tunionfield* _temp1040;
_LL1036: if(*(( void**) _temp1034) == Cyc_TunionField_tok){ _LL1041: _temp1040=((
struct Cyc_TunionField_tok_struct*) _temp1034)->f1; goto _LL1037;} else{ goto
_LL1038;} _LL1038: goto _LL1039; _LL1037: return _temp1040; _LL1039:( int)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1035:;} static unsigned char
_temp1043[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1043, _temp1043, _temp1043 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1044= yy1; struct Cyc_List_List*
_temp1050; _LL1046: if(*(( void**) _temp1044) == Cyc_TunionFieldList_tok){
_LL1051: _temp1050=(( struct Cyc_TunionFieldList_tok_struct*) _temp1044)->f1;
goto _LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049; _LL1047: return
_temp1050; _LL1049:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1045:;} static unsigned char _temp1053[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1053, _temp1053, _temp1053 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1054= yy1; struct _tuple18* _temp1060; _LL1056: if(*((
void**) _temp1054) == Cyc_QualSpecList_tok){ _LL1061: _temp1060=(( struct Cyc_QualSpecList_tok_struct*)
_temp1054)->f1; goto _LL1057;} else{ goto _LL1058;} _LL1058: goto _LL1059;
_LL1057: return _temp1060; _LL1059:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1055:;} static unsigned char _temp1063[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1063, _temp1063, _temp1063 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1064= yy1;
struct Cyc_List_List* _temp1070; _LL1066: if(*(( void**) _temp1064) == Cyc_IdList_tok){
_LL1071: _temp1070=(( struct Cyc_IdList_tok_struct*) _temp1064)->f1; goto
_LL1067;} else{ goto _LL1068;} _LL1068: goto _LL1069; _LL1067: return _temp1070;
_LL1069:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1065:;} static
unsigned char _temp1073[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1073, _temp1073, _temp1073 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1074= yy1;
struct _tuple2* _temp1080; _LL1076: if(*(( void**) _temp1074) == Cyc_ParamDecl_tok){
_LL1081: _temp1080=(( struct Cyc_ParamDecl_tok_struct*) _temp1074)->f1; goto
_LL1077;} else{ goto _LL1078;} _LL1078: goto _LL1079; _LL1077: return _temp1080;
_LL1079:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1075:;} static
unsigned char _temp1083[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1083, _temp1083,
_temp1083 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1084= yy1; struct Cyc_List_List* _temp1090; _LL1086: if(*(( void**)
_temp1084) == Cyc_ParamDeclList_tok){ _LL1091: _temp1090=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1084)->f1; goto _LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089;
_LL1087: return _temp1090; _LL1089:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1085:;} static unsigned char _temp1093[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1093, _temp1093, _temp1093 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1094= yy1; struct _tuple17* _temp1100; _LL1096: if(*((
void**) _temp1094) == Cyc_ParamDeclListBool_tok){ _LL1101: _temp1100=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1094)->f1; goto _LL1097;} else{ goto
_LL1098;} _LL1098: goto _LL1099; _LL1097: return _temp1100; _LL1099:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1095:;} static
unsigned char _temp1103[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1103, _temp1103, _temp1103 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1104=
yy1; struct Cyc_List_List* _temp1110; _LL1106: if(*(( void**) _temp1104) == Cyc_TypeList_tok){
_LL1111: _temp1110=(( struct Cyc_TypeList_tok_struct*) _temp1104)->f1; goto
_LL1107;} else{ goto _LL1108;} _LL1108: goto _LL1109; _LL1107: return _temp1110;
_LL1109:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1105:;} static
unsigned char _temp1113[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1113, _temp1113,
_temp1113 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1114= yy1; struct Cyc_List_List* _temp1120; _LL1116: if(*(( void**)
_temp1114) == Cyc_DesignatorList_tok){ _LL1121: _temp1120=(( struct Cyc_DesignatorList_tok_struct*)
_temp1114)->f1; goto _LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119;
_LL1117: return _temp1120; _LL1119:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1115:;} static unsigned char _temp1123[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1123,
_temp1123, _temp1123 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1124= yy1; void* _temp1130; _LL1126: if(*(( void**) _temp1124) == Cyc_Designator_tok){
_LL1131: _temp1130=( void*)(( struct Cyc_Designator_tok_struct*) _temp1124)->f1;
goto _LL1127;} else{ goto _LL1128;} _LL1128: goto _LL1129; _LL1127: return
_temp1130; _LL1129:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1125:;}
static unsigned char _temp1133[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1133, _temp1133, _temp1133 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1134= yy1; void* _temp1140;
_LL1136: if(*(( void**) _temp1134) == Cyc_Kind_tok){ _LL1141: _temp1140=( void*)((
struct Cyc_Kind_tok_struct*) _temp1134)->f1; goto _LL1137;} else{ goto _LL1138;}
_LL1138: goto _LL1139; _LL1137: return _temp1140; _LL1139:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1135:;} static unsigned char _temp1143[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1143, _temp1143, _temp1143 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1144= yy1; void* _temp1150; _LL1146: if(*(( void**) _temp1144) == Cyc_Type_tok){
_LL1151: _temp1150=( void*)(( struct Cyc_Type_tok_struct*) _temp1144)->f1; goto
_LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149; _LL1147: return _temp1150;
_LL1149:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1145:;} static
unsigned char _temp1153[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1153, _temp1153,
_temp1153 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1154= yy1; struct Cyc_List_List* _temp1160; _LL1156: if(*(( void**)
_temp1154) == Cyc_AttributeList_tok){ _LL1161: _temp1160=(( struct Cyc_AttributeList_tok_struct*)
_temp1154)->f1; goto _LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159;
_LL1157: return _temp1160; _LL1159:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1155:;} static unsigned char _temp1163[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1163,
_temp1163, _temp1163 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1164= yy1; void* _temp1170; _LL1166: if(*(( void**) _temp1164) == Cyc_Attribute_tok){
_LL1171: _temp1170=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1164)->f1;
goto _LL1167;} else{ goto _LL1168;} _LL1168: goto _LL1169; _LL1167: return
_temp1170; _LL1169:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1165:;}
static unsigned char _temp1173[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1173, _temp1173, _temp1173 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1174= yy1; struct Cyc_Absyn_Enumfield* _temp1180; _LL1176: if(*(( void**)
_temp1174) == Cyc_Enumfield_tok){ _LL1181: _temp1180=(( struct Cyc_Enumfield_tok_struct*)
_temp1174)->f1; goto _LL1177;} else{ goto _LL1178;} _LL1178: goto _LL1179;
_LL1177: return _temp1180; _LL1179:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1175:;} static unsigned char _temp1183[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1183, _temp1183, _temp1183 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1184= yy1; struct Cyc_List_List* _temp1190; _LL1186: if(*((
void**) _temp1184) == Cyc_EnumfieldList_tok){ _LL1191: _temp1190=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1184)->f1; goto _LL1187;} else{ goto _LL1188;} _LL1188: goto _LL1189;
_LL1187: return _temp1190; _LL1189:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1185:;} static unsigned char _temp1193[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1193, _temp1193, _temp1193 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1194= yy1;
struct Cyc_Core_Opt* _temp1200; _LL1196: if(*(( void**) _temp1194) == Cyc_TypeOpt_tok){
_LL1201: _temp1200=(( struct Cyc_TypeOpt_tok_struct*) _temp1194)->f1; goto
_LL1197;} else{ goto _LL1198;} _LL1198: goto _LL1199; _LL1197: return _temp1200;
_LL1199:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1195:;} static
unsigned char _temp1203[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1203, _temp1203, _temp1203 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1204=
yy1; struct Cyc_List_List* _temp1210; _LL1206: if(*(( void**) _temp1204) == Cyc_Rgnorder_tok){
_LL1211: _temp1210=(( struct Cyc_Rgnorder_tok_struct*) _temp1204)->f1; goto
_LL1207;} else{ goto _LL1208;} _LL1208: goto _LL1209; _LL1207: return _temp1210;
_LL1209:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1205:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1212;
_temp1212.timestamp= 0; _temp1212.first_line= 0; _temp1212.first_column= 0;
_temp1212.last_line= 0; _temp1212.last_column= 0; _temp1212;});} struct Cyc_Yyltype
Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[ 347u]={ 0, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 118, 2, 2, 100, 116, 112, 2, 96, 97, 107, 110, 98, 111, 104, 115, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 103, 92, 101, 95, 102, 109, 108, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 105, 2, 106, 114, 99, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 93, 113, 94,
117, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
86, 87, 88, 89, 90, 91}; static unsigned char _temp1213[ 2u]="$"; static
unsigned char _temp1214[ 6u]="error"; static unsigned char _temp1215[ 12u]="$undefined.";
static unsigned char _temp1216[ 5u]="AUTO"; static unsigned char _temp1217[ 9u]="REGISTER";
static unsigned char _temp1218[ 7u]="STATIC"; static unsigned char _temp1219[ 7u]="EXTERN";
static unsigned char _temp1220[ 8u]="TYPEDEF"; static unsigned char _temp1221[ 5u]="VOID";
static unsigned char _temp1222[ 5u]="CHAR"; static unsigned char _temp1223[ 6u]="SHORT";
static unsigned char _temp1224[ 4u]="INT"; static unsigned char _temp1225[ 5u]="LONG";
static unsigned char _temp1226[ 6u]="FLOAT"; static unsigned char _temp1227[ 7u]="DOUBLE";
static unsigned char _temp1228[ 7u]="SIGNED"; static unsigned char _temp1229[ 9u]="UNSIGNED";
static unsigned char _temp1230[ 6u]="CONST"; static unsigned char _temp1231[ 9u]="VOLATILE";
static unsigned char _temp1232[ 9u]="RESTRICT"; static unsigned char _temp1233[
7u]="STRUCT"; static unsigned char _temp1234[ 6u]="UNION"; static unsigned char
_temp1235[ 5u]="CASE"; static unsigned char _temp1236[ 8u]="DEFAULT"; static
unsigned char _temp1237[ 7u]="INLINE"; static unsigned char _temp1238[ 7u]="SIZEOF";
static unsigned char _temp1239[ 9u]="OFFSETOF"; static unsigned char _temp1240[
3u]="IF"; static unsigned char _temp1241[ 5u]="ELSE"; static unsigned char
_temp1242[ 7u]="SWITCH"; static unsigned char _temp1243[ 6u]="WHILE"; static
unsigned char _temp1244[ 3u]="DO"; static unsigned char _temp1245[ 4u]="FOR";
static unsigned char _temp1246[ 5u]="GOTO"; static unsigned char _temp1247[ 9u]="CONTINUE";
static unsigned char _temp1248[ 6u]="BREAK"; static unsigned char _temp1249[ 7u]="RETURN";
static unsigned char _temp1250[ 5u]="ENUM"; static unsigned char _temp1251[ 8u]="NULL_kw";
static unsigned char _temp1252[ 4u]="LET"; static unsigned char _temp1253[ 6u]="THROW";
static unsigned char _temp1254[ 4u]="TRY"; static unsigned char _temp1255[ 6u]="CATCH";
static unsigned char _temp1256[ 4u]="NEW"; static unsigned char _temp1257[ 9u]="ABSTRACT";
static unsigned char _temp1258[ 9u]="FALLTHRU"; static unsigned char _temp1259[
6u]="USING"; static unsigned char _temp1260[ 10u]="NAMESPACE"; static
unsigned char _temp1261[ 7u]="TUNION"; static unsigned char _temp1262[ 8u]="XTUNION";
static unsigned char _temp1263[ 5u]="FILL"; static unsigned char _temp1264[ 8u]="CODEGEN";
static unsigned char _temp1265[ 4u]="CUT"; static unsigned char _temp1266[ 7u]="SPLICE";
static unsigned char _temp1267[ 7u]="MALLOC"; static unsigned char _temp1268[ 9u]="REGION_T";
static unsigned char _temp1269[ 7u]="REGION"; static unsigned char _temp1270[ 5u]="RNEW";
static unsigned char _temp1271[ 8u]="RMALLOC"; static unsigned char _temp1272[ 7u]="PTR_OP";
static unsigned char _temp1273[ 7u]="INC_OP"; static unsigned char _temp1274[ 7u]="DEC_OP";
static unsigned char _temp1275[ 8u]="LEFT_OP"; static unsigned char _temp1276[ 9u]="RIGHT_OP";
static unsigned char _temp1277[ 6u]="LE_OP"; static unsigned char _temp1278[ 6u]="GE_OP";
static unsigned char _temp1279[ 6u]="EQ_OP"; static unsigned char _temp1280[ 6u]="NE_OP";
static unsigned char _temp1281[ 7u]="AND_OP"; static unsigned char _temp1282[ 6u]="OR_OP";
static unsigned char _temp1283[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1284[ 11u]="DIV_ASSIGN"; static unsigned char _temp1285[ 11u]="MOD_ASSIGN";
static unsigned char _temp1286[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1287[ 11u]="SUB_ASSIGN"; static unsigned char _temp1288[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1289[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1290[ 11u]="AND_ASSIGN"; static unsigned char _temp1291[ 11u]="XOR_ASSIGN";
static unsigned char _temp1292[ 10u]="OR_ASSIGN"; static unsigned char _temp1293[
9u]="ELLIPSIS"; static unsigned char _temp1294[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1295[ 12u]="COLON_COLON"; static unsigned char _temp1296[ 11u]="IDENTIFIER";
static unsigned char _temp1297[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1298[ 7u]="STRING"; static unsigned char _temp1299[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1300[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1301[ 9u]="TYPE_VAR"; static unsigned char _temp1302[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1303[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1304[ 10u]="ATTRIBUTE"; static unsigned char _temp1305[ 4u]="';'"; static
unsigned char _temp1306[ 4u]="'{'"; static unsigned char _temp1307[ 4u]="'}'";
static unsigned char _temp1308[ 4u]="'='"; static unsigned char _temp1309[ 4u]="'('";
static unsigned char _temp1310[ 4u]="')'"; static unsigned char _temp1311[ 4u]="','";
static unsigned char _temp1312[ 4u]="'_'"; static unsigned char _temp1313[ 4u]="'$'";
static unsigned char _temp1314[ 4u]="'<'"; static unsigned char _temp1315[ 4u]="'>'";
static unsigned char _temp1316[ 4u]="':'"; static unsigned char _temp1317[ 4u]="'.'";
static unsigned char _temp1318[ 4u]="'['"; static unsigned char _temp1319[ 4u]="']'";
static unsigned char _temp1320[ 4u]="'*'"; static unsigned char _temp1321[ 4u]="'@'";
static unsigned char _temp1322[ 4u]="'?'"; static unsigned char _temp1323[ 4u]="'+'";
static unsigned char _temp1324[ 4u]="'-'"; static unsigned char _temp1325[ 4u]="'&'";
static unsigned char _temp1326[ 4u]="'|'"; static unsigned char _temp1327[ 4u]="'^'";
static unsigned char _temp1328[ 4u]="'/'"; static unsigned char _temp1329[ 4u]="'%'";
static unsigned char _temp1330[ 4u]="'~'"; static unsigned char _temp1331[ 4u]="'!'";
static unsigned char _temp1332[ 5u]="prog"; static unsigned char _temp1333[ 17u]="translation_unit";
static unsigned char _temp1334[ 21u]="external_declaration"; static
unsigned char _temp1335[ 20u]="function_definition"; static unsigned char
_temp1336[ 21u]="function_definition2"; static unsigned char _temp1337[ 13u]="using_action";
static unsigned char _temp1338[ 15u]="unusing_action"; static unsigned char
_temp1339[ 17u]="namespace_action"; static unsigned char _temp1340[ 19u]="unnamespace_action";
static unsigned char _temp1341[ 12u]="declaration"; static unsigned char
_temp1342[ 17u]="declaration_list"; static unsigned char _temp1343[ 23u]="declaration_specifiers";
static unsigned char _temp1344[ 24u]="storage_class_specifier"; static
unsigned char _temp1345[ 15u]="attributes_opt"; static unsigned char _temp1346[
11u]="attributes"; static unsigned char _temp1347[ 15u]="attribute_list"; static
unsigned char _temp1348[ 10u]="attribute"; static unsigned char _temp1349[ 15u]="type_specifier";
static unsigned char _temp1350[ 5u]="kind"; static unsigned char _temp1351[ 15u]="type_qualifier";
static unsigned char _temp1352[ 15u]="enum_specifier"; static unsigned char
_temp1353[ 11u]="enum_field"; static unsigned char _temp1354[ 22u]="enum_declaration_list";
static unsigned char _temp1355[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1356[ 16u]="type_params_opt"; static unsigned char _temp1357[
16u]="struct_or_union"; static unsigned char _temp1358[ 24u]="struct_declaration_list";
static unsigned char _temp1359[ 25u]="struct_declaration_list0"; static
unsigned char _temp1360[ 21u]="init_declarator_list"; static unsigned char
_temp1361[ 22u]="init_declarator_list0"; static unsigned char _temp1362[ 16u]="init_declarator";
static unsigned char _temp1363[ 19u]="struct_declaration"; static unsigned char
_temp1364[ 25u]="specifier_qualifier_list"; static unsigned char _temp1365[ 23u]="struct_declarator_list";
static unsigned char _temp1366[ 24u]="struct_declarator_list0"; static
unsigned char _temp1367[ 18u]="struct_declarator"; static unsigned char
_temp1368[ 17u]="tunion_specifier"; static unsigned char _temp1369[ 18u]="tunion_or_xtunion";
static unsigned char _temp1370[ 17u]="tunionfield_list"; static unsigned char
_temp1371[ 18u]="tunionfield_scope"; static unsigned char _temp1372[ 12u]="tunionfield";
static unsigned char _temp1373[ 11u]="declarator"; static unsigned char
_temp1374[ 18u]="direct_declarator"; static unsigned char _temp1375[ 8u]="pointer";
static unsigned char _temp1376[ 13u]="pointer_char"; static unsigned char
_temp1377[ 8u]="rgn_opt"; static unsigned char _temp1378[ 4u]="rgn"; static
unsigned char _temp1379[ 20u]="type_qualifier_list"; static unsigned char
_temp1380[ 20u]="parameter_type_list"; static unsigned char _temp1381[ 16u]="optional_effect";
static unsigned char _temp1382[ 19u]="optional_rgn_order"; static unsigned char
_temp1383[ 10u]="rgn_order"; static unsigned char _temp1384[ 16u]="optional_inject";
static unsigned char _temp1385[ 11u]="effect_set"; static unsigned char
_temp1386[ 14u]="atomic_effect"; static unsigned char _temp1387[ 11u]="region_set";
static unsigned char _temp1388[ 15u]="parameter_list"; static unsigned char
_temp1389[ 22u]="parameter_declaration"; static unsigned char _temp1390[ 16u]="identifier_list";
static unsigned char _temp1391[ 17u]="identifier_list0"; static unsigned char
_temp1392[ 12u]="initializer"; static unsigned char _temp1393[ 18u]="array_initializer";
static unsigned char _temp1394[ 17u]="initializer_list"; static unsigned char
_temp1395[ 12u]="designation"; static unsigned char _temp1396[ 16u]="designator_list";
static unsigned char _temp1397[ 11u]="designator"; static unsigned char
_temp1398[ 10u]="type_name"; static unsigned char _temp1399[ 14u]="any_type_name";
static unsigned char _temp1400[ 15u]="type_name_list"; static unsigned char
_temp1401[ 20u]="abstract_declarator"; static unsigned char _temp1402[ 27u]="direct_abstract_declarator";
static unsigned char _temp1403[ 10u]="statement"; static unsigned char _temp1404[
18u]="labeled_statement"; static unsigned char _temp1405[ 21u]="expression_statement";
static unsigned char _temp1406[ 19u]="compound_statement"; static unsigned char
_temp1407[ 16u]="block_item_list"; static unsigned char _temp1408[ 11u]="block_item";
static unsigned char _temp1409[ 20u]="selection_statement"; static unsigned char
_temp1410[ 15u]="switch_clauses"; static unsigned char _temp1411[ 16u]="switchC_clauses";
static unsigned char _temp1412[ 20u]="iteration_statement"; static unsigned char
_temp1413[ 15u]="jump_statement"; static unsigned char _temp1414[ 8u]="pattern";
static unsigned char _temp1415[ 19u]="tuple_pattern_list"; static unsigned char
_temp1416[ 20u]="tuple_pattern_list0"; static unsigned char _temp1417[ 14u]="field_pattern";
static unsigned char _temp1418[ 19u]="field_pattern_list"; static unsigned char
_temp1419[ 20u]="field_pattern_list0"; static unsigned char _temp1420[ 11u]="expression";
static unsigned char _temp1421[ 22u]="assignment_expression"; static
unsigned char _temp1422[ 20u]="assignment_operator"; static unsigned char
_temp1423[ 23u]="conditional_expression"; static unsigned char _temp1424[ 20u]="constant_expression";
static unsigned char _temp1425[ 22u]="logical_or_expression"; static
unsigned char _temp1426[ 23u]="logical_and_expression"; static unsigned char
_temp1427[ 24u]="inclusive_or_expression"; static unsigned char _temp1428[ 24u]="exclusive_or_expression";
static unsigned char _temp1429[ 15u]="and_expression"; static unsigned char
_temp1430[ 20u]="equality_expression"; static unsigned char _temp1431[ 22u]="relational_expression";
static unsigned char _temp1432[ 17u]="shift_expression"; static unsigned char
_temp1433[ 20u]="additive_expression"; static unsigned char _temp1434[ 26u]="multiplicative_expression";
static unsigned char _temp1435[ 16u]="cast_expression"; static unsigned char
_temp1436[ 17u]="unary_expression"; static unsigned char _temp1437[ 15u]="unary_operator";
static unsigned char _temp1438[ 19u]="postfix_expression"; static unsigned char
_temp1439[ 19u]="primary_expression"; static unsigned char _temp1440[ 25u]="argument_expression_list";
static unsigned char _temp1441[ 26u]="argument_expression_list0"; static
unsigned char _temp1442[ 9u]="constant"; static unsigned char _temp1443[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 231u]={{ _temp1213, _temp1213, _temp1213
+ 2u},{ _temp1214, _temp1214, _temp1214 + 6u},{ _temp1215, _temp1215, _temp1215
+ 12u},{ _temp1216, _temp1216, _temp1216 + 5u},{ _temp1217, _temp1217, _temp1217
+ 9u},{ _temp1218, _temp1218, _temp1218 + 7u},{ _temp1219, _temp1219, _temp1219
+ 7u},{ _temp1220, _temp1220, _temp1220 + 8u},{ _temp1221, _temp1221, _temp1221
+ 5u},{ _temp1222, _temp1222, _temp1222 + 5u},{ _temp1223, _temp1223, _temp1223
+ 6u},{ _temp1224, _temp1224, _temp1224 + 4u},{ _temp1225, _temp1225, _temp1225
+ 5u},{ _temp1226, _temp1226, _temp1226 + 6u},{ _temp1227, _temp1227, _temp1227
+ 7u},{ _temp1228, _temp1228, _temp1228 + 7u},{ _temp1229, _temp1229, _temp1229
+ 9u},{ _temp1230, _temp1230, _temp1230 + 6u},{ _temp1231, _temp1231, _temp1231
+ 9u},{ _temp1232, _temp1232, _temp1232 + 9u},{ _temp1233, _temp1233, _temp1233
+ 7u},{ _temp1234, _temp1234, _temp1234 + 6u},{ _temp1235, _temp1235, _temp1235
+ 5u},{ _temp1236, _temp1236, _temp1236 + 8u},{ _temp1237, _temp1237, _temp1237
+ 7u},{ _temp1238, _temp1238, _temp1238 + 7u},{ _temp1239, _temp1239, _temp1239
+ 9u},{ _temp1240, _temp1240, _temp1240 + 3u},{ _temp1241, _temp1241, _temp1241
+ 5u},{ _temp1242, _temp1242, _temp1242 + 7u},{ _temp1243, _temp1243, _temp1243
+ 6u},{ _temp1244, _temp1244, _temp1244 + 3u},{ _temp1245, _temp1245, _temp1245
+ 4u},{ _temp1246, _temp1246, _temp1246 + 5u},{ _temp1247, _temp1247, _temp1247
+ 9u},{ _temp1248, _temp1248, _temp1248 + 6u},{ _temp1249, _temp1249, _temp1249
+ 7u},{ _temp1250, _temp1250, _temp1250 + 5u},{ _temp1251, _temp1251, _temp1251
+ 8u},{ _temp1252, _temp1252, _temp1252 + 4u},{ _temp1253, _temp1253, _temp1253
+ 6u},{ _temp1254, _temp1254, _temp1254 + 4u},{ _temp1255, _temp1255, _temp1255
+ 6u},{ _temp1256, _temp1256, _temp1256 + 4u},{ _temp1257, _temp1257, _temp1257
+ 9u},{ _temp1258, _temp1258, _temp1258 + 9u},{ _temp1259, _temp1259, _temp1259
+ 6u},{ _temp1260, _temp1260, _temp1260 + 10u},{ _temp1261, _temp1261, _temp1261
+ 7u},{ _temp1262, _temp1262, _temp1262 + 8u},{ _temp1263, _temp1263, _temp1263
+ 5u},{ _temp1264, _temp1264, _temp1264 + 8u},{ _temp1265, _temp1265, _temp1265
+ 4u},{ _temp1266, _temp1266, _temp1266 + 7u},{ _temp1267, _temp1267, _temp1267
+ 7u},{ _temp1268, _temp1268, _temp1268 + 9u},{ _temp1269, _temp1269, _temp1269
+ 7u},{ _temp1270, _temp1270, _temp1270 + 5u},{ _temp1271, _temp1271, _temp1271
+ 8u},{ _temp1272, _temp1272, _temp1272 + 7u},{ _temp1273, _temp1273, _temp1273
+ 7u},{ _temp1274, _temp1274, _temp1274 + 7u},{ _temp1275, _temp1275, _temp1275
+ 8u},{ _temp1276, _temp1276, _temp1276 + 9u},{ _temp1277, _temp1277, _temp1277
+ 6u},{ _temp1278, _temp1278, _temp1278 + 6u},{ _temp1279, _temp1279, _temp1279
+ 6u},{ _temp1280, _temp1280, _temp1280 + 6u},{ _temp1281, _temp1281, _temp1281
+ 7u},{ _temp1282, _temp1282, _temp1282 + 6u},{ _temp1283, _temp1283, _temp1283
+ 11u},{ _temp1284, _temp1284, _temp1284 + 11u},{ _temp1285, _temp1285,
_temp1285 + 11u},{ _temp1286, _temp1286, _temp1286 + 11u},{ _temp1287, _temp1287,
_temp1287 + 11u},{ _temp1288, _temp1288, _temp1288 + 12u},{ _temp1289, _temp1289,
_temp1289 + 13u},{ _temp1290, _temp1290, _temp1290 + 11u},{ _temp1291, _temp1291,
_temp1291 + 11u},{ _temp1292, _temp1292, _temp1292 + 10u},{ _temp1293, _temp1293,
_temp1293 + 9u},{ _temp1294, _temp1294, _temp1294 + 11u},{ _temp1295, _temp1295,
_temp1295 + 12u},{ _temp1296, _temp1296, _temp1296 + 11u},{ _temp1297, _temp1297,
_temp1297 + 17u},{ _temp1298, _temp1298, _temp1298 + 7u},{ _temp1299, _temp1299,
_temp1299 + 19u},{ _temp1300, _temp1300, _temp1300 + 18u},{ _temp1301, _temp1301,
_temp1301 + 9u},{ _temp1302, _temp1302, _temp1302 + 16u},{ _temp1303, _temp1303,
_temp1303 + 18u},{ _temp1304, _temp1304, _temp1304 + 10u},{ _temp1305, _temp1305,
_temp1305 + 4u},{ _temp1306, _temp1306, _temp1306 + 4u},{ _temp1307, _temp1307,
_temp1307 + 4u},{ _temp1308, _temp1308, _temp1308 + 4u},{ _temp1309, _temp1309,
_temp1309 + 4u},{ _temp1310, _temp1310, _temp1310 + 4u},{ _temp1311, _temp1311,
_temp1311 + 4u},{ _temp1312, _temp1312, _temp1312 + 4u},{ _temp1313, _temp1313,
_temp1313 + 4u},{ _temp1314, _temp1314, _temp1314 + 4u},{ _temp1315, _temp1315,
_temp1315 + 4u},{ _temp1316, _temp1316, _temp1316 + 4u},{ _temp1317, _temp1317,
_temp1317 + 4u},{ _temp1318, _temp1318, _temp1318 + 4u},{ _temp1319, _temp1319,
_temp1319 + 4u},{ _temp1320, _temp1320, _temp1320 + 4u},{ _temp1321, _temp1321,
_temp1321 + 4u},{ _temp1322, _temp1322, _temp1322 + 4u},{ _temp1323, _temp1323,
_temp1323 + 4u},{ _temp1324, _temp1324, _temp1324 + 4u},{ _temp1325, _temp1325,
_temp1325 + 4u},{ _temp1326, _temp1326, _temp1326 + 4u},{ _temp1327, _temp1327,
_temp1327 + 4u},{ _temp1328, _temp1328, _temp1328 + 4u},{ _temp1329, _temp1329,
_temp1329 + 4u},{ _temp1330, _temp1330, _temp1330 + 4u},{ _temp1331, _temp1331,
_temp1331 + 4u},{ _temp1332, _temp1332, _temp1332 + 5u},{ _temp1333, _temp1333,
_temp1333 + 17u},{ _temp1334, _temp1334, _temp1334 + 21u},{ _temp1335, _temp1335,
_temp1335 + 20u},{ _temp1336, _temp1336, _temp1336 + 21u},{ _temp1337, _temp1337,
_temp1337 + 13u},{ _temp1338, _temp1338, _temp1338 + 15u},{ _temp1339, _temp1339,
_temp1339 + 17u},{ _temp1340, _temp1340, _temp1340 + 19u},{ _temp1341, _temp1341,
_temp1341 + 12u},{ _temp1342, _temp1342, _temp1342 + 17u},{ _temp1343, _temp1343,
_temp1343 + 23u},{ _temp1344, _temp1344, _temp1344 + 24u},{ _temp1345, _temp1345,
_temp1345 + 15u},{ _temp1346, _temp1346, _temp1346 + 11u},{ _temp1347, _temp1347,
_temp1347 + 15u},{ _temp1348, _temp1348, _temp1348 + 10u},{ _temp1349, _temp1349,
_temp1349 + 15u},{ _temp1350, _temp1350, _temp1350 + 5u},{ _temp1351, _temp1351,
_temp1351 + 15u},{ _temp1352, _temp1352, _temp1352 + 15u},{ _temp1353, _temp1353,
_temp1353 + 11u},{ _temp1354, _temp1354, _temp1354 + 22u},{ _temp1355, _temp1355,
_temp1355 + 26u},{ _temp1356, _temp1356, _temp1356 + 16u},{ _temp1357, _temp1357,
_temp1357 + 16u},{ _temp1358, _temp1358, _temp1358 + 24u},{ _temp1359, _temp1359,
_temp1359 + 25u},{ _temp1360, _temp1360, _temp1360 + 21u},{ _temp1361, _temp1361,
_temp1361 + 22u},{ _temp1362, _temp1362, _temp1362 + 16u},{ _temp1363, _temp1363,
_temp1363 + 19u},{ _temp1364, _temp1364, _temp1364 + 25u},{ _temp1365, _temp1365,
_temp1365 + 23u},{ _temp1366, _temp1366, _temp1366 + 24u},{ _temp1367, _temp1367,
_temp1367 + 18u},{ _temp1368, _temp1368, _temp1368 + 17u},{ _temp1369, _temp1369,
_temp1369 + 18u},{ _temp1370, _temp1370, _temp1370 + 17u},{ _temp1371, _temp1371,
_temp1371 + 18u},{ _temp1372, _temp1372, _temp1372 + 12u},{ _temp1373, _temp1373,
_temp1373 + 11u},{ _temp1374, _temp1374, _temp1374 + 18u},{ _temp1375, _temp1375,
_temp1375 + 8u},{ _temp1376, _temp1376, _temp1376 + 13u},{ _temp1377, _temp1377,
_temp1377 + 8u},{ _temp1378, _temp1378, _temp1378 + 4u},{ _temp1379, _temp1379,
_temp1379 + 20u},{ _temp1380, _temp1380, _temp1380 + 20u},{ _temp1381, _temp1381,
_temp1381 + 16u},{ _temp1382, _temp1382, _temp1382 + 19u},{ _temp1383, _temp1383,
_temp1383 + 10u},{ _temp1384, _temp1384, _temp1384 + 16u},{ _temp1385, _temp1385,
_temp1385 + 11u},{ _temp1386, _temp1386, _temp1386 + 14u},{ _temp1387, _temp1387,
_temp1387 + 11u},{ _temp1388, _temp1388, _temp1388 + 15u},{ _temp1389, _temp1389,
_temp1389 + 22u},{ _temp1390, _temp1390, _temp1390 + 16u},{ _temp1391, _temp1391,
_temp1391 + 17u},{ _temp1392, _temp1392, _temp1392 + 12u},{ _temp1393, _temp1393,
_temp1393 + 18u},{ _temp1394, _temp1394, _temp1394 + 17u},{ _temp1395, _temp1395,
_temp1395 + 12u},{ _temp1396, _temp1396, _temp1396 + 16u},{ _temp1397, _temp1397,
_temp1397 + 11u},{ _temp1398, _temp1398, _temp1398 + 10u},{ _temp1399, _temp1399,
_temp1399 + 14u},{ _temp1400, _temp1400, _temp1400 + 15u},{ _temp1401, _temp1401,
_temp1401 + 20u},{ _temp1402, _temp1402, _temp1402 + 27u},{ _temp1403, _temp1403,
_temp1403 + 10u},{ _temp1404, _temp1404, _temp1404 + 18u},{ _temp1405, _temp1405,
_temp1405 + 21u},{ _temp1406, _temp1406, _temp1406 + 19u},{ _temp1407, _temp1407,
_temp1407 + 16u},{ _temp1408, _temp1408, _temp1408 + 11u},{ _temp1409, _temp1409,
_temp1409 + 20u},{ _temp1410, _temp1410, _temp1410 + 15u},{ _temp1411, _temp1411,
_temp1411 + 16u},{ _temp1412, _temp1412, _temp1412 + 20u},{ _temp1413, _temp1413,
_temp1413 + 15u},{ _temp1414, _temp1414, _temp1414 + 8u},{ _temp1415, _temp1415,
_temp1415 + 19u},{ _temp1416, _temp1416, _temp1416 + 20u},{ _temp1417, _temp1417,
_temp1417 + 14u},{ _temp1418, _temp1418, _temp1418 + 19u},{ _temp1419, _temp1419,
_temp1419 + 20u},{ _temp1420, _temp1420, _temp1420 + 11u},{ _temp1421, _temp1421,
_temp1421 + 22u},{ _temp1422, _temp1422, _temp1422 + 20u},{ _temp1423, _temp1423,
_temp1423 + 23u},{ _temp1424, _temp1424, _temp1424 + 20u},{ _temp1425, _temp1425,
_temp1425 + 22u},{ _temp1426, _temp1426, _temp1426 + 23u},{ _temp1427, _temp1427,
_temp1427 + 24u},{ _temp1428, _temp1428, _temp1428 + 24u},{ _temp1429, _temp1429,
_temp1429 + 15u},{ _temp1430, _temp1430, _temp1430 + 20u},{ _temp1431, _temp1431,
_temp1431 + 22u},{ _temp1432, _temp1432, _temp1432 + 17u},{ _temp1433, _temp1433,
_temp1433 + 20u},{ _temp1434, _temp1434, _temp1434 + 26u},{ _temp1435, _temp1435,
_temp1435 + 16u},{ _temp1436, _temp1436, _temp1436 + 17u},{ _temp1437, _temp1437,
_temp1437 + 15u},{ _temp1438, _temp1438, _temp1438 + 19u},{ _temp1439, _temp1439,
_temp1439 + 19u},{ _temp1440, _temp1440, _temp1440 + 25u},{ _temp1441, _temp1441,
_temp1441 + 26u},{ _temp1442, _temp1442, _temp1442 + 9u},{ _temp1443, _temp1443,
_temp1443 + 20u}}; static short Cyc_yyr1[ 403u]={ 0, 119, 120, 120, 120, 120,
120, 120, 120, 121, 121, 122, 122, 122, 122, 123, 123, 124, 125, 126, 127, 128,
128, 128, 128, 129, 129, 130, 130, 130, 130, 130, 130, 130, 130, 131, 131, 131,
131, 131, 131, 131, 132, 132, 133, 134, 134, 135, 135, 135, 135, 135, 136, 136,
136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136,
137, 137, 138, 138, 138, 139, 139, 140, 140, 141, 141, 142, 142, 142, 142, 142,
143, 143, 143, 144, 144, 145, 146, 146, 147, 148, 148, 149, 149, 150, 151, 151,
151, 151, 152, 153, 153, 154, 154, 154, 155, 155, 155, 155, 156, 156, 157, 157,
157, 157, 158, 158, 158, 159, 159, 160, 160, 161, 161, 161, 161, 161, 161, 161,
161, 161, 161, 161, 161, 162, 162, 162, 162, 163, 163, 163, 163, 163, 164, 164,
165, 165, 165, 166, 166, 167, 167, 167, 167, 168, 168, 169, 169, 170, 170, 171,
171, 172, 172, 173, 173, 173, 173, 174, 174, 174, 174, 175, 175, 176, 176, 176,
177, 178, 178, 179, 179, 180, 180, 180, 180, 181, 181, 181, 181, 182, 183, 183,
184, 184, 185, 185, 186, 186, 186, 186, 187, 187, 188, 188, 188, 189, 189, 189,
189, 189, 189, 189, 189, 189, 189, 189, 189, 190, 190, 190, 190, 190, 190, 190,
190, 190, 190, 191, 192, 192, 193, 193, 194, 194, 195, 195, 195, 196, 196, 196,
196, 196, 197, 197, 197, 197, 197, 197, 198, 198, 198, 198, 199, 199, 199, 199,
199, 199, 199, 199, 199, 199, 199, 199, 199, 199, 200, 200, 200, 200, 200, 200,
200, 200, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201,
202, 202, 203, 203, 204, 204, 205, 206, 206, 207, 207, 208, 208, 209, 209, 209,
209, 209, 209, 209, 209, 209, 209, 209, 210, 210, 210, 210, 210, 210, 210, 211,
212, 212, 213, 213, 214, 214, 215, 215, 216, 216, 217, 217, 217, 218, 218, 218,
218, 218, 219, 219, 219, 220, 220, 220, 221, 221, 221, 221, 222, 222, 223, 223,
223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 224, 224, 224, 225, 225, 225,
225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 226, 226, 226, 226, 226,
226, 226, 226, 226, 227, 228, 228, 229, 229, 229, 229, 230, 230}; static short
Cyc_yyr2[ 403u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1,
2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1,
1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 4, 4, 1, 1, 1, 1, 1,
5, 2, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3,
2, 3, 1, 1, 3, 1, 2, 3, 6, 3, 4, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3,
3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 3, 1, 1, 2, 3,
5, 6, 8, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 1, 3, 1, 3, 3, 5, 1, 3, 2, 1, 2, 1,
1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 3, 1, 3, 1, 1,
2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2,
3, 1, 2, 1, 1, 1, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8,
7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4,
5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1,
3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6, 7, 9, 1, 1, 1, 1,
4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1,
1, 1, 1, 1}; static short Cyc_yydefact[ 794u]={ 0, 138, 35, 36, 37, 38, 40, 52,
54, 55, 56, 57, 58, 59, 60, 61, 72, 73, 74, 89, 90, 42, 0, 0, 41, 0, 0, 114, 115,
0, 401, 65, 402, 86, 0, 53, 0, 143, 144, 147, 1, 0, 9, 0, 0, 10, 0, 42, 42, 42,
62, 63, 0, 64, 0, 0, 125, 0, 148, 127, 39, 0, 33, 43, 76, 286, 401, 282, 285,
284, 0, 280, 0, 0, 0, 0, 0, 182, 0, 287, 17, 19, 0, 0, 0, 67, 0, 0, 0, 0, 2, 0,
0, 0, 0, 21, 0, 94, 95, 97, 27, 29, 31, 86, 0, 86, 150, 152, 0, 86, 38, 0, 25, 0,
0, 11, 159, 0, 0, 136, 126, 42, 149, 137, 0, 0, 34, 0, 0, 294, 293, 283, 292, 24,
0, 0, 0, 0, 42, 42, 200, 202, 0, 70, 71, 66, 206, 0, 128, 0, 0, 177, 0, 0, 400,
0, 0, 0, 0, 0, 0, 0, 0, 0, 397, 387, 398, 399, 0, 0, 0, 0, 370, 0, 368, 369, 0,
305, 318, 326, 328, 330, 332, 334, 336, 339, 344, 347, 350, 354, 0, 356, 371,
386, 385, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 85, 0, 91, 92, 0, 84, 0,
86, 0, 111, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 401, 234, 236, 0, 242,
240, 0, 241, 223, 224, 225, 0, 238, 226, 227, 228, 0, 303, 26, 13, 97, 0, 183, 0,
0, 161, 159, 0, 0, 129, 0, 139, 0, 0, 79, 0, 77, 281, 296, 0, 295, 184, 0, 0,
294, 173, 203, 0, 100, 102, 159, 0, 208, 201, 209, 69, 0, 88, 0, 87, 0, 179, 0,
181, 68, 0, 0, 364, 0, 320, 354, 0, 321, 322, 0, 0, 0, 0, 0, 0, 357, 358, 0, 0,
0, 0, 360, 361, 359, 145, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 307, 0, 362, 0, 379, 380, 0,
0, 0, 389, 0, 0, 146, 18, 0, 20, 0, 96, 98, 186, 185, 14, 0, 81, 93, 0, 0, 104,
105, 107, 0, 151, 112, 86, 120, 0, 0, 0, 0, 0, 0, 0, 273, 274, 275, 0, 0, 277, 0,
231, 232, 0, 0, 0, 0, 97, 237, 239, 235, 0, 165, 171, 0, 160, 167, 131, 0, 0, 0,
161, 133, 135, 134, 130, 153, 141, 140, 0, 48, 47, 0, 45, 0, 75, 0, 289, 0, 23,
290, 0, 0, 0, 0, 196, 298, 301, 0, 300, 0, 0, 0, 204, 101, 103, 0, 161, 0, 212,
0, 210, 159, 0, 0, 222, 205, 207, 178, 0, 0, 0, 187, 191, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 388, 395, 0, 394, 327, 0, 329, 331, 333, 335, 337, 338, 342, 343, 340,
341, 345, 346, 348, 349, 351, 352, 353, 306, 377, 378, 373, 0, 375, 376, 0, 0, 0,
4, 6, 0, 325, 108, 99, 0, 0, 0, 113, 122, 121, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0, 0,
272, 276, 0, 0, 0, 230, 0, 233, 0, 15, 304, 166, 0, 0, 169, 0, 0, 0, 162, 132,
159, 155, 154, 142, 7, 0, 0, 0, 80, 78, 297, 199, 0, 299, 195, 197, 291, 0, 288,
175, 174, 217, 0, 211, 214, 0, 161, 0, 213, 0, 363, 0, 0, 188, 0, 192, 383, 384,
0, 0, 0, 0, 0, 0, 0, 355, 391, 0, 0, 374, 372, 392, 0, 0, 83, 106, 109, 82, 110,
123, 120, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 278, 0, 0, 16, 159, 172, 170,
168, 0, 165, 161, 0, 0, 0, 44, 46, 198, 302, 0, 216, 219, 0, 221, 220, 215, 0, 0,
189, 193, 0, 0, 323, 324, 0, 393, 0, 396, 319, 390, 0, 119, 118, 243, 0, 248,
258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 279, 0, 161, 163, 0, 156, 0, 49, 50,
176, 218, 365, 0, 194, 0, 0, 381, 0, 0, 0, 254, 0, 0, 260, 0, 0, 0, 268, 0, 0, 0,
0, 0, 0, 0, 0, 247, 229, 157, 0, 159, 0, 0, 366, 0, 382, 0, 244, 0, 0, 0, 245,
259, 261, 262, 0, 270, 269, 0, 264, 0, 0, 0, 0, 248, 249, 164, 161, 0, 0, 0, 124,
0, 0, 246, 263, 271, 265, 266, 0, 0, 248, 250, 158, 0, 190, 367, 254, 255, 267,
248, 251, 51, 254, 256, 248, 252, 257, 253, 0, 0, 0}; static short Cyc_yydefgoto[
112u]={ 791, 40, 41, 42, 237, 43, 366, 44, 368, 45, 203, 46, 47, 62, 63, 432,
433, 48, 145, 49, 50, 267, 268, 51, 85, 52, 209, 210, 96, 97, 98, 211, 149, 378,
379, 380, 53, 54, 530, 531, 532, 55, 56, 57, 58, 121, 108, 428, 456, 457, 419,
559, 553, 415, 416, 280, 259, 151, 76, 77, 473, 371, 474, 475, 444, 445, 141,
146, 147, 458, 287, 240, 241, 242, 243, 244, 245, 246, 691, 742, 247, 248, 271,
272, 273, 447, 448, 449, 249, 250, 353, 177, 522, 178, 179, 180, 181, 182, 183,
184, 185, 186, 187, 188, 189, 190, 191, 192, 487, 488, 193, 194}; static short
Cyc_yypact[ 794u]={ 1886, - -32768, - -32768, - -32768, - -32768, 0, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, 23, - 24, 649, -
-32768, - 24, 89, - -32768, - -32768, 84, - -32768, 111, - -32768, 94, 434, -
-32768, 147, 207, 230, - -32768, - -32768, 1776, - -32768, 64, 334, - -32768,
401, 23, 23, 23, - -32768, - -32768, 185, - -32768, 199, 2419, 95, 51, 20, - 24,
232, 154, 2473, - -32768, 243, - -32768, 241, - -32768, - -32768, - -32768, 703,
- -32768, 242, 262, 269, 703, 275, 314, 290, 165, - -32768, - -32768, 1711, 125,
1711, - -32768, 295, 2044, 3933, 3933, - -32768, 1776, 1995, 1776, 1995, -
-32768, 349, 356, - -32768, 2362, 2473, 2473, 2473, 94, 2044, 94, 374, - -32768,
- 24, 299, 383, 1203, - -32768, 2419, 401, - -32768, 2304, 1711, 2893, - -32768,
95, 23, - -32768, - -32768, 1995, 376, - -32768, - 24, 386, 703, - -32768, -
-32768, - -32768, - -32768, 405, 3933, 187, 246, 23, 23, 288, - -32768, 43, -
-32768, - -32768, - -32768, 375, 52, - -32768, 549, 351, - -32768, 4080, 396, -
-32768, 3933, 3974, 400, 409, 420, 428, 432, 4135, 4135, - -32768, - -32768, -
-32768, - -32768, 1546, 440, 4176, 4176, - -32768, 4176, - -32768, - -32768, 460,
- -32768, - 2, 470, 450, 453, 474, 392, 138, 403, 366, 126, - -32768, 756, 4176,
283, - -32768, - -32768, 5, 478, - -32768, 482, - -32768, 488, - -32768, 434,
2958, 2419, - -32768, - -32768, - -32768, - -32768, 498, 501, 2044, - -32768,
494, 505, 125, 94, - 24, 518, - -32768, 520, 45, 529, 2524, 531, 548, 543, 544,
3023, 2524, 47, 2524, 2524, 26, 534, - -32768, - -32768, 546, - -32768, - -32768,
401, - -32768, - -32768, - -32768, - -32768, 545, 1319, - -32768, - -32768, -
-32768, 257, - -32768, - -32768, - -32768, 553, 20, - -32768, 191, 547, 537, 276,
554, 72, - -32768, 555, 164, 556, 22, 557, 559, 564, - -32768, - -32768, 565,
566, - -32768, 312, 595, 703, 13, - -32768, 571, 2044, 2044, 2255, 3088, - 8, -
-32768, 108, - -32768, 191, - -32768, 1711, - -32768, 2153, - -32768, 109, -
-32768, - -32768, 2044, 1546, - -32768, 2044, - -32768, - -32768, 2589, - -32768,
574, 3933, 2104, 641, 3933, 3933, 1546, - -32768, - -32768, 1319, 570, 402, 3933,
- -32768, - -32768, - -32768, - -32768, 4176, 3933, 4176, 4176, 4176, 4176, 4176,
4176, 4176, 4176, 4176, 4176, 4176, 4176, 4176, 4176, 4176, 4176, 4176, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, 3933, - -32768, 140, - -32768, - -32768, 3153, 141, 3933, -
-32768, 2817, 568, - -32768, - -32768, 1776, - -32768, 1776, - -32768, - -32768,
- -32768, - -32768, - -32768, 2044, - -32768, - -32768, 3933, 578, 573, - -32768,
572, 2044, - -32768, - -32768, 94, 507, 3933, 577, 3933, 3933, 644, 1435, 585, -
-32768, - -32768, - -32768, 313, 638, - -32768, 3218, - -32768, - -32768, 2524,
597, 2524, 1657, 2362, - -32768, - -32768, - -32768, 3933, 600, 604, 319, -
-32768, 580, - -32768, 605, 591, 4301, 537, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, 164, 1776, - -32768, 596, 599, 606, - 24, - -32768,
3933, - -32768, 703, - -32768, - -32768, 614, 3933, 703, 294, - -32768, - -32768,
- -32768, 609, 611, 615, 125, 623, - -32768, - -32768, - -32768, 619, 537, 624,
- -32768, 617, 108, 4287, 1711, 3283, - -32768, - -32768, 375, - -32768, 628,
622, 643, - -32768, - -32768, 75, 2958, 422, 632, 434, 634, 424, 636, 640, 645,
4039, - -32768, - -32768, 646, 642, 470, 218, 450, 453, 474, 392, 138, 138, 403,
403, 403, 403, 366, 366, 126, 126, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, 650, - -32768, - -32768, 69, 198, 1711, - -32768, -
-32768, 648, - -32768, - -32768, - -32768, 494, 3933, 652, - -32768, - -32768, -
-32768, 656, - 24, 316, 429, 3933, 448, 454, 655, 3348, 3413, 331, - -32768, -
-32768, 659, 661, 662, - -32768, 663, - -32768, 2419, - -32768, - -32768, -
-32768, 2044, 125, - -32768, 669, 191, 665, - -32768, - -32768, 157, - -32768, -
-32768, - -32768, - -32768, 378, 667, 22, - -32768, - -32768, - -32768, - -32768,
682, - -32768, - -32768, - -32768, - -32768, 825, - -32768, 670, - -32768, -
-32768, 694, - -32768, - -32768, 696, 537, 86, - -32768, 688, 702, 713, 697, -
-32768, 2665, - -32768, - -32768, - -32768, 2419, 2044, 3974, 772, 702, 704,
2817, - -32768, - -32768, 3933, 3933, - -32768, - -32768, - -32768, 2817, 161, -
-32768, - -32768, - -32768, - -32768, - -32768, 268, 24, 507, 2524, 461, 707,
2524, 3933, 3478, 333, 3543, 336, 3608, 509, - -32768, 712, 722, - -32768, 716,
- -32768, - -32768, - -32768, 729, 600, 537, 720, 723, 725, - -32768, - -32768,
- -32768, - -32768, 623, - -32768, - -32768, 726, - -32768, - -32768, - -32768,
727, 3933, - -32768, - -32768, 2958, 739, - -32768, 574, 741, - -32768, 214, -
-32768, - -32768, - -32768, 742, - -32768, - -32768, 791, 747, 509, - -32768,
464, 2524, 477, 3673, 2524, 481, 3738, 3803, 338, 703, 738, 748, - -32768, 2524,
537, 746, 2044, - -32768, 761, - -32768, - -32768, - -32768, - -32768, - -32768,
273, - -32768, 749, 2044, - -32768, 2741, 2044, 2524, 558, 753, 758, - -32768,
2524, 2524, 487, - -32768, 2524, 2524, 491, 2524, 496, 3868, 19, 1319, - -32768,
- -32768, - -32768, 605, 716, 759, 3933, - -32768, 757, - -32768, 512, - -32768,
3933, 764, 762, - -32768, - -32768, - -32768, - -32768, 2524, - -32768, - -32768,
2524, - -32768, 2524, 2524, 521, 3933, 971, - -32768, - -32768, 537, 775, 346,
763, - -32768, 280, 1319, - -32768, - -32768, - -32768, - -32768, - -32768, 2524,
339, 509, - -32768, - -32768, 767, - -32768, - -32768, 1087, - -32768, - -32768,
971, - -32768, - -32768, 558, - -32768, 509, - -32768, - -32768, - -32768, 861,
868, - -32768}; static short Cyc_yypgoto[ 112u]={ - -32768, - 20, - -32768, 561,
- -32768, - -32768, - -32768, - -32768, - -32768, - 51, - 41, - 55, - -32768,
180, - 40, 302, - -32768, 12, - 189, - 47, - -32768, - -32768, 437, - -32768, -
28, - -32768, - 250, - -32768, - -32768, - -32768, 671, 664, 8, - -32768, -
-32768, 352, - -32768, - -32768, - 17, - -32768, - -32768, - 23, - 38, - 122, -
-32768, - -32768, - 57, - -32768, - 101, - 104, - 368, 142, 234, 318, 589, - 321,
- 84, - 276, 765, - -32768, - 194, - 151, - 336, - 263, - -32768, 439, - 135, -
65, - 93, 71, - 257, 132, - -32768, - -32768, - 35, - 209, - -32768, - -32768, -
636, - 340, - -32768, - -32768, - 21, 610, - -32768, 310, - -32768, - -32768,
179, 149, - -32768, - 145, - 360, - 150, 567, 569, 563, 575, 579, 291, 148, 287,
292, - 11, 444, - -32768, - -32768, - -32768, - 317, - -32768, - -32768, 9};
static short Cyc_yytable[ 4402u]={ 114, 122, 78, 150, 112, 305, 306, 126, 370,
59, 302, 86, 258, 443, 113, 257, 119, 142, 285, 120, 115, 90, 468, 99, 261, 383,
516, 295, 461, 528, 529, 64, 79, 316, 80, 139, 409, 139, 461, 430, 139, 512, 713,
59, 114, 205, 206, 207, 112, 128, 59, 136, 1, 562, 132, 59, 239, 139, 114, 30,
238, 105, 251, 109, 204, 32, 59, 323, 123, 139, 139, 196, 197, 198, 199, 208,
570, 213, 252, 79, 119, 217, 573, 545, 79, 60, 361, 755, 283, 583, 140, 253, 140,
556, 138, 451, 138, 284, 362, 138, 59, 59, 59, 59, 265, 431, 483, 324, 106, 403,
1, 452, 212, 363, 61, 290, 138, 215, - 117, 107, 774, 139, 756, 59, 520, 140,
294, 404, 138, 138, 388, 581, 526, 59, 30, 423, 269, 783, 79, 399, 32, 389, 427,
400, 468, 288, 788, 34, 114, 656, 291, 790, 251, 289, 292, 421, 91, 92, 59, 319,
320, 285, 321, 139, 469, 617, 470, 411, 373, 594, 291, 295, 81, 595, 424, 611,
140, 482, 253, 354, 138, 16, 17, 18, 291, 82, 61, 384, 657, 381, 239, 116, 30,
83, 238, 84, 117, 412, 32, 61, 118, - 208, 331, 332, 462, 293, - 208, - 208, 143,
463, 59, 286, - 208, 464, 284, 144, 407, 426, 212, 655, 296, 59, 138, 509, 513,
385, 467, 100, 101, 102, 510, 514, 521, 339, 139, 139, 139, 176, 195, 333, 334,
340, 341, 87, 139, 106, 139, 465, 59, 256, 125, 139, 139, 478, 139, 446, 107,
120, - 86, 291, 239, - 86, 580, 672, 238, 139, 84, 263, 30, 669, 86, 37, 38, 39,
32, 103, 697, 638, 104, 413, 276, 596, 30, 277, 414, 79, 79, 106, 32, 454, 455,
521, 612, 138, 138, 138, 613, 521, 107, 140, 88, 264, 59, 138, 59, 138, 564, 140,
708, 140, 138, 138, 709, 138, 275, 443, 411, 59, 281, 282, 140, 609, 150, 89,
138, 124, 730, 139, 505, 506, 507, 701, 663, - 183, 278, 139, 127, 114, 129, -
183, 279, 539, 355, 356, 357, 130, 518, 317, 519, 410, 663, 372, 114, 131, 391,
411, 112, 527, 587, 139, 398, 586, 401, 402, - 86, 639, 549, 133, 256, 84, 588,
411, 550, 139, 420, 59, 734, 59, 411, 358, 521, 563, 212, 779, 283, 135, 138,
359, 360, 575, 212, 775, 148, 284, 138, 37, 38, 39, 441, 442, 84, 662, 1, 216,
439, 542, 397, 278, 621, 565, 411, 411, 134, 555, 622, 139, 139, 571, 138, 662,
732, 465, 574, 632, 614, 683, 93, 94, 686, 411, 725, 411, 138, 460, 411, 1, 411,
411, 59, 786, 777, 200, 782, 269, 411, 789, 663, 79, 297, 298, 665, 666, 79, 372,
201, 599, 214, 644, 329, 330, 481, 645, 646, 647, 671, 335, 336, 486, 218, 705,
139, 140, 266, 606, 138, 138, 337, 338, 317, 497, 498, 499, 500, 270, 30, 289,
476, 59, 274, 480, 32, 317, 301, 95, 114, 1, 307, 34, 251, 485, 411, 381, 508,
490, 643, 308, 139, 486, 37, 38, 39, 372, 528, 529, 637, 662, 309, 30, 757, 597,
411, 601, 411, 32, 310, 140, 623, 411, 311, 138, 34, 689, 690, 59, 694, 546, 318,
548, 325, 515, 620, 37, 38, 39, 114, 625, 411, 773, 112, 486, 1, 626, 411, 139,
322, 486, 780, 446, 677, 411, 551, 714, 411, 326, 204, 138, 533, 327, 535, 536,
785, 540, 364, 787, 716, 411, 365, 30, 720, 411, 740, 741, 367, 32, 747, 411,
328, 79, 750, 411, 34, 374, 673, 752, 411, 375, 300, 377, 382, 303, 303, 37, 38,
39, 674, 675, 313, 314, 664, 763, 298, 386, 138, 590, 303, 303, 387, 303, 771,
411, 495, 496, 501, 502, 372, 390, 738, 392, 759, 503, 504, 393, 30, 65, 303,
394, 395, 405, 32, 408, 418, - 180, 406, 323, 417, 293, - 180, - 180, 202, 139,
429, 422, - 180, 435, 284, 434, 37, 38, 39, 436, 139, 425, 437, 139, 438, 453,
479, 484, 726, 517, 523, 524, 239, 534, 537, 525, 238, 541, 30, 67, 543, 68, 69,
552, 32, 547, 554, 65, 560, 440, 557, 70, 566, 558, 71, 72, 567, 572, 79, 441,
442, 239, 73, 577, 568, 238, 74, 75, 138, 578, 239, 278, 579, 624, 238, 736, 582,
629, 631, 138, 592, 584, 138, 585, 239, 591, 593, 239, 238, 598, 600, 238, 66,
67, 602, 68, 69, 603, 32, 604, 608, 65, 615, 607, 372, 70, 618, 610, 71, 72, 619,
627, 633, 634, 372, 676, 73, 670, 679, 635, 74, 75, 372, 640, 648, 636, 642, 303,
652, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
303, 303, 30, 67, 650, 68, 69, 653, 32, 654, 658, 605, 659, 667, 660, 70, 678,
668, 71, 72, 692, 693, 680, 682, 256, 685, 73, 688, 372, 715, 74, 75, 719, 695,
698, 711, 699, 303, 700, 702, 703, 729, 342, 343, 344, 345, 346, 347, 348, 349,
350, 351, 706, 707, 710, 704, 712, 727, 728, 739, 731, 733, 735, 743, 745, 746,
744, 352, 748, 749, 762, 751, 766, 760, 372, 776, 778, 792, 718, 65, 784, 722,
724, 765, 793, 477, 649, 569, 369, 758, 376, 641, 616, 696, 466, 767, 303, 260,
768, 576, 769, 770, 303, 450, 651, 492, 489, 0, 0, 0, 491, 0, 0, 0, 0, 0, 0, 0,
493, 781, 754, 0, 0, 494, 30, 67, 0, 68, 69, 761, 32, 0, 0, 0, 0, 764, 0, 70, 0,
0, 71, 72, 0, 0, 303, 441, 442, 0, 73, 0, 772, 0, 74, 75, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 303, 0, 0,
0, 0, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 689,
690, 21, 152, 153, 219, 0, 220, 221, 222, 223, 224, 225, 226, 227, 22, 154, 23,
155, 228, 0, 156, 24, 229, 0, 0, 27, 28, 157, 158, 230, 231, 159, 29, 232, 160,
161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 303, 0, 0, 0, 0, 0, 0, 0,
303, 233, 164, 165, 166, 167, 31, 32, 33, 0, 234, 111, 0, 0, 168, 0, 0, 35, 236,
0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 2, 3, 4, 110,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 740, 741, 21, 152, 153,
219, 0, 220, 221, 222, 223, 224, 225, 226, 227, 22, 154, 23, 155, 228, 0, 156,
24, 229, 0, 0, 27, 28, 157, 158, 230, 231, 159, 29, 232, 160, 161, 0, 162, 163,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 164, 165,
166, 167, 31, 32, 33, 0, 234, 111, 0, 0, 168, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0,
170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 152, 153, 219, 0, 220, 221, 222,
223, 224, 225, 226, 227, 22, 154, 23, 155, 228, 0, 156, 24, 229, 0, 0, 27, 28,
157, 158, 230, 231, 159, 29, 232, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 164, 165, 166, 167, 31, 32, 33, 0,
234, 111, 235, 0, 168, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173,
0, 0, 0, 0, 174, 175, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 0, 21, 152, 153, 219, 0, 220, 221, 222, 223, 224, 225, 226, 227,
22, 154, 23, 155, 228, 0, 156, 24, 229, 0, 0, 27, 28, 157, 158, 230, 231, 159,
29, 232, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 233, 164, 165, 166, 167, 31, 32, 33, 0, 234, 111, 0, 0, 168, 0, 0,
35, 236, 0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 2, 3,
4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 152,
153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 154, 23, 155, 0, 0, 156, 24, 0, 0, 0, 27,
28, 157, 158, 0, 0, 159, 29, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 31, 32, 33, 0, 538,
0, 0, 0, 168, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0,
0, 174, 175, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 152,
153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 154, 0, 155, 0, 0, 156, 0, 0, 0, 0, 27,
28, 157, 158, 0, 0, 159, 29, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 31, 32, 33, 0, 0,
315, 0, 0, 168, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0,
0, 0, 174, 175, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0,
152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 154, 0, 155, 0, 0, 156, 0, 0, 0, 0,
27, 28, 157, 158, 0, 0, 159, 29, 0, 160, 161, 0, 162, 163, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 31,
32, 33, 22, 0, 0, 0, 0, 168, 0, 0, 35, 236, 0, 27, 28, 0, 0, 0, 170, 0, 29, 171,
172, 173, 0, 0, 0, 0, 174, 175, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 31, 21, 33, 0, 0, 137, 0, 0, 0, 0, 0, 35, 36,
0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0,
0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39,
- 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26,
27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 0, 0,
35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0,
22, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37,
38, 39, 1, 0, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 0, 22, 0, 35, 36, 0, 0, 0, 24,
0, 0, 0, 27, 28, 1, 0, 0, 0, 0, 29, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0,
0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0,
256, 0, 0, 0, 293, - 159, 0, 35, 36, 0, 0, - 159, 0, 284, 0, 37, 38, 39, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0,
0, 0, 0, 22, 0, 31, 0, 33, 0, 256, 0, 0, 0, 283, 27, 28, 35, 36, 0, 0, 0, 29,
284, 0, 37, 38, 39, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 254, 0, 21, 255, 0, 0, 0, 0, 31, 0, 33, 0, 256, 0, 0, 22, 0, 23, 0, 35,
36, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 2, 3, 4, 110, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 31, 0,
33, 0, 0, 111, 22, 202, 23, 0, 0, 35, 36, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29,
0, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0, 111, 0, 0, 0, 0, 24, 35, 36, 0,
27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 152, 153, 219, 0, 220, 221, 222, 223, 224, 225, 226, 227, 31, 154, 33, 155,
228, 0, 156, 0, 229, 0, 0, 35, 36, 157, 158, 230, 231, 159, 0, 232, 160, 161, 0,
162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233,
164, 165, 166, 167, 0, 32, 152, 153, 234, 111, 0, 0, 168, 471, 0, 0, 169, 0, 0,
154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0,
160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 0, 0, 304, 472, 0, 168, 0, 0, 0, 169,
152, 153, 0, 441, 442, 0, 170, 0, 0, 171, 172, 173, 0, 154, 0, 155, 174, 175,
156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0,
32, 0, 0, 0, 304, 661, 0, 168, 0, 0, 0, 169, 152, 153, 0, 441, 442, 0, 170, 0, 0,
171, 172, 173, 0, 154, 0, 155, 174, 175, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0,
159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 0, 0, 304, 737, 0, 168, 0, 0, 0,
169, 152, 153, 0, 441, 442, 0, 170, 0, 0, 171, 172, 173, 0, 154, 0, 155, 174,
175, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 0, 0, 160, 161, 0, 162, 163, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166,
167, 0, 32, 0, 0, 0, 304, 0, 0, 168, 0, 0, 0, 169, 152, 153, 0, 441, 442, 0, 170,
0, 0, 171, 172, 173, 0, 154, 0, 155, 174, 175, 156, 0, 0, 0, 0, 0, 0, 157, 158,
0, 0, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 0,
0, 0, 169, 0, 0, 154, 0, 155, 262, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158,
174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 304, 0, 0,
168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157,
158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 396, 0, 0,
0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0,
157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0,
0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 459, 170, 156, 0, 171, 172, 173,
0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152,
153, 0, 0, 0, 0, 168, 511, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171,
172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32,
152, 153, 0, 0, 0, 0, 168, 544, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0,
171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167,
0, 32, 152, 153, 0, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 589, 170, 156,
0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166,
167, 0, 32, 152, 153, 628, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170,
156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162,
163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165,
166, 167, 0, 32, 152, 153, 630, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0,
170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0,
162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164,
165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 681, 0, 0, 169, 0, 0, 154, 0,
155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160,
161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 684, 0, 0, 169, 0, 0,
154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0,
160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 687, 0, 0, 0, 168, 0, 0, 0, 169,
0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159,
0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 717, 0, 0,
169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175,
159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 721, 0,
0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174,
175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168,
723, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158,
174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0,
168, 753, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157,
158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 152, 153, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 154, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 157, 158,
0, 0, 159, 168, 0, 0, 161, 169, 162, 163, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173,
0, 0, 0, 0, 174, 175, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0,
304, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0,
157, 158, 174, 175, 159, 0, 0, 0, 161, 0, 162, 163, 0, 0, 0, 0, 152, 153, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 157,
158, 605, 0, 159, 168, 0, 0, 161, 169, 162, 163, 0, 0, 0, 0, 170, 0, 0, 171, 172,
173, 0, 0, 0, 0, 174, 175, 0, 0, 152, 153, 0, 30, 164, 165, 166, 167, 0, 32, 0,
0, 0, 154, 0, 0, 299, 0, 0, 0, 169, 0, 0, 0, 0, 157, 158, 170, 0, 159, 171, 172,
173, 161, 0, 162, 163, 174, 175, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
154, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 157, 158, 0, 0, 159, 312, 0, 0,
161, 169, 162, 163, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175,
0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 0, 0, 0, 0, 0, 168, 0, 0, 0,
169, 0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 22, 0, 0, 0, 29,
0, 0, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254,
0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 256, 0, 561, 0, 0, 0, 0, 35, 36, 0, 31, 0, 33,
0, 0, 0, 0, 0, 0, 0, 0, 35, 36}; static short Cyc_yycheck[ 4402u]={ 55, 58, 23,
87, 55, 156, 156, 62, 202, 0, 155, 34, 116, 276, 55, 116, 56, 82, 140, 57, 55,
41, 298, 46, 117, 214, 362, 149, 285, 5, 6, 22, 23, 168, 25, 82, 245, 84, 295,
17, 87, 358, 678, 34, 99, 100, 101, 102, 99, 70, 41, 79, 1, 421, 75, 46, 111,
104, 113, 83, 111, 52, 113, 54, 99, 89, 57, 69, 59, 116, 117, 91, 92, 93, 94,
103, 436, 105, 113, 70, 120, 109, 442, 400, 75, 85, 81, 68, 96, 457, 82, 114, 84,
414, 82, 82, 84, 105, 93, 87, 91, 92, 93, 94, 124, 83, 315, 109, 88, 83, 1, 98,
104, 108, 91, 63, 104, 108, 94, 99, 756, 168, 103, 114, 374, 117, 149, 101, 116,
117, 85, 452, 382, 124, 83, 63, 127, 773, 129, 92, 89, 96, 264, 96, 420, 102,
782, 96, 203, 63, 98, 787, 203, 110, 102, 259, 92, 93, 149, 170, 171, 283, 173,
210, 299, 525, 301, 98, 203, 94, 98, 293, 83, 98, 102, 106, 168, 312, 201, 190,
168, 17, 18, 19, 98, 101, 91, 215, 102, 212, 245, 96, 83, 82, 245, 101, 101, 254,
89, 91, 105, 92, 64, 65, 96, 96, 97, 98, 83, 101, 201, 140, 103, 105, 105, 90,
239, 264, 210, 587, 149, 212, 210, 83, 83, 216, 291, 47, 48, 49, 90, 90, 377,
107, 281, 282, 283, 88, 89, 101, 102, 115, 116, 96, 291, 88, 293, 287, 239, 92,
96, 298, 299, 308, 301, 276, 99, 295, 93, 98, 315, 96, 451, 102, 315, 312, 101,
118, 83, 605, 293, 107, 108, 109, 89, 90, 644, 553, 93, 88, 93, 475, 83, 96, 93,
276, 277, 88, 89, 281, 282, 436, 94, 281, 282, 283, 98, 442, 99, 291, 93, 121,
293, 291, 295, 293, 428, 299, 94, 301, 298, 299, 98, 301, 135, 578, 98, 308, 138,
139, 312, 103, 406, 93, 312, 93, 694, 374, 339, 340, 341, 652, 595, 92, 88, 382,
93, 392, 96, 98, 94, 392, 59, 60, 61, 83, 366, 168, 368, 92, 613, 202, 407, 84,
222, 98, 407, 385, 462, 406, 228, 462, 230, 231, 96, 554, 407, 92, 92, 101, 463,
98, 407, 420, 98, 366, 103, 368, 98, 96, 525, 428, 374, 103, 96, 95, 374, 104,
105, 95, 382, 759, 97, 105, 382, 107, 108, 109, 104, 105, 101, 595, 1, 104, 92,
92, 227, 88, 92, 429, 98, 98, 98, 94, 98, 462, 463, 438, 406, 613, 696, 461, 443,
92, 517, 92, 92, 93, 92, 98, 92, 98, 420, 284, 98, 1, 98, 98, 429, 779, 94, 92,
103, 434, 98, 785, 709, 438, 97, 98, 601, 601, 443, 304, 98, 478, 82, 561, 66,
67, 311, 83, 84, 85, 609, 62, 63, 318, 85, 663, 517, 463, 96, 484, 462, 463, 110,
111, 299, 331, 332, 333, 334, 97, 83, 110, 307, 478, 83, 310, 89, 312, 96, 92,
549, 1, 96, 96, 549, 97, 98, 524, 353, 324, 561, 96, 553, 358, 107, 108, 109,
362, 5, 6, 549, 709, 96, 83, 727, 97, 98, 97, 98, 89, 96, 517, 97, 98, 96, 517,
96, 22, 23, 524, 638, 403, 96, 405, 68, 360, 531, 107, 108, 109, 599, 97, 98,
756, 599, 400, 1, 97, 98, 600, 94, 406, 765, 578, 97, 98, 411, 97, 98, 113, 599,
553, 387, 114, 389, 390, 779, 392, 94, 782, 97, 98, 94, 83, 97, 98, 22, 23, 94,
89, 97, 98, 112, 578, 97, 98, 96, 93, 620, 97, 98, 94, 152, 103, 93, 155, 156,
107, 108, 109, 621, 622, 162, 163, 600, 97, 98, 93, 600, 464, 170, 171, 96, 173,
97, 98, 329, 330, 335, 336, 475, 96, 710, 96, 732, 337, 338, 83, 83, 38, 190, 92,
92, 103, 89, 94, 103, 92, 96, 69, 97, 96, 97, 98, 95, 696, 94, 97, 103, 94, 105,
98, 107, 108, 109, 95, 707, 106, 97, 710, 98, 94, 25, 97, 689, 101, 92, 98, 727,
96, 30, 103, 727, 92, 83, 84, 42, 86, 87, 83, 89, 88, 82, 38, 97, 94, 110, 96,
96, 88, 99, 100, 97, 83, 689, 104, 105, 756, 107, 94, 98, 756, 111, 112, 696, 98,
765, 88, 97, 534, 765, 707, 97, 538, 539, 707, 98, 97, 710, 106, 779, 97, 83,
782, 779, 97, 96, 782, 83, 84, 98, 86, 87, 97, 89, 94, 98, 38, 94, 97, 595, 96,
94, 97, 99, 100, 94, 96, 93, 92, 605, 623, 107, 608, 626, 97, 111, 112, 613, 94,
97, 102, 101, 323, 98, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335,
336, 337, 338, 339, 340, 341, 83, 84, 106, 86, 87, 97, 89, 97, 106, 93, 83, 25,
101, 96, 93, 97, 99, 100, 92, 83, 627, 628, 92, 630, 107, 632, 663, 681, 111,
112, 684, 88, 98, 28, 97, 377, 97, 97, 97, 693, 70, 71, 72, 73, 74, 75, 76, 77,
78, 79, 97, 96, 96, 660, 93, 103, 94, 711, 98, 84, 97, 94, 716, 717, 92, 95, 720,
721, 97, 723, 94, 98, 709, 84, 97, 0, 683, 38, 97, 686, 687, 103, 0, 308, 568,
434, 201, 731, 210, 557, 524, 643, 289, 747, 436, 116, 750, 444, 752, 753, 442,
277, 578, 326, 323, - 1, - 1, - 1, 325, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 327,
771, 725, - 1, - 1, 328, 83, 84, - 1, 86, 87, 734, 89, - 1, - 1, - 1, - 1, 740,
- 1, 96, - 1, - 1, 99, 100, - 1, - 1, 484, 104, 105, - 1, 107, - 1, 755, - 1,
111, 112, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 525, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36,
37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55,
56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 601, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 609, 83, 84, 85, 86, 87, 88, 89, 90,
- 1, 92, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107,
- 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30,
31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49,
50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84,
85, 86, 87, 88, 89, 90, - 1, 92, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1,
- 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43,
44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, 61, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, 92, 93, 94, - 1, 96, -
1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1,
- 1, - 1, - 1, 117, 118, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37,
38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1,
92, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1,
- 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 37, 38, 39, 40, - 1, - 1, 43, 44, - 1, - 1, - 1,
48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, 92, - 1, - 1, - 1, 96, - 1, - 1, 99,
100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1,
- 1, 117, 118, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, -
1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, -
1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, - 1, 93,
- 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1,
110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51,
- 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, 61, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89,
90, 37, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, 48, 49, - 1, - 1, - 1,
107, - 1, 55, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 0, 1, - 1, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 88, 24, 90, - 1, -
1, 93, - 1, - 1, - 1, - 1, - 1, 99, 100, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1,
44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, - 1, - 1, 88, 89, 90, - 1, - 1, - 1,
94, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, 108, 109, 0,
1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1,
- 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39,
- 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, - 1, - 1, 88, 89,
90, - 1, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1,
107, 108, 109, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, -
1, - 1, 55, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, 37, - 1, 88, 89,
90, - 1, - 1, - 1, 94, - 1, 96, 48, 49, 99, 100, - 1, - 1, - 1, 55, - 1, - 1,
107, 108, 109, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, 88, - 1, 90, - 1, - 1, - 1, - 1, - 1, -
1, 37, - 1, 99, 100, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, 1, - 1, - 1, - 1,
- 1, 55, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, 37, - 1, 88, 89,
90, - 1, - 1, - 1, - 1, - 1, 96, 48, 49, 99, 100, - 1, - 1, - 1, 55, - 1, - 1,
107, 108, 109, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 80, - 1, - 1, 83, - 1, - 1, - 1, - 1, 88, 89, 90,
- 1, 92, - 1, - 1, - 1, 96, 97, - 1, 99, 100, - 1, - 1, 103, - 1, 105, - 1, 107,
108, 109, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 80, - 1, - 1, - 1, - 1, - 1, 37, - 1, 88, - 1, 90, - 1, 92, - 1, - 1, -
1, 96, 48, 49, 99, 100, - 1, - 1, - 1, 55, 105, - 1, 107, 108, 109, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 80, - 1, 24, 83, - 1, -
1, - 1, - 1, 88, - 1, 90, - 1, 92, - 1, - 1, 37, - 1, 39, - 1, 99, 100, - 1, 44,
- 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, -
1, - 1, - 1, - 1, - 1, 88, - 1, 90, - 1, - 1, 93, 37, 95, 39, - 1, - 1, 99, 100,
44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, - 1, 90, 37, - 1, 93, - 1, - 1, - 1, - 1, 44, 99,
100, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 25, 26, 27, - 1,
29, 30, 31, 32, 33, 34, 35, 36, 88, 38, 90, 40, 41, - 1, 43, - 1, 45, - 1, - 1,
99, 100, 50, 51, 52, 53, 54, - 1, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, 93, - 1, - 1, 96, 32, - 1, - 1, 100,
- 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117,
118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86,
87, - 1, 89, - 1, - 1, - 1, 93, 94, - 1, 96, - 1, - 1, - 1, 100, 25, 26, - 1,
104, 105, - 1, 107, - 1, - 1, 110, 111, 112, - 1, 38, - 1, 40, 117, 118, 43, - 1,
- 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, 61, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 93, 94, - 1,
96, - 1, - 1, - 1, 100, 25, 26, - 1, 104, 105, - 1, 107, - 1, - 1, 110, 111, 112,
- 1, 38, - 1, 40, 117, 118, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1,
54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87,
- 1, 89, - 1, - 1, - 1, 93, 94, - 1, 96, - 1, - 1, - 1, 100, 25, 26, - 1, 104,
105, - 1, 107, - 1, - 1, 110, 111, 112, - 1, 38, - 1, 40, 117, 118, 43, - 1, - 1,
- 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 93, - 1, - 1, 96,
- 1, - 1, - 1, 100, 25, 26, - 1, 104, 105, - 1, 107, - 1, - 1, 110, 111, 112, -
1, 38, - 1, 40, 117, 118, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54,
- 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1,
89, 25, 26, - 1, - 1, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40,
106, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57,
58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1,
93, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1,
110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1, - 1, - 1,
96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112,
- 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, - 1, - 1, -
1, 100, - 1, - 1, 38, - 1, 40, 106, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50,
51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84,
85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1,
38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, -
1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89,
25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1,
107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1,
- 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, 106, 107, 43, - 1, 110,
111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1, - 1, - 1, 96, -
1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1,
- 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1, - 1, - 1, 96, - 1, - 1, - 1,
100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51,
117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85,
86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38,
- 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1,
- 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89,
25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1,
107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1,
- 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110,
111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96,
97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1,
- 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1,
100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51,
117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85,
86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38,
- 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1,
- 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89,
25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1,
107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 38, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, 50, 51, - 1, -
1, 54, 96, - 1, - 1, 58, 100, 60, 61, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110,
111, 112, - 1, - 1, - 1, - 1, 117, 118, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86,
87, - 1, 89, 25, 26, - 1, 93, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, -
1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1,
- 1, - 1, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, -
1, 50, 51, 93, - 1, 54, 96, - 1, - 1, 58, 100, 60, 61, - 1, - 1, - 1, - 1, 107,
- 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, - 1, - 1, 25, 26, - 1, 83,
84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 38, - 1, - 1, 96, - 1, - 1, - 1, 100, -
1, - 1, - 1, - 1, 50, 51, 107, - 1, 54, 110, 111, 112, 58, - 1, 60, 61, 117, 118,
- 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1,
- 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, 50, 51, - 1, - 1, 54, 96, - 1, - 1,
58, 100, 60, 61, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1,
- 1, 117, 118, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, -
1, - 1, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1,
- 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, 37, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, 37, - 1, -
1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 80, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 88, - 1, 90, - 1, 92, - 1, 80, - 1, - 1, - 1, - 1, 99, 100, - 1, 88, - 1,
90, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 99, 100}; unsigned char Cyc_Yyimpossible[
17u]="\000\000\000\000Yyimpossible"; unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tuple19{ void* f1; void* f2; } ; struct _tuple20{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short*
yyss=( short*)({ unsigned int _temp3288= 10000u; short* _temp3289=( short*)
GC_malloc_atomic( sizeof( short) * _temp3288);{ unsigned int _temp3290=
_temp3288; unsigned int i; for( i= 0; i < _temp3290; i ++){ _temp3289[ i]=(
short) 0;}}; _temp3289;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3285= 10000u; void** _temp3286=( void**) GC_malloc( sizeof(
void*) * _temp3285);{ unsigned int _temp3287= _temp3285; unsigned int i; for( i=
0; i < _temp3287; i ++){ _temp3286[ i]= Cyc_yylval;}}; _temp3286;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3282= 10000u; struct Cyc_Yyltype* _temp3283=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) * _temp3282);{ unsigned int
_temp3284= _temp3282; unsigned int i; for( i= 0; i < _temp3284; i ++){ _temp3283[
i]= Cyc_yynewloc();}}; _temp3283;}); int yystacksize= 10000; void* yyval= Cyc_yylval;
int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2;
yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate: yyss[
_check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate; if(
yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 794u, yystate)]; if( yyn == - 32768){ goto
yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <=
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
346: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 347u, Cyc_yychar)]:
231;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4401)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4402u, yyn)] != yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4402u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrlab;}} if( yyn == 793){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus != 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 794u, yystate)]; if(
yyn == 0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 403u, yyn)]; if( yylen > 0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset + 1) - yylen)];} switch(
yyn){ case 1: _LL1444: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1445:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1447=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1447[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1448; _temp1448.tag= Cyc_DeclList_tok; _temp1448.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1448;});
_temp1447;}); break; case 3: _LL1446: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1450=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1450[ 0]=({ struct Cyc_DeclList_tok_struct _temp1451; _temp1451.tag= Cyc_DeclList_tok;
_temp1451.f1=({ struct Cyc_List_List* _temp1452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1452->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1453=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1453->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1454=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1454[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1455; _temp1455.tag= Cyc_Absyn_Using_d;
_temp1455.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1455.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1455;}); _temp1454;}));
_temp1453->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1453;});
_temp1452->tl= 0; _temp1452;}); _temp1451;}); _temp1450;}); Cyc_Lex_leave_using();
break; case 4: _LL1449: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1457=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1457[ 0]=({ struct Cyc_DeclList_tok_struct _temp1458; _temp1458.tag= Cyc_DeclList_tok;
_temp1458.f1=({ struct Cyc_List_List* _temp1459=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1459->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1460=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1460->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1461=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1461[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1462; _temp1462.tag= Cyc_Absyn_Using_d;
_temp1462.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1462.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1462;});
_temp1461;})); _temp1460->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1460;}); _temp1459->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1459;}); _temp1458;});
_temp1457;}); break; case 5: _LL1456: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1464=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1464[ 0]=({ struct Cyc_DeclList_tok_struct _temp1465; _temp1465.tag= Cyc_DeclList_tok;
_temp1465.f1=({ struct Cyc_List_List* _temp1466=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1466->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1467=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1467->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1468=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1468[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1469; _temp1469.tag=
Cyc_Absyn_Namespace_d; _temp1469.f1=({ struct _tagged_arr* _temp1470=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1470[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1470;});
_temp1469.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1469;}); _temp1468;})); _temp1467->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1467;}); _temp1466->tl= 0; _temp1466;}); _temp1465;}); _temp1464;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1463: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1472=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1472[ 0]=({ struct Cyc_DeclList_tok_struct _temp1473; _temp1473.tag= Cyc_DeclList_tok;
_temp1473.f1=({ struct Cyc_List_List* _temp1474=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1474->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1475=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1475->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1476=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1476[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1477; _temp1477.tag=
Cyc_Absyn_Namespace_d; _temp1477.f1=({ struct _tagged_arr* _temp1478=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1478[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1478;});
_temp1477.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1477;}); _temp1476;})); _temp1475->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1475;}); _temp1474->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1474;}); _temp1473;});
_temp1472;}); break; case 7: _LL1471: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1480=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1480[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1481; _temp1481.tag= Cyc_DeclList_tok; _temp1481.f1=({ struct Cyc_List_List*
_temp1482=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1482->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1483=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1483->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1484=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1484[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1485; _temp1485.tag= Cyc_Absyn_ExternC_d;
_temp1485.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1485;}); _temp1484;})); _temp1483->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1483;}); _temp1482->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1482;}); _temp1481;});
_temp1480;}); break; case 8: _LL1479: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1487=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1487[ 0]=({ struct Cyc_DeclList_tok_struct _temp1488; _temp1488.tag= Cyc_DeclList_tok;
_temp1488.f1= 0; _temp1488;}); _temp1487;}); break; case 9: _LL1486: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1490=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1490[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1491; _temp1491.tag= Cyc_DeclList_tok; _temp1491.f1=({ struct Cyc_List_List*
_temp1492=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1492->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1493=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1493[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1494; _temp1494.tag= Cyc_Absyn_Fn_d;
_temp1494.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1494;}); _temp1493;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1492->tl=
0; _temp1492;}); _temp1491;}); _temp1490;}); break; case 10: _LL1489: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1495: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1497=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1497[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1498; _temp1498.tag= Cyc_FnDecl_tok; _temp1498.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1498;});
_temp1497;}); break; case 12: _LL1496: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1500=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1500[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1501; _temp1501.tag= Cyc_FnDecl_tok;
_temp1501.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1502=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1502->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1502;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1501;}); _temp1500;}); break; case 13: _LL1499: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1504=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1504[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1505; _temp1505.tag= Cyc_FnDecl_tok;
_temp1505.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1505;}); _temp1504;}); break; case 14: _LL1503: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1507=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1507[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1508; _temp1508.tag= Cyc_FnDecl_tok;
_temp1508.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1509=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1509->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1509;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1508;}); _temp1507;}); break; case 15: _LL1506: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1511=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1511[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1512; _temp1512.tag= Cyc_FnDecl_tok;
_temp1512.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1513=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1513->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1513;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1512;}); _temp1511;}); break; case 16: _LL1510: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1515=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1515[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1516; _temp1516.tag= Cyc_FnDecl_tok;
_temp1516.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1517=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1517->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1517;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1516;}); _temp1515;}); break; case 17: _LL1514: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1518:
Cyc_Lex_leave_using(); break; case 19: _LL1519: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1521=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1521[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1521;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1520: Cyc_Lex_leave_namespace(); break; case 21: _LL1522: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1524=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1524[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1525; _temp1525.tag= Cyc_DeclList_tok; _temp1525.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1525;}); _temp1524;}); break; case 22: _LL1523: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1527=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1527[ 0]=({ struct Cyc_DeclList_tok_struct _temp1528; _temp1528.tag= Cyc_DeclList_tok;
_temp1528.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1528;}); _temp1527;}); break; case 23: _LL1526: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1530=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1530[ 0]=({ struct Cyc_DeclList_tok_struct _temp1531; _temp1531.tag= Cyc_DeclList_tok;
_temp1531.f1=({ struct Cyc_List_List* _temp1532=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1532->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1532->tl=
0; _temp1532;}); _temp1531;}); _temp1530;}); break; case 24: _LL1529: { struct
Cyc_List_List* _temp1534= 0;{ struct Cyc_List_List* _temp1535= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1535 != 0; _temp1535=(( struct Cyc_List_List*) _check_null( _temp1535))->tl){
struct _tagged_arr* _temp1536=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1535))->hd; struct _tuple1* _temp1537=({ struct _tuple1*
_temp1540=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1540->f1=
Cyc_Absyn_rel_ns_null; _temp1540->f2= _temp1536; _temp1540;}); struct Cyc_Absyn_Vardecl*
_temp1538= Cyc_Absyn_new_vardecl( _temp1537, Cyc_Absyn_wildtyp( 0), 0);
_temp1534=({ struct Cyc_List_List* _temp1539=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1539->hd=( void*) _temp1538; _temp1539->tl=
_temp1534; _temp1539;});}} _temp1534=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1534); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1541=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1541[ 0]=({ struct Cyc_DeclList_tok_struct _temp1542; _temp1542.tag= Cyc_DeclList_tok;
_temp1542.f1=({ struct Cyc_List_List* _temp1543=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1543->hd=( void*) Cyc_Absyn_letv_decl(
_temp1534, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1543->tl= 0; _temp1543;}); _temp1542;});
_temp1541;}); break;} case 25: _LL1533: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1544:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1546=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1546[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1547; _temp1547.tag= Cyc_DeclList_tok; _temp1547.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1547;});
_temp1546;}); break; case 27: _LL1545: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1549=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1549[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1550; _temp1550.tag= Cyc_DeclSpec_tok;
_temp1550.f1=({ struct Cyc_Parse_Declaration_spec* _temp1551=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1551->sc=({ struct
Cyc_Core_Opt* _temp1552=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1552->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1552;});
_temp1551->tq= Cyc_Absyn_empty_tqual(); _temp1551->type_specs= 0; _temp1551->is_inline=
0; _temp1551->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1551;}); _temp1550;});
_temp1549;}); break; case 28: _LL1548: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1554=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1554[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1555; _temp1555.tag= Cyc_DeclSpec_tok; _temp1555.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1556=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1556->sc=({ struct Cyc_Core_Opt* _temp1557=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1557->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1557;});
_temp1556->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1556->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1556->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1556->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1556;}); _temp1555;}); _temp1554;}); break; case 29: _LL1553: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1559=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1559[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1560; _temp1560.tag= Cyc_DeclSpec_tok; _temp1560.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1561=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1561->sc= 0; _temp1561->tq= Cyc_Absyn_empty_tqual(); _temp1561->type_specs=({
struct Cyc_List_List* _temp1562=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1562->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1562->tl= 0;
_temp1562;}); _temp1561->is_inline= 0; _temp1561->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1561;});
_temp1560;}); _temp1559;}); break; case 30: _LL1558: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1564=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1564[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1565; _temp1565.tag= Cyc_DeclSpec_tok;
_temp1565.f1=({ struct Cyc_Parse_Declaration_spec* _temp1566=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1566->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1566->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1566->type_specs=({ struct Cyc_List_List* _temp1567=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1567->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1567->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1567;});
_temp1566->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1566->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1566;}); _temp1565;}); _temp1564;}); break; case 31: _LL1563: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1569=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1569[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1570; _temp1570.tag= Cyc_DeclSpec_tok; _temp1570.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1571=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1571->sc= 0; _temp1571->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1571->type_specs=
0; _temp1571->is_inline= 0; _temp1571->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1571;});
_temp1570;}); _temp1569;}); break; case 32: _LL1568: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1573=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1573[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1574; _temp1574.tag= Cyc_DeclSpec_tok;
_temp1574.f1=({ struct Cyc_Parse_Declaration_spec* _temp1575=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1575->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1575->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1575->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1575->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1575->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1575;}); _temp1574;}); _temp1573;}); break; case 33: _LL1572: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1577=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1577[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1578; _temp1578.tag= Cyc_DeclSpec_tok; _temp1578.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1579=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1579->sc= 0; _temp1579->tq= Cyc_Absyn_empty_tqual(); _temp1579->type_specs=
0; _temp1579->is_inline= 1; _temp1579->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1579;});
_temp1578;}); _temp1577;}); break; case 34: _LL1576: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1581=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1581[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1582; _temp1582.tag= Cyc_DeclSpec_tok;
_temp1582.f1=({ struct Cyc_Parse_Declaration_spec* _temp1583=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1583->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1583->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1583->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1583->is_inline=
1; _temp1583->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1583;}); _temp1582;}); _temp1581;}); break; case 35: _LL1580: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1585=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1585[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1586; _temp1586.tag= Cyc_StorageClass_tok;
_temp1586.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1586;}); _temp1585;});
break; case 36: _LL1584: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1588=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1588[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1589; _temp1589.tag=
Cyc_StorageClass_tok; _temp1589.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1589;}); _temp1588;}); break; case 37: _LL1587: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1591=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1591[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1592; _temp1592.tag=
Cyc_StorageClass_tok; _temp1592.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1592;}); _temp1591;}); break; case 38: _LL1590: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1594=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1594[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1595; _temp1595.tag=
Cyc_StorageClass_tok; _temp1595.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1595;}); _temp1594;}); break; case 39: _LL1593: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1597=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1597[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1598; _temp1598.tag= Cyc_StorageClass_tok;
_temp1598.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1598;}); _temp1597;});
break; case 40: _LL1596: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1600=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1600[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1601; _temp1601.tag=
Cyc_StorageClass_tok; _temp1601.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1601;}); _temp1600;}); break; case 41: _LL1599: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1603=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1603[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1604; _temp1604.tag=
Cyc_StorageClass_tok; _temp1604.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1604;}); _temp1603;}); break; case 42: _LL1602: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1606=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1606[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1607; _temp1607.tag=
Cyc_AttributeList_tok; _temp1607.f1= 0; _temp1607;}); _temp1606;}); break; case
43: _LL1605: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1608: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1610=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1610[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1611; _temp1611.tag=
Cyc_AttributeList_tok; _temp1611.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1611;});
_temp1610;}); break; case 45: _LL1609: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1613=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1613[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1614; _temp1614.tag=
Cyc_AttributeList_tok; _temp1614.f1=({ struct Cyc_List_List* _temp1615=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1615->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1615->tl= 0; _temp1615;}); _temp1614;}); _temp1613;});
break; case 46: _LL1612: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1617=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1617[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1618; _temp1618.tag=
Cyc_AttributeList_tok; _temp1618.f1=({ struct Cyc_List_List* _temp1619=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1619->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1619->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1619;}); _temp1618;});
_temp1617;}); break; case 47: _LL1616: { struct _tagged_arr _temp1621= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1621, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1621, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1621, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1621,
sizeof( unsigned char),( int)( _get_arr_size( _temp1621, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1621,
sizeof( unsigned char),( int)( _get_arr_size( _temp1621, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1621=( struct _tagged_arr) Cyc_String_substring(
_temp1621, 2,( int)( _get_arr_size( _temp1621, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1621,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1622=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1622[ 0]=({ struct Cyc_Attribute_tok_struct _temp1623; _temp1623.tag= Cyc_Attribute_tok;
_temp1623.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1623;}); _temp1622;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1624=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1624[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1625; _temp1625.tag= Cyc_Attribute_tok; _temp1625.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1625;}); _temp1624;});} break;}} case
48: _LL1620: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1627=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1627[ 0]=({ struct Cyc_Attribute_tok_struct _temp1628; _temp1628.tag= Cyc_Attribute_tok;
_temp1628.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1628;}); _temp1627;});
break; case 49: _LL1626: { struct _tagged_arr _temp1630= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1633; int _temp1634; struct _tuple16* _temp1631= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1633=*
_temp1631; _LL1635: _temp1634= _temp1633.f2; goto _LL1632; _LL1632: { void* a;
if( Cyc_String_zstrcmp( _temp1630, _tag_arr("regparm", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1630, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) == 0){ if( _temp1634 <= 0? 1: _temp1634 > 3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1636=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1636[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1637; _temp1637.tag= Cyc_Absyn_Regparm_att; _temp1637.f1= _temp1634;
_temp1637;}); _temp1636;});} else{ if( Cyc_String_zstrcmp( _temp1630, _tag_arr("aligned",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1630, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1634 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1634; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1638=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1638[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1639; _temp1639.tag= Cyc_Absyn_Aligned_att;
_temp1639.f1= _temp1634; _temp1639;}); _temp1638;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1640=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1640[ 0]=({ struct Cyc_Attribute_tok_struct _temp1641; _temp1641.tag= Cyc_Attribute_tok;
_temp1641.f1=( void*) a; _temp1641;}); _temp1640;}); break;}} case 50: _LL1629: {
struct _tagged_arr _temp1643= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1644= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1643, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1643, _tag_arr("__section__",
sizeof( unsigned char), 12u)) == 0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1645=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1645[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1646; _temp1646.tag=
Cyc_Absyn_Section_att; _temp1646.f1= _temp1644; _temp1646;}); _temp1645;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1647=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1647[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1648; _temp1648.tag= Cyc_Attribute_tok; _temp1648.f1=(
void*) a; _temp1648;}); _temp1647;}); break;} case 51: _LL1642: { struct
_tagged_arr _temp1650= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)]); struct _tagged_arr
_temp1651= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); int _temp1654; struct _tuple16 _temp1652=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1655:
_temp1654= _temp1652.f2; goto _LL1653; _LL1653: { int _temp1658; struct _tuple16
_temp1656=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _LL1659: _temp1658= _temp1656.f2; goto _LL1657; _LL1657: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_String_zstrcmp( _temp1650,
_tag_arr("format", sizeof( unsigned char), 7u)) == 0? 1: Cyc_String_zstrcmp(
_temp1650, _tag_arr("__format__", sizeof( unsigned char), 11u)) == 0){ if( Cyc_String_zstrcmp(
_temp1651, _tag_arr("printf", sizeof( unsigned char), 7u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1660=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1660[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1661; _temp1661.tag= Cyc_Absyn_Format_att;
_temp1661.f1=( void*)(( void*) Cyc_Absyn_Printf_ft); _temp1661.f2= _temp1654;
_temp1661.f3= _temp1658; _temp1661;}); _temp1660;});} else{ if( Cyc_String_zstrcmp(
_temp1651, _tag_arr("scanf", sizeof( unsigned char), 6u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1662=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1662[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1663; _temp1663.tag= Cyc_Absyn_Format_att;
_temp1663.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft); _temp1663.f2= _temp1654;
_temp1663.f3= _temp1658; _temp1663;}); _temp1662;});} else{ Cyc_Parse_err(
_tag_arr("unrecognized format type", sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line));}}}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1664=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1664[ 0]=({ struct Cyc_Attribute_tok_struct _temp1665; _temp1665.tag= Cyc_Attribute_tok;
_temp1665.f1=( void*) a; _temp1665;}); _temp1664;}); break;}}} case 52: _LL1649:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1667=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1667[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1668; _temp1668.tag= Cyc_TypeSpecifier_tok;
_temp1668.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1668;});
_temp1667;}); break; case 53: _LL1666: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1670=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1670[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1671; _temp1671.tag=
Cyc_TypeSpecifier_tok; _temp1671.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1671;}); _temp1670;}); break; case 54: _LL1669:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1673=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1673[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1674; _temp1674.tag= Cyc_TypeSpecifier_tok;
_temp1674.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1674;});
_temp1673;}); break; case 55: _LL1672: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1676=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1676[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1677; _temp1677.tag=
Cyc_TypeSpecifier_tok; _temp1677.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1678=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1678[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1679; _temp1679.tag=
Cyc_Parse_Short_spec; _temp1679.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1679;});
_temp1678;})); _temp1677;}); _temp1676;}); break; case 56: _LL1675: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1681=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1681[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1682; _temp1682.tag= Cyc_TypeSpecifier_tok;
_temp1682.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1682;});
_temp1681;}); break; case 57: _LL1680: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1684=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1684[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1685; _temp1685.tag=
Cyc_TypeSpecifier_tok; _temp1685.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1686=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1686[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1687; _temp1687.tag= Cyc_Parse_Long_spec;
_temp1687.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1687;}); _temp1686;})); _temp1685;}); _temp1684;});
break; case 58: _LL1683: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1689=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1689[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1690; _temp1690.tag=
Cyc_TypeSpecifier_tok; _temp1690.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1690;}); _temp1689;}); break; case 59: _LL1688:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1692=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1692[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1693; _temp1693.tag= Cyc_TypeSpecifier_tok;
_temp1693.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1693;});
_temp1692;}); break; case 60: _LL1691: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1695=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1695[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1696; _temp1696.tag=
Cyc_TypeSpecifier_tok; _temp1696.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1697=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1697[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1698; _temp1698.tag=
Cyc_Parse_Signed_spec; _temp1698.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1698;});
_temp1697;})); _temp1696;}); _temp1695;}); break; case 61: _LL1694: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1700=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1700[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1701; _temp1701.tag= Cyc_TypeSpecifier_tok;
_temp1701.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1702=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1702[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1703; _temp1703.tag= Cyc_Parse_Unsigned_spec; _temp1703.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1703;});
_temp1702;})); _temp1701;}); _temp1700;}); break; case 62: _LL1699: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1704:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1705: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1706: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1708=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1708[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1709; _temp1709.tag=
Cyc_TypeSpecifier_tok; _temp1709.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1709;});
_temp1708;}); break; case 66: _LL1707: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1711=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1711[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1712; _temp1712.tag=
Cyc_TypeSpecifier_tok; _temp1712.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1712;}); _temp1711;}); break; case 67: _LL1710: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1714=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1714[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1715; _temp1715.tag=
Cyc_TypeSpecifier_tok; _temp1715.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1716=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1716[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1717; _temp1717.tag= Cyc_Absyn_TypedefType;
_temp1717.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1717.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1717.f3= 0;
_temp1717;}); _temp1716;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1715;});
_temp1714;}); break; case 68: _LL1713: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1719=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1719[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1720; _temp1720.tag=
Cyc_TypeSpecifier_tok; _temp1720.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1721=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1721[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1722; _temp1722.tag= Cyc_Absyn_TupleType;
_temp1722.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1722;});
_temp1721;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1720;}); _temp1719;}); break; case 69:
_LL1718: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1724=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1724[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1725; _temp1725.tag=
Cyc_TypeSpecifier_tok; _temp1725.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1726=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1726[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1727; _temp1727.tag= Cyc_Absyn_RgnHandleType;
_temp1727.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1727;}); _temp1726;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1725;}); _temp1724;}); break; case 70: _LL1723: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1729=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1729[ 0]=({ struct Cyc_Kind_tok_struct _temp1730; _temp1730.tag= Cyc_Kind_tok;
_temp1730.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1730;});
_temp1729;}); break; case 71: _LL1728: { struct _tagged_arr* _temp1734; void*
_temp1736; struct _tuple1 _temp1732=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1737: _temp1736=
_temp1732.f1; goto _LL1735; _LL1735: _temp1734= _temp1732.f2; goto _LL1733;
_LL1733: if( _temp1736 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1738=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1738[ 0]=({ struct Cyc_Kind_tok_struct
_temp1739; _temp1739.tag= Cyc_Kind_tok; _temp1739.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1734, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1739;}); _temp1738;}); break;} case 72:
_LL1731: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1741=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1741[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1742; _temp1742.tag= Cyc_TypeQual_tok;
_temp1742.f1=({ struct Cyc_Absyn_Tqual _temp1743; _temp1743.q_const= 1;
_temp1743.q_volatile= 0; _temp1743.q_restrict= 0; _temp1743;}); _temp1742;});
_temp1741;}); break; case 73: _LL1740: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1745=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1745[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1746; _temp1746.tag= Cyc_TypeQual_tok; _temp1746.f1=({ struct Cyc_Absyn_Tqual
_temp1747; _temp1747.q_const= 0; _temp1747.q_volatile= 1; _temp1747.q_restrict=
0; _temp1747;}); _temp1746;}); _temp1745;}); break; case 74: _LL1744: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1749=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1749[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1750; _temp1750.tag= Cyc_TypeQual_tok;
_temp1750.f1=({ struct Cyc_Absyn_Tqual _temp1751; _temp1751.q_const= 0;
_temp1751.q_volatile= 0; _temp1751.q_restrict= 1; _temp1751;}); _temp1750;});
_temp1749;}); break; case 75: _LL1748: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1753=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1753[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1754; _temp1754.tag=
Cyc_TypeSpecifier_tok; _temp1754.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1755=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1755[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1756; _temp1756.tag= Cyc_Parse_Decl_spec;
_temp1756.f1=({ struct Cyc_Absyn_Decl* _temp1757=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1757->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1758=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1758[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1759; _temp1759.tag= Cyc_Absyn_Enum_d; _temp1759.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1760=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1760->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1760->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1760->fields=({
struct Cyc_Core_Opt* _temp1761=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1761->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1761;});
_temp1760;}); _temp1759;}); _temp1758;})); _temp1757->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1757;}); _temp1756;}); _temp1755;})); _temp1754;}); _temp1753;}); break;
case 76: _LL1752: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1763=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1763[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1764; _temp1764.tag=
Cyc_TypeSpecifier_tok; _temp1764.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1765=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1765[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1766; _temp1766.tag= Cyc_Absyn_EnumType;
_temp1766.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1766.f2= 0; _temp1766;}); _temp1765;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1764;}); _temp1763;}); break; case 77: _LL1762: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1768=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1768[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1769; _temp1769.tag= Cyc_Enumfield_tok;
_temp1769.f1=({ struct Cyc_Absyn_Enumfield* _temp1770=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1770->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1770->tag= 0;
_temp1770->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1770;});
_temp1769;}); _temp1768;}); break; case 78: _LL1767: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1772=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1772[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1773; _temp1773.tag= Cyc_Enumfield_tok;
_temp1773.f1=({ struct Cyc_Absyn_Enumfield* _temp1774=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1774->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1774->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1774->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1774;});
_temp1773;}); _temp1772;}); break; case 79: _LL1771: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1776=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1776[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1777; _temp1777.tag=
Cyc_EnumfieldList_tok; _temp1777.f1=({ struct Cyc_List_List* _temp1778=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1778->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1778->tl= 0; _temp1778;}); _temp1777;}); _temp1776;});
break; case 80: _LL1775: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1780=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1780[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1781; _temp1781.tag=
Cyc_EnumfieldList_tok; _temp1781.f1=({ struct Cyc_List_List* _temp1782=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1782->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1782->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1782;}); _temp1781;});
_temp1780;}); break; case 81: _LL1779: { void* t;{ void* _temp1784= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1786: if(
_temp1784 ==( void*) Cyc_Parse_Struct_su){ goto _LL1787;} else{ goto _LL1788;}
_LL1788: if( _temp1784 ==( void*) Cyc_Parse_Union_su){ goto _LL1789;} else{ goto
_LL1785;} _LL1787: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1790=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1790[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1791; _temp1791.tag= Cyc_Absyn_AnonStructType; _temp1791.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1791;});
_temp1790;}); goto _LL1785; _LL1789: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1792=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1792[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1793; _temp1793.tag= Cyc_Absyn_AnonUnionType; _temp1793.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1793;});
_temp1792;}); goto _LL1785; _LL1785:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1794=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1794[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1795; _temp1795.tag=
Cyc_TypeSpecifier_tok; _temp1795.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1796=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1796[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1797; _temp1797.tag= Cyc_Parse_Type_spec;
_temp1797.f1=( void*) t; _temp1797.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1797;});
_temp1796;})); _temp1795;}); _temp1794;}); break;} case 82: _LL1783: { struct
Cyc_List_List* _temp1799=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1800= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1802: if(
_temp1800 ==( void*) Cyc_Parse_Struct_su){ goto _LL1803;} else{ goto _LL1804;}
_LL1804: if( _temp1800 ==( void*) Cyc_Parse_Union_su){ goto _LL1805;} else{ goto
_LL1801;} _LL1803: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1806=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1806->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1806;}), _temp1799,({ struct Cyc_Core_Opt*
_temp1807=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1807->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1807;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1801; _LL1805: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1808=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1808->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1808;}),
_temp1799,({ struct Cyc_Core_Opt* _temp1809=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1809->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1809;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1801; _LL1801:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1810=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1810[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1811; _temp1811.tag= Cyc_TypeSpecifier_tok;
_temp1811.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1812=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1812[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1813; _temp1813.tag= Cyc_Parse_Decl_spec;
_temp1813.f1= d; _temp1813;}); _temp1812;})); _temp1811;}); _temp1810;}); break;}
case 83: _LL1798: { struct Cyc_List_List* _temp1815=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1816= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1818: if(
_temp1816 ==( void*) Cyc_Parse_Struct_su){ goto _LL1819;} else{ goto _LL1820;}
_LL1820: if( _temp1816 ==( void*) Cyc_Parse_Union_su){ goto _LL1821;} else{ goto
_LL1817;} _LL1819: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1822=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1822->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1822;}), _temp1815,({ struct Cyc_Core_Opt*
_temp1823=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1823->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1823;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1817; _LL1821: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1824=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1824->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1824;}),
_temp1815,({ struct Cyc_Core_Opt* _temp1825=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1825->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1825;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1817; _LL1817:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1826=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1826[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1827; _temp1827.tag= Cyc_TypeSpecifier_tok;
_temp1827.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1828=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1828[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1829; _temp1829.tag= Cyc_Parse_Decl_spec;
_temp1829.f1= d; _temp1829;}); _temp1828;})); _temp1827;}); _temp1826;}); break;}
case 84: _LL1814:{ void* _temp1831= Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1833: if(
_temp1831 ==( void*) Cyc_Parse_Struct_su){ goto _LL1834;} else{ goto _LL1835;}
_LL1835: if( _temp1831 ==( void*) Cyc_Parse_Union_su){ goto _LL1836;} else{ goto
_LL1832;} _LL1834: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1837=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1837[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1838; _temp1838.tag=
Cyc_TypeSpecifier_tok; _temp1838.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1839=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1839[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1840; _temp1840.tag= Cyc_Absyn_StructType;
_temp1840.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1840.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1840.f3= 0;
_temp1840;}); _temp1839;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1838;});
_temp1837;}); goto _LL1832; _LL1836: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1841=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1841[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1842; _temp1842.tag=
Cyc_TypeSpecifier_tok; _temp1842.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1843=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1843[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1844; _temp1844.tag= Cyc_Absyn_UnionType;
_temp1844.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1844.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1844.f3= 0;
_temp1844;}); _temp1843;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1842;});
_temp1841;}); goto _LL1832; _LL1832:;} break; case 85: _LL1830:{ void* _temp1846=
Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL1848: if( _temp1846 ==( void*) Cyc_Parse_Struct_su){
goto _LL1849;} else{ goto _LL1850;} _LL1850: if( _temp1846 ==( void*) Cyc_Parse_Union_su){
goto _LL1851;} else{ goto _LL1847;} _LL1849: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1852=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1852[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1853; _temp1853.tag=
Cyc_TypeSpecifier_tok; _temp1853.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1854=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1854[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1855; _temp1855.tag= Cyc_Absyn_StructType;
_temp1855.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1855.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1855.f3= 0;
_temp1855;}); _temp1854;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1853;});
_temp1852;}); goto _LL1847; _LL1851: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1856=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1856[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1857; _temp1857.tag=
Cyc_TypeSpecifier_tok; _temp1857.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1858=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1858[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1859; _temp1859.tag= Cyc_Absyn_UnionType;
_temp1859.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1859.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1859.f3= 0;
_temp1859;}); _temp1858;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1857;});
_temp1856;}); goto _LL1847; _LL1847:;} break; case 86: _LL1845: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1861=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1861[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1862; _temp1862.tag= Cyc_TypeList_tok; _temp1862.f1= 0; _temp1862;});
_temp1861;}); break; case 87: _LL1860: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1864=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1864[ 0]=({ struct Cyc_TypeList_tok_struct _temp1865; _temp1865.tag= Cyc_TypeList_tok;
_temp1865.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1865;});
_temp1864;}); break; case 88: _LL1863:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1867=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1867[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1868; _temp1868.tag= Cyc_TypeList_tok; _temp1868.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1868;});
_temp1867;}); break; case 89: _LL1866: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1870=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1870[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1871; _temp1871.tag=
Cyc_StructOrUnion_tok; _temp1871.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp1871;}); _temp1870;}); break; case 90: _LL1869: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1873=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1873[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1874; _temp1874.tag=
Cyc_StructOrUnion_tok; _temp1874.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp1874;}); _temp1873;}); break; case 91: _LL1872: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1876=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1876[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1877; _temp1877.tag= Cyc_StructFieldDeclList_tok; _temp1877.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1877;});
_temp1876;}); break; case 92: _LL1875: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1879=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1879[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1880; _temp1880.tag= Cyc_StructFieldDeclListList_tok; _temp1880.f1=({
struct Cyc_List_List* _temp1881=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1881->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1881->tl= 0;
_temp1881;}); _temp1880;}); _temp1879;}); break; case 93: _LL1878: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1883=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1883[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1884; _temp1884.tag= Cyc_StructFieldDeclListList_tok;
_temp1884.f1=({ struct Cyc_List_List* _temp1885=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1885->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1885->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1885;}); _temp1884;}); _temp1883;}); break;
case 94: _LL1882: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1887=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1887[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1888; _temp1888.tag=
Cyc_InitDeclList_tok; _temp1888.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1888;});
_temp1887;}); break; case 95: _LL1886: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1890=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1890[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1891; _temp1891.tag=
Cyc_InitDeclList_tok; _temp1891.f1=({ struct Cyc_List_List* _temp1892=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1892->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1892->tl= 0; _temp1892;}); _temp1891;}); _temp1890;});
break; case 96: _LL1889: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1894=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1894[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1895; _temp1895.tag=
Cyc_InitDeclList_tok; _temp1895.f1=({ struct Cyc_List_List* _temp1896=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1896->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1896->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1896;});
_temp1895;}); _temp1894;}); break; case 97: _LL1893: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1898=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1898[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1899; _temp1899.tag= Cyc_InitDecl_tok;
_temp1899.f1=({ struct _tuple15* _temp1900=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1900->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1900->f2= 0;
_temp1900;}); _temp1899;}); _temp1898;}); break; case 98: _LL1897: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1902=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1902[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1903; _temp1903.tag= Cyc_InitDecl_tok; _temp1903.f1=({ struct _tuple15*
_temp1904=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1904->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1904->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1904;});
_temp1903;}); _temp1902;}); break; case 99: _LL1901: { struct Cyc_Absyn_Tqual tq=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])).f1; struct Cyc_List_List* _temp1906=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3; void*
_temp1907= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
struct Cyc_List_List* _temp1910; struct Cyc_List_List* _temp1912; struct _tuple0
_temp1908=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _LL1913:
_temp1912= _temp1908.f1; goto _LL1911; _LL1911: _temp1910= _temp1908.f2; goto
_LL1909; _LL1909: { struct Cyc_List_List* _temp1914=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp1907, _temp1912, _temp1906), _temp1910); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1915=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1915[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1916; _temp1916.tag= Cyc_StructFieldDeclList_tok; _temp1916.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),
_temp1914); _temp1916;}); _temp1915;}); break;}} case 100: _LL1905: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1918=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1918[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1919; _temp1919.tag= Cyc_QualSpecList_tok;
_temp1919.f1=({ struct _tuple18* _temp1920=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1920->f1= Cyc_Absyn_empty_tqual(); _temp1920->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1921->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1921->tl=
0; _temp1921;}); _temp1920->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1920;}); _temp1919;});
_temp1918;}); break; case 101: _LL1917: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1923=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1923[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1924; _temp1924.tag=
Cyc_QualSpecList_tok; _temp1924.f1=({ struct _tuple18* _temp1925=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1925->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1925->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1926=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1926->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1926->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1926;}); _temp1925->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1925;});
_temp1924;}); _temp1923;}); break; case 102: _LL1922: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1928=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1928[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1929; _temp1929.tag= Cyc_QualSpecList_tok;
_temp1929.f1=({ struct _tuple18* _temp1930=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1930->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1930->f2= 0;
_temp1930->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1930;}); _temp1929;}); _temp1928;}); break; case
103: _LL1927: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1932=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1932[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1933; _temp1933.tag=
Cyc_QualSpecList_tok; _temp1933.f1=({ struct _tuple18* _temp1934=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1934->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1934->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1934->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1934;});
_temp1933;}); _temp1932;}); break; case 104: _LL1931: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1936=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1936[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1937; _temp1937.tag= Cyc_DeclaratorExpoptList_tok;
_temp1937.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1937;}); _temp1936;}); break; case 105: _LL1935: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1939=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1939[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1940; _temp1940.tag= Cyc_DeclaratorExpoptList_tok;
_temp1940.f1=({ struct Cyc_List_List* _temp1941=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1941->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1941->tl= 0;
_temp1941;}); _temp1940;}); _temp1939;}); break; case 106: _LL1938: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp1943=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1943[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1944; _temp1944.tag= Cyc_DeclaratorExpoptList_tok;
_temp1944.f1=({ struct Cyc_List_List* _temp1945=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1945->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1945->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1945;}); _temp1944;}); _temp1943;}); break; case 107:
_LL1942: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp1947=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp1947[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp1948; _temp1948.tag=
Cyc_DeclaratorExpopt_tok; _temp1948.f1=({ struct _tuple13* _temp1949=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1949->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1949->f2= 0;
_temp1949;}); _temp1948;}); _temp1947;}); break; case 108: _LL1946: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1951=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1951[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1952; _temp1952.tag= Cyc_DeclaratorExpopt_tok;
_temp1952.f1=({ struct _tuple13* _temp1953=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1953->f1=({ struct Cyc_Parse_Declarator*
_temp1955=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp1955->id=({ struct _tuple1* _temp1956=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp1956->f1= Cyc_Absyn_rel_ns_null; _temp1956->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1956;}); _temp1955->tms= 0; _temp1955;});
_temp1953->f2=({ struct Cyc_Core_Opt* _temp1954=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1954->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1954;});
_temp1953;}); _temp1952;}); _temp1951;}); break; case 109: _LL1950: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1958=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1958[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1959; _temp1959.tag= Cyc_DeclaratorExpopt_tok;
_temp1959.f1=({ struct _tuple13* _temp1960=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1960->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1960->f2=({
struct Cyc_Core_Opt* _temp1961=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1961->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1961;}); _temp1960;});
_temp1959;}); _temp1958;}); break; case 110: _LL1957: { struct Cyc_List_List*
_temp1963=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1964=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1964[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1965; _temp1965.tag= Cyc_TypeSpecifier_tok;
_temp1965.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1966=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1966[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1967; _temp1967.tag= Cyc_Parse_Decl_spec;
_temp1967.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp1963,({
struct Cyc_Core_Opt* _temp1968=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1968->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1968;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1967;}); _temp1966;})); _temp1965;}); _temp1964;}); break;} case 111:
_LL1962: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1970=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1970[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1971; _temp1971.tag=
Cyc_TypeSpecifier_tok; _temp1971.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1972=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1972[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1973; _temp1973.tag= Cyc_Absyn_TunionType;
_temp1973.f1=({ struct Cyc_Absyn_TunionInfo _temp1974; _temp1974.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1975=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1975[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1976; _temp1976.tag= Cyc_Absyn_UnknownTunion;
_temp1976.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1977; _temp1977.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1977.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1977;});
_temp1976;}); _temp1975;})); _temp1974.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1974.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp1974;}); _temp1973;}); _temp1972;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1971;}); _temp1970;}); break; case 112: _LL1969: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1979=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1979[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1980; _temp1980.tag= Cyc_TypeSpecifier_tok;
_temp1980.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1981=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1981[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1982; _temp1982.tag=
Cyc_Absyn_TunionType; _temp1982.f1=({ struct Cyc_Absyn_TunionInfo _temp1983;
_temp1983.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1984=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1984[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1985; _temp1985.tag= Cyc_Absyn_UnknownTunion; _temp1985.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1986; _temp1986.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1986.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp1986;}); _temp1985;}); _temp1984;})); _temp1983.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1983.rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1983;}); _temp1982;}); _temp1981;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1980;}); _temp1979;}); break; case 113: _LL1978: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1988=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1988[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1989; _temp1989.tag= Cyc_TypeSpecifier_tok;
_temp1989.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1990=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1990[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1991; _temp1991.tag= Cyc_Absyn_TunionFieldType; _temp1991.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1992; _temp1992.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1993=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1993[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1994; _temp1994.tag= Cyc_Absyn_UnknownTunionfield; _temp1994.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1995; _temp1995.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1995.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1995.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1995;});
_temp1994;}); _temp1993;})); _temp1992.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1992;}); _temp1991;});
_temp1990;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1989;}); _temp1988;}); break; case 114:
_LL1987: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1997=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1997[ 0]=({ struct
Cyc_Bool_tok_struct _temp1998; _temp1998.tag= Cyc_Bool_tok; _temp1998.f1= 0;
_temp1998;}); _temp1997;}); break; case 115: _LL1996: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2000=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2000[ 0]=({ struct Cyc_Bool_tok_struct
_temp2001; _temp2001.tag= Cyc_Bool_tok; _temp2001.f1= 1; _temp2001;}); _temp2000;});
break; case 116: _LL1999: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2003=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2003[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2004; _temp2004.tag= Cyc_TunionFieldList_tok; _temp2004.f1=({ struct Cyc_List_List*
_temp2005=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2005->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2005->tl= 0;
_temp2005;}); _temp2004;}); _temp2003;}); break; case 117: _LL2002: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2007=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2007[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2008; _temp2008.tag= Cyc_TunionFieldList_tok;
_temp2008.f1=({ struct Cyc_List_List* _temp2009=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2009->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2009->tl=
0; _temp2009;}); _temp2008;}); _temp2007;}); break; case 118: _LL2006: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2011=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2011[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2012; _temp2012.tag= Cyc_TunionFieldList_tok;
_temp2012.f1=({ struct Cyc_List_List* _temp2013=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2013->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2013->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2013;}); _temp2012;}); _temp2011;}); break; case 119:
_LL2010: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2015=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2015[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2016; _temp2016.tag=
Cyc_TunionFieldList_tok; _temp2016.f1=({ struct Cyc_List_List* _temp2017=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2017->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2017->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2017;}); _temp2016;});
_temp2015;}); break; case 120: _LL2014: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2019=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2019[ 0]=({ struct Cyc_Scope_tok_struct _temp2020; _temp2020.tag= Cyc_Scope_tok;
_temp2020.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2020;}); _temp2019;});
break; case 121: _LL2018: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2022=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2022[ 0]=({ struct Cyc_Scope_tok_struct _temp2023; _temp2023.tag= Cyc_Scope_tok;
_temp2023.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2023;}); _temp2022;});
break; case 122: _LL2021: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2025=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2025[ 0]=({ struct Cyc_Scope_tok_struct _temp2026; _temp2026.tag= Cyc_Scope_tok;
_temp2026.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2026;}); _temp2025;});
break; case 123: _LL2024: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2028=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2028[ 0]=({ struct Cyc_TunionField_tok_struct _temp2029; _temp2029.tag= Cyc_TunionField_tok;
_temp2029.f1=({ struct Cyc_Absyn_Tunionfield* _temp2030=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2030->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2030->tvs= 0;
_temp2030->typs= 0; _temp2030->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2030->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2030;}); _temp2029;}); _temp2028;}); break; case 124:
_LL2027: { struct Cyc_List_List* _temp2032=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2033=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2034=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2034[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2035; _temp2035.tag= Cyc_TunionField_tok;
_temp2035.f1=({ struct Cyc_Absyn_Tunionfield* _temp2036=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2036->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2036->tvs=
_temp2033; _temp2036->typs= _temp2032; _temp2036->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2036->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2036;}); _temp2035;}); _temp2034;}); break;}
case 125: _LL2031: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 126: _LL2037: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2039=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2039[ 0]=({ struct Cyc_Declarator_tok_struct _temp2040; _temp2040.tag= Cyc_Declarator_tok;
_temp2040.f1=({ struct Cyc_Parse_Declarator* _temp2041=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2041->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2041->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2041;});
_temp2040;}); _temp2039;}); break; case 127: _LL2038: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2043=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2043[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2044; _temp2044.tag= Cyc_Declarator_tok;
_temp2044.f1=({ struct Cyc_Parse_Declarator* _temp2045=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2045->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2045->tms= 0;
_temp2045;}); _temp2044;}); _temp2043;}); break; case 128: _LL2042: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 129:
_LL2046: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2048=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2048[ 0]=({ struct Cyc_Declarator_tok_struct _temp2049; _temp2049.tag= Cyc_Declarator_tok;
_temp2049.f1=({ struct Cyc_Parse_Declarator* _temp2050=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2050->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2050->tms=({ struct Cyc_List_List* _temp2051=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2051->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2051->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2051;}); _temp2050;}); _temp2049;});
_temp2048;}); break; case 130: _LL2047: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2053=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2053[ 0]=({ struct Cyc_Declarator_tok_struct _temp2054; _temp2054.tag= Cyc_Declarator_tok;
_temp2054.f1=({ struct Cyc_Parse_Declarator* _temp2055=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2055->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2055->tms=({ struct Cyc_List_List* _temp2056=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2056->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2057=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2057[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2058; _temp2058.tag= Cyc_Absyn_ConstArray_mod;
_temp2058.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2058;}); _temp2057;})); _temp2056->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2056;}); _temp2055;}); _temp2054;}); _temp2053;}); break; case 131: _LL2052: {
struct _tuple17 _temp2062; struct Cyc_List_List* _temp2063; struct Cyc_Core_Opt*
_temp2065; struct Cyc_Absyn_VarargInfo* _temp2067; int _temp2069; struct Cyc_List_List*
_temp2071; struct _tuple17* _temp2060= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2062=*
_temp2060; _LL2072: _temp2071= _temp2062.f1; goto _LL2070; _LL2070: _temp2069=
_temp2062.f2; goto _LL2068; _LL2068: _temp2067= _temp2062.f3; goto _LL2066;
_LL2066: _temp2065= _temp2062.f4; goto _LL2064; _LL2064: _temp2063= _temp2062.f5;
goto _LL2061; _LL2061: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2073=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2073[ 0]=({ struct Cyc_Declarator_tok_struct _temp2074; _temp2074.tag= Cyc_Declarator_tok;
_temp2074.f1=({ struct Cyc_Parse_Declarator* _temp2075=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2075->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2075->tms=({ struct Cyc_List_List* _temp2076=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2076->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2077=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2077[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2078; _temp2078.tag= Cyc_Absyn_Function_mod;
_temp2078.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2079=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2079[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2080; _temp2080.tag= Cyc_Absyn_WithTypes;
_temp2080.f1= _temp2071; _temp2080.f2= _temp2069; _temp2080.f3= _temp2067;
_temp2080.f4= _temp2065; _temp2080.f5= _temp2063; _temp2080;}); _temp2079;}));
_temp2078;}); _temp2077;})); _temp2076->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2076;});
_temp2075;}); _temp2074;}); _temp2073;}); break;} case 132: _LL2059: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2082=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2082[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2083; _temp2083.tag= Cyc_Declarator_tok;
_temp2083.f1=({ struct Cyc_Parse_Declarator* _temp2084=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2084->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2084->tms=({ struct Cyc_List_List* _temp2085=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2085->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2086=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2086[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2087; _temp2087.tag= Cyc_Absyn_Function_mod;
_temp2087.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2088=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2088[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2089; _temp2089.tag= Cyc_Absyn_WithTypes;
_temp2089.f1= 0; _temp2089.f2= 0; _temp2089.f3= 0; _temp2089.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2089.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2089;}); _temp2088;})); _temp2087;}); _temp2086;}));
_temp2085->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2085;}); _temp2084;}); _temp2083;});
_temp2082;}); break; case 133: _LL2081: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2091=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2091[ 0]=({ struct Cyc_Declarator_tok_struct _temp2092; _temp2092.tag= Cyc_Declarator_tok;
_temp2092.f1=({ struct Cyc_Parse_Declarator* _temp2093=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2093->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2093->tms=({ struct Cyc_List_List* _temp2094=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2094->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2095=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2095[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2096; _temp2096.tag= Cyc_Absyn_Function_mod;
_temp2096.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2097=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2097[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2098; _temp2098.tag= Cyc_Absyn_NoTypes;
_temp2098.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2098.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2098;});
_temp2097;})); _temp2096;}); _temp2095;})); _temp2094->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2094;}); _temp2093;}); _temp2092;}); _temp2091;}); break; case 134: _LL2090: {
struct Cyc_List_List* _temp2100=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2101=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2101[ 0]=({ struct Cyc_Declarator_tok_struct _temp2102; _temp2102.tag= Cyc_Declarator_tok;
_temp2102.f1=({ struct Cyc_Parse_Declarator* _temp2103=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2103->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2103->tms=({ struct Cyc_List_List* _temp2104=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2104->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2105=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2105[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2106; _temp2106.tag= Cyc_Absyn_TypeParams_mod;
_temp2106.f1= _temp2100; _temp2106.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2106.f3=
0; _temp2106;}); _temp2105;})); _temp2104->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2104;});
_temp2103;}); _temp2102;}); _temp2101;}); break;} case 135: _LL2099:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2108=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2109=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2109[ 0]=({ struct Cyc_Declarator_tok_struct _temp2110; _temp2110.tag= Cyc_Declarator_tok;
_temp2110.f1=({ struct Cyc_Parse_Declarator* _temp2111=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2111->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2111->tms=({ struct Cyc_List_List* _temp2112=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2112->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2113=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2113[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2114; _temp2114.tag= Cyc_Absyn_TypeParams_mod;
_temp2114.f1= _temp2108; _temp2114.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2114.f3=
0; _temp2114;}); _temp2113;})); _temp2112->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2112;});
_temp2111;}); _temp2110;}); _temp2109;}); break;} case 136: _LL2107: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2116=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2116[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2117; _temp2117.tag= Cyc_Declarator_tok;
_temp2117.f1=({ struct Cyc_Parse_Declarator* _temp2118=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2118->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2118->tms=({ struct Cyc_List_List* _temp2119=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2119->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2120=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2120[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2121; _temp2121.tag= Cyc_Absyn_Attributes_mod;
_temp2121.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2121.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2121;}); _temp2120;}));
_temp2119->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2119;}); _temp2118;}); _temp2117;});
_temp2116;}); break; case 137: _LL2115: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2123=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2123[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2124; _temp2124.tag= Cyc_Declarator_tok;
_temp2124.f1=({ struct Cyc_Parse_Declarator* _temp2125=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2125->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2125->tms= 0;
_temp2125;}); _temp2124;}); _temp2123;}); break; case 138: _LL2122: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2127=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2127[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2128; _temp2128.tag= Cyc_Declarator_tok;
_temp2128.f1=({ struct Cyc_Parse_Declarator* _temp2129=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2129->id= Cyc_Absyn_exn_name;
_temp2129->tms= 0; _temp2129;}); _temp2128;}); _temp2127;}); break; case 139:
_LL2126: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2131=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2131[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2132; _temp2132.tag=
Cyc_TypeModifierList_tok; _temp2132.f1=({ struct Cyc_List_List* _temp2133=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2133->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2134=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2134[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2135; _temp2135.tag= Cyc_Absyn_Pointer_mod;
_temp2135.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2135.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2135.f3= Cyc_Absyn_empty_tqual(); _temp2135;}); _temp2134;}));
_temp2133->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2133;});
_temp2132;}); _temp2131;}); break; case 140: _LL2130: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2137=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2137[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2138; _temp2138.tag= Cyc_TypeModifierList_tok;
_temp2138.f1=({ struct Cyc_List_List* _temp2139=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2139->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2140=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2140[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2141; _temp2141.tag= Cyc_Absyn_Pointer_mod;
_temp2141.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2141.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2141.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2141;}); _temp2140;})); _temp2139->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2139;});
_temp2138;}); _temp2137;}); break; case 141: _LL2136: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2143=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2143[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2144; _temp2144.tag= Cyc_TypeModifierList_tok;
_temp2144.f1=({ struct Cyc_List_List* _temp2145=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2145->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2146=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2146[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2147; _temp2147.tag= Cyc_Absyn_Pointer_mod;
_temp2147.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2147.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2147.f3= Cyc_Absyn_empty_tqual(); _temp2147;}); _temp2146;}));
_temp2145->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2145;});
_temp2144;}); _temp2143;}); break; case 142: _LL2142: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2149=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2149[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2150; _temp2150.tag= Cyc_TypeModifierList_tok;
_temp2150.f1=({ struct Cyc_List_List* _temp2151=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2151->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2152=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2152[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2153; _temp2153.tag= Cyc_Absyn_Pointer_mod;
_temp2153.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2153.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2153.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2153;}); _temp2152;})); _temp2151->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2151;});
_temp2150;}); _temp2149;}); break; case 143: _LL2148: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2155=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2155[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2156; _temp2156.tag= Cyc_Pointer_Sort_tok;
_temp2156.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2157=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2157[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2158; _temp2158.tag=
Cyc_Absyn_Nullable_ps; _temp2158.f1= Cyc_Absyn_exp_unsigned_one; _temp2158;});
_temp2157;})); _temp2156;}); _temp2155;}); break; case 144: _LL2154: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2160=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2160[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2161; _temp2161.tag= Cyc_Pointer_Sort_tok;
_temp2161.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2162=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2162[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2163; _temp2163.tag= Cyc_Absyn_NonNullable_ps; _temp2163.f1= Cyc_Absyn_exp_unsigned_one;
_temp2163;}); _temp2162;})); _temp2161;}); _temp2160;}); break; case 145:
_LL2159: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2165=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2165[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2166; _temp2166.tag=
Cyc_Pointer_Sort_tok; _temp2166.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2167=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2167[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2168; _temp2168.tag=
Cyc_Absyn_Nullable_ps; _temp2168.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2168;});
_temp2167;})); _temp2166;}); _temp2165;}); break; case 146: _LL2164: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2170=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2170[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2171; _temp2171.tag= Cyc_Pointer_Sort_tok;
_temp2171.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2172=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2172[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2173; _temp2173.tag= Cyc_Absyn_NonNullable_ps; _temp2173.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2173;});
_temp2172;})); _temp2171;}); _temp2170;}); break; case 147: _LL2169: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2175=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2175[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2176; _temp2176.tag= Cyc_Pointer_Sort_tok;
_temp2176.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2176;});
_temp2175;}); break; case 148: _LL2174: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2178=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2178[ 0]=({ struct Cyc_Rgn_tok_struct _temp2179; _temp2179.tag= Cyc_Rgn_tok;
_temp2179.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2179;}); _temp2178;});
break; case 149: _LL2177: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2180: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2182=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2182[ 0]=({ struct Cyc_Rgn_tok_struct _temp2183; _temp2183.tag= Cyc_Rgn_tok;
_temp2183.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2183;}); _temp2182;}); break; case 151: _LL2181:
if( Cyc_yyget_Kind_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind\n",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Rgn_tok_struct* _temp2185=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp2185[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2186; _temp2186.tag= Cyc_Rgn_tok; _temp2186.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind)); _temp2186;});
_temp2185;}); break; case 152: _LL2184: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2188=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2188[ 0]=({ struct Cyc_Rgn_tok_struct _temp2189; _temp2189.tag= Cyc_Rgn_tok;
_temp2189.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2190=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2190->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2190;}), 0); _temp2189;}); _temp2188;});
break; case 153: _LL2187: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 154: _LL2191: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2193=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2193[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2194; _temp2194.tag= Cyc_TypeQual_tok; _temp2194.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2194;});
_temp2193;}); break; case 155: _LL2192: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2196=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2196[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2197; _temp2197.tag= Cyc_ParamDeclListBool_tok; _temp2197.f1=({ struct
_tuple17* _temp2198=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2198->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2198->f2= 0; _temp2198->f3= 0; _temp2198->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2198->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2198;}); _temp2197;}); _temp2196;}); break; case 156:
_LL2195: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2200=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2200[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2201; _temp2201.tag=
Cyc_ParamDeclListBool_tok; _temp2201.f1=({ struct _tuple17* _temp2202=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2202->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2202->f2= 1;
_temp2202->f3= 0; _temp2202->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2202->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2202;});
_temp2201;}); _temp2200;}); break; case 157: _LL2199: { void* _temp2206; struct
Cyc_Absyn_Tqual _temp2208; struct Cyc_Core_Opt* _temp2210; struct _tuple2
_temp2204=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL2211: _temp2210= _temp2204.f1; goto _LL2209; _LL2209:
_temp2208= _temp2204.f2; goto _LL2207; _LL2207: _temp2206= _temp2204.f3; goto
_LL2205; _LL2205: { struct Cyc_Absyn_VarargInfo* _temp2212=({ struct Cyc_Absyn_VarargInfo*
_temp2216=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2216->name= _temp2210; _temp2216->tq= _temp2208; _temp2216->type=( void*)
_temp2206; _temp2216->rgn=( void*) Cyc_yyget_Rgn_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2216->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2216;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2213=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2213[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2214; _temp2214.tag= Cyc_ParamDeclListBool_tok; _temp2214.f1=({ struct
_tuple17* _temp2215=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2215->f1= 0; _temp2215->f2= 0; _temp2215->f3= _temp2212; _temp2215->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2215->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2215;}); _temp2214;}); _temp2213;}); break;}} case 158:
_LL2203: { void* _temp2220; struct Cyc_Absyn_Tqual _temp2222; struct Cyc_Core_Opt*
_temp2224; struct _tuple2 _temp2218=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL2225: _temp2224=
_temp2218.f1; goto _LL2223; _LL2223: _temp2222= _temp2218.f2; goto _LL2221;
_LL2221: _temp2220= _temp2218.f3; goto _LL2219; _LL2219: { struct Cyc_Absyn_VarargInfo*
_temp2226=({ struct Cyc_Absyn_VarargInfo* _temp2230=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2230->name= _temp2224;
_temp2230->tq= _temp2222; _temp2230->type=( void*) _temp2220; _temp2230->rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2230->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2230;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2227=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2227[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2228; _temp2228.tag= Cyc_ParamDeclListBool_tok;
_temp2228.f1=({ struct _tuple17* _temp2229=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2229->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)])); _temp2229->f2= 0;
_temp2229->f3= _temp2226; _temp2229->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2229->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2229;});
_temp2228;}); _temp2227;}); break;}} case 159: _LL2217: yyval=( void*)({ struct
Cyc_TypeOpt_tok_struct* _temp2232=( struct Cyc_TypeOpt_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2232[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2233; _temp2233.tag= Cyc_TypeOpt_tok; _temp2233.f1= 0; _temp2233;});
_temp2232;}); break; case 160: _LL2231: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2235=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2235[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2236; _temp2236.tag= Cyc_TypeOpt_tok;
_temp2236.f1=({ struct Cyc_Core_Opt* _temp2237=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2237->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2238=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2238[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2239; _temp2239.tag= Cyc_Absyn_JoinEff;
_temp2239.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2239;}); _temp2238;})); _temp2237;}); _temp2236;});
_temp2235;}); break; case 161: _LL2234: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2241=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2241[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2242; _temp2242.tag= Cyc_Rgnorder_tok;
_temp2242.f1= 0; _temp2242;}); _temp2241;}); break; case 162: _LL2240: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2243: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2245=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2245[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2246; _temp2246.tag= Cyc_Rgnorder_tok; _temp2246.f1=({ struct Cyc_List_List*
_temp2247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2247->hd=( void*)({ struct _tuple19* _temp2248=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2248->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2248->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2248;}); _temp2247->tl= 0; _temp2247;});
_temp2246;}); _temp2245;}); break; case 164: _LL2244: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2250=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2250[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2251; _temp2251.tag= Cyc_Rgnorder_tok; _temp2251.f1=({ struct Cyc_List_List*
_temp2252=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2252->hd=( void*)({ struct _tuple19* _temp2253=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2253->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2253->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2253;}); _temp2252->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2252;});
_temp2251;}); _temp2250;}); break; case 165: _LL2249: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2255=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2255[ 0]=({ struct Cyc_Bool_tok_struct
_temp2256; _temp2256.tag= Cyc_Bool_tok; _temp2256.f1= 0; _temp2256;}); _temp2255;});
break; case 166: _LL2254: { struct _tagged_arr _temp2258= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2258, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2259=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2259[ 0]=({ struct Cyc_Bool_tok_struct _temp2260; _temp2260.tag= Cyc_Bool_tok;
_temp2260.f1= 1; _temp2260;}); _temp2259;}); break;} case 167: _LL2257: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2261: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2263=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2263[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2264; _temp2264.tag= Cyc_TypeList_tok; _temp2264.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2264;});
_temp2263;}); break; case 169: _LL2262: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2266=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2266[ 0]=({ struct Cyc_TypeList_tok_struct _temp2267; _temp2267.tag= Cyc_TypeList_tok;
_temp2267.f1= 0; _temp2267;}); _temp2266;}); break; case 170: _LL2265: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
171: _LL2268: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2270=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2270[ 0]=({ struct Cyc_TypeList_tok_struct _temp2271; _temp2271.tag= Cyc_TypeList_tok;
_temp2271.f1=({ struct Cyc_List_List* _temp2272=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2272->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2272->tl= 0; _temp2272;});
_temp2271;}); _temp2270;}); break; case 172: _LL2269: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_EffKind){
Cyc_Parse_err( _tag_arr("expecing effect kind (E)", sizeof( unsigned char), 25u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2274=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2274[ 0]=({ struct Cyc_TypeList_tok_struct _temp2275; _temp2275.tag= Cyc_TypeList_tok;
_temp2275.f1=({ struct Cyc_List_List* _temp2276=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2276->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2276->tl= 0;
_temp2276;}); _temp2275;}); _temp2274;}); break; case 173: _LL2273: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2278=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2278[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2279; _temp2279.tag= Cyc_TypeList_tok; _temp2279.f1=({ struct Cyc_List_List*
_temp2280=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2280->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2281=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2281[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2282; _temp2282.tag= Cyc_Absyn_AccessEff;
_temp2282.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2282;}); _temp2281;})); _temp2280->tl= 0;
_temp2280;}); _temp2279;}); _temp2278;}); break; case 174: _LL2277: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2284=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2284[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2285; _temp2285.tag= Cyc_TypeList_tok; _temp2285.f1=({ struct Cyc_List_List*
_temp2286=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2286->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2287=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2287[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2288; _temp2288.tag= Cyc_Absyn_AccessEff;
_temp2288.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2288;}); _temp2287;})); _temp2286->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2286;});
_temp2285;}); _temp2284;}); break; case 175: _LL2283: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2290=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2290[ 0]=({ struct Cyc_TypeList_tok_struct _temp2291; _temp2291.tag= Cyc_TypeList_tok;
_temp2291.f1=({ struct Cyc_List_List* _temp2292=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2292->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2293=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2293[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2294; _temp2294.tag= Cyc_Absyn_AccessEff;
_temp2294.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2294;}); _temp2293;})); _temp2292->tl= 0;
_temp2292;}); _temp2291;}); _temp2290;}); break; case 176: _LL2289: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2296=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2296[ 0]=({ struct Cyc_TypeList_tok_struct _temp2297; _temp2297.tag= Cyc_TypeList_tok;
_temp2297.f1=({ struct Cyc_List_List* _temp2298=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2298->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2299=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2299[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2300; _temp2300.tag= Cyc_Absyn_AccessEff;
_temp2300.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2300;}); _temp2299;})); _temp2298->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2298;});
_temp2297;}); _temp2296;}); break; case 177: _LL2295: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2302=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2302[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2303; _temp2303.tag= Cyc_ParamDeclList_tok;
_temp2303.f1=({ struct Cyc_List_List* _temp2304=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2304->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2304->tl= 0;
_temp2304;}); _temp2303;}); _temp2302;}); break; case 178: _LL2301: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2306=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2306[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2307; _temp2307.tag= Cyc_ParamDeclList_tok;
_temp2307.f1=({ struct Cyc_List_List* _temp2308=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2308->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2308->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2308;}); _temp2307;}); _temp2306;}); break; case 179:
_LL2305: { void* _temp2310= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2311=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2312=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2313=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2314= Cyc_Parse_apply_tms( _temp2312, _temp2310, _temp2311,
_temp2313); if( _temp2314.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
_tuple1* _temp2315=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id;{ void* _temp2316=(*
_temp2315).f1; struct Cyc_List_List* _temp2326; struct Cyc_List_List* _temp2328;
_LL2318: if( _temp2316 ==( void*) Cyc_Absyn_Loc_n){ goto _LL2319;} else{ goto
_LL2320;} _LL2320: if(( unsigned int) _temp2316 > 1u?*(( int*) _temp2316) == Cyc_Absyn_Rel_n:
0){ _LL2327: _temp2326=(( struct Cyc_Absyn_Rel_n_struct*) _temp2316)->f1; if(
_temp2326 == 0){ goto _LL2321;} else{ goto _LL2322;}} else{ goto _LL2322;}
_LL2322: if(( unsigned int) _temp2316 > 1u?*(( int*) _temp2316) == Cyc_Absyn_Abs_n:
0){ _LL2329: _temp2328=(( struct Cyc_Absyn_Abs_n_struct*) _temp2316)->f1; if(
_temp2328 == 0){ goto _LL2323;} else{ goto _LL2324;}} else{ goto _LL2324;}
_LL2324: goto _LL2325; _LL2319: goto _LL2317; _LL2321: goto _LL2317; _LL2323:
goto _LL2317; _LL2325: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); goto
_LL2317; _LL2317:;}{ struct Cyc_Core_Opt* _temp2330=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2334=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2334->v=( void*)(* _temp2315).f2; _temp2334;}); if(
_temp2314.f4 != 0){ Cyc_Parse_warn( _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2314.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2331=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2331[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2332; _temp2332.tag= Cyc_ParamDecl_tok; _temp2332.f1=({
struct _tuple2* _temp2333=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2333->f1= _temp2330; _temp2333->f2= _temp2314.f1; _temp2333->f3= _temp2314.f2;
_temp2333;}); _temp2332;}); _temp2331;}); break;}}} case 180: _LL2309: { void*
_temp2336= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2337=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; struct Cyc_Absyn_Tqual _temp2338=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; if( _temp2337
!= 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring", sizeof(
unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2339=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2339[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2340; _temp2340.tag= Cyc_ParamDecl_tok; _temp2340.f1=({
struct _tuple2* _temp2341=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2341->f1= 0; _temp2341->f2= _temp2338; _temp2341->f3= _temp2336; _temp2341;});
_temp2340;}); _temp2339;}); break;} case 181: _LL2335: { void* _temp2343= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2344=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2345=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2346=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2347= Cyc_Parse_apply_tms( _temp2345, _temp2343, _temp2344,
_temp2346); if( _temp2347.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2347.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2348=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2348[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2349; _temp2349.tag= Cyc_ParamDecl_tok; _temp2349.f1=({
struct _tuple2* _temp2350=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2350->f1= 0; _temp2350->f2= _temp2347.f1; _temp2350->f3= _temp2347.f2;
_temp2350;}); _temp2349;}); _temp2348;}); break;} case 182: _LL2342: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2352=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2352[ 0]=({ struct Cyc_IdList_tok_struct
_temp2353; _temp2353.tag= Cyc_IdList_tok; _temp2353.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2353;});
_temp2352;}); break; case 183: _LL2351: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2355=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2355[ 0]=({ struct Cyc_IdList_tok_struct _temp2356; _temp2356.tag= Cyc_IdList_tok;
_temp2356.f1=({ struct Cyc_List_List* _temp2357=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2357->hd=( void*)({ struct
_tagged_arr* _temp2358=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2358[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2358;}); _temp2357->tl=
0; _temp2357;}); _temp2356;}); _temp2355;}); break; case 184: _LL2354: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2360=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2360[ 0]=({ struct Cyc_IdList_tok_struct
_temp2361; _temp2361.tag= Cyc_IdList_tok; _temp2361.f1=({ struct Cyc_List_List*
_temp2362=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2362->hd=( void*)({ struct _tagged_arr* _temp2363=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2363[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2363;});
_temp2362->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2362;}); _temp2361;}); _temp2360;}); break; case 185:
_LL2359: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 186: _LL2364: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 187: _LL2365: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2367=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2367[ 0]=({ struct Cyc_Exp_tok_struct _temp2368; _temp2368.tag= Cyc_Exp_tok;
_temp2368.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2369=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2369[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2370; _temp2370.tag= Cyc_Absyn_UnresolvedMem_e; _temp2370.f1= 0; _temp2370.f2=
0; _temp2370;}); _temp2369;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2368;});
_temp2367;}); break; case 188: _LL2366: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2372=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2372[ 0]=({ struct Cyc_Exp_tok_struct _temp2373; _temp2373.tag= Cyc_Exp_tok;
_temp2373.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2374=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2374[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2375; _temp2375.tag= Cyc_Absyn_UnresolvedMem_e; _temp2375.f1= 0; _temp2375.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2375;});
_temp2374;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2373;}); _temp2372;}); break; case 189:
_LL2371: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2377=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2377[ 0]=({ struct Cyc_Exp_tok_struct
_temp2378; _temp2378.tag= Cyc_Exp_tok; _temp2378.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2379=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2379[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2380; _temp2380.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2380.f1= 0; _temp2380.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2380;});
_temp2379;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2378;}); _temp2377;}); break; case 190:
_LL2376: { struct Cyc_Absyn_Vardecl* _temp2382= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2388=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2388->f1=( void*) Cyc_Absyn_Loc_n; _temp2388->f2=({ struct _tagged_arr*
_temp2389=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp2389[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2389;}); _temp2388;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2382->tq=({ struct Cyc_Absyn_Tqual _temp2383; _temp2383.q_const= 1;
_temp2383.q_volatile= 0; _temp2383.q_restrict= 1; _temp2383;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2384=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2384[ 0]=({ struct Cyc_Exp_tok_struct
_temp2385; _temp2385.tag= Cyc_Exp_tok; _temp2385.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2386=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2386[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2387; _temp2387.tag= Cyc_Absyn_Comprehension_e;
_temp2387.f1= _temp2382; _temp2387.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2387.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2387;});
_temp2386;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2385;}); _temp2384;}); break;} case
191: _LL2381: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2391=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2391[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2392; _temp2392.tag=
Cyc_InitializerList_tok; _temp2392.f1=({ struct Cyc_List_List* _temp2393=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2393->hd=(
void*)({ struct _tuple20* _temp2394=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2394->f1= 0; _temp2394->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2394;}); _temp2393->tl=
0; _temp2393;}); _temp2392;}); _temp2391;}); break; case 192: _LL2390: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2396=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2396[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2397; _temp2397.tag= Cyc_InitializerList_tok;
_temp2397.f1=({ struct Cyc_List_List* _temp2398=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2398->hd=( void*)({ struct
_tuple20* _temp2399=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2399->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2399->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2399;});
_temp2398->tl= 0; _temp2398;}); _temp2397;}); _temp2396;}); break; case 193:
_LL2395: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2401=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2401[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2402; _temp2402.tag=
Cyc_InitializerList_tok; _temp2402.f1=({ struct Cyc_List_List* _temp2403=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2403->hd=(
void*)({ struct _tuple20* _temp2404=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2404->f1= 0; _temp2404->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2404;}); _temp2403->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2403;}); _temp2402;}); _temp2401;}); break; case 194:
_LL2400: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2406=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2406[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2407; _temp2407.tag=
Cyc_InitializerList_tok; _temp2407.f1=({ struct Cyc_List_List* _temp2408=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2408->hd=(
void*)({ struct _tuple20* _temp2409=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2409->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2409->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2409;});
_temp2408->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2408;});
_temp2407;}); _temp2406;}); break; case 195: _LL2405: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2411=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2411[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2412; _temp2412.tag= Cyc_DesignatorList_tok;
_temp2412.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2412;});
_temp2411;}); break; case 196: _LL2410: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2414=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2414[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2415; _temp2415.tag=
Cyc_DesignatorList_tok; _temp2415.f1=({ struct Cyc_List_List* _temp2416=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2416->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2416->tl= 0; _temp2416;}); _temp2415;}); _temp2414;});
break; case 197: _LL2413: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2418=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2418[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2419; _temp2419.tag=
Cyc_DesignatorList_tok; _temp2419.f1=({ struct Cyc_List_List* _temp2420=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2420->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2420->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2420;});
_temp2419;}); _temp2418;}); break; case 198: _LL2417: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2422=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2422[ 0]=({ struct
Cyc_Designator_tok_struct _temp2423; _temp2423.tag= Cyc_Designator_tok;
_temp2423.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2424=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2424[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2425; _temp2425.tag=
Cyc_Absyn_ArrayElement; _temp2425.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2425;});
_temp2424;})); _temp2423;}); _temp2422;}); break; case 199: _LL2421: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2427=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2427[ 0]=({ struct
Cyc_Designator_tok_struct _temp2428; _temp2428.tag= Cyc_Designator_tok;
_temp2428.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2429=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2429[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2430; _temp2430.tag= Cyc_Absyn_FieldName;
_temp2430.f1=({ struct _tagged_arr* _temp2431=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2431[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2431;}); _temp2430;});
_temp2429;})); _temp2428;}); _temp2427;}); break; case 200: _LL2426: { void*
_temp2433= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2434=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2434 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2435=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2436=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2436[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2437; _temp2437.tag= Cyc_ParamDecl_tok; _temp2437.f1=({
struct _tuple2* _temp2438=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp2438[ 0]=({ struct _tuple2 _temp2439; _temp2439.f1= 0; _temp2439.f2=
_temp2435; _temp2439.f3= _temp2433; _temp2439;}); _temp2438;}); _temp2437;});
_temp2436;}); break;}} case 201: _LL2432: { void* _temp2441= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2442=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2443=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2444=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2445= Cyc_Parse_apply_tms( _temp2443, _temp2441, _temp2442,
_temp2444); if( _temp2445.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2445.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2446=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2446[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2447; _temp2447.tag= Cyc_ParamDecl_tok; _temp2447.f1=({
struct _tuple2* _temp2448=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2448->f1= 0; _temp2448->f2= _temp2445.f1; _temp2448->f3= _temp2445.f2;
_temp2448;}); _temp2447;}); _temp2446;}); break;} case 202: _LL2440: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2450=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2450[ 0]=({ struct Cyc_Type_tok_struct
_temp2451; _temp2451.tag= Cyc_Type_tok; _temp2451.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2451;});
_temp2450;}); break; case 203: _LL2449: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2453=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2453[ 0]=({ struct Cyc_Type_tok_struct _temp2454; _temp2454.tag= Cyc_Type_tok;
_temp2454.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2455=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2455[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2456; _temp2456.tag= Cyc_Absyn_JoinEff;
_temp2456.f1= 0; _temp2456;}); _temp2455;})); _temp2454;}); _temp2453;}); break;
case 204: _LL2452: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2458=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2458[ 0]=({ struct Cyc_Type_tok_struct _temp2459; _temp2459.tag= Cyc_Type_tok;
_temp2459.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2460=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2460[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2461; _temp2461.tag= Cyc_Absyn_JoinEff;
_temp2461.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2461;}); _temp2460;})); _temp2459;});
_temp2458;}); break; case 205: _LL2457: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2463=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2463[ 0]=({ struct Cyc_Type_tok_struct _temp2464; _temp2464.tag= Cyc_Type_tok;
_temp2464.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2465=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2465[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2466; _temp2466.tag= Cyc_Absyn_JoinEff;
_temp2466.f1=({ struct Cyc_List_List* _temp2467=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2467->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2467->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2467;}); _temp2466;}); _temp2465;})); _temp2464;});
_temp2463;}); break; case 206: _LL2462: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2469=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2469[ 0]=({ struct Cyc_TypeList_tok_struct _temp2470; _temp2470.tag= Cyc_TypeList_tok;
_temp2470.f1=({ struct Cyc_List_List* _temp2471=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2471->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2471->tl= 0;
_temp2471;}); _temp2470;}); _temp2469;}); break; case 207: _LL2468: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2473=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2473[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2474; _temp2474.tag= Cyc_TypeList_tok; _temp2474.f1=({ struct Cyc_List_List*
_temp2475=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2475->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2475->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2475;});
_temp2474;}); _temp2473;}); break; case 208: _LL2472: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2477=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2477[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2478; _temp2478.tag= Cyc_AbstractDeclarator_tok;
_temp2478.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2479=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2479->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2479;});
_temp2478;}); _temp2477;}); break; case 209: _LL2476: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 210: _LL2480:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2482=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2482[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2483; _temp2483.tag=
Cyc_AbstractDeclarator_tok; _temp2483.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2484=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2484->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2484;});
_temp2483;}); _temp2482;}); break; case 211: _LL2481: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 212:
_LL2485: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2487=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2487[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2488; _temp2488.tag=
Cyc_AbstractDeclarator_tok; _temp2488.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2489=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2489->tms=({ struct Cyc_List_List* _temp2490=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2490->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2490->tl= 0; _temp2490;}); _temp2489;}); _temp2488;}); _temp2487;}); break;
case 213: _LL2486: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2492=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2492[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2493; _temp2493.tag= Cyc_AbstractDeclarator_tok; _temp2493.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2494=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2494->tms=({ struct Cyc_List_List* _temp2495=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2495->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2495->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2495;});
_temp2494;}); _temp2493;}); _temp2492;}); break; case 214: _LL2491: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2497=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2497[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2498; _temp2498.tag= Cyc_AbstractDeclarator_tok;
_temp2498.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2499=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2499->tms=({
struct Cyc_List_List* _temp2500=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2500->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2501=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2501[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2502; _temp2502.tag= Cyc_Absyn_ConstArray_mod; _temp2502.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2502;});
_temp2501;})); _temp2500->tl= 0; _temp2500;}); _temp2499;}); _temp2498;});
_temp2497;}); break; case 215: _LL2496: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2504=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2504[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2505; _temp2505.tag= Cyc_AbstractDeclarator_tok; _temp2505.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2506=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2506->tms=({ struct Cyc_List_List* _temp2507=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2507->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2508=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2508[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2509; _temp2509.tag= Cyc_Absyn_ConstArray_mod;
_temp2509.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2509;}); _temp2508;})); _temp2507->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2507;}); _temp2506;}); _temp2505;}); _temp2504;}); break; case 216: _LL2503:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2511=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2511[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2512; _temp2512.tag=
Cyc_AbstractDeclarator_tok; _temp2512.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2513=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2513->tms=({ struct Cyc_List_List* _temp2514=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2514->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2515=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2515[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2516; _temp2516.tag= Cyc_Absyn_Function_mod;
_temp2516.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2517=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2517[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2518; _temp2518.tag= Cyc_Absyn_WithTypes;
_temp2518.f1= 0; _temp2518.f2= 0; _temp2518.f3= 0; _temp2518.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2518.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2518;}); _temp2517;})); _temp2516;}); _temp2515;}));
_temp2514->tl= 0; _temp2514;}); _temp2513;}); _temp2512;}); _temp2511;}); break;
case 217: _LL2510: { struct _tuple17 _temp2522; struct Cyc_List_List* _temp2523;
struct Cyc_Core_Opt* _temp2525; struct Cyc_Absyn_VarargInfo* _temp2527; int
_temp2529; struct Cyc_List_List* _temp2531; struct _tuple17* _temp2520= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2522=*
_temp2520; _LL2532: _temp2531= _temp2522.f1; goto _LL2530; _LL2530: _temp2529=
_temp2522.f2; goto _LL2528; _LL2528: _temp2527= _temp2522.f3; goto _LL2526;
_LL2526: _temp2525= _temp2522.f4; goto _LL2524; _LL2524: _temp2523= _temp2522.f5;
goto _LL2521; _LL2521: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2533=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2533[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2534; _temp2534.tag= Cyc_AbstractDeclarator_tok; _temp2534.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2535=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2535->tms=({ struct Cyc_List_List* _temp2536=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2536->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2537=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2537[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2538; _temp2538.tag= Cyc_Absyn_Function_mod;
_temp2538.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2539=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2539[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2540; _temp2540.tag= Cyc_Absyn_WithTypes;
_temp2540.f1= _temp2531; _temp2540.f2= _temp2529; _temp2540.f3= _temp2527;
_temp2540.f4= _temp2525; _temp2540.f5= _temp2523; _temp2540;}); _temp2539;}));
_temp2538;}); _temp2537;})); _temp2536->tl= 0; _temp2536;}); _temp2535;});
_temp2534;}); _temp2533;}); break;} case 218: _LL2519: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2542=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2542[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2543; _temp2543.tag= Cyc_AbstractDeclarator_tok;
_temp2543.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2544=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2544->tms=({
struct Cyc_List_List* _temp2545=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2545->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2546=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2546[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2547; _temp2547.tag=
Cyc_Absyn_Function_mod; _temp2547.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2548=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2548[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2549; _temp2549.tag= Cyc_Absyn_WithTypes;
_temp2549.f1= 0; _temp2549.f2= 0; _temp2549.f3= 0; _temp2549.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2549.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2549;}); _temp2548;})); _temp2547;}); _temp2546;}));
_temp2545->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2545;});
_temp2544;}); _temp2543;}); _temp2542;}); break; case 219: _LL2541: { struct
_tuple17 _temp2553; struct Cyc_List_List* _temp2554; struct Cyc_Core_Opt*
_temp2556; struct Cyc_Absyn_VarargInfo* _temp2558; int _temp2560; struct Cyc_List_List*
_temp2562; struct _tuple17* _temp2551= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2553=*
_temp2551; _LL2563: _temp2562= _temp2553.f1; goto _LL2561; _LL2561: _temp2560=
_temp2553.f2; goto _LL2559; _LL2559: _temp2558= _temp2553.f3; goto _LL2557;
_LL2557: _temp2556= _temp2553.f4; goto _LL2555; _LL2555: _temp2554= _temp2553.f5;
goto _LL2552; _LL2552: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2564=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2564[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2565; _temp2565.tag= Cyc_AbstractDeclarator_tok; _temp2565.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2566=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2566->tms=({ struct Cyc_List_List* _temp2567=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2567->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2568=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2568[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2569; _temp2569.tag= Cyc_Absyn_Function_mod;
_temp2569.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2570=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2570[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2571; _temp2571.tag= Cyc_Absyn_WithTypes;
_temp2571.f1= _temp2562; _temp2571.f2= _temp2560; _temp2571.f3= _temp2558;
_temp2571.f4= _temp2556; _temp2571.f5= _temp2554; _temp2571;}); _temp2570;}));
_temp2569;}); _temp2568;})); _temp2567->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2567;}); _temp2566;}); _temp2565;}); _temp2564;}); break;} case 220:
_LL2550: { struct Cyc_List_List* _temp2573=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2574=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2574[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2575; _temp2575.tag= Cyc_AbstractDeclarator_tok; _temp2575.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2576=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2576->tms=({ struct Cyc_List_List* _temp2577=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2577->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2578=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2578[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2579; _temp2579.tag= Cyc_Absyn_TypeParams_mod;
_temp2579.f1= _temp2573; _temp2579.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2579.f3=
0; _temp2579;}); _temp2578;})); _temp2577->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2577;}); _temp2576;}); _temp2575;}); _temp2574;}); break;} case 221:
_LL2572:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2581=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2582=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2582[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2583; _temp2583.tag= Cyc_AbstractDeclarator_tok; _temp2583.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2584=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2584->tms=({ struct Cyc_List_List* _temp2585=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2585->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2586=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2586[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2587; _temp2587.tag= Cyc_Absyn_TypeParams_mod;
_temp2587.f1= _temp2581; _temp2587.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2587.f3=
0; _temp2587;}); _temp2586;})); _temp2585->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2585;}); _temp2584;}); _temp2583;}); _temp2582;}); break;} case 222:
_LL2580: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2589=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2589[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2590; _temp2590.tag=
Cyc_AbstractDeclarator_tok; _temp2590.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2591=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2591->tms=({ struct Cyc_List_List* _temp2592=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2592->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2593=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2593[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2594; _temp2594.tag= Cyc_Absyn_Attributes_mod;
_temp2594.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2594.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2594;}); _temp2593;}));
_temp2592->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms; _temp2592;});
_temp2591;}); _temp2590;}); _temp2589;}); break; case 223: _LL2588: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL2595:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL2596: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL2597: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL2598: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL2599:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
229: _LL2600: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2612=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2612->name=({ struct
_tagged_arr* _temp2613=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2613[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2613;});
_temp2612->identity= 0; _temp2612->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2612;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2610=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2610[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2611; _temp2611.tag= Cyc_Absyn_VarType;
_temp2611.f1= tv; _temp2611;}); _temp2610;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2602=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2602[ 0]=({ struct Cyc_Stmt_tok_struct _temp2603; _temp2603.tag= Cyc_Stmt_tok;
_temp2603.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2604=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2604[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2605; _temp2605.tag= Cyc_Absyn_Region_s;
_temp2605.f1= tv; _temp2605.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2606=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2606->f1=(
void*) Cyc_Absyn_Loc_n; _temp2606->f2=({ struct _tagged_arr* _temp2607=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2607[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2607;});
_temp2606;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2608=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2608[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2609; _temp2609.tag=
Cyc_Absyn_RgnHandleType; _temp2609.f1=( void*) t; _temp2609;}); _temp2608;}), 0);
_temp2605.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2605;}); _temp2604;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2603;});
_temp2602;}); break;} case 230: _LL2601: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2625=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2625->name=({ struct
_tagged_arr* _temp2626=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2626[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp2628; _temp2628.tag= Cyc_Stdio_String_pa; _temp2628.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]);{ void* _temp2627[ 1u]={& _temp2628}; Cyc_Stdio_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2627, sizeof( void*), 1u));}});
_temp2626;}); _temp2625->identity= 0; _temp2625->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2625;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2623=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2623[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2624; _temp2624.tag= Cyc_Absyn_VarType;
_temp2624.f1= tv; _temp2624;}); _temp2623;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2615=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2615[ 0]=({ struct Cyc_Stmt_tok_struct _temp2616; _temp2616.tag= Cyc_Stmt_tok;
_temp2616.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2617=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2617[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2618; _temp2618.tag= Cyc_Absyn_Region_s;
_temp2618.f1= tv; _temp2618.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2619=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2619->f1=(
void*) Cyc_Absyn_Loc_n; _temp2619->f2=({ struct _tagged_arr* _temp2620=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2620[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2620;});
_temp2619;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2621=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2621[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2622; _temp2622.tag=
Cyc_Absyn_RgnHandleType; _temp2622.f1=( void*) t; _temp2622;}); _temp2621;}), 0);
_temp2618.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2618;}); _temp2617;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2616;});
_temp2615;}); break;} case 231: _LL2614: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2630=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2630[ 0]=({ struct Cyc_Stmt_tok_struct _temp2631; _temp2631.tag= Cyc_Stmt_tok;
_temp2631.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2632=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2632[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2633; _temp2633.tag= Cyc_Absyn_Cut_s;
_temp2633.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2633;}); _temp2632;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2631;});
_temp2630;}); break; case 232: _LL2629: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2635=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2635[ 0]=({ struct Cyc_Stmt_tok_struct _temp2636; _temp2636.tag= Cyc_Stmt_tok;
_temp2636.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2637=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2637[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2638; _temp2638.tag= Cyc_Absyn_Splice_s;
_temp2638.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2638;}); _temp2637;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2636;});
_temp2635;}); break; case 233: _LL2634: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2640=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2640[ 0]=({ struct Cyc_Stmt_tok_struct _temp2641; _temp2641.tag= Cyc_Stmt_tok;
_temp2641.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2642=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2642[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2643; _temp2643.tag= Cyc_Absyn_Label_s;
_temp2643.f1=({ struct _tagged_arr* _temp2644=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2644[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2644;});
_temp2643.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2643;}); _temp2642;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2641;});
_temp2640;}); break; case 234: _LL2639: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2646=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2646[ 0]=({ struct Cyc_Stmt_tok_struct _temp2647; _temp2647.tag= Cyc_Stmt_tok;
_temp2647.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2647;});
_temp2646;}); break; case 235: _LL2645: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2649=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2649[ 0]=({ struct Cyc_Stmt_tok_struct _temp2650; _temp2650.tag= Cyc_Stmt_tok;
_temp2650.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2650;}); _temp2649;}); break; case 236: _LL2648: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2652=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2652[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2653; _temp2653.tag= Cyc_Stmt_tok; _temp2653.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2653;}); _temp2652;}); break; case 237: _LL2651: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 238:
_LL2654:{ void* _temp2656= Cyc_yyget_BlockItem_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); struct Cyc_List_List*
_temp2664; struct Cyc_Absyn_Fndecl* _temp2666; struct Cyc_Absyn_Stmt* _temp2668;
_LL2658: if(*(( int*) _temp2656) == Cyc_Parse_TopDecls_bl){ _LL2665: _temp2664=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2656)->f1; goto _LL2659;} else{ goto
_LL2660;} _LL2660: if(*(( int*) _temp2656) == Cyc_Parse_FnDecl_bl){ _LL2667:
_temp2666=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2656)->f1; goto _LL2661;}
else{ goto _LL2662;} _LL2662: if(*(( int*) _temp2656) == Cyc_Parse_Stmt_bl){
_LL2669: _temp2668=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2656)->f1; goto
_LL2663;} else{ goto _LL2657;} _LL2659: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2670=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2670[ 0]=({ struct Cyc_Stmt_tok_struct _temp2671; _temp2671.tag= Cyc_Stmt_tok;
_temp2671.f1= Cyc_Parse_flatten_declarations( _temp2664, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2671;}); _temp2670;}); goto _LL2657; _LL2661:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2672=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2672[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2673; _temp2673.tag= Cyc_Stmt_tok; _temp2673.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2674=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2674[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2675; _temp2675.tag= Cyc_Absyn_Fn_d;
_temp2675.f1= _temp2666; _temp2675;}); _temp2674;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2673;}); _temp2672;}); goto _LL2657; _LL2663:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2676=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2676[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2677; _temp2677.tag= Cyc_Stmt_tok; _temp2677.f1= _temp2668; _temp2677;});
_temp2676;}); goto _LL2657; _LL2657:;} break; case 239: _LL2655:{ void*
_temp2679= Cyc_yyget_BlockItem_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); struct Cyc_List_List* _temp2687; struct Cyc_Absyn_Fndecl*
_temp2689; struct Cyc_Absyn_Stmt* _temp2691; _LL2681: if(*(( int*) _temp2679) ==
Cyc_Parse_TopDecls_bl){ _LL2688: _temp2687=(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp2679)->f1; goto _LL2682;} else{ goto _LL2683;} _LL2683: if(*(( int*)
_temp2679) == Cyc_Parse_FnDecl_bl){ _LL2690: _temp2689=(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp2679)->f1; goto _LL2684;} else{ goto _LL2685;} _LL2685: if(*(( int*)
_temp2679) == Cyc_Parse_Stmt_bl){ _LL2692: _temp2691=(( struct Cyc_Parse_Stmt_bl_struct*)
_temp2679)->f1; goto _LL2686;} else{ goto _LL2680;} _LL2682: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2693=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2693[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2694; _temp2694.tag= Cyc_Stmt_tok; _temp2694.f1= Cyc_Parse_flatten_declarations(
_temp2687, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2694;}); _temp2693;}); goto _LL2680; _LL2684: yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2695=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2695[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2696; _temp2696.tag= Cyc_Stmt_tok; _temp2696.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2697=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2697[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2698; _temp2698.tag= Cyc_Absyn_Fn_d;
_temp2698.f1= _temp2689; _temp2698;}); _temp2697;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2696;}); _temp2695;}); goto _LL2680; _LL2686: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2699=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2699[ 0]=({ struct Cyc_Stmt_tok_struct _temp2700; _temp2700.tag= Cyc_Stmt_tok;
_temp2700.f1= Cyc_Absyn_seq_stmt( _temp2691, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2700;}); _temp2699;}); goto _LL2680; _LL2680:;} break; case 240: _LL2678:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2702=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp2702[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp2703; _temp2703.tag= Cyc_BlockItem_tok; _temp2703.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp2704=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp2704[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp2705; _temp2705.tag= Cyc_Parse_TopDecls_bl;
_temp2705.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2705;}); _temp2704;})); _temp2703;}); _temp2702;});
break; case 241: _LL2701: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp2707=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2707[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2708; _temp2708.tag= Cyc_BlockItem_tok;
_temp2708.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp2709=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp2709[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp2710; _temp2710.tag= Cyc_Parse_Stmt_bl;
_temp2710.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2710;}); _temp2709;})); _temp2708;}); _temp2707;}); break;
case 242: _LL2706: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2712=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2712[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2713; _temp2713.tag= Cyc_BlockItem_tok;
_temp2713.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp2714=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp2714[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp2715; _temp2715.tag= Cyc_Parse_FnDecl_bl;
_temp2715.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2715;}); _temp2714;})); _temp2713;}); _temp2712;}); break;
case 243: _LL2711: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2717=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2717[ 0]=({ struct Cyc_Stmt_tok_struct _temp2718; _temp2718.tag= Cyc_Stmt_tok;
_temp2718.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2718;}); _temp2717;}); break; case 244:
_LL2716: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2720=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2720[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2721; _temp2721.tag= Cyc_Stmt_tok; _temp2721.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2721;});
_temp2720;}); break; case 245: _LL2719: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2723=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2723[ 0]=({ struct Cyc_Stmt_tok_struct _temp2724; _temp2724.tag= Cyc_Stmt_tok;
_temp2724.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2724;}); _temp2723;}); break; case 246: _LL2722: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2726=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2726[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2727; _temp2727.tag= Cyc_Stmt_tok; _temp2727.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2728=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2728[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2729; _temp2729.tag= Cyc_Absyn_SwitchC_s;
_temp2729.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2729.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2729;});
_temp2728;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2727;}); _temp2726;}); break; case 247:
_LL2725: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2731=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2731[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2732; _temp2732.tag= Cyc_Stmt_tok; _temp2732.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2732;});
_temp2731;}); break; case 248: _LL2730: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2734=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2734[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2735; _temp2735.tag= Cyc_SwitchClauseList_tok; _temp2735.f1= 0; _temp2735;});
_temp2734;}); break; case 249: _LL2733: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2737=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2737[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2738; _temp2738.tag= Cyc_SwitchClauseList_tok; _temp2738.f1=({ struct Cyc_List_List*
_temp2739=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2739->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2740=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2740->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2740->pat_vars= 0; _temp2740->where_clause= 0; _temp2740->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2740->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2740;}); _temp2739->tl= 0; _temp2739;});
_temp2738;}); _temp2737;}); break; case 250: _LL2736: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2742=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2742[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2743; _temp2743.tag= Cyc_SwitchClauseList_tok;
_temp2743.f1=({ struct Cyc_List_List* _temp2744=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2744->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2745=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2745->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2745->pat_vars= 0; _temp2745->where_clause= 0;
_temp2745->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2745->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2745;});
_temp2744->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2744;}); _temp2743;});
_temp2742;}); break; case 251: _LL2741: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2747=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2747[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2748; _temp2748.tag= Cyc_SwitchClauseList_tok; _temp2748.f1=({ struct Cyc_List_List*
_temp2749=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2749->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2750=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2750->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2750->pat_vars=
0; _temp2750->where_clause= 0; _temp2750->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2750->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2750;}); _temp2749->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2749;}); _temp2748;});
_temp2747;}); break; case 252: _LL2746: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2752=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2752[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2753; _temp2753.tag= Cyc_SwitchClauseList_tok; _temp2753.f1=({ struct Cyc_List_List*
_temp2754=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2754->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2755=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2755->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2755->pat_vars=
0; _temp2755->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2755->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2755->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2755;});
_temp2754->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2754;}); _temp2753;});
_temp2752;}); break; case 253: _LL2751: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2757=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2757[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2758; _temp2758.tag= Cyc_SwitchClauseList_tok; _temp2758.f1=({ struct Cyc_List_List*
_temp2759=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2759->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2760=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2760->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2760->pat_vars=
0; _temp2760->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2760->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2760->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2760;});
_temp2759->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2759;}); _temp2758;});
_temp2757;}); break; case 254: _LL2756: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2762=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2762[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2763; _temp2763.tag= Cyc_SwitchCClauseList_tok; _temp2763.f1= 0; _temp2763;});
_temp2762;}); break; case 255: _LL2761: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2765=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2765[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2766; _temp2766.tag= Cyc_SwitchCClauseList_tok; _temp2766.f1=({ struct Cyc_List_List*
_temp2767=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2767->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2768=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2768->cnst_exp= 0;
_temp2768->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2768->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2768;});
_temp2767->tl= 0; _temp2767;}); _temp2766;}); _temp2765;}); break; case 256:
_LL2764: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2770=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2770[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2771; _temp2771.tag=
Cyc_SwitchCClauseList_tok; _temp2771.f1=({ struct Cyc_List_List* _temp2772=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2772->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2773=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2773->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2773->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2773->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2773;});
_temp2772->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2772;}); _temp2771;});
_temp2770;}); break; case 257: _LL2769: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2775=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2775[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2776; _temp2776.tag= Cyc_SwitchCClauseList_tok; _temp2776.f1=({ struct Cyc_List_List*
_temp2777=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2777->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2778=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2778->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2778->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2778->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2778;});
_temp2777->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2777;}); _temp2776;});
_temp2775;}); break; case 258: _LL2774: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2780=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2780[ 0]=({ struct Cyc_Stmt_tok_struct _temp2781; _temp2781.tag= Cyc_Stmt_tok;
_temp2781.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2781;}); _temp2780;}); break; case 259: _LL2779: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2783=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2783[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2784; _temp2784.tag= Cyc_Stmt_tok; _temp2784.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2784;}); _temp2783;}); break; case 260: _LL2782: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2786=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2786[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2787; _temp2787.tag= Cyc_Stmt_tok; _temp2787.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2787;}); _temp2786;}); break; case 261: _LL2785: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2789=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2789[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2790; _temp2790.tag= Cyc_Stmt_tok; _temp2790.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2790;}); _temp2789;}); break; case 262: _LL2788: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2792=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2792[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2793; _temp2793.tag= Cyc_Stmt_tok; _temp2793.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2793;}); _temp2792;}); break; case 263: _LL2791: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2795=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2795[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2796; _temp2796.tag= Cyc_Stmt_tok; _temp2796.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2796;}); _temp2795;}); break; case 264: _LL2794: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2798=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2798[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2799; _temp2799.tag= Cyc_Stmt_tok; _temp2799.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2799;}); _temp2798;}); break; case 265: _LL2797: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2801=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2801[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2802; _temp2802.tag= Cyc_Stmt_tok; _temp2802.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2802;});
_temp2801;}); break; case 266: _LL2800: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2804=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2804[ 0]=({ struct Cyc_Stmt_tok_struct _temp2805; _temp2805.tag= Cyc_Stmt_tok;
_temp2805.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2805;});
_temp2804;}); break; case 267: _LL2803: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2807=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2807[ 0]=({ struct Cyc_Stmt_tok_struct _temp2808; _temp2808.tag= Cyc_Stmt_tok;
_temp2808.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2808;}); _temp2807;}); break; case 268: _LL2806: { struct Cyc_List_List*
_temp2810= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2811= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2812=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2812[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2813; _temp2813.tag= Cyc_Stmt_tok; _temp2813.f1= Cyc_Parse_flatten_declarations(
_temp2810, _temp2811); _temp2813;}); _temp2812;}); break;} case 269: _LL2809: {
struct Cyc_List_List* _temp2815= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2816= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2817=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2817[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2818; _temp2818.tag= Cyc_Stmt_tok; _temp2818.f1= Cyc_Parse_flatten_declarations(
_temp2815, _temp2816); _temp2818;}); _temp2817;}); break;} case 270: _LL2814: {
struct Cyc_List_List* _temp2820= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2821= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2822=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2822[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2823; _temp2823.tag= Cyc_Stmt_tok; _temp2823.f1= Cyc_Parse_flatten_declarations(
_temp2820, _temp2821); _temp2823;}); _temp2822;}); break;} case 271: _LL2819: {
struct Cyc_List_List* _temp2825= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2826= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2827=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2827[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2828; _temp2828.tag= Cyc_Stmt_tok; _temp2828.f1= Cyc_Parse_flatten_declarations(
_temp2825, _temp2826); _temp2828;}); _temp2827;}); break;} case 272: _LL2824:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2830=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2830[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2831; _temp2831.tag= Cyc_Stmt_tok; _temp2831.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2832=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2832[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2832;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2831;}); _temp2830;}); break; case 273: _LL2829: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2834=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2834[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2835; _temp2835.tag= Cyc_Stmt_tok; _temp2835.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2835;}); _temp2834;}); break; case 274:
_LL2833: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2837=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2837[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2838; _temp2838.tag= Cyc_Stmt_tok; _temp2838.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2838;}); _temp2837;}); break; case 275: _LL2836: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2840=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2840[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2841; _temp2841.tag= Cyc_Stmt_tok; _temp2841.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2841;}); _temp2840;}); break; case 276:
_LL2839: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2843=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2843[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2844; _temp2844.tag= Cyc_Stmt_tok; _temp2844.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2844;}); _temp2843;}); break; case 277: _LL2842: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2846=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2846[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2847; _temp2847.tag= Cyc_Stmt_tok; _temp2847.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2847;}); _temp2846;}); break; case 278:
_LL2845: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2849=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2849[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2850; _temp2850.tag= Cyc_Stmt_tok; _temp2850.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2850;}); _temp2849;}); break; case 279:
_LL2848: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2852=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2852[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2853; _temp2853.tag= Cyc_Stmt_tok; _temp2853.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2853;}); _temp2852;}); break; case 280: _LL2851: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2855=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2855[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2856; _temp2856.tag= Cyc_Pattern_tok; _temp2856.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2856;});
_temp2855;}); break; case 281: _LL2854: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 282:
_LL2857: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2859=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2859[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2860; _temp2860.tag= Cyc_Pattern_tok; _temp2860.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2861=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2861[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2862; _temp2862.tag= Cyc_Absyn_Int_p; _temp2862.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2862.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2862;}); _temp2861;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2860;});
_temp2859;}); break; case 283: _LL2858: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2864=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2864[ 0]=({ struct Cyc_Pattern_tok_struct _temp2865; _temp2865.tag= Cyc_Pattern_tok;
_temp2865.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2866=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2866[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2867; _temp2867.tag= Cyc_Absyn_Int_p;
_temp2867.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2867.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2867;});
_temp2866;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2865;}); _temp2864;}); break; case 284:
_LL2863: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2869=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2869[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2870; _temp2870.tag= Cyc_Pattern_tok; _temp2870.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2871=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2871[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2872; _temp2872.tag= Cyc_Absyn_Float_p; _temp2872.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2872;}); _temp2871;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2870;});
_temp2869;}); break; case 285: _LL2868: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2874=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2874[ 0]=({ struct Cyc_Pattern_tok_struct _temp2875; _temp2875.tag= Cyc_Pattern_tok;
_temp2875.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2876=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2876[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2877; _temp2877.tag= Cyc_Absyn_Char_p; _temp2877.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2877;});
_temp2876;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2875;}); _temp2874;}); break; case 286:
_LL2873: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2879=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2879[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2880; _temp2880.tag= Cyc_Pattern_tok; _temp2880.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2880;});
_temp2879;}); break; case 287: _LL2878: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2882=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2882[ 0]=({ struct Cyc_Pattern_tok_struct _temp2883; _temp2883.tag= Cyc_Pattern_tok;
_temp2883.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2884=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2884[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2885; _temp2885.tag=
Cyc_Absyn_UnknownId_p; _temp2885.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2885;}); _temp2884;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2883;}); _temp2882;}); break; case 288:
_LL2881: { struct Cyc_List_List* _temp2887=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2888=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2888[ 0]=({ struct Cyc_Pattern_tok_struct _temp2889; _temp2889.tag= Cyc_Pattern_tok;
_temp2889.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2890=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2890[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2891; _temp2891.tag= Cyc_Absyn_UnknownCall_p; _temp2891.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2891.f2=
_temp2887; _temp2891.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2891;});
_temp2890;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2889;}); _temp2888;}); break;} case
289: _LL2886: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2893=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2893[ 0]=({ struct Cyc_Pattern_tok_struct _temp2894; _temp2894.tag= Cyc_Pattern_tok;
_temp2894.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2895=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2895[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2896; _temp2896.tag= Cyc_Absyn_Tuple_p;
_temp2896.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2896;}); _temp2895;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2894;}); _temp2893;}); break; case 290: _LL2892: { struct Cyc_List_List*
_temp2898=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2899=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2899[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2900; _temp2900.tag= Cyc_Pattern_tok; _temp2900.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2901=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2901[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2902; _temp2902.tag= Cyc_Absyn_UnknownFields_p;
_temp2902.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2902.f2= _temp2898; _temp2902.f3= 0; _temp2902;});
_temp2901;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2900;}); _temp2899;}); break;} case
291: _LL2897: { struct Cyc_List_List* _temp2904=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2905=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2905[ 0]=({ struct Cyc_Pattern_tok_struct _temp2906; _temp2906.tag= Cyc_Pattern_tok;
_temp2906.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2907=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2907[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2908; _temp2908.tag= Cyc_Absyn_UnknownFields_p; _temp2908.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2908.f2=
_temp2904; _temp2908.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2908;});
_temp2907;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2906;}); _temp2905;}); break;} case
292: _LL2903: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2910=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2910[ 0]=({ struct Cyc_Pattern_tok_struct _temp2911; _temp2911.tag= Cyc_Pattern_tok;
_temp2911.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2912=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2912[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2913; _temp2913.tag= Cyc_Absyn_Pointer_p;
_temp2913.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2913;}); _temp2912;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2911;});
_temp2910;}); break; case 293: _LL2909: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2915=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2915[ 0]=({ struct Cyc_Pattern_tok_struct _temp2916; _temp2916.tag= Cyc_Pattern_tok;
_temp2916.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2917=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2917[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2918; _temp2918.tag=
Cyc_Absyn_Reference_p; _temp2918.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2919=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2919->f1=(
void*) Cyc_Absyn_Loc_n; _temp2919->f2=({ struct _tagged_arr* _temp2920=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2920[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2920;});
_temp2919;}),( void*) Cyc_Absyn_VoidType, 0); _temp2918;}); _temp2917;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2916;}); _temp2915;}); break; case 294: _LL2914: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2922=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2922[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2923; _temp2923.tag= Cyc_PatternList_tok;
_temp2923.f1= 0; _temp2923;}); _temp2922;}); break; case 295: _LL2921: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2925=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2925[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2926; _temp2926.tag= Cyc_PatternList_tok;
_temp2926.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2926;}); _temp2925;}); break; case 296: _LL2924: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2928=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2928[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2929; _temp2929.tag= Cyc_PatternList_tok;
_temp2929.f1=({ struct Cyc_List_List* _temp2930=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2930->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2930->tl= 0;
_temp2930;}); _temp2929;}); _temp2928;}); break; case 297: _LL2927: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2932=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2932[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2933; _temp2933.tag= Cyc_PatternList_tok;
_temp2933.f1=({ struct Cyc_List_List* _temp2934=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2934->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2934->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2934;}); _temp2933;}); _temp2932;}); break; case 298:
_LL2931: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2936=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2936[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2937; _temp2937.tag=
Cyc_FieldPattern_tok; _temp2937.f1=({ struct _tuple14* _temp2938=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2938->f1= 0; _temp2938->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2938;}); _temp2937;}); _temp2936;}); break; case 299:
_LL2935: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2940=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2940[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2941; _temp2941.tag=
Cyc_FieldPattern_tok; _temp2941.f1=({ struct _tuple14* _temp2942=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2942->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2942->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2942;}); _temp2941;}); _temp2940;}); break; case 300:
_LL2939: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2944=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2944[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2945; _temp2945.tag=
Cyc_FieldPatternList_tok; _temp2945.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2945;});
_temp2944;}); break; case 301: _LL2943: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2947=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2947[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2948; _temp2948.tag= Cyc_FieldPatternList_tok; _temp2948.f1=({ struct Cyc_List_List*
_temp2949=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2949->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2949->tl= 0;
_temp2949;}); _temp2948;}); _temp2947;}); break; case 302: _LL2946: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2951=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2951[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2952; _temp2952.tag= Cyc_FieldPatternList_tok;
_temp2952.f1=({ struct Cyc_List_List* _temp2953=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2953->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2953->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2953;}); _temp2952;}); _temp2951;}); break; case 303:
_LL2950: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL2954: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2956=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2956[ 0]=({ struct Cyc_Exp_tok_struct _temp2957; _temp2957.tag= Cyc_Exp_tok;
_temp2957.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2957;}); _temp2956;}); break; case 305: _LL2955: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL2958:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2960=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2960[ 0]=({ struct Cyc_Exp_tok_struct
_temp2961; _temp2961.tag= Cyc_Exp_tok; _temp2961.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2961;}); _temp2960;}); break; case 307: _LL2959: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2963=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2963[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2964; _temp2964.tag= Cyc_Primopopt_tok; _temp2964.f1=
0; _temp2964;}); _temp2963;}); break; case 308: _LL2962: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2966=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2966[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2967; _temp2967.tag= Cyc_Primopopt_tok; _temp2967.f1=({
struct Cyc_Core_Opt* _temp2968=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2968->v=( void*)(( void*) Cyc_Absyn_Times); _temp2968;});
_temp2967;}); _temp2966;}); break; case 309: _LL2965: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2970=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2970[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2971; _temp2971.tag= Cyc_Primopopt_tok; _temp2971.f1=({
struct Cyc_Core_Opt* _temp2972=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2972->v=( void*)(( void*) Cyc_Absyn_Div); _temp2972;});
_temp2971;}); _temp2970;}); break; case 310: _LL2969: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2974=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2974[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2975; _temp2975.tag= Cyc_Primopopt_tok; _temp2975.f1=({
struct Cyc_Core_Opt* _temp2976=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2976->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2976;});
_temp2975;}); _temp2974;}); break; case 311: _LL2973: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2978=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2978[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2979; _temp2979.tag= Cyc_Primopopt_tok; _temp2979.f1=({
struct Cyc_Core_Opt* _temp2980=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2980->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2980;});
_temp2979;}); _temp2978;}); break; case 312: _LL2977: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2982=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2982[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2983; _temp2983.tag= Cyc_Primopopt_tok; _temp2983.f1=({
struct Cyc_Core_Opt* _temp2984=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2984->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2984;});
_temp2983;}); _temp2982;}); break; case 313: _LL2981: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2986=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2986[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2987; _temp2987.tag= Cyc_Primopopt_tok; _temp2987.f1=({
struct Cyc_Core_Opt* _temp2988=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2988->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2988;});
_temp2987;}); _temp2986;}); break; case 314: _LL2985: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2990=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2990[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2991; _temp2991.tag= Cyc_Primopopt_tok; _temp2991.f1=({
struct Cyc_Core_Opt* _temp2992=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2992->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2992;});
_temp2991;}); _temp2990;}); break; case 315: _LL2989: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2994=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2994[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2995; _temp2995.tag= Cyc_Primopopt_tok; _temp2995.f1=({
struct Cyc_Core_Opt* _temp2996=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2996->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2996;});
_temp2995;}); _temp2994;}); break; case 316: _LL2993: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2998=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2998[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2999; _temp2999.tag= Cyc_Primopopt_tok; _temp2999.f1=({
struct Cyc_Core_Opt* _temp3000=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3000->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3000;});
_temp2999;}); _temp2998;}); break; case 317: _LL2997: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3002=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3002[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3003; _temp3003.tag= Cyc_Primopopt_tok; _temp3003.f1=({
struct Cyc_Core_Opt* _temp3004=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3004->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3004;});
_temp3003;}); _temp3002;}); break; case 318: _LL3001: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL3005:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3007=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3007[ 0]=({ struct Cyc_Exp_tok_struct
_temp3008; _temp3008.tag= Cyc_Exp_tok; _temp3008.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3008;});
_temp3007;}); break; case 320: _LL3006: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3010=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3010[ 0]=({ struct Cyc_Exp_tok_struct _temp3011; _temp3011.tag= Cyc_Exp_tok;
_temp3011.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3011;}); _temp3010;}); break; case 321: _LL3009: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3013=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3013[ 0]=({ struct Cyc_Exp_tok_struct
_temp3014; _temp3014.tag= Cyc_Exp_tok; _temp3014.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3014;}); _temp3013;}); break; case 322: _LL3012: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3016=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3016[ 0]=({ struct Cyc_Exp_tok_struct
_temp3017; _temp3017.tag= Cyc_Exp_tok; _temp3017.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3017;}); _temp3016;}); break; case 323: _LL3015: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3019=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3019[ 0]=({ struct Cyc_Exp_tok_struct
_temp3020; _temp3020.tag= Cyc_Exp_tok; _temp3020.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3020;});
_temp3019;}); break; case 324: _LL3018: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3022=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3022[ 0]=({ struct Cyc_Exp_tok_struct _temp3023; _temp3023.tag= Cyc_Exp_tok;
_temp3023.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3023;}); _temp3022;}); break; case 325: _LL3021: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL3024:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL3025: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3027=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3027[ 0]=({ struct Cyc_Exp_tok_struct
_temp3028; _temp3028.tag= Cyc_Exp_tok; _temp3028.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3028;}); _temp3027;}); break; case 328: _LL3026: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL3029:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3031=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3031[ 0]=({ struct Cyc_Exp_tok_struct
_temp3032; _temp3032.tag= Cyc_Exp_tok; _temp3032.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3032;}); _temp3031;}); break; case 330: _LL3030: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL3033:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3035=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3035[ 0]=({ struct Cyc_Exp_tok_struct
_temp3036; _temp3036.tag= Cyc_Exp_tok; _temp3036.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3036;});
_temp3035;}); break; case 332: _LL3034: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL3037:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3039=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3039[ 0]=({ struct Cyc_Exp_tok_struct
_temp3040; _temp3040.tag= Cyc_Exp_tok; _temp3040.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3040;});
_temp3039;}); break; case 334: _LL3038: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL3041:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3043=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3043[ 0]=({ struct Cyc_Exp_tok_struct
_temp3044; _temp3044.tag= Cyc_Exp_tok; _temp3044.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3044;});
_temp3043;}); break; case 336: _LL3042: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL3045:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3047=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3047[ 0]=({ struct Cyc_Exp_tok_struct
_temp3048; _temp3048.tag= Cyc_Exp_tok; _temp3048.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3048;}); _temp3047;}); break; case 338: _LL3046: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3050=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3050[ 0]=({ struct Cyc_Exp_tok_struct
_temp3051; _temp3051.tag= Cyc_Exp_tok; _temp3051.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3051;}); _temp3050;}); break; case 339: _LL3049: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL3052:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3054=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3054[ 0]=({ struct Cyc_Exp_tok_struct
_temp3055; _temp3055.tag= Cyc_Exp_tok; _temp3055.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3055;}); _temp3054;}); break; case 341: _LL3053: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3057=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3057[ 0]=({ struct Cyc_Exp_tok_struct
_temp3058; _temp3058.tag= Cyc_Exp_tok; _temp3058.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3058;}); _temp3057;}); break; case 342: _LL3056: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3060=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3060[ 0]=({ struct Cyc_Exp_tok_struct
_temp3061; _temp3061.tag= Cyc_Exp_tok; _temp3061.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3061;}); _temp3060;}); break; case 343: _LL3059: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3063=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3063[ 0]=({ struct Cyc_Exp_tok_struct
_temp3064; _temp3064.tag= Cyc_Exp_tok; _temp3064.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3064;}); _temp3063;}); break; case 344: _LL3062: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL3065:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3067=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3067[ 0]=({ struct Cyc_Exp_tok_struct
_temp3068; _temp3068.tag= Cyc_Exp_tok; _temp3068.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3068;}); _temp3067;}); break; case 346: _LL3066: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3071;}); _temp3070;}); break; case 347: _LL3069: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL3072:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3074=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3074[ 0]=({ struct Cyc_Exp_tok_struct
_temp3075; _temp3075.tag= Cyc_Exp_tok; _temp3075.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3075;});
_temp3074;}); break; case 349: _LL3073: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3077=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3077[ 0]=({ struct Cyc_Exp_tok_struct _temp3078; _temp3078.tag= Cyc_Exp_tok;
_temp3078.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3078;}); _temp3077;}); break; case 350: _LL3076: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL3079:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3081=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3081[ 0]=({ struct Cyc_Exp_tok_struct
_temp3082; _temp3082.tag= Cyc_Exp_tok; _temp3082.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3082;});
_temp3081;}); break; case 352: _LL3080: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3084=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3084[ 0]=({ struct Cyc_Exp_tok_struct _temp3085; _temp3085.tag= Cyc_Exp_tok;
_temp3085.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3085;}); _temp3084;}); break; case 353: _LL3083: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3087=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3087[ 0]=({ struct Cyc_Exp_tok_struct
_temp3088; _temp3088.tag= Cyc_Exp_tok; _temp3088.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3088;});
_temp3087;}); break; case 354: _LL3086: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3089:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3091=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3091[ 0]=({ struct Cyc_Exp_tok_struct
_temp3092; _temp3092.tag= Cyc_Exp_tok; _temp3092.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3092;}); _temp3091;}); break; case 356: _LL3090: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3093:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3095=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3095[ 0]=({ struct Cyc_Exp_tok_struct
_temp3096; _temp3096.tag= Cyc_Exp_tok; _temp3096.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3096;}); _temp3095;}); break; case 358: _LL3094: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3098=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3098[ 0]=({ struct Cyc_Exp_tok_struct
_temp3099; _temp3099.tag= Cyc_Exp_tok; _temp3099.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3099;}); _temp3098;}); break; case 359: _LL3097: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3101=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3101[ 0]=({ struct Cyc_Exp_tok_struct
_temp3102; _temp3102.tag= Cyc_Exp_tok; _temp3102.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3102;}); _temp3101;}); break; case 360: _LL3100: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3104=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3104[ 0]=({ struct Cyc_Exp_tok_struct
_temp3105; _temp3105.tag= Cyc_Exp_tok; _temp3105.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3105;}); _temp3104;}); break; case 361: _LL3103: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL3106:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3108=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3108[ 0]=({ struct Cyc_Exp_tok_struct
_temp3109; _temp3109.tag= Cyc_Exp_tok; _temp3109.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3109;}); _temp3108;}); break; case 363: _LL3107: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3111=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3111[ 0]=({ struct Cyc_Exp_tok_struct
_temp3112; _temp3112.tag= Cyc_Exp_tok; _temp3112.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3112;});
_temp3111;}); break; case 364: _LL3110: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3114=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3114[ 0]=({ struct Cyc_Exp_tok_struct _temp3115; _temp3115.tag= Cyc_Exp_tok;
_temp3115.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3115;}); _temp3114;}); break; case 365: _LL3113: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3117=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3117[ 0]=({ struct Cyc_Exp_tok_struct
_temp3118; _temp3118.tag= Cyc_Exp_tok; _temp3118.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3119=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3119[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3119;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3118;}); _temp3117;}); break; case 366:
_LL3116: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3121=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3121[ 0]=({ struct Cyc_Exp_tok_struct
_temp3122; _temp3122.tag= Cyc_Exp_tok; _temp3122.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3123=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3123[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3124; _temp3124.tag= Cyc_Absyn_Malloc_e;
_temp3124.f1= 0; _temp3124.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3124;}); _temp3123;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3122;});
_temp3121;}); break; case 367: _LL3120: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3126=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3126[ 0]=({ struct Cyc_Exp_tok_struct _temp3127; _temp3127.tag= Cyc_Exp_tok;
_temp3127.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3128=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3128[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3129; _temp3129.tag= Cyc_Absyn_Malloc_e;
_temp3129.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3129.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3129;}); _temp3128;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3127;});
_temp3126;}); break; case 368: _LL3125: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3131=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3131[ 0]=({ struct Cyc_Primop_tok_struct _temp3132; _temp3132.tag= Cyc_Primop_tok;
_temp3132.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3132;}); _temp3131;});
break; case 369: _LL3130: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3134=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3134[ 0]=({ struct Cyc_Primop_tok_struct _temp3135; _temp3135.tag= Cyc_Primop_tok;
_temp3135.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3135;}); _temp3134;});
break; case 370: _LL3133: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3137=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3137[ 0]=({ struct Cyc_Primop_tok_struct _temp3138; _temp3138.tag= Cyc_Primop_tok;
_temp3138.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3138;}); _temp3137;});
break; case 371: _LL3136: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 372: _LL3139: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3141=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3141[ 0]=({ struct Cyc_Exp_tok_struct _temp3142; _temp3142.tag= Cyc_Exp_tok;
_temp3142.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3142;}); _temp3141;}); break; case 373: _LL3140: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3144=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3144[ 0]=({ struct Cyc_Exp_tok_struct
_temp3145; _temp3145.tag= Cyc_Exp_tok; _temp3145.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3145;}); _temp3144;}); break; case 374:
_LL3143: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3147=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3147[ 0]=({ struct Cyc_Exp_tok_struct
_temp3148; _temp3148.tag= Cyc_Exp_tok; _temp3148.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3148;});
_temp3147;}); break; case 375: _LL3146: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3150=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3150[ 0]=({ struct Cyc_Exp_tok_struct _temp3151; _temp3151.tag= Cyc_Exp_tok;
_temp3151.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3152=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3152[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3152;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3151;}); _temp3150;}); break; case 376: _LL3149: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3154=(*
q).f1; struct Cyc_List_List* _temp3164; struct Cyc_List_List* _temp3166; _LL3156:
if( _temp3154 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3157;} else{ goto _LL3158;}
_LL3158: if(( unsigned int) _temp3154 > 1u?*(( int*) _temp3154) == Cyc_Absyn_Rel_n:
0){ _LL3165: _temp3164=(( struct Cyc_Absyn_Rel_n_struct*) _temp3154)->f1; if(
_temp3164 == 0){ goto _LL3159;} else{ goto _LL3160;}} else{ goto _LL3160;}
_LL3160: if(( unsigned int) _temp3154 > 1u?*(( int*) _temp3154) == Cyc_Absyn_Abs_n:
0){ _LL3167: _temp3166=(( struct Cyc_Absyn_Abs_n_struct*) _temp3154)->f1; if(
_temp3166 == 0){ goto _LL3161;} else{ goto _LL3162;}} else{ goto _LL3162;}
_LL3162: goto _LL3163; _LL3157: goto _LL3155; _LL3159: goto _LL3155; _LL3161:
goto _LL3155; _LL3163: Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3155; _LL3155:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3168=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3168[ 0]=({ struct Cyc_Exp_tok_struct _temp3169; _temp3169.tag= Cyc_Exp_tok;
_temp3169.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3169;}); _temp3168;}); break;} case 377: _LL3153: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3171=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3171[ 0]=({ struct Cyc_Exp_tok_struct
_temp3172; _temp3172.tag= Cyc_Exp_tok; _temp3172.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),({ struct _tagged_arr* _temp3173=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp3173[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3173;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3172;}); _temp3171;}); break; case 378: _LL3170: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3175=(*
q).f1; struct Cyc_List_List* _temp3185; struct Cyc_List_List* _temp3187; _LL3177:
if( _temp3175 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3178;} else{ goto _LL3179;}
_LL3179: if(( unsigned int) _temp3175 > 1u?*(( int*) _temp3175) == Cyc_Absyn_Rel_n:
0){ _LL3186: _temp3185=(( struct Cyc_Absyn_Rel_n_struct*) _temp3175)->f1; if(
_temp3185 == 0){ goto _LL3180;} else{ goto _LL3181;}} else{ goto _LL3181;}
_LL3181: if(( unsigned int) _temp3175 > 1u?*(( int*) _temp3175) == Cyc_Absyn_Abs_n:
0){ _LL3188: _temp3187=(( struct Cyc_Absyn_Abs_n_struct*) _temp3175)->f1; if(
_temp3187 == 0){ goto _LL3182;} else{ goto _LL3183;}} else{ goto _LL3183;}
_LL3183: goto _LL3184; _LL3178: goto _LL3176; _LL3180: goto _LL3176; _LL3182:
goto _LL3176; _LL3184: Cyc_Parse_err( _tag_arr("struct field is qualified with module name",
sizeof( unsigned char), 43u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3176; _LL3176:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3189=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3189[ 0]=({ struct Cyc_Exp_tok_struct _temp3190; _temp3190.tag= Cyc_Exp_tok;
_temp3190.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3190;}); _temp3189;}); break;} case 379: _LL3174: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3192=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3192[ 0]=({ struct Cyc_Exp_tok_struct
_temp3193; _temp3193.tag= Cyc_Exp_tok; _temp3193.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3193;}); _temp3192;}); break; case 380: _LL3191: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3195=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3195[ 0]=({ struct Cyc_Exp_tok_struct
_temp3196; _temp3196.tag= Cyc_Exp_tok; _temp3196.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3196;}); _temp3195;}); break; case 381: _LL3194: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3198=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3198[ 0]=({ struct Cyc_Exp_tok_struct
_temp3199; _temp3199.tag= Cyc_Exp_tok; _temp3199.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3200=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3200[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3201; _temp3201.tag= Cyc_Absyn_CompoundLit_e;
_temp3201.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3201.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3201;});
_temp3200;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3199;}); _temp3198;}); break; case 382:
_LL3197: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3203=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3203[ 0]=({ struct Cyc_Exp_tok_struct
_temp3204; _temp3204.tag= Cyc_Exp_tok; _temp3204.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3205=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3205[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3206; _temp3206.tag= Cyc_Absyn_CompoundLit_e;
_temp3206.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3206.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3206;});
_temp3205;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3204;}); _temp3203;}); break; case 383:
_LL3202: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3208=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3208[ 0]=({ struct Cyc_Exp_tok_struct
_temp3209; _temp3209.tag= Cyc_Exp_tok; _temp3209.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3210=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3210[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3211; _temp3211.tag= Cyc_Absyn_Fill_e; _temp3211.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3211;});
_temp3210;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3209;}); _temp3208;}); break; case 384:
_LL3207: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3213=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3213[ 0]=({ struct Cyc_Exp_tok_struct
_temp3214; _temp3214.tag= Cyc_Exp_tok; _temp3214.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3215=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3215[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3216; _temp3216.tag= Cyc_Absyn_Codegen_e;
_temp3216.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3216;}); _temp3215;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3214;}); _temp3213;}); break; case 385: _LL3212: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3218=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3218[ 0]=({ struct Cyc_Exp_tok_struct
_temp3219; _temp3219.tag= Cyc_Exp_tok; _temp3219.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3220=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3220[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3221; _temp3221.tag= Cyc_Absyn_UnknownId_e;
_temp3221.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3221;}); _temp3220;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3219;});
_temp3218;}); break; case 386: _LL3217: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 387: _LL3222:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3224=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3224[ 0]=({ struct Cyc_Exp_tok_struct
_temp3225; _temp3225.tag= Cyc_Exp_tok; _temp3225.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3225;});
_temp3224;}); break; case 388: _LL3223: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 389:
_LL3226: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3228=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3228[ 0]=({ struct Cyc_Exp_tok_struct
_temp3229; _temp3229.tag= Cyc_Exp_tok; _temp3229.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3230=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3230[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3231; _temp3231.tag=
Cyc_Absyn_UnknownId_e; _temp3231.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3231;});
_temp3230;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3229;});
_temp3228;}); break; case 390: _LL3227: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3233=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3233[ 0]=({ struct Cyc_Exp_tok_struct _temp3234; _temp3234.tag= Cyc_Exp_tok;
_temp3234.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3235=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3235[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3236; _temp3236.tag=
Cyc_Absyn_UnknownId_e; _temp3236.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3236;});
_temp3235;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3234;}); _temp3233;}); break; case 391: _LL3232: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3238=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3238[ 0]=({ struct Cyc_Exp_tok_struct
_temp3239; _temp3239.tag= Cyc_Exp_tok; _temp3239.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3239;}); _temp3238;}); break; case 392: _LL3237: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3241=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3241[ 0]=({ struct Cyc_Exp_tok_struct
_temp3242; _temp3242.tag= Cyc_Exp_tok; _temp3242.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3243=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3243[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3244; _temp3244.tag= Cyc_Absyn_Struct_e;
_temp3244.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3244.f2= 0; _temp3244.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3244.f4= 0;
_temp3244;}); _temp3243;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3242;});
_temp3241;}); break; case 393: _LL3240: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3246=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3246[ 0]=({ struct Cyc_Exp_tok_struct _temp3247; _temp3247.tag= Cyc_Exp_tok;
_temp3247.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3247;}); _temp3246;}); break; case 394: _LL3245: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3249=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3249[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3250; _temp3250.tag= Cyc_ExpList_tok; _temp3250.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3250;});
_temp3249;}); break; case 395: _LL3248: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3252=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3252[ 0]=({ struct Cyc_ExpList_tok_struct _temp3253; _temp3253.tag= Cyc_ExpList_tok;
_temp3253.f1=({ struct Cyc_List_List* _temp3254=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3254->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3254->tl= 0;
_temp3254;}); _temp3253;}); _temp3252;}); break; case 396: _LL3251: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3256=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3256[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3257; _temp3257.tag= Cyc_ExpList_tok; _temp3257.f1=({ struct Cyc_List_List*
_temp3258=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3258->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3258->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3258;});
_temp3257;}); _temp3256;}); break; case 397: _LL3255: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3260=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3260[ 0]=({ struct Cyc_Exp_tok_struct
_temp3261; _temp3261.tag= Cyc_Exp_tok; _temp3261.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3261;});
_temp3260;}); break; case 398: _LL3259: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3263=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3263[ 0]=({ struct Cyc_Exp_tok_struct _temp3264; _temp3264.tag= Cyc_Exp_tok;
_temp3264.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3264;});
_temp3263;}); break; case 399: _LL3262: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3266=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3266[ 0]=({ struct Cyc_Exp_tok_struct _temp3267; _temp3267.tag= Cyc_Exp_tok;
_temp3267.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3267;});
_temp3266;}); break; case 400: _LL3265: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3269=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3269[ 0]=({ struct Cyc_Exp_tok_struct _temp3270; _temp3270.tag= Cyc_Exp_tok;
_temp3270.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3270;});
_temp3269;}); break; case 401: _LL3268: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3272=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3272[ 0]=({ struct Cyc_QualId_tok_struct _temp3273; _temp3273.tag= Cyc_QualId_tok;
_temp3273.f1=({ struct _tuple1* _temp3274=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3274->f1= Cyc_Absyn_rel_ns_null; _temp3274->f2=({ struct
_tagged_arr* _temp3275=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3275[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3275;}); _temp3274;});
_temp3273;}); _temp3272;}); break; case 402: _LL3271: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3276:(
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 403u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn - 119)] + yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >= 0?
yystate <= 4401: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4402u, yystate)]
== yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4402u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 119)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 794u, yystate)]; if( yyn > - 32768? yyn < 4401:
0){ int sze= 0; struct _tagged_arr msg; int x; int count; count= 0; for( x= yyn
< 0? - yyn: 0; x < 231u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4402u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 231u, x)]) + 15, count ++);}} msg=({
unsigned int _temp3278=( unsigned int)( sze + 15); unsigned char* _temp3279=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3278); struct
_tagged_arr _temp3281= _tag_arr( _temp3279, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3280= _temp3278; unsigned int i;
for( i= 0; i < _temp3280; i ++){ _temp3279[ i]='\000';}}; _temp3281;}); Cyc_String_strcpy(
msg, _tag_arr("parse error", sizeof( unsigned char), 12u)); if( count < 5){
count= 0; for( x= yyn < 0? - yyn: 0; x < 231u / sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4402u, x + yyn)] == x){ Cyc_String_strcat(
msg, count == 0? _tag_arr(", expecting `", sizeof( unsigned char), 14u):
_tag_arr(" or `", sizeof( unsigned char), 6u)); Cyc_String_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 231u, x)]); Cyc_String_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset == 0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 794u,
yystate)]; if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1:
yyn > 4401)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4402u, yyn)] != 1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4402u,
yyn)]; if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 793){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3291= v; struct
_tuple16* _temp3311; struct _tuple16 _temp3313; int _temp3314; unsigned char
_temp3316; short _temp3318; struct _tagged_arr _temp3320; struct Cyc_Core_Opt*
_temp3322; struct Cyc_Core_Opt* _temp3324; struct Cyc_Core_Opt _temp3326; struct
_tagged_arr* _temp3327; struct _tuple1* _temp3329; struct _tuple1 _temp3331;
struct _tagged_arr* _temp3332; void* _temp3334; _LL3293: if( _temp3291 == Cyc_Okay_tok){
goto _LL3294;} else{ goto _LL3295;} _LL3295: if(*(( void**) _temp3291) == Cyc_Int_tok){
_LL3312: _temp3311=(( struct Cyc_Int_tok_struct*) _temp3291)->f1; _temp3313=*
_temp3311; _LL3315: _temp3314= _temp3313.f2; goto _LL3296;} else{ goto _LL3297;}
_LL3297: if(*(( void**) _temp3291) == Cyc_Char_tok){ _LL3317: _temp3316=((
struct Cyc_Char_tok_struct*) _temp3291)->f1; goto _LL3298;} else{ goto _LL3299;}
_LL3299: if(*(( void**) _temp3291) == Cyc_Short_tok){ _LL3319: _temp3318=((
struct Cyc_Short_tok_struct*) _temp3291)->f1; goto _LL3300;} else{ goto _LL3301;}
_LL3301: if(*(( void**) _temp3291) == Cyc_String_tok){ _LL3321: _temp3320=((
struct Cyc_String_tok_struct*) _temp3291)->f1; goto _LL3302;} else{ goto _LL3303;}
_LL3303: if(*(( void**) _temp3291) == Cyc_Stringopt_tok){ _LL3323: _temp3322=((
struct Cyc_Stringopt_tok_struct*) _temp3291)->f1; if( _temp3322 == 0){ goto
_LL3304;} else{ goto _LL3305;}} else{ goto _LL3305;} _LL3305: if(*(( void**)
_temp3291) == Cyc_Stringopt_tok){ _LL3325: _temp3324=(( struct Cyc_Stringopt_tok_struct*)
_temp3291)->f1; if( _temp3324 == 0){ goto _LL3307;} else{ _temp3326=* _temp3324;
_LL3328: _temp3327=( struct _tagged_arr*) _temp3326.v; goto _LL3306;}} else{
goto _LL3307;} _LL3307: if(*(( void**) _temp3291) == Cyc_QualId_tok){ _LL3330:
_temp3329=(( struct Cyc_QualId_tok_struct*) _temp3291)->f1; _temp3331=*
_temp3329; _LL3335: _temp3334= _temp3331.f1; goto _LL3333; _LL3333: _temp3332=
_temp3331.f2; goto _LL3308;} else{ goto _LL3309;} _LL3309: goto _LL3310; _LL3294:({
void* _temp3336[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("ok",
sizeof( unsigned char), 3u), _tag_arr( _temp3336, sizeof( void*), 0u));}); goto
_LL3292; _LL3296:({ struct Cyc_Stdio_Int_pa_struct _temp3338; _temp3338.tag= Cyc_Stdio_Int_pa;
_temp3338.f1=( int)(( unsigned int) _temp3314);{ void* _temp3337[ 1u]={&
_temp3338}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp3337, sizeof( void*), 1u));}}); goto _LL3292;
_LL3298:({ struct Cyc_Stdio_Int_pa_struct _temp3340; _temp3340.tag= Cyc_Stdio_Int_pa;
_temp3340.f1=( int)(( unsigned int)(( int) _temp3316));{ void* _temp3339[ 1u]={&
_temp3340}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3339, sizeof( void*), 1u));}}); goto _LL3292;
_LL3300:({ struct Cyc_Stdio_Int_pa_struct _temp3342; _temp3342.tag= Cyc_Stdio_Int_pa;
_temp3342.f1=( int)(( unsigned int)(( int) _temp3318));{ void* _temp3341[ 1u]={&
_temp3342}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3341, sizeof( void*), 1u));}}); goto _LL3292;
_LL3302:({ struct Cyc_Stdio_String_pa_struct _temp3344; _temp3344.tag= Cyc_Stdio_String_pa;
_temp3344.f1=( struct _tagged_arr) _temp3320;{ void* _temp3343[ 1u]={& _temp3344};
Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("\"%s\"", sizeof( unsigned char),
5u), _tag_arr( _temp3343, sizeof( void*), 1u));}}); goto _LL3292; _LL3304:({
void* _temp3345[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("null",
sizeof( unsigned char), 5u), _tag_arr( _temp3345, sizeof( void*), 0u));}); goto
_LL3292; _LL3306:({ struct Cyc_Stdio_String_pa_struct _temp3347; _temp3347.tag=
Cyc_Stdio_String_pa; _temp3347.f1=( struct _tagged_arr)* _temp3327;{ void*
_temp3346[ 1u]={& _temp3347}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("\"%s\"",
sizeof( unsigned char), 5u), _tag_arr( _temp3346, sizeof( void*), 1u));}}); goto
_LL3292; _LL3308: { struct Cyc_List_List* _temp3348= 0;{ void* _temp3349=
_temp3334; struct Cyc_List_List* _temp3357; struct Cyc_List_List* _temp3359;
_LL3351: if(( unsigned int) _temp3349 > 1u?*(( int*) _temp3349) == Cyc_Absyn_Rel_n:
0){ _LL3358: _temp3357=(( struct Cyc_Absyn_Rel_n_struct*) _temp3349)->f1; goto
_LL3352;} else{ goto _LL3353;} _LL3353: if(( unsigned int) _temp3349 > 1u?*((
int*) _temp3349) == Cyc_Absyn_Abs_n: 0){ _LL3360: _temp3359=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3349)->f1; goto _LL3354;} else{ goto _LL3355;} _LL3355: if( _temp3349 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL3356;} else{ goto _LL3350;} _LL3352: _temp3348=
_temp3357; goto _LL3350; _LL3354: _temp3348= _temp3359; goto _LL3350; _LL3356:
goto _LL3350; _LL3350:;} for( 0; _temp3348 != 0; _temp3348=(( struct Cyc_List_List*)
_check_null( _temp3348))->tl){({ struct Cyc_Stdio_String_pa_struct _temp3362;
_temp3362.tag= Cyc_Stdio_String_pa; _temp3362.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp3348))->hd);{ void*
_temp3361[ 1u]={& _temp3362}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3361, sizeof( void*), 1u));}});}({
struct Cyc_Stdio_String_pa_struct _temp3364; _temp3364.tag= Cyc_Stdio_String_pa;
_temp3364.f1=( struct _tagged_arr)* _temp3332;{ void* _temp3363[ 1u]={&
_temp3364}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3363, sizeof( void*), 1u));}}); goto _LL3292;}
_LL3310:({ void* _temp3365[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("?", sizeof( unsigned char), 2u), _tag_arr( _temp3365, sizeof( void*),
0u));}); goto _LL3292; _LL3292:;} struct Cyc_List_List* Cyc_Parse_parse_file(
struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({
struct Cyc_Core_Opt* _temp3366=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3366->v=( void*) Cyc_Lexing_from_file( f); _temp3366;});
Cyc_yyparse(); return Cyc_Parse_parse_result;}
