 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); struct Cyc_std___sFILE; extern struct Cyc_std___sFILE* Cyc_std_stderr;
extern unsigned char Cyc_std_FileCloseError[ 19u]; extern unsigned char Cyc_std_FileOpenError[
18u]; struct Cyc_std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_std_String_pa= 0; struct Cyc_std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_std_Int_pa= 1; struct
Cyc_std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_std_Double_pa=
2; struct Cyc_std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa=
3; struct Cyc_std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa= 0;
struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_std_UShortPtr_sa=
1; struct Cyc_std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_std___sFILE*);
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
Cyc_List_List* x); extern unsigned int Cyc_std_strlen( struct _tagged_arr s);
extern int Cyc_std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_std_zstrcmp( struct _tagged_arr, struct _tagged_arr); extern int Cyc_std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_std_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_std_strcpy(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_std_substring(
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
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct Cyc_std_String_pa_struct
_temp19; _temp19.tag= Cyc_std_String_pa; _temp19.f1=( struct _tagged_arr) msg;{
struct Cyc_std_String_pa_struct _temp18; _temp18.tag= Cyc_std_String_pa; _temp18.f1=(
struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void* _temp17[ 2u]={&
_temp18,& _temp19}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s: Warning: %s\n",
sizeof( unsigned char), 17u), _tag_arr( _temp17, sizeof( void*), 2u));}}});
return;} static void* Cyc_Parse_unimp( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ return Cyc_Parse_abort(( struct _tagged_arr)({ struct Cyc_std_String_pa_struct
_temp21; _temp21.tag= Cyc_std_String_pa; _temp21.f1=( struct _tagged_arr) msg;{
void* _temp20[ 1u]={& _temp21}; Cyc_std_aprintf( _tag_arr("%s unimplemented",
sizeof( unsigned char), 17u), _tag_arr( _temp20, sizeof( void*), 1u));}}), sg);}
static void Cyc_Parse_unimp2( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){({ struct Cyc_std_String_pa_struct _temp24; _temp24.tag= Cyc_std_String_pa;
_temp24.f1=( struct _tagged_arr) msg;{ struct Cyc_std_String_pa_struct _temp23;
_temp23.tag= Cyc_std_String_pa; _temp23.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
sg);{ void* _temp22[ 2u]={& _temp23,& _temp24}; Cyc_std_fprintf( Cyc_std_stderr,
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
_temp43=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
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
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp56->v=(
void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp53))->v)).f2;
_temp56;});} if( _temp47 !=  0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("parameter should have no type parameters",
sizeof( unsigned char), 41u), loc);} return({ struct _tuple2* _temp57=( struct
_tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp57->f1= _temp55; _temp57->f2=
_temp51; _temp57->f3= _temp49; _temp57;});}} static void* Cyc_Parse_type_spec(
void* t, struct Cyc_Position_Segment* loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp58=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
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
_temp72=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp72[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp73; _temp73.tag= Cyc_Absyn_Upper_b;
_temp73.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp66); _temp73;}); _temp72;}));
_LL65: return t; _LL61:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp74=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp74->f1=(* t).f2; _temp74->f2=(* t).f3;
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
params=(( struct Cyc_List_List*) _check_null( params))->tl){ if( Cyc_std_zstrptrcmp((*
_temp101->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
params))->hd) ==  0){ found= 1; break;}} if( ! found){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_std_String_pa_struct _temp104; _temp104.tag= Cyc_std_String_pa;
_temp104.f1=( struct _tagged_arr)*(* _temp101->name).f2;{ void* _temp103[ 1u]={&
_temp104}; Cyc_std_aprintf( _tag_arr("%s is not listed as a parameter", sizeof(
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
struct Cyc_std_String_pa_struct _temp106; _temp106.tag= Cyc_std_String_pa;
_temp106.f1=( struct _tagged_arr) decl_kind;{ void* _temp105[ 1u]={& _temp106};
Cyc_std_aprintf( _tag_arr("%s appears in parameter type", sizeof( unsigned char),
29u), _tag_arr( _temp105, sizeof( void*), 1u));}}), x->loc); return;} struct
_tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_arr* x){ struct _tuple12 _temp109; struct Cyc_Position_Segment* _temp110;
struct Cyc_List_List* _temp112; struct _tuple12* _temp107= env; _temp109=*
_temp107; _LL113: _temp112= _temp109.f1; goto _LL111; _LL111: _temp110= _temp109.f2;
goto _LL108; _LL108: if( _temp112 ==  0){ return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_std_String_pa_struct _temp115; _temp115.tag= Cyc_std_String_pa;
_temp115.f1=( struct _tagged_arr)* x;{ void* _temp114[ 1u]={& _temp115}; Cyc_std_aprintf(
_tag_arr("missing type for parameter %s", sizeof( unsigned char), 30u), _tag_arr(
_temp114, sizeof( void*), 1u));}}), _temp110);}{ void* _temp116=( void*)((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp112))->hd)->r;
struct Cyc_Absyn_Vardecl* _temp122; _LL118: if(*(( int*) _temp116) ==  Cyc_Absyn_Var_d){
_LL123: _temp122=(( struct Cyc_Absyn_Var_d_struct*) _temp116)->f1; goto _LL119;}
else{ goto _LL120;} _LL120: goto _LL121; _LL119: if( Cyc_Absyn_is_qvar_qualified(
_temp122->name)){ Cyc_Parse_err( _tag_arr("namespace not allowed on parameter",
sizeof( unsigned char), 35u), _temp110);} if( Cyc_std_zstrptrcmp((* _temp122->name).f2,
x) ==  0){ return({ struct _tuple2* _temp124=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2)); _temp124->f1=({ struct Cyc_Core_Opt* _temp125=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp125->v=( void*)(*
_temp122->name).f2; _temp125;}); _temp124->f2= _temp122->tq; _temp124->f3=( void*)
_temp122->type; _temp124;});} else{ return Cyc_Parse_get_param_type(({ struct
_tuple12* _temp126=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp126->f1=(( struct Cyc_List_List*) _check_null( _temp112))->tl; _temp126->f2=
_temp110; _temp126;}), x);} _LL121: return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration",
sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp112))->hd)->loc); _LL117:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp127= tm; _LL129: if(( unsigned int) _temp127 >  1u?*(( int*)
_temp127) ==  Cyc_Absyn_TypeParams_mod: 0){ goto _LL130;} else{ goto _LL131;}
_LL131: goto _LL132; _LL130: return 1; _LL132: return 0; _LL128:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_std_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp133=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp133[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp134; _temp134.tag= Cyc_Absyn_VarType; _temp134.f1=({ struct Cyc_Absyn_Tvar*
_temp135=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp135->name=({ struct _tagged_arr* _temp136=( struct _tagged_arr*) GC_malloc(
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
_temp145=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
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
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp168[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp169; _temp169.tag= Cyc_Absyn_Eq_constr; _temp169.f1=(
void*) k; _temp169;}); _temp168;}))); return 1; _LL163: return k ==  _temp166;
_LL165:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp170=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
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
GC_malloc( sizeof( struct _tuple12)); _temp194->f1= tds; _temp194->f2= loc;
_temp194;}); return({ struct Cyc_List_List* _temp189=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp189->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp190=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp190[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp191; _temp191.tag= Cyc_Absyn_Function_mod;
_temp191.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp192=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp192[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp193; _temp193.tag= Cyc_Absyn_WithTypes;
_temp193.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp188, _temp186); _temp193.f2= 0; _temp193.f3= 0;
_temp193.f4= 0; _temp193.f5= 0; _temp193;}); _temp192;})); _temp191;}); _temp190;}));
_temp189->tl= 0; _temp189;});} _LL181:;} else{ return({ struct Cyc_List_List*
_temp195=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp195->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp195->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp195;});} _LL177: return({ struct Cyc_List_List*
_temp196=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp196->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp196->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp196;}); _LL173:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds !=  0){ d=({ struct Cyc_Parse_Declarator* _temp197=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp197->id= d->id; _temp197->tms=
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
_temp244=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
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
GC_malloc( sizeof( struct _tuple4)); _temp245->f1= _init_tag_arr(( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp245->f2=(* t).f2; _temp245->f3=(* t).f3; _temp245;});} else{ return({
struct _tuple4* _temp246=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
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
_temp306=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp306[ 0]=({ struct Cyc_Absyn_StructType_struct _temp307; _temp307.tag= Cyc_Absyn_StructType;
_temp307.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp297->name))->v); _temp307.f2= _temp305; _temp307.f3= 0;
_temp307;}); _temp306;}); if( _temp297->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp308=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp308->v=( void*) _temp277; _temp308;});} goto _LL286;} _LL290: { struct Cyc_List_List*
_temp309=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp299->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp310=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp310[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp311; _temp311.tag= Cyc_Absyn_TunionType;
_temp311.f1=({ struct Cyc_Absyn_TunionInfo _temp312; _temp312.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp313=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp313[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp314; _temp314.tag= Cyc_Absyn_KnownTunion;
_temp314.f1= _temp299; _temp314;}); _temp313;})); _temp312.targs= _temp309;
_temp312.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp312;}); _temp311;});
_temp310;}); if( _temp299->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp315=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp315->v=( void*) _temp277; _temp315;});} goto _LL286;} _LL292: { struct Cyc_List_List*
_temp316=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp301->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp317=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp317[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp318; _temp318.tag= Cyc_Absyn_UnionType; _temp318.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp301->name))->v); _temp318.f2= _temp316; _temp318.f3= 0; _temp318;});
_temp317;}); if( _temp301->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp319=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp319->v=( void*) _temp277; _temp319;});} goto _LL286;} _LL294: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp320=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp320[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp321; _temp321.tag= Cyc_Absyn_EnumType; _temp321.f1=
_temp303->name; _temp321.f2= 0; _temp321;}); _temp320;}); declopt=({ struct Cyc_Core_Opt*
_temp322=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp322->v=( void*) _temp277; _temp322;}); goto _LL286; _LL296:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp277->loc); goto _LL286; _LL286:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp277->loc);} goto _LL252; _LL252:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp323=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp323[ 0]=({ struct Cyc_Absyn_IntType_struct _temp324; _temp324.tag= Cyc_Absyn_IntType;
_temp324.f1=( void*) sgn; _temp324.f2=( void*) sz; _temp324;}); _temp323;});}
else{ if( seen_sign){ void* _temp325= t; void* _temp331; _LL327: if((
unsigned int) _temp325 >  4u?*(( int*) _temp325) ==  Cyc_Absyn_IntType: 0){
_LL332: _temp331=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp325)->f2;
goto _LL328;} else{ goto _LL329;} _LL329: goto _LL330; _LL328: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp333=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp333[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp334; _temp334.tag= Cyc_Absyn_IntType; _temp334.f1=( void*) sgn; _temp334.f2=(
void*) _temp331; _temp334;}); _temp333;}); goto _LL326; _LL330: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL326; _LL326:;} if( seen_size){ void* _temp335= t; void*
_temp341; _LL337: if(( unsigned int) _temp335 >  4u?*(( int*) _temp335) ==  Cyc_Absyn_IntType:
0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp335)->f1;
goto _LL338;} else{ goto _LL339;} _LL339: goto _LL340; _LL338: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp343=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp343[ 0]=({ struct Cyc_Absyn_IntType_struct
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
GC_malloc( sizeof( struct Cyc_List_List)); _temp358->hd=( void*)({ struct
_tuple7* _temp359=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
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
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp393[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp394; _temp394.tag= Cyc_Absyn_ArrayType; _temp394.f1=(
void*) t; _temp394.f2= tq; _temp394.f3= 0; _temp394;}); _temp393;}), atts,((
struct Cyc_List_List*) _check_null( tms))->tl); _LL366: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp395=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
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
_temp415=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp415->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp415->tl= fn_atts; _temp415;});} else{ new_atts=({ struct Cyc_List_List*
_temp416=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
_temp438=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp438[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp439; _temp439.tag= Cyc_Absyn_Upper_b;
_temp439.f1= _temp434; _temp439;}); _temp438;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL431: return Cyc_Parse_apply_tms( _temp383, Cyc_Absyn_starb_typ(
t, _temp385, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp440=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
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
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
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
return({ struct Cyc_List_List* _temp491=( struct Cyc_List_List*) GC_malloc(
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
struct Cyc_Absyn_Structdecl* _temp539=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp539->sc=( void*) s; _temp539->name=({
struct Cyc_Core_Opt* _temp540=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp540->v=( void*)(( struct _tuple1*) _check_null( _temp508));
_temp540;}); _temp539->tvs= _temp534; _temp539->fields= 0; _temp539->attributes=
0; _temp539;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp536=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp536->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp537=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp537[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp538; _temp538.tag= Cyc_Absyn_Struct_d;
_temp538.f1= _temp535; _temp538;}); _temp537;}), loc); _temp536->tl= 0; _temp536;});}
_LL497: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp541=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp541->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp542=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp542[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp543; _temp543.tag= Cyc_Absyn_Tunion_d;
_temp543.f1= _temp514; _temp543;}); _temp542;}), loc); _temp541->tl= 0; _temp541;});
_LL499: { struct Cyc_List_List* _temp544=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp518);
struct Cyc_Absyn_Decl* _temp545= Cyc_Absyn_tunion_decl( s, _temp526, _temp544, 0,
_temp524, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp546=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp546->hd=(
void*) _temp545; _temp546->tl= 0; _temp546;});} _LL501: { struct Cyc_List_List*
_temp547=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp528); struct Cyc_Absyn_Uniondecl* _temp548=({
struct Cyc_Absyn_Uniondecl* _temp553=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp553->sc=( void*) s; _temp553->name=({
struct Cyc_Core_Opt* _temp554=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp554->v=( void*)(( struct _tuple1*) _check_null( _temp530));
_temp554;}); _temp553->tvs= _temp547; _temp553->fields= 0; _temp553->attributes=
0; _temp553;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp549=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp549->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp550=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp550->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp551=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp551[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp552; _temp552.tag= Cyc_Absyn_Union_d;
_temp552.f1= _temp548; _temp552;}); _temp551;})); _temp550->loc= loc; _temp550;});
_temp549->tl= 0; _temp549;});} _LL503: { struct Cyc_Absyn_Enumdecl* _temp555=({
struct Cyc_Absyn_Enumdecl* _temp560=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp560->sc=( void*) s; _temp560->name=
_temp532; _temp560->fields= 0; _temp560;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp556=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp556->hd=( void*)({ struct Cyc_Absyn_Decl* _temp557=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp557->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp558=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp558[ 0]=({ struct Cyc_Absyn_Enum_d_struct
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
_temp582=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp582->hd=( void*) d; _temp582->tl= decls; _temp582;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp584; _temp584.tag=
Cyc_std_String_pa; _temp584.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp583[ 1u]={&
_temp584}; Cyc_std_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
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
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp603->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp604=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp604[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp605; _temp605.tag= Cyc_Absyn_Var_d; _temp605.f1= _temp600; _temp605;});
_temp604;})); _temp603->loc= loc; _temp603;}); decls=({ struct Cyc_List_List*
_temp602=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp602->hd=( void*) _temp601; _temp602->tl= decls; _temp602;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_std_String_pa_struct _temp607; _temp607.tag= Cyc_std_String_pa;
_temp607.f1=( struct _tagged_arr) s;{ void* _temp606[ 1u]={& _temp607}; Cyc_std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp606,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL608: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL609: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL610: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL611: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL612: return(
void*) Cyc_Absyn_EffKind; default: _LL613: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_std_String_pa_struct _temp616; _temp616.tag= Cyc_std_String_pa;
_temp616.f1=( struct _tagged_arr) s;{ void* _temp615[ 1u]={& _temp616}; Cyc_std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp615,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp617=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp617->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp618=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp618[ 0]=({
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
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp634;
_temp634.tag= Cyc_std_String_pa; _temp634.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp623))->hd);{ void* _temp633[ 1u]={&
_temp634}; Cyc_std_aprintf( _tag_arr("bad attribute %s within typedef", sizeof(
unsigned char), 32u), _tag_arr( _temp633, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp635=(
struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp635[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp636; _temp636.tag= Cyc_Absyn_Typedef_d;
_temp636.f1=({ struct Cyc_Absyn_Typedefdecl* _temp637=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp637->name= _temp631;
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
1, 1, 2, 3, 5, 6, 8, 1, 3, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 4, 1, 1, 3, 1, 3,
2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 4,
3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2,
2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5,
5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1,
1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3,
1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6,
7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4,
5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 796u]={ 0, 137, 35, 36,
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
96, 0, 181, 0, 0, 161, 159, 0, 0, 128, 0, 138, 0, 0, 78, 0, 76, 281, 296, 0, 295,
182, 0, 0, 294, 0, 201, 173, 0, 99, 101, 159, 0, 207, 199, 208, 68, 0, 87, 0, 86,
0, 177, 0, 179, 67, 0, 0, 364, 0, 320, 354, 0, 321, 322, 0, 0, 0, 0, 0, 0, 357,
358, 0, 0, 0, 0, 360, 361, 359, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 307, 0, 362, 0,
379, 380, 0, 0, 0, 389, 0, 0, 145, 18, 0, 20, 0, 95, 97, 184, 183, 14, 0, 80, 92,
0, 0, 103, 104, 106, 0, 110, 85, 119, 0, 0, 0, 0, 0, 0, 0, 273, 274, 275, 0, 0,
277, 0, 230, 231, 0, 0, 0, 0, 242, 238, 96, 240, 236, 234, 0, 165, 0, 0, 172,
160, 167, 130, 0, 0, 0, 161, 132, 134, 133, 129, 151, 140, 139, 0, 48, 47, 0, 45,
0, 74, 0, 289, 0, 23, 290, 0, 0, 0, 0, 194, 298, 301, 0, 300, 0, 0, 0, 202, 100,
102, 0, 161, 0, 211, 0, 209, 159, 0, 0, 221, 204, 206, 176, 0, 0, 0, 185, 189, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 388, 395, 0, 394, 327, 0, 329, 331, 333, 335, 337,
338, 342, 343, 340, 341, 345, 346, 348, 349, 351, 352, 353, 306, 377, 378, 373,
0, 375, 376, 0, 0, 0, 4, 6, 0, 325, 107, 98, 0, 0, 0, 112, 121, 120, 0, 0, 115,
0, 0, 0, 0, 0, 0, 0, 0, 272, 276, 0, 0, 0, 229, 0, 232, 0, 15, 304, 166, 0, 0,
169, 0, 0, 0, 162, 131, 159, 153, 152, 141, 7, 0, 0, 0, 79, 77, 297, 197, 0, 299,
193, 195, 291, 0, 288, 203, 174, 216, 0, 210, 213, 0, 161, 0, 212, 0, 363, 0, 0,
186, 0, 190, 383, 384, 0, 0, 0, 0, 0, 0, 0, 355, 391, 0, 0, 374, 372, 392, 0, 0,
82, 105, 108, 81, 109, 122, 119, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 278, 0,
0, 16, 159, 0, 170, 168, 0, 165, 161, 0, 0, 0, 44, 46, 196, 302, 215, 218, 0,
220, 219, 214, 0, 0, 187, 191, 0, 0, 323, 324, 0, 393, 0, 396, 319, 390, 0, 118,
117, 243, 0, 248, 258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 279, 0, 161, 171, 163,
0, 154, 0, 49, 50, 217, 365, 0, 192, 0, 0, 381, 0, 0, 0, 254, 0, 0, 260, 0, 0, 0,
268, 0, 0, 0, 0, 0, 0, 0, 0, 247, 228, 155, 0, 159, 0, 0, 366, 0, 382, 0, 244, 0,
0, 0, 245, 259, 261, 262, 0, 270, 269, 0, 264, 0, 0, 0, 0, 248, 249, 164, 161, 0,
0, 0, 123, 0, 0, 246, 263, 271, 265, 266, 0, 0, 248, 250, 156, 0, 188, 367, 254,
255, 267, 248, 251, 51, 254, 256, 248, 252, 257, 253, 0, 0, 0}; static short Cyc_yydefgoto[
112u]={ 793, 40, 41, 42, 238, 43, 367, 44, 369, 45, 205, 46, 47, 63, 64, 435,
436, 48, 147, 49, 50, 267, 268, 51, 86, 52, 211, 212, 97, 98, 99, 213, 142, 379,
380, 381, 53, 54, 533, 534, 535, 55, 56, 57, 58, 122, 108, 431, 459, 59, 460,
422, 562, 556, 418, 419, 281, 259, 153, 77, 78, 476, 372, 477, 478, 447, 448,
143, 148, 149, 461, 288, 241, 242, 243, 244, 245, 246, 693, 744, 247, 248, 271,
272, 273, 450, 451, 452, 249, 250, 354, 179, 525, 180, 181, 182, 183, 184, 185,
186, 187, 188, 189, 190, 191, 192, 193, 194, 490, 491, 195, 196}; static short
Cyc_yypact[ 796u]={ 1998, - -32768, - -32768, - -32768, - -32768, 5, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - 38, 145, 720, -
-32768, 145, - 16, - -32768, - -32768, 9, - -32768, 113, - -32768, 116, 208, -
-32768, 137, 162, 191, - -32768, - -32768, 1887, - -32768, 35, 55, - -32768, 468,
- 38, - 38, - 38, - -32768, - -32768, 107, - -32768, 133, 2434, 156, 92, 37, -
-32768, 145, 319, 228, 2489, - -32768, 328, - -32768, 176, - -32768, - -32768, -
-32768, 780, - -32768, 299, 322, 356, 780, 353, 359, 365, 206, - -32768, -
-32768, 4261, 155, 4261, - -32768, 379, 2048, 3894, 3894, - -32768, 1887, 2108,
1887, 2108, - -32768, 400, 401, - -32768, 2376, 2489, 2489, 2489, 116, 2048, 116,
- -32768, 145, - -32768, 234, 423, 1309, - -32768, 2434, 468, - -32768, 1828,
4261, 2838, - -32768, 156, - 38, - -32768, - -32768, 2108, 417, - -32768, 145,
425, 780, - -32768, - -32768, - -32768, - -32768, 443, 3894, 336, 441, 195, - 38,
- 38, 173, - -32768, 74, - -32768, - -32768, - -32768, 430, - 30, - -32768, 527,
175, - -32768, 4042, 446, - -32768, 3894, 3935, 448, 451, 453, 456, 462, 4108,
4108, - -32768, - -32768, - -32768, - -32768, 1655, 469, 4149, 4149, - -32768,
4149, - -32768, - -32768, 492, - -32768, - 28, 525, 478, 487, 483, 376, 101, 386,
360, 180, - -32768, 898, 4149, 393, - -32768, - -32768, 15, 504, - -32768, 513,
- -32768, 524, - -32768, 208, 2904, 2434, - -32768, - -32768, - -32768, - -32768,
535, 528, 2048, - -32768, 122, 540, 116, 145, 546, - -32768, 548, 58, 551, 2541,
552, 568, 560, 562, 2970, 2541, 67, 2541, 2541, 4, 553, - -32768, - -32768, 559,
1426, 1426, 468, 1426, - -32768, - -32768, - -32768, 563, - -32768, - -32768, -
-32768, 233, - -32768, - -32768, - -32768, 566, 37, - -32768, 19, 561, 564, 245,
567, - 27, - -32768, 565, 134, 572, 11, 570, 580, 583, - -32768, - -32768, 585,
581, - -32768, 247, 576, 780, 4261, - -32768, 586, 591, 2048, 2048, 1082, 3036,
30, - -32768, 174, - -32768, 19, - -32768, 4261, - -32768, 2268, - -32768, 491,
- -32768, - -32768, 2048, 1655, - -32768, 2048, - -32768, - -32768, 901, -
-32768, 617, 3894, 2218, 665, 3894, 3894, 1655, - -32768, - -32768, 1426, 593,
377, 3894, - -32768, - -32768, - -32768, - -32768, 4149, 3894, 4149, 4149, 4149,
4149, 4149, 4149, 4149, 4149, 4149, 4149, 4149, 4149, 4149, 4149, 4149, 4149,
4149, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, 3894, - -32768, 231, - -32768, - -32768,
3102, 236, 3894, - -32768, 2761, 590, - -32768, - -32768, 1887, - -32768, 1887,
- -32768, - -32768, - -32768, - -32768, - -32768, 2048, - -32768, - -32768, 3894,
601, 596, - -32768, 592, 2048, - -32768, 116, 481, 3894, 600, 3894, 3894, 668,
1543, 606, - -32768, - -32768, - -32768, 258, 658, - -32768, 3168, - -32768, -
-32768, 2541, 613, 2541, 1767, - -32768, - -32768, 2376, - -32768, - -32768, -
-32768, 3894, 620, 608, 291, - -32768, - -32768, 595, - -32768, 619, 611, 4323,
564, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 134, 1887, -
-32768, 614, 612, 616, 145, - -32768, 3894, - -32768, 780, - -32768, - -32768,
629, 3894, 780, 75, - -32768, - -32768, - -32768, 622, 621, 623, - 72, 630, -
-32768, - -32768, - -32768, 624, 564, 625, - -32768, 618, 174, 4275, 4261, 3234,
- -32768, - -32768, 430, - -32768, 633, 628, 648, - -32768, - -32768, 215, 2904,
405, 637, 208, 646, 407, 645, 647, 654, 4001, - -32768, - -32768, 653, 662, 525,
153, 478, 487, 483, 376, 101, 101, 386, 386, 386, 386, 360, 360, 180, 180, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 664, -
-32768, - -32768, 160, 257, 4261, - -32768, - -32768, 671, - -32768, - -32768, -
-32768, 122, 3894, 674, - -32768, - -32768, - -32768, 676, 145, 292, 409, 3894,
419, 422, 675, 3300, 3366, 294, - -32768, - -32768, 679, 682, 678, - -32768, 681,
- -32768, 2434, - -32768, - -32768, - -32768, 2048, 4261, - -32768, 683, 19, 677,
- -32768, - -32768, - 6, - -32768, - -32768, - -32768, - -32768, 382, 684, 11, -
-32768, - -32768, - -32768, - -32768, 687, - -32768, - -32768, - -32768, -
-32768, 680, - -32768, - -32768, - -32768, - -32768, 685, - -32768, - -32768,
689, 564, 16, - -32768, 688, 696, 712, 695, - -32768, 2607, - -32768, - -32768,
- -32768, 2434, 2048, 3935, 773, 696, 702, 2761, - -32768, - -32768, 3894, 3894,
- -32768, - -32768, - -32768, 2761, 264, - -32768, - -32768, - -32768, - -32768,
- -32768, 273, 25, 481, 2541, 431, 707, 2541, 3894, 3432, 295, 3498, 302, 3564,
541, - -32768, 710, 722, - -32768, 716, - 13, - -32768, - -32768, 723, 620, 564,
715, 713, 717, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 721,
- -32768, - -32768, - -32768, 724, 3894, - -32768, - -32768, 2904, 725, - -32768,
617, 727, - -32768, 333, - -32768, - -32768, - -32768, 729, - -32768, - -32768,
799, 735, 541, - -32768, 472, 2541, 475, 3630, 2541, 484, 3696, 3762, 317, 780,
726, 736, - -32768, 2541, 564, - -32768, 737, 2048, - -32768, 749, - -32768, -
-32768, - -32768, - -32768, 256, - -32768, 740, 2048, - -32768, 2684, 2048, 2541,
557, 744, 742, - -32768, 2541, 2541, 507, - -32768, 2541, 2541, 514, 2541, 517,
3828, 6, 1426, - -32768, - -32768, - -32768, 619, 716, 741, 3894, - -32768, 747,
- -32768, 523, - -32768, 3894, 743, 755, - -32768, - -32768, - -32768, - -32768,
2541, - -32768, - -32768, 2541, - -32768, 2541, 2541, 529, 3894, 1027, - -32768,
- -32768, 564, 766, 341, 756, - -32768, 310, 1426, - -32768, - -32768, - -32768,
- -32768, - -32768, 2541, 325, 541, - -32768, - -32768, 758, - -32768, - -32768,
1192, - -32768, - -32768, 1027, - -32768, - -32768, 557, - -32768, 541, - -32768,
- -32768, - -32768, 857, 858, - -32768}; static short Cyc_yypgoto[ 112u]={ -
-32768, 38, - -32768, 550, - -32768, - -32768, - -32768, - -32768, - -32768, -
51, - 33, - 55, - -32768, 52, - 37, 289, - -32768, 90, - -32768, 29, - -32768, -
-32768, 424, - -32768, - 14, - -32768, - 84, - -32768, - -32768, - -32768, 659,
651, 51, - -32768, - -32768, 339, - -32768, - -32768, 14, - -32768, - -32768, 7,
- 32, - 122, - -32768, - -32768, - 49, - -32768, - 99, - 18, - 116, - 353, 136,
225, 314, 588, - 378, - 82, - 278, 759, - -32768, - 201, - 145, - 340, - 233, -
-32768, 428, - 143, - 78, - 102, 98, - 117, 130, - -32768, - -32768, - 40, - 227,
- -32768, - 564, - 364, - -32768, - -32768, - 21, 609, - -32768, 306, - -32768,
- -32768, 211, 126, - -32768, - 150, - 387, - 141, 571, 575, 545, 555, 569, 311,
149, 307, 312, 227, 582, - -32768, - -32768, - -32768, - 335, - -32768, - -32768,
10}; static short Cyc_yytable[ 4425u]={ 115, 258, 79, 371, 113, 144, 152, 303,
127, 123, 60, 407, 408, 306, 410, 116, 261, 307, 257, 120, 286, 471, 114, 519,
515, 121, 583, 317, 433, 296, 531, 532, 65, 80, 291, 81, 109, 426, 559, 290, 109,
87, 324, 446, 60, 115, 207, 208, 209, 113, 129, 60, 573, 100, 62, 133, 60, 240,
576, 115, 206, 239, 106, 251, 110, 548, 137, 60, 82, 292, 124, 565, 292, 293,
252, 757, 427, 584, 415, 91, 658, 80, 325, 31, 120, 697, 80, 256, 403, 486, 210,
61, 215, 1, 107, 434, 218, 362, 290, 101, 102, 103, 60, 60, 60, 60, 404, 586, 31,
363, 758, 83, 141, 416, 141, 292, 715, 141, 216, 659, - 116, 280, 253, 1, 364,
60, 31, 284, 92, 93, 198, 199, 200, 201, 141, 60, 285, 107, 269, 151, 80, 620,
430, 424, 388, 471, 141, 141, 94, 95, 115, 16, 17, 18, 251, 389, 214, 472, 295,
473, 399, 60, 286, 265, 400, 374, 332, 333, 151, 464, 485, 578, 296, 140, 264,
140, 30, 289, 140, 464, 444, 445, 32, 240, 240, 290, 240, 239, 239, 34, 239, 30,
282, 283, 776, 140, 84, 32, 104, 141, 454, 105, 384, 334, 335, 414, 30, 140, 140,
1, 253, 785, 32, 60, 470, 178, 197, 30, 85, 34, 790, 382, 31, 32, 60, 792, 378,
385, 524, 30, 37, 38, 39, 107, 88, 32, 109, 657, 417, 145, 287, 141, 37, 38, 39,
263, 146, 409, 62, 297, 60, 468, 413, 117, 481, 449, 89, 612, 118, 413, 140, 240,
119, 214, 121, 239, 62, 614, 671, - 181, 284, 465, 417, 298, 299, - 181, 466,
599, 641, 285, 467, 37, 38, 39, 31, 90, 80, 80, 340, 524, 279, 523, 30, 429, 700,
524, 341, 342, 32, 529, - 85, 87, 140, - 85, 60, 34, 60, 141, 85, 567, 597, 141,
141, 141, 598, 512, 37, 38, 39, 60, 516, 141, 513, 141, 152, 126, 412, 517, 141,
141, 373, 141, 413, 457, 458, 151, 85, 115, 256, 217, 442, 542, 141, 732, 423,
151, 413, 275, 446, 590, 151, 545, 615, 391, 115, 413, 616, 413, 113, 398, 736,
401, 402, 292, 591, 665, 589, 674, 140, 553, - 85, 530, 140, 140, 140, 85, 552,
60, 524, 60, 31, 318, 140, 665, 140, 624, 558, 635, 685, 140, 140, 625, 140, 413,
413, 688, 130, 664, 280, 320, 321, 413, 322, 140, 141, 521, 131, 522, 777, 413,
727, 463, 141, 125, 781, 664, 413, 788, 617, 355, 574, 734, 128, 791, 413, 577,
214, 468, 710, 784, 276, 373, 711, 277, 214, 141, 779, 280, 484, 397, 413, 132,
60, 330, 331, 489, 134, 269, 647, 336, 337, 80, 141, 356, 357, 358, 80, 151, 135,
667, 566, 136, 673, 668, 707, 140, 648, 649, 650, 1, 568, 338, 339, 140, 151,
488, 413, 150, 665, 642, 511, 500, 501, 502, 503, 489, 531, 532, 602, 373, 359,
60, 1, 202, 141, 141, 140, 115, 360, 361, 203, 251, 759, 600, 413, 604, 413, 626,
413, 219, 664, 318, 640, 140, 266, 646, 151, 628, 413, 479, 629, 413, 483, 270,
318, 696, 489, 274, 1, 679, 413, 775, 489, 549, 382, 551, 493, 60, 278, 554, 782,
290, 417, 302, 623, 308, 109, 115, 309, 141, 310, 113, 30, 311, 787, 140, 140,
789, 32, 312, 449, 96, 206, 691, 692, 34, 319, 508, 509, 510, 716, 413, 518, 718,
413, 30, 37, 38, 39, 742, 743, 32, 722, 413, - 207, 141, 141, 323, 294, - 207, -
207, 80, 327, 593, 326, - 207, 329, 285, 536, 365, 538, 539, 328, 543, 373, 749,
413, 151, 366, 675, 140, 30, 752, 413, 66, 754, 413, 32, 761, 368, - 178, 765,
299, 376, 294, - 178, - 178, 773, 413, 375, 740, - 178, 141, 285, 383, 37, 38,
39, 676, 677, 386, 498, 499, 504, 505, 387, 140, 140, 390, 392, 506, 507, 393,
394, 666, 395, 406, 405, 411, 420, 30, 68, 204, 69, 70, 425, 32, 432, 421, 437,
728, 443, 428, 71, 240, 438, 72, 73, 239, 439, 441, 444, 445, 440, 74, 455, 456,
324, 75, 76, 482, 487, 520, 140, 526, 527, 528, 537, 540, 544, 546, 80, 550, 240,
555, 557, 560, 239, 561, 563, 570, 569, 240, 575, 609, 571, 239, 580, 66, 31,
581, 582, 585, 587, 373, 588, 240, 595, 141, 240, 239, 594, 596, 239, 373, 601,
301, 672, 141, 304, 304, 141, 373, 603, 605, 606, 314, 315, 627, 607, 151, 610,
632, 634, 304, 304, 678, 304, 66, 681, 738, 611, 613, 151, 30, 68, 618, 69, 70,
621, 32, 622, 630, 636, 304, 637, 638, 71, 643, 645, 72, 73, 651, 655, 639, 444,
445, 656, 74, 140, 608, 373, 75, 76, 653, 660, 661, 662, 669, 140, 670, 680, 140,
694, 67, 68, 695, 69, 70, 256, 32, 702, 698, 717, 701, 703, 721, 71, 66, 704, 72,
73, 705, 708, 709, 731, 712, 713, 74, 714, 729, 730, 75, 76, 735, 746, 733, 373,
737, 745, 762, 682, 684, 741, 687, 764, 690, 767, 747, 748, 768, 778, 750, 751,
780, 753, 786, 794, 795, 480, 652, 572, 370, 377, 30, 68, 619, 69, 70, 760, 32,
699, 495, 706, 644, 579, 260, 71, 469, 769, 72, 73, 770, 496, 771, 772, 453, 654,
74, 0, 0, 0, 75, 76, 0, 492, 720, 0, 497, 724, 726, 494, 0, 783, 0, 0, 304, 0,
304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 304,
304, 0, 154, 155, 0, 0, 0, 0, 0, 474, 0, 0, 0, 0, 756, 156, 0, 157, 0, 0, 158, 0,
0, 763, 0, 0, 0, 159, 160, 766, 0, 161, 0, 0, 162, 163, 304, 0, 164, 165, 0, 0,
0, 0, 774, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 0, 0, 0, 0, 0, 0,
30, 166, 167, 168, 169, 0, 32, 0, 0, 353, 305, 475, 0, 170, 0, 0, 0, 171, 0, 0,
0, 444, 445, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 304, 0, 0, 0, 0,
0, 304, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 691, 692, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226, 227, 228, 22,
156, 23, 157, 229, 304, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231, 232, 161, 29,
233, 162, 163, 0, 0, 164, 165, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 0, 0, 0, 0, 304, 234, 166, 167, 168, 169, 31, 32, 33, 22, 235, 112, 0,
0, 170, 0, 0, 35, 237, 0, 27, 28, 0, 0, 0, 172, 0, 29, 173, 174, 175, 0, 0, 0, 0,
176, 177, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 0,
0, 31, 0, 33, 0, 256, 0, 0, 0, 284, 0, 0, 35, 36, 0, 0, 304, 0, 285, 0, 37, 38,
39, 0, 304, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
742, 743, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226, 227, 228, 22, 156,
23, 157, 229, 0, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231, 232, 161, 29, 233,
162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 234, 166, 167, 168, 169, 31, 32, 33, 0, 235, 112, 0, 0, 170, 0, 0, 35,
237, 0, 0, 0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 2, 3, 4,
111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155,
220, 0, 221, 222, 223, 224, 225, 226, 227, 228, 22, 156, 23, 157, 229, 0, 158,
24, 230, 0, 0, 27, 28, 159, 160, 231, 232, 161, 29, 233, 162, 163, 0, 0, 164,
165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166,
167, 168, 169, 31, 32, 33, 0, 235, 112, 236, 0, 170, 0, 0, 35, 237, 0, 0, 0, 0,
0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 2, 3, 4, 111, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 220, 0, 221, 222,
223, 224, 225, 226, 227, 228, 22, 156, 23, 157, 229, 0, 158, 24, 230, 0, 0, 27,
28, 159, 160, 231, 232, 161, 29, 233, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166, 167, 168, 169, 31, 32, 33,
0, 235, 112, 0, 0, 170, 0, 0, 35, 237, 0, 0, 0, 0, 0, 0, 172, 0, 0, 173, 174,
175, 0, 0, 0, 0, 176, 177, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 21, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 156, 23,
157, 0, 0, 158, 24, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 31, 32, 33, 0, 541, 0, 0, 0, 170, 0, 0, 35, 237, 0, 0, 0, 0, 0, 0,
172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 156, 0,
157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 31, 32, 33, 0, 0, 316, 0, 0, 170, 0, 0, 35, 237, 0, 0, 0, 0, 0, 0,
172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 156, 0,
157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 30, 166, 167, 168, 169, 31, 32, 33, 0, 0, 0, 0, 0, 170, 22, 0, 35, 237, 0, 0,
0, 0, 0, 0, 172, 27, 28, 173, 174, 175, 0, 0, 29, 0, 176, 177, - 8, 1, 0, 2, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 254, 0, 21, 255, 0,
0, 0, 0, 31, 0, 33, 0, 256, 0, 0, 22, 0, 23, 0, 35, 36, 0, 24, 0, 25, 26, 27, 28,
0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 0, 0, 35,
36, 0, 0, 0, 0, 0, 0, 37, 38, 39, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30,
0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0,
37, 38, 39, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 0, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 22, 0, 23, 35, 36, 0, 0, 24,
0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, -
8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4, 111, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 1, 0, 0, 0, 29, 0,
0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0,
0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 254, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 256, 0, 0, 0, 294, - 159, 0, 35, 36,
0, 0, - 159, 0, 285, 0, 37, 38, 39, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23,
0, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 2, 3, 4, 111,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0,
0, 31, 0, 33, 0, 0, 112, 22, 204, 23, 0, 0, 35, 36, 24, 0, 0, 0, 27, 28, 0, 0, 0,
0, 0, 29, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0, 112, 0, 0, 0, 0, 24,
35, 36, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226, 227, 228,
31, 156, 33, 157, 229, 0, 158, 0, 230, 0, 0, 35, 36, 159, 160, 231, 232, 161, 0,
233, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 234, 166, 167, 168, 169, 0, 32, 154, 155, 235, 112, 0, 0, 170, 0, 0,
0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176,
177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 0, 0, 0, 305, 663, 0, 170,
0, 0, 0, 171, 154, 155, 0, 444, 445, 0, 172, 0, 0, 173, 174, 175, 0, 156, 0, 157,
176, 177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 0, 164,
165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167,
168, 169, 0, 32, 0, 0, 0, 305, 739, 0, 170, 0, 0, 0, 171, 154, 155, 0, 444, 445,
0, 172, 0, 0, 173, 174, 175, 0, 156, 0, 157, 176, 177, 158, 0, 0, 0, 0, 0, 0,
159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 0, 0, 0, 305,
0, 0, 170, 0, 0, 0, 171, 154, 155, 0, 444, 445, 0, 172, 0, 0, 173, 174, 175, 0,
156, 0, 157, 176, 177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0,
156, 0, 157, 262, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0,
0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 305, 0, 0, 170, 0, 0, 0,
171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 396, 0, 0, 0, 170, 0,
0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176,
177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170,
0, 0, 0, 171, 0, 0, 156, 0, 157, 462, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160,
176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0,
0, 170, 514, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0,
159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155,
0, 0, 0, 0, 170, 547, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174,
175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32,
154, 155, 0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 592, 172, 158, 0,
173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 154, 155, 631, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172,
158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164,
165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167,
168, 169, 0, 32, 154, 155, 633, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 0,
172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 683, 0, 0, 171, 0, 0, 156, 0,
157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 686, 0, 0, 171, 0,
0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0,
0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 689, 0, 0, 0, 170, 0, 0, 0,
171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 719,
0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176,
177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170,
723, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160,
176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0,
0, 170, 725, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0,
159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155,
0, 0, 0, 0, 170, 755, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174,
175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 154,
155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0,
32, 159, 160, 0, 0, 161, 0, 170, 0, 163, 0, 171, 164, 165, 0, 0, 0, 0, 172, 0, 0,
173, 174, 175, 0, 0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0,
32, 154, 155, 0, 305, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 0, 0, 172, 0, 0,
173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 0, 163, 0, 0, 164, 165, 0, 0,
0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 159, 160, 0, 608, 161, 0, 170, 0, 163, 0, 171, 164, 165, 0, 0, 0, 0,
172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 154, 155, 0, 0, 0, 0, 300, 0, 0, 0, 171, 0, 0, 156, 0, 0, 0, 172, 0,
0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 0, 163, 0, 0, 164, 165, 0,
0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 30, 166, 167,
168, 169, 0, 32, 159, 160, 0, 0, 161, 0, 313, 0, 163, 0, 171, 164, 165, 0, 0, 0,
0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 166, 167,
168, 169, 0, 32, 0, 0, 0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 0, 0, 0, 0, 172, 0,
0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 22, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 22, 0, 0, 0, 29, 0, 0, 0, 138, 0, 0, 27, 28, 0,
0, 0, 0, 0, 29, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0,
0, 31, 0, 33, 0, 0, 139, 254, 0, 0, 0, 22, 35, 36, 0, 31, 0, 33, 0, 256, 0, 0,
27, 28, 0, 0, 35, 36, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 564, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 0, 0,
0, 35, 36}; static short Cyc_yycheck[ 4425u]={ 55, 117, 23, 204, 55, 83, 88, 157,
63, 58, 0, 238, 239, 158, 241, 55, 118, 158, 117, 56, 142, 299, 55, 363, 359, 57,
98, 170, 17, 151, 5, 6, 22, 23, 64, 25, 54, 64, 416, 111, 58, 34, 70, 276, 34,
100, 101, 102, 103, 100, 71, 41, 439, 46, 92, 76, 46, 112, 445, 114, 100, 112,
52, 114, 54, 400, 80, 57, 84, 99, 60, 424, 99, 103, 114, 69, 103, 455, 59, 41,
64, 71, 110, 89, 121, 98, 76, 93, 84, 316, 104, 86, 106, 1, 100, 84, 110, 82,
111, 47, 48, 49, 92, 93, 94, 95, 102, 460, 89, 94, 104, 102, 83, 94, 85, 99, 680,
88, 108, 103, 95, 139, 115, 1, 109, 115, 89, 97, 93, 94, 92, 93, 94, 95, 105,
125, 106, 100, 128, 88, 130, 528, 264, 259, 86, 423, 117, 118, 93, 94, 205, 17,
18, 19, 205, 97, 105, 300, 151, 302, 93, 151, 284, 125, 97, 205, 65, 66, 117,
286, 313, 96, 294, 83, 122, 85, 84, 103, 88, 296, 105, 106, 90, 238, 239, 111,
241, 238, 239, 97, 241, 84, 140, 141, 758, 105, 83, 90, 91, 170, 278, 94, 216,
102, 103, 254, 84, 117, 118, 1, 203, 775, 90, 203, 292, 89, 90, 84, 102, 97, 784,
214, 89, 90, 214, 789, 104, 217, 378, 84, 108, 109, 110, 100, 97, 90, 254, 590,
256, 84, 142, 212, 108, 109, 110, 119, 91, 240, 92, 151, 240, 288, 99, 97, 309,
276, 94, 104, 102, 99, 170, 316, 106, 212, 296, 316, 92, 107, 608, 93, 97, 97,
290, 98, 99, 99, 102, 478, 556, 106, 106, 108, 109, 110, 89, 94, 276, 277, 108,
439, 95, 375, 84, 264, 647, 445, 116, 117, 90, 383, 94, 294, 212, 97, 294, 97,
296, 278, 102, 431, 95, 282, 283, 284, 99, 84, 108, 109, 110, 309, 84, 292, 91,
294, 406, 97, 93, 91, 299, 300, 204, 302, 99, 282, 283, 284, 102, 392, 93, 105,
93, 392, 313, 696, 99, 294, 99, 136, 581, 465, 299, 93, 95, 223, 409, 99, 99, 99,
409, 229, 104, 231, 232, 99, 466, 598, 465, 103, 278, 409, 97, 385, 282, 283,
284, 102, 409, 367, 528, 369, 89, 170, 292, 616, 294, 93, 95, 93, 93, 299, 300,
99, 302, 99, 99, 93, 97, 598, 416, 172, 173, 99, 175, 313, 375, 367, 84, 369,
761, 99, 93, 285, 383, 94, 104, 616, 99, 781, 520, 192, 441, 699, 94, 787, 99,
446, 375, 464, 95, 104, 94, 305, 99, 97, 383, 406, 95, 455, 312, 228, 99, 85,
432, 67, 68, 319, 93, 437, 564, 63, 64, 441, 423, 60, 61, 62, 446, 406, 99, 604,
431, 96, 612, 604, 665, 375, 84, 85, 86, 1, 432, 111, 112, 383, 423, 98, 99, 98,
711, 557, 354, 332, 333, 334, 335, 359, 5, 6, 481, 363, 97, 481, 1, 93, 465, 466,
406, 552, 105, 106, 99, 552, 729, 98, 99, 98, 99, 98, 99, 86, 711, 300, 552, 423,
97, 564, 465, 98, 99, 308, 98, 99, 311, 98, 313, 641, 400, 84, 1, 98, 99, 758,
406, 403, 527, 405, 325, 527, 97, 413, 767, 111, 560, 97, 534, 97, 564, 602, 97,
520, 97, 602, 84, 97, 781, 465, 466, 784, 90, 97, 581, 93, 602, 22, 23, 97, 97,
340, 341, 342, 98, 99, 361, 98, 99, 84, 108, 109, 110, 22, 23, 90, 98, 99, 93,
556, 557, 95, 97, 98, 99, 581, 114, 467, 69, 104, 113, 106, 387, 95, 389, 390,
115, 392, 478, 98, 99, 556, 95, 623, 520, 84, 98, 99, 38, 98, 99, 90, 734, 95,
93, 98, 99, 95, 97, 98, 99, 98, 99, 94, 712, 104, 603, 106, 94, 108, 109, 110,
624, 625, 94, 330, 331, 336, 337, 97, 556, 557, 97, 97, 338, 339, 84, 93, 603,
93, 97, 104, 95, 98, 84, 85, 96, 87, 88, 98, 90, 95, 104, 99, 691, 95, 107, 97,
729, 95, 100, 101, 729, 96, 99, 105, 106, 98, 108, 99, 95, 70, 112, 113, 25, 98,
102, 603, 93, 99, 104, 97, 30, 93, 42, 691, 89, 758, 84, 97, 111, 758, 89, 98,
98, 97, 767, 84, 487, 99, 767, 95, 38, 89, 99, 98, 98, 98, 598, 107, 781, 99,
699, 784, 781, 98, 84, 784, 608, 98, 154, 611, 709, 157, 158, 712, 616, 97, 99,
98, 164, 165, 537, 95, 699, 98, 541, 542, 172, 173, 626, 175, 38, 629, 709, 99,
98, 712, 84, 85, 95, 87, 88, 95, 90, 95, 97, 94, 192, 93, 98, 97, 95, 102, 100,
101, 98, 98, 103, 105, 106, 98, 108, 699, 94, 665, 112, 113, 107, 107, 84, 102,
25, 709, 98, 94, 712, 93, 84, 85, 84, 87, 88, 93, 90, 98, 89, 683, 99, 98, 686,
97, 38, 98, 100, 101, 98, 98, 97, 695, 97, 28, 108, 94, 104, 95, 112, 113, 85,
93, 99, 711, 98, 95, 99, 630, 631, 713, 633, 98, 635, 104, 718, 719, 95, 85, 722,
723, 98, 725, 98, 0, 0, 309, 571, 437, 203, 212, 84, 85, 527, 87, 88, 733, 90,
646, 327, 662, 560, 447, 117, 97, 290, 749, 100, 101, 752, 328, 754, 755, 277,
581, 108, - 1, - 1, - 1, 112, 113, - 1, 324, 685, - 1, 329, 688, 689, 326, - 1,
773, - 1, - 1, 324, - 1, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336,
337, 338, 339, 340, 341, 342, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, 32, - 1, - 1,
- 1, - 1, 727, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, 736, - 1, - 1, - 1, 50, 51,
742, - 1, 54, - 1, - 1, 57, 58, 378, - 1, 61, 62, - 1, - 1, - 1, - 1, 757, 71,
72, 73, 74, 75, 76, 77, 78, 79, 80, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, - 1, 90, - 1, - 1, 96, 94, 95, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, - 1,
105, 106, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 439,
- 1, - 1, - 1, - 1, - 1, 445, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34,
35, 36, 37, 38, 39, 40, 41, 487, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53,
54, 55, 56, 57, 58, - 1, - 1, 61, 62, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, 528, 84, 85, 86, 87, 88, 89, 90, 91,
37, 93, 94, - 1, - 1, 97, - 1, - 1, 100, 101, - 1, 48, 49, - 1, - 1, - 1, 108, -
1, 55, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 89, - 1, 91, - 1, 93, - 1, - 1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1,
604, - 1, 106, - 1, 108, 109, 110, - 1, 612, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33,
34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52,
53, 54, 55, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, 89, 90, 91, - 1, 93, 94, - 1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, -
1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26,
27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, -
1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, 93, 94, 95, - 1, 97, - 1, -
1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, -
1, - 1, - 1, 118, 119, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37,
38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91,
- 1, 93, 94, - 1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108,
- 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, 39, 40, - 1, - 1, 43, 44, - 1, - 1, -
1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, 93, - 1, - 1, - 1, 97, - 1, -
1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, -
1, - 1, - 1, 118, 119, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1,
- 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1,
40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57,
58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1,
- 1, 94, - 1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, -
1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49,
50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1,
- 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 84, 85, 86, 87,
88, 89, 90, 91, - 1, - 1, - 1, - 1, - 1, 97, 37, - 1, 100, 101, - 1, - 1, - 1, -
1, - 1, - 1, 108, 48, 49, 111, 112, 113, - 1, - 1, 55, - 1, 118, 119, 0, 1, - 1,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 81, - 1, 24,
84, - 1, - 1, - 1, - 1, 89, - 1, 91, - 1, 93, - 1, - 1, 37, - 1, 39, - 1, 100,
101, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, - 1, - 1, - 1, - 1, 89, 90, 91, -
1, - 1, - 1, 95, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108,
109, 110, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, -
1, 55, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, - 1, - 1, 37, - 1, 89, 90,
91, - 1, - 1, - 1, - 1, - 1, 97, 48, 49, 100, 101, - 1, - 1, - 1, 55, - 1, - 1,
108, 109, 110, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, 89, - 1, 91, - 1, - 1, - 1, - 1, -
1, 37, - 1, 39, 100, 101, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1,
- 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, - 1, - 1,
- 1, - 1, 89, 90, 91, - 1, - 1, - 1, 95, - 1, 97, - 1, - 1, 100, 101, - 1, - 1,
- 1, - 1, - 1, - 1, 108, 109, 110, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48,
49, - 1, 1, - 1, - 1, - 1, 55, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
- 1, - 1, 37, - 1, 89, 90, 91, - 1, - 1, - 1, - 1, - 1, 97, 48, 49, 100, 101, -
1, - 1, - 1, 55, - 1, - 1, 108, 109, 110, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, - 1, - 1, 84, -
1, - 1, - 1, - 1, 89, 90, 91, - 1, 93, - 1, - 1, - 1, 97, 98, - 1, 100, 101, - 1,
- 1, 104, - 1, 106, - 1, 108, 109, 110, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49,
- 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, -
1, - 1, 89, - 1, 91, - 1, - 1, 94, 37, 96, 39, - 1, - 1, 100, 101, 44, - 1, - 1,
- 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 89, - 1, 91, 37, - 1, 94, - 1, - 1, - 1, - 1, 44, 100, 101, -
1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 25, 26, 27, - 1,
29, 30, 31, 32, 33, 34, 35, 36, 89, 38, 91, 40, 41, - 1, 43, - 1, 45, - 1, - 1,
100, 101, 50, 51, 52, 53, 54, - 1, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93, 94, - 1, - 1, 97, - 1, - 1, -
1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50,
51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, - 1, - 1, - 1, 94, 95, - 1, 97, - 1, - 1, - 1, 101, 25,
26, - 1, 105, 106, - 1, 108, - 1, - 1, 111, 112, 113, - 1, 38, - 1, 40, 118, 119,
43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, -
1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, - 1, - 1, - 1,
94, 95, - 1, 97, - 1, - 1, - 1, 101, 25, 26, - 1, 105, 106, - 1, 108, - 1, - 1,
111, 112, 113, - 1, 38, - 1, 40, 118, 119, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50,
51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, - 1, - 1, - 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, 25,
26, - 1, 105, 106, - 1, 108, - 1, - 1, 111, 112, 113, - 1, 38, - 1, 40, 118, 119,
43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, -
1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1,
- 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, 107, 108, 43, - 1, 111,
112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, 94, - 1, - 1,
97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113,
- 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93, - 1, - 1, - 1, 97, - 1, -
1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1,
50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1,
101, - 1, - 1, 38, - 1, 40, 107, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51,
118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1,
- 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119,
54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, -
1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, -
1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90,
25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, 107,
108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, -
1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93,
- 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1,
111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61,
62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93, - 1, - 1, -
1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112,
113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97,
98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1,
- 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, -
1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50,
51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, 25, 26, 93, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1,
- 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119,
54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, -
1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, -
1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90,
25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1,
108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, -
1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1,
- 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1,
111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61,
62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1,
- 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112,
113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, -
1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1,
- 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 50, 51, - 1, - 1, 54, - 1, 97, - 1,
58, - 1, 101, 61, 62, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1,
- 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26,
- 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, - 1, - 1, 108, - 1,
- 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, - 1, 58, - 1, - 1,
61, 62, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 38, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 50, 51, - 1, 94, 54, -
1, 97, - 1, 58, - 1, 101, 61, 62, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112,
113, - 1, - 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88,
- 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1,
- 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1,
- 1, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 50,
51, - 1, - 1, 54, - 1, 97, - 1, 58, - 1, 101, 61, 62, - 1, - 1, - 1, - 1, 108, -
1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, - 1, - 1, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, -
1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1,
118, 119, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 48, 49, - 1, 37, - 1, - 1, - 1, 55, - 1, - 1, - 1, 59, - 1, - 1,
48, 49, - 1, - 1, - 1, - 1, - 1, 55, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, - 1, - 1, - 1, 89, - 1, 91, - 1, - 1, 94, 81, - 1, - 1, -
1, 37, 100, 101, - 1, 89, - 1, 91, - 1, 93, - 1, - 1, 48, 49, - 1, - 1, 100, 101,
- 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 100, 101};
unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short*
yyss=( short*)({ unsigned int _temp7376= 10000u; short* _temp7377=( short*)
GC_malloc_atomic( sizeof( short) *  _temp7376);{ unsigned int _temp7378=
_temp7376; unsigned int i; for( i= 0; i <  _temp7378; i ++){ _temp7377[ i]=(
short) 0;}}; _temp7377;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp7373= 10000u; void** _temp7374=( void**) GC_malloc( sizeof(
void*) *  _temp7373);{ unsigned int _temp7375= _temp7373; unsigned int i; for( i=
0; i <  _temp7375; i ++){ _temp7374[ i]= Cyc_yylval;}}; _temp7374;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp7370= 10000u; struct Cyc_Yyltype* _temp7371=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) *  _temp7370);{ unsigned int
_temp7372= _temp7370; unsigned int i; for( i= 0; i <  _temp7372; i ++){
_temp7371[ i]= Cyc_yynewloc();}}; _temp7371;}); int yystacksize= 10000; void*
yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar=
- 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate: yyss[
_check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate; if(
yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 796u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
347: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 348u, Cyc_yychar)]:
232;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4424)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4425u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4425u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  795){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 796u, yystate)]; if(
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
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
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
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1007[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1008; _temp1008.tag= Cyc_DeclList_tok; _temp1008.f1=({ struct Cyc_List_List*
_temp1009=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1009->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1010=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1010->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp1011=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1011[ 0]=({ struct
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
_temp1030=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1030[ 0]=({ struct Cyc_DeclList_tok_struct _temp1031; _temp1031.tag= Cyc_DeclList_tok;
_temp1031.f1=({ struct Cyc_List_List* _temp1032=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1032->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1041=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1041->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1042=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
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
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1061[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1062; _temp1062.tag= Cyc_DeclList_tok; _temp1062.f1=({ struct Cyc_List_List*
_temp1063=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1063->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1064=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1064->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp1065=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1065[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1066; _temp1066.tag= Cyc_Absyn_Namespace_d;
_temp1066.f1=({ struct _tagged_arr* _temp1067=( struct _tagged_arr*) GC_malloc(
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
_temp1085=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1085[ 0]=({ struct Cyc_DeclList_tok_struct _temp1086; _temp1086.tag= Cyc_DeclList_tok;
_temp1086.f1=({ struct Cyc_List_List* _temp1087=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1087->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1096=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1096->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1097=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1097[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1098; _temp1098.tag=
Cyc_Absyn_Namespace_d; _temp1098.f1=({ struct _tagged_arr* _temp1099=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1099[ 0]=({ struct
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
Cyc_std_strcmp(({ struct _tagged_arr yyzzz;{ void* _temp1117= yyvs[
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
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1125[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1126; _temp1126.tag= Cyc_DeclList_tok; _temp1126.f1=({ struct Cyc_List_List*
_temp1127=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1127->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1136=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1136->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1137=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1137[ 0]=({ struct
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
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1148[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1149; _temp1149.tag= Cyc_DeclList_tok; _temp1149.f1= 0; _temp1149;});
_temp1148;}); break; case 9: _LL1147: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1151=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1151[ 0]=({ struct Cyc_DeclList_tok_struct _temp1152; _temp1152.tag= Cyc_DeclList_tok;
_temp1152.f1=({ struct Cyc_List_List* _temp1153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1153->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp1154=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1154[ 0]=({ struct Cyc_Absyn_Fn_d_struct
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
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1166[ 0]=({ struct Cyc_FnDecl_tok_struct
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
_temp1185=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1185[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1186; _temp1186.tag= Cyc_FnDecl_tok;
_temp1186.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1187=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1187->v=( void*)({
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
_temp1213=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
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
_temp1240=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1240[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1241; _temp1241.tag= Cyc_FnDecl_tok;
_temp1241.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1242=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1242->v=( void*)({
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
_temp1276=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1276[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1277; _temp1277.tag= Cyc_FnDecl_tok;
_temp1277.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1278=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1278->v=( void*)({
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
_temp1304=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1304[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1305; _temp1305.tag= Cyc_FnDecl_tok;
_temp1305.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1306=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1306->v=( void*)({
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
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1350[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp1351= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tagged_arr _temp1357; _LL1353: if(*(( void**) _temp1351)
==  Cyc_String_tok){ _LL1358: _temp1357=(( struct Cyc_String_tok_struct*)
_temp1351)->f1; goto _LL1354;} else{ goto _LL1355;} _LL1355: goto _LL1356;
_LL1354: yyzzz= _temp1357; goto _LL1352; _LL1356:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL1352:;} yyzzz;}); _temp1350;})); yyval= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; break; case 20: _LL1349: Cyc_Lex_leave_namespace();
break; case 21: _LL1359: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1361=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
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
_temp1372=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
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
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1391[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1392; _temp1392.tag= Cyc_DeclList_tok; _temp1392.f1=({ struct Cyc_List_List*
_temp1393=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
struct _tuple1* _temp1417=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1417->f1= Cyc_Absyn_rel_ns_null; _temp1417->f2= _temp1413; _temp1417;});
struct Cyc_Absyn_Vardecl* _temp1415= Cyc_Absyn_new_vardecl( _temp1414, Cyc_Absyn_wildtyp(
0), 0); _temp1411=({ struct Cyc_List_List* _temp1416=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1416->hd=( void*) _temp1415;
_temp1416->tl= _temp1411; _temp1416;});}} _temp1411=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1411); yyval=( void*)({ struct
Cyc_DeclList_tok_struct* _temp1426=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct)); _temp1426[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1427; _temp1427.tag= Cyc_DeclList_tok; _temp1427.f1=({ struct Cyc_List_List*
_temp1428=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1428->hd=( void*) Cyc_Absyn_letv_decl( _temp1411, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1428->tl= 0; _temp1428;}); _temp1427;}); _temp1426;}); break;} case 25:
_LL1410: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 26: _LL1429: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1431=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
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
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1450[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1451; _temp1451.tag= Cyc_DeclSpec_tok; _temp1451.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1452=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1452->sc=({ struct Cyc_Core_Opt* _temp1461=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1461->v=( void*)({ enum 
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
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1479[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1480; _temp1480.tag= Cyc_DeclSpec_tok; _temp1480.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1481=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1481->sc=({ struct Cyc_Core_Opt* _temp1522=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1522->v=( void*)({ enum 
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
_temp1532=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1532[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1533; _temp1533.tag= Cyc_DeclSpec_tok;
_temp1533.f1=({ struct Cyc_Parse_Declaration_spec* _temp1534=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1534->sc= 0;
_temp1534->tq= Cyc_Absyn_empty_tqual(); _temp1534->type_specs=({ struct Cyc_List_List*
_temp1543=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1553[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1554; _temp1554.tag= Cyc_DeclSpec_tok; _temp1554.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1555=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
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
_temp1580=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
_temp1614=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1614[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1615; _temp1615.tag= Cyc_DeclSpec_tok;
_temp1615.f1=({ struct Cyc_Parse_Declaration_spec* _temp1616=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1616->sc= 0;
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
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1634[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1635; _temp1635.tag= Cyc_DeclSpec_tok; _temp1635.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1636=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
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
_temp1694=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1694[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1695; _temp1695.tag= Cyc_DeclSpec_tok;
_temp1695.f1=({ struct Cyc_Parse_Declaration_spec* _temp1696=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1696->sc= 0;
_temp1696->tq= Cyc_Absyn_empty_tqual(); _temp1696->type_specs= 0; _temp1696->is_inline=
1; _temp1696->attributes=({ struct Cyc_List_List* yyzzz;{ void* _temp1697= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp1703; _LL1699: if(*(( void**) _temp1697) ==  Cyc_AttributeList_tok){
_LL1704: _temp1703=(( struct Cyc_AttributeList_tok_struct*) _temp1697)->f1; goto
_LL1700;} else{ goto _LL1701;} _LL1701: goto _LL1702; _LL1700: yyzzz= _temp1703;
goto _LL1698; _LL1702:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1698:;} yyzzz;}); _temp1696;}); _temp1695;}); _temp1694;}); break; case 34:
_LL1693: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1706=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1706[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1707; _temp1707.tag= Cyc_DeclSpec_tok; _temp1707.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1708=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
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
_temp1750=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1750[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1751; _temp1751.tag= Cyc_StorageClass_tok; _temp1751.f1= Cyc_Parse_Auto_sc;
_temp1751;}); _temp1750;}); break; case 36: _LL1749: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1753=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1753[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1754; _temp1754.tag= Cyc_StorageClass_tok; _temp1754.f1= Cyc_Parse_Register_sc;
_temp1754;}); _temp1753;}); break; case 37: _LL1752: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1756=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1756[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1757; _temp1757.tag= Cyc_StorageClass_tok; _temp1757.f1= Cyc_Parse_Static_sc;
_temp1757;}); _temp1756;}); break; case 38: _LL1755: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1759=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1759[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1760; _temp1760.tag= Cyc_StorageClass_tok; _temp1760.f1= Cyc_Parse_Extern_sc;
_temp1760;}); _temp1759;}); break; case 39: _LL1758: if( Cyc_std_strcmp(({
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
_temp1770=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1770[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1771; _temp1771.tag= Cyc_StorageClass_tok; _temp1771.f1= Cyc_Parse_ExternC_sc;
_temp1771;}); _temp1770;}); break; case 40: _LL1761: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1773=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1773[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1774; _temp1774.tag= Cyc_StorageClass_tok; _temp1774.f1= Cyc_Parse_Typedef_sc;
_temp1774;}); _temp1773;}); break; case 41: _LL1772: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1776=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1776[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1777; _temp1777.tag= Cyc_StorageClass_tok; _temp1777.f1= Cyc_Parse_Abstract_sc;
_temp1777;}); _temp1776;}); break; case 42: _LL1775: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1779=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1779[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1780; _temp1780.tag=
Cyc_AttributeList_tok; _temp1780.f1= 0; _temp1780;}); _temp1779;}); break; case
43: _LL1778: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1781: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1783=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1783[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1784; _temp1784.tag=
Cyc_AttributeList_tok; _temp1784.f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp1785= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp1791; _LL1787: if(*(( void**) _temp1785) ==  Cyc_AttributeList_tok){
_LL1792: _temp1791=(( struct Cyc_AttributeList_tok_struct*) _temp1785)->f1; goto
_LL1788;} else{ goto _LL1789;} _LL1789: goto _LL1790; _LL1788: yyzzz= _temp1791;
goto _LL1786; _LL1790:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1786:;} yyzzz;}); _temp1784;}); _temp1783;}); break; case 45: _LL1782: yyval=(
void*)({ struct Cyc_AttributeList_tok_struct* _temp1794=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp1794[ 0]=({
struct Cyc_AttributeList_tok_struct _temp1795; _temp1795.tag= Cyc_AttributeList_tok;
_temp1795.f1=({ struct Cyc_List_List* _temp1796=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1796->hd=( void*)({ void* yyzzz;{
void* _temp1797= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp1803; _LL1799: if(*(( void**) _temp1797) ==  Cyc_Attribute_tok){
_LL1804: _temp1803=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1797)->f1;
goto _LL1800;} else{ goto _LL1801;} _LL1801: goto _LL1802; _LL1800: yyzzz=
_temp1803; goto _LL1798; _LL1802:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1798:;} yyzzz;}); _temp1796->tl= 0; _temp1796;}); _temp1795;}); _temp1794;});
break; case 46: _LL1793: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1806=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1806[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1807; _temp1807.tag=
Cyc_AttributeList_tok; _temp1807.f1=({ struct Cyc_List_List* _temp1808=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1808->hd=( void*)({
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
_temp1826=( struct _tagged_arr) Cyc_std_substring( _temp1826, 2, _get_arr_size(
_temp1826, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0; i <  16u; ++ i){
if( Cyc_std_strcmp( _temp1826,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1827=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1827[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1828; _temp1828.tag= Cyc_Attribute_tok; _temp1828.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1828;}); _temp1827;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1829=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1829[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1830; _temp1830.tag= Cyc_Attribute_tok; _temp1830.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1830;}); _temp1829;});} break;}} case
48: _LL1825: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1840=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
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
void* a; if( Cyc_std_zstrcmp( _temp1843, _tag_arr("regparm", sizeof(
unsigned char), 8u)) ==  0? 1: Cyc_std_zstrcmp( _temp1843, _tag_arr("__regparm__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1855 <=  0? 1: _temp1855 >  3){
Cyc_Parse_err( _tag_arr("regparm requires value between 1 and 3", sizeof(
unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp1857=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp1857[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp1858; _temp1858.tag= Cyc_Absyn_Regparm_att;
_temp1858.f1= _temp1855; _temp1858;}); _temp1857;});} else{ if( Cyc_std_zstrcmp(
_temp1843, _tag_arr("aligned", sizeof( unsigned char), 8u)) ==  0? 1: Cyc_std_zstrcmp(
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
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1859[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1860; _temp1860.tag= Cyc_Absyn_Aligned_att;
_temp1860.f1= _temp1855; _temp1860;}); _temp1859;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1861=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
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
void* a; if( Cyc_std_zstrcmp( _temp1872, _tag_arr("section", sizeof(
unsigned char), 8u)) ==  0? 1: Cyc_std_zstrcmp( _temp1872, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1874=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1874[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1875; _temp1875.tag=
Cyc_Absyn_Section_att; _temp1875.f1= _temp1873; _temp1875;}); _temp1874;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1876=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1876[ 0]=({ struct
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
if( Cyc_std_zstrcmp( _temp1895, _tag_arr("format", sizeof( unsigned char), 7u))
==  0? 1: Cyc_std_zstrcmp( _temp1895, _tag_arr("__format__", sizeof(
unsigned char), 11u)) ==  0){ if( Cyc_std_zstrcmp( _temp1896, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1921=( struct Cyc_Absyn_Format_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1921[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1922; _temp1922.tag=
Cyc_Absyn_Format_att; _temp1922.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1922.f2= _temp1907; _temp1922.f3= _temp1919; _temp1922;}); _temp1921;});}
else{ if( Cyc_std_zstrcmp( _temp1896, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1923=( struct
Cyc_Absyn_Format_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct));
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
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1925[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1926; _temp1926.tag= Cyc_Attribute_tok; _temp1926.f1=(
void*) a; _temp1926;}); _temp1925;}); break;}}} case 52: _LL1894: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1944=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1944[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1945; _temp1945.tag= Cyc_TypeSpecifier_tok;
_temp1945.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1945;});
_temp1944;}); break; case 53: _LL1943: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1947=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1947[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1948; _temp1948.tag=
Cyc_TypeSpecifier_tok; _temp1948.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1948;}); _temp1947;}); break; case 54: _LL1946:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1950=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1950[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1951; _temp1951.tag= Cyc_TypeSpecifier_tok;
_temp1951.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1951;});
_temp1950;}); break; case 55: _LL1949: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1953=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1953[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1954; _temp1954.tag=
Cyc_TypeSpecifier_tok; _temp1954.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1955=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1955[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1956; _temp1956.tag=
Cyc_Parse_Short_spec; _temp1956.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1956;});
_temp1955;})); _temp1954;}); _temp1953;}); break; case 56: _LL1952: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1958=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1958[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1959; _temp1959.tag= Cyc_TypeSpecifier_tok;
_temp1959.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1959;});
_temp1958;}); break; case 57: _LL1957: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1961=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1961[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1962; _temp1962.tag=
Cyc_TypeSpecifier_tok; _temp1962.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1963=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1963[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1964; _temp1964.tag= Cyc_Parse_Long_spec;
_temp1964.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1964;}); _temp1963;})); _temp1962;}); _temp1961;});
break; case 58: _LL1960: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1966=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1966[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1967; _temp1967.tag=
Cyc_TypeSpecifier_tok; _temp1967.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1967;}); _temp1966;}); break; case 59: _LL1965:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1969=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1969[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1970; _temp1970.tag= Cyc_TypeSpecifier_tok;
_temp1970.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1970;});
_temp1969;}); break; case 60: _LL1968: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1972=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1972[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1973; _temp1973.tag=
Cyc_TypeSpecifier_tok; _temp1973.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1974=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1974[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1975; _temp1975.tag=
Cyc_Parse_Signed_spec; _temp1975.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1975;});
_temp1974;})); _temp1973;}); _temp1972;}); break; case 61: _LL1971: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1977=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1977[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1978; _temp1978.tag= Cyc_TypeSpecifier_tok;
_temp1978.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1979=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1979[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1980; _temp1980.tag= Cyc_Parse_Unsigned_spec; _temp1980.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1980;});
_temp1979;})); _temp1978;}); _temp1977;}); break; case 62: _LL1976: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1981:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1982: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1983: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1985=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
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
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1996[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1997; _temp1997.tag= Cyc_TypeSpecifier_tok;
_temp1997.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp1998=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
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
_temp2017=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2017[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2018; _temp2018.tag=
Cyc_TypeSpecifier_tok; _temp2018.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2019=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2019[ 0]=({ struct
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
_temp2030=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2030[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2031; _temp2031.tag=
Cyc_TypeSpecifier_tok; _temp2031.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2032=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2032[ 0]=({
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
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2043[ 0]=({ struct Cyc_Kind_tok_struct
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
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2068[ 0]=({ struct Cyc_Kind_tok_struct
_temp2069; _temp2069.tag= Cyc_Kind_tok; _temp2069.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2064, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2069;}); _temp2068;}); break;} case 71:
_LL2053: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp2071=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp2071[ 0]=({
struct Cyc_TypeQual_tok_struct _temp2072; _temp2072.tag= Cyc_TypeQual_tok;
_temp2072.f1=({ struct Cyc_Absyn_Tqual _temp2073; _temp2073.q_const= 1;
_temp2073.q_volatile= 0; _temp2073.q_restrict= 0; _temp2073;}); _temp2072;});
_temp2071;}); break; case 72: _LL2070: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2075=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2075[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2076; _temp2076.tag= Cyc_TypeQual_tok; _temp2076.f1=({ struct Cyc_Absyn_Tqual
_temp2077; _temp2077.q_const= 0; _temp2077.q_volatile= 1; _temp2077.q_restrict=
0; _temp2077;}); _temp2076;}); _temp2075;}); break; case 73: _LL2074: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp2079=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp2079[ 0]=({
struct Cyc_TypeQual_tok_struct _temp2080; _temp2080.tag= Cyc_TypeQual_tok;
_temp2080.f1=({ struct Cyc_Absyn_Tqual _temp2081; _temp2081.q_const= 0;
_temp2081.q_volatile= 0; _temp2081.q_restrict= 1; _temp2081;}); _temp2080;});
_temp2079;}); break; case 74: _LL2078: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2083=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2083[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2084; _temp2084.tag=
Cyc_TypeSpecifier_tok; _temp2084.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2085=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2085[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2086; _temp2086.tag= Cyc_Parse_Decl_spec;
_temp2086.f1=({ struct Cyc_Absyn_Decl* _temp2087=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2087->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2088=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2088[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2089; _temp2089.tag= Cyc_Absyn_Enum_d; _temp2089.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2090=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2090->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2090->name=({ struct
_tuple1* yyzzz;{ void* _temp2100= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct _tuple1* _temp2106; _LL2102: if(*(( void**)
_temp2100) ==  Cyc_QualId_tok){ _LL2107: _temp2106=(( struct Cyc_QualId_tok_struct*)
_temp2100)->f1; goto _LL2103;} else{ goto _LL2104;} _LL2104: goto _LL2105;
_LL2103: yyzzz= _temp2106; goto _LL2101; _LL2105:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2101:;} yyzzz;}); _temp2090->fields=({ struct Cyc_Core_Opt* _temp2091=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2091->v=(
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
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2109[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2110; _temp2110.tag= Cyc_TypeSpecifier_tok;
_temp2110.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2111=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
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
_temp2122=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2122[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2123; _temp2123.tag= Cyc_Enumfield_tok;
_temp2123.f1=({ struct Cyc_Absyn_Enumfield* _temp2124=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2124->name=({ struct
_tuple1* yyzzz;{ void* _temp2125= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple1* _temp2131; _LL2127: if(*(( void**) _temp2125) == 
Cyc_QualId_tok){ _LL2132: _temp2131=(( struct Cyc_QualId_tok_struct*) _temp2125)->f1;
goto _LL2128;} else{ goto _LL2129;} _LL2129: goto _LL2130; _LL2128: yyzzz=
_temp2131; goto _LL2126; _LL2130:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2126:;} yyzzz;}); _temp2124->tag= 0; _temp2124->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2124;});
_temp2123;}); _temp2122;}); break; case 77: _LL2121: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2134=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2134[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2135; _temp2135.tag= Cyc_Enumfield_tok;
_temp2135.f1=({ struct Cyc_Absyn_Enumfield* _temp2136=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2136->name=({ struct
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
_temp2154=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2154[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2155; _temp2155.tag=
Cyc_EnumfieldList_tok; _temp2155.f1=({ struct Cyc_List_List* _temp2156=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2156->hd=( void*)({
struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp2157= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Enumfield*
_temp2163; _LL2159: if(*(( void**) _temp2157) ==  Cyc_Enumfield_tok){ _LL2164:
_temp2163=(( struct Cyc_Enumfield_tok_struct*) _temp2157)->f1; goto _LL2160;}
else{ goto _LL2161;} _LL2161: goto _LL2162; _LL2160: yyzzz= _temp2163; goto
_LL2158; _LL2162:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL2158:;}
yyzzz;}); _temp2156->tl= 0; _temp2156;}); _temp2155;}); _temp2154;}); break;
case 79: _LL2153: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2166=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2166[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2167; _temp2167.tag=
Cyc_EnumfieldList_tok; _temp2167.f1=({ struct Cyc_List_List* _temp2168=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2168->hd=( void*)({
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
_temp2196=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2196[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2197; _temp2197.tag= Cyc_Absyn_AnonStructType; _temp2197.f1=({ struct Cyc_List_List*
yyzzz;{ void* _temp2198= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2204; _LL2200: if(*(( void**)
_temp2198) ==  Cyc_StructFieldDeclList_tok){ _LL2205: _temp2204=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp2198)->f1; goto _LL2201;} else{ goto _LL2202;} _LL2202: goto _LL2203;
_LL2201: yyzzz= _temp2204; goto _LL2199; _LL2203:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2199:;} yyzzz;}); _temp2197;}); _temp2196;}); break; case Cyc_Parse_Union_su:
_LL2195: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp2207=( struct
Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp2207[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp2208; _temp2208.tag=
Cyc_Absyn_AnonUnionType; _temp2208.f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp2209= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp2215; _LL2211: if(*(( void**) _temp2209) ==  Cyc_StructFieldDeclList_tok){
_LL2216: _temp2215=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp2209)->f1;
goto _LL2212;} else{ goto _LL2213;} _LL2213: goto _LL2214; _LL2212: yyzzz=
_temp2215; goto _LL2210; _LL2214:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2210:;} yyzzz;}); _temp2208;}); _temp2207;}); break;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2217=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2217[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2218; _temp2218.tag= Cyc_TypeSpecifier_tok;
_temp2218.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct* _temp2219=(
struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
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
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2233->v=( void*)({ struct _tuple1*
yyzzz;{ void* _temp2234= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp2240; _LL2236: if(*(( void**)
_temp2234) ==  Cyc_QualId_tok){ _LL2241: _temp2240=(( struct Cyc_QualId_tok_struct*)
_temp2234)->f1; goto _LL2237;} else{ goto _LL2238;} _LL2238: goto _LL2239;
_LL2237: yyzzz= _temp2240; goto _LL2235; _LL2239:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2235:;} yyzzz;}); _temp2233;}), _temp2222,({ struct Cyc_Core_Opt* _temp2242=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2242->v=(
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
struct Cyc_Core_Opt* _temp2252=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2252->v=( void*)({ struct _tuple1* yyzzz;{ void* _temp2253=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct
_tuple1* _temp2259; _LL2255: if(*(( void**) _temp2253) ==  Cyc_QualId_tok){
_LL2260: _temp2259=(( struct Cyc_QualId_tok_struct*) _temp2253)->f1; goto
_LL2256;} else{ goto _LL2257;} _LL2257: goto _LL2258; _LL2256: yyzzz= _temp2259;
goto _LL2254; _LL2258:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2254:;}
yyzzz;}); _temp2252;}), _temp2222,({ struct Cyc_Core_Opt* _temp2261=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2261->v=( void*)({ struct Cyc_List_List*
yyzzz;{ void* _temp2262= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2268; _LL2264: if(*(( void**)
_temp2262) ==  Cyc_StructFieldDeclList_tok){ _LL2269: _temp2268=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp2262)->f1; goto _LL2265;} else{ goto _LL2266;} _LL2266: goto _LL2267;
_LL2265: yyzzz= _temp2268; goto _LL2263; _LL2267:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2263:;} yyzzz;}); _temp2261;}), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2270=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2270[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2271; _temp2271.tag= Cyc_TypeSpecifier_tok;
_temp2271.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2272=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
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
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2294->v=( void*)({ struct _tuple1*
yyzzz;{ void* _temp2295= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp2301; _LL2297: if(*(( void**)
_temp2295) ==  Cyc_QualId_tok){ _LL2302: _temp2301=(( struct Cyc_QualId_tok_struct*)
_temp2295)->f1; goto _LL2298;} else{ goto _LL2299;} _LL2299: goto _LL2300;
_LL2298: yyzzz= _temp2301; goto _LL2296; _LL2300:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2296:;} yyzzz;}); _temp2294;}), _temp2283,({ struct Cyc_Core_Opt* _temp2303=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2303->v=(
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
struct Cyc_Core_Opt* _temp2313=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2313->v=( void*)({ struct _tuple1* yyzzz;{ void* _temp2314=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct
_tuple1* _temp2320; _LL2316: if(*(( void**) _temp2314) ==  Cyc_QualId_tok){
_LL2321: _temp2320=(( struct Cyc_QualId_tok_struct*) _temp2314)->f1; goto
_LL2317;} else{ goto _LL2318;} _LL2318: goto _LL2319; _LL2317: yyzzz= _temp2320;
goto _LL2315; _LL2319:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL2315:;}
yyzzz;}); _temp2313;}), _temp2283,({ struct Cyc_Core_Opt* _temp2322=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2322->v=( void*)({ struct Cyc_List_List*
yyzzz;{ void* _temp2323= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp2329; _LL2325: if(*(( void**)
_temp2323) ==  Cyc_StructFieldDeclList_tok){ _LL2330: _temp2329=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp2323)->f1; goto _LL2326;} else{ goto _LL2327;} _LL2327: goto _LL2328;
_LL2326: yyzzz= _temp2329; goto _LL2324; _LL2328:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2324:;} yyzzz;}); _temp2322;}), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2331=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2331[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2332; _temp2332.tag= Cyc_TypeSpecifier_tok;
_temp2332.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2333=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
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
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2354[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2355; _temp2355.tag= Cyc_TypeSpecifier_tok;
_temp2355.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2356=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
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
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2375[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2376; _temp2376.tag= Cyc_TypeSpecifier_tok;
_temp2376.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2377=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
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
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2406[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2407; _temp2407.tag= Cyc_TypeSpecifier_tok;
_temp2407.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2408=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
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
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2427[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2428; _temp2428.tag= Cyc_TypeSpecifier_tok;
_temp2428.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2429=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
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
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2448[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2449; _temp2449.tag= Cyc_TypeList_tok; _temp2449.f1= 0; _temp2449;});
_temp2448;}); break; case 86: _LL2447: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2451=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
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
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2462[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2463; _temp2463.tag= Cyc_TypeList_tok; _temp2463.f1= Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp2464= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp2470; _LL2466: if(*(( void**) _temp2464) ==  Cyc_TypeList_tok){ _LL2471:
_temp2470=(( struct Cyc_TypeList_tok_struct*) _temp2464)->f1; goto _LL2467;}
else{ goto _LL2468;} _LL2468: goto _LL2469; _LL2467: yyzzz= _temp2470; goto
_LL2465; _LL2469:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL2465:;}
yyzzz;})); _temp2463;}); _temp2462;}); break; case 88: _LL2461: yyval=( void*)({
struct Cyc_StructOrUnion_tok_struct* _temp2473=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp2473[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp2474; _temp2474.tag= Cyc_StructOrUnion_tok;
_temp2474.f1= Cyc_Parse_Struct_su; _temp2474;}); _temp2473;}); break; case 89:
_LL2472: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct* _temp2476=(
struct Cyc_StructOrUnion_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2476[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2477; _temp2477.tag=
Cyc_StructOrUnion_tok; _temp2477.f1= Cyc_Parse_Union_su; _temp2477;}); _temp2476;});
break; case 90: _LL2475: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2479=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
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
struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct));
_temp2490[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct _temp2491;
_temp2491.tag= Cyc_StructFieldDeclListList_tok; _temp2491.f1=({ struct Cyc_List_List*
_temp2492=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2492->hd=( void*)({ struct Cyc_List_List* yyzzz;{ void* _temp2493= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp2499; _LL2495: if(*(( void**) _temp2493) ==  Cyc_StructFieldDeclList_tok){
_LL2500: _temp2499=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp2493)->f1;
goto _LL2496;} else{ goto _LL2497;} _LL2497: goto _LL2498; _LL2496: yyzzz=
_temp2499; goto _LL2494; _LL2498:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL2494:;} yyzzz;}); _temp2492->tl= 0; _temp2492;}); _temp2491;}); _temp2490;});
break; case 92: _LL2489: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2502=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2502[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2503; _temp2503.tag= Cyc_StructFieldDeclListList_tok; _temp2503.f1=({
struct Cyc_List_List* _temp2504=( struct Cyc_List_List*) GC_malloc( sizeof(
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
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
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
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp2533[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp2534; _temp2534.tag= Cyc_InitDeclList_tok;
_temp2534.f1=({ struct Cyc_List_List* _temp2535=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2535->hd=( void*)({ struct
_tuple13* yyzzz;{ void* _temp2536= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple13* _temp2542; _LL2538: if(*(( void**) _temp2536)
==  Cyc_InitDecl_tok){ _LL2543: _temp2542=(( struct Cyc_InitDecl_tok_struct*)
_temp2536)->f1; goto _LL2539;} else{ goto _LL2540;} _LL2540: goto _LL2541;
_LL2539: yyzzz= _temp2542; goto _LL2537; _LL2541:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL2537:;} yyzzz;}); _temp2535->tl= 0; _temp2535;}); _temp2534;}); _temp2533;});
break; case 95: _LL2532: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp2545=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2545[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2546; _temp2546.tag=
Cyc_InitDeclList_tok; _temp2546.f1=({ struct Cyc_List_List* _temp2547=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2547->hd=( void*)({
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
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2565[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2566; _temp2566.tag= Cyc_InitDecl_tok; _temp2566.f1=({ struct _tuple13*
_temp2567=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2567->f1=({
struct Cyc_Parse_Declarator* yyzzz;{ void* _temp2568= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declarator*
_temp2574; _LL2570: if(*(( void**) _temp2568) ==  Cyc_Declarator_tok){ _LL2575:
_temp2574=(( struct Cyc_Declarator_tok_struct*) _temp2568)->f1; goto _LL2571;}
else{ goto _LL2572;} _LL2572: goto _LL2573; _LL2571: yyzzz= _temp2574; goto
_LL2569; _LL2573:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL2569:;}
yyzzz;}); _temp2567->f2= 0; _temp2567;}); _temp2566;}); _temp2565;}); break;
case 97: _LL2564: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2577=(
struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp2577[ 0]=({ struct Cyc_InitDecl_tok_struct _temp2578; _temp2578.tag= Cyc_InitDecl_tok;
_temp2578.f1=({ struct _tuple13* _temp2579=( struct _tuple13*) GC_malloc(
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
Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp2630[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp2631; _temp2631.tag=
Cyc_StructFieldDeclList_tok; _temp2631.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp2629); _temp2631;}); _temp2630;}); break;}}}
case 99: _LL2596: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2633=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2633[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2634; _temp2634.tag=
Cyc_QualSpecList_tok; _temp2634.f1=({ struct _tuple17* _temp2635=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2635->f1= Cyc_Absyn_empty_tqual();
_temp2635->f2=({ struct Cyc_List_List* _temp2644=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2644->hd=( void*)({ void* yyzzz;{
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
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2654[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2655; _temp2655.tag=
Cyc_QualSpecList_tok; _temp2655.f1=({ struct _tuple17* _temp2656=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2656->f1=(*({ struct
_tuple17* yyzzz;{ void* _temp2690= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple17* _temp2696; _LL2692: if(*(( void**) _temp2690)
==  Cyc_QualSpecList_tok){ _LL2697: _temp2696=(( struct Cyc_QualSpecList_tok_struct*)
_temp2690)->f1; goto _LL2693;} else{ goto _LL2694;} _LL2694: goto _LL2695;
_LL2693: yyzzz= _temp2696; goto _LL2691; _LL2695:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL2691:;} yyzzz;})).f1; _temp2656->f2=({ struct Cyc_List_List* _temp2673=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2673->hd=(
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
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2699[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2700; _temp2700.tag=
Cyc_QualSpecList_tok; _temp2700.f1=({ struct _tuple17* _temp2701=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2701->f1=({ struct Cyc_Absyn_Tqual
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
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2719[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2720; _temp2720.tag=
Cyc_QualSpecList_tok; _temp2720.f1=({ struct _tuple17* _temp2721=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2721->f1= Cyc_Absyn_combine_tqual(({
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
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
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
Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp2774[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp2775; _temp2775.tag=
Cyc_DeclaratorExpoptList_tok; _temp2775.f1=({ struct Cyc_List_List* _temp2776=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2776->hd=(
void*)({ struct _tuple13* yyzzz;{ void* _temp2777= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple13*
_temp2783; _LL2779: if(*(( void**) _temp2777) ==  Cyc_DeclaratorExpopt_tok){
_LL2784: _temp2783=(( struct Cyc_DeclaratorExpopt_tok_struct*) _temp2777)->f1;
goto _LL2780;} else{ goto _LL2781;} _LL2781: goto _LL2782; _LL2780: yyzzz=
_temp2783; goto _LL2778; _LL2782:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL2778:;} yyzzz;}); _temp2776->tl= 0; _temp2776;}); _temp2775;}); _temp2774;});
break; case 105: _LL2773: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp2786=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2786[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp2787; _temp2787.tag= Cyc_DeclaratorExpoptList_tok; _temp2787.f1=({ struct
Cyc_List_List* _temp2788=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2806[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2807; _temp2807.tag=
Cyc_DeclaratorExpopt_tok; _temp2807.f1=({ struct _tuple13* _temp2808=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2808->f1=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp2809= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Declarator* _temp2815; _LL2811: if(*(( void**)
_temp2809) ==  Cyc_Declarator_tok){ _LL2816: _temp2815=(( struct Cyc_Declarator_tok_struct*)
_temp2809)->f1; goto _LL2812;} else{ goto _LL2813;} _LL2813: goto _LL2814;
_LL2812: yyzzz= _temp2815; goto _LL2810; _LL2814:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL2810:;} yyzzz;}); _temp2808->f2= 0; _temp2808;}); _temp2807;}); _temp2806;});
break; case 107: _LL2805: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2818=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2818[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2819; _temp2819.tag= Cyc_DeclaratorExpopt_tok; _temp2819.f1=({ struct
_tuple13* _temp2820=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp2820->f1=({ struct Cyc_Parse_Declarator* _temp2829=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2829->id=({ struct
_tuple1* _temp2830=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2830->f1= Cyc_Absyn_rel_ns_null; _temp2830->f2= _init_tag_arr(( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char),
1u); _temp2830;}); _temp2829->tms= 0; _temp2829;}); _temp2820->f2=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp2821= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp2827; _LL2823: if(*(( void**) _temp2821) ==  Cyc_Exp_tok){ _LL2828:
_temp2827=(( struct Cyc_Exp_tok_struct*) _temp2821)->f1; goto _LL2824;} else{
goto _LL2825;} _LL2825: goto _LL2826; _LL2824: yyzzz= _temp2827; goto _LL2822;
_LL2826:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL2822:;} yyzzz;});
_temp2820;}); _temp2819;}); _temp2818;}); break; case 108: _LL2817: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2832=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2832[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2833; _temp2833.tag= Cyc_DeclaratorExpopt_tok;
_temp2833.f1=({ struct _tuple13* _temp2834=( struct _tuple13*) GC_malloc(
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
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2853[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2854; _temp2854.tag=
Cyc_TypeSpecifier_tok; _temp2854.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2855=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2855[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2856; _temp2856.tag= Cyc_Parse_Decl_spec;
_temp2856.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct _tuple1*
yyzzz;{ void* _temp2857= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp2863; _LL2859: if(*(( void**)
_temp2857) ==  Cyc_QualId_tok){ _LL2864: _temp2863=(( struct Cyc_QualId_tok_struct*)
_temp2857)->f1; goto _LL2860;} else{ goto _LL2861;} _LL2861: goto _LL2862;
_LL2860: yyzzz= _temp2863; goto _LL2858; _LL2862:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL2858:;} yyzzz;}), _temp2852,({ struct Cyc_Core_Opt* _temp2865=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2865->v=( void*)({ struct Cyc_List_List*
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
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2891[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2892; _temp2892.tag=
Cyc_TypeSpecifier_tok; _temp2892.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2893=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2893[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2894; _temp2894.tag= Cyc_Absyn_TunionType;
_temp2894.f1=({ struct Cyc_Absyn_TunionInfo _temp2895; _temp2895.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2912=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2912[ 0]=({
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
_temp2932=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2932[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2933; _temp2933.tag=
Cyc_TypeSpecifier_tok; _temp2933.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2934=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2934[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2935; _temp2935.tag= Cyc_Absyn_TunionType;
_temp2935.f1=({ struct Cyc_Absyn_TunionInfo _temp2936; _temp2936.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2945=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2945[ 0]=({
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
_temp2965=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2965[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2966; _temp2966.tag=
Cyc_TypeSpecifier_tok; _temp2966.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp2967=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp2967[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp2968; _temp2968.tag= Cyc_Absyn_TunionFieldType;
_temp2968.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp2969; _temp2969.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp2978=( struct
Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
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
Cyc_Bool_tok_struct* _temp3006=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp3006[ 0]=({ struct Cyc_Bool_tok_struct
_temp3007; _temp3007.tag= Cyc_Bool_tok; _temp3007.f1= 0; _temp3007;}); _temp3006;});
break; case 114: _LL3005: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3009=(
struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp3009[ 0]=({ struct Cyc_Bool_tok_struct _temp3010; _temp3010.tag= Cyc_Bool_tok;
_temp3010.f1= 1; _temp3010;}); _temp3009;}); break; case 115: _LL3008: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3012=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3012[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3013; _temp3013.tag= Cyc_TunionFieldList_tok;
_temp3013.f1=({ struct Cyc_List_List* _temp3014=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3014->hd=( void*)({ struct Cyc_Absyn_Tunionfield*
yyzzz;{ void* _temp3015= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Tunionfield* _temp3021; _LL3017: if(*(( void**)
_temp3015) ==  Cyc_TunionField_tok){ _LL3022: _temp3021=(( struct Cyc_TunionField_tok_struct*)
_temp3015)->f1; goto _LL3018;} else{ goto _LL3019;} _LL3019: goto _LL3020;
_LL3018: yyzzz= _temp3021; goto _LL3016; _LL3020:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL3016:;} yyzzz;}); _temp3014->tl= 0; _temp3014;}); _temp3013;}); _temp3012;});
break; case 116: _LL3011: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3024=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3024[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3025; _temp3025.tag= Cyc_TunionFieldList_tok; _temp3025.f1=({ struct Cyc_List_List*
_temp3026=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3026->hd=( void*)({ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp3027=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Tunionfield*
_temp3033; _LL3029: if(*(( void**) _temp3027) ==  Cyc_TunionField_tok){ _LL3034:
_temp3033=(( struct Cyc_TunionField_tok_struct*) _temp3027)->f1; goto _LL3030;}
else{ goto _LL3031;} _LL3031: goto _LL3032; _LL3030: yyzzz= _temp3033; goto
_LL3028; _LL3032:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL3028:;}
yyzzz;}); _temp3026->tl= 0; _temp3026;}); _temp3025;}); _temp3024;}); break;
case 117: _LL3023: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3036=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3036[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3037; _temp3037.tag= Cyc_TunionFieldList_tok; _temp3037.f1=({ struct Cyc_List_List*
_temp3038=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
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
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3056[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3057; _temp3057.tag=
Cyc_TunionFieldList_tok; _temp3057.f1=({ struct Cyc_List_List* _temp3058=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3058->hd=(
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
GC_malloc( sizeof( struct Cyc_Scope_tok_struct)); _temp3076[ 0]=({ struct Cyc_Scope_tok_struct
_temp3077; _temp3077.tag= Cyc_Scope_tok; _temp3077.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp3077;}); _temp3076;}); break; case 120: _LL3075: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp3079=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp3079[ 0]=({ struct Cyc_Scope_tok_struct
_temp3080; _temp3080.tag= Cyc_Scope_tok; _temp3080.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp3080;}); _temp3079;}); break; case 121: _LL3078: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp3082=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp3082[ 0]=({ struct Cyc_Scope_tok_struct
_temp3083; _temp3083.tag= Cyc_Scope_tok; _temp3083.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp3083;}); _temp3082;}); break; case 122: _LL3081: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp3085=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3085[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3086; _temp3086.tag= Cyc_TunionField_tok;
_temp3086.f1=({ struct Cyc_Absyn_Tunionfield* _temp3087=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3087->name=({ struct
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
struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3107[ 0]=({ struct Cyc_TunionField_tok_struct _temp3108; _temp3108.tag= Cyc_TunionField_tok;
_temp3108.f1=({ struct Cyc_Absyn_Tunionfield* _temp3109=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3109->name=({ struct
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
_temp3144=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3144[ 0]=({ struct Cyc_Declarator_tok_struct _temp3145; _temp3145.tag= Cyc_Declarator_tok;
_temp3145.f1=({ struct Cyc_Parse_Declarator* _temp3146=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3146->id=({ struct Cyc_Parse_Declarator*
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
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3172[ 0]=({ struct Cyc_Declarator_tok_struct _temp3173; _temp3173.tag= Cyc_Declarator_tok;
_temp3173.f1=({ struct Cyc_Parse_Declarator* _temp3174=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3174->id=({ struct
_tuple1* yyzzz;{ void* _temp3175= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple1* _temp3181; _LL3177: if(*(( void**) _temp3175) == 
Cyc_QualId_tok){ _LL3182: _temp3181=(( struct Cyc_QualId_tok_struct*) _temp3175)->f1;
goto _LL3178;} else{ goto _LL3179;} _LL3179: goto _LL3180; _LL3178: yyzzz=
_temp3181; goto _LL3176; _LL3180:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL3176:;} yyzzz;}); _temp3174->tms= 0; _temp3174;}); _temp3173;}); _temp3172;});
break; case 127: _LL3171: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; break; case 128: _LL3183: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3185=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3185[ 0]=({ struct Cyc_Declarator_tok_struct _temp3186; _temp3186.tag= Cyc_Declarator_tok;
_temp3186.f1=({ struct Cyc_Parse_Declarator* _temp3187=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3187->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3197= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Parse_Declarator* _temp3203; _LL3199: if(*((
void**) _temp3197) ==  Cyc_Declarator_tok){ _LL3204: _temp3203=(( struct Cyc_Declarator_tok_struct*)
_temp3197)->f1; goto _LL3200;} else{ goto _LL3201;} _LL3201: goto _LL3202;
_LL3200: yyzzz= _temp3203; goto _LL3198; _LL3202:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3198:;} yyzzz;})->id; _temp3187->tms=({ struct Cyc_List_List* _temp3188=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3188->hd=(
void*)(( void*) Cyc_Absyn_Carray_mod); _temp3188->tl=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3189= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Parse_Declarator* _temp3195; _LL3191: if(*((
void**) _temp3189) ==  Cyc_Declarator_tok){ _LL3196: _temp3195=(( struct Cyc_Declarator_tok_struct*)
_temp3189)->f1; goto _LL3192;} else{ goto _LL3193;} _LL3193: goto _LL3194;
_LL3192: yyzzz= _temp3195; goto _LL3190; _LL3194:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3190:;} yyzzz;})->tms; _temp3188;}); _temp3187;}); _temp3186;}); _temp3185;});
break; case 129: _LL3184: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3206=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3206[ 0]=({ struct Cyc_Declarator_tok_struct _temp3207; _temp3207.tag= Cyc_Declarator_tok;
_temp3207.f1=({ struct Cyc_Parse_Declarator* _temp3208=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3208->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3228= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3234; _LL3230: if(*((
void**) _temp3228) ==  Cyc_Declarator_tok){ _LL3235: _temp3234=(( struct Cyc_Declarator_tok_struct*)
_temp3228)->f1; goto _LL3231;} else{ goto _LL3232;} _LL3232: goto _LL3233;
_LL3231: yyzzz= _temp3234; goto _LL3229; _LL3233:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3229:;} yyzzz;})->id; _temp3208->tms=({ struct Cyc_List_List* _temp3209=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3209->hd=(
void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3218=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3218[ 0]=({
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
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3258[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3259; _temp3259.tag= Cyc_Declarator_tok;
_temp3259.f1=({ struct Cyc_Parse_Declarator* _temp3260=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3260->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3274= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3280; _LL3276: if(*((
void**) _temp3274) ==  Cyc_Declarator_tok){ _LL3281: _temp3280=(( struct Cyc_Declarator_tok_struct*)
_temp3274)->f1; goto _LL3277;} else{ goto _LL3278;} _LL3278: goto _LL3279;
_LL3277: yyzzz= _temp3280; goto _LL3275; _LL3279:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3275:;} yyzzz;})->id; _temp3260->tms=({ struct Cyc_List_List* _temp3261=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3261->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3270=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3270[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3271; _temp3271.tag= Cyc_Absyn_Function_mod;
_temp3271.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3272=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
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
_temp3283=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3283[ 0]=({ struct Cyc_Declarator_tok_struct _temp3284; _temp3284.tag= Cyc_Declarator_tok;
_temp3284.f1=({ struct Cyc_Parse_Declarator* _temp3285=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3285->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3315= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct Cyc_Parse_Declarator* _temp3321; _LL3317: if(*((
void**) _temp3315) ==  Cyc_Declarator_tok){ _LL3322: _temp3321=(( struct Cyc_Declarator_tok_struct*)
_temp3315)->f1; goto _LL3318;} else{ goto _LL3319;} _LL3319: goto _LL3320;
_LL3318: yyzzz= _temp3321; goto _LL3316; _LL3320:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3316:;} yyzzz;})->id; _temp3285->tms=({ struct Cyc_List_List* _temp3286=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3286->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3295=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3295[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3296; _temp3296.tag= Cyc_Absyn_Function_mod;
_temp3296.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3297=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
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
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3324[ 0]=({ struct Cyc_Declarator_tok_struct _temp3325; _temp3325.tag= Cyc_Declarator_tok;
_temp3325.f1=({ struct Cyc_Parse_Declarator* _temp3326=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3326->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3348= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3354; _LL3350: if(*((
void**) _temp3348) ==  Cyc_Declarator_tok){ _LL3355: _temp3354=(( struct Cyc_Declarator_tok_struct*)
_temp3348)->f1; goto _LL3351;} else{ goto _LL3352;} _LL3352: goto _LL3353;
_LL3351: yyzzz= _temp3354; goto _LL3349; _LL3353:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3349:;} yyzzz;})->id; _temp3326->tms=({ struct Cyc_List_List* _temp3327=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3327->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3336=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3336[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3337; _temp3337.tag= Cyc_Absyn_Function_mod;
_temp3337.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp3338=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
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
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3358[ 0]=({ struct Cyc_Declarator_tok_struct _temp3359; _temp3359.tag= Cyc_Declarator_tok;
_temp3359.f1=({ struct Cyc_Parse_Declarator* _temp3360=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3360->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3372= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3378; _LL3374: if(*((
void**) _temp3372) ==  Cyc_Declarator_tok){ _LL3379: _temp3378=(( struct Cyc_Declarator_tok_struct*)
_temp3372)->f1; goto _LL3375;} else{ goto _LL3376;} _LL3376: goto _LL3377;
_LL3375: yyzzz= _temp3378; goto _LL3373; _LL3377:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3373:;} yyzzz;})->id; _temp3360->tms=({ struct Cyc_List_List* _temp3361=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3361->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3370=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3370[ 0]=({
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
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3390[ 0]=({ struct Cyc_Declarator_tok_struct _temp3391; _temp3391.tag= Cyc_Declarator_tok;
_temp3391.f1=({ struct Cyc_Parse_Declarator* _temp3392=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3392->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3404= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Declarator* _temp3410; _LL3406: if(*((
void**) _temp3404) ==  Cyc_Declarator_tok){ _LL3411: _temp3410=(( struct Cyc_Declarator_tok_struct*)
_temp3404)->f1; goto _LL3407;} else{ goto _LL3408;} _LL3408: goto _LL3409;
_LL3407: yyzzz= _temp3410; goto _LL3405; _LL3409:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3405:;} yyzzz;})->id; _temp3392->tms=({ struct Cyc_List_List* _temp3393=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3393->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3402=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3402[ 0]=({
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
_temp3421=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3421[ 0]=({ struct Cyc_Declarator_tok_struct _temp3422; _temp3422.tag= Cyc_Declarator_tok;
_temp3422.f1=({ struct Cyc_Parse_Declarator* _temp3423=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3423->id=({ struct Cyc_Parse_Declarator*
yyzzz;{ void* _temp3443= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Parse_Declarator* _temp3449; _LL3445: if(*((
void**) _temp3443) ==  Cyc_Declarator_tok){ _LL3450: _temp3449=(( struct Cyc_Declarator_tok_struct*)
_temp3443)->f1; goto _LL3446;} else{ goto _LL3447;} _LL3447: goto _LL3448;
_LL3446: yyzzz= _temp3449; goto _LL3444; _LL3448:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL3444:;} yyzzz;})->id; _temp3423->tms=({ struct Cyc_List_List* _temp3424=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3424->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp3433=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3433[ 0]=({
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
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3452[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3453; _temp3453.tag= Cyc_Declarator_tok;
_temp3453.f1=({ struct Cyc_Parse_Declarator* _temp3454=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3454->id=({ struct
_tuple1* yyzzz;{ void* _temp3455= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple1* _temp3461; _LL3457: if(*(( void**) _temp3455) == 
Cyc_QualId_tok){ _LL3462: _temp3461=(( struct Cyc_QualId_tok_struct*) _temp3455)->f1;
goto _LL3458;} else{ goto _LL3459;} _LL3459: goto _LL3460; _LL3458: yyzzz=
_temp3461; goto _LL3456; _LL3460:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL3456:;} yyzzz;}); _temp3454->tms= 0; _temp3454;}); _temp3453;}); _temp3452;});
break; case 137: _LL3451: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3464=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3464[ 0]=({ struct Cyc_Declarator_tok_struct _temp3465; _temp3465.tag= Cyc_Declarator_tok;
_temp3465.f1=({ struct Cyc_Parse_Declarator* _temp3466=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3466->id= Cyc_Absyn_exn_name;
_temp3466->tms= 0; _temp3466;}); _temp3465;}); _temp3464;}); break; case 138:
_LL3463: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3468=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3468[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3469; _temp3469.tag=
Cyc_TypeModifierList_tok; _temp3469.f1=({ struct Cyc_List_List* _temp3470=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3470->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3479=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3479[ 0]=({
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
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3498[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3499; _temp3499.tag=
Cyc_TypeModifierList_tok; _temp3499.f1=({ struct Cyc_List_List* _temp3500=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3500->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3509=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3509[ 0]=({
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
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3536[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3537; _temp3537.tag=
Cyc_TypeModifierList_tok; _temp3537.f1=({ struct Cyc_List_List* _temp3538=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3538->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3555=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3555[ 0]=({
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
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp3574[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp3575; _temp3575.tag=
Cyc_TypeModifierList_tok; _temp3575.f1=({ struct Cyc_List_List* _temp3576=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3576->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3593=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3593[ 0]=({
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
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3620[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3621; _temp3621.tag=
Cyc_Pointer_Sort_tok; _temp3621.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3622=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3622[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3623; _temp3623.tag=
Cyc_Absyn_Nullable_ps; _temp3623.f1= Cyc_Absyn_exp_unsigned_one; _temp3623;});
_temp3622;})); _temp3621;}); _temp3620;}); break; case 143: _LL3619: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3625=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3625[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3626; _temp3626.tag= Cyc_Pointer_Sort_tok;
_temp3626.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3627=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3627[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3628; _temp3628.tag= Cyc_Absyn_NonNullable_ps; _temp3628.f1= Cyc_Absyn_exp_unsigned_one;
_temp3628;}); _temp3627;})); _temp3626;}); _temp3625;}); break; case 144:
_LL3624: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3630=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3630[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3631; _temp3631.tag=
Cyc_Pointer_Sort_tok; _temp3631.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3632=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3632[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3633; _temp3633.tag=
Cyc_Absyn_Nullable_ps; _temp3633.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp3634= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Exp* _temp3640; _LL3636: if(*(( void**) _temp3634) ==  Cyc_Exp_tok){
_LL3641: _temp3640=(( struct Cyc_Exp_tok_struct*) _temp3634)->f1; goto _LL3637;}
else{ goto _LL3638;} _LL3638: goto _LL3639; _LL3637: yyzzz= _temp3640; goto
_LL3635; _LL3639:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL3635:;} yyzzz;});
_temp3633;}); _temp3632;})); _temp3631;}); _temp3630;}); break; case 145:
_LL3629: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3643=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3643[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3644; _temp3644.tag=
Cyc_Pointer_Sort_tok; _temp3644.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3645=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3645[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3646; _temp3646.tag= Cyc_Absyn_NonNullable_ps; _temp3646.f1=({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp3647= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Absyn_Exp* _temp3653; _LL3649: if(*(( void**)
_temp3647) ==  Cyc_Exp_tok){ _LL3654: _temp3653=(( struct Cyc_Exp_tok_struct*)
_temp3647)->f1; goto _LL3650;} else{ goto _LL3651;} _LL3651: goto _LL3652;
_LL3650: yyzzz= _temp3653; goto _LL3648; _LL3652:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL3648:;} yyzzz;}); _temp3646;}); _temp3645;})); _temp3644;}); _temp3643;});
break; case 146: _LL3642: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3656=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3656[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3657; _temp3657.tag=
Cyc_Pointer_Sort_tok; _temp3657.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp3657;}); _temp3656;}); break; case 147: _LL3655: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp3659=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp3659[ 0]=({ struct Cyc_Type_tok_struct
_temp3660; _temp3660.tag= Cyc_Type_tok; _temp3660.f1=( void*) Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp3661=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
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
Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3673[
0]=({ struct Cyc_Type_tok_struct _temp3674; _temp3674.tag= Cyc_Type_tok;
_temp3674.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp3675=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3675->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp3675;}), 0); _temp3674;}); _temp3673;});
break; case 151: _LL3672: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL3676: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp3678=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
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
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3697[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3698; _temp3698.tag= Cyc_ParamDeclListBool_tok;
_temp3698.f1=({ struct _tuple16* _temp3699=( struct _tuple16*) GC_malloc(
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
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3725[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3726; _temp3726.tag= Cyc_ParamDeclListBool_tok;
_temp3726.f1=({ struct _tuple16* _temp3727=( struct _tuple16*) GC_malloc(
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
struct Cyc_Absyn_VarargInfo* _temp3790=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp3790->name= _temp3768; _temp3790->tq=
_temp3766; _temp3790->type=( void*) _temp3764; _temp3790->rgn=( void*)({ void*
yyzzz;{ void* _temp3799= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; void* _temp3805; _LL3801: if(*(( void**) _temp3799) ==  Cyc_Type_tok){
_LL3806: _temp3805=( void*)(( struct Cyc_Type_tok_struct*) _temp3799)->f1; goto
_LL3802;} else{ goto _LL3803;} _LL3803: goto _LL3804; _LL3802: yyzzz= _temp3805;
goto _LL3800; _LL3804:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL3800:;}
yyzzz;}); _temp3790->inject=({ int yyzzz;{ void* _temp3791= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; int _temp3797;
_LL3793: if(*(( void**) _temp3791) ==  Cyc_Bool_tok){ _LL3798: _temp3797=((
struct Cyc_Bool_tok_struct*) _temp3791)->f1; goto _LL3794;} else{ goto _LL3795;}
_LL3795: goto _LL3796; _LL3794: yyzzz= _temp3797; goto _LL3792; _LL3796:( int)
_throw(( void*)& Cyc_yyfail_Bool_tok); _LL3792:;} yyzzz;}); _temp3790;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp3771=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3771[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3772; _temp3772.tag= Cyc_ParamDeclListBool_tok;
_temp3772.f1=({ struct _tuple16* _temp3773=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp3773->f1= 0; _temp3773->f2= 0; _temp3773->f3=
_temp3770; _temp3773->f4=({ struct Cyc_Core_Opt* yyzzz;{ void* _temp3782= yyvs[
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
struct _tuple2 _temp3818; void* _temp3819; struct Cyc_Absyn_Tqual _temp3821;
struct Cyc_Core_Opt* _temp3823; struct _tuple2* _temp3816=({ struct _tuple2*
yyzzz;{ void* _temp3808= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tuple2* _temp3814; _LL3810: if(*(( void**)
_temp3808) ==  Cyc_ParamDecl_tok){ _LL3815: _temp3814=(( struct Cyc_ParamDecl_tok_struct*)
_temp3808)->f1; goto _LL3811;} else{ goto _LL3812;} _LL3812: goto _LL3813;
_LL3811: yyzzz= _temp3814; goto _LL3809; _LL3813:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL3809:;} yyzzz;}); _temp3818=* _temp3816; _LL3824: _temp3823= _temp3818.f1;
goto _LL3822; _LL3822: _temp3821= _temp3818.f2; goto _LL3820; _LL3820: _temp3819=
_temp3818.f3; goto _LL3817; _LL3817: { struct Cyc_Absyn_VarargInfo* _temp3825=({
struct Cyc_Absyn_VarargInfo* _temp3853=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp3853->name= _temp3823; _temp3853->tq=
_temp3821; _temp3853->type=( void*) _temp3819; _temp3853->rgn=( void*)({ void*
yyzzz;{ void* _temp3862= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; void* _temp3868; _LL3864: if(*(( void**) _temp3862) ==  Cyc_Type_tok){
_LL3869: _temp3868=( void*)(( struct Cyc_Type_tok_struct*) _temp3862)->f1; goto
_LL3865;} else{ goto _LL3866;} _LL3866: goto _LL3867; _LL3865: yyzzz= _temp3868;
goto _LL3863; _LL3867:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL3863:;}
yyzzz;}); _temp3853->inject=({ int yyzzz;{ void* _temp3854= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; int _temp3860;
_LL3856: if(*(( void**) _temp3854) ==  Cyc_Bool_tok){ _LL3861: _temp3860=((
struct Cyc_Bool_tok_struct*) _temp3854)->f1; goto _LL3857;} else{ goto _LL3858;}
_LL3858: goto _LL3859; _LL3857: yyzzz= _temp3860; goto _LL3855; _LL3859:( int)
_throw(( void*)& Cyc_yyfail_Bool_tok); _LL3855:;} yyzzz;}); _temp3853;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp3826=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3826[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3827; _temp3827.tag= Cyc_ParamDeclListBool_tok;
_temp3827.f1=({ struct _tuple16* _temp3828=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp3828->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp3845= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]; struct Cyc_List_List*
_temp3851; _LL3847: if(*(( void**) _temp3845) ==  Cyc_ParamDeclList_tok){
_LL3852: _temp3851=(( struct Cyc_ParamDeclList_tok_struct*) _temp3845)->f1; goto
_LL3848;} else{ goto _LL3849;} _LL3849: goto _LL3850; _LL3848: yyzzz= _temp3851;
goto _LL3846; _LL3850:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL3846:;} yyzzz;})); _temp3828->f2= 0; _temp3828->f3= _temp3825; _temp3828->f4=({
struct Cyc_Core_Opt* yyzzz;{ void* _temp3837= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Core_Opt*
_temp3843; _LL3839: if(*(( void**) _temp3837) ==  Cyc_TypeOpt_tok){ _LL3844:
_temp3843=(( struct Cyc_TypeOpt_tok_struct*) _temp3837)->f1; goto _LL3840;}
else{ goto _LL3841;} _LL3841: goto _LL3842; _LL3840: yyzzz= _temp3843; goto
_LL3838; _LL3842:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL3838:;}
yyzzz;}); _temp3828->f5=({ struct Cyc_List_List* yyzzz;{ void* _temp3829= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3835; _LL3831: if(*(( void**) _temp3829) ==  Cyc_Rgnorder_tok){ _LL3836:
_temp3835=(( struct Cyc_Rgnorder_tok_struct*) _temp3829)->f1; goto _LL3832;}
else{ goto _LL3833;} _LL3833: goto _LL3834; _LL3832: yyzzz= _temp3835; goto
_LL3830; _LL3834:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3830:;}
yyzzz;}); _temp3828;}); _temp3827;}); _temp3826;}); break;}} case 157: _LL3807:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3871=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3871[ 0]=({ struct Cyc_Type_tok_struct
_temp3872; _temp3872.tag= Cyc_Type_tok; _temp3872.f1=( void*) Cyc_Parse_id2type(({
struct _tagged_arr yyzzz;{ void* _temp3873= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp3879; _LL3875: if(*(( void**)
_temp3873) ==  Cyc_String_tok){ _LL3880: _temp3879=(( struct Cyc_String_tok_struct*)
_temp3873)->f1; goto _LL3876;} else{ goto _LL3877;} _LL3877: goto _LL3878;
_LL3876: yyzzz= _temp3879; goto _LL3874; _LL3878:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL3874:;} yyzzz;}), Cyc_Absyn_empty_conref()); _temp3872;}); _temp3871;});
break; case 158: _LL3870: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3882=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3882[ 0]=({ struct Cyc_Type_tok_struct _temp3883; _temp3883.tag= Cyc_Type_tok;
_temp3883.f1=( void*) Cyc_Parse_id2type(({ struct _tagged_arr yyzzz;{ void*
_temp3884= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct _tagged_arr _temp3890; _LL3886: if(*(( void**) _temp3884) ==  Cyc_String_tok){
_LL3891: _temp3890=(( struct Cyc_String_tok_struct*) _temp3884)->f1; goto
_LL3887;} else{ goto _LL3888;} _LL3888: goto _LL3889; _LL3887: yyzzz= _temp3890;
goto _LL3885; _LL3889:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3885:;}
yyzzz;}), Cyc_Absyn_new_conref(({ void* yyzzz;{ void* _temp3892= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp3898; _LL3894:
if(*(( void**) _temp3892) ==  Cyc_Kind_tok){ _LL3899: _temp3898=( void*)((
struct Cyc_Kind_tok_struct*) _temp3892)->f1; goto _LL3895;} else{ goto _LL3896;}
_LL3896: goto _LL3897; _LL3895: yyzzz= _temp3898; goto _LL3893; _LL3897:( int)
_throw(( void*)& Cyc_yyfail_Kind_tok); _LL3893:;} yyzzz;}))); _temp3883;});
_temp3882;}); break; case 159: _LL3881: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp3901=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp3901[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp3902; _temp3902.tag= Cyc_TypeOpt_tok;
_temp3902.f1= 0; _temp3902;}); _temp3901;}); break; case 160: _LL3900: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp3904=( struct Cyc_TypeOpt_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp3904[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp3905; _temp3905.tag= Cyc_TypeOpt_tok; _temp3905.f1=({ struct Cyc_Core_Opt*
_temp3906=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3906->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3907=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3907[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3908; _temp3908.tag= Cyc_Absyn_JoinEff;
_temp3908.f1=({ struct Cyc_List_List* yyzzz;{ void* _temp3909= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3915; _LL3911: if(*(( void**) _temp3909) ==  Cyc_TypeList_tok){ _LL3916:
_temp3915=(( struct Cyc_TypeList_tok_struct*) _temp3909)->f1; goto _LL3912;}
else{ goto _LL3913;} _LL3913: goto _LL3914; _LL3912: yyzzz= _temp3915; goto
_LL3910; _LL3914:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3910:;}
yyzzz;}); _temp3908;}); _temp3907;})); _temp3906;}); _temp3905;}); _temp3904;});
break; case 161: _LL3903: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp3918=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp3918[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp3919; _temp3919.tag= Cyc_Rgnorder_tok;
_temp3919.f1= 0; _temp3919;}); _temp3918;}); break; case 162: _LL3917: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL3920: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp3922=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp3922[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp3923; _temp3923.tag= Cyc_Rgnorder_tok; _temp3923.f1=({ struct Cyc_List_List*
_temp3924=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3924->hd=( void*)({ struct _tuple18* _temp3925=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp3925->f1= Cyc_Parse_id2type(({ struct
_tagged_arr yyzzz;{ void* _temp3934= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tagged_arr _temp3940; _LL3936: if(*(( void**)
_temp3934) ==  Cyc_String_tok){ _LL3941: _temp3940=(( struct Cyc_String_tok_struct*)
_temp3934)->f1; goto _LL3937;} else{ goto _LL3938;} _LL3938: goto _LL3939;
_LL3937: yyzzz= _temp3940; goto _LL3935; _LL3939:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL3935:;} yyzzz;}), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind));
_temp3925->f2= Cyc_Parse_id2type(({ struct _tagged_arr yyzzz;{ void* _temp3926=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp3932; _LL3928: if(*(( void**) _temp3926) ==  Cyc_String_tok){ _LL3933:
_temp3932=(( struct Cyc_String_tok_struct*) _temp3926)->f1; goto _LL3929;} else{
goto _LL3930;} _LL3930: goto _LL3931; _LL3929: yyzzz= _temp3932; goto _LL3927;
_LL3931:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3927:;} yyzzz;}), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp3925;}); _temp3924->tl= 0; _temp3924;});
_temp3923;}); _temp3922;}); break; case 164: _LL3921: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp3943=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp3943[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp3944; _temp3944.tag= Cyc_Rgnorder_tok; _temp3944.f1=({ struct Cyc_List_List*
_temp3945=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3945->hd=( void*)({ struct _tuple18* _temp3954=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp3954->f1= Cyc_Parse_id2type(({ struct
_tagged_arr yyzzz;{ void* _temp3963= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tagged_arr _temp3969; _LL3965: if(*(( void**)
_temp3963) ==  Cyc_String_tok){ _LL3970: _temp3969=(( struct Cyc_String_tok_struct*)
_temp3963)->f1; goto _LL3966;} else{ goto _LL3967;} _LL3967: goto _LL3968;
_LL3966: yyzzz= _temp3969; goto _LL3964; _LL3968:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL3964:;} yyzzz;}), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind));
_temp3954->f2= Cyc_Parse_id2type(({ struct _tagged_arr yyzzz;{ void* _temp3955=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct
_tagged_arr _temp3961; _LL3957: if(*(( void**) _temp3955) ==  Cyc_String_tok){
_LL3962: _temp3961=(( struct Cyc_String_tok_struct*) _temp3955)->f1; goto
_LL3958;} else{ goto _LL3959;} _LL3959: goto _LL3960; _LL3958: yyzzz= _temp3961;
goto _LL3956; _LL3960:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3956:;}
yyzzz;}), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind)); _temp3954;});
_temp3945->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp3946= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp3952; _LL3948: if(*(( void**) _temp3946) ==  Cyc_Rgnorder_tok){ _LL3953:
_temp3952=(( struct Cyc_Rgnorder_tok_struct*) _temp3946)->f1; goto _LL3949;}
else{ goto _LL3950;} _LL3950: goto _LL3951; _LL3949: yyzzz= _temp3952; goto
_LL3947; _LL3951:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL3947:;}
yyzzz;}); _temp3945;}); _temp3944;}); _temp3943;}); break; case 165: _LL3942:
yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3972=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp3972[ 0]=({ struct
Cyc_Bool_tok_struct _temp3973; _temp3973.tag= Cyc_Bool_tok; _temp3973.f1= 0;
_temp3973;}); _temp3972;}); break; case 166: _LL3971: { struct _tagged_arr
_temp3975=({ struct _tagged_arr yyzzz;{ void* _temp3978= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp3984; _LL3980: if(*(( void**) _temp3978) ==  Cyc_String_tok){ _LL3985:
_temp3984=(( struct Cyc_String_tok_struct*) _temp3978)->f1; goto _LL3981;} else{
goto _LL3982;} _LL3982: goto _LL3983; _LL3981: yyzzz= _temp3984; goto _LL3979;
_LL3983:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL3979:;} yyzzz;}); if(
Cyc_std_zstrcmp( _temp3975, _tag_arr("inject", sizeof( unsigned char), 7u)) != 
0){ Cyc_Parse_err( _tag_arr("missing type in function declaration", sizeof(
unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Bool_tok_struct* _temp3976=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp3976[ 0]=({ struct
Cyc_Bool_tok_struct _temp3977; _temp3977.tag= Cyc_Bool_tok; _temp3977.f1= 1;
_temp3977;}); _temp3976;}); break;} case 167: _LL3974: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168: _LL3986:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3988=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3988[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3989; _temp3989.tag= Cyc_TypeList_tok; _temp3989.f1= Cyc_List_imp_append(({
struct Cyc_List_List* yyzzz;{ void* _temp3990= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp3996; _LL3992: if(*(( void**) _temp3990) ==  Cyc_TypeList_tok){ _LL3997:
_temp3996=(( struct Cyc_TypeList_tok_struct*) _temp3990)->f1; goto _LL3993;}
else{ goto _LL3994;} _LL3994: goto _LL3995; _LL3993: yyzzz= _temp3996; goto
_LL3991; _LL3995:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3991:;}
yyzzz;}),({ struct Cyc_List_List* yyzzz;{ void* _temp3998= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp4004; _LL4000: if(*(( void**) _temp3998) ==  Cyc_TypeList_tok){ _LL4005:
_temp4004=(( struct Cyc_TypeList_tok_struct*) _temp3998)->f1; goto _LL4001;}
else{ goto _LL4002;} _LL4002: goto _LL4003; _LL4001: yyzzz= _temp4004; goto
_LL3999; _LL4003:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL3999:;}
yyzzz;})); _temp3989;}); _temp3988;}); break; case 169: _LL3987: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp4007=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4007[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4008; _temp4008.tag= Cyc_TypeList_tok; _temp4008.f1= 0; _temp4008;});
_temp4007;}); break; case 170: _LL4006: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 171:
_LL4009: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4011=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4011[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4012; _temp4012.tag= Cyc_TypeList_tok; _temp4012.f1=({ struct Cyc_List_List*
_temp4013=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4013->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp4014=(
struct Cyc_Absyn_RgnsEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp4014[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp4015; _temp4015.tag= Cyc_Absyn_RgnsEff;
_temp4015.f1=( void*)({ void* yyzzz;{ void* _temp4016= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp4022;
_LL4018: if(*(( void**) _temp4016) ==  Cyc_Type_tok){ _LL4023: _temp4022=( void*)((
struct Cyc_Type_tok_struct*) _temp4016)->f1; goto _LL4019;} else{ goto _LL4020;}
_LL4020: goto _LL4021; _LL4019: yyzzz= _temp4022; goto _LL4017; _LL4021:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4017:;} yyzzz;}); _temp4015;});
_temp4014;})); _temp4013->tl= 0; _temp4013;}); _temp4012;}); _temp4011;});
break; case 172: _LL4010: Cyc_Parse_set_vartyp_kind(({ void* yyzzz;{ void*
_temp4025= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; void*
_temp4031; _LL4027: if(*(( void**) _temp4025) ==  Cyc_Type_tok){ _LL4032:
_temp4031=( void*)(( struct Cyc_Type_tok_struct*) _temp4025)->f1; goto _LL4028;}
else{ goto _LL4029;} _LL4029: goto _LL4030; _LL4028: yyzzz= _temp4031; goto
_LL4026; _LL4030:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4026:;} yyzzz;}),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4033=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4033[ 0]=({ struct Cyc_TypeList_tok_struct _temp4034; _temp4034.tag= Cyc_TypeList_tok;
_temp4034.f1=({ struct Cyc_List_List* _temp4035=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4035->hd=( void*)({ void* yyzzz;{
void* _temp4036= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp4042; _LL4038: if(*(( void**) _temp4036) ==  Cyc_Type_tok){ _LL4043:
_temp4042=( void*)(( struct Cyc_Type_tok_struct*) _temp4036)->f1; goto _LL4039;}
else{ goto _LL4040;} _LL4040: goto _LL4041; _LL4039: yyzzz= _temp4042; goto
_LL4037; _LL4041:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4037:;} yyzzz;});
_temp4035->tl= 0; _temp4035;}); _temp4034;}); _temp4033;}); break; case 173:
_LL4024: if( ! Cyc_Parse_set_vartyp_kind(({ void* yyzzz;{ void* _temp4045= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp4051; _LL4047:
if(*(( void**) _temp4045) ==  Cyc_Type_tok){ _LL4052: _temp4051=( void*)((
struct Cyc_Type_tok_struct*) _temp4045)->f1; goto _LL4048;} else{ goto _LL4049;}
_LL4049: goto _LL4050; _LL4048: yyzzz= _temp4051; goto _LL4046; _LL4050:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4046:;} yyzzz;}),( void*) Cyc_Absyn_RgnKind)){
Cyc_Parse_err( _tag_arr("expecting region kind", sizeof( unsigned char), 22u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4053=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4053[ 0]=({ struct Cyc_TypeList_tok_struct _temp4054; _temp4054.tag= Cyc_TypeList_tok;
_temp4054.f1=({ struct Cyc_List_List* _temp4055=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4055->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4056=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4056[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4057; _temp4057.tag= Cyc_Absyn_AccessEff;
_temp4057.f1=( void*)({ void* yyzzz;{ void* _temp4058= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp4064; _LL4060:
if(*(( void**) _temp4058) ==  Cyc_Type_tok){ _LL4065: _temp4064=( void*)((
struct Cyc_Type_tok_struct*) _temp4058)->f1; goto _LL4061;} else{ goto _LL4062;}
_LL4062: goto _LL4063; _LL4061: yyzzz= _temp4064; goto _LL4059; _LL4063:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4059:;} yyzzz;}); _temp4057;});
_temp4056;})); _temp4055->tl= 0; _temp4055;}); _temp4054;}); _temp4053;});
break; case 174: _LL4044: if( ! Cyc_Parse_set_vartyp_kind(({ void* yyzzz;{ void*
_temp4067= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
void* _temp4073; _LL4069: if(*(( void**) _temp4067) ==  Cyc_Type_tok){ _LL4074:
_temp4073=( void*)(( struct Cyc_Type_tok_struct*) _temp4067)->f1; goto _LL4070;}
else{ goto _LL4071;} _LL4071: goto _LL4072; _LL4070: yyzzz= _temp4073; goto
_LL4068; _LL4072:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4068:;} yyzzz;}),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp4075=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4075[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4076; _temp4076.tag= Cyc_TypeList_tok; _temp4076.f1=({ struct Cyc_List_List*
_temp4077=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4077->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4086=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4086[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4087; _temp4087.tag= Cyc_Absyn_AccessEff;
_temp4087.f1=( void*)({ void* yyzzz;{ void* _temp4088= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void* _temp4094;
_LL4090: if(*(( void**) _temp4088) ==  Cyc_Type_tok){ _LL4095: _temp4094=( void*)((
struct Cyc_Type_tok_struct*) _temp4088)->f1; goto _LL4091;} else{ goto _LL4092;}
_LL4092: goto _LL4093; _LL4091: yyzzz= _temp4094; goto _LL4089; _LL4093:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4089:;} yyzzz;}); _temp4087;});
_temp4086;})); _temp4077->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4078=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp4084; _LL4080: if(*(( void**) _temp4078) ==  Cyc_TypeList_tok){ _LL4085:
_temp4084=(( struct Cyc_TypeList_tok_struct*) _temp4078)->f1; goto _LL4081;}
else{ goto _LL4082;} _LL4082: goto _LL4083; _LL4081: yyzzz= _temp4084; goto
_LL4079; _LL4083:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4079:;}
yyzzz;}); _temp4077;}); _temp4076;}); _temp4075;}); break; case 175: _LL4066:
yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp4097=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4097[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4098; _temp4098.tag= Cyc_ParamDeclList_tok;
_temp4098.f1=({ struct Cyc_List_List* _temp4099=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4099->hd=( void*)({ struct
_tuple2* yyzzz;{ void* _temp4100= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple2* _temp4106; _LL4102: if(*(( void**) _temp4100) == 
Cyc_ParamDecl_tok){ _LL4107: _temp4106=(( struct Cyc_ParamDecl_tok_struct*)
_temp4100)->f1; goto _LL4103;} else{ goto _LL4104;} _LL4104: goto _LL4105;
_LL4103: yyzzz= _temp4106; goto _LL4101; _LL4105:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL4101:;} yyzzz;}); _temp4099->tl= 0; _temp4099;}); _temp4098;}); _temp4097;});
break; case 176: _LL4096: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct*
_temp4109=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4109[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4110; _temp4110.tag=
Cyc_ParamDeclList_tok; _temp4110.f1=({ struct Cyc_List_List* _temp4111=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4111->hd=( void*)({
struct _tuple2* yyzzz;{ void* _temp4120= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tuple2* _temp4126; _LL4122: if(*(( void**)
_temp4120) ==  Cyc_ParamDecl_tok){ _LL4127: _temp4126=(( struct Cyc_ParamDecl_tok_struct*)
_temp4120)->f1; goto _LL4123;} else{ goto _LL4124;} _LL4124: goto _LL4125;
_LL4123: yyzzz= _temp4126; goto _LL4121; _LL4125:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL4121:;} yyzzz;}); _temp4111->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp4112= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp4118; _LL4114: if(*(( void**) _temp4112) ==  Cyc_ParamDeclList_tok){
_LL4119: _temp4118=(( struct Cyc_ParamDeclList_tok_struct*) _temp4112)->f1; goto
_LL4115;} else{ goto _LL4116;} _LL4116: goto _LL4117; _LL4115: yyzzz= _temp4118;
goto _LL4113; _LL4117:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL4113:;} yyzzz;}); _temp4111;}); _temp4110;}); _temp4109;}); break; case 177:
_LL4108: { struct _tuple17 _temp4139; struct Cyc_List_List* _temp4140; struct
Cyc_List_List* _temp4142; struct Cyc_Absyn_Tqual _temp4144; struct _tuple17*
_temp4137=({ struct _tuple17* yyzzz;{ void* _temp4129= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple17*
_temp4135; _LL4131: if(*(( void**) _temp4129) ==  Cyc_QualSpecList_tok){ _LL4136:
_temp4135=(( struct Cyc_QualSpecList_tok_struct*) _temp4129)->f1; goto _LL4132;}
else{ goto _LL4133;} _LL4133: goto _LL4134; _LL4132: yyzzz= _temp4135; goto
_LL4130; _LL4134:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4130:;}
yyzzz;}); _temp4139=* _temp4137; _LL4145: _temp4144= _temp4139.f1; goto _LL4143;
_LL4143: _temp4142= _temp4139.f2; goto _LL4141; _LL4141: _temp4140= _temp4139.f3;
goto _LL4138; _LL4138: { struct Cyc_Parse_Declarator _temp4156; struct Cyc_List_List*
_temp4157; struct _tuple1* _temp4159; struct Cyc_Parse_Declarator* _temp4154=({
struct Cyc_Parse_Declarator* yyzzz;{ void* _temp4146= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Declarator*
_temp4152; _LL4148: if(*(( void**) _temp4146) ==  Cyc_Declarator_tok){ _LL4153:
_temp4152=(( struct Cyc_Declarator_tok_struct*) _temp4146)->f1; goto _LL4149;}
else{ goto _LL4150;} _LL4150: goto _LL4151; _LL4149: yyzzz= _temp4152; goto
_LL4147; _LL4151:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL4147:;}
yyzzz;}); _temp4156=* _temp4154; _LL4160: _temp4159= _temp4156.id; goto _LL4158;
_LL4158: _temp4157= _temp4156.tms; goto _LL4155; _LL4155: { void* _temp4161= Cyc_Parse_speclist2typ(
_temp4142, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)); struct Cyc_List_List* _temp4164;
struct Cyc_List_List* _temp4166; void* _temp4168; struct Cyc_Absyn_Tqual
_temp4170; struct _tuple6 _temp4162= Cyc_Parse_apply_tms( _temp4144, _temp4161,
_temp4140, _temp4157); _LL4171: _temp4170= _temp4162.f1; goto _LL4169; _LL4169:
_temp4168= _temp4162.f2; goto _LL4167; _LL4167: _temp4166= _temp4162.f3; goto
_LL4165; _LL4165: _temp4164= _temp4162.f4; goto _LL4163; _LL4163: if( _temp4166
!=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp4159)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp4172=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp4176=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4176->v=( void*)(* _temp4159).f2; _temp4176;}); if( _temp4164 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp4173=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4173[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4174; _temp4174.tag= Cyc_ParamDecl_tok;
_temp4174.f1=({ struct _tuple2* _temp4175=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4175->f1= _temp4172; _temp4175->f2= _temp4170; _temp4175->f3=
_temp4168; _temp4175;}); _temp4174;}); _temp4173;}); break;}}}} case 178:
_LL4128: { struct _tuple17 _temp4188; struct Cyc_List_List* _temp4189; struct
Cyc_List_List* _temp4191; struct Cyc_Absyn_Tqual _temp4193; struct _tuple17*
_temp4186=({ struct _tuple17* yyzzz;{ void* _temp4178= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp4184; _LL4180: if(*(( void**) _temp4178) ==  Cyc_QualSpecList_tok){ _LL4185:
_temp4184=(( struct Cyc_QualSpecList_tok_struct*) _temp4178)->f1; goto _LL4181;}
else{ goto _LL4182;} _LL4182: goto _LL4183; _LL4181: yyzzz= _temp4184; goto
_LL4179; _LL4183:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4179:;}
yyzzz;}); _temp4188=* _temp4186; _LL4194: _temp4193= _temp4188.f1; goto _LL4192;
_LL4192: _temp4191= _temp4188.f2; goto _LL4190; _LL4190: _temp4189= _temp4188.f3;
goto _LL4187; _LL4187: { void* _temp4195= Cyc_Parse_speclist2typ( _temp4191, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); if(
_temp4189 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp4196=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4196[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4197; _temp4197.tag= Cyc_ParamDecl_tok; _temp4197.f1=({
struct _tuple2* _temp4198=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4198->f1= 0; _temp4198->f2= _temp4193; _temp4198->f3= _temp4195; _temp4198;});
_temp4197;}); _temp4196;}); break;}} case 179: _LL4177: { struct _tuple17
_temp4210; struct Cyc_List_List* _temp4211; struct Cyc_List_List* _temp4213;
struct Cyc_Absyn_Tqual _temp4215; struct _tuple17* _temp4208=({ struct _tuple17*
yyzzz;{ void* _temp4200= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tuple17* _temp4206; _LL4202: if(*(( void**)
_temp4200) ==  Cyc_QualSpecList_tok){ _LL4207: _temp4206=(( struct Cyc_QualSpecList_tok_struct*)
_temp4200)->f1; goto _LL4203;} else{ goto _LL4204;} _LL4204: goto _LL4205;
_LL4203: yyzzz= _temp4206; goto _LL4201; _LL4205:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4201:;} yyzzz;}); _temp4210=* _temp4208; _LL4216: _temp4215= _temp4210.f1;
goto _LL4214; _LL4214: _temp4213= _temp4210.f2; goto _LL4212; _LL4212: _temp4211=
_temp4210.f3; goto _LL4209; _LL4209: { void* _temp4217= Cyc_Parse_speclist2typ(
_temp4213, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)); struct Cyc_List_List* _temp4218=({
struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp4232= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Parse_Abstractdeclarator*
_temp4238; _LL4234: if(*(( void**) _temp4232) ==  Cyc_AbstractDeclarator_tok){
_LL4239: _temp4238=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp4232)->f1;
goto _LL4235;} else{ goto _LL4236;} _LL4236: goto _LL4237; _LL4235: yyzzz=
_temp4238; goto _LL4233; _LL4237:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4233:;} yyzzz;})->tms; struct Cyc_List_List* _temp4221; struct Cyc_List_List*
_temp4223; void* _temp4225; struct Cyc_Absyn_Tqual _temp4227; struct _tuple6
_temp4219= Cyc_Parse_apply_tms( _temp4215, _temp4217, _temp4211, _temp4218);
_LL4228: _temp4227= _temp4219.f1; goto _LL4226; _LL4226: _temp4225= _temp4219.f2;
goto _LL4224; _LL4224: _temp4223= _temp4219.f3; goto _LL4222; _LL4222: _temp4221=
_temp4219.f4; goto _LL4220; _LL4220: if( _temp4223 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type parameters on formal argument, ignoring", sizeof(
unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp4221 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp4229=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4229[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4230; _temp4230.tag= Cyc_ParamDecl_tok; _temp4230.f1=({
struct _tuple2* _temp4231=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4231->f1= 0; _temp4231->f2= _temp4227; _temp4231->f3= _temp4225; _temp4231;});
_temp4230;}); _temp4229;}); break;}} case 180: _LL4199: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp4241=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp4241[ 0]=({ struct Cyc_IdList_tok_struct
_temp4242; _temp4242.tag= Cyc_IdList_tok; _temp4242.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{
void* _temp4243= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp4249; _LL4245: if(*(( void**) _temp4243) ==  Cyc_IdList_tok){
_LL4250: _temp4249=(( struct Cyc_IdList_tok_struct*) _temp4243)->f1; goto
_LL4246;} else{ goto _LL4247;} _LL4247: goto _LL4248; _LL4246: yyzzz= _temp4249;
goto _LL4244; _LL4248:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL4244:;}
yyzzz;})); _temp4242;}); _temp4241;}); break; case 181: _LL4240: yyval=( void*)({
struct Cyc_IdList_tok_struct* _temp4252=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4252[ 0]=({ struct Cyc_IdList_tok_struct
_temp4253; _temp4253.tag= Cyc_IdList_tok; _temp4253.f1=({ struct Cyc_List_List*
_temp4254=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4254->hd=( void*)({ struct _tagged_arr* _temp4255=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp4255[ 0]=({ struct _tagged_arr
yyzzz;{ void* _temp4256= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tagged_arr _temp4262; _LL4258: if(*(( void**) _temp4256)
==  Cyc_String_tok){ _LL4263: _temp4262=(( struct Cyc_String_tok_struct*)
_temp4256)->f1; goto _LL4259;} else{ goto _LL4260;} _LL4260: goto _LL4261;
_LL4259: yyzzz= _temp4262; goto _LL4257; _LL4261:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL4257:;} yyzzz;}); _temp4255;}); _temp4254->tl= 0; _temp4254;}); _temp4253;});
_temp4252;}); break; case 182: _LL4251: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp4265=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4265[ 0]=({ struct Cyc_IdList_tok_struct _temp4266; _temp4266.tag= Cyc_IdList_tok;
_temp4266.f1=({ struct Cyc_List_List* _temp4267=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4267->hd=( void*)({ struct
_tagged_arr* _temp4276=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp4276[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp4277= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp4283; _LL4279: if(*(( void**) _temp4277) ==  Cyc_String_tok){ _LL4284:
_temp4283=(( struct Cyc_String_tok_struct*) _temp4277)->f1; goto _LL4280;} else{
goto _LL4281;} _LL4281: goto _LL4282; _LL4280: yyzzz= _temp4283; goto _LL4278;
_LL4282:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4278:;} yyzzz;});
_temp4276;}); _temp4267->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4268=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp4274; _LL4270: if(*(( void**) _temp4268) ==  Cyc_IdList_tok){ _LL4275:
_temp4274=(( struct Cyc_IdList_tok_struct*) _temp4268)->f1; goto _LL4271;} else{
goto _LL4272;} _LL4272: goto _LL4273; _LL4271: yyzzz= _temp4274; goto _LL4269;
_LL4273:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL4269:;} yyzzz;});
_temp4267;}); _temp4266;}); _temp4265;}); break; case 183: _LL4264: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 184: _LL4285:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
185: _LL4286: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4288=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4288[ 0]=({ struct Cyc_Exp_tok_struct
_temp4289; _temp4289.tag= Cyc_Exp_tok; _temp4289.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4290=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4290[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4291; _temp4291.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4291.f1= 0; _temp4291.f2= 0; _temp4291;}); _temp4290;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp4289;}); _temp4288;}); break; case 186: _LL4287: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4293=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4293[ 0]=({ struct Cyc_Exp_tok_struct
_temp4294; _temp4294.tag= Cyc_Exp_tok; _temp4294.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4295=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4295[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4296; _temp4296.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4296.f1= 0; _temp4296.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp4297= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4303; _LL4299: if(*(( void**) _temp4297) ==  Cyc_InitializerList_tok){
_LL4304: _temp4303=(( struct Cyc_InitializerList_tok_struct*) _temp4297)->f1;
goto _LL4300;} else{ goto _LL4301;} _LL4301: goto _LL4302; _LL4300: yyzzz=
_temp4303; goto _LL4298; _LL4302:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4298:;} yyzzz;})); _temp4296;}); _temp4295;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp4294;}); _temp4293;}); break; case 187: _LL4292: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4306=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4306[ 0]=({ struct Cyc_Exp_tok_struct
_temp4307; _temp4307.tag= Cyc_Exp_tok; _temp4307.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4308=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4308[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4309; _temp4309.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4309.f1= 0; _temp4309.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp4310= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp4316; _LL4312: if(*(( void**) _temp4310) ==  Cyc_InitializerList_tok){
_LL4317: _temp4316=(( struct Cyc_InitializerList_tok_struct*) _temp4310)->f1;
goto _LL4313;} else{ goto _LL4314;} _LL4314: goto _LL4315; _LL4313: yyzzz=
_temp4316; goto _LL4311; _LL4315:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4311:;} yyzzz;})); _temp4309;}); _temp4308;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp4307;}); _temp4306;}); break; case 188: _LL4305: { struct Cyc_Absyn_Vardecl*
_temp4319= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4341=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp4341->f1=( void*) Cyc_Absyn_Loc_n;
_temp4341->f2=({ struct _tagged_arr* _temp4342=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp4342[ 0]=({ struct _tagged_arr yyzzz;{ void*
_temp4343= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)];
struct _tagged_arr _temp4349; _LL4345: if(*(( void**) _temp4343) ==  Cyc_String_tok){
_LL4350: _temp4349=(( struct Cyc_String_tok_struct*) _temp4343)->f1; goto
_LL4346;} else{ goto _LL4347;} _LL4347: goto _LL4348; _LL4346: yyzzz= _temp4349;
goto _LL4344; _LL4348:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4344:;}
yyzzz;}); _temp4342;}); _temp4341;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp4319->tq=({ struct Cyc_Absyn_Tqual _temp4320; _temp4320.q_const= 1;
_temp4320.q_volatile= 0; _temp4320.q_restrict= 1; _temp4320;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4321=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4321[ 0]=({ struct Cyc_Exp_tok_struct
_temp4322; _temp4322.tag= Cyc_Exp_tok; _temp4322.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp4323=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4323[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4324; _temp4324.tag= Cyc_Absyn_Comprehension_e;
_temp4324.f1= _temp4319; _temp4324.f2=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4325= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Absyn_Exp* _temp4331; _LL4327: if(*(( void**) _temp4325) ==  Cyc_Exp_tok){
_LL4332: _temp4331=(( struct Cyc_Exp_tok_struct*) _temp4325)->f1; goto _LL4328;}
else{ goto _LL4329;} _LL4329: goto _LL4330; _LL4328: yyzzz= _temp4331; goto
_LL4326; _LL4330:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4326:;} yyzzz;});
_temp4324.f3=({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp4333= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp4339; _LL4335: if(*(( void**) _temp4333) ==  Cyc_Exp_tok){ _LL4340:
_temp4339=(( struct Cyc_Exp_tok_struct*) _temp4333)->f1; goto _LL4336;} else{
goto _LL4337;} _LL4337: goto _LL4338; _LL4336: yyzzz= _temp4339; goto _LL4334;
_LL4338:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4334:;} yyzzz;});
_temp4324;}); _temp4323;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp4322;});
_temp4321;}); break;} case 189: _LL4318: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4352=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4352[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4353; _temp4353.tag= Cyc_InitializerList_tok; _temp4353.f1=({ struct Cyc_List_List*
_temp4354=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4354->hd=( void*)({ struct _tuple19* _temp4355=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp4355->f1= 0; _temp4355->f2=({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp4356= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp* _temp4362; _LL4358: if(*(( void**)
_temp4356) ==  Cyc_Exp_tok){ _LL4363: _temp4362=(( struct Cyc_Exp_tok_struct*)
_temp4356)->f1; goto _LL4359;} else{ goto _LL4360;} _LL4360: goto _LL4361;
_LL4359: yyzzz= _temp4362; goto _LL4357; _LL4361:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL4357:;} yyzzz;}); _temp4355;}); _temp4354->tl= 0; _temp4354;}); _temp4353;});
_temp4352;}); break; case 190: _LL4351: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4365=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4365[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4366; _temp4366.tag= Cyc_InitializerList_tok; _temp4366.f1=({ struct Cyc_List_List*
_temp4367=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4367->hd=( void*)({ struct _tuple19* _temp4368=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp4368->f1=({ struct Cyc_List_List*
yyzzz;{ void* _temp4377= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp4383; _LL4379: if(*(( void**)
_temp4377) ==  Cyc_DesignatorList_tok){ _LL4384: _temp4383=(( struct Cyc_DesignatorList_tok_struct*)
_temp4377)->f1; goto _LL4380;} else{ goto _LL4381;} _LL4381: goto _LL4382;
_LL4380: yyzzz= _temp4383; goto _LL4378; _LL4382:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4378:;} yyzzz;}); _temp4368->f2=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4369= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Exp* _temp4375; _LL4371: if(*(( void**) _temp4369) ==  Cyc_Exp_tok){
_LL4376: _temp4375=(( struct Cyc_Exp_tok_struct*) _temp4369)->f1; goto _LL4372;}
else{ goto _LL4373;} _LL4373: goto _LL4374; _LL4372: yyzzz= _temp4375; goto
_LL4370; _LL4374:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4370:;} yyzzz;});
_temp4368;}); _temp4367->tl= 0; _temp4367;}); _temp4366;}); _temp4365;}); break;
case 191: _LL4364: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4386=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4386[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4387; _temp4387.tag= Cyc_InitializerList_tok; _temp4387.f1=({ struct Cyc_List_List*
_temp4388=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4388->hd=( void*)({ struct _tuple19* _temp4397=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp4397->f1= 0; _temp4397->f2=({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp4398= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp* _temp4404; _LL4400: if(*(( void**)
_temp4398) ==  Cyc_Exp_tok){ _LL4405: _temp4404=(( struct Cyc_Exp_tok_struct*)
_temp4398)->f1; goto _LL4401;} else{ goto _LL4402;} _LL4402: goto _LL4403;
_LL4401: yyzzz= _temp4404; goto _LL4399; _LL4403:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL4399:;} yyzzz;}); _temp4397;}); _temp4388->tl=({ struct Cyc_List_List* yyzzz;{
void* _temp4389= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp4395; _LL4391: if(*(( void**) _temp4389) ==  Cyc_InitializerList_tok){
_LL4396: _temp4395=(( struct Cyc_InitializerList_tok_struct*) _temp4389)->f1;
goto _LL4392;} else{ goto _LL4393;} _LL4393: goto _LL4394; _LL4392: yyzzz=
_temp4395; goto _LL4390; _LL4394:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4390:;} yyzzz;}); _temp4388;}); _temp4387;}); _temp4386;}); break; case 192:
_LL4385: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp4407=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4407[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4408; _temp4408.tag=
Cyc_InitializerList_tok; _temp4408.f1=({ struct Cyc_List_List* _temp4409=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4409->hd=(
void*)({ struct _tuple19* _temp4418=( struct _tuple19*) GC_malloc( sizeof(
struct _tuple19)); _temp4418->f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp4427= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4433; _LL4429: if(*(( void**) _temp4427) ==  Cyc_DesignatorList_tok){
_LL4434: _temp4433=(( struct Cyc_DesignatorList_tok_struct*) _temp4427)->f1;
goto _LL4430;} else{ goto _LL4431;} _LL4431: goto _LL4432; _LL4430: yyzzz=
_temp4433; goto _LL4428; _LL4432:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4428:;} yyzzz;}); _temp4418->f2=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4419= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Exp* _temp4425; _LL4421: if(*(( void**) _temp4419) ==  Cyc_Exp_tok){
_LL4426: _temp4425=(( struct Cyc_Exp_tok_struct*) _temp4419)->f1; goto _LL4422;}
else{ goto _LL4423;} _LL4423: goto _LL4424; _LL4422: yyzzz= _temp4425; goto
_LL4420; _LL4424:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4420:;} yyzzz;});
_temp4418;}); _temp4409->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4410=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp4416; _LL4412: if(*(( void**) _temp4410) ==  Cyc_InitializerList_tok){
_LL4417: _temp4416=(( struct Cyc_InitializerList_tok_struct*) _temp4410)->f1;
goto _LL4413;} else{ goto _LL4414;} _LL4414: goto _LL4415; _LL4413: yyzzz=
_temp4416; goto _LL4411; _LL4415:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL4411:;} yyzzz;}); _temp4409;}); _temp4408;}); _temp4407;}); break; case 193:
_LL4406: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp4436=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4436[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4437; _temp4437.tag=
Cyc_DesignatorList_tok; _temp4437.f1= Cyc_List_imp_rev(({ struct Cyc_List_List*
yyzzz;{ void* _temp4438= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp4444; _LL4440: if(*(( void**)
_temp4438) ==  Cyc_DesignatorList_tok){ _LL4445: _temp4444=(( struct Cyc_DesignatorList_tok_struct*)
_temp4438)->f1; goto _LL4441;} else{ goto _LL4442;} _LL4442: goto _LL4443;
_LL4441: yyzzz= _temp4444; goto _LL4439; _LL4443:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4439:;} yyzzz;})); _temp4437;}); _temp4436;}); break; case 194: _LL4435:
yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp4447=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4447[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4448; _temp4448.tag= Cyc_DesignatorList_tok;
_temp4448.f1=({ struct Cyc_List_List* _temp4449=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4449->hd=( void*)({ void* yyzzz;{
void* _temp4450= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp4456; _LL4452: if(*(( void**) _temp4450) ==  Cyc_Designator_tok){
_LL4457: _temp4456=( void*)(( struct Cyc_Designator_tok_struct*) _temp4450)->f1;
goto _LL4453;} else{ goto _LL4454;} _LL4454: goto _LL4455; _LL4453: yyzzz=
_temp4456; goto _LL4451; _LL4455:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL4451:;} yyzzz;}); _temp4449->tl= 0; _temp4449;}); _temp4448;}); _temp4447;});
break; case 195: _LL4446: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp4459=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4459[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4460; _temp4460.tag=
Cyc_DesignatorList_tok; _temp4460.f1=({ struct Cyc_List_List* _temp4461=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4461->hd=( void*)({
void* yyzzz;{ void* _temp4470= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; void* _temp4476; _LL4472: if(*(( void**) _temp4470) ==  Cyc_Designator_tok){
_LL4477: _temp4476=( void*)(( struct Cyc_Designator_tok_struct*) _temp4470)->f1;
goto _LL4473;} else{ goto _LL4474;} _LL4474: goto _LL4475; _LL4473: yyzzz=
_temp4476; goto _LL4471; _LL4475:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL4471:;} yyzzz;}); _temp4461->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp4462= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4468; _LL4464: if(*(( void**) _temp4462) ==  Cyc_DesignatorList_tok){
_LL4469: _temp4468=(( struct Cyc_DesignatorList_tok_struct*) _temp4462)->f1;
goto _LL4465;} else{ goto _LL4466;} _LL4466: goto _LL4467; _LL4465: yyzzz=
_temp4468; goto _LL4463; _LL4467:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL4463:;} yyzzz;}); _temp4461;}); _temp4460;}); _temp4459;}); break; case 196:
_LL4458: yyval=( void*)({ struct Cyc_Designator_tok_struct* _temp4479=( struct
Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4479[ 0]=({ struct Cyc_Designator_tok_struct _temp4480; _temp4480.tag= Cyc_Designator_tok;
_temp4480.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4481=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4481[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4482; _temp4482.tag=
Cyc_Absyn_ArrayElement; _temp4482.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp4483= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Exp* _temp4489; _LL4485: if(*(( void**) _temp4483) ==  Cyc_Exp_tok){
_LL4490: _temp4489=(( struct Cyc_Exp_tok_struct*) _temp4483)->f1; goto _LL4486;}
else{ goto _LL4487;} _LL4487: goto _LL4488; _LL4486: yyzzz= _temp4489; goto
_LL4484; _LL4488:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4484:;} yyzzz;});
_temp4482;}); _temp4481;})); _temp4480;}); _temp4479;}); break; case 197:
_LL4478: yyval=( void*)({ struct Cyc_Designator_tok_struct* _temp4492=( struct
Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4492[ 0]=({ struct Cyc_Designator_tok_struct _temp4493; _temp4493.tag= Cyc_Designator_tok;
_temp4493.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4494=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4494[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4495; _temp4495.tag= Cyc_Absyn_FieldName;
_temp4495.f1=({ struct _tagged_arr* _temp4496=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp4496[ 0]=({ struct _tagged_arr yyzzz;{ void*
_temp4497= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
_tagged_arr _temp4503; _LL4499: if(*(( void**) _temp4497) ==  Cyc_String_tok){
_LL4504: _temp4503=(( struct Cyc_String_tok_struct*) _temp4497)->f1; goto
_LL4500;} else{ goto _LL4501;} _LL4501: goto _LL4502; _LL4500: yyzzz= _temp4503;
goto _LL4498; _LL4502:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4498:;}
yyzzz;}); _temp4496;}); _temp4495;}); _temp4494;})); _temp4493;}); _temp4492;});
break; case 198: _LL4491: { void* _temp4506= Cyc_Parse_speclist2typ((*({ struct
_tuple17* yyzzz;{ void* _temp4528= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple17* _temp4534; _LL4530: if(*(( void**) _temp4528)
==  Cyc_QualSpecList_tok){ _LL4535: _temp4534=(( struct Cyc_QualSpecList_tok_struct*)
_temp4528)->f1; goto _LL4531;} else{ goto _LL4532;} _LL4532: goto _LL4533;
_LL4531: yyzzz= _temp4534; goto _LL4529; _LL4533:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4529:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp4507=(*({ struct _tuple17* yyzzz;{ void* _temp4520= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp4526; _LL4522: if(*(( void**) _temp4520) ==  Cyc_QualSpecList_tok){ _LL4527:
_temp4526=(( struct Cyc_QualSpecList_tok_struct*) _temp4520)->f1; goto _LL4523;}
else{ goto _LL4524;} _LL4524: goto _LL4525; _LL4523: yyzzz= _temp4526; goto
_LL4521; _LL4525:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4521:;}
yyzzz;})).f3; if( _temp4507 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp4508=(*({ struct _tuple17* yyzzz;{ void* _temp4512= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple17*
_temp4518; _LL4514: if(*(( void**) _temp4512) ==  Cyc_QualSpecList_tok){ _LL4519:
_temp4518=(( struct Cyc_QualSpecList_tok_struct*) _temp4512)->f1; goto _LL4515;}
else{ goto _LL4516;} _LL4516: goto _LL4517; _LL4515: yyzzz= _temp4518; goto
_LL4513; _LL4517:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4513:;}
yyzzz;})).f1; yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4509=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4509[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4510; _temp4510.tag= Cyc_ParamDecl_tok;
_temp4510.f1=({ struct _tuple2* _temp4511=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4511->f1= 0; _temp4511->f2= _temp4508; _temp4511->f3=
_temp4506; _temp4511;}); _temp4510;}); _temp4509;}); break;}} case 199: _LL4505: {
void* _temp4537= Cyc_Parse_speclist2typ((*({ struct _tuple17* yyzzz;{ void*
_temp4569= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct _tuple17* _temp4575; _LL4571: if(*(( void**) _temp4569) ==  Cyc_QualSpecList_tok){
_LL4576: _temp4575=(( struct Cyc_QualSpecList_tok_struct*) _temp4569)->f1; goto
_LL4572;} else{ goto _LL4573;} _LL4573: goto _LL4574; _LL4572: yyzzz= _temp4575;
goto _LL4570; _LL4574:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4570:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp4538=(*({ struct _tuple17* yyzzz;{ void* _temp4561= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple17*
_temp4567; _LL4563: if(*(( void**) _temp4561) ==  Cyc_QualSpecList_tok){ _LL4568:
_temp4567=(( struct Cyc_QualSpecList_tok_struct*) _temp4561)->f1; goto _LL4564;}
else{ goto _LL4565;} _LL4565: goto _LL4566; _LL4564: yyzzz= _temp4567; goto
_LL4562; _LL4566:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL4562:;}
yyzzz;})).f3; struct Cyc_Absyn_Tqual _temp4539=(*({ struct _tuple17* yyzzz;{
void* _temp4553= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct _tuple17* _temp4559; _LL4555: if(*(( void**) _temp4553) ==  Cyc_QualSpecList_tok){
_LL4560: _temp4559=(( struct Cyc_QualSpecList_tok_struct*) _temp4553)->f1; goto
_LL4556;} else{ goto _LL4557;} _LL4557: goto _LL4558; _LL4556: yyzzz= _temp4559;
goto _LL4554; _LL4558:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL4554:;} yyzzz;})).f1; struct Cyc_List_List* _temp4540=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4545= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Parse_Abstractdeclarator* _temp4551; _LL4547: if(*((
void**) _temp4545) ==  Cyc_AbstractDeclarator_tok){ _LL4552: _temp4551=(( struct
Cyc_AbstractDeclarator_tok_struct*) _temp4545)->f1; goto _LL4548;} else{ goto
_LL4549;} _LL4549: goto _LL4550; _LL4548: yyzzz= _temp4551; goto _LL4546;
_LL4550:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4546:;}
yyzzz;})->tms; struct _tuple6 _temp4541= Cyc_Parse_apply_tms( _temp4539,
_temp4537, _temp4538, _temp4540); if( _temp4541.f3 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp4541.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp4542=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4542[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4543; _temp4543.tag= Cyc_ParamDecl_tok; _temp4543.f1=({
struct _tuple2* _temp4544=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4544->f1= 0; _temp4544->f2= _temp4541.f1; _temp4544->f3= _temp4541.f2;
_temp4544;}); _temp4543;}); _temp4542;}); break;} case 200: _LL4536: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp4578=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp4578[ 0]=({ struct Cyc_Type_tok_struct
_temp4579; _temp4579.tag= Cyc_Type_tok; _temp4579.f1=( void*)(*({ struct _tuple2*
yyzzz;{ void* _temp4580= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple2* _temp4586; _LL4582: if(*(( void**) _temp4580) == 
Cyc_ParamDecl_tok){ _LL4587: _temp4586=(( struct Cyc_ParamDecl_tok_struct*)
_temp4580)->f1; goto _LL4583;} else{ goto _LL4584;} _LL4584: goto _LL4585;
_LL4583: yyzzz= _temp4586; goto _LL4581; _LL4585:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL4581:;} yyzzz;})).f3; _temp4579;}); _temp4578;}); break; case 201: _LL4577:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4589=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp4589[ 0]=({ struct Cyc_Type_tok_struct
_temp4590; _temp4590.tag= Cyc_Type_tok; _temp4590.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4591=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4591[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4592; _temp4592.tag= Cyc_Absyn_JoinEff; _temp4592.f1=
0; _temp4592;}); _temp4591;})); _temp4590;}); _temp4589;}); break; case 202:
_LL4588: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4594=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp4594[ 0]=({ struct Cyc_Type_tok_struct
_temp4595; _temp4595.tag= Cyc_Type_tok; _temp4595.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4596=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4596[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4597; _temp4597.tag= Cyc_Absyn_JoinEff; _temp4597.f1=({
struct Cyc_List_List* yyzzz;{ void* _temp4598= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4604; _LL4600: if(*(( void**) _temp4598) ==  Cyc_TypeList_tok){ _LL4605:
_temp4604=(( struct Cyc_TypeList_tok_struct*) _temp4598)->f1; goto _LL4601;}
else{ goto _LL4602;} _LL4602: goto _LL4603; _LL4601: yyzzz= _temp4604; goto
_LL4599; _LL4603:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4599:;}
yyzzz;}); _temp4597;}); _temp4596;})); _temp4595;}); _temp4594;}); break; case
203: _LL4593: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4607=( struct
Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp4607[
0]=({ struct Cyc_Type_tok_struct _temp4608; _temp4608.tag= Cyc_Type_tok;
_temp4608.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp4609=(
struct Cyc_Absyn_RgnsEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp4609[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp4610; _temp4610.tag= Cyc_Absyn_RgnsEff;
_temp4610.f1=( void*)({ void* yyzzz;{ void* _temp4611= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; void* _temp4617;
_LL4613: if(*(( void**) _temp4611) ==  Cyc_Type_tok){ _LL4618: _temp4617=( void*)((
struct Cyc_Type_tok_struct*) _temp4611)->f1; goto _LL4614;} else{ goto _LL4615;}
_LL4615: goto _LL4616; _LL4614: yyzzz= _temp4617; goto _LL4612; _LL4616:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4612:;} yyzzz;}); _temp4610;});
_temp4609;})); _temp4608;}); _temp4607;}); break; case 204: _LL4606: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp4620=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp4620[ 0]=({ struct Cyc_Type_tok_struct
_temp4621; _temp4621.tag= Cyc_Type_tok; _temp4621.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4622=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4622[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4623; _temp4623.tag= Cyc_Absyn_JoinEff; _temp4623.f1=({
struct Cyc_List_List* _temp4624=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4624->hd=( void*)({ void* yyzzz;{ void* _temp4633=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; void*
_temp4639; _LL4635: if(*(( void**) _temp4633) ==  Cyc_Type_tok){ _LL4640:
_temp4639=( void*)(( struct Cyc_Type_tok_struct*) _temp4633)->f1; goto _LL4636;}
else{ goto _LL4637;} _LL4637: goto _LL4638; _LL4636: yyzzz= _temp4639; goto
_LL4634; _LL4638:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4634:;} yyzzz;});
_temp4624->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp4625= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp4631; _LL4627: if(*(( void**) _temp4625) ==  Cyc_TypeList_tok){ _LL4632:
_temp4631=(( struct Cyc_TypeList_tok_struct*) _temp4625)->f1; goto _LL4628;}
else{ goto _LL4629;} _LL4629: goto _LL4630; _LL4628: yyzzz= _temp4631; goto
_LL4626; _LL4630:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4626:;}
yyzzz;}); _temp4624;}); _temp4623;}); _temp4622;})); _temp4621;}); _temp4620;});
break; case 205: _LL4619: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4642=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4642[ 0]=({ struct Cyc_TypeList_tok_struct _temp4643; _temp4643.tag= Cyc_TypeList_tok;
_temp4643.f1=({ struct Cyc_List_List* _temp4644=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4644->hd=( void*)({ void* yyzzz;{
void* _temp4645= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
void* _temp4651; _LL4647: if(*(( void**) _temp4645) ==  Cyc_Type_tok){ _LL4652:
_temp4651=( void*)(( struct Cyc_Type_tok_struct*) _temp4645)->f1; goto _LL4648;}
else{ goto _LL4649;} _LL4649: goto _LL4650; _LL4648: yyzzz= _temp4651; goto
_LL4646; _LL4650:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL4646:;} yyzzz;});
_temp4644->tl= 0; _temp4644;}); _temp4643;}); _temp4642;}); break; case 206:
_LL4641: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4654=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4654[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4655; _temp4655.tag= Cyc_TypeList_tok; _temp4655.f1=({ struct Cyc_List_List*
_temp4656=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4656->hd=( void*)({ void* yyzzz;{ void* _temp4665= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; void* _temp4671; _LL4667:
if(*(( void**) _temp4665) ==  Cyc_Type_tok){ _LL4672: _temp4671=( void*)((
struct Cyc_Type_tok_struct*) _temp4665)->f1; goto _LL4668;} else{ goto _LL4669;}
_LL4669: goto _LL4670; _LL4668: yyzzz= _temp4671; goto _LL4666; _LL4670:( int)
_throw(( void*)& Cyc_yyfail_Type_tok); _LL4666:;} yyzzz;}); _temp4656->tl=({
struct Cyc_List_List* yyzzz;{ void* _temp4657= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp4663; _LL4659: if(*(( void**) _temp4657) ==  Cyc_TypeList_tok){ _LL4664:
_temp4663=(( struct Cyc_TypeList_tok_struct*) _temp4657)->f1; goto _LL4660;}
else{ goto _LL4661;} _LL4661: goto _LL4662; _LL4660: yyzzz= _temp4663; goto
_LL4658; _LL4662:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4658:;}
yyzzz;}); _temp4656;}); _temp4655;}); _temp4654;}); break; case 207: _LL4653:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4674=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4674[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4675; _temp4675.tag=
Cyc_AbstractDeclarator_tok; _temp4675.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4676=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4676->tms=({ struct Cyc_List_List* yyzzz;{ void* _temp4677= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp4683; _LL4679: if(*(( void**) _temp4677) ==  Cyc_TypeModifierList_tok){
_LL4684: _temp4683=(( struct Cyc_TypeModifierList_tok_struct*) _temp4677)->f1;
goto _LL4680;} else{ goto _LL4681;} _LL4681: goto _LL4682; _LL4680: yyzzz=
_temp4683; goto _LL4678; _LL4682:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL4678:;} yyzzz;}); _temp4676;}); _temp4675;}); _temp4674;}); break; case 208:
_LL4673: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 209: _LL4685: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4687=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4687[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4688; _temp4688.tag= Cyc_AbstractDeclarator_tok; _temp4688.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4689=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4689->tms= Cyc_List_imp_append(({ struct Cyc_List_List* yyzzz;{ void*
_temp4690= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4696; _LL4692: if(*(( void**) _temp4690) ==  Cyc_TypeModifierList_tok){
_LL4697: _temp4696=(( struct Cyc_TypeModifierList_tok_struct*) _temp4690)->f1;
goto _LL4693;} else{ goto _LL4694;} _LL4694: goto _LL4695; _LL4693: yyzzz=
_temp4696; goto _LL4691; _LL4695:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL4691:;} yyzzz;}),({ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void*
_temp4698= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Parse_Abstractdeclarator* _temp4704; _LL4700: if(*(( void**) _temp4698) == 
Cyc_AbstractDeclarator_tok){ _LL4705: _temp4704=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp4698)->f1; goto _LL4701;} else{ goto _LL4702;} _LL4702: goto _LL4703;
_LL4701: yyzzz= _temp4704; goto _LL4699; _LL4703:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4699:;} yyzzz;})->tms); _temp4689;}); _temp4688;}); _temp4687;}); break; case
210: _LL4686: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]; break; case 211: _LL4706: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4708=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4708[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4709; _temp4709.tag= Cyc_AbstractDeclarator_tok; _temp4709.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4710=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4710->tms=({ struct Cyc_List_List* _temp4711=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4711->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4711->tl= 0; _temp4711;}); _temp4710;}); _temp4709;}); _temp4708;}); break;
case 212: _LL4707: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4713=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4713[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4714; _temp4714.tag= Cyc_AbstractDeclarator_tok; _temp4714.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4715=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4715->tms=({ struct Cyc_List_List* _temp4716=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4716->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4716->tl=({ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp4717=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Parse_Abstractdeclarator*
_temp4723; _LL4719: if(*(( void**) _temp4717) ==  Cyc_AbstractDeclarator_tok){
_LL4724: _temp4723=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp4717)->f1;
goto _LL4720;} else{ goto _LL4721;} _LL4721: goto _LL4722; _LL4720: yyzzz=
_temp4723; goto _LL4718; _LL4722:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4718:;} yyzzz;})->tms; _temp4716;}); _temp4715;}); _temp4714;}); _temp4713;});
break; case 213: _LL4712: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4726=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4726[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4727; _temp4727.tag= Cyc_AbstractDeclarator_tok; _temp4727.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4728=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4728->tms=({ struct Cyc_List_List* _temp4729=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4729->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp4730=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp4730[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp4731; _temp4731.tag= Cyc_Absyn_ConstArray_mod;
_temp4731.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp4732= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp4738; _LL4734: if(*(( void**) _temp4732) ==  Cyc_Exp_tok){ _LL4739:
_temp4738=(( struct Cyc_Exp_tok_struct*) _temp4732)->f1; goto _LL4735;} else{
goto _LL4736;} _LL4736: goto _LL4737; _LL4735: yyzzz= _temp4738; goto _LL4733;
_LL4737:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL4733:;} yyzzz;});
_temp4731;}); _temp4730;})); _temp4729->tl= 0; _temp4729;}); _temp4728;});
_temp4727;}); _temp4726;}); break; case 214: _LL4725: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4741=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4741[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4742; _temp4742.tag= Cyc_AbstractDeclarator_tok;
_temp4742.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4743=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4743->tms=({
struct Cyc_List_List* _temp4744=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4744->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4753=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4753[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4754; _temp4754.tag= Cyc_Absyn_ConstArray_mod; _temp4754.f1=({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp4755= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Absyn_Exp* _temp4761; _LL4757: if(*(( void**)
_temp4755) ==  Cyc_Exp_tok){ _LL4762: _temp4761=(( struct Cyc_Exp_tok_struct*)
_temp4755)->f1; goto _LL4758;} else{ goto _LL4759;} _LL4759: goto _LL4760;
_LL4758: yyzzz= _temp4761; goto _LL4756; _LL4760:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL4756:;} yyzzz;}); _temp4754;}); _temp4753;})); _temp4744->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4745= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4751; _LL4747:
if(*(( void**) _temp4745) ==  Cyc_AbstractDeclarator_tok){ _LL4752: _temp4751=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4745)->f1; goto _LL4748;} else{
goto _LL4749;} _LL4749: goto _LL4750; _LL4748: yyzzz= _temp4751; goto _LL4746;
_LL4750:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4746:;}
yyzzz;})->tms; _temp4744;}); _temp4743;}); _temp4742;}); _temp4741;}); break;
case 215: _LL4740: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4764=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4764[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4765; _temp4765.tag= Cyc_AbstractDeclarator_tok; _temp4765.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4766=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4766->tms=({ struct Cyc_List_List* _temp4767=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4767->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4768=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4768[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4769; _temp4769.tag= Cyc_Absyn_Function_mod;
_temp4769.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4770=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4770[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4771; _temp4771.tag= Cyc_Absyn_WithTypes;
_temp4771.f1= 0; _temp4771.f2= 0; _temp4771.f3= 0; _temp4771.f4=({ struct Cyc_Core_Opt*
yyzzz;{ void* _temp4772= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Core_Opt* _temp4778; _LL4774: if(*(( void**)
_temp4772) ==  Cyc_TypeOpt_tok){ _LL4779: _temp4778=(( struct Cyc_TypeOpt_tok_struct*)
_temp4772)->f1; goto _LL4775;} else{ goto _LL4776;} _LL4776: goto _LL4777;
_LL4775: yyzzz= _temp4778; goto _LL4773; _LL4777:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok);
_LL4773:;} yyzzz;}); _temp4771.f5=({ struct Cyc_List_List* yyzzz;{ void*
_temp4780= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4786; _LL4782: if(*(( void**) _temp4780) ==  Cyc_Rgnorder_tok){
_LL4787: _temp4786=(( struct Cyc_Rgnorder_tok_struct*) _temp4780)->f1; goto
_LL4783;} else{ goto _LL4784;} _LL4784: goto _LL4785; _LL4783: yyzzz= _temp4786;
goto _LL4781; _LL4785:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL4781:;}
yyzzz;}); _temp4771;}); _temp4770;})); _temp4769;}); _temp4768;})); _temp4767->tl=
0; _temp4767;}); _temp4766;}); _temp4765;}); _temp4764;}); break; case 216:
_LL4763: { struct _tuple16 _temp4799; struct Cyc_List_List* _temp4800; struct
Cyc_Core_Opt* _temp4802; struct Cyc_Absyn_VarargInfo* _temp4804; int _temp4806;
struct Cyc_List_List* _temp4808; struct _tuple16* _temp4797=({ struct _tuple16*
yyzzz;{ void* _temp4789= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tuple16* _temp4795; _LL4791: if(*(( void**)
_temp4789) ==  Cyc_ParamDeclListBool_tok){ _LL4796: _temp4795=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp4789)->f1; goto _LL4792;} else{ goto _LL4793;} _LL4793: goto _LL4794;
_LL4792: yyzzz= _temp4795; goto _LL4790; _LL4794:( int) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL4790:;} yyzzz;}); _temp4799=* _temp4797; _LL4809: _temp4808= _temp4799.f1;
goto _LL4807; _LL4807: _temp4806= _temp4799.f2; goto _LL4805; _LL4805: _temp4804=
_temp4799.f3; goto _LL4803; _LL4803: _temp4802= _temp4799.f4; goto _LL4801;
_LL4801: _temp4800= _temp4799.f5; goto _LL4798; _LL4798: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4810=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4810[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4811; _temp4811.tag= Cyc_AbstractDeclarator_tok;
_temp4811.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4812=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4812->tms=({
struct Cyc_List_List* _temp4813=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4813->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4814=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4814[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4815; _temp4815.tag=
Cyc_Absyn_Function_mod; _temp4815.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4816=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4816[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4817; _temp4817.tag= Cyc_Absyn_WithTypes;
_temp4817.f1= _temp4808; _temp4817.f2= _temp4806; _temp4817.f3= _temp4804;
_temp4817.f4= _temp4802; _temp4817.f5= _temp4800; _temp4817;}); _temp4816;}));
_temp4815;}); _temp4814;})); _temp4813->tl= 0; _temp4813;}); _temp4812;});
_temp4811;}); _temp4810;}); break;} case 217: _LL4788: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4819=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4819[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4820; _temp4820.tag= Cyc_AbstractDeclarator_tok;
_temp4820.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4821=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4821->tms=({
struct Cyc_List_List* _temp4822=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4822->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4831=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4831[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4832; _temp4832.tag=
Cyc_Absyn_Function_mod; _temp4832.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4833=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4833[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4834; _temp4834.tag= Cyc_Absyn_WithTypes;
_temp4834.f1= 0; _temp4834.f2= 0; _temp4834.f3= 0; _temp4834.f4=({ struct Cyc_Core_Opt*
yyzzz;{ void* _temp4835= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct Cyc_Core_Opt* _temp4841; _LL4837: if(*(( void**)
_temp4835) ==  Cyc_TypeOpt_tok){ _LL4842: _temp4841=(( struct Cyc_TypeOpt_tok_struct*)
_temp4835)->f1; goto _LL4838;} else{ goto _LL4839;} _LL4839: goto _LL4840;
_LL4838: yyzzz= _temp4841; goto _LL4836; _LL4840:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok);
_LL4836:;} yyzzz;}); _temp4834.f5=({ struct Cyc_List_List* yyzzz;{ void*
_temp4843= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp4849; _LL4845: if(*(( void**) _temp4843) ==  Cyc_Rgnorder_tok){
_LL4850: _temp4849=(( struct Cyc_Rgnorder_tok_struct*) _temp4843)->f1; goto
_LL4846;} else{ goto _LL4847;} _LL4847: goto _LL4848; _LL4846: yyzzz= _temp4849;
goto _LL4844; _LL4848:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL4844:;}
yyzzz;}); _temp4834;}); _temp4833;})); _temp4832;}); _temp4831;})); _temp4822->tl=({
struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp4823= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Parse_Abstractdeclarator*
_temp4829; _LL4825: if(*(( void**) _temp4823) ==  Cyc_AbstractDeclarator_tok){
_LL4830: _temp4829=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp4823)->f1;
goto _LL4826;} else{ goto _LL4827;} _LL4827: goto _LL4828; _LL4826: yyzzz=
_temp4829; goto _LL4824; _LL4828:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL4824:;} yyzzz;})->tms; _temp4822;}); _temp4821;}); _temp4820;}); _temp4819;});
break; case 218: _LL4818: { struct _tuple16 _temp4862; struct Cyc_List_List*
_temp4863; struct Cyc_Core_Opt* _temp4865; struct Cyc_Absyn_VarargInfo*
_temp4867; int _temp4869; struct Cyc_List_List* _temp4871; struct _tuple16*
_temp4860=({ struct _tuple16* yyzzz;{ void* _temp4852= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tuple16*
_temp4858; _LL4854: if(*(( void**) _temp4852) ==  Cyc_ParamDeclListBool_tok){
_LL4859: _temp4858=(( struct Cyc_ParamDeclListBool_tok_struct*) _temp4852)->f1;
goto _LL4855;} else{ goto _LL4856;} _LL4856: goto _LL4857; _LL4855: yyzzz=
_temp4858; goto _LL4853; _LL4857:( int) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL4853:;} yyzzz;}); _temp4862=* _temp4860; _LL4872: _temp4871= _temp4862.f1;
goto _LL4870; _LL4870: _temp4869= _temp4862.f2; goto _LL4868; _LL4868: _temp4867=
_temp4862.f3; goto _LL4866; _LL4866: _temp4865= _temp4862.f4; goto _LL4864;
_LL4864: _temp4863= _temp4862.f5; goto _LL4861; _LL4861: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4873=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4873[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4874; _temp4874.tag= Cyc_AbstractDeclarator_tok;
_temp4874.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4875=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4875->tms=({
struct Cyc_List_List* _temp4876=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4876->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4885=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4885[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4886; _temp4886.tag=
Cyc_Absyn_Function_mod; _temp4886.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4887=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4887[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4888; _temp4888.tag= Cyc_Absyn_WithTypes;
_temp4888.f1= _temp4871; _temp4888.f2= _temp4869; _temp4888.f3= _temp4867;
_temp4888.f4= _temp4865; _temp4888.f5= _temp4863; _temp4888;}); _temp4887;}));
_temp4886;}); _temp4885;})); _temp4876->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4877= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4883; _LL4879:
if(*(( void**) _temp4877) ==  Cyc_AbstractDeclarator_tok){ _LL4884: _temp4883=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4877)->f1; goto _LL4880;} else{
goto _LL4881;} _LL4881: goto _LL4882; _LL4880: yyzzz= _temp4883; goto _LL4878;
_LL4882:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4878:;}
yyzzz;})->tms; _temp4876;}); _temp4875;}); _temp4874;}); _temp4873;}); break;}
case 219: _LL4851: { struct Cyc_List_List* _temp4890=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp4905= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4911; _LL4907: if(*(( void**) _temp4905) ==  Cyc_TypeList_tok){ _LL4912:
_temp4911=(( struct Cyc_TypeList_tok_struct*) _temp4905)->f1; goto _LL4908;}
else{ goto _LL4909;} _LL4909: goto _LL4910; _LL4908: yyzzz= _temp4911; goto
_LL4906; _LL4910:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4906:;}
yyzzz;}))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4891=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4891[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4892; _temp4892.tag=
Cyc_AbstractDeclarator_tok; _temp4892.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4893=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4893->tms=({ struct Cyc_List_List* _temp4894=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4894->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4903=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4903[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4904; _temp4904.tag= Cyc_Absyn_TypeParams_mod;
_temp4904.f1= _temp4890; _temp4904.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp4904.f3=
0; _temp4904;}); _temp4903;})); _temp4894->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4895= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4901; _LL4897:
if(*(( void**) _temp4895) ==  Cyc_AbstractDeclarator_tok){ _LL4902: _temp4901=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4895)->f1; goto _LL4898;} else{
goto _LL4899;} _LL4899: goto _LL4900; _LL4898: yyzzz= _temp4901; goto _LL4896;
_LL4900:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4896:;}
yyzzz;})->tms; _temp4894;}); _temp4893;}); _temp4892;}); _temp4891;}); break;}
case 220: _LL4889:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;{ struct Cyc_List_List*
_temp4914=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp4929= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp4935; _LL4931: if(*(( void**) _temp4929) ==  Cyc_TypeList_tok){ _LL4936:
_temp4935=(( struct Cyc_TypeList_tok_struct*) _temp4929)->f1; goto _LL4932;}
else{ goto _LL4933;} _LL4933: goto _LL4934; _LL4932: yyzzz= _temp4935; goto
_LL4930; _LL4934:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL4930:;}
yyzzz;}))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4915=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4915[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4916; _temp4916.tag=
Cyc_AbstractDeclarator_tok; _temp4916.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4917=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4917->tms=({ struct Cyc_List_List* _temp4918=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4918->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4927=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4927[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4928; _temp4928.tag= Cyc_Absyn_TypeParams_mod;
_temp4928.f1= _temp4914; _temp4928.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp4928.f3=
0; _temp4928;}); _temp4927;})); _temp4918->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4919= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct Cyc_Parse_Abstractdeclarator* _temp4925; _LL4921:
if(*(( void**) _temp4919) ==  Cyc_AbstractDeclarator_tok){ _LL4926: _temp4925=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4919)->f1; goto _LL4922;} else{
goto _LL4923;} _LL4923: goto _LL4924; _LL4922: yyzzz= _temp4925; goto _LL4920;
_LL4924:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4920:;}
yyzzz;})->tms; _temp4918;}); _temp4917;}); _temp4916;}); _temp4915;}); break;}
case 221: _LL4913: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4938=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4938[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4939; _temp4939.tag= Cyc_AbstractDeclarator_tok; _temp4939.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4940=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4940->tms=({ struct Cyc_List_List* _temp4941=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4941->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp4950=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp4950[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp4951; _temp4951.tag= Cyc_Absyn_Attributes_mod;
_temp4951.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp4951.f2=({ struct Cyc_List_List* yyzzz;{ void*
_temp4952= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_List_List* _temp4958; _LL4954: if(*(( void**) _temp4952) ==  Cyc_AttributeList_tok){
_LL4959: _temp4958=(( struct Cyc_AttributeList_tok_struct*) _temp4952)->f1; goto
_LL4955;} else{ goto _LL4956;} _LL4956: goto _LL4957; _LL4955: yyzzz= _temp4958;
goto _LL4953; _LL4957:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL4953:;} yyzzz;}); _temp4951;}); _temp4950;})); _temp4941->tl=({ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp4942= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Parse_Abstractdeclarator* _temp4948; _LL4944:
if(*(( void**) _temp4942) ==  Cyc_AbstractDeclarator_tok){ _LL4949: _temp4948=((
struct Cyc_AbstractDeclarator_tok_struct*) _temp4942)->f1; goto _LL4945;} else{
goto _LL4946;} _LL4946: goto _LL4947; _LL4945: yyzzz= _temp4948; goto _LL4943;
_LL4947:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL4943:;}
yyzzz;})->tms; _temp4941;}); _temp4940;}); _temp4939;}); _temp4938;}); break;
case 222: _LL4937: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 223: _LL4960: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL4961:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL4962: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL4963: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL4964: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL4965:
if( Cyc_std_zstrcmp(({ struct _tagged_arr yyzzz;{ void* _temp4967= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct _tagged_arr
_temp4973; _LL4969: if(*(( void**) _temp4967) ==  Cyc_String_tok){ _LL4974:
_temp4973=(( struct Cyc_String_tok_struct*) _temp4967)->f1; goto _LL4970;} else{
goto _LL4971;} _LL4971: goto _LL4972; _LL4970: yyzzz= _temp4973; goto _LL4968;
_LL4972:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL4968:;} yyzzz;}),
_tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5001=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5001->name=({ struct
_tagged_arr* _temp5002=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp5002[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp5003= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct _tagged_arr
_temp5009; _LL5005: if(*(( void**) _temp5003) ==  Cyc_String_tok){ _LL5010:
_temp5009=(( struct Cyc_String_tok_struct*) _temp5003)->f1; goto _LL5006;} else{
goto _LL5007;} _LL5007: goto _LL5008; _LL5006: yyzzz= _temp5009; goto _LL5004;
_LL5008:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL5004:;} yyzzz;});
_temp5002;}); _temp5001->identity= 0; _temp5001->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp5001;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp4999=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp4999[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5000; _temp5000.tag= Cyc_Absyn_VarType;
_temp5000.f1= tv; _temp5000;}); _temp4999;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp4975=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4975[ 0]=({ struct Cyc_Stmt_tok_struct _temp4976; _temp4976.tag= Cyc_Stmt_tok;
_temp4976.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp4977=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp4977[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp4978; _temp4978.tag= Cyc_Absyn_Region_s;
_temp4978.f1= tv; _temp4978.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp4979=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4979->f1=(
void*) Cyc_Absyn_Loc_n; _temp4979->f2=({ struct _tagged_arr* _temp4980=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp4980[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp4981= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp4987; _LL4983: if(*(( void**)
_temp4981) ==  Cyc_String_tok){ _LL4988: _temp4987=(( struct Cyc_String_tok_struct*)
_temp4981)->f1; goto _LL4984;} else{ goto _LL4985;} _LL4985: goto _LL4986;
_LL4984: yyzzz= _temp4987; goto _LL4982; _LL4986:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL4982:;} yyzzz;}); _temp4980;}); _temp4979;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp4989=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp4989[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp4990; _temp4990.tag= Cyc_Absyn_RgnHandleType; _temp4990.f1=( void*) t;
_temp4990;}); _temp4989;}), 0); _temp4978.f3=({ struct Cyc_Absyn_Stmt* yyzzz;{
void* _temp4991= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_Absyn_Stmt* _temp4997; _LL4993: if(*(( void**) _temp4991) ==  Cyc_Stmt_tok){
_LL4998: _temp4997=(( struct Cyc_Stmt_tok_struct*) _temp4991)->f1; goto _LL4994;}
else{ goto _LL4995;} _LL4995: goto _LL4996; _LL4994: yyzzz= _temp4997; goto
_LL4992; _LL4996:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL4992:;} yyzzz;});
_temp4978;}); _temp4977;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp4976;});
_temp4975;}); break;} case 229: _LL4966: if( Cyc_std_zstrcmp(({ struct
_tagged_arr yyzzz;{ void* _temp5012= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp5018; _LL5014: if(*(( void**)
_temp5012) ==  Cyc_String_tok){ _LL5019: _temp5018=(( struct Cyc_String_tok_struct*)
_temp5012)->f1; goto _LL5015;} else{ goto _LL5016;} _LL5016: goto _LL5017;
_LL5015: yyzzz= _temp5018; goto _LL5013; _LL5017:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5013:;} yyzzz;}), _tag_arr("H", sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err(
_tag_arr("bad occurrence of heap region `H", sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5046=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5046->name=({ struct
_tagged_arr* _temp5047=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp5047[ 0]=( struct _tagged_arr)({ struct Cyc_std_String_pa_struct
_temp5049; _temp5049.tag= Cyc_std_String_pa; _temp5049.f1=( struct _tagged_arr)({
struct _tagged_arr yyzzz;{ void* _temp5050= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct _tagged_arr _temp5056; _LL5052: if(*(( void**)
_temp5050) ==  Cyc_String_tok){ _LL5057: _temp5056=(( struct Cyc_String_tok_struct*)
_temp5050)->f1; goto _LL5053;} else{ goto _LL5054;} _LL5054: goto _LL5055;
_LL5053: yyzzz= _temp5056; goto _LL5051; _LL5055:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5051:;} yyzzz;});{ void* _temp5048[ 1u]={& _temp5049}; Cyc_std_aprintf(
_tag_arr("`%s", sizeof( unsigned char), 4u), _tag_arr( _temp5048, sizeof( void*),
1u));}}); _temp5047;}); _temp5046->identity= 0; _temp5046->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp5046;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5044=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5044[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5045; _temp5045.tag= Cyc_Absyn_VarType;
_temp5045.f1= tv; _temp5045;}); _temp5044;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5020=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5020[ 0]=({ struct Cyc_Stmt_tok_struct _temp5021; _temp5021.tag= Cyc_Stmt_tok;
_temp5021.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5022=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5022[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5023; _temp5023.tag= Cyc_Absyn_Region_s;
_temp5023.f1= tv; _temp5023.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5024=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5024->f1=(
void*) Cyc_Absyn_Loc_n; _temp5024->f2=({ struct _tagged_arr* _temp5025=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp5025[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp5026= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp5032; _LL5028: if(*(( void**)
_temp5026) ==  Cyc_String_tok){ _LL5033: _temp5032=(( struct Cyc_String_tok_struct*)
_temp5026)->f1; goto _LL5029;} else{ goto _LL5030;} _LL5030: goto _LL5031;
_LL5029: yyzzz= _temp5032; goto _LL5027; _LL5031:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5027:;} yyzzz;}); _temp5025;}); _temp5024;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp5034=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp5034[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp5035; _temp5035.tag= Cyc_Absyn_RgnHandleType; _temp5035.f1=( void*) t;
_temp5035;}); _temp5034;}), 0); _temp5023.f3=({ struct Cyc_Absyn_Stmt* yyzzz;{
void* _temp5036= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_Absyn_Stmt* _temp5042; _LL5038: if(*(( void**) _temp5036) ==  Cyc_Stmt_tok){
_LL5043: _temp5042=(( struct Cyc_Stmt_tok_struct*) _temp5036)->f1; goto _LL5039;}
else{ goto _LL5040;} _LL5040: goto _LL5041; _LL5039: yyzzz= _temp5042; goto
_LL5037; _LL5041:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5037:;} yyzzz;});
_temp5023;}); _temp5022;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5021;});
_temp5020;}); break;} case 230: _LL5011: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5059=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5059[ 0]=({ struct Cyc_Stmt_tok_struct _temp5060; _temp5060.tag= Cyc_Stmt_tok;
_temp5060.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp5061=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp5061[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp5062; _temp5062.tag= Cyc_Absyn_Cut_s;
_temp5062.f1=({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5063= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5069; _LL5065: if(*(( void**) _temp5063) ==  Cyc_Stmt_tok){ _LL5070:
_temp5069=(( struct Cyc_Stmt_tok_struct*) _temp5063)->f1; goto _LL5066;} else{
goto _LL5067;} _LL5067: goto _LL5068; _LL5066: yyzzz= _temp5069; goto _LL5064;
_LL5068:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5064:;} yyzzz;});
_temp5062;}); _temp5061;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5060;});
_temp5059;}); break; case 231: _LL5058: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5072=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5072[ 0]=({ struct Cyc_Stmt_tok_struct _temp5073; _temp5073.tag= Cyc_Stmt_tok;
_temp5073.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp5074=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp5074[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp5075; _temp5075.tag= Cyc_Absyn_Splice_s;
_temp5075.f1=({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5076= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5082; _LL5078: if(*(( void**) _temp5076) ==  Cyc_Stmt_tok){ _LL5083:
_temp5082=(( struct Cyc_Stmt_tok_struct*) _temp5076)->f1; goto _LL5079;} else{
goto _LL5080;} _LL5080: goto _LL5081; _LL5079: yyzzz= _temp5082; goto _LL5077;
_LL5081:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5077:;} yyzzz;});
_temp5075;}); _temp5074;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5073;});
_temp5072;}); break; case 232: _LL5071: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5085=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5085[ 0]=({ struct Cyc_Stmt_tok_struct _temp5086; _temp5086.tag= Cyc_Stmt_tok;
_temp5086.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp5087=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp5087[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp5088; _temp5088.tag= Cyc_Absyn_Label_s;
_temp5088.f1=({ struct _tagged_arr* _temp5089=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp5089[ 0]=({ struct _tagged_arr yyzzz;{ void*
_temp5090= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct _tagged_arr _temp5096; _LL5092: if(*(( void**) _temp5090) ==  Cyc_String_tok){
_LL5097: _temp5096=(( struct Cyc_String_tok_struct*) _temp5090)->f1; goto
_LL5093;} else{ goto _LL5094;} _LL5094: goto _LL5095; _LL5093: yyzzz= _temp5096;
goto _LL5091; _LL5095:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL5091:;}
yyzzz;}); _temp5089;}); _temp5088.f2=({ struct Cyc_Absyn_Stmt* yyzzz;{ void*
_temp5098= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Stmt* _temp5104; _LL5100: if(*(( void**) _temp5098) ==  Cyc_Stmt_tok){
_LL5105: _temp5104=(( struct Cyc_Stmt_tok_struct*) _temp5098)->f1; goto _LL5101;}
else{ goto _LL5102;} _LL5102: goto _LL5103; _LL5101: yyzzz= _temp5104; goto
_LL5099; _LL5103:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5099:;} yyzzz;});
_temp5088;}); _temp5087;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5086;});
_temp5085;}); break; case 233: _LL5084: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5107=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5107[ 0]=({ struct Cyc_Stmt_tok_struct _temp5108; _temp5108.tag= Cyc_Stmt_tok;
_temp5108.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5108;});
_temp5107;}); break; case 234: _LL5106: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5110=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5110[ 0]=({ struct Cyc_Stmt_tok_struct _temp5111; _temp5111.tag= Cyc_Stmt_tok;
_temp5111.f1= Cyc_Absyn_exp_stmt(({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp5112= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Exp* _temp5118; _LL5114: if(*(( void**) _temp5112) ==  Cyc_Exp_tok){
_LL5119: _temp5118=(( struct Cyc_Exp_tok_struct*) _temp5112)->f1; goto _LL5115;}
else{ goto _LL5116;} _LL5116: goto _LL5117; _LL5115: yyzzz= _temp5118; goto
_LL5113; _LL5117:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5113:;} yyzzz;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp5111;}); _temp5110;}); break; case 235:
_LL5109: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5121=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5121[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5122; _temp5122.tag= Cyc_Stmt_tok; _temp5122.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5122;}); _temp5121;}); break; case 236: _LL5120: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 237:
_LL5123: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5125=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5125[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5126; _temp5126.tag= Cyc_Stmt_tok; _temp5126.f1= Cyc_Parse_flatten_declarations(({
struct Cyc_List_List* yyzzz;{ void* _temp5127= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5133; _LL5129: if(*(( void**) _temp5127) ==  Cyc_DeclList_tok){ _LL5134:
_temp5133=(( struct Cyc_DeclList_tok_struct*) _temp5127)->f1; goto _LL5130;}
else{ goto _LL5131;} _LL5131: goto _LL5132; _LL5130: yyzzz= _temp5133; goto
_LL5128; _LL5132:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5128:;}
yyzzz;}), Cyc_Absyn_skip_stmt( 0)); _temp5126;}); _temp5125;}); break; case 238:
_LL5124: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5136=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5136[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5137; _temp5137.tag= Cyc_Stmt_tok; _temp5137.f1= Cyc_Parse_flatten_declarations(({
struct Cyc_List_List* yyzzz;{ void* _temp5138= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5144; _LL5140: if(*(( void**) _temp5138) ==  Cyc_DeclList_tok){ _LL5145:
_temp5144=(( struct Cyc_DeclList_tok_struct*) _temp5138)->f1; goto _LL5141;}
else{ goto _LL5142;} _LL5142: goto _LL5143; _LL5141: yyzzz= _temp5144; goto
_LL5139; _LL5143:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5139:;}
yyzzz;}),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5146= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5152; _LL5148: if(*(( void**) _temp5146) ==  Cyc_Stmt_tok){ _LL5153:
_temp5152=(( struct Cyc_Stmt_tok_struct*) _temp5146)->f1; goto _LL5149;} else{
goto _LL5150;} _LL5150: goto _LL5151; _LL5149: yyzzz= _temp5152; goto _LL5147;
_LL5151:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5147:;} yyzzz;}));
_temp5137;}); _temp5136;}); break; case 239: _LL5135: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 240: _LL5154:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5156=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5156[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5157; _temp5157.tag= Cyc_Stmt_tok; _temp5157.f1= Cyc_Absyn_seq_stmt(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5158= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Stmt*
_temp5164; _LL5160: if(*(( void**) _temp5158) ==  Cyc_Stmt_tok){ _LL5165:
_temp5164=(( struct Cyc_Stmt_tok_struct*) _temp5158)->f1; goto _LL5161;} else{
goto _LL5162;} _LL5162: goto _LL5163; _LL5161: yyzzz= _temp5164; goto _LL5159;
_LL5163:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5159:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5166= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5172; _LL5168: if(*(( void**) _temp5166) ==  Cyc_Stmt_tok){ _LL5173:
_temp5172=(( struct Cyc_Stmt_tok_struct*) _temp5166)->f1; goto _LL5169;} else{
goto _LL5170;} _LL5170: goto _LL5171; _LL5169: yyzzz= _temp5172; goto _LL5167;
_LL5171:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5167:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5157;}); _temp5156;}); break; case 241: _LL5155: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5175=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5175[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5176; _temp5176.tag= Cyc_Stmt_tok; _temp5176.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp5177=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5177[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5178; _temp5178.tag= Cyc_Absyn_Fn_d;
_temp5178.f1=({ struct Cyc_Absyn_Fndecl* yyzzz;{ void* _temp5179= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Fndecl*
_temp5185; _LL5181: if(*(( void**) _temp5179) ==  Cyc_FnDecl_tok){ _LL5186:
_temp5185=(( struct Cyc_FnDecl_tok_struct*) _temp5179)->f1; goto _LL5182;} else{
goto _LL5183;} _LL5183: goto _LL5184; _LL5182: yyzzz= _temp5185; goto _LL5180;
_LL5184:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL5180:;} yyzzz;});
_temp5178;}); _temp5177;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp5176;}); _temp5175;}); break; case 242: _LL5174: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp5188=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5188[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5189; _temp5189.tag= Cyc_Stmt_tok; _temp5189.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp5190=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5190[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5191; _temp5191.tag= Cyc_Absyn_Fn_d;
_temp5191.f1=({ struct Cyc_Absyn_Fndecl* yyzzz;{ void* _temp5192= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Fndecl*
_temp5198; _LL5194: if(*(( void**) _temp5192) ==  Cyc_FnDecl_tok){ _LL5199:
_temp5198=(( struct Cyc_FnDecl_tok_struct*) _temp5192)->f1; goto _LL5195;} else{
goto _LL5196;} _LL5196: goto _LL5197; _LL5195: yyzzz= _temp5198; goto _LL5193;
_LL5197:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL5193:;} yyzzz;});
_temp5191;}); _temp5190;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5200= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5206; _LL5202: if(*(( void**) _temp5200) ==  Cyc_Stmt_tok){ _LL5207:
_temp5206=(( struct Cyc_Stmt_tok_struct*) _temp5200)->f1; goto _LL5203;} else{
goto _LL5204;} _LL5204: goto _LL5205; _LL5203: yyzzz= _temp5206; goto _LL5201;
_LL5205:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5201:;} yyzzz;}));
_temp5189;}); _temp5188;}); break; case 243: _LL5187: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5209=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5209[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5210; _temp5210.tag= Cyc_Stmt_tok; _temp5210.f1= Cyc_Absyn_ifthenelse_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5211= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5217; _LL5213: if(*(( void**) _temp5211) ==  Cyc_Exp_tok){ _LL5218:
_temp5217=(( struct Cyc_Exp_tok_struct*) _temp5211)->f1; goto _LL5214;} else{
goto _LL5215;} _LL5215: goto _LL5216; _LL5214: yyzzz= _temp5217; goto _LL5212;
_LL5216:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5212:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5219= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5225; _LL5221: if(*(( void**) _temp5219) ==  Cyc_Stmt_tok){ _LL5226:
_temp5225=(( struct Cyc_Stmt_tok_struct*) _temp5219)->f1; goto _LL5222;} else{
goto _LL5223;} _LL5223: goto _LL5224; _LL5222: yyzzz= _temp5225; goto _LL5220;
_LL5224:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5220:;} yyzzz;}), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp5210;}); _temp5209;}); break; case 244:
_LL5208: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5228=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5228[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5229; _temp5229.tag= Cyc_Stmt_tok; _temp5229.f1= Cyc_Absyn_ifthenelse_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5230= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5236; _LL5232: if(*(( void**) _temp5230) ==  Cyc_Exp_tok){ _LL5237:
_temp5236=(( struct Cyc_Exp_tok_struct*) _temp5230)->f1; goto _LL5233;} else{
goto _LL5234;} _LL5234: goto _LL5235; _LL5233: yyzzz= _temp5236; goto _LL5231;
_LL5235:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5231:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5238= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Stmt*
_temp5244; _LL5240: if(*(( void**) _temp5238) ==  Cyc_Stmt_tok){ _LL5245:
_temp5244=(( struct Cyc_Stmt_tok_struct*) _temp5238)->f1; goto _LL5241;} else{
goto _LL5242;} _LL5242: goto _LL5243; _LL5241: yyzzz= _temp5244; goto _LL5239;
_LL5243:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5239:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5246= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5252; _LL5248: if(*(( void**) _temp5246) ==  Cyc_Stmt_tok){ _LL5253:
_temp5252=(( struct Cyc_Stmt_tok_struct*) _temp5246)->f1; goto _LL5249;} else{
goto _LL5250;} _LL5250: goto _LL5251; _LL5249: yyzzz= _temp5252; goto _LL5247;
_LL5251:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5247:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5229;}); _temp5228;}); break; case 245: _LL5227: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5255=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5255[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5256; _temp5256.tag= Cyc_Stmt_tok; _temp5256.f1= Cyc_Absyn_switch_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5257= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5263; _LL5259: if(*(( void**) _temp5257) ==  Cyc_Exp_tok){ _LL5264:
_temp5263=(( struct Cyc_Exp_tok_struct*) _temp5257)->f1; goto _LL5260;} else{
goto _LL5261;} _LL5261: goto _LL5262; _LL5260: yyzzz= _temp5263; goto _LL5258;
_LL5262:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5258:;} yyzzz;}),({
struct Cyc_List_List* yyzzz;{ void* _temp5265= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5271; _LL5267: if(*(( void**) _temp5265) ==  Cyc_SwitchClauseList_tok){
_LL5272: _temp5271=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5265)->f1;
goto _LL5268;} else{ goto _LL5269;} _LL5269: goto _LL5270; _LL5268: yyzzz=
_temp5271; goto _LL5266; _LL5270:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5266:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5256;});
_temp5255;}); break; case 246: _LL5254: if( Cyc_std_strcmp(({ struct _tagged_arr
yyzzz;{ void* _temp5274= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  6)]; struct _tagged_arr _temp5280; _LL5276: if(*(( void**)
_temp5274) ==  Cyc_String_tok){ _LL5281: _temp5280=(( struct Cyc_String_tok_struct*)
_temp5274)->f1; goto _LL5277;} else{ goto _LL5278;} _LL5278: goto _LL5279;
_LL5277: yyzzz= _temp5280; goto _LL5275; _LL5279:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5275:;} yyzzz;}), _tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err(
_tag_arr("only switch \"C\" { ... } is allowed", sizeof( unsigned char), 35u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5282=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5282[ 0]=({ struct Cyc_Stmt_tok_struct _temp5283; _temp5283.tag= Cyc_Stmt_tok;
_temp5283.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_SwitchC_s_struct*
_temp5284=( struct Cyc_Absyn_SwitchC_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct));
_temp5284[ 0]=({ struct Cyc_Absyn_SwitchC_s_struct _temp5285; _temp5285.tag= Cyc_Absyn_SwitchC_s;
_temp5285.f1=({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5286= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5292; _LL5288: if(*(( void**) _temp5286) ==  Cyc_Exp_tok){ _LL5293:
_temp5292=(( struct Cyc_Exp_tok_struct*) _temp5286)->f1; goto _LL5289;} else{
goto _LL5290;} _LL5290: goto _LL5291; _LL5289: yyzzz= _temp5292; goto _LL5287;
_LL5291:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5287:;} yyzzz;});
_temp5285.f2=({ struct Cyc_List_List* yyzzz;{ void* _temp5294= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5300; _LL5296: if(*(( void**) _temp5294) ==  Cyc_SwitchCClauseList_tok){
_LL5301: _temp5300=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp5294)->f1;
goto _LL5297;} else{ goto _LL5298;} _LL5298: goto _LL5299; _LL5297: yyzzz=
_temp5300; goto _LL5295; _LL5299:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL5295:;} yyzzz;}); _temp5285;}); _temp5284;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5283;}); _temp5282;}); break; case 247: _LL5273: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5303=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5303[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5304; _temp5304.tag= Cyc_Stmt_tok; _temp5304.f1= Cyc_Absyn_trycatch_stmt(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5305= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Stmt*
_temp5311; _LL5307: if(*(( void**) _temp5305) ==  Cyc_Stmt_tok){ _LL5312:
_temp5311=(( struct Cyc_Stmt_tok_struct*) _temp5305)->f1; goto _LL5308;} else{
goto _LL5309;} _LL5309: goto _LL5310; _LL5308: yyzzz= _temp5311; goto _LL5306;
_LL5310:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5306:;} yyzzz;}),({
struct Cyc_List_List* yyzzz;{ void* _temp5313= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp5319; _LL5315: if(*(( void**) _temp5313) ==  Cyc_SwitchClauseList_tok){
_LL5320: _temp5319=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5313)->f1;
goto _LL5316;} else{ goto _LL5317;} _LL5317: goto _LL5318; _LL5316: yyzzz=
_temp5319; goto _LL5314; _LL5318:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5314:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5304;});
_temp5303;}); break; case 248: _LL5302: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5322=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5322[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5323; _temp5323.tag= Cyc_SwitchClauseList_tok; _temp5323.f1= 0; _temp5323;});
_temp5322;}); break; case 249: _LL5321: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5325=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5325[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5326; _temp5326.tag= Cyc_SwitchClauseList_tok; _temp5326.f1=({ struct Cyc_List_List*
_temp5327=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5327->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5328=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5328->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp5328->pat_vars= 0; _temp5328->where_clause= 0; _temp5328->body=({ struct
Cyc_Absyn_Stmt* yyzzz;{ void* _temp5329= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt* _temp5335; _LL5331: if(*(( void**)
_temp5329) ==  Cyc_Stmt_tok){ _LL5336: _temp5335=(( struct Cyc_Stmt_tok_struct*)
_temp5329)->f1; goto _LL5332;} else{ goto _LL5333;} _LL5333: goto _LL5334;
_LL5332: yyzzz= _temp5335; goto _LL5330; _LL5334:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL5330:;} yyzzz;}); _temp5328->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5328;});
_temp5327->tl= 0; _temp5327;}); _temp5326;}); _temp5325;}); break; case 250:
_LL5324: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5338=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5338[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5339; _temp5339.tag=
Cyc_SwitchClauseList_tok; _temp5339.f1=({ struct Cyc_List_List* _temp5340=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5340->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5349=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5349->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5350= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Pat*
_temp5356; _LL5352: if(*(( void**) _temp5350) ==  Cyc_Pattern_tok){ _LL5357:
_temp5356=(( struct Cyc_Pattern_tok_struct*) _temp5350)->f1; goto _LL5353;}
else{ goto _LL5354;} _LL5354: goto _LL5355; _LL5353: yyzzz= _temp5356; goto
_LL5351; _LL5355:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5351:;}
yyzzz;}); _temp5349->pat_vars= 0; _temp5349->where_clause= 0; _temp5349->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5349->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5349;});
_temp5340->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5341= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5347; _LL5343: if(*(( void**) _temp5341) ==  Cyc_SwitchClauseList_tok){
_LL5348: _temp5347=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5341)->f1;
goto _LL5344;} else{ goto _LL5345;} _LL5345: goto _LL5346; _LL5344: yyzzz=
_temp5347; goto _LL5342; _LL5346:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5342:;} yyzzz;}); _temp5340;}); _temp5339;}); _temp5338;}); break; case 251:
_LL5337: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5359=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5359[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5360; _temp5360.tag=
Cyc_SwitchClauseList_tok; _temp5360.f1=({ struct Cyc_List_List* _temp5361=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5361->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5370=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5370->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5379= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Pat*
_temp5385; _LL5381: if(*(( void**) _temp5379) ==  Cyc_Pattern_tok){ _LL5386:
_temp5385=(( struct Cyc_Pattern_tok_struct*) _temp5379)->f1; goto _LL5382;}
else{ goto _LL5383;} _LL5383: goto _LL5384; _LL5382: yyzzz= _temp5385; goto
_LL5380; _LL5384:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5380:;}
yyzzz;}); _temp5370->pat_vars= 0; _temp5370->where_clause= 0; _temp5370->body=({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5371= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Stmt*
_temp5377; _LL5373: if(*(( void**) _temp5371) ==  Cyc_Stmt_tok){ _LL5378:
_temp5377=(( struct Cyc_Stmt_tok_struct*) _temp5371)->f1; goto _LL5374;} else{
goto _LL5375;} _LL5375: goto _LL5376; _LL5374: yyzzz= _temp5377; goto _LL5372;
_LL5376:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5372:;} yyzzz;});
_temp5370->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp5370;}); _temp5361->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5362=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5368; _LL5364: if(*(( void**) _temp5362) ==  Cyc_SwitchClauseList_tok){
_LL5369: _temp5368=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5362)->f1;
goto _LL5365;} else{ goto _LL5366;} _LL5366: goto _LL5367; _LL5365: yyzzz=
_temp5368; goto _LL5363; _LL5367:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5363:;} yyzzz;}); _temp5361;}); _temp5360;}); _temp5359;}); break; case 252:
_LL5358: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5388=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5388[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5389; _temp5389.tag=
Cyc_SwitchClauseList_tok; _temp5389.f1=({ struct Cyc_List_List* _temp5390=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5390->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5399=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5399->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5408= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Pat*
_temp5414; _LL5410: if(*(( void**) _temp5408) ==  Cyc_Pattern_tok){ _LL5415:
_temp5414=(( struct Cyc_Pattern_tok_struct*) _temp5408)->f1; goto _LL5411;}
else{ goto _LL5412;} _LL5412: goto _LL5413; _LL5411: yyzzz= _temp5414; goto
_LL5409; _LL5413:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5409:;}
yyzzz;}); _temp5399->pat_vars= 0; _temp5399->where_clause=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5400= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5406; _LL5402: if(*(( void**) _temp5400) ==  Cyc_Exp_tok){ _LL5407:
_temp5406=(( struct Cyc_Exp_tok_struct*) _temp5400)->f1; goto _LL5403;} else{
goto _LL5404;} _LL5404: goto _LL5405; _LL5403: yyzzz= _temp5406; goto _LL5401;
_LL5405:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5401:;} yyzzz;});
_temp5399->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5399->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5399;});
_temp5390->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5391= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5397; _LL5393: if(*(( void**) _temp5391) ==  Cyc_SwitchClauseList_tok){
_LL5398: _temp5397=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5391)->f1;
goto _LL5394;} else{ goto _LL5395;} _LL5395: goto _LL5396; _LL5394: yyzzz=
_temp5397; goto _LL5392; _LL5396:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5392:;} yyzzz;}); _temp5390;}); _temp5389;}); _temp5388;}); break; case 253:
_LL5387: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5417=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5417[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5418; _temp5418.tag=
Cyc_SwitchClauseList_tok; _temp5418.f1=({ struct Cyc_List_List* _temp5419=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5419->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5428=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5428->pattern=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp5445= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Pat*
_temp5451; _LL5447: if(*(( void**) _temp5445) ==  Cyc_Pattern_tok){ _LL5452:
_temp5451=(( struct Cyc_Pattern_tok_struct*) _temp5445)->f1; goto _LL5448;}
else{ goto _LL5449;} _LL5449: goto _LL5450; _LL5448: yyzzz= _temp5451; goto
_LL5446; _LL5450:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL5446:;}
yyzzz;}); _temp5428->pat_vars= 0; _temp5428->where_clause=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5437= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5443; _LL5439: if(*(( void**) _temp5437) ==  Cyc_Exp_tok){ _LL5444:
_temp5443=(( struct Cyc_Exp_tok_struct*) _temp5437)->f1; goto _LL5440;} else{
goto _LL5441;} _LL5441: goto _LL5442; _LL5440: yyzzz= _temp5443; goto _LL5438;
_LL5442:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5438:;} yyzzz;});
_temp5428->body=({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5429= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Stmt*
_temp5435; _LL5431: if(*(( void**) _temp5429) ==  Cyc_Stmt_tok){ _LL5436:
_temp5435=(( struct Cyc_Stmt_tok_struct*) _temp5429)->f1; goto _LL5432;} else{
goto _LL5433;} _LL5433: goto _LL5434; _LL5432: yyzzz= _temp5435; goto _LL5430;
_LL5434:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5430:;} yyzzz;});
_temp5428->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5428;});
_temp5419->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5420= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5426; _LL5422: if(*(( void**) _temp5420) ==  Cyc_SwitchClauseList_tok){
_LL5427: _temp5426=(( struct Cyc_SwitchClauseList_tok_struct*) _temp5420)->f1;
goto _LL5423;} else{ goto _LL5424;} _LL5424: goto _LL5425; _LL5423: yyzzz=
_temp5426; goto _LL5421; _LL5425:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL5421:;} yyzzz;}); _temp5419;}); _temp5418;}); _temp5417;}); break; case 254:
_LL5416: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp5454=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp5454[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp5455; _temp5455.tag=
Cyc_SwitchCClauseList_tok; _temp5455.f1= 0; _temp5455;}); _temp5454;}); break;
case 255: _LL5453: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp5457=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp5457[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp5458; _temp5458.tag= Cyc_SwitchCClauseList_tok; _temp5458.f1=({ struct Cyc_List_List*
_temp5459=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5459->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp5460=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp5460->cnst_exp= 0;
_temp5460->body= Cyc_Absyn_seq_stmt(({ struct Cyc_Absyn_Stmt* yyzzz;{ void*
_temp5461= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Stmt* _temp5467; _LL5463: if(*(( void**) _temp5461) ==  Cyc_Stmt_tok){
_LL5468: _temp5467=(( struct Cyc_Stmt_tok_struct*) _temp5461)->f1; goto _LL5464;}
else{ goto _LL5465;} _LL5465: goto _LL5466; _LL5464: yyzzz= _temp5467; goto
_LL5462; _LL5466:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5462:;} yyzzz;}),
Cyc_Absyn_break_stmt( 0), 0); _temp5460->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp5460;});
_temp5459->tl= 0; _temp5459;}); _temp5458;}); _temp5457;}); break; case 256:
_LL5456: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp5470=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp5470[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp5471; _temp5471.tag=
Cyc_SwitchCClauseList_tok; _temp5471.f1=({ struct Cyc_List_List* _temp5472=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5472->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp5481=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp5481->cnst_exp=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5482= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5488; _LL5484: if(*(( void**) _temp5482) ==  Cyc_Exp_tok){ _LL5489:
_temp5488=(( struct Cyc_Exp_tok_struct*) _temp5482)->f1; goto _LL5485;} else{
goto _LL5486;} _LL5486: goto _LL5487; _LL5485: yyzzz= _temp5488; goto _LL5483;
_LL5487:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5483:;} yyzzz;});
_temp5481->body= Cyc_Absyn_fallthru_stmt( 0, 0); _temp5481->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp5481;}); _temp5472->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5473=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5479; _LL5475: if(*(( void**) _temp5473) ==  Cyc_SwitchCClauseList_tok){
_LL5480: _temp5479=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp5473)->f1;
goto _LL5476;} else{ goto _LL5477;} _LL5477: goto _LL5478; _LL5476: yyzzz=
_temp5479; goto _LL5474; _LL5478:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL5474:;} yyzzz;}); _temp5472;}); _temp5471;}); _temp5470;}); break; case 257:
_LL5469: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp5491=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp5491[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp5492; _temp5492.tag=
Cyc_SwitchCClauseList_tok; _temp5492.f1=({ struct Cyc_List_List* _temp5493=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5493->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp5502=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp5502->cnst_exp=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5511= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5517; _LL5513: if(*(( void**) _temp5511) ==  Cyc_Exp_tok){ _LL5518:
_temp5517=(( struct Cyc_Exp_tok_struct*) _temp5511)->f1; goto _LL5514;} else{
goto _LL5515;} _LL5515: goto _LL5516; _LL5514: yyzzz= _temp5517; goto _LL5512;
_LL5516:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5512:;} yyzzz;});
_temp5502->body= Cyc_Absyn_seq_stmt(({ struct Cyc_Absyn_Stmt* yyzzz;{ void*
_temp5503= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_Absyn_Stmt* _temp5509; _LL5505: if(*(( void**) _temp5503) ==  Cyc_Stmt_tok){
_LL5510: _temp5509=(( struct Cyc_Stmt_tok_struct*) _temp5503)->f1; goto _LL5506;}
else{ goto _LL5507;} _LL5507: goto _LL5508; _LL5506: yyzzz= _temp5509; goto
_LL5504; _LL5508:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5504:;} yyzzz;}),
Cyc_Absyn_fallthru_stmt( 0, 0), 0); _temp5502->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp5502;}); _temp5493->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp5494=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp5500; _LL5496: if(*(( void**) _temp5494) ==  Cyc_SwitchCClauseList_tok){
_LL5501: _temp5500=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp5494)->f1;
goto _LL5497;} else{ goto _LL5498;} _LL5498: goto _LL5499; _LL5497: yyzzz=
_temp5500; goto _LL5495; _LL5499:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL5495:;} yyzzz;}); _temp5493;}); _temp5492;}); _temp5491;}); break; case 258:
_LL5490: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5520=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5520[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5521; _temp5521.tag= Cyc_Stmt_tok; _temp5521.f1= Cyc_Absyn_while_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5522= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5528; _LL5524: if(*(( void**) _temp5522) ==  Cyc_Exp_tok){ _LL5529:
_temp5528=(( struct Cyc_Exp_tok_struct*) _temp5522)->f1; goto _LL5525;} else{
goto _LL5526;} _LL5526: goto _LL5527; _LL5525: yyzzz= _temp5528; goto _LL5523;
_LL5527:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5523:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5530= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5536; _LL5532: if(*(( void**) _temp5530) ==  Cyc_Stmt_tok){ _LL5537:
_temp5536=(( struct Cyc_Stmt_tok_struct*) _temp5530)->f1; goto _LL5533;} else{
goto _LL5534;} _LL5534: goto _LL5535; _LL5533: yyzzz= _temp5536; goto _LL5531;
_LL5535:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5531:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5521;}); _temp5520;}); break; case 259: _LL5519: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5539=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5539[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5540; _temp5540.tag= Cyc_Stmt_tok; _temp5540.f1= Cyc_Absyn_do_stmt(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5541= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Stmt*
_temp5547; _LL5543: if(*(( void**) _temp5541) ==  Cyc_Stmt_tok){ _LL5548:
_temp5547=(( struct Cyc_Stmt_tok_struct*) _temp5541)->f1; goto _LL5544;} else{
goto _LL5545;} _LL5545: goto _LL5546; _LL5544: yyzzz= _temp5547; goto _LL5542;
_LL5546:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5542:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5549= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5555; _LL5551: if(*(( void**) _temp5549) ==  Cyc_Exp_tok){ _LL5556:
_temp5555=(( struct Cyc_Exp_tok_struct*) _temp5549)->f1; goto _LL5552;} else{
goto _LL5553;} _LL5553: goto _LL5554; _LL5552: yyzzz= _temp5555; goto _LL5550;
_LL5554:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5550:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5540;}); _temp5539;}); break; case 260: _LL5538: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5558=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5558[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5559; _temp5559.tag= Cyc_Stmt_tok; _temp5559.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0),({ struct Cyc_Absyn_Stmt*
yyzzz;{ void* _temp5560= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Stmt* _temp5566; _LL5562: if(*(( void**)
_temp5560) ==  Cyc_Stmt_tok){ _LL5567: _temp5566=(( struct Cyc_Stmt_tok_struct*)
_temp5560)->f1; goto _LL5563;} else{ goto _LL5564;} _LL5564: goto _LL5565;
_LL5563: yyzzz= _temp5566; goto _LL5561; _LL5565:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL5561:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5559;});
_temp5558;}); break; case 261: _LL5557: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5569=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5569[ 0]=({ struct Cyc_Stmt_tok_struct _temp5570; _temp5570.tag= Cyc_Stmt_tok;
_temp5570.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5571= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5577; _LL5573: if(*(( void**) _temp5571) ==  Cyc_Exp_tok){ _LL5578:
_temp5577=(( struct Cyc_Exp_tok_struct*) _temp5571)->f1; goto _LL5574;} else{
goto _LL5575;} _LL5575: goto _LL5576; _LL5574: yyzzz= _temp5577; goto _LL5572;
_LL5576:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5572:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5579= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5585; _LL5581: if(*(( void**) _temp5579) ==  Cyc_Stmt_tok){ _LL5586:
_temp5585=(( struct Cyc_Stmt_tok_struct*) _temp5579)->f1; goto _LL5582;} else{
goto _LL5583;} _LL5583: goto _LL5584; _LL5582: yyzzz= _temp5585; goto _LL5580;
_LL5584:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5580:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5570;}); _temp5569;}); break; case 262: _LL5568: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5588=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5588[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5589; _temp5589.tag= Cyc_Stmt_tok; _temp5589.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5590= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5596; _LL5592: if(*(( void**) _temp5590) ==  Cyc_Exp_tok){ _LL5597:
_temp5596=(( struct Cyc_Exp_tok_struct*) _temp5590)->f1; goto _LL5593;} else{
goto _LL5594;} _LL5594: goto _LL5595; _LL5593: yyzzz= _temp5596; goto _LL5591;
_LL5595:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5591:;} yyzzz;}), Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5598= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5604; _LL5600: if(*(( void**) _temp5598) ==  Cyc_Stmt_tok){ _LL5605:
_temp5604=(( struct Cyc_Stmt_tok_struct*) _temp5598)->f1; goto _LL5601;} else{
goto _LL5602;} _LL5602: goto _LL5603; _LL5601: yyzzz= _temp5604; goto _LL5599;
_LL5603:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5599:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5589;}); _temp5588;}); break; case 263: _LL5587: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5607=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5607[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5608; _temp5608.tag= Cyc_Stmt_tok; _temp5608.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5609= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5615; _LL5611: if(*(( void**) _temp5609) ==  Cyc_Exp_tok){ _LL5616:
_temp5615=(( struct Cyc_Exp_tok_struct*) _temp5609)->f1; goto _LL5612;} else{
goto _LL5613;} _LL5613: goto _LL5614; _LL5612: yyzzz= _temp5615; goto _LL5610;
_LL5614:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5610:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5617= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5623; _LL5619: if(*(( void**) _temp5617) ==  Cyc_Exp_tok){ _LL5624:
_temp5623=(( struct Cyc_Exp_tok_struct*) _temp5617)->f1; goto _LL5620;} else{
goto _LL5621;} _LL5621: goto _LL5622; _LL5620: yyzzz= _temp5623; goto _LL5618;
_LL5622:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5618:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5625= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5631; _LL5627: if(*(( void**) _temp5625) ==  Cyc_Stmt_tok){ _LL5632:
_temp5631=(( struct Cyc_Stmt_tok_struct*) _temp5625)->f1; goto _LL5628;} else{
goto _LL5629;} _LL5629: goto _LL5630; _LL5628: yyzzz= _temp5631; goto _LL5626;
_LL5630:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5626:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5608;}); _temp5607;}); break; case 264: _LL5606: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5634=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5634[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5635; _temp5635.tag= Cyc_Stmt_tok; _temp5635.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5636= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5642; _LL5638: if(*(( void**) _temp5636) ==  Cyc_Exp_tok){ _LL5643:
_temp5642=(( struct Cyc_Exp_tok_struct*) _temp5636)->f1; goto _LL5639;} else{
goto _LL5640;} _LL5640: goto _LL5641; _LL5639: yyzzz= _temp5642; goto _LL5637;
_LL5641:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5637:;} yyzzz;}), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5644=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5650; _LL5646: if(*(( void**) _temp5644) ==  Cyc_Stmt_tok){ _LL5651:
_temp5650=(( struct Cyc_Stmt_tok_struct*) _temp5644)->f1; goto _LL5647;} else{
goto _LL5648;} _LL5648: goto _LL5649; _LL5647: yyzzz= _temp5650; goto _LL5645;
_LL5649:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5645:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5635;}); _temp5634;}); break; case 265: _LL5633: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5653=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5653[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5654; _temp5654.tag= Cyc_Stmt_tok; _temp5654.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5655= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Exp*
_temp5661; _LL5657: if(*(( void**) _temp5655) ==  Cyc_Exp_tok){ _LL5662:
_temp5661=(( struct Cyc_Exp_tok_struct*) _temp5655)->f1; goto _LL5658;} else{
goto _LL5659;} _LL5659: goto _LL5660; _LL5658: yyzzz= _temp5661; goto _LL5656;
_LL5660:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5656:;} yyzzz;}), Cyc_Absyn_true_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5663= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5669; _LL5665: if(*(( void**) _temp5663) ==  Cyc_Exp_tok){ _LL5670:
_temp5669=(( struct Cyc_Exp_tok_struct*) _temp5663)->f1; goto _LL5666;} else{
goto _LL5667;} _LL5667: goto _LL5668; _LL5666: yyzzz= _temp5669; goto _LL5664;
_LL5668:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5664:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5671= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5677; _LL5673: if(*(( void**) _temp5671) ==  Cyc_Stmt_tok){ _LL5678:
_temp5677=(( struct Cyc_Stmt_tok_struct*) _temp5671)->f1; goto _LL5674;} else{
goto _LL5675;} _LL5675: goto _LL5676; _LL5674: yyzzz= _temp5677; goto _LL5672;
_LL5676:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5672:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5654;}); _temp5653;}); break; case 266: _LL5652: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5680=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5680[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5681; _temp5681.tag= Cyc_Stmt_tok; _temp5681.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5682= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_Absyn_Exp*
_temp5688; _LL5684: if(*(( void**) _temp5682) ==  Cyc_Exp_tok){ _LL5689:
_temp5688=(( struct Cyc_Exp_tok_struct*) _temp5682)->f1; goto _LL5685;} else{
goto _LL5686;} _LL5686: goto _LL5687; _LL5685: yyzzz= _temp5688; goto _LL5683;
_LL5687:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5683:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5690= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5696; _LL5692: if(*(( void**) _temp5690) ==  Cyc_Exp_tok){ _LL5697:
_temp5696=(( struct Cyc_Exp_tok_struct*) _temp5690)->f1; goto _LL5693;} else{
goto _LL5694;} _LL5694: goto _LL5695; _LL5693: yyzzz= _temp5696; goto _LL5691;
_LL5695:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5691:;} yyzzz;}), Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5698= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5704; _LL5700: if(*(( void**) _temp5698) ==  Cyc_Stmt_tok){ _LL5705:
_temp5704=(( struct Cyc_Stmt_tok_struct*) _temp5698)->f1; goto _LL5701;} else{
goto _LL5702;} _LL5702: goto _LL5703; _LL5701: yyzzz= _temp5704; goto _LL5699;
_LL5703:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5699:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5681;}); _temp5680;}); break; case 267: _LL5679: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5707=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5707[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5708; _temp5708.tag= Cyc_Stmt_tok; _temp5708.f1= Cyc_Absyn_for_stmt(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5709= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]; struct Cyc_Absyn_Exp*
_temp5715; _LL5711: if(*(( void**) _temp5709) ==  Cyc_Exp_tok){ _LL5716:
_temp5715=(( struct Cyc_Exp_tok_struct*) _temp5709)->f1; goto _LL5712;} else{
goto _LL5713;} _LL5713: goto _LL5714; _LL5712: yyzzz= _temp5715; goto _LL5710;
_LL5714:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5710:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5717= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5723; _LL5719: if(*(( void**) _temp5717) ==  Cyc_Exp_tok){ _LL5724:
_temp5723=(( struct Cyc_Exp_tok_struct*) _temp5717)->f1; goto _LL5720;} else{
goto _LL5721;} _LL5721: goto _LL5722; _LL5720: yyzzz= _temp5723; goto _LL5718;
_LL5722:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5718:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5725= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5731; _LL5727: if(*(( void**) _temp5725) ==  Cyc_Exp_tok){ _LL5732:
_temp5731=(( struct Cyc_Exp_tok_struct*) _temp5725)->f1; goto _LL5728;} else{
goto _LL5729;} _LL5729: goto _LL5730; _LL5728: yyzzz= _temp5731; goto _LL5726;
_LL5730:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5726:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5733= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5739; _LL5735: if(*(( void**) _temp5733) ==  Cyc_Stmt_tok){ _LL5740:
_temp5739=(( struct Cyc_Stmt_tok_struct*) _temp5733)->f1; goto _LL5736;} else{
goto _LL5737;} _LL5737: goto _LL5738; _LL5736: yyzzz= _temp5739; goto _LL5734;
_LL5738:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5734:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5708;}); _temp5707;}); break; case 268: _LL5706: { struct Cyc_List_List*
_temp5742=({ struct Cyc_List_List* yyzzz;{ void* _temp5754= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp5760; _LL5756: if(*(( void**) _temp5754) ==  Cyc_DeclList_tok){ _LL5761:
_temp5760=(( struct Cyc_DeclList_tok_struct*) _temp5754)->f1; goto _LL5757;}
else{ goto _LL5758;} _LL5758: goto _LL5759; _LL5757: yyzzz= _temp5760; goto
_LL5755; _LL5759:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5755:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5743= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0),({ struct Cyc_Absyn_Stmt*
yyzzz;{ void* _temp5746= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Stmt* _temp5752; _LL5748: if(*(( void**)
_temp5746) ==  Cyc_Stmt_tok){ _LL5753: _temp5752=(( struct Cyc_Stmt_tok_struct*)
_temp5746)->f1; goto _LL5749;} else{ goto _LL5750;} _LL5750: goto _LL5751;
_LL5749: yyzzz= _temp5752; goto _LL5747; _LL5751:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL5747:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5744=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5744[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5745; _temp5745.tag= Cyc_Stmt_tok; _temp5745.f1= Cyc_Parse_flatten_declarations(
_temp5742, _temp5743); _temp5745;}); _temp5744;}); break;} case 269: _LL5741: {
struct Cyc_List_List* _temp5763=({ struct Cyc_List_List* yyzzz;{ void* _temp5783=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_List_List*
_temp5789; _LL5785: if(*(( void**) _temp5783) ==  Cyc_DeclList_tok){ _LL5790:
_temp5789=(( struct Cyc_DeclList_tok_struct*) _temp5783)->f1; goto _LL5786;}
else{ goto _LL5787;} _LL5787: goto _LL5788; _LL5786: yyzzz= _temp5789; goto
_LL5784; _LL5788:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5784:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5764= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5767= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp5773; _LL5769: if(*(( void**) _temp5767) ==  Cyc_Exp_tok){ _LL5774:
_temp5773=(( struct Cyc_Exp_tok_struct*) _temp5767)->f1; goto _LL5770;} else{
goto _LL5771;} _LL5771: goto _LL5772; _LL5770: yyzzz= _temp5773; goto _LL5768;
_LL5772:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5768:;} yyzzz;}), Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5775= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5781; _LL5777: if(*(( void**) _temp5775) ==  Cyc_Stmt_tok){ _LL5782:
_temp5781=(( struct Cyc_Stmt_tok_struct*) _temp5775)->f1; goto _LL5778;} else{
goto _LL5779;} _LL5779: goto _LL5780; _LL5778: yyzzz= _temp5781; goto _LL5776;
_LL5780:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5776:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5765=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5765[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5766; _temp5766.tag= Cyc_Stmt_tok; _temp5766.f1= Cyc_Parse_flatten_declarations(
_temp5763, _temp5764); _temp5766;}); _temp5765;}); break;} case 270: _LL5762: {
struct Cyc_List_List* _temp5792=({ struct Cyc_List_List* yyzzz;{ void* _temp5812=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_List_List*
_temp5818; _LL5814: if(*(( void**) _temp5812) ==  Cyc_DeclList_tok){ _LL5819:
_temp5818=(( struct Cyc_DeclList_tok_struct*) _temp5812)->f1; goto _LL5815;}
else{ goto _LL5816;} _LL5816: goto _LL5817; _LL5815: yyzzz= _temp5818; goto
_LL5813; _LL5817:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5813:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5793= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5796=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5802; _LL5798: if(*(( void**) _temp5796) ==  Cyc_Exp_tok){ _LL5803:
_temp5802=(( struct Cyc_Exp_tok_struct*) _temp5796)->f1; goto _LL5799;} else{
goto _LL5800;} _LL5800: goto _LL5801; _LL5799: yyzzz= _temp5802; goto _LL5797;
_LL5801:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5797:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5804= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5810; _LL5806: if(*(( void**) _temp5804) ==  Cyc_Stmt_tok){ _LL5811:
_temp5810=(( struct Cyc_Stmt_tok_struct*) _temp5804)->f1; goto _LL5807;} else{
goto _LL5808;} _LL5808: goto _LL5809; _LL5807: yyzzz= _temp5810; goto _LL5805;
_LL5809:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5805:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5794=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5794[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5795; _temp5795.tag= Cyc_Stmt_tok; _temp5795.f1= Cyc_Parse_flatten_declarations(
_temp5792, _temp5793); _temp5795;}); _temp5794;}); break;} case 271: _LL5791: {
struct Cyc_List_List* _temp5821=({ struct Cyc_List_List* yyzzz;{ void* _temp5849=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct Cyc_List_List*
_temp5855; _LL5851: if(*(( void**) _temp5849) ==  Cyc_DeclList_tok){ _LL5856:
_temp5855=(( struct Cyc_DeclList_tok_struct*) _temp5849)->f1; goto _LL5852;}
else{ goto _LL5853;} _LL5853: goto _LL5854; _LL5852: yyzzz= _temp5855; goto
_LL5850; _LL5854:( int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL5850:;}
yyzzz;}); struct Cyc_Absyn_Stmt* _temp5822= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5825= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp5831; _LL5827: if(*(( void**) _temp5825) ==  Cyc_Exp_tok){ _LL5832:
_temp5831=(( struct Cyc_Exp_tok_struct*) _temp5825)->f1; goto _LL5828;} else{
goto _LL5829;} _LL5829: goto _LL5830; _LL5828: yyzzz= _temp5831; goto _LL5826;
_LL5830:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5826:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5833= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp5839; _LL5835: if(*(( void**) _temp5833) ==  Cyc_Exp_tok){ _LL5840:
_temp5839=(( struct Cyc_Exp_tok_struct*) _temp5833)->f1; goto _LL5836;} else{
goto _LL5837;} _LL5837: goto _LL5838; _LL5836: yyzzz= _temp5839; goto _LL5834;
_LL5838:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5834:;} yyzzz;}),({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp5841= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Stmt*
_temp5847; _LL5843: if(*(( void**) _temp5841) ==  Cyc_Stmt_tok){ _LL5848:
_temp5847=(( struct Cyc_Stmt_tok_struct*) _temp5841)->f1; goto _LL5844;} else{
goto _LL5845;} _LL5845: goto _LL5846; _LL5844: yyzzz= _temp5847; goto _LL5842;
_LL5846:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL5842:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp5823=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5823[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5824; _temp5824.tag= Cyc_Stmt_tok; _temp5824.f1= Cyc_Parse_flatten_declarations(
_temp5821, _temp5822); _temp5824;}); _temp5823;}); break;} case 272: _LL5820:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5858=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5858[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5859; _temp5859.tag= Cyc_Stmt_tok; _temp5859.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp5860=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp5860[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp5861= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct _tagged_arr
_temp5867; _LL5863: if(*(( void**) _temp5861) ==  Cyc_String_tok){ _LL5868:
_temp5867=(( struct Cyc_String_tok_struct*) _temp5861)->f1; goto _LL5864;} else{
goto _LL5865;} _LL5865: goto _LL5866; _LL5864: yyzzz= _temp5867; goto _LL5862;
_LL5866:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL5862:;} yyzzz;});
_temp5860;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)); _temp5859;}); _temp5858;}); break;
case 273: _LL5857: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5870=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5870[ 0]=({ struct Cyc_Stmt_tok_struct _temp5871; _temp5871.tag= Cyc_Stmt_tok;
_temp5871.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5871;}); _temp5870;}); break; case 274: _LL5869: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5873=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5873[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5874; _temp5874.tag= Cyc_Stmt_tok; _temp5874.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5874;}); _temp5873;}); break; case 275: _LL5872: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5876=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5876[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5877; _temp5877.tag= Cyc_Stmt_tok; _temp5877.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp5877;}); _temp5876;}); break; case 276:
_LL5875: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5879=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5879[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5880; _temp5880.tag= Cyc_Stmt_tok; _temp5880.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp5881= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp5887; _LL5883: if(*(( void**) _temp5881) ==  Cyc_Exp_tok){ _LL5888:
_temp5887=(( struct Cyc_Exp_tok_struct*) _temp5881)->f1; goto _LL5884;} else{
goto _LL5885;} _LL5885: goto _LL5886; _LL5884: yyzzz= _temp5887; goto _LL5882;
_LL5886:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL5882:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp5880;}); _temp5879;}); break; case 277: _LL5878: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5890=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5890[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5891; _temp5891.tag= Cyc_Stmt_tok; _temp5891.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp5891;}); _temp5890;}); break; case 278:
_LL5889: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5893=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5893[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5894; _temp5894.tag= Cyc_Stmt_tok; _temp5894.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp5894;}); _temp5893;}); break; case 279:
_LL5892: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5896=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5896[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5897; _temp5897.tag= Cyc_Stmt_tok; _temp5897.f1= Cyc_Absyn_fallthru_stmt(({
struct Cyc_List_List* yyzzz;{ void* _temp5898= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp5904; _LL5900: if(*(( void**) _temp5898) ==  Cyc_ExpList_tok){ _LL5905:
_temp5904=(( struct Cyc_ExpList_tok_struct*) _temp5898)->f1; goto _LL5901;}
else{ goto _LL5902;} _LL5902: goto _LL5903; _LL5901: yyzzz= _temp5904; goto
_LL5899; _LL5903:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL5899:;}
yyzzz;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)); _temp5897;}); _temp5896;}); break;
case 280: _LL5895: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5907=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5907[ 0]=({ struct Cyc_Pattern_tok_struct _temp5908; _temp5908.tag= Cyc_Pattern_tok;
_temp5908.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5908;});
_temp5907;}); break; case 281: _LL5906: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 282:
_LL5909: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5911=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5911[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5912; _temp5912.tag= Cyc_Pattern_tok; _temp5912.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5913=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5913[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5914; _temp5914.tag= Cyc_Absyn_Int_p; _temp5914.f1=( void*)(*({ struct
_tuple15* yyzzz;{ void* _temp5915= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple15* _temp5921; _LL5917: if(*(( void**) _temp5915)
==  Cyc_Int_tok){ _LL5922: _temp5921=(( struct Cyc_Int_tok_struct*) _temp5915)->f1;
goto _LL5918;} else{ goto _LL5919;} _LL5919: goto _LL5920; _LL5918: yyzzz=
_temp5921; goto _LL5916; _LL5920:( int) _throw(( void*)& Cyc_yyfail_Int_tok);
_LL5916:;} yyzzz;})).f1; _temp5914.f2=(*({ struct _tuple15* yyzzz;{ void*
_temp5923= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
_tuple15* _temp5929; _LL5925: if(*(( void**) _temp5923) ==  Cyc_Int_tok){
_LL5930: _temp5929=(( struct Cyc_Int_tok_struct*) _temp5923)->f1; goto _LL5926;}
else{ goto _LL5927;} _LL5927: goto _LL5928; _LL5926: yyzzz= _temp5929; goto
_LL5924; _LL5928:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL5924:;} yyzzz;})).f2;
_temp5914;}); _temp5913;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5912;});
_temp5911;}); break; case 283: _LL5910: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5932=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5932[ 0]=({ struct Cyc_Pattern_tok_struct _temp5933; _temp5933.tag= Cyc_Pattern_tok;
_temp5933.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5934=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5934[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5935; _temp5935.tag= Cyc_Absyn_Int_p;
_temp5935.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp5935.f2= -(*({ struct
_tuple15* yyzzz;{ void* _temp5936= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple15* _temp5942; _LL5938: if(*(( void**) _temp5936)
==  Cyc_Int_tok){ _LL5943: _temp5942=(( struct Cyc_Int_tok_struct*) _temp5936)->f1;
goto _LL5939;} else{ goto _LL5940;} _LL5940: goto _LL5941; _LL5939: yyzzz=
_temp5942; goto _LL5937; _LL5941:( int) _throw(( void*)& Cyc_yyfail_Int_tok);
_LL5937:;} yyzzz;})).f2; _temp5935;}); _temp5934;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5933;}); _temp5932;}); break; case 284: _LL5931: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp5945=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp5945[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5946; _temp5946.tag= Cyc_Pattern_tok; _temp5946.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp5947=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5947[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp5948; _temp5948.tag= Cyc_Absyn_Float_p; _temp5948.f1=({
struct _tagged_arr yyzzz;{ void* _temp5949= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp5955; _LL5951: if(*(( void**)
_temp5949) ==  Cyc_String_tok){ _LL5956: _temp5955=(( struct Cyc_String_tok_struct*)
_temp5949)->f1; goto _LL5952;} else{ goto _LL5953;} _LL5953: goto _LL5954;
_LL5952: yyzzz= _temp5955; goto _LL5950; _LL5954:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL5950:;} yyzzz;}); _temp5948;}); _temp5947;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5946;});
_temp5945;}); break; case 285: _LL5944: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5958=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5958[ 0]=({ struct Cyc_Pattern_tok_struct _temp5959; _temp5959.tag= Cyc_Pattern_tok;
_temp5959.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp5960=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp5960[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp5961; _temp5961.tag= Cyc_Absyn_Char_p; _temp5961.f1=({ unsigned char yyzzz;{
void* _temp5962= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
unsigned char _temp5968; _LL5964: if(*(( void**) _temp5962) ==  Cyc_Char_tok){
_LL5969: _temp5968=(( struct Cyc_Char_tok_struct*) _temp5962)->f1; goto _LL5965;}
else{ goto _LL5966;} _LL5966: goto _LL5967; _LL5965: yyzzz= _temp5968; goto
_LL5963; _LL5967:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL5963:;} yyzzz;});
_temp5961;}); _temp5960;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5959;});
_temp5958;}); break; case 286: _LL5957: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5971=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5971[ 0]=({ struct Cyc_Pattern_tok_struct _temp5972; _temp5972.tag= Cyc_Pattern_tok;
_temp5972.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5972;});
_temp5971;}); break; case 287: _LL5970: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp5974=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5974[ 0]=({ struct Cyc_Pattern_tok_struct _temp5975; _temp5975.tag= Cyc_Pattern_tok;
_temp5975.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp5976=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp5976[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp5977; _temp5977.tag=
Cyc_Absyn_UnknownId_p; _temp5977.f1=({ struct _tuple1* yyzzz;{ void* _temp5978=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp5984; _LL5980: if(*(( void**) _temp5978) ==  Cyc_QualId_tok){ _LL5985:
_temp5984=(( struct Cyc_QualId_tok_struct*) _temp5978)->f1; goto _LL5981;} else{
goto _LL5982;} _LL5982: goto _LL5983; _LL5981: yyzzz= _temp5984; goto _LL5979;
_LL5983:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL5979:;} yyzzz;});
_temp5977;}); _temp5976;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp5975;});
_temp5974;}); break; case 288: _LL5973: { struct Cyc_List_List* _temp5987=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp6008= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp6014; _LL6010: if(*(( void**) _temp6008) ==  Cyc_TypeList_tok){ _LL6015:
_temp6014=(( struct Cyc_TypeList_tok_struct*) _temp6008)->f1; goto _LL6011;}
else{ goto _LL6012;} _LL6012: goto _LL6013; _LL6011: yyzzz= _temp6014; goto
_LL6009; _LL6013:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL6009:;}
yyzzz;})); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5988=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5988[ 0]=({ struct Cyc_Pattern_tok_struct _temp5989; _temp5989.tag= Cyc_Pattern_tok;
_temp5989.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp5990=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp5990[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp5991; _temp5991.tag= Cyc_Absyn_UnknownCall_p; _temp5991.f1=({ struct
_tuple1* yyzzz;{ void* _temp5992= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp5998; _LL5994: if(*(( void**)
_temp5992) ==  Cyc_QualId_tok){ _LL5999: _temp5998=(( struct Cyc_QualId_tok_struct*)
_temp5992)->f1; goto _LL5995;} else{ goto _LL5996;} _LL5996: goto _LL5997;
_LL5995: yyzzz= _temp5998; goto _LL5993; _LL5997:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL5993:;} yyzzz;}); _temp5991.f2= _temp5987; _temp5991.f3=({ struct Cyc_List_List*
yyzzz;{ void* _temp6000= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp6006; _LL6002: if(*(( void**)
_temp6000) ==  Cyc_PatternList_tok){ _LL6007: _temp6006=(( struct Cyc_PatternList_tok_struct*)
_temp6000)->f1; goto _LL6003;} else{ goto _LL6004;} _LL6004: goto _LL6005;
_LL6003: yyzzz= _temp6006; goto _LL6001; _LL6005:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL6001:;} yyzzz;}); _temp5991;}); _temp5990;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp5989;}); _temp5988;}); break;} case 289: _LL5986: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6017=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6017[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6018; _temp6018.tag= Cyc_Pattern_tok; _temp6018.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6019=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6019[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6020; _temp6020.tag= Cyc_Absyn_Tuple_p; _temp6020.f1=({
struct Cyc_List_List* yyzzz;{ void* _temp6021= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp6027; _LL6023: if(*(( void**) _temp6021) ==  Cyc_PatternList_tok){ _LL6028:
_temp6027=(( struct Cyc_PatternList_tok_struct*) _temp6021)->f1; goto _LL6024;}
else{ goto _LL6025;} _LL6025: goto _LL6026; _LL6024: yyzzz= _temp6027; goto
_LL6022; _LL6026:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok); _LL6022:;}
yyzzz;}); _temp6020;}); _temp6019;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6018;});
_temp6017;}); break; case 290: _LL6016: { struct Cyc_List_List* _temp6030=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp6043= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp6049; _LL6045: if(*(( void**) _temp6043) ==  Cyc_TypeList_tok){ _LL6050:
_temp6049=(( struct Cyc_TypeList_tok_struct*) _temp6043)->f1; goto _LL6046;}
else{ goto _LL6047;} _LL6047: goto _LL6048; _LL6046: yyzzz= _temp6049; goto
_LL6044; _LL6048:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL6044:;}
yyzzz;})); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp6031=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6031[ 0]=({ struct Cyc_Pattern_tok_struct _temp6032; _temp6032.tag= Cyc_Pattern_tok;
_temp6032.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6033=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6033[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6034; _temp6034.tag= Cyc_Absyn_UnknownFields_p; _temp6034.f1=({ struct
_tuple1* yyzzz;{ void* _temp6035= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]; struct _tuple1* _temp6041; _LL6037: if(*(( void**)
_temp6035) ==  Cyc_QualId_tok){ _LL6042: _temp6041=(( struct Cyc_QualId_tok_struct*)
_temp6035)->f1; goto _LL6038;} else{ goto _LL6039;} _LL6039: goto _LL6040;
_LL6038: yyzzz= _temp6041; goto _LL6036; _LL6040:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL6036:;} yyzzz;}); _temp6034.f2= _temp6030; _temp6034.f3= 0; _temp6034;});
_temp6033;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp6032;}); _temp6031;}); break;} case
291: _LL6029: { struct Cyc_List_List* _temp6052=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),({
struct Cyc_List_List* yyzzz;{ void* _temp6073= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_List_List*
_temp6079; _LL6075: if(*(( void**) _temp6073) ==  Cyc_TypeList_tok){ _LL6080:
_temp6079=(( struct Cyc_TypeList_tok_struct*) _temp6073)->f1; goto _LL6076;}
else{ goto _LL6077;} _LL6077: goto _LL6078; _LL6076: yyzzz= _temp6079; goto
_LL6074; _LL6078:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL6074:;}
yyzzz;})); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp6053=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6053[ 0]=({ struct Cyc_Pattern_tok_struct _temp6054; _temp6054.tag= Cyc_Pattern_tok;
_temp6054.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6055=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6055[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6056; _temp6056.tag= Cyc_Absyn_UnknownFields_p; _temp6056.f1=({ struct
_tuple1* yyzzz;{ void* _temp6057= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]; struct _tuple1* _temp6063; _LL6059: if(*(( void**)
_temp6057) ==  Cyc_QualId_tok){ _LL6064: _temp6063=(( struct Cyc_QualId_tok_struct*)
_temp6057)->f1; goto _LL6060;} else{ goto _LL6061;} _LL6061: goto _LL6062;
_LL6060: yyzzz= _temp6063; goto _LL6058; _LL6062:( int) _throw(( void*)& Cyc_yyfail_QualId_tok);
_LL6058:;} yyzzz;}); _temp6056.f2= _temp6052; _temp6056.f3=({ struct Cyc_List_List*
yyzzz;{ void* _temp6065= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_List_List* _temp6071; _LL6067: if(*(( void**)
_temp6065) ==  Cyc_FieldPatternList_tok){ _LL6072: _temp6071=(( struct Cyc_FieldPatternList_tok_struct*)
_temp6065)->f1; goto _LL6068;} else{ goto _LL6069;} _LL6069: goto _LL6070;
_LL6068: yyzzz= _temp6071; goto _LL6066; _LL6070:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL6066:;} yyzzz;}); _temp6056;}); _temp6055;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6054;}); _temp6053;}); break;} case 292: _LL6051: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6082=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6082[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6083; _temp6083.tag= Cyc_Pattern_tok; _temp6083.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6084=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6084[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6085; _temp6085.tag= Cyc_Absyn_Pointer_p;
_temp6085.f1=({ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp6086= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Pat*
_temp6092; _LL6088: if(*(( void**) _temp6086) ==  Cyc_Pattern_tok){ _LL6093:
_temp6092=(( struct Cyc_Pattern_tok_struct*) _temp6086)->f1; goto _LL6089;}
else{ goto _LL6090;} _LL6090: goto _LL6091; _LL6089: yyzzz= _temp6092; goto
_LL6087; _LL6091:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL6087:;}
yyzzz;}); _temp6085;}); _temp6084;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6083;});
_temp6082;}); break; case 293: _LL6081: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6095=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6095[ 0]=({ struct Cyc_Pattern_tok_struct _temp6096; _temp6096.tag= Cyc_Pattern_tok;
_temp6096.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp6097=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp6097[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp6098; _temp6098.tag=
Cyc_Absyn_Reference_p; _temp6098.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp6099=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6099->f1=(
void*) Cyc_Absyn_Loc_n; _temp6099->f2=({ struct _tagged_arr* _temp6100=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp6100[ 0]=({ struct
_tagged_arr yyzzz;{ void* _temp6101= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tagged_arr _temp6107; _LL6103: if(*(( void**) _temp6101)
==  Cyc_String_tok){ _LL6108: _temp6107=(( struct Cyc_String_tok_struct*)
_temp6101)->f1; goto _LL6104;} else{ goto _LL6105;} _LL6105: goto _LL6106;
_LL6104: yyzzz= _temp6107; goto _LL6102; _LL6106:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL6102:;} yyzzz;}); _temp6100;}); _temp6099;}),( void*) Cyc_Absyn_VoidType, 0);
_temp6098;}); _temp6097;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6096;});
_temp6095;}); break; case 294: _LL6094: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp6110=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6110[ 0]=({ struct Cyc_PatternList_tok_struct _temp6111; _temp6111.tag= Cyc_PatternList_tok;
_temp6111.f1= 0; _temp6111;}); _temp6110;}); break; case 295: _LL6109: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp6113=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6113[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6114; _temp6114.tag= Cyc_PatternList_tok;
_temp6114.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({
struct Cyc_List_List* yyzzz;{ void* _temp6115= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp6121; _LL6117: if(*(( void**) _temp6115) ==  Cyc_PatternList_tok){ _LL6122:
_temp6121=(( struct Cyc_PatternList_tok_struct*) _temp6115)->f1; goto _LL6118;}
else{ goto _LL6119;} _LL6119: goto _LL6120; _LL6118: yyzzz= _temp6121; goto
_LL6116; _LL6120:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok); _LL6116:;}
yyzzz;})); _temp6114;}); _temp6113;}); break; case 296: _LL6112: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp6124=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6124[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6125; _temp6125.tag= Cyc_PatternList_tok;
_temp6125.f1=({ struct Cyc_List_List* _temp6126=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6126->hd=( void*)({ struct Cyc_Absyn_Pat*
yyzzz;{ void* _temp6127= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Pat* _temp6133; _LL6129: if(*(( void**)
_temp6127) ==  Cyc_Pattern_tok){ _LL6134: _temp6133=(( struct Cyc_Pattern_tok_struct*)
_temp6127)->f1; goto _LL6130;} else{ goto _LL6131;} _LL6131: goto _LL6132;
_LL6130: yyzzz= _temp6133; goto _LL6128; _LL6132:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL6128:;} yyzzz;}); _temp6126->tl= 0; _temp6126;}); _temp6125;}); _temp6124;});
break; case 297: _LL6123: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp6136=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6136[ 0]=({ struct Cyc_PatternList_tok_struct _temp6137; _temp6137.tag= Cyc_PatternList_tok;
_temp6137.f1=({ struct Cyc_List_List* _temp6138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6138->hd=( void*)({ struct Cyc_Absyn_Pat*
yyzzz;{ void* _temp6147= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct Cyc_Absyn_Pat* _temp6153; _LL6149: if(*(( void**)
_temp6147) ==  Cyc_Pattern_tok){ _LL6154: _temp6153=(( struct Cyc_Pattern_tok_struct*)
_temp6147)->f1; goto _LL6150;} else{ goto _LL6151;} _LL6151: goto _LL6152;
_LL6150: yyzzz= _temp6153; goto _LL6148; _LL6152:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL6148:;} yyzzz;}); _temp6138->tl=({ struct Cyc_List_List* yyzzz;{ void*
_temp6139= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct Cyc_List_List* _temp6145; _LL6141: if(*(( void**) _temp6139) ==  Cyc_PatternList_tok){
_LL6146: _temp6145=(( struct Cyc_PatternList_tok_struct*) _temp6139)->f1; goto
_LL6142;} else{ goto _LL6143;} _LL6143: goto _LL6144; _LL6142: yyzzz= _temp6145;
goto _LL6140; _LL6144:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL6140:;} yyzzz;}); _temp6138;}); _temp6137;}); _temp6136;}); break; case 298:
_LL6135: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp6156=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6156[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6157; _temp6157.tag=
Cyc_FieldPattern_tok; _temp6157.f1=({ struct _tuple14* _temp6158=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp6158->f1= 0; _temp6158->f2=({
struct Cyc_Absyn_Pat* yyzzz;{ void* _temp6159= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Pat*
_temp6165; _LL6161: if(*(( void**) _temp6159) ==  Cyc_Pattern_tok){ _LL6166:
_temp6165=(( struct Cyc_Pattern_tok_struct*) _temp6159)->f1; goto _LL6162;}
else{ goto _LL6163;} _LL6163: goto _LL6164; _LL6162: yyzzz= _temp6165; goto
_LL6160; _LL6164:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL6160:;}
yyzzz;}); _temp6158;}); _temp6157;}); _temp6156;}); break; case 299: _LL6155:
yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp6168=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp6168[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp6169; _temp6169.tag= Cyc_FieldPattern_tok;
_temp6169.f1=({ struct _tuple14* _temp6170=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp6170->f1=({ struct Cyc_List_List* yyzzz;{ void*
_temp6179= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
struct Cyc_List_List* _temp6185; _LL6181: if(*(( void**) _temp6179) ==  Cyc_DesignatorList_tok){
_LL6186: _temp6185=(( struct Cyc_DesignatorList_tok_struct*) _temp6179)->f1;
goto _LL6182;} else{ goto _LL6183;} _LL6183: goto _LL6184; _LL6182: yyzzz=
_temp6185; goto _LL6180; _LL6184:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL6180:;} yyzzz;}); _temp6170->f2=({ struct Cyc_Absyn_Pat* yyzzz;{ void*
_temp6171= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Pat* _temp6177; _LL6173: if(*(( void**) _temp6171) ==  Cyc_Pattern_tok){
_LL6178: _temp6177=(( struct Cyc_Pattern_tok_struct*) _temp6171)->f1; goto
_LL6174;} else{ goto _LL6175;} _LL6175: goto _LL6176; _LL6174: yyzzz= _temp6177;
goto _LL6172; _LL6176:( int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL6172:;}
yyzzz;}); _temp6170;}); _temp6169;}); _temp6168;}); break; case 300: _LL6167:
yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp6188=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6188[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6189; _temp6189.tag= Cyc_FieldPatternList_tok;
_temp6189.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({
struct Cyc_List_List* yyzzz;{ void* _temp6190= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_List_List*
_temp6196; _LL6192: if(*(( void**) _temp6190) ==  Cyc_FieldPatternList_tok){
_LL6197: _temp6196=(( struct Cyc_FieldPatternList_tok_struct*) _temp6190)->f1;
goto _LL6193;} else{ goto _LL6194;} _LL6194: goto _LL6195; _LL6193: yyzzz=
_temp6196; goto _LL6191; _LL6195:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL6191:;} yyzzz;})); _temp6189;}); _temp6188;}); break; case 301: _LL6187:
yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp6199=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6199[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6200; _temp6200.tag= Cyc_FieldPatternList_tok;
_temp6200.f1=({ struct Cyc_List_List* _temp6201=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6201->hd=( void*)({ struct
_tuple14* yyzzz;{ void* _temp6202= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; struct _tuple14* _temp6208; _LL6204: if(*(( void**) _temp6202)
==  Cyc_FieldPattern_tok){ _LL6209: _temp6208=(( struct Cyc_FieldPattern_tok_struct*)
_temp6202)->f1; goto _LL6205;} else{ goto _LL6206;} _LL6206: goto _LL6207;
_LL6205: yyzzz= _temp6208; goto _LL6203; _LL6207:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL6203:;} yyzzz;}); _temp6201->tl= 0; _temp6201;}); _temp6200;}); _temp6199;});
break; case 302: _LL6198: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp6211=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp6211[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp6212; _temp6212.tag= Cyc_FieldPatternList_tok; _temp6212.f1=({ struct Cyc_List_List*
_temp6213=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp6213->hd=( void*)({ struct _tuple14* yyzzz;{ void* _temp6222= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple14*
_temp6228; _LL6224: if(*(( void**) _temp6222) ==  Cyc_FieldPattern_tok){ _LL6229:
_temp6228=(( struct Cyc_FieldPattern_tok_struct*) _temp6222)->f1; goto _LL6225;}
else{ goto _LL6226;} _LL6226: goto _LL6227; _LL6225: yyzzz= _temp6228; goto
_LL6223; _LL6227:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL6223:;}
yyzzz;}); _temp6213->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp6214= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp6220; _LL6216: if(*(( void**) _temp6214) ==  Cyc_FieldPatternList_tok){
_LL6221: _temp6220=(( struct Cyc_FieldPatternList_tok_struct*) _temp6214)->f1;
goto _LL6217;} else{ goto _LL6218;} _LL6218: goto _LL6219; _LL6217: yyzzz=
_temp6220; goto _LL6215; _LL6219:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL6215:;} yyzzz;}); _temp6213;}); _temp6212;}); _temp6211;}); break; case 303:
_LL6210: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL6230: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6232=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6232[ 0]=({ struct Cyc_Exp_tok_struct _temp6233; _temp6233.tag= Cyc_Exp_tok;
_temp6233.f1= Cyc_Absyn_seq_exp(({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6234=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6240; _LL6236: if(*(( void**) _temp6234) ==  Cyc_Exp_tok){ _LL6241:
_temp6240=(( struct Cyc_Exp_tok_struct*) _temp6234)->f1; goto _LL6237;} else{
goto _LL6238;} _LL6238: goto _LL6239; _LL6237: yyzzz= _temp6240; goto _LL6235;
_LL6239:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6235:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6242= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6248; _LL6244: if(*(( void**) _temp6242) ==  Cyc_Exp_tok){ _LL6249:
_temp6248=(( struct Cyc_Exp_tok_struct*) _temp6242)->f1; goto _LL6245;} else{
goto _LL6246;} _LL6246: goto _LL6247; _LL6245: yyzzz= _temp6248; goto _LL6243;
_LL6247:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6243:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6233;}); _temp6232;}); break; case 305: _LL6231: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL6250:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6252=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6252[ 0]=({ struct Cyc_Exp_tok_struct
_temp6253; _temp6253.tag= Cyc_Exp_tok; _temp6253.f1= Cyc_Absyn_assignop_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6254= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6260; _LL6256: if(*(( void**) _temp6254) ==  Cyc_Exp_tok){ _LL6261:
_temp6260=(( struct Cyc_Exp_tok_struct*) _temp6254)->f1; goto _LL6257;} else{
goto _LL6258;} _LL6258: goto _LL6259; _LL6257: yyzzz= _temp6260; goto _LL6255;
_LL6259:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6255:;} yyzzz;}),({
struct Cyc_Core_Opt* yyzzz;{ void* _temp6262= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Core_Opt*
_temp6268; _LL6264: if(*(( void**) _temp6262) ==  Cyc_Primopopt_tok){ _LL6269:
_temp6268=(( struct Cyc_Primopopt_tok_struct*) _temp6262)->f1; goto _LL6265;}
else{ goto _LL6266;} _LL6266: goto _LL6267; _LL6265: yyzzz= _temp6268; goto
_LL6263; _LL6267:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL6263:;}
yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6270= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6276; _LL6272: if(*(( void**) _temp6270) ==  Cyc_Exp_tok){ _LL6277:
_temp6276=(( struct Cyc_Exp_tok_struct*) _temp6270)->f1; goto _LL6273;} else{
goto _LL6274;} _LL6274: goto _LL6275; _LL6273: yyzzz= _temp6276; goto _LL6271;
_LL6275:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6271:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6253;}); _temp6252;}); break; case 307: _LL6251: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6279=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6279[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6280; _temp6280.tag= Cyc_Primopopt_tok; _temp6280.f1=
0; _temp6280;}); _temp6279;}); break; case 308: _LL6278: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6282=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6282[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6283; _temp6283.tag= Cyc_Primopopt_tok; _temp6283.f1=({
struct Cyc_Core_Opt* _temp6284=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6284->v=( void*)(( void*) Cyc_Absyn_Times); _temp6284;});
_temp6283;}); _temp6282;}); break; case 309: _LL6281: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6286=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6286[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6287; _temp6287.tag= Cyc_Primopopt_tok; _temp6287.f1=({
struct Cyc_Core_Opt* _temp6288=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6288->v=( void*)(( void*) Cyc_Absyn_Div); _temp6288;});
_temp6287;}); _temp6286;}); break; case 310: _LL6285: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6290=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6290[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6291; _temp6291.tag= Cyc_Primopopt_tok; _temp6291.f1=({
struct Cyc_Core_Opt* _temp6292=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6292->v=( void*)(( void*) Cyc_Absyn_Mod); _temp6292;});
_temp6291;}); _temp6290;}); break; case 311: _LL6289: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6294=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6294[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6295; _temp6295.tag= Cyc_Primopopt_tok; _temp6295.f1=({
struct Cyc_Core_Opt* _temp6296=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6296->v=( void*)(( void*) Cyc_Absyn_Plus); _temp6296;});
_temp6295;}); _temp6294;}); break; case 312: _LL6293: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6298=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6298[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6299; _temp6299.tag= Cyc_Primopopt_tok; _temp6299.f1=({
struct Cyc_Core_Opt* _temp6300=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6300->v=( void*)(( void*) Cyc_Absyn_Minus); _temp6300;});
_temp6299;}); _temp6298;}); break; case 313: _LL6297: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6302=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6302[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6303; _temp6303.tag= Cyc_Primopopt_tok; _temp6303.f1=({
struct Cyc_Core_Opt* _temp6304=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6304->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp6304;});
_temp6303;}); _temp6302;}); break; case 314: _LL6301: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6306=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6306[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6307; _temp6307.tag= Cyc_Primopopt_tok; _temp6307.f1=({
struct Cyc_Core_Opt* _temp6308=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6308->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp6308;});
_temp6307;}); _temp6306;}); break; case 315: _LL6305: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6310=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6310[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6311; _temp6311.tag= Cyc_Primopopt_tok; _temp6311.f1=({
struct Cyc_Core_Opt* _temp6312=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6312->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp6312;});
_temp6311;}); _temp6310;}); break; case 316: _LL6309: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6314=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6314[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6315; _temp6315.tag= Cyc_Primopopt_tok; _temp6315.f1=({
struct Cyc_Core_Opt* _temp6316=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6316->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp6316;});
_temp6315;}); _temp6314;}); break; case 317: _LL6313: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6318=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6318[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6319; _temp6319.tag= Cyc_Primopopt_tok; _temp6319.f1=({
struct Cyc_Core_Opt* _temp6320=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6320->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp6320;});
_temp6319;}); _temp6318;}); break; case 318: _LL6317: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL6321:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6323=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6323[ 0]=({ struct Cyc_Exp_tok_struct
_temp6324; _temp6324.tag= Cyc_Exp_tok; _temp6324.f1= Cyc_Absyn_conditional_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6325= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct Cyc_Absyn_Exp*
_temp6331; _LL6327: if(*(( void**) _temp6325) ==  Cyc_Exp_tok){ _LL6332:
_temp6331=(( struct Cyc_Exp_tok_struct*) _temp6325)->f1; goto _LL6328;} else{
goto _LL6329;} _LL6329: goto _LL6330; _LL6328: yyzzz= _temp6331; goto _LL6326;
_LL6330:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6326:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6333= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6339; _LL6335: if(*(( void**) _temp6333) ==  Cyc_Exp_tok){ _LL6340:
_temp6339=(( struct Cyc_Exp_tok_struct*) _temp6333)->f1; goto _LL6336;} else{
goto _LL6337;} _LL6337: goto _LL6338; _LL6336: yyzzz= _temp6339; goto _LL6334;
_LL6338:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6334:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6341= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6347; _LL6343: if(*(( void**) _temp6341) ==  Cyc_Exp_tok){ _LL6348:
_temp6347=(( struct Cyc_Exp_tok_struct*) _temp6341)->f1; goto _LL6344;} else{
goto _LL6345;} _LL6345: goto _LL6346; _LL6344: yyzzz= _temp6347; goto _LL6342;
_LL6346:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6342:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6324;}); _temp6323;}); break; case 320: _LL6322: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6350=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6350[ 0]=({ struct Cyc_Exp_tok_struct
_temp6351; _temp6351.tag= Cyc_Exp_tok; _temp6351.f1= Cyc_Absyn_throw_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6352= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6358; _LL6354: if(*(( void**) _temp6352) ==  Cyc_Exp_tok){ _LL6359:
_temp6358=(( struct Cyc_Exp_tok_struct*) _temp6352)->f1; goto _LL6355;} else{
goto _LL6356;} _LL6356: goto _LL6357; _LL6355: yyzzz= _temp6358; goto _LL6353;
_LL6357:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6353:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6351;}); _temp6350;}); break; case 321: _LL6349: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6361=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6361[ 0]=({ struct Cyc_Exp_tok_struct
_temp6362; _temp6362.tag= Cyc_Exp_tok; _temp6362.f1= Cyc_Absyn_New_exp( 0,({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6363= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6369; _LL6365: if(*(( void**) _temp6363) ==  Cyc_Exp_tok){ _LL6370:
_temp6369=(( struct Cyc_Exp_tok_struct*) _temp6363)->f1; goto _LL6366;} else{
goto _LL6367;} _LL6367: goto _LL6368; _LL6366: yyzzz= _temp6369; goto _LL6364;
_LL6368:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6364:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp6362;}); _temp6361;}); break; case 322: _LL6360: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6372=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6372[ 0]=({ struct Cyc_Exp_tok_struct
_temp6373; _temp6373.tag= Cyc_Exp_tok; _temp6373.f1= Cyc_Absyn_New_exp( 0,({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6374= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6380; _LL6376: if(*(( void**) _temp6374) ==  Cyc_Exp_tok){ _LL6381:
_temp6380=(( struct Cyc_Exp_tok_struct*) _temp6374)->f1; goto _LL6377;} else{
goto _LL6378;} _LL6378: goto _LL6379; _LL6377: yyzzz= _temp6380; goto _LL6375;
_LL6379:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6375:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp6373;}); _temp6372;}); break; case 323: _LL6371: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6383=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6383[ 0]=({ struct Cyc_Exp_tok_struct
_temp6384; _temp6384.tag= Cyc_Exp_tok; _temp6384.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6385= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6391; _LL6387: if(*(( void**) _temp6385) ==  Cyc_Exp_tok){ _LL6392:
_temp6391=(( struct Cyc_Exp_tok_struct*) _temp6385)->f1; goto _LL6388;} else{
goto _LL6389;} _LL6389: goto _LL6390; _LL6388: yyzzz= _temp6391; goto _LL6386;
_LL6390:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6386:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6393= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6399; _LL6395: if(*(( void**) _temp6393) ==  Cyc_Exp_tok){ _LL6400:
_temp6399=(( struct Cyc_Exp_tok_struct*) _temp6393)->f1; goto _LL6396;} else{
goto _LL6397;} _LL6397: goto _LL6398; _LL6396: yyzzz= _temp6399; goto _LL6394;
_LL6398:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6394:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6384;}); _temp6383;}); break; case 324: _LL6382: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6402=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6402[ 0]=({ struct Cyc_Exp_tok_struct
_temp6403; _temp6403.tag= Cyc_Exp_tok; _temp6403.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6404= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6410; _LL6406: if(*(( void**) _temp6404) ==  Cyc_Exp_tok){ _LL6411:
_temp6410=(( struct Cyc_Exp_tok_struct*) _temp6404)->f1; goto _LL6407;} else{
goto _LL6408;} _LL6408: goto _LL6409; _LL6407: yyzzz= _temp6410; goto _LL6405;
_LL6409:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6405:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6412= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6418; _LL6414: if(*(( void**) _temp6412) ==  Cyc_Exp_tok){ _LL6419:
_temp6418=(( struct Cyc_Exp_tok_struct*) _temp6412)->f1; goto _LL6415;} else{
goto _LL6416;} _LL6416: goto _LL6417; _LL6415: yyzzz= _temp6418; goto _LL6413;
_LL6417:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6413:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6403;}); _temp6402;}); break; case 325: _LL6401: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL6420:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL6421: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6423=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6423[ 0]=({ struct Cyc_Exp_tok_struct
_temp6424; _temp6424.tag= Cyc_Exp_tok; _temp6424.f1= Cyc_Absyn_or_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6425= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6431; _LL6427: if(*((
void**) _temp6425) ==  Cyc_Exp_tok){ _LL6432: _temp6431=(( struct Cyc_Exp_tok_struct*)
_temp6425)->f1; goto _LL6428;} else{ goto _LL6429;} _LL6429: goto _LL6430;
_LL6428: yyzzz= _temp6431; goto _LL6426; _LL6430:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6426:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6433= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6439; _LL6435: if(*(( void**) _temp6433) ==  Cyc_Exp_tok){ _LL6440:
_temp6439=(( struct Cyc_Exp_tok_struct*) _temp6433)->f1; goto _LL6436;} else{
goto _LL6437;} _LL6437: goto _LL6438; _LL6436: yyzzz= _temp6439; goto _LL6434;
_LL6438:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6434:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6424;}); _temp6423;}); break; case 328: _LL6422: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL6441:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6443=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6443[ 0]=({ struct Cyc_Exp_tok_struct
_temp6444; _temp6444.tag= Cyc_Exp_tok; _temp6444.f1= Cyc_Absyn_and_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6445= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6451; _LL6447: if(*((
void**) _temp6445) ==  Cyc_Exp_tok){ _LL6452: _temp6451=(( struct Cyc_Exp_tok_struct*)
_temp6445)->f1; goto _LL6448;} else{ goto _LL6449;} _LL6449: goto _LL6450;
_LL6448: yyzzz= _temp6451; goto _LL6446; _LL6450:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6446:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6453= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6459; _LL6455: if(*(( void**) _temp6453) ==  Cyc_Exp_tok){ _LL6460:
_temp6459=(( struct Cyc_Exp_tok_struct*) _temp6453)->f1; goto _LL6456;} else{
goto _LL6457;} _LL6457: goto _LL6458; _LL6456: yyzzz= _temp6459; goto _LL6454;
_LL6458:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6454:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6444;}); _temp6443;}); break; case 330: _LL6442: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL6461:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6463=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6463[ 0]=({ struct Cyc_Exp_tok_struct
_temp6464; _temp6464.tag= Cyc_Exp_tok; _temp6464.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6465= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6471; _LL6467: if(*(( void**) _temp6465) ==  Cyc_Exp_tok){ _LL6472:
_temp6471=(( struct Cyc_Exp_tok_struct*) _temp6465)->f1; goto _LL6468;} else{
goto _LL6469;} _LL6469: goto _LL6470; _LL6468: yyzzz= _temp6471; goto _LL6466;
_LL6470:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6466:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6473= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6479; _LL6475: if(*(( void**) _temp6473) ==  Cyc_Exp_tok){ _LL6480:
_temp6479=(( struct Cyc_Exp_tok_struct*) _temp6473)->f1; goto _LL6476;} else{
goto _LL6477;} _LL6477: goto _LL6478; _LL6476: yyzzz= _temp6479; goto _LL6474;
_LL6478:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6474:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6464;}); _temp6463;}); break; case 332: _LL6462: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL6481:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6483=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6483[ 0]=({ struct Cyc_Exp_tok_struct
_temp6484; _temp6484.tag= Cyc_Exp_tok; _temp6484.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6485= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6491; _LL6487: if(*(( void**) _temp6485) ==  Cyc_Exp_tok){ _LL6492:
_temp6491=(( struct Cyc_Exp_tok_struct*) _temp6485)->f1; goto _LL6488;} else{
goto _LL6489;} _LL6489: goto _LL6490; _LL6488: yyzzz= _temp6491; goto _LL6486;
_LL6490:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6486:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6493= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6499; _LL6495: if(*(( void**) _temp6493) ==  Cyc_Exp_tok){ _LL6500:
_temp6499=(( struct Cyc_Exp_tok_struct*) _temp6493)->f1; goto _LL6496;} else{
goto _LL6497;} _LL6497: goto _LL6498; _LL6496: yyzzz= _temp6499; goto _LL6494;
_LL6498:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6494:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6484;}); _temp6483;}); break; case 334: _LL6482: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL6501:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6503=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6503[ 0]=({ struct Cyc_Exp_tok_struct
_temp6504; _temp6504.tag= Cyc_Exp_tok; _temp6504.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6505= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6511; _LL6507: if(*(( void**) _temp6505) ==  Cyc_Exp_tok){ _LL6512:
_temp6511=(( struct Cyc_Exp_tok_struct*) _temp6505)->f1; goto _LL6508;} else{
goto _LL6509;} _LL6509: goto _LL6510; _LL6508: yyzzz= _temp6511; goto _LL6506;
_LL6510:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6506:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6513= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6519; _LL6515: if(*(( void**) _temp6513) ==  Cyc_Exp_tok){ _LL6520:
_temp6519=(( struct Cyc_Exp_tok_struct*) _temp6513)->f1; goto _LL6516;} else{
goto _LL6517;} _LL6517: goto _LL6518; _LL6516: yyzzz= _temp6519; goto _LL6514;
_LL6518:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6514:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6504;}); _temp6503;}); break; case 336: _LL6502: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL6521:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6523=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6523[ 0]=({ struct Cyc_Exp_tok_struct
_temp6524; _temp6524.tag= Cyc_Exp_tok; _temp6524.f1= Cyc_Absyn_eq_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6525= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6531; _LL6527: if(*((
void**) _temp6525) ==  Cyc_Exp_tok){ _LL6532: _temp6531=(( struct Cyc_Exp_tok_struct*)
_temp6525)->f1; goto _LL6528;} else{ goto _LL6529;} _LL6529: goto _LL6530;
_LL6528: yyzzz= _temp6531; goto _LL6526; _LL6530:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6526:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6533= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6539; _LL6535: if(*(( void**) _temp6533) ==  Cyc_Exp_tok){ _LL6540:
_temp6539=(( struct Cyc_Exp_tok_struct*) _temp6533)->f1; goto _LL6536;} else{
goto _LL6537;} _LL6537: goto _LL6538; _LL6536: yyzzz= _temp6539; goto _LL6534;
_LL6538:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6534:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6524;}); _temp6523;}); break; case 338: _LL6522: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6542=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6542[ 0]=({ struct Cyc_Exp_tok_struct
_temp6543; _temp6543.tag= Cyc_Exp_tok; _temp6543.f1= Cyc_Absyn_neq_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6544= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6550; _LL6546: if(*((
void**) _temp6544) ==  Cyc_Exp_tok){ _LL6551: _temp6550=(( struct Cyc_Exp_tok_struct*)
_temp6544)->f1; goto _LL6547;} else{ goto _LL6548;} _LL6548: goto _LL6549;
_LL6547: yyzzz= _temp6550; goto _LL6545; _LL6549:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6545:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6552= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6558; _LL6554: if(*(( void**) _temp6552) ==  Cyc_Exp_tok){ _LL6559:
_temp6558=(( struct Cyc_Exp_tok_struct*) _temp6552)->f1; goto _LL6555;} else{
goto _LL6556;} _LL6556: goto _LL6557; _LL6555: yyzzz= _temp6558; goto _LL6553;
_LL6557:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6553:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6543;}); _temp6542;}); break; case 339: _LL6541: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL6560:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6562=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6562[ 0]=({ struct Cyc_Exp_tok_struct
_temp6563; _temp6563.tag= Cyc_Exp_tok; _temp6563.f1= Cyc_Absyn_lt_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6564= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6570; _LL6566: if(*((
void**) _temp6564) ==  Cyc_Exp_tok){ _LL6571: _temp6570=(( struct Cyc_Exp_tok_struct*)
_temp6564)->f1; goto _LL6567;} else{ goto _LL6568;} _LL6568: goto _LL6569;
_LL6567: yyzzz= _temp6570; goto _LL6565; _LL6569:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6565:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6572= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6578; _LL6574: if(*(( void**) _temp6572) ==  Cyc_Exp_tok){ _LL6579:
_temp6578=(( struct Cyc_Exp_tok_struct*) _temp6572)->f1; goto _LL6575;} else{
goto _LL6576;} _LL6576: goto _LL6577; _LL6575: yyzzz= _temp6578; goto _LL6573;
_LL6577:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6573:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6563;}); _temp6562;}); break; case 341: _LL6561: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6581=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6581[ 0]=({ struct Cyc_Exp_tok_struct
_temp6582; _temp6582.tag= Cyc_Exp_tok; _temp6582.f1= Cyc_Absyn_gt_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6583= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6589; _LL6585: if(*((
void**) _temp6583) ==  Cyc_Exp_tok){ _LL6590: _temp6589=(( struct Cyc_Exp_tok_struct*)
_temp6583)->f1; goto _LL6586;} else{ goto _LL6587;} _LL6587: goto _LL6588;
_LL6586: yyzzz= _temp6589; goto _LL6584; _LL6588:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6584:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6591= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6597; _LL6593: if(*(( void**) _temp6591) ==  Cyc_Exp_tok){ _LL6598:
_temp6597=(( struct Cyc_Exp_tok_struct*) _temp6591)->f1; goto _LL6594;} else{
goto _LL6595;} _LL6595: goto _LL6596; _LL6594: yyzzz= _temp6597; goto _LL6592;
_LL6596:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6592:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6582;}); _temp6581;}); break; case 342: _LL6580: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6600=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6600[ 0]=({ struct Cyc_Exp_tok_struct
_temp6601; _temp6601.tag= Cyc_Exp_tok; _temp6601.f1= Cyc_Absyn_lte_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6602= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6608; _LL6604: if(*((
void**) _temp6602) ==  Cyc_Exp_tok){ _LL6609: _temp6608=(( struct Cyc_Exp_tok_struct*)
_temp6602)->f1; goto _LL6605;} else{ goto _LL6606;} _LL6606: goto _LL6607;
_LL6605: yyzzz= _temp6608; goto _LL6603; _LL6607:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6603:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6610= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6616; _LL6612: if(*(( void**) _temp6610) ==  Cyc_Exp_tok){ _LL6617:
_temp6616=(( struct Cyc_Exp_tok_struct*) _temp6610)->f1; goto _LL6613;} else{
goto _LL6614;} _LL6614: goto _LL6615; _LL6613: yyzzz= _temp6616; goto _LL6611;
_LL6615:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6611:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6601;}); _temp6600;}); break; case 343: _LL6599: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6619=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6619[ 0]=({ struct Cyc_Exp_tok_struct
_temp6620; _temp6620.tag= Cyc_Exp_tok; _temp6620.f1= Cyc_Absyn_gte_exp(({ struct
Cyc_Absyn_Exp* yyzzz;{ void* _temp6621= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp* _temp6627; _LL6623: if(*((
void**) _temp6621) ==  Cyc_Exp_tok){ _LL6628: _temp6627=(( struct Cyc_Exp_tok_struct*)
_temp6621)->f1; goto _LL6624;} else{ goto _LL6625;} _LL6625: goto _LL6626;
_LL6624: yyzzz= _temp6627; goto _LL6622; _LL6626:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL6622:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6629= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6635; _LL6631: if(*(( void**) _temp6629) ==  Cyc_Exp_tok){ _LL6636:
_temp6635=(( struct Cyc_Exp_tok_struct*) _temp6629)->f1; goto _LL6632;} else{
goto _LL6633;} _LL6633: goto _LL6634; _LL6632: yyzzz= _temp6635; goto _LL6630;
_LL6634:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6630:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6620;}); _temp6619;}); break; case 344: _LL6618: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL6637:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6639=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6639[ 0]=({ struct Cyc_Exp_tok_struct
_temp6640; _temp6640.tag= Cyc_Exp_tok; _temp6640.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6641= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6647; _LL6643: if(*(( void**) _temp6641) ==  Cyc_Exp_tok){ _LL6648:
_temp6647=(( struct Cyc_Exp_tok_struct*) _temp6641)->f1; goto _LL6644;} else{
goto _LL6645;} _LL6645: goto _LL6646; _LL6644: yyzzz= _temp6647; goto _LL6642;
_LL6646:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6642:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6649= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6655; _LL6651: if(*(( void**) _temp6649) ==  Cyc_Exp_tok){ _LL6656:
_temp6655=(( struct Cyc_Exp_tok_struct*) _temp6649)->f1; goto _LL6652;} else{
goto _LL6653;} _LL6653: goto _LL6654; _LL6652: yyzzz= _temp6655; goto _LL6650;
_LL6654:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6650:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6640;}); _temp6639;}); break; case 346: _LL6638: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6658=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6658[ 0]=({ struct Cyc_Exp_tok_struct
_temp6659; _temp6659.tag= Cyc_Exp_tok; _temp6659.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6660= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6666; _LL6662: if(*(( void**) _temp6660) ==  Cyc_Exp_tok){ _LL6667:
_temp6666=(( struct Cyc_Exp_tok_struct*) _temp6660)->f1; goto _LL6663;} else{
goto _LL6664;} _LL6664: goto _LL6665; _LL6663: yyzzz= _temp6666; goto _LL6661;
_LL6665:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6661:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6668= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6674; _LL6670: if(*(( void**) _temp6668) ==  Cyc_Exp_tok){ _LL6675:
_temp6674=(( struct Cyc_Exp_tok_struct*) _temp6668)->f1; goto _LL6671;} else{
goto _LL6672;} _LL6672: goto _LL6673; _LL6671: yyzzz= _temp6674; goto _LL6669;
_LL6673:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6669:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6659;}); _temp6658;}); break; case 347: _LL6657: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL6676:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6678=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6678[ 0]=({ struct Cyc_Exp_tok_struct
_temp6679; _temp6679.tag= Cyc_Exp_tok; _temp6679.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6680= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6686; _LL6682: if(*(( void**) _temp6680) ==  Cyc_Exp_tok){ _LL6687:
_temp6686=(( struct Cyc_Exp_tok_struct*) _temp6680)->f1; goto _LL6683;} else{
goto _LL6684;} _LL6684: goto _LL6685; _LL6683: yyzzz= _temp6686; goto _LL6681;
_LL6685:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6681:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6688= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6694; _LL6690: if(*(( void**) _temp6688) ==  Cyc_Exp_tok){ _LL6695:
_temp6694=(( struct Cyc_Exp_tok_struct*) _temp6688)->f1; goto _LL6691;} else{
goto _LL6692;} _LL6692: goto _LL6693; _LL6691: yyzzz= _temp6694; goto _LL6689;
_LL6693:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6689:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6679;}); _temp6678;}); break; case 349: _LL6677: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6697=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6697[ 0]=({ struct Cyc_Exp_tok_struct
_temp6698; _temp6698.tag= Cyc_Exp_tok; _temp6698.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6699= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6705; _LL6701: if(*(( void**) _temp6699) ==  Cyc_Exp_tok){ _LL6706:
_temp6705=(( struct Cyc_Exp_tok_struct*) _temp6699)->f1; goto _LL6702;} else{
goto _LL6703;} _LL6703: goto _LL6704; _LL6702: yyzzz= _temp6705; goto _LL6700;
_LL6704:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6700:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6707= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6713; _LL6709: if(*(( void**) _temp6707) ==  Cyc_Exp_tok){ _LL6714:
_temp6713=(( struct Cyc_Exp_tok_struct*) _temp6707)->f1; goto _LL6710;} else{
goto _LL6711;} _LL6711: goto _LL6712; _LL6710: yyzzz= _temp6713; goto _LL6708;
_LL6712:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6708:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6698;}); _temp6697;}); break; case 350: _LL6696: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL6715:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6717=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6717[ 0]=({ struct Cyc_Exp_tok_struct
_temp6718; _temp6718.tag= Cyc_Exp_tok; _temp6718.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6719= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6725; _LL6721: if(*(( void**) _temp6719) ==  Cyc_Exp_tok){ _LL6726:
_temp6725=(( struct Cyc_Exp_tok_struct*) _temp6719)->f1; goto _LL6722;} else{
goto _LL6723;} _LL6723: goto _LL6724; _LL6722: yyzzz= _temp6725; goto _LL6720;
_LL6724:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6720:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6727= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6733; _LL6729: if(*(( void**) _temp6727) ==  Cyc_Exp_tok){ _LL6734:
_temp6733=(( struct Cyc_Exp_tok_struct*) _temp6727)->f1; goto _LL6730;} else{
goto _LL6731;} _LL6731: goto _LL6732; _LL6730: yyzzz= _temp6733; goto _LL6728;
_LL6732:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6728:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6718;}); _temp6717;}); break; case 352: _LL6716: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6736=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6736[ 0]=({ struct Cyc_Exp_tok_struct
_temp6737; _temp6737.tag= Cyc_Exp_tok; _temp6737.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6738= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6744; _LL6740: if(*(( void**) _temp6738) ==  Cyc_Exp_tok){ _LL6745:
_temp6744=(( struct Cyc_Exp_tok_struct*) _temp6738)->f1; goto _LL6741;} else{
goto _LL6742;} _LL6742: goto _LL6743; _LL6741: yyzzz= _temp6744; goto _LL6739;
_LL6743:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6739:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6746= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6752; _LL6748: if(*(( void**) _temp6746) ==  Cyc_Exp_tok){ _LL6753:
_temp6752=(( struct Cyc_Exp_tok_struct*) _temp6746)->f1; goto _LL6749;} else{
goto _LL6750;} _LL6750: goto _LL6751; _LL6749: yyzzz= _temp6752; goto _LL6747;
_LL6751:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6747:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6737;}); _temp6736;}); break; case 353: _LL6735: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6755=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6755[ 0]=({ struct Cyc_Exp_tok_struct
_temp6756; _temp6756.tag= Cyc_Exp_tok; _temp6756.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6757= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6763; _LL6759: if(*(( void**) _temp6757) ==  Cyc_Exp_tok){ _LL6764:
_temp6763=(( struct Cyc_Exp_tok_struct*) _temp6757)->f1; goto _LL6760;} else{
goto _LL6761;} _LL6761: goto _LL6762; _LL6760: yyzzz= _temp6763; goto _LL6758;
_LL6762:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6758:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6765= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6771; _LL6767: if(*(( void**) _temp6765) ==  Cyc_Exp_tok){ _LL6772:
_temp6771=(( struct Cyc_Exp_tok_struct*) _temp6765)->f1; goto _LL6768;} else{
goto _LL6769;} _LL6769: goto _LL6770; _LL6768: yyzzz= _temp6771; goto _LL6766;
_LL6770:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6766:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6756;}); _temp6755;}); break; case 354: _LL6754: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL6773:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6775=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6775[ 0]=({ struct Cyc_Exp_tok_struct
_temp6776; _temp6776.tag= Cyc_Exp_tok; _temp6776.f1= Cyc_Absyn_cast_exp((*({
struct _tuple2* yyzzz;{ void* _temp6777= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]; struct _tuple2* _temp6783; _LL6779: if(*(( void**)
_temp6777) ==  Cyc_ParamDecl_tok){ _LL6784: _temp6783=(( struct Cyc_ParamDecl_tok_struct*)
_temp6777)->f1; goto _LL6780;} else{ goto _LL6781;} _LL6781: goto _LL6782;
_LL6780: yyzzz= _temp6783; goto _LL6778; _LL6782:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL6778:;} yyzzz;})).f3,({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6785= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6791; _LL6787: if(*(( void**) _temp6785) ==  Cyc_Exp_tok){ _LL6792:
_temp6791=(( struct Cyc_Exp_tok_struct*) _temp6785)->f1; goto _LL6788;} else{
goto _LL6789;} _LL6789: goto _LL6790; _LL6788: yyzzz= _temp6791; goto _LL6786;
_LL6790:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6786:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6776;}); _temp6775;}); break; case 356: _LL6774: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL6793:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6795=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6795[ 0]=({ struct Cyc_Exp_tok_struct
_temp6796; _temp6796.tag= Cyc_Exp_tok; _temp6796.f1= Cyc_Absyn_pre_inc_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6797= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6803; _LL6799: if(*(( void**) _temp6797) ==  Cyc_Exp_tok){ _LL6804:
_temp6803=(( struct Cyc_Exp_tok_struct*) _temp6797)->f1; goto _LL6800;} else{
goto _LL6801;} _LL6801: goto _LL6802; _LL6800: yyzzz= _temp6803; goto _LL6798;
_LL6802:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6798:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6796;}); _temp6795;}); break; case 358: _LL6794: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6806=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6806[ 0]=({ struct Cyc_Exp_tok_struct
_temp6807; _temp6807.tag= Cyc_Exp_tok; _temp6807.f1= Cyc_Absyn_pre_dec_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6808= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6814; _LL6810: if(*(( void**) _temp6808) ==  Cyc_Exp_tok){ _LL6815:
_temp6814=(( struct Cyc_Exp_tok_struct*) _temp6808)->f1; goto _LL6811;} else{
goto _LL6812;} _LL6812: goto _LL6813; _LL6811: yyzzz= _temp6814; goto _LL6809;
_LL6813:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6809:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6807;}); _temp6806;}); break; case 359: _LL6805: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6817=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6817[ 0]=({ struct Cyc_Exp_tok_struct
_temp6818; _temp6818.tag= Cyc_Exp_tok; _temp6818.f1= Cyc_Absyn_address_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6819= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6825; _LL6821: if(*(( void**) _temp6819) ==  Cyc_Exp_tok){ _LL6826:
_temp6825=(( struct Cyc_Exp_tok_struct*) _temp6819)->f1; goto _LL6822;} else{
goto _LL6823;} _LL6823: goto _LL6824; _LL6822: yyzzz= _temp6825; goto _LL6820;
_LL6824:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6820:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6818;}); _temp6817;}); break; case 360: _LL6816: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6828=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6828[ 0]=({ struct Cyc_Exp_tok_struct
_temp6829; _temp6829.tag= Cyc_Exp_tok; _temp6829.f1= Cyc_Absyn_deref_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6830= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6836; _LL6832: if(*(( void**) _temp6830) ==  Cyc_Exp_tok){ _LL6837:
_temp6836=(( struct Cyc_Exp_tok_struct*) _temp6830)->f1; goto _LL6833;} else{
goto _LL6834;} _LL6834: goto _LL6835; _LL6833: yyzzz= _temp6836; goto _LL6831;
_LL6835:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6831:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6829;}); _temp6828;}); break; case 361: _LL6827: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL6838:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6840=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6840[ 0]=({ struct Cyc_Exp_tok_struct
_temp6841; _temp6841.tag= Cyc_Exp_tok; _temp6841.f1= Cyc_Absyn_prim1_exp(({ void*
yyzzz;{ void* _temp6842= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; void* _temp6848; _LL6844: if(*(( void**) _temp6842) ==  Cyc_Primop_tok){
_LL6849: _temp6848=( void*)(( struct Cyc_Primop_tok_struct*) _temp6842)->f1;
goto _LL6845;} else{ goto _LL6846;} _LL6846: goto _LL6847; _LL6845: yyzzz=
_temp6848; goto _LL6843; _LL6847:( int) _throw(( void*)& Cyc_yyfail_Primop_tok);
_LL6843:;} yyzzz;}),({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6850= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp6856; _LL6852: if(*(( void**) _temp6850) ==  Cyc_Exp_tok){ _LL6857:
_temp6856=(( struct Cyc_Exp_tok_struct*) _temp6850)->f1; goto _LL6853;} else{
goto _LL6854;} _LL6854: goto _LL6855; _LL6853: yyzzz= _temp6856; goto _LL6851;
_LL6855:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6851:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6841;}); _temp6840;}); break; case 363: _LL6839: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6859=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6859[ 0]=({ struct Cyc_Exp_tok_struct
_temp6860; _temp6860.tag= Cyc_Exp_tok; _temp6860.f1= Cyc_Absyn_sizeoftyp_exp((*({
struct _tuple2* yyzzz;{ void* _temp6861= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]; struct _tuple2* _temp6867; _LL6863: if(*(( void**)
_temp6861) ==  Cyc_ParamDecl_tok){ _LL6868: _temp6867=(( struct Cyc_ParamDecl_tok_struct*)
_temp6861)->f1; goto _LL6864;} else{ goto _LL6865;} _LL6865: goto _LL6866;
_LL6864: yyzzz= _temp6867; goto _LL6862; _LL6866:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL6862:;} yyzzz;})).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6860;});
_temp6859;}); break; case 364: _LL6858: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6870=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6870[ 0]=({ struct Cyc_Exp_tok_struct _temp6871; _temp6871.tag= Cyc_Exp_tok;
_temp6871.f1= Cyc_Absyn_sizeofexp_exp(({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp6872= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct
Cyc_Absyn_Exp* _temp6878; _LL6874: if(*(( void**) _temp6872) ==  Cyc_Exp_tok){
_LL6879: _temp6878=(( struct Cyc_Exp_tok_struct*) _temp6872)->f1; goto _LL6875;}
else{ goto _LL6876;} _LL6876: goto _LL6877; _LL6875: yyzzz= _temp6878; goto
_LL6873; _LL6877:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6873:;} yyzzz;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp6871;}); _temp6870;}); break; case 365:
_LL6869: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6881=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6881[ 0]=({ struct Cyc_Exp_tok_struct
_temp6882; _temp6882.tag= Cyc_Exp_tok; _temp6882.f1= Cyc_Absyn_offsetof_exp((*({
struct _tuple2* yyzzz;{ void* _temp6883= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]; struct _tuple2* _temp6889; _LL6885: if(*(( void**)
_temp6883) ==  Cyc_ParamDecl_tok){ _LL6890: _temp6889=(( struct Cyc_ParamDecl_tok_struct*)
_temp6883)->f1; goto _LL6886;} else{ goto _LL6887;} _LL6887: goto _LL6888;
_LL6886: yyzzz= _temp6889; goto _LL6884; _LL6888:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL6884:;} yyzzz;})).f3,({ struct _tagged_arr* _temp6891=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp6891[ 0]=({ struct _tagged_arr
yyzzz;{ void* _temp6892= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct _tagged_arr _temp6898; _LL6894: if(*(( void**)
_temp6892) ==  Cyc_String_tok){ _LL6899: _temp6898=(( struct Cyc_String_tok_struct*)
_temp6892)->f1; goto _LL6895;} else{ goto _LL6896;} _LL6896: goto _LL6897;
_LL6895: yyzzz= _temp6898; goto _LL6893; _LL6897:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL6893:;} yyzzz;}); _temp6891;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6882;});
_temp6881;}); break; case 366: _LL6880: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6901=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6901[ 0]=({ struct Cyc_Exp_tok_struct _temp6902; _temp6902.tag= Cyc_Exp_tok;
_temp6902.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6903=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6903[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6904; _temp6904.tag= Cyc_Absyn_Malloc_e;
_temp6904.f1= 0; _temp6904.f2=( void*) Cyc_Parse_speclist2typ((*({ struct
_tuple17* yyzzz;{ void* _temp6905= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]; struct _tuple17* _temp6911; _LL6907: if(*(( void**)
_temp6905) ==  Cyc_QualSpecList_tok){ _LL6912: _temp6911=(( struct Cyc_QualSpecList_tok_struct*)
_temp6905)->f1; goto _LL6908;} else{ goto _LL6909;} _LL6909: goto _LL6910;
_LL6908: yyzzz= _temp6911; goto _LL6906; _LL6910:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL6906:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp6904;}); _temp6903;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6902;});
_temp6901;}); break; case 367: _LL6900: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6914=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6914[ 0]=({ struct Cyc_Exp_tok_struct _temp6915; _temp6915.tag= Cyc_Exp_tok;
_temp6915.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6916=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6916[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6917; _temp6917.tag= Cyc_Absyn_Malloc_e;
_temp6917.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp6918= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)];
struct Cyc_Absyn_Exp* _temp6924; _LL6920: if(*(( void**) _temp6918) ==  Cyc_Exp_tok){
_LL6925: _temp6924=(( struct Cyc_Exp_tok_struct*) _temp6918)->f1; goto _LL6921;}
else{ goto _LL6922;} _LL6922: goto _LL6923; _LL6921: yyzzz= _temp6924; goto
_LL6919; _LL6923:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6919:;} yyzzz;});
_temp6917.f2=( void*) Cyc_Parse_speclist2typ((*({ struct _tuple17* yyzzz;{ void*
_temp6926= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)];
struct _tuple17* _temp6932; _LL6928: if(*(( void**) _temp6926) ==  Cyc_QualSpecList_tok){
_LL6933: _temp6932=(( struct Cyc_QualSpecList_tok_struct*) _temp6926)->f1; goto
_LL6929;} else{ goto _LL6930;} _LL6930: goto _LL6931; _LL6929: yyzzz= _temp6932;
goto _LL6927; _LL6931:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL6927:;} yyzzz;})).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp6917;}); _temp6916;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp6915;});
_temp6914;}); break; case 368: _LL6913: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp6935=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6935[ 0]=({ struct Cyc_Primop_tok_struct _temp6936; _temp6936.tag= Cyc_Primop_tok;
_temp6936.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp6936;}); _temp6935;});
break; case 369: _LL6934: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp6938=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6938[ 0]=({ struct Cyc_Primop_tok_struct _temp6939; _temp6939.tag= Cyc_Primop_tok;
_temp6939.f1=( void*)(( void*) Cyc_Absyn_Not); _temp6939;}); _temp6938;});
break; case 370: _LL6937: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp6941=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6941[ 0]=({ struct Cyc_Primop_tok_struct _temp6942; _temp6942.tag= Cyc_Primop_tok;
_temp6942.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp6942;}); _temp6941;});
break; case 371: _LL6940: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 372: _LL6943: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp6945=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6945[ 0]=({ struct Cyc_Exp_tok_struct _temp6946; _temp6946.tag= Cyc_Exp_tok;
_temp6946.f1= Cyc_Absyn_subscript_exp(({ struct Cyc_Absyn_Exp* yyzzz;{ void*
_temp6947= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)];
struct Cyc_Absyn_Exp* _temp6953; _LL6949: if(*(( void**) _temp6947) ==  Cyc_Exp_tok){
_LL6954: _temp6953=(( struct Cyc_Exp_tok_struct*) _temp6947)->f1; goto _LL6950;}
else{ goto _LL6951;} _LL6951: goto _LL6952; _LL6950: yyzzz= _temp6953; goto
_LL6948; _LL6952:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6948:;} yyzzz;}),({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6955= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp6961; _LL6957: if(*(( void**) _temp6955) ==  Cyc_Exp_tok){ _LL6962:
_temp6961=(( struct Cyc_Exp_tok_struct*) _temp6955)->f1; goto _LL6958;} else{
goto _LL6959;} _LL6959: goto _LL6960; _LL6958: yyzzz= _temp6961; goto _LL6956;
_LL6960:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6956:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6946;}); _temp6945;}); break; case 373: _LL6944: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6964=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6964[ 0]=({ struct Cyc_Exp_tok_struct
_temp6965; _temp6965.tag= Cyc_Exp_tok; _temp6965.f1= Cyc_Absyn_unknowncall_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6966= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp6972; _LL6968: if(*(( void**) _temp6966) ==  Cyc_Exp_tok){ _LL6973:
_temp6972=(( struct Cyc_Exp_tok_struct*) _temp6966)->f1; goto _LL6969;} else{
goto _LL6970;} _LL6970: goto _LL6971; _LL6969: yyzzz= _temp6972; goto _LL6967;
_LL6971:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6967:;} yyzzz;}), 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp6965;}); _temp6964;}); break; case 374: _LL6963: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6975=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6975[ 0]=({ struct Cyc_Exp_tok_struct
_temp6976; _temp6976.tag= Cyc_Exp_tok; _temp6976.f1= Cyc_Absyn_unknowncall_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6977= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct Cyc_Absyn_Exp*
_temp6983; _LL6979: if(*(( void**) _temp6977) ==  Cyc_Exp_tok){ _LL6984:
_temp6983=(( struct Cyc_Exp_tok_struct*) _temp6977)->f1; goto _LL6980;} else{
goto _LL6981;} _LL6981: goto _LL6982; _LL6980: yyzzz= _temp6983; goto _LL6978;
_LL6982:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6978:;} yyzzz;}),({
struct Cyc_List_List* yyzzz;{ void* _temp6985= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp6991; _LL6987: if(*(( void**) _temp6985) ==  Cyc_ExpList_tok){ _LL6992:
_temp6991=(( struct Cyc_ExpList_tok_struct*) _temp6985)->f1; goto _LL6988;}
else{ goto _LL6989;} _LL6989: goto _LL6990; _LL6988: yyzzz= _temp6991; goto
_LL6986; _LL6990:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL6986:;}
yyzzz;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp6976;}); _temp6975;}); break; case 375:
_LL6974: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6994=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6994[ 0]=({ struct Cyc_Exp_tok_struct
_temp6995; _temp6995.tag= Cyc_Exp_tok; _temp6995.f1= Cyc_Absyn_structmember_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp6996= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp7002; _LL6998: if(*(( void**) _temp6996) ==  Cyc_Exp_tok){ _LL7003:
_temp7002=(( struct Cyc_Exp_tok_struct*) _temp6996)->f1; goto _LL6999;} else{
goto _LL7000;} _LL7000: goto _LL7001; _LL6999: yyzzz= _temp7002; goto _LL6997;
_LL7001:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL6997:;} yyzzz;}),({
struct _tagged_arr* _temp7004=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp7004[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp7005= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp7011; _LL7007: if(*(( void**) _temp7005) ==  Cyc_String_tok){ _LL7012:
_temp7011=(( struct Cyc_String_tok_struct*) _temp7005)->f1; goto _LL7008;} else{
goto _LL7009;} _LL7009: goto _LL7010; _LL7008: yyzzz= _temp7011; goto _LL7006;
_LL7010:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL7006:;} yyzzz;});
_temp7004;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp6995;}); _temp6994;}); break; case 376:
_LL6993: { struct _tuple1* q=({ struct _tuple1* yyzzz;{ void* _temp7024= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp7030; _LL7026: if(*(( void**) _temp7024) ==  Cyc_QualId_tok){ _LL7031:
_temp7030=(( struct Cyc_QualId_tok_struct*) _temp7024)->f1; goto _LL7027;} else{
goto _LL7028;} _LL7028: goto _LL7029; _LL7027: yyzzz= _temp7030; goto _LL7025;
_LL7029:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7025:;} yyzzz;}); if(
Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp7014=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7014[ 0]=({ struct Cyc_Exp_tok_struct
_temp7015; _temp7015.tag= Cyc_Exp_tok; _temp7015.f1= Cyc_Absyn_structmember_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7016= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp7022; _LL7018: if(*(( void**) _temp7016) ==  Cyc_Exp_tok){ _LL7023:
_temp7022=(( struct Cyc_Exp_tok_struct*) _temp7016)->f1; goto _LL7019;} else{
goto _LL7020;} _LL7020: goto _LL7021; _LL7019: yyzzz= _temp7022; goto _LL7017;
_LL7021:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7017:;} yyzzz;}),(* q).f2,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp7015;}); _temp7014;}); break;} case 377:
_LL7013: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7033=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7033[ 0]=({ struct Cyc_Exp_tok_struct
_temp7034; _temp7034.tag= Cyc_Exp_tok; _temp7034.f1= Cyc_Absyn_structarrow_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7035= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp7041; _LL7037: if(*(( void**) _temp7035) ==  Cyc_Exp_tok){ _LL7042:
_temp7041=(( struct Cyc_Exp_tok_struct*) _temp7035)->f1; goto _LL7038;} else{
goto _LL7039;} _LL7039: goto _LL7040; _LL7038: yyzzz= _temp7041; goto _LL7036;
_LL7040:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7036:;} yyzzz;}),({
struct _tagged_arr* _temp7043=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp7043[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp7044= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp7050; _LL7046: if(*(( void**) _temp7044) ==  Cyc_String_tok){ _LL7051:
_temp7050=(( struct Cyc_String_tok_struct*) _temp7044)->f1; goto _LL7047;} else{
goto _LL7048;} _LL7048: goto _LL7049; _LL7047: yyzzz= _temp7050; goto _LL7045;
_LL7049:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL7045:;} yyzzz;});
_temp7043;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp7034;}); _temp7033;}); break; case 378:
_LL7032: { struct _tuple1* q=({ struct _tuple1* yyzzz;{ void* _temp7063= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp7069; _LL7065: if(*(( void**) _temp7063) ==  Cyc_QualId_tok){ _LL7070:
_temp7069=(( struct Cyc_QualId_tok_struct*) _temp7063)->f1; goto _LL7066;} else{
goto _LL7067;} _LL7067: goto _LL7068; _LL7066: yyzzz= _temp7069; goto _LL7064;
_LL7068:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7064:;} yyzzz;}); if(
Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp7053=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7053[ 0]=({ struct Cyc_Exp_tok_struct
_temp7054; _temp7054.tag= Cyc_Exp_tok; _temp7054.f1= Cyc_Absyn_structarrow_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7055= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Exp*
_temp7061; _LL7057: if(*(( void**) _temp7055) ==  Cyc_Exp_tok){ _LL7062:
_temp7061=(( struct Cyc_Exp_tok_struct*) _temp7055)->f1; goto _LL7058;} else{
goto _LL7059;} _LL7059: goto _LL7060; _LL7058: yyzzz= _temp7061; goto _LL7056;
_LL7060:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7056:;} yyzzz;}),(* q).f2,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp7054;}); _temp7053;}); break;} case 379:
_LL7052: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7072=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7072[ 0]=({ struct Cyc_Exp_tok_struct
_temp7073; _temp7073.tag= Cyc_Exp_tok; _temp7073.f1= Cyc_Absyn_post_inc_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7074= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp7080; _LL7076: if(*(( void**) _temp7074) ==  Cyc_Exp_tok){ _LL7081:
_temp7080=(( struct Cyc_Exp_tok_struct*) _temp7074)->f1; goto _LL7077;} else{
goto _LL7078;} _LL7078: goto _LL7079; _LL7077: yyzzz= _temp7080; goto _LL7075;
_LL7079:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7075:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7073;}); _temp7072;}); break; case 380: _LL7071: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7083=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7083[ 0]=({ struct Cyc_Exp_tok_struct
_temp7084; _temp7084.tag= Cyc_Exp_tok; _temp7084.f1= Cyc_Absyn_post_dec_exp(({
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7085= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Exp*
_temp7091; _LL7087: if(*(( void**) _temp7085) ==  Cyc_Exp_tok){ _LL7092:
_temp7091=(( struct Cyc_Exp_tok_struct*) _temp7085)->f1; goto _LL7088;} else{
goto _LL7089;} _LL7089: goto _LL7090; _LL7088: yyzzz= _temp7091; goto _LL7086;
_LL7090:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7086:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7084;}); _temp7083;}); break; case 381: _LL7082: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7094=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7094[ 0]=({ struct Cyc_Exp_tok_struct
_temp7095; _temp7095.tag= Cyc_Exp_tok; _temp7095.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp7096=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7096[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7097; _temp7097.tag= Cyc_Absyn_CompoundLit_e;
_temp7097.f1=({ struct _tuple2* yyzzz;{ void* _temp7098= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct _tuple2*
_temp7104; _LL7100: if(*(( void**) _temp7098) ==  Cyc_ParamDecl_tok){ _LL7105:
_temp7104=(( struct Cyc_ParamDecl_tok_struct*) _temp7098)->f1; goto _LL7101;}
else{ goto _LL7102;} _LL7102: goto _LL7103; _LL7101: yyzzz= _temp7104; goto
_LL7099; _LL7103:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL7099:;}
yyzzz;}); _temp7097.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp7106= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7112; _LL7108: if(*(( void**) _temp7106) ==  Cyc_InitializerList_tok){
_LL7113: _temp7112=(( struct Cyc_InitializerList_tok_struct*) _temp7106)->f1;
goto _LL7109;} else{ goto _LL7110;} _LL7110: goto _LL7111; _LL7109: yyzzz=
_temp7112; goto _LL7107; _LL7111:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL7107:;} yyzzz;})); _temp7097;}); _temp7096;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7095;}); _temp7094;}); break; case 382: _LL7093: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7115=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7115[ 0]=({ struct Cyc_Exp_tok_struct
_temp7116; _temp7116.tag= Cyc_Exp_tok; _temp7116.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp7117=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7117[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7118; _temp7118.tag= Cyc_Absyn_CompoundLit_e;
_temp7118.f1=({ struct _tuple2* yyzzz;{ void* _temp7119= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]; struct _tuple2*
_temp7125; _LL7121: if(*(( void**) _temp7119) ==  Cyc_ParamDecl_tok){ _LL7126:
_temp7125=(( struct Cyc_ParamDecl_tok_struct*) _temp7119)->f1; goto _LL7122;}
else{ goto _LL7123;} _LL7123: goto _LL7124; _LL7122: yyzzz= _temp7125; goto
_LL7120; _LL7124:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL7120:;}
yyzzz;}); _temp7118.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp7127= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp7133; _LL7129: if(*(( void**) _temp7127) ==  Cyc_InitializerList_tok){
_LL7134: _temp7133=(( struct Cyc_InitializerList_tok_struct*) _temp7127)->f1;
goto _LL7130;} else{ goto _LL7131;} _LL7131: goto _LL7132; _LL7130: yyzzz=
_temp7133; goto _LL7128; _LL7132:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL7128:;} yyzzz;})); _temp7118;}); _temp7117;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7116;}); _temp7115;}); break; case 383: _LL7114: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7136=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7136[ 0]=({ struct Cyc_Exp_tok_struct
_temp7137; _temp7137.tag= Cyc_Exp_tok; _temp7137.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp7138=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7138[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7139; _temp7139.tag= Cyc_Absyn_Fill_e; _temp7139.f1=({ struct Cyc_Absyn_Exp*
yyzzz;{ void* _temp7140= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; struct Cyc_Absyn_Exp* _temp7146; _LL7142: if(*(( void**)
_temp7140) ==  Cyc_Exp_tok){ _LL7147: _temp7146=(( struct Cyc_Exp_tok_struct*)
_temp7140)->f1; goto _LL7143;} else{ goto _LL7144;} _LL7144: goto _LL7145;
_LL7143: yyzzz= _temp7146; goto _LL7141; _LL7145:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL7141:;} yyzzz;}); _temp7139;}); _temp7138;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7137;}); _temp7136;}); break; case 384: _LL7135: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7149=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7149[ 0]=({ struct Cyc_Exp_tok_struct
_temp7150; _temp7150.tag= Cyc_Exp_tok; _temp7150.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp7151=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7151[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7152; _temp7152.tag= Cyc_Absyn_Codegen_e;
_temp7152.f1=({ struct Cyc_Absyn_Fndecl* yyzzz;{ void* _temp7153= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_Absyn_Fndecl*
_temp7159; _LL7155: if(*(( void**) _temp7153) ==  Cyc_FnDecl_tok){ _LL7160:
_temp7159=(( struct Cyc_FnDecl_tok_struct*) _temp7153)->f1; goto _LL7156;} else{
goto _LL7157;} _LL7157: goto _LL7158; _LL7156: yyzzz= _temp7159; goto _LL7154;
_LL7158:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL7154:;} yyzzz;});
_temp7152;}); _temp7151;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7150;});
_temp7149;}); break; case 385: _LL7148: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp7162=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7162[ 0]=({ struct Cyc_Exp_tok_struct _temp7163; _temp7163.tag= Cyc_Exp_tok;
_temp7163.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7164=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7164[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7165; _temp7165.tag=
Cyc_Absyn_UnknownId_e; _temp7165.f1=({ struct _tuple1* yyzzz;{ void* _temp7166=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple1*
_temp7172; _LL7168: if(*(( void**) _temp7166) ==  Cyc_QualId_tok){ _LL7173:
_temp7172=(( struct Cyc_QualId_tok_struct*) _temp7166)->f1; goto _LL7169;} else{
goto _LL7170;} _LL7170: goto _LL7171; _LL7169: yyzzz= _temp7172; goto _LL7167;
_LL7171:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7167:;} yyzzz;});
_temp7165;}); _temp7164;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7163;});
_temp7162;}); break; case 386: _LL7161: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 387: _LL7174:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7176=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7176[ 0]=({ struct Cyc_Exp_tok_struct
_temp7177; _temp7177.tag= Cyc_Exp_tok; _temp7177.f1= Cyc_Absyn_string_exp(({
struct _tagged_arr yyzzz;{ void* _temp7178= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tagged_arr _temp7184; _LL7180: if(*(( void**)
_temp7178) ==  Cyc_String_tok){ _LL7185: _temp7184=(( struct Cyc_String_tok_struct*)
_temp7178)->f1; goto _LL7181;} else{ goto _LL7182;} _LL7182: goto _LL7183;
_LL7181: yyzzz= _temp7184; goto _LL7179; _LL7183:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL7179:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7177;});
_temp7176;}); break; case 388: _LL7175: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 389:
_LL7186: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7188=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7188[ 0]=({ struct Cyc_Exp_tok_struct
_temp7189; _temp7189.tag= Cyc_Exp_tok; _temp7189.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7190=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7190[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7191; _temp7191.tag=
Cyc_Absyn_UnknownId_e; _temp7191.f1=({ struct _tuple1* yyzzz;{ void* _temp7192=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct
_tuple1* _temp7198; _LL7194: if(*(( void**) _temp7192) ==  Cyc_QualId_tok){
_LL7199: _temp7198=(( struct Cyc_QualId_tok_struct*) _temp7192)->f1; goto
_LL7195;} else{ goto _LL7196;} _LL7196: goto _LL7197; _LL7195: yyzzz= _temp7198;
goto _LL7193; _LL7197:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7193:;}
yyzzz;}); _temp7191;}); _temp7190;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7189;}); _temp7188;}); break; case 390: _LL7187: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7201=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7201[ 0]=({ struct Cyc_Exp_tok_struct
_temp7202; _temp7202.tag= Cyc_Exp_tok; _temp7202.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7203=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7203[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7204; _temp7204.tag=
Cyc_Absyn_UnknownId_e; _temp7204.f1=({ struct _tuple1* yyzzz;{ void* _temp7205=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]; struct
_tuple1* _temp7211; _LL7207: if(*(( void**) _temp7205) ==  Cyc_QualId_tok){
_LL7212: _temp7211=(( struct Cyc_QualId_tok_struct*) _temp7205)->f1; goto
_LL7208;} else{ goto _LL7209;} _LL7209: goto _LL7210; _LL7208: yyzzz= _temp7211;
goto _LL7206; _LL7210:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7206:;}
yyzzz;}); _temp7204;}); _temp7203;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev(({
struct Cyc_List_List* yyzzz;{ void* _temp7213= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7219; _LL7215: if(*(( void**) _temp7213) ==  Cyc_TypeList_tok){ _LL7220:
_temp7219=(( struct Cyc_TypeList_tok_struct*) _temp7213)->f1; goto _LL7216;}
else{ goto _LL7217;} _LL7217: goto _LL7218; _LL7216: yyzzz= _temp7219; goto
_LL7214; _LL7218:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL7214:;}
yyzzz;})), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp7202;}); _temp7201;}); break; case 391:
_LL7200: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7222=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7222[ 0]=({ struct Cyc_Exp_tok_struct
_temp7223; _temp7223.tag= Cyc_Exp_tok; _temp7223.f1= Cyc_Absyn_tuple_exp(({
struct Cyc_List_List* yyzzz;{ void* _temp7224= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7230; _LL7226: if(*(( void**) _temp7224) ==  Cyc_ExpList_tok){ _LL7231:
_temp7230=(( struct Cyc_ExpList_tok_struct*) _temp7224)->f1; goto _LL7227;}
else{ goto _LL7228;} _LL7228: goto _LL7229; _LL7227: yyzzz= _temp7230; goto
_LL7225; _LL7229:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL7225:;}
yyzzz;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp7223;}); _temp7222;}); break; case 392:
_LL7221: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7233=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7233[ 0]=({ struct Cyc_Exp_tok_struct
_temp7234; _temp7234.tag= Cyc_Exp_tok; _temp7234.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp7235=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7235[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7236; _temp7236.tag= Cyc_Absyn_Struct_e;
_temp7236.f1=({ struct _tuple1* yyzzz;{ void* _temp7237= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]; struct _tuple1*
_temp7243; _LL7239: if(*(( void**) _temp7237) ==  Cyc_QualId_tok){ _LL7244:
_temp7243=(( struct Cyc_QualId_tok_struct*) _temp7237)->f1; goto _LL7240;} else{
goto _LL7241;} _LL7241: goto _LL7242; _LL7240: yyzzz= _temp7243; goto _LL7238;
_LL7242:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL7238:;} yyzzz;});
_temp7236.f2= 0; _temp7236.f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{ void* _temp7245= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; struct Cyc_List_List*
_temp7251; _LL7247: if(*(( void**) _temp7245) ==  Cyc_InitializerList_tok){
_LL7252: _temp7251=(( struct Cyc_InitializerList_tok_struct*) _temp7245)->f1;
goto _LL7248;} else{ goto _LL7249;} _LL7249: goto _LL7250; _LL7248: yyzzz=
_temp7251; goto _LL7246; _LL7250:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL7246:;} yyzzz;})); _temp7236.f4= 0; _temp7236;}); _temp7235;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7234;}); _temp7233;}); break; case 393: _LL7232: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7254=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7254[ 0]=({ struct Cyc_Exp_tok_struct
_temp7255; _temp7255.tag= Cyc_Exp_tok; _temp7255.f1= Cyc_Absyn_stmt_exp(({
struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp7256= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_Absyn_Stmt*
_temp7262; _LL7258: if(*(( void**) _temp7256) ==  Cyc_Stmt_tok){ _LL7263:
_temp7262=(( struct Cyc_Stmt_tok_struct*) _temp7256)->f1; goto _LL7259;} else{
goto _LL7260;} _LL7260: goto _LL7261; _LL7259: yyzzz= _temp7262; goto _LL7257;
_LL7261:( int) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL7257:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp7255;}); _temp7254;}); break; case 394: _LL7253: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp7265=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp7265[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7266; _temp7266.tag= Cyc_ExpList_tok; _temp7266.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* yyzzz;{
void* _temp7267= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
struct Cyc_List_List* _temp7273; _LL7269: if(*(( void**) _temp7267) ==  Cyc_ExpList_tok){
_LL7274: _temp7273=(( struct Cyc_ExpList_tok_struct*) _temp7267)->f1; goto
_LL7270;} else{ goto _LL7271;} _LL7271: goto _LL7272; _LL7270: yyzzz= _temp7273;
goto _LL7268; _LL7272:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL7268:;}
yyzzz;})); _temp7266;}); _temp7265;}); break; case 395: _LL7264: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp7276=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7276[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7277; _temp7277.tag= Cyc_ExpList_tok; _temp7277.f1=({ struct Cyc_List_List*
_temp7278=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7278->hd=( void*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7279= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp7285; _LL7281: if(*(( void**) _temp7279) ==  Cyc_Exp_tok){ _LL7286:
_temp7285=(( struct Cyc_Exp_tok_struct*) _temp7279)->f1; goto _LL7282;} else{
goto _LL7283;} _LL7283: goto _LL7284; _LL7282: yyzzz= _temp7285; goto _LL7280;
_LL7284:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7280:;} yyzzz;});
_temp7278->tl= 0; _temp7278;}); _temp7277;}); _temp7276;}); break; case 396:
_LL7275: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp7288=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7288[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7289; _temp7289.tag= Cyc_ExpList_tok; _temp7289.f1=({ struct Cyc_List_List*
_temp7290=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7290->hd=( void*)({ struct Cyc_Absyn_Exp* yyzzz;{ void* _temp7299= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct Cyc_Absyn_Exp*
_temp7305; _LL7301: if(*(( void**) _temp7299) ==  Cyc_Exp_tok){ _LL7306:
_temp7305=(( struct Cyc_Exp_tok_struct*) _temp7299)->f1; goto _LL7302;} else{
goto _LL7303;} _LL7303: goto _LL7304; _LL7302: yyzzz= _temp7305; goto _LL7300;
_LL7304:( int) _throw(( void*)& Cyc_yyfail_Exp_tok); _LL7300:;} yyzzz;});
_temp7290->tl=({ struct Cyc_List_List* yyzzz;{ void* _temp7291= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]; struct Cyc_List_List*
_temp7297; _LL7293: if(*(( void**) _temp7291) ==  Cyc_ExpList_tok){ _LL7298:
_temp7297=(( struct Cyc_ExpList_tok_struct*) _temp7291)->f1; goto _LL7294;}
else{ goto _LL7295;} _LL7295: goto _LL7296; _LL7294: yyzzz= _temp7297; goto
_LL7292; _LL7296:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL7292:;}
yyzzz;}); _temp7290;}); _temp7289;}); _temp7288;}); break; case 397: _LL7287:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7308=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7308[ 0]=({ struct Cyc_Exp_tok_struct
_temp7309; _temp7309.tag= Cyc_Exp_tok; _temp7309.f1= Cyc_Absyn_int_exp((*({
struct _tuple15* yyzzz;{ void* _temp7310= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; struct _tuple15* _temp7316; _LL7312: if(*(( void**)
_temp7310) ==  Cyc_Int_tok){ _LL7317: _temp7316=(( struct Cyc_Int_tok_struct*)
_temp7310)->f1; goto _LL7313;} else{ goto _LL7314;} _LL7314: goto _LL7315;
_LL7313: yyzzz= _temp7316; goto _LL7311; _LL7315:( int) _throw(( void*)& Cyc_yyfail_Int_tok);
_LL7311:;} yyzzz;})).f1,(*({ struct _tuple15* yyzzz;{ void* _temp7318= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tuple15*
_temp7324; _LL7320: if(*(( void**) _temp7318) ==  Cyc_Int_tok){ _LL7325:
_temp7324=(( struct Cyc_Int_tok_struct*) _temp7318)->f1; goto _LL7321;} else{
goto _LL7322;} _LL7322: goto _LL7323; _LL7321: yyzzz= _temp7324; goto _LL7319;
_LL7323:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL7319:;} yyzzz;})).f2,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp7309;}); _temp7308;}); break; case 398:
_LL7307: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7327=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7327[ 0]=({ struct Cyc_Exp_tok_struct
_temp7328; _temp7328.tag= Cyc_Exp_tok; _temp7328.f1= Cyc_Absyn_char_exp(({
unsigned char yyzzz;{ void* _temp7329= yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]; unsigned char _temp7335; _LL7331: if(*(( void**)
_temp7329) ==  Cyc_Char_tok){ _LL7336: _temp7335=(( struct Cyc_Char_tok_struct*)
_temp7329)->f1; goto _LL7332;} else{ goto _LL7333;} _LL7333: goto _LL7334;
_LL7332: yyzzz= _temp7335; goto _LL7330; _LL7334:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL7330:;} yyzzz;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7328;});
_temp7327;}); break; case 399: _LL7326: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp7338=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7338[ 0]=({ struct Cyc_Exp_tok_struct _temp7339; _temp7339.tag= Cyc_Exp_tok;
_temp7339.f1= Cyc_Absyn_float_exp(({ struct _tagged_arr yyzzz;{ void* _temp7340=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp7346; _LL7342: if(*(( void**) _temp7340) ==  Cyc_String_tok){ _LL7347:
_temp7346=(( struct Cyc_String_tok_struct*) _temp7340)->f1; goto _LL7343;} else{
goto _LL7344;} _LL7344: goto _LL7345; _LL7343: yyzzz= _temp7346; goto _LL7341;
_LL7345:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL7341:;} yyzzz;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7339;});
_temp7338;}); break; case 400: _LL7337: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp7349=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7349[ 0]=({ struct Cyc_Exp_tok_struct _temp7350; _temp7350.tag= Cyc_Exp_tok;
_temp7350.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp7350;});
_temp7349;}); break; case 401: _LL7348: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp7352=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp7352[ 0]=({ struct Cyc_QualId_tok_struct _temp7353; _temp7353.tag= Cyc_QualId_tok;
_temp7353.f1=({ struct _tuple1* _temp7354=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp7354->f1= Cyc_Absyn_rel_ns_null; _temp7354->f2=({ struct
_tagged_arr* _temp7355=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp7355[ 0]=({ struct _tagged_arr yyzzz;{ void* _temp7356= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; struct _tagged_arr
_temp7362; _LL7358: if(*(( void**) _temp7356) ==  Cyc_String_tok){ _LL7363:
_temp7362=(( struct Cyc_String_tok_struct*) _temp7356)->f1; goto _LL7359;} else{
goto _LL7360;} _LL7360: goto _LL7361; _LL7359: yyzzz= _temp7362; goto _LL7357;
_LL7361:( int) _throw(( void*)& Cyc_yyfail_String_tok); _LL7357:;} yyzzz;});
_temp7355;}); _temp7354;}); _temp7353;}); _temp7352;}); break; case 402: _LL7351:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break;
default: _LL7364:( int) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -=
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
yystate <=  4424: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4425u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4425u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
120)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 796u, yystate)]; if( yyn >  -
32768? yyn <  4424: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  232u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4425u, x +  yyn)] ==  x){( sze
+= Cyc_std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 232u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp7366=( unsigned int)( sze +  15);
unsigned char* _temp7367=( unsigned char*) GC_malloc_atomic( sizeof(
unsigned char) *  _temp7366); struct _tagged_arr _temp7369= _tag_arr( _temp7367,
sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int _temp7368=
_temp7366; unsigned int i; for( i= 0; i <  _temp7368; i ++){ _temp7367[ i]='\000';}};
_temp7369;}); Cyc_std_strcpy( msg, _tag_arr("parse error", sizeof( unsigned char),
12u)); if( count <  5){ count= 0; for( x= yyn <  0? - yyn: 0; x <  232u / 
sizeof( unsigned char*); x ++){ if( Cyc_yycheck[ _check_known_subscript_notnull(
4425u, x +  yyn)] ==  x){ Cyc_std_strcat( msg, count ==  0? _tag_arr(", expecting `",
sizeof( unsigned char), 14u): _tag_arr(" or `", sizeof( unsigned char), 6u));
Cyc_std_strcat( msg, Cyc_yytname[ _check_known_subscript_notnull( 232u, x)]);
Cyc_std_strcat( msg, _tag_arr("'", sizeof( unsigned char), 2u)); count ++;}}}
Cyc_yyerror(( struct _tagged_arr) msg);} else{ Cyc_yyerror( _tag_arr("parse error",
sizeof( unsigned char), 12u));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 
3){ if( Cyc_yychar ==  0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset ==  0){ return 1;}
yyvsp_offset --; yystate=( int) yyss[ _check_known_subscript_notnull( 10000u, --
yyssp_offset)]; yylsp_offset --; yyerrhandle: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 796u, yystate)]; if( yyn ==  - 32768){ goto
yyerrdefault;} yyn += 1; if(( yyn <  0? 1: yyn >  4424)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4425u, yyn)] !=  1){ goto yyerrdefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4425u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrpop;}} if( yyn ==  795){ return 0;} yyvs[
_check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval; yyls[
_check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; goto
yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp7379= v; struct
_tuple15* _temp7397; struct _tuple15 _temp7399; int _temp7400; unsigned char
_temp7402; short _temp7404; struct _tagged_arr _temp7406; struct Cyc_Core_Opt*
_temp7408; struct Cyc_Core_Opt* _temp7410; struct Cyc_Core_Opt _temp7412; struct
_tagged_arr* _temp7413; struct _tuple1* _temp7415; struct _tuple1 _temp7417;
struct _tagged_arr* _temp7418; void* _temp7420; _LL7381: if(*(( void**)
_temp7379) ==  Cyc_Int_tok){ _LL7398: _temp7397=(( struct Cyc_Int_tok_struct*)
_temp7379)->f1; _temp7399=* _temp7397; _LL7401: _temp7400= _temp7399.f2; goto
_LL7382;} else{ goto _LL7383;} _LL7383: if(*(( void**) _temp7379) ==  Cyc_Char_tok){
_LL7403: _temp7402=(( struct Cyc_Char_tok_struct*) _temp7379)->f1; goto _LL7384;}
else{ goto _LL7385;} _LL7385: if(*(( void**) _temp7379) ==  Cyc_Short_tok){
_LL7405: _temp7404=(( struct Cyc_Short_tok_struct*) _temp7379)->f1; goto _LL7386;}
else{ goto _LL7387;} _LL7387: if(*(( void**) _temp7379) ==  Cyc_String_tok){
_LL7407: _temp7406=(( struct Cyc_String_tok_struct*) _temp7379)->f1; goto
_LL7388;} else{ goto _LL7389;} _LL7389: if(*(( void**) _temp7379) ==  Cyc_Stringopt_tok){
_LL7409: _temp7408=(( struct Cyc_Stringopt_tok_struct*) _temp7379)->f1; if(
_temp7408 ==  0){ goto _LL7390;} else{ goto _LL7391;}} else{ goto _LL7391;}
_LL7391: if(*(( void**) _temp7379) ==  Cyc_Stringopt_tok){ _LL7411: _temp7410=((
struct Cyc_Stringopt_tok_struct*) _temp7379)->f1; if( _temp7410 ==  0){ goto
_LL7393;} else{ _temp7412=* _temp7410; _LL7414: _temp7413=( struct _tagged_arr*)
_temp7412.v; goto _LL7392;}} else{ goto _LL7393;} _LL7393: if(*(( void**)
_temp7379) ==  Cyc_QualId_tok){ _LL7416: _temp7415=(( struct Cyc_QualId_tok_struct*)
_temp7379)->f1; _temp7417=* _temp7415; _LL7421: _temp7420= _temp7417.f1; goto
_LL7419; _LL7419: _temp7418= _temp7417.f2; goto _LL7394;} else{ goto _LL7395;}
_LL7395: goto _LL7396; _LL7382:({ struct Cyc_std_Int_pa_struct _temp7423;
_temp7423.tag= Cyc_std_Int_pa; _temp7423.f1=( int)(( unsigned int) _temp7400);{
void* _temp7422[ 1u]={& _temp7423}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp7422, sizeof( void*), 1u));}}); goto
_LL7380; _LL7384:({ struct Cyc_std_Int_pa_struct _temp7425; _temp7425.tag= Cyc_std_Int_pa;
_temp7425.f1=( int)(( unsigned int)(( int) _temp7402));{ void* _temp7424[ 1u]={&
_temp7425}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp7424, sizeof( void*), 1u));}}); goto _LL7380;
_LL7386:({ struct Cyc_std_Int_pa_struct _temp7427; _temp7427.tag= Cyc_std_Int_pa;
_temp7427.f1=( int)(( unsigned int)(( int) _temp7404));{ void* _temp7426[ 1u]={&
_temp7427}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp7426, sizeof( void*), 1u));}}); goto _LL7380;
_LL7388:({ struct Cyc_std_String_pa_struct _temp7429; _temp7429.tag= Cyc_std_String_pa;
_temp7429.f1=( struct _tagged_arr) _temp7406;{ void* _temp7428[ 1u]={& _temp7429};
Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp7428, sizeof( void*), 1u));}}); goto _LL7380; _LL7390:({ void*
_temp7430[ 0u]={}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp7430, sizeof( void*), 0u));}); goto _LL7380;
_LL7392:({ struct Cyc_std_String_pa_struct _temp7432; _temp7432.tag= Cyc_std_String_pa;
_temp7432.f1=( struct _tagged_arr)* _temp7413;{ void* _temp7431[ 1u]={&
_temp7432}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp7431, sizeof( void*), 1u));}}); goto _LL7380;
_LL7394: { struct Cyc_List_List* _temp7433= 0;{ void* _temp7434= _temp7420;
struct Cyc_List_List* _temp7442; struct Cyc_List_List* _temp7444; _LL7436: if((
unsigned int) _temp7434 >  1u?*(( int*) _temp7434) ==  Cyc_Absyn_Rel_n: 0){
_LL7443: _temp7442=(( struct Cyc_Absyn_Rel_n_struct*) _temp7434)->f1; goto
_LL7437;} else{ goto _LL7438;} _LL7438: if(( unsigned int) _temp7434 >  1u?*((
int*) _temp7434) ==  Cyc_Absyn_Abs_n: 0){ _LL7445: _temp7444=(( struct Cyc_Absyn_Abs_n_struct*)
_temp7434)->f1; goto _LL7439;} else{ goto _LL7440;} _LL7440: if( _temp7434 == (
void*) Cyc_Absyn_Loc_n){ goto _LL7441;} else{ goto _LL7435;} _LL7437: _temp7433=
_temp7442; goto _LL7435; _LL7439: _temp7433= _temp7444; goto _LL7435; _LL7441:
goto _LL7435; _LL7435:;} for( 0; _temp7433 !=  0; _temp7433=(( struct Cyc_List_List*)
_check_null( _temp7433))->tl){({ struct Cyc_std_String_pa_struct _temp7447;
_temp7447.tag= Cyc_std_String_pa; _temp7447.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp7433))->hd);{ void*
_temp7446[ 1u]={& _temp7447}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp7446, sizeof( void*), 1u));}});}({
struct Cyc_std_String_pa_struct _temp7449; _temp7449.tag= Cyc_std_String_pa;
_temp7449.f1=( struct _tagged_arr)* _temp7418;{ void* _temp7448[ 1u]={&
_temp7449}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp7448, sizeof( void*), 1u));}}); goto _LL7380;}
_LL7396:({ void* _temp7450[ 0u]={}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp7450, sizeof( void*), 0u));}); goto
_LL7380; _LL7380:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7451=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7451->v=(
void*) Cyc_Lexing_from_file( f); _temp7451;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
