 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_xprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned char
Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag;
struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct
Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr lex_buffer; int
lex_buffer_len; int lex_abs_pos; int lex_start_pos; int lex_curr_pos; int
lex_last_pos; int lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_arr, int, void*); void* read_fun_state; } ;
struct Cyc_Lexing_lex_tables{ struct _tagged_arr lex_base; struct _tagged_arr
lex_backtrk; struct _tagged_arr lex_default; struct _tagged_arr lex_trans;
struct _tagged_arr lex_check; } ; extern struct _tagged_arr Cyc_Lexing_lexeme(
struct Cyc_Lexing_lexbuf*); extern unsigned char Cyc_Lexing_lexeme_char( struct
Cyc_Lexing_lexbuf*, int); extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern void Cyc_Set_iter(
void(* f)( void*), struct Cyc_Set_Set* s); extern unsigned char Cyc_Set_Absent[
11u]; extern int Cyc_String_strlen( struct _tagged_arr s); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_str_sepstr(
struct Cyc_List_List*, struct _tagged_arr); extern struct _tagged_arr Cyc_String_zstrncpy(
struct _tagged_arr, struct _tagged_arr, int); extern struct _tagged_arr Cyc_String_substring(
struct _tagged_arr, int ofs, int n); struct Cyc_Xarray_Xarray{ struct
_tagged_arr elmts; int num_elmts; } ; extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*,
int); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int, void*); extern
void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*); extern int Cyc_Xarray_add_ind(
struct Cyc_Xarray_Xarray*, void*); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; extern struct
Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int); static const int
Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const int
Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*,
struct _tagged_arr); extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
15u]; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value; extern void*
Cyc_Absyn_rel_ns_null; extern struct Cyc_Core_Opt* Cyc_Parse_lbuf; struct Cyc_Declaration_spec;
struct Cyc_Declarator; struct Cyc_Abstractdeclarator; extern unsigned char Cyc_AbstractDeclarator_tok[
27u]; struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern unsigned char Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[
18u]; struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
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
} ; extern struct Cyc_Yyltype Cyc_yylloc; extern void* Cyc_yylval; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
void Cyc_yyerror( struct _tagged_arr s){ Cyc_Position_post_error( Cyc_Position_mk_err_parse(
Cyc_Position_segment_of_abs( Cyc_yylloc.first_line, Cyc_yylloc.last_line), s));}
struct Cyc_Lex_Trie; static const int Cyc_Lex_Zero= 0; static const int Cyc_Lex_One=
0; struct Cyc_Lex_One_struct{ int tag; int f1; struct Cyc_Lex_Trie* f2; } ;
static const int Cyc_Lex_Many= 1; struct Cyc_Lex_Many_struct{ int tag; struct
Cyc_Lex_Trie** f1; } ; struct Cyc_Lex_Trie{ void* children; int shared_str; } ;
static int Cyc_Lex_num_kws= 0; static struct _tagged_arr Cyc_Lex_kw_nums={( void*)
0u,( void*) 0u,( void*) 0u + 0u}; static struct Cyc_Xarray_Xarray* Cyc_Lex_symbols=
0; static struct Cyc_Lex_Trie* Cyc_Lex_ids_trie= 0; static struct Cyc_Lex_Trie*
Cyc_Lex_typedefs_trie= 0; static int Cyc_Lex_comment_depth= 0; static struct
_tuple6 Cyc_Lex_token_int_pair={( void*) 0u, 0}; static unsigned char _temp0[ 8u]="*bogus*";
static struct _tagged_arr Cyc_Lex_bogus_string={ _temp0, _temp0, _temp0 + 8u};
static struct _tuple0 Cyc_Lex_token_id_pair={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Lex_bogus_string}; static unsigned char Cyc_Lex_token_char='\000'; static
unsigned char _temp1[ 1u]=""; static struct _tagged_arr Cyc_Lex_token_string={
_temp1, _temp1, _temp1 + 1u}; static struct _tuple6* Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;
static struct _tuple0* Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair; static int
Cyc_Lex_runaway_start= 0; static void Cyc_Lex_err( struct _tagged_arr msg,
struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s= Cyc_Position_segment_of_abs(
Cyc_Lexing_lexeme_start( lb), Cyc_Lexing_lexeme_end( lb)); Cyc_Position_post_error(
Cyc_Position_mk_err_lex( s, msg));} static void Cyc_Lex_runaway_err( struct
_tagged_arr msg, struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s=
Cyc_Position_segment_of_abs( Cyc_Lex_runaway_start, Cyc_Lexing_lexeme_start( lb));
Cyc_Position_post_error( Cyc_Position_mk_err_lex( s, msg));} struct _tuple9{
struct _tagged_arr f1; short f2; } ; static unsigned char _temp2[ 14u]="__attribute__";
static unsigned char _temp3[ 9u]="abstract"; static unsigned char _temp4[ 5u]="auto";
static unsigned char _temp5[ 6u]="break"; static unsigned char _temp6[ 5u]="case";
static unsigned char _temp7[ 6u]="catch"; static unsigned char _temp8[ 5u]="char";
static unsigned char _temp9[ 8u]="codegen"; static unsigned char _temp10[ 6u]="const";
static unsigned char _temp11[ 9u]="continue"; static unsigned char _temp12[ 4u]="cut";
static unsigned char _temp13[ 8u]="default"; static unsigned char _temp14[ 3u]="do";
static unsigned char _temp15[ 7u]="double"; static unsigned char _temp16[ 5u]="else";
static unsigned char _temp17[ 5u]="enum"; static unsigned char _temp18[ 7u]="extern";
static unsigned char _temp19[ 9u]="fallthru"; static unsigned char _temp20[ 5u]="fill";
static unsigned char _temp21[ 6u]="float"; static unsigned char _temp22[ 4u]="for";
static unsigned char _temp23[ 5u]="goto"; static unsigned char _temp24[ 3u]="if";
static unsigned char _temp25[ 7u]="inline"; static unsigned char _temp26[ 4u]="int";
static unsigned char _temp27[ 4u]="let"; static unsigned char _temp28[ 5u]="long";
static unsigned char _temp29[ 7u]="malloc"; static unsigned char _temp30[ 10u]="namespace";
static unsigned char _temp31[ 4u]="new"; static unsigned char _temp32[ 5u]="null";
static unsigned char _temp33[ 9u]="offsetof"; static unsigned char _temp34[ 9u]="region_t";
static unsigned char _temp35[ 7u]="region"; static unsigned char _temp36[ 9u]="register";
static unsigned char _temp37[ 9u]="restrict"; static unsigned char _temp38[ 7u]="return";
static unsigned char _temp39[ 8u]="rmalloc"; static unsigned char _temp40[ 5u]="rnew";
static unsigned char _temp41[ 6u]="short"; static unsigned char _temp42[ 7u]="signed";
static unsigned char _temp43[ 7u]="sizeof"; static unsigned char _temp44[ 7u]="splice";
static unsigned char _temp45[ 7u]="static"; static unsigned char _temp46[ 7u]="struct";
static unsigned char _temp47[ 7u]="switch"; static unsigned char _temp48[ 6u]="throw";
static unsigned char _temp49[ 4u]="try"; static unsigned char _temp50[ 7u]="tunion";
static unsigned char _temp51[ 8u]="typedef"; static unsigned char _temp52[ 6u]="union";
static unsigned char _temp53[ 9u]="unsigned"; static unsigned char _temp54[ 6u]="using";
static unsigned char _temp55[ 5u]="void"; static unsigned char _temp56[ 9u]="volatile";
static unsigned char _temp57[ 6u]="while"; static unsigned char _temp58[ 8u]="xtunion";
static struct _tuple9 Cyc_Lex_rw_array[ 57u]={{{ _temp2, _temp2, _temp2 + 14u},
352},{{ _temp3, _temp3, _temp3 + 9u}, 299},{{ _temp4, _temp4, _temp4 + 5u}, 258},{{
_temp5, _temp5, _temp5 + 6u}, 290},{{ _temp6, _temp6, _temp6 + 5u}, 277},{{
_temp7, _temp7, _temp7 + 6u}, 297},{{ _temp8, _temp8, _temp8 + 5u}, 264},{{
_temp9, _temp9, _temp9 + 8u}, 306},{{ _temp10, _temp10, _temp10 + 6u}, 272},{{
_temp11, _temp11, _temp11 + 9u}, 289},{{ _temp12, _temp12, _temp12 + 4u}, 307},{{
_temp13, _temp13, _temp13 + 8u}, 278},{{ _temp14, _temp14, _temp14 + 3u}, 286},{{
_temp15, _temp15, _temp15 + 7u}, 269},{{ _temp16, _temp16, _temp16 + 5u}, 283},{{
_temp17, _temp17, _temp17 + 5u}, 292},{{ _temp18, _temp18, _temp18 + 7u}, 261},{{
_temp19, _temp19, _temp19 + 9u}, 300},{{ _temp20, _temp20, _temp20 + 5u}, 305},{{
_temp21, _temp21, _temp21 + 6u}, 268},{{ _temp22, _temp22, _temp22 + 4u}, 287},{{
_temp23, _temp23, _temp23 + 5u}, 288},{{ _temp24, _temp24, _temp24 + 3u}, 282},{{
_temp25, _temp25, _temp25 + 7u}, 279},{{ _temp26, _temp26, _temp26 + 4u}, 266},{{
_temp27, _temp27, _temp27 + 4u}, 294},{{ _temp28, _temp28, _temp28 + 5u}, 267},{{
_temp29, _temp29, _temp29 + 7u}, 315},{{ _temp30, _temp30, _temp30 + 10u}, 302},{{
_temp31, _temp31, _temp31 + 4u}, 298},{{ _temp32, _temp32, _temp32 + 5u}, 293},{{
_temp33, _temp33, _temp33 + 9u}, 281},{{ _temp34, _temp34, _temp34 + 9u}, 316},{{
_temp35, _temp35, _temp35 + 7u}, 317},{{ _temp36, _temp36, _temp36 + 9u}, 259},{{
_temp37, _temp37, _temp37 + 9u}, 274},{{ _temp38, _temp38, _temp38 + 7u}, 291},{{
_temp39, _temp39, _temp39 + 8u}, 319},{{ _temp40, _temp40, _temp40 + 5u}, 318},{{
_temp41, _temp41, _temp41 + 6u}, 265},{{ _temp42, _temp42, _temp42 + 7u}, 270},{{
_temp43, _temp43, _temp43 + 7u}, 280},{{ _temp44, _temp44, _temp44 + 7u}, 308},{{
_temp45, _temp45, _temp45 + 7u}, 260},{{ _temp46, _temp46, _temp46 + 7u}, 275},{{
_temp47, _temp47, _temp47 + 7u}, 284},{{ _temp48, _temp48, _temp48 + 6u}, 295},{{
_temp49, _temp49, _temp49 + 4u}, 296},{{ _temp50, _temp50, _temp50 + 7u}, 303},{{
_temp51, _temp51, _temp51 + 8u}, 262},{{ _temp52, _temp52, _temp52 + 6u}, 276},{{
_temp53, _temp53, _temp53 + 9u}, 271},{{ _temp54, _temp54, _temp54 + 6u}, 301},{{
_temp55, _temp55, _temp55 + 5u}, 263},{{ _temp56, _temp56, _temp56 + 9u}, 273},{{
_temp57, _temp57, _temp57 + 6u}, 285},{{ _temp58, _temp58, _temp58 + 8u}, 304}};
static int Cyc_Lex_trie_char( int c){ if( c >= 95){ return c - 59;} else{ if( c
> 64){ return c - 55;}} return c - 48;} static struct Cyc_Lex_Trie* Cyc_Lex_trie_lookup(
struct Cyc_Lex_Trie* t, struct _tagged_arr buff, int offset, int len){ int i=
offset; int last=( offset + len) - 1; while( i <= last) { void* _temp59=( void*)((
struct Cyc_Lex_Trie*) _check_null( t))->children; struct Cyc_Lex_Trie** _temp67;
struct Cyc_Lex_Trie* _temp69; int _temp71; _LL61: if(( unsigned int) _temp59 > 1u?*((
int*) _temp59) == Cyc_Lex_Many: 0){ _LL68: _temp67=(( struct Cyc_Lex_Many_struct*)
_temp59)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(( unsigned int) _temp59
> 1u?*(( int*) _temp59) == Cyc_Lex_One: 0){ _LL72: _temp71=(( struct Cyc_Lex_One_struct*)
_temp59)->f1; goto _LL70; _LL70: _temp69=(( struct Cyc_Lex_One_struct*) _temp59)->f2;
goto _LL64;} else{ goto _LL65;} _LL65: if( _temp59 ==( void*) Cyc_Lex_Zero){
goto _LL66;} else{ goto _LL60;} _LL62: { int ch= Cyc_Lex_trie_char(( int)*((
const unsigned char*) _check_unknown_subscript( buff, sizeof( unsigned char), i)));
if( _temp67[ _check_known_subscript_notnull( 64u, ch)] == 0){ _temp67[
_check_known_subscript_notnull( 64u, ch)]=({ struct Cyc_Lex_Trie* _temp73=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp73->children=(
void*)(( void*) Cyc_Lex_Zero); _temp73->shared_str= 0; _temp73;});} t= _temp67[
_check_known_subscript_notnull( 64u, ch)]; ++ i; goto _LL60;} _LL64: if( _temp71
==*(( const unsigned char*) _check_unknown_subscript( buff, sizeof(
unsigned char), i))){ t= _temp69;} else{ struct Cyc_Lex_Trie** _temp74=({
unsigned int _temp79= 64u; struct Cyc_Lex_Trie** _temp80=( struct Cyc_Lex_Trie**)
GC_malloc( sizeof( struct Cyc_Lex_Trie*) * _temp79);{ unsigned int _temp81=
_temp79; unsigned int j; for( j= 0; j < _temp81; j ++){ _temp80[ j]= 0;}};
_temp80;}); _temp74[ _check_known_subscript_notnull( 64u, Cyc_Lex_trie_char(
_temp71))]= _temp69;{ int _temp75= Cyc_Lex_trie_char(( int)*(( const
unsigned char*) _check_unknown_subscript( buff, sizeof( unsigned char), i)));
_temp74[ _check_known_subscript_notnull( 64u, _temp75)]=({ struct Cyc_Lex_Trie*
_temp76=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie));
_temp76->children=( void*)(( void*) Cyc_Lex_Zero); _temp76->shared_str= 0;
_temp76;});( void*)((( struct Cyc_Lex_Trie*) _check_null( t))->children=( void*)((
void*)({ struct Cyc_Lex_Many_struct* _temp77=( struct Cyc_Lex_Many_struct*)
GC_malloc( sizeof( struct Cyc_Lex_Many_struct)); _temp77[ 0]=({ struct Cyc_Lex_Many_struct
_temp78; _temp78.tag= Cyc_Lex_Many; _temp78.f1= _temp74; _temp78;}); _temp77;})));
t= _temp74[ _check_known_subscript_notnull( 64u, _temp75)];}} ++ i; goto _LL60;
_LL66: while( i <= last) { struct Cyc_Lex_Trie* _temp82=({ struct Cyc_Lex_Trie*
_temp85=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie));
_temp85->children=( void*)(( void*) Cyc_Lex_Zero); _temp85->shared_str= 0;
_temp85;});( void*)((( struct Cyc_Lex_Trie*) _check_null( t))->children=( void*)((
void*)({ struct Cyc_Lex_One_struct* _temp83=( struct Cyc_Lex_One_struct*)
GC_malloc( sizeof( struct Cyc_Lex_One_struct)); _temp83[ 0]=({ struct Cyc_Lex_One_struct
_temp84; _temp84.tag= Cyc_Lex_One; _temp84.f1=( int)*(( const unsigned char*)
_check_unknown_subscript( buff, sizeof( unsigned char), i ++)); _temp84.f2=
_temp82; _temp84;}); _temp83;}))); t= _temp82;} return t; _LL60:;} return t;}
static int Cyc_Lex_str_index( struct _tagged_arr buff, int offset, int len){
struct Cyc_Lex_Trie* _temp86= Cyc_Lex_trie_lookup( Cyc_Lex_ids_trie, buff,
offset, len); if((( struct Cyc_Lex_Trie*) _check_null( _temp86))->shared_str ==
0){ struct _tagged_arr _temp87= Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy(
_temp87, _tagged_arr_plus( buff, sizeof( unsigned char), offset), len);{ int ans=((
int(*)( struct Cyc_Xarray_Xarray*, struct _tagged_arr*)) Cyc_Xarray_add_ind)((
struct Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols),({ struct _tagged_arr*
_temp88=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp88[
0]=( struct _tagged_arr) _temp87; _temp88;}));(( struct Cyc_Lex_Trie*)
_check_null( _temp86))->shared_str= ans;}} return(( struct Cyc_Lex_Trie*)
_check_null( _temp86))->shared_str;} static int Cyc_Lex_str_index_lbuf( struct
Cyc_Lexing_lexbuf* lbuf){ return Cyc_Lex_str_index(( struct _tagged_arr) lbuf->lex_buffer,
lbuf->lex_start_pos, lbuf->lex_curr_pos - lbuf->lex_start_pos);} static void Cyc_Lex_insert_typedef(
struct _tagged_arr* sptr){ struct _tagged_arr _temp89=* sptr; struct Cyc_Lex_Trie*
_temp90= Cyc_Lex_trie_lookup( Cyc_Lex_typedefs_trie, _temp89, 0,( int)(
_get_arr_size( _temp89, sizeof( unsigned char)) - 1));(( struct Cyc_Lex_Trie*)
_check_null( _temp90))->shared_str= 1; return;} static struct _tagged_arr* Cyc_Lex_get_symbol(
int symbol_num){ return(( struct _tagged_arr*(*)( struct Cyc_Xarray_Xarray*, int))
Cyc_Xarray_get)(( struct Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols),
symbol_num);} static int Cyc_Lex_int_of_char( unsigned char c){ if('0' <= c? c
<='9': 0){ return c -'0';} else{ if('a' <= c? c <='f': 0){ return( 10 + c) -'a';}
else{ if('A' <= c? c <='F': 0){ return( 10 + c) -'A';} else{( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp91=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp91[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp92; _temp92.tag= Cyc_Core_InvalidArg; _temp92.f1=
_tag_arr("string to integer conversion", sizeof( unsigned char), 29u); _temp92;});
_temp91;}));}}}} static struct _tuple6* Cyc_Lex_intconst( struct Cyc_Lexing_lexbuf*
lbuf, int start, int end, int base){ unsigned int n= 0; int end2= lbuf->lex_curr_pos
- end; struct _tagged_arr buff= lbuf->lex_buffer; int i= start + lbuf->lex_start_pos;{
int i= start + lbuf->lex_start_pos; for( 0; i < end2; ++ i){ unsigned char c=*((
unsigned char*) _check_unknown_subscript( buff, sizeof( unsigned char), i));
switch( c){ case 'u': _LL93: goto _LL94; case 'U': _LL94: return({ struct
_tuple6* _temp96=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp96->f1=(
void*) Cyc_Absyn_Unsigned; _temp96->f2=( int) n; _temp96;}); case 'l': _LL95:
break; case 'L': _LL97: break; default: _LL98: n= n * base +( unsigned int) Cyc_Lex_int_of_char(
c); break;}}} return({ struct _tuple6* _temp100=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp100->f1=( void*) Cyc_Absyn_Signed; _temp100->f2=(
int) n; _temp100;});} unsigned char Cyc_Lex_string_buffer_v[ 10u]={'x','x','x','x','x','x','x','x','x','x'};
struct _tagged_arr Cyc_Lex_string_buffer={( void*) Cyc_Lex_string_buffer_v,(
void*) Cyc_Lex_string_buffer_v,( void*) Cyc_Lex_string_buffer_v + 10u}; int Cyc_Lex_string_pos=
0; void Cyc_Lex_store_string_char( unsigned char c){ int sz=( int) _get_arr_size(
Cyc_Lex_string_buffer, sizeof( unsigned char)); if( Cyc_Lex_string_pos >= sz){
int newsz= sz; while( Cyc_Lex_string_pos >= newsz) { newsz= newsz * 2;}{ struct
_tagged_arr str=({ unsigned int _temp101=( unsigned int) newsz; unsigned char*
_temp102=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp101);
struct _tagged_arr _temp104= _tag_arr( _temp102, sizeof( unsigned char),(
unsigned int) newsz);{ unsigned int _temp103= _temp101; unsigned int i; for( i=
0; i < _temp103; i ++){ _temp102[ i]= i < sz?*(( unsigned char*)
_check_unknown_subscript( Cyc_Lex_string_buffer, sizeof( unsigned char),( int) i)):'\000';}};
_temp104;}); Cyc_Lex_string_buffer= str;}}*(( unsigned char*)
_check_unknown_subscript( Cyc_Lex_string_buffer, sizeof( unsigned char), Cyc_Lex_string_pos))=
c; ++ Cyc_Lex_string_pos;} struct _tagged_arr Cyc_Lex_get_stored_string(){
struct _tagged_arr str= Cyc_String_substring(( struct _tagged_arr) Cyc_Lex_string_buffer,
0, Cyc_Lex_string_pos); Cyc_Lex_string_pos= 0; return str;} struct Cyc_Lex_Ldecls{
struct Cyc_Set_Set* typedefs; struct Cyc_Set_Set* namespaces; } ; struct Cyc_Lex_Lvis{
struct Cyc_List_List* current_namespace; struct Cyc_List_List*
imported_namespaces; } ; struct Cyc_Lex_Lstate{ struct Cyc_List_List* lstack;
struct Cyc_Dict_Dict* decls; } ; static struct Cyc_Core_Opt* Cyc_Lex_lstate= 0;
static void Cyc_Lex_typedef_init(){ struct Cyc_Lex_Lvis* _temp105=({ struct Cyc_Lex_Lvis*
_temp112=( struct Cyc_Lex_Lvis*) GC_malloc( sizeof( struct Cyc_Lex_Lvis));
_temp112->current_namespace= 0; _temp112->imported_namespaces= 0; _temp112;});
struct Cyc_List_List* _temp106=({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp111->hd=( void*) _temp105;
_temp111->tl= 0; _temp111;}); struct Cyc_Dict_Dict* _temp107=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), 0,({ struct
Cyc_Lex_Ldecls* _temp110=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp110->typedefs=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp110->namespaces=((
struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp110;})); Cyc_Lex_lstate=({ struct
Cyc_Core_Opt* _temp108=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp108->v=( void*)({ struct Cyc_Lex_Lstate* _temp109=( struct Cyc_Lex_Lstate*)
GC_malloc( sizeof( struct Cyc_Lex_Lstate)); _temp109->lstack= _temp106; _temp109->decls=
_temp107; _temp109;}); _temp108;});} static struct Cyc_List_List* Cyc_Lex_get_absolute_namespace(
struct Cyc_List_List* ns){ struct _tagged_arr* n=( struct _tagged_arr*) ns->hd;{
struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack; for( 0; ls != 0;
ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ struct Cyc_Lex_Lvis* lv=(
struct Cyc_Lex_Lvis*)(( struct Cyc_List_List*) _check_null( ls))->hd; struct Cyc_List_List*
x=({ struct Cyc_List_List* _temp113=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp113->hd=( void*) lv->current_namespace; _temp113->tl=
lv->imported_namespaces; _temp113;}); for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct
Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( x))->hd); if((( int(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->namespaces,
n)){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)(( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( x))->hd,( struct Cyc_List_List*) ns);}}}} Cyc_yyerror(( struct
_tagged_arr)({ struct _tagged_arr(* _temp114)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp118= _tag_arr("undeclared namespace %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp119;
_temp119.tag= Cyc_Stdio_String_pa; _temp119.f1=( struct _tagged_arr) Cyc_String_str_sepstr((
struct Cyc_List_List*) ns, _tag_arr("::", sizeof( unsigned char), 3u));{ void*
_temp117=( void*)& _temp119; void* _temp115[ 1u]={ _temp117}; struct _tagged_arr
_temp116={( void*) _temp115,( void*) _temp115,( void*)( _temp115 + 1u)};
_temp114( _temp118, _temp116);}})); return 0;} static void Cyc_Lex_recompute_typedefs(){
Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp120=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp120->children=( void*)(( void*)
Cyc_Lex_Zero); _temp120->shared_str= 0; _temp120;});{ struct Cyc_List_List* ls=(
struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack; for( 0; ls != 0; ls=(( struct Cyc_List_List*)
_check_null( ls))->tl){ struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*)(( struct
Cyc_List_List*) _check_null( ls))->hd; struct Cyc_List_List* x=({ struct Cyc_List_List*
_temp121=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp121->hd=( void*) lv->current_namespace; _temp121->tl= lv->imported_namespaces;
_temp121;}); for( 0; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( x))->hd);(( void(*)( void(* f)( struct
_tagged_arr*), struct Cyc_Set_Set* s)) Cyc_Set_iter)( Cyc_Lex_insert_typedef, ld->typedefs);}}}}
static int Cyc_Lex_is_typedef_in_namespace( struct Cyc_List_List* ns, struct
_tagged_arr* v){ struct Cyc_List_List* ans= Cyc_Lex_get_absolute_namespace( ns);
struct _handler_cons _temp122; _push_handler(& _temp122);{ int _temp124= 0; if(
setjmp( _temp122.handler)){ _temp124= 1;} if( ! _temp124){{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls, ans); int _temp125=(( int(*)( struct
Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->typedefs, v);
_npop_handler( 0u); return _temp125;}; _pop_handler();} else{ void* _temp123=(
void*) _exn_thrown; void* _temp127= _temp123; _LL129: if( _temp127 == Cyc_Dict_Absent){
goto _LL130;} else{ goto _LL131;} _LL131: goto _LL132; _LL130: return 0; _LL132:(
void) _throw( _temp127); _LL128:;}}} static int Cyc_Lex_is_typedef( struct Cyc_List_List*
ns, struct _tagged_arr* v){ if( ns != 0){ return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*) _check_null( ns), v);}{ struct _tagged_arr _temp133=* v;
int len=( int)( _get_arr_size( _temp133, sizeof( unsigned char)) - 1); struct
Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len; ++ i){ void*
_temp134=( void*)(( struct Cyc_Lex_Trie*) _check_null( t))->children; struct Cyc_Lex_Trie*
_temp145; int _temp147; struct Cyc_Lex_Trie* _temp149; struct Cyc_Lex_Trie**
_temp151; _LL136: if( _temp134 ==( void*) Cyc_Lex_Zero){ goto _LL137;} else{
goto _LL138;} _LL138: if(( unsigned int) _temp134 > 1u?*(( int*) _temp134) ==
Cyc_Lex_One: 0){ _LL148: _temp147=(( struct Cyc_Lex_One_struct*) _temp134)->f1;
goto _LL146; _LL146: _temp145=(( struct Cyc_Lex_One_struct*) _temp134)->f2; goto
_LL144;} else{ goto _LL140;} _LL144: if( _temp147 !=*(( const unsigned char*)
_check_unknown_subscript( _temp133, sizeof( unsigned char), i))){ goto _LL139;}
else{ goto _LL140;} _LL140: if(( unsigned int) _temp134 > 1u?*(( int*) _temp134)
== Cyc_Lex_One: 0){ _LL150: _temp149=(( struct Cyc_Lex_One_struct*) _temp134)->f2;
goto _LL141;} else{ goto _LL142;} _LL142: if(( unsigned int) _temp134 > 1u?*((
int*) _temp134) == Cyc_Lex_Many: 0){ _LL152: _temp151=(( struct Cyc_Lex_Many_struct*)
_temp134)->f1; goto _LL143;} else{ goto _LL135;} _LL137: return 0; _LL139:
return 0; _LL141: t= _temp149; goto _LL135; _LL143: { struct Cyc_Lex_Trie*
_temp153= _temp151[ _check_known_subscript_notnull( 64u, Cyc_Lex_trie_char(( int)*((
const unsigned char*) _check_unknown_subscript( _temp133, sizeof( unsigned char),
i))))]; if( _temp153 == 0){ return 0;} t= _temp153; goto _LL135;} _LL135:;}}
return(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str;}} void Cyc_Lex_enter_namespace(
struct _tagged_arr* s){ struct Cyc_List_List* ns=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List* new_ns=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp158=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp158->hd=( void*) s; _temp158->tl= 0; _temp158;}));(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack=({ struct Cyc_List_List*
_temp154=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp154->hd=( void*)({ struct Cyc_Lex_Lvis* _temp155=( struct Cyc_Lex_Lvis*)
GC_malloc( sizeof( struct Cyc_Lex_Lvis)); _temp155->current_namespace= new_ns;
_temp155->imported_namespaces= 0; _temp155;}); _temp154->tl=( struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack;
_temp154;});{ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls, ns); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->namespaces,
s)){(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls, ns,({ struct Cyc_Lex_Ldecls*
_temp156=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp156->typedefs= ld->typedefs; _temp156->namespaces=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( ld->namespaces,
s); _temp156;}));(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,
new_ns,({ struct Cyc_Lex_Ldecls* _temp157=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp157->typedefs=(( struct Cyc_Set_Set*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp157->namespaces=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp157;}));}((
void(*)( void(* f)( struct _tagged_arr*), struct Cyc_Set_Set* s)) Cyc_Set_iter)(
Cyc_Lex_insert_typedef,((( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls, new_ns))->typedefs);}}
void Cyc_Lex_leave_namespace(){(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack=( struct Cyc_List_List*) _check_null((((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();} void Cyc_Lex_enter_using( struct _tuple0* q){
struct Cyc_List_List* ns;{ void* _temp159=(* q).f1; struct Cyc_List_List*
_temp167; struct Cyc_List_List* _temp169; _LL161: if( _temp159 ==( void*) Cyc_Absyn_Loc_n){
goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int) _temp159 > 1u?*((
int*) _temp159) == Cyc_Absyn_Rel_n: 0){ _LL168: _temp167=(( struct Cyc_Absyn_Rel_n_struct*)
_temp159)->f1; goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int)
_temp159 > 1u?*(( int*) _temp159) == Cyc_Absyn_Abs_n: 0){ _LL170: _temp169=((
struct Cyc_Absyn_Abs_n_struct*) _temp159)->f1; goto _LL166;} else{ goto _LL160;}
_LL162: ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp171=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp171->hd=( void*)(*
q).f2; _temp171->tl= 0; _temp171;}); goto _LL160; _LL164: _temp169= _temp167;
goto _LL166; _LL166: ns=( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp169,({
struct Cyc_List_List* _temp172=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp172->hd=( void*)(* q).f2; _temp172->tl= 0; _temp172;})));
goto _LL160; _LL160:;}{ struct Cyc_List_List* _temp173= Cyc_Lex_get_absolute_namespace(
ns); struct Cyc_List_List* _temp174=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({ struct
Cyc_List_List* _temp175=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp175->hd=( void*) _temp173; _temp175->tl= _temp174; _temp175;});(( void(*)(
void(* f)( struct _tagged_arr*), struct Cyc_Set_Set* s)) Cyc_Set_iter)( Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->decls, _temp173))->typedefs);}} void Cyc_Lex_leave_using(){
struct Cyc_List_List* _temp176=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=(( struct
Cyc_List_List*) _check_null( _temp176))->tl; Cyc_Lex_recompute_typedefs();} void
Cyc_Lex_register_typedef( struct _tuple0* q){ struct Cyc_List_List* _temp177=((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace; struct Cyc_Dict_Dict*
_temp178=(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls;
struct Cyc_Lex_Ldecls* _temp179=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp178, _temp177); struct Cyc_Lex_Ldecls*
_temp180=({ struct Cyc_Lex_Ldecls* _temp181=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp181->typedefs=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( _temp179->typedefs,(*
q).f2); _temp181->namespaces= _temp179->namespaces; _temp181;});(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)( _temp178, _temp177, _temp180); Cyc_Lex_insert_typedef((* q).f2);}
static short Cyc_Lex_process_id( struct Cyc_Lexing_lexbuf* lbuf){ int symbol_num=
Cyc_Lex_str_index_lbuf( lbuf); if( symbol_num <= Cyc_Lex_num_kws){ return( short)*((
int*) _check_unknown_subscript( Cyc_Lex_kw_nums, sizeof( int), symbol_num - 1));}{
struct _tagged_arr* _temp182= Cyc_Lex_get_symbol( symbol_num); if( Cyc_Lex_is_typedef(
0, _temp182)){ Cyc_Lex_token_qvar=({ struct _tuple0* _temp183=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp183->f1= Cyc_Absyn_rel_ns_null;
_temp183->f2= _temp182; _temp183;}); return 351;} Cyc_Lex_token_string=*
_temp182; return 344;}} static short Cyc_Lex_process_qual_id( struct Cyc_Lexing_lexbuf*
lbuf){ int i= lbuf->lex_start_pos; int end= lbuf->lex_curr_pos; struct
_tagged_arr s= lbuf->lex_buffer; struct Cyc_List_List* rev_vs= 0; while( i < end) {
int start= i; for( 0; i < end?*(( unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)) !=':': 0; i ++){;} if( start == i){( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp184=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp184[ 0]=({ struct
Cyc_Core_Impossible_struct _temp185; _temp185.tag= Cyc_Core_Impossible; _temp185.f1=
_tag_arr("bad namespace", sizeof( unsigned char), 14u); _temp185;}); _temp184;}));}{
int vlen= i - start; struct _tagged_arr* v= Cyc_Lex_get_symbol( Cyc_Lex_str_index((
struct _tagged_arr) s, start, vlen)); rev_vs=({ struct Cyc_List_List* _temp186=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp186->hd=(
void*) v; _temp186->tl= rev_vs; _temp186;}); i += 2;}} if( rev_vs == 0){( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp187=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp187[ 0]=({ struct
Cyc_Core_Impossible_struct _temp188; _temp188.tag= Cyc_Core_Impossible; _temp188.f1=
_tag_arr("bad namespace", sizeof( unsigned char), 14u); _temp188;}); _temp187;}));}{
struct _tagged_arr* v=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( rev_vs))->hd; struct Cyc_List_List* vs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)((( struct Cyc_List_List*)
_check_null( rev_vs))->tl); Cyc_Lex_token_qvar=({ struct _tuple0* _temp189=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp189->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp190=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp190[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp191; _temp191.tag= Cyc_Absyn_Rel_n; _temp191.f1= vs; _temp191;}); _temp190;});
_temp189->f2= v; _temp189;}); if( Cyc_Lex_is_typedef( vs, v)){ return 351;}
else{ return 350;}}} extern int Cyc_Lex_token( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*); int Cyc_yylex(){ int ans=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Parse_lbuf))->v); Cyc_yylloc.first_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); Cyc_yylloc.last_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_end)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); switch( ans){ case 344: _LL192: Cyc_yylval=(
void*)({ struct Cyc_String_tok_struct* _temp194=( struct Cyc_String_tok_struct*)
GC_malloc( sizeof( struct Cyc_String_tok_struct)); _temp194[ 0]=({ struct Cyc_String_tok_struct
_temp195; _temp195.tag= Cyc_String_tok; _temp195.f1= Cyc_Lex_token_string;
_temp195;}); _temp194;}); break; case 350: _LL193: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp197=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp197[ 0]=({ struct Cyc_QualId_tok_struct
_temp198; _temp198.tag= Cyc_QualId_tok; _temp198.f1= Cyc_Lex_token_qvar;
_temp198;}); _temp197;}); break; case 351: _LL196: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp200=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp200[ 0]=({ struct Cyc_QualId_tok_struct
_temp201; _temp201.tag= Cyc_QualId_tok; _temp201.f1= Cyc_Lex_token_qvar;
_temp201;}); _temp200;}); break; case 349: _LL199: Cyc_yylval=( void*)({ struct
Cyc_String_tok_struct* _temp203=( struct Cyc_String_tok_struct*) GC_malloc(
sizeof( struct Cyc_String_tok_struct)); _temp203[ 0]=({ struct Cyc_String_tok_struct
_temp204; _temp204.tag= Cyc_String_tok; _temp204.f1= Cyc_Lex_token_string;
_temp204;}); _temp203;}); break; case 345: _LL202: Cyc_yylval=( void*)({ struct
Cyc_Int_tok_struct* _temp206=( struct Cyc_Int_tok_struct*) GC_malloc( sizeof(
struct Cyc_Int_tok_struct)); _temp206[ 0]=({ struct Cyc_Int_tok_struct _temp207;
_temp207.tag= Cyc_Int_tok; _temp207.f1= Cyc_Lex_token_int; _temp207;}); _temp206;});
break; case 347: _LL205: Cyc_yylval=( void*)({ struct Cyc_Char_tok_struct*
_temp209=( struct Cyc_Char_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Char_tok_struct));
_temp209[ 0]=({ struct Cyc_Char_tok_struct _temp210; _temp210.tag= Cyc_Char_tok;
_temp210.f1= Cyc_Lex_token_char; _temp210;}); _temp209;}); break; case 348:
_LL208: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp212=( struct
Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp212[ 0]=({ struct Cyc_String_tok_struct _temp213; _temp213.tag= Cyc_String_tok;
_temp213.f1= Cyc_Lex_token_string; _temp213;}); _temp212;}); break; case 346:
_LL211: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp215=( struct
Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp215[ 0]=({ struct Cyc_String_tok_struct _temp216; _temp216.tag= Cyc_String_tok;
_temp216.f1= Cyc_Lex_token_string; _temp216;}); _temp215;}); break; default:
_LL214: break;} return ans;} const int Cyc_Lex_lex_base[ 149u]=( const int[ 149u]){
0, 113, 17, 83, 16, 2, - 3, - 1, - 2, - 17, - 18, 4, 118, 119, - 19, 5, - 13, -
12, 85, - 14, - 11, - 4, - 5, - 6, - 7, - 8, - 9, - 10, 192, 215, 111, - 15, 166,
- 18, - 55, 8, 30, - 39, 18, 31, 116, 139, 32, 138, 135, 137, 159, 250, 325, 83,
82, 84, 94, 395, 85, 470, 545, 90, - 54, - 22, - 28, 620, 695, 90, 753, 828, 99,
- 23, 903, 123, - 26, 134, - 31, - 25, - 34, 978, 1007, 363, 117, 128, 292, 1017,
1047, 1083, 335, 1108, 1120, 1190, 133, 143, 1260, 1298, 137, 147, 191, 201, 193,
203, - 6, - 38, 199, - 20, - 37, 6, 157, 1230, - 33, - 16, - 32, - 17, 2, 1338,
168, 169, 279, 171, 174, 176, 185, 186, 187, 188, 189, 191, 1411, 1495, - 52, -
46, - 45, - 44, - 43, - 42, - 41, - 40, - 47, - 50, - 51, 311, 193, - 48, - 49,
- 53, - 27, - 24, - 21, 310, - 35, 12, 425}; const int Cyc_Lex_lex_backtrk[ 149u]=(
const int[ 149u]){ - 1, - 1, - 1, 5, 3, 4, - 1, - 1, - 1, - 1, - 1, 16, 1, 19, -
1, 2, - 1, - 1, 14, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 15, 14, -
1, - 1, - 1, - 1, 35, 54, - 1, 54, 54, 54, 54, 54, 54, 54, 54, 54, 5, 7, 54, 54,
54, 54, 0, 54, 54, 54, 54, - 1, - 1, - 1, 3, 1, - 1, - 1, 2, - 1, - 1, 0, 29, -
1, 28, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9, 5, 6, 5, 5, - 1,
4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, - 1, - 1, 36, - 1, 9, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 34, 35}; const int Cyc_Lex_lex_default[ 149u]=( const int[
149u]){ 34, 9, 3, 3, - 1, - 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1, - 1, 0, 145, - 1, -
1, 110, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, 0, - 1, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 100, 0, 0, - 1, - 1, - 1, 0, 0, 0, 0, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0, 145, 0, - 1, - 1}; const int
Cyc_Lex_lex_trans[ 1752u]=( const int[ 1752u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 35,
35, 35, 35, 35, 33, 6, 102, 148, 148, 148, 148, 148, 146, 0, 0, 0, 0, 0, 146, 0,
0, 147, 35, 36, 37, 38, 0, 39, 40, 41, 148, 141, 42, 43, 7, 44, 45, 46, 47, 48,
48, 48, 48, 48, 48, 48, 48, 48, 49, 4, 50, 51, 52, 8, 5, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
17, 144, 14, 54, 55, 56, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 10, 57, - 1, 11, 32, 32, 6, - 1,
32, 15, 30, 30, 30, 30, 30, 30, 30, 30, 74, 71, 16, 72, 20, 67, 12, 64, - 1, 32,
59, 7, 16, 142, 19, 69, 64, 17, 31, 31, 31, 31, 31, 31, 31, 31, 18, 18, 18, 18,
18, 18, 18, 18, 32, 32, 143, - 1, 32, 107, 31, 19, 104, 70, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 73, 33, 108, 32, 109, 7, 99, 25, 106, 25, 13, 100,
140, 139, 102, 135, 20, 103, 134, 60, 133, 21, 22, 23, 23, 101, 23, 22, 22, 132,
131, 130, 129, 128, 24, 127, 111, 136, 25, 25, 26, 25, 27, 0, 28, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 23, 23, 0, 0, 22, 22, 58, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 6, - 1, 0, 24, 24, 98, 98, 29, 29, 29, 29, 29,
29, 0, 0, 0, 29, 29, 29, 29, 29, 29, 0, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86,
87, 87, 24, 24, 98, 98, 29, 29, 29, 29, 29, 29, 136, 77, 146, 0, 0, 147, 0, 0,
88, 137, 137, 137, 137, 137, 137, 137, 137, 89, 0, 0, 90, - 1, 81, 81, 81, 81,
81, 81, 81, 81, 81, 81, 136, 77, 0, 0, 0, 0, 0, 0, 88, 138, 138, 138, 138, 138,
138, 138, 138, 89, 0, 14, 90, 75, 0, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 0, 77, - 1, 0, 0, 0, 0, 0, 78, 0, 0, 0, 0,
80, 0, 80, 0, 79, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 0, 0, 0, 0, 77, 0,
0, 0, 0, 0, 0, 78, 148, 148, 148, 148, 148, 0, 0, 0, 79, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 63, 0, - 1, 0, 148, 0, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 0, 0, 0, 0, 68,
0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 63, 0, 0, 0,
0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 62, - 1, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 61, 0, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 0, 0, 0, 0, 62, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 66, 0, 0, 0, 0, 0, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 0, 0, 0, 0, 65, 0, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 82, 82, 82, 82,
82, 82, 82, 82, 82, 82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 27, 0, 0, 0, 0, 75,
27, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81, 81, 81, 81, 81, 81, 81, 81,
81, 0, 77, 0, 0, 83, 27, 0, 0, 78, 0, 0, 27, 26, 0, 0, 0, 0, 79, 26, 0, 82, 82,
82, 82, 82, 82, 82, 82, 82, 82, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 78, 83, 27, 0, 26,
0, 0, 0, 27, 79, 26, 84, 0, 84, 0, 0, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 0,
0, 0, 0, 0, 0, 0, 83, 27, 0, 0, 0, 0, 0, 27, 85, 85, 85, 85, 85, 85, 85, 85, 85,
85, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 27, 0, 0, 0, 0, 0, 27, 0, 0,
0, 0, 77, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 97, 0, 0, 0, 0, 27, 0, 0,
0, 0, 0, 27, 0, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 75, 97,
87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 77, 0,
0, 0, 0, 0, 0, 94, 0, 0, 0, 0, 0, 0, 0, 0, 95, 0, 0, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 94, 83, 27, 0, 0, 0, 0,
0, 27, 95, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 0, 0, 0, 0, 91, 91,
91, 91, 91, 91, 83, 27, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91,
91, 91, 91, 91, 91, 91, 0, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0,
0, 112, 0, 92, 0, 0, 113, 0, 0, 0, 0, 0, 93, 0, 0, 114, 114, 114, 114, 114, 114,
114, 114, 0, 91, 91, 91, 91, 91, 91, 115, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0,
93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 116, 0, 0, 0, 0, 117, 118, 0, 0, 0,
119, 0, 0, 0, 0, 0, 0, 0, 120, 0, 0, 0, 121, 0, 122, 0, 123, 0, 124, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 126, 0, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int Cyc_Lex_lex_check[ 1752u]=( const int[ 1752u]){ - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 0, 0, 0, 0, 0, 11, 15, 103, 35, 35, 35, 35, 35, 147, - 1,
- 1, - 1, - 1, - 1, 38, - 1, - 1, 38, 0, 0, 0, 0, - 1, 0, 0, 0, 35, 110, 0, 0, 5,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 42, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
3, 1, 12, 12, 13, 3, 12, 13, 18, 18, 18, 18, 18, 18, 18, 18, 49, 50, 50, 50, 51,
54, 1, 63, 41, 12, 57, 12, 13, 40, 52, 52, 66, 13, 30, 30, 30, 30, 30, 30, 30,
30, 13, 13, 13, 13, 13, 13, 13, 13, 32, 32, 40, 41, 32, 44, 43, 13, 45, 69, 45,
45, 45, 45, 45, 45, 45, 45, 45, 45, 71, 44, 44, 32, 43, 32, 46, 78, 104, 79, 1,
46, 112, 113, 100, 115, 13, 100, 116, 57, 117, 13, 13, 88, 89, 46, 13, 92, 93,
118, 119, 120, 121, 122, 13, 123, 41, 138, 13, 78, 13, 79, 13, - 1, 13, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 88, 89, - 1, - 1, 92, 93, 0, 28, 28, 28, 28, 28,
28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 2, 38, - 1, 94, 95, 96, 97, 29, 29,
29, 29, 29, 29, - 1, - 1, - 1, 28, 28, 28, 28, 28, 28, - 1, 47, - 1, 47, 47, 47,
47, 47, 47, 47, 47, 47, 47, 94, 95, 96, 97, 29, 29, 29, 29, 29, 29, 114, 47, 145,
- 1, - 1, 145, - 1, - 1, 47, 114, 114, 114, 114, 114, 114, 114, 114, 47, - 1, -
1, 47, 3, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 137, 47, - 1, - 1, - 1, - 1, -
1, - 1, 47, 137, 137, 137, 137, 137, 137, 137, 137, 47, - 1, 1, 47, 48, - 1, 48,
48, 48, 48, 48, 48, 48, 48, 48, 48, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, - 1,
48, 41, - 1, - 1, - 1, - 1, - 1, 48, - 1, - 1, - 1, - 1, 77, - 1, 77, - 1, 48,
77, 77, 77, 77, 77, 77, 77, 77, 77, 77, - 1, - 1, - 1, - 1, - 1, 48, - 1, - 1, -
1, - 1, - 1, - 1, 48, 148, 148, 148, 148, 148, - 1, - 1, - 1, 48, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, - 1, 100, - 1, 148, - 1, - 1, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
- 1, - 1, - 1, - 1, 53, - 1, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, - 1, - 1, - 1, - 1, - 1, - 1, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, - 1, - 1, -
1, - 1, 55, 145, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, - 1, - 1, - 1, - 1,
56, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1, - 1, - 1, 61, - 1, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, -
1, - 1, - 1, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, - 1, 62, - 1, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, - 1, - 1, - 1, - 1, 64, - 1, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, - 1, - 1, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, 65, - 1, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, - 1, - 1, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, - 1, - 1, - 1, - 1, 68, - 1, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 75, 75, 75, 75, 75,
75, 75, 75, 75, 75, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 75,
75, - 1, - 1, - 1, - 1, 76, 75, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81,
81, 81, 81, 81, 81, 81, 81, 81, - 1, 76, - 1, - 1, 75, 75, - 1, - 1, 76, - 1, -
1, 75, 81, - 1, - 1, - 1, - 1, 76, 81, - 1, 82, 82, 82, 82, 82, 82, 82, 82, 82,
82, - 1, - 1, - 1, 76, - 1, - 1, - 1, - 1, - 1, - 1, 76, 82, 82, - 1, 81, - 1, -
1, - 1, 82, 76, 81, 83, - 1, 83, - 1, - 1, 83, 83, 83, 83, 83, 83, 83, 83, 83,
83, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 82, 82, - 1, - 1, - 1, - 1, - 1, 82, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 86, - 1, 86, 86, 86, 86, 86, 86, 86, 86, 86,
86, 85, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1,
- 1, - 1, 86, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1, 85,
- 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1, - 1, -
1, 86, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 87, 86, 87, 87, 87, 87, 87, 87, 87, 87,
87, 87, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, - 1,
- 1, - 1, - 1, 87, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, - 1, - 1, - 1, 87, - 1, - 1, - 1, -
1, - 1, - 1, 87, 105, 105, - 1, - 1, - 1, - 1, - 1, 105, 87, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 90, 90, 90, 90, 90, 90,
105, 105, - 1, - 1, - 1, - 1, - 1, 105, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91,
91, 91, 91, 91, 91, 91, 91, 91, 91, - 1, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91,
91, 91, - 1, - 1, - 1, 111, - 1, 91, - 1, - 1, 111, - 1, - 1, - 1, - 1, - 1, 91,
- 1, - 1, 111, 111, 111, 111, 111, 111, 111, 111, - 1, 91, 91, 91, 91, 91, 91,
111, - 1, - 1, - 1, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 111, - 1, - 1, -
1, - 1, 111, 111, - 1, - 1, - 1, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 111, -
1, - 1, - 1, 111, - 1, 111, - 1, 111, - 1, 111, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, - 1, - 1, - 1, - 1, - 1, - 1, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 125, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1, - 1, - 1, - 1, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int
Cyc_Lex_lex_engine( int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int state;
int base; int backtrk; int c; state= start_state; if( state >= 0){ lbuf->lex_last_pos=(
lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{
state=( - state) - 1;} while( 1) { base= Cyc_Lex_lex_base[
_check_known_subscript_notnull( 149u, state)]; if( base < 0){ return( - base) -
1;} backtrk= Cyc_Lex_lex_backtrk[ _check_known_subscript_notnull( 149u, state)];
if( backtrk >= 0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >= lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) - 1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c == - 1){ c= 256;}} if( Cyc_Lex_lex_check[
_check_known_subscript_notnull( 1752u, base + c)] == state){ state= Cyc_Lex_lex_trans[
_check_known_subscript_notnull( 1752u, base + c)];} else{ state= Cyc_Lex_lex_default[
_check_known_subscript_notnull( 149u, state)];} if( state < 0){ lbuf->lex_curr_pos=
lbuf->lex_last_pos; if( lbuf->lex_last_action == - 1){( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp218=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp218[ 0]=({ struct Cyc_Lexing_Error_struct
_temp219; _temp219.tag= Cyc_Lexing_Error; _temp219.f1= _tag_arr("empty token",
sizeof( unsigned char), 12u); _temp219;}); _temp218;}));} else{ return lbuf->lex_last_action;}}
else{ if( c == 256){ lbuf->lex_eof_reached= 0;}}}} int Cyc_Lex_token_rec( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_Lex_lex_engine( lexstate,
lexbuf); switch( lexstate){ case 0: _LL220: return( int) Cyc_Lex_process_id(
lexbuf); case 1: _LL221: return( int) Cyc_Lex_process_id( lexbuf); case 2:
_LL222: return( int) Cyc_Lex_process_qual_id( lexbuf); case 3: _LL223: Cyc_Lex_token_string=*
Cyc_Lex_get_symbol( Cyc_Lex_str_index_lbuf( lexbuf)); return 349; case 4: _LL224:
Cyc_Lex_token_int= Cyc_Lex_intconst( lexbuf, 2, 0, 16); return 345; case 5:
_LL225: Cyc_Lex_token_int= Cyc_Lex_intconst( lexbuf, 0, 0, 8); return 345; case
6: _LL226: Cyc_Lex_token_int= Cyc_Lex_intconst( lexbuf, 0, 0, 10); return 345;
case 7: _LL227: Cyc_Lex_token_int= Cyc_Lex_intconst( lexbuf, 0, 0, 10); return
345; case 8: _LL228: Cyc_Lex_token_string=( struct _tagged_arr) Cyc_Lexing_lexeme(
lexbuf); return 348; case 9: _LL229: Cyc_Lex_token_string=( struct _tagged_arr)
Cyc_Lexing_lexeme( lexbuf); return 348; case 10: _LL230: return 327; case 11:
_LL231: return 328; case 12: _LL232: return 325; case 13: _LL233: return 326;
case 14: _LL234: return 321; case 15: _LL235: return 322; case 16: _LL236:
return 334; case 17: _LL237: return 335; case 18: _LL238: return 331; case 19:
_LL239: return 332; case 20: _LL240: return 333; case 21: _LL241: return 340;
case 22: _LL242: return 339; case 23: _LL243: return 338; case 24: _LL244:
return 336; case 25: _LL245: return 337; case 26: _LL246: return 329; case 27:
_LL247: return 330; case 28: _LL248: return 323; case 29: _LL249: return 324;
case 30: _LL250: return 342; case 31: _LL251: return 320; case 32: _LL252:
return 341; case 33: _LL253: return 343; case 34: _LL254: return Cyc_Lex_token(
lexbuf); case 35: _LL255: return Cyc_Lex_token( lexbuf); case 36: _LL256: return
Cyc_Lex_token( lexbuf); case 37: _LL257: Cyc_Lex_comment_depth= 1; Cyc_Lex_runaway_start=
Cyc_Lexing_lexeme_start( lexbuf); Cyc_Lex_comment( lexbuf); return Cyc_Lex_token(
lexbuf); case 38: _LL258: Cyc_Lex_string_pos= 0; Cyc_Lex_runaway_start= Cyc_Lexing_lexeme_start(
lexbuf); Cyc_Lex_strng( lexbuf); Cyc_Lex_token_string=( struct _tagged_arr) Cyc_Lex_get_stored_string();
return 346; case 39: _LL259: Cyc_Lex_token_char='\a'; return 347; case 40:
_LL260: Cyc_Lex_token_char='\b'; return 347; case 41: _LL261: Cyc_Lex_token_char='\f';
return 347; case 42: _LL262: Cyc_Lex_token_char='\n'; return 347; case 43:
_LL263: Cyc_Lex_token_char='\r'; return 347; case 44: _LL264: Cyc_Lex_token_char='\t';
return 347; case 45: _LL265: Cyc_Lex_token_char='\v'; return 347; case 46:
_LL266: Cyc_Lex_token_char='\\'; return 347; case 47: _LL267: Cyc_Lex_token_char='\'';
return 347; case 48: _LL268: Cyc_Lex_token_char='"'; return 347; case 49: _LL269:
Cyc_Lex_token_char='?'; return 347; case 50: _LL270: Cyc_Lex_token_char=(
unsigned char)(* Cyc_Lex_intconst( lexbuf, 2, 1, 8)).f2; return 347; case 51:
_LL271: Cyc_Lex_token_char=( unsigned char)(* Cyc_Lex_intconst( lexbuf, 3, 1, 16)).f2;
return 347; case 52: _LL272: Cyc_Lex_token_char= Cyc_Lexing_lexeme_char( lexbuf,
1); return 347; case 53: _LL273: return - 1; case 54: _LL274: return( int) Cyc_Lexing_lexeme_char(
lexbuf, 0); default: _LL275:( lexbuf->refill_buff)( lexbuf); return Cyc_Lex_token_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp277=( struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp277[ 0]=({ struct Cyc_Lexing_Error_struct _temp278; _temp278.tag= Cyc_Lexing_Error;
_temp278.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp278;}); _temp277;}));} int Cyc_Lex_token( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_Lex_token_rec( lexbuf, 0);} int Cyc_Lex_strng_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_Lex_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL279: return Cyc_Lex_strng( lexbuf); case 1: _LL280: return
0; case 2: _LL281: return Cyc_Lex_strng( lexbuf); case 3: _LL282: Cyc_Lex_store_string_char('\a');
return Cyc_Lex_strng( lexbuf); case 4: _LL283: Cyc_Lex_store_string_char('\b');
return Cyc_Lex_strng( lexbuf); case 5: _LL284: Cyc_Lex_store_string_char('\f');
return Cyc_Lex_strng( lexbuf); case 6: _LL285: Cyc_Lex_store_string_char('\n');
return Cyc_Lex_strng( lexbuf); case 7: _LL286: Cyc_Lex_store_string_char('\r');
return Cyc_Lex_strng( lexbuf); case 8: _LL287: Cyc_Lex_store_string_char('\t');
return Cyc_Lex_strng( lexbuf); case 9: _LL288: Cyc_Lex_store_string_char('\v');
return Cyc_Lex_strng( lexbuf); case 10: _LL289: Cyc_Lex_store_string_char('\\');
return Cyc_Lex_strng( lexbuf); case 11: _LL290: Cyc_Lex_store_string_char('\'');
return Cyc_Lex_strng( lexbuf); case 12: _LL291: Cyc_Lex_store_string_char('"');
return Cyc_Lex_strng( lexbuf); case 13: _LL292: Cyc_Lex_store_string_char('?');
return Cyc_Lex_strng( lexbuf); case 14: _LL293: Cyc_Lex_store_string_char((
unsigned char)(* Cyc_Lex_intconst( lexbuf, 1, 0, 8)).f2); return Cyc_Lex_strng(
lexbuf); case 15: _LL294: Cyc_Lex_store_string_char(( unsigned char)(* Cyc_Lex_intconst(
lexbuf, 2, 0, 16)).f2); return Cyc_Lex_strng( lexbuf); case 16: _LL295: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char( lexbuf, 0)); return Cyc_Lex_strng( lexbuf); case 17:
_LL296: Cyc_Lex_runaway_err( _tag_arr("string ends in newline", sizeof(
unsigned char), 23u), lexbuf); return 0; case 18: _LL297: Cyc_Lex_runaway_err(
_tag_arr("unterminated string", sizeof( unsigned char), 20u), lexbuf); return 0;
case 19: _LL298: Cyc_Lex_err( _tag_arr("bad character following backslash in string",
sizeof( unsigned char), 44u), lexbuf); return Cyc_Lex_strng( lexbuf); default:
_LL299:( lexbuf->refill_buff)( lexbuf); return Cyc_Lex_strng_rec( lexbuf,
lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp301=(
struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp301[ 0]=({ struct Cyc_Lexing_Error_struct _temp302; _temp302.tag= Cyc_Lexing_Error;
_temp302.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp302;}); _temp301;}));} int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_Lex_strng_rec( lexbuf, 1);} int Cyc_Lex_comment_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_Lex_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL303: ++ Cyc_Lex_comment_depth; return Cyc_Lex_comment(
lexbuf); case 1: _LL304: -- Cyc_Lex_comment_depth; if( Cyc_Lex_comment_depth > 0){
return Cyc_Lex_comment( lexbuf);} return 0; case 2: _LL305: Cyc_Lex_runaway_err(
_tag_arr("unterminated comment", sizeof( unsigned char), 21u), lexbuf); return 0;
case 3: _LL306: return Cyc_Lex_comment( lexbuf); case 4: _LL307: return Cyc_Lex_comment(
lexbuf); case 5: _LL308: return Cyc_Lex_comment( lexbuf); default: _LL309:(
lexbuf->refill_buff)( lexbuf); return Cyc_Lex_comment_rec( lexbuf, lexstate);}(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp311=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp311[ 0]=({ struct Cyc_Lexing_Error_struct
_temp312; _temp312.tag= Cyc_Lexing_Error; _temp312.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp312;}); _temp311;}));} int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_Lex_comment_rec( lexbuf, 2);} void
Cyc_Lex_lex_init(){ Cyc_Lex_ids_trie=({ struct Cyc_Lex_Trie* _temp313=( struct
Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp313->children=(
void*)(( void*) Cyc_Lex_Zero); _temp313->shared_str= 0; _temp313;}); Cyc_Lex_typedefs_trie=({
struct Cyc_Lex_Trie* _temp314=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct
Cyc_Lex_Trie)); _temp314->children=( void*)(( void*) Cyc_Lex_Zero); _temp314->shared_str=
0; _temp314;}); Cyc_Lex_symbols=( struct Cyc_Xarray_Xarray*)(( struct Cyc_Xarray_Xarray*(*)(
int, struct _tagged_arr*)) Cyc_Xarray_create)( 101, _init_tag_arr(( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char),
1u));(( void(*)( struct Cyc_Xarray_Xarray*, struct _tagged_arr*)) Cyc_Xarray_add)((
struct Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols),& Cyc_Lex_bogus_string);
Cyc_Lex_num_kws=( int) 57u; Cyc_Lex_kw_nums=({ unsigned int _temp315=(
unsigned int) Cyc_Lex_num_kws; int* _temp316=( int*) GC_malloc_atomic( sizeof(
int) * _temp315); struct _tagged_arr _temp318= _tag_arr( _temp316, sizeof( int),(
unsigned int) Cyc_Lex_num_kws);{ unsigned int _temp317= _temp315; unsigned int i;
for( i= 0; i < _temp317; i ++){ _temp316[ i]= 0;}}; _temp318;});{ int i= 0; for(
0; i < Cyc_Lex_num_kws; ++ i){ struct _tagged_arr _temp319=( Cyc_Lex_rw_array[
_check_known_subscript_notnull( 57u, i)]).f1; Cyc_Lex_str_index( _temp319, 0,
Cyc_String_strlen( _temp319));*(( int*) _check_unknown_subscript( Cyc_Lex_kw_nums,
sizeof( int), i))=( int)( Cyc_Lex_rw_array[ _check_known_subscript_notnull( 57u,
i)]).f2;}} Cyc_Lex_typedef_init(); Cyc_Lex_comment_depth= 0;}