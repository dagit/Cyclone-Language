 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_arr lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_arr, int, void*); void* read_fun_state; } ;
struct Cyc_Lexing_lex_tables{ struct _tagged_arr lex_base; struct _tagged_arr
lex_backtrk; struct _tagged_arr lex_default; struct _tagged_arr lex_trans;
struct _tagged_arr lex_check; } ; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Stdio___sFILE*); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual
x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
extern struct _tuple1* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one;
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
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct _tagged_arr
_temp17=( struct _tagged_arr) Cyc_Position_string_of_segment( sg); struct
_tagged_arr _temp18= msg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: %.*s\n",
_get_arr_size( _temp17, 1u), _temp17.curr, _get_arr_size( _temp18, 1u), _temp18.curr);});
return;} static void* Cyc_Parse_unimp( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ return(( void*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_arr)({ struct _tagged_arr _temp19= msg;
xprintf("%.*s unimplemented", _get_arr_size( _temp19, 1u), _temp19.curr);}), sg);}
static void Cyc_Parse_unimp2( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){({ struct _tagged_arr _temp20=( struct _tagged_arr) Cyc_Position_string_of_segment(
sg); struct _tagged_arr _temp21= msg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: Cyclone does not yet support %.*s\n",
_get_arr_size( _temp20, 1u), _temp20.curr, _get_arr_size( _temp21, 1u), _temp21.curr);});
return;} struct _tuple9{ struct _tuple7* f1; struct Cyc_Core_Opt* f2; } ; static
struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field( struct Cyc_Position_Segment*
loc, struct _tuple9* field_info){ struct Cyc_Core_Opt* _temp24; struct _tuple7*
_temp26; struct _tuple9 _temp22=* field_info; _LL27: _temp26= _temp22.f1; goto
_LL25; _LL25: _temp24= _temp22.f2; goto _LL23; _LL23: if((* _temp26).f4 != 0){
Cyc_Parse_err( _tag_arr("bad type params in struct field", sizeof( unsigned char),
32u), loc);}{ struct _tuple1* _temp28=(* _temp26).f1; goto _LL29; _LL29:{ void*
_temp30=(* _temp28).f1; struct Cyc_List_List* _temp40; struct Cyc_List_List*
_temp42; _LL32: if(( unsigned int) _temp30 > 1u?*(( int*) _temp30) == Cyc_Absyn_Rel_n:
0){ _LL41: _temp40=(( struct Cyc_Absyn_Rel_n_struct*) _temp30)->f1; if( _temp40
== 0){ goto _LL33;} else{ goto _LL34;}} else{ goto _LL34;} _LL34: if((
unsigned int) _temp30 > 1u?*(( int*) _temp30) == Cyc_Absyn_Abs_n: 0){ _LL43:
_temp42=(( struct Cyc_Absyn_Abs_n_struct*) _temp30)->f1; if( _temp42 == 0){ goto
_LL35;} else{ goto _LL36;}} else{ goto _LL36;} _LL36: if( _temp30 ==( void*) Cyc_Absyn_Loc_n){
goto _LL37;} else{ goto _LL38;} _LL38: goto _LL39; _LL33: goto _LL31; _LL35:
goto _LL31; _LL37: goto _LL31; _LL39: Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a module name",
sizeof( unsigned char), 52u), loc); goto _LL31; _LL31:;}{ struct Cyc_List_List*
_temp44=(* _temp26).f5; goto _LL45; _LL45: return({ struct Cyc_Absyn_Structfield*
_temp46=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp46->name=(* _temp28).f2; _temp46->tq=(* _temp26).f2; _temp46->type=( void*)(*
_temp26).f3; _temp46->width= _temp24; _temp46->attributes= _temp44; _temp46;});}}}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp49; struct Cyc_List_List* _temp50; void* _temp52; struct Cyc_Absyn_Tqual
_temp54; struct Cyc_Core_Opt* _temp56; struct _tuple10* _temp47= field; _temp49=*
_temp47; _LL57: _temp56= _temp49.f1; goto _LL55; _LL55: _temp54= _temp49.f2;
goto _LL53; _LL53: _temp52= _temp49.f3; goto _LL51; _LL51: _temp50= _temp49.f4;
goto _LL48; _LL48: { struct Cyc_Core_Opt* _temp58= 0; goto _LL59; _LL59: if(
_temp56 != 0){ _temp58=({ struct Cyc_Core_Opt* _temp60=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp60->v=( void*)(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp56))->v)).f2; _temp60;});{ void* _temp61=(*((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp56))->v)).f1; struct
Cyc_List_List* _temp71; struct Cyc_List_List* _temp73; _LL63: if(( unsigned int)
_temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_Rel_n: 0){ _LL72: _temp71=(( struct
Cyc_Absyn_Rel_n_struct*) _temp61)->f1; if( _temp71 == 0){ goto _LL64;} else{
goto _LL65;}} else{ goto _LL65;} _LL65: if(( unsigned int) _temp61 > 1u?*(( int*)
_temp61) == Cyc_Absyn_Abs_n: 0){ _LL74: _temp73=(( struct Cyc_Absyn_Abs_n_struct*)
_temp61)->f1; if( _temp73 == 0){ goto _LL66;} else{ goto _LL67;}} else{ goto
_LL67;} _LL67: if( _temp61 ==( void*) Cyc_Absyn_Loc_n){ goto _LL68;} else{ goto
_LL69;} _LL69: goto _LL70; _LL64: goto _LL62; _LL66: goto _LL62; _LL68: goto
_LL62; _LL70: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), loc); goto _LL62; _LL62:;}} if( _temp50 != 0){((
int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters", sizeof( unsigned char), 41u),
loc);} return({ struct _tuple2* _temp75=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp75->f1= _temp58; _temp75->f2= _temp54; _temp75->f3=
_temp52; _temp75;});}} static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment*
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
x->r; struct Cyc_Absyn_Vardecl* _temp119; int _temp121; struct Cyc_Absyn_Exp*
_temp123; struct Cyc_Core_Opt* _temp125; struct Cyc_Core_Opt* _temp127; struct
Cyc_Absyn_Pat* _temp129; struct Cyc_List_List* _temp131; struct Cyc_Absyn_Fndecl*
_temp133; struct Cyc_Absyn_Structdecl* _temp135; struct Cyc_Absyn_Uniondecl*
_temp137; struct Cyc_Absyn_Tuniondecl* _temp139; struct Cyc_Absyn_Typedefdecl*
_temp141; struct Cyc_Absyn_Enumdecl* _temp143; struct Cyc_List_List* _temp145;
struct _tagged_arr* _temp147; struct Cyc_List_List* _temp149; struct _tuple1*
_temp151; struct Cyc_List_List* _temp153; _LL95: if(*(( int*) _temp93) == Cyc_Absyn_Var_d){
_LL120: _temp119=(( struct Cyc_Absyn_Var_d_struct*) _temp93)->f1; goto _LL96;}
else{ goto _LL97;} _LL97: if(*(( int*) _temp93) == Cyc_Absyn_Let_d){ _LL130:
_temp129=(( struct Cyc_Absyn_Let_d_struct*) _temp93)->f1; goto _LL128; _LL128:
_temp127=(( struct Cyc_Absyn_Let_d_struct*) _temp93)->f2; goto _LL126; _LL126:
_temp125=(( struct Cyc_Absyn_Let_d_struct*) _temp93)->f3; goto _LL124; _LL124:
_temp123=(( struct Cyc_Absyn_Let_d_struct*) _temp93)->f4; goto _LL122; _LL122:
_temp121=(( struct Cyc_Absyn_Let_d_struct*) _temp93)->f5; goto _LL98;} else{
goto _LL99;} _LL99: if(*(( int*) _temp93) == Cyc_Absyn_Letv_d){ _LL132: _temp131=((
struct Cyc_Absyn_Letv_d_struct*) _temp93)->f1; goto _LL100;} else{ goto _LL101;}
_LL101: if(*(( int*) _temp93) == Cyc_Absyn_Fn_d){ _LL134: _temp133=(( struct Cyc_Absyn_Fn_d_struct*)
_temp93)->f1; goto _LL102;} else{ goto _LL103;} _LL103: if(*(( int*) _temp93) ==
Cyc_Absyn_Struct_d){ _LL136: _temp135=(( struct Cyc_Absyn_Struct_d_struct*)
_temp93)->f1; goto _LL104;} else{ goto _LL105;} _LL105: if(*(( int*) _temp93) ==
Cyc_Absyn_Union_d){ _LL138: _temp137=(( struct Cyc_Absyn_Union_d_struct*)
_temp93)->f1; goto _LL106;} else{ goto _LL107;} _LL107: if(*(( int*) _temp93) ==
Cyc_Absyn_Tunion_d){ _LL140: _temp139=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp93)->f1; goto _LL108;} else{ goto _LL109;} _LL109: if(*(( int*) _temp93) ==
Cyc_Absyn_Typedef_d){ _LL142: _temp141=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp93)->f1; goto _LL110;} else{ goto _LL111;} _LL111: if(*(( int*) _temp93) ==
Cyc_Absyn_Enum_d){ _LL144: _temp143=(( struct Cyc_Absyn_Enum_d_struct*) _temp93)->f1;
goto _LL112;} else{ goto _LL113;} _LL113: if(*(( int*) _temp93) == Cyc_Absyn_Namespace_d){
_LL148: _temp147=(( struct Cyc_Absyn_Namespace_d_struct*) _temp93)->f1; goto
_LL146; _LL146: _temp145=(( struct Cyc_Absyn_Namespace_d_struct*) _temp93)->f2;
goto _LL114;} else{ goto _LL115;} _LL115: if(*(( int*) _temp93) == Cyc_Absyn_Using_d){
_LL152: _temp151=(( struct Cyc_Absyn_Using_d_struct*) _temp93)->f1; goto _LL150;
_LL150: _temp149=(( struct Cyc_Absyn_Using_d_struct*) _temp93)->f2; goto _LL116;}
else{ goto _LL117;} _LL117: if(*(( int*) _temp93) == Cyc_Absyn_ExternC_d){
_LL154: _temp153=(( struct Cyc_Absyn_ExternC_d_struct*) _temp93)->f1; goto
_LL118;} else{ goto _LL94;} _LL96: if( _temp119->initializer != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);}{ void* _temp155=(* _temp119->name).f1; struct
Cyc_List_List* _temp165; struct Cyc_List_List* _temp167; _LL157: if( _temp155 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL158;} else{ goto _LL159;} _LL159: if((
unsigned int) _temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Rel_n: 0){ _LL166:
_temp165=(( struct Cyc_Absyn_Rel_n_struct*) _temp155)->f1; if( _temp165 == 0){
goto _LL160;} else{ goto _LL161;}} else{ goto _LL161;} _LL161: if(( unsigned int)
_temp155 > 1u?*(( int*) _temp155) == Cyc_Absyn_Abs_n: 0){ _LL168: _temp167=((
struct Cyc_Absyn_Abs_n_struct*) _temp155)->f1; if( _temp167 == 0){ goto _LL162;}
else{ goto _LL163;}} else{ goto _LL163;} _LL163: goto _LL164; _LL158: goto
_LL156; _LL160: goto _LL156; _LL162: goto _LL156; _LL164: Cyc_Parse_err(
_tag_arr("module names not allowed on parameter declarations", sizeof(
unsigned char), 51u), x->loc); goto _LL156; _LL156:;}{ int found= 0; for( 0;
params != 0; params=(( struct Cyc_List_List*) _check_null( params))->tl){ if(
Cyc_String_zstrptrcmp((* _temp119->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( params))->hd) == 0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct _tagged_arr _temp169=*(* _temp119->name).f2;
xprintf("%.*s is not listed as a parameter", _get_arr_size( _temp169, 1u),
_temp169.curr);}), x->loc);} return;} _LL98: decl_kind= _tag_arr("let declaration",
sizeof( unsigned char), 16u); goto _LL94; _LL100: decl_kind= _tag_arr("let declaration",
sizeof( unsigned char), 16u); goto _LL94; _LL102: decl_kind= _tag_arr("function declaration",
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
struct _tagged_arr _temp170= decl_kind; xprintf("%.*s appears in parameter type",
_get_arr_size( _temp170, 1u), _temp170.curr);}), x->loc); return;} struct
_tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_arr* x){ struct _tuple12 _temp173; struct Cyc_Position_Segment* _temp174;
struct Cyc_List_List* _temp176; struct _tuple12* _temp171= env; _temp173=*
_temp171; _LL177: _temp176= _temp173.f1; goto _LL175; _LL175: _temp174= _temp173.f2;
goto _LL172; _LL172: if( _temp176 == 0){ return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct _tagged_arr _temp178=* x; xprintf("missing type for parameter %.*s",
_get_arr_size( _temp178, 1u), _temp178.curr);}), _temp174);}{ void* _temp179=(
void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp176))->hd)->r;
struct Cyc_Absyn_Vardecl* _temp185; _LL181: if(*(( int*) _temp179) == Cyc_Absyn_Var_d){
_LL186: _temp185=(( struct Cyc_Absyn_Var_d_struct*) _temp179)->f1; goto _LL182;}
else{ goto _LL183;} _LL183: goto _LL184; _LL182:{ void* _temp187=(* _temp185->name).f1;
struct Cyc_List_List* _temp197; struct Cyc_List_List* _temp199; _LL189: if(
_temp187 ==( void*) Cyc_Absyn_Loc_n){ goto _LL190;} else{ goto _LL191;} _LL191:
if(( unsigned int) _temp187 > 1u?*(( int*) _temp187) == Cyc_Absyn_Rel_n: 0){
_LL198: _temp197=(( struct Cyc_Absyn_Rel_n_struct*) _temp187)->f1; if( _temp197
== 0){ goto _LL192;} else{ goto _LL193;}} else{ goto _LL193;} _LL193: if((
unsigned int) _temp187 > 1u?*(( int*) _temp187) == Cyc_Absyn_Abs_n: 0){ _LL200:
_temp199=(( struct Cyc_Absyn_Abs_n_struct*) _temp187)->f1; if( _temp199 == 0){
goto _LL194;} else{ goto _LL195;}} else{ goto _LL195;} _LL195: goto _LL196;
_LL190: goto _LL188; _LL192: goto _LL188; _LL194: goto _LL188; _LL196: Cyc_Parse_err(
_tag_arr("module name not allowed on parameter", sizeof( unsigned char), 37u),
_temp174); goto _LL188; _LL188:;} if( Cyc_String_zstrptrcmp((* _temp185->name).f2,
x) == 0){ return({ struct _tuple2* _temp201=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2) * 1); _temp201[ 0]=({ struct _tuple2 _temp202; _temp202.f1=({
struct Cyc_Core_Opt* _temp203=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp203[ 0]=({ struct Cyc_Core_Opt _temp204; _temp204.v=(
void*)(* _temp185->name).f2; _temp204;}); _temp203;}); _temp202.f2= _temp185->tq;
_temp202.f3=( void*) _temp185->type; _temp202;}); _temp201;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple12* _temp205=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp205->f1=(( struct Cyc_List_List*) _check_null( _temp176))->tl; _temp205->f2=
_temp174; _temp205;}), x);} _LL184: return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration",
sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp176))->hd)->loc); _LL180:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp206= tm; int _temp212; struct Cyc_Position_Segment*
_temp214; struct Cyc_List_List* _temp216; _LL208: if(( unsigned int) _temp206 >
1u?*(( int*) _temp206) == Cyc_Absyn_TypeParams_mod: 0){ _LL217: _temp216=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp206)->f1; goto _LL215; _LL215:
_temp214=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp206)->f2; goto _LL213;
_LL213: _temp212=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp206)->f3; goto
_LL209;} else{ goto _LL210;} _LL210: goto _LL211; _LL209: return 1; _LL211:
return 0; _LL207:;} static void* Cyc_Parse_id2type( struct _tagged_arr s, struct
Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s, _tag_arr("`H", sizeof(
unsigned char), 3u)) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return(
void*)({ struct Cyc_Absyn_VarType_struct* _temp218=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp218[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp219; _temp219.tag= Cyc_Absyn_VarType; _temp219.f1=({ struct Cyc_Absyn_Tvar*
_temp220=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp220->name=({ struct _tagged_arr* _temp221=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp221[ 0]= s; _temp221;}); _temp220->identity=
0; _temp220->kind= k; _temp220;}); _temp219;}); _temp218;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp222=
t; struct Cyc_Absyn_Tvar* _temp228; _LL224: if(( unsigned int) _temp222 > 4u?*((
int*) _temp222) == Cyc_Absyn_VarType: 0){ _LL229: _temp228=(( struct Cyc_Absyn_VarType_struct*)
_temp222)->f1; goto _LL225;} else{ goto _LL226;} _LL226: goto _LL227; _LL225:
return _temp228; _LL227: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL223:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp230=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp230[ 0]=({ struct Cyc_Absyn_VarType_struct _temp231; _temp231.tag= Cyc_Absyn_VarType;
_temp231.f1= pr; _temp231;}); _temp230;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp232=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp238; _LL234:
if(( unsigned int) _temp232 > 1u?*(( int*) _temp232) == Cyc_Absyn_Function_mod:
0){ _LL239: _temp238=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp232)->f1;
goto _LL235;} else{ goto _LL236;} _LL236: goto _LL237; _LL235: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp240= _temp238; struct Cyc_List_List* _temp246; struct Cyc_Core_Opt*
_temp248; struct Cyc_Absyn_VarargInfo* _temp250; int _temp252; struct Cyc_List_List*
_temp254; struct Cyc_Position_Segment* _temp256; struct Cyc_List_List* _temp258;
_LL242: if(*(( int*) _temp240) == Cyc_Absyn_WithTypes){ _LL255: _temp254=((
struct Cyc_Absyn_WithTypes_struct*) _temp240)->f1; goto _LL253; _LL253: _temp252=((
struct Cyc_Absyn_WithTypes_struct*) _temp240)->f2; goto _LL251; _LL251: _temp250=((
struct Cyc_Absyn_WithTypes_struct*) _temp240)->f3; goto _LL249; _LL249: _temp248=((
struct Cyc_Absyn_WithTypes_struct*) _temp240)->f4; goto _LL247; _LL247: _temp246=((
struct Cyc_Absyn_WithTypes_struct*) _temp240)->f5; goto _LL243;} else{ goto
_LL244;} _LL244: if(*(( int*) _temp240) == Cyc_Absyn_NoTypes){ _LL259: _temp258=((
struct Cyc_Absyn_NoTypes_struct*) _temp240)->f1; goto _LL257; _LL257: _temp256=((
struct Cyc_Absyn_NoTypes_struct*) _temp240)->f2; goto _LL245;} else{ goto _LL241;}
_LL243: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL245:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp258,
tds);{ struct _tuple12* _temp261=({ struct _tuple12* _temp260=( struct _tuple12*)
GC_malloc( sizeof( struct _tuple12)); _temp260->f1= tds; _temp260->f2= loc;
_temp260;}); goto _LL262; _LL262: return({ struct Cyc_List_List* _temp263=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp263->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp264=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp264[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp265; _temp265.tag= Cyc_Absyn_Function_mod;
_temp265.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp266=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp266[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp267; _temp267.tag= Cyc_Absyn_WithTypes;
_temp267.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp261, _temp258); _temp267.f2= 0; _temp267.f3= 0;
_temp267.f4= 0; _temp267.f5= 0; _temp267;}); _temp266;})); _temp265;}); _temp264;}));
_temp263->tl= 0; _temp263;});} _LL241:;} else{ return({ struct Cyc_List_List*
_temp268=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp268->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp268->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp268;});} _LL237: return({ struct Cyc_List_List*
_temp269=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp269->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp269->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp269;}); _LL233:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp270=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp270->id= d->id; _temp270->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp270;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp271=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL273: if( _temp271 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL274;} else{ goto _LL275;} _LL275: if(
_temp271 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL276;} else{ goto _LL277;}
_LL277: if( _temp271 ==( void*) Cyc_Parse_Static_sc){ goto _LL278;} else{ goto
_LL279;} _LL279: goto _LL280; _LL274: sc=( void*) Cyc_Absyn_Extern; goto _LL272;
_LL276: sc=( void*) Cyc_Absyn_ExternC; goto _LL272; _LL278: sc=( void*) Cyc_Absyn_Static;
goto _LL272; _LL280: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL272; _LL272:;}}{ struct Cyc_Core_Opt*
_temp283; void* _temp285; struct _tuple5 _temp281= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL286: _temp285= _temp281.f1; goto _LL284; _LL284: _temp283=
_temp281.f2; goto _LL282; _LL282: { struct Cyc_List_List* _temp289; struct Cyc_List_List*
_temp291; void* _temp293; struct Cyc_Absyn_Tqual _temp295; struct _tuple6
_temp287= Cyc_Parse_apply_tms( tq, _temp285, atts, d->tms); _LL296: _temp295=
_temp287.f1; goto _LL294; _LL294: _temp293= _temp287.f2; goto _LL292; _LL292:
_temp291= _temp287.f3; goto _LL290; _LL290: _temp289= _temp287.f4; goto _LL288;
_LL288: if( _temp283 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp291 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp297= _temp293; struct Cyc_Absyn_FnInfo _temp303; struct Cyc_List_List*
_temp305; struct Cyc_List_List* _temp307; struct Cyc_Absyn_VarargInfo* _temp309;
int _temp311; struct Cyc_List_List* _temp313; void* _temp315; struct Cyc_Core_Opt*
_temp317; struct Cyc_List_List* _temp319; _LL299: if(( unsigned int) _temp297 >
4u?*(( int*) _temp297) == Cyc_Absyn_FnType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_FnType_struct*)
_temp297)->f1; _LL320: _temp319= _temp303.tvars; goto _LL318; _LL318: _temp317=
_temp303.effect; goto _LL316; _LL316: _temp315=( void*) _temp303.ret_typ; goto
_LL314; _LL314: _temp313= _temp303.args; goto _LL312; _LL312: _temp311= _temp303.c_varargs;
goto _LL310; _LL310: _temp309= _temp303.cyc_varargs; goto _LL308; _LL308:
_temp307= _temp303.rgn_po; goto _LL306; _LL306: _temp305= _temp303.attributes;
goto _LL300;} else{ goto _LL301;} _LL301: goto _LL302; _LL300: { struct Cyc_List_List*
_temp321=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp313); goto _LL322; _LL322:
return({ struct Cyc_Absyn_Fndecl* _temp323=( struct Cyc_Absyn_Fndecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Fndecl)); _temp323->sc=( void*) sc; _temp323->name= d->id;
_temp323->tvs= _temp319; _temp323->is_inline= is_inline; _temp323->effect=
_temp317; _temp323->ret_type=( void*) _temp315; _temp323->args= _temp321;
_temp323->c_varargs= _temp311; _temp323->cyc_varargs= _temp309; _temp323->rgn_po=
_temp307; _temp323->body= body; _temp323->cached_typ= 0; _temp323->param_vardecls=
0; _temp323->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp305, _temp289); _temp323;});}
_LL302: return(( struct Cyc_Absyn_Fndecl*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("declarator is not a function prototype",
sizeof( unsigned char), 39u), loc); _LL298:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp324=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp324->f1=({ struct _tagged_arr*
_temp325=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp325[ 0]= _tag_arr("?", sizeof( unsigned char), 2u); _temp325;}); _temp324->f2=(*
t).f2; _temp324->f3=(* t).f3; _temp324;});} else{ return({ struct _tuple4*
_temp326=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp326->f1=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp326->f2=(*
t).f2; _temp326->f3=(* t).f3; _temp326;});}} static unsigned char _temp327[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp327, _temp327, _temp327 + 52u};
static unsigned char _temp328[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp328, _temp328, _temp328 + 63u};
static unsigned char _temp329[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp329, _temp329, _temp329 + 50u};
static unsigned char _temp330[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp330, _temp330, _temp330 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp331=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp345; void* _temp347; struct Cyc_Position_Segment*
_temp349; struct Cyc_Position_Segment* _temp351; struct Cyc_Position_Segment*
_temp353; struct Cyc_Position_Segment* _temp355; struct Cyc_Absyn_Decl* _temp357;
_LL333: if(*(( int*) _temp331) == Cyc_Parse_Type_spec){ _LL348: _temp347=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp331)->f1; goto _LL346; _LL346: _temp345=((
struct Cyc_Parse_Type_spec_struct*) _temp331)->f2; goto _LL334;} else{ goto
_LL335;} _LL335: if(*(( int*) _temp331) == Cyc_Parse_Signed_spec){ _LL350:
_temp349=(( struct Cyc_Parse_Signed_spec_struct*) _temp331)->f1; goto _LL336;}
else{ goto _LL337;} _LL337: if(*(( int*) _temp331) == Cyc_Parse_Unsigned_spec){
_LL352: _temp351=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp331)->f1; goto
_LL338;} else{ goto _LL339;} _LL339: if(*(( int*) _temp331) == Cyc_Parse_Short_spec){
_LL354: _temp353=(( struct Cyc_Parse_Short_spec_struct*) _temp331)->f1; goto
_LL340;} else{ goto _LL341;} _LL341: if(*(( int*) _temp331) == Cyc_Parse_Long_spec){
_LL356: _temp355=(( struct Cyc_Parse_Long_spec_struct*) _temp331)->f1; goto
_LL342;} else{ goto _LL343;} _LL343: if(*(( int*) _temp331) == Cyc_Parse_Decl_spec){
_LL358: _temp357=(( struct Cyc_Parse_Decl_spec_struct*) _temp331)->f1; goto
_LL344;} else{ goto _LL332;} _LL334: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp345);} last_loc= _temp345; seen_type= 1; t= _temp347; goto _LL332; _LL336:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp349);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp349);} last_loc= _temp349; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL332; _LL338: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp351);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp351);} last_loc= _temp351; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL332; _LL340: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp353);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp353);} last_loc= _temp353; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL332; _LL342: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp355);} if( seen_size){ void* _temp359= sz; _LL361: if( _temp359 ==( void*)
Cyc_Absyn_B4){ goto _LL362;} else{ goto _LL363;} _LL363: goto _LL364; _LL362: sz=(
void*) Cyc_Absyn_B8; goto _LL360; _LL364: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp355); goto _LL360; _LL360:;} last_loc=
_temp355; seen_size= 1; goto _LL332; _LL344: last_loc= _temp357->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp365=( void*) _temp357->r;
struct Cyc_Absyn_Structdecl* _temp377; struct Cyc_Absyn_Tuniondecl* _temp379;
struct Cyc_Absyn_Uniondecl* _temp381; struct Cyc_Absyn_Enumdecl* _temp383;
_LL367: if(*(( int*) _temp365) == Cyc_Absyn_Struct_d){ _LL378: _temp377=((
struct Cyc_Absyn_Struct_d_struct*) _temp365)->f1; goto _LL368;} else{ goto
_LL369;} _LL369: if(*(( int*) _temp365) == Cyc_Absyn_Tunion_d){ _LL380: _temp379=((
struct Cyc_Absyn_Tunion_d_struct*) _temp365)->f1; goto _LL370;} else{ goto
_LL371;} _LL371: if(*(( int*) _temp365) == Cyc_Absyn_Union_d){ _LL382: _temp381=((
struct Cyc_Absyn_Union_d_struct*) _temp365)->f1; goto _LL372;} else{ goto _LL373;}
_LL373: if(*(( int*) _temp365) == Cyc_Absyn_Enum_d){ _LL384: _temp383=(( struct
Cyc_Absyn_Enum_d_struct*) _temp365)->f1; goto _LL374;} else{ goto _LL375;}
_LL375: goto _LL376; _LL368: { struct Cyc_List_List* _temp385=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp377->tvs); goto _LL386; _LL386: t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp387=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp387[ 0]=({ struct Cyc_Absyn_StructType_struct _temp388; _temp388.tag= Cyc_Absyn_StructType;
_temp388.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp377->name))->v); _temp388.f2= _temp385; _temp388.f3= 0;
_temp388;}); _temp387;}); if( _temp377->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp389=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp389->v=( void*) _temp357; _temp389;});} goto _LL366;} _LL370: { struct Cyc_List_List*
_temp390=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp379->tvs);
goto _LL391; _LL391: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp392=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp392[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp393; _temp393.tag= Cyc_Absyn_TunionType;
_temp393.f1=({ struct Cyc_Absyn_TunionInfo _temp394; _temp394.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp395=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp395[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp396; _temp396.tag= Cyc_Absyn_KnownTunion;
_temp396.f1= _temp379; _temp396;}); _temp395;})); _temp394.targs= _temp390;
_temp394.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp394;}); _temp393;});
_temp392;}); if( _temp379->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp397=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp397->v=( void*) _temp357; _temp397;});} goto _LL366;} _LL372: { struct Cyc_List_List*
_temp398=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp381->tvs);
goto _LL399; _LL399: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp400=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp400[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp401; _temp401.tag= Cyc_Absyn_UnionType;
_temp401.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp381->name))->v); _temp401.f2= _temp398; _temp401.f3= 0;
_temp401;}); _temp400;}); if( _temp381->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp402=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp402->v=( void*) _temp357; _temp402;});} goto _LL366;} _LL374: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp403=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp403[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp404; _temp404.tag= Cyc_Absyn_EnumType; _temp404.f1=
_temp383->name; _temp404.f2= 0; _temp404;}); _temp403;}); declopt=({ struct Cyc_Core_Opt*
_temp405=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp405->v=( void*) _temp357; _temp405;}); goto _LL366; _LL376:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp357->loc); goto _LL366; _LL366:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp357->loc);} goto _LL332; _LL332:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp406=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp406[ 0]=({ struct Cyc_Absyn_IntType_struct _temp407; _temp407.tag= Cyc_Absyn_IntType;
_temp407.f1=( void*) sgn; _temp407.f2=( void*) sz; _temp407;}); _temp406;});}
else{ if( seen_sign){ void* _temp408= t; void* _temp414; void* _temp416; _LL410:
if(( unsigned int) _temp408 > 4u?*(( int*) _temp408) == Cyc_Absyn_IntType: 0){
_LL417: _temp416=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp408)->f1;
goto _LL415; _LL415: _temp414=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp408)->f2; goto _LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL411: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp418=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp418[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp419; _temp419.tag= Cyc_Absyn_IntType; _temp419.f1=( void*) sgn; _temp419.f2=(
void*) _temp414; _temp419;}); _temp418;}); goto _LL409; _LL413: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL409; _LL409:;} if( seen_size){ void* _temp420= t; void*
_temp426; void* _temp428; _LL422: if(( unsigned int) _temp420 > 4u?*(( int*)
_temp420) == Cyc_Absyn_IntType: 0){ _LL429: _temp428=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp420)->f1; goto _LL427; _LL427: _temp426=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp420)->f2; goto _LL423;} else{ goto _LL424;} _LL424: goto _LL425; _LL423: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp430=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp430[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp431; _temp431.tag= Cyc_Absyn_IntType; _temp431.f1=( void*) _temp428;
_temp431.f2=( void*) sz; _temp431;}); _temp430;}); goto _LL421; _LL425: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL421; _LL421:;}} return({ struct _tuple5 _temp432; _temp432.f1=
t; _temp432.f2= declopt; _temp432;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp433=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL434; _LL434: { struct _tuple1* _temp435= _temp433->id; goto _LL436;
_LL436: { struct Cyc_List_List* _temp439; struct Cyc_List_List* _temp441; void*
_temp443; struct Cyc_Absyn_Tqual _temp445; struct _tuple6 _temp437= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp433->tms); _LL446: _temp445= _temp437.f1; goto _LL444;
_LL444: _temp443= _temp437.f2; goto _LL442; _LL442: _temp441= _temp437.f3; goto
_LL440; _LL440: _temp439= _temp437.f4; goto _LL438; _LL438: return({ struct Cyc_List_List*
_temp447=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp447->hd=( void*)({ struct _tuple7* _temp448=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp448->f1= _temp435; _temp448->f2= _temp445;
_temp448->f3= _temp443; _temp448->f4= _temp441; _temp448->f5= _temp439; _temp448;});
_temp447->tl= Cyc_Parse_apply_tmss( _temp445, Cyc_Tcutil_copy_type( t),(( struct
Cyc_List_List*) _check_null( ds))->tl, shared_atts); _temp447;});}}}} static
struct _tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual tq, void* t, struct
Cyc_List_List* atts, struct Cyc_List_List* tms){ if( tms == 0){ return({ struct
_tuple6 _temp449; _temp449.f1= tq; _temp449.f2= t; _temp449.f3= 0; _temp449.f4=
atts; _temp449;});}{ void* _temp450=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp464; void* _temp466; int
_temp468; struct Cyc_Position_Segment* _temp470; struct Cyc_List_List* _temp472;
struct Cyc_Absyn_Tqual _temp474; void* _temp476; void* _temp478; struct Cyc_List_List*
_temp480; struct Cyc_Position_Segment* _temp482; _LL452: if( _temp450 ==( void*)
Cyc_Absyn_Carray_mod){ goto _LL453;} else{ goto _LL454;} _LL454: if((
unsigned int) _temp450 > 1u?*(( int*) _temp450) == Cyc_Absyn_ConstArray_mod: 0){
_LL465: _temp464=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp450)->f1; goto
_LL455;} else{ goto _LL456;} _LL456: if(( unsigned int) _temp450 > 1u?*(( int*)
_temp450) == Cyc_Absyn_Function_mod: 0){ _LL467: _temp466=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp450)->f1; goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int)
_temp450 > 1u?*(( int*) _temp450) == Cyc_Absyn_TypeParams_mod: 0){ _LL473:
_temp472=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp450)->f1; goto _LL471;
_LL471: _temp470=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp450)->f2; goto
_LL469; _LL469: _temp468=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp450)->f3;
goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int) _temp450 > 1u?*((
int*) _temp450) == Cyc_Absyn_Pointer_mod: 0){ _LL479: _temp478=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp450)->f1; goto _LL477; _LL477: _temp476=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp450)->f2; goto _LL475;
_LL475: _temp474=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp450)->f3; goto
_LL461;} else{ goto _LL462;} _LL462: if(( unsigned int) _temp450 > 1u?*(( int*)
_temp450) == Cyc_Absyn_Attributes_mod: 0){ _LL483: _temp482=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp450)->f1; goto _LL481; _LL481: _temp480=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp450)->f2; goto _LL463;} else{ goto _LL451;} _LL453: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp484=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp484[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp485; _temp485.tag= Cyc_Absyn_ArrayType;
_temp485.f1=( void*) t; _temp485.f2= tq; _temp485.f3= 0; _temp485;}); _temp484;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL455: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp486=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp486[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp487; _temp487.tag= Cyc_Absyn_ArrayType;
_temp487.f1=( void*) t; _temp487.f2= tq; _temp487.f3=( struct Cyc_Absyn_Exp*)
_temp464; _temp487;}); _temp486;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL457: { void* _temp488= _temp466; struct Cyc_List_List* _temp494;
struct Cyc_Core_Opt* _temp496; struct Cyc_Absyn_VarargInfo* _temp498; int
_temp500; struct Cyc_List_List* _temp502; struct Cyc_Position_Segment* _temp504;
struct Cyc_List_List* _temp506; _LL490: if(*(( int*) _temp488) == Cyc_Absyn_WithTypes){
_LL503: _temp502=(( struct Cyc_Absyn_WithTypes_struct*) _temp488)->f1; goto
_LL501; _LL501: _temp500=(( struct Cyc_Absyn_WithTypes_struct*) _temp488)->f2;
goto _LL499; _LL499: _temp498=(( struct Cyc_Absyn_WithTypes_struct*) _temp488)->f3;
goto _LL497; _LL497: _temp496=(( struct Cyc_Absyn_WithTypes_struct*) _temp488)->f4;
goto _LL495; _LL495: _temp494=(( struct Cyc_Absyn_WithTypes_struct*) _temp488)->f5;
goto _LL491;} else{ goto _LL492;} _LL492: if(*(( int*) _temp488) == Cyc_Absyn_NoTypes){
_LL507: _temp506=(( struct Cyc_Absyn_NoTypes_struct*) _temp488)->f1; goto _LL505;
_LL505: _temp504=(( struct Cyc_Absyn_NoTypes_struct*) _temp488)->f2; goto _LL493;}
else{ goto _LL489;} _LL491: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as != 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp508=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp508->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp508->tl= fn_atts; _temp508;});} else{ new_atts=({ struct Cyc_List_List*
_temp509=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp509->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp509->tl= new_atts; _temp509;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp510=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; int _temp516; struct Cyc_Position_Segment*
_temp518; struct Cyc_List_List* _temp520; _LL512: if(( unsigned int) _temp510 >
1u?*(( int*) _temp510) == Cyc_Absyn_TypeParams_mod: 0){ _LL521: _temp520=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp510)->f1; goto _LL519; _LL519:
_temp518=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp510)->f2; goto _LL517;
_LL517: _temp516=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp510)->f3; goto
_LL513;} else{ goto _LL514;} _LL514: goto _LL515; _LL513: typvars= _temp520; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL511; _LL515: goto _LL511;
_LL511:;} if((((( ! _temp500? _temp498 == 0: 0)? _temp502 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp502))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp502))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp502))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp502= 0;}
t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp502); return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ( typvars, _temp496, t, _temp502,
_temp500, _temp498, _temp494, fn_atts), new_atts,(( struct Cyc_List_List*)
_check_null( tms))->tl);} _LL493:( int) _throw((( void*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp504)); _LL489:;} _LL459: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0){ return({ struct _tuple6 _temp522; _temp522.f1= tq;
_temp522.f2= t; _temp522.f3= _temp472; _temp522.f4= atts; _temp522;});}( int)
_throw((( void*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)( _tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp470)); _LL461: { void* _temp523= _temp478;
struct Cyc_Absyn_Exp* _temp531; struct Cyc_Absyn_Exp* _temp533; _LL525: if((
unsigned int) _temp523 > 1u?*(( int*) _temp523) == Cyc_Absyn_NonNullable_ps: 0){
_LL532: _temp531=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp523)->f1; goto
_LL526;} else{ goto _LL527;} _LL527: if(( unsigned int) _temp523 > 1u?*(( int*)
_temp523) == Cyc_Absyn_Nullable_ps: 0){ _LL534: _temp533=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp523)->f1; goto _LL528;} else{ goto _LL529;} _LL529: if( _temp523 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL530;} else{ goto _LL524;} _LL526: return Cyc_Parse_apply_tms(
_temp474, Cyc_Absyn_atb_typ( t, _temp476, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp535=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp535[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp536; _temp536.tag= Cyc_Absyn_Upper_b;
_temp536.f1= _temp531; _temp536;}); _temp535;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL528: return Cyc_Parse_apply_tms( _temp474, Cyc_Absyn_starb_typ(
t, _temp476, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp537=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp537[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp538; _temp538.tag= Cyc_Absyn_Upper_b;
_temp538.f1= _temp533; _temp538;}); _temp537;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL530: return Cyc_Parse_apply_tms( _temp474, Cyc_Absyn_tagged_typ(
t, _temp476, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL524:;}
_LL463: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp480),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL451:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp541; void* _temp543; struct _tuple5 _temp539= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL544: _temp543= _temp539.f1; goto _LL542; _LL542: _temp541=
_temp539.f2; goto _LL540; _LL540: if( _temp541 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp543;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp545=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp545[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp546; _temp546.tag= Cyc_Absyn_Decl_s;
_temp546.f1= d; _temp546.f2= s; _temp546;}); _temp545;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc != 0){ void* _temp547=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL549: if( _temp547 ==(
void*) Cyc_Parse_Typedef_sc){ goto _LL550;} else{ goto _LL551;} _LL551: if(
_temp547 ==( void*) Cyc_Parse_Extern_sc){ goto _LL552;} else{ goto _LL553;}
_LL553: if( _temp547 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL554;} else{ goto
_LL555;} _LL555: if( _temp547 ==( void*) Cyc_Parse_Static_sc){ goto _LL556;}
else{ goto _LL557;} _LL557: if( _temp547 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL558;} else{ goto _LL559;} _LL559: if( _temp547 ==( void*) Cyc_Parse_Register_sc){
goto _LL560;} else{ goto _LL561;} _LL561: if( _temp547 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL562;} else{ goto _LL548;} _LL550: istypedef= 1; goto _LL548; _LL552: s=(
void*) Cyc_Absyn_Extern; goto _LL548; _LL554: s=( void*) Cyc_Absyn_ExternC; goto
_LL548; _LL556: s=( void*) Cyc_Absyn_Static; goto _LL548; _LL558: s=( void*) Cyc_Absyn_Public;
goto _LL548; _LL560: s=( void*) Cyc_Absyn_Public; goto _LL548; _LL562: s=( void*)
Cyc_Absyn_Abstract; goto _LL548; _LL548:;}{ struct Cyc_List_List* _temp565;
struct Cyc_List_List* _temp567; struct _tuple0 _temp563=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL568: _temp567= _temp563.f1;
goto _LL566; _LL566: _temp565= _temp563.f2; goto _LL564; _LL564: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp565; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp569= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL570; _LL570: if( _temp567 == 0){ struct Cyc_Core_Opt* _temp573; void*
_temp575; struct _tuple5 _temp571= _temp569; _LL576: _temp575= _temp571.f1; goto
_LL574; _LL574: _temp573= _temp571.f2; goto _LL572; _LL572: if( _temp573 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp573))->v;{ void* _temp577=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp589; struct Cyc_Absyn_Structdecl* _temp591; struct Cyc_Absyn_Uniondecl*
_temp593; struct Cyc_Absyn_Tuniondecl* _temp595; _LL579: if(*(( int*) _temp577)
== Cyc_Absyn_Enum_d){ _LL590: _temp589=(( struct Cyc_Absyn_Enum_d_struct*)
_temp577)->f1; goto _LL580;} else{ goto _LL581;} _LL581: if(*(( int*) _temp577)
== Cyc_Absyn_Struct_d){ _LL592: _temp591=(( struct Cyc_Absyn_Struct_d_struct*)
_temp577)->f1; goto _LL582;} else{ goto _LL583;} _LL583: if(*(( int*) _temp577)
== Cyc_Absyn_Union_d){ _LL594: _temp593=(( struct Cyc_Absyn_Union_d_struct*)
_temp577)->f1; goto _LL584;} else{ goto _LL585;} _LL585: if(*(( int*) _temp577)
== Cyc_Absyn_Tunion_d){ _LL596: _temp595=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp577)->f1; goto _LL586;} else{ goto _LL587;} _LL587: goto _LL588; _LL580:(
void*)( _temp589->sc=( void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL578; _LL582:( void*)( _temp591->sc=(
void*) s); _temp591->attributes= atts; goto _LL578; _LL584:( void*)( _temp593->sc=(
void*) s); _temp593->attributes= atts; goto _LL578; _LL586:( void*)( _temp595->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL578; _LL588: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL578:;}
return({ struct Cyc_List_List* _temp597=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp597->hd=( void*) d; _temp597->tl= 0;
_temp597;});} else{ void* _temp598= _temp575; struct Cyc_Absyn_Structdecl**
_temp612; struct Cyc_List_List* _temp614; struct _tuple1* _temp616; struct Cyc_Absyn_TunionInfo
_temp618; void* _temp620; struct Cyc_List_List* _temp622; void* _temp624; struct
Cyc_Absyn_Tuniondecl* _temp626; struct Cyc_Absyn_TunionInfo _temp628; void*
_temp630; struct Cyc_List_List* _temp632; void* _temp634; struct Cyc_Absyn_UnknownTunionInfo
_temp636; int _temp638; struct _tuple1* _temp640; struct Cyc_Absyn_Uniondecl**
_temp642; struct Cyc_List_List* _temp644; struct _tuple1* _temp646; struct Cyc_Absyn_Enumdecl*
_temp648; struct _tuple1* _temp650; _LL600: if(( unsigned int) _temp598 > 4u?*((
int*) _temp598) == Cyc_Absyn_StructType: 0){ _LL617: _temp616=(( struct Cyc_Absyn_StructType_struct*)
_temp598)->f1; goto _LL615; _LL615: _temp614=(( struct Cyc_Absyn_StructType_struct*)
_temp598)->f2; goto _LL613; _LL613: _temp612=(( struct Cyc_Absyn_StructType_struct*)
_temp598)->f3; goto _LL601;} else{ goto _LL602;} _LL602: if(( unsigned int)
_temp598 > 4u?*(( int*) _temp598) == Cyc_Absyn_TunionType: 0){ _LL619: _temp618=((
struct Cyc_Absyn_TunionType_struct*) _temp598)->f1; _LL625: _temp624=( void*)
_temp618.tunion_info; if(*(( int*) _temp624) == Cyc_Absyn_KnownTunion){ _LL627:
_temp626=(( struct Cyc_Absyn_KnownTunion_struct*) _temp624)->f1; goto _LL623;}
else{ goto _LL604;} _LL623: _temp622= _temp618.targs; goto _LL621; _LL621:
_temp620=( void*) _temp618.rgn; goto _LL603;} else{ goto _LL604;} _LL604: if((
unsigned int) _temp598 > 4u?*(( int*) _temp598) == Cyc_Absyn_TunionType: 0){
_LL629: _temp628=(( struct Cyc_Absyn_TunionType_struct*) _temp598)->f1; _LL635:
_temp634=( void*) _temp628.tunion_info; if(*(( int*) _temp634) == Cyc_Absyn_UnknownTunion){
_LL637: _temp636=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp634)->f1;
_LL641: _temp640= _temp636.name; goto _LL639; _LL639: _temp638= _temp636.is_xtunion;
goto _LL633;} else{ goto _LL606;} _LL633: _temp632= _temp628.targs; goto _LL631;
_LL631: _temp630=( void*) _temp628.rgn; goto _LL605;} else{ goto _LL606;} _LL606:
if(( unsigned int) _temp598 > 4u?*(( int*) _temp598) == Cyc_Absyn_UnionType: 0){
_LL647: _temp646=(( struct Cyc_Absyn_UnionType_struct*) _temp598)->f1; goto
_LL645; _LL645: _temp644=(( struct Cyc_Absyn_UnionType_struct*) _temp598)->f2;
goto _LL643; _LL643: _temp642=(( struct Cyc_Absyn_UnionType_struct*) _temp598)->f3;
goto _LL607;} else{ goto _LL608;} _LL608: if(( unsigned int) _temp598 > 4u?*((
int*) _temp598) == Cyc_Absyn_EnumType: 0){ _LL651: _temp650=(( struct Cyc_Absyn_EnumType_struct*)
_temp598)->f1; goto _LL649; _LL649: _temp648=(( struct Cyc_Absyn_EnumType_struct*)
_temp598)->f2; goto _LL609;} else{ goto _LL610;} _LL610: goto _LL611; _LL601: {
struct Cyc_List_List* _temp652=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp614);
goto _LL653; _LL653: { struct Cyc_Absyn_Structdecl* _temp656=({ struct Cyc_Absyn_Structdecl*
_temp654=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp654->sc=( void*) s; _temp654->name=({ struct Cyc_Core_Opt* _temp655=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp655->v=(
void*)(( struct _tuple1*) _check_null( _temp616)); _temp655;}); _temp654->tvs=
_temp652; _temp654->fields= 0; _temp654->attributes= 0; _temp654;}); goto _LL657;
_LL657: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp658=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp658->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp659=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp659[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp660; _temp660.tag= Cyc_Absyn_Struct_d;
_temp660.f1= _temp656; _temp660;}); _temp659;}), loc); _temp658->tl= 0; _temp658;});}}
_LL603: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp661=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp661->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp662=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp662[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp663; _temp663.tag= Cyc_Absyn_Tunion_d;
_temp663.f1= _temp626; _temp663;}); _temp662;}), loc); _temp661->tl= 0; _temp661;});
_LL605: { struct Cyc_List_List* _temp664=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp632);
goto _LL665; _LL665: { struct Cyc_Absyn_Decl* _temp666= Cyc_Absyn_tunion_decl( s,
_temp640, _temp664, 0, _temp638, loc); goto _LL667; _LL667: if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on tunion", sizeof( unsigned char), 25u), loc);} return({
struct Cyc_List_List* _temp668=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp668->hd=( void*) _temp666; _temp668->tl= 0;
_temp668;});}} _LL607: { struct Cyc_List_List* _temp669=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp644); goto _LL670; _LL670: { struct Cyc_Absyn_Uniondecl* _temp673=({ struct
Cyc_Absyn_Uniondecl* _temp671=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Uniondecl)); _temp671->sc=( void*) s; _temp671->name=({ struct
Cyc_Core_Opt* _temp672=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp672->v=( void*)(( struct _tuple1*) _check_null( _temp646)); _temp672;});
_temp671->tvs= _temp669; _temp671->fields= 0; _temp671->attributes= 0; _temp671;});
goto _LL674; _LL674: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp675=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp675->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp676=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp676->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp677=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp677[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp678; _temp678.tag= Cyc_Absyn_Union_d;
_temp678.f1= _temp673; _temp678;}); _temp677;})); _temp676->loc= loc; _temp676;});
_temp675->tl= 0; _temp675;});}} _LL609: { struct Cyc_Absyn_Enumdecl* _temp680=({
struct Cyc_Absyn_Enumdecl* _temp679=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp679->sc=( void*) s; _temp679->name=
_temp650; _temp679->fields= 0; _temp679;}); goto _LL681; _LL681: if( atts != 0){
Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof( unsigned char), 23u),
loc);} return({ struct Cyc_List_List* _temp682=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp682->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp683=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp683->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp684=(
struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp684[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp685; _temp685.tag= Cyc_Absyn_Enum_d;
_temp685.f1= _temp680; _temp685;}); _temp684;})); _temp683->loc= loc; _temp683;});
_temp682->tl= 0; _temp682;});} _LL611: Cyc_Parse_err( _tag_arr("missing declarator",
sizeof( unsigned char), 19u), loc); return 0; _LL599:;}} else{ void* t= _temp569.f1;
struct Cyc_List_List* _temp686= Cyc_Parse_apply_tmss( tq, t, _temp567, atts);
goto _LL687; _LL687: if( istypedef){ if( ! exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp686); if( _temp569.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp569.f2))->v;{ void* _temp688=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp700; struct Cyc_Absyn_Tuniondecl* _temp702;
struct Cyc_Absyn_Uniondecl* _temp704; struct Cyc_Absyn_Enumdecl* _temp706;
_LL690: if(*(( int*) _temp688) == Cyc_Absyn_Struct_d){ _LL701: _temp700=((
struct Cyc_Absyn_Struct_d_struct*) _temp688)->f1; goto _LL691;} else{ goto
_LL692;} _LL692: if(*(( int*) _temp688) == Cyc_Absyn_Tunion_d){ _LL703: _temp702=((
struct Cyc_Absyn_Tunion_d_struct*) _temp688)->f1; goto _LL693;} else{ goto
_LL694;} _LL694: if(*(( int*) _temp688) == Cyc_Absyn_Union_d){ _LL705: _temp704=((
struct Cyc_Absyn_Union_d_struct*) _temp688)->f1; goto _LL695;} else{ goto _LL696;}
_LL696: if(*(( int*) _temp688) == Cyc_Absyn_Enum_d){ _LL707: _temp706=(( struct
Cyc_Absyn_Enum_d_struct*) _temp688)->f1; goto _LL697;} else{ goto _LL698;}
_LL698: goto _LL699; _LL691:( void*)( _temp700->sc=( void*) s); _temp700->attributes=
atts; atts= 0; goto _LL689; _LL693:( void*)( _temp702->sc=( void*) s); goto
_LL689; _LL695:( void*)( _temp704->sc=( void*) s); goto _LL689; _LL697:( void*)(
_temp706->sc=( void*) s); goto _LL689; _LL699: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL689; _LL689:;} decls=({ struct Cyc_List_List*
_temp708=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp708->hd=( void*) d; _temp708->tl= decls; _temp708;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct _tagged_arr _temp709= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in typedef",
_get_arr_size( _temp709, 1u), _temp709.curr);}), loc);} return decls;}} else{
if( _temp569.f2 != 0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp710= _temp686; goto _LL711; _LL711: for( 0; _temp710 != 0;(
_temp710=(( struct Cyc_List_List*) _check_null( _temp710))->tl, _temp565=((
struct Cyc_List_List*) _check_null( _temp565))->tl)){ struct _tuple7 _temp714;
struct Cyc_List_List* _temp715; struct Cyc_List_List* _temp717; void* _temp719;
struct Cyc_Absyn_Tqual _temp721; struct _tuple1* _temp723; struct _tuple7*
_temp712=( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp710))->hd;
_temp714=* _temp712; _LL724: _temp723= _temp714.f1; goto _LL722; _LL722:
_temp721= _temp714.f2; goto _LL720; _LL720: _temp719= _temp714.f3; goto _LL718;
_LL718: _temp717= _temp714.f4; goto _LL716; _LL716: _temp715= _temp714.f5; goto
_LL713; _LL713: if( _temp717 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), loc);} if( _temp565 == 0){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!",
sizeof( unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp725=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp565))->hd; goto
_LL726; _LL726: { struct Cyc_Absyn_Vardecl* _temp727= Cyc_Absyn_new_vardecl(
_temp723, _temp719, _temp725); goto _LL728; _LL728: _temp727->tq= _temp721;(
void*)( _temp727->sc=( void*) s); _temp727->attributes= _temp715;{ struct Cyc_Absyn_Decl*
_temp732=({ struct Cyc_Absyn_Decl* _temp729=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp729->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp730=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp730[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp731; _temp731.tag= Cyc_Absyn_Var_d;
_temp731.f1= _temp727; _temp731;}); _temp730;})); _temp729->loc= loc; _temp729;});
goto _LL733; _LL733: decls=({ struct Cyc_List_List* _temp734=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp734->hd=( void*) _temp732;
_temp734->tl= decls; _temp734;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_arr s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({ struct _tagged_arr _temp735= s;
xprintf("bad kind: %.*s", _get_arr_size( _temp735, 1u), _temp735.curr);}), loc);
return( void*) Cyc_Absyn_BoxKind;} else{ switch(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0))){ case 'A': _LL736:
return( void*) Cyc_Absyn_AnyKind; case 'M': _LL737: return( void*) Cyc_Absyn_MemKind;
case 'B': _LL738: return( void*) Cyc_Absyn_BoxKind; case 'R': _LL739: return(
void*) Cyc_Absyn_RgnKind; case 'E': _LL740: return( void*) Cyc_Absyn_EffKind;
default: _LL741: Cyc_Parse_err(( struct _tagged_arr)({ struct _tagged_arr
_temp743= s; xprintf("bad kind: %.*s", _get_arr_size( _temp743, 1u), _temp743.curr);}),
loc); return( void*) Cyc_Absyn_BoxKind;}}} static struct Cyc_List_List* Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( atts == 0){ return tms;} else{ return({ struct Cyc_List_List* _temp744=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp744->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp745=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp745[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp746; _temp746.tag= Cyc_Absyn_Attributes_mod;
_temp746.f1= loc; _temp746.f2= atts; _temp746;}); _temp745;})); _temp744->tl=
tms; _temp744;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(( struct
_tagged_arr)({ struct _tagged_arr _temp747= Cyc_Absyn_attribute2string(( void*)((
struct Cyc_List_List*) _check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_get_arr_size( _temp747, 1u), _temp747.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp748=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp748[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp749; _temp749.tag= Cyc_Absyn_Typedef_d; _temp749.f1=({
struct Cyc_Absyn_Typedefdecl* _temp750=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp750->name= x; _temp750->tvs=(*
t).f4; _temp750->defn=( void*)(* t).f3; _temp750;}); _temp749;}); _temp748;}),
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
unsigned char _temp752[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp752, _temp752, _temp752 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp753= yy1; struct _tuple16* _temp759; _LL755: if(*(( void**)
_temp753) == Cyc_Int_tok){ _LL760: _temp759=(( struct Cyc_Int_tok_struct*)
_temp753)->f1; goto _LL756;} else{ goto _LL757;} _LL757: goto _LL758; _LL756:
return _temp759; _LL758:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL754:;}
static unsigned char _temp762[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp762, _temp762, _temp762 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp763= yy1; struct
_tagged_arr _temp769; _LL765: if(*(( void**) _temp763) == Cyc_String_tok){
_LL770: _temp769=(( struct Cyc_String_tok_struct*) _temp763)->f1; goto _LL766;}
else{ goto _LL767;} _LL767: goto _LL768; _LL766: return _temp769; _LL768:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL764:;} static unsigned char _temp772[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp772, _temp772, _temp772 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp773= yy1; unsigned char _temp779; _LL775: if(*(( void**) _temp773) ==
Cyc_Char_tok){ _LL780: _temp779=(( struct Cyc_Char_tok_struct*) _temp773)->f1;
goto _LL776;} else{ goto _LL777;} _LL777: goto _LL778; _LL776: return _temp779;
_LL778:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL774:;} static
unsigned char _temp782[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp782, _temp782, _temp782 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp783= yy1; void*
_temp789; _LL785: if(*(( void**) _temp783) == Cyc_Pointer_Sort_tok){ _LL790:
_temp789=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp783)->f1; goto
_LL786;} else{ goto _LL787;} _LL787: goto _LL788; _LL786: return _temp789;
_LL788:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL784:;} static
unsigned char _temp792[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp792, _temp792, _temp792 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp793= yy1; struct Cyc_Absyn_Exp*
_temp799; _LL795: if(*(( void**) _temp793) == Cyc_Exp_tok){ _LL800: _temp799=((
struct Cyc_Exp_tok_struct*) _temp793)->f1; goto _LL796;} else{ goto _LL797;}
_LL797: goto _LL798; _LL796: return _temp799; _LL798:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL794:;} static unsigned char _temp802[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp802, _temp802, _temp802 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp803= yy1;
struct Cyc_List_List* _temp809; _LL805: if(*(( void**) _temp803) == Cyc_ExpList_tok){
_LL810: _temp809=(( struct Cyc_ExpList_tok_struct*) _temp803)->f1; goto _LL806;}
else{ goto _LL807;} _LL807: goto _LL808; _LL806: return _temp809; _LL808:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL804:;} static unsigned char
_temp812[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp812, _temp812, _temp812 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp813= yy1; struct Cyc_List_List*
_temp819; _LL815: if(*(( void**) _temp813) == Cyc_InitializerList_tok){ _LL820:
_temp819=(( struct Cyc_InitializerList_tok_struct*) _temp813)->f1; goto _LL816;}
else{ goto _LL817;} _LL817: goto _LL818; _LL816: return _temp819; _LL818:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL814:;} static unsigned char
_temp822[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp822, _temp822, _temp822 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp823= yy1; void* _temp829; _LL825: if(*(( void**) _temp823)
== Cyc_Primop_tok){ _LL830: _temp829=( void*)(( struct Cyc_Primop_tok_struct*)
_temp823)->f1; goto _LL826;} else{ goto _LL827;} _LL827: goto _LL828; _LL826:
return _temp829; _LL828:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL824:;}
static unsigned char _temp832[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp832, _temp832, _temp832 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp833= yy1;
struct Cyc_Core_Opt* _temp839; _LL835: if(*(( void**) _temp833) == Cyc_Primopopt_tok){
_LL840: _temp839=(( struct Cyc_Primopopt_tok_struct*) _temp833)->f1; goto _LL836;}
else{ goto _LL837;} _LL837: goto _LL838; _LL836: return _temp839; _LL838:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL834:;} static unsigned char
_temp842[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp842, _temp842, _temp842 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp843= yy1; struct _tuple1* _temp849; _LL845: if(*(( void**)
_temp843) == Cyc_QualId_tok){ _LL850: _temp849=(( struct Cyc_QualId_tok_struct*)
_temp843)->f1; goto _LL846;} else{ goto _LL847;} _LL847: goto _LL848; _LL846:
return _temp849; _LL848:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL844:;}
static unsigned char _temp852[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp852, _temp852, _temp852 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp853= yy1;
struct Cyc_Absyn_Stmt* _temp859; _LL855: if(*(( void**) _temp853) == Cyc_Stmt_tok){
_LL860: _temp859=(( struct Cyc_Stmt_tok_struct*) _temp853)->f1; goto _LL856;}
else{ goto _LL857;} _LL857: goto _LL858; _LL856: return _temp859; _LL858:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL854:;} static unsigned char _temp862[
14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp862, _temp862, _temp862 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp863= yy1; void* _temp869; _LL865: if(*(( void**) _temp863)
== Cyc_BlockItem_tok){ _LL870: _temp869=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp863)->f1; goto _LL866;} else{ goto _LL867;} _LL867: goto _LL868; _LL866:
return _temp869; _LL868:( int) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL864:;} static unsigned char _temp872[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp872, _temp872, _temp872 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp873= yy1; struct Cyc_List_List* _temp879; _LL875: if(*((
void**) _temp873) == Cyc_SwitchClauseList_tok){ _LL880: _temp879=(( struct Cyc_SwitchClauseList_tok_struct*)
_temp873)->f1; goto _LL876;} else{ goto _LL877;} _LL877: goto _LL878; _LL876:
return _temp879; _LL878:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL874:;} static unsigned char _temp882[ 22u]="SwitchCClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp882, _temp882, _temp882 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ void* _temp883= yy1; struct Cyc_List_List* _temp889; _LL885: if(*((
void**) _temp883) == Cyc_SwitchCClauseList_tok){ _LL890: _temp889=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp883)->f1; goto _LL886;} else{ goto _LL887;} _LL887: goto _LL888; _LL886:
return _temp889; _LL888:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL884:;} static unsigned char _temp892[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp892, _temp892, _temp892 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp893= yy1;
struct Cyc_Absyn_Pat* _temp899; _LL895: if(*(( void**) _temp893) == Cyc_Pattern_tok){
_LL900: _temp899=(( struct Cyc_Pattern_tok_struct*) _temp893)->f1; goto _LL896;}
else{ goto _LL897;} _LL897: goto _LL898; _LL896: return _temp899; _LL898:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL894:;} static unsigned char
_temp902[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp902, _temp902, _temp902 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp903= yy1; struct Cyc_List_List*
_temp909; _LL905: if(*(( void**) _temp903) == Cyc_PatternList_tok){ _LL910:
_temp909=(( struct Cyc_PatternList_tok_struct*) _temp903)->f1; goto _LL906;}
else{ goto _LL907;} _LL907: goto _LL908; _LL906: return _temp909; _LL908:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL904:;} static unsigned char
_temp912[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp912, _temp912, _temp912 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp913= yy1; struct _tuple14* _temp919; _LL915: if(*(( void**)
_temp913) == Cyc_FieldPattern_tok){ _LL920: _temp919=(( struct Cyc_FieldPattern_tok_struct*)
_temp913)->f1; goto _LL916;} else{ goto _LL917;} _LL917: goto _LL918; _LL916:
return _temp919; _LL918:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL914:;} static unsigned char _temp922[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp922, _temp922, _temp922 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp923= yy1; struct Cyc_List_List* _temp929; _LL925: if(*((
void**) _temp923) == Cyc_FieldPatternList_tok){ _LL930: _temp929=(( struct Cyc_FieldPatternList_tok_struct*)
_temp923)->f1; goto _LL926;} else{ goto _LL927;} _LL927: goto _LL928; _LL926:
return _temp929; _LL928:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL924:;} static unsigned char _temp932[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp932, _temp932, _temp932 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp933= yy1;
struct Cyc_Absyn_Fndecl* _temp939; _LL935: if(*(( void**) _temp933) == Cyc_FnDecl_tok){
_LL940: _temp939=(( struct Cyc_FnDecl_tok_struct*) _temp933)->f1; goto _LL936;}
else{ goto _LL937;} _LL937: goto _LL938; _LL936: return _temp939; _LL938:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL934:;} static unsigned char _temp942[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp942, _temp942, _temp942 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp943= yy1; struct Cyc_List_List*
_temp949; _LL945: if(*(( void**) _temp943) == Cyc_DeclList_tok){ _LL950:
_temp949=(( struct Cyc_DeclList_tok_struct*) _temp943)->f1; goto _LL946;} else{
goto _LL947;} _LL947: goto _LL948; _LL946: return _temp949; _LL948:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL944:;} static unsigned char _temp952[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp952, _temp952, _temp952 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp953= yy1; struct Cyc_Parse_Declaration_spec* _temp959;
_LL955: if(*(( void**) _temp953) == Cyc_DeclSpec_tok){ _LL960: _temp959=((
struct Cyc_DeclSpec_tok_struct*) _temp953)->f1; goto _LL956;} else{ goto _LL957;}
_LL957: goto _LL958; _LL956: return _temp959; _LL958:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL954:;} static unsigned char _temp962[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp962, _temp962, _temp962 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp963= yy1; struct
_tuple15* _temp969; _LL965: if(*(( void**) _temp963) == Cyc_InitDecl_tok){
_LL970: _temp969=(( struct Cyc_InitDecl_tok_struct*) _temp963)->f1; goto _LL966;}
else{ goto _LL967;} _LL967: goto _LL968; _LL966: return _temp969; _LL968:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL964:;} static unsigned char
_temp972[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp972, _temp972, _temp972 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp973= yy1; struct Cyc_List_List*
_temp979; _LL975: if(*(( void**) _temp973) == Cyc_InitDeclList_tok){ _LL980:
_temp979=(( struct Cyc_InitDeclList_tok_struct*) _temp973)->f1; goto _LL976;}
else{ goto _LL977;} _LL977: goto _LL978; _LL976: return _temp979; _LL978:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL974:;} static unsigned char
_temp982[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp982, _temp982, _temp982 + 17u}}; void* Cyc_yyget_StorageClass_tok(
void* yy1){ void* _temp983= yy1; void* _temp989; _LL985: if(*(( void**) _temp983)
== Cyc_StorageClass_tok){ _LL990: _temp989=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp983)->f1; goto _LL986;} else{ goto _LL987;} _LL987: goto _LL988; _LL986:
return _temp989; _LL988:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL984:;} static unsigned char _temp992[ 18u]="TypeSpecifier_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp992, _temp992, _temp992 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp993= yy1; void* _temp999; _LL995: if(*(( void**) _temp993) ==
Cyc_TypeSpecifier_tok){ _LL1000: _temp999=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp993)->f1; goto _LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL996:
return _temp999; _LL998:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL994:;} static unsigned char _temp1002[ 18u]="StructOrUnion_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp1002, _temp1002, _temp1002 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1003= yy1; void* _temp1009; _LL1005: if(*(( void**) _temp1003)
== Cyc_StructOrUnion_tok){ _LL1010: _temp1009=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1003)->f1; goto _LL1006;} else{ goto _LL1007;} _LL1007: goto _LL1008;
_LL1006: return _temp1009; _LL1008:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1004:;} static unsigned char _temp1012[ 13u]="TypeQual_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp1012,
_temp1012, _temp1012 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1013= yy1; struct Cyc_Absyn_Tqual _temp1019; _LL1015: if(*((
void**) _temp1013) == Cyc_TypeQual_tok){ _LL1020: _temp1019=(( struct Cyc_TypeQual_tok_struct*)
_temp1013)->f1; goto _LL1016;} else{ goto _LL1017;} _LL1017: goto _LL1018;
_LL1016: return _temp1019; _LL1018:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL1014:;} static unsigned char _temp1022[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp1022, _temp1022, _temp1022 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp1023= yy1; struct Cyc_List_List* _temp1029; _LL1025: if(*((
void**) _temp1023) == Cyc_StructFieldDeclList_tok){ _LL1030: _temp1029=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp1023)->f1; goto _LL1026;} else{ goto
_LL1027;} _LL1027: goto _LL1028; _LL1026: return _temp1029; _LL1028:( int)
_throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL1024:;} static
unsigned char _temp1032[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp1032, _temp1032,
_temp1032 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp1033= yy1; struct Cyc_List_List* _temp1039; _LL1035: if(*((
void**) _temp1033) == Cyc_StructFieldDeclListList_tok){ _LL1040: _temp1039=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp1033)->f1; goto _LL1036;}
else{ goto _LL1037;} _LL1037: goto _LL1038; _LL1036: return _temp1039; _LL1038:(
int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL1034:;} static
unsigned char _temp1042[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp1042, _temp1042,
_temp1042 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp1043= yy1; struct Cyc_List_List* _temp1049; _LL1045: if(*((
void**) _temp1043) == Cyc_TypeModifierList_tok){ _LL1050: _temp1049=(( struct
Cyc_TypeModifierList_tok_struct*) _temp1043)->f1; goto _LL1046;} else{ goto
_LL1047;} _LL1047: goto _LL1048; _LL1046: return _temp1049; _LL1048:( int)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1044:;} static
unsigned char _temp1052[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp1052, _temp1052, _temp1052 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1053= yy1; void* _temp1059;
_LL1055: if(*(( void**) _temp1053) == Cyc_Rgn_tok){ _LL1060: _temp1059=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1053)->f1; goto _LL1056;} else{ goto _LL1057;}
_LL1057: goto _LL1058; _LL1056: return _temp1059; _LL1058:( int) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1054:;} static unsigned char _temp1062[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{
_temp1062, _temp1062, _temp1062 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp1063= yy1; struct Cyc_Parse_Declarator* _temp1069;
_LL1065: if(*(( void**) _temp1063) == Cyc_Declarator_tok){ _LL1070: _temp1069=((
struct Cyc_Declarator_tok_struct*) _temp1063)->f1; goto _LL1066;} else{ goto
_LL1067;} _LL1067: goto _LL1068; _LL1066: return _temp1069; _LL1068:( int)
_throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1064:;} static unsigned char
_temp1072[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1072, _temp1072,
_temp1072 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1073= yy1; struct _tuple13* _temp1079; _LL1075: if(*(( void**)
_temp1073) == Cyc_DeclaratorExpopt_tok){ _LL1080: _temp1079=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1073)->f1; goto _LL1076;} else{ goto _LL1077;} _LL1077: goto _LL1078;
_LL1076: return _temp1079; _LL1078:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1074:;} static unsigned char _temp1082[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1082, _temp1082, _temp1082 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1083= yy1; struct Cyc_List_List* _temp1089; _LL1085: if(*((
void**) _temp1083) == Cyc_DeclaratorExpoptList_tok){ _LL1090: _temp1089=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1083)->f1; goto _LL1086;}
else{ goto _LL1087;} _LL1087: goto _LL1088; _LL1086: return _temp1089; _LL1088:(
int) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1084:;} static
unsigned char _temp1092[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1092, _temp1092,
_temp1092 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1093= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1099;
_LL1095: if(*(( void**) _temp1093) == Cyc_AbstractDeclarator_tok){ _LL1100:
_temp1099=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1093)->f1; goto
_LL1096;} else{ goto _LL1097;} _LL1097: goto _LL1098; _LL1096: return _temp1099;
_LL1098:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1094:;}
static unsigned char _temp1102[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1102, _temp1102, _temp1102 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1103= yy1; int _temp1109; _LL1105:
if(*(( void**) _temp1103) == Cyc_Bool_tok){ _LL1110: _temp1109=(( struct Cyc_Bool_tok_struct*)
_temp1103)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: goto _LL1108;
_LL1106: return _temp1109; _LL1108:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1104:;} static unsigned char _temp1112[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1112, _temp1112, _temp1112 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1113= yy1; void* _temp1119;
_LL1115: if(*(( void**) _temp1113) == Cyc_Scope_tok){ _LL1120: _temp1119=( void*)((
struct Cyc_Scope_tok_struct*) _temp1113)->f1; goto _LL1116;} else{ goto _LL1117;}
_LL1117: goto _LL1118; _LL1116: return _temp1119; _LL1118:( int) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1114:;} static unsigned char _temp1122[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1122, _temp1122, _temp1122 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1123= yy1; struct Cyc_Absyn_Tunionfield* _temp1129;
_LL1125: if(*(( void**) _temp1123) == Cyc_TunionField_tok){ _LL1130: _temp1129=((
struct Cyc_TunionField_tok_struct*) _temp1123)->f1; goto _LL1126;} else{ goto
_LL1127;} _LL1127: goto _LL1128; _LL1126: return _temp1129; _LL1128:( int)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1124:;} static unsigned char
_temp1132[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1132, _temp1132, _temp1132 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1133= yy1; struct Cyc_List_List*
_temp1139; _LL1135: if(*(( void**) _temp1133) == Cyc_TunionFieldList_tok){
_LL1140: _temp1139=(( struct Cyc_TunionFieldList_tok_struct*) _temp1133)->f1;
goto _LL1136;} else{ goto _LL1137;} _LL1137: goto _LL1138; _LL1136: return
_temp1139; _LL1138:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1134:;} static unsigned char _temp1142[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1142, _temp1142, _temp1142 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1143= yy1; struct _tuple18* _temp1149; _LL1145: if(*((
void**) _temp1143) == Cyc_QualSpecList_tok){ _LL1150: _temp1149=(( struct Cyc_QualSpecList_tok_struct*)
_temp1143)->f1; goto _LL1146;} else{ goto _LL1147;} _LL1147: goto _LL1148;
_LL1146: return _temp1149; _LL1148:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1144:;} static unsigned char _temp1152[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1152, _temp1152, _temp1152 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1153= yy1;
struct Cyc_List_List* _temp1159; _LL1155: if(*(( void**) _temp1153) == Cyc_IdList_tok){
_LL1160: _temp1159=(( struct Cyc_IdList_tok_struct*) _temp1153)->f1; goto
_LL1156;} else{ goto _LL1157;} _LL1157: goto _LL1158; _LL1156: return _temp1159;
_LL1158:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1154:;} static
unsigned char _temp1162[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1162, _temp1162, _temp1162 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1163= yy1;
struct _tuple2* _temp1169; _LL1165: if(*(( void**) _temp1163) == Cyc_ParamDecl_tok){
_LL1170: _temp1169=(( struct Cyc_ParamDecl_tok_struct*) _temp1163)->f1; goto
_LL1166;} else{ goto _LL1167;} _LL1167: goto _LL1168; _LL1166: return _temp1169;
_LL1168:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1164:;} static
unsigned char _temp1172[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1172, _temp1172,
_temp1172 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1173= yy1; struct Cyc_List_List* _temp1179; _LL1175: if(*(( void**)
_temp1173) == Cyc_ParamDeclList_tok){ _LL1180: _temp1179=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1173)->f1; goto _LL1176;} else{ goto _LL1177;} _LL1177: goto _LL1178;
_LL1176: return _temp1179; _LL1178:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1174:;} static unsigned char _temp1182[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1182, _temp1182, _temp1182 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1183= yy1; struct _tuple17* _temp1189; _LL1185: if(*((
void**) _temp1183) == Cyc_ParamDeclListBool_tok){ _LL1190: _temp1189=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1183)->f1; goto _LL1186;} else{ goto
_LL1187;} _LL1187: goto _LL1188; _LL1186: return _temp1189; _LL1188:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1184:;} static
unsigned char _temp1192[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1192, _temp1192, _temp1192 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1193=
yy1; struct Cyc_List_List* _temp1199; _LL1195: if(*(( void**) _temp1193) == Cyc_TypeList_tok){
_LL1200: _temp1199=(( struct Cyc_TypeList_tok_struct*) _temp1193)->f1; goto
_LL1196;} else{ goto _LL1197;} _LL1197: goto _LL1198; _LL1196: return _temp1199;
_LL1198:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1194:;} static
unsigned char _temp1202[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1202, _temp1202,
_temp1202 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1203= yy1; struct Cyc_List_List* _temp1209; _LL1205: if(*(( void**)
_temp1203) == Cyc_DesignatorList_tok){ _LL1210: _temp1209=(( struct Cyc_DesignatorList_tok_struct*)
_temp1203)->f1; goto _LL1206;} else{ goto _LL1207;} _LL1207: goto _LL1208;
_LL1206: return _temp1209; _LL1208:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1204:;} static unsigned char _temp1212[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1212,
_temp1212, _temp1212 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1213= yy1; void* _temp1219; _LL1215: if(*(( void**) _temp1213) == Cyc_Designator_tok){
_LL1220: _temp1219=( void*)(( struct Cyc_Designator_tok_struct*) _temp1213)->f1;
goto _LL1216;} else{ goto _LL1217;} _LL1217: goto _LL1218; _LL1216: return
_temp1219; _LL1218:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1214:;}
static unsigned char _temp1222[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1222, _temp1222, _temp1222 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1223= yy1; void* _temp1229;
_LL1225: if(*(( void**) _temp1223) == Cyc_Kind_tok){ _LL1230: _temp1229=( void*)((
struct Cyc_Kind_tok_struct*) _temp1223)->f1; goto _LL1226;} else{ goto _LL1227;}
_LL1227: goto _LL1228; _LL1226: return _temp1229; _LL1228:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1224:;} static unsigned char _temp1232[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1232, _temp1232, _temp1232 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1233= yy1; void* _temp1239; _LL1235: if(*(( void**) _temp1233) == Cyc_Type_tok){
_LL1240: _temp1239=( void*)(( struct Cyc_Type_tok_struct*) _temp1233)->f1; goto
_LL1236;} else{ goto _LL1237;} _LL1237: goto _LL1238; _LL1236: return _temp1239;
_LL1238:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1234:;} static
unsigned char _temp1242[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1242, _temp1242,
_temp1242 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1243= yy1; struct Cyc_List_List* _temp1249; _LL1245: if(*(( void**)
_temp1243) == Cyc_AttributeList_tok){ _LL1250: _temp1249=(( struct Cyc_AttributeList_tok_struct*)
_temp1243)->f1; goto _LL1246;} else{ goto _LL1247;} _LL1247: goto _LL1248;
_LL1246: return _temp1249; _LL1248:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1244:;} static unsigned char _temp1252[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1252,
_temp1252, _temp1252 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1253= yy1; void* _temp1259; _LL1255: if(*(( void**) _temp1253) == Cyc_Attribute_tok){
_LL1260: _temp1259=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1253)->f1;
goto _LL1256;} else{ goto _LL1257;} _LL1257: goto _LL1258; _LL1256: return
_temp1259; _LL1258:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1254:;}
static unsigned char _temp1262[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1262, _temp1262, _temp1262 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1263= yy1; struct Cyc_Absyn_Enumfield* _temp1269; _LL1265: if(*(( void**)
_temp1263) == Cyc_Enumfield_tok){ _LL1270: _temp1269=(( struct Cyc_Enumfield_tok_struct*)
_temp1263)->f1; goto _LL1266;} else{ goto _LL1267;} _LL1267: goto _LL1268;
_LL1266: return _temp1269; _LL1268:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1264:;} static unsigned char _temp1272[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1272, _temp1272, _temp1272 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1273= yy1; struct Cyc_List_List* _temp1279; _LL1275: if(*((
void**) _temp1273) == Cyc_EnumfieldList_tok){ _LL1280: _temp1279=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1273)->f1; goto _LL1276;} else{ goto _LL1277;} _LL1277: goto _LL1278;
_LL1276: return _temp1279; _LL1278:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1274:;} static unsigned char _temp1282[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1282, _temp1282, _temp1282 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1283= yy1;
struct Cyc_Core_Opt* _temp1289; _LL1285: if(*(( void**) _temp1283) == Cyc_TypeOpt_tok){
_LL1290: _temp1289=(( struct Cyc_TypeOpt_tok_struct*) _temp1283)->f1; goto
_LL1286;} else{ goto _LL1287;} _LL1287: goto _LL1288; _LL1286: return _temp1289;
_LL1288:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1284:;} static
unsigned char _temp1292[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1292, _temp1292, _temp1292 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1293=
yy1; struct Cyc_List_List* _temp1299; _LL1295: if(*(( void**) _temp1293) == Cyc_Rgnorder_tok){
_LL1300: _temp1299=(( struct Cyc_Rgnorder_tok_struct*) _temp1293)->f1; goto
_LL1296;} else{ goto _LL1297;} _LL1297: goto _LL1298; _LL1296: return _temp1299;
_LL1298:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1294:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_arr text; } ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct
Cyc_Yyltype _temp1301; _temp1301.timestamp= 0; _temp1301.first_line= 0;
_temp1301.first_column= 0; _temp1301.last_line= 0; _temp1301.last_column= 0;
_temp1301.text= _tag_arr("", sizeof( unsigned char), 1u); _temp1301;});} static
unsigned char _temp1302[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0,{
_temp1302, _temp1302, _temp1302 + 1u}}; static short Cyc_yytranslate[ 353u]={ 0,
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
unsigned char _temp1303[ 2u]="$"; static unsigned char _temp1304[ 6u]="error";
static unsigned char _temp1305[ 12u]="$undefined."; static unsigned char
_temp1306[ 5u]="AUTO"; static unsigned char _temp1307[ 9u]="REGISTER"; static
unsigned char _temp1308[ 7u]="STATIC"; static unsigned char _temp1309[ 7u]="EXTERN";
static unsigned char _temp1310[ 8u]="TYPEDEF"; static unsigned char _temp1311[ 5u]="VOID";
static unsigned char _temp1312[ 5u]="CHAR"; static unsigned char _temp1313[ 6u]="SHORT";
static unsigned char _temp1314[ 4u]="INT"; static unsigned char _temp1315[ 5u]="LONG";
static unsigned char _temp1316[ 6u]="FLOAT"; static unsigned char _temp1317[ 7u]="DOUBLE";
static unsigned char _temp1318[ 7u]="SIGNED"; static unsigned char _temp1319[ 9u]="UNSIGNED";
static unsigned char _temp1320[ 6u]="CONST"; static unsigned char _temp1321[ 9u]="VOLATILE";
static unsigned char _temp1322[ 9u]="RESTRICT"; static unsigned char _temp1323[
7u]="STRUCT"; static unsigned char _temp1324[ 6u]="UNION"; static unsigned char
_temp1325[ 5u]="CASE"; static unsigned char _temp1326[ 8u]="DEFAULT"; static
unsigned char _temp1327[ 7u]="INLINE"; static unsigned char _temp1328[ 7u]="SIZEOF";
static unsigned char _temp1329[ 9u]="OFFSETOF"; static unsigned char _temp1330[
3u]="IF"; static unsigned char _temp1331[ 5u]="ELSE"; static unsigned char
_temp1332[ 7u]="SWITCH"; static unsigned char _temp1333[ 6u]="WHILE"; static
unsigned char _temp1334[ 3u]="DO"; static unsigned char _temp1335[ 4u]="FOR";
static unsigned char _temp1336[ 5u]="GOTO"; static unsigned char _temp1337[ 9u]="CONTINUE";
static unsigned char _temp1338[ 6u]="BREAK"; static unsigned char _temp1339[ 7u]="RETURN";
static unsigned char _temp1340[ 5u]="ENUM"; static unsigned char _temp1341[ 8u]="NULL_kw";
static unsigned char _temp1342[ 4u]="LET"; static unsigned char _temp1343[ 6u]="THROW";
static unsigned char _temp1344[ 4u]="TRY"; static unsigned char _temp1345[ 6u]="CATCH";
static unsigned char _temp1346[ 4u]="NEW"; static unsigned char _temp1347[ 9u]="ABSTRACT";
static unsigned char _temp1348[ 9u]="FALLTHRU"; static unsigned char _temp1349[
6u]="USING"; static unsigned char _temp1350[ 10u]="NAMESPACE"; static
unsigned char _temp1351[ 7u]="TUNION"; static unsigned char _temp1352[ 8u]="XTUNION";
static unsigned char _temp1353[ 5u]="FILL"; static unsigned char _temp1354[ 8u]="CODEGEN";
static unsigned char _temp1355[ 4u]="CUT"; static unsigned char _temp1356[ 7u]="SPLICE";
static unsigned char _temp1357[ 7u]="PRINTF"; static unsigned char _temp1358[ 8u]="FPRINTF";
static unsigned char _temp1359[ 8u]="XPRINTF"; static unsigned char _temp1360[ 6u]="SCANF";
static unsigned char _temp1361[ 7u]="FSCANF"; static unsigned char _temp1362[ 7u]="SSCANF";
static unsigned char _temp1363[ 7u]="MALLOC"; static unsigned char _temp1364[ 9u]="REGION_T";
static unsigned char _temp1365[ 7u]="REGION"; static unsigned char _temp1366[ 5u]="RNEW";
static unsigned char _temp1367[ 8u]="RMALLOC"; static unsigned char _temp1368[ 7u]="PTR_OP";
static unsigned char _temp1369[ 7u]="INC_OP"; static unsigned char _temp1370[ 7u]="DEC_OP";
static unsigned char _temp1371[ 8u]="LEFT_OP"; static unsigned char _temp1372[ 9u]="RIGHT_OP";
static unsigned char _temp1373[ 6u]="LE_OP"; static unsigned char _temp1374[ 6u]="GE_OP";
static unsigned char _temp1375[ 6u]="EQ_OP"; static unsigned char _temp1376[ 6u]="NE_OP";
static unsigned char _temp1377[ 7u]="AND_OP"; static unsigned char _temp1378[ 6u]="OR_OP";
static unsigned char _temp1379[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1380[ 11u]="DIV_ASSIGN"; static unsigned char _temp1381[ 11u]="MOD_ASSIGN";
static unsigned char _temp1382[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1383[ 11u]="SUB_ASSIGN"; static unsigned char _temp1384[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1385[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1386[ 11u]="AND_ASSIGN"; static unsigned char _temp1387[ 11u]="XOR_ASSIGN";
static unsigned char _temp1388[ 10u]="OR_ASSIGN"; static unsigned char _temp1389[
9u]="ELLIPSIS"; static unsigned char _temp1390[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1391[ 12u]="COLON_COLON"; static unsigned char _temp1392[ 11u]="IDENTIFIER";
static unsigned char _temp1393[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1394[ 7u]="STRING"; static unsigned char _temp1395[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1396[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1397[ 9u]="TYPE_VAR"; static unsigned char _temp1398[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1399[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1400[ 10u]="ATTRIBUTE"; static unsigned char _temp1401[ 4u]="';'"; static
unsigned char _temp1402[ 4u]="'{'"; static unsigned char _temp1403[ 4u]="'}'";
static unsigned char _temp1404[ 4u]="'='"; static unsigned char _temp1405[ 4u]="'('";
static unsigned char _temp1406[ 4u]="')'"; static unsigned char _temp1407[ 4u]="','";
static unsigned char _temp1408[ 4u]="'_'"; static unsigned char _temp1409[ 4u]="'$'";
static unsigned char _temp1410[ 4u]="'<'"; static unsigned char _temp1411[ 4u]="'>'";
static unsigned char _temp1412[ 4u]="':'"; static unsigned char _temp1413[ 4u]="'.'";
static unsigned char _temp1414[ 4u]="'['"; static unsigned char _temp1415[ 4u]="']'";
static unsigned char _temp1416[ 4u]="'*'"; static unsigned char _temp1417[ 4u]="'@'";
static unsigned char _temp1418[ 4u]="'?'"; static unsigned char _temp1419[ 4u]="'+'";
static unsigned char _temp1420[ 4u]="'-'"; static unsigned char _temp1421[ 4u]="'&'";
static unsigned char _temp1422[ 4u]="'|'"; static unsigned char _temp1423[ 4u]="'^'";
static unsigned char _temp1424[ 4u]="'/'"; static unsigned char _temp1425[ 4u]="'%'";
static unsigned char _temp1426[ 4u]="'~'"; static unsigned char _temp1427[ 4u]="'!'";
static unsigned char _temp1428[ 5u]="prog"; static unsigned char _temp1429[ 17u]="translation_unit";
static unsigned char _temp1430[ 21u]="external_declaration"; static
unsigned char _temp1431[ 20u]="function_definition"; static unsigned char
_temp1432[ 21u]="function_definition2"; static unsigned char _temp1433[ 13u]="using_action";
static unsigned char _temp1434[ 15u]="unusing_action"; static unsigned char
_temp1435[ 17u]="namespace_action"; static unsigned char _temp1436[ 19u]="unnamespace_action";
static unsigned char _temp1437[ 12u]="declaration"; static unsigned char
_temp1438[ 17u]="declaration_list"; static unsigned char _temp1439[ 23u]="declaration_specifiers";
static unsigned char _temp1440[ 24u]="storage_class_specifier"; static
unsigned char _temp1441[ 15u]="attributes_opt"; static unsigned char _temp1442[
11u]="attributes"; static unsigned char _temp1443[ 15u]="attribute_list"; static
unsigned char _temp1444[ 10u]="attribute"; static unsigned char _temp1445[ 15u]="type_specifier";
static unsigned char _temp1446[ 5u]="kind"; static unsigned char _temp1447[ 15u]="type_qualifier";
static unsigned char _temp1448[ 15u]="enum_specifier"; static unsigned char
_temp1449[ 11u]="enum_field"; static unsigned char _temp1450[ 22u]="enum_declaration_list";
static unsigned char _temp1451[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1452[ 16u]="type_params_opt"; static unsigned char _temp1453[
16u]="struct_or_union"; static unsigned char _temp1454[ 24u]="struct_declaration_list";
static unsigned char _temp1455[ 25u]="struct_declaration_list0"; static
unsigned char _temp1456[ 21u]="init_declarator_list"; static unsigned char
_temp1457[ 22u]="init_declarator_list0"; static unsigned char _temp1458[ 16u]="init_declarator";
static unsigned char _temp1459[ 19u]="struct_declaration"; static unsigned char
_temp1460[ 25u]="specifier_qualifier_list"; static unsigned char _temp1461[ 23u]="struct_declarator_list";
static unsigned char _temp1462[ 24u]="struct_declarator_list0"; static
unsigned char _temp1463[ 18u]="struct_declarator"; static unsigned char
_temp1464[ 17u]="tunion_specifier"; static unsigned char _temp1465[ 18u]="tunion_or_xtunion";
static unsigned char _temp1466[ 17u]="tunionfield_list"; static unsigned char
_temp1467[ 18u]="tunionfield_scope"; static unsigned char _temp1468[ 12u]="tunionfield";
static unsigned char _temp1469[ 11u]="declarator"; static unsigned char
_temp1470[ 18u]="direct_declarator"; static unsigned char _temp1471[ 8u]="pointer";
static unsigned char _temp1472[ 13u]="pointer_char"; static unsigned char
_temp1473[ 8u]="rgn_opt"; static unsigned char _temp1474[ 4u]="rgn"; static
unsigned char _temp1475[ 20u]="type_qualifier_list"; static unsigned char
_temp1476[ 20u]="parameter_type_list"; static unsigned char _temp1477[ 16u]="optional_effect";
static unsigned char _temp1478[ 19u]="optional_rgn_order"; static unsigned char
_temp1479[ 10u]="rgn_order"; static unsigned char _temp1480[ 16u]="optional_inject";
static unsigned char _temp1481[ 11u]="effect_set"; static unsigned char
_temp1482[ 14u]="atomic_effect"; static unsigned char _temp1483[ 11u]="region_set";
static unsigned char _temp1484[ 15u]="parameter_list"; static unsigned char
_temp1485[ 22u]="parameter_declaration"; static unsigned char _temp1486[ 16u]="identifier_list";
static unsigned char _temp1487[ 17u]="identifier_list0"; static unsigned char
_temp1488[ 12u]="initializer"; static unsigned char _temp1489[ 18u]="array_initializer";
static unsigned char _temp1490[ 17u]="initializer_list"; static unsigned char
_temp1491[ 12u]="designation"; static unsigned char _temp1492[ 16u]="designator_list";
static unsigned char _temp1493[ 11u]="designator"; static unsigned char
_temp1494[ 10u]="type_name"; static unsigned char _temp1495[ 14u]="any_type_name";
static unsigned char _temp1496[ 15u]="type_name_list"; static unsigned char
_temp1497[ 20u]="abstract_declarator"; static unsigned char _temp1498[ 27u]="direct_abstract_declarator";
static unsigned char _temp1499[ 10u]="statement"; static unsigned char _temp1500[
18u]="labeled_statement"; static unsigned char _temp1501[ 21u]="expression_statement";
static unsigned char _temp1502[ 19u]="compound_statement"; static unsigned char
_temp1503[ 16u]="block_item_list"; static unsigned char _temp1504[ 11u]="block_item";
static unsigned char _temp1505[ 20u]="selection_statement"; static unsigned char
_temp1506[ 15u]="switch_clauses"; static unsigned char _temp1507[ 16u]="switchC_clauses";
static unsigned char _temp1508[ 20u]="iteration_statement"; static unsigned char
_temp1509[ 15u]="jump_statement"; static unsigned char _temp1510[ 8u]="pattern";
static unsigned char _temp1511[ 19u]="tuple_pattern_list"; static unsigned char
_temp1512[ 20u]="tuple_pattern_list0"; static unsigned char _temp1513[ 14u]="field_pattern";
static unsigned char _temp1514[ 19u]="field_pattern_list"; static unsigned char
_temp1515[ 20u]="field_pattern_list0"; static unsigned char _temp1516[ 11u]="expression";
static unsigned char _temp1517[ 22u]="assignment_expression"; static
unsigned char _temp1518[ 20u]="assignment_operator"; static unsigned char
_temp1519[ 23u]="conditional_expression"; static unsigned char _temp1520[ 20u]="constant_expression";
static unsigned char _temp1521[ 22u]="logical_or_expression"; static
unsigned char _temp1522[ 23u]="logical_and_expression"; static unsigned char
_temp1523[ 24u]="inclusive_or_expression"; static unsigned char _temp1524[ 24u]="exclusive_or_expression";
static unsigned char _temp1525[ 15u]="and_expression"; static unsigned char
_temp1526[ 20u]="equality_expression"; static unsigned char _temp1527[ 22u]="relational_expression";
static unsigned char _temp1528[ 17u]="shift_expression"; static unsigned char
_temp1529[ 20u]="additive_expression"; static unsigned char _temp1530[ 26u]="multiplicative_expression";
static unsigned char _temp1531[ 16u]="cast_expression"; static unsigned char
_temp1532[ 17u]="unary_expression"; static unsigned char _temp1533[ 14u]="format_primop";
static unsigned char _temp1534[ 15u]="unary_operator"; static unsigned char
_temp1535[ 19u]="postfix_expression"; static unsigned char _temp1536[ 19u]="primary_expression";
static unsigned char _temp1537[ 25u]="argument_expression_list"; static
unsigned char _temp1538[ 26u]="argument_expression_list0"; static unsigned char
_temp1539[ 9u]="constant"; static unsigned char _temp1540[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 238u]={{ _temp1303, _temp1303, _temp1303
+ 2u},{ _temp1304, _temp1304, _temp1304 + 6u},{ _temp1305, _temp1305, _temp1305
+ 12u},{ _temp1306, _temp1306, _temp1306 + 5u},{ _temp1307, _temp1307, _temp1307
+ 9u},{ _temp1308, _temp1308, _temp1308 + 7u},{ _temp1309, _temp1309, _temp1309
+ 7u},{ _temp1310, _temp1310, _temp1310 + 8u},{ _temp1311, _temp1311, _temp1311
+ 5u},{ _temp1312, _temp1312, _temp1312 + 5u},{ _temp1313, _temp1313, _temp1313
+ 6u},{ _temp1314, _temp1314, _temp1314 + 4u},{ _temp1315, _temp1315, _temp1315
+ 5u},{ _temp1316, _temp1316, _temp1316 + 6u},{ _temp1317, _temp1317, _temp1317
+ 7u},{ _temp1318, _temp1318, _temp1318 + 7u},{ _temp1319, _temp1319, _temp1319
+ 9u},{ _temp1320, _temp1320, _temp1320 + 6u},{ _temp1321, _temp1321, _temp1321
+ 9u},{ _temp1322, _temp1322, _temp1322 + 9u},{ _temp1323, _temp1323, _temp1323
+ 7u},{ _temp1324, _temp1324, _temp1324 + 6u},{ _temp1325, _temp1325, _temp1325
+ 5u},{ _temp1326, _temp1326, _temp1326 + 8u},{ _temp1327, _temp1327, _temp1327
+ 7u},{ _temp1328, _temp1328, _temp1328 + 7u},{ _temp1329, _temp1329, _temp1329
+ 9u},{ _temp1330, _temp1330, _temp1330 + 3u},{ _temp1331, _temp1331, _temp1331
+ 5u},{ _temp1332, _temp1332, _temp1332 + 7u},{ _temp1333, _temp1333, _temp1333
+ 6u},{ _temp1334, _temp1334, _temp1334 + 3u},{ _temp1335, _temp1335, _temp1335
+ 4u},{ _temp1336, _temp1336, _temp1336 + 5u},{ _temp1337, _temp1337, _temp1337
+ 9u},{ _temp1338, _temp1338, _temp1338 + 6u},{ _temp1339, _temp1339, _temp1339
+ 7u},{ _temp1340, _temp1340, _temp1340 + 5u},{ _temp1341, _temp1341, _temp1341
+ 8u},{ _temp1342, _temp1342, _temp1342 + 4u},{ _temp1343, _temp1343, _temp1343
+ 6u},{ _temp1344, _temp1344, _temp1344 + 4u},{ _temp1345, _temp1345, _temp1345
+ 6u},{ _temp1346, _temp1346, _temp1346 + 4u},{ _temp1347, _temp1347, _temp1347
+ 9u},{ _temp1348, _temp1348, _temp1348 + 9u},{ _temp1349, _temp1349, _temp1349
+ 6u},{ _temp1350, _temp1350, _temp1350 + 10u},{ _temp1351, _temp1351, _temp1351
+ 7u},{ _temp1352, _temp1352, _temp1352 + 8u},{ _temp1353, _temp1353, _temp1353
+ 5u},{ _temp1354, _temp1354, _temp1354 + 8u},{ _temp1355, _temp1355, _temp1355
+ 4u},{ _temp1356, _temp1356, _temp1356 + 7u},{ _temp1357, _temp1357, _temp1357
+ 7u},{ _temp1358, _temp1358, _temp1358 + 8u},{ _temp1359, _temp1359, _temp1359
+ 8u},{ _temp1360, _temp1360, _temp1360 + 6u},{ _temp1361, _temp1361, _temp1361
+ 7u},{ _temp1362, _temp1362, _temp1362 + 7u},{ _temp1363, _temp1363, _temp1363
+ 7u},{ _temp1364, _temp1364, _temp1364 + 9u},{ _temp1365, _temp1365, _temp1365
+ 7u},{ _temp1366, _temp1366, _temp1366 + 5u},{ _temp1367, _temp1367, _temp1367
+ 8u},{ _temp1368, _temp1368, _temp1368 + 7u},{ _temp1369, _temp1369, _temp1369
+ 7u},{ _temp1370, _temp1370, _temp1370 + 7u},{ _temp1371, _temp1371, _temp1371
+ 8u},{ _temp1372, _temp1372, _temp1372 + 9u},{ _temp1373, _temp1373, _temp1373
+ 6u},{ _temp1374, _temp1374, _temp1374 + 6u},{ _temp1375, _temp1375, _temp1375
+ 6u},{ _temp1376, _temp1376, _temp1376 + 6u},{ _temp1377, _temp1377, _temp1377
+ 7u},{ _temp1378, _temp1378, _temp1378 + 6u},{ _temp1379, _temp1379, _temp1379
+ 11u},{ _temp1380, _temp1380, _temp1380 + 11u},{ _temp1381, _temp1381,
_temp1381 + 11u},{ _temp1382, _temp1382, _temp1382 + 11u},{ _temp1383, _temp1383,
_temp1383 + 11u},{ _temp1384, _temp1384, _temp1384 + 12u},{ _temp1385, _temp1385,
_temp1385 + 13u},{ _temp1386, _temp1386, _temp1386 + 11u},{ _temp1387, _temp1387,
_temp1387 + 11u},{ _temp1388, _temp1388, _temp1388 + 10u},{ _temp1389, _temp1389,
_temp1389 + 9u},{ _temp1390, _temp1390, _temp1390 + 11u},{ _temp1391, _temp1391,
_temp1391 + 12u},{ _temp1392, _temp1392, _temp1392 + 11u},{ _temp1393, _temp1393,
_temp1393 + 17u},{ _temp1394, _temp1394, _temp1394 + 7u},{ _temp1395, _temp1395,
_temp1395 + 19u},{ _temp1396, _temp1396, _temp1396 + 18u},{ _temp1397, _temp1397,
_temp1397 + 9u},{ _temp1398, _temp1398, _temp1398 + 16u},{ _temp1399, _temp1399,
_temp1399 + 18u},{ _temp1400, _temp1400, _temp1400 + 10u},{ _temp1401, _temp1401,
_temp1401 + 4u},{ _temp1402, _temp1402, _temp1402 + 4u},{ _temp1403, _temp1403,
_temp1403 + 4u},{ _temp1404, _temp1404, _temp1404 + 4u},{ _temp1405, _temp1405,
_temp1405 + 4u},{ _temp1406, _temp1406, _temp1406 + 4u},{ _temp1407, _temp1407,
_temp1407 + 4u},{ _temp1408, _temp1408, _temp1408 + 4u},{ _temp1409, _temp1409,
_temp1409 + 4u},{ _temp1410, _temp1410, _temp1410 + 4u},{ _temp1411, _temp1411,
_temp1411 + 4u},{ _temp1412, _temp1412, _temp1412 + 4u},{ _temp1413, _temp1413,
_temp1413 + 4u},{ _temp1414, _temp1414, _temp1414 + 4u},{ _temp1415, _temp1415,
_temp1415 + 4u},{ _temp1416, _temp1416, _temp1416 + 4u},{ _temp1417, _temp1417,
_temp1417 + 4u},{ _temp1418, _temp1418, _temp1418 + 4u},{ _temp1419, _temp1419,
_temp1419 + 4u},{ _temp1420, _temp1420, _temp1420 + 4u},{ _temp1421, _temp1421,
_temp1421 + 4u},{ _temp1422, _temp1422, _temp1422 + 4u},{ _temp1423, _temp1423,
_temp1423 + 4u},{ _temp1424, _temp1424, _temp1424 + 4u},{ _temp1425, _temp1425,
_temp1425 + 4u},{ _temp1426, _temp1426, _temp1426 + 4u},{ _temp1427, _temp1427,
_temp1427 + 4u},{ _temp1428, _temp1428, _temp1428 + 5u},{ _temp1429, _temp1429,
_temp1429 + 17u},{ _temp1430, _temp1430, _temp1430 + 21u},{ _temp1431, _temp1431,
_temp1431 + 20u},{ _temp1432, _temp1432, _temp1432 + 21u},{ _temp1433, _temp1433,
_temp1433 + 13u},{ _temp1434, _temp1434, _temp1434 + 15u},{ _temp1435, _temp1435,
_temp1435 + 17u},{ _temp1436, _temp1436, _temp1436 + 19u},{ _temp1437, _temp1437,
_temp1437 + 12u},{ _temp1438, _temp1438, _temp1438 + 17u},{ _temp1439, _temp1439,
_temp1439 + 23u},{ _temp1440, _temp1440, _temp1440 + 24u},{ _temp1441, _temp1441,
_temp1441 + 15u},{ _temp1442, _temp1442, _temp1442 + 11u},{ _temp1443, _temp1443,
_temp1443 + 15u},{ _temp1444, _temp1444, _temp1444 + 10u},{ _temp1445, _temp1445,
_temp1445 + 15u},{ _temp1446, _temp1446, _temp1446 + 5u},{ _temp1447, _temp1447,
_temp1447 + 15u},{ _temp1448, _temp1448, _temp1448 + 15u},{ _temp1449, _temp1449,
_temp1449 + 11u},{ _temp1450, _temp1450, _temp1450 + 22u},{ _temp1451, _temp1451,
_temp1451 + 26u},{ _temp1452, _temp1452, _temp1452 + 16u},{ _temp1453, _temp1453,
_temp1453 + 16u},{ _temp1454, _temp1454, _temp1454 + 24u},{ _temp1455, _temp1455,
_temp1455 + 25u},{ _temp1456, _temp1456, _temp1456 + 21u},{ _temp1457, _temp1457,
_temp1457 + 22u},{ _temp1458, _temp1458, _temp1458 + 16u},{ _temp1459, _temp1459,
_temp1459 + 19u},{ _temp1460, _temp1460, _temp1460 + 25u},{ _temp1461, _temp1461,
_temp1461 + 23u},{ _temp1462, _temp1462, _temp1462 + 24u},{ _temp1463, _temp1463,
_temp1463 + 18u},{ _temp1464, _temp1464, _temp1464 + 17u},{ _temp1465, _temp1465,
_temp1465 + 18u},{ _temp1466, _temp1466, _temp1466 + 17u},{ _temp1467, _temp1467,
_temp1467 + 18u},{ _temp1468, _temp1468, _temp1468 + 12u},{ _temp1469, _temp1469,
_temp1469 + 11u},{ _temp1470, _temp1470, _temp1470 + 18u},{ _temp1471, _temp1471,
_temp1471 + 8u},{ _temp1472, _temp1472, _temp1472 + 13u},{ _temp1473, _temp1473,
_temp1473 + 8u},{ _temp1474, _temp1474, _temp1474 + 4u},{ _temp1475, _temp1475,
_temp1475 + 20u},{ _temp1476, _temp1476, _temp1476 + 20u},{ _temp1477, _temp1477,
_temp1477 + 16u},{ _temp1478, _temp1478, _temp1478 + 19u},{ _temp1479, _temp1479,
_temp1479 + 10u},{ _temp1480, _temp1480, _temp1480 + 16u},{ _temp1481, _temp1481,
_temp1481 + 11u},{ _temp1482, _temp1482, _temp1482 + 14u},{ _temp1483, _temp1483,
_temp1483 + 11u},{ _temp1484, _temp1484, _temp1484 + 15u},{ _temp1485, _temp1485,
_temp1485 + 22u},{ _temp1486, _temp1486, _temp1486 + 16u},{ _temp1487, _temp1487,
_temp1487 + 17u},{ _temp1488, _temp1488, _temp1488 + 12u},{ _temp1489, _temp1489,
_temp1489 + 18u},{ _temp1490, _temp1490, _temp1490 + 17u},{ _temp1491, _temp1491,
_temp1491 + 12u},{ _temp1492, _temp1492, _temp1492 + 16u},{ _temp1493, _temp1493,
_temp1493 + 11u},{ _temp1494, _temp1494, _temp1494 + 10u},{ _temp1495, _temp1495,
_temp1495 + 14u},{ _temp1496, _temp1496, _temp1496 + 15u},{ _temp1497, _temp1497,
_temp1497 + 20u},{ _temp1498, _temp1498, _temp1498 + 27u},{ _temp1499, _temp1499,
_temp1499 + 10u},{ _temp1500, _temp1500, _temp1500 + 18u},{ _temp1501, _temp1501,
_temp1501 + 21u},{ _temp1502, _temp1502, _temp1502 + 19u},{ _temp1503, _temp1503,
_temp1503 + 16u},{ _temp1504, _temp1504, _temp1504 + 11u},{ _temp1505, _temp1505,
_temp1505 + 20u},{ _temp1506, _temp1506, _temp1506 + 15u},{ _temp1507, _temp1507,
_temp1507 + 16u},{ _temp1508, _temp1508, _temp1508 + 20u},{ _temp1509, _temp1509,
_temp1509 + 15u},{ _temp1510, _temp1510, _temp1510 + 8u},{ _temp1511, _temp1511,
_temp1511 + 19u},{ _temp1512, _temp1512, _temp1512 + 20u},{ _temp1513, _temp1513,
_temp1513 + 14u},{ _temp1514, _temp1514, _temp1514 + 19u},{ _temp1515, _temp1515,
_temp1515 + 20u},{ _temp1516, _temp1516, _temp1516 + 11u},{ _temp1517, _temp1517,
_temp1517 + 22u},{ _temp1518, _temp1518, _temp1518 + 20u},{ _temp1519, _temp1519,
_temp1519 + 23u},{ _temp1520, _temp1520, _temp1520 + 20u},{ _temp1521, _temp1521,
_temp1521 + 22u},{ _temp1522, _temp1522, _temp1522 + 23u},{ _temp1523, _temp1523,
_temp1523 + 24u},{ _temp1524, _temp1524, _temp1524 + 24u},{ _temp1525, _temp1525,
_temp1525 + 15u},{ _temp1526, _temp1526, _temp1526 + 20u},{ _temp1527, _temp1527,
_temp1527 + 22u},{ _temp1528, _temp1528, _temp1528 + 17u},{ _temp1529, _temp1529,
_temp1529 + 20u},{ _temp1530, _temp1530, _temp1530 + 26u},{ _temp1531, _temp1531,
_temp1531 + 16u},{ _temp1532, _temp1532, _temp1532 + 17u},{ _temp1533, _temp1533,
_temp1533 + 14u},{ _temp1534, _temp1534, _temp1534 + 15u},{ _temp1535, _temp1535,
_temp1535 + 19u},{ _temp1536, _temp1536, _temp1536 + 19u},{ _temp1537, _temp1537,
_temp1537 + 25u},{ _temp1538, _temp1538, _temp1538 + 26u},{ _temp1539, _temp1539,
_temp1539 + 9u},{ _temp1540, _temp1540, _temp1540 + 20u}}; static short Cyc_yyr1[
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
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; struct
_tagged_arr yyss= _tag_arr(({ unsigned int _temp3467=( unsigned int) 200; short*
_temp3468=( short*) GC_malloc_atomic( sizeof( short) * _temp3467);{ unsigned int
_temp3469= _temp3467; unsigned int i; for( i= 0; i < _temp3469; i ++){ _temp3468[
i]= 0;}}; _temp3468;}), sizeof( short),( unsigned int) 200); int yyvsp_offset;
struct _tagged_arr yyvs= _tag_arr(({ unsigned int _temp3464=( unsigned int) 200;
void** _temp3465=( void**) GC_malloc( sizeof( void*) * _temp3464);{ unsigned int
_temp3466= _temp3464; unsigned int i; for( i= 0; i < _temp3466; i ++){ _temp3465[
i]= Cyc_yylval;}}; _temp3465;}), sizeof( void*),( unsigned int) 200); int
yylsp_offset; struct _tagged_arr yyls= _tag_arr(({ unsigned int _temp3461=(
unsigned int) 200; struct Cyc_Yyltype* _temp3462=( struct Cyc_Yyltype*)
GC_malloc( sizeof( struct Cyc_Yyltype) * _temp3461);{ unsigned int _temp3463=
_temp3461; unsigned int i; for( i= 0; i < _temp3463; i ++){ _temp3462[ i]= Cyc_yynewloc();}};
_temp3462;}), sizeof( struct Cyc_Yyltype),( unsigned int) 200); int yystacksize=
200; void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs=
0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0;
yynewstate:*(( short*) _check_unknown_subscript( yyss, sizeof( short), ++
yyssp_offset))=( short) yystate; if( yyssp_offset >= yystacksize - 1){ if(
yystacksize >= 10000){ Cyc_yyerror( _tag_arr("parser stack overflow", sizeof(
unsigned char), 22u));( int) _throw(( void*) Cyc_Yystack_overflow);} yystacksize
*= 2; if( yystacksize > 10000){ yystacksize= 10000;}{ struct _tagged_arr yyss1=({
unsigned int _temp1549=( unsigned int) yystacksize; short* _temp1550=( short*)
GC_malloc_atomic( sizeof( short) * _temp1549); struct _tagged_arr _temp1552=
_tag_arr( _temp1550, sizeof( short),( unsigned int) yystacksize);{ unsigned int
_temp1551= _temp1549; unsigned int i; for( i= 0; i < _temp1551; i ++){ _temp1550[
i]= 0;}}; _temp1552;}); struct _tagged_arr yyvs1=({ unsigned int _temp1545=(
unsigned int) yystacksize; void** _temp1546=( void**) GC_malloc( sizeof( void*)
* _temp1545); struct _tagged_arr _temp1548= _tag_arr( _temp1546, sizeof( void*),(
unsigned int) yystacksize);{ unsigned int _temp1547= _temp1545; unsigned int i;
for( i= 0; i < _temp1547; i ++){ _temp1546[ i]= Cyc_yylval;}}; _temp1548;});
struct _tagged_arr yyls1=({ unsigned int _temp1541=( unsigned int) yystacksize;
struct Cyc_Yyltype* _temp1542=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct
Cyc_Yyltype) * _temp1541); struct _tagged_arr _temp1544= _tag_arr( _temp1542,
sizeof( struct Cyc_Yyltype),( unsigned int) yystacksize);{ unsigned int
_temp1543= _temp1541; unsigned int i; for( i= 0; i < _temp1543; i ++){ _temp1542[
i]= Cyc_yynewloc();}}; _temp1544;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){*((
short*) _check_unknown_subscript( yyss1, sizeof( short), i))=*(( short*)
_check_unknown_subscript( yyss, sizeof( short), i));*(( void**)
_check_unknown_subscript( yyvs1, sizeof( void*), i))=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), i));*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls1, sizeof( struct Cyc_Yyltype), i))=*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), i));}} yyss= yyss1;
yyvs= yyvs1; yyls= yyls1;}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 798u, yystate)]; if( yyn == - 32768){ goto
yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <=
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
352: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 353u, Cyc_yychar)]:
238;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4792)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4793u, yyn)] != yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4793u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrlab;}} if( yyn == 797){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar=
- 2;}*(( void**) _check_unknown_subscript( yyvs, sizeof( void*), ++ yyvsp_offset))=
Cyc_yylval;*(( struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof(
struct Cyc_Yyltype), ++ yylsp_offset))= Cyc_yylloc; if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int) Cyc_yydefact[
_check_known_subscript_notnull( 798u, yystate)]; if( yyn == 0){ goto yyerrlab;}
yyreduce: yylen=( int) Cyc_yyr2[ _check_known_subscript_notnull( 409u, yyn)];
if( yylen > 0){ yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),(
yyvsp_offset + 1) - yylen));} switch( yyn){ case 1: _LL1553: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); break; case 2: _LL1554: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1556=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1556[ 0]=({ struct Cyc_DeclList_tok_struct _temp1557; _temp1557.tag= Cyc_DeclList_tok;
_temp1557.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))));
_temp1557;}); _temp1556;}); break; case 3: _LL1555: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1559=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1559[ 0]=({ struct Cyc_DeclList_tok_struct _temp1560; _temp1560.tag= Cyc_DeclList_tok;
_temp1560.f1=({ struct Cyc_List_List* _temp1561=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1561->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1562=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1562->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1563=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1563[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1564; _temp1564.tag= Cyc_Absyn_Using_d;
_temp1564.f1= Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp1564.f2= Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1564;}); _temp1563;})); _temp1562->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp1562;}); _temp1561->tl= 0; _temp1561;}); _temp1560;}); _temp1559;}); Cyc_Lex_leave_using();
break; case 4: _LL1558: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1566=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1566[ 0]=({ struct Cyc_DeclList_tok_struct _temp1567; _temp1567.tag= Cyc_DeclList_tok;
_temp1567.f1=({ struct Cyc_List_List* _temp1568=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1568->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1569=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1569->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1570=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1570[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1571; _temp1571.tag= Cyc_Absyn_Using_d;
_temp1571.f1= Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 4))); _temp1571.f2= Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp1571;}); _temp1570;})); _temp1569->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line);
_temp1569;}); _temp1568->tl= Cyc_yyget_DeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1568;});
_temp1567;}); _temp1566;}); break; case 5: _LL1565: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1573=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1573[ 0]=({ struct Cyc_DeclList_tok_struct _temp1574; _temp1574.tag= Cyc_DeclList_tok;
_temp1574.f1=({ struct Cyc_List_List* _temp1575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1575->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1576=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1576->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1577=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1577[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1578; _temp1578.tag=
Cyc_Absyn_Namespace_d; _temp1578.f1=({ struct _tagged_arr* _temp1579=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1579[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp1579;}); _temp1578.f2= Cyc_yyget_DeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1578;});
_temp1577;})); _temp1576->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp1576;}); _temp1575->tl= 0; _temp1575;});
_temp1574;}); _temp1573;}); Cyc_Lex_leave_namespace(); break; case 6: _LL1572:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1581=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1581[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1582; _temp1582.tag= Cyc_DeclList_tok; _temp1582.f1=({ struct Cyc_List_List*
_temp1583=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1583->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1584=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1584->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp1585=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1585[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1586; _temp1586.tag= Cyc_Absyn_Namespace_d;
_temp1586.f1=({ struct _tagged_arr* _temp1587=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp1587[ 0]= Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))); _temp1587;});
_temp1586.f2= Cyc_yyget_DeclList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp1586;}); _temp1585;})); _temp1584->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line); _temp1584;}); _temp1583->tl= Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1583;}); _temp1582;}); _temp1581;}); break; case 7: _LL1580: if( Cyc_String_strcmp(
Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 4))), _tag_arr("C", sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err(
_tag_arr("only extern \"C\" { ... } is allowed", sizeof( unsigned char), 35u),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).last_line));} yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1589=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1589[ 0]=({ struct Cyc_DeclList_tok_struct _temp1590; _temp1590.tag= Cyc_DeclList_tok;
_temp1590.f1=({ struct Cyc_List_List* _temp1591=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1591->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1592=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1592->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp1593=(
struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp1593[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp1594; _temp1594.tag= Cyc_Absyn_ExternC_d;
_temp1594.f1= Cyc_yyget_DeclList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp1594;}); _temp1593;})); _temp1592->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line); _temp1592;}); _temp1591->tl= Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1591;}); _temp1590;}); _temp1589;}); break; case 8: _LL1588: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1596=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1596[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1597; _temp1597.tag= Cyc_DeclList_tok; _temp1597.f1= 0; _temp1597;});
_temp1596;}); break; case 9: _LL1595: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1599=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1599[ 0]=({ struct Cyc_DeclList_tok_struct _temp1600; _temp1600.tag= Cyc_DeclList_tok;
_temp1600.f1=({ struct Cyc_List_List* _temp1601=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1601->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp1602=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1602[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1603; _temp1603.tag= Cyc_Absyn_Fn_d; _temp1603.f1= Cyc_yyget_FnDecl_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1603;}); _temp1602;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1601->tl= 0; _temp1601;}); _temp1600;});
_temp1599;}); break; case 10: _LL1598: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 11:
_LL1604: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1606=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1606[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1607; _temp1607.tag= Cyc_FnDecl_tok; _temp1607.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1))), 0, Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1607;}); _temp1606;}); break; case 12: _LL1605: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1609=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1609[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1610; _temp1610.tag= Cyc_FnDecl_tok;
_temp1610.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1611=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1611->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))); _temp1611;}), Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), 0, Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1610;}); _temp1609;}); break; case 13: _LL1608: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1613=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1613[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1614; _temp1614.tag= Cyc_FnDecl_tok;
_temp1614.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1614;}); _temp1613;}); break; case 14: _LL1612: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1616=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1616[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1617; _temp1617.tag= Cyc_FnDecl_tok;
_temp1617.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1618=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1618->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3))); _temp1618;}), Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1617;}); _temp1616;}); break; case 15: _LL1615: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1620=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1620[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1621; _temp1621.tag= Cyc_FnDecl_tok;
_temp1621.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1622=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1622->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))); _temp1622;}), Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), 0, Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1621;}); _temp1620;}); break; case 16: _LL1619: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1624=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1624[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1625; _temp1625.tag= Cyc_FnDecl_tok;
_temp1625.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1626=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1626->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3))); _temp1626;}), Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1625;}); _temp1624;}); break; case 17: _LL1623: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))); yyval=*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break;
case 18: _LL1627: Cyc_Lex_leave_using(); break; case 19: _LL1628: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1630=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1630[ 0]= Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1630;}));
yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset));
break; case 20: _LL1629: Cyc_Lex_leave_namespace(); break; case 21: _LL1631:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1633=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1633[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1634; _temp1634.tag= Cyc_DeclList_tok; _temp1634.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1))), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); _temp1634;}); _temp1633;}); break; case 22:
_LL1632: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1636=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1636[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1637; _temp1637.tag= Cyc_DeclList_tok; _temp1637.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))), Cyc_yyget_InitDeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1637;}); _temp1636;}); break; case 23: _LL1635: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1639=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1639[ 0]=({ struct Cyc_DeclList_tok_struct _temp1640; _temp1640.tag= Cyc_DeclList_tok;
_temp1640.f1=({ struct Cyc_List_List* _temp1641=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1641->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3))), 0, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1641->tl= 0; _temp1641;}); _temp1640;}); _temp1639;}); break; case 24:
_LL1638: { struct Cyc_List_List* _temp1643= 0; goto _LL1644; _LL1644:{ struct
Cyc_List_List* _temp1645= Cyc_yyget_IdList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); goto
_LL1646; _LL1646: for( 0; _temp1645 != 0; _temp1645=(( struct Cyc_List_List*)
_check_null( _temp1645))->tl){ struct _tagged_arr* _temp1647=( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp1645))->hd; goto
_LL1648; _LL1648: { struct _tuple1* _temp1652=({ struct _tuple1* _temp1649=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1649->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp1650=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp1650[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp1651; _temp1651.tag= Cyc_Absyn_Rel_n; _temp1651.f1= 0; _temp1651;});
_temp1650;}); _temp1649->f2= _temp1647; _temp1649;}); goto _LL1653; _LL1653: {
struct Cyc_Absyn_Vardecl* _temp1654= Cyc_Absyn_new_vardecl( _temp1652, Cyc_Absyn_wildtyp(
0), 0); goto _LL1655; _LL1655: _temp1643=({ struct Cyc_List_List* _temp1656=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1656->hd=(
void*) _temp1654; _temp1656->tl= _temp1643; _temp1656;});}}}} _temp1643=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1643);
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1657=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1657[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1658; _temp1658.tag= Cyc_DeclList_tok; _temp1658.f1=({ struct Cyc_List_List*
_temp1659=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1659->hd=( void*) Cyc_Absyn_letv_decl( _temp1643, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1659->tl= 0; _temp1659;}); _temp1658;}); _temp1657;}); break;} case 25:
_LL1642: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 26: _LL1660: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1662=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1662[ 0]=({ struct Cyc_DeclList_tok_struct _temp1663; _temp1663.tag= Cyc_DeclList_tok;
_temp1663.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))));
_temp1663;}); _temp1662;}); break; case 27: _LL1661: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1665=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1665[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1666; _temp1666.tag= Cyc_DeclSpec_tok;
_temp1666.f1=({ struct Cyc_Parse_Declaration_spec* _temp1667=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1667->sc=({ struct
Cyc_Core_Opt* _temp1668=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1668->v=( void*) Cyc_yyget_StorageClass_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1668;});
_temp1667->tq= Cyc_Absyn_empty_tqual(); _temp1667->type_specs= 0; _temp1667->is_inline=
0; _temp1667->attributes= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1667;});
_temp1666;}); _temp1665;}); break; case 28: _LL1664: if(( Cyc_yyget_DeclSpec_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->sc !=
0){ Cyc_Parse_warn( _tag_arr("Only one storage class is allowed in a declaration",
sizeof( unsigned char), 51u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));} yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1670=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1670[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1671; _temp1671.tag= Cyc_DeclSpec_tok;
_temp1671.f1=({ struct Cyc_Parse_Declaration_spec* _temp1672=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1672->sc=({ struct
Cyc_Core_Opt* _temp1673=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1673->v=( void*) Cyc_yyget_StorageClass_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp1673;});
_temp1672->tq=( Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))))->tq; _temp1672->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->type_specs;
_temp1672->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->is_inline;
_temp1672->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))),(
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))))->attributes); _temp1672;}); _temp1671;}); _temp1670;}); break;
case 29: _LL1669: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1675=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1675[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1676; _temp1676.tag= Cyc_DeclSpec_tok;
_temp1676.f1=({ struct Cyc_Parse_Declaration_spec* _temp1677=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1677->sc= 0;
_temp1677->tq= Cyc_Absyn_empty_tqual(); _temp1677->type_specs=({ struct Cyc_List_List*
_temp1678=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1678->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1678->tl=
0; _temp1678;}); _temp1677->is_inline= 0; _temp1677->attributes= Cyc_yyget_AttributeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1677;}); _temp1676;}); _temp1675;}); break; case 30: _LL1674: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1680=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1680[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1681; _temp1681.tag= Cyc_DeclSpec_tok; _temp1681.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1682=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1682->sc=( Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))))->sc; _temp1682->tq=( Cyc_yyget_DeclSpec_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->tq;
_temp1682->type_specs=({ struct Cyc_List_List* _temp1683=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1683->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp1683->tl=( Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))))->type_specs; _temp1683;}); _temp1682->is_inline=(
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))))->is_inline; _temp1682->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))),(
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))))->attributes); _temp1682;}); _temp1681;}); _temp1680;}); break;
case 31: _LL1679: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1685=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1685[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1686; _temp1686.tag= Cyc_DeclSpec_tok;
_temp1686.f1=({ struct Cyc_Parse_Declaration_spec* _temp1687=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1687->sc= 0;
_temp1687->tq= Cyc_yyget_TypeQual_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp1687->type_specs= 0; _temp1687->is_inline=
0; _temp1687->attributes= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1687;});
_temp1686;}); _temp1685;}); break; case 32: _LL1684: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1689=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1689[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1690; _temp1690.tag= Cyc_DeclSpec_tok;
_temp1690.f1=({ struct Cyc_Parse_Declaration_spec* _temp1691=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1691->sc=( Cyc_yyget_DeclSpec_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->sc;
_temp1691->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),( Cyc_yyget_DeclSpec_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->tq);
_temp1691->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->type_specs;
_temp1691->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->is_inline;
_temp1691->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))),(
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))))->attributes); _temp1691;}); _temp1690;}); _temp1689;}); break;
case 33: _LL1688: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1693=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1693[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1694; _temp1694.tag= Cyc_DeclSpec_tok;
_temp1694.f1=({ struct Cyc_Parse_Declaration_spec* _temp1695=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1695->sc= 0;
_temp1695->tq= Cyc_Absyn_empty_tqual(); _temp1695->type_specs= 0; _temp1695->is_inline=
1; _temp1695->attributes= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1695;});
_temp1694;}); _temp1693;}); break; case 34: _LL1692: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1697=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1697[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1698; _temp1698.tag= Cyc_DeclSpec_tok;
_temp1698.f1=({ struct Cyc_Parse_Declaration_spec* _temp1699=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1699->sc=( Cyc_yyget_DeclSpec_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->sc;
_temp1699->tq=( Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))))->tq; _temp1699->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->type_specs;
_temp1699->is_inline= 1; _temp1699->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))),(
Cyc_yyget_DeclSpec_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))))->attributes); _temp1699;}); _temp1698;}); _temp1697;}); break;
case 35: _LL1696: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1701=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1701[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1702; _temp1702.tag=
Cyc_StorageClass_tok; _temp1702.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp1702;}); _temp1701;}); break; case 36: _LL1700: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1704=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1704[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1705; _temp1705.tag=
Cyc_StorageClass_tok; _temp1705.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1705;}); _temp1704;}); break; case 37: _LL1703: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1707=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1707[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1708; _temp1708.tag=
Cyc_StorageClass_tok; _temp1708.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1708;}); _temp1707;}); break; case 38: _LL1706: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1710=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1710[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1711; _temp1711.tag=
Cyc_StorageClass_tok; _temp1711.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1711;}); _temp1710;}); break; case 39: _LL1709: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))),
_tag_arr("C", sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1713=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1713[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1714; _temp1714.tag=
Cyc_StorageClass_tok; _temp1714.f1=( void*)(( void*) Cyc_Parse_ExternC_sc);
_temp1714;}); _temp1713;}); break; case 40: _LL1712: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1716=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1716[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1717; _temp1717.tag=
Cyc_StorageClass_tok; _temp1717.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1717;}); _temp1716;}); break; case 41: _LL1715: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1719=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1719[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1720; _temp1720.tag=
Cyc_StorageClass_tok; _temp1720.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1720;}); _temp1719;}); break; case 42: _LL1718: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1722=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1722[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1723; _temp1723.tag=
Cyc_AttributeList_tok; _temp1723.f1= 0; _temp1723;}); _temp1722;}); break; case
43: _LL1721: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 44: _LL1724: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1726=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1726[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1727; _temp1727.tag=
Cyc_AttributeList_tok; _temp1727.f1= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp1727;});
_temp1726;}); break; case 45: _LL1725: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1729=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1729[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1730; _temp1730.tag=
Cyc_AttributeList_tok; _temp1730.f1=({ struct Cyc_List_List* _temp1731=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1731->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1731->tl= 0; _temp1731;}); _temp1730;}); _temp1729;});
break; case 46: _LL1728: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1733=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1733[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1734; _temp1734.tag=
Cyc_AttributeList_tok; _temp1734.f1=({ struct Cyc_List_List* _temp1735=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1735->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))); _temp1735->tl= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1735;});
_temp1734;}); _temp1733;}); break; case 47: _LL1732: { struct _tagged_arr
_temp1737= Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); goto _LL1738; _LL1738: if(((( _get_arr_size(
_temp1737, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1737, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1737, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1737,
sizeof( unsigned char),( int)( _get_arr_size( _temp1737, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1737,
sizeof( unsigned char),( int)( _get_arr_size( _temp1737, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1737=( struct _tagged_arr) Cyc_String_substring(
_temp1737, 2,( int)( _get_arr_size( _temp1737, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1737,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1739=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1739[ 0]=({ struct Cyc_Attribute_tok_struct _temp1740; _temp1740.tag= Cyc_Attribute_tok;
_temp1740.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1740;}); _temp1739;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1741=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1741[ 0]=({ struct Cyc_Attribute_tok_struct _temp1742; _temp1742.tag= Cyc_Attribute_tok;
_temp1742.f1=( void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1742;}); _temp1741;});}
break;}} case 48: _LL1736: yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1744=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1744[ 0]=({ struct Cyc_Attribute_tok_struct _temp1745; _temp1745.tag= Cyc_Attribute_tok;
_temp1745.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1745;}); _temp1744;});
break; case 49: _LL1743: { struct _tagged_arr _temp1747= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL1748; _LL1748: { struct _tuple16 _temp1751; int _temp1752; void*
_temp1754; struct _tuple16* _temp1749= Cyc_yyget_Int_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1751=*
_temp1749; _LL1755: _temp1754= _temp1751.f1; goto _LL1753; _LL1753: _temp1752=
_temp1751.f2; goto _LL1750; _LL1750: { void* a; if( Cyc_String_zstrcmp(
_temp1747, _tag_arr("regparm", sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp(
_temp1747, _tag_arr("__regparm__", sizeof( unsigned char), 12u)) == 0){ if(
_temp1752 <= 0? 1: _temp1752 > 3){ Cyc_Parse_err( _tag_arr("regparm requires value between 1 and 3",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1756=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct));
_temp1756[ 0]=({ struct Cyc_Absyn_Regparm_att_struct _temp1757; _temp1757.tag=
Cyc_Absyn_Regparm_att; _temp1757.f1= _temp1752; _temp1757;}); _temp1756;});}
else{ if( Cyc_String_zstrcmp( _temp1747, _tag_arr("aligned", sizeof(
unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1747, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1752 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));}{ unsigned int j=( unsigned int) _temp1752;
for( 0;( j & 1) == 0; j= j >> 1){;} j= j >> 1; if( j != 0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1758=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp1758[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp1759; _temp1759.tag=
Cyc_Absyn_Aligned_att; _temp1759.f1= _temp1752; _temp1759;}); _temp1758;});}}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1760=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1760[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1761; _temp1761.tag= Cyc_Attribute_tok; _temp1761.f1=(
void*) a; _temp1761;}); _temp1760;}); break;}}} case 50: _LL1746: { struct
_tagged_arr _temp1763= Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 3))); goto _LL1764; _LL1764: { struct
_tagged_arr _temp1765= Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))); goto _LL1766; _LL1766: { void* a; if(
Cyc_String_zstrcmp( _temp1763, _tag_arr("section", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1763, _tag_arr("__section__", sizeof(
unsigned char), 12u))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1767=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1767[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1768; _temp1768.tag=
Cyc_Absyn_Section_att; _temp1768.f1= _temp1765; _temp1768;}); _temp1767;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1769=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1769[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1770; _temp1770.tag= Cyc_Attribute_tok; _temp1770.f1=(
void*) a; _temp1770;}); _temp1769;}); break;}}} case 51: _LL1762: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1772=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1772[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1773; _temp1773.tag= Cyc_TypeSpecifier_tok;
_temp1773.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp1773;});
_temp1772;}); break; case 52: _LL1771: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1775=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1775[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1776; _temp1776.tag=
Cyc_TypeSpecifier_tok; _temp1776.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1776;}); _temp1775;}); break; case 53: _LL1774:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1778=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1778[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1779; _temp1779.tag= Cyc_TypeSpecifier_tok;
_temp1779.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp1779;});
_temp1778;}); break; case 54: _LL1777: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1781=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1781[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1782; _temp1782.tag=
Cyc_TypeSpecifier_tok; _temp1782.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1783=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1783[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1784; _temp1784.tag=
Cyc_Parse_Short_spec; _temp1784.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp1784;}); _temp1783;})); _temp1782;}); _temp1781;});
break; case 55: _LL1780: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1786=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1786[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1787; _temp1787.tag=
Cyc_TypeSpecifier_tok; _temp1787.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1787;}); _temp1786;}); break; case 56: _LL1785: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1789=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1789[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1790; _temp1790.tag=
Cyc_TypeSpecifier_tok; _temp1790.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1791=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1791[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1792; _temp1792.tag= Cyc_Parse_Long_spec;
_temp1792.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp1792;}); _temp1791;})); _temp1790;}); _temp1789;});
break; case 57: _LL1788: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1794=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1794[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1795; _temp1795.tag=
Cyc_TypeSpecifier_tok; _temp1795.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1795;}); _temp1794;}); break; case 58: _LL1793: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1797=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1797[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1798; _temp1798.tag=
Cyc_TypeSpecifier_tok; _temp1798.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1798;}); _temp1797;}); break; case 59: _LL1796: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1800=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1800[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1801; _temp1801.tag=
Cyc_TypeSpecifier_tok; _temp1801.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1802=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1802[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1803; _temp1803.tag=
Cyc_Parse_Signed_spec; _temp1803.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp1803;}); _temp1802;})); _temp1801;}); _temp1800;});
break; case 60: _LL1799: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1805=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1805[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1806; _temp1806.tag=
Cyc_TypeSpecifier_tok; _temp1806.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1807=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1807[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1808; _temp1808.tag= Cyc_Parse_Unsigned_spec; _temp1808.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line); _temp1808;});
_temp1807;})); _temp1806;}); _temp1805;}); break; case 61: _LL1804: yyval=*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break;
case 62: _LL1809: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 63: _LL1810: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 64:
_LL1811: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1813=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1813[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1814; _temp1814.tag=
Cyc_TypeSpecifier_tok; _temp1814.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1814;}); _temp1813;}); break; case 65: _LL1812: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1816=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1816[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1817; _temp1817.tag=
Cyc_TypeSpecifier_tok; _temp1817.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1817;}); _temp1816;}); break; case 66: _LL1815:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1819=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1819[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1820; _temp1820.tag= Cyc_TypeSpecifier_tok;
_temp1820.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp1821=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp1821[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp1822; _temp1822.tag=
Cyc_Absyn_TypedefType; _temp1822.f1= Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1822.f2=
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1822.f3= 0; _temp1822;}); _temp1821;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1820;}); _temp1819;}); break; case 67: _LL1818: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1824=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1824[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1825; _temp1825.tag=
Cyc_TypeSpecifier_tok; _temp1825.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1826=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1826[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1827; _temp1827.tag= Cyc_Absyn_TupleType;
_temp1827.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))));
_temp1827;}); _temp1826;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1825;}); _temp1824;}); break; case 68: _LL1823:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1829=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1829[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1830; _temp1830.tag= Cyc_TypeSpecifier_tok;
_temp1830.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1831=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1831[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1832; _temp1832.tag= Cyc_Absyn_RgnHandleType; _temp1832.f1=( void*) Cyc_yyget_Type_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1832;}); _temp1831;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1830;}); _temp1829;}); break; case 69: _LL1828:
yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp1834=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1834[ 0]=({ struct Cyc_Kind_tok_struct
_temp1835; _temp1835.tag= Cyc_Kind_tok; _temp1835.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1835;}); _temp1834;}); break; case 70: _LL1833: {
struct _tagged_arr* _temp1839; void* _temp1841; struct _tuple1 _temp1837=* Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _LL1842:
_temp1841= _temp1837.f1; goto _LL1840; _LL1840: _temp1839= _temp1837.f2; goto
_LL1838; _LL1838: if( _temp1841 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(
_tag_arr("bad kind in type specifier", sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));} yyval=( void*)({
struct Cyc_Kind_tok_struct* _temp1843=( struct Cyc_Kind_tok_struct*) GC_malloc(
sizeof( struct Cyc_Kind_tok_struct)); _temp1843[ 0]=({ struct Cyc_Kind_tok_struct
_temp1844; _temp1844.tag= Cyc_Kind_tok; _temp1844.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1839, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1844;}); _temp1843;}); break;} case 71:
_LL1836: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1846=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1846[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1847; _temp1847.tag= Cyc_TypeQual_tok; _temp1847.f1=({ struct Cyc_Absyn_Tqual
_temp1848; _temp1848.q_const= 1; _temp1848.q_volatile= 0; _temp1848.q_restrict=
0; _temp1848;}); _temp1847;}); _temp1846;}); break; case 72: _LL1845: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1850=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1850[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1851; _temp1851.tag= Cyc_TypeQual_tok; _temp1851.f1=({ struct Cyc_Absyn_Tqual
_temp1852; _temp1852.q_const= 0; _temp1852.q_volatile= 1; _temp1852.q_restrict=
0; _temp1852;}); _temp1851;}); _temp1850;}); break; case 73: _LL1849: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1854=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1854[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1855; _temp1855.tag= Cyc_TypeQual_tok; _temp1855.f1=({ struct Cyc_Absyn_Tqual
_temp1856; _temp1856.q_const= 0; _temp1856.q_volatile= 0; _temp1856.q_restrict=
1; _temp1856;}); _temp1855;}); _temp1854;}); break; case 74: _LL1853: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1858=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1858[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1859; _temp1859.tag= Cyc_TypeSpecifier_tok;
_temp1859.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1860=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1860[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1861; _temp1861.tag= Cyc_Parse_Decl_spec;
_temp1861.f1=({ struct Cyc_Absyn_Decl* _temp1862=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1862->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1863=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1863[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1864; _temp1864.tag= Cyc_Absyn_Enum_d; _temp1864.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1865=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1865->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1865->name= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3)));
_temp1865->fields=({ struct Cyc_Core_Opt* _temp1866=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1866->v=( void*) Cyc_yyget_EnumfieldList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1866;}); _temp1865;}); _temp1864;}); _temp1863;})); _temp1862->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp1862;}); _temp1861;}); _temp1860;})); _temp1859;}); _temp1858;}); break;
case 75: _LL1857: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1868=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1868[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1869; _temp1869.tag=
Cyc_TypeSpecifier_tok; _temp1869.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1870=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1870[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1871; _temp1871.tag= Cyc_Absyn_EnumType;
_temp1871.f1= Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp1871.f2= 0; _temp1871;}); _temp1870;}),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp1869;}); _temp1868;}); break; case 76: _LL1867:
yyval=( void*)({ struct Cyc_Enumfield_tok_struct* _temp1873=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp1873[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp1874; _temp1874.tag= Cyc_Enumfield_tok; _temp1874.f1=({
struct Cyc_Absyn_Enumfield* _temp1875=( struct Cyc_Absyn_Enumfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp1875->name= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1875->tag= 0; _temp1875->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp1875;}); _temp1874;}); _temp1873;}); break;
case 77: _LL1872: yyval=( void*)({ struct Cyc_Enumfield_tok_struct* _temp1877=(
struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1877[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1878; _temp1878.tag= Cyc_Enumfield_tok;
_temp1878.f1=({ struct Cyc_Absyn_Enumfield* _temp1879=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1879->name= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp1879->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1879->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp1879;}); _temp1878;}); _temp1877;}); break;
case 78: _LL1876: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1881=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1881[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1882; _temp1882.tag=
Cyc_EnumfieldList_tok; _temp1882.f1=({ struct Cyc_List_List* _temp1883=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1883->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1883->tl= 0; _temp1883;}); _temp1882;}); _temp1881;});
break; case 79: _LL1880: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1885=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1885[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1886; _temp1886.tag=
Cyc_EnumfieldList_tok; _temp1886.f1=({ struct Cyc_List_List* _temp1887=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1887->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))); _temp1887->tl= Cyc_yyget_EnumfieldList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp1887;});
_temp1886;}); _temp1885;}); break; case 80: _LL1884: { void* t;{ void* _temp1889=
Cyc_yyget_StructOrUnion_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 3))); _LL1891: if( _temp1889 ==( void*) Cyc_Parse_Struct_su){
goto _LL1892;} else{ goto _LL1893;} _LL1893: if( _temp1889 ==( void*) Cyc_Parse_Union_su){
goto _LL1894;} else{ goto _LL1890;} _LL1892: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1895=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1895[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1896; _temp1896.tag= Cyc_Absyn_AnonStructType; _temp1896.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1896;}); _temp1895;}); goto _LL1890; _LL1894: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1897=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1897[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1898; _temp1898.tag= Cyc_Absyn_AnonUnionType; _temp1898.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1898;}); _temp1897;}); goto _LL1890; _LL1890:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1899=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1899[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1900; _temp1900.tag=
Cyc_TypeSpecifier_tok; _temp1900.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1901=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1901[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1902; _temp1902.tag= Cyc_Parse_Type_spec;
_temp1902.f1=( void*) t; _temp1902.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp1902;}); _temp1901;})); _temp1900;}); _temp1899;}); break;} case 81:
_LL1888: { struct Cyc_List_List* _temp1904=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).last_line),
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3)))); goto _LL1905; _LL1905: { struct Cyc_Absyn_Decl* d;{ void*
_temp1906= Cyc_yyget_StructOrUnion_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 5))); _LL1908: if( _temp1906 ==( void*) Cyc_Parse_Struct_su){
goto _LL1909;} else{ goto _LL1910;} _LL1910: if( _temp1906 ==( void*) Cyc_Parse_Union_su){
goto _LL1911;} else{ goto _LL1907;} _LL1909: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1912=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1912->v=( void*) Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp1912;}), _temp1904,({ struct Cyc_Core_Opt* _temp1913=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1913->v=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1913;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); goto _LL1907; _LL1911: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1914=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1914->v=( void*) Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp1914;}), _temp1904,({ struct Cyc_Core_Opt* _temp1915=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1915->v=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1915;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); goto _LL1907; _LL1907:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1916=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1916[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1917; _temp1917.tag= Cyc_TypeSpecifier_tok;
_temp1917.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1918=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1918[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1919; _temp1919.tag= Cyc_Parse_Decl_spec;
_temp1919.f1= d; _temp1919;}); _temp1918;})); _temp1917;}); _temp1916;}); break;}}
case 82: _LL1903: { struct Cyc_List_List* _temp1921=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).last_line),
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3)))); goto _LL1922; _LL1922: { struct Cyc_Absyn_Decl* d;{ void*
_temp1923= Cyc_yyget_StructOrUnion_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 5))); _LL1925: if( _temp1923 ==( void*) Cyc_Parse_Struct_su){
goto _LL1926;} else{ goto _LL1927;} _LL1927: if( _temp1923 ==( void*) Cyc_Parse_Union_su){
goto _LL1928;} else{ goto _LL1924;} _LL1926: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1929=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1929->v=( void*) Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp1929;}), _temp1921,({ struct Cyc_Core_Opt* _temp1930=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1930->v=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1930;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); goto _LL1924; _LL1928: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1931=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1931->v=( void*) Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp1931;}), _temp1921,({ struct Cyc_Core_Opt* _temp1932=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1932->v=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp1932;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); goto _LL1924; _LL1924:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1933=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1933[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1934; _temp1934.tag= Cyc_TypeSpecifier_tok;
_temp1934.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1935=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1935[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1936; _temp1936.tag= Cyc_Parse_Decl_spec;
_temp1936.f1= d; _temp1936;}); _temp1935;})); _temp1934;}); _temp1933;}); break;}}
case 83: _LL1920:{ void* _temp1938= Cyc_yyget_StructOrUnion_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _LL1940:
if( _temp1938 ==( void*) Cyc_Parse_Struct_su){ goto _LL1941;} else{ goto _LL1942;}
_LL1942: if( _temp1938 ==( void*) Cyc_Parse_Union_su){ goto _LL1943;} else{ goto
_LL1939;} _LL1941: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1944=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1944[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1945; _temp1945.tag=
Cyc_TypeSpecifier_tok; _temp1945.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1946=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1946[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1947; _temp1947.tag= Cyc_Absyn_StructType;
_temp1947.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1947.f2=
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1947.f3= 0; _temp1947;}); _temp1946;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1945;}); _temp1944;}); goto _LL1939; _LL1943: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1948=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1948[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1949; _temp1949.tag=
Cyc_TypeSpecifier_tok; _temp1949.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1950=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1950[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1951; _temp1951.tag= Cyc_Absyn_UnionType;
_temp1951.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1951.f2=
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1951.f3= 0; _temp1951;}); _temp1950;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1949;}); _temp1948;}); goto _LL1939; _LL1939:;} break; case 84: _LL1937:{
void* _temp1953= Cyc_yyget_StructOrUnion_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _LL1955:
if( _temp1953 ==( void*) Cyc_Parse_Struct_su){ goto _LL1956;} else{ goto _LL1957;}
_LL1957: if( _temp1953 ==( void*) Cyc_Parse_Union_su){ goto _LL1958;} else{ goto
_LL1954;} _LL1956: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1959=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1959[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1960; _temp1960.tag=
Cyc_TypeSpecifier_tok; _temp1960.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1961=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1961[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1962; _temp1962.tag= Cyc_Absyn_StructType;
_temp1962.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1962.f2=
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1962.f3= 0; _temp1962;}); _temp1961;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1960;}); _temp1959;}); goto _LL1954; _LL1958: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1963=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1963[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1964; _temp1964.tag=
Cyc_TypeSpecifier_tok; _temp1964.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1965=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1965[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1966; _temp1966.tag= Cyc_Absyn_UnionType;
_temp1966.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1966.f2=
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1966.f3= 0; _temp1966;}); _temp1965;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp1964;}); _temp1963;}); goto _LL1954; _LL1954:;} break; case 85: _LL1952:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp1968=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1968[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1969; _temp1969.tag= Cyc_TypeList_tok; _temp1969.f1= 0; _temp1969;});
_temp1968;}); break; case 86: _LL1967: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1971=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1971[ 0]=({ struct Cyc_TypeList_tok_struct _temp1972; _temp1972.tag= Cyc_TypeList_tok;
_temp1972.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1)))); _temp1972;}); _temp1971;}); break; case 87: _LL1970:((
struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1; yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp1974=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1974[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1975; _temp1975.tag= Cyc_TypeList_tok; _temp1975.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp1975;});
_temp1974;}); break; case 88: _LL1973: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1977=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1977[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1978; _temp1978.tag=
Cyc_StructOrUnion_tok; _temp1978.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp1978;}); _temp1977;}); break; case 89: _LL1976: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1980=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1980[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1981; _temp1981.tag=
Cyc_StructOrUnion_tok; _temp1981.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp1981;}); _temp1980;}); break; case 90: _LL1979: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1983=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1983[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1984; _temp1984.tag= Cyc_StructFieldDeclList_tok; _temp1984.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))));
_temp1984;}); _temp1983;}); break; case 91: _LL1982: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1986=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1986[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1987; _temp1987.tag= Cyc_StructFieldDeclListList_tok; _temp1987.f1=({
struct Cyc_List_List* _temp1988=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1988->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1988->tl= 0; _temp1988;}); _temp1987;}); _temp1986;}); break; case 92:
_LL1985: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1990=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1990[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1991; _temp1991.tag= Cyc_StructFieldDeclListList_tok; _temp1991.f1=({
struct Cyc_List_List* _temp1992=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1992->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp1992->tl= Cyc_yyget_StructFieldDeclListList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp1992;});
_temp1991;}); _temp1990;}); break; case 93: _LL1989: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1994=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1994[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1995; _temp1995.tag=
Cyc_InitDeclList_tok; _temp1995.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))); _temp1995;});
_temp1994;}); break; case 94: _LL1993: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1997=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1997[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1998; _temp1998.tag=
Cyc_InitDeclList_tok; _temp1998.f1=({ struct Cyc_List_List* _temp1999=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1999->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp1999->tl= 0; _temp1999;}); _temp1998;}); _temp1997;});
break; case 95: _LL1996: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp2001=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2001[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2002; _temp2002.tag=
Cyc_InitDeclList_tok; _temp2002.f1=({ struct Cyc_List_List* _temp2003=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2003->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp2003->tl= Cyc_yyget_InitDeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2003;});
_temp2002;}); _temp2001;}); break; case 96: _LL2000: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp2005=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp2005[ 0]=({ struct Cyc_InitDecl_tok_struct _temp2006; _temp2006.tag= Cyc_InitDecl_tok;
_temp2006.f1=({ struct _tuple15* _temp2007=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp2007->f1= Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2007->f2=
0; _temp2007;}); _temp2006;}); _temp2005;}); break; case 97: _LL2004: yyval=(
void*)({ struct Cyc_InitDecl_tok_struct* _temp2009=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2009[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2010; _temp2010.tag= Cyc_InitDecl_tok; _temp2010.f1=({ struct _tuple15*
_temp2011=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2011->f1=
Cyc_yyget_Declarator_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 2))); _temp2011->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2011;}); _temp2010;}); _temp2009;}); break; case 98: _LL2008: { struct Cyc_Absyn_Tqual
tq=(* Cyc_yyget_QualSpecList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2)))).f1; struct Cyc_List_List* _temp2013=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)))).f3;
goto _LL2014; _LL2014: { void* _temp2015= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).last_line)); goto _LL2016; _LL2016: { struct Cyc_List_List*
_temp2019; struct Cyc_List_List* _temp2021; struct _tuple0 _temp2017=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))));
_LL2022: _temp2021= _temp2017.f1; goto _LL2020; _LL2020: _temp2019= _temp2017.f2;
goto _LL2018; _LL2018: { struct Cyc_List_List* _temp2023=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp2015, _temp2021, _temp2013), _temp2019); goto _LL2024; _LL2024: yyval=(
void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp2025=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct)); _temp2025[ 0]=({
struct Cyc_StructFieldDeclList_tok_struct _temp2026; _temp2026.tag= Cyc_StructFieldDeclList_tok;
_temp2026.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Position_Segment*, struct _tuple9*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line),
_temp2023); _temp2026;}); _temp2025;}); break;}}}} case 99: _LL2012: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2028=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2028[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2029; _temp2029.tag= Cyc_QualSpecList_tok;
_temp2029.f1=({ struct _tuple18* _temp2030=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2030->f1= Cyc_Absyn_empty_tqual(); _temp2030->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2031=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2031->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2031->tl= 0; _temp2031;}); _temp2030->f3= Cyc_yyget_AttributeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2030;}); _temp2029;}); _temp2028;}); break; case 100: _LL2027: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp2033=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2033[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2034; _temp2034.tag= Cyc_QualSpecList_tok;
_temp2034.f1=({ struct _tuple18* _temp2035=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2035->f1=(* Cyc_yyget_QualSpecList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f1; _temp2035->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2036=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2036->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp2036->tl=(* Cyc_yyget_QualSpecList_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset)))).f2; _temp2036;}); _temp2035->f3=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
Cyc_yyget_AttributeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1))),(* Cyc_yyget_QualSpecList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f3); _temp2035;});
_temp2034;}); _temp2033;}); break; case 101: _LL2032: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp2038=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2038[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2039; _temp2039.tag= Cyc_QualSpecList_tok;
_temp2039.f1=({ struct _tuple18* _temp2040=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2040->f1= Cyc_yyget_TypeQual_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2040->f2=
0; _temp2040->f3= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2040;});
_temp2039;}); _temp2038;}); break; case 102: _LL2037: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp2042=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2042[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2043; _temp2043.tag= Cyc_QualSpecList_tok;
_temp2043.f1=({ struct _tuple18* _temp2044=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2044->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),(*
Cyc_yyget_QualSpecList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset)))).f1); _temp2044->f2=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f2;
_temp2044->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))),(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f3);
_temp2044;}); _temp2043;}); _temp2042;}); break; case 103: _LL2041: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp2046=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2046[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2047; _temp2047.tag= Cyc_DeclaratorExpoptList_tok;
_temp2047.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset)))); _temp2047;}); _temp2046;}); break; case 104:
_LL2045: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2049=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp2049[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp2050; _temp2050.tag=
Cyc_DeclaratorExpoptList_tok; _temp2050.f1=({ struct Cyc_List_List* _temp2051=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2051->hd=(
void*) Cyc_yyget_DeclaratorExpopt_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2051->tl= 0; _temp2051;}); _temp2050;});
_temp2049;}); break; case 105: _LL2048: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp2053=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2053[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp2054; _temp2054.tag= Cyc_DeclaratorExpoptList_tok; _temp2054.f1=({ struct
Cyc_List_List* _temp2055=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2055->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2055->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp2055;}); _temp2054;}); _temp2053;});
break; case 106: _LL2052: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2057=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2057[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2058; _temp2058.tag= Cyc_DeclaratorExpopt_tok; _temp2058.f1=({ struct
_tuple13* _temp2059=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp2059->f1= Cyc_yyget_Declarator_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))); _temp2059->f2= 0; _temp2059;}); _temp2058;});
_temp2057;}); break; case 107: _LL2056: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2061=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2061[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2062; _temp2062.tag= Cyc_DeclaratorExpopt_tok; _temp2062.f1=({ struct
_tuple13* _temp2063=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp2063->f1=({ struct Cyc_Parse_Declarator* _temp2065=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2065->id=({ struct
_tuple1* _temp2066=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2066->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct* _temp2067=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp2067[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp2068; _temp2068.tag= Cyc_Absyn_Rel_n; _temp2068.f1= 0; _temp2068;});
_temp2067;}); _temp2066->f2= _init_tag_arr(( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u); _temp2066;});
_temp2065->tms= 0; _temp2065;}); _temp2063->f2=({ struct Cyc_Core_Opt* _temp2064=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2064->v=(
void*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset))); _temp2064;}); _temp2063;}); _temp2062;}); _temp2061;});
break; case 108: _LL2060: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2070=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2070[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2071; _temp2071.tag= Cyc_DeclaratorExpopt_tok; _temp2071.f1=({ struct
_tuple13* _temp2072=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp2072->f1= Cyc_yyget_Declarator_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2072->f2=({ struct Cyc_Core_Opt*
_temp2073=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2073->v=( void*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))); _temp2073;}); _temp2072;}); _temp2071;});
_temp2070;}); break; case 109: _LL2069: { struct Cyc_List_List* _temp2075=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).last_line), Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3)))); goto
_LL2076; _LL2076: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2077=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2077[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2078; _temp2078.tag=
Cyc_TypeSpecifier_tok; _temp2078.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2079=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2079[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2080; _temp2080.tag= Cyc_Parse_Decl_spec;
_temp2080.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))),
_temp2075,({ struct Cyc_Core_Opt* _temp2081=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2081->v=( void*) Cyc_yyget_TunionFieldList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2081;}), Cyc_yyget_Bool_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 5))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2080;}); _temp2079;})); _temp2078;});
_temp2077;}); break;} case 110: _LL2074: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2083=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2083[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2084; _temp2084.tag=
Cyc_TypeSpecifier_tok; _temp2084.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2085=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2085[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2086; _temp2086.tag= Cyc_Absyn_TunionType;
_temp2086.f1=({ struct Cyc_Absyn_TunionInfo _temp2087; _temp2087.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2088=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2088[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2089; _temp2089.tag= Cyc_Absyn_UnknownTunion;
_temp2089.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2090; _temp2090.name=
Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1))); _temp2090.is_xtunion= Cyc_yyget_Bool_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2090;});
_temp2089;}); _temp2088;})); _temp2087.targs= Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2087.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2087;}); _temp2086;}); _temp2085;}),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2084;}); _temp2083;}); break; case 111:
_LL2082: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2092=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2092[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2093; _temp2093.tag=
Cyc_TypeSpecifier_tok; _temp2093.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2094=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2094[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2095; _temp2095.tag= Cyc_Absyn_TunionType;
_temp2095.f1=({ struct Cyc_Absyn_TunionInfo _temp2096; _temp2096.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2097=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2097[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2098; _temp2098.tag= Cyc_Absyn_UnknownTunion;
_temp2098.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2099; _temp2099.name=
Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1))); _temp2099.is_xtunion= Cyc_yyget_Bool_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2099;});
_temp2098;}); _temp2097;})); _temp2096.targs= Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2096.rgn=(
void*) Cyc_yyget_Rgn_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 2))); _temp2096;}); _temp2095;}); _temp2094;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2093;}); _temp2092;}); break; case 112: _LL2091: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2101=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2101[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2102; _temp2102.tag= Cyc_TypeSpecifier_tok;
_temp2102.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2103=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2103[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2104; _temp2104.tag= Cyc_Absyn_TunionFieldType; _temp2104.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2105; _temp2105.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2106=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2106[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2107; _temp2107.tag= Cyc_Absyn_UnknownTunionfield; _temp2107.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2108; _temp2108.tunion_name= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3)));
_temp2108.field_name= Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2108.is_xtunion= Cyc_yyget_Bool_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp2108;}); _temp2107;}); _temp2106;})); _temp2105.targs= Cyc_yyget_TypeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2105;}); _temp2104;}); _temp2103;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2102;}); _temp2101;}); break; case 113: _LL2100: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2110=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2110[ 0]=({ struct Cyc_Bool_tok_struct
_temp2111; _temp2111.tag= Cyc_Bool_tok; _temp2111.f1= 0; _temp2111;}); _temp2110;});
break; case 114: _LL2109: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2113=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2113[ 0]=({ struct Cyc_Bool_tok_struct _temp2114; _temp2114.tag= Cyc_Bool_tok;
_temp2114.f1= 1; _temp2114;}); _temp2113;}); break; case 115: _LL2112: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2116=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2116[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2117; _temp2117.tag= Cyc_TunionFieldList_tok;
_temp2117.f1=({ struct Cyc_List_List* _temp2118=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2118->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2118->tl= 0; _temp2118;}); _temp2117;}); _temp2116;}); break; case 116:
_LL2115: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2120=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2120[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2121; _temp2121.tag=
Cyc_TunionFieldList_tok; _temp2121.f1=({ struct Cyc_List_List* _temp2122=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2122->hd=(
void*) Cyc_yyget_TunionField_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2122->tl= 0; _temp2122;}); _temp2121;});
_temp2120;}); break; case 117: _LL2119: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2124=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2124[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2125; _temp2125.tag= Cyc_TunionFieldList_tok; _temp2125.f1=({ struct Cyc_List_List*
_temp2126=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2126->hd=( void*) Cyc_yyget_TunionField_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2126->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2126;}); _temp2125;}); _temp2124;});
break; case 118: _LL2123: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2128=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2128[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2129; _temp2129.tag= Cyc_TunionFieldList_tok; _temp2129.f1=({ struct Cyc_List_List*
_temp2130=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2130->hd=( void*) Cyc_yyget_TunionField_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2130->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2130;}); _temp2129;}); _temp2128;});
break; case 119: _LL2127: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2132=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2132[ 0]=({ struct Cyc_Scope_tok_struct _temp2133; _temp2133.tag= Cyc_Scope_tok;
_temp2133.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2133;}); _temp2132;});
break; case 120: _LL2131: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2135=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2135[ 0]=({ struct Cyc_Scope_tok_struct _temp2136; _temp2136.tag= Cyc_Scope_tok;
_temp2136.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2136;}); _temp2135;});
break; case 121: _LL2134: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2138=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2138[ 0]=({ struct Cyc_Scope_tok_struct _temp2139; _temp2139.tag= Cyc_Scope_tok;
_temp2139.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2139;}); _temp2138;});
break; case 122: _LL2137: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2141=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2141[ 0]=({ struct Cyc_TunionField_tok_struct _temp2142; _temp2142.tag= Cyc_TunionField_tok;
_temp2142.f1=({ struct Cyc_Absyn_Tunionfield* _temp2143=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2143->name= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2143->tvs= 0; _temp2143->typs= 0; _temp2143->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2143->sc=( void*) Cyc_yyget_Scope_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2143;}); _temp2142;}); _temp2141;});
break; case 123: _LL2140: { struct Cyc_List_List* _temp2145=(( struct Cyc_List_List*(*)(
struct _tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct
Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))))); goto
_LL2146; _LL2146: { struct Cyc_List_List* _temp2147=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).last_line),
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3)))); goto _LL2148; _LL2148: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2149=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2149[ 0]=({ struct Cyc_TunionField_tok_struct _temp2150; _temp2150.tag= Cyc_TunionField_tok;
_temp2150.f1=({ struct Cyc_Absyn_Tunionfield* _temp2151=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2151->name= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp2151->tvs= _temp2147; _temp2151->typs= _temp2145; _temp2151->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2151->sc=( void*) Cyc_yyget_Scope_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 5))); _temp2151;}); _temp2150;}); _temp2149;});
break;}} case 124: _LL2144: yyval=*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset)); break; case 125: _LL2152: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2154=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2154[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2155; _temp2155.tag= Cyc_Declarator_tok;
_temp2155.f1=({ struct Cyc_Parse_Declarator* _temp2156=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2156->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->id;
_temp2156->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))),( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->tms);
_temp2156;}); _temp2155;}); _temp2154;}); break; case 126: _LL2153: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2158=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2158[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2159; _temp2159.tag= Cyc_Declarator_tok;
_temp2159.f1=({ struct Cyc_Parse_Declarator* _temp2160=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2160->id= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2160->tms= 0; _temp2160;}); _temp2159;}); _temp2158;}); break; case 127:
_LL2157: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1)); break; case 128: _LL2161: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2163=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2163[ 0]=({ struct Cyc_Declarator_tok_struct _temp2164; _temp2164.tag= Cyc_Declarator_tok;
_temp2164.f1=({ struct Cyc_Parse_Declarator* _temp2165=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2165->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))))->id;
_temp2165->tms=({ struct Cyc_List_List* _temp2166=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2166->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2166->tl=( Cyc_yyget_Declarator_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))))->tms; _temp2166;}); _temp2165;});
_temp2164;}); _temp2163;}); break; case 129: _LL2162: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2168=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2168[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2169; _temp2169.tag= Cyc_Declarator_tok;
_temp2169.f1=({ struct Cyc_Parse_Declarator* _temp2170=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2170->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->id;
_temp2170->tms=({ struct Cyc_List_List* _temp2171=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2171->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2172=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2172[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2173; _temp2173.tag= Cyc_Absyn_ConstArray_mod;
_temp2173.f1= Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2173;}); _temp2172;})); _temp2171->tl=(
Cyc_yyget_Declarator_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 3))))->tms; _temp2171;}); _temp2170;}); _temp2169;});
_temp2168;}); break; case 130: _LL2167: { struct _tuple17 _temp2177; struct Cyc_List_List*
_temp2178; struct Cyc_Core_Opt* _temp2180; struct Cyc_Absyn_VarargInfo*
_temp2182; int _temp2184; struct Cyc_List_List* _temp2186; struct _tuple17*
_temp2175= Cyc_yyget_ParamDeclListBool_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2177=* _temp2175; _LL2187:
_temp2186= _temp2177.f1; goto _LL2185; _LL2185: _temp2184= _temp2177.f2; goto
_LL2183; _LL2183: _temp2182= _temp2177.f3; goto _LL2181; _LL2181: _temp2180=
_temp2177.f4; goto _LL2179; _LL2179: _temp2178= _temp2177.f5; goto _LL2176;
_LL2176: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2188=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2188[ 0]=({ struct Cyc_Declarator_tok_struct _temp2189; _temp2189.tag= Cyc_Declarator_tok;
_temp2189.f1=({ struct Cyc_Parse_Declarator* _temp2190=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2190->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->id;
_temp2190->tms=({ struct Cyc_List_List* _temp2191=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2191->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2192=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2192[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2193; _temp2193.tag= Cyc_Absyn_Function_mod;
_temp2193.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2194=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2194[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2195; _temp2195.tag= Cyc_Absyn_WithTypes;
_temp2195.f1= _temp2186; _temp2195.f2= _temp2184; _temp2195.f3= _temp2182;
_temp2195.f4= _temp2180; _temp2195.f5= _temp2178; _temp2195;}); _temp2194;}));
_temp2193;}); _temp2192;})); _temp2191->tl=( Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->tms;
_temp2191;}); _temp2190;}); _temp2189;}); _temp2188;}); break;} case 131:
_LL2174: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2197=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2197[ 0]=({ struct Cyc_Declarator_tok_struct _temp2198; _temp2198.tag= Cyc_Declarator_tok;
_temp2198.f1=({ struct Cyc_Parse_Declarator* _temp2199=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2199->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))))->id;
_temp2199->tms=({ struct Cyc_List_List* _temp2200=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2200->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2201=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2201[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2202; _temp2202.tag= Cyc_Absyn_Function_mod;
_temp2202.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2203=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2203[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2204; _temp2204.tag= Cyc_Absyn_WithTypes;
_temp2204.f1= 0; _temp2204.f2= 0; _temp2204.f3= 0; _temp2204.f4= Cyc_yyget_TypeOpt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp2204.f5= Cyc_yyget_Rgnorder_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2204;}); _temp2203;})); _temp2202;});
_temp2201;})); _temp2200->tl=( Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))))->tms;
_temp2200;}); _temp2199;}); _temp2198;}); _temp2197;}); break; case 132: _LL2196:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2206=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2206[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2207; _temp2207.tag= Cyc_Declarator_tok;
_temp2207.f1=({ struct Cyc_Parse_Declarator* _temp2208=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2208->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->id;
_temp2208->tms=({ struct Cyc_List_List* _temp2209=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2209->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2210=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2210[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2211; _temp2211.tag= Cyc_Absyn_Function_mod;
_temp2211.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2212=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2212[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2213; _temp2213.tag= Cyc_Absyn_NoTypes;
_temp2213.f1= Cyc_yyget_IdList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2213.f2= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2213;}); _temp2212;})); _temp2211;}); _temp2210;})); _temp2209->tl=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->tms;
_temp2209;}); _temp2208;}); _temp2207;}); _temp2206;}); break; case 133: _LL2205: {
struct Cyc_List_List* _temp2215=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))));
goto _LL2216; _LL2216: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2217=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2217[ 0]=({ struct Cyc_Declarator_tok_struct _temp2218; _temp2218.tag= Cyc_Declarator_tok;
_temp2218.f1=({ struct Cyc_Parse_Declarator* _temp2219=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2219->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->id;
_temp2219->tms=({ struct Cyc_List_List* _temp2220=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2220->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2221=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2221[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2222; _temp2222.tag= Cyc_Absyn_TypeParams_mod;
_temp2222.f1= _temp2215; _temp2222.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2222.f3= 0; _temp2222;}); _temp2221;})); _temp2220->tl=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->tms;
_temp2220;}); _temp2219;}); _temp2218;}); _temp2217;}); break;} case 134:
_LL2214:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2224=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))));
goto _LL2225; _LL2225: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2226=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2226[ 0]=({ struct Cyc_Declarator_tok_struct _temp2227; _temp2227.tag= Cyc_Declarator_tok;
_temp2227.f1=({ struct Cyc_Parse_Declarator* _temp2228=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2228->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->id;
_temp2228->tms=({ struct Cyc_List_List* _temp2229=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2229->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2230=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2230[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2231; _temp2231.tag= Cyc_Absyn_TypeParams_mod;
_temp2231.f1= _temp2224; _temp2231.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2231.f3= 0; _temp2231;}); _temp2230;})); _temp2229->tl=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->tms;
_temp2229;}); _temp2228;}); _temp2227;}); _temp2226;}); break;} case 135:
_LL2223: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2233=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2233[ 0]=({ struct Cyc_Declarator_tok_struct _temp2234; _temp2234.tag= Cyc_Declarator_tok;
_temp2234.f1=({ struct Cyc_Parse_Declarator* _temp2235=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2235->id=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))))->id;
_temp2235->tms=({ struct Cyc_List_List* _temp2236=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2236->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2237=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2237[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2238; _temp2238.tag= Cyc_Absyn_Attributes_mod;
_temp2238.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp2238.f2= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2238;});
_temp2237;})); _temp2236->tl=( Cyc_yyget_Declarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))))->tms;
_temp2236;}); _temp2235;}); _temp2234;}); _temp2233;}); break; case 136: _LL2232:
Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef", sizeof(
unsigned char), 46u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2240=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2240[ 0]=({ struct Cyc_Declarator_tok_struct _temp2241; _temp2241.tag= Cyc_Declarator_tok;
_temp2241.f1=({ struct Cyc_Parse_Declarator* _temp2242=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2242->id= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2242->tms= 0; _temp2242;}); _temp2241;}); _temp2240;}); break; case 137:
_LL2239: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2244=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2244[ 0]=({ struct Cyc_Declarator_tok_struct _temp2245; _temp2245.tag= Cyc_Declarator_tok;
_temp2245.f1=({ struct Cyc_Parse_Declarator* _temp2246=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2246->id= Cyc_Absyn_exn_name;
_temp2246->tms= 0; _temp2246;}); _temp2245;}); _temp2244;}); break; case 138:
_LL2243: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2248=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2248[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2249; _temp2249.tag=
Cyc_TypeModifierList_tok; _temp2249.f1=({ struct Cyc_List_List* _temp2250=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2250->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2251=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2251[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2252; _temp2252.tag= Cyc_Absyn_Pointer_mod;
_temp2252.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2252.f2=(
void*) Cyc_yyget_Rgn_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1))); _temp2252.f3= Cyc_Absyn_empty_tqual(); _temp2252;});
_temp2251;})); _temp2250->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line), Cyc_yyget_AttributeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), 0);
_temp2250;}); _temp2249;}); _temp2248;}); break; case 139: _LL2247: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2254=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2254[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2255; _temp2255.tag= Cyc_TypeModifierList_tok;
_temp2255.f1=({ struct Cyc_List_List* _temp2256=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2256->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2257=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2257[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2258; _temp2258.tag= Cyc_Absyn_Pointer_mod;
_temp2258.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2258.f2=(
void*) Cyc_yyget_Rgn_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 2))); _temp2258.f3= Cyc_yyget_TypeQual_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2258;});
_temp2257;})); _temp2256->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line),
Cyc_yyget_AttributeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1))), 0); _temp2256;}); _temp2255;}); _temp2254;});
break; case 140: _LL2253: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2260=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2260[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2261; _temp2261.tag= Cyc_TypeModifierList_tok; _temp2261.f1=({ struct Cyc_List_List*
_temp2262=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2262->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2263=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2263[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2264; _temp2264.tag=
Cyc_Absyn_Pointer_mod; _temp2264.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2264.f2=(
void*) Cyc_yyget_Rgn_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 2))); _temp2264.f3= Cyc_Absyn_empty_tqual(); _temp2264;});
_temp2263;})); _temp2262->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line),
Cyc_yyget_AttributeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1))), Cyc_yyget_TypeModifierList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))); _temp2262;});
_temp2261;}); _temp2260;}); break; case 141: _LL2259: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2266=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2266[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2267; _temp2267.tag= Cyc_TypeModifierList_tok;
_temp2267.f1=({ struct Cyc_List_List* _temp2268=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2268->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2269=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2269[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2270; _temp2270.tag= Cyc_Absyn_Pointer_mod;
_temp2270.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))); _temp2270.f2=(
void*) Cyc_yyget_Rgn_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 3))); _temp2270.f3= Cyc_yyget_TypeQual_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2270;});
_temp2269;})); _temp2268->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).last_line),
Cyc_yyget_AttributeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 2))), Cyc_yyget_TypeModifierList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))); _temp2268;});
_temp2267;}); _temp2266;}); break; case 142: _LL2265: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2272=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2272[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2273; _temp2273.tag= Cyc_Pointer_Sort_tok;
_temp2273.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2274=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2274[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2275; _temp2275.tag=
Cyc_Absyn_Nullable_ps; _temp2275.f1= Cyc_Absyn_exp_unsigned_one; _temp2275;});
_temp2274;})); _temp2273;}); _temp2272;}); break; case 143: _LL2271: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2277=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2277[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2278; _temp2278.tag= Cyc_Pointer_Sort_tok;
_temp2278.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2279=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2279[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2280; _temp2280.tag= Cyc_Absyn_NonNullable_ps; _temp2280.f1= Cyc_Absyn_exp_unsigned_one;
_temp2280;}); _temp2279;})); _temp2278;}); _temp2277;}); break; case 144:
_LL2276: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2282=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2282[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2283; _temp2283.tag=
Cyc_Pointer_Sort_tok; _temp2283.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2284=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2284[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2285; _temp2285.tag=
Cyc_Absyn_Nullable_ps; _temp2285.f1= Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2285;});
_temp2284;})); _temp2283;}); _temp2282;}); break; case 145: _LL2281: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2287=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2287[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2288; _temp2288.tag= Cyc_Pointer_Sort_tok;
_temp2288.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2289=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2289[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2290; _temp2290.tag= Cyc_Absyn_NonNullable_ps; _temp2290.f1= Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2290;}); _temp2289;})); _temp2288;}); _temp2287;}); break; case 146:
_LL2286: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2292=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2292[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2293; _temp2293.tag=
Cyc_Pointer_Sort_tok; _temp2293.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp2293;}); _temp2292;}); break; case 147: _LL2291: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp2295=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp2295[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2296; _temp2296.tag= Cyc_Rgn_tok; _temp2296.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp2296;}); _temp2295;}); break; case 148: _LL2294: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 149:
_LL2297: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp2299=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp2299[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2300; _temp2300.tag= Cyc_Rgn_tok; _temp2300.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp2300;}); _temp2299;}); break; case 150: _LL2298:
if( Cyc_yyget_Kind_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind\n",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2302=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2302[ 0]=({ struct Cyc_Rgn_tok_struct _temp2303; _temp2303.tag= Cyc_Rgn_tok;
_temp2303.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2303;}); _temp2302;}); break; case 151: _LL2301: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp2305=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp2305[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2306; _temp2306.tag= Cyc_Rgn_tok; _temp2306.f1=( void*) Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2307=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2307->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2307;}),
0); _temp2306;}); _temp2305;}); break; case 152: _LL2304: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 153:
_LL2308: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp2310=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp2310[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2311; _temp2311.tag= Cyc_TypeQual_tok; _temp2311.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1))), Cyc_yyget_TypeQual_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset)))); _temp2311;}); _temp2310;}); break; case
154: _LL2309: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2313=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2313[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2314; _temp2314.tag= Cyc_ParamDeclListBool_tok; _temp2314.f1=({ struct
_tuple17* _temp2315=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2315->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 2)))); _temp2315->f2= 0; _temp2315->f3= 0; _temp2315->f4=
Cyc_yyget_TypeOpt_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1))); _temp2315->f5= Cyc_yyget_Rgnorder_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2315;});
_temp2314;}); _temp2313;}); break; case 155: _LL2312: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp2317=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2317[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2318; _temp2318.tag= Cyc_ParamDeclListBool_tok;
_temp2318.f1=({ struct _tuple17* _temp2319=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2319->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)))); _temp2319->f2=
1; _temp2319->f3= 0; _temp2319->f4= Cyc_yyget_TypeOpt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2319->f5=
Cyc_yyget_Rgnorder_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp2319;}); _temp2318;}); _temp2317;}); break; case 156:
_LL2316: { void* _temp2323; struct Cyc_Absyn_Tqual _temp2325; struct Cyc_Core_Opt*
_temp2327; struct _tuple2 _temp2321=* Cyc_yyget_ParamDecl_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _LL2328:
_temp2327= _temp2321.f1; goto _LL2326; _LL2326: _temp2325= _temp2321.f2; goto
_LL2324; _LL2324: _temp2323= _temp2321.f3; goto _LL2322; _LL2322: { struct Cyc_Absyn_VarargInfo*
_temp2330=({ struct Cyc_Absyn_VarargInfo* _temp2329=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2329->name= _temp2327;
_temp2329->tq= _temp2325; _temp2329->type=( void*) _temp2323; _temp2329->rgn=(
void*) Cyc_yyget_Rgn_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 4))); _temp2329->inject= Cyc_yyget_Bool_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2329;});
goto _LL2331; _LL2331: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2332=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2332[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2333; _temp2333.tag= Cyc_ParamDeclListBool_tok; _temp2333.f1=({ struct
_tuple17* _temp2334=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2334->f1= 0; _temp2334->f2= 0; _temp2334->f3= _temp2330; _temp2334->f4= Cyc_yyget_TypeOpt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2334->f5= Cyc_yyget_Rgnorder_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2334;}); _temp2333;}); _temp2332;});
break;}} case 157: _LL2320: { void* _temp2338; struct Cyc_Absyn_Tqual _temp2340;
struct Cyc_Core_Opt* _temp2342; struct _tuple2 _temp2336=* Cyc_yyget_ParamDecl_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_LL2343: _temp2342= _temp2336.f1; goto _LL2341; _LL2341: _temp2340= _temp2336.f2;
goto _LL2339; _LL2339: _temp2338= _temp2336.f3; goto _LL2337; _LL2337: { struct
Cyc_Absyn_VarargInfo* _temp2345=({ struct Cyc_Absyn_VarargInfo* _temp2344=(
struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2344->name= _temp2342; _temp2344->tq= _temp2340; _temp2344->type=( void*)
_temp2338; _temp2344->rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))); _temp2344->inject=
Cyc_yyget_Bool_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3))); _temp2344;}); goto _LL2346; _LL2346: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp2347=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2347[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2348; _temp2348.tag= Cyc_ParamDeclListBool_tok;
_temp2348.f1=({ struct _tuple17* _temp2349=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2349->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 7)))); _temp2349->f2=
0; _temp2349->f3= _temp2345; _temp2349->f4= Cyc_yyget_TypeOpt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2349->f5=
Cyc_yyget_Rgnorder_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp2349;}); _temp2348;}); _temp2347;}); break;}} case 158:
_LL2335: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct* _temp2351=( struct Cyc_TypeOpt_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2351[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2352; _temp2352.tag= Cyc_TypeOpt_tok; _temp2352.f1= 0; _temp2352;});
_temp2351;}); break; case 159: _LL2350: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2354=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2354[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2355; _temp2355.tag= Cyc_TypeOpt_tok;
_temp2355.f1=({ struct Cyc_Core_Opt* _temp2356=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2356->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2357=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2357[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2358; _temp2358.tag= Cyc_Absyn_JoinEff;
_temp2358.f1= Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2358;}); _temp2357;})); _temp2356;});
_temp2355;}); _temp2354;}); break; case 160: _LL2353: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2360=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2360[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2361; _temp2361.tag= Cyc_Rgnorder_tok; _temp2361.f1= 0; _temp2361;});
_temp2360;}); break; case 161: _LL2359: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 162:
_LL2362: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2364=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2364[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2365; _temp2365.tag= Cyc_Rgnorder_tok; _temp2365.f1=({ struct Cyc_List_List*
_temp2366=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2366->hd=( void*)({ struct _tuple19* _temp2367=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2367->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2367->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2367;});
_temp2366->tl= 0; _temp2366;}); _temp2365;}); _temp2364;}); break; case 163:
_LL2363: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2369=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2369[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2370; _temp2370.tag= Cyc_Rgnorder_tok; _temp2370.f1=({ struct Cyc_List_List*
_temp2371=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2371->hd=( void*)({ struct _tuple19* _temp2372=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2372->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2372->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2372;}); _temp2371->tl= Cyc_yyget_Rgnorder_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2371;});
_temp2370;}); _temp2369;}); break; case 164: _LL2368: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2374=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2374[ 0]=({ struct Cyc_Bool_tok_struct
_temp2375; _temp2375.tag= Cyc_Bool_tok; _temp2375.f1= 0; _temp2375;}); _temp2374;});
break; case 165: _LL2373: { struct _tagged_arr _temp2377= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); goto
_LL2378; _LL2378: if( Cyc_String_zstrcmp( _temp2377, _tag_arr("inject", sizeof(
unsigned char), 7u)) != 0){ Cyc_Parse_err( _tag_arr("missing type in function declaration",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2379=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2379[ 0]=({ struct Cyc_Bool_tok_struct _temp2380; _temp2380.tag= Cyc_Bool_tok;
_temp2380.f1= 1; _temp2380;}); _temp2379;}); break;} case 166: _LL2376: yyval=*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break;
case 167: _LL2381: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2383=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2383[ 0]=({ struct Cyc_TypeList_tok_struct _temp2384; _temp2384.tag= Cyc_TypeList_tok;
_temp2384.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_TypeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))));
_temp2384;}); _temp2383;}); break; case 168: _LL2382: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp2386=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp2386[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2387; _temp2387.tag= Cyc_TypeList_tok; _temp2387.f1= 0; _temp2387;});
_temp2386;}); break; case 169: _LL2385: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)); break; case
170: _LL2388: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2390=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2390[ 0]=({ struct Cyc_TypeList_tok_struct _temp2391; _temp2391.tag= Cyc_TypeList_tok;
_temp2391.f1=({ struct Cyc_List_List* _temp2392=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2392->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_EffKind)); _temp2392->tl= 0; _temp2392;}); _temp2391;});
_temp2390;}); break; case 171: _LL2389: if( Cyc_yyget_Kind_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))) !=( void*) Cyc_Absyn_EffKind){
Cyc_Parse_err( _tag_arr("expecing effect kind (E)", sizeof( unsigned char), 25u),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2394=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2394[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2395; _temp2395.tag= Cyc_TypeList_tok; _temp2395.f1=({ struct Cyc_List_List*
_temp2396=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2396->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp2396->tl= 0; _temp2396;}); _temp2395;}); _temp2394;}); break; case 172:
_LL2393: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2398=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2398[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2399; _temp2399.tag= Cyc_TypeList_tok; _temp2399.f1=({ struct Cyc_List_List*
_temp2400=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2400->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2401=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2401[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2402; _temp2402.tag= Cyc_Absyn_AccessEff;
_temp2402.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2402;});
_temp2401;})); _temp2400->tl= 0; _temp2400;}); _temp2399;}); _temp2398;});
break; case 173: _LL2397: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2404=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2404[ 0]=({ struct Cyc_TypeList_tok_struct _temp2405; _temp2405.tag= Cyc_TypeList_tok;
_temp2405.f1=({ struct Cyc_List_List* _temp2406=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2406->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2407=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2407[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2408; _temp2408.tag= Cyc_Absyn_AccessEff;
_temp2408.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2408;}); _temp2407;})); _temp2406->tl= Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2406;});
_temp2405;}); _temp2404;}); break; case 174: _LL2403: if( Cyc_yyget_Kind_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))) !=( void*)
Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof(
unsigned char), 26u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2410=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2410[ 0]=({ struct Cyc_TypeList_tok_struct _temp2411; _temp2411.tag= Cyc_TypeList_tok;
_temp2411.f1=({ struct Cyc_List_List* _temp2412=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2412->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2413=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2413[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2414; _temp2414.tag= Cyc_Absyn_AccessEff;
_temp2414.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2414;}); _temp2413;})); _temp2412->tl= 0; _temp2412;}); _temp2411;});
_temp2410;}); break; case 175: _LL2409: if( Cyc_yyget_Kind_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))) !=( void*)
Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof(
unsigned char), 26u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2416=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2416[ 0]=({ struct Cyc_TypeList_tok_struct _temp2417; _temp2417.tag= Cyc_TypeList_tok;
_temp2417.f1=({ struct Cyc_List_List* _temp2418=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2418->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2419=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2419[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2420; _temp2420.tag= Cyc_Absyn_AccessEff;
_temp2420.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2420;}); _temp2419;})); _temp2418->tl= Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2418;});
_temp2417;}); _temp2416;}); break; case 176: _LL2415: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2422=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2422[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2423; _temp2423.tag= Cyc_ParamDeclList_tok;
_temp2423.f1=({ struct Cyc_List_List* _temp2424=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2424->hd=( void*) Cyc_yyget_ParamDecl_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2424->tl= 0; _temp2424;}); _temp2423;}); _temp2422;}); break; case 177:
_LL2421: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp2426=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp2426[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp2427; _temp2427.tag=
Cyc_ParamDeclList_tok; _temp2427.f1=({ struct Cyc_List_List* _temp2428=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2428->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp2428->tl= Cyc_yyget_ParamDeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2428;});
_temp2427;}); _temp2426;}); break; case 178: _LL2425: { void* _temp2430= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1)))).f2, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); goto _LL2431; _LL2431: { struct Cyc_List_List*
_temp2432=(* Cyc_yyget_QualSpecList_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1)))).f3; goto _LL2433; _LL2433: { struct
Cyc_Absyn_Tqual _temp2434=(* Cyc_yyget_QualSpecList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))).f1; goto
_LL2435; _LL2435: { struct Cyc_List_List* _temp2436=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->tms;
goto _LL2437; _LL2437: { struct _tuple6 _temp2438= Cyc_Parse_apply_tms(
_temp2434, _temp2430, _temp2432, _temp2436); goto _LL2439; _LL2439: if(
_temp2438.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));}{ struct _tuple1* _temp2440=( Cyc_yyget_Declarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->id;
goto _LL2441; _LL2441:{ void* _temp2442=(* _temp2440).f1; struct Cyc_List_List*
_temp2452; struct Cyc_List_List* _temp2454; _LL2444: if( _temp2442 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL2445;} else{ goto _LL2446;} _LL2446: if(( unsigned int)
_temp2442 > 1u?*(( int*) _temp2442) == Cyc_Absyn_Rel_n: 0){ _LL2453: _temp2452=((
struct Cyc_Absyn_Rel_n_struct*) _temp2442)->f1; if( _temp2452 == 0){ goto
_LL2447;} else{ goto _LL2448;}} else{ goto _LL2448;} _LL2448: if(( unsigned int)
_temp2442 > 1u?*(( int*) _temp2442) == Cyc_Absyn_Abs_n: 0){ _LL2455: _temp2454=((
struct Cyc_Absyn_Abs_n_struct*) _temp2442)->f1; if( _temp2454 == 0){ goto
_LL2449;} else{ goto _LL2450;}} else{ goto _LL2450;} _LL2450: goto _LL2451;
_LL2445: goto _LL2443; _LL2447: goto _LL2443; _LL2449: goto _LL2443; _LL2451:
Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); goto _LL2443; _LL2443:;}{ struct Cyc_Core_Opt*
_temp2457=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2456=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2456->v=( void*)(* _temp2440).f2;
_temp2456;}); goto _LL2458; _LL2458: if( _temp2438.f4 != 0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} if( _temp2438.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2459=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2459[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2460; _temp2460.tag= Cyc_ParamDecl_tok;
_temp2460.f1=({ struct _tuple2* _temp2461=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp2461->f1= _temp2457; _temp2461->f2= _temp2438.f1;
_temp2461->f3= _temp2438.f2; _temp2461;}); _temp2460;}); _temp2459;}); break;}}}}}}}
case 179: _LL2429: { void* _temp2463= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); goto _LL2464;
_LL2464: { struct Cyc_List_List* _temp2465=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f3;
goto _LL2466; _LL2466: { struct Cyc_Absyn_Tqual _temp2467=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f1;
goto _LL2468; _LL2468: if( _temp2465 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2469=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2469[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2470; _temp2470.tag= Cyc_ParamDecl_tok;
_temp2470.f1=({ struct _tuple2* _temp2471=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp2471->f1= 0; _temp2471->f2= _temp2467; _temp2471->f3=
_temp2463; _temp2471;}); _temp2470;}); _temp2469;}); break;}}} case 180: _LL2462: {
void* _temp2473= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line));
goto _LL2474; _LL2474: { struct Cyc_List_List* _temp2475=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))).f3;
goto _LL2476; _LL2476: { struct Cyc_Absyn_Tqual _temp2477=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))).f1;
goto _LL2478; _LL2478: { struct Cyc_List_List* _temp2479=( Cyc_yyget_AbstractDeclarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->tms;
goto _LL2480; _LL2480: { struct _tuple6 _temp2481= Cyc_Parse_apply_tms(
_temp2477, _temp2473, _temp2475, _temp2479); goto _LL2482; _LL2482: if(
_temp2481.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} if( _temp2481.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2483=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2483[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2484; _temp2484.tag= Cyc_ParamDecl_tok;
_temp2484.f1=({ struct _tuple2* _temp2485=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp2485->f1= 0; _temp2485->f2= _temp2481.f1; _temp2485->f3=
_temp2481.f2; _temp2485;}); _temp2484;}); _temp2483;}); break;}}}}} case 181:
_LL2472: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp2487=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2487[ 0]=({ struct Cyc_IdList_tok_struct
_temp2488; _temp2488.tag= Cyc_IdList_tok; _temp2488.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))); _temp2488;});
_temp2487;}); break; case 182: _LL2486: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2490=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2490[ 0]=({ struct Cyc_IdList_tok_struct _temp2491; _temp2491.tag= Cyc_IdList_tok;
_temp2491.f1=({ struct Cyc_List_List* _temp2492=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2492->hd=( void*)({ struct
_tagged_arr* _temp2493=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2493[ 0]= Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2493;});
_temp2492->tl= 0; _temp2492;}); _temp2491;}); _temp2490;}); break; case 183:
_LL2489: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp2495=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2495[ 0]=({ struct Cyc_IdList_tok_struct
_temp2496; _temp2496.tag= Cyc_IdList_tok; _temp2496.f1=({ struct Cyc_List_List*
_temp2497=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2497->hd=( void*)({ struct _tagged_arr* _temp2498=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2498[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2498;}); _temp2497->tl= Cyc_yyget_IdList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2497;});
_temp2496;}); _temp2495;}); break; case 184: _LL2494: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 185:
_LL2499: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 186: _LL2500: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2502=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2502[ 0]=({ struct Cyc_Exp_tok_struct _temp2503; _temp2503.tag= Cyc_Exp_tok;
_temp2503.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2504=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2504[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2505; _temp2505.tag= Cyc_Absyn_UnresolvedMem_e; _temp2505.f1= 0; _temp2505.f2=
0; _temp2505;}); _temp2504;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2503;}); _temp2502;}); break; case 187:
_LL2501: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2507=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2507[ 0]=({ struct Cyc_Exp_tok_struct
_temp2508; _temp2508.tag= Cyc_Exp_tok; _temp2508.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2509=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2509[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2510; _temp2510.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2510.f1= 0; _temp2510.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2510;});
_temp2509;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2508;}); _temp2507;}); break; case 188:
_LL2506: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2512=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2512[ 0]=({ struct Cyc_Exp_tok_struct
_temp2513; _temp2513.tag= Cyc_Exp_tok; _temp2513.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2514=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2514[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2515; _temp2515.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2515.f1= 0; _temp2515.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)))); _temp2515;});
_temp2514;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2513;}); _temp2512;}); break; case 189:
_LL2511: { struct Cyc_Absyn_Vardecl* _temp2519= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2517=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2517->f1=( void*) Cyc_Absyn_Loc_n; _temp2517->f2=({ struct _tagged_arr*
_temp2518=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp2518[ 0]= Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 5))); _temp2518;}); _temp2517;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).last_line)));
goto _LL2520; _LL2520: _temp2519->tq=({ struct Cyc_Absyn_Tqual _temp2521;
_temp2521.q_const= 1; _temp2521.q_volatile= 0; _temp2521.q_restrict= 1;
_temp2521;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2522=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2522[ 0]=({ struct Cyc_Exp_tok_struct
_temp2523; _temp2523.tag= Cyc_Exp_tok; _temp2523.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2524=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2524[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2525; _temp2525.tag= Cyc_Absyn_Comprehension_e;
_temp2525.f1= _temp2519; _temp2525.f2= Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2525.f3=
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1))); _temp2525;}); _temp2524;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2523;}); _temp2522;}); break;} case 190: _LL2516: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp2527=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2527[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2528; _temp2528.tag= Cyc_InitializerList_tok;
_temp2528.f1=({ struct Cyc_List_List* _temp2529=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2529->hd=( void*)({ struct
_tuple20* _temp2530=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2530->f1= 0; _temp2530->f2= Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2530;});
_temp2529->tl= 0; _temp2529;}); _temp2528;}); _temp2527;}); break; case 191:
_LL2526: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2532=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2532[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2533; _temp2533.tag=
Cyc_InitializerList_tok; _temp2533.f1=({ struct Cyc_List_List* _temp2534=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2534->hd=(
void*)({ struct _tuple20* _temp2535=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2535->f1= Cyc_yyget_DesignatorList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2535->f2=
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp2535;}); _temp2534->tl= 0; _temp2534;}); _temp2533;});
_temp2532;}); break; case 192: _LL2531: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp2537=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp2537[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp2538; _temp2538.tag= Cyc_InitializerList_tok; _temp2538.f1=({ struct Cyc_List_List*
_temp2539=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2539->hd=( void*)({ struct _tuple20* _temp2540=( struct _tuple20*)
GC_malloc( sizeof( struct _tuple20)); _temp2540->f1= 0; _temp2540->f2= Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2540;}); _temp2539->tl= Cyc_yyget_InitializerList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2539;});
_temp2538;}); _temp2537;}); break; case 193: _LL2536: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp2542=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2542[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2543; _temp2543.tag= Cyc_InitializerList_tok;
_temp2543.f1=({ struct Cyc_List_List* _temp2544=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2544->hd=( void*)({ struct
_tuple20* _temp2545=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2545->f1= Cyc_yyget_DesignatorList_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2545->f2= Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2545;}); _temp2544->tl= Cyc_yyget_InitializerList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2544;});
_temp2543;}); _temp2542;}); break; case 194: _LL2541: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2547=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2547[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2548; _temp2548.tag= Cyc_DesignatorList_tok;
_temp2548.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1)))); _temp2548;}); _temp2547;}); break; case 195:
_LL2546: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp2550=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2550[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2551; _temp2551.tag=
Cyc_DesignatorList_tok; _temp2551.f1=({ struct Cyc_List_List* _temp2552=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2552->hd=( void*)
Cyc_yyget_Designator_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset))); _temp2552->tl= 0; _temp2552;}); _temp2551;}); _temp2550;});
break; case 196: _LL2549: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2554=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2554[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2555; _temp2555.tag=
Cyc_DesignatorList_tok; _temp2555.f1=({ struct Cyc_List_List* _temp2556=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2556->hd=( void*)
Cyc_yyget_Designator_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset))); _temp2556->tl= Cyc_yyget_DesignatorList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2556;});
_temp2555;}); _temp2554;}); break; case 197: _LL2553: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2558=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2558[ 0]=({ struct
Cyc_Designator_tok_struct _temp2559; _temp2559.tag= Cyc_Designator_tok;
_temp2559.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2560=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2560[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2561; _temp2561.tag=
Cyc_Absyn_ArrayElement; _temp2561.f1= Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2561;});
_temp2560;})); _temp2559;}); _temp2558;}); break; case 198: _LL2557: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2563=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2563[ 0]=({ struct
Cyc_Designator_tok_struct _temp2564; _temp2564.tag= Cyc_Designator_tok;
_temp2564.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2565=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2565[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2566; _temp2566.tag= Cyc_Absyn_FieldName;
_temp2566.f1=({ struct _tagged_arr* _temp2567=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2567[ 0]= Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2567;});
_temp2566;}); _temp2565;})); _temp2564;}); _temp2563;}); break; case 199:
_LL2562: { void* _temp2569= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); goto _LL2570;
_LL2570: { struct Cyc_List_List* _temp2571=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f3;
goto _LL2572; _LL2572: if( _temp2571 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));}{ struct Cyc_Absyn_Tqual _temp2573=(* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f1;
goto _LL2574; _LL2574: yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2575=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2575[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2576; _temp2576.tag= Cyc_ParamDecl_tok;
_temp2576.f1=({ struct _tuple2* _temp2577=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp2577[ 0]=({ struct _tuple2 _temp2578; _temp2578.f1= 0;
_temp2578.f2= _temp2573; _temp2578.f3= _temp2569; _temp2578;}); _temp2577;});
_temp2576;}); _temp2575;}); break;}}} case 200: _LL2568: { void* _temp2580= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1)))).f2, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); goto _LL2581; _LL2581: { struct Cyc_List_List*
_temp2582=(* Cyc_yyget_QualSpecList_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1)))).f3; goto _LL2583; _LL2583: { struct
Cyc_Absyn_Tqual _temp2584=(* Cyc_yyget_QualSpecList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))).f1; goto
_LL2585; _LL2585: { struct Cyc_List_List* _temp2586=( Cyc_yyget_AbstractDeclarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->tms;
goto _LL2587; _LL2587: { struct _tuple6 _temp2588= Cyc_Parse_apply_tms(
_temp2584, _temp2580, _temp2582, _temp2586); goto _LL2589; _LL2589: if(
_temp2588.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} if( _temp2588.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2590=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2590[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2591; _temp2591.tag= Cyc_ParamDecl_tok;
_temp2591.f1=({ struct _tuple2* _temp2592=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp2592->f1= 0; _temp2592->f2= _temp2588.f1; _temp2592->f3=
_temp2588.f2; _temp2592;}); _temp2591;}); _temp2590;}); break;}}}}} case 201:
_LL2579: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2594=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2594[ 0]=({ struct Cyc_Type_tok_struct
_temp2595; _temp2595.tag= Cyc_Type_tok; _temp2595.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f3;
_temp2595;}); _temp2594;}); break; case 202: _LL2593: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2597=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2597[ 0]=({ struct Cyc_Type_tok_struct
_temp2598; _temp2598.tag= Cyc_Type_tok; _temp2598.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp2599=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2599[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2600; _temp2600.tag= Cyc_Absyn_JoinEff; _temp2600.f1=
0; _temp2600;}); _temp2599;})); _temp2598;}); _temp2597;}); break; case 203:
_LL2596: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2602=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2602[ 0]=({ struct Cyc_Type_tok_struct
_temp2603; _temp2603.tag= Cyc_Type_tok; _temp2603.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp2604=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2604[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2605; _temp2605.tag= Cyc_Absyn_JoinEff; _temp2605.f1=
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1))); _temp2605;}); _temp2604;})); _temp2603;}); _temp2602;});
break; case 204: _LL2601: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2607=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2607[ 0]=({ struct Cyc_Type_tok_struct _temp2608; _temp2608.tag= Cyc_Type_tok;
_temp2608.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2609=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2609[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2610; _temp2610.tag= Cyc_Absyn_JoinEff;
_temp2610.f1=({ struct Cyc_List_List* _temp2611=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2611->hd=( void*) Cyc_yyget_Type_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp2611->tl= Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2611;}); _temp2610;}); _temp2609;}));
_temp2608;}); _temp2607;}); break; case 205: _LL2606: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp2613=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp2613[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2614; _temp2614.tag= Cyc_TypeList_tok; _temp2614.f1=({ struct Cyc_List_List*
_temp2615=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2615->hd=( void*) Cyc_yyget_Type_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))); _temp2615->tl= 0; _temp2615;}); _temp2614;});
_temp2613;}); break; case 206: _LL2612: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2617=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2617[ 0]=({ struct Cyc_TypeList_tok_struct _temp2618; _temp2618.tag= Cyc_TypeList_tok;
_temp2618.f1=({ struct Cyc_List_List* _temp2619=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2619->hd=( void*) Cyc_yyget_Type_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2619->tl= Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp2619;}); _temp2618;}); _temp2617;});
break; case 207: _LL2616: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2621=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2621[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2622; _temp2622.tag= Cyc_AbstractDeclarator_tok; _temp2622.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2623=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2623->tms= Cyc_yyget_TypeModifierList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2623;});
_temp2622;}); _temp2621;}); break; case 208: _LL2620: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 209:
_LL2624: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2626=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2626[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2627; _temp2627.tag=
Cyc_AbstractDeclarator_tok; _temp2627.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2628=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2628->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))),( Cyc_yyget_AbstractDeclarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))))->tms);
_temp2628;}); _temp2627;}); _temp2626;}); break; case 210: _LL2625: yyval=*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1));
break; case 211: _LL2629: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2631=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2631[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2632; _temp2632.tag= Cyc_AbstractDeclarator_tok; _temp2632.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2633=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2633->tms=({ struct Cyc_List_List* _temp2634=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2634->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2634->tl= 0; _temp2634;}); _temp2633;}); _temp2632;}); _temp2631;}); break;
case 212: _LL2630: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2636=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2636[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2637; _temp2637.tag= Cyc_AbstractDeclarator_tok; _temp2637.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2638=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2638->tms=({ struct Cyc_List_List* _temp2639=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2639->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2639->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))))->tms;
_temp2639;}); _temp2638;}); _temp2637;}); _temp2636;}); break; case 213: _LL2635:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2641=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2641[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2642; _temp2642.tag=
Cyc_AbstractDeclarator_tok; _temp2642.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2643=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2643->tms=({ struct Cyc_List_List* _temp2644=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2644->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2645=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2645[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2646; _temp2646.tag= Cyc_Absyn_ConstArray_mod;
_temp2646.f1= Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2646;}); _temp2645;})); _temp2644->tl=
0; _temp2644;}); _temp2643;}); _temp2642;}); _temp2641;}); break; case 214:
_LL2640: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2648=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2648[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2649; _temp2649.tag=
Cyc_AbstractDeclarator_tok; _temp2649.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2650=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2650->tms=({ struct Cyc_List_List* _temp2651=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2651->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2652=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2652[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2653; _temp2653.tag= Cyc_Absyn_ConstArray_mod;
_temp2653.f1= Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2653;}); _temp2652;})); _temp2651->tl=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 3))))->tms; _temp2651;}); _temp2650;}); _temp2649;});
_temp2648;}); break; case 215: _LL2647: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2655=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2655[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2656; _temp2656.tag= Cyc_AbstractDeclarator_tok; _temp2656.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2657=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2657->tms=({ struct Cyc_List_List* _temp2658=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2658->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2659=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2659[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2660; _temp2660.tag= Cyc_Absyn_Function_mod;
_temp2660.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2661=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2661[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2662; _temp2662.tag= Cyc_Absyn_WithTypes;
_temp2662.f1= 0; _temp2662.f2= 0; _temp2662.f3= 0; _temp2662.f4= Cyc_yyget_TypeOpt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp2662.f5= Cyc_yyget_Rgnorder_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2662;}); _temp2661;})); _temp2660;});
_temp2659;})); _temp2658->tl= 0; _temp2658;}); _temp2657;}); _temp2656;});
_temp2655;}); break; case 216: _LL2654: { struct _tuple17 _temp2666; struct Cyc_List_List*
_temp2667; struct Cyc_Core_Opt* _temp2669; struct Cyc_Absyn_VarargInfo*
_temp2671; int _temp2673; struct Cyc_List_List* _temp2675; struct _tuple17*
_temp2664= Cyc_yyget_ParamDeclListBool_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2666=* _temp2664; _LL2676:
_temp2675= _temp2666.f1; goto _LL2674; _LL2674: _temp2673= _temp2666.f2; goto
_LL2672; _LL2672: _temp2671= _temp2666.f3; goto _LL2670; _LL2670: _temp2669=
_temp2666.f4; goto _LL2668; _LL2668: _temp2667= _temp2666.f5; goto _LL2665;
_LL2665: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2677=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2677[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2678; _temp2678.tag=
Cyc_AbstractDeclarator_tok; _temp2678.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2679=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2679->tms=({ struct Cyc_List_List* _temp2680=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2680->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2681=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2681[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2682; _temp2682.tag= Cyc_Absyn_Function_mod;
_temp2682.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2683=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2683[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2684; _temp2684.tag= Cyc_Absyn_WithTypes;
_temp2684.f1= _temp2675; _temp2684.f2= _temp2673; _temp2684.f3= _temp2671;
_temp2684.f4= _temp2669; _temp2684.f5= _temp2667; _temp2684;}); _temp2683;}));
_temp2682;}); _temp2681;})); _temp2680->tl= 0; _temp2680;}); _temp2679;});
_temp2678;}); _temp2677;}); break;} case 217: _LL2663: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2686=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2686[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2687; _temp2687.tag= Cyc_AbstractDeclarator_tok;
_temp2687.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2688=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2688->tms=({
struct Cyc_List_List* _temp2689=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2689->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2690=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2690[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2691; _temp2691.tag=
Cyc_Absyn_Function_mod; _temp2691.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2692=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2692[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2693; _temp2693.tag= Cyc_Absyn_WithTypes;
_temp2693.f1= 0; _temp2693.f2= 0; _temp2693.f3= 0; _temp2693.f4= Cyc_yyget_TypeOpt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp2693.f5= Cyc_yyget_Rgnorder_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp2693;}); _temp2692;})); _temp2691;});
_temp2690;})); _temp2689->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))))->tms;
_temp2689;}); _temp2688;}); _temp2687;}); _temp2686;}); break; case 218: _LL2685: {
struct _tuple17 _temp2697; struct Cyc_List_List* _temp2698; struct Cyc_Core_Opt*
_temp2700; struct Cyc_Absyn_VarargInfo* _temp2702; int _temp2704; struct Cyc_List_List*
_temp2706; struct _tuple17* _temp2695= Cyc_yyget_ParamDeclListBool_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2697=*
_temp2695; _LL2707: _temp2706= _temp2697.f1; goto _LL2705; _LL2705: _temp2704=
_temp2697.f2; goto _LL2703; _LL2703: _temp2702= _temp2697.f3; goto _LL2701;
_LL2701: _temp2700= _temp2697.f4; goto _LL2699; _LL2699: _temp2698= _temp2697.f5;
goto _LL2696; _LL2696: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2708=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2708[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2709; _temp2709.tag= Cyc_AbstractDeclarator_tok; _temp2709.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2710=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2710->tms=({ struct Cyc_List_List* _temp2711=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2711->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2712=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2712[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2713; _temp2713.tag= Cyc_Absyn_Function_mod;
_temp2713.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2714=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2714[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2715; _temp2715.tag= Cyc_Absyn_WithTypes;
_temp2715.f1= _temp2706; _temp2715.f2= _temp2704; _temp2715.f3= _temp2702;
_temp2715.f4= _temp2700; _temp2715.f5= _temp2698; _temp2715;}); _temp2714;}));
_temp2713;}); _temp2712;})); _temp2711->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->tms;
_temp2711;}); _temp2710;}); _temp2709;}); _temp2708;}); break;} case 219:
_LL2694: { struct Cyc_List_List* _temp2717=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))));
goto _LL2718; _LL2718: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2719=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2719[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2720; _temp2720.tag= Cyc_AbstractDeclarator_tok; _temp2720.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2721=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2721->tms=({ struct Cyc_List_List* _temp2722=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2722->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2723=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2723[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2724; _temp2724.tag= Cyc_Absyn_TypeParams_mod;
_temp2724.f1= _temp2717; _temp2724.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2724.f3= 0; _temp2724;}); _temp2723;})); _temp2722->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->tms;
_temp2722;}); _temp2721;}); _temp2720;}); _temp2719;}); break;} case 220:
_LL2716:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2726=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))));
goto _LL2727; _LL2727: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2728=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2728[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2729; _temp2729.tag= Cyc_AbstractDeclarator_tok; _temp2729.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2730=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2730->tms=({ struct Cyc_List_List* _temp2731=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2731->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2732=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2732[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2733; _temp2733.tag= Cyc_Absyn_TypeParams_mod;
_temp2733.f1= _temp2726; _temp2733.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2733.f3= 0; _temp2733;}); _temp2732;})); _temp2731->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))))->tms;
_temp2731;}); _temp2730;}); _temp2729;}); _temp2728;}); break;} case 221:
_LL2725: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2735=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2735[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2736; _temp2736.tag=
Cyc_AbstractDeclarator_tok; _temp2736.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2737=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2737->tms=({ struct Cyc_List_List* _temp2738=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2738->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2739=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2739[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2740; _temp2740.tag= Cyc_Absyn_Attributes_mod;
_temp2740.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp2740.f2= Cyc_yyget_AttributeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2740;});
_temp2739;})); _temp2738->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))))->tms;
_temp2738;}); _temp2737;}); _temp2736;}); _temp2735;}); break; case 222: _LL2734:
yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset));
break; case 223: _LL2741: yyval=*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset)); break; case 224: _LL2742: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 225:
_LL2743: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 226: _LL2744: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 227:
_LL2745: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 228: _LL2746: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))),
_tag_arr("`H", sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));}{ struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar*
_temp2758=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp2758->name=({ struct _tagged_arr* _temp2759=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp2759[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3)));
_temp2759;}); _temp2758->identity= 0; _temp2758->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp2758;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2756=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2756[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2757; _temp2757.tag= Cyc_Absyn_VarType; _temp2757.f1=
tv; _temp2757;}); _temp2756;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2748=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2748[ 0]=({ struct Cyc_Stmt_tok_struct _temp2749; _temp2749.tag= Cyc_Stmt_tok;
_temp2749.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2750=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2750[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2751; _temp2751.tag= Cyc_Absyn_Region_s;
_temp2751.f1= tv; _temp2751.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2752=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2752->f1=(
void*) Cyc_Absyn_Loc_n; _temp2752->f2=({ struct _tagged_arr* _temp2753=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2753[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2753;}); _temp2752;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2754=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2754[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2755; _temp2755.tag= Cyc_Absyn_RgnHandleType; _temp2755.f1=( void*) t;
_temp2755;}); _temp2754;}), 0); _temp2751.f3= Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2751;});
_temp2750;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2749;}); _temp2748;}); break;} case 229:
_LL2747: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));}{ struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar*
_temp2771=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp2771->name=({ struct _tagged_arr* _temp2772=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2772[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp2773= Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); xprintf("`%.*s",
_get_arr_size( _temp2773, 1u), _temp2773.curr);}); _temp2772;}); _temp2771->identity=
0; _temp2771->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp2771;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2769=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2769[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2770; _temp2770.tag= Cyc_Absyn_VarType;
_temp2770.f1= tv; _temp2770;}); _temp2769;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2761=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2761[ 0]=({ struct Cyc_Stmt_tok_struct _temp2762; _temp2762.tag= Cyc_Stmt_tok;
_temp2762.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2763=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2763[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2764; _temp2764.tag= Cyc_Absyn_Region_s;
_temp2764.f1= tv; _temp2764.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2765=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2765->f1=(
void*) Cyc_Absyn_Loc_n; _temp2765->f2=({ struct _tagged_arr* _temp2766=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2766[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2766;}); _temp2765;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2767=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2767[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2768; _temp2768.tag= Cyc_Absyn_RgnHandleType; _temp2768.f1=( void*) t;
_temp2768;}); _temp2767;}), 0); _temp2764.f3= Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2764;});
_temp2763;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2762;}); _temp2761;}); break;} case 230:
_LL2760: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2775=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2775[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2776; _temp2776.tag= Cyc_Stmt_tok; _temp2776.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp2777=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp2777[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp2778; _temp2778.tag= Cyc_Absyn_Cut_s; _temp2778.f1= Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2778;}); _temp2777;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2776;}); _temp2775;}); break; case 231:
_LL2774: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2780=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2780[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2781; _temp2781.tag= Cyc_Stmt_tok; _temp2781.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Splice_s_struct* _temp2782=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp2782[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp2783; _temp2783.tag= Cyc_Absyn_Splice_s;
_temp2783.f1= Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2783;}); _temp2782;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2781;}); _temp2780;}); break; case 232: _LL2779: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2785=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2785[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2786; _temp2786.tag= Cyc_Stmt_tok; _temp2786.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp2787=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp2787[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp2788; _temp2788.tag= Cyc_Absyn_Label_s; _temp2788.f1=({
struct _tagged_arr* _temp2789=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2789[ 0]= Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2789;});
_temp2788.f2= Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2788;}); _temp2787;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2786;}); _temp2785;}); break; case 233: _LL2784: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2791=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2791[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2792; _temp2792.tag= Cyc_Stmt_tok; _temp2792.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp2792;});
_temp2791;}); break; case 234: _LL2790: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2794=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2794[ 0]=({ struct Cyc_Stmt_tok_struct _temp2795; _temp2795.tag= Cyc_Stmt_tok;
_temp2795.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2795;}); _temp2794;}); break; case 235: _LL2793: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2797=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2797[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2798; _temp2798.tag= Cyc_Stmt_tok; _temp2798.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2798;}); _temp2797;}); break; case 236: _LL2796: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)); break; case
237: _LL2799:{ void* _temp2801= Cyc_yyget_BlockItem_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); struct Cyc_List_List*
_temp2809; struct Cyc_Absyn_Fndecl* _temp2811; struct Cyc_Absyn_Stmt* _temp2813;
_LL2803: if(*(( int*) _temp2801) == Cyc_Parse_TopDecls_bl){ _LL2810: _temp2809=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2801)->f1; goto _LL2804;} else{ goto
_LL2805;} _LL2805: if(*(( int*) _temp2801) == Cyc_Parse_FnDecl_bl){ _LL2812:
_temp2811=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2801)->f1; goto _LL2806;}
else{ goto _LL2807;} _LL2807: if(*(( int*) _temp2801) == Cyc_Parse_Stmt_bl){
_LL2814: _temp2813=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2801)->f1; goto
_LL2808;} else{ goto _LL2802;} _LL2804: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2815=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2815[ 0]=({ struct Cyc_Stmt_tok_struct _temp2816; _temp2816.tag= Cyc_Stmt_tok;
_temp2816.f1= Cyc_Parse_flatten_declarations( _temp2809, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)));
_temp2816;}); _temp2815;}); goto _LL2802; _LL2806: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2817=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2817[ 0]=({ struct Cyc_Stmt_tok_struct _temp2818; _temp2818.tag= Cyc_Stmt_tok;
_temp2818.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2819=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2819[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2820; _temp2820.tag= Cyc_Absyn_Fn_d;
_temp2820.f1= _temp2811; _temp2820;}); _temp2819;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)), Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)));
_temp2818;}); _temp2817;}); goto _LL2802; _LL2808: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2821=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2821[ 0]=({ struct Cyc_Stmt_tok_struct _temp2822; _temp2822.tag= Cyc_Stmt_tok;
_temp2822.f1= _temp2813; _temp2822;}); _temp2821;}); goto _LL2802; _LL2802:;}
break; case 238: _LL2800:{ void* _temp2824= Cyc_yyget_BlockItem_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); struct Cyc_List_List*
_temp2832; struct Cyc_Absyn_Fndecl* _temp2834; struct Cyc_Absyn_Stmt* _temp2836;
_LL2826: if(*(( int*) _temp2824) == Cyc_Parse_TopDecls_bl){ _LL2833: _temp2832=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2824)->f1; goto _LL2827;} else{ goto
_LL2828;} _LL2828: if(*(( int*) _temp2824) == Cyc_Parse_FnDecl_bl){ _LL2835:
_temp2834=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2824)->f1; goto _LL2829;}
else{ goto _LL2830;} _LL2830: if(*(( int*) _temp2824) == Cyc_Parse_Stmt_bl){
_LL2837: _temp2836=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2824)->f1; goto
_LL2831;} else{ goto _LL2825;} _LL2827: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2838=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2838[ 0]=({ struct Cyc_Stmt_tok_struct _temp2839; _temp2839.tag= Cyc_Stmt_tok;
_temp2839.f1= Cyc_Parse_flatten_declarations( _temp2832, Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))));
_temp2839;}); _temp2838;}); goto _LL2825; _LL2829: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2840=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2840[ 0]=({ struct Cyc_Stmt_tok_struct _temp2841; _temp2841.tag= Cyc_Stmt_tok;
_temp2841.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2842=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2842[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2843; _temp2843.tag= Cyc_Absyn_Fn_d;
_temp2843.f1= _temp2834; _temp2843;}); _temp2842;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line)),
Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)))); _temp2841;}); _temp2840;}); goto _LL2825; _LL2831: yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2844=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2844[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2845; _temp2845.tag= Cyc_Stmt_tok; _temp2845.f1= Cyc_Absyn_seq_stmt(
_temp2836, Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2845;}); _temp2844;}); goto _LL2825; _LL2825:;}
break; case 239: _LL2823: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp2847=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2847[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2848; _temp2848.tag= Cyc_BlockItem_tok;
_temp2848.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp2849=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp2849[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp2850; _temp2850.tag=
Cyc_Parse_TopDecls_bl; _temp2850.f1= Cyc_yyget_DeclList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2850;});
_temp2849;})); _temp2848;}); _temp2847;}); break; case 240: _LL2846: yyval=(
void*)({ struct Cyc_BlockItem_tok_struct* _temp2852=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp2852[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp2853; _temp2853.tag= Cyc_BlockItem_tok; _temp2853.f1=(
void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp2854=( struct Cyc_Parse_Stmt_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct)); _temp2854[ 0]=({ struct
Cyc_Parse_Stmt_bl_struct _temp2855; _temp2855.tag= Cyc_Parse_Stmt_bl; _temp2855.f1=
Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp2855;}); _temp2854;})); _temp2853;}); _temp2852;}); break;
case 241: _LL2851: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2857=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2857[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2858; _temp2858.tag= Cyc_BlockItem_tok;
_temp2858.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp2859=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp2859[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp2860; _temp2860.tag= Cyc_Parse_FnDecl_bl;
_temp2860.f1= Cyc_yyget_FnDecl_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp2860;}); _temp2859;})); _temp2858;});
_temp2857;}); break; case 242: _LL2856: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2862=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2862[ 0]=({ struct Cyc_Stmt_tok_struct _temp2863; _temp2863.tag= Cyc_Stmt_tok;
_temp2863.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2863;}); _temp2862;}); break; case 243:
_LL2861: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2865=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2865[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2866; _temp2866.tag= Cyc_Stmt_tok; _temp2866.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 4))), Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2866;}); _temp2865;}); break; case 244: _LL2864: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2868=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2868[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2869; _temp2869.tag= Cyc_Stmt_tok; _temp2869.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))), Cyc_yyget_SwitchClauseList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2869;}); _temp2868;}); break; case 245: _LL2867: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 6))),
_tag_arr("C", sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line));} yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2871=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2871[ 0]=({ struct Cyc_Stmt_tok_struct _temp2872; _temp2872.tag= Cyc_Stmt_tok;
_temp2872.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_SwitchC_s_struct*
_temp2873=( struct Cyc_Absyn_SwitchC_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct));
_temp2873[ 0]=({ struct Cyc_Absyn_SwitchC_s_struct _temp2874; _temp2874.tag= Cyc_Absyn_SwitchC_s;
_temp2874.f1= Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 4))); _temp2874.f2= Cyc_yyget_SwitchCClauseList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2874;}); _temp2873;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2872;}); _temp2871;}); break; case 246:
_LL2870: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2876=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2876[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2877; _temp2877.tag= Cyc_Stmt_tok; _temp2877.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 4))), Cyc_yyget_SwitchClauseList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2877;}); _temp2876;}); break; case 247: _LL2875: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2879=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2879[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2880; _temp2880.tag= Cyc_SwitchClauseList_tok;
_temp2880.f1= 0; _temp2880;}); _temp2879;}); break; case 248: _LL2878: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2882=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2882[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2883; _temp2883.tag= Cyc_SwitchClauseList_tok;
_temp2883.f1=({ struct Cyc_List_List* _temp2884=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2884->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2885=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2885->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).last_line));
_temp2885->pat_vars= 0; _temp2885->where_clause= 0; _temp2885->body= Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2885->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp2885;}); _temp2884->tl= 0; _temp2884;});
_temp2883;}); _temp2882;}); break; case 249: _LL2881: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2887=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2887[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2888; _temp2888.tag= Cyc_SwitchClauseList_tok;
_temp2888.f1=({ struct Cyc_List_List* _temp2889=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2889->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2890=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2890->pattern= Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2890->pat_vars= 0; _temp2890->where_clause=
0; _temp2890->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line));
_temp2890->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp2890;}); _temp2889->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2889;}); _temp2888;}); _temp2887;}); break; case 250: _LL2886: yyval=( void*)({
struct Cyc_SwitchClauseList_tok_struct* _temp2892=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2892[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2893; _temp2893.tag= Cyc_SwitchClauseList_tok;
_temp2893.f1=({ struct Cyc_List_List* _temp2894=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2894->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2895=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2895->pattern= Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2895->pat_vars= 0; _temp2895->where_clause=
0; _temp2895->body= Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2895->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line);
_temp2895;}); _temp2894->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2894;});
_temp2893;}); _temp2892;}); break; case 251: _LL2891: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2897=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2897[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2898; _temp2898.tag= Cyc_SwitchClauseList_tok;
_temp2898.f1=({ struct Cyc_List_List* _temp2899=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2899->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2900=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2900->pattern= Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 4))); _temp2900->pat_vars= 0; _temp2900->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2900->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); _temp2900->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2900;}); _temp2899->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2899;});
_temp2898;}); _temp2897;}); break; case 252: _LL2896: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2902=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2902[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2903; _temp2903.tag= Cyc_SwitchClauseList_tok;
_temp2903.f1=({ struct Cyc_List_List* _temp2904=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2904->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2905=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2905->pattern= Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 5))); _temp2905->pat_vars= 0; _temp2905->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2905->body= Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2905->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp2905;}); _temp2904->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2904;}); _temp2903;}); _temp2902;}); break; case 253: _LL2901: yyval=( void*)({
struct Cyc_SwitchCClauseList_tok_struct* _temp2907=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp2907[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp2908; _temp2908.tag= Cyc_SwitchCClauseList_tok;
_temp2908.f1= 0; _temp2908;}); _temp2907;}); break; case 254: _LL2906: yyval=(
void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2910=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp2910[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp2911; _temp2911.tag= Cyc_SwitchCClauseList_tok;
_temp2911.f1=({ struct Cyc_List_List* _temp2912=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2912->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp2913=( struct Cyc_Absyn_SwitchC_clause*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp2913->cnst_exp= 0; _temp2913->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Absyn_break_stmt(
0), 0); _temp2913->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp2913;}); _temp2912->tl= 0; _temp2912;});
_temp2911;}); _temp2910;}); break; case 255: _LL2909: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp2915=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp2915[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp2916; _temp2916.tag= Cyc_SwitchCClauseList_tok;
_temp2916.f1=({ struct Cyc_List_List* _temp2917=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2917->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp2918=( struct Cyc_Absyn_SwitchC_clause*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp2918->cnst_exp=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp2918->body=
Cyc_Absyn_fallthru_stmt( 0, 0); _temp2918->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line);
_temp2918;}); _temp2917->tl= Cyc_yyget_SwitchCClauseList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp2917;});
_temp2916;}); _temp2915;}); break; case 256: _LL2914: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp2920=( struct Cyc_SwitchCClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp2920[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp2921; _temp2921.tag= Cyc_SwitchCClauseList_tok;
_temp2921.f1=({ struct Cyc_List_List* _temp2922=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2922->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp2923=( struct Cyc_Absyn_SwitchC_clause*) GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp2923->cnst_exp=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp2923->body=
Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Absyn_fallthru_stmt( 0, 0), 0);
_temp2923->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line); _temp2923;}); _temp2922->tl= Cyc_yyget_SwitchCClauseList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp2922;}); _temp2921;}); _temp2920;}); break; case 257: _LL2919: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2925=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2925[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2926; _temp2926.tag= Cyc_Stmt_tok; _temp2926.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2926;}); _temp2925;}); break; case 258: _LL2924: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2928=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2928[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2929; _temp2929.tag= Cyc_Stmt_tok; _temp2929.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 5))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2929;}); _temp2928;}); break; case 259: _LL2927: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2931=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2931[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2932; _temp2932.tag= Cyc_Stmt_tok; _temp2932.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2932;}); _temp2931;}); break; case 260: _LL2930: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2934=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2934[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2935; _temp2935.tag= Cyc_Stmt_tok; _temp2935.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2935;}); _temp2934;}); break; case 261: _LL2933: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2937=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2937[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2938; _temp2938.tag= Cyc_Stmt_tok; _temp2938.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3))), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2938;}); _temp2937;}); break; case 262: _LL2936: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2940=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2940[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2941; _temp2941.tag= Cyc_Stmt_tok; _temp2941.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 4))), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line));
_temp2941;}); _temp2940;}); break; case 263: _LL2939: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2943=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2943[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2944; _temp2944.tag= Cyc_Stmt_tok; _temp2944.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2944;}); _temp2943;}); break; case 264: _LL2942: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2946=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2946[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2947; _temp2947.tag= Cyc_Stmt_tok; _temp2947.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 5))), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2947;}); _temp2946;}); break; case 265: _LL2945: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2949=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2949[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2950; _temp2950.tag= Cyc_Stmt_tok; _temp2950.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 5))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp2950;}); _temp2949;}); break; case 266:
_LL2948: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2952=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2952[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2953; _temp2953.tag= Cyc_Stmt_tok; _temp2953.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 6))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 8))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp2953;}); _temp2952;}); break; case 267: _LL2951: { struct Cyc_List_List*
_temp2955= Cyc_yyget_DeclList_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 3))); goto _LL2956; _LL2956: { struct Cyc_Absyn_Stmt*
_temp2957= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
goto _LL2958; _LL2958: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2959=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2959[ 0]=({ struct Cyc_Stmt_tok_struct _temp2960; _temp2960.tag= Cyc_Stmt_tok;
_temp2960.f1= Cyc_Parse_flatten_declarations( _temp2955, _temp2957); _temp2960;});
_temp2959;}); break;}} case 268: _LL2954: { struct Cyc_List_List* _temp2962= Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
goto _LL2963; _LL2963: { struct Cyc_Absyn_Stmt* _temp2964= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 3))), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
goto _LL2965; _LL2965: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2966=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2966[ 0]=({ struct Cyc_Stmt_tok_struct _temp2967; _temp2967.tag= Cyc_Stmt_tok;
_temp2967.f1= Cyc_Parse_flatten_declarations( _temp2962, _temp2964); _temp2967;});
_temp2966;}); break;}} case 269: _LL2961: { struct Cyc_List_List* _temp2969= Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
goto _LL2970; _LL2970: { struct Cyc_Absyn_Stmt* _temp2971= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
goto _LL2972; _LL2972: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2973=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2973[ 0]=({ struct Cyc_Stmt_tok_struct _temp2974; _temp2974.tag= Cyc_Stmt_tok;
_temp2974.f1= Cyc_Parse_flatten_declarations( _temp2969, _temp2971); _temp2974;});
_temp2973;}); break;}} case 270: _LL2968: { struct Cyc_List_List* _temp2976= Cyc_yyget_DeclList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 5)));
goto _LL2977; _LL2977: { struct Cyc_Absyn_Stmt* _temp2978= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 4))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
goto _LL2979; _LL2979: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2980=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2980[ 0]=({ struct Cyc_Stmt_tok_struct _temp2981; _temp2981.tag= Cyc_Stmt_tok;
_temp2981.f1= Cyc_Parse_flatten_declarations( _temp2976, _temp2978); _temp2981;});
_temp2980;}); break;}} case 271: _LL2975: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2983=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2983[ 0]=({ struct Cyc_Stmt_tok_struct _temp2984; _temp2984.tag= Cyc_Stmt_tok;
_temp2984.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_arr* _temp2985=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2985[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2985;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); _temp2984;}); _temp2983;}); break; case 272:
_LL2982: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2987=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2987[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2988; _temp2988.tag= Cyc_Stmt_tok; _temp2988.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); _temp2988;}); _temp2987;}); break; case 273:
_LL2986: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2990=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2990[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2991; _temp2991.tag= Cyc_Stmt_tok; _temp2991.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line));
_temp2991;}); _temp2990;}); break; case 274: _LL2989: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2993=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2993[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2994; _temp2994.tag= Cyc_Stmt_tok; _temp2994.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); _temp2994;}); _temp2993;}); break; case 275:
_LL2992: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2996=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2996[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2997; _temp2997.tag= Cyc_Stmt_tok; _temp2997.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line));
_temp2997;}); _temp2996;}); break; case 276: _LL2995: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2999=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2999[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3000; _temp3000.tag= Cyc_Stmt_tok; _temp3000.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)); _temp3000;}); _temp2999;}); break; case 277:
_LL2998: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3002=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3002[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3003; _temp3003.tag= Cyc_Stmt_tok; _temp3003.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).last_line)); _temp3003;}); _temp3002;}); break; case 278:
_LL3001: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3005=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3005[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3006; _temp3006.tag= Cyc_Stmt_tok; _temp3006.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).last_line)); _temp3006;}); _temp3005;}); break; case 279:
_LL3004: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3008=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3008[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3009; _temp3009.tag= Cyc_Pattern_tok; _temp3009.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3009;}); _temp3008;}); break; case 280:
_LL3007: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1)); break; case 281: _LL3010: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3012=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3012[ 0]=({ struct Cyc_Pattern_tok_struct _temp3013; _temp3013.tag= Cyc_Pattern_tok;
_temp3013.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3014=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3014[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3015; _temp3015.tag= Cyc_Absyn_Int_p;
_temp3015.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset)))).f1; _temp3015.f2=(* Cyc_yyget_Int_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f2;
_temp3015;}); _temp3014;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3013;}); _temp3012;}); break; case 282:
_LL3011: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3017=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3017[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3018; _temp3018.tag= Cyc_Pattern_tok; _temp3018.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3019=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3019[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3020; _temp3020.tag= Cyc_Absyn_Int_p; _temp3020.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3020.f2= -(* Cyc_yyget_Int_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset)))).f2; _temp3020;}); _temp3019;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3018;}); _temp3017;}); break; case 283: _LL3016: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3022=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3022[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3023; _temp3023.tag= Cyc_Pattern_tok; _temp3023.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3024=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3024[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3025; _temp3025.tag= Cyc_Absyn_Float_p; _temp3025.f1=
Cyc_yyget_String_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp3025;}); _temp3024;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp3023;});
_temp3022;}); break; case 284: _LL3021: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3027=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3027[ 0]=({ struct Cyc_Pattern_tok_struct _temp3028; _temp3028.tag= Cyc_Pattern_tok;
_temp3028.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3029=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3029[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3030; _temp3030.tag= Cyc_Absyn_Char_p;
_temp3030.f1= Cyc_yyget_Char_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp3030;}); _temp3029;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp3028;});
_temp3027;}); break; case 285: _LL3026: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3032=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3032[ 0]=({ struct Cyc_Pattern_tok_struct _temp3033; _temp3033.tag= Cyc_Pattern_tok;
_temp3033.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp3033;});
_temp3032;}); break; case 286: _LL3031: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3035=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3035[ 0]=({ struct Cyc_Pattern_tok_struct _temp3036; _temp3036.tag= Cyc_Pattern_tok;
_temp3036.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3037=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3037[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3038; _temp3038.tag=
Cyc_Absyn_UnknownId_p; _temp3038.f1= Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp3038;});
_temp3037;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3036;}); _temp3035;}); break; case 287:
_LL3034: { struct Cyc_List_List* _temp3040=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).last_line),
Cyc_yyget_TypeList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3)))); goto _LL3041; _LL3041: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3042=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3042[ 0]=({ struct Cyc_Pattern_tok_struct _temp3043; _temp3043.tag= Cyc_Pattern_tok;
_temp3043.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3044=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3044[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3045; _temp3045.tag= Cyc_Absyn_UnknownCall_p; _temp3045.f1= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp3045.f2= _temp3040; _temp3045.f3= Cyc_yyget_PatternList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3045;});
_temp3044;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3043;}); _temp3042;}); break;} case 288:
_LL3039: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3047=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3047[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3048; _temp3048.tag= Cyc_Pattern_tok; _temp3048.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3049=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3049[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3050; _temp3050.tag= Cyc_Absyn_Tuple_p; _temp3050.f1=
Cyc_yyget_PatternList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset - 1))); _temp3050;}); _temp3049;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3048;}); _temp3047;}); break; case 289: _LL3046: { struct Cyc_List_List*
_temp3052=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).last_line), Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)))); goto
_LL3053; _LL3053: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3054=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3054[ 0]=({ struct Cyc_Pattern_tok_struct _temp3055; _temp3055.tag= Cyc_Pattern_tok;
_temp3055.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3056=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3056[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3057; _temp3057.tag= Cyc_Absyn_UnknownFields_p; _temp3057.f1= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3)));
_temp3057.f2= _temp3052; _temp3057.f3= 0; _temp3057;}); _temp3056;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3055;}); _temp3054;}); break;} case 290: _LL3051: { struct Cyc_List_List*
_temp3059=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).last_line), Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3)))); goto
_LL3060; _LL3060: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3061=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3061[ 0]=({ struct Cyc_Pattern_tok_struct _temp3062; _temp3062.tag= Cyc_Pattern_tok;
_temp3062.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3063=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3063[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3064; _temp3064.tag= Cyc_Absyn_UnknownFields_p; _temp3064.f1= Cyc_yyget_QualId_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4)));
_temp3064.f2= _temp3059; _temp3064.f3= Cyc_yyget_FieldPatternList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3064;});
_temp3063;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3062;}); _temp3061;}); break;} case 291:
_LL3058: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3066=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3066[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3067; _temp3067.tag= Cyc_Pattern_tok; _temp3067.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3068=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3068[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3069; _temp3069.tag= Cyc_Absyn_Pointer_p;
_temp3069.f1= Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp3069;}); _temp3068;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3067;}); _temp3066;}); break; case 292: _LL3065: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3071=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3071[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3072; _temp3072.tag= Cyc_Pattern_tok; _temp3072.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3073=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3073[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3074; _temp3074.tag= Cyc_Absyn_Reference_p;
_temp3074.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3075=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3075->f1=( void*) Cyc_Absyn_Loc_n;
_temp3075->f2=({ struct _tagged_arr* _temp3076=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp3076[ 0]= Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))); _temp3076;});
_temp3075;}),( void*) Cyc_Absyn_VoidType, 0); _temp3074;}); _temp3073;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3072;}); _temp3071;}); break; case 293: _LL3070: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3078=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3078[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3079; _temp3079.tag= Cyc_PatternList_tok;
_temp3079.f1= 0; _temp3079;}); _temp3078;}); break; case 294: _LL3077: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3081=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3081[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3082; _temp3082.tag= Cyc_PatternList_tok;
_temp3082.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof(
void*), yyvsp_offset)))); _temp3082;}); _temp3081;}); break; case 295: _LL3080:
yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp3084=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3084[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3085; _temp3085.tag= Cyc_PatternList_tok;
_temp3085.f1=({ struct Cyc_List_List* _temp3086=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3086->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp3086->tl= 0; _temp3086;}); _temp3085;}); _temp3084;}); break; case 296:
_LL3083: yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp3088=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp3088[ 0]=({ struct Cyc_PatternList_tok_struct _temp3089; _temp3089.tag= Cyc_PatternList_tok;
_temp3089.f1=({ struct Cyc_List_List* _temp3090=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3090->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp3090->tl= Cyc_yyget_PatternList_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3090;}); _temp3089;}); _temp3088;});
break; case 297: _LL3087: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct*
_temp3092=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp3092[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp3093; _temp3093.tag=
Cyc_FieldPattern_tok; _temp3093.f1=({ struct _tuple14* _temp3094=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3094->f1= 0; _temp3094->f2=
Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))); _temp3094;}); _temp3093;}); _temp3092;}); break; case 298:
_LL3091: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp3096=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp3096[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp3097; _temp3097.tag=
Cyc_FieldPattern_tok; _temp3097.f1=({ struct _tuple14* _temp3098=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3098->f1= Cyc_yyget_DesignatorList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3098->f2= Cyc_yyget_Pattern_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp3098;}); _temp3097;}); _temp3096;});
break; case 299: _LL3095: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp3100=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp3100[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp3101; _temp3101.tag= Cyc_FieldPatternList_tok; _temp3101.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))));
_temp3101;}); _temp3100;}); break; case 300: _LL3099: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3103=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3103[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3104; _temp3104.tag= Cyc_FieldPatternList_tok;
_temp3104.f1=({ struct Cyc_List_List* _temp3105=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3105->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp3105->tl= 0; _temp3105;}); _temp3104;}); _temp3103;}); break; case 301:
_LL3102: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3107=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp3107[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp3108; _temp3108.tag=
Cyc_FieldPatternList_tok; _temp3108.f1=({ struct Cyc_List_List* _temp3109=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3109->hd=(
void*) Cyc_yyget_FieldPattern_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp3109->tl= Cyc_yyget_FieldPatternList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp3109;}); _temp3108;}); _temp3107;}); break; case 302: _LL3106: yyval=*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break;
case 303: _LL3110: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3112=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3112[ 0]=({ struct Cyc_Exp_tok_struct _temp3113; _temp3113.tag= Cyc_Exp_tok;
_temp3113.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3113;}); _temp3112;}); break; case 304: _LL3111: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 305:
_LL3114: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3116=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3116[ 0]=({ struct Cyc_Exp_tok_struct
_temp3117; _temp3117.tag= Cyc_Exp_tok; _temp3117.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Primopopt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3117;}); _temp3116;}); break; case 306: _LL3115: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3119=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3119[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3120; _temp3120.tag= Cyc_Primopopt_tok; _temp3120.f1=
0; _temp3120;}); _temp3119;}); break; case 307: _LL3118: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3122=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3122[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3123; _temp3123.tag= Cyc_Primopopt_tok; _temp3123.f1=({
struct Cyc_Core_Opt* _temp3124=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3124->v=( void*)(( void*) Cyc_Absyn_Times); _temp3124;});
_temp3123;}); _temp3122;}); break; case 308: _LL3121: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3126=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3126[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3127; _temp3127.tag= Cyc_Primopopt_tok; _temp3127.f1=({
struct Cyc_Core_Opt* _temp3128=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3128->v=( void*)(( void*) Cyc_Absyn_Div); _temp3128;});
_temp3127;}); _temp3126;}); break; case 309: _LL3125: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3130=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3130[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3131; _temp3131.tag= Cyc_Primopopt_tok; _temp3131.f1=({
struct Cyc_Core_Opt* _temp3132=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3132->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3132;});
_temp3131;}); _temp3130;}); break; case 310: _LL3129: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3134=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3134[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3135; _temp3135.tag= Cyc_Primopopt_tok; _temp3135.f1=({
struct Cyc_Core_Opt* _temp3136=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3136->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3136;});
_temp3135;}); _temp3134;}); break; case 311: _LL3133: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3138=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3138[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3139; _temp3139.tag= Cyc_Primopopt_tok; _temp3139.f1=({
struct Cyc_Core_Opt* _temp3140=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3140->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3140;});
_temp3139;}); _temp3138;}); break; case 312: _LL3137: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3142=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3142[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3143; _temp3143.tag= Cyc_Primopopt_tok; _temp3143.f1=({
struct Cyc_Core_Opt* _temp3144=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3144->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3144;});
_temp3143;}); _temp3142;}); break; case 313: _LL3141: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3146=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3146[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3147; _temp3147.tag= Cyc_Primopopt_tok; _temp3147.f1=({
struct Cyc_Core_Opt* _temp3148=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3148->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3148;});
_temp3147;}); _temp3146;}); break; case 314: _LL3145: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3150=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3150[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3151; _temp3151.tag= Cyc_Primopopt_tok; _temp3151.f1=({
struct Cyc_Core_Opt* _temp3152=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3152->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3152;});
_temp3151;}); _temp3150;}); break; case 315: _LL3149: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3154=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3154[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3155; _temp3155.tag= Cyc_Primopopt_tok; _temp3155.f1=({
struct Cyc_Core_Opt* _temp3156=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3156->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3156;});
_temp3155;}); _temp3154;}); break; case 316: _LL3153: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3158=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3158[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3159; _temp3159.tag= Cyc_Primopopt_tok; _temp3159.f1=({
struct Cyc_Core_Opt* _temp3160=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3160->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3160;});
_temp3159;}); _temp3158;}); break; case 317: _LL3157: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 318:
_LL3161: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3163=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3163[ 0]=({ struct Cyc_Exp_tok_struct
_temp3164; _temp3164.tag= Cyc_Exp_tok; _temp3164.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 4))), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3164;}); _temp3163;}); break; case 319: _LL3162: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3166=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3166[ 0]=({ struct Cyc_Exp_tok_struct
_temp3167; _temp3167.tag= Cyc_Exp_tok; _temp3167.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3167;}); _temp3166;}); break; case 320: _LL3165: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3169=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3169[ 0]=({ struct Cyc_Exp_tok_struct
_temp3170; _temp3170.tag= Cyc_Exp_tok; _temp3170.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1))).last_line));
_temp3170;}); _temp3169;}); break; case 321: _LL3168: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3172=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3172[ 0]=({ struct Cyc_Exp_tok_struct
_temp3173; _temp3173.tag= Cyc_Exp_tok; _temp3173.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1))).last_line));
_temp3173;}); _temp3172;}); break; case 322: _LL3171: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3175=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3175[ 0]=({ struct Cyc_Exp_tok_struct
_temp3176; _temp3176.tag= Cyc_Exp_tok; _temp3176.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3176;}); _temp3175;}); break; case 323: _LL3174: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3178=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3178[ 0]=({ struct Cyc_Exp_tok_struct
_temp3179; _temp3179.tag= Cyc_Exp_tok; _temp3179.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3179;}); _temp3178;}); break; case 324: _LL3177: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 325:
_LL3180: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 326: _LL3181: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3183=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3183[ 0]=({ struct Cyc_Exp_tok_struct _temp3184; _temp3184.tag= Cyc_Exp_tok;
_temp3184.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3184;}); _temp3183;}); break; case 327: _LL3182: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 328:
_LL3185: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3187=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3187[ 0]=({ struct Cyc_Exp_tok_struct
_temp3188; _temp3188.tag= Cyc_Exp_tok; _temp3188.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3188;}); _temp3187;}); break; case 329: _LL3186: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 330:
_LL3189: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3191=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3191[ 0]=({ struct Cyc_Exp_tok_struct
_temp3192; _temp3192.tag= Cyc_Exp_tok; _temp3192.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3192;}); _temp3191;}); break; case 331: _LL3190: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 332:
_LL3193: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3195=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3195[ 0]=({ struct Cyc_Exp_tok_struct
_temp3196; _temp3196.tag= Cyc_Exp_tok; _temp3196.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3196;}); _temp3195;}); break; case 333: _LL3194: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 334:
_LL3197: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3199=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3199[ 0]=({ struct Cyc_Exp_tok_struct
_temp3200; _temp3200.tag= Cyc_Exp_tok; _temp3200.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3200;}); _temp3199;}); break; case 335: _LL3198: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 336:
_LL3201: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3203=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3203[ 0]=({ struct Cyc_Exp_tok_struct
_temp3204; _temp3204.tag= Cyc_Exp_tok; _temp3204.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3204;}); _temp3203;}); break; case 337: _LL3202: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3206=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3206[ 0]=({ struct Cyc_Exp_tok_struct
_temp3207; _temp3207.tag= Cyc_Exp_tok; _temp3207.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3207;}); _temp3206;}); break; case 338: _LL3205: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 339:
_LL3208: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3210=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3210[ 0]=({ struct Cyc_Exp_tok_struct
_temp3211; _temp3211.tag= Cyc_Exp_tok; _temp3211.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3211;}); _temp3210;}); break; case 340: _LL3209: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3213=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3213[ 0]=({ struct Cyc_Exp_tok_struct
_temp3214; _temp3214.tag= Cyc_Exp_tok; _temp3214.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3214;}); _temp3213;}); break; case 341: _LL3212: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3216=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3216[ 0]=({ struct Cyc_Exp_tok_struct
_temp3217; _temp3217.tag= Cyc_Exp_tok; _temp3217.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3217;}); _temp3216;}); break; case 342: _LL3215: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3219=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3219[ 0]=({ struct Cyc_Exp_tok_struct
_temp3220; _temp3220.tag= Cyc_Exp_tok; _temp3220.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3220;}); _temp3219;}); break; case 343: _LL3218: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 344:
_LL3221: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3223=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3223[ 0]=({ struct Cyc_Exp_tok_struct
_temp3224; _temp3224.tag= Cyc_Exp_tok; _temp3224.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3224;}); _temp3223;}); break; case 345: _LL3222: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3226=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3226[ 0]=({ struct Cyc_Exp_tok_struct
_temp3227; _temp3227.tag= Cyc_Exp_tok; _temp3227.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3227;}); _temp3226;}); break; case 346: _LL3225: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 347:
_LL3228: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3230=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3230[ 0]=({ struct Cyc_Exp_tok_struct
_temp3231; _temp3231.tag= Cyc_Exp_tok; _temp3231.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3231;}); _temp3230;}); break; case 348: _LL3229: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3233=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3233[ 0]=({ struct Cyc_Exp_tok_struct
_temp3234; _temp3234.tag= Cyc_Exp_tok; _temp3234.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3234;}); _temp3233;}); break; case 349: _LL3232: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 350:
_LL3235: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3237=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3237[ 0]=({ struct Cyc_Exp_tok_struct
_temp3238; _temp3238.tag= Cyc_Exp_tok; _temp3238.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3238;}); _temp3237;}); break; case 351: _LL3236: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3240=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3240[ 0]=({ struct Cyc_Exp_tok_struct
_temp3241; _temp3241.tag= Cyc_Exp_tok; _temp3241.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3241;}); _temp3240;}); break; case 352: _LL3239: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3243=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3243[ 0]=({ struct Cyc_Exp_tok_struct
_temp3244; _temp3244.tag= Cyc_Exp_tok; _temp3244.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 2))), Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3244;}); _temp3243;}); break; case 353: _LL3242: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 354:
_LL3245: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3247=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3247[ 0]=({ struct Cyc_Exp_tok_struct
_temp3248; _temp3248.tag= Cyc_Exp_tok; _temp3248.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)))).f3,
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3248;}); _temp3247;}); break; case 355:
_LL3246: yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)); break; case 356: _LL3249: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3251=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3251[ 0]=({ struct Cyc_Exp_tok_struct _temp3252; _temp3252.tag= Cyc_Exp_tok;
_temp3252.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3252;}); _temp3251;}); break; case 357: _LL3250: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3254=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3254[ 0]=({ struct Cyc_Exp_tok_struct
_temp3255; _temp3255.tag= Cyc_Exp_tok; _temp3255.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3255;}); _temp3254;}); break; case 358: _LL3253: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3257=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3257[ 0]=({ struct Cyc_Exp_tok_struct
_temp3258; _temp3258.tag= Cyc_Exp_tok; _temp3258.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3258;}); _temp3257;}); break; case 359: _LL3256: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3260=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3260[ 0]=({ struct Cyc_Exp_tok_struct
_temp3261; _temp3261.tag= Cyc_Exp_tok; _temp3261.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3261;}); _temp3260;}); break; case 360: _LL3259: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 361:
_LL3262: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3264=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3264[ 0]=({ struct Cyc_Exp_tok_struct
_temp3265; _temp3265.tag= Cyc_Exp_tok; _temp3265.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3265;}); _temp3264;}); break; case 362: _LL3263: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3267=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3267[ 0]=({ struct Cyc_Exp_tok_struct
_temp3268; _temp3268.tag= Cyc_Exp_tok; _temp3268.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 1)))).f3, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3268;}); _temp3267;}); break; case 363:
_LL3266: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3270=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3270[ 0]=({ struct Cyc_Exp_tok_struct
_temp3271; _temp3271.tag= Cyc_Exp_tok; _temp3271.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset))), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3271;}); _temp3270;}); break; case 364:
_LL3269: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3273=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3273[ 0]=({ struct Cyc_Exp_tok_struct
_temp3274; _temp3274.tag= Cyc_Exp_tok; _temp3274.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3)))).f3,({ struct _tagged_arr* _temp3275=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3275[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3275;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3274;}); _temp3273;}); break; case 365:
_LL3272: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3277=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3277[ 0]=({ struct Cyc_Exp_tok_struct
_temp3278; _temp3278.tag= Cyc_Exp_tok; _temp3278.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 3))), Cyc_yyget_ExpList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3278;}); _temp3277;}); break; case 366: _LL3276: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3280=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3280[ 0]=({ struct Cyc_Exp_tok_struct
_temp3281; _temp3281.tag= Cyc_Exp_tok; _temp3281.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3282=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3282[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3283; _temp3283.tag= Cyc_Absyn_Malloc_e;
_temp3283.f1= 0; _temp3283.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*(( struct
Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).last_line)); _temp3283;}); _temp3282;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3281;}); _temp3280;}); break; case 367: _LL3279: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3285=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3285[ 0]=({ struct Cyc_Exp_tok_struct
_temp3286; _temp3286.tag= Cyc_Exp_tok; _temp3286.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3287=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3287[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3288; _temp3288.tag= Cyc_Absyn_Malloc_e;
_temp3288.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 6))); _temp3288.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).last_line));
_temp3288;}); _temp3287;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3286;}); _temp3285;}); break; case 368:
_LL3284: yyval=( void*)({ struct Cyc_Primop_tok_struct* _temp3290=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp3290[ 0]=({ struct Cyc_Primop_tok_struct
_temp3291; _temp3291.tag= Cyc_Primop_tok; _temp3291.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp3291;}); _temp3290;}); break; case 369: _LL3289: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3293=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3293[ 0]=({ struct Cyc_Primop_tok_struct
_temp3294; _temp3294.tag= Cyc_Primop_tok; _temp3294.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp3294;}); _temp3293;}); break; case 370: _LL3292: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3296=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3296[ 0]=({ struct Cyc_Primop_tok_struct
_temp3297; _temp3297.tag= Cyc_Primop_tok; _temp3297.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp3297;}); _temp3296;}); break; case 371: _LL3295: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3299=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3299[ 0]=({ struct Cyc_Primop_tok_struct
_temp3300; _temp3300.tag= Cyc_Primop_tok; _temp3300.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp3300;}); _temp3299;}); break; case 372: _LL3298: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3302=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3302[ 0]=({ struct Cyc_Primop_tok_struct
_temp3303; _temp3303.tag= Cyc_Primop_tok; _temp3303.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp3303;}); _temp3302;}); break; case 373: _LL3301: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3305=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3305[ 0]=({ struct Cyc_Primop_tok_struct
_temp3306; _temp3306.tag= Cyc_Primop_tok; _temp3306.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp3306;}); _temp3305;}); break; case 374: _LL3304: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3308=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3308[ 0]=({ struct Cyc_Primop_tok_struct
_temp3309; _temp3309.tag= Cyc_Primop_tok; _temp3309.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp3309;}); _temp3308;}); break; case 375: _LL3307: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3311=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3311[ 0]=({ struct Cyc_Primop_tok_struct
_temp3312; _temp3312.tag= Cyc_Primop_tok; _temp3312.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp3312;}); _temp3311;}); break; case 376: _LL3310: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3314=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3314[ 0]=({ struct Cyc_Primop_tok_struct
_temp3315; _temp3315.tag= Cyc_Primop_tok; _temp3315.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp3315;}); _temp3314;}); break; case 377: _LL3313: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 378:
_LL3316: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3318=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3318[ 0]=({ struct Cyc_Exp_tok_struct
_temp3319; _temp3319.tag= Cyc_Exp_tok; _temp3319.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3))), Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3319;}); _temp3318;}); break; case 379: _LL3317: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3321=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3321[ 0]=({ struct Cyc_Exp_tok_struct
_temp3322; _temp3322.tag= Cyc_Exp_tok; _temp3322.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3322;}); _temp3321;}); break; case 380:
_LL3320: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3324=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3324[ 0]=({ struct Cyc_Exp_tok_struct
_temp3325; _temp3325.tag= Cyc_Exp_tok; _temp3325.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 3))), Cyc_yyget_ExpList_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3325;}); _temp3324;}); break; case 381: _LL3323: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3327=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3327[ 0]=({ struct Cyc_Exp_tok_struct
_temp3328; _temp3328.tag= Cyc_Exp_tok; _temp3328.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))),({ struct _tagged_arr* _temp3329=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3329[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp3329;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3328;}); _temp3327;}); break; case 382:
_LL3326: { struct _tuple1* q= Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));{ void*
_temp3331=(* q).f1; struct Cyc_List_List* _temp3341; struct Cyc_List_List*
_temp3343; _LL3333: if( _temp3331 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3334;}
else{ goto _LL3335;} _LL3335: if(( unsigned int) _temp3331 > 1u?*(( int*)
_temp3331) == Cyc_Absyn_Rel_n: 0){ _LL3342: _temp3341=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3331)->f1; if( _temp3341 == 0){ goto _LL3336;} else{ goto _LL3337;}} else{
goto _LL3337;} _LL3337: if(( unsigned int) _temp3331 > 1u?*(( int*) _temp3331)
== Cyc_Absyn_Abs_n: 0){ _LL3344: _temp3343=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3331)->f1; if( _temp3343 == 0){ goto _LL3338;} else{ goto _LL3339;}} else{
goto _LL3339;} _LL3339: goto _LL3340; _LL3334: goto _LL3332; _LL3336: goto
_LL3332; _LL3338: goto _LL3332; _LL3340: Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); goto _LL3332; _LL3332:;} yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3345=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3345[ 0]=({ struct Cyc_Exp_tok_struct
_temp3346; _temp3346.tag= Cyc_Exp_tok; _temp3346.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))),(* q).f2, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3346;}); _temp3345;}); break;} case 383:
_LL3330: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3348=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3348[ 0]=({ struct Cyc_Exp_tok_struct
_temp3349; _temp3349.tag= Cyc_Exp_tok; _temp3349.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))),({ struct _tagged_arr* _temp3350=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp3350[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp3350;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3349;}); _temp3348;}); break; case 384:
_LL3347: { struct _tuple1* q= Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));{ void*
_temp3352=(* q).f1; struct Cyc_List_List* _temp3362; struct Cyc_List_List*
_temp3364; _LL3354: if( _temp3352 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3355;}
else{ goto _LL3356;} _LL3356: if(( unsigned int) _temp3352 > 1u?*(( int*)
_temp3352) == Cyc_Absyn_Rel_n: 0){ _LL3363: _temp3362=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3352)->f1; if( _temp3362 == 0){ goto _LL3357;} else{ goto _LL3358;}} else{
goto _LL3358;} _LL3358: if(( unsigned int) _temp3352 > 1u?*(( int*) _temp3352)
== Cyc_Absyn_Abs_n: 0){ _LL3365: _temp3364=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3352)->f1; if( _temp3364 == 0){ goto _LL3359;} else{ goto _LL3360;}} else{
goto _LL3360;} _LL3360: goto _LL3361; _LL3355: goto _LL3353; _LL3357: goto
_LL3353; _LL3359: goto _LL3353; _LL3361: Cyc_Parse_err( _tag_arr("struct field is qualified with module name",
sizeof( unsigned char), 43u), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); goto _LL3353; _LL3353:;} yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3366=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3366[ 0]=({ struct Cyc_Exp_tok_struct
_temp3367; _temp3367.tag= Cyc_Exp_tok; _temp3367.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset - 2))),(* q).f2, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3367;}); _temp3366;}); break;} case 385:
_LL3351: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3369=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3369[ 0]=({ struct Cyc_Exp_tok_struct
_temp3370; _temp3370.tag= Cyc_Exp_tok; _temp3370.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3370;}); _temp3369;}); break; case 386: _LL3368: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3372=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3372[ 0]=({ struct Cyc_Exp_tok_struct
_temp3373; _temp3373.tag= Cyc_Exp_tok; _temp3373.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3373;}); _temp3372;}); break; case 387: _LL3371: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3375=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3375[ 0]=({ struct Cyc_Exp_tok_struct
_temp3376; _temp3376.tag= Cyc_Exp_tok; _temp3376.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3377=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3377[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3378; _temp3378.tag= Cyc_Absyn_CompoundLit_e;
_temp3378.f1= Cyc_yyget_ParamDecl_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 4))); _temp3378.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))));
_temp3378;}); _temp3377;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3376;}); _temp3375;}); break; case 388:
_LL3374: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3380=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3380[ 0]=({ struct Cyc_Exp_tok_struct
_temp3381; _temp3381.tag= Cyc_Exp_tok; _temp3381.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3382=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3382[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3383; _temp3383.tag= Cyc_Absyn_CompoundLit_e;
_temp3383.f1= Cyc_yyget_ParamDecl_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 5))); _temp3383.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))));
_temp3383;}); _temp3382;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3381;}); _temp3380;}); break; case 389:
_LL3379: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3385=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3385[ 0]=({ struct Cyc_Exp_tok_struct
_temp3386; _temp3386.tag= Cyc_Exp_tok; _temp3386.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3387=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3387[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3388; _temp3388.tag= Cyc_Absyn_Fill_e; _temp3388.f1= Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3388;}); _temp3387;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3386;}); _temp3385;}); break; case 390:
_LL3384: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3390=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3390[ 0]=({ struct Cyc_Exp_tok_struct
_temp3391; _temp3391.tag= Cyc_Exp_tok; _temp3391.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3392=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3392[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3393; _temp3393.tag= Cyc_Absyn_Codegen_e;
_temp3393.f1= Cyc_yyget_FnDecl_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp3393;}); _temp3392;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3391;}); _temp3390;}); break; case 391: _LL3389: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3395=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3395[ 0]=({ struct Cyc_Exp_tok_struct
_temp3396; _temp3396.tag= Cyc_Exp_tok; _temp3396.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3397=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3397[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3398; _temp3398.tag= Cyc_Absyn_UnknownId_e;
_temp3398.f1= Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset))); _temp3398;}); _temp3397;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp3396;});
_temp3395;}); break; case 392: _LL3394: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)); break; case 393:
_LL3399: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3401=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3401[ 0]=({ struct Cyc_Exp_tok_struct
_temp3402; _temp3402.tag= Cyc_Exp_tok; _temp3402.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp3402;});
_temp3401;}); break; case 394: _LL3400: yyval=*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)); break; case
395: _LL3403: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3405=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3405[ 0]=({ struct Cyc_Exp_tok_struct
_temp3406; _temp3406.tag= Cyc_Exp_tok; _temp3406.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3407=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3407[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3408; _temp3408.tag=
Cyc_Absyn_UnknownId_e; _temp3408.f1= Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3408;});
_temp3407;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3406;}); _temp3405;}); break; case 396:
_LL3404: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3410=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3410[ 0]=({ struct Cyc_Exp_tok_struct
_temp3411; _temp3411.tag= Cyc_Exp_tok; _temp3411.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3412=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3412[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3413; _temp3413.tag=
Cyc_Absyn_UnknownId_e; _temp3413.f1= Cyc_yyget_QualId_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 4))); _temp3413;});
_temp3412;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).last_line)),(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3411;}); _temp3410;}); break; case 397: _LL3409: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3415=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3415[ 0]=({ struct Cyc_Exp_tok_struct
_temp3416; _temp3416.tag= Cyc_Exp_tok; _temp3416.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3416;}); _temp3415;}); break; case 398: _LL3414: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3418=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3418[ 0]=({ struct Cyc_Exp_tok_struct
_temp3419; _temp3419.tag= Cyc_Exp_tok; _temp3419.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3420=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3420[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3421; _temp3421.tag= Cyc_Absyn_Struct_e;
_temp3421.f1= Cyc_yyget_QualId_tok(*(( void**) _check_unknown_subscript( yyvs,
sizeof( void*), yyvsp_offset - 3))); _temp3421.f2= 0; _temp3421.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 1))));
_temp3421.f4= 0; _temp3421;}); _temp3420;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3419;}); _temp3418;}); break; case 399: _LL3417: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3423=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3423[ 0]=({ struct Cyc_Exp_tok_struct
_temp3424; _temp3424.tag= Cyc_Exp_tok; _temp3424.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line));
_temp3424;}); _temp3423;}); break; case 400: _LL3422: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3426=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3426[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3427; _temp3427.tag= Cyc_ExpList_tok; _temp3427.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))); _temp3427;});
_temp3426;}); break; case 401: _LL3425: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3429=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3429[ 0]=({ struct Cyc_ExpList_tok_struct _temp3430; _temp3430.tag= Cyc_ExpList_tok;
_temp3430.f1=({ struct Cyc_List_List* _temp3431=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3431->hd=( void*) Cyc_yyget_Exp_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp3431->tl= 0; _temp3431;}); _temp3430;}); _temp3429;}); break; case 402:
_LL3428: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp3433=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3433[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3434; _temp3434.tag= Cyc_ExpList_tok; _temp3434.f1=({ struct Cyc_List_List*
_temp3435=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3435->hd=( void*) Cyc_yyget_Exp_tok(*(( void**) _check_unknown_subscript(
yyvs, sizeof( void*), yyvsp_offset))); _temp3435->tl= Cyc_yyget_ExpList_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset - 2)));
_temp3435;}); _temp3434;}); _temp3433;}); break; case 403: _LL3432: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3437=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3437[ 0]=({ struct Cyc_Exp_tok_struct
_temp3438; _temp3438.tag= Cyc_Exp_tok; _temp3438.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)))).f1,(*
Cyc_yyget_Int_tok(*(( void**) _check_unknown_subscript( yyvs, sizeof( void*),
yyvsp_offset)))).f2, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3438;}); _temp3437;}); break; case 404:
_LL3436: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3440=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3440[ 0]=({ struct Cyc_Exp_tok_struct
_temp3441; _temp3441.tag= Cyc_Exp_tok; _temp3441.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp3441;});
_temp3440;}); break; case 405: _LL3439: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3443=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3443[ 0]=({ struct Cyc_Exp_tok_struct _temp3444; _temp3444.tag= Cyc_Exp_tok;
_temp3444.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).first_line,(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line)); _temp3444;});
_temp3443;}); break; case 406: _LL3442: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3446=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3446[ 0]=({ struct Cyc_Exp_tok_struct _temp3447; _temp3447.tag= Cyc_Exp_tok;
_temp3447.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line)); _temp3447;}); _temp3446;}); break; case 407:
_LL3445: yyval=( void*)({ struct Cyc_QualId_tok_struct* _temp3449=( struct Cyc_QualId_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualId_tok_struct)); _temp3449[ 0]=({ struct Cyc_QualId_tok_struct
_temp3450; _temp3450.tag= Cyc_QualId_tok; _temp3450.f1=({ struct _tuple1*
_temp3451=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3451->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3453=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3453[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3454; _temp3454.tag= Cyc_Absyn_Rel_n; _temp3454.f1= 0; _temp3454;});
_temp3453;}); _temp3451->f2=({ struct _tagged_arr* _temp3452=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp3452[ 0]= Cyc_yyget_String_tok(*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset)));
_temp3452;}); _temp3451;}); _temp3450;}); _temp3449;}); break; case 408: _LL3448:
yyval=*(( void**) _check_unknown_subscript( yyvs, sizeof( void*), yyvsp_offset));
break; default: _LL3455:( int) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset
-= yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;*(( void**)
_check_unknown_subscript( yyvs, sizeof( void*), ++ yyvsp_offset))= yyval;
yylsp_offset ++; if( yylen == 0){(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).first_line=
Cyc_yylloc.first_line;(*(( struct Cyc_Yyltype*) _check_unknown_subscript( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset))).first_column= Cyc_yylloc.first_column;(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset))).last_line=(*(( struct Cyc_Yyltype*) _check_unknown_subscript(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1))).last_line;(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_column=(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1))).last_column;(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).text=
_tag_arr("", sizeof( unsigned char), 1u);} else{(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_line=(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),(
yylsp_offset + yylen) - 1))).last_line;(*(( struct Cyc_Yyltype*)
_check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset))).last_column=(*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),(
yylsp_offset + yylen) - 1))).last_column;} yyn=( int) Cyc_yyr1[
_check_known_subscript_notnull( 409u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 113u, yyn - 125)] +*(( short*)
_check_unknown_subscript( yyss, sizeof( short), yyssp_offset)); if(( yystate >=
0? yystate <= 4792: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4793u,
yystate)] ==*(( short*) _check_unknown_subscript( yyss, sizeof( short),
yyssp_offset)): 0){ yystate=( int) Cyc_yytable[ _check_known_subscript_notnull(
4793u, yystate)];} else{ yystate=( int) Cyc_yydefgoto[
_check_known_subscript_notnull( 113u, yyn - 125)];} goto yynewstate; yyerrlab:
if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 798u, yystate)]; if( yyn > - 32768? yyn < 4792:
0){ int sze= 0; struct _tagged_arr msg; int x; int count; count= 0; for( x= yyn
< 0? - yyn: 0; x < 238u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4793u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 238u, x)]) + 15, count ++);}} msg=({
unsigned int _temp3457=( unsigned int)( sze + 15); unsigned char* _temp3458=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3457); struct
_tagged_arr _temp3460= _tag_arr( _temp3458, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3459= _temp3457; unsigned int i;
for( i= 0; i < _temp3459; i ++){ _temp3458[ i]='\000';}}; _temp3460;}); Cyc_String_strcpy(
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
yyssp_offset == 0){ return 1;} yyvsp_offset --; yystate=( int)*(( short*)
_check_unknown_subscript( yyss, sizeof( short), -- yyssp_offset)); yylsp_offset
--; yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 798u,
yystate)]; if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1:
yyn > 4792)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4793u, yyn)] != 1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4793u,
yyn)]; if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 797){ return 0;}*((
void**) _check_unknown_subscript( yyvs, sizeof( void*), ++ yyvsp_offset))= Cyc_yylval;*((
struct Cyc_Yyltype*) _check_unknown_subscript( yyls, sizeof( struct Cyc_Yyltype),
++ yylsp_offset))= Cyc_yylloc; goto yynewstate;} void Cyc_yyprint( int i, void*
v){ void* _temp3470= v; struct _tuple16* _temp3490; struct _tuple16 _temp3492;
int _temp3493; void* _temp3495; unsigned char _temp3497; short _temp3499; struct
_tagged_arr _temp3501; struct Cyc_Core_Opt* _temp3503; struct Cyc_Core_Opt*
_temp3505; struct Cyc_Core_Opt _temp3507; struct _tagged_arr* _temp3508; struct
_tuple1* _temp3510; struct _tuple1 _temp3512; struct _tagged_arr* _temp3513;
void* _temp3515; _LL3472: if( _temp3470 == Cyc_Okay_tok){ goto _LL3473;} else{
goto _LL3474;} _LL3474: if(*(( void**) _temp3470) == Cyc_Int_tok){ _LL3491:
_temp3490=(( struct Cyc_Int_tok_struct*) _temp3470)->f1; _temp3492=* _temp3490;
_LL3496: _temp3495= _temp3492.f1; goto _LL3494; _LL3494: _temp3493= _temp3492.f2;
goto _LL3475;} else{ goto _LL3476;} _LL3476: if(*(( void**) _temp3470) == Cyc_Char_tok){
_LL3498: _temp3497=(( struct Cyc_Char_tok_struct*) _temp3470)->f1; goto _LL3477;}
else{ goto _LL3478;} _LL3478: if(*(( void**) _temp3470) == Cyc_Short_tok){
_LL3500: _temp3499=(( struct Cyc_Short_tok_struct*) _temp3470)->f1; goto _LL3479;}
else{ goto _LL3480;} _LL3480: if(*(( void**) _temp3470) == Cyc_String_tok){
_LL3502: _temp3501=(( struct Cyc_String_tok_struct*) _temp3470)->f1; goto
_LL3481;} else{ goto _LL3482;} _LL3482: if(*(( void**) _temp3470) == Cyc_Stringopt_tok){
_LL3504: _temp3503=(( struct Cyc_Stringopt_tok_struct*) _temp3470)->f1; if(
_temp3503 == 0){ goto _LL3483;} else{ goto _LL3484;}} else{ goto _LL3484;}
_LL3484: if(*(( void**) _temp3470) == Cyc_Stringopt_tok){ _LL3506: _temp3505=((
struct Cyc_Stringopt_tok_struct*) _temp3470)->f1; if( _temp3505 == 0){ goto
_LL3486;} else{ _temp3507=* _temp3505; _LL3509: _temp3508=( struct _tagged_arr*)
_temp3507.v; goto _LL3485;}} else{ goto _LL3486;} _LL3486: if(*(( void**)
_temp3470) == Cyc_QualId_tok){ _LL3511: _temp3510=(( struct Cyc_QualId_tok_struct*)
_temp3470)->f1; _temp3512=* _temp3510; _LL3516: _temp3515= _temp3512.f1; goto
_LL3514; _LL3514: _temp3513= _temp3512.f2; goto _LL3487;} else{ goto _LL3488;}
_LL3488: goto _LL3489; _LL3473: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"ok");
goto _LL3471; _LL3475: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%d",
_temp3493); goto _LL3471; _LL3477: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%c",(
int) _temp3497); goto _LL3471; _LL3479: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%ds",(
int) _temp3499); goto _LL3471; _LL3481:({ struct _tagged_arr _temp3517=
_temp3501; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _get_arr_size(
_temp3517, 1u), _temp3517.curr);}); goto _LL3471; _LL3483: fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"null"); goto _LL3471; _LL3485:({ struct
_tagged_arr _temp3518=* _temp3508; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"",
_get_arr_size( _temp3518, 1u), _temp3518.curr);}); goto _LL3471; _LL3487: {
struct Cyc_List_List* _temp3519= 0; goto _LL3520; _LL3520:{ void* _temp3521=
_temp3515; struct Cyc_List_List* _temp3529; struct Cyc_List_List* _temp3531;
_LL3523: if(( unsigned int) _temp3521 > 1u?*(( int*) _temp3521) == Cyc_Absyn_Rel_n:
0){ _LL3530: _temp3529=(( struct Cyc_Absyn_Rel_n_struct*) _temp3521)->f1; goto
_LL3524;} else{ goto _LL3525;} _LL3525: if(( unsigned int) _temp3521 > 1u?*((
int*) _temp3521) == Cyc_Absyn_Abs_n: 0){ _LL3532: _temp3531=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3521)->f1; goto _LL3526;} else{ goto _LL3527;} _LL3527: if( _temp3521 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL3528;} else{ goto _LL3522;} _LL3524: _temp3519=
_temp3529; goto _LL3522; _LL3526: _temp3519= _temp3531; goto _LL3522; _LL3528:
goto _LL3522; _LL3522:;} for( 0; _temp3519 != 0; _temp3519=(( struct Cyc_List_List*)
_check_null( _temp3519))->tl){({ struct _tagged_arr _temp3533=*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp3519))->hd); fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"%.*s::", _get_arr_size( _temp3533, 1u),
_temp3533.curr);});}({ struct _tagged_arr _temp3534=* _temp3513; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"%.*s::", _get_arr_size( _temp3534, 1u),
_temp3534.curr);}); goto _LL3471;} _LL3489: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"?");
goto _LL3471; _LL3471:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3535=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3535->v=(
void*) Cyc_Lexing_from_file( f); _temp3535;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}