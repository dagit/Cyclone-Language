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
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_string
f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_string, int, void*); void* read_fun_state; } ;
struct _tagged_ptr1{ int* curr; int* base; int* last_plus_one; } ; struct Cyc_Lexing_lex_tables{
struct _tagged_ptr1 lex_base; struct _tagged_ptr1 lex_backtrk; struct
_tagged_ptr1 lex_default; struct _tagged_ptr1 lex_trans; struct _tagged_ptr1
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
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
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
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp(
void*, struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct
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
struct Cyc_List_List* rgn_po, struct Cyc_List_List*); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct
Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char
Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
Cyc_Lex_enter_namespace( struct _tagged_string*); extern void Cyc_Lex_leave_namespace();
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
struct _tuple4{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment*
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
struct _tagged_string _temp49=( struct _tagged_string) Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp50= msg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: %.*s\n",
_temp49.last_plus_one - _temp49.curr, _temp49.curr, _temp50.last_plus_one -
_temp50.curr, _temp50.curr);}); return;} static void* Cyc_Parse_unimp( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ struct _tagged_string _temp51= msg; xprintf("%.*s unimplemented",
_temp51.last_plus_one - _temp51.curr, _temp51.curr);}), sg);} static void Cyc_Parse_unimp2(
struct _tagged_string msg, struct Cyc_Position_Segment* sg){({ struct
_tagged_string _temp52=( struct _tagged_string) Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp53= msg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp52.last_plus_one - _temp52.curr, _temp52.curr, _temp53.last_plus_one -
_temp53.curr, _temp53.curr);}); return;} struct _tuple9{ struct _tuple7* f1;
struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct Cyc_Core_Opt*
_temp56; struct _tuple7* _temp58; struct _tuple9 _temp54=* field_info; _LL59:
_temp58= _temp54.f1; goto _LL57; _LL57: _temp56= _temp54.f2; goto _LL55; _LL55:
if((* _temp58).f4 != 0){ Cyc_Parse_err(({ unsigned char* _temp60=( unsigned char*)"bad type params in struct field";
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
unsigned char* _temp78=( unsigned char*)"struct field cannot be qualified with a module name";
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
Cyc_Parse_err(({ unsigned char* _temp111=( unsigned char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 49; _temp112;}), loc); goto _LL98; _LL98:;}}
if( _temp86 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ unsigned char* _temp113=( unsigned char*)"parameter should have no type parameters";
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
unsigned char* _temp195=( unsigned char*)"initializers are not allowed in parameter declarations";
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
_LL206: Cyc_Parse_err(({ unsigned char* _temp211=( unsigned char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 51; _temp212;}), x->loc); goto _LL198; _LL198:;}{
int found= 0; for( 0; params != 0; params=(( struct Cyc_List_List*) _check_null(
params))->tl){ if( Cyc_String_zstrptrcmp((* _temp159->name).f2,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( params))->hd) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ struct
_tagged_string _temp213=*(* _temp159->name).f2; xprintf("%.*s is not listed as a parameter",
_temp213.last_plus_one - _temp213.curr, _temp213.curr);}), x->loc);} return;}
_LL138: decl_kind=({ unsigned char* _temp214=( unsigned char*)"let declaration";
struct _tagged_string _temp215; _temp215.curr= _temp214; _temp215.base= _temp214;
_temp215.last_plus_one= _temp214 + 16; _temp215;}); goto _LL134; _LL140:
decl_kind=({ unsigned char* _temp216=( unsigned char*)"let declaration"; struct
_tagged_string _temp217; _temp217.curr= _temp216; _temp217.base= _temp216;
_temp217.last_plus_one= _temp216 + 16; _temp217;}); goto _LL134; _LL142:
decl_kind=({ unsigned char* _temp218=( unsigned char*)"function declaration";
struct _tagged_string _temp219; _temp219.curr= _temp218; _temp219.base= _temp218;
_temp219.last_plus_one= _temp218 + 21; _temp219;}); goto _LL134; _LL144:
decl_kind=({ unsigned char* _temp220=( unsigned char*)"struct declaration";
struct _tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 19; _temp221;}); goto _LL134; _LL146:
decl_kind=({ unsigned char* _temp222=( unsigned char*)"union declaration";
struct _tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 18; _temp223;}); goto _LL134; _LL148:
decl_kind=({ unsigned char* _temp224=( unsigned char*)"tunion declaration";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 19; _temp225;}); goto _LL134; _LL150:
decl_kind=({ unsigned char* _temp226=( unsigned char*)"typedef"; struct
_tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 8; _temp227;}); goto _LL134; _LL152:
decl_kind=({ unsigned char* _temp228=( unsigned char*)"enum declaration"; struct
_tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 17; _temp229;}); goto _LL134; _LL154:
decl_kind=({ unsigned char* _temp230=( unsigned char*)"namespace declaration";
struct _tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 22; _temp231;}); goto _LL134; _LL156:
decl_kind=({ unsigned char* _temp232=( unsigned char*)"using declaration";
struct _tagged_string _temp233; _temp233.curr= _temp232; _temp233.base= _temp232;
_temp233.last_plus_one= _temp232 + 18; _temp233;}); goto _LL134; _LL158:
decl_kind=({ unsigned char* _temp234=( unsigned char*)"extern C declaration";
struct _tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 21; _temp235;}); goto _LL134; _LL134:;}((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ struct _tagged_string _temp236= decl_kind; xprintf("%.*s appears in parameter type",
_temp236.last_plus_one - _temp236.curr, _temp236.curr);}), x->loc); return;}
struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_string* x){ struct _tuple12 _temp239; struct Cyc_Position_Segment*
_temp240; struct Cyc_List_List* _temp242; struct _tuple12* _temp237= env;
_temp239=* _temp237; _LL243: _temp242= _temp239.f1; goto _LL241; _LL241:
_temp240= _temp239.f2; goto _LL238; _LL238: if( _temp242 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ struct _tagged_string _temp244=* x; xprintf("missing type for parameter %.*s",
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
_LL262: Cyc_Parse_err(({ unsigned char* _temp267=( unsigned char*)"module name not allowed on parameter";
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
unsigned char* _temp274=( unsigned char*)"non-variable declaration"; struct
_tagged_string _temp275; _temp275.curr= _temp274; _temp275.base= _temp274;
_temp275.last_plus_one= _temp274 + 25; _temp275;}),(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp242))->hd)->loc); _LL246:;}} static int
Cyc_Parse_is_typeparam( void* tm){ void* _temp276= tm; int _temp282; struct Cyc_Position_Segment*
_temp284; struct Cyc_List_List* _temp286; _LL278: if(( unsigned int) _temp276 >
1u?*(( int*) _temp276) == Cyc_Absyn_TypeParams_mod: 0){ _LL287: _temp286=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f1; goto _LL285; _LL285:
_temp284=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f2; goto _LL283;
_LL283: _temp282=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f3; goto
_LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279: return 1; _LL281:
return 0; _LL277:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,({ unsigned char*
_temp288=( unsigned char*)"`H"; struct _tagged_string _temp289; _temp289.curr=
_temp288; _temp289.base= _temp288; _temp289.last_plus_one= _temp288 + 3;
_temp289;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({
struct Cyc_Absyn_VarType_struct* _temp290=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp290[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp291; _temp291.tag= Cyc_Absyn_VarType; _temp291.f1=({ struct Cyc_Absyn_Tvar*
_temp292=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp292->name=({ struct _tagged_string* _temp293=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp293[ 0]= s; _temp293;});
_temp292->identity= 0; _temp292->kind= k; _temp292;}); _temp291;}); _temp290;});}}
static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar( struct Cyc_Position_Segment*
loc, void* t){ void* _temp294= t; struct Cyc_Absyn_Tvar* _temp300; _LL296: if((
unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_VarType: 0){ _LL301:
_temp300=(( struct Cyc_Absyn_VarType_struct*) _temp294)->f1; goto _LL297;} else{
goto _LL298;} _LL298: goto _LL299; _LL297: return _temp300; _LL299: return((
struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ unsigned char* _temp302=( unsigned char*)"expecting a list of type variables, not types";
struct _tagged_string _temp303; _temp303.curr= _temp302; _temp303.base= _temp302;
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
== 0: 0)){ void* _temp314= _temp312; struct Cyc_List_List* _temp320; struct Cyc_Core_Opt*
_temp322; struct Cyc_Absyn_VarargInfo* _temp324; int _temp326; struct Cyc_List_List*
_temp328; struct Cyc_Position_Segment* _temp330; struct Cyc_List_List* _temp332;
_LL316: if(*(( int*) _temp314) == Cyc_Absyn_WithTypes){ _LL329: _temp328=((
struct Cyc_Absyn_WithTypes_struct*) _temp314)->f1; goto _LL327; _LL327: _temp326=((
struct Cyc_Absyn_WithTypes_struct*) _temp314)->f2; goto _LL325; _LL325: _temp324=((
struct Cyc_Absyn_WithTypes_struct*) _temp314)->f3; goto _LL323; _LL323: _temp322=((
struct Cyc_Absyn_WithTypes_struct*) _temp314)->f4; goto _LL321; _LL321: _temp320=((
struct Cyc_Absyn_WithTypes_struct*) _temp314)->f5; goto _LL317;} else{ goto
_LL318;} _LL318: if(*(( int*) _temp314) == Cyc_Absyn_NoTypes){ _LL333: _temp332=((
struct Cyc_Absyn_NoTypes_struct*) _temp314)->f1; goto _LL331; _LL331: _temp330=((
struct Cyc_Absyn_NoTypes_struct*) _temp314)->f2; goto _LL319;} else{ goto _LL315;}
_LL317: Cyc_Parse_warn(({ unsigned char* _temp334=( unsigned char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 92; _temp335;}), loc); return tms; _LL319:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp332, tds);{ struct _tuple12* _temp337=({ struct _tuple12* _temp336=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp336->f1= tds; _temp336->f2=
loc; _temp336;}); goto _LL338; _LL338: return({ struct Cyc_List_List* _temp339=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp339->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp340=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp340[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp341; _temp341.tag= Cyc_Absyn_Function_mod;
_temp341.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp342=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp342[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp343; _temp343.tag= Cyc_Absyn_WithTypes;
_temp343.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_string*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp337, _temp332); _temp343.f2= 0; _temp343.f3= 0;
_temp343.f4= 0; _temp343.f5= 0; _temp343;}); _temp342;})); _temp341;}); _temp340;}));
_temp339->tl= 0; _temp339;});} _LL315:;} else{ return({ struct Cyc_List_List*
_temp344=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp344->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp344->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp344;});} _LL311: return({ struct Cyc_List_List*
_temp345=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp345->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp345->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp345;}); _LL307:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp346=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp346->id= d->id; _temp346->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp346;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp347=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL349: if( _temp347 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL350;} else{ goto _LL351;} _LL351: if(
_temp347 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL352;} else{ goto _LL353;}
_LL353: if( _temp347 ==( void*) Cyc_Parse_Static_sc){ goto _LL354;} else{ goto
_LL355;} _LL355: goto _LL356; _LL350: sc=( void*) Cyc_Absyn_Extern; goto _LL348;
_LL352: sc=( void*) Cyc_Absyn_ExternC; goto _LL348; _LL354: sc=( void*) Cyc_Absyn_Static;
goto _LL348; _LL356: Cyc_Parse_err(({ unsigned char* _temp357=( unsigned char*)"bad storage class on function";
struct _tagged_string _temp358; _temp358.curr= _temp357; _temp358.base= _temp357;
_temp358.last_plus_one= _temp357 + 30; _temp358;}), loc); goto _LL348; _LL348:;}}{
struct Cyc_Core_Opt* _temp361; void* _temp363; struct _tuple5 _temp359= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL364: _temp363= _temp359.f1; goto _LL362; _LL362: _temp361=
_temp359.f2; goto _LL360; _LL360: { struct Cyc_List_List* _temp367; struct Cyc_List_List*
_temp369; void* _temp371; struct Cyc_Absyn_Tqual _temp373; struct _tuple6
_temp365= Cyc_Parse_apply_tms( tq, _temp363, atts, d->tms); _LL374: _temp373=
_temp365.f1; goto _LL372; _LL372: _temp371= _temp365.f2; goto _LL370; _LL370:
_temp369= _temp365.f3; goto _LL368; _LL368: _temp367= _temp365.f4; goto _LL366;
_LL366: if( _temp361 != 0){ Cyc_Parse_warn(({ unsigned char* _temp375=(
unsigned char*)"nested type declaration within function prototype"; struct
_tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 50; _temp376;}), loc);} if( _temp369 != 0){
Cyc_Parse_warn(({ unsigned char* _temp377=( unsigned char*)"bad type params, ignoring";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 26; _temp378;}), loc);}{ void* _temp379=
_temp371; struct Cyc_Absyn_FnInfo _temp385; struct Cyc_List_List* _temp387;
struct Cyc_List_List* _temp389; struct Cyc_Absyn_VarargInfo* _temp391; int
_temp393; struct Cyc_List_List* _temp395; void* _temp397; struct Cyc_Core_Opt*
_temp399; struct Cyc_List_List* _temp401; _LL381: if(( unsigned int) _temp379 >
4u?*(( int*) _temp379) == Cyc_Absyn_FnType: 0){ _LL386: _temp385=(( struct Cyc_Absyn_FnType_struct*)
_temp379)->f1; _LL402: _temp401= _temp385.tvars; goto _LL400; _LL400: _temp399=
_temp385.effect; goto _LL398; _LL398: _temp397=( void*) _temp385.ret_typ; goto
_LL396; _LL396: _temp395= _temp385.args; goto _LL394; _LL394: _temp393= _temp385.c_varargs;
goto _LL392; _LL392: _temp391= _temp385.cyc_varargs; goto _LL390; _LL390:
_temp389= _temp385.rgn_po; goto _LL388; _LL388: _temp387= _temp385.attributes;
goto _LL382;} else{ goto _LL383;} _LL383: goto _LL384; _LL382: { struct Cyc_List_List*
_temp403=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp395); goto _LL404; _LL404:
return({ struct Cyc_Absyn_Fndecl* _temp405=( struct Cyc_Absyn_Fndecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Fndecl)); _temp405->sc=( void*) sc; _temp405->name= d->id;
_temp405->tvs= _temp401; _temp405->is_inline= is_inline; _temp405->effect=
_temp399; _temp405->ret_type=( void*) _temp397; _temp405->args= _temp403;
_temp405->c_varargs= _temp393; _temp405->cyc_varargs= _temp391; _temp405->rgn_po=
_temp389; _temp405->body= body; _temp405->cached_typ= 0; _temp405->param_vardecls=
0; _temp405->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp387, _temp367); _temp405;});}
_LL384: return(( struct Cyc_Absyn_Fndecl*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp406=(
unsigned char*)"declarator is not a function prototype"; struct _tagged_string
_temp407; _temp407.curr= _temp406; _temp407.base= _temp406; _temp407.last_plus_one=
_temp406 + 39; _temp407;}), loc); _LL380:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(({
unsigned char* _temp408=( unsigned char*)"missing argument variable in function prototype";
struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 48; _temp409;}), loc); return({ struct
_tuple4* _temp410=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp410->f1=({ struct _tagged_string* _temp411=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp411[ 0]=({ unsigned char*
_temp412=( unsigned char*)"?"; struct _tagged_string _temp413; _temp413.curr=
_temp412; _temp413.base= _temp412; _temp413.last_plus_one= _temp412 + 2;
_temp413;}); _temp411;}); _temp410->f2=(* t).f2; _temp410->f3=(* t).f3; _temp410;});}
else{ return({ struct _tuple4* _temp414=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp414->f1=( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v; _temp414->f2=(* t).f2; _temp414->f3=(* t).f3;
_temp414;});}} static unsigned char _temp417[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1={ _temp417, _temp417, _temp417 + 52u};
static unsigned char _temp420[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2={ _temp420, _temp420, _temp420 + 63u};
static unsigned char _temp423[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3={ _temp423, _temp423, _temp423 + 50u};
static unsigned char _temp426[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4={ _temp426, _temp426, _temp426 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp427=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp441; void* _temp443; struct Cyc_Position_Segment*
_temp445; struct Cyc_Position_Segment* _temp447; struct Cyc_Position_Segment*
_temp449; struct Cyc_Position_Segment* _temp451; struct Cyc_Absyn_Decl* _temp453;
_LL429: if(*(( int*) _temp427) == Cyc_Parse_Type_spec){ _LL444: _temp443=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp427)->f1; goto _LL442; _LL442: _temp441=((
struct Cyc_Parse_Type_spec_struct*) _temp427)->f2; goto _LL430;} else{ goto
_LL431;} _LL431: if(*(( int*) _temp427) == Cyc_Parse_Signed_spec){ _LL446:
_temp445=(( struct Cyc_Parse_Signed_spec_struct*) _temp427)->f1; goto _LL432;}
else{ goto _LL433;} _LL433: if(*(( int*) _temp427) == Cyc_Parse_Unsigned_spec){
_LL448: _temp447=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp427)->f1; goto
_LL434;} else{ goto _LL435;} _LL435: if(*(( int*) _temp427) == Cyc_Parse_Short_spec){
_LL450: _temp449=(( struct Cyc_Parse_Short_spec_struct*) _temp427)->f1; goto
_LL436;} else{ goto _LL437;} _LL437: if(*(( int*) _temp427) == Cyc_Parse_Long_spec){
_LL452: _temp451=(( struct Cyc_Parse_Long_spec_struct*) _temp427)->f1; goto
_LL438;} else{ goto _LL439;} _LL439: if(*(( int*) _temp427) == Cyc_Parse_Decl_spec){
_LL454: _temp453=(( struct Cyc_Parse_Decl_spec_struct*) _temp427)->f1; goto
_LL440;} else{ goto _LL428;} _LL430: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp441);} last_loc= _temp441; seen_type= 1; t= _temp443; goto _LL428; _LL432:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp445);} if( seen_type){ Cyc_Parse_err(({
unsigned char* _temp455=( unsigned char*)"signed qualifier must come before type";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 39; _temp456;}), _temp445);} last_loc=
_temp445; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL428; _LL434: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp447);} if( seen_type){ Cyc_Parse_err(({
unsigned char* _temp457=( unsigned char*)"signed qualifier must come before type";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 39; _temp458;}), _temp447);} last_loc=
_temp447; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL428; _LL436:
if( seen_size){ Cyc_Parse_err(({ unsigned char* _temp459=( unsigned char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 59; _temp460;}), _temp449);} if( seen_type){
Cyc_Parse_err(({ unsigned char* _temp461=( unsigned char*)"short modifier must come before base type";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 42; _temp462;}), _temp449);} last_loc=
_temp449; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL428; _LL438: if(
seen_type){ Cyc_Parse_err(({ unsigned char* _temp463=( unsigned char*)"long modifier must come before base type";
struct _tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 41; _temp464;}), _temp451);} if( seen_size){
void* _temp465= sz; _LL467: if( _temp465 ==( void*) Cyc_Absyn_B4){ goto _LL468;}
else{ goto _LL469;} _LL469: goto _LL470; _LL468: sz=( void*) Cyc_Absyn_B8; goto
_LL466; _LL470: Cyc_Parse_err(({ unsigned char* _temp471=( unsigned char*)"extra long in type specifier";
struct _tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 29; _temp472;}), _temp451); goto _LL466;
_LL466:;} last_loc= _temp451; seen_size= 1; goto _LL428; _LL440: last_loc=
_temp453->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp473=(
void*) _temp453->r; struct Cyc_Absyn_Structdecl* _temp485; struct Cyc_Absyn_Tuniondecl*
_temp487; struct Cyc_Absyn_Uniondecl* _temp489; struct Cyc_Absyn_Enumdecl*
_temp491; _LL475: if(*(( int*) _temp473) == Cyc_Absyn_Struct_d){ _LL486:
_temp485=(( struct Cyc_Absyn_Struct_d_struct*) _temp473)->f1; goto _LL476;}
else{ goto _LL477;} _LL477: if(*(( int*) _temp473) == Cyc_Absyn_Tunion_d){
_LL488: _temp487=(( struct Cyc_Absyn_Tunion_d_struct*) _temp473)->f1; goto
_LL478;} else{ goto _LL479;} _LL479: if(*(( int*) _temp473) == Cyc_Absyn_Union_d){
_LL490: _temp489=(( struct Cyc_Absyn_Union_d_struct*) _temp473)->f1; goto _LL480;}
else{ goto _LL481;} _LL481: if(*(( int*) _temp473) == Cyc_Absyn_Enum_d){ _LL492:
_temp491=(( struct Cyc_Absyn_Enum_d_struct*) _temp473)->f1; goto _LL482;} else{
goto _LL483;} _LL483: goto _LL484; _LL476: { struct Cyc_List_List* _temp493=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp485->tvs); goto _LL494; _LL494: t=(
void*)({ struct Cyc_Absyn_StructType_struct* _temp495=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp495[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp496; _temp496.tag= Cyc_Absyn_StructType;
_temp496.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp485->name))->v); _temp496.f2= _temp493; _temp496.f3= 0;
_temp496;}); _temp495;}); if( _temp485->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp497=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp497->v=( void*) _temp453; _temp497;});} goto _LL474;} _LL478: { struct Cyc_List_List*
_temp498=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp487->tvs);
goto _LL499; _LL499: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp500=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp500[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp501; _temp501.tag= Cyc_Absyn_TunionType;
_temp501.f1=({ struct Cyc_Absyn_TunionInfo _temp502; _temp502.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp503=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp503[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp504; _temp504.tag= Cyc_Absyn_KnownTunion;
_temp504.f1= _temp487; _temp504;}); _temp503;})); _temp502.targs= _temp498;
_temp502.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp502;}); _temp501;});
_temp500;}); if( _temp487->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp505=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp505->v=( void*) _temp453; _temp505;});} goto _LL474;} _LL480: { struct Cyc_List_List*
_temp506=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp489->tvs);
goto _LL507; _LL507: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp508=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp508[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp509; _temp509.tag= Cyc_Absyn_UnionType;
_temp509.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp489->name))->v); _temp509.f2= _temp506; _temp509.f3= 0;
_temp509;}); _temp508;}); if( _temp489->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp510=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp510->v=( void*) _temp453; _temp510;});} goto _LL474;} _LL482: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp511=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp511[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp512; _temp512.tag= Cyc_Absyn_EnumType; _temp512.f1=
_temp491->name; _temp512.f2= 0; _temp512;}); _temp511;}); declopt=({ struct Cyc_Core_Opt*
_temp513=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp513->v=( void*) _temp453; _temp513;}); goto _LL474; _LL484:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp514=( unsigned char*)"bad declaration within type specifier";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 38; _temp515;}), _temp453->loc); goto _LL474;
_LL474:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp453->loc);} goto _LL428;
_LL428:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err(({
unsigned char* _temp516=( unsigned char*)"missing type withing specifier";
struct _tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 31; _temp517;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp518=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp518[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp519; _temp519.tag= Cyc_Absyn_IntType; _temp519.f1=( void*) sgn; _temp519.f2=(
void*) sz; _temp519;}); _temp518;});} else{ if( seen_sign){ void* _temp520= t;
void* _temp526; void* _temp528; _LL522: if(( unsigned int) _temp520 > 4u?*(( int*)
_temp520) == Cyc_Absyn_IntType: 0){ _LL529: _temp528=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp520)->f1; goto _LL527; _LL527: _temp526=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp520)->f2; goto _LL523;} else{ goto _LL524;} _LL524: goto _LL525; _LL523: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp530=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp530[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp531; _temp531.tag= Cyc_Absyn_IntType; _temp531.f1=( void*) sgn; _temp531.f2=(
void*) _temp526; _temp531;}); _temp530;}); goto _LL521; _LL525: Cyc_Parse_err(({
unsigned char* _temp532=( unsigned char*)"sign specification on non-integral type";
struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 40; _temp533;}), last_loc); goto _LL521;
_LL521:;} if( seen_size){ void* _temp534= t; void* _temp540; void* _temp542;
_LL536: if(( unsigned int) _temp534 > 4u?*(( int*) _temp534) == Cyc_Absyn_IntType:
0){ _LL543: _temp542=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp534)->f1;
goto _LL541; _LL541: _temp540=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp534)->f2; goto _LL537;} else{ goto _LL538;} _LL538: goto _LL539; _LL537: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp544=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp544[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp545; _temp545.tag= Cyc_Absyn_IntType; _temp545.f1=( void*) _temp542;
_temp545.f2=( void*) sz; _temp545;}); _temp544;}); goto _LL535; _LL539: Cyc_Parse_err(({
unsigned char* _temp546=( unsigned char*)"size qualifier on non-integral type";
struct _tagged_string _temp547; _temp547.curr= _temp546; _temp547.base= _temp546;
_temp547.last_plus_one= _temp546 + 36; _temp547;}), last_loc); goto _LL535;
_LL535:;}} return({ struct _tuple5 _temp548; _temp548.f1= t; _temp548.f2=
declopt; _temp548;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp549=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL550; _LL550: { struct _tuple1* _temp551= _temp549->id; goto _LL552;
_LL552: { struct Cyc_List_List* _temp555; struct Cyc_List_List* _temp557; void*
_temp559; struct Cyc_Absyn_Tqual _temp561; struct _tuple6 _temp553= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp549->tms); _LL562: _temp561= _temp553.f1; goto _LL560;
_LL560: _temp559= _temp553.f2; goto _LL558; _LL558: _temp557= _temp553.f3; goto
_LL556; _LL556: _temp555= _temp553.f4; goto _LL554; _LL554: return({ struct Cyc_List_List*
_temp563=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp563->hd=( void*)({ struct _tuple7* _temp564=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp564->f1= _temp551; _temp564->f2= _temp561;
_temp564->f3= _temp559; _temp564->f4= _temp557; _temp564->f5= _temp555; _temp564;});
_temp563->tl= Cyc_Parse_apply_tmss( _temp561, Cyc_Tcutil_copy_type( t),(( struct
Cyc_List_List*) _check_null( ds))->tl, shared_atts); _temp563;});}}}} static
struct _tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual tq, void* t, struct
Cyc_List_List* atts, struct Cyc_List_List* tms){ if( tms == 0){ return({ struct
_tuple6 _temp565; _temp565.f1= tq; _temp565.f2= t; _temp565.f3= 0; _temp565.f4=
atts; _temp565;});}{ void* _temp566=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp580; void* _temp582; int
_temp584; struct Cyc_Position_Segment* _temp586; struct Cyc_List_List* _temp588;
struct Cyc_Absyn_Tqual _temp590; void* _temp592; void* _temp594; struct Cyc_List_List*
_temp596; struct Cyc_Position_Segment* _temp598; _LL568: if( _temp566 ==( void*)
Cyc_Absyn_Carray_mod){ goto _LL569;} else{ goto _LL570;} _LL570: if((
unsigned int) _temp566 > 1u?*(( int*) _temp566) == Cyc_Absyn_ConstArray_mod: 0){
_LL581: _temp580=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp566)->f1; goto
_LL571;} else{ goto _LL572;} _LL572: if(( unsigned int) _temp566 > 1u?*(( int*)
_temp566) == Cyc_Absyn_Function_mod: 0){ _LL583: _temp582=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp566)->f1; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp566 > 1u?*(( int*) _temp566) == Cyc_Absyn_TypeParams_mod: 0){ _LL589:
_temp588=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp566)->f1; goto _LL587;
_LL587: _temp586=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp566)->f2; goto
_LL585; _LL585: _temp584=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp566)->f3;
goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int) _temp566 > 1u?*((
int*) _temp566) == Cyc_Absyn_Pointer_mod: 0){ _LL595: _temp594=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp566)->f1; goto _LL593; _LL593: _temp592=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp566)->f2; goto _LL591;
_LL591: _temp590=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp566)->f3; goto
_LL577;} else{ goto _LL578;} _LL578: if(( unsigned int) _temp566 > 1u?*(( int*)
_temp566) == Cyc_Absyn_Attributes_mod: 0){ _LL599: _temp598=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp566)->f1; goto _LL597; _LL597: _temp596=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp566)->f2; goto _LL579;} else{ goto _LL567;} _LL569: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp600=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp600[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp601; _temp601.tag= Cyc_Absyn_ArrayType;
_temp601.f1=( void*) t; _temp601.f2= tq; _temp601.f3= 0; _temp601;}); _temp600;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL571: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp602=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp602[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp603; _temp603.tag= Cyc_Absyn_ArrayType;
_temp603.f1=( void*) t; _temp603.f2= tq; _temp603.f3=( struct Cyc_Absyn_Exp*)
_temp580; _temp603;}); _temp602;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL573: { void* _temp604= _temp582; struct Cyc_List_List* _temp610;
struct Cyc_Core_Opt* _temp612; struct Cyc_Absyn_VarargInfo* _temp614; int
_temp616; struct Cyc_List_List* _temp618; struct Cyc_Position_Segment* _temp620;
struct Cyc_List_List* _temp622; _LL606: if(*(( int*) _temp604) == Cyc_Absyn_WithTypes){
_LL619: _temp618=(( struct Cyc_Absyn_WithTypes_struct*) _temp604)->f1; goto
_LL617; _LL617: _temp616=(( struct Cyc_Absyn_WithTypes_struct*) _temp604)->f2;
goto _LL615; _LL615: _temp614=(( struct Cyc_Absyn_WithTypes_struct*) _temp604)->f3;
goto _LL613; _LL613: _temp612=(( struct Cyc_Absyn_WithTypes_struct*) _temp604)->f4;
goto _LL611; _LL611: _temp610=(( struct Cyc_Absyn_WithTypes_struct*) _temp604)->f5;
goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp604) == Cyc_Absyn_NoTypes){
_LL623: _temp622=(( struct Cyc_Absyn_NoTypes_struct*) _temp604)->f1; goto _LL621;
_LL621: _temp620=(( struct Cyc_Absyn_NoTypes_struct*) _temp604)->f2; goto _LL609;}
else{ goto _LL605;} _LL607: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as != 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp624=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp624->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp624->tl= fn_atts; _temp624;});} else{ new_atts=({ struct Cyc_List_List*
_temp625=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp625->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp625->tl= new_atts; _temp625;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp626=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; int _temp632; struct Cyc_Position_Segment*
_temp634; struct Cyc_List_List* _temp636; _LL628: if(( unsigned int) _temp626 >
1u?*(( int*) _temp626) == Cyc_Absyn_TypeParams_mod: 0){ _LL637: _temp636=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp626)->f1; goto _LL635; _LL635:
_temp634=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp626)->f2; goto _LL633;
_LL633: _temp632=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp626)->f3; goto
_LL629;} else{ goto _LL630;} _LL630: goto _LL631; _LL629: typvars= _temp636; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL627; _LL631: goto _LL627;
_LL627:;} if((((( ! _temp616? _temp614 == 0: 0)? _temp618 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp618))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp618))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp618))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp618= 0;}
t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp618); return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ( typvars, _temp612, t, _temp618,
_temp616, _temp614, _temp610, fn_atts), new_atts,(( struct Cyc_List_List*)
_check_null( tms))->tl);} _LL609:( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp638=( unsigned char*)"function declaration without parameter types";
struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 45; _temp639;}), _temp620)); _LL605:;} _LL575:
if((( struct Cyc_List_List*) _check_null( tms))->tl == 0){ return({ struct
_tuple6 _temp640; _temp640.f1= tq; _temp640.f2= t; _temp640.f3= _temp588;
_temp640.f4= atts; _temp640;});}( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp641=( unsigned char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp642; _temp642.curr= _temp641; _temp642.base= _temp641;
_temp642.last_plus_one= _temp641 + 68; _temp642;}), _temp586)); _LL577: { void*
_temp643= _temp594; struct Cyc_Absyn_Exp* _temp651; struct Cyc_Absyn_Exp*
_temp653; _LL645: if(( unsigned int) _temp643 > 1u?*(( int*) _temp643) == Cyc_Absyn_NonNullable_ps:
0){ _LL652: _temp651=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp643)->f1;
goto _LL646;} else{ goto _LL647;} _LL647: if(( unsigned int) _temp643 > 1u?*((
int*) _temp643) == Cyc_Absyn_Nullable_ps: 0){ _LL654: _temp653=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp643)->f1; goto _LL648;} else{ goto _LL649;} _LL649: if( _temp643 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL650;} else{ goto _LL644;} _LL646: return Cyc_Parse_apply_tms(
_temp590, Cyc_Absyn_atb_typ( t, _temp592, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp655=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp655[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp656; _temp656.tag= Cyc_Absyn_Upper_b;
_temp656.f1= _temp651; _temp656;}); _temp655;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL648: return Cyc_Parse_apply_tms( _temp590, Cyc_Absyn_starb_typ(
t, _temp592, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp657=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp657[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp658; _temp658.tag= Cyc_Absyn_Upper_b;
_temp658.f1= _temp653; _temp658;}); _temp657;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL650: return Cyc_Parse_apply_tms( _temp590, Cyc_Absyn_tagged_typ(
t, _temp592, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL644:;}
_LL579: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp596),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL567:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp661; void* _temp663; struct _tuple5 _temp659= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL664: _temp663= _temp659.f1; goto _LL662; _LL662: _temp661=
_temp659.f2; goto _LL660; _LL660: if( _temp661 != 0){ Cyc_Parse_warn(({
unsigned char* _temp665=( unsigned char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 54; _temp666;}), loc);} return _temp663;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp667=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp667[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp668; _temp668.tag= Cyc_Absyn_Decl_s;
_temp668.f1= d; _temp668.f2= s; _temp668;}); _temp667;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(({ unsigned char* _temp669=(
unsigned char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp670; _temp670.curr= _temp669; _temp670.base= _temp669;
_temp670.last_plus_one= _temp669 + 47; _temp670;}), loc);} if( tss == 0){ Cyc_Parse_err(({
unsigned char* _temp671=( unsigned char*)"missing type specifiers in declaration";
struct _tagged_string _temp672; _temp672.curr= _temp671; _temp672.base= _temp671;
_temp672.last_plus_one= _temp671 + 39; _temp672;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp673=( void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v;
_LL675: if( _temp673 ==( void*) Cyc_Parse_Typedef_sc){ goto _LL676;} else{ goto
_LL677;} _LL677: if( _temp673 ==( void*) Cyc_Parse_Extern_sc){ goto _LL678;}
else{ goto _LL679;} _LL679: if( _temp673 ==( void*) Cyc_Parse_ExternC_sc){ goto
_LL680;} else{ goto _LL681;} _LL681: if( _temp673 ==( void*) Cyc_Parse_Static_sc){
goto _LL682;} else{ goto _LL683;} _LL683: if( _temp673 ==( void*) Cyc_Parse_Auto_sc){
goto _LL684;} else{ goto _LL685;} _LL685: if( _temp673 ==( void*) Cyc_Parse_Register_sc){
goto _LL686;} else{ goto _LL687;} _LL687: if( _temp673 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL688;} else{ goto _LL674;} _LL676: istypedef= 1; goto _LL674; _LL678: s=(
void*) Cyc_Absyn_Extern; goto _LL674; _LL680: s=( void*) Cyc_Absyn_ExternC; goto
_LL674; _LL682: s=( void*) Cyc_Absyn_Static; goto _LL674; _LL684: s=( void*) Cyc_Absyn_Public;
goto _LL674; _LL686: s=( void*) Cyc_Absyn_Public; goto _LL674; _LL688: s=( void*)
Cyc_Absyn_Abstract; goto _LL674; _LL674:;}{ struct Cyc_List_List* _temp691;
struct Cyc_List_List* _temp693; struct _tuple0 _temp689=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL694: _temp693= _temp689.f1;
goto _LL692; _LL692: _temp691= _temp689.f2; goto _LL690; _LL690: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp691; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp695= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL696; _LL696: if( _temp693 == 0){ struct Cyc_Core_Opt* _temp699; void*
_temp701; struct _tuple5 _temp697= _temp695; _LL702: _temp701= _temp697.f1; goto
_LL700; _LL700: _temp699= _temp697.f2; goto _LL698; _LL698: if( _temp699 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp699))->v;{ void* _temp703=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp715; struct Cyc_Absyn_Structdecl* _temp717; struct Cyc_Absyn_Uniondecl*
_temp719; struct Cyc_Absyn_Tuniondecl* _temp721; _LL705: if(*(( int*) _temp703)
== Cyc_Absyn_Enum_d){ _LL716: _temp715=(( struct Cyc_Absyn_Enum_d_struct*)
_temp703)->f1; goto _LL706;} else{ goto _LL707;} _LL707: if(*(( int*) _temp703)
== Cyc_Absyn_Struct_d){ _LL718: _temp717=(( struct Cyc_Absyn_Struct_d_struct*)
_temp703)->f1; goto _LL708;} else{ goto _LL709;} _LL709: if(*(( int*) _temp703)
== Cyc_Absyn_Union_d){ _LL720: _temp719=(( struct Cyc_Absyn_Union_d_struct*)
_temp703)->f1; goto _LL710;} else{ goto _LL711;} _LL711: if(*(( int*) _temp703)
== Cyc_Absyn_Tunion_d){ _LL722: _temp721=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp703)->f1; goto _LL712;} else{ goto _LL713;} _LL713: goto _LL714; _LL706:(
void*)( _temp715->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(({ unsigned char*
_temp723=( unsigned char*)"bad attributes on enum"; struct _tagged_string
_temp724; _temp724.curr= _temp723; _temp724.base= _temp723; _temp724.last_plus_one=
_temp723 + 23; _temp724;}), loc);} goto _LL704; _LL708:( void*)( _temp717->sc=(
void*) s); _temp717->attributes= atts; goto _LL704; _LL710:( void*)( _temp719->sc=(
void*) s); _temp719->attributes= atts; goto _LL704; _LL712:( void*)( _temp721->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp725=(
unsigned char*)"bad attributes on tunion"; struct _tagged_string _temp726;
_temp726.curr= _temp725; _temp726.base= _temp725; _temp726.last_plus_one=
_temp725 + 25; _temp726;}), loc);} goto _LL704; _LL714: Cyc_Parse_err(({
unsigned char* _temp727=( unsigned char*)"bad declaration"; struct
_tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 16; _temp728;}), loc); return 0; _LL704:;}
return({ struct Cyc_List_List* _temp729=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp729->hd=( void*) d; _temp729->tl= 0;
_temp729;});} else{ void* _temp730= _temp701; struct Cyc_Absyn_Structdecl**
_temp744; struct Cyc_List_List* _temp746; struct _tuple1* _temp748; struct Cyc_Absyn_TunionInfo
_temp750; void* _temp752; struct Cyc_List_List* _temp754; void* _temp756; struct
Cyc_Absyn_Tuniondecl* _temp758; struct Cyc_Absyn_TunionInfo _temp760; void*
_temp762; struct Cyc_List_List* _temp764; void* _temp766; struct Cyc_Absyn_UnknownTunionInfo
_temp768; int _temp770; struct _tuple1* _temp772; struct Cyc_Absyn_Uniondecl**
_temp774; struct Cyc_List_List* _temp776; struct _tuple1* _temp778; struct Cyc_Absyn_Enumdecl*
_temp780; struct _tuple1* _temp782; _LL732: if(( unsigned int) _temp730 > 4u?*((
int*) _temp730) == Cyc_Absyn_StructType: 0){ _LL749: _temp748=(( struct Cyc_Absyn_StructType_struct*)
_temp730)->f1; goto _LL747; _LL747: _temp746=(( struct Cyc_Absyn_StructType_struct*)
_temp730)->f2; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_StructType_struct*)
_temp730)->f3; goto _LL733;} else{ goto _LL734;} _LL734: if(( unsigned int)
_temp730 > 4u?*(( int*) _temp730) == Cyc_Absyn_TunionType: 0){ _LL751: _temp750=((
struct Cyc_Absyn_TunionType_struct*) _temp730)->f1; _LL757: _temp756=( void*)
_temp750.tunion_info; if(*(( int*) _temp756) == Cyc_Absyn_KnownTunion){ _LL759:
_temp758=(( struct Cyc_Absyn_KnownTunion_struct*) _temp756)->f1; goto _LL755;}
else{ goto _LL736;} _LL755: _temp754= _temp750.targs; goto _LL753; _LL753:
_temp752=( void*) _temp750.rgn; goto _LL735;} else{ goto _LL736;} _LL736: if((
unsigned int) _temp730 > 4u?*(( int*) _temp730) == Cyc_Absyn_TunionType: 0){
_LL761: _temp760=(( struct Cyc_Absyn_TunionType_struct*) _temp730)->f1; _LL767:
_temp766=( void*) _temp760.tunion_info; if(*(( int*) _temp766) == Cyc_Absyn_UnknownTunion){
_LL769: _temp768=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp766)->f1;
_LL773: _temp772= _temp768.name; goto _LL771; _LL771: _temp770= _temp768.is_xtunion;
goto _LL765;} else{ goto _LL738;} _LL765: _temp764= _temp760.targs; goto _LL763;
_LL763: _temp762=( void*) _temp760.rgn; goto _LL737;} else{ goto _LL738;} _LL738:
if(( unsigned int) _temp730 > 4u?*(( int*) _temp730) == Cyc_Absyn_UnionType: 0){
_LL779: _temp778=(( struct Cyc_Absyn_UnionType_struct*) _temp730)->f1; goto
_LL777; _LL777: _temp776=(( struct Cyc_Absyn_UnionType_struct*) _temp730)->f2;
goto _LL775; _LL775: _temp774=(( struct Cyc_Absyn_UnionType_struct*) _temp730)->f3;
goto _LL739;} else{ goto _LL740;} _LL740: if(( unsigned int) _temp730 > 4u?*((
int*) _temp730) == Cyc_Absyn_EnumType: 0){ _LL783: _temp782=(( struct Cyc_Absyn_EnumType_struct*)
_temp730)->f1; goto _LL781; _LL781: _temp780=(( struct Cyc_Absyn_EnumType_struct*)
_temp730)->f2; goto _LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL733: {
struct Cyc_List_List* _temp784=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp746);
goto _LL785; _LL785: { struct Cyc_Absyn_Structdecl* _temp788=({ struct Cyc_Absyn_Structdecl*
_temp786=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp786->sc=( void*) s; _temp786->name=({ struct Cyc_Core_Opt* _temp787=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp787->v=(
void*)(( struct _tuple1*) _check_null( _temp748)); _temp787;}); _temp786->tvs=
_temp784; _temp786->fields= 0; _temp786->attributes= 0; _temp786;}); goto _LL789;
_LL789: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp790=( unsigned char*)"bad attributes on struct";
struct _tagged_string _temp791; _temp791.curr= _temp790; _temp791.base= _temp790;
_temp791.last_plus_one= _temp790 + 25; _temp791;}), loc);} return({ struct Cyc_List_List*
_temp792=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp792->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp793=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp793[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp794; _temp794.tag= Cyc_Absyn_Struct_d;
_temp794.f1= _temp788; _temp794;}); _temp793;}), loc); _temp792->tl= 0; _temp792;});}}
_LL735: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp795=( unsigned char*)"bad attributes on tunion";
struct _tagged_string _temp796; _temp796.curr= _temp795; _temp796.base= _temp795;
_temp796.last_plus_one= _temp795 + 25; _temp796;}), loc);} return({ struct Cyc_List_List*
_temp797=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp797->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp798=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp798[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp799; _temp799.tag= Cyc_Absyn_Tunion_d;
_temp799.f1= _temp758; _temp799;}); _temp798;}), loc); _temp797->tl= 0; _temp797;});
_LL737: { struct Cyc_List_List* _temp800=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp764);
goto _LL801; _LL801: { struct Cyc_Absyn_Decl* _temp802= Cyc_Absyn_tunion_decl( s,
_temp772, _temp800, 0, _temp770, loc); goto _LL803; _LL803: if( atts != 0){ Cyc_Parse_err(({
unsigned char* _temp804=( unsigned char*)"bad attributes on tunion"; struct
_tagged_string _temp805; _temp805.curr= _temp804; _temp805.base= _temp804;
_temp805.last_plus_one= _temp804 + 25; _temp805;}), loc);} return({ struct Cyc_List_List*
_temp806=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp806->hd=( void*) _temp802; _temp806->tl= 0; _temp806;});}} _LL739: { struct
Cyc_List_List* _temp807=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp776); goto
_LL808; _LL808: { struct Cyc_Absyn_Uniondecl* _temp811=({ struct Cyc_Absyn_Uniondecl*
_temp809=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp809->sc=( void*) s; _temp809->name=({ struct Cyc_Core_Opt* _temp810=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp810->v=(
void*)(( struct _tuple1*) _check_null( _temp778)); _temp810;}); _temp809->tvs=
_temp807; _temp809->fields= 0; _temp809->attributes= 0; _temp809;}); goto _LL812;
_LL812: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp813=( unsigned char*)"bad attributes on union";
struct _tagged_string _temp814; _temp814.curr= _temp813; _temp814.base= _temp813;
_temp814.last_plus_one= _temp813 + 24; _temp814;}), loc);} return({ struct Cyc_List_List*
_temp815=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp815->hd=( void*)({ struct Cyc_Absyn_Decl* _temp816=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp816->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp817=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp817[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp818; _temp818.tag= Cyc_Absyn_Union_d; _temp818.f1= _temp811; _temp818;});
_temp817;})); _temp816->loc= loc; _temp816;}); _temp815->tl= 0; _temp815;});}}
_LL741: { struct Cyc_Absyn_Enumdecl* _temp820=({ struct Cyc_Absyn_Enumdecl*
_temp819=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp819->sc=( void*) s; _temp819->name= _temp782; _temp819->fields= 0; _temp819;});
goto _LL821; _LL821: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp822=(
unsigned char*)"bad attributes on enum"; struct _tagged_string _temp823;
_temp823.curr= _temp822; _temp823.base= _temp822; _temp823.last_plus_one=
_temp822 + 23; _temp823;}), loc);} return({ struct Cyc_List_List* _temp824=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp824->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp825=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp825->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp826=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp826[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp827; _temp827.tag= Cyc_Absyn_Enum_d;
_temp827.f1= _temp820; _temp827;}); _temp826;})); _temp825->loc= loc; _temp825;});
_temp824->tl= 0; _temp824;});} _LL743: Cyc_Parse_err(({ unsigned char* _temp828=(
unsigned char*)"missing declarator"; struct _tagged_string _temp829; _temp829.curr=
_temp828; _temp829.base= _temp828; _temp829.last_plus_one= _temp828 + 19;
_temp829;}), loc); return 0; _LL731:;}} else{ void* t= _temp695.f1; struct Cyc_List_List*
_temp830= Cyc_Parse_apply_tmss( tq, t, _temp693, atts); goto _LL831; _LL831: if(
istypedef){ if( ! exps_empty){ Cyc_Parse_err(({ unsigned char* _temp832=(
unsigned char*)"initializer in typedef declaration"; struct _tagged_string
_temp833; _temp833.curr= _temp832; _temp833.base= _temp832; _temp833.last_plus_one=
_temp832 + 35; _temp833;}), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp830); if( _temp695.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp695.f2))->v;{ void* _temp834=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp846; struct Cyc_Absyn_Tuniondecl* _temp848;
struct Cyc_Absyn_Uniondecl* _temp850; struct Cyc_Absyn_Enumdecl* _temp852;
_LL836: if(*(( int*) _temp834) == Cyc_Absyn_Struct_d){ _LL847: _temp846=((
struct Cyc_Absyn_Struct_d_struct*) _temp834)->f1; goto _LL837;} else{ goto
_LL838;} _LL838: if(*(( int*) _temp834) == Cyc_Absyn_Tunion_d){ _LL849: _temp848=((
struct Cyc_Absyn_Tunion_d_struct*) _temp834)->f1; goto _LL839;} else{ goto
_LL840;} _LL840: if(*(( int*) _temp834) == Cyc_Absyn_Union_d){ _LL851: _temp850=((
struct Cyc_Absyn_Union_d_struct*) _temp834)->f1; goto _LL841;} else{ goto _LL842;}
_LL842: if(*(( int*) _temp834) == Cyc_Absyn_Enum_d){ _LL853: _temp852=(( struct
Cyc_Absyn_Enum_d_struct*) _temp834)->f1; goto _LL843;} else{ goto _LL844;}
_LL844: goto _LL845; _LL837:( void*)( _temp846->sc=( void*) s); _temp846->attributes=
atts; atts= 0; goto _LL835; _LL839:( void*)( _temp848->sc=( void*) s); goto
_LL835; _LL841:( void*)( _temp850->sc=( void*) s); goto _LL835; _LL843:( void*)(
_temp852->sc=( void*) s); goto _LL835; _LL845: Cyc_Parse_err(({ unsigned char*
_temp854=( unsigned char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp855; _temp855.curr= _temp854; _temp855.base= _temp854;
_temp855.last_plus_one= _temp854 + 70; _temp855;}), loc); goto _LL835; _LL835:;}
decls=({ struct Cyc_List_List* _temp856=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp856->hd=( void*) d; _temp856->tl= decls;
_temp856;});} if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ struct
_tagged_string _temp857= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); xprintf("bad attribute %.*s in typedef", _temp857.last_plus_one
- _temp857.curr, _temp857.curr);}), loc);} return decls;}} else{ if( _temp695.f2
!= 0){ Cyc_Parse_unimp2(({ unsigned char* _temp858=( unsigned char*)"nested type declaration within declarator";
struct _tagged_string _temp859; _temp859.curr= _temp858; _temp859.base= _temp858;
_temp859.last_plus_one= _temp858 + 42; _temp859;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp860= _temp830; goto _LL861; _LL861: for( 0;
_temp860 != 0;( _temp860=(( struct Cyc_List_List*) _check_null( _temp860))->tl,
_temp691=(( struct Cyc_List_List*) _check_null( _temp691))->tl)){ struct _tuple7
_temp864; struct Cyc_List_List* _temp865; struct Cyc_List_List* _temp867; void*
_temp869; struct Cyc_Absyn_Tqual _temp871; struct _tuple1* _temp873; struct
_tuple7* _temp862=( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp860))->hd; _temp864=* _temp862; _LL874: _temp873= _temp864.f1; goto _LL872;
_LL872: _temp871= _temp864.f2; goto _LL870; _LL870: _temp869= _temp864.f3; goto
_LL868; _LL868: _temp867= _temp864.f4; goto _LL866; _LL866: _temp865= _temp864.f5;
goto _LL863; _LL863: if( _temp867 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp875=( unsigned char*)"bad type params, ignoring"; struct _tagged_string
_temp876; _temp876.curr= _temp875; _temp876.base= _temp875; _temp876.last_plus_one=
_temp875 + 26; _temp876;}), loc);} if( _temp691 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp877=( unsigned char*)"unexpected null in parse!"; struct
_tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 26; _temp878;}), loc);}{ struct Cyc_Absyn_Exp*
_temp879=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp691))->hd;
goto _LL880; _LL880: { struct Cyc_Absyn_Vardecl* _temp881= Cyc_Absyn_new_vardecl(
_temp873, _temp869, _temp879); goto _LL882; _LL882: _temp881->tq= _temp871;(
void*)( _temp881->sc=( void*) s); _temp881->attributes= _temp865;{ struct Cyc_Absyn_Decl*
_temp886=({ struct Cyc_Absyn_Decl* _temp883=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp883->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp884=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp884[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp885; _temp885.tag= Cyc_Absyn_Var_d;
_temp885.f1= _temp881; _temp885;}); _temp884;})); _temp883->loc= loc; _temp883;});
goto _LL887; _LL887: decls=({ struct Cyc_List_List* _temp888=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp888->hd=( void*) _temp886;
_temp888->tl= decls; _temp888;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(( struct _tagged_string)({ struct _tagged_string
_temp889= s; xprintf("bad kind: %.*s", _temp889.last_plus_one - _temp889.curr,
_temp889.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), 0)){ case 'A': _LL890:
return( void*) Cyc_Absyn_AnyKind; case 'M': _LL891: return( void*) Cyc_Absyn_MemKind;
case 'B': _LL892: return( void*) Cyc_Absyn_BoxKind; case 'R': _LL893: return(
void*) Cyc_Absyn_RgnKind; case 'E': _LL894: return( void*) Cyc_Absyn_EffKind;
default: _LL895: Cyc_Parse_err(( struct _tagged_string)({ struct _tagged_string
_temp897= s; xprintf("bad kind: %.*s", _temp897.last_plus_one - _temp897.curr,
_temp897.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static struct Cyc_List_List*
Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc, struct Cyc_List_List*
atts, struct Cyc_List_List* tms){ if( atts == 0){ return tms;} else{ return({
struct Cyc_List_List* _temp898=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp898->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp899=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp899[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp900; _temp900.tag= Cyc_Absyn_Attributes_mod; _temp900.f1= loc; _temp900.f2=
atts; _temp900;}); _temp899;})); _temp898->tl= tms; _temp898;});}} static struct
Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment* loc,
struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
if((* t).f5 != 0){ Cyc_Parse_err(( struct _tagged_string)({ struct
_tagged_string _temp901= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_temp901.last_plus_one - _temp901.curr, _temp901.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp902=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp902[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp903; _temp903.tag= Cyc_Absyn_Typedef_d; _temp903.f1=({
struct Cyc_Absyn_Typedefdecl* _temp904=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp904->name= x; _temp904->tvs=(*
t).f4; _temp904->defn=( void*)(* t).f3; _temp904;}); _temp903;}); _temp902;}),
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
unsigned char _temp908[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp908, _temp908, _temp908 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp909= yy1; struct _tuple16* _temp915; _LL911: if(*(( void**)
_temp909) == Cyc_Int_tok){ _LL916: _temp915=(( struct Cyc_Int_tok_struct*)
_temp909)->f1; goto _LL912;} else{ goto _LL913;} _LL913: goto _LL914; _LL912:
return _temp915; _LL914:( void) _throw(( void*)& Cyc_yyfail_Int_tok); _LL910:;}
static unsigned char _temp920[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp920, _temp920, _temp920 + 11u}};
struct _tagged_string Cyc_yyget_String_tok( void* yy1){ void* _temp921= yy1;
struct _tagged_string _temp927; _LL923: if(*(( void**) _temp921) == Cyc_String_tok){
_LL928: _temp927=(( struct Cyc_String_tok_struct*) _temp921)->f1; goto _LL924;}
else{ goto _LL925;} _LL925: goto _LL926; _LL924: return _temp927; _LL926:( void)
_throw(( void*)& Cyc_yyfail_String_tok); _LL922:;} static unsigned char _temp932[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp932, _temp932, _temp932 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp933= yy1; unsigned char _temp939; _LL935: if(*(( void**) _temp933) ==
Cyc_Char_tok){ _LL940: _temp939=(( struct Cyc_Char_tok_struct*) _temp933)->f1;
goto _LL936;} else{ goto _LL937;} _LL937: goto _LL938; _LL936: return _temp939;
_LL938:( void) _throw(( void*)& Cyc_yyfail_Char_tok); _LL934:;} static
unsigned char _temp944[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp944, _temp944, _temp944 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp945= yy1; void*
_temp951; _LL947: if(*(( void**) _temp945) == Cyc_Pointer_Sort_tok){ _LL952:
_temp951=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp945)->f1; goto
_LL948;} else{ goto _LL949;} _LL949: goto _LL950; _LL948: return _temp951;
_LL950:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL946:;} static
unsigned char _temp956[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp956, _temp956, _temp956 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp957= yy1; struct Cyc_Absyn_Exp*
_temp963; _LL959: if(*(( void**) _temp957) == Cyc_Exp_tok){ _LL964: _temp963=((
struct Cyc_Exp_tok_struct*) _temp957)->f1; goto _LL960;} else{ goto _LL961;}
_LL961: goto _LL962; _LL960: return _temp963; _LL962:( void) _throw(( void*)&
Cyc_yyfail_Exp_tok); _LL958:;} static unsigned char _temp968[ 12u]="ExpList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{
_temp968, _temp968, _temp968 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok(
void* yy1){ void* _temp969= yy1; struct Cyc_List_List* _temp975; _LL971: if(*((
void**) _temp969) == Cyc_ExpList_tok){ _LL976: _temp975=(( struct Cyc_ExpList_tok_struct*)
_temp969)->f1; goto _LL972;} else{ goto _LL973;} _LL973: goto _LL974; _LL972:
return _temp975; _LL974:( void) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL970:;}
static unsigned char _temp980[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{ _temp980, _temp980, _temp980
+ 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok( void* yy1){ void*
_temp981= yy1; struct Cyc_List_List* _temp987; _LL983: if(*(( void**) _temp981)
== Cyc_InitializerList_tok){ _LL988: _temp987=(( struct Cyc_InitializerList_tok_struct*)
_temp981)->f1; goto _LL984;} else{ goto _LL985;} _LL985: goto _LL986; _LL984:
return _temp987; _LL986:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL982:;} static unsigned char _temp992[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp992, _temp992, _temp992 + 11u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* _temp993= yy1; void* _temp999;
_LL995: if(*(( void**) _temp993) == Cyc_Primop_tok){ _LL1000: _temp999=( void*)((
struct Cyc_Primop_tok_struct*) _temp993)->f1; goto _LL996;} else{ goto _LL997;}
_LL997: goto _LL998; _LL996: return _temp999; _LL998:( void) _throw(( void*)&
Cyc_yyfail_Primop_tok); _LL994:;} static unsigned char _temp1004[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{
_temp1004, _temp1004, _temp1004 + 14u}}; struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok(
void* yy1){ void* _temp1005= yy1; struct Cyc_Core_Opt* _temp1011; _LL1007: if(*((
void**) _temp1005) == Cyc_Primopopt_tok){ _LL1012: _temp1011=(( struct Cyc_Primopopt_tok_struct*)
_temp1005)->f1; goto _LL1008;} else{ goto _LL1009;} _LL1009: goto _LL1010;
_LL1008: return _temp1011; _LL1010:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL1006:;} static unsigned char _temp1016[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{ _temp1016, _temp1016, _temp1016 + 11u}};
struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){ void* _temp1017= yy1; struct
_tuple1* _temp1023; _LL1019: if(*(( void**) _temp1017) == Cyc_QualId_tok){
_LL1024: _temp1023=(( struct Cyc_QualId_tok_struct*) _temp1017)->f1; goto
_LL1020;} else{ goto _LL1021;} _LL1021: goto _LL1022; _LL1020: return _temp1023;
_LL1022:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL1018:;} static
unsigned char _temp1028[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp1028, _temp1028, _temp1028 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1029= yy1;
struct Cyc_Absyn_Stmt* _temp1035; _LL1031: if(*(( void**) _temp1029) == Cyc_Stmt_tok){
_LL1036: _temp1035=(( struct Cyc_Stmt_tok_struct*) _temp1029)->f1; goto _LL1032;}
else{ goto _LL1033;} _LL1033: goto _LL1034; _LL1032: return _temp1035; _LL1034:(
void) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1030:;} static unsigned char
_temp1040[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp1040, _temp1040, _temp1040 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp1041= yy1; void* _temp1047; _LL1043: if(*(( void**)
_temp1041) == Cyc_BlockItem_tok){ _LL1048: _temp1047=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp1041)->f1; goto _LL1044;} else{ goto _LL1045;} _LL1045: goto _LL1046;
_LL1044: return _temp1047; _LL1046:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL1042:;} static unsigned char _temp1052[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp1052, _temp1052, _temp1052 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp1053= yy1; struct Cyc_List_List* _temp1059; _LL1055: if(*((
void**) _temp1053) == Cyc_SwitchClauseList_tok){ _LL1060: _temp1059=(( struct
Cyc_SwitchClauseList_tok_struct*) _temp1053)->f1; goto _LL1056;} else{ goto
_LL1057;} _LL1057: goto _LL1058; _LL1056: return _temp1059; _LL1058:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1054:;} static
unsigned char _temp1064[ 22u]="SwitchCClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{ _temp1064, _temp1064,
_temp1064 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok( void*
yy1){ void* _temp1065= yy1; struct Cyc_List_List* _temp1071; _LL1067: if(*((
void**) _temp1065) == Cyc_SwitchCClauseList_tok){ _LL1072: _temp1071=(( struct
Cyc_SwitchCClauseList_tok_struct*) _temp1065)->f1; goto _LL1068;} else{ goto
_LL1069;} _LL1069: goto _LL1070; _LL1068: return _temp1071; _LL1070:( void)
_throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok); _LL1066:;} static
unsigned char _temp1076[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp1076, _temp1076, _temp1076 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1077= yy1;
struct Cyc_Absyn_Pat* _temp1083; _LL1079: if(*(( void**) _temp1077) == Cyc_Pattern_tok){
_LL1084: _temp1083=(( struct Cyc_Pattern_tok_struct*) _temp1077)->f1; goto
_LL1080;} else{ goto _LL1081;} _LL1081: goto _LL1082; _LL1080: return _temp1083;
_LL1082:( void) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL1078:;} static
unsigned char _temp1088[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp1088, _temp1088, _temp1088
+ 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ void*
_temp1089= yy1; struct Cyc_List_List* _temp1095; _LL1091: if(*(( void**)
_temp1089) == Cyc_PatternList_tok){ _LL1096: _temp1095=(( struct Cyc_PatternList_tok_struct*)
_temp1089)->f1; goto _LL1092;} else{ goto _LL1093;} _LL1093: goto _LL1094;
_LL1092: return _temp1095; _LL1094:( void) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL1090:;} static unsigned char _temp1100[ 17u]="FieldPattern_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp1100, _temp1100, _temp1100 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp1101= yy1; struct _tuple14* _temp1107; _LL1103: if(*((
void**) _temp1101) == Cyc_FieldPattern_tok){ _LL1108: _temp1107=(( struct Cyc_FieldPattern_tok_struct*)
_temp1101)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: goto _LL1106;
_LL1104: return _temp1107; _LL1106:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1102:;} static unsigned char _temp1112[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp1112, _temp1112, _temp1112 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp1113= yy1; struct Cyc_List_List* _temp1119; _LL1115: if(*((
void**) _temp1113) == Cyc_FieldPatternList_tok){ _LL1120: _temp1119=(( struct
Cyc_FieldPatternList_tok_struct*) _temp1113)->f1; goto _LL1116;} else{ goto
_LL1117;} _LL1117: goto _LL1118; _LL1116: return _temp1119; _LL1118:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1114:;} static
unsigned char _temp1124[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp1124, _temp1124, _temp1124 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1125= yy1;
struct Cyc_Absyn_Fndecl* _temp1131; _LL1127: if(*(( void**) _temp1125) == Cyc_FnDecl_tok){
_LL1132: _temp1131=(( struct Cyc_FnDecl_tok_struct*) _temp1125)->f1; goto
_LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130; _LL1128: return _temp1131;
_LL1130:( void) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL1126:;} static
unsigned char _temp1136[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{ _temp1136, _temp1136, _temp1136 +
13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok( void* yy1){ void* _temp1137=
yy1; struct Cyc_List_List* _temp1143; _LL1139: if(*(( void**) _temp1137) == Cyc_DeclList_tok){
_LL1144: _temp1143=(( struct Cyc_DeclList_tok_struct*) _temp1137)->f1; goto
_LL1140;} else{ goto _LL1141;} _LL1141: goto _LL1142; _LL1140: return _temp1143;
_LL1142:( void) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1138:;} static
unsigned char _temp1148[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp1148, _temp1148, _temp1148 +
13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){
void* _temp1149= yy1; struct Cyc_Parse_Declaration_spec* _temp1155; _LL1151: if(*((
void**) _temp1149) == Cyc_DeclSpec_tok){ _LL1156: _temp1155=(( struct Cyc_DeclSpec_tok_struct*)
_temp1149)->f1; goto _LL1152;} else{ goto _LL1153;} _LL1153: goto _LL1154;
_LL1152: return _temp1155; _LL1154:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1150:;} static unsigned char _temp1160[ 13u]="InitDecl_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp1160,
_temp1160, _temp1160 + 13u}}; struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){
void* _temp1161= yy1; struct _tuple15* _temp1167; _LL1163: if(*(( void**)
_temp1161) == Cyc_InitDecl_tok){ _LL1168: _temp1167=(( struct Cyc_InitDecl_tok_struct*)
_temp1161)->f1; goto _LL1164;} else{ goto _LL1165;} _LL1165: goto _LL1166;
_LL1164: return _temp1167; _LL1166:( void) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL1162:;} static unsigned char _temp1172[ 17u]="InitDeclList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp1172, _temp1172, _temp1172 + 17u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ void* _temp1173= yy1; struct Cyc_List_List* _temp1179; _LL1175: if(*((
void**) _temp1173) == Cyc_InitDeclList_tok){ _LL1180: _temp1179=(( struct Cyc_InitDeclList_tok_struct*)
_temp1173)->f1; goto _LL1176;} else{ goto _LL1177;} _LL1177: goto _LL1178;
_LL1176: return _temp1179; _LL1178:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1174:;} static unsigned char _temp1184[ 17u]="StorageClass_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{
_temp1184, _temp1184, _temp1184 + 17u}}; void* Cyc_yyget_StorageClass_tok( void*
yy1){ void* _temp1185= yy1; void* _temp1191; _LL1187: if(*(( void**) _temp1185)
== Cyc_StorageClass_tok){ _LL1192: _temp1191=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp1185)->f1; goto _LL1188;} else{ goto _LL1189;} _LL1189: goto _LL1190;
_LL1188: return _temp1191; _LL1190:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1186:;} static unsigned char _temp1196[ 18u]="TypeSpecifier_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp1196, _temp1196, _temp1196 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp1197= yy1; void* _temp1203; _LL1199: if(*(( void**) _temp1197)
== Cyc_TypeSpecifier_tok){ _LL1204: _temp1203=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp1197)->f1; goto _LL1200;} else{ goto _LL1201;} _LL1201: goto _LL1202;
_LL1200: return _temp1203; _LL1202:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1198:;} static unsigned char _temp1208[ 18u]="StructOrUnion_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp1208, _temp1208, _temp1208 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1209= yy1; void* _temp1215; _LL1211: if(*(( void**) _temp1209)
== Cyc_StructOrUnion_tok){ _LL1216: _temp1215=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1209)->f1; goto _LL1212;} else{ goto _LL1213;} _LL1213: goto _LL1214;
_LL1212: return _temp1215; _LL1214:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1210:;} static unsigned char _temp1220[ 13u]="TypeQual_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp1220,
_temp1220, _temp1220 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1221= yy1; struct Cyc_Absyn_Tqual _temp1227; _LL1223: if(*((
void**) _temp1221) == Cyc_TypeQual_tok){ _LL1228: _temp1227=(( struct Cyc_TypeQual_tok_struct*)
_temp1221)->f1; goto _LL1224;} else{ goto _LL1225;} _LL1225: goto _LL1226;
_LL1224: return _temp1227; _LL1226:( void) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL1222:;} static unsigned char _temp1232[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp1232, _temp1232, _temp1232 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp1233= yy1; struct Cyc_List_List* _temp1239; _LL1235: if(*((
void**) _temp1233) == Cyc_StructFieldDeclList_tok){ _LL1240: _temp1239=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp1233)->f1; goto _LL1236;} else{ goto
_LL1237;} _LL1237: goto _LL1238; _LL1236: return _temp1239; _LL1238:( void)
_throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL1234:;} static
unsigned char _temp1244[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp1244, _temp1244,
_temp1244 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp1245= yy1; struct Cyc_List_List* _temp1251; _LL1247: if(*((
void**) _temp1245) == Cyc_StructFieldDeclListList_tok){ _LL1252: _temp1251=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp1245)->f1; goto _LL1248;}
else{ goto _LL1249;} _LL1249: goto _LL1250; _LL1248: return _temp1251; _LL1250:(
void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL1246:;}
static unsigned char _temp1256[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp1256, _temp1256,
_temp1256 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp1257= yy1; struct Cyc_List_List* _temp1263; _LL1259: if(*((
void**) _temp1257) == Cyc_TypeModifierList_tok){ _LL1264: _temp1263=(( struct
Cyc_TypeModifierList_tok_struct*) _temp1257)->f1; goto _LL1260;} else{ goto
_LL1261;} _LL1261: goto _LL1262; _LL1260: return _temp1263; _LL1262:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1258:;} static
unsigned char _temp1268[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp1268, _temp1268, _temp1268 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1269= yy1; void* _temp1275;
_LL1271: if(*(( void**) _temp1269) == Cyc_Rgn_tok){ _LL1276: _temp1275=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1269)->f1; goto _LL1272;} else{ goto _LL1273;}
_LL1273: goto _LL1274; _LL1272: return _temp1275; _LL1274:( void) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1270:;} static unsigned char _temp1280[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{
_temp1280, _temp1280, _temp1280 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp1281= yy1; struct Cyc_Parse_Declarator* _temp1287;
_LL1283: if(*(( void**) _temp1281) == Cyc_Declarator_tok){ _LL1288: _temp1287=((
struct Cyc_Declarator_tok_struct*) _temp1281)->f1; goto _LL1284;} else{ goto
_LL1285;} _LL1285: goto _LL1286; _LL1284: return _temp1287; _LL1286:( void)
_throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1282:;} static unsigned char
_temp1292[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1292, _temp1292,
_temp1292 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1293= yy1; struct _tuple13* _temp1299; _LL1295: if(*(( void**)
_temp1293) == Cyc_DeclaratorExpopt_tok){ _LL1300: _temp1299=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1293)->f1; goto _LL1296;} else{ goto _LL1297;} _LL1297: goto _LL1298;
_LL1296: return _temp1299; _LL1298:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1294:;} static unsigned char _temp1304[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1304, _temp1304, _temp1304 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1305= yy1; struct Cyc_List_List* _temp1311; _LL1307: if(*((
void**) _temp1305) == Cyc_DeclaratorExpoptList_tok){ _LL1312: _temp1311=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1305)->f1; goto _LL1308;}
else{ goto _LL1309;} _LL1309: goto _LL1310; _LL1308: return _temp1311; _LL1310:(
void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1306:;} static
unsigned char _temp1316[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1316, _temp1316,
_temp1316 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1317= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1323;
_LL1319: if(*(( void**) _temp1317) == Cyc_AbstractDeclarator_tok){ _LL1324:
_temp1323=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1317)->f1; goto
_LL1320;} else{ goto _LL1321;} _LL1321: goto _LL1322; _LL1320: return _temp1323;
_LL1322:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1318:;}
static unsigned char _temp1328[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1328, _temp1328, _temp1328 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1329= yy1; int _temp1335; _LL1331:
if(*(( void**) _temp1329) == Cyc_Bool_tok){ _LL1336: _temp1335=(( struct Cyc_Bool_tok_struct*)
_temp1329)->f1; goto _LL1332;} else{ goto _LL1333;} _LL1333: goto _LL1334;
_LL1332: return _temp1335; _LL1334:( void) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1330:;} static unsigned char _temp1340[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1340, _temp1340, _temp1340 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1341= yy1; void* _temp1347;
_LL1343: if(*(( void**) _temp1341) == Cyc_Scope_tok){ _LL1348: _temp1347=( void*)((
struct Cyc_Scope_tok_struct*) _temp1341)->f1; goto _LL1344;} else{ goto _LL1345;}
_LL1345: goto _LL1346; _LL1344: return _temp1347; _LL1346:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1342:;} static unsigned char _temp1352[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1352, _temp1352, _temp1352 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1353= yy1; struct Cyc_Absyn_Tunionfield* _temp1359;
_LL1355: if(*(( void**) _temp1353) == Cyc_TunionField_tok){ _LL1360: _temp1359=((
struct Cyc_TunionField_tok_struct*) _temp1353)->f1; goto _LL1356;} else{ goto
_LL1357;} _LL1357: goto _LL1358; _LL1356: return _temp1359; _LL1358:( void)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1354:;} static unsigned char
_temp1364[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1364, _temp1364, _temp1364 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1365= yy1; struct Cyc_List_List*
_temp1371; _LL1367: if(*(( void**) _temp1365) == Cyc_TunionFieldList_tok){
_LL1372: _temp1371=(( struct Cyc_TunionFieldList_tok_struct*) _temp1365)->f1;
goto _LL1368;} else{ goto _LL1369;} _LL1369: goto _LL1370; _LL1368: return
_temp1371; _LL1370:( void) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1366:;} static unsigned char _temp1376[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1376, _temp1376, _temp1376 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1377= yy1; struct _tuple18* _temp1383; _LL1379: if(*((
void**) _temp1377) == Cyc_QualSpecList_tok){ _LL1384: _temp1383=(( struct Cyc_QualSpecList_tok_struct*)
_temp1377)->f1; goto _LL1380;} else{ goto _LL1381;} _LL1381: goto _LL1382;
_LL1380: return _temp1383; _LL1382:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1378:;} static unsigned char _temp1388[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1388, _temp1388, _temp1388 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1389= yy1;
struct Cyc_List_List* _temp1395; _LL1391: if(*(( void**) _temp1389) == Cyc_IdList_tok){
_LL1396: _temp1395=(( struct Cyc_IdList_tok_struct*) _temp1389)->f1; goto
_LL1392;} else{ goto _LL1393;} _LL1393: goto _LL1394; _LL1392: return _temp1395;
_LL1394:( void) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1390:;} static
unsigned char _temp1400[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1400, _temp1400, _temp1400 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1401= yy1;
struct _tuple2* _temp1407; _LL1403: if(*(( void**) _temp1401) == Cyc_ParamDecl_tok){
_LL1408: _temp1407=(( struct Cyc_ParamDecl_tok_struct*) _temp1401)->f1; goto
_LL1404;} else{ goto _LL1405;} _LL1405: goto _LL1406; _LL1404: return _temp1407;
_LL1406:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1402:;} static
unsigned char _temp1412[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1412, _temp1412,
_temp1412 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1413= yy1; struct Cyc_List_List* _temp1419; _LL1415: if(*(( void**)
_temp1413) == Cyc_ParamDeclList_tok){ _LL1420: _temp1419=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1413)->f1; goto _LL1416;} else{ goto _LL1417;} _LL1417: goto _LL1418;
_LL1416: return _temp1419; _LL1418:( void) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1414:;} static unsigned char _temp1424[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1424, _temp1424, _temp1424 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1425= yy1; struct _tuple17* _temp1431; _LL1427: if(*((
void**) _temp1425) == Cyc_ParamDeclListBool_tok){ _LL1432: _temp1431=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1425)->f1; goto _LL1428;} else{ goto
_LL1429;} _LL1429: goto _LL1430; _LL1428: return _temp1431; _LL1430:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1426:;} static
unsigned char _temp1436[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1436, _temp1436, _temp1436 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1437=
yy1; struct Cyc_List_List* _temp1443; _LL1439: if(*(( void**) _temp1437) == Cyc_TypeList_tok){
_LL1444: _temp1443=(( struct Cyc_TypeList_tok_struct*) _temp1437)->f1; goto
_LL1440;} else{ goto _LL1441;} _LL1441: goto _LL1442; _LL1440: return _temp1443;
_LL1442:( void) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1438:;} static
unsigned char _temp1448[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1448, _temp1448,
_temp1448 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1449= yy1; struct Cyc_List_List* _temp1455; _LL1451: if(*(( void**)
_temp1449) == Cyc_DesignatorList_tok){ _LL1456: _temp1455=(( struct Cyc_DesignatorList_tok_struct*)
_temp1449)->f1; goto _LL1452;} else{ goto _LL1453;} _LL1453: goto _LL1454;
_LL1452: return _temp1455; _LL1454:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1450:;} static unsigned char _temp1460[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1460,
_temp1460, _temp1460 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1461= yy1; void* _temp1467; _LL1463: if(*(( void**) _temp1461) == Cyc_Designator_tok){
_LL1468: _temp1467=( void*)(( struct Cyc_Designator_tok_struct*) _temp1461)->f1;
goto _LL1464;} else{ goto _LL1465;} _LL1465: goto _LL1466; _LL1464: return
_temp1467; _LL1466:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1462:;}
static unsigned char _temp1472[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1472, _temp1472, _temp1472 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1473= yy1; void* _temp1479;
_LL1475: if(*(( void**) _temp1473) == Cyc_Kind_tok){ _LL1480: _temp1479=( void*)((
struct Cyc_Kind_tok_struct*) _temp1473)->f1; goto _LL1476;} else{ goto _LL1477;}
_LL1477: goto _LL1478; _LL1476: return _temp1479; _LL1478:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1474:;} static unsigned char _temp1484[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1484, _temp1484, _temp1484 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1485= yy1; void* _temp1491; _LL1487: if(*(( void**) _temp1485) == Cyc_Type_tok){
_LL1492: _temp1491=( void*)(( struct Cyc_Type_tok_struct*) _temp1485)->f1; goto
_LL1488;} else{ goto _LL1489;} _LL1489: goto _LL1490; _LL1488: return _temp1491;
_LL1490:( void) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1486:;} static
unsigned char _temp1496[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1496, _temp1496,
_temp1496 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1497= yy1; struct Cyc_List_List* _temp1503; _LL1499: if(*(( void**)
_temp1497) == Cyc_AttributeList_tok){ _LL1504: _temp1503=(( struct Cyc_AttributeList_tok_struct*)
_temp1497)->f1; goto _LL1500;} else{ goto _LL1501;} _LL1501: goto _LL1502;
_LL1500: return _temp1503; _LL1502:( void) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1498:;} static unsigned char _temp1508[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1508,
_temp1508, _temp1508 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1509= yy1; void* _temp1515; _LL1511: if(*(( void**) _temp1509) == Cyc_Attribute_tok){
_LL1516: _temp1515=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1509)->f1;
goto _LL1512;} else{ goto _LL1513;} _LL1513: goto _LL1514; _LL1512: return
_temp1515; _LL1514:( void) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1510:;}
static unsigned char _temp1520[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1520, _temp1520, _temp1520 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1521= yy1; struct Cyc_Absyn_Enumfield* _temp1527; _LL1523: if(*(( void**)
_temp1521) == Cyc_Enumfield_tok){ _LL1528: _temp1527=(( struct Cyc_Enumfield_tok_struct*)
_temp1521)->f1; goto _LL1524;} else{ goto _LL1525;} _LL1525: goto _LL1526;
_LL1524: return _temp1527; _LL1526:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1522:;} static unsigned char _temp1532[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1532, _temp1532, _temp1532 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1533= yy1; struct Cyc_List_List* _temp1539; _LL1535: if(*((
void**) _temp1533) == Cyc_EnumfieldList_tok){ _LL1540: _temp1539=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1533)->f1; goto _LL1536;} else{ goto _LL1537;} _LL1537: goto _LL1538;
_LL1536: return _temp1539; _LL1538:( void) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1534:;} static unsigned char _temp1544[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1544, _temp1544, _temp1544 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1545= yy1;
struct Cyc_Core_Opt* _temp1551; _LL1547: if(*(( void**) _temp1545) == Cyc_TypeOpt_tok){
_LL1552: _temp1551=(( struct Cyc_TypeOpt_tok_struct*) _temp1545)->f1; goto
_LL1548;} else{ goto _LL1549;} _LL1549: goto _LL1550; _LL1548: return _temp1551;
_LL1550:( void) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1546:;} static
unsigned char _temp1556[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1556, _temp1556, _temp1556 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1557=
yy1; struct Cyc_List_List* _temp1563; _LL1559: if(*(( void**) _temp1557) == Cyc_Rgnorder_tok){
_LL1564: _temp1563=(( struct Cyc_Rgnorder_tok_struct*) _temp1557)->f1; goto
_LL1560;} else{ goto _LL1561;} _LL1561: goto _LL1562; _LL1560: return _temp1563;
_LL1562:( void) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1558:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; struct Cyc_Yyltype Cyc_yynewloc(){ return({
struct Cyc_Yyltype _temp1565; _temp1565.timestamp= 0; _temp1565.first_line= 0;
_temp1565.first_column= 0; _temp1565.last_line= 0; _temp1565.last_column= 0;
_temp1565.text=({ unsigned char* _temp1566=( unsigned char*)""; struct
_tagged_string _temp1567; _temp1567.curr= _temp1566; _temp1567.base= _temp1566;
_temp1567.last_plus_one= _temp1566 + 1; _temp1567;}); _temp1565;});} static
unsigned char _temp1570[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0,{
_temp1570, _temp1570, _temp1570 + 1u}}; static short Cyc_yytranslate[ 353u]={ 0,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 124, 2, 2, 106, 122, 118, 2, 102, 103, 113, 116, 104, 117, 110,
121, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 109, 98, 107, 101, 108, 115, 114, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 111, 2, 112,
120, 105, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 99, 119, 100, 123, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97}; static
unsigned char _temp1573[ 2u]="$"; static unsigned char _temp1576[ 6u]="error";
static unsigned char _temp1579[ 12u]="$undefined."; static unsigned char
_temp1582[ 5u]="AUTO"; static unsigned char _temp1585[ 9u]="REGISTER"; static
unsigned char _temp1588[ 7u]="STATIC"; static unsigned char _temp1591[ 7u]="EXTERN";
static unsigned char _temp1594[ 8u]="TYPEDEF"; static unsigned char _temp1597[ 5u]="VOID";
static unsigned char _temp1600[ 5u]="CHAR"; static unsigned char _temp1603[ 6u]="SHORT";
static unsigned char _temp1606[ 4u]="INT"; static unsigned char _temp1609[ 5u]="LONG";
static unsigned char _temp1612[ 6u]="FLOAT"; static unsigned char _temp1615[ 7u]="DOUBLE";
static unsigned char _temp1618[ 7u]="SIGNED"; static unsigned char _temp1621[ 9u]="UNSIGNED";
static unsigned char _temp1624[ 6u]="CONST"; static unsigned char _temp1627[ 9u]="VOLATILE";
static unsigned char _temp1630[ 9u]="RESTRICT"; static unsigned char _temp1633[
7u]="STRUCT"; static unsigned char _temp1636[ 6u]="UNION"; static unsigned char
_temp1639[ 5u]="CASE"; static unsigned char _temp1642[ 8u]="DEFAULT"; static
unsigned char _temp1645[ 7u]="INLINE"; static unsigned char _temp1648[ 7u]="SIZEOF";
static unsigned char _temp1651[ 9u]="OFFSETOF"; static unsigned char _temp1654[
3u]="IF"; static unsigned char _temp1657[ 5u]="ELSE"; static unsigned char
_temp1660[ 7u]="SWITCH"; static unsigned char _temp1663[ 6u]="WHILE"; static
unsigned char _temp1666[ 3u]="DO"; static unsigned char _temp1669[ 4u]="FOR";
static unsigned char _temp1672[ 5u]="GOTO"; static unsigned char _temp1675[ 9u]="CONTINUE";
static unsigned char _temp1678[ 6u]="BREAK"; static unsigned char _temp1681[ 7u]="RETURN";
static unsigned char _temp1684[ 5u]="ENUM"; static unsigned char _temp1687[ 8u]="NULL_kw";
static unsigned char _temp1690[ 4u]="LET"; static unsigned char _temp1693[ 6u]="THROW";
static unsigned char _temp1696[ 4u]="TRY"; static unsigned char _temp1699[ 6u]="CATCH";
static unsigned char _temp1702[ 4u]="NEW"; static unsigned char _temp1705[ 9u]="ABSTRACT";
static unsigned char _temp1708[ 9u]="FALLTHRU"; static unsigned char _temp1711[
6u]="USING"; static unsigned char _temp1714[ 10u]="NAMESPACE"; static
unsigned char _temp1717[ 7u]="TUNION"; static unsigned char _temp1720[ 8u]="XTUNION";
static unsigned char _temp1723[ 5u]="FILL"; static unsigned char _temp1726[ 8u]="CODEGEN";
static unsigned char _temp1729[ 4u]="CUT"; static unsigned char _temp1732[ 7u]="SPLICE";
static unsigned char _temp1735[ 7u]="PRINTF"; static unsigned char _temp1738[ 8u]="FPRINTF";
static unsigned char _temp1741[ 8u]="XPRINTF"; static unsigned char _temp1744[ 6u]="SCANF";
static unsigned char _temp1747[ 7u]="FSCANF"; static unsigned char _temp1750[ 7u]="SSCANF";
static unsigned char _temp1753[ 7u]="MALLOC"; static unsigned char _temp1756[ 9u]="REGION_T";
static unsigned char _temp1759[ 7u]="REGION"; static unsigned char _temp1762[ 5u]="RNEW";
static unsigned char _temp1765[ 8u]="RMALLOC"; static unsigned char _temp1768[ 7u]="PTR_OP";
static unsigned char _temp1771[ 7u]="INC_OP"; static unsigned char _temp1774[ 7u]="DEC_OP";
static unsigned char _temp1777[ 8u]="LEFT_OP"; static unsigned char _temp1780[ 9u]="RIGHT_OP";
static unsigned char _temp1783[ 6u]="LE_OP"; static unsigned char _temp1786[ 6u]="GE_OP";
static unsigned char _temp1789[ 6u]="EQ_OP"; static unsigned char _temp1792[ 6u]="NE_OP";
static unsigned char _temp1795[ 7u]="AND_OP"; static unsigned char _temp1798[ 6u]="OR_OP";
static unsigned char _temp1801[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1804[ 11u]="DIV_ASSIGN"; static unsigned char _temp1807[ 11u]="MOD_ASSIGN";
static unsigned char _temp1810[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1813[ 11u]="SUB_ASSIGN"; static unsigned char _temp1816[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1819[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1822[ 11u]="AND_ASSIGN"; static unsigned char _temp1825[ 11u]="XOR_ASSIGN";
static unsigned char _temp1828[ 10u]="OR_ASSIGN"; static unsigned char _temp1831[
9u]="ELLIPSIS"; static unsigned char _temp1834[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1837[ 12u]="COLON_COLON"; static unsigned char _temp1840[ 11u]="IDENTIFIER";
static unsigned char _temp1843[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1846[ 7u]="STRING"; static unsigned char _temp1849[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1852[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1855[ 9u]="TYPE_VAR"; static unsigned char _temp1858[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1861[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1864[ 10u]="ATTRIBUTE"; static unsigned char _temp1867[ 4u]="';'"; static
unsigned char _temp1870[ 4u]="'{'"; static unsigned char _temp1873[ 4u]="'}'";
static unsigned char _temp1876[ 4u]="'='"; static unsigned char _temp1879[ 4u]="'('";
static unsigned char _temp1882[ 4u]="')'"; static unsigned char _temp1885[ 4u]="','";
static unsigned char _temp1888[ 4u]="'_'"; static unsigned char _temp1891[ 4u]="'$'";
static unsigned char _temp1894[ 4u]="'<'"; static unsigned char _temp1897[ 4u]="'>'";
static unsigned char _temp1900[ 4u]="':'"; static unsigned char _temp1903[ 4u]="'.'";
static unsigned char _temp1906[ 4u]="'['"; static unsigned char _temp1909[ 4u]="']'";
static unsigned char _temp1912[ 4u]="'*'"; static unsigned char _temp1915[ 4u]="'@'";
static unsigned char _temp1918[ 4u]="'?'"; static unsigned char _temp1921[ 4u]="'+'";
static unsigned char _temp1924[ 4u]="'-'"; static unsigned char _temp1927[ 4u]="'&'";
static unsigned char _temp1930[ 4u]="'|'"; static unsigned char _temp1933[ 4u]="'^'";
static unsigned char _temp1936[ 4u]="'/'"; static unsigned char _temp1939[ 4u]="'%'";
static unsigned char _temp1942[ 4u]="'~'"; static unsigned char _temp1945[ 4u]="'!'";
static unsigned char _temp1948[ 5u]="prog"; static unsigned char _temp1951[ 17u]="translation_unit";
static unsigned char _temp1954[ 21u]="external_declaration"; static
unsigned char _temp1957[ 20u]="function_definition"; static unsigned char
_temp1960[ 21u]="function_definition2"; static unsigned char _temp1963[ 13u]="using_action";
static unsigned char _temp1966[ 15u]="unusing_action"; static unsigned char
_temp1969[ 17u]="namespace_action"; static unsigned char _temp1972[ 19u]="unnamespace_action";
static unsigned char _temp1975[ 12u]="declaration"; static unsigned char
_temp1978[ 17u]="declaration_list"; static unsigned char _temp1981[ 23u]="declaration_specifiers";
static unsigned char _temp1984[ 24u]="storage_class_specifier"; static
unsigned char _temp1987[ 15u]="attributes_opt"; static unsigned char _temp1990[
11u]="attributes"; static unsigned char _temp1993[ 15u]="attribute_list"; static
unsigned char _temp1996[ 10u]="attribute"; static unsigned char _temp1999[ 15u]="type_specifier";
static unsigned char _temp2002[ 5u]="kind"; static unsigned char _temp2005[ 15u]="type_qualifier";
static unsigned char _temp2008[ 15u]="enum_specifier"; static unsigned char
_temp2011[ 11u]="enum_field"; static unsigned char _temp2014[ 22u]="enum_declaration_list";
static unsigned char _temp2017[ 26u]="struct_or_union_specifier"; static
unsigned char _temp2020[ 16u]="type_params_opt"; static unsigned char _temp2023[
16u]="struct_or_union"; static unsigned char _temp2026[ 24u]="struct_declaration_list";
static unsigned char _temp2029[ 25u]="struct_declaration_list0"; static
unsigned char _temp2032[ 21u]="init_declarator_list"; static unsigned char
_temp2035[ 22u]="init_declarator_list0"; static unsigned char _temp2038[ 16u]="init_declarator";
static unsigned char _temp2041[ 19u]="struct_declaration"; static unsigned char
_temp2044[ 25u]="specifier_qualifier_list"; static unsigned char _temp2047[ 23u]="struct_declarator_list";
static unsigned char _temp2050[ 24u]="struct_declarator_list0"; static
unsigned char _temp2053[ 18u]="struct_declarator"; static unsigned char
_temp2056[ 17u]="tunion_specifier"; static unsigned char _temp2059[ 18u]="tunion_or_xtunion";
static unsigned char _temp2062[ 17u]="tunionfield_list"; static unsigned char
_temp2065[ 18u]="tunionfield_scope"; static unsigned char _temp2068[ 12u]="tunionfield";
static unsigned char _temp2071[ 11u]="declarator"; static unsigned char
_temp2074[ 18u]="direct_declarator"; static unsigned char _temp2077[ 8u]="pointer";
static unsigned char _temp2080[ 13u]="pointer_char"; static unsigned char
_temp2083[ 8u]="rgn_opt"; static unsigned char _temp2086[ 4u]="rgn"; static
unsigned char _temp2089[ 20u]="type_qualifier_list"; static unsigned char
_temp2092[ 20u]="parameter_type_list"; static unsigned char _temp2095[ 16u]="optional_effect";
static unsigned char _temp2098[ 19u]="optional_rgn_order"; static unsigned char
_temp2101[ 10u]="rgn_order"; static unsigned char _temp2104[ 16u]="optional_inject";
static unsigned char _temp2107[ 11u]="effect_set"; static unsigned char
_temp2110[ 14u]="atomic_effect"; static unsigned char _temp2113[ 11u]="region_set";
static unsigned char _temp2116[ 15u]="parameter_list"; static unsigned char
_temp2119[ 22u]="parameter_declaration"; static unsigned char _temp2122[ 16u]="identifier_list";
static unsigned char _temp2125[ 17u]="identifier_list0"; static unsigned char
_temp2128[ 12u]="initializer"; static unsigned char _temp2131[ 18u]="array_initializer";
static unsigned char _temp2134[ 17u]="initializer_list"; static unsigned char
_temp2137[ 12u]="designation"; static unsigned char _temp2140[ 16u]="designator_list";
static unsigned char _temp2143[ 11u]="designator"; static unsigned char
_temp2146[ 10u]="type_name"; static unsigned char _temp2149[ 14u]="any_type_name";
static unsigned char _temp2152[ 15u]="type_name_list"; static unsigned char
_temp2155[ 20u]="abstract_declarator"; static unsigned char _temp2158[ 27u]="direct_abstract_declarator";
static unsigned char _temp2161[ 10u]="statement"; static unsigned char _temp2164[
18u]="labeled_statement"; static unsigned char _temp2167[ 21u]="expression_statement";
static unsigned char _temp2170[ 19u]="compound_statement"; static unsigned char
_temp2173[ 16u]="block_item_list"; static unsigned char _temp2176[ 11u]="block_item";
static unsigned char _temp2179[ 20u]="selection_statement"; static unsigned char
_temp2182[ 15u]="switch_clauses"; static unsigned char _temp2185[ 16u]="switchC_clauses";
static unsigned char _temp2188[ 20u]="iteration_statement"; static unsigned char
_temp2191[ 15u]="jump_statement"; static unsigned char _temp2194[ 8u]="pattern";
static unsigned char _temp2197[ 19u]="tuple_pattern_list"; static unsigned char
_temp2200[ 20u]="tuple_pattern_list0"; static unsigned char _temp2203[ 14u]="field_pattern";
static unsigned char _temp2206[ 19u]="field_pattern_list"; static unsigned char
_temp2209[ 20u]="field_pattern_list0"; static unsigned char _temp2212[ 11u]="expression";
static unsigned char _temp2215[ 22u]="assignment_expression"; static
unsigned char _temp2218[ 20u]="assignment_operator"; static unsigned char
_temp2221[ 23u]="conditional_expression"; static unsigned char _temp2224[ 20u]="constant_expression";
static unsigned char _temp2227[ 22u]="logical_or_expression"; static
unsigned char _temp2230[ 23u]="logical_and_expression"; static unsigned char
_temp2233[ 24u]="inclusive_or_expression"; static unsigned char _temp2236[ 24u]="exclusive_or_expression";
static unsigned char _temp2239[ 15u]="and_expression"; static unsigned char
_temp2242[ 20u]="equality_expression"; static unsigned char _temp2245[ 22u]="relational_expression";
static unsigned char _temp2248[ 17u]="shift_expression"; static unsigned char
_temp2251[ 20u]="additive_expression"; static unsigned char _temp2254[ 26u]="multiplicative_expression";
static unsigned char _temp2257[ 16u]="cast_expression"; static unsigned char
_temp2260[ 17u]="unary_expression"; static unsigned char _temp2263[ 14u]="format_primop";
static unsigned char _temp2266[ 15u]="unary_operator"; static unsigned char
_temp2269[ 19u]="postfix_expression"; static unsigned char _temp2272[ 19u]="primary_expression";
static unsigned char _temp2275[ 25u]="argument_expression_list"; static
unsigned char _temp2278[ 26u]="argument_expression_list0"; static unsigned char
_temp2281[ 9u]="constant"; static unsigned char _temp2284[ 20u]="qual_opt_identifier";
static struct _tagged_string Cyc_yytname[ 238u]={{ _temp1573, _temp1573,
_temp1573 + 2u},{ _temp1576, _temp1576, _temp1576 + 6u},{ _temp1579, _temp1579,
_temp1579 + 12u},{ _temp1582, _temp1582, _temp1582 + 5u},{ _temp1585, _temp1585,
_temp1585 + 9u},{ _temp1588, _temp1588, _temp1588 + 7u},{ _temp1591, _temp1591,
_temp1591 + 7u},{ _temp1594, _temp1594, _temp1594 + 8u},{ _temp1597, _temp1597,
_temp1597 + 5u},{ _temp1600, _temp1600, _temp1600 + 5u},{ _temp1603, _temp1603,
_temp1603 + 6u},{ _temp1606, _temp1606, _temp1606 + 4u},{ _temp1609, _temp1609,
_temp1609 + 5u},{ _temp1612, _temp1612, _temp1612 + 6u},{ _temp1615, _temp1615,
_temp1615 + 7u},{ _temp1618, _temp1618, _temp1618 + 7u},{ _temp1621, _temp1621,
_temp1621 + 9u},{ _temp1624, _temp1624, _temp1624 + 6u},{ _temp1627, _temp1627,
_temp1627 + 9u},{ _temp1630, _temp1630, _temp1630 + 9u},{ _temp1633, _temp1633,
_temp1633 + 7u},{ _temp1636, _temp1636, _temp1636 + 6u},{ _temp1639, _temp1639,
_temp1639 + 5u},{ _temp1642, _temp1642, _temp1642 + 8u},{ _temp1645, _temp1645,
_temp1645 + 7u},{ _temp1648, _temp1648, _temp1648 + 7u},{ _temp1651, _temp1651,
_temp1651 + 9u},{ _temp1654, _temp1654, _temp1654 + 3u},{ _temp1657, _temp1657,
_temp1657 + 5u},{ _temp1660, _temp1660, _temp1660 + 7u},{ _temp1663, _temp1663,
_temp1663 + 6u},{ _temp1666, _temp1666, _temp1666 + 3u},{ _temp1669, _temp1669,
_temp1669 + 4u},{ _temp1672, _temp1672, _temp1672 + 5u},{ _temp1675, _temp1675,
_temp1675 + 9u},{ _temp1678, _temp1678, _temp1678 + 6u},{ _temp1681, _temp1681,
_temp1681 + 7u},{ _temp1684, _temp1684, _temp1684 + 5u},{ _temp1687, _temp1687,
_temp1687 + 8u},{ _temp1690, _temp1690, _temp1690 + 4u},{ _temp1693, _temp1693,
_temp1693 + 6u},{ _temp1696, _temp1696, _temp1696 + 4u},{ _temp1699, _temp1699,
_temp1699 + 6u},{ _temp1702, _temp1702, _temp1702 + 4u},{ _temp1705, _temp1705,
_temp1705 + 9u},{ _temp1708, _temp1708, _temp1708 + 9u},{ _temp1711, _temp1711,
_temp1711 + 6u},{ _temp1714, _temp1714, _temp1714 + 10u},{ _temp1717, _temp1717,
_temp1717 + 7u},{ _temp1720, _temp1720, _temp1720 + 8u},{ _temp1723, _temp1723,
_temp1723 + 5u},{ _temp1726, _temp1726, _temp1726 + 8u},{ _temp1729, _temp1729,
_temp1729 + 4u},{ _temp1732, _temp1732, _temp1732 + 7u},{ _temp1735, _temp1735,
_temp1735 + 7u},{ _temp1738, _temp1738, _temp1738 + 8u},{ _temp1741, _temp1741,
_temp1741 + 8u},{ _temp1744, _temp1744, _temp1744 + 6u},{ _temp1747, _temp1747,
_temp1747 + 7u},{ _temp1750, _temp1750, _temp1750 + 7u},{ _temp1753, _temp1753,
_temp1753 + 7u},{ _temp1756, _temp1756, _temp1756 + 9u},{ _temp1759, _temp1759,
_temp1759 + 7u},{ _temp1762, _temp1762, _temp1762 + 5u},{ _temp1765, _temp1765,
_temp1765 + 8u},{ _temp1768, _temp1768, _temp1768 + 7u},{ _temp1771, _temp1771,
_temp1771 + 7u},{ _temp1774, _temp1774, _temp1774 + 7u},{ _temp1777, _temp1777,
_temp1777 + 8u},{ _temp1780, _temp1780, _temp1780 + 9u},{ _temp1783, _temp1783,
_temp1783 + 6u},{ _temp1786, _temp1786, _temp1786 + 6u},{ _temp1789, _temp1789,
_temp1789 + 6u},{ _temp1792, _temp1792, _temp1792 + 6u},{ _temp1795, _temp1795,
_temp1795 + 7u},{ _temp1798, _temp1798, _temp1798 + 6u},{ _temp1801, _temp1801,
_temp1801 + 11u},{ _temp1804, _temp1804, _temp1804 + 11u},{ _temp1807, _temp1807,
_temp1807 + 11u},{ _temp1810, _temp1810, _temp1810 + 11u},{ _temp1813, _temp1813,
_temp1813 + 11u},{ _temp1816, _temp1816, _temp1816 + 12u},{ _temp1819, _temp1819,
_temp1819 + 13u},{ _temp1822, _temp1822, _temp1822 + 11u},{ _temp1825, _temp1825,
_temp1825 + 11u},{ _temp1828, _temp1828, _temp1828 + 10u},{ _temp1831, _temp1831,
_temp1831 + 9u},{ _temp1834, _temp1834, _temp1834 + 11u},{ _temp1837, _temp1837,
_temp1837 + 12u},{ _temp1840, _temp1840, _temp1840 + 11u},{ _temp1843, _temp1843,
_temp1843 + 17u},{ _temp1846, _temp1846, _temp1846 + 7u},{ _temp1849, _temp1849,
_temp1849 + 19u},{ _temp1852, _temp1852, _temp1852 + 18u},{ _temp1855, _temp1855,
_temp1855 + 9u},{ _temp1858, _temp1858, _temp1858 + 16u},{ _temp1861, _temp1861,
_temp1861 + 18u},{ _temp1864, _temp1864, _temp1864 + 10u},{ _temp1867, _temp1867,
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
_temp1927 + 4u},{ _temp1930, _temp1930, _temp1930 + 4u},{ _temp1933, _temp1933,
_temp1933 + 4u},{ _temp1936, _temp1936, _temp1936 + 4u},{ _temp1939, _temp1939,
_temp1939 + 4u},{ _temp1942, _temp1942, _temp1942 + 4u},{ _temp1945, _temp1945,
_temp1945 + 4u},{ _temp1948, _temp1948, _temp1948 + 5u},{ _temp1951, _temp1951,
_temp1951 + 17u},{ _temp1954, _temp1954, _temp1954 + 21u},{ _temp1957, _temp1957,
_temp1957 + 20u},{ _temp1960, _temp1960, _temp1960 + 21u},{ _temp1963, _temp1963,
_temp1963 + 13u},{ _temp1966, _temp1966, _temp1966 + 15u},{ _temp1969, _temp1969,
_temp1969 + 17u},{ _temp1972, _temp1972, _temp1972 + 19u},{ _temp1975, _temp1975,
_temp1975 + 12u},{ _temp1978, _temp1978, _temp1978 + 17u},{ _temp1981, _temp1981,
_temp1981 + 23u},{ _temp1984, _temp1984, _temp1984 + 24u},{ _temp1987, _temp1987,
_temp1987 + 15u},{ _temp1990, _temp1990, _temp1990 + 11u},{ _temp1993, _temp1993,
_temp1993 + 15u},{ _temp1996, _temp1996, _temp1996 + 10u},{ _temp1999, _temp1999,
_temp1999 + 15u},{ _temp2002, _temp2002, _temp2002 + 5u},{ _temp2005, _temp2005,
_temp2005 + 15u},{ _temp2008, _temp2008, _temp2008 + 15u},{ _temp2011, _temp2011,
_temp2011 + 11u},{ _temp2014, _temp2014, _temp2014 + 22u},{ _temp2017, _temp2017,
_temp2017 + 26u},{ _temp2020, _temp2020, _temp2020 + 16u},{ _temp2023, _temp2023,
_temp2023 + 16u},{ _temp2026, _temp2026, _temp2026 + 24u},{ _temp2029, _temp2029,
_temp2029 + 25u},{ _temp2032, _temp2032, _temp2032 + 21u},{ _temp2035, _temp2035,
_temp2035 + 22u},{ _temp2038, _temp2038, _temp2038 + 16u},{ _temp2041, _temp2041,
_temp2041 + 19u},{ _temp2044, _temp2044, _temp2044 + 25u},{ _temp2047, _temp2047,
_temp2047 + 23u},{ _temp2050, _temp2050, _temp2050 + 24u},{ _temp2053, _temp2053,
_temp2053 + 18u},{ _temp2056, _temp2056, _temp2056 + 17u},{ _temp2059, _temp2059,
_temp2059 + 18u},{ _temp2062, _temp2062, _temp2062 + 17u},{ _temp2065, _temp2065,
_temp2065 + 18u},{ _temp2068, _temp2068, _temp2068 + 12u},{ _temp2071, _temp2071,
_temp2071 + 11u},{ _temp2074, _temp2074, _temp2074 + 18u},{ _temp2077, _temp2077,
_temp2077 + 8u},{ _temp2080, _temp2080, _temp2080 + 13u},{ _temp2083, _temp2083,
_temp2083 + 8u},{ _temp2086, _temp2086, _temp2086 + 4u},{ _temp2089, _temp2089,
_temp2089 + 20u},{ _temp2092, _temp2092, _temp2092 + 20u},{ _temp2095, _temp2095,
_temp2095 + 16u},{ _temp2098, _temp2098, _temp2098 + 19u},{ _temp2101, _temp2101,
_temp2101 + 10u},{ _temp2104, _temp2104, _temp2104 + 16u},{ _temp2107, _temp2107,
_temp2107 + 11u},{ _temp2110, _temp2110, _temp2110 + 14u},{ _temp2113, _temp2113,
_temp2113 + 11u},{ _temp2116, _temp2116, _temp2116 + 15u},{ _temp2119, _temp2119,
_temp2119 + 22u},{ _temp2122, _temp2122, _temp2122 + 16u},{ _temp2125, _temp2125,
_temp2125 + 17u},{ _temp2128, _temp2128, _temp2128 + 12u},{ _temp2131, _temp2131,
_temp2131 + 18u},{ _temp2134, _temp2134, _temp2134 + 17u},{ _temp2137, _temp2137,
_temp2137 + 12u},{ _temp2140, _temp2140, _temp2140 + 16u},{ _temp2143, _temp2143,
_temp2143 + 11u},{ _temp2146, _temp2146, _temp2146 + 10u},{ _temp2149, _temp2149,
_temp2149 + 14u},{ _temp2152, _temp2152, _temp2152 + 15u},{ _temp2155, _temp2155,
_temp2155 + 20u},{ _temp2158, _temp2158, _temp2158 + 27u},{ _temp2161, _temp2161,
_temp2161 + 10u},{ _temp2164, _temp2164, _temp2164 + 18u},{ _temp2167, _temp2167,
_temp2167 + 21u},{ _temp2170, _temp2170, _temp2170 + 19u},{ _temp2173, _temp2173,
_temp2173 + 16u},{ _temp2176, _temp2176, _temp2176 + 11u},{ _temp2179, _temp2179,
_temp2179 + 20u},{ _temp2182, _temp2182, _temp2182 + 15u},{ _temp2185, _temp2185,
_temp2185 + 16u},{ _temp2188, _temp2188, _temp2188 + 20u},{ _temp2191, _temp2191,
_temp2191 + 15u},{ _temp2194, _temp2194, _temp2194 + 8u},{ _temp2197, _temp2197,
_temp2197 + 19u},{ _temp2200, _temp2200, _temp2200 + 20u},{ _temp2203, _temp2203,
_temp2203 + 14u},{ _temp2206, _temp2206, _temp2206 + 19u},{ _temp2209, _temp2209,
_temp2209 + 20u},{ _temp2212, _temp2212, _temp2212 + 11u},{ _temp2215, _temp2215,
_temp2215 + 22u},{ _temp2218, _temp2218, _temp2218 + 20u},{ _temp2221, _temp2221,
_temp2221 + 23u},{ _temp2224, _temp2224, _temp2224 + 20u},{ _temp2227, _temp2227,
_temp2227 + 22u},{ _temp2230, _temp2230, _temp2230 + 23u},{ _temp2233, _temp2233,
_temp2233 + 24u},{ _temp2236, _temp2236, _temp2236 + 24u},{ _temp2239, _temp2239,
_temp2239 + 15u},{ _temp2242, _temp2242, _temp2242 + 20u},{ _temp2245, _temp2245,
_temp2245 + 22u},{ _temp2248, _temp2248, _temp2248 + 17u},{ _temp2251, _temp2251,
_temp2251 + 20u},{ _temp2254, _temp2254, _temp2254 + 26u},{ _temp2257, _temp2257,
_temp2257 + 16u},{ _temp2260, _temp2260, _temp2260 + 17u},{ _temp2263, _temp2263,
_temp2263 + 14u},{ _temp2266, _temp2266, _temp2266 + 15u},{ _temp2269, _temp2269,
_temp2269 + 19u},{ _temp2272, _temp2272, _temp2272 + 19u},{ _temp2275, _temp2275,
_temp2275 + 25u},{ _temp2278, _temp2278, _temp2278 + 26u},{ _temp2281, _temp2281,
_temp2281 + 9u},{ _temp2284, _temp2284, _temp2284 + 20u}}; static short Cyc_yyr1[
407u]={ 0, 125, 126, 126, 126, 126, 126, 126, 126, 127, 127, 128, 128, 128, 128,
129, 129, 130, 131, 132, 133, 134, 134, 134, 134, 135, 135, 136, 136, 136, 136,
136, 136, 136, 136, 137, 137, 137, 137, 137, 137, 137, 138, 138, 139, 140, 140,
141, 141, 141, 141, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142,
142, 142, 142, 142, 142, 142, 143, 143, 144, 144, 144, 145, 145, 146, 146, 147,
147, 148, 148, 148, 148, 148, 149, 149, 149, 150, 150, 151, 152, 152, 153, 154,
154, 155, 155, 156, 157, 157, 157, 157, 158, 159, 159, 160, 160, 160, 161, 161,
161, 161, 162, 162, 163, 163, 163, 163, 164, 164, 164, 165, 165, 166, 166, 167,
167, 167, 167, 167, 167, 167, 167, 167, 167, 168, 168, 168, 168, 169, 169, 169,
169, 169, 170, 170, 171, 171, 171, 172, 172, 173, 173, 173, 173, 174, 174, 175,
175, 176, 176, 177, 177, 178, 178, 179, 179, 179, 179, 180, 180, 180, 180, 181,
181, 182, 182, 182, 183, 184, 184, 185, 185, 186, 186, 186, 186, 187, 187, 187,
187, 188, 189, 189, 190, 190, 191, 191, 192, 192, 192, 192, 193, 193, 194, 194,
194, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 196, 196, 196,
196, 196, 196, 196, 196, 196, 196, 197, 198, 198, 199, 199, 200, 200, 201, 201,
201, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 204, 204, 204, 204,
205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 206, 206,
206, 206, 206, 206, 206, 206, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
207, 207, 207, 207, 208, 208, 209, 209, 210, 210, 211, 212, 212, 213, 213, 214,
214, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 216, 216, 216, 216,
216, 216, 216, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 222, 223, 223,
223, 224, 224, 224, 224, 224, 225, 225, 225, 226, 226, 226, 227, 227, 227, 227,
228, 228, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 230,
230, 230, 230, 230, 230, 231, 231, 231, 232, 232, 232, 232, 232, 232, 232, 232,
232, 232, 232, 232, 232, 232, 233, 233, 233, 233, 233, 233, 233, 233, 233, 234,
235, 235, 236, 236, 236, 236, 237, 237}; static short Cyc_yyr2[ 407u]={ 0, 1, 2,
3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2,
3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 4, 4, 1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6, 6, 3,
3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 3,
4, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2, 3, 4,
4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 3, 1, 1, 2, 3, 5, 6, 8, 0, 2, 0, 2, 3, 5, 0, 1, 1,
3, 2, 3, 1, 3, 1, 3, 3, 5, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4,
2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4,
2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 1, 1, 5, 7, 7, 8, 6, 0,
3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2,
3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3,
1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3,
1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1,
2, 2, 2, 2, 2, 2, 4, 2, 6, 4, 7, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 3, 4, 3, 3,
3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1};
static short Cyc_yydefact[ 796u]={ 8, 35, 36, 37, 38, 40, 51, 53, 54, 55, 56, 57,
58, 59, 60, 71, 72, 73, 88, 89, 42, 0, 0, 41, 0, 0, 113, 114, 0, 405, 64, 406,
85, 0, 52, 0, 140, 141, 144, 1, 8, 9, 0, 0, 10, 0, 42, 42, 42, 61, 62, 0, 63, 0,
0, 124, 0, 145, 126, 39, 0, 33, 43, 75, 283, 405, 279, 282, 281, 0, 277, 0, 0, 0,
0, 0, 179, 0, 284, 17, 19, 0, 0, 0, 66, 0, 0, 0, 0, 2, 8, 8, 8, 8, 21, 0, 93, 94,
96, 27, 29, 31, 85, 0, 85, 147, 149, 0, 85, 38, 0, 25, 0, 0, 11, 156, 0, 0, 135,
125, 42, 146, 8, 0, 34, 0, 0, 291, 290, 280, 289, 24, 0, 0, 0, 0, 42, 42, 197,
199, 0, 69, 70, 65, 203, 0, 127, 177, 0, 174, 0, 0, 404, 0, 0, 0, 0, 366, 367,
368, 369, 370, 371, 0, 0, 0, 0, 0, 401, 391, 402, 403, 0, 0, 0, 0, 374, 0, 372,
373, 0, 302, 315, 323, 325, 327, 329, 331, 333, 336, 341, 344, 347, 351, 0, 0,
353, 375, 390, 389, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 84, 0, 90, 91, 0,
83, 0, 85, 0, 110, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 405, 231, 233,
0, 239, 237, 0, 238, 220, 221, 222, 0, 235, 223, 224, 225, 0, 300, 26, 13, 96, 0,
180, 0, 0, 158, 156, 0, 0, 128, 0, 136, 0, 0, 78, 0, 76, 278, 293, 0, 292, 181,
0, 0, 291, 170, 200, 0, 99, 101, 156, 0, 205, 198, 206, 68, 0, 87, 0, 86, 156,
176, 205, 178, 67, 0, 0, 361, 0, 317, 351, 0, 318, 319, 0, 0, 0, 0, 0, 0, 354,
355, 0, 0, 0, 0, 357, 358, 356, 142, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 304, 0, 0, 359, 0,
383, 384, 0, 0, 0, 393, 0, 0, 143, 18, 8, 20, 8, 95, 97, 183, 182, 14, 0, 80, 92,
0, 0, 103, 104, 106, 0, 148, 111, 85, 119, 0, 0, 0, 0, 0, 0, 0, 270, 271, 272, 0,
0, 274, 0, 228, 229, 0, 0, 0, 0, 96, 234, 236, 232, 0, 162, 168, 0, 157, 164,
130, 0, 0, 0, 158, 132, 134, 133, 129, 150, 138, 137, 8, 48, 47, 0, 45, 0, 74, 0,
286, 0, 23, 287, 0, 0, 0, 0, 193, 295, 298, 0, 297, 0, 0, 0, 201, 100, 102, 0,
158, 0, 209, 0, 207, 156, 0, 0, 219, 202, 204, 175, 0, 0, 0, 184, 188, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 392, 399, 0, 398, 324, 0, 326, 328, 330, 332, 334, 335,
339, 340, 337, 338, 342, 343, 345, 346, 348, 349, 350, 303, 0, 381, 382, 377, 0,
379, 380, 0, 0, 0, 4, 6, 0, 322, 107, 98, 0, 0, 0, 112, 121, 120, 0, 0, 115, 0,
0, 0, 0, 0, 0, 0, 0, 269, 273, 0, 0, 0, 227, 0, 230, 0, 15, 301, 163, 0, 0, 166,
0, 0, 0, 159, 131, 156, 152, 151, 139, 7, 0, 0, 0, 79, 77, 294, 196, 0, 296, 192,
194, 288, 0, 285, 172, 171, 214, 0, 208, 211, 0, 158, 0, 210, 0, 360, 0, 0, 185,
0, 189, 387, 388, 0, 0, 0, 0, 0, 0, 0, 352, 395, 0, 0, 363, 378, 376, 396, 0, 0,
82, 105, 108, 81, 109, 122, 119, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 245, 275, 0,
0, 16, 156, 169, 167, 165, 0, 162, 158, 0, 0, 44, 46, 195, 299, 0, 213, 216, 0,
218, 217, 212, 0, 0, 186, 190, 0, 0, 320, 321, 0, 397, 0, 400, 316, 394, 0, 118,
117, 240, 0, 245, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 276, 0, 158, 160, 0,
153, 49, 50, 173, 215, 362, 0, 191, 0, 0, 385, 0, 0, 0, 251, 0, 0, 257, 0, 0, 0,
265, 0, 0, 0, 0, 0, 0, 0, 0, 244, 226, 154, 0, 156, 0, 364, 0, 386, 0, 241, 0, 0,
0, 242, 256, 258, 259, 0, 267, 266, 0, 261, 0, 0, 0, 0, 245, 246, 161, 158, 0, 0,
123, 0, 0, 243, 260, 268, 262, 263, 0, 0, 245, 247, 155, 187, 365, 251, 252, 264,
245, 248, 251, 253, 245, 249, 254, 250, 0, 0, 0}; static short Cyc_yydefgoto[
113u]={ 793, 39, 40, 41, 242, 42, 372, 43, 374, 44, 208, 45, 46, 61, 62, 438,
439, 47, 143, 48, 49, 272, 273, 50, 84, 51, 214, 215, 95, 96, 97, 216, 147, 384,
385, 386, 52, 53, 537, 538, 539, 54, 55, 56, 57, 120, 107, 434, 462, 463, 425,
566, 560, 421, 422, 285, 264, 149, 75, 76, 479, 377, 480, 481, 450, 451, 139,
144, 145, 464, 292, 245, 246, 247, 248, 249, 250, 251, 698, 747, 252, 253, 276,
277, 278, 453, 454, 455, 254, 255, 358, 181, 529, 182, 183, 184, 185, 186, 187,
188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 493, 494, 198, 199}; static
short Cyc_yypact[ 796u]={ 907, - -32768, - -32768, - -32768, - 63, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - 45, 173, 487, -
-32768, 173, - 18, - -32768, - -32768, - 12, - -32768, 9, - -32768, 71, 351, -
-32768, 44, 77, 92, - -32768, - -32768, 907, - -32768, 67, 390, - -32768, 296, -
45, - 45, - 45, - -32768, - -32768, 98, - -32768, 130, 2218, 137, 47, 39, -
-32768, 116, 124, 2278, - -32768, 134, - -32768, 182, - -32768, - -32768, -
-32768, 650, - -32768, 141, 23, 215, 650, 231, 218, 240, - 9, - -32768, - -32768,
4345, 35, 4345, - -32768, 262, 4453, 3870, 3870, - -32768, 907, 907, 907, 907, -
-32768, 269, 270, - -32768, 2155, 2278, 2278, 2278, 71, 4453, 71, 316, - -32768,
173, 197, 311, 1385, - -32768, 2218, 296, - -32768, 1157, 4345, 2734, - -32768,
137, - 45, - -32768, 907, 285, - -32768, 173, 346, 650, - -32768, - -32768, -
-32768, - -32768, 328, 3870, 368, 196, - 45, - 45, 224, - -32768, - 43, - -32768,
- -32768, - -32768, 325, - 27, - -32768, 317, 393, - -32768, 4083, 331, - -32768,
3870, 3941, 353, 366, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
370, 373, 391, 4154, 4154, - -32768, - -32768, - -32768, - -32768, 1746, 398,
4225, 4225, - -32768, 4225, - -32768, - -32768, 403, - -32768, - 48, 431, 389,
399, 395, 426, 101, 470, 429, 181, - -32768, 728, 409, 4225, 425, - -32768, -
-32768, 69, 440, - -32768, 451, - -32768, 455, - -32768, 351, 2805, 2218, -
-32768, - -32768, - -32768, - -32768, 427, 478, 4453, - -32768, 415, 482, 35, 71,
173, 495, - -32768, 506, 19, 523, 2335, 530, 508, 538, 541, 2876, 2335, 153,
2335, 2335, 15, 535, - -32768, - -32768, 546, - -32768, - -32768, 296, - -32768,
- -32768, - -32768, - -32768, 549, 1507, - -32768, - -32768, - -32768, 217, -
-32768, - -32768, - -32768, 552, 39, - -32768, 118, 551, 550, 220, 557, 5, -
-32768, 553, 89, 561, 16, 560, 566, 567, - -32768, - -32768, 569, 565, - -32768,
245, 545, 650, 26, - -32768, 573, 4453, 4453, 2095, 2947, 48, - -32768, 372, -
-32768, 118, - -32768, 4345, - -32768, 2040, - -32768, 103, - -32768, - -32768,
4453, 1746, - -32768, 4453, - -32768, - -32768, 2406, - -32768, 599, 3870, 1985,
652, 3870, 3870, 1746, - -32768, - -32768, 1507, 572, 439, 3870, - -32768, -
-32768, - -32768, - -32768, 4225, 3870, 4225, 4225, 4225, 4225, 4225, 4225, 4225,
4225, 4225, 4225, 4225, 4225, 4225, 4225, 4225, 4225, 4225, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, 3870, 3870, - -32768, 157, - -32768, - -32768, 3018, 160, 3870, -
-32768, 2652, 571, - -32768, - -32768, 907, - -32768, 907, - -32768, - -32768, -
-32768, - -32768, - -32768, 4453, - -32768, - -32768, 3870, 582, 585, - -32768,
577, 4453, - -32768, - -32768, 71, 544, 3870, 588, 3870, 3870, 661, 1629, 594, -
-32768, - -32768, - -32768, 248, 653, - -32768, 3089, - -32768, - -32768, 2335,
605, 2335, 1863, 2155, - -32768, - -32768, - -32768, 3870, 611, 616, 261, -
-32768, 596, - -32768, 614, 610, 4399, 550, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, 89, 907, - -32768, 612, 615, 613, 173, - -32768,
3870, - -32768, 650, - -32768, - -32768, 631, 3870, 650, 110, - -32768, - -32768,
- -32768, 621, 618, 620, 35, 630, - -32768, - -32768, - -32768, 622, 550, 624, -
-32768, 625, 372, 4285, 4345, 3160, - -32768, - -32768, 325, - -32768, 627, 629,
645, - -32768, - -32768, 177, 2805, 450, 633, 351, 636, 454, 637, 644, 651, 4012,
- -32768, - -32768, 654, 655, 431, 169, 389, 399, 395, 426, 101, 101, 470, 470,
470, 470, 429, 429, 181, 181, - -32768, - -32768, - -32768, - -32768, 657, -
-32768, - -32768, - -32768, 658, - -32768, - -32768, 106, 314, 4345, - -32768, -
-32768, 664, - -32768, - -32768, - -32768, 415, 3870, 665, - -32768, - -32768, -
-32768, 666, 173, 279, 456, 3870, 459, 464, 656, 3231, 3302, 288, - -32768, -
-32768, 670, 672, 668, - -32768, 646, - -32768, 2218, - -32768, - -32768, -
-32768, 4453, 35, - -32768, 673, 118, 667, - -32768, - -32768, 83, - -32768, -
-32768, - -32768, - -32768, 480, 669, 16, - -32768, - -32768, - -32768, - -32768,
663, - -32768, - -32768, - -32768, - -32768, 592, - -32768, 674, - -32768, -
-32768, 677, - -32768, - -32768, 678, 550, 7, - -32768, 671, 683, 687, 679, -
-32768, 2488, - -32768, - -32768, - -32768, 2218, 4453, 3941, 762, 683, 689,
2652, - -32768, - -32768, 3870, 3870, - -32768, - -32768, - -32768, - -32768,
2652, 330, - -32768, - -32768, - -32768, - -32768, - -32768, 186, 20, 544, 2335,
471, 691, 2335, 3870, 3373, 299, 3444, 309, 3515, 562, - -32768, 695, 705, -
-32768, 697, - -32768, - -32768, - -32768, 702, 611, 550, 694, 698, - -32768, -
-32768, - -32768, - -32768, 630, - -32768, - -32768, 715, - -32768, - -32768, -
-32768, 718, 3870, - -32768, - -32768, 2805, 719, - -32768, 599, 696, - -32768,
347, - -32768, - -32768, - -32768, 723, - -32768, - -32768, 772, 729, 562, -
-32768, 483, 2335, 499, 3586, 2335, 503, 3657, 3728, 338, 650, 721, 727, -
-32768, 2335, 550, 730, 4453, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, 249, - -32768, 732, 4453, - -32768, 2570, 4453, 2335, 568, 731, 734, -
-32768, 2335, 2335, 507, - -32768, 2335, 2335, 509, 2335, 511, 3799, - 21, 1507,
- -32768, - -32768, - -32768, 614, 697, 3870, - -32768, 733, - -32768, 513, -
-32768, 3870, 724, 740, - -32768, - -32768, - -32768, - -32768, 2335, - -32768,
- -32768, 2335, - -32768, 2335, 2335, 515, 3870, 1097, - -32768, - -32768, 550,
357, 739, - -32768, 253, 1507, - -32768, - -32768, - -32768, - -32768, - -32768,
2335, 312, 562, - -32768, - -32768, - -32768, - -32768, 1263, - -32768, - -32768,
1097, - -32768, 568, - -32768, 562, - -32768, - -32768, - -32768, 845, 846, -
-32768}; static short Cyc_yypgoto[ 113u]={ - -32768, 192, - -32768, 534, -
-32768, - -32768, - -32768, - -32768, - -32768, - 50, - 40, - 54, - -32768, 93,
- 47, 273, - -32768, 13, - 183, - 46, - -32768, - -32768, 410, - -32768, - 19, -
-32768, - 113, - -32768, - -32768, - -32768, 643, 638, 64, - -32768, - -32768,
320, - -32768, - -32768, - 8, - -32768, - -32768, - 22, - 37, - 109, - -32768, -
-32768, - 52, - -32768, - 99, - 112, - 388, 115, 204, 290, 563, - 390, - 85, -
288, 741, - -32768, - 194, - 148, - 347, - 257, - -32768, 408, - 131, - 59, - 98,
52, - 137, 62, - -32768, - -32768, - 34, - 238, - -32768, - -32768, - 634, - 382,
- -32768, - -32768, - 20, 578, - -32768, 274, - -32768, - -32768, 203, 31, -
-32768, - 136, - 362, - 145, 533, 532, 536, 531, 539, 289, 148, 301, 286, - 13,
649, - -32768, - -32768, - -32768, - -32768, - 272, - -32768, - -32768, 10};
static short Cyc_yytable[ 4560u]={ 113, 148, 77, 263, 111, 121, 310, 124, 118,
311, 58, 85, 415, 376, 112, 474, 262, 307, 266, 119, 114, 523, 140, 98, 449, 535,
536, 328, 59, 290, 563, 63, 78, 436, 79, 137, 389, 137, 300, 569, 137, 321, 295,
58, 113, 210, 211, 212, 111, 126, 58, 719, 60, 760, 130, 58, 244, 137, 113, 134,
243, 104, 256, 108, 209, 293, 58, 329, 588, 137, 137, 80, 118, 294, 429, 590,
663, 296, 257, 78, 577, 297, 489, 213, 78, 218, 580, 515, 761, 222, - 85, 258,
519, - 85, 136, 81, 136, 82, 83, 136, 58, 58, 58, 58, 409, 437, 15, 16, 17, 296,
394, 296, 128, 430, 457, 664, 136, 220, 180, 200, - 116, 395, 410, 58, 141, 299,
137, 778, 136, 136, 458, 142, 58, 105, 552, 274, 29, 78, 474, 99, 100, 101, 31,
786, 106, 138, 86, 138, 268, 33, 288, 790, 427, 467, 113, 792, 367, 58, 256, 289,
433, 324, 325, 467, 326, 90, 91, 217, 368, 137, 625, 336, 337, 475, 379, 476, 87,
105, 83, 290, 138, 261, 360, 369, 258, 136, 488, 29, 106, 300, 291, 88, 29, 31,
102, 387, 244, 103, 31, 301, 243, 390, 36, 37, 38, 298, 662, 418, 338, 339, 417,
582, 419, 269, 289, 122, 58, 420, 619, 29, 447, 448, 413, 432, 105, 31, 123, 58,
136, 286, 287, 391, 89, 125, 60, 106, 138, 473, 378, 115, 137, 137, 137, 127,
116, 471, 516, 528, 117, 520, 137, 405, 137, 517, 58, 406, 521, 137, 137, 484,
137, 452, 29, 119, 704, 676, 244, 527, 31, 707, 243, 137, 646, 417, 587, 533, 85,
601, 616, 217, - 180, 602, 201, 202, 203, 204, - 180, 603, - 85, 397, 283, 78,
78, 83, 344, 404, 284, 407, 408, 136, 136, 136, 345, 346, 83, 129, 528, 221, 58,
136, 58, 136, 528, 736, 270, 416, 136, 136, 261, 136, 466, 417, 132, 58, 426,
571, 288, 148, 449, 131, 136, 511, 512, 513, 137, 289, 280, 36, 37, 38, 378, 133,
137, 445, 113, 670, 549, 487, 546, 417, 460, 461, 417, 417, 492, 283, 594, 417,
739, 113, 138, 562, 782, 111, 670, 146, 137, 205, 138, 593, 138, 595, 534, 556,
206, 322, 779, 629, 647, 557, 137, 138, 58, 630, 58, 29, 640, 271, 570, 514, 492,
31, 417, 136, 94, 492, 528, 690, 33, 378, 788, 136, 223, 417, 219, 791, 29, 693,
669, 36, 37, 38, 31, 417, 620, 738, 417, 279, 621, 298, 471, 785, 137, 137, 578,
136, 622, 669, 289, 581, 36, 37, 38, 306, 296, 403, 731, 492, 679, 136, 29, 294,
417, 492, 217, 58, 31, 714, 558, 275, 274, 715, 217, 33, 78, 312, 652, 780, 670,
78, 672, 417, 606, 673, 36, 37, 38, 281, 313, 60, 282, 553, 314, 555, 468, 315,
711, 613, 137, 469, 678, 136, 136, 470, 503, 504, 505, 506, 92, 93, 361, 362,
363, 316, 58, 762, 302, 303, 334, 335, 323, 597, 113, 327, 29, 330, 256, 322,
331, 387, 31, 359, 378, 333, 137, 482, 651, 33, 486, 332, 322, 669, 645, 777,
383, 64, 380, 364, 36, 37, 38, 783, 496, 138, 701, 365, 366, 136, 340, 341, 370,
58, 491, 417, 787, 342, 343, 789, 628, 535, 536, 371, 113, 604, 417, 373, 111,
608, 417, 631, 417, 137, 633, 417, 525, 452, 526, 634, 417, 522, 653, 654, 209,
136, 684, 417, 65, 66, 381, 67, 68, 388, 31, 64, 696, 697, 720, 417, 138, 69,
745, 746, 70, 71, 392, 78, 540, 399, 542, 543, 72, 547, 722, 417, 73, 74, 726,
417, 393, 680, 752, 417, 755, 417, 757, 417, 767, 303, 775, 417, 136, 681, 682,
501, 502, 396, 764, 572, 509, 510, 64, 743, 398, 378, 29, 66, 400, 67, 68, 401,
31, 507, 508, 378, 411, 446, 677, 69, 412, 414, 70, 71, 378, 207, 423, 447, 448,
137, 72, 424, 428, 435, 73, 74, 440, 431, 441, 137, 442, 444, 137, 671, 443, 459,
328, 490, 732, 485, 524, 244, 530, 29, 66, 243, 67, 68, 532, 31, 64, 531, 541,
544, 548, 683, 69, 550, 686, 70, 71, 554, 559, 378, 447, 448, 561, 72, 78, 244,
565, 73, 74, 243, 564, 567, 573, 244, 136, 575, 574, 243, 579, 584, 585, 586,
283, 589, 136, 591, 244, 136, 598, 244, 243, 599, 600, 243, 605, 592, 607, 29,
66, 609, 67, 68, 632, 31, 378, 610, 637, 639, 721, 611, 69, 725, 644, 70, 71,
614, 635, 615, 617, 618, 735, 72, 623, 626, 627, 73, 74, 641, 642, 643, 655, 648,
650, 657, 666, 741, 659, 744, 660, 661, 612, 665, 750, 751, 667, 674, 753, 754,
685, 756, 675, 699, 700, 261, 702, 705, 713, 305, 717, 706, 308, 308, 347, 348,
349, 350, 351, 352, 353, 354, 355, 356, 771, 318, 319, 772, 708, 773, 774, 709,
712, 308, 308, 716, 308, 734, 718, 357, 733, 748, 749, 769, 737, 740, 766, 784,
687, 689, 770, 692, 781, 695, 308, 794, 795, 483, 656, 375, 576, 624, 763, 382,
649, 703, 265, 472, 583, 658, 456, 495, 497, 499, 0, 0, 0, 498, 0, 0, 710, 0,
500, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 724, 0, 0, 728,
730, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0, 759, 0, 0, 0, 0, 0, 0, 0, 765, 0, 21,
0, 22, 0, 768, 0, 0, 23, 0, 24, 25, 26, 27, 0, 0, 0, 0, 0, 0, 776, 0, 0, 0, 0,
28, 0, 0, 0, 0, 0, 0, 0, 0, 308, 0, 308, 308, 308, 308, 308, 308, 308, 308, 308,
308, 308, 308, 308, 308, 308, 308, 308, 29, 0, 0, 0, 0, 30, 31, 32, 0, 0, 0, 0,
0, 33, 0, 0, 34, 35, 0, 0, 0, 0, 0, 0, 36, 37, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0,
308, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 308, 0, 0, 0, 0, 0, 308, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 696, 697, 20, 150, 151, 224, 0, 225, 226,
227, 228, 229, 230, 231, 232, 21, 152, 22, 153, 233, 308, 154, 23, 234, 0, 0, 26,
27, 155, 156, 235, 236, 157, 158, 159, 160, 161, 162, 163, 28, 237, 164, 165, 0,
166, 167, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 308, 0, 0, 0,
0, 238, 168, 169, 170, 171, 30, 31, 32, 21, 239, 110, 0, 0, 172, 0, 0, 34, 241,
0, 26, 27, 0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 28, 0, 178, 179, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 259, 0, 0, 260, 0, 0, 0, 0,
30, 0, 32, 0, 261, 0, 308, 0, 0, 0, 0, 34, 35, 0, 308, 1, 2, 3, 109, 5, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 745, 746, 20, 150, 151, 224, 0, 225,
226, 227, 228, 229, 230, 231, 232, 21, 152, 22, 153, 233, 0, 154, 23, 234, 0, 0,
26, 27, 155, 156, 235, 236, 157, 158, 159, 160, 161, 162, 163, 28, 237, 164, 165,
0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 238,
168, 169, 170, 171, 30, 31, 32, 0, 239, 110, 0, 0, 172, 0, 0, 34, 241, 0, 0, 0,
0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 0, 0, 178, 179, 1, 2, 3, 109, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 150, 151, 224, 0, 225,
226, 227, 228, 229, 230, 231, 232, 21, 152, 22, 153, 233, 0, 154, 23, 234, 0, 0,
26, 27, 155, 156, 235, 236, 157, 158, 159, 160, 161, 162, 163, 28, 237, 164, 165,
0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 238,
168, 169, 170, 171, 30, 31, 32, 0, 239, 110, 240, 0, 172, 0, 0, 34, 241, 0, 0, 0,
0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 0, 0, 178, 179, 1, 2, 3, 109, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 150, 151, 224, 0, 225,
226, 227, 228, 229, 230, 231, 232, 21, 152, 22, 153, 233, 0, 154, 23, 234, 0, 0,
26, 27, 155, 156, 235, 236, 157, 158, 159, 160, 161, 162, 163, 28, 237, 164, 165,
0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 238,
168, 169, 170, 171, 30, 31, 32, 0, 239, 110, 0, 0, 172, 0, 0, 34, 241, 0, 0, 0,
0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 0, 0, 178, 179, 1, 2, 3, 109, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 150, 151, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 21, 152, 22, 153, 0, 0, 154, 23, 0, 0, 0, 26, 27, 155, 156, 0, 0,
157, 158, 159, 160, 161, 162, 163, 28, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 30, 31,
32, 0, 545, 0, 0, 0, 172, 0, 0, 34, 241, 0, 0, 0, 0, 0, 0, 174, 0, 0, 175, 176,
177, 0, 0, 0, 0, 178, 179, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0,
0, 0, 150, 151, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 152, 0, 153, 0, 0, 154, 0, 0,
0, 0, 26, 27, 155, 156, 0, 0, 157, 158, 159, 160, 161, 162, 163, 28, 0, 164, 165,
0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29,
168, 169, 170, 171, 30, 31, 32, 0, 0, 320, 0, 0, 172, 0, 0, 34, 241, 0, 0, 0, 0,
0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 0, 0, 178, 179, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 0, 0, 0, 150, 151, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 152,
0, 153, 0, 0, 154, 0, 0, 0, 0, 26, 27, 155, 156, 0, 0, 157, 158, 159, 160, 161,
162, 163, 28, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 30, 31, 32, 0, 0, 0, 0, 0, 172, 0,
0, 34, 241, 0, 0, 0, 0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 0, 0, 178, 179, 1,
2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 26, 27, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 0, 21, 0, 30, 31, 32, 0, 0, 0, 0,
0, 33, 26, 27, 34, 35, 0, 0, 0, 0, 0, 0, 36, 37, 38, 28, 0, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 259, 0, 0, 29, 0, 0,
21, 0, 30, 31, 32, 0, 261, 0, 0, 0, 298, 26, 27, 34, 35, 0, 0, 0, 0, 289, 0, 36,
37, 38, 28, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 0, 0, 20, 0, 259, 0, 0, 0, 0, 0, 0, 0, 30, 0, 32, 21, 261, 22, 0, 0, 288, 0,
23, 34, 35, 0, 26, 27, 0, 289, 0, 36, 37, 38, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 1,
2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0,
0, 0, 0, 0, 30, 0, 32, 0, 0, 110, 21, 207, 22, 0, 0, 34, 35, 23, 0, 0, 0, 26, 27,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 32, 21,
0, 110, 0, 0, 0, 0, 23, 34, 35, 0, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150, 151, 224, 0,
225, 226, 227, 228, 229, 230, 231, 232, 30, 152, 32, 153, 233, 0, 154, 0, 234, 0,
0, 34, 35, 155, 156, 235, 236, 157, 158, 159, 160, 161, 162, 163, 0, 237, 164,
165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
238, 168, 169, 170, 171, 0, 31, 150, 151, 239, 110, 0, 0, 172, 477, 0, 0, 173, 0,
0, 152, 0, 153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157,
158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 0, 0, 0,
309, 478, 0, 172, 0, 0, 0, 173, 150, 151, 0, 447, 448, 0, 174, 0, 0, 175, 176,
177, 0, 152, 0, 153, 178, 179, 154, 0, 0, 0, 0, 0, 0, 155, 156, 0, 0, 157, 158,
159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 0, 0, 0, 309,
668, 0, 172, 0, 0, 0, 173, 150, 151, 0, 447, 448, 0, 174, 0, 0, 175, 176, 177, 0,
152, 0, 153, 178, 179, 154, 0, 0, 0, 0, 0, 0, 155, 156, 0, 0, 157, 158, 159, 160,
161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 0, 0, 0, 309, 742, 0,
172, 0, 0, 0, 173, 150, 151, 0, 447, 448, 0, 174, 0, 0, 175, 176, 177, 0, 152, 0,
153, 178, 179, 154, 0, 0, 0, 0, 0, 0, 155, 156, 0, 0, 157, 158, 159, 160, 161,
162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 0, 0, 0, 309, 0, 0, 172, 0,
0, 0, 173, 150, 151, 0, 447, 448, 0, 174, 0, 0, 175, 176, 177, 0, 152, 0, 153,
178, 179, 154, 0, 0, 0, 0, 0, 0, 155, 156, 0, 0, 157, 158, 159, 160, 161, 162,
163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 0, 0, 0, 0, 172, 0, 0, 0,
173, 0, 0, 152, 0, 153, 267, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178,
179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31,
150, 151, 0, 309, 0, 0, 172, 0, 0, 0, 173, 0, 0, 152, 0, 153, 0, 174, 154, 0,
175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0,
164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 402, 0, 0, 0, 172, 0, 0, 0, 173,
0, 0, 152, 0, 153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157,
158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150,
151, 0, 0, 0, 0, 172, 0, 0, 0, 173, 0, 0, 152, 0, 153, 465, 174, 154, 0, 175,
176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 164,
165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
29, 168, 169, 170, 171, 0, 31, 150, 151, 0, 0, 0, 0, 172, 518, 0, 0, 173, 0, 0,
152, 0, 153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158,
159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 0,
0, 0, 0, 172, 551, 0, 0, 173, 0, 0, 152, 0, 153, 0, 174, 154, 0, 175, 176, 177,
0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0,
166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168,
169, 170, 171, 0, 31, 150, 151, 0, 0, 0, 0, 172, 0, 0, 0, 173, 0, 0, 152, 0, 153,
596, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160,
161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 636, 0, 0, 0,
172, 0, 0, 0, 173, 0, 0, 152, 0, 153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155,
156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170,
171, 0, 31, 150, 151, 638, 0, 0, 0, 172, 0, 0, 0, 173, 0, 0, 152, 0, 153, 0, 174,
154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162,
163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 0, 0, 0, 0, 172, 688, 0,
0, 173, 0, 0, 152, 0, 153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178,
179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31,
150, 151, 0, 0, 0, 0, 172, 691, 0, 0, 173, 0, 0, 152, 0, 153, 0, 174, 154, 0,
175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0,
164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 694, 0, 0, 0, 172, 0, 0, 0, 173,
0, 0, 152, 0, 153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157,
158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150,
151, 0, 0, 0, 0, 172, 723, 0, 0, 173, 0, 0, 152, 0, 153, 0, 174, 154, 0, 175,
176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 164,
165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
29, 168, 169, 170, 171, 0, 31, 150, 151, 0, 0, 0, 0, 172, 727, 0, 0, 173, 0, 0,
152, 0, 153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158,
159, 160, 161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 0,
0, 0, 0, 172, 729, 0, 0, 173, 0, 0, 152, 0, 153, 0, 174, 154, 0, 175, 176, 177,
0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 164, 165, 0,
166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168,
169, 170, 171, 0, 31, 150, 151, 0, 0, 0, 0, 172, 758, 0, 0, 173, 0, 0, 152, 0,
153, 0, 174, 154, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160,
161, 162, 163, 0, 0, 164, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 0, 0, 0, 0,
172, 0, 0, 0, 173, 0, 0, 152, 0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 155, 156,
178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 0, 165, 0, 166, 167, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0,
31, 150, 151, 0, 309, 0, 0, 172, 0, 0, 0, 173, 0, 0, 152, 0, 0, 0, 174, 0, 0,
175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0,
0, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 0, 612, 0, 0, 172, 0, 0, 0, 173, 0,
0, 152, 0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158,
159, 160, 161, 162, 163, 0, 0, 0, 165, 0, 166, 167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 150, 151, 0, 0,
0, 0, 304, 0, 0, 0, 173, 0, 0, 152, 0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 155,
156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0, 0, 0, 165, 0, 166, 167, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 168, 169, 170,
171, 0, 31, 150, 151, 0, 0, 0, 0, 317, 0, 0, 0, 173, 0, 0, 152, 0, 0, 0, 174, 0,
0, 175, 176, 177, 0, 0, 155, 156, 178, 179, 157, 158, 159, 160, 161, 162, 163, 0,
0, 0, 165, 0, 166, 167, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0,
0, 0, 0, 0, 0, 29, 168, 169, 170, 171, 0, 31, 0, 21, 0, 0, 0, 0, 172, 0, 0, 0,
173, 0, 26, 27, 0, 0, 0, 174, 0, 0, 175, 176, 177, 0, 0, 28, 0, 178, 179, 0, 0,
0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 259, 0, 0, 0,
0, 0, 0, 0, 30, 0, 32, 21, 261, 0, 0, 0, 0, 0, 0, 34, 35, 0, 26, 27, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 28, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 30, 0, 32, 0, 0, 135, 0, 0,
26, 27, 0, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 28, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 568, 0, 0, 0, 0, 21, 0, 0, 30,
0, 32, 0, 0, 0, 0, 0, 26, 27, 0, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 34, 35}; static short Cyc_yycheck[ 4560u]={
54, 86, 22, 115, 54, 57, 154, 61, 55, 154, 0, 33, 250, 207, 54, 303, 115, 153,
116, 56, 54, 368, 81, 45, 281, 5, 6, 75, 91, 138, 420, 21, 22, 17, 24, 81, 219,
83, 147, 427, 86, 172, 69, 33, 98, 99, 100, 101, 98, 69, 40, 685, 97, 74, 74, 45,
110, 103, 112, 78, 110, 51, 112, 53, 98, 108, 56, 115, 458, 115, 116, 89, 119,
116, 69, 463, 69, 104, 112, 69, 442, 108, 320, 102, 74, 104, 448, 359, 109, 108,
99, 113, 364, 102, 81, 107, 83, 88, 107, 86, 90, 91, 92, 93, 89, 89, 17, 18, 19,
104, 91, 104, 89, 108, 88, 108, 103, 107, 87, 88, 100, 102, 107, 113, 89, 147,
172, 761, 115, 116, 104, 96, 122, 94, 406, 125, 89, 127, 426, 46, 47, 48, 95,
777, 105, 81, 102, 83, 117, 102, 102, 785, 264, 290, 208, 789, 87, 147, 208, 111,
269, 174, 175, 300, 177, 98, 99, 103, 99, 215, 532, 70, 71, 304, 208, 306, 99,
94, 107, 288, 116, 98, 195, 114, 206, 172, 317, 89, 105, 298, 138, 99, 89, 95,
96, 217, 250, 99, 95, 147, 250, 220, 113, 114, 115, 102, 594, 259, 107, 108, 104,
101, 94, 120, 111, 99, 206, 99, 112, 89, 110, 111, 244, 269, 94, 95, 102, 217,
215, 136, 137, 221, 40, 99, 97, 105, 172, 296, 207, 102, 286, 287, 288, 102, 107,
292, 89, 383, 111, 89, 296, 98, 298, 96, 244, 102, 96, 303, 304, 313, 306, 281,
89, 300, 652, 612, 320, 380, 95, 659, 320, 317, 560, 104, 457, 388, 298, 100,
109, 215, 98, 104, 90, 91, 92, 93, 104, 481, 102, 227, 94, 281, 282, 107, 113,
233, 100, 235, 236, 286, 287, 288, 121, 122, 107, 90, 442, 110, 298, 296, 300,
298, 448, 701, 122, 98, 303, 304, 98, 306, 289, 104, 104, 313, 104, 434, 102,
412, 585, 98, 317, 344, 345, 346, 380, 111, 133, 113, 114, 115, 309, 101, 388,
98, 398, 602, 98, 316, 398, 104, 286, 287, 104, 104, 323, 94, 468, 104, 109, 413,
296, 100, 109, 413, 621, 103, 412, 98, 304, 468, 306, 469, 391, 413, 104, 172,
764, 98, 561, 413, 426, 317, 372, 104, 374, 89, 98, 102, 434, 358, 359, 95, 104,
380, 98, 364, 532, 98, 102, 368, 782, 388, 91, 104, 88, 787, 89, 98, 602, 113,
114, 115, 95, 104, 100, 703, 104, 89, 104, 102, 467, 109, 468, 469, 444, 412,
524, 621, 111, 449, 113, 114, 115, 102, 104, 232, 98, 406, 108, 426, 89, 116,
104, 412, 380, 435, 95, 100, 417, 103, 440, 104, 388, 102, 444, 102, 568, 100,
715, 449, 608, 104, 484, 608, 113, 114, 115, 99, 102, 97, 102, 409, 102, 411,
102, 102, 670, 490, 524, 107, 616, 468, 469, 111, 336, 337, 338, 339, 98, 99, 65,
66, 67, 102, 484, 733, 103, 104, 72, 73, 102, 470, 556, 100, 89, 74, 556, 304,
119, 531, 95, 102, 481, 118, 560, 312, 568, 102, 315, 120, 317, 715, 556, 761,
109, 38, 99, 102, 113, 114, 115, 769, 329, 469, 646, 110, 111, 524, 68, 69, 100,
531, 103, 104, 782, 116, 117, 785, 538, 5, 6, 100, 606, 103, 104, 100, 606, 103,
104, 103, 104, 607, 103, 104, 372, 585, 374, 103, 104, 366, 90, 91, 606, 560,
103, 104, 89, 90, 100, 92, 93, 99, 95, 38, 22, 23, 103, 104, 524, 102, 22, 23,
105, 106, 99, 585, 393, 89, 395, 396, 113, 398, 103, 104, 117, 118, 103, 104,
102, 628, 103, 104, 103, 104, 103, 104, 103, 104, 103, 104, 607, 629, 630, 334,
335, 102, 738, 435, 342, 343, 38, 716, 102, 602, 89, 90, 98, 92, 93, 98, 95, 340,
341, 612, 109, 100, 615, 102, 102, 100, 105, 106, 621, 101, 103, 110, 111, 703,
113, 109, 103, 100, 117, 118, 104, 112, 100, 713, 101, 104, 716, 607, 103, 100,
75, 103, 696, 25, 107, 733, 98, 89, 90, 733, 92, 93, 109, 95, 38, 104, 102, 30,
98, 631, 102, 42, 634, 105, 106, 94, 89, 670, 110, 111, 88, 113, 696, 761, 94,
117, 118, 761, 116, 103, 102, 769, 703, 104, 103, 769, 89, 100, 104, 103, 94,
103, 713, 103, 782, 716, 103, 785, 782, 104, 89, 785, 103, 112, 102, 89, 90, 104,
92, 93, 541, 95, 715, 103, 545, 546, 688, 100, 102, 691, 108, 105, 106, 103, 102,
104, 103, 103, 700, 113, 100, 100, 100, 117, 118, 99, 98, 103, 103, 100, 107,
112, 89, 713, 104, 717, 103, 103, 99, 112, 722, 723, 107, 25, 726, 727, 99, 729,
103, 98, 89, 98, 94, 103, 102, 150, 28, 103, 153, 154, 76, 77, 78, 79, 80, 81,
82, 83, 84, 85, 752, 166, 167, 755, 103, 757, 758, 103, 103, 174, 175, 102, 177,
100, 99, 101, 109, 100, 98, 109, 104, 103, 103, 775, 635, 636, 100, 638, 103,
640, 195, 0, 0, 313, 575, 206, 440, 531, 737, 215, 564, 651, 115, 294, 450, 585,
282, 328, 330, 332, - 1, - 1, - 1, 331, - 1, - 1, 667, - 1, 333, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 690, - 1, - 1, 693, 694, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, 24, - 1, - 1, 731, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 739, - 1, 37, - 1,
39, - 1, 745, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1,
760, - 1, - 1, - 1, - 1, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 328, - 1,
330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345,
346, 89, - 1, - 1, - 1, - 1, 94, 95, 96, - 1, - 1, - 1, - 1, - 1, 102, - 1, - 1,
105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, 114, 115, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 383, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 442,
- 1, - 1, - 1, - 1, - 1, 448, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34,
35, 36, 37, 38, 39, 40, 41, 490, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53,
54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, - 1, 66, 67, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 532, - 1, - 1, - 1, - 1, 89, 90, 91,
92, 93, 94, 95, 96, 37, 98, 99, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, 48, 49,
- 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 61, - 1, 123, 124, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 86, - 1, - 1, 89, - 1, - 1, - 1, - 1, 94, - 1, 96, - 1, 98, -
1, 608, - 1, - 1, - 1, - 1, 105, 106, - 1, 616, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32,
33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51,
52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, - 1, 66, 67, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98, 99, - 1, - 1, 102, - 1, - 1,
105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1,
- 1, - 1, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
58, 59, 60, 61, 62, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92,
93, 94, 95, 96, - 1, 98, 99, 100, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, -
1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26,
27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, -
1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98, 99, -
1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1,
116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 37, 38, 39, 40, - 1, - 1, 43, 44, - 1, - 1, - 1, 48, 49,
50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60, 61, - 1, 63, 64, - 1, 66, 67, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98, - 1, - 1, - 1, 102,
- 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118,
- 1, - 1, - 1, - 1, 123, 124, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37,
38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55,
56, 57, 58, 59, 60, 61, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89,
90, 91, 92, 93, 94, 95, 96, - 1, - 1, 99, - 1, - 1, 102, - 1, - 1, 105, 106, - 1,
- 1, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, - 1, 123,
124, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43,
- 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60, 61, -
1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96,
- 1, - 1, - 1, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1,
113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1, 44, -
1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61,
- 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1, - 1, 37, - 1, 94, 95, 96, - 1, -
1, - 1, - 1, - 1, 102, 48, 49, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, 114,
115, 61, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, 89, - 1, - 1, 37, - 1, 94, 95, 96,
- 1, 98, - 1, - 1, - 1, 102, 48, 49, 105, 106, - 1, - 1, - 1, - 1, 111, - 1, 113,
114, 115, 61, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, - 1, 86, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96,
37, 98, 39, - 1, - 1, 102, - 1, 44, 105, 106, - 1, 48, 49, - 1, 111, - 1, 113,
114, 115, - 1, - 1, - 1, - 1, - 1, 61, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
- 1, - 1, 94, - 1, 96, - 1, - 1, 99, 37, 101, 39, - 1, - 1, 105, 106, 44, - 1, -
1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, - 1,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, 37, - 1, 99, - 1,
- 1, - 1, - 1, 44, 105, 106, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34,
35, 36, 94, 38, 96, 40, 41, - 1, 43, - 1, 45, - 1, - 1, 105, 106, 50, 51, 52, 53,
54, 55, 56, 57, 58, 59, 60, - 1, 62, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, 98, 99, - 1, - 1, 102, 32, - 1, - 1, 106,
- 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123,
124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 99, 100, - 1, 102, - 1, - 1,
- 1, 106, 25, 26, - 1, 110, 111, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1,
40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, 55, 56, 57,
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
- 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, 110, 111, - 1, 113, - 1, - 1,
116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50,
51, - 1, - 1, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102,
- 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, 112, 113, 43, - 1, 116, 117, 118, - 1,
- 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, 99, - 1, - 1,
102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118,
- 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, 98, - 1, -
1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116,
117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63,
64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1,
- 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, 112, 113, 43, - 1,
116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1,
63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26,
- 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43,
- 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1,
- 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95,
25, 26, - 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1,
113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59,
60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93,
- 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1,
40, 112, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57,
58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91,
92, 93, - 1, 95, 25, 26, 98, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1,
38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54,
55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
89, 90, 91, 92, 93, - 1, 95, 25, 26, 98, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106,
- 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123,
124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, 103, - 1,
- 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1, - 1, 50,
51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102,
103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118, - 1,
- 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, 63, 64, - 1, 66, 67,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, 98, - 1, - 1, - 1,
102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43, - 1, 116, 117, 118,
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
- 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1, 113, 43,
- 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1,
- 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95,
25, 26, - 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106, - 1, - 1, 38, - 1, 40, - 1,
113, 43, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59,
60, - 1, - 1, 63, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93,
- 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1,
- 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56,
57, 58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90,
91, 92, 93, - 1, 95, 25, 26, - 1, 99, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, -
1, 38, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 50, 51, 123, 124,
54, 55, 56, 57, 58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, 99, - 1, - 1, 102, - 1, - 1, - 1,
106, - 1, - 1, 38, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, 50, 51,
123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1, 102, - 1,
- 1, - 1, 106, - 1, - 1, 38, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, -
1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, - 1, 64, - 1, 66, 67,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, 25, 26, - 1, - 1, - 1, - 1,
102, - 1, - 1, - 1, 106, - 1, - 1, 38, - 1, - 1, - 1, 113, - 1, - 1, 116, 117,
118, - 1, - 1, 50, 51, 123, 124, 54, 55, 56, 57, 58, 59, 60, - 1, - 1, - 1, 64,
- 1, 66, 67, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, 37, - 1, - 1, - 1, - 1,
102, - 1, - 1, - 1, 106, - 1, 48, 49, - 1, - 1, - 1, 113, - 1, - 1, 116, 117,
118, - 1, - 1, 61, - 1, 123, 124, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 94, - 1, 96, 37, 98, - 1, - 1, - 1, - 1, - 1, - 1, 105, 106, - 1, 48, 49, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 37, - 1, - 1, 94, - 1, 96, - 1, - 1, 99, - 1, - 1, 48, 49,
- 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
86, - 1, - 1, - 1, - 1, 37, - 1, - 1, 94, - 1, 96, - 1, - 1, - 1, - 1, - 1, 48,
49, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 105, 106}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tagged_ptr2{ short* curr; short* base; short*
last_plus_one; } ; struct _tagged_ptr3{ void** curr; void** base; void**
last_plus_one; } ; struct _tagged_ptr4{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype*
base; struct Cyc_Yyltype* last_plus_one; } ; struct _tuple19{ void* f1; void* f2;
} ; struct _tuple20{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int
Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int
yyssp_offset; struct _tagged_ptr2 yyss=({ short* _temp4315=({ unsigned int
_temp4312= 200u; short* _temp4313=( short*) GC_malloc_atomic( sizeof( short) *
_temp4312);{ unsigned int _temp4314= _temp4312; unsigned int i; for( i= 0; i <
_temp4314; i ++){ _temp4313[ i]= 0;}}; _temp4313;}); struct _tagged_ptr2
_temp4316; _temp4316.curr= _temp4315; _temp4316.base= _temp4315; _temp4316.last_plus_one=
_temp4315 + 200; _temp4316;}); int yyvsp_offset; struct _tagged_ptr3 yyvs=({
void** _temp4310=({ unsigned int _temp4307= 200u; void** _temp4308=( void**)
GC_malloc( sizeof( void*) * _temp4307);{ unsigned int _temp4309= _temp4307;
unsigned int i; for( i= 0; i < _temp4309; i ++){ _temp4308[ i]= Cyc_yylval;}};
_temp4308;}); struct _tagged_ptr3 _temp4311; _temp4311.curr= _temp4310;
_temp4311.base= _temp4310; _temp4311.last_plus_one= _temp4310 + 200; _temp4311;});
int yylsp_offset; struct _tagged_ptr4 yyls=({ struct Cyc_Yyltype* _temp4305=({
unsigned int _temp4302= 200u; struct Cyc_Yyltype* _temp4303=( struct Cyc_Yyltype*)
GC_malloc( sizeof( struct Cyc_Yyltype) * _temp4302);{ unsigned int _temp4304=
_temp4302; unsigned int i; for( i= 0; i < _temp4304; i ++){ _temp4303[ i]= Cyc_yynewloc();}};
_temp4303;}); struct _tagged_ptr4 _temp4306; _temp4306.curr= _temp4305;
_temp4306.base= _temp4305; _temp4306.last_plus_one= _temp4305 + 200; _temp4306;});
int yystacksize= 200; void* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:*(( short*(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), ++ yyssp_offset)=(
short) yystate; if( yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){
Cyc_yyerror(({ unsigned char* _temp2285=( unsigned char*)"parser stack overflow";
struct _tagged_string _temp2286; _temp2286.curr= _temp2285; _temp2286.base=
_temp2285; _temp2286.last_plus_one= _temp2285 + 22; _temp2286;}));( void) _throw((
void*) Cyc_Yystack_overflow);} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr2 yyss1=({ unsigned int _temp2295=(
unsigned int) yystacksize; short* _temp2296=( short*) GC_malloc_atomic( sizeof(
short) * _temp2295); struct _tagged_ptr2 _temp2298={ _temp2296, _temp2296,
_temp2296 + _temp2295};{ unsigned int _temp2297= _temp2295; unsigned int i; for(
i= 0; i < _temp2297; i ++){ _temp2296[ i]= 0;}}; _temp2298;}); struct
_tagged_ptr3 yyvs1=({ unsigned int _temp2291=( unsigned int) yystacksize; void**
_temp2292=( void**) GC_malloc( sizeof( void*) * _temp2291); struct _tagged_ptr3
_temp2294={ _temp2292, _temp2292, _temp2292 + _temp2291};{ unsigned int
_temp2293= _temp2291; unsigned int i; for( i= 0; i < _temp2293; i ++){ _temp2292[
i]= Cyc_yylval;}}; _temp2294;}); struct _tagged_ptr4 yyls1=({ unsigned int
_temp2287=( unsigned int) yystacksize; struct Cyc_Yyltype* _temp2288=( struct
Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) * _temp2287); struct
_tagged_ptr4 _temp2290={ _temp2288, _temp2288, _temp2288 + _temp2287};{
unsigned int _temp2289= _temp2287; unsigned int i; for( i= 0; i < _temp2289; i
++){ _temp2288[ i]= Cyc_yynewloc();}}; _temp2290;});{ int i= 0; for( 0; i <=
yyssp_offset; i ++){*(( short*(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss1, sizeof( short), i)=*(( short*(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyss, sizeof( short), i);*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs1, sizeof( void*), i)=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), i);*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls1, sizeof( struct
Cyc_Yyltype), i)=*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int,
unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), i);}}
yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 796u, yystate)]; if( yyn == - 32768){ goto
yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <=
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
352: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 353u, Cyc_yychar)]:
238;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4559)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4560u, yyn)] != yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4560u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrlab;}} if( yyn == 795){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar=
- 2;}*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), ++ yylsp_offset)=
Cyc_yylloc; if( yyerrstatus != 0){ yyerrstatus --;} yystate= yyn; goto
yynewstate; yydefault: yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull(
796u, yystate)]; if( yyn == 0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 407u, yyn)]; if( yylen > 0){ yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*),( yyvsp_offset + 1) - yylen);} switch( yyn){ case 1:
_LL2299: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
break; case 2: _LL2300: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2302=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2302[ 0]=({ struct Cyc_DeclList_tok_struct _temp2303; _temp2303.tag= Cyc_DeclList_tok;
_temp2303.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2303;}); _temp2302;}); break; case 3:
_LL2301: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2305=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2305[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2306; _temp2306.tag= Cyc_DeclList_tok; _temp2306.f1=({ struct Cyc_List_List*
_temp2307=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2307->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2308=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2308->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2309=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2309[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2310; _temp2310.tag= Cyc_Absyn_Using_d; _temp2310.f1=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2310.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2310;}); _temp2309;})); _temp2308->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2308;}); _temp2307->tl= 0; _temp2307;}); _temp2306;}); _temp2305;}); Cyc_Lex_leave_using();
break; case 4: _LL2304: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2312=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2312[ 0]=({ struct Cyc_DeclList_tok_struct _temp2313; _temp2313.tag= Cyc_DeclList_tok;
_temp2313.f1=({ struct Cyc_List_List* _temp2314=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2314->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2315=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2315->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2316=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2316[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2317; _temp2317.tag= Cyc_Absyn_Using_d;
_temp2317.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2317.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2317;}); _temp2316;})); _temp2315->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2315;}); _temp2314->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2314;});
_temp2313;}); _temp2312;}); break; case 5: _LL2311: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2319=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2319[ 0]=({ struct Cyc_DeclList_tok_struct _temp2320; _temp2320.tag= Cyc_DeclList_tok;
_temp2320.f1=({ struct Cyc_List_List* _temp2321=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2321->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2322=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2322->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2323=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2323[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2324; _temp2324.tag=
Cyc_Absyn_Namespace_d; _temp2324.f1=({ struct _tagged_string* _temp2325=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2325[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2325;}); _temp2324.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2324;}); _temp2323;})); _temp2322->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2322;}); _temp2321->tl= 0; _temp2321;}); _temp2320;}); _temp2319;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2318: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2327=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2327[ 0]=({ struct Cyc_DeclList_tok_struct _temp2328; _temp2328.tag= Cyc_DeclList_tok;
_temp2328.f1=({ struct Cyc_List_List* _temp2329=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2329->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2330=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2330->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2331=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2331[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2332; _temp2332.tag=
Cyc_Absyn_Namespace_d; _temp2332.f1=({ struct _tagged_string* _temp2333=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2333[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp2333;}); _temp2332.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2332;}); _temp2331;})); _temp2330->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2330;}); _temp2329->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2329;});
_temp2328;}); _temp2327;}); break; case 7: _LL2326: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),({
unsigned char* _temp2335=( unsigned char*)"C"; struct _tagged_string _temp2336;
_temp2336.curr= _temp2335; _temp2336.base= _temp2335; _temp2336.last_plus_one=
_temp2335 + 2; _temp2336;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp2337=(
unsigned char*)"only extern \"C\" { ... } is allowed"; struct _tagged_string
_temp2338; _temp2338.curr= _temp2337; _temp2338.base= _temp2337; _temp2338.last_plus_one=
_temp2337 + 35; _temp2338;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line));} yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp2339=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2339[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2340; _temp2340.tag= Cyc_DeclList_tok; _temp2340.f1=({ struct Cyc_List_List*
_temp2341=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2341->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2342=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2342->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2343=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2343[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2344; _temp2344.tag= Cyc_Absyn_ExternC_d;
_temp2344.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2344;}); _temp2343;})); _temp2342->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp2342;}); _temp2341->tl= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2341;}); _temp2340;}); _temp2339;}); break;
case 8: _LL2334: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2346=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2346[ 0]=({ struct Cyc_DeclList_tok_struct _temp2347; _temp2347.tag= Cyc_DeclList_tok;
_temp2347.f1= 0; _temp2347;}); _temp2346;}); break; case 9: _LL2345: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2349=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2349[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2350; _temp2350.tag= Cyc_DeclList_tok; _temp2350.f1=({ struct Cyc_List_List*
_temp2351=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2351->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2352=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2352[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2353; _temp2353.tag= Cyc_Absyn_Fn_d;
_temp2353.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2353;}); _temp2352;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2351->tl= 0; _temp2351;}); _temp2350;}); _temp2349;}); break; case 10:
_LL2348: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 11:
_LL2354: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2356=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2356[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2357; _temp2357.tag= Cyc_FnDecl_tok; _temp2357.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2357;}); _temp2356;}); break; case 12: _LL2355:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2359=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2359[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2360; _temp2360.tag= Cyc_FnDecl_tok; _temp2360.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2361=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2361->v=( void*) Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2361;}), Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2360;}); _temp2359;}); break; case 13: _LL2358: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2363=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2363[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2364; _temp2364.tag= Cyc_FnDecl_tok;
_temp2364.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2364;}); _temp2363;}); break; case 14: _LL2362: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2366=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2366[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2367; _temp2367.tag= Cyc_FnDecl_tok;
_temp2367.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2368=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2368->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2368;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2367;});
_temp2366;}); break; case 15: _LL2365: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2370=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2370[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2371; _temp2371.tag= Cyc_FnDecl_tok;
_temp2371.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2372=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2372->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2372;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2371;});
_temp2370;}); break; case 16: _LL2369: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2374=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2374[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2375; _temp2375.tag= Cyc_FnDecl_tok;
_temp2375.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2376=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2376->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2376;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2375;});
_temp2374;}); break; case 17: _LL2373: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 18: _LL2377: Cyc_Lex_leave_using();
break; case 19: _LL2378: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2380=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2380[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2380;})); yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 20: _LL2379: Cyc_Lex_leave_namespace(); break; case
21: _LL2381: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2383=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2383[ 0]=({ struct Cyc_DeclList_tok_struct _temp2384; _temp2384.tag= Cyc_DeclList_tok;
_temp2384.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp2384;}); _temp2383;}); break; case 22: _LL2382: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2386=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2386[ 0]=({ struct Cyc_DeclList_tok_struct _temp2387; _temp2387.tag= Cyc_DeclList_tok;
_temp2387.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_InitDeclList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2387;}); _temp2386;}); break; case 23: _LL2385: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2389=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2389[ 0]=({ struct Cyc_DeclList_tok_struct _temp2390; _temp2390.tag= Cyc_DeclList_tok;
_temp2390.f1=({ struct Cyc_List_List* _temp2391=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2391->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
0, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2391->tl= 0; _temp2391;}); _temp2390;});
_temp2389;}); break; case 24: _LL2388: { struct Cyc_List_List* _temp2393= 0;
goto _LL2394; _LL2394:{ struct Cyc_List_List* _temp2395= Cyc_yyget_IdList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); goto
_LL2396; _LL2396: for( 0; _temp2395 != 0; _temp2395=(( struct Cyc_List_List*)
_check_null( _temp2395))->tl){ struct _tagged_string* _temp2397=( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( _temp2395))->hd; goto
_LL2398; _LL2398: { struct _tuple1* _temp2402=({ struct _tuple1* _temp2399=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2399->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp2400=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp2400[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp2401; _temp2401.tag= Cyc_Absyn_Rel_n; _temp2401.f1= 0; _temp2401;});
_temp2400;}); _temp2399->f2= _temp2397; _temp2399;}); goto _LL2403; _LL2403: {
struct Cyc_Absyn_Vardecl* _temp2404= Cyc_Absyn_new_vardecl( _temp2402, Cyc_Absyn_wildtyp(
0), 0); goto _LL2405; _LL2405: _temp2393=({ struct Cyc_List_List* _temp2406=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2406->hd=(
void*) _temp2404; _temp2406->tl= _temp2393; _temp2406;});}}}} _temp2393=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2393);
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2407=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2407[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2408; _temp2408.tag= Cyc_DeclList_tok; _temp2408.f1=({ struct Cyc_List_List*
_temp2409=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2409->hd=( void*) Cyc_Absyn_letv_decl( _temp2393, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2409->tl= 0; _temp2409;}); _temp2408;}); _temp2407;}); break;} case 25:
_LL2392: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 26:
_LL2410: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2412=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2412[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2413; _temp2413.tag= Cyc_DeclList_tok; _temp2413.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2413;});
_temp2412;}); break; case 27: _LL2411: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2415=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2415[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2416; _temp2416.tag= Cyc_DeclSpec_tok;
_temp2416.f1=({ struct Cyc_Parse_Declaration_spec* _temp2417=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2417->sc=({ struct
Cyc_Core_Opt* _temp2418=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2418->v=( void*) Cyc_yyget_StorageClass_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2418;}); _temp2417->tq= Cyc_Absyn_empty_tqual();
_temp2417->type_specs= 0; _temp2417->is_inline= 0; _temp2417->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2417;});
_temp2416;}); _temp2415;}); break; case 28: _LL2414: if(( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc != 0){ Cyc_Parse_warn(({
unsigned char* _temp2420=( unsigned char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2421; _temp2421.curr= _temp2420; _temp2421.base=
_temp2420; _temp2421.last_plus_one= _temp2420 + 51; _temp2421;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2422=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2422[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2423; _temp2423.tag= Cyc_DeclSpec_tok; _temp2423.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2424=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2424->sc=({ struct Cyc_Core_Opt* _temp2425=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2425->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2425;});
_temp2424->tq=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tq; _temp2424->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->type_specs; _temp2424->is_inline=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->is_inline;
_temp2424->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2424;}); _temp2423;}); _temp2422;}); break; case 29: _LL2419: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2427=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2427[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2428; _temp2428.tag= Cyc_DeclSpec_tok; _temp2428.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2429=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2429->sc= 0; _temp2429->tq= Cyc_Absyn_empty_tqual(); _temp2429->type_specs=({
struct Cyc_List_List* _temp2430=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2430->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2430->tl=
0; _temp2430;}); _temp2429->is_inline= 0; _temp2429->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2429;});
_temp2428;}); _temp2427;}); break; case 30: _LL2426: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2432=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2432[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2433; _temp2433.tag= Cyc_DeclSpec_tok;
_temp2433.f1=({ struct Cyc_Parse_Declaration_spec* _temp2434=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2434->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2434->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2434->type_specs=({ struct Cyc_List_List* _temp2435=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2435->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2435->tl=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2435;}); _temp2434->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->is_inline; _temp2434->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2434;}); _temp2433;}); _temp2432;}); break; case 31: _LL2431: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2437=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2437[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2438; _temp2438.tag= Cyc_DeclSpec_tok; _temp2438.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2439=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2439->sc= 0; _temp2439->tq= Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2439->type_specs= 0; _temp2439->is_inline=
0; _temp2439->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2439;}); _temp2438;}); _temp2437;}); break;
case 32: _LL2436: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2441=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2441[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2442; _temp2442.tag= Cyc_DeclSpec_tok;
_temp2442.f1=({ struct Cyc_Parse_Declaration_spec* _temp2443=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2443->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2443->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tq); _temp2443->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2443->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->is_inline; _temp2443->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2443;}); _temp2442;}); _temp2441;}); break; case 33: _LL2440: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2445=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2445[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2446; _temp2446.tag= Cyc_DeclSpec_tok; _temp2446.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2447=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2447->sc= 0; _temp2447->tq= Cyc_Absyn_empty_tqual(); _temp2447->type_specs=
0; _temp2447->is_inline= 1; _temp2447->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2447;});
_temp2446;}); _temp2445;}); break; case 34: _LL2444: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2449=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2449[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2450; _temp2450.tag= Cyc_DeclSpec_tok;
_temp2450.f1=({ struct Cyc_Parse_Declaration_spec* _temp2451=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2451->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2451->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2451->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->type_specs; _temp2451->is_inline= 1; _temp2451->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2451;}); _temp2450;}); _temp2449;}); break; case 35: _LL2448: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp2453=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2453[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2454; _temp2454.tag= Cyc_StorageClass_tok;
_temp2454.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp2454;}); _temp2453;});
break; case 36: _LL2452: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2456=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2456[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2457; _temp2457.tag=
Cyc_StorageClass_tok; _temp2457.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2457;}); _temp2456;}); break; case 37: _LL2455: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2459=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2459[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2460; _temp2460.tag=
Cyc_StorageClass_tok; _temp2460.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2460;}); _temp2459;}); break; case 38: _LL2458: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2462=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2462[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2463; _temp2463.tag=
Cyc_StorageClass_tok; _temp2463.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2463;}); _temp2462;}); break; case 39: _LL2461: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),({ unsigned char*
_temp2465=( unsigned char*)"C"; struct _tagged_string _temp2466; _temp2466.curr=
_temp2465; _temp2466.base= _temp2465; _temp2466.last_plus_one= _temp2465 + 2;
_temp2466;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp2467=( unsigned char*)"only extern or extern \"C\" is allowed";
struct _tagged_string _temp2468; _temp2468.curr= _temp2467; _temp2468.base=
_temp2467; _temp2468.last_plus_one= _temp2467 + 37; _temp2468;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2469=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2469[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2470; _temp2470.tag= Cyc_StorageClass_tok;
_temp2470.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2470;}); _temp2469;});
break; case 40: _LL2464: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2472=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2472[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2473; _temp2473.tag=
Cyc_StorageClass_tok; _temp2473.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2473;}); _temp2472;}); break; case 41: _LL2471: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2475=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2475[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2476; _temp2476.tag=
Cyc_StorageClass_tok; _temp2476.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2476;}); _temp2475;}); break; case 42: _LL2474: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2478=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2478[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2479; _temp2479.tag=
Cyc_AttributeList_tok; _temp2479.f1= 0; _temp2479;}); _temp2478;}); break; case
43: _LL2477: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 44: _LL2480: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2482=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2482[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2483; _temp2483.tag=
Cyc_AttributeList_tok; _temp2483.f1= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2483;}); _temp2482;}); break;
case 45: _LL2481: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2485=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2485[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2486; _temp2486.tag=
Cyc_AttributeList_tok; _temp2486.f1=({ struct Cyc_List_List* _temp2487=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2487->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2487->tl= 0; _temp2487;}); _temp2486;}); _temp2485;}); break; case 46:
_LL2484: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp2489=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2489[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2490; _temp2490.tag=
Cyc_AttributeList_tok; _temp2490.f1=({ struct Cyc_List_List* _temp2491=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2491->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2491->tl= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2491;}); _temp2490;}); _temp2489;}); break; case 47:
_LL2488: { struct _tagged_string _temp2493= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); goto _LL2494; _LL2494: if((((({ struct
_tagged_string _temp2495= _temp2493;( unsigned int)( _temp2495.last_plus_one -
_temp2495.curr);}) > 4?*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2493, sizeof(
unsigned char), 0) =='_': 0)?*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2493, sizeof(
unsigned char), 1) =='_': 0)?*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2493, sizeof(
unsigned char),( int)(({ struct _tagged_string _temp2496= _temp2493;(
unsigned int)( _temp2496.last_plus_one - _temp2496.curr);}) - 2)) =='_': 0)?*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2493, sizeof( unsigned char),( int)(({ struct
_tagged_string _temp2497= _temp2493;( unsigned int)( _temp2497.last_plus_one -
_temp2497.curr);}) - 3)) =='_': 0){ _temp2493=( struct _tagged_string) Cyc_String_substring(
_temp2493, 2,({ struct _tagged_string _temp2498= _temp2493;( unsigned int)(
_temp2498.last_plus_one - _temp2498.curr);}) - 5);}{ int i= 0; for( 0; i < 16u;
++ i){ if( Cyc_String_strcmp( _temp2493,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2499=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2499[ 0]=({ struct Cyc_Attribute_tok_struct _temp2500; _temp2500.tag= Cyc_Attribute_tok;
_temp2500.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp2500;}); _temp2499;}); break;}} if( i == 16u){ Cyc_Parse_err(({
unsigned char* _temp2501=( unsigned char*)"unrecognized attribute"; struct
_tagged_string _temp2502; _temp2502.curr= _temp2501; _temp2502.base= _temp2501;
_temp2502.last_plus_one= _temp2501 + 23; _temp2502;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2503=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2503[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2504; _temp2504.tag= Cyc_Attribute_tok; _temp2504.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp2504;}); _temp2503;});} break;}} case
48: _LL2492: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2506=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2506[ 0]=({ struct Cyc_Attribute_tok_struct _temp2507; _temp2507.tag= Cyc_Attribute_tok;
_temp2507.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2507;}); _temp2506;});
break; case 49: _LL2505: { struct _tagged_string _temp2509= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); goto
_LL2510; _LL2510: { struct _tuple16 _temp2513; int _temp2514; void* _temp2516;
struct _tuple16* _temp2511= Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2513=* _temp2511; _LL2517: _temp2516= _temp2513.f1;
goto _LL2515; _LL2515: _temp2514= _temp2513.f2; goto _LL2512; _LL2512: { void* a;
if( Cyc_String_zstrcmp( _temp2509,({ unsigned char* _temp2518=( unsigned char*)"regparm";
struct _tagged_string _temp2519; _temp2519.curr= _temp2518; _temp2519.base=
_temp2518; _temp2519.last_plus_one= _temp2518 + 8; _temp2519;})) == 0? 1: Cyc_String_zstrcmp(
_temp2509,({ unsigned char* _temp2520=( unsigned char*)"__regparm__"; struct
_tagged_string _temp2521; _temp2521.curr= _temp2520; _temp2521.base= _temp2520;
_temp2521.last_plus_one= _temp2520 + 12; _temp2521;})) == 0){ if( _temp2514 <= 0?
1: _temp2514 > 3){ Cyc_Parse_err(({ unsigned char* _temp2522=( unsigned char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2523; _temp2523.curr= _temp2522; _temp2523.base=
_temp2522; _temp2523.last_plus_one= _temp2522 + 39; _temp2523;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2524=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2524[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2525; _temp2525.tag= Cyc_Absyn_Regparm_att;
_temp2525.f1= _temp2514; _temp2525;}); _temp2524;});} else{ if( Cyc_String_zstrcmp(
_temp2509,({ unsigned char* _temp2526=( unsigned char*)"aligned"; struct
_tagged_string _temp2527; _temp2527.curr= _temp2526; _temp2527.base= _temp2526;
_temp2527.last_plus_one= _temp2526 + 8; _temp2527;})) == 0? 1: Cyc_String_zstrcmp(
_temp2509,({ unsigned char* _temp2528=( unsigned char*)"__aligned__"; struct
_tagged_string _temp2529; _temp2529.curr= _temp2528; _temp2529.base= _temp2528;
_temp2529.last_plus_one= _temp2528 + 12; _temp2529;})) == 0){ if( _temp2514 < 0){
Cyc_Parse_err(({ unsigned char* _temp2530=( unsigned char*)"aligned requires positive power of two";
struct _tagged_string _temp2531; _temp2531.curr= _temp2530; _temp2531.base=
_temp2530; _temp2531.last_plus_one= _temp2530 + 39; _temp2531;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}{
unsigned int j=( unsigned int) _temp2514; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(({ unsigned char* _temp2532=( unsigned char*)"aligned requires positive power of two";
struct _tagged_string _temp2533; _temp2533.curr= _temp2532; _temp2533.base=
_temp2532; _temp2533.last_plus_one= _temp2532 + 39; _temp2533;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2534=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2534[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2535; _temp2535.tag= Cyc_Absyn_Aligned_att;
_temp2535.f1= _temp2514; _temp2535;}); _temp2534;});}} else{ Cyc_Parse_err(({
unsigned char* _temp2536=( unsigned char*)"unrecognized attribute"; struct
_tagged_string _temp2537; _temp2537.curr= _temp2536; _temp2537.base= _temp2536;
_temp2537.last_plus_one= _temp2536 + 23; _temp2537;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2538=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2538[ 0]=({ struct Cyc_Attribute_tok_struct _temp2539; _temp2539.tag= Cyc_Attribute_tok;
_temp2539.f1=( void*) a; _temp2539;}); _temp2538;}); break;}}} case 50: _LL2508: {
struct _tagged_string _temp2541= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); goto _LL2542; _LL2542: { struct
_tagged_string _temp2543= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); goto _LL2544; _LL2544: { void* a; if( Cyc_String_zstrcmp(
_temp2541,({ unsigned char* _temp2545=( unsigned char*)"section"; struct
_tagged_string _temp2546; _temp2546.curr= _temp2545; _temp2546.base= _temp2545;
_temp2546.last_plus_one= _temp2545 + 8; _temp2546;})) == 0? 1: Cyc_String_zstrcmp(
_temp2541,({ unsigned char* _temp2547=( unsigned char*)"__section__"; struct
_tagged_string _temp2548; _temp2548.curr= _temp2547; _temp2548.base= _temp2547;
_temp2548.last_plus_one= _temp2547 + 12; _temp2548;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2549=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2549[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2550; _temp2550.tag=
Cyc_Absyn_Section_att; _temp2550.f1= _temp2543; _temp2550;}); _temp2549;});}
else{ Cyc_Parse_err(({ unsigned char* _temp2551=( unsigned char*)"unrecognized attribute";
struct _tagged_string _temp2552; _temp2552.curr= _temp2551; _temp2552.base=
_temp2551; _temp2552.last_plus_one= _temp2551 + 23; _temp2552;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2553=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2553[ 0]=({ struct Cyc_Attribute_tok_struct _temp2554; _temp2554.tag= Cyc_Attribute_tok;
_temp2554.f1=( void*) a; _temp2554;}); _temp2553;}); break;}}} case 51: _LL2540:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2556=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2556[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2557; _temp2557.tag= Cyc_TypeSpecifier_tok;
_temp2557.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2557;}); _temp2556;}); break; case 52: _LL2555: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2559=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2559[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2560; _temp2560.tag=
Cyc_TypeSpecifier_tok; _temp2560.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2560;});
_temp2559;}); break; case 53: _LL2558: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2562=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2562[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2563; _temp2563.tag=
Cyc_TypeSpecifier_tok; _temp2563.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2563;}); _temp2562;}); break; case 54: _LL2561:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2565=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2565[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2566; _temp2566.tag= Cyc_TypeSpecifier_tok;
_temp2566.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct* _temp2567=(
struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2567[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2568; _temp2568.tag=
Cyc_Parse_Short_spec; _temp2568.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2568;});
_temp2567;})); _temp2566;}); _temp2565;}); break; case 55: _LL2564: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2570=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2570[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2571; _temp2571.tag= Cyc_TypeSpecifier_tok;
_temp2571.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2571;}); _temp2570;}); break; case 56: _LL2569: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2573=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2573[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2574; _temp2574.tag=
Cyc_TypeSpecifier_tok; _temp2574.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2575=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2575[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2576; _temp2576.tag= Cyc_Parse_Long_spec;
_temp2576.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2576;});
_temp2575;})); _temp2574;}); _temp2573;}); break; case 57: _LL2572: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2578=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2578[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2579; _temp2579.tag= Cyc_TypeSpecifier_tok;
_temp2579.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2579;}); _temp2578;}); break; case 58: _LL2577: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2581=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2581[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2582; _temp2582.tag=
Cyc_TypeSpecifier_tok; _temp2582.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2582;}); _temp2581;}); break; case 59: _LL2580:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2584=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2584[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2585; _temp2585.tag= Cyc_TypeSpecifier_tok;
_temp2585.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct* _temp2586=(
struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2586[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2587; _temp2587.tag=
Cyc_Parse_Signed_spec; _temp2587.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2587;});
_temp2586;})); _temp2585;}); _temp2584;}); break; case 60: _LL2583: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2589=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2589[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2590; _temp2590.tag= Cyc_TypeSpecifier_tok;
_temp2590.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2591=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2591[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2592; _temp2592.tag= Cyc_Parse_Unsigned_spec; _temp2592.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2592;}); _temp2591;})); _temp2590;}); _temp2589;}); break; case 61: _LL2588:
yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 62:
_LL2593: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 63:
_LL2594: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 64:
_LL2595: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2597=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2597[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2598; _temp2598.tag=
Cyc_TypeSpecifier_tok; _temp2598.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2598;}); _temp2597;}); break; case 65: _LL2596: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2600=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2600[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2601; _temp2601.tag=
Cyc_TypeSpecifier_tok; _temp2601.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2601;}); _temp2600;}); break; case 66: _LL2599: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2603=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2603[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2604; _temp2604.tag=
Cyc_TypeSpecifier_tok; _temp2604.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2605=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2605[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp2606; _temp2606.tag= Cyc_Absyn_TypedefType;
_temp2606.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2606.f2= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2606.f3= 0; _temp2606;}); _temp2605;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2604;}); _temp2603;}); break; case 67: _LL2602: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2608=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2608[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2609; _temp2609.tag=
Cyc_TypeSpecifier_tok; _temp2609.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2610=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2610[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2611; _temp2611.tag= Cyc_Absyn_TupleType;
_temp2611.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2611;});
_temp2610;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2609;});
_temp2608;}); break; case 68: _LL2607: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2613=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2613[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2614; _temp2614.tag=
Cyc_TypeSpecifier_tok; _temp2614.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2615=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2615[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2616; _temp2616.tag= Cyc_Absyn_RgnHandleType;
_temp2616.f1=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2616;}); _temp2615;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2614;}); _temp2613;}); break; case 69: _LL2612: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp2618=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2618[ 0]=({ struct Cyc_Kind_tok_struct _temp2619; _temp2619.tag= Cyc_Kind_tok;
_temp2619.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2619;}); _temp2618;}); break; case 70: _LL2617: { struct _tagged_string*
_temp2623; void* _temp2625; struct _tuple1 _temp2621=* Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _LL2626:
_temp2625= _temp2621.f1; goto _LL2624; _LL2624: _temp2623= _temp2621.f2; goto
_LL2622; _LL2622: if( _temp2625 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(({
unsigned char* _temp2627=( unsigned char*)"bad kind in type specifier"; struct
_tagged_string _temp2628; _temp2628.curr= _temp2627; _temp2628.base= _temp2627;
_temp2628.last_plus_one= _temp2627 + 27; _temp2628;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp2629=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2629[ 0]=({ struct Cyc_Kind_tok_struct
_temp2630; _temp2630.tag= Cyc_Kind_tok; _temp2630.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2623, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2630;});
_temp2629;}); break;} case 71: _LL2620: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2632=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2632[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2633; _temp2633.tag= Cyc_TypeQual_tok;
_temp2633.f1=({ struct Cyc_Absyn_Tqual _temp2634; _temp2634.q_const= 1;
_temp2634.q_volatile= 0; _temp2634.q_restrict= 0; _temp2634;}); _temp2633;});
_temp2632;}); break; case 72: _LL2631: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2636=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2636[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2637; _temp2637.tag= Cyc_TypeQual_tok;
_temp2637.f1=({ struct Cyc_Absyn_Tqual _temp2638; _temp2638.q_const= 0;
_temp2638.q_volatile= 1; _temp2638.q_restrict= 0; _temp2638;}); _temp2637;});
_temp2636;}); break; case 73: _LL2635: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2640=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2640[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2641; _temp2641.tag= Cyc_TypeQual_tok;
_temp2641.f1=({ struct Cyc_Absyn_Tqual _temp2642; _temp2642.q_const= 0;
_temp2642.q_volatile= 0; _temp2642.q_restrict= 1; _temp2642;}); _temp2641;});
_temp2640;}); break; case 74: _LL2639: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2644=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2644[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2645; _temp2645.tag=
Cyc_TypeSpecifier_tok; _temp2645.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2646=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2646[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2647; _temp2647.tag= Cyc_Parse_Decl_spec;
_temp2647.f1=({ struct Cyc_Absyn_Decl* _temp2648=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2648->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2649=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2649[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2650; _temp2650.tag= Cyc_Absyn_Enum_d; _temp2650.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2651=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2651->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2651->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2651->fields=({
struct Cyc_Core_Opt* _temp2652=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2652->v=( void*) Cyc_yyget_EnumfieldList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2652;}); _temp2651;}); _temp2650;});
_temp2649;})); _temp2648->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2648;});
_temp2647;}); _temp2646;})); _temp2645;}); _temp2644;}); break; case 75: _LL2643:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2654=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2654[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2655; _temp2655.tag= Cyc_TypeSpecifier_tok;
_temp2655.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2656=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2656[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2657; _temp2657.tag= Cyc_Absyn_EnumType;
_temp2657.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2657.f2= 0; _temp2657;}); _temp2656;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2655;}); _temp2654;}); break; case 76: _LL2653: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2659=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2659[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2660; _temp2660.tag= Cyc_Enumfield_tok;
_temp2660.f1=({ struct Cyc_Absyn_Enumfield* _temp2661=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2661->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2661->tag=
0; _temp2661->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2661;});
_temp2660;}); _temp2659;}); break; case 77: _LL2658: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2663=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2663[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2664; _temp2664.tag= Cyc_Enumfield_tok;
_temp2664.f1=({ struct Cyc_Absyn_Enumfield* _temp2665=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2665->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2665->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2665->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2665;});
_temp2664;}); _temp2663;}); break; case 78: _LL2662: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2667=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2667[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2668; _temp2668.tag=
Cyc_EnumfieldList_tok; _temp2668.f1=({ struct Cyc_List_List* _temp2669=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2669->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2669->tl= 0; _temp2669;}); _temp2668;}); _temp2667;}); break; case 79:
_LL2666: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp2671=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2671[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2672; _temp2672.tag=
Cyc_EnumfieldList_tok; _temp2672.f1=({ struct Cyc_List_List* _temp2673=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2673->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2673->tl= Cyc_yyget_EnumfieldList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2673;}); _temp2672;}); _temp2671;}); break; case 80:
_LL2670: { void* t;{ void* _temp2675= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _LL2677: if( _temp2675 ==( void*) Cyc_Parse_Struct_su){
goto _LL2678;} else{ goto _LL2679;} _LL2679: if( _temp2675 ==( void*) Cyc_Parse_Union_su){
goto _LL2680;} else{ goto _LL2676;} _LL2678: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2681=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2681[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2682; _temp2682.tag= Cyc_Absyn_AnonStructType; _temp2682.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2682;});
_temp2681;}); goto _LL2676; _LL2680: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp2683=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp2683[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp2684; _temp2684.tag= Cyc_Absyn_AnonUnionType; _temp2684.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2684;});
_temp2683;}); goto _LL2676; _LL2676:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2685=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2685[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2686; _temp2686.tag=
Cyc_TypeSpecifier_tok; _temp2686.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp2687=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2687[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2688; _temp2688.tag= Cyc_Parse_Type_spec;
_temp2688.f1=( void*) t; _temp2688.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2688;}); _temp2687;})); _temp2686;}); _temp2685;}); break;} case 81:
_LL2674: { struct Cyc_List_List* _temp2690=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2691; _LL2691: { struct Cyc_Absyn_Decl* d;{ void* _temp2692= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2694:
if( _temp2692 ==( void*) Cyc_Parse_Struct_su){ goto _LL2695;} else{ goto _LL2696;}
_LL2696: if( _temp2692 ==( void*) Cyc_Parse_Union_su){ goto _LL2697;} else{ goto
_LL2693;} _LL2695: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2698=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2698->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2698;}), _temp2690,({ struct Cyc_Core_Opt* _temp2699=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2699->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2699;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2693;
_LL2697: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2700=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2700->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2700;}), _temp2690,({ struct Cyc_Core_Opt* _temp2701=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2701->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2701;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2693;
_LL2693:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2702=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2702[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2703; _temp2703.tag=
Cyc_TypeSpecifier_tok; _temp2703.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2704=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2704[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2705; _temp2705.tag= Cyc_Parse_Decl_spec;
_temp2705.f1= d; _temp2705;}); _temp2704;})); _temp2703;}); _temp2702;}); break;}}
case 82: _LL2689: { struct Cyc_List_List* _temp2707=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2708; _LL2708: { struct Cyc_Absyn_Decl* d;{ void* _temp2709= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2711:
if( _temp2709 ==( void*) Cyc_Parse_Struct_su){ goto _LL2712;} else{ goto _LL2713;}
_LL2713: if( _temp2709 ==( void*) Cyc_Parse_Union_su){ goto _LL2714;} else{ goto
_LL2710;} _LL2712: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2715=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2715->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2715;}), _temp2707,({ struct Cyc_Core_Opt* _temp2716=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2716->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2716;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2710;
_LL2714: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2717=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2717->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2717;}), _temp2707,({ struct Cyc_Core_Opt* _temp2718=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2718->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2718;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2710;
_LL2710:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2719=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2719[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2720; _temp2720.tag=
Cyc_TypeSpecifier_tok; _temp2720.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2721=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2721[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2722; _temp2722.tag= Cyc_Parse_Decl_spec;
_temp2722.f1= d; _temp2722;}); _temp2721;})); _temp2720;}); _temp2719;}); break;}}
case 83: _LL2706:{ void* _temp2724= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _LL2726: if( _temp2724 ==( void*) Cyc_Parse_Struct_su){
goto _LL2727;} else{ goto _LL2728;} _LL2728: if( _temp2724 ==( void*) Cyc_Parse_Union_su){
goto _LL2729;} else{ goto _LL2725;} _LL2727: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2730=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2730[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2731; _temp2731.tag=
Cyc_TypeSpecifier_tok; _temp2731.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2732=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2732[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2733; _temp2733.tag= Cyc_Absyn_StructType;
_temp2733.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2733.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2733.f3= 0;
_temp2733;}); _temp2732;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2731;});
_temp2730;}); goto _LL2725; _LL2729: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2734=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2734[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2735; _temp2735.tag=
Cyc_TypeSpecifier_tok; _temp2735.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2736=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2736[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2737; _temp2737.tag= Cyc_Absyn_UnionType;
_temp2737.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2737.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2737.f3= 0;
_temp2737;}); _temp2736;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2735;});
_temp2734;}); goto _LL2725; _LL2725:;} break; case 84: _LL2723:{ void* _temp2739=
Cyc_yyget_StructOrUnion_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_LL2741: if( _temp2739 ==( void*) Cyc_Parse_Struct_su){ goto _LL2742;} else{
goto _LL2743;} _LL2743: if( _temp2739 ==( void*) Cyc_Parse_Union_su){ goto
_LL2744;} else{ goto _LL2740;} _LL2742: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2745=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2745[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2746; _temp2746.tag=
Cyc_TypeSpecifier_tok; _temp2746.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2747=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2747[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2748; _temp2748.tag= Cyc_Absyn_StructType;
_temp2748.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2748.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2748.f3= 0;
_temp2748;}); _temp2747;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2746;});
_temp2745;}); goto _LL2740; _LL2744: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2749=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2749[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2750; _temp2750.tag=
Cyc_TypeSpecifier_tok; _temp2750.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2751=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2751[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2752; _temp2752.tag= Cyc_Absyn_UnionType;
_temp2752.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2752.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2752.f3= 0;
_temp2752;}); _temp2751;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2750;});
_temp2749;}); goto _LL2740; _LL2740:;} break; case 85: _LL2738: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2754=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2754[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2755; _temp2755.tag= Cyc_TypeList_tok; _temp2755.f1= 0; _temp2755;});
_temp2754;}); break; case 86: _LL2753: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2757=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2757[ 0]=({ struct Cyc_TypeList_tok_struct _temp2758; _temp2758.tag= Cyc_TypeList_tok;
_temp2758.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2758;}); _temp2757;}); break; case 87: _LL2756:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2760=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2760[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2761; _temp2761.tag= Cyc_TypeList_tok; _temp2761.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2761;}); _temp2760;}); break;
case 88: _LL2759: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2763=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2763[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2764; _temp2764.tag=
Cyc_StructOrUnion_tok; _temp2764.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp2764;}); _temp2763;}); break; case 89: _LL2762: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2766=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2766[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2767; _temp2767.tag=
Cyc_StructOrUnion_tok; _temp2767.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp2767;}); _temp2766;}); break; case 90: _LL2765: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2769=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2769[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2770; _temp2770.tag= Cyc_StructFieldDeclList_tok; _temp2770.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))); _temp2770;});
_temp2769;}); break; case 91: _LL2768: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2772=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2772[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2773; _temp2773.tag= Cyc_StructFieldDeclListList_tok; _temp2773.f1=({
struct Cyc_List_List* _temp2774=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2774->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2774->tl=
0; _temp2774;}); _temp2773;}); _temp2772;}); break; case 92: _LL2771: yyval=(
void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2776=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp2776[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp2777; _temp2777.tag= Cyc_StructFieldDeclListList_tok;
_temp2777.f1=({ struct Cyc_List_List* _temp2778=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2778->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2778->tl=
Cyc_yyget_StructFieldDeclListList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2778;}); _temp2777;}); _temp2776;}); break; case 93:
_LL2775: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2780=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2780[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2781; _temp2781.tag=
Cyc_InitDeclList_tok; _temp2781.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2781;}); _temp2780;}); break; case 94:
_LL2779: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2783=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2783[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2784; _temp2784.tag=
Cyc_InitDeclList_tok; _temp2784.f1=({ struct Cyc_List_List* _temp2785=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2785->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2785->tl= 0; _temp2785;}); _temp2784;}); _temp2783;}); break; case 95:
_LL2782: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2787=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2787[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2788; _temp2788.tag=
Cyc_InitDeclList_tok; _temp2788.f1=({ struct Cyc_List_List* _temp2789=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2789->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2789->tl= Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2789;}); _temp2788;}); _temp2787;}); break; case 96:
_LL2786: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2791=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2791[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2792; _temp2792.tag= Cyc_InitDecl_tok; _temp2792.f1=({ struct _tuple15*
_temp2793=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2793->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2793->f2= 0; _temp2793;}); _temp2792;}); _temp2791;}); break; case 97:
_LL2790: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2795=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2795[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2796; _temp2796.tag= Cyc_InitDecl_tok; _temp2796.f1=({ struct _tuple15*
_temp2797=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2797->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2797->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2797;}); _temp2796;}); _temp2795;}); break;
case 98: _LL2794: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f1; struct
Cyc_List_List* _temp2799=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f3; goto _LL2800; _LL2800: { void* _temp2801=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
goto _LL2802; _LL2802: { struct Cyc_List_List* _temp2805; struct Cyc_List_List*
_temp2807; struct _tuple0 _temp2803=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _LL2808: _temp2807= _temp2803.f1; goto
_LL2806; _LL2806: _temp2805= _temp2803.f2; goto _LL2804; _LL2804: { struct Cyc_List_List*
_temp2809=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, _temp2801, _temp2807, _temp2799),
_temp2805); goto _LL2810; _LL2810: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2811=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2811[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2812; _temp2812.tag= Cyc_StructFieldDeclList_tok; _temp2812.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
_temp2809); _temp2812;}); _temp2811;}); break;}}}} case 99: _LL2798: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2814=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2814[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2815; _temp2815.tag= Cyc_QualSpecList_tok;
_temp2815.f1=({ struct _tuple18* _temp2816=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2816->f1= Cyc_Absyn_empty_tqual(); _temp2816->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2817=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2817->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2817->tl=
0; _temp2817;}); _temp2816->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp2816;}); _temp2815;}); _temp2814;});
break; case 100: _LL2813: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2819=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2819[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2820; _temp2820.tag=
Cyc_QualSpecList_tok; _temp2820.f1=({ struct _tuple18* _temp2821=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2821->f1=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; _temp2821->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2822=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2822->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2822->tl=(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2;
_temp2822;}); _temp2821->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2821;}); _temp2820;}); _temp2819;});
break; case 101: _LL2818: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2824=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2824[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2825; _temp2825.tag=
Cyc_QualSpecList_tok; _temp2825.f1=({ struct _tuple18* _temp2826=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2826->f1= Cyc_yyget_TypeQual_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2826->f2=
0; _temp2826->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2826;}); _temp2825;}); _temp2824;}); break; case 102:
_LL2823: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2828=( struct
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2828[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2829; _temp2829.tag=
Cyc_QualSpecList_tok; _temp2829.f1=({ struct _tuple18* _temp2830=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2830->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1);
_temp2830->f2=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp2830->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2830;});
_temp2829;}); _temp2828;}); break; case 103: _LL2827: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp2832=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2832[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2833; _temp2833.tag= Cyc_DeclaratorExpoptList_tok;
_temp2833.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))); _temp2833;}); _temp2832;}); break; case 104: _LL2831: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2835=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2835[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2836; _temp2836.tag= Cyc_DeclaratorExpoptList_tok;
_temp2836.f1=({ struct Cyc_List_List* _temp2837=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2837->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2837->tl=
0; _temp2837;}); _temp2836;}); _temp2835;}); break; case 105: _LL2834: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2839=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2839[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2840; _temp2840.tag= Cyc_DeclaratorExpoptList_tok;
_temp2840.f1=({ struct Cyc_List_List* _temp2841=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2841->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2841->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2841;}); _temp2840;}); _temp2839;}); break; case 106:
_LL2838: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2843=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2843[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2844; _temp2844.tag=
Cyc_DeclaratorExpopt_tok; _temp2844.f1=({ struct _tuple13* _temp2845=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2845->f1= Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2845->f2=
0; _temp2845;}); _temp2844;}); _temp2843;}); break; case 107: _LL2842: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2847=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2847[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2848; _temp2848.tag= Cyc_DeclaratorExpopt_tok;
_temp2848.f1=({ struct _tuple13* _temp2849=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2849->f1=({ struct Cyc_Parse_Declarator*
_temp2851=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2851->id=({ struct _tuple1* _temp2852=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2852->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp2856=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp2856[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp2857; _temp2857.tag= Cyc_Absyn_Rel_n;
_temp2857.f1= 0; _temp2857;}); _temp2856;}); _temp2852->f2=({ struct
_tagged_string* _temp2853=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp2853[ 0]=({ unsigned char* _temp2854=( unsigned char*)"";
struct _tagged_string _temp2855; _temp2855.curr= _temp2854; _temp2855.base=
_temp2854; _temp2855.last_plus_one= _temp2854 + 1; _temp2855;}); _temp2853;});
_temp2852;}); _temp2851->tms= 0; _temp2851;}); _temp2849->f2=({ struct Cyc_Core_Opt*
_temp2850=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2850->v=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2850;}); _temp2849;}); _temp2848;}); _temp2847;}); break;
case 108: _LL2846: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2859=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2859[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2860; _temp2860.tag= Cyc_DeclaratorExpopt_tok; _temp2860.f1=({ struct
_tuple13* _temp2861=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp2861->f1= Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2861->f2=({ struct Cyc_Core_Opt* _temp2862=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2862->v=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2862;});
_temp2861;}); _temp2860;}); _temp2859;}); break; case 109: _LL2858: { struct Cyc_List_List*
_temp2864=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL2865; _LL2865: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2866=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2866[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2867; _temp2867.tag=
Cyc_TypeSpecifier_tok; _temp2867.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2868=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2868[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2869; _temp2869.tag= Cyc_Parse_Decl_spec;
_temp2869.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), _temp2864,({
struct Cyc_Core_Opt* _temp2870=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2870->v=( void*) Cyc_yyget_TunionFieldList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2870;}), Cyc_yyget_Bool_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2869;}); _temp2868;})); _temp2867;}); _temp2866;}); break;} case 110:
_LL2863: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2872=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2872[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2873; _temp2873.tag=
Cyc_TypeSpecifier_tok; _temp2873.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2874=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2874[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2875; _temp2875.tag= Cyc_Absyn_TunionType;
_temp2875.f1=({ struct Cyc_Absyn_TunionInfo _temp2876; _temp2876.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2877=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2877[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2878; _temp2878.tag= Cyc_Absyn_UnknownTunion;
_temp2878.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2879; _temp2879.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2879.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2879;}); _temp2878;}); _temp2877;})); _temp2876.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2876.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2876;}); _temp2875;});
_temp2874;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2873;});
_temp2872;}); break; case 111: _LL2871: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2881=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2881[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2882; _temp2882.tag=
Cyc_TypeSpecifier_tok; _temp2882.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2883=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2883[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2884; _temp2884.tag= Cyc_Absyn_TunionType;
_temp2884.f1=({ struct Cyc_Absyn_TunionInfo _temp2885; _temp2885.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2886=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2886[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2887; _temp2887.tag= Cyc_Absyn_UnknownTunion;
_temp2887.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2888; _temp2888.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2888.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp2888;}); _temp2887;}); _temp2886;})); _temp2885.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2885.rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2885;}); _temp2884;}); _temp2883;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2882;}); _temp2881;}); break; case 112: _LL2880: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2890=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2890[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2891; _temp2891.tag= Cyc_TypeSpecifier_tok;
_temp2891.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2892=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2892[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2893; _temp2893.tag= Cyc_Absyn_TunionFieldType; _temp2893.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2894; _temp2894.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2895=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2895[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2896; _temp2896.tag= Cyc_Absyn_UnknownTunionfield; _temp2896.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2897; _temp2897.tunion_name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2897.field_name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2897.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2897;}); _temp2896;}); _temp2895;})); _temp2894.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2894;}); _temp2893;}); _temp2892;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2891;}); _temp2890;}); break; case 113: _LL2889: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2899=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2899[ 0]=({ struct Cyc_Bool_tok_struct
_temp2900; _temp2900.tag= Cyc_Bool_tok; _temp2900.f1= 0; _temp2900;}); _temp2899;});
break; case 114: _LL2898: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2902=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2902[ 0]=({ struct Cyc_Bool_tok_struct _temp2903; _temp2903.tag= Cyc_Bool_tok;
_temp2903.f1= 1; _temp2903;}); _temp2902;}); break; case 115: _LL2901: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2905=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2905[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2906; _temp2906.tag= Cyc_TunionFieldList_tok;
_temp2906.f1=({ struct Cyc_List_List* _temp2907=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2907->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2907->tl=
0; _temp2907;}); _temp2906;}); _temp2905;}); break; case 116: _LL2904: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2909=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2909[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2910; _temp2910.tag= Cyc_TunionFieldList_tok;
_temp2910.f1=({ struct Cyc_List_List* _temp2911=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2911->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2911->tl=
0; _temp2911;}); _temp2910;}); _temp2909;}); break; case 117: _LL2908: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2913=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2913[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2914; _temp2914.tag= Cyc_TunionFieldList_tok;
_temp2914.f1=({ struct Cyc_List_List* _temp2915=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2915->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2915->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2915;}); _temp2914;}); _temp2913;}); break; case 118: _LL2912: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2917=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2917[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2918; _temp2918.tag= Cyc_TunionFieldList_tok;
_temp2918.f1=({ struct Cyc_List_List* _temp2919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2919->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2919->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2919;}); _temp2918;}); _temp2917;}); break; case 119: _LL2916: yyval=( void*)({
struct Cyc_Scope_tok_struct* _temp2921=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2921[ 0]=({ struct Cyc_Scope_tok_struct
_temp2922; _temp2922.tag= Cyc_Scope_tok; _temp2922.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2922;}); _temp2921;}); break; case 120: _LL2920: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2924=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2924[ 0]=({ struct Cyc_Scope_tok_struct
_temp2925; _temp2925.tag= Cyc_Scope_tok; _temp2925.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2925;}); _temp2924;}); break; case 121: _LL2923: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2927=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2927[ 0]=({ struct Cyc_Scope_tok_struct
_temp2928; _temp2928.tag= Cyc_Scope_tok; _temp2928.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2928;}); _temp2927;}); break; case 122: _LL2926: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2930=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2930[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2931; _temp2931.tag= Cyc_TunionField_tok;
_temp2931.f1=({ struct Cyc_Absyn_Tunionfield* _temp2932=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2932->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2932->tvs=
0; _temp2932->typs= 0; _temp2932->loc= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2932->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2932;}); _temp2931;}); _temp2930;}); break; case 123:
_LL2929: { struct Cyc_List_List* _temp2934=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2935; _LL2935: { struct Cyc_List_List* _temp2936=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2937; _LL2937: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2938=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2938[ 0]=({ struct Cyc_TunionField_tok_struct _temp2939; _temp2939.tag= Cyc_TunionField_tok;
_temp2939.f1=({ struct Cyc_Absyn_Tunionfield* _temp2940=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2940->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2940->tvs=
_temp2936; _temp2940->typs= _temp2934; _temp2940->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2940->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp2940;}); _temp2939;}); _temp2938;}); break;}} case 124:
_LL2933: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 125:
_LL2941: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2943=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2943[ 0]=({ struct Cyc_Declarator_tok_struct _temp2944; _temp2944.tag= Cyc_Declarator_tok;
_temp2944.f1=({ struct Cyc_Parse_Declarator* _temp2945=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2945->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->id; _temp2945->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms);
_temp2945;}); _temp2944;}); _temp2943;}); break; case 126: _LL2942: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2947=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2947[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2948; _temp2948.tag= Cyc_Declarator_tok;
_temp2948.f1=({ struct Cyc_Parse_Declarator* _temp2949=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2949->id= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2949->tms=
0; _temp2949;}); _temp2948;}); _temp2947;}); break; case 127: _LL2946: yyval=*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1); break; case
128: _LL2950: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2952=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2952[ 0]=({ struct Cyc_Declarator_tok_struct _temp2953; _temp2953.tag= Cyc_Declarator_tok;
_temp2953.f1=({ struct Cyc_Parse_Declarator* _temp2954=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2954->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2954->tms=({ struct Cyc_List_List* _temp2955=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2955->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2955->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)))->tms; _temp2955;}); _temp2954;}); _temp2953;}); _temp2952;});
break; case 129: _LL2951: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2957=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2957[ 0]=({ struct Cyc_Declarator_tok_struct _temp2958; _temp2958.tag= Cyc_Declarator_tok;
_temp2958.f1=({ struct Cyc_Parse_Declarator* _temp2959=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2959->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2959->tms=({ struct Cyc_List_List* _temp2960=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2960->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2961=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2961[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2962; _temp2962.tag= Cyc_Absyn_ConstArray_mod;
_temp2962.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2962;}); _temp2961;})); _temp2960->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2960;}); _temp2959;});
_temp2958;}); _temp2957;}); break; case 130: _LL2956: { struct _tuple17
_temp2966; struct Cyc_List_List* _temp2967; struct Cyc_Core_Opt* _temp2969;
struct Cyc_Absyn_VarargInfo* _temp2971; int _temp2973; struct Cyc_List_List*
_temp2975; struct _tuple17* _temp2964= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2966=* _temp2964; _LL2976:
_temp2975= _temp2966.f1; goto _LL2974; _LL2974: _temp2973= _temp2966.f2; goto
_LL2972; _LL2972: _temp2971= _temp2966.f3; goto _LL2970; _LL2970: _temp2969=
_temp2966.f4; goto _LL2968; _LL2968: _temp2967= _temp2966.f5; goto _LL2965;
_LL2965: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2977=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2977[ 0]=({ struct Cyc_Declarator_tok_struct _temp2978; _temp2978.tag= Cyc_Declarator_tok;
_temp2978.f1=({ struct Cyc_Parse_Declarator* _temp2979=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2979->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2979->tms=({ struct Cyc_List_List* _temp2980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2980->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2981=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2981[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2982; _temp2982.tag= Cyc_Absyn_Function_mod;
_temp2982.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2983=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2983[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2984; _temp2984.tag= Cyc_Absyn_WithTypes;
_temp2984.f1= _temp2975; _temp2984.f2= _temp2973; _temp2984.f3= _temp2971;
_temp2984.f4= _temp2969; _temp2984.f5= _temp2967; _temp2984;}); _temp2983;}));
_temp2982;}); _temp2981;})); _temp2980->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2980;}); _temp2979;});
_temp2978;}); _temp2977;}); break;} case 131: _LL2963: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2986=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2986[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2987; _temp2987.tag= Cyc_Declarator_tok;
_temp2987.f1=({ struct Cyc_Parse_Declarator* _temp2988=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2988->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->id;
_temp2988->tms=({ struct Cyc_List_List* _temp2989=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2989->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2990=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2990[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2991; _temp2991.tag= Cyc_Absyn_Function_mod;
_temp2991.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2992=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2992[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2993; _temp2993.tag= Cyc_Absyn_WithTypes;
_temp2993.f1= 0; _temp2993.f2= 0; _temp2993.f3= 0; _temp2993.f4= Cyc_yyget_TypeOpt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2993.f5=
Cyc_yyget_Rgnorder_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2993;}); _temp2992;})); _temp2991;}); _temp2990;})); _temp2989->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp2989;}); _temp2988;}); _temp2987;}); _temp2986;}); break; case 132: _LL2985:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2995=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2995[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2996; _temp2996.tag= Cyc_Declarator_tok;
_temp2996.f1=({ struct Cyc_Parse_Declarator* _temp2997=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2997->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2997->tms=({ struct Cyc_List_List* _temp2998=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2998->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2999=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2999[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3000; _temp3000.tag= Cyc_Absyn_Function_mod;
_temp3000.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp3001=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3001[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp3002; _temp3002.tag= Cyc_Absyn_NoTypes;
_temp3002.f1= Cyc_yyget_IdList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3002.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3002;});
_temp3001;})); _temp3000;}); _temp2999;})); _temp2998->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2998;}); _temp2997;}); _temp2996;}); _temp2995;}); break; case 133: _LL2994: {
struct Cyc_List_List* _temp3004=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3005; _LL3005: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3006=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3006[ 0]=({ struct Cyc_Declarator_tok_struct _temp3007; _temp3007.tag= Cyc_Declarator_tok;
_temp3007.f1=({ struct Cyc_Parse_Declarator* _temp3008=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3008->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp3008->tms=({ struct Cyc_List_List* _temp3009=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3009->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3010=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3010[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3011; _temp3011.tag= Cyc_Absyn_TypeParams_mod;
_temp3011.f1= _temp3004; _temp3011.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3011.f3= 0; _temp3011;}); _temp3010;})); _temp3009->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3009;}); _temp3008;}); _temp3007;}); _temp3006;}); break;} case 134:
_LL3003:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3013=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3014; _LL3014: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3015=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3015[ 0]=({ struct Cyc_Declarator_tok_struct _temp3016; _temp3016.tag= Cyc_Declarator_tok;
_temp3016.f1=({ struct Cyc_Parse_Declarator* _temp3017=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3017->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp3017->tms=({ struct Cyc_List_List* _temp3018=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3018->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3019=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3019[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3020; _temp3020.tag= Cyc_Absyn_TypeParams_mod;
_temp3020.f1= _temp3013; _temp3020.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3020.f3= 0; _temp3020;}); _temp3019;})); _temp3018->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3018;}); _temp3017;}); _temp3016;}); _temp3015;}); break;} case 135:
_LL3012: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3022=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3022[ 0]=({ struct Cyc_Declarator_tok_struct _temp3023; _temp3023.tag= Cyc_Declarator_tok;
_temp3023.f1=({ struct Cyc_Parse_Declarator* _temp3024=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3024->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))->id;
_temp3024->tms=({ struct Cyc_List_List* _temp3025=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3025->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3026=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3026[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3027; _temp3027.tag= Cyc_Absyn_Attributes_mod;
_temp3027.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3027.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3027;});
_temp3026;})); _temp3025->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)))->tms; _temp3025;}); _temp3024;}); _temp3023;});
_temp3022;}); break; case 136: _LL3021: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3029=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3029[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3030; _temp3030.tag= Cyc_TypeModifierList_tok; _temp3030.f1=({ struct Cyc_List_List*
_temp3031=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3031->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3032=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3032[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3033; _temp3033.tag=
Cyc_Absyn_Pointer_mod; _temp3033.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3033.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3033.f3=
Cyc_Absyn_empty_tqual(); _temp3033;}); _temp3032;})); _temp3031->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), 0); _temp3031;}); _temp3030;}); _temp3029;});
break; case 137: _LL3028: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3035=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3035[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3036; _temp3036.tag= Cyc_TypeModifierList_tok; _temp3036.f1=({ struct Cyc_List_List*
_temp3037=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3037->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3038=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3038[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3039; _temp3039.tag=
Cyc_Absyn_Pointer_mod; _temp3039.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3039.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3039.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3039;}); _temp3038;})); _temp3037->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0); _temp3037;}); _temp3036;}); _temp3035;}); break; case 138: _LL3034: yyval=(
void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3041=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp3041[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp3042; _temp3042.tag= Cyc_TypeModifierList_tok;
_temp3042.f1=({ struct Cyc_List_List* _temp3043=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3043->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp3044=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3044[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3045; _temp3045.tag= Cyc_Absyn_Pointer_mod;
_temp3045.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3045.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3045.f3=
Cyc_Absyn_empty_tqual(); _temp3045;}); _temp3044;})); _temp3043->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_TypeModifierList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3043;}); _temp3042;}); _temp3041;});
break; case 139: _LL3040: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3047=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3047[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3048; _temp3048.tag= Cyc_TypeModifierList_tok; _temp3048.f1=({ struct Cyc_List_List*
_temp3049=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3049->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3050=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3050[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3051; _temp3051.tag=
Cyc_Absyn_Pointer_mod; _temp3051.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3051.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3051.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3051;}); _temp3050;})); _temp3049->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3049;}); _temp3048;}); _temp3047;}); break; case 140: _LL3046: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp3053=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3053[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3054; _temp3054.tag= Cyc_Pointer_Sort_tok;
_temp3054.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp3055=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3055[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3056; _temp3056.tag=
Cyc_Absyn_Nullable_ps; _temp3056.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3056;}); _temp3055;})); _temp3054;}); _temp3053;}); break; case 141:
_LL3052: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3058=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3058[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3059; _temp3059.tag=
Cyc_Pointer_Sort_tok; _temp3059.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3060=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3060[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3061; _temp3061.tag= Cyc_Absyn_NonNullable_ps; _temp3061.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3061;}); _temp3060;})); _temp3059;}); _temp3058;});
break; case 142: _LL3057: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3063=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3063[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3064; _temp3064.tag=
Cyc_Pointer_Sort_tok; _temp3064.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3065=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3065[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3066; _temp3066.tag=
Cyc_Absyn_Nullable_ps; _temp3066.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3066;}); _temp3065;})); _temp3064;});
_temp3063;}); break; case 143: _LL3062: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3068=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3068[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3069; _temp3069.tag=
Cyc_Pointer_Sort_tok; _temp3069.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3070=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3070[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3071; _temp3071.tag= Cyc_Absyn_NonNullable_ps; _temp3071.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3071;});
_temp3070;})); _temp3069;}); _temp3068;}); break; case 144: _LL3067: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3073=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3073[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3074; _temp3074.tag= Cyc_Pointer_Sort_tok;
_temp3074.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp3074;});
_temp3073;}); break; case 145: _LL3072: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp3076=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3076[ 0]=({ struct Cyc_Rgn_tok_struct _temp3077; _temp3077.tag= Cyc_Rgn_tok;
_temp3077.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3077;}); _temp3076;});
break; case 146: _LL3075: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 147: _LL3078: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3080=(
struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3080[ 0]=({ struct Cyc_Rgn_tok_struct _temp3081; _temp3081.tag= Cyc_Rgn_tok;
_temp3081.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3081;}); _temp3080;});
break; case 148: _LL3079: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({
unsigned char* _temp3083=( unsigned char*)"expecting region kind\n"; struct
_tagged_string _temp3084; _temp3084.curr= _temp3083; _temp3084.base= _temp3083;
_temp3084.last_plus_one= _temp3083 + 23; _temp3084;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3085=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp3085[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3086; _temp3086.tag= Cyc_Rgn_tok; _temp3086.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3086;}); _temp3085;}); break; case 149: _LL3082: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp3088=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3088[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3089; _temp3089.tag= Cyc_Rgn_tok; _temp3089.f1=( void*) Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3090=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3090->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp3090;}),
0); _temp3089;}); _temp3088;}); break; case 150: _LL3087: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 151: _LL3091: yyval=( void*)({
struct Cyc_TypeQual_tok_struct* _temp3093=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3093[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3094; _temp3094.tag= Cyc_TypeQual_tok; _temp3094.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3094;}); _temp3093;}); break; case 152: _LL3092: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3096=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3096[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3097; _temp3097.tag= Cyc_ParamDeclListBool_tok;
_temp3097.f1=({ struct _tuple17* _temp3098=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3098->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3098->f2= 0; _temp3098->f3= 0;
_temp3098->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3098->f5= Cyc_yyget_Rgnorder_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3098;}); _temp3097;}); _temp3096;}); break;
case 153: _LL3095: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3100=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp3100[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp3101; _temp3101.tag= Cyc_ParamDeclListBool_tok; _temp3101.f1=({ struct
_tuple17* _temp3102=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp3102->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp3102->f2= 1; _temp3102->f3= 0; _temp3102->f4= Cyc_yyget_TypeOpt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3102->f5=
Cyc_yyget_Rgnorder_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3102;}); _temp3101;}); _temp3100;}); break; case 154: _LL3099: { void*
_temp3106; struct Cyc_Absyn_Tqual _temp3108; struct Cyc_Core_Opt* _temp3110;
struct _tuple2 _temp3104=* Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _LL3111: _temp3110= _temp3104.f1; goto
_LL3109; _LL3109: _temp3108= _temp3104.f2; goto _LL3107; _LL3107: _temp3106=
_temp3104.f3; goto _LL3105; _LL3105: { struct Cyc_Absyn_VarargInfo* _temp3113=({
struct Cyc_Absyn_VarargInfo* _temp3112=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp3112->name= _temp3110; _temp3112->tq=
_temp3108; _temp3112->type=( void*) _temp3106; _temp3112->rgn=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3112->inject=
Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp3112;}); goto _LL3114; _LL3114: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3115=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp3115[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp3116; _temp3116.tag= Cyc_ParamDeclListBool_tok; _temp3116.f1=({ struct
_tuple17* _temp3117=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp3117->f1= 0; _temp3117->f2= 0; _temp3117->f3= _temp3113; _temp3117->f4= Cyc_yyget_TypeOpt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3117->f5=
Cyc_yyget_Rgnorder_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3117;}); _temp3116;}); _temp3115;}); break;}} case 155: _LL3103: { void*
_temp3121; struct Cyc_Absyn_Tqual _temp3123; struct Cyc_Core_Opt* _temp3125;
struct _tuple2 _temp3119=* Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _LL3126: _temp3125= _temp3119.f1; goto
_LL3124; _LL3124: _temp3123= _temp3119.f2; goto _LL3122; _LL3122: _temp3121=
_temp3119.f3; goto _LL3120; _LL3120: { struct Cyc_Absyn_VarargInfo* _temp3128=({
struct Cyc_Absyn_VarargInfo* _temp3127=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp3127->name= _temp3125; _temp3127->tq=
_temp3123; _temp3127->type=( void*) _temp3121; _temp3127->rgn=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3127->inject=
Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp3127;}); goto _LL3129; _LL3129: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3130=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp3130[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp3131; _temp3131.tag= Cyc_ParamDeclListBool_tok; _temp3131.f1=({ struct
_tuple17* _temp3132=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp3132->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 7)));
_temp3132->f2= 0; _temp3132->f3= _temp3128; _temp3132->f4= Cyc_yyget_TypeOpt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3132->f5=
Cyc_yyget_Rgnorder_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3132;}); _temp3131;}); _temp3130;}); break;}} case 156: _LL3118: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp3134=( struct Cyc_TypeOpt_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp3134[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp3135; _temp3135.tag= Cyc_TypeOpt_tok; _temp3135.f1= 0; _temp3135;});
_temp3134;}); break; case 157: _LL3133: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp3137=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp3137[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp3138; _temp3138.tag= Cyc_TypeOpt_tok;
_temp3138.f1=({ struct Cyc_Core_Opt* _temp3139=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3139->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3140=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3140[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3141; _temp3141.tag= Cyc_Absyn_JoinEff;
_temp3141.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3141;}); _temp3140;})); _temp3139;}); _temp3138;});
_temp3137;}); break; case 158: _LL3136: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp3143=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp3143[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp3144; _temp3144.tag= Cyc_Rgnorder_tok;
_temp3144.f1= 0; _temp3144;}); _temp3143;}); break; case 159: _LL3142: yyval=*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 160:
_LL3145: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp3147=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp3147[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp3148; _temp3148.tag= Cyc_Rgnorder_tok; _temp3148.f1=({ struct Cyc_List_List*
_temp3149=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3149->hd=( void*)({ struct _tuple19* _temp3150=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp3150->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3150->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp3150;}); _temp3149->tl= 0; _temp3149;});
_temp3148;}); _temp3147;}); break; case 161: _LL3146: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp3152=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp3152[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp3153; _temp3153.tag= Cyc_Rgnorder_tok; _temp3153.f1=({ struct Cyc_List_List*
_temp3154=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3154->hd=( void*)({ struct _tuple19* _temp3155=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp3155->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3155->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp3155;}); _temp3154->tl= Cyc_yyget_Rgnorder_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3154;});
_temp3153;}); _temp3152;}); break; case 162: _LL3151: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp3157=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp3157[ 0]=({ struct Cyc_Bool_tok_struct
_temp3158; _temp3158.tag= Cyc_Bool_tok; _temp3158.f1= 0; _temp3158;}); _temp3157;});
break; case 163: _LL3156: { struct _tagged_string _temp3160= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); goto _LL3161;
_LL3161: if( Cyc_String_zstrcmp( _temp3160,({ unsigned char* _temp3162=(
unsigned char*)"inject"; struct _tagged_string _temp3163; _temp3163.curr=
_temp3162; _temp3163.base= _temp3162; _temp3163.last_plus_one= _temp3162 + 7;
_temp3163;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp3164=( unsigned char*)"missing type in function declaration";
struct _tagged_string _temp3165; _temp3165.curr= _temp3164; _temp3165.base=
_temp3164; _temp3165.last_plus_one= _temp3164 + 37; _temp3165;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3166=( struct Cyc_Bool_tok_struct*)
GC_malloc( sizeof( struct Cyc_Bool_tok_struct)); _temp3166[ 0]=({ struct Cyc_Bool_tok_struct
_temp3167; _temp3167.tag= Cyc_Bool_tok; _temp3167.f1= 1; _temp3167;}); _temp3166;});
break;} case 164: _LL3159: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 165: _LL3168: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3170=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3170[ 0]=({ struct Cyc_TypeList_tok_struct _temp3171; _temp3171.tag= Cyc_TypeList_tok;
_temp3171.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3171;}); _temp3170;}); break; case 166:
_LL3169: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3173=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3173[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3174; _temp3174.tag= Cyc_TypeList_tok; _temp3174.f1= 0; _temp3174;});
_temp3173;}); break; case 167: _LL3172: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 168: _LL3175: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3177=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3177[ 0]=({ struct Cyc_TypeList_tok_struct _temp3178; _temp3178.tag= Cyc_TypeList_tok;
_temp3178.f1=({ struct Cyc_List_List* _temp3179=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3179->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp3179->tl= 0; _temp3179;}); _temp3178;}); _temp3177;}); break; case 169:
_LL3176: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err(({ unsigned char* _temp3181=(
unsigned char*)"expecing effect kind (E)"; struct _tagged_string _temp3182;
_temp3182.curr= _temp3181; _temp3182.base= _temp3181; _temp3182.last_plus_one=
_temp3181 + 25; _temp3182;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3183=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3183[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3184; _temp3184.tag= Cyc_TypeList_tok; _temp3184.f1=({ struct Cyc_List_List*
_temp3185=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3185->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp3185->tl= 0; _temp3185;});
_temp3184;}); _temp3183;}); break; case 170: _LL3180: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp3187=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3187[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3188; _temp3188.tag= Cyc_TypeList_tok; _temp3188.f1=({ struct Cyc_List_List*
_temp3189=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3189->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3190=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3190[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3191; _temp3191.tag= Cyc_Absyn_AccessEff;
_temp3191.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3191;}); _temp3190;}));
_temp3189->tl= 0; _temp3189;}); _temp3188;}); _temp3187;}); break; case 171:
_LL3186: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3193=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3193[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3194; _temp3194.tag= Cyc_TypeList_tok; _temp3194.f1=({ struct Cyc_List_List*
_temp3195=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3195->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3196=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3196[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3197; _temp3197.tag= Cyc_Absyn_AccessEff;
_temp3197.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3197;}); _temp3196;}));
_temp3195->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3195;}); _temp3194;}); _temp3193;}); break; case 172:
_LL3192: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ unsigned char* _temp3199=(
unsigned char*)"expecting region kind (R)"; struct _tagged_string _temp3200;
_temp3200.curr= _temp3199; _temp3200.base= _temp3199; _temp3200.last_plus_one=
_temp3199 + 26; _temp3200;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3201=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3201[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3202; _temp3202.tag= Cyc_TypeList_tok; _temp3202.f1=({ struct Cyc_List_List*
_temp3203=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3203->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3204=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3204[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3205; _temp3205.tag= Cyc_Absyn_AccessEff;
_temp3205.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3205;}); _temp3204;}));
_temp3203->tl= 0; _temp3203;}); _temp3202;}); _temp3201;}); break; case 173:
_LL3198: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ unsigned char* _temp3207=(
unsigned char*)"expecting region kind (R)"; struct _tagged_string _temp3208;
_temp3208.curr= _temp3207; _temp3208.base= _temp3207; _temp3208.last_plus_one=
_temp3207 + 26; _temp3208;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3209=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3209[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3210; _temp3210.tag= Cyc_TypeList_tok; _temp3210.f1=({ struct Cyc_List_List*
_temp3211=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3211->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3212=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3212[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3213; _temp3213.tag= Cyc_Absyn_AccessEff;
_temp3213.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3213;}); _temp3212;}));
_temp3211->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3211;}); _temp3210;}); _temp3209;}); break; case 174:
_LL3206: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3215=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3215[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3216; _temp3216.tag=
Cyc_ParamDeclList_tok; _temp3216.f1=({ struct Cyc_List_List* _temp3217=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3217->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3217->tl= 0; _temp3217;}); _temp3216;}); _temp3215;}); break; case 175:
_LL3214: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3219=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3219[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3220; _temp3220.tag=
Cyc_ParamDeclList_tok; _temp3220.f1=({ struct Cyc_List_List* _temp3221=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3221->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3221->tl= Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3221;}); _temp3220;}); _temp3219;}); break; case 176:
_LL3218: { void* _temp3223= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3224; _LL3224: { struct Cyc_List_List* _temp3225=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3; goto
_LL3226; _LL3226: { struct Cyc_Absyn_Tqual _temp3227=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f1; goto
_LL3228; _LL3228: { struct Cyc_List_List* _temp3229=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms; goto
_LL3230; _LL3230: { struct _tuple6 _temp3231= Cyc_Parse_apply_tms( _temp3227,
_temp3223, _temp3225, _temp3229); goto _LL3232; _LL3232: if( _temp3231.f3 != 0){
Cyc_Parse_err(({ unsigned char* _temp3233=( unsigned char*)"parameter with bad type params";
struct _tagged_string _temp3234; _temp3234.curr= _temp3233; _temp3234.base=
_temp3233; _temp3234.last_plus_one= _temp3233 + 31; _temp3234;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct _tuple1* _temp3235=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->id; goto _LL3236; _LL3236:{ void* _temp3237=(*
_temp3235).f1; struct Cyc_List_List* _temp3247; struct Cyc_List_List* _temp3249;
_LL3239: if( _temp3237 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3240;} else{ goto
_LL3241;} _LL3241: if(( unsigned int) _temp3237 > 1u?*(( int*) _temp3237) == Cyc_Absyn_Rel_n:
0){ _LL3248: _temp3247=(( struct Cyc_Absyn_Rel_n_struct*) _temp3237)->f1; if(
_temp3247 == 0){ goto _LL3242;} else{ goto _LL3243;}} else{ goto _LL3243;}
_LL3243: if(( unsigned int) _temp3237 > 1u?*(( int*) _temp3237) == Cyc_Absyn_Abs_n:
0){ _LL3250: _temp3249=(( struct Cyc_Absyn_Abs_n_struct*) _temp3237)->f1; if(
_temp3249 == 0){ goto _LL3244;} else{ goto _LL3245;}} else{ goto _LL3245;}
_LL3245: goto _LL3246; _LL3240: goto _LL3238; _LL3242: goto _LL3238; _LL3244:
goto _LL3238; _LL3246: Cyc_Parse_err(({ unsigned char* _temp3251=( unsigned char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3252; _temp3252.curr= _temp3251; _temp3252.base=
_temp3251; _temp3252.last_plus_one= _temp3251 + 49; _temp3252;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3238; _LL3238:;}{ struct Cyc_Core_Opt* _temp3254=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3253=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3253->v=( void*)(* _temp3235).f2; _temp3253;}); goto
_LL3255; _LL3255: if( _temp3231.f4 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3256=( unsigned char*)"extra attributes on parameter, ignoring"; struct
_tagged_string _temp3257; _temp3257.curr= _temp3256; _temp3257.base= _temp3256;
_temp3257.last_plus_one= _temp3256 + 40; _temp3257;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3231.f3 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3258=(
unsigned char*)"extra type variables on parameter, ignoring"; struct
_tagged_string _temp3259; _temp3259.curr= _temp3258; _temp3259.base= _temp3258;
_temp3259.last_plus_one= _temp3258 + 44; _temp3259;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3260=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3260[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3261; _temp3261.tag= Cyc_ParamDecl_tok; _temp3261.f1=({
struct _tuple2* _temp3262=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3262->f1= _temp3254; _temp3262->f2= _temp3231.f1; _temp3262->f3= _temp3231.f2;
_temp3262;}); _temp3261;}); _temp3260;}); break;}}}}}}} case 177: _LL3222: {
void* _temp3264= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3265; _LL3265: { struct Cyc_List_List* _temp3266=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3267; _LL3267: { struct Cyc_Absyn_Tqual _temp3268=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; goto
_LL3269; _LL3269: if( _temp3266 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3270=( unsigned char*)"bad attributes on parameter, ignoring"; struct
_tagged_string _temp3271; _temp3271.curr= _temp3270; _temp3271.base= _temp3270;
_temp3271.last_plus_one= _temp3270 + 38; _temp3271;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3272=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3272[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3273; _temp3273.tag= Cyc_ParamDecl_tok; _temp3273.f1=({
struct _tuple2* _temp3274=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3274->f1= 0; _temp3274->f2= _temp3268; _temp3274->f3= _temp3264; _temp3274;});
_temp3273;}); _temp3272;}); break;}}} case 178: _LL3263: { void* _temp3276= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3277; _LL3277: { struct Cyc_List_List*
_temp3278=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3279; _LL3279: { struct Cyc_Absyn_Tqual
_temp3280=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3281; _LL3281: { struct Cyc_List_List* _temp3282=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3283; _LL3283: { struct _tuple6 _temp3284= Cyc_Parse_apply_tms(
_temp3280, _temp3276, _temp3278, _temp3282); goto _LL3285; _LL3285: if(
_temp3284.f3 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3286=( unsigned char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp3287; _temp3287.curr= _temp3286; _temp3287.base=
_temp3286; _temp3287.last_plus_one= _temp3286 + 49; _temp3287;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3284.f4 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3288=(
unsigned char*)"bad attributes on parameter, ignoring"; struct _tagged_string
_temp3289; _temp3289.curr= _temp3288; _temp3289.base= _temp3288; _temp3289.last_plus_one=
_temp3288 + 38; _temp3289;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp3290=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3290[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3291; _temp3291.tag= Cyc_ParamDecl_tok; _temp3291.f1=({
struct _tuple2* _temp3292=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3292->f1= 0; _temp3292->f2= _temp3284.f1; _temp3292->f3= _temp3284.f2;
_temp3292;}); _temp3291;}); _temp3290;}); break;}}}}} case 179: _LL3275: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp3294=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp3294[ 0]=({ struct Cyc_IdList_tok_struct
_temp3295; _temp3295.tag= Cyc_IdList_tok; _temp3295.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3295;}); _temp3294;}); break; case
180: _LL3293: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp3297=( struct
Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp3297[ 0]=({ struct Cyc_IdList_tok_struct _temp3298; _temp3298.tag= Cyc_IdList_tok;
_temp3298.f1=({ struct Cyc_List_List* _temp3299=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3299->hd=( void*)({ struct
_tagged_string* _temp3300=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3300[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3300;}); _temp3299->tl= 0; _temp3299;});
_temp3298;}); _temp3297;}); break; case 181: _LL3296: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp3302=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp3302[ 0]=({ struct Cyc_IdList_tok_struct
_temp3303; _temp3303.tag= Cyc_IdList_tok; _temp3303.f1=({ struct Cyc_List_List*
_temp3304=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3304->hd=( void*)({ struct _tagged_string* _temp3305=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3305[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3305;}); _temp3304->tl= Cyc_yyget_IdList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3304;}); _temp3303;}); _temp3302;});
break; case 182: _LL3301: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 183: _LL3306: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 184: _LL3307: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3309=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3309[ 0]=({ struct Cyc_Exp_tok_struct _temp3310; _temp3310.tag= Cyc_Exp_tok;
_temp3310.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3311=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3311[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3312; _temp3312.tag= Cyc_Absyn_UnresolvedMem_e; _temp3312.f1= 0; _temp3312.f2=
0; _temp3312;}); _temp3311;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3310;});
_temp3309;}); break; case 185: _LL3308: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3314=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3314[ 0]=({ struct Cyc_Exp_tok_struct _temp3315; _temp3315.tag= Cyc_Exp_tok;
_temp3315.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3316=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3316[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3317; _temp3317.tag= Cyc_Absyn_UnresolvedMem_e; _temp3317.f1= 0; _temp3317.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3317;});
_temp3316;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3315;});
_temp3314;}); break; case 186: _LL3313: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3319=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3319[ 0]=({ struct Cyc_Exp_tok_struct _temp3320; _temp3320.tag= Cyc_Exp_tok;
_temp3320.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3321=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3321[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3322; _temp3322.tag= Cyc_Absyn_UnresolvedMem_e; _temp3322.f1= 0; _temp3322.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3322;});
_temp3321;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3320;});
_temp3319;}); break; case 187: _LL3318: { struct Cyc_Absyn_Vardecl* _temp3326=
Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3324=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp3324->f1=( void*) Cyc_Absyn_Loc_n; _temp3324->f2=({
struct _tagged_string* _temp3325=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3325[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3325;}); _temp3324;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).last_line)));
goto _LL3327; _LL3327: _temp3326->tq=({ struct Cyc_Absyn_Tqual _temp3328;
_temp3328.q_const= 1; _temp3328.q_volatile= 0; _temp3328.q_restrict= 1;
_temp3328;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3329=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3329[ 0]=({ struct Cyc_Exp_tok_struct
_temp3330; _temp3330.tag= Cyc_Exp_tok; _temp3330.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp3331=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp3331[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp3332; _temp3332.tag= Cyc_Absyn_Comprehension_e;
_temp3332.f1= _temp3326; _temp3332.f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3332.f3= Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3332;}); _temp3331;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3330;}); _temp3329;}); break;} case 188: _LL3323: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3334=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3334[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3335; _temp3335.tag= Cyc_InitializerList_tok;
_temp3335.f1=({ struct Cyc_List_List* _temp3336=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3336->hd=( void*)({ struct
_tuple20* _temp3337=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp3337->f1= 0; _temp3337->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3337;}); _temp3336->tl= 0; _temp3336;});
_temp3335;}); _temp3334;}); break; case 189: _LL3333: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3339=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3339[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3340; _temp3340.tag= Cyc_InitializerList_tok;
_temp3340.f1=({ struct Cyc_List_List* _temp3341=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3341->hd=( void*)({ struct
_tuple20* _temp3342=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp3342->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3342->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3342;}); _temp3341->tl= 0; _temp3341;});
_temp3340;}); _temp3339;}); break; case 190: _LL3338: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3344=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3344[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3345; _temp3345.tag= Cyc_InitializerList_tok;
_temp3345.f1=({ struct Cyc_List_List* _temp3346=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3346->hd=( void*)({ struct
_tuple20* _temp3347=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp3347->f1= 0; _temp3347->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3347;}); _temp3346->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3346;});
_temp3345;}); _temp3344;}); break; case 191: _LL3343: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3349=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3349[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3350; _temp3350.tag= Cyc_InitializerList_tok;
_temp3350.f1=({ struct Cyc_List_List* _temp3351=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3351->hd=( void*)({ struct
_tuple20* _temp3352=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp3352->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3352->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3352;}); _temp3351->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3351;});
_temp3350;}); _temp3349;}); break; case 192: _LL3348: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3354=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3354[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3355; _temp3355.tag= Cyc_DesignatorList_tok;
_temp3355.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3355;}); _temp3354;}); break; case 193: _LL3353: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3357=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3357[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3358; _temp3358.tag= Cyc_DesignatorList_tok;
_temp3358.f1=({ struct Cyc_List_List* _temp3359=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3359->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3359->tl=
0; _temp3359;}); _temp3358;}); _temp3357;}); break; case 194: _LL3356: yyval=(
void*)({ struct Cyc_DesignatorList_tok_struct* _temp3361=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3361[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3362; _temp3362.tag= Cyc_DesignatorList_tok;
_temp3362.f1=({ struct Cyc_List_List* _temp3363=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3363->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3363->tl=
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3363;}); _temp3362;}); _temp3361;}); break; case 195: _LL3360: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp3365=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp3365[ 0]=({ struct
Cyc_Designator_tok_struct _temp3366; _temp3366.tag= Cyc_Designator_tok;
_temp3366.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp3367=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp3367[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp3368; _temp3368.tag=
Cyc_Absyn_ArrayElement; _temp3368.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3368;}); _temp3367;})); _temp3366;});
_temp3365;}); break; case 196: _LL3364: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp3370=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp3370[ 0]=({ struct Cyc_Designator_tok_struct _temp3371; _temp3371.tag= Cyc_Designator_tok;
_temp3371.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3372=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3372[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3373; _temp3373.tag= Cyc_Absyn_FieldName;
_temp3373.f1=({ struct _tagged_string* _temp3374=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3374[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3374;});
_temp3373;}); _temp3372;})); _temp3371;}); _temp3370;}); break; case 197:
_LL3369: { void* _temp3376= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3377; _LL3377: { struct Cyc_List_List* _temp3378=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3379; _LL3379: if( _temp3378 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3380=( unsigned char*)"ignoring attributes in type"; struct _tagged_string
_temp3381; _temp3381.curr= _temp3380; _temp3381.base= _temp3380; _temp3381.last_plus_one=
_temp3380 + 28; _temp3381;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{ struct Cyc_Absyn_Tqual
_temp3382=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; goto _LL3383; _LL3383: yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp3384=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp3384[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp3385; _temp3385.tag= Cyc_ParamDecl_tok;
_temp3385.f1=({ struct _tuple2* _temp3386=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp3386[ 0]=({ struct _tuple2 _temp3387; _temp3387.f1= 0;
_temp3387.f2= _temp3382; _temp3387.f3= _temp3376; _temp3387;}); _temp3386;});
_temp3385;}); _temp3384;}); break;}}} case 198: _LL3375: { void* _temp3389= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3390; _LL3390: { struct Cyc_List_List*
_temp3391=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3392; _LL3392: { struct Cyc_Absyn_Tqual
_temp3393=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3394; _LL3394: { struct Cyc_List_List* _temp3395=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3396; _LL3396: { struct _tuple6 _temp3397= Cyc_Parse_apply_tms(
_temp3393, _temp3389, _temp3391, _temp3395); goto _LL3398; _LL3398: if(
_temp3397.f3 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3399=( unsigned char*)"bad type params, ignoring";
struct _tagged_string _temp3400; _temp3400.curr= _temp3399; _temp3400.base=
_temp3399; _temp3400.last_plus_one= _temp3399 + 26; _temp3400;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3397.f4 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3401=(
unsigned char*)"bad specifiers, ignoring"; struct _tagged_string _temp3402;
_temp3402.curr= _temp3401; _temp3402.base= _temp3401; _temp3402.last_plus_one=
_temp3401 + 25; _temp3402;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp3403=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3403[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3404; _temp3404.tag= Cyc_ParamDecl_tok; _temp3404.f1=({
struct _tuple2* _temp3405=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3405->f1= 0; _temp3405->f2= _temp3397.f1; _temp3405->f3= _temp3397.f2;
_temp3405;}); _temp3404;}); _temp3403;}); break;}}}}} case 199: _LL3388: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp3407=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3407[ 0]=({ struct Cyc_Type_tok_struct
_temp3408; _temp3408.tag= Cyc_Type_tok; _temp3408.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; _temp3408;});
_temp3407;}); break; case 200: _LL3406: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp3410=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3410[ 0]=({ struct Cyc_Type_tok_struct _temp3411; _temp3411.tag= Cyc_Type_tok;
_temp3411.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3412=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3412[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3413; _temp3413.tag= Cyc_Absyn_JoinEff;
_temp3413.f1= 0; _temp3413;}); _temp3412;})); _temp3411;}); _temp3410;}); break;
case 201: _LL3409: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3415=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3415[ 0]=({ struct Cyc_Type_tok_struct _temp3416; _temp3416.tag= Cyc_Type_tok;
_temp3416.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3417=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3417[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3418; _temp3418.tag= Cyc_Absyn_JoinEff;
_temp3418.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3418;}); _temp3417;})); _temp3416;}); _temp3415;});
break; case 202: _LL3414: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3420=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3420[ 0]=({ struct Cyc_Type_tok_struct _temp3421; _temp3421.tag= Cyc_Type_tok;
_temp3421.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3422=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3422[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3423; _temp3423.tag= Cyc_Absyn_JoinEff;
_temp3423.f1=({ struct Cyc_List_List* _temp3424=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3424->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3424->tl=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3424;}); _temp3423;}); _temp3422;})); _temp3421;}); _temp3420;}); break;
case 203: _LL3419: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3426=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3426[ 0]=({ struct Cyc_TypeList_tok_struct _temp3427; _temp3427.tag= Cyc_TypeList_tok;
_temp3427.f1=({ struct Cyc_List_List* _temp3428=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3428->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3428->tl=
0; _temp3428;}); _temp3427;}); _temp3426;}); break; case 204: _LL3425: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp3430=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3430[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3431; _temp3431.tag= Cyc_TypeList_tok; _temp3431.f1=({ struct Cyc_List_List*
_temp3432=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3432->hd=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3432->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3432;}); _temp3431;}); _temp3430;});
break; case 205: _LL3429: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3434=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3434[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3435; _temp3435.tag= Cyc_AbstractDeclarator_tok; _temp3435.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3436=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3436->tms= Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3436;}); _temp3435;}); _temp3434;}); break;
case 206: _LL3433: yyval=*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 207: _LL3437: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3439=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3439[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3440; _temp3440.tag= Cyc_AbstractDeclarator_tok; _temp3440.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3441=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3441->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tms); _temp3441;}); _temp3440;});
_temp3439;}); break; case 208: _LL3438: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 209: _LL3442: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3444=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3444[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3445; _temp3445.tag= Cyc_AbstractDeclarator_tok; _temp3445.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3446=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3446->tms=({ struct Cyc_List_List* _temp3447=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3447->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3447->tl= 0; _temp3447;}); _temp3446;}); _temp3445;}); _temp3444;}); break;
case 210: _LL3443: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3449=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3449[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3450; _temp3450.tag= Cyc_AbstractDeclarator_tok; _temp3450.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3451=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3451->tms=({ struct Cyc_List_List* _temp3452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3452->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3452->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)))->tms; _temp3452;}); _temp3451;}); _temp3450;});
_temp3449;}); break; case 211: _LL3448: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3454=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3454[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3455; _temp3455.tag= Cyc_AbstractDeclarator_tok; _temp3455.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3456=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3456->tms=({ struct Cyc_List_List* _temp3457=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3457->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3458=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3458[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3459; _temp3459.tag= Cyc_Absyn_ConstArray_mod;
_temp3459.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3459;}); _temp3458;})); _temp3457->tl= 0; _temp3457;}); _temp3456;});
_temp3455;}); _temp3454;}); break; case 212: _LL3453: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3461=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3461[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3462; _temp3462.tag= Cyc_AbstractDeclarator_tok;
_temp3462.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3463=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3463->tms=({
struct Cyc_List_List* _temp3464=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3464->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3465=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3465[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3466; _temp3466.tag= Cyc_Absyn_ConstArray_mod; _temp3466.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3466;});
_temp3465;})); _temp3464->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp3464;}); _temp3463;});
_temp3462;}); _temp3461;}); break; case 213: _LL3460: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3468=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3468[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3469; _temp3469.tag= Cyc_AbstractDeclarator_tok;
_temp3469.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3470=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3470->tms=({
struct Cyc_List_List* _temp3471=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3471->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3472=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3472[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3473; _temp3473.tag=
Cyc_Absyn_Function_mod; _temp3473.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3474=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3474[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3475; _temp3475.tag= Cyc_Absyn_WithTypes;
_temp3475.f1= 0; _temp3475.f2= 0; _temp3475.f3= 0; _temp3475.f4= Cyc_yyget_TypeOpt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3475.f5=
Cyc_yyget_Rgnorder_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3475;}); _temp3474;})); _temp3473;}); _temp3472;})); _temp3471->tl= 0;
_temp3471;}); _temp3470;}); _temp3469;}); _temp3468;}); break; case 214: _LL3467: {
struct _tuple17 _temp3479; struct Cyc_List_List* _temp3480; struct Cyc_Core_Opt*
_temp3482; struct Cyc_Absyn_VarargInfo* _temp3484; int _temp3486; struct Cyc_List_List*
_temp3488; struct _tuple17* _temp3477= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3479=* _temp3477; _LL3489:
_temp3488= _temp3479.f1; goto _LL3487; _LL3487: _temp3486= _temp3479.f2; goto
_LL3485; _LL3485: _temp3484= _temp3479.f3; goto _LL3483; _LL3483: _temp3482=
_temp3479.f4; goto _LL3481; _LL3481: _temp3480= _temp3479.f5; goto _LL3478;
_LL3478: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3490=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3490[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3491; _temp3491.tag=
Cyc_AbstractDeclarator_tok; _temp3491.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3492=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3492->tms=({ struct Cyc_List_List* _temp3493=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3493->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3494=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3494[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3495; _temp3495.tag= Cyc_Absyn_Function_mod;
_temp3495.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3496=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3496[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3497; _temp3497.tag= Cyc_Absyn_WithTypes;
_temp3497.f1= _temp3488; _temp3497.f2= _temp3486; _temp3497.f3= _temp3484;
_temp3497.f4= _temp3482; _temp3497.f5= _temp3480; _temp3497;}); _temp3496;}));
_temp3495;}); _temp3494;})); _temp3493->tl= 0; _temp3493;}); _temp3492;});
_temp3491;}); _temp3490;}); break;} case 215: _LL3476: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3499=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3499[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3500; _temp3500.tag= Cyc_AbstractDeclarator_tok;
_temp3500.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3501=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3501->tms=({
struct Cyc_List_List* _temp3502=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3502->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3503=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3503[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3504; _temp3504.tag=
Cyc_Absyn_Function_mod; _temp3504.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3505=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3505[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3506; _temp3506.tag= Cyc_Absyn_WithTypes;
_temp3506.f1= 0; _temp3506.f2= 0; _temp3506.f3= 0; _temp3506.f4= Cyc_yyget_TypeOpt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3506.f5=
Cyc_yyget_Rgnorder_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3506;}); _temp3505;})); _temp3504;}); _temp3503;})); _temp3502->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp3502;}); _temp3501;}); _temp3500;}); _temp3499;}); break; case 216: _LL3498: {
struct _tuple17 _temp3510; struct Cyc_List_List* _temp3511; struct Cyc_Core_Opt*
_temp3513; struct Cyc_Absyn_VarargInfo* _temp3515; int _temp3517; struct Cyc_List_List*
_temp3519; struct _tuple17* _temp3508= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3510=* _temp3508; _LL3520:
_temp3519= _temp3510.f1; goto _LL3518; _LL3518: _temp3517= _temp3510.f2; goto
_LL3516; _LL3516: _temp3515= _temp3510.f3; goto _LL3514; _LL3514: _temp3513=
_temp3510.f4; goto _LL3512; _LL3512: _temp3511= _temp3510.f5; goto _LL3509;
_LL3509: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3521=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3521[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3522; _temp3522.tag=
Cyc_AbstractDeclarator_tok; _temp3522.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3523=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3523->tms=({ struct Cyc_List_List* _temp3524=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3524->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3525=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3525[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3526; _temp3526.tag= Cyc_Absyn_Function_mod;
_temp3526.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3527=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3527[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3528; _temp3528.tag= Cyc_Absyn_WithTypes;
_temp3528.f1= _temp3519; _temp3528.f2= _temp3517; _temp3528.f3= _temp3515;
_temp3528.f4= _temp3513; _temp3528.f5= _temp3511; _temp3528;}); _temp3527;}));
_temp3526;}); _temp3525;})); _temp3524->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3524;}); _temp3523;}); _temp3522;}); _temp3521;}); break;} case 217:
_LL3507: { struct Cyc_List_List* _temp3530=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
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
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3537.f3= 0; _temp3537;}); _temp3536;})); _temp3535->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3535;}); _temp3534;}); _temp3533;}); _temp3532;}); break;} case 218:
_LL3529:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3539=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3540; _LL3540: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3541=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3541[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3542; _temp3542.tag= Cyc_AbstractDeclarator_tok; _temp3542.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3543=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3543->tms=({ struct Cyc_List_List* _temp3544=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3544->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3545=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3545[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3546; _temp3546.tag= Cyc_Absyn_TypeParams_mod;
_temp3546.f1= _temp3539; _temp3546.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3546.f3= 0; _temp3546;}); _temp3545;})); _temp3544->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3544;}); _temp3543;}); _temp3542;}); _temp3541;}); break;} case 219:
_LL3538: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3548=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3548[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3549; _temp3549.tag=
Cyc_AbstractDeclarator_tok; _temp3549.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3550=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3550->tms=({ struct Cyc_List_List* _temp3551=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3551->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3552=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3552[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3553; _temp3553.tag= Cyc_Absyn_Attributes_mod;
_temp3553.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3553.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3553;});
_temp3552;})); _temp3551->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))->tms; _temp3551;}); _temp3550;});
_temp3549;}); _temp3548;}); break; case 220: _LL3547: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 221: _LL3554: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 222: _LL3555: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 223: _LL3556: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 224: _LL3557: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 225: _LL3558: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 226: _LL3559: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),({
unsigned char* _temp3561=( unsigned char*)"`H"; struct _tagged_string _temp3562;
_temp3562.curr= _temp3561; _temp3562.base= _temp3561; _temp3562.last_plus_one=
_temp3561 + 3; _temp3562;})) == 0){ Cyc_Parse_err(({ unsigned char* _temp3563=(
unsigned char*)"bad occurrence of heap region `H"; struct _tagged_string
_temp3564; _temp3564.curr= _temp3563; _temp3564.base= _temp3563; _temp3564.last_plus_one=
_temp3563 + 33; _temp3564;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));}{ struct Cyc_Absyn_Tvar*
tv=({ struct Cyc_Absyn_Tvar* _temp3575=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp3575->name=({ struct _tagged_string*
_temp3576=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp3576[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp3576;}); _temp3575->identity= 0; _temp3575->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind);
_temp3575;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp3573=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3573[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3574; _temp3574.tag= Cyc_Absyn_VarType;
_temp3574.f1= tv; _temp3574;}); _temp3573;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3565=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3565[ 0]=({ struct Cyc_Stmt_tok_struct _temp3566; _temp3566.tag= Cyc_Stmt_tok;
_temp3566.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3567=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3567[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3568; _temp3568.tag= Cyc_Absyn_Region_s;
_temp3568.f1= tv; _temp3568.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3569=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3569->f1=(
void*) Cyc_Absyn_Loc_n; _temp3569->f2=({ struct _tagged_string* _temp3570=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3570[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3570;}); _temp3569;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3571=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3571[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3572; _temp3572.tag= Cyc_Absyn_RgnHandleType; _temp3572.f1=( void*) t;
_temp3572;}); _temp3571;}), 0); _temp3568.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3568;}); _temp3567;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3566;}); _temp3565;}); break;} case 227: _LL3560: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),({
unsigned char* _temp3578=( unsigned char*)"H"; struct _tagged_string _temp3579;
_temp3579.curr= _temp3578; _temp3579.base= _temp3578; _temp3579.last_plus_one=
_temp3578 + 2; _temp3579;})) == 0){ Cyc_Parse_err(({ unsigned char* _temp3580=(
unsigned char*)"bad occurrence of heap region `H"; struct _tagged_string
_temp3581; _temp3581.curr= _temp3580; _temp3581.base= _temp3580; _temp3581.last_plus_one=
_temp3580 + 33; _temp3581;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{ struct Cyc_Absyn_Tvar*
tv=({ struct Cyc_Absyn_Tvar* _temp3592=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp3592->name=({ struct _tagged_string*
_temp3593=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp3593[ 0]=( struct _tagged_string)({ struct _tagged_string _temp3594=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
xprintf("`%.*s", _temp3594.last_plus_one - _temp3594.curr, _temp3594.curr);});
_temp3593;}); _temp3592->identity= 0; _temp3592->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp3592;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp3590=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3590[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3591; _temp3591.tag= Cyc_Absyn_VarType; _temp3591.f1=
tv; _temp3591;}); _temp3590;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3582=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3582[ 0]=({ struct Cyc_Stmt_tok_struct _temp3583; _temp3583.tag= Cyc_Stmt_tok;
_temp3583.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3584=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3584[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3585; _temp3585.tag= Cyc_Absyn_Region_s;
_temp3585.f1= tv; _temp3585.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3586=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3586->f1=(
void*) Cyc_Absyn_Loc_n; _temp3586->f2=({ struct _tagged_string* _temp3587=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3587[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3587;}); _temp3586;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3588=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3588[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3589; _temp3589.tag= Cyc_Absyn_RgnHandleType; _temp3589.f1=( void*) t;
_temp3589;}); _temp3588;}), 0); _temp3585.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3585;}); _temp3584;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3583;}); _temp3582;}); break;} case 228: _LL3577: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3596=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3596[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3597; _temp3597.tag= Cyc_Stmt_tok; _temp3597.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp3598=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp3598[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp3599; _temp3599.tag= Cyc_Absyn_Cut_s; _temp3599.f1= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3599;});
_temp3598;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3597;});
_temp3596;}); break; case 229: _LL3595: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3601=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3601[ 0]=({ struct Cyc_Stmt_tok_struct _temp3602; _temp3602.tag= Cyc_Stmt_tok;
_temp3602.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp3603=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp3603[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp3604; _temp3604.tag= Cyc_Absyn_Splice_s;
_temp3604.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3604;}); _temp3603;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3602;}); _temp3601;}); break; case 230: _LL3600: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3606=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3606[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3607; _temp3607.tag= Cyc_Stmt_tok; _temp3607.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp3608=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp3608[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp3609; _temp3609.tag= Cyc_Absyn_Label_s; _temp3609.f1=({
struct _tagged_string* _temp3610=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3610[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3610;}); _temp3609.f2= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3609;});
_temp3608;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3607;});
_temp3606;}); break; case 231: _LL3605: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3612=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3612[ 0]=({ struct Cyc_Stmt_tok_struct _temp3613; _temp3613.tag= Cyc_Stmt_tok;
_temp3613.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3613;});
_temp3612;}); break; case 232: _LL3611: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3615=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3615[ 0]=({ struct Cyc_Stmt_tok_struct _temp3616; _temp3616.tag= Cyc_Stmt_tok;
_temp3616.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3616;});
_temp3615;}); break; case 233: _LL3614: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3618=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3618[ 0]=({ struct Cyc_Stmt_tok_struct _temp3619; _temp3619.tag= Cyc_Stmt_tok;
_temp3619.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3619;});
_temp3618;}); break; case 234: _LL3617: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 235: _LL3620:{ void* _temp3622= Cyc_yyget_BlockItem_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); struct Cyc_List_List*
_temp3630; struct Cyc_Absyn_Fndecl* _temp3632; struct Cyc_Absyn_Stmt* _temp3634;
_LL3624: if(*(( int*) _temp3622) == Cyc_Parse_TopDecls_bl){ _LL3631: _temp3630=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp3622)->f1; goto _LL3625;} else{ goto
_LL3626;} _LL3626: if(*(( int*) _temp3622) == Cyc_Parse_FnDecl_bl){ _LL3633:
_temp3632=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp3622)->f1; goto _LL3627;}
else{ goto _LL3628;} _LL3628: if(*(( int*) _temp3622) == Cyc_Parse_Stmt_bl){
_LL3635: _temp3634=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3622)->f1; goto
_LL3629;} else{ goto _LL3623;} _LL3625: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3636=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3636[ 0]=({ struct Cyc_Stmt_tok_struct _temp3637; _temp3637.tag= Cyc_Stmt_tok;
_temp3637.f1= Cyc_Parse_flatten_declarations( _temp3630, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line))); _temp3637;}); _temp3636;}); goto _LL3623; _LL3627:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3638=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3638[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3639; _temp3639.tag= Cyc_Stmt_tok; _temp3639.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp3640=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3640[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3641; _temp3641.tag= Cyc_Absyn_Fn_d;
_temp3641.f1= _temp3632; _temp3641;}); _temp3640;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line))); _temp3639;});
_temp3638;}); goto _LL3623; _LL3629: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3642=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3642[ 0]=({ struct Cyc_Stmt_tok_struct _temp3643; _temp3643.tag= Cyc_Stmt_tok;
_temp3643.f1= _temp3634; _temp3643;}); _temp3642;}); goto _LL3623; _LL3623:;}
break; case 236: _LL3621:{ void* _temp3645= Cyc_yyget_BlockItem_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); struct Cyc_List_List* _temp3653;
struct Cyc_Absyn_Fndecl* _temp3655; struct Cyc_Absyn_Stmt* _temp3657; _LL3647:
if(*(( int*) _temp3645) == Cyc_Parse_TopDecls_bl){ _LL3654: _temp3653=(( struct
Cyc_Parse_TopDecls_bl_struct*) _temp3645)->f1; goto _LL3648;} else{ goto _LL3649;}
_LL3649: if(*(( int*) _temp3645) == Cyc_Parse_FnDecl_bl){ _LL3656: _temp3655=((
struct Cyc_Parse_FnDecl_bl_struct*) _temp3645)->f1; goto _LL3650;} else{ goto
_LL3651;} _LL3651: if(*(( int*) _temp3645) == Cyc_Parse_Stmt_bl){ _LL3658:
_temp3657=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3645)->f1; goto _LL3652;}
else{ goto _LL3646;} _LL3648: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3659=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3659[ 0]=({ struct Cyc_Stmt_tok_struct _temp3660; _temp3660.tag= Cyc_Stmt_tok;
_temp3660.f1= Cyc_Parse_flatten_declarations( _temp3653, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp3660;});
_temp3659;}); goto _LL3646; _LL3650: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3661=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3661[ 0]=({ struct Cyc_Stmt_tok_struct _temp3662; _temp3662.tag= Cyc_Stmt_tok;
_temp3662.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp3663=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3663[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3664; _temp3664.tag= Cyc_Absyn_Fn_d;
_temp3664.f1= _temp3655; _temp3664;}); _temp3663;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3662;}); _temp3661;}); goto _LL3646; _LL3652: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3665=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3665[ 0]=({ struct Cyc_Stmt_tok_struct _temp3666; _temp3666.tag= Cyc_Stmt_tok;
_temp3666.f1= Cyc_Absyn_seq_stmt( _temp3657, Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3666;}); _temp3665;}); goto _LL3646; _LL3646:;} break; case 237: _LL3644:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3668=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp3668[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp3669; _temp3669.tag= Cyc_BlockItem_tok; _temp3669.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp3670=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp3670[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp3671; _temp3671.tag= Cyc_Parse_TopDecls_bl;
_temp3671.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3671;}); _temp3670;})); _temp3669;}); _temp3668;}); break;
case 238: _LL3667: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3673=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3673[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3674; _temp3674.tag= Cyc_BlockItem_tok;
_temp3674.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp3675=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp3675[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp3676; _temp3676.tag= Cyc_Parse_Stmt_bl;
_temp3676.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3676;}); _temp3675;})); _temp3674;}); _temp3673;}); break;
case 239: _LL3672: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3678=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3678[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3679; _temp3679.tag= Cyc_BlockItem_tok;
_temp3679.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp3680=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp3680[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp3681; _temp3681.tag= Cyc_Parse_FnDecl_bl;
_temp3681.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3681;}); _temp3680;})); _temp3679;}); _temp3678;}); break;
case 240: _LL3677: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3683=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3683[ 0]=({ struct Cyc_Stmt_tok_struct _temp3684; _temp3684.tag= Cyc_Stmt_tok;
_temp3684.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3684;}); _temp3683;}); break; case 241: _LL3682: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3686=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3686[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3687; _temp3687.tag= Cyc_Stmt_tok; _temp3687.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3687;}); _temp3686;}); break; case 242: _LL3685: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3689=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3689[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3690; _temp3690.tag= Cyc_Stmt_tok; _temp3690.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3690;}); _temp3689;}); break; case 243: _LL3688: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 6)),({
unsigned char* _temp3692=( unsigned char*)"C"; struct _tagged_string _temp3693;
_temp3693.curr= _temp3692; _temp3693.base= _temp3692; _temp3693.last_plus_one=
_temp3692 + 2; _temp3693;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp3694=(
unsigned char*)"only switch \"C\" { ... } is allowed"; struct _tagged_string
_temp3695; _temp3695.curr= _temp3694; _temp3695.base= _temp3694; _temp3695.last_plus_one=
_temp3694 + 35; _temp3695;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp3696=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp3696[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3697; _temp3697.tag= Cyc_Stmt_tok; _temp3697.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp3698=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp3698[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp3699; _temp3699.tag= Cyc_Absyn_SwitchC_s;
_temp3699.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp3699.f2= Cyc_yyget_SwitchCClauseList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3699;}); _temp3698;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3697;}); _temp3696;}); break; case 244: _LL3691: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3701=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3701[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3702; _temp3702.tag= Cyc_Stmt_tok; _temp3702.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3702;}); _temp3701;}); break; case 245: _LL3700:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3704=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3704[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3705; _temp3705.tag= Cyc_SwitchClauseList_tok;
_temp3705.f1= 0; _temp3705;}); _temp3704;}); break; case 246: _LL3703: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3707=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3707[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3708; _temp3708.tag= Cyc_SwitchClauseList_tok;
_temp3708.f1=({ struct Cyc_List_List* _temp3709=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3709->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3710=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3710->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3710->pat_vars= 0; _temp3710->where_clause= 0; _temp3710->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3710->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3710;}); _temp3709->tl= 0; _temp3709;});
_temp3708;}); _temp3707;}); break; case 247: _LL3706: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp3712=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3712[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3713; _temp3713.tag= Cyc_SwitchClauseList_tok;
_temp3713.f1=({ struct Cyc_List_List* _temp3714=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3714->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3715=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3715->pattern= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3715->pat_vars= 0; _temp3715->where_clause= 0;
_temp3715->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3715->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3715;});
_temp3714->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3714;}); _temp3713;}); _temp3712;}); break; case 248:
_LL3711: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3717=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp3717[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp3718; _temp3718.tag=
Cyc_SwitchClauseList_tok; _temp3718.f1=({ struct Cyc_List_List* _temp3719=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3719->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp3720=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3720->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3720->pat_vars=
0; _temp3720->where_clause= 0; _temp3720->body= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3720->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp3720;}); _temp3719->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3719;}); _temp3718;}); _temp3717;});
break; case 249: _LL3716: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3722=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3722[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3723; _temp3723.tag= Cyc_SwitchClauseList_tok; _temp3723.f1=({ struct Cyc_List_List*
_temp3724=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3724->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3725=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3725->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3725->pat_vars=
0; _temp3725->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3725->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3725->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3725;}); _temp3724->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3724;}); _temp3723;}); _temp3722;});
break; case 250: _LL3721: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3727=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3727[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3728; _temp3728.tag= Cyc_SwitchClauseList_tok; _temp3728.f1=({ struct Cyc_List_List*
_temp3729=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3729->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3730=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3730->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3730->pat_vars=
0; _temp3730->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3730->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3730->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3730;}); _temp3729->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3729;});
_temp3728;}); _temp3727;}); break; case 251: _LL3726: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp3732=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp3732[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp3733; _temp3733.tag= Cyc_SwitchCClauseList_tok;
_temp3733.f1= 0; _temp3733;}); _temp3732;}); break; case 252: _LL3731: yyval=(
void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp3735=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp3735[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp3736; _temp3736.tag= Cyc_SwitchCClauseList_tok;
_temp3736.f1=({ struct Cyc_List_List* _temp3737=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3737->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp3738=( struct Cyc_Absyn_SwitchC_clause*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp3738->cnst_exp= 0; _temp3738->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Absyn_break_stmt(
0), 0); _temp3738->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3738;});
_temp3737->tl= 0; _temp3737;}); _temp3736;}); _temp3735;}); break; case 253:
_LL3734: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp3740=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp3740[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp3741; _temp3741.tag=
Cyc_SwitchCClauseList_tok; _temp3741.f1=({ struct Cyc_List_List* _temp3742=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3742->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp3743=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp3743->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3743->body= Cyc_Absyn_fallthru_stmt( 0, 0); _temp3743->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3743;}); _temp3742->tl= Cyc_yyget_SwitchCClauseList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3742;});
_temp3741;}); _temp3740;}); break; case 254: _LL3739: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp3745=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp3745[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp3746; _temp3746.tag= Cyc_SwitchCClauseList_tok;
_temp3746.f1=({ struct Cyc_List_List* _temp3747=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3747->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp3748=( struct Cyc_Absyn_SwitchC_clause*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp3748->cnst_exp=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3748->body= Cyc_Absyn_seq_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Absyn_fallthru_stmt( 0, 0), 0); _temp3748->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3748;}); _temp3747->tl= Cyc_yyget_SwitchCClauseList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3747;}); _temp3746;}); _temp3745;});
break; case 255: _LL3744: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3750=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3750[ 0]=({ struct Cyc_Stmt_tok_struct _temp3751; _temp3751.tag= Cyc_Stmt_tok;
_temp3751.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3751;});
_temp3750;}); break; case 256: _LL3749: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3753=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3753[ 0]=({ struct Cyc_Stmt_tok_struct _temp3754; _temp3754.tag= Cyc_Stmt_tok;
_temp3754.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3754;});
_temp3753;}); break; case 257: _LL3752: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3756=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3756[ 0]=({ struct Cyc_Stmt_tok_struct _temp3757; _temp3757.tag= Cyc_Stmt_tok;
_temp3757.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3757;});
_temp3756;}); break; case 258: _LL3755: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3759=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3759[ 0]=({ struct Cyc_Stmt_tok_struct _temp3760; _temp3760.tag= Cyc_Stmt_tok;
_temp3760.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3760;}); _temp3759;}); break; case 259: _LL3758: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3762=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3762[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3763; _temp3763.tag= Cyc_Stmt_tok; _temp3763.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3763;});
_temp3762;}); break; case 260: _LL3761: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3765=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3765[ 0]=({ struct Cyc_Stmt_tok_struct _temp3766; _temp3766.tag= Cyc_Stmt_tok;
_temp3766.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3766;}); _temp3765;}); break; case 261: _LL3764: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3768=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3768[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3769; _temp3769.tag= Cyc_Stmt_tok; _temp3769.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3769;});
_temp3768;}); break; case 262: _LL3767: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3771=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3771[ 0]=({ struct Cyc_Stmt_tok_struct _temp3772; _temp3772.tag= Cyc_Stmt_tok;
_temp3772.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 5)), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3772;}); _temp3771;}); break; case 263: _LL3770: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3774=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3774[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3775; _temp3775.tag= Cyc_Stmt_tok; _temp3775.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 7)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3775;}); _temp3774;}); break; case 264: _LL3773:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3777=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3777[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3778; _temp3778.tag= Cyc_Stmt_tok; _temp3778.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 6)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3778;}); _temp3777;}); break; case 265: _LL3776: { struct Cyc_List_List*
_temp3780= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); goto _LL3781; _LL3781: { struct Cyc_Absyn_Stmt* _temp3782=
Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); goto _LL3783; _LL3783: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3784=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3784[ 0]=({ struct Cyc_Stmt_tok_struct _temp3785; _temp3785.tag= Cyc_Stmt_tok;
_temp3785.f1= Cyc_Parse_flatten_declarations( _temp3780, _temp3782); _temp3785;});
_temp3784;}); break;}} case 266: _LL3779: { struct Cyc_List_List* _temp3787= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); goto
_LL3788; _LL3788: { struct Cyc_Absyn_Stmt* _temp3789= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL3790;
_LL3790: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3791=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3791[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3792; _temp3792.tag= Cyc_Stmt_tok; _temp3792.f1= Cyc_Parse_flatten_declarations(
_temp3787, _temp3789); _temp3792;}); _temp3791;}); break;}} case 267: _LL3786: {
struct Cyc_List_List* _temp3794= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); goto _LL3795; _LL3795: { struct Cyc_Absyn_Stmt*
_temp3796= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3797; _LL3797: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3798=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3798[ 0]=({ struct Cyc_Stmt_tok_struct _temp3799; _temp3799.tag= Cyc_Stmt_tok;
_temp3799.f1= Cyc_Parse_flatten_declarations( _temp3794, _temp3796); _temp3799;});
_temp3798;}); break;}} case 268: _LL3793: { struct Cyc_List_List* _temp3801= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); goto
_LL3802; _LL3802: { struct Cyc_Absyn_Stmt* _temp3803= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3804; _LL3804: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3805=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3805[ 0]=({ struct Cyc_Stmt_tok_struct _temp3806; _temp3806.tag= Cyc_Stmt_tok;
_temp3806.f1= Cyc_Parse_flatten_declarations( _temp3801, _temp3803); _temp3806;});
_temp3805;}); break;}} case 269: _LL3800: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3808=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3808[ 0]=({ struct Cyc_Stmt_tok_struct _temp3809; _temp3809.tag= Cyc_Stmt_tok;
_temp3809.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp3810=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3810[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3810;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3809;});
_temp3808;}); break; case 270: _LL3807: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3812=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3812[ 0]=({ struct Cyc_Stmt_tok_struct _temp3813; _temp3813.tag= Cyc_Stmt_tok;
_temp3813.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3813;}); _temp3812;}); break; case 271: _LL3811: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3815=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3815[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3816; _temp3816.tag= Cyc_Stmt_tok; _temp3816.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3816;}); _temp3815;}); break; case 272: _LL3814: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3818=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3818[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3819; _temp3819.tag= Cyc_Stmt_tok; _temp3819.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3819;}); _temp3818;}); break; case 273:
_LL3817: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3821=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3821[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3822; _temp3822.tag= Cyc_Stmt_tok; _temp3822.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3822;});
_temp3821;}); break; case 274: _LL3820: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3824=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3824[ 0]=({ struct Cyc_Stmt_tok_struct _temp3825; _temp3825.tag= Cyc_Stmt_tok;
_temp3825.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3825;}); _temp3824;}); break; case 275: _LL3823: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3827=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3827[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3828; _temp3828.tag= Cyc_Stmt_tok; _temp3828.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).last_line)); _temp3828;}); _temp3827;}); break; case 276:
_LL3826: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3830=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3830[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3831; _temp3831.tag= Cyc_Stmt_tok; _temp3831.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).last_line)); _temp3831;}); _temp3830;}); break; case 277:
_LL3829: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3833=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3833[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3834; _temp3834.tag= Cyc_Pattern_tok; _temp3834.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3834;});
_temp3833;}); break; case 278: _LL3832: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 279: _LL3835: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3837=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3837[ 0]=({ struct Cyc_Pattern_tok_struct _temp3838; _temp3838.tag= Cyc_Pattern_tok;
_temp3838.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3839=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3839[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3840; _temp3840.tag= Cyc_Absyn_Int_p;
_temp3840.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; _temp3840.f2=(* Cyc_yyget_Int_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))).f2; _temp3840;}); _temp3839;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3838;}); _temp3837;}); break; case 280: _LL3836: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3842=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3842[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3843; _temp3843.tag= Cyc_Pattern_tok; _temp3843.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3844=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3844[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3845; _temp3845.tag= Cyc_Absyn_Int_p; _temp3845.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3845.f2= -(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp3845;}); _temp3844;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3843;}); _temp3842;}); break; case 281: _LL3841: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3847=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3847[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3848; _temp3848.tag= Cyc_Pattern_tok; _temp3848.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3849=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3849[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3850; _temp3850.tag= Cyc_Absyn_Float_p; _temp3850.f1=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3850;}); _temp3849;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3848;});
_temp3847;}); break; case 282: _LL3846: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3852=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3852[ 0]=({ struct Cyc_Pattern_tok_struct _temp3853; _temp3853.tag= Cyc_Pattern_tok;
_temp3853.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3854=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3854[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3855; _temp3855.tag= Cyc_Absyn_Char_p;
_temp3855.f1= Cyc_yyget_Char_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3855;}); _temp3854;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3853;}); _temp3852;}); break; case 283: _LL3851: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3857=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3857[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3858; _temp3858.tag= Cyc_Pattern_tok; _temp3858.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3858;});
_temp3857;}); break; case 284: _LL3856: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3860=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3860[ 0]=({ struct Cyc_Pattern_tok_struct _temp3861; _temp3861.tag= Cyc_Pattern_tok;
_temp3861.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3862=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3862[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3863; _temp3863.tag=
Cyc_Absyn_UnknownId_p; _temp3863.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3863;}); _temp3862;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3861;}); _temp3860;}); break; case 285: _LL3859: { struct Cyc_List_List*
_temp3865=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3866; _LL3866: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3867=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3867[ 0]=({ struct Cyc_Pattern_tok_struct _temp3868; _temp3868.tag= Cyc_Pattern_tok;
_temp3868.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3869=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3869[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3870; _temp3870.tag= Cyc_Absyn_UnknownCall_p; _temp3870.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3870.f2=
_temp3865; _temp3870.f3= Cyc_yyget_PatternList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3870;}); _temp3869;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3868;}); _temp3867;}); break;} case 286: _LL3864: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3872=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3872[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3873; _temp3873.tag= Cyc_Pattern_tok; _temp3873.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3874=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3874[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3875; _temp3875.tag= Cyc_Absyn_Tuple_p; _temp3875.f1=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3875;}); _temp3874;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3873;});
_temp3872;}); break; case 287: _LL3871: { struct Cyc_List_List* _temp3877=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); goto
_LL3878; _LL3878: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3879=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3879[ 0]=({ struct Cyc_Pattern_tok_struct _temp3880; _temp3880.tag= Cyc_Pattern_tok;
_temp3880.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3881=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3881[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3882; _temp3882.tag= Cyc_Absyn_UnknownFields_p; _temp3882.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3882.f2=
_temp3877; _temp3882.f3= 0; _temp3882;}); _temp3881;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3880;}); _temp3879;}); break;} case 288: _LL3876: { struct Cyc_List_List*
_temp3884=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3885; _LL3885: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3886=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3886[ 0]=({ struct Cyc_Pattern_tok_struct _temp3887; _temp3887.tag= Cyc_Pattern_tok;
_temp3887.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3888=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3888[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3889; _temp3889.tag= Cyc_Absyn_UnknownFields_p; _temp3889.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3889.f2=
_temp3884; _temp3889.f3= Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3889;}); _temp3888;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3887;}); _temp3886;}); break;} case 289: _LL3883: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3891=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3891[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3892; _temp3892.tag= Cyc_Pattern_tok; _temp3892.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3893=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3893[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3894; _temp3894.tag= Cyc_Absyn_Pointer_p;
_temp3894.f1= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3894;}); _temp3893;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3892;}); _temp3891;}); break; case 290: _LL3890: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3896=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3896[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3897; _temp3897.tag= Cyc_Pattern_tok; _temp3897.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3898=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3898[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3899; _temp3899.tag= Cyc_Absyn_Reference_p;
_temp3899.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3900=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3900->f1=( void*) Cyc_Absyn_Loc_n;
_temp3900->f2=({ struct _tagged_string* _temp3901=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3901[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3901;});
_temp3900;}),( void*) Cyc_Absyn_VoidType, 0); _temp3899;}); _temp3898;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3897;}); _temp3896;}); break; case 291: _LL3895: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3903=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3903[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3904; _temp3904.tag= Cyc_PatternList_tok;
_temp3904.f1= 0; _temp3904;}); _temp3903;}); break; case 292: _LL3902: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3906=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3906[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3907; _temp3907.tag= Cyc_PatternList_tok;
_temp3907.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3907;}); _temp3906;}); break; case 293: _LL3905: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3909=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3909[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3910; _temp3910.tag= Cyc_PatternList_tok;
_temp3910.f1=({ struct Cyc_List_List* _temp3911=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3911->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3911->tl=
0; _temp3911;}); _temp3910;}); _temp3909;}); break; case 294: _LL3908: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3913=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3913[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3914; _temp3914.tag= Cyc_PatternList_tok;
_temp3914.f1=({ struct Cyc_List_List* _temp3915=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3915->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3915->tl=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3915;}); _temp3914;}); _temp3913;}); break; case 295: _LL3912: yyval=( void*)({
struct Cyc_FieldPattern_tok_struct* _temp3917=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3917[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3918; _temp3918.tag= Cyc_FieldPattern_tok;
_temp3918.f1=({ struct _tuple14* _temp3919=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3919->f1= 0; _temp3919->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3919;});
_temp3918;}); _temp3917;}); break; case 296: _LL3916: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp3921=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3921[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3922; _temp3922.tag= Cyc_FieldPattern_tok;
_temp3922.f1=({ struct _tuple14* _temp3923=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3923->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3923->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3923;});
_temp3922;}); _temp3921;}); break; case 297: _LL3920: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3925=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3925[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3926; _temp3926.tag= Cyc_FieldPatternList_tok;
_temp3926.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3926;}); _temp3925;}); break; case 298: _LL3924: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3928=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3928[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3929; _temp3929.tag= Cyc_FieldPatternList_tok;
_temp3929.f1=({ struct Cyc_List_List* _temp3930=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3930->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3930->tl=
0; _temp3930;}); _temp3929;}); _temp3928;}); break; case 299: _LL3927: yyval=(
void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3932=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3932[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3933; _temp3933.tag= Cyc_FieldPatternList_tok;
_temp3933.f1=({ struct Cyc_List_List* _temp3934=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3934->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3934->tl=
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3934;}); _temp3933;}); _temp3932;}); break; case 300: _LL3931: yyval=*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 301:
_LL3935: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3937=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3937[ 0]=({ struct Cyc_Exp_tok_struct
_temp3938; _temp3938.tag= Cyc_Exp_tok; _temp3938.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3938;}); _temp3937;}); break; case 302: _LL3936: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 303: _LL3939: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3941=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3941[ 0]=({ struct Cyc_Exp_tok_struct
_temp3942; _temp3942.tag= Cyc_Exp_tok; _temp3942.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Primopopt_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3942;}); _temp3941;}); break; case 304: _LL3940: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3944=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3944[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3945; _temp3945.tag= Cyc_Primopopt_tok; _temp3945.f1=
0; _temp3945;}); _temp3944;}); break; case 305: _LL3943: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3947=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3947[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3948; _temp3948.tag= Cyc_Primopopt_tok; _temp3948.f1=({
struct Cyc_Core_Opt* _temp3949=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3949->v=( void*)(( void*) Cyc_Absyn_Times); _temp3949;});
_temp3948;}); _temp3947;}); break; case 306: _LL3946: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3951=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3951[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3952; _temp3952.tag= Cyc_Primopopt_tok; _temp3952.f1=({
struct Cyc_Core_Opt* _temp3953=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3953->v=( void*)(( void*) Cyc_Absyn_Div); _temp3953;});
_temp3952;}); _temp3951;}); break; case 307: _LL3950: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3955=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3955[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3956; _temp3956.tag= Cyc_Primopopt_tok; _temp3956.f1=({
struct Cyc_Core_Opt* _temp3957=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3957->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3957;});
_temp3956;}); _temp3955;}); break; case 308: _LL3954: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3959=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3959[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3960; _temp3960.tag= Cyc_Primopopt_tok; _temp3960.f1=({
struct Cyc_Core_Opt* _temp3961=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3961->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3961;});
_temp3960;}); _temp3959;}); break; case 309: _LL3958: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3963=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3963[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3964; _temp3964.tag= Cyc_Primopopt_tok; _temp3964.f1=({
struct Cyc_Core_Opt* _temp3965=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3965->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3965;});
_temp3964;}); _temp3963;}); break; case 310: _LL3962: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3967=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3967[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3968; _temp3968.tag= Cyc_Primopopt_tok; _temp3968.f1=({
struct Cyc_Core_Opt* _temp3969=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3969->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3969;});
_temp3968;}); _temp3967;}); break; case 311: _LL3966: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3971=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3971[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3972; _temp3972.tag= Cyc_Primopopt_tok; _temp3972.f1=({
struct Cyc_Core_Opt* _temp3973=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3973->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3973;});
_temp3972;}); _temp3971;}); break; case 312: _LL3970: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3975=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3975[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3976; _temp3976.tag= Cyc_Primopopt_tok; _temp3976.f1=({
struct Cyc_Core_Opt* _temp3977=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3977->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3977;});
_temp3976;}); _temp3975;}); break; case 313: _LL3974: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3979=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3979[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3980; _temp3980.tag= Cyc_Primopopt_tok; _temp3980.f1=({
struct Cyc_Core_Opt* _temp3981=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3981->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3981;});
_temp3980;}); _temp3979;}); break; case 314: _LL3978: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3983=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3983[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3984; _temp3984.tag= Cyc_Primopopt_tok; _temp3984.f1=({
struct Cyc_Core_Opt* _temp3985=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3985->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3985;});
_temp3984;}); _temp3983;}); break; case 315: _LL3982: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 316: _LL3986: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3988=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3988[ 0]=({ struct Cyc_Exp_tok_struct
_temp3989; _temp3989.tag= Cyc_Exp_tok; _temp3989.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3989;}); _temp3988;}); break; case 317: _LL3987: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3991=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3991[ 0]=({ struct Cyc_Exp_tok_struct
_temp3992; _temp3992.tag= Cyc_Exp_tok; _temp3992.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3992;}); _temp3991;}); break; case 318: _LL3990: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3994=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3994[ 0]=({ struct Cyc_Exp_tok_struct
_temp3995; _temp3995.tag= Cyc_Exp_tok; _temp3995.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3995;}); _temp3994;}); break; case 319: _LL3993: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3997=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3997[ 0]=({ struct Cyc_Exp_tok_struct
_temp3998; _temp3998.tag= Cyc_Exp_tok; _temp3998.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3998;}); _temp3997;}); break; case 320: _LL3996: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4000=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4000[ 0]=({ struct Cyc_Exp_tok_struct
_temp4001; _temp4001.tag= Cyc_Exp_tok; _temp4001.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4001;});
_temp4000;}); break; case 321: _LL3999: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4003=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4003[ 0]=({ struct Cyc_Exp_tok_struct _temp4004; _temp4004.tag= Cyc_Exp_tok;
_temp4004.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4004;}); _temp4003;}); break; case 322: _LL4002: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 323: _LL4005: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 324: _LL4006: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4008=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4008[ 0]=({ struct Cyc_Exp_tok_struct
_temp4009; _temp4009.tag= Cyc_Exp_tok; _temp4009.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4009;}); _temp4008;}); break; case 325: _LL4007: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 326: _LL4010: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4012=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4012[ 0]=({ struct Cyc_Exp_tok_struct
_temp4013; _temp4013.tag= Cyc_Exp_tok; _temp4013.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4013;}); _temp4012;}); break; case 327: _LL4011: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 328: _LL4014: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4016=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4016[ 0]=({ struct Cyc_Exp_tok_struct
_temp4017; _temp4017.tag= Cyc_Exp_tok; _temp4017.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4017;});
_temp4016;}); break; case 329: _LL4015: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 330: _LL4018: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4020=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4020[ 0]=({ struct Cyc_Exp_tok_struct _temp4021; _temp4021.tag= Cyc_Exp_tok;
_temp4021.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4021;}); _temp4020;}); break; case 331: _LL4019: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 332: _LL4022: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4024=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4024[ 0]=({ struct Cyc_Exp_tok_struct
_temp4025; _temp4025.tag= Cyc_Exp_tok; _temp4025.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4025;});
_temp4024;}); break; case 333: _LL4023: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 334: _LL4026: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4028=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4028[ 0]=({ struct Cyc_Exp_tok_struct _temp4029; _temp4029.tag= Cyc_Exp_tok;
_temp4029.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4029;});
_temp4028;}); break; case 335: _LL4027: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4031=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4031[ 0]=({ struct Cyc_Exp_tok_struct _temp4032; _temp4032.tag= Cyc_Exp_tok;
_temp4032.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4032;});
_temp4031;}); break; case 336: _LL4030: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 337: _LL4033: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4035=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4035[ 0]=({ struct Cyc_Exp_tok_struct _temp4036; _temp4036.tag= Cyc_Exp_tok;
_temp4036.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4036;});
_temp4035;}); break; case 338: _LL4034: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4038=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4038[ 0]=({ struct Cyc_Exp_tok_struct _temp4039; _temp4039.tag= Cyc_Exp_tok;
_temp4039.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4039;});
_temp4038;}); break; case 339: _LL4037: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4041=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4041[ 0]=({ struct Cyc_Exp_tok_struct _temp4042; _temp4042.tag= Cyc_Exp_tok;
_temp4042.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4042;});
_temp4041;}); break; case 340: _LL4040: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4044=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4044[ 0]=({ struct Cyc_Exp_tok_struct _temp4045; _temp4045.tag= Cyc_Exp_tok;
_temp4045.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4045;});
_temp4044;}); break; case 341: _LL4043: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 342: _LL4046: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4048=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4048[ 0]=({ struct Cyc_Exp_tok_struct _temp4049; _temp4049.tag= Cyc_Exp_tok;
_temp4049.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4049;}); _temp4048;}); break; case 343: _LL4047: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4051=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4051[ 0]=({ struct Cyc_Exp_tok_struct
_temp4052; _temp4052.tag= Cyc_Exp_tok; _temp4052.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4052;});
_temp4051;}); break; case 344: _LL4050: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 345: _LL4053: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4055=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4055[ 0]=({ struct Cyc_Exp_tok_struct _temp4056; _temp4056.tag= Cyc_Exp_tok;
_temp4056.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4056;}); _temp4055;}); break; case 346: _LL4054: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4058=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4058[ 0]=({ struct Cyc_Exp_tok_struct
_temp4059; _temp4059.tag= Cyc_Exp_tok; _temp4059.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4059;});
_temp4058;}); break; case 347: _LL4057: yyval=*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 348: _LL4060: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4062=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4062[ 0]=({ struct Cyc_Exp_tok_struct _temp4063; _temp4063.tag= Cyc_Exp_tok;
_temp4063.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4063;}); _temp4062;}); break; case 349: _LL4061: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4065=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4065[ 0]=({ struct Cyc_Exp_tok_struct
_temp4066; _temp4066.tag= Cyc_Exp_tok; _temp4066.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4066;});
_temp4065;}); break; case 350: _LL4064: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4068=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4068[ 0]=({ struct Cyc_Exp_tok_struct _temp4069; _temp4069.tag= Cyc_Exp_tok;
_temp4069.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4069;}); _temp4068;}); break; case 351: _LL4067: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 352: _LL4070: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4072=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4072[ 0]=({ struct Cyc_Exp_tok_struct
_temp4073; _temp4073.tag= Cyc_Exp_tok; _temp4073.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f3, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4073;}); _temp4072;}); break; case 353: _LL4071: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 354: _LL4074: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4076=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4076[ 0]=({ struct Cyc_Exp_tok_struct
_temp4077; _temp4077.tag= Cyc_Exp_tok; _temp4077.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4077;}); _temp4076;}); break; case 355: _LL4075: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4079=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4079[ 0]=({ struct Cyc_Exp_tok_struct
_temp4080; _temp4080.tag= Cyc_Exp_tok; _temp4080.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4080;}); _temp4079;}); break; case 356: _LL4078: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4082=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4082[ 0]=({ struct Cyc_Exp_tok_struct
_temp4083; _temp4083.tag= Cyc_Exp_tok; _temp4083.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4083;}); _temp4082;}); break; case 357: _LL4081: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4085=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4085[ 0]=({ struct Cyc_Exp_tok_struct
_temp4086; _temp4086.tag= Cyc_Exp_tok; _temp4086.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4086;}); _temp4085;}); break; case 358: _LL4084: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 359: _LL4087: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4089=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4089[ 0]=({ struct Cyc_Exp_tok_struct
_temp4090; _temp4090.tag= Cyc_Exp_tok; _temp4090.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4090;}); _temp4089;}); break; case 360: _LL4088: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4092=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4092[ 0]=({ struct Cyc_Exp_tok_struct
_temp4093; _temp4093.tag= Cyc_Exp_tok; _temp4093.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4093;}); _temp4092;}); break; case 361: _LL4091:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4095=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4095[ 0]=({ struct Cyc_Exp_tok_struct
_temp4096; _temp4096.tag= Cyc_Exp_tok; _temp4096.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4096;}); _temp4095;}); break; case 362: _LL4094: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4098=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4098[ 0]=({ struct Cyc_Exp_tok_struct
_temp4099; _temp4099.tag= Cyc_Exp_tok; _temp4099.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))).f3,({
struct _tagged_string* _temp4100=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp4100[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4100;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4099;}); _temp4098;}); break; case 363: _LL4097: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4102=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4102[ 0]=({ struct Cyc_Exp_tok_struct
_temp4103; _temp4103.tag= Cyc_Exp_tok; _temp4103.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4103;}); _temp4102;}); break; case 364: _LL4101: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4105=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4105[ 0]=({ struct Cyc_Exp_tok_struct
_temp4106; _temp4106.tag= Cyc_Exp_tok; _temp4106.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp4107=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp4107[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp4108; _temp4108.tag= Cyc_Absyn_Malloc_e;
_temp4108.f1= 0; _temp4108.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp4108;}); _temp4107;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4106;});
_temp4105;}); break; case 365: _LL4104: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4110=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4110[ 0]=({ struct Cyc_Exp_tok_struct _temp4111; _temp4111.tag= Cyc_Exp_tok;
_temp4111.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp4112=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp4112[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp4113; _temp4113.tag= Cyc_Absyn_Malloc_e;
_temp4113.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6)); _temp4113.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).last_line)); _temp4113;}); _temp4112;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4111;}); _temp4110;}); break; case 366: _LL4109: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4115=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4115[ 0]=({ struct Cyc_Primop_tok_struct
_temp4116; _temp4116.tag= Cyc_Primop_tok; _temp4116.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp4116;}); _temp4115;}); break; case 367: _LL4114: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4118=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4118[ 0]=({ struct Cyc_Primop_tok_struct
_temp4119; _temp4119.tag= Cyc_Primop_tok; _temp4119.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp4119;}); _temp4118;}); break; case 368: _LL4117: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4121=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4121[ 0]=({ struct Cyc_Primop_tok_struct
_temp4122; _temp4122.tag= Cyc_Primop_tok; _temp4122.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp4122;}); _temp4121;}); break; case 369: _LL4120: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4124=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4124[ 0]=({ struct Cyc_Primop_tok_struct
_temp4125; _temp4125.tag= Cyc_Primop_tok; _temp4125.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp4125;}); _temp4124;}); break; case 370: _LL4123: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4127=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4127[ 0]=({ struct Cyc_Primop_tok_struct
_temp4128; _temp4128.tag= Cyc_Primop_tok; _temp4128.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp4128;}); _temp4127;}); break; case 371: _LL4126: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4130=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4130[ 0]=({ struct Cyc_Primop_tok_struct
_temp4131; _temp4131.tag= Cyc_Primop_tok; _temp4131.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp4131;}); _temp4130;}); break; case 372: _LL4129: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4133=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4133[ 0]=({ struct Cyc_Primop_tok_struct
_temp4134; _temp4134.tag= Cyc_Primop_tok; _temp4134.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp4134;}); _temp4133;}); break; case 373: _LL4132: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4136=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4136[ 0]=({ struct Cyc_Primop_tok_struct
_temp4137; _temp4137.tag= Cyc_Primop_tok; _temp4137.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp4137;}); _temp4136;}); break; case 374: _LL4135: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4139=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4139[ 0]=({ struct Cyc_Primop_tok_struct
_temp4140; _temp4140.tag= Cyc_Primop_tok; _temp4140.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp4140;}); _temp4139;}); break; case 375: _LL4138: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 376: _LL4141: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4143=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4143[ 0]=({ struct Cyc_Exp_tok_struct
_temp4144; _temp4144.tag= Cyc_Exp_tok; _temp4144.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4144;}); _temp4143;}); break; case 377: _LL4142: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4146=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4146[ 0]=({ struct Cyc_Exp_tok_struct
_temp4147; _temp4147.tag= Cyc_Exp_tok; _temp4147.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4147;}); _temp4146;}); break; case 378: _LL4145: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4149=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4149[ 0]=({ struct Cyc_Exp_tok_struct
_temp4150; _temp4150.tag= Cyc_Exp_tok; _temp4150.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4150;}); _temp4149;}); break; case 379: _LL4148: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4152=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4152[ 0]=({ struct Cyc_Exp_tok_struct
_temp4153; _temp4153.tag= Cyc_Exp_tok; _temp4153.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4154=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp4154[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4154;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4153;}); _temp4152;}); break; case 380: _LL4151: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4156=(* q).f1; struct Cyc_List_List* _temp4166; struct Cyc_List_List*
_temp4168; _LL4158: if( _temp4156 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4159;}
else{ goto _LL4160;} _LL4160: if(( unsigned int) _temp4156 > 1u?*(( int*)
_temp4156) == Cyc_Absyn_Rel_n: 0){ _LL4167: _temp4166=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4156)->f1; if( _temp4166 == 0){ goto _LL4161;} else{ goto _LL4162;}} else{
goto _LL4162;} _LL4162: if(( unsigned int) _temp4156 > 1u?*(( int*) _temp4156)
== Cyc_Absyn_Abs_n: 0){ _LL4169: _temp4168=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4156)->f1; if( _temp4168 == 0){ goto _LL4163;} else{ goto _LL4164;}} else{
goto _LL4164;} _LL4164: goto _LL4165; _LL4159: goto _LL4157; _LL4161: goto
_LL4157; _LL4163: goto _LL4157; _LL4165: Cyc_Parse_err(({ unsigned char*
_temp4170=( unsigned char*)"struct field name is qualified"; struct
_tagged_string _temp4171; _temp4171.curr= _temp4170; _temp4171.base= _temp4170;
_temp4171.last_plus_one= _temp4170 + 31; _temp4171;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL4157; _LL4157:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4172=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4172[ 0]=({ struct Cyc_Exp_tok_struct _temp4173; _temp4173.tag= Cyc_Exp_tok;
_temp4173.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4173;}); _temp4172;}); break;} case 381: _LL4155: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4175=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4175[ 0]=({ struct Cyc_Exp_tok_struct
_temp4176; _temp4176.tag= Cyc_Exp_tok; _temp4176.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4177=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4177[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4177;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4176;}); _temp4175;}); break; case 382: _LL4174: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4179=(* q).f1; struct Cyc_List_List* _temp4189; struct Cyc_List_List*
_temp4191; _LL4181: if( _temp4179 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4182;}
else{ goto _LL4183;} _LL4183: if(( unsigned int) _temp4179 > 1u?*(( int*)
_temp4179) == Cyc_Absyn_Rel_n: 0){ _LL4190: _temp4189=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4179)->f1; if( _temp4189 == 0){ goto _LL4184;} else{ goto _LL4185;}} else{
goto _LL4185;} _LL4185: if(( unsigned int) _temp4179 > 1u?*(( int*) _temp4179)
== Cyc_Absyn_Abs_n: 0){ _LL4192: _temp4191=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4179)->f1; if( _temp4191 == 0){ goto _LL4186;} else{ goto _LL4187;}} else{
goto _LL4187;} _LL4187: goto _LL4188; _LL4182: goto _LL4180; _LL4184: goto
_LL4180; _LL4186: goto _LL4180; _LL4188: Cyc_Parse_err(({ unsigned char*
_temp4193=( unsigned char*)"struct field is qualified with module name"; struct
_tagged_string _temp4194; _temp4194.curr= _temp4193; _temp4194.base= _temp4193;
_temp4194.last_plus_one= _temp4193 + 43; _temp4194;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL4180; _LL4180:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4195=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4195[ 0]=({ struct Cyc_Exp_tok_struct _temp4196; _temp4196.tag= Cyc_Exp_tok;
_temp4196.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),(* q).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4196;}); _temp4195;}); break;} case 383: _LL4178: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4198=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4198[ 0]=({ struct Cyc_Exp_tok_struct
_temp4199; _temp4199.tag= Cyc_Exp_tok; _temp4199.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4199;}); _temp4198;}); break; case 384: _LL4197: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4201=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4201[ 0]=({ struct Cyc_Exp_tok_struct
_temp4202; _temp4202.tag= Cyc_Exp_tok; _temp4202.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4202;}); _temp4201;}); break; case 385: _LL4200: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4204=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4204[ 0]=({ struct Cyc_Exp_tok_struct
_temp4205; _temp4205.tag= Cyc_Exp_tok; _temp4205.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4206=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4206[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4207; _temp4207.tag= Cyc_Absyn_CompoundLit_e;
_temp4207.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp4207.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp4207;}); _temp4206;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4205;}); _temp4204;}); break; case 386: _LL4203: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4209=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4209[ 0]=({ struct Cyc_Exp_tok_struct
_temp4210; _temp4210.tag= Cyc_Exp_tok; _temp4210.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4211=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4211[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4212; _temp4212.tag= Cyc_Absyn_CompoundLit_e;
_temp4212.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp4212.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp4212;}); _temp4211;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4210;}); _temp4209;}); break; case 387: _LL4208: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4214=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4214[ 0]=({ struct Cyc_Exp_tok_struct
_temp4215; _temp4215.tag= Cyc_Exp_tok; _temp4215.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp4216=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp4216[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp4217; _temp4217.tag= Cyc_Absyn_Fill_e; _temp4217.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4217;});
_temp4216;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4215;});
_temp4214;}); break; case 388: _LL4213: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4219=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4219[ 0]=({ struct Cyc_Exp_tok_struct _temp4220; _temp4220.tag= Cyc_Exp_tok;
_temp4220.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp4221=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp4221[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp4222; _temp4222.tag= Cyc_Absyn_Codegen_e;
_temp4222.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp4222;}); _temp4221;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4220;}); _temp4219;}); break; case 389: _LL4218: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4224=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4224[ 0]=({ struct Cyc_Exp_tok_struct
_temp4225; _temp4225.tag= Cyc_Exp_tok; _temp4225.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp4226=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp4226[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp4227; _temp4227.tag= Cyc_Absyn_UnknownId_e;
_temp4227.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4227;}); _temp4226;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4225;}); _temp4224;}); break; case 390: _LL4223: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 391: _LL4228: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4230=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4230[ 0]=({ struct Cyc_Exp_tok_struct
_temp4231; _temp4231.tag= Cyc_Exp_tok; _temp4231.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4231;}); _temp4230;}); break; case 392: _LL4229: yyval=*(( void**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1); break; case 393: _LL4232: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4234=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4234[ 0]=({ struct Cyc_Exp_tok_struct
_temp4235; _temp4235.tag= Cyc_Exp_tok; _temp4235.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4236=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4236[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4237; _temp4237.tag=
Cyc_Absyn_UnknownId_e; _temp4237.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp4237;}); _temp4236;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4235;}); _temp4234;}); break; case 394: _LL4233:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4239=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4239[ 0]=({ struct Cyc_Exp_tok_struct
_temp4240; _temp4240.tag= Cyc_Exp_tok; _temp4240.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4241=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4241[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4242; _temp4242.tag=
Cyc_Absyn_UnknownId_e; _temp4242.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); _temp4242;}); _temp4241;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4240;}); _temp4239;}); break; case 395: _LL4238: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4244=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4244[ 0]=({ struct Cyc_Exp_tok_struct
_temp4245; _temp4245.tag= Cyc_Exp_tok; _temp4245.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4245;}); _temp4244;}); break; case 396: _LL4243: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4247=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4247[ 0]=({ struct Cyc_Exp_tok_struct
_temp4248; _temp4248.tag= Cyc_Exp_tok; _temp4248.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp4249=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp4249[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp4250; _temp4250.tag= Cyc_Absyn_Struct_e;
_temp4250.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp4250.f2= 0; _temp4250.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp4250.f4=
0; _temp4250;}); _temp4249;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4248;});
_temp4247;}); break; case 397: _LL4246: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4252=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4252[ 0]=({ struct Cyc_Exp_tok_struct _temp4253; _temp4253.tag= Cyc_Exp_tok;
_temp4253.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4253;});
_temp4252;}); break; case 398: _LL4251: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4255=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4255[ 0]=({ struct Cyc_ExpList_tok_struct _temp4256; _temp4256.tag= Cyc_ExpList_tok;
_temp4256.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp4256;}); _temp4255;}); break; case 399: _LL4254: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp4258=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp4258[ 0]=({ struct Cyc_ExpList_tok_struct
_temp4259; _temp4259.tag= Cyc_ExpList_tok; _temp4259.f1=({ struct Cyc_List_List*
_temp4260=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4260->hd=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4260->tl= 0; _temp4260;}); _temp4259;}); _temp4258;});
break; case 400: _LL4257: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4262=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4262[ 0]=({ struct Cyc_ExpList_tok_struct _temp4263; _temp4263.tag= Cyc_ExpList_tok;
_temp4263.f1=({ struct Cyc_List_List* _temp4264=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4264->hd=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4264->tl=
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr3, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp4264;}); _temp4263;}); _temp4262;}); break; case 401: _LL4261: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4266=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4266[ 0]=({ struct Cyc_Exp_tok_struct
_temp4267; _temp4267.tag= Cyc_Exp_tok; _temp4267.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1,(* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4267;}); _temp4266;}); break; case 402: _LL4265: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4269=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4269[ 0]=({ struct Cyc_Exp_tok_struct
_temp4270; _temp4270.tag= Cyc_Exp_tok; _temp4270.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4270;}); _temp4269;}); break; case 403: _LL4268: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4272=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4272[ 0]=({ struct Cyc_Exp_tok_struct
_temp4273; _temp4273.tag= Cyc_Exp_tok; _temp4273.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4273;}); _temp4272;}); break; case 404: _LL4271: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4275=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4275[ 0]=({ struct Cyc_Exp_tok_struct
_temp4276; _temp4276.tag= Cyc_Exp_tok; _temp4276.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4276;}); _temp4275;}); break; case 405: _LL4274: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp4278=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp4278[ 0]=({ struct Cyc_QualId_tok_struct
_temp4279; _temp4279.tag= Cyc_QualId_tok; _temp4279.f1=({ struct _tuple1*
_temp4280=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4280->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp4282=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp4282[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp4283; _temp4283.tag= Cyc_Absyn_Rel_n; _temp4283.f1= 0; _temp4283;});
_temp4282;}); _temp4280->f2=({ struct _tagged_string* _temp4281=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp4281[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4281;});
_temp4280;}); _temp4279;}); _temp4278;}); break; case 406: _LL4277: yyval=*((
void**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; default:
_LL4284:( void) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen;
yyssp_offset -= yylen; yylsp_offset -= yylen;*(( void**(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), ++
yyvsp_offset)= yyval; yylsp_offset ++; if( yylen == 0){(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line= Cyc_yylloc.first_line;(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_column=
Cyc_yylloc.first_column;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_column=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_column;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr4,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).text=({ unsigned char* _temp4286=( unsigned char*)""; struct
_tagged_string _temp4287; _temp4287.curr= _temp4286; _temp4287.base= _temp4286;
_temp4287.last_plus_one= _temp4286 + 1; _temp4287;});} else{(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line=(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset + yylen) - 1)).last_line;(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_column=(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr4, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset +
yylen) - 1)).last_column;} yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull(
407u, yyn)]; yystate= Cyc_yypgoto[ _check_known_subscript_notnull( 113u, yyn -
125)] +*(( short*(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset); if(( yystate >=
0? yystate <= 4559: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4560u,
yystate)] ==*(( short*(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset): 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4560u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 113u, yyn - 125)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 796u, yystate)]; if( yyn > - 32768? yyn < 4559:
0){ int sze= 0; struct _tagged_string msg; int x; int count; count= 0; for( x=
yyn < 0? - yyn: 0; x < 238u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4560u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 238u, x)]) + 15, count ++);}} msg=({
unsigned int _temp4288=( unsigned int)( sze + 15); unsigned char* _temp4289=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp4288); struct
_tagged_string _temp4291={ _temp4289, _temp4289, _temp4289 + _temp4288};{
unsigned int _temp4290= _temp4288; unsigned int i; for( i= 0; i < _temp4290; i
++){ _temp4289[ i]='\000';}}; _temp4291;}); Cyc_String_strcpy( msg,({
unsigned char* _temp4292=( unsigned char*)"parse error"; struct _tagged_string
_temp4293; _temp4293.curr= _temp4292; _temp4293.base= _temp4292; _temp4293.last_plus_one=
_temp4292 + 12; _temp4293;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 238u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4560u, x + yyn)] == x){ Cyc_String_strcat( msg,
count == 0?({ unsigned char* _temp4294=( unsigned char*)", expecting `"; struct
_tagged_string _temp4295; _temp4295.curr= _temp4294; _temp4295.base= _temp4294;
_temp4295.last_plus_one= _temp4294 + 14; _temp4295;}):({ unsigned char*
_temp4296=( unsigned char*)" or `"; struct _tagged_string _temp4297; _temp4297.curr=
_temp4296; _temp4297.base= _temp4296; _temp4297.last_plus_one= _temp4296 + 6;
_temp4297;})); Cyc_String_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 238u, x)]); Cyc_String_strcat( msg,({
unsigned char* _temp4298=( unsigned char*)"'"; struct _tagged_string _temp4299;
_temp4299.curr= _temp4298; _temp4299.base= _temp4298; _temp4299.last_plus_one=
_temp4298 + 2; _temp4299;})); count ++;}}} Cyc_yyerror(( struct _tagged_string)
msg);} else{ Cyc_yyerror(({ unsigned char* _temp4300=( unsigned char*)"parse error";
struct _tagged_string _temp4301; _temp4301.curr= _temp4300; _temp4301.base=
_temp4300; _temp4301.last_plus_one= _temp4300 + 12; _temp4301;}));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset == 0){ return 1;} yyvsp_offset --; yystate=( int)*(( short*(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyss, sizeof( short), -- yyssp_offset); yylsp_offset --; yyerrhandle: yyn=( int)
Cyc_yypact[ _check_known_subscript_notnull( 796u, yystate)]; if( yyn == - 32768){
goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4559)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4560u, yyn)] != 1){ goto yyerrdefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4560u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrpop;}} if( yyn == 795){ return 0;}*(( void**(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr4, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; goto yynewstate;}
void Cyc_yyprint( int i, void* v){ void* _temp4317= v; struct _tuple16*
_temp4337; struct _tuple16 _temp4339; int _temp4340; void* _temp4342;
unsigned char _temp4344; short _temp4346; struct _tagged_string _temp4348;
struct Cyc_Core_Opt* _temp4350; struct Cyc_Core_Opt* _temp4352; struct Cyc_Core_Opt
_temp4354; struct _tagged_string* _temp4355; struct _tuple1* _temp4357; struct
_tuple1 _temp4359; struct _tagged_string* _temp4360; void* _temp4362; _LL4319:
if( _temp4317 == Cyc_Okay_tok){ goto _LL4320;} else{ goto _LL4321;} _LL4321: if(*((
void**) _temp4317) == Cyc_Int_tok){ _LL4338: _temp4337=(( struct Cyc_Int_tok_struct*)
_temp4317)->f1; _temp4339=* _temp4337; _LL4343: _temp4342= _temp4339.f1; goto
_LL4341; _LL4341: _temp4340= _temp4339.f2; goto _LL4322;} else{ goto _LL4323;}
_LL4323: if(*(( void**) _temp4317) == Cyc_Char_tok){ _LL4345: _temp4344=((
struct Cyc_Char_tok_struct*) _temp4317)->f1; goto _LL4324;} else{ goto _LL4325;}
_LL4325: if(*(( void**) _temp4317) == Cyc_Short_tok){ _LL4347: _temp4346=((
struct Cyc_Short_tok_struct*) _temp4317)->f1; goto _LL4326;} else{ goto _LL4327;}
_LL4327: if(*(( void**) _temp4317) == Cyc_String_tok){ _LL4349: _temp4348=((
struct Cyc_String_tok_struct*) _temp4317)->f1; goto _LL4328;} else{ goto _LL4329;}
_LL4329: if(*(( void**) _temp4317) == Cyc_Stringopt_tok){ _LL4351: _temp4350=((
struct Cyc_Stringopt_tok_struct*) _temp4317)->f1; if( _temp4350 == 0){ goto
_LL4330;} else{ goto _LL4331;}} else{ goto _LL4331;} _LL4331: if(*(( void**)
_temp4317) == Cyc_Stringopt_tok){ _LL4353: _temp4352=(( struct Cyc_Stringopt_tok_struct*)
_temp4317)->f1; if( _temp4352 == 0){ goto _LL4333;} else{ _temp4354=* _temp4352;
_LL4356: _temp4355=( struct _tagged_string*) _temp4354.v; goto _LL4332;}} else{
goto _LL4333;} _LL4333: if(*(( void**) _temp4317) == Cyc_QualId_tok){ _LL4358:
_temp4357=(( struct Cyc_QualId_tok_struct*) _temp4317)->f1; _temp4359=*
_temp4357; _LL4363: _temp4362= _temp4359.f1; goto _LL4361; _LL4361: _temp4360=
_temp4359.f2; goto _LL4334;} else{ goto _LL4335;} _LL4335: goto _LL4336; _LL4320:
fprintf( sfile_to_file( Cyc_Stdio_stderr),"ok"); goto _LL4318; _LL4322: fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%d", _temp4340); goto _LL4318; _LL4324:
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%c",( int) _temp4344); goto _LL4318;
_LL4326: fprintf( sfile_to_file( Cyc_Stdio_stderr),"%ds",( int) _temp4346); goto
_LL4318; _LL4328:({ struct _tagged_string _temp4364= _temp4348; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _temp4364.last_plus_one - _temp4364.curr,
_temp4364.curr);}); goto _LL4318; _LL4330: fprintf( sfile_to_file( Cyc_Stdio_stderr),"null");
goto _LL4318; _LL4332:({ struct _tagged_string _temp4365=* _temp4355; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _temp4365.last_plus_one - _temp4365.curr,
_temp4365.curr);}); goto _LL4318; _LL4334: { struct Cyc_List_List* _temp4366= 0;
goto _LL4367; _LL4367:{ void* _temp4368= _temp4362; struct Cyc_List_List*
_temp4376; struct Cyc_List_List* _temp4378; _LL4370: if(( unsigned int)
_temp4368 > 1u?*(( int*) _temp4368) == Cyc_Absyn_Rel_n: 0){ _LL4377: _temp4376=((
struct Cyc_Absyn_Rel_n_struct*) _temp4368)->f1; goto _LL4371;} else{ goto
_LL4372;} _LL4372: if(( unsigned int) _temp4368 > 1u?*(( int*) _temp4368) == Cyc_Absyn_Abs_n:
0){ _LL4379: _temp4378=(( struct Cyc_Absyn_Abs_n_struct*) _temp4368)->f1; goto
_LL4373;} else{ goto _LL4374;} _LL4374: if( _temp4368 ==( void*) Cyc_Absyn_Loc_n){
goto _LL4375;} else{ goto _LL4369;} _LL4371: _temp4366= _temp4376; goto _LL4369;
_LL4373: _temp4366= _temp4378; goto _LL4369; _LL4375: goto _LL4369; _LL4369:;}
for( 0; _temp4366 != 0; _temp4366=(( struct Cyc_List_List*) _check_null(
_temp4366))->tl){({ struct _tagged_string _temp4380=*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( _temp4366))->hd); fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s::", _temp4380.last_plus_one - _temp4380.curr, _temp4380.curr);});}({
struct _tagged_string _temp4381=* _temp4360; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_temp4381.last_plus_one - _temp4381.curr, _temp4381.curr);}); goto _LL4318;}
_LL4336: fprintf( sfile_to_file( Cyc_Stdio_stderr),"?"); goto _LL4318; _LL4318:;}
struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result=
0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp4382=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4382->v=( void*) Cyc_Lexing_from_file(
f); _temp4382;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}