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
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u];
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*); extern
unsigned char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*, int); extern
int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern void Cyc_Set_iter( void(* f)( void*), struct Cyc_Set_Set*
s); extern unsigned char Cyc_Set_Absent[ 11u]; extern int Cyc_String_strlen(
struct _tagged_arr s); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_String_str_sepstr( struct
Cyc_List_List*, struct _tagged_arr); extern struct _tagged_arr Cyc_String_zstrncpy(
struct _tagged_arr, struct _tagged_arr, int); extern struct _tagged_arr Cyc_String_substring(
struct _tagged_arr, int ofs, int n); struct Cyc_Xarray_Xarray{ struct
_tagged_arr elmts; int num_elmts; } ; extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*,
int); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int, void*); extern
int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*, void*); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*, struct _tagged_arr); extern struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*, struct _tagged_arr);
extern unsigned char Cyc_Position_Nocontext[ 14u]; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); struct _tuple0{ void* f1; struct _tagged_arr* f2; }
; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
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
15u]; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Parse_lbuf; struct Cyc_Declaration_spec; struct
Cyc_Declarator; struct Cyc_Abstractdeclarator; extern unsigned char Cyc_AbstractDeclarator_tok[
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
struct _tagged_arr text; } ; extern struct Cyc_Yyltype Cyc_yylloc; extern void*
Cyc_yylval; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u];
extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); void Cyc_yyerror( struct _tagged_arr s){
Cyc_Position_post_error( Cyc_Position_mk_err_parse( Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line, Cyc_yylloc.last_line), s));} struct Cyc_Lex_Trie; struct
Cyc_Lex_Trie{ struct Cyc_Lex_Trie*** children; int shared_str; } ; static int
Cyc_Lex_num_kws= 0; static struct _tagged_arr Cyc_Lex_kw_nums={( void*) 0u,(
void*) 0u,( void*) 0u + 0u}; static struct Cyc_Xarray_Xarray* Cyc_Lex_symbols= 0;
static struct Cyc_Lex_Trie* Cyc_Lex_ids_trie= 0; static struct Cyc_Lex_Trie* Cyc_Lex_typedefs_trie=
0; static int Cyc_Lex_comment_depth= 0; static struct _tuple6 Cyc_Lex_token_int_pair={(
void*) 0u, 0}; static unsigned char _temp0[ 8u]="*bogus*"; static struct
_tagged_arr Cyc_Lex_bogus_string={ _temp0, _temp0, _temp0 + 8u}; static struct
Cyc_Absyn_Abs_n_struct Cyc_Lex_absn_null={ 1u, 0}; static struct _tuple0 Cyc_Lex_token_id_pair={(
void*)& Cyc_Lex_absn_null,& Cyc_Lex_bogus_string}; static unsigned char Cyc_Lex_token_char='\000';
static unsigned char _temp2[ 1u]=""; static struct _tagged_arr Cyc_Lex_token_string={
_temp2, _temp2, _temp2 + 1u}; static struct _tuple6* Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;
static struct _tuple0* Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair; static int
Cyc_Lex_runaway_start= 0; static void Cyc_Lex_err( struct _tagged_arr msg,
struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s= Cyc_Position_segment_of_abs(((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lb),(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lb)); Cyc_Position_post_error(
Cyc_Position_mk_err_lex( s, msg));} static void Cyc_Lex_runaway_err( struct
_tagged_arr msg, struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s=
Cyc_Position_segment_of_abs( Cyc_Lex_runaway_start,(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)( lb)); Cyc_Position_post_error( Cyc_Position_mk_err_lex(
s, msg));} struct _tuple9{ struct _tagged_arr f1; short f2; } ; static
unsigned char _temp3[ 14u]="__attribute__"; static unsigned char _temp4[ 9u]="abstract";
static unsigned char _temp5[ 5u]="auto"; static unsigned char _temp6[ 6u]="break";
static unsigned char _temp7[ 5u]="case"; static unsigned char _temp8[ 6u]="catch";
static unsigned char _temp9[ 5u]="char"; static unsigned char _temp10[ 8u]="codegen";
static unsigned char _temp11[ 6u]="const"; static unsigned char _temp12[ 9u]="continue";
static unsigned char _temp13[ 4u]="cut"; static unsigned char _temp14[ 8u]="default";
static unsigned char _temp15[ 3u]="do"; static unsigned char _temp16[ 7u]="double";
static unsigned char _temp17[ 5u]="else"; static unsigned char _temp18[ 5u]="enum";
static unsigned char _temp19[ 7u]="extern"; static unsigned char _temp20[ 9u]="fallthru";
static unsigned char _temp21[ 5u]="fill"; static unsigned char _temp22[ 6u]="float";
static unsigned char _temp23[ 4u]="for"; static unsigned char _temp24[ 8u]="fprintf";
static unsigned char _temp25[ 7u]="fscanf"; static unsigned char _temp26[ 5u]="goto";
static unsigned char _temp27[ 3u]="if"; static unsigned char _temp28[ 7u]="inline";
static unsigned char _temp29[ 4u]="int"; static unsigned char _temp30[ 4u]="let";
static unsigned char _temp31[ 5u]="long"; static unsigned char _temp32[ 7u]="malloc";
static unsigned char _temp33[ 10u]="namespace"; static unsigned char _temp34[ 4u]="new";
static unsigned char _temp35[ 5u]="null"; static unsigned char _temp36[ 9u]="offsetof";
static unsigned char _temp37[ 7u]="printf"; static unsigned char _temp38[ 9u]="region_t";
static unsigned char _temp39[ 7u]="region"; static unsigned char _temp40[ 9u]="register";
static unsigned char _temp41[ 9u]="restrict"; static unsigned char _temp42[ 7u]="return";
static unsigned char _temp43[ 8u]="rmalloc"; static unsigned char _temp44[ 5u]="rnew";
static unsigned char _temp45[ 6u]="scanf"; static unsigned char _temp46[ 6u]="short";
static unsigned char _temp47[ 7u]="signed"; static unsigned char _temp48[ 7u]="sizeof";
static unsigned char _temp49[ 7u]="splice"; static unsigned char _temp50[ 7u]="sscanf";
static unsigned char _temp51[ 7u]="static"; static unsigned char _temp52[ 7u]="struct";
static unsigned char _temp53[ 7u]="switch"; static unsigned char _temp54[ 6u]="throw";
static unsigned char _temp55[ 4u]="try"; static unsigned char _temp56[ 7u]="tunion";
static unsigned char _temp57[ 8u]="typedef"; static unsigned char _temp58[ 6u]="union";
static unsigned char _temp59[ 9u]="unsigned"; static unsigned char _temp60[ 6u]="using";
static unsigned char _temp61[ 5u]="void"; static unsigned char _temp62[ 9u]="volatile";
static unsigned char _temp63[ 6u]="while"; static unsigned char _temp64[ 8u]="xtunion";
static unsigned char _temp65[ 8u]="xprintf"; static struct _tuple9 Cyc_Lex_rw_array[
63u]={{{ _temp3, _temp3, _temp3 + 14u}, 352},{{ _temp4, _temp4, _temp4 + 9u},
299},{{ _temp5, _temp5, _temp5 + 5u}, 258},{{ _temp6, _temp6, _temp6 + 6u}, 290},{{
_temp7, _temp7, _temp7 + 5u}, 277},{{ _temp8, _temp8, _temp8 + 6u}, 297},{{
_temp9, _temp9, _temp9 + 5u}, 264},{{ _temp10, _temp10, _temp10 + 8u}, 306},{{
_temp11, _temp11, _temp11 + 6u}, 272},{{ _temp12, _temp12, _temp12 + 9u}, 289},{{
_temp13, _temp13, _temp13 + 4u}, 307},{{ _temp14, _temp14, _temp14 + 8u}, 278},{{
_temp15, _temp15, _temp15 + 3u}, 286},{{ _temp16, _temp16, _temp16 + 7u}, 269},{{
_temp17, _temp17, _temp17 + 5u}, 283},{{ _temp18, _temp18, _temp18 + 5u}, 292},{{
_temp19, _temp19, _temp19 + 7u}, 261},{{ _temp20, _temp20, _temp20 + 9u}, 300},{{
_temp21, _temp21, _temp21 + 5u}, 305},{{ _temp22, _temp22, _temp22 + 6u}, 268},{{
_temp23, _temp23, _temp23 + 4u}, 287},{{ _temp24, _temp24, _temp24 + 8u}, 310},{{
_temp25, _temp25, _temp25 + 7u}, 313},{{ _temp26, _temp26, _temp26 + 5u}, 288},{{
_temp27, _temp27, _temp27 + 3u}, 282},{{ _temp28, _temp28, _temp28 + 7u}, 279},{{
_temp29, _temp29, _temp29 + 4u}, 266},{{ _temp30, _temp30, _temp30 + 4u}, 294},{{
_temp31, _temp31, _temp31 + 5u}, 267},{{ _temp32, _temp32, _temp32 + 7u}, 315},{{
_temp33, _temp33, _temp33 + 10u}, 302},{{ _temp34, _temp34, _temp34 + 4u}, 298},{{
_temp35, _temp35, _temp35 + 5u}, 293},{{ _temp36, _temp36, _temp36 + 9u}, 281},{{
_temp37, _temp37, _temp37 + 7u}, 309},{{ _temp38, _temp38, _temp38 + 9u}, 316},{{
_temp39, _temp39, _temp39 + 7u}, 317},{{ _temp40, _temp40, _temp40 + 9u}, 259},{{
_temp41, _temp41, _temp41 + 9u}, 274},{{ _temp42, _temp42, _temp42 + 7u}, 291},{{
_temp43, _temp43, _temp43 + 8u}, 319},{{ _temp44, _temp44, _temp44 + 5u}, 318},{{
_temp45, _temp45, _temp45 + 6u}, 312},{{ _temp46, _temp46, _temp46 + 6u}, 265},{{
_temp47, _temp47, _temp47 + 7u}, 270},{{ _temp48, _temp48, _temp48 + 7u}, 280},{{
_temp49, _temp49, _temp49 + 7u}, 308},{{ _temp50, _temp50, _temp50 + 7u}, 314},{{
_temp51, _temp51, _temp51 + 7u}, 260},{{ _temp52, _temp52, _temp52 + 7u}, 275},{{
_temp53, _temp53, _temp53 + 7u}, 284},{{ _temp54, _temp54, _temp54 + 6u}, 295},{{
_temp55, _temp55, _temp55 + 4u}, 296},{{ _temp56, _temp56, _temp56 + 7u}, 303},{{
_temp57, _temp57, _temp57 + 8u}, 262},{{ _temp58, _temp58, _temp58 + 6u}, 276},{{
_temp59, _temp59, _temp59 + 9u}, 271},{{ _temp60, _temp60, _temp60 + 6u}, 301},{{
_temp61, _temp61, _temp61 + 5u}, 263},{{ _temp62, _temp62, _temp62 + 9u}, 273},{{
_temp63, _temp63, _temp63 + 6u}, 285},{{ _temp64, _temp64, _temp64 + 8u}, 304},{{
_temp65, _temp65, _temp65 + 8u}, 311}}; static int Cyc_Lex_str_index( struct
_tagged_arr buff, int offset, int len){ int i= offset; int last=( offset + len)
- 1; struct Cyc_Lex_Trie* t= Cyc_Lex_ids_trie; while( i <= last) { int ch=( int)*((
const unsigned char*) _check_unknown_subscript( buff, sizeof( unsigned char), i))
- 48; if( ch > 47){ ch -= 12;} else{ if( ch > 16){ ch -= 7;}} if((( struct Cyc_Lex_Trie*)
_check_null( t))->children == 0){ while( i <= last) { struct Cyc_Lex_Trie**
_temp66=( struct Cyc_Lex_Trie**)({ unsigned int _temp70= 63u; struct Cyc_Lex_Trie**
_temp71=( struct Cyc_Lex_Trie**) GC_malloc( sizeof( struct Cyc_Lex_Trie*) *
_temp70);{ unsigned int _temp72= _temp70; unsigned int q; for( q= 0; q < _temp72;
q ++){ _temp71[ q]= 0;}}; _temp71;}); struct Cyc_Lex_Trie*** _temp67=({ struct
Cyc_Lex_Trie*** _temp69=( struct Cyc_Lex_Trie***) GC_malloc( sizeof( struct Cyc_Lex_Trie**));
_temp69[ 0]= _temp66; _temp69;});(( struct Cyc_Lex_Trie*) _check_null( t))->children=(
struct Cyc_Lex_Trie***) _temp67;(*(( struct Cyc_Lex_Trie***) _check_null(((
struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)]=({ struct Cyc_Lex_Trie* _temp68=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp68->children=
0; _temp68->shared_str= - 1; _temp68;}); t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)]; ++ i; ch=( int)*(( const
unsigned char*) _check_unknown_subscript( buff, sizeof( unsigned char), i)) - 48;
if( ch > 47){ ch -= 12;} else{ if( ch > 16){ ch -= 7;}}}{ struct _tagged_arr
_temp73= Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy( _temp73,
_tagged_arr_plus( buff, sizeof( unsigned char), offset), len);{ int ans=(( int(*)(
struct Cyc_Xarray_Xarray*, struct _tagged_arr*)) Cyc_Xarray_add_ind)(( struct
Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols),({ struct _tagged_arr* _temp74=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp74[ 0]=(
struct _tagged_arr) _temp73; _temp74;}));(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str=
ans; return ans;}}} if((*(( struct Cyc_Lex_Trie***) _check_null((( struct Cyc_Lex_Trie*)
_check_null( t))->children)))[ _check_known_subscript_notnull( 63u, ch)] == 0){(*((
struct Cyc_Lex_Trie***) _check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)]=({ struct Cyc_Lex_Trie* _temp75=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp75->children=
0; _temp75->shared_str= - 1; _temp75;});} t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)]; ++ i;} if((( struct Cyc_Lex_Trie*)
_check_null( t))->shared_str == - 1){ struct _tagged_arr _temp76= Cyc_Core_new_string(
len + 1); Cyc_String_zstrncpy( _temp76, _tagged_arr_plus( buff, sizeof(
unsigned char), offset), len);{ int ans=(( int(*)( struct Cyc_Xarray_Xarray*,
struct _tagged_arr*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*)
_check_null( Cyc_Lex_symbols),({ struct _tagged_arr* _temp77=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp77[ 0]=( struct
_tagged_arr) _temp76; _temp77;}));(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str=
ans; return ans;}} return(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str;}
static int Cyc_Lex_str_index_lbuf( struct Cyc_Lexing_lexbuf* lbuf){ return Cyc_Lex_str_index((
struct _tagged_arr) lbuf->lex_buffer, lbuf->lex_start_pos, lbuf->lex_curr_pos -
lbuf->lex_start_pos);} struct _tuple10{ struct Cyc_Lex_Trie** f1; } ; static
void Cyc_Lex_insert_typedef( struct _tagged_arr* sptr){ struct _tagged_arr
_temp78=* sptr; int len=( int)( _get_arr_size( _temp78, sizeof( unsigned char))
- 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len;
++ i){ int ch=( int)*(( const unsigned char*) _check_unknown_subscript( _temp78,
sizeof( unsigned char), i)) - 48; if( ch > 47){ ch -= 12;} else{ if( ch > 16){
ch -= 7;}} if((( struct Cyc_Lex_Trie*) _check_null( t))->children == 0){ while(
i < len) {(( struct Cyc_Lex_Trie*) _check_null( t))->children=( struct Cyc_Lex_Trie***)({
struct _tuple10* _temp79=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp79->f1=({ unsigned int _temp80= 63u; struct Cyc_Lex_Trie** _temp81=( struct
Cyc_Lex_Trie**) GC_malloc( sizeof( struct Cyc_Lex_Trie*) * _temp80);{
unsigned int _temp82= _temp80; unsigned int i; for( i= 0; i < _temp82; i ++){
_temp81[ i]= 0;}}; _temp81;}); _temp79;});(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)]=({ struct Cyc_Lex_Trie* _temp83=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp83->children=
0; _temp83->shared_str= 0; _temp83;}); t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)]; ++ i; ch=( int)*(( const
unsigned char*) _check_unknown_subscript( _temp78, sizeof( unsigned char), i)) -
48; if( ch > 47){ ch -= 12;} else{ if( ch > 16){ ch -= 7;}}}(( struct Cyc_Lex_Trie*)
_check_null( t))->shared_str= 1; return;} if((*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)] == 0){(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)]=({ struct Cyc_Lex_Trie* _temp84=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp84->children=
0; _temp84->shared_str= 0; _temp84;});} t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 63u, ch)];}}(( struct Cyc_Lex_Trie*) _check_null(
t))->shared_str= 1; return;} static struct _tagged_arr* Cyc_Lex_get_symbol( int
symbol_num){ return(( struct _tagged_arr*(*)( struct Cyc_Xarray_Xarray*, int))
Cyc_Xarray_get)(( struct Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols),
symbol_num);} static int Cyc_Lex_int_of_char( unsigned char c){ if('0' <= c? c
<='9': 0){ return c -'0';} else{ if('a' <= c? c <='f': 0){ return( 10 + c) -'a';}
else{ if('A' <= c? c <='F': 0){ return( 10 + c) -'A';} else{( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp85=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp85[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp86; _temp86.tag= Cyc_Core_InvalidArg; _temp86.f1=
_tag_arr("string to integer conversion", sizeof( unsigned char), 29u); _temp86;});
_temp85;}));}}}} struct _tuple6* Cyc_Lex_intconst( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base){ unsigned int n= 0; int end= lbuf->lex_curr_pos;
struct _tagged_arr buff= lbuf->lex_buffer; int i= start + lbuf->lex_start_pos;{
int i= start + lbuf->lex_start_pos; for( 0; i < end; ++ i){ unsigned char c=*((
unsigned char*) _check_unknown_subscript( buff, sizeof( unsigned char), i));
switch( c){ case 'u': _LL87: goto _LL88; case 'U': _LL88: return({ struct
_tuple6* _temp90=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp90->f1=(
void*) Cyc_Absyn_Unsigned; _temp90->f2=( int) n; _temp90;}); case 'l': _LL89:
break; case 'L': _LL91: break; default: _LL92: n= n * base +( unsigned int) Cyc_Lex_int_of_char(
c); break;}}} return({ struct _tuple6* _temp94=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp94->f1=( void*) Cyc_Absyn_Signed; _temp94->f2=(
int) n; _temp94;});} static unsigned char Cyc_Lex_char_for_octal_code( struct
Cyc_Lexing_lexbuf* lb, int start, int howmany){ int c= 0;{ int i= 0; for( 0; i <
howmany; i ++){ c=( 8 * c +(( unsigned char(*)( struct Cyc_Lexing_lexbuf*, int))
Cyc_Lexing_lexeme_char)( lb, start + i)) - 48;}} return( unsigned char) c;}
static unsigned char Cyc_Lex_char_for_hex_code( struct _tagged_arr s, int start){
int c= 0; int len= Cyc_String_strlen( s);{ int i= 0; for( 0; start + i < len; i
++){ unsigned char x=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), start + i)); if('0' <= x? x <='9': 0){ c=( 16 * c +( int)
x) -( int)'0';} else{ if('A' <= x? x <='F': 0){ c=( 16 * c +( int) x) -( int)'A';}
else{ if('a' <= x? x <='f': 0){ c=( 16 * c +( int) x) -( int)'a';} else{ break;}}}}}
return( unsigned char) c;} unsigned char Cyc_Lex_string_buffer_v[ 10u]={'x','x','x','x','x','x','x','x','x','x'};
struct _tagged_arr Cyc_Lex_string_buffer={( void*) Cyc_Lex_string_buffer_v,(
void*) Cyc_Lex_string_buffer_v,( void*) Cyc_Lex_string_buffer_v + 10u}; int Cyc_Lex_string_pos=
0; void Cyc_Lex_store_string_char( unsigned char c){ int sz=( int) _get_arr_size(
Cyc_Lex_string_buffer, sizeof( unsigned char)); if( Cyc_Lex_string_pos >= sz){
int newsz= sz; while( Cyc_Lex_string_pos >= newsz) { newsz= newsz * 2;}{ struct
_tagged_arr str=({ unsigned int _temp95=( unsigned int) newsz; unsigned char*
_temp96=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp95);
struct _tagged_arr _temp98= _tag_arr( _temp96, sizeof( unsigned char),(
unsigned int) newsz);{ unsigned int _temp97= _temp95; unsigned int i; for( i= 0;
i < _temp97; i ++){ _temp96[ i]= i < sz?*(( unsigned char*)
_check_unknown_subscript( Cyc_Lex_string_buffer, sizeof( unsigned char),( int) i)):'\000';}};
_temp98;}); Cyc_Lex_string_buffer= str;}}*(( unsigned char*)
_check_unknown_subscript( Cyc_Lex_string_buffer, sizeof( unsigned char), Cyc_Lex_string_pos))=
c; ++ Cyc_Lex_string_pos;} struct _tagged_arr Cyc_Lex_get_stored_string(){
struct _tagged_arr str= Cyc_String_substring(( struct _tagged_arr) Cyc_Lex_string_buffer,
0, Cyc_Lex_string_pos); Cyc_Lex_string_pos= 0; return str;} struct Cyc_Lex_Ldecls{
struct Cyc_Set_Set* typedefs; struct Cyc_Set_Set* namespaces; } ; struct Cyc_Lex_Lvis{
struct Cyc_List_List* current_namespace; struct Cyc_List_List*
imported_namespaces; } ; struct Cyc_Lex_Lstate{ struct Cyc_List_List* lstack;
struct Cyc_Dict_Dict* decls; } ; static struct Cyc_Core_Opt* Cyc_Lex_lstate= 0;
static void Cyc_Lex_typedef_init(){ struct Cyc_Lex_Lvis* _temp99=({ struct Cyc_Lex_Lvis*
_temp106=( struct Cyc_Lex_Lvis*) GC_malloc( sizeof( struct Cyc_Lex_Lvis));
_temp106->current_namespace= 0; _temp106->imported_namespaces= 0; _temp106;});
struct Cyc_List_List* _temp100=({ struct Cyc_List_List* _temp105=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp105->hd=( void*) _temp99;
_temp105->tl= 0; _temp105;}); struct Cyc_Dict_Dict* _temp101=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), 0,({ struct
Cyc_Lex_Ldecls* _temp104=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp104->typedefs=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp104->namespaces=((
struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp104;})); Cyc_Lex_lstate=({ struct
Cyc_Core_Opt* _temp102=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp102->v=( void*)({ struct Cyc_Lex_Lstate* _temp103=( struct Cyc_Lex_Lstate*)
GC_malloc( sizeof( struct Cyc_Lex_Lstate)); _temp103->lstack= _temp100; _temp103->decls=
_temp101; _temp103;}); _temp102;});} static struct Cyc_List_List* Cyc_Lex_get_absolute_namespace(
struct Cyc_List_List* ns){ struct _tagged_arr* n=( struct _tagged_arr*) ns->hd;{
struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack; for( 0; ls != 0;
ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ struct Cyc_Lex_Lvis* lv=(
struct Cyc_Lex_Lvis*)(( struct Cyc_List_List*) _check_null( ls))->hd; struct Cyc_List_List*
x=({ struct Cyc_List_List* _temp107=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp107->hd=( void*) lv->current_namespace; _temp107->tl=
lv->imported_namespaces; _temp107;}); for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct
Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( x))->hd); if((( int(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->namespaces,
n)){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)(( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( x))->hd,( struct Cyc_List_List*) ns);}}}} Cyc_yyerror(( struct
_tagged_arr)({ struct _tagged_arr _temp108=( struct _tagged_arr) Cyc_String_str_sepstr((
struct Cyc_List_List*) ns, _tag_arr("::", sizeof( unsigned char), 3u)); xprintf("undeclared namespace %.*s",
_get_arr_size( _temp108, 1u), _temp108.curr);})); return 0;} static void Cyc_Lex_recompute_typedefs(){
Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp109=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp109->children= 0; _temp109->shared_str=
0; _temp109;});{ struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack; for( 0; ls != 0;
ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ struct Cyc_Lex_Lvis* lv=(
struct Cyc_Lex_Lvis*)(( struct Cyc_List_List*) _check_null( ls))->hd; struct Cyc_List_List*
x=({ struct Cyc_List_List* _temp110=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp110->hd=( void*) lv->current_namespace; _temp110->tl=
lv->imported_namespaces; _temp110;}); for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct
Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( x))->hd);(( void(*)(
void(* f)( struct _tagged_arr*), struct Cyc_Set_Set* s)) Cyc_Set_iter)( Cyc_Lex_insert_typedef,
ld->typedefs);}}}} static int Cyc_Lex_is_typedef_in_namespace( struct Cyc_List_List*
ns, struct _tagged_arr* v){ struct Cyc_List_List* ans= Cyc_Lex_get_absolute_namespace(
ns); struct _handler_cons _temp111; _push_handler(& _temp111);{ int _temp113= 0;
if( setjmp( _temp111.handler)){ _temp113= 1;} if( ! _temp113){{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls, ans); int _temp114=(( int(*)( struct
Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->typedefs, v);
_npop_handler( 0u); return _temp114;}; _pop_handler();} else{ void* _temp112=(
void*) _exn_thrown; void* _temp116= _temp112; _LL118: if( _temp116 == Cyc_Dict_Absent){
goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL119: return 0; _LL121:(
void) _throw( _temp116); _LL117:;}}} static int Cyc_Lex_is_typedef( struct Cyc_List_List*
ns, struct _tagged_arr* v){ if( ns != 0){ return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*) _check_null( ns), v);}{ struct _tagged_arr _temp122=* v;
int len=( int)( _get_arr_size( _temp122, sizeof( unsigned char)) - 1); struct
Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len; ++ i){ if( t
== 0? 1:(( struct Cyc_Lex_Trie*) _check_null( t))->children == 0){ return 0;}
else{ int ch=( int)*(( const unsigned char*) _check_unknown_subscript( _temp122,
sizeof( unsigned char), i)) - 48; if( ch > 47){ ch -= 12;} else{ if( ch > 16){
ch -= 7;}} t=(*(( struct Cyc_Lex_Trie***) _check_null((( struct Cyc_Lex_Trie*)
_check_null( t))->children)))[ _check_known_subscript_notnull( 63u, ch)];}}} if(
t == 0){ return 0;} return(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str;}}
void Cyc_Lex_enter_namespace( struct _tagged_arr* s){ struct Cyc_List_List* ns=((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace; struct Cyc_List_List*
new_ns=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( ns,({ struct Cyc_List_List* _temp127=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp127->hd=( void*) s; _temp127->tl=
0; _temp127;}));(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->lstack=({ struct Cyc_List_List* _temp123=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp123->hd=( void*)({ struct Cyc_Lex_Lvis*
_temp124=( struct Cyc_Lex_Lvis*) GC_malloc( sizeof( struct Cyc_Lex_Lvis));
_temp124->current_namespace= new_ns; _temp124->imported_namespaces= 0; _temp124;});
_temp123->tl=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack; _temp123;});{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls, ns); if( !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->namespaces, s)){(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->decls, ns,({ struct Cyc_Lex_Ldecls* _temp125=( struct Cyc_Lex_Ldecls*)
GC_malloc( sizeof( struct Cyc_Lex_Ldecls)); _temp125->typedefs= ld->typedefs;
_temp125->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_insert)( ld->namespaces, s); _temp125;}));(( struct
Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls, new_ns,({ struct
Cyc_Lex_Ldecls* _temp126=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp126->typedefs=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp126->namespaces=((
struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp126;}));} Cyc_Lex_recompute_typedefs();}}
void Cyc_Lex_leave_namespace(){(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack=( struct Cyc_List_List*) _check_null((((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();} void Cyc_Lex_enter_using( struct _tuple0* q){
struct Cyc_List_List* ns;{ void* _temp128=(* q).f1; struct Cyc_List_List*
_temp136; struct Cyc_List_List* _temp138; _LL130: if( _temp128 ==( void*) Cyc_Absyn_Loc_n){
goto _LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp128 > 1u?*((
int*) _temp128) == Cyc_Absyn_Rel_n: 0){ _LL137: _temp136=(( struct Cyc_Absyn_Rel_n_struct*)
_temp128)->f1; goto _LL133;} else{ goto _LL134;} _LL134: if(( unsigned int)
_temp128 > 1u?*(( int*) _temp128) == Cyc_Absyn_Abs_n: 0){ _LL139: _temp138=((
struct Cyc_Absyn_Abs_n_struct*) _temp128)->f1; goto _LL135;} else{ goto _LL129;}
_LL131: ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp140=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp140->hd=( void*)(*
q).f2; _temp140->tl= 0; _temp140;}); goto _LL129; _LL133: _temp138= _temp136;
goto _LL135; _LL135: ns=( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp138,({
struct Cyc_List_List* _temp141=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp141->hd=( void*)(* q).f2; _temp141->tl= 0; _temp141;})));
goto _LL129; _LL129:;}{ struct Cyc_List_List* _temp142= Cyc_Lex_get_absolute_namespace(
ns); struct Cyc_List_List* _temp143=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({ struct
Cyc_List_List* _temp144=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp144->hd=( void*) _temp142; _temp144->tl= _temp143; _temp144;}); Cyc_Lex_recompute_typedefs();}}
void Cyc_Lex_leave_using(){ struct Cyc_List_List* _temp145=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=(( struct
Cyc_List_List*) _check_null( _temp145))->tl; Cyc_Lex_recompute_typedefs();} void
Cyc_Lex_register_typedef( struct _tuple0* q){ struct Cyc_List_List* _temp146=((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace; struct Cyc_Dict_Dict*
_temp147=(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls;
struct Cyc_Lex_Ldecls* _temp148=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp147, _temp146); struct Cyc_Lex_Ldecls*
_temp149=({ struct Cyc_Lex_Ldecls* _temp150=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp150->typedefs=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( _temp148->typedefs,(*
q).f2); _temp150->namespaces= _temp148->namespaces; _temp150;});(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)( _temp147, _temp146, _temp149); Cyc_Lex_insert_typedef((* q).f2);}
static short Cyc_Lex_process_id( struct Cyc_Lexing_lexbuf* lbuf){ int symbol_num=((
int(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_str_index_lbuf)( lbuf); if(
symbol_num < Cyc_Lex_num_kws){ return( short)*(( int*) _check_unknown_subscript(
Cyc_Lex_kw_nums, sizeof( int), symbol_num));}{ struct _tagged_arr* _temp151= Cyc_Lex_get_symbol(
symbol_num); if( Cyc_Lex_is_typedef( 0, _temp151)){ Cyc_Lex_token_qvar=({ struct
_tuple0* _temp152=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp152->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct* _temp153=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp153[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp154; _temp154.tag= Cyc_Absyn_Rel_n; _temp154.f1= 0; _temp154;}); _temp153;});
_temp152->f2= _temp151; _temp152;}); return 351;} Cyc_Lex_token_string=*
_temp151; return 344;}} static short Cyc_Lex_process_qual_id( struct Cyc_Lexing_lexbuf*
lbuf){ int i= lbuf->lex_start_pos; int end= lbuf->lex_curr_pos; struct
_tagged_arr s= lbuf->lex_buffer; struct Cyc_List_List* rev_vs= 0; while( i < end) {
int start= i; for( 0; i < end?*(( unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)) !=':': 0; i ++){;} if( start == i){( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp155=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp155[ 0]=({ struct
Cyc_Core_Impossible_struct _temp156; _temp156.tag= Cyc_Core_Impossible; _temp156.f1=
_tag_arr("bad namespace", sizeof( unsigned char), 14u); _temp156;}); _temp155;}));}{
int vlen= i - start; struct _tagged_arr* v= Cyc_Lex_get_symbol( Cyc_Lex_str_index((
struct _tagged_arr) s, start, vlen)); rev_vs=({ struct Cyc_List_List* _temp157=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp157->hd=(
void*) v; _temp157->tl= rev_vs; _temp157;}); i += 2;}} if( rev_vs == 0){( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp158=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp158[ 0]=({ struct
Cyc_Core_Impossible_struct _temp159; _temp159.tag= Cyc_Core_Impossible; _temp159.f1=
_tag_arr("bad namespace", sizeof( unsigned char), 14u); _temp159;}); _temp158;}));}{
struct _tagged_arr* v=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( rev_vs))->hd; struct Cyc_List_List* vs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)((( struct Cyc_List_List*)
_check_null( rev_vs))->tl); Cyc_Lex_token_qvar=({ struct _tuple0* _temp160=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp160->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp161=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp161[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp162; _temp162.tag= Cyc_Absyn_Rel_n; _temp162.f1= vs; _temp162;}); _temp161;});
_temp160->f2= v; _temp160;}); if( Cyc_Lex_is_typedef( vs, v)){ return 351;}
else{ return 350;}}} extern int Cyc_Lex_token( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*); int Cyc_yylex(){ int ans=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Parse_lbuf))->v); Cyc_yylloc.first_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); Cyc_yylloc.last_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_end)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); switch( ans){ case 344: _LL163: Cyc_yylval=(
void*)({ struct Cyc_String_tok_struct* _temp165=( struct Cyc_String_tok_struct*)
GC_malloc( sizeof( struct Cyc_String_tok_struct)); _temp165[ 0]=({ struct Cyc_String_tok_struct
_temp166; _temp166.tag= Cyc_String_tok; _temp166.f1= Cyc_Lex_token_string;
_temp166;}); _temp165;}); break; case 350: _LL164: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp168=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp168[ 0]=({ struct Cyc_QualId_tok_struct
_temp169; _temp169.tag= Cyc_QualId_tok; _temp169.f1= Cyc_Lex_token_qvar;
_temp169;}); _temp168;}); break; case 351: _LL167: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp171=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp171[ 0]=({ struct Cyc_QualId_tok_struct
_temp172; _temp172.tag= Cyc_QualId_tok; _temp172.f1= Cyc_Lex_token_qvar;
_temp172;}); _temp171;}); break; case 349: _LL170: Cyc_yylval=( void*)({ struct
Cyc_String_tok_struct* _temp174=( struct Cyc_String_tok_struct*) GC_malloc(
sizeof( struct Cyc_String_tok_struct)); _temp174[ 0]=({ struct Cyc_String_tok_struct
_temp175; _temp175.tag= Cyc_String_tok; _temp175.f1= Cyc_Lex_token_string;
_temp175;}); _temp174;}); break; case 345: _LL173: Cyc_yylval=( void*)({ struct
Cyc_Int_tok_struct* _temp177=( struct Cyc_Int_tok_struct*) GC_malloc( sizeof(
struct Cyc_Int_tok_struct)); _temp177[ 0]=({ struct Cyc_Int_tok_struct _temp178;
_temp178.tag= Cyc_Int_tok; _temp178.f1= Cyc_Lex_token_int; _temp178;}); _temp177;});
break; case 347: _LL176: Cyc_yylval=( void*)({ struct Cyc_Char_tok_struct*
_temp180=( struct Cyc_Char_tok_struct*) GC_malloc( sizeof( struct Cyc_Char_tok_struct));
_temp180[ 0]=({ struct Cyc_Char_tok_struct _temp181; _temp181.tag= Cyc_Char_tok;
_temp181.f1= Cyc_Lex_token_char; _temp181;}); _temp180;}); break; case 348:
_LL179: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp183=( struct
Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp183[ 0]=({ struct Cyc_String_tok_struct _temp184; _temp184.tag= Cyc_String_tok;
_temp184.f1= Cyc_Lex_token_string; _temp184;}); _temp183;}); break; case 346:
_LL182: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp186=( struct
Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp186[ 0]=({ struct Cyc_String_tok_struct _temp187; _temp187.tag= Cyc_String_tok;
_temp187.f1= Cyc_Lex_token_string; _temp187;}); _temp186;}); break; default:
_LL185: break;} return ans;} struct Cyc_Lexing_lex_tables* Cyc_Lex_lt= 0; int
Cyc_Lex_lbase[ 151u]={ 0, 113, 17, 83, 16, 2, - 3, - 1, - 2, - 19, - 20, 4, 118,
119, - 21, 5, - 13, - 12, 85, - 14, - 11, - 4, - 5, - 6, - 7, - 8, - 9, - 10,
209, 305, 111, - 15, 166, - 20, - 57, 8, 30, - 39, 12, 31, 116, 175, 32, 135,
141, 192, 145, 384, 427, 83, 82, 84, 87, 497, 85, 572, 647, 94, - 56, - 22, - 28,
722, 797, 93, 855, 930, 98, - 23, 1005, 96, - 26, 119, - 31, - 25, - 34, 1080,
1109, 292, 96, 107, 394, 1119, 1149, 465, 1090, 1182, 1213, 1251, 103, 114, 1321,
1359, 106, 117, 109, 119, 114, 128, - 6, - 38, 13, - 37, 6, 138, 1291, - 33, -
16, - 18, - 32, - 17, - 19, 2, 1399, 150, 158, 413, 162, 168, 169, 170, 171, 173,
180, 185, 189, 1472, 1556, - 54, - 46, - 45, - 44, - 43, - 42, - 41, - 40, - 47,
- 50, - 53, 488, - 52, 191, - 51, - 48, - 49, - 55, - 27, - 24, 14, - 35, 18,
477}; int Cyc_Lex_lbacktrk[ 151u]={ - 1, - 1, - 1, 5, 3, 4, - 1, - 1, - 1, - 1,
- 1, 18, 1, 21, - 1, 2, - 1, - 1, 16, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 17, 15, - 1, - 1, - 1, - 1, 35, 56, - 1, 56, 56, 56, 56, 56, 56, 56, 56,
56, 5, 7, 56, 56, 56, 56, 0, 56, 56, 56, 56, - 1, - 1, - 1, 3, 1, - 1, - 1, 2, -
1, - 1, 0, 29, - 1, 28, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9,
5, 6, 5, 5, - 1, 4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, - 1, 36, - 1, 9, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 34, 35}; int Cyc_Lex_ldefault[
151u]={ 34, 9, 3, 3, - 1, - 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1, - 1, 0, 147, - 1, -
1, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, 0, - 1, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 100, 0, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 0, - 1, 0, 0, 0, 0, 0, 0, 147, 0, - 1, - 1}; int
Cyc_Lex_ltrans[ 1813u]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 35, 35, 35, 33, 6,
101, 150, 150, 150, 150, 150, 148, 101, 148, 149, 102, 149, 148, 0, 0, 0, 35, 36,
37, 38, 0, 39, 40, 41, 150, 144, 42, 43, 7, 44, 45, 46, 47, 48, 48, 48, 48, 48,
48, 48, 48, 48, 49, 4, 50, 51, 52, 8, 5, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 17, 14, 110, 54,
55, 56, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 10, 57, - 1, 11, 32, 32, 6, - 1, 32, 15, 30, 30,
30, 30, 30, 30, 30, 30, 74, 71, 16, 72, 20, 67, 12, 19, 69, 32, 64, 7, 16, 145,
59, 64, 70, 17, 31, 31, 31, 31, 31, 31, 31, 31, 18, 18, 18, 18, 18, 18, 18, 18,
32, 32, 146, 31, 32, 73, 25, 19, 25, 105, - 1, 106, 99, 23, 143, 23, 22, 100, 22,
24, 24, 109, 142, 32, 98, 7, 136, 107, 108, 98, 13, 33, 135, 134, 133, 132, 20,
131, 25, - 1, 25, 21, 22, 60, 130, 23, 23, 23, 22, 129, 22, 24, 24, 128, 24, 141,
98, 0, 25, 0, 26, 98, 27, 103, 28, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 0, 0, 0, 0, 0, 0, 58, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 112, - 1, - 1,
- 1, 0, 0, 6, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 80, 0, 80, 0, - 1, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 0, 0, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 14, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 75, - 1, 86, 86, 86, 86, 86, 86, 86,
86, 87, 87, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 137, 77, 0, 0, 0, 0, 0, 0,
88, 138, 138, 138, 138, 138, 138, 138, 138, 89, 0, 0, 90, 75, 0, 76, 76, 76, 76,
76, 76, 76, 76, 76, 76, 77, 150, 150, 150, 150, 150, 0, 88, 0, 0, 0, 77, 0, 0, 0,
0, 89, 0, 78, 90, 0, 0, 0, 84, 150, 84, 0, 79, 85, 85, 85, 85, 85, 85, 85, 85,
85, 85, 0, 0, 0, 0, 139, 77, 0, 0, 0, 0, 0, 0, 78, 140, 140, 140, 140, 140, 140,
140, 140, 79, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0,
62, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0,
0, 0, 0, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 62, 0, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 66, 0, 0, 0, 0, 0, 0, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 85, 85, 85, 85, 85,
85, 85, 85, 85, 85, 0, 83, 27, 0, 0, 0, 0, 75, 27, 76, 76, 76, 76, 76, 76, 76,
76, 76, 76, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 77, 0, 0, 83, 27, 0, 0,
78, 0, 0, 27, 26, 0, 0, 0, 0, 79, 26, 0, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,
0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 78, 83, 27, 0, 26, 0, 0, 0, 27, 79, 26, 0, 0, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 27, 27, 0,
0, 0, 0, 27, 27, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 77, 0, 27, 0, 0, 0, 0, 96, 27, 0, 0, 0, 0, 0, 0, 75, 97, 87, 87,
87, 87, 87, 87, 87, 87, 87, 87, 0, 0, 0, 0, 0, 77, 0, 0, 0, 0, 0, 77, 96, 0, 0,
0, 0, 0, 94, 0, 0, 97, 0, 0, 0, 0, 0, 95, 0, 0, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 94, 83, 27, 0, 0, 0, 0, 0, 27,
95, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91,
91, 91, 83, 27, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91, 91, 91,
91, 91, 91, 91, 0, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 113,
0, 92, 0, 0, 114, 0, 0, 0, 0, 0, 93, 0, 0, 115, 115, 115, 115, 115, 115, 115,
115, 0, 91, 91, 91, 91, 91, 91, 116, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 93,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 0, 0, 0, 0, 118, 119, 0, 0, 0,
120, 0, 0, 0, 0, 0, 0, 0, 121, 0, 0, 0, 122, 0, 123, 0, 124, 0, 125, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 127, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int Cyc_Lex_lcheck[ 1813u]={ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 11, 15, 102, 35, 35, 35, 35, 35, 38, 100, 147, 38, 100, 147, 149, - 1,
- 1, - 1, 0, 0, 0, 0, - 1, 0, 0, 0, 35, 111, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 12, 12, 13, 3, 12,
13, 18, 18, 18, 18, 18, 18, 18, 18, 49, 50, 50, 50, 51, 54, 1, 52, 52, 12, 63,
12, 13, 40, 57, 66, 69, 13, 30, 30, 30, 30, 30, 30, 30, 30, 13, 13, 13, 13, 13,
13, 13, 13, 32, 32, 40, 43, 32, 71, 78, 13, 79, 103, 41, 44, 46, 88, 113, 89, 92,
46, 93, 94, 95, 43, 114, 32, 96, 32, 116, 44, 44, 97, 1, 46, 117, 118, 119, 120,
13, 121, 78, 41, 79, 13, 13, 57, 122, 88, 13, 89, 92, 123, 93, 94, 95, 124, 13,
140, 96, - 1, 13, - 1, 13, 97, 13, 45, 13, 45, 45, 45, 45, 45, 45, 45, 45, 45,
45, - 1, - 1, - 1, - 1, - 1, - 1, 0, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 41,
38, 100, 147, - 1, - 1, 2, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, - 1, - 1, - 1, - 1, - 1, - 1,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, - 1, - 1, - 1, 77, - 1, 77, - 1, 3, 77, 77, 77, 77, 77,
77, 77, 77, 77, 77, - 1, - 1, - 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1,
- 1, - 1, - 1, - 1, - 1, 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1, - 1, - 1, - 1, - 1, - 1,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, - 1, - 1, 47, 41, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 115, 47, - 1, - 1, - 1, - 1, - 1, - 1,
47, 115, 115, 115, 115, 115, 115, 115, 115, 47, - 1, - 1, 47, 48, - 1, 48, 48,
48, 48, 48, 48, 48, 48, 48, 48, 47, 150, 150, 150, 150, 150, - 1, 47, - 1, - 1,
- 1, 48, - 1, - 1, - 1, - 1, 47, - 1, 48, 47, - 1, - 1, - 1, 83, 150, 83, - 1,
48, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, - 1, - 1, - 1, - 1, 138, 48, - 1, -
1, - 1, - 1, - 1, - 1, 48, 138, 138, 138, 138, 138, 138, 138, 138, 48, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, - 1, - 1, - 1, - 1, - 1, - 1, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, - 1, - 1, - 1, - 1, 53, - 1, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, - 1, - 1, - 1, - 1, - 1, - 1, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, - 1,
- 1, - 1, - 1, 55, - 1, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, - 1, - 1, -
1, - 1, 56, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1, - 1, - 1,
61, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1,
- 1, - 1, - 1, - 1, - 1, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, - 1, 62, - 1, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, - 1, - 1, - 1, - 1, 64, - 1, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1,
- 1, - 1, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, 65, - 1, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, - 1, - 1, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, 68, - 1, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 75, 75,
75, 75, 75, 75, 75, 75, 75, 75, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, - 1, 75,
75, - 1, - 1, - 1, - 1, 76, 75, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81,
81, 81, 81, 81, 81, 81, 81, 81, - 1, 76, - 1, - 1, 75, 75, - 1, - 1, 76, - 1, -
1, 75, 81, - 1, - 1, - 1, - 1, 76, 81, - 1, 82, 82, 82, 82, 82, 82, 82, 82, 82,
82, - 1, - 1, - 1, 76, - 1, - 1, - 1, - 1, - 1, - 1, 76, 82, 82, - 1, 81, - 1, -
1, - 1, 82, 76, 81, - 1, - 1, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 82, 82, 85, - 1, - 1, - 1, - 1, 82, 85,
86, - 1, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 86, - 1, 85, - 1, - 1, - 1, - 1, 86, 85, - 1, - 1, - 1, -
1, - 1, - 1, 87, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, - 1, - 1, - 1, - 1,
- 1, 86, - 1, - 1, - 1, - 1, - 1, 87, 86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1,
86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, - 1, - 1, - 1, 87, - 1, - 1, - 1, - 1, - 1, - 1, 87, 104, 104, -
1, - 1, - 1, - 1, - 1, 104, 87, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 90, 90, 90, 90, 90, 90, 104, 104, - 1, - 1, - 1, - 1, -
1, 104, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, 91, 91, 91, 91, 91, 91, 91, 91,
91, - 1, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 91, - 1, - 1, - 1, 112, - 1,
91, - 1, - 1, 112, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, 112, 112, 112, 112,
112, 112, 112, 112, - 1, 91, 91, 91, 91, 91, 91, 112, - 1, - 1, - 1, - 1, 91, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, - 1, 112, 112, - 1, - 1, - 1,
112, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, 112, - 1, 112, - 1,
112, - 1, 112, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 126, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int Cyc_Lex_token_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables*
_temp189=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp189->lex_base= _tag_arr(( int*) Cyc_Lex_lbase, sizeof( int), 151u);
_temp189->lex_backtrk= _tag_arr(( int*) Cyc_Lex_lbacktrk, sizeof( int), 151u);
_temp189->lex_default= _tag_arr(( int*) Cyc_Lex_ldefault, sizeof( int), 151u);
_temp189->lex_trans= _tag_arr(( int*) Cyc_Lex_ltrans, sizeof( int), 1813u);
_temp189->lex_check= _tag_arr(( int*) Cyc_Lex_lcheck, sizeof( int), 1813u);
_temp189;});} lexstate=(( int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*) _check_null( Cyc_Lex_lt),
lexstate, lexbuf); switch( lexstate){ case 0: _LL190: return( int)(( short(*)(
struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_id)( lexbuf); case 1: _LL191:
return( int)(( short(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_id)(
lexbuf); case 2: _LL192: return( int)(( short(*)( struct Cyc_Lexing_lexbuf* lbuf))
Cyc_Lex_process_qual_id)( lexbuf); case 3: _LL193: Cyc_Lex_token_string=* Cyc_Lex_get_symbol(((
int(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_str_index_lbuf)( lexbuf));
return 349; case 4: _LL194: Cyc_Lex_token_int=(( struct _tuple6*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 2, 16); return 345; case
5: _LL195: Cyc_Lex_token_int=(( struct _tuple6*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 8); return 345; case 6:
_LL196: Cyc_Lex_token_int=(( struct _tuple6*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 345; case 7:
_LL197: Cyc_Lex_token_int=(( struct _tuple6*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 345; case 8:
_LL198: Cyc_Lex_token_string=( struct _tagged_arr)(( struct _tagged_arr(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf); return 348; case 9:
_LL199: Cyc_Lex_token_string=( struct _tagged_arr)(( struct _tagged_arr(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf); return 348; case 10:
_LL200: return 327; case 11: _LL201: return 328; case 12: _LL202: return 325;
case 13: _LL203: return 326; case 14: _LL204: return 321; case 15: _LL205:
return 322; case 16: _LL206: return 334; case 17: _LL207: return 335; case 18:
_LL208: return 331; case 19: _LL209: return 332; case 20: _LL210: return 333;
case 21: _LL211: return 340; case 22: _LL212: return 339; case 23: _LL213:
return 338; case 24: _LL214: return 336; case 25: _LL215: return 337; case 26:
_LL216: return 329; case 27: _LL217: return 330; case 28: _LL218: return 323;
case 29: _LL219: return 324; case 30: _LL220: return 342; case 31: _LL221:
return 320; case 32: _LL222: return 341; case 33: _LL223: return 343; case 34:
_LL224: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)( lexbuf);
case 35: _LL225: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 36: _LL226: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 37: _LL227: Cyc_Lex_comment_depth= 1; Cyc_Lex_runaway_start=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)( struct
Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)( lexbuf); case 38: _LL228: Cyc_Lex_string_pos= 0; Cyc_Lex_runaway_start=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); Cyc_Lex_token_string=(
struct _tagged_arr) Cyc_Lex_get_stored_string(); return 346; case 39: _LL229:
Cyc_Lex_token_char='\a'; return 347; case 40: _LL230: Cyc_Lex_token_char='\b';
return 347; case 41: _LL231: Cyc_Lex_token_char='\f'; return 347; case 42:
_LL232: Cyc_Lex_token_char='\n'; return 347; case 43: _LL233: Cyc_Lex_token_char='\r';
return 347; case 44: _LL234: Cyc_Lex_token_char='\t'; return 347; case 45:
_LL235: Cyc_Lex_token_char='\v'; return 347; case 46: _LL236: Cyc_Lex_token_char='\\';
return 347; case 47: _LL237: Cyc_Lex_token_char='\''; return 347; case 48:
_LL238: Cyc_Lex_token_char='"'; return 347; case 49: _LL239: Cyc_Lex_token_char='?';
return 347; case 50: _LL240: Cyc_Lex_token_char=(( unsigned char(*)( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 2, 3); return
347; case 51: _LL241: Cyc_Lex_token_char=(( unsigned char(*)( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 2, 2); return
347; case 52: _LL242: Cyc_Lex_token_char=(( unsigned char(*)( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 2, 1); return
347; case 53: _LL243: Cyc_Lex_token_char= Cyc_Lex_char_for_hex_code(( struct
_tagged_arr)(( struct _tagged_arr(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)(
lexbuf), 3); return 347; case 54: _LL244: Cyc_Lex_token_char=(( unsigned char(*)(
struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)( lexbuf, 1); return 347;
case 55: _LL245: return - 1; case 56: _LL246: return( int)(( unsigned char(*)(
struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)( lexbuf, 0); default:
_LL247:( lexbuf->refill_buff)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf, lexstate);}( int) _throw((
void*)({ struct Cyc_Lexing_Error_struct* _temp249=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp249[ 0]=({ struct Cyc_Lexing_Error_struct
_temp250; _temp250.tag= Cyc_Lexing_Error; _temp250.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp250;}); _temp249;}));} int Cyc_Lex_token(
struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf, 0);} int Cyc_Lex_strng_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({
struct Cyc_Lexing_lex_tables* _temp251=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp251->lex_base= _tag_arr((
int*) Cyc_Lex_lbase, sizeof( int), 151u); _temp251->lex_backtrk= _tag_arr(( int*)
Cyc_Lex_lbacktrk, sizeof( int), 151u); _temp251->lex_default= _tag_arr(( int*)
Cyc_Lex_ldefault, sizeof( int), 151u); _temp251->lex_trans= _tag_arr(( int*) Cyc_Lex_ltrans,
sizeof( int), 1813u); _temp251->lex_check= _tag_arr(( int*) Cyc_Lex_lcheck,
sizeof( int), 1813u); _temp251;});} lexstate=(( int(*)( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*)
_check_null( Cyc_Lex_lt), lexstate, lexbuf); switch( lexstate){ case 0: _LL252:
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 1:
_LL253: return 0; case 2: _LL254: return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 3: _LL255: Cyc_Lex_store_string_char('\a'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 4: _LL256: Cyc_Lex_store_string_char('\b');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 5:
_LL257: Cyc_Lex_store_string_char('\f'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 6: _LL258: Cyc_Lex_store_string_char('\n'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 7: _LL259: Cyc_Lex_store_string_char('\r');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 8:
_LL260: Cyc_Lex_store_string_char('\t'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 9: _LL261: Cyc_Lex_store_string_char('\v'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 10: _LL262:
Cyc_Lex_store_string_char('\\'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 11: _LL263: Cyc_Lex_store_string_char('\'');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 12:
_LL264: Cyc_Lex_store_string_char('"'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 13: _LL265: Cyc_Lex_store_string_char('?'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 14: _LL266:
Cyc_Lex_store_string_char((( unsigned char(*)( struct Cyc_Lexing_lexbuf* lb, int
start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 3)); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 15: _LL267: Cyc_Lex_store_string_char(((
unsigned char(*)( struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 1, 2)); return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)(
lexbuf); case 16: _LL268: Cyc_Lex_store_string_char((( unsigned char(*)( struct
Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 1, 1)); return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)(
lexbuf); case 17: _LL269: Cyc_Lex_store_string_char( Cyc_Lex_char_for_hex_code((
struct _tagged_arr)(( struct _tagged_arr(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)(
lexbuf), 2)); return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)(
lexbuf); case 18: _LL270: Cyc_Lex_store_string_char((( unsigned char(*)( struct
Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)( lexbuf, 0)); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 19: _LL271:(( void(*)(
struct _tagged_arr msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(
_tag_arr("string ends in newline", sizeof( unsigned char), 23u), lexbuf); return
0; case 20: _LL272:(( void(*)( struct _tagged_arr msg, struct Cyc_Lexing_lexbuf*
lb)) Cyc_Lex_runaway_err)( _tag_arr("unterminated string", sizeof( unsigned char),
20u), lexbuf); return 0; case 21: _LL273:(( void(*)( struct _tagged_arr msg,
struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_err)( _tag_arr("bad character following backslash in string",
sizeof( unsigned char), 44u), lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); default: _LL274:( lexbuf->refill_buff)( lexbuf); return((
int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp276=( struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp276[ 0]=({ struct Cyc_Lexing_Error_struct _temp277; _temp277.tag= Cyc_Lexing_Error;
_temp277.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp277;}); _temp276;}));} int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf* lexbuf){
return(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)(
lexbuf, 1);} int Cyc_Lex_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables*
_temp278=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp278->lex_base= _tag_arr(( int*) Cyc_Lex_lbase, sizeof( int), 151u);
_temp278->lex_backtrk= _tag_arr(( int*) Cyc_Lex_lbacktrk, sizeof( int), 151u);
_temp278->lex_default= _tag_arr(( int*) Cyc_Lex_ldefault, sizeof( int), 151u);
_temp278->lex_trans= _tag_arr(( int*) Cyc_Lex_ltrans, sizeof( int), 1813u);
_temp278->lex_check= _tag_arr(( int*) Cyc_Lex_lcheck, sizeof( int), 1813u);
_temp278;});} lexstate=(( int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*) _check_null( Cyc_Lex_lt),
lexstate, lexbuf); switch( lexstate){ case 0: _LL279: ++ Cyc_Lex_comment_depth;
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 1:
_LL280: -- Cyc_Lex_comment_depth; if( Cyc_Lex_comment_depth > 0){ return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf);} return 0; case 2: _LL281:((
void(*)( struct _tagged_arr msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(
_tag_arr("unterminated comment", sizeof( unsigned char), 21u), lexbuf); return 0;
case 3: _LL282: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf); case 4: _LL283: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf); case 5: _LL284: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf); default: _LL285:( lexbuf->refill_buff)( lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf,
lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp287=(
struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp287[ 0]=({ struct Cyc_Lexing_Error_struct _temp288; _temp288.tag= Cyc_Lexing_Error;
_temp288.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp288;}); _temp287;}));} int Cyc_Lex_comment( struct Cyc_Lexing_lexbuf*
lexbuf){ return(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_comment_rec)(
lexbuf, 2);} void Cyc_Lex_lex_init(){ Cyc_Lex_ids_trie=({ struct Cyc_Lex_Trie*
_temp289=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1);
_temp289[ 0]=({ struct Cyc_Lex_Trie _temp290; _temp290.children= 0; _temp290.shared_str=
- 1; _temp290;}); _temp289;}); Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie*
_temp291=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1);
_temp291[ 0]=({ struct Cyc_Lex_Trie _temp292; _temp292.children= 0; _temp292.shared_str=
0; _temp292;}); _temp291;}); Cyc_Lex_symbols=( struct Cyc_Xarray_Xarray*)((
struct Cyc_Xarray_Xarray*(*)( int, struct _tagged_arr*)) Cyc_Xarray_create)( 101,
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u)); Cyc_Lex_num_kws=( int) 63u; Cyc_Lex_kw_nums=({
unsigned int _temp293=( unsigned int) Cyc_Lex_num_kws; int* _temp294=( int*)
GC_malloc_atomic( sizeof( int) * _temp293); struct _tagged_arr _temp296=
_tag_arr( _temp294, sizeof( int),( unsigned int) Cyc_Lex_num_kws);{ unsigned int
_temp295= _temp293; unsigned int i; for( i= 0; i < _temp295; i ++){ _temp294[ i]=
0;}}; _temp296;});{ int i= 0; for( 0; i < Cyc_Lex_num_kws; ++ i){ struct
_tagged_arr _temp297=( Cyc_Lex_rw_array[ _check_known_subscript_notnull( 63u, i)]).f1;
Cyc_Lex_str_index( _temp297, 0, Cyc_String_strlen( _temp297));*(( int*)
_check_unknown_subscript( Cyc_Lex_kw_nums, sizeof( int), i))=( int)( Cyc_Lex_rw_array[
_check_known_subscript_notnull( 63u, i)]).f2;}} Cyc_Lex_typedef_init(); Cyc_Lex_comment_depth=
0;}