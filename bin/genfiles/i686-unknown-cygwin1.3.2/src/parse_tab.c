 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Std___sFILE*);
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
Cyc_List_List* x); extern unsigned int Cyc_Std_strlen( struct _tagged_arr s);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_Std_zstrcmp( struct _tagged_arr, struct _tagged_arr); extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_strcpy(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; extern
struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int); extern
struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
name; struct Cyc_Absyn_Tqual tq; void* type; int inject; } ; struct Cyc_Absyn_FnInfo{
struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct
Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs;
struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; struct Cyc_List_List* f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Printf_ft= 0; static const int Cyc_Absyn_Scanf_ft=
1; static const int Cyc_Absyn_Regparm_att= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att= 0; static const int
Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att= 2; static const
int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att= 4; static
const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Absyn_Nocommon_att= 6; static const int Cyc_Absyn_Shared_att=
7; static const int Cyc_Absyn_Unused_att= 8; static const int Cyc_Absyn_Weak_att=
9; static const int Cyc_Absyn_Dllimport_att= 10; static const int Cyc_Absyn_Dllexport_att=
11; static const int Cyc_Absyn_No_instrument_function_att= 12; static const int
Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att= 14;
static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int Cyc_Absyn_Format_att=
3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1; int f2; int f3; } ;
static const int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
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
15u]; extern void* Cyc_Absyn_rel_ns_null; extern int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct
Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt*
k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern struct
_tuple1* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one;
extern void* Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq,
void* b); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq, void* b); extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp* rgn_handle,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
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
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
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
void* t); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Lex_register_typedef(
struct _tuple1* s); extern void Cyc_Lex_enter_namespace( struct _tagged_arr*);
extern void Cyc_Lex_leave_namespace(); extern void Cyc_Lex_enter_using( struct
_tuple1*); extern void Cyc_Lex_leave_using(); enum  Cyc_Parse_Struct_or_union{
Cyc_Parse_Struct_su  =  0u, Cyc_Parse_Union_su  =  1u}; static const int Cyc_Parse_Signed_spec=
0; struct Cyc_Parse_Signed_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Unsigned_spec= 1; struct Cyc_Parse_Unsigned_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Short_spec=
2; struct Cyc_Parse_Short_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Long_spec= 3; struct Cyc_Parse_Long_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Type_spec=
4; struct Cyc_Parse_Type_spec_struct{ int tag; void* f1; struct Cyc_Position_Segment*
f2; } ; static const int Cyc_Parse_Decl_spec= 5; struct Cyc_Parse_Decl_spec_struct{
int tag; struct Cyc_Absyn_Decl* f1; } ; enum  Cyc_Parse_Storage_class{ Cyc_Parse_Typedef_sc
 =  0u, Cyc_Parse_Extern_sc  =  1u, Cyc_Parse_ExternC_sc  =  2u, Cyc_Parse_Static_sc
 =  3u, Cyc_Parse_Auto_sc  =  4u, Cyc_Parse_Register_sc  =  5u, Cyc_Parse_Abstract_sc
 =  6u}; struct Cyc_Parse_Declaration_spec{ struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual
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
struct _tuple8 Cyc_Parse_att_map[ 16u]={{{ _temp1, _temp1, _temp1 +  8u},( void*)
0u},{{ _temp2, _temp2, _temp2 +  6u},( void*) 1u},{{ _temp3, _temp3, _temp3 +  9u},(
void*) 2u},{{ _temp4, _temp4, _temp4 +  9u},( void*) 3u},{{ _temp5, _temp5,
_temp5 +  6u},( void*) 4u},{{ _temp6, _temp6, _temp6 +  8u},( void*)& Cyc_Parse_att_aligned},{{
_temp7, _temp7, _temp7 +  7u},( void*) 5u},{{ _temp8, _temp8, _temp8 +  7u},(
void*) 7u},{{ _temp9, _temp9, _temp9 +  7u},( void*) 8u},{{ _temp10, _temp10,
_temp10 +  5u},( void*) 9u},{{ _temp11, _temp11, _temp11 +  10u},( void*) 10u},{{
_temp12, _temp12, _temp12 +  10u},( void*) 11u},{{ _temp13, _temp13, _temp13 + 
23u},( void*) 12u},{{ _temp14, _temp14, _temp14 +  12u},( void*) 13u},{{ _temp15,
_temp15, _temp15 +  11u},( void*) 14u},{{ _temp16, _temp16, _temp16 +  22u},(
void*) 15u}}; static void Cyc_Parse_err( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse( sg, msg));} static void*
Cyc_Parse_abort( struct _tagged_arr msg, struct Cyc_Position_Segment* sg){ Cyc_Parse_err(
msg, sg);( int) _throw(( void*) Cyc_Position_Exit);} static void Cyc_Parse_warn(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct Cyc_Std_String_pa_struct
_temp19; _temp19.tag= Cyc_Std_String_pa; _temp19.f1=( struct _tagged_arr) msg;{
struct Cyc_Std_String_pa_struct _temp18; _temp18.tag= Cyc_Std_String_pa; _temp18.f1=(
struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void* _temp17[ 2u]={&
_temp18,& _temp19}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s: Warning: %s\n",
sizeof( unsigned char), 17u), _tag_arr( _temp17, sizeof( void*), 2u));}}});
return;} static void* Cyc_Parse_unimp( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ return Cyc_Parse_abort(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp21; _temp21.tag= Cyc_Std_String_pa; _temp21.f1=( struct _tagged_arr) msg;{
void* _temp20[ 1u]={& _temp21}; Cyc_Std_aprintf( _tag_arr("%s unimplemented",
sizeof( unsigned char), 17u), _tag_arr( _temp20, sizeof( void*), 1u));}}), sg);}
static void Cyc_Parse_unimp2( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){({ struct Cyc_Std_String_pa_struct _temp24; _temp24.tag= Cyc_Std_String_pa;
_temp24.f1=( struct _tagged_arr) msg;{ struct Cyc_Std_String_pa_struct _temp23;
_temp23.tag= Cyc_Std_String_pa; _temp23.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
sg);{ void* _temp22[ 2u]={& _temp23,& _temp24}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: Warning: Cyclone does not yet support %s\n", sizeof( unsigned char),
46u), _tag_arr( _temp22, sizeof( void*), 2u));}}}); return;} struct _tuple9{
struct _tuple7* f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Structfield*
Cyc_Parse_make_struct_field( struct Cyc_Position_Segment* loc, struct _tuple9*
field_info){ struct _tuple9 _temp27; struct Cyc_Absyn_Exp* _temp28; struct
_tuple7* _temp30; struct _tuple7 _temp32; struct Cyc_List_List* _temp33; struct
Cyc_List_List* _temp35; void* _temp37; struct Cyc_Absyn_Tqual _temp39; struct
_tuple1* _temp41; struct _tuple9* _temp25= field_info; _temp27=* _temp25; _LL31:
_temp30= _temp27.f1; _temp32=* _temp30; _LL42: _temp41= _temp32.f1; goto _LL40;
_LL40: _temp39= _temp32.f2; goto _LL38; _LL38: _temp37= _temp32.f3; goto _LL36;
_LL36: _temp35= _temp32.f4; goto _LL34; _LL34: _temp33= _temp32.f5; goto _LL29;
_LL29: _temp28= _temp27.f2; goto _LL26; _LL26: if( _temp35 !=  0){ Cyc_Parse_err(
_tag_arr("bad type params in struct field", sizeof( unsigned char), 32u), loc);}
if( Cyc_Absyn_is_qvar_qualified( _temp41)){ Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a namespace",
sizeof( unsigned char), 50u), loc);} return({ struct Cyc_Absyn_Structfield*
_temp43=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp43->name=(* _temp41).f2; _temp43->tq= _temp39; _temp43->type=( void*)
_temp37; _temp43->width= _temp28; _temp43->attributes= _temp33; _temp43;});}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp46; struct Cyc_List_List* _temp47; void* _temp49; struct Cyc_Absyn_Tqual
_temp51; struct Cyc_Core_Opt* _temp53; struct _tuple10* _temp44= field; _temp46=*
_temp44; _LL54: _temp53= _temp46.f1; goto _LL52; _LL52: _temp51= _temp46.f2;
goto _LL50; _LL50: _temp49= _temp46.f3; goto _LL48; _LL48: _temp47= _temp46.f4;
goto _LL45; _LL45: { struct Cyc_Core_Opt* _temp55= 0; if( _temp53 !=  0){ if(
Cyc_Absyn_is_qvar_qualified(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp53))->v)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), loc);} _temp55=({ struct Cyc_Core_Opt* _temp56=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp56->v=(
void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp53))->v)).f2;
_temp56;});} if( _temp47 !=  0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("parameter should have no type parameters",
sizeof( unsigned char), 41u), loc);} return({ struct _tuple2* _temp57=( struct
_tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp57->f1= _temp55; _temp57->f2=
_temp51; _temp57->f3= _temp49; _temp57;});}} static void* Cyc_Parse_type_spec(
void* t, struct Cyc_Position_Segment* loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp58=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp58[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp59; _temp59.tag= Cyc_Parse_Type_spec;
_temp59.f1=( void*) t; _temp59.f2= loc; _temp59;}); _temp58;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp60= t; struct Cyc_Absyn_Exp* _temp66;
struct Cyc_Absyn_Tqual _temp68; void* _temp70; _LL62: if(( unsigned int) _temp60
>  4u?*(( int*) _temp60) ==  Cyc_Absyn_ArrayType: 0){ _LL71: _temp70=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp60)->f1; goto _LL69; _LL69: _temp68=((
struct Cyc_Absyn_ArrayType_struct*) _temp60)->f2; goto _LL67; _LL67: _temp66=((
struct Cyc_Absyn_ArrayType_struct*) _temp60)->f3; goto _LL63;} else{ goto _LL64;}
_LL64: goto _LL65; _LL63: return Cyc_Absyn_starb_typ( _temp70,( void*) Cyc_Absyn_HeapRgn,
_temp68, _temp66 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp72=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp72[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp73; _temp73.tag= Cyc_Absyn_Upper_b;
_temp73.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp66); _temp73;}); _temp72;}));
_LL65: return t; _LL61:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp74=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp74->f1=(* t).f2; _temp74->f2=(* t).f3;
_temp74;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp75=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp101; _LL77: if(*(( int*) _temp75) ==  Cyc_Absyn_Var_d){
_LL102: _temp101=(( struct Cyc_Absyn_Var_d_struct*) _temp75)->f1; goto _LL78;}
else{ goto _LL79;} _LL79: if(*(( int*) _temp75) ==  Cyc_Absyn_Let_d){ goto _LL80;}
else{ goto _LL81;} _LL81: if(*(( int*) _temp75) ==  Cyc_Absyn_Letv_d){ goto
_LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp75) ==  Cyc_Absyn_Fn_d){
goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp75) ==  Cyc_Absyn_Struct_d){
goto _LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp75) ==  Cyc_Absyn_Union_d){
goto _LL88;} else{ goto _LL89;} _LL89: if(*(( int*) _temp75) ==  Cyc_Absyn_Tunion_d){
goto _LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp75) ==  Cyc_Absyn_Typedef_d){
goto _LL92;} else{ goto _LL93;} _LL93: if(*(( int*) _temp75) ==  Cyc_Absyn_Enum_d){
goto _LL94;} else{ goto _LL95;} _LL95: if(*(( int*) _temp75) ==  Cyc_Absyn_Namespace_d){
goto _LL96;} else{ goto _LL97;} _LL97: if(*(( int*) _temp75) ==  Cyc_Absyn_Using_d){
goto _LL98;} else{ goto _LL99;} _LL99: if(*(( int*) _temp75) ==  Cyc_Absyn_ExternC_d){
goto _LL100;} else{ goto _LL76;} _LL78: if( _temp101->initializer !=  0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);} if( Cyc_Absyn_is_qvar_qualified( _temp101->name)){
Cyc_Parse_err( _tag_arr("namespaces not allowed on parameter declarations",
sizeof( unsigned char), 49u), x->loc);}{ int found= 0; for( 0; params !=  0;
params=(( struct Cyc_List_List*) _check_null( params))->tl){ if( Cyc_Std_zstrptrcmp((*
_temp101->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
params))->hd) ==  0){ found= 1; break;}} if( ! found){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp104; _temp104.tag= Cyc_Std_String_pa;
_temp104.f1=( struct _tagged_arr)*(* _temp101->name).f2;{ void* _temp103[ 1u]={&
_temp104}; Cyc_Std_aprintf( _tag_arr("%s is not listed as a parameter", sizeof(
unsigned char), 32u), _tag_arr( _temp103, sizeof( void*), 1u));}}), x->loc);}
return;} _LL80: decl_kind= _tag_arr("let declaration", sizeof( unsigned char),
16u); goto _LL76; _LL82: decl_kind= _tag_arr("let declaration", sizeof(
unsigned char), 16u); goto _LL76; _LL84: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL76; _LL86: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL76; _LL88: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL76; _LL90: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL76; _LL92: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL76; _LL94: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL76; _LL96: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL76; _LL98: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL76; _LL100: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL76; _LL76:;}(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp106; _temp106.tag= Cyc_Std_String_pa;
_temp106.f1=( struct _tagged_arr) decl_kind;{ void* _temp105[ 1u]={& _temp106};
Cyc_Std_aprintf( _tag_arr("%s appears in parameter type", sizeof( unsigned char),
29u), _tag_arr( _temp105, sizeof( void*), 1u));}}), x->loc); return;} struct
_tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_arr* x){ struct _tuple12 _temp109; struct Cyc_Position_Segment* _temp110;
struct Cyc_List_List* _temp112; struct _tuple12* _temp107= env; _temp109=*
_temp107; _LL113: _temp112= _temp109.f1; goto _LL111; _LL111: _temp110= _temp109.f2;
goto _LL108; _LL108: if( _temp112 ==  0){ return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp115; _temp115.tag= Cyc_Std_String_pa;
_temp115.f1=( struct _tagged_arr)* x;{ void* _temp114[ 1u]={& _temp115}; Cyc_Std_aprintf(
_tag_arr("missing type for parameter %s", sizeof( unsigned char), 30u), _tag_arr(
_temp114, sizeof( void*), 1u));}}), _temp110);}{ void* _temp116=( void*)((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp112))->hd)->r;
struct Cyc_Absyn_Vardecl* _temp122; _LL118: if(*(( int*) _temp116) ==  Cyc_Absyn_Var_d){
_LL123: _temp122=(( struct Cyc_Absyn_Var_d_struct*) _temp116)->f1; goto _LL119;}
else{ goto _LL120;} _LL120: goto _LL121; _LL119: if( Cyc_Absyn_is_qvar_qualified(
_temp122->name)){ Cyc_Parse_err( _tag_arr("namespace not allowed on parameter",
sizeof( unsigned char), 35u), _temp110);} if( Cyc_Std_zstrptrcmp((* _temp122->name).f2,
x) ==  0){ return({ struct _tuple2* _temp124=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp124->f1=({ struct Cyc_Core_Opt* _temp125=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp125->v=( void*)(*
_temp122->name).f2; _temp125;}); _temp124->f2= _temp122->tq; _temp124->f3=( void*)
_temp122->type; _temp124;});} else{ return Cyc_Parse_get_param_type(({ struct
_tuple12* _temp126=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp126->f1=(( struct Cyc_List_List*) _check_null( _temp112))->tl; _temp126->f2=
_temp110; _temp126;}), x);} _LL121: return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration",
sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp112))->hd)->loc); _LL117:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp127= tm; _LL129: if(( unsigned int) _temp127 >  1u?*(( int*)
_temp127) ==  Cyc_Absyn_TypeParams_mod: 0){ goto _LL130;} else{ goto _LL131;}
_LL131: goto _LL132; _LL130: return 1; _LL132: return 0; _LL128:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_Std_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp133=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp133[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp134; _temp134.tag= Cyc_Absyn_VarType; _temp134.f1=({ struct Cyc_Absyn_Tvar*
_temp135=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp135->name=({ struct _tagged_arr* _temp136=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp136[ 0]= s; _temp136;}); _temp135->identity=
0; _temp135->kind= k; _temp135;}); _temp134;}); _temp133;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp137=
t; struct Cyc_Absyn_Tvar* _temp143; _LL139: if(( unsigned int) _temp137 >  4u?*((
int*) _temp137) ==  Cyc_Absyn_VarType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_VarType_struct*)
_temp137)->f1; goto _LL140;} else{ goto _LL141;} _LL141: goto _LL142; _LL140:
return _temp143; _LL142: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL138:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp145=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp145[ 0]=({ struct Cyc_Absyn_VarType_struct _temp146; _temp146.tag= Cyc_Absyn_VarType;
_temp146.f1= pr; _temp146;}); _temp145;});} static int Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp147= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp153; struct Cyc_Absyn_Tvar _temp155; struct Cyc_Absyn_Conref* _temp156;
_LL149: if(( unsigned int) _temp147 >  4u?*(( int*) _temp147) ==  Cyc_Absyn_VarType:
0){ _LL154: _temp153=(( struct Cyc_Absyn_VarType_struct*) _temp147)->f1;
_temp155=* _temp153; _LL157: _temp156= _temp155.kind; goto _LL150;} else{ goto
_LL151;} _LL151: goto _LL152; _LL150: _temp156= Cyc_Absyn_compress_conref(
_temp156);{ void* _temp158=( void*) _temp156->v; void* _temp166; _LL160: if(
_temp158 == ( void*) Cyc_Absyn_No_constr){ goto _LL161;} else{ goto _LL162;}
_LL162: if(( unsigned int) _temp158 >  1u?*(( int*) _temp158) ==  Cyc_Absyn_Eq_constr:
0){ _LL167: _temp166=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp158)->f1;
goto _LL163;} else{ goto _LL164;} _LL164: goto _LL165; _LL161:( void*)( _temp156->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp168=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp168[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp169; _temp169.tag= Cyc_Absyn_Eq_constr; _temp169.f1=(
void*) k; _temp169;}); _temp168;}))); return 1; _LL163: return k ==  _temp166;
_LL165:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp170=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp170[ 0]=({ struct Cyc_Core_Impossible_struct _temp171; _temp171.tag= Cyc_Core_Impossible;
_temp171.f1= _tag_arr("forward after compress_conref", sizeof( unsigned char),
30u); _temp171;}); _temp170;})); _LL159:;} _LL152: return 0; _LL148:;} static
struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List* tms,
struct Cyc_List_List* tds, struct Cyc_Position_Segment* loc){ if( tds ==  0){
return tms;} if( tms ==  0){ return 0;}{ void* _temp172=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; void* _temp178; _LL174: if(( unsigned int) _temp172 >  1u?*((
int*) _temp172) ==  Cyc_Absyn_Function_mod: 0){ _LL179: _temp178=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp172)->f1; goto _LL175;} else{ goto
_LL176;} _LL176: goto _LL177; _LL175: if((( struct Cyc_List_List*) _check_null(
tms))->tl ==  0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
==  0: 0)){ void* _temp180= _temp178; struct Cyc_List_List* _temp186; _LL182:
if(*(( int*) _temp180) ==  Cyc_Absyn_WithTypes){ goto _LL183;} else{ goto _LL184;}
_LL184: if(*(( int*) _temp180) ==  Cyc_Absyn_NoTypes){ _LL187: _temp186=((
struct Cyc_Absyn_NoTypes_struct*) _temp180)->f1; goto _LL185;} else{ goto _LL181;}
_LL183: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL185:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp186,
tds);{ struct _tuple12* _temp188=({ struct _tuple12* _temp194=( struct _tuple12*)
_cycalloc( sizeof( struct _tuple12)); _temp194->f1= tds; _temp194->f2= loc;
_temp194;}); return({ struct Cyc_List_List* _temp189=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp189->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp190=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp190[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp191; _temp191.tag= Cyc_Absyn_Function_mod;
_temp191.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp192=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp192[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp193; _temp193.tag= Cyc_Absyn_WithTypes;
_temp193.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp188, _temp186); _temp193.f2= 0; _temp193.f3= 0;
_temp193.f4= 0; _temp193.f5= 0; _temp193;}); _temp192;})); _temp191;}); _temp190;}));
_temp189->tl= 0; _temp189;});} _LL181:;} else{ return({ struct Cyc_List_List*
_temp195=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp195->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp195->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp195;});} _LL177: return({ struct Cyc_List_List*
_temp196=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp196->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp196->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp196;}); _LL173:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds !=  0){ d=({ struct Cyc_Parse_Declarator* _temp197=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp197->id= d->id; _temp197->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp197;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc !=  0){ switch(( enum  Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v){ case Cyc_Parse_Extern_sc:
_LL198: sc=( void*) Cyc_Absyn_Extern; break; case Cyc_Parse_ExternC_sc: _LL199:
sc=( void*) Cyc_Absyn_ExternC; break; case Cyc_Parse_Static_sc: _LL200: sc=(
void*) Cyc_Absyn_Static; break; default: _LL201: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); break;}}}{ struct Cyc_Core_Opt* _temp205;
void* _temp207; struct _tuple5 _temp203= Cyc_Parse_collapse_type_specifiers( tss,
loc); _LL208: _temp207= _temp203.f1; goto _LL206; _LL206: _temp205= _temp203.f2;
goto _LL204; _LL204: { struct Cyc_List_List* _temp211; struct Cyc_List_List*
_temp213; void* _temp215; struct Cyc_Absyn_Tqual _temp217; struct _tuple6
_temp209= Cyc_Parse_apply_tms( tq, _temp207, atts, d->tms); _LL218: _temp217=
_temp209.f1; goto _LL216; _LL216: _temp215= _temp209.f2; goto _LL214; _LL214:
_temp213= _temp209.f3; goto _LL212; _LL212: _temp211= _temp209.f4; goto _LL210;
_LL210: if( _temp205 !=  0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp213 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp219= _temp215; struct Cyc_Absyn_FnInfo _temp225; struct Cyc_List_List*
_temp227; struct Cyc_List_List* _temp229; struct Cyc_Absyn_VarargInfo* _temp231;
int _temp233; struct Cyc_List_List* _temp235; void* _temp237; struct Cyc_Core_Opt*
_temp239; struct Cyc_List_List* _temp241; _LL221: if(( unsigned int) _temp219 > 
4u?*(( int*) _temp219) ==  Cyc_Absyn_FnType: 0){ _LL226: _temp225=(( struct Cyc_Absyn_FnType_struct*)
_temp219)->f1; _LL242: _temp241= _temp225.tvars; goto _LL240; _LL240: _temp239=
_temp225.effect; goto _LL238; _LL238: _temp237=( void*) _temp225.ret_typ; goto
_LL236; _LL236: _temp235= _temp225.args; goto _LL234; _LL234: _temp233= _temp225.c_varargs;
goto _LL232; _LL232: _temp231= _temp225.cyc_varargs; goto _LL230; _LL230:
_temp229= _temp225.rgn_po; goto _LL228; _LL228: _temp227= _temp225.attributes;
goto _LL222;} else{ goto _LL223;} _LL223: goto _LL224; _LL222: { struct Cyc_List_List*
_temp243=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp235); return({ struct Cyc_Absyn_Fndecl*
_temp244=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp244->sc=( void*) sc; _temp244->name= d->id; _temp244->tvs= _temp241;
_temp244->is_inline= is_inline; _temp244->effect= _temp239; _temp244->ret_type=(
void*) _temp237; _temp244->args= _temp243; _temp244->c_varargs= _temp233;
_temp244->cyc_varargs= _temp231; _temp244->rgn_po= _temp229; _temp244->body=
body; _temp244->cached_typ= 0; _temp244->param_vardecls= 0; _temp244->attributes=
Cyc_List_append( _temp227, _temp211); _temp244;});} _LL224: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL220:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 ==  0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp245=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp245->f1= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp245->f2=(* t).f2; _temp245->f3=(* t).f3; _temp245;});} else{ return({
struct _tuple4* _temp246=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp246->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp246->f2=(* t).f2; _temp246->f3=(* t).f3; _temp246;});}} static
unsigned char _temp247[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp247, _temp247, _temp247 +  52u};
static unsigned char _temp248[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp248, _temp248, _temp248 +  63u};
static unsigned char _temp249[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp249, _temp249, _temp249 +  50u};
static unsigned char _temp250[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp250, _temp250, _temp250 +  60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts !=  0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp251=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp265; void* _temp267; struct Cyc_Position_Segment*
_temp269; struct Cyc_Position_Segment* _temp271; struct Cyc_Position_Segment*
_temp273; struct Cyc_Position_Segment* _temp275; struct Cyc_Absyn_Decl* _temp277;
_LL253: if(*(( int*) _temp251) ==  Cyc_Parse_Type_spec){ _LL268: _temp267=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp251)->f1; goto _LL266; _LL266: _temp265=((
struct Cyc_Parse_Type_spec_struct*) _temp251)->f2; goto _LL254;} else{ goto
_LL255;} _LL255: if(*(( int*) _temp251) ==  Cyc_Parse_Signed_spec){ _LL270:
_temp269=(( struct Cyc_Parse_Signed_spec_struct*) _temp251)->f1; goto _LL256;}
else{ goto _LL257;} _LL257: if(*(( int*) _temp251) ==  Cyc_Parse_Unsigned_spec){
_LL272: _temp271=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp251)->f1; goto
_LL258;} else{ goto _LL259;} _LL259: if(*(( int*) _temp251) ==  Cyc_Parse_Short_spec){
_LL274: _temp273=(( struct Cyc_Parse_Short_spec_struct*) _temp251)->f1; goto
_LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp251) ==  Cyc_Parse_Long_spec){
_LL276: _temp275=(( struct Cyc_Parse_Long_spec_struct*) _temp251)->f1; goto
_LL262;} else{ goto _LL263;} _LL263: if(*(( int*) _temp251) ==  Cyc_Parse_Decl_spec){
_LL278: _temp277=(( struct Cyc_Parse_Decl_spec_struct*) _temp251)->f1; goto
_LL264;} else{ goto _LL252;} _LL254: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp265);} last_loc= _temp265; seen_type= 1; t= _temp267; goto _LL252; _LL256:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp269);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp269);} last_loc= _temp269; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL252; _LL258: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp271);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp271);} last_loc= _temp271; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL252; _LL260: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp273);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp273);} last_loc= _temp273; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL252; _LL262: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp275);} if( seen_size){ void* _temp279= sz; _LL281: if( _temp279 == ( void*)
Cyc_Absyn_B4){ goto _LL282;} else{ goto _LL283;} _LL283: goto _LL284; _LL282: sz=(
void*) Cyc_Absyn_B8; goto _LL280; _LL284: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp275); goto _LL280; _LL280:;} last_loc=
_temp275; seen_size= 1; goto _LL252; _LL264: last_loc= _temp277->loc; if(
declopt ==  0? ! seen_type: 0){ seen_type= 1;{ void* _temp285=( void*) _temp277->r;
struct Cyc_Absyn_Structdecl* _temp297; struct Cyc_Absyn_Tuniondecl* _temp299;
struct Cyc_Absyn_Uniondecl* _temp301; struct Cyc_Absyn_Enumdecl* _temp303;
_LL287: if(*(( int*) _temp285) ==  Cyc_Absyn_Struct_d){ _LL298: _temp297=((
struct Cyc_Absyn_Struct_d_struct*) _temp285)->f1; goto _LL288;} else{ goto
_LL289;} _LL289: if(*(( int*) _temp285) ==  Cyc_Absyn_Tunion_d){ _LL300:
_temp299=(( struct Cyc_Absyn_Tunion_d_struct*) _temp285)->f1; goto _LL290;}
else{ goto _LL291;} _LL291: if(*(( int*) _temp285) ==  Cyc_Absyn_Union_d){
_LL302: _temp301=(( struct Cyc_Absyn_Union_d_struct*) _temp285)->f1; goto _LL292;}
else{ goto _LL293;} _LL293: if(*(( int*) _temp285) ==  Cyc_Absyn_Enum_d){ _LL304:
_temp303=(( struct Cyc_Absyn_Enum_d_struct*) _temp285)->f1; goto _LL294;} else{
goto _LL295;} _LL295: goto _LL296; _LL288: { struct Cyc_List_List* _temp305=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp297->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp306=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp306[ 0]=({ struct Cyc_Absyn_StructType_struct _temp307; _temp307.tag= Cyc_Absyn_StructType;
_temp307.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp297->name))->v); _temp307.f2= _temp305; _temp307.f3= 0;
_temp307;}); _temp306;}); if( _temp297->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp308=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp308->v=( void*) _temp277; _temp308;});} goto _LL286;} _LL290: { struct Cyc_List_List*
_temp309=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp299->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp310=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp310[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp311; _temp311.tag= Cyc_Absyn_TunionType;
_temp311.f1=({ struct Cyc_Absyn_TunionInfo _temp312; _temp312.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp313=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp313[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp314; _temp314.tag= Cyc_Absyn_KnownTunion;
_temp314.f1= _temp299; _temp314;}); _temp313;})); _temp312.targs= _temp309;
_temp312.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp312;}); _temp311;});
_temp310;}); if( _temp299->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp315=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp315->v=( void*) _temp277; _temp315;});} goto _LL286;} _LL292: { struct Cyc_List_List*
_temp316=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp301->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp317=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp317[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp318; _temp318.tag= Cyc_Absyn_UnionType; _temp318.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp301->name))->v); _temp318.f2= _temp316; _temp318.f3= 0; _temp318;});
_temp317;}); if( _temp301->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp319=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp319->v=( void*) _temp277; _temp319;});} goto _LL286;} _LL294: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp320=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp320[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp321; _temp321.tag= Cyc_Absyn_EnumType; _temp321.f1=
_temp303->name; _temp321.f2= 0; _temp321;}); _temp320;}); declopt=({ struct Cyc_Core_Opt*
_temp322=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp322->v=( void*) _temp277; _temp322;}); goto _LL286; _LL296:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp277->loc); goto _LL286; _LL286:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp277->loc);} goto _LL252; _LL252:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp323=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp323[ 0]=({ struct Cyc_Absyn_IntType_struct _temp324; _temp324.tag= Cyc_Absyn_IntType;
_temp324.f1=( void*) sgn; _temp324.f2=( void*) sz; _temp324;}); _temp323;});}
else{ if( seen_sign){ void* _temp325= t; void* _temp331; _LL327: if((
unsigned int) _temp325 >  4u?*(( int*) _temp325) ==  Cyc_Absyn_IntType: 0){
_LL332: _temp331=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp325)->f2;
goto _LL328;} else{ goto _LL329;} _LL329: goto _LL330; _LL328: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp333=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp333[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp334; _temp334.tag= Cyc_Absyn_IntType; _temp334.f1=( void*) sgn; _temp334.f2=(
void*) _temp331; _temp334;}); _temp333;}); goto _LL326; _LL330: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL326; _LL326:;} if( seen_size){ void* _temp335= t; void*
_temp341; _LL337: if(( unsigned int) _temp335 >  4u?*(( int*) _temp335) ==  Cyc_Absyn_IntType:
0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp335)->f1;
goto _LL338;} else{ goto _LL339;} _LL339: goto _LL340; _LL338: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp343=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp343[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp344; _temp344.tag= Cyc_Absyn_IntType; _temp344.f1=( void*) _temp341;
_temp344.f2=( void*) sz; _temp344;}); _temp343;}); goto _LL336; _LL340: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL336; _LL336:;}} return({ struct _tuple5 _temp345; _temp345.f1=
t; _temp345.f2= declopt; _temp345;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp346=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp347= _temp346->id; struct Cyc_List_List* _temp350; struct
Cyc_List_List* _temp352; void* _temp354; struct Cyc_Absyn_Tqual _temp356; struct
_tuple6 _temp348= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp346->tms);
_LL357: _temp356= _temp348.f1; goto _LL355; _LL355: _temp354= _temp348.f2; goto
_LL353; _LL353: _temp352= _temp348.f3; goto _LL351; _LL351: _temp350= _temp348.f4;
goto _LL349; _LL349: return({ struct Cyc_List_List* _temp358=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp358->hd=( void*)({ struct
_tuple7* _temp359=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp359->f1= _temp347; _temp359->f2= _temp356; _temp359->f3= _temp354; _temp359->f4=
_temp352; _temp359->f5= _temp350; _temp359;}); _temp358->tl= Cyc_Parse_apply_tmss(
_temp356, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp358;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp360; _temp360.f1= tq;
_temp360.f2= t; _temp360.f3= 0; _temp360.f4= atts; _temp360;});}{ void* _temp361=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp375; void* _temp377; struct Cyc_Position_Segment* _temp379; struct Cyc_List_List*
_temp381; struct Cyc_Absyn_Tqual _temp383; void* _temp385; void* _temp387;
struct Cyc_List_List* _temp389; struct Cyc_Position_Segment* _temp391; _LL363:
if( _temp361 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL364;} else{ goto _LL365;}
_LL365: if(( unsigned int) _temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_ConstArray_mod:
0){ _LL376: _temp375=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp361)->f1;
goto _LL366;} else{ goto _LL367;} _LL367: if(( unsigned int) _temp361 >  1u?*((
int*) _temp361) ==  Cyc_Absyn_Function_mod: 0){ _LL378: _temp377=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp361)->f1; goto _LL368;} else{ goto
_LL369;} _LL369: if(( unsigned int) _temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL382: _temp381=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp361)->f1;
goto _LL380; _LL380: _temp379=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp361)->f2; goto _LL370;} else{ goto _LL371;} _LL371: if(( unsigned int)
_temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_Pointer_mod: 0){ _LL388:
_temp387=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp361)->f1; goto
_LL386; _LL386: _temp385=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp361)->f2; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp361)->f3; goto _LL372;} else{ goto _LL373;} _LL373: if(( unsigned int)
_temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_Attributes_mod: 0){ _LL392:
_temp391=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp361)->f1; goto _LL390;
_LL390: _temp389=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp361)->f2; goto
_LL374;} else{ goto _LL362;} _LL364: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp393=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp393[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp394; _temp394.tag= Cyc_Absyn_ArrayType; _temp394.f1=(
void*) t; _temp394.f2= tq; _temp394.f3= 0; _temp394;}); _temp393;}), atts,((
struct Cyc_List_List*) _check_null( tms))->tl); _LL366: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp395=(
struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp395[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp396; _temp396.tag= Cyc_Absyn_ArrayType;
_temp396.f1=( void*) t; _temp396.f2= tq; _temp396.f3=( struct Cyc_Absyn_Exp*)
_temp375; _temp396;}); _temp395;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL368: { void* _temp397= _temp377; struct Cyc_List_List* _temp403;
struct Cyc_Core_Opt* _temp405; struct Cyc_Absyn_VarargInfo* _temp407; int
_temp409; struct Cyc_List_List* _temp411; struct Cyc_Position_Segment* _temp413;
_LL399: if(*(( int*) _temp397) ==  Cyc_Absyn_WithTypes){ _LL412: _temp411=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f1; goto _LL410; _LL410: _temp409=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f2; goto _LL408; _LL408: _temp407=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f3; goto _LL406; _LL406: _temp405=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f4; goto _LL404; _LL404: _temp403=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f5; goto _LL400;} else{ goto
_LL401;} _LL401: if(*(( int*) _temp397) ==  Cyc_Absyn_NoTypes){ _LL414: _temp413=((
struct Cyc_Absyn_NoTypes_struct*) _temp397)->f2; goto _LL402;} else{ goto _LL398;}
_LL400: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!=  0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp415=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp415->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp415->tl= fn_atts; _temp415;});} else{ new_atts=({ struct Cyc_List_List*
_temp416=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp416->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp416->tl= new_atts; _temp416;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl !=  0){ void* _temp417=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp423; _LL419: if(( unsigned int) _temp417 >  1u?*(( int*) _temp417) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL424: _temp423=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp417)->f1;
goto _LL420;} else{ goto _LL421;} _LL421: goto _LL422; _LL420: typvars= _temp423;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL418; _LL422: goto
_LL418; _LL418:;} if((((( ! _temp409? _temp407 ==  0: 0)? _temp411 !=  0: 0)?((
struct Cyc_List_List*) _check_null( _temp411))->tl ==  0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp411))->hd)).f1 ==  0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp411))->hd)).f3 == ( void*)
Cyc_Absyn_VoidType: 0){ _temp411= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp411); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp405, t, _temp411, _temp409, _temp407, _temp403, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL402:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp413)); _LL398:;} _LL370: if((( struct Cyc_List_List*) _check_null(
tms))->tl ==  0){ return({ struct _tuple6 _temp425; _temp425.f1= tq; _temp425.f2=
t; _temp425.f3= _temp381; _temp425.f4= atts; _temp425;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp379)); _LL372: { void* _temp426= _temp387;
struct Cyc_Absyn_Exp* _temp434; struct Cyc_Absyn_Exp* _temp436; _LL428: if((
unsigned int) _temp426 >  1u?*(( int*) _temp426) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL435: _temp434=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp426)->f1; goto
_LL429;} else{ goto _LL430;} _LL430: if(( unsigned int) _temp426 >  1u?*(( int*)
_temp426) ==  Cyc_Absyn_Nullable_ps: 0){ _LL437: _temp436=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp426)->f1; goto _LL431;} else{ goto _LL432;} _LL432: if( _temp426 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL433;} else{ goto _LL427;} _LL429: return Cyc_Parse_apply_tms(
_temp383, Cyc_Absyn_atb_typ( t, _temp385, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp438=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp438[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp439; _temp439.tag= Cyc_Absyn_Upper_b;
_temp439.f1= _temp434; _temp439;}); _temp438;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL431: return Cyc_Parse_apply_tms( _temp383, Cyc_Absyn_starb_typ(
t, _temp385, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp440=( struct
Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp440[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp441; _temp441.tag= Cyc_Absyn_Upper_b;
_temp441.f1= _temp436; _temp441;}); _temp440;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL433: return Cyc_Parse_apply_tms( _temp383, Cyc_Absyn_tagged_typ(
t, _temp385, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL427:;}
_LL374: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp389),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL362:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp444; void* _temp446; struct _tuple5 _temp442= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL447: _temp446= _temp442.f1; goto _LL445; _LL445: _temp444=
_temp442.f2; goto _LL443; _LL443: if( _temp444 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp446;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp448=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp448[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp449; _temp449.tag= Cyc_Absyn_Decl_s;
_temp449.f1= d; _temp449.f2= s; _temp449;}); _temp448;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err( _tag_arr("inline is only allowed on function definitions",
sizeof( unsigned char), 47u), loc);} if( tss ==  0){ Cyc_Parse_err( _tag_arr("missing type specifiers in declaration",
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ switch(( enum 
Cyc_Parse_Storage_class)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v){ case
Cyc_Parse_Typedef_sc: _LL450: istypedef= 1; break; case Cyc_Parse_Extern_sc:
_LL451: s=( void*) Cyc_Absyn_Extern; break; case Cyc_Parse_ExternC_sc: _LL452: s=(
void*) Cyc_Absyn_ExternC; break; case Cyc_Parse_Static_sc: _LL453: s=( void*)
Cyc_Absyn_Static; break; case Cyc_Parse_Auto_sc: _LL454: s=( void*) Cyc_Absyn_Public;
break; case Cyc_Parse_Register_sc: _LL455: s=( void*) Cyc_Absyn_Public; break;
case Cyc_Parse_Abstract_sc: _LL456: s=( void*) Cyc_Absyn_Abstract; break;}}{
struct Cyc_List_List* _temp460; struct Cyc_List_List* _temp462; struct _tuple0
_temp458=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( ids);
_LL463: _temp462= _temp458.f1; goto _LL461; _LL461: _temp460= _temp458.f2; goto
_LL459; _LL459: { int exps_empty= 1;{ struct Cyc_List_List* es= _temp460; for( 0;
es !=  0; es=(( struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd !=  0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp464= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp462 ==  0){ struct Cyc_Core_Opt* _temp467; void* _temp469; struct _tuple5
_temp465= _temp464; _LL470: _temp469= _temp465.f1; goto _LL468; _LL468: _temp467=
_temp465.f2; goto _LL466; _LL466: if( _temp467 !=  0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp467))->v;{
void* _temp471=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp483; struct Cyc_Absyn_Structdecl*
_temp485; struct Cyc_Absyn_Uniondecl* _temp487; struct Cyc_Absyn_Tuniondecl*
_temp489; _LL473: if(*(( int*) _temp471) ==  Cyc_Absyn_Enum_d){ _LL484: _temp483=((
struct Cyc_Absyn_Enum_d_struct*) _temp471)->f1; goto _LL474;} else{ goto _LL475;}
_LL475: if(*(( int*) _temp471) ==  Cyc_Absyn_Struct_d){ _LL486: _temp485=((
struct Cyc_Absyn_Struct_d_struct*) _temp471)->f1; goto _LL476;} else{ goto
_LL477;} _LL477: if(*(( int*) _temp471) ==  Cyc_Absyn_Union_d){ _LL488: _temp487=((
struct Cyc_Absyn_Union_d_struct*) _temp471)->f1; goto _LL478;} else{ goto _LL479;}
_LL479: if(*(( int*) _temp471) ==  Cyc_Absyn_Tunion_d){ _LL490: _temp489=((
struct Cyc_Absyn_Tunion_d_struct*) _temp471)->f1; goto _LL480;} else{ goto
_LL481;} _LL481: goto _LL482; _LL474:( void*)( _temp483->sc=( void*) s); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL472; _LL476:( void*)( _temp485->sc=( void*)
s); _temp485->attributes= atts; goto _LL472; _LL478:( void*)( _temp487->sc=(
void*) s); _temp487->attributes= atts; goto _LL472; _LL480:( void*)( _temp489->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL472; _LL482: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL472:;}
return({ struct Cyc_List_List* _temp491=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp491->hd=( void*) d; _temp491->tl= 0;
_temp491;});} else{ void* _temp492= _temp469; struct Cyc_List_List* _temp506;
struct _tuple1* _temp508; struct Cyc_Absyn_TunionInfo _temp510; void* _temp512;
struct Cyc_Absyn_Tuniondecl* _temp514; struct Cyc_Absyn_TunionInfo _temp516;
struct Cyc_List_List* _temp518; void* _temp520; struct Cyc_Absyn_UnknownTunionInfo
_temp522; int _temp524; struct _tuple1* _temp526; struct Cyc_List_List* _temp528;
struct _tuple1* _temp530; struct _tuple1* _temp532; _LL494: if(( unsigned int)
_temp492 >  4u?*(( int*) _temp492) ==  Cyc_Absyn_StructType: 0){ _LL509:
_temp508=(( struct Cyc_Absyn_StructType_struct*) _temp492)->f1; goto _LL507;
_LL507: _temp506=(( struct Cyc_Absyn_StructType_struct*) _temp492)->f2; goto
_LL495;} else{ goto _LL496;} _LL496: if(( unsigned int) _temp492 >  4u?*(( int*)
_temp492) ==  Cyc_Absyn_TunionType: 0){ _LL511: _temp510=(( struct Cyc_Absyn_TunionType_struct*)
_temp492)->f1; _LL513: _temp512=( void*) _temp510.tunion_info; if(*(( int*)
_temp512) ==  Cyc_Absyn_KnownTunion){ _LL515: _temp514=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp512)->f1; goto _LL497;} else{ goto _LL498;}} else{ goto _LL498;} _LL498:
if(( unsigned int) _temp492 >  4u?*(( int*) _temp492) ==  Cyc_Absyn_TunionType:
0){ _LL517: _temp516=(( struct Cyc_Absyn_TunionType_struct*) _temp492)->f1;
_LL521: _temp520=( void*) _temp516.tunion_info; if(*(( int*) _temp520) ==  Cyc_Absyn_UnknownTunion){
_LL523: _temp522=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp520)->f1;
_LL527: _temp526= _temp522.name; goto _LL525; _LL525: _temp524= _temp522.is_xtunion;
goto _LL519;} else{ goto _LL500;} _LL519: _temp518= _temp516.targs; goto _LL499;}
else{ goto _LL500;} _LL500: if(( unsigned int) _temp492 >  4u?*(( int*) _temp492)
==  Cyc_Absyn_UnionType: 0){ _LL531: _temp530=(( struct Cyc_Absyn_UnionType_struct*)
_temp492)->f1; goto _LL529; _LL529: _temp528=(( struct Cyc_Absyn_UnionType_struct*)
_temp492)->f2; goto _LL501;} else{ goto _LL502;} _LL502: if(( unsigned int)
_temp492 >  4u?*(( int*) _temp492) ==  Cyc_Absyn_EnumType: 0){ _LL533: _temp532=((
struct Cyc_Absyn_EnumType_struct*) _temp492)->f1; goto _LL503;} else{ goto
_LL504;} _LL504: goto _LL505; _LL495: { struct Cyc_List_List* _temp534=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp506); struct Cyc_Absyn_Structdecl* _temp535=({
struct Cyc_Absyn_Structdecl* _temp539=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp539->sc=( void*) s; _temp539->name=({
struct Cyc_Core_Opt* _temp540=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp540->v=( void*)(( struct _tuple1*) _check_null( _temp508));
_temp540;}); _temp539->tvs= _temp534; _temp539->fields= 0; _temp539->attributes=
0; _temp539;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp536=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp536->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp537=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp537[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp538; _temp538.tag= Cyc_Absyn_Struct_d;
_temp538.f1= _temp535; _temp538;}); _temp537;}), loc); _temp536->tl= 0; _temp536;});}
_LL497: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp541=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp541->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp542=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp542[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp543; _temp543.tag= Cyc_Absyn_Tunion_d;
_temp543.f1= _temp514; _temp543;}); _temp542;}), loc); _temp541->tl= 0; _temp541;});
_LL499: { struct Cyc_List_List* _temp544=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp518);
struct Cyc_Absyn_Decl* _temp545= Cyc_Absyn_tunion_decl( s, _temp526, _temp544, 0,
_temp524, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp546=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp546->hd=(
void*) _temp545; _temp546->tl= 0; _temp546;});} _LL501: { struct Cyc_List_List*
_temp547=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp528); struct Cyc_Absyn_Uniondecl* _temp548=({
struct Cyc_Absyn_Uniondecl* _temp553=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp553->sc=( void*) s; _temp553->name=({
struct Cyc_Core_Opt* _temp554=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp554->v=( void*)(( struct _tuple1*) _check_null( _temp530));
_temp554;}); _temp553->tvs= _temp547; _temp553->fields= 0; _temp553->attributes=
0; _temp553;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp549=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp549->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp550=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp550->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp551=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp551[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp552; _temp552.tag= Cyc_Absyn_Union_d;
_temp552.f1= _temp548; _temp552;}); _temp551;})); _temp550->loc= loc; _temp550;});
_temp549->tl= 0; _temp549;});} _LL503: { struct Cyc_Absyn_Enumdecl* _temp555=({
struct Cyc_Absyn_Enumdecl* _temp560=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp560->sc=( void*) s; _temp560->name=
_temp532; _temp560->fields= 0; _temp560;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp556=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp556->hd=( void*)({ struct Cyc_Absyn_Decl* _temp557=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp557->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp558=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp558[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp559; _temp559.tag= Cyc_Absyn_Enum_d; _temp559.f1= _temp555; _temp559;});
_temp558;})); _temp557->loc= loc; _temp557;}); _temp556->tl= 0; _temp556;});}
_LL505: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL493:;}} else{ void* t= _temp464.f1; struct Cyc_List_List*
_temp561= Cyc_Parse_apply_tmss( tq, t, _temp462, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp561); if( _temp464.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp464.f2))->v;{ void* _temp562=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp574; struct Cyc_Absyn_Tuniondecl* _temp576;
struct Cyc_Absyn_Uniondecl* _temp578; struct Cyc_Absyn_Enumdecl* _temp580;
_LL564: if(*(( int*) _temp562) ==  Cyc_Absyn_Struct_d){ _LL575: _temp574=((
struct Cyc_Absyn_Struct_d_struct*) _temp562)->f1; goto _LL565;} else{ goto
_LL566;} _LL566: if(*(( int*) _temp562) ==  Cyc_Absyn_Tunion_d){ _LL577:
_temp576=(( struct Cyc_Absyn_Tunion_d_struct*) _temp562)->f1; goto _LL567;}
else{ goto _LL568;} _LL568: if(*(( int*) _temp562) ==  Cyc_Absyn_Union_d){
_LL579: _temp578=(( struct Cyc_Absyn_Union_d_struct*) _temp562)->f1; goto _LL569;}
else{ goto _LL570;} _LL570: if(*(( int*) _temp562) ==  Cyc_Absyn_Enum_d){ _LL581:
_temp580=(( struct Cyc_Absyn_Enum_d_struct*) _temp562)->f1; goto _LL571;} else{
goto _LL572;} _LL572: goto _LL573; _LL565:( void*)( _temp574->sc=( void*) s);
_temp574->attributes= atts; atts= 0; goto _LL563; _LL567:( void*)( _temp576->sc=(
void*) s); goto _LL563; _LL569:( void*)( _temp578->sc=( void*) s); goto _LL563;
_LL571:( void*)( _temp580->sc=( void*) s); goto _LL563; _LL573: Cyc_Parse_err(
_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL563; _LL563:;} decls=({ struct Cyc_List_List*
_temp582=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp582->hd=( void*) d; _temp582->tl= decls; _temp582;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp584; _temp584.tag=
Cyc_Std_String_pa; _temp584.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp583[ 1u]={&
_temp584}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp583, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp464.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp585= _temp561; for( 0; _temp585 !=  0;( _temp585=(( struct
Cyc_List_List*) _check_null( _temp585))->tl, _temp460=(( struct Cyc_List_List*)
_check_null( _temp460))->tl)){ struct _tuple7 _temp588; struct Cyc_List_List*
_temp589; struct Cyc_List_List* _temp591; void* _temp593; struct Cyc_Absyn_Tqual
_temp595; struct _tuple1* _temp597; struct _tuple7* _temp586=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp585))->hd; _temp588=* _temp586; _LL598:
_temp597= _temp588.f1; goto _LL596; _LL596: _temp595= _temp588.f2; goto _LL594;
_LL594: _temp593= _temp588.f3; goto _LL592; _LL592: _temp591= _temp588.f4; goto
_LL590; _LL590: _temp589= _temp588.f5; goto _LL587; _LL587: if( _temp591 !=  0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp460 ==  0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp599=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp460))->hd; struct Cyc_Absyn_Vardecl*
_temp600= Cyc_Absyn_new_vardecl( _temp597, _temp593, _temp599); _temp600->tq=
_temp595;( void*)( _temp600->sc=( void*) s); _temp600->attributes= _temp589;{
struct Cyc_Absyn_Decl* _temp601=({ struct Cyc_Absyn_Decl* _temp603=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp603->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp604=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp604[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp605; _temp605.tag= Cyc_Absyn_Var_d; _temp605.f1= _temp600; _temp605;});
_temp604;})); _temp603->loc= loc; _temp603;}); decls=({ struct Cyc_List_List*
_temp602=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp602->hd=( void*) _temp601; _temp602->tl= decls; _temp602;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp607; _temp607.tag= Cyc_Std_String_pa;
_temp607.f1=( struct _tagged_arr) s;{ void* _temp606[ 1u]={& _temp607}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp606,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL608: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL609: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL610: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL611: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL612: return(
void*) Cyc_Absyn_EffKind; default: _LL613: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp616; _temp616.tag= Cyc_Std_String_pa;
_temp616.f1=( struct _tagged_arr) s;{ void* _temp615[ 1u]={& _temp616}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp615,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp617=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp617->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp618=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp618[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp619; _temp619.tag= Cyc_Absyn_Attributes_mod;
_temp619.f1= loc; _temp619.f2= atts; _temp619;}); _temp618;})); _temp617->tl=
tms; _temp617;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp622;
struct Cyc_List_List* _temp623; struct Cyc_List_List* _temp625; void* _temp627;
struct Cyc_Absyn_Tqual _temp629; struct _tuple1* _temp631; struct _tuple7*
_temp620= t; _temp622=* _temp620; _LL632: _temp631= _temp622.f1; goto _LL630;
_LL630: _temp629= _temp622.f2; goto _LL628; _LL628: _temp627= _temp622.f3; goto
_LL626; _LL626: _temp625= _temp622.f4; goto _LL624; _LL624: _temp623= _temp622.f5;
goto _LL621; _LL621: Cyc_Lex_register_typedef( _temp631); if( _temp623 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp634;
_temp634.tag= Cyc_Std_String_pa; _temp634.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp623))->hd);{ void* _temp633[ 1u]={&
_temp634}; Cyc_Std_aprintf( _tag_arr("bad attribute %s within typedef", sizeof(
unsigned char), 32u), _tag_arr( _temp633, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp635=(
struct Cyc_Absyn_Typedef_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp635[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp636; _temp636.tag= Cyc_Absyn_Typedef_d;
_temp636.f1=({ struct Cyc_Absyn_Typedefdecl* _temp637=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp637->name= _temp631;
_temp637->tvs= _temp625; _temp637->defn=( void*) _temp627; _temp637;}); _temp636;});
_temp635;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; unsigned char Cyc_AttributeList_tok[ 22u]="\000\000\000\000AttributeList_tok";
struct Cyc_AttributeList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Attribute_tok[ 18u]="\000\000\000\000Attribute_tok";
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char
Cyc_Bool_tok[ 13u]="\000\000\000\000Bool_tok"; struct Cyc_Bool_tok_struct{
unsigned char* tag; int f1; } ; unsigned char Cyc_Char_tok[ 13u]="\000\000\000\000Char_tok";
struct Cyc_Char_tok_struct{ unsigned char* tag; unsigned char f1; } ;
unsigned char Cyc_DeclList_tok[ 17u]="\000\000\000\000DeclList_tok"; struct Cyc_DeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_DeclSpec_tok[
17u]="\000\000\000\000DeclSpec_tok"; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Parse_Declaration_spec* f1; } ; unsigned char Cyc_DeclaratorExpoptList_tok[
29u]="\000\000\000\000DeclaratorExpoptList_tok"; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_DeclaratorExpopt_tok[
25u]="\000\000\000\000DeclaratorExpopt_tok"; struct _tuple13{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_DeclaratorExpopt_tok_struct{
unsigned char* tag; struct _tuple13* f1; } ; unsigned char Cyc_Declarator_tok[
19u]="\000\000\000\000Declarator_tok"; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Parse_Declarator* f1; } ; unsigned char Cyc_DesignatorList_tok[
23u]="\000\000\000\000DesignatorList_tok"; struct Cyc_DesignatorList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_Designator_tok[
19u]="\000\000\000\000Designator_tok"; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_EnumfieldList_tok[ 22u]="\000\000\000\000EnumfieldList_tok";
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
17u]="\000\000\000\000InitDecl_tok"; struct Cyc_InitDecl_tok_struct{
unsigned char* tag; struct _tuple13* f1; } ; unsigned char Cyc_InitializerList_tok[
24u]="\000\000\000\000InitializerList_tok"; struct Cyc_InitializerList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_Int_tok[ 12u]="\000\000\000\000Int_tok";
struct _tuple15{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{ unsigned char*
tag; struct _tuple15* f1; } ; unsigned char Cyc_Kind_tok[ 13u]="\000\000\000\000Kind_tok";
struct Cyc_Kind_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_Okay_tok[
13u]="\000\000\000\000Okay_tok"; unsigned char Cyc_ParamDeclListBool_tok[ 26u]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple16{ struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo*
f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List* f5; } ; struct Cyc_ParamDeclListBool_tok_struct{
unsigned char* tag; struct _tuple16* f1; } ; unsigned char Cyc_ParamDeclList_tok[
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
21u]="\000\000\000\000QualSpecList_tok"; struct _tuple17{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple17* f1; } ; unsigned char Cyc_Rgnorder_tok[ 17u]="\000\000\000\000Rgnorder_tok";
struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; }
; unsigned char Cyc_Scope_tok[ 14u]="\000\000\000\000Scope_tok"; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Short_tok[ 14u]="\000\000\000\000Short_tok";
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; unsigned char Cyc_Stmt_tok[
13u]="\000\000\000\000Stmt_tok"; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; unsigned char Cyc_StorageClass_tok[ 21u]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; enum  Cyc_Parse_Storage_class
f1; } ; unsigned char Cyc_String_tok[ 15u]="\000\000\000\000String_tok"; struct
Cyc_String_tok_struct{ unsigned char* tag; struct _tagged_arr f1; } ;
unsigned char Cyc_Stringopt_tok[ 18u]="\000\000\000\000Stringopt_tok"; struct
Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; } ;
unsigned char Cyc_StructFieldDeclListList_tok[ 32u]="\000\000\000\000StructFieldDeclListList_tok";
struct Cyc_StructFieldDeclListList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_StructFieldDeclList_tok[ 28u]="\000\000\000\000StructFieldDeclList_tok";
struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_StructOrUnion_tok[ 22u]="\000\000\000\000StructOrUnion_tok";
struct Cyc_StructOrUnion_tok_struct{ unsigned char* tag; enum  Cyc_Parse_Struct_or_union
f1; } ; unsigned char Cyc_SwitchCClauseList_tok[ 26u]="\000\000\000\000SwitchCClauseList_tok";
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
unsigned char _temp639[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp639, _temp639, _temp639 +  8u}}; static unsigned char
_temp641[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{ _temp641, _temp641, _temp641 +  11u}}; static unsigned char
_temp643[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{ _temp643, _temp643, _temp643 +  9u}}; static unsigned char
_temp645[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={
Cyc_Core_Failure,{ _temp645, _temp645, _temp645 +  17u}}; static unsigned char
_temp647[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp647, _temp647, _temp647 +  8u}}; static unsigned char
_temp649[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp649, _temp649, _temp649 +  12u}}; static unsigned char
_temp651[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp651, _temp651, _temp651 +  20u}}; static unsigned char
_temp653[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp653, _temp653, _temp653 +  11u}}; static unsigned char
_temp655[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={
Cyc_Core_Failure,{ _temp655, _temp655, _temp655 +  14u}}; static unsigned char
_temp657[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp657, _temp657, _temp657 +  11u}}; static unsigned char
_temp659[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{ _temp659, _temp659, _temp659 +  9u}}; static unsigned char
_temp661[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{ _temp661, _temp661, _temp661 +  21u}}; static unsigned char
_temp663[ 22u]="SwitchCClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{ _temp663, _temp663,
_temp663 +  22u}}; static unsigned char _temp665[ 12u]="Pattern_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp665, _temp665, _temp665 +  12u}}; static unsigned char _temp667[ 16u]="PatternList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{
_temp667, _temp667, _temp667 +  16u}}; static unsigned char _temp669[ 17u]="FieldPattern_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp669, _temp669, _temp669 +  17u}}; static unsigned char _temp671[ 21u]="FieldPatternList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp671, _temp671, _temp671 +  21u}}; static unsigned char _temp673[ 11u]="FnDecl_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{
_temp673, _temp673, _temp673 +  11u}}; static unsigned char _temp675[ 13u]="DeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp675, _temp675, _temp675 +  13u}}; static unsigned char _temp677[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp677, _temp677, _temp677 +  13u}}; static unsigned char _temp679[ 13u]="InitDecl_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{
_temp679, _temp679, _temp679 +  13u}}; static unsigned char _temp681[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp681, _temp681, _temp681 +  17u}}; static unsigned char _temp683[ 17u]="StorageClass_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{
_temp683, _temp683, _temp683 +  17u}}; static unsigned char _temp685[ 18u]="TypeSpecifier_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp685, _temp685, _temp685 +  18u}}; static unsigned char _temp687[ 18u]="StructOrUnion_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp687, _temp687, _temp687 +  18u}}; static unsigned char _temp689[ 13u]="TypeQual_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{
_temp689, _temp689, _temp689 +  13u}}; static unsigned char _temp691[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp691, _temp691, _temp691 +  24u}}; static unsigned char _temp693[ 28u]="StructFieldDeclListList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp693, _temp693, _temp693 +  28u}}; static unsigned char
_temp695[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={
Cyc_Core_Failure,{ _temp695, _temp695, _temp695 +  21u}}; static unsigned char
_temp697[ 15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={
Cyc_Core_Failure,{ _temp697, _temp697, _temp697 +  15u}}; static unsigned char
_temp699[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpopt_tok={
Cyc_Core_Failure,{ _temp699, _temp699, _temp699 +  21u}}; static unsigned char
_temp701[ 25u]="DeclaratorExpoptList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{ _temp701, _temp701,
_temp701 +  25u}}; static unsigned char _temp703[ 23u]="AbstractDeclarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp703, _temp703, _temp703 +  23u}}; static unsigned char _temp705[ 9u]="Bool_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{
_temp705, _temp705, _temp705 +  9u}}; static unsigned char _temp707[ 10u]="Scope_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{
_temp707, _temp707, _temp707 +  10u}}; static unsigned char _temp709[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp709, _temp709, _temp709 +  16u}}; static unsigned char _temp711[ 20u]="TunionFieldList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp711, _temp711, _temp711 +  20u}}; static unsigned char _temp713[ 17u]="QualSpecList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp713, _temp713, _temp713 +  17u}}; static unsigned char _temp715[ 11u]="IdList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp715, _temp715, _temp715 +  11u}}; static unsigned char _temp717[ 14u]="ParamDecl_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp717, _temp717, _temp717 +  14u}}; static unsigned char _temp719[ 18u]="ParamDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp719, _temp719, _temp719 +  18u}}; static unsigned char _temp721[ 22u]="ParamDeclListBool_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp721, _temp721, _temp721 +  22u}}; static unsigned char _temp723[ 13u]="TypeList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{
_temp723, _temp723, _temp723 +  13u}}; static unsigned char _temp725[ 19u]="DesignatorList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp725, _temp725, _temp725 +  19u}}; static unsigned char _temp727[ 15u]="Designator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp727, _temp727, _temp727 +  15u}}; static unsigned char _temp729[ 9u]="Kind_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp729, _temp729, _temp729 +  9u}}; static unsigned char _temp731[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp731, _temp731, _temp731 +  9u}}; static unsigned char _temp733[ 18u]="AttributeList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp733, _temp733, _temp733 +  18u}}; static unsigned char _temp735[ 14u]="Attribute_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp735, _temp735, _temp735 +  14u}}; static unsigned char _temp737[ 14u]="Enumfield_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp737, _temp737, _temp737 +  14u}}; static unsigned char _temp739[ 18u]="EnumfieldList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp739, _temp739, _temp739 +  18u}}; static unsigned char _temp741[ 12u]="TypeOpt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp741, _temp741, _temp741 +  12u}}; static unsigned char _temp743[ 13u]="Rgnorder_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp743, _temp743, _temp743 +  13u}}; struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; } ; struct Cyc_Yyltype
Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp744; _temp744.timestamp= 0;
_temp744.first_line= 0; _temp744.first_column= 0; _temp744.last_line= 0;
_temp744.last_column= 0; _temp744;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0,
0, 0}; static short Cyc_yytranslate[ 348u]={ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 119, 2, 2, 101,
117, 113, 2, 97, 98, 108, 111, 99, 112, 105, 116, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
104, 93, 102, 96, 103, 110, 109, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 106, 2, 107, 115, 100, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 94, 114, 95, 118, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
90, 91, 92}; static unsigned char _temp745[ 2u]="$"; static unsigned char
_temp746[ 6u]="error"; static unsigned char _temp747[ 12u]="$undefined."; static
unsigned char _temp748[ 5u]="AUTO"; static unsigned char _temp749[ 9u]="REGISTER";
static unsigned char _temp750[ 7u]="STATIC"; static unsigned char _temp751[ 7u]="EXTERN";
static unsigned char _temp752[ 8u]="TYPEDEF"; static unsigned char _temp753[ 5u]="VOID";
static unsigned char _temp754[ 5u]="CHAR"; static unsigned char _temp755[ 6u]="SHORT";
static unsigned char _temp756[ 4u]="INT"; static unsigned char _temp757[ 5u]="LONG";
static unsigned char _temp758[ 6u]="FLOAT"; static unsigned char _temp759[ 7u]="DOUBLE";
static unsigned char _temp760[ 7u]="SIGNED"; static unsigned char _temp761[ 9u]="UNSIGNED";
static unsigned char _temp762[ 6u]="CONST"; static unsigned char _temp763[ 9u]="VOLATILE";
static unsigned char _temp764[ 9u]="RESTRICT"; static unsigned char _temp765[ 7u]="STRUCT";
static unsigned char _temp766[ 6u]="UNION"; static unsigned char _temp767[ 5u]="CASE";
static unsigned char _temp768[ 8u]="DEFAULT"; static unsigned char _temp769[ 7u]="INLINE";
static unsigned char _temp770[ 7u]="SIZEOF"; static unsigned char _temp771[ 9u]="OFFSETOF";
static unsigned char _temp772[ 3u]="IF"; static unsigned char _temp773[ 5u]="ELSE";
static unsigned char _temp774[ 7u]="SWITCH"; static unsigned char _temp775[ 6u]="WHILE";
static unsigned char _temp776[ 3u]="DO"; static unsigned char _temp777[ 4u]="FOR";
static unsigned char _temp778[ 5u]="GOTO"; static unsigned char _temp779[ 9u]="CONTINUE";
static unsigned char _temp780[ 6u]="BREAK"; static unsigned char _temp781[ 7u]="RETURN";
static unsigned char _temp782[ 5u]="ENUM"; static unsigned char _temp783[ 8u]="NULL_kw";
static unsigned char _temp784[ 4u]="LET"; static unsigned char _temp785[ 6u]="THROW";
static unsigned char _temp786[ 4u]="TRY"; static unsigned char _temp787[ 6u]="CATCH";
static unsigned char _temp788[ 4u]="NEW"; static unsigned char _temp789[ 9u]="ABSTRACT";
static unsigned char _temp790[ 9u]="FALLTHRU"; static unsigned char _temp791[ 6u]="USING";
static unsigned char _temp792[ 10u]="NAMESPACE"; static unsigned char _temp793[
7u]="TUNION"; static unsigned char _temp794[ 8u]="XTUNION"; static unsigned char
_temp795[ 5u]="FILL"; static unsigned char _temp796[ 8u]="CODEGEN"; static
unsigned char _temp797[ 4u]="CUT"; static unsigned char _temp798[ 7u]="SPLICE";
static unsigned char _temp799[ 7u]="MALLOC"; static unsigned char _temp800[ 9u]="REGION_T";
static unsigned char _temp801[ 7u]="REGION"; static unsigned char _temp802[ 5u]="RNEW";
static unsigned char _temp803[ 8u]="RMALLOC"; static unsigned char _temp804[ 8u]="REGIONS";
static unsigned char _temp805[ 7u]="PTR_OP"; static unsigned char _temp806[ 7u]="INC_OP";
static unsigned char _temp807[ 7u]="DEC_OP"; static unsigned char _temp808[ 8u]="LEFT_OP";
static unsigned char _temp809[ 9u]="RIGHT_OP"; static unsigned char _temp810[ 6u]="LE_OP";
static unsigned char _temp811[ 6u]="GE_OP"; static unsigned char _temp812[ 6u]="EQ_OP";
static unsigned char _temp813[ 6u]="NE_OP"; static unsigned char _temp814[ 7u]="AND_OP";
static unsigned char _temp815[ 6u]="OR_OP"; static unsigned char _temp816[ 11u]="MUL_ASSIGN";
static unsigned char _temp817[ 11u]="DIV_ASSIGN"; static unsigned char _temp818[
11u]="MOD_ASSIGN"; static unsigned char _temp819[ 11u]="ADD_ASSIGN"; static
unsigned char _temp820[ 11u]="SUB_ASSIGN"; static unsigned char _temp821[ 12u]="LEFT_ASSIGN";
static unsigned char _temp822[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp823[ 11u]="AND_ASSIGN"; static unsigned char _temp824[ 11u]="XOR_ASSIGN";
static unsigned char _temp825[ 10u]="OR_ASSIGN"; static unsigned char _temp826[
9u]="ELLIPSIS"; static unsigned char _temp827[ 11u]="LEFT_RIGHT"; static
unsigned char _temp828[ 12u]="COLON_COLON"; static unsigned char _temp829[ 11u]="IDENTIFIER";
static unsigned char _temp830[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp831[ 7u]="STRING"; static unsigned char _temp832[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp833[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp834[ 9u]="TYPE_VAR"; static unsigned char _temp835[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp836[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp837[ 10u]="ATTRIBUTE"; static unsigned char _temp838[ 4u]="';'"; static
unsigned char _temp839[ 4u]="'{'"; static unsigned char _temp840[ 4u]="'}'";
static unsigned char _temp841[ 4u]="'='"; static unsigned char _temp842[ 4u]="'('";
static unsigned char _temp843[ 4u]="')'"; static unsigned char _temp844[ 4u]="','";
static unsigned char _temp845[ 4u]="'_'"; static unsigned char _temp846[ 4u]="'$'";
static unsigned char _temp847[ 4u]="'<'"; static unsigned char _temp848[ 4u]="'>'";
static unsigned char _temp849[ 4u]="':'"; static unsigned char _temp850[ 4u]="'.'";
static unsigned char _temp851[ 4u]="'['"; static unsigned char _temp852[ 4u]="']'";
static unsigned char _temp853[ 4u]="'*'"; static unsigned char _temp854[ 4u]="'@'";
static unsigned char _temp855[ 4u]="'?'"; static unsigned char _temp856[ 4u]="'+'";
static unsigned char _temp857[ 4u]="'-'"; static unsigned char _temp858[ 4u]="'&'";
static unsigned char _temp859[ 4u]="'|'"; static unsigned char _temp860[ 4u]="'^'";
static unsigned char _temp861[ 4u]="'/'"; static unsigned char _temp862[ 4u]="'%'";
static unsigned char _temp863[ 4u]="'~'"; static unsigned char _temp864[ 4u]="'!'";
static unsigned char _temp865[ 5u]="prog"; static unsigned char _temp866[ 17u]="translation_unit";
static unsigned char _temp867[ 21u]="external_declaration"; static unsigned char
_temp868[ 20u]="function_definition"; static unsigned char _temp869[ 21u]="function_definition2";
static unsigned char _temp870[ 13u]="using_action"; static unsigned char
_temp871[ 15u]="unusing_action"; static unsigned char _temp872[ 17u]="namespace_action";
static unsigned char _temp873[ 19u]="unnamespace_action"; static unsigned char
_temp874[ 12u]="declaration"; static unsigned char _temp875[ 17u]="declaration_list";
static unsigned char _temp876[ 23u]="declaration_specifiers"; static
unsigned char _temp877[ 24u]="storage_class_specifier"; static unsigned char
_temp878[ 15u]="attributes_opt"; static unsigned char _temp879[ 11u]="attributes";
static unsigned char _temp880[ 15u]="attribute_list"; static unsigned char
_temp881[ 10u]="attribute"; static unsigned char _temp882[ 15u]="type_specifier";
static unsigned char _temp883[ 5u]="kind"; static unsigned char _temp884[ 15u]="type_qualifier";
static unsigned char _temp885[ 15u]="enum_specifier"; static unsigned char
_temp886[ 11u]="enum_field"; static unsigned char _temp887[ 22u]="enum_declaration_list";
static unsigned char _temp888[ 26u]="struct_or_union_specifier"; static
unsigned char _temp889[ 16u]="type_params_opt"; static unsigned char _temp890[
16u]="struct_or_union"; static unsigned char _temp891[ 24u]="struct_declaration_list";
static unsigned char _temp892[ 25u]="struct_declaration_list0"; static
unsigned char _temp893[ 21u]="init_declarator_list"; static unsigned char
_temp894[ 22u]="init_declarator_list0"; static unsigned char _temp895[ 16u]="init_declarator";
static unsigned char _temp896[ 19u]="struct_declaration"; static unsigned char
_temp897[ 25u]="specifier_qualifier_list"; static unsigned char _temp898[ 23u]="struct_declarator_list";
static unsigned char _temp899[ 24u]="struct_declarator_list0"; static
unsigned char _temp900[ 18u]="struct_declarator"; static unsigned char _temp901[
17u]="tunion_specifier"; static unsigned char _temp902[ 18u]="tunion_or_xtunion";
static unsigned char _temp903[ 17u]="tunionfield_list"; static unsigned char
_temp904[ 18u]="tunionfield_scope"; static unsigned char _temp905[ 12u]="tunionfield";
static unsigned char _temp906[ 11u]="declarator"; static unsigned char _temp907[
18u]="direct_declarator"; static unsigned char _temp908[ 8u]="pointer"; static
unsigned char _temp909[ 13u]="pointer_char"; static unsigned char _temp910[ 8u]="rgn_opt";
static unsigned char _temp911[ 4u]="rgn"; static unsigned char _temp912[ 20u]="type_qualifier_list";
static unsigned char _temp913[ 20u]="parameter_type_list"; static unsigned char
_temp914[ 9u]="type_var"; static unsigned char _temp915[ 16u]="optional_effect";
static unsigned char _temp916[ 19u]="optional_rgn_order"; static unsigned char
_temp917[ 10u]="rgn_order"; static unsigned char _temp918[ 16u]="optional_inject";
static unsigned char _temp919[ 11u]="effect_set"; static unsigned char _temp920[
14u]="atomic_effect"; static unsigned char _temp921[ 11u]="region_set"; static
unsigned char _temp922[ 15u]="parameter_list"; static unsigned char _temp923[ 22u]="parameter_declaration";
static unsigned char _temp924[ 16u]="identifier_list"; static unsigned char
_temp925[ 17u]="identifier_list0"; static unsigned char _temp926[ 12u]="initializer";
static unsigned char _temp927[ 18u]="array_initializer"; static unsigned char
_temp928[ 17u]="initializer_list"; static unsigned char _temp929[ 12u]="designation";
static unsigned char _temp930[ 16u]="designator_list"; static unsigned char
_temp931[ 11u]="designator"; static unsigned char _temp932[ 10u]="type_name";
static unsigned char _temp933[ 14u]="any_type_name"; static unsigned char
_temp934[ 15u]="type_name_list"; static unsigned char _temp935[ 20u]="abstract_declarator";
static unsigned char _temp936[ 27u]="direct_abstract_declarator"; static
unsigned char _temp937[ 10u]="statement"; static unsigned char _temp938[ 18u]="labeled_statement";
static unsigned char _temp939[ 21u]="expression_statement"; static unsigned char
_temp940[ 19u]="compound_statement"; static unsigned char _temp941[ 16u]="block_item_list";
static unsigned char _temp942[ 20u]="selection_statement"; static unsigned char
_temp943[ 15u]="switch_clauses"; static unsigned char _temp944[ 16u]="switchC_clauses";
static unsigned char _temp945[ 20u]="iteration_statement"; static unsigned char
_temp946[ 15u]="jump_statement"; static unsigned char _temp947[ 8u]="pattern";
static unsigned char _temp948[ 19u]="tuple_pattern_list"; static unsigned char
_temp949[ 20u]="tuple_pattern_list0"; static unsigned char _temp950[ 14u]="field_pattern";
static unsigned char _temp951[ 19u]="field_pattern_list"; static unsigned char
_temp952[ 20u]="field_pattern_list0"; static unsigned char _temp953[ 11u]="expression";
static unsigned char _temp954[ 22u]="assignment_expression"; static
unsigned char _temp955[ 20u]="assignment_operator"; static unsigned char
_temp956[ 23u]="conditional_expression"; static unsigned char _temp957[ 20u]="constant_expression";
static unsigned char _temp958[ 22u]="logical_or_expression"; static
unsigned char _temp959[ 23u]="logical_and_expression"; static unsigned char
_temp960[ 24u]="inclusive_or_expression"; static unsigned char _temp961[ 24u]="exclusive_or_expression";
static unsigned char _temp962[ 15u]="and_expression"; static unsigned char
_temp963[ 20u]="equality_expression"; static unsigned char _temp964[ 22u]="relational_expression";
static unsigned char _temp965[ 17u]="shift_expression"; static unsigned char
_temp966[ 20u]="additive_expression"; static unsigned char _temp967[ 26u]="multiplicative_expression";
static unsigned char _temp968[ 16u]="cast_expression"; static unsigned char
_temp969[ 17u]="unary_expression"; static unsigned char _temp970[ 15u]="unary_operator";
static unsigned char _temp971[ 19u]="postfix_expression"; static unsigned char
_temp972[ 19u]="primary_expression"; static unsigned char _temp973[ 25u]="argument_expression_list";
static unsigned char _temp974[ 26u]="argument_expression_list0"; static
unsigned char _temp975[ 9u]="constant"; static unsigned char _temp976[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 232u]={{ _temp745, _temp745, _temp745 + 
2u},{ _temp746, _temp746, _temp746 +  6u},{ _temp747, _temp747, _temp747 +  12u},{
_temp748, _temp748, _temp748 +  5u},{ _temp749, _temp749, _temp749 +  9u},{
_temp750, _temp750, _temp750 +  7u},{ _temp751, _temp751, _temp751 +  7u},{
_temp752, _temp752, _temp752 +  8u},{ _temp753, _temp753, _temp753 +  5u},{
_temp754, _temp754, _temp754 +  5u},{ _temp755, _temp755, _temp755 +  6u},{
_temp756, _temp756, _temp756 +  4u},{ _temp757, _temp757, _temp757 +  5u},{
_temp758, _temp758, _temp758 +  6u},{ _temp759, _temp759, _temp759 +  7u},{
_temp760, _temp760, _temp760 +  7u},{ _temp761, _temp761, _temp761 +  9u},{
_temp762, _temp762, _temp762 +  6u},{ _temp763, _temp763, _temp763 +  9u},{
_temp764, _temp764, _temp764 +  9u},{ _temp765, _temp765, _temp765 +  7u},{
_temp766, _temp766, _temp766 +  6u},{ _temp767, _temp767, _temp767 +  5u},{
_temp768, _temp768, _temp768 +  8u},{ _temp769, _temp769, _temp769 +  7u},{
_temp770, _temp770, _temp770 +  7u},{ _temp771, _temp771, _temp771 +  9u},{
_temp772, _temp772, _temp772 +  3u},{ _temp773, _temp773, _temp773 +  5u},{
_temp774, _temp774, _temp774 +  7u},{ _temp775, _temp775, _temp775 +  6u},{
_temp776, _temp776, _temp776 +  3u},{ _temp777, _temp777, _temp777 +  4u},{
_temp778, _temp778, _temp778 +  5u},{ _temp779, _temp779, _temp779 +  9u},{
_temp780, _temp780, _temp780 +  6u},{ _temp781, _temp781, _temp781 +  7u},{
_temp782, _temp782, _temp782 +  5u},{ _temp783, _temp783, _temp783 +  8u},{
_temp784, _temp784, _temp784 +  4u},{ _temp785, _temp785, _temp785 +  6u},{
_temp786, _temp786, _temp786 +  4u},{ _temp787, _temp787, _temp787 +  6u},{
_temp788, _temp788, _temp788 +  4u},{ _temp789, _temp789, _temp789 +  9u},{
_temp790, _temp790, _temp790 +  9u},{ _temp791, _temp791, _temp791 +  6u},{
_temp792, _temp792, _temp792 +  10u},{ _temp793, _temp793, _temp793 +  7u},{
_temp794, _temp794, _temp794 +  8u},{ _temp795, _temp795, _temp795 +  5u},{
_temp796, _temp796, _temp796 +  8u},{ _temp797, _temp797, _temp797 +  4u},{
_temp798, _temp798, _temp798 +  7u},{ _temp799, _temp799, _temp799 +  7u},{
_temp800, _temp800, _temp800 +  9u},{ _temp801, _temp801, _temp801 +  7u},{
_temp802, _temp802, _temp802 +  5u},{ _temp803, _temp803, _temp803 +  8u},{
_temp804, _temp804, _temp804 +  8u},{ _temp805, _temp805, _temp805 +  7u},{
_temp806, _temp806, _temp806 +  7u},{ _temp807, _temp807, _temp807 +  7u},{
_temp808, _temp808, _temp808 +  8u},{ _temp809, _temp809, _temp809 +  9u},{
_temp810, _temp810, _temp810 +  6u},{ _temp811, _temp811, _temp811 +  6u},{
_temp812, _temp812, _temp812 +  6u},{ _temp813, _temp813, _temp813 +  6u},{
_temp814, _temp814, _temp814 +  7u},{ _temp815, _temp815, _temp815 +  6u},{
_temp816, _temp816, _temp816 +  11u},{ _temp817, _temp817, _temp817 +  11u},{
_temp818, _temp818, _temp818 +  11u},{ _temp819, _temp819, _temp819 +  11u},{
_temp820, _temp820, _temp820 +  11u},{ _temp821, _temp821, _temp821 +  12u},{
_temp822, _temp822, _temp822 +  13u},{ _temp823, _temp823, _temp823 +  11u},{
_temp824, _temp824, _temp824 +  11u},{ _temp825, _temp825, _temp825 +  10u},{
_temp826, _temp826, _temp826 +  9u},{ _temp827, _temp827, _temp827 +  11u},{
_temp828, _temp828, _temp828 +  12u},{ _temp829, _temp829, _temp829 +  11u},{
_temp830, _temp830, _temp830 +  17u},{ _temp831, _temp831, _temp831 +  7u},{
_temp832, _temp832, _temp832 +  19u},{ _temp833, _temp833, _temp833 +  18u},{
_temp834, _temp834, _temp834 +  9u},{ _temp835, _temp835, _temp835 +  16u},{
_temp836, _temp836, _temp836 +  18u},{ _temp837, _temp837, _temp837 +  10u},{
_temp838, _temp838, _temp838 +  4u},{ _temp839, _temp839, _temp839 +  4u},{
_temp840, _temp840, _temp840 +  4u},{ _temp841, _temp841, _temp841 +  4u},{
_temp842, _temp842, _temp842 +  4u},{ _temp843, _temp843, _temp843 +  4u},{
_temp844, _temp844, _temp844 +  4u},{ _temp845, _temp845, _temp845 +  4u},{
_temp846, _temp846, _temp846 +  4u},{ _temp847, _temp847, _temp847 +  4u},{
_temp848, _temp848, _temp848 +  4u},{ _temp849, _temp849, _temp849 +  4u},{
_temp850, _temp850, _temp850 +  4u},{ _temp851, _temp851, _temp851 +  4u},{
_temp852, _temp852, _temp852 +  4u},{ _temp853, _temp853, _temp853 +  4u},{
_temp854, _temp854, _temp854 +  4u},{ _temp855, _temp855, _temp855 +  4u},{
_temp856, _temp856, _temp856 +  4u},{ _temp857, _temp857, _temp857 +  4u},{
_temp858, _temp858, _temp858 +  4u},{ _temp859, _temp859, _temp859 +  4u},{
_temp860, _temp860, _temp860 +  4u},{ _temp861, _temp861, _temp861 +  4u},{
_temp862, _temp862, _temp862 +  4u},{ _temp863, _temp863, _temp863 +  4u},{
_temp864, _temp864, _temp864 +  4u},{ _temp865, _temp865, _temp865 +  5u},{
_temp866, _temp866, _temp866 +  17u},{ _temp867, _temp867, _temp867 +  21u},{
_temp868, _temp868, _temp868 +  20u},{ _temp869, _temp869, _temp869 +  21u},{
_temp870, _temp870, _temp870 +  13u},{ _temp871, _temp871, _temp871 +  15u},{
_temp872, _temp872, _temp872 +  17u},{ _temp873, _temp873, _temp873 +  19u},{
_temp874, _temp874, _temp874 +  12u},{ _temp875, _temp875, _temp875 +  17u},{
_temp876, _temp876, _temp876 +  23u},{ _temp877, _temp877, _temp877 +  24u},{
_temp878, _temp878, _temp878 +  15u},{ _temp879, _temp879, _temp879 +  11u},{
_temp880, _temp880, _temp880 +  15u},{ _temp881, _temp881, _temp881 +  10u},{
_temp882, _temp882, _temp882 +  15u},{ _temp883, _temp883, _temp883 +  5u},{
_temp884, _temp884, _temp884 +  15u},{ _temp885, _temp885, _temp885 +  15u},{
_temp886, _temp886, _temp886 +  11u},{ _temp887, _temp887, _temp887 +  22u},{
_temp888, _temp888, _temp888 +  26u},{ _temp889, _temp889, _temp889 +  16u},{
_temp890, _temp890, _temp890 +  16u},{ _temp891, _temp891, _temp891 +  24u},{
_temp892, _temp892, _temp892 +  25u},{ _temp893, _temp893, _temp893 +  21u},{
_temp894, _temp894, _temp894 +  22u},{ _temp895, _temp895, _temp895 +  16u},{
_temp896, _temp896, _temp896 +  19u},{ _temp897, _temp897, _temp897 +  25u},{
_temp898, _temp898, _temp898 +  23u},{ _temp899, _temp899, _temp899 +  24u},{
_temp900, _temp900, _temp900 +  18u},{ _temp901, _temp901, _temp901 +  17u},{
_temp902, _temp902, _temp902 +  18u},{ _temp903, _temp903, _temp903 +  17u},{
_temp904, _temp904, _temp904 +  18u},{ _temp905, _temp905, _temp905 +  12u},{
_temp906, _temp906, _temp906 +  11u},{ _temp907, _temp907, _temp907 +  18u},{
_temp908, _temp908, _temp908 +  8u},{ _temp909, _temp909, _temp909 +  13u},{
_temp910, _temp910, _temp910 +  8u},{ _temp911, _temp911, _temp911 +  4u},{
_temp912, _temp912, _temp912 +  20u},{ _temp913, _temp913, _temp913 +  20u},{
_temp914, _temp914, _temp914 +  9u},{ _temp915, _temp915, _temp915 +  16u},{
_temp916, _temp916, _temp916 +  19u},{ _temp917, _temp917, _temp917 +  10u},{
_temp918, _temp918, _temp918 +  16u},{ _temp919, _temp919, _temp919 +  11u},{
_temp920, _temp920, _temp920 +  14u},{ _temp921, _temp921, _temp921 +  11u},{
_temp922, _temp922, _temp922 +  15u},{ _temp923, _temp923, _temp923 +  22u},{
_temp924, _temp924, _temp924 +  16u},{ _temp925, _temp925, _temp925 +  17u},{
_temp926, _temp926, _temp926 +  12u},{ _temp927, _temp927, _temp927 +  18u},{
_temp928, _temp928, _temp928 +  17u},{ _temp929, _temp929, _temp929 +  12u},{
_temp930, _temp930, _temp930 +  16u},{ _temp931, _temp931, _temp931 +  11u},{
_temp932, _temp932, _temp932 +  10u},{ _temp933, _temp933, _temp933 +  14u},{
_temp934, _temp934, _temp934 +  15u},{ _temp935, _temp935, _temp935 +  20u},{
_temp936, _temp936, _temp936 +  27u},{ _temp937, _temp937, _temp937 +  10u},{
_temp938, _temp938, _temp938 +  18u},{ _temp939, _temp939, _temp939 +  21u},{
_temp940, _temp940, _temp940 +  19u},{ _temp941, _temp941, _temp941 +  16u},{
_temp942, _temp942, _temp942 +  20u},{ _temp943, _temp943, _temp943 +  15u},{
_temp944, _temp944, _temp944 +  16u},{ _temp945, _temp945, _temp945 +  20u},{
_temp946, _temp946, _temp946 +  15u},{ _temp947, _temp947, _temp947 +  8u},{
_temp948, _temp948, _temp948 +  19u},{ _temp949, _temp949, _temp949 +  20u},{
_temp950, _temp950, _temp950 +  14u},{ _temp951, _temp951, _temp951 +  19u},{
_temp952, _temp952, _temp952 +  20u},{ _temp953, _temp953, _temp953 +  11u},{
_temp954, _temp954, _temp954 +  22u},{ _temp955, _temp955, _temp955 +  20u},{
_temp956, _temp956, _temp956 +  23u},{ _temp957, _temp957, _temp957 +  20u},{
_temp958, _temp958, _temp958 +  22u},{ _temp959, _temp959, _temp959 +  23u},{
_temp960, _temp960, _temp960 +  24u},{ _temp961, _temp961, _temp961 +  24u},{
_temp962, _temp962, _temp962 +  15u},{ _temp963, _temp963, _temp963 +  20u},{
_temp964, _temp964, _temp964 +  22u},{ _temp965, _temp965, _temp965 +  17u},{
_temp966, _temp966, _temp966 +  20u},{ _temp967, _temp967, _temp967 +  26u},{
_temp968, _temp968, _temp968 +  16u},{ _temp969, _temp969, _temp969 +  17u},{
_temp970, _temp970, _temp970 +  15u},{ _temp971, _temp971, _temp971 +  19u},{
_temp972, _temp972, _temp972 +  19u},{ _temp973, _temp973, _temp973 +  25u},{
_temp974, _temp974, _temp974 +  26u},{ _temp975, _temp975, _temp975 +  9u},{
_temp976, _temp976, _temp976 +  20u}}; static short Cyc_yyr1[ 403u]={ 0, 120,
121, 121, 121, 121, 121, 121, 121, 122, 122, 123, 123, 123, 123, 124, 124, 125,
126, 127, 128, 129, 129, 129, 129, 130, 130, 131, 131, 131, 131, 131, 131, 131,
131, 132, 132, 132, 132, 132, 132, 132, 133, 133, 134, 135, 135, 136, 136, 136,
136, 136, 137, 137, 137, 137, 137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
137, 137, 137, 138, 138, 139, 139, 139, 140, 140, 141, 141, 142, 142, 143, 143,
143, 143, 143, 144, 144, 144, 145, 145, 146, 147, 147, 148, 149, 149, 150, 150,
151, 152, 152, 152, 152, 153, 154, 154, 155, 155, 155, 156, 156, 156, 156, 157,
157, 158, 158, 158, 158, 159, 159, 159, 160, 160, 161, 161, 162, 162, 162, 162,
162, 162, 162, 162, 162, 162, 162, 162, 163, 163, 163, 163, 164, 164, 164, 164,
164, 165, 165, 166, 166, 167, 167, 168, 168, 168, 168, 169, 169, 170, 170, 171,
171, 172, 172, 173, 173, 174, 174, 175, 175, 175, 175, 176, 176, 177, 177, 178,
178, 178, 179, 180, 180, 181, 181, 182, 182, 182, 182, 183, 183, 183, 183, 184,
185, 185, 186, 186, 187, 187, 188, 188, 188, 188, 188, 189, 189, 190, 190, 190,
191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 192, 192, 192, 192,
192, 192, 192, 192, 192, 192, 193, 194, 194, 195, 195, 196, 196, 196, 196, 196,
196, 197, 197, 197, 197, 197, 198, 198, 198, 198, 198, 198, 199, 199, 199, 199,
200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 201, 201,
201, 201, 201, 201, 201, 201, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202,
202, 202, 202, 202, 203, 203, 204, 204, 205, 205, 206, 207, 207, 208, 208, 209,
209, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 211, 211, 211, 211,
211, 211, 211, 212, 213, 213, 214, 214, 215, 215, 216, 216, 217, 217, 218, 218,
218, 219, 219, 219, 219, 219, 220, 220, 220, 221, 221, 221, 222, 222, 222, 222,
223, 223, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 225, 225,
225, 226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 227,
227, 227, 227, 227, 227, 227, 227, 227, 228, 229, 229, 230, 230, 230, 230, 231,
231}; static short Cyc_yyr2[ 403u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4,
3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1,
0, 1, 6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4,
1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3,
1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 4, 3, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2,
6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1,
1, 1, 2, 3, 5, 5, 7, 1, 3, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 4, 1, 1, 3, 1, 3,
2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 4,
3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2,
2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5,
5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1,
1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3,
1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6,
7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4,
5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 794u]={ 0, 137, 35, 36,
37, 38, 40, 52, 54, 55, 56, 57, 58, 59, 60, 61, 71, 72, 73, 88, 89, 42, 0, 0, 41,
0, 0, 113, 114, 0, 401, 157, 402, 85, 0, 53, 0, 142, 143, 146, 1, 0, 9, 0, 0, 10,
0, 42, 42, 42, 62, 63, 0, 64, 0, 0, 124, 0, 147, 65, 126, 39, 0, 33, 43, 75, 286,
401, 282, 285, 284, 0, 280, 0, 0, 0, 0, 0, 180, 0, 287, 17, 19, 0, 0, 0, 66, 0,
0, 0, 0, 2, 0, 0, 0, 0, 21, 0, 93, 94, 96, 27, 29, 31, 85, 0, 85, 150, 0, 149,
85, 38, 0, 25, 0, 0, 11, 159, 0, 0, 135, 125, 42, 148, 136, 0, 0, 34, 0, 0, 294,
293, 283, 292, 24, 0, 0, 0, 0, 0, 42, 42, 198, 200, 0, 69, 70, 158, 205, 0, 127,
0, 0, 175, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 397, 387, 398, 399, 0, 0, 0, 0,
370, 0, 368, 369, 0, 305, 318, 326, 328, 330, 332, 334, 336, 339, 344, 347, 350,
354, 0, 356, 371, 386, 385, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 84, 0,
90, 91, 0, 83, 85, 0, 111, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 401,
233, 235, 0, 241, 237, 0, 239, 222, 223, 224, 0, 225, 226, 227, 0, 303, 26, 13,
96, 165, 181, 0, 0, 161, 159, 0, 0, 128, 0, 138, 0, 0, 78, 0, 76, 281, 296, 0,
295, 182, 0, 0, 294, 0, 201, 173, 0, 99, 101, 159, 0, 207, 199, 208, 68, 0, 87,
0, 86, 0, 177, 0, 179, 67, 0, 0, 364, 0, 320, 354, 0, 321, 322, 0, 0, 0, 0, 0, 0,
357, 358, 0, 0, 0, 0, 360, 361, 359, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 307, 0, 362,
0, 379, 380, 0, 0, 0, 389, 0, 0, 145, 18, 0, 20, 0, 95, 97, 184, 183, 14, 0, 80,
92, 0, 0, 103, 104, 106, 0, 110, 85, 119, 0, 0, 0, 0, 0, 0, 0, 273, 274, 275, 0,
0, 277, 0, 230, 231, 0, 0, 0, 0, 242, 238, 96, 240, 236, 234, 0, 166, 0, 0, 0,
172, 160, 167, 130, 0, 0, 0, 161, 132, 134, 133, 129, 151, 140, 139, 0, 48, 47,
0, 45, 0, 74, 0, 289, 0, 23, 290, 0, 0, 0, 0, 194, 298, 301, 0, 300, 0, 0, 0,
202, 100, 102, 0, 161, 0, 211, 0, 209, 159, 0, 0, 221, 204, 206, 176, 0, 0, 0,
185, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 388, 395, 0, 394, 327, 0, 329, 331,
333, 335, 337, 338, 342, 343, 340, 341, 345, 346, 348, 349, 351, 352, 353, 306,
377, 378, 373, 0, 375, 376, 0, 0, 0, 4, 6, 0, 325, 107, 98, 0, 0, 0, 112, 121,
120, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 272, 276, 0, 0, 0, 229, 0, 232, 0, 15,
304, 159, 0, 169, 0, 0, 0, 162, 131, 165, 153, 152, 141, 7, 0, 0, 0, 79, 77, 297,
197, 0, 299, 193, 195, 291, 0, 288, 203, 174, 216, 0, 210, 213, 0, 161, 0, 212,
0, 363, 0, 0, 186, 0, 190, 383, 384, 0, 0, 0, 0, 0, 0, 0, 355, 391, 0, 0, 374,
372, 392, 0, 0, 82, 105, 108, 81, 109, 122, 119, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 248, 278, 0, 0, 16, 161, 0, 170, 168, 0, 161, 0, 0, 0, 0, 44, 46, 196, 302,
215, 218, 0, 220, 219, 214, 0, 0, 187, 191, 0, 0, 323, 324, 0, 393, 0, 396, 319,
390, 0, 118, 117, 243, 0, 248, 258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 279, 0,
155, 171, 163, 154, 159, 0, 49, 50, 217, 365, 0, 192, 0, 0, 381, 0, 0, 0, 254, 0,
0, 260, 0, 0, 0, 268, 0, 0, 0, 0, 0, 0, 0, 0, 247, 228, 0, 161, 0, 0, 366, 0,
382, 0, 244, 0, 0, 0, 245, 259, 261, 262, 0, 270, 269, 0, 264, 0, 0, 0, 0, 248,
249, 164, 156, 0, 0, 0, 123, 0, 0, 246, 263, 271, 265, 266, 0, 0, 248, 250, 0,
188, 367, 254, 255, 267, 248, 251, 51, 254, 256, 248, 252, 257, 253, 0, 0, 0};
static short Cyc_yydefgoto[ 112u]={ 791, 40, 41, 42, 238, 43, 367, 44, 369, 45,
205, 46, 47, 63, 64, 436, 437, 48, 147, 49, 50, 267, 268, 51, 86, 52, 211, 212,
97, 98, 99, 213, 142, 379, 380, 381, 53, 54, 534, 535, 536, 55, 56, 57, 58, 122,
108, 432, 460, 59, 461, 423, 562, 415, 419, 420, 281, 259, 153, 77, 78, 477, 372,
478, 479, 448, 449, 143, 148, 149, 462, 288, 241, 242, 243, 244, 245, 246, 693,
743, 247, 248, 271, 272, 273, 451, 452, 453, 249, 250, 354, 179, 526, 180, 181,
182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 491, 492, 195,
196}; static short Cyc_yypact[ 794u]={ 1941, - -32768, - -32768, - -32768, -
-32768, - 57, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - 19, 154, 572, - -32768, 154, 105, - -32768, - -32768, - 59, - -32768,
137, - -32768, 80, 107, - -32768, 112, 185, 200, - -32768, - -32768, 1830, -
-32768, 292, 389, - -32768, 180, - 19, - 19, - 19, - -32768, - -32768, 308, -
-32768, 182, 2377, 150, 16, 96, - -32768, 154, 211, 206, 2432, - -32768, 241, -
-32768, 169, - -32768, - -32768, - -32768, 1012, - -32768, 277, 264, 298, 1012,
300, 296, 314, 262, - -32768, - -32768, 4183, - 15, 4183, - -32768, 339, 1991,
3914, 3914, - -32768, 1830, 2051, 1830, 2051, - -32768, 356, 333, - -32768, 2319,
2432, 2432, 2432, 80, 1991, 80, - -32768, 154, - -32768, 271, 365, 1252, -
-32768, 2377, 180, - -32768, 1771, 4183, 2858, - -32768, 150, - 19, - -32768, -
-32768, 2051, 357, - -32768, 154, 361, 1012, - -32768, - -32768, - -32768, -
-32768, 391, 3914, 42, 380, 238, - 19, - 19, 243, - -32768, 134, - -32768, -
-32768, - -32768, 378, 28, - -32768, 545, 387, - -32768, 4021, 383, - -32768,
3914, 704, 396, 400, 402, 404, 407, 4062, 4062, - -32768, - -32768, - -32768, -
-32768, 1598, 412, 4128, 4128, - -32768, 4128, - -32768, - -32768, 416, - -32768,
- 45, 448, 406, 411, 409, 439, 76, 451, 423, 168, - -32768, 706, 4128, 368, -
-32768, - -32768, 17, 436, - -32768, 442, - -32768, 445, - -32768, 107, 2924,
2377, - -32768, - -32768, - -32768, - -32768, 458, 462, 1991, - -32768, 635, 478,
80, 154, 481, - -32768, 464, 106, 487, 2484, 491, 506, 485, 511, 2990, 2484, -
66, 2484, 2484, - 31, 495, - -32768, - -32768, 520, 1369, 1369, 180, 1369, -
-32768, - -32768, - -32768, 536, - -32768, - -32768, - -32768, 282, - -32768, -
-32768, - -32768, 512, 535, - -32768, 118, 539, 530, 307, 543, 34, - -32768, 551,
126, 553, 3, 562, 555, 556, - -32768, - -32768, 567, 576, - -32768, 310, 794,
1012, 4183, - -32768, 578, 586, 1991, 1991, 1026, 3056, 124, - -32768, 228, -
-32768, 118, - -32768, 4183, - -32768, 2211, - -32768, 483, - -32768, - -32768,
1991, 1598, - -32768, 1991, - -32768, - -32768, 2550, - -32768, 601, 3914, 2161,
671, 3914, 3914, 1598, - -32768, - -32768, 1369, 607, 450, 3914, - -32768, -
-32768, - -32768, - -32768, 4128, 3914, 4128, 4128, 4128, 4128, 4128, 4128, 4128,
4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, 3914, - -32768, 5, - -32768, - -32768, 3122, 141, 3914, - -32768, 2781,
610, - -32768, - -32768, 1830, - -32768, 1830, - -32768, - -32768, - -32768, -
-32768, - -32768, 1991, - -32768, - -32768, 3914, 614, 615, - -32768, 609, 1991,
- -32768, 80, 558, 3914, 619, 3914, 3914, 688, 1486, 627, - -32768, - -32768, -
-32768, 312, 679, - -32768, 3188, - -32768, - -32768, 2484, 633, 2484, 1710, -
-32768, - -32768, 2319, - -32768, - -32768, - -32768, 3914, - -32768, 1991, 626,
318, - -32768, - -32768, 622, - -32768, 637, 636, 4289, 530, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, 126, 1830, - -32768, 639, 642, 638, 154,
- -32768, 3914, - -32768, 1012, - -32768, - -32768, 663, 3914, 1012, 123, -
-32768, - -32768, - -32768, 653, 650, 654, - 4, 664, - -32768, - -32768, -
-32768, 658, 530, 659, - -32768, 652, 228, 4241, 4183, 3254, - -32768, - -32768,
378, - -32768, 662, 665, 677, - -32768, - -32768, 197, 2924, 467, 669, 107, 666,
472, 670, 672, 673, 3955, - -32768, - -32768, 675, 694, 448, - 9, 406, 411, 409,
439, 76, 76, 451, 451, 451, 451, 423, 423, 168, 168, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, 676, - -32768, - -32768, 184,
219, 4183, - -32768, - -32768, 681, - -32768, - -32768, - -32768, 635, 3914, 692,
- -32768, - -32768, - -32768, 700, 154, 315, 494, 3914, 504, 508, 674, 3320,
3386, 324, - -32768, - -32768, 702, 710, 701, - -32768, 703, - -32768, 2377, -
-32768, - -32768, 711, 4183, - -32768, 712, 118, 707, - -32768, - -32768, 176, -
-32768, - -32768, - -32768, - -32768, 355, 713, 3, - -32768, - -32768, - -32768,
- -32768, 714, - -32768, - -32768, - -32768, - -32768, 833, - -32768, - -32768,
- -32768, - -32768, 715, - -32768, - -32768, 716, 530, 99, - -32768, 717, 725,
724, 708, - -32768, 2627, - -32768, - -32768, - -32768, 2377, 1991, 704, 795,
725, 727, 2781, - -32768, - -32768, 3914, 3914, - -32768, - -32768, - -32768,
2781, 164, - -32768, - -32768, - -32768, - -32768, - -32768, 70, 33, 558, 2484,
513, 732, 2484, 3914, 3452, 334, 3518, 369, 3584, 591, - -32768, 734, 745, -
-32768, 530, 64, - -32768, - -32768, 741, 530, 1991, 735, 737, 739, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, 742, - -32768, - -32768, -
-32768, 744, 3914, - -32768, - -32768, 2924, 746, - -32768, 601, 736, - -32768,
237, - -32768, - -32768, - -32768, 748, - -32768, - -32768, 818, 753, 591, -
-32768, 517, 2484, 524, 3650, 2484, 526, 3716, 3782, 373, 1012, 764, 755, -
-32768, 2484, - -32768, - -32768, 749, - -32768, 711, 784, - -32768, - -32768, -
-32768, - -32768, 218, - -32768, 774, 1991, - -32768, 2704, 1991, 2484, 605, 778,
781, - -32768, 2484, 2484, 541, - -32768, 2484, 2484, 548, 2484, 565, 3848, - 29,
1369, - -32768, - -32768, 637, 530, 776, 3914, - -32768, 779, - -32768, 569, -
-32768, 3914, 772, 785, - -32768, - -32768, - -32768, - -32768, 2484, - -32768,
- -32768, 2484, - -32768, 2484, 2484, 588, 3914, 971, - -32768, - -32768, -
-32768, 798, 323, 788, - -32768, 251, 1369, - -32768, - -32768, - -32768, -
-32768, - -32768, 2484, 321, 591, - -32768, 789, - -32768, - -32768, 1135, -
-32768, - -32768, 971, - -32768, - -32768, 605, - -32768, 591, - -32768, -
-32768, - -32768, 888, 890, - -32768}; static short Cyc_yypgoto[ 112u]={ -
-32768, 27, - -32768, 583, - -32768, - -32768, - -32768, - -32768, - -32768, -
51, - 42, - 55, - -32768, - 25, - 49, 327, - -32768, 88, - -32768, 29, - -32768,
- -32768, 463, - -32768, - 22, - -32768, - 67, - -32768, - -32768, - -32768, 705,
693, 63, - -32768, - -32768, 376, - -32768, - -32768, 68, - -32768, - -32768, 8,
- 39, - 41, - -32768, - -32768, 851, - -32768, - 98, 25, - 106, - 395, 178, 347,
352, 623, - 308, - 82, - 258, 797, - -32768, - 201, - 157, - 349, - 255, -
-32768, 468, - 144, - 78, - 102, 98, - 126, - 195, - -32768, - -32768, - 40, -
161, - -32768, - 556, - 309, - -32768, - -32768, - 21, 645, - -32768, 338, -
-32768, - -32768, 208, 220, - -32768, - 145, - 355, - 149, 600, 589, 598, 599,
597, 364, 209, 367, 370, - 43, 525, - -32768, - -32768, - -32768, - 307, -
-32768, - -32768, 10}; static short Cyc_yytable[ 4391u]={ 115, 306, 79, 371, 113,
144, 152, 120, 127, 307, 60, 258, 303, 114, 520, 116, 261, 1, 121, 257, 434, 447,
101, 102, 103, 324, 317, 399, 391, 61, 565, 400, 65, 80, 398, 81, 401, 402, 532,
533, 756, 472, 87, 83, 60, 115, 207, 208, 209, 113, 129, 60, 516, 403, 100, 133,
60, 240, 137, 115, 206, 239, 106, 251, 110, 325, 586, 60, 91, 145, 124, 404, 120,
62, 252, 757, 146, 407, 408, 109, 410, 80, 210, 109, 215, 573, 80, 435, 218, 513,
413, 576, 291, 549, 583, 612, 514, 264, 427, 362, 30, 286, 60, 60, 60, 60, 32,
290, 1, 559, 296, 363, 141, 34, 141, 282, 283, 141, 216, 198, 199, 200, 201, 253,
715, 60, 364, 292, - 116, 320, 321, 293, 322, 292, 141, 60, 276, 428, 269, 277,
80, 332, 333, 16, 17, 18, 141, 141, 584, 355, 115, 151, 265, 425, 251, 487, 473,
556, 474, 295, 465, 60, 697, 658, 280, 374, 472, - 85, 214, 486, 465, 140, 85,
140, 620, 290, 140, 416, 334, 335, 151, 1, 85, 240, 240, 31, 240, 239, 239, 82,
239, 30, 388, 140, 384, 657, 107, 32, 292, 141, 455, 775, 659, 389, 34, 140, 140,
31, 550, 88, 552, 253, 417, 60, 471, 37, 38, 39, 783, 578, 84, 284, 382, 431, 60,
517, 788, 385, 445, 446, 285, 790, 518, 525, 37, 38, 39, 289, 30, 469, 287, 141,
62, 286, 32, 290, 696, 117, 409, 297, 60, 699, 118, 296, 482, 450, 119, 121, 140,
671, 414, 240, - 181, 292, 30, 239, 30, 674, - 181, 256, 32, 31, 32, 96, - 159,
214, 340, 34, 599, 89, - 159, 418, 107, 413, 341, 342, 80, 80, 37, 38, 39, 614,
597, 430, 90, 525, 598, 509, 510, 511, 140, 525, 87, 126, 60, 125, 60, 141, 524,
178, 197, 141, 141, 141, 615, 418, 530, 413, 616, 60, 62, 141, 735, 141, 152,
466, 447, 31, 141, 141, 467, 141, 710, 279, 468, 128, 711, 115, 760, 263, 284,
543, 141, 665, 275, 458, 459, 151, 131, 285, 413, 37, 38, 39, 115, 779, - 85,
151, 113, - 85, 590, 665, 151, 531, 85, 591, 140, 553, 589, 554, 140, 140, 140,
85, 130, 412, 217, 60, 318, 60, 140, 413, 140, 132, 525, 92, 93, 140, 140, 700,
140, 567, 30, 134, 522, 135, 523, 664, 32, 104, 256, 140, 105, 443, 141, 546,
424, 31, 624, 413, 136, 413, 141, 558, 625, 664, 469, 635, 777, 617, 413, 574,
413, 413, 373, 782, 577, 685, 356, 357, 358, 678, 203, 413, 681, 141, 397, 150,
214, 648, 649, 650, 280, 60, 141, 609, 214, 667, 269, 202, 641, 219, 80, 141,
266, 668, 665, 80, 646, 270, 568, 566, 688, 140, 707, 359, 727, 673, 413, 151,
786, 140, 413, 360, 361, 274, 789, 278, 151, 642, 302, 280, 94, 95, 1, 298, 299,
151, 717, 290, 602, 721, 60, 308, 140, 141, 141, 309, 115, 310, 731, 311, 251,
140, 312, 464, 330, 331, 318, 319, 664, 323, 140, 640, 336, 337, 480, 326, 740,
484, 327, 318, 329, 746, 747, 373, 328, 749, 750, 151, 752, 365, 485, 494, 338,
339, 382, 366, 60, 490, 368, 501, 502, 503, 504, 623, 1, 115, 489, 413, 141, 113,
375, 768, 140, 140, 769, 376, 770, 771, 450, 387, 206, 532, 533, 600, 413, 30,
758, 519, 604, 413, 383, 32, 512, 386, - 207, 781, 394, 490, 294, - 207, - 207,
373, 390, 418, 141, - 207, 392, 285, 393, 80, 626, 413, 733, 537, 774, 539, 540,
405, 544, 675, 628, 413, 395, 780, 629, 413, 204, 140, 66, 679, 413, 691, 692,
716, 413, 406, 785, 414, 490, 787, 718, 413, 722, 413, 490, 741, 742, 30, 739,
411, 141, 555, 422, 32, 1, 421, - 178, 748, 413, 426, 294, - 178, - 178, 140,
751, 413, 433, - 178, 439, 285, 440, 37, 38, 39, 67, 68, 429, 69, 70, 438, 32,
753, 413, 441, 666, 764, 299, 71, 728, 324, 72, 73, 240, 442, 141, 456, 239, 301,
74, 457, 304, 304, 75, 76, 772, 413, 593, 314, 315, 140, 676, 677, 499, 500, 483,
304, 304, 373, 304, 80, 240, 505, 506, 488, 239, 527, 507, 508, 151, 240, 521,
529, 528, 239, 538, 304, 541, 30, 545, 547, 551, 557, 240, 32, 561, 240, 239,
154, 155, 239, 34, 560, 563, 140, 569, 571, 141, 378, 570, 141, 156, 37, 38, 39,
627, 575, 580, 581, 632, 634, 582, 31, 159, 160, 585, 587, 161, 588, 594, 596,
163, 603, 595, 164, 165, 601, 607, 605, 606, 630, 737, 610, 613, 151, 618, 343,
344, 345, 346, 347, 348, 349, 350, 351, 352, 621, 30, 166, 167, 168, 169, 611,
32, 622, 636, 140, 305, 638, 140, 170, 353, 637, 256, 171, 639, 643, 661, 645,
662, 651, 172, 655, 656, 173, 174, 175, 373, 608, 669, 653, 176, 177, 660, 670,
680, 694, 373, 695, 698, 672, 66, 709, 701, 702, 373, 703, 682, 684, 704, 687,
705, 690, 708, 712, 713, 714, 732, 304, 730, 304, 304, 304, 304, 304, 304, 304,
304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 729, 734, 706, 66, 736, 744,
745, 761, 766, 763, 30, 68, 767, 69, 70, 776, 32, 373, 778, 784, 792, 444, 793,
71, 481, 720, 72, 73, 724, 726, 652, 445, 446, 572, 74, 304, 619, 377, 75, 76,
370, 123, 759, 647, 644, 470, 260, 495, 579, 30, 68, 654, 69, 70, 454, 32, 493,
496, 498, 497, 0, 0, 71, 373, 0, 72, 73, 755, 0, 0, 445, 446, 0, 74, 0, 762, 0,
75, 76, 0, 0, 765, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 773, 304, 0, 0, 0,
0, 0, 304, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 691, 692, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226, 227, 228,
22, 156, 23, 157, 229, 304, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231, 232, 161,
29, 233, 162, 163, 0, 0, 164, 165, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 0, 0, 66, 0, 0, 0, 304, 234, 166, 167, 168, 169, 31, 32, 33, 22, 235,
112, 0, 0, 170, 0, 0, 35, 237, 0, 27, 28, 0, 0, 0, 172, 0, 29, 173, 174, 175, 0,
0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 68, 0, 69, 70, 0, 32, 0, 0, 0, 0, 254, 0,
71, 0, 0, 72, 73, 0, 31, 0, 33, 0, 256, 74, 0, 0, 284, 75, 76, 35, 36, 0, 304, 0,
0, 285, 0, 37, 38, 39, 304, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 741, 742, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226,
227, 228, 22, 156, 23, 157, 229, 0, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231,
232, 161, 29, 233, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166, 167, 168, 169, 31, 32, 33, 0, 235, 112, 0,
0, 170, 0, 0, 35, 237, 0, 0, 0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0,
176, 177, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 0, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226, 227, 228, 22, 156, 23,
157, 229, 0, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231, 232, 161, 29, 233, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 234, 166, 167, 168, 169, 31, 32, 33, 0, 235, 112, 236, 0, 170, 0, 0, 35, 237,
0, 0, 0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 2, 3, 4, 111,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 220,
0, 221, 222, 223, 224, 225, 226, 227, 228, 22, 156, 23, 157, 229, 0, 158, 24,
230, 0, 0, 27, 28, 159, 160, 231, 232, 161, 29, 233, 162, 163, 0, 0, 164, 165, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166, 167, 168,
169, 31, 32, 33, 0, 235, 112, 0, 0, 170, 0, 0, 35, 237, 0, 0, 0, 0, 0, 0, 172, 0,
0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
156, 23, 157, 0, 0, 158, 24, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 31, 32, 33, 0, 542, 0, 0, 0, 170, 0, 0, 35, 237, 0, 0,
0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
22, 156, 0, 157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 31, 32, 33, 0, 0, 316, 0, 0, 170, 0, 0, 35, 237, 0, 0,
0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
22, 156, 0, 157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 30, 166, 167, 168, 169, 31, 32, 33, 0, 0, 0, 0, 0, 170, 22, 0, 35,
237, 0, 0, 0, 0, 0, 0, 172, 27, 28, 173, 174, 175, 0, 0, 29, 0, 176, 177, - 8, 1,
0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 254, 0,
21, 255, 0, 0, 0, 0, 31, 0, 33, 0, 256, 0, 0, 22, 0, 23, 0, 35, 36, 0, 24, 0, 25,
26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34,
0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0,
29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 22, 0, 23, 35,
36, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33,
0, 0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4,
111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 1, 0, 0,
0, 29, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35,
36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 254, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 256, 0, 0, 0, 294, -
159, 0, 35, 36, 0, 0, - 159, 0, 285, 0, 37, 38, 39, 2, 3, 4, 111, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0,
0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 112, 22, 204, 23, 0, 0, 35, 36, 24, 0, 0,
0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0,
112, 0, 0, 0, 0, 24, 35, 36, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 155, 220, 0, 221, 222, 223, 224,
225, 226, 227, 228, 31, 156, 33, 157, 229, 0, 158, 0, 230, 0, 0, 35, 36, 159,
160, 231, 232, 161, 0, 233, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166, 167, 168, 169, 0, 32, 154, 155,
235, 112, 0, 0, 170, 475, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174,
175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32,
0, 0, 0, 305, 476, 0, 170, 0, 0, 0, 171, 154, 155, 0, 445, 446, 0, 172, 0, 0,
173, 174, 175, 0, 156, 0, 157, 176, 177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 0, 0, 0, 305, 663, 0, 170, 0, 0,
0, 171, 154, 155, 0, 445, 446, 0, 172, 0, 0, 173, 174, 175, 0, 156, 0, 157, 176,
177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 0, 164, 165,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 0, 0, 0, 305, 738, 0, 170, 0, 0, 0, 171, 154, 155, 0, 445, 446, 0,
172, 0, 0, 173, 174, 175, 0, 156, 0, 157, 176, 177, 158, 0, 0, 0, 0, 0, 0, 159,
160, 0, 0, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 0, 0, 0, 305, 0, 0,
170, 0, 0, 0, 171, 154, 155, 0, 445, 446, 0, 172, 0, 0, 173, 174, 175, 0, 156, 0,
157, 176, 177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157,
262, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163,
0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 305, 0, 0, 170, 0, 0, 0, 171, 0, 0,
156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0,
162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 396, 0, 0, 0, 170, 0, 0, 0,
171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 0, 0,
0, 171, 0, 0, 156, 0, 157, 463, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176,
177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170,
515, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160,
176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0,
0, 170, 548, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0,
159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155,
0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 592, 172, 158, 0, 173, 174,
175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32,
154, 155, 631, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0,
173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 154, 155, 633, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172,
158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164,
165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167,
168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 683, 0, 0, 171, 0, 0, 156, 0, 157, 0,
172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 686, 0, 0, 171, 0, 0, 156, 0,
157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 689, 0, 0, 0, 170, 0, 0, 0, 171, 0,
0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0,
0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 719, 0, 0,
171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 723,
0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176,
177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170,
725, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160,
176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0,
0, 170, 754, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0,
159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 154, 155, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 159,
160, 0, 0, 161, 0, 170, 0, 163, 0, 171, 164, 165, 0, 0, 0, 0, 172, 0, 0, 173,
174, 175, 0, 0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32,
154, 155, 0, 608, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 0, 0, 172, 0, 0, 173,
174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 0, 163, 0, 0, 164, 165, 0, 0, 0,
154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 159, 160, 0, 0, 161, 0, 300, 0, 163, 0, 171, 164, 165, 0, 0, 0, 0,
172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 154, 155, 0, 0, 0, 0, 313, 0, 0, 0, 171, 0, 0, 156, 0, 0, 0, 172, 0,
0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 0, 163, 0, 0, 164, 165, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 0, 32, 0, 22, 0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 27, 28, 0, 0, 0,
172, 0, 29, 173, 174, 175, 138, 0, 0, 0, 176, 177, 0, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 139, 22,
0, 0, 0, 0, 35, 36, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0,
22, 0, 0, 0, 31, 0, 33, 0, 256, 0, 0, 27, 28, 0, 0, 35, 36, 0, 29, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 564, 0, 0, 0, 0, 0,
0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 35, 36}; static short Cyc_yycheck[ 4391u]={
55, 158, 23, 204, 55, 83, 88, 56, 63, 158, 0, 117, 157, 55, 363, 55, 118, 1, 57,
117, 17, 276, 47, 48, 49, 70, 170, 93, 223, 86, 425, 97, 22, 23, 229, 25, 231,
232, 5, 6, 69, 299, 34, 102, 34, 100, 101, 102, 103, 100, 71, 41, 359, 84, 46,
76, 46, 112, 80, 114, 100, 112, 52, 114, 54, 110, 461, 57, 41, 84, 60, 102, 121,
92, 114, 104, 91, 238, 239, 54, 241, 71, 104, 58, 106, 440, 76, 84, 110, 84, 99,
446, 64, 400, 98, 104, 91, 122, 64, 82, 84, 142, 92, 93, 94, 95, 90, 111, 1, 417,
151, 94, 83, 97, 85, 140, 141, 88, 108, 92, 93, 94, 95, 115, 680, 115, 109, 99,
95, 172, 173, 103, 175, 99, 105, 125, 94, 103, 128, 97, 130, 65, 66, 17, 18, 19,
117, 118, 456, 192, 205, 88, 125, 259, 205, 316, 300, 415, 302, 151, 286, 151,
98, 64, 139, 205, 424, 97, 105, 313, 296, 83, 102, 85, 529, 111, 88, 59, 102,
103, 117, 1, 102, 238, 239, 89, 241, 238, 239, 84, 241, 84, 86, 105, 216, 590,
100, 90, 99, 170, 278, 757, 103, 97, 97, 117, 118, 89, 403, 97, 405, 203, 94,
203, 292, 108, 109, 110, 774, 96, 83, 97, 214, 264, 214, 84, 782, 217, 105, 106,
106, 787, 91, 378, 108, 109, 110, 103, 84, 288, 142, 212, 92, 284, 90, 111, 641,
97, 240, 151, 240, 646, 102, 294, 309, 276, 106, 296, 170, 608, 84, 316, 93, 99,
84, 316, 84, 103, 99, 93, 90, 89, 90, 93, 98, 212, 108, 97, 479, 94, 104, 256,
100, 99, 116, 117, 276, 277, 108, 109, 110, 107, 95, 264, 94, 440, 99, 340, 341,
342, 212, 446, 294, 97, 294, 94, 296, 278, 375, 89, 90, 282, 283, 284, 95, 290,
383, 99, 99, 309, 92, 292, 104, 294, 406, 97, 581, 89, 299, 300, 102, 302, 95,
95, 106, 94, 99, 392, 733, 119, 97, 392, 313, 598, 136, 282, 283, 284, 84, 106,
99, 108, 109, 110, 409, 104, 94, 294, 409, 97, 466, 616, 299, 385, 102, 467, 278,
409, 466, 409, 282, 283, 284, 102, 97, 93, 105, 367, 170, 369, 292, 99, 294, 85,
529, 93, 94, 299, 300, 647, 302, 432, 84, 93, 367, 99, 369, 598, 90, 91, 93, 313,
94, 93, 375, 93, 99, 89, 93, 99, 96, 99, 383, 95, 99, 616, 465, 93, 95, 521, 99,
442, 99, 99, 204, 104, 447, 93, 60, 61, 62, 626, 99, 99, 629, 406, 228, 98, 375,
84, 85, 86, 417, 433, 415, 488, 383, 604, 438, 93, 556, 86, 442, 424, 97, 604,
711, 447, 564, 98, 433, 432, 93, 375, 665, 97, 93, 612, 99, 406, 779, 383, 99,
105, 106, 84, 785, 97, 415, 557, 97, 456, 93, 94, 1, 98, 99, 424, 683, 111, 482,
686, 482, 97, 406, 466, 467, 97, 553, 97, 695, 97, 553, 415, 97, 285, 67, 68,
300, 97, 711, 95, 424, 553, 63, 64, 308, 69, 713, 311, 114, 313, 113, 718, 719,
305, 115, 722, 723, 466, 725, 95, 312, 325, 111, 112, 528, 95, 528, 319, 95, 332,
333, 334, 335, 535, 1, 602, 98, 99, 521, 602, 94, 748, 466, 467, 751, 95, 753,
754, 581, 97, 602, 5, 6, 98, 99, 84, 729, 361, 98, 99, 94, 90, 354, 94, 93, 772,
93, 359, 97, 98, 99, 363, 97, 560, 557, 104, 97, 106, 84, 581, 98, 99, 700, 387,
757, 389, 390, 104, 392, 623, 98, 99, 93, 766, 98, 99, 96, 521, 38, 98, 99, 22,
23, 98, 99, 97, 779, 84, 400, 782, 98, 99, 98, 99, 406, 22, 23, 84, 712, 95, 603,
413, 104, 90, 1, 98, 93, 98, 99, 98, 97, 98, 99, 557, 98, 99, 95, 104, 95, 106,
96, 108, 109, 110, 84, 85, 107, 87, 88, 99, 90, 98, 99, 98, 603, 98, 99, 97, 691,
70, 100, 101, 729, 99, 647, 99, 729, 154, 108, 95, 157, 158, 112, 113, 98, 99,
468, 164, 165, 603, 624, 625, 330, 331, 25, 172, 173, 479, 175, 691, 757, 336,
337, 98, 757, 93, 338, 339, 647, 766, 102, 104, 99, 766, 97, 192, 30, 84, 93, 42,
89, 97, 779, 90, 89, 782, 779, 25, 26, 782, 97, 111, 98, 647, 97, 99, 709, 104,
98, 712, 38, 108, 109, 110, 538, 84, 95, 99, 542, 543, 98, 89, 50, 51, 98, 98,
54, 107, 98, 84, 58, 97, 99, 61, 62, 98, 95, 99, 98, 97, 709, 98, 98, 712, 95,
71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 95, 84, 85, 86, 87, 88, 99, 90, 95, 94,
709, 94, 98, 712, 97, 96, 93, 93, 101, 103, 95, 84, 102, 102, 98, 108, 98, 98,
111, 112, 113, 598, 94, 25, 107, 118, 119, 107, 98, 94, 93, 608, 84, 89, 611, 38,
97, 99, 98, 616, 98, 630, 631, 98, 633, 98, 635, 98, 97, 28, 94, 99, 324, 95,
326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341,
342, 104, 85, 662, 38, 98, 95, 93, 99, 104, 98, 84, 85, 95, 87, 88, 85, 90, 665,
98, 98, 0, 95, 0, 97, 309, 685, 100, 101, 688, 689, 571, 105, 106, 438, 108, 378,
528, 212, 112, 113, 203, 58, 732, 564, 560, 290, 117, 326, 448, 84, 85, 581, 87,
88, 277, 90, 324, 327, 329, 328, - 1, - 1, 97, 711, - 1, 100, 101, 727, - 1, - 1,
105, 106, - 1, 108, - 1, 735, - 1, 112, 113, - 1, - 1, 741, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 756, 440, - 1, - 1, - 1, - 1,
- 1, 446, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, 488, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, -
1, - 1, 61, 62, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
38, - 1, - 1, - 1, 529, 84, 85, 86, 87, 88, 89, 90, 91, 37, 93, 94, - 1, - 1, 97,
- 1, - 1, 100, 101, - 1, 48, 49, - 1, - 1, - 1, 108, - 1, 55, 111, 112, 113, - 1,
- 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, 84, 85, - 1, 87, 88, - 1, 90,
- 1, - 1, - 1, - 1, 81, - 1, 97, - 1, - 1, 100, 101, - 1, 89, - 1, 91, - 1, 93,
108, - 1, - 1, 97, 112, 113, 100, 101, - 1, 604, - 1, - 1, 106, - 1, 108, 109,
110, 612, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
- 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, - 1,
61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, 93, 94, -
1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111,
112, 113, - 1, - 1, - 1, - 1, 118, 119, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33,
34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52,
53, 54, 55, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, 89, 90, 91, - 1, 93, 94, 95, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1,
- 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26,
27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, -
1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, 93, 94, - 1, - 1, 97, - 1, -
1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, -
1, - 1, - 1, 118, 119, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 37, 38, 39, 40, - 1, - 1, 43, 44, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54,
55, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88,
89, 90, 91, - 1, 93, - 1, - 1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1,
- 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1,
- 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, - 1, 94, - 1, - 1, 97, - 1,
- 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1,
- 1, - 1, - 1, 118, 119, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, -
1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1,
57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, - 1, - 1,
- 1, - 1, 97, 37, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, 48, 49, 111,
112, 113, - 1, - 1, 55, - 1, 118, 119, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 81, - 1, 24, 84, - 1, - 1, - 1, - 1, 89,
- 1, 91, - 1, 93, - 1, - 1, 37, - 1, 39, - 1, 100, 101, - 1, 44, - 1, 46, 47, 48,
49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, - 1, - 1, - 1, - 1, 89, 90, 91, - 1, - 1, - 1, 95, - 1, 97, - 1,
- 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, 109, 110, 0, 1, - 1, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1,
- 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 84, - 1, - 1, 37, - 1, 89, 90, 91, - 1, - 1, - 1, - 1, -
1, 97, 48, 49, 100, 101, - 1, - 1, - 1, 55, - 1, - 1, 108, 109, 110, 1, - 1, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24,
- 1, - 1, - 1, - 1, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, 100, 101,
- 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, - 1, - 1, - 1, - 1, 89, 90, 91, -
1, - 1, - 1, 95, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108,
109, 110, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
37, - 1, - 1, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, 1, - 1, - 1, -
1, 55, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, - 1, - 1, 37, - 1, 89, 90,
91, - 1, - 1, - 1, - 1, - 1, 97, 48, 49, 100, 101, - 1, - 1, - 1, 55, - 1, - 1,
108, 109, 110, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, - 1, - 1, 84, - 1, - 1, - 1, - 1, 89, 90,
91, - 1, 93, - 1, - 1, - 1, 97, 98, - 1, 100, 101, - 1, - 1, 104, - 1, 106, - 1,
108, 109, 110, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37,
- 1, 39, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1,
55, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1, 91, - 1, -
1, 94, 37, 96, 39, - 1, - 1, 100, 101, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1,
- 1, - 1, 55, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1,
91, 37, - 1, 94, - 1, - 1, - 1, - 1, 44, 100, 101, - 1, 48, 49, - 1, - 1, - 1, -
1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35,
36, 89, 38, 91, 40, 41, - 1, 43, - 1, 45, - 1, - 1, 100, 101, 50, 51, 52, 53, 54,
- 1, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88,
- 1, 90, 25, 26, 93, 94, - 1, - 1, 97, 32, - 1, - 1, 101, - 1, - 1, 38, - 1, 40,
- 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57,
58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, - 1, -
1, - 1, 94, 95, - 1, 97, - 1, - 1, - 1, 101, 25, 26, - 1, 105, 106, - 1, 108, -
1, - 1, 111, 112, 113, - 1, 38, - 1, 40, 118, 119, 43, - 1, - 1, - 1, - 1, - 1,
- 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 84, 85, 86, 87, 88, - 1, 90, - 1, - 1, - 1, 94, 95, - 1, 97, - 1, - 1, -
1, 101, 25, 26, - 1, 105, 106, - 1, 108, - 1, - 1, 111, 112, 113, - 1, 38, - 1,
40, 118, 119, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1,
57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, -
1, - 1, - 1, 94, 95, - 1, 97, - 1, - 1, - 1, 101, 25, 26, - 1, 105, 106, - 1,
108, - 1, - 1, 111, 112, 113, - 1, 38, - 1, 40, 118, 119, 43, - 1, - 1, - 1, - 1,
- 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, - 1, - 1, - 1, 94, - 1, - 1, 97, - 1,
- 1, - 1, 101, 25, 26, - 1, 105, 106, - 1, 108, - 1, - 1, 111, 112, 113, - 1, 38,
- 1, 40, 118, 119, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1,
- 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1,
90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40,
107, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57,
58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26,
- 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43,
- 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1,
61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93, - 1, -
1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111,
112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1,
97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, 107, 108, 43, - 1, 111, 112, 113,
- 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, -
1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1,
50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1,
101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51,
118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1,
- 1, 38, - 1, 40, 107, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119,
54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, - 1, 90, 25, 26, 93, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, -
1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, -
1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90,
25, 26, 93, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1,
108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, -
1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1,
- 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1,
111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61,
62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1,
- 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112,
113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93, - 1, - 1, - 1, 97, -
1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1,
- 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, -
1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50,
51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1,
- 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119,
54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, -
1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, -
1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90,
25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1,
108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, -
1, - 1, 61, 62, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 38, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 50, 51, - 1, -
1, 54, - 1, 97, - 1, 58, - 1, 101, 61, 62, - 1, - 1, - 1, - 1, 108, - 1, - 1,
111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, 84, 85, 86,
87, 88, - 1, 90, 25, 26, - 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38,
- 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1,
- 1, - 1, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1,
90, 50, 51, - 1, - 1, 54, - 1, 97, - 1, 58, - 1, 101, 61, 62, - 1, - 1, - 1, - 1,
108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1,
- 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1,
101, - 1, - 1, 38, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, 50, 51,
118, 119, 54, - 1, - 1, - 1, 58, - 1, - 1, 61, 62, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, -
1, 90, - 1, 37, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, 48, 49, - 1, -
1, - 1, 108, - 1, 55, 111, 112, 113, 59, - 1, - 1, - 1, 118, 119, - 1, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 89, - 1, 91, - 1, - 1, 94, 37, - 1, - 1, - 1, - 1, 100, 101, - 1, - 1, -
1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, -
1, - 1, - 1, 37, - 1, - 1, - 1, 89, - 1, 91, - 1, 93, - 1, - 1, 48, 49, - 1, - 1,
100, 101, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 100,
101}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short*
yyss=( short*)({ unsigned int _temp7360= 10000u; short* _temp7361=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp7360));{ unsigned int
_temp7362= _temp7360; unsigned int i; for( i= 0; i <  _temp7362; i ++){
_temp7361[ i]=( short) 0;}}; _temp7361;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp7357= 10000u; void** _temp7358=( void**) _cycalloc(
_check_times( sizeof( void*), _temp7357));{ unsigned int _temp7359= _temp7357;
unsigned int i; for( i= 0; i <  _temp7359; i ++){ _temp7358[ i]= Cyc_yylval;}};
_temp7358;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp7354= 10000u; struct Cyc_Yyltype* _temp7355=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp7354));{
unsigned int _temp7356= _temp7354; unsigned int i; for( i= 0; i <  _temp7356; i
++){ _temp7355[ i]= Cyc_yynewloc();}}; _temp7355;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 794u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
347: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 348u, Cyc_yychar)]:
232;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4390)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4391u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4391u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  793){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 794u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 403u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL977: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result=({ struct Cyc_List_List* yyzzz;{ void*
_temp979= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp985; _LL981: if(*(( void**) _temp979) ==  Cyc_DeclList_tok){
_LL986: _temp985=(( struct Cyc_DeclList_tok_struct*) _temp979)->f1; goto _LL982;}
else{ goto _LL983;} _LL983: goto _LL984; _LL982: yyzzz= _temp985; goto _LL980;
_LL984:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL980:;} yyzzz;});
break; case 2: _LL978: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp988=(
struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp988[ 0]=({ struct Cyc_DeclList_tok_struct _temp989; _temp989.tag= Cyc_DeclList_tok;
_temp989.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)(({ struct Cyc_List_List* yyzzz;{ void* _temp990= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp996; _LL992: if(*(( void**) _temp990) ==  Cyc_DeclList_tok){ _LL997:
_temp996=(( struct Cyc_DeclList_tok_struct*) _temp990)->f1; goto _LL993;} else{
goto _LL994;} _LL994: goto _LL995; _LL993: yyzzz= _temp996; goto _LL991; _LL995:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL991:;} yyzzz;}),({ struct Cyc_List_List*
yyzzz;{ void* _temp998= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_List_List* _temp1004; _LL1000: if(*(( void**)
_temp998) ==  Cyc_DeclList_tok){ _LL1005: _temp1004=(( struct Cyc_DeclList_tok_struct*)
_temp998)->f1; goto _LL1001;} else{ goto _LL1002;} _LL1002: goto _LL1003;
_LL1001: yyzzz= _temp1004; goto _LL999; _LL1003:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok);
_LL999:;} yyzzz;})); _temp989;}); _temp988;}); break; case 3: _LL987: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1007=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1007[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1008; _temp1008.tag= Cyc_DeclList_tok; _temp1008.f1=({ struct Cyc_List_List*
_temp1009=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1009->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1010=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1010->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp1011=( struct Cyc_Absyn_Using_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1011[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp1012; _temp1012.tag= Cyc_Absyn_Using_d; _temp1012.f1=({
struct _tuple1* yyzzz;{ void* _temp1013= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct _tuple1* _temp1019; _LL1015: if(*(( void**)
_temp1013) ==  Cyc_QualId_tok){ _LL1020: _temp1019=(( struct Cyc_QualId_tok_struct*)
_temp1013)->f1; goto _LL1016;} else{ goto _LL1017;} _LL1017: goto _LL1018;
_LL1016: yyzzz= _temp1019; goto _LL1014; _LL1018:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL1014:;} yyzzz;}); _temp1012.f2=({ struct Cyc_List_List* yyzzz;{ void*
_temp1021= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp1027; _LL1023: if(*(( void**) _temp1021) ==  Cyc_DeclList_tok){
_LL1028: _temp1027=(( struct Cyc_DeclList_tok_struct*) _temp1021)->f1; goto
_LL1024;} else{ goto _LL1025;} _LL1025: goto _LL1026; _LL1024: yyzzz= _temp1027;
goto _LL1022; _LL1026:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1022:;}
yyzzz;}); _temp1012;}); _temp1011;})); _temp1010->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1010;}); _temp1009->tl= 0; _temp1009;}); _temp1008;}); _temp1007;}); Cyc_Lex_leave_using();
break; case 4: _LL1006: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1030=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1030[ 0]=({ struct Cyc_DeclList_tok_struct _temp1031; _temp1031.tag= Cyc_DeclList_tok;
_temp1031.f1=({ struct Cyc_List_List* _temp1032=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1032->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1041=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1041->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1042=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1042[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1043; _temp1043.tag= Cyc_Absyn_Using_d;
_temp1043.f1=({ struct _tuple1* yyzzz;{ void* _temp1044= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct _tuple1*
_temp1050; _LL1046: if(*(( void**) _temp1044) ==  Cyc_QualId_tok){ _LL1051:
_temp1050=(( struct Cyc_QualId_tok_struct*) _temp1044)->f1; goto _LL1047;} else{
goto _LL1048;} _LL1048: goto _LL1049; _LL1047: yyzzz= _temp1050; goto _LL1045;
_LL1049:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL1045:;} yyzzz;});
_temp1043.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp1052= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp1058; _LL1054: if(*(( void**) _temp1052) ==  Cyc_DeclList_tok){ _LL1059:
_temp1058=(( struct Cyc_DeclList_tok_struct*) _temp1052)->f1; goto _LL1055;}
else{ goto _LL1056;} _LL1056: goto _LL1057; _LL1055: yyzzz= _temp1058; goto
_LL1053; _LL1057:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1053:;}
yyzzz;}); _temp1043;}); _temp1042;})); _temp1041->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1041;}); _temp1032->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp1033=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1039; _LL1035: if(*(( void**) _temp1033) ==  Cyc_DeclList_tok){ _LL1040:
_temp1039=(( struct Cyc_DeclList_tok_struct*) _temp1033)->f1; goto _LL1036;}
else{ goto _LL1037;} _LL1037: goto _LL1038; _LL1036: yyzzz= _temp1039; goto
_LL1034; _LL1038:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1034:;}
yyzzz;}); _temp1032;}); _temp1031;}); _temp1030;}); break; case 5: _LL1029:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1061=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1061[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1062; _temp1062.tag= Cyc_DeclList_tok; _temp1062.f1=({ struct Cyc_List_List*
_temp1063=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1063->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1064=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1064->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp1065=( struct Cyc_Absyn_Namespace_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1065[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1066; _temp1066.tag= Cyc_Absyn_Namespace_d;
_temp1066.f1=({ struct _tagged_arr* _temp1067=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp1067[ 0]=({ struct _tagged_arr yyzzz;{ void*
_temp1068= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct _tagged_arr _temp1074; _LL1070: if(*(( void**) _temp1068) ==  Cyc_String_tok){
_LL1075: _temp1074=(( struct Cyc_String_tok_struct*) _temp1068)->f1; goto
_LL1071;} else{ goto _LL1072;} _LL1072: goto _LL1073; _LL1071: yyzzz= _temp1074;
goto _LL1069; _LL1073:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL1069:;}
yyzzz;}); _temp1067;}); _temp1066.f2=({ struct Cyc_List_List* yyzzz;{ void*
_temp1076= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp1082; _LL1078: if(*(( void**) _temp1076) ==  Cyc_DeclList_tok){
_LL1083: _temp1082=(( struct Cyc_DeclList_tok_struct*) _temp1076)->f1; goto
_LL1079;} else{ goto _LL1080;} _LL1080: goto _LL1081; _LL1079: yyzzz= _temp1082;
goto _LL1077; _LL1081:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1077:;}
yyzzz;}); _temp1066;}); _temp1065;})); _temp1064->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1064;}); _temp1063->tl= 0; _temp1063;}); _temp1062;}); _temp1061;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1060: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1085=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1085[ 0]=({ struct Cyc_DeclList_tok_struct _temp1086; _temp1086.tag= Cyc_DeclList_tok;
_temp1086.f1=({ struct Cyc_List_List* _temp1087=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1087->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1096=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1096->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1097=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1097[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1098; _temp1098.tag=
Cyc_Absyn_Namespace_d; _temp1098.f1=({ struct _tagged_arr* _temp1099=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1099[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp1100= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tagged_arr _temp1106; _LL1102: if(*(( void**)
_temp1100) ==  Cyc_String_tok){ _LL1107: _temp1106=(( struct Cyc_String_tok_struct*)
_temp1100)->f1; goto _LL1103;} else{ goto _LL1104;} _LL1104: goto _LL1105;
_LL1103: yyzzz= _temp1106; goto _LL1101; _LL1105:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL1101:;} yyzzz;}); _temp1099;}); _temp1098.f2=({ struct Cyc_List_List* yyzzz;{
void* _temp1108= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp1114; _LL1110: if(*(( void**) _temp1108) ==  Cyc_DeclList_tok){
_LL1115: _temp1114=(( struct Cyc_DeclList_tok_struct*) _temp1108)->f1; goto
_LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113; _LL1111: yyzzz= _temp1114;
goto _LL1109; _LL1113:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1109:;}
yyzzz;}); _temp1098;}); _temp1097;})); _temp1096->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1096;}); _temp1087->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp1088=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1094; _LL1090: if(*(( void**) _temp1088) ==  Cyc_DeclList_tok){ _LL1095:
_temp1094=(( struct Cyc_DeclList_tok_struct*) _temp1088)->f1; goto _LL1091;}
else{ goto _LL1092;} _LL1092: goto _LL1093; _LL1091: yyzzz= _temp1094; goto
_LL1089; _LL1093:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1089:;}
yyzzz;}); _temp1087;}); _temp1086;}); _temp1085;}); break; case 7: _LL1084: if(
Cyc_Std_strcmp(({ struct _tagged_arr yyzzz;{ void* _temp1117= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct _tagged_arr
_temp1123; _LL1119: if(*(( void**) _temp1117) ==  Cyc_String_tok){ _LL1124:
_temp1123=(( struct Cyc_String_tok_struct*) _temp1117)->f1; goto _LL1120;} else{
goto _LL1121;} _LL1121: goto _LL1122; _LL1120: yyzzz= _temp1123; goto _LL1118;
_LL1122:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL1118:;} yyzzz;}),
_tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1125=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1125[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1126; _temp1126.tag= Cyc_DeclList_tok; _temp1126.f1=({ struct Cyc_List_List*
_temp1127=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1127->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1136=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1136->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1137=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1137[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1138; _temp1138.tag= Cyc_Absyn_ExternC_d;
_temp1138.f1=({ struct Cyc_List_List* yyzzz;{ void* _temp1139= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp1145; _LL1141: if(*(( void**) _temp1139) ==  Cyc_DeclList_tok){ _LL1146:
_temp1145=(( struct Cyc_DeclList_tok_struct*) _temp1139)->f1; goto _LL1142;}
else{ goto _LL1143;} _LL1143: goto _LL1144; _LL1142: yyzzz= _temp1145; goto
_LL1140; _LL1144:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1140:;}
yyzzz;}); _temp1138;}); _temp1137;})); _temp1136->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1136;}); _temp1127->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp1128=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1134; _LL1130: if(*(( void**) _temp1128) ==  Cyc_DeclList_tok){ _LL1135:
_temp1134=(( struct Cyc_DeclList_tok_struct*) _temp1128)->f1; goto _LL1131;}
else{ goto _LL1132;} _LL1132: goto _LL1133; _LL1131: yyzzz= _temp1134; goto
_LL1129; _LL1133:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1129:;}
yyzzz;}); _temp1127;}); _temp1126;}); _temp1125;}); break; case 8: _LL1116:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1148=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1148[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1149; _temp1149.tag= Cyc_DeclList_tok; _temp1149.f1= 0; _temp1149;});
_temp1148;}); break; case 9: _LL1147: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1151=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1151[ 0]=({ struct Cyc_DeclList_tok_struct _temp1152; _temp1152.tag= Cyc_DeclList_tok;
_temp1152.f1=({ struct Cyc_List_List* _temp1153=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1153->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp1154=( struct Cyc_Absyn_Fn_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1154[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1155; _temp1155.tag= Cyc_Absyn_Fn_d; _temp1155.f1=({ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp1156= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Fndecl* _temp1162; _LL1158: if(*(( void**)
_temp1156) ==  Cyc_FnDecl_tok){ _LL1163: _temp1162=(( struct Cyc_FnDecl_tok_struct*)
_temp1156)->f1; goto _LL1159;} else{ goto _LL1160;} _LL1160: goto _LL1161;
_LL1159: yyzzz= _temp1162; goto _LL1157; _LL1161:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL1157:;} yyzzz;}); _temp1155;}); _temp1154;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1153->tl=
0; _temp1153;}); _temp1152;}); _temp1151;}); break; case 10: _LL1150: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1164: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1166=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1166[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1167; _temp1167.tag= Cyc_FnDecl_tok; _temp1167.f1= Cyc_Parse_make_function(
0,({ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp1168= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Parse_Declarator*
_temp1174; _LL1170: if(*(( void**) _temp1168) ==  Cyc_Declarator_tok){ _LL1175:
_temp1174=(( struct Cyc_Declarator_tok_struct*) _temp1168)->f1; goto _LL1171;}
else{ goto _LL1172;} _LL1172: goto _LL1173; _LL1171: yyzzz= _temp1174; goto
_LL1169; _LL1173:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1169:;}
yyzzz;}), 0,({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp1176= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp1182; _LL1178: if(*(( void**) _temp1176) ==  Cyc_Stmt_tok){ _LL1183:
_temp1182=(( struct Cyc_Stmt_tok_struct*) _temp1176)->f1; goto _LL1179;} else{
goto _LL1180;} _LL1180: goto _LL1181; _LL1179: yyzzz= _temp1182; goto _LL1177;
_LL1181:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1177:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1167;}); _temp1166;}); break; case 12: _LL1165: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1185=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1185[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1186; _temp1186.tag= Cyc_FnDecl_tok;
_temp1186.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1187=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1187->v=( void*)({
struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1188= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Parse_Declaration_spec*
_temp1194; _LL1190: if(*(( void**) _temp1188) ==  Cyc_DeclSpec_tok){ _LL1195:
_temp1194=(( struct Cyc_DeclSpec_tok_struct*) _temp1188)->f1; goto _LL1191;}
else{ goto _LL1192;} _LL1192: goto _LL1193; _LL1191: yyzzz= _temp1194; goto
_LL1189; _LL1193:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1189:;}
yyzzz;}); _temp1187;}),({ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp1196=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Parse_Declarator*
_temp1202; _LL1198: if(*(( void**) _temp1196) ==  Cyc_Declarator_tok){ _LL1203:
_temp1202=(( struct Cyc_Declarator_tok_struct*) _temp1196)->f1; goto _LL1199;}
else{ goto _LL1200;} _LL1200: goto _LL1201; _LL1199: yyzzz= _temp1202; goto
_LL1197; _LL1201:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1197:;}
yyzzz;}), 0,({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp1204= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp1210; _LL1206: if(*(( void**) _temp1204) ==  Cyc_Stmt_tok){ _LL1211:
_temp1210=(( struct Cyc_Stmt_tok_struct*) _temp1204)->f1; goto _LL1207;} else{
goto _LL1208;} _LL1208: goto _LL1209; _LL1207: yyzzz= _temp1210; goto _LL1205;
_LL1209:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1205:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1186;}); _temp1185;}); break; case 13: _LL1184: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1213=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1213[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1214; _temp1214.tag= Cyc_FnDecl_tok;
_temp1214.f1= Cyc_Parse_make_function( 0,({ struct Cyc_Parse_Declarator* yyzzz;{
void* _temp1215= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_Parse_Declarator* _temp1221; _LL1217: if(*(( void**) _temp1215) == 
Cyc_Declarator_tok){ _LL1222: _temp1221=(( struct Cyc_Declarator_tok_struct*)
_temp1215)->f1; goto _LL1218;} else{ goto _LL1219;} _LL1219: goto _LL1220;
_LL1218: yyzzz= _temp1221; goto _LL1216; _LL1220:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL1216:;} yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp1223= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1229; _LL1225: if(*(( void**) _temp1223) ==  Cyc_DeclList_tok){ _LL1230:
_temp1229=(( struct Cyc_DeclList_tok_struct*) _temp1223)->f1; goto _LL1226;}
else{ goto _LL1227;} _LL1227: goto _LL1228; _LL1226: yyzzz= _temp1229; goto
_LL1224; _LL1228:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1224:;}
yyzzz;}),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp1231= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp1237; _LL1233: if(*(( void**) _temp1231) ==  Cyc_Stmt_tok){ _LL1238:
_temp1237=(( struct Cyc_Stmt_tok_struct*) _temp1231)->f1; goto _LL1234;} else{
goto _LL1235;} _LL1235: goto _LL1236; _LL1234: yyzzz= _temp1237; goto _LL1232;
_LL1236:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1232:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1214;}); _temp1213;}); break; case 14: _LL1212: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1240=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1240[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1241; _temp1241.tag= Cyc_FnDecl_tok;
_temp1241.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1242=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1242->v=( void*)({
struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1243= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Parse_Declaration_spec*
_temp1249; _LL1245: if(*(( void**) _temp1243) ==  Cyc_DeclSpec_tok){ _LL1250:
_temp1249=(( struct Cyc_DeclSpec_tok_struct*) _temp1243)->f1; goto _LL1246;}
else{ goto _LL1247;} _LL1247: goto _LL1248; _LL1246: yyzzz= _temp1249; goto
_LL1244; _LL1248:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1244:;}
yyzzz;}); _temp1242;}),({ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp1251=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Parse_Declarator*
_temp1257; _LL1253: if(*(( void**) _temp1251) ==  Cyc_Declarator_tok){ _LL1258:
_temp1257=(( struct Cyc_Declarator_tok_struct*) _temp1251)->f1; goto _LL1254;}
else{ goto _LL1255;} _LL1255: goto _LL1256; _LL1254: yyzzz= _temp1257; goto
_LL1252; _LL1256:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1252:;}
yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp1259= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1265; _LL1261: if(*(( void**) _temp1259) ==  Cyc_DeclList_tok){ _LL1266:
_temp1265=(( struct Cyc_DeclList_tok_struct*) _temp1259)->f1; goto _LL1262;}
else{ goto _LL1263;} _LL1263: goto _LL1264; _LL1262: yyzzz= _temp1265; goto
_LL1260; _LL1264:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1260:;}
yyzzz;}),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp1267= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp1273; _LL1269: if(*(( void**) _temp1267) ==  Cyc_Stmt_tok){ _LL1274:
_temp1273=(( struct Cyc_Stmt_tok_struct*) _temp1267)->f1; goto _LL1270;} else{
goto _LL1271;} _LL1271: goto _LL1272; _LL1270: yyzzz= _temp1273; goto _LL1268;
_LL1272:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1268:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1241;}); _temp1240;}); break; case 15: _LL1239: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1276=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1276[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1277; _temp1277.tag= Cyc_FnDecl_tok;
_temp1277.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1278=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1278->v=( void*)({
struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1279= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Parse_Declaration_spec*
_temp1285; _LL1281: if(*(( void**) _temp1279) ==  Cyc_DeclSpec_tok){ _LL1286:
_temp1285=(( struct Cyc_DeclSpec_tok_struct*) _temp1279)->f1; goto _LL1282;}
else{ goto _LL1283;} _LL1283: goto _LL1284; _LL1282: yyzzz= _temp1285; goto
_LL1280; _LL1284:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1280:;}
yyzzz;}); _temp1278;}),({ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp1287=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Parse_Declarator*
_temp1293; _LL1289: if(*(( void**) _temp1287) ==  Cyc_Declarator_tok){ _LL1294:
_temp1293=(( struct Cyc_Declarator_tok_struct*) _temp1287)->f1; goto _LL1290;}
else{ goto _LL1291;} _LL1291: goto _LL1292; _LL1290: yyzzz= _temp1293; goto
_LL1288; _LL1292:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1288:;}
yyzzz;}), 0,({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp1295= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp1301; _LL1297: if(*(( void**) _temp1295) ==  Cyc_Stmt_tok){ _LL1302:
_temp1301=(( struct Cyc_Stmt_tok_struct*) _temp1295)->f1; goto _LL1298;} else{
goto _LL1299;} _LL1299: goto _LL1300; _LL1298: yyzzz= _temp1301; goto _LL1296;
_LL1300:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1296:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1277;}); _temp1276;}); break; case 16: _LL1275: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1304=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1304[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1305; _temp1305.tag= Cyc_FnDecl_tok;
_temp1305.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1306=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1306->v=( void*)({
struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1307= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Parse_Declaration_spec*
_temp1313; _LL1309: if(*(( void**) _temp1307) ==  Cyc_DeclSpec_tok){ _LL1314:
_temp1313=(( struct Cyc_DeclSpec_tok_struct*) _temp1307)->f1; goto _LL1310;}
else{ goto _LL1311;} _LL1311: goto _LL1312; _LL1310: yyzzz= _temp1313; goto
_LL1308; _LL1312:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1308:;}
yyzzz;}); _temp1306;}),({ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp1315=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Parse_Declarator*
_temp1321; _LL1317: if(*(( void**) _temp1315) ==  Cyc_Declarator_tok){ _LL1322:
_temp1321=(( struct Cyc_Declarator_tok_struct*) _temp1315)->f1; goto _LL1318;}
else{ goto _LL1319;} _LL1319: goto _LL1320; _LL1318: yyzzz= _temp1321; goto
_LL1316; _LL1320:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1316:;}
yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp1323= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1329; _LL1325: if(*(( void**) _temp1323) ==  Cyc_DeclList_tok){ _LL1330:
_temp1329=(( struct Cyc_DeclList_tok_struct*) _temp1323)->f1; goto _LL1326;}
else{ goto _LL1327;} _LL1327: goto _LL1328; _LL1326: yyzzz= _temp1329; goto
_LL1324; _LL1328:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1324:;}
yyzzz;}),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp1331= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp1337; _LL1333: if(*(( void**) _temp1331) ==  Cyc_Stmt_tok){ _LL1338:
_temp1337=(( struct Cyc_Stmt_tok_struct*) _temp1331)->f1; goto _LL1334;} else{
goto _LL1335;} _LL1335: goto _LL1336; _LL1334: yyzzz= _temp1337; goto _LL1332;
_LL1336:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1332:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1305;}); _temp1304;}); break; case 17: _LL1303: Cyc_Lex_enter_using(({
struct _tuple1* yyzzz;{ void* _temp1340= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tuple1* _temp1346; _LL1342: if(*(( void**)
_temp1340) ==  Cyc_QualId_tok){ _LL1347: _temp1346=(( struct Cyc_QualId_tok_struct*)
_temp1340)->f1; goto _LL1343;} else{ goto _LL1344;} _LL1344: goto _LL1345;
_LL1343: yyzzz= _temp1346; goto _LL1341; _LL1345:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL1341:;} yyzzz;})); yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 18: _LL1339: Cyc_Lex_leave_using(); break; case 19:
_LL1348: Cyc_Lex_enter_namespace(({ struct _tagged_arr* _temp1350=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1350[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp1351= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tagged_arr _temp1357; _LL1353: if(*(( void**) _temp1351)
==  Cyc_String_tok){ _LL1358: _temp1357=(( struct Cyc_String_tok_struct*)
_temp1351)->f1; goto _LL1354;} else{ goto _LL1355;} _LL1355: goto _LL1356;
_LL1354: yyzzz= _temp1357; goto _LL1352; _LL1356:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL1352:;} yyzzz;}); _temp1350;})); yyval= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; break; case 20: _LL1349: Cyc_Lex_leave_namespace();
break; case 21: _LL1359: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1361=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1361[ 0]=({ struct Cyc_DeclList_tok_struct _temp1362; _temp1362.tag= Cyc_DeclList_tok;
_temp1362.f1= Cyc_Parse_make_declarations(({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1363= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Parse_Declaration_spec* _temp1369; _LL1365: if(*((
void**) _temp1363) ==  Cyc_DeclSpec_tok){ _LL1370: _temp1369=(( struct Cyc_DeclSpec_tok_struct*)
_temp1363)->f1; goto _LL1366;} else{ goto _LL1367;} _LL1367: goto _LL1368;
_LL1366: yyzzz= _temp1369; goto _LL1364; _LL1368:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1364:;} yyzzz;}), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1362;}); _temp1361;}); break; case 22: _LL1360: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1372=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1372[ 0]=({ struct Cyc_DeclList_tok_struct _temp1373; _temp1373.tag= Cyc_DeclList_tok;
_temp1373.f1= Cyc_Parse_make_declarations(({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1374= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Parse_Declaration_spec* _temp1380; _LL1376: if(*((
void**) _temp1374) ==  Cyc_DeclSpec_tok){ _LL1381: _temp1380=(( struct Cyc_DeclSpec_tok_struct*)
_temp1374)->f1; goto _LL1377;} else{ goto _LL1378;} _LL1378: goto _LL1379;
_LL1377: yyzzz= _temp1380; goto _LL1375; _LL1379:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1375:;} yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp1382= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1388; _LL1384: if(*(( void**) _temp1382) ==  Cyc_InitDeclList_tok){ _LL1389:
_temp1388=(( struct Cyc_InitDeclList_tok_struct*) _temp1382)->f1; goto _LL1385;}
else{ goto _LL1386;} _LL1386: goto _LL1387; _LL1385: yyzzz= _temp1388; goto
_LL1383; _LL1387:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL1383:;}
yyzzz;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1373;}); _temp1372;}); break; case 23:
_LL1371: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1391=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1391[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1392; _temp1392.tag= Cyc_DeclList_tok; _temp1392.f1=({ struct Cyc_List_List*
_temp1393=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1393->hd=( void*) Cyc_Absyn_let_decl(({ struct Cyc_Absyn_Pat* yyzzz;{ void*
_temp1394= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Absyn_Pat* _temp1400; _LL1396: if(*(( void**) _temp1394) ==  Cyc_Pattern_tok){
_LL1401: _temp1400=(( struct Cyc_Pattern_tok_struct*) _temp1394)->f1; goto
_LL1397;} else{ goto _LL1398;} _LL1398: goto _LL1399; _LL1397: yyzzz= _temp1400;
goto _LL1395; _LL1399:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL1395:;}
yyzzz;}), 0,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp1402= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp1408; _LL1404: if(*(( void**) _temp1402) ==  Cyc_Exp_tok){ _LL1409:
_temp1408=(( struct Cyc_Exp_tok_struct*) _temp1402)->f1; goto _LL1405;} else{
goto _LL1406;} _LL1406: goto _LL1407; _LL1405: yyzzz= _temp1408; goto _LL1403;
_LL1407:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL1403:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1393->tl= 0; _temp1393;}); _temp1392;}); _temp1391;}); break; case 24:
_LL1390: { struct Cyc_List_List* _temp1411= 0;{ struct Cyc_List_List* _temp1412=({
struct Cyc_List_List* yyzzz;{ void* _temp1418= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1424; _LL1420: if(*(( void**) _temp1418) ==  Cyc_IdList_tok){ _LL1425:
_temp1424=(( struct Cyc_IdList_tok_struct*) _temp1418)->f1; goto _LL1421;} else{
goto _LL1422;} _LL1422: goto _LL1423; _LL1421: yyzzz= _temp1424; goto _LL1419;
_LL1423:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1419:;} yyzzz;});
for( 0; _temp1412 !=  0; _temp1412=(( struct Cyc_List_List*) _check_null(
_temp1412))->tl){ struct _tagged_arr* _temp1413=( struct _tagged_arr*)(( struct
Cyc_List_List*) _check_null( _temp1412))->hd; struct _tuple1* _temp1414=({
struct _tuple1* _temp1417=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp1417->f1= Cyc_Absyn_rel_ns_null; _temp1417->f2= _temp1413; _temp1417;});
struct Cyc_Absyn_Vardecl* _temp1415= Cyc_Absyn_new_vardecl( _temp1414, Cyc_Absyn_wildtyp(
0), 0); _temp1411=({ struct Cyc_List_List* _temp1416=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1416->hd=( void*) _temp1415;
_temp1416->tl= _temp1411; _temp1416;});}} _temp1411=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1411); yyval=( void*)({ struct
Cyc_DeclList_tok_struct* _temp1426=( struct Cyc_DeclList_tok_struct*) _cycalloc(
sizeof( struct Cyc_DeclList_tok_struct)); _temp1426[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1427; _temp1427.tag= Cyc_DeclList_tok; _temp1427.f1=({ struct Cyc_List_List*
_temp1428=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1428->hd=( void*) Cyc_Absyn_letv_decl( _temp1411, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1428->tl= 0; _temp1428;}); _temp1427;}); _temp1426;}); break;} case 25:
_LL1410: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 26: _LL1429: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1431=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1431[ 0]=({ struct Cyc_DeclList_tok_struct _temp1432; _temp1432.tag= Cyc_DeclList_tok;
_temp1432.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)(({ struct Cyc_List_List* yyzzz;{ void* _temp1433= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1439; _LL1435: if(*(( void**) _temp1433) ==  Cyc_DeclList_tok){ _LL1440:
_temp1439=(( struct Cyc_DeclList_tok_struct*) _temp1433)->f1; goto _LL1436;}
else{ goto _LL1437;} _LL1437: goto _LL1438; _LL1436: yyzzz= _temp1439; goto
_LL1434; _LL1438:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1434:;}
yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp1441= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1447; _LL1443: if(*(( void**) _temp1441) ==  Cyc_DeclList_tok){ _LL1448:
_temp1447=(( struct Cyc_DeclList_tok_struct*) _temp1441)->f1; goto _LL1444;}
else{ goto _LL1445;} _LL1445: goto _LL1446; _LL1444: yyzzz= _temp1447; goto
_LL1442; _LL1446:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1442:;}
yyzzz;})); _temp1432;}); _temp1431;}); break; case 27: _LL1430: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1450=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1450[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1451; _temp1451.tag= Cyc_DeclSpec_tok; _temp1451.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1452=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1452->sc=({ struct Cyc_Core_Opt* _temp1461=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1461->v=( void*)({ enum 
Cyc_Parse_Storage_class yyzzz;{ void* _temp1462= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; enum  Cyc_Parse_Storage_class
_temp1468; _LL1464: if(*(( void**) _temp1462) ==  Cyc_StorageClass_tok){ _LL1469:
_temp1468=(( struct Cyc_StorageClass_tok_struct*) _temp1462)->f1; goto _LL1465;}
else{ goto _LL1466;} _LL1466: goto _LL1467; _LL1465: yyzzz= _temp1468; goto
_LL1463; _LL1467:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL1463:;}
yyzzz;}); _temp1461;}); _temp1452->tq= Cyc_Absyn_empty_tqual(); _temp1452->type_specs=
0; _temp1452->is_inline= 0; _temp1452->attributes=({ struct Cyc_List_List* yyzzz;{
void* _temp1453= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp1459; _LL1455: if(*(( void**) _temp1453) ==  Cyc_AttributeList_tok){
_LL1460: _temp1459=(( struct Cyc_AttributeList_tok_struct*) _temp1453)->f1; goto
_LL1456;} else{ goto _LL1457;} _LL1457: goto _LL1458; _LL1456: yyzzz= _temp1459;
goto _LL1454; _LL1458:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1454:;} yyzzz;}); _temp1452;}); _temp1451;}); _temp1450;}); break; case 28:
_LL1449: if(({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1471= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declaration_spec*
_temp1477; _LL1473: if(*(( void**) _temp1471) ==  Cyc_DeclSpec_tok){ _LL1478:
_temp1477=(( struct Cyc_DeclSpec_tok_struct*) _temp1471)->f1; goto _LL1474;}
else{ goto _LL1475;} _LL1475: goto _LL1476; _LL1474: yyzzz= _temp1477; goto
_LL1472; _LL1476:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1472:;}
yyzzz;})->sc !=  0){ Cyc_Parse_warn( _tag_arr("Only one storage class is allowed in a declaration",
sizeof( unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1479=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1479[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1480; _temp1480.tag= Cyc_DeclSpec_tok; _temp1480.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1481=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1481->sc=({ struct Cyc_Core_Opt* _temp1522=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1522->v=( void*)({ enum 
Cyc_Parse_Storage_class yyzzz;{ void* _temp1523= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; enum  Cyc_Parse_Storage_class
_temp1529; _LL1525: if(*(( void**) _temp1523) ==  Cyc_StorageClass_tok){ _LL1530:
_temp1529=(( struct Cyc_StorageClass_tok_struct*) _temp1523)->f1; goto _LL1526;}
else{ goto _LL1527;} _LL1527: goto _LL1528; _LL1526: yyzzz= _temp1529; goto
_LL1524; _LL1528:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL1524:;}
yyzzz;}); _temp1522;}); _temp1481->tq=({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1514= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declaration_spec* _temp1520; _LL1516: if(*((
void**) _temp1514) ==  Cyc_DeclSpec_tok){ _LL1521: _temp1520=(( struct Cyc_DeclSpec_tok_struct*)
_temp1514)->f1; goto _LL1517;} else{ goto _LL1518;} _LL1518: goto _LL1519;
_LL1517: yyzzz= _temp1520; goto _LL1515; _LL1519:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1515:;} yyzzz;})->tq; _temp1481->type_specs=({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1506= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declaration_spec* _temp1512; _LL1508: if(*((
void**) _temp1506) ==  Cyc_DeclSpec_tok){ _LL1513: _temp1512=(( struct Cyc_DeclSpec_tok_struct*)
_temp1506)->f1; goto _LL1509;} else{ goto _LL1510;} _LL1510: goto _LL1511;
_LL1509: yyzzz= _temp1512; goto _LL1507; _LL1511:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1507:;} yyzzz;})->type_specs; _temp1481->is_inline=({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1498= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declaration_spec* _temp1504; _LL1500: if(*((
void**) _temp1498) ==  Cyc_DeclSpec_tok){ _LL1505: _temp1504=(( struct Cyc_DeclSpec_tok_struct*)
_temp1498)->f1; goto _LL1501;} else{ goto _LL1502;} _LL1502: goto _LL1503;
_LL1501: yyzzz= _temp1504; goto _LL1499; _LL1503:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1499:;} yyzzz;})->is_inline; _temp1481->attributes= Cyc_List_imp_append(({
struct Cyc_List_List* yyzzz;{ void* _temp1482= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1488; _LL1484: if(*(( void**) _temp1482) ==  Cyc_AttributeList_tok){
_LL1489: _temp1488=(( struct Cyc_AttributeList_tok_struct*) _temp1482)->f1; goto
_LL1485;} else{ goto _LL1486;} _LL1486: goto _LL1487; _LL1485: yyzzz= _temp1488;
goto _LL1483; _LL1487:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1483:;} yyzzz;}),({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void*
_temp1490= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Declaration_spec* _temp1496; _LL1492: if(*(( void**) _temp1490) == 
Cyc_DeclSpec_tok){ _LL1497: _temp1496=(( struct Cyc_DeclSpec_tok_struct*)
_temp1490)->f1; goto _LL1493;} else{ goto _LL1494;} _LL1494: goto _LL1495;
_LL1493: yyzzz= _temp1496; goto _LL1491; _LL1495:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1491:;} yyzzz;})->attributes); _temp1481;}); _temp1480;}); _temp1479;});
break; case 29: _LL1470: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1532=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1532[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1533; _temp1533.tag= Cyc_DeclSpec_tok;
_temp1533.f1=({ struct Cyc_Parse_Declaration_spec* _temp1534=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1534->sc= 0;
_temp1534->tq= Cyc_Absyn_empty_tqual(); _temp1534->type_specs=({ struct Cyc_List_List*
_temp1543=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1543->hd=( void*)({ void* yyzzz;{ void* _temp1544= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp1550;
_LL1546: if(*(( void**) _temp1544) ==  Cyc_TypeSpecifier_tok){ _LL1551:
_temp1550=( void*)(( struct Cyc_TypeSpecifier_tok_struct*) _temp1544)->f1; goto
_LL1547;} else{ goto _LL1548;} _LL1548: goto _LL1549; _LL1547: yyzzz= _temp1550;
goto _LL1545; _LL1549:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1545:;} yyzzz;}); _temp1543->tl= 0; _temp1543;}); _temp1534->is_inline= 0;
_temp1534->attributes=({ struct Cyc_List_List* yyzzz;{ void* _temp1535= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1541; _LL1537: if(*(( void**) _temp1535) ==  Cyc_AttributeList_tok){
_LL1542: _temp1541=(( struct Cyc_AttributeList_tok_struct*) _temp1535)->f1; goto
_LL1538;} else{ goto _LL1539;} _LL1539: goto _LL1540; _LL1538: yyzzz= _temp1541;
goto _LL1536; _LL1540:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1536:;} yyzzz;}); _temp1534;}); _temp1533;}); _temp1532;}); break; case 30:
_LL1531: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1553=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1553[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1554; _temp1554.tag= Cyc_DeclSpec_tok; _temp1554.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1555=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1555->sc=({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1605=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declaration_spec*
_temp1611; _LL1607: if(*(( void**) _temp1605) ==  Cyc_DeclSpec_tok){ _LL1612:
_temp1611=(( struct Cyc_DeclSpec_tok_struct*) _temp1605)->f1; goto _LL1608;}
else{ goto _LL1609;} _LL1609: goto _LL1610; _LL1608: yyzzz= _temp1611; goto
_LL1606; _LL1610:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1606:;}
yyzzz;})->sc; _temp1555->tq=({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void*
_temp1597= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Declaration_spec* _temp1603; _LL1599: if(*(( void**) _temp1597) == 
Cyc_DeclSpec_tok){ _LL1604: _temp1603=(( struct Cyc_DeclSpec_tok_struct*)
_temp1597)->f1; goto _LL1600;} else{ goto _LL1601;} _LL1601: goto _LL1602;
_LL1600: yyzzz= _temp1603; goto _LL1598; _LL1602:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1598:;} yyzzz;})->tq; _temp1555->type_specs=({ struct Cyc_List_List*
_temp1580=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1580->hd=( void*)({ void* yyzzz;{ void* _temp1589= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void* _temp1595;
_LL1591: if(*(( void**) _temp1589) ==  Cyc_TypeSpecifier_tok){ _LL1596:
_temp1595=( void*)(( struct Cyc_TypeSpecifier_tok_struct*) _temp1589)->f1; goto
_LL1592;} else{ goto _LL1593;} _LL1593: goto _LL1594; _LL1592: yyzzz= _temp1595;
goto _LL1590; _LL1594:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1590:;} yyzzz;}); _temp1580->tl=({ struct Cyc_Parse_Declaration_spec* yyzzz;{
void* _temp1581= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_Parse_Declaration_spec* _temp1587; _LL1583: if(*(( void**) _temp1581)
==  Cyc_DeclSpec_tok){ _LL1588: _temp1587=(( struct Cyc_DeclSpec_tok_struct*)
_temp1581)->f1; goto _LL1584;} else{ goto _LL1585;} _LL1585: goto _LL1586;
_LL1584: yyzzz= _temp1587; goto _LL1582; _LL1586:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1582:;} yyzzz;})->type_specs; _temp1580;}); _temp1555->is_inline=({ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1572= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declaration_spec*
_temp1578; _LL1574: if(*(( void**) _temp1572) ==  Cyc_DeclSpec_tok){ _LL1579:
_temp1578=(( struct Cyc_DeclSpec_tok_struct*) _temp1572)->f1; goto _LL1575;}
else{ goto _LL1576;} _LL1576: goto _LL1577; _LL1575: yyzzz= _temp1578; goto
_LL1573; _LL1577:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1573:;}
yyzzz;})->is_inline; _temp1555->attributes= Cyc_List_imp_append(({ struct Cyc_List_List*
yyzzz;{ void* _temp1556= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp1562; _LL1558: if(*(( void**)
_temp1556) ==  Cyc_AttributeList_tok){ _LL1563: _temp1562=(( struct Cyc_AttributeList_tok_struct*)
_temp1556)->f1; goto _LL1559;} else{ goto _LL1560;} _LL1560: goto _LL1561;
_LL1559: yyzzz= _temp1562; goto _LL1557; _LL1561:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1557:;} yyzzz;}),({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void*
_temp1564= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Declaration_spec* _temp1570; _LL1566: if(*(( void**) _temp1564) == 
Cyc_DeclSpec_tok){ _LL1571: _temp1570=(( struct Cyc_DeclSpec_tok_struct*)
_temp1564)->f1; goto _LL1567;} else{ goto _LL1568;} _LL1568: goto _LL1569;
_LL1567: yyzzz= _temp1570; goto _LL1565; _LL1569:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1565:;} yyzzz;})->attributes); _temp1555;}); _temp1554;}); _temp1553;});
break; case 31: _LL1552: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1614=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1614[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1615; _temp1615.tag= Cyc_DeclSpec_tok;
_temp1615.f1=({ struct Cyc_Parse_Declaration_spec* _temp1616=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1616->sc= 0;
_temp1616->tq=({ struct Cyc_Absyn_Tqual yyzzz;{ void* _temp1625= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Tqual
_temp1631; _LL1627: if(*(( void**) _temp1625) ==  Cyc_TypeQual_tok){ _LL1632:
_temp1631=(( struct Cyc_TypeQual_tok_struct*) _temp1625)->f1; goto _LL1628;}
else{ goto _LL1629;} _LL1629: goto _LL1630; _LL1628: yyzzz= _temp1631; goto
_LL1626; _LL1630:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL1626:;}
yyzzz;}); _temp1616->type_specs= 0; _temp1616->is_inline= 0; _temp1616->attributes=({
struct Cyc_List_List* yyzzz;{ void* _temp1617= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1623; _LL1619: if(*(( void**) _temp1617) ==  Cyc_AttributeList_tok){
_LL1624: _temp1623=(( struct Cyc_AttributeList_tok_struct*) _temp1617)->f1; goto
_LL1620;} else{ goto _LL1621;} _LL1621: goto _LL1622; _LL1620: yyzzz= _temp1623;
goto _LL1618; _LL1622:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1618:;} yyzzz;}); _temp1616;}); _temp1615;}); _temp1614;}); break; case 32:
_LL1613: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1634=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1634[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1635; _temp1635.tag= Cyc_DeclSpec_tok; _temp1635.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1636=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1636->sc=({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1685=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declaration_spec*
_temp1691; _LL1687: if(*(( void**) _temp1685) ==  Cyc_DeclSpec_tok){ _LL1692:
_temp1691=(( struct Cyc_DeclSpec_tok_struct*) _temp1685)->f1; goto _LL1688;}
else{ goto _LL1689;} _LL1689: goto _LL1690; _LL1688: yyzzz= _temp1691; goto
_LL1686; _LL1690:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1686:;}
yyzzz;})->sc; _temp1636->tq= Cyc_Absyn_combine_tqual(({ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp1669= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Absyn_Tqual _temp1675; _LL1671: if(*(( void**)
_temp1669) ==  Cyc_TypeQual_tok){ _LL1676: _temp1675=(( struct Cyc_TypeQual_tok_struct*)
_temp1669)->f1; goto _LL1672;} else{ goto _LL1673;} _LL1673: goto _LL1674;
_LL1672: yyzzz= _temp1675; goto _LL1670; _LL1674:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL1670:;} yyzzz;}),({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void*
_temp1677= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Declaration_spec* _temp1683; _LL1679: if(*(( void**) _temp1677) == 
Cyc_DeclSpec_tok){ _LL1684: _temp1683=(( struct Cyc_DeclSpec_tok_struct*)
_temp1677)->f1; goto _LL1680;} else{ goto _LL1681;} _LL1681: goto _LL1682;
_LL1680: yyzzz= _temp1683; goto _LL1678; _LL1682:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1678:;} yyzzz;})->tq); _temp1636->type_specs=({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1661= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declaration_spec* _temp1667; _LL1663: if(*((
void**) _temp1661) ==  Cyc_DeclSpec_tok){ _LL1668: _temp1667=(( struct Cyc_DeclSpec_tok_struct*)
_temp1661)->f1; goto _LL1664;} else{ goto _LL1665;} _LL1665: goto _LL1666;
_LL1664: yyzzz= _temp1667; goto _LL1662; _LL1666:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1662:;} yyzzz;})->type_specs; _temp1636->is_inline=({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1653= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declaration_spec* _temp1659; _LL1655: if(*((
void**) _temp1653) ==  Cyc_DeclSpec_tok){ _LL1660: _temp1659=(( struct Cyc_DeclSpec_tok_struct*)
_temp1653)->f1; goto _LL1656;} else{ goto _LL1657;} _LL1657: goto _LL1658;
_LL1656: yyzzz= _temp1659; goto _LL1654; _LL1658:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1654:;} yyzzz;})->is_inline; _temp1636->attributes= Cyc_List_imp_append(({
struct Cyc_List_List* yyzzz;{ void* _temp1637= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1643; _LL1639: if(*(( void**) _temp1637) ==  Cyc_AttributeList_tok){
_LL1644: _temp1643=(( struct Cyc_AttributeList_tok_struct*) _temp1637)->f1; goto
_LL1640;} else{ goto _LL1641;} _LL1641: goto _LL1642; _LL1640: yyzzz= _temp1643;
goto _LL1638; _LL1642:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1638:;} yyzzz;}),({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void*
_temp1645= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Declaration_spec* _temp1651; _LL1647: if(*(( void**) _temp1645) == 
Cyc_DeclSpec_tok){ _LL1652: _temp1651=(( struct Cyc_DeclSpec_tok_struct*)
_temp1645)->f1; goto _LL1648;} else{ goto _LL1649;} _LL1649: goto _LL1650;
_LL1648: yyzzz= _temp1651; goto _LL1646; _LL1650:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1646:;} yyzzz;})->attributes); _temp1636;}); _temp1635;}); _temp1634;});
break; case 33: _LL1633: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1694=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1694[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1695; _temp1695.tag= Cyc_DeclSpec_tok;
_temp1695.f1=({ struct Cyc_Parse_Declaration_spec* _temp1696=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1696->sc= 0;
_temp1696->tq= Cyc_Absyn_empty_tqual(); _temp1696->type_specs= 0; _temp1696->is_inline=
1; _temp1696->attributes=({ struct Cyc_List_List* yyzzz;{ void* _temp1697= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1703; _LL1699: if(*(( void**) _temp1697) ==  Cyc_AttributeList_tok){
_LL1704: _temp1703=(( struct Cyc_AttributeList_tok_struct*) _temp1697)->f1; goto
_LL1700;} else{ goto _LL1701;} _LL1701: goto _LL1702; _LL1700: yyzzz= _temp1703;
goto _LL1698; _LL1702:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1698:;} yyzzz;}); _temp1696;}); _temp1695;}); _temp1694;}); break; case 34:
_LL1693: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1706=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1706[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1707; _temp1707.tag= Cyc_DeclSpec_tok; _temp1707.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1708=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1708->sc=({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp1741=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declaration_spec*
_temp1747; _LL1743: if(*(( void**) _temp1741) ==  Cyc_DeclSpec_tok){ _LL1748:
_temp1747=(( struct Cyc_DeclSpec_tok_struct*) _temp1741)->f1; goto _LL1744;}
else{ goto _LL1745;} _LL1745: goto _LL1746; _LL1744: yyzzz= _temp1747; goto
_LL1742; _LL1746:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL1742:;}
yyzzz;})->sc; _temp1708->tq=({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void*
_temp1733= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Declaration_spec* _temp1739; _LL1735: if(*(( void**) _temp1733) == 
Cyc_DeclSpec_tok){ _LL1740: _temp1739=(( struct Cyc_DeclSpec_tok_struct*)
_temp1733)->f1; goto _LL1736;} else{ goto _LL1737;} _LL1737: goto _LL1738;
_LL1736: yyzzz= _temp1739; goto _LL1734; _LL1738:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1734:;} yyzzz;})->tq; _temp1708->type_specs=({ struct Cyc_Parse_Declaration_spec*
yyzzz;{ void* _temp1725= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declaration_spec* _temp1731; _LL1727: if(*((
void**) _temp1725) ==  Cyc_DeclSpec_tok){ _LL1732: _temp1731=(( struct Cyc_DeclSpec_tok_struct*)
_temp1725)->f1; goto _LL1728;} else{ goto _LL1729;} _LL1729: goto _LL1730;
_LL1728: yyzzz= _temp1731; goto _LL1726; _LL1730:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1726:;} yyzzz;})->type_specs; _temp1708->is_inline= 1; _temp1708->attributes=
Cyc_List_imp_append(({ struct Cyc_List_List* yyzzz;{ void* _temp1709= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp1715; _LL1711: if(*(( void**) _temp1709) ==  Cyc_AttributeList_tok){
_LL1716: _temp1715=(( struct Cyc_AttributeList_tok_struct*) _temp1709)->f1; goto
_LL1712;} else{ goto _LL1713;} _LL1713: goto _LL1714; _LL1712: yyzzz= _temp1715;
goto _LL1710; _LL1714:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1710:;} yyzzz;}),({ struct Cyc_Parse_Declaration_spec* yyzzz;{ void*
_temp1717= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Declaration_spec* _temp1723; _LL1719: if(*(( void**) _temp1717) == 
Cyc_DeclSpec_tok){ _LL1724: _temp1723=(( struct Cyc_DeclSpec_tok_struct*)
_temp1717)->f1; goto _LL1720;} else{ goto _LL1721;} _LL1721: goto _LL1722;
_LL1720: yyzzz= _temp1723; goto _LL1718; _LL1722:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1718:;} yyzzz;})->attributes); _temp1708;}); _temp1707;}); _temp1706;});
break; case 35: _LL1705: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1750=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1750[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1751; _temp1751.tag= Cyc_StorageClass_tok; _temp1751.f1= Cyc_Parse_Auto_sc;
_temp1751;}); _temp1750;}); break; case 36: _LL1749: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1753=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1753[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1754; _temp1754.tag= Cyc_StorageClass_tok; _temp1754.f1= Cyc_Parse_Register_sc;
_temp1754;}); _temp1753;}); break; case 37: _LL1752: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1756=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1756[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1757; _temp1757.tag= Cyc_StorageClass_tok; _temp1757.f1= Cyc_Parse_Static_sc;
_temp1757;}); _temp1756;}); break; case 38: _LL1755: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1759=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1759[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1760; _temp1760.tag= Cyc_StorageClass_tok; _temp1760.f1= Cyc_Parse_Extern_sc;
_temp1760;}); _temp1759;}); break; case 39: _LL1758: if( Cyc_Std_strcmp(({
struct _tagged_arr yyzzz;{ void* _temp1762= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp1768; _LL1764: if(*(( void**)
_temp1762) ==  Cyc_String_tok){ _LL1769: _temp1768=(( struct Cyc_String_tok_struct*)
_temp1762)->f1; goto _LL1765;} else{ goto _LL1766;} _LL1766: goto _LL1767;
_LL1765: yyzzz= _temp1768; goto _LL1763; _LL1767:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL1763:;} yyzzz;}), _tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err(
_tag_arr("only extern or extern \"C\" is allowed", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1770=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1770[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1771; _temp1771.tag= Cyc_StorageClass_tok; _temp1771.f1= Cyc_Parse_ExternC_sc;
_temp1771;}); _temp1770;}); break; case 40: _LL1761: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1773=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1773[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1774; _temp1774.tag= Cyc_StorageClass_tok; _temp1774.f1= Cyc_Parse_Typedef_sc;
_temp1774;}); _temp1773;}); break; case 41: _LL1772: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1776=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1776[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1777; _temp1777.tag= Cyc_StorageClass_tok; _temp1777.f1= Cyc_Parse_Abstract_sc;
_temp1777;}); _temp1776;}); break; case 42: _LL1775: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1779=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1779[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1780; _temp1780.tag=
Cyc_AttributeList_tok; _temp1780.f1= 0; _temp1780;}); _temp1779;}); break; case
43: _LL1778: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1781: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1783=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1783[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1784; _temp1784.tag=
Cyc_AttributeList_tok; _temp1784.f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp1785= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp1791; _LL1787: if(*(( void**) _temp1785) ==  Cyc_AttributeList_tok){
_LL1792: _temp1791=(( struct Cyc_AttributeList_tok_struct*) _temp1785)->f1; goto
_LL1788;} else{ goto _LL1789;} _LL1789: goto _LL1790; _LL1788: yyzzz= _temp1791;
goto _LL1786; _LL1790:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1786:;} yyzzz;}); _temp1784;}); _temp1783;}); break; case 45: _LL1782: yyval=(
void*)({ struct Cyc_AttributeList_tok_struct* _temp1794=( struct Cyc_AttributeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp1794[ 0]=({
struct Cyc_AttributeList_tok_struct _temp1795; _temp1795.tag= Cyc_AttributeList_tok;
_temp1795.f1=({ struct Cyc_List_List* _temp1796=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1796->hd=( void*)({ void* yyzzz;{
void* _temp1797= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp1803; _LL1799: if(*(( void**) _temp1797) ==  Cyc_Attribute_tok){
_LL1804: _temp1803=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1797)->f1;
goto _LL1800;} else{ goto _LL1801;} _LL1801: goto _LL1802; _LL1800: yyzzz=
_temp1803; goto _LL1798; _LL1802:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1798:;} yyzzz;}); _temp1796->tl= 0; _temp1796;}); _temp1795;}); _temp1794;});
break; case 46: _LL1793: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1806=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1806[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1807; _temp1807.tag=
Cyc_AttributeList_tok; _temp1807.f1=({ struct Cyc_List_List* _temp1808=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1808->hd=( void*)({
void* yyzzz;{ void* _temp1817= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; void* _temp1823; _LL1819: if(*(( void**) _temp1817) ==  Cyc_Attribute_tok){
_LL1824: _temp1823=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1817)->f1;
goto _LL1820;} else{ goto _LL1821;} _LL1821: goto _LL1822; _LL1820: yyzzz=
_temp1823; goto _LL1818; _LL1822:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1818:;} yyzzz;}); _temp1808->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp1809= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp1815; _LL1811: if(*(( void**) _temp1809) ==  Cyc_AttributeList_tok){
_LL1816: _temp1815=(( struct Cyc_AttributeList_tok_struct*) _temp1809)->f1; goto
_LL1812;} else{ goto _LL1813;} _LL1813: goto _LL1814; _LL1812: yyzzz= _temp1815;
goto _LL1810; _LL1814:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1810:;} yyzzz;}); _temp1808;}); _temp1807;}); _temp1806;}); break; case 47:
_LL1805: { struct _tagged_arr _temp1826=({ struct _tagged_arr yyzzz;{ void*
_temp1831= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
_tagged_arr _temp1837; _LL1833: if(*(( void**) _temp1831) ==  Cyc_String_tok){
_LL1838: _temp1837=(( struct Cyc_String_tok_struct*) _temp1831)->f1; goto
_LL1834;} else{ goto _LL1835;} _LL1835: goto _LL1836; _LL1834: yyzzz= _temp1837;
goto _LL1832; _LL1836:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL1832:;}
yyzzz;}); if(((( _get_arr_size( _temp1826, sizeof( unsigned char)) >  4?*((
const unsigned char*) _check_unknown_subscript( _temp1826, sizeof( unsigned char),
0)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1826,
sizeof( unsigned char), 1)) == '_': 0)?*(( const unsigned char*)
_check_unknown_subscript( _temp1826, sizeof( unsigned char),( int)(
_get_arr_size( _temp1826, sizeof( unsigned char)) -  2))) == '_': 0)?*(( const
unsigned char*) _check_unknown_subscript( _temp1826, sizeof( unsigned char),(
int)( _get_arr_size( _temp1826, sizeof( unsigned char)) -  3))) == '_': 0){
_temp1826=( struct _tagged_arr) Cyc_Std_substring( _temp1826, 2, _get_arr_size(
_temp1826, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0; i <  16u; ++ i){
if( Cyc_Std_strcmp( _temp1826,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1827=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1827[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1828; _temp1828.tag= Cyc_Attribute_tok; _temp1828.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1828;}); _temp1827;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1829=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1829[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1830; _temp1830.tag= Cyc_Attribute_tok; _temp1830.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1830;}); _temp1829;});} break;}} case
48: _LL1825: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1840=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1840[ 0]=({ struct Cyc_Attribute_tok_struct _temp1841; _temp1841.tag= Cyc_Attribute_tok;
_temp1841.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1841;}); _temp1840;});
break; case 49: _LL1839: { struct _tagged_arr _temp1843=({ struct _tagged_arr
yyzzz;{ void* _temp1863= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct _tagged_arr _temp1869; _LL1865: if(*(( void**)
_temp1863) ==  Cyc_String_tok){ _LL1870: _temp1869=(( struct Cyc_String_tok_struct*)
_temp1863)->f1; goto _LL1866;} else{ goto _LL1867;} _LL1867: goto _LL1868;
_LL1866: yyzzz= _temp1869; goto _LL1864; _LL1868:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL1864:;} yyzzz;}); struct _tuple15 _temp1854; int _temp1855; struct _tuple15*
_temp1852=({ struct _tuple15* yyzzz;{ void* _temp1844= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple15*
_temp1850; _LL1846: if(*(( void**) _temp1844) ==  Cyc_Int_tok){ _LL1851:
_temp1850=(( struct Cyc_Int_tok_struct*) _temp1844)->f1; goto _LL1847;} else{
goto _LL1848;} _LL1848: goto _LL1849; _LL1847: yyzzz= _temp1850; goto _LL1845;
_LL1849:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL1845:;} yyzzz;});
_temp1854=* _temp1852; _LL1856: _temp1855= _temp1854.f2; goto _LL1853; _LL1853: {
void* a; if( Cyc_Std_zstrcmp( _temp1843, _tag_arr("regparm", sizeof(
unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1843, _tag_arr("__regparm__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1855 <=  0? 1: _temp1855 >  3){
Cyc_Parse_err( _tag_arr("regparm requires value between 1 and 3", sizeof(
unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp1857=( struct Cyc_Absyn_Regparm_att_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp1857[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp1858; _temp1858.tag= Cyc_Absyn_Regparm_att;
_temp1858.f1= _temp1855; _temp1858;}); _temp1857;});} else{ if( Cyc_Std_zstrcmp(
_temp1843, _tag_arr("aligned", sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp(
_temp1843, _tag_arr("__aligned__", sizeof( unsigned char), 12u)) ==  0){ if(
_temp1855 <  0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
unsigned int j=( unsigned int) _temp1855; for( 0;( j &  1) ==  0; j= j >>  1){;}
j= j >>  1; if( j !=  0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1859=( struct Cyc_Absyn_Aligned_att_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1859[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1860; _temp1860.tag= Cyc_Absyn_Aligned_att;
_temp1860.f1= _temp1855; _temp1860;}); _temp1859;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1861=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1861[ 0]=({ struct Cyc_Attribute_tok_struct _temp1862; _temp1862.tag= Cyc_Attribute_tok;
_temp1862.f1=( void*) a; _temp1862;}); _temp1861;}); break;}} case 50: _LL1842: {
struct _tagged_arr _temp1872=({ struct _tagged_arr yyzzz;{ void* _temp1886= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct _tagged_arr
_temp1892; _LL1888: if(*(( void**) _temp1886) ==  Cyc_String_tok){ _LL1893:
_temp1892=(( struct Cyc_String_tok_struct*) _temp1886)->f1; goto _LL1889;} else{
goto _LL1890;} _LL1890: goto _LL1891; _LL1889: yyzzz= _temp1892; goto _LL1887;
_LL1891:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL1887:;} yyzzz;});
struct _tagged_arr _temp1873=({ struct _tagged_arr yyzzz;{ void* _temp1878= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tagged_arr
_temp1884; _LL1880: if(*(( void**) _temp1878) ==  Cyc_String_tok){ _LL1885:
_temp1884=(( struct Cyc_String_tok_struct*) _temp1878)->f1; goto _LL1881;} else{
goto _LL1882;} _LL1882: goto _LL1883; _LL1881: yyzzz= _temp1884; goto _LL1879;
_LL1883:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL1879:;} yyzzz;});
void* a; if( Cyc_Std_zstrcmp( _temp1872, _tag_arr("section", sizeof(
unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1872, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1874=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1874[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1875; _temp1875.tag=
Cyc_Absyn_Section_att; _temp1875.f1= _temp1873; _temp1875;}); _temp1874;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1876=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1876[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1877; _temp1877.tag= Cyc_Attribute_tok; _temp1877.f1=(
void*) a; _temp1877;}); _temp1876;}); break;} case 51: _LL1871: { struct
_tagged_arr _temp1895=({ struct _tagged_arr yyzzz;{ void* _temp1935= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]; struct _tagged_arr
_temp1941; _LL1937: if(*(( void**) _temp1935) ==  Cyc_String_tok){ _LL1942:
_temp1941=(( struct Cyc_String_tok_struct*) _temp1935)->f1; goto _LL1938;} else{
goto _LL1939;} _LL1939: goto _LL1940; _LL1938: yyzzz= _temp1941; goto _LL1936;
_LL1940:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL1936:;} yyzzz;});
struct _tagged_arr _temp1896=({ struct _tagged_arr yyzzz;{ void* _temp1927= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct _tagged_arr
_temp1933; _LL1929: if(*(( void**) _temp1927) ==  Cyc_String_tok){ _LL1934:
_temp1933=(( struct Cyc_String_tok_struct*) _temp1927)->f1; goto _LL1930;} else{
goto _LL1931;} _LL1931: goto _LL1932; _LL1930: yyzzz= _temp1933; goto _LL1928;
_LL1932:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL1928:;} yyzzz;}); int
_temp1907; struct _tuple15 _temp1905=*({ struct _tuple15* yyzzz;{ void*
_temp1897= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct _tuple15* _temp1903; _LL1899: if(*(( void**) _temp1897) ==  Cyc_Int_tok){
_LL1904: _temp1903=(( struct Cyc_Int_tok_struct*) _temp1897)->f1; goto _LL1900;}
else{ goto _LL1901;} _LL1901: goto _LL1902; _LL1900: yyzzz= _temp1903; goto
_LL1898; _LL1902:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL1898:;} yyzzz;});
_LL1908: _temp1907= _temp1905.f2; goto _LL1906; _LL1906: { int _temp1919; struct
_tuple15 _temp1917=*({ struct _tuple15* yyzzz;{ void* _temp1909= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple15*
_temp1915; _LL1911: if(*(( void**) _temp1909) ==  Cyc_Int_tok){ _LL1916:
_temp1915=(( struct Cyc_Int_tok_struct*) _temp1909)->f1; goto _LL1912;} else{
goto _LL1913;} _LL1913: goto _LL1914; _LL1912: yyzzz= _temp1915; goto _LL1910;
_LL1914:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL1910:;} yyzzz;});
_LL1920: _temp1919= _temp1917.f2; goto _LL1918; _LL1918: { void* a=( void*) Cyc_Absyn_Cdecl_att;
if( Cyc_Std_zstrcmp( _temp1895, _tag_arr("format", sizeof( unsigned char), 7u))
==  0? 1: Cyc_Std_zstrcmp( _temp1895, _tag_arr("__format__", sizeof(
unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1896, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1921=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1921[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1922; _temp1922.tag=
Cyc_Absyn_Format_att; _temp1922.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1922.f2= _temp1907; _temp1922.f3= _temp1919; _temp1922;}); _temp1921;});}
else{ if( Cyc_Std_zstrcmp( _temp1896, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1923=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1923[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1924; _temp1924.tag=
Cyc_Absyn_Format_att; _temp1924.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1924.f2= _temp1907; _temp1924.f3= _temp1919; _temp1924;}); _temp1923;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1925=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1925[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1926; _temp1926.tag= Cyc_Attribute_tok; _temp1926.f1=(
void*) a; _temp1926;}); _temp1925;}); break;}}} case 52: _LL1894: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1944=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1944[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1945; _temp1945.tag= Cyc_TypeSpecifier_tok;
_temp1945.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1945;});
_temp1944;}); break; case 53: _LL1943: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1947=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1947[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1948; _temp1948.tag=
Cyc_TypeSpecifier_tok; _temp1948.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1948;}); _temp1947;}); break; case 54: _LL1946:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1950=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1950[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1951; _temp1951.tag= Cyc_TypeSpecifier_tok;
_temp1951.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1951;});
_temp1950;}); break; case 55: _LL1949: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1953=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1953[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1954; _temp1954.tag=
Cyc_TypeSpecifier_tok; _temp1954.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1955=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1955[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1956; _temp1956.tag=
Cyc_Parse_Short_spec; _temp1956.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1956;});
_temp1955;})); _temp1954;}); _temp1953;}); break; case 56: _LL1952: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1958=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1958[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1959; _temp1959.tag= Cyc_TypeSpecifier_tok;
_temp1959.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1959;});
_temp1958;}); break; case 57: _LL1957: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1961=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1961[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1962; _temp1962.tag=
Cyc_TypeSpecifier_tok; _temp1962.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1963=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1963[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1964; _temp1964.tag= Cyc_Parse_Long_spec;
_temp1964.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1964;}); _temp1963;})); _temp1962;}); _temp1961;});
break; case 58: _LL1960: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1966=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1966[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1967; _temp1967.tag=
Cyc_TypeSpecifier_tok; _temp1967.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1967;}); _temp1966;}); break; case 59: _LL1965:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1969=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1969[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1970; _temp1970.tag= Cyc_TypeSpecifier_tok;
_temp1970.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1970;});
_temp1969;}); break; case 60: _LL1968: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1972=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1972[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1973; _temp1973.tag=
Cyc_TypeSpecifier_tok; _temp1973.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1974=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1974[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1975; _temp1975.tag=
Cyc_Parse_Signed_spec; _temp1975.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1975;});
_temp1974;})); _temp1973;}); _temp1972;}); break; case 61: _LL1971: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1977=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1977[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1978; _temp1978.tag= Cyc_TypeSpecifier_tok;
_temp1978.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1979=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1979[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1980; _temp1980.tag= Cyc_Parse_Unsigned_spec; _temp1980.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1980;});
_temp1979;})); _temp1978;}); _temp1977;}); break; case 62: _LL1976: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1981:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1982: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1983: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1985=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1985[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1986; _temp1986.tag=
Cyc_TypeSpecifier_tok; _temp1986.f1=( void*) Cyc_Parse_type_spec(({ void* yyzzz;{
void* _temp1987= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp1993; _LL1989: if(*(( void**) _temp1987) ==  Cyc_Type_tok){ _LL1994:
_temp1993=( void*)(( struct Cyc_Type_tok_struct*) _temp1987)->f1; goto _LL1990;}
else{ goto _LL1991;} _LL1991: goto _LL1992; _LL1990: yyzzz= _temp1993; goto
_LL1988; _LL1992:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1988:;} yyzzz;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1986;}); _temp1985;}); break; case 66: _LL1984:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1996=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1996[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1997; _temp1997.tag= Cyc_TypeSpecifier_tok;
_temp1997.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp1998=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp1998[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp1999; _temp1999.tag=
Cyc_Absyn_TypedefType; _temp1999.f1=({ struct _tuple1* yyzzz;{ void* _temp2000=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct
_tuple1* _temp2006; _LL2002: if(*(( void**) _temp2000) ==  Cyc_QualId_tok){
_LL2007: _temp2006=(( struct Cyc_QualId_tok_struct*) _temp2000)->f1; goto
_LL2003;} else{ goto _LL2004;} _LL2004: goto _LL2005; _LL2003: yyzzz= _temp2006;
goto _LL2001; _LL2005:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2001:;}
yyzzz;}); _temp1999.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp2008= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2014; _LL2010: if(*(( void**) _temp2008) ==  Cyc_TypeList_tok){ _LL2015:
_temp2014=(( struct Cyc_TypeList_tok_struct*) _temp2008)->f1; goto _LL2011;}
else{ goto _LL2012;} _LL2012: goto _LL2013; _LL2011: yyzzz= _temp2014; goto
_LL2009; _LL2013:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2009:;}
yyzzz;}); _temp1999.f3= 0; _temp1999;}); _temp1998;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1997;}); _temp1996;}); break; case 67: _LL1995: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2017=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2017[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2018; _temp2018.tag=
Cyc_TypeSpecifier_tok; _temp2018.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2019=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2019[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2020; _temp2020.tag= Cyc_Absyn_TupleType;
_temp2020.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* yyzzz;{ void* _temp2021= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct Cyc_List_List* _temp2027; _LL2023: if(*((
void**) _temp2021) ==  Cyc_ParamDeclList_tok){ _LL2028: _temp2027=(( struct Cyc_ParamDeclList_tok_struct*)
_temp2021)->f1; goto _LL2024;} else{ goto _LL2025;} _LL2025: goto _LL2026;
_LL2024: yyzzz= _temp2027; goto _LL2022; _LL2026:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL2022:;} yyzzz;}))); _temp2020;}); _temp2019;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2018;}); _temp2017;}); break; case 68: _LL2016: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2030=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2030[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2031; _temp2031.tag=
Cyc_TypeSpecifier_tok; _temp2031.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2032=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2032[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2033; _temp2033.tag= Cyc_Absyn_RgnHandleType;
_temp2033.f1=( void*)({ void* yyzzz;{ void* _temp2034= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp2040;
_LL2036: if(*(( void**) _temp2034) ==  Cyc_Type_tok){ _LL2041: _temp2040=( void*)((
struct Cyc_Type_tok_struct*) _temp2034)->f1; goto _LL2037;} else{ goto _LL2038;}
_LL2038: goto _LL2039; _LL2037: yyzzz= _temp2040; goto _LL2035; _LL2039:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL2035:;} yyzzz;}); _temp2033;});
_temp2032;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2031;}); _temp2030;}); break; case 69:
_LL2029: yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp2043=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2043[ 0]=({ struct Cyc_Kind_tok_struct
_temp2044; _temp2044.tag= Cyc_Kind_tok; _temp2044.f1=( void*) Cyc_Parse_id_to_kind(({
struct _tagged_arr yyzzz;{ void* _temp2045= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp2051; _LL2047: if(*(( void**)
_temp2045) ==  Cyc_String_tok){ _LL2052: _temp2051=(( struct Cyc_String_tok_struct*)
_temp2045)->f1; goto _LL2048;} else{ goto _LL2049;} _LL2049: goto _LL2050;
_LL2048: yyzzz= _temp2051; goto _LL2046; _LL2050:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL2046:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2044;});
_temp2043;}); break; case 70: _LL2042: { struct _tagged_arr* _temp2064; void*
_temp2066; struct _tuple1 _temp2062=*({ struct _tuple1* yyzzz;{ void* _temp2054=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp2060; _LL2056: if(*(( void**) _temp2054) ==  Cyc_QualId_tok){ _LL2061:
_temp2060=(( struct Cyc_QualId_tok_struct*) _temp2054)->f1; goto _LL2057;} else{
goto _LL2058;} _LL2058: goto _LL2059; _LL2057: yyzzz= _temp2060; goto _LL2055;
_LL2059:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2055:;} yyzzz;});
_LL2067: _temp2066= _temp2062.f1; goto _LL2065; _LL2065: _temp2064= _temp2062.f2;
goto _LL2063; _LL2063: if( _temp2066 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(
_tag_arr("bad kind in type specifier", sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp2068=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2068[ 0]=({ struct Cyc_Kind_tok_struct
_temp2069; _temp2069.tag= Cyc_Kind_tok; _temp2069.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2064, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2069;}); _temp2068;}); break;} case 71:
_LL2053: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp2071=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp2071[ 0]=({
struct Cyc_TypeQual_tok_struct _temp2072; _temp2072.tag= Cyc_TypeQual_tok;
_temp2072.f1=({ struct Cyc_Absyn_Tqual _temp2073; _temp2073.q_const= 1;
_temp2073.q_volatile= 0; _temp2073.q_restrict= 0; _temp2073;}); _temp2072;});
_temp2071;}); break; case 72: _LL2070: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2075=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2075[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2076; _temp2076.tag= Cyc_TypeQual_tok; _temp2076.f1=({ struct Cyc_Absyn_Tqual
_temp2077; _temp2077.q_const= 0; _temp2077.q_volatile= 1; _temp2077.q_restrict=
0; _temp2077;}); _temp2076;}); _temp2075;}); break; case 73: _LL2074: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp2079=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp2079[ 0]=({
struct Cyc_TypeQual_tok_struct _temp2080; _temp2080.tag= Cyc_TypeQual_tok;
_temp2080.f1=({ struct Cyc_Absyn_Tqual _temp2081; _temp2081.q_const= 0;
_temp2081.q_volatile= 0; _temp2081.q_restrict= 1; _temp2081;}); _temp2080;});
_temp2079;}); break; case 74: _LL2078: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2083=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2083[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2084; _temp2084.tag=
Cyc_TypeSpecifier_tok; _temp2084.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2085=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2085[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2086; _temp2086.tag= Cyc_Parse_Decl_spec;
_temp2086.f1=({ struct Cyc_Absyn_Decl* _temp2087=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp2087->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2088=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2088[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2089; _temp2089.tag= Cyc_Absyn_Enum_d; _temp2089.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2090=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2090->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2090->name=({ struct
_tuple1* yyzzz;{ void* _temp2100= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct _tuple1* _temp2106; _LL2102: if(*(( void**)
_temp2100) ==  Cyc_QualId_tok){ _LL2107: _temp2106=(( struct Cyc_QualId_tok_struct*)
_temp2100)->f1; goto _LL2103;} else{ goto _LL2104;} _LL2104: goto _LL2105;
_LL2103: yyzzz= _temp2106; goto _LL2101; _LL2105:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2101:;} yyzzz;}); _temp2090->fields=({ struct Cyc_Core_Opt* _temp2091=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2091->v=(
void*)({ struct Cyc_List_List* yyzzz;{ void* _temp2092= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp2098; _LL2094: if(*(( void**) _temp2092) ==  Cyc_EnumfieldList_tok){
_LL2099: _temp2098=(( struct Cyc_EnumfieldList_tok_struct*) _temp2092)->f1; goto
_LL2095;} else{ goto _LL2096;} _LL2096: goto _LL2097; _LL2095: yyzzz= _temp2098;
goto _LL2093; _LL2097:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL2093:;} yyzzz;}); _temp2091;}); _temp2090;}); _temp2089;}); _temp2088;}));
_temp2087->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2087;});
_temp2086;}); _temp2085;})); _temp2084;}); _temp2083;}); break; case 75: _LL2082:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2109=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2109[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2110; _temp2110.tag= Cyc_TypeSpecifier_tok;
_temp2110.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2111=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2111[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2112; _temp2112.tag= Cyc_Absyn_EnumType;
_temp2112.f1=({ struct _tuple1* yyzzz;{ void* _temp2113= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp2119; _LL2115: if(*(( void**) _temp2113) ==  Cyc_QualId_tok){ _LL2120:
_temp2119=(( struct Cyc_QualId_tok_struct*) _temp2113)->f1; goto _LL2116;} else{
goto _LL2117;} _LL2117: goto _LL2118; _LL2116: yyzzz= _temp2119; goto _LL2114;
_LL2118:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2114:;} yyzzz;});
_temp2112.f2= 0; _temp2112;}); _temp2111;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2110;});
_temp2109;}); break; case 76: _LL2108: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2122=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2122[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2123; _temp2123.tag= Cyc_Enumfield_tok;
_temp2123.f1=({ struct Cyc_Absyn_Enumfield* _temp2124=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2124->name=({ struct
_tuple1* yyzzz;{ void* _temp2125= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple1* _temp2131; _LL2127: if(*(( void**) _temp2125) == 
Cyc_QualId_tok){ _LL2132: _temp2131=(( struct Cyc_QualId_tok_struct*) _temp2125)->f1;
goto _LL2128;} else{ goto _LL2129;} _LL2129: goto _LL2130; _LL2128: yyzzz=
_temp2131; goto _LL2126; _LL2130:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2126:;} yyzzz;}); _temp2124->tag= 0; _temp2124->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2124;});
_temp2123;}); _temp2122;}); break; case 77: _LL2121: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2134=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2134[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2135; _temp2135.tag= Cyc_Enumfield_tok;
_temp2135.f1=({ struct Cyc_Absyn_Enumfield* _temp2136=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2136->name=({ struct
_tuple1* yyzzz;{ void* _temp2145= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tuple1* _temp2151; _LL2147: if(*(( void**)
_temp2145) ==  Cyc_QualId_tok){ _LL2152: _temp2151=(( struct Cyc_QualId_tok_struct*)
_temp2145)->f1; goto _LL2148;} else{ goto _LL2149;} _LL2149: goto _LL2150;
_LL2148: yyzzz= _temp2151; goto _LL2146; _LL2150:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2146:;} yyzzz;}); _temp2136->tag=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp2137= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Exp* _temp2143; _LL2139: if(*(( void**)
_temp2137) ==  Cyc_Exp_tok){ _LL2144: _temp2143=(( struct Cyc_Exp_tok_struct*)
_temp2137)->f1; goto _LL2140;} else{ goto _LL2141;} _LL2141: goto _LL2142;
_LL2140: yyzzz= _temp2143; goto _LL2138; _LL2142:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL2138:;} yyzzz;}); _temp2136->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2136;});
_temp2135;}); _temp2134;}); break; case 78: _LL2133: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2154=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2154[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2155; _temp2155.tag=
Cyc_EnumfieldList_tok; _temp2155.f1=({ struct Cyc_List_List* _temp2156=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2156->hd=( void*)({
struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp2157= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Enumfield*
_temp2163; _LL2159: if(*(( void**) _temp2157) ==  Cyc_Enumfield_tok){ _LL2164:
_temp2163=(( struct Cyc_Enumfield_tok_struct*) _temp2157)->f1; goto _LL2160;}
else{ goto _LL2161;} _LL2161: goto _LL2162; _LL2160: yyzzz= _temp2163; goto
_LL2158; _LL2162:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL2158:;}
yyzzz;}); _temp2156->tl= 0; _temp2156;}); _temp2155;}); _temp2154;}); break;
case 79: _LL2153: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2166=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2166[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2167; _temp2167.tag=
Cyc_EnumfieldList_tok; _temp2167.f1=({ struct Cyc_List_List* _temp2168=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2168->hd=( void*)({
struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp2177= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Enumfield*
_temp2183; _LL2179: if(*(( void**) _temp2177) ==  Cyc_Enumfield_tok){ _LL2184:
_temp2183=(( struct Cyc_Enumfield_tok_struct*) _temp2177)->f1; goto _LL2180;}
else{ goto _LL2181;} _LL2181: goto _LL2182; _LL2180: yyzzz= _temp2183; goto
_LL2178; _LL2182:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL2178:;}
yyzzz;}); _temp2168->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp2169= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2175; _LL2171: if(*(( void**) _temp2169) ==  Cyc_EnumfieldList_tok){
_LL2176: _temp2175=(( struct Cyc_EnumfieldList_tok_struct*) _temp2169)->f1; goto
_LL2172;} else{ goto _LL2173;} _LL2173: goto _LL2174; _LL2172: yyzzz= _temp2175;
goto _LL2170; _LL2174:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL2170:;} yyzzz;}); _temp2168;}); _temp2167;}); _temp2166;}); break; case 80:
_LL2165: { void* t; switch(({ enum  Cyc_Parse_Struct_or_union yyzzz;{ void*
_temp2186= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
enum  Cyc_Parse_Struct_or_union _temp2192; _LL2188: if(*(( void**) _temp2186) == 
Cyc_StructOrUnion_tok){ _LL2193: _temp2192=(( struct Cyc_StructOrUnion_tok_struct*)
_temp2186)->f1; goto _LL2189;} else{ goto _LL2190;} _LL2190: goto _LL2191;
_LL2189: yyzzz= _temp2192; goto _LL2187; _LL2191:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL2187:;} yyzzz;})){ case Cyc_Parse_Struct_su: _LL2194: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2196=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2196[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2197; _temp2197.tag= Cyc_Absyn_AnonStructType; _temp2197.f1=({ struct Cyc_List_List*
yyzzz;{ void* _temp2198= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2204; _LL2200: if(*(( void**)
_temp2198) ==  Cyc_StructFieldDeclList_tok){ _LL2205: _temp2204=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp2198)->f1; goto _LL2201;} else{ goto _LL2202;} _LL2202: goto _LL2203;
_LL2201: yyzzz= _temp2204; goto _LL2199; _LL2203:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2199:;} yyzzz;}); _temp2197;}); _temp2196;}); break; case Cyc_Parse_Union_su:
_LL2195: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp2207=( struct
Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp2207[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp2208; _temp2208.tag=
Cyc_Absyn_AnonUnionType; _temp2208.f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp2209= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp2215; _LL2211: if(*(( void**) _temp2209) ==  Cyc_StructFieldDeclList_tok){
_LL2216: _temp2215=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp2209)->f1;
goto _LL2212;} else{ goto _LL2213;} _LL2213: goto _LL2214; _LL2212: yyzzz=
_temp2215; goto _LL2210; _LL2214:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2210:;} yyzzz;}); _temp2208;}); _temp2207;}); break;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2217=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2217[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2218; _temp2218.tag= Cyc_TypeSpecifier_tok;
_temp2218.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct* _temp2219=(
struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2219[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2220; _temp2220.tag= Cyc_Parse_Type_spec;
_temp2220.f1=( void*) t; _temp2220.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2220;});
_temp2219;})); _temp2218;}); _temp2217;}); break;} case 81: _LL2185: { struct
Cyc_List_List* _temp2222=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp2274= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp2280; _LL2276: if(*(( void**) _temp2274) ==  Cyc_TypeList_tok){ _LL2281:
_temp2280=(( struct Cyc_TypeList_tok_struct*) _temp2274)->f1; goto _LL2277;}
else{ goto _LL2278;} _LL2278: goto _LL2279; _LL2277: yyzzz= _temp2280; goto
_LL2275; _LL2279:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2275:;}
yyzzz;})); struct Cyc_Absyn_Decl* d; switch(({ enum  Cyc_Parse_Struct_or_union
yyzzz;{ void* _temp2223= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]; enum  Cyc_Parse_Struct_or_union _temp2229; _LL2225: if(*((
void**) _temp2223) ==  Cyc_StructOrUnion_tok){ _LL2230: _temp2229=(( struct Cyc_StructOrUnion_tok_struct*)
_temp2223)->f1; goto _LL2226;} else{ goto _LL2227;} _LL2227: goto _LL2228;
_LL2226: yyzzz= _temp2229; goto _LL2224; _LL2228:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL2224:;} yyzzz;})){ case Cyc_Parse_Struct_su: _LL2231: d= Cyc_Absyn_struct_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp2233=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2233->v=( void*)({ struct _tuple1*
yyzzz;{ void* _temp2234= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp2240; _LL2236: if(*(( void**)
_temp2234) ==  Cyc_QualId_tok){ _LL2241: _temp2240=(( struct Cyc_QualId_tok_struct*)
_temp2234)->f1; goto _LL2237;} else{ goto _LL2238;} _LL2238: goto _LL2239;
_LL2237: yyzzz= _temp2240; goto _LL2235; _LL2239:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2235:;} yyzzz;}); _temp2233;}), _temp2222,({ struct Cyc_Core_Opt* _temp2242=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2242->v=(
void*)({ struct Cyc_List_List* yyzzz;{ void* _temp2243= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp2249; _LL2245: if(*(( void**) _temp2243) ==  Cyc_StructFieldDeclList_tok){
_LL2250: _temp2249=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp2243)->f1;
goto _LL2246;} else{ goto _LL2247;} _LL2247: goto _LL2248; _LL2246: yyzzz=
_temp2249; goto _LL2244; _LL2248:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2244:;} yyzzz;}); _temp2242;}), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); break; case
Cyc_Parse_Union_su: _LL2232: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2252=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2252->v=( void*)({ struct _tuple1* yyzzz;{ void* _temp2253=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct
_tuple1* _temp2259; _LL2255: if(*(( void**) _temp2253) ==  Cyc_QualId_tok){
_LL2260: _temp2259=(( struct Cyc_QualId_tok_struct*) _temp2253)->f1; goto
_LL2256;} else{ goto _LL2257;} _LL2257: goto _LL2258; _LL2256: yyzzz= _temp2259;
goto _LL2254; _LL2258:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2254:;}
yyzzz;}); _temp2252;}), _temp2222,({ struct Cyc_Core_Opt* _temp2261=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2261->v=( void*)({ struct Cyc_List_List*
yyzzz;{ void* _temp2262= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2268; _LL2264: if(*(( void**)
_temp2262) ==  Cyc_StructFieldDeclList_tok){ _LL2269: _temp2268=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp2262)->f1; goto _LL2265;} else{ goto _LL2266;} _LL2266: goto _LL2267;
_LL2265: yyzzz= _temp2268; goto _LL2263; _LL2267:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2263:;} yyzzz;}); _temp2261;}), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2270=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2270[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2271; _temp2271.tag= Cyc_TypeSpecifier_tok;
_temp2271.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2272=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2272[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2273; _temp2273.tag= Cyc_Parse_Decl_spec;
_temp2273.f1= d; _temp2273;}); _temp2272;})); _temp2271;}); _temp2270;}); break;}
case 82: _LL2221: { struct Cyc_List_List* _temp2283=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp2335= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp2341; _LL2337: if(*(( void**) _temp2335) ==  Cyc_TypeList_tok){ _LL2342:
_temp2341=(( struct Cyc_TypeList_tok_struct*) _temp2335)->f1; goto _LL2338;}
else{ goto _LL2339;} _LL2339: goto _LL2340; _LL2338: yyzzz= _temp2341; goto
_LL2336; _LL2340:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2336:;}
yyzzz;})); struct Cyc_Absyn_Decl* d; switch(({ enum  Cyc_Parse_Struct_or_union
yyzzz;{ void* _temp2284= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]; enum  Cyc_Parse_Struct_or_union _temp2290; _LL2286: if(*((
void**) _temp2284) ==  Cyc_StructOrUnion_tok){ _LL2291: _temp2290=(( struct Cyc_StructOrUnion_tok_struct*)
_temp2284)->f1; goto _LL2287;} else{ goto _LL2288;} _LL2288: goto _LL2289;
_LL2287: yyzzz= _temp2290; goto _LL2285; _LL2289:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL2285:;} yyzzz;})){ case Cyc_Parse_Struct_su: _LL2292: d= Cyc_Absyn_struct_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp2294=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2294->v=( void*)({ struct _tuple1*
yyzzz;{ void* _temp2295= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp2301; _LL2297: if(*(( void**)
_temp2295) ==  Cyc_QualId_tok){ _LL2302: _temp2301=(( struct Cyc_QualId_tok_struct*)
_temp2295)->f1; goto _LL2298;} else{ goto _LL2299;} _LL2299: goto _LL2300;
_LL2298: yyzzz= _temp2301; goto _LL2296; _LL2300:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2296:;} yyzzz;}); _temp2294;}), _temp2283,({ struct Cyc_Core_Opt* _temp2303=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2303->v=(
void*)({ struct Cyc_List_List* yyzzz;{ void* _temp2304= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp2310; _LL2306: if(*(( void**) _temp2304) ==  Cyc_StructFieldDeclList_tok){
_LL2311: _temp2310=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp2304)->f1;
goto _LL2307;} else{ goto _LL2308;} _LL2308: goto _LL2309; _LL2307: yyzzz=
_temp2310; goto _LL2305; _LL2309:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2305:;} yyzzz;}); _temp2303;}), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); break; case
Cyc_Parse_Union_su: _LL2293: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2313=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2313->v=( void*)({ struct _tuple1* yyzzz;{ void* _temp2314=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct
_tuple1* _temp2320; _LL2316: if(*(( void**) _temp2314) ==  Cyc_QualId_tok){
_LL2321: _temp2320=(( struct Cyc_QualId_tok_struct*) _temp2314)->f1; goto
_LL2317;} else{ goto _LL2318;} _LL2318: goto _LL2319; _LL2317: yyzzz= _temp2320;
goto _LL2315; _LL2319:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2315:;}
yyzzz;}); _temp2313;}), _temp2283,({ struct Cyc_Core_Opt* _temp2322=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2322->v=( void*)({ struct Cyc_List_List*
yyzzz;{ void* _temp2323= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2329; _LL2325: if(*(( void**)
_temp2323) ==  Cyc_StructFieldDeclList_tok){ _LL2330: _temp2329=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp2323)->f1; goto _LL2326;} else{ goto _LL2327;} _LL2327: goto _LL2328;
_LL2326: yyzzz= _temp2329; goto _LL2324; _LL2328:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2324:;} yyzzz;}); _temp2322;}), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2331=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2331[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2332; _temp2332.tag= Cyc_TypeSpecifier_tok;
_temp2332.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2333=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2333[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2334; _temp2334.tag= Cyc_Parse_Decl_spec;
_temp2334.f1= d; _temp2334;}); _temp2333;})); _temp2332;}); _temp2331;}); break;}
case 83: _LL2282: switch(({ enum  Cyc_Parse_Struct_or_union yyzzz;{ void*
_temp2344= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
enum  Cyc_Parse_Struct_or_union _temp2350; _LL2346: if(*(( void**) _temp2344) == 
Cyc_StructOrUnion_tok){ _LL2351: _temp2350=(( struct Cyc_StructOrUnion_tok_struct*)
_temp2344)->f1; goto _LL2347;} else{ goto _LL2348;} _LL2348: goto _LL2349;
_LL2347: yyzzz= _temp2350; goto _LL2345; _LL2349:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL2345:;} yyzzz;})){ case Cyc_Parse_Struct_su: _LL2352: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2354=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2354[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2355; _temp2355.tag= Cyc_TypeSpecifier_tok;
_temp2355.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2356=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2356[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2357; _temp2357.tag=
Cyc_Absyn_StructType; _temp2357.f1=( struct _tuple1*)({ struct _tuple1* yyzzz;{
void* _temp2358= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct _tuple1* _temp2364; _LL2360: if(*(( void**) _temp2358) ==  Cyc_QualId_tok){
_LL2365: _temp2364=(( struct Cyc_QualId_tok_struct*) _temp2358)->f1; goto
_LL2361;} else{ goto _LL2362;} _LL2362: goto _LL2363; _LL2361: yyzzz= _temp2364;
goto _LL2359; _LL2363:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2359:;}
yyzzz;}); _temp2357.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp2366= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2372; _LL2368: if(*(( void**) _temp2366) ==  Cyc_TypeList_tok){ _LL2373:
_temp2372=(( struct Cyc_TypeList_tok_struct*) _temp2366)->f1; goto _LL2369;}
else{ goto _LL2370;} _LL2370: goto _LL2371; _LL2369: yyzzz= _temp2372; goto
_LL2367; _LL2371:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2367:;}
yyzzz;}); _temp2357.f3= 0; _temp2357;}); _temp2356;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2355;}); _temp2354;}); break; case Cyc_Parse_Union_su: _LL2353: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2375=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2375[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2376; _temp2376.tag= Cyc_TypeSpecifier_tok;
_temp2376.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2377=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2377[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2378; _temp2378.tag= Cyc_Absyn_UnionType;
_temp2378.f1=( struct _tuple1*)({ struct _tuple1* yyzzz;{ void* _temp2379= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple1*
_temp2385; _LL2381: if(*(( void**) _temp2379) ==  Cyc_QualId_tok){ _LL2386:
_temp2385=(( struct Cyc_QualId_tok_struct*) _temp2379)->f1; goto _LL2382;} else{
goto _LL2383;} _LL2383: goto _LL2384; _LL2382: yyzzz= _temp2385; goto _LL2380;
_LL2384:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2380:;} yyzzz;});
_temp2378.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp2387= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2393; _LL2389: if(*(( void**) _temp2387) ==  Cyc_TypeList_tok){ _LL2394:
_temp2393=(( struct Cyc_TypeList_tok_struct*) _temp2387)->f1; goto _LL2390;}
else{ goto _LL2391;} _LL2391: goto _LL2392; _LL2390: yyzzz= _temp2393; goto
_LL2388; _LL2392:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2388:;}
yyzzz;}); _temp2378.f3= 0; _temp2378;}); _temp2377;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2376;}); _temp2375;}); break;} break; case 84: _LL2343: switch(({ enum  Cyc_Parse_Struct_or_union
yyzzz;{ void* _temp2396= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; enum  Cyc_Parse_Struct_or_union _temp2402; _LL2398: if(*((
void**) _temp2396) ==  Cyc_StructOrUnion_tok){ _LL2403: _temp2402=(( struct Cyc_StructOrUnion_tok_struct*)
_temp2396)->f1; goto _LL2399;} else{ goto _LL2400;} _LL2400: goto _LL2401;
_LL2399: yyzzz= _temp2402; goto _LL2397; _LL2401:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL2397:;} yyzzz;})){ case Cyc_Parse_Struct_su: _LL2404: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2406=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2406[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2407; _temp2407.tag= Cyc_TypeSpecifier_tok;
_temp2407.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2408=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2408[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2409; _temp2409.tag=
Cyc_Absyn_StructType; _temp2409.f1=( struct _tuple1*)({ struct _tuple1* yyzzz;{
void* _temp2410= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct _tuple1* _temp2416; _LL2412: if(*(( void**) _temp2410) ==  Cyc_QualId_tok){
_LL2417: _temp2416=(( struct Cyc_QualId_tok_struct*) _temp2410)->f1; goto
_LL2413;} else{ goto _LL2414;} _LL2414: goto _LL2415; _LL2413: yyzzz= _temp2416;
goto _LL2411; _LL2415:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2411:;}
yyzzz;}); _temp2409.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp2418= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2424; _LL2420: if(*(( void**) _temp2418) ==  Cyc_TypeList_tok){ _LL2425:
_temp2424=(( struct Cyc_TypeList_tok_struct*) _temp2418)->f1; goto _LL2421;}
else{ goto _LL2422;} _LL2422: goto _LL2423; _LL2421: yyzzz= _temp2424; goto
_LL2419; _LL2423:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2419:;}
yyzzz;}); _temp2409.f3= 0; _temp2409;}); _temp2408;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2407;}); _temp2406;}); break; case Cyc_Parse_Union_su: _LL2405: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2427=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2427[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2428; _temp2428.tag= Cyc_TypeSpecifier_tok;
_temp2428.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2429=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2429[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2430; _temp2430.tag= Cyc_Absyn_UnionType;
_temp2430.f1=( struct _tuple1*)({ struct _tuple1* yyzzz;{ void* _temp2431= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple1*
_temp2437; _LL2433: if(*(( void**) _temp2431) ==  Cyc_QualId_tok){ _LL2438:
_temp2437=(( struct Cyc_QualId_tok_struct*) _temp2431)->f1; goto _LL2434;} else{
goto _LL2435;} _LL2435: goto _LL2436; _LL2434: yyzzz= _temp2437; goto _LL2432;
_LL2436:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2432:;} yyzzz;});
_temp2430.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp2439= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2445; _LL2441: if(*(( void**) _temp2439) ==  Cyc_TypeList_tok){ _LL2446:
_temp2445=(( struct Cyc_TypeList_tok_struct*) _temp2439)->f1; goto _LL2442;}
else{ goto _LL2443;} _LL2443: goto _LL2444; _LL2442: yyzzz= _temp2445; goto
_LL2440; _LL2444:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2440:;}
yyzzz;}); _temp2430.f3= 0; _temp2430;}); _temp2429;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2428;}); _temp2427;}); break;} break; case 85: _LL2395: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2448=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2448[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2449; _temp2449.tag= Cyc_TypeList_tok; _temp2449.f1= 0; _temp2449;});
_temp2448;}); break; case 86: _LL2447: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2451=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2451[ 0]=({ struct Cyc_TypeList_tok_struct _temp2452; _temp2452.tag= Cyc_TypeList_tok;
_temp2452.f1= Cyc_List_imp_rev(({ struct Cyc_List_List* yyzzz;{ void* _temp2453=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp2459; _LL2455: if(*(( void**) _temp2453) ==  Cyc_TypeList_tok){ _LL2460:
_temp2459=(( struct Cyc_TypeList_tok_struct*) _temp2453)->f1; goto _LL2456;}
else{ goto _LL2457;} _LL2457: goto _LL2458; _LL2456: yyzzz= _temp2459; goto
_LL2454; _LL2458:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2454:;}
yyzzz;})); _temp2452;}); _temp2451;}); break; case 87: _LL2450:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2462=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2462[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2463; _temp2463.tag= Cyc_TypeList_tok; _temp2463.f1= Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp2464= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp2470; _LL2466: if(*(( void**) _temp2464) ==  Cyc_TypeList_tok){ _LL2471:
_temp2470=(( struct Cyc_TypeList_tok_struct*) _temp2464)->f1; goto _LL2467;}
else{ goto _LL2468;} _LL2468: goto _LL2469; _LL2467: yyzzz= _temp2470; goto
_LL2465; _LL2469:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2465:;}
yyzzz;})); _temp2463;}); _temp2462;}); break; case 88: _LL2461: yyval=( void*)({
struct Cyc_StructOrUnion_tok_struct* _temp2473=( struct Cyc_StructOrUnion_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp2473[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp2474; _temp2474.tag= Cyc_StructOrUnion_tok;
_temp2474.f1= Cyc_Parse_Struct_su; _temp2474;}); _temp2473;}); break; case 89:
_LL2472: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct* _temp2476=(
struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2476[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2477; _temp2477.tag=
Cyc_StructOrUnion_tok; _temp2477.f1= Cyc_Parse_Union_su; _temp2477;}); _temp2476;});
break; case 90: _LL2475: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2479=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2479[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2480; _temp2480.tag= Cyc_StructFieldDeclList_tok; _temp2480.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{
void* _temp2481= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp2487; _LL2483: if(*(( void**) _temp2481) ==  Cyc_StructFieldDeclListList_tok){
_LL2488: _temp2487=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp2481)->f1;
goto _LL2484;} else{ goto _LL2485;} _LL2485: goto _LL2486; _LL2484: yyzzz=
_temp2487; goto _LL2482; _LL2486:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL2482:;} yyzzz;}))); _temp2480;}); _temp2479;}); break; case 91: _LL2478:
yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2490=(
struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct));
_temp2490[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct _temp2491;
_temp2491.tag= Cyc_StructFieldDeclListList_tok; _temp2491.f1=({ struct Cyc_List_List*
_temp2492=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2492->hd=( void*)({ struct Cyc_List_List* yyzzz;{ void* _temp2493= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2499; _LL2495: if(*(( void**) _temp2493) ==  Cyc_StructFieldDeclList_tok){
_LL2500: _temp2499=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp2493)->f1;
goto _LL2496;} else{ goto _LL2497;} _LL2497: goto _LL2498; _LL2496: yyzzz=
_temp2499; goto _LL2494; _LL2498:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2494:;} yyzzz;}); _temp2492->tl= 0; _temp2492;}); _temp2491;}); _temp2490;});
break; case 92: _LL2489: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2502=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2502[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2503; _temp2503.tag= Cyc_StructFieldDeclListList_tok; _temp2503.f1=({
struct Cyc_List_List* _temp2504=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2504->hd=( void*)({ struct Cyc_List_List* yyzzz;{
void* _temp2513= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp2519; _LL2515: if(*(( void**) _temp2513) ==  Cyc_StructFieldDeclList_tok){
_LL2520: _temp2519=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp2513)->f1;
goto _LL2516;} else{ goto _LL2517;} _LL2517: goto _LL2518; _LL2516: yyzzz=
_temp2519; goto _LL2514; _LL2518:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2514:;} yyzzz;}); _temp2504->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp2505= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp2511; _LL2507: if(*(( void**) _temp2505) ==  Cyc_StructFieldDeclListList_tok){
_LL2512: _temp2511=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp2505)->f1;
goto _LL2508;} else{ goto _LL2509;} _LL2509: goto _LL2510; _LL2508: yyzzz=
_temp2511; goto _LL2506; _LL2510:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL2506:;} yyzzz;}); _temp2504;}); _temp2503;}); _temp2502;}); break; case 93:
_LL2501: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2522=( struct
Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2522[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2523; _temp2523.tag=
Cyc_InitDeclList_tok; _temp2523.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp2524= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2530; _LL2526: if(*(( void**) _temp2524) ==  Cyc_InitDeclList_tok){ _LL2531:
_temp2530=(( struct Cyc_InitDeclList_tok_struct*) _temp2524)->f1; goto _LL2527;}
else{ goto _LL2528;} _LL2528: goto _LL2529; _LL2527: yyzzz= _temp2530; goto
_LL2525; _LL2529:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL2525:;}
yyzzz;})); _temp2523;}); _temp2522;}); break; case 94: _LL2521: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp2533=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp2533[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp2534; _temp2534.tag= Cyc_InitDeclList_tok;
_temp2534.f1=({ struct Cyc_List_List* _temp2535=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2535->hd=( void*)({ struct
_tuple13* yyzzz;{ void* _temp2536= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple13* _temp2542; _LL2538: if(*(( void**) _temp2536)
==  Cyc_InitDecl_tok){ _LL2543: _temp2542=(( struct Cyc_InitDecl_tok_struct*)
_temp2536)->f1; goto _LL2539;} else{ goto _LL2540;} _LL2540: goto _LL2541;
_LL2539: yyzzz= _temp2542; goto _LL2537; _LL2541:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL2537:;} yyzzz;}); _temp2535->tl= 0; _temp2535;}); _temp2534;}); _temp2533;});
break; case 95: _LL2532: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp2545=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2545[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2546; _temp2546.tag=
Cyc_InitDeclList_tok; _temp2546.f1=({ struct Cyc_List_List* _temp2547=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2547->hd=( void*)({
struct _tuple13* yyzzz;{ void* _temp2556= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tuple13* _temp2562; _LL2558: if(*(( void**)
_temp2556) ==  Cyc_InitDecl_tok){ _LL2563: _temp2562=(( struct Cyc_InitDecl_tok_struct*)
_temp2556)->f1; goto _LL2559;} else{ goto _LL2560;} _LL2560: goto _LL2561;
_LL2559: yyzzz= _temp2562; goto _LL2557; _LL2561:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL2557:;} yyzzz;}); _temp2547->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp2548= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp2554; _LL2550: if(*(( void**) _temp2548) ==  Cyc_InitDeclList_tok){
_LL2555: _temp2554=(( struct Cyc_InitDeclList_tok_struct*) _temp2548)->f1; goto
_LL2551;} else{ goto _LL2552;} _LL2552: goto _LL2553; _LL2551: yyzzz= _temp2554;
goto _LL2549; _LL2553:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL2549:;} yyzzz;}); _temp2547;}); _temp2546;}); _temp2545;}); break; case 96:
_LL2544: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2565=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2565[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2566; _temp2566.tag= Cyc_InitDecl_tok; _temp2566.f1=({ struct _tuple13*
_temp2567=( struct _tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2567->f1=({
struct Cyc_Parse_Declarator* yyzzz;{ void* _temp2568= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declarator*
_temp2574; _LL2570: if(*(( void**) _temp2568) ==  Cyc_Declarator_tok){ _LL2575:
_temp2574=(( struct Cyc_Declarator_tok_struct*) _temp2568)->f1; goto _LL2571;}
else{ goto _LL2572;} _LL2572: goto _LL2573; _LL2571: yyzzz= _temp2574; goto
_LL2569; _LL2573:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL2569:;}
yyzzz;}); _temp2567->f2= 0; _temp2567;}); _temp2566;}); _temp2565;}); break;
case 97: _LL2564: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2577=(
struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp2577[ 0]=({ struct Cyc_InitDecl_tok_struct _temp2578; _temp2578.tag= Cyc_InitDecl_tok;
_temp2578.f1=({ struct _tuple13* _temp2579=( struct _tuple13*) _cycalloc(
sizeof( struct _tuple13)); _temp2579->f1=({ struct Cyc_Parse_Declarator* yyzzz;{
void* _temp2588= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_Parse_Declarator* _temp2594; _LL2590: if(*(( void**) _temp2588) == 
Cyc_Declarator_tok){ _LL2595: _temp2594=(( struct Cyc_Declarator_tok_struct*)
_temp2588)->f1; goto _LL2591;} else{ goto _LL2592;} _LL2592: goto _LL2593;
_LL2591: yyzzz= _temp2594; goto _LL2589; _LL2593:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL2589:;} yyzzz;}); _temp2579->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp2580= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Exp* _temp2586; _LL2582: if(*(( void**)
_temp2580) ==  Cyc_Exp_tok){ _LL2587: _temp2586=(( struct Cyc_Exp_tok_struct*)
_temp2580)->f1; goto _LL2583;} else{ goto _LL2584;} _LL2584: goto _LL2585;
_LL2583: yyzzz= _temp2586; goto _LL2581; _LL2585:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL2581:;} yyzzz;}); _temp2579;}); _temp2578;}); _temp2577;}); break; case 98:
_LL2576: { struct _tuple17 _temp2607; struct Cyc_List_List* _temp2608; struct
Cyc_List_List* _temp2610; struct Cyc_Absyn_Tqual _temp2612; struct _tuple17*
_temp2605=({ struct _tuple17* yyzzz;{ void* _temp2597= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct _tuple17*
_temp2603; _LL2599: if(*(( void**) _temp2597) ==  Cyc_QualSpecList_tok){ _LL2604:
_temp2603=(( struct Cyc_QualSpecList_tok_struct*) _temp2597)->f1; goto _LL2600;}
else{ goto _LL2601;} _LL2601: goto _LL2602; _LL2600: yyzzz= _temp2603; goto
_LL2598; _LL2602:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL2598:;}
yyzzz;}); _temp2607=* _temp2605; _LL2613: _temp2612= _temp2607.f1; goto _LL2611;
_LL2611: _temp2610= _temp2607.f2; goto _LL2609; _LL2609: _temp2608= _temp2607.f3;
goto _LL2606; _LL2606: { struct Cyc_List_List* _temp2624; struct Cyc_List_List*
_temp2626; struct _tuple0 _temp2622=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)(({ struct Cyc_List_List* yyzzz;{ void* _temp2614= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp2620; _LL2616: if(*(( void**) _temp2614) ==  Cyc_DeclaratorExpoptList_tok){
_LL2621: _temp2620=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp2614)->f1;
goto _LL2617;} else{ goto _LL2618;} _LL2618: goto _LL2619; _LL2617: yyzzz=
_temp2620; goto _LL2615; _LL2619:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL2615:;} yyzzz;})); _LL2627: _temp2626= _temp2622.f1; goto _LL2625; _LL2625:
_temp2624= _temp2622.f2; goto _LL2623; _LL2623: { void* _temp2628= Cyc_Parse_speclist2typ(
_temp2610, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp2629=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp2612, _temp2628, _temp2626, _temp2608), _temp2624);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp2630=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp2630[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp2631; _temp2631.tag=
Cyc_StructFieldDeclList_tok; _temp2631.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp2629); _temp2631;}); _temp2630;}); break;}}}
case 99: _LL2596: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2633=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2633[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2634; _temp2634.tag=
Cyc_QualSpecList_tok; _temp2634.f1=({ struct _tuple17* _temp2635=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp2635->f1= Cyc_Absyn_empty_tqual();
_temp2635->f2=({ struct Cyc_List_List* _temp2644=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2644->hd=( void*)({ void* yyzzz;{
void* _temp2645= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
void* _temp2651; _LL2647: if(*(( void**) _temp2645) ==  Cyc_TypeSpecifier_tok){
_LL2652: _temp2651=( void*)(( struct Cyc_TypeSpecifier_tok_struct*) _temp2645)->f1;
goto _LL2648;} else{ goto _LL2649;} _LL2649: goto _LL2650; _LL2648: yyzzz=
_temp2651; goto _LL2646; _LL2650:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL2646:;} yyzzz;}); _temp2644->tl= 0; _temp2644;}); _temp2635->f3=({ struct Cyc_List_List*
yyzzz;{ void* _temp2636= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_List_List* _temp2642; _LL2638: if(*(( void**)
_temp2636) ==  Cyc_AttributeList_tok){ _LL2643: _temp2642=(( struct Cyc_AttributeList_tok_struct*)
_temp2636)->f1; goto _LL2639;} else{ goto _LL2640;} _LL2640: goto _LL2641;
_LL2639: yyzzz= _temp2642; goto _LL2637; _LL2641:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL2637:;} yyzzz;}); _temp2635;}); _temp2634;}); _temp2633;}); break; case 100:
_LL2632: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2654=( struct
Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2654[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2655; _temp2655.tag=
Cyc_QualSpecList_tok; _temp2655.f1=({ struct _tuple17* _temp2656=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp2656->f1=(*({ struct
_tuple17* yyzzz;{ void* _temp2690= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple17* _temp2696; _LL2692: if(*(( void**) _temp2690)
==  Cyc_QualSpecList_tok){ _LL2697: _temp2696=(( struct Cyc_QualSpecList_tok_struct*)
_temp2690)->f1; goto _LL2693;} else{ goto _LL2694;} _LL2694: goto _LL2695;
_LL2693: yyzzz= _temp2696; goto _LL2691; _LL2695:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL2691:;} yyzzz;})).f1; _temp2656->f2=({ struct Cyc_List_List* _temp2673=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2673->hd=(
void*)({ void* yyzzz;{ void* _temp2682= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; void* _temp2688; _LL2684: if(*(( void**) _temp2682)
==  Cyc_TypeSpecifier_tok){ _LL2689: _temp2688=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp2682)->f1; goto _LL2685;} else{ goto _LL2686;} _LL2686: goto _LL2687;
_LL2685: yyzzz= _temp2688; goto _LL2683; _LL2687:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL2683:;} yyzzz;}); _temp2673->tl=(*({ struct _tuple17* yyzzz;{ void* _temp2674=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp2680; _LL2676: if(*(( void**) _temp2674) ==  Cyc_QualSpecList_tok){ _LL2681:
_temp2680=(( struct Cyc_QualSpecList_tok_struct*) _temp2674)->f1; goto _LL2677;}
else{ goto _LL2678;} _LL2678: goto _LL2679; _LL2677: yyzzz= _temp2680; goto
_LL2675; _LL2679:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL2675:;}
yyzzz;})).f2; _temp2673;}); _temp2656->f3= Cyc_List_append(({ struct Cyc_List_List*
yyzzz;{ void* _temp2657= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2663; _LL2659: if(*(( void**)
_temp2657) ==  Cyc_AttributeList_tok){ _LL2664: _temp2663=(( struct Cyc_AttributeList_tok_struct*)
_temp2657)->f1; goto _LL2660;} else{ goto _LL2661;} _LL2661: goto _LL2662;
_LL2660: yyzzz= _temp2663; goto _LL2658; _LL2662:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL2658:;} yyzzz;}),(*({ struct _tuple17* yyzzz;{ void* _temp2665= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp2671; _LL2667: if(*(( void**) _temp2665) ==  Cyc_QualSpecList_tok){ _LL2672:
_temp2671=(( struct Cyc_QualSpecList_tok_struct*) _temp2665)->f1; goto _LL2668;}
else{ goto _LL2669;} _LL2669: goto _LL2670; _LL2668: yyzzz= _temp2671; goto
_LL2666; _LL2670:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL2666:;}
yyzzz;})).f3); _temp2656;}); _temp2655;}); _temp2654;}); break; case 101:
_LL2653: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2699=( struct
Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2699[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2700; _temp2700.tag=
Cyc_QualSpecList_tok; _temp2700.f1=({ struct _tuple17* _temp2701=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp2701->f1=({ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp2710= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Absyn_Tqual _temp2716; _LL2712: if(*(( void**)
_temp2710) ==  Cyc_TypeQual_tok){ _LL2717: _temp2716=(( struct Cyc_TypeQual_tok_struct*)
_temp2710)->f1; goto _LL2713;} else{ goto _LL2714;} _LL2714: goto _LL2715;
_LL2713: yyzzz= _temp2716; goto _LL2711; _LL2715:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL2711:;} yyzzz;}); _temp2701->f2= 0; _temp2701->f3=({ struct Cyc_List_List*
yyzzz;{ void* _temp2702= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_List_List* _temp2708; _LL2704: if(*(( void**)
_temp2702) ==  Cyc_AttributeList_tok){ _LL2709: _temp2708=(( struct Cyc_AttributeList_tok_struct*)
_temp2702)->f1; goto _LL2705;} else{ goto _LL2706;} _LL2706: goto _LL2707;
_LL2705: yyzzz= _temp2708; goto _LL2703; _LL2707:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL2703:;} yyzzz;}); _temp2701;}); _temp2700;}); _temp2699;}); break; case 102:
_LL2698: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2719=( struct
Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2719[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2720; _temp2720.tag=
Cyc_QualSpecList_tok; _temp2720.f1=({ struct _tuple17* _temp2721=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp2721->f1= Cyc_Absyn_combine_tqual(({
struct Cyc_Absyn_Tqual yyzzz;{ void* _temp2746= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Tqual
_temp2752; _LL2748: if(*(( void**) _temp2746) ==  Cyc_TypeQual_tok){ _LL2753:
_temp2752=(( struct Cyc_TypeQual_tok_struct*) _temp2746)->f1; goto _LL2749;}
else{ goto _LL2750;} _LL2750: goto _LL2751; _LL2749: yyzzz= _temp2752; goto
_LL2747; _LL2751:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL2747:;}
yyzzz;}),(*({ struct _tuple17* yyzzz;{ void* _temp2754= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp2760; _LL2756: if(*(( void**) _temp2754) ==  Cyc_QualSpecList_tok){ _LL2761:
_temp2760=(( struct Cyc_QualSpecList_tok_struct*) _temp2754)->f1; goto _LL2757;}
else{ goto _LL2758;} _LL2758: goto _LL2759; _LL2757: yyzzz= _temp2760; goto
_LL2755; _LL2759:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL2755:;}
yyzzz;})).f1); _temp2721->f2=(*({ struct _tuple17* yyzzz;{ void* _temp2738= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp2744; _LL2740: if(*(( void**) _temp2738) ==  Cyc_QualSpecList_tok){ _LL2745:
_temp2744=(( struct Cyc_QualSpecList_tok_struct*) _temp2738)->f1; goto _LL2741;}
else{ goto _LL2742;} _LL2742: goto _LL2743; _LL2741: yyzzz= _temp2744; goto
_LL2739; _LL2743:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL2739:;}
yyzzz;})).f2; _temp2721->f3= Cyc_List_append(({ struct Cyc_List_List* yyzzz;{
void* _temp2722= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp2728; _LL2724: if(*(( void**) _temp2722) ==  Cyc_AttributeList_tok){
_LL2729: _temp2728=(( struct Cyc_AttributeList_tok_struct*) _temp2722)->f1; goto
_LL2725;} else{ goto _LL2726;} _LL2726: goto _LL2727; _LL2725: yyzzz= _temp2728;
goto _LL2723; _LL2727:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL2723:;} yyzzz;}),(*({ struct _tuple17* yyzzz;{ void* _temp2730= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp2736; _LL2732: if(*(( void**) _temp2730) ==  Cyc_QualSpecList_tok){ _LL2737:
_temp2736=(( struct Cyc_QualSpecList_tok_struct*) _temp2730)->f1; goto _LL2733;}
else{ goto _LL2734;} _LL2734: goto _LL2735; _LL2733: yyzzz= _temp2736; goto
_LL2731; _LL2735:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL2731:;}
yyzzz;})).f3); _temp2721;}); _temp2720;}); _temp2719;}); break; case 103:
_LL2718: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2763=(
struct Cyc_DeclaratorExpoptList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp2763[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp2764; _temp2764.tag=
Cyc_DeclaratorExpoptList_tok; _temp2764.f1=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void*
_temp2765= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp2771; _LL2767: if(*(( void**) _temp2765) ==  Cyc_DeclaratorExpoptList_tok){
_LL2772: _temp2771=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp2765)->f1;
goto _LL2768;} else{ goto _LL2769;} _LL2769: goto _LL2770; _LL2768: yyzzz=
_temp2771; goto _LL2766; _LL2770:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL2766:;} yyzzz;})); _temp2764;}); _temp2763;}); break; case 104: _LL2762:
yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2774=( struct
Cyc_DeclaratorExpoptList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp2774[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp2775; _temp2775.tag=
Cyc_DeclaratorExpoptList_tok; _temp2775.f1=({ struct Cyc_List_List* _temp2776=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2776->hd=(
void*)({ struct _tuple13* yyzzz;{ void* _temp2777= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple13*
_temp2783; _LL2779: if(*(( void**) _temp2777) ==  Cyc_DeclaratorExpopt_tok){
_LL2784: _temp2783=(( struct Cyc_DeclaratorExpopt_tok_struct*) _temp2777)->f1;
goto _LL2780;} else{ goto _LL2781;} _LL2781: goto _LL2782; _LL2780: yyzzz=
_temp2783; goto _LL2778; _LL2782:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL2778:;} yyzzz;}); _temp2776->tl= 0; _temp2776;}); _temp2775;}); _temp2774;});
break; case 105: _LL2773: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp2786=( struct Cyc_DeclaratorExpoptList_tok_struct*) _cycalloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2786[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp2787; _temp2787.tag= Cyc_DeclaratorExpoptList_tok; _temp2787.f1=({ struct
Cyc_List_List* _temp2788=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2788->hd=( void*)({ struct _tuple13* yyzzz;{ void* _temp2797= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple13*
_temp2803; _LL2799: if(*(( void**) _temp2797) ==  Cyc_DeclaratorExpopt_tok){
_LL2804: _temp2803=(( struct Cyc_DeclaratorExpopt_tok_struct*) _temp2797)->f1;
goto _LL2800;} else{ goto _LL2801;} _LL2801: goto _LL2802; _LL2800: yyzzz=
_temp2803; goto _LL2798; _LL2802:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL2798:;} yyzzz;}); _temp2788->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp2789= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp2795; _LL2791: if(*(( void**) _temp2789) ==  Cyc_DeclaratorExpoptList_tok){
_LL2796: _temp2795=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp2789)->f1;
goto _LL2792;} else{ goto _LL2793;} _LL2793: goto _LL2794; _LL2792: yyzzz=
_temp2795; goto _LL2790; _LL2794:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL2790:;} yyzzz;}); _temp2788;}); _temp2787;}); _temp2786;}); break; case 106:
_LL2785: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2806=(
struct Cyc_DeclaratorExpopt_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2806[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2807; _temp2807.tag=
Cyc_DeclaratorExpopt_tok; _temp2807.f1=({ struct _tuple13* _temp2808=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2808->f1=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp2809= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declarator* _temp2815; _LL2811: if(*(( void**)
_temp2809) ==  Cyc_Declarator_tok){ _LL2816: _temp2815=(( struct Cyc_Declarator_tok_struct*)
_temp2809)->f1; goto _LL2812;} else{ goto _LL2813;} _LL2813: goto _LL2814;
_LL2812: yyzzz= _temp2815; goto _LL2810; _LL2814:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL2810:;} yyzzz;}); _temp2808->f2= 0; _temp2808;}); _temp2807;}); _temp2806;});
break; case 107: _LL2805: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2818=( struct Cyc_DeclaratorExpopt_tok_struct*) _cycalloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2818[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2819; _temp2819.tag= Cyc_DeclaratorExpopt_tok; _temp2819.f1=({ struct
_tuple13* _temp2820=( struct _tuple13*) _cycalloc( sizeof( struct _tuple13));
_temp2820->f1=({ struct Cyc_Parse_Declarator* _temp2829=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2829->id=({ struct
_tuple1* _temp2830=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2830->f1= Cyc_Absyn_rel_ns_null; _temp2830->f2= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char),
1u); _temp2830;}); _temp2829->tms= 0; _temp2829;}); _temp2820->f2=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp2821= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp2827; _LL2823: if(*(( void**) _temp2821) ==  Cyc_Exp_tok){ _LL2828:
_temp2827=(( struct Cyc_Exp_tok_struct*) _temp2821)->f1; goto _LL2824;} else{
goto _LL2825;} _LL2825: goto _LL2826; _LL2824: yyzzz= _temp2827; goto _LL2822;
_LL2826:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL2822:;} yyzzz;});
_temp2820;}); _temp2819;}); _temp2818;}); break; case 108: _LL2817: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2832=( struct Cyc_DeclaratorExpopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2832[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2833; _temp2833.tag= Cyc_DeclaratorExpopt_tok;
_temp2833.f1=({ struct _tuple13* _temp2834=( struct _tuple13*) _cycalloc(
sizeof( struct _tuple13)); _temp2834->f1=({ struct Cyc_Parse_Declarator* yyzzz;{
void* _temp2843= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_Parse_Declarator* _temp2849; _LL2845: if(*(( void**) _temp2843) == 
Cyc_Declarator_tok){ _LL2850: _temp2849=(( struct Cyc_Declarator_tok_struct*)
_temp2843)->f1; goto _LL2846;} else{ goto _LL2847;} _LL2847: goto _LL2848;
_LL2846: yyzzz= _temp2849; goto _LL2844; _LL2848:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL2844:;} yyzzz;}); _temp2834->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp2835= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Exp* _temp2841; _LL2837: if(*(( void**)
_temp2835) ==  Cyc_Exp_tok){ _LL2842: _temp2841=(( struct Cyc_Exp_tok_struct*)
_temp2835)->f1; goto _LL2838;} else{ goto _LL2839;} _LL2839: goto _LL2840;
_LL2838: yyzzz= _temp2841; goto _LL2836; _LL2840:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL2836:;} yyzzz;}); _temp2834;}); _temp2833;}); _temp2832;}); break; case 109:
_LL2831: { struct Cyc_List_List* _temp2852=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp2882= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp2888; _LL2884: if(*(( void**) _temp2882) ==  Cyc_TypeList_tok){ _LL2889:
_temp2888=(( struct Cyc_TypeList_tok_struct*) _temp2882)->f1; goto _LL2885;}
else{ goto _LL2886;} _LL2886: goto _LL2887; _LL2885: yyzzz= _temp2888; goto
_LL2883; _LL2887:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2883:;}
yyzzz;})); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2853=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2853[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2854; _temp2854.tag=
Cyc_TypeSpecifier_tok; _temp2854.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2855=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2855[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2856; _temp2856.tag= Cyc_Parse_Decl_spec;
_temp2856.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct _tuple1*
yyzzz;{ void* _temp2857= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp2863; _LL2859: if(*(( void**)
_temp2857) ==  Cyc_QualId_tok){ _LL2864: _temp2863=(( struct Cyc_QualId_tok_struct*)
_temp2857)->f1; goto _LL2860;} else{ goto _LL2861;} _LL2861: goto _LL2862;
_LL2860: yyzzz= _temp2863; goto _LL2858; _LL2862:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2858:;} yyzzz;}), _temp2852,({ struct Cyc_Core_Opt* _temp2865=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2865->v=( void*)({ struct Cyc_List_List*
yyzzz;{ void* _temp2866= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2872; _LL2868: if(*(( void**)
_temp2866) ==  Cyc_TunionFieldList_tok){ _LL2873: _temp2872=(( struct Cyc_TunionFieldList_tok_struct*)
_temp2866)->f1; goto _LL2869;} else{ goto _LL2870;} _LL2870: goto _LL2871;
_LL2869: yyzzz= _temp2872; goto _LL2867; _LL2871:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL2867:;} yyzzz;}); _temp2865;}),({ int yyzzz;{ void* _temp2874= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; int _temp2880;
_LL2876: if(*(( void**) _temp2874) ==  Cyc_Bool_tok){ _LL2881: _temp2880=((
struct Cyc_Bool_tok_struct*) _temp2874)->f1; goto _LL2877;} else{ goto _LL2878;}
_LL2878: goto _LL2879; _LL2877: yyzzz= _temp2880; goto _LL2875; _LL2879:( int)
_throw(( void*)& Cyc_yyfail_Bool_tok); _LL2875:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2856;}); _temp2855;})); _temp2854;}); _temp2853;}); break;} case 110:
_LL2851: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2891=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2891[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2892; _temp2892.tag=
Cyc_TypeSpecifier_tok; _temp2892.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2893=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2893[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2894; _temp2894.tag= Cyc_Absyn_TunionType;
_temp2894.f1=({ struct Cyc_Absyn_TunionInfo _temp2895; _temp2895.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2912=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2912[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2913; _temp2913.tag= Cyc_Absyn_UnknownTunion;
_temp2913.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2914; _temp2914.name=({
struct _tuple1* yyzzz;{ void* _temp2923= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct _tuple1* _temp2929; _LL2925: if(*(( void**)
_temp2923) ==  Cyc_QualId_tok){ _LL2930: _temp2929=(( struct Cyc_QualId_tok_struct*)
_temp2923)->f1; goto _LL2926;} else{ goto _LL2927;} _LL2927: goto _LL2928;
_LL2926: yyzzz= _temp2929; goto _LL2924; _LL2928:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2924:;} yyzzz;}); _temp2914.is_xtunion=({ int yyzzz;{ void* _temp2915= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; int _temp2921;
_LL2917: if(*(( void**) _temp2915) ==  Cyc_Bool_tok){ _LL2922: _temp2921=((
struct Cyc_Bool_tok_struct*) _temp2915)->f1; goto _LL2918;} else{ goto _LL2919;}
_LL2919: goto _LL2920; _LL2918: yyzzz= _temp2921; goto _LL2916; _LL2920:( int)
_throw(( void*)& Cyc_yyfail_Bool_tok); _LL2916:;} yyzzz;}); _temp2914;});
_temp2913;}); _temp2912;})); _temp2895.targs=({ struct Cyc_List_List* yyzzz;{
void* _temp2904= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp2910; _LL2906: if(*(( void**) _temp2904) ==  Cyc_TypeList_tok){
_LL2911: _temp2910=(( struct Cyc_TypeList_tok_struct*) _temp2904)->f1; goto
_LL2907;} else{ goto _LL2908;} _LL2908: goto _LL2909; _LL2907: yyzzz= _temp2910;
goto _LL2905; _LL2909:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2905:;}
yyzzz;}); _temp2895.rgn=( void*)({ void* yyzzz;{ void* _temp2896= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void* _temp2902;
_LL2898: if(*(( void**) _temp2896) ==  Cyc_Type_tok){ _LL2903: _temp2902=( void*)((
struct Cyc_Type_tok_struct*) _temp2896)->f1; goto _LL2899;} else{ goto _LL2900;}
_LL2900: goto _LL2901; _LL2899: yyzzz= _temp2902; goto _LL2897; _LL2901:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL2897:;} yyzzz;}); _temp2895;});
_temp2894;}); _temp2893;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2892;});
_temp2891;}); break; case 111: _LL2890: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2932=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2932[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2933; _temp2933.tag=
Cyc_TypeSpecifier_tok; _temp2933.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2934=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2934[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2935; _temp2935.tag= Cyc_Absyn_TunionType;
_temp2935.f1=({ struct Cyc_Absyn_TunionInfo _temp2936; _temp2936.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2945=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2945[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2946; _temp2946.tag= Cyc_Absyn_UnknownTunion;
_temp2946.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2947; _temp2947.name=({
struct _tuple1* yyzzz;{ void* _temp2956= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct _tuple1* _temp2962; _LL2958: if(*(( void**)
_temp2956) ==  Cyc_QualId_tok){ _LL2963: _temp2962=(( struct Cyc_QualId_tok_struct*)
_temp2956)->f1; goto _LL2959;} else{ goto _LL2960;} _LL2960: goto _LL2961;
_LL2959: yyzzz= _temp2962; goto _LL2957; _LL2961:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2957:;} yyzzz;}); _temp2947.is_xtunion=({ int yyzzz;{ void* _temp2948= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; int _temp2954;
_LL2950: if(*(( void**) _temp2948) ==  Cyc_Bool_tok){ _LL2955: _temp2954=((
struct Cyc_Bool_tok_struct*) _temp2948)->f1; goto _LL2951;} else{ goto _LL2952;}
_LL2952: goto _LL2953; _LL2951: yyzzz= _temp2954; goto _LL2949; _LL2953:( int)
_throw(( void*)& Cyc_yyfail_Bool_tok); _LL2949:;} yyzzz;}); _temp2947;});
_temp2946;}); _temp2945;})); _temp2936.targs=({ struct Cyc_List_List* yyzzz;{
void* _temp2937= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp2943; _LL2939: if(*(( void**) _temp2937) ==  Cyc_TypeList_tok){
_LL2944: _temp2943=(( struct Cyc_TypeList_tok_struct*) _temp2937)->f1; goto
_LL2940;} else{ goto _LL2941;} _LL2941: goto _LL2942; _LL2940: yyzzz= _temp2943;
goto _LL2938; _LL2942:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2938:;}
yyzzz;}); _temp2936.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2936;});
_temp2935;}); _temp2934;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2933;});
_temp2932;}); break; case 112: _LL2931: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2965=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2965[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2966; _temp2966.tag=
Cyc_TypeSpecifier_tok; _temp2966.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp2967=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp2967[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp2968; _temp2968.tag= Cyc_Absyn_TunionFieldType;
_temp2968.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp2969; _temp2969.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp2978=( struct
Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp2978[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp2979; _temp2979.tag=
Cyc_Absyn_UnknownTunionfield; _temp2979.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2980; _temp2980.tunion_name=({ struct _tuple1* yyzzz;{ void* _temp2997=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct
_tuple1* _temp3003; _LL2999: if(*(( void**) _temp2997) ==  Cyc_QualId_tok){
_LL3004: _temp3003=(( struct Cyc_QualId_tok_struct*) _temp2997)->f1; goto
_LL3000;} else{ goto _LL3001;} _LL3001: goto _LL3002; _LL3000: yyzzz= _temp3003;
goto _LL2998; _LL3002:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2998:;}
yyzzz;}); _temp2980.field_name=({ struct _tuple1* yyzzz;{ void* _temp2989= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple1*
_temp2995; _LL2991: if(*(( void**) _temp2989) ==  Cyc_QualId_tok){ _LL2996:
_temp2995=(( struct Cyc_QualId_tok_struct*) _temp2989)->f1; goto _LL2992;} else{
goto _LL2993;} _LL2993: goto _LL2994; _LL2992: yyzzz= _temp2995; goto _LL2990;
_LL2994:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2990:;} yyzzz;});
_temp2980.is_xtunion=({ int yyzzz;{ void* _temp2981= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; int _temp2987;
_LL2983: if(*(( void**) _temp2981) ==  Cyc_Bool_tok){ _LL2988: _temp2987=((
struct Cyc_Bool_tok_struct*) _temp2981)->f1; goto _LL2984;} else{ goto _LL2985;}
_LL2985: goto _LL2986; _LL2984: yyzzz= _temp2987; goto _LL2982; _LL2986:( int)
_throw(( void*)& Cyc_yyfail_Bool_tok); _LL2982:;} yyzzz;}); _temp2980;});
_temp2979;}); _temp2978;})); _temp2969.targs=({ struct Cyc_List_List* yyzzz;{
void* _temp2970= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp2976; _LL2972: if(*(( void**) _temp2970) ==  Cyc_TypeList_tok){
_LL2977: _temp2976=(( struct Cyc_TypeList_tok_struct*) _temp2970)->f1; goto
_LL2973;} else{ goto _LL2974;} _LL2974: goto _LL2975; _LL2973: yyzzz= _temp2976;
goto _LL2971; _LL2975:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2971:;}
yyzzz;}); _temp2969;}); _temp2968;}); _temp2967;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2966;}); _temp2965;}); break; case 113: _LL2964: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp3006=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp3006[ 0]=({ struct Cyc_Bool_tok_struct
_temp3007; _temp3007.tag= Cyc_Bool_tok; _temp3007.f1= 0; _temp3007;}); _temp3006;});
break; case 114: _LL3005: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3009=(
struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp3009[ 0]=({ struct Cyc_Bool_tok_struct _temp3010; _temp3010.tag= Cyc_Bool_tok;
_temp3010.f1= 1; _temp3010;}); _temp3009;}); break; case 115: _LL3008: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3012=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3012[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3013; _temp3013.tag= Cyc_TunionFieldList_tok;
_temp3013.f1=({ struct Cyc_List_List* _temp3014=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3014->hd=( void*)({ struct Cyc_Absyn_Tunionfield*
yyzzz;{ void* _temp3015= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Tunionfield* _temp3021; _LL3017: if(*(( void**)
_temp3015) ==  Cyc_TunionField_tok){ _LL3022: _temp3021=(( struct Cyc_TunionField_tok_struct*)
_temp3015)->f1; goto _LL3018;} else{ goto _LL3019;} _LL3019: goto _LL3020;
_LL3018: yyzzz= _temp3021; goto _LL3016; _LL3020:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL3016:;} yyzzz;}); _temp3014->tl= 0; _temp3014;}); _temp3013;}); _temp3012;});
break; case 116: _LL3011: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3024=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3024[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3025; _temp3025.tag= Cyc_TunionFieldList_tok; _temp3025.f1=({ struct Cyc_List_List*
_temp3026=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3026->hd=( void*)({ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp3027=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Tunionfield*
_temp3033; _LL3029: if(*(( void**) _temp3027) ==  Cyc_TunionField_tok){ _LL3034:
_temp3033=(( struct Cyc_TunionField_tok_struct*) _temp3027)->f1; goto _LL3030;}
else{ goto _LL3031;} _LL3031: goto _LL3032; _LL3030: yyzzz= _temp3033; goto
_LL3028; _LL3032:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL3028:;}
yyzzz;}); _temp3026->tl= 0; _temp3026;}); _temp3025;}); _temp3024;}); break;
case 117: _LL3023: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3036=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3036[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3037; _temp3037.tag= Cyc_TunionFieldList_tok; _temp3037.f1=({ struct Cyc_List_List*
_temp3038=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3038->hd=( void*)({ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp3047=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Tunionfield*
_temp3053; _LL3049: if(*(( void**) _temp3047) ==  Cyc_TunionField_tok){ _LL3054:
_temp3053=(( struct Cyc_TunionField_tok_struct*) _temp3047)->f1; goto _LL3050;}
else{ goto _LL3051;} _LL3051: goto _LL3052; _LL3050: yyzzz= _temp3053; goto
_LL3048; _LL3052:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL3048:;}
yyzzz;}); _temp3038->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp3039= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3045; _LL3041: if(*(( void**) _temp3039) ==  Cyc_TunionFieldList_tok){
_LL3046: _temp3045=(( struct Cyc_TunionFieldList_tok_struct*) _temp3039)->f1;
goto _LL3042;} else{ goto _LL3043;} _LL3043: goto _LL3044; _LL3042: yyzzz=
_temp3045; goto _LL3040; _LL3044:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL3040:;} yyzzz;}); _temp3038;}); _temp3037;}); _temp3036;}); break; case 118:
_LL3035: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3056=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3056[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3057; _temp3057.tag=
Cyc_TunionFieldList_tok; _temp3057.f1=({ struct Cyc_List_List* _temp3058=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3058->hd=(
void*)({ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp3067= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Tunionfield*
_temp3073; _LL3069: if(*(( void**) _temp3067) ==  Cyc_TunionField_tok){ _LL3074:
_temp3073=(( struct Cyc_TunionField_tok_struct*) _temp3067)->f1; goto _LL3070;}
else{ goto _LL3071;} _LL3071: goto _LL3072; _LL3070: yyzzz= _temp3073; goto
_LL3068; _LL3072:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL3068:;}
yyzzz;}); _temp3058->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp3059= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3065; _LL3061: if(*(( void**) _temp3059) ==  Cyc_TunionFieldList_tok){
_LL3066: _temp3065=(( struct Cyc_TunionFieldList_tok_struct*) _temp3059)->f1;
goto _LL3062;} else{ goto _LL3063;} _LL3063: goto _LL3064; _LL3062: yyzzz=
_temp3065; goto _LL3060; _LL3064:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL3060:;} yyzzz;}); _temp3058;}); _temp3057;}); _temp3056;}); break; case 119:
_LL3055: yyval=( void*)({ struct Cyc_Scope_tok_struct* _temp3076=( struct Cyc_Scope_tok_struct*)
_cycalloc( sizeof( struct Cyc_Scope_tok_struct)); _temp3076[ 0]=({ struct Cyc_Scope_tok_struct
_temp3077; _temp3077.tag= Cyc_Scope_tok; _temp3077.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp3077;}); _temp3076;}); break; case 120: _LL3075: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp3079=( struct Cyc_Scope_tok_struct*) _cycalloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp3079[ 0]=({ struct Cyc_Scope_tok_struct
_temp3080; _temp3080.tag= Cyc_Scope_tok; _temp3080.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp3080;}); _temp3079;}); break; case 121: _LL3078: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp3082=( struct Cyc_Scope_tok_struct*) _cycalloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp3082[ 0]=({ struct Cyc_Scope_tok_struct
_temp3083; _temp3083.tag= Cyc_Scope_tok; _temp3083.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp3083;}); _temp3082;}); break; case 122: _LL3081: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp3085=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3085[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3086; _temp3086.tag= Cyc_TunionField_tok;
_temp3086.f1=({ struct Cyc_Absyn_Tunionfield* _temp3087=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3087->name=({ struct
_tuple1* yyzzz;{ void* _temp3096= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple1* _temp3102; _LL3098: if(*(( void**) _temp3096) == 
Cyc_QualId_tok){ _LL3103: _temp3102=(( struct Cyc_QualId_tok_struct*) _temp3096)->f1;
goto _LL3099;} else{ goto _LL3100;} _LL3100: goto _LL3101; _LL3099: yyzzz=
_temp3102; goto _LL3097; _LL3101:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL3097:;} yyzzz;}); _temp3087->tvs= 0; _temp3087->typs= 0; _temp3087->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp3087->sc=( void*)({ void* yyzzz;{ void* _temp3088= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp3094;
_LL3090: if(*(( void**) _temp3088) ==  Cyc_Scope_tok){ _LL3095: _temp3094=( void*)((
struct Cyc_Scope_tok_struct*) _temp3088)->f1; goto _LL3091;} else{ goto _LL3092;}
_LL3092: goto _LL3093; _LL3091: yyzzz= _temp3094; goto _LL3089; _LL3093:( int)
_throw(( void*)& Cyc_yyfail_Scope_tok); _LL3089:;} yyzzz;}); _temp3087;});
_temp3086;}); _temp3085;}); break; case 123: _LL3084: { struct Cyc_List_List*
_temp3105=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* yyzzz;{ void* _temp3134= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct Cyc_List_List* _temp3140; _LL3136: if(*((
void**) _temp3134) ==  Cyc_ParamDeclList_tok){ _LL3141: _temp3140=(( struct Cyc_ParamDeclList_tok_struct*)
_temp3134)->f1; goto _LL3137;} else{ goto _LL3138;} _LL3138: goto _LL3139;
_LL3137: yyzzz= _temp3140; goto _LL3135; _LL3139:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL3135:;} yyzzz;}))); struct Cyc_List_List* _temp3106=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp3126= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp3132; _LL3128: if(*(( void**) _temp3126) ==  Cyc_TypeList_tok){ _LL3133:
_temp3132=(( struct Cyc_TypeList_tok_struct*) _temp3126)->f1; goto _LL3129;}
else{ goto _LL3130;} _LL3130: goto _LL3131; _LL3129: yyzzz= _temp3132; goto
_LL3127; _LL3131:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3127:;}
yyzzz;})); yyval=( void*)({ struct Cyc_TunionField_tok_struct* _temp3107=(
struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3107[ 0]=({ struct Cyc_TunionField_tok_struct _temp3108; _temp3108.tag= Cyc_TunionField_tok;
_temp3108.f1=({ struct Cyc_Absyn_Tunionfield* _temp3109=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3109->name=({ struct
_tuple1* yyzzz;{ void* _temp3118= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp3124; _LL3120: if(*(( void**)
_temp3118) ==  Cyc_QualId_tok){ _LL3125: _temp3124=(( struct Cyc_QualId_tok_struct*)
_temp3118)->f1; goto _LL3121;} else{ goto _LL3122;} _LL3122: goto _LL3123;
_LL3121: yyzzz= _temp3124; goto _LL3119; _LL3123:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL3119:;} yyzzz;}); _temp3109->tvs= _temp3106; _temp3109->typs= _temp3105;
_temp3109->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp3109->sc=(
void*)({ void* yyzzz;{ void* _temp3110= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]; void* _temp3116; _LL3112: if(*(( void**) _temp3110)
==  Cyc_Scope_tok){ _LL3117: _temp3116=( void*)(( struct Cyc_Scope_tok_struct*)
_temp3110)->f1; goto _LL3113;} else{ goto _LL3114;} _LL3114: goto _LL3115;
_LL3113: yyzzz= _temp3116; goto _LL3111; _LL3115:( int) _throw(( void*)& Cyc_yyfail_Scope_tok);
_LL3111:;} yyzzz;}); _temp3109;}); _temp3108;}); _temp3107;}); break;} case 124:
_LL3104: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 125: _LL3142: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3144=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3144[ 0]=({ struct Cyc_Declarator_tok_struct _temp3145; _temp3145.tag= Cyc_Declarator_tok;
_temp3145.f1=({ struct Cyc_Parse_Declarator* _temp3146=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3146->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3163= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declarator* _temp3169; _LL3165: if(*(( void**)
_temp3163) ==  Cyc_Declarator_tok){ _LL3170: _temp3169=(( struct Cyc_Declarator_tok_struct*)
_temp3163)->f1; goto _LL3166;} else{ goto _LL3167;} _LL3167: goto _LL3168;
_LL3166: yyzzz= _temp3169; goto _LL3164; _LL3168:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3164:;} yyzzz;})->id; _temp3146->tms= Cyc_List_imp_append(({ struct Cyc_List_List*
yyzzz;{ void* _temp3147= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp3153; _LL3149: if(*(( void**)
_temp3147) ==  Cyc_TypeModifierList_tok){ _LL3154: _temp3153=(( struct Cyc_TypeModifierList_tok_struct*)
_temp3147)->f1; goto _LL3150;} else{ goto _LL3151;} _LL3151: goto _LL3152;
_LL3150: yyzzz= _temp3153; goto _LL3148; _LL3152:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL3148:;} yyzzz;}),({ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp3155=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declarator*
_temp3161; _LL3157: if(*(( void**) _temp3155) ==  Cyc_Declarator_tok){ _LL3162:
_temp3161=(( struct Cyc_Declarator_tok_struct*) _temp3155)->f1; goto _LL3158;}
else{ goto _LL3159;} _LL3159: goto _LL3160; _LL3158: yyzzz= _temp3161; goto
_LL3156; _LL3160:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL3156:;}
yyzzz;})->tms); _temp3146;}); _temp3145;}); _temp3144;}); break; case 126:
_LL3143: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3172=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3172[ 0]=({ struct Cyc_Declarator_tok_struct _temp3173; _temp3173.tag= Cyc_Declarator_tok;
_temp3173.f1=({ struct Cyc_Parse_Declarator* _temp3174=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3174->id=({ struct
_tuple1* yyzzz;{ void* _temp3175= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple1* _temp3181; _LL3177: if(*(( void**) _temp3175) == 
Cyc_QualId_tok){ _LL3182: _temp3181=(( struct Cyc_QualId_tok_struct*) _temp3175)->f1;
goto _LL3178;} else{ goto _LL3179;} _LL3179: goto _LL3180; _LL3178: yyzzz=
_temp3181; goto _LL3176; _LL3180:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL3176:;} yyzzz;}); _temp3174->tms= 0; _temp3174;}); _temp3173;}); _temp3172;});
break; case 127: _LL3171: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; break; case 128: _LL3183: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3185=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3185[ 0]=({ struct Cyc_Declarator_tok_struct _temp3186; _temp3186.tag= Cyc_Declarator_tok;
_temp3186.f1=({ struct Cyc_Parse_Declarator* _temp3187=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3187->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3197= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Parse_Declarator* _temp3203; _LL3199: if(*((
void**) _temp3197) ==  Cyc_Declarator_tok){ _LL3204: _temp3203=(( struct Cyc_Declarator_tok_struct*)
_temp3197)->f1; goto _LL3200;} else{ goto _LL3201;} _LL3201: goto _LL3202;
_LL3200: yyzzz= _temp3203; goto _LL3198; _LL3202:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3198:;} yyzzz;})->id; _temp3187->tms=({ struct Cyc_List_List* _temp3188=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3188->hd=(
void*)(( void*) Cyc_Absyn_Carray_mod); _temp3188->tl=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3189= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Parse_Declarator* _temp3195; _LL3191: if(*((
void**) _temp3189) ==  Cyc_Declarator_tok){ _LL3196: _temp3195=(( struct Cyc_Declarator_tok_struct*)
_temp3189)->f1; goto _LL3192;} else{ goto _LL3193;} _LL3193: goto _LL3194;
_LL3192: yyzzz= _temp3195; goto _LL3190; _LL3194:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3190:;} yyzzz;})->tms; _temp3188;}); _temp3187;}); _temp3186;}); _temp3185;});
break; case 129: _LL3184: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3206=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3206[ 0]=({ struct Cyc_Declarator_tok_struct _temp3207; _temp3207.tag= Cyc_Declarator_tok;
_temp3207.f1=({ struct Cyc_Parse_Declarator* _temp3208=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3208->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3228= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3234; _LL3230: if(*((
void**) _temp3228) ==  Cyc_Declarator_tok){ _LL3235: _temp3234=(( struct Cyc_Declarator_tok_struct*)
_temp3228)->f1; goto _LL3231;} else{ goto _LL3232;} _LL3232: goto _LL3233;
_LL3231: yyzzz= _temp3234; goto _LL3229; _LL3233:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3229:;} yyzzz;})->id; _temp3208->tms=({ struct Cyc_List_List* _temp3209=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3209->hd=(
void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3218=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3218[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3219; _temp3219.tag= Cyc_Absyn_ConstArray_mod;
_temp3219.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp3220= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp3226; _LL3222: if(*(( void**) _temp3220) ==  Cyc_Exp_tok){ _LL3227:
_temp3226=(( struct Cyc_Exp_tok_struct*) _temp3220)->f1; goto _LL3223;} else{
goto _LL3224;} _LL3224: goto _LL3225; _LL3223: yyzzz= _temp3226; goto _LL3221;
_LL3225:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL3221:;} yyzzz;});
_temp3219;}); _temp3218;})); _temp3209->tl=({ struct Cyc_Parse_Declarator* yyzzz;{
void* _temp3210= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Parse_Declarator* _temp3216; _LL3212: if(*(( void**) _temp3210) == 
Cyc_Declarator_tok){ _LL3217: _temp3216=(( struct Cyc_Declarator_tok_struct*)
_temp3210)->f1; goto _LL3213;} else{ goto _LL3214;} _LL3214: goto _LL3215;
_LL3213: yyzzz= _temp3216; goto _LL3211; _LL3215:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3211:;} yyzzz;})->tms; _temp3209;}); _temp3208;}); _temp3207;}); _temp3206;});
break; case 130: _LL3205: { struct _tuple16 _temp3247; struct Cyc_List_List*
_temp3248; struct Cyc_Core_Opt* _temp3250; struct Cyc_Absyn_VarargInfo*
_temp3252; int _temp3254; struct Cyc_List_List* _temp3256; struct _tuple16*
_temp3245=({ struct _tuple16* yyzzz;{ void* _temp3237= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple16*
_temp3243; _LL3239: if(*(( void**) _temp3237) ==  Cyc_ParamDeclListBool_tok){
_LL3244: _temp3243=(( struct Cyc_ParamDeclListBool_tok_struct*) _temp3237)->f1;
goto _LL3240;} else{ goto _LL3241;} _LL3241: goto _LL3242; _LL3240: yyzzz=
_temp3243; goto _LL3238; _LL3242:( int) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL3238:;} yyzzz;}); _temp3247=* _temp3245; _LL3257: _temp3256= _temp3247.f1;
goto _LL3255; _LL3255: _temp3254= _temp3247.f2; goto _LL3253; _LL3253: _temp3252=
_temp3247.f3; goto _LL3251; _LL3251: _temp3250= _temp3247.f4; goto _LL3249;
_LL3249: _temp3248= _temp3247.f5; goto _LL3246; _LL3246: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp3258=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3258[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3259; _temp3259.tag= Cyc_Declarator_tok;
_temp3259.f1=({ struct Cyc_Parse_Declarator* _temp3260=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3260->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3274= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3280; _LL3276: if(*((
void**) _temp3274) ==  Cyc_Declarator_tok){ _LL3281: _temp3280=(( struct Cyc_Declarator_tok_struct*)
_temp3274)->f1; goto _LL3277;} else{ goto _LL3278;} _LL3278: goto _LL3279;
_LL3277: yyzzz= _temp3280; goto _LL3275; _LL3279:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3275:;} yyzzz;})->id; _temp3260->tms=({ struct Cyc_List_List* _temp3261=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3261->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3270=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3270[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3271; _temp3271.tag= Cyc_Absyn_Function_mod;
_temp3271.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3272=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3272[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3273; _temp3273.tag= Cyc_Absyn_WithTypes;
_temp3273.f1= _temp3256; _temp3273.f2= _temp3254; _temp3273.f3= _temp3252;
_temp3273.f4= _temp3250; _temp3273.f5= _temp3248; _temp3273;}); _temp3272;}));
_temp3271;}); _temp3270;})); _temp3261->tl=({ struct Cyc_Parse_Declarator* yyzzz;{
void* _temp3262= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Parse_Declarator* _temp3268; _LL3264: if(*(( void**) _temp3262) == 
Cyc_Declarator_tok){ _LL3269: _temp3268=(( struct Cyc_Declarator_tok_struct*)
_temp3262)->f1; goto _LL3265;} else{ goto _LL3266;} _LL3266: goto _LL3267;
_LL3265: yyzzz= _temp3268; goto _LL3263; _LL3267:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3263:;} yyzzz;})->tms; _temp3261;}); _temp3260;}); _temp3259;}); _temp3258;});
break;} case 131: _LL3236: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3283=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3283[ 0]=({ struct Cyc_Declarator_tok_struct _temp3284; _temp3284.tag= Cyc_Declarator_tok;
_temp3284.f1=({ struct Cyc_Parse_Declarator* _temp3285=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3285->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3315= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct Cyc_Parse_Declarator* _temp3321; _LL3317: if(*((
void**) _temp3315) ==  Cyc_Declarator_tok){ _LL3322: _temp3321=(( struct Cyc_Declarator_tok_struct*)
_temp3315)->f1; goto _LL3318;} else{ goto _LL3319;} _LL3319: goto _LL3320;
_LL3318: yyzzz= _temp3321; goto _LL3316; _LL3320:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3316:;} yyzzz;})->id; _temp3285->tms=({ struct Cyc_List_List* _temp3286=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3286->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3295=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3295[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3296; _temp3296.tag= Cyc_Absyn_Function_mod;
_temp3296.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3297=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3297[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3298; _temp3298.tag= Cyc_Absyn_WithTypes;
_temp3298.f1= 0; _temp3298.f2= 0; _temp3298.f3= 0; _temp3298.f4=({ struct Cyc_Core_Opt*
yyzzz;{ void* _temp3299= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Core_Opt* _temp3305; _LL3301: if(*(( void**)
_temp3299) ==  Cyc_TypeOpt_tok){ _LL3306: _temp3305=(( struct Cyc_TypeOpt_tok_struct*)
_temp3299)->f1; goto _LL3302;} else{ goto _LL3303;} _LL3303: goto _LL3304;
_LL3302: yyzzz= _temp3305; goto _LL3300; _LL3304:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok);
_LL3300:;} yyzzz;}); _temp3298.f5=({ struct Cyc_List_List* yyzzz;{ void*
_temp3307= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp3313; _LL3309: if(*(( void**) _temp3307) ==  Cyc_Rgnorder_tok){
_LL3314: _temp3313=(( struct Cyc_Rgnorder_tok_struct*) _temp3307)->f1; goto
_LL3310;} else{ goto _LL3311;} _LL3311: goto _LL3312; _LL3310: yyzzz= _temp3313;
goto _LL3308; _LL3312:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3308:;}
yyzzz;}); _temp3298;}); _temp3297;})); _temp3296;}); _temp3295;})); _temp3286->tl=({
struct Cyc_Parse_Declarator* yyzzz;{ void* _temp3287= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Parse_Declarator*
_temp3293; _LL3289: if(*(( void**) _temp3287) ==  Cyc_Declarator_tok){ _LL3294:
_temp3293=(( struct Cyc_Declarator_tok_struct*) _temp3287)->f1; goto _LL3290;}
else{ goto _LL3291;} _LL3291: goto _LL3292; _LL3290: yyzzz= _temp3293; goto
_LL3288; _LL3292:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL3288:;}
yyzzz;})->tms; _temp3286;}); _temp3285;}); _temp3284;}); _temp3283;}); break;
case 132: _LL3282: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3324=(
struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3324[ 0]=({ struct Cyc_Declarator_tok_struct _temp3325; _temp3325.tag= Cyc_Declarator_tok;
_temp3325.f1=({ struct Cyc_Parse_Declarator* _temp3326=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3326->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3348= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3354; _LL3350: if(*((
void**) _temp3348) ==  Cyc_Declarator_tok){ _LL3355: _temp3354=(( struct Cyc_Declarator_tok_struct*)
_temp3348)->f1; goto _LL3351;} else{ goto _LL3352;} _LL3352: goto _LL3353;
_LL3351: yyzzz= _temp3354; goto _LL3349; _LL3353:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3349:;} yyzzz;})->id; _temp3326->tms=({ struct Cyc_List_List* _temp3327=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3327->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3336=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3336[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3337; _temp3337.tag= Cyc_Absyn_Function_mod;
_temp3337.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp3338=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3338[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp3339; _temp3339.tag= Cyc_Absyn_NoTypes;
_temp3339.f1=({ struct Cyc_List_List* yyzzz;{ void* _temp3340= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp3346; _LL3342: if(*(( void**) _temp3340) ==  Cyc_IdList_tok){ _LL3347:
_temp3346=(( struct Cyc_IdList_tok_struct*) _temp3340)->f1; goto _LL3343;} else{
goto _LL3344;} _LL3344: goto _LL3345; _LL3343: yyzzz= _temp3346; goto _LL3341;
_LL3345:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL3341:;} yyzzz;});
_temp3339.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line); _temp3339;}); _temp3338;})); _temp3337;});
_temp3336;})); _temp3327->tl=({ struct Cyc_Parse_Declarator* yyzzz;{ void*
_temp3328= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Parse_Declarator* _temp3334; _LL3330: if(*(( void**) _temp3328) == 
Cyc_Declarator_tok){ _LL3335: _temp3334=(( struct Cyc_Declarator_tok_struct*)
_temp3328)->f1; goto _LL3331;} else{ goto _LL3332;} _LL3332: goto _LL3333;
_LL3331: yyzzz= _temp3334; goto _LL3329; _LL3333:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3329:;} yyzzz;})->tms; _temp3327;}); _temp3326;}); _temp3325;}); _temp3324;});
break; case 133: _LL3323: { struct Cyc_List_List* _temp3357=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp3380= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp3386; _LL3382: if(*(( void**) _temp3380) ==  Cyc_TypeList_tok){ _LL3387:
_temp3386=(( struct Cyc_TypeList_tok_struct*) _temp3380)->f1; goto _LL3383;}
else{ goto _LL3384;} _LL3384: goto _LL3385; _LL3383: yyzzz= _temp3386; goto
_LL3381; _LL3385:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3381:;}
yyzzz;}))); yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3358=(
struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3358[ 0]=({ struct Cyc_Declarator_tok_struct _temp3359; _temp3359.tag= Cyc_Declarator_tok;
_temp3359.f1=({ struct Cyc_Parse_Declarator* _temp3360=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3360->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3372= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3378; _LL3374: if(*((
void**) _temp3372) ==  Cyc_Declarator_tok){ _LL3379: _temp3378=(( struct Cyc_Declarator_tok_struct*)
_temp3372)->f1; goto _LL3375;} else{ goto _LL3376;} _LL3376: goto _LL3377;
_LL3375: yyzzz= _temp3378; goto _LL3373; _LL3377:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3373:;} yyzzz;})->id; _temp3360->tms=({ struct Cyc_List_List* _temp3361=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3361->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3370=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3370[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3371; _temp3371.tag= Cyc_Absyn_TypeParams_mod;
_temp3371.f1= _temp3357; _temp3371.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp3371.f3=
0; _temp3371;}); _temp3370;})); _temp3361->tl=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3362= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3368; _LL3364: if(*((
void**) _temp3362) ==  Cyc_Declarator_tok){ _LL3369: _temp3368=(( struct Cyc_Declarator_tok_struct*)
_temp3362)->f1; goto _LL3365;} else{ goto _LL3366;} _LL3366: goto _LL3367;
_LL3365: yyzzz= _temp3368; goto _LL3363; _LL3367:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3363:;} yyzzz;})->tms; _temp3361;}); _temp3360;}); _temp3359;}); _temp3358;});
break;} case 134: _LL3356:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;{ struct Cyc_List_List*
_temp3389=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp3412= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp3418; _LL3414: if(*(( void**) _temp3412) ==  Cyc_TypeList_tok){ _LL3419:
_temp3418=(( struct Cyc_TypeList_tok_struct*) _temp3412)->f1; goto _LL3415;}
else{ goto _LL3416;} _LL3416: goto _LL3417; _LL3415: yyzzz= _temp3418; goto
_LL3413; _LL3417:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3413:;}
yyzzz;}))); yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp3390=(
struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3390[ 0]=({ struct Cyc_Declarator_tok_struct _temp3391; _temp3391.tag= Cyc_Declarator_tok;
_temp3391.f1=({ struct Cyc_Parse_Declarator* _temp3392=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3392->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3404= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3410; _LL3406: if(*((
void**) _temp3404) ==  Cyc_Declarator_tok){ _LL3411: _temp3410=(( struct Cyc_Declarator_tok_struct*)
_temp3404)->f1; goto _LL3407;} else{ goto _LL3408;} _LL3408: goto _LL3409;
_LL3407: yyzzz= _temp3410; goto _LL3405; _LL3409:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3405:;} yyzzz;})->id; _temp3392->tms=({ struct Cyc_List_List* _temp3393=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3393->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3402=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3402[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3403; _temp3403.tag= Cyc_Absyn_TypeParams_mod;
_temp3403.f1= _temp3389; _temp3403.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp3403.f3=
0; _temp3403;}); _temp3402;})); _temp3393->tl=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3394= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3400; _LL3396: if(*((
void**) _temp3394) ==  Cyc_Declarator_tok){ _LL3401: _temp3400=(( struct Cyc_Declarator_tok_struct*)
_temp3394)->f1; goto _LL3397;} else{ goto _LL3398;} _LL3398: goto _LL3399;
_LL3397: yyzzz= _temp3400; goto _LL3395; _LL3399:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3395:;} yyzzz;})->tms; _temp3393;}); _temp3392;}); _temp3391;}); _temp3390;});
break;} case 135: _LL3388: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3421=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3421[ 0]=({ struct Cyc_Declarator_tok_struct _temp3422; _temp3422.tag= Cyc_Declarator_tok;
_temp3422.f1=({ struct Cyc_Parse_Declarator* _temp3423=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3423->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3443= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Parse_Declarator* _temp3449; _LL3445: if(*((
void**) _temp3443) ==  Cyc_Declarator_tok){ _LL3450: _temp3449=(( struct Cyc_Declarator_tok_struct*)
_temp3443)->f1; goto _LL3446;} else{ goto _LL3447;} _LL3447: goto _LL3448;
_LL3446: yyzzz= _temp3449; goto _LL3444; _LL3448:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3444:;} yyzzz;})->id; _temp3423->tms=({ struct Cyc_List_List* _temp3424=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3424->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp3433=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3433[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3434; _temp3434.tag= Cyc_Absyn_Attributes_mod;
_temp3434.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp3434.f2=({ struct Cyc_List_List* yyzzz;{ void*
_temp3435= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp3441; _LL3437: if(*(( void**) _temp3435) ==  Cyc_AttributeList_tok){
_LL3442: _temp3441=(( struct Cyc_AttributeList_tok_struct*) _temp3435)->f1; goto
_LL3438;} else{ goto _LL3439;} _LL3439: goto _LL3440; _LL3438: yyzzz= _temp3441;
goto _LL3436; _LL3440:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL3436:;} yyzzz;}); _temp3434;}); _temp3433;})); _temp3424->tl=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3425= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Parse_Declarator* _temp3431; _LL3427: if(*((
void**) _temp3425) ==  Cyc_Declarator_tok){ _LL3432: _temp3431=(( struct Cyc_Declarator_tok_struct*)
_temp3425)->f1; goto _LL3428;} else{ goto _LL3429;} _LL3429: goto _LL3430;
_LL3428: yyzzz= _temp3431; goto _LL3426; _LL3430:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3426:;} yyzzz;})->tms; _temp3424;}); _temp3423;}); _temp3422;}); _temp3421;});
break; case 136: _LL3420: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp3452=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3452[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3453; _temp3453.tag= Cyc_Declarator_tok;
_temp3453.f1=({ struct Cyc_Parse_Declarator* _temp3454=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3454->id=({ struct
_tuple1* yyzzz;{ void* _temp3455= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple1* _temp3461; _LL3457: if(*(( void**) _temp3455) == 
Cyc_QualId_tok){ _LL3462: _temp3461=(( struct Cyc_QualId_tok_struct*) _temp3455)->f1;
goto _LL3458;} else{ goto _LL3459;} _LL3459: goto _LL3460; _LL3458: yyzzz=
_temp3461; goto _LL3456; _LL3460:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL3456:;} yyzzz;}); _temp3454->tms= 0; _temp3454;}); _temp3453;}); _temp3452;});
break; case 137: _LL3451: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3464=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3464[ 0]=({ struct Cyc_Declarator_tok_struct _temp3465; _temp3465.tag= Cyc_Declarator_tok;
_temp3465.f1=({ struct Cyc_Parse_Declarator* _temp3466=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp3466->id= Cyc_Absyn_exn_name;
_temp3466->tms= 0; _temp3466;}); _temp3465;}); _temp3464;}); break; case 138:
_LL3463: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3468=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3468[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3469; _temp3469.tag=
Cyc_TypeModifierList_tok; _temp3469.f1=({ struct Cyc_List_List* _temp3470=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3470->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3479=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3479[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3480; _temp3480.tag= Cyc_Absyn_Pointer_mod;
_temp3480.f1=( void*)({ void* yyzzz;{ void* _temp3481= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void* _temp3487;
_LL3483: if(*(( void**) _temp3481) ==  Cyc_Pointer_Sort_tok){ _LL3488: _temp3487=(
void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp3481)->f1; goto _LL3484;}
else{ goto _LL3485;} _LL3485: goto _LL3486; _LL3484: yyzzz= _temp3487; goto
_LL3482; _LL3486:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL3482:;}
yyzzz;}); _temp3480.f2=( void*)({ void* yyzzz;{ void* _temp3489= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp3495;
_LL3491: if(*(( void**) _temp3489) ==  Cyc_Type_tok){ _LL3496: _temp3495=( void*)((
struct Cyc_Type_tok_struct*) _temp3489)->f1; goto _LL3492;} else{ goto _LL3493;}
_LL3493: goto _LL3494; _LL3492: yyzzz= _temp3495; goto _LL3490; _LL3494:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL3490:;} yyzzz;}); _temp3480.f3= Cyc_Absyn_empty_tqual();
_temp3480;}); _temp3479;})); _temp3470->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),({ struct Cyc_List_List*
yyzzz;{ void* _temp3471= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_List_List* _temp3477; _LL3473: if(*(( void**)
_temp3471) ==  Cyc_AttributeList_tok){ _LL3478: _temp3477=(( struct Cyc_AttributeList_tok_struct*)
_temp3471)->f1; goto _LL3474;} else{ goto _LL3475;} _LL3475: goto _LL3476;
_LL3474: yyzzz= _temp3477; goto _LL3472; _LL3476:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL3472:;} yyzzz;}), 0); _temp3470;}); _temp3469;}); _temp3468;}); break; case
139: _LL3467: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3498=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3498[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3499; _temp3499.tag=
Cyc_TypeModifierList_tok; _temp3499.f1=({ struct Cyc_List_List* _temp3500=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3500->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3509=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3509[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3510; _temp3510.tag= Cyc_Absyn_Pointer_mod;
_temp3510.f1=( void*)({ void* yyzzz;{ void* _temp3511= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; void* _temp3517;
_LL3513: if(*(( void**) _temp3511) ==  Cyc_Pointer_Sort_tok){ _LL3518: _temp3517=(
void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp3511)->f1; goto _LL3514;}
else{ goto _LL3515;} _LL3515: goto _LL3516; _LL3514: yyzzz= _temp3517; goto
_LL3512; _LL3516:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL3512:;}
yyzzz;}); _temp3510.f2=( void*)({ void* yyzzz;{ void* _temp3519= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void* _temp3525;
_LL3521: if(*(( void**) _temp3519) ==  Cyc_Type_tok){ _LL3526: _temp3525=( void*)((
struct Cyc_Type_tok_struct*) _temp3519)->f1; goto _LL3522;} else{ goto _LL3523;}
_LL3523: goto _LL3524; _LL3522: yyzzz= _temp3525; goto _LL3520; _LL3524:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL3520:;} yyzzz;}); _temp3510.f3=({
struct Cyc_Absyn_Tqual yyzzz;{ void* _temp3527= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Tqual
_temp3533; _LL3529: if(*(( void**) _temp3527) ==  Cyc_TypeQual_tok){ _LL3534:
_temp3533=(( struct Cyc_TypeQual_tok_struct*) _temp3527)->f1; goto _LL3530;}
else{ goto _LL3531;} _LL3531: goto _LL3532; _LL3530: yyzzz= _temp3533; goto
_LL3528; _LL3532:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL3528:;}
yyzzz;}); _temp3510;}); _temp3509;})); _temp3500->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line),({ struct Cyc_List_List* yyzzz;{ void* _temp3501=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp3507; _LL3503: if(*(( void**) _temp3501) ==  Cyc_AttributeList_tok){
_LL3508: _temp3507=(( struct Cyc_AttributeList_tok_struct*) _temp3501)->f1; goto
_LL3504;} else{ goto _LL3505;} _LL3505: goto _LL3506; _LL3504: yyzzz= _temp3507;
goto _LL3502; _LL3506:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL3502:;} yyzzz;}), 0); _temp3500;}); _temp3499;}); _temp3498;}); break; case
140: _LL3497: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3536=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3536[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3537; _temp3537.tag=
Cyc_TypeModifierList_tok; _temp3537.f1=({ struct Cyc_List_List* _temp3538=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3538->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3555=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3555[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3556; _temp3556.tag= Cyc_Absyn_Pointer_mod;
_temp3556.f1=( void*)({ void* yyzzz;{ void* _temp3557= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; void* _temp3563;
_LL3559: if(*(( void**) _temp3557) ==  Cyc_Pointer_Sort_tok){ _LL3564: _temp3563=(
void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp3557)->f1; goto _LL3560;}
else{ goto _LL3561;} _LL3561: goto _LL3562; _LL3560: yyzzz= _temp3563; goto
_LL3558; _LL3562:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL3558:;}
yyzzz;}); _temp3556.f2=( void*)({ void* yyzzz;{ void* _temp3565= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void* _temp3571;
_LL3567: if(*(( void**) _temp3565) ==  Cyc_Type_tok){ _LL3572: _temp3571=( void*)((
struct Cyc_Type_tok_struct*) _temp3565)->f1; goto _LL3568;} else{ goto _LL3569;}
_LL3569: goto _LL3570; _LL3568: yyzzz= _temp3571; goto _LL3566; _LL3570:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL3566:;} yyzzz;}); _temp3556.f3= Cyc_Absyn_empty_tqual();
_temp3556;}); _temp3555;})); _temp3538->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp3539= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp3545; _LL3541: if(*(( void**) _temp3539) ==  Cyc_AttributeList_tok){
_LL3546: _temp3545=(( struct Cyc_AttributeList_tok_struct*) _temp3539)->f1; goto
_LL3542;} else{ goto _LL3543;} _LL3543: goto _LL3544; _LL3542: yyzzz= _temp3545;
goto _LL3540; _LL3544:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL3540:;} yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp3547= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3553; _LL3549: if(*(( void**) _temp3547) ==  Cyc_TypeModifierList_tok){
_LL3554: _temp3553=(( struct Cyc_TypeModifierList_tok_struct*) _temp3547)->f1;
goto _LL3550;} else{ goto _LL3551;} _LL3551: goto _LL3552; _LL3550: yyzzz=
_temp3553; goto _LL3548; _LL3552:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL3548:;} yyzzz;})); _temp3538;}); _temp3537;}); _temp3536;}); break; case 141:
_LL3535: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3574=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3574[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3575; _temp3575.tag=
Cyc_TypeModifierList_tok; _temp3575.f1=({ struct Cyc_List_List* _temp3576=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp3576->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3593=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3593[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3594; _temp3594.tag= Cyc_Absyn_Pointer_mod;
_temp3594.f1=( void*)({ void* yyzzz;{ void* _temp3595= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; void* _temp3601;
_LL3597: if(*(( void**) _temp3595) ==  Cyc_Pointer_Sort_tok){ _LL3602: _temp3601=(
void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp3595)->f1; goto _LL3598;}
else{ goto _LL3599;} _LL3599: goto _LL3600; _LL3598: yyzzz= _temp3601; goto
_LL3596; _LL3600:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL3596:;}
yyzzz;}); _temp3594.f2=( void*)({ void* yyzzz;{ void* _temp3603= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; void* _temp3609;
_LL3605: if(*(( void**) _temp3603) ==  Cyc_Type_tok){ _LL3610: _temp3609=( void*)((
struct Cyc_Type_tok_struct*) _temp3603)->f1; goto _LL3606;} else{ goto _LL3607;}
_LL3607: goto _LL3608; _LL3606: yyzzz= _temp3609; goto _LL3604; _LL3608:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL3604:;} yyzzz;}); _temp3594.f3=({
struct Cyc_Absyn_Tqual yyzzz;{ void* _temp3611= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Tqual
_temp3617; _LL3613: if(*(( void**) _temp3611) ==  Cyc_TypeQual_tok){ _LL3618:
_temp3617=(( struct Cyc_TypeQual_tok_struct*) _temp3611)->f1; goto _LL3614;}
else{ goto _LL3615;} _LL3615: goto _LL3616; _LL3614: yyzzz= _temp3617; goto
_LL3612; _LL3616:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL3612:;}
yyzzz;}); _temp3594;}); _temp3593;})); _temp3576->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).last_line),({ struct Cyc_List_List* yyzzz;{ void* _temp3577=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp3583; _LL3579: if(*(( void**) _temp3577) ==  Cyc_AttributeList_tok){
_LL3584: _temp3583=(( struct Cyc_AttributeList_tok_struct*) _temp3577)->f1; goto
_LL3580;} else{ goto _LL3581;} _LL3581: goto _LL3582; _LL3580: yyzzz= _temp3583;
goto _LL3578; _LL3582:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL3578:;} yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp3585= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3591; _LL3587: if(*(( void**) _temp3585) ==  Cyc_TypeModifierList_tok){
_LL3592: _temp3591=(( struct Cyc_TypeModifierList_tok_struct*) _temp3585)->f1;
goto _LL3588;} else{ goto _LL3589;} _LL3589: goto _LL3590; _LL3588: yyzzz=
_temp3591; goto _LL3586; _LL3590:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL3586:;} yyzzz;})); _temp3576;}); _temp3575;}); _temp3574;}); break; case 142:
_LL3573: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3620=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3620[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3621; _temp3621.tag=
Cyc_Pointer_Sort_tok; _temp3621.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3622=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3622[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3623; _temp3623.tag=
Cyc_Absyn_Nullable_ps; _temp3623.f1= Cyc_Absyn_exp_unsigned_one; _temp3623;});
_temp3622;})); _temp3621;}); _temp3620;}); break; case 143: _LL3619: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3625=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3625[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3626; _temp3626.tag= Cyc_Pointer_Sort_tok;
_temp3626.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3627=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3627[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3628; _temp3628.tag= Cyc_Absyn_NonNullable_ps; _temp3628.f1= Cyc_Absyn_exp_unsigned_one;
_temp3628;}); _temp3627;})); _temp3626;}); _temp3625;}); break; case 144:
_LL3624: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3630=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3630[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3631; _temp3631.tag=
Cyc_Pointer_Sort_tok; _temp3631.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3632=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3632[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3633; _temp3633.tag=
Cyc_Absyn_Nullable_ps; _temp3633.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp3634= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Exp* _temp3640; _LL3636: if(*(( void**) _temp3634) ==  Cyc_Exp_tok){
_LL3641: _temp3640=(( struct Cyc_Exp_tok_struct*) _temp3634)->f1; goto _LL3637;}
else{ goto _LL3638;} _LL3638: goto _LL3639; _LL3637: yyzzz= _temp3640; goto
_LL3635; _LL3639:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL3635:;} yyzzz;});
_temp3633;}); _temp3632;})); _temp3631;}); _temp3630;}); break; case 145:
_LL3629: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3643=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3643[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3644; _temp3644.tag=
Cyc_Pointer_Sort_tok; _temp3644.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3645=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3645[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3646; _temp3646.tag= Cyc_Absyn_NonNullable_ps; _temp3646.f1=({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp3647= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Absyn_Exp* _temp3653; _LL3649: if(*(( void**)
_temp3647) ==  Cyc_Exp_tok){ _LL3654: _temp3653=(( struct Cyc_Exp_tok_struct*)
_temp3647)->f1; goto _LL3650;} else{ goto _LL3651;} _LL3651: goto _LL3652;
_LL3650: yyzzz= _temp3653; goto _LL3648; _LL3652:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL3648:;} yyzzz;}); _temp3646;}); _temp3645;})); _temp3644;}); _temp3643;});
break; case 146: _LL3642: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3656=( struct Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3656[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3657; _temp3657.tag=
Cyc_Pointer_Sort_tok; _temp3657.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp3657;}); _temp3656;}); break; case 147: _LL3655: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp3659=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp3659[ 0]=({ struct Cyc_Type_tok_struct
_temp3660; _temp3660.tag= Cyc_Type_tok; _temp3660.f1=( void*) Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3661=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3661->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp3661;}),
0); _temp3660;}); _temp3659;}); break; case 148: _LL3658: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 149: _LL3662:
Cyc_Parse_set_vartyp_kind(({ void* yyzzz;{ void* _temp3664= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp3670; _LL3666:
if(*(( void**) _temp3664) ==  Cyc_Type_tok){ _LL3671: _temp3670=( void*)((
struct Cyc_Type_tok_struct*) _temp3664)->f1; goto _LL3667;} else{ goto _LL3668;}
_LL3668: goto _LL3669; _LL3667: yyzzz= _temp3670; goto _LL3665; _LL3669:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL3665:;} yyzzz;}),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
150: _LL3663: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3673=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp3673[
0]=({ struct Cyc_Type_tok_struct _temp3674; _temp3674.tag= Cyc_Type_tok;
_temp3674.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp3675=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3675->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp3675;}), 0); _temp3674;}); _temp3673;});
break; case 151: _LL3672: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL3676: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp3678=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp3678[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3679; _temp3679.tag= Cyc_TypeQual_tok; _temp3679.f1= Cyc_Absyn_combine_tqual(({
struct Cyc_Absyn_Tqual yyzzz;{ void* _temp3680= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Tqual
_temp3686; _LL3682: if(*(( void**) _temp3680) ==  Cyc_TypeQual_tok){ _LL3687:
_temp3686=(( struct Cyc_TypeQual_tok_struct*) _temp3680)->f1; goto _LL3683;}
else{ goto _LL3684;} _LL3684: goto _LL3685; _LL3683: yyzzz= _temp3686; goto
_LL3681; _LL3685:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL3681:;}
yyzzz;}),({ struct Cyc_Absyn_Tqual yyzzz;{ void* _temp3688= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Tqual
_temp3694; _LL3690: if(*(( void**) _temp3688) ==  Cyc_TypeQual_tok){ _LL3695:
_temp3694=(( struct Cyc_TypeQual_tok_struct*) _temp3688)->f1; goto _LL3691;}
else{ goto _LL3692;} _LL3692: goto _LL3693; _LL3691: yyzzz= _temp3694; goto
_LL3689; _LL3693:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL3689:;}
yyzzz;})); _temp3679;}); _temp3678;}); break; case 153: _LL3677: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp3697=( struct Cyc_ParamDeclListBool_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3697[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3698; _temp3698.tag= Cyc_ParamDeclListBool_tok;
_temp3698.f1=({ struct _tuple16* _temp3699=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp3699->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp3716= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp3722; _LL3718: if(*(( void**) _temp3716) ==  Cyc_ParamDeclList_tok){
_LL3723: _temp3722=(( struct Cyc_ParamDeclList_tok_struct*) _temp3716)->f1; goto
_LL3719;} else{ goto _LL3720;} _LL3720: goto _LL3721; _LL3719: yyzzz= _temp3722;
goto _LL3717; _LL3721:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL3717:;} yyzzz;})); _temp3699->f2= 0; _temp3699->f3= 0; _temp3699->f4=({
struct Cyc_Core_Opt* yyzzz;{ void* _temp3708= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Core_Opt*
_temp3714; _LL3710: if(*(( void**) _temp3708) ==  Cyc_TypeOpt_tok){ _LL3715:
_temp3714=(( struct Cyc_TypeOpt_tok_struct*) _temp3708)->f1; goto _LL3711;}
else{ goto _LL3712;} _LL3712: goto _LL3713; _LL3711: yyzzz= _temp3714; goto
_LL3709; _LL3713:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL3709:;}
yyzzz;}); _temp3699->f5=({ struct Cyc_List_List* yyzzz;{ void* _temp3700= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3706; _LL3702: if(*(( void**) _temp3700) ==  Cyc_Rgnorder_tok){ _LL3707:
_temp3706=(( struct Cyc_Rgnorder_tok_struct*) _temp3700)->f1; goto _LL3703;}
else{ goto _LL3704;} _LL3704: goto _LL3705; _LL3703: yyzzz= _temp3706; goto
_LL3701; _LL3705:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3701:;}
yyzzz;}); _temp3699;}); _temp3698;}); _temp3697;}); break; case 154: _LL3696:
yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp3725=( struct Cyc_ParamDeclListBool_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3725[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3726; _temp3726.tag= Cyc_ParamDeclListBool_tok;
_temp3726.f1=({ struct _tuple16* _temp3727=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp3727->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp3744= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_List_List*
_temp3750; _LL3746: if(*(( void**) _temp3744) ==  Cyc_ParamDeclList_tok){
_LL3751: _temp3750=(( struct Cyc_ParamDeclList_tok_struct*) _temp3744)->f1; goto
_LL3747;} else{ goto _LL3748;} _LL3748: goto _LL3749; _LL3747: yyzzz= _temp3750;
goto _LL3745; _LL3749:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL3745:;} yyzzz;})); _temp3727->f2= 1; _temp3727->f3= 0; _temp3727->f4=({
struct Cyc_Core_Opt* yyzzz;{ void* _temp3736= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Core_Opt*
_temp3742; _LL3738: if(*(( void**) _temp3736) ==  Cyc_TypeOpt_tok){ _LL3743:
_temp3742=(( struct Cyc_TypeOpt_tok_struct*) _temp3736)->f1; goto _LL3739;}
else{ goto _LL3740;} _LL3740: goto _LL3741; _LL3739: yyzzz= _temp3742; goto
_LL3737; _LL3741:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL3737:;}
yyzzz;}); _temp3727->f5=({ struct Cyc_List_List* yyzzz;{ void* _temp3728= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3734; _LL3730: if(*(( void**) _temp3728) ==  Cyc_Rgnorder_tok){ _LL3735:
_temp3734=(( struct Cyc_Rgnorder_tok_struct*) _temp3728)->f1; goto _LL3731;}
else{ goto _LL3732;} _LL3732: goto _LL3733; _LL3731: yyzzz= _temp3734; goto
_LL3729; _LL3733:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3729:;}
yyzzz;}); _temp3727;}); _temp3726;}); _temp3725;}); break; case 155: _LL3724: {
struct _tuple2 _temp3763; void* _temp3764; struct Cyc_Absyn_Tqual _temp3766;
struct Cyc_Core_Opt* _temp3768; struct _tuple2* _temp3761=({ struct _tuple2*
yyzzz;{ void* _temp3753= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tuple2* _temp3759; _LL3755: if(*(( void**)
_temp3753) ==  Cyc_ParamDecl_tok){ _LL3760: _temp3759=(( struct Cyc_ParamDecl_tok_struct*)
_temp3753)->f1; goto _LL3756;} else{ goto _LL3757;} _LL3757: goto _LL3758;
_LL3756: yyzzz= _temp3759; goto _LL3754; _LL3758:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL3754:;} yyzzz;}); _temp3763=* _temp3761; _LL3769: _temp3768= _temp3763.f1;
goto _LL3767; _LL3767: _temp3766= _temp3763.f2; goto _LL3765; _LL3765: _temp3764=
_temp3763.f3; goto _LL3762; _LL3762: { struct Cyc_Absyn_VarargInfo* _temp3770=({
struct Cyc_Absyn_VarargInfo* _temp3790=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp3790->name= _temp3768; _temp3790->tq=
_temp3766; _temp3790->type=( void*) _temp3764; _temp3790->inject=({ int yyzzz;{
void* _temp3791= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
int _temp3797; _LL3793: if(*(( void**) _temp3791) ==  Cyc_Bool_tok){ _LL3798:
_temp3797=(( struct Cyc_Bool_tok_struct*) _temp3791)->f1; goto _LL3794;} else{
goto _LL3795;} _LL3795: goto _LL3796; _LL3794: yyzzz= _temp3797; goto _LL3792;
_LL3796:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL3792:;} yyzzz;});
_temp3790;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3771=( struct Cyc_ParamDeclListBool_tok_struct*) _cycalloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp3771[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp3772; _temp3772.tag= Cyc_ParamDeclListBool_tok; _temp3772.f1=({ struct
_tuple16* _temp3773=( struct _tuple16*) _cycalloc( sizeof( struct _tuple16));
_temp3773->f1= 0; _temp3773->f2= 0; _temp3773->f3= _temp3770; _temp3773->f4=({
struct Cyc_Core_Opt* yyzzz;{ void* _temp3782= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Core_Opt*
_temp3788; _LL3784: if(*(( void**) _temp3782) ==  Cyc_TypeOpt_tok){ _LL3789:
_temp3788=(( struct Cyc_TypeOpt_tok_struct*) _temp3782)->f1; goto _LL3785;}
else{ goto _LL3786;} _LL3786: goto _LL3787; _LL3785: yyzzz= _temp3788; goto
_LL3783; _LL3787:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL3783:;}
yyzzz;}); _temp3773->f5=({ struct Cyc_List_List* yyzzz;{ void* _temp3774= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3780; _LL3776: if(*(( void**) _temp3774) ==  Cyc_Rgnorder_tok){ _LL3781:
_temp3780=(( struct Cyc_Rgnorder_tok_struct*) _temp3774)->f1; goto _LL3777;}
else{ goto _LL3778;} _LL3778: goto _LL3779; _LL3777: yyzzz= _temp3780; goto
_LL3775; _LL3779:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3775:;}
yyzzz;}); _temp3773;}); _temp3772;}); _temp3771;}); break;}} case 156: _LL3752: {
struct _tuple2 _temp3810; void* _temp3811; struct Cyc_Absyn_Tqual _temp3813;
struct Cyc_Core_Opt* _temp3815; struct _tuple2* _temp3808=({ struct _tuple2*
yyzzz;{ void* _temp3800= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tuple2* _temp3806; _LL3802: if(*(( void**)
_temp3800) ==  Cyc_ParamDecl_tok){ _LL3807: _temp3806=(( struct Cyc_ParamDecl_tok_struct*)
_temp3800)->f1; goto _LL3803;} else{ goto _LL3804;} _LL3804: goto _LL3805;
_LL3803: yyzzz= _temp3806; goto _LL3801; _LL3805:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL3801:;} yyzzz;}); _temp3810=* _temp3808; _LL3816: _temp3815= _temp3810.f1;
goto _LL3814; _LL3814: _temp3813= _temp3810.f2; goto _LL3812; _LL3812: _temp3811=
_temp3810.f3; goto _LL3809; _LL3809: { struct Cyc_Absyn_VarargInfo* _temp3817=({
struct Cyc_Absyn_VarargInfo* _temp3845=( struct Cyc_Absyn_VarargInfo*) _cycalloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp3845->name= _temp3815; _temp3845->tq=
_temp3813; _temp3845->type=( void*) _temp3811; _temp3845->inject=({ int yyzzz;{
void* _temp3846= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
int _temp3852; _LL3848: if(*(( void**) _temp3846) ==  Cyc_Bool_tok){ _LL3853:
_temp3852=(( struct Cyc_Bool_tok_struct*) _temp3846)->f1; goto _LL3849;} else{
goto _LL3850;} _LL3850: goto _LL3851; _LL3849: yyzzz= _temp3852; goto _LL3847;
_LL3851:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL3847:;} yyzzz;});
_temp3845;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3818=( struct Cyc_ParamDeclListBool_tok_struct*) _cycalloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp3818[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp3819; _temp3819.tag= Cyc_ParamDeclListBool_tok; _temp3819.f1=({ struct
_tuple16* _temp3820=( struct _tuple16*) _cycalloc( sizeof( struct _tuple16));
_temp3820->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({
struct Cyc_List_List* yyzzz;{ void* _temp3837= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]; struct Cyc_List_List*
_temp3843; _LL3839: if(*(( void**) _temp3837) ==  Cyc_ParamDeclList_tok){
_LL3844: _temp3843=(( struct Cyc_ParamDeclList_tok_struct*) _temp3837)->f1; goto
_LL3840;} else{ goto _LL3841;} _LL3841: goto _LL3842; _LL3840: yyzzz= _temp3843;
goto _LL3838; _LL3842:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL3838:;} yyzzz;})); _temp3820->f2= 0; _temp3820->f3= _temp3817; _temp3820->f4=({
struct Cyc_Core_Opt* yyzzz;{ void* _temp3829= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Core_Opt*
_temp3835; _LL3831: if(*(( void**) _temp3829) ==  Cyc_TypeOpt_tok){ _LL3836:
_temp3835=(( struct Cyc_TypeOpt_tok_struct*) _temp3829)->f1; goto _LL3832;}
else{ goto _LL3833;} _LL3833: goto _LL3834; _LL3832: yyzzz= _temp3835; goto
_LL3830; _LL3834:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL3830:;}
yyzzz;}); _temp3820->f5=({ struct Cyc_List_List* yyzzz;{ void* _temp3821= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3827; _LL3823: if(*(( void**) _temp3821) ==  Cyc_Rgnorder_tok){ _LL3828:
_temp3827=(( struct Cyc_Rgnorder_tok_struct*) _temp3821)->f1; goto _LL3824;}
else{ goto _LL3825;} _LL3825: goto _LL3826; _LL3824: yyzzz= _temp3827; goto
_LL3822; _LL3826:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3822:;}
yyzzz;}); _temp3820;}); _temp3819;}); _temp3818;}); break;}} case 157: _LL3799:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3855=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp3855[ 0]=({ struct Cyc_Type_tok_struct
_temp3856; _temp3856.tag= Cyc_Type_tok; _temp3856.f1=( void*) Cyc_Parse_id2type(({
struct _tagged_arr yyzzz;{ void* _temp3857= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp3863; _LL3859: if(*(( void**)
_temp3857) ==  Cyc_String_tok){ _LL3864: _temp3863=(( struct Cyc_String_tok_struct*)
_temp3857)->f1; goto _LL3860;} else{ goto _LL3861;} _LL3861: goto _LL3862;
_LL3860: yyzzz= _temp3863; goto _LL3858; _LL3862:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL3858:;} yyzzz;}), Cyc_Absyn_empty_conref()); _temp3856;}); _temp3855;});
break; case 158: _LL3854: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3866=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp3866[ 0]=({ struct Cyc_Type_tok_struct _temp3867; _temp3867.tag= Cyc_Type_tok;
_temp3867.f1=( void*) Cyc_Parse_id2type(({ struct _tagged_arr yyzzz;{ void*
_temp3868= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct _tagged_arr _temp3874; _LL3870: if(*(( void**) _temp3868) ==  Cyc_String_tok){
_LL3875: _temp3874=(( struct Cyc_String_tok_struct*) _temp3868)->f1; goto
_LL3871;} else{ goto _LL3872;} _LL3872: goto _LL3873; _LL3871: yyzzz= _temp3874;
goto _LL3869; _LL3873:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3869:;}
yyzzz;}), Cyc_Absyn_new_conref(({ void* yyzzz;{ void* _temp3876= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp3882; _LL3878:
if(*(( void**) _temp3876) ==  Cyc_Kind_tok){ _LL3883: _temp3882=( void*)((
struct Cyc_Kind_tok_struct*) _temp3876)->f1; goto _LL3879;} else{ goto _LL3880;}
_LL3880: goto _LL3881; _LL3879: yyzzz= _temp3882; goto _LL3877; _LL3881:( int)
_throw(( void*)& Cyc_yyfail_Kind_tok); _LL3877:;} yyzzz;}))); _temp3867;});
_temp3866;}); break; case 159: _LL3865: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp3885=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp3885[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp3886; _temp3886.tag= Cyc_TypeOpt_tok;
_temp3886.f1= 0; _temp3886;}); _temp3885;}); break; case 160: _LL3884: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp3888=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp3888[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp3889; _temp3889.tag= Cyc_TypeOpt_tok; _temp3889.f1=({ struct Cyc_Core_Opt*
_temp3890=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp3890->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3891=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3891[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3892; _temp3892.tag= Cyc_Absyn_JoinEff;
_temp3892.f1=({ struct Cyc_List_List* yyzzz;{ void* _temp3893= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3899; _LL3895: if(*(( void**) _temp3893) ==  Cyc_TypeList_tok){ _LL3900:
_temp3899=(( struct Cyc_TypeList_tok_struct*) _temp3893)->f1; goto _LL3896;}
else{ goto _LL3897;} _LL3897: goto _LL3898; _LL3896: yyzzz= _temp3899; goto
_LL3894; _LL3898:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3894:;}
yyzzz;}); _temp3892;}); _temp3891;})); _temp3890;}); _temp3889;}); _temp3888;});
break; case 161: _LL3887: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp3902=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp3902[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp3903; _temp3903.tag= Cyc_Rgnorder_tok;
_temp3903.f1= 0; _temp3903;}); _temp3902;}); break; case 162: _LL3901: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL3904: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp3906=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp3906[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp3907; _temp3907.tag= Cyc_Rgnorder_tok; _temp3907.f1=({ struct Cyc_List_List*
_temp3908=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3908->hd=( void*)({ struct _tuple18* _temp3909=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp3909->f1= Cyc_Parse_id2type(({ struct
_tagged_arr yyzzz;{ void* _temp3918= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tagged_arr _temp3924; _LL3920: if(*(( void**)
_temp3918) ==  Cyc_String_tok){ _LL3925: _temp3924=(( struct Cyc_String_tok_struct*)
_temp3918)->f1; goto _LL3921;} else{ goto _LL3922;} _LL3922: goto _LL3923;
_LL3921: yyzzz= _temp3924; goto _LL3919; _LL3923:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL3919:;} yyzzz;}), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind));
_temp3909->f2= Cyc_Parse_id2type(({ struct _tagged_arr yyzzz;{ void* _temp3910=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp3916; _LL3912: if(*(( void**) _temp3910) ==  Cyc_String_tok){ _LL3917:
_temp3916=(( struct Cyc_String_tok_struct*) _temp3910)->f1; goto _LL3913;} else{
goto _LL3914;} _LL3914: goto _LL3915; _LL3913: yyzzz= _temp3916; goto _LL3911;
_LL3915:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3911:;} yyzzz;}), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp3909;}); _temp3908->tl= 0; _temp3908;});
_temp3907;}); _temp3906;}); break; case 164: _LL3905: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp3927=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp3927[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp3928; _temp3928.tag= Cyc_Rgnorder_tok; _temp3928.f1=({ struct Cyc_List_List*
_temp3929=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3929->hd=( void*)({ struct _tuple18* _temp3938=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp3938->f1= Cyc_Parse_id2type(({ struct
_tagged_arr yyzzz;{ void* _temp3947= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tagged_arr _temp3953; _LL3949: if(*(( void**)
_temp3947) ==  Cyc_String_tok){ _LL3954: _temp3953=(( struct Cyc_String_tok_struct*)
_temp3947)->f1; goto _LL3950;} else{ goto _LL3951;} _LL3951: goto _LL3952;
_LL3950: yyzzz= _temp3953; goto _LL3948; _LL3952:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL3948:;} yyzzz;}), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind));
_temp3938->f2= Cyc_Parse_id2type(({ struct _tagged_arr yyzzz;{ void* _temp3939=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct
_tagged_arr _temp3945; _LL3941: if(*(( void**) _temp3939) ==  Cyc_String_tok){
_LL3946: _temp3945=(( struct Cyc_String_tok_struct*) _temp3939)->f1; goto
_LL3942;} else{ goto _LL3943;} _LL3943: goto _LL3944; _LL3942: yyzzz= _temp3945;
goto _LL3940; _LL3944:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3940:;}
yyzzz;}), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind)); _temp3938;});
_temp3929->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp3930= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3936; _LL3932: if(*(( void**) _temp3930) ==  Cyc_Rgnorder_tok){ _LL3937:
_temp3936=(( struct Cyc_Rgnorder_tok_struct*) _temp3930)->f1; goto _LL3933;}
else{ goto _LL3934;} _LL3934: goto _LL3935; _LL3933: yyzzz= _temp3936; goto
_LL3931; _LL3935:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3931:;}
yyzzz;}); _temp3929;}); _temp3928;}); _temp3927;}); break; case 165: _LL3926:
yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3956=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp3956[ 0]=({ struct
Cyc_Bool_tok_struct _temp3957; _temp3957.tag= Cyc_Bool_tok; _temp3957.f1= 0;
_temp3957;}); _temp3956;}); break; case 166: _LL3955: { struct _tagged_arr
_temp3959=({ struct _tagged_arr yyzzz;{ void* _temp3962= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp3968; _LL3964: if(*(( void**) _temp3962) ==  Cyc_String_tok){ _LL3969:
_temp3968=(( struct Cyc_String_tok_struct*) _temp3962)->f1; goto _LL3965;} else{
goto _LL3966;} _LL3966: goto _LL3967; _LL3965: yyzzz= _temp3968; goto _LL3963;
_LL3967:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3963:;} yyzzz;}); if(
Cyc_Std_zstrcmp( _temp3959, _tag_arr("inject", sizeof( unsigned char), 7u)) != 
0){ Cyc_Parse_err( _tag_arr("missing type in function declaration", sizeof(
unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Bool_tok_struct* _temp3960=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp3960[ 0]=({ struct
Cyc_Bool_tok_struct _temp3961; _temp3961.tag= Cyc_Bool_tok; _temp3961.f1= 1;
_temp3961;}); _temp3960;}); break;} case 167: _LL3958: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168: _LL3970:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3972=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3972[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3973; _temp3973.tag= Cyc_TypeList_tok; _temp3973.f1= Cyc_List_imp_append(({
struct Cyc_List_List* yyzzz;{ void* _temp3974= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp3980; _LL3976: if(*(( void**) _temp3974) ==  Cyc_TypeList_tok){ _LL3981:
_temp3980=(( struct Cyc_TypeList_tok_struct*) _temp3974)->f1; goto _LL3977;}
else{ goto _LL3978;} _LL3978: goto _LL3979; _LL3977: yyzzz= _temp3980; goto
_LL3975; _LL3979:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3975:;}
yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp3982= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3988; _LL3984: if(*(( void**) _temp3982) ==  Cyc_TypeList_tok){ _LL3989:
_temp3988=(( struct Cyc_TypeList_tok_struct*) _temp3982)->f1; goto _LL3985;}
else{ goto _LL3986;} _LL3986: goto _LL3987; _LL3985: yyzzz= _temp3988; goto
_LL3983; _LL3987:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3983:;}
yyzzz;})); _temp3973;}); _temp3972;}); break; case 169: _LL3971: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3991=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3991[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3992; _temp3992.tag= Cyc_TypeList_tok; _temp3992.f1= 0; _temp3992;});
_temp3991;}); break; case 170: _LL3990: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 171:
_LL3993: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3995=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3995[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3996; _temp3996.tag= Cyc_TypeList_tok; _temp3996.f1=({ struct Cyc_List_List*
_temp3997=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3997->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp3998=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp3998[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp3999; _temp3999.tag= Cyc_Absyn_RgnsEff;
_temp3999.f1=( void*)({ void* yyzzz;{ void* _temp4000= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp4006;
_LL4002: if(*(( void**) _temp4000) ==  Cyc_Type_tok){ _LL4007: _temp4006=( void*)((
struct Cyc_Type_tok_struct*) _temp4000)->f1; goto _LL4003;} else{ goto _LL4004;}
_LL4004: goto _LL4005; _LL4003: yyzzz= _temp4006; goto _LL4001; _LL4005:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4001:;} yyzzz;}); _temp3999;});
_temp3998;})); _temp3997->tl= 0; _temp3997;}); _temp3996;}); _temp3995;});
break; case 172: _LL3994: Cyc_Parse_set_vartyp_kind(({ void* yyzzz;{ void*
_temp4009= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; void*
_temp4015; _LL4011: if(*(( void**) _temp4009) ==  Cyc_Type_tok){ _LL4016:
_temp4015=( void*)(( struct Cyc_Type_tok_struct*) _temp4009)->f1; goto _LL4012;}
else{ goto _LL4013;} _LL4013: goto _LL4014; _LL4012: yyzzz= _temp4015; goto
_LL4010; _LL4014:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4010:;} yyzzz;}),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4017=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4017[ 0]=({ struct Cyc_TypeList_tok_struct _temp4018; _temp4018.tag= Cyc_TypeList_tok;
_temp4018.f1=({ struct Cyc_List_List* _temp4019=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4019->hd=( void*)({ void* yyzzz;{
void* _temp4020= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp4026; _LL4022: if(*(( void**) _temp4020) ==  Cyc_Type_tok){ _LL4027:
_temp4026=( void*)(( struct Cyc_Type_tok_struct*) _temp4020)->f1; goto _LL4023;}
else{ goto _LL4024;} _LL4024: goto _LL4025; _LL4023: yyzzz= _temp4026; goto
_LL4021; _LL4025:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4021:;} yyzzz;});
_temp4019->tl= 0; _temp4019;}); _temp4018;}); _temp4017;}); break; case 173:
_LL4008: if( ! Cyc_Parse_set_vartyp_kind(({ void* yyzzz;{ void* _temp4029= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp4035; _LL4031:
if(*(( void**) _temp4029) ==  Cyc_Type_tok){ _LL4036: _temp4035=( void*)((
struct Cyc_Type_tok_struct*) _temp4029)->f1; goto _LL4032;} else{ goto _LL4033;}
_LL4033: goto _LL4034; _LL4032: yyzzz= _temp4035; goto _LL4030; _LL4034:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4030:;} yyzzz;}),( void*) Cyc_Absyn_RgnKind)){
Cyc_Parse_err( _tag_arr("expecting region kind", sizeof( unsigned char), 22u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4037=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4037[ 0]=({ struct Cyc_TypeList_tok_struct _temp4038; _temp4038.tag= Cyc_TypeList_tok;
_temp4038.f1=({ struct Cyc_List_List* _temp4039=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4039->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4040=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4040[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4041; _temp4041.tag= Cyc_Absyn_AccessEff;
_temp4041.f1=( void*)({ void* yyzzz;{ void* _temp4042= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp4048; _LL4044:
if(*(( void**) _temp4042) ==  Cyc_Type_tok){ _LL4049: _temp4048=( void*)((
struct Cyc_Type_tok_struct*) _temp4042)->f1; goto _LL4045;} else{ goto _LL4046;}
_LL4046: goto _LL4047; _LL4045: yyzzz= _temp4048; goto _LL4043; _LL4047:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4043:;} yyzzz;}); _temp4041;});
_temp4040;})); _temp4039->tl= 0; _temp4039;}); _temp4038;}); _temp4037;});
break; case 174: _LL4028: if( ! Cyc_Parse_set_vartyp_kind(({ void* yyzzz;{ void*
_temp4051= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
void* _temp4057; _LL4053: if(*(( void**) _temp4051) ==  Cyc_Type_tok){ _LL4058:
_temp4057=( void*)(( struct Cyc_Type_tok_struct*) _temp4051)->f1; goto _LL4054;}
else{ goto _LL4055;} _LL4055: goto _LL4056; _LL4054: yyzzz= _temp4057; goto
_LL4052; _LL4056:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4052:;} yyzzz;}),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp4059=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4059[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4060; _temp4060.tag= Cyc_TypeList_tok; _temp4060.f1=({ struct Cyc_List_List*
_temp4061=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4061->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4070=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4070[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4071; _temp4071.tag= Cyc_Absyn_AccessEff;
_temp4071.f1=( void*)({ void* yyzzz;{ void* _temp4072= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void* _temp4078;
_LL4074: if(*(( void**) _temp4072) ==  Cyc_Type_tok){ _LL4079: _temp4078=( void*)((
struct Cyc_Type_tok_struct*) _temp4072)->f1; goto _LL4075;} else{ goto _LL4076;}
_LL4076: goto _LL4077; _LL4075: yyzzz= _temp4078; goto _LL4073; _LL4077:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4073:;} yyzzz;}); _temp4071;});
_temp4070;})); _temp4061->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4062=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp4068; _LL4064: if(*(( void**) _temp4062) ==  Cyc_TypeList_tok){ _LL4069:
_temp4068=(( struct Cyc_TypeList_tok_struct*) _temp4062)->f1; goto _LL4065;}
else{ goto _LL4066;} _LL4066: goto _LL4067; _LL4065: yyzzz= _temp4068; goto
_LL4063; _LL4067:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4063:;}
yyzzz;}); _temp4061;}); _temp4060;}); _temp4059;}); break; case 175: _LL4050:
yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp4081=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4081[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4082; _temp4082.tag= Cyc_ParamDeclList_tok;
_temp4082.f1=({ struct Cyc_List_List* _temp4083=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4083->hd=( void*)({ struct
_tuple2* yyzzz;{ void* _temp4084= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple2* _temp4090; _LL4086: if(*(( void**) _temp4084) == 
Cyc_ParamDecl_tok){ _LL4091: _temp4090=(( struct Cyc_ParamDecl_tok_struct*)
_temp4084)->f1; goto _LL4087;} else{ goto _LL4088;} _LL4088: goto _LL4089;
_LL4087: yyzzz= _temp4090; goto _LL4085; _LL4089:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL4085:;} yyzzz;}); _temp4083->tl= 0; _temp4083;}); _temp4082;}); _temp4081;});
break; case 176: _LL4080: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct*
_temp4093=( struct Cyc_ParamDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4093[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4094; _temp4094.tag=
Cyc_ParamDeclList_tok; _temp4094.f1=({ struct Cyc_List_List* _temp4095=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp4095->hd=( void*)({
struct _tuple2* yyzzz;{ void* _temp4104= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tuple2* _temp4110; _LL4106: if(*(( void**)
_temp4104) ==  Cyc_ParamDecl_tok){ _LL4111: _temp4110=(( struct Cyc_ParamDecl_tok_struct*)
_temp4104)->f1; goto _LL4107;} else{ goto _LL4108;} _LL4108: goto _LL4109;
_LL4107: yyzzz= _temp4110; goto _LL4105; _LL4109:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL4105:;} yyzzz;}); _temp4095->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp4096= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp4102; _LL4098: if(*(( void**) _temp4096) ==  Cyc_ParamDeclList_tok){
_LL4103: _temp4102=(( struct Cyc_ParamDeclList_tok_struct*) _temp4096)->f1; goto
_LL4099;} else{ goto _LL4100;} _LL4100: goto _LL4101; _LL4099: yyzzz= _temp4102;
goto _LL4097; _LL4101:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL4097:;} yyzzz;}); _temp4095;}); _temp4094;}); _temp4093;}); break; case 177:
_LL4092: { struct _tuple17 _temp4123; struct Cyc_List_List* _temp4124; struct
Cyc_List_List* _temp4126; struct Cyc_Absyn_Tqual _temp4128; struct _tuple17*
_temp4121=({ struct _tuple17* yyzzz;{ void* _temp4113= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple17*
_temp4119; _LL4115: if(*(( void**) _temp4113) ==  Cyc_QualSpecList_tok){ _LL4120:
_temp4119=(( struct Cyc_QualSpecList_tok_struct*) _temp4113)->f1; goto _LL4116;}
else{ goto _LL4117;} _LL4117: goto _LL4118; _LL4116: yyzzz= _temp4119; goto
_LL4114; _LL4118:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4114:;}
yyzzz;}); _temp4123=* _temp4121; _LL4129: _temp4128= _temp4123.f1; goto _LL4127;
_LL4127: _temp4126= _temp4123.f2; goto _LL4125; _LL4125: _temp4124= _temp4123.f3;
goto _LL4122; _LL4122: { struct Cyc_Parse_Declarator _temp4140; struct Cyc_List_List*
_temp4141; struct _tuple1* _temp4143; struct Cyc_Parse_Declarator* _temp4138=({
struct Cyc_Parse_Declarator* yyzzz;{ void* _temp4130= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declarator*
_temp4136; _LL4132: if(*(( void**) _temp4130) ==  Cyc_Declarator_tok){ _LL4137:
_temp4136=(( struct Cyc_Declarator_tok_struct*) _temp4130)->f1; goto _LL4133;}
else{ goto _LL4134;} _LL4134: goto _LL4135; _LL4133: yyzzz= _temp4136; goto
_LL4131; _LL4135:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL4131:;}
yyzzz;}); _temp4140=* _temp4138; _LL4144: _temp4143= _temp4140.id; goto _LL4142;
_LL4142: _temp4141= _temp4140.tms; goto _LL4139; _LL4139: { void* _temp4145= Cyc_Parse_speclist2typ(
_temp4126, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)); struct Cyc_List_List* _temp4148;
struct Cyc_List_List* _temp4150; void* _temp4152; struct Cyc_Absyn_Tqual
_temp4154; struct _tuple6 _temp4146= Cyc_Parse_apply_tms( _temp4128, _temp4145,
_temp4124, _temp4141); _LL4155: _temp4154= _temp4146.f1; goto _LL4153; _LL4153:
_temp4152= _temp4146.f2; goto _LL4151; _LL4151: _temp4150= _temp4146.f3; goto
_LL4149; _LL4149: _temp4148= _temp4146.f4; goto _LL4147; _LL4147: if( _temp4150
!=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp4143)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp4156=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp4160=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp4160->v=( void*)(* _temp4143).f2; _temp4160;}); if( _temp4148 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp4157=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4157[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4158; _temp4158.tag= Cyc_ParamDecl_tok;
_temp4158.f1=({ struct _tuple2* _temp4159=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp4159->f1= _temp4156; _temp4159->f2= _temp4154; _temp4159->f3=
_temp4152; _temp4159;}); _temp4158;}); _temp4157;}); break;}}}} case 178:
_LL4112: { struct _tuple17 _temp4172; struct Cyc_List_List* _temp4173; struct
Cyc_List_List* _temp4175; struct Cyc_Absyn_Tqual _temp4177; struct _tuple17*
_temp4170=({ struct _tuple17* yyzzz;{ void* _temp4162= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp4168; _LL4164: if(*(( void**) _temp4162) ==  Cyc_QualSpecList_tok){ _LL4169:
_temp4168=(( struct Cyc_QualSpecList_tok_struct*) _temp4162)->f1; goto _LL4165;}
else{ goto _LL4166;} _LL4166: goto _LL4167; _LL4165: yyzzz= _temp4168; goto
_LL4163; _LL4167:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4163:;}
yyzzz;}); _temp4172=* _temp4170; _LL4178: _temp4177= _temp4172.f1; goto _LL4176;
_LL4176: _temp4175= _temp4172.f2; goto _LL4174; _LL4174: _temp4173= _temp4172.f3;
goto _LL4171; _LL4171: { void* _temp4179= Cyc_Parse_speclist2typ( _temp4175, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); if(
_temp4173 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp4180=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4180[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4181; _temp4181.tag= Cyc_ParamDecl_tok; _temp4181.f1=({
struct _tuple2* _temp4182=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp4182->f1= 0; _temp4182->f2= _temp4177; _temp4182->f3= _temp4179; _temp4182;});
_temp4181;}); _temp4180;}); break;}} case 179: _LL4161: { struct _tuple17
_temp4194; struct Cyc_List_List* _temp4195; struct Cyc_List_List* _temp4197;
struct Cyc_Absyn_Tqual _temp4199; struct _tuple17* _temp4192=({ struct _tuple17*
yyzzz;{ void* _temp4184= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tuple17* _temp4190; _LL4186: if(*(( void**)
_temp4184) ==  Cyc_QualSpecList_tok){ _LL4191: _temp4190=(( struct Cyc_QualSpecList_tok_struct*)
_temp4184)->f1; goto _LL4187;} else{ goto _LL4188;} _LL4188: goto _LL4189;
_LL4187: yyzzz= _temp4190; goto _LL4185; _LL4189:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4185:;} yyzzz;}); _temp4194=* _temp4192; _LL4200: _temp4199= _temp4194.f1;
goto _LL4198; _LL4198: _temp4197= _temp4194.f2; goto _LL4196; _LL4196: _temp4195=
_temp4194.f3; goto _LL4193; _LL4193: { void* _temp4201= Cyc_Parse_speclist2typ(
_temp4197, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)); struct Cyc_List_List* _temp4202=({
struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp4216= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Abstractdeclarator*
_temp4222; _LL4218: if(*(( void**) _temp4216) ==  Cyc_AbstractDeclarator_tok){
_LL4223: _temp4222=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp4216)->f1;
goto _LL4219;} else{ goto _LL4220;} _LL4220: goto _LL4221; _LL4219: yyzzz=
_temp4222; goto _LL4217; _LL4221:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4217:;} yyzzz;})->tms; struct Cyc_List_List* _temp4205; struct Cyc_List_List*
_temp4207; void* _temp4209; struct Cyc_Absyn_Tqual _temp4211; struct _tuple6
_temp4203= Cyc_Parse_apply_tms( _temp4199, _temp4201, _temp4195, _temp4202);
_LL4212: _temp4211= _temp4203.f1; goto _LL4210; _LL4210: _temp4209= _temp4203.f2;
goto _LL4208; _LL4208: _temp4207= _temp4203.f3; goto _LL4206; _LL4206: _temp4205=
_temp4203.f4; goto _LL4204; _LL4204: if( _temp4207 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type parameters on formal argument, ignoring", sizeof(
unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp4205 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp4213=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4213[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4214; _temp4214.tag= Cyc_ParamDecl_tok; _temp4214.f1=({
struct _tuple2* _temp4215=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp4215->f1= 0; _temp4215->f2= _temp4211; _temp4215->f3= _temp4209; _temp4215;});
_temp4214;}); _temp4213;}); break;}} case 180: _LL4183: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp4225=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp4225[ 0]=({ struct Cyc_IdList_tok_struct
_temp4226; _temp4226.tag= Cyc_IdList_tok; _temp4226.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{
void* _temp4227= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp4233; _LL4229: if(*(( void**) _temp4227) ==  Cyc_IdList_tok){
_LL4234: _temp4233=(( struct Cyc_IdList_tok_struct*) _temp4227)->f1; goto
_LL4230;} else{ goto _LL4231;} _LL4231: goto _LL4232; _LL4230: yyzzz= _temp4233;
goto _LL4228; _LL4232:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL4228:;}
yyzzz;})); _temp4226;}); _temp4225;}); break; case 181: _LL4224: yyval=( void*)({
struct Cyc_IdList_tok_struct* _temp4236=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4236[ 0]=({ struct Cyc_IdList_tok_struct
_temp4237; _temp4237.tag= Cyc_IdList_tok; _temp4237.f1=({ struct Cyc_List_List*
_temp4238=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4238->hd=( void*)({ struct _tagged_arr* _temp4239=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp4239[ 0]=({ struct _tagged_arr
yyzzz;{ void* _temp4240= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tagged_arr _temp4246; _LL4242: if(*(( void**) _temp4240)
==  Cyc_String_tok){ _LL4247: _temp4246=(( struct Cyc_String_tok_struct*)
_temp4240)->f1; goto _LL4243;} else{ goto _LL4244;} _LL4244: goto _LL4245;
_LL4243: yyzzz= _temp4246; goto _LL4241; _LL4245:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL4241:;} yyzzz;}); _temp4239;}); _temp4238->tl= 0; _temp4238;}); _temp4237;});
_temp4236;}); break; case 182: _LL4235: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp4249=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4249[ 0]=({ struct Cyc_IdList_tok_struct _temp4250; _temp4250.tag= Cyc_IdList_tok;
_temp4250.f1=({ struct Cyc_List_List* _temp4251=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4251->hd=( void*)({ struct
_tagged_arr* _temp4260=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp4260[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp4261= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp4267; _LL4263: if(*(( void**) _temp4261) ==  Cyc_String_tok){ _LL4268:
_temp4267=(( struct Cyc_String_tok_struct*) _temp4261)->f1; goto _LL4264;} else{
goto _LL4265;} _LL4265: goto _LL4266; _LL4264: yyzzz= _temp4267; goto _LL4262;
_LL4266:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4262:;} yyzzz;});
_temp4260;}); _temp4251->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4252=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp4258; _LL4254: if(*(( void**) _temp4252) ==  Cyc_IdList_tok){ _LL4259:
_temp4258=(( struct Cyc_IdList_tok_struct*) _temp4252)->f1; goto _LL4255;} else{
goto _LL4256;} _LL4256: goto _LL4257; _LL4255: yyzzz= _temp4258; goto _LL4253;
_LL4257:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL4253:;} yyzzz;});
_temp4251;}); _temp4250;}); _temp4249;}); break; case 183: _LL4248: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 184: _LL4269:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
185: _LL4270: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4272=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4272[ 0]=({ struct Cyc_Exp_tok_struct
_temp4273; _temp4273.tag= Cyc_Exp_tok; _temp4273.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4274=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4274[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4275; _temp4275.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4275.f1= 0; _temp4275.f2= 0; _temp4275;}); _temp4274;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp4273;}); _temp4272;}); break; case 186: _LL4271: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4277=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4277[ 0]=({ struct Cyc_Exp_tok_struct
_temp4278; _temp4278.tag= Cyc_Exp_tok; _temp4278.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4279=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4279[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4280; _temp4280.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4280.f1= 0; _temp4280.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp4281= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4287; _LL4283: if(*(( void**) _temp4281) ==  Cyc_InitializerList_tok){
_LL4288: _temp4287=(( struct Cyc_InitializerList_tok_struct*) _temp4281)->f1;
goto _LL4284;} else{ goto _LL4285;} _LL4285: goto _LL4286; _LL4284: yyzzz=
_temp4287; goto _LL4282; _LL4286:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4282:;} yyzzz;})); _temp4280;}); _temp4279;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp4278;}); _temp4277;}); break; case 187: _LL4276: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4290=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4290[ 0]=({ struct Cyc_Exp_tok_struct
_temp4291; _temp4291.tag= Cyc_Exp_tok; _temp4291.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4292=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4292[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4293; _temp4293.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4293.f1= 0; _temp4293.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp4294= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp4300; _LL4296: if(*(( void**) _temp4294) ==  Cyc_InitializerList_tok){
_LL4301: _temp4300=(( struct Cyc_InitializerList_tok_struct*) _temp4294)->f1;
goto _LL4297;} else{ goto _LL4298;} _LL4298: goto _LL4299; _LL4297: yyzzz=
_temp4300; goto _LL4295; _LL4299:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4295:;} yyzzz;})); _temp4293;}); _temp4292;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp4291;}); _temp4290;}); break; case 188: _LL4289: { struct Cyc_Absyn_Vardecl*
_temp4303= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4325=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp4325->f1=( void*) Cyc_Absyn_Loc_n;
_temp4325->f2=({ struct _tagged_arr* _temp4326=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp4326[ 0]=({ struct _tagged_arr yyzzz;{ void*
_temp4327= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)];
struct _tagged_arr _temp4333; _LL4329: if(*(( void**) _temp4327) ==  Cyc_String_tok){
_LL4334: _temp4333=(( struct Cyc_String_tok_struct*) _temp4327)->f1; goto
_LL4330;} else{ goto _LL4331;} _LL4331: goto _LL4332; _LL4330: yyzzz= _temp4333;
goto _LL4328; _LL4332:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4328:;}
yyzzz;}); _temp4326;}); _temp4325;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp4303->tq=({ struct Cyc_Absyn_Tqual _temp4304; _temp4304.q_const= 1;
_temp4304.q_volatile= 0; _temp4304.q_restrict= 1; _temp4304;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4305=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4305[ 0]=({ struct Cyc_Exp_tok_struct
_temp4306; _temp4306.tag= Cyc_Exp_tok; _temp4306.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp4307=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4307[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4308; _temp4308.tag= Cyc_Absyn_Comprehension_e;
_temp4308.f1= _temp4303; _temp4308.f2=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4309= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Absyn_Exp* _temp4315; _LL4311: if(*(( void**) _temp4309) ==  Cyc_Exp_tok){
_LL4316: _temp4315=(( struct Cyc_Exp_tok_struct*) _temp4309)->f1; goto _LL4312;}
else{ goto _LL4313;} _LL4313: goto _LL4314; _LL4312: yyzzz= _temp4315; goto
_LL4310; _LL4314:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4310:;} yyzzz;});
_temp4308.f3=({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp4317= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp4323; _LL4319: if(*(( void**) _temp4317) ==  Cyc_Exp_tok){ _LL4324:
_temp4323=(( struct Cyc_Exp_tok_struct*) _temp4317)->f1; goto _LL4320;} else{
goto _LL4321;} _LL4321: goto _LL4322; _LL4320: yyzzz= _temp4323; goto _LL4318;
_LL4322:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4318:;} yyzzz;});
_temp4308;}); _temp4307;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp4306;});
_temp4305;}); break;} case 189: _LL4302: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4336=( struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4336[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4337; _temp4337.tag= Cyc_InitializerList_tok; _temp4337.f1=({ struct Cyc_List_List*
_temp4338=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4338->hd=( void*)({ struct _tuple19* _temp4339=( struct _tuple19*)
_cycalloc( sizeof( struct _tuple19)); _temp4339->f1= 0; _temp4339->f2=({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp4340= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp* _temp4346; _LL4342: if(*(( void**)
_temp4340) ==  Cyc_Exp_tok){ _LL4347: _temp4346=(( struct Cyc_Exp_tok_struct*)
_temp4340)->f1; goto _LL4343;} else{ goto _LL4344;} _LL4344: goto _LL4345;
_LL4343: yyzzz= _temp4346; goto _LL4341; _LL4345:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL4341:;} yyzzz;}); _temp4339;}); _temp4338->tl= 0; _temp4338;}); _temp4337;});
_temp4336;}); break; case 190: _LL4335: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4349=( struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4349[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4350; _temp4350.tag= Cyc_InitializerList_tok; _temp4350.f1=({ struct Cyc_List_List*
_temp4351=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4351->hd=( void*)({ struct _tuple19* _temp4352=( struct _tuple19*)
_cycalloc( sizeof( struct _tuple19)); _temp4352->f1=({ struct Cyc_List_List*
yyzzz;{ void* _temp4361= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp4367; _LL4363: if(*(( void**)
_temp4361) ==  Cyc_DesignatorList_tok){ _LL4368: _temp4367=(( struct Cyc_DesignatorList_tok_struct*)
_temp4361)->f1; goto _LL4364;} else{ goto _LL4365;} _LL4365: goto _LL4366;
_LL4364: yyzzz= _temp4367; goto _LL4362; _LL4366:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4362:;} yyzzz;}); _temp4352->f2=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4353= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Exp* _temp4359; _LL4355: if(*(( void**) _temp4353) ==  Cyc_Exp_tok){
_LL4360: _temp4359=(( struct Cyc_Exp_tok_struct*) _temp4353)->f1; goto _LL4356;}
else{ goto _LL4357;} _LL4357: goto _LL4358; _LL4356: yyzzz= _temp4359; goto
_LL4354; _LL4358:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4354:;} yyzzz;});
_temp4352;}); _temp4351->tl= 0; _temp4351;}); _temp4350;}); _temp4349;}); break;
case 191: _LL4348: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4370=( struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4370[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4371; _temp4371.tag= Cyc_InitializerList_tok; _temp4371.f1=({ struct Cyc_List_List*
_temp4372=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4372->hd=( void*)({ struct _tuple19* _temp4381=( struct _tuple19*)
_cycalloc( sizeof( struct _tuple19)); _temp4381->f1= 0; _temp4381->f2=({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp4382= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp* _temp4388; _LL4384: if(*(( void**)
_temp4382) ==  Cyc_Exp_tok){ _LL4389: _temp4388=(( struct Cyc_Exp_tok_struct*)
_temp4382)->f1; goto _LL4385;} else{ goto _LL4386;} _LL4386: goto _LL4387;
_LL4385: yyzzz= _temp4388; goto _LL4383; _LL4387:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL4383:;} yyzzz;}); _temp4381;}); _temp4372->tl=({ struct Cyc_List_List* yyzzz;{
void* _temp4373= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp4379; _LL4375: if(*(( void**) _temp4373) ==  Cyc_InitializerList_tok){
_LL4380: _temp4379=(( struct Cyc_InitializerList_tok_struct*) _temp4373)->f1;
goto _LL4376;} else{ goto _LL4377;} _LL4377: goto _LL4378; _LL4376: yyzzz=
_temp4379; goto _LL4374; _LL4378:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4374:;} yyzzz;}); _temp4372;}); _temp4371;}); _temp4370;}); break; case 192:
_LL4369: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp4391=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4391[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4392; _temp4392.tag=
Cyc_InitializerList_tok; _temp4392.f1=({ struct Cyc_List_List* _temp4393=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp4393->hd=(
void*)({ struct _tuple19* _temp4402=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp4402->f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp4411= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4417; _LL4413: if(*(( void**) _temp4411) ==  Cyc_DesignatorList_tok){
_LL4418: _temp4417=(( struct Cyc_DesignatorList_tok_struct*) _temp4411)->f1;
goto _LL4414;} else{ goto _LL4415;} _LL4415: goto _LL4416; _LL4414: yyzzz=
_temp4417; goto _LL4412; _LL4416:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4412:;} yyzzz;}); _temp4402->f2=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4403= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Exp* _temp4409; _LL4405: if(*(( void**) _temp4403) ==  Cyc_Exp_tok){
_LL4410: _temp4409=(( struct Cyc_Exp_tok_struct*) _temp4403)->f1; goto _LL4406;}
else{ goto _LL4407;} _LL4407: goto _LL4408; _LL4406: yyzzz= _temp4409; goto
_LL4404; _LL4408:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4404:;} yyzzz;});
_temp4402;}); _temp4393->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4394=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp4400; _LL4396: if(*(( void**) _temp4394) ==  Cyc_InitializerList_tok){
_LL4401: _temp4400=(( struct Cyc_InitializerList_tok_struct*) _temp4394)->f1;
goto _LL4397;} else{ goto _LL4398;} _LL4398: goto _LL4399; _LL4397: yyzzz=
_temp4400; goto _LL4395; _LL4399:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4395:;} yyzzz;}); _temp4393;}); _temp4392;}); _temp4391;}); break; case 193:
_LL4390: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp4420=(
struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4420[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4421; _temp4421.tag=
Cyc_DesignatorList_tok; _temp4421.f1= Cyc_List_imp_rev(({ struct Cyc_List_List*
yyzzz;{ void* _temp4422= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp4428; _LL4424: if(*(( void**)
_temp4422) ==  Cyc_DesignatorList_tok){ _LL4429: _temp4428=(( struct Cyc_DesignatorList_tok_struct*)
_temp4422)->f1; goto _LL4425;} else{ goto _LL4426;} _LL4426: goto _LL4427;
_LL4425: yyzzz= _temp4428; goto _LL4423; _LL4427:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4423:;} yyzzz;})); _temp4421;}); _temp4420;}); break; case 194: _LL4419:
yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp4431=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4431[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4432; _temp4432.tag= Cyc_DesignatorList_tok;
_temp4432.f1=({ struct Cyc_List_List* _temp4433=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4433->hd=( void*)({ void* yyzzz;{
void* _temp4434= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp4440; _LL4436: if(*(( void**) _temp4434) ==  Cyc_Designator_tok){
_LL4441: _temp4440=( void*)(( struct Cyc_Designator_tok_struct*) _temp4434)->f1;
goto _LL4437;} else{ goto _LL4438;} _LL4438: goto _LL4439; _LL4437: yyzzz=
_temp4440; goto _LL4435; _LL4439:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL4435:;} yyzzz;}); _temp4433->tl= 0; _temp4433;}); _temp4432;}); _temp4431;});
break; case 195: _LL4430: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp4443=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4443[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4444; _temp4444.tag=
Cyc_DesignatorList_tok; _temp4444.f1=({ struct Cyc_List_List* _temp4445=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp4445->hd=( void*)({
void* yyzzz;{ void* _temp4454= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; void* _temp4460; _LL4456: if(*(( void**) _temp4454) ==  Cyc_Designator_tok){
_LL4461: _temp4460=( void*)(( struct Cyc_Designator_tok_struct*) _temp4454)->f1;
goto _LL4457;} else{ goto _LL4458;} _LL4458: goto _LL4459; _LL4457: yyzzz=
_temp4460; goto _LL4455; _LL4459:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL4455:;} yyzzz;}); _temp4445->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp4446= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4452; _LL4448: if(*(( void**) _temp4446) ==  Cyc_DesignatorList_tok){
_LL4453: _temp4452=(( struct Cyc_DesignatorList_tok_struct*) _temp4446)->f1;
goto _LL4449;} else{ goto _LL4450;} _LL4450: goto _LL4451; _LL4449: yyzzz=
_temp4452; goto _LL4447; _LL4451:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4447:;} yyzzz;}); _temp4445;}); _temp4444;}); _temp4443;}); break; case 196:
_LL4442: yyval=( void*)({ struct Cyc_Designator_tok_struct* _temp4463=( struct
Cyc_Designator_tok_struct*) _cycalloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4463[ 0]=({ struct Cyc_Designator_tok_struct _temp4464; _temp4464.tag= Cyc_Designator_tok;
_temp4464.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4465=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4465[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4466; _temp4466.tag=
Cyc_Absyn_ArrayElement; _temp4466.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4467= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Exp* _temp4473; _LL4469: if(*(( void**) _temp4467) ==  Cyc_Exp_tok){
_LL4474: _temp4473=(( struct Cyc_Exp_tok_struct*) _temp4467)->f1; goto _LL4470;}
else{ goto _LL4471;} _LL4471: goto _LL4472; _LL4470: yyzzz= _temp4473; goto
_LL4468; _LL4472:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4468:;} yyzzz;});
_temp4466;}); _temp4465;})); _temp4464;}); _temp4463;}); break; case 197:
_LL4462: yyval=( void*)({ struct Cyc_Designator_tok_struct* _temp4476=( struct
Cyc_Designator_tok_struct*) _cycalloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4476[ 0]=({ struct Cyc_Designator_tok_struct _temp4477; _temp4477.tag= Cyc_Designator_tok;
_temp4477.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4478=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4478[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4479; _temp4479.tag= Cyc_Absyn_FieldName;
_temp4479.f1=({ struct _tagged_arr* _temp4480=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp4480[ 0]=({ struct _tagged_arr yyzzz;{ void*
_temp4481= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
_tagged_arr _temp4487; _LL4483: if(*(( void**) _temp4481) ==  Cyc_String_tok){
_LL4488: _temp4487=(( struct Cyc_String_tok_struct*) _temp4481)->f1; goto
_LL4484;} else{ goto _LL4485;} _LL4485: goto _LL4486; _LL4484: yyzzz= _temp4487;
goto _LL4482; _LL4486:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4482:;}
yyzzz;}); _temp4480;}); _temp4479;}); _temp4478;})); _temp4477;}); _temp4476;});
break; case 198: _LL4475: { void* _temp4490= Cyc_Parse_speclist2typ((*({ struct
_tuple17* yyzzz;{ void* _temp4512= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple17* _temp4518; _LL4514: if(*(( void**) _temp4512)
==  Cyc_QualSpecList_tok){ _LL4519: _temp4518=(( struct Cyc_QualSpecList_tok_struct*)
_temp4512)->f1; goto _LL4515;} else{ goto _LL4516;} _LL4516: goto _LL4517;
_LL4515: yyzzz= _temp4518; goto _LL4513; _LL4517:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4513:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp4491=(*({ struct _tuple17* yyzzz;{ void* _temp4504= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp4510; _LL4506: if(*(( void**) _temp4504) ==  Cyc_QualSpecList_tok){ _LL4511:
_temp4510=(( struct Cyc_QualSpecList_tok_struct*) _temp4504)->f1; goto _LL4507;}
else{ goto _LL4508;} _LL4508: goto _LL4509; _LL4507: yyzzz= _temp4510; goto
_LL4505; _LL4509:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4505:;}
yyzzz;})).f3; if( _temp4491 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp4492=(*({ struct _tuple17* yyzzz;{ void* _temp4496= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp4502; _LL4498: if(*(( void**) _temp4496) ==  Cyc_QualSpecList_tok){ _LL4503:
_temp4502=(( struct Cyc_QualSpecList_tok_struct*) _temp4496)->f1; goto _LL4499;}
else{ goto _LL4500;} _LL4500: goto _LL4501; _LL4499: yyzzz= _temp4502; goto
_LL4497; _LL4501:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4497:;}
yyzzz;})).f1; yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4493=(
struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4493[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4494; _temp4494.tag= Cyc_ParamDecl_tok;
_temp4494.f1=({ struct _tuple2* _temp4495=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp4495->f1= 0; _temp4495->f2= _temp4492; _temp4495->f3=
_temp4490; _temp4495;}); _temp4494;}); _temp4493;}); break;}} case 199: _LL4489: {
void* _temp4521= Cyc_Parse_speclist2typ((*({ struct _tuple17* yyzzz;{ void*
_temp4553= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct _tuple17* _temp4559; _LL4555: if(*(( void**) _temp4553) ==  Cyc_QualSpecList_tok){
_LL4560: _temp4559=(( struct Cyc_QualSpecList_tok_struct*) _temp4553)->f1; goto
_LL4556;} else{ goto _LL4557;} _LL4557: goto _LL4558; _LL4556: yyzzz= _temp4559;
goto _LL4554; _LL4558:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4554:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp4522=(*({ struct _tuple17* yyzzz;{ void* _temp4545= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple17*
_temp4551; _LL4547: if(*(( void**) _temp4545) ==  Cyc_QualSpecList_tok){ _LL4552:
_temp4551=(( struct Cyc_QualSpecList_tok_struct*) _temp4545)->f1; goto _LL4548;}
else{ goto _LL4549;} _LL4549: goto _LL4550; _LL4548: yyzzz= _temp4551; goto
_LL4546; _LL4550:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4546:;}
yyzzz;})).f3; struct Cyc_Absyn_Tqual _temp4523=(*({ struct _tuple17* yyzzz;{
void* _temp4537= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct _tuple17* _temp4543; _LL4539: if(*(( void**) _temp4537) ==  Cyc_QualSpecList_tok){
_LL4544: _temp4543=(( struct Cyc_QualSpecList_tok_struct*) _temp4537)->f1; goto
_LL4540;} else{ goto _LL4541;} _LL4541: goto _LL4542; _LL4540: yyzzz= _temp4543;
goto _LL4538; _LL4542:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4538:;} yyzzz;})).f1; struct Cyc_List_List* _temp4524=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4529= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Abstractdeclarator* _temp4535; _LL4531: if(*((
void**) _temp4529) ==  Cyc_AbstractDeclarator_tok){ _LL4536: _temp4535=(( struct
Cyc_AbstractDeclarator_tok_struct*) _temp4529)->f1; goto _LL4532;} else{ goto
_LL4533;} _LL4533: goto _LL4534; _LL4532: yyzzz= _temp4535; goto _LL4530;
_LL4534:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4530:;}
yyzzz;})->tms; struct _tuple6 _temp4525= Cyc_Parse_apply_tms( _temp4523,
_temp4521, _temp4522, _temp4524); if( _temp4525.f3 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp4525.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp4526=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4526[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4527; _temp4527.tag= Cyc_ParamDecl_tok; _temp4527.f1=({
struct _tuple2* _temp4528=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp4528->f1= 0; _temp4528->f2= _temp4525.f1; _temp4528->f3= _temp4525.f2;
_temp4528;}); _temp4527;}); _temp4526;}); break;} case 200: _LL4520: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp4562=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp4562[ 0]=({ struct Cyc_Type_tok_struct
_temp4563; _temp4563.tag= Cyc_Type_tok; _temp4563.f1=( void*)(*({ struct _tuple2*
yyzzz;{ void* _temp4564= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple2* _temp4570; _LL4566: if(*(( void**) _temp4564) == 
Cyc_ParamDecl_tok){ _LL4571: _temp4570=(( struct Cyc_ParamDecl_tok_struct*)
_temp4564)->f1; goto _LL4567;} else{ goto _LL4568;} _LL4568: goto _LL4569;
_LL4567: yyzzz= _temp4570; goto _LL4565; _LL4569:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL4565:;} yyzzz;})).f3; _temp4563;}); _temp4562;}); break; case 201: _LL4561:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4573=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp4573[ 0]=({ struct Cyc_Type_tok_struct
_temp4574; _temp4574.tag= Cyc_Type_tok; _temp4574.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4575=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4575[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4576; _temp4576.tag= Cyc_Absyn_JoinEff; _temp4576.f1=
0; _temp4576;}); _temp4575;})); _temp4574;}); _temp4573;}); break; case 202:
_LL4572: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4578=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp4578[ 0]=({ struct Cyc_Type_tok_struct
_temp4579; _temp4579.tag= Cyc_Type_tok; _temp4579.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4580=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4580[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4581; _temp4581.tag= Cyc_Absyn_JoinEff; _temp4581.f1=({
struct Cyc_List_List* yyzzz;{ void* _temp4582= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4588; _LL4584: if(*(( void**) _temp4582) ==  Cyc_TypeList_tok){ _LL4589:
_temp4588=(( struct Cyc_TypeList_tok_struct*) _temp4582)->f1; goto _LL4585;}
else{ goto _LL4586;} _LL4586: goto _LL4587; _LL4585: yyzzz= _temp4588; goto
_LL4583; _LL4587:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4583:;}
yyzzz;}); _temp4581;}); _temp4580;})); _temp4579;}); _temp4578;}); break; case
203: _LL4577: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4591=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp4591[
0]=({ struct Cyc_Type_tok_struct _temp4592; _temp4592.tag= Cyc_Type_tok;
_temp4592.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp4593=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp4593[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp4594; _temp4594.tag= Cyc_Absyn_RgnsEff;
_temp4594.f1=( void*)({ void* yyzzz;{ void* _temp4595= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp4601;
_LL4597: if(*(( void**) _temp4595) ==  Cyc_Type_tok){ _LL4602: _temp4601=( void*)((
struct Cyc_Type_tok_struct*) _temp4595)->f1; goto _LL4598;} else{ goto _LL4599;}
_LL4599: goto _LL4600; _LL4598: yyzzz= _temp4601; goto _LL4596; _LL4600:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4596:;} yyzzz;}); _temp4594;});
_temp4593;})); _temp4592;}); _temp4591;}); break; case 204: _LL4590: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp4604=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp4604[ 0]=({ struct Cyc_Type_tok_struct
_temp4605; _temp4605.tag= Cyc_Type_tok; _temp4605.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4606=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4606[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4607; _temp4607.tag= Cyc_Absyn_JoinEff; _temp4607.f1=({
struct Cyc_List_List* _temp4608=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp4608->hd=( void*)({ void* yyzzz;{ void* _temp4617=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void*
_temp4623; _LL4619: if(*(( void**) _temp4617) ==  Cyc_Type_tok){ _LL4624:
_temp4623=( void*)(( struct Cyc_Type_tok_struct*) _temp4617)->f1; goto _LL4620;}
else{ goto _LL4621;} _LL4621: goto _LL4622; _LL4620: yyzzz= _temp4623; goto
_LL4618; _LL4622:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4618:;} yyzzz;});
_temp4608->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4609= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp4615; _LL4611: if(*(( void**) _temp4609) ==  Cyc_TypeList_tok){ _LL4616:
_temp4615=(( struct Cyc_TypeList_tok_struct*) _temp4609)->f1; goto _LL4612;}
else{ goto _LL4613;} _LL4613: goto _LL4614; _LL4612: yyzzz= _temp4615; goto
_LL4610; _LL4614:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4610:;}
yyzzz;}); _temp4608;}); _temp4607;}); _temp4606;})); _temp4605;}); _temp4604;});
break; case 205: _LL4603: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4626=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4626[ 0]=({ struct Cyc_TypeList_tok_struct _temp4627; _temp4627.tag= Cyc_TypeList_tok;
_temp4627.f1=({ struct Cyc_List_List* _temp4628=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4628->hd=( void*)({ void* yyzzz;{
void* _temp4629= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp4635; _LL4631: if(*(( void**) _temp4629) ==  Cyc_Type_tok){ _LL4636:
_temp4635=( void*)(( struct Cyc_Type_tok_struct*) _temp4629)->f1; goto _LL4632;}
else{ goto _LL4633;} _LL4633: goto _LL4634; _LL4632: yyzzz= _temp4635; goto
_LL4630; _LL4634:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4630:;} yyzzz;});
_temp4628->tl= 0; _temp4628;}); _temp4627;}); _temp4626;}); break; case 206:
_LL4625: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4638=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4638[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4639; _temp4639.tag= Cyc_TypeList_tok; _temp4639.f1=({ struct Cyc_List_List*
_temp4640=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp4640->hd=( void*)({ void* yyzzz;{ void* _temp4649= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp4655; _LL4651:
if(*(( void**) _temp4649) ==  Cyc_Type_tok){ _LL4656: _temp4655=( void*)((
struct Cyc_Type_tok_struct*) _temp4649)->f1; goto _LL4652;} else{ goto _LL4653;}
_LL4653: goto _LL4654; _LL4652: yyzzz= _temp4655; goto _LL4650; _LL4654:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4650:;} yyzzz;}); _temp4640->tl=({
struct Cyc_List_List* yyzzz;{ void* _temp4641= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp4647; _LL4643: if(*(( void**) _temp4641) ==  Cyc_TypeList_tok){ _LL4648:
_temp4647=(( struct Cyc_TypeList_tok_struct*) _temp4641)->f1; goto _LL4644;}
else{ goto _LL4645;} _LL4645: goto _LL4646; _LL4644: yyzzz= _temp4647; goto
_LL4642; _LL4646:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4642:;}
yyzzz;}); _temp4640;}); _temp4639;}); _temp4638;}); break; case 207: _LL4637:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4658=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4658[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4659; _temp4659.tag=
Cyc_AbstractDeclarator_tok; _temp4659.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4660=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4660->tms=({ struct Cyc_List_List* yyzzz;{ void* _temp4661= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp4667; _LL4663: if(*(( void**) _temp4661) ==  Cyc_TypeModifierList_tok){
_LL4668: _temp4667=(( struct Cyc_TypeModifierList_tok_struct*) _temp4661)->f1;
goto _LL4664;} else{ goto _LL4665;} _LL4665: goto _LL4666; _LL4664: yyzzz=
_temp4667; goto _LL4662; _LL4666:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL4662:;} yyzzz;}); _temp4660;}); _temp4659;}); _temp4658;}); break; case 208:
_LL4657: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 209: _LL4669: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4671=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4671[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4672; _temp4672.tag= Cyc_AbstractDeclarator_tok; _temp4672.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4673=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4673->tms= Cyc_List_imp_append(({ struct Cyc_List_List* yyzzz;{ void*
_temp4674= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4680; _LL4676: if(*(( void**) _temp4674) ==  Cyc_TypeModifierList_tok){
_LL4681: _temp4680=(( struct Cyc_TypeModifierList_tok_struct*) _temp4674)->f1;
goto _LL4677;} else{ goto _LL4678;} _LL4678: goto _LL4679; _LL4677: yyzzz=
_temp4680; goto _LL4675; _LL4679:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL4675:;} yyzzz;}),({ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void*
_temp4682= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Abstractdeclarator* _temp4688; _LL4684: if(*(( void**) _temp4682) == 
Cyc_AbstractDeclarator_tok){ _LL4689: _temp4688=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp4682)->f1; goto _LL4685;} else{ goto _LL4686;} _LL4686: goto _LL4687;
_LL4685: yyzzz= _temp4688; goto _LL4683; _LL4687:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4683:;} yyzzz;})->tms); _temp4673;}); _temp4672;}); _temp4671;}); break; case
210: _LL4670: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]; break; case 211: _LL4690: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4692=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4692[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4693; _temp4693.tag= Cyc_AbstractDeclarator_tok; _temp4693.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4694=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4694->tms=({ struct Cyc_List_List* _temp4695=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4695->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4695->tl= 0; _temp4695;}); _temp4694;}); _temp4693;}); _temp4692;}); break;
case 212: _LL4691: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4697=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4697[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4698; _temp4698.tag= Cyc_AbstractDeclarator_tok; _temp4698.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4699=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4699->tms=({ struct Cyc_List_List* _temp4700=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4700->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4700->tl=({ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp4701=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Parse_Abstractdeclarator*
_temp4707; _LL4703: if(*(( void**) _temp4701) ==  Cyc_AbstractDeclarator_tok){
_LL4708: _temp4707=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp4701)->f1;
goto _LL4704;} else{ goto _LL4705;} _LL4705: goto _LL4706; _LL4704: yyzzz=
_temp4707; goto _LL4702; _LL4706:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4702:;} yyzzz;})->tms; _temp4700;}); _temp4699;}); _temp4698;}); _temp4697;});
break; case 213: _LL4696: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4710=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4710[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4711; _temp4711.tag= Cyc_AbstractDeclarator_tok; _temp4711.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4712=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4712->tms=({ struct Cyc_List_List* _temp4713=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4713->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp4714=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp4714[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp4715; _temp4715.tag= Cyc_Absyn_ConstArray_mod;
_temp4715.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp4716= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp4722; _LL4718: if(*(( void**) _temp4716) ==  Cyc_Exp_tok){ _LL4723:
_temp4722=(( struct Cyc_Exp_tok_struct*) _temp4716)->f1; goto _LL4719;} else{
goto _LL4720;} _LL4720: goto _LL4721; _LL4719: yyzzz= _temp4722; goto _LL4717;
_LL4721:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4717:;} yyzzz;});
_temp4715;}); _temp4714;})); _temp4713->tl= 0; _temp4713;}); _temp4712;});
_temp4711;}); _temp4710;}); break; case 214: _LL4709: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4725=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4725[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4726; _temp4726.tag= Cyc_AbstractDeclarator_tok;
_temp4726.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4727=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4727->tms=({
struct Cyc_List_List* _temp4728=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp4728->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4737=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4737[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4738; _temp4738.tag= Cyc_Absyn_ConstArray_mod; _temp4738.f1=({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp4739= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Absyn_Exp* _temp4745; _LL4741: if(*(( void**)
_temp4739) ==  Cyc_Exp_tok){ _LL4746: _temp4745=(( struct Cyc_Exp_tok_struct*)
_temp4739)->f1; goto _LL4742;} else{ goto _LL4743;} _LL4743: goto _LL4744;
_LL4742: yyzzz= _temp4745; goto _LL4740; _LL4744:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL4740:;} yyzzz;}); _temp4738;}); _temp4737;})); _temp4728->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4729= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4735; _LL4731:
if(*(( void**) _temp4729) ==  Cyc_AbstractDeclarator_tok){ _LL4736: _temp4735=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4729)->f1; goto _LL4732;} else{
goto _LL4733;} _LL4733: goto _LL4734; _LL4732: yyzzz= _temp4735; goto _LL4730;
_LL4734:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4730:;}
yyzzz;})->tms; _temp4728;}); _temp4727;}); _temp4726;}); _temp4725;}); break;
case 215: _LL4724: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4748=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4748[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4749; _temp4749.tag= Cyc_AbstractDeclarator_tok; _temp4749.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4750=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4750->tms=({ struct Cyc_List_List* _temp4751=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4751->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4752=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4752[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4753; _temp4753.tag= Cyc_Absyn_Function_mod;
_temp4753.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4754=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4754[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4755; _temp4755.tag= Cyc_Absyn_WithTypes;
_temp4755.f1= 0; _temp4755.f2= 0; _temp4755.f3= 0; _temp4755.f4=({ struct Cyc_Core_Opt*
yyzzz;{ void* _temp4756= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Core_Opt* _temp4762; _LL4758: if(*(( void**)
_temp4756) ==  Cyc_TypeOpt_tok){ _LL4763: _temp4762=(( struct Cyc_TypeOpt_tok_struct*)
_temp4756)->f1; goto _LL4759;} else{ goto _LL4760;} _LL4760: goto _LL4761;
_LL4759: yyzzz= _temp4762; goto _LL4757; _LL4761:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok);
_LL4757:;} yyzzz;}); _temp4755.f5=({ struct Cyc_List_List* yyzzz;{ void*
_temp4764= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4770; _LL4766: if(*(( void**) _temp4764) ==  Cyc_Rgnorder_tok){
_LL4771: _temp4770=(( struct Cyc_Rgnorder_tok_struct*) _temp4764)->f1; goto
_LL4767;} else{ goto _LL4768;} _LL4768: goto _LL4769; _LL4767: yyzzz= _temp4770;
goto _LL4765; _LL4769:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL4765:;}
yyzzz;}); _temp4755;}); _temp4754;})); _temp4753;}); _temp4752;})); _temp4751->tl=
0; _temp4751;}); _temp4750;}); _temp4749;}); _temp4748;}); break; case 216:
_LL4747: { struct _tuple16 _temp4783; struct Cyc_List_List* _temp4784; struct
Cyc_Core_Opt* _temp4786; struct Cyc_Absyn_VarargInfo* _temp4788; int _temp4790;
struct Cyc_List_List* _temp4792; struct _tuple16* _temp4781=({ struct _tuple16*
yyzzz;{ void* _temp4773= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tuple16* _temp4779; _LL4775: if(*(( void**)
_temp4773) ==  Cyc_ParamDeclListBool_tok){ _LL4780: _temp4779=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp4773)->f1; goto _LL4776;} else{ goto _LL4777;} _LL4777: goto _LL4778;
_LL4776: yyzzz= _temp4779; goto _LL4774; _LL4778:( int) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL4774:;} yyzzz;}); _temp4783=* _temp4781; _LL4793: _temp4792= _temp4783.f1;
goto _LL4791; _LL4791: _temp4790= _temp4783.f2; goto _LL4789; _LL4789: _temp4788=
_temp4783.f3; goto _LL4787; _LL4787: _temp4786= _temp4783.f4; goto _LL4785;
_LL4785: _temp4784= _temp4783.f5; goto _LL4782; _LL4782: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4794=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4794[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4795; _temp4795.tag= Cyc_AbstractDeclarator_tok;
_temp4795.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4796=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4796->tms=({
struct Cyc_List_List* _temp4797=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp4797->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4798=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4798[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4799; _temp4799.tag=
Cyc_Absyn_Function_mod; _temp4799.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4800=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4800[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4801; _temp4801.tag= Cyc_Absyn_WithTypes;
_temp4801.f1= _temp4792; _temp4801.f2= _temp4790; _temp4801.f3= _temp4788;
_temp4801.f4= _temp4786; _temp4801.f5= _temp4784; _temp4801;}); _temp4800;}));
_temp4799;}); _temp4798;})); _temp4797->tl= 0; _temp4797;}); _temp4796;});
_temp4795;}); _temp4794;}); break;} case 217: _LL4772: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4803=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4803[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4804; _temp4804.tag= Cyc_AbstractDeclarator_tok;
_temp4804.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4805=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4805->tms=({
struct Cyc_List_List* _temp4806=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp4806->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4815=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4815[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4816; _temp4816.tag=
Cyc_Absyn_Function_mod; _temp4816.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4817=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4817[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4818; _temp4818.tag= Cyc_Absyn_WithTypes;
_temp4818.f1= 0; _temp4818.f2= 0; _temp4818.f3= 0; _temp4818.f4=({ struct Cyc_Core_Opt*
yyzzz;{ void* _temp4819= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Core_Opt* _temp4825; _LL4821: if(*(( void**)
_temp4819) ==  Cyc_TypeOpt_tok){ _LL4826: _temp4825=(( struct Cyc_TypeOpt_tok_struct*)
_temp4819)->f1; goto _LL4822;} else{ goto _LL4823;} _LL4823: goto _LL4824;
_LL4822: yyzzz= _temp4825; goto _LL4820; _LL4824:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok);
_LL4820:;} yyzzz;}); _temp4818.f5=({ struct Cyc_List_List* yyzzz;{ void*
_temp4827= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4833; _LL4829: if(*(( void**) _temp4827) ==  Cyc_Rgnorder_tok){
_LL4834: _temp4833=(( struct Cyc_Rgnorder_tok_struct*) _temp4827)->f1; goto
_LL4830;} else{ goto _LL4831;} _LL4831: goto _LL4832; _LL4830: yyzzz= _temp4833;
goto _LL4828; _LL4832:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL4828:;}
yyzzz;}); _temp4818;}); _temp4817;})); _temp4816;}); _temp4815;})); _temp4806->tl=({
struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp4807= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Parse_Abstractdeclarator*
_temp4813; _LL4809: if(*(( void**) _temp4807) ==  Cyc_AbstractDeclarator_tok){
_LL4814: _temp4813=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp4807)->f1;
goto _LL4810;} else{ goto _LL4811;} _LL4811: goto _LL4812; _LL4810: yyzzz=
_temp4813; goto _LL4808; _LL4812:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4808:;} yyzzz;})->tms; _temp4806;}); _temp4805;}); _temp4804;}); _temp4803;});
break; case 218: _LL4802: { struct _tuple16 _temp4846; struct Cyc_List_List*
_temp4847; struct Cyc_Core_Opt* _temp4849; struct Cyc_Absyn_VarargInfo*
_temp4851; int _temp4853; struct Cyc_List_List* _temp4855; struct _tuple16*
_temp4844=({ struct _tuple16* yyzzz;{ void* _temp4836= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple16*
_temp4842; _LL4838: if(*(( void**) _temp4836) ==  Cyc_ParamDeclListBool_tok){
_LL4843: _temp4842=(( struct Cyc_ParamDeclListBool_tok_struct*) _temp4836)->f1;
goto _LL4839;} else{ goto _LL4840;} _LL4840: goto _LL4841; _LL4839: yyzzz=
_temp4842; goto _LL4837; _LL4841:( int) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL4837:;} yyzzz;}); _temp4846=* _temp4844; _LL4856: _temp4855= _temp4846.f1;
goto _LL4854; _LL4854: _temp4853= _temp4846.f2; goto _LL4852; _LL4852: _temp4851=
_temp4846.f3; goto _LL4850; _LL4850: _temp4849= _temp4846.f4; goto _LL4848;
_LL4848: _temp4847= _temp4846.f5; goto _LL4845; _LL4845: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4857=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4857[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4858; _temp4858.tag= Cyc_AbstractDeclarator_tok;
_temp4858.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4859=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4859->tms=({
struct Cyc_List_List* _temp4860=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp4860->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4869=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4869[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4870; _temp4870.tag=
Cyc_Absyn_Function_mod; _temp4870.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4871=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4871[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4872; _temp4872.tag= Cyc_Absyn_WithTypes;
_temp4872.f1= _temp4855; _temp4872.f2= _temp4853; _temp4872.f3= _temp4851;
_temp4872.f4= _temp4849; _temp4872.f5= _temp4847; _temp4872;}); _temp4871;}));
_temp4870;}); _temp4869;})); _temp4860->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4861= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4867; _LL4863:
if(*(( void**) _temp4861) ==  Cyc_AbstractDeclarator_tok){ _LL4868: _temp4867=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4861)->f1; goto _LL4864;} else{
goto _LL4865;} _LL4865: goto _LL4866; _LL4864: yyzzz= _temp4867; goto _LL4862;
_LL4866:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4862:;}
yyzzz;})->tms; _temp4860;}); _temp4859;}); _temp4858;}); _temp4857;}); break;}
case 219: _LL4835: { struct Cyc_List_List* _temp4874=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp4889= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4895; _LL4891: if(*(( void**) _temp4889) ==  Cyc_TypeList_tok){ _LL4896:
_temp4895=(( struct Cyc_TypeList_tok_struct*) _temp4889)->f1; goto _LL4892;}
else{ goto _LL4893;} _LL4893: goto _LL4894; _LL4892: yyzzz= _temp4895; goto
_LL4890; _LL4894:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4890:;}
yyzzz;}))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4875=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4875[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4876; _temp4876.tag=
Cyc_AbstractDeclarator_tok; _temp4876.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4877=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4877->tms=({ struct Cyc_List_List* _temp4878=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4878->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4887=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4887[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4888; _temp4888.tag= Cyc_Absyn_TypeParams_mod;
_temp4888.f1= _temp4874; _temp4888.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp4888.f3=
0; _temp4888;}); _temp4887;})); _temp4878->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4879= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4885; _LL4881:
if(*(( void**) _temp4879) ==  Cyc_AbstractDeclarator_tok){ _LL4886: _temp4885=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4879)->f1; goto _LL4882;} else{
goto _LL4883;} _LL4883: goto _LL4884; _LL4882: yyzzz= _temp4885; goto _LL4880;
_LL4884:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4880:;}
yyzzz;})->tms; _temp4878;}); _temp4877;}); _temp4876;}); _temp4875;}); break;}
case 220: _LL4873:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;{ struct Cyc_List_List*
_temp4898=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp4913= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4919; _LL4915: if(*(( void**) _temp4913) ==  Cyc_TypeList_tok){ _LL4920:
_temp4919=(( struct Cyc_TypeList_tok_struct*) _temp4913)->f1; goto _LL4916;}
else{ goto _LL4917;} _LL4917: goto _LL4918; _LL4916: yyzzz= _temp4919; goto
_LL4914; _LL4918:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4914:;}
yyzzz;}))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4899=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4899[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4900; _temp4900.tag=
Cyc_AbstractDeclarator_tok; _temp4900.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4901=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4901->tms=({ struct Cyc_List_List* _temp4902=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4902->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4911=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4911[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4912; _temp4912.tag= Cyc_Absyn_TypeParams_mod;
_temp4912.f1= _temp4898; _temp4912.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp4912.f3=
0; _temp4912;}); _temp4911;})); _temp4902->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4903= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4909; _LL4905:
if(*(( void**) _temp4903) ==  Cyc_AbstractDeclarator_tok){ _LL4910: _temp4909=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4903)->f1; goto _LL4906;} else{
goto _LL4907;} _LL4907: goto _LL4908; _LL4906: yyzzz= _temp4909; goto _LL4904;
_LL4908:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4904:;}
yyzzz;})->tms; _temp4902;}); _temp4901;}); _temp4900;}); _temp4899;}); break;}
case 221: _LL4897: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4922=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4922[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4923; _temp4923.tag= Cyc_AbstractDeclarator_tok; _temp4923.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4924=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4924->tms=({ struct Cyc_List_List* _temp4925=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp4925->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp4934=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp4934[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp4935; _temp4935.tag= Cyc_Absyn_Attributes_mod;
_temp4935.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp4935.f2=({ struct Cyc_List_List* yyzzz;{ void*
_temp4936= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp4942; _LL4938: if(*(( void**) _temp4936) ==  Cyc_AttributeList_tok){
_LL4943: _temp4942=(( struct Cyc_AttributeList_tok_struct*) _temp4936)->f1; goto
_LL4939;} else{ goto _LL4940;} _LL4940: goto _LL4941; _LL4939: yyzzz= _temp4942;
goto _LL4937; _LL4941:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL4937:;} yyzzz;}); _temp4935;}); _temp4934;})); _temp4925->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4926= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Parse_Abstractdeclarator* _temp4932; _LL4928:
if(*(( void**) _temp4926) ==  Cyc_AbstractDeclarator_tok){ _LL4933: _temp4932=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4926)->f1; goto _LL4929;} else{
goto _LL4930;} _LL4930: goto _LL4931; _LL4929: yyzzz= _temp4932; goto _LL4927;
_LL4931:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4927:;}
yyzzz;})->tms; _temp4925;}); _temp4924;}); _temp4923;}); _temp4922;}); break;
case 222: _LL4921: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 223: _LL4944: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL4945:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL4946: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL4947: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL4948: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL4949:
if( Cyc_Std_zstrcmp(({ struct _tagged_arr yyzzz;{ void* _temp4951= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct _tagged_arr
_temp4957; _LL4953: if(*(( void**) _temp4951) ==  Cyc_String_tok){ _LL4958:
_temp4957=(( struct Cyc_String_tok_struct*) _temp4951)->f1; goto _LL4954;} else{
goto _LL4955;} _LL4955: goto _LL4956; _LL4954: yyzzz= _temp4957; goto _LL4952;
_LL4956:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4952:;} yyzzz;}),
_tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp4985=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp4985->name=({ struct
_tagged_arr* _temp4986=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp4986[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp4987= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct _tagged_arr
_temp4993; _LL4989: if(*(( void**) _temp4987) ==  Cyc_String_tok){ _LL4994:
_temp4993=(( struct Cyc_String_tok_struct*) _temp4987)->f1; goto _LL4990;} else{
goto _LL4991;} _LL4991: goto _LL4992; _LL4990: yyzzz= _temp4993; goto _LL4988;
_LL4992:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4988:;} yyzzz;});
_temp4986;}); _temp4985->identity= 0; _temp4985->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp4985;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp4983=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp4983[ 0]=({ struct Cyc_Absyn_VarType_struct _temp4984; _temp4984.tag= Cyc_Absyn_VarType;
_temp4984.f1= tv; _temp4984;}); _temp4983;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp4959=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4959[ 0]=({ struct Cyc_Stmt_tok_struct _temp4960; _temp4960.tag= Cyc_Stmt_tok;
_temp4960.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp4961=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp4961[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp4962; _temp4962.tag= Cyc_Absyn_Region_s;
_temp4962.f1= tv; _temp4962.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp4963=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp4963->f1=(
void*) Cyc_Absyn_Loc_n; _temp4963->f2=({ struct _tagged_arr* _temp4964=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp4964[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp4965= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp4971; _LL4967: if(*(( void**)
_temp4965) ==  Cyc_String_tok){ _LL4972: _temp4971=(( struct Cyc_String_tok_struct*)
_temp4965)->f1; goto _LL4968;} else{ goto _LL4969;} _LL4969: goto _LL4970;
_LL4968: yyzzz= _temp4971; goto _LL4966; _LL4970:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL4966:;} yyzzz;}); _temp4964;}); _temp4963;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp4973=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp4973[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp4974; _temp4974.tag= Cyc_Absyn_RgnHandleType; _temp4974.f1=( void*) t;
_temp4974;}); _temp4973;}), 0); _temp4962.f3=({ struct Cyc_Absyn_Stmt* yyzzz;{
void* _temp4975= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_Absyn_Stmt* _temp4981; _LL4977: if(*(( void**) _temp4975) ==  Cyc_Stmt_tok){
_LL4982: _temp4981=(( struct Cyc_Stmt_tok_struct*) _temp4975)->f1; goto _LL4978;}
else{ goto _LL4979;} _LL4979: goto _LL4980; _LL4978: yyzzz= _temp4981; goto
_LL4976; _LL4980:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL4976:;} yyzzz;});
_temp4962;}); _temp4961;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp4960;});
_temp4959;}); break;} case 229: _LL4950: if( Cyc_Std_zstrcmp(({ struct
_tagged_arr yyzzz;{ void* _temp4996= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp5002; _LL4998: if(*(( void**)
_temp4996) ==  Cyc_String_tok){ _LL5003: _temp5002=(( struct Cyc_String_tok_struct*)
_temp4996)->f1; goto _LL4999;} else{ goto _LL5000;} _LL5000: goto _LL5001;
_LL4999: yyzzz= _temp5002; goto _LL4997; _LL5001:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL4997:;} yyzzz;}), _tag_arr("H", sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err(
_tag_arr("bad occurrence of heap region `H", sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5030=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5030->name=({ struct
_tagged_arr* _temp5031=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp5031[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp5033; _temp5033.tag= Cyc_Std_String_pa; _temp5033.f1=( struct _tagged_arr)({
struct _tagged_arr yyzzz;{ void* _temp5034= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct _tagged_arr _temp5040; _LL5036: if(*(( void**)
_temp5034) ==  Cyc_String_tok){ _LL5041: _temp5040=(( struct Cyc_String_tok_struct*)
_temp5034)->f1; goto _LL5037;} else{ goto _LL5038;} _LL5038: goto _LL5039;
_LL5037: yyzzz= _temp5040; goto _LL5035; _LL5039:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5035:;} yyzzz;});{ void* _temp5032[ 1u]={& _temp5033}; Cyc_Std_aprintf(
_tag_arr("`%s", sizeof( unsigned char), 4u), _tag_arr( _temp5032, sizeof( void*),
1u));}}); _temp5031;}); _temp5030->identity= 0; _temp5030->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp5030;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5028=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5028[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5029; _temp5029.tag= Cyc_Absyn_VarType;
_temp5029.f1= tv; _temp5029;}); _temp5028;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5004=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5004[ 0]=({ struct Cyc_Stmt_tok_struct _temp5005; _temp5005.tag= Cyc_Stmt_tok;
_temp5005.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5006=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5006[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5007; _temp5007.tag= Cyc_Absyn_Region_s;
_temp5007.f1= tv; _temp5007.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5008=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp5008->f1=(
void*) Cyc_Absyn_Loc_n; _temp5008->f2=({ struct _tagged_arr* _temp5009=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp5009[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp5010= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp5016; _LL5012: if(*(( void**)
_temp5010) ==  Cyc_String_tok){ _LL5017: _temp5016=(( struct Cyc_String_tok_struct*)
_temp5010)->f1; goto _LL5013;} else{ goto _LL5014;} _LL5014: goto _LL5015;
_LL5013: yyzzz= _temp5016; goto _LL5011; _LL5015:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5011:;} yyzzz;}); _temp5009;}); _temp5008;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp5018=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp5018[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp5019; _temp5019.tag= Cyc_Absyn_RgnHandleType; _temp5019.f1=( void*) t;
_temp5019;}); _temp5018;}), 0); _temp5007.f3=({ struct Cyc_Absyn_Stmt* yyzzz;{
void* _temp5020= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_Absyn_Stmt* _temp5026; _LL5022: if(*(( void**) _temp5020) ==  Cyc_Stmt_tok){
_LL5027: _temp5026=(( struct Cyc_Stmt_tok_struct*) _temp5020)->f1; goto _LL5023;}
else{ goto _LL5024;} _LL5024: goto _LL5025; _LL5023: yyzzz= _temp5026; goto
_LL5021; _LL5025:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5021:;} yyzzz;});
_temp5007;}); _temp5006;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5005;});
_temp5004;}); break;} case 230: _LL4995: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5043=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5043[ 0]=({ struct Cyc_Stmt_tok_struct _temp5044; _temp5044.tag= Cyc_Stmt_tok;
_temp5044.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp5045=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp5045[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp5046; _temp5046.tag= Cyc_Absyn_Cut_s;
_temp5046.f1=({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5047= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5053; _LL5049: if(*(( void**) _temp5047) ==  Cyc_Stmt_tok){ _LL5054:
_temp5053=(( struct Cyc_Stmt_tok_struct*) _temp5047)->f1; goto _LL5050;} else{
goto _LL5051;} _LL5051: goto _LL5052; _LL5050: yyzzz= _temp5053; goto _LL5048;
_LL5052:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5048:;} yyzzz;});
_temp5046;}); _temp5045;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5044;});
_temp5043;}); break; case 231: _LL5042: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5056=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5056[ 0]=({ struct Cyc_Stmt_tok_struct _temp5057; _temp5057.tag= Cyc_Stmt_tok;
_temp5057.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp5058=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp5058[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp5059; _temp5059.tag= Cyc_Absyn_Splice_s;
_temp5059.f1=({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5060= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5066; _LL5062: if(*(( void**) _temp5060) ==  Cyc_Stmt_tok){ _LL5067:
_temp5066=(( struct Cyc_Stmt_tok_struct*) _temp5060)->f1; goto _LL5063;} else{
goto _LL5064;} _LL5064: goto _LL5065; _LL5063: yyzzz= _temp5066; goto _LL5061;
_LL5065:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5061:;} yyzzz;});
_temp5059;}); _temp5058;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5057;});
_temp5056;}); break; case 232: _LL5055: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5069=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5069[ 0]=({ struct Cyc_Stmt_tok_struct _temp5070; _temp5070.tag= Cyc_Stmt_tok;
_temp5070.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp5071=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp5071[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp5072; _temp5072.tag= Cyc_Absyn_Label_s;
_temp5072.f1=({ struct _tagged_arr* _temp5073=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp5073[ 0]=({ struct _tagged_arr yyzzz;{ void*
_temp5074= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct _tagged_arr _temp5080; _LL5076: if(*(( void**) _temp5074) ==  Cyc_String_tok){
_LL5081: _temp5080=(( struct Cyc_String_tok_struct*) _temp5074)->f1; goto
_LL5077;} else{ goto _LL5078;} _LL5078: goto _LL5079; _LL5077: yyzzz= _temp5080;
goto _LL5075; _LL5079:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL5075:;}
yyzzz;}); _temp5073;}); _temp5072.f2=({ struct Cyc_Absyn_Stmt* yyzzz;{ void*
_temp5082= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Stmt* _temp5088; _LL5084: if(*(( void**) _temp5082) ==  Cyc_Stmt_tok){
_LL5089: _temp5088=(( struct Cyc_Stmt_tok_struct*) _temp5082)->f1; goto _LL5085;}
else{ goto _LL5086;} _LL5086: goto _LL5087; _LL5085: yyzzz= _temp5088; goto
_LL5083; _LL5087:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5083:;} yyzzz;});
_temp5072;}); _temp5071;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5070;});
_temp5069;}); break; case 233: _LL5068: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5091=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5091[ 0]=({ struct Cyc_Stmt_tok_struct _temp5092; _temp5092.tag= Cyc_Stmt_tok;
_temp5092.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5092;});
_temp5091;}); break; case 234: _LL5090: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5094=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5094[ 0]=({ struct Cyc_Stmt_tok_struct _temp5095; _temp5095.tag= Cyc_Stmt_tok;
_temp5095.f1= Cyc_Absyn_exp_stmt(({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp5096= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Exp* _temp5102; _LL5098: if(*(( void**) _temp5096) ==  Cyc_Exp_tok){
_LL5103: _temp5102=(( struct Cyc_Exp_tok_struct*) _temp5096)->f1; goto _LL5099;}
else{ goto _LL5100;} _LL5100: goto _LL5101; _LL5099: yyzzz= _temp5102; goto
_LL5097; _LL5101:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5097:;} yyzzz;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp5095;}); _temp5094;}); break; case 235:
_LL5093: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5105=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5105[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5106; _temp5106.tag= Cyc_Stmt_tok; _temp5106.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5106;}); _temp5105;}); break; case 236: _LL5104: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 237:
_LL5107: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5109=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5109[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5110; _temp5110.tag= Cyc_Stmt_tok; _temp5110.f1= Cyc_Parse_flatten_declarations(({
struct Cyc_List_List* yyzzz;{ void* _temp5111= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5117; _LL5113: if(*(( void**) _temp5111) ==  Cyc_DeclList_tok){ _LL5118:
_temp5117=(( struct Cyc_DeclList_tok_struct*) _temp5111)->f1; goto _LL5114;}
else{ goto _LL5115;} _LL5115: goto _LL5116; _LL5114: yyzzz= _temp5117; goto
_LL5112; _LL5116:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5112:;}
yyzzz;}), Cyc_Absyn_skip_stmt( 0)); _temp5110;}); _temp5109;}); break; case 238:
_LL5108: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5120=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5120[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5121; _temp5121.tag= Cyc_Stmt_tok; _temp5121.f1= Cyc_Parse_flatten_declarations(({
struct Cyc_List_List* yyzzz;{ void* _temp5122= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5128; _LL5124: if(*(( void**) _temp5122) ==  Cyc_DeclList_tok){ _LL5129:
_temp5128=(( struct Cyc_DeclList_tok_struct*) _temp5122)->f1; goto _LL5125;}
else{ goto _LL5126;} _LL5126: goto _LL5127; _LL5125: yyzzz= _temp5128; goto
_LL5123; _LL5127:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5123:;}
yyzzz;}),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5130= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5136; _LL5132: if(*(( void**) _temp5130) ==  Cyc_Stmt_tok){ _LL5137:
_temp5136=(( struct Cyc_Stmt_tok_struct*) _temp5130)->f1; goto _LL5133;} else{
goto _LL5134;} _LL5134: goto _LL5135; _LL5133: yyzzz= _temp5136; goto _LL5131;
_LL5135:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5131:;} yyzzz;}));
_temp5121;}); _temp5120;}); break; case 239: _LL5119: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 240: _LL5138:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5140=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5140[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5141; _temp5141.tag= Cyc_Stmt_tok; _temp5141.f1= Cyc_Absyn_seq_stmt(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5142= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Stmt*
_temp5148; _LL5144: if(*(( void**) _temp5142) ==  Cyc_Stmt_tok){ _LL5149:
_temp5148=(( struct Cyc_Stmt_tok_struct*) _temp5142)->f1; goto _LL5145;} else{
goto _LL5146;} _LL5146: goto _LL5147; _LL5145: yyzzz= _temp5148; goto _LL5143;
_LL5147:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5143:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5150= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5156; _LL5152: if(*(( void**) _temp5150) ==  Cyc_Stmt_tok){ _LL5157:
_temp5156=(( struct Cyc_Stmt_tok_struct*) _temp5150)->f1; goto _LL5153;} else{
goto _LL5154;} _LL5154: goto _LL5155; _LL5153: yyzzz= _temp5156; goto _LL5151;
_LL5155:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5151:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5141;}); _temp5140;}); break; case 241: _LL5139: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5159=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5159[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5160; _temp5160.tag= Cyc_Stmt_tok; _temp5160.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp5161=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5161[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5162; _temp5162.tag= Cyc_Absyn_Fn_d;
_temp5162.f1=({ struct Cyc_Absyn_Fndecl* yyzzz;{ void* _temp5163= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Fndecl*
_temp5169; _LL5165: if(*(( void**) _temp5163) ==  Cyc_FnDecl_tok){ _LL5170:
_temp5169=(( struct Cyc_FnDecl_tok_struct*) _temp5163)->f1; goto _LL5166;} else{
goto _LL5167;} _LL5167: goto _LL5168; _LL5166: yyzzz= _temp5169; goto _LL5164;
_LL5168:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL5164:;} yyzzz;});
_temp5162;}); _temp5161;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp5160;}); _temp5159;}); break; case 242: _LL5158: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp5172=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5172[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5173; _temp5173.tag= Cyc_Stmt_tok; _temp5173.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp5174=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5174[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5175; _temp5175.tag= Cyc_Absyn_Fn_d;
_temp5175.f1=({ struct Cyc_Absyn_Fndecl* yyzzz;{ void* _temp5176= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Fndecl*
_temp5182; _LL5178: if(*(( void**) _temp5176) ==  Cyc_FnDecl_tok){ _LL5183:
_temp5182=(( struct Cyc_FnDecl_tok_struct*) _temp5176)->f1; goto _LL5179;} else{
goto _LL5180;} _LL5180: goto _LL5181; _LL5179: yyzzz= _temp5182; goto _LL5177;
_LL5181:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL5177:;} yyzzz;});
_temp5175;}); _temp5174;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5184= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5190; _LL5186: if(*(( void**) _temp5184) ==  Cyc_Stmt_tok){ _LL5191:
_temp5190=(( struct Cyc_Stmt_tok_struct*) _temp5184)->f1; goto _LL5187;} else{
goto _LL5188;} _LL5188: goto _LL5189; _LL5187: yyzzz= _temp5190; goto _LL5185;
_LL5189:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5185:;} yyzzz;}));
_temp5173;}); _temp5172;}); break; case 243: _LL5171: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5193=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5193[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5194; _temp5194.tag= Cyc_Stmt_tok; _temp5194.f1= Cyc_Absyn_ifthenelse_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5195= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5201; _LL5197: if(*(( void**) _temp5195) ==  Cyc_Exp_tok){ _LL5202:
_temp5201=(( struct Cyc_Exp_tok_struct*) _temp5195)->f1; goto _LL5198;} else{
goto _LL5199;} _LL5199: goto _LL5200; _LL5198: yyzzz= _temp5201; goto _LL5196;
_LL5200:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5196:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5203= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5209; _LL5205: if(*(( void**) _temp5203) ==  Cyc_Stmt_tok){ _LL5210:
_temp5209=(( struct Cyc_Stmt_tok_struct*) _temp5203)->f1; goto _LL5206;} else{
goto _LL5207;} _LL5207: goto _LL5208; _LL5206: yyzzz= _temp5209; goto _LL5204;
_LL5208:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5204:;} yyzzz;}), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp5194;}); _temp5193;}); break; case 244:
_LL5192: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5212=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5212[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5213; _temp5213.tag= Cyc_Stmt_tok; _temp5213.f1= Cyc_Absyn_ifthenelse_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5214= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5220; _LL5216: if(*(( void**) _temp5214) ==  Cyc_Exp_tok){ _LL5221:
_temp5220=(( struct Cyc_Exp_tok_struct*) _temp5214)->f1; goto _LL5217;} else{
goto _LL5218;} _LL5218: goto _LL5219; _LL5217: yyzzz= _temp5220; goto _LL5215;
_LL5219:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5215:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5222= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Stmt*
_temp5228; _LL5224: if(*(( void**) _temp5222) ==  Cyc_Stmt_tok){ _LL5229:
_temp5228=(( struct Cyc_Stmt_tok_struct*) _temp5222)->f1; goto _LL5225;} else{
goto _LL5226;} _LL5226: goto _LL5227; _LL5225: yyzzz= _temp5228; goto _LL5223;
_LL5227:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5223:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5230= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5236; _LL5232: if(*(( void**) _temp5230) ==  Cyc_Stmt_tok){ _LL5237:
_temp5236=(( struct Cyc_Stmt_tok_struct*) _temp5230)->f1; goto _LL5233;} else{
goto _LL5234;} _LL5234: goto _LL5235; _LL5233: yyzzz= _temp5236; goto _LL5231;
_LL5235:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5231:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5213;}); _temp5212;}); break; case 245: _LL5211: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5239=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5239[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5240; _temp5240.tag= Cyc_Stmt_tok; _temp5240.f1= Cyc_Absyn_switch_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5241= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5247; _LL5243: if(*(( void**) _temp5241) ==  Cyc_Exp_tok){ _LL5248:
_temp5247=(( struct Cyc_Exp_tok_struct*) _temp5241)->f1; goto _LL5244;} else{
goto _LL5245;} _LL5245: goto _LL5246; _LL5244: yyzzz= _temp5247; goto _LL5242;
_LL5246:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5242:;} yyzzz;}),({
struct Cyc_List_List* yyzzz;{ void* _temp5249= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5255; _LL5251: if(*(( void**) _temp5249) ==  Cyc_SwitchClauseList_tok){
_LL5256: _temp5255=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5249)->f1;
goto _LL5252;} else{ goto _LL5253;} _LL5253: goto _LL5254; _LL5252: yyzzz=
_temp5255; goto _LL5250; _LL5254:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5250:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5240;});
_temp5239;}); break; case 246: _LL5238: if( Cyc_Std_strcmp(({ struct _tagged_arr
yyzzz;{ void* _temp5258= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  6)]; struct _tagged_arr _temp5264; _LL5260: if(*(( void**)
_temp5258) ==  Cyc_String_tok){ _LL5265: _temp5264=(( struct Cyc_String_tok_struct*)
_temp5258)->f1; goto _LL5261;} else{ goto _LL5262;} _LL5262: goto _LL5263;
_LL5261: yyzzz= _temp5264; goto _LL5259; _LL5263:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5259:;} yyzzz;}), _tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err(
_tag_arr("only switch \"C\" { ... } is allowed", sizeof( unsigned char), 35u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5266=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5266[ 0]=({ struct Cyc_Stmt_tok_struct _temp5267; _temp5267.tag= Cyc_Stmt_tok;
_temp5267.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_SwitchC_s_struct*
_temp5268=( struct Cyc_Absyn_SwitchC_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct));
_temp5268[ 0]=({ struct Cyc_Absyn_SwitchC_s_struct _temp5269; _temp5269.tag= Cyc_Absyn_SwitchC_s;
_temp5269.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5270= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5276; _LL5272: if(*(( void**) _temp5270) ==  Cyc_Exp_tok){ _LL5277:
_temp5276=(( struct Cyc_Exp_tok_struct*) _temp5270)->f1; goto _LL5273;} else{
goto _LL5274;} _LL5274: goto _LL5275; _LL5273: yyzzz= _temp5276; goto _LL5271;
_LL5275:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5271:;} yyzzz;});
_temp5269.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp5278= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5284; _LL5280: if(*(( void**) _temp5278) ==  Cyc_SwitchCClauseList_tok){
_LL5285: _temp5284=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp5278)->f1;
goto _LL5281;} else{ goto _LL5282;} _LL5282: goto _LL5283; _LL5281: yyzzz=
_temp5284; goto _LL5279; _LL5283:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL5279:;} yyzzz;}); _temp5269;}); _temp5268;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5267;}); _temp5266;}); break; case 247: _LL5257: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5287=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5287[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5288; _temp5288.tag= Cyc_Stmt_tok; _temp5288.f1= Cyc_Absyn_trycatch_stmt(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5289= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Stmt*
_temp5295; _LL5291: if(*(( void**) _temp5289) ==  Cyc_Stmt_tok){ _LL5296:
_temp5295=(( struct Cyc_Stmt_tok_struct*) _temp5289)->f1; goto _LL5292;} else{
goto _LL5293;} _LL5293: goto _LL5294; _LL5292: yyzzz= _temp5295; goto _LL5290;
_LL5294:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5290:;} yyzzz;}),({
struct Cyc_List_List* yyzzz;{ void* _temp5297= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5303; _LL5299: if(*(( void**) _temp5297) ==  Cyc_SwitchClauseList_tok){
_LL5304: _temp5303=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5297)->f1;
goto _LL5300;} else{ goto _LL5301;} _LL5301: goto _LL5302; _LL5300: yyzzz=
_temp5303; goto _LL5298; _LL5302:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5298:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5288;});
_temp5287;}); break; case 248: _LL5286: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5306=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5306[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5307; _temp5307.tag= Cyc_SwitchClauseList_tok; _temp5307.f1= 0; _temp5307;});
_temp5306;}); break; case 249: _LL5305: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5309=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5309[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5310; _temp5310.tag= Cyc_SwitchClauseList_tok; _temp5310.f1=({ struct Cyc_List_List*
_temp5311=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp5311->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5312=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5312->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp5312->pat_vars= 0; _temp5312->where_clause= 0; _temp5312->body=({ struct
Cyc_Absyn_Stmt* yyzzz;{ void* _temp5313= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt* _temp5319; _LL5315: if(*(( void**)
_temp5313) ==  Cyc_Stmt_tok){ _LL5320: _temp5319=(( struct Cyc_Stmt_tok_struct*)
_temp5313)->f1; goto _LL5316;} else{ goto _LL5317;} _LL5317: goto _LL5318;
_LL5316: yyzzz= _temp5319; goto _LL5314; _LL5318:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL5314:;} yyzzz;}); _temp5312->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5312;});
_temp5311->tl= 0; _temp5311;}); _temp5310;}); _temp5309;}); break; case 250:
_LL5308: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5322=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5322[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5323; _temp5323.tag=
Cyc_SwitchClauseList_tok; _temp5323.f1=({ struct Cyc_List_List* _temp5324=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp5324->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5333=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5333->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5334= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Pat*
_temp5340; _LL5336: if(*(( void**) _temp5334) ==  Cyc_Pattern_tok){ _LL5341:
_temp5340=(( struct Cyc_Pattern_tok_struct*) _temp5334)->f1; goto _LL5337;}
else{ goto _LL5338;} _LL5338: goto _LL5339; _LL5337: yyzzz= _temp5340; goto
_LL5335; _LL5339:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5335:;}
yyzzz;}); _temp5333->pat_vars= 0; _temp5333->where_clause= 0; _temp5333->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5333->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5333;});
_temp5324->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5325= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5331; _LL5327: if(*(( void**) _temp5325) ==  Cyc_SwitchClauseList_tok){
_LL5332: _temp5331=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5325)->f1;
goto _LL5328;} else{ goto _LL5329;} _LL5329: goto _LL5330; _LL5328: yyzzz=
_temp5331; goto _LL5326; _LL5330:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5326:;} yyzzz;}); _temp5324;}); _temp5323;}); _temp5322;}); break; case 251:
_LL5321: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5343=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5343[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5344; _temp5344.tag=
Cyc_SwitchClauseList_tok; _temp5344.f1=({ struct Cyc_List_List* _temp5345=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp5345->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5354=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5354->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5363= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Pat*
_temp5369; _LL5365: if(*(( void**) _temp5363) ==  Cyc_Pattern_tok){ _LL5370:
_temp5369=(( struct Cyc_Pattern_tok_struct*) _temp5363)->f1; goto _LL5366;}
else{ goto _LL5367;} _LL5367: goto _LL5368; _LL5366: yyzzz= _temp5369; goto
_LL5364; _LL5368:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5364:;}
yyzzz;}); _temp5354->pat_vars= 0; _temp5354->where_clause= 0; _temp5354->body=({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5355= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Stmt*
_temp5361; _LL5357: if(*(( void**) _temp5355) ==  Cyc_Stmt_tok){ _LL5362:
_temp5361=(( struct Cyc_Stmt_tok_struct*) _temp5355)->f1; goto _LL5358;} else{
goto _LL5359;} _LL5359: goto _LL5360; _LL5358: yyzzz= _temp5361; goto _LL5356;
_LL5360:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5356:;} yyzzz;});
_temp5354->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp5354;}); _temp5345->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5346=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5352; _LL5348: if(*(( void**) _temp5346) ==  Cyc_SwitchClauseList_tok){
_LL5353: _temp5352=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5346)->f1;
goto _LL5349;} else{ goto _LL5350;} _LL5350: goto _LL5351; _LL5349: yyzzz=
_temp5352; goto _LL5347; _LL5351:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5347:;} yyzzz;}); _temp5345;}); _temp5344;}); _temp5343;}); break; case 252:
_LL5342: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5372=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5372[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5373; _temp5373.tag=
Cyc_SwitchClauseList_tok; _temp5373.f1=({ struct Cyc_List_List* _temp5374=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp5374->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5383=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5383->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5392= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Pat*
_temp5398; _LL5394: if(*(( void**) _temp5392) ==  Cyc_Pattern_tok){ _LL5399:
_temp5398=(( struct Cyc_Pattern_tok_struct*) _temp5392)->f1; goto _LL5395;}
else{ goto _LL5396;} _LL5396: goto _LL5397; _LL5395: yyzzz= _temp5398; goto
_LL5393; _LL5397:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5393:;}
yyzzz;}); _temp5383->pat_vars= 0; _temp5383->where_clause=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5384= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5390; _LL5386: if(*(( void**) _temp5384) ==  Cyc_Exp_tok){ _LL5391:
_temp5390=(( struct Cyc_Exp_tok_struct*) _temp5384)->f1; goto _LL5387;} else{
goto _LL5388;} _LL5388: goto _LL5389; _LL5387: yyzzz= _temp5390; goto _LL5385;
_LL5389:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5385:;} yyzzz;});
_temp5383->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5383->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5383;});
_temp5374->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5375= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5381; _LL5377: if(*(( void**) _temp5375) ==  Cyc_SwitchClauseList_tok){
_LL5382: _temp5381=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5375)->f1;
goto _LL5378;} else{ goto _LL5379;} _LL5379: goto _LL5380; _LL5378: yyzzz=
_temp5381; goto _LL5376; _LL5380:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5376:;} yyzzz;}); _temp5374;}); _temp5373;}); _temp5372;}); break; case 253:
_LL5371: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5401=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5401[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5402; _temp5402.tag=
Cyc_SwitchClauseList_tok; _temp5402.f1=({ struct Cyc_List_List* _temp5403=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp5403->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5412=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5412->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5429= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Pat*
_temp5435; _LL5431: if(*(( void**) _temp5429) ==  Cyc_Pattern_tok){ _LL5436:
_temp5435=(( struct Cyc_Pattern_tok_struct*) _temp5429)->f1; goto _LL5432;}
else{ goto _LL5433;} _LL5433: goto _LL5434; _LL5432: yyzzz= _temp5435; goto
_LL5430; _LL5434:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5430:;}
yyzzz;}); _temp5412->pat_vars= 0; _temp5412->where_clause=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5421= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5427; _LL5423: if(*(( void**) _temp5421) ==  Cyc_Exp_tok){ _LL5428:
_temp5427=(( struct Cyc_Exp_tok_struct*) _temp5421)->f1; goto _LL5424;} else{
goto _LL5425;} _LL5425: goto _LL5426; _LL5424: yyzzz= _temp5427; goto _LL5422;
_LL5426:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5422:;} yyzzz;});
_temp5412->body=({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5413= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Stmt*
_temp5419; _LL5415: if(*(( void**) _temp5413) ==  Cyc_Stmt_tok){ _LL5420:
_temp5419=(( struct Cyc_Stmt_tok_struct*) _temp5413)->f1; goto _LL5416;} else{
goto _LL5417;} _LL5417: goto _LL5418; _LL5416: yyzzz= _temp5419; goto _LL5414;
_LL5418:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5414:;} yyzzz;});
_temp5412->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5412;});
_temp5403->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5404= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5410; _LL5406: if(*(( void**) _temp5404) ==  Cyc_SwitchClauseList_tok){
_LL5411: _temp5410=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5404)->f1;
goto _LL5407;} else{ goto _LL5408;} _LL5408: goto _LL5409; _LL5407: yyzzz=
_temp5410; goto _LL5405; _LL5409:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5405:;} yyzzz;}); _temp5403;}); _temp5402;}); _temp5401;}); break; case 254:
_LL5400: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp5438=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp5438[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp5439; _temp5439.tag=
Cyc_SwitchCClauseList_tok; _temp5439.f1= 0; _temp5439;}); _temp5438;}); break;
case 255: _LL5437: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp5441=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp5441[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp5442; _temp5442.tag= Cyc_SwitchCClauseList_tok; _temp5442.f1=({ struct Cyc_List_List*
_temp5443=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp5443->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp5444=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp5444->cnst_exp= 0;
_temp5444->body= Cyc_Absyn_seq_stmt(({ struct Cyc_Absyn_Stmt* yyzzz;{ void*
_temp5445= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Stmt* _temp5451; _LL5447: if(*(( void**) _temp5445) ==  Cyc_Stmt_tok){
_LL5452: _temp5451=(( struct Cyc_Stmt_tok_struct*) _temp5445)->f1; goto _LL5448;}
else{ goto _LL5449;} _LL5449: goto _LL5450; _LL5448: yyzzz= _temp5451; goto
_LL5446; _LL5450:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5446:;} yyzzz;}),
Cyc_Absyn_break_stmt( 0), 0); _temp5444->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5444;});
_temp5443->tl= 0; _temp5443;}); _temp5442;}); _temp5441;}); break; case 256:
_LL5440: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp5454=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp5454[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp5455; _temp5455.tag=
Cyc_SwitchCClauseList_tok; _temp5455.f1=({ struct Cyc_List_List* _temp5456=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp5456->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp5465=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp5465->cnst_exp=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5466= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5472; _LL5468: if(*(( void**) _temp5466) ==  Cyc_Exp_tok){ _LL5473:
_temp5472=(( struct Cyc_Exp_tok_struct*) _temp5466)->f1; goto _LL5469;} else{
goto _LL5470;} _LL5470: goto _LL5471; _LL5469: yyzzz= _temp5472; goto _LL5467;
_LL5471:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5467:;} yyzzz;});
_temp5465->body= Cyc_Absyn_fallthru_stmt( 0, 0); _temp5465->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp5465;}); _temp5456->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5457=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5463; _LL5459: if(*(( void**) _temp5457) ==  Cyc_SwitchCClauseList_tok){
_LL5464: _temp5463=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp5457)->f1;
goto _LL5460;} else{ goto _LL5461;} _LL5461: goto _LL5462; _LL5460: yyzzz=
_temp5463; goto _LL5458; _LL5462:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL5458:;} yyzzz;}); _temp5456;}); _temp5455;}); _temp5454;}); break; case 257:
_LL5453: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp5475=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp5475[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp5476; _temp5476.tag=
Cyc_SwitchCClauseList_tok; _temp5476.f1=({ struct Cyc_List_List* _temp5477=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp5477->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp5486=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp5486->cnst_exp=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5495= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5501; _LL5497: if(*(( void**) _temp5495) ==  Cyc_Exp_tok){ _LL5502:
_temp5501=(( struct Cyc_Exp_tok_struct*) _temp5495)->f1; goto _LL5498;} else{
goto _LL5499;} _LL5499: goto _LL5500; _LL5498: yyzzz= _temp5501; goto _LL5496;
_LL5500:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5496:;} yyzzz;});
_temp5486->body= Cyc_Absyn_seq_stmt(({ struct Cyc_Absyn_Stmt* yyzzz;{ void*
_temp5487= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Stmt* _temp5493; _LL5489: if(*(( void**) _temp5487) ==  Cyc_Stmt_tok){
_LL5494: _temp5493=(( struct Cyc_Stmt_tok_struct*) _temp5487)->f1; goto _LL5490;}
else{ goto _LL5491;} _LL5491: goto _LL5492; _LL5490: yyzzz= _temp5493; goto
_LL5488; _LL5492:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5488:;} yyzzz;}),
Cyc_Absyn_fallthru_stmt( 0, 0), 0); _temp5486->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp5486;}); _temp5477->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5478=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5484; _LL5480: if(*(( void**) _temp5478) ==  Cyc_SwitchCClauseList_tok){
_LL5485: _temp5484=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp5478)->f1;
goto _LL5481;} else{ goto _LL5482;} _LL5482: goto _LL5483; _LL5481: yyzzz=
_temp5484; goto _LL5479; _LL5483:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL5479:;} yyzzz;}); _temp5477;}); _temp5476;}); _temp5475;}); break; case 258:
_LL5474: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5504=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5504[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5505; _temp5505.tag= Cyc_Stmt_tok; _temp5505.f1= Cyc_Absyn_while_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5506= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5512; _LL5508: if(*(( void**) _temp5506) ==  Cyc_Exp_tok){ _LL5513:
_temp5512=(( struct Cyc_Exp_tok_struct*) _temp5506)->f1; goto _LL5509;} else{
goto _LL5510;} _LL5510: goto _LL5511; _LL5509: yyzzz= _temp5512; goto _LL5507;
_LL5511:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5507:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5514= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5520; _LL5516: if(*(( void**) _temp5514) ==  Cyc_Stmt_tok){ _LL5521:
_temp5520=(( struct Cyc_Stmt_tok_struct*) _temp5514)->f1; goto _LL5517;} else{
goto _LL5518;} _LL5518: goto _LL5519; _LL5517: yyzzz= _temp5520; goto _LL5515;
_LL5519:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5515:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5505;}); _temp5504;}); break; case 259: _LL5503: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5523=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5523[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5524; _temp5524.tag= Cyc_Stmt_tok; _temp5524.f1= Cyc_Absyn_do_stmt(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5525= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Stmt*
_temp5531; _LL5527: if(*(( void**) _temp5525) ==  Cyc_Stmt_tok){ _LL5532:
_temp5531=(( struct Cyc_Stmt_tok_struct*) _temp5525)->f1; goto _LL5528;} else{
goto _LL5529;} _LL5529: goto _LL5530; _LL5528: yyzzz= _temp5531; goto _LL5526;
_LL5530:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5526:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5533= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5539; _LL5535: if(*(( void**) _temp5533) ==  Cyc_Exp_tok){ _LL5540:
_temp5539=(( struct Cyc_Exp_tok_struct*) _temp5533)->f1; goto _LL5536;} else{
goto _LL5537;} _LL5537: goto _LL5538; _LL5536: yyzzz= _temp5539; goto _LL5534;
_LL5538:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5534:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5524;}); _temp5523;}); break; case 260: _LL5522: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5542=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5542[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5543; _temp5543.tag= Cyc_Stmt_tok; _temp5543.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0),({ struct Cyc_Absyn_Stmt*
yyzzz;{ void* _temp5544= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Stmt* _temp5550; _LL5546: if(*(( void**)
_temp5544) ==  Cyc_Stmt_tok){ _LL5551: _temp5550=(( struct Cyc_Stmt_tok_struct*)
_temp5544)->f1; goto _LL5547;} else{ goto _LL5548;} _LL5548: goto _LL5549;
_LL5547: yyzzz= _temp5550; goto _LL5545; _LL5549:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL5545:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5543;});
_temp5542;}); break; case 261: _LL5541: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5553=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5553[ 0]=({ struct Cyc_Stmt_tok_struct _temp5554; _temp5554.tag= Cyc_Stmt_tok;
_temp5554.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5555= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5561; _LL5557: if(*(( void**) _temp5555) ==  Cyc_Exp_tok){ _LL5562:
_temp5561=(( struct Cyc_Exp_tok_struct*) _temp5555)->f1; goto _LL5558;} else{
goto _LL5559;} _LL5559: goto _LL5560; _LL5558: yyzzz= _temp5561; goto _LL5556;
_LL5560:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5556:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5563= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5569; _LL5565: if(*(( void**) _temp5563) ==  Cyc_Stmt_tok){ _LL5570:
_temp5569=(( struct Cyc_Stmt_tok_struct*) _temp5563)->f1; goto _LL5566;} else{
goto _LL5567;} _LL5567: goto _LL5568; _LL5566: yyzzz= _temp5569; goto _LL5564;
_LL5568:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5564:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5554;}); _temp5553;}); break; case 262: _LL5552: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5572=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5572[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5573; _temp5573.tag= Cyc_Stmt_tok; _temp5573.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5574= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5580; _LL5576: if(*(( void**) _temp5574) ==  Cyc_Exp_tok){ _LL5581:
_temp5580=(( struct Cyc_Exp_tok_struct*) _temp5574)->f1; goto _LL5577;} else{
goto _LL5578;} _LL5578: goto _LL5579; _LL5577: yyzzz= _temp5580; goto _LL5575;
_LL5579:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5575:;} yyzzz;}), Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5582= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5588; _LL5584: if(*(( void**) _temp5582) ==  Cyc_Stmt_tok){ _LL5589:
_temp5588=(( struct Cyc_Stmt_tok_struct*) _temp5582)->f1; goto _LL5585;} else{
goto _LL5586;} _LL5586: goto _LL5587; _LL5585: yyzzz= _temp5588; goto _LL5583;
_LL5587:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5583:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5573;}); _temp5572;}); break; case 263: _LL5571: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5591=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5591[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5592; _temp5592.tag= Cyc_Stmt_tok; _temp5592.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5593= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5599; _LL5595: if(*(( void**) _temp5593) ==  Cyc_Exp_tok){ _LL5600:
_temp5599=(( struct Cyc_Exp_tok_struct*) _temp5593)->f1; goto _LL5596;} else{
goto _LL5597;} _LL5597: goto _LL5598; _LL5596: yyzzz= _temp5599; goto _LL5594;
_LL5598:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5594:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5601= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5607; _LL5603: if(*(( void**) _temp5601) ==  Cyc_Exp_tok){ _LL5608:
_temp5607=(( struct Cyc_Exp_tok_struct*) _temp5601)->f1; goto _LL5604;} else{
goto _LL5605;} _LL5605: goto _LL5606; _LL5604: yyzzz= _temp5607; goto _LL5602;
_LL5606:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5602:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5609= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5615; _LL5611: if(*(( void**) _temp5609) ==  Cyc_Stmt_tok){ _LL5616:
_temp5615=(( struct Cyc_Stmt_tok_struct*) _temp5609)->f1; goto _LL5612;} else{
goto _LL5613;} _LL5613: goto _LL5614; _LL5612: yyzzz= _temp5615; goto _LL5610;
_LL5614:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5610:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5592;}); _temp5591;}); break; case 264: _LL5590: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5618=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5618[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5619; _temp5619.tag= Cyc_Stmt_tok; _temp5619.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5620= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5626; _LL5622: if(*(( void**) _temp5620) ==  Cyc_Exp_tok){ _LL5627:
_temp5626=(( struct Cyc_Exp_tok_struct*) _temp5620)->f1; goto _LL5623;} else{
goto _LL5624;} _LL5624: goto _LL5625; _LL5623: yyzzz= _temp5626; goto _LL5621;
_LL5625:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5621:;} yyzzz;}), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5628=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5634; _LL5630: if(*(( void**) _temp5628) ==  Cyc_Stmt_tok){ _LL5635:
_temp5634=(( struct Cyc_Stmt_tok_struct*) _temp5628)->f1; goto _LL5631;} else{
goto _LL5632;} _LL5632: goto _LL5633; _LL5631: yyzzz= _temp5634; goto _LL5629;
_LL5633:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5629:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5619;}); _temp5618;}); break; case 265: _LL5617: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5637=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5637[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5638; _temp5638.tag= Cyc_Stmt_tok; _temp5638.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5639= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Exp*
_temp5645; _LL5641: if(*(( void**) _temp5639) ==  Cyc_Exp_tok){ _LL5646:
_temp5645=(( struct Cyc_Exp_tok_struct*) _temp5639)->f1; goto _LL5642;} else{
goto _LL5643;} _LL5643: goto _LL5644; _LL5642: yyzzz= _temp5645; goto _LL5640;
_LL5644:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5640:;} yyzzz;}), Cyc_Absyn_true_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5647= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5653; _LL5649: if(*(( void**) _temp5647) ==  Cyc_Exp_tok){ _LL5654:
_temp5653=(( struct Cyc_Exp_tok_struct*) _temp5647)->f1; goto _LL5650;} else{
goto _LL5651;} _LL5651: goto _LL5652; _LL5650: yyzzz= _temp5653; goto _LL5648;
_LL5652:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5648:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5655= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5661; _LL5657: if(*(( void**) _temp5655) ==  Cyc_Stmt_tok){ _LL5662:
_temp5661=(( struct Cyc_Stmt_tok_struct*) _temp5655)->f1; goto _LL5658;} else{
goto _LL5659;} _LL5659: goto _LL5660; _LL5658: yyzzz= _temp5661; goto _LL5656;
_LL5660:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5656:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5638;}); _temp5637;}); break; case 266: _LL5636: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5664=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5664[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5665; _temp5665.tag= Cyc_Stmt_tok; _temp5665.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5666= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Exp*
_temp5672; _LL5668: if(*(( void**) _temp5666) ==  Cyc_Exp_tok){ _LL5673:
_temp5672=(( struct Cyc_Exp_tok_struct*) _temp5666)->f1; goto _LL5669;} else{
goto _LL5670;} _LL5670: goto _LL5671; _LL5669: yyzzz= _temp5672; goto _LL5667;
_LL5671:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5667:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5674= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5680; _LL5676: if(*(( void**) _temp5674) ==  Cyc_Exp_tok){ _LL5681:
_temp5680=(( struct Cyc_Exp_tok_struct*) _temp5674)->f1; goto _LL5677;} else{
goto _LL5678;} _LL5678: goto _LL5679; _LL5677: yyzzz= _temp5680; goto _LL5675;
_LL5679:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5675:;} yyzzz;}), Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5682= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5688; _LL5684: if(*(( void**) _temp5682) ==  Cyc_Stmt_tok){ _LL5689:
_temp5688=(( struct Cyc_Stmt_tok_struct*) _temp5682)->f1; goto _LL5685;} else{
goto _LL5686;} _LL5686: goto _LL5687; _LL5685: yyzzz= _temp5688; goto _LL5683;
_LL5687:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5683:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5665;}); _temp5664;}); break; case 267: _LL5663: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5691=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5691[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5692; _temp5692.tag= Cyc_Stmt_tok; _temp5692.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5693= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]; struct Cyc_Absyn_Exp*
_temp5699; _LL5695: if(*(( void**) _temp5693) ==  Cyc_Exp_tok){ _LL5700:
_temp5699=(( struct Cyc_Exp_tok_struct*) _temp5693)->f1; goto _LL5696;} else{
goto _LL5697;} _LL5697: goto _LL5698; _LL5696: yyzzz= _temp5699; goto _LL5694;
_LL5698:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5694:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5701= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5707; _LL5703: if(*(( void**) _temp5701) ==  Cyc_Exp_tok){ _LL5708:
_temp5707=(( struct Cyc_Exp_tok_struct*) _temp5701)->f1; goto _LL5704;} else{
goto _LL5705;} _LL5705: goto _LL5706; _LL5704: yyzzz= _temp5707; goto _LL5702;
_LL5706:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5702:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5709= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5715; _LL5711: if(*(( void**) _temp5709) ==  Cyc_Exp_tok){ _LL5716:
_temp5715=(( struct Cyc_Exp_tok_struct*) _temp5709)->f1; goto _LL5712;} else{
goto _LL5713;} _LL5713: goto _LL5714; _LL5712: yyzzz= _temp5715; goto _LL5710;
_LL5714:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5710:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5717= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5723; _LL5719: if(*(( void**) _temp5717) ==  Cyc_Stmt_tok){ _LL5724:
_temp5723=(( struct Cyc_Stmt_tok_struct*) _temp5717)->f1; goto _LL5720;} else{
goto _LL5721;} _LL5721: goto _LL5722; _LL5720: yyzzz= _temp5723; goto _LL5718;
_LL5722:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5718:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5692;}); _temp5691;}); break; case 268: _LL5690: { struct Cyc_List_List*
_temp5726=({ struct Cyc_List_List* yyzzz;{ void* _temp5738= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp5744; _LL5740: if(*(( void**) _temp5738) ==  Cyc_DeclList_tok){ _LL5745:
_temp5744=(( struct Cyc_DeclList_tok_struct*) _temp5738)->f1; goto _LL5741;}
else{ goto _LL5742;} _LL5742: goto _LL5743; _LL5741: yyzzz= _temp5744; goto
_LL5739; _LL5743:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5739:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5727= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0),({ struct Cyc_Absyn_Stmt*
yyzzz;{ void* _temp5730= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Stmt* _temp5736; _LL5732: if(*(( void**)
_temp5730) ==  Cyc_Stmt_tok){ _LL5737: _temp5736=(( struct Cyc_Stmt_tok_struct*)
_temp5730)->f1; goto _LL5733;} else{ goto _LL5734;} _LL5734: goto _LL5735;
_LL5733: yyzzz= _temp5736; goto _LL5731; _LL5735:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL5731:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5728=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5728[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5729; _temp5729.tag= Cyc_Stmt_tok; _temp5729.f1= Cyc_Parse_flatten_declarations(
_temp5726, _temp5727); _temp5729;}); _temp5728;}); break;} case 269: _LL5725: {
struct Cyc_List_List* _temp5747=({ struct Cyc_List_List* yyzzz;{ void* _temp5767=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_List_List*
_temp5773; _LL5769: if(*(( void**) _temp5767) ==  Cyc_DeclList_tok){ _LL5774:
_temp5773=(( struct Cyc_DeclList_tok_struct*) _temp5767)->f1; goto _LL5770;}
else{ goto _LL5771;} _LL5771: goto _LL5772; _LL5770: yyzzz= _temp5773; goto
_LL5768; _LL5772:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5768:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5748= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5751= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5757; _LL5753: if(*(( void**) _temp5751) ==  Cyc_Exp_tok){ _LL5758:
_temp5757=(( struct Cyc_Exp_tok_struct*) _temp5751)->f1; goto _LL5754;} else{
goto _LL5755;} _LL5755: goto _LL5756; _LL5754: yyzzz= _temp5757; goto _LL5752;
_LL5756:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5752:;} yyzzz;}), Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5759= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5765; _LL5761: if(*(( void**) _temp5759) ==  Cyc_Stmt_tok){ _LL5766:
_temp5765=(( struct Cyc_Stmt_tok_struct*) _temp5759)->f1; goto _LL5762;} else{
goto _LL5763;} _LL5763: goto _LL5764; _LL5762: yyzzz= _temp5765; goto _LL5760;
_LL5764:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5760:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5749=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5749[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5750; _temp5750.tag= Cyc_Stmt_tok; _temp5750.f1= Cyc_Parse_flatten_declarations(
_temp5747, _temp5748); _temp5750;}); _temp5749;}); break;} case 270: _LL5746: {
struct Cyc_List_List* _temp5776=({ struct Cyc_List_List* yyzzz;{ void* _temp5796=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_List_List*
_temp5802; _LL5798: if(*(( void**) _temp5796) ==  Cyc_DeclList_tok){ _LL5803:
_temp5802=(( struct Cyc_DeclList_tok_struct*) _temp5796)->f1; goto _LL5799;}
else{ goto _LL5800;} _LL5800: goto _LL5801; _LL5799: yyzzz= _temp5802; goto
_LL5797; _LL5801:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5797:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5777= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5780=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5786; _LL5782: if(*(( void**) _temp5780) ==  Cyc_Exp_tok){ _LL5787:
_temp5786=(( struct Cyc_Exp_tok_struct*) _temp5780)->f1; goto _LL5783;} else{
goto _LL5784;} _LL5784: goto _LL5785; _LL5783: yyzzz= _temp5786; goto _LL5781;
_LL5785:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5781:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5788= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5794; _LL5790: if(*(( void**) _temp5788) ==  Cyc_Stmt_tok){ _LL5795:
_temp5794=(( struct Cyc_Stmt_tok_struct*) _temp5788)->f1; goto _LL5791;} else{
goto _LL5792;} _LL5792: goto _LL5793; _LL5791: yyzzz= _temp5794; goto _LL5789;
_LL5793:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5789:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5778=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5778[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5779; _temp5779.tag= Cyc_Stmt_tok; _temp5779.f1= Cyc_Parse_flatten_declarations(
_temp5776, _temp5777); _temp5779;}); _temp5778;}); break;} case 271: _LL5775: {
struct Cyc_List_List* _temp5805=({ struct Cyc_List_List* yyzzz;{ void* _temp5833=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_List_List*
_temp5839; _LL5835: if(*(( void**) _temp5833) ==  Cyc_DeclList_tok){ _LL5840:
_temp5839=(( struct Cyc_DeclList_tok_struct*) _temp5833)->f1; goto _LL5836;}
else{ goto _LL5837;} _LL5837: goto _LL5838; _LL5836: yyzzz= _temp5839; goto
_LL5834; _LL5838:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5834:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5806= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5809= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5815; _LL5811: if(*(( void**) _temp5809) ==  Cyc_Exp_tok){ _LL5816:
_temp5815=(( struct Cyc_Exp_tok_struct*) _temp5809)->f1; goto _LL5812;} else{
goto _LL5813;} _LL5813: goto _LL5814; _LL5812: yyzzz= _temp5815; goto _LL5810;
_LL5814:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5810:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5817= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5823; _LL5819: if(*(( void**) _temp5817) ==  Cyc_Exp_tok){ _LL5824:
_temp5823=(( struct Cyc_Exp_tok_struct*) _temp5817)->f1; goto _LL5820;} else{
goto _LL5821;} _LL5821: goto _LL5822; _LL5820: yyzzz= _temp5823; goto _LL5818;
_LL5822:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5818:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5825= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5831; _LL5827: if(*(( void**) _temp5825) ==  Cyc_Stmt_tok){ _LL5832:
_temp5831=(( struct Cyc_Stmt_tok_struct*) _temp5825)->f1; goto _LL5828;} else{
goto _LL5829;} _LL5829: goto _LL5830; _LL5828: yyzzz= _temp5831; goto _LL5826;
_LL5830:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5826:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5807=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5807[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5808; _temp5808.tag= Cyc_Stmt_tok; _temp5808.f1= Cyc_Parse_flatten_declarations(
_temp5805, _temp5806); _temp5808;}); _temp5807;}); break;} case 272: _LL5804:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5842=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5842[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5843; _temp5843.tag= Cyc_Stmt_tok; _temp5843.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp5844=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp5844[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp5845= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tagged_arr
_temp5851; _LL5847: if(*(( void**) _temp5845) ==  Cyc_String_tok){ _LL5852:
_temp5851=(( struct Cyc_String_tok_struct*) _temp5845)->f1; goto _LL5848;} else{
goto _LL5849;} _LL5849: goto _LL5850; _LL5848: yyzzz= _temp5851; goto _LL5846;
_LL5850:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL5846:;} yyzzz;});
_temp5844;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)); _temp5843;}); _temp5842;}); break;
case 273: _LL5841: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5854=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5854[ 0]=({ struct Cyc_Stmt_tok_struct _temp5855; _temp5855.tag= Cyc_Stmt_tok;
_temp5855.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5855;}); _temp5854;}); break; case 274: _LL5853: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5857=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5857[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5858; _temp5858.tag= Cyc_Stmt_tok; _temp5858.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5858;}); _temp5857;}); break; case 275: _LL5856: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5860=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5860[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5861; _temp5861.tag= Cyc_Stmt_tok; _temp5861.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp5861;}); _temp5860;}); break; case 276:
_LL5859: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5863=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5863[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5864; _temp5864.tag= Cyc_Stmt_tok; _temp5864.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5865= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp5871; _LL5867: if(*(( void**) _temp5865) ==  Cyc_Exp_tok){ _LL5872:
_temp5871=(( struct Cyc_Exp_tok_struct*) _temp5865)->f1; goto _LL5868;} else{
goto _LL5869;} _LL5869: goto _LL5870; _LL5868: yyzzz= _temp5871; goto _LL5866;
_LL5870:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5866:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5864;}); _temp5863;}); break; case 277: _LL5862: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5874=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5874[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5875; _temp5875.tag= Cyc_Stmt_tok; _temp5875.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp5875;}); _temp5874;}); break; case 278:
_LL5873: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5877=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5877[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5878; _temp5878.tag= Cyc_Stmt_tok; _temp5878.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp5878;}); _temp5877;}); break; case 279:
_LL5876: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5880=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5880[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5881; _temp5881.tag= Cyc_Stmt_tok; _temp5881.f1= Cyc_Absyn_fallthru_stmt(({
struct Cyc_List_List* yyzzz;{ void* _temp5882= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp5888; _LL5884: if(*(( void**) _temp5882) ==  Cyc_ExpList_tok){ _LL5889:
_temp5888=(( struct Cyc_ExpList_tok_struct*) _temp5882)->f1; goto _LL5885;}
else{ goto _LL5886;} _LL5886: goto _LL5887; _LL5885: yyzzz= _temp5888; goto
_LL5883; _LL5887:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL5883:;}
yyzzz;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)); _temp5881;}); _temp5880;}); break;
case 280: _LL5879: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5891=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5891[ 0]=({ struct Cyc_Pattern_tok_struct _temp5892; _temp5892.tag= Cyc_Pattern_tok;
_temp5892.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5892;});
_temp5891;}); break; case 281: _LL5890: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 282:
_LL5893: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5895=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5895[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5896; _temp5896.tag= Cyc_Pattern_tok; _temp5896.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5897=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5897[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5898; _temp5898.tag= Cyc_Absyn_Int_p; _temp5898.f1=( void*)(*({ struct
_tuple15* yyzzz;{ void* _temp5899= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple15* _temp5905; _LL5901: if(*(( void**) _temp5899)
==  Cyc_Int_tok){ _LL5906: _temp5905=(( struct Cyc_Int_tok_struct*) _temp5899)->f1;
goto _LL5902;} else{ goto _LL5903;} _LL5903: goto _LL5904; _LL5902: yyzzz=
_temp5905; goto _LL5900; _LL5904:( int) _throw(( void*)& Cyc_yyfail_Int_tok);
_LL5900:;} yyzzz;})).f1; _temp5898.f2=(*({ struct _tuple15* yyzzz;{ void*
_temp5907= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
_tuple15* _temp5913; _LL5909: if(*(( void**) _temp5907) ==  Cyc_Int_tok){
_LL5914: _temp5913=(( struct Cyc_Int_tok_struct*) _temp5907)->f1; goto _LL5910;}
else{ goto _LL5911;} _LL5911: goto _LL5912; _LL5910: yyzzz= _temp5913; goto
_LL5908; _LL5912:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL5908:;} yyzzz;})).f2;
_temp5898;}); _temp5897;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5896;});
_temp5895;}); break; case 283: _LL5894: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5916=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5916[ 0]=({ struct Cyc_Pattern_tok_struct _temp5917; _temp5917.tag= Cyc_Pattern_tok;
_temp5917.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5918=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5918[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5919; _temp5919.tag= Cyc_Absyn_Int_p;
_temp5919.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp5919.f2= -(*({ struct
_tuple15* yyzzz;{ void* _temp5920= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple15* _temp5926; _LL5922: if(*(( void**) _temp5920)
==  Cyc_Int_tok){ _LL5927: _temp5926=(( struct Cyc_Int_tok_struct*) _temp5920)->f1;
goto _LL5923;} else{ goto _LL5924;} _LL5924: goto _LL5925; _LL5923: yyzzz=
_temp5926; goto _LL5921; _LL5925:( int) _throw(( void*)& Cyc_yyfail_Int_tok);
_LL5921:;} yyzzz;})).f2; _temp5919;}); _temp5918;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5917;}); _temp5916;}); break; case 284: _LL5915: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp5929=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp5929[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5930; _temp5930.tag= Cyc_Pattern_tok; _temp5930.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp5931=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5931[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp5932; _temp5932.tag= Cyc_Absyn_Float_p; _temp5932.f1=({
struct _tagged_arr yyzzz;{ void* _temp5933= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp5939; _LL5935: if(*(( void**)
_temp5933) ==  Cyc_String_tok){ _LL5940: _temp5939=(( struct Cyc_String_tok_struct*)
_temp5933)->f1; goto _LL5936;} else{ goto _LL5937;} _LL5937: goto _LL5938;
_LL5936: yyzzz= _temp5939; goto _LL5934; _LL5938:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5934:;} yyzzz;}); _temp5932;}); _temp5931;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5930;});
_temp5929;}); break; case 285: _LL5928: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5942=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5942[ 0]=({ struct Cyc_Pattern_tok_struct _temp5943; _temp5943.tag= Cyc_Pattern_tok;
_temp5943.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp5944=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp5944[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp5945; _temp5945.tag= Cyc_Absyn_Char_p; _temp5945.f1=({ unsigned char yyzzz;{
void* _temp5946= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
unsigned char _temp5952; _LL5948: if(*(( void**) _temp5946) ==  Cyc_Char_tok){
_LL5953: _temp5952=(( struct Cyc_Char_tok_struct*) _temp5946)->f1; goto _LL5949;}
else{ goto _LL5950;} _LL5950: goto _LL5951; _LL5949: yyzzz= _temp5952; goto
_LL5947; _LL5951:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL5947:;} yyzzz;});
_temp5945;}); _temp5944;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5943;});
_temp5942;}); break; case 286: _LL5941: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5955=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5955[ 0]=({ struct Cyc_Pattern_tok_struct _temp5956; _temp5956.tag= Cyc_Pattern_tok;
_temp5956.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5956;});
_temp5955;}); break; case 287: _LL5954: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5958=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5958[ 0]=({ struct Cyc_Pattern_tok_struct _temp5959; _temp5959.tag= Cyc_Pattern_tok;
_temp5959.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp5960=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp5960[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp5961; _temp5961.tag=
Cyc_Absyn_UnknownId_p; _temp5961.f1=({ struct _tuple1* yyzzz;{ void* _temp5962=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp5968; _LL5964: if(*(( void**) _temp5962) ==  Cyc_QualId_tok){ _LL5969:
_temp5968=(( struct Cyc_QualId_tok_struct*) _temp5962)->f1; goto _LL5965;} else{
goto _LL5966;} _LL5966: goto _LL5967; _LL5965: yyzzz= _temp5968; goto _LL5963;
_LL5967:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL5963:;} yyzzz;});
_temp5961;}); _temp5960;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5959;});
_temp5958;}); break; case 288: _LL5957: { struct Cyc_List_List* _temp5971=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp5992= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp5998; _LL5994: if(*(( void**) _temp5992) ==  Cyc_TypeList_tok){ _LL5999:
_temp5998=(( struct Cyc_TypeList_tok_struct*) _temp5992)->f1; goto _LL5995;}
else{ goto _LL5996;} _LL5996: goto _LL5997; _LL5995: yyzzz= _temp5998; goto
_LL5993; _LL5997:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL5993:;}
yyzzz;})); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5972=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5972[ 0]=({ struct Cyc_Pattern_tok_struct _temp5973; _temp5973.tag= Cyc_Pattern_tok;
_temp5973.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp5974=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp5974[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp5975; _temp5975.tag= Cyc_Absyn_UnknownCall_p; _temp5975.f1=({ struct
_tuple1* yyzzz;{ void* _temp5976= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp5982; _LL5978: if(*(( void**)
_temp5976) ==  Cyc_QualId_tok){ _LL5983: _temp5982=(( struct Cyc_QualId_tok_struct*)
_temp5976)->f1; goto _LL5979;} else{ goto _LL5980;} _LL5980: goto _LL5981;
_LL5979: yyzzz= _temp5982; goto _LL5977; _LL5981:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL5977:;} yyzzz;}); _temp5975.f2= _temp5971; _temp5975.f3=({ struct Cyc_List_List*
yyzzz;{ void* _temp5984= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp5990; _LL5986: if(*(( void**)
_temp5984) ==  Cyc_PatternList_tok){ _LL5991: _temp5990=(( struct Cyc_PatternList_tok_struct*)
_temp5984)->f1; goto _LL5987;} else{ goto _LL5988;} _LL5988: goto _LL5989;
_LL5987: yyzzz= _temp5990; goto _LL5985; _LL5989:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL5985:;} yyzzz;}); _temp5975;}); _temp5974;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5973;}); _temp5972;}); break;} case 289: _LL5970: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6001=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6001[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6002; _temp6002.tag= Cyc_Pattern_tok; _temp6002.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6003=( struct Cyc_Absyn_Tuple_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6003[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6004; _temp6004.tag= Cyc_Absyn_Tuple_p; _temp6004.f1=({
struct Cyc_List_List* yyzzz;{ void* _temp6005= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp6011; _LL6007: if(*(( void**) _temp6005) ==  Cyc_PatternList_tok){ _LL6012:
_temp6011=(( struct Cyc_PatternList_tok_struct*) _temp6005)->f1; goto _LL6008;}
else{ goto _LL6009;} _LL6009: goto _LL6010; _LL6008: yyzzz= _temp6011; goto
_LL6006; _LL6010:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok); _LL6006:;}
yyzzz;}); _temp6004;}); _temp6003;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6002;});
_temp6001;}); break; case 290: _LL6000: { struct Cyc_List_List* _temp6014=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp6027= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp6033; _LL6029: if(*(( void**) _temp6027) ==  Cyc_TypeList_tok){ _LL6034:
_temp6033=(( struct Cyc_TypeList_tok_struct*) _temp6027)->f1; goto _LL6030;}
else{ goto _LL6031;} _LL6031: goto _LL6032; _LL6030: yyzzz= _temp6033; goto
_LL6028; _LL6032:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL6028:;}
yyzzz;})); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp6015=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6015[ 0]=({ struct Cyc_Pattern_tok_struct _temp6016; _temp6016.tag= Cyc_Pattern_tok;
_temp6016.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6017=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6017[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6018; _temp6018.tag= Cyc_Absyn_UnknownFields_p; _temp6018.f1=({ struct
_tuple1* yyzzz;{ void* _temp6019= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct _tuple1* _temp6025; _LL6021: if(*(( void**)
_temp6019) ==  Cyc_QualId_tok){ _LL6026: _temp6025=(( struct Cyc_QualId_tok_struct*)
_temp6019)->f1; goto _LL6022;} else{ goto _LL6023;} _LL6023: goto _LL6024;
_LL6022: yyzzz= _temp6025; goto _LL6020; _LL6024:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL6020:;} yyzzz;}); _temp6018.f2= _temp6014; _temp6018.f3= 0; _temp6018;});
_temp6017;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp6016;}); _temp6015;}); break;} case
291: _LL6013: { struct Cyc_List_List* _temp6036=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp6057= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp6063; _LL6059: if(*(( void**) _temp6057) ==  Cyc_TypeList_tok){ _LL6064:
_temp6063=(( struct Cyc_TypeList_tok_struct*) _temp6057)->f1; goto _LL6060;}
else{ goto _LL6061;} _LL6061: goto _LL6062; _LL6060: yyzzz= _temp6063; goto
_LL6058; _LL6062:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL6058:;}
yyzzz;})); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp6037=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6037[ 0]=({ struct Cyc_Pattern_tok_struct _temp6038; _temp6038.tag= Cyc_Pattern_tok;
_temp6038.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6039=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6039[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6040; _temp6040.tag= Cyc_Absyn_UnknownFields_p; _temp6040.f1=({ struct
_tuple1* yyzzz;{ void* _temp6041= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp6047; _LL6043: if(*(( void**)
_temp6041) ==  Cyc_QualId_tok){ _LL6048: _temp6047=(( struct Cyc_QualId_tok_struct*)
_temp6041)->f1; goto _LL6044;} else{ goto _LL6045;} _LL6045: goto _LL6046;
_LL6044: yyzzz= _temp6047; goto _LL6042; _LL6046:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL6042:;} yyzzz;}); _temp6040.f2= _temp6036; _temp6040.f3=({ struct Cyc_List_List*
yyzzz;{ void* _temp6049= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp6055; _LL6051: if(*(( void**)
_temp6049) ==  Cyc_FieldPatternList_tok){ _LL6056: _temp6055=(( struct Cyc_FieldPatternList_tok_struct*)
_temp6049)->f1; goto _LL6052;} else{ goto _LL6053;} _LL6053: goto _LL6054;
_LL6052: yyzzz= _temp6055; goto _LL6050; _LL6054:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL6050:;} yyzzz;}); _temp6040;}); _temp6039;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6038;}); _temp6037;}); break;} case 292: _LL6035: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6066=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6066[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6067; _temp6067.tag= Cyc_Pattern_tok; _temp6067.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6068=( struct Cyc_Absyn_Pointer_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6068[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6069; _temp6069.tag= Cyc_Absyn_Pointer_p;
_temp6069.f1=({ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp6070= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Pat*
_temp6076; _LL6072: if(*(( void**) _temp6070) ==  Cyc_Pattern_tok){ _LL6077:
_temp6076=(( struct Cyc_Pattern_tok_struct*) _temp6070)->f1; goto _LL6073;}
else{ goto _LL6074;} _LL6074: goto _LL6075; _LL6073: yyzzz= _temp6076; goto
_LL6071; _LL6075:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL6071:;}
yyzzz;}); _temp6069;}); _temp6068;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6067;});
_temp6066;}); break; case 293: _LL6065: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6079=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6079[ 0]=({ struct Cyc_Pattern_tok_struct _temp6080; _temp6080.tag= Cyc_Pattern_tok;
_temp6080.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp6081=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp6081[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp6082; _temp6082.tag=
Cyc_Absyn_Reference_p; _temp6082.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp6083=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp6083->f1=(
void*) Cyc_Absyn_Loc_n; _temp6083->f2=({ struct _tagged_arr* _temp6084=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp6084[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp6085= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tagged_arr _temp6091; _LL6087: if(*(( void**) _temp6085)
==  Cyc_String_tok){ _LL6092: _temp6091=(( struct Cyc_String_tok_struct*)
_temp6085)->f1; goto _LL6088;} else{ goto _LL6089;} _LL6089: goto _LL6090;
_LL6088: yyzzz= _temp6091; goto _LL6086; _LL6090:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL6086:;} yyzzz;}); _temp6084;}); _temp6083;}),( void*) Cyc_Absyn_VoidType, 0);
_temp6082;}); _temp6081;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6080;});
_temp6079;}); break; case 294: _LL6078: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp6094=( struct Cyc_PatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6094[ 0]=({ struct Cyc_PatternList_tok_struct _temp6095; _temp6095.tag= Cyc_PatternList_tok;
_temp6095.f1= 0; _temp6095;}); _temp6094;}); break; case 295: _LL6093: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp6097=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6097[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6098; _temp6098.tag= Cyc_PatternList_tok;
_temp6098.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({
struct Cyc_List_List* yyzzz;{ void* _temp6099= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp6105; _LL6101: if(*(( void**) _temp6099) ==  Cyc_PatternList_tok){ _LL6106:
_temp6105=(( struct Cyc_PatternList_tok_struct*) _temp6099)->f1; goto _LL6102;}
else{ goto _LL6103;} _LL6103: goto _LL6104; _LL6102: yyzzz= _temp6105; goto
_LL6100; _LL6104:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok); _LL6100:;}
yyzzz;})); _temp6098;}); _temp6097;}); break; case 296: _LL6096: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp6108=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6108[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6109; _temp6109.tag= Cyc_PatternList_tok;
_temp6109.f1=({ struct Cyc_List_List* _temp6110=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp6110->hd=( void*)({ struct Cyc_Absyn_Pat*
yyzzz;{ void* _temp6111= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Pat* _temp6117; _LL6113: if(*(( void**)
_temp6111) ==  Cyc_Pattern_tok){ _LL6118: _temp6117=(( struct Cyc_Pattern_tok_struct*)
_temp6111)->f1; goto _LL6114;} else{ goto _LL6115;} _LL6115: goto _LL6116;
_LL6114: yyzzz= _temp6117; goto _LL6112; _LL6116:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL6112:;} yyzzz;}); _temp6110->tl= 0; _temp6110;}); _temp6109;}); _temp6108;});
break; case 297: _LL6107: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp6120=( struct Cyc_PatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6120[ 0]=({ struct Cyc_PatternList_tok_struct _temp6121; _temp6121.tag= Cyc_PatternList_tok;
_temp6121.f1=({ struct Cyc_List_List* _temp6122=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp6122->hd=( void*)({ struct Cyc_Absyn_Pat*
yyzzz;{ void* _temp6131= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Pat* _temp6137; _LL6133: if(*(( void**)
_temp6131) ==  Cyc_Pattern_tok){ _LL6138: _temp6137=(( struct Cyc_Pattern_tok_struct*)
_temp6131)->f1; goto _LL6134;} else{ goto _LL6135;} _LL6135: goto _LL6136;
_LL6134: yyzzz= _temp6137; goto _LL6132; _LL6136:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL6132:;} yyzzz;}); _temp6122->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp6123= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp6129; _LL6125: if(*(( void**) _temp6123) ==  Cyc_PatternList_tok){
_LL6130: _temp6129=(( struct Cyc_PatternList_tok_struct*) _temp6123)->f1; goto
_LL6126;} else{ goto _LL6127;} _LL6127: goto _LL6128; _LL6126: yyzzz= _temp6129;
goto _LL6124; _LL6128:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL6124:;} yyzzz;}); _temp6122;}); _temp6121;}); _temp6120;}); break; case 298:
_LL6119: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp6140=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6140[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6141; _temp6141.tag=
Cyc_FieldPattern_tok; _temp6141.f1=({ struct _tuple14* _temp6142=( struct
_tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp6142->f1= 0; _temp6142->f2=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp6143= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Pat*
_temp6149; _LL6145: if(*(( void**) _temp6143) ==  Cyc_Pattern_tok){ _LL6150:
_temp6149=(( struct Cyc_Pattern_tok_struct*) _temp6143)->f1; goto _LL6146;}
else{ goto _LL6147;} _LL6147: goto _LL6148; _LL6146: yyzzz= _temp6149; goto
_LL6144; _LL6148:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL6144:;}
yyzzz;}); _temp6142;}); _temp6141;}); _temp6140;}); break; case 299: _LL6139:
yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp6152=( struct Cyc_FieldPattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp6152[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp6153; _temp6153.tag= Cyc_FieldPattern_tok;
_temp6153.f1=({ struct _tuple14* _temp6154=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp6154->f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp6163= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp6169; _LL6165: if(*(( void**) _temp6163) ==  Cyc_DesignatorList_tok){
_LL6170: _temp6169=(( struct Cyc_DesignatorList_tok_struct*) _temp6163)->f1;
goto _LL6166;} else{ goto _LL6167;} _LL6167: goto _LL6168; _LL6166: yyzzz=
_temp6169; goto _LL6164; _LL6168:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL6164:;} yyzzz;}); _temp6154->f2=({ struct Cyc_Absyn_Pat* yyzzz;{ void*
_temp6155= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Pat* _temp6161; _LL6157: if(*(( void**) _temp6155) ==  Cyc_Pattern_tok){
_LL6162: _temp6161=(( struct Cyc_Pattern_tok_struct*) _temp6155)->f1; goto
_LL6158;} else{ goto _LL6159;} _LL6159: goto _LL6160; _LL6158: yyzzz= _temp6161;
goto _LL6156; _LL6160:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL6156:;}
yyzzz;}); _temp6154;}); _temp6153;}); _temp6152;}); break; case 300: _LL6151:
yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp6172=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6172[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6173; _temp6173.tag= Cyc_FieldPatternList_tok;
_temp6173.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({
struct Cyc_List_List* yyzzz;{ void* _temp6174= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp6180; _LL6176: if(*(( void**) _temp6174) ==  Cyc_FieldPatternList_tok){
_LL6181: _temp6180=(( struct Cyc_FieldPatternList_tok_struct*) _temp6174)->f1;
goto _LL6177;} else{ goto _LL6178;} _LL6178: goto _LL6179; _LL6177: yyzzz=
_temp6180; goto _LL6175; _LL6179:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL6175:;} yyzzz;})); _temp6173;}); _temp6172;}); break; case 301: _LL6171:
yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp6183=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6183[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6184; _temp6184.tag= Cyc_FieldPatternList_tok;
_temp6184.f1=({ struct Cyc_List_List* _temp6185=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp6185->hd=( void*)({ struct
_tuple14* yyzzz;{ void* _temp6186= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple14* _temp6192; _LL6188: if(*(( void**) _temp6186)
==  Cyc_FieldPattern_tok){ _LL6193: _temp6192=(( struct Cyc_FieldPattern_tok_struct*)
_temp6186)->f1; goto _LL6189;} else{ goto _LL6190;} _LL6190: goto _LL6191;
_LL6189: yyzzz= _temp6192; goto _LL6187; _LL6191:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL6187:;} yyzzz;}); _temp6185->tl= 0; _temp6185;}); _temp6184;}); _temp6183;});
break; case 302: _LL6182: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp6195=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp6195[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp6196; _temp6196.tag= Cyc_FieldPatternList_tok; _temp6196.f1=({ struct Cyc_List_List*
_temp6197=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp6197->hd=( void*)({ struct _tuple14* yyzzz;{ void* _temp6206= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple14*
_temp6212; _LL6208: if(*(( void**) _temp6206) ==  Cyc_FieldPattern_tok){ _LL6213:
_temp6212=(( struct Cyc_FieldPattern_tok_struct*) _temp6206)->f1; goto _LL6209;}
else{ goto _LL6210;} _LL6210: goto _LL6211; _LL6209: yyzzz= _temp6212; goto
_LL6207; _LL6211:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL6207:;}
yyzzz;}); _temp6197->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp6198= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp6204; _LL6200: if(*(( void**) _temp6198) ==  Cyc_FieldPatternList_tok){
_LL6205: _temp6204=(( struct Cyc_FieldPatternList_tok_struct*) _temp6198)->f1;
goto _LL6201;} else{ goto _LL6202;} _LL6202: goto _LL6203; _LL6201: yyzzz=
_temp6204; goto _LL6199; _LL6203:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL6199:;} yyzzz;}); _temp6197;}); _temp6196;}); _temp6195;}); break; case 303:
_LL6194: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL6214: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6216=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6216[ 0]=({ struct Cyc_Exp_tok_struct _temp6217; _temp6217.tag= Cyc_Exp_tok;
_temp6217.f1= Cyc_Absyn_seq_exp(({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6218=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6224; _LL6220: if(*(( void**) _temp6218) ==  Cyc_Exp_tok){ _LL6225:
_temp6224=(( struct Cyc_Exp_tok_struct*) _temp6218)->f1; goto _LL6221;} else{
goto _LL6222;} _LL6222: goto _LL6223; _LL6221: yyzzz= _temp6224; goto _LL6219;
_LL6223:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6219:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6226= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6232; _LL6228: if(*(( void**) _temp6226) ==  Cyc_Exp_tok){ _LL6233:
_temp6232=(( struct Cyc_Exp_tok_struct*) _temp6226)->f1; goto _LL6229;} else{
goto _LL6230;} _LL6230: goto _LL6231; _LL6229: yyzzz= _temp6232; goto _LL6227;
_LL6231:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6227:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6217;}); _temp6216;}); break; case 305: _LL6215: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL6234:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6236=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6236[ 0]=({ struct Cyc_Exp_tok_struct
_temp6237; _temp6237.tag= Cyc_Exp_tok; _temp6237.f1= Cyc_Absyn_assignop_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6238= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6244; _LL6240: if(*(( void**) _temp6238) ==  Cyc_Exp_tok){ _LL6245:
_temp6244=(( struct Cyc_Exp_tok_struct*) _temp6238)->f1; goto _LL6241;} else{
goto _LL6242;} _LL6242: goto _LL6243; _LL6241: yyzzz= _temp6244; goto _LL6239;
_LL6243:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6239:;} yyzzz;}),({
struct Cyc_Core_Opt* yyzzz;{ void* _temp6246= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Core_Opt*
_temp6252; _LL6248: if(*(( void**) _temp6246) ==  Cyc_Primopopt_tok){ _LL6253:
_temp6252=(( struct Cyc_Primopopt_tok_struct*) _temp6246)->f1; goto _LL6249;}
else{ goto _LL6250;} _LL6250: goto _LL6251; _LL6249: yyzzz= _temp6252; goto
_LL6247; _LL6251:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL6247:;}
yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6254= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6260; _LL6256: if(*(( void**) _temp6254) ==  Cyc_Exp_tok){ _LL6261:
_temp6260=(( struct Cyc_Exp_tok_struct*) _temp6254)->f1; goto _LL6257;} else{
goto _LL6258;} _LL6258: goto _LL6259; _LL6257: yyzzz= _temp6260; goto _LL6255;
_LL6259:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6255:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6237;}); _temp6236;}); break; case 307: _LL6235: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6263=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6263[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6264; _temp6264.tag= Cyc_Primopopt_tok; _temp6264.f1=
0; _temp6264;}); _temp6263;}); break; case 308: _LL6262: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6266=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6266[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6267; _temp6267.tag= Cyc_Primopopt_tok; _temp6267.f1=({
struct Cyc_Core_Opt* _temp6268=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6268->v=( void*)(( void*) Cyc_Absyn_Times); _temp6268;});
_temp6267;}); _temp6266;}); break; case 309: _LL6265: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6270=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6270[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6271; _temp6271.tag= Cyc_Primopopt_tok; _temp6271.f1=({
struct Cyc_Core_Opt* _temp6272=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6272->v=( void*)(( void*) Cyc_Absyn_Div); _temp6272;});
_temp6271;}); _temp6270;}); break; case 310: _LL6269: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6274=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6274[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6275; _temp6275.tag= Cyc_Primopopt_tok; _temp6275.f1=({
struct Cyc_Core_Opt* _temp6276=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6276->v=( void*)(( void*) Cyc_Absyn_Mod); _temp6276;});
_temp6275;}); _temp6274;}); break; case 311: _LL6273: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6278=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6278[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6279; _temp6279.tag= Cyc_Primopopt_tok; _temp6279.f1=({
struct Cyc_Core_Opt* _temp6280=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6280->v=( void*)(( void*) Cyc_Absyn_Plus); _temp6280;});
_temp6279;}); _temp6278;}); break; case 312: _LL6277: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6282=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6282[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6283; _temp6283.tag= Cyc_Primopopt_tok; _temp6283.f1=({
struct Cyc_Core_Opt* _temp6284=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6284->v=( void*)(( void*) Cyc_Absyn_Minus); _temp6284;});
_temp6283;}); _temp6282;}); break; case 313: _LL6281: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6286=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6286[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6287; _temp6287.tag= Cyc_Primopopt_tok; _temp6287.f1=({
struct Cyc_Core_Opt* _temp6288=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6288->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp6288;});
_temp6287;}); _temp6286;}); break; case 314: _LL6285: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6290=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6290[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6291; _temp6291.tag= Cyc_Primopopt_tok; _temp6291.f1=({
struct Cyc_Core_Opt* _temp6292=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6292->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp6292;});
_temp6291;}); _temp6290;}); break; case 315: _LL6289: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6294=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6294[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6295; _temp6295.tag= Cyc_Primopopt_tok; _temp6295.f1=({
struct Cyc_Core_Opt* _temp6296=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6296->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp6296;});
_temp6295;}); _temp6294;}); break; case 316: _LL6293: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6298=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6298[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6299; _temp6299.tag= Cyc_Primopopt_tok; _temp6299.f1=({
struct Cyc_Core_Opt* _temp6300=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6300->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp6300;});
_temp6299;}); _temp6298;}); break; case 317: _LL6297: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6302=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6302[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6303; _temp6303.tag= Cyc_Primopopt_tok; _temp6303.f1=({
struct Cyc_Core_Opt* _temp6304=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp6304->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp6304;});
_temp6303;}); _temp6302;}); break; case 318: _LL6301: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL6305:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6307=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6307[ 0]=({ struct Cyc_Exp_tok_struct
_temp6308; _temp6308.tag= Cyc_Exp_tok; _temp6308.f1= Cyc_Absyn_conditional_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6309= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp6315; _LL6311: if(*(( void**) _temp6309) ==  Cyc_Exp_tok){ _LL6316:
_temp6315=(( struct Cyc_Exp_tok_struct*) _temp6309)->f1; goto _LL6312;} else{
goto _LL6313;} _LL6313: goto _LL6314; _LL6312: yyzzz= _temp6315; goto _LL6310;
_LL6314:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6310:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6317= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6323; _LL6319: if(*(( void**) _temp6317) ==  Cyc_Exp_tok){ _LL6324:
_temp6323=(( struct Cyc_Exp_tok_struct*) _temp6317)->f1; goto _LL6320;} else{
goto _LL6321;} _LL6321: goto _LL6322; _LL6320: yyzzz= _temp6323; goto _LL6318;
_LL6322:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6318:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6325= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6331; _LL6327: if(*(( void**) _temp6325) ==  Cyc_Exp_tok){ _LL6332:
_temp6331=(( struct Cyc_Exp_tok_struct*) _temp6325)->f1; goto _LL6328;} else{
goto _LL6329;} _LL6329: goto _LL6330; _LL6328: yyzzz= _temp6331; goto _LL6326;
_LL6330:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6326:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6308;}); _temp6307;}); break; case 320: _LL6306: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6334=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6334[ 0]=({ struct Cyc_Exp_tok_struct
_temp6335; _temp6335.tag= Cyc_Exp_tok; _temp6335.f1= Cyc_Absyn_throw_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6336= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6342; _LL6338: if(*(( void**) _temp6336) ==  Cyc_Exp_tok){ _LL6343:
_temp6342=(( struct Cyc_Exp_tok_struct*) _temp6336)->f1; goto _LL6339;} else{
goto _LL6340;} _LL6340: goto _LL6341; _LL6339: yyzzz= _temp6342; goto _LL6337;
_LL6341:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6337:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6335;}); _temp6334;}); break; case 321: _LL6333: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6345=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6345[ 0]=({ struct Cyc_Exp_tok_struct
_temp6346; _temp6346.tag= Cyc_Exp_tok; _temp6346.f1= Cyc_Absyn_New_exp( 0,({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6347= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6353; _LL6349: if(*(( void**) _temp6347) ==  Cyc_Exp_tok){ _LL6354:
_temp6353=(( struct Cyc_Exp_tok_struct*) _temp6347)->f1; goto _LL6350;} else{
goto _LL6351;} _LL6351: goto _LL6352; _LL6350: yyzzz= _temp6353; goto _LL6348;
_LL6352:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6348:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp6346;}); _temp6345;}); break; case 322: _LL6344: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6356=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6356[ 0]=({ struct Cyc_Exp_tok_struct
_temp6357; _temp6357.tag= Cyc_Exp_tok; _temp6357.f1= Cyc_Absyn_New_exp( 0,({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6358= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6364; _LL6360: if(*(( void**) _temp6358) ==  Cyc_Exp_tok){ _LL6365:
_temp6364=(( struct Cyc_Exp_tok_struct*) _temp6358)->f1; goto _LL6361;} else{
goto _LL6362;} _LL6362: goto _LL6363; _LL6361: yyzzz= _temp6364; goto _LL6359;
_LL6363:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6359:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp6357;}); _temp6356;}); break; case 323: _LL6355: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6367=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6367[ 0]=({ struct Cyc_Exp_tok_struct
_temp6368; _temp6368.tag= Cyc_Exp_tok; _temp6368.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6369= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6375; _LL6371: if(*(( void**) _temp6369) ==  Cyc_Exp_tok){ _LL6376:
_temp6375=(( struct Cyc_Exp_tok_struct*) _temp6369)->f1; goto _LL6372;} else{
goto _LL6373;} _LL6373: goto _LL6374; _LL6372: yyzzz= _temp6375; goto _LL6370;
_LL6374:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6370:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6377= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6383; _LL6379: if(*(( void**) _temp6377) ==  Cyc_Exp_tok){ _LL6384:
_temp6383=(( struct Cyc_Exp_tok_struct*) _temp6377)->f1; goto _LL6380;} else{
goto _LL6381;} _LL6381: goto _LL6382; _LL6380: yyzzz= _temp6383; goto _LL6378;
_LL6382:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6378:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6368;}); _temp6367;}); break; case 324: _LL6366: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6386=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6386[ 0]=({ struct Cyc_Exp_tok_struct
_temp6387; _temp6387.tag= Cyc_Exp_tok; _temp6387.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6388= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6394; _LL6390: if(*(( void**) _temp6388) ==  Cyc_Exp_tok){ _LL6395:
_temp6394=(( struct Cyc_Exp_tok_struct*) _temp6388)->f1; goto _LL6391;} else{
goto _LL6392;} _LL6392: goto _LL6393; _LL6391: yyzzz= _temp6394; goto _LL6389;
_LL6393:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6389:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6396= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6402; _LL6398: if(*(( void**) _temp6396) ==  Cyc_Exp_tok){ _LL6403:
_temp6402=(( struct Cyc_Exp_tok_struct*) _temp6396)->f1; goto _LL6399;} else{
goto _LL6400;} _LL6400: goto _LL6401; _LL6399: yyzzz= _temp6402; goto _LL6397;
_LL6401:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6397:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6387;}); _temp6386;}); break; case 325: _LL6385: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL6404:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL6405: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6407=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6407[ 0]=({ struct Cyc_Exp_tok_struct
_temp6408; _temp6408.tag= Cyc_Exp_tok; _temp6408.f1= Cyc_Absyn_or_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6409= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6415; _LL6411: if(*((
void**) _temp6409) ==  Cyc_Exp_tok){ _LL6416: _temp6415=(( struct Cyc_Exp_tok_struct*)
_temp6409)->f1; goto _LL6412;} else{ goto _LL6413;} _LL6413: goto _LL6414;
_LL6412: yyzzz= _temp6415; goto _LL6410; _LL6414:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6410:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6417= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6423; _LL6419: if(*(( void**) _temp6417) ==  Cyc_Exp_tok){ _LL6424:
_temp6423=(( struct Cyc_Exp_tok_struct*) _temp6417)->f1; goto _LL6420;} else{
goto _LL6421;} _LL6421: goto _LL6422; _LL6420: yyzzz= _temp6423; goto _LL6418;
_LL6422:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6418:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6408;}); _temp6407;}); break; case 328: _LL6406: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL6425:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6427=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6427[ 0]=({ struct Cyc_Exp_tok_struct
_temp6428; _temp6428.tag= Cyc_Exp_tok; _temp6428.f1= Cyc_Absyn_and_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6429= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6435; _LL6431: if(*((
void**) _temp6429) ==  Cyc_Exp_tok){ _LL6436: _temp6435=(( struct Cyc_Exp_tok_struct*)
_temp6429)->f1; goto _LL6432;} else{ goto _LL6433;} _LL6433: goto _LL6434;
_LL6432: yyzzz= _temp6435; goto _LL6430; _LL6434:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6430:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6437= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6443; _LL6439: if(*(( void**) _temp6437) ==  Cyc_Exp_tok){ _LL6444:
_temp6443=(( struct Cyc_Exp_tok_struct*) _temp6437)->f1; goto _LL6440;} else{
goto _LL6441;} _LL6441: goto _LL6442; _LL6440: yyzzz= _temp6443; goto _LL6438;
_LL6442:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6438:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6428;}); _temp6427;}); break; case 330: _LL6426: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL6445:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6447=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6447[ 0]=({ struct Cyc_Exp_tok_struct
_temp6448; _temp6448.tag= Cyc_Exp_tok; _temp6448.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6449= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6455; _LL6451: if(*(( void**) _temp6449) ==  Cyc_Exp_tok){ _LL6456:
_temp6455=(( struct Cyc_Exp_tok_struct*) _temp6449)->f1; goto _LL6452;} else{
goto _LL6453;} _LL6453: goto _LL6454; _LL6452: yyzzz= _temp6455; goto _LL6450;
_LL6454:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6450:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6457= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6463; _LL6459: if(*(( void**) _temp6457) ==  Cyc_Exp_tok){ _LL6464:
_temp6463=(( struct Cyc_Exp_tok_struct*) _temp6457)->f1; goto _LL6460;} else{
goto _LL6461;} _LL6461: goto _LL6462; _LL6460: yyzzz= _temp6463; goto _LL6458;
_LL6462:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6458:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6448;}); _temp6447;}); break; case 332: _LL6446: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL6465:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6467=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6467[ 0]=({ struct Cyc_Exp_tok_struct
_temp6468; _temp6468.tag= Cyc_Exp_tok; _temp6468.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6469= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6475; _LL6471: if(*(( void**) _temp6469) ==  Cyc_Exp_tok){ _LL6476:
_temp6475=(( struct Cyc_Exp_tok_struct*) _temp6469)->f1; goto _LL6472;} else{
goto _LL6473;} _LL6473: goto _LL6474; _LL6472: yyzzz= _temp6475; goto _LL6470;
_LL6474:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6470:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6477= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6483; _LL6479: if(*(( void**) _temp6477) ==  Cyc_Exp_tok){ _LL6484:
_temp6483=(( struct Cyc_Exp_tok_struct*) _temp6477)->f1; goto _LL6480;} else{
goto _LL6481;} _LL6481: goto _LL6482; _LL6480: yyzzz= _temp6483; goto _LL6478;
_LL6482:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6478:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6468;}); _temp6467;}); break; case 334: _LL6466: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL6485:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6487=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6487[ 0]=({ struct Cyc_Exp_tok_struct
_temp6488; _temp6488.tag= Cyc_Exp_tok; _temp6488.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6489= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6495; _LL6491: if(*(( void**) _temp6489) ==  Cyc_Exp_tok){ _LL6496:
_temp6495=(( struct Cyc_Exp_tok_struct*) _temp6489)->f1; goto _LL6492;} else{
goto _LL6493;} _LL6493: goto _LL6494; _LL6492: yyzzz= _temp6495; goto _LL6490;
_LL6494:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6490:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6497= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6503; _LL6499: if(*(( void**) _temp6497) ==  Cyc_Exp_tok){ _LL6504:
_temp6503=(( struct Cyc_Exp_tok_struct*) _temp6497)->f1; goto _LL6500;} else{
goto _LL6501;} _LL6501: goto _LL6502; _LL6500: yyzzz= _temp6503; goto _LL6498;
_LL6502:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6498:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6488;}); _temp6487;}); break; case 336: _LL6486: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL6505:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6507=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6507[ 0]=({ struct Cyc_Exp_tok_struct
_temp6508; _temp6508.tag= Cyc_Exp_tok; _temp6508.f1= Cyc_Absyn_eq_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6509= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6515; _LL6511: if(*((
void**) _temp6509) ==  Cyc_Exp_tok){ _LL6516: _temp6515=(( struct Cyc_Exp_tok_struct*)
_temp6509)->f1; goto _LL6512;} else{ goto _LL6513;} _LL6513: goto _LL6514;
_LL6512: yyzzz= _temp6515; goto _LL6510; _LL6514:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6510:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6517= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6523; _LL6519: if(*(( void**) _temp6517) ==  Cyc_Exp_tok){ _LL6524:
_temp6523=(( struct Cyc_Exp_tok_struct*) _temp6517)->f1; goto _LL6520;} else{
goto _LL6521;} _LL6521: goto _LL6522; _LL6520: yyzzz= _temp6523; goto _LL6518;
_LL6522:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6518:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6508;}); _temp6507;}); break; case 338: _LL6506: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6526=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6526[ 0]=({ struct Cyc_Exp_tok_struct
_temp6527; _temp6527.tag= Cyc_Exp_tok; _temp6527.f1= Cyc_Absyn_neq_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6528= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6534; _LL6530: if(*((
void**) _temp6528) ==  Cyc_Exp_tok){ _LL6535: _temp6534=(( struct Cyc_Exp_tok_struct*)
_temp6528)->f1; goto _LL6531;} else{ goto _LL6532;} _LL6532: goto _LL6533;
_LL6531: yyzzz= _temp6534; goto _LL6529; _LL6533:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6529:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6536= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6542; _LL6538: if(*(( void**) _temp6536) ==  Cyc_Exp_tok){ _LL6543:
_temp6542=(( struct Cyc_Exp_tok_struct*) _temp6536)->f1; goto _LL6539;} else{
goto _LL6540;} _LL6540: goto _LL6541; _LL6539: yyzzz= _temp6542; goto _LL6537;
_LL6541:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6537:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6527;}); _temp6526;}); break; case 339: _LL6525: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL6544:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6546=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6546[ 0]=({ struct Cyc_Exp_tok_struct
_temp6547; _temp6547.tag= Cyc_Exp_tok; _temp6547.f1= Cyc_Absyn_lt_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6548= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6554; _LL6550: if(*((
void**) _temp6548) ==  Cyc_Exp_tok){ _LL6555: _temp6554=(( struct Cyc_Exp_tok_struct*)
_temp6548)->f1; goto _LL6551;} else{ goto _LL6552;} _LL6552: goto _LL6553;
_LL6551: yyzzz= _temp6554; goto _LL6549; _LL6553:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6549:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6556= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6562; _LL6558: if(*(( void**) _temp6556) ==  Cyc_Exp_tok){ _LL6563:
_temp6562=(( struct Cyc_Exp_tok_struct*) _temp6556)->f1; goto _LL6559;} else{
goto _LL6560;} _LL6560: goto _LL6561; _LL6559: yyzzz= _temp6562; goto _LL6557;
_LL6561:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6557:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6547;}); _temp6546;}); break; case 341: _LL6545: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6565=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6565[ 0]=({ struct Cyc_Exp_tok_struct
_temp6566; _temp6566.tag= Cyc_Exp_tok; _temp6566.f1= Cyc_Absyn_gt_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6567= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6573; _LL6569: if(*((
void**) _temp6567) ==  Cyc_Exp_tok){ _LL6574: _temp6573=(( struct Cyc_Exp_tok_struct*)
_temp6567)->f1; goto _LL6570;} else{ goto _LL6571;} _LL6571: goto _LL6572;
_LL6570: yyzzz= _temp6573; goto _LL6568; _LL6572:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6568:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6575= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6581; _LL6577: if(*(( void**) _temp6575) ==  Cyc_Exp_tok){ _LL6582:
_temp6581=(( struct Cyc_Exp_tok_struct*) _temp6575)->f1; goto _LL6578;} else{
goto _LL6579;} _LL6579: goto _LL6580; _LL6578: yyzzz= _temp6581; goto _LL6576;
_LL6580:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6576:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6566;}); _temp6565;}); break; case 342: _LL6564: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6584=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6584[ 0]=({ struct Cyc_Exp_tok_struct
_temp6585; _temp6585.tag= Cyc_Exp_tok; _temp6585.f1= Cyc_Absyn_lte_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6586= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6592; _LL6588: if(*((
void**) _temp6586) ==  Cyc_Exp_tok){ _LL6593: _temp6592=(( struct Cyc_Exp_tok_struct*)
_temp6586)->f1; goto _LL6589;} else{ goto _LL6590;} _LL6590: goto _LL6591;
_LL6589: yyzzz= _temp6592; goto _LL6587; _LL6591:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6587:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6594= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6600; _LL6596: if(*(( void**) _temp6594) ==  Cyc_Exp_tok){ _LL6601:
_temp6600=(( struct Cyc_Exp_tok_struct*) _temp6594)->f1; goto _LL6597;} else{
goto _LL6598;} _LL6598: goto _LL6599; _LL6597: yyzzz= _temp6600; goto _LL6595;
_LL6599:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6595:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6585;}); _temp6584;}); break; case 343: _LL6583: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6603=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6603[ 0]=({ struct Cyc_Exp_tok_struct
_temp6604; _temp6604.tag= Cyc_Exp_tok; _temp6604.f1= Cyc_Absyn_gte_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6605= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6611; _LL6607: if(*((
void**) _temp6605) ==  Cyc_Exp_tok){ _LL6612: _temp6611=(( struct Cyc_Exp_tok_struct*)
_temp6605)->f1; goto _LL6608;} else{ goto _LL6609;} _LL6609: goto _LL6610;
_LL6608: yyzzz= _temp6611; goto _LL6606; _LL6610:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6606:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6613= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6619; _LL6615: if(*(( void**) _temp6613) ==  Cyc_Exp_tok){ _LL6620:
_temp6619=(( struct Cyc_Exp_tok_struct*) _temp6613)->f1; goto _LL6616;} else{
goto _LL6617;} _LL6617: goto _LL6618; _LL6616: yyzzz= _temp6619; goto _LL6614;
_LL6618:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6614:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6604;}); _temp6603;}); break; case 344: _LL6602: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL6621:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6623=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6623[ 0]=({ struct Cyc_Exp_tok_struct
_temp6624; _temp6624.tag= Cyc_Exp_tok; _temp6624.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6625= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6631; _LL6627: if(*(( void**) _temp6625) ==  Cyc_Exp_tok){ _LL6632:
_temp6631=(( struct Cyc_Exp_tok_struct*) _temp6625)->f1; goto _LL6628;} else{
goto _LL6629;} _LL6629: goto _LL6630; _LL6628: yyzzz= _temp6631; goto _LL6626;
_LL6630:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6626:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6633= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6639; _LL6635: if(*(( void**) _temp6633) ==  Cyc_Exp_tok){ _LL6640:
_temp6639=(( struct Cyc_Exp_tok_struct*) _temp6633)->f1; goto _LL6636;} else{
goto _LL6637;} _LL6637: goto _LL6638; _LL6636: yyzzz= _temp6639; goto _LL6634;
_LL6638:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6634:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6624;}); _temp6623;}); break; case 346: _LL6622: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6642=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6642[ 0]=({ struct Cyc_Exp_tok_struct
_temp6643; _temp6643.tag= Cyc_Exp_tok; _temp6643.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6644= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6650; _LL6646: if(*(( void**) _temp6644) ==  Cyc_Exp_tok){ _LL6651:
_temp6650=(( struct Cyc_Exp_tok_struct*) _temp6644)->f1; goto _LL6647;} else{
goto _LL6648;} _LL6648: goto _LL6649; _LL6647: yyzzz= _temp6650; goto _LL6645;
_LL6649:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6645:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6652= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6658; _LL6654: if(*(( void**) _temp6652) ==  Cyc_Exp_tok){ _LL6659:
_temp6658=(( struct Cyc_Exp_tok_struct*) _temp6652)->f1; goto _LL6655;} else{
goto _LL6656;} _LL6656: goto _LL6657; _LL6655: yyzzz= _temp6658; goto _LL6653;
_LL6657:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6653:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6643;}); _temp6642;}); break; case 347: _LL6641: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL6660:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6662=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6662[ 0]=({ struct Cyc_Exp_tok_struct
_temp6663; _temp6663.tag= Cyc_Exp_tok; _temp6663.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6664= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6670; _LL6666: if(*(( void**) _temp6664) ==  Cyc_Exp_tok){ _LL6671:
_temp6670=(( struct Cyc_Exp_tok_struct*) _temp6664)->f1; goto _LL6667;} else{
goto _LL6668;} _LL6668: goto _LL6669; _LL6667: yyzzz= _temp6670; goto _LL6665;
_LL6669:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6665:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6672= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6678; _LL6674: if(*(( void**) _temp6672) ==  Cyc_Exp_tok){ _LL6679:
_temp6678=(( struct Cyc_Exp_tok_struct*) _temp6672)->f1; goto _LL6675;} else{
goto _LL6676;} _LL6676: goto _LL6677; _LL6675: yyzzz= _temp6678; goto _LL6673;
_LL6677:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6673:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6663;}); _temp6662;}); break; case 349: _LL6661: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6681=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6681[ 0]=({ struct Cyc_Exp_tok_struct
_temp6682; _temp6682.tag= Cyc_Exp_tok; _temp6682.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6683= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6689; _LL6685: if(*(( void**) _temp6683) ==  Cyc_Exp_tok){ _LL6690:
_temp6689=(( struct Cyc_Exp_tok_struct*) _temp6683)->f1; goto _LL6686;} else{
goto _LL6687;} _LL6687: goto _LL6688; _LL6686: yyzzz= _temp6689; goto _LL6684;
_LL6688:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6684:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6691= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6697; _LL6693: if(*(( void**) _temp6691) ==  Cyc_Exp_tok){ _LL6698:
_temp6697=(( struct Cyc_Exp_tok_struct*) _temp6691)->f1; goto _LL6694;} else{
goto _LL6695;} _LL6695: goto _LL6696; _LL6694: yyzzz= _temp6697; goto _LL6692;
_LL6696:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6692:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6682;}); _temp6681;}); break; case 350: _LL6680: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL6699:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6701=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6701[ 0]=({ struct Cyc_Exp_tok_struct
_temp6702; _temp6702.tag= Cyc_Exp_tok; _temp6702.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6703= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6709; _LL6705: if(*(( void**) _temp6703) ==  Cyc_Exp_tok){ _LL6710:
_temp6709=(( struct Cyc_Exp_tok_struct*) _temp6703)->f1; goto _LL6706;} else{
goto _LL6707;} _LL6707: goto _LL6708; _LL6706: yyzzz= _temp6709; goto _LL6704;
_LL6708:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6704:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6711= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6717; _LL6713: if(*(( void**) _temp6711) ==  Cyc_Exp_tok){ _LL6718:
_temp6717=(( struct Cyc_Exp_tok_struct*) _temp6711)->f1; goto _LL6714;} else{
goto _LL6715;} _LL6715: goto _LL6716; _LL6714: yyzzz= _temp6717; goto _LL6712;
_LL6716:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6712:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6702;}); _temp6701;}); break; case 352: _LL6700: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6720=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6720[ 0]=({ struct Cyc_Exp_tok_struct
_temp6721; _temp6721.tag= Cyc_Exp_tok; _temp6721.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6722= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6728; _LL6724: if(*(( void**) _temp6722) ==  Cyc_Exp_tok){ _LL6729:
_temp6728=(( struct Cyc_Exp_tok_struct*) _temp6722)->f1; goto _LL6725;} else{
goto _LL6726;} _LL6726: goto _LL6727; _LL6725: yyzzz= _temp6728; goto _LL6723;
_LL6727:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6723:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6730= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6736; _LL6732: if(*(( void**) _temp6730) ==  Cyc_Exp_tok){ _LL6737:
_temp6736=(( struct Cyc_Exp_tok_struct*) _temp6730)->f1; goto _LL6733;} else{
goto _LL6734;} _LL6734: goto _LL6735; _LL6733: yyzzz= _temp6736; goto _LL6731;
_LL6735:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6731:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6721;}); _temp6720;}); break; case 353: _LL6719: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6739=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6739[ 0]=({ struct Cyc_Exp_tok_struct
_temp6740; _temp6740.tag= Cyc_Exp_tok; _temp6740.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6741= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6747; _LL6743: if(*(( void**) _temp6741) ==  Cyc_Exp_tok){ _LL6748:
_temp6747=(( struct Cyc_Exp_tok_struct*) _temp6741)->f1; goto _LL6744;} else{
goto _LL6745;} _LL6745: goto _LL6746; _LL6744: yyzzz= _temp6747; goto _LL6742;
_LL6746:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6742:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6749= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6755; _LL6751: if(*(( void**) _temp6749) ==  Cyc_Exp_tok){ _LL6756:
_temp6755=(( struct Cyc_Exp_tok_struct*) _temp6749)->f1; goto _LL6752;} else{
goto _LL6753;} _LL6753: goto _LL6754; _LL6752: yyzzz= _temp6755; goto _LL6750;
_LL6754:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6750:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6740;}); _temp6739;}); break; case 354: _LL6738: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL6757:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6759=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6759[ 0]=({ struct Cyc_Exp_tok_struct
_temp6760; _temp6760.tag= Cyc_Exp_tok; _temp6760.f1= Cyc_Absyn_cast_exp((*({
struct _tuple2* yyzzz;{ void* _temp6761= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct _tuple2* _temp6767; _LL6763: if(*(( void**)
_temp6761) ==  Cyc_ParamDecl_tok){ _LL6768: _temp6767=(( struct Cyc_ParamDecl_tok_struct*)
_temp6761)->f1; goto _LL6764;} else{ goto _LL6765;} _LL6765: goto _LL6766;
_LL6764: yyzzz= _temp6767; goto _LL6762; _LL6766:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL6762:;} yyzzz;})).f3,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6769= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6775; _LL6771: if(*(( void**) _temp6769) ==  Cyc_Exp_tok){ _LL6776:
_temp6775=(( struct Cyc_Exp_tok_struct*) _temp6769)->f1; goto _LL6772;} else{
goto _LL6773;} _LL6773: goto _LL6774; _LL6772: yyzzz= _temp6775; goto _LL6770;
_LL6774:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6770:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6760;}); _temp6759;}); break; case 356: _LL6758: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL6777:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6779=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6779[ 0]=({ struct Cyc_Exp_tok_struct
_temp6780; _temp6780.tag= Cyc_Exp_tok; _temp6780.f1= Cyc_Absyn_pre_inc_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6781= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6787; _LL6783: if(*(( void**) _temp6781) ==  Cyc_Exp_tok){ _LL6788:
_temp6787=(( struct Cyc_Exp_tok_struct*) _temp6781)->f1; goto _LL6784;} else{
goto _LL6785;} _LL6785: goto _LL6786; _LL6784: yyzzz= _temp6787; goto _LL6782;
_LL6786:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6782:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6780;}); _temp6779;}); break; case 358: _LL6778: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6790=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6790[ 0]=({ struct Cyc_Exp_tok_struct
_temp6791; _temp6791.tag= Cyc_Exp_tok; _temp6791.f1= Cyc_Absyn_pre_dec_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6792= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6798; _LL6794: if(*(( void**) _temp6792) ==  Cyc_Exp_tok){ _LL6799:
_temp6798=(( struct Cyc_Exp_tok_struct*) _temp6792)->f1; goto _LL6795;} else{
goto _LL6796;} _LL6796: goto _LL6797; _LL6795: yyzzz= _temp6798; goto _LL6793;
_LL6797:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6793:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6791;}); _temp6790;}); break; case 359: _LL6789: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6801=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6801[ 0]=({ struct Cyc_Exp_tok_struct
_temp6802; _temp6802.tag= Cyc_Exp_tok; _temp6802.f1= Cyc_Absyn_address_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6803= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6809; _LL6805: if(*(( void**) _temp6803) ==  Cyc_Exp_tok){ _LL6810:
_temp6809=(( struct Cyc_Exp_tok_struct*) _temp6803)->f1; goto _LL6806;} else{
goto _LL6807;} _LL6807: goto _LL6808; _LL6806: yyzzz= _temp6809; goto _LL6804;
_LL6808:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6804:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6802;}); _temp6801;}); break; case 360: _LL6800: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6812=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6812[ 0]=({ struct Cyc_Exp_tok_struct
_temp6813; _temp6813.tag= Cyc_Exp_tok; _temp6813.f1= Cyc_Absyn_deref_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6814= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6820; _LL6816: if(*(( void**) _temp6814) ==  Cyc_Exp_tok){ _LL6821:
_temp6820=(( struct Cyc_Exp_tok_struct*) _temp6814)->f1; goto _LL6817;} else{
goto _LL6818;} _LL6818: goto _LL6819; _LL6817: yyzzz= _temp6820; goto _LL6815;
_LL6819:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6815:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6813;}); _temp6812;}); break; case 361: _LL6811: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL6822:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6824=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6824[ 0]=({ struct Cyc_Exp_tok_struct
_temp6825; _temp6825.tag= Cyc_Exp_tok; _temp6825.f1= Cyc_Absyn_prim1_exp(({ void*
yyzzz;{ void* _temp6826= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; void* _temp6832; _LL6828: if(*(( void**) _temp6826) ==  Cyc_Primop_tok){
_LL6833: _temp6832=( void*)(( struct Cyc_Primop_tok_struct*) _temp6826)->f1;
goto _LL6829;} else{ goto _LL6830;} _LL6830: goto _LL6831; _LL6829: yyzzz=
_temp6832; goto _LL6827; _LL6831:( int) _throw(( void*)& Cyc_yyfail_Primop_tok);
_LL6827:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6834= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6840; _LL6836: if(*(( void**) _temp6834) ==  Cyc_Exp_tok){ _LL6841:
_temp6840=(( struct Cyc_Exp_tok_struct*) _temp6834)->f1; goto _LL6837;} else{
goto _LL6838;} _LL6838: goto _LL6839; _LL6837: yyzzz= _temp6840; goto _LL6835;
_LL6839:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6835:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6825;}); _temp6824;}); break; case 363: _LL6823: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6843=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6843[ 0]=({ struct Cyc_Exp_tok_struct
_temp6844; _temp6844.tag= Cyc_Exp_tok; _temp6844.f1= Cyc_Absyn_sizeoftyp_exp((*({
struct _tuple2* yyzzz;{ void* _temp6845= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct _tuple2* _temp6851; _LL6847: if(*(( void**)
_temp6845) ==  Cyc_ParamDecl_tok){ _LL6852: _temp6851=(( struct Cyc_ParamDecl_tok_struct*)
_temp6845)->f1; goto _LL6848;} else{ goto _LL6849;} _LL6849: goto _LL6850;
_LL6848: yyzzz= _temp6851; goto _LL6846; _LL6850:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL6846:;} yyzzz;})).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6844;});
_temp6843;}); break; case 364: _LL6842: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6854=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6854[ 0]=({ struct Cyc_Exp_tok_struct _temp6855; _temp6855.tag= Cyc_Exp_tok;
_temp6855.f1= Cyc_Absyn_sizeofexp_exp(({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp6856= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Exp* _temp6862; _LL6858: if(*(( void**) _temp6856) ==  Cyc_Exp_tok){
_LL6863: _temp6862=(( struct Cyc_Exp_tok_struct*) _temp6856)->f1; goto _LL6859;}
else{ goto _LL6860;} _LL6860: goto _LL6861; _LL6859: yyzzz= _temp6862; goto
_LL6857; _LL6861:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6857:;} yyzzz;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp6855;}); _temp6854;}); break; case 365:
_LL6853: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6865=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6865[ 0]=({ struct Cyc_Exp_tok_struct
_temp6866; _temp6866.tag= Cyc_Exp_tok; _temp6866.f1= Cyc_Absyn_offsetof_exp((*({
struct _tuple2* yyzzz;{ void* _temp6867= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]; struct _tuple2* _temp6873; _LL6869: if(*(( void**)
_temp6867) ==  Cyc_ParamDecl_tok){ _LL6874: _temp6873=(( struct Cyc_ParamDecl_tok_struct*)
_temp6867)->f1; goto _LL6870;} else{ goto _LL6871;} _LL6871: goto _LL6872;
_LL6870: yyzzz= _temp6873; goto _LL6868; _LL6872:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL6868:;} yyzzz;})).f3,({ struct _tagged_arr* _temp6875=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp6875[ 0]=({ struct _tagged_arr
yyzzz;{ void* _temp6876= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp6882; _LL6878: if(*(( void**)
_temp6876) ==  Cyc_String_tok){ _LL6883: _temp6882=(( struct Cyc_String_tok_struct*)
_temp6876)->f1; goto _LL6879;} else{ goto _LL6880;} _LL6880: goto _LL6881;
_LL6879: yyzzz= _temp6882; goto _LL6877; _LL6881:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL6877:;} yyzzz;}); _temp6875;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6866;});
_temp6865;}); break; case 366: _LL6864: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6885=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6885[ 0]=({ struct Cyc_Exp_tok_struct _temp6886; _temp6886.tag= Cyc_Exp_tok;
_temp6886.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6887=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6887[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6888; _temp6888.tag= Cyc_Absyn_Malloc_e;
_temp6888.f1= 0; _temp6888.f2=( void*) Cyc_Parse_speclist2typ((*({ struct
_tuple17* yyzzz;{ void* _temp6889= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tuple17* _temp6895; _LL6891: if(*(( void**)
_temp6889) ==  Cyc_QualSpecList_tok){ _LL6896: _temp6895=(( struct Cyc_QualSpecList_tok_struct*)
_temp6889)->f1; goto _LL6892;} else{ goto _LL6893;} _LL6893: goto _LL6894;
_LL6892: yyzzz= _temp6895; goto _LL6890; _LL6894:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL6890:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp6888;}); _temp6887;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6886;});
_temp6885;}); break; case 367: _LL6884: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6898=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6898[ 0]=({ struct Cyc_Exp_tok_struct _temp6899; _temp6899.tag= Cyc_Exp_tok;
_temp6899.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6900=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6900[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6901; _temp6901.tag= Cyc_Absyn_Malloc_e;
_temp6901.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp6902= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)];
struct Cyc_Absyn_Exp* _temp6908; _LL6904: if(*(( void**) _temp6902) ==  Cyc_Exp_tok){
_LL6909: _temp6908=(( struct Cyc_Exp_tok_struct*) _temp6902)->f1; goto _LL6905;}
else{ goto _LL6906;} _LL6906: goto _LL6907; _LL6905: yyzzz= _temp6908; goto
_LL6903; _LL6907:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6903:;} yyzzz;});
_temp6901.f2=( void*) Cyc_Parse_speclist2typ((*({ struct _tuple17* yyzzz;{ void*
_temp6910= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct _tuple17* _temp6916; _LL6912: if(*(( void**) _temp6910) ==  Cyc_QualSpecList_tok){
_LL6917: _temp6916=(( struct Cyc_QualSpecList_tok_struct*) _temp6910)->f1; goto
_LL6913;} else{ goto _LL6914;} _LL6914: goto _LL6915; _LL6913: yyzzz= _temp6916;
goto _LL6911; _LL6915:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL6911:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp6901;}); _temp6900;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6899;});
_temp6898;}); break; case 368: _LL6897: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp6919=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6919[ 0]=({ struct Cyc_Primop_tok_struct _temp6920; _temp6920.tag= Cyc_Primop_tok;
_temp6920.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp6920;}); _temp6919;});
break; case 369: _LL6918: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp6922=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6922[ 0]=({ struct Cyc_Primop_tok_struct _temp6923; _temp6923.tag= Cyc_Primop_tok;
_temp6923.f1=( void*)(( void*) Cyc_Absyn_Not); _temp6923;}); _temp6922;});
break; case 370: _LL6921: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp6925=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6925[ 0]=({ struct Cyc_Primop_tok_struct _temp6926; _temp6926.tag= Cyc_Primop_tok;
_temp6926.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp6926;}); _temp6925;});
break; case 371: _LL6924: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 372: _LL6927: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6929=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6929[ 0]=({ struct Cyc_Exp_tok_struct _temp6930; _temp6930.tag= Cyc_Exp_tok;
_temp6930.f1= Cyc_Absyn_subscript_exp(({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp6931= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Absyn_Exp* _temp6937; _LL6933: if(*(( void**) _temp6931) ==  Cyc_Exp_tok){
_LL6938: _temp6937=(( struct Cyc_Exp_tok_struct*) _temp6931)->f1; goto _LL6934;}
else{ goto _LL6935;} _LL6935: goto _LL6936; _LL6934: yyzzz= _temp6937; goto
_LL6932; _LL6936:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6932:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6939= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp6945; _LL6941: if(*(( void**) _temp6939) ==  Cyc_Exp_tok){ _LL6946:
_temp6945=(( struct Cyc_Exp_tok_struct*) _temp6939)->f1; goto _LL6942;} else{
goto _LL6943;} _LL6943: goto _LL6944; _LL6942: yyzzz= _temp6945; goto _LL6940;
_LL6944:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6940:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6930;}); _temp6929;}); break; case 373: _LL6928: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6948=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6948[ 0]=({ struct Cyc_Exp_tok_struct
_temp6949; _temp6949.tag= Cyc_Exp_tok; _temp6949.f1= Cyc_Absyn_unknowncall_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6950= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6956; _LL6952: if(*(( void**) _temp6950) ==  Cyc_Exp_tok){ _LL6957:
_temp6956=(( struct Cyc_Exp_tok_struct*) _temp6950)->f1; goto _LL6953;} else{
goto _LL6954;} _LL6954: goto _LL6955; _LL6953: yyzzz= _temp6956; goto _LL6951;
_LL6955:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6951:;} yyzzz;}), 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6949;}); _temp6948;}); break; case 374: _LL6947: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6959=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6959[ 0]=({ struct Cyc_Exp_tok_struct
_temp6960; _temp6960.tag= Cyc_Exp_tok; _temp6960.f1= Cyc_Absyn_unknowncall_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6961= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp6967; _LL6963: if(*(( void**) _temp6961) ==  Cyc_Exp_tok){ _LL6968:
_temp6967=(( struct Cyc_Exp_tok_struct*) _temp6961)->f1; goto _LL6964;} else{
goto _LL6965;} _LL6965: goto _LL6966; _LL6964: yyzzz= _temp6967; goto _LL6962;
_LL6966:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6962:;} yyzzz;}),({
struct Cyc_List_List* yyzzz;{ void* _temp6969= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp6975; _LL6971: if(*(( void**) _temp6969) ==  Cyc_ExpList_tok){ _LL6976:
_temp6975=(( struct Cyc_ExpList_tok_struct*) _temp6969)->f1; goto _LL6972;}
else{ goto _LL6973;} _LL6973: goto _LL6974; _LL6972: yyzzz= _temp6975; goto
_LL6970; _LL6974:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL6970:;}
yyzzz;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp6960;}); _temp6959;}); break; case 375:
_LL6958: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6978=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6978[ 0]=({ struct Cyc_Exp_tok_struct
_temp6979; _temp6979.tag= Cyc_Exp_tok; _temp6979.f1= Cyc_Absyn_structmember_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6980= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6986; _LL6982: if(*(( void**) _temp6980) ==  Cyc_Exp_tok){ _LL6987:
_temp6986=(( struct Cyc_Exp_tok_struct*) _temp6980)->f1; goto _LL6983;} else{
goto _LL6984;} _LL6984: goto _LL6985; _LL6983: yyzzz= _temp6986; goto _LL6981;
_LL6985:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6981:;} yyzzz;}),({
struct _tagged_arr* _temp6988=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp6988[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp6989= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp6995; _LL6991: if(*(( void**) _temp6989) ==  Cyc_String_tok){ _LL6996:
_temp6995=(( struct Cyc_String_tok_struct*) _temp6989)->f1; goto _LL6992;} else{
goto _LL6993;} _LL6993: goto _LL6994; _LL6992: yyzzz= _temp6995; goto _LL6990;
_LL6994:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL6990:;} yyzzz;});
_temp6988;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp6979;}); _temp6978;}); break; case 376:
_LL6977: { struct _tuple1* q=({ struct _tuple1* yyzzz;{ void* _temp7008= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp7014; _LL7010: if(*(( void**) _temp7008) ==  Cyc_QualId_tok){ _LL7015:
_temp7014=(( struct Cyc_QualId_tok_struct*) _temp7008)->f1; goto _LL7011;} else{
goto _LL7012;} _LL7012: goto _LL7013; _LL7011: yyzzz= _temp7014; goto _LL7009;
_LL7013:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7009:;} yyzzz;}); if(
Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp6998=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6998[ 0]=({ struct Cyc_Exp_tok_struct
_temp6999; _temp6999.tag= Cyc_Exp_tok; _temp6999.f1= Cyc_Absyn_structmember_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7000= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp7006; _LL7002: if(*(( void**) _temp7000) ==  Cyc_Exp_tok){ _LL7007:
_temp7006=(( struct Cyc_Exp_tok_struct*) _temp7000)->f1; goto _LL7003;} else{
goto _LL7004;} _LL7004: goto _LL7005; _LL7003: yyzzz= _temp7006; goto _LL7001;
_LL7005:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7001:;} yyzzz;}),(* q).f2,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp6999;}); _temp6998;}); break;} case 377:
_LL6997: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7017=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7017[ 0]=({ struct Cyc_Exp_tok_struct
_temp7018; _temp7018.tag= Cyc_Exp_tok; _temp7018.f1= Cyc_Absyn_structarrow_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7019= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp7025; _LL7021: if(*(( void**) _temp7019) ==  Cyc_Exp_tok){ _LL7026:
_temp7025=(( struct Cyc_Exp_tok_struct*) _temp7019)->f1; goto _LL7022;} else{
goto _LL7023;} _LL7023: goto _LL7024; _LL7022: yyzzz= _temp7025; goto _LL7020;
_LL7024:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7020:;} yyzzz;}),({
struct _tagged_arr* _temp7027=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp7027[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp7028= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp7034; _LL7030: if(*(( void**) _temp7028) ==  Cyc_String_tok){ _LL7035:
_temp7034=(( struct Cyc_String_tok_struct*) _temp7028)->f1; goto _LL7031;} else{
goto _LL7032;} _LL7032: goto _LL7033; _LL7031: yyzzz= _temp7034; goto _LL7029;
_LL7033:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL7029:;} yyzzz;});
_temp7027;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp7018;}); _temp7017;}); break; case 378:
_LL7016: { struct _tuple1* q=({ struct _tuple1* yyzzz;{ void* _temp7047= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp7053; _LL7049: if(*(( void**) _temp7047) ==  Cyc_QualId_tok){ _LL7054:
_temp7053=(( struct Cyc_QualId_tok_struct*) _temp7047)->f1; goto _LL7050;} else{
goto _LL7051;} _LL7051: goto _LL7052; _LL7050: yyzzz= _temp7053; goto _LL7048;
_LL7052:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7048:;} yyzzz;}); if(
Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp7037=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7037[ 0]=({ struct Cyc_Exp_tok_struct
_temp7038; _temp7038.tag= Cyc_Exp_tok; _temp7038.f1= Cyc_Absyn_structarrow_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7039= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp7045; _LL7041: if(*(( void**) _temp7039) ==  Cyc_Exp_tok){ _LL7046:
_temp7045=(( struct Cyc_Exp_tok_struct*) _temp7039)->f1; goto _LL7042;} else{
goto _LL7043;} _LL7043: goto _LL7044; _LL7042: yyzzz= _temp7045; goto _LL7040;
_LL7044:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7040:;} yyzzz;}),(* q).f2,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp7038;}); _temp7037;}); break;} case 379:
_LL7036: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7056=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7056[ 0]=({ struct Cyc_Exp_tok_struct
_temp7057; _temp7057.tag= Cyc_Exp_tok; _temp7057.f1= Cyc_Absyn_post_inc_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7058= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp7064; _LL7060: if(*(( void**) _temp7058) ==  Cyc_Exp_tok){ _LL7065:
_temp7064=(( struct Cyc_Exp_tok_struct*) _temp7058)->f1; goto _LL7061;} else{
goto _LL7062;} _LL7062: goto _LL7063; _LL7061: yyzzz= _temp7064; goto _LL7059;
_LL7063:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7059:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7057;}); _temp7056;}); break; case 380: _LL7055: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7067=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7067[ 0]=({ struct Cyc_Exp_tok_struct
_temp7068; _temp7068.tag= Cyc_Exp_tok; _temp7068.f1= Cyc_Absyn_post_dec_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7069= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp7075; _LL7071: if(*(( void**) _temp7069) ==  Cyc_Exp_tok){ _LL7076:
_temp7075=(( struct Cyc_Exp_tok_struct*) _temp7069)->f1; goto _LL7072;} else{
goto _LL7073;} _LL7073: goto _LL7074; _LL7072: yyzzz= _temp7075; goto _LL7070;
_LL7074:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7070:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7068;}); _temp7067;}); break; case 381: _LL7066: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7078=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7078[ 0]=({ struct Cyc_Exp_tok_struct
_temp7079; _temp7079.tag= Cyc_Exp_tok; _temp7079.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp7080=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7080[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7081; _temp7081.tag= Cyc_Absyn_CompoundLit_e;
_temp7081.f1=({ struct _tuple2* yyzzz;{ void* _temp7082= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct _tuple2*
_temp7088; _LL7084: if(*(( void**) _temp7082) ==  Cyc_ParamDecl_tok){ _LL7089:
_temp7088=(( struct Cyc_ParamDecl_tok_struct*) _temp7082)->f1; goto _LL7085;}
else{ goto _LL7086;} _LL7086: goto _LL7087; _LL7085: yyzzz= _temp7088; goto
_LL7083; _LL7087:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL7083:;}
yyzzz;}); _temp7081.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp7090= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7096; _LL7092: if(*(( void**) _temp7090) ==  Cyc_InitializerList_tok){
_LL7097: _temp7096=(( struct Cyc_InitializerList_tok_struct*) _temp7090)->f1;
goto _LL7093;} else{ goto _LL7094;} _LL7094: goto _LL7095; _LL7093: yyzzz=
_temp7096; goto _LL7091; _LL7095:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL7091:;} yyzzz;})); _temp7081;}); _temp7080;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7079;}); _temp7078;}); break; case 382: _LL7077: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7099=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7099[ 0]=({ struct Cyc_Exp_tok_struct
_temp7100; _temp7100.tag= Cyc_Exp_tok; _temp7100.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp7101=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7101[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7102; _temp7102.tag= Cyc_Absyn_CompoundLit_e;
_temp7102.f1=({ struct _tuple2* yyzzz;{ void* _temp7103= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct _tuple2*
_temp7109; _LL7105: if(*(( void**) _temp7103) ==  Cyc_ParamDecl_tok){ _LL7110:
_temp7109=(( struct Cyc_ParamDecl_tok_struct*) _temp7103)->f1; goto _LL7106;}
else{ goto _LL7107;} _LL7107: goto _LL7108; _LL7106: yyzzz= _temp7109; goto
_LL7104; _LL7108:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL7104:;}
yyzzz;}); _temp7102.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp7111= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp7117; _LL7113: if(*(( void**) _temp7111) ==  Cyc_InitializerList_tok){
_LL7118: _temp7117=(( struct Cyc_InitializerList_tok_struct*) _temp7111)->f1;
goto _LL7114;} else{ goto _LL7115;} _LL7115: goto _LL7116; _LL7114: yyzzz=
_temp7117; goto _LL7112; _LL7116:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL7112:;} yyzzz;})); _temp7102;}); _temp7101;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7100;}); _temp7099;}); break; case 383: _LL7098: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7120=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7120[ 0]=({ struct Cyc_Exp_tok_struct
_temp7121; _temp7121.tag= Cyc_Exp_tok; _temp7121.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp7122=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7122[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7123; _temp7123.tag= Cyc_Absyn_Fill_e; _temp7123.f1=({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp7124= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Absyn_Exp* _temp7130; _LL7126: if(*(( void**)
_temp7124) ==  Cyc_Exp_tok){ _LL7131: _temp7130=(( struct Cyc_Exp_tok_struct*)
_temp7124)->f1; goto _LL7127;} else{ goto _LL7128;} _LL7128: goto _LL7129;
_LL7127: yyzzz= _temp7130; goto _LL7125; _LL7129:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL7125:;} yyzzz;}); _temp7123;}); _temp7122;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7121;}); _temp7120;}); break; case 384: _LL7119: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7133=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7133[ 0]=({ struct Cyc_Exp_tok_struct
_temp7134; _temp7134.tag= Cyc_Exp_tok; _temp7134.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp7135=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7135[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7136; _temp7136.tag= Cyc_Absyn_Codegen_e;
_temp7136.f1=({ struct Cyc_Absyn_Fndecl* yyzzz;{ void* _temp7137= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Fndecl*
_temp7143; _LL7139: if(*(( void**) _temp7137) ==  Cyc_FnDecl_tok){ _LL7144:
_temp7143=(( struct Cyc_FnDecl_tok_struct*) _temp7137)->f1; goto _LL7140;} else{
goto _LL7141;} _LL7141: goto _LL7142; _LL7140: yyzzz= _temp7143; goto _LL7138;
_LL7142:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL7138:;} yyzzz;});
_temp7136;}); _temp7135;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7134;});
_temp7133;}); break; case 385: _LL7132: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp7146=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7146[ 0]=({ struct Cyc_Exp_tok_struct _temp7147; _temp7147.tag= Cyc_Exp_tok;
_temp7147.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7148=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7148[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7149; _temp7149.tag=
Cyc_Absyn_UnknownId_e; _temp7149.f1=({ struct _tuple1* yyzzz;{ void* _temp7150=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp7156; _LL7152: if(*(( void**) _temp7150) ==  Cyc_QualId_tok){ _LL7157:
_temp7156=(( struct Cyc_QualId_tok_struct*) _temp7150)->f1; goto _LL7153;} else{
goto _LL7154;} _LL7154: goto _LL7155; _LL7153: yyzzz= _temp7156; goto _LL7151;
_LL7155:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7151:;} yyzzz;});
_temp7149;}); _temp7148;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7147;});
_temp7146;}); break; case 386: _LL7145: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 387: _LL7158:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7160=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7160[ 0]=({ struct Cyc_Exp_tok_struct
_temp7161; _temp7161.tag= Cyc_Exp_tok; _temp7161.f1= Cyc_Absyn_string_exp(({
struct _tagged_arr yyzzz;{ void* _temp7162= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp7168; _LL7164: if(*(( void**)
_temp7162) ==  Cyc_String_tok){ _LL7169: _temp7168=(( struct Cyc_String_tok_struct*)
_temp7162)->f1; goto _LL7165;} else{ goto _LL7166;} _LL7166: goto _LL7167;
_LL7165: yyzzz= _temp7168; goto _LL7163; _LL7167:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL7163:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7161;});
_temp7160;}); break; case 388: _LL7159: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 389:
_LL7170: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7172=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7172[ 0]=({ struct Cyc_Exp_tok_struct
_temp7173; _temp7173.tag= Cyc_Exp_tok; _temp7173.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7174=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7174[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7175; _temp7175.tag=
Cyc_Absyn_UnknownId_e; _temp7175.f1=({ struct _tuple1* yyzzz;{ void* _temp7176=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct
_tuple1* _temp7182; _LL7178: if(*(( void**) _temp7176) ==  Cyc_QualId_tok){
_LL7183: _temp7182=(( struct Cyc_QualId_tok_struct*) _temp7176)->f1; goto
_LL7179;} else{ goto _LL7180;} _LL7180: goto _LL7181; _LL7179: yyzzz= _temp7182;
goto _LL7177; _LL7181:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7177:;}
yyzzz;}); _temp7175;}); _temp7174;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7173;}); _temp7172;}); break; case 390: _LL7171: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7185=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7185[ 0]=({ struct Cyc_Exp_tok_struct
_temp7186; _temp7186.tag= Cyc_Exp_tok; _temp7186.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7187=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7187[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7188; _temp7188.tag=
Cyc_Absyn_UnknownId_e; _temp7188.f1=({ struct _tuple1* yyzzz;{ void* _temp7189=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct
_tuple1* _temp7195; _LL7191: if(*(( void**) _temp7189) ==  Cyc_QualId_tok){
_LL7196: _temp7195=(( struct Cyc_QualId_tok_struct*) _temp7189)->f1; goto
_LL7192;} else{ goto _LL7193;} _LL7193: goto _LL7194; _LL7192: yyzzz= _temp7195;
goto _LL7190; _LL7194:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7190:;}
yyzzz;}); _temp7188;}); _temp7187;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp7197= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7203; _LL7199: if(*(( void**) _temp7197) ==  Cyc_TypeList_tok){ _LL7204:
_temp7203=(( struct Cyc_TypeList_tok_struct*) _temp7197)->f1; goto _LL7200;}
else{ goto _LL7201;} _LL7201: goto _LL7202; _LL7200: yyzzz= _temp7203; goto
_LL7198; _LL7202:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL7198:;}
yyzzz;})), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp7186;}); _temp7185;}); break; case 391:
_LL7184: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7206=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7206[ 0]=({ struct Cyc_Exp_tok_struct
_temp7207; _temp7207.tag= Cyc_Exp_tok; _temp7207.f1= Cyc_Absyn_tuple_exp(({
struct Cyc_List_List* yyzzz;{ void* _temp7208= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7214; _LL7210: if(*(( void**) _temp7208) ==  Cyc_ExpList_tok){ _LL7215:
_temp7214=(( struct Cyc_ExpList_tok_struct*) _temp7208)->f1; goto _LL7211;}
else{ goto _LL7212;} _LL7212: goto _LL7213; _LL7211: yyzzz= _temp7214; goto
_LL7209; _LL7213:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL7209:;}
yyzzz;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp7207;}); _temp7206;}); break; case 392:
_LL7205: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7217=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7217[ 0]=({ struct Cyc_Exp_tok_struct
_temp7218; _temp7218.tag= Cyc_Exp_tok; _temp7218.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp7219=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7219[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7220; _temp7220.tag= Cyc_Absyn_Struct_e;
_temp7220.f1=({ struct _tuple1* yyzzz;{ void* _temp7221= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct _tuple1*
_temp7227; _LL7223: if(*(( void**) _temp7221) ==  Cyc_QualId_tok){ _LL7228:
_temp7227=(( struct Cyc_QualId_tok_struct*) _temp7221)->f1; goto _LL7224;} else{
goto _LL7225;} _LL7225: goto _LL7226; _LL7224: yyzzz= _temp7227; goto _LL7222;
_LL7226:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7222:;} yyzzz;});
_temp7220.f2= 0; _temp7220.f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp7229= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7235; _LL7231: if(*(( void**) _temp7229) ==  Cyc_InitializerList_tok){
_LL7236: _temp7235=(( struct Cyc_InitializerList_tok_struct*) _temp7229)->f1;
goto _LL7232;} else{ goto _LL7233;} _LL7233: goto _LL7234; _LL7232: yyzzz=
_temp7235; goto _LL7230; _LL7234:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL7230:;} yyzzz;})); _temp7220.f4= 0; _temp7220;}); _temp7219;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7218;}); _temp7217;}); break; case 393: _LL7216: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7238=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7238[ 0]=({ struct Cyc_Exp_tok_struct
_temp7239; _temp7239.tag= Cyc_Exp_tok; _temp7239.f1= Cyc_Absyn_stmt_exp(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp7240= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Stmt*
_temp7246; _LL7242: if(*(( void**) _temp7240) ==  Cyc_Stmt_tok){ _LL7247:
_temp7246=(( struct Cyc_Stmt_tok_struct*) _temp7240)->f1; goto _LL7243;} else{
goto _LL7244;} _LL7244: goto _LL7245; _LL7243: yyzzz= _temp7246; goto _LL7241;
_LL7245:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL7241:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7239;}); _temp7238;}); break; case 394: _LL7237: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp7249=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp7249[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7250; _temp7250.tag= Cyc_ExpList_tok; _temp7250.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{
void* _temp7251= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp7257; _LL7253: if(*(( void**) _temp7251) ==  Cyc_ExpList_tok){
_LL7258: _temp7257=(( struct Cyc_ExpList_tok_struct*) _temp7251)->f1; goto
_LL7254;} else{ goto _LL7255;} _LL7255: goto _LL7256; _LL7254: yyzzz= _temp7257;
goto _LL7252; _LL7256:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL7252:;}
yyzzz;})); _temp7250;}); _temp7249;}); break; case 395: _LL7248: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp7260=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7260[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7261; _temp7261.tag= Cyc_ExpList_tok; _temp7261.f1=({ struct Cyc_List_List*
_temp7262=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp7262->hd=( void*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7263= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp7269; _LL7265: if(*(( void**) _temp7263) ==  Cyc_Exp_tok){ _LL7270:
_temp7269=(( struct Cyc_Exp_tok_struct*) _temp7263)->f1; goto _LL7266;} else{
goto _LL7267;} _LL7267: goto _LL7268; _LL7266: yyzzz= _temp7269; goto _LL7264;
_LL7268:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7264:;} yyzzz;});
_temp7262->tl= 0; _temp7262;}); _temp7261;}); _temp7260;}); break; case 396:
_LL7259: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp7272=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7272[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7273; _temp7273.tag= Cyc_ExpList_tok; _temp7273.f1=({ struct Cyc_List_List*
_temp7274=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp7274->hd=( void*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7283= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp7289; _LL7285: if(*(( void**) _temp7283) ==  Cyc_Exp_tok){ _LL7290:
_temp7289=(( struct Cyc_Exp_tok_struct*) _temp7283)->f1; goto _LL7286;} else{
goto _LL7287;} _LL7287: goto _LL7288; _LL7286: yyzzz= _temp7289; goto _LL7284;
_LL7288:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7284:;} yyzzz;});
_temp7274->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp7275= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp7281; _LL7277: if(*(( void**) _temp7275) ==  Cyc_ExpList_tok){ _LL7282:
_temp7281=(( struct Cyc_ExpList_tok_struct*) _temp7275)->f1; goto _LL7278;}
else{ goto _LL7279;} _LL7279: goto _LL7280; _LL7278: yyzzz= _temp7281; goto
_LL7276; _LL7280:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL7276:;}
yyzzz;}); _temp7274;}); _temp7273;}); _temp7272;}); break; case 397: _LL7271:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7292=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7292[ 0]=({ struct Cyc_Exp_tok_struct
_temp7293; _temp7293.tag= Cyc_Exp_tok; _temp7293.f1= Cyc_Absyn_int_exp((*({
struct _tuple15* yyzzz;{ void* _temp7294= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tuple15* _temp7300; _LL7296: if(*(( void**)
_temp7294) ==  Cyc_Int_tok){ _LL7301: _temp7300=(( struct Cyc_Int_tok_struct*)
_temp7294)->f1; goto _LL7297;} else{ goto _LL7298;} _LL7298: goto _LL7299;
_LL7297: yyzzz= _temp7300; goto _LL7295; _LL7299:( int) _throw(( void*)& Cyc_yyfail_Int_tok);
_LL7295:;} yyzzz;})).f1,(*({ struct _tuple15* yyzzz;{ void* _temp7302= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple15*
_temp7308; _LL7304: if(*(( void**) _temp7302) ==  Cyc_Int_tok){ _LL7309:
_temp7308=(( struct Cyc_Int_tok_struct*) _temp7302)->f1; goto _LL7305;} else{
goto _LL7306;} _LL7306: goto _LL7307; _LL7305: yyzzz= _temp7308; goto _LL7303;
_LL7307:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL7303:;} yyzzz;})).f2,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp7293;}); _temp7292;}); break; case 398:
_LL7291: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7311=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7311[ 0]=({ struct Cyc_Exp_tok_struct
_temp7312; _temp7312.tag= Cyc_Exp_tok; _temp7312.f1= Cyc_Absyn_char_exp(({
unsigned char yyzzz;{ void* _temp7313= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; unsigned char _temp7319; _LL7315: if(*(( void**)
_temp7313) ==  Cyc_Char_tok){ _LL7320: _temp7319=(( struct Cyc_Char_tok_struct*)
_temp7313)->f1; goto _LL7316;} else{ goto _LL7317;} _LL7317: goto _LL7318;
_LL7316: yyzzz= _temp7319; goto _LL7314; _LL7318:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL7314:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7312;});
_temp7311;}); break; case 399: _LL7310: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp7322=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7322[ 0]=({ struct Cyc_Exp_tok_struct _temp7323; _temp7323.tag= Cyc_Exp_tok;
_temp7323.f1= Cyc_Absyn_float_exp(({ struct _tagged_arr yyzzz;{ void* _temp7324=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp7330; _LL7326: if(*(( void**) _temp7324) ==  Cyc_String_tok){ _LL7331:
_temp7330=(( struct Cyc_String_tok_struct*) _temp7324)->f1; goto _LL7327;} else{
goto _LL7328;} _LL7328: goto _LL7329; _LL7327: yyzzz= _temp7330; goto _LL7325;
_LL7329:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL7325:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7323;});
_temp7322;}); break; case 400: _LL7321: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp7333=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7333[ 0]=({ struct Cyc_Exp_tok_struct _temp7334; _temp7334.tag= Cyc_Exp_tok;
_temp7334.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7334;});
_temp7333;}); break; case 401: _LL7332: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp7336=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp7336[ 0]=({ struct Cyc_QualId_tok_struct _temp7337; _temp7337.tag= Cyc_QualId_tok;
_temp7337.f1=({ struct _tuple1* _temp7338=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp7338->f1= Cyc_Absyn_rel_ns_null; _temp7338->f2=({ struct
_tagged_arr* _temp7339=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp7339[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp7340= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp7346; _LL7342: if(*(( void**) _temp7340) ==  Cyc_String_tok){ _LL7347:
_temp7346=(( struct Cyc_String_tok_struct*) _temp7340)->f1; goto _LL7343;} else{
goto _LL7344;} _LL7344: goto _LL7345; _LL7343: yyzzz= _temp7346; goto _LL7341;
_LL7345:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL7341:;} yyzzz;});
_temp7339;}); _temp7338;}); _temp7337;}); _temp7336;}); break; case 402: _LL7335:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break;
default: _LL7348:( int) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -=
yylen; yyssp_offset -= yylen; yylsp_offset -= yylen; yyvs[
_check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= yyval; yylsp_offset
++; if( yylen ==  0){( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line= Cyc_yylloc.first_line;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_column= Cyc_yylloc.first_column;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_column;} else{(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset +  yylen) -  1)]).last_line;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset +  yylen) -  1)]).last_column;}
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 403u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  120)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4390: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4391u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4391u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
120)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 794u, yystate)]; if( yyn >  -
32768? yyn <  4390: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  232u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4391u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 232u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp7350=( unsigned int)( sze +  15);
unsigned char* _temp7351=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp7350)); struct _tagged_arr _temp7353= _tag_arr(
_temp7351, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp7352= _temp7350; unsigned int i; for( i= 0; i <  _temp7352; i ++){
_temp7351[ i]='\000';}}; _temp7353;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  232u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4391u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 232u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 794u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4390)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4391u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4391u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  793){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp7363= v; struct
_tuple15* _temp7381; struct _tuple15 _temp7383; int _temp7384; unsigned char
_temp7386; short _temp7388; struct _tagged_arr _temp7390; struct Cyc_Core_Opt*
_temp7392; struct Cyc_Core_Opt* _temp7394; struct Cyc_Core_Opt _temp7396; struct
_tagged_arr* _temp7397; struct _tuple1* _temp7399; struct _tuple1 _temp7401;
struct _tagged_arr* _temp7402; void* _temp7404; _LL7365: if(*(( void**)
_temp7363) ==  Cyc_Int_tok){ _LL7382: _temp7381=(( struct Cyc_Int_tok_struct*)
_temp7363)->f1; _temp7383=* _temp7381; _LL7385: _temp7384= _temp7383.f2; goto
_LL7366;} else{ goto _LL7367;} _LL7367: if(*(( void**) _temp7363) ==  Cyc_Char_tok){
_LL7387: _temp7386=(( struct Cyc_Char_tok_struct*) _temp7363)->f1; goto _LL7368;}
else{ goto _LL7369;} _LL7369: if(*(( void**) _temp7363) ==  Cyc_Short_tok){
_LL7389: _temp7388=(( struct Cyc_Short_tok_struct*) _temp7363)->f1; goto _LL7370;}
else{ goto _LL7371;} _LL7371: if(*(( void**) _temp7363) ==  Cyc_String_tok){
_LL7391: _temp7390=(( struct Cyc_String_tok_struct*) _temp7363)->f1; goto
_LL7372;} else{ goto _LL7373;} _LL7373: if(*(( void**) _temp7363) ==  Cyc_Stringopt_tok){
_LL7393: _temp7392=(( struct Cyc_Stringopt_tok_struct*) _temp7363)->f1; if(
_temp7392 ==  0){ goto _LL7374;} else{ goto _LL7375;}} else{ goto _LL7375;}
_LL7375: if(*(( void**) _temp7363) ==  Cyc_Stringopt_tok){ _LL7395: _temp7394=((
struct Cyc_Stringopt_tok_struct*) _temp7363)->f1; if( _temp7394 ==  0){ goto
_LL7377;} else{ _temp7396=* _temp7394; _LL7398: _temp7397=( struct _tagged_arr*)
_temp7396.v; goto _LL7376;}} else{ goto _LL7377;} _LL7377: if(*(( void**)
_temp7363) ==  Cyc_QualId_tok){ _LL7400: _temp7399=(( struct Cyc_QualId_tok_struct*)
_temp7363)->f1; _temp7401=* _temp7399; _LL7405: _temp7404= _temp7401.f1; goto
_LL7403; _LL7403: _temp7402= _temp7401.f2; goto _LL7378;} else{ goto _LL7379;}
_LL7379: goto _LL7380; _LL7366:({ struct Cyc_Std_Int_pa_struct _temp7407;
_temp7407.tag= Cyc_Std_Int_pa; _temp7407.f1=( int)(( unsigned int) _temp7384);{
void* _temp7406[ 1u]={& _temp7407}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp7406, sizeof( void*), 1u));}}); goto
_LL7364; _LL7368:({ struct Cyc_Std_Int_pa_struct _temp7409; _temp7409.tag= Cyc_Std_Int_pa;
_temp7409.f1=( int)(( unsigned int)(( int) _temp7386));{ void* _temp7408[ 1u]={&
_temp7409}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp7408, sizeof( void*), 1u));}}); goto _LL7364;
_LL7370:({ struct Cyc_Std_Int_pa_struct _temp7411; _temp7411.tag= Cyc_Std_Int_pa;
_temp7411.f1=( int)(( unsigned int)(( int) _temp7388));{ void* _temp7410[ 1u]={&
_temp7411}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp7410, sizeof( void*), 1u));}}); goto _LL7364;
_LL7372:({ struct Cyc_Std_String_pa_struct _temp7413; _temp7413.tag= Cyc_Std_String_pa;
_temp7413.f1=( struct _tagged_arr) _temp7390;{ void* _temp7412[ 1u]={& _temp7413};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp7412, sizeof( void*), 1u));}}); goto _LL7364; _LL7374:({ void*
_temp7414[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp7414, sizeof( void*), 0u));}); goto _LL7364;
_LL7376:({ struct Cyc_Std_String_pa_struct _temp7416; _temp7416.tag= Cyc_Std_String_pa;
_temp7416.f1=( struct _tagged_arr)* _temp7397;{ void* _temp7415[ 1u]={&
_temp7416}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp7415, sizeof( void*), 1u));}}); goto _LL7364;
_LL7378: { struct Cyc_List_List* _temp7417= 0;{ void* _temp7418= _temp7404;
struct Cyc_List_List* _temp7426; struct Cyc_List_List* _temp7428; _LL7420: if((
unsigned int) _temp7418 >  1u?*(( int*) _temp7418) ==  Cyc_Absyn_Rel_n: 0){
_LL7427: _temp7426=(( struct Cyc_Absyn_Rel_n_struct*) _temp7418)->f1; goto
_LL7421;} else{ goto _LL7422;} _LL7422: if(( unsigned int) _temp7418 >  1u?*((
int*) _temp7418) ==  Cyc_Absyn_Abs_n: 0){ _LL7429: _temp7428=(( struct Cyc_Absyn_Abs_n_struct*)
_temp7418)->f1; goto _LL7423;} else{ goto _LL7424;} _LL7424: if( _temp7418 == (
void*) Cyc_Absyn_Loc_n){ goto _LL7425;} else{ goto _LL7419;} _LL7421: _temp7417=
_temp7426; goto _LL7419; _LL7423: _temp7417= _temp7428; goto _LL7419; _LL7425:
goto _LL7419; _LL7419:;} for( 0; _temp7417 !=  0; _temp7417=(( struct Cyc_List_List*)
_check_null( _temp7417))->tl){({ struct Cyc_Std_String_pa_struct _temp7431;
_temp7431.tag= Cyc_Std_String_pa; _temp7431.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp7417))->hd);{ void*
_temp7430[ 1u]={& _temp7431}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp7430, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp7433; _temp7433.tag= Cyc_Std_String_pa;
_temp7433.f1=( struct _tagged_arr)* _temp7402;{ void* _temp7432[ 1u]={&
_temp7433}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp7432, sizeof( void*), 1u));}}); goto _LL7364;}
_LL7380:({ void* _temp7434[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp7434, sizeof( void*), 0u));}); goto
_LL7364; _LL7364:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7435=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp7435->v=(
void*) Cyc_Lexing_from_file( f); _temp7435;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
