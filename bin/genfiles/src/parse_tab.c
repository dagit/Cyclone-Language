 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)(
struct _tagged_string, int, void*); void* read_fun_state; } ; struct
_tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; struct Cyc_Lexing_lex_tables{
struct _tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct
_tagged_ptr0 lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0
lex_check; } ; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*);
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
Cyc_List_List* x); extern unsigned int Cyc_String_strlen( struct _tagged_string
s); extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strcat( struct
_tagged_string dest, struct _tagged_string src); extern struct _tagged_string
Cyc_String_strcpy( struct _tagged_string dest, struct _tagged_string src);
extern struct _tagged_string Cyc_String_substring( struct _tagged_string, int
ofs, unsigned int n); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct
Cyc_Position_Segment*, struct Cyc_Position_Segment*); extern struct
_tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_string); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct
Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
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
_tagged_string* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
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
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; static
const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y);
extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_starb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ( void*
t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
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
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1,
struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List* el, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
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
struct Cyc_List_List*); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; extern void Cyc_Lex_register_typedef( struct
_tuple1* s); extern void Cyc_Lex_enter_namespace( struct _tagged_string*);
extern void Cyc_Lex_leave_namespace(); extern void Cyc_Lex_enter_using( struct
_tuple1*); extern void Cyc_Lex_leave_using(); static const int Cyc_Parse_Struct_su=
0; static const int Cyc_Parse_Union_su= 1; static const int Cyc_Parse_TopDecls_bl=
0; struct Cyc_Parse_TopDecls_bl_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Parse_Stmt_bl= 1; struct Cyc_Parse_Stmt_bl_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Parse_FnDecl_bl= 2; struct
Cyc_Parse_FnDecl_bl_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ; static
const int Cyc_Parse_Signed_spec= 0; struct Cyc_Parse_Signed_spec_struct{ int tag;
struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Unsigned_spec= 1;
struct Cyc_Parse_Unsigned_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Short_spec= 2; struct Cyc_Parse_Short_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Long_spec=
3; struct Cyc_Parse_Long_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Type_spec= 4; struct Cyc_Parse_Type_spec_struct{
int tag; void* f1; struct Cyc_Position_Segment* f2; } ; static const int Cyc_Parse_Decl_spec=
5; struct Cyc_Parse_Decl_spec_struct{ int tag; struct Cyc_Absyn_Decl* f1; } ;
static const int Cyc_Parse_Typedef_sc= 0; static const int Cyc_Parse_Extern_sc=
1; static const int Cyc_Parse_ExternC_sc= 2; static const int Cyc_Parse_Static_sc=
3; static const int Cyc_Parse_Auto_sc= 4; static const int Cyc_Parse_Register_sc=
5; static const int Cyc_Parse_Abstract_sc= 6; struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual tq; struct Cyc_List_List*
type_specs; int is_inline; struct Cyc_List_List* attributes; } ; struct Cyc_Parse_Declarator{
struct _tuple1* id; struct Cyc_List_List* tms; } ; struct Cyc_Parse_Abstractdeclarator{
struct Cyc_List_List* tms; } ; struct _tuple4{ struct _tagged_string* f1; struct
Cyc_Absyn_Tqual f2; void* f3; } ; static struct _tuple4* Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment* loc, struct _tuple2* t); struct _tuple5{ void* f1;
struct Cyc_Core_Opt* f2; } ; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc); struct _tuple6{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; struct Cyc_List_List*
f4; } ; static struct _tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual, void*,
struct Cyc_List_List*, struct Cyc_List_List*); struct _tuple7{ struct _tuple1*
f1; struct Cyc_Absyn_Tqual f2; void* f3; struct Cyc_List_List* f4; struct Cyc_List_List*
f5; } ; static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t); struct Cyc_Core_Opt* Cyc_Parse_lbuf= 0; static struct
Cyc_List_List* Cyc_Parse_parse_result= 0; struct Cyc_Absyn_Aligned_att_struct
Cyc_Parse_att_aligned={ 1u, - 1}; struct _tuple8{ struct _tagged_string f1; void*
f2; } ; static unsigned char _temp3[ 8u]="stdcall"; static unsigned char _temp6[
6u]="cdecl"; static unsigned char _temp9[ 9u]="fastcall"; static unsigned char
_temp12[ 9u]="noreturn"; static unsigned char _temp15[ 6u]="const"; static
unsigned char _temp18[ 8u]="aligned"; static unsigned char _temp21[ 7u]="packed";
static unsigned char _temp24[ 7u]="shared"; static unsigned char _temp27[ 7u]="unused";
static unsigned char _temp30[ 5u]="weak"; static unsigned char _temp33[ 10u]="dllimport";
static unsigned char _temp36[ 10u]="dllexport"; static unsigned char _temp39[ 23u]="no_instrument_function";
static unsigned char _temp42[ 12u]="constructor"; static unsigned char _temp45[
11u]="destructor"; static unsigned char _temp48[ 22u]="no_check_memory_usage";
struct _tuple8 Cyc_Parse_att_map[ 16u]={{{ _temp3, _temp3, _temp3 + 8u},( void*)
0u},{{ _temp6, _temp6, _temp6 + 6u},( void*) 1u},{{ _temp9, _temp9, _temp9 + 9u},(
void*) 2u},{{ _temp12, _temp12, _temp12 + 9u},( void*) 3u},{{ _temp15, _temp15,
_temp15 + 6u},( void*) 4u},{{ _temp18, _temp18, _temp18 + 8u},( void*)& Cyc_Parse_att_aligned},{{
_temp21, _temp21, _temp21 + 7u},( void*) 5u},{{ _temp24, _temp24, _temp24 + 7u},(
void*) 7u},{{ _temp27, _temp27, _temp27 + 7u},( void*) 8u},{{ _temp30, _temp30,
_temp30 + 5u},( void*) 9u},{{ _temp33, _temp33, _temp33 + 10u},( void*) 10u},{{
_temp36, _temp36, _temp36 + 10u},( void*) 11u},{{ _temp39, _temp39, _temp39 + 23u},(
void*) 12u},{{ _temp42, _temp42, _temp42 + 12u},( void*) 13u},{{ _temp45,
_temp45, _temp45 + 11u},( void*) 14u},{{ _temp48, _temp48, _temp48 + 22u},( void*)
15u}}; static void Cyc_Parse_err( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse( sg, msg));} static void*
Cyc_Parse_abort( struct _tagged_string msg, struct Cyc_Position_Segment* sg){
Cyc_Parse_err( msg, sg);( void) _throw(( void*) Cyc_Position_Exit);} static void
Cyc_Parse_warn( struct _tagged_string msg, struct Cyc_Position_Segment* sg){({
struct _tagged_string _temp49= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp50= msg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: %.*s\n",
_temp49.last_plus_one - _temp49.curr, _temp49.curr, _temp50.last_plus_one -
_temp50.curr, _temp50.curr);}); return;} static void* Cyc_Parse_unimp( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp51= msg; xprintf("%.*s unimplemented", _temp51.last_plus_one
- _temp51.curr, _temp51.curr);}), sg);} static void Cyc_Parse_unimp2( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string
_temp52= Cyc_Position_string_of_segment( sg); struct _tagged_string _temp53= msg;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp52.last_plus_one - _temp52.curr, _temp52.curr, _temp53.last_plus_one -
_temp53.curr, _temp53.curr);}); return;} struct _tuple9{ struct _tuple7* f1;
struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct Cyc_Core_Opt*
_temp56; struct _tuple7* _temp58; struct _tuple9 _temp54=* field_info; _LL59:
_temp58= _temp54.f1; goto _LL57; _LL57: _temp56= _temp54.f2; goto _LL55; _LL55:
if((* _temp58).f4 != 0){ Cyc_Parse_err(({ unsigned char* _temp60="bad type params in struct field";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 32; _temp61;}), loc);}{ struct _tuple1* _temp62=(*
_temp58).f1; goto _LL63; _LL63:{ void* _temp64=(* _temp62).f1; struct Cyc_List_List*
_temp74; struct Cyc_List_List* _temp76; _LL66: if(( unsigned int) _temp64 > 1u?*((
int*) _temp64) == Cyc_Absyn_Rel_n: 0){ _LL75: _temp74=(( struct Cyc_Absyn_Rel_n_struct*)
_temp64)->f1; if( _temp74 == 0){ goto _LL67;} else{ goto _LL68;}} else{ goto
_LL68;} _LL68: if(( unsigned int) _temp64 > 1u?*(( int*) _temp64) == Cyc_Absyn_Abs_n:
0){ _LL77: _temp76=(( struct Cyc_Absyn_Abs_n_struct*) _temp64)->f1; if( _temp76
== 0){ goto _LL69;} else{ goto _LL70;}} else{ goto _LL70;} _LL70: if( _temp64 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL71;} else{ goto _LL72;} _LL72: goto _LL73;
_LL67: goto _LL65; _LL69: goto _LL65; _LL71: goto _LL65; _LL73: Cyc_Parse_err(({
unsigned char* _temp78="struct field cannot be qualified with a module name";
struct _tagged_string _temp79; _temp79.curr= _temp78; _temp79.base= _temp78;
_temp79.last_plus_one= _temp78 + 52; _temp79;}), loc); goto _LL65; _LL65:;}{
struct Cyc_List_List* _temp80=(* _temp58).f5; goto _LL81; _LL81: return({ struct
Cyc_Absyn_Structfield* _temp82=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp82->name=(* _temp62).f2; _temp82->tq=(*
_temp58).f2; _temp82->type=( void*)(* _temp58).f3; _temp82->width= _temp56;
_temp82->attributes= _temp80; _temp82;});}}} struct _tuple10{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; struct Cyc_List_List* f4; } ; static
struct _tuple2* Cyc_Parse_make_param( struct Cyc_Position_Segment* loc, struct
_tuple10* field){ struct _tuple10 _temp85; struct Cyc_List_List* _temp86; void*
_temp88; struct Cyc_Absyn_Tqual _temp90; struct Cyc_Core_Opt* _temp92; struct
_tuple10* _temp83= field; _temp85=* _temp83; _LL93: _temp92= _temp85.f1; goto
_LL91; _LL91: _temp90= _temp85.f2; goto _LL89; _LL89: _temp88= _temp85.f3; goto
_LL87; _LL87: _temp86= _temp85.f4; goto _LL84; _LL84: { struct Cyc_Core_Opt*
_temp94= 0; goto _LL95; _LL95: if( _temp92 != 0){ _temp94=({ struct Cyc_Core_Opt*
_temp96=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp96->v=( void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp92))->v)).f2; _temp96;});{ void* _temp97=(*(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp92))->v)).f1; struct Cyc_List_List* _temp107; struct Cyc_List_List*
_temp109; _LL99: if(( unsigned int) _temp97 > 1u?*(( int*) _temp97) == Cyc_Absyn_Rel_n:
0){ _LL108: _temp107=(( struct Cyc_Absyn_Rel_n_struct*) _temp97)->f1; if(
_temp107 == 0){ goto _LL100;} else{ goto _LL101;}} else{ goto _LL101;} _LL101:
if(( unsigned int) _temp97 > 1u?*(( int*) _temp97) == Cyc_Absyn_Abs_n: 0){
_LL110: _temp109=(( struct Cyc_Absyn_Abs_n_struct*) _temp97)->f1; if( _temp109
== 0){ goto _LL102;} else{ goto _LL103;}} else{ goto _LL103;} _LL103: if(
_temp97 ==( void*) Cyc_Absyn_Loc_n){ goto _LL104;} else{ goto _LL105;} _LL105:
goto _LL106; _LL100: goto _LL98; _LL102: goto _LL98; _LL104: goto _LL98; _LL106:
Cyc_Parse_err(({ unsigned char* _temp111="parameter cannot be qualified with a module name";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 49; _temp112;}), loc); goto _LL98; _LL98:;}}
if( _temp86 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ unsigned char* _temp113="parameter should have no type parameters";
struct _tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 41; _temp114;}), loc);} return({ struct
_tuple2* _temp115=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp115->f1= _temp94; _temp115->f2= _temp90; _temp115->f3= _temp88; _temp115;});}}
static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){
return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp116=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp116[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp117; _temp117.tag= Cyc_Parse_Type_spec; _temp117.f1=(
void*) t; _temp117.f2= loc; _temp117;}); _temp116;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp118= t; struct Cyc_Absyn_Exp* _temp124; struct Cyc_Absyn_Tqual
_temp126; void* _temp128; _LL120: if(( unsigned int) _temp118 > 4u?*(( int*)
_temp118) == Cyc_Absyn_ArrayType: 0){ _LL129: _temp128=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f1; goto _LL127; _LL127: _temp126=(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f2; goto _LL125; _LL125: _temp124=(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f3; goto _LL121;} else{ goto _LL122;} _LL122: goto _LL123; _LL121:
return Cyc_Absyn_starb_typ( _temp128,( void*) Cyc_Absyn_HeapRgn, _temp126,
_temp124 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp130=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp130[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp131; _temp131.tag= Cyc_Absyn_Upper_b;
_temp131.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp124); _temp131;});
_temp130;})); _LL123: return t; _LL119:;} static void Cyc_Parse_arg_array2ptr(
struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple11*
_temp132=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp132->f1=(*
t).f2; _temp132->f2=(* t).f3; _temp132;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp133=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp159;
int _temp161; struct Cyc_Absyn_Exp* _temp163; struct Cyc_Core_Opt* _temp165;
struct Cyc_Core_Opt* _temp167; struct Cyc_Absyn_Pat* _temp169; struct Cyc_List_List*
_temp171; struct Cyc_Absyn_Fndecl* _temp173; struct Cyc_Absyn_Structdecl*
_temp175; struct Cyc_Absyn_Uniondecl* _temp177; struct Cyc_Absyn_Tuniondecl*
_temp179; struct Cyc_Absyn_Typedefdecl* _temp181; struct Cyc_Absyn_Enumdecl*
_temp183; struct Cyc_List_List* _temp185; struct _tagged_string* _temp187;
struct Cyc_List_List* _temp189; struct _tuple1* _temp191; struct Cyc_List_List*
_temp193; _LL135: if(*(( int*) _temp133) == Cyc_Absyn_Var_d){ _LL160: _temp159=((
struct Cyc_Absyn_Var_d_struct*) _temp133)->f1; goto _LL136;} else{ goto _LL137;}
_LL137: if(*(( int*) _temp133) == Cyc_Absyn_Let_d){ _LL170: _temp169=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f1; goto _LL168; _LL168: _temp167=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f2; goto _LL166; _LL166: _temp165=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f3; goto _LL164; _LL164: _temp163=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f4; goto _LL162; _LL162: _temp161=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f5; goto _LL138;} else{ goto _LL139;} _LL139:
if(*(( int*) _temp133) == Cyc_Absyn_Letv_d){ _LL172: _temp171=(( struct Cyc_Absyn_Letv_d_struct*)
_temp133)->f1; goto _LL140;} else{ goto _LL141;} _LL141: if(*(( int*) _temp133)
== Cyc_Absyn_Fn_d){ _LL174: _temp173=(( struct Cyc_Absyn_Fn_d_struct*) _temp133)->f1;
goto _LL142;} else{ goto _LL143;} _LL143: if(*(( int*) _temp133) == Cyc_Absyn_Struct_d){
_LL176: _temp175=(( struct Cyc_Absyn_Struct_d_struct*) _temp133)->f1; goto
_LL144;} else{ goto _LL145;} _LL145: if(*(( int*) _temp133) == Cyc_Absyn_Union_d){
_LL178: _temp177=(( struct Cyc_Absyn_Union_d_struct*) _temp133)->f1; goto _LL146;}
else{ goto _LL147;} _LL147: if(*(( int*) _temp133) == Cyc_Absyn_Tunion_d){
_LL180: _temp179=(( struct Cyc_Absyn_Tunion_d_struct*) _temp133)->f1; goto
_LL148;} else{ goto _LL149;} _LL149: if(*(( int*) _temp133) == Cyc_Absyn_Typedef_d){
_LL182: _temp181=(( struct Cyc_Absyn_Typedef_d_struct*) _temp133)->f1; goto
_LL150;} else{ goto _LL151;} _LL151: if(*(( int*) _temp133) == Cyc_Absyn_Enum_d){
_LL184: _temp183=(( struct Cyc_Absyn_Enum_d_struct*) _temp133)->f1; goto _LL152;}
else{ goto _LL153;} _LL153: if(*(( int*) _temp133) == Cyc_Absyn_Namespace_d){
_LL188: _temp187=(( struct Cyc_Absyn_Namespace_d_struct*) _temp133)->f1; goto
_LL186; _LL186: _temp185=(( struct Cyc_Absyn_Namespace_d_struct*) _temp133)->f2;
goto _LL154;} else{ goto _LL155;} _LL155: if(*(( int*) _temp133) == Cyc_Absyn_Using_d){
_LL192: _temp191=(( struct Cyc_Absyn_Using_d_struct*) _temp133)->f1; goto _LL190;
_LL190: _temp189=(( struct Cyc_Absyn_Using_d_struct*) _temp133)->f2; goto _LL156;}
else{ goto _LL157;} _LL157: if(*(( int*) _temp133) == Cyc_Absyn_ExternC_d){
_LL194: _temp193=(( struct Cyc_Absyn_ExternC_d_struct*) _temp133)->f1; goto
_LL158;} else{ goto _LL134;} _LL136: if( _temp159->initializer != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp195="initializers are not allowed in parameter declarations";
struct _tagged_string _temp196; _temp196.curr= _temp195; _temp196.base= _temp195;
_temp196.last_plus_one= _temp195 + 55; _temp196;}), x->loc);}{ void* _temp197=(*
_temp159->name).f1; struct Cyc_List_List* _temp207; struct Cyc_List_List*
_temp209; _LL199: if( _temp197 ==( void*) Cyc_Absyn_Loc_n){ goto _LL200;} else{
goto _LL201;} _LL201: if(( unsigned int) _temp197 > 1u?*(( int*) _temp197) ==
Cyc_Absyn_Rel_n: 0){ _LL208: _temp207=(( struct Cyc_Absyn_Rel_n_struct*)
_temp197)->f1; if( _temp207 == 0){ goto _LL202;} else{ goto _LL203;}} else{ goto
_LL203;} _LL203: if(( unsigned int) _temp197 > 1u?*(( int*) _temp197) == Cyc_Absyn_Abs_n:
0){ _LL210: _temp209=(( struct Cyc_Absyn_Abs_n_struct*) _temp197)->f1; if(
_temp209 == 0){ goto _LL204;} else{ goto _LL205;}} else{ goto _LL205;} _LL205:
goto _LL206; _LL200: goto _LL198; _LL202: goto _LL198; _LL204: goto _LL198;
_LL206: Cyc_Parse_err(({ unsigned char* _temp211="module names not allowed on parameter declarations";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 51; _temp212;}), x->loc); goto _LL198; _LL198:;}{
int found= 0; for( 0; params != 0; params=(( struct Cyc_List_List*) _check_null(
params))->tl){ if( Cyc_String_zstrptrcmp((* _temp159->name).f2,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( params))->hd) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp213=*(*
_temp159->name).f2; xprintf("%.*s is not listed as a parameter", _temp213.last_plus_one
- _temp213.curr, _temp213.curr);}), x->loc);} return;} _LL138: decl_kind=({
unsigned char* _temp214="let declaration"; struct _tagged_string _temp215;
_temp215.curr= _temp214; _temp215.base= _temp214; _temp215.last_plus_one=
_temp214 + 16; _temp215;}); goto _LL134; _LL140: decl_kind=({ unsigned char*
_temp216="let declaration"; struct _tagged_string _temp217; _temp217.curr=
_temp216; _temp217.base= _temp216; _temp217.last_plus_one= _temp216 + 16;
_temp217;}); goto _LL134; _LL142: decl_kind=({ unsigned char* _temp218="function declaration";
struct _tagged_string _temp219; _temp219.curr= _temp218; _temp219.base= _temp218;
_temp219.last_plus_one= _temp218 + 21; _temp219;}); goto _LL134; _LL144:
decl_kind=({ unsigned char* _temp220="struct declaration"; struct _tagged_string
_temp221; _temp221.curr= _temp220; _temp221.base= _temp220; _temp221.last_plus_one=
_temp220 + 19; _temp221;}); goto _LL134; _LL146: decl_kind=({ unsigned char*
_temp222="union declaration"; struct _tagged_string _temp223; _temp223.curr=
_temp222; _temp223.base= _temp222; _temp223.last_plus_one= _temp222 + 18;
_temp223;}); goto _LL134; _LL148: decl_kind=({ unsigned char* _temp224="tunion declaration";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 19; _temp225;}); goto _LL134; _LL150:
decl_kind=({ unsigned char* _temp226="typedef"; struct _tagged_string _temp227;
_temp227.curr= _temp226; _temp227.base= _temp226; _temp227.last_plus_one=
_temp226 + 8; _temp227;}); goto _LL134; _LL152: decl_kind=({ unsigned char*
_temp228="enum declaration"; struct _tagged_string _temp229; _temp229.curr=
_temp228; _temp229.base= _temp228; _temp229.last_plus_one= _temp228 + 17;
_temp229;}); goto _LL134; _LL154: decl_kind=({ unsigned char* _temp230="namespace declaration";
struct _tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 22; _temp231;}); goto _LL134; _LL156:
decl_kind=({ unsigned char* _temp232="using declaration"; struct _tagged_string
_temp233; _temp233.curr= _temp232; _temp233.base= _temp232; _temp233.last_plus_one=
_temp232 + 18; _temp233;}); goto _LL134; _LL158: decl_kind=({ unsigned char*
_temp234="extern C declaration"; struct _tagged_string _temp235; _temp235.curr=
_temp234; _temp235.base= _temp234; _temp235.last_plus_one= _temp234 + 21;
_temp235;}); goto _LL134; _LL134:;}(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp236=
decl_kind; xprintf("%.*s appears in parameter type", _temp236.last_plus_one -
_temp236.curr, _temp236.curr);}), x->loc); return;} struct _tuple12{ struct Cyc_List_List*
f1; struct Cyc_Position_Segment* f2; } ; static struct _tuple2* Cyc_Parse_get_param_type(
struct _tuple12* env, struct _tagged_string* x){ struct _tuple12 _temp239;
struct Cyc_Position_Segment* _temp240; struct Cyc_List_List* _temp242; struct
_tuple12* _temp237= env; _temp239=* _temp237; _LL243: _temp242= _temp239.f1;
goto _LL241; _LL241: _temp240= _temp239.f2; goto _LL238; _LL238: if( _temp242 ==
0){ return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp244=* x; xprintf("missing type for parameter %.*s",
_temp244.last_plus_one - _temp244.curr, _temp244.curr);}), _temp240);}{ void*
_temp245=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null(
_temp242))->hd)->r; struct Cyc_Absyn_Vardecl* _temp251; _LL247: if(*(( int*)
_temp245) == Cyc_Absyn_Var_d){ _LL252: _temp251=(( struct Cyc_Absyn_Var_d_struct*)
_temp245)->f1; goto _LL248;} else{ goto _LL249;} _LL249: goto _LL250; _LL248:{
void* _temp253=(* _temp251->name).f1; struct Cyc_List_List* _temp263; struct Cyc_List_List*
_temp265; _LL255: if( _temp253 ==( void*) Cyc_Absyn_Loc_n){ goto _LL256;} else{
goto _LL257;} _LL257: if(( unsigned int) _temp253 > 1u?*(( int*) _temp253) ==
Cyc_Absyn_Rel_n: 0){ _LL264: _temp263=(( struct Cyc_Absyn_Rel_n_struct*)
_temp253)->f1; if( _temp263 == 0){ goto _LL258;} else{ goto _LL259;}} else{ goto
_LL259;} _LL259: if(( unsigned int) _temp253 > 1u?*(( int*) _temp253) == Cyc_Absyn_Abs_n:
0){ _LL266: _temp265=(( struct Cyc_Absyn_Abs_n_struct*) _temp253)->f1; if(
_temp265 == 0){ goto _LL260;} else{ goto _LL261;}} else{ goto _LL261;} _LL261:
goto _LL262; _LL256: goto _LL254; _LL258: goto _LL254; _LL260: goto _LL254;
_LL262: Cyc_Parse_err(({ unsigned char* _temp267="module name not allowed on parameter";
struct _tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 37; _temp268;}), _temp240); goto _LL254;
_LL254:;} if( Cyc_String_zstrptrcmp((* _temp251->name).f2, x) == 0){ return({
struct _tuple2* _temp269=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp269[ 0]=({ struct _tuple2 _temp270; _temp270.f1=({ struct Cyc_Core_Opt*
_temp271=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp271[ 0]=({ struct Cyc_Core_Opt _temp272; _temp272.v=( void*)(* _temp251->name).f2;
_temp272;}); _temp271;}); _temp270.f2= _temp251->tq; _temp270.f3=( void*)
_temp251->type; _temp270;}); _temp269;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple12* _temp273=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp273->f1=(( struct Cyc_List_List*) _check_null( _temp242))->tl; _temp273->f2=
_temp240; _temp273;}), x);} _LL250: return(( struct _tuple2*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp274="non-variable declaration"; struct _tagged_string
_temp275; _temp275.curr= _temp274; _temp275.base= _temp274; _temp275.last_plus_one=
_temp274 + 25; _temp275;}),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp242))->hd)->loc); _LL246:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp276= tm; int _temp282; struct Cyc_Position_Segment*
_temp284; struct Cyc_List_List* _temp286; _LL278: if(( unsigned int) _temp276 >
1u?*(( int*) _temp276) == Cyc_Absyn_TypeParams_mod: 0){ _LL287: _temp286=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f1; goto _LL285; _LL285:
_temp284=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f2; goto _LL283;
_LL283: _temp282=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f3; goto
_LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279: return 1; _LL281:
return 0; _LL277:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,({ unsigned char*
_temp288="`H"; struct _tagged_string _temp289; _temp289.curr= _temp288; _temp289.base=
_temp288; _temp289.last_plus_one= _temp288 + 3; _temp289;})) == 0){ return( void*)
Cyc_Absyn_HeapRgn;} else{ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp290=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp290[ 0]=({ struct Cyc_Absyn_VarType_struct _temp291; _temp291.tag= Cyc_Absyn_VarType;
_temp291.f1=({ struct Cyc_Absyn_Tvar* _temp292=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp292->name=({ struct
_tagged_string* _temp293=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp293[ 0]= s; _temp293;}); _temp292->identity= 0;
_temp292->kind= k; _temp292;}); _temp291;}); _temp290;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp294=
t; struct Cyc_Absyn_Tvar* _temp300; _LL296: if(( unsigned int) _temp294 > 4u?*((
int*) _temp294) == Cyc_Absyn_VarType: 0){ _LL301: _temp300=(( struct Cyc_Absyn_VarType_struct*)
_temp294)->f1; goto _LL297;} else{ goto _LL298;} _LL298: goto _LL299; _LL297:
return _temp300; _LL299: return(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp302="expecting a list of type variables, not types"; struct
_tagged_string _temp303; _temp303.curr= _temp302; _temp303.base= _temp302;
_temp303.last_plus_one= _temp302 + 46; _temp303;}), loc); _LL295:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp304=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp304[ 0]=({ struct Cyc_Absyn_VarType_struct _temp305; _temp305.tag= Cyc_Absyn_VarType;
_temp305.f1= pr; _temp305;}); _temp304;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp306=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp312; _LL308:
if(( unsigned int) _temp306 > 1u?*(( int*) _temp306) == Cyc_Absyn_Function_mod:
0){ _LL313: _temp312=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp306)->f1;
goto _LL309;} else{ goto _LL310;} _LL310: goto _LL311; _LL309: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp314= _temp312; struct Cyc_Core_Opt* _temp320; struct Cyc_Absyn_VarargInfo*
_temp322; int _temp324; struct Cyc_List_List* _temp326; struct Cyc_Position_Segment*
_temp328; struct Cyc_List_List* _temp330; _LL316: if(*(( int*) _temp314) == Cyc_Absyn_WithTypes){
_LL327: _temp326=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f1; goto
_LL325; _LL325: _temp324=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f2;
goto _LL323; _LL323: _temp322=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f3;
goto _LL321; _LL321: _temp320=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f4;
goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp314) == Cyc_Absyn_NoTypes){
_LL331: _temp330=(( struct Cyc_Absyn_NoTypes_struct*) _temp314)->f1; goto _LL329;
_LL329: _temp328=(( struct Cyc_Absyn_NoTypes_struct*) _temp314)->f2; goto _LL319;}
else{ goto _LL315;} _LL317: Cyc_Parse_warn(({ unsigned char* _temp332="function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 92; _temp333;}), loc); return tms; _LL319:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp330, tds);{ struct _tuple12* _temp335=({ struct _tuple12* _temp334=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp334->f1= tds; _temp334->f2=
loc; _temp334;}); goto _LL336; _LL336: return({ struct Cyc_List_List* _temp337=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp337->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp338=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp338[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp339; _temp339.tag= Cyc_Absyn_Function_mod;
_temp339.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp340=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp340[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp341; _temp341.tag= Cyc_Absyn_WithTypes;
_temp341.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_string*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp335, _temp330); _temp341.f2= 0; _temp341.f3= 0;
_temp341.f4= 0; _temp341;}); _temp340;})); _temp339;}); _temp338;})); _temp337->tl=
0; _temp337;});} _LL315:;} else{ return({ struct Cyc_List_List* _temp342=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp342->hd=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd); _temp342->tl=
Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null( tms))->tl,
tds, loc); _temp342;});} _LL311: return({ struct Cyc_List_List* _temp343=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp343->hd=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd); _temp343->tl=
Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null( tms))->tl,
tds, loc); _temp343;}); _LL307:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds !=
0){ d=({ struct Cyc_Parse_Declarator* _temp344=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp344->id= d->id; _temp344->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp344;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp345=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL347: if( _temp345 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL348;} else{ goto _LL349;} _LL349: if(
_temp345 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL350;} else{ goto _LL351;}
_LL351: if( _temp345 ==( void*) Cyc_Parse_Static_sc){ goto _LL352;} else{ goto
_LL353;} _LL353: goto _LL354; _LL348: sc=( void*) Cyc_Absyn_Extern; goto _LL346;
_LL350: sc=( void*) Cyc_Absyn_ExternC; goto _LL346; _LL352: sc=( void*) Cyc_Absyn_Static;
goto _LL346; _LL354: Cyc_Parse_err(({ unsigned char* _temp355="bad storage class on function";
struct _tagged_string _temp356; _temp356.curr= _temp355; _temp356.base= _temp355;
_temp356.last_plus_one= _temp355 + 30; _temp356;}), loc); goto _LL346; _LL346:;}}{
struct Cyc_Core_Opt* _temp359; void* _temp361; struct _tuple5 _temp357= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL362: _temp361= _temp357.f1; goto _LL360; _LL360: _temp359=
_temp357.f2; goto _LL358; _LL358: { struct Cyc_List_List* _temp365; struct Cyc_List_List*
_temp367; void* _temp369; struct Cyc_Absyn_Tqual _temp371; struct _tuple6
_temp363= Cyc_Parse_apply_tms( tq, _temp361, atts, d->tms); _LL372: _temp371=
_temp363.f1; goto _LL370; _LL370: _temp369= _temp363.f2; goto _LL368; _LL368:
_temp367= _temp363.f3; goto _LL366; _LL366: _temp365= _temp363.f4; goto _LL364;
_LL364: if( _temp359 != 0){ Cyc_Parse_warn(({ unsigned char* _temp373="nested type declaration within function prototype";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 50; _temp374;}), loc);} if( _temp367 != 0){
Cyc_Parse_warn(({ unsigned char* _temp375="bad type params, ignoring"; struct
_tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 26; _temp376;}), loc);}{ void* _temp377=
_temp369; struct Cyc_Absyn_FnInfo _temp383; struct Cyc_List_List* _temp385;
struct Cyc_Absyn_VarargInfo* _temp387; int _temp389; struct Cyc_List_List*
_temp391; void* _temp393; struct Cyc_Core_Opt* _temp395; struct Cyc_List_List*
_temp397; _LL379: if(( unsigned int) _temp377 > 4u?*(( int*) _temp377) == Cyc_Absyn_FnType:
0){ _LL384: _temp383=(( struct Cyc_Absyn_FnType_struct*) _temp377)->f1; _LL398:
_temp397= _temp383.tvars; goto _LL396; _LL396: _temp395= _temp383.effect; goto
_LL394; _LL394: _temp393=( void*) _temp383.ret_typ; goto _LL392; _LL392:
_temp391= _temp383.args; goto _LL390; _LL390: _temp389= _temp383.c_varargs; goto
_LL388; _LL388: _temp387= _temp383.cyc_varargs; goto _LL386; _LL386: _temp385=
_temp383.attributes; goto _LL380;} else{ goto _LL381;} _LL381: goto _LL382;
_LL380: { struct Cyc_List_List* _temp399=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc,
_temp391); goto _LL400; _LL400: return({ struct Cyc_Absyn_Fndecl* _temp401=(
struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl)); _temp401->sc=(
void*) sc; _temp401->name= d->id; _temp401->tvs= _temp397; _temp401->is_inline=
is_inline; _temp401->effect= _temp395; _temp401->ret_type=( void*) _temp393;
_temp401->args= _temp399; _temp401->c_varargs= _temp389; _temp401->cyc_varargs=
_temp387; _temp401->body= body; _temp401->cached_typ= 0; _temp401->param_vardecls=
0; _temp401->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp385, _temp365); _temp401;});}
_LL382: return(( struct Cyc_Absyn_Fndecl*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp402="declarator is not a function prototype";
struct _tagged_string _temp403; _temp403.curr= _temp402; _temp403.base= _temp402;
_temp403.last_plus_one= _temp402 + 39; _temp403;}), loc); _LL378:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(({ unsigned char* _temp404="missing argument variable in function prototype";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 48; _temp405;}), loc); return({ struct
_tuple4* _temp406=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp406->f1=({ struct _tagged_string* _temp407=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp407[ 0]=({ unsigned char*
_temp408="?"; struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base=
_temp408; _temp409.last_plus_one= _temp408 + 2; _temp409;}); _temp407;});
_temp406->f2=(* t).f2; _temp406->f3=(* t).f3; _temp406;});} else{ return({
struct _tuple4* _temp410=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp410->f1=( struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp410->f2=(* t).f2; _temp410->f3=(* t).f3; _temp410;});}} static
unsigned char _temp413[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1={ _temp413, _temp413, _temp413 + 52u};
static unsigned char _temp416[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2={ _temp416, _temp416, _temp416 + 63u};
static unsigned char _temp419[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3={ _temp419, _temp419, _temp419 + 50u};
static unsigned char _temp422[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4={ _temp422, _temp422, _temp422 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp423=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp437; void* _temp439; struct Cyc_Position_Segment*
_temp441; struct Cyc_Position_Segment* _temp443; struct Cyc_Position_Segment*
_temp445; struct Cyc_Position_Segment* _temp447; struct Cyc_Absyn_Decl* _temp449;
_LL425: if(*(( int*) _temp423) == Cyc_Parse_Type_spec){ _LL440: _temp439=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp423)->f1; goto _LL438; _LL438: _temp437=((
struct Cyc_Parse_Type_spec_struct*) _temp423)->f2; goto _LL426;} else{ goto
_LL427;} _LL427: if(*(( int*) _temp423) == Cyc_Parse_Signed_spec){ _LL442:
_temp441=(( struct Cyc_Parse_Signed_spec_struct*) _temp423)->f1; goto _LL428;}
else{ goto _LL429;} _LL429: if(*(( int*) _temp423) == Cyc_Parse_Unsigned_spec){
_LL444: _temp443=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp423)->f1; goto
_LL430;} else{ goto _LL431;} _LL431: if(*(( int*) _temp423) == Cyc_Parse_Short_spec){
_LL446: _temp445=(( struct Cyc_Parse_Short_spec_struct*) _temp423)->f1; goto
_LL432;} else{ goto _LL433;} _LL433: if(*(( int*) _temp423) == Cyc_Parse_Long_spec){
_LL448: _temp447=(( struct Cyc_Parse_Long_spec_struct*) _temp423)->f1; goto
_LL434;} else{ goto _LL435;} _LL435: if(*(( int*) _temp423) == Cyc_Parse_Decl_spec){
_LL450: _temp449=(( struct Cyc_Parse_Decl_spec_struct*) _temp423)->f1; goto
_LL436;} else{ goto _LL424;} _LL426: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp437);} last_loc= _temp437; seen_type= 1; t= _temp439; goto _LL424; _LL428:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp441);} if( seen_type){ Cyc_Parse_err(({
unsigned char* _temp451="signed qualifier must come before type"; struct
_tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 39; _temp452;}), _temp441);} last_loc=
_temp441; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL424; _LL430: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp443);} if( seen_type){ Cyc_Parse_err(({
unsigned char* _temp453="signed qualifier must come before type"; struct
_tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 39; _temp454;}), _temp443);} last_loc=
_temp443; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL424; _LL432:
if( seen_size){ Cyc_Parse_err(({ unsigned char* _temp455="integral size may appear only once within a type specifier";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 59; _temp456;}), _temp445);} if( seen_type){
Cyc_Parse_err(({ unsigned char* _temp457="short modifier must come before base type";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 42; _temp458;}), _temp445);} last_loc=
_temp445; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL424; _LL434: if(
seen_type){ Cyc_Parse_err(({ unsigned char* _temp459="long modifier must come before base type";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 41; _temp460;}), _temp447);} if( seen_size){
void* _temp461= sz; _LL463: if( _temp461 ==( void*) Cyc_Absyn_B4){ goto _LL464;}
else{ goto _LL465;} _LL465: goto _LL466; _LL464: sz=( void*) Cyc_Absyn_B8; goto
_LL462; _LL466: Cyc_Parse_err(({ unsigned char* _temp467="extra long in type specifier";
struct _tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 29; _temp468;}), _temp447); goto _LL462;
_LL462:;} last_loc= _temp447; seen_size= 1; goto _LL424; _LL436: last_loc=
_temp449->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp469=(
void*) _temp449->r; struct Cyc_Absyn_Structdecl* _temp481; struct Cyc_Absyn_Tuniondecl*
_temp483; struct Cyc_Absyn_Uniondecl* _temp485; struct Cyc_Absyn_Enumdecl*
_temp487; _LL471: if(*(( int*) _temp469) == Cyc_Absyn_Struct_d){ _LL482:
_temp481=(( struct Cyc_Absyn_Struct_d_struct*) _temp469)->f1; goto _LL472;}
else{ goto _LL473;} _LL473: if(*(( int*) _temp469) == Cyc_Absyn_Tunion_d){
_LL484: _temp483=(( struct Cyc_Absyn_Tunion_d_struct*) _temp469)->f1; goto
_LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp469) == Cyc_Absyn_Union_d){
_LL486: _temp485=(( struct Cyc_Absyn_Union_d_struct*) _temp469)->f1; goto _LL476;}
else{ goto _LL477;} _LL477: if(*(( int*) _temp469) == Cyc_Absyn_Enum_d){ _LL488:
_temp487=(( struct Cyc_Absyn_Enum_d_struct*) _temp469)->f1; goto _LL478;} else{
goto _LL479;} _LL479: goto _LL480; _LL472: { struct Cyc_List_List* _temp489=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp481->tvs); goto _LL490; _LL490: t=(
void*)({ struct Cyc_Absyn_StructType_struct* _temp491=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp491[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp492; _temp492.tag= Cyc_Absyn_StructType;
_temp492.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp481->name))->v); _temp492.f2= _temp489; _temp492.f3= 0;
_temp492;}); _temp491;}); if( _temp481->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp493=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp493->v=( void*) _temp449; _temp493;});} goto _LL470;} _LL474: { struct Cyc_List_List*
_temp494=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp483->tvs);
goto _LL495; _LL495: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp496=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp496[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp497; _temp497.tag= Cyc_Absyn_TunionType;
_temp497.f1=({ struct Cyc_Absyn_TunionInfo _temp498; _temp498.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp499=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp499[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp500; _temp500.tag= Cyc_Absyn_KnownTunion;
_temp500.f1= _temp483; _temp500;}); _temp499;})); _temp498.targs= _temp494;
_temp498.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp498;}); _temp497;});
_temp496;}); if( _temp483->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp501=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp501->v=( void*) _temp449; _temp501;});} goto _LL470;} _LL476: { struct Cyc_List_List*
_temp502=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp485->tvs);
goto _LL503; _LL503: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp504=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp504[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp505; _temp505.tag= Cyc_Absyn_UnionType;
_temp505.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp485->name))->v); _temp505.f2= _temp502; _temp505.f3= 0;
_temp505;}); _temp504;}); if( _temp485->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp506=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp506->v=( void*) _temp449; _temp506;});} goto _LL470;} _LL478: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp507=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp507[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp508; _temp508.tag= Cyc_Absyn_EnumType; _temp508.f1=
_temp487->name; _temp508.f2= 0; _temp508;}); _temp507;}); declopt=({ struct Cyc_Core_Opt*
_temp509=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp509->v=( void*) _temp449; _temp509;}); goto _LL470; _LL480:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp510="bad declaration within type specifier"; struct
_tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 38; _temp511;}), _temp449->loc); goto _LL470;
_LL470:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp449->loc);} goto _LL424;
_LL424:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err(({
unsigned char* _temp512="missing type withing specifier"; struct _tagged_string
_temp513; _temp513.curr= _temp512; _temp513.base= _temp512; _temp513.last_plus_one=
_temp512 + 31; _temp513;}), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp514=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp514[ 0]=({ struct Cyc_Absyn_IntType_struct _temp515; _temp515.tag= Cyc_Absyn_IntType;
_temp515.f1=( void*) sgn; _temp515.f2=( void*) sz; _temp515;}); _temp514;});}
else{ if( seen_sign){ void* _temp516= t; void* _temp522; void* _temp524; _LL518:
if(( unsigned int) _temp516 > 4u?*(( int*) _temp516) == Cyc_Absyn_IntType: 0){
_LL525: _temp524=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp516)->f1;
goto _LL523; _LL523: _temp522=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp516)->f2; goto _LL519;} else{ goto _LL520;} _LL520: goto _LL521; _LL519: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp526=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp526[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp527; _temp527.tag= Cyc_Absyn_IntType; _temp527.f1=( void*) sgn; _temp527.f2=(
void*) _temp522; _temp527;}); _temp526;}); goto _LL517; _LL521: Cyc_Parse_err(({
unsigned char* _temp528="sign specification on non-integral type"; struct
_tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 40; _temp529;}), last_loc); goto _LL517;
_LL517:;} if( seen_size){ void* _temp530= t; void* _temp536; void* _temp538;
_LL532: if(( unsigned int) _temp530 > 4u?*(( int*) _temp530) == Cyc_Absyn_IntType:
0){ _LL539: _temp538=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp530)->f1;
goto _LL537; _LL537: _temp536=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp530)->f2; goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL533: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp540=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp540[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp541; _temp541.tag= Cyc_Absyn_IntType; _temp541.f1=( void*) _temp538;
_temp541.f2=( void*) sz; _temp541;}); _temp540;}); goto _LL531; _LL535: Cyc_Parse_err(({
unsigned char* _temp542="size qualifier on non-integral type"; struct
_tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
_temp543.last_plus_one= _temp542 + 36; _temp543;}), last_loc); goto _LL531;
_LL531:;}} return({ struct _tuple5 _temp544; _temp544.f1= t; _temp544.f2=
declopt; _temp544;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp545=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL546; _LL546: { struct _tuple1* _temp547= _temp545->id; goto _LL548;
_LL548: { struct Cyc_List_List* _temp551; struct Cyc_List_List* _temp553; void*
_temp555; struct Cyc_Absyn_Tqual _temp557; struct _tuple6 _temp549= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp545->tms); _LL558: _temp557= _temp549.f1; goto _LL556;
_LL556: _temp555= _temp549.f2; goto _LL554; _LL554: _temp553= _temp549.f3; goto
_LL552; _LL552: _temp551= _temp549.f4; goto _LL550; _LL550: return({ struct Cyc_List_List*
_temp559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp559->hd=( void*)({ struct _tuple7* _temp560=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp560->f1= _temp547; _temp560->f2= _temp557;
_temp560->f3= _temp555; _temp560->f4= _temp553; _temp560->f5= _temp551; _temp560;});
_temp559->tl= Cyc_Parse_apply_tmss( _temp557, t,(( struct Cyc_List_List*)
_check_null( ds))->tl, shared_atts); _temp559;});}}}} static struct _tuple6 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp561; _temp561.f1= tq; _temp561.f2=
t; _temp561.f3= 0; _temp561.f4= atts; _temp561;});}{ void* _temp562=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp576;
void* _temp578; int _temp580; struct Cyc_Position_Segment* _temp582; struct Cyc_List_List*
_temp584; struct Cyc_Absyn_Tqual _temp586; void* _temp588; void* _temp590;
struct Cyc_List_List* _temp592; struct Cyc_Position_Segment* _temp594; _LL564:
if( _temp562 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL565;} else{ goto _LL566;}
_LL566: if(( unsigned int) _temp562 > 1u?*(( int*) _temp562) == Cyc_Absyn_ConstArray_mod:
0){ _LL577: _temp576=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp562)->f1;
goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int) _temp562 > 1u?*((
int*) _temp562) == Cyc_Absyn_Function_mod: 0){ _LL579: _temp578=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp562)->f1; goto _LL569;} else{ goto
_LL570;} _LL570: if(( unsigned int) _temp562 > 1u?*(( int*) _temp562) == Cyc_Absyn_TypeParams_mod:
0){ _LL585: _temp584=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp562)->f1;
goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp562)->f2; goto _LL581; _LL581: _temp580=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp562)->f3; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp562 > 1u?*(( int*) _temp562) == Cyc_Absyn_Pointer_mod: 0){ _LL591: _temp590=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp562)->f1; goto _LL589;
_LL589: _temp588=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp562)->f2;
goto _LL587; _LL587: _temp586=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp562)->f3;
goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int) _temp562 > 1u?*((
int*) _temp562) == Cyc_Absyn_Attributes_mod: 0){ _LL595: _temp594=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp562)->f1; goto _LL593; _LL593: _temp592=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp562)->f2; goto _LL575;} else{ goto _LL563;} _LL565: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp596=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp596[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp597; _temp597.tag= Cyc_Absyn_ArrayType;
_temp597.f1=( void*) t; _temp597.f2= tq; _temp597.f3= 0; _temp597;}); _temp596;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL567: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp598=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp598[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp599; _temp599.tag= Cyc_Absyn_ArrayType;
_temp599.f1=( void*) t; _temp599.f2= tq; _temp599.f3=( struct Cyc_Absyn_Exp*)
_temp576; _temp599;}); _temp598;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL569: { void* _temp600= _temp578; struct Cyc_Core_Opt* _temp606;
struct Cyc_Absyn_VarargInfo* _temp608; int _temp610; struct Cyc_List_List*
_temp612; struct Cyc_Position_Segment* _temp614; struct Cyc_List_List* _temp616;
_LL602: if(*(( int*) _temp600) == Cyc_Absyn_WithTypes){ _LL613: _temp612=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f1; goto _LL611; _LL611: _temp610=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f2; goto _LL609; _LL609: _temp608=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f3; goto _LL607; _LL607: _temp606=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f4; goto _LL603;} else{ goto
_LL604;} _LL604: if(*(( int*) _temp600) == Cyc_Absyn_NoTypes){ _LL617: _temp616=((
struct Cyc_Absyn_NoTypes_struct*) _temp600)->f1; goto _LL615; _LL615: _temp614=((
struct Cyc_Absyn_NoTypes_struct*) _temp600)->f2; goto _LL605;} else{ goto _LL601;}
_LL603: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp618=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp618->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp618->tl= fn_atts; _temp618;});} else{ new_atts=({ struct Cyc_List_List*
_temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp619->tl= new_atts; _temp619;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp620=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; int _temp626; struct Cyc_Position_Segment*
_temp628; struct Cyc_List_List* _temp630; _LL622: if(( unsigned int) _temp620 >
1u?*(( int*) _temp620) == Cyc_Absyn_TypeParams_mod: 0){ _LL631: _temp630=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp620)->f1; goto _LL629; _LL629:
_temp628=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp620)->f2; goto _LL627;
_LL627: _temp626=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp620)->f3; goto
_LL623;} else{ goto _LL624;} _LL624: goto _LL625; _LL623: typvars= _temp630; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL621; _LL625: goto _LL621;
_LL621:;} if((((( ! _temp610? _temp608 == 0: 0)? _temp612 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp612))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp612))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp612))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp612= 0;}
t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp612); return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ( typvars, _temp606, t, _temp612,
_temp610, _temp608, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null(
tms))->tl);} _LL605:( void) _throw((( void*(*)( struct _tagged_string msg,
struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp632="function declaration without parameter types";
struct _tagged_string _temp633; _temp633.curr= _temp632; _temp633.base= _temp632;
_temp633.last_plus_one= _temp632 + 45; _temp633;}), _temp614)); _LL601:;} _LL571:
if((( struct Cyc_List_List*) _check_null( tms))->tl == 0){ return({ struct
_tuple6 _temp634; _temp634.f1= tq; _temp634.f2= t; _temp634.f3= _temp584;
_temp634.f4= atts; _temp634;});}( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp635="type parameters must appear before function arguments in declarator";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 68; _temp636;}), _temp582)); _LL573: { void*
_temp637= _temp590; struct Cyc_Absyn_Exp* _temp645; struct Cyc_Absyn_Exp*
_temp647; _LL639: if(( unsigned int) _temp637 > 1u?*(( int*) _temp637) == Cyc_Absyn_NonNullable_ps:
0){ _LL646: _temp645=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp637)->f1;
goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int) _temp637 > 1u?*((
int*) _temp637) == Cyc_Absyn_Nullable_ps: 0){ _LL648: _temp647=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp637)->f1; goto _LL642;} else{ goto _LL643;} _LL643: if( _temp637 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL644;} else{ goto _LL638;} _LL640: return Cyc_Parse_apply_tms(
_temp586, Cyc_Absyn_atb_typ( t, _temp588, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp649=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp649[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp650; _temp650.tag= Cyc_Absyn_Upper_b;
_temp650.f1= _temp645; _temp650;}); _temp649;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL642: return Cyc_Parse_apply_tms( _temp586, Cyc_Absyn_starb_typ(
t, _temp588, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp651=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp651[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp652; _temp652.tag= Cyc_Absyn_Upper_b;
_temp652.f1= _temp647; _temp652;}); _temp651;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL644: return Cyc_Parse_apply_tms( _temp586, Cyc_Absyn_tagged_typ(
t, _temp588, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL638:;}
_LL575: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp592),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL563:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp655; void* _temp657; struct _tuple5 _temp653= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL658: _temp657= _temp653.f1; goto _LL656; _LL656: _temp655=
_temp653.f2; goto _LL654; _LL654: if( _temp655 != 0){ Cyc_Parse_warn(({
unsigned char* _temp659="ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp660; _temp660.curr= _temp659; _temp660.base= _temp659;
_temp660.last_plus_one= _temp659 + 54; _temp660;}), loc);} return _temp657;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp661=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp661[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp662; _temp662.tag= Cyc_Absyn_Decl_s;
_temp662.f1= d; _temp662.f2= s; _temp662;}); _temp661;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(({ unsigned char* _temp663="inline is only allowed on function definitions";
struct _tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 47; _temp664;}), loc);} if( tss == 0){ Cyc_Parse_err(({
unsigned char* _temp665="missing type specifiers in declaration"; struct
_tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 39; _temp666;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp667=( void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v;
_LL669: if( _temp667 ==( void*) Cyc_Parse_Typedef_sc){ goto _LL670;} else{ goto
_LL671;} _LL671: if( _temp667 ==( void*) Cyc_Parse_Extern_sc){ goto _LL672;}
else{ goto _LL673;} _LL673: if( _temp667 ==( void*) Cyc_Parse_ExternC_sc){ goto
_LL674;} else{ goto _LL675;} _LL675: if( _temp667 ==( void*) Cyc_Parse_Static_sc){
goto _LL676;} else{ goto _LL677;} _LL677: if( _temp667 ==( void*) Cyc_Parse_Auto_sc){
goto _LL678;} else{ goto _LL679;} _LL679: if( _temp667 ==( void*) Cyc_Parse_Register_sc){
goto _LL680;} else{ goto _LL681;} _LL681: if( _temp667 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL682;} else{ goto _LL668;} _LL670: istypedef= 1; goto _LL668; _LL672: s=(
void*) Cyc_Absyn_Extern; goto _LL668; _LL674: s=( void*) Cyc_Absyn_ExternC; goto
_LL668; _LL676: s=( void*) Cyc_Absyn_Static; goto _LL668; _LL678: s=( void*) Cyc_Absyn_Public;
goto _LL668; _LL680: s=( void*) Cyc_Absyn_Public; goto _LL668; _LL682: s=( void*)
Cyc_Absyn_Abstract; goto _LL668; _LL668:;}{ struct Cyc_List_List* _temp685;
struct Cyc_List_List* _temp687; struct _tuple0 _temp683=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL688: _temp687= _temp683.f1;
goto _LL686; _LL686: _temp685= _temp683.f2; goto _LL684; _LL684: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp685; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp689= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL690; _LL690: if( _temp687 == 0){ struct Cyc_Core_Opt* _temp693; void*
_temp695; struct _tuple5 _temp691= _temp689; _LL696: _temp695= _temp691.f1; goto
_LL694; _LL694: _temp693= _temp691.f2; goto _LL692; _LL692: if( _temp693 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp693))->v;{ void* _temp697=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp709; struct Cyc_Absyn_Structdecl* _temp711; struct Cyc_Absyn_Uniondecl*
_temp713; struct Cyc_Absyn_Tuniondecl* _temp715; _LL699: if(*(( int*) _temp697)
== Cyc_Absyn_Enum_d){ _LL710: _temp709=(( struct Cyc_Absyn_Enum_d_struct*)
_temp697)->f1; goto _LL700;} else{ goto _LL701;} _LL701: if(*(( int*) _temp697)
== Cyc_Absyn_Struct_d){ _LL712: _temp711=(( struct Cyc_Absyn_Struct_d_struct*)
_temp697)->f1; goto _LL702;} else{ goto _LL703;} _LL703: if(*(( int*) _temp697)
== Cyc_Absyn_Union_d){ _LL714: _temp713=(( struct Cyc_Absyn_Union_d_struct*)
_temp697)->f1; goto _LL704;} else{ goto _LL705;} _LL705: if(*(( int*) _temp697)
== Cyc_Absyn_Tunion_d){ _LL716: _temp715=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp697)->f1; goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL700:(
void*)( _temp709->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(({ unsigned char*
_temp717="bad attributes on enum"; struct _tagged_string _temp718; _temp718.curr=
_temp717; _temp718.base= _temp717; _temp718.last_plus_one= _temp717 + 23;
_temp718;}), loc);} goto _LL698; _LL702:( void*)( _temp711->sc=( void*) s);
_temp711->attributes= atts; goto _LL698; _LL704:( void*)( _temp713->sc=( void*)
s); _temp713->attributes= atts; goto _LL698; _LL706:( void*)( _temp715->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp719="bad attributes on tunion";
struct _tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 25; _temp720;}), loc);} goto _LL698; _LL708:
Cyc_Parse_err(({ unsigned char* _temp721="bad declaration"; struct
_tagged_string _temp722; _temp722.curr= _temp721; _temp722.base= _temp721;
_temp722.last_plus_one= _temp721 + 16; _temp722;}), loc); return 0; _LL698:;}
return({ struct Cyc_List_List* _temp723=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp723->hd=( void*) d; _temp723->tl= 0;
_temp723;});} else{ void* _temp724= _temp695; struct Cyc_Absyn_Structdecl**
_temp738; struct Cyc_List_List* _temp740; struct _tuple1* _temp742; struct Cyc_Absyn_TunionInfo
_temp744; void* _temp746; struct Cyc_List_List* _temp748; void* _temp750; struct
Cyc_Absyn_Tuniondecl* _temp752; struct Cyc_Absyn_TunionInfo _temp754; void*
_temp756; struct Cyc_List_List* _temp758; void* _temp760; struct Cyc_Absyn_UnknownTunionInfo
_temp762; int _temp764; struct _tuple1* _temp766; struct Cyc_Absyn_Uniondecl**
_temp768; struct Cyc_List_List* _temp770; struct _tuple1* _temp772; struct Cyc_Absyn_Enumdecl*
_temp774; struct _tuple1* _temp776; _LL726: if(( unsigned int) _temp724 > 4u?*((
int*) _temp724) == Cyc_Absyn_StructType: 0){ _LL743: _temp742=(( struct Cyc_Absyn_StructType_struct*)
_temp724)->f1; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_StructType_struct*)
_temp724)->f2; goto _LL739; _LL739: _temp738=(( struct Cyc_Absyn_StructType_struct*)
_temp724)->f3; goto _LL727;} else{ goto _LL728;} _LL728: if(( unsigned int)
_temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_TunionType: 0){ _LL745: _temp744=((
struct Cyc_Absyn_TunionType_struct*) _temp724)->f1; _LL751: _temp750=( void*)
_temp744.tunion_info; if(*(( int*) _temp750) == Cyc_Absyn_KnownTunion){ _LL753:
_temp752=(( struct Cyc_Absyn_KnownTunion_struct*) _temp750)->f1; goto _LL749;}
else{ goto _LL730;} _LL749: _temp748= _temp744.targs; goto _LL747; _LL747:
_temp746=( void*) _temp744.rgn; goto _LL729;} else{ goto _LL730;} _LL730: if((
unsigned int) _temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_TunionType: 0){
_LL755: _temp754=(( struct Cyc_Absyn_TunionType_struct*) _temp724)->f1; _LL761:
_temp760=( void*) _temp754.tunion_info; if(*(( int*) _temp760) == Cyc_Absyn_UnknownTunion){
_LL763: _temp762=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp760)->f1;
_LL767: _temp766= _temp762.name; goto _LL765; _LL765: _temp764= _temp762.is_xtunion;
goto _LL759;} else{ goto _LL732;} _LL759: _temp758= _temp754.targs; goto _LL757;
_LL757: _temp756=( void*) _temp754.rgn; goto _LL731;} else{ goto _LL732;} _LL732:
if(( unsigned int) _temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_UnionType: 0){
_LL773: _temp772=(( struct Cyc_Absyn_UnionType_struct*) _temp724)->f1; goto
_LL771; _LL771: _temp770=(( struct Cyc_Absyn_UnionType_struct*) _temp724)->f2;
goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_UnionType_struct*) _temp724)->f3;
goto _LL733;} else{ goto _LL734;} _LL734: if(( unsigned int) _temp724 > 4u?*((
int*) _temp724) == Cyc_Absyn_EnumType: 0){ _LL777: _temp776=(( struct Cyc_Absyn_EnumType_struct*)
_temp724)->f1; goto _LL775; _LL775: _temp774=(( struct Cyc_Absyn_EnumType_struct*)
_temp724)->f2; goto _LL735;} else{ goto _LL736;} _LL736: goto _LL737; _LL727: {
struct Cyc_List_List* _temp778=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp740);
goto _LL779; _LL779: { struct Cyc_Absyn_Structdecl* _temp782=({ struct Cyc_Absyn_Structdecl*
_temp780=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp780->sc=( void*) s; _temp780->name=({ struct Cyc_Core_Opt* _temp781=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp781->v=(
void*)(( struct _tuple1*) _check_null( _temp742)); _temp781;}); _temp780->tvs=
_temp778; _temp780->fields= 0; _temp780->attributes= 0; _temp780;}); goto _LL783;
_LL783: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp784="bad attributes on struct";
struct _tagged_string _temp785; _temp785.curr= _temp784; _temp785.base= _temp784;
_temp785.last_plus_one= _temp784 + 25; _temp785;}), loc);} return({ struct Cyc_List_List*
_temp786=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp786->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp787=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp787[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp788; _temp788.tag= Cyc_Absyn_Struct_d;
_temp788.f1= _temp782; _temp788;}); _temp787;}), loc); _temp786->tl= 0; _temp786;});}}
_LL729: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp789="bad attributes on tunion";
struct _tagged_string _temp790; _temp790.curr= _temp789; _temp790.base= _temp789;
_temp790.last_plus_one= _temp789 + 25; _temp790;}), loc);} return({ struct Cyc_List_List*
_temp791=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp791->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp792=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp792[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp793; _temp793.tag= Cyc_Absyn_Tunion_d;
_temp793.f1= _temp752; _temp793;}); _temp792;}), loc); _temp791->tl= 0; _temp791;});
_LL731: { struct Cyc_List_List* _temp794=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp758);
goto _LL795; _LL795: { struct Cyc_Absyn_Decl* _temp796= Cyc_Absyn_tunion_decl( s,
_temp766, _temp794, 0, _temp764, loc); goto _LL797; _LL797: if( atts != 0){ Cyc_Parse_err(({
unsigned char* _temp798="bad attributes on tunion"; struct _tagged_string
_temp799; _temp799.curr= _temp798; _temp799.base= _temp798; _temp799.last_plus_one=
_temp798 + 25; _temp799;}), loc);} return({ struct Cyc_List_List* _temp800=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp800->hd=(
void*) _temp796; _temp800->tl= 0; _temp800;});}} _LL733: { struct Cyc_List_List*
_temp801=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp770); goto _LL802; _LL802: { struct Cyc_Absyn_Uniondecl*
_temp805=({ struct Cyc_Absyn_Uniondecl* _temp803=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp803->sc=( void*) s;
_temp803->name=({ struct Cyc_Core_Opt* _temp804=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp804->v=( void*)(( struct _tuple1*)
_check_null( _temp772)); _temp804;}); _temp803->tvs= _temp801; _temp803->fields=
0; _temp803->attributes= 0; _temp803;}); goto _LL806; _LL806: if( atts != 0){
Cyc_Parse_err(({ unsigned char* _temp807="bad attributes on union"; struct
_tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 24; _temp808;}), loc);} return({ struct Cyc_List_List*
_temp809=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp809->hd=( void*)({ struct Cyc_Absyn_Decl* _temp810=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp810->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp811=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp811[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp812; _temp812.tag= Cyc_Absyn_Union_d; _temp812.f1= _temp805; _temp812;});
_temp811;})); _temp810->loc= loc; _temp810;}); _temp809->tl= 0; _temp809;});}}
_LL735: { struct Cyc_Absyn_Enumdecl* _temp814=({ struct Cyc_Absyn_Enumdecl*
_temp813=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp813->sc=( void*) s; _temp813->name= _temp776; _temp813->fields= 0; _temp813;});
goto _LL815; _LL815: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp816="bad attributes on enum";
struct _tagged_string _temp817; _temp817.curr= _temp816; _temp817.base= _temp816;
_temp817.last_plus_one= _temp816 + 23; _temp817;}), loc);} return({ struct Cyc_List_List*
_temp818=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp818->hd=( void*)({ struct Cyc_Absyn_Decl* _temp819=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp819->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp820=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp820[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp821; _temp821.tag= Cyc_Absyn_Enum_d; _temp821.f1= _temp814; _temp821;});
_temp820;})); _temp819->loc= loc; _temp819;}); _temp818->tl= 0; _temp818;});}
_LL737: Cyc_Parse_err(({ unsigned char* _temp822="missing declarator"; struct
_tagged_string _temp823; _temp823.curr= _temp822; _temp823.base= _temp822;
_temp823.last_plus_one= _temp822 + 19; _temp823;}), loc); return 0; _LL725:;}}
else{ void* t= _temp689.f1; struct Cyc_List_List* _temp824= Cyc_Parse_apply_tmss(
tq, t, _temp687, atts); goto _LL825; _LL825: if( istypedef){ if( ! exps_empty){
Cyc_Parse_err(({ unsigned char* _temp826="initializer in typedef declaration";
struct _tagged_string _temp827; _temp827.curr= _temp826; _temp827.base= _temp826;
_temp827.last_plus_one= _temp826 + 35; _temp827;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, _temp824); if( _temp689.f2 !=
0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp689.f2))->v;{ void* _temp828=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp840; struct Cyc_Absyn_Tuniondecl* _temp842; struct Cyc_Absyn_Uniondecl*
_temp844; struct Cyc_Absyn_Enumdecl* _temp846; _LL830: if(*(( int*) _temp828) ==
Cyc_Absyn_Struct_d){ _LL841: _temp840=(( struct Cyc_Absyn_Struct_d_struct*)
_temp828)->f1; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp828)
== Cyc_Absyn_Tunion_d){ _LL843: _temp842=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp828)->f1; goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp828)
== Cyc_Absyn_Union_d){ _LL845: _temp844=(( struct Cyc_Absyn_Union_d_struct*)
_temp828)->f1; goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp828)
== Cyc_Absyn_Enum_d){ _LL847: _temp846=(( struct Cyc_Absyn_Enum_d_struct*)
_temp828)->f1; goto _LL837;} else{ goto _LL838;} _LL838: goto _LL839; _LL831:(
void*)( _temp840->sc=( void*) s); _temp840->attributes= atts; atts= 0; goto
_LL829; _LL833:( void*)( _temp842->sc=( void*) s); goto _LL829; _LL835:( void*)(
_temp844->sc=( void*) s); goto _LL829; _LL837:( void*)( _temp846->sc=( void*) s);
goto _LL829; _LL839: Cyc_Parse_err(({ unsigned char* _temp848="declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp849; _temp849.curr= _temp848; _temp849.base= _temp848;
_temp849.last_plus_one= _temp848 + 70; _temp849;}), loc); goto _LL829; _LL829:;}
decls=({ struct Cyc_List_List* _temp850=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp850->hd=( void*) d; _temp850->tl= decls;
_temp850;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp851=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s in typedef", _temp851.last_plus_one - _temp851.curr,
_temp851.curr);}), loc);} return decls;}} else{ if( _temp689.f2 != 0){ Cyc_Parse_unimp2(({
unsigned char* _temp852="nested type declaration within declarator"; struct
_tagged_string _temp853; _temp853.curr= _temp852; _temp853.base= _temp852;
_temp853.last_plus_one= _temp852 + 42; _temp853;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp854= _temp824; goto _LL855; _LL855: for( 0;
_temp854 != 0;( _temp854=(( struct Cyc_List_List*) _check_null( _temp854))->tl,
_temp685=(( struct Cyc_List_List*) _check_null( _temp685))->tl)){ struct _tuple7
_temp858; struct Cyc_List_List* _temp859; struct Cyc_List_List* _temp861; void*
_temp863; struct Cyc_Absyn_Tqual _temp865; struct _tuple1* _temp867; struct
_tuple7* _temp856=( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp854))->hd; _temp858=* _temp856; _LL868: _temp867= _temp858.f1; goto _LL866;
_LL866: _temp865= _temp858.f2; goto _LL864; _LL864: _temp863= _temp858.f3; goto
_LL862; _LL862: _temp861= _temp858.f4; goto _LL860; _LL860: _temp859= _temp858.f5;
goto _LL857; _LL857: if( _temp861 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp869="bad type params, ignoring"; struct _tagged_string _temp870; _temp870.curr=
_temp869; _temp870.base= _temp869; _temp870.last_plus_one= _temp869 + 26;
_temp870;}), loc);} if( _temp685 == 0){(( void(*)( struct _tagged_string msg,
struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp871="unexpected null in parse!";
struct _tagged_string _temp872; _temp872.curr= _temp871; _temp872.base= _temp871;
_temp872.last_plus_one= _temp871 + 26; _temp872;}), loc);}{ struct Cyc_Absyn_Exp*
_temp873=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp685))->hd;
goto _LL874; _LL874: { struct Cyc_Absyn_Vardecl* _temp875= Cyc_Absyn_new_vardecl(
_temp867, _temp863, _temp873); goto _LL876; _LL876: _temp875->tq= _temp865;(
void*)( _temp875->sc=( void*) s); _temp875->attributes= _temp859;{ struct Cyc_Absyn_Decl*
_temp880=({ struct Cyc_Absyn_Decl* _temp877=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp877->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp878=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp878[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp879; _temp879.tag= Cyc_Absyn_Var_d;
_temp879.f1= _temp875; _temp879;}); _temp878;})); _temp877->loc= loc; _temp877;});
goto _LL881; _LL881: decls=({ struct Cyc_List_List* _temp882=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp882->hd=( void*) _temp880;
_temp882->tl= decls; _temp882;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(({ struct _tagged_string _temp883= s; xprintf("bad kind: %.*s",
_temp883.last_plus_one - _temp883.curr, _temp883.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
0)){ case 'A': _LL884: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL885:
return( void*) Cyc_Absyn_MemKind; case 'B': _LL886: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL887: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL888: return(
void*) Cyc_Absyn_EffKind; default: _LL889: Cyc_Parse_err(({ struct
_tagged_string _temp891= s; xprintf("bad kind: %.*s", _temp891.last_plus_one -
_temp891.curr, _temp891.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp892=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp892->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp893=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp893[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp894; _temp894.tag= Cyc_Absyn_Attributes_mod;
_temp894.f1= loc; _temp894.f2= atts; _temp894;}); _temp893;})); _temp892->tl=
tms; _temp892;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp895= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_temp895.last_plus_one - _temp895.curr, _temp895.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp896=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp896[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp897; _temp897.tag= Cyc_Absyn_Typedef_d; _temp897.f1=({
struct Cyc_Absyn_Typedefdecl* _temp898=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp898->name= x; _temp898->tvs=(*
t).f4; _temp898->defn=( void*)(* t).f3; _temp898;}); _temp897;}); _temp896;}),
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
f3; struct Cyc_Core_Opt* f4; } ; struct Cyc_ParamDeclListBool_tok_struct{
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
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_Scope_tok[
14u]="\000\000\000\000Scope_tok"; struct Cyc_Scope_tok_struct{ unsigned char*
tag; void* f1; } ; unsigned char Cyc_Short_tok[ 14u]="\000\000\000\000Short_tok";
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; unsigned char Cyc_Stmt_tok[
13u]="\000\000\000\000Stmt_tok"; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; unsigned char Cyc_StorageClass_tok[ 21u]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_String_tok[ 15u]="\000\000\000\000String_tok"; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_string f1; } ; unsigned char Cyc_Stringopt_tok[
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
unsigned char _temp902[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp902, _temp902, _temp902 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp903= yy1; struct _tuple16* _temp909; _LL905: if(*(( void**)
_temp903) == Cyc_Int_tok){ _LL910: _temp909=(( struct Cyc_Int_tok_struct*)
_temp903)->f1; goto _LL906;} else{ goto _LL907;} _LL907: goto _LL908; _LL906:
return _temp909; _LL908:( void) _throw(( void*)& Cyc_yyfail_Int_tok); _LL904:;}
static unsigned char _temp914[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp914, _temp914, _temp914 + 11u}};
struct _tagged_string Cyc_yyget_String_tok( void* yy1){ void* _temp915= yy1;
struct _tagged_string _temp921; _LL917: if(*(( void**) _temp915) == Cyc_String_tok){
_LL922: _temp921=(( struct Cyc_String_tok_struct*) _temp915)->f1; goto _LL918;}
else{ goto _LL919;} _LL919: goto _LL920; _LL918: return _temp921; _LL920:( void)
_throw(( void*)& Cyc_yyfail_String_tok); _LL916:;} static unsigned char _temp926[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp926, _temp926, _temp926 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp927= yy1; unsigned char _temp933; _LL929: if(*(( void**) _temp927) ==
Cyc_Char_tok){ _LL934: _temp933=(( struct Cyc_Char_tok_struct*) _temp927)->f1;
goto _LL930;} else{ goto _LL931;} _LL931: goto _LL932; _LL930: return _temp933;
_LL932:( void) _throw(( void*)& Cyc_yyfail_Char_tok); _LL928:;} static
unsigned char _temp938[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp938, _temp938, _temp938 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp939= yy1; void*
_temp945; _LL941: if(*(( void**) _temp939) == Cyc_Pointer_Sort_tok){ _LL946:
_temp945=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp939)->f1; goto
_LL942;} else{ goto _LL943;} _LL943: goto _LL944; _LL942: return _temp945;
_LL944:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL940:;} static
unsigned char _temp950[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp950, _temp950, _temp950 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp951= yy1; struct Cyc_Absyn_Exp*
_temp957; _LL953: if(*(( void**) _temp951) == Cyc_Exp_tok){ _LL958: _temp957=((
struct Cyc_Exp_tok_struct*) _temp951)->f1; goto _LL954;} else{ goto _LL955;}
_LL955: goto _LL956; _LL954: return _temp957; _LL956:( void) _throw(( void*)&
Cyc_yyfail_Exp_tok); _LL952:;} static unsigned char _temp962[ 12u]="ExpList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{
_temp962, _temp962, _temp962 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok(
void* yy1){ void* _temp963= yy1; struct Cyc_List_List* _temp969; _LL965: if(*((
void**) _temp963) == Cyc_ExpList_tok){ _LL970: _temp969=(( struct Cyc_ExpList_tok_struct*)
_temp963)->f1; goto _LL966;} else{ goto _LL967;} _LL967: goto _LL968; _LL966:
return _temp969; _LL968:( void) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL964:;}
static unsigned char _temp974[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{ _temp974, _temp974, _temp974
+ 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok( void* yy1){ void*
_temp975= yy1; struct Cyc_List_List* _temp981; _LL977: if(*(( void**) _temp975)
== Cyc_InitializerList_tok){ _LL982: _temp981=(( struct Cyc_InitializerList_tok_struct*)
_temp975)->f1; goto _LL978;} else{ goto _LL979;} _LL979: goto _LL980; _LL978:
return _temp981; _LL980:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL976:;} static unsigned char _temp986[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp986, _temp986, _temp986 + 11u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* _temp987= yy1; void* _temp993;
_LL989: if(*(( void**) _temp987) == Cyc_Primop_tok){ _LL994: _temp993=( void*)((
struct Cyc_Primop_tok_struct*) _temp987)->f1; goto _LL990;} else{ goto _LL991;}
_LL991: goto _LL992; _LL990: return _temp993; _LL992:( void) _throw(( void*)&
Cyc_yyfail_Primop_tok); _LL988:;} static unsigned char _temp998[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{
_temp998, _temp998, _temp998 + 14u}}; struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok(
void* yy1){ void* _temp999= yy1; struct Cyc_Core_Opt* _temp1005; _LL1001: if(*((
void**) _temp999) == Cyc_Primopopt_tok){ _LL1006: _temp1005=(( struct Cyc_Primopopt_tok_struct*)
_temp999)->f1; goto _LL1002;} else{ goto _LL1003;} _LL1003: goto _LL1004;
_LL1002: return _temp1005; _LL1004:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL1000:;} static unsigned char _temp1010[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{ _temp1010, _temp1010, _temp1010 + 11u}};
struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){ void* _temp1011= yy1; struct
_tuple1* _temp1017; _LL1013: if(*(( void**) _temp1011) == Cyc_QualId_tok){
_LL1018: _temp1017=(( struct Cyc_QualId_tok_struct*) _temp1011)->f1; goto
_LL1014;} else{ goto _LL1015;} _LL1015: goto _LL1016; _LL1014: return _temp1017;
_LL1016:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL1012:;} static
unsigned char _temp1022[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp1022, _temp1022, _temp1022 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1023= yy1;
struct Cyc_Absyn_Stmt* _temp1029; _LL1025: if(*(( void**) _temp1023) == Cyc_Stmt_tok){
_LL1030: _temp1029=(( struct Cyc_Stmt_tok_struct*) _temp1023)->f1; goto _LL1026;}
else{ goto _LL1027;} _LL1027: goto _LL1028; _LL1026: return _temp1029; _LL1028:(
void) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1024:;} static unsigned char
_temp1034[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp1034, _temp1034, _temp1034 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp1035= yy1; void* _temp1041; _LL1037: if(*(( void**)
_temp1035) == Cyc_BlockItem_tok){ _LL1042: _temp1041=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp1035)->f1; goto _LL1038;} else{ goto _LL1039;} _LL1039: goto _LL1040;
_LL1038: return _temp1041; _LL1040:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL1036:;} static unsigned char _temp1046[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp1046, _temp1046, _temp1046 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp1047= yy1; struct Cyc_List_List* _temp1053; _LL1049: if(*((
void**) _temp1047) == Cyc_SwitchClauseList_tok){ _LL1054: _temp1053=(( struct
Cyc_SwitchClauseList_tok_struct*) _temp1047)->f1; goto _LL1050;} else{ goto
_LL1051;} _LL1051: goto _LL1052; _LL1050: return _temp1053; _LL1052:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1048:;} static
unsigned char _temp1058[ 22u]="SwitchCClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{ _temp1058, _temp1058,
_temp1058 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok( void*
yy1){ void* _temp1059= yy1; struct Cyc_List_List* _temp1065; _LL1061: if(*((
void**) _temp1059) == Cyc_SwitchCClauseList_tok){ _LL1066: _temp1065=(( struct
Cyc_SwitchCClauseList_tok_struct*) _temp1059)->f1; goto _LL1062;} else{ goto
_LL1063;} _LL1063: goto _LL1064; _LL1062: return _temp1065; _LL1064:( void)
_throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok); _LL1060:;} static
unsigned char _temp1070[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp1070, _temp1070, _temp1070 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1071= yy1;
struct Cyc_Absyn_Pat* _temp1077; _LL1073: if(*(( void**) _temp1071) == Cyc_Pattern_tok){
_LL1078: _temp1077=(( struct Cyc_Pattern_tok_struct*) _temp1071)->f1; goto
_LL1074;} else{ goto _LL1075;} _LL1075: goto _LL1076; _LL1074: return _temp1077;
_LL1076:( void) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL1072:;} static
unsigned char _temp1082[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp1082, _temp1082, _temp1082
+ 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ void*
_temp1083= yy1; struct Cyc_List_List* _temp1089; _LL1085: if(*(( void**)
_temp1083) == Cyc_PatternList_tok){ _LL1090: _temp1089=(( struct Cyc_PatternList_tok_struct*)
_temp1083)->f1; goto _LL1086;} else{ goto _LL1087;} _LL1087: goto _LL1088;
_LL1086: return _temp1089; _LL1088:( void) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL1084:;} static unsigned char _temp1094[ 17u]="FieldPattern_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp1094, _temp1094, _temp1094 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp1095= yy1; struct _tuple14* _temp1101; _LL1097: if(*((
void**) _temp1095) == Cyc_FieldPattern_tok){ _LL1102: _temp1101=(( struct Cyc_FieldPattern_tok_struct*)
_temp1095)->f1; goto _LL1098;} else{ goto _LL1099;} _LL1099: goto _LL1100;
_LL1098: return _temp1101; _LL1100:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1096:;} static unsigned char _temp1106[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp1106, _temp1106, _temp1106 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp1107= yy1; struct Cyc_List_List* _temp1113; _LL1109: if(*((
void**) _temp1107) == Cyc_FieldPatternList_tok){ _LL1114: _temp1113=(( struct
Cyc_FieldPatternList_tok_struct*) _temp1107)->f1; goto _LL1110;} else{ goto
_LL1111;} _LL1111: goto _LL1112; _LL1110: return _temp1113; _LL1112:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1108:;} static
unsigned char _temp1118[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp1118, _temp1118, _temp1118 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1119= yy1;
struct Cyc_Absyn_Fndecl* _temp1125; _LL1121: if(*(( void**) _temp1119) == Cyc_FnDecl_tok){
_LL1126: _temp1125=(( struct Cyc_FnDecl_tok_struct*) _temp1119)->f1; goto
_LL1122;} else{ goto _LL1123;} _LL1123: goto _LL1124; _LL1122: return _temp1125;
_LL1124:( void) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL1120:;} static
unsigned char _temp1130[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{ _temp1130, _temp1130, _temp1130 +
13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok( void* yy1){ void* _temp1131=
yy1; struct Cyc_List_List* _temp1137; _LL1133: if(*(( void**) _temp1131) == Cyc_DeclList_tok){
_LL1138: _temp1137=(( struct Cyc_DeclList_tok_struct*) _temp1131)->f1; goto
_LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136; _LL1134: return _temp1137;
_LL1136:( void) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1132:;} static
unsigned char _temp1142[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp1142, _temp1142, _temp1142 +
13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){
void* _temp1143= yy1; struct Cyc_Parse_Declaration_spec* _temp1149; _LL1145: if(*((
void**) _temp1143) == Cyc_DeclSpec_tok){ _LL1150: _temp1149=(( struct Cyc_DeclSpec_tok_struct*)
_temp1143)->f1; goto _LL1146;} else{ goto _LL1147;} _LL1147: goto _LL1148;
_LL1146: return _temp1149; _LL1148:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1144:;} static unsigned char _temp1154[ 13u]="InitDecl_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp1154,
_temp1154, _temp1154 + 13u}}; struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){
void* _temp1155= yy1; struct _tuple15* _temp1161; _LL1157: if(*(( void**)
_temp1155) == Cyc_InitDecl_tok){ _LL1162: _temp1161=(( struct Cyc_InitDecl_tok_struct*)
_temp1155)->f1; goto _LL1158;} else{ goto _LL1159;} _LL1159: goto _LL1160;
_LL1158: return _temp1161; _LL1160:( void) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL1156:;} static unsigned char _temp1166[ 17u]="InitDeclList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp1166, _temp1166, _temp1166 + 17u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ void* _temp1167= yy1; struct Cyc_List_List* _temp1173; _LL1169: if(*((
void**) _temp1167) == Cyc_InitDeclList_tok){ _LL1174: _temp1173=(( struct Cyc_InitDeclList_tok_struct*)
_temp1167)->f1; goto _LL1170;} else{ goto _LL1171;} _LL1171: goto _LL1172;
_LL1170: return _temp1173; _LL1172:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1168:;} static unsigned char _temp1178[ 17u]="StorageClass_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{
_temp1178, _temp1178, _temp1178 + 17u}}; void* Cyc_yyget_StorageClass_tok( void*
yy1){ void* _temp1179= yy1; void* _temp1185; _LL1181: if(*(( void**) _temp1179)
== Cyc_StorageClass_tok){ _LL1186: _temp1185=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp1179)->f1; goto _LL1182;} else{ goto _LL1183;} _LL1183: goto _LL1184;
_LL1182: return _temp1185; _LL1184:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1180:;} static unsigned char _temp1190[ 18u]="TypeSpecifier_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp1190, _temp1190, _temp1190 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp1191= yy1; void* _temp1197; _LL1193: if(*(( void**) _temp1191)
== Cyc_TypeSpecifier_tok){ _LL1198: _temp1197=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp1191)->f1; goto _LL1194;} else{ goto _LL1195;} _LL1195: goto _LL1196;
_LL1194: return _temp1197; _LL1196:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1192:;} static unsigned char _temp1202[ 18u]="StructOrUnion_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp1202, _temp1202, _temp1202 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1203= yy1; void* _temp1209; _LL1205: if(*(( void**) _temp1203)
== Cyc_StructOrUnion_tok){ _LL1210: _temp1209=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1203)->f1; goto _LL1206;} else{ goto _LL1207;} _LL1207: goto _LL1208;
_LL1206: return _temp1209; _LL1208:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1204:;} static unsigned char _temp1214[ 13u]="TypeQual_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp1214,
_temp1214, _temp1214 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1215= yy1; struct Cyc_Absyn_Tqual _temp1221; _LL1217: if(*((
void**) _temp1215) == Cyc_TypeQual_tok){ _LL1222: _temp1221=(( struct Cyc_TypeQual_tok_struct*)
_temp1215)->f1; goto _LL1218;} else{ goto _LL1219;} _LL1219: goto _LL1220;
_LL1218: return _temp1221; _LL1220:( void) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL1216:;} static unsigned char _temp1226[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp1226, _temp1226, _temp1226 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp1227= yy1; struct Cyc_List_List* _temp1233; _LL1229: if(*((
void**) _temp1227) == Cyc_StructFieldDeclList_tok){ _LL1234: _temp1233=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp1227)->f1; goto _LL1230;} else{ goto
_LL1231;} _LL1231: goto _LL1232; _LL1230: return _temp1233; _LL1232:( void)
_throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL1228:;} static
unsigned char _temp1238[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp1238, _temp1238,
_temp1238 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp1239= yy1; struct Cyc_List_List* _temp1245; _LL1241: if(*((
void**) _temp1239) == Cyc_StructFieldDeclListList_tok){ _LL1246: _temp1245=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp1239)->f1; goto _LL1242;}
else{ goto _LL1243;} _LL1243: goto _LL1244; _LL1242: return _temp1245; _LL1244:(
void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL1240:;}
static unsigned char _temp1250[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp1250, _temp1250,
_temp1250 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp1251= yy1; struct Cyc_List_List* _temp1257; _LL1253: if(*((
void**) _temp1251) == Cyc_TypeModifierList_tok){ _LL1258: _temp1257=(( struct
Cyc_TypeModifierList_tok_struct*) _temp1251)->f1; goto _LL1254;} else{ goto
_LL1255;} _LL1255: goto _LL1256; _LL1254: return _temp1257; _LL1256:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1252:;} static
unsigned char _temp1262[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp1262, _temp1262, _temp1262 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1263= yy1; void* _temp1269;
_LL1265: if(*(( void**) _temp1263) == Cyc_Rgn_tok){ _LL1270: _temp1269=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1263)->f1; goto _LL1266;} else{ goto _LL1267;}
_LL1267: goto _LL1268; _LL1266: return _temp1269; _LL1268:( void) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1264:;} static unsigned char _temp1274[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{
_temp1274, _temp1274, _temp1274 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp1275= yy1; struct Cyc_Parse_Declarator* _temp1281;
_LL1277: if(*(( void**) _temp1275) == Cyc_Declarator_tok){ _LL1282: _temp1281=((
struct Cyc_Declarator_tok_struct*) _temp1275)->f1; goto _LL1278;} else{ goto
_LL1279;} _LL1279: goto _LL1280; _LL1278: return _temp1281; _LL1280:( void)
_throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1276:;} static unsigned char
_temp1286[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1286, _temp1286,
_temp1286 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1287= yy1; struct _tuple13* _temp1293; _LL1289: if(*(( void**)
_temp1287) == Cyc_DeclaratorExpopt_tok){ _LL1294: _temp1293=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1287)->f1; goto _LL1290;} else{ goto _LL1291;} _LL1291: goto _LL1292;
_LL1290: return _temp1293; _LL1292:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1288:;} static unsigned char _temp1298[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1298, _temp1298, _temp1298 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1299= yy1; struct Cyc_List_List* _temp1305; _LL1301: if(*((
void**) _temp1299) == Cyc_DeclaratorExpoptList_tok){ _LL1306: _temp1305=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1299)->f1; goto _LL1302;}
else{ goto _LL1303;} _LL1303: goto _LL1304; _LL1302: return _temp1305; _LL1304:(
void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1300:;} static
unsigned char _temp1310[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1310, _temp1310,
_temp1310 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1311= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1317;
_LL1313: if(*(( void**) _temp1311) == Cyc_AbstractDeclarator_tok){ _LL1318:
_temp1317=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1311)->f1; goto
_LL1314;} else{ goto _LL1315;} _LL1315: goto _LL1316; _LL1314: return _temp1317;
_LL1316:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1312:;}
static unsigned char _temp1322[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1322, _temp1322, _temp1322 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1323= yy1; int _temp1329; _LL1325:
if(*(( void**) _temp1323) == Cyc_Bool_tok){ _LL1330: _temp1329=(( struct Cyc_Bool_tok_struct*)
_temp1323)->f1; goto _LL1326;} else{ goto _LL1327;} _LL1327: goto _LL1328;
_LL1326: return _temp1329; _LL1328:( void) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1324:;} static unsigned char _temp1334[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1334, _temp1334, _temp1334 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1335= yy1; void* _temp1341;
_LL1337: if(*(( void**) _temp1335) == Cyc_Scope_tok){ _LL1342: _temp1341=( void*)((
struct Cyc_Scope_tok_struct*) _temp1335)->f1; goto _LL1338;} else{ goto _LL1339;}
_LL1339: goto _LL1340; _LL1338: return _temp1341; _LL1340:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1336:;} static unsigned char _temp1346[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1346, _temp1346, _temp1346 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1347= yy1; struct Cyc_Absyn_Tunionfield* _temp1353;
_LL1349: if(*(( void**) _temp1347) == Cyc_TunionField_tok){ _LL1354: _temp1353=((
struct Cyc_TunionField_tok_struct*) _temp1347)->f1; goto _LL1350;} else{ goto
_LL1351;} _LL1351: goto _LL1352; _LL1350: return _temp1353; _LL1352:( void)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1348:;} static unsigned char
_temp1358[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1358, _temp1358, _temp1358 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1359= yy1; struct Cyc_List_List*
_temp1365; _LL1361: if(*(( void**) _temp1359) == Cyc_TunionFieldList_tok){
_LL1366: _temp1365=(( struct Cyc_TunionFieldList_tok_struct*) _temp1359)->f1;
goto _LL1362;} else{ goto _LL1363;} _LL1363: goto _LL1364; _LL1362: return
_temp1365; _LL1364:( void) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1360:;} static unsigned char _temp1370[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1370, _temp1370, _temp1370 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1371= yy1; struct _tuple18* _temp1377; _LL1373: if(*((
void**) _temp1371) == Cyc_QualSpecList_tok){ _LL1378: _temp1377=(( struct Cyc_QualSpecList_tok_struct*)
_temp1371)->f1; goto _LL1374;} else{ goto _LL1375;} _LL1375: goto _LL1376;
_LL1374: return _temp1377; _LL1376:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1372:;} static unsigned char _temp1382[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1382, _temp1382, _temp1382 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1383= yy1;
struct Cyc_List_List* _temp1389; _LL1385: if(*(( void**) _temp1383) == Cyc_IdList_tok){
_LL1390: _temp1389=(( struct Cyc_IdList_tok_struct*) _temp1383)->f1; goto
_LL1386;} else{ goto _LL1387;} _LL1387: goto _LL1388; _LL1386: return _temp1389;
_LL1388:( void) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1384:;} static
unsigned char _temp1394[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1394, _temp1394, _temp1394 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1395= yy1;
struct _tuple2* _temp1401; _LL1397: if(*(( void**) _temp1395) == Cyc_ParamDecl_tok){
_LL1402: _temp1401=(( struct Cyc_ParamDecl_tok_struct*) _temp1395)->f1; goto
_LL1398;} else{ goto _LL1399;} _LL1399: goto _LL1400; _LL1398: return _temp1401;
_LL1400:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1396:;} static
unsigned char _temp1406[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1406, _temp1406,
_temp1406 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1407= yy1; struct Cyc_List_List* _temp1413; _LL1409: if(*(( void**)
_temp1407) == Cyc_ParamDeclList_tok){ _LL1414: _temp1413=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1407)->f1; goto _LL1410;} else{ goto _LL1411;} _LL1411: goto _LL1412;
_LL1410: return _temp1413; _LL1412:( void) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1408:;} static unsigned char _temp1418[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1418, _temp1418, _temp1418 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1419= yy1; struct _tuple17* _temp1425; _LL1421: if(*((
void**) _temp1419) == Cyc_ParamDeclListBool_tok){ _LL1426: _temp1425=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1419)->f1; goto _LL1422;} else{ goto
_LL1423;} _LL1423: goto _LL1424; _LL1422: return _temp1425; _LL1424:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1420:;} static
unsigned char _temp1430[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1430, _temp1430, _temp1430 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1431=
yy1; struct Cyc_List_List* _temp1437; _LL1433: if(*(( void**) _temp1431) == Cyc_TypeList_tok){
_LL1438: _temp1437=(( struct Cyc_TypeList_tok_struct*) _temp1431)->f1; goto
_LL1434;} else{ goto _LL1435;} _LL1435: goto _LL1436; _LL1434: return _temp1437;
_LL1436:( void) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1432:;} static
unsigned char _temp1442[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1442, _temp1442,
_temp1442 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1443= yy1; struct Cyc_List_List* _temp1449; _LL1445: if(*(( void**)
_temp1443) == Cyc_DesignatorList_tok){ _LL1450: _temp1449=(( struct Cyc_DesignatorList_tok_struct*)
_temp1443)->f1; goto _LL1446;} else{ goto _LL1447;} _LL1447: goto _LL1448;
_LL1446: return _temp1449; _LL1448:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1444:;} static unsigned char _temp1454[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1454,
_temp1454, _temp1454 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1455= yy1; void* _temp1461; _LL1457: if(*(( void**) _temp1455) == Cyc_Designator_tok){
_LL1462: _temp1461=( void*)(( struct Cyc_Designator_tok_struct*) _temp1455)->f1;
goto _LL1458;} else{ goto _LL1459;} _LL1459: goto _LL1460; _LL1458: return
_temp1461; _LL1460:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1456:;}
static unsigned char _temp1466[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1466, _temp1466, _temp1466 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1467= yy1; void* _temp1473;
_LL1469: if(*(( void**) _temp1467) == Cyc_Kind_tok){ _LL1474: _temp1473=( void*)((
struct Cyc_Kind_tok_struct*) _temp1467)->f1; goto _LL1470;} else{ goto _LL1471;}
_LL1471: goto _LL1472; _LL1470: return _temp1473; _LL1472:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1468:;} static unsigned char _temp1478[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1478, _temp1478, _temp1478 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1479= yy1; void* _temp1485; _LL1481: if(*(( void**) _temp1479) == Cyc_Type_tok){
_LL1486: _temp1485=( void*)(( struct Cyc_Type_tok_struct*) _temp1479)->f1; goto
_LL1482;} else{ goto _LL1483;} _LL1483: goto _LL1484; _LL1482: return _temp1485;
_LL1484:( void) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1480:;} static
unsigned char _temp1490[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1490, _temp1490,
_temp1490 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1491= yy1; struct Cyc_List_List* _temp1497; _LL1493: if(*(( void**)
_temp1491) == Cyc_AttributeList_tok){ _LL1498: _temp1497=(( struct Cyc_AttributeList_tok_struct*)
_temp1491)->f1; goto _LL1494;} else{ goto _LL1495;} _LL1495: goto _LL1496;
_LL1494: return _temp1497; _LL1496:( void) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1492:;} static unsigned char _temp1502[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1502,
_temp1502, _temp1502 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1503= yy1; void* _temp1509; _LL1505: if(*(( void**) _temp1503) == Cyc_Attribute_tok){
_LL1510: _temp1509=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1503)->f1;
goto _LL1506;} else{ goto _LL1507;} _LL1507: goto _LL1508; _LL1506: return
_temp1509; _LL1508:( void) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1504:;}
static unsigned char _temp1514[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1514, _temp1514, _temp1514 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1515= yy1; struct Cyc_Absyn_Enumfield* _temp1521; _LL1517: if(*(( void**)
_temp1515) == Cyc_Enumfield_tok){ _LL1522: _temp1521=(( struct Cyc_Enumfield_tok_struct*)
_temp1515)->f1; goto _LL1518;} else{ goto _LL1519;} _LL1519: goto _LL1520;
_LL1518: return _temp1521; _LL1520:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1516:;} static unsigned char _temp1526[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1526, _temp1526, _temp1526 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1527= yy1; struct Cyc_List_List* _temp1533; _LL1529: if(*((
void**) _temp1527) == Cyc_EnumfieldList_tok){ _LL1534: _temp1533=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1527)->f1; goto _LL1530;} else{ goto _LL1531;} _LL1531: goto _LL1532;
_LL1530: return _temp1533; _LL1532:( void) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1528:;} static unsigned char _temp1538[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1538, _temp1538, _temp1538 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1539= yy1;
struct Cyc_Core_Opt* _temp1545; _LL1541: if(*(( void**) _temp1539) == Cyc_TypeOpt_tok){
_LL1546: _temp1545=(( struct Cyc_TypeOpt_tok_struct*) _temp1539)->f1; goto
_LL1542;} else{ goto _LL1543;} _LL1543: goto _LL1544; _LL1542: return _temp1545;
_LL1544:( void) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1540:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; struct Cyc_Yyltype Cyc_yynewloc(){ return({
struct Cyc_Yyltype _temp1547; _temp1547.timestamp= 0; _temp1547.first_line= 0;
_temp1547.first_column= 0; _temp1547.last_line= 0; _temp1547.last_column= 0;
_temp1547.text=({ unsigned char* _temp1548=""; struct _tagged_string _temp1549;
_temp1549.curr= _temp1548; _temp1549.base= _temp1548; _temp1549.last_plus_one=
_temp1548 + 1; _temp1549;}); _temp1547;});} static unsigned char _temp1552[ 1u]="";
struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0,{ _temp1552, _temp1552, _temp1552
+ 1u}}; static short Cyc_yytranslate[ 352u]={ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 123, 2, 2, 105,
121, 117, 2, 101, 102, 112, 115, 103, 116, 109, 120, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 108, 97, 106, 100, 107, 114, 113, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 110, 2, 111, 119, 104, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 98, 118, 99, 122, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
89, 90, 91, 92, 93, 94, 95, 96}; static unsigned char _temp1555[ 2u]="$"; static
unsigned char _temp1558[ 6u]="error"; static unsigned char _temp1561[ 12u]="$undefined.";
static unsigned char _temp1564[ 5u]="AUTO"; static unsigned char _temp1567[ 9u]="REGISTER";
static unsigned char _temp1570[ 7u]="STATIC"; static unsigned char _temp1573[ 7u]="EXTERN";
static unsigned char _temp1576[ 8u]="TYPEDEF"; static unsigned char _temp1579[ 5u]="VOID";
static unsigned char _temp1582[ 5u]="CHAR"; static unsigned char _temp1585[ 6u]="SHORT";
static unsigned char _temp1588[ 4u]="INT"; static unsigned char _temp1591[ 5u]="LONG";
static unsigned char _temp1594[ 6u]="FLOAT"; static unsigned char _temp1597[ 7u]="DOUBLE";
static unsigned char _temp1600[ 7u]="SIGNED"; static unsigned char _temp1603[ 9u]="UNSIGNED";
static unsigned char _temp1606[ 6u]="CONST"; static unsigned char _temp1609[ 9u]="VOLATILE";
static unsigned char _temp1612[ 9u]="RESTRICT"; static unsigned char _temp1615[
7u]="STRUCT"; static unsigned char _temp1618[ 6u]="UNION"; static unsigned char
_temp1621[ 5u]="CASE"; static unsigned char _temp1624[ 8u]="DEFAULT"; static
unsigned char _temp1627[ 7u]="INLINE"; static unsigned char _temp1630[ 3u]="IF";
static unsigned char _temp1633[ 5u]="ELSE"; static unsigned char _temp1636[ 7u]="SWITCH";
static unsigned char _temp1639[ 6u]="WHILE"; static unsigned char _temp1642[ 3u]="DO";
static unsigned char _temp1645[ 4u]="FOR"; static unsigned char _temp1648[ 5u]="GOTO";
static unsigned char _temp1651[ 9u]="CONTINUE"; static unsigned char _temp1654[
6u]="BREAK"; static unsigned char _temp1657[ 7u]="RETURN"; static unsigned char
_temp1660[ 7u]="SIZEOF"; static unsigned char _temp1663[ 5u]="ENUM"; static
unsigned char _temp1666[ 8u]="NULL_kw"; static unsigned char _temp1669[ 4u]="LET";
static unsigned char _temp1672[ 6u]="THROW"; static unsigned char _temp1675[ 4u]="TRY";
static unsigned char _temp1678[ 6u]="CATCH"; static unsigned char _temp1681[ 4u]="NEW";
static unsigned char _temp1684[ 9u]="ABSTRACT"; static unsigned char _temp1687[
9u]="FALLTHRU"; static unsigned char _temp1690[ 6u]="USING"; static
unsigned char _temp1693[ 10u]="NAMESPACE"; static unsigned char _temp1696[ 7u]="TUNION";
static unsigned char _temp1699[ 8u]="XTUNION"; static unsigned char _temp1702[ 5u]="FILL";
static unsigned char _temp1705[ 8u]="CODEGEN"; static unsigned char _temp1708[ 4u]="CUT";
static unsigned char _temp1711[ 7u]="SPLICE"; static unsigned char _temp1714[ 7u]="PRINTF";
static unsigned char _temp1717[ 8u]="FPRINTF"; static unsigned char _temp1720[ 8u]="XPRINTF";
static unsigned char _temp1723[ 6u]="SCANF"; static unsigned char _temp1726[ 7u]="FSCANF";
static unsigned char _temp1729[ 7u]="SSCANF"; static unsigned char _temp1732[ 7u]="MALLOC";
static unsigned char _temp1735[ 9u]="REGION_T"; static unsigned char _temp1738[
7u]="REGION"; static unsigned char _temp1741[ 5u]="RNEW"; static unsigned char
_temp1744[ 8u]="RMALLOC"; static unsigned char _temp1747[ 7u]="PTR_OP"; static
unsigned char _temp1750[ 7u]="INC_OP"; static unsigned char _temp1753[ 7u]="DEC_OP";
static unsigned char _temp1756[ 8u]="LEFT_OP"; static unsigned char _temp1759[ 9u]="RIGHT_OP";
static unsigned char _temp1762[ 6u]="LE_OP"; static unsigned char _temp1765[ 6u]="GE_OP";
static unsigned char _temp1768[ 6u]="EQ_OP"; static unsigned char _temp1771[ 6u]="NE_OP";
static unsigned char _temp1774[ 7u]="AND_OP"; static unsigned char _temp1777[ 6u]="OR_OP";
static unsigned char _temp1780[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1783[ 11u]="DIV_ASSIGN"; static unsigned char _temp1786[ 11u]="MOD_ASSIGN";
static unsigned char _temp1789[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1792[ 11u]="SUB_ASSIGN"; static unsigned char _temp1795[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1798[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1801[ 11u]="AND_ASSIGN"; static unsigned char _temp1804[ 11u]="XOR_ASSIGN";
static unsigned char _temp1807[ 10u]="OR_ASSIGN"; static unsigned char _temp1810[
9u]="ELLIPSIS"; static unsigned char _temp1813[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1816[ 12u]="COLON_COLON"; static unsigned char _temp1819[ 11u]="IDENTIFIER";
static unsigned char _temp1822[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1825[ 7u]="STRING"; static unsigned char _temp1828[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1831[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1834[ 9u]="TYPE_VAR"; static unsigned char _temp1837[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1840[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1843[ 10u]="ATTRIBUTE"; static unsigned char _temp1846[ 4u]="';'"; static
unsigned char _temp1849[ 4u]="'{'"; static unsigned char _temp1852[ 4u]="'}'";
static unsigned char _temp1855[ 4u]="'='"; static unsigned char _temp1858[ 4u]="'('";
static unsigned char _temp1861[ 4u]="')'"; static unsigned char _temp1864[ 4u]="','";
static unsigned char _temp1867[ 4u]="'_'"; static unsigned char _temp1870[ 4u]="'$'";
static unsigned char _temp1873[ 4u]="'<'"; static unsigned char _temp1876[ 4u]="'>'";
static unsigned char _temp1879[ 4u]="':'"; static unsigned char _temp1882[ 4u]="'.'";
static unsigned char _temp1885[ 4u]="'['"; static unsigned char _temp1888[ 4u]="']'";
static unsigned char _temp1891[ 4u]="'*'"; static unsigned char _temp1894[ 4u]="'@'";
static unsigned char _temp1897[ 4u]="'?'"; static unsigned char _temp1900[ 4u]="'+'";
static unsigned char _temp1903[ 4u]="'-'"; static unsigned char _temp1906[ 4u]="'&'";
static unsigned char _temp1909[ 4u]="'|'"; static unsigned char _temp1912[ 4u]="'^'";
static unsigned char _temp1915[ 4u]="'/'"; static unsigned char _temp1918[ 4u]="'%'";
static unsigned char _temp1921[ 4u]="'~'"; static unsigned char _temp1924[ 4u]="'!'";
static unsigned char _temp1927[ 5u]="prog"; static unsigned char _temp1930[ 17u]="translation_unit";
static unsigned char _temp1933[ 21u]="external_declaration"; static
unsigned char _temp1936[ 20u]="function_definition"; static unsigned char
_temp1939[ 21u]="function_definition2"; static unsigned char _temp1942[ 13u]="using_action";
static unsigned char _temp1945[ 15u]="unusing_action"; static unsigned char
_temp1948[ 17u]="namespace_action"; static unsigned char _temp1951[ 19u]="unnamespace_action";
static unsigned char _temp1954[ 12u]="declaration"; static unsigned char
_temp1957[ 17u]="declaration_list"; static unsigned char _temp1960[ 23u]="declaration_specifiers";
static unsigned char _temp1963[ 24u]="storage_class_specifier"; static
unsigned char _temp1966[ 15u]="attributes_opt"; static unsigned char _temp1969[
11u]="attributes"; static unsigned char _temp1972[ 15u]="attribute_list"; static
unsigned char _temp1975[ 10u]="attribute"; static unsigned char _temp1978[ 15u]="type_specifier";
static unsigned char _temp1981[ 5u]="kind"; static unsigned char _temp1984[ 15u]="type_qualifier";
static unsigned char _temp1987[ 15u]="enum_specifier"; static unsigned char
_temp1990[ 11u]="enum_field"; static unsigned char _temp1993[ 22u]="enum_declaration_list";
static unsigned char _temp1996[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1999[ 16u]="type_params_opt"; static unsigned char _temp2002[
16u]="struct_or_union"; static unsigned char _temp2005[ 24u]="struct_declaration_list";
static unsigned char _temp2008[ 25u]="struct_declaration_list0"; static
unsigned char _temp2011[ 21u]="init_declarator_list"; static unsigned char
_temp2014[ 22u]="init_declarator_list0"; static unsigned char _temp2017[ 16u]="init_declarator";
static unsigned char _temp2020[ 19u]="struct_declaration"; static unsigned char
_temp2023[ 25u]="specifier_qualifier_list"; static unsigned char _temp2026[ 23u]="struct_declarator_list";
static unsigned char _temp2029[ 24u]="struct_declarator_list0"; static
unsigned char _temp2032[ 18u]="struct_declarator"; static unsigned char
_temp2035[ 17u]="tunion_specifier"; static unsigned char _temp2038[ 18u]="tunion_or_xtunion";
static unsigned char _temp2041[ 17u]="tunionfield_list"; static unsigned char
_temp2044[ 18u]="tunionfield_scope"; static unsigned char _temp2047[ 12u]="tunionfield";
static unsigned char _temp2050[ 11u]="declarator"; static unsigned char
_temp2053[ 18u]="direct_declarator"; static unsigned char _temp2056[ 8u]="pointer";
static unsigned char _temp2059[ 13u]="pointer_char"; static unsigned char
_temp2062[ 8u]="rgn_opt"; static unsigned char _temp2065[ 4u]="rgn"; static
unsigned char _temp2068[ 20u]="type_qualifier_list"; static unsigned char
_temp2071[ 20u]="parameter_type_list"; static unsigned char _temp2074[ 16u]="optional_effect";
static unsigned char _temp2077[ 16u]="optional_inject"; static unsigned char
_temp2080[ 11u]="effect_set"; static unsigned char _temp2083[ 14u]="atomic_effect";
static unsigned char _temp2086[ 11u]="region_set"; static unsigned char
_temp2089[ 15u]="parameter_list"; static unsigned char _temp2092[ 22u]="parameter_declaration";
static unsigned char _temp2095[ 16u]="identifier_list"; static unsigned char
_temp2098[ 17u]="identifier_list0"; static unsigned char _temp2101[ 12u]="initializer";
static unsigned char _temp2104[ 18u]="array_initializer"; static unsigned char
_temp2107[ 17u]="initializer_list"; static unsigned char _temp2110[ 12u]="designation";
static unsigned char _temp2113[ 16u]="designator_list"; static unsigned char
_temp2116[ 11u]="designator"; static unsigned char _temp2119[ 10u]="type_name";
static unsigned char _temp2122[ 14u]="any_type_name"; static unsigned char
_temp2125[ 15u]="type_name_list"; static unsigned char _temp2128[ 20u]="abstract_declarator";
static unsigned char _temp2131[ 27u]="direct_abstract_declarator"; static
unsigned char _temp2134[ 10u]="statement"; static unsigned char _temp2137[ 18u]="labeled_statement";
static unsigned char _temp2140[ 21u]="expression_statement"; static
unsigned char _temp2143[ 19u]="compound_statement"; static unsigned char
_temp2146[ 16u]="block_item_list"; static unsigned char _temp2149[ 11u]="block_item";
static unsigned char _temp2152[ 20u]="selection_statement"; static unsigned char
_temp2155[ 15u]="switch_clauses"; static unsigned char _temp2158[ 16u]="switchC_clauses";
static unsigned char _temp2161[ 20u]="iteration_statement"; static unsigned char
_temp2164[ 15u]="jump_statement"; static unsigned char _temp2167[ 8u]="pattern";
static unsigned char _temp2170[ 19u]="tuple_pattern_list"; static unsigned char
_temp2173[ 20u]="tuple_pattern_list0"; static unsigned char _temp2176[ 14u]="field_pattern";
static unsigned char _temp2179[ 19u]="field_pattern_list"; static unsigned char
_temp2182[ 20u]="field_pattern_list0"; static unsigned char _temp2185[ 11u]="expression";
static unsigned char _temp2188[ 22u]="assignment_expression"; static
unsigned char _temp2191[ 20u]="assignment_operator"; static unsigned char
_temp2194[ 23u]="conditional_expression"; static unsigned char _temp2197[ 20u]="constant_expression";
static unsigned char _temp2200[ 22u]="logical_or_expression"; static
unsigned char _temp2203[ 23u]="logical_and_expression"; static unsigned char
_temp2206[ 24u]="inclusive_or_expression"; static unsigned char _temp2209[ 24u]="exclusive_or_expression";
static unsigned char _temp2212[ 15u]="and_expression"; static unsigned char
_temp2215[ 20u]="equality_expression"; static unsigned char _temp2218[ 22u]="relational_expression";
static unsigned char _temp2221[ 17u]="shift_expression"; static unsigned char
_temp2224[ 20u]="additive_expression"; static unsigned char _temp2227[ 26u]="multiplicative_expression";
static unsigned char _temp2230[ 16u]="cast_expression"; static unsigned char
_temp2233[ 17u]="unary_expression"; static unsigned char _temp2236[ 14u]="format_primop";
static unsigned char _temp2239[ 15u]="unary_operator"; static unsigned char
_temp2242[ 19u]="postfix_expression"; static unsigned char _temp2245[ 19u]="primary_expression";
static unsigned char _temp2248[ 25u]="argument_expression_list"; static
unsigned char _temp2251[ 26u]="argument_expression_list0"; static unsigned char
_temp2254[ 9u]="constant"; static unsigned char _temp2257[ 20u]="qual_opt_identifier";
static struct _tagged_string Cyc_yytname[ 235u]={{ _temp1555, _temp1555,
_temp1555 + 2u},{ _temp1558, _temp1558, _temp1558 + 6u},{ _temp1561, _temp1561,
_temp1561 + 12u},{ _temp1564, _temp1564, _temp1564 + 5u},{ _temp1567, _temp1567,
_temp1567 + 9u},{ _temp1570, _temp1570, _temp1570 + 7u},{ _temp1573, _temp1573,
_temp1573 + 7u},{ _temp1576, _temp1576, _temp1576 + 8u},{ _temp1579, _temp1579,
_temp1579 + 5u},{ _temp1582, _temp1582, _temp1582 + 5u},{ _temp1585, _temp1585,
_temp1585 + 6u},{ _temp1588, _temp1588, _temp1588 + 4u},{ _temp1591, _temp1591,
_temp1591 + 5u},{ _temp1594, _temp1594, _temp1594 + 6u},{ _temp1597, _temp1597,
_temp1597 + 7u},{ _temp1600, _temp1600, _temp1600 + 7u},{ _temp1603, _temp1603,
_temp1603 + 9u},{ _temp1606, _temp1606, _temp1606 + 6u},{ _temp1609, _temp1609,
_temp1609 + 9u},{ _temp1612, _temp1612, _temp1612 + 9u},{ _temp1615, _temp1615,
_temp1615 + 7u},{ _temp1618, _temp1618, _temp1618 + 6u},{ _temp1621, _temp1621,
_temp1621 + 5u},{ _temp1624, _temp1624, _temp1624 + 8u},{ _temp1627, _temp1627,
_temp1627 + 7u},{ _temp1630, _temp1630, _temp1630 + 3u},{ _temp1633, _temp1633,
_temp1633 + 5u},{ _temp1636, _temp1636, _temp1636 + 7u},{ _temp1639, _temp1639,
_temp1639 + 6u},{ _temp1642, _temp1642, _temp1642 + 3u},{ _temp1645, _temp1645,
_temp1645 + 4u},{ _temp1648, _temp1648, _temp1648 + 5u},{ _temp1651, _temp1651,
_temp1651 + 9u},{ _temp1654, _temp1654, _temp1654 + 6u},{ _temp1657, _temp1657,
_temp1657 + 7u},{ _temp1660, _temp1660, _temp1660 + 7u},{ _temp1663, _temp1663,
_temp1663 + 5u},{ _temp1666, _temp1666, _temp1666 + 8u},{ _temp1669, _temp1669,
_temp1669 + 4u},{ _temp1672, _temp1672, _temp1672 + 6u},{ _temp1675, _temp1675,
_temp1675 + 4u},{ _temp1678, _temp1678, _temp1678 + 6u},{ _temp1681, _temp1681,
_temp1681 + 4u},{ _temp1684, _temp1684, _temp1684 + 9u},{ _temp1687, _temp1687,
_temp1687 + 9u},{ _temp1690, _temp1690, _temp1690 + 6u},{ _temp1693, _temp1693,
_temp1693 + 10u},{ _temp1696, _temp1696, _temp1696 + 7u},{ _temp1699, _temp1699,
_temp1699 + 8u},{ _temp1702, _temp1702, _temp1702 + 5u},{ _temp1705, _temp1705,
_temp1705 + 8u},{ _temp1708, _temp1708, _temp1708 + 4u},{ _temp1711, _temp1711,
_temp1711 + 7u},{ _temp1714, _temp1714, _temp1714 + 7u},{ _temp1717, _temp1717,
_temp1717 + 8u},{ _temp1720, _temp1720, _temp1720 + 8u},{ _temp1723, _temp1723,
_temp1723 + 6u},{ _temp1726, _temp1726, _temp1726 + 7u},{ _temp1729, _temp1729,
_temp1729 + 7u},{ _temp1732, _temp1732, _temp1732 + 7u},{ _temp1735, _temp1735,
_temp1735 + 9u},{ _temp1738, _temp1738, _temp1738 + 7u},{ _temp1741, _temp1741,
_temp1741 + 5u},{ _temp1744, _temp1744, _temp1744 + 8u},{ _temp1747, _temp1747,
_temp1747 + 7u},{ _temp1750, _temp1750, _temp1750 + 7u},{ _temp1753, _temp1753,
_temp1753 + 7u},{ _temp1756, _temp1756, _temp1756 + 8u},{ _temp1759, _temp1759,
_temp1759 + 9u},{ _temp1762, _temp1762, _temp1762 + 6u},{ _temp1765, _temp1765,
_temp1765 + 6u},{ _temp1768, _temp1768, _temp1768 + 6u},{ _temp1771, _temp1771,
_temp1771 + 6u},{ _temp1774, _temp1774, _temp1774 + 7u},{ _temp1777, _temp1777,
_temp1777 + 6u},{ _temp1780, _temp1780, _temp1780 + 11u},{ _temp1783, _temp1783,
_temp1783 + 11u},{ _temp1786, _temp1786, _temp1786 + 11u},{ _temp1789, _temp1789,
_temp1789 + 11u},{ _temp1792, _temp1792, _temp1792 + 11u},{ _temp1795, _temp1795,
_temp1795 + 12u},{ _temp1798, _temp1798, _temp1798 + 13u},{ _temp1801, _temp1801,
_temp1801 + 11u},{ _temp1804, _temp1804, _temp1804 + 11u},{ _temp1807, _temp1807,
_temp1807 + 10u},{ _temp1810, _temp1810, _temp1810 + 9u},{ _temp1813, _temp1813,
_temp1813 + 11u},{ _temp1816, _temp1816, _temp1816 + 12u},{ _temp1819, _temp1819,
_temp1819 + 11u},{ _temp1822, _temp1822, _temp1822 + 17u},{ _temp1825, _temp1825,
_temp1825 + 7u},{ _temp1828, _temp1828, _temp1828 + 19u},{ _temp1831, _temp1831,
_temp1831 + 18u},{ _temp1834, _temp1834, _temp1834 + 9u},{ _temp1837, _temp1837,
_temp1837 + 16u},{ _temp1840, _temp1840, _temp1840 + 18u},{ _temp1843, _temp1843,
_temp1843 + 10u},{ _temp1846, _temp1846, _temp1846 + 4u},{ _temp1849, _temp1849,
_temp1849 + 4u},{ _temp1852, _temp1852, _temp1852 + 4u},{ _temp1855, _temp1855,
_temp1855 + 4u},{ _temp1858, _temp1858, _temp1858 + 4u},{ _temp1861, _temp1861,
_temp1861 + 4u},{ _temp1864, _temp1864, _temp1864 + 4u},{ _temp1867, _temp1867,
_temp1867 + 4u},{ _temp1870, _temp1870, _temp1870 + 4u},{ _temp1873, _temp1873,
_temp1873 + 4u},{ _temp1876, _temp1876, _temp1876 + 4u},{ _temp1879, _temp1879,
_temp1879 + 4u},{ _temp1882, _temp1882, _temp1882 + 4u},{ _temp1885, _temp1885,
_temp1885 + 4u},{ _temp1888, _temp1888, _temp1888 + 4u},{ _temp1891, _temp1891,
_temp1891 + 4u},{ _temp1894, _temp1894, _temp1894 + 4u},{ _temp1897, _temp1897,
_temp1897 + 4u},{ _temp1900, _temp1900, _temp1900 + 4u},{ _temp1903, _temp1903,
_temp1903 + 4u},{ _temp1906, _temp1906, _temp1906 + 4u},{ _temp1909, _temp1909,
_temp1909 + 4u},{ _temp1912, _temp1912, _temp1912 + 4u},{ _temp1915, _temp1915,
_temp1915 + 4u},{ _temp1918, _temp1918, _temp1918 + 4u},{ _temp1921, _temp1921,
_temp1921 + 4u},{ _temp1924, _temp1924, _temp1924 + 4u},{ _temp1927, _temp1927,
_temp1927 + 5u},{ _temp1930, _temp1930, _temp1930 + 17u},{ _temp1933, _temp1933,
_temp1933 + 21u},{ _temp1936, _temp1936, _temp1936 + 20u},{ _temp1939, _temp1939,
_temp1939 + 21u},{ _temp1942, _temp1942, _temp1942 + 13u},{ _temp1945, _temp1945,
_temp1945 + 15u},{ _temp1948, _temp1948, _temp1948 + 17u},{ _temp1951, _temp1951,
_temp1951 + 19u},{ _temp1954, _temp1954, _temp1954 + 12u},{ _temp1957, _temp1957,
_temp1957 + 17u},{ _temp1960, _temp1960, _temp1960 + 23u},{ _temp1963, _temp1963,
_temp1963 + 24u},{ _temp1966, _temp1966, _temp1966 + 15u},{ _temp1969, _temp1969,
_temp1969 + 11u},{ _temp1972, _temp1972, _temp1972 + 15u},{ _temp1975, _temp1975,
_temp1975 + 10u},{ _temp1978, _temp1978, _temp1978 + 15u},{ _temp1981, _temp1981,
_temp1981 + 5u},{ _temp1984, _temp1984, _temp1984 + 15u},{ _temp1987, _temp1987,
_temp1987 + 15u},{ _temp1990, _temp1990, _temp1990 + 11u},{ _temp1993, _temp1993,
_temp1993 + 22u},{ _temp1996, _temp1996, _temp1996 + 26u},{ _temp1999, _temp1999,
_temp1999 + 16u},{ _temp2002, _temp2002, _temp2002 + 16u},{ _temp2005, _temp2005,
_temp2005 + 24u},{ _temp2008, _temp2008, _temp2008 + 25u},{ _temp2011, _temp2011,
_temp2011 + 21u},{ _temp2014, _temp2014, _temp2014 + 22u},{ _temp2017, _temp2017,
_temp2017 + 16u},{ _temp2020, _temp2020, _temp2020 + 19u},{ _temp2023, _temp2023,
_temp2023 + 25u},{ _temp2026, _temp2026, _temp2026 + 23u},{ _temp2029, _temp2029,
_temp2029 + 24u},{ _temp2032, _temp2032, _temp2032 + 18u},{ _temp2035, _temp2035,
_temp2035 + 17u},{ _temp2038, _temp2038, _temp2038 + 18u},{ _temp2041, _temp2041,
_temp2041 + 17u},{ _temp2044, _temp2044, _temp2044 + 18u},{ _temp2047, _temp2047,
_temp2047 + 12u},{ _temp2050, _temp2050, _temp2050 + 11u},{ _temp2053, _temp2053,
_temp2053 + 18u},{ _temp2056, _temp2056, _temp2056 + 8u},{ _temp2059, _temp2059,
_temp2059 + 13u},{ _temp2062, _temp2062, _temp2062 + 8u},{ _temp2065, _temp2065,
_temp2065 + 4u},{ _temp2068, _temp2068, _temp2068 + 20u},{ _temp2071, _temp2071,
_temp2071 + 20u},{ _temp2074, _temp2074, _temp2074 + 16u},{ _temp2077, _temp2077,
_temp2077 + 16u},{ _temp2080, _temp2080, _temp2080 + 11u},{ _temp2083, _temp2083,
_temp2083 + 14u},{ _temp2086, _temp2086, _temp2086 + 11u},{ _temp2089, _temp2089,
_temp2089 + 15u},{ _temp2092, _temp2092, _temp2092 + 22u},{ _temp2095, _temp2095,
_temp2095 + 16u},{ _temp2098, _temp2098, _temp2098 + 17u},{ _temp2101, _temp2101,
_temp2101 + 12u},{ _temp2104, _temp2104, _temp2104 + 18u},{ _temp2107, _temp2107,
_temp2107 + 17u},{ _temp2110, _temp2110, _temp2110 + 12u},{ _temp2113, _temp2113,
_temp2113 + 16u},{ _temp2116, _temp2116, _temp2116 + 11u},{ _temp2119, _temp2119,
_temp2119 + 10u},{ _temp2122, _temp2122, _temp2122 + 14u},{ _temp2125, _temp2125,
_temp2125 + 15u},{ _temp2128, _temp2128, _temp2128 + 20u},{ _temp2131, _temp2131,
_temp2131 + 27u},{ _temp2134, _temp2134, _temp2134 + 10u},{ _temp2137, _temp2137,
_temp2137 + 18u},{ _temp2140, _temp2140, _temp2140 + 21u},{ _temp2143, _temp2143,
_temp2143 + 19u},{ _temp2146, _temp2146, _temp2146 + 16u},{ _temp2149, _temp2149,
_temp2149 + 11u},{ _temp2152, _temp2152, _temp2152 + 20u},{ _temp2155, _temp2155,
_temp2155 + 15u},{ _temp2158, _temp2158, _temp2158 + 16u},{ _temp2161, _temp2161,
_temp2161 + 20u},{ _temp2164, _temp2164, _temp2164 + 15u},{ _temp2167, _temp2167,
_temp2167 + 8u},{ _temp2170, _temp2170, _temp2170 + 19u},{ _temp2173, _temp2173,
_temp2173 + 20u},{ _temp2176, _temp2176, _temp2176 + 14u},{ _temp2179, _temp2179,
_temp2179 + 19u},{ _temp2182, _temp2182, _temp2182 + 20u},{ _temp2185, _temp2185,
_temp2185 + 11u},{ _temp2188, _temp2188, _temp2188 + 22u},{ _temp2191, _temp2191,
_temp2191 + 20u},{ _temp2194, _temp2194, _temp2194 + 23u},{ _temp2197, _temp2197,
_temp2197 + 20u},{ _temp2200, _temp2200, _temp2200 + 22u},{ _temp2203, _temp2203,
_temp2203 + 23u},{ _temp2206, _temp2206, _temp2206 + 24u},{ _temp2209, _temp2209,
_temp2209 + 24u},{ _temp2212, _temp2212, _temp2212 + 15u},{ _temp2215, _temp2215,
_temp2215 + 20u},{ _temp2218, _temp2218, _temp2218 + 22u},{ _temp2221, _temp2221,
_temp2221 + 17u},{ _temp2224, _temp2224, _temp2224 + 20u},{ _temp2227, _temp2227,
_temp2227 + 26u},{ _temp2230, _temp2230, _temp2230 + 16u},{ _temp2233, _temp2233,
_temp2233 + 17u},{ _temp2236, _temp2236, _temp2236 + 14u},{ _temp2239, _temp2239,
_temp2239 + 15u},{ _temp2242, _temp2242, _temp2242 + 19u},{ _temp2245, _temp2245,
_temp2245 + 19u},{ _temp2248, _temp2248, _temp2248 + 25u},{ _temp2251, _temp2251,
_temp2251 + 26u},{ _temp2254, _temp2254, _temp2254 + 9u},{ _temp2257, _temp2257,
_temp2257 + 20u}}; static short Cyc_yyr1[ 405u]={ 0, 124, 125, 125, 125, 125,
125, 125, 125, 126, 126, 127, 127, 127, 127, 128, 128, 129, 130, 131, 132, 133,
133, 133, 133, 134, 134, 135, 135, 135, 135, 135, 135, 135, 135, 136, 136, 136,
136, 136, 136, 136, 137, 137, 138, 139, 139, 140, 140, 140, 140, 141, 141, 141,
141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 142,
142, 143, 143, 143, 144, 144, 145, 145, 146, 146, 147, 147, 147, 147, 147, 148,
148, 148, 149, 149, 150, 151, 151, 152, 153, 153, 154, 154, 155, 156, 156, 156,
156, 157, 158, 158, 159, 159, 159, 160, 160, 160, 160, 161, 161, 162, 162, 162,
162, 163, 163, 163, 164, 164, 165, 165, 166, 166, 166, 166, 166, 166, 166, 166,
166, 166, 166, 167, 167, 167, 167, 168, 168, 168, 168, 168, 169, 169, 170, 170,
170, 171, 171, 172, 172, 172, 172, 173, 173, 174, 174, 175, 175, 176, 176, 176,
176, 177, 177, 177, 177, 178, 178, 179, 179, 179, 180, 181, 181, 182, 182, 183,
183, 183, 183, 184, 184, 184, 184, 185, 186, 186, 187, 187, 188, 188, 189, 189,
189, 189, 190, 190, 191, 191, 191, 192, 192, 192, 192, 192, 192, 192, 192, 192,
192, 192, 192, 192, 192, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 194,
195, 195, 196, 196, 197, 197, 198, 198, 198, 199, 199, 199, 199, 199, 200, 200,
200, 200, 200, 200, 201, 201, 201, 201, 202, 202, 202, 202, 202, 202, 202, 202,
202, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 203, 203, 204, 204,
204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 205, 205, 206, 206,
207, 207, 208, 209, 209, 210, 210, 211, 211, 212, 212, 212, 212, 212, 212, 212,
212, 212, 212, 212, 213, 213, 213, 213, 213, 213, 213, 214, 215, 215, 216, 216,
217, 217, 218, 218, 219, 219, 220, 220, 220, 221, 221, 221, 221, 221, 222, 222,
222, 223, 223, 223, 224, 224, 224, 224, 225, 225, 226, 226, 226, 226, 226, 226,
226, 226, 226, 226, 226, 226, 227, 227, 227, 227, 227, 227, 228, 228, 228, 229,
229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 230, 230, 230,
230, 230, 230, 230, 230, 230, 231, 232, 232, 233, 233, 233, 233, 234, 234};
static short Cyc_yyr2[ 405u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4,
2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1,
6, 1, 3, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 4, 4, 1, 1,
1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3,
3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 3, 4, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1,
2, 1, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 3, 1, 1,
2, 2, 4, 5, 7, 0, 2, 0, 1, 1, 3, 2, 3, 1, 3, 1, 3, 3, 5, 1, 3, 2, 1, 2, 1, 1, 3,
1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 3, 1, 3, 1, 1, 2, 3,
2, 3, 3, 4, 2, 4, 3, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2,
3, 1, 2, 1, 1, 1, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8,
7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4,
5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1,
3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 4, 7, 9, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4,
5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 782u]={ 8, 35, 36, 37,
38, 40, 51, 53, 54, 55, 56, 57, 58, 59, 60, 71, 72, 73, 88, 89, 42, 0, 0, 41, 0,
0, 113, 114, 0, 403, 64, 404, 85, 0, 52, 0, 141, 142, 145, 1, 8, 9, 0, 0, 10, 0,
42, 42, 42, 61, 62, 0, 63, 0, 0, 124, 0, 146, 126, 39, 0, 33, 43, 75, 282, 403,
278, 281, 280, 0, 276, 0, 0, 0, 0, 0, 176, 0, 283, 17, 19, 0, 0, 0, 66, 0, 0, 0,
0, 2, 8, 8, 8, 8, 21, 0, 93, 94, 96, 27, 29, 31, 85, 0, 85, 148, 150, 0, 85, 38,
0, 25, 0, 0, 11, 0, 0, 0, 136, 125, 42, 147, 8, 0, 34, 0, 0, 290, 289, 279, 288,
24, 0, 0, 0, 0, 42, 42, 194, 196, 0, 69, 70, 65, 200, 0, 127, 174, 0, 171, 0,
402, 0, 0, 0, 0, 364, 365, 366, 367, 368, 369, 0, 0, 0, 0, 0, 399, 389, 400, 401,
0, 0, 0, 0, 372, 0, 370, 371, 0, 301, 314, 322, 324, 326, 328, 330, 332, 335,
340, 343, 346, 350, 0, 0, 352, 373, 388, 387, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28,
30, 32, 84, 0, 90, 91, 0, 83, 0, 85, 0, 110, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 403, 230, 232, 0, 238, 236, 0, 237, 219, 220, 221, 0, 234, 222, 223,
224, 0, 299, 26, 13, 96, 0, 177, 0, 131, 0, 157, 0, 0, 128, 0, 137, 0, 0, 78, 0,
76, 277, 292, 0, 291, 178, 0, 0, 290, 167, 197, 0, 99, 101, 0, 0, 202, 195, 203,
68, 0, 87, 0, 86, 0, 173, 202, 175, 67, 0, 0, 360, 316, 350, 0, 317, 318, 0, 0,
0, 0, 0, 0, 353, 354, 0, 0, 0, 0, 356, 357, 355, 143, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313,
303, 0, 0, 358, 0, 381, 382, 0, 0, 0, 391, 0, 0, 144, 18, 8, 20, 8, 95, 97, 180,
179, 14, 0, 80, 92, 0, 0, 103, 104, 106, 0, 149, 111, 85, 119, 0, 0, 0, 0, 0, 0,
0, 269, 270, 271, 0, 0, 273, 0, 227, 228, 0, 0, 0, 0, 96, 233, 235, 231, 0, 159,
165, 0, 0, 161, 130, 0, 0, 153, 133, 135, 134, 129, 151, 139, 138, 8, 48, 47, 0,
45, 0, 74, 0, 285, 0, 23, 286, 0, 0, 0, 0, 190, 294, 297, 0, 296, 0, 0, 0, 198,
100, 102, 0, 210, 0, 0, 206, 0, 204, 0, 0, 0, 218, 199, 201, 172, 0, 0, 181, 185,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 390, 397, 0, 396, 323, 0, 325, 327, 329, 331,
333, 334, 338, 339, 336, 337, 341, 342, 344, 345, 347, 348, 349, 302, 0, 379,
380, 375, 0, 377, 378, 0, 0, 0, 4, 6, 0, 321, 107, 98, 0, 0, 0, 112, 121, 120, 0,
0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 268, 272, 0, 0, 0, 226, 0, 229, 0, 15, 300, 160,
0, 0, 163, 0, 132, 0, 158, 157, 152, 140, 7, 0, 0, 0, 79, 77, 293, 193, 0, 295,
189, 191, 287, 0, 284, 169, 168, 0, 212, 205, 208, 0, 213, 0, 0, 207, 0, 359, 0,
182, 0, 186, 385, 386, 0, 0, 0, 0, 0, 0, 0, 351, 393, 0, 0, 361, 376, 374, 394,
0, 0, 82, 105, 108, 81, 109, 122, 119, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 244,
274, 0, 0, 16, 157, 166, 164, 162, 159, 154, 0, 0, 44, 46, 192, 298, 0, 211, 0,
215, 217, 216, 209, 0, 183, 187, 0, 0, 319, 320, 0, 395, 0, 398, 315, 392, 0,
118, 117, 239, 0, 244, 254, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 275, 0, 155, 0,
49, 50, 170, 214, 0, 188, 0, 0, 383, 0, 0, 0, 250, 0, 0, 256, 0, 0, 0, 264, 0, 0,
0, 0, 0, 0, 0, 0, 243, 225, 157, 0, 362, 0, 384, 0, 240, 0, 0, 0, 241, 255, 257,
258, 0, 266, 265, 0, 260, 0, 0, 0, 0, 244, 245, 156, 0, 0, 123, 0, 0, 242, 259,
267, 261, 262, 0, 0, 244, 246, 184, 363, 250, 251, 263, 244, 247, 250, 252, 244,
248, 253, 249, 0, 0, 0}; static short Cyc_yydefgoto[ 111u]={ 779, 39, 40, 41,
241, 42, 370, 43, 372, 44, 207, 45, 46, 61, 62, 435, 436, 47, 143, 48, 49, 271,
272, 50, 84, 51, 213, 214, 95, 96, 97, 215, 147, 382, 383, 384, 52, 53, 534, 535,
536, 54, 55, 56, 57, 120, 107, 431, 461, 424, 557, 419, 420, 284, 263, 149, 75,
76, 476, 375, 477, 478, 447, 448, 139, 144, 145, 462, 291, 244, 245, 246, 247,
248, 249, 250, 691, 735, 251, 252, 275, 276, 277, 450, 451, 452, 253, 254, 356,
180, 526, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
195, 196, 490, 491, 197, 198}; static short Cyc_yypact[ 782u]={ 1807, - -32768,
- -32768, - -32768, 20, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - 16, 119, 483, - -32768, 119, - 6, - -32768, - -32768, 31, -
-32768, 90, - -32768, 73, 359, - -32768, 108, 162, 175, - -32768, - -32768, 1807,
- -32768, 309, 418, - -32768, 270, - 16, - 16, - 16, - -32768, - -32768, 160, -
-32768, 148, 2105, 237, 26, 15, - -32768, 197, 125, 2165, - -32768, 231, -
-32768, 193, - -32768, - -32768, - -32768, 549, - -32768, 138, 257, 273, 549,
263, 290, 274, 271, - -32768, - -32768, 2417, 61, 2417, - -32768, 284, 2484,
3906, 3906, - -32768, 1807, 1807, 1807, 1807, - -32768, 326, 313, - -32768, 2042,
2165, 2165, 2165, 73, 2484, 73, 355, - -32768, 119, 369, 387, 1212, - -32768,
2105, 270, - -32768, 2305, 2417, 2770, - -32768, 237, - 16, - -32768, 1807, 348,
- -32768, 119, 378, 549, - -32768, - -32768, - -32768, - -32768, 395, 3906, 366,
228, - 16, - 16, 227, - -32768, 152, - -32768, - -32768, - -32768, 373, 41, -
-32768, 206, 428, - -32768, 4119, - -32768, 3906, 3977, 397, 399, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, 411, 420, 423, 4190, 4190, -
-32768, - -32768, - -32768, - -32768, 1570, 425, 4261, 4261, - -32768, 4261, -
-32768, - -32768, 394, - -32768, - 11, 446, 410, 414, 422, 464, 62, 470, 426, 63,
- -32768, 692, 445, 4261, 136, - -32768, - -32768, - 37, 451, - -32768, 458, -
-32768, 461, - -32768, 359, 2841, 2105, - -32768, - -32768, - -32768, - -32768,
471, 486, 2484, - -32768, 307, 493, 61, 73, 119, 495, - -32768, 475, 60, 502,
2189, 506, 530, 536, 539, 2912, 2189, 128, 2189, 2189, 23, 513, - -32768, -
-32768, 538, - -32768, - -32768, 270, - -32768, - -32768, - -32768, - -32768,
545, 1333, - -32768, - -32768, - -32768, 276, - -32768, - -32768, - -32768, 557,
15, - -32768, 283, - -32768, 547, 293, 562, 67, - -32768, 556, 53, 569, 18, 567,
573, 575, - -32768, - -32768, 576, 574, - -32768, 297, 964, 549, 35, - -32768,
582, 2484, 2484, 1982, 2983, - 20, - -32768, 253, - -32768, 283, - -32768, 2417,
- -32768, 1922, - -32768, 120, - -32768, - -32768, 2484, 1570, - -32768, -
-32768, - -32768, 802, - -32768, 608, 3906, 1867, 648, 3906, 3906, 1570, -
-32768, - -32768, 1333, 583, 441, 3906, - -32768, - -32768, - -32768, - -32768,
4261, 3906, 4261, 4261, 4261, 4261, 4261, 4261, 4261, 4261, 4261, 4261, 4261,
4261, 4261, 4261, 4261, 4261, 4261, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 3906, 3906,
- -32768, 155, - -32768, - -32768, 3054, 213, 3906, - -32768, 2681, 578, -
-32768, - -32768, 1807, - -32768, 1807, - -32768, - -32768, - -32768, - -32768,
- -32768, 2484, - -32768, - -32768, 3906, 590, 585, - -32768, 581, 2484, -
-32768, - -32768, 73, 543, 3906, 589, 3906, 3906, 663, 1454, 596, - -32768, -
-32768, - -32768, 301, 653, - -32768, 3125, - -32768, - -32768, 2189, 603, 2189,
1686, 2042, - -32768, - -32768, - -32768, 3906, 610, 613, 232, 599, 588, -
-32768, 283, 2431, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, 53, 1807, - -32768, 609, 602, 606, 119, - -32768, 3906, - -32768, 549, -
-32768, - -32768, 623, 3906, 549, 42, - -32768, - -32768, - -32768, 614, 612,
617, 61, 619, - -32768, - -32768, - -32768, 283, - -32768, 618, 621, - -32768,
605, 253, 2319, 2417, 3196, - -32768, - -32768, 373, - -32768, 622, 637, -
-32768, - -32768, 286, 2841, 450, 626, 359, 629, 452, 628, 630, 634, 4048, -
-32768, - -32768, 632, 633, 446, 294, 410, 414, 422, 464, 62, 62, 470, 470, 470,
470, 426, 426, 63, 63, - -32768, - -32768, - -32768, - -32768, 639, - -32768, -
-32768, - -32768, 640, - -32768, - -32768, 181, 358, 2417, - -32768, - -32768,
636, - -32768, - -32768, - -32768, 307, 3906, 644, - -32768, - -32768, - -32768,
645, 119, 314, 460, 3906, 462, 476, 638, 3267, 3338, 327, - -32768, - -32768,
647, 650, 646, - -32768, 642, - -32768, 2105, - -32768, - -32768, - -32768, 2484,
61, - -32768, 651, - -32768, 283, - -32768, 99, - -32768, - -32768, - -32768,
477, 649, 18, - -32768, - -32768, - -32768, - -32768, 643, - -32768, - -32768, -
-32768, - -32768, 4297, - -32768, 652, - -32768, 654, - -32768, - -32768, -
-32768, 283, - -32768, 657, 91, - -32768, 655, 662, 659, - -32768, 2503, -
-32768, - -32768, - -32768, 2105, 2484, 3977, 718, 662, 661, 2681, - -32768, -
-32768, 3906, 3906, - -32768, - -32768, - -32768, - -32768, 2681, 382, - -32768,
- -32768, - -32768, - -32768, - -32768, 308, 46, 543, 2189, 478, 679, 2189, 3906,
3409, 330, 3480, 331, 3551, 560, - -32768, 681, 691, - -32768, 683, - -32768, -
-32768, - -32768, 610, - -32768, 680, 682, - -32768, - -32768, - -32768, -
-32768, 619, - -32768, 684, - -32768, - -32768, - -32768, - -32768, 3906, -
-32768, - -32768, 2841, 685, - -32768, 608, 688, - -32768, 391, - -32768, -
-32768, - -32768, 693, - -32768, - -32768, 755, 697, 560, - -32768, 494, 2189,
499, 3622, 2189, 503, 3693, 3764, 335, 549, 675, 698, - -32768, 2189, - -32768,
2484, - -32768, - -32768, - -32768, - -32768, 332, - -32768, 694, 2484, - -32768,
2592, 2484, 2189, 587, 699, 702, - -32768, 2189, 2189, 509, - -32768, 2189, 2189,
512, 2189, 514, 3835, - 8, 1333, - -32768, - -32768, 683, 3906, - -32768, 700, -
-32768, 522, - -32768, 3906, 695, 701, - -32768, - -32768, - -32768, - -32768,
2189, - -32768, - -32768, 2189, - -32768, 2189, 2189, 524, 3906, 928, - -32768,
- -32768, 392, 703, - -32768, 340, 1333, - -32768, - -32768, - -32768, - -32768,
- -32768, 2189, 342, 560, - -32768, - -32768, - -32768, 1091, - -32768, - -32768,
928, - -32768, 587, - -32768, 560, - -32768, - -32768, - -32768, 801, 804, -
-32768}; static short Cyc_yypgoto[ 111u]={ - -32768, 141, - -32768, 498, -
-32768, - -32768, - -32768, - -32768, - -32768, - 50, - 33, - 54, - -32768, 51,
- 26, 236, - -32768, 2, - 193, - 75, - -32768, - -32768, 390, - -32768, - 65, -
-32768, - 76, - -32768, - -32768, - -32768, 624, 616, - 80, - -32768, - -32768,
300, - -32768, - -32768, 5, - -32768, - -32768, - 19, - 14, 25, - -32768, -
-32768, - 39, - -32768, - 91, - 537, 186, - 384, 540, - 387, - 84, - 282, 719, -
-32768, - 187, - 148, - 351, - 246, - -32768, 389, - 139, - 51, - 99, 44, - 212,
45, - -32768, - -32768, - 45, - 239, - -32768, - -32768, - 628, - 310, - -32768,
- -32768, - 10, 559, - -32768, 258, - -32768, - -32768, - 41, 130, - -32768, -
130, - 344, - 137, 516, 515, 517, 518, 519, 319, 170, 321, 322, - 61, 482, -
-32768, - -32768, - -32768, - -32768, - 288, - -32768, - -32768, 33}; static
short Cyc_yytable[ 4415u]={ 113, 138, 148, 138, 111, 308, 137, 124, 137, 114,
413, 137, 77, 134, 85, 520, 309, 265, 121, 374, 472, 112, 305, 216, 262, 387, 98,
646, 137, 118, 140, 560, 319, 58, 446, 433, 138, 212, 563, 217, 137, 137, 119,
221, 113, 209, 210, 211, 111, 365, 709, 532, 533, 208, 63, 78, 243, 79, 113, 126,
242, 366, 255, 326, 130, 748, 58, 256, 583, 512, 15, 16, 17, 58, 516, 584, 367,
465, 58, 486, 60, 287, 80, 136, 104, 136, 108, 465, 136, 58, 288, 138, 279, 118,
257, 572, 137, 99, 100, 101, 749, 575, 78, 327, 694, 136, 434, 78, 105, 294, 59,
407, 322, 323, 29, 324, 549, 136, 136, 106, 31, 765, 454, 58, 58, 58, 58, 33,
298, 408, 320, 334, 335, 358, 216, 426, 772, 81, 455, 137, 219, 472, 577, 776,
295, - 116, 58, 778, 296, 141, 392, 444, 445, 113, 388, 58, 142, 255, 273, 657,
78, 393, 377, 289, 473, 36, 37, 38, 336, 337, 295, 268, 299, 136, 427, 342, 485,
82, 644, 83, 58, 89, 290, 343, 344, 620, 257, 285, 286, 751, 401, 300, 105, 429,
295, 243, 422, 385, 658, 242, 359, 360, 361, 106, 655, 457, 458, 29, 29, 86, 137,
137, 137, 31, 31, 138, 136, 179, 199, 416, 137, 297, 137, 138, 411, 403, 123,
137, 137, 404, 288, 200, 201, 202, 203, 138, 29, 362, 58, 127, 137, 105, 31, 513,
471, 363, 364, 267, 29, 58, 514, 525, 106, 389, 31, 102, 669, 481, 103, 292, 87,
582, 320, 269, 243, 469, 698, 293, 242, 479, 449, 395, 483, 88, 320, 641, 58,
402, 85, 405, 406, 508, 509, 510, 415, 119, 493, 136, 136, 136, - 177, 598, 614,
430, 29, 122, - 177, 136, 216, 136, 31, 517, 524, 137, 136, 136, 216, 297, 518,
525, 530, 137, 289, 78, 78, 525, 288, 136, 36, 37, 38, 282, 299, 519, 531, 282,
148, 283, 287, 125, 58, 559, 58, 60, 446, 137, 376, 288, 115, 36, 37, 38, 113,
116, 58, 128, 543, 117, 137, 60, 537, 663, 539, 540, 466, 544, 565, 113, 29, 467,
131, 111, 129, 468, 31, 642, 554, 94, 591, - 85, 663, 33, - 85, 414, 133, 590,
417, 83, 553, 415, 136, 418, 36, 37, 38, 596, 146, 138, 136, 597, 422, 137, 137,
132, 442, 29, 423, 415, 546, 525, 415, 31, 611, 58, 415, 58, 90, 91, 33, - 85,
662, 624, 136, 726, 83, 381, 205, 625, 464, 36, 37, 38, 617, 204, 635, 136, 608,
683, 686, 662, 415, 573, 721, 415, 415, 415, 576, 376, 415, 469, 727, 138, 218,
415, 484, 415, 137, 29, 768, 270, 771, 489, 550, 31, 552, 665, 566, 615, 774,
663, 33, 616, 601, 777, 280, 58, 666, 281, 136, 136, 273, 36, 37, 38, 78, 83,
701, 222, 220, 78, 274, 671, 137, 278, 750, 295, 511, 489, 293, 672, 704, 766,
489, 325, 705, 415, 376, 627, 310, 113, 311, 632, 634, 255, 500, 501, 502, 503,
640, 385, 764, 522, 312, 523, 58, 92, 93, 769, 662, 328, 64, 313, 664, 136, 314,
645, 321, 137, 329, 773, 301, 302, 775, 330, 489, 332, 333, 338, 339, 331, 489,
340, 341, 488, 415, 555, 357, 113, 532, 533, 368, 111, 599, 415, 603, 415, 208,
369, 673, 136, 371, 58, 626, 415, 628, 415, 647, 648, 623, 378, 449, 65, 66, 567,
67, 68, 391, 31, 629, 415, 677, 415, 689, 690, 69, 379, 64, 70, 71, 680, 682,
386, 685, 390, 688, 72, 710, 415, 593, 73, 74, 712, 415, 394, 136, 716, 415, 396,
376, 733, 734, 740, 415, 78, 743, 415, 745, 415, 397, 700, 137, 409, 731, 729,
754, 302, 762, 415, 137, 674, 675, 137, 304, 398, 306, 306, 399, 29, 66, 410, 67,
68, 714, 31, 412, 718, 720, 316, 317, 421, 69, 498, 499, 70, 71, 306, 306, 206,
306, 504, 505, 72, 506, 507, 425, 73, 74, 428, 432, 243, 437, 676, 438, 242, 679,
439, 306, 441, 440, 722, 747, 456, 326, 482, 521, 487, 752, 527, 528, 529, 538,
541, 755, 545, 547, 243, 551, 136, 556, 242, 558, 561, 243, 562, 569, 136, 242,
763, 136, 570, 568, 574, 282, 579, 243, 580, 587, 243, 242, 581, 585, 242, 78,
586, 594, 595, 711, 376, 600, 715, 602, 604, 605, 606, 609, 618, 610, 376, 725,
630, 670, 612, 613, 621, 622, 636, 376, 637, 638, 639, 643, 649, 732, 667, 651,
653, 654, 738, 739, 656, 607, 741, 742, 668, 744, 660, 659, 345, 346, 347, 348,
349, 350, 351, 352, 353, 354, 678, 692, 693, 422, 707, 696, 723, 697, 758, 699,
702, 759, 703, 760, 761, 355, 376, 706, 708, 728, 724, 736, 737, 757, 780, 753,
756, 781, 767, 650, 770, 306, 480, 306, 306, 306, 306, 306, 306, 306, 306, 306,
306, 306, 306, 306, 306, 306, 306, 306, 571, 619, 373, 380, 695, 474, 470, 264,
376, 578, 150, 652, 151, 453, 152, 492, 494, 153, 0, 495, 0, 496, 0, 497, 154,
155, 0, 0, 156, 157, 158, 159, 160, 161, 162, 0, 306, 163, 164, 0, 165, 166, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169,
170, 0, 31, 0, 0, 0, 307, 475, 0, 171, 0, 0, 0, 172, 0, 0, 0, 444, 445, 0, 173,
0, 0, 174, 175, 176, 0, 306, 0, 0, 177, 178, 0, 306, 0, 0, 0, 1, 2, 3, 109, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 689, 690, 20, 223, 0, 224, 225,
226, 227, 228, 229, 230, 231, 150, 21, 151, 22, 152, 232, 306, 153, 23, 233, 0,
0, 26, 27, 154, 155, 234, 235, 156, 157, 158, 159, 160, 161, 162, 28, 236, 163,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 0, 0, 0,
0, 237, 167, 168, 169, 170, 30, 31, 32, 0, 238, 110, 0, 0, 171, 0, 0, 34, 240, 0,
0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 29, 66, 0, 67, 68,
0, 31, 0, 0, 0, 0, 443, 0, 69, 0, 0, 70, 71, 0, 0, 0, 444, 445, 0, 72, 0, 0, 0,
73, 74, 0, 0, 0, 306, 0, 0, 0, 0, 0, 0, 0, 306, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 733, 734, 20, 223, 0, 224, 225, 226, 227,
228, 229, 230, 231, 150, 21, 151, 22, 152, 232, 0, 153, 23, 233, 0, 0, 26, 27,
154, 155, 234, 235, 156, 157, 158, 159, 160, 161, 162, 28, 236, 163, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 237, 167,
168, 169, 170, 30, 31, 32, 0, 238, 110, 0, 0, 171, 0, 0, 34, 240, 0, 0, 0, 0, 0,
0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 1, 2, 3, 109, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 223, 0, 224, 225, 226, 227,
228, 229, 230, 231, 150, 21, 151, 22, 152, 232, 0, 153, 23, 233, 0, 0, 26, 27,
154, 155, 234, 235, 156, 157, 158, 159, 160, 161, 162, 28, 236, 163, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 237, 167,
168, 169, 170, 30, 31, 32, 0, 238, 110, 239, 0, 171, 0, 0, 34, 240, 0, 0, 0, 0,
0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 1, 2, 3, 109, 5, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 223, 0, 224, 225, 226, 227,
228, 229, 230, 231, 150, 21, 151, 22, 152, 232, 0, 153, 23, 233, 0, 0, 26, 27,
154, 155, 234, 235, 156, 157, 158, 159, 160, 161, 162, 28, 236, 163, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 237, 167,
168, 169, 170, 30, 31, 32, 0, 238, 110, 0, 0, 171, 0, 0, 34, 240, 0, 0, 0, 0, 0,
0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 1, 2, 3, 109, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
150, 21, 151, 22, 152, 0, 0, 153, 23, 0, 0, 0, 26, 27, 154, 155, 0, 0, 156, 157,
158, 159, 160, 161, 162, 28, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 30, 31, 32, 0, 542,
0, 0, 0, 171, 0, 0, 34, 240, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0,
0, 177, 178, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 150, 21, 151, 0, 152, 0, 0, 153, 0, 0, 0, 0, 26, 27, 154,
155, 0, 0, 156, 157, 158, 159, 160, 161, 162, 28, 0, 163, 164, 0, 165, 166, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170,
30, 31, 32, 0, 0, 318, 0, 0, 171, 0, 0, 34, 240, 0, 0, 0, 0, 0, 0, 173, 0, 0,
174, 175, 176, 0, 0, 0, 0, 177, 178, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150, 21, 151, 0, 152, 0, 0, 153,
0, 0, 0, 0, 26, 27, 154, 155, 0, 0, 156, 157, 158, 159, 160, 161, 162, 28, 0,
163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 29, 167, 168, 169, 170, 30, 31, 32, 0, 0, 0, 0, 0, 171, 0, 0, 34, 240, 0,
0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 1, 2, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 21, 0, 22, 0, 0, 0, 0, 23, 0, 24, 25, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 28, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 0, 0, 20, 0, 0, 0, 29, 0, 0, 0, 0, 30, 31, 32, 21, 0, 0, 0, 0, 33, 0, 23,
34, 35, 0, 26, 27, 0, 0, 0, 36, 37, 38, 0, 0, 0, 0, 0, 28, 0, 0, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 0,
21, 0, 30, 31, 32, 0, 0, 0, 0, 0, 33, 26, 27, 34, 35, 0, 0, 0, 0, 0, 0, 36, 37,
38, 28, 0, 0, 0, 0, 0, 0, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
0, 0, 0, 258, 0, 0, 29, 0, 0, 0, 0, 30, 31, 32, 21, 459, 0, 0, 0, 297, 460, 0,
34, 35, 0, 26, 27, 0, 288, 0, 36, 37, 38, 0, 0, 0, 0, 0, 28, 0, 0, 1, 2, 3, 109,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 258, 0, 0, 0, 0,
0, 0, 0, 30, 0, 32, 21, 459, 22, 0, 0, 287, 460, 23, 34, 35, 0, 26, 27, 0, 288,
0, 36, 37, 38, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0, 0, 0, 0, 30, 0, 32, 0, 0,
110, 21, 206, 22, 0, 0, 34, 35, 23, 0, 0, 0, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 28, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 32, 21, 0, 110, 0, 0, 0, 0, 23, 34,
35, 0, 26, 27, 223, 0, 224, 225, 226, 227, 228, 229, 230, 231, 150, 28, 151, 0,
152, 232, 0, 153, 0, 233, 0, 0, 0, 0, 154, 155, 234, 235, 156, 157, 158, 159,
160, 161, 162, 0, 236, 163, 164, 0, 165, 166, 0, 0, 30, 0, 32, 0, 0, 0, 0, 0, 0,
0, 0, 34, 35, 0, 0, 0, 0, 0, 0, 237, 167, 168, 169, 170, 0, 31, 0, 0, 238, 110,
0, 0, 171, 0, 0, 0, 172, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0,
177, 178, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 27, 0, 21,
0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 258, 0, 0, 259, 0, 0, 0, 0, 30, 0, 32, 0, 260, 0, 258, 0,
0, 261, 0, 34, 35, 0, 30, 0, 32, 0, 588, 0, 0, 0, 0, 589, 0, 34, 35, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 27, 0, 21, 0, 0, 0, 0, 0, 0, 0,
0, 0, 28, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 30, 0, 32, 0, 0, 135, 564, 0, 0, 0, 21,
34, 35, 0, 30, 0, 32, 0, 0, 0, 0, 26, 27, 0, 0, 34, 35, 0, 150, 0, 151, 0, 152,
0, 28, 153, 0, 0, 0, 0, 0, 0, 154, 155, 0, 0, 156, 157, 158, 159, 160, 161, 162,
0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 30, 0, 32, 0, 0, 0, 0, 0, 0, 0,
0, 34, 35, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 307, 661, 0, 171, 0, 0, 0,
172, 0, 0, 0, 444, 445, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 150,
0, 151, 0, 152, 0, 0, 153, 0, 0, 0, 0, 0, 0, 154, 155, 0, 0, 156, 157, 158, 159,
160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 307, 730, 0,
171, 0, 0, 0, 172, 0, 0, 0, 444, 445, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0,
177, 178, 150, 0, 151, 0, 152, 0, 0, 153, 0, 0, 0, 0, 0, 0, 154, 155, 0, 0, 156,
157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0,
307, 0, 0, 171, 0, 0, 0, 172, 0, 0, 0, 444, 445, 0, 173, 0, 0, 174, 175, 176, 0,
0, 0, 0, 177, 178, 150, 0, 151, 0, 152, 0, 0, 153, 0, 0, 0, 0, 0, 0, 154, 155, 0,
0, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0,
0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 266, 173, 153, 0, 174,
175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 307, 0, 0, 171, 0, 0, 0, 172, 150, 0,
151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157,
158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 400, 0,
0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0,
0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168,
169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 463,
173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160,
161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 515,
0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155,
177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170,
0, 31, 0, 0, 0, 0, 0, 0, 171, 548, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153,
0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0,
0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 150,
0, 151, 0, 152, 592, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156,
157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0,
631, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175,
176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164,
0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29,
167, 168, 169, 170, 0, 31, 0, 0, 633, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0,
152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159,
160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171,
681, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154,
155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169,
170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 684, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173,
153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161,
162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 687, 0, 0, 0, 171, 0, 0, 0,
172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177,
178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31,
0, 0, 0, 0, 0, 0, 171, 713, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174,
175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 717, 0, 0, 172, 150, 0,
151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157,
158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0,
0, 171, 719, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0,
154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168,
169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 746, 0, 0, 172, 150, 0, 151, 0, 152, 0,
173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160,
161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 0, 0,
0, 172, 150, 0, 151, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178,
156, 157, 158, 159, 160, 161, 162, 0, 0, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0,
0, 307, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 0, 0, 173, 0, 0, 174, 175, 176,
0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 0, 164, 0,
165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167,
168, 169, 170, 0, 31, 0, 0, 0, 607, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 0,
0, 173, 0, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160,
161, 162, 0, 0, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 303, 0, 0, 0,
172, 150, 0, 151, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178,
156, 157, 158, 159, 160, 161, 162, 0, 0, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0,
0, 0, 0, 0, 315, 0, 0, 0, 172, 150, 0, 151, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0,
0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 0, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167,
168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 0, 0, 0, 0, 173,
0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 29, 66, 0, 67, 68, 0, 31, 0, 0, 0, 0,
0, 0, 69, 0, 0, 70, 71, 0, 0, 0, 444, 445, 0, 72, 0, 0, 0, 73, 74}; static short
Cyc_yycheck[ 4415u]={ 54, 81, 86, 83, 54, 153, 81, 61, 83, 54, 249, 86, 22, 78,
33, 366, 153, 116, 57, 206, 302, 54, 152, 103, 115, 218, 45, 564, 103, 55, 81,
418, 171, 0, 280, 17, 116, 102, 422, 104, 115, 116, 56, 108, 98, 99, 100, 101,
98, 86, 678, 5, 6, 98, 21, 22, 110, 24, 112, 69, 110, 98, 112, 74, 74, 73, 33,
112, 455, 357, 17, 18, 19, 40, 362, 459, 113, 289, 45, 318, 96, 101, 88, 81, 51,
83, 53, 299, 86, 56, 110, 171, 133, 119, 113, 439, 171, 46, 47, 48, 108, 445, 69,
114, 641, 103, 88, 74, 93, 68, 90, 88, 173, 174, 88, 176, 404, 115, 116, 104, 94,
749, 87, 90, 91, 92, 93, 101, 147, 106, 171, 69, 70, 194, 214, 68, 764, 106, 103,
214, 107, 423, 100, 771, 103, 99, 113, 775, 107, 88, 90, 109, 110, 207, 219, 122,
95, 207, 125, 68, 127, 101, 207, 138, 303, 112, 113, 114, 106, 107, 103, 120,
147, 171, 107, 112, 315, 87, 562, 106, 147, 40, 138, 120, 121, 529, 205, 136,
137, 726, 231, 147, 93, 268, 103, 249, 97, 216, 107, 249, 64, 65, 66, 104, 588,
285, 286, 88, 88, 101, 285, 286, 287, 94, 94, 295, 214, 87, 88, 258, 295, 101,
297, 303, 243, 97, 101, 302, 303, 101, 110, 90, 91, 92, 93, 315, 88, 101, 205,
101, 315, 93, 94, 88, 295, 109, 110, 117, 88, 216, 95, 381, 104, 220, 94, 95,
607, 311, 98, 107, 98, 454, 303, 122, 318, 291, 653, 115, 318, 310, 280, 226,
313, 98, 315, 557, 243, 232, 297, 234, 235, 342, 343, 344, 103, 299, 327, 285,
286, 287, 97, 478, 111, 268, 88, 98, 103, 295, 378, 297, 94, 88, 378, 378, 302,
303, 386, 101, 95, 439, 386, 386, 287, 280, 281, 445, 110, 315, 112, 113, 114,
93, 297, 364, 389, 93, 410, 99, 101, 98, 297, 99, 299, 96, 580, 410, 206, 110,
101, 112, 113, 114, 396, 106, 311, 88, 396, 110, 423, 96, 391, 597, 393, 394,
101, 396, 431, 411, 88, 106, 97, 411, 89, 110, 94, 558, 411, 97, 467, 98, 616,
101, 101, 97, 100, 466, 93, 106, 411, 103, 378, 98, 112, 113, 114, 99, 102, 467,
386, 103, 97, 466, 467, 103, 97, 88, 103, 103, 97, 529, 103, 94, 108, 370, 103,
372, 97, 98, 101, 101, 597, 97, 410, 695, 106, 108, 103, 103, 288, 112, 113, 114,
521, 97, 97, 423, 487, 97, 97, 616, 103, 441, 97, 103, 103, 103, 446, 307, 103,
465, 108, 521, 87, 103, 314, 103, 521, 88, 108, 101, 108, 321, 407, 94, 409, 603,
431, 99, 768, 705, 101, 103, 481, 773, 98, 432, 603, 101, 466, 467, 437, 112,
113, 114, 441, 106, 663, 90, 109, 446, 102, 611, 557, 88, 723, 103, 356, 357,
115, 107, 99, 99, 362, 99, 103, 103, 366, 538, 101, 553, 101, 542, 543, 553, 334,
335, 336, 337, 553, 528, 749, 370, 101, 372, 481, 97, 98, 756, 705, 73, 37, 101,
602, 521, 101, 564, 101, 602, 118, 768, 102, 103, 771, 119, 404, 71, 72, 67, 68,
117, 410, 115, 116, 102, 103, 415, 101, 601, 5, 6, 99, 601, 102, 103, 102, 103,
601, 99, 623, 557, 99, 528, 102, 103, 102, 103, 89, 90, 535, 98, 580, 88, 89,
432, 91, 92, 101, 94, 102, 103, 102, 103, 22, 23, 101, 99, 37, 104, 105, 630,
631, 98, 633, 98, 635, 112, 102, 103, 468, 116, 117, 102, 103, 101, 602, 102,
103, 101, 478, 22, 23, 102, 103, 580, 102, 103, 102, 103, 88, 660, 695, 108, 706,
703, 102, 103, 102, 103, 703, 624, 625, 706, 150, 97, 152, 153, 97, 88, 89, 101,
91, 92, 683, 94, 99, 686, 687, 165, 166, 102, 101, 332, 333, 104, 105, 173, 174,
100, 176, 338, 339, 112, 340, 341, 102, 116, 117, 111, 99, 723, 103, 626, 99,
723, 629, 100, 194, 103, 102, 689, 721, 99, 74, 35, 106, 102, 727, 97, 103, 108,
101, 28, 733, 97, 41, 749, 93, 695, 88, 749, 87, 102, 756, 115, 102, 703, 756,
748, 706, 103, 101, 88, 93, 99, 768, 103, 111, 771, 768, 102, 102, 771, 689, 102,
102, 88, 681, 597, 102, 684, 101, 103, 102, 99, 102, 99, 103, 607, 693, 101, 610,
102, 102, 99, 99, 98, 616, 97, 102, 107, 99, 102, 707, 35, 111, 103, 102, 712,
713, 102, 98, 716, 717, 102, 719, 106, 111, 75, 76, 77, 78, 79, 80, 81, 82, 83,
84, 98, 97, 88, 97, 26, 102, 108, 102, 740, 102, 102, 743, 101, 745, 746, 100,
663, 101, 98, 102, 99, 99, 97, 99, 0, 102, 108, 0, 102, 570, 762, 326, 311, 328,
329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344,
437, 528, 205, 214, 645, 30, 293, 115, 705, 447, 35, 580, 37, 281, 39, 326, 328,
42, - 1, 329, - 1, 330, - 1, 331, 49, 50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59,
- 1, 381, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1,
94, - 1, - 1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105, - 1, - 1, - 1, 109, 110,
- 1, 112, - 1, - 1, 115, 116, 117, - 1, 439, - 1, - 1, 122, 123, - 1, 445, - 1,
- 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
22, 23, 24, 25, - 1, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 487,
42, 43, 44, - 1, - 1, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 529, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1,
97, 98, - 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, -
1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 88, 89, - 1, 91, 92, - 1,
94, - 1, - 1, - 1, - 1, 99, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, 109,
110, - 1, 112, - 1, - 1, - 1, 116, 117, - 1, - 1, - 1, 603, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 611, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 22, 23, 24, 25, - 1, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, - 1, 42, 43, 44, - 1, - 1, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93,
94, 95, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1,
- 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, - 1, 27,
28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, - 1, 42, 43, 44, - 1, - 1,
47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, - 1, 65, 66,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, 97, 98, 99, - 1,
101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116,
117, - 1, - 1, - 1, - 1, 122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, - 1, 27, 28, 29, 30, 31, 32, 33, 34,
35, 36, 37, 38, 39, 40, - 1, 42, 43, 44, - 1, - 1, 47, 48, 49, 50, 51, 52, 53,
54, 55, 56, 57, 58, 59, 60, 61, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, 93, 94, 95, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 104, 105, - 1,
- 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122,
123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, -
1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 35, 36, 37, 38, 39, - 1,
- 1, 42, 43, - 1, - 1, - 1, 47, 48, 49, 50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59,
60, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93,
94, 95, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1,
- 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 35, 36, 37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1, - 1,
47, 48, 49, 50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59, 60, - 1, 62, 63, - 1, 65,
66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, - 1, 98, - 1, -
1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115,
116, 117, - 1, - 1, - 1, - 1, 122, 123, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 35,
36, 37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1, - 1, 47, 48, 49, 50, - 1, - 1, 53,
54, 55, 56, 57, 58, 59, 60, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, 93, 94, 95, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, 104, 105, -
1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1,
122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 36, - 1, 38,
- 1, - 1, - 1, - 1, 43, - 1, 45, 46, 47, 48, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 60, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, 88, - 1, - 1, - 1, - 1, 93,
94, 95, 36, - 1, - 1, - 1, - 1, 101, - 1, 43, 104, 105, - 1, 47, 48, - 1, - 1, -
1, 112, 113, 114, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 88, - 1, - 1, 36, - 1, 93, 94, 95, - 1, - 1, - 1, - 1, - 1, 101, 47, 48,
104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, 113, 114, 60, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1,
85, - 1, - 1, 88, - 1, - 1, - 1, - 1, 93, 94, 95, 36, 97, - 1, - 1, - 1, 101,
102, - 1, 104, 105, - 1, 47, 48, - 1, 110, - 1, 112, 113, 114, - 1, - 1, - 1, -
1, - 1, 60, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 85, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 93, - 1, 95, 36,
97, 38, - 1, - 1, 101, 102, 43, 104, 105, - 1, 47, 48, - 1, 110, - 1, 112, 113,
114, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
- 1, 93, - 1, 95, - 1, - 1, 98, 36, 100, 38, - 1, - 1, 104, 105, 43, - 1, - 1, -
1, 47, 48, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 93, - 1, 95, 36, - 1, 98, - 1, - 1,
- 1, - 1, 43, 104, 105, - 1, 47, 48, 25, - 1, 27, 28, 29, 30, 31, 32, 33, 34, 35,
60, 37, - 1, 39, 40, - 1, 42, - 1, 44, - 1, - 1, - 1, - 1, 49, 50, 51, 52, 53,
54, 55, 56, 57, 58, 59, - 1, 61, 62, 63, - 1, 65, 66, - 1, - 1, 93, - 1, 95, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, -
1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1,
122, 123, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, 36, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 47, 48, - 1, 36, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 60, 47,
48, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, 88, - 1, - 1, - 1, - 1, 93, - 1,
95, - 1, 97, - 1, 85, - 1, - 1, 102, - 1, 104, 105, - 1, 93, - 1, 95, - 1, 97, -
1, - 1, - 1, - 1, 102, - 1, 104, 105, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 36, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 47, 48, - 1, 36, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 60, 47, 48, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 60, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, -
1, 93, - 1, 95, - 1, - 1, 98, 85, - 1, - 1, - 1, 36, 104, 105, - 1, 93, - 1, 95,
- 1, - 1, - 1, - 1, 47, 48, - 1, - 1, 104, 105, - 1, 35, - 1, 37, - 1, 39, - 1,
60, 42, - 1, - 1, - 1, - 1, - 1, - 1, 49, 50, - 1, - 1, 53, 54, 55, 56, 57, 58,
59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 93, - 1,
95, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 104, 105, - 1, 88, 89, 90, 91, 92, -
1, 94, - 1, - 1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105, - 1, - 1, - 1, 109,
110, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 35, - 1,
37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1, - 1, - 1, - 1, 49, 50, - 1, - 1, 53,
54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105,
- 1, - 1, - 1, 109, 110, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1,
122, 123, 35, - 1, 37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1, - 1, - 1, - 1, 49,
50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, - 1, - 1, 101,
- 1, - 1, - 1, 105, - 1, - 1, - 1, 109, 110, - 1, 112, - 1, - 1, 115, 116, 117,
- 1, - 1, - 1, - 1, 122, 123, 35, - 1, 37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1,
- 1, - 1, - 1, 49, 50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, -
1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, 111, 112, 42, - 1,
115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1,
62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, -
1, - 1, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112,
42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59,
- 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1,
94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39,
- 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57,
58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91,
92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37,
- 1, 39, 111, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54,
55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105,
35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122,
123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1,
- 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49,
50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101,
- 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, 111, 112, 42, - 1, 115, 116, 117, - 1,
- 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, 97, - 1, - 1, -
1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116,
117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, 97, -
1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1,
115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1,
62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, -
1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112,
42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59,
- 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1,
94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, 35, - 1, 37, - 1, 39,
- 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57,
58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91,
92, - 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37,
- 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54,
55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105,
35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122,
123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1,
- 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49,
50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101,
102, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1,
- 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, -
1, 101, 102, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116,
117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, -
1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, - 1, - 1, 112, - 1, - 1,
115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1,
- 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, -
1, - 1, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, - 1, - 1, 112,
- 1, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59,
- 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1,
94, - 1, - 1, - 1, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, - 1,
- 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56,
57, 58, 59, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90,
91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1,
37, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53,
54, 55, 56, 57, 58, 59, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1,
105, 35, - 1, 37, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 49, 50,
122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1,
- 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1,
- 1, - 1, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1,
- 1, - 1, - 1, 122, 123, 88, 89, - 1, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1,
- 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, 109, 110, - 1, 112, - 1, - 1, - 1,
116, 117}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tagged_ptr1{ short* curr; short* base; short*
last_plus_one; } ; struct _tagged_ptr2{ void** curr; void** base; void**
last_plus_one; } ; struct _tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype*
base; struct Cyc_Yyltype* last_plus_one; } ; struct _tuple19{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate; int yyn= 0;
int yyerrstatus; int yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=({
short* _temp4302=({ unsigned int _temp4299= 200u; short* _temp4300=( short*)
GC_malloc_atomic( sizeof( short) * _temp4299);{ unsigned int _temp4301=
_temp4299; unsigned int i; for( i= 0; i < _temp4301; i ++){ _temp4300[ i]= 0;}};
_temp4300;}); struct _tagged_ptr1 _temp4303; _temp4303.curr= _temp4302;
_temp4303.base= _temp4302; _temp4303.last_plus_one= _temp4302 + 200; _temp4303;});
int yyvsp_offset; struct _tagged_ptr2 yyvs=({ void** _temp4297=({ unsigned int
_temp4294= 200u; void** _temp4295=( void**) GC_malloc( sizeof( void*) *
_temp4294);{ unsigned int _temp4296= _temp4294; unsigned int i; for( i= 0; i <
_temp4296; i ++){ _temp4295[ i]= Cyc_yylval;}}; _temp4295;}); struct
_tagged_ptr2 _temp4298; _temp4298.curr= _temp4297; _temp4298.base= _temp4297;
_temp4298.last_plus_one= _temp4297 + 200; _temp4298;}); int yylsp_offset; struct
_tagged_ptr3 yyls=({ struct Cyc_Yyltype* _temp4292=({ unsigned int _temp4289=
200u; struct Cyc_Yyltype* _temp4290=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp4289);{ unsigned int _temp4291= _temp4289;
unsigned int i; for( i= 0; i < _temp4291; i ++){ _temp4290[ i]= Cyc_yynewloc();}};
_temp4290;}); struct _tagged_ptr3 _temp4293; _temp4293.curr= _temp4292;
_temp4293.base= _temp4292; _temp4293.last_plus_one= _temp4292 + 200; _temp4293;});
int yystacksize= 200; void* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), ++ yyssp_offset)=(
short) yystate; if( yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){
Cyc_yyerror(({ unsigned char* _temp2258="parser stack overflow"; struct
_tagged_string _temp2259; _temp2259.curr= _temp2258; _temp2259.base= _temp2258;
_temp2259.last_plus_one= _temp2258 + 22; _temp2259;}));( void) _throw(( void*)
Cyc_Yystack_overflow);} yystacksize *= 2; if( yystacksize > 10000){ yystacksize=
10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2268=( unsigned int)
yystacksize; short* _temp2269=( short*) GC_malloc_atomic( sizeof( short) *
_temp2268); struct _tagged_ptr1 _temp2271={ _temp2269, _temp2269, _temp2269 +
_temp2268};{ unsigned int _temp2270= _temp2268; unsigned int i; for( i= 0; i <
_temp2270; i ++){ _temp2269[ i]= 0;}}; _temp2271;}); struct _tagged_ptr2 yyvs1=({
unsigned int _temp2264=( unsigned int) yystacksize; void** _temp2265=( void**)
GC_malloc( sizeof( void*) * _temp2264); struct _tagged_ptr2 _temp2267={
_temp2265, _temp2265, _temp2265 + _temp2264};{ unsigned int _temp2266= _temp2264;
unsigned int i; for( i= 0; i < _temp2266; i ++){ _temp2265[ i]= Cyc_yylval;}};
_temp2267;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2260=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2261=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2260); struct _tagged_ptr3 _temp2263={
_temp2261, _temp2261, _temp2261 + _temp2260};{ unsigned int _temp2262= _temp2260;
unsigned int i; for( i= 0; i < _temp2262; i ++){ _temp2261[ i]= Cyc_yynewloc();}};
_temp2263;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){*(( short*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( yyss1,
sizeof( short), i)=*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), i);*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs1, sizeof( void*), i)=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), i);*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls1, sizeof( struct Cyc_Yyltype), i)=*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), i);}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull(
782u, yystate)]; if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar == - 2){
Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar= 0;}
else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int) Cyc_yytranslate[
_check_known_subscript_notnull( 352u, Cyc_yychar)]: 235;} yyn += yychar1; if((
yyn < 0? 1: yyn > 4414)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4415u,
yyn)] != yychar1){ goto yydefault;} yyn=( int) Cyc_yytable[
_check_known_subscript_notnull( 4415u, yyn)]; if( yyn < 0){ if( yyn == - 32768){
goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}}
if( yyn == 781){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; if( yyerrstatus
!= 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int) Cyc_yydefact[
_check_known_subscript_notnull( 782u, yystate)]; if( yyn == 0){ goto yyerrlab;}
yyreduce: yylen=( int) Cyc_yyr2[ _check_known_subscript_notnull( 405u, yyn)];
if( yylen > 0){ yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),( yyvsp_offset +
1) - yylen);} switch( yyn){ case 1: _LL2272: yyval=*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); break; case 2:
_LL2273: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2275=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2275[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2276; _temp2276.tag= Cyc_DeclList_tok; _temp2276.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2276;});
_temp2275;}); break; case 3: _LL2274: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2278=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2278[ 0]=({ struct Cyc_DeclList_tok_struct _temp2279; _temp2279.tag= Cyc_DeclList_tok;
_temp2279.f1=({ struct Cyc_List_List* _temp2280=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2280->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2281=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2281->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2282=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2282[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2283; _temp2283.tag= Cyc_Absyn_Using_d;
_temp2283.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2283.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2283;}); _temp2282;})); _temp2281->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2281;}); _temp2280->tl= 0; _temp2280;}); _temp2279;}); _temp2278;}); Cyc_Lex_leave_using();
break; case 4: _LL2277: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2285=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2285[ 0]=({ struct Cyc_DeclList_tok_struct _temp2286; _temp2286.tag= Cyc_DeclList_tok;
_temp2286.f1=({ struct Cyc_List_List* _temp2287=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2287->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2288=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2288->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2289=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2289[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2290; _temp2290.tag= Cyc_Absyn_Using_d;
_temp2290.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2290.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2290;}); _temp2289;})); _temp2288->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2288;}); _temp2287->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2287;});
_temp2286;}); _temp2285;}); break; case 5: _LL2284: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2292=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2292[ 0]=({ struct Cyc_DeclList_tok_struct _temp2293; _temp2293.tag= Cyc_DeclList_tok;
_temp2293.f1=({ struct Cyc_List_List* _temp2294=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2294->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2295=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2295->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2296=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2296[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2297; _temp2297.tag=
Cyc_Absyn_Namespace_d; _temp2297.f1=({ struct _tagged_string* _temp2298=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2298[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2298;}); _temp2297.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2297;}); _temp2296;})); _temp2295->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2295;}); _temp2294->tl= 0; _temp2294;}); _temp2293;}); _temp2292;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2291: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2300=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2300[ 0]=({ struct Cyc_DeclList_tok_struct _temp2301; _temp2301.tag= Cyc_DeclList_tok;
_temp2301.f1=({ struct Cyc_List_List* _temp2302=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2302->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2303=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2303->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2304=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2304[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2305; _temp2305.tag=
Cyc_Absyn_Namespace_d; _temp2305.f1=({ struct _tagged_string* _temp2306=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2306[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp2306;}); _temp2305.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2305;}); _temp2304;})); _temp2303->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2303;}); _temp2302->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2302;});
_temp2301;}); _temp2300;}); break; case 7: _LL2299: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),({
unsigned char* _temp2308="C"; struct _tagged_string _temp2309; _temp2309.curr=
_temp2308; _temp2309.base= _temp2308; _temp2309.last_plus_one= _temp2308 + 2;
_temp2309;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp2310="only extern \"C\" { ... } is allowed";
struct _tagged_string _temp2311; _temp2311.curr= _temp2310; _temp2311.base=
_temp2310; _temp2311.last_plus_one= _temp2310 + 35; _temp2311;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2312=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2312[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2313; _temp2313.tag= Cyc_DeclList_tok; _temp2313.f1=({ struct Cyc_List_List*
_temp2314=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2314->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2315=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2315->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2316=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2316[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2317; _temp2317.tag= Cyc_Absyn_ExternC_d;
_temp2317.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2317;}); _temp2316;})); _temp2315->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp2315;}); _temp2314->tl= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2314;}); _temp2313;}); _temp2312;}); break;
case 8: _LL2307: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2319=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2319[ 0]=({ struct Cyc_DeclList_tok_struct _temp2320; _temp2320.tag= Cyc_DeclList_tok;
_temp2320.f1= 0; _temp2320;}); _temp2319;}); break; case 9: _LL2318: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2322=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2322[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2323; _temp2323.tag= Cyc_DeclList_tok; _temp2323.f1=({ struct Cyc_List_List*
_temp2324=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2324->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2325=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2325[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2326; _temp2326.tag= Cyc_Absyn_Fn_d;
_temp2326.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2326;}); _temp2325;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2324->tl= 0; _temp2324;}); _temp2323;}); _temp2322;}); break; case 10:
_LL2321: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 11:
_LL2327: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2329=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2329[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2330; _temp2330.tag= Cyc_FnDecl_tok; _temp2330.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2330;}); _temp2329;}); break; case 12: _LL2328:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2332=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2332[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2333; _temp2333.tag= Cyc_FnDecl_tok; _temp2333.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2334=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2334->v=( void*) Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2334;}), Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2333;}); _temp2332;}); break; case 13: _LL2331: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2336=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2336[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2337; _temp2337.tag= Cyc_FnDecl_tok;
_temp2337.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2337;}); _temp2336;}); break; case 14: _LL2335: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2339=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2339[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2340; _temp2340.tag= Cyc_FnDecl_tok;
_temp2340.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2341=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2341->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2341;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2340;});
_temp2339;}); break; case 15: _LL2338: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2343=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2343[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2344; _temp2344.tag= Cyc_FnDecl_tok;
_temp2344.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2345=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2345->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2345;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2344;});
_temp2343;}); break; case 16: _LL2342: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2347=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2347[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2348; _temp2348.tag= Cyc_FnDecl_tok;
_temp2348.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2349=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2349->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2349;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2348;});
_temp2347;}); break; case 17: _LL2346: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 18: _LL2350: Cyc_Lex_leave_using();
break; case 19: _LL2351: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2353=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2353[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2353;})); yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 20: _LL2352: Cyc_Lex_leave_namespace(); break; case
21: _LL2354: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2356=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2356[ 0]=({ struct Cyc_DeclList_tok_struct _temp2357; _temp2357.tag= Cyc_DeclList_tok;
_temp2357.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp2357;}); _temp2356;}); break; case 22: _LL2355: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2359=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2359[ 0]=({ struct Cyc_DeclList_tok_struct _temp2360; _temp2360.tag= Cyc_DeclList_tok;
_temp2360.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_InitDeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2360;}); _temp2359;}); break; case 23: _LL2358: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2362=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2362[ 0]=({ struct Cyc_DeclList_tok_struct _temp2363; _temp2363.tag= Cyc_DeclList_tok;
_temp2363.f1=({ struct Cyc_List_List* _temp2364=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2364->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
0, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2364->tl= 0; _temp2364;}); _temp2363;});
_temp2362;}); break; case 24: _LL2361: { struct Cyc_List_List* _temp2366= 0;
goto _LL2367; _LL2367:{ struct Cyc_List_List* _temp2368= Cyc_yyget_IdList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); goto
_LL2369; _LL2369: for( 0; _temp2368 != 0; _temp2368=(( struct Cyc_List_List*)
_check_null( _temp2368))->tl){ struct _tagged_string* _temp2370=( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( _temp2368))->hd; goto
_LL2371; _LL2371: { struct _tuple1* _temp2375=({ struct _tuple1* _temp2372=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2372->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp2373=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp2373[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp2374; _temp2374.tag= Cyc_Absyn_Rel_n; _temp2374.f1= 0; _temp2374;});
_temp2373;}); _temp2372->f2= _temp2370; _temp2372;}); goto _LL2376; _LL2376: {
struct Cyc_Absyn_Vardecl* _temp2377= Cyc_Absyn_new_vardecl( _temp2375, Cyc_Absyn_wildtyp(
0), 0); goto _LL2378; _LL2378: _temp2366=({ struct Cyc_List_List* _temp2379=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2379->hd=(
void*) _temp2377; _temp2379->tl= _temp2366; _temp2379;});}}}} _temp2366=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2366);
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2380=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2380[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2381; _temp2381.tag= Cyc_DeclList_tok; _temp2381.f1=({ struct Cyc_List_List*
_temp2382=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2382->hd=( void*) Cyc_Absyn_letv_decl( _temp2366, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2382->tl= 0; _temp2382;}); _temp2381;}); _temp2380;}); break;} case 25:
_LL2365: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 26:
_LL2383: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2385=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2385[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2386; _temp2386.tag= Cyc_DeclList_tok; _temp2386.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2386;});
_temp2385;}); break; case 27: _LL2384: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2388=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2388[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2389; _temp2389.tag= Cyc_DeclSpec_tok;
_temp2389.f1=({ struct Cyc_Parse_Declaration_spec* _temp2390=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2390->sc=({ struct
Cyc_Core_Opt* _temp2391=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2391->v=( void*) Cyc_yyget_StorageClass_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2391;}); _temp2390->tq= Cyc_Absyn_empty_tqual();
_temp2390->type_specs= 0; _temp2390->is_inline= 0; _temp2390->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2390;});
_temp2389;}); _temp2388;}); break; case 28: _LL2387: if(( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc != 0){ Cyc_Parse_warn(({
unsigned char* _temp2393="Only one storage class is allowed in a declaration";
struct _tagged_string _temp2394; _temp2394.curr= _temp2393; _temp2394.base=
_temp2393; _temp2394.last_plus_one= _temp2393 + 51; _temp2394;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2395=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2395[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2396; _temp2396.tag= Cyc_DeclSpec_tok; _temp2396.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2397=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2397->sc=({ struct Cyc_Core_Opt* _temp2398=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2398->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2398;});
_temp2397->tq=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tq; _temp2397->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->type_specs; _temp2397->is_inline=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->is_inline;
_temp2397->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2397;}); _temp2396;}); _temp2395;}); break; case 29: _LL2392: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2400=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2400[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2401; _temp2401.tag= Cyc_DeclSpec_tok; _temp2401.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2402=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2402->sc= 0; _temp2402->tq= Cyc_Absyn_empty_tqual(); _temp2402->type_specs=({
struct Cyc_List_List* _temp2403=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2403->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2403->tl=
0; _temp2403;}); _temp2402->is_inline= 0; _temp2402->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2402;});
_temp2401;}); _temp2400;}); break; case 30: _LL2399: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2405=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2405[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2406; _temp2406.tag= Cyc_DeclSpec_tok;
_temp2406.f1=({ struct Cyc_Parse_Declaration_spec* _temp2407=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2407->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2407->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2407->type_specs=({ struct Cyc_List_List* _temp2408=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2408->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2408->tl=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2408;}); _temp2407->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->is_inline; _temp2407->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2407;}); _temp2406;}); _temp2405;}); break; case 31: _LL2404: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2410=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2410[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2411; _temp2411.tag= Cyc_DeclSpec_tok; _temp2411.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2412=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2412->sc= 0; _temp2412->tq= Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2412->type_specs= 0; _temp2412->is_inline=
0; _temp2412->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2412;}); _temp2411;}); _temp2410;}); break;
case 32: _LL2409: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2414=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2414[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2415; _temp2415.tag= Cyc_DeclSpec_tok;
_temp2415.f1=({ struct Cyc_Parse_Declaration_spec* _temp2416=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2416->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2416->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tq); _temp2416->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2416->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->is_inline; _temp2416->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2416;}); _temp2415;}); _temp2414;}); break; case 33: _LL2413: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2418=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2418[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2419; _temp2419.tag= Cyc_DeclSpec_tok; _temp2419.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2420=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2420->sc= 0; _temp2420->tq= Cyc_Absyn_empty_tqual(); _temp2420->type_specs=
0; _temp2420->is_inline= 1; _temp2420->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2420;});
_temp2419;}); _temp2418;}); break; case 34: _LL2417: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2422=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2422[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2423; _temp2423.tag= Cyc_DeclSpec_tok;
_temp2423.f1=({ struct Cyc_Parse_Declaration_spec* _temp2424=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2424->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2424->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2424->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->type_specs; _temp2424->is_inline= 1; _temp2424->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2424;}); _temp2423;}); _temp2422;}); break; case 35: _LL2421: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp2426=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2426[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2427; _temp2427.tag= Cyc_StorageClass_tok;
_temp2427.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp2427;}); _temp2426;});
break; case 36: _LL2425: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2429=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2429[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2430; _temp2430.tag=
Cyc_StorageClass_tok; _temp2430.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2430;}); _temp2429;}); break; case 37: _LL2428: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2432=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2432[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2433; _temp2433.tag=
Cyc_StorageClass_tok; _temp2433.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2433;}); _temp2432;}); break; case 38: _LL2431: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2435=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2435[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2436; _temp2436.tag=
Cyc_StorageClass_tok; _temp2436.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2436;}); _temp2435;}); break; case 39: _LL2434: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),({ unsigned char*
_temp2438="C"; struct _tagged_string _temp2439; _temp2439.curr= _temp2438;
_temp2439.base= _temp2438; _temp2439.last_plus_one= _temp2438 + 2; _temp2439;}))
!= 0){ Cyc_Parse_err(({ unsigned char* _temp2440="only extern or extern \"C\" is allowed";
struct _tagged_string _temp2441; _temp2441.curr= _temp2440; _temp2441.base=
_temp2440; _temp2441.last_plus_one= _temp2440 + 37; _temp2441;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2442=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2442[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2443; _temp2443.tag= Cyc_StorageClass_tok;
_temp2443.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2443;}); _temp2442;});
break; case 40: _LL2437: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2445=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2445[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2446; _temp2446.tag=
Cyc_StorageClass_tok; _temp2446.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2446;}); _temp2445;}); break; case 41: _LL2444: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2448=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2448[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2449; _temp2449.tag=
Cyc_StorageClass_tok; _temp2449.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2449;}); _temp2448;}); break; case 42: _LL2447: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2451=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2451[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2452; _temp2452.tag=
Cyc_AttributeList_tok; _temp2452.f1= 0; _temp2452;}); _temp2451;}); break; case
43: _LL2450: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 44: _LL2453: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2455=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2455[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2456; _temp2456.tag=
Cyc_AttributeList_tok; _temp2456.f1= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2456;}); _temp2455;}); break;
case 45: _LL2454: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2458=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2458[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2459; _temp2459.tag=
Cyc_AttributeList_tok; _temp2459.f1=({ struct Cyc_List_List* _temp2460=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2460->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2460->tl= 0; _temp2460;}); _temp2459;}); _temp2458;}); break; case 46:
_LL2457: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp2462=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2462[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2463; _temp2463.tag=
Cyc_AttributeList_tok; _temp2463.f1=({ struct Cyc_List_List* _temp2464=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2464->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2464->tl= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2464;}); _temp2463;}); _temp2462;}); break; case 47:
_LL2461: { struct _tagged_string _temp2466= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); goto _LL2467; _LL2467: if((((({ struct
_tagged_string _temp2468= _temp2466;( unsigned int)( _temp2468.last_plus_one -
_temp2468.curr);}) > 4?*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2466, sizeof(
unsigned char), 0) =='_': 0)?*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2466, sizeof(
unsigned char), 1) =='_': 0)?*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2466, sizeof(
unsigned char),( int)(({ struct _tagged_string _temp2469= _temp2466;(
unsigned int)( _temp2469.last_plus_one - _temp2469.curr);}) - 2)) =='_': 0)?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2466, sizeof( unsigned char),( int)(({ struct
_tagged_string _temp2470= _temp2466;( unsigned int)( _temp2470.last_plus_one -
_temp2470.curr);}) - 3)) =='_': 0){ _temp2466= Cyc_String_substring( _temp2466,
2,({ struct _tagged_string _temp2471= _temp2466;( unsigned int)( _temp2471.last_plus_one
- _temp2471.curr);}) - 5);}{ int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp(
_temp2466,( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f1) ==
0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2472=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2472[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2473; _temp2473.tag= Cyc_Attribute_tok; _temp2473.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp2473;}); _temp2472;}); break;}} if( i == 16u){ Cyc_Parse_err(({
unsigned char* _temp2474="unrecognized attribute"; struct _tagged_string
_temp2475; _temp2475.curr= _temp2474; _temp2475.base= _temp2474; _temp2475.last_plus_one=
_temp2474 + 23; _temp2475;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp2476=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2476[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2477; _temp2477.tag= Cyc_Attribute_tok; _temp2477.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp2477;}); _temp2476;});} break;}} case
48: _LL2465: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2479=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2479[ 0]=({ struct Cyc_Attribute_tok_struct _temp2480; _temp2480.tag= Cyc_Attribute_tok;
_temp2480.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2480;}); _temp2479;});
break; case 49: _LL2478: { struct _tagged_string _temp2482= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); goto
_LL2483; _LL2483: { struct _tuple16 _temp2486; int _temp2487; void* _temp2489;
struct _tuple16* _temp2484= Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2486=* _temp2484; _LL2490: _temp2489= _temp2486.f1;
goto _LL2488; _LL2488: _temp2487= _temp2486.f2; goto _LL2485; _LL2485: { void* a;
if( Cyc_String_zstrcmp( _temp2482,({ unsigned char* _temp2491="regparm"; struct
_tagged_string _temp2492; _temp2492.curr= _temp2491; _temp2492.base= _temp2491;
_temp2492.last_plus_one= _temp2491 + 8; _temp2492;})) == 0? 1: Cyc_String_zstrcmp(
_temp2482,({ unsigned char* _temp2493="__regparm__"; struct _tagged_string
_temp2494; _temp2494.curr= _temp2493; _temp2494.base= _temp2493; _temp2494.last_plus_one=
_temp2493 + 12; _temp2494;})) == 0){ if( _temp2487 <= 0? 1: _temp2487 > 3){ Cyc_Parse_err(({
unsigned char* _temp2495="regparm requires value between 1 and 3"; struct
_tagged_string _temp2496; _temp2496.curr= _temp2495; _temp2496.base= _temp2495;
_temp2496.last_plus_one= _temp2495 + 39; _temp2496;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2497=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2497[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2498; _temp2498.tag= Cyc_Absyn_Regparm_att;
_temp2498.f1= _temp2487; _temp2498;}); _temp2497;});} else{ if( Cyc_String_zstrcmp(
_temp2482,({ unsigned char* _temp2499="aligned"; struct _tagged_string _temp2500;
_temp2500.curr= _temp2499; _temp2500.base= _temp2499; _temp2500.last_plus_one=
_temp2499 + 8; _temp2500;})) == 0? 1: Cyc_String_zstrcmp( _temp2482,({
unsigned char* _temp2501="__aligned__"; struct _tagged_string _temp2502;
_temp2502.curr= _temp2501; _temp2502.base= _temp2501; _temp2502.last_plus_one=
_temp2501 + 12; _temp2502;})) == 0){ if( _temp2487 < 0){ Cyc_Parse_err(({
unsigned char* _temp2503="aligned requires positive power of two"; struct
_tagged_string _temp2504; _temp2504.curr= _temp2503; _temp2504.base= _temp2503;
_temp2504.last_plus_one= _temp2503 + 39; _temp2504;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}{
unsigned int j=( unsigned int) _temp2487; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(({ unsigned char* _temp2505="aligned requires positive power of two";
struct _tagged_string _temp2506; _temp2506.curr= _temp2505; _temp2506.base=
_temp2505; _temp2506.last_plus_one= _temp2505 + 39; _temp2506;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2507=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2507[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2508; _temp2508.tag= Cyc_Absyn_Aligned_att;
_temp2508.f1= _temp2487; _temp2508;}); _temp2507;});}} else{ Cyc_Parse_err(({
unsigned char* _temp2509="unrecognized attribute"; struct _tagged_string
_temp2510; _temp2510.curr= _temp2509; _temp2510.base= _temp2509; _temp2510.last_plus_one=
_temp2509 + 23; _temp2510;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line)); a=( void*)
Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2511=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2511[ 0]=({ struct Cyc_Attribute_tok_struct _temp2512; _temp2512.tag= Cyc_Attribute_tok;
_temp2512.f1=( void*) a; _temp2512;}); _temp2511;}); break;}}} case 50: _LL2481: {
struct _tagged_string _temp2514= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); goto _LL2515; _LL2515: { struct
_tagged_string _temp2516= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); goto _LL2517; _LL2517: { void* a; if( Cyc_String_zstrcmp(
_temp2514,({ unsigned char* _temp2518="section"; struct _tagged_string _temp2519;
_temp2519.curr= _temp2518; _temp2519.base= _temp2518; _temp2519.last_plus_one=
_temp2518 + 8; _temp2519;})) == 0? 1: Cyc_String_zstrcmp( _temp2514,({
unsigned char* _temp2520="__section__"; struct _tagged_string _temp2521;
_temp2521.curr= _temp2520; _temp2521.base= _temp2520; _temp2521.last_plus_one=
_temp2520 + 12; _temp2521;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2522=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2522[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2523; _temp2523.tag=
Cyc_Absyn_Section_att; _temp2523.f1= _temp2516; _temp2523;}); _temp2522;});}
else{ Cyc_Parse_err(({ unsigned char* _temp2524="unrecognized attribute"; struct
_tagged_string _temp2525; _temp2525.curr= _temp2524; _temp2525.base= _temp2524;
_temp2525.last_plus_one= _temp2524 + 23; _temp2525;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2526=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2526[ 0]=({ struct Cyc_Attribute_tok_struct _temp2527; _temp2527.tag= Cyc_Attribute_tok;
_temp2527.f1=( void*) a; _temp2527;}); _temp2526;}); break;}}} case 51: _LL2513:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2529=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2529[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2530; _temp2530.tag= Cyc_TypeSpecifier_tok;
_temp2530.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2530;}); _temp2529;}); break; case 52: _LL2528: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2532=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2532[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2533; _temp2533.tag=
Cyc_TypeSpecifier_tok; _temp2533.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2533;});
_temp2532;}); break; case 53: _LL2531: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2535=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2535[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2536; _temp2536.tag=
Cyc_TypeSpecifier_tok; _temp2536.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2536;}); _temp2535;}); break; case 54: _LL2534:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2538=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2538[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2539; _temp2539.tag= Cyc_TypeSpecifier_tok;
_temp2539.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct* _temp2540=(
struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2540[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2541; _temp2541.tag=
Cyc_Parse_Short_spec; _temp2541.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2541;});
_temp2540;})); _temp2539;}); _temp2538;}); break; case 55: _LL2537: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2543=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2543[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2544; _temp2544.tag= Cyc_TypeSpecifier_tok;
_temp2544.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2544;}); _temp2543;}); break; case 56: _LL2542: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2546=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2546[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2547; _temp2547.tag=
Cyc_TypeSpecifier_tok; _temp2547.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2548=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2548[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2549; _temp2549.tag= Cyc_Parse_Long_spec;
_temp2549.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2549;});
_temp2548;})); _temp2547;}); _temp2546;}); break; case 57: _LL2545: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2551=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2551[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2552; _temp2552.tag= Cyc_TypeSpecifier_tok;
_temp2552.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2552;}); _temp2551;}); break; case 58: _LL2550: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2554=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2554[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2555; _temp2555.tag=
Cyc_TypeSpecifier_tok; _temp2555.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2555;}); _temp2554;}); break; case 59: _LL2553:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2557=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2557[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2558; _temp2558.tag= Cyc_TypeSpecifier_tok;
_temp2558.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct* _temp2559=(
struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2559[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2560; _temp2560.tag=
Cyc_Parse_Signed_spec; _temp2560.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2560;});
_temp2559;})); _temp2558;}); _temp2557;}); break; case 60: _LL2556: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2562=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2562[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2563; _temp2563.tag= Cyc_TypeSpecifier_tok;
_temp2563.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2564=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2564[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2565; _temp2565.tag= Cyc_Parse_Unsigned_spec; _temp2565.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2565;}); _temp2564;})); _temp2563;}); _temp2562;}); break; case 61: _LL2561:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 62:
_LL2566: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 63:
_LL2567: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 64:
_LL2568: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2570=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2570[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2571; _temp2571.tag=
Cyc_TypeSpecifier_tok; _temp2571.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2571;}); _temp2570;}); break; case 65: _LL2569: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2573=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2573[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2574; _temp2574.tag=
Cyc_TypeSpecifier_tok; _temp2574.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2574;}); _temp2573;}); break; case 66: _LL2572: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2576=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2576[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2577; _temp2577.tag=
Cyc_TypeSpecifier_tok; _temp2577.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2578=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2578[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp2579; _temp2579.tag= Cyc_Absyn_TypedefType;
_temp2579.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2579.f2= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2579.f3= 0; _temp2579;}); _temp2578;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2577;}); _temp2576;}); break; case 67: _LL2575: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2581=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2581[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2582; _temp2582.tag=
Cyc_TypeSpecifier_tok; _temp2582.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2583=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2583[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2584; _temp2584.tag= Cyc_Absyn_TupleType;
_temp2584.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2584;});
_temp2583;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2582;});
_temp2581;}); break; case 68: _LL2580: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2586=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2586[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2587; _temp2587.tag=
Cyc_TypeSpecifier_tok; _temp2587.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2588=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2588[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2589; _temp2589.tag= Cyc_Absyn_RgnHandleType;
_temp2589.f1=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2589;}); _temp2588;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2587;}); _temp2586;}); break; case 69: _LL2585: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp2591=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2591[ 0]=({ struct Cyc_Kind_tok_struct _temp2592; _temp2592.tag= Cyc_Kind_tok;
_temp2592.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2592;}); _temp2591;}); break; case 70: _LL2590: { struct _tagged_string*
_temp2596; void* _temp2598; struct _tuple1 _temp2594=* Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _LL2599:
_temp2598= _temp2594.f1; goto _LL2597; _LL2597: _temp2596= _temp2594.f2; goto
_LL2595; _LL2595: if( _temp2598 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(({
unsigned char* _temp2600="bad kind in type specifier"; struct _tagged_string
_temp2601; _temp2601.curr= _temp2600; _temp2601.base= _temp2600; _temp2601.last_plus_one=
_temp2600 + 27; _temp2601;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Kind_tok_struct* _temp2602=( struct Cyc_Kind_tok_struct*) GC_malloc(
sizeof( struct Cyc_Kind_tok_struct)); _temp2602[ 0]=({ struct Cyc_Kind_tok_struct
_temp2603; _temp2603.tag= Cyc_Kind_tok; _temp2603.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2596, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2603;});
_temp2602;}); break;} case 71: _LL2593: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2605=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2605[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2606; _temp2606.tag= Cyc_TypeQual_tok;
_temp2606.f1=({ struct Cyc_Absyn_Tqual _temp2607; _temp2607.q_const= 1;
_temp2607.q_volatile= 0; _temp2607.q_restrict= 0; _temp2607;}); _temp2606;});
_temp2605;}); break; case 72: _LL2604: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2609=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2609[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2610; _temp2610.tag= Cyc_TypeQual_tok;
_temp2610.f1=({ struct Cyc_Absyn_Tqual _temp2611; _temp2611.q_const= 0;
_temp2611.q_volatile= 1; _temp2611.q_restrict= 0; _temp2611;}); _temp2610;});
_temp2609;}); break; case 73: _LL2608: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2613=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2613[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2614; _temp2614.tag= Cyc_TypeQual_tok;
_temp2614.f1=({ struct Cyc_Absyn_Tqual _temp2615; _temp2615.q_const= 0;
_temp2615.q_volatile= 0; _temp2615.q_restrict= 1; _temp2615;}); _temp2614;});
_temp2613;}); break; case 74: _LL2612: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2617=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2617[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2618; _temp2618.tag=
Cyc_TypeSpecifier_tok; _temp2618.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2619=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2619[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2620; _temp2620.tag= Cyc_Parse_Decl_spec;
_temp2620.f1=({ struct Cyc_Absyn_Decl* _temp2621=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2621->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2622=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2622[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2623; _temp2623.tag= Cyc_Absyn_Enum_d; _temp2623.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2624=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2624->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2624->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2624->fields=({
struct Cyc_Core_Opt* _temp2625=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2625->v=( void*) Cyc_yyget_EnumfieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2625;}); _temp2624;}); _temp2623;});
_temp2622;})); _temp2621->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2621;});
_temp2620;}); _temp2619;})); _temp2618;}); _temp2617;}); break; case 75: _LL2616:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2627=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2627[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2628; _temp2628.tag= Cyc_TypeSpecifier_tok;
_temp2628.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2629=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2629[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2630; _temp2630.tag= Cyc_Absyn_EnumType;
_temp2630.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2630.f2= 0; _temp2630;}); _temp2629;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2628;}); _temp2627;}); break; case 76: _LL2626: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2632=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2632[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2633; _temp2633.tag= Cyc_Enumfield_tok;
_temp2633.f1=({ struct Cyc_Absyn_Enumfield* _temp2634=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2634->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2634->tag=
0; _temp2634->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2634;});
_temp2633;}); _temp2632;}); break; case 77: _LL2631: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2636=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2636[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2637; _temp2637.tag= Cyc_Enumfield_tok;
_temp2637.f1=({ struct Cyc_Absyn_Enumfield* _temp2638=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2638->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2638->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2638->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2638;});
_temp2637;}); _temp2636;}); break; case 78: _LL2635: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2640=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2640[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2641; _temp2641.tag=
Cyc_EnumfieldList_tok; _temp2641.f1=({ struct Cyc_List_List* _temp2642=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2642->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2642->tl= 0; _temp2642;}); _temp2641;}); _temp2640;}); break; case 79:
_LL2639: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp2644=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2644[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2645; _temp2645.tag=
Cyc_EnumfieldList_tok; _temp2645.f1=({ struct Cyc_List_List* _temp2646=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2646->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2646->tl= Cyc_yyget_EnumfieldList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2646;}); _temp2645;}); _temp2644;}); break; case 80:
_LL2643: { void* t;{ void* _temp2648= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _LL2650: if( _temp2648 ==( void*) Cyc_Parse_Struct_su){
goto _LL2651;} else{ goto _LL2652;} _LL2652: if( _temp2648 ==( void*) Cyc_Parse_Union_su){
goto _LL2653;} else{ goto _LL2649;} _LL2651: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2654=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2654[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2655; _temp2655.tag= Cyc_Absyn_AnonStructType; _temp2655.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2655;});
_temp2654;}); goto _LL2649; _LL2653: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp2656=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp2656[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp2657; _temp2657.tag= Cyc_Absyn_AnonUnionType; _temp2657.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2657;});
_temp2656;}); goto _LL2649; _LL2649:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2658=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2658[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2659; _temp2659.tag=
Cyc_TypeSpecifier_tok; _temp2659.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp2660=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2660[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2661; _temp2661.tag= Cyc_Parse_Type_spec;
_temp2661.f1=( void*) t; _temp2661.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2661;}); _temp2660;})); _temp2659;}); _temp2658;}); break;} case 81:
_LL2647: { struct Cyc_List_List* _temp2663=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2664; _LL2664: { struct Cyc_Absyn_Decl* d;{ void* _temp2665= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2667:
if( _temp2665 ==( void*) Cyc_Parse_Struct_su){ goto _LL2668;} else{ goto _LL2669;}
_LL2669: if( _temp2665 ==( void*) Cyc_Parse_Union_su){ goto _LL2670;} else{ goto
_LL2666;} _LL2668: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2671=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2671->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2671;}), _temp2663,({ struct Cyc_Core_Opt* _temp2672=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2672->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2672;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2666;
_LL2670: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2673=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2673->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2673;}), _temp2663,({ struct Cyc_Core_Opt* _temp2674=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2674->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2674;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2666;
_LL2666:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2675=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2675[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2676; _temp2676.tag=
Cyc_TypeSpecifier_tok; _temp2676.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2677=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2677[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2678; _temp2678.tag= Cyc_Parse_Decl_spec;
_temp2678.f1= d; _temp2678;}); _temp2677;})); _temp2676;}); _temp2675;}); break;}}
case 82: _LL2662: { struct Cyc_List_List* _temp2680=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2681; _LL2681: { struct Cyc_Absyn_Decl* d;{ void* _temp2682= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2684:
if( _temp2682 ==( void*) Cyc_Parse_Struct_su){ goto _LL2685;} else{ goto _LL2686;}
_LL2686: if( _temp2682 ==( void*) Cyc_Parse_Union_su){ goto _LL2687;} else{ goto
_LL2683;} _LL2685: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2688=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2688->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2688;}), _temp2680,({ struct Cyc_Core_Opt* _temp2689=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2689->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2689;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2683;
_LL2687: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2690=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2690->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2690;}), _temp2680,({ struct Cyc_Core_Opt* _temp2691=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2691->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2691;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2683;
_LL2683:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2692=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2692[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2693; _temp2693.tag=
Cyc_TypeSpecifier_tok; _temp2693.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2694=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2694[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2695; _temp2695.tag= Cyc_Parse_Decl_spec;
_temp2695.f1= d; _temp2695;}); _temp2694;})); _temp2693;}); _temp2692;}); break;}}
case 83: _LL2679:{ void* _temp2697= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _LL2699: if( _temp2697 ==( void*) Cyc_Parse_Struct_su){
goto _LL2700;} else{ goto _LL2701;} _LL2701: if( _temp2697 ==( void*) Cyc_Parse_Union_su){
goto _LL2702;} else{ goto _LL2698;} _LL2700: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2703=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2703[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2704; _temp2704.tag=
Cyc_TypeSpecifier_tok; _temp2704.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2705=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2705[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2706; _temp2706.tag= Cyc_Absyn_StructType;
_temp2706.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2706.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2706.f3= 0;
_temp2706;}); _temp2705;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2704;});
_temp2703;}); goto _LL2698; _LL2702: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2707=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2707[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2708; _temp2708.tag=
Cyc_TypeSpecifier_tok; _temp2708.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2709=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2709[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2710; _temp2710.tag= Cyc_Absyn_UnionType;
_temp2710.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2710.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2710.f3= 0;
_temp2710;}); _temp2709;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2708;});
_temp2707;}); goto _LL2698; _LL2698:;} break; case 84: _LL2696:{ void* _temp2712=
Cyc_yyget_StructOrUnion_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_LL2714: if( _temp2712 ==( void*) Cyc_Parse_Struct_su){ goto _LL2715;} else{
goto _LL2716;} _LL2716: if( _temp2712 ==( void*) Cyc_Parse_Union_su){ goto
_LL2717;} else{ goto _LL2713;} _LL2715: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2718=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2718[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2719; _temp2719.tag=
Cyc_TypeSpecifier_tok; _temp2719.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2720=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2720[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2721; _temp2721.tag= Cyc_Absyn_StructType;
_temp2721.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2721.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2721.f3= 0;
_temp2721;}); _temp2720;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2719;});
_temp2718;}); goto _LL2713; _LL2717: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2722=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2722[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2723; _temp2723.tag=
Cyc_TypeSpecifier_tok; _temp2723.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2724=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2724[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2725; _temp2725.tag= Cyc_Absyn_UnionType;
_temp2725.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2725.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2725.f3= 0;
_temp2725;}); _temp2724;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2723;});
_temp2722;}); goto _LL2713; _LL2713:;} break; case 85: _LL2711: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2727=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2727[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2728; _temp2728.tag= Cyc_TypeList_tok; _temp2728.f1= 0; _temp2728;});
_temp2727;}); break; case 86: _LL2726: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2730=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2730[ 0]=({ struct Cyc_TypeList_tok_struct _temp2731; _temp2731.tag= Cyc_TypeList_tok;
_temp2731.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2731;}); _temp2730;}); break; case 87: _LL2729:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2733=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2733[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2734; _temp2734.tag= Cyc_TypeList_tok; _temp2734.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2734;}); _temp2733;}); break;
case 88: _LL2732: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2736=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2736[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2737; _temp2737.tag=
Cyc_StructOrUnion_tok; _temp2737.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp2737;}); _temp2736;}); break; case 89: _LL2735: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2739=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2739[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2740; _temp2740.tag=
Cyc_StructOrUnion_tok; _temp2740.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp2740;}); _temp2739;}); break; case 90: _LL2738: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2742=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2742[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2743; _temp2743.tag= Cyc_StructFieldDeclList_tok; _temp2743.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))); _temp2743;});
_temp2742;}); break; case 91: _LL2741: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2745=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2745[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2746; _temp2746.tag= Cyc_StructFieldDeclListList_tok; _temp2746.f1=({
struct Cyc_List_List* _temp2747=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2747->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2747->tl=
0; _temp2747;}); _temp2746;}); _temp2745;}); break; case 92: _LL2744: yyval=(
void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2749=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp2749[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp2750; _temp2750.tag= Cyc_StructFieldDeclListList_tok;
_temp2750.f1=({ struct Cyc_List_List* _temp2751=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2751->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2751->tl=
Cyc_yyget_StructFieldDeclListList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2751;}); _temp2750;}); _temp2749;}); break; case 93:
_LL2748: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2753=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2753[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2754; _temp2754.tag=
Cyc_InitDeclList_tok; _temp2754.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2754;}); _temp2753;}); break; case 94:
_LL2752: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2756=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2756[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2757; _temp2757.tag=
Cyc_InitDeclList_tok; _temp2757.f1=({ struct Cyc_List_List* _temp2758=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2758->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2758->tl= 0; _temp2758;}); _temp2757;}); _temp2756;}); break; case 95:
_LL2755: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2760=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2760[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2761; _temp2761.tag=
Cyc_InitDeclList_tok; _temp2761.f1=({ struct Cyc_List_List* _temp2762=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2762->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2762->tl= Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2762;}); _temp2761;}); _temp2760;}); break; case 96:
_LL2759: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2764=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2764[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2765; _temp2765.tag= Cyc_InitDecl_tok; _temp2765.f1=({ struct _tuple15*
_temp2766=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2766->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2766->f2= 0; _temp2766;}); _temp2765;}); _temp2764;}); break; case 97:
_LL2763: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2768=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2768[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2769; _temp2769.tag= Cyc_InitDecl_tok; _temp2769.f1=({ struct _tuple15*
_temp2770=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2770->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2770->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2770;}); _temp2769;}); _temp2768;}); break;
case 98: _LL2767: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f1; struct
Cyc_List_List* _temp2772=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f3; goto _LL2773; _LL2773: { void* _temp2774=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
goto _LL2775; _LL2775: { struct Cyc_List_List* _temp2778; struct Cyc_List_List*
_temp2780; struct _tuple0 _temp2776=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _LL2781: _temp2780= _temp2776.f1; goto
_LL2779; _LL2779: _temp2778= _temp2776.f2; goto _LL2777; _LL2777: { struct Cyc_List_List*
_temp2782=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, _temp2774, _temp2780, _temp2772),
_temp2778); goto _LL2783; _LL2783: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2784=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2784[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2785; _temp2785.tag= Cyc_StructFieldDeclList_tok; _temp2785.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
_temp2782); _temp2785;}); _temp2784;}); break;}}}} case 99: _LL2771: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2787=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2787[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2788; _temp2788.tag= Cyc_QualSpecList_tok;
_temp2788.f1=({ struct _tuple18* _temp2789=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2789->f1= Cyc_Absyn_empty_tqual(); _temp2789->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2790=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2790->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2790->tl=
0; _temp2790;}); _temp2789->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp2789;}); _temp2788;}); _temp2787;});
break; case 100: _LL2786: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2792=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2792[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2793; _temp2793.tag=
Cyc_QualSpecList_tok; _temp2793.f1=({ struct _tuple18* _temp2794=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2794->f1=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; _temp2794->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2795=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2795->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2795->tl=(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2;
_temp2795;}); _temp2794->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2794;}); _temp2793;}); _temp2792;});
break; case 101: _LL2791: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2797=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2797[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2798; _temp2798.tag=
Cyc_QualSpecList_tok; _temp2798.f1=({ struct _tuple18* _temp2799=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2799->f1= Cyc_yyget_TypeQual_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2799->f2=
0; _temp2799->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2799;}); _temp2798;}); _temp2797;}); break; case 102:
_LL2796: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2801=( struct
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2801[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2802; _temp2802.tag=
Cyc_QualSpecList_tok; _temp2802.f1=({ struct _tuple18* _temp2803=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2803->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1);
_temp2803->f2=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp2803->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2803;});
_temp2802;}); _temp2801;}); break; case 103: _LL2800: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp2805=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2805[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2806; _temp2806.tag= Cyc_DeclaratorExpoptList_tok;
_temp2806.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))); _temp2806;}); _temp2805;}); break; case 104: _LL2804: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2808=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2808[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2809; _temp2809.tag= Cyc_DeclaratorExpoptList_tok;
_temp2809.f1=({ struct Cyc_List_List* _temp2810=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2810->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2810->tl=
0; _temp2810;}); _temp2809;}); _temp2808;}); break; case 105: _LL2807: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2812=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2812[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2813; _temp2813.tag= Cyc_DeclaratorExpoptList_tok;
_temp2813.f1=({ struct Cyc_List_List* _temp2814=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2814->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2814->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2814;}); _temp2813;}); _temp2812;}); break; case 106:
_LL2811: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2816=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2816[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2817; _temp2817.tag=
Cyc_DeclaratorExpopt_tok; _temp2817.f1=({ struct _tuple13* _temp2818=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2818->f1= Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2818->f2=
0; _temp2818;}); _temp2817;}); _temp2816;}); break; case 107: _LL2815: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2820=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2820[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2821; _temp2821.tag= Cyc_DeclaratorExpopt_tok;
_temp2821.f1=({ struct _tuple13* _temp2822=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2822->f1=({ struct Cyc_Parse_Declarator*
_temp2824=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2824->id=({ struct _tuple1* _temp2825=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2825->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp2829=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp2829[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp2830; _temp2830.tag= Cyc_Absyn_Rel_n;
_temp2830.f1= 0; _temp2830;}); _temp2829;}); _temp2825->f2=({ struct
_tagged_string* _temp2826=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp2826[ 0]=({ unsigned char* _temp2827=""; struct
_tagged_string _temp2828; _temp2828.curr= _temp2827; _temp2828.base= _temp2827;
_temp2828.last_plus_one= _temp2827 + 1; _temp2828;}); _temp2826;}); _temp2825;});
_temp2824->tms= 0; _temp2824;}); _temp2822->f2=({ struct Cyc_Core_Opt* _temp2823=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2823->v=(
void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2823;}); _temp2822;}); _temp2821;}); _temp2820;}); break; case 108: _LL2819:
yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2832=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2832[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2833; _temp2833.tag= Cyc_DeclaratorExpopt_tok;
_temp2833.f1=({ struct _tuple13* _temp2834=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2834->f1= Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2834->f2=({ struct Cyc_Core_Opt*
_temp2835=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2835->v=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2835;}); _temp2834;}); _temp2833;}); _temp2832;}); break;
case 109: _LL2831: { struct Cyc_List_List* _temp2837=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2838; _LL2838: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2839=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2839[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2840; _temp2840.tag=
Cyc_TypeSpecifier_tok; _temp2840.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2841=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2841[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2842; _temp2842.tag= Cyc_Parse_Decl_spec;
_temp2842.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), _temp2837,({
struct Cyc_Core_Opt* _temp2843=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2843->v=( void*) Cyc_yyget_TunionFieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2843;}), Cyc_yyget_Bool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2842;}); _temp2841;})); _temp2840;}); _temp2839;}); break;} case 110:
_LL2836: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2845=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2845[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2846; _temp2846.tag=
Cyc_TypeSpecifier_tok; _temp2846.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2847=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2847[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2848; _temp2848.tag= Cyc_Absyn_TunionType;
_temp2848.f1=({ struct Cyc_Absyn_TunionInfo _temp2849; _temp2849.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2850=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2850[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2851; _temp2851.tag= Cyc_Absyn_UnknownTunion;
_temp2851.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2852; _temp2852.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2852.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2852;}); _temp2851;}); _temp2850;})); _temp2849.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2849.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2849;}); _temp2848;});
_temp2847;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2846;});
_temp2845;}); break; case 111: _LL2844: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2854=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2854[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2855; _temp2855.tag=
Cyc_TypeSpecifier_tok; _temp2855.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2856=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2856[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2857; _temp2857.tag= Cyc_Absyn_TunionType;
_temp2857.f1=({ struct Cyc_Absyn_TunionInfo _temp2858; _temp2858.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2859=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2859[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2860; _temp2860.tag= Cyc_Absyn_UnknownTunion;
_temp2860.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2861; _temp2861.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2861.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp2861;}); _temp2860;}); _temp2859;})); _temp2858.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2858.rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2858;}); _temp2857;}); _temp2856;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2855;}); _temp2854;}); break; case 112: _LL2853: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2863=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2863[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2864; _temp2864.tag= Cyc_TypeSpecifier_tok;
_temp2864.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2865=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2865[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2866; _temp2866.tag= Cyc_Absyn_TunionFieldType; _temp2866.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2867; _temp2867.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2868=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2868[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2869; _temp2869.tag= Cyc_Absyn_UnknownTunionfield; _temp2869.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2870; _temp2870.tunion_name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2870.field_name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2870.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2870;}); _temp2869;}); _temp2868;})); _temp2867.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2867;}); _temp2866;}); _temp2865;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2864;}); _temp2863;}); break; case 113: _LL2862: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2872=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2872[ 0]=({ struct Cyc_Bool_tok_struct
_temp2873; _temp2873.tag= Cyc_Bool_tok; _temp2873.f1= 0; _temp2873;}); _temp2872;});
break; case 114: _LL2871: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2875=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2875[ 0]=({ struct Cyc_Bool_tok_struct _temp2876; _temp2876.tag= Cyc_Bool_tok;
_temp2876.f1= 1; _temp2876;}); _temp2875;}); break; case 115: _LL2874: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2878=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2878[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2879; _temp2879.tag= Cyc_TunionFieldList_tok;
_temp2879.f1=({ struct Cyc_List_List* _temp2880=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2880->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2880->tl=
0; _temp2880;}); _temp2879;}); _temp2878;}); break; case 116: _LL2877: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2882=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2882[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2883; _temp2883.tag= Cyc_TunionFieldList_tok;
_temp2883.f1=({ struct Cyc_List_List* _temp2884=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2884->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2884->tl=
0; _temp2884;}); _temp2883;}); _temp2882;}); break; case 117: _LL2881: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2886=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2886[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2887; _temp2887.tag= Cyc_TunionFieldList_tok;
_temp2887.f1=({ struct Cyc_List_List* _temp2888=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2888->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2888->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2888;}); _temp2887;}); _temp2886;}); break; case 118: _LL2885: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2890=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2890[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2891; _temp2891.tag= Cyc_TunionFieldList_tok;
_temp2891.f1=({ struct Cyc_List_List* _temp2892=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2892->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2892->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2892;}); _temp2891;}); _temp2890;}); break; case 119: _LL2889: yyval=( void*)({
struct Cyc_Scope_tok_struct* _temp2894=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2894[ 0]=({ struct Cyc_Scope_tok_struct
_temp2895; _temp2895.tag= Cyc_Scope_tok; _temp2895.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2895;}); _temp2894;}); break; case 120: _LL2893: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2897=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2897[ 0]=({ struct Cyc_Scope_tok_struct
_temp2898; _temp2898.tag= Cyc_Scope_tok; _temp2898.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2898;}); _temp2897;}); break; case 121: _LL2896: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2900=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2900[ 0]=({ struct Cyc_Scope_tok_struct
_temp2901; _temp2901.tag= Cyc_Scope_tok; _temp2901.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2901;}); _temp2900;}); break; case 122: _LL2899: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2903=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2903[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2904; _temp2904.tag= Cyc_TunionField_tok;
_temp2904.f1=({ struct Cyc_Absyn_Tunionfield* _temp2905=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2905->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2905->tvs=
0; _temp2905->typs= 0; _temp2905->loc= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2905->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2905;}); _temp2904;}); _temp2903;}); break; case 123:
_LL2902: { struct Cyc_List_List* _temp2907=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2908; _LL2908: { struct Cyc_List_List* _temp2909=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2910; _LL2910: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2911=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2911[ 0]=({ struct Cyc_TunionField_tok_struct _temp2912; _temp2912.tag= Cyc_TunionField_tok;
_temp2912.f1=({ struct Cyc_Absyn_Tunionfield* _temp2913=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2913->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2913->tvs=
_temp2909; _temp2913->typs= _temp2907; _temp2913->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2913->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp2913;}); _temp2912;}); _temp2911;}); break;}} case 124:
_LL2906: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 125:
_LL2914: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2916=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2916[ 0]=({ struct Cyc_Declarator_tok_struct _temp2917; _temp2917.tag= Cyc_Declarator_tok;
_temp2917.f1=({ struct Cyc_Parse_Declarator* _temp2918=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2918->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->id; _temp2918->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms);
_temp2918;}); _temp2917;}); _temp2916;}); break; case 126: _LL2915: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2920=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2920[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2921; _temp2921.tag= Cyc_Declarator_tok;
_temp2921.f1=({ struct Cyc_Parse_Declarator* _temp2922=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2922->id= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2922->tms=
0; _temp2922;}); _temp2921;}); _temp2920;}); break; case 127: _LL2919: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1); break; case
128: _LL2923: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2925=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2925[ 0]=({ struct Cyc_Declarator_tok_struct _temp2926; _temp2926.tag= Cyc_Declarator_tok;
_temp2926.f1=({ struct Cyc_Parse_Declarator* _temp2927=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2927->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2927->tms=({ struct Cyc_List_List* _temp2928=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2928->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2928->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)))->tms; _temp2928;}); _temp2927;}); _temp2926;}); _temp2925;});
break; case 129: _LL2924: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2930=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2930[ 0]=({ struct Cyc_Declarator_tok_struct _temp2931; _temp2931.tag= Cyc_Declarator_tok;
_temp2931.f1=({ struct Cyc_Parse_Declarator* _temp2932=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2932->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2932->tms=({ struct Cyc_List_List* _temp2933=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2933->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2934=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2934[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2935; _temp2935.tag= Cyc_Absyn_ConstArray_mod;
_temp2935.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2935;}); _temp2934;})); _temp2933->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2933;}); _temp2932;});
_temp2931;}); _temp2930;}); break; case 130: _LL2929: { struct _tuple17
_temp2939; struct Cyc_Core_Opt* _temp2940; struct Cyc_Absyn_VarargInfo*
_temp2942; int _temp2944; struct Cyc_List_List* _temp2946; struct _tuple17*
_temp2937= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2939=* _temp2937; _LL2947: _temp2946= _temp2939.f1;
goto _LL2945; _LL2945: _temp2944= _temp2939.f2; goto _LL2943; _LL2943: _temp2942=
_temp2939.f3; goto _LL2941; _LL2941: _temp2940= _temp2939.f4; goto _LL2938;
_LL2938: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2948=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2948[ 0]=({ struct Cyc_Declarator_tok_struct _temp2949; _temp2949.tag= Cyc_Declarator_tok;
_temp2949.f1=({ struct Cyc_Parse_Declarator* _temp2950=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2950->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2950->tms=({ struct Cyc_List_List* _temp2951=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2951->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2952=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2952[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2953; _temp2953.tag= Cyc_Absyn_Function_mod;
_temp2953.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2954=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2954[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2955; _temp2955.tag= Cyc_Absyn_WithTypes;
_temp2955.f1= _temp2946; _temp2955.f2= _temp2944; _temp2955.f3= _temp2942;
_temp2955.f4= _temp2940; _temp2955;}); _temp2954;})); _temp2953;}); _temp2952;}));
_temp2951->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)))->tms; _temp2951;}); _temp2950;}); _temp2949;}); _temp2948;});
break;} case 131: _LL2936: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2957=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2957[ 0]=({ struct Cyc_Declarator_tok_struct _temp2958; _temp2958.tag= Cyc_Declarator_tok;
_temp2958.f1=({ struct Cyc_Parse_Declarator* _temp2959=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2959->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2959->tms=({ struct Cyc_List_List* _temp2960=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2960->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2961=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2961[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2962; _temp2962.tag= Cyc_Absyn_Function_mod;
_temp2962.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2963=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2963[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2964; _temp2964.tag= Cyc_Absyn_WithTypes;
_temp2964.f1= 0; _temp2964.f2= 0; _temp2964.f3= 0; _temp2964.f4= 0; _temp2964;});
_temp2963;})); _temp2962;}); _temp2961;})); _temp2960->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp2960;}); _temp2959;}); _temp2958;}); _temp2957;}); break; case 132: _LL2956:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2966=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2966[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2967; _temp2967.tag= Cyc_Declarator_tok;
_temp2967.f1=({ struct Cyc_Parse_Declarator* _temp2968=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2968->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->id;
_temp2968->tms=({ struct Cyc_List_List* _temp2969=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2969->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2970=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2970[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2971; _temp2971.tag= Cyc_Absyn_Function_mod;
_temp2971.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2972=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2972[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2973; _temp2973.tag= Cyc_Absyn_WithTypes;
_temp2973.f1= 0; _temp2973.f2= 0; _temp2973.f3= 0; _temp2973.f4=({ struct Cyc_Core_Opt*
_temp2974=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2974->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2975=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2975[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2976; _temp2976.tag= Cyc_Absyn_JoinEff;
_temp2976.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2976;}); _temp2975;})); _temp2974;}); _temp2973;});
_temp2972;})); _temp2971;}); _temp2970;})); _temp2969->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp2969;}); _temp2968;}); _temp2967;}); _temp2966;}); break; case 133: _LL2965:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2978=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2978[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2979; _temp2979.tag= Cyc_Declarator_tok;
_temp2979.f1=({ struct Cyc_Parse_Declarator* _temp2980=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2980->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2980->tms=({ struct Cyc_List_List* _temp2981=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2981->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2982=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2982[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2983; _temp2983.tag= Cyc_Absyn_Function_mod;
_temp2983.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2984=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2984[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2985; _temp2985.tag= Cyc_Absyn_NoTypes;
_temp2985.f1= Cyc_yyget_IdList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2985.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2985;});
_temp2984;})); _temp2983;}); _temp2982;})); _temp2981->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2981;}); _temp2980;}); _temp2979;}); _temp2978;}); break; case 134: _LL2977: {
struct Cyc_List_List* _temp2987=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2988; _LL2988: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2989=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2989[ 0]=({ struct Cyc_Declarator_tok_struct _temp2990; _temp2990.tag= Cyc_Declarator_tok;
_temp2990.f1=({ struct Cyc_Parse_Declarator* _temp2991=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2991->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2991->tms=({ struct Cyc_List_List* _temp2992=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2992->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2993=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2993[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2994; _temp2994.tag= Cyc_Absyn_TypeParams_mod;
_temp2994.f1= _temp2987; _temp2994.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2994.f3= 0; _temp2994;}); _temp2993;})); _temp2992->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2992;}); _temp2991;}); _temp2990;}); _temp2989;}); break;} case 135:
_LL2986:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2996=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2997; _LL2997: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2998=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2998[ 0]=({ struct Cyc_Declarator_tok_struct _temp2999; _temp2999.tag= Cyc_Declarator_tok;
_temp2999.f1=({ struct Cyc_Parse_Declarator* _temp3000=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3000->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp3000->tms=({ struct Cyc_List_List* _temp3001=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3001->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3002=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3002[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3003; _temp3003.tag= Cyc_Absyn_TypeParams_mod;
_temp3003.f1= _temp2996; _temp3003.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3003.f3= 0; _temp3003;}); _temp3002;})); _temp3001->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3001;}); _temp3000;}); _temp2999;}); _temp2998;}); break;} case 136:
_LL2995: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3005=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3005[ 0]=({ struct Cyc_Declarator_tok_struct _temp3006; _temp3006.tag= Cyc_Declarator_tok;
_temp3006.f1=({ struct Cyc_Parse_Declarator* _temp3007=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3007->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))->id;
_temp3007->tms=({ struct Cyc_List_List* _temp3008=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3008->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3009=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3009[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3010; _temp3010.tag= Cyc_Absyn_Attributes_mod;
_temp3010.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3010.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3010;});
_temp3009;})); _temp3008->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)))->tms; _temp3008;}); _temp3007;}); _temp3006;});
_temp3005;}); break; case 137: _LL3004: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3012=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3012[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3013; _temp3013.tag= Cyc_TypeModifierList_tok; _temp3013.f1=({ struct Cyc_List_List*
_temp3014=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3014->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3015=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3015[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3016; _temp3016.tag=
Cyc_Absyn_Pointer_mod; _temp3016.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3016.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3016.f3=
Cyc_Absyn_empty_tqual(); _temp3016;}); _temp3015;})); _temp3014->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), 0); _temp3014;}); _temp3013;}); _temp3012;});
break; case 138: _LL3011: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3018=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3018[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3019; _temp3019.tag= Cyc_TypeModifierList_tok; _temp3019.f1=({ struct Cyc_List_List*
_temp3020=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3020->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3021=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3021[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3022; _temp3022.tag=
Cyc_Absyn_Pointer_mod; _temp3022.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3022.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3022.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3022;}); _temp3021;})); _temp3020->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0); _temp3020;}); _temp3019;}); _temp3018;}); break; case 139: _LL3017: yyval=(
void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3024=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp3024[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp3025; _temp3025.tag= Cyc_TypeModifierList_tok;
_temp3025.f1=({ struct Cyc_List_List* _temp3026=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3026->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp3027=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3027[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3028; _temp3028.tag= Cyc_Absyn_Pointer_mod;
_temp3028.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3028.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3028.f3=
Cyc_Absyn_empty_tqual(); _temp3028;}); _temp3027;})); _temp3026->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_TypeModifierList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3026;}); _temp3025;}); _temp3024;});
break; case 140: _LL3023: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3030=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3030[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3031; _temp3031.tag= Cyc_TypeModifierList_tok; _temp3031.f1=({ struct Cyc_List_List*
_temp3032=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3032->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3033=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3033[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3034; _temp3034.tag=
Cyc_Absyn_Pointer_mod; _temp3034.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3034.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3034.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3034;}); _temp3033;})); _temp3032->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3032;}); _temp3031;}); _temp3030;}); break; case 141: _LL3029: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp3036=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3036[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3037; _temp3037.tag= Cyc_Pointer_Sort_tok;
_temp3037.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp3038=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3038[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3039; _temp3039.tag=
Cyc_Absyn_Nullable_ps; _temp3039.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3039;}); _temp3038;})); _temp3037;}); _temp3036;}); break; case 142:
_LL3035: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3041=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3041[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3042; _temp3042.tag=
Cyc_Pointer_Sort_tok; _temp3042.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3043=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3043[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3044; _temp3044.tag= Cyc_Absyn_NonNullable_ps; _temp3044.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3044;}); _temp3043;})); _temp3042;}); _temp3041;});
break; case 143: _LL3040: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3046=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3046[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3047; _temp3047.tag=
Cyc_Pointer_Sort_tok; _temp3047.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3048=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3048[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3049; _temp3049.tag=
Cyc_Absyn_Nullable_ps; _temp3049.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3049;}); _temp3048;})); _temp3047;});
_temp3046;}); break; case 144: _LL3045: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3051=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3051[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3052; _temp3052.tag=
Cyc_Pointer_Sort_tok; _temp3052.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3053=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3053[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3054; _temp3054.tag= Cyc_Absyn_NonNullable_ps; _temp3054.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3054;});
_temp3053;})); _temp3052;}); _temp3051;}); break; case 145: _LL3050: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3056=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3056[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3057; _temp3057.tag= Cyc_Pointer_Sort_tok;
_temp3057.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp3057;});
_temp3056;}); break; case 146: _LL3055: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp3059=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3059[ 0]=({ struct Cyc_Rgn_tok_struct _temp3060; _temp3060.tag= Cyc_Rgn_tok;
_temp3060.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3060;}); _temp3059;});
break; case 147: _LL3058: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 148: _LL3061: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3063=(
struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3063[ 0]=({ struct Cyc_Rgn_tok_struct _temp3064; _temp3064.tag= Cyc_Rgn_tok;
_temp3064.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3064;}); _temp3063;});
break; case 149: _LL3062: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({
unsigned char* _temp3066="expecting region kind\n"; struct _tagged_string
_temp3067; _temp3067.curr= _temp3066; _temp3067.base= _temp3066; _temp3067.last_plus_one=
_temp3066 + 23; _temp3067;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Rgn_tok_struct* _temp3068=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp3068[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3069; _temp3069.tag= Cyc_Rgn_tok; _temp3069.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3069;}); _temp3068;}); break; case 150: _LL3065: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp3071=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3071[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3072; _temp3072.tag= Cyc_Rgn_tok; _temp3072.f1=( void*) Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3073=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3073->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp3073;}),
0); _temp3072;}); _temp3071;}); break; case 151: _LL3070: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 152: _LL3074: yyval=( void*)({
struct Cyc_TypeQual_tok_struct* _temp3076=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3076[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3077; _temp3077.tag= Cyc_TypeQual_tok; _temp3077.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3077;}); _temp3076;}); break; case 153: _LL3075: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3079=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3079[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3080; _temp3080.tag= Cyc_ParamDeclListBool_tok;
_temp3080.f1=({ struct _tuple17* _temp3081=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3081->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp3081->f2= 0; _temp3081->f3= 0;
_temp3081->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3081;}); _temp3080;}); _temp3079;}); break; case 154:
_LL3078: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp3083=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp3083[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp3084; _temp3084.tag=
Cyc_ParamDeclListBool_tok; _temp3084.f1=({ struct _tuple17* _temp3085=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp3085->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp3085->f2=
1; _temp3085->f3= 0; _temp3085->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3085;}); _temp3084;}); _temp3083;}); break;
case 155: _LL3082: { void* _temp3089; struct Cyc_Absyn_Tqual _temp3091; struct
Cyc_Core_Opt* _temp3093; struct _tuple2 _temp3087=* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _LL3094:
_temp3093= _temp3087.f1; goto _LL3092; _LL3092: _temp3091= _temp3087.f2; goto
_LL3090; _LL3090: _temp3089= _temp3087.f3; goto _LL3088; _LL3088: { struct Cyc_Absyn_VarargInfo*
_temp3096=({ struct Cyc_Absyn_VarargInfo* _temp3095=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp3095->name= _temp3093;
_temp3095->tq= _temp3091; _temp3095->type=( void*) _temp3089; _temp3095->rgn=(
void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp3095->inject= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3095;}); goto _LL3097; _LL3097: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3098=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3098[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3099; _temp3099.tag= Cyc_ParamDeclListBool_tok;
_temp3099.f1=({ struct _tuple17* _temp3100=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3100->f1= 0; _temp3100->f2= 0; _temp3100->f3=
_temp3096; _temp3100->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3100;}); _temp3099;}); _temp3098;}); break;}}
case 156: _LL3086: { void* _temp3104; struct Cyc_Absyn_Tqual _temp3106; struct
Cyc_Core_Opt* _temp3108; struct _tuple2 _temp3102=* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _LL3109:
_temp3108= _temp3102.f1; goto _LL3107; _LL3107: _temp3106= _temp3102.f2; goto
_LL3105; _LL3105: _temp3104= _temp3102.f3; goto _LL3103; _LL3103: { struct Cyc_Absyn_VarargInfo*
_temp3111=({ struct Cyc_Absyn_VarargInfo* _temp3110=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp3110->name= _temp3108;
_temp3110->tq= _temp3106; _temp3110->type=( void*) _temp3104; _temp3110->rgn=(
void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp3110->inject= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3110;}); goto _LL3112; _LL3112: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3113=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3113[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3114; _temp3114.tag= Cyc_ParamDeclListBool_tok;
_temp3114.f1=({ struct _tuple17* _temp3115=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3115->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6))); _temp3115->f2= 0; _temp3115->f3= _temp3111;
_temp3115->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3115;}); _temp3114;}); _temp3113;}); break;}} case 157:
_LL3101: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct* _temp3117=( struct Cyc_TypeOpt_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp3117[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp3118; _temp3118.tag= Cyc_TypeOpt_tok; _temp3118.f1= 0; _temp3118;});
_temp3117;}); break; case 158: _LL3116: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp3120=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp3120[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp3121; _temp3121.tag= Cyc_TypeOpt_tok;
_temp3121.f1=({ struct Cyc_Core_Opt* _temp3122=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3122->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3123=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3123[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3124; _temp3124.tag= Cyc_Absyn_JoinEff;
_temp3124.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3124;}); _temp3123;})); _temp3122;}); _temp3121;});
_temp3120;}); break; case 159: _LL3119: yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp3126=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp3126[ 0]=({ struct Cyc_Bool_tok_struct _temp3127; _temp3127.tag= Cyc_Bool_tok;
_temp3127.f1= 0; _temp3127;}); _temp3126;}); break; case 160: _LL3125: { struct
_tagged_string _temp3129= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); goto _LL3130; _LL3130: if( Cyc_String_zstrcmp(
_temp3129,({ unsigned char* _temp3131="inject"; struct _tagged_string _temp3132;
_temp3132.curr= _temp3131; _temp3132.base= _temp3131; _temp3132.last_plus_one=
_temp3131 + 7; _temp3132;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp3133="missing type in function declaration";
struct _tagged_string _temp3134; _temp3134.curr= _temp3133; _temp3134.base=
_temp3133; _temp3134.last_plus_one= _temp3133 + 37; _temp3134;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3135=( struct Cyc_Bool_tok_struct*)
GC_malloc( sizeof( struct Cyc_Bool_tok_struct)); _temp3135[ 0]=({ struct Cyc_Bool_tok_struct
_temp3136; _temp3136.tag= Cyc_Bool_tok; _temp3136.f1= 1; _temp3136;}); _temp3135;});
break;} case 161: _LL3128: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 162: _LL3137: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3139=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3139[ 0]=({ struct Cyc_TypeList_tok_struct _temp3140; _temp3140.tag= Cyc_TypeList_tok;
_temp3140.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3140;}); _temp3139;}); break; case 163:
_LL3138: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3142=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3142[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3143; _temp3143.tag= Cyc_TypeList_tok; _temp3143.f1= 0; _temp3143;});
_temp3142;}); break; case 164: _LL3141: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 165: _LL3144: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3146=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3146[ 0]=({ struct Cyc_TypeList_tok_struct _temp3147; _temp3147.tag= Cyc_TypeList_tok;
_temp3147.f1=({ struct Cyc_List_List* _temp3148=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3148->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp3148->tl= 0; _temp3148;}); _temp3147;}); _temp3146;}); break; case 166:
_LL3145: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err(({ unsigned char* _temp3150="expecing effect kind (E)";
struct _tagged_string _temp3151; _temp3151.curr= _temp3150; _temp3151.base=
_temp3150; _temp3151.last_plus_one= _temp3150 + 25; _temp3151;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3152=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3152[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3153; _temp3153.tag= Cyc_TypeList_tok; _temp3153.f1=({ struct Cyc_List_List*
_temp3154=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3154->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp3154->tl= 0; _temp3154;});
_temp3153;}); _temp3152;}); break; case 167: _LL3149: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp3156=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3156[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3157; _temp3157.tag= Cyc_TypeList_tok; _temp3157.f1=({ struct Cyc_List_List*
_temp3158=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3158->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3159=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3159[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3160; _temp3160.tag= Cyc_Absyn_AccessEff;
_temp3160.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3160;}); _temp3159;}));
_temp3158->tl= 0; _temp3158;}); _temp3157;}); _temp3156;}); break; case 168:
_LL3155: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3162=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3162[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3163; _temp3163.tag= Cyc_TypeList_tok; _temp3163.f1=({ struct Cyc_List_List*
_temp3164=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3164->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3165=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3165[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3166; _temp3166.tag= Cyc_Absyn_AccessEff;
_temp3166.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3166;}); _temp3165;}));
_temp3164->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3164;}); _temp3163;}); _temp3162;}); break; case 169:
_LL3161: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ unsigned char* _temp3168="expecting region kind (R)";
struct _tagged_string _temp3169; _temp3169.curr= _temp3168; _temp3169.base=
_temp3168; _temp3169.last_plus_one= _temp3168 + 26; _temp3169;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3170=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3170[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3171; _temp3171.tag= Cyc_TypeList_tok; _temp3171.f1=({ struct Cyc_List_List*
_temp3172=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3172->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3173=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3173[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3174; _temp3174.tag= Cyc_Absyn_AccessEff;
_temp3174.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3174;}); _temp3173;}));
_temp3172->tl= 0; _temp3172;}); _temp3171;}); _temp3170;}); break; case 170:
_LL3167: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ unsigned char* _temp3176="expecting region kind (R)";
struct _tagged_string _temp3177; _temp3177.curr= _temp3176; _temp3177.base=
_temp3176; _temp3177.last_plus_one= _temp3176 + 26; _temp3177;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3178=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3178[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3179; _temp3179.tag= Cyc_TypeList_tok; _temp3179.f1=({ struct Cyc_List_List*
_temp3180=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3180->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3181=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3181[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3182; _temp3182.tag= Cyc_Absyn_AccessEff;
_temp3182.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3182;}); _temp3181;}));
_temp3180->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3180;}); _temp3179;}); _temp3178;}); break; case 171:
_LL3175: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3184=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3184[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3185; _temp3185.tag=
Cyc_ParamDeclList_tok; _temp3185.f1=({ struct Cyc_List_List* _temp3186=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3186->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3186->tl= 0; _temp3186;}); _temp3185;}); _temp3184;}); break; case 172:
_LL3183: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3188=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3188[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3189; _temp3189.tag=
Cyc_ParamDeclList_tok; _temp3189.f1=({ struct Cyc_List_List* _temp3190=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3190->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3190->tl= Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3190;}); _temp3189;}); _temp3188;}); break; case 173:
_LL3187: { void* _temp3192= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3193; _LL3193: { struct Cyc_List_List* _temp3194=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3; goto
_LL3195; _LL3195: { struct Cyc_Absyn_Tqual _temp3196=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f1; goto
_LL3197; _LL3197: { struct Cyc_List_List* _temp3198=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms; goto
_LL3199; _LL3199: { struct _tuple6 _temp3200= Cyc_Parse_apply_tms( _temp3196,
_temp3192, _temp3194, _temp3198); goto _LL3201; _LL3201: if( _temp3200.f3 != 0){
Cyc_Parse_err(({ unsigned char* _temp3202="parameter with bad type params";
struct _tagged_string _temp3203; _temp3203.curr= _temp3202; _temp3203.base=
_temp3202; _temp3203.last_plus_one= _temp3202 + 31; _temp3203;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct _tuple1* _temp3204=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->id; goto _LL3205; _LL3205:{ void* _temp3206=(*
_temp3204).f1; struct Cyc_List_List* _temp3216; struct Cyc_List_List* _temp3218;
_LL3208: if( _temp3206 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3209;} else{ goto
_LL3210;} _LL3210: if(( unsigned int) _temp3206 > 1u?*(( int*) _temp3206) == Cyc_Absyn_Rel_n:
0){ _LL3217: _temp3216=(( struct Cyc_Absyn_Rel_n_struct*) _temp3206)->f1; if(
_temp3216 == 0){ goto _LL3211;} else{ goto _LL3212;}} else{ goto _LL3212;}
_LL3212: if(( unsigned int) _temp3206 > 1u?*(( int*) _temp3206) == Cyc_Absyn_Abs_n:
0){ _LL3219: _temp3218=(( struct Cyc_Absyn_Abs_n_struct*) _temp3206)->f1; if(
_temp3218 == 0){ goto _LL3213;} else{ goto _LL3214;}} else{ goto _LL3214;}
_LL3214: goto _LL3215; _LL3209: goto _LL3207; _LL3211: goto _LL3207; _LL3213:
goto _LL3207; _LL3215: Cyc_Parse_err(({ unsigned char* _temp3220="parameter cannot be qualified with a module name";
struct _tagged_string _temp3221; _temp3221.curr= _temp3220; _temp3221.base=
_temp3220; _temp3221.last_plus_one= _temp3220 + 49; _temp3221;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3207; _LL3207:;}{ struct Cyc_Core_Opt* _temp3223=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3222=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3222->v=( void*)(* _temp3204).f2; _temp3222;}); goto
_LL3224; _LL3224: if( _temp3200.f4 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3225="extra attributes on parameter, ignoring"; struct _tagged_string
_temp3226; _temp3226.curr= _temp3225; _temp3226.base= _temp3225; _temp3226.last_plus_one=
_temp3225 + 40; _temp3226;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} if( _temp3200.f3
!= 0){ Cyc_Parse_warn(({ unsigned char* _temp3227="extra type variables on parameter, ignoring";
struct _tagged_string _temp3228; _temp3228.curr= _temp3227; _temp3228.base=
_temp3227; _temp3228.last_plus_one= _temp3227 + 44; _temp3228;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3229=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3229[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3230; _temp3230.tag= Cyc_ParamDecl_tok; _temp3230.f1=({
struct _tuple2* _temp3231=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3231->f1= _temp3223; _temp3231->f2= _temp3200.f1; _temp3231->f3= _temp3200.f2;
_temp3231;}); _temp3230;}); _temp3229;}); break;}}}}}}} case 174: _LL3191: {
void* _temp3233= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3234; _LL3234: { struct Cyc_List_List* _temp3235=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3236; _LL3236: { struct Cyc_Absyn_Tqual _temp3237=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; goto
_LL3238; _LL3238: if( _temp3235 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3239="bad attributes on parameter, ignoring"; struct _tagged_string
_temp3240; _temp3240.curr= _temp3239; _temp3240.base= _temp3239; _temp3240.last_plus_one=
_temp3239 + 38; _temp3240;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp3241=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3241[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3242; _temp3242.tag= Cyc_ParamDecl_tok; _temp3242.f1=({
struct _tuple2* _temp3243=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3243->f1= 0; _temp3243->f2= _temp3237; _temp3243->f3= _temp3233; _temp3243;});
_temp3242;}); _temp3241;}); break;}}} case 175: _LL3232: { void* _temp3245= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3246; _LL3246: { struct Cyc_List_List*
_temp3247=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3248; _LL3248: { struct Cyc_Absyn_Tqual
_temp3249=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3250; _LL3250: { struct Cyc_List_List* _temp3251=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3252; _LL3252: { struct _tuple6 _temp3253= Cyc_Parse_apply_tms(
_temp3249, _temp3245, _temp3247, _temp3251); goto _LL3254; _LL3254: if(
_temp3253.f3 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3255="bad type parameters on formal argument, ignoring";
struct _tagged_string _temp3256; _temp3256.curr= _temp3255; _temp3256.base=
_temp3255; _temp3256.last_plus_one= _temp3255 + 49; _temp3256;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3253.f4 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3257="bad attributes on parameter, ignoring";
struct _tagged_string _temp3258; _temp3258.curr= _temp3257; _temp3258.base=
_temp3257; _temp3258.last_plus_one= _temp3257 + 38; _temp3258;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3259=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3259[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3260; _temp3260.tag= Cyc_ParamDecl_tok; _temp3260.f1=({
struct _tuple2* _temp3261=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3261->f1= 0; _temp3261->f2= _temp3253.f1; _temp3261->f3= _temp3253.f2;
_temp3261;}); _temp3260;}); _temp3259;}); break;}}}}} case 176: _LL3244: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp3263=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp3263[ 0]=({ struct Cyc_IdList_tok_struct
_temp3264; _temp3264.tag= Cyc_IdList_tok; _temp3264.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3264;}); _temp3263;}); break; case
177: _LL3262: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp3266=( struct
Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp3266[ 0]=({ struct Cyc_IdList_tok_struct _temp3267; _temp3267.tag= Cyc_IdList_tok;
_temp3267.f1=({ struct Cyc_List_List* _temp3268=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3268->hd=( void*)({ struct
_tagged_string* _temp3269=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3269[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3269;}); _temp3268->tl= 0; _temp3268;});
_temp3267;}); _temp3266;}); break; case 178: _LL3265: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp3271=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp3271[ 0]=({ struct Cyc_IdList_tok_struct
_temp3272; _temp3272.tag= Cyc_IdList_tok; _temp3272.f1=({ struct Cyc_List_List*
_temp3273=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3273->hd=( void*)({ struct _tagged_string* _temp3274=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3274[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3274;}); _temp3273->tl= Cyc_yyget_IdList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3273;}); _temp3272;}); _temp3271;});
break; case 179: _LL3270: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 180: _LL3275: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 181: _LL3276: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3278=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3278[ 0]=({ struct Cyc_Exp_tok_struct _temp3279; _temp3279.tag= Cyc_Exp_tok;
_temp3279.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3280=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3280[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3281; _temp3281.tag= Cyc_Absyn_UnresolvedMem_e; _temp3281.f1= 0; _temp3281.f2=
0; _temp3281;}); _temp3280;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3279;});
_temp3278;}); break; case 182: _LL3277: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3283=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3283[ 0]=({ struct Cyc_Exp_tok_struct _temp3284; _temp3284.tag= Cyc_Exp_tok;
_temp3284.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3285=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3285[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3286; _temp3286.tag= Cyc_Absyn_UnresolvedMem_e; _temp3286.f1= 0; _temp3286.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3286;});
_temp3285;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3284;});
_temp3283;}); break; case 183: _LL3282: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3288=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3288[ 0]=({ struct Cyc_Exp_tok_struct _temp3289; _temp3289.tag= Cyc_Exp_tok;
_temp3289.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3290=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3290[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3291; _temp3291.tag= Cyc_Absyn_UnresolvedMem_e; _temp3291.f1= 0; _temp3291.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3291;});
_temp3290;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3289;});
_temp3288;}); break; case 184: _LL3287: { struct Cyc_Absyn_Vardecl* _temp3295=
Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3293=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp3293->f1=( void*) Cyc_Absyn_Loc_n; _temp3293->f2=({
struct _tagged_string* _temp3294=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3294[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3294;}); _temp3293;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).last_line)));
goto _LL3296; _LL3296: _temp3295->tq=({ struct Cyc_Absyn_Tqual _temp3297;
_temp3297.q_const= 1; _temp3297.q_volatile= 0; _temp3297.q_restrict= 1;
_temp3297;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3298=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3298[ 0]=({ struct Cyc_Exp_tok_struct
_temp3299; _temp3299.tag= Cyc_Exp_tok; _temp3299.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp3300=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp3300[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp3301; _temp3301.tag= Cyc_Absyn_Comprehension_e;
_temp3301.f1= _temp3295; _temp3301.f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3301.f3= Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3301;}); _temp3300;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3299;}); _temp3298;}); break;} case 185: _LL3292: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3303=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3303[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3304; _temp3304.tag= Cyc_InitializerList_tok;
_temp3304.f1=({ struct Cyc_List_List* _temp3305=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3305->hd=( void*)({ struct
_tuple19* _temp3306=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3306->f1= 0; _temp3306->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3306;}); _temp3305->tl= 0; _temp3305;});
_temp3304;}); _temp3303;}); break; case 186: _LL3302: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3308=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3308[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3309; _temp3309.tag= Cyc_InitializerList_tok;
_temp3309.f1=({ struct Cyc_List_List* _temp3310=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3310->hd=( void*)({ struct
_tuple19* _temp3311=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3311->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3311->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3311;}); _temp3310->tl= 0; _temp3310;});
_temp3309;}); _temp3308;}); break; case 187: _LL3307: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3313=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3313[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3314; _temp3314.tag= Cyc_InitializerList_tok;
_temp3314.f1=({ struct Cyc_List_List* _temp3315=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3315->hd=( void*)({ struct
_tuple19* _temp3316=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3316->f1= 0; _temp3316->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3316;}); _temp3315->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3315;});
_temp3314;}); _temp3313;}); break; case 188: _LL3312: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3318=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3318[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3319; _temp3319.tag= Cyc_InitializerList_tok;
_temp3319.f1=({ struct Cyc_List_List* _temp3320=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3320->hd=( void*)({ struct
_tuple19* _temp3321=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3321->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3321->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3321;}); _temp3320->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3320;});
_temp3319;}); _temp3318;}); break; case 189: _LL3317: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3323=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3323[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3324; _temp3324.tag= Cyc_DesignatorList_tok;
_temp3324.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3324;}); _temp3323;}); break; case 190: _LL3322: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3326=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3326[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3327; _temp3327.tag= Cyc_DesignatorList_tok;
_temp3327.f1=({ struct Cyc_List_List* _temp3328=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3328->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3328->tl=
0; _temp3328;}); _temp3327;}); _temp3326;}); break; case 191: _LL3325: yyval=(
void*)({ struct Cyc_DesignatorList_tok_struct* _temp3330=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3330[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3331; _temp3331.tag= Cyc_DesignatorList_tok;
_temp3331.f1=({ struct Cyc_List_List* _temp3332=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3332->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3332->tl=
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3332;}); _temp3331;}); _temp3330;}); break; case 192: _LL3329: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp3334=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp3334[ 0]=({ struct
Cyc_Designator_tok_struct _temp3335; _temp3335.tag= Cyc_Designator_tok;
_temp3335.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp3336=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp3336[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp3337; _temp3337.tag=
Cyc_Absyn_ArrayElement; _temp3337.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3337;}); _temp3336;})); _temp3335;});
_temp3334;}); break; case 193: _LL3333: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp3339=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp3339[ 0]=({ struct Cyc_Designator_tok_struct _temp3340; _temp3340.tag= Cyc_Designator_tok;
_temp3340.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3341=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3341[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3342; _temp3342.tag= Cyc_Absyn_FieldName;
_temp3342.f1=({ struct _tagged_string* _temp3343=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3343[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3343;});
_temp3342;}); _temp3341;})); _temp3340;}); _temp3339;}); break; case 194:
_LL3338: { void* _temp3345= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3346; _LL3346: { struct Cyc_List_List* _temp3347=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3348; _LL3348: if( _temp3347 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3349="ignoring attributes in type"; struct _tagged_string _temp3350;
_temp3350.curr= _temp3349; _temp3350.base= _temp3349; _temp3350.last_plus_one=
_temp3349 + 28; _temp3350;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{ struct Cyc_Absyn_Tqual
_temp3351=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; goto _LL3352; _LL3352: yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp3353=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp3353[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp3354; _temp3354.tag= Cyc_ParamDecl_tok;
_temp3354.f1=({ struct _tuple2* _temp3355=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp3355[ 0]=({ struct _tuple2 _temp3356; _temp3356.f1= 0;
_temp3356.f2= _temp3351; _temp3356.f3= _temp3345; _temp3356;}); _temp3355;});
_temp3354;}); _temp3353;}); break;}}} case 195: _LL3344: { void* _temp3358= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3359; _LL3359: { struct Cyc_List_List*
_temp3360=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3361; _LL3361: { struct Cyc_Absyn_Tqual
_temp3362=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3363; _LL3363: { struct Cyc_List_List* _temp3364=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3365; _LL3365: { struct _tuple6 _temp3366= Cyc_Parse_apply_tms(
_temp3362, _temp3358, _temp3360, _temp3364); goto _LL3367; _LL3367: if(
_temp3366.f3 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3368="bad type params, ignoring";
struct _tagged_string _temp3369; _temp3369.curr= _temp3368; _temp3369.base=
_temp3368; _temp3369.last_plus_one= _temp3368 + 26; _temp3369;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3366.f4 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3370="bad specifiers, ignoring";
struct _tagged_string _temp3371; _temp3371.curr= _temp3370; _temp3371.base=
_temp3370; _temp3371.last_plus_one= _temp3370 + 25; _temp3371;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3372=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3372[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3373; _temp3373.tag= Cyc_ParamDecl_tok; _temp3373.f1=({
struct _tuple2* _temp3374=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3374->f1= 0; _temp3374->f2= _temp3366.f1; _temp3374->f3= _temp3366.f2;
_temp3374;}); _temp3373;}); _temp3372;}); break;}}}}} case 196: _LL3357: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp3376=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3376[ 0]=({ struct Cyc_Type_tok_struct
_temp3377; _temp3377.tag= Cyc_Type_tok; _temp3377.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; _temp3377;});
_temp3376;}); break; case 197: _LL3375: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp3379=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3379[ 0]=({ struct Cyc_Type_tok_struct _temp3380; _temp3380.tag= Cyc_Type_tok;
_temp3380.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3381=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3381[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3382; _temp3382.tag= Cyc_Absyn_JoinEff;
_temp3382.f1= 0; _temp3382;}); _temp3381;})); _temp3380;}); _temp3379;}); break;
case 198: _LL3378: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3384=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3384[ 0]=({ struct Cyc_Type_tok_struct _temp3385; _temp3385.tag= Cyc_Type_tok;
_temp3385.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3386=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3386[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3387; _temp3387.tag= Cyc_Absyn_JoinEff;
_temp3387.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3387;}); _temp3386;})); _temp3385;}); _temp3384;});
break; case 199: _LL3383: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3389=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3389[ 0]=({ struct Cyc_Type_tok_struct _temp3390; _temp3390.tag= Cyc_Type_tok;
_temp3390.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3391=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3391[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3392; _temp3392.tag= Cyc_Absyn_JoinEff;
_temp3392.f1=({ struct Cyc_List_List* _temp3393=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3393->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3393->tl=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3393;}); _temp3392;}); _temp3391;})); _temp3390;}); _temp3389;}); break;
case 200: _LL3388: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3395=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3395[ 0]=({ struct Cyc_TypeList_tok_struct _temp3396; _temp3396.tag= Cyc_TypeList_tok;
_temp3396.f1=({ struct Cyc_List_List* _temp3397=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3397->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3397->tl=
0; _temp3397;}); _temp3396;}); _temp3395;}); break; case 201: _LL3394: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp3399=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3399[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3400; _temp3400.tag= Cyc_TypeList_tok; _temp3400.f1=({ struct Cyc_List_List*
_temp3401=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3401->hd=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3401->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3401;}); _temp3400;}); _temp3399;});
break; case 202: _LL3398: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3403=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3403[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3404; _temp3404.tag= Cyc_AbstractDeclarator_tok; _temp3404.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3405=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3405->tms= Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3405;}); _temp3404;}); _temp3403;}); break;
case 203: _LL3402: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 204: _LL3406: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3408=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3408[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3409; _temp3409.tag= Cyc_AbstractDeclarator_tok; _temp3409.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3410=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3410->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tms); _temp3410;}); _temp3409;});
_temp3408;}); break; case 205: _LL3407: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 206: _LL3411: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3413=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3413[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3414; _temp3414.tag= Cyc_AbstractDeclarator_tok; _temp3414.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3415=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3415->tms=({ struct Cyc_List_List* _temp3416=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3416->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3416->tl= 0; _temp3416;}); _temp3415;}); _temp3414;}); _temp3413;}); break;
case 207: _LL3412: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3418=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3418[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3419; _temp3419.tag= Cyc_AbstractDeclarator_tok; _temp3419.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3420=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3420->tms=({ struct Cyc_List_List* _temp3421=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3421->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3421->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)))->tms; _temp3421;}); _temp3420;}); _temp3419;});
_temp3418;}); break; case 208: _LL3417: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3423=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3423[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3424; _temp3424.tag= Cyc_AbstractDeclarator_tok; _temp3424.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3425=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3425->tms=({ struct Cyc_List_List* _temp3426=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3426->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3427=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3427[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3428; _temp3428.tag= Cyc_Absyn_ConstArray_mod;
_temp3428.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3428;}); _temp3427;})); _temp3426->tl= 0; _temp3426;}); _temp3425;});
_temp3424;}); _temp3423;}); break; case 209: _LL3422: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3430=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3430[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3431; _temp3431.tag= Cyc_AbstractDeclarator_tok;
_temp3431.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3432=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3432->tms=({
struct Cyc_List_List* _temp3433=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3433->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3434=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3434[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3435; _temp3435.tag= Cyc_Absyn_ConstArray_mod; _temp3435.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3435;});
_temp3434;})); _temp3433->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp3433;}); _temp3432;});
_temp3431;}); _temp3430;}); break; case 210: _LL3429: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3437=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3437[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3438; _temp3438.tag= Cyc_AbstractDeclarator_tok;
_temp3438.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3439=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3439->tms=({
struct Cyc_List_List* _temp3440=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3440->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3441=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3441[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3442; _temp3442.tag=
Cyc_Absyn_Function_mod; _temp3442.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3443=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3443[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3444; _temp3444.tag= Cyc_Absyn_WithTypes;
_temp3444.f1= 0; _temp3444.f2= 0; _temp3444.f3= 0; _temp3444.f4= 0; _temp3444;});
_temp3443;})); _temp3442;}); _temp3441;})); _temp3440->tl= 0; _temp3440;});
_temp3439;}); _temp3438;}); _temp3437;}); break; case 211: _LL3436: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3446=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3446[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3447; _temp3447.tag= Cyc_AbstractDeclarator_tok;
_temp3447.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3448=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3448->tms=({
struct Cyc_List_List* _temp3449=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3449->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3450=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3450[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3451; _temp3451.tag=
Cyc_Absyn_Function_mod; _temp3451.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3452=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3452[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3453; _temp3453.tag= Cyc_Absyn_WithTypes;
_temp3453.f1= 0; _temp3453.f2= 0; _temp3453.f3= 0; _temp3453.f4=({ struct Cyc_Core_Opt*
_temp3454=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3454->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3455=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3455[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3456; _temp3456.tag= Cyc_Absyn_JoinEff;
_temp3456.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3456;}); _temp3455;})); _temp3454;}); _temp3453;});
_temp3452;})); _temp3451;}); _temp3450;})); _temp3449->tl= 0; _temp3449;});
_temp3448;}); _temp3447;}); _temp3446;}); break; case 212: _LL3445: { struct
_tuple17 _temp3460; struct Cyc_Core_Opt* _temp3461; struct Cyc_Absyn_VarargInfo*
_temp3463; int _temp3465; struct Cyc_List_List* _temp3467; struct _tuple17*
_temp3458= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3460=* _temp3458; _LL3468: _temp3467= _temp3460.f1;
goto _LL3466; _LL3466: _temp3465= _temp3460.f2; goto _LL3464; _LL3464: _temp3463=
_temp3460.f3; goto _LL3462; _LL3462: _temp3461= _temp3460.f4; goto _LL3459;
_LL3459: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3469=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3469[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3470; _temp3470.tag=
Cyc_AbstractDeclarator_tok; _temp3470.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3471=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3471->tms=({ struct Cyc_List_List* _temp3472=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3472->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3473=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3473[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3474; _temp3474.tag= Cyc_Absyn_Function_mod;
_temp3474.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3475=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3475[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3476; _temp3476.tag= Cyc_Absyn_WithTypes;
_temp3476.f1= _temp3467; _temp3476.f2= _temp3465; _temp3476.f3= _temp3463;
_temp3476.f4= _temp3461; _temp3476;}); _temp3475;})); _temp3474;}); _temp3473;}));
_temp3472->tl= 0; _temp3472;}); _temp3471;}); _temp3470;}); _temp3469;}); break;}
case 213: _LL3457: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3478=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3478[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3479; _temp3479.tag= Cyc_AbstractDeclarator_tok; _temp3479.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3480=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3480->tms=({ struct Cyc_List_List* _temp3481=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3481->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3482=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3482[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3483; _temp3483.tag= Cyc_Absyn_Function_mod;
_temp3483.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3484=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3484[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3485; _temp3485.tag= Cyc_Absyn_WithTypes;
_temp3485.f1= 0; _temp3485.f2= 0; _temp3485.f3= 0; _temp3485.f4= 0; _temp3485;});
_temp3484;})); _temp3483;}); _temp3482;})); _temp3481->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp3481;}); _temp3480;}); _temp3479;}); _temp3478;}); break; case 214: _LL3477: {
struct Cyc_Core_Opt* _temp3490=({ struct Cyc_Core_Opt* _temp3487=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3487->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3488=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3488[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3489; _temp3489.tag= Cyc_Absyn_JoinEff; _temp3489.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3489;}); _temp3488;})); _temp3487;}); goto _LL3491; _LL3491: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3492=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3492[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3493; _temp3493.tag= Cyc_AbstractDeclarator_tok;
_temp3493.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3494=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3494->tms=({
struct Cyc_List_List* _temp3495=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3495->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3496=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3496[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3497; _temp3497.tag=
Cyc_Absyn_Function_mod; _temp3497.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3498=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3498[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3499; _temp3499.tag= Cyc_Absyn_WithTypes;
_temp3499.f1= 0; _temp3499.f2= 0; _temp3499.f3= 0; _temp3499.f4= _temp3490;
_temp3499;}); _temp3498;})); _temp3497;}); _temp3496;})); _temp3495->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp3495;}); _temp3494;}); _temp3493;}); _temp3492;}); break;} case 215:
_LL3486: { struct _tuple17 _temp3503; struct Cyc_Core_Opt* _temp3504; struct Cyc_Absyn_VarargInfo*
_temp3506; int _temp3508; struct Cyc_List_List* _temp3510; struct _tuple17*
_temp3501= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3503=* _temp3501; _LL3511: _temp3510= _temp3503.f1;
goto _LL3509; _LL3509: _temp3508= _temp3503.f2; goto _LL3507; _LL3507: _temp3506=
_temp3503.f3; goto _LL3505; _LL3505: _temp3504= _temp3503.f4; goto _LL3502;
_LL3502: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3512=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3512[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3513; _temp3513.tag=
Cyc_AbstractDeclarator_tok; _temp3513.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3514=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3514->tms=({ struct Cyc_List_List* _temp3515=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3515->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3516=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3516[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3517; _temp3517.tag= Cyc_Absyn_Function_mod;
_temp3517.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3518=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3518[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3519; _temp3519.tag= Cyc_Absyn_WithTypes;
_temp3519.f1= _temp3510; _temp3519.f2= _temp3508; _temp3519.f3= _temp3506;
_temp3519.f4= _temp3504; _temp3519;}); _temp3518;})); _temp3517;}); _temp3516;}));
_temp3515->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)))->tms; _temp3515;}); _temp3514;}); _temp3513;});
_temp3512;}); break;} case 216: _LL3500: { struct Cyc_List_List* _temp3521=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))); goto _LL3522; _LL3522: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3523=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3523[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3524; _temp3524.tag= Cyc_AbstractDeclarator_tok;
_temp3524.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3525=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3525->tms=({
struct Cyc_List_List* _temp3526=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3526->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp3527=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp3527[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp3528; _temp3528.tag= Cyc_Absyn_TypeParams_mod; _temp3528.f1= _temp3521;
_temp3528.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3528.f3= 0;
_temp3528;}); _temp3527;})); _temp3526->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3526;}); _temp3525;}); _temp3524;}); _temp3523;}); break;} case 217:
_LL3520:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3530=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3531; _LL3531: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3532=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3532[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3533; _temp3533.tag= Cyc_AbstractDeclarator_tok; _temp3533.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3534=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3534->tms=({ struct Cyc_List_List* _temp3535=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3535->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3536=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3536[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3537; _temp3537.tag= Cyc_Absyn_TypeParams_mod;
_temp3537.f1= _temp3530; _temp3537.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3537.f3= 0; _temp3537;}); _temp3536;})); _temp3535->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3535;}); _temp3534;}); _temp3533;}); _temp3532;}); break;} case 218:
_LL3529: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3539=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3539[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3540; _temp3540.tag=
Cyc_AbstractDeclarator_tok; _temp3540.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3541=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3541->tms=({ struct Cyc_List_List* _temp3542=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3542->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3543=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3543[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3544; _temp3544.tag= Cyc_Absyn_Attributes_mod;
_temp3544.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3544.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3544;});
_temp3543;})); _temp3542->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))->tms; _temp3542;}); _temp3541;});
_temp3540;}); _temp3539;}); break; case 219: _LL3538: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 220: _LL3545: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 221: _LL3546: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 222: _LL3547: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 223: _LL3548: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 224: _LL3549: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 225: _LL3550: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),({
unsigned char* _temp3552="`H"; struct _tagged_string _temp3553; _temp3553.curr=
_temp3552; _temp3553.base= _temp3552; _temp3553.last_plus_one= _temp3552 + 3;
_temp3553;})) == 0){ Cyc_Parse_err(({ unsigned char* _temp3554="bad occurrence of heap region `H";
struct _tagged_string _temp3555; _temp3555.curr= _temp3554; _temp3555.base=
_temp3554; _temp3555.last_plus_one= _temp3554 + 33; _temp3555;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3566=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3566->name=({ struct
_tagged_string* _temp3567=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3567[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3567;}); _temp3566->identity= 0;
_temp3566->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp3566;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3564=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3564[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3565; _temp3565.tag= Cyc_Absyn_VarType;
_temp3565.f1= tv; _temp3565;}); _temp3564;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3556=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3556[ 0]=({ struct Cyc_Stmt_tok_struct _temp3557; _temp3557.tag= Cyc_Stmt_tok;
_temp3557.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3558=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3558[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3559; _temp3559.tag= Cyc_Absyn_Region_s;
_temp3559.f1= tv; _temp3559.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3560=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3560->f1=(
void*) Cyc_Absyn_Loc_n; _temp3560->f2=({ struct _tagged_string* _temp3561=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3561[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3561;}); _temp3560;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3562=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3562[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3563; _temp3563.tag= Cyc_Absyn_RgnHandleType; _temp3563.f1=( void*) t;
_temp3563;}); _temp3562;}), 0); _temp3559.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3559;}); _temp3558;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3557;}); _temp3556;}); break;} case 226: _LL3551: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),({
unsigned char* _temp3569="H"; struct _tagged_string _temp3570; _temp3570.curr=
_temp3569; _temp3570.base= _temp3569; _temp3570.last_plus_one= _temp3569 + 2;
_temp3570;})) == 0){ Cyc_Parse_err(({ unsigned char* _temp3571="bad occurrence of heap region `H";
struct _tagged_string _temp3572; _temp3572.curr= _temp3571; _temp3572.base=
_temp3571; _temp3572.last_plus_one= _temp3571 + 33; _temp3572;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3583=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3583->name=({ struct
_tagged_string* _temp3584=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3584[ 0]=({ struct _tagged_string _temp3585= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); xprintf("`%.*s",
_temp3585.last_plus_one - _temp3585.curr, _temp3585.curr);}); _temp3584;});
_temp3583->identity= 0; _temp3583->kind=(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp3583;}); void* t=( void*)({
struct Cyc_Absyn_VarType_struct* _temp3581=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3581[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3582; _temp3582.tag= Cyc_Absyn_VarType; _temp3582.f1=
tv; _temp3582;}); _temp3581;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3573=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3573[ 0]=({ struct Cyc_Stmt_tok_struct _temp3574; _temp3574.tag= Cyc_Stmt_tok;
_temp3574.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3575=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3575[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3576; _temp3576.tag= Cyc_Absyn_Region_s;
_temp3576.f1= tv; _temp3576.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3577=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3577->f1=(
void*) Cyc_Absyn_Loc_n; _temp3577->f2=({ struct _tagged_string* _temp3578=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3578[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3578;}); _temp3577;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3579=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3579[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3580; _temp3580.tag= Cyc_Absyn_RgnHandleType; _temp3580.f1=( void*) t;
_temp3580;}); _temp3579;}), 0); _temp3576.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3576;}); _temp3575;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3574;}); _temp3573;}); break;} case 227: _LL3568: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3587=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3587[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3588; _temp3588.tag= Cyc_Stmt_tok; _temp3588.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp3589=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp3589[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp3590; _temp3590.tag= Cyc_Absyn_Cut_s; _temp3590.f1= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3590;});
_temp3589;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3588;});
_temp3587;}); break; case 228: _LL3586: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3592=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3592[ 0]=({ struct Cyc_Stmt_tok_struct _temp3593; _temp3593.tag= Cyc_Stmt_tok;
_temp3593.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp3594=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp3594[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp3595; _temp3595.tag= Cyc_Absyn_Splice_s;
_temp3595.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3595;}); _temp3594;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3593;}); _temp3592;}); break; case 229: _LL3591: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3597=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3597[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3598; _temp3598.tag= Cyc_Stmt_tok; _temp3598.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp3599=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp3599[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp3600; _temp3600.tag= Cyc_Absyn_Label_s; _temp3600.f1=({
struct _tagged_string* _temp3601=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3601[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3601;}); _temp3600.f2= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3600;});
_temp3599;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3598;});
_temp3597;}); break; case 230: _LL3596: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3603=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3603[ 0]=({ struct Cyc_Stmt_tok_struct _temp3604; _temp3604.tag= Cyc_Stmt_tok;
_temp3604.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3604;});
_temp3603;}); break; case 231: _LL3602: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3606=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3606[ 0]=({ struct Cyc_Stmt_tok_struct _temp3607; _temp3607.tag= Cyc_Stmt_tok;
_temp3607.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3607;});
_temp3606;}); break; case 232: _LL3605: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3609=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3609[ 0]=({ struct Cyc_Stmt_tok_struct _temp3610; _temp3610.tag= Cyc_Stmt_tok;
_temp3610.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3610;});
_temp3609;}); break; case 233: _LL3608: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 234: _LL3611:{ void* _temp3613= Cyc_yyget_BlockItem_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); struct Cyc_List_List*
_temp3621; struct Cyc_Absyn_Fndecl* _temp3623; struct Cyc_Absyn_Stmt* _temp3625;
_LL3615: if(*(( int*) _temp3613) == Cyc_Parse_TopDecls_bl){ _LL3622: _temp3621=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp3613)->f1; goto _LL3616;} else{ goto
_LL3617;} _LL3617: if(*(( int*) _temp3613) == Cyc_Parse_FnDecl_bl){ _LL3624:
_temp3623=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp3613)->f1; goto _LL3618;}
else{ goto _LL3619;} _LL3619: if(*(( int*) _temp3613) == Cyc_Parse_Stmt_bl){
_LL3626: _temp3625=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3613)->f1; goto
_LL3620;} else{ goto _LL3614;} _LL3616: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3627=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3627[ 0]=({ struct Cyc_Stmt_tok_struct _temp3628; _temp3628.tag= Cyc_Stmt_tok;
_temp3628.f1= Cyc_Parse_flatten_declarations( _temp3621, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line))); _temp3628;}); _temp3627;}); goto _LL3614; _LL3618:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3629=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3629[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3630; _temp3630.tag= Cyc_Stmt_tok; _temp3630.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp3631=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3631[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3632; _temp3632.tag= Cyc_Absyn_Fn_d;
_temp3632.f1= _temp3623; _temp3632;}); _temp3631;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line))); _temp3630;});
_temp3629;}); goto _LL3614; _LL3620: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3633=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3633[ 0]=({ struct Cyc_Stmt_tok_struct _temp3634; _temp3634.tag= Cyc_Stmt_tok;
_temp3634.f1= _temp3625; _temp3634;}); _temp3633;}); goto _LL3614; _LL3614:;}
break; case 235: _LL3612:{ void* _temp3636= Cyc_yyget_BlockItem_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); struct Cyc_List_List* _temp3644;
struct Cyc_Absyn_Fndecl* _temp3646; struct Cyc_Absyn_Stmt* _temp3648; _LL3638:
if(*(( int*) _temp3636) == Cyc_Parse_TopDecls_bl){ _LL3645: _temp3644=(( struct
Cyc_Parse_TopDecls_bl_struct*) _temp3636)->f1; goto _LL3639;} else{ goto _LL3640;}
_LL3640: if(*(( int*) _temp3636) == Cyc_Parse_FnDecl_bl){ _LL3647: _temp3646=((
struct Cyc_Parse_FnDecl_bl_struct*) _temp3636)->f1; goto _LL3641;} else{ goto
_LL3642;} _LL3642: if(*(( int*) _temp3636) == Cyc_Parse_Stmt_bl){ _LL3649:
_temp3648=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3636)->f1; goto _LL3643;}
else{ goto _LL3637;} _LL3639: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3650=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3650[ 0]=({ struct Cyc_Stmt_tok_struct _temp3651; _temp3651.tag= Cyc_Stmt_tok;
_temp3651.f1= Cyc_Parse_flatten_declarations( _temp3644, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp3651;});
_temp3650;}); goto _LL3637; _LL3641: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3652=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3652[ 0]=({ struct Cyc_Stmt_tok_struct _temp3653; _temp3653.tag= Cyc_Stmt_tok;
_temp3653.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp3654=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3654[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3655; _temp3655.tag= Cyc_Absyn_Fn_d;
_temp3655.f1= _temp3646; _temp3655;}); _temp3654;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3653;}); _temp3652;}); goto _LL3637; _LL3643: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3656=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3656[ 0]=({ struct Cyc_Stmt_tok_struct _temp3657; _temp3657.tag= Cyc_Stmt_tok;
_temp3657.f1= Cyc_Absyn_seq_stmt( _temp3648, Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3657;}); _temp3656;}); goto _LL3637; _LL3637:;} break; case 236: _LL3635:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3659=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp3659[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp3660; _temp3660.tag= Cyc_BlockItem_tok; _temp3660.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp3661=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp3661[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp3662; _temp3662.tag= Cyc_Parse_TopDecls_bl;
_temp3662.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3662;}); _temp3661;})); _temp3660;}); _temp3659;}); break;
case 237: _LL3658: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3664=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3664[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3665; _temp3665.tag= Cyc_BlockItem_tok;
_temp3665.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp3666=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp3666[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp3667; _temp3667.tag= Cyc_Parse_Stmt_bl;
_temp3667.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3667;}); _temp3666;})); _temp3665;}); _temp3664;}); break;
case 238: _LL3663: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3669=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3669[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3670; _temp3670.tag= Cyc_BlockItem_tok;
_temp3670.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp3671=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp3671[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp3672; _temp3672.tag= Cyc_Parse_FnDecl_bl;
_temp3672.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3672;}); _temp3671;})); _temp3670;}); _temp3669;}); break;
case 239: _LL3668: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3674=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3674[ 0]=({ struct Cyc_Stmt_tok_struct _temp3675; _temp3675.tag= Cyc_Stmt_tok;
_temp3675.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3675;}); _temp3674;}); break; case 240: _LL3673: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3677=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3677[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3678; _temp3678.tag= Cyc_Stmt_tok; _temp3678.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3678;}); _temp3677;}); break; case 241: _LL3676: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3680=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3680[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3681; _temp3681.tag= Cyc_Stmt_tok; _temp3681.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3681;}); _temp3680;}); break; case 242: _LL3679: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 6)),({
unsigned char* _temp3683="C"; struct _tagged_string _temp3684; _temp3684.curr=
_temp3683; _temp3684.base= _temp3683; _temp3684.last_plus_one= _temp3683 + 2;
_temp3684;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp3685="only switch \"C\" { ... } is allowed";
struct _tagged_string _temp3686; _temp3686.curr= _temp3685; _temp3686.base=
_temp3685; _temp3686.last_plus_one= _temp3685 + 35; _temp3686;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3687=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3687[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3688; _temp3688.tag= Cyc_Stmt_tok; _temp3688.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp3689=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp3689[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp3690; _temp3690.tag= Cyc_Absyn_SwitchC_s;
_temp3690.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp3690.f2= Cyc_yyget_SwitchCClauseList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3690;}); _temp3689;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3688;}); _temp3687;}); break; case 243: _LL3682: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3692=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3692[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3693; _temp3693.tag= Cyc_Stmt_tok; _temp3693.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3693;}); _temp3692;}); break; case 244: _LL3691:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3695=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3695[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3696; _temp3696.tag= Cyc_SwitchClauseList_tok;
_temp3696.f1= 0; _temp3696;}); _temp3695;}); break; case 245: _LL3694: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3698=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3698[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3699; _temp3699.tag= Cyc_SwitchClauseList_tok;
_temp3699.f1=({ struct Cyc_List_List* _temp3700=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3700->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3701=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3701->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3701->pat_vars= 0; _temp3701->where_clause= 0; _temp3701->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3701->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3701;}); _temp3700->tl= 0; _temp3700;});
_temp3699;}); _temp3698;}); break; case 246: _LL3697: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp3703=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3703[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3704; _temp3704.tag= Cyc_SwitchClauseList_tok;
_temp3704.f1=({ struct Cyc_List_List* _temp3705=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3705->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3706=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3706->pattern= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3706->pat_vars= 0; _temp3706->where_clause= 0;
_temp3706->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3706->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3706;});
_temp3705->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3705;}); _temp3704;}); _temp3703;}); break; case 247:
_LL3702: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3708=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp3708[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp3709; _temp3709.tag=
Cyc_SwitchClauseList_tok; _temp3709.f1=({ struct Cyc_List_List* _temp3710=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3710->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp3711=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3711->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3711->pat_vars=
0; _temp3711->where_clause= 0; _temp3711->body= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3711->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp3711;}); _temp3710->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3710;}); _temp3709;}); _temp3708;});
break; case 248: _LL3707: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3713=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3713[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3714; _temp3714.tag= Cyc_SwitchClauseList_tok; _temp3714.f1=({ struct Cyc_List_List*
_temp3715=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3715->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3716=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3716->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3716->pat_vars=
0; _temp3716->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3716->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3716->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3716;}); _temp3715->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3715;}); _temp3714;}); _temp3713;});
break; case 249: _LL3712: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3718=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3718[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3719; _temp3719.tag= Cyc_SwitchClauseList_tok; _temp3719.f1=({ struct Cyc_List_List*
_temp3720=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3720->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3721=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3721->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3721->pat_vars=
0; _temp3721->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3721->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3721->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3721;}); _temp3720->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3720;});
_temp3719;}); _temp3718;}); break; case 250: _LL3717: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp3723=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp3723[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp3724; _temp3724.tag= Cyc_SwitchCClauseList_tok;
_temp3724.f1= 0; _temp3724;}); _temp3723;}); break; case 251: _LL3722: yyval=(
void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp3726=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp3726[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp3727; _temp3727.tag= Cyc_SwitchCClauseList_tok;
_temp3727.f1=({ struct Cyc_List_List* _temp3728=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3728->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp3729=( struct Cyc_Absyn_SwitchC_clause*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp3729->cnst_exp= 0; _temp3729->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Absyn_break_stmt(
0), 0); _temp3729->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3729;});
_temp3728->tl= 0; _temp3728;}); _temp3727;}); _temp3726;}); break; case 252:
_LL3725: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp3731=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp3731[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp3732; _temp3732.tag=
Cyc_SwitchCClauseList_tok; _temp3732.f1=({ struct Cyc_List_List* _temp3733=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3733->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp3734=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp3734->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3734->body= Cyc_Absyn_fallthru_stmt( 0, 0); _temp3734->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3734;}); _temp3733->tl= Cyc_yyget_SwitchCClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3733;});
_temp3732;}); _temp3731;}); break; case 253: _LL3730: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp3736=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp3736[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp3737; _temp3737.tag= Cyc_SwitchCClauseList_tok;
_temp3737.f1=({ struct Cyc_List_List* _temp3738=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3738->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp3739=( struct Cyc_Absyn_SwitchC_clause*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp3739->cnst_exp=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3739->body= Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Absyn_fallthru_stmt( 0, 0), 0); _temp3739->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3739;}); _temp3738->tl= Cyc_yyget_SwitchCClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3738;}); _temp3737;}); _temp3736;});
break; case 254: _LL3735: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3741=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3741[ 0]=({ struct Cyc_Stmt_tok_struct _temp3742; _temp3742.tag= Cyc_Stmt_tok;
_temp3742.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3742;});
_temp3741;}); break; case 255: _LL3740: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3744=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3744[ 0]=({ struct Cyc_Stmt_tok_struct _temp3745; _temp3745.tag= Cyc_Stmt_tok;
_temp3745.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3745;});
_temp3744;}); break; case 256: _LL3743: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3747=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3747[ 0]=({ struct Cyc_Stmt_tok_struct _temp3748; _temp3748.tag= Cyc_Stmt_tok;
_temp3748.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3748;});
_temp3747;}); break; case 257: _LL3746: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3750=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3750[ 0]=({ struct Cyc_Stmt_tok_struct _temp3751; _temp3751.tag= Cyc_Stmt_tok;
_temp3751.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3751;}); _temp3750;}); break; case 258: _LL3749: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3753=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3753[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3754; _temp3754.tag= Cyc_Stmt_tok; _temp3754.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3754;});
_temp3753;}); break; case 259: _LL3752: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3756=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3756[ 0]=({ struct Cyc_Stmt_tok_struct _temp3757; _temp3757.tag= Cyc_Stmt_tok;
_temp3757.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3757;}); _temp3756;}); break; case 260: _LL3755: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3759=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3759[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3760; _temp3760.tag= Cyc_Stmt_tok; _temp3760.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3760;});
_temp3759;}); break; case 261: _LL3758: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3762=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3762[ 0]=({ struct Cyc_Stmt_tok_struct _temp3763; _temp3763.tag= Cyc_Stmt_tok;
_temp3763.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 5)), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3763;}); _temp3762;}); break; case 262: _LL3761: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3765=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3765[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3766; _temp3766.tag= Cyc_Stmt_tok; _temp3766.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 7)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3766;}); _temp3765;}); break; case 263: _LL3764:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3768=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3768[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3769; _temp3769.tag= Cyc_Stmt_tok; _temp3769.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 6)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3769;}); _temp3768;}); break; case 264: _LL3767: { struct Cyc_List_List*
_temp3771= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); goto _LL3772; _LL3772: { struct Cyc_Absyn_Stmt* _temp3773=
Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); goto _LL3774; _LL3774: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3775=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3775[ 0]=({ struct Cyc_Stmt_tok_struct _temp3776; _temp3776.tag= Cyc_Stmt_tok;
_temp3776.f1= Cyc_Parse_flatten_declarations( _temp3771, _temp3773); _temp3776;});
_temp3775;}); break;}} case 265: _LL3770: { struct Cyc_List_List* _temp3778= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); goto
_LL3779; _LL3779: { struct Cyc_Absyn_Stmt* _temp3780= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL3781;
_LL3781: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3782=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3782[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3783; _temp3783.tag= Cyc_Stmt_tok; _temp3783.f1= Cyc_Parse_flatten_declarations(
_temp3778, _temp3780); _temp3783;}); _temp3782;}); break;}} case 266: _LL3777: {
struct Cyc_List_List* _temp3785= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); goto _LL3786; _LL3786: { struct Cyc_Absyn_Stmt*
_temp3787= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3788; _LL3788: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3789=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3789[ 0]=({ struct Cyc_Stmt_tok_struct _temp3790; _temp3790.tag= Cyc_Stmt_tok;
_temp3790.f1= Cyc_Parse_flatten_declarations( _temp3785, _temp3787); _temp3790;});
_temp3789;}); break;}} case 267: _LL3784: { struct Cyc_List_List* _temp3792= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); goto
_LL3793; _LL3793: { struct Cyc_Absyn_Stmt* _temp3794= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3795; _LL3795: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3796=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3796[ 0]=({ struct Cyc_Stmt_tok_struct _temp3797; _temp3797.tag= Cyc_Stmt_tok;
_temp3797.f1= Cyc_Parse_flatten_declarations( _temp3792, _temp3794); _temp3797;});
_temp3796;}); break;}} case 268: _LL3791: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3799=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3799[ 0]=({ struct Cyc_Stmt_tok_struct _temp3800; _temp3800.tag= Cyc_Stmt_tok;
_temp3800.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp3801=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3801[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3801;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3800;});
_temp3799;}); break; case 269: _LL3798: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3803=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3803[ 0]=({ struct Cyc_Stmt_tok_struct _temp3804; _temp3804.tag= Cyc_Stmt_tok;
_temp3804.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3804;}); _temp3803;}); break; case 270: _LL3802: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3806=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3806[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3807; _temp3807.tag= Cyc_Stmt_tok; _temp3807.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3807;}); _temp3806;}); break; case 271: _LL3805: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3809=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3809[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3810; _temp3810.tag= Cyc_Stmt_tok; _temp3810.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3810;}); _temp3809;}); break; case 272:
_LL3808: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3812=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3812[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3813; _temp3813.tag= Cyc_Stmt_tok; _temp3813.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3813;});
_temp3812;}); break; case 273: _LL3811: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3815=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3815[ 0]=({ struct Cyc_Stmt_tok_struct _temp3816; _temp3816.tag= Cyc_Stmt_tok;
_temp3816.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3816;}); _temp3815;}); break; case 274: _LL3814: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3818=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3818[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3819; _temp3819.tag= Cyc_Stmt_tok; _temp3819.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).last_line)); _temp3819;}); _temp3818;}); break; case 275:
_LL3817: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3821=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3821[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3822; _temp3822.tag= Cyc_Stmt_tok; _temp3822.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).last_line)); _temp3822;}); _temp3821;}); break; case 276:
_LL3820: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3824=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3824[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3825; _temp3825.tag= Cyc_Pattern_tok; _temp3825.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3825;});
_temp3824;}); break; case 277: _LL3823: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 278: _LL3826: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3828=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3828[ 0]=({ struct Cyc_Pattern_tok_struct _temp3829; _temp3829.tag= Cyc_Pattern_tok;
_temp3829.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3830=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3830[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3831; _temp3831.tag= Cyc_Absyn_Int_p;
_temp3831.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; _temp3831.f2=(* Cyc_yyget_Int_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))).f2; _temp3831;}); _temp3830;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3829;}); _temp3828;}); break; case 279: _LL3827: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3833=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3833[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3834; _temp3834.tag= Cyc_Pattern_tok; _temp3834.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3835=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3835[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3836; _temp3836.tag= Cyc_Absyn_Int_p; _temp3836.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3836.f2= -(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp3836;}); _temp3835;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3834;}); _temp3833;}); break; case 280: _LL3832: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3838=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3838[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3839; _temp3839.tag= Cyc_Pattern_tok; _temp3839.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3840=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3840[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3841; _temp3841.tag= Cyc_Absyn_Float_p; _temp3841.f1=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3841;}); _temp3840;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3839;});
_temp3838;}); break; case 281: _LL3837: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3843=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3843[ 0]=({ struct Cyc_Pattern_tok_struct _temp3844; _temp3844.tag= Cyc_Pattern_tok;
_temp3844.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3845=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3845[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3846; _temp3846.tag= Cyc_Absyn_Char_p;
_temp3846.f1= Cyc_yyget_Char_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3846;}); _temp3845;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3844;}); _temp3843;}); break; case 282: _LL3842: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3848=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3848[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3849; _temp3849.tag= Cyc_Pattern_tok; _temp3849.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3849;});
_temp3848;}); break; case 283: _LL3847: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3851=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3851[ 0]=({ struct Cyc_Pattern_tok_struct _temp3852; _temp3852.tag= Cyc_Pattern_tok;
_temp3852.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3853=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3853[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3854; _temp3854.tag=
Cyc_Absyn_UnknownId_p; _temp3854.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3854;}); _temp3853;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3852;}); _temp3851;}); break; case 284: _LL3850: { struct Cyc_List_List*
_temp3856=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3857; _LL3857: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3858=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3858[ 0]=({ struct Cyc_Pattern_tok_struct _temp3859; _temp3859.tag= Cyc_Pattern_tok;
_temp3859.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3860=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3860[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3861; _temp3861.tag= Cyc_Absyn_UnknownCall_p; _temp3861.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3861.f2=
_temp3856; _temp3861.f3= Cyc_yyget_PatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3861;}); _temp3860;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3859;}); _temp3858;}); break;} case 285: _LL3855: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3863=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3863[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3864; _temp3864.tag= Cyc_Pattern_tok; _temp3864.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3865=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3865[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3866; _temp3866.tag= Cyc_Absyn_Tuple_p; _temp3866.f1=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3866;}); _temp3865;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3864;});
_temp3863;}); break; case 286: _LL3862: { struct Cyc_List_List* _temp3868=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); goto
_LL3869; _LL3869: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3870=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3870[ 0]=({ struct Cyc_Pattern_tok_struct _temp3871; _temp3871.tag= Cyc_Pattern_tok;
_temp3871.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3872=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3872[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3873; _temp3873.tag= Cyc_Absyn_UnknownFields_p; _temp3873.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3873.f2=
_temp3868; _temp3873.f3= 0; _temp3873;}); _temp3872;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3871;}); _temp3870;}); break;} case 287: _LL3867: { struct Cyc_List_List*
_temp3875=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3876; _LL3876: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3877=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3877[ 0]=({ struct Cyc_Pattern_tok_struct _temp3878; _temp3878.tag= Cyc_Pattern_tok;
_temp3878.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3879=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3879[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3880; _temp3880.tag= Cyc_Absyn_UnknownFields_p; _temp3880.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3880.f2=
_temp3875; _temp3880.f3= Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3880;}); _temp3879;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3878;}); _temp3877;}); break;} case 288: _LL3874: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3882=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3882[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3883; _temp3883.tag= Cyc_Pattern_tok; _temp3883.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3884=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3884[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3885; _temp3885.tag= Cyc_Absyn_Pointer_p;
_temp3885.f1= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3885;}); _temp3884;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3883;}); _temp3882;}); break; case 289: _LL3881: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3887=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3887[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3888; _temp3888.tag= Cyc_Pattern_tok; _temp3888.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3889=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3889[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3890; _temp3890.tag= Cyc_Absyn_Reference_p;
_temp3890.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3891=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3891->f1=( void*) Cyc_Absyn_Loc_n;
_temp3891->f2=({ struct _tagged_string* _temp3892=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3892[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3892;});
_temp3891;}),( void*) Cyc_Absyn_VoidType, 0); _temp3890;}); _temp3889;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3888;}); _temp3887;}); break; case 290: _LL3886: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3894=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3894[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3895; _temp3895.tag= Cyc_PatternList_tok;
_temp3895.f1= 0; _temp3895;}); _temp3894;}); break; case 291: _LL3893: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3897=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3897[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3898; _temp3898.tag= Cyc_PatternList_tok;
_temp3898.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3898;}); _temp3897;}); break; case 292: _LL3896: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3900=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3900[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3901; _temp3901.tag= Cyc_PatternList_tok;
_temp3901.f1=({ struct Cyc_List_List* _temp3902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3902->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3902->tl=
0; _temp3902;}); _temp3901;}); _temp3900;}); break; case 293: _LL3899: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3904=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3904[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3905; _temp3905.tag= Cyc_PatternList_tok;
_temp3905.f1=({ struct Cyc_List_List* _temp3906=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3906->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3906->tl=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3906;}); _temp3905;}); _temp3904;}); break; case 294: _LL3903: yyval=( void*)({
struct Cyc_FieldPattern_tok_struct* _temp3908=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3908[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3909; _temp3909.tag= Cyc_FieldPattern_tok;
_temp3909.f1=({ struct _tuple14* _temp3910=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3910->f1= 0; _temp3910->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3910;});
_temp3909;}); _temp3908;}); break; case 295: _LL3907: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp3912=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3912[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3913; _temp3913.tag= Cyc_FieldPattern_tok;
_temp3913.f1=({ struct _tuple14* _temp3914=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3914->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3914->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3914;});
_temp3913;}); _temp3912;}); break; case 296: _LL3911: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3916=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3916[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3917; _temp3917.tag= Cyc_FieldPatternList_tok;
_temp3917.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3917;}); _temp3916;}); break; case 297: _LL3915: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3919=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3919[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3920; _temp3920.tag= Cyc_FieldPatternList_tok;
_temp3920.f1=({ struct Cyc_List_List* _temp3921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3921->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3921->tl=
0; _temp3921;}); _temp3920;}); _temp3919;}); break; case 298: _LL3918: yyval=(
void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3923=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3923[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3924; _temp3924.tag= Cyc_FieldPatternList_tok;
_temp3924.f1=({ struct Cyc_List_List* _temp3925=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3925->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3925->tl=
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3925;}); _temp3924;}); _temp3923;}); break; case 299: _LL3922: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 300:
_LL3926: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3928=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3928[ 0]=({ struct Cyc_Exp_tok_struct
_temp3929; _temp3929.tag= Cyc_Exp_tok; _temp3929.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3929;}); _temp3928;}); break; case 301: _LL3927: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 302: _LL3930: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3932=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3932[ 0]=({ struct Cyc_Exp_tok_struct
_temp3933; _temp3933.tag= Cyc_Exp_tok; _temp3933.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Primopopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3933;}); _temp3932;}); break; case 303: _LL3931: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3935=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3935[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3936; _temp3936.tag= Cyc_Primopopt_tok; _temp3936.f1=
0; _temp3936;}); _temp3935;}); break; case 304: _LL3934: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3938=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3938[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3939; _temp3939.tag= Cyc_Primopopt_tok; _temp3939.f1=({
struct Cyc_Core_Opt* _temp3940=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3940->v=( void*)(( void*) Cyc_Absyn_Times); _temp3940;});
_temp3939;}); _temp3938;}); break; case 305: _LL3937: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3942=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3942[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3943; _temp3943.tag= Cyc_Primopopt_tok; _temp3943.f1=({
struct Cyc_Core_Opt* _temp3944=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3944->v=( void*)(( void*) Cyc_Absyn_Div); _temp3944;});
_temp3943;}); _temp3942;}); break; case 306: _LL3941: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3946=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3946[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3947; _temp3947.tag= Cyc_Primopopt_tok; _temp3947.f1=({
struct Cyc_Core_Opt* _temp3948=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3948->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3948;});
_temp3947;}); _temp3946;}); break; case 307: _LL3945: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3950=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3950[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3951; _temp3951.tag= Cyc_Primopopt_tok; _temp3951.f1=({
struct Cyc_Core_Opt* _temp3952=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3952->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3952;});
_temp3951;}); _temp3950;}); break; case 308: _LL3949: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3954=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3954[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3955; _temp3955.tag= Cyc_Primopopt_tok; _temp3955.f1=({
struct Cyc_Core_Opt* _temp3956=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3956->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3956;});
_temp3955;}); _temp3954;}); break; case 309: _LL3953: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3958=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3958[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3959; _temp3959.tag= Cyc_Primopopt_tok; _temp3959.f1=({
struct Cyc_Core_Opt* _temp3960=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3960->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3960;});
_temp3959;}); _temp3958;}); break; case 310: _LL3957: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3962=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3962[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3963; _temp3963.tag= Cyc_Primopopt_tok; _temp3963.f1=({
struct Cyc_Core_Opt* _temp3964=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3964->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3964;});
_temp3963;}); _temp3962;}); break; case 311: _LL3961: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3966=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3966[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3967; _temp3967.tag= Cyc_Primopopt_tok; _temp3967.f1=({
struct Cyc_Core_Opt* _temp3968=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3968->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3968;});
_temp3967;}); _temp3966;}); break; case 312: _LL3965: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3970=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3970[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3971; _temp3971.tag= Cyc_Primopopt_tok; _temp3971.f1=({
struct Cyc_Core_Opt* _temp3972=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3972->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3972;});
_temp3971;}); _temp3970;}); break; case 313: _LL3969: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3974=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3974[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3975; _temp3975.tag= Cyc_Primopopt_tok; _temp3975.f1=({
struct Cyc_Core_Opt* _temp3976=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3976->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3976;});
_temp3975;}); _temp3974;}); break; case 314: _LL3973: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 315: _LL3977: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3979=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3979[ 0]=({ struct Cyc_Exp_tok_struct
_temp3980; _temp3980.tag= Cyc_Exp_tok; _temp3980.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3980;}); _temp3979;}); break; case 316: _LL3978: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3982=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3982[ 0]=({ struct Cyc_Exp_tok_struct
_temp3983; _temp3983.tag= Cyc_Exp_tok; _temp3983.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3983;}); _temp3982;}); break; case 317: _LL3981: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3985=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3985[ 0]=({ struct Cyc_Exp_tok_struct
_temp3986; _temp3986.tag= Cyc_Exp_tok; _temp3986.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3986;}); _temp3985;}); break; case 318: _LL3984: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3988=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3988[ 0]=({ struct Cyc_Exp_tok_struct
_temp3989; _temp3989.tag= Cyc_Exp_tok; _temp3989.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3989;}); _temp3988;}); break; case 319: _LL3987: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3991=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3991[ 0]=({ struct Cyc_Exp_tok_struct
_temp3992; _temp3992.tag= Cyc_Exp_tok; _temp3992.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3992;});
_temp3991;}); break; case 320: _LL3990: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3994=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3994[ 0]=({ struct Cyc_Exp_tok_struct _temp3995; _temp3995.tag= Cyc_Exp_tok;
_temp3995.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3995;}); _temp3994;}); break; case 321: _LL3993: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 322: _LL3996: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 323: _LL3997: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3999=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3999[ 0]=({ struct Cyc_Exp_tok_struct
_temp4000; _temp4000.tag= Cyc_Exp_tok; _temp4000.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4000;}); _temp3999;}); break; case 324: _LL3998: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 325: _LL4001: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4003=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4003[ 0]=({ struct Cyc_Exp_tok_struct
_temp4004; _temp4004.tag= Cyc_Exp_tok; _temp4004.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4004;}); _temp4003;}); break; case 326: _LL4002: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 327: _LL4005: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4007=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4007[ 0]=({ struct Cyc_Exp_tok_struct
_temp4008; _temp4008.tag= Cyc_Exp_tok; _temp4008.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4008;});
_temp4007;}); break; case 328: _LL4006: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 329: _LL4009: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4011=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4011[ 0]=({ struct Cyc_Exp_tok_struct _temp4012; _temp4012.tag= Cyc_Exp_tok;
_temp4012.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4012;}); _temp4011;}); break; case 330: _LL4010: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 331: _LL4013: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4015=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4015[ 0]=({ struct Cyc_Exp_tok_struct
_temp4016; _temp4016.tag= Cyc_Exp_tok; _temp4016.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4016;});
_temp4015;}); break; case 332: _LL4014: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 333: _LL4017: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4019=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4019[ 0]=({ struct Cyc_Exp_tok_struct _temp4020; _temp4020.tag= Cyc_Exp_tok;
_temp4020.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4020;});
_temp4019;}); break; case 334: _LL4018: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4022=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4022[ 0]=({ struct Cyc_Exp_tok_struct _temp4023; _temp4023.tag= Cyc_Exp_tok;
_temp4023.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4023;});
_temp4022;}); break; case 335: _LL4021: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 336: _LL4024: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4026=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4026[ 0]=({ struct Cyc_Exp_tok_struct _temp4027; _temp4027.tag= Cyc_Exp_tok;
_temp4027.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4027;});
_temp4026;}); break; case 337: _LL4025: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4029=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4029[ 0]=({ struct Cyc_Exp_tok_struct _temp4030; _temp4030.tag= Cyc_Exp_tok;
_temp4030.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4030;});
_temp4029;}); break; case 338: _LL4028: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4032=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4032[ 0]=({ struct Cyc_Exp_tok_struct _temp4033; _temp4033.tag= Cyc_Exp_tok;
_temp4033.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4033;});
_temp4032;}); break; case 339: _LL4031: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4035=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4035[ 0]=({ struct Cyc_Exp_tok_struct _temp4036; _temp4036.tag= Cyc_Exp_tok;
_temp4036.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4036;});
_temp4035;}); break; case 340: _LL4034: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 341: _LL4037: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4039=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4039[ 0]=({ struct Cyc_Exp_tok_struct _temp4040; _temp4040.tag= Cyc_Exp_tok;
_temp4040.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4040;}); _temp4039;}); break; case 342: _LL4038: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4042=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4042[ 0]=({ struct Cyc_Exp_tok_struct
_temp4043; _temp4043.tag= Cyc_Exp_tok; _temp4043.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4043;});
_temp4042;}); break; case 343: _LL4041: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 344: _LL4044: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4046=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4046[ 0]=({ struct Cyc_Exp_tok_struct _temp4047; _temp4047.tag= Cyc_Exp_tok;
_temp4047.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4047;}); _temp4046;}); break; case 345: _LL4045: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4049=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4049[ 0]=({ struct Cyc_Exp_tok_struct
_temp4050; _temp4050.tag= Cyc_Exp_tok; _temp4050.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4050;});
_temp4049;}); break; case 346: _LL4048: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 347: _LL4051: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4053=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4053[ 0]=({ struct Cyc_Exp_tok_struct _temp4054; _temp4054.tag= Cyc_Exp_tok;
_temp4054.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4054;}); _temp4053;}); break; case 348: _LL4052: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4056=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4056[ 0]=({ struct Cyc_Exp_tok_struct
_temp4057; _temp4057.tag= Cyc_Exp_tok; _temp4057.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4057;});
_temp4056;}); break; case 349: _LL4055: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4059=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4059[ 0]=({ struct Cyc_Exp_tok_struct _temp4060; _temp4060.tag= Cyc_Exp_tok;
_temp4060.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4060;}); _temp4059;}); break; case 350: _LL4058: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 351: _LL4061: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4063=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4063[ 0]=({ struct Cyc_Exp_tok_struct
_temp4064; _temp4064.tag= Cyc_Exp_tok; _temp4064.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f3, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4064;}); _temp4063;}); break; case 352: _LL4062: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 353: _LL4065: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4067=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4067[ 0]=({ struct Cyc_Exp_tok_struct
_temp4068; _temp4068.tag= Cyc_Exp_tok; _temp4068.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4068;}); _temp4067;}); break; case 354: _LL4066: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4070=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4070[ 0]=({ struct Cyc_Exp_tok_struct
_temp4071; _temp4071.tag= Cyc_Exp_tok; _temp4071.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4071;}); _temp4070;}); break; case 355: _LL4069: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4073=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4073[ 0]=({ struct Cyc_Exp_tok_struct
_temp4074; _temp4074.tag= Cyc_Exp_tok; _temp4074.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4074;}); _temp4073;}); break; case 356: _LL4072: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4076=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4076[ 0]=({ struct Cyc_Exp_tok_struct
_temp4077; _temp4077.tag= Cyc_Exp_tok; _temp4077.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4077;}); _temp4076;}); break; case 357: _LL4075: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 358: _LL4078: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4080=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4080[ 0]=({ struct Cyc_Exp_tok_struct
_temp4081; _temp4081.tag= Cyc_Exp_tok; _temp4081.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4081;}); _temp4080;}); break; case 359: _LL4079: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4083=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4083[ 0]=({ struct Cyc_Exp_tok_struct
_temp4084; _temp4084.tag= Cyc_Exp_tok; _temp4084.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4084;}); _temp4083;}); break; case 360: _LL4082:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4086=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4086[ 0]=({ struct Cyc_Exp_tok_struct
_temp4087; _temp4087.tag= Cyc_Exp_tok; _temp4087.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4087;}); _temp4086;}); break; case 361: _LL4085: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4089=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4089[ 0]=({ struct Cyc_Exp_tok_struct
_temp4090; _temp4090.tag= Cyc_Exp_tok; _temp4090.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4090;}); _temp4089;}); break; case 362: _LL4088: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4092=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4092[ 0]=({ struct Cyc_Exp_tok_struct
_temp4093; _temp4093.tag= Cyc_Exp_tok; _temp4093.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp4094=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp4094[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp4095; _temp4095.tag= Cyc_Absyn_Malloc_e;
_temp4095.f1= 0; _temp4095.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp4095;}); _temp4094;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4093;});
_temp4092;}); break; case 363: _LL4091: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4097=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4097[ 0]=({ struct Cyc_Exp_tok_struct _temp4098; _temp4098.tag= Cyc_Exp_tok;
_temp4098.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp4099=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp4099[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp4100; _temp4100.tag= Cyc_Absyn_Malloc_e;
_temp4100.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6)); _temp4100.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).last_line)); _temp4100;}); _temp4099;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4098;}); _temp4097;}); break; case 364: _LL4096: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4102=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4102[ 0]=({ struct Cyc_Primop_tok_struct
_temp4103; _temp4103.tag= Cyc_Primop_tok; _temp4103.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp4103;}); _temp4102;}); break; case 365: _LL4101: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4105=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4105[ 0]=({ struct Cyc_Primop_tok_struct
_temp4106; _temp4106.tag= Cyc_Primop_tok; _temp4106.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp4106;}); _temp4105;}); break; case 366: _LL4104: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4108=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4108[ 0]=({ struct Cyc_Primop_tok_struct
_temp4109; _temp4109.tag= Cyc_Primop_tok; _temp4109.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp4109;}); _temp4108;}); break; case 367: _LL4107: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4111=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4111[ 0]=({ struct Cyc_Primop_tok_struct
_temp4112; _temp4112.tag= Cyc_Primop_tok; _temp4112.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp4112;}); _temp4111;}); break; case 368: _LL4110: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4114=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4114[ 0]=({ struct Cyc_Primop_tok_struct
_temp4115; _temp4115.tag= Cyc_Primop_tok; _temp4115.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp4115;}); _temp4114;}); break; case 369: _LL4113: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4117=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4117[ 0]=({ struct Cyc_Primop_tok_struct
_temp4118; _temp4118.tag= Cyc_Primop_tok; _temp4118.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp4118;}); _temp4117;}); break; case 370: _LL4116: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4120=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4120[ 0]=({ struct Cyc_Primop_tok_struct
_temp4121; _temp4121.tag= Cyc_Primop_tok; _temp4121.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp4121;}); _temp4120;}); break; case 371: _LL4119: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4123=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4123[ 0]=({ struct Cyc_Primop_tok_struct
_temp4124; _temp4124.tag= Cyc_Primop_tok; _temp4124.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp4124;}); _temp4123;}); break; case 372: _LL4122: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4126=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4126[ 0]=({ struct Cyc_Primop_tok_struct
_temp4127; _temp4127.tag= Cyc_Primop_tok; _temp4127.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp4127;}); _temp4126;}); break; case 373: _LL4125: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 374: _LL4128: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4130=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4130[ 0]=({ struct Cyc_Exp_tok_struct
_temp4131; _temp4131.tag= Cyc_Exp_tok; _temp4131.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4131;}); _temp4130;}); break; case 375: _LL4129: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4133=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4133[ 0]=({ struct Cyc_Exp_tok_struct
_temp4134; _temp4134.tag= Cyc_Exp_tok; _temp4134.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4134;}); _temp4133;}); break; case 376: _LL4132: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4136=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4136[ 0]=({ struct Cyc_Exp_tok_struct
_temp4137; _temp4137.tag= Cyc_Exp_tok; _temp4137.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4137;}); _temp4136;}); break; case 377: _LL4135: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4139=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4139[ 0]=({ struct Cyc_Exp_tok_struct
_temp4140; _temp4140.tag= Cyc_Exp_tok; _temp4140.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4141=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4141[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4141;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4140;}); _temp4139;}); break; case 378: _LL4138: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4143=(* q).f1; struct Cyc_List_List* _temp4153; struct Cyc_List_List*
_temp4155; _LL4145: if( _temp4143 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4146;}
else{ goto _LL4147;} _LL4147: if(( unsigned int) _temp4143 > 1u?*(( int*)
_temp4143) == Cyc_Absyn_Rel_n: 0){ _LL4154: _temp4153=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4143)->f1; if( _temp4153 == 0){ goto _LL4148;} else{ goto _LL4149;}} else{
goto _LL4149;} _LL4149: if(( unsigned int) _temp4143 > 1u?*(( int*) _temp4143)
== Cyc_Absyn_Abs_n: 0){ _LL4156: _temp4155=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4143)->f1; if( _temp4155 == 0){ goto _LL4150;} else{ goto _LL4151;}} else{
goto _LL4151;} _LL4151: goto _LL4152; _LL4146: goto _LL4144; _LL4148: goto
_LL4144; _LL4150: goto _LL4144; _LL4152: Cyc_Parse_err(({ unsigned char*
_temp4157="struct field name is qualified"; struct _tagged_string _temp4158;
_temp4158.curr= _temp4157; _temp4158.base= _temp4157; _temp4158.last_plus_one=
_temp4157 + 31; _temp4158;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4144;
_LL4144:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4159=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4159[ 0]=({ struct Cyc_Exp_tok_struct
_temp4160; _temp4160.tag= Cyc_Exp_tok; _temp4160.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4160;}); _temp4159;}); break;} case 379:
_LL4142: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4162=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4162[ 0]=({ struct Cyc_Exp_tok_struct
_temp4163; _temp4163.tag= Cyc_Exp_tok; _temp4163.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4164=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4164[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4164;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4163;}); _temp4162;}); break; case 380: _LL4161: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4166=(* q).f1; struct Cyc_List_List* _temp4176; struct Cyc_List_List*
_temp4178; _LL4168: if( _temp4166 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4169;}
else{ goto _LL4170;} _LL4170: if(( unsigned int) _temp4166 > 1u?*(( int*)
_temp4166) == Cyc_Absyn_Rel_n: 0){ _LL4177: _temp4176=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4166)->f1; if( _temp4176 == 0){ goto _LL4171;} else{ goto _LL4172;}} else{
goto _LL4172;} _LL4172: if(( unsigned int) _temp4166 > 1u?*(( int*) _temp4166)
== Cyc_Absyn_Abs_n: 0){ _LL4179: _temp4178=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4166)->f1; if( _temp4178 == 0){ goto _LL4173;} else{ goto _LL4174;}} else{
goto _LL4174;} _LL4174: goto _LL4175; _LL4169: goto _LL4167; _LL4171: goto
_LL4167; _LL4173: goto _LL4167; _LL4175: Cyc_Parse_err(({ unsigned char*
_temp4180="struct field is qualified with module name"; struct _tagged_string
_temp4181; _temp4181.curr= _temp4180; _temp4181.base= _temp4180; _temp4181.last_plus_one=
_temp4180 + 43; _temp4181;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4167;
_LL4167:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4182=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4182[ 0]=({ struct Cyc_Exp_tok_struct
_temp4183; _temp4183.tag= Cyc_Exp_tok; _temp4183.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4183;}); _temp4182;}); break;} case 381:
_LL4165: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4185=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4185[ 0]=({ struct Cyc_Exp_tok_struct
_temp4186; _temp4186.tag= Cyc_Exp_tok; _temp4186.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4186;}); _temp4185;}); break; case 382: _LL4184: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4188=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4188[ 0]=({ struct Cyc_Exp_tok_struct
_temp4189; _temp4189.tag= Cyc_Exp_tok; _temp4189.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4189;}); _temp4188;}); break; case 383: _LL4187: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4191=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4191[ 0]=({ struct Cyc_Exp_tok_struct
_temp4192; _temp4192.tag= Cyc_Exp_tok; _temp4192.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4193=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4193[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4194; _temp4194.tag= Cyc_Absyn_CompoundLit_e;
_temp4194.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp4194.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp4194;}); _temp4193;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4192;}); _temp4191;}); break; case 384: _LL4190: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4196=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4196[ 0]=({ struct Cyc_Exp_tok_struct
_temp4197; _temp4197.tag= Cyc_Exp_tok; _temp4197.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4198=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4198[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4199; _temp4199.tag= Cyc_Absyn_CompoundLit_e;
_temp4199.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp4199.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp4199;}); _temp4198;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4197;}); _temp4196;}); break; case 385: _LL4195: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4201=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4201[ 0]=({ struct Cyc_Exp_tok_struct
_temp4202; _temp4202.tag= Cyc_Exp_tok; _temp4202.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp4203=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp4203[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp4204; _temp4204.tag= Cyc_Absyn_Fill_e; _temp4204.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4204;});
_temp4203;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4202;});
_temp4201;}); break; case 386: _LL4200: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4206=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4206[ 0]=({ struct Cyc_Exp_tok_struct _temp4207; _temp4207.tag= Cyc_Exp_tok;
_temp4207.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp4208=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp4208[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp4209; _temp4209.tag= Cyc_Absyn_Codegen_e;
_temp4209.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp4209;}); _temp4208;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4207;}); _temp4206;}); break; case 387: _LL4205: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4211=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4211[ 0]=({ struct Cyc_Exp_tok_struct
_temp4212; _temp4212.tag= Cyc_Exp_tok; _temp4212.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp4213=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp4213[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp4214; _temp4214.tag= Cyc_Absyn_UnknownId_e;
_temp4214.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4214;}); _temp4213;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4212;}); _temp4211;}); break; case 388: _LL4210: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 389: _LL4215: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4217=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4217[ 0]=({ struct Cyc_Exp_tok_struct
_temp4218; _temp4218.tag= Cyc_Exp_tok; _temp4218.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4218;}); _temp4217;}); break; case 390: _LL4216: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1); break; case 391: _LL4219: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4221=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4221[ 0]=({ struct Cyc_Exp_tok_struct
_temp4222; _temp4222.tag= Cyc_Exp_tok; _temp4222.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4223=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4223[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4224; _temp4224.tag=
Cyc_Absyn_UnknownId_e; _temp4224.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp4224;}); _temp4223;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4222;}); _temp4221;}); break; case 392: _LL4220:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4226=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4226[ 0]=({ struct Cyc_Exp_tok_struct
_temp4227; _temp4227.tag= Cyc_Exp_tok; _temp4227.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4228=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4228[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4229; _temp4229.tag=
Cyc_Absyn_UnknownId_e; _temp4229.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); _temp4229;}); _temp4228;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4227;}); _temp4226;}); break; case 393: _LL4225: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4231=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4231[ 0]=({ struct Cyc_Exp_tok_struct
_temp4232; _temp4232.tag= Cyc_Exp_tok; _temp4232.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4232;}); _temp4231;}); break; case 394: _LL4230: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4234=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4234[ 0]=({ struct Cyc_Exp_tok_struct
_temp4235; _temp4235.tag= Cyc_Exp_tok; _temp4235.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp4236=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp4236[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp4237; _temp4237.tag= Cyc_Absyn_Struct_e;
_temp4237.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp4237.f2= 0; _temp4237.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp4237.f4=
0; _temp4237;}); _temp4236;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4235;});
_temp4234;}); break; case 395: _LL4233: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4239=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4239[ 0]=({ struct Cyc_Exp_tok_struct _temp4240; _temp4240.tag= Cyc_Exp_tok;
_temp4240.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4240;});
_temp4239;}); break; case 396: _LL4238: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4242=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4242[ 0]=({ struct Cyc_ExpList_tok_struct _temp4243; _temp4243.tag= Cyc_ExpList_tok;
_temp4243.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp4243;}); _temp4242;}); break; case 397: _LL4241: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp4245=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp4245[ 0]=({ struct Cyc_ExpList_tok_struct
_temp4246; _temp4246.tag= Cyc_ExpList_tok; _temp4246.f1=({ struct Cyc_List_List*
_temp4247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4247->hd=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4247->tl= 0; _temp4247;}); _temp4246;}); _temp4245;});
break; case 398: _LL4244: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4249=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4249[ 0]=({ struct Cyc_ExpList_tok_struct _temp4250; _temp4250.tag= Cyc_ExpList_tok;
_temp4250.f1=({ struct Cyc_List_List* _temp4251=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4251->hd=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4251->tl=
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp4251;}); _temp4250;}); _temp4249;}); break; case 399: _LL4248: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4253=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4253[ 0]=({ struct Cyc_Exp_tok_struct
_temp4254; _temp4254.tag= Cyc_Exp_tok; _temp4254.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1,(* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4254;}); _temp4253;}); break; case 400: _LL4252: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4256=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4256[ 0]=({ struct Cyc_Exp_tok_struct
_temp4257; _temp4257.tag= Cyc_Exp_tok; _temp4257.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4257;}); _temp4256;}); break; case 401: _LL4255: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4259=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4259[ 0]=({ struct Cyc_Exp_tok_struct
_temp4260; _temp4260.tag= Cyc_Exp_tok; _temp4260.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4260;}); _temp4259;}); break; case 402: _LL4258: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4262=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4262[ 0]=({ struct Cyc_Exp_tok_struct
_temp4263; _temp4263.tag= Cyc_Exp_tok; _temp4263.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4263;}); _temp4262;}); break; case 403: _LL4261: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp4265=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp4265[ 0]=({ struct Cyc_QualId_tok_struct
_temp4266; _temp4266.tag= Cyc_QualId_tok; _temp4266.f1=({ struct _tuple1*
_temp4267=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4267->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp4269=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp4269[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp4270; _temp4270.tag= Cyc_Absyn_Rel_n; _temp4270.f1= 0; _temp4270;});
_temp4269;}); _temp4267->f2=({ struct _tagged_string* _temp4268=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp4268[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4268;});
_temp4267;}); _temp4266;}); _temp4265;}); break; case 404: _LL4264: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; default:
_LL4271:( void) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen;
yyssp_offset -= yylen; yylsp_offset -= yylen;*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), ++
yyvsp_offset)= yyval; yylsp_offset ++; if( yylen == 0){(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line= Cyc_yylloc.first_line;(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_column=
Cyc_yylloc.first_column;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_column=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_column;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).text=({ unsigned char* _temp4273=""; struct _tagged_string
_temp4274; _temp4274.curr= _temp4273; _temp4274.base= _temp4273; _temp4274.last_plus_one=
_temp4273 + 1; _temp4274;});} else{(*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line=(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset + yylen) - 1)).last_line;(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_column=(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset +
yylen) - 1)).last_column;} yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull(
405u, yyn)]; yystate= Cyc_yypgoto[ _check_known_subscript_notnull( 111u, yyn -
124)] +*(( short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset); if(( yystate >=
0? yystate <= 4414: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4415u,
yystate)] ==*(( short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset): 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4415u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 111u, yyn - 124)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 782u, yystate)]; if( yyn > - 32768? yyn < 4414:
0){ int sze= 0; struct _tagged_string msg; int x; int count; count= 0; for( x=
yyn < 0? - yyn: 0; x < 235u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4415u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 235u, x)]) + 15, count ++);}} msg=({
unsigned int _temp4275=( unsigned int)( sze + 15); unsigned char* _temp4276=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp4275); struct
_tagged_string _temp4278={ _temp4276, _temp4276, _temp4276 + _temp4275};{
unsigned int _temp4277= _temp4275; unsigned int i; for( i= 0; i < _temp4277; i
++){ _temp4276[ i]='\000';}}; _temp4278;}); Cyc_String_strcpy( msg,({
unsigned char* _temp4279="parse error"; struct _tagged_string _temp4280;
_temp4280.curr= _temp4279; _temp4280.base= _temp4279; _temp4280.last_plus_one=
_temp4279 + 12; _temp4280;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 235u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4415u, x + yyn)] == x){ Cyc_String_strcat( msg,
count == 0?({ unsigned char* _temp4281=", expecting `"; struct _tagged_string
_temp4282; _temp4282.curr= _temp4281; _temp4282.base= _temp4281; _temp4282.last_plus_one=
_temp4281 + 14; _temp4282;}):({ unsigned char* _temp4283=" or `"; struct
_tagged_string _temp4284; _temp4284.curr= _temp4283; _temp4284.base= _temp4283;
_temp4284.last_plus_one= _temp4283 + 6; _temp4284;})); Cyc_String_strcat( msg,
Cyc_yytname[ _check_known_subscript_notnull( 235u, x)]); Cyc_String_strcat( msg,({
unsigned char* _temp4285="'"; struct _tagged_string _temp4286; _temp4286.curr=
_temp4285; _temp4286.base= _temp4285; _temp4286.last_plus_one= _temp4285 + 2;
_temp4286;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(({
unsigned char* _temp4287="parse error"; struct _tagged_string _temp4288;
_temp4288.curr= _temp4287; _temp4288.base= _temp4287; _temp4288.last_plus_one=
_temp4287 + 12; _temp4288;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), -- yyssp_offset);
yylsp_offset --; yyerrhandle: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 782u, yystate)]; if( yyn == - 32768){ goto
yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4414)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4415u, yyn)] != 1){ goto yyerrdefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4415u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrpop;}} if( yyn == 781){ return 0;}*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; goto yynewstate;}
void Cyc_yyprint( int i, void* v){ void* _temp4304= v; struct _tuple16*
_temp4324; struct _tuple16 _temp4326; int _temp4327; void* _temp4329;
unsigned char _temp4331; short _temp4333; struct _tagged_string _temp4335;
struct Cyc_Core_Opt* _temp4337; struct Cyc_Core_Opt* _temp4339; struct Cyc_Core_Opt
_temp4341; struct _tagged_string* _temp4342; struct _tuple1* _temp4344; struct
_tuple1 _temp4346; struct _tagged_string* _temp4347; void* _temp4349; _LL4306:
if( _temp4304 == Cyc_Okay_tok){ goto _LL4307;} else{ goto _LL4308;} _LL4308: if(*((
void**) _temp4304) == Cyc_Int_tok){ _LL4325: _temp4324=(( struct Cyc_Int_tok_struct*)
_temp4304)->f1; _temp4326=* _temp4324; _LL4330: _temp4329= _temp4326.f1; goto
_LL4328; _LL4328: _temp4327= _temp4326.f2; goto _LL4309;} else{ goto _LL4310;}
_LL4310: if(*(( void**) _temp4304) == Cyc_Char_tok){ _LL4332: _temp4331=((
struct Cyc_Char_tok_struct*) _temp4304)->f1; goto _LL4311;} else{ goto _LL4312;}
_LL4312: if(*(( void**) _temp4304) == Cyc_Short_tok){ _LL4334: _temp4333=((
struct Cyc_Short_tok_struct*) _temp4304)->f1; goto _LL4313;} else{ goto _LL4314;}
_LL4314: if(*(( void**) _temp4304) == Cyc_String_tok){ _LL4336: _temp4335=((
struct Cyc_String_tok_struct*) _temp4304)->f1; goto _LL4315;} else{ goto _LL4316;}
_LL4316: if(*(( void**) _temp4304) == Cyc_Stringopt_tok){ _LL4338: _temp4337=((
struct Cyc_Stringopt_tok_struct*) _temp4304)->f1; if( _temp4337 == 0){ goto
_LL4317;} else{ goto _LL4318;}} else{ goto _LL4318;} _LL4318: if(*(( void**)
_temp4304) == Cyc_Stringopt_tok){ _LL4340: _temp4339=(( struct Cyc_Stringopt_tok_struct*)
_temp4304)->f1; if( _temp4339 == 0){ goto _LL4320;} else{ _temp4341=* _temp4339;
_LL4343: _temp4342=( struct _tagged_string*) _temp4341.v; goto _LL4319;}} else{
goto _LL4320;} _LL4320: if(*(( void**) _temp4304) == Cyc_QualId_tok){ _LL4345:
_temp4344=(( struct Cyc_QualId_tok_struct*) _temp4304)->f1; _temp4346=*
_temp4344; _LL4350: _temp4349= _temp4346.f1; goto _LL4348; _LL4348: _temp4347=
_temp4346.f2; goto _LL4321;} else{ goto _LL4322;} _LL4322: goto _LL4323; _LL4307:
fprintf( sfile_to_file( Cyc_Stdio_stderr),"ok"); goto _LL4305; _LL4309: fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%d", _temp4327); goto _LL4305; _LL4311:
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%c",( int) _temp4331); goto _LL4305;
_LL4313: fprintf( sfile_to_file( Cyc_Stdio_stderr),"%ds",( int) _temp4333); goto
_LL4305; _LL4315:({ struct _tagged_string _temp4351= _temp4335; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _temp4351.last_plus_one - _temp4351.curr,
_temp4351.curr);}); goto _LL4305; _LL4317: fprintf( sfile_to_file( Cyc_Stdio_stderr),"null");
goto _LL4305; _LL4319:({ struct _tagged_string _temp4352=* _temp4342; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _temp4352.last_plus_one - _temp4352.curr,
_temp4352.curr);}); goto _LL4305; _LL4321: { struct Cyc_List_List* _temp4353= 0;
goto _LL4354; _LL4354:{ void* _temp4355= _temp4349; struct Cyc_List_List*
_temp4363; struct Cyc_List_List* _temp4365; _LL4357: if(( unsigned int)
_temp4355 > 1u?*(( int*) _temp4355) == Cyc_Absyn_Rel_n: 0){ _LL4364: _temp4363=((
struct Cyc_Absyn_Rel_n_struct*) _temp4355)->f1; goto _LL4358;} else{ goto
_LL4359;} _LL4359: if(( unsigned int) _temp4355 > 1u?*(( int*) _temp4355) == Cyc_Absyn_Abs_n:
0){ _LL4366: _temp4365=(( struct Cyc_Absyn_Abs_n_struct*) _temp4355)->f1; goto
_LL4360;} else{ goto _LL4361;} _LL4361: if( _temp4355 ==( void*) Cyc_Absyn_Loc_n){
goto _LL4362;} else{ goto _LL4356;} _LL4358: _temp4353= _temp4363; goto _LL4356;
_LL4360: _temp4353= _temp4365; goto _LL4356; _LL4362: goto _LL4356; _LL4356:;}
for( 0; _temp4353 != 0; _temp4353=(( struct Cyc_List_List*) _check_null(
_temp4353))->tl){({ struct _tagged_string _temp4367=*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( _temp4353))->hd); fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s::", _temp4367.last_plus_one - _temp4367.curr, _temp4367.curr);});}({
struct _tagged_string _temp4368=* _temp4347; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_temp4368.last_plus_one - _temp4368.curr, _temp4368.curr);}); goto _LL4305;}
_LL4323: fprintf( sfile_to_file( Cyc_Stdio_stderr),"?"); goto _LL4305; _LL4305:;}
struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result=
0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp4369=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4369->v=( void*) Cyc_Lexing_from_file(
f); _temp4369;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}