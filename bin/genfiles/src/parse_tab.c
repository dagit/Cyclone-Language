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
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct _tagged_arr
_temp17=( struct _tagged_arr) Cyc_Position_string_of_segment( sg); struct
_tagged_arr _temp18= msg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: %.*s\n",
_get_arr_size( _temp17, 1u), _temp17.curr, _get_arr_size( _temp18, 1u), _temp18.curr);});
return;} static void* Cyc_Parse_unimp( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ return Cyc_Parse_abort(( struct _tagged_arr)({ struct _tagged_arr _temp19=
msg; xprintf("%.*s unimplemented", _get_arr_size( _temp19, 1u), _temp19.curr);}),
sg);} static void Cyc_Parse_unimp2( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){({ struct _tagged_arr _temp20=( struct _tagged_arr) Cyc_Position_string_of_segment(
sg); struct _tagged_arr _temp21= msg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: Cyclone does not yet support %.*s\n",
_get_arr_size( _temp20, 1u), _temp20.curr, _get_arr_size( _temp21, 1u), _temp21.curr);});
return;} struct _tuple9{ struct _tuple7* f1; struct Cyc_Core_Opt* f2; } ; static
struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field( struct Cyc_Position_Segment*
loc, struct _tuple9* field_info){ struct Cyc_Core_Opt* _temp24; struct _tuple7*
_temp26; struct _tuple9 _temp22=* field_info; _LL27: _temp26= _temp22.f1; goto
_LL25; _LL25: _temp24= _temp22.f2; goto _LL23; _LL23: if((* _temp26).f4 != 0){
Cyc_Parse_err( _tag_arr("bad type params in struct field", sizeof( unsigned char),
32u), loc);}{ struct _tuple1* _temp28=(* _temp26).f1;{ void* _temp29=(* _temp28).f1;
struct Cyc_List_List* _temp39; struct Cyc_List_List* _temp41; _LL31: if((
unsigned int) _temp29 > 1u?*(( int*) _temp29) == Cyc_Absyn_Rel_n: 0){ _LL40:
_temp39=(( struct Cyc_Absyn_Rel_n_struct*) _temp29)->f1; if( _temp39 == 0){ goto
_LL32;} else{ goto _LL33;}} else{ goto _LL33;} _LL33: if(( unsigned int) _temp29
> 1u?*(( int*) _temp29) == Cyc_Absyn_Abs_n: 0){ _LL42: _temp41=(( struct Cyc_Absyn_Abs_n_struct*)
_temp29)->f1; if( _temp41 == 0){ goto _LL34;} else{ goto _LL35;}} else{ goto
_LL35;} _LL35: if( _temp29 ==( void*) Cyc_Absyn_Loc_n){ goto _LL36;} else{ goto
_LL37;} _LL37: goto _LL38; _LL32: goto _LL30; _LL34: goto _LL30; _LL36: goto
_LL30; _LL38: Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a module name",
sizeof( unsigned char), 52u), loc); goto _LL30; _LL30:;}{ struct Cyc_List_List*
_temp43=(* _temp26).f5; return({ struct Cyc_Absyn_Structfield* _temp44=( struct
Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp44->name=(* _temp28).f2; _temp44->tq=(* _temp26).f2; _temp44->type=( void*)(*
_temp26).f3; _temp44->width= _temp24; _temp44->attributes= _temp43; _temp44;});}}}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp47; struct Cyc_List_List* _temp48; void* _temp50; struct Cyc_Absyn_Tqual
_temp52; struct Cyc_Core_Opt* _temp54; struct _tuple10* _temp45= field; _temp47=*
_temp45; _LL55: _temp54= _temp47.f1; goto _LL53; _LL53: _temp52= _temp47.f2;
goto _LL51; _LL51: _temp50= _temp47.f3; goto _LL49; _LL49: _temp48= _temp47.f4;
goto _LL46; _LL46: { struct Cyc_Core_Opt* _temp56= 0; if( _temp54 != 0){ _temp56=({
struct Cyc_Core_Opt* _temp57=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp57->v=( void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp54))->v)).f2; _temp57;});{ void* _temp58=(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp54))->v)).f1; struct Cyc_List_List*
_temp68; struct Cyc_List_List* _temp70; _LL60: if(( unsigned int) _temp58 > 1u?*((
int*) _temp58) == Cyc_Absyn_Rel_n: 0){ _LL69: _temp68=(( struct Cyc_Absyn_Rel_n_struct*)
_temp58)->f1; if( _temp68 == 0){ goto _LL61;} else{ goto _LL62;}} else{ goto
_LL62;} _LL62: if(( unsigned int) _temp58 > 1u?*(( int*) _temp58) == Cyc_Absyn_Abs_n:
0){ _LL71: _temp70=(( struct Cyc_Absyn_Abs_n_struct*) _temp58)->f1; if( _temp70
== 0){ goto _LL63;} else{ goto _LL64;}} else{ goto _LL64;} _LL64: if( _temp58 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL65;} else{ goto _LL66;} _LL66: goto _LL67;
_LL61: goto _LL59; _LL63: goto _LL59; _LL65: goto _LL59; _LL67: Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a module name", sizeof(
unsigned char), 49u), loc); goto _LL59; _LL59:;}} if( _temp48 != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters", sizeof( unsigned char), 41u),
loc);} return({ struct _tuple2* _temp72=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp72->f1= _temp56; _temp72->f2= _temp52; _temp72->f3=
_temp50; _temp72;});}} static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment*
loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp73=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp73[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp74; _temp74.tag= Cyc_Parse_Type_spec; _temp74.f1=(
void*) t; _temp74.f2= loc; _temp74;}); _temp73;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp75= t; struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Tqual
_temp83; void* _temp85; _LL77: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75)
== Cyc_Absyn_ArrayType: 0){ _LL86: _temp85=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp75)->f1; goto _LL84; _LL84: _temp83=(( struct Cyc_Absyn_ArrayType_struct*)
_temp75)->f2; goto _LL82; _LL82: _temp81=(( struct Cyc_Absyn_ArrayType_struct*)
_temp75)->f3; goto _LL78;} else{ goto _LL79;} _LL79: goto _LL80; _LL78: return
Cyc_Absyn_starb_typ( _temp85,( void*) Cyc_Absyn_HeapRgn, _temp83, _temp81 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp87=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp87[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp88; _temp88.tag= Cyc_Absyn_Upper_b;
_temp88.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp81); _temp88;}); _temp87;}));
_LL80: return t; _LL76:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp89=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp89->f1=(* t).f2; _temp89->f2=(* t).f3;
_temp89;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp90=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp116; _LL92: if(*(( int*) _temp90) == Cyc_Absyn_Var_d){
_LL117: _temp116=(( struct Cyc_Absyn_Var_d_struct*) _temp90)->f1; goto _LL93;}
else{ goto _LL94;} _LL94: if(*(( int*) _temp90) == Cyc_Absyn_Let_d){ goto _LL95;}
else{ goto _LL96;} _LL96: if(*(( int*) _temp90) == Cyc_Absyn_Letv_d){ goto _LL97;}
else{ goto _LL98;} _LL98: if(*(( int*) _temp90) == Cyc_Absyn_Fn_d){ goto _LL99;}
else{ goto _LL100;} _LL100: if(*(( int*) _temp90) == Cyc_Absyn_Struct_d){ goto
_LL101;} else{ goto _LL102;} _LL102: if(*(( int*) _temp90) == Cyc_Absyn_Union_d){
goto _LL103;} else{ goto _LL104;} _LL104: if(*(( int*) _temp90) == Cyc_Absyn_Tunion_d){
goto _LL105;} else{ goto _LL106;} _LL106: if(*(( int*) _temp90) == Cyc_Absyn_Typedef_d){
goto _LL107;} else{ goto _LL108;} _LL108: if(*(( int*) _temp90) == Cyc_Absyn_Enum_d){
goto _LL109;} else{ goto _LL110;} _LL110: if(*(( int*) _temp90) == Cyc_Absyn_Namespace_d){
goto _LL111;} else{ goto _LL112;} _LL112: if(*(( int*) _temp90) == Cyc_Absyn_Using_d){
goto _LL113;} else{ goto _LL114;} _LL114: if(*(( int*) _temp90) == Cyc_Absyn_ExternC_d){
goto _LL115;} else{ goto _LL91;} _LL93: if( _temp116->initializer != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);}{ void* _temp118=(* _temp116->name).f1; struct
Cyc_List_List* _temp128; struct Cyc_List_List* _temp130; _LL120: if( _temp118 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL121;} else{ goto _LL122;} _LL122: if((
unsigned int) _temp118 > 1u?*(( int*) _temp118) == Cyc_Absyn_Rel_n: 0){ _LL129:
_temp128=(( struct Cyc_Absyn_Rel_n_struct*) _temp118)->f1; if( _temp128 == 0){
goto _LL123;} else{ goto _LL124;}} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp118 > 1u?*(( int*) _temp118) == Cyc_Absyn_Abs_n: 0){ _LL131: _temp130=((
struct Cyc_Absyn_Abs_n_struct*) _temp118)->f1; if( _temp130 == 0){ goto _LL125;}
else{ goto _LL126;}} else{ goto _LL126;} _LL126: goto _LL127; _LL121: goto
_LL119; _LL123: goto _LL119; _LL125: goto _LL119; _LL127: Cyc_Parse_err(
_tag_arr("module names not allowed on parameter declarations", sizeof(
unsigned char), 51u), x->loc); goto _LL119; _LL119:;}{ int found= 0; for( 0;
params != 0; params=(( struct Cyc_List_List*) _check_null( params))->tl){ if(
Cyc_String_zstrptrcmp((* _temp116->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( params))->hd) == 0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct _tagged_arr _temp132=*(* _temp116->name).f2;
xprintf("%.*s is not listed as a parameter", _get_arr_size( _temp132, 1u),
_temp132.curr);}), x->loc);} return;} _LL95: decl_kind= _tag_arr("let declaration",
sizeof( unsigned char), 16u); goto _LL91; _LL97: decl_kind= _tag_arr("let declaration",
sizeof( unsigned char), 16u); goto _LL91; _LL99: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL91; _LL101: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL91; _LL103: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL91; _LL105: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL91; _LL107: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL91; _LL109: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL91; _LL111: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL91; _LL113: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL91; _LL115: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL91; _LL91:;}(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_arr)({
struct _tagged_arr _temp133= decl_kind; xprintf("%.*s appears in parameter type",
_get_arr_size( _temp133, 1u), _temp133.curr);}), x->loc); return;} struct
_tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_arr* x){ struct _tuple12 _temp136; struct Cyc_Position_Segment* _temp137;
struct Cyc_List_List* _temp139; struct _tuple12* _temp134= env; _temp136=*
_temp134; _LL140: _temp139= _temp136.f1; goto _LL138; _LL138: _temp137= _temp136.f2;
goto _LL135; _LL135: if( _temp139 == 0){ return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct _tagged_arr _temp141=* x; xprintf("missing type for parameter %.*s",
_get_arr_size( _temp141, 1u), _temp141.curr);}), _temp137);}{ void* _temp142=(
void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp139))->hd)->r;
struct Cyc_Absyn_Vardecl* _temp148; _LL144: if(*(( int*) _temp142) == Cyc_Absyn_Var_d){
_LL149: _temp148=(( struct Cyc_Absyn_Var_d_struct*) _temp142)->f1; goto _LL145;}
else{ goto _LL146;} _LL146: goto _LL147; _LL145:{ void* _temp150=(* _temp148->name).f1;
struct Cyc_List_List* _temp160; struct Cyc_List_List* _temp162; _LL152: if(
_temp150 ==( void*) Cyc_Absyn_Loc_n){ goto _LL153;} else{ goto _LL154;} _LL154:
if(( unsigned int) _temp150 > 1u?*(( int*) _temp150) == Cyc_Absyn_Rel_n: 0){
_LL161: _temp160=(( struct Cyc_Absyn_Rel_n_struct*) _temp150)->f1; if( _temp160
== 0){ goto _LL155;} else{ goto _LL156;}} else{ goto _LL156;} _LL156: if((
unsigned int) _temp150 > 1u?*(( int*) _temp150) == Cyc_Absyn_Abs_n: 0){ _LL163:
_temp162=(( struct Cyc_Absyn_Abs_n_struct*) _temp150)->f1; if( _temp162 == 0){
goto _LL157;} else{ goto _LL158;}} else{ goto _LL158;} _LL158: goto _LL159;
_LL153: goto _LL151; _LL155: goto _LL151; _LL157: goto _LL151; _LL159: Cyc_Parse_err(
_tag_arr("module name not allowed on parameter", sizeof( unsigned char), 37u),
_temp137); goto _LL151; _LL151:;} if( Cyc_String_zstrptrcmp((* _temp148->name).f2,
x) == 0){ return({ struct _tuple2* _temp164=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2) * 1); _temp164[ 0]=({ struct _tuple2 _temp165; _temp165.f1=({
struct Cyc_Core_Opt* _temp166=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp166[ 0]=({ struct Cyc_Core_Opt _temp167; _temp167.v=(
void*)(* _temp148->name).f2; _temp167;}); _temp166;}); _temp165.f2= _temp148->tq;
_temp165.f3=( void*) _temp148->type; _temp165;}); _temp164;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple12* _temp168=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp168->f1=(( struct Cyc_List_List*) _check_null( _temp139))->tl; _temp168->f2=
_temp137; _temp168;}), x);} _LL147: return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration",
sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp139))->hd)->loc); _LL143:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp169= tm; _LL171: if(( unsigned int) _temp169 > 1u?*(( int*)
_temp169) == Cyc_Absyn_TypeParams_mod: 0){ goto _LL172;} else{ goto _LL173;}
_LL173: goto _LL174; _LL172: return 1; _LL174: return 0; _LL170:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) == 0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp175=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp175[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp176; _temp176.tag= Cyc_Absyn_VarType; _temp176.f1=({ struct Cyc_Absyn_Tvar*
_temp177=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp177->name=({ struct _tagged_arr* _temp178=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp178[ 0]= s; _temp178;}); _temp177->identity=
0; _temp177->kind= k; _temp177;}); _temp176;}); _temp175;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp179=
t; struct Cyc_Absyn_Tvar* _temp185; _LL181: if(( unsigned int) _temp179 > 4u?*((
int*) _temp179) == Cyc_Absyn_VarType: 0){ _LL186: _temp185=(( struct Cyc_Absyn_VarType_struct*)
_temp179)->f1; goto _LL182;} else{ goto _LL183;} _LL183: goto _LL184; _LL182:
return _temp185; _LL184: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL180:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp187=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp187[ 0]=({ struct Cyc_Absyn_VarType_struct _temp188; _temp188.tag= Cyc_Absyn_VarType;
_temp188.f1= pr; _temp188;}); _temp187;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp189=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp195; _LL191:
if(( unsigned int) _temp189 > 1u?*(( int*) _temp189) == Cyc_Absyn_Function_mod:
0){ _LL196: _temp195=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp189)->f1;
goto _LL192;} else{ goto _LL193;} _LL193: goto _LL194; _LL192: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp197= _temp195; struct Cyc_List_List* _temp203; _LL199: if(*((
int*) _temp197) == Cyc_Absyn_WithTypes){ goto _LL200;} else{ goto _LL201;}
_LL201: if(*(( int*) _temp197) == Cyc_Absyn_NoTypes){ _LL204: _temp203=(( struct
Cyc_Absyn_NoTypes_struct*) _temp197)->f1; goto _LL202;} else{ goto _LL198;}
_LL200: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL202:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp203,
tds);{ struct _tuple12* _temp205=({ struct _tuple12* _temp211=( struct _tuple12*)
GC_malloc( sizeof( struct _tuple12)); _temp211->f1= tds; _temp211->f2= loc;
_temp211;}); return({ struct Cyc_List_List* _temp206=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp206->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp207=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp207[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp208; _temp208.tag= Cyc_Absyn_Function_mod;
_temp208.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp209=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp209[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp210; _temp210.tag= Cyc_Absyn_WithTypes;
_temp210.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp205, _temp203); _temp210.f2= 0; _temp210.f3= 0;
_temp210.f4= 0; _temp210.f5= 0; _temp210;}); _temp209;})); _temp208;}); _temp207;}));
_temp206->tl= 0; _temp206;});} _LL198:;} else{ return({ struct Cyc_List_List*
_temp212=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp212->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp212->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp212;});} _LL194: return({ struct Cyc_List_List*
_temp213=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp213->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp213->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp213;}); _LL190:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp214=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp214->id= d->id; _temp214->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp214;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp215=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL217: if( _temp215 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL218;} else{ goto _LL219;} _LL219: if(
_temp215 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL220;} else{ goto _LL221;}
_LL221: if( _temp215 ==( void*) Cyc_Parse_Static_sc){ goto _LL222;} else{ goto
_LL223;} _LL223: goto _LL224; _LL218: sc=( void*) Cyc_Absyn_Extern; goto _LL216;
_LL220: sc=( void*) Cyc_Absyn_ExternC; goto _LL216; _LL222: sc=( void*) Cyc_Absyn_Static;
goto _LL216; _LL224: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL216; _LL216:;}}{ struct Cyc_Core_Opt*
_temp227; void* _temp229; struct _tuple5 _temp225= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL230: _temp229= _temp225.f1; goto _LL228; _LL228: _temp227=
_temp225.f2; goto _LL226; _LL226: { struct Cyc_List_List* _temp233; struct Cyc_List_List*
_temp235; void* _temp237; struct Cyc_Absyn_Tqual _temp239; struct _tuple6
_temp231= Cyc_Parse_apply_tms( tq, _temp229, atts, d->tms); _LL240: _temp239=
_temp231.f1; goto _LL238; _LL238: _temp237= _temp231.f2; goto _LL236; _LL236:
_temp235= _temp231.f3; goto _LL234; _LL234: _temp233= _temp231.f4; goto _LL232;
_LL232: if( _temp227 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp235 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp241= _temp237; struct Cyc_Absyn_FnInfo _temp247; struct Cyc_List_List*
_temp249; struct Cyc_List_List* _temp251; struct Cyc_Absyn_VarargInfo* _temp253;
int _temp255; struct Cyc_List_List* _temp257; void* _temp259; struct Cyc_Core_Opt*
_temp261; struct Cyc_List_List* _temp263; _LL243: if(( unsigned int) _temp241 >
4u?*(( int*) _temp241) == Cyc_Absyn_FnType: 0){ _LL248: _temp247=(( struct Cyc_Absyn_FnType_struct*)
_temp241)->f1; _LL264: _temp263= _temp247.tvars; goto _LL262; _LL262: _temp261=
_temp247.effect; goto _LL260; _LL260: _temp259=( void*) _temp247.ret_typ; goto
_LL258; _LL258: _temp257= _temp247.args; goto _LL256; _LL256: _temp255= _temp247.c_varargs;
goto _LL254; _LL254: _temp253= _temp247.cyc_varargs; goto _LL252; _LL252:
_temp251= _temp247.rgn_po; goto _LL250; _LL250: _temp249= _temp247.attributes;
goto _LL244;} else{ goto _LL245;} _LL245: goto _LL246; _LL244: { struct Cyc_List_List*
_temp265=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp257); return({ struct Cyc_Absyn_Fndecl*
_temp266=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp266->sc=( void*) sc; _temp266->name= d->id; _temp266->tvs= _temp263;
_temp266->is_inline= is_inline; _temp266->effect= _temp261; _temp266->ret_type=(
void*) _temp259; _temp266->args= _temp265; _temp266->c_varargs= _temp255;
_temp266->cyc_varargs= _temp253; _temp266->rgn_po= _temp251; _temp266->body=
body; _temp266->cached_typ= 0; _temp266->param_vardecls= 0; _temp266->attributes=
Cyc_List_append( _temp249, _temp233); _temp266;});} _LL246: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL242:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp267=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp267->f1=({ struct _tagged_arr*
_temp268=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp268[ 0]= _tag_arr("?", sizeof( unsigned char), 2u); _temp268;}); _temp267->f2=(*
t).f2; _temp267->f3=(* t).f3; _temp267;});} else{ return({ struct _tuple4*
_temp269=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp269->f1=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp269->f2=(*
t).f2; _temp269->f3=(* t).f3; _temp269;});}} static unsigned char _temp270[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp270, _temp270, _temp270 + 52u};
static unsigned char _temp271[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp271, _temp271, _temp271 + 63u};
static unsigned char _temp272[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp272, _temp272, _temp272 + 50u};
static unsigned char _temp273[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp273, _temp273, _temp273 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp274=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp288; void* _temp290; struct Cyc_Position_Segment*
_temp292; struct Cyc_Position_Segment* _temp294; struct Cyc_Position_Segment*
_temp296; struct Cyc_Position_Segment* _temp298; struct Cyc_Absyn_Decl* _temp300;
_LL276: if(*(( int*) _temp274) == Cyc_Parse_Type_spec){ _LL291: _temp290=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp274)->f1; goto _LL289; _LL289: _temp288=((
struct Cyc_Parse_Type_spec_struct*) _temp274)->f2; goto _LL277;} else{ goto
_LL278;} _LL278: if(*(( int*) _temp274) == Cyc_Parse_Signed_spec){ _LL293:
_temp292=(( struct Cyc_Parse_Signed_spec_struct*) _temp274)->f1; goto _LL279;}
else{ goto _LL280;} _LL280: if(*(( int*) _temp274) == Cyc_Parse_Unsigned_spec){
_LL295: _temp294=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp274)->f1; goto
_LL281;} else{ goto _LL282;} _LL282: if(*(( int*) _temp274) == Cyc_Parse_Short_spec){
_LL297: _temp296=(( struct Cyc_Parse_Short_spec_struct*) _temp274)->f1; goto
_LL283;} else{ goto _LL284;} _LL284: if(*(( int*) _temp274) == Cyc_Parse_Long_spec){
_LL299: _temp298=(( struct Cyc_Parse_Long_spec_struct*) _temp274)->f1; goto
_LL285;} else{ goto _LL286;} _LL286: if(*(( int*) _temp274) == Cyc_Parse_Decl_spec){
_LL301: _temp300=(( struct Cyc_Parse_Decl_spec_struct*) _temp274)->f1; goto
_LL287;} else{ goto _LL275;} _LL277: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp288);} last_loc= _temp288; seen_type= 1; t= _temp290; goto _LL275; _LL279:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp292);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp292);} last_loc= _temp292; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL275; _LL281: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp294);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp294);} last_loc= _temp294; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL275; _LL283: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp296);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp296);} last_loc= _temp296; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL275; _LL285: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp298);} if( seen_size){ void* _temp302= sz; _LL304: if( _temp302 ==( void*)
Cyc_Absyn_B4){ goto _LL305;} else{ goto _LL306;} _LL306: goto _LL307; _LL305: sz=(
void*) Cyc_Absyn_B8; goto _LL303; _LL307: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp298); goto _LL303; _LL303:;} last_loc=
_temp298; seen_size= 1; goto _LL275; _LL287: last_loc= _temp300->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp308=( void*) _temp300->r;
struct Cyc_Absyn_Structdecl* _temp320; struct Cyc_Absyn_Tuniondecl* _temp322;
struct Cyc_Absyn_Uniondecl* _temp324; struct Cyc_Absyn_Enumdecl* _temp326;
_LL310: if(*(( int*) _temp308) == Cyc_Absyn_Struct_d){ _LL321: _temp320=((
struct Cyc_Absyn_Struct_d_struct*) _temp308)->f1; goto _LL311;} else{ goto
_LL312;} _LL312: if(*(( int*) _temp308) == Cyc_Absyn_Tunion_d){ _LL323: _temp322=((
struct Cyc_Absyn_Tunion_d_struct*) _temp308)->f1; goto _LL313;} else{ goto
_LL314;} _LL314: if(*(( int*) _temp308) == Cyc_Absyn_Union_d){ _LL325: _temp324=((
struct Cyc_Absyn_Union_d_struct*) _temp308)->f1; goto _LL315;} else{ goto _LL316;}
_LL316: if(*(( int*) _temp308) == Cyc_Absyn_Enum_d){ _LL327: _temp326=(( struct
Cyc_Absyn_Enum_d_struct*) _temp308)->f1; goto _LL317;} else{ goto _LL318;}
_LL318: goto _LL319; _LL311: { struct Cyc_List_List* _temp328=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp320->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp329=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp329[ 0]=({ struct Cyc_Absyn_StructType_struct _temp330; _temp330.tag= Cyc_Absyn_StructType;
_temp330.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp320->name))->v); _temp330.f2= _temp328; _temp330.f3= 0;
_temp330;}); _temp329;}); if( _temp320->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp331=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp331->v=( void*) _temp300; _temp331;});} goto _LL309;} _LL313: { struct Cyc_List_List*
_temp332=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp322->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp333=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp333[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp334; _temp334.tag= Cyc_Absyn_TunionType;
_temp334.f1=({ struct Cyc_Absyn_TunionInfo _temp335; _temp335.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp336=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp336[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp337; _temp337.tag= Cyc_Absyn_KnownTunion;
_temp337.f1= _temp322; _temp337;}); _temp336;})); _temp335.targs= _temp332;
_temp335.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp335;}); _temp334;});
_temp333;}); if( _temp322->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp338=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp338->v=( void*) _temp300; _temp338;});} goto _LL309;} _LL315: { struct Cyc_List_List*
_temp339=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp324->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp340=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp340[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp341; _temp341.tag= Cyc_Absyn_UnionType; _temp341.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp324->name))->v); _temp341.f2= _temp339; _temp341.f3= 0; _temp341;});
_temp340;}); if( _temp324->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp342=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp342->v=( void*) _temp300; _temp342;});} goto _LL309;} _LL317: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp343=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp343[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp344; _temp344.tag= Cyc_Absyn_EnumType; _temp344.f1=
_temp326->name; _temp344.f2= 0; _temp344;}); _temp343;}); declopt=({ struct Cyc_Core_Opt*
_temp345=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp345->v=( void*) _temp300; _temp345;}); goto _LL309; _LL319:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp300->loc); goto _LL309; _LL309:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp300->loc);} goto _LL275; _LL275:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp346=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp346[ 0]=({ struct Cyc_Absyn_IntType_struct _temp347; _temp347.tag= Cyc_Absyn_IntType;
_temp347.f1=( void*) sgn; _temp347.f2=( void*) sz; _temp347;}); _temp346;});}
else{ if( seen_sign){ void* _temp348= t; void* _temp354; _LL350: if((
unsigned int) _temp348 > 4u?*(( int*) _temp348) == Cyc_Absyn_IntType: 0){ _LL355:
_temp354=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp348)->f2; goto _LL351;}
else{ goto _LL352;} _LL352: goto _LL353; _LL351: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp356=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp356[ 0]=({ struct Cyc_Absyn_IntType_struct _temp357; _temp357.tag= Cyc_Absyn_IntType;
_temp357.f1=( void*) sgn; _temp357.f2=( void*) _temp354; _temp357;}); _temp356;});
goto _LL349; _LL353: Cyc_Parse_err( _tag_arr("sign specification on non-integral type",
sizeof( unsigned char), 40u), last_loc); goto _LL349; _LL349:;} if( seen_size){
void* _temp358= t; void* _temp364; _LL360: if(( unsigned int) _temp358 > 4u?*((
int*) _temp358) == Cyc_Absyn_IntType: 0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp358)->f1; goto _LL361;} else{ goto _LL362;} _LL362: goto _LL363; _LL361: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp366=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp366[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp367; _temp367.tag= Cyc_Absyn_IntType; _temp367.f1=( void*) _temp364;
_temp367.f2=( void*) sz; _temp367;}); _temp366;}); goto _LL359; _LL363: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL359; _LL359:;}} return({ struct _tuple5 _temp368; _temp368.f1=
t; _temp368.f2= declopt; _temp368;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp369=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp370= _temp369->id; struct Cyc_List_List* _temp373; struct
Cyc_List_List* _temp375; void* _temp377; struct Cyc_Absyn_Tqual _temp379; struct
_tuple6 _temp371= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp369->tms);
_LL380: _temp379= _temp371.f1; goto _LL378; _LL378: _temp377= _temp371.f2; goto
_LL376; _LL376: _temp375= _temp371.f3; goto _LL374; _LL374: _temp373= _temp371.f4;
goto _LL372; _LL372: return({ struct Cyc_List_List* _temp381=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp381->hd=( void*)({ struct
_tuple7* _temp382=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp382->f1= _temp370; _temp382->f2= _temp379; _temp382->f3= _temp377; _temp382->f4=
_temp375; _temp382->f5= _temp373; _temp382;}); _temp381->tl= Cyc_Parse_apply_tmss(
_temp379, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp381;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp383; _temp383.f1= tq; _temp383.f2=
t; _temp383.f3= 0; _temp383.f4= atts; _temp383;});}{ void* _temp384=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp398;
void* _temp400; struct Cyc_Position_Segment* _temp402; struct Cyc_List_List*
_temp404; struct Cyc_Absyn_Tqual _temp406; void* _temp408; void* _temp410;
struct Cyc_List_List* _temp412; struct Cyc_Position_Segment* _temp414; _LL386:
if( _temp384 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL387;} else{ goto _LL388;}
_LL388: if(( unsigned int) _temp384 > 1u?*(( int*) _temp384) == Cyc_Absyn_ConstArray_mod:
0){ _LL399: _temp398=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp384)->f1;
goto _LL389;} else{ goto _LL390;} _LL390: if(( unsigned int) _temp384 > 1u?*((
int*) _temp384) == Cyc_Absyn_Function_mod: 0){ _LL401: _temp400=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp384)->f1; goto _LL391;} else{ goto
_LL392;} _LL392: if(( unsigned int) _temp384 > 1u?*(( int*) _temp384) == Cyc_Absyn_TypeParams_mod:
0){ _LL405: _temp404=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp384)->f1;
goto _LL403; _LL403: _temp402=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp384)->f2; goto _LL393;} else{ goto _LL394;} _LL394: if(( unsigned int)
_temp384 > 1u?*(( int*) _temp384) == Cyc_Absyn_Pointer_mod: 0){ _LL411: _temp410=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp384)->f1; goto _LL409;
_LL409: _temp408=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp384)->f2;
goto _LL407; _LL407: _temp406=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp384)->f3;
goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp384 > 1u?*((
int*) _temp384) == Cyc_Absyn_Attributes_mod: 0){ _LL415: _temp414=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp384)->f1; goto _LL413; _LL413: _temp412=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp384)->f2; goto _LL397;} else{ goto _LL385;} _LL387: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp416=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp416[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp417; _temp417.tag= Cyc_Absyn_ArrayType;
_temp417.f1=( void*) t; _temp417.f2= tq; _temp417.f3= 0; _temp417;}); _temp416;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL389: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp418=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp418[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp419; _temp419.tag= Cyc_Absyn_ArrayType;
_temp419.f1=( void*) t; _temp419.f2= tq; _temp419.f3=( struct Cyc_Absyn_Exp*)
_temp398; _temp419;}); _temp418;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL391: { void* _temp420= _temp400; struct Cyc_List_List* _temp426;
struct Cyc_Core_Opt* _temp428; struct Cyc_Absyn_VarargInfo* _temp430; int
_temp432; struct Cyc_List_List* _temp434; struct Cyc_Position_Segment* _temp436;
_LL422: if(*(( int*) _temp420) == Cyc_Absyn_WithTypes){ _LL435: _temp434=((
struct Cyc_Absyn_WithTypes_struct*) _temp420)->f1; goto _LL433; _LL433: _temp432=((
struct Cyc_Absyn_WithTypes_struct*) _temp420)->f2; goto _LL431; _LL431: _temp430=((
struct Cyc_Absyn_WithTypes_struct*) _temp420)->f3; goto _LL429; _LL429: _temp428=((
struct Cyc_Absyn_WithTypes_struct*) _temp420)->f4; goto _LL427; _LL427: _temp426=((
struct Cyc_Absyn_WithTypes_struct*) _temp420)->f5; goto _LL423;} else{ goto
_LL424;} _LL424: if(*(( int*) _temp420) == Cyc_Absyn_NoTypes){ _LL437: _temp436=((
struct Cyc_Absyn_NoTypes_struct*) _temp420)->f2; goto _LL425;} else{ goto _LL421;}
_LL423: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp438=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp438->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp438->tl= fn_atts; _temp438;});} else{ new_atts=({ struct Cyc_List_List*
_temp439=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp439->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp439->tl= new_atts; _temp439;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp440=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp446; _LL442: if(( unsigned int) _temp440 > 1u?*(( int*) _temp440) == Cyc_Absyn_TypeParams_mod:
0){ _LL447: _temp446=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp440)->f1;
goto _LL443;} else{ goto _LL444;} _LL444: goto _LL445; _LL443: typvars= _temp446;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL441; _LL445: goto
_LL441; _LL441:;} if((((( ! _temp432? _temp430 == 0: 0)? _temp434 != 0: 0)?((
struct Cyc_List_List*) _check_null( _temp434))->tl == 0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp434))->hd)).f1 == 0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp434))->hd)).f3 ==( void*)
Cyc_Absyn_VoidType: 0){ _temp434= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp434); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp428, t, _temp434, _temp432, _temp430, _temp426, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL425:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp436)); _LL421:;} _LL393: if((( struct Cyc_List_List*) _check_null(
tms))->tl == 0){ return({ struct _tuple6 _temp448; _temp448.f1= tq; _temp448.f2=
t; _temp448.f3= _temp404; _temp448.f4= atts; _temp448;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp402)); _LL395: { void* _temp449= _temp410;
struct Cyc_Absyn_Exp* _temp457; struct Cyc_Absyn_Exp* _temp459; _LL451: if((
unsigned int) _temp449 > 1u?*(( int*) _temp449) == Cyc_Absyn_NonNullable_ps: 0){
_LL458: _temp457=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp449)->f1; goto
_LL452;} else{ goto _LL453;} _LL453: if(( unsigned int) _temp449 > 1u?*(( int*)
_temp449) == Cyc_Absyn_Nullable_ps: 0){ _LL460: _temp459=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp449)->f1; goto _LL454;} else{ goto _LL455;} _LL455: if( _temp449 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL456;} else{ goto _LL450;} _LL452: return Cyc_Parse_apply_tms(
_temp406, Cyc_Absyn_atb_typ( t, _temp408, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp461=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp461[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp462; _temp462.tag= Cyc_Absyn_Upper_b;
_temp462.f1= _temp457; _temp462;}); _temp461;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL454: return Cyc_Parse_apply_tms( _temp406, Cyc_Absyn_starb_typ(
t, _temp408, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp463=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp463[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp464; _temp464.tag= Cyc_Absyn_Upper_b;
_temp464.f1= _temp459; _temp464;}); _temp463;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL456: return Cyc_Parse_apply_tms( _temp406, Cyc_Absyn_tagged_typ(
t, _temp408, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL450:;}
_LL397: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp412),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL385:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp467; void* _temp469; struct _tuple5 _temp465= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL470: _temp469= _temp465.f1; goto _LL468; _LL468: _temp467=
_temp465.f2; goto _LL466; _LL466: if( _temp467 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp469;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp471=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp471[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp472; _temp472.tag= Cyc_Absyn_Decl_s;
_temp472.f1= d; _temp472.f2= s; _temp472;}); _temp471;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc != 0){ void* _temp473=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL475: if( _temp473 ==(
void*) Cyc_Parse_Typedef_sc){ goto _LL476;} else{ goto _LL477;} _LL477: if(
_temp473 ==( void*) Cyc_Parse_Extern_sc){ goto _LL478;} else{ goto _LL479;}
_LL479: if( _temp473 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL480;} else{ goto
_LL481;} _LL481: if( _temp473 ==( void*) Cyc_Parse_Static_sc){ goto _LL482;}
else{ goto _LL483;} _LL483: if( _temp473 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL484;} else{ goto _LL485;} _LL485: if( _temp473 ==( void*) Cyc_Parse_Register_sc){
goto _LL486;} else{ goto _LL487;} _LL487: if( _temp473 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL488;} else{ goto _LL474;} _LL476: istypedef= 1; goto _LL474; _LL478: s=(
void*) Cyc_Absyn_Extern; goto _LL474; _LL480: s=( void*) Cyc_Absyn_ExternC; goto
_LL474; _LL482: s=( void*) Cyc_Absyn_Static; goto _LL474; _LL484: s=( void*) Cyc_Absyn_Public;
goto _LL474; _LL486: s=( void*) Cyc_Absyn_Public; goto _LL474; _LL488: s=( void*)
Cyc_Absyn_Abstract; goto _LL474; _LL474:;}{ struct Cyc_List_List* _temp491;
struct Cyc_List_List* _temp493; struct _tuple0 _temp489=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL494: _temp493= _temp489.f1;
goto _LL492; _LL492: _temp491= _temp489.f2; goto _LL490; _LL490: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp491; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp495= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp493 == 0){ struct Cyc_Core_Opt* _temp498; void* _temp500; struct _tuple5
_temp496= _temp495; _LL501: _temp500= _temp496.f1; goto _LL499; _LL499: _temp498=
_temp496.f2; goto _LL497; _LL497: if( _temp498 != 0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp498))->v;{
void* _temp502=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp514; struct Cyc_Absyn_Structdecl*
_temp516; struct Cyc_Absyn_Uniondecl* _temp518; struct Cyc_Absyn_Tuniondecl*
_temp520; _LL504: if(*(( int*) _temp502) == Cyc_Absyn_Enum_d){ _LL515: _temp514=((
struct Cyc_Absyn_Enum_d_struct*) _temp502)->f1; goto _LL505;} else{ goto _LL506;}
_LL506: if(*(( int*) _temp502) == Cyc_Absyn_Struct_d){ _LL517: _temp516=((
struct Cyc_Absyn_Struct_d_struct*) _temp502)->f1; goto _LL507;} else{ goto
_LL508;} _LL508: if(*(( int*) _temp502) == Cyc_Absyn_Union_d){ _LL519: _temp518=((
struct Cyc_Absyn_Union_d_struct*) _temp502)->f1; goto _LL509;} else{ goto _LL510;}
_LL510: if(*(( int*) _temp502) == Cyc_Absyn_Tunion_d){ _LL521: _temp520=((
struct Cyc_Absyn_Tunion_d_struct*) _temp502)->f1; goto _LL511;} else{ goto
_LL512;} _LL512: goto _LL513; _LL505:( void*)( _temp514->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL503; _LL507:( void*)( _temp516->sc=( void*)
s); _temp516->attributes= atts; goto _LL503; _LL509:( void*)( _temp518->sc=(
void*) s); _temp518->attributes= atts; goto _LL503; _LL511:( void*)( _temp520->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL503; _LL513: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL503:;}
return({ struct Cyc_List_List* _temp522=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp522->hd=( void*) d; _temp522->tl= 0;
_temp522;});} else{ void* _temp523= _temp500; struct Cyc_List_List* _temp537;
struct _tuple1* _temp539; struct Cyc_Absyn_TunionInfo _temp541; void* _temp543;
struct Cyc_Absyn_Tuniondecl* _temp545; struct Cyc_Absyn_TunionInfo _temp547;
struct Cyc_List_List* _temp549; void* _temp551; struct Cyc_Absyn_UnknownTunionInfo
_temp553; int _temp555; struct _tuple1* _temp557; struct Cyc_List_List* _temp559;
struct _tuple1* _temp561; struct _tuple1* _temp563; _LL525: if(( unsigned int)
_temp523 > 4u?*(( int*) _temp523) == Cyc_Absyn_StructType: 0){ _LL540: _temp539=((
struct Cyc_Absyn_StructType_struct*) _temp523)->f1; goto _LL538; _LL538:
_temp537=(( struct Cyc_Absyn_StructType_struct*) _temp523)->f2; goto _LL526;}
else{ goto _LL527;} _LL527: if(( unsigned int) _temp523 > 4u?*(( int*) _temp523)
== Cyc_Absyn_TunionType: 0){ _LL542: _temp541=(( struct Cyc_Absyn_TunionType_struct*)
_temp523)->f1; _LL544: _temp543=( void*) _temp541.tunion_info; if(*(( int*)
_temp543) == Cyc_Absyn_KnownTunion){ _LL546: _temp545=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp543)->f1; goto _LL528;} else{ goto _LL529;}} else{ goto _LL529;} _LL529:
if(( unsigned int) _temp523 > 4u?*(( int*) _temp523) == Cyc_Absyn_TunionType: 0){
_LL548: _temp547=(( struct Cyc_Absyn_TunionType_struct*) _temp523)->f1; _LL552:
_temp551=( void*) _temp547.tunion_info; if(*(( int*) _temp551) == Cyc_Absyn_UnknownTunion){
_LL554: _temp553=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp551)->f1;
_LL558: _temp557= _temp553.name; goto _LL556; _LL556: _temp555= _temp553.is_xtunion;
goto _LL550;} else{ goto _LL531;} _LL550: _temp549= _temp547.targs; goto _LL530;}
else{ goto _LL531;} _LL531: if(( unsigned int) _temp523 > 4u?*(( int*) _temp523)
== Cyc_Absyn_UnionType: 0){ _LL562: _temp561=(( struct Cyc_Absyn_UnionType_struct*)
_temp523)->f1; goto _LL560; _LL560: _temp559=(( struct Cyc_Absyn_UnionType_struct*)
_temp523)->f2; goto _LL532;} else{ goto _LL533;} _LL533: if(( unsigned int)
_temp523 > 4u?*(( int*) _temp523) == Cyc_Absyn_EnumType: 0){ _LL564: _temp563=((
struct Cyc_Absyn_EnumType_struct*) _temp523)->f1; goto _LL534;} else{ goto
_LL535;} _LL535: goto _LL536; _LL526: { struct Cyc_List_List* _temp565=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp537); struct Cyc_Absyn_Structdecl* _temp566=({
struct Cyc_Absyn_Structdecl* _temp570=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp570->sc=( void*) s; _temp570->name=({
struct Cyc_Core_Opt* _temp571=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp571->v=( void*)(( struct _tuple1*) _check_null( _temp539));
_temp571;}); _temp570->tvs= _temp565; _temp570->fields= 0; _temp570->attributes=
0; _temp570;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp567=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp567->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp568=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp568[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp569; _temp569.tag= Cyc_Absyn_Struct_d;
_temp569.f1= _temp566; _temp569;}); _temp568;}), loc); _temp567->tl= 0; _temp567;});}
_LL528: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp572=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp572->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp573=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp573[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp574; _temp574.tag= Cyc_Absyn_Tunion_d;
_temp574.f1= _temp545; _temp574;}); _temp573;}), loc); _temp572->tl= 0; _temp572;});
_LL530: { struct Cyc_List_List* _temp575=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp549);
struct Cyc_Absyn_Decl* _temp576= Cyc_Absyn_tunion_decl( s, _temp557, _temp575, 0,
_temp555, loc); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp577=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp577->hd=(
void*) _temp576; _temp577->tl= 0; _temp577;});} _LL532: { struct Cyc_List_List*
_temp578=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp559); struct Cyc_Absyn_Uniondecl* _temp579=({
struct Cyc_Absyn_Uniondecl* _temp584=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp584->sc=( void*) s; _temp584->name=({
struct Cyc_Core_Opt* _temp585=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp585->v=( void*)(( struct _tuple1*) _check_null( _temp561));
_temp585;}); _temp584->tvs= _temp578; _temp584->fields= 0; _temp584->attributes=
0; _temp584;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp580=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp580->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp581=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp581->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp582=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp582[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp583; _temp583.tag= Cyc_Absyn_Union_d;
_temp583.f1= _temp579; _temp583;}); _temp582;})); _temp581->loc= loc; _temp581;});
_temp580->tl= 0; _temp580;});} _LL534: { struct Cyc_Absyn_Enumdecl* _temp586=({
struct Cyc_Absyn_Enumdecl* _temp591=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp591->sc=( void*) s; _temp591->name=
_temp563; _temp591->fields= 0; _temp591;}); if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp587=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp587->hd=( void*)({ struct Cyc_Absyn_Decl* _temp588=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp588->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp589=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp589[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp590; _temp590.tag= Cyc_Absyn_Enum_d; _temp590.f1= _temp586; _temp590;});
_temp589;})); _temp588->loc= loc; _temp588;}); _temp587->tl= 0; _temp587;});}
_LL536: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL524:;}} else{ void* t= _temp495.f1; struct Cyc_List_List*
_temp592= Cyc_Parse_apply_tmss( tq, t, _temp493, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp592); if( _temp495.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp495.f2))->v;{ void* _temp593=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp605; struct Cyc_Absyn_Tuniondecl* _temp607;
struct Cyc_Absyn_Uniondecl* _temp609; struct Cyc_Absyn_Enumdecl* _temp611;
_LL595: if(*(( int*) _temp593) == Cyc_Absyn_Struct_d){ _LL606: _temp605=((
struct Cyc_Absyn_Struct_d_struct*) _temp593)->f1; goto _LL596;} else{ goto
_LL597;} _LL597: if(*(( int*) _temp593) == Cyc_Absyn_Tunion_d){ _LL608: _temp607=((
struct Cyc_Absyn_Tunion_d_struct*) _temp593)->f1; goto _LL598;} else{ goto
_LL599;} _LL599: if(*(( int*) _temp593) == Cyc_Absyn_Union_d){ _LL610: _temp609=((
struct Cyc_Absyn_Union_d_struct*) _temp593)->f1; goto _LL600;} else{ goto _LL601;}
_LL601: if(*(( int*) _temp593) == Cyc_Absyn_Enum_d){ _LL612: _temp611=(( struct
Cyc_Absyn_Enum_d_struct*) _temp593)->f1; goto _LL602;} else{ goto _LL603;}
_LL603: goto _LL604; _LL596:( void*)( _temp605->sc=( void*) s); _temp605->attributes=
atts; atts= 0; goto _LL594; _LL598:( void*)( _temp607->sc=( void*) s); goto
_LL594; _LL600:( void*)( _temp609->sc=( void*) s); goto _LL594; _LL602:( void*)(
_temp611->sc=( void*) s); goto _LL594; _LL604: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL594; _LL594:;} decls=({ struct Cyc_List_List*
_temp613=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp613->hd=( void*) d; _temp613->tl= decls; _temp613;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct _tagged_arr _temp614= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in typedef",
_get_arr_size( _temp614, 1u), _temp614.curr);}), loc);} return decls;}} else{
if( _temp495.f2 != 0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp615= _temp592; for( 0; _temp615 != 0;( _temp615=(( struct
Cyc_List_List*) _check_null( _temp615))->tl, _temp491=(( struct Cyc_List_List*)
_check_null( _temp491))->tl)){ struct _tuple7 _temp618; struct Cyc_List_List*
_temp619; struct Cyc_List_List* _temp621; void* _temp623; struct Cyc_Absyn_Tqual
_temp625; struct _tuple1* _temp627; struct _tuple7* _temp616=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp615))->hd; _temp618=* _temp616; _LL628:
_temp627= _temp618.f1; goto _LL626; _LL626: _temp625= _temp618.f2; goto _LL624;
_LL624: _temp623= _temp618.f3; goto _LL622; _LL622: _temp621= _temp618.f4; goto
_LL620; _LL620: _temp619= _temp618.f5; goto _LL617; _LL617: if( _temp621 != 0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp491 == 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp629=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp491))->hd; struct Cyc_Absyn_Vardecl*
_temp630= Cyc_Absyn_new_vardecl( _temp627, _temp623, _temp629); _temp630->tq=
_temp625;( void*)( _temp630->sc=( void*) s); _temp630->attributes= _temp619;{
struct Cyc_Absyn_Decl* _temp631=({ struct Cyc_Absyn_Decl* _temp633=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp633->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp634=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp634[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp635; _temp635.tag= Cyc_Absyn_Var_d; _temp635.f1= _temp630; _temp635;});
_temp634;})); _temp633->loc= loc; _temp633;}); decls=({ struct Cyc_List_List*
_temp632=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp632->hd=( void*) _temp631; _temp632->tl= decls; _temp632;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({
struct _tagged_arr _temp636= s; xprintf("bad kind: %.*s", _get_arr_size(
_temp636, 1u), _temp636.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;} else{
switch(*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), 0))){ case 'A': _LL637: return( void*) Cyc_Absyn_AnyKind; case 'M':
_LL638: return( void*) Cyc_Absyn_MemKind; case 'B': _LL639: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL640: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL641: return(
void*) Cyc_Absyn_EffKind; default: _LL642: Cyc_Parse_err(( struct _tagged_arr)({
struct _tagged_arr _temp644= s; xprintf("bad kind: %.*s", _get_arr_size(
_temp644, 1u), _temp644.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp645=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp645->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp646=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp646[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp647; _temp647.tag= Cyc_Absyn_Attributes_mod;
_temp647.f1= loc; _temp647.f2= atts; _temp647;}); _temp646;})); _temp645->tl=
tms; _temp645;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(( struct
_tagged_arr)({ struct _tagged_arr _temp648= Cyc_Absyn_attribute2string(( void*)((
struct Cyc_List_List*) _check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_get_arr_size( _temp648, 1u), _temp648.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp649=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp649[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp650; _temp650.tag= Cyc_Absyn_Typedef_d; _temp650.f1=({
struct Cyc_Absyn_Typedefdecl* _temp651=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp651->name= x; _temp651->tvs=(*
t).f4; _temp651->defn=( void*)(* t).f3; _temp651;}); _temp650;}); _temp649;}),
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
unsigned char _temp653[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp653, _temp653, _temp653 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp654= yy1; struct _tuple16* _temp660; _LL656: if(*(( void**)
_temp654) == Cyc_Int_tok){ _LL661: _temp660=(( struct Cyc_Int_tok_struct*)
_temp654)->f1; goto _LL657;} else{ goto _LL658;} _LL658: goto _LL659; _LL657:
return _temp660; _LL659:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL655:;}
static unsigned char _temp663[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp663, _temp663, _temp663 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp664= yy1; struct
_tagged_arr _temp670; _LL666: if(*(( void**) _temp664) == Cyc_String_tok){
_LL671: _temp670=(( struct Cyc_String_tok_struct*) _temp664)->f1; goto _LL667;}
else{ goto _LL668;} _LL668: goto _LL669; _LL667: return _temp670; _LL669:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL665:;} static unsigned char _temp673[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp673, _temp673, _temp673 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp674= yy1; unsigned char _temp680; _LL676: if(*(( void**) _temp674) ==
Cyc_Char_tok){ _LL681: _temp680=(( struct Cyc_Char_tok_struct*) _temp674)->f1;
goto _LL677;} else{ goto _LL678;} _LL678: goto _LL679; _LL677: return _temp680;
_LL679:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL675:;} static
unsigned char _temp683[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp683, _temp683, _temp683 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp684= yy1; void*
_temp690; _LL686: if(*(( void**) _temp684) == Cyc_Pointer_Sort_tok){ _LL691:
_temp690=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp684)->f1; goto
_LL687;} else{ goto _LL688;} _LL688: goto _LL689; _LL687: return _temp690;
_LL689:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL685:;} static
unsigned char _temp693[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp693, _temp693, _temp693 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp694= yy1; struct Cyc_Absyn_Exp*
_temp700; _LL696: if(*(( void**) _temp694) == Cyc_Exp_tok){ _LL701: _temp700=((
struct Cyc_Exp_tok_struct*) _temp694)->f1; goto _LL697;} else{ goto _LL698;}
_LL698: goto _LL699; _LL697: return _temp700; _LL699:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL695:;} static unsigned char _temp703[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp703, _temp703, _temp703 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp704= yy1;
struct Cyc_List_List* _temp710; _LL706: if(*(( void**) _temp704) == Cyc_ExpList_tok){
_LL711: _temp710=(( struct Cyc_ExpList_tok_struct*) _temp704)->f1; goto _LL707;}
else{ goto _LL708;} _LL708: goto _LL709; _LL707: return _temp710; _LL709:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL705:;} static unsigned char
_temp713[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp713, _temp713, _temp713 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp714= yy1; struct Cyc_List_List*
_temp720; _LL716: if(*(( void**) _temp714) == Cyc_InitializerList_tok){ _LL721:
_temp720=(( struct Cyc_InitializerList_tok_struct*) _temp714)->f1; goto _LL717;}
else{ goto _LL718;} _LL718: goto _LL719; _LL717: return _temp720; _LL719:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL715:;} static unsigned char
_temp723[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp723, _temp723, _temp723 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp724= yy1; void* _temp730; _LL726: if(*(( void**) _temp724)
== Cyc_Primop_tok){ _LL731: _temp730=( void*)(( struct Cyc_Primop_tok_struct*)
_temp724)->f1; goto _LL727;} else{ goto _LL728;} _LL728: goto _LL729; _LL727:
return _temp730; _LL729:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL725:;}
static unsigned char _temp733[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp733, _temp733, _temp733 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp734= yy1;
struct Cyc_Core_Opt* _temp740; _LL736: if(*(( void**) _temp734) == Cyc_Primopopt_tok){
_LL741: _temp740=(( struct Cyc_Primopopt_tok_struct*) _temp734)->f1; goto _LL737;}
else{ goto _LL738;} _LL738: goto _LL739; _LL737: return _temp740; _LL739:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL735:;} static unsigned char
_temp743[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp743, _temp743, _temp743 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp744= yy1; struct _tuple1* _temp750; _LL746: if(*(( void**)
_temp744) == Cyc_QualId_tok){ _LL751: _temp750=(( struct Cyc_QualId_tok_struct*)
_temp744)->f1; goto _LL747;} else{ goto _LL748;} _LL748: goto _LL749; _LL747:
return _temp750; _LL749:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL745:;}
static unsigned char _temp753[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp753, _temp753, _temp753 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp754= yy1;
struct Cyc_Absyn_Stmt* _temp760; _LL756: if(*(( void**) _temp754) == Cyc_Stmt_tok){
_LL761: _temp760=(( struct Cyc_Stmt_tok_struct*) _temp754)->f1; goto _LL757;}
else{ goto _LL758;} _LL758: goto _LL759; _LL757: return _temp760; _LL759:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL755:;} static unsigned char _temp763[
14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp763, _temp763, _temp763 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp764= yy1; void* _temp770; _LL766: if(*(( void**) _temp764)
== Cyc_BlockItem_tok){ _LL771: _temp770=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp764)->f1; goto _LL767;} else{ goto _LL768;} _LL768: goto _LL769; _LL767:
return _temp770; _LL769:( int) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL765:;} static unsigned char _temp773[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp773, _temp773, _temp773 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp774= yy1; struct Cyc_List_List* _temp780; _LL776: if(*((
void**) _temp774) == Cyc_SwitchClauseList_tok){ _LL781: _temp780=(( struct Cyc_SwitchClauseList_tok_struct*)
_temp774)->f1; goto _LL777;} else{ goto _LL778;} _LL778: goto _LL779; _LL777:
return _temp780; _LL779:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL775:;} static unsigned char _temp783[ 22u]="SwitchCClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp783, _temp783, _temp783 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ void* _temp784= yy1; struct Cyc_List_List* _temp790; _LL786: if(*((
void**) _temp784) == Cyc_SwitchCClauseList_tok){ _LL791: _temp790=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp784)->f1; goto _LL787;} else{ goto _LL788;} _LL788: goto _LL789; _LL787:
return _temp790; _LL789:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL785:;} static unsigned char _temp793[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp793, _temp793, _temp793 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp794= yy1;
struct Cyc_Absyn_Pat* _temp800; _LL796: if(*(( void**) _temp794) == Cyc_Pattern_tok){
_LL801: _temp800=(( struct Cyc_Pattern_tok_struct*) _temp794)->f1; goto _LL797;}
else{ goto _LL798;} _LL798: goto _LL799; _LL797: return _temp800; _LL799:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL795:;} static unsigned char
_temp803[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp803, _temp803, _temp803 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp804= yy1; struct Cyc_List_List*
_temp810; _LL806: if(*(( void**) _temp804) == Cyc_PatternList_tok){ _LL811:
_temp810=(( struct Cyc_PatternList_tok_struct*) _temp804)->f1; goto _LL807;}
else{ goto _LL808;} _LL808: goto _LL809; _LL807: return _temp810; _LL809:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL805:;} static unsigned char
_temp813[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp813, _temp813, _temp813 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp814= yy1; struct _tuple14* _temp820; _LL816: if(*(( void**)
_temp814) == Cyc_FieldPattern_tok){ _LL821: _temp820=(( struct Cyc_FieldPattern_tok_struct*)
_temp814)->f1; goto _LL817;} else{ goto _LL818;} _LL818: goto _LL819; _LL817:
return _temp820; _LL819:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL815:;} static unsigned char _temp823[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp823, _temp823, _temp823 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp824= yy1; struct Cyc_List_List* _temp830; _LL826: if(*((
void**) _temp824) == Cyc_FieldPatternList_tok){ _LL831: _temp830=(( struct Cyc_FieldPatternList_tok_struct*)
_temp824)->f1; goto _LL827;} else{ goto _LL828;} _LL828: goto _LL829; _LL827:
return _temp830; _LL829:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL825:;} static unsigned char _temp833[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp833, _temp833, _temp833 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp834= yy1;
struct Cyc_Absyn_Fndecl* _temp840; _LL836: if(*(( void**) _temp834) == Cyc_FnDecl_tok){
_LL841: _temp840=(( struct Cyc_FnDecl_tok_struct*) _temp834)->f1; goto _LL837;}
else{ goto _LL838;} _LL838: goto _LL839; _LL837: return _temp840; _LL839:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL835:;} static unsigned char _temp843[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp843, _temp843, _temp843 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp844= yy1; struct Cyc_List_List*
_temp850; _LL846: if(*(( void**) _temp844) == Cyc_DeclList_tok){ _LL851:
_temp850=(( struct Cyc_DeclList_tok_struct*) _temp844)->f1; goto _LL847;} else{
goto _LL848;} _LL848: goto _LL849; _LL847: return _temp850; _LL849:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL845:;} static unsigned char _temp853[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp853, _temp853, _temp853 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp854= yy1; struct Cyc_Parse_Declaration_spec* _temp860;
_LL856: if(*(( void**) _temp854) == Cyc_DeclSpec_tok){ _LL861: _temp860=((
struct Cyc_DeclSpec_tok_struct*) _temp854)->f1; goto _LL857;} else{ goto _LL858;}
_LL858: goto _LL859; _LL857: return _temp860; _LL859:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL855:;} static unsigned char _temp863[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp863, _temp863, _temp863 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp864= yy1; struct
_tuple15* _temp870; _LL866: if(*(( void**) _temp864) == Cyc_InitDecl_tok){
_LL871: _temp870=(( struct Cyc_InitDecl_tok_struct*) _temp864)->f1; goto _LL867;}
else{ goto _LL868;} _LL868: goto _LL869; _LL867: return _temp870; _LL869:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL865:;} static unsigned char
_temp873[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp873, _temp873, _temp873 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp874= yy1; struct Cyc_List_List*
_temp880; _LL876: if(*(( void**) _temp874) == Cyc_InitDeclList_tok){ _LL881:
_temp880=(( struct Cyc_InitDeclList_tok_struct*) _temp874)->f1; goto _LL877;}
else{ goto _LL878;} _LL878: goto _LL879; _LL877: return _temp880; _LL879:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL875:;} static unsigned char
_temp883[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp883, _temp883, _temp883 + 17u}}; void* Cyc_yyget_StorageClass_tok(
void* yy1){ void* _temp884= yy1; void* _temp890; _LL886: if(*(( void**) _temp884)
== Cyc_StorageClass_tok){ _LL891: _temp890=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp884)->f1; goto _LL887;} else{ goto _LL888;} _LL888: goto _LL889; _LL887:
return _temp890; _LL889:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL885:;} static unsigned char _temp893[ 18u]="TypeSpecifier_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp893, _temp893, _temp893 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp894= yy1; void* _temp900; _LL896: if(*(( void**) _temp894) ==
Cyc_TypeSpecifier_tok){ _LL901: _temp900=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp894)->f1; goto _LL897;} else{ goto _LL898;} _LL898: goto _LL899; _LL897:
return _temp900; _LL899:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL895:;} static unsigned char _temp903[ 18u]="StructOrUnion_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp903, _temp903, _temp903 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp904= yy1; void* _temp910; _LL906: if(*(( void**) _temp904) ==
Cyc_StructOrUnion_tok){ _LL911: _temp910=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp904)->f1; goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907:
return _temp910; _LL909:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL905:;} static unsigned char _temp913[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp913, _temp913, _temp913 + 13u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ void* _temp914= yy1;
struct Cyc_Absyn_Tqual _temp920; _LL916: if(*(( void**) _temp914) == Cyc_TypeQual_tok){
_LL921: _temp920=(( struct Cyc_TypeQual_tok_struct*) _temp914)->f1; goto _LL917;}
else{ goto _LL918;} _LL918: goto _LL919; _LL917: return _temp920; _LL919:( int)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL915:;} static unsigned char
_temp923[ 24u]="StructFieldDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{ _temp923, _temp923,
_temp923 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok( void*
yy1){ void* _temp924= yy1; struct Cyc_List_List* _temp930; _LL926: if(*(( void**)
_temp924) == Cyc_StructFieldDeclList_tok){ _LL931: _temp930=(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp924)->f1; goto _LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL927:
return _temp930; _LL929:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL925:;} static unsigned char _temp933[ 28u]="StructFieldDeclListList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp933, _temp933, _temp933 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ void* _temp934= yy1; struct
Cyc_List_List* _temp940; _LL936: if(*(( void**) _temp934) == Cyc_StructFieldDeclListList_tok){
_LL941: _temp940=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp934)->f1;
goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937: return _temp940;
_LL939:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL935:;}
static unsigned char _temp943[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp943, _temp943,
_temp943 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp944= yy1; struct Cyc_List_List* _temp950; _LL946: if(*(( void**)
_temp944) == Cyc_TypeModifierList_tok){ _LL951: _temp950=(( struct Cyc_TypeModifierList_tok_struct*)
_temp944)->f1; goto _LL947;} else{ goto _LL948;} _LL948: goto _LL949; _LL947:
return _temp950; _LL949:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL945:;} static unsigned char _temp953[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp953, _temp953, _temp953 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp954= yy1; void* _temp960; _LL956:
if(*(( void**) _temp954) == Cyc_Rgn_tok){ _LL961: _temp960=( void*)(( struct Cyc_Rgn_tok_struct*)
_temp954)->f1; goto _LL957;} else{ goto _LL958;} _LL958: goto _LL959; _LL957:
return _temp960; _LL959:( int) _throw(( void*)& Cyc_yyfail_Rgn_tok); _LL955:;}
static unsigned char _temp963[ 15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp963, _temp963, _temp963 + 15u}};
struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( void* yy1){ void*
_temp964= yy1; struct Cyc_Parse_Declarator* _temp970; _LL966: if(*(( void**)
_temp964) == Cyc_Declarator_tok){ _LL971: _temp970=(( struct Cyc_Declarator_tok_struct*)
_temp964)->f1; goto _LL967;} else{ goto _LL968;} _LL968: goto _LL969; _LL967:
return _temp970; _LL969:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL965:;} static unsigned char _temp973[ 21u]="DeclaratorExpopt_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{
_temp973, _temp973, _temp973 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp974= yy1; struct _tuple13* _temp980; _LL976: if(*(( void**)
_temp974) == Cyc_DeclaratorExpopt_tok){ _LL981: _temp980=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp974)->f1; goto _LL977;} else{ goto _LL978;} _LL978: goto _LL979; _LL977:
return _temp980; _LL979:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL975:;} static unsigned char _temp983[ 25u]="DeclaratorExpoptList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp983, _temp983, _temp983 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp984= yy1; struct Cyc_List_List* _temp990; _LL986: if(*((
void**) _temp984) == Cyc_DeclaratorExpoptList_tok){ _LL991: _temp990=(( struct
Cyc_DeclaratorExpoptList_tok_struct*) _temp984)->f1; goto _LL987;} else{ goto
_LL988;} _LL988: goto _LL989; _LL987: return _temp990; _LL989:( int) _throw((
void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL985:;} static unsigned char
_temp993[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp993, _temp993,
_temp993 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp994= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1000;
_LL996: if(*(( void**) _temp994) == Cyc_AbstractDeclarator_tok){ _LL1001:
_temp1000=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp994)->f1; goto
_LL997;} else{ goto _LL998;} _LL998: goto _LL999; _LL997: return _temp1000;
_LL999:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL995:;}
static unsigned char _temp1003[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1003, _temp1003, _temp1003 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1004= yy1; int _temp1010; _LL1006:
if(*(( void**) _temp1004) == Cyc_Bool_tok){ _LL1011: _temp1010=(( struct Cyc_Bool_tok_struct*)
_temp1004)->f1; goto _LL1007;} else{ goto _LL1008;} _LL1008: goto _LL1009;
_LL1007: return _temp1010; _LL1009:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1005:;} static unsigned char _temp1013[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1013, _temp1013, _temp1013 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1014= yy1; void* _temp1020;
_LL1016: if(*(( void**) _temp1014) == Cyc_Scope_tok){ _LL1021: _temp1020=( void*)((
struct Cyc_Scope_tok_struct*) _temp1014)->f1; goto _LL1017;} else{ goto _LL1018;}
_LL1018: goto _LL1019; _LL1017: return _temp1020; _LL1019:( int) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1015:;} static unsigned char _temp1023[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1023, _temp1023, _temp1023 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1024= yy1; struct Cyc_Absyn_Tunionfield* _temp1030;
_LL1026: if(*(( void**) _temp1024) == Cyc_TunionField_tok){ _LL1031: _temp1030=((
struct Cyc_TunionField_tok_struct*) _temp1024)->f1; goto _LL1027;} else{ goto
_LL1028;} _LL1028: goto _LL1029; _LL1027: return _temp1030; _LL1029:( int)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1025:;} static unsigned char
_temp1033[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1033, _temp1033, _temp1033 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1034= yy1; struct Cyc_List_List*
_temp1040; _LL1036: if(*(( void**) _temp1034) == Cyc_TunionFieldList_tok){
_LL1041: _temp1040=(( struct Cyc_TunionFieldList_tok_struct*) _temp1034)->f1;
goto _LL1037;} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: return
_temp1040; _LL1039:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1035:;} static unsigned char _temp1043[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1043, _temp1043, _temp1043 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1044= yy1; struct _tuple18* _temp1050; _LL1046: if(*((
void**) _temp1044) == Cyc_QualSpecList_tok){ _LL1051: _temp1050=(( struct Cyc_QualSpecList_tok_struct*)
_temp1044)->f1; goto _LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049;
_LL1047: return _temp1050; _LL1049:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1045:;} static unsigned char _temp1053[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1053, _temp1053, _temp1053 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1054= yy1;
struct Cyc_List_List* _temp1060; _LL1056: if(*(( void**) _temp1054) == Cyc_IdList_tok){
_LL1061: _temp1060=(( struct Cyc_IdList_tok_struct*) _temp1054)->f1; goto
_LL1057;} else{ goto _LL1058;} _LL1058: goto _LL1059; _LL1057: return _temp1060;
_LL1059:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1055:;} static
unsigned char _temp1063[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1063, _temp1063, _temp1063 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1064= yy1;
struct _tuple2* _temp1070; _LL1066: if(*(( void**) _temp1064) == Cyc_ParamDecl_tok){
_LL1071: _temp1070=(( struct Cyc_ParamDecl_tok_struct*) _temp1064)->f1; goto
_LL1067;} else{ goto _LL1068;} _LL1068: goto _LL1069; _LL1067: return _temp1070;
_LL1069:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1065:;} static
unsigned char _temp1073[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1073, _temp1073,
_temp1073 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1074= yy1; struct Cyc_List_List* _temp1080; _LL1076: if(*(( void**)
_temp1074) == Cyc_ParamDeclList_tok){ _LL1081: _temp1080=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1074)->f1; goto _LL1077;} else{ goto _LL1078;} _LL1078: goto _LL1079;
_LL1077: return _temp1080; _LL1079:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1075:;} static unsigned char _temp1083[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1083, _temp1083, _temp1083 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1084= yy1; struct _tuple17* _temp1090; _LL1086: if(*((
void**) _temp1084) == Cyc_ParamDeclListBool_tok){ _LL1091: _temp1090=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1084)->f1; goto _LL1087;} else{ goto
_LL1088;} _LL1088: goto _LL1089; _LL1087: return _temp1090; _LL1089:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1085:;} static
unsigned char _temp1093[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1093, _temp1093, _temp1093 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1094=
yy1; struct Cyc_List_List* _temp1100; _LL1096: if(*(( void**) _temp1094) == Cyc_TypeList_tok){
_LL1101: _temp1100=(( struct Cyc_TypeList_tok_struct*) _temp1094)->f1; goto
_LL1097;} else{ goto _LL1098;} _LL1098: goto _LL1099; _LL1097: return _temp1100;
_LL1099:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1095:;} static
unsigned char _temp1103[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1103, _temp1103,
_temp1103 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1104= yy1; struct Cyc_List_List* _temp1110; _LL1106: if(*(( void**)
_temp1104) == Cyc_DesignatorList_tok){ _LL1111: _temp1110=(( struct Cyc_DesignatorList_tok_struct*)
_temp1104)->f1; goto _LL1107;} else{ goto _LL1108;} _LL1108: goto _LL1109;
_LL1107: return _temp1110; _LL1109:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1105:;} static unsigned char _temp1113[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1113,
_temp1113, _temp1113 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1114= yy1; void* _temp1120; _LL1116: if(*(( void**) _temp1114) == Cyc_Designator_tok){
_LL1121: _temp1120=( void*)(( struct Cyc_Designator_tok_struct*) _temp1114)->f1;
goto _LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119; _LL1117: return
_temp1120; _LL1119:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1115:;}
static unsigned char _temp1123[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1123, _temp1123, _temp1123 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1124= yy1; void* _temp1130;
_LL1126: if(*(( void**) _temp1124) == Cyc_Kind_tok){ _LL1131: _temp1130=( void*)((
struct Cyc_Kind_tok_struct*) _temp1124)->f1; goto _LL1127;} else{ goto _LL1128;}
_LL1128: goto _LL1129; _LL1127: return _temp1130; _LL1129:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1125:;} static unsigned char _temp1133[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1133, _temp1133, _temp1133 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1134= yy1; void* _temp1140; _LL1136: if(*(( void**) _temp1134) == Cyc_Type_tok){
_LL1141: _temp1140=( void*)(( struct Cyc_Type_tok_struct*) _temp1134)->f1; goto
_LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139; _LL1137: return _temp1140;
_LL1139:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1135:;} static
unsigned char _temp1143[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1143, _temp1143,
_temp1143 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1144= yy1; struct Cyc_List_List* _temp1150; _LL1146: if(*(( void**)
_temp1144) == Cyc_AttributeList_tok){ _LL1151: _temp1150=(( struct Cyc_AttributeList_tok_struct*)
_temp1144)->f1; goto _LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149;
_LL1147: return _temp1150; _LL1149:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1145:;} static unsigned char _temp1153[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1153,
_temp1153, _temp1153 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1154= yy1; void* _temp1160; _LL1156: if(*(( void**) _temp1154) == Cyc_Attribute_tok){
_LL1161: _temp1160=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1154)->f1;
goto _LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159; _LL1157: return
_temp1160; _LL1159:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1155:;}
static unsigned char _temp1163[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1163, _temp1163, _temp1163 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1164= yy1; struct Cyc_Absyn_Enumfield* _temp1170; _LL1166: if(*(( void**)
_temp1164) == Cyc_Enumfield_tok){ _LL1171: _temp1170=(( struct Cyc_Enumfield_tok_struct*)
_temp1164)->f1; goto _LL1167;} else{ goto _LL1168;} _LL1168: goto _LL1169;
_LL1167: return _temp1170; _LL1169:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1165:;} static unsigned char _temp1173[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1173, _temp1173, _temp1173 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1174= yy1; struct Cyc_List_List* _temp1180; _LL1176: if(*((
void**) _temp1174) == Cyc_EnumfieldList_tok){ _LL1181: _temp1180=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1174)->f1; goto _LL1177;} else{ goto _LL1178;} _LL1178: goto _LL1179;
_LL1177: return _temp1180; _LL1179:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1175:;} static unsigned char _temp1183[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1183, _temp1183, _temp1183 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1184= yy1;
struct Cyc_Core_Opt* _temp1190; _LL1186: if(*(( void**) _temp1184) == Cyc_TypeOpt_tok){
_LL1191: _temp1190=(( struct Cyc_TypeOpt_tok_struct*) _temp1184)->f1; goto
_LL1187;} else{ goto _LL1188;} _LL1188: goto _LL1189; _LL1187: return _temp1190;
_LL1189:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1185:;} static
unsigned char _temp1193[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1193, _temp1193, _temp1193 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1194=
yy1; struct Cyc_List_List* _temp1200; _LL1196: if(*(( void**) _temp1194) == Cyc_Rgnorder_tok){
_LL1201: _temp1200=(( struct Cyc_Rgnorder_tok_struct*) _temp1194)->f1; goto
_LL1197;} else{ goto _LL1198;} _LL1198: goto _LL1199; _LL1197: return _temp1200;
_LL1199:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1195:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1202;
_temp1202.timestamp= 0; _temp1202.first_line= 0; _temp1202.first_column= 0;
_temp1202.last_line= 0; _temp1202.last_column= 0; _temp1202;});} struct Cyc_Yyltype
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
_temp1203[ 2u]="$"; static unsigned char _temp1204[ 6u]="error"; static
unsigned char _temp1205[ 12u]="$undefined."; static unsigned char _temp1206[ 5u]="AUTO";
static unsigned char _temp1207[ 9u]="REGISTER"; static unsigned char _temp1208[
7u]="STATIC"; static unsigned char _temp1209[ 7u]="EXTERN"; static unsigned char
_temp1210[ 8u]="TYPEDEF"; static unsigned char _temp1211[ 5u]="VOID"; static
unsigned char _temp1212[ 5u]="CHAR"; static unsigned char _temp1213[ 6u]="SHORT";
static unsigned char _temp1214[ 4u]="INT"; static unsigned char _temp1215[ 5u]="LONG";
static unsigned char _temp1216[ 6u]="FLOAT"; static unsigned char _temp1217[ 7u]="DOUBLE";
static unsigned char _temp1218[ 7u]="SIGNED"; static unsigned char _temp1219[ 9u]="UNSIGNED";
static unsigned char _temp1220[ 6u]="CONST"; static unsigned char _temp1221[ 9u]="VOLATILE";
static unsigned char _temp1222[ 9u]="RESTRICT"; static unsigned char _temp1223[
7u]="STRUCT"; static unsigned char _temp1224[ 6u]="UNION"; static unsigned char
_temp1225[ 5u]="CASE"; static unsigned char _temp1226[ 8u]="DEFAULT"; static
unsigned char _temp1227[ 7u]="INLINE"; static unsigned char _temp1228[ 7u]="SIZEOF";
static unsigned char _temp1229[ 9u]="OFFSETOF"; static unsigned char _temp1230[
3u]="IF"; static unsigned char _temp1231[ 5u]="ELSE"; static unsigned char
_temp1232[ 7u]="SWITCH"; static unsigned char _temp1233[ 6u]="WHILE"; static
unsigned char _temp1234[ 3u]="DO"; static unsigned char _temp1235[ 4u]="FOR";
static unsigned char _temp1236[ 5u]="GOTO"; static unsigned char _temp1237[ 9u]="CONTINUE";
static unsigned char _temp1238[ 6u]="BREAK"; static unsigned char _temp1239[ 7u]="RETURN";
static unsigned char _temp1240[ 5u]="ENUM"; static unsigned char _temp1241[ 8u]="NULL_kw";
static unsigned char _temp1242[ 4u]="LET"; static unsigned char _temp1243[ 6u]="THROW";
static unsigned char _temp1244[ 4u]="TRY"; static unsigned char _temp1245[ 6u]="CATCH";
static unsigned char _temp1246[ 4u]="NEW"; static unsigned char _temp1247[ 9u]="ABSTRACT";
static unsigned char _temp1248[ 9u]="FALLTHRU"; static unsigned char _temp1249[
6u]="USING"; static unsigned char _temp1250[ 10u]="NAMESPACE"; static
unsigned char _temp1251[ 7u]="TUNION"; static unsigned char _temp1252[ 8u]="XTUNION";
static unsigned char _temp1253[ 5u]="FILL"; static unsigned char _temp1254[ 8u]="CODEGEN";
static unsigned char _temp1255[ 4u]="CUT"; static unsigned char _temp1256[ 7u]="SPLICE";
static unsigned char _temp1257[ 7u]="PRINTF"; static unsigned char _temp1258[ 8u]="FPRINTF";
static unsigned char _temp1259[ 8u]="XPRINTF"; static unsigned char _temp1260[ 6u]="SCANF";
static unsigned char _temp1261[ 7u]="FSCANF"; static unsigned char _temp1262[ 7u]="SSCANF";
static unsigned char _temp1263[ 7u]="MALLOC"; static unsigned char _temp1264[ 9u]="REGION_T";
static unsigned char _temp1265[ 7u]="REGION"; static unsigned char _temp1266[ 5u]="RNEW";
static unsigned char _temp1267[ 8u]="RMALLOC"; static unsigned char _temp1268[ 7u]="PTR_OP";
static unsigned char _temp1269[ 7u]="INC_OP"; static unsigned char _temp1270[ 7u]="DEC_OP";
static unsigned char _temp1271[ 8u]="LEFT_OP"; static unsigned char _temp1272[ 9u]="RIGHT_OP";
static unsigned char _temp1273[ 6u]="LE_OP"; static unsigned char _temp1274[ 6u]="GE_OP";
static unsigned char _temp1275[ 6u]="EQ_OP"; static unsigned char _temp1276[ 6u]="NE_OP";
static unsigned char _temp1277[ 7u]="AND_OP"; static unsigned char _temp1278[ 6u]="OR_OP";
static unsigned char _temp1279[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1280[ 11u]="DIV_ASSIGN"; static unsigned char _temp1281[ 11u]="MOD_ASSIGN";
static unsigned char _temp1282[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1283[ 11u]="SUB_ASSIGN"; static unsigned char _temp1284[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1285[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1286[ 11u]="AND_ASSIGN"; static unsigned char _temp1287[ 11u]="XOR_ASSIGN";
static unsigned char _temp1288[ 10u]="OR_ASSIGN"; static unsigned char _temp1289[
9u]="ELLIPSIS"; static unsigned char _temp1290[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1291[ 12u]="COLON_COLON"; static unsigned char _temp1292[ 11u]="IDENTIFIER";
static unsigned char _temp1293[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1294[ 7u]="STRING"; static unsigned char _temp1295[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1296[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1297[ 9u]="TYPE_VAR"; static unsigned char _temp1298[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1299[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1300[ 10u]="ATTRIBUTE"; static unsigned char _temp1301[ 4u]="';'"; static
unsigned char _temp1302[ 4u]="'{'"; static unsigned char _temp1303[ 4u]="'}'";
static unsigned char _temp1304[ 4u]="'='"; static unsigned char _temp1305[ 4u]="'('";
static unsigned char _temp1306[ 4u]="')'"; static unsigned char _temp1307[ 4u]="','";
static unsigned char _temp1308[ 4u]="'_'"; static unsigned char _temp1309[ 4u]="'$'";
static unsigned char _temp1310[ 4u]="'<'"; static unsigned char _temp1311[ 4u]="'>'";
static unsigned char _temp1312[ 4u]="':'"; static unsigned char _temp1313[ 4u]="'.'";
static unsigned char _temp1314[ 4u]="'['"; static unsigned char _temp1315[ 4u]="']'";
static unsigned char _temp1316[ 4u]="'*'"; static unsigned char _temp1317[ 4u]="'@'";
static unsigned char _temp1318[ 4u]="'?'"; static unsigned char _temp1319[ 4u]="'+'";
static unsigned char _temp1320[ 4u]="'-'"; static unsigned char _temp1321[ 4u]="'&'";
static unsigned char _temp1322[ 4u]="'|'"; static unsigned char _temp1323[ 4u]="'^'";
static unsigned char _temp1324[ 4u]="'/'"; static unsigned char _temp1325[ 4u]="'%'";
static unsigned char _temp1326[ 4u]="'~'"; static unsigned char _temp1327[ 4u]="'!'";
static unsigned char _temp1328[ 5u]="prog"; static unsigned char _temp1329[ 17u]="translation_unit";
static unsigned char _temp1330[ 21u]="external_declaration"; static
unsigned char _temp1331[ 20u]="function_definition"; static unsigned char
_temp1332[ 21u]="function_definition2"; static unsigned char _temp1333[ 13u]="using_action";
static unsigned char _temp1334[ 15u]="unusing_action"; static unsigned char
_temp1335[ 17u]="namespace_action"; static unsigned char _temp1336[ 19u]="unnamespace_action";
static unsigned char _temp1337[ 12u]="declaration"; static unsigned char
_temp1338[ 17u]="declaration_list"; static unsigned char _temp1339[ 23u]="declaration_specifiers";
static unsigned char _temp1340[ 24u]="storage_class_specifier"; static
unsigned char _temp1341[ 15u]="attributes_opt"; static unsigned char _temp1342[
11u]="attributes"; static unsigned char _temp1343[ 15u]="attribute_list"; static
unsigned char _temp1344[ 10u]="attribute"; static unsigned char _temp1345[ 15u]="type_specifier";
static unsigned char _temp1346[ 5u]="kind"; static unsigned char _temp1347[ 15u]="type_qualifier";
static unsigned char _temp1348[ 15u]="enum_specifier"; static unsigned char
_temp1349[ 11u]="enum_field"; static unsigned char _temp1350[ 22u]="enum_declaration_list";
static unsigned char _temp1351[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1352[ 16u]="type_params_opt"; static unsigned char _temp1353[
16u]="struct_or_union"; static unsigned char _temp1354[ 24u]="struct_declaration_list";
static unsigned char _temp1355[ 25u]="struct_declaration_list0"; static
unsigned char _temp1356[ 21u]="init_declarator_list"; static unsigned char
_temp1357[ 22u]="init_declarator_list0"; static unsigned char _temp1358[ 16u]="init_declarator";
static unsigned char _temp1359[ 19u]="struct_declaration"; static unsigned char
_temp1360[ 25u]="specifier_qualifier_list"; static unsigned char _temp1361[ 23u]="struct_declarator_list";
static unsigned char _temp1362[ 24u]="struct_declarator_list0"; static
unsigned char _temp1363[ 18u]="struct_declarator"; static unsigned char
_temp1364[ 17u]="tunion_specifier"; static unsigned char _temp1365[ 18u]="tunion_or_xtunion";
static unsigned char _temp1366[ 17u]="tunionfield_list"; static unsigned char
_temp1367[ 18u]="tunionfield_scope"; static unsigned char _temp1368[ 12u]="tunionfield";
static unsigned char _temp1369[ 11u]="declarator"; static unsigned char
_temp1370[ 18u]="direct_declarator"; static unsigned char _temp1371[ 8u]="pointer";
static unsigned char _temp1372[ 13u]="pointer_char"; static unsigned char
_temp1373[ 8u]="rgn_opt"; static unsigned char _temp1374[ 4u]="rgn"; static
unsigned char _temp1375[ 20u]="type_qualifier_list"; static unsigned char
_temp1376[ 20u]="parameter_type_list"; static unsigned char _temp1377[ 16u]="optional_effect";
static unsigned char _temp1378[ 19u]="optional_rgn_order"; static unsigned char
_temp1379[ 10u]="rgn_order"; static unsigned char _temp1380[ 16u]="optional_inject";
static unsigned char _temp1381[ 11u]="effect_set"; static unsigned char
_temp1382[ 14u]="atomic_effect"; static unsigned char _temp1383[ 11u]="region_set";
static unsigned char _temp1384[ 15u]="parameter_list"; static unsigned char
_temp1385[ 22u]="parameter_declaration"; static unsigned char _temp1386[ 16u]="identifier_list";
static unsigned char _temp1387[ 17u]="identifier_list0"; static unsigned char
_temp1388[ 12u]="initializer"; static unsigned char _temp1389[ 18u]="array_initializer";
static unsigned char _temp1390[ 17u]="initializer_list"; static unsigned char
_temp1391[ 12u]="designation"; static unsigned char _temp1392[ 16u]="designator_list";
static unsigned char _temp1393[ 11u]="designator"; static unsigned char
_temp1394[ 10u]="type_name"; static unsigned char _temp1395[ 14u]="any_type_name";
static unsigned char _temp1396[ 15u]="type_name_list"; static unsigned char
_temp1397[ 20u]="abstract_declarator"; static unsigned char _temp1398[ 27u]="direct_abstract_declarator";
static unsigned char _temp1399[ 10u]="statement"; static unsigned char _temp1400[
18u]="labeled_statement"; static unsigned char _temp1401[ 21u]="expression_statement";
static unsigned char _temp1402[ 19u]="compound_statement"; static unsigned char
_temp1403[ 16u]="block_item_list"; static unsigned char _temp1404[ 11u]="block_item";
static unsigned char _temp1405[ 20u]="selection_statement"; static unsigned char
_temp1406[ 15u]="switch_clauses"; static unsigned char _temp1407[ 16u]="switchC_clauses";
static unsigned char _temp1408[ 20u]="iteration_statement"; static unsigned char
_temp1409[ 15u]="jump_statement"; static unsigned char _temp1410[ 8u]="pattern";
static unsigned char _temp1411[ 19u]="tuple_pattern_list"; static unsigned char
_temp1412[ 20u]="tuple_pattern_list0"; static unsigned char _temp1413[ 14u]="field_pattern";
static unsigned char _temp1414[ 19u]="field_pattern_list"; static unsigned char
_temp1415[ 20u]="field_pattern_list0"; static unsigned char _temp1416[ 11u]="expression";
static unsigned char _temp1417[ 22u]="assignment_expression"; static
unsigned char _temp1418[ 20u]="assignment_operator"; static unsigned char
_temp1419[ 23u]="conditional_expression"; static unsigned char _temp1420[ 20u]="constant_expression";
static unsigned char _temp1421[ 22u]="logical_or_expression"; static
unsigned char _temp1422[ 23u]="logical_and_expression"; static unsigned char
_temp1423[ 24u]="inclusive_or_expression"; static unsigned char _temp1424[ 24u]="exclusive_or_expression";
static unsigned char _temp1425[ 15u]="and_expression"; static unsigned char
_temp1426[ 20u]="equality_expression"; static unsigned char _temp1427[ 22u]="relational_expression";
static unsigned char _temp1428[ 17u]="shift_expression"; static unsigned char
_temp1429[ 20u]="additive_expression"; static unsigned char _temp1430[ 26u]="multiplicative_expression";
static unsigned char _temp1431[ 16u]="cast_expression"; static unsigned char
_temp1432[ 17u]="unary_expression"; static unsigned char _temp1433[ 14u]="format_primop";
static unsigned char _temp1434[ 15u]="unary_operator"; static unsigned char
_temp1435[ 19u]="postfix_expression"; static unsigned char _temp1436[ 19u]="primary_expression";
static unsigned char _temp1437[ 25u]="argument_expression_list"; static
unsigned char _temp1438[ 26u]="argument_expression_list0"; static unsigned char
_temp1439[ 9u]="constant"; static unsigned char _temp1440[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 238u]={{ _temp1203, _temp1203, _temp1203
+ 2u},{ _temp1204, _temp1204, _temp1204 + 6u},{ _temp1205, _temp1205, _temp1205
+ 12u},{ _temp1206, _temp1206, _temp1206 + 5u},{ _temp1207, _temp1207, _temp1207
+ 9u},{ _temp1208, _temp1208, _temp1208 + 7u},{ _temp1209, _temp1209, _temp1209
+ 7u},{ _temp1210, _temp1210, _temp1210 + 8u},{ _temp1211, _temp1211, _temp1211
+ 5u},{ _temp1212, _temp1212, _temp1212 + 5u},{ _temp1213, _temp1213, _temp1213
+ 6u},{ _temp1214, _temp1214, _temp1214 + 4u},{ _temp1215, _temp1215, _temp1215
+ 5u},{ _temp1216, _temp1216, _temp1216 + 6u},{ _temp1217, _temp1217, _temp1217
+ 7u},{ _temp1218, _temp1218, _temp1218 + 7u},{ _temp1219, _temp1219, _temp1219
+ 9u},{ _temp1220, _temp1220, _temp1220 + 6u},{ _temp1221, _temp1221, _temp1221
+ 9u},{ _temp1222, _temp1222, _temp1222 + 9u},{ _temp1223, _temp1223, _temp1223
+ 7u},{ _temp1224, _temp1224, _temp1224 + 6u},{ _temp1225, _temp1225, _temp1225
+ 5u},{ _temp1226, _temp1226, _temp1226 + 8u},{ _temp1227, _temp1227, _temp1227
+ 7u},{ _temp1228, _temp1228, _temp1228 + 7u},{ _temp1229, _temp1229, _temp1229
+ 9u},{ _temp1230, _temp1230, _temp1230 + 3u},{ _temp1231, _temp1231, _temp1231
+ 5u},{ _temp1232, _temp1232, _temp1232 + 7u},{ _temp1233, _temp1233, _temp1233
+ 6u},{ _temp1234, _temp1234, _temp1234 + 3u},{ _temp1235, _temp1235, _temp1235
+ 4u},{ _temp1236, _temp1236, _temp1236 + 5u},{ _temp1237, _temp1237, _temp1237
+ 9u},{ _temp1238, _temp1238, _temp1238 + 6u},{ _temp1239, _temp1239, _temp1239
+ 7u},{ _temp1240, _temp1240, _temp1240 + 5u},{ _temp1241, _temp1241, _temp1241
+ 8u},{ _temp1242, _temp1242, _temp1242 + 4u},{ _temp1243, _temp1243, _temp1243
+ 6u},{ _temp1244, _temp1244, _temp1244 + 4u},{ _temp1245, _temp1245, _temp1245
+ 6u},{ _temp1246, _temp1246, _temp1246 + 4u},{ _temp1247, _temp1247, _temp1247
+ 9u},{ _temp1248, _temp1248, _temp1248 + 9u},{ _temp1249, _temp1249, _temp1249
+ 6u},{ _temp1250, _temp1250, _temp1250 + 10u},{ _temp1251, _temp1251, _temp1251
+ 7u},{ _temp1252, _temp1252, _temp1252 + 8u},{ _temp1253, _temp1253, _temp1253
+ 5u},{ _temp1254, _temp1254, _temp1254 + 8u},{ _temp1255, _temp1255, _temp1255
+ 4u},{ _temp1256, _temp1256, _temp1256 + 7u},{ _temp1257, _temp1257, _temp1257
+ 7u},{ _temp1258, _temp1258, _temp1258 + 8u},{ _temp1259, _temp1259, _temp1259
+ 8u},{ _temp1260, _temp1260, _temp1260 + 6u},{ _temp1261, _temp1261, _temp1261
+ 7u},{ _temp1262, _temp1262, _temp1262 + 7u},{ _temp1263, _temp1263, _temp1263
+ 7u},{ _temp1264, _temp1264, _temp1264 + 9u},{ _temp1265, _temp1265, _temp1265
+ 7u},{ _temp1266, _temp1266, _temp1266 + 5u},{ _temp1267, _temp1267, _temp1267
+ 8u},{ _temp1268, _temp1268, _temp1268 + 7u},{ _temp1269, _temp1269, _temp1269
+ 7u},{ _temp1270, _temp1270, _temp1270 + 7u},{ _temp1271, _temp1271, _temp1271
+ 8u},{ _temp1272, _temp1272, _temp1272 + 9u},{ _temp1273, _temp1273, _temp1273
+ 6u},{ _temp1274, _temp1274, _temp1274 + 6u},{ _temp1275, _temp1275, _temp1275
+ 6u},{ _temp1276, _temp1276, _temp1276 + 6u},{ _temp1277, _temp1277, _temp1277
+ 7u},{ _temp1278, _temp1278, _temp1278 + 6u},{ _temp1279, _temp1279, _temp1279
+ 11u},{ _temp1280, _temp1280, _temp1280 + 11u},{ _temp1281, _temp1281,
_temp1281 + 11u},{ _temp1282, _temp1282, _temp1282 + 11u},{ _temp1283, _temp1283,
_temp1283 + 11u},{ _temp1284, _temp1284, _temp1284 + 12u},{ _temp1285, _temp1285,
_temp1285 + 13u},{ _temp1286, _temp1286, _temp1286 + 11u},{ _temp1287, _temp1287,
_temp1287 + 11u},{ _temp1288, _temp1288, _temp1288 + 10u},{ _temp1289, _temp1289,
_temp1289 + 9u},{ _temp1290, _temp1290, _temp1290 + 11u},{ _temp1291, _temp1291,
_temp1291 + 12u},{ _temp1292, _temp1292, _temp1292 + 11u},{ _temp1293, _temp1293,
_temp1293 + 17u},{ _temp1294, _temp1294, _temp1294 + 7u},{ _temp1295, _temp1295,
_temp1295 + 19u},{ _temp1296, _temp1296, _temp1296 + 18u},{ _temp1297, _temp1297,
_temp1297 + 9u},{ _temp1298, _temp1298, _temp1298 + 16u},{ _temp1299, _temp1299,
_temp1299 + 18u},{ _temp1300, _temp1300, _temp1300 + 10u},{ _temp1301, _temp1301,
_temp1301 + 4u},{ _temp1302, _temp1302, _temp1302 + 4u},{ _temp1303, _temp1303,
_temp1303 + 4u},{ _temp1304, _temp1304, _temp1304 + 4u},{ _temp1305, _temp1305,
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
_temp1327 + 4u},{ _temp1328, _temp1328, _temp1328 + 5u},{ _temp1329, _temp1329,
_temp1329 + 17u},{ _temp1330, _temp1330, _temp1330 + 21u},{ _temp1331, _temp1331,
_temp1331 + 20u},{ _temp1332, _temp1332, _temp1332 + 21u},{ _temp1333, _temp1333,
_temp1333 + 13u},{ _temp1334, _temp1334, _temp1334 + 15u},{ _temp1335, _temp1335,
_temp1335 + 17u},{ _temp1336, _temp1336, _temp1336 + 19u},{ _temp1337, _temp1337,
_temp1337 + 12u},{ _temp1338, _temp1338, _temp1338 + 17u},{ _temp1339, _temp1339,
_temp1339 + 23u},{ _temp1340, _temp1340, _temp1340 + 24u},{ _temp1341, _temp1341,
_temp1341 + 15u},{ _temp1342, _temp1342, _temp1342 + 11u},{ _temp1343, _temp1343,
_temp1343 + 15u},{ _temp1344, _temp1344, _temp1344 + 10u},{ _temp1345, _temp1345,
_temp1345 + 15u},{ _temp1346, _temp1346, _temp1346 + 5u},{ _temp1347, _temp1347,
_temp1347 + 15u},{ _temp1348, _temp1348, _temp1348 + 15u},{ _temp1349, _temp1349,
_temp1349 + 11u},{ _temp1350, _temp1350, _temp1350 + 22u},{ _temp1351, _temp1351,
_temp1351 + 26u},{ _temp1352, _temp1352, _temp1352 + 16u},{ _temp1353, _temp1353,
_temp1353 + 16u},{ _temp1354, _temp1354, _temp1354 + 24u},{ _temp1355, _temp1355,
_temp1355 + 25u},{ _temp1356, _temp1356, _temp1356 + 21u},{ _temp1357, _temp1357,
_temp1357 + 22u},{ _temp1358, _temp1358, _temp1358 + 16u},{ _temp1359, _temp1359,
_temp1359 + 19u},{ _temp1360, _temp1360, _temp1360 + 25u},{ _temp1361, _temp1361,
_temp1361 + 23u},{ _temp1362, _temp1362, _temp1362 + 24u},{ _temp1363, _temp1363,
_temp1363 + 18u},{ _temp1364, _temp1364, _temp1364 + 17u},{ _temp1365, _temp1365,
_temp1365 + 18u},{ _temp1366, _temp1366, _temp1366 + 17u},{ _temp1367, _temp1367,
_temp1367 + 18u},{ _temp1368, _temp1368, _temp1368 + 12u},{ _temp1369, _temp1369,
_temp1369 + 11u},{ _temp1370, _temp1370, _temp1370 + 18u},{ _temp1371, _temp1371,
_temp1371 + 8u},{ _temp1372, _temp1372, _temp1372 + 13u},{ _temp1373, _temp1373,
_temp1373 + 8u},{ _temp1374, _temp1374, _temp1374 + 4u},{ _temp1375, _temp1375,
_temp1375 + 20u},{ _temp1376, _temp1376, _temp1376 + 20u},{ _temp1377, _temp1377,
_temp1377 + 16u},{ _temp1378, _temp1378, _temp1378 + 19u},{ _temp1379, _temp1379,
_temp1379 + 10u},{ _temp1380, _temp1380, _temp1380 + 16u},{ _temp1381, _temp1381,
_temp1381 + 11u},{ _temp1382, _temp1382, _temp1382 + 14u},{ _temp1383, _temp1383,
_temp1383 + 11u},{ _temp1384, _temp1384, _temp1384 + 15u},{ _temp1385, _temp1385,
_temp1385 + 22u},{ _temp1386, _temp1386, _temp1386 + 16u},{ _temp1387, _temp1387,
_temp1387 + 17u},{ _temp1388, _temp1388, _temp1388 + 12u},{ _temp1389, _temp1389,
_temp1389 + 18u},{ _temp1390, _temp1390, _temp1390 + 17u},{ _temp1391, _temp1391,
_temp1391 + 12u},{ _temp1392, _temp1392, _temp1392 + 16u},{ _temp1393, _temp1393,
_temp1393 + 11u},{ _temp1394, _temp1394, _temp1394 + 10u},{ _temp1395, _temp1395,
_temp1395 + 14u},{ _temp1396, _temp1396, _temp1396 + 15u},{ _temp1397, _temp1397,
_temp1397 + 20u},{ _temp1398, _temp1398, _temp1398 + 27u},{ _temp1399, _temp1399,
_temp1399 + 10u},{ _temp1400, _temp1400, _temp1400 + 18u},{ _temp1401, _temp1401,
_temp1401 + 21u},{ _temp1402, _temp1402, _temp1402 + 19u},{ _temp1403, _temp1403,
_temp1403 + 16u},{ _temp1404, _temp1404, _temp1404 + 11u},{ _temp1405, _temp1405,
_temp1405 + 20u},{ _temp1406, _temp1406, _temp1406 + 15u},{ _temp1407, _temp1407,
_temp1407 + 16u},{ _temp1408, _temp1408, _temp1408 + 20u},{ _temp1409, _temp1409,
_temp1409 + 15u},{ _temp1410, _temp1410, _temp1410 + 8u},{ _temp1411, _temp1411,
_temp1411 + 19u},{ _temp1412, _temp1412, _temp1412 + 20u},{ _temp1413, _temp1413,
_temp1413 + 14u},{ _temp1414, _temp1414, _temp1414 + 19u},{ _temp1415, _temp1415,
_temp1415 + 20u},{ _temp1416, _temp1416, _temp1416 + 11u},{ _temp1417, _temp1417,
_temp1417 + 22u},{ _temp1418, _temp1418, _temp1418 + 20u},{ _temp1419, _temp1419,
_temp1419 + 23u},{ _temp1420, _temp1420, _temp1420 + 20u},{ _temp1421, _temp1421,
_temp1421 + 22u},{ _temp1422, _temp1422, _temp1422 + 23u},{ _temp1423, _temp1423,
_temp1423 + 24u},{ _temp1424, _temp1424, _temp1424 + 24u},{ _temp1425, _temp1425,
_temp1425 + 15u},{ _temp1426, _temp1426, _temp1426 + 20u},{ _temp1427, _temp1427,
_temp1427 + 22u},{ _temp1428, _temp1428, _temp1428 + 17u},{ _temp1429, _temp1429,
_temp1429 + 20u},{ _temp1430, _temp1430, _temp1430 + 26u},{ _temp1431, _temp1431,
_temp1431 + 16u},{ _temp1432, _temp1432, _temp1432 + 17u},{ _temp1433, _temp1433,
_temp1433 + 14u},{ _temp1434, _temp1434, _temp1434 + 15u},{ _temp1435, _temp1435,
_temp1435 + 19u},{ _temp1436, _temp1436, _temp1436 + 19u},{ _temp1437, _temp1437,
_temp1437 + 25u},{ _temp1438, _temp1438, _temp1438 + 26u},{ _temp1439, _temp1439,
_temp1439 + 9u},{ _temp1440, _temp1440, _temp1440 + 20u}}; static short Cyc_yyr1[
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
yyn){ case 1: _LL1441: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1442:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1444=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1444[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1445; _temp1445.tag= Cyc_DeclList_tok; _temp1445.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1445;});
_temp1444;}); break; case 3: _LL1443: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1447=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1447[ 0]=({ struct Cyc_DeclList_tok_struct _temp1448; _temp1448.tag= Cyc_DeclList_tok;
_temp1448.f1=({ struct Cyc_List_List* _temp1449=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1449->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1450=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1450->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1451=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1451[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1452; _temp1452.tag= Cyc_Absyn_Using_d;
_temp1452.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1452.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1452;}); _temp1451;}));
_temp1450->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1450;});
_temp1449->tl= 0; _temp1449;}); _temp1448;}); _temp1447;}); Cyc_Lex_leave_using();
break; case 4: _LL1446: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1454=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1454[ 0]=({ struct Cyc_DeclList_tok_struct _temp1455; _temp1455.tag= Cyc_DeclList_tok;
_temp1455.f1=({ struct Cyc_List_List* _temp1456=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1456->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1457=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1457->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1458=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1458[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1459; _temp1459.tag= Cyc_Absyn_Using_d;
_temp1459.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1459.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1459;});
_temp1458;})); _temp1457->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1457;}); _temp1456->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1456;}); _temp1455;});
_temp1454;}); break; case 5: _LL1453: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1461=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1461[ 0]=({ struct Cyc_DeclList_tok_struct _temp1462; _temp1462.tag= Cyc_DeclList_tok;
_temp1462.f1=({ struct Cyc_List_List* _temp1463=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1463->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1464=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1464->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1465=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1465[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1466; _temp1466.tag=
Cyc_Absyn_Namespace_d; _temp1466.f1=({ struct _tagged_arr* _temp1467=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1467[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1467;});
_temp1466.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1466;}); _temp1465;})); _temp1464->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1464;}); _temp1463->tl= 0; _temp1463;}); _temp1462;}); _temp1461;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1460: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1469=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1469[ 0]=({ struct Cyc_DeclList_tok_struct _temp1470; _temp1470.tag= Cyc_DeclList_tok;
_temp1470.f1=({ struct Cyc_List_List* _temp1471=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1471->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1472=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1472->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1473=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1473[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1474; _temp1474.tag=
Cyc_Absyn_Namespace_d; _temp1474.f1=({ struct _tagged_arr* _temp1475=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1475[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1475;});
_temp1474.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1474;}); _temp1473;})); _temp1472->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1472;}); _temp1471->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1471;}); _temp1470;});
_temp1469;}); break; case 7: _LL1468: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1477=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1477[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1478; _temp1478.tag= Cyc_DeclList_tok; _temp1478.f1=({ struct Cyc_List_List*
_temp1479=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1479->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1480=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1480->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1481=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1481[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1482; _temp1482.tag= Cyc_Absyn_ExternC_d;
_temp1482.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1482;}); _temp1481;})); _temp1480->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1480;}); _temp1479->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1479;}); _temp1478;});
_temp1477;}); break; case 8: _LL1476: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1484=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1484[ 0]=({ struct Cyc_DeclList_tok_struct _temp1485; _temp1485.tag= Cyc_DeclList_tok;
_temp1485.f1= 0; _temp1485;}); _temp1484;}); break; case 9: _LL1483: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1487=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1487[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1488; _temp1488.tag= Cyc_DeclList_tok; _temp1488.f1=({ struct Cyc_List_List*
_temp1489=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1489->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1490=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1490[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1491; _temp1491.tag= Cyc_Absyn_Fn_d;
_temp1491.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1491;}); _temp1490;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1489->tl=
0; _temp1489;}); _temp1488;}); _temp1487;}); break; case 10: _LL1486: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1492: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1494=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1494[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1495; _temp1495.tag= Cyc_FnDecl_tok; _temp1495.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1495;});
_temp1494;}); break; case 12: _LL1493: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1497=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1497[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1498; _temp1498.tag= Cyc_FnDecl_tok;
_temp1498.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1499=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1499->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1499;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1498;}); _temp1497;}); break; case 13: _LL1496: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1501=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1501[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1502; _temp1502.tag= Cyc_FnDecl_tok;
_temp1502.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1502;}); _temp1501;}); break; case 14: _LL1500: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1504=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1504[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1505; _temp1505.tag= Cyc_FnDecl_tok;
_temp1505.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1506=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1506->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1506;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1505;}); _temp1504;}); break; case 15: _LL1503: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1508=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1508[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1509; _temp1509.tag= Cyc_FnDecl_tok;
_temp1509.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1510=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1510->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1510;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1509;}); _temp1508;}); break; case 16: _LL1507: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1512=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1512[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1513; _temp1513.tag= Cyc_FnDecl_tok;
_temp1513.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1514=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1514->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1514;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1513;}); _temp1512;}); break; case 17: _LL1511: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1515:
Cyc_Lex_leave_using(); break; case 19: _LL1516: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1518=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1518[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1518;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1517: Cyc_Lex_leave_namespace(); break; case 21: _LL1519: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1521=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1521[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1522; _temp1522.tag= Cyc_DeclList_tok; _temp1522.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1522;}); _temp1521;}); break; case 22: _LL1520: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1524=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1524[ 0]=({ struct Cyc_DeclList_tok_struct _temp1525; _temp1525.tag= Cyc_DeclList_tok;
_temp1525.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1525;}); _temp1524;}); break; case 23: _LL1523: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1527=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1527[ 0]=({ struct Cyc_DeclList_tok_struct _temp1528; _temp1528.tag= Cyc_DeclList_tok;
_temp1528.f1=({ struct Cyc_List_List* _temp1529=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1529->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1529->tl=
0; _temp1529;}); _temp1528;}); _temp1527;}); break; case 24: _LL1526: { struct
Cyc_List_List* _temp1531= 0;{ struct Cyc_List_List* _temp1532= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1532 != 0; _temp1532=(( struct Cyc_List_List*) _check_null( _temp1532))->tl){
struct _tagged_arr* _temp1533=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1532))->hd; struct _tuple1* _temp1534=({ struct _tuple1*
_temp1537=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1537->f1=
Cyc_Absyn_rel_ns_null; _temp1537->f2= _temp1533; _temp1537;}); struct Cyc_Absyn_Vardecl*
_temp1535= Cyc_Absyn_new_vardecl( _temp1534, Cyc_Absyn_wildtyp( 0), 0);
_temp1531=({ struct Cyc_List_List* _temp1536=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1536->hd=( void*) _temp1535; _temp1536->tl=
_temp1531; _temp1536;});}} _temp1531=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1531); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1538=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1538[ 0]=({ struct Cyc_DeclList_tok_struct _temp1539; _temp1539.tag= Cyc_DeclList_tok;
_temp1539.f1=({ struct Cyc_List_List* _temp1540=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1540->hd=( void*) Cyc_Absyn_letv_decl(
_temp1531, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1540->tl= 0; _temp1540;}); _temp1539;});
_temp1538;}); break;} case 25: _LL1530: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1541:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1543=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1543[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1544; _temp1544.tag= Cyc_DeclList_tok; _temp1544.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1544;});
_temp1543;}); break; case 27: _LL1542: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1546=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1546[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1547; _temp1547.tag= Cyc_DeclSpec_tok;
_temp1547.f1=({ struct Cyc_Parse_Declaration_spec* _temp1548=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1548->sc=({ struct
Cyc_Core_Opt* _temp1549=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1549->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1549;});
_temp1548->tq= Cyc_Absyn_empty_tqual(); _temp1548->type_specs= 0; _temp1548->is_inline=
0; _temp1548->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1548;}); _temp1547;});
_temp1546;}); break; case 28: _LL1545: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1551=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1551[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1552; _temp1552.tag= Cyc_DeclSpec_tok; _temp1552.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1553=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1553->sc=({ struct Cyc_Core_Opt* _temp1554=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1554->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1554;});
_temp1553->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1553->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1553->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1553->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1553;}); _temp1552;}); _temp1551;}); break; case 29: _LL1550: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1556=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1556[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1557; _temp1557.tag= Cyc_DeclSpec_tok; _temp1557.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1558=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1558->sc= 0; _temp1558->tq= Cyc_Absyn_empty_tqual(); _temp1558->type_specs=({
struct Cyc_List_List* _temp1559=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1559->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1559->tl= 0;
_temp1559;}); _temp1558->is_inline= 0; _temp1558->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1558;});
_temp1557;}); _temp1556;}); break; case 30: _LL1555: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1561=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1561[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1562; _temp1562.tag= Cyc_DeclSpec_tok;
_temp1562.f1=({ struct Cyc_Parse_Declaration_spec* _temp1563=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1563->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1563->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1563->type_specs=({ struct Cyc_List_List* _temp1564=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1564->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1564->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1564;});
_temp1563->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1563->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1563;}); _temp1562;}); _temp1561;}); break; case 31: _LL1560: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1566=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1566[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1567; _temp1567.tag= Cyc_DeclSpec_tok; _temp1567.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1568=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1568->sc= 0; _temp1568->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1568->type_specs=
0; _temp1568->is_inline= 0; _temp1568->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1568;});
_temp1567;}); _temp1566;}); break; case 32: _LL1565: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1570=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1570[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1571; _temp1571.tag= Cyc_DeclSpec_tok;
_temp1571.f1=({ struct Cyc_Parse_Declaration_spec* _temp1572=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1572->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1572->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1572->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1572->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1572->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1572;}); _temp1571;}); _temp1570;}); break; case 33: _LL1569: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1574=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1574[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1575; _temp1575.tag= Cyc_DeclSpec_tok; _temp1575.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1576=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1576->sc= 0; _temp1576->tq= Cyc_Absyn_empty_tqual(); _temp1576->type_specs=
0; _temp1576->is_inline= 1; _temp1576->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1576;});
_temp1575;}); _temp1574;}); break; case 34: _LL1573: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1578=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1578[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1579; _temp1579.tag= Cyc_DeclSpec_tok;
_temp1579.f1=({ struct Cyc_Parse_Declaration_spec* _temp1580=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1580->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1580->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1580->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1580->is_inline=
1; _temp1580->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1580;}); _temp1579;}); _temp1578;}); break; case 35: _LL1577: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1582=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1582[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1583; _temp1583.tag= Cyc_StorageClass_tok;
_temp1583.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1583;}); _temp1582;});
break; case 36: _LL1581: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1585=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1585[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1586; _temp1586.tag=
Cyc_StorageClass_tok; _temp1586.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1586;}); _temp1585;}); break; case 37: _LL1584: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1588=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1588[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1589; _temp1589.tag=
Cyc_StorageClass_tok; _temp1589.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1589;}); _temp1588;}); break; case 38: _LL1587: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1591=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1591[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1592; _temp1592.tag=
Cyc_StorageClass_tok; _temp1592.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1592;}); _temp1591;}); break; case 39: _LL1590: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1594=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1594[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1595; _temp1595.tag= Cyc_StorageClass_tok;
_temp1595.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1595;}); _temp1594;});
break; case 40: _LL1593: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1597=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1597[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1598; _temp1598.tag=
Cyc_StorageClass_tok; _temp1598.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1598;}); _temp1597;}); break; case 41: _LL1596: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1600=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1600[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1601; _temp1601.tag=
Cyc_StorageClass_tok; _temp1601.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1601;}); _temp1600;}); break; case 42: _LL1599: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1603=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1603[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1604; _temp1604.tag=
Cyc_AttributeList_tok; _temp1604.f1= 0; _temp1604;}); _temp1603;}); break; case
43: _LL1602: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1605: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1607=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1607[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1608; _temp1608.tag=
Cyc_AttributeList_tok; _temp1608.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1608;});
_temp1607;}); break; case 45: _LL1606: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1610=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1610[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1611; _temp1611.tag=
Cyc_AttributeList_tok; _temp1611.f1=({ struct Cyc_List_List* _temp1612=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1612->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1612->tl= 0; _temp1612;}); _temp1611;}); _temp1610;});
break; case 46: _LL1609: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1614=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1614[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1615; _temp1615.tag=
Cyc_AttributeList_tok; _temp1615.f1=({ struct Cyc_List_List* _temp1616=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1616->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1616->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1616;}); _temp1615;});
_temp1614;}); break; case 47: _LL1613: { struct _tagged_arr _temp1618= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1618, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1618, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1618, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1618,
sizeof( unsigned char),( int)( _get_arr_size( _temp1618, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1618,
sizeof( unsigned char),( int)( _get_arr_size( _temp1618, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1618=( struct _tagged_arr) Cyc_String_substring(
_temp1618, 2,( int)( _get_arr_size( _temp1618, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1618,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1619=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1619[ 0]=({ struct Cyc_Attribute_tok_struct _temp1620; _temp1620.tag= Cyc_Attribute_tok;
_temp1620.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1620;}); _temp1619;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1621=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1621[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1622; _temp1622.tag= Cyc_Attribute_tok; _temp1622.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1622;}); _temp1621;});} break;}} case
48: _LL1617: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1624=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1624[ 0]=({ struct Cyc_Attribute_tok_struct _temp1625; _temp1625.tag= Cyc_Attribute_tok;
_temp1625.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1625;}); _temp1624;});
break; case 49: _LL1623: { struct _tagged_arr _temp1627= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1630; int _temp1631; struct _tuple16* _temp1628= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1630=*
_temp1628; _LL1632: _temp1631= _temp1630.f2; goto _LL1629; _LL1629: { void* a;
if( Cyc_String_zstrcmp( _temp1627, _tag_arr("regparm", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1627, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) == 0){ if( _temp1631 <= 0? 1: _temp1631 > 3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1633=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1633[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1634; _temp1634.tag= Cyc_Absyn_Regparm_att; _temp1634.f1= _temp1631;
_temp1634;}); _temp1633;});} else{ if( Cyc_String_zstrcmp( _temp1627, _tag_arr("aligned",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1627, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1631 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1631; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1635=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1635[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1636; _temp1636.tag= Cyc_Absyn_Aligned_att;
_temp1636.f1= _temp1631; _temp1636;}); _temp1635;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1637=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1637[ 0]=({ struct Cyc_Attribute_tok_struct _temp1638; _temp1638.tag= Cyc_Attribute_tok;
_temp1638.f1=( void*) a; _temp1638;}); _temp1637;}); break;}} case 50: _LL1626: {
struct _tagged_arr _temp1640= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1641= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1640, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1640, _tag_arr("__section__",
sizeof( unsigned char), 12u))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1642=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1642[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1643; _temp1643.tag=
Cyc_Absyn_Section_att; _temp1643.f1= _temp1641; _temp1643;}); _temp1642;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1644=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1644[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1645; _temp1645.tag= Cyc_Attribute_tok; _temp1645.f1=(
void*) a; _temp1645;}); _temp1644;}); break;} case 51: _LL1639: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1647=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1647[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1648; _temp1648.tag= Cyc_TypeSpecifier_tok;
_temp1648.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1648;});
_temp1647;}); break; case 52: _LL1646: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1650=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1650[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1651; _temp1651.tag=
Cyc_TypeSpecifier_tok; _temp1651.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1651;}); _temp1650;}); break; case 53: _LL1649:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1653=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1653[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1654; _temp1654.tag= Cyc_TypeSpecifier_tok;
_temp1654.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1654;});
_temp1653;}); break; case 54: _LL1652: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1656=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1656[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1657; _temp1657.tag=
Cyc_TypeSpecifier_tok; _temp1657.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1658=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1658[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1659; _temp1659.tag=
Cyc_Parse_Short_spec; _temp1659.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1659;});
_temp1658;})); _temp1657;}); _temp1656;}); break; case 55: _LL1655: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1661=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1661[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1662; _temp1662.tag= Cyc_TypeSpecifier_tok;
_temp1662.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1662;});
_temp1661;}); break; case 56: _LL1660: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1664=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1664[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1665; _temp1665.tag=
Cyc_TypeSpecifier_tok; _temp1665.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1666=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1666[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1667; _temp1667.tag= Cyc_Parse_Long_spec;
_temp1667.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1667;}); _temp1666;})); _temp1665;}); _temp1664;});
break; case 57: _LL1663: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1669=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1669[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1670; _temp1670.tag=
Cyc_TypeSpecifier_tok; _temp1670.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1670;}); _temp1669;}); break; case 58: _LL1668:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1672=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1672[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1673; _temp1673.tag= Cyc_TypeSpecifier_tok;
_temp1673.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1673;});
_temp1672;}); break; case 59: _LL1671: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1675=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1675[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1676; _temp1676.tag=
Cyc_TypeSpecifier_tok; _temp1676.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1677=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1677[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1678; _temp1678.tag=
Cyc_Parse_Signed_spec; _temp1678.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1678;});
_temp1677;})); _temp1676;}); _temp1675;}); break; case 60: _LL1674: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1680=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1680[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1681; _temp1681.tag= Cyc_TypeSpecifier_tok;
_temp1681.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1682=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1682[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1683; _temp1683.tag= Cyc_Parse_Unsigned_spec; _temp1683.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1683;});
_temp1682;})); _temp1681;}); _temp1680;}); break; case 61: _LL1679: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 62: _LL1684:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
63: _LL1685: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 64: _LL1686: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1688=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1688[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1689; _temp1689.tag=
Cyc_TypeSpecifier_tok; _temp1689.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1689;});
_temp1688;}); break; case 65: _LL1687: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1691=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1691[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1692; _temp1692.tag=
Cyc_TypeSpecifier_tok; _temp1692.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1692;}); _temp1691;}); break; case 66: _LL1690: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1694=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1694[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1695; _temp1695.tag=
Cyc_TypeSpecifier_tok; _temp1695.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1696=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1696[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1697; _temp1697.tag= Cyc_Absyn_TypedefType;
_temp1697.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1697.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1697.f3= 0;
_temp1697;}); _temp1696;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1695;});
_temp1694;}); break; case 67: _LL1693: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1699=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1699[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1700; _temp1700.tag=
Cyc_TypeSpecifier_tok; _temp1700.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1701=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1701[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1702; _temp1702.tag= Cyc_Absyn_TupleType;
_temp1702.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1702;});
_temp1701;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1700;}); _temp1699;}); break; case 68:
_LL1698: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1704=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1704[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1705; _temp1705.tag=
Cyc_TypeSpecifier_tok; _temp1705.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1706=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1706[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1707; _temp1707.tag= Cyc_Absyn_RgnHandleType;
_temp1707.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1707;}); _temp1706;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1705;}); _temp1704;}); break; case 69: _LL1703: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1709=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1709[ 0]=({ struct Cyc_Kind_tok_struct _temp1710; _temp1710.tag= Cyc_Kind_tok;
_temp1710.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1710;});
_temp1709;}); break; case 70: _LL1708: { struct _tagged_arr* _temp1714; void*
_temp1716; struct _tuple1 _temp1712=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1717: _temp1716=
_temp1712.f1; goto _LL1715; _LL1715: _temp1714= _temp1712.f2; goto _LL1713;
_LL1713: if( _temp1716 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1718=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1718[ 0]=({ struct Cyc_Kind_tok_struct
_temp1719; _temp1719.tag= Cyc_Kind_tok; _temp1719.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1714, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1719;}); _temp1718;}); break;} case 71:
_LL1711: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1721=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1721[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1722; _temp1722.tag= Cyc_TypeQual_tok;
_temp1722.f1=({ struct Cyc_Absyn_Tqual _temp1723; _temp1723.q_const= 1;
_temp1723.q_volatile= 0; _temp1723.q_restrict= 0; _temp1723;}); _temp1722;});
_temp1721;}); break; case 72: _LL1720: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1725=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1725[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1726; _temp1726.tag= Cyc_TypeQual_tok; _temp1726.f1=({ struct Cyc_Absyn_Tqual
_temp1727; _temp1727.q_const= 0; _temp1727.q_volatile= 1; _temp1727.q_restrict=
0; _temp1727;}); _temp1726;}); _temp1725;}); break; case 73: _LL1724: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1729=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1729[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1730; _temp1730.tag= Cyc_TypeQual_tok;
_temp1730.f1=({ struct Cyc_Absyn_Tqual _temp1731; _temp1731.q_const= 0;
_temp1731.q_volatile= 0; _temp1731.q_restrict= 1; _temp1731;}); _temp1730;});
_temp1729;}); break; case 74: _LL1728: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1733=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1733[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1734; _temp1734.tag=
Cyc_TypeSpecifier_tok; _temp1734.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1735=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1735[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1736; _temp1736.tag= Cyc_Parse_Decl_spec;
_temp1736.f1=({ struct Cyc_Absyn_Decl* _temp1737=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1737->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1738=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1738[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1739; _temp1739.tag= Cyc_Absyn_Enum_d; _temp1739.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1740=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1740->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1740->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1740->fields=({
struct Cyc_Core_Opt* _temp1741=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1741->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1741;});
_temp1740;}); _temp1739;}); _temp1738;})); _temp1737->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1737;}); _temp1736;}); _temp1735;})); _temp1734;}); _temp1733;}); break;
case 75: _LL1732: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1743=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1743[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1744; _temp1744.tag=
Cyc_TypeSpecifier_tok; _temp1744.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1745=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1745[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1746; _temp1746.tag= Cyc_Absyn_EnumType;
_temp1746.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1746.f2= 0; _temp1746;}); _temp1745;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1744;}); _temp1743;}); break; case 76: _LL1742: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1748=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1748[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1749; _temp1749.tag= Cyc_Enumfield_tok;
_temp1749.f1=({ struct Cyc_Absyn_Enumfield* _temp1750=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1750->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1750->tag= 0;
_temp1750->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1750;});
_temp1749;}); _temp1748;}); break; case 77: _LL1747: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1752=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1752[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1753; _temp1753.tag= Cyc_Enumfield_tok;
_temp1753.f1=({ struct Cyc_Absyn_Enumfield* _temp1754=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1754->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1754->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1754->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1754;});
_temp1753;}); _temp1752;}); break; case 78: _LL1751: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1756=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1756[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1757; _temp1757.tag=
Cyc_EnumfieldList_tok; _temp1757.f1=({ struct Cyc_List_List* _temp1758=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1758->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1758->tl= 0; _temp1758;}); _temp1757;}); _temp1756;});
break; case 79: _LL1755: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1760=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1760[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1761; _temp1761.tag=
Cyc_EnumfieldList_tok; _temp1761.f1=({ struct Cyc_List_List* _temp1762=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1762->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1762->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1762;}); _temp1761;});
_temp1760;}); break; case 80: _LL1759: { void* t;{ void* _temp1764= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1766: if(
_temp1764 ==( void*) Cyc_Parse_Struct_su){ goto _LL1767;} else{ goto _LL1768;}
_LL1768: if( _temp1764 ==( void*) Cyc_Parse_Union_su){ goto _LL1769;} else{ goto
_LL1765;} _LL1767: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1770=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1770[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1771; _temp1771.tag= Cyc_Absyn_AnonStructType; _temp1771.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1771;});
_temp1770;}); goto _LL1765; _LL1769: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1772=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1772[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1773; _temp1773.tag= Cyc_Absyn_AnonUnionType; _temp1773.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1773;});
_temp1772;}); goto _LL1765; _LL1765:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1774=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1774[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1775; _temp1775.tag=
Cyc_TypeSpecifier_tok; _temp1775.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1776=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1776[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1777; _temp1777.tag= Cyc_Parse_Type_spec;
_temp1777.f1=( void*) t; _temp1777.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1777;});
_temp1776;})); _temp1775;}); _temp1774;}); break;} case 81: _LL1763: { struct
Cyc_List_List* _temp1779=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1780= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1782: if(
_temp1780 ==( void*) Cyc_Parse_Struct_su){ goto _LL1783;} else{ goto _LL1784;}
_LL1784: if( _temp1780 ==( void*) Cyc_Parse_Union_su){ goto _LL1785;} else{ goto
_LL1781;} _LL1783: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1786=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1786->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1786;}), _temp1779,({ struct Cyc_Core_Opt*
_temp1787=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1787->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1787;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1781; _LL1785: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1788=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1788->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1788;}),
_temp1779,({ struct Cyc_Core_Opt* _temp1789=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1789->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1789;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1781; _LL1781:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1790=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1790[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1791; _temp1791.tag= Cyc_TypeSpecifier_tok;
_temp1791.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1792=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1792[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1793; _temp1793.tag= Cyc_Parse_Decl_spec;
_temp1793.f1= d; _temp1793;}); _temp1792;})); _temp1791;}); _temp1790;}); break;}
case 82: _LL1778: { struct Cyc_List_List* _temp1795=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1796= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1798: if(
_temp1796 ==( void*) Cyc_Parse_Struct_su){ goto _LL1799;} else{ goto _LL1800;}
_LL1800: if( _temp1796 ==( void*) Cyc_Parse_Union_su){ goto _LL1801;} else{ goto
_LL1797;} _LL1799: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1802=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1802->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1802;}), _temp1795,({ struct Cyc_Core_Opt*
_temp1803=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1803->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1803;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1797; _LL1801: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1804=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1804->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1804;}),
_temp1795,({ struct Cyc_Core_Opt* _temp1805=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1805->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1805;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1797; _LL1797:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1806=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1806[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1807; _temp1807.tag= Cyc_TypeSpecifier_tok;
_temp1807.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1808=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1808[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1809; _temp1809.tag= Cyc_Parse_Decl_spec;
_temp1809.f1= d; _temp1809;}); _temp1808;})); _temp1807;}); _temp1806;}); break;}
case 83: _LL1794:{ void* _temp1811= Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1813: if(
_temp1811 ==( void*) Cyc_Parse_Struct_su){ goto _LL1814;} else{ goto _LL1815;}
_LL1815: if( _temp1811 ==( void*) Cyc_Parse_Union_su){ goto _LL1816;} else{ goto
_LL1812;} _LL1814: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1817=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1817[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1818; _temp1818.tag=
Cyc_TypeSpecifier_tok; _temp1818.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1819=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1819[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1820; _temp1820.tag= Cyc_Absyn_StructType;
_temp1820.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1820.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1820.f3= 0;
_temp1820;}); _temp1819;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1818;});
_temp1817;}); goto _LL1812; _LL1816: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1821=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1821[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1822; _temp1822.tag=
Cyc_TypeSpecifier_tok; _temp1822.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1823=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1823[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1824; _temp1824.tag= Cyc_Absyn_UnionType;
_temp1824.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1824.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1824.f3= 0;
_temp1824;}); _temp1823;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1822;});
_temp1821;}); goto _LL1812; _LL1812:;} break; case 84: _LL1810:{ void* _temp1826=
Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL1828: if( _temp1826 ==( void*) Cyc_Parse_Struct_su){
goto _LL1829;} else{ goto _LL1830;} _LL1830: if( _temp1826 ==( void*) Cyc_Parse_Union_su){
goto _LL1831;} else{ goto _LL1827;} _LL1829: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1832=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1832[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1833; _temp1833.tag=
Cyc_TypeSpecifier_tok; _temp1833.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1834=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1834[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1835; _temp1835.tag= Cyc_Absyn_StructType;
_temp1835.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1835.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1835.f3= 0;
_temp1835;}); _temp1834;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1833;});
_temp1832;}); goto _LL1827; _LL1831: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1836=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1836[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1837; _temp1837.tag=
Cyc_TypeSpecifier_tok; _temp1837.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1838=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1838[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1839; _temp1839.tag= Cyc_Absyn_UnionType;
_temp1839.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1839.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1839.f3= 0;
_temp1839;}); _temp1838;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1837;});
_temp1836;}); goto _LL1827; _LL1827:;} break; case 85: _LL1825: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1841=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1841[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1842; _temp1842.tag= Cyc_TypeList_tok; _temp1842.f1= 0; _temp1842;});
_temp1841;}); break; case 86: _LL1840: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1844=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1844[ 0]=({ struct Cyc_TypeList_tok_struct _temp1845; _temp1845.tag= Cyc_TypeList_tok;
_temp1845.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1845;});
_temp1844;}); break; case 87: _LL1843:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1847=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1847[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1848; _temp1848.tag= Cyc_TypeList_tok; _temp1848.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1848;});
_temp1847;}); break; case 88: _LL1846: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1850=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1850[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1851; _temp1851.tag=
Cyc_StructOrUnion_tok; _temp1851.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp1851;}); _temp1850;}); break; case 89: _LL1849: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1853=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1853[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1854; _temp1854.tag=
Cyc_StructOrUnion_tok; _temp1854.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp1854;}); _temp1853;}); break; case 90: _LL1852: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1856=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1856[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1857; _temp1857.tag= Cyc_StructFieldDeclList_tok; _temp1857.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1857;});
_temp1856;}); break; case 91: _LL1855: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1859=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1859[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1860; _temp1860.tag= Cyc_StructFieldDeclListList_tok; _temp1860.f1=({
struct Cyc_List_List* _temp1861=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1861->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1861->tl= 0;
_temp1861;}); _temp1860;}); _temp1859;}); break; case 92: _LL1858: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1863=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1863[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1864; _temp1864.tag= Cyc_StructFieldDeclListList_tok;
_temp1864.f1=({ struct Cyc_List_List* _temp1865=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1865->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1865->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1865;}); _temp1864;}); _temp1863;}); break;
case 93: _LL1862: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1867=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1867[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1868; _temp1868.tag=
Cyc_InitDeclList_tok; _temp1868.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1868;});
_temp1867;}); break; case 94: _LL1866: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1870=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1870[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1871; _temp1871.tag=
Cyc_InitDeclList_tok; _temp1871.f1=({ struct Cyc_List_List* _temp1872=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1872->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1872->tl= 0; _temp1872;}); _temp1871;}); _temp1870;});
break; case 95: _LL1869: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1874=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1874[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1875; _temp1875.tag=
Cyc_InitDeclList_tok; _temp1875.f1=({ struct Cyc_List_List* _temp1876=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1876->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1876->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1876;});
_temp1875;}); _temp1874;}); break; case 96: _LL1873: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1878=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1878[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1879; _temp1879.tag= Cyc_InitDecl_tok;
_temp1879.f1=({ struct _tuple15* _temp1880=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1880->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1880->f2= 0;
_temp1880;}); _temp1879;}); _temp1878;}); break; case 97: _LL1877: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1882=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1882[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1883; _temp1883.tag= Cyc_InitDecl_tok; _temp1883.f1=({ struct _tuple15*
_temp1884=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1884->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1884->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1884;});
_temp1883;}); _temp1882;}); break; case 98: _LL1881: { struct Cyc_Absyn_Tqual tq=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])).f1; struct Cyc_List_List* _temp1886=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3; void*
_temp1887= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
struct Cyc_List_List* _temp1890; struct Cyc_List_List* _temp1892; struct _tuple0
_temp1888=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _LL1893:
_temp1892= _temp1888.f1; goto _LL1891; _LL1891: _temp1890= _temp1888.f2; goto
_LL1889; _LL1889: { struct Cyc_List_List* _temp1894=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp1887, _temp1892, _temp1886), _temp1890); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1895=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1895[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1896; _temp1896.tag= Cyc_StructFieldDeclList_tok; _temp1896.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),
_temp1894); _temp1896;}); _temp1895;}); break;}} case 99: _LL1885: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1898=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1898[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1899; _temp1899.tag= Cyc_QualSpecList_tok;
_temp1899.f1=({ struct _tuple18* _temp1900=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1900->f1= Cyc_Absyn_empty_tqual(); _temp1900->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1901=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1901->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1901->tl=
0; _temp1901;}); _temp1900->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1900;}); _temp1899;});
_temp1898;}); break; case 100: _LL1897: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1903=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1903[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1904; _temp1904.tag=
Cyc_QualSpecList_tok; _temp1904.f1=({ struct _tuple18* _temp1905=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1905->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1905->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1906=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1906->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1906->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1906;}); _temp1905->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1905;});
_temp1904;}); _temp1903;}); break; case 101: _LL1902: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1908=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1908[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1909; _temp1909.tag= Cyc_QualSpecList_tok;
_temp1909.f1=({ struct _tuple18* _temp1910=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1910->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1910->f2= 0;
_temp1910->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1910;}); _temp1909;}); _temp1908;}); break; case
102: _LL1907: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1912=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1912[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1913; _temp1913.tag=
Cyc_QualSpecList_tok; _temp1913.f1=({ struct _tuple18* _temp1914=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1914->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1914->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1914->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1914;});
_temp1913;}); _temp1912;}); break; case 103: _LL1911: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1916=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1916[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1917; _temp1917.tag= Cyc_DeclaratorExpoptList_tok;
_temp1917.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1917;}); _temp1916;}); break; case 104: _LL1915: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1919=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1919[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1920; _temp1920.tag= Cyc_DeclaratorExpoptList_tok;
_temp1920.f1=({ struct Cyc_List_List* _temp1921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1921->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1921->tl= 0;
_temp1921;}); _temp1920;}); _temp1919;}); break; case 105: _LL1918: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp1923=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1923[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1924; _temp1924.tag= Cyc_DeclaratorExpoptList_tok;
_temp1924.f1=({ struct Cyc_List_List* _temp1925=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1925->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1925->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1925;}); _temp1924;}); _temp1923;}); break; case 106:
_LL1922: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp1927=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp1927[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp1928; _temp1928.tag=
Cyc_DeclaratorExpopt_tok; _temp1928.f1=({ struct _tuple13* _temp1929=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1929->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1929->f2= 0;
_temp1929;}); _temp1928;}); _temp1927;}); break; case 107: _LL1926: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1931=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1931[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1932; _temp1932.tag= Cyc_DeclaratorExpopt_tok;
_temp1932.f1=({ struct _tuple13* _temp1933=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1933->f1=({ struct Cyc_Parse_Declarator*
_temp1935=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp1935->id=({ struct _tuple1* _temp1936=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp1936->f1= Cyc_Absyn_rel_ns_null; _temp1936->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1936;}); _temp1935->tms= 0; _temp1935;});
_temp1933->f2=({ struct Cyc_Core_Opt* _temp1934=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1934->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1934;});
_temp1933;}); _temp1932;}); _temp1931;}); break; case 108: _LL1930: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1938=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1938[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1939; _temp1939.tag= Cyc_DeclaratorExpopt_tok;
_temp1939.f1=({ struct _tuple13* _temp1940=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1940->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1940->f2=({
struct Cyc_Core_Opt* _temp1941=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1941->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1941;}); _temp1940;});
_temp1939;}); _temp1938;}); break; case 109: _LL1937: { struct Cyc_List_List*
_temp1943=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1944=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1944[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1945; _temp1945.tag= Cyc_TypeSpecifier_tok;
_temp1945.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1946=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1946[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1947; _temp1947.tag= Cyc_Parse_Decl_spec;
_temp1947.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp1943,({
struct Cyc_Core_Opt* _temp1948=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1948->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1948;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1947;}); _temp1946;})); _temp1945;}); _temp1944;}); break;} case 110:
_LL1942: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1950=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1950[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1951; _temp1951.tag=
Cyc_TypeSpecifier_tok; _temp1951.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1952=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1952[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1953; _temp1953.tag= Cyc_Absyn_TunionType;
_temp1953.f1=({ struct Cyc_Absyn_TunionInfo _temp1954; _temp1954.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1955=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1955[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1956; _temp1956.tag= Cyc_Absyn_UnknownTunion;
_temp1956.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1957; _temp1957.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1957.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1957;});
_temp1956;}); _temp1955;})); _temp1954.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1954.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp1954;}); _temp1953;}); _temp1952;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1951;}); _temp1950;}); break; case 111: _LL1949: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1959=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1959[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1960; _temp1960.tag= Cyc_TypeSpecifier_tok;
_temp1960.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1961=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1961[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1962; _temp1962.tag=
Cyc_Absyn_TunionType; _temp1962.f1=({ struct Cyc_Absyn_TunionInfo _temp1963;
_temp1963.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1964=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1964[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1965; _temp1965.tag= Cyc_Absyn_UnknownTunion; _temp1965.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1966; _temp1966.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1966.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp1966;}); _temp1965;}); _temp1964;})); _temp1963.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1963.rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1963;}); _temp1962;}); _temp1961;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1960;}); _temp1959;}); break; case 112: _LL1958: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1968=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1968[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1969; _temp1969.tag= Cyc_TypeSpecifier_tok;
_temp1969.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1970=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1970[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1971; _temp1971.tag= Cyc_Absyn_TunionFieldType; _temp1971.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1972; _temp1972.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1973=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1973[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1974; _temp1974.tag= Cyc_Absyn_UnknownTunionfield; _temp1974.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1975; _temp1975.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1975.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1975.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1975;});
_temp1974;}); _temp1973;})); _temp1972.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1972;}); _temp1971;});
_temp1970;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1969;}); _temp1968;}); break; case 113:
_LL1967: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1977=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1977[ 0]=({ struct
Cyc_Bool_tok_struct _temp1978; _temp1978.tag= Cyc_Bool_tok; _temp1978.f1= 0;
_temp1978;}); _temp1977;}); break; case 114: _LL1976: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1980=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1980[ 0]=({ struct Cyc_Bool_tok_struct
_temp1981; _temp1981.tag= Cyc_Bool_tok; _temp1981.f1= 1; _temp1981;}); _temp1980;});
break; case 115: _LL1979: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1983=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1983[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1984; _temp1984.tag= Cyc_TunionFieldList_tok; _temp1984.f1=({ struct Cyc_List_List*
_temp1985=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1985->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1985->tl= 0;
_temp1985;}); _temp1984;}); _temp1983;}); break; case 116: _LL1982: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1987=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1987[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1988; _temp1988.tag= Cyc_TunionFieldList_tok;
_temp1988.f1=({ struct Cyc_List_List* _temp1989=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1989->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1989->tl=
0; _temp1989;}); _temp1988;}); _temp1987;}); break; case 117: _LL1986: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1991=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1991[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1992; _temp1992.tag= Cyc_TunionFieldList_tok;
_temp1992.f1=({ struct Cyc_List_List* _temp1993=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1993->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1993->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1993;}); _temp1992;}); _temp1991;}); break; case 118:
_LL1990: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1995=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1995[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1996; _temp1996.tag=
Cyc_TunionFieldList_tok; _temp1996.f1=({ struct Cyc_List_List* _temp1997=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1997->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1997->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1997;}); _temp1996;});
_temp1995;}); break; case 119: _LL1994: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1999=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1999[ 0]=({ struct Cyc_Scope_tok_struct _temp2000; _temp2000.tag= Cyc_Scope_tok;
_temp2000.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2000;}); _temp1999;});
break; case 120: _LL1998: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2002=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2002[ 0]=({ struct Cyc_Scope_tok_struct _temp2003; _temp2003.tag= Cyc_Scope_tok;
_temp2003.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2003;}); _temp2002;});
break; case 121: _LL2001: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2005=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2005[ 0]=({ struct Cyc_Scope_tok_struct _temp2006; _temp2006.tag= Cyc_Scope_tok;
_temp2006.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2006;}); _temp2005;});
break; case 122: _LL2004: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2008=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2008[ 0]=({ struct Cyc_TunionField_tok_struct _temp2009; _temp2009.tag= Cyc_TunionField_tok;
_temp2009.f1=({ struct Cyc_Absyn_Tunionfield* _temp2010=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2010->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2010->tvs= 0;
_temp2010->typs= 0; _temp2010->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2010->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2010;}); _temp2009;}); _temp2008;}); break; case 123:
_LL2007: { struct Cyc_List_List* _temp2012=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2013=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2014=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2014[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2015; _temp2015.tag= Cyc_TunionField_tok;
_temp2015.f1=({ struct Cyc_Absyn_Tunionfield* _temp2016=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2016->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2016->tvs=
_temp2013; _temp2016->typs= _temp2012; _temp2016->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2016->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2016;}); _temp2015;}); _temp2014;}); break;}
case 124: _LL2011: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL2017: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2019=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2019[ 0]=({ struct Cyc_Declarator_tok_struct _temp2020; _temp2020.tag= Cyc_Declarator_tok;
_temp2020.f1=({ struct Cyc_Parse_Declarator* _temp2021=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2021->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2021->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2021;});
_temp2020;}); _temp2019;}); break; case 126: _LL2018: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2023=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2023[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2024; _temp2024.tag= Cyc_Declarator_tok;
_temp2024.f1=({ struct Cyc_Parse_Declarator* _temp2025=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2025->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2025->tms= 0;
_temp2025;}); _temp2024;}); _temp2023;}); break; case 127: _LL2022: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 128:
_LL2026: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2028=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2028[ 0]=({ struct Cyc_Declarator_tok_struct _temp2029; _temp2029.tag= Cyc_Declarator_tok;
_temp2029.f1=({ struct Cyc_Parse_Declarator* _temp2030=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2030->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2030->tms=({ struct Cyc_List_List* _temp2031=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2031->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2031->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2031;}); _temp2030;}); _temp2029;});
_temp2028;}); break; case 129: _LL2027: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2033=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2033[ 0]=({ struct Cyc_Declarator_tok_struct _temp2034; _temp2034.tag= Cyc_Declarator_tok;
_temp2034.f1=({ struct Cyc_Parse_Declarator* _temp2035=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2035->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2035->tms=({ struct Cyc_List_List* _temp2036=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2036->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2037=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2037[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2038; _temp2038.tag= Cyc_Absyn_ConstArray_mod;
_temp2038.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2038;}); _temp2037;})); _temp2036->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2036;}); _temp2035;}); _temp2034;}); _temp2033;}); break; case 130: _LL2032: {
struct _tuple17 _temp2042; struct Cyc_List_List* _temp2043; struct Cyc_Core_Opt*
_temp2045; struct Cyc_Absyn_VarargInfo* _temp2047; int _temp2049; struct Cyc_List_List*
_temp2051; struct _tuple17* _temp2040= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2042=*
_temp2040; _LL2052: _temp2051= _temp2042.f1; goto _LL2050; _LL2050: _temp2049=
_temp2042.f2; goto _LL2048; _LL2048: _temp2047= _temp2042.f3; goto _LL2046;
_LL2046: _temp2045= _temp2042.f4; goto _LL2044; _LL2044: _temp2043= _temp2042.f5;
goto _LL2041; _LL2041: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2053=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2053[ 0]=({ struct Cyc_Declarator_tok_struct _temp2054; _temp2054.tag= Cyc_Declarator_tok;
_temp2054.f1=({ struct Cyc_Parse_Declarator* _temp2055=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2055->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2055->tms=({ struct Cyc_List_List* _temp2056=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2056->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2057=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2057[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2058; _temp2058.tag= Cyc_Absyn_Function_mod;
_temp2058.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2059=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2059[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2060; _temp2060.tag= Cyc_Absyn_WithTypes;
_temp2060.f1= _temp2051; _temp2060.f2= _temp2049; _temp2060.f3= _temp2047;
_temp2060.f4= _temp2045; _temp2060.f5= _temp2043; _temp2060;}); _temp2059;}));
_temp2058;}); _temp2057;})); _temp2056->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2056;});
_temp2055;}); _temp2054;}); _temp2053;}); break;} case 131: _LL2039: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2062=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2062[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2063; _temp2063.tag= Cyc_Declarator_tok;
_temp2063.f1=({ struct Cyc_Parse_Declarator* _temp2064=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2064->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2064->tms=({ struct Cyc_List_List* _temp2065=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2065->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2066=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2066[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2067; _temp2067.tag= Cyc_Absyn_Function_mod;
_temp2067.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2068=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2068[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2069; _temp2069.tag= Cyc_Absyn_WithTypes;
_temp2069.f1= 0; _temp2069.f2= 0; _temp2069.f3= 0; _temp2069.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2069.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2069;}); _temp2068;})); _temp2067;}); _temp2066;}));
_temp2065->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2065;}); _temp2064;}); _temp2063;});
_temp2062;}); break; case 132: _LL2061: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2071=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2071[ 0]=({ struct Cyc_Declarator_tok_struct _temp2072; _temp2072.tag= Cyc_Declarator_tok;
_temp2072.f1=({ struct Cyc_Parse_Declarator* _temp2073=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2073->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2073->tms=({ struct Cyc_List_List* _temp2074=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2074->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2075=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2075[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2076; _temp2076.tag= Cyc_Absyn_Function_mod;
_temp2076.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2077=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2077[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2078; _temp2078.tag= Cyc_Absyn_NoTypes;
_temp2078.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2078.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2078;});
_temp2077;})); _temp2076;}); _temp2075;})); _temp2074->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2074;}); _temp2073;}); _temp2072;}); _temp2071;}); break; case 133: _LL2070: {
struct Cyc_List_List* _temp2080=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2081=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2081[ 0]=({ struct Cyc_Declarator_tok_struct _temp2082; _temp2082.tag= Cyc_Declarator_tok;
_temp2082.f1=({ struct Cyc_Parse_Declarator* _temp2083=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2083->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2083->tms=({ struct Cyc_List_List* _temp2084=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2084->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2085=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2085[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2086; _temp2086.tag= Cyc_Absyn_TypeParams_mod;
_temp2086.f1= _temp2080; _temp2086.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2086.f3=
0; _temp2086;}); _temp2085;})); _temp2084->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2084;});
_temp2083;}); _temp2082;}); _temp2081;}); break;} case 134: _LL2079:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2088=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2089=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2089[ 0]=({ struct Cyc_Declarator_tok_struct _temp2090; _temp2090.tag= Cyc_Declarator_tok;
_temp2090.f1=({ struct Cyc_Parse_Declarator* _temp2091=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2091->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2091->tms=({ struct Cyc_List_List* _temp2092=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2092->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2093=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2093[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2094; _temp2094.tag= Cyc_Absyn_TypeParams_mod;
_temp2094.f1= _temp2088; _temp2094.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2094.f3=
0; _temp2094;}); _temp2093;})); _temp2092->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2092;});
_temp2091;}); _temp2090;}); _temp2089;}); break;} case 135: _LL2087: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2096=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2096[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2097; _temp2097.tag= Cyc_Declarator_tok;
_temp2097.f1=({ struct Cyc_Parse_Declarator* _temp2098=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2098->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2098->tms=({ struct Cyc_List_List* _temp2099=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2099->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2100=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2100[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2101; _temp2101.tag= Cyc_Absyn_Attributes_mod;
_temp2101.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2101.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2101;}); _temp2100;}));
_temp2099->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2099;}); _temp2098;}); _temp2097;});
_temp2096;}); break; case 136: _LL2095: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2103=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2103[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2104; _temp2104.tag= Cyc_Declarator_tok;
_temp2104.f1=({ struct Cyc_Parse_Declarator* _temp2105=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2105->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2105->tms= 0;
_temp2105;}); _temp2104;}); _temp2103;}); break; case 137: _LL2102: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2107=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2107[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2108; _temp2108.tag= Cyc_Declarator_tok;
_temp2108.f1=({ struct Cyc_Parse_Declarator* _temp2109=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2109->id= Cyc_Absyn_exn_name;
_temp2109->tms= 0; _temp2109;}); _temp2108;}); _temp2107;}); break; case 138:
_LL2106: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2111=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2111[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2112; _temp2112.tag=
Cyc_TypeModifierList_tok; _temp2112.f1=({ struct Cyc_List_List* _temp2113=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2113->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2114=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2114[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2115; _temp2115.tag= Cyc_Absyn_Pointer_mod;
_temp2115.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2115.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2115.f3= Cyc_Absyn_empty_tqual(); _temp2115;}); _temp2114;}));
_temp2113->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2113;});
_temp2112;}); _temp2111;}); break; case 139: _LL2110: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2117=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2117[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2118; _temp2118.tag= Cyc_TypeModifierList_tok;
_temp2118.f1=({ struct Cyc_List_List* _temp2119=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2119->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2120=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2120[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2121; _temp2121.tag= Cyc_Absyn_Pointer_mod;
_temp2121.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2121.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2121.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2121;}); _temp2120;})); _temp2119->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2119;});
_temp2118;}); _temp2117;}); break; case 140: _LL2116: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2123=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2123[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2124; _temp2124.tag= Cyc_TypeModifierList_tok;
_temp2124.f1=({ struct Cyc_List_List* _temp2125=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2125->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2126=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2126[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2127; _temp2127.tag= Cyc_Absyn_Pointer_mod;
_temp2127.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2127.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2127.f3= Cyc_Absyn_empty_tqual(); _temp2127;}); _temp2126;}));
_temp2125->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2125;});
_temp2124;}); _temp2123;}); break; case 141: _LL2122: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2129=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2129[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2130; _temp2130.tag= Cyc_TypeModifierList_tok;
_temp2130.f1=({ struct Cyc_List_List* _temp2131=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2131->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2132=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2132[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2133; _temp2133.tag= Cyc_Absyn_Pointer_mod;
_temp2133.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2133.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2133.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2133;}); _temp2132;})); _temp2131->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2131;});
_temp2130;}); _temp2129;}); break; case 142: _LL2128: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2135=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2135[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2136; _temp2136.tag= Cyc_Pointer_Sort_tok;
_temp2136.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2137=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2137[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2138; _temp2138.tag=
Cyc_Absyn_Nullable_ps; _temp2138.f1= Cyc_Absyn_exp_unsigned_one; _temp2138;});
_temp2137;})); _temp2136;}); _temp2135;}); break; case 143: _LL2134: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2140=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2140[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2141; _temp2141.tag= Cyc_Pointer_Sort_tok;
_temp2141.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2142=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2142[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2143; _temp2143.tag= Cyc_Absyn_NonNullable_ps; _temp2143.f1= Cyc_Absyn_exp_unsigned_one;
_temp2143;}); _temp2142;})); _temp2141;}); _temp2140;}); break; case 144:
_LL2139: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2145=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2145[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2146; _temp2146.tag=
Cyc_Pointer_Sort_tok; _temp2146.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2147=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2147[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2148; _temp2148.tag=
Cyc_Absyn_Nullable_ps; _temp2148.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2148;});
_temp2147;})); _temp2146;}); _temp2145;}); break; case 145: _LL2144: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2150=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2150[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2151; _temp2151.tag= Cyc_Pointer_Sort_tok;
_temp2151.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2152=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2152[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2153; _temp2153.tag= Cyc_Absyn_NonNullable_ps; _temp2153.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2153;});
_temp2152;})); _temp2151;}); _temp2150;}); break; case 146: _LL2149: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2155=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2155[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2156; _temp2156.tag= Cyc_Pointer_Sort_tok;
_temp2156.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2156;});
_temp2155;}); break; case 147: _LL2154: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2158=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2158[ 0]=({ struct Cyc_Rgn_tok_struct _temp2159; _temp2159.tag= Cyc_Rgn_tok;
_temp2159.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2159;}); _temp2158;});
break; case 148: _LL2157: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2160: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2162=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2162[ 0]=({ struct Cyc_Rgn_tok_struct _temp2163; _temp2163.tag= Cyc_Rgn_tok;
_temp2163.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2163;}); _temp2162;}); break; case 150: _LL2161:
if( Cyc_yyget_Kind_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind\n",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Rgn_tok_struct* _temp2165=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp2165[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2166; _temp2166.tag= Cyc_Rgn_tok; _temp2166.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind)); _temp2166;});
_temp2165;}); break; case 151: _LL2164: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2168=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2168[ 0]=({ struct Cyc_Rgn_tok_struct _temp2169; _temp2169.tag= Cyc_Rgn_tok;
_temp2169.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2170=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2170->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2170;}), 0); _temp2169;}); _temp2168;});
break; case 152: _LL2167: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 153: _LL2171: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2173=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2173[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2174; _temp2174.tag= Cyc_TypeQual_tok; _temp2174.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2174;});
_temp2173;}); break; case 154: _LL2172: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2176=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2176[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2177; _temp2177.tag= Cyc_ParamDeclListBool_tok; _temp2177.f1=({ struct
_tuple17* _temp2178=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2178->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2178->f2= 0; _temp2178->f3= 0; _temp2178->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2178->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2178;}); _temp2177;}); _temp2176;}); break; case 155:
_LL2175: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2180=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2180[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2181; _temp2181.tag=
Cyc_ParamDeclListBool_tok; _temp2181.f1=({ struct _tuple17* _temp2182=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2182->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2182->f2= 1;
_temp2182->f3= 0; _temp2182->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2182->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2182;});
_temp2181;}); _temp2180;}); break; case 156: _LL2179: { void* _temp2186; struct
Cyc_Absyn_Tqual _temp2188; struct Cyc_Core_Opt* _temp2190; struct _tuple2
_temp2184=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL2191: _temp2190= _temp2184.f1; goto _LL2189; _LL2189:
_temp2188= _temp2184.f2; goto _LL2187; _LL2187: _temp2186= _temp2184.f3; goto
_LL2185; _LL2185: { struct Cyc_Absyn_VarargInfo* _temp2192=({ struct Cyc_Absyn_VarargInfo*
_temp2196=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2196->name= _temp2190; _temp2196->tq= _temp2188; _temp2196->type=( void*)
_temp2186; _temp2196->rgn=( void*) Cyc_yyget_Rgn_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2196->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2196;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2193=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2193[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2194; _temp2194.tag= Cyc_ParamDeclListBool_tok; _temp2194.f1=({ struct
_tuple17* _temp2195=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2195->f1= 0; _temp2195->f2= 0; _temp2195->f3= _temp2192; _temp2195->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2195->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2195;}); _temp2194;}); _temp2193;}); break;}} case 157:
_LL2183: { void* _temp2200; struct Cyc_Absyn_Tqual _temp2202; struct Cyc_Core_Opt*
_temp2204; struct _tuple2 _temp2198=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL2205: _temp2204=
_temp2198.f1; goto _LL2203; _LL2203: _temp2202= _temp2198.f2; goto _LL2201;
_LL2201: _temp2200= _temp2198.f3; goto _LL2199; _LL2199: { struct Cyc_Absyn_VarargInfo*
_temp2206=({ struct Cyc_Absyn_VarargInfo* _temp2210=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2210->name= _temp2204;
_temp2210->tq= _temp2202; _temp2210->type=( void*) _temp2200; _temp2210->rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2210->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2210;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2207=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2207[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2208; _temp2208.tag= Cyc_ParamDeclListBool_tok;
_temp2208.f1=({ struct _tuple17* _temp2209=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2209->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)])); _temp2209->f2= 0;
_temp2209->f3= _temp2206; _temp2209->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2209->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2209;});
_temp2208;}); _temp2207;}); break;}} case 158: _LL2197: yyval=( void*)({ struct
Cyc_TypeOpt_tok_struct* _temp2212=( struct Cyc_TypeOpt_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2212[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2213; _temp2213.tag= Cyc_TypeOpt_tok; _temp2213.f1= 0; _temp2213;});
_temp2212;}); break; case 159: _LL2211: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2215=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2215[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2216; _temp2216.tag= Cyc_TypeOpt_tok;
_temp2216.f1=({ struct Cyc_Core_Opt* _temp2217=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2217->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2218=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2218[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2219; _temp2219.tag= Cyc_Absyn_JoinEff;
_temp2219.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2219;}); _temp2218;})); _temp2217;}); _temp2216;});
_temp2215;}); break; case 160: _LL2214: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2221=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2221[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2222; _temp2222.tag= Cyc_Rgnorder_tok;
_temp2222.f1= 0; _temp2222;}); _temp2221;}); break; case 161: _LL2220: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 162:
_LL2223: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2225=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2225[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2226; _temp2226.tag= Cyc_Rgnorder_tok; _temp2226.f1=({ struct Cyc_List_List*
_temp2227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2227->hd=( void*)({ struct _tuple19* _temp2228=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2228->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2228->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2228;}); _temp2227->tl= 0; _temp2227;});
_temp2226;}); _temp2225;}); break; case 163: _LL2224: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2230=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2230[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2231; _temp2231.tag= Cyc_Rgnorder_tok; _temp2231.f1=({ struct Cyc_List_List*
_temp2232=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2232->hd=( void*)({ struct _tuple19* _temp2233=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2233->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2233->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2233;}); _temp2232->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2232;});
_temp2231;}); _temp2230;}); break; case 164: _LL2229: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2235=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2235[ 0]=({ struct Cyc_Bool_tok_struct
_temp2236; _temp2236.tag= Cyc_Bool_tok; _temp2236.f1= 0; _temp2236;}); _temp2235;});
break; case 165: _LL2234: { struct _tagged_arr _temp2238= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2238, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2239=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2239[ 0]=({ struct Cyc_Bool_tok_struct _temp2240; _temp2240.tag= Cyc_Bool_tok;
_temp2240.f1= 1; _temp2240;}); _temp2239;}); break;} case 166: _LL2237: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 167:
_LL2241: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2243=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2243[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2244; _temp2244.tag= Cyc_TypeList_tok; _temp2244.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2244;});
_temp2243;}); break; case 168: _LL2242: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2246=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2246[ 0]=({ struct Cyc_TypeList_tok_struct _temp2247; _temp2247.tag= Cyc_TypeList_tok;
_temp2247.f1= 0; _temp2247;}); _temp2246;}); break; case 169: _LL2245: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
170: _LL2248: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2250=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2250[ 0]=({ struct Cyc_TypeList_tok_struct _temp2251; _temp2251.tag= Cyc_TypeList_tok;
_temp2251.f1=({ struct Cyc_List_List* _temp2252=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2252->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2252->tl= 0; _temp2252;});
_temp2251;}); _temp2250;}); break; case 171: _LL2249: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_EffKind){
Cyc_Parse_err( _tag_arr("expecing effect kind (E)", sizeof( unsigned char), 25u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2254=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2254[ 0]=({ struct Cyc_TypeList_tok_struct _temp2255; _temp2255.tag= Cyc_TypeList_tok;
_temp2255.f1=({ struct Cyc_List_List* _temp2256=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2256->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2256->tl= 0;
_temp2256;}); _temp2255;}); _temp2254;}); break; case 172: _LL2253: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2258=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2258[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2259; _temp2259.tag= Cyc_TypeList_tok; _temp2259.f1=({ struct Cyc_List_List*
_temp2260=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2260->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2261=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2261[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2262; _temp2262.tag= Cyc_Absyn_AccessEff;
_temp2262.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2262;}); _temp2261;})); _temp2260->tl= 0;
_temp2260;}); _temp2259;}); _temp2258;}); break; case 173: _LL2257: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2264=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2264[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2265; _temp2265.tag= Cyc_TypeList_tok; _temp2265.f1=({ struct Cyc_List_List*
_temp2266=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2266->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2267=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2267[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2268; _temp2268.tag= Cyc_Absyn_AccessEff;
_temp2268.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2268;}); _temp2267;})); _temp2266->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2266;});
_temp2265;}); _temp2264;}); break; case 174: _LL2263: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2270=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2270[ 0]=({ struct Cyc_TypeList_tok_struct _temp2271; _temp2271.tag= Cyc_TypeList_tok;
_temp2271.f1=({ struct Cyc_List_List* _temp2272=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2272->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2273=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2273[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2274; _temp2274.tag= Cyc_Absyn_AccessEff;
_temp2274.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2274;}); _temp2273;})); _temp2272->tl= 0;
_temp2272;}); _temp2271;}); _temp2270;}); break; case 175: _LL2269: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2276=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2276[ 0]=({ struct Cyc_TypeList_tok_struct _temp2277; _temp2277.tag= Cyc_TypeList_tok;
_temp2277.f1=({ struct Cyc_List_List* _temp2278=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2278->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2279=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2279[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2280; _temp2280.tag= Cyc_Absyn_AccessEff;
_temp2280.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2280;}); _temp2279;})); _temp2278->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2278;});
_temp2277;}); _temp2276;}); break; case 176: _LL2275: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2282=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2282[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2283; _temp2283.tag= Cyc_ParamDeclList_tok;
_temp2283.f1=({ struct Cyc_List_List* _temp2284=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2284->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2284->tl= 0;
_temp2284;}); _temp2283;}); _temp2282;}); break; case 177: _LL2281: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2286=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2286[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2287; _temp2287.tag= Cyc_ParamDeclList_tok;
_temp2287.f1=({ struct Cyc_List_List* _temp2288=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2288->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2288->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2288;}); _temp2287;}); _temp2286;}); break; case 178:
_LL2285: { void* _temp2290= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2291=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2292=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2293=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2294= Cyc_Parse_apply_tms( _temp2292, _temp2290, _temp2291,
_temp2293); if( _temp2294.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
_tuple1* _temp2295=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id;{ void* _temp2296=(*
_temp2295).f1; struct Cyc_List_List* _temp2306; struct Cyc_List_List* _temp2308;
_LL2298: if( _temp2296 ==( void*) Cyc_Absyn_Loc_n){ goto _LL2299;} else{ goto
_LL2300;} _LL2300: if(( unsigned int) _temp2296 > 1u?*(( int*) _temp2296) == Cyc_Absyn_Rel_n:
0){ _LL2307: _temp2306=(( struct Cyc_Absyn_Rel_n_struct*) _temp2296)->f1; if(
_temp2306 == 0){ goto _LL2301;} else{ goto _LL2302;}} else{ goto _LL2302;}
_LL2302: if(( unsigned int) _temp2296 > 1u?*(( int*) _temp2296) == Cyc_Absyn_Abs_n:
0){ _LL2309: _temp2308=(( struct Cyc_Absyn_Abs_n_struct*) _temp2296)->f1; if(
_temp2308 == 0){ goto _LL2303;} else{ goto _LL2304;}} else{ goto _LL2304;}
_LL2304: goto _LL2305; _LL2299: goto _LL2297; _LL2301: goto _LL2297; _LL2303:
goto _LL2297; _LL2305: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); goto
_LL2297; _LL2297:;}{ struct Cyc_Core_Opt* _temp2310=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2314=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2314->v=( void*)(* _temp2295).f2; _temp2314;}); if(
_temp2294.f4 != 0){ Cyc_Parse_warn( _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2294.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2311=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2311[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2312; _temp2312.tag= Cyc_ParamDecl_tok; _temp2312.f1=({
struct _tuple2* _temp2313=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2313->f1= _temp2310; _temp2313->f2= _temp2294.f1; _temp2313->f3= _temp2294.f2;
_temp2313;}); _temp2312;}); _temp2311;}); break;}}} case 179: _LL2289: { void*
_temp2316= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2317=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; struct Cyc_Absyn_Tqual _temp2318=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; if( _temp2317
!= 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring", sizeof(
unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2319=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2319[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2320; _temp2320.tag= Cyc_ParamDecl_tok; _temp2320.f1=({
struct _tuple2* _temp2321=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2321->f1= 0; _temp2321->f2= _temp2318; _temp2321->f3= _temp2316; _temp2321;});
_temp2320;}); _temp2319;}); break;} case 180: _LL2315: { void* _temp2323= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2324=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2325=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2326=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2327= Cyc_Parse_apply_tms( _temp2325, _temp2323, _temp2324,
_temp2326); if( _temp2327.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2327.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2328=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2328[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2329; _temp2329.tag= Cyc_ParamDecl_tok; _temp2329.f1=({
struct _tuple2* _temp2330=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2330->f1= 0; _temp2330->f2= _temp2327.f1; _temp2330->f3= _temp2327.f2;
_temp2330;}); _temp2329;}); _temp2328;}); break;} case 181: _LL2322: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2332=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2332[ 0]=({ struct Cyc_IdList_tok_struct
_temp2333; _temp2333.tag= Cyc_IdList_tok; _temp2333.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2333;});
_temp2332;}); break; case 182: _LL2331: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2335=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2335[ 0]=({ struct Cyc_IdList_tok_struct _temp2336; _temp2336.tag= Cyc_IdList_tok;
_temp2336.f1=({ struct Cyc_List_List* _temp2337=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2337->hd=( void*)({ struct
_tagged_arr* _temp2338=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2338[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2338;}); _temp2337->tl=
0; _temp2337;}); _temp2336;}); _temp2335;}); break; case 183: _LL2334: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2340=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2340[ 0]=({ struct Cyc_IdList_tok_struct
_temp2341; _temp2341.tag= Cyc_IdList_tok; _temp2341.f1=({ struct Cyc_List_List*
_temp2342=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2342->hd=( void*)({ struct _tagged_arr* _temp2343=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2343[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2343;});
_temp2342->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2342;}); _temp2341;}); _temp2340;}); break; case 184:
_LL2339: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 185: _LL2344: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 186: _LL2345: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2347=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2347[ 0]=({ struct Cyc_Exp_tok_struct _temp2348; _temp2348.tag= Cyc_Exp_tok;
_temp2348.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2349=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2349[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2350; _temp2350.tag= Cyc_Absyn_UnresolvedMem_e; _temp2350.f1= 0; _temp2350.f2=
0; _temp2350;}); _temp2349;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2348;});
_temp2347;}); break; case 187: _LL2346: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2352=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2352[ 0]=({ struct Cyc_Exp_tok_struct _temp2353; _temp2353.tag= Cyc_Exp_tok;
_temp2353.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2354=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2354[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2355; _temp2355.tag= Cyc_Absyn_UnresolvedMem_e; _temp2355.f1= 0; _temp2355.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2355;});
_temp2354;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2353;}); _temp2352;}); break; case 188:
_LL2351: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2357=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2357[ 0]=({ struct Cyc_Exp_tok_struct
_temp2358; _temp2358.tag= Cyc_Exp_tok; _temp2358.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2359=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2359[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2360; _temp2360.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2360.f1= 0; _temp2360.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2360;});
_temp2359;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2358;}); _temp2357;}); break; case 189:
_LL2356: { struct Cyc_Absyn_Vardecl* _temp2362= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2368=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2368->f1=( void*) Cyc_Absyn_Loc_n; _temp2368->f2=({ struct _tagged_arr*
_temp2369=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp2369[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2369;}); _temp2368;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2362->tq=({ struct Cyc_Absyn_Tqual _temp2363; _temp2363.q_const= 1;
_temp2363.q_volatile= 0; _temp2363.q_restrict= 1; _temp2363;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2364=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2364[ 0]=({ struct Cyc_Exp_tok_struct
_temp2365; _temp2365.tag= Cyc_Exp_tok; _temp2365.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2366=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2366[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2367; _temp2367.tag= Cyc_Absyn_Comprehension_e;
_temp2367.f1= _temp2362; _temp2367.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2367.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2367;});
_temp2366;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2365;}); _temp2364;}); break;} case
190: _LL2361: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2371=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2371[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2372; _temp2372.tag=
Cyc_InitializerList_tok; _temp2372.f1=({ struct Cyc_List_List* _temp2373=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2373->hd=(
void*)({ struct _tuple20* _temp2374=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2374->f1= 0; _temp2374->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2374;}); _temp2373->tl=
0; _temp2373;}); _temp2372;}); _temp2371;}); break; case 191: _LL2370: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2376=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2376[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2377; _temp2377.tag= Cyc_InitializerList_tok;
_temp2377.f1=({ struct Cyc_List_List* _temp2378=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2378->hd=( void*)({ struct
_tuple20* _temp2379=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2379->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2379->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2379;});
_temp2378->tl= 0; _temp2378;}); _temp2377;}); _temp2376;}); break; case 192:
_LL2375: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2381=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2381[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2382; _temp2382.tag=
Cyc_InitializerList_tok; _temp2382.f1=({ struct Cyc_List_List* _temp2383=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2383->hd=(
void*)({ struct _tuple20* _temp2384=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2384->f1= 0; _temp2384->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2384;}); _temp2383->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2383;}); _temp2382;}); _temp2381;}); break; case 193:
_LL2380: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2386=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2386[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2387; _temp2387.tag=
Cyc_InitializerList_tok; _temp2387.f1=({ struct Cyc_List_List* _temp2388=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2388->hd=(
void*)({ struct _tuple20* _temp2389=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2389->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2389->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2389;});
_temp2388->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2388;});
_temp2387;}); _temp2386;}); break; case 194: _LL2385: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2391=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2391[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2392; _temp2392.tag= Cyc_DesignatorList_tok;
_temp2392.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2392;});
_temp2391;}); break; case 195: _LL2390: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2394=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2394[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2395; _temp2395.tag=
Cyc_DesignatorList_tok; _temp2395.f1=({ struct Cyc_List_List* _temp2396=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2396->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2396->tl= 0; _temp2396;}); _temp2395;}); _temp2394;});
break; case 196: _LL2393: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2398=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2398[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2399; _temp2399.tag=
Cyc_DesignatorList_tok; _temp2399.f1=({ struct Cyc_List_List* _temp2400=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2400->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2400->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2400;});
_temp2399;}); _temp2398;}); break; case 197: _LL2397: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2402=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2402[ 0]=({ struct
Cyc_Designator_tok_struct _temp2403; _temp2403.tag= Cyc_Designator_tok;
_temp2403.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2404=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2404[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2405; _temp2405.tag=
Cyc_Absyn_ArrayElement; _temp2405.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2405;});
_temp2404;})); _temp2403;}); _temp2402;}); break; case 198: _LL2401: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2407=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2407[ 0]=({ struct
Cyc_Designator_tok_struct _temp2408; _temp2408.tag= Cyc_Designator_tok;
_temp2408.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2409=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2409[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2410; _temp2410.tag= Cyc_Absyn_FieldName;
_temp2410.f1=({ struct _tagged_arr* _temp2411=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2411[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2411;}); _temp2410;});
_temp2409;})); _temp2408;}); _temp2407;}); break; case 199: _LL2406: { void*
_temp2413= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2414=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2414 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2415=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2416=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2416[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2417; _temp2417.tag= Cyc_ParamDecl_tok; _temp2417.f1=({
struct _tuple2* _temp2418=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp2418[ 0]=({ struct _tuple2 _temp2419; _temp2419.f1= 0; _temp2419.f2=
_temp2415; _temp2419.f3= _temp2413; _temp2419;}); _temp2418;}); _temp2417;});
_temp2416;}); break;}} case 200: _LL2412: { void* _temp2421= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2422=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2423=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2424=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2425= Cyc_Parse_apply_tms( _temp2423, _temp2421, _temp2422,
_temp2424); if( _temp2425.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2425.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2426=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2426[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2427; _temp2427.tag= Cyc_ParamDecl_tok; _temp2427.f1=({
struct _tuple2* _temp2428=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2428->f1= 0; _temp2428->f2= _temp2425.f1; _temp2428->f3= _temp2425.f2;
_temp2428;}); _temp2427;}); _temp2426;}); break;} case 201: _LL2420: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2430=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2430[ 0]=({ struct Cyc_Type_tok_struct
_temp2431; _temp2431.tag= Cyc_Type_tok; _temp2431.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2431;});
_temp2430;}); break; case 202: _LL2429: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2433=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2433[ 0]=({ struct Cyc_Type_tok_struct _temp2434; _temp2434.tag= Cyc_Type_tok;
_temp2434.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2435=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2435[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2436; _temp2436.tag= Cyc_Absyn_JoinEff;
_temp2436.f1= 0; _temp2436;}); _temp2435;})); _temp2434;}); _temp2433;}); break;
case 203: _LL2432: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2438=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2438[ 0]=({ struct Cyc_Type_tok_struct _temp2439; _temp2439.tag= Cyc_Type_tok;
_temp2439.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2440=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2440[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2441; _temp2441.tag= Cyc_Absyn_JoinEff;
_temp2441.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2441;}); _temp2440;})); _temp2439;});
_temp2438;}); break; case 204: _LL2437: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2443=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2443[ 0]=({ struct Cyc_Type_tok_struct _temp2444; _temp2444.tag= Cyc_Type_tok;
_temp2444.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2445=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2445[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2446; _temp2446.tag= Cyc_Absyn_JoinEff;
_temp2446.f1=({ struct Cyc_List_List* _temp2447=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2447->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2447->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2447;}); _temp2446;}); _temp2445;})); _temp2444;});
_temp2443;}); break; case 205: _LL2442: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2449=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2449[ 0]=({ struct Cyc_TypeList_tok_struct _temp2450; _temp2450.tag= Cyc_TypeList_tok;
_temp2450.f1=({ struct Cyc_List_List* _temp2451=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2451->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2451->tl= 0;
_temp2451;}); _temp2450;}); _temp2449;}); break; case 206: _LL2448: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2453=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2453[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2454; _temp2454.tag= Cyc_TypeList_tok; _temp2454.f1=({ struct Cyc_List_List*
_temp2455=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2455->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2455->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2455;});
_temp2454;}); _temp2453;}); break; case 207: _LL2452: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2457=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2457[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2458; _temp2458.tag= Cyc_AbstractDeclarator_tok;
_temp2458.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2459=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2459->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2459;});
_temp2458;}); _temp2457;}); break; case 208: _LL2456: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2460:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2462=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2462[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2463; _temp2463.tag=
Cyc_AbstractDeclarator_tok; _temp2463.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2464=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2464->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2464;});
_temp2463;}); _temp2462;}); break; case 210: _LL2461: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 211:
_LL2465: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2467=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2467[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2468; _temp2468.tag=
Cyc_AbstractDeclarator_tok; _temp2468.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2469=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2469->tms=({ struct Cyc_List_List* _temp2470=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2470->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2470->tl= 0; _temp2470;}); _temp2469;}); _temp2468;}); _temp2467;}); break;
case 212: _LL2466: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2472=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2472[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2473; _temp2473.tag= Cyc_AbstractDeclarator_tok; _temp2473.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2474=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2474->tms=({ struct Cyc_List_List* _temp2475=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2475->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2475->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2475;});
_temp2474;}); _temp2473;}); _temp2472;}); break; case 213: _LL2471: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2477=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2477[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2478; _temp2478.tag= Cyc_AbstractDeclarator_tok;
_temp2478.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2479=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2479->tms=({
struct Cyc_List_List* _temp2480=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2480->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2481=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2481[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2482; _temp2482.tag= Cyc_Absyn_ConstArray_mod; _temp2482.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2482;});
_temp2481;})); _temp2480->tl= 0; _temp2480;}); _temp2479;}); _temp2478;});
_temp2477;}); break; case 214: _LL2476: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2484=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2484[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2485; _temp2485.tag= Cyc_AbstractDeclarator_tok; _temp2485.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2486=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2486->tms=({ struct Cyc_List_List* _temp2487=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2487->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2488=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2488[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2489; _temp2489.tag= Cyc_Absyn_ConstArray_mod;
_temp2489.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2489;}); _temp2488;})); _temp2487->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2487;}); _temp2486;}); _temp2485;}); _temp2484;}); break; case 215: _LL2483:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2491=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2491[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2492; _temp2492.tag=
Cyc_AbstractDeclarator_tok; _temp2492.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2493=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2493->tms=({ struct Cyc_List_List* _temp2494=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2494->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2495=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2495[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2496; _temp2496.tag= Cyc_Absyn_Function_mod;
_temp2496.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2497=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2497[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2498; _temp2498.tag= Cyc_Absyn_WithTypes;
_temp2498.f1= 0; _temp2498.f2= 0; _temp2498.f3= 0; _temp2498.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2498.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2498;}); _temp2497;})); _temp2496;}); _temp2495;}));
_temp2494->tl= 0; _temp2494;}); _temp2493;}); _temp2492;}); _temp2491;}); break;
case 216: _LL2490: { struct _tuple17 _temp2502; struct Cyc_List_List* _temp2503;
struct Cyc_Core_Opt* _temp2505; struct Cyc_Absyn_VarargInfo* _temp2507; int
_temp2509; struct Cyc_List_List* _temp2511; struct _tuple17* _temp2500= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2502=*
_temp2500; _LL2512: _temp2511= _temp2502.f1; goto _LL2510; _LL2510: _temp2509=
_temp2502.f2; goto _LL2508; _LL2508: _temp2507= _temp2502.f3; goto _LL2506;
_LL2506: _temp2505= _temp2502.f4; goto _LL2504; _LL2504: _temp2503= _temp2502.f5;
goto _LL2501; _LL2501: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2513=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2513[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2514; _temp2514.tag= Cyc_AbstractDeclarator_tok; _temp2514.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2515=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2515->tms=({ struct Cyc_List_List* _temp2516=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2516->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2517=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2517[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2518; _temp2518.tag= Cyc_Absyn_Function_mod;
_temp2518.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2519=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2519[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2520; _temp2520.tag= Cyc_Absyn_WithTypes;
_temp2520.f1= _temp2511; _temp2520.f2= _temp2509; _temp2520.f3= _temp2507;
_temp2520.f4= _temp2505; _temp2520.f5= _temp2503; _temp2520;}); _temp2519;}));
_temp2518;}); _temp2517;})); _temp2516->tl= 0; _temp2516;}); _temp2515;});
_temp2514;}); _temp2513;}); break;} case 217: _LL2499: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2522=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2522[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2523; _temp2523.tag= Cyc_AbstractDeclarator_tok;
_temp2523.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2524=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2524->tms=({
struct Cyc_List_List* _temp2525=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2525->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2526=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2526[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2527; _temp2527.tag=
Cyc_Absyn_Function_mod; _temp2527.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2528=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2528[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2529; _temp2529.tag= Cyc_Absyn_WithTypes;
_temp2529.f1= 0; _temp2529.f2= 0; _temp2529.f3= 0; _temp2529.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2529.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2529;}); _temp2528;})); _temp2527;}); _temp2526;}));
_temp2525->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2525;});
_temp2524;}); _temp2523;}); _temp2522;}); break; case 218: _LL2521: { struct
_tuple17 _temp2533; struct Cyc_List_List* _temp2534; struct Cyc_Core_Opt*
_temp2536; struct Cyc_Absyn_VarargInfo* _temp2538; int _temp2540; struct Cyc_List_List*
_temp2542; struct _tuple17* _temp2531= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2533=*
_temp2531; _LL2543: _temp2542= _temp2533.f1; goto _LL2541; _LL2541: _temp2540=
_temp2533.f2; goto _LL2539; _LL2539: _temp2538= _temp2533.f3; goto _LL2537;
_LL2537: _temp2536= _temp2533.f4; goto _LL2535; _LL2535: _temp2534= _temp2533.f5;
goto _LL2532; _LL2532: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2544=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2544[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2545; _temp2545.tag= Cyc_AbstractDeclarator_tok; _temp2545.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2546=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2546->tms=({ struct Cyc_List_List* _temp2547=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2547->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2548=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2548[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2549; _temp2549.tag= Cyc_Absyn_Function_mod;
_temp2549.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2550=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2550[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2551; _temp2551.tag= Cyc_Absyn_WithTypes;
_temp2551.f1= _temp2542; _temp2551.f2= _temp2540; _temp2551.f3= _temp2538;
_temp2551.f4= _temp2536; _temp2551.f5= _temp2534; _temp2551;}); _temp2550;}));
_temp2549;}); _temp2548;})); _temp2547->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2547;}); _temp2546;}); _temp2545;}); _temp2544;}); break;} case 219:
_LL2530: { struct Cyc_List_List* _temp2553=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2554=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2554[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2555; _temp2555.tag= Cyc_AbstractDeclarator_tok; _temp2555.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2556=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2556->tms=({ struct Cyc_List_List* _temp2557=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2557->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2558=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2558[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2559; _temp2559.tag= Cyc_Absyn_TypeParams_mod;
_temp2559.f1= _temp2553; _temp2559.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2559.f3=
0; _temp2559;}); _temp2558;})); _temp2557->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2557;}); _temp2556;}); _temp2555;}); _temp2554;}); break;} case 220:
_LL2552:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2561=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2562=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2562[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2563; _temp2563.tag= Cyc_AbstractDeclarator_tok; _temp2563.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2564=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2564->tms=({ struct Cyc_List_List* _temp2565=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2565->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2566=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2566[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2567; _temp2567.tag= Cyc_Absyn_TypeParams_mod;
_temp2567.f1= _temp2561; _temp2567.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2567.f3=
0; _temp2567;}); _temp2566;})); _temp2565->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2565;}); _temp2564;}); _temp2563;}); _temp2562;}); break;} case 221:
_LL2560: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2569=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2569[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2570; _temp2570.tag=
Cyc_AbstractDeclarator_tok; _temp2570.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2571=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2571->tms=({ struct Cyc_List_List* _temp2572=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2572->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2573=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2573[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2574; _temp2574.tag= Cyc_Absyn_Attributes_mod;
_temp2574.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2574.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2574;}); _temp2573;}));
_temp2572->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms; _temp2572;});
_temp2571;}); _temp2570;}); _temp2569;}); break; case 222: _LL2568: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 223: _LL2575:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
224: _LL2576: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 225: _LL2577: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 226: _LL2578: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 227: _LL2579:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
228: _LL2580: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2592=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2592->name=({ struct
_tagged_arr* _temp2593=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2593[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2593;});
_temp2592->identity= 0; _temp2592->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2592;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2590=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2590[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2591; _temp2591.tag= Cyc_Absyn_VarType;
_temp2591.f1= tv; _temp2591;}); _temp2590;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2582=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2582[ 0]=({ struct Cyc_Stmt_tok_struct _temp2583; _temp2583.tag= Cyc_Stmt_tok;
_temp2583.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2584=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2584[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2585; _temp2585.tag= Cyc_Absyn_Region_s;
_temp2585.f1= tv; _temp2585.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2586=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2586->f1=(
void*) Cyc_Absyn_Loc_n; _temp2586->f2=({ struct _tagged_arr* _temp2587=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2587[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2587;});
_temp2586;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2588=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2588[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2589; _temp2589.tag=
Cyc_Absyn_RgnHandleType; _temp2589.f1=( void*) t; _temp2589;}); _temp2588;}), 0);
_temp2585.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2585;}); _temp2584;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2583;});
_temp2582;}); break;} case 229: _LL2581: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2605=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2605->name=({ struct
_tagged_arr* _temp2606=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2606[ 0]=( struct _tagged_arr)({ struct _tagged_arr
_temp2607= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); xprintf("`%.*s", _get_arr_size( _temp2607, 1u), _temp2607.curr);});
_temp2606;}); _temp2605->identity= 0; _temp2605->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2605;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2603=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2603[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2604; _temp2604.tag= Cyc_Absyn_VarType;
_temp2604.f1= tv; _temp2604;}); _temp2603;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2595=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2595[ 0]=({ struct Cyc_Stmt_tok_struct _temp2596; _temp2596.tag= Cyc_Stmt_tok;
_temp2596.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2597=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2597[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2598; _temp2598.tag= Cyc_Absyn_Region_s;
_temp2598.f1= tv; _temp2598.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2599=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2599->f1=(
void*) Cyc_Absyn_Loc_n; _temp2599->f2=({ struct _tagged_arr* _temp2600=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2600[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2600;});
_temp2599;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2601=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2601[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2602; _temp2602.tag=
Cyc_Absyn_RgnHandleType; _temp2602.f1=( void*) t; _temp2602;}); _temp2601;}), 0);
_temp2598.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2598;}); _temp2597;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2596;});
_temp2595;}); break;} case 230: _LL2594: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2609=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2609[ 0]=({ struct Cyc_Stmt_tok_struct _temp2610; _temp2610.tag= Cyc_Stmt_tok;
_temp2610.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2611=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2611[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2612; _temp2612.tag= Cyc_Absyn_Cut_s;
_temp2612.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2612;}); _temp2611;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2610;});
_temp2609;}); break; case 231: _LL2608: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2614=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2614[ 0]=({ struct Cyc_Stmt_tok_struct _temp2615; _temp2615.tag= Cyc_Stmt_tok;
_temp2615.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2616=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2616[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2617; _temp2617.tag= Cyc_Absyn_Splice_s;
_temp2617.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2617;}); _temp2616;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2615;});
_temp2614;}); break; case 232: _LL2613: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2619=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2619[ 0]=({ struct Cyc_Stmt_tok_struct _temp2620; _temp2620.tag= Cyc_Stmt_tok;
_temp2620.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2621=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2621[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2622; _temp2622.tag= Cyc_Absyn_Label_s;
_temp2622.f1=({ struct _tagged_arr* _temp2623=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2623[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2623;});
_temp2622.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2622;}); _temp2621;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2620;});
_temp2619;}); break; case 233: _LL2618: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2625=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2625[ 0]=({ struct Cyc_Stmt_tok_struct _temp2626; _temp2626.tag= Cyc_Stmt_tok;
_temp2626.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2626;});
_temp2625;}); break; case 234: _LL2624: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2628=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2628[ 0]=({ struct Cyc_Stmt_tok_struct _temp2629; _temp2629.tag= Cyc_Stmt_tok;
_temp2629.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2629;}); _temp2628;}); break; case 235: _LL2627: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2631=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2631[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2632; _temp2632.tag= Cyc_Stmt_tok; _temp2632.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2632;}); _temp2631;}); break; case 236: _LL2630: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 237:
_LL2633:{ void* _temp2635= Cyc_yyget_BlockItem_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); struct Cyc_List_List*
_temp2643; struct Cyc_Absyn_Fndecl* _temp2645; struct Cyc_Absyn_Stmt* _temp2647;
_LL2637: if(*(( int*) _temp2635) == Cyc_Parse_TopDecls_bl){ _LL2644: _temp2643=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2635)->f1; goto _LL2638;} else{ goto
_LL2639;} _LL2639: if(*(( int*) _temp2635) == Cyc_Parse_FnDecl_bl){ _LL2646:
_temp2645=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2635)->f1; goto _LL2640;}
else{ goto _LL2641;} _LL2641: if(*(( int*) _temp2635) == Cyc_Parse_Stmt_bl){
_LL2648: _temp2647=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2635)->f1; goto
_LL2642;} else{ goto _LL2636;} _LL2638: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2649=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2649[ 0]=({ struct Cyc_Stmt_tok_struct _temp2650; _temp2650.tag= Cyc_Stmt_tok;
_temp2650.f1= Cyc_Parse_flatten_declarations( _temp2643, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2650;}); _temp2649;}); goto _LL2636; _LL2640:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2651=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2651[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2652; _temp2652.tag= Cyc_Stmt_tok; _temp2652.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2653=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2653[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2654; _temp2654.tag= Cyc_Absyn_Fn_d;
_temp2654.f1= _temp2645; _temp2654;}); _temp2653;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2652;}); _temp2651;}); goto _LL2636; _LL2642:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2655=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2655[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2656; _temp2656.tag= Cyc_Stmt_tok; _temp2656.f1= _temp2647; _temp2656;});
_temp2655;}); goto _LL2636; _LL2636:;} break; case 238: _LL2634:{ void*
_temp2658= Cyc_yyget_BlockItem_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); struct Cyc_List_List* _temp2666; struct Cyc_Absyn_Fndecl*
_temp2668; struct Cyc_Absyn_Stmt* _temp2670; _LL2660: if(*(( int*) _temp2658) ==
Cyc_Parse_TopDecls_bl){ _LL2667: _temp2666=(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp2658)->f1; goto _LL2661;} else{ goto _LL2662;} _LL2662: if(*(( int*)
_temp2658) == Cyc_Parse_FnDecl_bl){ _LL2669: _temp2668=(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp2658)->f1; goto _LL2663;} else{ goto _LL2664;} _LL2664: if(*(( int*)
_temp2658) == Cyc_Parse_Stmt_bl){ _LL2671: _temp2670=(( struct Cyc_Parse_Stmt_bl_struct*)
_temp2658)->f1; goto _LL2665;} else{ goto _LL2659;} _LL2661: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2672=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2672[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2673; _temp2673.tag= Cyc_Stmt_tok; _temp2673.f1= Cyc_Parse_flatten_declarations(
_temp2666, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2673;}); _temp2672;}); goto _LL2659; _LL2663: yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2674=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2674[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2675; _temp2675.tag= Cyc_Stmt_tok; _temp2675.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2676=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2676[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2677; _temp2677.tag= Cyc_Absyn_Fn_d;
_temp2677.f1= _temp2668; _temp2677;}); _temp2676;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2675;}); _temp2674;}); goto _LL2659; _LL2665: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2678=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2678[ 0]=({ struct Cyc_Stmt_tok_struct _temp2679; _temp2679.tag= Cyc_Stmt_tok;
_temp2679.f1= Cyc_Absyn_seq_stmt( _temp2670, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2679;}); _temp2678;}); goto _LL2659; _LL2659:;} break; case 239: _LL2657:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2681=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp2681[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp2682; _temp2682.tag= Cyc_BlockItem_tok; _temp2682.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp2683=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp2683[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp2684; _temp2684.tag= Cyc_Parse_TopDecls_bl;
_temp2684.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2684;}); _temp2683;})); _temp2682;}); _temp2681;});
break; case 240: _LL2680: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp2686=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2686[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2687; _temp2687.tag= Cyc_BlockItem_tok;
_temp2687.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp2688=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp2688[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp2689; _temp2689.tag= Cyc_Parse_Stmt_bl;
_temp2689.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2689;}); _temp2688;})); _temp2687;}); _temp2686;}); break;
case 241: _LL2685: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2691=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2691[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2692; _temp2692.tag= Cyc_BlockItem_tok;
_temp2692.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp2693=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp2693[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp2694; _temp2694.tag= Cyc_Parse_FnDecl_bl;
_temp2694.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2694;}); _temp2693;})); _temp2692;}); _temp2691;}); break;
case 242: _LL2690: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2696=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2696[ 0]=({ struct Cyc_Stmt_tok_struct _temp2697; _temp2697.tag= Cyc_Stmt_tok;
_temp2697.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2697;}); _temp2696;}); break; case 243:
_LL2695: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2699=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2699[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2700; _temp2700.tag= Cyc_Stmt_tok; _temp2700.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2700;});
_temp2699;}); break; case 244: _LL2698: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2702=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2702[ 0]=({ struct Cyc_Stmt_tok_struct _temp2703; _temp2703.tag= Cyc_Stmt_tok;
_temp2703.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2703;}); _temp2702;}); break; case 245: _LL2701: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2705=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2705[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2706; _temp2706.tag= Cyc_Stmt_tok; _temp2706.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2707=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2707[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2708; _temp2708.tag= Cyc_Absyn_SwitchC_s;
_temp2708.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2708.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2708;});
_temp2707;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2706;}); _temp2705;}); break; case 246:
_LL2704: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2710=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2710[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2711; _temp2711.tag= Cyc_Stmt_tok; _temp2711.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2711;});
_temp2710;}); break; case 247: _LL2709: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2713=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2713[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2714; _temp2714.tag= Cyc_SwitchClauseList_tok; _temp2714.f1= 0; _temp2714;});
_temp2713;}); break; case 248: _LL2712: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2716=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2716[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2717; _temp2717.tag= Cyc_SwitchClauseList_tok; _temp2717.f1=({ struct Cyc_List_List*
_temp2718=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2718->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2719=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2719->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2719->pat_vars= 0; _temp2719->where_clause= 0; _temp2719->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2719->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2719;}); _temp2718->tl= 0; _temp2718;});
_temp2717;}); _temp2716;}); break; case 249: _LL2715: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2721=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2721[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2722; _temp2722.tag= Cyc_SwitchClauseList_tok;
_temp2722.f1=({ struct Cyc_List_List* _temp2723=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2723->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2724=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2724->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2724->pat_vars= 0; _temp2724->where_clause= 0;
_temp2724->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2724->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2724;});
_temp2723->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2723;}); _temp2722;});
_temp2721;}); break; case 250: _LL2720: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2726=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2726[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2727; _temp2727.tag= Cyc_SwitchClauseList_tok; _temp2727.f1=({ struct Cyc_List_List*
_temp2728=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2728->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2729=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2729->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2729->pat_vars=
0; _temp2729->where_clause= 0; _temp2729->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2729->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2729;}); _temp2728->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2728;}); _temp2727;});
_temp2726;}); break; case 251: _LL2725: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2731=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2731[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2732; _temp2732.tag= Cyc_SwitchClauseList_tok; _temp2732.f1=({ struct Cyc_List_List*
_temp2733=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2733->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2734=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2734->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2734->pat_vars=
0; _temp2734->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2734->body=
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2734->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2734;});
_temp2733->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2733;}); _temp2732;});
_temp2731;}); break; case 252: _LL2730: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2736=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2736[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2737; _temp2737.tag= Cyc_SwitchClauseList_tok; _temp2737.f1=({ struct Cyc_List_List*
_temp2738=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2738->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2739=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2739->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2739->pat_vars=
0; _temp2739->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2739->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2739->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2739;});
_temp2738->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2738;}); _temp2737;});
_temp2736;}); break; case 253: _LL2735: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2741=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2741[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2742; _temp2742.tag= Cyc_SwitchCClauseList_tok; _temp2742.f1= 0; _temp2742;});
_temp2741;}); break; case 254: _LL2740: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2744=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2744[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2745; _temp2745.tag= Cyc_SwitchCClauseList_tok; _temp2745.f1=({ struct Cyc_List_List*
_temp2746=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2746->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2747=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2747->cnst_exp= 0;
_temp2747->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2747->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2747;});
_temp2746->tl= 0; _temp2746;}); _temp2745;}); _temp2744;}); break; case 255:
_LL2743: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2749=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2749[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2750; _temp2750.tag=
Cyc_SwitchCClauseList_tok; _temp2750.f1=({ struct Cyc_List_List* _temp2751=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2751->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2752=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2752->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2752->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2752->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2752;});
_temp2751->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2751;}); _temp2750;});
_temp2749;}); break; case 256: _LL2748: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2754=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2754[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2755; _temp2755.tag= Cyc_SwitchCClauseList_tok; _temp2755.f1=({ struct Cyc_List_List*
_temp2756=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2756->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2757=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2757->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2757->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2757->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2757;});
_temp2756->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2756;}); _temp2755;});
_temp2754;}); break; case 257: _LL2753: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2759=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2759[ 0]=({ struct Cyc_Stmt_tok_struct _temp2760; _temp2760.tag= Cyc_Stmt_tok;
_temp2760.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2760;}); _temp2759;}); break; case 258: _LL2758: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2762=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2762[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2763; _temp2763.tag= Cyc_Stmt_tok; _temp2763.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2763;}); _temp2762;}); break; case 259: _LL2761: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2765=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2765[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2766; _temp2766.tag= Cyc_Stmt_tok; _temp2766.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2766;}); _temp2765;}); break; case 260: _LL2764: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2768=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2768[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2769; _temp2769.tag= Cyc_Stmt_tok; _temp2769.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2769;}); _temp2768;}); break; case 261: _LL2767: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2771=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2771[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2772; _temp2772.tag= Cyc_Stmt_tok; _temp2772.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2772;}); _temp2771;}); break; case 262: _LL2770: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2774=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2774[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2775; _temp2775.tag= Cyc_Stmt_tok; _temp2775.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2775;}); _temp2774;}); break; case 263: _LL2773: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2777=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2777[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2778; _temp2778.tag= Cyc_Stmt_tok; _temp2778.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2778;}); _temp2777;}); break; case 264: _LL2776: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2780=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2780[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2781; _temp2781.tag= Cyc_Stmt_tok; _temp2781.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2781;});
_temp2780;}); break; case 265: _LL2779: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2783=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2783[ 0]=({ struct Cyc_Stmt_tok_struct _temp2784; _temp2784.tag= Cyc_Stmt_tok;
_temp2784.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2784;});
_temp2783;}); break; case 266: _LL2782: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2786=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2786[ 0]=({ struct Cyc_Stmt_tok_struct _temp2787; _temp2787.tag= Cyc_Stmt_tok;
_temp2787.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2787;}); _temp2786;}); break; case 267: _LL2785: { struct Cyc_List_List*
_temp2789= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2790= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2791=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2791[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2792; _temp2792.tag= Cyc_Stmt_tok; _temp2792.f1= Cyc_Parse_flatten_declarations(
_temp2789, _temp2790); _temp2792;}); _temp2791;}); break;} case 268: _LL2788: {
struct Cyc_List_List* _temp2794= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2795= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2796=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2796[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2797; _temp2797.tag= Cyc_Stmt_tok; _temp2797.f1= Cyc_Parse_flatten_declarations(
_temp2794, _temp2795); _temp2797;}); _temp2796;}); break;} case 269: _LL2793: {
struct Cyc_List_List* _temp2799= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2800= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2801=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2801[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2802; _temp2802.tag= Cyc_Stmt_tok; _temp2802.f1= Cyc_Parse_flatten_declarations(
_temp2799, _temp2800); _temp2802;}); _temp2801;}); break;} case 270: _LL2798: {
struct Cyc_List_List* _temp2804= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2805= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2806=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2806[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2807; _temp2807.tag= Cyc_Stmt_tok; _temp2807.f1= Cyc_Parse_flatten_declarations(
_temp2804, _temp2805); _temp2807;}); _temp2806;}); break;} case 271: _LL2803:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2809=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2809[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2810; _temp2810.tag= Cyc_Stmt_tok; _temp2810.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2811=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2811[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2811;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2810;}); _temp2809;}); break; case 272: _LL2808: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2813=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2813[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2814; _temp2814.tag= Cyc_Stmt_tok; _temp2814.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2814;}); _temp2813;}); break; case 273:
_LL2812: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2816=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2816[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2817; _temp2817.tag= Cyc_Stmt_tok; _temp2817.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2817;}); _temp2816;}); break; case 274: _LL2815: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2819=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2819[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2820; _temp2820.tag= Cyc_Stmt_tok; _temp2820.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2820;}); _temp2819;}); break; case 275:
_LL2818: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2822=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2822[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2823; _temp2823.tag= Cyc_Stmt_tok; _temp2823.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2823;}); _temp2822;}); break; case 276: _LL2821: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2825=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2825[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2826; _temp2826.tag= Cyc_Stmt_tok; _temp2826.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2826;}); _temp2825;}); break; case 277:
_LL2824: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2828=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2828[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2829; _temp2829.tag= Cyc_Stmt_tok; _temp2829.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2829;}); _temp2828;}); break; case 278:
_LL2827: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2831=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2831[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2832; _temp2832.tag= Cyc_Stmt_tok; _temp2832.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2832;}); _temp2831;}); break; case 279: _LL2830: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2834=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2834[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2835; _temp2835.tag= Cyc_Pattern_tok; _temp2835.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2835;});
_temp2834;}); break; case 280: _LL2833: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 281:
_LL2836: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2838=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2838[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2839; _temp2839.tag= Cyc_Pattern_tok; _temp2839.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2840=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2840[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2841; _temp2841.tag= Cyc_Absyn_Int_p; _temp2841.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2841.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2841;}); _temp2840;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2839;});
_temp2838;}); break; case 282: _LL2837: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2843=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2843[ 0]=({ struct Cyc_Pattern_tok_struct _temp2844; _temp2844.tag= Cyc_Pattern_tok;
_temp2844.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2845=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2845[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2846; _temp2846.tag= Cyc_Absyn_Int_p;
_temp2846.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2846.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2846;});
_temp2845;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2844;}); _temp2843;}); break; case 283:
_LL2842: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2848=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2848[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2849; _temp2849.tag= Cyc_Pattern_tok; _temp2849.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2850=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2850[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2851; _temp2851.tag= Cyc_Absyn_Float_p; _temp2851.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2851;}); _temp2850;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2849;});
_temp2848;}); break; case 284: _LL2847: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2853=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2853[ 0]=({ struct Cyc_Pattern_tok_struct _temp2854; _temp2854.tag= Cyc_Pattern_tok;
_temp2854.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2855=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2855[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2856; _temp2856.tag= Cyc_Absyn_Char_p; _temp2856.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2856;});
_temp2855;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2854;}); _temp2853;}); break; case 285:
_LL2852: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2858=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2858[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2859; _temp2859.tag= Cyc_Pattern_tok; _temp2859.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2859;});
_temp2858;}); break; case 286: _LL2857: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2861=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2861[ 0]=({ struct Cyc_Pattern_tok_struct _temp2862; _temp2862.tag= Cyc_Pattern_tok;
_temp2862.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2863=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2863[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2864; _temp2864.tag=
Cyc_Absyn_UnknownId_p; _temp2864.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2864;}); _temp2863;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2862;}); _temp2861;}); break; case 287:
_LL2860: { struct Cyc_List_List* _temp2866=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2867=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2867[ 0]=({ struct Cyc_Pattern_tok_struct _temp2868; _temp2868.tag= Cyc_Pattern_tok;
_temp2868.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2869=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2869[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2870; _temp2870.tag= Cyc_Absyn_UnknownCall_p; _temp2870.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2870.f2=
_temp2866; _temp2870.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2870;});
_temp2869;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2868;}); _temp2867;}); break;} case
288: _LL2865: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2872=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2872[ 0]=({ struct Cyc_Pattern_tok_struct _temp2873; _temp2873.tag= Cyc_Pattern_tok;
_temp2873.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2874=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2874[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2875; _temp2875.tag= Cyc_Absyn_Tuple_p;
_temp2875.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2875;}); _temp2874;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2873;}); _temp2872;}); break; case 289: _LL2871: { struct Cyc_List_List*
_temp2877=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2878=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2878[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2879; _temp2879.tag= Cyc_Pattern_tok; _temp2879.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2880=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2880[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2881; _temp2881.tag= Cyc_Absyn_UnknownFields_p;
_temp2881.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2881.f2= _temp2877; _temp2881.f3= 0; _temp2881;});
_temp2880;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2879;}); _temp2878;}); break;} case
290: _LL2876: { struct Cyc_List_List* _temp2883=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2884=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2884[ 0]=({ struct Cyc_Pattern_tok_struct _temp2885; _temp2885.tag= Cyc_Pattern_tok;
_temp2885.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2886=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2886[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2887; _temp2887.tag= Cyc_Absyn_UnknownFields_p; _temp2887.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2887.f2=
_temp2883; _temp2887.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2887;});
_temp2886;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2885;}); _temp2884;}); break;} case
291: _LL2882: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2889=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2889[ 0]=({ struct Cyc_Pattern_tok_struct _temp2890; _temp2890.tag= Cyc_Pattern_tok;
_temp2890.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2891=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2891[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2892; _temp2892.tag= Cyc_Absyn_Pointer_p;
_temp2892.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2892;}); _temp2891;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2890;});
_temp2889;}); break; case 292: _LL2888: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2894=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2894[ 0]=({ struct Cyc_Pattern_tok_struct _temp2895; _temp2895.tag= Cyc_Pattern_tok;
_temp2895.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2896=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2896[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2897; _temp2897.tag=
Cyc_Absyn_Reference_p; _temp2897.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2898=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2898->f1=(
void*) Cyc_Absyn_Loc_n; _temp2898->f2=({ struct _tagged_arr* _temp2899=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2899[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2899;});
_temp2898;}),( void*) Cyc_Absyn_VoidType, 0); _temp2897;}); _temp2896;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2895;}); _temp2894;}); break; case 293: _LL2893: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2901=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2901[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2902; _temp2902.tag= Cyc_PatternList_tok;
_temp2902.f1= 0; _temp2902;}); _temp2901;}); break; case 294: _LL2900: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2904=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2904[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2905; _temp2905.tag= Cyc_PatternList_tok;
_temp2905.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2905;}); _temp2904;}); break; case 295: _LL2903: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2907=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2907[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2908; _temp2908.tag= Cyc_PatternList_tok;
_temp2908.f1=({ struct Cyc_List_List* _temp2909=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2909->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2909->tl= 0;
_temp2909;}); _temp2908;}); _temp2907;}); break; case 296: _LL2906: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2911=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2911[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2912; _temp2912.tag= Cyc_PatternList_tok;
_temp2912.f1=({ struct Cyc_List_List* _temp2913=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2913->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2913->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2913;}); _temp2912;}); _temp2911;}); break; case 297:
_LL2910: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2915=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2915[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2916; _temp2916.tag=
Cyc_FieldPattern_tok; _temp2916.f1=({ struct _tuple14* _temp2917=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2917->f1= 0; _temp2917->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2917;}); _temp2916;}); _temp2915;}); break; case 298:
_LL2914: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2919=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2919[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2920; _temp2920.tag=
Cyc_FieldPattern_tok; _temp2920.f1=({ struct _tuple14* _temp2921=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2921->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2921->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2921;}); _temp2920;}); _temp2919;}); break; case 299:
_LL2918: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2923=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2923[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2924; _temp2924.tag=
Cyc_FieldPatternList_tok; _temp2924.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2924;});
_temp2923;}); break; case 300: _LL2922: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2926=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2926[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2927; _temp2927.tag= Cyc_FieldPatternList_tok; _temp2927.f1=({ struct Cyc_List_List*
_temp2928=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2928->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2928->tl= 0;
_temp2928;}); _temp2927;}); _temp2926;}); break; case 301: _LL2925: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2930=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2930[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2931; _temp2931.tag= Cyc_FieldPatternList_tok;
_temp2931.f1=({ struct Cyc_List_List* _temp2932=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2932->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2932->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2932;}); _temp2931;}); _temp2930;}); break; case 302:
_LL2929: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 303: _LL2933: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2935=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2935[ 0]=({ struct Cyc_Exp_tok_struct _temp2936; _temp2936.tag= Cyc_Exp_tok;
_temp2936.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2936;}); _temp2935;}); break; case 304: _LL2934: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 305: _LL2937:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2939=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2939[ 0]=({ struct Cyc_Exp_tok_struct
_temp2940; _temp2940.tag= Cyc_Exp_tok; _temp2940.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2940;}); _temp2939;}); break; case 306: _LL2938: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2942=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2942[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2943; _temp2943.tag= Cyc_Primopopt_tok; _temp2943.f1=
0; _temp2943;}); _temp2942;}); break; case 307: _LL2941: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2945=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2945[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2946; _temp2946.tag= Cyc_Primopopt_tok; _temp2946.f1=({
struct Cyc_Core_Opt* _temp2947=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2947->v=( void*)(( void*) Cyc_Absyn_Times); _temp2947;});
_temp2946;}); _temp2945;}); break; case 308: _LL2944: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2949=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2949[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2950; _temp2950.tag= Cyc_Primopopt_tok; _temp2950.f1=({
struct Cyc_Core_Opt* _temp2951=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2951->v=( void*)(( void*) Cyc_Absyn_Div); _temp2951;});
_temp2950;}); _temp2949;}); break; case 309: _LL2948: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2953=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2953[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2954; _temp2954.tag= Cyc_Primopopt_tok; _temp2954.f1=({
struct Cyc_Core_Opt* _temp2955=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2955->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2955;});
_temp2954;}); _temp2953;}); break; case 310: _LL2952: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2957=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2957[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2958; _temp2958.tag= Cyc_Primopopt_tok; _temp2958.f1=({
struct Cyc_Core_Opt* _temp2959=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2959->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2959;});
_temp2958;}); _temp2957;}); break; case 311: _LL2956: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2961=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2961[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2962; _temp2962.tag= Cyc_Primopopt_tok; _temp2962.f1=({
struct Cyc_Core_Opt* _temp2963=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2963->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2963;});
_temp2962;}); _temp2961;}); break; case 312: _LL2960: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2965=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2965[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2966; _temp2966.tag= Cyc_Primopopt_tok; _temp2966.f1=({
struct Cyc_Core_Opt* _temp2967=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2967->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2967;});
_temp2966;}); _temp2965;}); break; case 313: _LL2964: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2969=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2969[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2970; _temp2970.tag= Cyc_Primopopt_tok; _temp2970.f1=({
struct Cyc_Core_Opt* _temp2971=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2971->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2971;});
_temp2970;}); _temp2969;}); break; case 314: _LL2968: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2973=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2973[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2974; _temp2974.tag= Cyc_Primopopt_tok; _temp2974.f1=({
struct Cyc_Core_Opt* _temp2975=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2975->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2975;});
_temp2974;}); _temp2973;}); break; case 315: _LL2972: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2977=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2977[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2978; _temp2978.tag= Cyc_Primopopt_tok; _temp2978.f1=({
struct Cyc_Core_Opt* _temp2979=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2979->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2979;});
_temp2978;}); _temp2977;}); break; case 316: _LL2976: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2981=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2981[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2982; _temp2982.tag= Cyc_Primopopt_tok; _temp2982.f1=({
struct Cyc_Core_Opt* _temp2983=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2983->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2983;});
_temp2982;}); _temp2981;}); break; case 317: _LL2980: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 318: _LL2984:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2986=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2986[ 0]=({ struct Cyc_Exp_tok_struct
_temp2987; _temp2987.tag= Cyc_Exp_tok; _temp2987.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2987;});
_temp2986;}); break; case 319: _LL2985: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2989=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2989[ 0]=({ struct Cyc_Exp_tok_struct _temp2990; _temp2990.tag= Cyc_Exp_tok;
_temp2990.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2990;}); _temp2989;}); break; case 320: _LL2988: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2992=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2992[ 0]=({ struct Cyc_Exp_tok_struct
_temp2993; _temp2993.tag= Cyc_Exp_tok; _temp2993.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2993;}); _temp2992;}); break; case 321: _LL2991: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2995=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2995[ 0]=({ struct Cyc_Exp_tok_struct
_temp2996; _temp2996.tag= Cyc_Exp_tok; _temp2996.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2996;}); _temp2995;}); break; case 322: _LL2994: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2998=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2998[ 0]=({ struct Cyc_Exp_tok_struct
_temp2999; _temp2999.tag= Cyc_Exp_tok; _temp2999.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2999;});
_temp2998;}); break; case 323: _LL2997: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3001=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3001[ 0]=({ struct Cyc_Exp_tok_struct _temp3002; _temp3002.tag= Cyc_Exp_tok;
_temp3002.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3002;}); _temp3001;}); break; case 324: _LL3000: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 325: _LL3003:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
326: _LL3004: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3006=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3006[ 0]=({ struct Cyc_Exp_tok_struct
_temp3007; _temp3007.tag= Cyc_Exp_tok; _temp3007.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3007;}); _temp3006;}); break; case 327: _LL3005: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 328: _LL3008:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3010=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3010[ 0]=({ struct Cyc_Exp_tok_struct
_temp3011; _temp3011.tag= Cyc_Exp_tok; _temp3011.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3011;}); _temp3010;}); break; case 329: _LL3009: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL3012:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3014=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3014[ 0]=({ struct Cyc_Exp_tok_struct
_temp3015; _temp3015.tag= Cyc_Exp_tok; _temp3015.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3015;});
_temp3014;}); break; case 331: _LL3013: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL3016:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3018=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3018[ 0]=({ struct Cyc_Exp_tok_struct
_temp3019; _temp3019.tag= Cyc_Exp_tok; _temp3019.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3019;});
_temp3018;}); break; case 333: _LL3017: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3020:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3022=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3022[ 0]=({ struct Cyc_Exp_tok_struct
_temp3023; _temp3023.tag= Cyc_Exp_tok; _temp3023.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3023;});
_temp3022;}); break; case 335: _LL3021: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3024:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3026=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3026[ 0]=({ struct Cyc_Exp_tok_struct
_temp3027; _temp3027.tag= Cyc_Exp_tok; _temp3027.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3027;}); _temp3026;}); break; case 337: _LL3025: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3029=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3029[ 0]=({ struct Cyc_Exp_tok_struct
_temp3030; _temp3030.tag= Cyc_Exp_tok; _temp3030.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3030;}); _temp3029;}); break; case 338: _LL3028: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 339: _LL3031:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3034;}); _temp3033;}); break; case 340: _LL3032: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3036=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3036[ 0]=({ struct Cyc_Exp_tok_struct
_temp3037; _temp3037.tag= Cyc_Exp_tok; _temp3037.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3037;}); _temp3036;}); break; case 341: _LL3035: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3039=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3039[ 0]=({ struct Cyc_Exp_tok_struct
_temp3040; _temp3040.tag= Cyc_Exp_tok; _temp3040.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3040;}); _temp3039;}); break; case 342: _LL3038: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3042=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3042[ 0]=({ struct Cyc_Exp_tok_struct
_temp3043; _temp3043.tag= Cyc_Exp_tok; _temp3043.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3043;}); _temp3042;}); break; case 343: _LL3041: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 344: _LL3044:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3046=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3046[ 0]=({ struct Cyc_Exp_tok_struct
_temp3047; _temp3047.tag= Cyc_Exp_tok; _temp3047.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3047;}); _temp3046;}); break; case 345: _LL3045: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3049=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3049[ 0]=({ struct Cyc_Exp_tok_struct
_temp3050; _temp3050.tag= Cyc_Exp_tok; _temp3050.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3050;}); _temp3049;}); break; case 346: _LL3048: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 347: _LL3051:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3053=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3053[ 0]=({ struct Cyc_Exp_tok_struct
_temp3054; _temp3054.tag= Cyc_Exp_tok; _temp3054.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3054;});
_temp3053;}); break; case 348: _LL3052: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3056=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3056[ 0]=({ struct Cyc_Exp_tok_struct _temp3057; _temp3057.tag= Cyc_Exp_tok;
_temp3057.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3057;}); _temp3056;}); break; case 349: _LL3055: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 350: _LL3058:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3060=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3060[ 0]=({ struct Cyc_Exp_tok_struct
_temp3061; _temp3061.tag= Cyc_Exp_tok; _temp3061.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3061;});
_temp3060;}); break; case 351: _LL3059: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3063=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3063[ 0]=({ struct Cyc_Exp_tok_struct _temp3064; _temp3064.tag= Cyc_Exp_tok;
_temp3064.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3064;}); _temp3063;}); break; case 352: _LL3062: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3066=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3066[ 0]=({ struct Cyc_Exp_tok_struct
_temp3067; _temp3067.tag= Cyc_Exp_tok; _temp3067.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3067;});
_temp3066;}); break; case 353: _LL3065: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 354: _LL3068:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3071;}); _temp3070;}); break; case 355: _LL3069: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3072:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3074=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3074[ 0]=({ struct Cyc_Exp_tok_struct
_temp3075; _temp3075.tag= Cyc_Exp_tok; _temp3075.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3075;}); _temp3074;}); break; case 357: _LL3073: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3078;}); _temp3077;}); break; case 358: _LL3076: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3080=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3080[ 0]=({ struct Cyc_Exp_tok_struct
_temp3081; _temp3081.tag= Cyc_Exp_tok; _temp3081.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3081;}); _temp3080;}); break; case 359: _LL3079: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3083=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3083[ 0]=({ struct Cyc_Exp_tok_struct
_temp3084; _temp3084.tag= Cyc_Exp_tok; _temp3084.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3084;}); _temp3083;}); break; case 360: _LL3082: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 361: _LL3085:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3087=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3087[ 0]=({ struct Cyc_Exp_tok_struct
_temp3088; _temp3088.tag= Cyc_Exp_tok; _temp3088.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3088;}); _temp3087;}); break; case 362: _LL3086: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3090=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3090[ 0]=({ struct Cyc_Exp_tok_struct
_temp3091; _temp3091.tag= Cyc_Exp_tok; _temp3091.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3091;});
_temp3090;}); break; case 363: _LL3089: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3093=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3093[ 0]=({ struct Cyc_Exp_tok_struct _temp3094; _temp3094.tag= Cyc_Exp_tok;
_temp3094.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3094;}); _temp3093;}); break; case 364: _LL3092: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3098=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3098[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3098;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3097;}); _temp3096;}); break; case 365:
_LL3095: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3100=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3100[ 0]=({ struct Cyc_Exp_tok_struct
_temp3101; _temp3101.tag= Cyc_Exp_tok; _temp3101.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3101;}); _temp3100;}); break; case 366: _LL3099: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3103=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3103[ 0]=({ struct Cyc_Exp_tok_struct
_temp3104; _temp3104.tag= Cyc_Exp_tok; _temp3104.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3105=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3105[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3106; _temp3106.tag= Cyc_Absyn_Malloc_e;
_temp3106.f1= 0; _temp3106.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3106;}); _temp3105;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3104;});
_temp3103;}); break; case 367: _LL3102: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3108=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3108[ 0]=({ struct Cyc_Exp_tok_struct _temp3109; _temp3109.tag= Cyc_Exp_tok;
_temp3109.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3110=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3110[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3111; _temp3111.tag= Cyc_Absyn_Malloc_e;
_temp3111.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3111.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3111;}); _temp3110;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3109;});
_temp3108;}); break; case 368: _LL3107: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3113=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3113[ 0]=({ struct Cyc_Primop_tok_struct _temp3114; _temp3114.tag= Cyc_Primop_tok;
_temp3114.f1=( void*)(( void*) Cyc_Absyn_Printf); _temp3114;}); _temp3113;});
break; case 369: _LL3112: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3116=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3116[ 0]=({ struct Cyc_Primop_tok_struct _temp3117; _temp3117.tag= Cyc_Primop_tok;
_temp3117.f1=( void*)(( void*) Cyc_Absyn_Fprintf); _temp3117;}); _temp3116;});
break; case 370: _LL3115: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3119=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3119[ 0]=({ struct Cyc_Primop_tok_struct _temp3120; _temp3120.tag= Cyc_Primop_tok;
_temp3120.f1=( void*)(( void*) Cyc_Absyn_Xprintf); _temp3120;}); _temp3119;});
break; case 371: _LL3118: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3122=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3122[ 0]=({ struct Cyc_Primop_tok_struct _temp3123; _temp3123.tag= Cyc_Primop_tok;
_temp3123.f1=( void*)(( void*) Cyc_Absyn_Scanf); _temp3123;}); _temp3122;});
break; case 372: _LL3121: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3125=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3125[ 0]=({ struct Cyc_Primop_tok_struct _temp3126; _temp3126.tag= Cyc_Primop_tok;
_temp3126.f1=( void*)(( void*) Cyc_Absyn_Fscanf); _temp3126;}); _temp3125;});
break; case 373: _LL3124: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3128=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3128[ 0]=({ struct Cyc_Primop_tok_struct _temp3129; _temp3129.tag= Cyc_Primop_tok;
_temp3129.f1=( void*)(( void*) Cyc_Absyn_Sscanf); _temp3129;}); _temp3128;});
break; case 374: _LL3127: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3131=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3131[ 0]=({ struct Cyc_Primop_tok_struct _temp3132; _temp3132.tag= Cyc_Primop_tok;
_temp3132.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3132;}); _temp3131;});
break; case 375: _LL3130: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3134=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3134[ 0]=({ struct Cyc_Primop_tok_struct _temp3135; _temp3135.tag= Cyc_Primop_tok;
_temp3135.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3135;}); _temp3134;});
break; case 376: _LL3133: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3137=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3137[ 0]=({ struct Cyc_Primop_tok_struct _temp3138; _temp3138.tag= Cyc_Primop_tok;
_temp3138.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3138;}); _temp3137;});
break; case 377: _LL3136: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 378: _LL3139: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3141=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3141[ 0]=({ struct Cyc_Exp_tok_struct _temp3142; _temp3142.tag= Cyc_Exp_tok;
_temp3142.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3142;}); _temp3141;}); break; case 379: _LL3140: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3144=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3144[ 0]=({ struct Cyc_Exp_tok_struct
_temp3145; _temp3145.tag= Cyc_Exp_tok; _temp3145.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3145;}); _temp3144;}); break; case 380:
_LL3143: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3147=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3147[ 0]=({ struct Cyc_Exp_tok_struct
_temp3148; _temp3148.tag= Cyc_Exp_tok; _temp3148.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3148;});
_temp3147;}); break; case 381: _LL3146: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3150=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3150[ 0]=({ struct Cyc_Exp_tok_struct _temp3151; _temp3151.tag= Cyc_Exp_tok;
_temp3151.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3152=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3152[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3152;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3151;}); _temp3150;}); break; case 382: _LL3149: { struct _tuple1* q= Cyc_yyget_QualId_tok(
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
_temp3169;}); _temp3168;}); break;} case 383: _LL3153: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3171=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3171[ 0]=({ struct Cyc_Exp_tok_struct
_temp3172; _temp3172.tag= Cyc_Exp_tok; _temp3172.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),({ struct _tagged_arr* _temp3173=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp3173[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3173;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3172;}); _temp3171;}); break; case 384: _LL3170: { struct _tuple1* q= Cyc_yyget_QualId_tok(
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
_temp3190;}); _temp3189;}); break;} case 385: _LL3174: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3192=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3192[ 0]=({ struct Cyc_Exp_tok_struct
_temp3193; _temp3193.tag= Cyc_Exp_tok; _temp3193.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3193;}); _temp3192;}); break; case 386: _LL3191: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3195=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3195[ 0]=({ struct Cyc_Exp_tok_struct
_temp3196; _temp3196.tag= Cyc_Exp_tok; _temp3196.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3196;}); _temp3195;}); break; case 387: _LL3194: yyval=( void*)({ struct
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
10000u, yylsp_offset)]).last_line)); _temp3199;}); _temp3198;}); break; case 388:
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
10000u, yylsp_offset)]).last_line)); _temp3204;}); _temp3203;}); break; case 389:
_LL3202: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3208=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3208[ 0]=({ struct Cyc_Exp_tok_struct
_temp3209; _temp3209.tag= Cyc_Exp_tok; _temp3209.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3210=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3210[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3211; _temp3211.tag= Cyc_Absyn_Fill_e; _temp3211.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3211;});
_temp3210;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3209;}); _temp3208;}); break; case 390:
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
_temp3214;}); _temp3213;}); break; case 391: _LL3212: yyval=( void*)({ struct
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
_temp3218;}); break; case 392: _LL3217: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 393: _LL3222:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3224=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3224[ 0]=({ struct Cyc_Exp_tok_struct
_temp3225; _temp3225.tag= Cyc_Exp_tok; _temp3225.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3225;});
_temp3224;}); break; case 394: _LL3223: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 395:
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
_temp3228;}); break; case 396: _LL3227: yyval=( void*)({ struct Cyc_Exp_tok_struct*
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
_temp3234;}); _temp3233;}); break; case 397: _LL3232: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3238=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3238[ 0]=({ struct Cyc_Exp_tok_struct
_temp3239; _temp3239.tag= Cyc_Exp_tok; _temp3239.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3239;}); _temp3238;}); break; case 398: _LL3237: yyval=( void*)({ struct
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
_temp3241;}); break; case 399: _LL3240: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3246=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3246[ 0]=({ struct Cyc_Exp_tok_struct _temp3247; _temp3247.tag= Cyc_Exp_tok;
_temp3247.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3247;}); _temp3246;}); break; case 400: _LL3245: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3249=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3249[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3250; _temp3250.tag= Cyc_ExpList_tok; _temp3250.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3250;});
_temp3249;}); break; case 401: _LL3248: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3252=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3252[ 0]=({ struct Cyc_ExpList_tok_struct _temp3253; _temp3253.tag= Cyc_ExpList_tok;
_temp3253.f1=({ struct Cyc_List_List* _temp3254=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3254->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3254->tl= 0;
_temp3254;}); _temp3253;}); _temp3252;}); break; case 402: _LL3251: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3256=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3256[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3257; _temp3257.tag= Cyc_ExpList_tok; _temp3257.f1=({ struct Cyc_List_List*
_temp3258=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3258->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3258->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3258;});
_temp3257;}); _temp3256;}); break; case 403: _LL3255: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3260=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3260[ 0]=({ struct Cyc_Exp_tok_struct
_temp3261; _temp3261.tag= Cyc_Exp_tok; _temp3261.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3261;});
_temp3260;}); break; case 404: _LL3259: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3263=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3263[ 0]=({ struct Cyc_Exp_tok_struct _temp3264; _temp3264.tag= Cyc_Exp_tok;
_temp3264.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3264;});
_temp3263;}); break; case 405: _LL3262: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3266=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3266[ 0]=({ struct Cyc_Exp_tok_struct _temp3267; _temp3267.tag= Cyc_Exp_tok;
_temp3267.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3267;});
_temp3266;}); break; case 406: _LL3265: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3269=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3269[ 0]=({ struct Cyc_Exp_tok_struct _temp3270; _temp3270.tag= Cyc_Exp_tok;
_temp3270.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3270;});
_temp3269;}); break; case 407: _LL3268: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3272=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3272[ 0]=({ struct Cyc_QualId_tok_struct _temp3273; _temp3273.tag= Cyc_QualId_tok;
_temp3273.f1=({ struct _tuple1* _temp3274=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3274->f1= Cyc_Absyn_rel_ns_null; _temp3274->f2=({ struct
_tagged_arr* _temp3275=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3275[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3275;}); _temp3274;});
_temp3273;}); _temp3272;}); break; case 408: _LL3271: yyval= yyvs[
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
unsigned int _temp3278=( unsigned int)( sze + 15); unsigned char* _temp3279=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3278); struct
_tagged_arr _temp3281= _tag_arr( _temp3279, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3280= _temp3278; unsigned int i;
for( i= 0; i < _temp3280; i ++){ _temp3279[ i]='\000';}}; _temp3281;}); Cyc_String_strcpy(
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
_temp3331.f2; goto _LL3308;} else{ goto _LL3309;} _LL3309: goto _LL3310; _LL3294:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"ok"); goto _LL3292; _LL3296: fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"%d", _temp3314); goto _LL3292; _LL3298:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%c",( int) _temp3316); goto _LL3292;
_LL3300: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%ds",( int) _temp3318);
goto _LL3292; _LL3302:({ struct _tagged_arr _temp3336= _temp3320; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _get_arr_size( _temp3336, 1u),
_temp3336.curr);}); goto _LL3292; _LL3304: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"null");
goto _LL3292; _LL3306:({ struct _tagged_arr _temp3337=* _temp3327; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _get_arr_size( _temp3337, 1u),
_temp3337.curr);}); goto _LL3292; _LL3308: { struct Cyc_List_List* _temp3338= 0;{
void* _temp3339= _temp3334; struct Cyc_List_List* _temp3347; struct Cyc_List_List*
_temp3349; _LL3341: if(( unsigned int) _temp3339 > 1u?*(( int*) _temp3339) ==
Cyc_Absyn_Rel_n: 0){ _LL3348: _temp3347=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3339)->f1; goto _LL3342;} else{ goto _LL3343;} _LL3343: if(( unsigned int)
_temp3339 > 1u?*(( int*) _temp3339) == Cyc_Absyn_Abs_n: 0){ _LL3350: _temp3349=((
struct Cyc_Absyn_Abs_n_struct*) _temp3339)->f1; goto _LL3344;} else{ goto
_LL3345;} _LL3345: if( _temp3339 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3346;}
else{ goto _LL3340;} _LL3342: _temp3338= _temp3347; goto _LL3340; _LL3344:
_temp3338= _temp3349; goto _LL3340; _LL3346: goto _LL3340; _LL3340:;} for( 0;
_temp3338 != 0; _temp3338=(( struct Cyc_List_List*) _check_null( _temp3338))->tl){({
struct _tagged_arr _temp3351=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3338))->hd); fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_get_arr_size( _temp3351, 1u), _temp3351.curr);});}({ struct _tagged_arr
_temp3352=* _temp3332; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_get_arr_size( _temp3352, 1u), _temp3352.curr);}); goto _LL3292;} _LL3310:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"?"); goto _LL3292; _LL3292:;} struct
Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result=
0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3353=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3353->v=( void*) Cyc_Lexing_from_file(
f); _temp3353;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}