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
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp30= _tag_arr("%s unimplemented",
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
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp155= _tag_arr("%s is not listed as a parameter",
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
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp161= _tag_arr("%s appears in parameter type",
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
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
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
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp652= _tag_arr("bad attribute %s in typedef",
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
struct _tagged_arr(* _temp675)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
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
_temp688)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
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
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp701= _tag_arr("bad attribute %s within typedef",
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
86, 87, 88, 89, 90, 91}; static unsigned char _temp1257[ 2u]="$"; static
unsigned char _temp1258[ 6u]="error"; static unsigned char _temp1259[ 12u]="$undefined.";
static unsigned char _temp1260[ 5u]="AUTO"; static unsigned char _temp1261[ 9u]="REGISTER";
static unsigned char _temp1262[ 7u]="STATIC"; static unsigned char _temp1263[ 7u]="EXTERN";
static unsigned char _temp1264[ 8u]="TYPEDEF"; static unsigned char _temp1265[ 5u]="VOID";
static unsigned char _temp1266[ 5u]="CHAR"; static unsigned char _temp1267[ 6u]="SHORT";
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
static unsigned char _temp1311[ 7u]="MALLOC"; static unsigned char _temp1312[ 9u]="REGION_T";
static unsigned char _temp1313[ 7u]="REGION"; static unsigned char _temp1314[ 5u]="RNEW";
static unsigned char _temp1315[ 8u]="RMALLOC"; static unsigned char _temp1316[ 7u]="PTR_OP";
static unsigned char _temp1317[ 7u]="INC_OP"; static unsigned char _temp1318[ 7u]="DEC_OP";
static unsigned char _temp1319[ 8u]="LEFT_OP"; static unsigned char _temp1320[ 9u]="RIGHT_OP";
static unsigned char _temp1321[ 6u]="LE_OP"; static unsigned char _temp1322[ 6u]="GE_OP";
static unsigned char _temp1323[ 6u]="EQ_OP"; static unsigned char _temp1324[ 6u]="NE_OP";
static unsigned char _temp1325[ 7u]="AND_OP"; static unsigned char _temp1326[ 6u]="OR_OP";
static unsigned char _temp1327[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1328[ 11u]="DIV_ASSIGN"; static unsigned char _temp1329[ 11u]="MOD_ASSIGN";
static unsigned char _temp1330[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1331[ 11u]="SUB_ASSIGN"; static unsigned char _temp1332[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1333[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1334[ 11u]="AND_ASSIGN"; static unsigned char _temp1335[ 11u]="XOR_ASSIGN";
static unsigned char _temp1336[ 10u]="OR_ASSIGN"; static unsigned char _temp1337[
9u]="ELLIPSIS"; static unsigned char _temp1338[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1339[ 12u]="COLON_COLON"; static unsigned char _temp1340[ 11u]="IDENTIFIER";
static unsigned char _temp1341[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1342[ 7u]="STRING"; static unsigned char _temp1343[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1344[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1345[ 9u]="TYPE_VAR"; static unsigned char _temp1346[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1347[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1348[ 10u]="ATTRIBUTE"; static unsigned char _temp1349[ 4u]="';'"; static
unsigned char _temp1350[ 4u]="'{'"; static unsigned char _temp1351[ 4u]="'}'";
static unsigned char _temp1352[ 4u]="'='"; static unsigned char _temp1353[ 4u]="'('";
static unsigned char _temp1354[ 4u]="')'"; static unsigned char _temp1355[ 4u]="','";
static unsigned char _temp1356[ 4u]="'_'"; static unsigned char _temp1357[ 4u]="'$'";
static unsigned char _temp1358[ 4u]="'<'"; static unsigned char _temp1359[ 4u]="'>'";
static unsigned char _temp1360[ 4u]="':'"; static unsigned char _temp1361[ 4u]="'.'";
static unsigned char _temp1362[ 4u]="'['"; static unsigned char _temp1363[ 4u]="']'";
static unsigned char _temp1364[ 4u]="'*'"; static unsigned char _temp1365[ 4u]="'@'";
static unsigned char _temp1366[ 4u]="'?'"; static unsigned char _temp1367[ 4u]="'+'";
static unsigned char _temp1368[ 4u]="'-'"; static unsigned char _temp1369[ 4u]="'&'";
static unsigned char _temp1370[ 4u]="'|'"; static unsigned char _temp1371[ 4u]="'^'";
static unsigned char _temp1372[ 4u]="'/'"; static unsigned char _temp1373[ 4u]="'%'";
static unsigned char _temp1374[ 4u]="'~'"; static unsigned char _temp1375[ 4u]="'!'";
static unsigned char _temp1376[ 5u]="prog"; static unsigned char _temp1377[ 17u]="translation_unit";
static unsigned char _temp1378[ 21u]="external_declaration"; static
unsigned char _temp1379[ 20u]="function_definition"; static unsigned char
_temp1380[ 21u]="function_definition2"; static unsigned char _temp1381[ 13u]="using_action";
static unsigned char _temp1382[ 15u]="unusing_action"; static unsigned char
_temp1383[ 17u]="namespace_action"; static unsigned char _temp1384[ 19u]="unnamespace_action";
static unsigned char _temp1385[ 12u]="declaration"; static unsigned char
_temp1386[ 17u]="declaration_list"; static unsigned char _temp1387[ 23u]="declaration_specifiers";
static unsigned char _temp1388[ 24u]="storage_class_specifier"; static
unsigned char _temp1389[ 15u]="attributes_opt"; static unsigned char _temp1390[
11u]="attributes"; static unsigned char _temp1391[ 15u]="attribute_list"; static
unsigned char _temp1392[ 10u]="attribute"; static unsigned char _temp1393[ 15u]="type_specifier";
static unsigned char _temp1394[ 5u]="kind"; static unsigned char _temp1395[ 15u]="type_qualifier";
static unsigned char _temp1396[ 15u]="enum_specifier"; static unsigned char
_temp1397[ 11u]="enum_field"; static unsigned char _temp1398[ 22u]="enum_declaration_list";
static unsigned char _temp1399[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1400[ 16u]="type_params_opt"; static unsigned char _temp1401[
16u]="struct_or_union"; static unsigned char _temp1402[ 24u]="struct_declaration_list";
static unsigned char _temp1403[ 25u]="struct_declaration_list0"; static
unsigned char _temp1404[ 21u]="init_declarator_list"; static unsigned char
_temp1405[ 22u]="init_declarator_list0"; static unsigned char _temp1406[ 16u]="init_declarator";
static unsigned char _temp1407[ 19u]="struct_declaration"; static unsigned char
_temp1408[ 25u]="specifier_qualifier_list"; static unsigned char _temp1409[ 23u]="struct_declarator_list";
static unsigned char _temp1410[ 24u]="struct_declarator_list0"; static
unsigned char _temp1411[ 18u]="struct_declarator"; static unsigned char
_temp1412[ 17u]="tunion_specifier"; static unsigned char _temp1413[ 18u]="tunion_or_xtunion";
static unsigned char _temp1414[ 17u]="tunionfield_list"; static unsigned char
_temp1415[ 18u]="tunionfield_scope"; static unsigned char _temp1416[ 12u]="tunionfield";
static unsigned char _temp1417[ 11u]="declarator"; static unsigned char
_temp1418[ 18u]="direct_declarator"; static unsigned char _temp1419[ 8u]="pointer";
static unsigned char _temp1420[ 13u]="pointer_char"; static unsigned char
_temp1421[ 8u]="rgn_opt"; static unsigned char _temp1422[ 4u]="rgn"; static
unsigned char _temp1423[ 20u]="type_qualifier_list"; static unsigned char
_temp1424[ 20u]="parameter_type_list"; static unsigned char _temp1425[ 16u]="optional_effect";
static unsigned char _temp1426[ 19u]="optional_rgn_order"; static unsigned char
_temp1427[ 10u]="rgn_order"; static unsigned char _temp1428[ 16u]="optional_inject";
static unsigned char _temp1429[ 11u]="effect_set"; static unsigned char
_temp1430[ 14u]="atomic_effect"; static unsigned char _temp1431[ 11u]="region_set";
static unsigned char _temp1432[ 15u]="parameter_list"; static unsigned char
_temp1433[ 22u]="parameter_declaration"; static unsigned char _temp1434[ 16u]="identifier_list";
static unsigned char _temp1435[ 17u]="identifier_list0"; static unsigned char
_temp1436[ 12u]="initializer"; static unsigned char _temp1437[ 18u]="array_initializer";
static unsigned char _temp1438[ 17u]="initializer_list"; static unsigned char
_temp1439[ 12u]="designation"; static unsigned char _temp1440[ 16u]="designator_list";
static unsigned char _temp1441[ 11u]="designator"; static unsigned char
_temp1442[ 10u]="type_name"; static unsigned char _temp1443[ 14u]="any_type_name";
static unsigned char _temp1444[ 15u]="type_name_list"; static unsigned char
_temp1445[ 20u]="abstract_declarator"; static unsigned char _temp1446[ 27u]="direct_abstract_declarator";
static unsigned char _temp1447[ 10u]="statement"; static unsigned char _temp1448[
18u]="labeled_statement"; static unsigned char _temp1449[ 21u]="expression_statement";
static unsigned char _temp1450[ 19u]="compound_statement"; static unsigned char
_temp1451[ 16u]="block_item_list"; static unsigned char _temp1452[ 11u]="block_item";
static unsigned char _temp1453[ 20u]="selection_statement"; static unsigned char
_temp1454[ 15u]="switch_clauses"; static unsigned char _temp1455[ 16u]="switchC_clauses";
static unsigned char _temp1456[ 20u]="iteration_statement"; static unsigned char
_temp1457[ 15u]="jump_statement"; static unsigned char _temp1458[ 8u]="pattern";
static unsigned char _temp1459[ 19u]="tuple_pattern_list"; static unsigned char
_temp1460[ 20u]="tuple_pattern_list0"; static unsigned char _temp1461[ 14u]="field_pattern";
static unsigned char _temp1462[ 19u]="field_pattern_list"; static unsigned char
_temp1463[ 20u]="field_pattern_list0"; static unsigned char _temp1464[ 11u]="expression";
static unsigned char _temp1465[ 22u]="assignment_expression"; static
unsigned char _temp1466[ 20u]="assignment_operator"; static unsigned char
_temp1467[ 23u]="conditional_expression"; static unsigned char _temp1468[ 20u]="constant_expression";
static unsigned char _temp1469[ 22u]="logical_or_expression"; static
unsigned char _temp1470[ 23u]="logical_and_expression"; static unsigned char
_temp1471[ 24u]="inclusive_or_expression"; static unsigned char _temp1472[ 24u]="exclusive_or_expression";
static unsigned char _temp1473[ 15u]="and_expression"; static unsigned char
_temp1474[ 20u]="equality_expression"; static unsigned char _temp1475[ 22u]="relational_expression";
static unsigned char _temp1476[ 17u]="shift_expression"; static unsigned char
_temp1477[ 20u]="additive_expression"; static unsigned char _temp1478[ 26u]="multiplicative_expression";
static unsigned char _temp1479[ 16u]="cast_expression"; static unsigned char
_temp1480[ 17u]="unary_expression"; static unsigned char _temp1481[ 15u]="unary_operator";
static unsigned char _temp1482[ 19u]="postfix_expression"; static unsigned char
_temp1483[ 19u]="primary_expression"; static unsigned char _temp1484[ 25u]="argument_expression_list";
static unsigned char _temp1485[ 26u]="argument_expression_list0"; static
unsigned char _temp1486[ 9u]="constant"; static unsigned char _temp1487[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 231u]={{ _temp1257, _temp1257, _temp1257
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
+ 7u},{ _temp1312, _temp1312, _temp1312 + 9u},{ _temp1313, _temp1313, _temp1313
+ 7u},{ _temp1314, _temp1314, _temp1314 + 5u},{ _temp1315, _temp1315, _temp1315
+ 8u},{ _temp1316, _temp1316, _temp1316 + 7u},{ _temp1317, _temp1317, _temp1317
+ 7u},{ _temp1318, _temp1318, _temp1318 + 7u},{ _temp1319, _temp1319, _temp1319
+ 8u},{ _temp1320, _temp1320, _temp1320 + 9u},{ _temp1321, _temp1321, _temp1321
+ 6u},{ _temp1322, _temp1322, _temp1322 + 6u},{ _temp1323, _temp1323, _temp1323
+ 6u},{ _temp1324, _temp1324, _temp1324 + 6u},{ _temp1325, _temp1325, _temp1325
+ 7u},{ _temp1326, _temp1326, _temp1326 + 6u},{ _temp1327, _temp1327, _temp1327
+ 11u},{ _temp1328, _temp1328, _temp1328 + 11u},{ _temp1329, _temp1329,
_temp1329 + 11u},{ _temp1330, _temp1330, _temp1330 + 11u},{ _temp1331, _temp1331,
_temp1331 + 11u},{ _temp1332, _temp1332, _temp1332 + 12u},{ _temp1333, _temp1333,
_temp1333 + 13u},{ _temp1334, _temp1334, _temp1334 + 11u},{ _temp1335, _temp1335,
_temp1335 + 11u},{ _temp1336, _temp1336, _temp1336 + 10u},{ _temp1337, _temp1337,
_temp1337 + 9u},{ _temp1338, _temp1338, _temp1338 + 11u},{ _temp1339, _temp1339,
_temp1339 + 12u},{ _temp1340, _temp1340, _temp1340 + 11u},{ _temp1341, _temp1341,
_temp1341 + 17u},{ _temp1342, _temp1342, _temp1342 + 7u},{ _temp1343, _temp1343,
_temp1343 + 19u},{ _temp1344, _temp1344, _temp1344 + 18u},{ _temp1345, _temp1345,
_temp1345 + 9u},{ _temp1346, _temp1346, _temp1346 + 16u},{ _temp1347, _temp1347,
_temp1347 + 18u},{ _temp1348, _temp1348, _temp1348 + 10u},{ _temp1349, _temp1349,
_temp1349 + 4u},{ _temp1350, _temp1350, _temp1350 + 4u},{ _temp1351, _temp1351,
_temp1351 + 4u},{ _temp1352, _temp1352, _temp1352 + 4u},{ _temp1353, _temp1353,
_temp1353 + 4u},{ _temp1354, _temp1354, _temp1354 + 4u},{ _temp1355, _temp1355,
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
_temp1375 + 4u},{ _temp1376, _temp1376, _temp1376 + 5u},{ _temp1377, _temp1377,
_temp1377 + 17u},{ _temp1378, _temp1378, _temp1378 + 21u},{ _temp1379, _temp1379,
_temp1379 + 20u},{ _temp1380, _temp1380, _temp1380 + 21u},{ _temp1381, _temp1381,
_temp1381 + 13u},{ _temp1382, _temp1382, _temp1382 + 15u},{ _temp1383, _temp1383,
_temp1383 + 17u},{ _temp1384, _temp1384, _temp1384 + 19u},{ _temp1385, _temp1385,
_temp1385 + 12u},{ _temp1386, _temp1386, _temp1386 + 17u},{ _temp1387, _temp1387,
_temp1387 + 23u},{ _temp1388, _temp1388, _temp1388 + 24u},{ _temp1389, _temp1389,
_temp1389 + 15u},{ _temp1390, _temp1390, _temp1390 + 11u},{ _temp1391, _temp1391,
_temp1391 + 15u},{ _temp1392, _temp1392, _temp1392 + 10u},{ _temp1393, _temp1393,
_temp1393 + 15u},{ _temp1394, _temp1394, _temp1394 + 5u},{ _temp1395, _temp1395,
_temp1395 + 15u},{ _temp1396, _temp1396, _temp1396 + 15u},{ _temp1397, _temp1397,
_temp1397 + 11u},{ _temp1398, _temp1398, _temp1398 + 22u},{ _temp1399, _temp1399,
_temp1399 + 26u},{ _temp1400, _temp1400, _temp1400 + 16u},{ _temp1401, _temp1401,
_temp1401 + 16u},{ _temp1402, _temp1402, _temp1402 + 24u},{ _temp1403, _temp1403,
_temp1403 + 25u},{ _temp1404, _temp1404, _temp1404 + 21u},{ _temp1405, _temp1405,
_temp1405 + 22u},{ _temp1406, _temp1406, _temp1406 + 16u},{ _temp1407, _temp1407,
_temp1407 + 19u},{ _temp1408, _temp1408, _temp1408 + 25u},{ _temp1409, _temp1409,
_temp1409 + 23u},{ _temp1410, _temp1410, _temp1410 + 24u},{ _temp1411, _temp1411,
_temp1411 + 18u},{ _temp1412, _temp1412, _temp1412 + 17u},{ _temp1413, _temp1413,
_temp1413 + 18u},{ _temp1414, _temp1414, _temp1414 + 17u},{ _temp1415, _temp1415,
_temp1415 + 18u},{ _temp1416, _temp1416, _temp1416 + 12u},{ _temp1417, _temp1417,
_temp1417 + 11u},{ _temp1418, _temp1418, _temp1418 + 18u},{ _temp1419, _temp1419,
_temp1419 + 8u},{ _temp1420, _temp1420, _temp1420 + 13u},{ _temp1421, _temp1421,
_temp1421 + 8u},{ _temp1422, _temp1422, _temp1422 + 4u},{ _temp1423, _temp1423,
_temp1423 + 20u},{ _temp1424, _temp1424, _temp1424 + 20u},{ _temp1425, _temp1425,
_temp1425 + 16u},{ _temp1426, _temp1426, _temp1426 + 19u},{ _temp1427, _temp1427,
_temp1427 + 10u},{ _temp1428, _temp1428, _temp1428 + 16u},{ _temp1429, _temp1429,
_temp1429 + 11u},{ _temp1430, _temp1430, _temp1430 + 14u},{ _temp1431, _temp1431,
_temp1431 + 11u},{ _temp1432, _temp1432, _temp1432 + 15u},{ _temp1433, _temp1433,
_temp1433 + 22u},{ _temp1434, _temp1434, _temp1434 + 16u},{ _temp1435, _temp1435,
_temp1435 + 17u},{ _temp1436, _temp1436, _temp1436 + 12u},{ _temp1437, _temp1437,
_temp1437 + 18u},{ _temp1438, _temp1438, _temp1438 + 17u},{ _temp1439, _temp1439,
_temp1439 + 12u},{ _temp1440, _temp1440, _temp1440 + 16u},{ _temp1441, _temp1441,
_temp1441 + 11u},{ _temp1442, _temp1442, _temp1442 + 10u},{ _temp1443, _temp1443,
_temp1443 + 14u},{ _temp1444, _temp1444, _temp1444 + 15u},{ _temp1445, _temp1445,
_temp1445 + 20u},{ _temp1446, _temp1446, _temp1446 + 27u},{ _temp1447, _temp1447,
_temp1447 + 10u},{ _temp1448, _temp1448, _temp1448 + 18u},{ _temp1449, _temp1449,
_temp1449 + 21u},{ _temp1450, _temp1450, _temp1450 + 19u},{ _temp1451, _temp1451,
_temp1451 + 16u},{ _temp1452, _temp1452, _temp1452 + 11u},{ _temp1453, _temp1453,
_temp1453 + 20u},{ _temp1454, _temp1454, _temp1454 + 15u},{ _temp1455, _temp1455,
_temp1455 + 16u},{ _temp1456, _temp1456, _temp1456 + 20u},{ _temp1457, _temp1457,
_temp1457 + 15u},{ _temp1458, _temp1458, _temp1458 + 8u},{ _temp1459, _temp1459,
_temp1459 + 19u},{ _temp1460, _temp1460, _temp1460 + 20u},{ _temp1461, _temp1461,
_temp1461 + 14u},{ _temp1462, _temp1462, _temp1462 + 19u},{ _temp1463, _temp1463,
_temp1463 + 20u},{ _temp1464, _temp1464, _temp1464 + 11u},{ _temp1465, _temp1465,
_temp1465 + 22u},{ _temp1466, _temp1466, _temp1466 + 20u},{ _temp1467, _temp1467,
_temp1467 + 23u},{ _temp1468, _temp1468, _temp1468 + 20u},{ _temp1469, _temp1469,
_temp1469 + 22u},{ _temp1470, _temp1470, _temp1470 + 23u},{ _temp1471, _temp1471,
_temp1471 + 24u},{ _temp1472, _temp1472, _temp1472 + 24u},{ _temp1473, _temp1473,
_temp1473 + 15u},{ _temp1474, _temp1474, _temp1474 + 20u},{ _temp1475, _temp1475,
_temp1475 + 22u},{ _temp1476, _temp1476, _temp1476 + 17u},{ _temp1477, _temp1477,
_temp1477 + 20u},{ _temp1478, _temp1478, _temp1478 + 26u},{ _temp1479, _temp1479,
_temp1479 + 16u},{ _temp1480, _temp1480, _temp1480 + 17u},{ _temp1481, _temp1481,
_temp1481 + 15u},{ _temp1482, _temp1482, _temp1482 + 19u},{ _temp1483, _temp1483,
_temp1483 + 19u},{ _temp1484, _temp1484, _temp1484 + 25u},{ _temp1485, _temp1485,
_temp1485 + 26u},{ _temp1486, _temp1486, _temp1486 + 9u},{ _temp1487, _temp1487,
_temp1487 + 20u}}; static short Cyc_yyr1[ 403u]={ 0, 119, 120, 120, 120, 120,
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
yyss=( short*)({ unsigned int _temp3336= 10000u; short* _temp3337=( short*)
GC_malloc_atomic( sizeof( short) * _temp3336);{ unsigned int _temp3338=
_temp3336; unsigned int i; for( i= 0; i < _temp3338; i ++){ _temp3337[ i]=(
short) 0;}}; _temp3337;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3333= 10000u; void** _temp3334=( void**) GC_malloc( sizeof(
void*) * _temp3333);{ unsigned int _temp3335= _temp3333; unsigned int i; for( i=
0; i < _temp3335; i ++){ _temp3334[ i]= Cyc_yylval;}}; _temp3334;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3330= 10000u; struct Cyc_Yyltype* _temp3331=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) * _temp3330);{ unsigned int
_temp3332= _temp3330; unsigned int i; for( i= 0; i < _temp3332; i ++){ _temp3331[
i]= Cyc_yynewloc();}}; _temp3331;}); int yystacksize= 10000; void* yyval= Cyc_yylval;
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
yyn){ case 1: _LL1488: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1489:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1491=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1491[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1492; _temp1492.tag= Cyc_DeclList_tok; _temp1492.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1492;});
_temp1491;}); break; case 3: _LL1490: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1494=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1494[ 0]=({ struct Cyc_DeclList_tok_struct _temp1495; _temp1495.tag= Cyc_DeclList_tok;
_temp1495.f1=({ struct Cyc_List_List* _temp1496=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1496->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1497=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1497->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1498=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1498[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1499; _temp1499.tag= Cyc_Absyn_Using_d;
_temp1499.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1499.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1499;}); _temp1498;}));
_temp1497->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1497;});
_temp1496->tl= 0; _temp1496;}); _temp1495;}); _temp1494;}); Cyc_Lex_leave_using();
break; case 4: _LL1493: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1501=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1501[ 0]=({ struct Cyc_DeclList_tok_struct _temp1502; _temp1502.tag= Cyc_DeclList_tok;
_temp1502.f1=({ struct Cyc_List_List* _temp1503=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1503->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1504=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1504->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1505=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1505[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1506; _temp1506.tag= Cyc_Absyn_Using_d;
_temp1506.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1506.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1506;});
_temp1505;})); _temp1504->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1504;}); _temp1503->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1503;}); _temp1502;});
_temp1501;}); break; case 5: _LL1500: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1508=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1508[ 0]=({ struct Cyc_DeclList_tok_struct _temp1509; _temp1509.tag= Cyc_DeclList_tok;
_temp1509.f1=({ struct Cyc_List_List* _temp1510=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1510->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1511=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1511->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1512=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1512[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1513; _temp1513.tag=
Cyc_Absyn_Namespace_d; _temp1513.f1=({ struct _tagged_arr* _temp1514=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1514[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1514;});
_temp1513.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1513;}); _temp1512;})); _temp1511->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1511;}); _temp1510->tl= 0; _temp1510;}); _temp1509;}); _temp1508;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1507: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1516=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1516[ 0]=({ struct Cyc_DeclList_tok_struct _temp1517; _temp1517.tag= Cyc_DeclList_tok;
_temp1517.f1=({ struct Cyc_List_List* _temp1518=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1518->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1519=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1519->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1520=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1520[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1521; _temp1521.tag=
Cyc_Absyn_Namespace_d; _temp1521.f1=({ struct _tagged_arr* _temp1522=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1522[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1522;});
_temp1521.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1521;}); _temp1520;})); _temp1519->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1519;}); _temp1518->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1518;}); _temp1517;});
_temp1516;}); break; case 7: _LL1515: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1524=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1524[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1525; _temp1525.tag= Cyc_DeclList_tok; _temp1525.f1=({ struct Cyc_List_List*
_temp1526=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1526->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1527=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1527->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1528=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1528[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1529; _temp1529.tag= Cyc_Absyn_ExternC_d;
_temp1529.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1529;}); _temp1528;})); _temp1527->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1527;}); _temp1526->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1526;}); _temp1525;});
_temp1524;}); break; case 8: _LL1523: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1531=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1531[ 0]=({ struct Cyc_DeclList_tok_struct _temp1532; _temp1532.tag= Cyc_DeclList_tok;
_temp1532.f1= 0; _temp1532;}); _temp1531;}); break; case 9: _LL1530: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1534=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1534[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1535; _temp1535.tag= Cyc_DeclList_tok; _temp1535.f1=({ struct Cyc_List_List*
_temp1536=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1536->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1537=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1537[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1538; _temp1538.tag= Cyc_Absyn_Fn_d;
_temp1538.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1538;}); _temp1537;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1536->tl=
0; _temp1536;}); _temp1535;}); _temp1534;}); break; case 10: _LL1533: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1539: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1541=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1541[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1542; _temp1542.tag= Cyc_FnDecl_tok; _temp1542.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1542;});
_temp1541;}); break; case 12: _LL1540: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1544=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1544[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1545; _temp1545.tag= Cyc_FnDecl_tok;
_temp1545.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1546=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1546->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1546;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1545;}); _temp1544;}); break; case 13: _LL1543: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1548=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1548[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1549; _temp1549.tag= Cyc_FnDecl_tok;
_temp1549.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1549;}); _temp1548;}); break; case 14: _LL1547: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1551=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1551[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1552; _temp1552.tag= Cyc_FnDecl_tok;
_temp1552.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1553=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1553->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1553;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1552;}); _temp1551;}); break; case 15: _LL1550: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1555=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1555[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1556; _temp1556.tag= Cyc_FnDecl_tok;
_temp1556.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1557=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1557->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1557;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1556;}); _temp1555;}); break; case 16: _LL1554: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1559=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1559[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1560; _temp1560.tag= Cyc_FnDecl_tok;
_temp1560.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1561=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1561->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1561;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1560;}); _temp1559;}); break; case 17: _LL1558: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1562:
Cyc_Lex_leave_using(); break; case 19: _LL1563: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1565=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1565[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1565;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1564: Cyc_Lex_leave_namespace(); break; case 21: _LL1566: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1568=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1568[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1569; _temp1569.tag= Cyc_DeclList_tok; _temp1569.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1569;}); _temp1568;}); break; case 22: _LL1567: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1571=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1571[ 0]=({ struct Cyc_DeclList_tok_struct _temp1572; _temp1572.tag= Cyc_DeclList_tok;
_temp1572.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1572;}); _temp1571;}); break; case 23: _LL1570: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1574=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1574[ 0]=({ struct Cyc_DeclList_tok_struct _temp1575; _temp1575.tag= Cyc_DeclList_tok;
_temp1575.f1=({ struct Cyc_List_List* _temp1576=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1576->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1576->tl=
0; _temp1576;}); _temp1575;}); _temp1574;}); break; case 24: _LL1573: { struct
Cyc_List_List* _temp1578= 0;{ struct Cyc_List_List* _temp1579= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1579 != 0; _temp1579=(( struct Cyc_List_List*) _check_null( _temp1579))->tl){
struct _tagged_arr* _temp1580=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1579))->hd; struct _tuple1* _temp1581=({ struct _tuple1*
_temp1584=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1584->f1=
Cyc_Absyn_rel_ns_null; _temp1584->f2= _temp1580; _temp1584;}); struct Cyc_Absyn_Vardecl*
_temp1582= Cyc_Absyn_new_vardecl( _temp1581, Cyc_Absyn_wildtyp( 0), 0);
_temp1578=({ struct Cyc_List_List* _temp1583=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1583->hd=( void*) _temp1582; _temp1583->tl=
_temp1578; _temp1583;});}} _temp1578=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1578); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1585=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1585[ 0]=({ struct Cyc_DeclList_tok_struct _temp1586; _temp1586.tag= Cyc_DeclList_tok;
_temp1586.f1=({ struct Cyc_List_List* _temp1587=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1587->hd=( void*) Cyc_Absyn_letv_decl(
_temp1578, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1587->tl= 0; _temp1587;}); _temp1586;});
_temp1585;}); break;} case 25: _LL1577: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1588:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1590=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1590[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1591; _temp1591.tag= Cyc_DeclList_tok; _temp1591.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1591;});
_temp1590;}); break; case 27: _LL1589: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1593=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1593[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1594; _temp1594.tag= Cyc_DeclSpec_tok;
_temp1594.f1=({ struct Cyc_Parse_Declaration_spec* _temp1595=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1595->sc=({ struct
Cyc_Core_Opt* _temp1596=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1596->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1596;});
_temp1595->tq= Cyc_Absyn_empty_tqual(); _temp1595->type_specs= 0; _temp1595->is_inline=
0; _temp1595->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1595;}); _temp1594;});
_temp1593;}); break; case 28: _LL1592: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1598=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1598[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1599; _temp1599.tag= Cyc_DeclSpec_tok; _temp1599.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1600=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1600->sc=({ struct Cyc_Core_Opt* _temp1601=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1601->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1601;});
_temp1600->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1600->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1600->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1600->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1600;}); _temp1599;}); _temp1598;}); break; case 29: _LL1597: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1603=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1603[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1604; _temp1604.tag= Cyc_DeclSpec_tok; _temp1604.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1605=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1605->sc= 0; _temp1605->tq= Cyc_Absyn_empty_tqual(); _temp1605->type_specs=({
struct Cyc_List_List* _temp1606=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1606->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1606->tl= 0;
_temp1606;}); _temp1605->is_inline= 0; _temp1605->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1605;});
_temp1604;}); _temp1603;}); break; case 30: _LL1602: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1608=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1608[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1609; _temp1609.tag= Cyc_DeclSpec_tok;
_temp1609.f1=({ struct Cyc_Parse_Declaration_spec* _temp1610=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1610->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1610->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1610->type_specs=({ struct Cyc_List_List* _temp1611=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1611->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1611->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1611;});
_temp1610->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1610->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1610;}); _temp1609;}); _temp1608;}); break; case 31: _LL1607: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1613=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1613[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1614; _temp1614.tag= Cyc_DeclSpec_tok; _temp1614.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1615=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1615->sc= 0; _temp1615->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1615->type_specs=
0; _temp1615->is_inline= 0; _temp1615->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1615;});
_temp1614;}); _temp1613;}); break; case 32: _LL1612: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1617=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1617[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1618; _temp1618.tag= Cyc_DeclSpec_tok;
_temp1618.f1=({ struct Cyc_Parse_Declaration_spec* _temp1619=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1619->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1619->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1619->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1619->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1619->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1619;}); _temp1618;}); _temp1617;}); break; case 33: _LL1616: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1621=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1621[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1622; _temp1622.tag= Cyc_DeclSpec_tok; _temp1622.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1623=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1623->sc= 0; _temp1623->tq= Cyc_Absyn_empty_tqual(); _temp1623->type_specs=
0; _temp1623->is_inline= 1; _temp1623->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1623;});
_temp1622;}); _temp1621;}); break; case 34: _LL1620: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1625=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1625[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1626; _temp1626.tag= Cyc_DeclSpec_tok;
_temp1626.f1=({ struct Cyc_Parse_Declaration_spec* _temp1627=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1627->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1627->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1627->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1627->is_inline=
1; _temp1627->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1627;}); _temp1626;}); _temp1625;}); break; case 35: _LL1624: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1629=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1629[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1630; _temp1630.tag= Cyc_StorageClass_tok;
_temp1630.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1630;}); _temp1629;});
break; case 36: _LL1628: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1632=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1632[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1633; _temp1633.tag=
Cyc_StorageClass_tok; _temp1633.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1633;}); _temp1632;}); break; case 37: _LL1631: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1635=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1635[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1636; _temp1636.tag=
Cyc_StorageClass_tok; _temp1636.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1636;}); _temp1635;}); break; case 38: _LL1634: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1638=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1638[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1639; _temp1639.tag=
Cyc_StorageClass_tok; _temp1639.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1639;}); _temp1638;}); break; case 39: _LL1637: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1641=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1641[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1642; _temp1642.tag= Cyc_StorageClass_tok;
_temp1642.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1642;}); _temp1641;});
break; case 40: _LL1640: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1644=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1644[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1645; _temp1645.tag=
Cyc_StorageClass_tok; _temp1645.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1645;}); _temp1644;}); break; case 41: _LL1643: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1647=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1647[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1648; _temp1648.tag=
Cyc_StorageClass_tok; _temp1648.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1648;}); _temp1647;}); break; case 42: _LL1646: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1650=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1650[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1651; _temp1651.tag=
Cyc_AttributeList_tok; _temp1651.f1= 0; _temp1651;}); _temp1650;}); break; case
43: _LL1649: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1652: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1654=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1654[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1655; _temp1655.tag=
Cyc_AttributeList_tok; _temp1655.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1655;});
_temp1654;}); break; case 45: _LL1653: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1657=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1657[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1658; _temp1658.tag=
Cyc_AttributeList_tok; _temp1658.f1=({ struct Cyc_List_List* _temp1659=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1659->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1659->tl= 0; _temp1659;}); _temp1658;}); _temp1657;});
break; case 46: _LL1656: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1661=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1661[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1662; _temp1662.tag=
Cyc_AttributeList_tok; _temp1662.f1=({ struct Cyc_List_List* _temp1663=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1663->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1663->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1663;}); _temp1662;});
_temp1661;}); break; case 47: _LL1660: { struct _tagged_arr _temp1665= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1665, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1665, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1665, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1665,
sizeof( unsigned char),( int)( _get_arr_size( _temp1665, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1665,
sizeof( unsigned char),( int)( _get_arr_size( _temp1665, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1665=( struct _tagged_arr) Cyc_String_substring(
_temp1665, 2,( int)( _get_arr_size( _temp1665, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1665,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1666=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1666[ 0]=({ struct Cyc_Attribute_tok_struct _temp1667; _temp1667.tag= Cyc_Attribute_tok;
_temp1667.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1667;}); _temp1666;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1668=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1668[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1669; _temp1669.tag= Cyc_Attribute_tok; _temp1669.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1669;}); _temp1668;});} break;}} case
48: _LL1664: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1671=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1671[ 0]=({ struct Cyc_Attribute_tok_struct _temp1672; _temp1672.tag= Cyc_Attribute_tok;
_temp1672.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1672;}); _temp1671;});
break; case 49: _LL1670: { struct _tagged_arr _temp1674= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1677; int _temp1678; struct _tuple16* _temp1675= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1677=*
_temp1675; _LL1679: _temp1678= _temp1677.f2; goto _LL1676; _LL1676: { void* a;
if( Cyc_String_zstrcmp( _temp1674, _tag_arr("regparm", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1674, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) == 0){ if( _temp1678 <= 0? 1: _temp1678 > 3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1680=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1680[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1681; _temp1681.tag= Cyc_Absyn_Regparm_att; _temp1681.f1= _temp1678;
_temp1681;}); _temp1680;});} else{ if( Cyc_String_zstrcmp( _temp1674, _tag_arr("aligned",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1674, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1678 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1678; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1682=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1682[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1683; _temp1683.tag= Cyc_Absyn_Aligned_att;
_temp1683.f1= _temp1678; _temp1683;}); _temp1682;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1684=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1684[ 0]=({ struct Cyc_Attribute_tok_struct _temp1685; _temp1685.tag= Cyc_Attribute_tok;
_temp1685.f1=( void*) a; _temp1685;}); _temp1684;}); break;}} case 50: _LL1673: {
struct _tagged_arr _temp1687= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1688= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1687, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1687, _tag_arr("__section__",
sizeof( unsigned char), 12u)) == 0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1689=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1689[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1690; _temp1690.tag=
Cyc_Absyn_Section_att; _temp1690.f1= _temp1688; _temp1690;}); _temp1689;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1691=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1691[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1692; _temp1692.tag= Cyc_Attribute_tok; _temp1692.f1=(
void*) a; _temp1692;}); _temp1691;}); break;} case 51: _LL1686: { struct
_tagged_arr _temp1694= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)]); struct _tagged_arr
_temp1695= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); int _temp1698; struct _tuple16 _temp1696=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1699:
_temp1698= _temp1696.f2; goto _LL1697; _LL1697: { int _temp1702; struct _tuple16
_temp1700=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _LL1703: _temp1702= _temp1700.f2; goto _LL1701; _LL1701: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_String_zstrcmp( _temp1694,
_tag_arr("format", sizeof( unsigned char), 7u)) == 0? 1: Cyc_String_zstrcmp(
_temp1694, _tag_arr("__format__", sizeof( unsigned char), 11u)) == 0){ if( Cyc_String_zstrcmp(
_temp1695, _tag_arr("printf", sizeof( unsigned char), 7u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1704=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1704[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1705; _temp1705.tag= Cyc_Absyn_Format_att;
_temp1705.f1=( void*)(( void*) Cyc_Absyn_Printf_ft); _temp1705.f2= _temp1698;
_temp1705.f3= _temp1702; _temp1705;}); _temp1704;});} else{ if( Cyc_String_zstrcmp(
_temp1695, _tag_arr("scanf", sizeof( unsigned char), 6u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1706=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1706[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1707; _temp1707.tag= Cyc_Absyn_Format_att;
_temp1707.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft); _temp1707.f2= _temp1698;
_temp1707.f3= _temp1702; _temp1707;}); _temp1706;});} else{ Cyc_Parse_err(
_tag_arr("unrecognized format type", sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line));}}}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1708=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1708[ 0]=({ struct Cyc_Attribute_tok_struct _temp1709; _temp1709.tag= Cyc_Attribute_tok;
_temp1709.f1=( void*) a; _temp1709;}); _temp1708;}); break;}}} case 52: _LL1693:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1711=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1711[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1712; _temp1712.tag= Cyc_TypeSpecifier_tok;
_temp1712.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1712;});
_temp1711;}); break; case 53: _LL1710: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1714=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1714[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1715; _temp1715.tag=
Cyc_TypeSpecifier_tok; _temp1715.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1715;}); _temp1714;}); break; case 54: _LL1713:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1717=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1717[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1718; _temp1718.tag= Cyc_TypeSpecifier_tok;
_temp1718.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1718;});
_temp1717;}); break; case 55: _LL1716: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1720=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1720[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1721; _temp1721.tag=
Cyc_TypeSpecifier_tok; _temp1721.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1722=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1722[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1723; _temp1723.tag=
Cyc_Parse_Short_spec; _temp1723.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1723;});
_temp1722;})); _temp1721;}); _temp1720;}); break; case 56: _LL1719: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1725=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1725[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1726; _temp1726.tag= Cyc_TypeSpecifier_tok;
_temp1726.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1726;});
_temp1725;}); break; case 57: _LL1724: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1728=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1728[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1729; _temp1729.tag=
Cyc_TypeSpecifier_tok; _temp1729.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1730=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1730[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1731; _temp1731.tag= Cyc_Parse_Long_spec;
_temp1731.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1731;}); _temp1730;})); _temp1729;}); _temp1728;});
break; case 58: _LL1727: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1733=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1733[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1734; _temp1734.tag=
Cyc_TypeSpecifier_tok; _temp1734.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1734;}); _temp1733;}); break; case 59: _LL1732:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1736=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1736[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1737; _temp1737.tag= Cyc_TypeSpecifier_tok;
_temp1737.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1737;});
_temp1736;}); break; case 60: _LL1735: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1739=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1739[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1740; _temp1740.tag=
Cyc_TypeSpecifier_tok; _temp1740.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1741=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1741[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1742; _temp1742.tag=
Cyc_Parse_Signed_spec; _temp1742.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1742;});
_temp1741;})); _temp1740;}); _temp1739;}); break; case 61: _LL1738: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1744=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1744[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1745; _temp1745.tag= Cyc_TypeSpecifier_tok;
_temp1745.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1746=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1746[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1747; _temp1747.tag= Cyc_Parse_Unsigned_spec; _temp1747.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1747;});
_temp1746;})); _temp1745;}); _temp1744;}); break; case 62: _LL1743: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1748:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1749: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1750: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1752=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1752[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1753; _temp1753.tag=
Cyc_TypeSpecifier_tok; _temp1753.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1753;});
_temp1752;}); break; case 66: _LL1751: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1755=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1755[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1756; _temp1756.tag=
Cyc_TypeSpecifier_tok; _temp1756.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1756;}); _temp1755;}); break; case 67: _LL1754: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1758=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1758[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1759; _temp1759.tag=
Cyc_TypeSpecifier_tok; _temp1759.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1760=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1760[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1761; _temp1761.tag= Cyc_Absyn_TypedefType;
_temp1761.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1761.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1761.f3= 0;
_temp1761;}); _temp1760;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1759;});
_temp1758;}); break; case 68: _LL1757: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1763=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1763[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1764; _temp1764.tag=
Cyc_TypeSpecifier_tok; _temp1764.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1765=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1765[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1766; _temp1766.tag= Cyc_Absyn_TupleType;
_temp1766.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1766;});
_temp1765;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1764;}); _temp1763;}); break; case 69:
_LL1762: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1768=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1768[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1769; _temp1769.tag=
Cyc_TypeSpecifier_tok; _temp1769.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1770=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1770[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1771; _temp1771.tag= Cyc_Absyn_RgnHandleType;
_temp1771.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1771;}); _temp1770;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1769;}); _temp1768;}); break; case 70: _LL1767: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1773=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1773[ 0]=({ struct Cyc_Kind_tok_struct _temp1774; _temp1774.tag= Cyc_Kind_tok;
_temp1774.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1774;});
_temp1773;}); break; case 71: _LL1772: { struct _tagged_arr* _temp1778; void*
_temp1780; struct _tuple1 _temp1776=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1781: _temp1780=
_temp1776.f1; goto _LL1779; _LL1779: _temp1778= _temp1776.f2; goto _LL1777;
_LL1777: if( _temp1780 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1782=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1782[ 0]=({ struct Cyc_Kind_tok_struct
_temp1783; _temp1783.tag= Cyc_Kind_tok; _temp1783.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1778, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1783;}); _temp1782;}); break;} case 72:
_LL1775: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1785=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1785[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1786; _temp1786.tag= Cyc_TypeQual_tok;
_temp1786.f1=({ struct Cyc_Absyn_Tqual _temp1787; _temp1787.q_const= 1;
_temp1787.q_volatile= 0; _temp1787.q_restrict= 0; _temp1787;}); _temp1786;});
_temp1785;}); break; case 73: _LL1784: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1789=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1789[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1790; _temp1790.tag= Cyc_TypeQual_tok; _temp1790.f1=({ struct Cyc_Absyn_Tqual
_temp1791; _temp1791.q_const= 0; _temp1791.q_volatile= 1; _temp1791.q_restrict=
0; _temp1791;}); _temp1790;}); _temp1789;}); break; case 74: _LL1788: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1793=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1793[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1794; _temp1794.tag= Cyc_TypeQual_tok;
_temp1794.f1=({ struct Cyc_Absyn_Tqual _temp1795; _temp1795.q_const= 0;
_temp1795.q_volatile= 0; _temp1795.q_restrict= 1; _temp1795;}); _temp1794;});
_temp1793;}); break; case 75: _LL1792: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1797=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1797[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1798; _temp1798.tag=
Cyc_TypeSpecifier_tok; _temp1798.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1799=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1799[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1800; _temp1800.tag= Cyc_Parse_Decl_spec;
_temp1800.f1=({ struct Cyc_Absyn_Decl* _temp1801=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1801->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1802=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1802[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1803; _temp1803.tag= Cyc_Absyn_Enum_d; _temp1803.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1804=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1804->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1804->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1804->fields=({
struct Cyc_Core_Opt* _temp1805=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1805->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1805;});
_temp1804;}); _temp1803;}); _temp1802;})); _temp1801->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1801;}); _temp1800;}); _temp1799;})); _temp1798;}); _temp1797;}); break;
case 76: _LL1796: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1807=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1807[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1808; _temp1808.tag=
Cyc_TypeSpecifier_tok; _temp1808.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1809=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1809[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1810; _temp1810.tag= Cyc_Absyn_EnumType;
_temp1810.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1810.f2= 0; _temp1810;}); _temp1809;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1808;}); _temp1807;}); break; case 77: _LL1806: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1812=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1812[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1813; _temp1813.tag= Cyc_Enumfield_tok;
_temp1813.f1=({ struct Cyc_Absyn_Enumfield* _temp1814=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1814->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1814->tag= 0;
_temp1814->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1814;});
_temp1813;}); _temp1812;}); break; case 78: _LL1811: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1816=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1816[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1817; _temp1817.tag= Cyc_Enumfield_tok;
_temp1817.f1=({ struct Cyc_Absyn_Enumfield* _temp1818=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1818->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1818->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1818->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1818;});
_temp1817;}); _temp1816;}); break; case 79: _LL1815: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1820=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1820[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1821; _temp1821.tag=
Cyc_EnumfieldList_tok; _temp1821.f1=({ struct Cyc_List_List* _temp1822=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1822->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1822->tl= 0; _temp1822;}); _temp1821;}); _temp1820;});
break; case 80: _LL1819: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1824=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1824[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1825; _temp1825.tag=
Cyc_EnumfieldList_tok; _temp1825.f1=({ struct Cyc_List_List* _temp1826=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1826->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1826->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1826;}); _temp1825;});
_temp1824;}); break; case 81: _LL1823: { void* t;{ void* _temp1828= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1830: if(
_temp1828 ==( void*) Cyc_Parse_Struct_su){ goto _LL1831;} else{ goto _LL1832;}
_LL1832: if( _temp1828 ==( void*) Cyc_Parse_Union_su){ goto _LL1833;} else{ goto
_LL1829;} _LL1831: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1834=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1834[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1835; _temp1835.tag= Cyc_Absyn_AnonStructType; _temp1835.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1835;});
_temp1834;}); goto _LL1829; _LL1833: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1836=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1836[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1837; _temp1837.tag= Cyc_Absyn_AnonUnionType; _temp1837.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1837;});
_temp1836;}); goto _LL1829; _LL1829:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1838=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1838[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1839; _temp1839.tag=
Cyc_TypeSpecifier_tok; _temp1839.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1840=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1840[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1841; _temp1841.tag= Cyc_Parse_Type_spec;
_temp1841.f1=( void*) t; _temp1841.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1841;});
_temp1840;})); _temp1839;}); _temp1838;}); break;} case 82: _LL1827: { struct
Cyc_List_List* _temp1843=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1844= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1846: if(
_temp1844 ==( void*) Cyc_Parse_Struct_su){ goto _LL1847;} else{ goto _LL1848;}
_LL1848: if( _temp1844 ==( void*) Cyc_Parse_Union_su){ goto _LL1849;} else{ goto
_LL1845;} _LL1847: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1850=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1850->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1850;}), _temp1843,({ struct Cyc_Core_Opt*
_temp1851=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1851->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1851;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1845; _LL1849: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1852=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1852->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1852;}),
_temp1843,({ struct Cyc_Core_Opt* _temp1853=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1853->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1853;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1845; _LL1845:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1854=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1854[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1855; _temp1855.tag= Cyc_TypeSpecifier_tok;
_temp1855.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1856=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1856[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1857; _temp1857.tag= Cyc_Parse_Decl_spec;
_temp1857.f1= d; _temp1857;}); _temp1856;})); _temp1855;}); _temp1854;}); break;}
case 83: _LL1842: { struct Cyc_List_List* _temp1859=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1860= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1862: if(
_temp1860 ==( void*) Cyc_Parse_Struct_su){ goto _LL1863;} else{ goto _LL1864;}
_LL1864: if( _temp1860 ==( void*) Cyc_Parse_Union_su){ goto _LL1865;} else{ goto
_LL1861;} _LL1863: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1866=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1866->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1866;}), _temp1859,({ struct Cyc_Core_Opt*
_temp1867=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1867->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1867;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1861; _LL1865: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1868=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1868->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1868;}),
_temp1859,({ struct Cyc_Core_Opt* _temp1869=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1869->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1869;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1861; _LL1861:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1870=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1870[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1871; _temp1871.tag= Cyc_TypeSpecifier_tok;
_temp1871.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1872=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1872[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1873; _temp1873.tag= Cyc_Parse_Decl_spec;
_temp1873.f1= d; _temp1873;}); _temp1872;})); _temp1871;}); _temp1870;}); break;}
case 84: _LL1858:{ void* _temp1875= Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1877: if(
_temp1875 ==( void*) Cyc_Parse_Struct_su){ goto _LL1878;} else{ goto _LL1879;}
_LL1879: if( _temp1875 ==( void*) Cyc_Parse_Union_su){ goto _LL1880;} else{ goto
_LL1876;} _LL1878: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1881=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1881[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1882; _temp1882.tag=
Cyc_TypeSpecifier_tok; _temp1882.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1883=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1883[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1884; _temp1884.tag= Cyc_Absyn_StructType;
_temp1884.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1884.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1884.f3= 0;
_temp1884;}); _temp1883;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1882;});
_temp1881;}); goto _LL1876; _LL1880: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1885=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1885[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1886; _temp1886.tag=
Cyc_TypeSpecifier_tok; _temp1886.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1887=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1887[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1888; _temp1888.tag= Cyc_Absyn_UnionType;
_temp1888.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1888.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1888.f3= 0;
_temp1888;}); _temp1887;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1886;});
_temp1885;}); goto _LL1876; _LL1876:;} break; case 85: _LL1874:{ void* _temp1890=
Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL1892: if( _temp1890 ==( void*) Cyc_Parse_Struct_su){
goto _LL1893;} else{ goto _LL1894;} _LL1894: if( _temp1890 ==( void*) Cyc_Parse_Union_su){
goto _LL1895;} else{ goto _LL1891;} _LL1893: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1896=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1896[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1897; _temp1897.tag=
Cyc_TypeSpecifier_tok; _temp1897.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1898=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1898[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1899; _temp1899.tag= Cyc_Absyn_StructType;
_temp1899.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1899.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1899.f3= 0;
_temp1899;}); _temp1898;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1897;});
_temp1896;}); goto _LL1891; _LL1895: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1900=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1900[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1901; _temp1901.tag=
Cyc_TypeSpecifier_tok; _temp1901.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1902=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1902[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1903; _temp1903.tag= Cyc_Absyn_UnionType;
_temp1903.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1903.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1903.f3= 0;
_temp1903;}); _temp1902;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1901;});
_temp1900;}); goto _LL1891; _LL1891:;} break; case 86: _LL1889: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1905=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1905[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1906; _temp1906.tag= Cyc_TypeList_tok; _temp1906.f1= 0; _temp1906;});
_temp1905;}); break; case 87: _LL1904: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1908=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1908[ 0]=({ struct Cyc_TypeList_tok_struct _temp1909; _temp1909.tag= Cyc_TypeList_tok;
_temp1909.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1909;});
_temp1908;}); break; case 88: _LL1907:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1911=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1911[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1912; _temp1912.tag= Cyc_TypeList_tok; _temp1912.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1912;});
_temp1911;}); break; case 89: _LL1910: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1914=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1914[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1915; _temp1915.tag=
Cyc_StructOrUnion_tok; _temp1915.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp1915;}); _temp1914;}); break; case 90: _LL1913: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1917=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1917[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1918; _temp1918.tag=
Cyc_StructOrUnion_tok; _temp1918.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp1918;}); _temp1917;}); break; case 91: _LL1916: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1920=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1920[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1921; _temp1921.tag= Cyc_StructFieldDeclList_tok; _temp1921.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1921;});
_temp1920;}); break; case 92: _LL1919: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1923=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1923[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1924; _temp1924.tag= Cyc_StructFieldDeclListList_tok; _temp1924.f1=({
struct Cyc_List_List* _temp1925=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1925->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1925->tl= 0;
_temp1925;}); _temp1924;}); _temp1923;}); break; case 93: _LL1922: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1927=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1927[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1928; _temp1928.tag= Cyc_StructFieldDeclListList_tok;
_temp1928.f1=({ struct Cyc_List_List* _temp1929=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1929->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1929->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1929;}); _temp1928;}); _temp1927;}); break;
case 94: _LL1926: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1931=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1931[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1932; _temp1932.tag=
Cyc_InitDeclList_tok; _temp1932.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1932;});
_temp1931;}); break; case 95: _LL1930: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1934=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1934[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1935; _temp1935.tag=
Cyc_InitDeclList_tok; _temp1935.f1=({ struct Cyc_List_List* _temp1936=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1936->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1936->tl= 0; _temp1936;}); _temp1935;}); _temp1934;});
break; case 96: _LL1933: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1938=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1938[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1939; _temp1939.tag=
Cyc_InitDeclList_tok; _temp1939.f1=({ struct Cyc_List_List* _temp1940=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1940->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1940->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1940;});
_temp1939;}); _temp1938;}); break; case 97: _LL1937: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1942=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1942[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1943; _temp1943.tag= Cyc_InitDecl_tok;
_temp1943.f1=({ struct _tuple15* _temp1944=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1944->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1944->f2= 0;
_temp1944;}); _temp1943;}); _temp1942;}); break; case 98: _LL1941: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1946=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1946[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1947; _temp1947.tag= Cyc_InitDecl_tok; _temp1947.f1=({ struct _tuple15*
_temp1948=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1948->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1948->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1948;});
_temp1947;}); _temp1946;}); break; case 99: _LL1945: { struct Cyc_Absyn_Tqual tq=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])).f1; struct Cyc_List_List* _temp1950=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3; void*
_temp1951= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
struct Cyc_List_List* _temp1954; struct Cyc_List_List* _temp1956; struct _tuple0
_temp1952=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _LL1957:
_temp1956= _temp1952.f1; goto _LL1955; _LL1955: _temp1954= _temp1952.f2; goto
_LL1953; _LL1953: { struct Cyc_List_List* _temp1958=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp1951, _temp1956, _temp1950), _temp1954); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1959=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1959[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1960; _temp1960.tag= Cyc_StructFieldDeclList_tok; _temp1960.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),
_temp1958); _temp1960;}); _temp1959;}); break;}} case 100: _LL1949: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1962=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1962[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1963; _temp1963.tag= Cyc_QualSpecList_tok;
_temp1963.f1=({ struct _tuple18* _temp1964=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1964->f1= Cyc_Absyn_empty_tqual(); _temp1964->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1965=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1965->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1965->tl=
0; _temp1965;}); _temp1964->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1964;}); _temp1963;});
_temp1962;}); break; case 101: _LL1961: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1967=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1967[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1968; _temp1968.tag=
Cyc_QualSpecList_tok; _temp1968.f1=({ struct _tuple18* _temp1969=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1969->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1969->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1970=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1970->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1970->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1970;}); _temp1969->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1969;});
_temp1968;}); _temp1967;}); break; case 102: _LL1966: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1972=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1972[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1973; _temp1973.tag= Cyc_QualSpecList_tok;
_temp1973.f1=({ struct _tuple18* _temp1974=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1974->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1974->f2= 0;
_temp1974->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1974;}); _temp1973;}); _temp1972;}); break; case
103: _LL1971: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1976=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1976[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1977; _temp1977.tag=
Cyc_QualSpecList_tok; _temp1977.f1=({ struct _tuple18* _temp1978=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1978->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1978->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1978->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1978;});
_temp1977;}); _temp1976;}); break; case 104: _LL1975: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1980=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1980[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1981; _temp1981.tag= Cyc_DeclaratorExpoptList_tok;
_temp1981.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1981;}); _temp1980;}); break; case 105: _LL1979: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1983=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1983[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1984; _temp1984.tag= Cyc_DeclaratorExpoptList_tok;
_temp1984.f1=({ struct Cyc_List_List* _temp1985=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1985->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1985->tl= 0;
_temp1985;}); _temp1984;}); _temp1983;}); break; case 106: _LL1982: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp1987=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1987[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1988; _temp1988.tag= Cyc_DeclaratorExpoptList_tok;
_temp1988.f1=({ struct Cyc_List_List* _temp1989=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1989->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1989->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1989;}); _temp1988;}); _temp1987;}); break; case 107:
_LL1986: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp1991=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp1991[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp1992; _temp1992.tag=
Cyc_DeclaratorExpopt_tok; _temp1992.f1=({ struct _tuple13* _temp1993=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1993->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1993->f2= 0;
_temp1993;}); _temp1992;}); _temp1991;}); break; case 108: _LL1990: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1995=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1995[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1996; _temp1996.tag= Cyc_DeclaratorExpopt_tok;
_temp1996.f1=({ struct _tuple13* _temp1997=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1997->f1=({ struct Cyc_Parse_Declarator*
_temp1999=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp1999->id=({ struct _tuple1* _temp2000=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2000->f1= Cyc_Absyn_rel_ns_null; _temp2000->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp2000;}); _temp1999->tms= 0; _temp1999;});
_temp1997->f2=({ struct Cyc_Core_Opt* _temp1998=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1998->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1998;});
_temp1997;}); _temp1996;}); _temp1995;}); break; case 109: _LL1994: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2002=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2002[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2003; _temp2003.tag= Cyc_DeclaratorExpopt_tok;
_temp2003.f1=({ struct _tuple13* _temp2004=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2004->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2004->f2=({
struct Cyc_Core_Opt* _temp2005=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2005->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2005;}); _temp2004;});
_temp2003;}); _temp2002;}); break; case 110: _LL2001: { struct Cyc_List_List*
_temp2007=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2008=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2008[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2009; _temp2009.tag= Cyc_TypeSpecifier_tok;
_temp2009.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2010=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2010[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2011; _temp2011.tag= Cyc_Parse_Decl_spec;
_temp2011.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp2007,({
struct Cyc_Core_Opt* _temp2012=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2012->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2012;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2011;}); _temp2010;})); _temp2009;}); _temp2008;}); break;} case 111:
_LL2006: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2014=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2014[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2015; _temp2015.tag=
Cyc_TypeSpecifier_tok; _temp2015.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2016=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2016[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2017; _temp2017.tag= Cyc_Absyn_TunionType;
_temp2017.f1=({ struct Cyc_Absyn_TunionInfo _temp2018; _temp2018.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2019=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2019[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2020; _temp2020.tag= Cyc_Absyn_UnknownTunion;
_temp2020.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2021; _temp2021.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2021.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2021;});
_temp2020;}); _temp2019;})); _temp2018.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2018.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp2018;}); _temp2017;}); _temp2016;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2015;}); _temp2014;}); break; case 112: _LL2013: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2023=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2023[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2024; _temp2024.tag= Cyc_TypeSpecifier_tok;
_temp2024.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2025=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2025[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2026; _temp2026.tag=
Cyc_Absyn_TunionType; _temp2026.f1=({ struct Cyc_Absyn_TunionInfo _temp2027;
_temp2027.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp2028=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp2028[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp2029; _temp2029.tag= Cyc_Absyn_UnknownTunion; _temp2029.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp2030; _temp2030.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2030.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2030;}); _temp2029;}); _temp2028;})); _temp2027.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2027.rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2027;}); _temp2026;}); _temp2025;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2024;}); _temp2023;}); break; case 113: _LL2022: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2032=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2032[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2033; _temp2033.tag= Cyc_TypeSpecifier_tok;
_temp2033.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2034=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2034[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2035; _temp2035.tag= Cyc_Absyn_TunionFieldType; _temp2035.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2036; _temp2036.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2037=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2037[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2038; _temp2038.tag= Cyc_Absyn_UnknownTunionfield; _temp2038.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2039; _temp2039.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2039.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2039.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2039;});
_temp2038;}); _temp2037;})); _temp2036.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2036;}); _temp2035;});
_temp2034;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2033;}); _temp2032;}); break; case 114:
_LL2031: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2041=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2041[ 0]=({ struct
Cyc_Bool_tok_struct _temp2042; _temp2042.tag= Cyc_Bool_tok; _temp2042.f1= 0;
_temp2042;}); _temp2041;}); break; case 115: _LL2040: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2044=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2044[ 0]=({ struct Cyc_Bool_tok_struct
_temp2045; _temp2045.tag= Cyc_Bool_tok; _temp2045.f1= 1; _temp2045;}); _temp2044;});
break; case 116: _LL2043: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2047=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2047[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2048; _temp2048.tag= Cyc_TunionFieldList_tok; _temp2048.f1=({ struct Cyc_List_List*
_temp2049=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2049->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2049->tl= 0;
_temp2049;}); _temp2048;}); _temp2047;}); break; case 117: _LL2046: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2051=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2051[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2052; _temp2052.tag= Cyc_TunionFieldList_tok;
_temp2052.f1=({ struct Cyc_List_List* _temp2053=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2053->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2053->tl=
0; _temp2053;}); _temp2052;}); _temp2051;}); break; case 118: _LL2050: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2055=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2055[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2056; _temp2056.tag= Cyc_TunionFieldList_tok;
_temp2056.f1=({ struct Cyc_List_List* _temp2057=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2057->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2057->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2057;}); _temp2056;}); _temp2055;}); break; case 119:
_LL2054: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2059=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2059[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2060; _temp2060.tag=
Cyc_TunionFieldList_tok; _temp2060.f1=({ struct Cyc_List_List* _temp2061=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2061->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2061->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2061;}); _temp2060;});
_temp2059;}); break; case 120: _LL2058: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2063=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2063[ 0]=({ struct Cyc_Scope_tok_struct _temp2064; _temp2064.tag= Cyc_Scope_tok;
_temp2064.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2064;}); _temp2063;});
break; case 121: _LL2062: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2066=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2066[ 0]=({ struct Cyc_Scope_tok_struct _temp2067; _temp2067.tag= Cyc_Scope_tok;
_temp2067.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2067;}); _temp2066;});
break; case 122: _LL2065: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2069=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2069[ 0]=({ struct Cyc_Scope_tok_struct _temp2070; _temp2070.tag= Cyc_Scope_tok;
_temp2070.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2070;}); _temp2069;});
break; case 123: _LL2068: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2072=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2072[ 0]=({ struct Cyc_TunionField_tok_struct _temp2073; _temp2073.tag= Cyc_TunionField_tok;
_temp2073.f1=({ struct Cyc_Absyn_Tunionfield* _temp2074=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2074->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2074->tvs= 0;
_temp2074->typs= 0; _temp2074->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2074->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2074;}); _temp2073;}); _temp2072;}); break; case 124:
_LL2071: { struct Cyc_List_List* _temp2076=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2077=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2078=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2078[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2079; _temp2079.tag= Cyc_TunionField_tok;
_temp2079.f1=({ struct Cyc_Absyn_Tunionfield* _temp2080=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2080->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2080->tvs=
_temp2077; _temp2080->typs= _temp2076; _temp2080->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2080->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2080;}); _temp2079;}); _temp2078;}); break;}
case 125: _LL2075: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 126: _LL2081: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2083=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2083[ 0]=({ struct Cyc_Declarator_tok_struct _temp2084; _temp2084.tag= Cyc_Declarator_tok;
_temp2084.f1=({ struct Cyc_Parse_Declarator* _temp2085=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2085->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2085->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2085;});
_temp2084;}); _temp2083;}); break; case 127: _LL2082: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2087=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2087[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2088; _temp2088.tag= Cyc_Declarator_tok;
_temp2088.f1=({ struct Cyc_Parse_Declarator* _temp2089=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2089->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2089->tms= 0;
_temp2089;}); _temp2088;}); _temp2087;}); break; case 128: _LL2086: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 129:
_LL2090: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2092=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2092[ 0]=({ struct Cyc_Declarator_tok_struct _temp2093; _temp2093.tag= Cyc_Declarator_tok;
_temp2093.f1=({ struct Cyc_Parse_Declarator* _temp2094=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2094->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2094->tms=({ struct Cyc_List_List* _temp2095=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2095->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2095->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2095;}); _temp2094;}); _temp2093;});
_temp2092;}); break; case 130: _LL2091: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2097=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2097[ 0]=({ struct Cyc_Declarator_tok_struct _temp2098; _temp2098.tag= Cyc_Declarator_tok;
_temp2098.f1=({ struct Cyc_Parse_Declarator* _temp2099=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2099->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2099->tms=({ struct Cyc_List_List* _temp2100=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2100->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2101=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2101[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2102; _temp2102.tag= Cyc_Absyn_ConstArray_mod;
_temp2102.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2102;}); _temp2101;})); _temp2100->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2100;}); _temp2099;}); _temp2098;}); _temp2097;}); break; case 131: _LL2096: {
struct _tuple17 _temp2106; struct Cyc_List_List* _temp2107; struct Cyc_Core_Opt*
_temp2109; struct Cyc_Absyn_VarargInfo* _temp2111; int _temp2113; struct Cyc_List_List*
_temp2115; struct _tuple17* _temp2104= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2106=*
_temp2104; _LL2116: _temp2115= _temp2106.f1; goto _LL2114; _LL2114: _temp2113=
_temp2106.f2; goto _LL2112; _LL2112: _temp2111= _temp2106.f3; goto _LL2110;
_LL2110: _temp2109= _temp2106.f4; goto _LL2108; _LL2108: _temp2107= _temp2106.f5;
goto _LL2105; _LL2105: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2117=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2117[ 0]=({ struct Cyc_Declarator_tok_struct _temp2118; _temp2118.tag= Cyc_Declarator_tok;
_temp2118.f1=({ struct Cyc_Parse_Declarator* _temp2119=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2119->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2119->tms=({ struct Cyc_List_List* _temp2120=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2120->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2121=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2121[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2122; _temp2122.tag= Cyc_Absyn_Function_mod;
_temp2122.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2123=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2123[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2124; _temp2124.tag= Cyc_Absyn_WithTypes;
_temp2124.f1= _temp2115; _temp2124.f2= _temp2113; _temp2124.f3= _temp2111;
_temp2124.f4= _temp2109; _temp2124.f5= _temp2107; _temp2124;}); _temp2123;}));
_temp2122;}); _temp2121;})); _temp2120->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2120;});
_temp2119;}); _temp2118;}); _temp2117;}); break;} case 132: _LL2103: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2126=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2126[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2127; _temp2127.tag= Cyc_Declarator_tok;
_temp2127.f1=({ struct Cyc_Parse_Declarator* _temp2128=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2128->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2128->tms=({ struct Cyc_List_List* _temp2129=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2129->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2130=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2130[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2131; _temp2131.tag= Cyc_Absyn_Function_mod;
_temp2131.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2132=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2132[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2133; _temp2133.tag= Cyc_Absyn_WithTypes;
_temp2133.f1= 0; _temp2133.f2= 0; _temp2133.f3= 0; _temp2133.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2133.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2133;}); _temp2132;})); _temp2131;}); _temp2130;}));
_temp2129->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2129;}); _temp2128;}); _temp2127;});
_temp2126;}); break; case 133: _LL2125: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2135=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2135[ 0]=({ struct Cyc_Declarator_tok_struct _temp2136; _temp2136.tag= Cyc_Declarator_tok;
_temp2136.f1=({ struct Cyc_Parse_Declarator* _temp2137=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2137->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2137->tms=({ struct Cyc_List_List* _temp2138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2138->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2139=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2139[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2140; _temp2140.tag= Cyc_Absyn_Function_mod;
_temp2140.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2141=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2141[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2142; _temp2142.tag= Cyc_Absyn_NoTypes;
_temp2142.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2142.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2142;});
_temp2141;})); _temp2140;}); _temp2139;})); _temp2138->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2138;}); _temp2137;}); _temp2136;}); _temp2135;}); break; case 134: _LL2134: {
struct Cyc_List_List* _temp2144=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2145=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2145[ 0]=({ struct Cyc_Declarator_tok_struct _temp2146; _temp2146.tag= Cyc_Declarator_tok;
_temp2146.f1=({ struct Cyc_Parse_Declarator* _temp2147=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2147->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2147->tms=({ struct Cyc_List_List* _temp2148=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2148->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2149=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2149[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2150; _temp2150.tag= Cyc_Absyn_TypeParams_mod;
_temp2150.f1= _temp2144; _temp2150.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2150.f3=
0; _temp2150;}); _temp2149;})); _temp2148->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2148;});
_temp2147;}); _temp2146;}); _temp2145;}); break;} case 135: _LL2143:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2152=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2153=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2153[ 0]=({ struct Cyc_Declarator_tok_struct _temp2154; _temp2154.tag= Cyc_Declarator_tok;
_temp2154.f1=({ struct Cyc_Parse_Declarator* _temp2155=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2155->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2155->tms=({ struct Cyc_List_List* _temp2156=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2156->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2157=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2157[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2158; _temp2158.tag= Cyc_Absyn_TypeParams_mod;
_temp2158.f1= _temp2152; _temp2158.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2158.f3=
0; _temp2158;}); _temp2157;})); _temp2156->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2156;});
_temp2155;}); _temp2154;}); _temp2153;}); break;} case 136: _LL2151: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2160=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2160[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2161; _temp2161.tag= Cyc_Declarator_tok;
_temp2161.f1=({ struct Cyc_Parse_Declarator* _temp2162=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2162->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2162->tms=({ struct Cyc_List_List* _temp2163=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2163->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2164=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2164[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2165; _temp2165.tag= Cyc_Absyn_Attributes_mod;
_temp2165.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2165.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2165;}); _temp2164;}));
_temp2163->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2163;}); _temp2162;}); _temp2161;});
_temp2160;}); break; case 137: _LL2159: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2167=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2167[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2168; _temp2168.tag= Cyc_Declarator_tok;
_temp2168.f1=({ struct Cyc_Parse_Declarator* _temp2169=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2169->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2169->tms= 0;
_temp2169;}); _temp2168;}); _temp2167;}); break; case 138: _LL2166: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2171=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2171[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2172; _temp2172.tag= Cyc_Declarator_tok;
_temp2172.f1=({ struct Cyc_Parse_Declarator* _temp2173=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2173->id= Cyc_Absyn_exn_name;
_temp2173->tms= 0; _temp2173;}); _temp2172;}); _temp2171;}); break; case 139:
_LL2170: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2175=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2175[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2176; _temp2176.tag=
Cyc_TypeModifierList_tok; _temp2176.f1=({ struct Cyc_List_List* _temp2177=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2177->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2178=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2178[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2179; _temp2179.tag= Cyc_Absyn_Pointer_mod;
_temp2179.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2179.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2179.f3= Cyc_Absyn_empty_tqual(); _temp2179;}); _temp2178;}));
_temp2177->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2177;});
_temp2176;}); _temp2175;}); break; case 140: _LL2174: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2181=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2181[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2182; _temp2182.tag= Cyc_TypeModifierList_tok;
_temp2182.f1=({ struct Cyc_List_List* _temp2183=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2183->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2184=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2184[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2185; _temp2185.tag= Cyc_Absyn_Pointer_mod;
_temp2185.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2185.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2185.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2185;}); _temp2184;})); _temp2183->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2183;});
_temp2182;}); _temp2181;}); break; case 141: _LL2180: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2187=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2187[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2188; _temp2188.tag= Cyc_TypeModifierList_tok;
_temp2188.f1=({ struct Cyc_List_List* _temp2189=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2189->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2190=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2190[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2191; _temp2191.tag= Cyc_Absyn_Pointer_mod;
_temp2191.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2191.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2191.f3= Cyc_Absyn_empty_tqual(); _temp2191;}); _temp2190;}));
_temp2189->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2189;});
_temp2188;}); _temp2187;}); break; case 142: _LL2186: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2193=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2193[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2194; _temp2194.tag= Cyc_TypeModifierList_tok;
_temp2194.f1=({ struct Cyc_List_List* _temp2195=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2195->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2196=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2196[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2197; _temp2197.tag= Cyc_Absyn_Pointer_mod;
_temp2197.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2197.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2197.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2197;}); _temp2196;})); _temp2195->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2195;});
_temp2194;}); _temp2193;}); break; case 143: _LL2192: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2199=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2199[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2200; _temp2200.tag= Cyc_Pointer_Sort_tok;
_temp2200.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2201=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2201[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2202; _temp2202.tag=
Cyc_Absyn_Nullable_ps; _temp2202.f1= Cyc_Absyn_exp_unsigned_one; _temp2202;});
_temp2201;})); _temp2200;}); _temp2199;}); break; case 144: _LL2198: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2204=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2204[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2205; _temp2205.tag= Cyc_Pointer_Sort_tok;
_temp2205.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2206=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2206[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2207; _temp2207.tag= Cyc_Absyn_NonNullable_ps; _temp2207.f1= Cyc_Absyn_exp_unsigned_one;
_temp2207;}); _temp2206;})); _temp2205;}); _temp2204;}); break; case 145:
_LL2203: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2209=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2209[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2210; _temp2210.tag=
Cyc_Pointer_Sort_tok; _temp2210.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2211=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2211[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2212; _temp2212.tag=
Cyc_Absyn_Nullable_ps; _temp2212.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2212;});
_temp2211;})); _temp2210;}); _temp2209;}); break; case 146: _LL2208: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2214=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2214[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2215; _temp2215.tag= Cyc_Pointer_Sort_tok;
_temp2215.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2216=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2216[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2217; _temp2217.tag= Cyc_Absyn_NonNullable_ps; _temp2217.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2217;});
_temp2216;})); _temp2215;}); _temp2214;}); break; case 147: _LL2213: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2219=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2219[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2220; _temp2220.tag= Cyc_Pointer_Sort_tok;
_temp2220.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2220;});
_temp2219;}); break; case 148: _LL2218: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2222=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2222[ 0]=({ struct Cyc_Rgn_tok_struct _temp2223; _temp2223.tag= Cyc_Rgn_tok;
_temp2223.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2223;}); _temp2222;});
break; case 149: _LL2221: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2224: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2226=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2226[ 0]=({ struct Cyc_Rgn_tok_struct _temp2227; _temp2227.tag= Cyc_Rgn_tok;
_temp2227.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2227;}); _temp2226;}); break; case 151: _LL2225:
if( Cyc_yyget_Kind_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind\n",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Rgn_tok_struct* _temp2229=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp2229[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2230; _temp2230.tag= Cyc_Rgn_tok; _temp2230.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind)); _temp2230;});
_temp2229;}); break; case 152: _LL2228: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2232=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2232[ 0]=({ struct Cyc_Rgn_tok_struct _temp2233; _temp2233.tag= Cyc_Rgn_tok;
_temp2233.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2234=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2234->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2234;}), 0); _temp2233;}); _temp2232;});
break; case 153: _LL2231: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 154: _LL2235: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2237=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2237[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2238; _temp2238.tag= Cyc_TypeQual_tok; _temp2238.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2238;});
_temp2237;}); break; case 155: _LL2236: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2240=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2240[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2241; _temp2241.tag= Cyc_ParamDeclListBool_tok; _temp2241.f1=({ struct
_tuple17* _temp2242=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2242->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2242->f2= 0; _temp2242->f3= 0; _temp2242->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2242->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2242;}); _temp2241;}); _temp2240;}); break; case 156:
_LL2239: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2244=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2244[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2245; _temp2245.tag=
Cyc_ParamDeclListBool_tok; _temp2245.f1=({ struct _tuple17* _temp2246=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2246->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2246->f2= 1;
_temp2246->f3= 0; _temp2246->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2246->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2246;});
_temp2245;}); _temp2244;}); break; case 157: _LL2243: { void* _temp2250; struct
Cyc_Absyn_Tqual _temp2252; struct Cyc_Core_Opt* _temp2254; struct _tuple2
_temp2248=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL2255: _temp2254= _temp2248.f1; goto _LL2253; _LL2253:
_temp2252= _temp2248.f2; goto _LL2251; _LL2251: _temp2250= _temp2248.f3; goto
_LL2249; _LL2249: { struct Cyc_Absyn_VarargInfo* _temp2256=({ struct Cyc_Absyn_VarargInfo*
_temp2260=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2260->name= _temp2254; _temp2260->tq= _temp2252; _temp2260->type=( void*)
_temp2250; _temp2260->rgn=( void*) Cyc_yyget_Rgn_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2260->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2260;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2257=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2257[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2258; _temp2258.tag= Cyc_ParamDeclListBool_tok; _temp2258.f1=({ struct
_tuple17* _temp2259=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2259->f1= 0; _temp2259->f2= 0; _temp2259->f3= _temp2256; _temp2259->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2259->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2259;}); _temp2258;}); _temp2257;}); break;}} case 158:
_LL2247: { void* _temp2264; struct Cyc_Absyn_Tqual _temp2266; struct Cyc_Core_Opt*
_temp2268; struct _tuple2 _temp2262=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL2269: _temp2268=
_temp2262.f1; goto _LL2267; _LL2267: _temp2266= _temp2262.f2; goto _LL2265;
_LL2265: _temp2264= _temp2262.f3; goto _LL2263; _LL2263: { struct Cyc_Absyn_VarargInfo*
_temp2270=({ struct Cyc_Absyn_VarargInfo* _temp2274=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2274->name= _temp2268;
_temp2274->tq= _temp2266; _temp2274->type=( void*) _temp2264; _temp2274->rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2274->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2274;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2271=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2271[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2272; _temp2272.tag= Cyc_ParamDeclListBool_tok;
_temp2272.f1=({ struct _tuple17* _temp2273=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2273->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)])); _temp2273->f2= 0;
_temp2273->f3= _temp2270; _temp2273->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2273->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2273;});
_temp2272;}); _temp2271;}); break;}} case 159: _LL2261: yyval=( void*)({ struct
Cyc_TypeOpt_tok_struct* _temp2276=( struct Cyc_TypeOpt_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2276[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2277; _temp2277.tag= Cyc_TypeOpt_tok; _temp2277.f1= 0; _temp2277;});
_temp2276;}); break; case 160: _LL2275: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2279=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2279[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2280; _temp2280.tag= Cyc_TypeOpt_tok;
_temp2280.f1=({ struct Cyc_Core_Opt* _temp2281=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2281->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2282=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2282[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2283; _temp2283.tag= Cyc_Absyn_JoinEff;
_temp2283.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2283;}); _temp2282;})); _temp2281;}); _temp2280;});
_temp2279;}); break; case 161: _LL2278: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2285=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2285[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2286; _temp2286.tag= Cyc_Rgnorder_tok;
_temp2286.f1= 0; _temp2286;}); _temp2285;}); break; case 162: _LL2284: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2287: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2289=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2289[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2290; _temp2290.tag= Cyc_Rgnorder_tok; _temp2290.f1=({ struct Cyc_List_List*
_temp2291=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2291->hd=( void*)({ struct _tuple19* _temp2292=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2292->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2292->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2292;}); _temp2291->tl= 0; _temp2291;});
_temp2290;}); _temp2289;}); break; case 164: _LL2288: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2294=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2294[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2295; _temp2295.tag= Cyc_Rgnorder_tok; _temp2295.f1=({ struct Cyc_List_List*
_temp2296=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2296->hd=( void*)({ struct _tuple19* _temp2297=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2297->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2297->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2297;}); _temp2296->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2296;});
_temp2295;}); _temp2294;}); break; case 165: _LL2293: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2299=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2299[ 0]=({ struct Cyc_Bool_tok_struct
_temp2300; _temp2300.tag= Cyc_Bool_tok; _temp2300.f1= 0; _temp2300;}); _temp2299;});
break; case 166: _LL2298: { struct _tagged_arr _temp2302= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2302, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2303=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2303[ 0]=({ struct Cyc_Bool_tok_struct _temp2304; _temp2304.tag= Cyc_Bool_tok;
_temp2304.f1= 1; _temp2304;}); _temp2303;}); break;} case 167: _LL2301: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2305: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2307=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2307[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2308; _temp2308.tag= Cyc_TypeList_tok; _temp2308.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2308;});
_temp2307;}); break; case 169: _LL2306: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2310=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2310[ 0]=({ struct Cyc_TypeList_tok_struct _temp2311; _temp2311.tag= Cyc_TypeList_tok;
_temp2311.f1= 0; _temp2311;}); _temp2310;}); break; case 170: _LL2309: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
171: _LL2312: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2314=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2314[ 0]=({ struct Cyc_TypeList_tok_struct _temp2315; _temp2315.tag= Cyc_TypeList_tok;
_temp2315.f1=({ struct Cyc_List_List* _temp2316=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2316->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2316->tl= 0; _temp2316;});
_temp2315;}); _temp2314;}); break; case 172: _LL2313: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_EffKind){
Cyc_Parse_err( _tag_arr("expecing effect kind (E)", sizeof( unsigned char), 25u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2318=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2318[ 0]=({ struct Cyc_TypeList_tok_struct _temp2319; _temp2319.tag= Cyc_TypeList_tok;
_temp2319.f1=({ struct Cyc_List_List* _temp2320=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2320->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2320->tl= 0;
_temp2320;}); _temp2319;}); _temp2318;}); break; case 173: _LL2317: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2322=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2322[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2323; _temp2323.tag= Cyc_TypeList_tok; _temp2323.f1=({ struct Cyc_List_List*
_temp2324=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2324->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2325=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2325[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2326; _temp2326.tag= Cyc_Absyn_AccessEff;
_temp2326.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2326;}); _temp2325;})); _temp2324->tl= 0;
_temp2324;}); _temp2323;}); _temp2322;}); break; case 174: _LL2321: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2328=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2328[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2329; _temp2329.tag= Cyc_TypeList_tok; _temp2329.f1=({ struct Cyc_List_List*
_temp2330=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2330->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2331=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2331[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2332; _temp2332.tag= Cyc_Absyn_AccessEff;
_temp2332.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2332;}); _temp2331;})); _temp2330->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2330;});
_temp2329;}); _temp2328;}); break; case 175: _LL2327: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2334=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2334[ 0]=({ struct Cyc_TypeList_tok_struct _temp2335; _temp2335.tag= Cyc_TypeList_tok;
_temp2335.f1=({ struct Cyc_List_List* _temp2336=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2336->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2337=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2337[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2338; _temp2338.tag= Cyc_Absyn_AccessEff;
_temp2338.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2338;}); _temp2337;})); _temp2336->tl= 0;
_temp2336;}); _temp2335;}); _temp2334;}); break; case 176: _LL2333: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2340=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2340[ 0]=({ struct Cyc_TypeList_tok_struct _temp2341; _temp2341.tag= Cyc_TypeList_tok;
_temp2341.f1=({ struct Cyc_List_List* _temp2342=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2342->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2343=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2343[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2344; _temp2344.tag= Cyc_Absyn_AccessEff;
_temp2344.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2344;}); _temp2343;})); _temp2342->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2342;});
_temp2341;}); _temp2340;}); break; case 177: _LL2339: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2346=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2346[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2347; _temp2347.tag= Cyc_ParamDeclList_tok;
_temp2347.f1=({ struct Cyc_List_List* _temp2348=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2348->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2348->tl= 0;
_temp2348;}); _temp2347;}); _temp2346;}); break; case 178: _LL2345: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2350=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2350[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2351; _temp2351.tag= Cyc_ParamDeclList_tok;
_temp2351.f1=({ struct Cyc_List_List* _temp2352=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2352->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2352->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2352;}); _temp2351;}); _temp2350;}); break; case 179:
_LL2349: { void* _temp2354= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2355=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2356=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2357=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2358= Cyc_Parse_apply_tms( _temp2356, _temp2354, _temp2355,
_temp2357); if( _temp2358.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
_tuple1* _temp2359=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id;{ void* _temp2360=(*
_temp2359).f1; struct Cyc_List_List* _temp2370; struct Cyc_List_List* _temp2372;
_LL2362: if( _temp2360 ==( void*) Cyc_Absyn_Loc_n){ goto _LL2363;} else{ goto
_LL2364;} _LL2364: if(( unsigned int) _temp2360 > 1u?*(( int*) _temp2360) == Cyc_Absyn_Rel_n:
0){ _LL2371: _temp2370=(( struct Cyc_Absyn_Rel_n_struct*) _temp2360)->f1; if(
_temp2370 == 0){ goto _LL2365;} else{ goto _LL2366;}} else{ goto _LL2366;}
_LL2366: if(( unsigned int) _temp2360 > 1u?*(( int*) _temp2360) == Cyc_Absyn_Abs_n:
0){ _LL2373: _temp2372=(( struct Cyc_Absyn_Abs_n_struct*) _temp2360)->f1; if(
_temp2372 == 0){ goto _LL2367;} else{ goto _LL2368;}} else{ goto _LL2368;}
_LL2368: goto _LL2369; _LL2363: goto _LL2361; _LL2365: goto _LL2361; _LL2367:
goto _LL2361; _LL2369: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); goto
_LL2361; _LL2361:;}{ struct Cyc_Core_Opt* _temp2374=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2378=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2378->v=( void*)(* _temp2359).f2; _temp2378;}); if(
_temp2358.f4 != 0){ Cyc_Parse_warn( _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2358.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2375=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2375[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2376; _temp2376.tag= Cyc_ParamDecl_tok; _temp2376.f1=({
struct _tuple2* _temp2377=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2377->f1= _temp2374; _temp2377->f2= _temp2358.f1; _temp2377->f3= _temp2358.f2;
_temp2377;}); _temp2376;}); _temp2375;}); break;}}} case 180: _LL2353: { void*
_temp2380= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2381=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; struct Cyc_Absyn_Tqual _temp2382=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; if( _temp2381
!= 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring", sizeof(
unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2383=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2383[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2384; _temp2384.tag= Cyc_ParamDecl_tok; _temp2384.f1=({
struct _tuple2* _temp2385=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2385->f1= 0; _temp2385->f2= _temp2382; _temp2385->f3= _temp2380; _temp2385;});
_temp2384;}); _temp2383;}); break;} case 181: _LL2379: { void* _temp2387= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2388=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2389=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2390=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2391= Cyc_Parse_apply_tms( _temp2389, _temp2387, _temp2388,
_temp2390); if( _temp2391.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2391.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2392=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2392[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2393; _temp2393.tag= Cyc_ParamDecl_tok; _temp2393.f1=({
struct _tuple2* _temp2394=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2394->f1= 0; _temp2394->f2= _temp2391.f1; _temp2394->f3= _temp2391.f2;
_temp2394;}); _temp2393;}); _temp2392;}); break;} case 182: _LL2386: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2396=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2396[ 0]=({ struct Cyc_IdList_tok_struct
_temp2397; _temp2397.tag= Cyc_IdList_tok; _temp2397.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2397;});
_temp2396;}); break; case 183: _LL2395: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2399=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2399[ 0]=({ struct Cyc_IdList_tok_struct _temp2400; _temp2400.tag= Cyc_IdList_tok;
_temp2400.f1=({ struct Cyc_List_List* _temp2401=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2401->hd=( void*)({ struct
_tagged_arr* _temp2402=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2402[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2402;}); _temp2401->tl=
0; _temp2401;}); _temp2400;}); _temp2399;}); break; case 184: _LL2398: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2404=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2404[ 0]=({ struct Cyc_IdList_tok_struct
_temp2405; _temp2405.tag= Cyc_IdList_tok; _temp2405.f1=({ struct Cyc_List_List*
_temp2406=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2406->hd=( void*)({ struct _tagged_arr* _temp2407=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2407[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2407;});
_temp2406->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2406;}); _temp2405;}); _temp2404;}); break; case 185:
_LL2403: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 186: _LL2408: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 187: _LL2409: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2411=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2411[ 0]=({ struct Cyc_Exp_tok_struct _temp2412; _temp2412.tag= Cyc_Exp_tok;
_temp2412.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2413=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2413[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2414; _temp2414.tag= Cyc_Absyn_UnresolvedMem_e; _temp2414.f1= 0; _temp2414.f2=
0; _temp2414;}); _temp2413;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2412;});
_temp2411;}); break; case 188: _LL2410: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2416=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2416[ 0]=({ struct Cyc_Exp_tok_struct _temp2417; _temp2417.tag= Cyc_Exp_tok;
_temp2417.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2418=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2418[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2419; _temp2419.tag= Cyc_Absyn_UnresolvedMem_e; _temp2419.f1= 0; _temp2419.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2419;});
_temp2418;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2417;}); _temp2416;}); break; case 189:
_LL2415: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2421=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2421[ 0]=({ struct Cyc_Exp_tok_struct
_temp2422; _temp2422.tag= Cyc_Exp_tok; _temp2422.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2423=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2423[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2424; _temp2424.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2424.f1= 0; _temp2424.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2424;});
_temp2423;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2422;}); _temp2421;}); break; case 190:
_LL2420: { struct Cyc_Absyn_Vardecl* _temp2426= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2432=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2432->f1=( void*) Cyc_Absyn_Loc_n; _temp2432->f2=({ struct _tagged_arr*
_temp2433=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp2433[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2433;}); _temp2432;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2426->tq=({ struct Cyc_Absyn_Tqual _temp2427; _temp2427.q_const= 1;
_temp2427.q_volatile= 0; _temp2427.q_restrict= 1; _temp2427;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2428=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2428[ 0]=({ struct Cyc_Exp_tok_struct
_temp2429; _temp2429.tag= Cyc_Exp_tok; _temp2429.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2430=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2430[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2431; _temp2431.tag= Cyc_Absyn_Comprehension_e;
_temp2431.f1= _temp2426; _temp2431.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2431.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2431;});
_temp2430;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2429;}); _temp2428;}); break;} case
191: _LL2425: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2435=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2435[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2436; _temp2436.tag=
Cyc_InitializerList_tok; _temp2436.f1=({ struct Cyc_List_List* _temp2437=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2437->hd=(
void*)({ struct _tuple20* _temp2438=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2438->f1= 0; _temp2438->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2438;}); _temp2437->tl=
0; _temp2437;}); _temp2436;}); _temp2435;}); break; case 192: _LL2434: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2440=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2440[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2441; _temp2441.tag= Cyc_InitializerList_tok;
_temp2441.f1=({ struct Cyc_List_List* _temp2442=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2442->hd=( void*)({ struct
_tuple20* _temp2443=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2443->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2443->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2443;});
_temp2442->tl= 0; _temp2442;}); _temp2441;}); _temp2440;}); break; case 193:
_LL2439: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2445=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2445[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2446; _temp2446.tag=
Cyc_InitializerList_tok; _temp2446.f1=({ struct Cyc_List_List* _temp2447=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2447->hd=(
void*)({ struct _tuple20* _temp2448=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2448->f1= 0; _temp2448->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2448;}); _temp2447->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2447;}); _temp2446;}); _temp2445;}); break; case 194:
_LL2444: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2450=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2450[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2451; _temp2451.tag=
Cyc_InitializerList_tok; _temp2451.f1=({ struct Cyc_List_List* _temp2452=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2452->hd=(
void*)({ struct _tuple20* _temp2453=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2453->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2453->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2453;});
_temp2452->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2452;});
_temp2451;}); _temp2450;}); break; case 195: _LL2449: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2455=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2455[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2456; _temp2456.tag= Cyc_DesignatorList_tok;
_temp2456.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2456;});
_temp2455;}); break; case 196: _LL2454: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2458=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2458[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2459; _temp2459.tag=
Cyc_DesignatorList_tok; _temp2459.f1=({ struct Cyc_List_List* _temp2460=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2460->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2460->tl= 0; _temp2460;}); _temp2459;}); _temp2458;});
break; case 197: _LL2457: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2462=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2462[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2463; _temp2463.tag=
Cyc_DesignatorList_tok; _temp2463.f1=({ struct Cyc_List_List* _temp2464=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2464->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2464->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2464;});
_temp2463;}); _temp2462;}); break; case 198: _LL2461: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2466=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2466[ 0]=({ struct
Cyc_Designator_tok_struct _temp2467; _temp2467.tag= Cyc_Designator_tok;
_temp2467.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2468=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2468[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2469; _temp2469.tag=
Cyc_Absyn_ArrayElement; _temp2469.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2469;});
_temp2468;})); _temp2467;}); _temp2466;}); break; case 199: _LL2465: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2471=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2471[ 0]=({ struct
Cyc_Designator_tok_struct _temp2472; _temp2472.tag= Cyc_Designator_tok;
_temp2472.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2473=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2473[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2474; _temp2474.tag= Cyc_Absyn_FieldName;
_temp2474.f1=({ struct _tagged_arr* _temp2475=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2475[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2475;}); _temp2474;});
_temp2473;})); _temp2472;}); _temp2471;}); break; case 200: _LL2470: { void*
_temp2477= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2478=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2478 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2479=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2480=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2480[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2481; _temp2481.tag= Cyc_ParamDecl_tok; _temp2481.f1=({
struct _tuple2* _temp2482=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp2482[ 0]=({ struct _tuple2 _temp2483; _temp2483.f1= 0; _temp2483.f2=
_temp2479; _temp2483.f3= _temp2477; _temp2483;}); _temp2482;}); _temp2481;});
_temp2480;}); break;}} case 201: _LL2476: { void* _temp2485= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2486=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2487=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2488=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2489= Cyc_Parse_apply_tms( _temp2487, _temp2485, _temp2486,
_temp2488); if( _temp2489.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2489.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2490=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2490[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2491; _temp2491.tag= Cyc_ParamDecl_tok; _temp2491.f1=({
struct _tuple2* _temp2492=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2492->f1= 0; _temp2492->f2= _temp2489.f1; _temp2492->f3= _temp2489.f2;
_temp2492;}); _temp2491;}); _temp2490;}); break;} case 202: _LL2484: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2494=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2494[ 0]=({ struct Cyc_Type_tok_struct
_temp2495; _temp2495.tag= Cyc_Type_tok; _temp2495.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2495;});
_temp2494;}); break; case 203: _LL2493: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2497=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2497[ 0]=({ struct Cyc_Type_tok_struct _temp2498; _temp2498.tag= Cyc_Type_tok;
_temp2498.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2499=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2499[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2500; _temp2500.tag= Cyc_Absyn_JoinEff;
_temp2500.f1= 0; _temp2500;}); _temp2499;})); _temp2498;}); _temp2497;}); break;
case 204: _LL2496: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2502=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2502[ 0]=({ struct Cyc_Type_tok_struct _temp2503; _temp2503.tag= Cyc_Type_tok;
_temp2503.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2504=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2504[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2505; _temp2505.tag= Cyc_Absyn_JoinEff;
_temp2505.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2505;}); _temp2504;})); _temp2503;});
_temp2502;}); break; case 205: _LL2501: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2507=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2507[ 0]=({ struct Cyc_Type_tok_struct _temp2508; _temp2508.tag= Cyc_Type_tok;
_temp2508.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2509=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2509[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2510; _temp2510.tag= Cyc_Absyn_JoinEff;
_temp2510.f1=({ struct Cyc_List_List* _temp2511=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2511->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2511->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2511;}); _temp2510;}); _temp2509;})); _temp2508;});
_temp2507;}); break; case 206: _LL2506: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2513=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2513[ 0]=({ struct Cyc_TypeList_tok_struct _temp2514; _temp2514.tag= Cyc_TypeList_tok;
_temp2514.f1=({ struct Cyc_List_List* _temp2515=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2515->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2515->tl= 0;
_temp2515;}); _temp2514;}); _temp2513;}); break; case 207: _LL2512: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2517=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2517[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2518; _temp2518.tag= Cyc_TypeList_tok; _temp2518.f1=({ struct Cyc_List_List*
_temp2519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2519->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2519->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2519;});
_temp2518;}); _temp2517;}); break; case 208: _LL2516: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2521=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2521[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2522; _temp2522.tag= Cyc_AbstractDeclarator_tok;
_temp2522.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2523=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2523->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2523;});
_temp2522;}); _temp2521;}); break; case 209: _LL2520: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 210: _LL2524:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2526=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2526[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2527; _temp2527.tag=
Cyc_AbstractDeclarator_tok; _temp2527.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2528=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2528->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2528;});
_temp2527;}); _temp2526;}); break; case 211: _LL2525: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 212:
_LL2529: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2531=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2531[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2532; _temp2532.tag=
Cyc_AbstractDeclarator_tok; _temp2532.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2533=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2533->tms=({ struct Cyc_List_List* _temp2534=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2534->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2534->tl= 0; _temp2534;}); _temp2533;}); _temp2532;}); _temp2531;}); break;
case 213: _LL2530: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2536=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2536[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2537; _temp2537.tag= Cyc_AbstractDeclarator_tok; _temp2537.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2538=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2538->tms=({ struct Cyc_List_List* _temp2539=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2539->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2539->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2539;});
_temp2538;}); _temp2537;}); _temp2536;}); break; case 214: _LL2535: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2541=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2541[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2542; _temp2542.tag= Cyc_AbstractDeclarator_tok;
_temp2542.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2543=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2543->tms=({
struct Cyc_List_List* _temp2544=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2544->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2545=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2545[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2546; _temp2546.tag= Cyc_Absyn_ConstArray_mod; _temp2546.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2546;});
_temp2545;})); _temp2544->tl= 0; _temp2544;}); _temp2543;}); _temp2542;});
_temp2541;}); break; case 215: _LL2540: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2548=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2548[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2549; _temp2549.tag= Cyc_AbstractDeclarator_tok; _temp2549.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2550=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2550->tms=({ struct Cyc_List_List* _temp2551=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2551->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2552=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2552[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2553; _temp2553.tag= Cyc_Absyn_ConstArray_mod;
_temp2553.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2553;}); _temp2552;})); _temp2551->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2551;}); _temp2550;}); _temp2549;}); _temp2548;}); break; case 216: _LL2547:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2555=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2555[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2556; _temp2556.tag=
Cyc_AbstractDeclarator_tok; _temp2556.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2557=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2557->tms=({ struct Cyc_List_List* _temp2558=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2558->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2559=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2559[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2560; _temp2560.tag= Cyc_Absyn_Function_mod;
_temp2560.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2561=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2561[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2562; _temp2562.tag= Cyc_Absyn_WithTypes;
_temp2562.f1= 0; _temp2562.f2= 0; _temp2562.f3= 0; _temp2562.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2562.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2562;}); _temp2561;})); _temp2560;}); _temp2559;}));
_temp2558->tl= 0; _temp2558;}); _temp2557;}); _temp2556;}); _temp2555;}); break;
case 217: _LL2554: { struct _tuple17 _temp2566; struct Cyc_List_List* _temp2567;
struct Cyc_Core_Opt* _temp2569; struct Cyc_Absyn_VarargInfo* _temp2571; int
_temp2573; struct Cyc_List_List* _temp2575; struct _tuple17* _temp2564= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2566=*
_temp2564; _LL2576: _temp2575= _temp2566.f1; goto _LL2574; _LL2574: _temp2573=
_temp2566.f2; goto _LL2572; _LL2572: _temp2571= _temp2566.f3; goto _LL2570;
_LL2570: _temp2569= _temp2566.f4; goto _LL2568; _LL2568: _temp2567= _temp2566.f5;
goto _LL2565; _LL2565: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2577=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2577[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2578; _temp2578.tag= Cyc_AbstractDeclarator_tok; _temp2578.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2579=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2579->tms=({ struct Cyc_List_List* _temp2580=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2580->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2581=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2581[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2582; _temp2582.tag= Cyc_Absyn_Function_mod;
_temp2582.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2583=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2583[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2584; _temp2584.tag= Cyc_Absyn_WithTypes;
_temp2584.f1= _temp2575; _temp2584.f2= _temp2573; _temp2584.f3= _temp2571;
_temp2584.f4= _temp2569; _temp2584.f5= _temp2567; _temp2584;}); _temp2583;}));
_temp2582;}); _temp2581;})); _temp2580->tl= 0; _temp2580;}); _temp2579;});
_temp2578;}); _temp2577;}); break;} case 218: _LL2563: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2586=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2586[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2587; _temp2587.tag= Cyc_AbstractDeclarator_tok;
_temp2587.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2588=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2588->tms=({
struct Cyc_List_List* _temp2589=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2589->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2590=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2590[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2591; _temp2591.tag=
Cyc_Absyn_Function_mod; _temp2591.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2592=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2592[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2593; _temp2593.tag= Cyc_Absyn_WithTypes;
_temp2593.f1= 0; _temp2593.f2= 0; _temp2593.f3= 0; _temp2593.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2593.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2593;}); _temp2592;})); _temp2591;}); _temp2590;}));
_temp2589->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2589;});
_temp2588;}); _temp2587;}); _temp2586;}); break; case 219: _LL2585: { struct
_tuple17 _temp2597; struct Cyc_List_List* _temp2598; struct Cyc_Core_Opt*
_temp2600; struct Cyc_Absyn_VarargInfo* _temp2602; int _temp2604; struct Cyc_List_List*
_temp2606; struct _tuple17* _temp2595= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2597=*
_temp2595; _LL2607: _temp2606= _temp2597.f1; goto _LL2605; _LL2605: _temp2604=
_temp2597.f2; goto _LL2603; _LL2603: _temp2602= _temp2597.f3; goto _LL2601;
_LL2601: _temp2600= _temp2597.f4; goto _LL2599; _LL2599: _temp2598= _temp2597.f5;
goto _LL2596; _LL2596: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2608=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2608[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2609; _temp2609.tag= Cyc_AbstractDeclarator_tok; _temp2609.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2610=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2610->tms=({ struct Cyc_List_List* _temp2611=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2611->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2612=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2612[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2613; _temp2613.tag= Cyc_Absyn_Function_mod;
_temp2613.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2614=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2614[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2615; _temp2615.tag= Cyc_Absyn_WithTypes;
_temp2615.f1= _temp2606; _temp2615.f2= _temp2604; _temp2615.f3= _temp2602;
_temp2615.f4= _temp2600; _temp2615.f5= _temp2598; _temp2615;}); _temp2614;}));
_temp2613;}); _temp2612;})); _temp2611->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2611;}); _temp2610;}); _temp2609;}); _temp2608;}); break;} case 220:
_LL2594: { struct Cyc_List_List* _temp2617=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2618=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2618[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2619; _temp2619.tag= Cyc_AbstractDeclarator_tok; _temp2619.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2620=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2620->tms=({ struct Cyc_List_List* _temp2621=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2621->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2622=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2622[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2623; _temp2623.tag= Cyc_Absyn_TypeParams_mod;
_temp2623.f1= _temp2617; _temp2623.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2623.f3=
0; _temp2623;}); _temp2622;})); _temp2621->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2621;}); _temp2620;}); _temp2619;}); _temp2618;}); break;} case 221:
_LL2616:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2625=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2626=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2626[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2627; _temp2627.tag= Cyc_AbstractDeclarator_tok; _temp2627.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2628=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2628->tms=({ struct Cyc_List_List* _temp2629=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2629->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2630=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2630[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2631; _temp2631.tag= Cyc_Absyn_TypeParams_mod;
_temp2631.f1= _temp2625; _temp2631.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2631.f3=
0; _temp2631;}); _temp2630;})); _temp2629->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2629;}); _temp2628;}); _temp2627;}); _temp2626;}); break;} case 222:
_LL2624: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2633=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2633[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2634; _temp2634.tag=
Cyc_AbstractDeclarator_tok; _temp2634.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2635=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2635->tms=({ struct Cyc_List_List* _temp2636=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2636->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2637=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2637[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2638; _temp2638.tag= Cyc_Absyn_Attributes_mod;
_temp2638.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2638.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2638;}); _temp2637;}));
_temp2636->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms; _temp2636;});
_temp2635;}); _temp2634;}); _temp2633;}); break; case 223: _LL2632: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL2639:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL2640: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL2641: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL2642: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL2643:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
229: _LL2644: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2656=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2656->name=({ struct
_tagged_arr* _temp2657=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2657[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2657;});
_temp2656->identity= 0; _temp2656->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2656;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2654=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2654[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2655; _temp2655.tag= Cyc_Absyn_VarType;
_temp2655.f1= tv; _temp2655;}); _temp2654;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2646=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2646[ 0]=({ struct Cyc_Stmt_tok_struct _temp2647; _temp2647.tag= Cyc_Stmt_tok;
_temp2647.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2648=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2648[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2649; _temp2649.tag= Cyc_Absyn_Region_s;
_temp2649.f1= tv; _temp2649.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2650=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2650->f1=(
void*) Cyc_Absyn_Loc_n; _temp2650->f2=({ struct _tagged_arr* _temp2651=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2651[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2651;});
_temp2650;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2652=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2652[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2653; _temp2653.tag=
Cyc_Absyn_RgnHandleType; _temp2653.f1=( void*) t; _temp2653;}); _temp2652;}), 0);
_temp2649.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2649;}); _temp2648;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2647;});
_temp2646;}); break;} case 230: _LL2645: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2669=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2669->name=({ struct
_tagged_arr* _temp2670=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2670[ 0]=( struct _tagged_arr)({ struct _tagged_arr(*
_temp2671)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2675= _tag_arr("`%s", sizeof( unsigned char), 4u);
struct Cyc_Stdio_String_pa_struct _temp2676; _temp2676.tag= Cyc_Stdio_String_pa;
_temp2676.f1=( struct _tagged_arr) Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]);{ void* _temp2674=(
void*)& _temp2676; void* _temp2672[ 1u]={ _temp2674}; struct _tagged_arr
_temp2673={( void*) _temp2672,( void*) _temp2672,( void*)( _temp2672 + 1u)};
_temp2671( _temp2675, _temp2673);}}); _temp2670;}); _temp2669->identity= 0;
_temp2669->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind); _temp2669;});
void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2667=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2667[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2668; _temp2668.tag= Cyc_Absyn_VarType; _temp2668.f1=
tv; _temp2668;}); _temp2667;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2659=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2659[ 0]=({ struct Cyc_Stmt_tok_struct _temp2660; _temp2660.tag= Cyc_Stmt_tok;
_temp2660.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2661=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2661[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2662; _temp2662.tag= Cyc_Absyn_Region_s;
_temp2662.f1= tv; _temp2662.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2663=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2663->f1=(
void*) Cyc_Absyn_Loc_n; _temp2663->f2=({ struct _tagged_arr* _temp2664=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2664[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2664;});
_temp2663;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2665=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2665[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2666; _temp2666.tag=
Cyc_Absyn_RgnHandleType; _temp2666.f1=( void*) t; _temp2666;}); _temp2665;}), 0);
_temp2662.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2662;}); _temp2661;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2660;});
_temp2659;}); break;} case 231: _LL2658: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2678=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2678[ 0]=({ struct Cyc_Stmt_tok_struct _temp2679; _temp2679.tag= Cyc_Stmt_tok;
_temp2679.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2680=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2680[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2681; _temp2681.tag= Cyc_Absyn_Cut_s;
_temp2681.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2681;}); _temp2680;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2679;});
_temp2678;}); break; case 232: _LL2677: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2683=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2683[ 0]=({ struct Cyc_Stmt_tok_struct _temp2684; _temp2684.tag= Cyc_Stmt_tok;
_temp2684.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2685=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2685[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2686; _temp2686.tag= Cyc_Absyn_Splice_s;
_temp2686.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2686;}); _temp2685;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2684;});
_temp2683;}); break; case 233: _LL2682: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2688=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2688[ 0]=({ struct Cyc_Stmt_tok_struct _temp2689; _temp2689.tag= Cyc_Stmt_tok;
_temp2689.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2690=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2690[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2691; _temp2691.tag= Cyc_Absyn_Label_s;
_temp2691.f1=({ struct _tagged_arr* _temp2692=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2692[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2692;});
_temp2691.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2691;}); _temp2690;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2689;});
_temp2688;}); break; case 234: _LL2687: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2694=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2694[ 0]=({ struct Cyc_Stmt_tok_struct _temp2695; _temp2695.tag= Cyc_Stmt_tok;
_temp2695.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2695;});
_temp2694;}); break; case 235: _LL2693: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2697=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2697[ 0]=({ struct Cyc_Stmt_tok_struct _temp2698; _temp2698.tag= Cyc_Stmt_tok;
_temp2698.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2698;}); _temp2697;}); break; case 236: _LL2696: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2700=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2700[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2701; _temp2701.tag= Cyc_Stmt_tok; _temp2701.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2701;}); _temp2700;}); break; case 237: _LL2699: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 238:
_LL2702:{ void* _temp2704= Cyc_yyget_BlockItem_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); struct Cyc_List_List*
_temp2712; struct Cyc_Absyn_Fndecl* _temp2714; struct Cyc_Absyn_Stmt* _temp2716;
_LL2706: if(*(( int*) _temp2704) == Cyc_Parse_TopDecls_bl){ _LL2713: _temp2712=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2704)->f1; goto _LL2707;} else{ goto
_LL2708;} _LL2708: if(*(( int*) _temp2704) == Cyc_Parse_FnDecl_bl){ _LL2715:
_temp2714=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2704)->f1; goto _LL2709;}
else{ goto _LL2710;} _LL2710: if(*(( int*) _temp2704) == Cyc_Parse_Stmt_bl){
_LL2717: _temp2716=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2704)->f1; goto
_LL2711;} else{ goto _LL2705;} _LL2707: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2718=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2718[ 0]=({ struct Cyc_Stmt_tok_struct _temp2719; _temp2719.tag= Cyc_Stmt_tok;
_temp2719.f1= Cyc_Parse_flatten_declarations( _temp2712, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2719;}); _temp2718;}); goto _LL2705; _LL2709:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2720=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2720[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2721; _temp2721.tag= Cyc_Stmt_tok; _temp2721.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2722=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2722[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2723; _temp2723.tag= Cyc_Absyn_Fn_d;
_temp2723.f1= _temp2714; _temp2723;}); _temp2722;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2721;}); _temp2720;}); goto _LL2705; _LL2711:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2724=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2724[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2725; _temp2725.tag= Cyc_Stmt_tok; _temp2725.f1= _temp2716; _temp2725;});
_temp2724;}); goto _LL2705; _LL2705:;} break; case 239: _LL2703:{ void*
_temp2727= Cyc_yyget_BlockItem_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); struct Cyc_List_List* _temp2735; struct Cyc_Absyn_Fndecl*
_temp2737; struct Cyc_Absyn_Stmt* _temp2739; _LL2729: if(*(( int*) _temp2727) ==
Cyc_Parse_TopDecls_bl){ _LL2736: _temp2735=(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp2727)->f1; goto _LL2730;} else{ goto _LL2731;} _LL2731: if(*(( int*)
_temp2727) == Cyc_Parse_FnDecl_bl){ _LL2738: _temp2737=(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp2727)->f1; goto _LL2732;} else{ goto _LL2733;} _LL2733: if(*(( int*)
_temp2727) == Cyc_Parse_Stmt_bl){ _LL2740: _temp2739=(( struct Cyc_Parse_Stmt_bl_struct*)
_temp2727)->f1; goto _LL2734;} else{ goto _LL2728;} _LL2730: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2741=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2741[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2742; _temp2742.tag= Cyc_Stmt_tok; _temp2742.f1= Cyc_Parse_flatten_declarations(
_temp2735, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2742;}); _temp2741;}); goto _LL2728; _LL2732: yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2743=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2743[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2744; _temp2744.tag= Cyc_Stmt_tok; _temp2744.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2745=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2745[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2746; _temp2746.tag= Cyc_Absyn_Fn_d;
_temp2746.f1= _temp2737; _temp2746;}); _temp2745;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2744;}); _temp2743;}); goto _LL2728; _LL2734: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2747=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2747[ 0]=({ struct Cyc_Stmt_tok_struct _temp2748; _temp2748.tag= Cyc_Stmt_tok;
_temp2748.f1= Cyc_Absyn_seq_stmt( _temp2739, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2748;}); _temp2747;}); goto _LL2728; _LL2728:;} break; case 240: _LL2726:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2750=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp2750[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp2751; _temp2751.tag= Cyc_BlockItem_tok; _temp2751.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp2752=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp2752[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp2753; _temp2753.tag= Cyc_Parse_TopDecls_bl;
_temp2753.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2753;}); _temp2752;})); _temp2751;}); _temp2750;});
break; case 241: _LL2749: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp2755=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2755[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2756; _temp2756.tag= Cyc_BlockItem_tok;
_temp2756.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp2757=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp2757[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp2758; _temp2758.tag= Cyc_Parse_Stmt_bl;
_temp2758.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2758;}); _temp2757;})); _temp2756;}); _temp2755;}); break;
case 242: _LL2754: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2760=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2760[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2761; _temp2761.tag= Cyc_BlockItem_tok;
_temp2761.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp2762=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp2762[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp2763; _temp2763.tag= Cyc_Parse_FnDecl_bl;
_temp2763.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2763;}); _temp2762;})); _temp2761;}); _temp2760;}); break;
case 243: _LL2759: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2765=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2765[ 0]=({ struct Cyc_Stmt_tok_struct _temp2766; _temp2766.tag= Cyc_Stmt_tok;
_temp2766.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2766;}); _temp2765;}); break; case 244:
_LL2764: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2768=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2768[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2769; _temp2769.tag= Cyc_Stmt_tok; _temp2769.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2769;});
_temp2768;}); break; case 245: _LL2767: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2771=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2771[ 0]=({ struct Cyc_Stmt_tok_struct _temp2772; _temp2772.tag= Cyc_Stmt_tok;
_temp2772.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2772;}); _temp2771;}); break; case 246: _LL2770: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2774=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2774[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2775; _temp2775.tag= Cyc_Stmt_tok; _temp2775.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2776=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2776[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2777; _temp2777.tag= Cyc_Absyn_SwitchC_s;
_temp2777.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2777.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2777;});
_temp2776;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2775;}); _temp2774;}); break; case 247:
_LL2773: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2779=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2779[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2780; _temp2780.tag= Cyc_Stmt_tok; _temp2780.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2780;});
_temp2779;}); break; case 248: _LL2778: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2782=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2782[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2783; _temp2783.tag= Cyc_SwitchClauseList_tok; _temp2783.f1= 0; _temp2783;});
_temp2782;}); break; case 249: _LL2781: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2785=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2785[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2786; _temp2786.tag= Cyc_SwitchClauseList_tok; _temp2786.f1=({ struct Cyc_List_List*
_temp2787=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2787->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2788=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2788->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2788->pat_vars= 0; _temp2788->where_clause= 0; _temp2788->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2788->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2788;}); _temp2787->tl= 0; _temp2787;});
_temp2786;}); _temp2785;}); break; case 250: _LL2784: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2790=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2790[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2791; _temp2791.tag= Cyc_SwitchClauseList_tok;
_temp2791.f1=({ struct Cyc_List_List* _temp2792=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2792->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2793=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2793->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2793->pat_vars= 0; _temp2793->where_clause= 0;
_temp2793->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2793->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2793;});
_temp2792->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2792;}); _temp2791;});
_temp2790;}); break; case 251: _LL2789: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2795=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2795[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2796; _temp2796.tag= Cyc_SwitchClauseList_tok; _temp2796.f1=({ struct Cyc_List_List*
_temp2797=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2797->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2798=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2798->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2798->pat_vars=
0; _temp2798->where_clause= 0; _temp2798->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2798->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2798;}); _temp2797->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2797;}); _temp2796;});
_temp2795;}); break; case 252: _LL2794: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2800=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2800[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2801; _temp2801.tag= Cyc_SwitchClauseList_tok; _temp2801.f1=({ struct Cyc_List_List*
_temp2802=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2802->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2803=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2803->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2803->pat_vars=
0; _temp2803->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2803->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2803->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2803;});
_temp2802->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2802;}); _temp2801;});
_temp2800;}); break; case 253: _LL2799: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2805=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2805[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2806; _temp2806.tag= Cyc_SwitchClauseList_tok; _temp2806.f1=({ struct Cyc_List_List*
_temp2807=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2807->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2808=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2808->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2808->pat_vars=
0; _temp2808->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2808->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2808->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2808;});
_temp2807->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2807;}); _temp2806;});
_temp2805;}); break; case 254: _LL2804: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2810=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2810[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2811; _temp2811.tag= Cyc_SwitchCClauseList_tok; _temp2811.f1= 0; _temp2811;});
_temp2810;}); break; case 255: _LL2809: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2813=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2813[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2814; _temp2814.tag= Cyc_SwitchCClauseList_tok; _temp2814.f1=({ struct Cyc_List_List*
_temp2815=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2815->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2816=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2816->cnst_exp= 0;
_temp2816->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2816->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2816;});
_temp2815->tl= 0; _temp2815;}); _temp2814;}); _temp2813;}); break; case 256:
_LL2812: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2818=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2818[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2819; _temp2819.tag=
Cyc_SwitchCClauseList_tok; _temp2819.f1=({ struct Cyc_List_List* _temp2820=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2820->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2821=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2821->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2821->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2821->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2821;});
_temp2820->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2820;}); _temp2819;});
_temp2818;}); break; case 257: _LL2817: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2823=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2823[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2824; _temp2824.tag= Cyc_SwitchCClauseList_tok; _temp2824.f1=({ struct Cyc_List_List*
_temp2825=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2825->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2826=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2826->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2826->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2826->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2826;});
_temp2825->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2825;}); _temp2824;});
_temp2823;}); break; case 258: _LL2822: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2828=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2828[ 0]=({ struct Cyc_Stmt_tok_struct _temp2829; _temp2829.tag= Cyc_Stmt_tok;
_temp2829.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2829;}); _temp2828;}); break; case 259: _LL2827: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2831=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2831[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2832; _temp2832.tag= Cyc_Stmt_tok; _temp2832.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2832;}); _temp2831;}); break; case 260: _LL2830: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2834=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2834[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2835; _temp2835.tag= Cyc_Stmt_tok; _temp2835.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2835;}); _temp2834;}); break; case 261: _LL2833: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2837=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2837[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2838; _temp2838.tag= Cyc_Stmt_tok; _temp2838.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2838;}); _temp2837;}); break; case 262: _LL2836: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2840=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2840[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2841; _temp2841.tag= Cyc_Stmt_tok; _temp2841.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2841;}); _temp2840;}); break; case 263: _LL2839: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2843=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2843[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2844; _temp2844.tag= Cyc_Stmt_tok; _temp2844.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2844;}); _temp2843;}); break; case 264: _LL2842: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2846=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2846[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2847; _temp2847.tag= Cyc_Stmt_tok; _temp2847.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2847;}); _temp2846;}); break; case 265: _LL2845: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2849=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2849[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2850; _temp2850.tag= Cyc_Stmt_tok; _temp2850.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2850;});
_temp2849;}); break; case 266: _LL2848: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2852=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2852[ 0]=({ struct Cyc_Stmt_tok_struct _temp2853; _temp2853.tag= Cyc_Stmt_tok;
_temp2853.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2853;});
_temp2852;}); break; case 267: _LL2851: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2855=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2855[ 0]=({ struct Cyc_Stmt_tok_struct _temp2856; _temp2856.tag= Cyc_Stmt_tok;
_temp2856.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2856;}); _temp2855;}); break; case 268: _LL2854: { struct Cyc_List_List*
_temp2858= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2859= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2860=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2860[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2861; _temp2861.tag= Cyc_Stmt_tok; _temp2861.f1= Cyc_Parse_flatten_declarations(
_temp2858, _temp2859); _temp2861;}); _temp2860;}); break;} case 269: _LL2857: {
struct Cyc_List_List* _temp2863= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2864= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2865=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2865[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2866; _temp2866.tag= Cyc_Stmt_tok; _temp2866.f1= Cyc_Parse_flatten_declarations(
_temp2863, _temp2864); _temp2866;}); _temp2865;}); break;} case 270: _LL2862: {
struct Cyc_List_List* _temp2868= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2869= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2870=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2870[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2871; _temp2871.tag= Cyc_Stmt_tok; _temp2871.f1= Cyc_Parse_flatten_declarations(
_temp2868, _temp2869); _temp2871;}); _temp2870;}); break;} case 271: _LL2867: {
struct Cyc_List_List* _temp2873= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2874= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2875=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2875[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2876; _temp2876.tag= Cyc_Stmt_tok; _temp2876.f1= Cyc_Parse_flatten_declarations(
_temp2873, _temp2874); _temp2876;}); _temp2875;}); break;} case 272: _LL2872:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2878=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2878[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2879; _temp2879.tag= Cyc_Stmt_tok; _temp2879.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2880=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2880[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2880;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2879;}); _temp2878;}); break; case 273: _LL2877: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2882=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2882[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2883; _temp2883.tag= Cyc_Stmt_tok; _temp2883.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2883;}); _temp2882;}); break; case 274:
_LL2881: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2885=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2885[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2886; _temp2886.tag= Cyc_Stmt_tok; _temp2886.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2886;}); _temp2885;}); break; case 275: _LL2884: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2888=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2888[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2889; _temp2889.tag= Cyc_Stmt_tok; _temp2889.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2889;}); _temp2888;}); break; case 276:
_LL2887: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2891=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2891[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2892; _temp2892.tag= Cyc_Stmt_tok; _temp2892.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2892;}); _temp2891;}); break; case 277: _LL2890: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2894=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2894[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2895; _temp2895.tag= Cyc_Stmt_tok; _temp2895.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2895;}); _temp2894;}); break; case 278:
_LL2893: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2897=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2897[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2898; _temp2898.tag= Cyc_Stmt_tok; _temp2898.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2898;}); _temp2897;}); break; case 279:
_LL2896: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2900=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2900[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2901; _temp2901.tag= Cyc_Stmt_tok; _temp2901.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2901;}); _temp2900;}); break; case 280: _LL2899: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2903=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2903[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2904; _temp2904.tag= Cyc_Pattern_tok; _temp2904.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2904;});
_temp2903;}); break; case 281: _LL2902: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 282:
_LL2905: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2907=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2907[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2908; _temp2908.tag= Cyc_Pattern_tok; _temp2908.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2909=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2909[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2910; _temp2910.tag= Cyc_Absyn_Int_p; _temp2910.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2910.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2910;}); _temp2909;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2908;});
_temp2907;}); break; case 283: _LL2906: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2912=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2912[ 0]=({ struct Cyc_Pattern_tok_struct _temp2913; _temp2913.tag= Cyc_Pattern_tok;
_temp2913.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2914=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2914[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2915; _temp2915.tag= Cyc_Absyn_Int_p;
_temp2915.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2915.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2915;});
_temp2914;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2913;}); _temp2912;}); break; case 284:
_LL2911: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2917=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2917[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2918; _temp2918.tag= Cyc_Pattern_tok; _temp2918.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2919=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2919[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2920; _temp2920.tag= Cyc_Absyn_Float_p; _temp2920.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2920;}); _temp2919;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2918;});
_temp2917;}); break; case 285: _LL2916: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2922=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2922[ 0]=({ struct Cyc_Pattern_tok_struct _temp2923; _temp2923.tag= Cyc_Pattern_tok;
_temp2923.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2924=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2924[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2925; _temp2925.tag= Cyc_Absyn_Char_p; _temp2925.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2925;});
_temp2924;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2923;}); _temp2922;}); break; case 286:
_LL2921: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2927=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2927[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2928; _temp2928.tag= Cyc_Pattern_tok; _temp2928.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2928;});
_temp2927;}); break; case 287: _LL2926: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2930=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2930[ 0]=({ struct Cyc_Pattern_tok_struct _temp2931; _temp2931.tag= Cyc_Pattern_tok;
_temp2931.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2932=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2932[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2933; _temp2933.tag=
Cyc_Absyn_UnknownId_p; _temp2933.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2933;}); _temp2932;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2931;}); _temp2930;}); break; case 288:
_LL2929: { struct Cyc_List_List* _temp2935=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2936=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2936[ 0]=({ struct Cyc_Pattern_tok_struct _temp2937; _temp2937.tag= Cyc_Pattern_tok;
_temp2937.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2938=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2938[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2939; _temp2939.tag= Cyc_Absyn_UnknownCall_p; _temp2939.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2939.f2=
_temp2935; _temp2939.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2939;});
_temp2938;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2937;}); _temp2936;}); break;} case
289: _LL2934: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2941=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2941[ 0]=({ struct Cyc_Pattern_tok_struct _temp2942; _temp2942.tag= Cyc_Pattern_tok;
_temp2942.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2943=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2943[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2944; _temp2944.tag= Cyc_Absyn_Tuple_p;
_temp2944.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2944;}); _temp2943;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2942;}); _temp2941;}); break; case 290: _LL2940: { struct Cyc_List_List*
_temp2946=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2947=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2947[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2948; _temp2948.tag= Cyc_Pattern_tok; _temp2948.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2949=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2949[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2950; _temp2950.tag= Cyc_Absyn_UnknownFields_p;
_temp2950.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2950.f2= _temp2946; _temp2950.f3= 0; _temp2950;});
_temp2949;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2948;}); _temp2947;}); break;} case
291: _LL2945: { struct Cyc_List_List* _temp2952=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2953=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2953[ 0]=({ struct Cyc_Pattern_tok_struct _temp2954; _temp2954.tag= Cyc_Pattern_tok;
_temp2954.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2955=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2955[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2956; _temp2956.tag= Cyc_Absyn_UnknownFields_p; _temp2956.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2956.f2=
_temp2952; _temp2956.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2956;});
_temp2955;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2954;}); _temp2953;}); break;} case
292: _LL2951: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2958=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2958[ 0]=({ struct Cyc_Pattern_tok_struct _temp2959; _temp2959.tag= Cyc_Pattern_tok;
_temp2959.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2960=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2960[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2961; _temp2961.tag= Cyc_Absyn_Pointer_p;
_temp2961.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2961;}); _temp2960;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2959;});
_temp2958;}); break; case 293: _LL2957: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2963=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2963[ 0]=({ struct Cyc_Pattern_tok_struct _temp2964; _temp2964.tag= Cyc_Pattern_tok;
_temp2964.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2965=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2965[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2966; _temp2966.tag=
Cyc_Absyn_Reference_p; _temp2966.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2967=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2967->f1=(
void*) Cyc_Absyn_Loc_n; _temp2967->f2=({ struct _tagged_arr* _temp2968=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2968[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2968;});
_temp2967;}),( void*) Cyc_Absyn_VoidType, 0); _temp2966;}); _temp2965;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2964;}); _temp2963;}); break; case 294: _LL2962: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2970=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2970[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2971; _temp2971.tag= Cyc_PatternList_tok;
_temp2971.f1= 0; _temp2971;}); _temp2970;}); break; case 295: _LL2969: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2973=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2973[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2974; _temp2974.tag= Cyc_PatternList_tok;
_temp2974.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2974;}); _temp2973;}); break; case 296: _LL2972: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2976=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2976[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2977; _temp2977.tag= Cyc_PatternList_tok;
_temp2977.f1=({ struct Cyc_List_List* _temp2978=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2978->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2978->tl= 0;
_temp2978;}); _temp2977;}); _temp2976;}); break; case 297: _LL2975: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2980=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2980[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2981; _temp2981.tag= Cyc_PatternList_tok;
_temp2981.f1=({ struct Cyc_List_List* _temp2982=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2982->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2982->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2982;}); _temp2981;}); _temp2980;}); break; case 298:
_LL2979: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2984=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2984[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2985; _temp2985.tag=
Cyc_FieldPattern_tok; _temp2985.f1=({ struct _tuple14* _temp2986=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2986->f1= 0; _temp2986->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2986;}); _temp2985;}); _temp2984;}); break; case 299:
_LL2983: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2988=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2988[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2989; _temp2989.tag=
Cyc_FieldPattern_tok; _temp2989.f1=({ struct _tuple14* _temp2990=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2990->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2990->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2990;}); _temp2989;}); _temp2988;}); break; case 300:
_LL2987: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2992=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2992[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2993; _temp2993.tag=
Cyc_FieldPatternList_tok; _temp2993.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2993;});
_temp2992;}); break; case 301: _LL2991: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2995=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2995[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2996; _temp2996.tag= Cyc_FieldPatternList_tok; _temp2996.f1=({ struct Cyc_List_List*
_temp2997=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2997->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2997->tl= 0;
_temp2997;}); _temp2996;}); _temp2995;}); break; case 302: _LL2994: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2999=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2999[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3000; _temp3000.tag= Cyc_FieldPatternList_tok;
_temp3000.f1=({ struct Cyc_List_List* _temp3001=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3001->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3001->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp3001;}); _temp3000;}); _temp2999;}); break; case 303:
_LL2998: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL3002: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3004=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3004[ 0]=({ struct Cyc_Exp_tok_struct _temp3005; _temp3005.tag= Cyc_Exp_tok;
_temp3005.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3005;}); _temp3004;}); break; case 305: _LL3003: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL3006:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3008=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3008[ 0]=({ struct Cyc_Exp_tok_struct
_temp3009; _temp3009.tag= Cyc_Exp_tok; _temp3009.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3009;}); _temp3008;}); break; case 307: _LL3007: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3011=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3011[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3012; _temp3012.tag= Cyc_Primopopt_tok; _temp3012.f1=
0; _temp3012;}); _temp3011;}); break; case 308: _LL3010: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3014=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3014[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3015; _temp3015.tag= Cyc_Primopopt_tok; _temp3015.f1=({
struct Cyc_Core_Opt* _temp3016=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3016->v=( void*)(( void*) Cyc_Absyn_Times); _temp3016;});
_temp3015;}); _temp3014;}); break; case 309: _LL3013: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3018=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3018[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3019; _temp3019.tag= Cyc_Primopopt_tok; _temp3019.f1=({
struct Cyc_Core_Opt* _temp3020=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3020->v=( void*)(( void*) Cyc_Absyn_Div); _temp3020;});
_temp3019;}); _temp3018;}); break; case 310: _LL3017: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3022=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3022[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3023; _temp3023.tag= Cyc_Primopopt_tok; _temp3023.f1=({
struct Cyc_Core_Opt* _temp3024=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3024->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3024;});
_temp3023;}); _temp3022;}); break; case 311: _LL3021: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3026=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3026[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3027; _temp3027.tag= Cyc_Primopopt_tok; _temp3027.f1=({
struct Cyc_Core_Opt* _temp3028=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3028->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3028;});
_temp3027;}); _temp3026;}); break; case 312: _LL3025: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3030=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3030[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3031; _temp3031.tag= Cyc_Primopopt_tok; _temp3031.f1=({
struct Cyc_Core_Opt* _temp3032=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3032->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3032;});
_temp3031;}); _temp3030;}); break; case 313: _LL3029: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3034=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3034[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3035; _temp3035.tag= Cyc_Primopopt_tok; _temp3035.f1=({
struct Cyc_Core_Opt* _temp3036=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3036->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3036;});
_temp3035;}); _temp3034;}); break; case 314: _LL3033: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3038=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3038[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3039; _temp3039.tag= Cyc_Primopopt_tok; _temp3039.f1=({
struct Cyc_Core_Opt* _temp3040=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3040->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3040;});
_temp3039;}); _temp3038;}); break; case 315: _LL3037: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3042=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3042[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3043; _temp3043.tag= Cyc_Primopopt_tok; _temp3043.f1=({
struct Cyc_Core_Opt* _temp3044=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3044->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3044;});
_temp3043;}); _temp3042;}); break; case 316: _LL3041: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3046=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3046[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3047; _temp3047.tag= Cyc_Primopopt_tok; _temp3047.f1=({
struct Cyc_Core_Opt* _temp3048=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3048->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3048;});
_temp3047;}); _temp3046;}); break; case 317: _LL3045: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3050=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3050[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3051; _temp3051.tag= Cyc_Primopopt_tok; _temp3051.f1=({
struct Cyc_Core_Opt* _temp3052=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3052->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3052;});
_temp3051;}); _temp3050;}); break; case 318: _LL3049: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL3053:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3055=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3055[ 0]=({ struct Cyc_Exp_tok_struct
_temp3056; _temp3056.tag= Cyc_Exp_tok; _temp3056.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3056;});
_temp3055;}); break; case 320: _LL3054: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3058=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3058[ 0]=({ struct Cyc_Exp_tok_struct _temp3059; _temp3059.tag= Cyc_Exp_tok;
_temp3059.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3059;}); _temp3058;}); break; case 321: _LL3057: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3061=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3061[ 0]=({ struct Cyc_Exp_tok_struct
_temp3062; _temp3062.tag= Cyc_Exp_tok; _temp3062.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3062;}); _temp3061;}); break; case 322: _LL3060: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3064=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3064[ 0]=({ struct Cyc_Exp_tok_struct
_temp3065; _temp3065.tag= Cyc_Exp_tok; _temp3065.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3065;}); _temp3064;}); break; case 323: _LL3063: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3067=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3067[ 0]=({ struct Cyc_Exp_tok_struct
_temp3068; _temp3068.tag= Cyc_Exp_tok; _temp3068.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3068;});
_temp3067;}); break; case 324: _LL3066: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3070=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3070[ 0]=({ struct Cyc_Exp_tok_struct _temp3071; _temp3071.tag= Cyc_Exp_tok;
_temp3071.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3071;}); _temp3070;}); break; case 325: _LL3069: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL3072:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL3073: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3075=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3075[ 0]=({ struct Cyc_Exp_tok_struct
_temp3076; _temp3076.tag= Cyc_Exp_tok; _temp3076.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3076;}); _temp3075;}); break; case 328: _LL3074: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL3077:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3079=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3079[ 0]=({ struct Cyc_Exp_tok_struct
_temp3080; _temp3080.tag= Cyc_Exp_tok; _temp3080.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3080;}); _temp3079;}); break; case 330: _LL3078: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL3081:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3083=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3083[ 0]=({ struct Cyc_Exp_tok_struct
_temp3084; _temp3084.tag= Cyc_Exp_tok; _temp3084.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3084;});
_temp3083;}); break; case 332: _LL3082: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL3085:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3087=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3087[ 0]=({ struct Cyc_Exp_tok_struct
_temp3088; _temp3088.tag= Cyc_Exp_tok; _temp3088.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3088;});
_temp3087;}); break; case 334: _LL3086: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL3089:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3091=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3091[ 0]=({ struct Cyc_Exp_tok_struct
_temp3092; _temp3092.tag= Cyc_Exp_tok; _temp3092.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3092;});
_temp3091;}); break; case 336: _LL3090: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL3093:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3095=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3095[ 0]=({ struct Cyc_Exp_tok_struct
_temp3096; _temp3096.tag= Cyc_Exp_tok; _temp3096.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3096;}); _temp3095;}); break; case 338: _LL3094: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3098=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3098[ 0]=({ struct Cyc_Exp_tok_struct
_temp3099; _temp3099.tag= Cyc_Exp_tok; _temp3099.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3099;}); _temp3098;}); break; case 339: _LL3097: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL3100:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3102=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3102[ 0]=({ struct Cyc_Exp_tok_struct
_temp3103; _temp3103.tag= Cyc_Exp_tok; _temp3103.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3103;}); _temp3102;}); break; case 341: _LL3101: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3105=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3105[ 0]=({ struct Cyc_Exp_tok_struct
_temp3106; _temp3106.tag= Cyc_Exp_tok; _temp3106.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3106;}); _temp3105;}); break; case 342: _LL3104: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3108=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3108[ 0]=({ struct Cyc_Exp_tok_struct
_temp3109; _temp3109.tag= Cyc_Exp_tok; _temp3109.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3109;}); _temp3108;}); break; case 343: _LL3107: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3111=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3111[ 0]=({ struct Cyc_Exp_tok_struct
_temp3112; _temp3112.tag= Cyc_Exp_tok; _temp3112.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3112;}); _temp3111;}); break; case 344: _LL3110: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL3113:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3115=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3115[ 0]=({ struct Cyc_Exp_tok_struct
_temp3116; _temp3116.tag= Cyc_Exp_tok; _temp3116.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3116;}); _temp3115;}); break; case 346: _LL3114: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3118=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3118[ 0]=({ struct Cyc_Exp_tok_struct
_temp3119; _temp3119.tag= Cyc_Exp_tok; _temp3119.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3119;}); _temp3118;}); break; case 347: _LL3117: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL3120:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3122=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3122[ 0]=({ struct Cyc_Exp_tok_struct
_temp3123; _temp3123.tag= Cyc_Exp_tok; _temp3123.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3123;});
_temp3122;}); break; case 349: _LL3121: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3125=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3125[ 0]=({ struct Cyc_Exp_tok_struct _temp3126; _temp3126.tag= Cyc_Exp_tok;
_temp3126.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3126;}); _temp3125;}); break; case 350: _LL3124: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL3127:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3129=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3129[ 0]=({ struct Cyc_Exp_tok_struct
_temp3130; _temp3130.tag= Cyc_Exp_tok; _temp3130.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3130;});
_temp3129;}); break; case 352: _LL3128: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3132=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3132[ 0]=({ struct Cyc_Exp_tok_struct _temp3133; _temp3133.tag= Cyc_Exp_tok;
_temp3133.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3133;}); _temp3132;}); break; case 353: _LL3131: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3135=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3135[ 0]=({ struct Cyc_Exp_tok_struct
_temp3136; _temp3136.tag= Cyc_Exp_tok; _temp3136.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3136;});
_temp3135;}); break; case 354: _LL3134: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3137:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3139=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3139[ 0]=({ struct Cyc_Exp_tok_struct
_temp3140; _temp3140.tag= Cyc_Exp_tok; _temp3140.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3140;}); _temp3139;}); break; case 356: _LL3138: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3141:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3143=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3143[ 0]=({ struct Cyc_Exp_tok_struct
_temp3144; _temp3144.tag= Cyc_Exp_tok; _temp3144.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3144;}); _temp3143;}); break; case 358: _LL3142: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3146=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3146[ 0]=({ struct Cyc_Exp_tok_struct
_temp3147; _temp3147.tag= Cyc_Exp_tok; _temp3147.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3147;}); _temp3146;}); break; case 359: _LL3145: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3149=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3149[ 0]=({ struct Cyc_Exp_tok_struct
_temp3150; _temp3150.tag= Cyc_Exp_tok; _temp3150.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3150;}); _temp3149;}); break; case 360: _LL3148: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3152=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3152[ 0]=({ struct Cyc_Exp_tok_struct
_temp3153; _temp3153.tag= Cyc_Exp_tok; _temp3153.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3153;}); _temp3152;}); break; case 361: _LL3151: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL3154:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3156=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3156[ 0]=({ struct Cyc_Exp_tok_struct
_temp3157; _temp3157.tag= Cyc_Exp_tok; _temp3157.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3157;}); _temp3156;}); break; case 363: _LL3155: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3159=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3159[ 0]=({ struct Cyc_Exp_tok_struct
_temp3160; _temp3160.tag= Cyc_Exp_tok; _temp3160.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3160;});
_temp3159;}); break; case 364: _LL3158: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3162=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3162[ 0]=({ struct Cyc_Exp_tok_struct _temp3163; _temp3163.tag= Cyc_Exp_tok;
_temp3163.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3163;}); _temp3162;}); break; case 365: _LL3161: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3165=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3165[ 0]=({ struct Cyc_Exp_tok_struct
_temp3166; _temp3166.tag= Cyc_Exp_tok; _temp3166.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3167=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3167[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3167;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3166;}); _temp3165;}); break; case 366:
_LL3164: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3169=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3169[ 0]=({ struct Cyc_Exp_tok_struct
_temp3170; _temp3170.tag= Cyc_Exp_tok; _temp3170.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3171=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3171[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3172; _temp3172.tag= Cyc_Absyn_Malloc_e;
_temp3172.f1= 0; _temp3172.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3172;}); _temp3171;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3170;});
_temp3169;}); break; case 367: _LL3168: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3174=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3174[ 0]=({ struct Cyc_Exp_tok_struct _temp3175; _temp3175.tag= Cyc_Exp_tok;
_temp3175.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3176=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3176[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3177; _temp3177.tag= Cyc_Absyn_Malloc_e;
_temp3177.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3177.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3177;}); _temp3176;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3175;});
_temp3174;}); break; case 368: _LL3173: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3179=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3179[ 0]=({ struct Cyc_Primop_tok_struct _temp3180; _temp3180.tag= Cyc_Primop_tok;
_temp3180.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3180;}); _temp3179;});
break; case 369: _LL3178: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3182=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3182[ 0]=({ struct Cyc_Primop_tok_struct _temp3183; _temp3183.tag= Cyc_Primop_tok;
_temp3183.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3183;}); _temp3182;});
break; case 370: _LL3181: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3185=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3185[ 0]=({ struct Cyc_Primop_tok_struct _temp3186; _temp3186.tag= Cyc_Primop_tok;
_temp3186.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3186;}); _temp3185;});
break; case 371: _LL3184: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 372: _LL3187: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3189=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3189[ 0]=({ struct Cyc_Exp_tok_struct _temp3190; _temp3190.tag= Cyc_Exp_tok;
_temp3190.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3190;}); _temp3189;}); break; case 373: _LL3188: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3192=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3192[ 0]=({ struct Cyc_Exp_tok_struct
_temp3193; _temp3193.tag= Cyc_Exp_tok; _temp3193.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3193;}); _temp3192;}); break; case 374:
_LL3191: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3195=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3195[ 0]=({ struct Cyc_Exp_tok_struct
_temp3196; _temp3196.tag= Cyc_Exp_tok; _temp3196.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3196;});
_temp3195;}); break; case 375: _LL3194: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3198=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3198[ 0]=({ struct Cyc_Exp_tok_struct _temp3199; _temp3199.tag= Cyc_Exp_tok;
_temp3199.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3200=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3200[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3200;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3199;}); _temp3198;}); break; case 376: _LL3197: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3202=(*
q).f1; struct Cyc_List_List* _temp3212; struct Cyc_List_List* _temp3214; _LL3204:
if( _temp3202 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3205;} else{ goto _LL3206;}
_LL3206: if(( unsigned int) _temp3202 > 1u?*(( int*) _temp3202) == Cyc_Absyn_Rel_n:
0){ _LL3213: _temp3212=(( struct Cyc_Absyn_Rel_n_struct*) _temp3202)->f1; if(
_temp3212 == 0){ goto _LL3207;} else{ goto _LL3208;}} else{ goto _LL3208;}
_LL3208: if(( unsigned int) _temp3202 > 1u?*(( int*) _temp3202) == Cyc_Absyn_Abs_n:
0){ _LL3215: _temp3214=(( struct Cyc_Absyn_Abs_n_struct*) _temp3202)->f1; if(
_temp3214 == 0){ goto _LL3209;} else{ goto _LL3210;}} else{ goto _LL3210;}
_LL3210: goto _LL3211; _LL3205: goto _LL3203; _LL3207: goto _LL3203; _LL3209:
goto _LL3203; _LL3211: Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3203; _LL3203:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3216=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3216[ 0]=({ struct Cyc_Exp_tok_struct _temp3217; _temp3217.tag= Cyc_Exp_tok;
_temp3217.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3217;}); _temp3216;}); break;} case 377: _LL3201: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3219=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3219[ 0]=({ struct Cyc_Exp_tok_struct
_temp3220; _temp3220.tag= Cyc_Exp_tok; _temp3220.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),({ struct _tagged_arr* _temp3221=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp3221[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3221;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3220;}); _temp3219;}); break; case 378: _LL3218: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3223=(*
q).f1; struct Cyc_List_List* _temp3233; struct Cyc_List_List* _temp3235; _LL3225:
if( _temp3223 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3226;} else{ goto _LL3227;}
_LL3227: if(( unsigned int) _temp3223 > 1u?*(( int*) _temp3223) == Cyc_Absyn_Rel_n:
0){ _LL3234: _temp3233=(( struct Cyc_Absyn_Rel_n_struct*) _temp3223)->f1; if(
_temp3233 == 0){ goto _LL3228;} else{ goto _LL3229;}} else{ goto _LL3229;}
_LL3229: if(( unsigned int) _temp3223 > 1u?*(( int*) _temp3223) == Cyc_Absyn_Abs_n:
0){ _LL3236: _temp3235=(( struct Cyc_Absyn_Abs_n_struct*) _temp3223)->f1; if(
_temp3235 == 0){ goto _LL3230;} else{ goto _LL3231;}} else{ goto _LL3231;}
_LL3231: goto _LL3232; _LL3226: goto _LL3224; _LL3228: goto _LL3224; _LL3230:
goto _LL3224; _LL3232: Cyc_Parse_err( _tag_arr("struct field is qualified with module name",
sizeof( unsigned char), 43u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3224; _LL3224:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3237=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3237[ 0]=({ struct Cyc_Exp_tok_struct _temp3238; _temp3238.tag= Cyc_Exp_tok;
_temp3238.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3238;}); _temp3237;}); break;} case 379: _LL3222: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3240=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3240[ 0]=({ struct Cyc_Exp_tok_struct
_temp3241; _temp3241.tag= Cyc_Exp_tok; _temp3241.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3241;}); _temp3240;}); break; case 380: _LL3239: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3243=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3243[ 0]=({ struct Cyc_Exp_tok_struct
_temp3244; _temp3244.tag= Cyc_Exp_tok; _temp3244.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3244;}); _temp3243;}); break; case 381: _LL3242: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3246=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3246[ 0]=({ struct Cyc_Exp_tok_struct
_temp3247; _temp3247.tag= Cyc_Exp_tok; _temp3247.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3248=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3248[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3249; _temp3249.tag= Cyc_Absyn_CompoundLit_e;
_temp3249.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3249.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3249;});
_temp3248;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3247;}); _temp3246;}); break; case 382:
_LL3245: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3251=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3251[ 0]=({ struct Cyc_Exp_tok_struct
_temp3252; _temp3252.tag= Cyc_Exp_tok; _temp3252.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3253=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3253[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3254; _temp3254.tag= Cyc_Absyn_CompoundLit_e;
_temp3254.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3254.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3254;});
_temp3253;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3252;}); _temp3251;}); break; case 383:
_LL3250: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3256=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3256[ 0]=({ struct Cyc_Exp_tok_struct
_temp3257; _temp3257.tag= Cyc_Exp_tok; _temp3257.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3258=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3258[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3259; _temp3259.tag= Cyc_Absyn_Fill_e; _temp3259.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3259;});
_temp3258;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3257;}); _temp3256;}); break; case 384:
_LL3255: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3261=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3261[ 0]=({ struct Cyc_Exp_tok_struct
_temp3262; _temp3262.tag= Cyc_Exp_tok; _temp3262.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3263=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3263[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3264; _temp3264.tag= Cyc_Absyn_Codegen_e;
_temp3264.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3264;}); _temp3263;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3262;}); _temp3261;}); break; case 385: _LL3260: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3266=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3266[ 0]=({ struct Cyc_Exp_tok_struct
_temp3267; _temp3267.tag= Cyc_Exp_tok; _temp3267.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3268=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3268[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3269; _temp3269.tag= Cyc_Absyn_UnknownId_e;
_temp3269.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3269;}); _temp3268;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3267;});
_temp3266;}); break; case 386: _LL3265: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 387: _LL3270:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3272=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3272[ 0]=({ struct Cyc_Exp_tok_struct
_temp3273; _temp3273.tag= Cyc_Exp_tok; _temp3273.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3273;});
_temp3272;}); break; case 388: _LL3271: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 389:
_LL3274: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3276=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3276[ 0]=({ struct Cyc_Exp_tok_struct
_temp3277; _temp3277.tag= Cyc_Exp_tok; _temp3277.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3278=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3278[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3279; _temp3279.tag=
Cyc_Absyn_UnknownId_e; _temp3279.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3279;});
_temp3278;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3277;});
_temp3276;}); break; case 390: _LL3275: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3281=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3281[ 0]=({ struct Cyc_Exp_tok_struct _temp3282; _temp3282.tag= Cyc_Exp_tok;
_temp3282.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3283=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3283[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3284; _temp3284.tag=
Cyc_Absyn_UnknownId_e; _temp3284.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3284;});
_temp3283;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3282;}); _temp3281;}); break; case 391: _LL3280: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3286=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3286[ 0]=({ struct Cyc_Exp_tok_struct
_temp3287; _temp3287.tag= Cyc_Exp_tok; _temp3287.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3287;}); _temp3286;}); break; case 392: _LL3285: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3289=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3289[ 0]=({ struct Cyc_Exp_tok_struct
_temp3290; _temp3290.tag= Cyc_Exp_tok; _temp3290.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3291=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3291[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3292; _temp3292.tag= Cyc_Absyn_Struct_e;
_temp3292.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3292.f2= 0; _temp3292.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3292.f4= 0;
_temp3292;}); _temp3291;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3290;});
_temp3289;}); break; case 393: _LL3288: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3294=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3294[ 0]=({ struct Cyc_Exp_tok_struct _temp3295; _temp3295.tag= Cyc_Exp_tok;
_temp3295.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3295;}); _temp3294;}); break; case 394: _LL3293: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3297=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3297[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3298; _temp3298.tag= Cyc_ExpList_tok; _temp3298.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3298;});
_temp3297;}); break; case 395: _LL3296: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3300=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3300[ 0]=({ struct Cyc_ExpList_tok_struct _temp3301; _temp3301.tag= Cyc_ExpList_tok;
_temp3301.f1=({ struct Cyc_List_List* _temp3302=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3302->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3302->tl= 0;
_temp3302;}); _temp3301;}); _temp3300;}); break; case 396: _LL3299: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3304=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3304[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3305; _temp3305.tag= Cyc_ExpList_tok; _temp3305.f1=({ struct Cyc_List_List*
_temp3306=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3306->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3306->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3306;});
_temp3305;}); _temp3304;}); break; case 397: _LL3303: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3308=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3308[ 0]=({ struct Cyc_Exp_tok_struct
_temp3309; _temp3309.tag= Cyc_Exp_tok; _temp3309.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3309;});
_temp3308;}); break; case 398: _LL3307: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3311=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3311[ 0]=({ struct Cyc_Exp_tok_struct _temp3312; _temp3312.tag= Cyc_Exp_tok;
_temp3312.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3312;});
_temp3311;}); break; case 399: _LL3310: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3314=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3314[ 0]=({ struct Cyc_Exp_tok_struct _temp3315; _temp3315.tag= Cyc_Exp_tok;
_temp3315.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3315;});
_temp3314;}); break; case 400: _LL3313: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3317=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3317[ 0]=({ struct Cyc_Exp_tok_struct _temp3318; _temp3318.tag= Cyc_Exp_tok;
_temp3318.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3318;});
_temp3317;}); break; case 401: _LL3316: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3320=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3320[ 0]=({ struct Cyc_QualId_tok_struct _temp3321; _temp3321.tag= Cyc_QualId_tok;
_temp3321.f1=({ struct _tuple1* _temp3322=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3322->f1= Cyc_Absyn_rel_ns_null; _temp3322->f2=({ struct
_tagged_arr* _temp3323=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3323[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3323;}); _temp3322;});
_temp3321;}); _temp3320;}); break; case 402: _LL3319: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3324:(
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
unsigned int _temp3326=( unsigned int)( sze + 15); unsigned char* _temp3327=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3326); struct
_tagged_arr _temp3329= _tag_arr( _temp3327, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3328= _temp3326; unsigned int i;
for( i= 0; i < _temp3328; i ++){ _temp3327[ i]='\000';}}; _temp3329;}); Cyc_String_strcpy(
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3339= v; struct
_tuple16* _temp3359; struct _tuple16 _temp3361; int _temp3362; unsigned char
_temp3364; short _temp3366; struct _tagged_arr _temp3368; struct Cyc_Core_Opt*
_temp3370; struct Cyc_Core_Opt* _temp3372; struct Cyc_Core_Opt _temp3374; struct
_tagged_arr* _temp3375; struct _tuple1* _temp3377; struct _tuple1 _temp3379;
struct _tagged_arr* _temp3380; void* _temp3382; _LL3341: if( _temp3339 == Cyc_Okay_tok){
goto _LL3342;} else{ goto _LL3343;} _LL3343: if(*(( void**) _temp3339) == Cyc_Int_tok){
_LL3360: _temp3359=(( struct Cyc_Int_tok_struct*) _temp3339)->f1; _temp3361=*
_temp3359; _LL3363: _temp3362= _temp3361.f2; goto _LL3344;} else{ goto _LL3345;}
_LL3345: if(*(( void**) _temp3339) == Cyc_Char_tok){ _LL3365: _temp3364=((
struct Cyc_Char_tok_struct*) _temp3339)->f1; goto _LL3346;} else{ goto _LL3347;}
_LL3347: if(*(( void**) _temp3339) == Cyc_Short_tok){ _LL3367: _temp3366=((
struct Cyc_Short_tok_struct*) _temp3339)->f1; goto _LL3348;} else{ goto _LL3349;}
_LL3349: if(*(( void**) _temp3339) == Cyc_String_tok){ _LL3369: _temp3368=((
struct Cyc_String_tok_struct*) _temp3339)->f1; goto _LL3350;} else{ goto _LL3351;}
_LL3351: if(*(( void**) _temp3339) == Cyc_Stringopt_tok){ _LL3371: _temp3370=((
struct Cyc_Stringopt_tok_struct*) _temp3339)->f1; if( _temp3370 == 0){ goto
_LL3352;} else{ goto _LL3353;}} else{ goto _LL3353;} _LL3353: if(*(( void**)
_temp3339) == Cyc_Stringopt_tok){ _LL3373: _temp3372=(( struct Cyc_Stringopt_tok_struct*)
_temp3339)->f1; if( _temp3372 == 0){ goto _LL3355;} else{ _temp3374=* _temp3372;
_LL3376: _temp3375=( struct _tagged_arr*) _temp3374.v; goto _LL3354;}} else{
goto _LL3355;} _LL3355: if(*(( void**) _temp3339) == Cyc_QualId_tok){ _LL3378:
_temp3377=(( struct Cyc_QualId_tok_struct*) _temp3339)->f1; _temp3379=*
_temp3377; _LL3383: _temp3382= _temp3379.f1; goto _LL3381; _LL3381: _temp3380=
_temp3379.f2; goto _LL3356;} else{ goto _LL3357;} _LL3357: goto _LL3358; _LL3342:({
int(* _temp3384)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3387= Cyc_Stdio_stderr;
struct _tagged_arr _temp3388= _tag_arr("ok", sizeof( unsigned char), 3u); void*
_temp3385[ 0u]={}; struct _tagged_arr _temp3386={( void*) _temp3385,( void*)
_temp3385,( void*)( _temp3385 + 0u)}; _temp3384( _temp3387, _temp3388, _temp3386);});
goto _LL3340; _LL3344:({ int(* _temp3389)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3393= Cyc_Stdio_stderr; struct _tagged_arr _temp3394= _tag_arr("%d",
sizeof( unsigned char), 3u); struct Cyc_Stdio_Int_pa_struct _temp3395; _temp3395.tag=
Cyc_Stdio_Int_pa; _temp3395.f1=( int)(( unsigned int) _temp3362);{ void*
_temp3392=( void*)& _temp3395; void* _temp3390[ 1u]={ _temp3392}; struct
_tagged_arr _temp3391={( void*) _temp3390,( void*) _temp3390,( void*)( _temp3390
+ 1u)}; _temp3389( _temp3393, _temp3394, _temp3391);}}); goto _LL3340; _LL3346:({
int(* _temp3396)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3400= Cyc_Stdio_stderr;
struct _tagged_arr _temp3401= _tag_arr("%c", sizeof( unsigned char), 3u); struct
Cyc_Stdio_Int_pa_struct _temp3402; _temp3402.tag= Cyc_Stdio_Int_pa; _temp3402.f1=(
int)(( unsigned int)(( int) _temp3364));{ void* _temp3399=( void*)& _temp3402;
void* _temp3397[ 1u]={ _temp3399}; struct _tagged_arr _temp3398={( void*)
_temp3397,( void*) _temp3397,( void*)( _temp3397 + 1u)}; _temp3396( _temp3400,
_temp3401, _temp3398);}}); goto _LL3340; _LL3348:({ int(* _temp3403)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3407= Cyc_Stdio_stderr; struct _tagged_arr _temp3408= _tag_arr("%ds",
sizeof( unsigned char), 4u); struct Cyc_Stdio_Int_pa_struct _temp3409; _temp3409.tag=
Cyc_Stdio_Int_pa; _temp3409.f1=( int)(( unsigned int)(( int) _temp3366));{ void*
_temp3406=( void*)& _temp3409; void* _temp3404[ 1u]={ _temp3406}; struct
_tagged_arr _temp3405={( void*) _temp3404,( void*) _temp3404,( void*)( _temp3404
+ 1u)}; _temp3403( _temp3407, _temp3408, _temp3405);}}); goto _LL3340; _LL3350:({
int(* _temp3410)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3414= Cyc_Stdio_stderr;
struct _tagged_arr _temp3415= _tag_arr("\"%s\"", sizeof( unsigned char), 5u);
struct Cyc_Stdio_String_pa_struct _temp3416; _temp3416.tag= Cyc_Stdio_String_pa;
_temp3416.f1=( struct _tagged_arr) _temp3368;{ void* _temp3413=( void*)&
_temp3416; void* _temp3411[ 1u]={ _temp3413}; struct _tagged_arr _temp3412={(
void*) _temp3411,( void*) _temp3411,( void*)( _temp3411 + 1u)}; _temp3410(
_temp3414, _temp3415, _temp3412);}}); goto _LL3340; _LL3352:({ int(* _temp3417)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp3420= Cyc_Stdio_stderr; struct _tagged_arr
_temp3421= _tag_arr("null", sizeof( unsigned char), 5u); void* _temp3418[ 0u]={};
struct _tagged_arr _temp3419={( void*) _temp3418,( void*) _temp3418,( void*)(
_temp3418 + 0u)}; _temp3417( _temp3420, _temp3421, _temp3419);}); goto _LL3340;
_LL3354:({ int(* _temp3422)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3426= Cyc_Stdio_stderr;
struct _tagged_arr _temp3427= _tag_arr("\"%s\"", sizeof( unsigned char), 5u);
struct Cyc_Stdio_String_pa_struct _temp3428; _temp3428.tag= Cyc_Stdio_String_pa;
_temp3428.f1=( struct _tagged_arr)* _temp3375;{ void* _temp3425=( void*)&
_temp3428; void* _temp3423[ 1u]={ _temp3425}; struct _tagged_arr _temp3424={(
void*) _temp3423,( void*) _temp3423,( void*)( _temp3423 + 1u)}; _temp3422(
_temp3426, _temp3427, _temp3424);}}); goto _LL3340; _LL3356: { struct Cyc_List_List*
_temp3429= 0;{ void* _temp3430= _temp3382; struct Cyc_List_List* _temp3438;
struct Cyc_List_List* _temp3440; _LL3432: if(( unsigned int) _temp3430 > 1u?*((
int*) _temp3430) == Cyc_Absyn_Rel_n: 0){ _LL3439: _temp3438=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3430)->f1; goto _LL3433;} else{ goto _LL3434;} _LL3434: if(( unsigned int)
_temp3430 > 1u?*(( int*) _temp3430) == Cyc_Absyn_Abs_n: 0){ _LL3441: _temp3440=((
struct Cyc_Absyn_Abs_n_struct*) _temp3430)->f1; goto _LL3435;} else{ goto
_LL3436;} _LL3436: if( _temp3430 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3437;}
else{ goto _LL3431;} _LL3433: _temp3429= _temp3438; goto _LL3431; _LL3435:
_temp3429= _temp3440; goto _LL3431; _LL3437: goto _LL3431; _LL3431:;} for( 0;
_temp3429 != 0; _temp3429=(( struct Cyc_List_List*) _check_null( _temp3429))->tl){({
int(* _temp3442)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp3446= Cyc_Stdio_stderr;
struct _tagged_arr _temp3447= _tag_arr("%s::", sizeof( unsigned char), 5u);
struct Cyc_Stdio_String_pa_struct _temp3448; _temp3448.tag= Cyc_Stdio_String_pa;
_temp3448.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3429))->hd);{ void* _temp3445=( void*)& _temp3448; void*
_temp3443[ 1u]={ _temp3445}; struct _tagged_arr _temp3444={( void*) _temp3443,(
void*) _temp3443,( void*)( _temp3443 + 1u)}; _temp3442( _temp3446, _temp3447,
_temp3444);}});}({ int(* _temp3449)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3453= Cyc_Stdio_stderr; struct _tagged_arr _temp3454= _tag_arr("%s::",
sizeof( unsigned char), 5u); struct Cyc_Stdio_String_pa_struct _temp3455;
_temp3455.tag= Cyc_Stdio_String_pa; _temp3455.f1=( struct _tagged_arr)*
_temp3380;{ void* _temp3452=( void*)& _temp3455; void* _temp3450[ 1u]={
_temp3452}; struct _tagged_arr _temp3451={( void*) _temp3450,( void*) _temp3450,(
void*)( _temp3450 + 1u)}; _temp3449( _temp3453, _temp3454, _temp3451);}}); goto
_LL3340;} _LL3358:({ int(* _temp3456)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3459= Cyc_Stdio_stderr; struct _tagged_arr _temp3460= _tag_arr("?", sizeof(
unsigned char), 2u); void* _temp3457[ 0u]={}; struct _tagged_arr _temp3458={(
void*) _temp3457,( void*) _temp3457,( void*)( _temp3457 + 0u)}; _temp3456(
_temp3459, _temp3460, _temp3458);}); goto _LL3340; _LL3340:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3461=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3461->v=( void*) Cyc_Lexing_from_file(
f); _temp3461;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
